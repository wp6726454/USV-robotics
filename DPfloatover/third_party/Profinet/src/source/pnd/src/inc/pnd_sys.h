#ifndef PND_SYS_H
#define PND_SYS_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/



#include "eps_rtos.h"   // EPS adaption to OS

#include "eps_sys.h"    // EPS framework for locks, events
#include "eps_timer.h"  // EPS framework for timer
#include "eps_locks.h"  // EPS Locks
#include "eps_events.h" // EPS Events
#include "eps_mem.h"    // Local Memory Handling

/*------------------------------------------------------------------------------
   definition for invalid LOCK- and Event-Handles and NO-Timeout
------------------------------------------------------------------------------*/

#define PND_LOCK_ID_INVALID   0
#define PND_EVENT_ID_INVALID  0
#define PND_EVENT_NO_TIMEOUT  0


typedef void(* pnd_postfunction)(void*);

/*===========================================================================*/
/*  PNDRIVER mbox adaption                                                 */
/*===========================================================================*/

typedef enum {
	PND_MBX_ID_IO_BASE_CORE   = 1,
	PND_MBX_ID_PNIO_USER_CORE = 2,
	/*-------------------------*/
	PND_MAX_MBX_ID
} PND_MBX_ID_TYPE;


// Note: Tasks see description of PNDRIVER tasks
typedef enum {
	PND_IO_BASE_CORE_TASK_ID = 1,
	PND_PNIO_USER_CORE_TASK  = 2,
	/*-------------------------*/
	PND_MAX_TASK_ID        /* Max TASK ID */
} PND_TASK_ID_TYPE;

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* stdio       ----------------------------------------------------------*/
#define pnd_memset  eps_memset
#define pnd_memcpy  eps_memcpy
#define pnd_memmove eps_memmove
#define pnd_strcpy  eps_strcpy
#define pnd_sprintf eps_sprintf

LSA_UINT32 pnd_hton32(LSA_UINT32 host_long);
LSA_UINT16 pnd_hton16(LSA_UINT16 host_short);

/* byte swap functions,---------------------------------------------------*/
LSA_VOID   PND_SWAP_U16( LSA_UINT16  * short_ptr ); /* correct alignment! */
LSA_VOID   PND_SWAP_U32( LSA_UINT32  * long_ptr );  /* correct alignment! */


/* stdio -----------------------------------------------------------------*/
LSA_INT    pnd_strcmp ( const LSA_UINT8 *str1, const LSA_UINT8 *str2 );

/* stdio, not mapped to eps_sys functions,--------------------------------*/
LSA_INT    pnd_strlen(const LSA_CHAR *str);
LSA_INT    pnd_strncmp(const LSA_CHAR *s1, const LSA_CHAR *s2, LSA_INT n);

/*  tasks      ---------------------------------------------------------- */

LSA_VOID   pnd_threads_init(LSA_VOID);
LSA_VOID   pnd_threads_undo_init (LSA_VOID);
LSA_VOID   pnd_start_threads(LSA_VOID);
LSA_VOID   pnd_stop_threads( LSA_VOID );
LSA_UINT32 pnd_threads_get_current_id(LSA_VOID);
LSA_VOID   pnd_threads_signal_ready(LSA_VOID_PTR_TYPE arg);
LSA_VOID   pnd_threads_signal_stopped(LSA_VOID_PTR_TYPE arg);

/*  mailbox     ----------------------------------------------------------*/

typedef LSA_VOID (*PND_REQUEST_FCT)( LSA_VOID_PTR_TYPE );

LSA_VOID pnd_mbx_sys_init (LSA_VOID);
LSA_VOID pnd_mbx_sys_undo_init(LSA_VOID);
LSA_VOID pnd_request_start(LSA_UINT16 mbx_id, PND_REQUEST_FCT req_func, LSA_VOID_PTR_TYPE pRQB ); /* start a PNIO request */ 
LSA_VOID pnd_request_local(LSA_UINT16 mbx_id, PND_REQUEST_FCT req_func, LSA_VOID_PTR_TYPE pRQB ); /* start a local PNIO request */
LSA_UINT32 pnd_core_thread_proc(LSA_VOID_PTR_TYPE arg);

/* locks       ----------------------------------------------------------*/

#define pnd_alloc_critical_section eps_alloc_critical_section
#define pnd_free_critical_section  eps_free_critical_section

#define pnd_enter_critical_section  eps_enter_critical_section
#define pnd_exit_critical_section   eps_exit_critical_section

/* events     ----------------------------------------------------------*/

#define PND_EVENT_ID_INVALID   0
#define PND_SYS_HANDLE_EVENT   LSA_VOID_PTR_TYPE

#define pnd_alloc_event  eps_alloc_event
#define pnd_free_event   eps_free_event
#define pnd_set_event    eps_set_event
#define pnd_reset_event  eps_reset_event
LSA_VOID pnd_wait_event( LSA_UINT16 event_id  );
LSA_VOID pnd_wait_event_t(LSA_UINT16 event_id, LSA_UINT16 wait_sec );


/*  ----local mem -----------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE pnd_mem_alloc( LSA_UINT32 nSize );
LSA_VOID pnd_mem_free( LSA_VOID_PTR_TYPE pBuffer );


/*  ---- time -----------------------------------------------------------------------*/

typedef LSA_VOID (*PND_TIMER_CBF) (LSA_TIMER_ID_TYPE, LSA_USER_ID_TYPE);

#define pnd_timer_allocate eps_timer_allocate_tgroup0
#define pnd_timer_free     eps_timer_free_tgroup0
#define pnd_timer_start    eps_timer_start_tgroup0
#define pnd_timer_stop     eps_timer_stop_tgroup0

/*---------- fatal --------------------*/

LSA_VOID pnd_fatal (const LSA_CHAR *file, LSA_INT line, const LSA_CHAR *func, const LSA_CHAR *str);
LSA_BOOL PND_FATAL_ERROR_OCCURED( LSA_VOID );

#define PND_FATAL( txt_ )  pnd_fatal( __FILE__, __LINE__, __FUNCTION__, txt_ )

#ifdef PND_DEBUG
#define PND_ASSERT(cond_) { \
	if (!(cond_)) { \
		pnd_fatal (__FILE__, __LINE__, __FUNCTION__, "assert: " #cond_); \
	} \
}
#else
#define PND_ASSERT(cond_) { \
    if (cond_){} \
}
#endif

#ifndef PND_STATIC_ASSERT
#  define PND_STATIC_ASSERT(expr_, msg_)    PND_ASSERT(expr_) /* use runtime assert */
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif


#endif  /* of PND_SYS_H */
