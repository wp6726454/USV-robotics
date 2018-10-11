/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_cp_mem.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation for NRT / PI / HIF  CP memory management using MEM3       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20003
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include "eps_sys.h"
#include "eps_cp_mem.h"
#include "eps_mem.h"
#include "eps_trc.h"   // Tracing
#include "mem3if.h"    // MEM3 allocator

#include "psi_cfg.h"   // PSI-Mtypes
#include "psi_usr.h"
#include "psi_sys.h"

/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/

#define EPS_MAX_BOARD_NR PSI_CFG_MAX_HD_CNT

#define EPS_MEM_USAGE_MEM3_FFT2BOUND    0
#define EPS_MEM_USAGE_MEM3_UNIVERSAL3   1
#define EPS_MEM_USAGE_HIF_INTERNAL      2
#define EPS_MEM_USAGE_HIF_RING_POOL     3 // only works for EPS_HIF_SER_MEM_USAGE 

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
#include "hif_mem_malloc.h"
#define EPS_NRT_MEM_USAGE EPS_MEM_USAGE_HIF_INTERNAL
//#define EPS_NRT_MEM_USAGE EPS_MEM_USAGE_MEM3_FFT2BOUND
#else
#define EPS_NRT_MEM_USAGE EPS_MEM_USAGE_MEM3_FFT2BOUND
#endif

#if (EPS_PLF == EPS_PLF_ADONIS_X86 || EPS_PLF == EPS_PLF_WINDOWS_X86 || EPS_PLF == EPS_PLF_LINUX_X86)
//#define EPS_HIF_SER_MEM_USAGE EPS_MEM_USAGE_MEM3_FFT2BOUND
#include "hif_mem_ring.h"
#define EPS_HIF_SER_MEM_USAGE EPS_MEM_USAGE_HIF_RING_POOL
//#include "hif_mem_malloc.h"
//#define EPS_HIF_SER_MEM_USAGE EPS_MEM_USAGE_HIF_INTERNAL
#else
#include "hif_mem_ring.h"
#define EPS_HIF_SER_MEM_USAGE EPS_MEM_USAGE_HIF_RING_POOL
//#include "hif_mem_malloc.h"
//#define EPS_HIF_SER_MEM_USAGE EPS_MEM_USAGE_HIF_INTERNAL
//#define EPS_HIF_SER_MEM_USAGE EPS_MEM_USAGE_MEM3_FFT2BOUND
#endif

/*---------------------------------------------------------------------------*/
/* Types                                                                     */
/*---------------------------------------------------------------------------*/

typedef enum eps_cp_pool_type_enum {    // pool types for each HD
	POOL_UNKNOWN               = 0x00,  // reserved for invalid
	EPS_RESERVED_LOCAL_MEM     = 0x01,  // reserved pool id for local mem (see eps_mem.c)
	EPS_RESERVED_FAST_MEM      = 0x02,  // reserved pool id for fast emm (see eps_mem.c)
	HIF_POOL                   = 0x03,  // HIF pool 
	NRT_POOL                   = 0x04,  // NRT pool
	PI_POOL                    = 0x05,  // PI pool
	NRT_POOL_DMACW_DEFAULT     = 0x06,  // NRT pool
	NRT_POOL_RX_DEFAULT        = 0x07,  // NRT pool
	NRT_POOL_TX_DEFAULT        = 0x08,  // NRT pool
	NRT_POOL_TX_LOW_FRAG       = 0x09,  // NRT pool
	NRT_POOL_TX_MID_FRAG       = 0x0A,  // NRT pool
	NRT_POOL_TX_HIGH_FRAG      = 0x0B,  // NRT pool
	NRT_POOL_TX_MGMTLOW_FRAG   = 0x0C,  // NRT pool
	NRT_POOL_TX_MGMTHIGH_FRAG  = 0x0D,  // NRT pool
	POOL_NR_MAX
} EPS_CP_POOL_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct eps_cp_mem_pool_tag
{
	LSA_UINT8* base_ptr;           // Base address of memory block
	LSA_UINT32 size;               // Size of memory block [bytes]
	LSA_UINT8* help_mem_ptr ;      // pointer to helper memory for data pool
	LSA_INT    pool_nr;            // Creation number for data pool or alias nr
	LSA_INT    alias_nr;           // Alias number for data pool (used only for Fft2Bound pools)
} EPS_CP_MEM_POOL_TYPE, *EPS_CP_MEM_POOL_PTR_TYPE;

/*---------------------------------------------------------------------------------*/
/* Note: board_nr = 0 is used for LD pools (only HIF POOL is allowed with this nr) */
typedef struct eps_cp_mem_instance_tag  // type for CP memory instance 
{
	LSA_BOOL             is_running;                    // Initialized yes/no
	EPS_CP_MEM_POOL_TYPE hif_mem[EPS_MAX_BOARD_NR+1];   // HIF memory pools (HIF buffer)
	EPS_CP_MEM_POOL_TYPE nrt_mem[EPS_MAX_BOARD_NR+1];   // NRT memory pools (NRT buffer)
	EPS_CP_MEM_POOL_TYPE nrt_dmacw_default_mem[EPS_MAX_BOARD_NR+1];    // NRT memory pools (NRT buffer for DMACWs)
	EPS_CP_MEM_POOL_TYPE nrt_tx_default_mem[EPS_MAX_BOARD_NR+1];       // NRT memory pools (NRT buffer for TX)
	EPS_CP_MEM_POOL_TYPE nrt_rx_default_mem[EPS_MAX_BOARD_NR+1];       // NRT memory pools (NRT buffer for RX)
	EPS_CP_MEM_POOL_TYPE nrt_tx_low_frag_mem[EPS_MAX_BOARD_NR+1];      // NRT memory pools (NRT buffer for TX fragmentation prio low, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_mid_frag_mem[EPS_MAX_BOARD_NR+1];      // NRT memory pools (NRT buffer for TX fragmentation prio mid, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_high_frag_mem[EPS_MAX_BOARD_NR+1];     // NRT memory pools (NRT buffer for TX fragmentation prio high, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_mgmtlow_frag_mem[EPS_MAX_BOARD_NR+1];  // NRT memory pools (NRT buffer for TX fragmentation prio MGMTLow, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_mgmthigh_frag_mem[EPS_MAX_BOARD_NR+1]; // NRT memory pools (NRT buffer for TX fragmentation prio MGMTHigh, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE pi_mem[EPS_MAX_BOARD_NR+1];                   // ProcessImage memory pool (Provider/Consumer)
} EPS_CP_MEM_INSTANCE_TYPE, *EPS_CP_MEM_INSTANCE_PTR_TYPE;

static EPS_CP_MEM_INSTANCE_TYPE  eps_pools = {0};  // the CP mem instance for max CPs

static LSA_UINT8* eps_cp_mem_nrt_alloc_by_poolptr( LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id );
static LSA_VOID eps_cp_mem_nrt_free_by_poolptr( LSA_UINT8* ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id );

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static LSA_VOID eps_cp_mem_init_pool_struct(EPS_CP_MEM_POOL_PTR_TYPE pCpMem)
{
	pCpMem->base_ptr     = 0;
	pCpMem->size         = 0;
	pCpMem->help_mem_ptr = 0;
	pCpMem->pool_nr      = POOL_UNKNOWN;
	pCpMem->alias_nr     = POOL_UNKNOWN;
}

LSA_VOID eps_cp_mem_init( LSA_VOID )
{
	LSA_UINT16 idx;

	for ( idx = 0; idx <= EPS_MAX_BOARD_NR; idx++ )
	{
		// Initialize the instance pool information HIF/NRT/PI for each possible PN Board
		eps_cp_mem_init_pool_struct(&eps_pools.hif_mem[idx]);

		// Initialize the instance pool information NRT
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_dmacw_default_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_tx_default_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_rx_default_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_tx_low_frag_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_tx_mid_frag_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_tx_high_frag_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_tx_mgmtlow_frag_mem[idx]);
		eps_cp_mem_init_pool_struct(&eps_pools.nrt_tx_mgmthigh_frag_mem[idx]);

		// Initialize the instance pool information PI
		eps_cp_mem_init_pool_struct(&eps_pools.pi_mem[idx]);
	}

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_init(): Mem pool init done, max boards(%u)", 
		EPS_MAX_BOARD_NR );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_undo_init( LSA_VOID )
{
	LSA_UINT16 idx;

	for ( idx = 0 ; idx <= EPS_MAX_BOARD_NR ; idx++ )  // Check for all CPs
	{
		// All pools freed ?
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.hif_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_dmacw_default_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_tx_default_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_rx_default_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_tx_low_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_tx_mid_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_tx_high_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_tx_mgmtlow_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.nrt_tx_mgmthigh_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.pi_mem[idx].pool_nr  );
		EPS_ASSERT( POOL_UNKNOWN == eps_pools.pi_mem[idx].alias_nr );
	}

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_undo_init(): Mem pool cleanup done, max boards(%u)", 
		EPS_MAX_BOARD_NR );
}

LSA_VOID eps_mem3_memset(LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len)
{
	eps_memset(dst, val, (LSA_UINT)len);
}

/*----------------------------------------------------------------------------*/
/*  Create the MEM3 pools for PN board (CP) with number                       */
/*----------------------------------------------------------------------------*/

LSA_INT eps_cp_mem_create_nrt_pool( LSA_UINT16 board_nr, LSA_UINT8* base_ptr, LSA_UINT32 size, LSA_UINT8 cp_mem_nrt )
{
	LSA_INT pool_id;
#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	ORDHW granSize, fftThreshold, helperSize;
	eMemType mem3pooltype;

	// create mem3 pool for NRT memory (Shared mem or SDRAM)
	// Note: we use the PN Board number as handle for the NRT pool
	LSA_VOID_PTR_TYPE pHelpMem;
	LSA_VOID_PTR_TYPE pBaseMem;
#endif

	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT((board_nr > 0 ) && (board_nr <= EPS_MAX_BOARD_NR));
	EPS_ASSERT( base_ptr   != 0 );
	EPS_ASSERT( size       >  0 );

	switch (cp_mem_nrt)
	{
		case PSI_HD_NRT_MEM:
		{
			pool_ptr = &eps_pools.nrt_mem[board_nr];

			// the pool number for creation is set based on HD-Nr and type of pool
			// Note: necessary to handle multiple pools of same type in MEM3
			// i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4, 5,6 for HD with HD-NR=1,....
			pool_id = (NRT_POOL + board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_DMACW_DEFAULT:
		{
			pool_ptr = &eps_pools.nrt_dmacw_default_mem[board_nr];
			pool_id = (NRT_POOL_DMACW_DEFAULT + board_nr*POOL_NR_MAX);
			break;
		}
#if (PSI_HD_NRT_MEM!=PSI_HD_NRT_RX_DEFAULT)
		case PSI_HD_NRT_RX_DEFAULT:
		{
			pool_ptr = &eps_pools.nrt_rx_default_mem[board_nr];
			pool_id = (NRT_POOL_RX_DEFAULT + board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_TX_DEFAULT:
		{
			pool_ptr = &eps_pools.nrt_tx_default_mem[board_nr];
			pool_id = (NRT_POOL_TX_DEFAULT + board_nr*POOL_NR_MAX);
			break;
		}
#endif
		case PSI_HD_NRT_TX_LOW_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_low_frag_mem[board_nr];
			pool_id = (NRT_POOL_TX_LOW_FRAG + board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_TX_MID_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_mid_frag_mem[board_nr];
			pool_id = (NRT_POOL_TX_MID_FRAG + board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_TX_HIGH_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_high_frag_mem[board_nr];
			pool_id = (NRT_POOL_TX_HIGH_FRAG + board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_TX_MGMTLOW_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_mgmtlow_frag_mem[board_nr];
			pool_id = (NRT_POOL_TX_MGMTLOW_FRAG + board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_TX_MGMTHIGH_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_mgmthigh_frag_mem[board_nr];
			pool_id = (NRT_POOL_TX_MGMTHIGH_FRAG + board_nr*POOL_NR_MAX);
			break;
		}
		default:
		{
			pool_ptr = (EPS_CP_MEM_POOL_PTR_TYPE)0;
			pool_id = (POOL_UNKNOWN);
			EPS_ASSERT( cp_mem_nrt < PSI_HD_NRT_MAX_MEMPOOLS );
		}
	}

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND)
	mem3pooltype = Fft2Bound;
	granSize     = 0x20; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 0x0;
#elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	mem3pooltype = Universal3;
	granSize     = 1024; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 1024;
#endif
#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	helperSize   = PoolHelperSize3 ( size, granSize, mem3pooltype, LSA_TRUE, 8 );
	pool_ptr->help_mem_ptr = (LSA_UINT8*)eps_mem_alloc(helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_nrt_pool(): boardNr(%u) pool helpPtr(0x%08x) helperSize(%#x)",
		board_nr,
		pool_ptr->help_mem_ptr,
		helperSize );

	// Note: assignment is necessary because ptr are modified in mem3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	SetMemSetFunc( pool_id, (EPS_MEMSET_FCT)eps_memset ); // set standard memset function for MEM3
	SetCheckOverwrite( pool_id, LSA_TRUE );   // enable overwrite check
	SetPoolAlignment( pool_id,  8 );          // 64 bit allignment (see eddi_dev.c)
	pool_ptr->pool_nr = CreatePool ( &pBaseMem, &pHelpMem, pool_ptr->size, helperSize, 
		granSize, fftThreshold, mem3pooltype, pool_id );
	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ));
#elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	pool_ptr->help_mem_ptr = hif_mem_malloc_init(base_ptr, size, NULL);
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
#endif

	EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_nrt_pool(): boardNr(%u) user poolID(%u) MEM3 poolNr(%u) basePtr(0x%08x) baseSize(%#x)",
		board_nr, pool_id, pool_ptr->pool_nr, pool_ptr->base_ptr, pool_ptr->size );

	return ( (LSA_INT)board_nr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_delete_nrt_pool( LSA_UINT16 board_nr, LSA_INT pool_handle, LSA_UINT8 cp_mem_nrt )
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT((board_nr > 0 ) && (board_nr <= EPS_MAX_BOARD_NR));
	EPS_ASSERT( pool_handle == board_nr );

	switch (cp_mem_nrt)
	{
		case PSI_HD_NRT_MEM:
		{
			pool_ptr = &eps_pools.nrt_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_DMACW_DEFAULT:
		{
			pool_ptr = &eps_pools.nrt_dmacw_default_mem[board_nr];
			break;
		}
#if (PSI_HD_NRT_MEM!=PSI_HD_NRT_RX_DEFAULT)
		case PSI_HD_NRT_RX_DEFAULT:
		{
			pool_ptr = &eps_pools.nrt_rx_default_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_TX_DEFAULT:
		{
			pool_ptr = &eps_pools.nrt_tx_default_mem[board_nr];
			break;
		}
#endif
		case PSI_HD_NRT_TX_LOW_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_low_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_TX_MID_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_mid_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_TX_HIGH_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_high_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_TX_MGMTLOW_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_mgmtlow_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_TX_MGMTHIGH_FRAG:
		{
			pool_ptr = &eps_pools.nrt_tx_mgmthigh_frag_mem[board_nr];
			break;
		}
		default:
		{
			pool_ptr = (EPS_CP_MEM_POOL_PTR_TYPE)0;
			EPS_ASSERT( cp_mem_nrt < PSI_HD_NRT_MAX_MEMPOOLS );
		}
	}

#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_nrt_pool(): board_nr(%u), poolNr(%u) helpPtr(0x%08x)", 
		board_nr, pool_ptr->pool_nr, pool_ptr->help_mem_ptr );

	// delete the memory pool, and the helper memory
	if ( BufNumUsedInPool( pool_ptr->pool_nr ) == 0 ) // all memory blocks freed ?
	{
		int result = DeletePool( pool_ptr->pool_nr );
		EPS_ASSERT( result == 0 );

        // check for overwrite
        if ( !VerifyMem3Buf( pool_ptr->help_mem_ptr ) )
        {
            // buffer is not ok!
            EPS_FATAL("eps_cp_mem_delete_nrt_pool(): MEM3 NRT pool, but check for overwrite is not ok");
        }

		eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );

		pool_ptr->help_mem_ptr = 0;
		pool_ptr->pool_nr      = POOL_UNKNOWN;
		pool_ptr->alias_nr     = POOL_UNKNOWN;
		pool_ptr->base_ptr     = 0;
		pool_ptr->size         = 0;
	}
	else
	{
		// Not all buffers freed		
		EPS_FATAL("eps_cp_mem_delete_nrt_pool(): MEM3 NRT pool, but not all NRT buffers deleted\n");
	}
#elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
	
	hif_mem_malloc_undo_init(pool_ptr->help_mem_ptr);
	
    pool_ptr->help_mem_ptr = 0;
    pool_ptr->pool_nr      = POOL_UNKNOWN;
    pool_ptr->alias_nr     = POOL_UNKNOWN;
    pool_ptr->base_ptr     = 0;
    pool_ptr->size         = 0;
#endif
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_VOID_PTR_TYPE ptr;
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.nrt_mem[pool_handle];
#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
#elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
#endif

	ptr = eps_cp_mem_nrt_alloc_by_poolptr(size, pool_handle, pool_ptr, lsa_comp_id);

	return ( ptr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_free( LSA_UINT8* ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.nrt_mem[pool_handle];
#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
#elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
#endif
	eps_cp_mem_nrt_free_by_poolptr(ptr, pool_handle, pool_ptr, lsa_comp_id);
}
/*----------------------------------------------------------------------------*/

static LSA_UINT8* eps_cp_mem_nrt_alloc_by_poolptr( LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_VOID_PTR_TYPE ptr;

	LSA_UNUSED_ARG(lsa_comp_id);

    EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

  if (size == 0)
  {
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL,
        "eps_cp_mem_nrt_alloc_by_poolptr: size(%u)!!!, pool_handle(%u), pool_ptr(%08x), lsa_comp_id(%u)", 
        size, pool_handle, pool_ptr, lsa_comp_id);

    EPS_FATAL("eps_cp_mem_nrt_alloc_by_poolptr(): Allocation with size == 0");
  }
  
#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	ptr = Mem3palloc ( size, pool_ptr->pool_nr );
#elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
	ptr = hif_mem_malloc(size, pool_ptr->help_mem_ptr);
#endif

	if (ptr == LSA_NULL)
	{	
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,
        "eps_cp_mem_nrt_alloc_by_poolptr(): Allocation returns 0-ptr!!!  size(%u)!!!, pool_handle(%u), pool_ptr(%08x), lsa_comp_id(%u)", 
        size, pool_handle, pool_ptr, lsa_comp_id);
	}

	return ( ptr );
}

/*----------------------------------------------------------------------------*/
static LSA_VOID eps_cp_mem_nrt_free_by_poolptr( LSA_UINT8* ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_UNUSED_ARG(lsa_comp_id);

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
#elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
	hif_mem_free(ptr, pool_ptr->help_mem_ptr);
#endif

#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	if ( GetBufSize( ptr ) > 0 ) // we have a valid buffer ?
	{
		Mem3free( ptr );
	}
	else
	{
		// free of an invalid mem block --> dump statistic and break		
		EPS_FATAL("MEM3 NRT memory free failed\n");
	}
#endif
}

/*----------------------------------------------------------------------------*/
//no inline function because of compiler dependencies!
#define EPS_CP_MEM_GET_POOL_HANDLE(pool_handle_, hSysDev_)                  \
    LSA_INT                 pool_handle_;                                   \
    EPS_SYS_PTR_TYPE const  pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev_;           \
	EPS_ASSERT( pSysPtr != 0 );                                             \
	EPS_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDI );                 \
    pool_handle_ = pSysPtr->hd_nr;

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_rxtx_default_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_INT pool_handle;
	EPS_SYS_PTR_TYPE const pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT( pSysPtr != 0 );
	EPS_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDI );
	pool_handle = pSysPtr->hd_nr;

	return ( eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_mem[pool_handle], lsa_comp_id ) );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_rxtx_default_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_INT pool_handle;
	EPS_SYS_PTR_TYPE const pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT( pSysPtr != 0 );
	EPS_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDI );
	pool_handle = pSysPtr->hd_nr;

	eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_dmacw_default_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_dmacw_default_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_dmacw_default_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_dmacw_default_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_rx_default_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
#if (PSI_HD_NRT_MEM==PSI_HD_NRT_RX_DEFAULT)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_mem[pool_handle], lsa_comp_id );
#else
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_rx_default_mem[pool_handle], lsa_comp_id );
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_rx_default_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
#if (PSI_HD_NRT_MEM==PSI_HD_NRT_RX_DEFAULT)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_mem[pool_handle], lsa_comp_id );
#else
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_rx_default_mem[pool_handle], lsa_comp_id );
#endif
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_tx_default_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
#if (PSI_HD_NRT_MEM==PSI_HD_NRT_RX_DEFAULT)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_mem[pool_handle], lsa_comp_id );
#else
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_tx_default_mem[pool_handle], lsa_comp_id );
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_tx_default_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
#if (PSI_HD_NRT_MEM==PSI_HD_NRT_RX_DEFAULT)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_mem[pool_handle], lsa_comp_id );
#else
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_tx_default_mem[pool_handle], lsa_comp_id );
#endif
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_tx_low_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_tx_low_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_tx_low_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_tx_low_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_tx_mid_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_tx_mid_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_tx_mid_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_tx_mid_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_tx_high_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_tx_high_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_tx_high_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_tx_high_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_tx_mgmtlow_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_tx_mgmtlow_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_tx_mgmtlow_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_tx_mgmtlow_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_tx_mgmthigh_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &eps_pools.nrt_tx_mgmthigh_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_tx_mgmthigh_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &eps_pools.nrt_tx_mgmthigh_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_nrt_fatal_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
		"eps_cp_mem_nrt_fatal_alloc(): pool_handle(%u) size(%08x) lsa_comp_id(%u)", 
		pool_handle, size, lsa_comp_id );
    EPS_ASSERT( LSA_FALSE );
    return 0;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_nrt_fatal_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE(pool_handle, hSysDev)
	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
		"eps_cp_mem_nrt_fatal_free(): pool_handle(%u) ptr(%08x) lsa_comp_id(%u)", 
		pool_handle, ptr, lsa_comp_id );
    EPS_ASSERT( LSA_FALSE );
}


/*----------------------------------------------------------------------------*/
LSA_INT eps_cp_mem_create_pi_pool( LSA_UINT16 board_nr, LSA_UINT8* base_ptr, 
									  LSA_UINT32 size, LSA_UINT32 gran_size,
									  EPS_MEMSET_FCT p_memset_fct )
{
	LSA_INT pool_id;
	ORDHW fftThreshold, helperSize;

	// create MEM3 pool for PI memory (KRAM, IOCC, ... )
	// Note: the pool is a Fft2Bound pool with created one for provider and an alias for consumer
	//       we use the PN Board number as handle for the PI pool
	LSA_VOID_PTR_TYPE pHelpMem;
	LSA_VOID_PTR_TYPE pBaseMem;

	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT((board_nr > 0 ) && (board_nr <= EPS_MAX_BOARD_NR));
	EPS_ASSERT( base_ptr != 0 );
	EPS_ASSERT( size     >  0 );
	EPS_ASSERT( p_memset_fct != LSA_NULL );

	pool_ptr = &eps_pools.pi_mem[board_nr];
	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

	fftThreshold = 0;      // Don't care (see MEM3 docu)

	helperSize = PoolHelperSize3 ( pool_ptr->size, gran_size, Fft2Bound, LSA_FALSE, 4 );

	pool_ptr->help_mem_ptr = eps_mem_alloc(helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );

	EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_pi_pool(): board_nr(%u) mem helpPtr(0x%08x) helpSize(%#x) gran_size(%#x)", 
		board_nr, pool_ptr->help_mem_ptr, helperSize, gran_size );

	// Note: assigment is necessary because ptr is modified in mem3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	// the pool number for creation is set based on HD-Nr and type of pool
	// Note: necessary to handle multiple pools of same type in MEM3
	// i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4, 5,6 for HD with HD-NR=1,....
	pool_id = (PI_POOL + (board_nr*POOL_NR_MAX));

	SetPresetVal( pool_id, 0x00 ); // for PI pool set to 0

	SetMemSetFunc( pool_id, p_memset_fct );
	SetCheckOverwrite( pool_id, LSA_FALSE );  // disable overwrite check
	SetPoolAlignment( pool_id, 4 );           // DWord allignment

	pool_ptr->pool_nr = CreatePool ( &pBaseMem, &pHelpMem, pool_ptr->size, helperSize, 
		gran_size, fftThreshold, Fft2Bound, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ));

	// Set the alias for the consumer pool
	pool_ptr->alias_nr = PoolAliasId( pool_ptr->pool_nr ); // AliasID for consumer pool

	pool_ptr->alias_nr = PoolAliasId( pool_ptr->pool_nr ); // AliasID for consumer pool
	EPS_ASSERT( PoolValid( pool_ptr->alias_nr ));

	EPS_SYSTEM_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_pi_pool(): board_nr(%u) pool-id(%u) provider_pool_nr(%#x) consumer_pool_nr(%#x) basePtr(0x%08x) baseSize(%#x)",
		board_nr, pool_id, pool_ptr->pool_nr, pool_ptr->alias_nr, pool_ptr->base_ptr, pool_ptr->size );

	return ( (LSA_INT)board_nr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_delete_pi_pool( LSA_UINT16 board_nr, LSA_INT pool_handle )
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT((board_nr > 0 ) && (board_nr <= EPS_MAX_BOARD_NR));
	EPS_ASSERT( pool_handle == board_nr );

	pool_ptr = &eps_pools.pi_mem[pool_handle];
	EPS_ASSERT( pool_ptr->pool_nr  != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );

	EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_pi_pool(): board_nr(%u), poolNr(%u) aliasNr(%u) helpPtr(0x%08x)", 
		board_nr, pool_ptr->pool_nr, pool_ptr->alias_nr, pool_ptr->help_mem_ptr );

	// delete the memory pool, and the helper memory
	if (( BufNumUsedInPool( pool_ptr->pool_nr  ) == 0 ) &&
		( BufNumUsedInPool( pool_ptr->alias_nr ) == 0 ))
	{
		int result = DeletePool( pool_ptr->pool_nr );
		EPS_ASSERT( result == 0 );

		eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );

		pool_ptr->help_mem_ptr = 0;
		pool_ptr->base_ptr     = 0;
		pool_ptr->size         = 0;
		pool_ptr->pool_nr      = POOL_UNKNOWN;
		pool_ptr->alias_nr     = POOL_UNKNOWN;
	}
	else
	{
		// Not all buffers freed --> dump statistics and break		
		EPS_FATAL("MEM3 not all PI buffers deleted\n");
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_pi_alloc( LSA_UINT32 size, LSA_BOOL is_provider, 
								   LSA_INT pool_handle,  LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_INT    pool_nr;
	LSA_UINT8 *ptr;
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	LSA_UNUSED_ARG(lsa_comp_id);

	EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.pi_mem[pool_handle];

	// Alloc process_image memory from Pool (provider = pool_nr, consumer = alias_nr)
	pool_nr = ( is_provider ) ? pool_ptr->pool_nr : pool_ptr->alias_nr;
	EPS_ASSERT( pool_nr != POOL_UNKNOWN );

	ptr = Mem3palloc ( size, pool_nr );

	if ( ptr != 0 ) // malloc ok ?
	{
		LSA_UINT32 offset;

		LSA_UINT32 allocSize = GetBufSize( ptr );

		// eps_mem_add_statistic( ptr, allocSize, __FILE__, __LINE__, lsa_comp_id, PSI_MTYPE_PI_MEM );
		offset = ptr - pool_ptr->base_ptr;

		EPS_SYSTEM_TRACE_07( 0, LSA_TRACE_LEVEL_CHAT,
			"eps_cp_mem_pi_alloc(): pool_handle(%u) ptr(%08x) is_provider(%u) offset(%#x) size(%#x) alloc Size(%#x) poolNr(%u)", 
			pool_handle, ptr, is_provider, offset, size, allocSize, pool_nr );
	}
	else
	{
		// failed
		EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR,
			"eps_cp_mem_pi_alloc(): alloc failed, pool_handle(%u) is_provider(%u) size(%#x) poolNr(%u)", 
			pool_handle, is_provider, size, pool_nr );
		
		// IOH handles the case to get 0-ptr. NO FATAL!!!
	}

	return ( ptr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_pi_free( LSA_UINT8* p_buffer, LSA_INT pool_handle, 
							    LSA_COMP_ID_TYPE lsa_comp_id )
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	LSA_UNUSED_ARG(lsa_comp_id);

	EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.pi_mem[pool_handle];
	EPS_ASSERT( pool_ptr->pool_nr  != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr != POOL_UNKNOWN );

	// eps_mem_remove_statistic( p_buffer, lsa_comp_id, PSI_MTYPE_PI_MEM );

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT,
		"eps_cp_mem_pi_free(): ptr(%08x)", 
		p_buffer );
	
	if ( GetBufSize( p_buffer ) != 0 ) // allocated buffer ?
	{
		Mem3free( p_buffer );
	}
	else
	{
		// it looks like, we have an invalid mem block		
		EPS_FATAL("eps_cp_mem_pi_free(): MEM3 PI memory free failed\n");
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 eps_cp_mem_get_pi_offset( LSA_UINT8* buffer_ptr, LSA_INT pool_handle )
{
	LSA_UINT32 offset;
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( buffer_ptr != 0 );
	EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.pi_mem[pool_handle];
	EPS_ASSERT( pool_ptr->pool_nr  != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr != POOL_UNKNOWN );

	offset = buffer_ptr - pool_ptr->base_ptr;

	return ( offset );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_get_pi_base( LSA_INT pool_handle )
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT((pool_handle > 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.pi_mem[pool_handle];
	EPS_ASSERT( pool_ptr->pool_nr  != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr != POOL_UNKNOWN );

	return ( pool_ptr->base_ptr );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** 
 * memset function for paea of SOC1 
 * 
 * Sets len bytes in memory at dst to val. 
 * 
 * @param hd_nr number of hardwaredevice
 * @param dst   startadress of memory
 * @param val   value the memory is set to
 * @param len   number of bytes to be changed
 * @return dst
 */
static LSA_VOID* eps_cp_mem_hd_paea_memset( LSA_UINT16 hd_nr, LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len )
{
	EPS_ASSERT((hd_nr > 0 ) && (hd_nr <= EPS_MAX_BOARD_NR));

#if ((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_SOC)))    // Only for SOC1
	if ( dst != 0 ) // buffer allocated ?
	{
		LSA_UINT8 *base_ptr;
		EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

		size_t buffer_len = len;
		LSA_UINT8 *buffer_ptr = (LSA_UINT8*)dst;

		pool_ptr = &eps_pools.pi_mem[hd_nr];  // using base of HD
		base_ptr = pool_ptr->base_ptr;
		EPS_ASSERT( base_ptr != LSA_NULL );

		// calc direct access addr. (offset in steps of 4)
		buffer_ptr = (((LSA_UINT8*)dst - base_ptr) << 2 ) + base_ptr;

		// Note: Setting is done over direct access address
		for ( ;buffer_len != 0; buffer_len-- )
		{
			*buffer_ptr = (LSA_UINT8)val;
			buffer_ptr += 4;    //direct access mode: PAEA ram is accessed in steps of 4
		}
	}
#else
	LSA_UNUSED_ARG( dst );
	LSA_UNUSED_ARG( val );
	LSA_UNUSED_ARG( len );

	EPS_FATAL("function can only be used with SOC1 Board.");
#endif

	return( dst );
}

/*----------------------------------------------------------------------------*/
LSA_VOID* eps_cp_mem_hd1_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len )
{
	return ( eps_cp_mem_hd_paea_memset( 1, dst, val, len ));
}

/*----------------------------------------------------------------------------*/
LSA_VOID* eps_cp_mem_hd2_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len )
{
	return ( eps_cp_mem_hd_paea_memset( 2, dst, val, len ));
}

/*----------------------------------------------------------------------------*/
LSA_VOID* eps_cp_mem_hd3_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len )
{
	return ( eps_cp_mem_hd_paea_memset( 3, dst, val, len ));
}

/*----------------------------------------------------------------------------*/
LSA_VOID* eps_cp_mem_hd4_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len )
{
	return ( eps_cp_mem_hd_paea_memset( 4, dst, val, len ));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_INT eps_cp_mem_create_hif_pool( LSA_UINT16 board_nr, LSA_UINT8* base_ptr, LSA_UINT32 size )
{

#if (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT || defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) \
	&& (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
	LSA_INT16 pool_handle_lower, pool_handle_upper;

	EPS_ASSERT( board_nr <= EPS_MAX_BOARD_NR );
	EPS_ASSERT( base_ptr   != 0 );
	EPS_ASSERT( size       >  0 );

	pool_handle_lower = (LSA_INT16)HIF_CREATE_MEM_RING_POOL((LSA_UINT8*)base_ptr, size/2);
	pool_handle_upper = (LSA_INT16)HIF_CREATE_MEM_RING_POOL((LSA_UINT8*)base_ptr + size/2, size/2);
		
	return (((LSA_INT)pool_handle_lower)<<16) + pool_handle_upper;

#else
	LSA_INT pool_id;

#if (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	ORDHW granSize, fftThreshold, helperSize;

	// create mem3 pool for HIF memory (Shared mem or SDRAM)
	// Note: we use the PN Board number as handle for the HIF pool
	//       in case of HIF LD the number is 0 
	LSA_VOID_PTR_TYPE pHelpMem;
	LSA_VOID_PTR_TYPE pBaseMem;
#endif

	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( board_nr <= EPS_MAX_BOARD_NR );
	EPS_ASSERT( base_ptr   != 0 );
	EPS_ASSERT( size       >  0 );

	pool_ptr = &eps_pools.hif_mem[board_nr];

	#if (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT || defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	if(pool_ptr->base_ptr != 0)
	{
	    // if pool was already created only increment pool_count (count pool create count after pool was created)
		LSA_INT16 pool_count, pool_nr;
		pool_count = (LSA_INT16)(pool_ptr->pool_nr>>16);
		pool_nr = (LSA_INT16)pool_ptr->pool_nr;
		pool_ptr->pool_nr = (((LSA_INT)(pool_count+1))<<16) + pool_nr;
		return ( (LSA_INT)board_nr );
	}
	#endif

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;
	
    // the pool number for creation is set based on HD-Nr and type of pool
    // Note: necessary to handle multiple pools of same type in MEM3
    // i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4, 5,6 for HD with HD-NR=1,....
    pool_id = (HIF_POOL + board_nr*POOL_NR_MAX);
	
#if (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	granSize     = 0x20; // 32 Byte packages 
	fftThreshold = 0x0; 
	helperSize = PoolHelperSize3 ( size, granSize, Fft2Bound, LSA_TRUE, 4 );

	pool_ptr->help_mem_ptr = eps_mem_alloc(helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_hif_pool(): boardNr(%u) helpPtr(0x%08x) helpSize(0x%x)", 
		board_nr, pool_ptr->help_mem_ptr, helperSize );

	// Note: assignment is necessary because ptr are modified in mem3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	SetMemSetFunc( pool_id, (EPS_MEMSET_FCT)eps_mem3_memset );
	SetCheckOverwrite( pool_id, LSA_TRUE );   // enable overwrite check
	SetPoolAlignment( pool_id, 4 );           // DWord allignment

	pool_ptr->pool_nr = CreatePool ( &pBaseMem, &pHelpMem, pool_ptr->size, helperSize, 
		granSize, fftThreshold, Fft2Bound, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ));
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
//	pool_ptr->help_mem_ptr = hif_mem_malloc_init(base_ptr, size, NULL);
//	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
    pool_ptr->pool_nr = hif_mem_malloc_create_pool(base_ptr, size);
    EPS_ASSERT( pool_ptr->pool_nr > 0 );
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
//	pool_ptr->help_mem_ptr = hif_mem_ring_pool_init(base_ptr, size, NULL);
//	EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
	pool_ptr->pool_nr = hif_mem_ring_pool_create(base_ptr, size);
    EPS_ASSERT( pool_ptr->pool_nr > 0 );
#endif

	EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_hif_pool(): boardNr(%u) PoolNr(%u) MEM3 poolNr(%u) basePtr(0x%08x) baseSize(%#x)",
		board_nr, pool_id, pool_ptr->pool_nr, pool_ptr->base_ptr, pool_ptr->size );


	return ( (LSA_INT)board_nr );

#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_delete_hif_pool( LSA_UINT16 board_nr, LSA_INT pool_handle )
{
#if (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT || defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) \
	&& (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
	LSA_INT16 pool_handle_lower, pool_handle_upper;
	EPS_ASSERT( board_nr <= EPS_MAX_BOARD_NR );

	pool_handle_lower = (LSA_INT16)(pool_handle>>16);
	pool_handle_upper = (LSA_INT16)pool_handle;

	HIF_DELETE_MEM_RING_POOL(pool_handle_lower);
	HIF_DELETE_MEM_RING_POOL(pool_handle_upper);
#else
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( board_nr <= EPS_MAX_BOARD_NR );
	EPS_ASSERT( pool_handle == board_nr );

	pool_ptr = &eps_pools.hif_mem[pool_handle];
	
	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_hif_pool(): board_nr(%u), poolNr(%u) helpPtr(0x%08x)", 
		board_nr, pool_ptr->pool_nr, pool_ptr->help_mem_ptr );

	#if (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT || defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	{
		LSA_INT16 pool_count, pool_nr;
		pool_count = (LSA_INT16)(pool_ptr->pool_nr>>16);
		pool_nr = (LSA_INT16)pool_ptr->pool_nr;
		if(pool_count > 0)
		{
		    // while pool_count bigger than 0 decrement pool_count and return
			pool_ptr->pool_nr = (((LSA_INT)(pool_count-1))<<16) + pool_nr;
			return;
		}
	}
	#endif


#if (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	// delete the memory pool, and the helper memory
	if ( BufNumUsedInPool( pool_ptr->pool_nr ) == 0 ) // all memory blocks freed ?
	{
		int result = DeletePool( pool_ptr->pool_nr );
		EPS_ASSERT( result == 0 );

        // check for overwrite
        if ( !VerifyMem3Buf( pool_ptr->help_mem_ptr ) )
        {
            // buffer is not ok!
            EPS_FATAL("eps_cp_mem_delete_hif_pool(): MEM3 HIF pool, but check for overwrite is not ok");
        }

        eps_mem_free(pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);

		pool_ptr->help_mem_ptr = 0;
		pool_ptr->pool_nr      = POOL_UNKNOWN;
		pool_ptr->alias_nr     = POOL_UNKNOWN;
		pool_ptr->base_ptr     = 0;
		pool_ptr->size         = 0;
	}
	else
	{
		// Not all buffers freed		
		EPS_FATAL("eps_cp_mem_delete_hif_pool(): MEM3 HIF pool, not all HIF buffers deleted\n");
	}
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
//    EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
//    hif_mem_malloc_undo_init(pool_ptr->help_mem_ptr);
    EPS_ASSERT( pool_ptr->pool_nr > 0 );
    hif_mem_malloc_delete_pool(pool_ptr->pool_nr);
    pool_ptr->help_mem_ptr = 0;
    pool_ptr->pool_nr      = POOL_UNKNOWN;
    pool_ptr->alias_nr     = POOL_UNKNOWN;
    pool_ptr->base_ptr     = 0;
    pool_ptr->size         = 0;
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
//    EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
//    hif_mem_ring_pool_undo_init(pool_ptr->help_mem_ptr);
    EPS_ASSERT( pool_ptr->pool_nr > 0 );
    hif_mem_ring_pool_delete(pool_ptr->pool_nr);
    pool_ptr->help_mem_ptr = 0;
    pool_ptr->pool_nr      = POOL_UNKNOWN;
    pool_ptr->alias_nr     = POOL_UNKNOWN;
    pool_ptr->base_ptr     = 0;
    pool_ptr->size         = 0;
#endif
#endif
}

/*----------------------------------------------------------------------------*/
LSA_UINT8* eps_cp_mem_hif_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_BOOL bUpper,
								    LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_VOID_PTR_TYPE ptr = LSA_NULL;

#if (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT || defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) \
	&& (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
	LSA_UNUSED_ARG(lsa_comp_id);
	if(bUpper)
	{
		ptr = HIF_MEM_RING_ALLOC(size, (LSA_INT16)pool_handle);
	}
	else
	{
		ptr = HIF_MEM_RING_ALLOC(size, (LSA_INT16)(pool_handle>>16));
	}
#else
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	LSA_UNUSED_ARG(bUpper);
	EPS_ASSERT((pool_handle >= 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.hif_mem[pool_handle];
	
#if (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	ptr = eps_mem_alloc_from_pool(size, lsa_comp_id, PSI_MTYPE_HIF_MEM, (LSA_INT16)pool_ptr->pool_nr);
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
    LSA_UNUSED_ARG(lsa_comp_id);
//    EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
//    ptr = hif_mem_malloc(size, pool_ptr->help_mem_ptr);
    EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    ptr = hif_mem_malloc_from_pool(size, pool_ptr->pool_nr);
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
    LSA_UNUSED_ARG(lsa_comp_id);
//    EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
//    ptr = hif_mem_ring_alloc_from_pool(size, pool_ptr->help_mem_ptr);
    EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    ptr = hif_mem_ring_pool_alloc(size, pool_ptr->pool_nr);
#endif

#endif
	if (ptr == LSA_NULL)
	{
    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL,
        "eps_cp_mem_hif_alloc(): pool_handle(%u) ptr(%08x) size(%#x)", 
        pool_handle, ptr, size);

    EPS_FATAL("eps_cp_mem_hif_alloc(): Allocation returns 0-ptr");
	}

	return ( ptr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_hif_free( LSA_UINT8* ptr, LSA_INT pool_handle, LSA_BOOL bUpper,
								 LSA_COMP_ID_TYPE lsa_comp_id )
{
#if (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT || defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) \
	&& (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
	LSA_UNUSED_ARG(lsa_comp_id);
	EPS_ASSERT( ptr != 0 );
	if(bUpper)
	{
		HIF_MEM_RING_FREE(ptr, (LSA_INT16)pool_handle);
	}
	else
	{
		HIF_MEM_RING_FREE(ptr, (LSA_INT16)(pool_handle>>16));
	}
#else
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	LSA_UNUSED_ARG(bUpper);

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT((pool_handle >= 0 ) && (pool_handle <= EPS_MAX_BOARD_NR));

	pool_ptr = &eps_pools.hif_mem[pool_handle];
	
#if (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	eps_mem_free_from_pool(ptr, lsa_comp_id, PSI_MTYPE_HIF_MEM, (LSA_INT16)pool_ptr->pool_nr);
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
    LSA_UNUSED_ARG(lsa_comp_id);
//    EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
//    hif_mem_free(ptr, pool_ptr->help_mem_ptr);
    EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    hif_mem_free_from_pool(ptr, pool_ptr->pool_nr);
#elif (EPS_HIF_SER_MEM_USAGE==EPS_MEM_USAGE_HIF_RING_POOL)
    LSA_UNUSED_ARG(lsa_comp_id);
//    EPS_ASSERT( pool_ptr->help_mem_ptr != 0 );
//    hif_mem_ring_free_from_pool(ptr, pool_ptr->help_mem_ptr);
    EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    hif_mem_ring_pool_free(ptr, pool_ptr->pool_nr);
#endif

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_CHAT,
		"eps_cp_mem_hif_free(): pool_handle(%u) ptr(%08x)", 
		pool_handle, ptr );
#endif
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
