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
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_cfg.c.txt                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of LSA-component PNTRC                     */
/*                                                                           */
/*****************************************************************************/
#define PNTRC_MODULE_ID    20101
#define LTRC_ACT_MODUL_ID  20101

#include "psi_inc.h"

#include "eps_rtos.h"  
#include "eps_sys.h"
#include "eps_locks.h"
#include "eps_mem.h"
#include "eps_tasks.h"
#include "eps_shm_if.h"
#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"
#include "eps_trc.h"
#include "pntrc_cfg.h"
#include "pntrc_usr.h"
#include "pntrc_sys.h"
#include "pntrc_int.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9))
#include "trc_infra.h"   //for handling of ERTEC200 and ERTEC400 trace timers
#endif
#if (EPS_PLF == EPS_PLF_ADONIS_X86)
#include "trc_infra.h"
#include "ttu.h"
#endif

/*----------------------------------------------------------------------------*/
static LSA_UINT32 hThread;
static EPS_SHM_HW_PTR_TYPE g_pPntrcShmHwPtr = LSA_NULL;
#if defined(EPS_USE_RTOS_ADONIS)
static pthread_spinlock_t pntrc_trace_spinlock;
#else
static LSA_UINT16 pntrc_trace_enter_exit_id     = EPS_LOCK_ID_INVALID;
#endif

static LSA_UINT16 pntrc_service_enter_exit_id     = EPS_LOCK_ID_INVALID;

static LSA_UINT64 pntrc_timer_start_value = 0;

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_READ_LOWER_CPU
LSA_UINT16 PNTRC_READ_LOWER_CPU(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT32 Offset, LSA_UINT32 size, LSA_UINT8* ptr)
{
	LSA_UINT16 result;
	EPS_PNDEV_HW_PTR_TYPE eps_pndev_hw;
	EPS_ASSERT(hLowerDev != LSA_NULL);

	eps_pndev_hw = (EPS_PNDEV_HW_PTR_TYPE)hLowerDev;
	EPS_ASSERT(eps_pndev_hw->ReadTraceData != LSA_NULL);
	result=(*eps_pndev_hw->ReadTraceData)(eps_pndev_hw,Offset,ptr,size);
	return result;
}
#endif

#ifndef PNTRC_WRITE_LOWER_CPU
LSA_UINT16 PNTRC_WRITE_LOWER_CPU(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT8* ptr, LSA_UINT32 Offset, LSA_UINT32 size)
{
	LSA_UINT16 result;
	EPS_PNDEV_HW_PTR_TYPE eps_pndev_hw;
	EPS_ASSERT(hLowerDev != LSA_NULL);

	eps_pndev_hw = (EPS_PNDEV_HW_PTR_TYPE)hLowerDev;
	EPS_ASSERT(eps_pndev_hw->WriteTraceData != LSA_NULL);
	result=(*eps_pndev_hw->WriteTraceData)(eps_pndev_hw,ptr,Offset,size);
	return result;
}
#endif

#ifndef PNTRC_BUFFER_FULL
LSA_UINT16 PNTRC_BUFFER_FULL(LSA_VOID* SysHandle, PNTRC_BUFFER_ID_TYPE BufferID, LSA_UINT8* pBuffer, LSA_UINT32 Size)
{
	EPS_ASSERT(g_pPntrcShmHwPtr != LSA_NULL);
	EPS_ASSERT(g_pPntrcShmHwPtr->TraceBufferFull != LSA_NULL);

#if 0 //Tracing during Buffer-Switch leads to an Enter/exit lock 
	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT,"PNTRC_BUFFER_FULL, BufferID(%08x)", BufferID); 
#endif

	g_pPntrcShmHwPtr->TraceBufferFull(SysHandle, (LSA_UINT32) BufferID, pBuffer, Size);

	return 0;
}
#endif

LSA_VOID pntrc_service_proc(LSA_UINT32 uParam, LSA_VOID* pArg)
{
	LSA_UNUSED_ARG(uParam);
	LSA_UNUSED_ARG(pArg);

	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT,"pntrc_service_proc");

#if (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) 
	pntrc_force_buffer_switch();
#endif
	pntrc_service();
}



#if (EPS_PLF == EPS_PLF_SOC_MIPS)

	///< map this address to get the counter value in user mode (see trc_infra.c/h)
	#define TTU_DRV_MMAP_ADDRESS								0x16000000

	///< base address of module TTU before configuration -> 0x16000000 + 0xA0000000
	#define TTU_BASE_ADDRESS										0x16000000

	///< start address of registers for endian swapper before configuration -> 0x1FB00330 + 0xA0000000
	#define SRCB_START_ENDIAN_SWAPPER_ADDRESS		0x1FB00330

//-- Registers for endian swapper
typedef struct srcb_endian_swap_s{
	uint32_t ctrl_bits_m;
	uint32_t ctrl_bits_sl;
}srcb_endian_swap_t;

//Set bits in endian swapper register
#define SRCB_ACT_BSWAP_TRACETIMER_SL		(1<<12)		///< activate byte lane swapper at module TTU, if mode is big endian

volatile srcb_endian_swap_t *ttu_drv_endian_swap;

//-- Registers of module TTU (Trace Timer Unit)
typedef struct ttu_timer_s{
	uint64_t load_reg;													///< load register for 64-bit-access
	uint32_t load_low_reg_little_endian;				///< load register for low 32-bit, mode: little endian
	uint32_t load_low_reg_big_endian;						///< load register for low 32-bit, mode: big endian
	uint32_t load_high_reg_little_endian;				///< load register for high 32-bit, mode: little endian
	uint32_t load_high_reg_big_endian;					///< load register for high 32-bit, mode: big endian
	uint64_t cnt_read_reg;											///< count register for 64-bit-access
	uint32_t cnt_read_low_reg_little_endian;		///< count register for read the low 32-bit, mode: little endian
	uint32_t cnt_read_low_reg_big_endian;				///< count register for read the low 32-bit, mode: big endian
	uint32_t cnt_read_high_reg_little_endian;		///< count register for read the high 32-bit, mode: little endian
	uint32_t cnt_read_high_reg_big_endian;			///< count register for read the high 32-bit, mode: big endian
	uint32_t adjust_reg_little_endian;					///< adjust register for read the resolution, mode: little endian
	uint32_t adjust_reg_big_endian;							///< adjust register for read the resolution, mode: big endian
	uint32_t control_reg_little_endian;					///< control register for select clock pulse and en/disable timer, mode: little endian
	uint32_t control_reg_big_endian;						///< control register for select clock pulse and en/disable timer, mode: big endian
}pntrc_ttu_timer_t;

#elif (EPS_PLF == EPS_PLF_PNIP_ARM9)

	///< map this address to get the counter value in user mode (see trc_infra.c/h)
	#define TTU_DRV_MMAP_ADDRESS								0x10B00000

	///< base address of module TTU before configuration -> 0x16000000 + 0xA0000000
	#define TTU_BASE_ADDRESS										0x10B00000


typedef struct ttu_timer_s{
	uint32_t load_low_reg_little_endian;				///< load register for low 32-bit, mode: little endian
	uint32_t load_high_reg_little_endian;				///< load register for high 32-bit, mode: little endian
	uint32_t cnt_read_low_reg_little_endian;		///< count register for read the low 32-bit, mode: little endian
	uint32_t cnt_read_high_reg_little_endian;		///< count register for read the high 32-bit, mode: little endian
	uint32_t adjust_reg_little_endian;					///< adjust register for read the resolution, mode: little endian
	uint32_t control_reg_little_endian;					///< control register for select clock pulse and en/disable timer, mode: little endian
}pntrc_ttu_timer_t;

#elif (EPS_PLF == EPS_PLF_ADONIS_X86)
	extern uint64_t trc_timer_res;
	static uint64_t x86_trc_timer_res;
#endif

#if ((EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_PNIP_ARM9))

//Set bits in control register
#define TTU_CLK_SELECT_INT_CLOCK_PULSE	(0<<0)		///< select internal clock pulse
#define TTU_COUNT_ENABLE								(1<<1)		///< enable counter
#define TTU_SET_TIMER										(TTU_CLK_SELECT_INT_CLOCK_PULSE | TTU_COUNT_ENABLE)

static volatile uint8_t* pntrc_ttu_counter_address=(void*) TTU_DRV_MMAP_ADDRESS;

LSA_VOID pntrc_init_ttu(LSA_VOID)
{
  volatile pntrc_ttu_timer_t *ttu_drv_timer;
	/// Set base/start address for module TTU
	ttu_drv_timer = (pntrc_ttu_timer_t*)TTU_BASE_ADDRESS;
	pntrc_ttu_counter_address= (void*) TTU_DRV_MMAP_ADDRESS;

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
	/// Set start address for endian swapper register
	ttu_drv_endian_swap = (srcb_endian_swap_t*)SRCB_START_ENDIAN_SWAPPER_ADDRESS;

	/// Deactivate endian swapper
	ttu_drv_endian_swap->ctrl_bits_sl &= ~SRCB_ACT_BSWAP_TRACETIMER_SL;
#endif

	/// Initialize control-, load low- and load high register with '0'
	ttu_drv_timer->control_reg_little_endian   	= 0x00000000;
	ttu_drv_timer->load_low_reg_little_endian  	= 0x00000000;
	ttu_drv_timer->load_high_reg_little_endian 	= 0x000000;

	/// Set adjust register with 0x01B4E81B for microsecond resolution
#if (EPS_PLF == EPS_PLF_SOC_MIPS)
	ttu_drv_timer->adjust_reg_little_endian    	= 0x01B4E81B;
#elif (EPS_PLF == EPS_PLF_PNIP_ARM9)
	ttu_drv_timer->adjust_reg_little_endian    	= 0x020C49BA;
#endif
	/// Enable timer and select internal clock pulse as source
	ttu_drv_timer->control_reg_little_endian   |= TTU_SET_TIMER;

}

#endif

LSA_UINT64 pntrc_get_trace_timer()
{
  LSA_UINT64 val=0;
#if (EPS_PLF == EPS_PLF_PNIP_ARM9)
	LSA_UINT32 val_l,val_h;

		// read TTU-timer	-> 56 bit timer
	val_l = *((uint32_t*)(pntrc_ttu_counter_address + 0x8));
	val_h = *((uint32_t*)(pntrc_ttu_counter_address + 0xC));
	val=((LSA_UINT64)val_h << 32) + val_l;
#elif (EPS_PLF == EPS_PLF_SOC_MIPS)
	LSA_UINT32 val_l,val_h;

		// read TTU-timer	-> 56 bit timer
	val_l = *((uint32_t*)(pntrc_ttu_counter_address + 0x20));		///< read low  32 bit (offset 0x20)
	val_h = *((uint32_t*)(pntrc_ttu_counter_address + 0x28));		///< read high 24 bit (offset 0x28)
	val=((LSA_UINT64)val_h << 32) + val_l;
#elif ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9))
	LSA_UINT32 val_l,val_h;

	TRC_GET_COUNTER((uint32_t*)&val_h,(uint32_t*)&val_l);
	val=((LSA_UINT64)val_h << 32) + val_l;
#elif (EPS_PLF == EPS_PLF_ADONIS_X86)
  uint64_t rv;
  __asm__ __volatile__("rdtsc" : "=a" (*((uint32_t*)&rv)), "=d" (*(((uint32_t*)&rv)+1)));
	val = rv / x86_trc_timer_res; // convert to nanoseconds
#elif (EPS_PLF == EPS_PLF_WINDOWS_X86)
	static/*!*/ LARGE_INTEGER freq;
	static/*!*/ LARGE_INTEGER base;
	LARGE_INTEGER now;
	LSA_UINT32 val_l,val_h;

	if( freq.QuadPart == 0 ) 
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&base);
	}
	QueryPerformanceCounter(&now);
	now.QuadPart=(now.QuadPart-base.QuadPart)*1000000/freq.QuadPart;  //return in usec
	 val_l =(LSA_UINT32)now.LowPart;
	 val_h =(LSA_UINT32)now.HighPart;
	val=((LSA_UINT64)val_h << 32) + val_l;
#elif (EPS_PLF == EPS_PLF_LINUX_X86) 
    struct timeval tv;
    EPS_POSIX_GETTIMEOFDAY(&tv,NULL);
    val = tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
#else
	static LSA_UINT64 x = 0;

	val = x++; //(LaM)ToDo - Ändern!
#endif
    if (pntrc_timer_start_value == 0) 
    {
	  pntrc_timer_start_value=val;
	}
	return val;
}

LSA_VOID PNTRC_INIT_TRACE_TIMER(PNTRC_INIT_TRACE_TIMER_PTR_TYPE pInit)
{
	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT,"PNTRC_INIT_TRACE_TIMER");

#if (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) 
	LSA_UNUSED_ARG( pInit );  
#elif ((EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_PNIP_ARM9))
	pntrc_init_ttu();
#elif ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9))
	adn_ertec_prf_timer_init();
#elif (EPS_PLF == EPS_PLF_ADONIS_X86)
	int32_t ttu_fd;
	int ret_val;

	ttu_drv_install();
	ttu_fd = open(TTU_NAME, O_RDWR);
	EPS_ASSERT(ttu_fd != -1);
	x86_trc_timer_res=trc_timer_res*10;  //factor to convert to nanoseconds
	pInit->Resolution=x86_trc_timer_res;
	ret_val = close(ttu_fd);
	EPS_ASSERT(ret_val != -1);
	ret_val = ttu_drv_uninstall();	///< Uninstall driver for Trace Timer Unit
	EPS_ASSERT(ret_val == 0);
#endif
}

/*=============================================================================
 * function name:  LTRC_GET_TRACESTAMP
 *
 * function:       read current tracestamp. function mainly
 * 				   			 returns two parts. First part is the tick count.
 * 				         second part is the current frequency.
 * 				         time is calculated by division.
 *
 * parameters:     LSA_UINT32  LTRC_LOCAL_MEM_ATTR *  ltrc_tracestamp_hi
 *                 LSA_UINT32  LTRC_LOCAL_MEM_ATTR *  ltrc_tracestamp_lo
 *
 * return value:   LSA_VOID
 *===========================================================================*/

 /**
	 * Necessary to get counter value of TTU-timer
	 */
LSA_VOID PNTRC_GET_TRACESTAMP(LSA_UINT32* pntrc_tracestamp_lo, LSA_UINT32* pntrc_tracestamp_hi)
{
	LSA_UINT64 val;

	val=pntrc_get_trace_timer() - pntrc_timer_start_value;
	(*pntrc_tracestamp_hi) = ((val >> 32) & 0xFFFFFFFF); /* main counter high 32 bit */
	(*pntrc_tracestamp_lo) = (val & 0xFFFFFFFF); /* main counter low 32 bit */
}

/*----------------------------------------------------------------------------*/

int pntrc_semicolon_missing	= 0;

/*----------------------------------------------------------------------------*/
static LSA_UINT8 PNTRC_Entered[PNTRC_MAX_CATEGORY_COUNT];

LSA_VOID pntrc_startstop (LSA_INT start, LSA_VOID* pShmHw)
{
	LSA_UINT32 i;
	if (start)
	{
		EPS_ASSERT(pShmHw != LSA_NULL);
		g_pPntrcShmHwPtr = (EPS_SHM_HW_PTR_TYPE)pShmHw;
		for (i=0; i<PNTRC_MAX_CATEGORY_COUNT; i++) {
			PNTRC_Entered[i]=0;
		}

		hThread = eps_tasks_start_poll_thread("EPS_PNTRC_POLL", EPS_POSIX_THREAD_PRIORITY_NORMAL, eSchedFifo, 100, pntrc_service_proc, 0, LSA_NULL);
		EPS_ASSERT(hThread != 0);

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS   
		eps_tasks_group_thread_add ( hThread, EPS_TGROUP_PSI );
#endif

		pntrc_set_ready();
	}
	else
	{
#ifndef EPS_CFG_DO_NOT_USE_TGROUPS 
		eps_tasks_group_thread_remove(hThread);
#endif

		eps_tasks_stop_poll_thread(hThread);
	}
}

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_ALLOC_LOCAL_MEM
LSA_VOID PNTRC_ALLOC_LOCAL_MEM(
    PNTRC_MEM_PTR_TYPE                         *  local_mem_ptr_ptr,
    LSA_UINT32                                    length
) {
	*local_mem_ptr_ptr=eps_mem_alloc(length, LSA_COMP_ID_LTRC, 0);
	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT,"PNTRC_ALLOC_LOCAL_MEM, local_mem_ptr(%08x)", *local_mem_ptr_ptr);
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_FREE_LOCAL_MEM
LSA_VOID  PNTRC_FREE_LOCAL_MEM(
    LSA_UINT16                                      *  ret_val_ptr,
    PNTRC_MEM_PTR_TYPE                              local_mem_ptr
) {
	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "PNTRC_FREE_LOCAL_MEM, local_mem_ptr(%08x)", local_mem_ptr);
	*ret_val_ptr=eps_mem_free(local_mem_ptr, LSA_COMP_ID_LTRC, 0);
}
#else
# error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_MEMCPY_LOCAL_MEM
LSA_VOID
PNTRC_MEMCPY_LOCAL_MEM (
    PNTRC_MEM_PTR_TYPE   dest,
    PNTRC_MEM_PTR_TYPE   src,
	LSA_UINT count
) {
	eps_memcpy( dest, src, count );
}
#else
# error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_MEMSET_LOCAL_MEM
LSA_VOID
PNTRC_MEMSET_LOCAL_MEM (
	PNTRC_MEM_PTR_TYPE   dest,
	LSA_UINT val,
	LSA_UINT count
) {
	eps_memset( dest, val, count );
}
#else
# error "by design a function"
#endif


#ifndef PNTRC_STRLEN
LSA_UINT PNTRC_STRLEN(
	PNTRC_MEM_PTR_TYPE str
) {
	return strlen((const char *)(str));
}
#else
# error "by design a function"
#endif

/*----------------------------------------------------------------------------*/


LSA_VOID PNTRC_WRITE_SYNC_TIME(LSA_UINT32 pntrc_tracestamp_lo, LSA_UINT32 pntrc_tracestamp_hi)
{
	EPS_SYSTEM_TRACE_SYNC_TIME(0, LSA_TRACE_LEVEL_FATAL,"SYNC TIME %ld %ld",pntrc_tracestamp_lo,pntrc_tracestamp_hi);
}

#ifndef PNTRC_ALLOC_CRITICAL_SECTION
LSA_VOID PNTRC_ALLOC_CRITICAL_SECTION(LSA_VOID)
{
	LSA_UINT16 rsp1;
#if defined(EPS_USE_RTOS_ADONIS)
  pthread_spin_init(&pntrc_trace_spinlock, 0);
#else
	LSA_UINT16 rsp;

	rsp = eps_alloc_critical_section( &pntrc_trace_enter_exit_id, LSA_TRUE); // recursive lock, PNTRC service may call ENTER twice
	PNTRC_ASSERT(rsp == LSA_RET_OK);

#endif
	rsp1 = eps_alloc_critical_section( &pntrc_service_enter_exit_id, LSA_TRUE); // recursive lock, PNTRC service may call ENTER twice
	PNTRC_ASSERT(rsp1 == LSA_RET_OK);
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_FREE_CRITICAL_SECTION
LSA_VOID PNTRC_FREE_CRITICAL_SECTION(LSA_VOID)
{
	LSA_UINT16 rsp1;
#if defined(EPS_USE_RTOS_ADONIS)
  pthread_spin_destroy(&pntrc_trace_spinlock);
#else
	LSA_UINT16 rsp;

	rsp=eps_free_critical_section(pntrc_trace_enter_exit_id);
	PNTRC_ASSERT(rsp == LSA_RET_OK);
	pntrc_trace_enter_exit_id = EPS_LOCK_ID_INVALID;

#endif
	rsp1=eps_free_critical_section(pntrc_service_enter_exit_id);
	PNTRC_ASSERT(rsp1 == LSA_RET_OK);
	pntrc_service_enter_exit_id = EPS_LOCK_ID_INVALID;
}
#else
# error "by design a function"
#endif

/*----------------------------------------------------------------------------*/
#define STATE ADN_SYS_STATE_DRIVER

#ifndef GET_CATEGORY
LSA_UINT8 PNTRC_GET_CATEGORY(PNTRC_SYS_HANDLE_TYPE hSysDev)
{
	LSA_UNUSED_ARG(hSysDev);
#if defined(EPS_USE_RTOS_ADONIS)
  if (adn_get_system_state() == ADN_SYS_STATE_IRQ) {
        //TODO SMP: 
        //Each core needs its own category for interrupt traces,
        //otherwise more than own core could trace at the same time in the same buffer!
        //Currently MAX_CATEGORY_COUNT is a fixed value for all targets, 
        //we need dynamic category counts for targets with different core numbers!
        //current workaround: only logical core 0 traces in interrupts, 
        //interrupt traces of other cores are ignored, 
        //since (adn_get_logical_core_id()*2 + 1)>MAX_CATEGORY_COUNT for other cores!
        return adn_get_logical_core_id()*2 + 1;
  } else {
        //TODO SMP:
        //Currently all cores use same buffer when in thread context,
        //buffer is protected with spinlock->if own core traces in thread contex all other cores have to wait!
        //When dynamic category count is implemented use different categories for each core!
        return 0; //adn_get_logical_core_id()*2 + 0;
  }
#else
	return 0;
#endif
/*  if (adn_sys_state[0] == ADN_SYS_STATE_BOOT) {
  	return 0;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_RUN) {
  	return 1;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_INIT) {
  	return 2;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_SYS) {
  	return 3;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_IRQ) {
  	return 4;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_DRIVER) {
  	return 5;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_EXC) {
  	return 6;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_SHUTDOWN) {
  	return 7;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_UNKNOWN) {
  	return 8;
  } else {
  	return 9;
  }*/
}
#endif

#ifndef PNTRC_ENTER_TRACE
LSA_UINT32
PNTRC_ENTER_TRACE(
    LSA_UINT8 Category
	)
{
#if defined(EPS_USE_RTOS_ADONIS)
	if (adn_get_system_state() != ADN_SYS_STATE_IRQ) {
		pthread_spin_lock(&pntrc_trace_spinlock);
	}
#else
	eps_enter_critical_section( pntrc_trace_enter_exit_id );
#endif
	if (PNTRC_Entered[Category] == 1) {
//		return 1;
		EPS_FATAL(7);
	}
	PNTRC_Entered[Category]=1;
	return 0;
}
#else
# error "by design a function"
#endif


#ifndef PNTRC_EXIT_TRACE
LSA_VOID
PNTRC_EXIT_TRACE(
    LSA_UINT8 Category
	)
{
	PNTRC_Entered[Category]=0;
#if defined(EPS_USE_RTOS_ADONIS)
	if (adn_get_system_state() != ADN_SYS_STATE_IRQ) {
		pthread_spin_unlock(&pntrc_trace_spinlock);
	}
#else
		eps_exit_critical_section( pntrc_trace_enter_exit_id );
#endif
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_ENTER_SERVICE
LSA_UINT32
PNTRC_ENTER_SERVICE(
    LSA_VOID
	)
{
	eps_enter_critical_section( pntrc_service_enter_exit_id );
	return 0;
}
#else
# error "by design a function"
#endif


#ifndef PNTRC_EXIT_SERVICE
LSA_VOID
PNTRC_EXIT_SERVICE(
    LSA_VOID
	)
{
	eps_exit_critical_section( pntrc_service_enter_exit_id );
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_FATAL_ERROR
LSA_VOID  PNTRC_FATAL_ERROR (
  LSA_UINT16                   length,
  PNTRC_FATAL_ERROR_PTR_TYPE     error_ptr
  )
{
	PSI_FATAL_ERROR ("pntrace", length, error_ptr);
}
#else
# error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_TRACE_OUT_00
LSA_BOOL PNTRC_TRACE_OUT_00 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_01
LSA_BOOL PNTRC_TRACE_OUT_01 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_02
LSA_BOOL PNTRC_TRACE_OUT_02 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_03
LSA_BOOL PNTRC_TRACE_OUT_03 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_04
LSA_BOOL PNTRC_TRACE_OUT_04 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_05
LSA_BOOL PNTRC_TRACE_OUT_05 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_06
LSA_BOOL PNTRC_TRACE_OUT_06 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_07
LSA_BOOL PNTRC_TRACE_OUT_07 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_08
LSA_BOOL PNTRC_TRACE_OUT_08 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_09
LSA_BOOL PNTRC_TRACE_OUT_09 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_10
LSA_BOOL PNTRC_TRACE_OUT_10 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_11
LSA_BOOL PNTRC_TRACE_OUT_11 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_12
LSA_BOOL PNTRC_TRACE_OUT_12 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_13
LSA_BOOL PNTRC_TRACE_OUT_13 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_14
LSA_BOOL PNTRC_TRACE_OUT_14 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13,
   LSA_UINT32             a14
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	LSA_UNUSED_ARG(a14);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_15
LSA_BOOL PNTRC_TRACE_OUT_15 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13,
   LSA_UINT32             a14,
   LSA_UINT32             a15
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	LSA_UNUSED_ARG(a14);
	LSA_UNUSED_ARG(a15);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_16
LSA_BOOL PNTRC_TRACE_OUT_16 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13,
   LSA_UINT32             a14,
   LSA_UINT32             a15,
   LSA_UINT32             a16
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	LSA_UNUSED_ARG(a14);
	LSA_UNUSED_ARG(a15);
	LSA_UNUSED_ARG(a16);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_BYTE_ARRAY
LSA_BOOL PNTRC_TRACE_OUT_BYTE_ARRAY (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT8 *            DataPtr,
   LSA_UINT16             DataLen
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(DataPtr);
	LSA_UNUSED_ARG(DataLen);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_STRING
LSA_BOOL PNTRC_TRACE_OUT_STRING (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_CHAR *             St
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(St);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_SYNC_TIME
LSA_BOOL PNTRC_TRACE_OUT_SYNC_TIME (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
   LSA_CHAR*              Msg,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2
   )
{
	LSA_UNUSED_ARG(SubSys);
	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModulId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	return LSA_TRUE;
}
#else
# error "by design a function"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
