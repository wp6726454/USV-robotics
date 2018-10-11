#ifndef EPS_TASKS_H             /* ----- reinclude-protection ----- */
#define EPS_TASKS_H

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_tasks.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS internal Task API.                                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_TASKS_USR_POOL                  1
#define EPS_TASKS_PSI_POOL                  2

#define EPS_TASKS_MAX_THREAD_NAME_SIZE      16  // Max thread name length = 16, because Linux does not support more!

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef LSA_VOID (*EPS_TASKS_THREAD_PROC_TYPE)(LSA_UINT32 uParam, LSA_VOID* pArgs);

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

typedef enum 
{
    EPS_TGROUP_NONE    = 0,
    EPS_TGROUP_PSI     = 1,
    EPS_TGROUP_NRT     = 2,
    EPS_TGROUP_USER    = 3,
    EPS_TGROUP_MAX
} EPS_TGROUPS_ENUM_TYPE;

typedef enum 
{
    eSchedInvalid   = 0,
    eSchedFifo      = 1,
    eSchedRR        = 2
} EPS_THREAD_SCHEDULING_TYPE;

LSA_VOID   eps_tasks_init                       ( LSA_VOID );
LSA_VOID   eps_tasks_undo_init                  ( LSA_VOID );
LSA_UINT32 eps_tasks_get_current_psi_thread_id  ( LSA_VOID );
LSA_VOID   eps_tasks_signal_psi_thread_ready    ( LSA_VOID_PTR_TYPE arg );
LSA_VOID   eps_tasks_signal_psi_thread_stopped  ( LSA_VOID_PTR_TYPE arg );
LSA_UINT32 eps_tasks_start_thread               ( LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs );
LSA_UINT32 eps_tasks_start_event_thread         ( LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs );
LSA_UINT32 eps_tasks_start_poll_thread          ( LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, LSA_UINT32 uIntervalMs, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs );
LSA_UINT32 eps_tasks_start_us_poll_thread       ( LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, LSA_UINT32 uIntervalUs, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs );
LSA_VOID   eps_tasks_group_thread_add           ( LSA_UINT32 hThread, EPS_TGROUPS_ENUM_TYPE eTGroup );
LSA_VOID   eps_tasks_group_thread_remove        ( LSA_UINT32 hThread );
LSA_VOID   eps_tasks_trigger_event_thread       ( LSA_UINT32 hThread );
LSA_VOID   eps_tasks_stop_thread                ( LSA_UINT32 hThread );
LSA_VOID   eps_tasks_stop_poll_thread           ( LSA_UINT32 hThread );
LSA_VOID   eps_tasks_stop_event_thread          ( LSA_UINT32 hThread );
LSA_VOID   eps_tasks_sleep                      ( LSA_UINT32 uTimeMs );
LSA_VOID   eps_tasks_usleep                     ( LSA_UINT32 uTimeUs );
LSA_VOID   eps_tasks_nswait                     ( LSA_UINT64 uTimeNs );
LSA_VOID   eps_tasks_fatal                      ( LSA_VOID );
LSA_VOID*  eps_tasks_get_os_handle              ( LSA_UINT32 hThread );
LSA_VOID*  eps_tasks_get_group_os_handle        ( EPS_TGROUPS_ENUM_TYPE eTGroup );
LSA_VOID   eps_tasks_start_psi_threads          ( LSA_VOID );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_TASKS_H */
