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
/*  F i l e               &F: hif_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of LSA-component HIF                       */
/*                                                                           */
/*****************************************************************************/

#define HIF_MODULE_ID      32018
#define LTRC_ACT_MODUL_ID  32018

#include "psi_int.h"

int hif_semicolon_missing = 0;

static LSA_UINT16 hif_enter_exit_id = PSI_LOCK_ID_INVALID;
static LSA_UINT16 hif_hd_enter_exit_id = PSI_LOCK_ID_INVALID;
static LSA_UINT16 hif_ld_enter_exit_id = PSI_LOCK_ID_INVALID;

/* Switch for Shared Memory without serialization debugging.
   Using this simulates a shared memory interface but everything
   is running on the same processor.
   This is an HIF Test Mode which should only be used by HIF Development! */
#if (defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)  || (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
static LSA_UINT32 g_HifLocalUSharedMem[PSI_CFG_MAX_HD_CNT+1][2][0x100000];
static LSA_UINT32 g_HifRemoteUSharedMem[PSI_CFG_MAX_HD_CNT+1][2][0x100000];
#endif


#if ( PSI_CFG_USE_HIF == 1 )
/*----------------------------------------------------------------------------*/

#include "psi_usr.h"
#include "psi_sys.h"
#include "hif_int.h"

#ifdef HIF_CFG_MEM_RING_CHECK_OVERWRITE
#include "hif_mem_ring.h"
#endif //HIF_CFG_MEM_RING_CHECK_OVERWRITE
#ifdef HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC
#include "stdlib.h" /* for malloc(), free()*/
#endif
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)

/*----------------------------------------------------------------------------*/

void hif_psi_startstop( int start )
{
	LSA_UINT16        rc;
	LSA_RESPONSE_TYPE rsp;

	if ( start ) 
	{
        PSI_ALLOC_REENTRANCE_LOCK( &rsp, &hif_enter_exit_id );
        PSI_ALLOC_REENTRANCE_LOCK( &rsp, &hif_hd_enter_exit_id );
        PSI_ALLOC_REENTRANCE_LOCK( &rsp, &hif_ld_enter_exit_id );
        HIF_ASSERT( rsp == LSA_RET_OK );
		rc = hif_init();
		HIF_ASSERT( rc == LSA_RET_OK );
	}
	else
	{
		rc = hif_undo_init();
		HIF_ASSERT( rc == LSA_RET_OK );
        PSI_FREE_REENTRANCE_LOCK( &rsp, hif_enter_exit_id );
        PSI_FREE_REENTRANCE_LOCK( &rsp, hif_hd_enter_exit_id );
        PSI_FREE_REENTRANCE_LOCK( &rsp, hif_ld_enter_exit_id );
        HIF_ASSERT( rsp == LSA_RET_OK );
        hif_enter_exit_id = PSI_LOCK_ID_INVALID;
	}
}

/*===========================================================================*/
/*     Memory pool                                                           */
/*===========================================================================*/

#ifndef HIF_CREATE_SER_POOL
LSA_VOID HIF_CREATE_SER_POOL(
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32        size,
	HIF_SYS_HANDLE    sys_handle,
	LSA_INT *         pool )
{
	HIF_PROGRAM_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,"HIF_CREATE_SER_POOL(): base 0x%x, size 0x%x",base, size);
	PSI_CREATE_HIF_MEM_POOL( base, size, sys_handle, pool );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_DELETE_SER_POOL
LSA_VOID HIF_DELETE_SER_POOL(
	LSA_UINT16 *   ret_val_ptr,
	HIF_SYS_HANDLE sys_handle,
	LSA_INT        pool )
{
	PSI_DELETE_HIF_MEM_POOL( ret_val_ptr, sys_handle, pool );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_ALLOC_SER_MEM
LSA_VOID HIF_ALLOC_SER_MEM(
	LSA_VOID_PTR_TYPE * ptr_ptr_mem,
	LSA_UINT32          length,
	HIF_SYS_HANDLE		sys_handle,
	LSA_INT             pool )
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_ALLOC_SER_MEM() --> length(%i)", length);
	#endif

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"HIF_ALLOC_SER_MEM(): invalid length 0" );
        *ptr_ptr_mem = LSA_NULL;
		HIF_FATAL( 0 );
        return;
    }

	PSI_HIF_ALLOC_MEM( ptr_ptr_mem, length, sys_handle, pool );

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_ALLOC_SER_MEM(): The total function duration is %d ns",(end_count - start_count));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_ALLOC_SER_MEM()");
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_FREE_SER_MEM
LSA_VOID HIF_FREE_SER_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   ptr_mem,
	HIF_SYS_HANDLE		sys_handle,
	LSA_INT             pool )
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_FREE_SER_MEM()");
	#endif

	PSI_HIF_FREE_MEM( ret_val_ptr, ptr_mem, sys_handle, pool );

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_FREE_SER_MEM(): The total function duration is %d ns",(end_count - start_count));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_FREE_SER_MEM()");
	#endif
}

#else
#error "by design a function!"
#endif

#ifndef HIF_SHM_DMA_COPY
LSA_VOID HIF_SHM_DMA_COPY(
	LSA_VOID * *    ptr_ptr_rqb,
	LSA_UINT32      rqb_len,
	HIF_SYS_HANDLE  sys_handle )
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "HIF_SHM_DMA_COPY() --> rb(0x%x) serRqbLen(%i)", *ptr_ptr_rqb, rqb_len);
	PSI_HIF_SHM_DMA_COPY( ptr_ptr_rqb, rqb_len, sys_handle );
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "HIF_SHM_DMA_COPY() <-- rb(0x%x) serRqbLen(%i)", *ptr_ptr_rqb, rqb_len);	
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   Timer                                                                   */
/*===========================================================================*/

#ifndef HIF_ALLOC_TIMER
LSA_VOID HIF_ALLOC_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * timer_id_ptr,
	LSA_UINT16   timer_type,
	LSA_UINT16   time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, hif_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_START_TIMER
LSA_VOID HIF_START_TIMER(
	LSA_UINT16 *        ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT16          time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_STOP_TIMER
LSA_VOID HIF_STOP_TIMER(
	LSA_UINT16 *        ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_FREE_TIMER
LSA_VOID HIF_FREE_TIMER(
	LSA_UINT16 *        ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_GET_NS_TICKS
LSA_UINT64 HIF_GET_NS_TICKS()
{
    return PSI_GET_TICKS_100NS() * 100;
}
#else
#error "by design a function!"
#endif

#ifndef HIF_WAIT_NS
LSA_VOID HIF_WAIT_NS( 
    LSA_UINT64 uTimeNs )
{
    PSI_WAIT_NS ( uTimeNs );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   LOCK                                                                    */
/*===========================================================================*/

#ifndef HIF_ENTER
LSA_VOID HIF_ENTER( LSA_VOID )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_ENTER()");
	PSI_ENTER_REENTRANCE_LOCK( hif_enter_exit_id );
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_ENTER()");
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_EXIT 
LSA_VOID HIF_EXIT( LSA_VOID )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_EXIT()");
	PSI_EXIT_REENTRANCE_LOCK( hif_enter_exit_id );
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_EXIT()");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_ALLOC_REENTRANCE_LOCK
LSA_VOID HIF_ALLOC_REENTRANCE_LOCK(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * lock_handle_ptr )
{
	PSI_ALLOC_REENTRANCE_LOCK( ret_val_ptr, lock_handle_ptr );
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_FREE_REENTRANCE_LOCK
LSA_VOID HIF_FREE_REENTRANCE_LOCK(
	 LSA_UINT16   *ret_val_ptr,
	 LSA_UINT16   lock_handle )
{
	PSI_FREE_REENTRANCE_LOCK( ret_val_ptr, lock_handle );
}
#else
#error "by design a function"
#endif


#ifndef HIF_ENTER_REENTRANCE_LOCK
LSA_VOID HIF_ENTER_REENTRANCE_LOCK( LSA_UINT16 lock_handle )
{
	PSI_ENTER_REENTRANCE_LOCK( lock_handle );
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_EXIT_REENTRANCE_LOCK
LSA_VOID HIF_EXIT_REENTRANCE_LOCK( LSA_UINT16 lock_handle )
{
	PSI_EXIT_REENTRANCE_LOCK( lock_handle );
}
#else
#error "by design a function"
#endif

/*===========================================================================*/
/*   Fatal error                                                             */
/*===========================================================================*/

#ifndef HIF_FATAL_ERROR
LSA_VOID HIF_FATAL_ERROR(
	LSA_UINT16               length,
	HIF_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "hif", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   Local memory                                                            */
/*===========================================================================*/

#ifndef HIF_ALLOC_LOCAL_MEM
LSA_VOID HIF_ALLOC_LOCAL_MEM(
	LSA_VOID_PTR_TYPE * ptr_ptr_mem,
	LSA_UINT32          length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};


	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_ALLOC_LOCAL_MEM() --> length(%i)", length );
	#endif

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"HIF_ALLOC_LOCAL_MEM(): invalid length 0" );
        *ptr_ptr_mem = LSA_NULL;
		HIF_FATAL( 0 );
        return;
    }

	#if (defined HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC)
	LSA_UNUSED_ARG(null_usr_id);
	*ptr_ptr_mem = malloc(length);
	#else
	PSI_ALLOC_LOCAL_MEM( ptr_ptr_mem, null_usr_id, length, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	#endif

	#ifndef HIF_ALLOC_LOCAL_MEM_WITH_NO_MEMSET
	if((*ptr_ptr_mem)!= LSA_NULL)
	{
		HIF_MEMSET( *ptr_ptr_mem, 0, length );
	}
	#endif

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_ALLOC_LOCAL_MEM(): The total function duration is %d ns",(end_count - start_count));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_ALLOC_LOCAL_MEM()");
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_FREE_LOCAL_MEM
LSA_VOID HIF_FREE_LOCAL_MEM(
	LSA_UINT16 *      ret_val_ptr,
	LSA_VOID_PTR_TYPE ptr_mem )
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_FREE_LOCAL_MEM()");
	#endif

	#if (defined HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC)
	free(ptr_mem);
	*ret_val_ptr = HIF_OK;
	#else
	PSI_FREE_LOCAL_MEM( ret_val_ptr, ptr_mem, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	#endif

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
    HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_FREE_LOCAL_MEM(): The total function duration is %d ns",(end_count - start_count));
    #else
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_FREE_LOCAL_MEM()");
	#endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   stdLib                                                                  */
/*===========================================================================*/

#ifndef HIF_MEMSET
LSA_VOID HIF_MEMSET(
	LSA_VOID_PTR_TYPE   ptr_mem,
	LSA_INT             value,
	LSA_UINT            length )
{
	PSI_MEMSET( ptr_mem, value, length );
}
#endif

#ifndef HIF_MEMCPY
LSA_VOID HIF_MEMCPY(
	LSA_VOID_PTR_TYPE   dst,
	LSA_VOID_PTR_TYPE   src,
	LSA_UINT            len )
{	
	PSI_MEMCPY( dst, src, len );

}
#endif

#ifndef HIF_MEMCMP
LSA_UINT HIF_MEMCMP(
	LSA_VOID_PTR_TYPE   pBuf1,
	LSA_VOID_PTR_TYPE   pBuf2,
	LSA_UINT            Length )
{
	return (PSI_MEMCMP( pBuf1, pBuf2, Length ));
}
#endif

#ifndef HIF_STRLEN
LSA_UINT HIF_STRLEN( LSA_VOID_PTR_TYPE str )
{
	return( PSI_STRLEN( str ) );
}
#endif

#ifndef HIF_NTOH_S
LSA_UINT16 HIF_NTOH_S( LSA_UINT16 value )
{
	return( PSI_NTOH16( value ) );
}
#endif

/*===========================================================================*/
/*   Type conversion                                                         */
/*===========================================================================*/

#ifndef HIF_LTOSHM_S
LSA_UINT16 HIF_LTOSHM_S(
	LSA_UINT16  local_short )
{
    #ifdef LSA_HOST_ENDIANESS_BIG
	{
		PSI_SWAP_U16(&local_short);
	}
    #endif
	return( local_short );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LTOSHM_L
LSA_UINT32 HIF_LTOSHM_L(
	LSA_UINT32  local_long )
{
    #ifdef LSA_HOST_ENDIANESS_BIG
	{
		PSI_SWAP_U32(&local_long);
	}
    #endif
	return( local_long );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LTOSHM_LL
LSA_UINT64 HIF_LTOSHM_LL(
	LSA_UINT64  local_dlong )
{
    #ifdef LSA_HOST_ENDIANESS_BIG
	{
		PSI_SWAP_U64(&local_dlong);
	}
    #endif
	return( local_dlong );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_SHMTOL_S
LSA_UINT16 HIF_SHMTOL_S(
	LSA_UINT16  shm_short )
{
    #ifdef LSA_HOST_ENDIANESS_BIG
	{
		PSI_SWAP_U16(&shm_short);
	}
    #endif
	return( shm_short );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_SHMTOL_L
LSA_UINT32 HIF_SHMTOL_L(
	LSA_UINT32  shm_long )
{
    #ifdef LSA_HOST_ENDIANESS_BIG
	{
		PSI_SWAP_U32(&shm_long);
	}
    #endif
	return( shm_long );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_SHMTOL_LL
LSA_UINT64 HIF_SHMTOL_LL(
	LSA_UINT64  shm_dlong )
{
    #ifdef LSA_HOST_ENDIANESS_BIG
	{
		PSI_SWAP_U64(&shm_dlong);
	}
    #endif
	return( shm_dlong );
}
#else
#error "by design a function!"
#endif


#ifndef HIF_NRT_ALLOC_TX_MEM
LSA_VOID HIF_NRT_ALLOC_TX_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	HIF_HD_ID_TYPE      hd_id )
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_NRT_ALLOC_TX_MEM() --> length(%i)", length);
	#endif

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"HIF_NRT_ALLOC_TX_MEM(): invalid length 0" );
        *mem_ptr_ptr = LSA_NULL;
		HIF_FATAL( 0 );
        return;
    }

    if ( psi_hd_is_edd_nrt_copy_if_on( hd_id ) )
    {
	    LSA_USER_ID_TYPE null_usr_id = {0};
		// allocate NRT buffer from local memory pool
		PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
    }
    else // edd nrt copy interface off
    {
 	    LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_id );

	    if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
	    {
	        LSA_USER_ID_TYPE null_usr_id = {0};
		    // allocate NRT buffer from local memory pool
		    PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
	    }
	    else // regular NRT pool handle
	    {
            #if defined (PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)
            // Here the EDD comp_id is not available. So the padding is made for all EDDx, not only for EDDI!
            if ( length < 64UL )
            {
                // padding to 4-byte-aligned length (EDDI specific bug)
                if ( length & 3UL )
                {
                    length = (LSA_UINT32)(length & (~3UL)) + 4UL;
                }
            }
            #endif
            // allocate NRT buffer from NRT memory pool
	        PSI_NRT_ALLOC_TX_MEM( mem_ptr_ptr, length, nrt_pool_handle, LSA_COMP_ID_HIF );
        }
    }

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_NRT_ALLOC_TX_MEM(): The total function duration is %d ns",(end_count - start_count));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_NRT_ALLOC_TX_MEM()");
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_NRT_ALLOC_RX_MEM
LSA_VOID HIF_NRT_ALLOC_RX_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	HIF_HD_ID_TYPE      hd_id )
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_NRT_ALLOC_RX_MEM() --> length(%i)", length);
	#endif

    if ( EDD_FRAME_BUFFER_LENGTH != length )
    {
		PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"HIF_NRT_ALLOC_RX_MEM(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *mem_ptr_ptr = LSA_NULL;
		HIF_FATAL( 0 );
        return;
    }

    if ( psi_hd_is_edd_nrt_copy_if_on( hd_id ) )
    {
	    LSA_USER_ID_TYPE null_usr_id = {0};
		// allocate NRT buffer from local memory pool
		PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
    }
    else // edd nrt copy interface off
    {
 	    LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_id );

	    if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
	    {
	        LSA_USER_ID_TYPE null_usr_id = {0};
		    // allocate NRT buffer from local memory pool
		    PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
	    }
	    else // regular NRT pool handle
	    {
            // allocate NRT buffer from NRT memory pool
	        PSI_NRT_ALLOC_RX_MEM( mem_ptr_ptr, length, nrt_pool_handle, LSA_COMP_ID_HIF );
        }
    }

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_NRT_ALLOC_RX_MEM(): The total function duration is %d ns",(end_count - start_count));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_NRT_ALLOC_RX_MEM()");
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_NRT_FREE_TX_MEM
LSA_VOID HIF_NRT_FREE_TX_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	HIF_HD_ID_TYPE      hd_id )
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_NRT_FREE_TX_MEM()");
	#endif

    if ( psi_hd_is_edd_nrt_copy_if_on( hd_id ) )
    {
		// free NRT buffer to local memory pool
		PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
    }
    else // edd nrt copy interface off
    {
 	    LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_id );

	    if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
	    {
		    // free NRT buffer to local memory pool
		    PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
	    }
	    else // regular NRT pool handle
	    {
			// free NRT buffer to NRT memory pool
	        PSI_NRT_FREE_TX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, LSA_COMP_ID_HIF );
        }
    }
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_NRT_FREE_TX_MEM(): The total function duration is %d ns",(end_count - start_count));
	#else
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_NRT_FREE_TX_MEM()");
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_NRT_FREE_RX_MEM
LSA_VOID HIF_NRT_FREE_RX_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	HIF_HD_ID_TYPE      hd_id )
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count;
	LSA_UINT64 start_count = HIF_GET_NS_TICKS();
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "HIF_NRT_FREE_RX_MEM()");
	#endif

    if ( psi_hd_is_edd_nrt_copy_if_on( hd_id ) )
    {
		// free NRT buffer to local memory pool
		PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
    }
    else // edd nrt copy interface off
    {
 	    LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_id );

	    if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
	    {
		    // free NRT buffer to local memory pool
		    PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM );
	    }
	    else // regular NRT pool handle
	    {
			// free NRT buffer to NRT memory pool
	        PSI_NRT_FREE_RX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, LSA_COMP_ID_HIF );
        }
    }

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"HIF_NRT_FREE_RX_MEM(): The total function duration is %d ns",(end_count - start_count));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<<HIF_NRT_FREE_RX_MEM()");
	#endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                 Check Shared Memory for corruption                        */
/*===========================================================================*/

#ifndef HIF_SHM_CHECK_OVERWRITE_SERIALIZE
LSA_VOID HIF_SHM_CHECK_OVERWRITE_SERIALIZE(
    LSA_UINT32           opcode,
    LSA_VOID          * ptr_ptr_rqb_dest,
    LSA_UINT32          rqb_size,
    LSA_UINT8         * help_ptr,
    const LSA_CHAR    * func)
{
    #if defined HIF_CFG_MEM_RING_CHECK_OVERWRITE
    if((LSA_UINT8*) ((LSA_UINT32)ptr_ptr_rqb_dest + rqb_size) == help_ptr)
    {
        if(hif_mem_ring_pool_check_overwrite((LSA_UINT8*)ptr_ptr_rqb_dest, func) == LSA_TRUE)
        {
            HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,"HIF_SHM_CHECK_OVERWRITE_SERIALIZE: RQB-Opcode:(%d) Start/Stop don't match at ptr(0x%08x)", opcode, ptr_ptr_rqb_dest);
            HIF_FATAL(0);
        }
    }
    else
    {
        //Fehler
        HIF_PROGRAM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "hif_mem_ring_pool_check_overwrite => Function(%s)", func);
        HIF_PROGRAM_TRACE_06(0, LSA_TRACE_LEVEL_ERROR,"HIF_SHM_CHECK_OVERWRITE_SERIALIZE: RQB-Opcode:(%d) help-ptr(0x%08x) is over start(0x%08x)+len(%d/0x%x):(0x%08x)", opcode, help_ptr, ptr_ptr_rqb_dest, rqb_size, rqb_size, ptr_ptr_rqb_dest + rqb_size);
    }
    #else //HIF_CFG_MEM_RING_CHECK_OVERWRITE
    LSA_UNUSED_ARG( opcode );
    LSA_UNUSED_ARG( ptr_ptr_rqb_dest );
    LSA_UNUSED_ARG( rqb_size );
    LSA_UNUSED_ARG( help_ptr );
    LSA_UNUSED_ARG( func );
    #endif //HIF_CFG_MEM_RING_CHECK_OVERWRITE

}
#else
#error "by design a function!"
#endif

#ifndef HIF_SHM_CHECK_OVERWRITE_DESERIALIZE
LSA_VOID HIF_SHM_CHECK_OVERWRITE_DESERIALIZE(
    LSA_VOID          * ptr_rqb_src,
    LSA_UINT32          opcode,
    LSA_UINT8         * help_ptr,
    const LSA_CHAR    * func)
{
    #if defined HIF_CFG_MEM_RING_CHECK_OVERWRITE
    LSA_UNUSED_ARG( help_ptr );
    if(hif_mem_ring_pool_check_overwrite((LSA_UINT8*)ptr_rqb_src, func) == LSA_TRUE)
    {
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,"HIF_SHM_CHECK_OVERWRITE_DESERIALIZE: RQB-Opcode:(%d) Start/Stop don't match at ptr(0x%08x)", opcode, ptr_rqb_src);
        HIF_FATAL(0);
    }
    #else //HIF_CFG_MEM_RING_CHECK_OVERWRITE
    LSA_UNUSED_ARG( ptr_rqb_src );
    LSA_UNUSED_ARG( opcode );
    LSA_UNUSED_ARG( help_ptr );
    LSA_UNUSED_ARG( func );
    #endif //HIF_CFG_MEM_RING_CHECK_OVERWRITE
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                           Common                                          */
/*===========================================================================*/

#if ( HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
#ifndef HIF_REQUEST_LOWER_DONE
LSA_VOID HIF_REQUEST_LOWER_DONE(
    HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_lower_done_ptr,
    HIF_RQB_PTR_TYPE                ptr_lower_rqb, 
    LSA_SYS_PTR_TYPE                hSysDev )
{
	LSA_UNUSED_ARG( hSysDev );

	hif_request_lower_done_ptr( ptr_lower_rqb ); // direct call
}
#else
#error "by design a function!"
#endif
#endif

#if ( HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
#ifndef HIF_SYS_REQUEST_LOWER_DONE
LSA_VOID HIF_SYS_REQUEST_LOWER_DONE(
    HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_request_lower_done_ptr,
    HIF_RQB_PTR_TYPE                 ptr_lower_rqb, 
    LSA_SYS_PTR_TYPE                 hSysDev )
{
	LSA_UNUSED_ARG( hSysDev );

	hif_request_lower_done_ptr( ptr_lower_rqb ); // direct call
}
#else
#error "by design a function!"
#endif
#endif

/*===========================================================================*/
/*                           Logical Device (LD)                             */
/*===========================================================================*/

#ifndef HIF_LD_DO_POLL_REQUEST
LSA_VOID HIF_LD_DO_POLL_REQUEST(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
	psi_request_local( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_request, ptr_upper_rqb );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_DO_TIMEOUT_REQUEST
LSA_VOID HIF_LD_DO_TIMEOUT_REQUEST(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
	psi_request_local( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_request, ptr_upper_rqb );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_DO_REQUEST_LOCAL
LSA_VOID HIF_LD_DO_REQUEST_LOCAL(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
	psi_request_local( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_request, ptr_upper_rqb );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_ENTER

LSA_VOID HIF_LD_ENTER( LSA_VOID )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_LD_ENTER()");
	PSI_ENTER_REENTRANCE_LOCK( hif_ld_enter_exit_id );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_LD_ENTER()");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_EXIT
LSA_VOID HIF_LD_EXIT( LSA_VOID )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_LD_EXIT()");
    PSI_EXIT_REENTRANCE_LOCK( hif_ld_enter_exit_id );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_LD_EXIT()");
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_U_GET_PARAMS
LSA_VOID HIF_LD_U_GET_PARAMS(
	LSA_UINT16              *ret_val_ptr,
	HIF_SYS_HANDLE          hSysDev,
	HIF_GET_PAR_PTR_TYPE    pPars )
{
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;

	HIF_ASSERT( pPars       != LSA_NULL );
	HIF_ASSERT( ret_val_ptr != LSA_NULL );

	#ifdef HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT
	pPars->if_pars.hif_lower_device         = pSys->hif_lower_handle;
	pPars->Mode                             = HIF_TM_SHARED_MEM;
	pPars->if_pars.SharedMem.bUseTimerPoll  = LSA_TRUE;                   
	pPars->if_pars.SharedMem.Local.Base     = (LSA_UINT8*)(g_HifLocalUSharedMem[pSys->hd_nr][0]); 
	pPars->if_pars.SharedMem.Local.Size     = sizeof(g_HifLocalUSharedMem[pSys->hd_nr][0]);     
	pPars->if_pars.SharedMem.Remote.Base    = (LSA_UINT8*)(g_HifRemoteUSharedMem[pSys->hd_nr][0]);
	pPars->if_pars.SharedMem.Remote.Size    = sizeof(g_HifRemoteUSharedMem[pSys->hd_nr][0]);
	#else
	if ( pSys->shortcut_mode ) // Upper/Lower shortcut ?
	{
		pPars->if_pars.hif_lower_device = pSys->hif_lower_handle;
		pPars->Mode = HIF_TM_SHORT;
	}
	else
	{
		// Connect to real LD Upper IF
		PSI_HIF_LD_U_GET_PARAMS( ret_val_ptr, hSysDev, pPars );
	}
    #endif
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_L_GET_PARAMS
LSA_VOID HIF_LD_L_GET_PARAMS(
	LSA_UINT16              *ret_val_ptr,
	HIF_SYS_HANDLE          hSysDev,
	HIF_GET_PAR_PTR_TYPE    pPars )
{
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;

	HIF_ASSERT( pPars       != LSA_NULL );
	HIF_ASSERT( ret_val_ptr != LSA_NULL );

	#if (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)       
	pPars->if_pars.hif_lower_device         = 0; // sanity
	pPars->Mode                             = HIF_TM_SHARED_MEM;    
	pPars->if_pars.SharedMem.bUseTimerPoll  = LSA_TRUE;                   
    pPars->if_pars.SharedMem.Local.Base     = (LSA_UINT8*)(g_HifRemoteUSharedMem[pSys->hd_nr][0]);
	pPars->if_pars.SharedMem.Local.Size     = sizeof(g_HifRemoteUSharedMem[pSys->hd_nr][0]);   
	pPars->if_pars.SharedMem.Remote.Base    = (LSA_UINT8*)(g_HifLocalUSharedMem[pSys->hd_nr][0]); 
	pPars->if_pars.SharedMem.Remote.Size    = sizeof(g_HifLocalUSharedMem[pSys->hd_nr][0]);  
    #else
	if ( pSys->shortcut_mode ) // Upper/Lower shortcut ?
	{
		pPars->if_pars.hif_lower_device = 0; // sanity
		pPars->Mode = HIF_TM_SHORT;
	}
	else
	{
		// Connect to real LD lower IF
		PSI_HIF_LD_L_GET_PARAMS( ret_val_ptr, hSysDev, pPars );
	}
    #endif

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_U_RELEASE_PATH_INFO
LSA_VOID HIF_LD_U_RELEASE_PATH_INFO(
	LSA_UINT16	            *ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	HIF_DETAIL_PTR_TYPE     detail_ptr )
{
	*ret_val_ptr = psi_hif_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_L_RELEASE_PATH_INFO
LSA_VOID HIF_LD_L_RELEASE_PATH_INFO(
	LSA_UINT16	            *ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	HIF_DETAIL_PTR_TYPE     detail_ptr )
{
	// Note: unused function, because no channel from LD lower to upper
	// FATAL is set, if called 
	LSA_UNUSED_ARG( sys_ptr );
	LSA_UNUSED_ARG( ret_val_ptr );
	LSA_UNUSED_ARG( detail_ptr );

	HIF_FATAL( 0 );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_U_GET_PATH_INFO
LSA_VOID HIF_LD_U_GET_PATH_INFO(
	LSA_UINT16            *ret_val_ptr,
	LSA_SYS_PTR_TYPE      *sys_ptr_ptr,
	HIF_DETAIL_PTR_TYPE   *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE     path )
{
	*ret_val_ptr = psi_hif_ld_upper_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE*)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_L_GET_PATH_INFO
LSA_VOID HIF_LD_L_GET_PATH_INFO(
	LSA_UINT16              *ret_val_ptr,
	LSA_SYS_PTR_TYPE	    *sys_ptr_ptr,
	HIF_DETAIL_PTR_TYPE	    *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       path )
{
	// Note: unused function, because no channel from LD lower to upper
	// FATAL is set, if called 
	LSA_UNUSED_ARG( sys_ptr_ptr );
	LSA_UNUSED_ARG( ret_val_ptr );
	LSA_UNUSED_ARG( path );
	LSA_UNUSED_ARG( detail_ptr_ptr );

	HIF_FATAL( 0 );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_DO_INTERRUPT_REQUEST
LSA_VOID HIF_LD_DO_INTERRUPT_REQUEST(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
	psi_request_local( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_request, ptr_upper_rqb );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_ENABLE_ISR
LSA_VOID HIF_LD_ENABLE_ISR(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{	 
	#if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	PSI_HIF_LD_ENABLE_ISR( hSysDev, hH );	
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_DISABLE_ISR
LSA_VOID HIF_LD_DISABLE_ISR(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{
	#if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	PSI_HIF_LD_DISABLE_ISR( hSysDev, hH );
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_SEND_IRQ
LSA_VOID HIF_LD_SEND_IRQ(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{
	#if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	PSI_HIF_LD_SEND_IRQ( hSysDev, hH );
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );
	hif_poll();
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef  HIF_LD_ACK_IRQ
LSA_VOID HIF_LD_ACK_IRQ(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{
	#if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	PSI_HIF_LD_ACK_IRQ( hSysDev, hH );
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );
	#endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_OPEN_DEVICE_LOWER 
LSA_VOID HIF_LD_OPEN_DEVICE_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr )
{
	// Respond HIF RQB without posting to PSI
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE cbf = lower_rqb_ptr->args.dev_ld_open.Cbf;

	HIF_RQB_SET_RESPONSE( lower_rqb_ptr, HIF_OK );

	psi_system_request_done( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)cbf, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_CLOSE_DEVICE_LOWER 
LSA_VOID HIF_LD_CLOSE_DEVICE_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr )
{
	// Respond HIF RQB without posting to PSI
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE cbf = lower_rqb_ptr->args.dev_close.Cbf;

	HIF_RQB_SET_RESPONSE( lower_rqb_ptr, HIF_OK );

	psi_system_request_done( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)cbf, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_SYSTEM_REQUEST_LOWER 
LSA_VOID HIF_LD_SYSTEM_REQUEST_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr )
{
	// post to PSI LD
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_system, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_OPEN_CHANNEL_LOWER 
LSA_VOID HIF_LD_OPEN_CHANNEL_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr,
	HIF_SYS_HANDLE   hSysDev )
{
	LSA_UNUSED_ARG( hSysDev );

	psi_ld_open_channel( (PSI_RQB_PTR_TYPE)lower_rqb_ptr ); // direct call
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_CLOSE_CHANNEL_LOWER 
LSA_VOID HIF_LD_CLOSE_CHANNEL_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr,
	HIF_SYS_HANDLE   hSysDev )
{
	LSA_UNUSED_ARG( hSysDev );

	psi_ld_close_channel( (PSI_RQB_PTR_TYPE)lower_rqb_ptr ); /* direct call */
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_LD_REQUEST_LOWER 
LSA_VOID HIF_LD_REQUEST_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr,
	HIF_SYS_HANDLE   hSysDev )
{
	LSA_UNUSED_ARG( hSysDev );

	psi_ld_request( (PSI_RQB_PTR_TYPE)lower_rqb_ptr );  /* direct call */
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_REQUEST_UPPER_DONE 
LSA_VOID HIF_LD_REQUEST_UPPER_DONE(
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_upper_done_ptr,
	HIF_RQB_PTR_TYPE                ptr_upper_rqb, 
	LSA_SYS_PTR_TYPE                sys_ptr )
{
    PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE_LOW, "HIF_LD_REQUEST_UPPER_DONE( hif_request_upper_done_ptr=0x%08x, ptr_upper_rqb=0x%08x, sys_ptr=0x%08x )", 
        hif_request_upper_done_ptr, ptr_upper_rqb, sys_ptr);

	PSI_HIF_LD_REQUEST_UPPER_DONE( (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)hif_request_upper_done_ptr, ptr_upper_rqb, sys_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_L_SYSTEM_REQUEST_DONE 
LSA_VOID HIF_LD_L_SYSTEM_REQUEST_DONE(
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
	HIF_RQB_PTR_TYPE                 ptr_upper_rqb )
{
	// Note: must be connect to APP (shortcut mode) or to LD FW system 
	PSI_HIF_LD_L_SYSTEM_REQUEST_DONE( (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)hif_system_request_done_ptr, ptr_upper_rqb );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_LD_U_SYSTEM_REQUEST_DONE 
LSA_VOID HIF_LD_U_SYSTEM_REQUEST_DONE(
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
	HIF_RQB_PTR_TYPE                 ptr_upper_rqb )
{
	PSI_HIF_LD_U_SYSTEM_REQUEST_DONE( (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)hif_system_request_done_ptr, ptr_upper_rqb );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                           Hardware Device (HD)                            */
/*===========================================================================*/

#ifndef HIF_HD_DO_POLL_REQUEST
LSA_VOID HIF_HD_DO_POLL_REQUEST(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_DO_POLL_REQUEST");
	psi_request_local( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, ptr_upper_rqb );
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_DO_POLL_REQUEST");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_DO_TIMEOUT_REQUEST
LSA_VOID HIF_HD_DO_TIMEOUT_REQUEST(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_DO_TIMEOUT_REQUEST");
	psi_request_local( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, ptr_upper_rqb );
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_DO_TIMEOUT_REQUEST");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_DO_REQUEST_LOCAL
LSA_VOID HIF_HD_DO_REQUEST_LOCAL(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_DO_REQUEST_LOCAL");
	psi_request_local( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, ptr_upper_rqb );
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_DO_REQUEST_LOCAL");
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_HD_ENTER

LSA_VOID HIF_HD_ENTER( LSA_VOID )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_ENTER()");
    PSI_ENTER_REENTRANCE_LOCK( hif_hd_enter_exit_id );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_ENTER()");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_EXIT
LSA_VOID HIF_HD_EXIT( LSA_VOID )
{
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_ENTER()");
    PSI_EXIT_REENTRANCE_LOCK( hif_hd_enter_exit_id );
    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_ENTER()");
}
#else
#error "by design a function!"
#endif


#ifndef HIF_HD_U_GET_PARAMS
LSA_VOID HIF_HD_U_GET_PARAMS(
	LSA_UINT16              *ret_val_ptr,
	HIF_SYS_HANDLE          hSysDev,
	HIF_GET_PAR_PTR_TYPE    pPars )
{
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;

	HIF_ASSERT( pPars       != LSA_NULL );
	HIF_ASSERT( ret_val_ptr != LSA_NULL );

	#if defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
    pPars->if_pars.hif_lower_device         = pSys->hif_lower_handle;
	pPars->Mode                             = HIF_TM_SHARED_MEM;
	pPars->if_pars.SharedMem.bUseTimerPoll  = LSA_TRUE;                   
    pPars->if_pars.SharedMem.Local.Base     = (LSA_UINT8*)(g_HifLocalUSharedMem[pSys->hd_nr][1]); 
	pPars->if_pars.SharedMem.Local.Size     = sizeof(g_HifLocalUSharedMem[pSys->hd_nr][1]);     
	pPars->if_pars.SharedMem.Remote.Base    = (LSA_UINT8*)(g_HifRemoteUSharedMem[pSys->hd_nr][1]);
	pPars->if_pars.SharedMem.Remote.Size    = sizeof(g_HifRemoteUSharedMem[pSys->hd_nr][1]);
    #else
	if ( pSys->shortcut_mode ) // Upper/Lower shortcut ?
	{
	    pPars->if_pars.hif_lower_device     = pSys->hif_lower_handle;
        pPars->Mode                         = HIF_TM_SHORT;
	}
	else
	{
		// Connect to real HD Upper IF
		PSI_HIF_HD_U_GET_PARAMS( ret_val_ptr, hSysDev, pPars );
	}
    #endif
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_L_GET_PARAMS
LSA_VOID HIF_HD_L_GET_PARAMS(
	LSA_UINT16              *ret_val_ptr,
	HIF_SYS_HANDLE          hSysDev,
	HIF_GET_PAR_PTR_TYPE    pPars )
{
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;

	HIF_ASSERT( pPars       != LSA_NULL );
	HIF_ASSERT( ret_val_ptr != LSA_NULL );

	#if defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
    pPars->if_pars.hif_lower_device         = 0; // sanity     
	pPars->Mode                             = HIF_TM_SHARED_MEM;
	pPars->if_pars.SharedMem.bUseTimerPoll  = LSA_TRUE;                   
    pPars->if_pars.SharedMem.Local.Base     = (LSA_UINT8*)(g_HifRemoteUSharedMem[pSys->hd_nr][1]);
	pPars->if_pars.SharedMem.Local.Size     = sizeof(g_HifRemoteUSharedMem[pSys->hd_nr][1]);   
	pPars->if_pars.SharedMem.Remote.Base    = (LSA_UINT8*)(g_HifLocalUSharedMem[pSys->hd_nr][1]); 
	pPars->if_pars.SharedMem.Remote.Size    = sizeof(g_HifLocalUSharedMem[pSys->hd_nr][1]);  
    #else
	if ( pSys->shortcut_mode ) // Upper/Lower shortcut ?
	{
        pPars->if_pars.hif_lower_device     = 0; // sanity    
		pPars->Mode                         = HIF_TM_SHORT;
	}
	else
	{
		// Connect to real HD IF
		PSI_HIF_HD_L_GET_PARAMS( ret_val_ptr, hSysDev, pPars );
	}
    #endif
    
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef HIF_HD_U_RELEASE_PATH_INFO
LSA_VOID HIF_HD_U_RELEASE_PATH_INFO(
	LSA_UINT16             *ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	HIF_DETAIL_PTR_TYPE     detail_ptr )
{
	*ret_val_ptr = psi_hif_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_L_RELEASE_PATH_INFO
LSA_VOID HIF_HD_L_RELEASE_PATH_INFO(
	LSA_UINT16             *ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	HIF_DETAIL_PTR_TYPE     detail_ptr )
{
	*ret_val_ptr = psi_hif_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_U_GET_PATH_INFO
LSA_VOID HIF_HD_U_GET_PATH_INFO(
	LSA_UINT16           *ret_val_ptr,
	LSA_SYS_PTR_TYPE     *sys_ptr_ptr,
	HIF_DETAIL_PTR_TYPE  *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE    path )
{
	*ret_val_ptr = psi_hif_hd_upper_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE*)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_L_GET_PATH_INFO
LSA_VOID HIF_HD_L_GET_PATH_INFO(
	LSA_UINT16           *ret_val_ptr,
	LSA_SYS_PTR_TYPE     *sys_ptr_ptr,
	HIF_DETAIL_PTR_TYPE  *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE    path )
{
	*ret_val_ptr = psi_hif_hd_lower_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE*)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_DO_INTERRUPT_REQUEST
LSA_VOID HIF_HD_DO_INTERRUPT_REQUEST(
	HIF_RQB_PTR_TYPE ptr_upper_rqb )
{
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_DO_INTERRUPT_REQUEST");
	psi_request_local( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, ptr_upper_rqb );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_DO_INTERRUPT_REQUEST");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_ENABLE_ISR
LSA_VOID HIF_HD_ENABLE_ISR(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{
	#if !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
	PSI_HIF_HD_ENABLE_ISR( hSysDev, hH );
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_DISABLE_ISR
LSA_VOID HIF_HD_DISABLE_ISR(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{
	#if !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
	PSI_HIF_HD_DISABLE_ISR( hSysDev, hH );
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_SEND_IRQ
LSA_VOID HIF_HD_SEND_IRQ(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>> HIF_HD_SEND_IRQ");
	#if !defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT && !defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT
	PSI_HIF_HD_SEND_IRQ( hSysDev, hH );
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );

    hif_poll(); 
	#endif
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<< HIF_HD_SEND_IRQ");
}
#else
#error "by design a function!"
#endif

#ifndef  HIF_HD_ACK_IRQ
LSA_VOID HIF_HD_ACK_IRQ(
	HIF_SYS_HANDLE hSysDev,
	HIF_HANDLE     hH )
{
	#if !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
	PSI_HIF_HD_ACK_IRQ( hSysDev, hH );
	#else
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hH );
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_OPEN_DEVICE_LOWER 
LSA_VOID HIF_HD_OPEN_DEVICE_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr )
{
	// Respond HIF RQB without posting to PSI
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE cbf = lower_rqb_ptr->args.dev_hd_open.Cbf;

	HIF_RQB_SET_RESPONSE( lower_rqb_ptr, HIF_OK );

	psi_system_request_done( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)cbf, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_CLOSE_DEVICE_LOWER 
LSA_VOID HIF_HD_CLOSE_DEVICE_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr )
{
	// Respond HIF RQB without posting to PSI
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE cbf = lower_rqb_ptr->args.dev_close.Cbf;

	HIF_RQB_SET_RESPONSE( lower_rqb_ptr, HIF_OK );

	psi_system_request_done( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)cbf, lower_rqb_ptr );

}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_SYSTEM_REQUEST_LOWER 
LSA_VOID HIF_HD_SYSTEM_REQUEST_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr )
{
	// post to PSI HD
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_system, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_OPEN_CHANNEL_LOWER 
LSA_VOID HIF_HD_OPEN_CHANNEL_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr,
	HIF_SYS_HANDLE   hSysDev )
{
	LSA_UNUSED_ARG( hSysDev );

	psi_hd_open_channel( (PSI_RQB_PTR_TYPE)lower_rqb_ptr );  /* direct call */
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_CLOSE_CHANNEL_LOWER 
LSA_VOID HIF_HD_CLOSE_CHANNEL_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr,
	HIF_SYS_HANDLE   hSysDev )
{
	// post to PSI HD
	LSA_UNUSED_ARG( hSysDev );

	psi_hd_close_channel( (PSI_RQB_PTR_TYPE)lower_rqb_ptr ); /* direct call */
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_REQUEST_LOWER 
LSA_VOID HIF_HD_REQUEST_LOWER(
	HIF_RQB_PTR_TYPE lower_rqb_ptr,
	HIF_SYS_HANDLE   hSysDev )
{
	// post to PSI HD
	LSA_UNUSED_ARG( hSysDev );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_REQUEST_LOWER");
	psi_hd_request( (PSI_RQB_PTR_TYPE)lower_rqb_ptr ); /* direct call */
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_REQUEST_LOWER");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_REQUEST_UPPER_DONE 
LSA_VOID HIF_HD_REQUEST_UPPER_DONE(
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_upper_done_ptr,
	HIF_RQB_PTR_TYPE                ptr_upper_rqb, 
	LSA_SYS_PTR_TYPE                sys_ptr )
{
    PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE_LOW, "HIF_HD_REQUEST_UPPER_DONE( hif_request_upper_done_ptr=0x%08x, ptr_upper_rqb=0x%08x, sys_ptr=0x%08x )", 
        hif_request_upper_done_ptr, ptr_upper_rqb, sys_ptr);

	// Note: posting is done based on RSP-MBX_ID set in sys_ptr (see GET_SYS_PATH)
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>>HIF_HD_REQUEST_UPPER_DONE");
	psi_request_done( hif_request_upper_done_ptr, ptr_upper_rqb, sys_ptr );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<<HIF_HD_REQUEST_UPPER_DONE");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_L_SYSTEM_REQUEST_DONE 
LSA_VOID HIF_HD_L_SYSTEM_REQUEST_DONE(
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
	HIF_RQB_PTR_TYPE                 ptr_upper_rqb )
{
	// Note: must be connect to PSI LD (shortcut mode) or to HD FW system 
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>> HIF_HD_L_SYSTEM_REQUEST_DONE");
	PSI_HIF_HD_L_SYSTEM_REQUEST_DONE( (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)hif_system_request_done_ptr, ptr_upper_rqb );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<< HIF_HD_L_SYSTEM_REQUEST_DONE");
}
#else
#error "by design a function!"
#endif

#ifndef HIF_HD_U_SYSTEM_REQUEST_DONE 
LSA_VOID HIF_HD_U_SYSTEM_REQUEST_DONE(
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
	HIF_RQB_PTR_TYPE                 ptr_upper_rqb )
{
	// post to PSI LD (called at HD OPEN/CLOSE UPPER)
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>>HIF_HD_U_SYSTEM_REQUEST_DONE");
	psi_system_request_done( PSI_MBX_ID_PSI, hif_system_request_done_ptr, ptr_upper_rqb );
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<<HIF_HD_U_SYSTEM_REQUEST_DONE");
}
#else
#error "by design a function!"
#endif


#ifndef HIF_SER_SYS_PATH_HTSHM
LSA_VOID HIF_SER_SYS_PATH_HTSHM(
		LSA_UINT32** dest,
		LSA_SYS_PATH_TYPE sys_path
)
{
	HIF_SER_COPY_HTSHM_16(dest,sys_path);
}
#else
# error "by design a function!"
#endif

#ifndef HIF_SER_SYS_PATH_SHMTH
LSA_VOID HIF_SER_SYS_PATH_SHMTH(
		LSA_SYS_PATH_TYPE* sys_path,
		LSA_UINT32** src		
)
{
    HIF_SER_COPY_SHMTH_16(sys_path,src);
}
#else
# error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#else
#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("***** not compiling PSI HIF support (PSI_CFG_USE_HIF=" PSI_STRINGIFY(PSI_CFG_USE_HIF) ")")
#endif
#endif // PSI_CFG_USE_HIF

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
