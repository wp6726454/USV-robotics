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
/*  F i l e               &F: pnd_tasks.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PnDriver core tasks and MBX extensions                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 1112
#define PND_MODULE_ID 1112

#include "eps_rtos.h"  // PNDRIVER adaption to OS
#include "psi_cfg.h"

#include "pnd_int.h"  // Internal header
#include "pnd_sys.h"

PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID)

/*---------------------------------------------------------------------------*/
/* defines                                                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* types                                                                     */
/*---------------------------------------------------------------------------*/

typedef struct pnd_thread_tag {
  LSA_CHAR* name_ptr;
  LSA_UINT32 pnd_threadId;
  LSA_INT threadId;
  pthread_t threadHandle;
  LSA_BOOL quitting;
  LSA_INT threadPrio;
  LSA_VOID* stackaddr;
} PND_THREAD;

#define PND_MAX_TASKS 2

/*----------------------------------------------------------------------------*/
// Defines the possible PNDRIVER tasks
static PND_THREAD pndriverTasks[PND_MAX_TASKS] = {
    {"IO_BASECORET", PND_MBX_ID_IO_BASE_CORE, 0, 0, LSA_FALSE,
     EPS_POSIX_THREAD_PRIORITY_NORMAL},
    {"PNIO_USRCORET", PND_MBX_ID_PNIO_USER_CORE, 0, 0, LSA_FALSE,
     EPS_POSIX_THREAD_PRIORITY_NORMAL}};

/*----------------------------------------------------------------------------*/
static struct {
  LSA_UINT16 enter_exit;
  LSA_UINT16 event_id;
  PND_THREAD* p_threads;
} pnd_threads;

/*----------------------------------------------------------------------------*/

LSA_VOID pnd_stop_threads(LSA_VOID) {
  LSA_UINT16 idx;
  PND_THREAD* tp;

  /* If still running, wait until thread has finished. */
  for (idx = 0; idx < PND_MAX_TASKS; idx++)  // Quit the configured threads
  {
    tp = &pnd_threads.p_threads[idx];

    // if ( tp->threadId != 0 ) // check for finished thread ?
    { pthread_join(tp->threadHandle, LSA_NULL); }
  }
}

/*----------------------------------------------------------------------------*/

LSA_VOID pnd_threads_init(LSA_VOID) {
  pnd_memset(&pnd_threads, 0, sizeof(pnd_threads));

  pnd_alloc_critical_section(&pnd_threads.enter_exit, LSA_FALSE);

  pnd_threads.p_threads = pndriverTasks;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID pnd_threads_undo_init(LSA_VOID) {
  pnd_stop_threads();

  pnd_free_critical_section(pnd_threads.enter_exit);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static /*unsigned int*/ void* pnd_thread_proc(void* arg) {
  /* start the PND thread (normal a message loop  */
  /* note: if ready PND signals over PND_if finished the pnd_tread_proc returns
   */

  pnd_core_thread_proc(arg);
  // threads ends
  return (0);
}

/*----------------------------------------------------------------------------*/

LSA_VOID pnd_start_thread(PND_THREAD* tp) {
  int ret_val;
  struct sched_param th_param;  // Scheduling parameters
  pthread_attr_t th_attr;       // Attributes

  // Init thread attributes
  ret_val = pthread_attr_init(&th_attr);
  PND_ASSERT(ret_val == 0);

#if (EPS_PLF == EPS_PLF_LINUX_X86)
  // Set scheduling policy to RR
  ret_val = pthread_attr_setschedpolicy(&th_attr,
                                        /*SCHED_OTHER atw12r13*/ SCHED_FIFO);
  EPS_ASSERT(ret_val == 0);
#endif

  // Enable explicitly to set individual scheduling parameters
  pthread_attr_setinheritsched(&th_attr, PTHREAD_EXPLICIT_SCHED);

  ret_val = pthread_attr_setstacksize(&th_attr, 128000); /* 128 KB Stacksize */

  // Set priority
  th_param.sched_priority = tp->threadPrio;

  ret_val = pthread_attr_setschedparam(&th_attr, &th_param);
  PND_ASSERT(ret_val == 0);

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  // Set scheduling policy to RR
  ret_val = pthread_attr_setschedpolicy(&th_attr, SCHED_RR);
  PND_ASSERT(ret_val == 0);

  // Set name
  ret_val = pthread_attr_setname(&th_attr, tp->name_ptr);
  PND_ASSERT(ret_val == 0);
#endif

  // Set argument to the task data structure used for creation
  ret_val = pthread_create(&(tp->threadHandle), &th_attr,
                           /*(void *)*/ pnd_thread_proc, tp);
  PND_ASSERT(ret_val == 0);

#if (EPS_PLF == EPS_PLF_LINUX_X86)
  ret_val = pthread_setname_np(tp->threadHandle, tp->name_ptr);
  PND_ASSERT(ret_val == 0);
#endif

  // PND_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"pnd_start_thread() - started
  // thread %d",tp->threadHandle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID pnd_start_threads(LSA_VOID) {
  PND_THREAD* tp;
  LSA_UINT16 idx;

  /* PND_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
      "pnd_start_threads(): count of threads(%u)", PND_MAX_TASKS );
  */
  pnd_alloc_event(&pnd_threads.event_id);

  for (idx = 0; idx < PND_MAX_TASKS; idx++)  // Start the configured threads
  {
    tp = &pnd_threads.p_threads[idx];

    pnd_enter_critical_section(pnd_threads.enter_exit);

    tp->threadId = idx + 1;
    pnd_start_thread(tp);

    PND_ASSERT(tp->threadHandle != 0);

    pnd_exit_critical_section(pnd_threads.enter_exit);

    // Wait till thread signals ready
    pnd_wait_event(pnd_threads.event_id);
    pnd_reset_event(pnd_threads.event_id);
  }

  pnd_free_event(pnd_threads.event_id);
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 pnd_threads_get_current_id(LSA_VOID) {
  LSA_UINT16 idx;
  LSA_UINT32 pnd_thread_id = PND_MAX_TASK_ID;
  LSA_UINT32 curId = pthread_self();

  for (idx = 0; idx < PND_MAX_TASK_ID; idx++) {
    if (curId == pndriverTasks[idx].threadHandle) {
      pnd_thread_id = pndriverTasks[idx].pnd_threadId;
      break;
    }
  }

  return (pnd_thread_id);
}

/*----------------------------------------------------------------------------*/
LSA_VOID pnd_threads_signal_ready(LSA_VOID_PTR_TYPE arg) {
  LSA_UNUSED_ARG(arg);

  /* Signal "can receive messages" */
  pnd_set_event(pnd_threads.event_id);
}

/*----------------------------------------------------------------------------*/
LSA_VOID pnd_threads_signal_stopped(LSA_VOID_PTR_TYPE arg) {
  PND_THREAD* tp_this = (PND_THREAD*)arg;

  PND_ASSERT(tp_this != LSA_NULL);

  // set the thread to inactive, finished
  // Note: called by PND thread if message loop is breaked
  tp_this->threadId = 0;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
