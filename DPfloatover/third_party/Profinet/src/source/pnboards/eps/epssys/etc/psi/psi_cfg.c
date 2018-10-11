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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of LSA-component PSI.                      */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      32099
#define LTRC_ACT_MODUL_ID  32099

#include "eps_rtos.h"
#include "psi_inc.h"
#include "psi_trc.h"
#include "eps_sys.h"
#include "eps_locks.h"
#include "eps_mem.h"
#include "eps_cp_mem.h"
#include "eps_cp_hw.h"
#include "eps_events.h"
#include "eps_isr.h"

#include "psi_cfg.h"

#include "eps_plf.h"
#include <eps_app.h>           /* EPS APP prototypes */
#include "eps_tasks.h"
#include "eps_hif_drv_if.h"

#include "eps_timer.h"
#include "string.h"
#include "math.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif


PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

#define EPS_IR_LOCK_MAGIC 0x5454

/*----------------------------------------------------------------------------*/

int psi_semicolon_missing = 0;

/*---------------------------------------------------------------------------*/

LSA_EXTERN LSA_INT eps_vsprintf (LSA_UINT8 *buffer, const char *format, va_list argptr);

/*===========================================================================*/
/*                 Timer                                                     */
/*===========================================================================*/

#ifndef PSI_ALLOC_TIMER
LSA_VOID PSI_ALLOC_TIMER_TGROUP0(
	LSA_UINT16          * ret_val_ptr,
	LSA_TIMER_ID_TYPE   * timer_id_ptr,
	LSA_UINT16          timer_type,
	LSA_UINT16          time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout )
{
   *ret_val_ptr = eps_timer_allocate_tgroup0(timer_id_ptr, timer_type, time_base,  psi_timeout);
}

LSA_VOID PSI_ALLOC_TIMER_TGROUP1(
	LSA_UINT16          * ret_val_ptr,
	LSA_TIMER_ID_TYPE   * timer_id_ptr,
	LSA_UINT16          timer_type,
	LSA_UINT16          time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout )
{
   *ret_val_ptr = eps_timer_allocate_tgroup1(timer_id_ptr, timer_type, time_base,  psi_timeout);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_START_TIMER
LSA_VOID  PSI_START_TIMER_TGROUP0(
	LSA_UINT16          *ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id,
	LSA_USER_ID_TYPE     user_id,
	LSA_UINT16           time )
{	
	*ret_val_ptr = eps_timer_start_tgroup0(timer_id, user_id, time);
}

LSA_VOID  PSI_START_TIMER_TGROUP1(
	LSA_UINT16          *ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id,
	LSA_USER_ID_TYPE     user_id,
	LSA_UINT16           time )
{	
	*ret_val_ptr = eps_timer_start_tgroup1(timer_id, user_id, time);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_STOP_TIMER
LSA_VOID  PSI_STOP_TIMER_TGROUP0(
	LSA_UINT16           *ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id )
{
	*ret_val_ptr = eps_timer_stop_tgroup0(timer_id);
}

LSA_VOID  PSI_STOP_TIMER_TGROUP1(
	LSA_UINT16           *ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id )
{
	*ret_val_ptr = eps_timer_stop_tgroup1(timer_id);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_TIMER
LSA_VOID  PSI_FREE_TIMER_TGROUP0(
	LSA_UINT16           *ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id )
{
	*ret_val_ptr = eps_timer_free_tgroup0(timer_id);
}

LSA_VOID  PSI_FREE_TIMER_TGROUP1(
	LSA_UINT16           *ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id )
{
	*ret_val_ptr = eps_timer_free_tgroup1(timer_id);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
/*  Ticks count                                                               */
/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_TICKS_1MS
LSA_UINT32 PSI_GET_TICKS_1MS( LSA_VOID )
{
    int            ret_val;
    LSA_UINT32     ticks_1ms;
    struct timeval my_time;

    ret_val = EPS_POSIX_GETTIMEOFDAY(&my_time, NULL);
	PSI_ASSERT(0 == ret_val);

    ticks_1ms  = (my_time.tv_sec  * 1000);
    ticks_1ms += (my_time.tv_usec / 1000);

    return ticks_1ms;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_TICKS_100NS
LSA_UINT64 PSI_GET_TICKS_100NS( LSA_VOID )
{
	LSA_UINT64      ticks_100ns;

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
    LSA_UINT32 val_l,val_h;
    // read TTU-timer   -> 56 bit timer
    val_l = *((uint32_t*)(0x16000000 + 0x20));      ///< read low  32 bit (offset 0x20)
    val_h = *((uint32_t*)(0x16000000 + 0x28));      ///< read high 24 bit (offset 0x28)
    ticks_100ns= (val_h*1000000 + val_l)*10;
#else
	struct timespec my_time;
    int             ret_val;
	
    ret_val = EPS_POSIX_CLOCK_GETTIME( CLOCK_REALTIME, &my_time );
	PSI_ASSERT(0 == ret_val);

	ticks_100ns  = ((LSA_UINT64)my_time.tv_sec  * (LSA_UINT64)10000000);
    ticks_100ns += ((LSA_UINT64)my_time.tv_nsec / (LSA_UINT64)100);
#endif

	return ticks_100ns;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                 Reentrance Locks                                          */
/*===========================================================================*/

#ifndef PSI_ALLOC_REENTRANCE_LOCK
LSA_VOID PSI_ALLOC_REENTRANCE_LOCK(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * lock_handle_ptr )
{
	*ret_val_ptr = eps_alloc_critical_section( lock_handle_ptr, LSA_FALSE );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_REENTRANCE_LOCK
LSA_VOID PSI_FREE_REENTRANCE_LOCK(
	 LSA_UINT16   *ret_val_ptr,
	 LSA_UINT16   lock_handle )
{
	*ret_val_ptr = eps_free_critical_section( lock_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_ENTER_REENTRANCE_LOCK
LSA_VOID PSI_ENTER_REENTRANCE_LOCK( LSA_UINT16 lock_handle )
{
	eps_enter_critical_section( lock_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EXIT_REENTRANCE_LOCK
LSA_VOID PSI_EXIT_REENTRANCE_LOCK( LSA_UINT16 lock_handle )
{
	eps_exit_critical_section( lock_handle );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                     IR Locks                                              */
/*===========================================================================*/
/* Note: no IR lock avaliable on WIN, using reentrance                        */

#ifndef PSI_ALLOC_IR_LOCK
LSA_VOID PSI_ALLOC_IR_LOCK(
	LSA_UINT16   * ret_val_ptr,
	LSA_UINT16   * lock_handle_ptr )
{
	*lock_handle_ptr = EPS_IR_LOCK_MAGIC;
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_IR_LOCK
LSA_VOID PSI_FREE_IR_LOCK(
	 LSA_UINT16   *ret_val_ptr,
	 LSA_UINT16   lock_handle )
{
	EPS_ASSERT(lock_handle == EPS_IR_LOCK_MAGIC);
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_ENTER_IR_LOCK
LSA_VOID PSI_ENTER_IR_LOCK( LSA_UINT16 lock_handle )
{
	EPS_ASSERT(lock_handle == EPS_IR_LOCK_MAGIC);
	EPS_PLF_DISABLE_INTERRUPTS();
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EXIT_IR_LOCK
LSA_VOID PSI_EXIT_IR_LOCK( LSA_UINT16 lock_handle )
{
	EPS_ASSERT(lock_handle == EPS_IR_LOCK_MAGIC);
	EPS_PLF_ENABLE_INTERRUPTS();
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
/* Blocking Waits                                                             */
/*----------------------------------------------------------------------------*/

typedef struct psi_blocking_wait_type
{
	LSA_UINT16         bw_handle;
} PSI_BLOCKING_WAIT_TYPE, *PSI_BLOCKING_WAIT_PTR_TYPE;

#ifndef PSI_ALLOC_BLOCKING_WAIT
LSA_VOID PSI_ALLOC_BLOCKING_WAIT(
	LSA_UINT16         *ret_val_ptr,
	LSA_VOID_PTR_TYPE  bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;
	PSI_ASSERT( p_wait_ptr );

	*ret_val_ptr = eps_alloc_event( &p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_BLOCKING_WAIT
LSA_VOID PSI_FREE_BLOCKING_WAIT(
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE    bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;

	*ret_val_ptr = eps_free_event( p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_SET_BLOCKING_WAIT
LSA_VOID PSI_SET_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;

	eps_reset_event( p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_RESET_BLOCKING_WAIT
LSA_VOID PSI_RESET_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;

	eps_set_event( p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_WAIT_BLOCKING_WAIT
LSA_VOID PSI_WAIT_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;

	eps_wait_event( p_wait_ptr->bw_handle, PSI_EVENT_NO_TIMEOUT );  // wait forever
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_WAIT_NS
LSA_VOID PSI_WAIT_NS(
    LSA_UINT64 uTimeNs )
{
    eps_tasks_nswait( uTimeNs );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                         Local memory                                      */
/*===========================================================================*/

#ifndef PSI_ALLOC_LOCAL_MEM
LSA_VOID PSI_ALLOC_LOCAL_MEM(
	LSA_VOID_PTR_TYPE     *mem_ptr_ptr,
	LSA_USER_ID_TYPE      user_id,
	LSA_UINT32            length,
	LSA_SYS_PTR_TYPE      sys_ptr,
	LSA_UINT16            comp_id,
	LSA_UINT16            mem_type )
{
    LSA_UNUSED_ARG(user_id);
    LSA_UNUSED_ARG(sys_ptr);

    *mem_ptr_ptr = eps_mem_alloc(length, comp_id, mem_type);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_LOCAL_MEM
LSA_VOID PSI_FREE_LOCAL_MEM(
	LSA_UINT16            *ret_val_ptr,
	LSA_VOID_PTR_TYPE     mem_ptr,
	LSA_SYS_PTR_TYPE      sys_ptr,
	LSA_UINT16            comp_id,
	LSA_UINT16            mem_type )
{
    LSA_UNUSED_ARG(sys_ptr);

    *ret_val_ptr = eps_mem_free(mem_ptr, comp_id, mem_type);
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                     HIF memory pool                                       */
/*===========================================================================*/

#ifndef PSI_CREATE_HIF_MEM_POOL
LSA_VOID PSI_CREATE_HIF_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT*            pool_handle_ptr )
{
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );

    #if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	eps_hif_drv_if_mem_pool_create(pSys->hd_nr, pSys->is_upper, pool_handle_ptr, basePtr, length);
    #else
    *pool_handle_ptr = eps_cp_mem_create_hif_pool(pSys->hd_nr, (LSA_UINT8*)basePtr, length);
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_HIF_MEM_POOL
LSA_VOID PSI_DELETE_HIF_MEM_POOL(
	LSA_UINT16         *ret_val_ptr,
	PSI_SYS_HANDLE     sys_handle,
	LSA_INT            pool_handle )
{
    #if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	LSA_UNUSED_ARG(sys_handle);

	eps_hif_drv_if_mem_pool_free(pool_handle, ret_val_ptr);
    #else
    PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );

    eps_cp_mem_delete_hif_pool(pSys->hd_nr, pool_handle);
    *ret_val_ptr = HIF_OK;
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef PSI_HIF_ALLOC_MEM
LSA_VOID PSI_HIF_ALLOC_MEM(
	LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT             pool_handle )
{
    #if !defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT && !defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
	LSA_UNUSED_ARG(sys_handle);
    eps_hif_drv_if_alloc_shm(pool_handle, mem_ptr_ptr, length);
    #else
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );
	*mem_ptr_ptr = (LSA_VOID_PTR_TYPE)eps_cp_mem_hif_alloc(length, pool_handle, pSys->is_upper, LSA_COMP_ID_HIF);
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef PSI_HIF_FREE_MEM
LSA_VOID PSI_HIF_FREE_MEM(
	LSA_UINT16           *ret_val_ptr,
	LSA_VOID_PTR_TYPE    mem_ptr,
	PSI_SYS_HANDLE       sys_handle,
	LSA_INT              pool_handle )
{
    #if !defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT && !defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
	LSA_UNUSED_ARG(sys_handle);
    eps_hif_drv_if_free_shm(pool_handle, ret_val_ptr, mem_ptr);
    #else
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );
    eps_cp_mem_hif_free((LSA_UINT8*)mem_ptr, pool_handle, pSys->is_upper, LSA_COMP_ID_HIF);
    *ret_val_ptr = HIF_OK;
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef PSI_HIF_SHM_DMA_COPY
LSA_VOID PSI_HIF_SHM_DMA_COPY(
	LSA_VOID** ptr_ptr_rqb,
	LSA_UINT32 rqb_len,
	PSI_SYS_HANDLE sys_handle)
{
    #if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );
	eps_hif_drv_if_dma_copy(pSys->hd_nr, pSys->is_upper, ptr_ptr_rqb, rqb_len);
    #else
    LSA_UNUSED_ARG(ptr_ptr_rqb);
    LSA_UNUSED_ARG(rqb_len);
    LSA_UNUSED_ARG(sys_handle);
    #endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                     NRT memory pool                                       */
/*===========================================================================*/

#ifndef PSI_CREATE_NRT_MEM_POOL
LSA_VOID PSI_CREATE_NRT_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT*            pool_handle_ptr,
    LSA_UINT8           cp_mem_nrt)  
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );

   *pool_handle_ptr = eps_cp_mem_create_nrt_pool( pSys->hd_nr, (LSA_UINT8 *)basePtr, length, cp_mem_nrt );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_NRT_MEM_POOL
LSA_VOID PSI_DELETE_NRT_MEM_POOL(
	LSA_UINT16         *ret_val_ptr,
	PSI_SYS_HANDLE     sys_handle,
	LSA_INT            pool_handle,
    LSA_UINT8          cp_mem_nrt )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );

    eps_cp_mem_delete_nrt_pool( pSys->hd_nr, pool_handle, cp_mem_nrt );
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*
 * At the moment, there is no difference in TX and RX
 * Both are using a combined NRT memory
 * Maybe there will be a solution to differ in TX and RX 
 */

/* Marked for DELETION */
#ifndef PSI_NRT_ALLOC_MEM
LSA_VOID PSI_NRT_ALLOC_MEM(
  LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
  LSA_UINT32          length,
  LSA_INT             pool_handle,
  LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_nrt_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_ALLOC_TX_MEM
LSA_VOID PSI_NRT_ALLOC_TX_MEM(
  LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
  LSA_UINT32          length,
  LSA_INT             pool_handle,
  LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_nrt_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_ALLOC_RX_MEM
LSA_VOID PSI_NRT_ALLOC_RX_MEM(
  LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
  LSA_UINT32          length,
  LSA_INT             pool_handle,
  LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_nrt_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

/* Marked for DELETION */
#ifndef PSI_NRT_FREE_MEM
LSA_VOID PSI_NRT_FREE_MEM(
  LSA_UINT16           *ret_val_ptr,
  LSA_VOID_PTR_TYPE    mem_ptr,
  LSA_INT              pool_handle,
  LSA_UINT16           comp_id )
{
    eps_cp_mem_nrt_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_FREE_TX_MEM
LSA_VOID PSI_NRT_FREE_TX_MEM(
  LSA_UINT16           *ret_val_ptr,
  LSA_VOID_PTR_TYPE    mem_ptr,
  LSA_INT              pool_handle,
  LSA_UINT16           comp_id )
{
    eps_cp_mem_nrt_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_FREE_RX_MEM
LSA_VOID PSI_NRT_FREE_RX_MEM(
  LSA_UINT16           *ret_val_ptr,
  LSA_VOID_PTR_TYPE    mem_ptr,
  LSA_INT              pool_handle,
  LSA_UINT16           comp_id )
{
    eps_cp_mem_nrt_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                     PI memory pool                                        */
/*===========================================================================*/

#ifndef PSI_CREATE_PI_MEM_POOL

/** Systemadaption for creating mempools
 * 
 * Depending on the hardware device type and the hd_nr a special
 * memset function is set to p_memset_fct pointer. With this information
 * the mempool is created by calling eps_cp_mem_create_pi_pool.
 *  
 * @param basePtr Base address of memory block
 * @param length Size of memory block [bytes]
 * @param sys_handle EPS system information (EPS_SYS_PTR_TYPE)
 * @param pool_handle_ptr handle to adress allocated pool
 */
LSA_VOID PSI_CREATE_PI_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT*            pool_handle_ptr )
{
	LSA_UINT32                 gran_size;
	EPS_BOARD_INFO_PTR_TYPE p_board;
	EPS_MEMSET_FCT          p_memset_fct = LSA_NULL;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );

	if ( p_board->edd_type == LSA_COMP_ID_EDDI )
	{
		gran_size = 0x08; // Default for KRAM

		if ( p_board->eddi.device_type == EDD_HW_TYPE_USED_SOC ) // SOC1 board ?
		{
			switch ( pSys->hd_nr )
			{
				case 1: 
				{
					// set special pool memset function
					p_memset_fct = (EPS_MEMSET_FCT)eps_cp_mem_hd1_paea_memset;
				}
				break;
				case 2: 
				{
					// set special pool memset function
					p_memset_fct = (EPS_MEMSET_FCT)eps_cp_mem_hd2_paea_memset;
				}
				break;
				case 3: 
				{
					// set special pool memset function
					p_memset_fct = (EPS_MEMSET_FCT)eps_cp_mem_hd3_paea_memset;
				}
				break;
				case 4: 
				{
					// set special pool memset function
					p_memset_fct = (EPS_MEMSET_FCT)eps_cp_mem_hd4_paea_memset;
				}
				break;
				default:
				{
					EPS_FATAL("Creating MEM_Pool: No supported HD-Number.");	
				}
				break;
			}
		}
		else
		{
			// use standard memset function
			p_memset_fct = (EPS_MEMSET_FCT)eps_memset;
		}
	}
	else
	{
		// use standard memset function
		p_memset_fct = (EPS_MEMSET_FCT)eps_memset;
		gran_size = 0x20;
	}

	*pool_handle_ptr = eps_cp_mem_create_pi_pool( pSys->hd_nr, (LSA_UINT8*)basePtr, length, gran_size, p_memset_fct );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_PI_MEM_POOL
LSA_VOID PSI_DELETE_PI_MEM_POOL(
	LSA_UINT16         * ret_val_ptr,
	PSI_SYS_HANDLE     sys_handle,
	LSA_INT            pool_handle )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys != 0 );

	eps_cp_mem_delete_pi_pool( pSys->hd_nr, pool_handle );
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_PI_ALLOC_MEM
LSA_VOID PSI_PI_ALLOC_MEM(
	LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_BOOL            is_provider )
{
    #if ( PSI_CFG_USE_IOH == 1 )
	*mem_ptr_ptr = eps_cp_mem_pi_alloc( length, is_provider, pool_handle, LSA_COMP_ID_IOH );
    #else
	*mem_ptr_ptr = eps_cp_mem_pi_alloc( length, is_provider, pool_handle, LSA_COMP_ID_PNBOARDS );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef PSI_PI_FREE_MEM
LSA_VOID PSI_PI_FREE_MEM(
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle )
{
    #if ( PSI_CFG_USE_IOH == 1 )
	eps_cp_mem_pi_free( mem_ptr, pool_handle, LSA_COMP_ID_IOH );
	#else
	eps_cp_mem_pi_free( mem_ptr, pool_handle, LSA_COMP_ID_PNBOARDS );
	#endif
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                    stdlib                                                 */
/*===========================================================================*/

#ifndef PSI_MEMSET
LSA_VOID PSI_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT           val,
	LSA_UINT          len )
{
	eps_memset(ptr, val, len);
}
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_MEMCPY
LSA_VOID PSI_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT          len )
{
	eps_memcpy (dst, src, len);
}
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_MEMCMP
LSA_INT PSI_MEMCMP(
	LSA_VOID_PTR_TYPE pBuf1,
	LSA_VOID_PTR_TYPE pBuf2,
	LSA_UINT          Length )
{
	return( eps_memcmp( pBuf1, pBuf2, (LSA_UINT)Length ));
}
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_MEMMOVE
LSA_VOID PSI_MEMMOVE(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len )
{
	eps_memmove( dst, src, len );
}
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_STRCPY
LSA_VOID PSI_STRCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src )
{
	eps_strcpy( dst, src );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_STRLEN
LSA_UINT PSI_STRLEN(
	LSA_VOID_PTR_TYPE str )
{
	return( eps_strlen( str ));
}
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_SPRINTF
LSA_INT PSI_SPRINTF(
	LSA_UINT8  *str,
	LSA_CHAR   *fmt,
	... )
{
	LSA_INT ret;
	va_list argptr;

    va_start( argptr, fmt );
	ret = eps_vsprintf( str, fmt, argptr);
	va_end (argptr);

	return( ret );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_SSCANF_UUID
LSA_INT PSI_SSCANF_UUID(
	LSA_UINT8  *uuid_string,
	LSA_CHAR   *fmt,
	LSA_UINT32 *time_low,
	LSA_INT    *time_mid,
	LSA_INT    *time_hi_and_version,
	LSA_INT    *clock_seq_hi_and_reserved,
	LSA_INT    *clock_seq_low,
	LSA_INT    *node0,
	LSA_INT    *node1,
	LSA_INT    *node2,
	LSA_INT    *node3,
	LSA_INT    *node4,
	LSA_INT    *node5,
	LSA_INT    *read_count )
{
	LSA_INT ret;

	ret = sscanf((const char*) uuid_string, fmt,
		time_low,
		time_mid,
		time_hi_and_version,
		clock_seq_hi_and_reserved,
		clock_seq_low,
		node0, node1, node2, node3, node4, node5,
		read_count );

	return ( ret );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_LOG_10
LSA_VOID PSI_LOG_10(
	LSA_UINT32  const Arg_in_ppm,
	LSA_INT32         * pResult_in_ppm )
{
	// coding from docu
	double LogOut;

	LogOut = log10 ((double)Arg_in_ppm);

	LogOut *= 1000000; /* ppm, parts per million */

	*pResult_in_ppm= (LSA_UINT32)LogOut;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_POW_10
LSA_VOID PSI_POW_10(
	LSA_INT16       const numerator,
	LSA_UINT16      const denominator,
	LSA_UINT32*     pResult )
{
	// coding from docu
	*pResult = (LSA_UINT32) pow (10, (double)numerator/(double)denominator);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EXCHANGE_LONG
long PSI_EXCHANGE_LONG(
	long volatile *long_ptr,
	long           val )
{
	return EPS_PLF_EXCHANGE_LONG(long_ptr, val);
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   FATAL / ERROR                                                           */
/*===========================================================================*/

#ifndef PSI_FATAL_ERROR
LSA_VOID PSI_FATAL_ERROR(
	LSA_CHAR                  *comp,
	LSA_UINT16                length,
	PSI_FATAL_ERROR_PTR_TYPE  error_ptr )
{
	eps_lsa_fatal (comp, length, error_ptr);
}
#else
#error "by design a function!"
#endif

#ifndef PSI_RQB_ERROR
LSA_VOID  PSI_RQB_ERROR(
	LSA_UINT16         comp_id,
	LSA_UINT16         comp_id_lower,
	LSA_VOID_PTR_TYPE  rqb_ptr )
{
	PSI_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_FATAL,
		"RQB_ERROR occured, compId(%u/%#x) compId-Lower(%u/%#x) ptr(%08x)",
		comp_id, comp_id,
		comp_id_lower, comp_id_lower,
		rqb_ptr );
	
	EPS_FATAL("RQB_ERROR occured");
}
#else
#error "by design a function!"
#endif

#ifndef PSI_FATAL_ERROR_OCCURED
LSA_BOOL PSI_FATAL_ERROR_OCCURED( LSA_VOID )
{
	EPS_ASSERT(g_pEpsData != LSA_NULL);

	return (g_pEpsData->bEpsInFatal);
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   HW param adaption                                                       */
/*===========================================================================*/

#ifndef PSI_GET_HD_PARAM
LSA_VOID  PSI_GET_HD_PARAM(
	LSA_UINT16               *ret_val_ptr,
	PSI_HD_INPUT_PTR_TYPE    hd_ptr,
	PSI_HD_SYS_ID_PTR_TYPE   hd_sys_id_ptr,
	PSI_HD_PARAM_PTR_TYPE    hd_param_ptr)
{
    PSI_ASSERT( hd_sys_id_ptr != LSA_NULL );
	PSI_ASSERT( hd_param_ptr  != LSA_NULL );

    // check and getting information for HD over core system (using driver)
	eps_hw_get_hd_params( hd_ptr->hd_id, hd_ptr->runs_on_level_basic, hd_sys_id_ptr, hd_param_ptr );

	*ret_val_ptr = PSI_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_FREE_HD_PARAM
LSA_VOID  PSI_FREE_HD_PARAM(
	LSA_UINT16               *ret_val_ptr,
	LSA_UINT16               hd_id )
{
	// check and getting information for HD over core system (using driver)
	eps_hw_free_hd_params( hd_id );

	*ret_val_ptr = PSI_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_LD_CLOSED_HD
LSA_VOID PSI_LD_CLOSED_HD(
	LSA_UINT16 hd_id )
{
	eps_hw_close_hd( hd_id );	
}
#endif

/*===========================================================================*/
/*   PNIO IR adaption                                                        */
/*===========================================================================*/

#ifndef PSI_HD_ENABLE_IR
LSA_VOID PSI_HD_ENABLE_IR(
	PSI_SYS_HANDLE sys_handle )
{
    EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sys_handle;
    PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PSI_HD_ENABLE_IR() - enable interrupts for hd_id = %d", pSys->hd_nr);
    eps_enable_pnio_ir( sys_handle );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_HD_DISABLE_IR
LSA_VOID PSI_HD_DISABLE_IR(
	PSI_SYS_HANDLE sys_handle )
{
    EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sys_handle;
    PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PSI_HD_DISABLE_IR() - disable interrupts for hd_id = %d", pSys->hd_nr);
    eps_disable_pnio_ir( sys_handle );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef PSI_THREAD_READY
LSA_VOID PSI_THREAD_READY( LSA_VOID_PTR_TYPE arg )
{
	eps_tasks_signal_psi_thread_ready( arg );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_THREAD_STOPPED
LSA_VOID PSI_THREAD_STOPPED( LSA_VOID_PTR_TYPE arg )
{
	eps_tasks_signal_psi_thread_stopped( arg );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef PSI_GET_IM_SERIAL_NUMBER
LSA_VOID  PSI_GET_IM_SERIAL_NUMBER(
	LSA_UINT16  *ret_val_ptr,
	LSA_UINT8*  im_serial_nr_ptr,
	LSA_UINT16  im_serial_nr_size )
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;

	PSI_ASSERT( ret_val_ptr != LSA_NULL );
	PSI_ASSERT( im_serial_nr_ptr != LSA_NULL );
	
    #if PSI_DEBUG
	PSI_ASSERT( im_serial_nr_size == PSI_MAX_SIZE_IM_SERIAL_NR );
    #else
	LSA_UNUSED_ARG( im_serial_nr_size );
    #endif

	// Note: IF MAC of first HD IF is used as serial number
	pBoard = eps_hw_get_board_store( 1 );
	PSI_ASSERT( pBoard != LSA_NULL );

	eps_sprintf( im_serial_nr_ptr, "%02x%02x%02x%02x%02x%02x", 
		pBoard->if_mac[0].mac[0],
		pBoard->if_mac[0].mac[1],
		pBoard->if_mac[0].mac[2],
		pBoard->if_mac[0].mac[3],
		pBoard->if_mac[0].mac[4],
		pBoard->if_mac[0].mac[5] );

    *ret_val_ptr = PSI_OK;
}
#else
#error "by design a function!"
#endif


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
