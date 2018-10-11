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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI system LSA interface                                  */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      5
#define LTRC_ACT_MODUL_ID  5

#include "psi_inc.h"
#include "psi_int.h"
#include "psi_ld.h"
#include "psi_hd.h"
#include "psi_sys.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*---------------------------------------------------------------------------*/
/*  Initialization / cleanup                                                 */
/*---------------------------------------------------------------------------*/

static LSA_VOID psi_init_comp_start( LSA_VOID )
{
	/* Initialize all PNIO stack components */

#if( PSI_CFG_USE_ACP == 1 )
	acp_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_CLRPC == 1 )
	clrpc_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_CM == 1 )
	cm_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_DCP == 1 )
	dcp_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_EDDI == 1 )
	eddi_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_EDDS == 1 )
	edds_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_EDDP == 1 )
	eddp_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_GSY == 1 )
	gsy_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_HIF == 1 )
	hif_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_IOH == 1 )
	ioh_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_LLDP == 1 )
	lldp_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_MRP == 1 )
	mrp_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_NARE == 1 )
	nare_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_OHA == 1 )
	oha_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_POF == 1 )
	pof_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_SNMPX == 1 )
	snmpx_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_SOCK == 1 )
	sock_psi_startstop( 1 );
#endif

#if( PSI_CFG_USE_TCIP == 1 )
	tcip_psi_startstop( 1 );
#endif
}

/*---------------------------------------------------------------------------*/

static LSA_VOID psi_init_comp_stop( LSA_VOID )
{
	/* Initialize all PNIO stack components */
	/* Note: in case of PNTRC component is used for tracing, the component must be undo init last */
#if( PSI_CFG_USE_ACP == 1 )
	acp_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_CLRPC == 1 )
	clrpc_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_CM == 1 )
	cm_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_DCP == 1 )
	dcp_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_EDDI == 1 )
	eddi_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_EDDS == 1 )
	edds_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_EDDP == 1 )
	eddp_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_GSY == 1 )
	gsy_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_HIF == 1 )
	hif_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_IOH == 1 )
	ioh_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_LLDP == 1 )
	lldp_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_MRP == 1 )
	mrp_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_NARE == 1 )
	nare_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_OHA == 1 )
	oha_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_POF == 1 )
	pof_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_SNMPX == 1 )
	snmpx_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_SOCK == 1 )
	sock_psi_startstop( 0 );
#endif

#if( PSI_CFG_USE_TCIP == 1 )
	tcip_psi_startstop( 0 );
#endif
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static LSA_UINT16 psi_enter_exit_id = PSI_LOCK_ID_INVALID;

LSA_UINT16 psi_init( LSA_VOID )
{
	LSA_RESPONSE_TYPE rsp = 0;
	
	PSI_LD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"psi_init()");

	PSI_ALLOC_REENTRANCE_LOCK(&rsp, &psi_enter_exit_id );

	/* Init the PSI instance vars */
	psi_ld_init();
	psi_hd_init();
	psi_res_calc_init();
	psi_mbx_sys_init();

	/* Init all PNIO stack components */
	psi_init_comp_start( );

	return( LSA_RET_OK );
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 psi_undo_init( LSA_VOID )
{
	LSA_RESPONSE_TYPE rsp = 0;

	PSI_LD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"psi_undo_init()");

	/* First quit the Threads to finish any open processes */
	psi_mbx_sys_undo_init();
	/* Undo INIT all PNIO stack components */
	psi_init_comp_stop();

	/* Undo init the PSI instance vars */
	psi_hd_undo_init();
	psi_ld_undo_init();
	psi_res_calc_undo_init();

	PSI_FREE_REENTRANCE_LOCK(&rsp, psi_enter_exit_id );

	return( LSA_RET_OK );
}

/*---------------------------------------------------------------------------*/
/*  FATAL Handling                                                           */
/*---------------------------------------------------------------------------*/

LSA_FATAL_ERROR_TYPE  psi_glob_fatal_error;

LSA_VOID psi_fatal_error (
	LSA_UINT16         comp_id,
	LSA_UINT16         module_id,
	LSA_UINT16         line,
	LSA_UINT32         error_code_0,
	LSA_UINT32         error_code_1,
	LSA_UINT32         error_code_2,
	LSA_UINT32         error_code_3,
	LSA_UINT16         error_data_len,
	LSA_VOID_PTR_TYPE  error_data )
{
	psi_glob_fatal_error.lsa_component_id  = comp_id;
	psi_glob_fatal_error.module_id         = module_id;
	psi_glob_fatal_error.line              = line;

	psi_glob_fatal_error.error_code[0]     = error_code_0;
	psi_glob_fatal_error.error_code[1]     = error_code_1;
	psi_glob_fatal_error.error_code[2]     = error_code_2;
	psi_glob_fatal_error.error_code[3]     = error_code_3;

	psi_glob_fatal_error.error_data_length = error_data_len;
	psi_glob_fatal_error.error_data_ptr    = error_data;

	PSI_FATAL_ERROR( "PSI", (LSA_UINT16)(sizeof(psi_glob_fatal_error)), &psi_glob_fatal_error);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE psi_alloc_local_mem( LSA_UINT32 length )
{
	LSA_USER_ID_TYPE dummy_id = {0};
	LSA_VOID_PTR_TYPE mem_ptr = LSA_NULL;

	PSI_ALLOC_LOCAL_MEM( &mem_ptr, dummy_id, length, 0, LSA_COMP_ID_PSI, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT( mem_ptr != LSA_NULL );

	if ( psi_is_null( mem_ptr ))
	{
		PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_alloc_local_mem(): failed, mem_ptr(%#x), size(%u/%#x)",
			mem_ptr, 
			length,
			length );

		PSI_FATAL( 0 );
	}

	return ( mem_ptr );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_free_local_mem( LSA_VOID_PTR_TYPE  mem_ptr )
{
	LSA_UINT16 ret_val;

	PSI_ASSERT( mem_ptr != LSA_NULL );

	PSI_FREE_LOCAL_MEM( &ret_val, mem_ptr, 0, LSA_COMP_ID_PSI, PSI_MTYPE_LOCAL_MEM );

	if ( ret_val != LSA_RET_OK )
	{
		PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_free_local_mem(): failed, mem_ptr(%#x), ret_val(%u/%#x)",
			mem_ptr, 
			ret_val,
			ret_val );

		PSI_FATAL( 0 );
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_BOOL psi_get_mrp_default_rports( 
	LSA_UINT16             if_nr, 
	PSI_HD_INPUT_PTR_TYPE  hd_input_ptr,
	LSA_UINT8              *rport1_ptr,
	LSA_UINT8              *rport2_ptr )
{
	LSA_UINT16 idx;
	LSA_UINT8  port[30] = {0};
	LSA_UINT16 found = 0;

	LSA_BOOL result = LSA_FALSE;

	PSI_ASSERT( hd_input_ptr != LSA_NULL );
	PSI_ASSERT(( if_nr > 0 ) && ( if_nr <= PSI_CFG_MAX_HD_IF_CNT));

	// lookup in HW port entries for user MRP ring ports matching this IF
	for ( idx = 0; idx < hd_input_ptr->nr_of_ports; idx++ )
	{
		if ( hd_input_ptr->hw_port[idx].if_nr == if_nr ) // HW port entry matches ?
		{
			if ( hd_input_ptr->hw_port[idx].mrp_ringport_type == PSI_RING_PORT_DEFAULT )
			{
				port[found] = (LSA_UINT8)hd_input_ptr->hw_port[idx].user_nr;
				found++;
			}
		}
	}

	if ( found == 0 ) // MRP ringports avaliable ?
	{
		*rport1_ptr = 0;  // No MRP port
		*rport2_ptr = 0;  // No MRP port

		result = LSA_FALSE;
	}
	else
	{
		PSI_ASSERT( found == 2 ); // check valid setting for default ringports

		*rport1_ptr = port[0];
		*rport2_ptr = port[1];

		result = LSA_TRUE;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
PSI_HD_PORT_INPUT_PTR_TYPE psi_get_hw_port_entry_for_user_port( 
	LSA_UINT16             if_nr,
	LSA_UINT16             usr_port_nr,
	PSI_HD_INPUT_PTR_TYPE  hd_input_ptr )
{
	LSA_UINT16 idx;
	PSI_HD_PORT_INPUT_PTR_TYPE entry_ptr = LSA_NULL;

	PSI_ASSERT( hd_input_ptr != LSA_NULL );
	PSI_ASSERT(( if_nr > 0 ) && ( if_nr <= PSI_CFG_MAX_HD_IF_CNT ));

	// lookup in HW port entries for matching entry
	for ( idx = 0; idx < hd_input_ptr->nr_of_ports; idx++ )
	{
		if (( hd_input_ptr->hw_port[idx].if_nr   == if_nr       ) &&  // HW port entry matches ?
			( hd_input_ptr->hw_port[idx].user_nr == usr_port_nr ))
		{
			entry_ptr = &hd_input_ptr->hw_port[idx];
			break;
		}
	}

	return ( entry_ptr );
}

/*------------------------------------------------------------------------------
//	Pointer-Test, used in module internal macros: is_null() and is_not_null()
//----------------------------------------------------------------------------*/

void * PSI_TEST_POINTER(void * ptr) {
	if( ptr != LSA_NULL ) { /* especially 0xCCCCCCCC */
		PSI_ASSERT( ptr != 0 ); /* 0 is never ok if not LSA_NULL */
		//PSI_ASSERT( (unsigned long)ptr < 0x80000000L ); /* at least true for NT */
	}
	return ptr;
}

void * PSI_TEST_ALIGN2(void * ptr) {
	ptr = PSI_TEST_POINTER(ptr);
	PSI_ASSERT( ((unsigned long)ptr & 0x1) == 0 );
	return ptr;
}

void * PSI_TEST_ALIGN4(void * ptr) {
	ptr = PSI_TEST_POINTER(ptr);
	PSI_ASSERT( ((unsigned long)ptr & 0x3) == 0 );
	return ptr;
}

#ifndef PSI_SWAP_U16
LSA_VOID   PSI_SWAP_U16(
	LSA_UINT16  * short_ptr ) /* correct alignment! */
{
	LSA_UINT16 temp;
	(void)PSI_TEST_ALIGN2(short_ptr);
	temp = *short_ptr;
	*short_ptr = (temp >> 8) | (temp << 8);
}
#else
# error "by design a function"
#endif

#ifndef PSI_SWAP_U32
LSA_VOID   PSI_SWAP_U32(
	LSA_UINT32  * long_ptr ) /* correct alignment! */
{
	LSA_UINT32 temp;
	(void)PSI_TEST_ALIGN4(long_ptr);
	temp = *long_ptr;
	*long_ptr = (temp >> 24)
		| ((temp >> 8) & 0x0000ff00)
		| ((temp << 8) & 0x00ff0000)
		| (temp << 24);
}
#else
# error "by design a function"
#endif


#ifndef PSI_SWAP_U64
LSA_VOID	PSI_SWAP_U64(LSA_UINT64  * long_long_ptr)  

{
    LSA_UINT64 temp;
    temp = *long_long_ptr;

  *long_long_ptr = (((LSA_UINT64)(temp) << 56) | 
  (((LSA_UINT64)(temp) << 40) & 0xff000000000000ULL) | 
  (((LSA_UINT64)(temp) << 24) & 0xff0000000000ULL) | 
  (((LSA_UINT64)(temp) << 8)  & 0xff00000000ULL) | 
  (((LSA_UINT64)(temp) >> 8)  & 0xff000000ULL) | 
  (((LSA_UINT64)(temp) >> 24) & 0xff0000ULL) | 
  (((LSA_UINT64)(temp) >> 40) & 0xff00ULL) | 
  ((LSA_UINT64)(temp)  >> 56));
}
 
#else
# error "by design a function"
#endif


/*=============================================================================
 * function name:  PSI_HTON16 ... host to network byte order, short integer
 *                 PSI_HTON32 ... host to network byte order, long  integer
 *                 PSI_NTOH16 ... network to host byte order, short integer
 *                 PSI_NTOH32 ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 *===========================================================================*/

#ifndef PSI_HTON16
LSA_UINT16  PSI_HTON16(
	LSA_UINT16 host_short )
{
#ifdef LSA_HOST_ENDIANESS_LITTLE  
	{
		PSI_SWAP_U16(&host_short);
	}
#endif

	return( host_short );
}
#else
# error "by design a function"
#endif

#ifndef PSI_HTON32
LSA_UINT32  PSI_HTON32(
	LSA_UINT32 host_long
) {
#ifdef LSA_HOST_ENDIANESS_LITTLE  
	{
		PSI_SWAP_U32(&host_long);
	}
#endif
	return( host_long );
}
#else
# error "by design a function"
#endif

#ifndef PSI_HTON64
LSA_UINT64  PSI_HTON64(
	LSA_UINT64 host_long_long )
{
#ifdef LSA_HOST_ENDIANESS_LITTLE  
	{
		PSI_SWAP_U64(&host_long_long);
	}
#endif

	return( host_long_long );
}
#else
# error "by design a function"
#endif

#ifndef PSI_NTOH16
LSA_UINT16  PSI_NTOH16(
	LSA_UINT16 network_short )
{
#ifdef LSA_HOST_ENDIANESS_LITTLE  
	{
		PSI_SWAP_U16(&network_short);
	}
#endif

	return( network_short);
}
#else
# error "by design a function"
#endif

#ifndef PSI_NTOH32
LSA_UINT32  PSI_NTOH32(
	LSA_UINT32 network_long )
{
#ifdef LSA_HOST_ENDIANESS_LITTLE  
	{
		PSI_SWAP_U32(&network_long);
	}
#endif 

	return( network_long );
}
#else
# error "by design a function"
#endif


#ifndef PSI_NTOH64
LSA_UINT64  PSI_NTOH64(
	LSA_UINT64 network_long_long )
{
#ifdef LSA_HOST_ENDIANESS_LITTLE  
	{
		PSI_SWAP_U64(&network_long_long);
	}
#endif

	return( network_long_long);
}
#else
# error "by design a function"
#endif
/*----------------------------------------------------------------------------*/
/* PSI ENTER / EXIT                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_ENTER
LSA_VOID   PSI_ENTER(LSA_VOID)
{
  PSI_ENTER_REENTRANCE_LOCK( psi_enter_exit_id );
}
#else
# error "by design a function"
#endif

#ifndef PSI_EXIT
LSA_VOID   PSI_EXIT(LSA_VOID)
{
  PSI_EXIT_REENTRANCE_LOCK( psi_enter_exit_id );
}
#else
# error "by design a function"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
