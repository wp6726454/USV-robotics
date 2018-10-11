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
/*  F i l e               &F: eps_tasks.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Task/Thread abstraction                                              */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20006
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include <psi_inc.h>   
#include <psi_cfg.h>      
#include <eps_rtos.h>      /* RTOS Interface          */
#include <eps_sys.h>       /* Types / Prototypes      */
#include <eps_locks.h>     /* EPS Locks               */
#include <eps_events.h>    /* EPS Events              */
#include <eps_mem.h>       /* EPS Local Mem           */
#include <eps_trc.h>       /* Tracing                 */
#include <eps_tasks.h>     /* Task If                 */
#include <eps_plf.h>       /* Platform switches       */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

EPS_FILE_SYSTEM_EXTENSION(EPS_MODULE_ID)

#define EPS_TASKS_THREAD_TYPE_PSI             1
#define EPS_TASKS_THREAD_TYPE_USR             2
#define EPS_TASKS_THREAD_TYPE_POLL_USR        3
#ifdef EPS_TASKS_USE_EVENT_TASKS
#define EPS_TASKS_THREAD_TYPE_EVENT           4
#endif

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS

    #if (EPS_PLF == EPS_PLF_SOC_MIPS)
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT_RUN             20   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT_RUN       100   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT                 50   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT            55   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_RT_ONLY                 5    // percent % (tgroup quota)
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_RT_DEFAULT         EPS_TASK_COMMUNICATIONLOAD_RT_ONLY
    #elif (EPS_PLF == EPS_PLF_PNIP_ARM9)
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT_RUN             20   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT_RUN       100   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT                 50   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT            55   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_RT_ONLY                 5    // percent % (tgroup quota)
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_RT_DEFAULT         EPS_TASK_COMMUNICATIONLOAD_RT_ONLY
    #elif (EPS_PLF == EPS_PLF_ERTEC200_ARM9)
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT_RUN             20   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT_RUN       100   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT                 50   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT            30   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_RT_ONLY                 50   // percent % (tgroup quota)
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_RT_DEFAULT         EPS_TASK_COMMUNICATIONLOAD_RT_ONLY
    #elif (EPS_PLF == EPS_PLF_ERTEC400_ARM9)
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT_RUN             20   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT_RUN       100   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT                 50   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT            30   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_RT_ONLY                 50   // percent % (tgroup quota)
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_RT_DEFAULT         EPS_TASK_COMMUNICATIONLOAD_RT_ONLY
    #elif (EPS_PLF == EPS_PLF_ADONIS_X86)
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT_RUN             20   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT_RUN       100   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT                 50   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT            55   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_RT_ONLY                 5    // percent % (tgroup quota)
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_RT_DEFAULT         EPS_TASK_COMMUNICATIONLOAD_RT_ONLY
    #else
        // CPU1518 Communication Thread Group (Asynchronous Threads) Parameters	
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT_RUN             20   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT_RUN       100   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_DEFAULT                 50   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT            55   // percent % (tgroup quota), DEFAULT=ASYNCH_RT+ASYNCH
        #define EPS_TASK_COMMUNICATIONLOAD_RT_ONLY                 5    // percent % (tgroup quota)
        #define EPS_TASK_COMMUNICATIONLOAD_PEAK_RT_DEFAULT         EPS_TASK_COMMUNICATIONLOAD_RT_ONLY
    #endif

#endif

//------------------------------------------------------------------------------
// PSI Threads
//------------------------------------------------------------------------------

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
typedef struct {
	tgroup_t		 OS_tgroup_id;
	LSA_UINT16       nr_threads_in_group;
} EPS_TGROUP_STRUCT_TYPE, *EPS_TGROUP_STRUCT_PTR_TYPE;
#endif

typedef struct eps_psi_thread_tag {
	LSA_CHAR*   pName;
	LSA_UINT32  uPsiThreadId;
	LSA_INT     Prio;
	EPS_TGROUPS_ENUM_TYPE eTGroup;
	EPS_THREAD_SCHEDULING_TYPE eSchedType;
} EPS_PSI_THREAD_INIT_DATA_TYPE, *EPS_PSI_THREAD_INIT_DATA_PTR_TYPE;

/*----------------------------------------------------------------------------*/
static EPS_PSI_THREAD_INIT_DATA_TYPE pnioThreads[] = { // Defines the possible PNIO threads and group-assignment
    { "NRT_ORG_TASK",       PSI_NRT_ORG_TASK_ID,       EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL,	EPS_TGROUP_NRT,	 eSchedRR },
	{ "NRT_LOW_TASK",       PSI_NRT_LOW_TASK_ID,       EPS_POSIX_THREAD_PRIORITY_NORMAL,        EPS_TGROUP_PSI,	 eSchedRR },
	{ "EDDI_REST_TASK",     PSI_EDDI_SYS_REST_TASK_ID, EPS_POSIX_THREAD_PRIORITY_NORMAL,        EPS_TGROUP_PSI,	 eSchedRR },
	{ "EDDI_AUX_TASK",      PSI_EDDI_AUX_TASK_ID,      EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL,  EPS_TGROUP_PSI,	 eSchedRR },
	{ "EDDP_TASK",          PSI_EDDP_TASK_ID,          EPS_POSIX_THREAD_PRIORITY_NORMAL,        EPS_TGROUP_PSI,	 eSchedRR },

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	{ "EDDS_RQB_TASK",      PSI_EDDS_RQB_TASK_ID,      EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	{ "EDDS_H_TASK",        PSI_EDDS_HIGH_TASK_ID,     EPS_POSIX_THREAD_PRIORITY_HIGH,		    EPS_TGROUP_PSI,	 eSchedFifo },
	{ "EDDS_M_TASK",        PSI_EDDS_MIDDLE_TASK_ID,   EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL,	EPS_TGROUP_PSI,	 eSchedRR },
	{ "EDDS_L_TASK",        PSI_EDDS_LOW_TASK_ID,      EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
	{ "IP_TASK",            PSI_IP_TASK_ID,            EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
	{ "PN_LSA_LD_T",        PSI_PN_LSA_LD_TASK_ID,     EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
	{ "PN_LSA_HD_T",        PSI_PN_LSA_HD_TASK_ID,     EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
	{ "PN_LSA_LOW_T",       PSI_PN_LSA_LOW_TASK_ID,    EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
	{ "HIF_HD_TASK",        PSI_HIF_HD_TASK_ID,        EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
	{ "HIF_LD_TASK",        PSI_HIF_LD_TASK_ID,        EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
	{ "PSI_PN_TASK",        PSI_PN_TASK_ID,            EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_NONE, eSchedRR },
	{ "POF_TASK",           PSI_POF_TASK_ID,           EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_PSI,	 eSchedRR },
	{ "IOH_TASK",           PSI_IOH_TASK_ID,           EPS_POSIX_THREAD_PRIORITY_NORMAL,		EPS_TGROUP_USER, eSchedFifo }
};

#define EPS_MAX_PSI_THREADS  (sizeof(pnioThreads) / sizeof(EPS_PSI_THREAD_INIT_DATA_TYPE))

//------------------------------------------------------------------------------
// Usr Threads
//------------------------------------------------------------------------------

typedef enum {
	eThInit        = 0,
	eThStarting    = 1,
	eThRun         = 2,
	eThStopping    = 3,
	eThCleanupDone = 4,
	eThStopped     = 5
} EPS_THREAD_STATE_TYPE;

typedef struct {
	volatile EPS_THREAD_STATE_TYPE eState;
	LSA_UINT32  hThread;
	LSA_CHAR*   pName;
	pthread_t   hOsHandle;
	EPS_TGROUPS_ENUM_TYPE eTGroup;
    EPS_THREAD_SCHEDULING_TYPE eSchedType;
	LSA_INT     Prio;
	LSA_UINT32  uStackSize;
	LSA_UINT8*  pStack;
	struct
	{
		LSA_UINT32 uThreadType;
		LSA_UINT32 uParam;
		LSA_VOID*  pArgs;
		LSA_UINT32 uIntervalMs;
		LSA_UINT32 uIntervalUs;
		LSA_UINT16 uSignal;
		EPS_TASKS_THREAD_PROC_TYPE pCbf;
		LSA_BOOL   bPollThreadAlive;
	} ThreadArgs;
	struct
	{
		LSA_BOOL   bWaitTillStarted;
		LSA_UINT16 hStartedEvent;
	} Startup;
} EPS_THREAD_TYPE, *EPS_THREAD_PTR_TYPE;

#define EPS_MAX_USR_THREADS 50

//------------------------------------------------------------------------------
// Global Thread Store
//------------------------------------------------------------------------------

typedef struct {
	LSA_UINT16		                   hEnterExit;
	LSA_BOOL                           bFatalError;
	EPS_PSI_THREAD_INIT_DATA_PTR_TYPE  pPsiThreadInitData;
	EPS_THREAD_TYPE                    sPsiThread[EPS_MAX_PSI_THREADS];
	EPS_THREAD_TYPE                    sUsrThread[EPS_MAX_USR_THREADS];
#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	EPS_TGROUP_STRUCT_TYPE             sTGroup[EPS_TGROUP_MAX];
#endif
} EPS_TASK_STORE_TYPE, *EPS_TASK_STORE_PTR_TYPE;

static EPS_TASK_STORE_TYPE g_EpsThreads;
EPS_TASK_STORE_PTR_TYPE g_pEpsThreads = LSA_NULL;

//------------------------------------------------------------------------------
// Static prototypes
//------------------------------------------------------------------------------

static LSA_VOID * eps_tasks_thread_proc_internal( void * arg );

#if defined (EDDS_CFG_HW_INTEL) || defined (EDDS_CFG_HW_PACKET32) || defined (EDDS_CFG_HW_KSZ88XX)
static LSA_VOID eps_tasks_hp_poll_thread_proc_internal( EPS_THREAD_PTR_TYPE const pThread );
#endif

/*----------------------------------------------------------------------------*/

static LSA_VOID eps_tasks_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsThreads->hEnterExit);
}

static LSA_VOID eps_tasks_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsThreads->hEnterExit, LSA_FALSE);
}

static LSA_VOID eps_tasks_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsThreads->hEnterExit);
}

static LSA_VOID eps_tasks_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsThreads->hEnterExit);
}

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS  
/*----------------------------------------------------------------------------*/
// CPU1518 THREAD QUOTA ASSIGNMENT
static LSA_INT16 eps_tasks_thrd_asynch_grp_setquota(int quota, int quota_peak)
{
	LSA_INT16 result = 0;
	LSA_INT16 not_rt_quota = 0;

	EPS_ASSERT(g_pEpsThreads->sTGroup[EPS_TGROUP_PSI].OS_tgroup_id >= 0);
	EPS_ASSERT(g_pEpsThreads->sTGroup[EPS_TGROUP_NRT].OS_tgroup_id >= 0);
  
	/* wrong negative quota!!! */
	EPS_ASSERT(quota >= 0);	
	
	if (quota < EPS_TASK_COMMUNICATIONLOAD_RT_ONLY)
	{
		not_rt_quota = 0;
	}
	else
	{
	    not_rt_quota = quota - EPS_TASK_COMMUNICATIONLOAD_RT_ONLY;
	}
  
	/* ignore quota_peak */
	result = tgroup_setquota(g_pEpsThreads->sTGroup[EPS_TGROUP_PSI].OS_tgroup_id, not_rt_quota, quota_peak);
	if(result >= 0)
	{
		result = tgroup_setquota(g_pEpsThreads->sTGroup[EPS_TGROUP_NRT].OS_tgroup_id, EPS_TASK_COMMUNICATIONLOAD_RT_ONLY, EPS_TASK_COMMUNICATIONLOAD_PEAK_RT_DEFAULT);
	}
  
	if(result >= 0)
	{
		result = 0;
	}
	else
	{
		result = -1;
	}
  
	return result;
}

static LSA_VOID eps_tasks_group_thread_add_internal(EPS_THREAD_PTR_TYPE pThread, EPS_TGROUPS_ENUM_TYPE eTGroup)
{
	LSA_UINT32 result;
	
	if(eTGroup != EPS_TGROUP_NONE)
	{
		result = tgroup_thread_add(g_pEpsThreads->sTGroup[eTGroup].OS_tgroup_id, pThread->hOsHandle);
		EPS_ASSERT(result == 0);

		pThread->eTGroup = eTGroup;

		g_pEpsThreads->sTGroup[eTGroup].nr_threads_in_group++;
	}
}

static LSA_VOID eps_tasks_group_thread_remove_internal(EPS_THREAD_PTR_TYPE pThread)
{
	LSA_UINT32 result;

	if(pThread->eTGroup != EPS_TGROUP_NONE)
	{
		EPS_ASSERT(g_pEpsThreads->sTGroup[pThread->eTGroup].nr_threads_in_group > 0);

		//remove thread from thread group
		#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
		result = tgroup_thread_remove(g_pEpsThreads->sTGroup[pThread->eTGroup].OS_tgroup_id, pThread->hOsHandle);
		EPS_ASSERT(result == 0);
		#endif
				
		g_pEpsThreads->sTGroup[pThread->eTGroup].nr_threads_in_group--;

		pThread->eTGroup = EPS_TGROUP_NONE;
	}
}
#endif

/**
 * initialization of created threads
 * 
 * @param pName name of thread
 * @param Prio  priority of thread
 * @param eSchedType SchedulingPolicy (RoundRobin or FiFo)
 * @param StackSize Stacksize of thread in bytes
 * @param pThread #EPS_THREAD_PTR_TYPE to the thread
 */
static LSA_VOID eps_tasks_init_thread_internal(LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, LSA_UINT32 StackSize, EPS_THREAD_PTR_TYPE pThread)
{
	LSA_UINT32 uStackSize = 128000;
	LSA_UINT32 i;

	pThread->eState  	= eThInit;
	pThread->Prio    	= Prio;
	pThread->eSchedType = eSchedType;
	pThread->pStack   	= LSA_NULL;
	pThread->eTGroup    = EPS_TGROUP_NONE;
	
	pThread->ThreadArgs.uThreadType   = 0;
	pThread->Startup.bWaitTillStarted = 0;

	if(StackSize != 0)
	{
		uStackSize = StackSize;
	}

	pThread->uStackSize = uStackSize;

	for(i=0; (i<21) && (pName[i] != 0); i++);

	pThread->pName = (LSA_CHAR*) eps_mem_alloc( i+2, LSA_COMP_ID_PNBOARDS,  PSI_MTYPE_IGNORE );
	EPS_ASSERT(pThread->pName != LSA_NULL);

	eps_memset(pThread->pName, 0, i+2);
	eps_memcpy(pThread->pName, pName, i+1);
}

/**
 * Creates a new thread, either in PSI_POOL oder USR_POOL
 * 
 * Searches for an unused thread in the threadpool.
 * if there is a free one, it gets initialized by eps_tasks_init_thread_internal.
 * 
 * @param pName c-string to threadname
 * @param Prio  priority of the thread to be created
 * @param eSchedType set SchedulingPolicy to RoundRobin or FiFo
 * @param StackSize size of thread stack in bytes
 * @param uPoolId
 * - #EPS_TASKS_PSI_POOL
 * - #EPS_TASKS_USR_POOL
 * @return LSA_NULL if unknown poolid or no unused threads in pool
 * @return #EPS_THREAD_PTR_TYPE pointer to the initialized thread
 * 
 */
static EPS_THREAD_PTR_TYPE eps_tasks_create_thread_internal(LSA_CHAR* pName, LSA_INT Prio,EPS_THREAD_SCHEDULING_TYPE eSchedType, LSA_UINT32 StackSize, LSA_UINT32 uPoolId)
{
	EPS_THREAD_PTR_TYPE pThread = LSA_NULL;
	LSA_UINT32 uCnt;
	LSA_UINT32 i;

	switch(uPoolId)
	{
		case EPS_TASKS_PSI_POOL:
			pThread = g_pEpsThreads->sPsiThread;
			uCnt = EPS_MAX_PSI_THREADS;
			break;
		case EPS_TASKS_USR_POOL:
			pThread = g_pEpsThreads->sUsrThread;
			uCnt = EPS_MAX_USR_THREADS;
			break;
		default:
			return LSA_NULL;
			break;
	}

	for( i = 0; i < uCnt; i++ )
	{
		if(pThread[i].hThread == 0) /* Unused? */
		{
			pThread[i].hThread = i + 1;
			eps_tasks_init_thread_internal(pName, Prio, eSchedType, StackSize, &pThread[i]);
			return &pThread[i];
		}
	}

	return LSA_NULL;
}

static LSA_VOID eps_tasks_destroy_thread_internal(EPS_THREAD_PTR_TYPE pThread)
{
	LSA_UINT16 retVal;

	EPS_ASSERT(pThread->pName != LSA_NULL);

	retVal = eps_mem_free((LSA_VOID_PTR_TYPE)pThread->pName, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	EPS_ASSERT(retVal == LSA_RET_OK);

	if(pThread->pStack != LSA_NULL)
	{
		retVal = eps_mem_free((LSA_VOID_PTR_TYPE)pThread->pStack, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
		EPS_ASSERT(retVal == LSA_RET_OK);
	}

	pThread->eState = eThStopped;

	pThread->hThread = 0;
}

static LSA_VOID eps_tasks_stop_thread_internal(EPS_THREAD_PTR_TYPE pThread)
{
    LSA_VOID * th_ret;

	EPS_ASSERT(pThread->eState != eThStopped);
	
	EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_tasks_stop_thread_internal() => Thread: (%s)", pThread->pName);

	// Synchronize with other thread wait until it is stopping
	// to prevent from race condition on setting pThread->eState.
	// Afterwards clean up and set pThread->eState=eThCleanupDone.
	while(pThread->eState != eThStopping)
	{
		eps_tasks_sleep(1);
	}
		
	#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
		eps_tasks_group_thread_remove_internal(pThread);
	#endif

	// Some os like adonis do some cleanup in pthread_exit, other os do not.
	// Therefore the thread waits for until cleaup is done.
	// Setting pThread->eState=eThCleanupDone will allow the thread to finish.
	pThread->eState = eThCleanupDone;
	
	// Wait until the thread finishes.
	EPS_POSIX_PTHREAD_JOIN(pThread->hOsHandle, &th_ret);
	
	// Destroy the thread, set pThread->eState=eThStopped.
	eps_tasks_destroy_thread_internal(pThread);	
}

/**
 * 
 * 
 * @param arg #EPS_THREAD_PTR_TYPE Thread parameters
 * @return
 */
static LSA_VOID * eps_tasks_thread_proc_internal( void * arg )
{
    #ifdef EPS_TASKS_USE_EVENT_TASKS
	sigset_t isr_event;
	int      info;
    #endif

	EPS_THREAD_PTR_TYPE pThread = (EPS_THREAD_PTR_TYPE)arg;

	EPS_ASSERT(pThread != LSA_NULL);

	pThread->eState = eThRun;

	if (pThread->Startup.bWaitTillStarted)
	{
		eps_set_event(pThread->Startup.hStartedEvent);
	}

	switch (pThread->ThreadArgs.uThreadType)
	{
		case EPS_TASKS_THREAD_TYPE_PSI:
			psi_thread_proc( arg, pThread->ThreadArgs.uParam );
			break;
		case EPS_TASKS_THREAD_TYPE_USR:
			pThread->ThreadArgs.pCbf(pThread->ThreadArgs.uParam, pThread->ThreadArgs.pArgs);
			break;
		case EPS_TASKS_THREAD_TYPE_POLL_USR:
			pThread->ThreadArgs.bPollThreadAlive = LSA_TRUE;

			if // Standard Poll Thread?
			   (pThread->Prio != EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE)
			{
                while (pThread->ThreadArgs.bPollThreadAlive)
			    {
                    // CBF
                    pThread->ThreadArgs.pCbf(pThread->ThreadArgs.uParam, pThread->ThreadArgs.pArgs);

					if (pThread->ThreadArgs.uIntervalMs != 0)
					{
						eps_tasks_sleep(pThread->ThreadArgs.uIntervalMs);
					}
					else if (pThread->ThreadArgs.uIntervalUs != 0)
					{
						eps_tasks_usleep(pThread->ThreadArgs.uIntervalUs);
					}

      				if (g_pEpsThreads->bFatalError) //On fatal poll thread has to be stopped!
					{
						pThread->ThreadArgs.bPollThreadAlive = LSA_FALSE;
                        //leave loop
						break;
					}
				}
			}
			else // High Performance Poll Thread with equidistant poll cycle (with adaptive sleep)
			{
                #if defined (EDDS_CFG_HW_INTEL) || defined (EDDS_CFG_HW_PACKET32) || defined (EDDS_CFG_HW_KSZ88XX)
                eps_tasks_hp_poll_thread_proc_internal(pThread);
                #else
                EPS_FATAL("HP Poll Thread only allowed with EDDS_CFG_HW_INTEL or EDDS_CFG_HW_PACKET32 or EDDS_CFG_HW_KSZ88XX");
                #endif
            }
			break;
		#ifdef EPS_TASKS_USE_EVENT_TASKS
		case EPS_TASKS_THREAD_TYPE_EVENT:
			sigemptyset(&isr_event);
			sigaddset(&isr_event, SIGRT0);                      /* Kill Signal   */
			sigaddset(&isr_event, pThread->ThreadArgs.uSignal); /* Wakeup Signal */

			EPS_ASSERT(SIGRT0 != pThread->ThreadArgs.uSignal);  /* Wakeup signal must be unequal to kill signal */

			do
			{
				sigwait(&isr_event, &info); //Wait on signal

				if (info & pThread->ThreadArgs.uSignal)
				{
					pThread->ThreadArgs.pCbf(pThread->ThreadArgs.uParam, pThread->ThreadArgs.pArgs);
				}
			} while (!(info & SIGRT0));
			
			break;
		#endif
		default:
			EPS_FATAL("unknown thread type");
	}

	// Other thread is cleaning up this thread
	// the cleanup thread waits until this thread is stopping
	// and cleans this thread afterwards.
	pThread->eState = eThStopping;

	// wait until cleanup is finished.
	while (pThread->eState != eThCleanupDone)
	{
		// sleep to give clean up thread time to clean up 
		// in case it's priority is below this thread.
		eps_tasks_sleep(1);
	}

	// thread end
	return 0;
}

#if defined (EDDS_CFG_HW_INTEL) || defined (EDDS_CFG_HW_PACKET32) || defined (EDDS_CFG_HW_KSZ88XX)

#if (defined (EDDS_CFG_HW_INTEL) || defined (EDDS_CFG_HW_KSZ88XX)) && (EPS_PLF == EPS_PLF_ADONIS_X86)
    // ADONIS selection
    #define EPS_CFG_HP_POLL_CYCLE_TIME_NS             ((LSA_UINT64)1000000)   // 1ms    // hp poll thread cycle time: range: 100us...1s
    #define EPS_CFG_HP_POLL_RUNTIME_COMPENSATION_NS   ((LSA_UINT64)10000)     // 10us   // compensate OS runtime/granularity
    //#define EPS_CFG_HP_POLL_DEBUG_MEASURE_ON                                          // activates measure and checks
    #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
    #define EPS_CFG_HP_POLL_MAX_START_DELAY_NS        ((LSA_UINT64)200000)    // 200us  // if defined: checks start trigger delay against this maximum delay (pre-delay and post-delay)
    //#define EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL                                 // all negative checks are finished with FATAL
    //#define EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON                                   // executes one trace entry per poll cycle
    #endif
#elif (defined (EDDS_CFG_HW_INTEL) || defined (EDDS_CFG_HW_KSZ88XX)) && (EPS_PLF == EPS_PLF_LINUX_X86)
    // LINUX selection
    #define EPS_CFG_HP_POLL_CYCLE_TIME_NS             ((LSA_UINT64)1000000)   // 1ms    // hp poll thread cycle time: range: 100us...1s
    #define EPS_CFG_HP_POLL_RUNTIME_COMPENSATION_NS   ((LSA_UINT64)10000)     // 10us   // compensate OS runtime/granularity
    //#define EPS_CFG_HP_POLL_DEBUG_MEASURE_ON                                          // activates measure and checks
    #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
    #define EPS_CFG_HP_POLL_MAX_START_DELAY_NS        ((LSA_UINT64)200000)    // 200us  // if defined: checks start trigger delay against this maximum delay (pre-delay and post-delay)
    //#define EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL                                 // all negative checks are finished with FATAL
    //#define EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON                                   // executes one trace entry per poll cycle
    #endif
#elif defined (EDDS_CFG_HW_PACKET32)
    // WINDOWS selection
    #define EPS_CFG_HP_POLL_CYCLE_TIME_NS             ((LSA_UINT64)32000000)  // 32ms   // hp poll thread cycle time: range: 100us...1s
    #define EPS_CFG_HP_POLL_RUNTIME_COMPENSATION_NS   ((LSA_UINT64)0)         // 0ns    // compensate OS runtime/granularity
    //#define EPS_CFG_HP_POLL_DEBUG_MEASURE_ON                                          // activates measure and checks
    #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
    #define EPS_CFG_HP_POLL_MAX_START_DELAY_NS        ((LSA_UINT64)6400000)   // 6400us // if defined: checks start trigger delay against this maximum delay (pre-delay and post-delay)
    //#define EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL                                 // all negative checks are finished with FATAL
    //#define EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON                                   // executes one trace entry per poll cycle
    #endif
#endif

static LSA_BOOL bHpPollThreadAlreadyStarted = LSA_FALSE;

/**
 * 
 * @param pThread
 * @return
 */
// High Performance Poll Thread with equidistant poll cycle (with adaptive sleep)
static LSA_VOID eps_tasks_hp_poll_thread_proc_internal( EPS_THREAD_PTR_TYPE const pThread )
{
    LSA_UINT64  StartTimeNs;
    LSA_UINT64  StopTimeNs;
    LSA_UINT64  SleepTimeNs                 = 0;
    LSA_UINT64  TmpTimeNs;
    LSA_UINT64  LastReferenceTimeNs;
    LSA_UINT64  DiffTimeNs;
    LSA_UINT64  StartDelayTimeNs;
    LSA_UINT64  StartPreDelayTimeNs         = 0;    // PreDelayTime  = delay time, the start trigger is too early
    LSA_UINT64  StartPostDelayTimeNs        = 0;    // PostDelayTime = delay time, the start trigger is too late
    LSA_UINT64  CbfRunTimeNs                = 0;    // runtime of CBF
    LSA_UINT32  CycleOverrunCtr             = 0;
    #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
    LSA_UINT64  MaxStartPreDelayTimeNs      = 0;
    LSA_UINT64  MaxStartPostDelayTimeNs     = 0;
    LSA_UINT64  MaxCbfRunTimeNs             = 0;
    LSA_UINT64  StartPreDelayCtr            = 0;
    LSA_UINT64  StartPostDelayCtr           = 0;
    #if defined (EPS_CFG_HP_POLL_MAX_START_DELAY_NS) && (!defined (EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL))
    LSA_UINT32  StartPreDelayOverrunCtr     = 0;
    LSA_UINT32  StartPostDelayOverrunCtr    = 0;
    #endif
    #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON)
    LSA_UINT64  LastStartTimeNs;                    // Start time of last cycle
    #endif
    #endif //EPS_CFG_HP_POLL_DEBUG_MEASURE_ON

    if // High Performance Poll Thread already started?
       (bHpPollThreadAlreadyStarted)
    {
	    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,
	        "eps_tasks_hp_poll_thread_proc_internal: HP Poll Thread started twice");
        EPS_FATAL(0);
        return;
    }
    else // High Performance Poll Thread not started yet
    {
        bHpPollThreadAlreadyStarted = LSA_TRUE;
    }

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,
	    "eps_tasks_hp_poll_thread_proc_internal: HP Poll Thread started with EDDS NEA");
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,
	    "eps_tasks_hp_poll_thread_proc_internal: HP Poll Thread started with EDDS non-NEA");
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

    // read and store current StartTime
    StartTimeNs = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;

    // preset LastReferenceTime
    LastReferenceTimeNs = StartTimeNs - EPS_CFG_HP_POLL_CYCLE_TIME_NS;

    #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON) && defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON)
    LastStartTimeNs = LastReferenceTimeNs;
    #endif

	while (pThread->ThreadArgs.bPollThreadAlive)
	{
        // read and store current StartTime
        StartTimeNs = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;

        #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
        #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON)
        // trace times of last cycle // do not change location!
	    EPS_SYSTEM_TRACE_09(0, LSA_TRACE_LEVEL_NOTE_HIGH,
	        "eps_tasks_hp_poll_thread_proc_internal: normal operation, StartPreDelayNs(%u) StartPostDelayNs(%u) CbfRunTimeNs(%u) SleepTimeNs(%u) MaxStartPreDelayTimeNs(%u) MaxStartPostDelayTimeNs(%u) MaxCbfRunTimeNs(%u) StartTimeNs(High:0x%x)(Low:0x%x)",
	        StartPreDelayTimeNs, StartPostDelayTimeNs, CbfRunTimeNs, SleepTimeNs, MaxStartPreDelayTimeNs, MaxStartPostDelayTimeNs, MaxCbfRunTimeNs, (LSA_UINT32)(LastStartTimeNs>>32), (LSA_UINT32)LastStartTimeNs);
        #endif //EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON
        StartDelayTimeNs = StartTimeNs - LastReferenceTimeNs;
        if // trigger is too early?
           (StartDelayTimeNs < EPS_CFG_HP_POLL_CYCLE_TIME_NS)
        {
            StartPreDelayCtr++;
            StartPreDelayTimeNs  = EPS_CFG_HP_POLL_CYCLE_TIME_NS - StartDelayTimeNs;
            StartPostDelayTimeNs = 0;
            // check if new maximum
            if (StartPreDelayTimeNs > MaxStartPreDelayTimeNs)
            {
                // store new maximum
                MaxStartPreDelayTimeNs = StartPreDelayTimeNs;
            }
            #if defined (EPS_CFG_HP_POLL_MAX_START_DELAY_NS)
            if // StartPreDelay exceeds maximum limit?
               (StartPreDelayTimeNs > EPS_CFG_HP_POLL_MAX_START_DELAY_NS)
            {
                #if defined (EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL)
	            EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL,
	                "eps_tasks_hp_poll_thread_proc_internal: StartPreDelay exceeds maximum, StartPreDelayNs(%u) MaxLimitStartPreDelayNs(%u) FixPollCycleTimeNs(%u) LastSleepTimeNs(%u)",
	                StartPreDelayTimeNs, EPS_CFG_HP_POLL_MAX_START_DELAY_NS, EPS_CFG_HP_POLL_CYCLE_TIME_NS, SleepTimeNs);
                EPS_FATAL(0);
                //leave loop
				break;
                #elif !defined (EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL)
                StartPreDelayOverrunCtr++;
	            EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_WARN,
	                "eps_tasks_hp_poll_thread_proc_internal: StartPreDelay exceeds maximum, StartPreDelayNs(%u) MaxLimitStartPreDelayNs(%u) FixPollCycleTimeNs(%u) LastSleepTimeNs(%u)",
	                StartPreDelayTimeNs, EPS_CFG_HP_POLL_MAX_START_DELAY_NS, EPS_CFG_HP_POLL_CYCLE_TIME_NS, SleepTimeNs);
                #endif //EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL
            }
            #endif //EPS_CFG_HP_POLL_MAX_START_DELAY_NS
        }
        else // trigger is too late
        {
            StartPostDelayCtr++;
            StartPreDelayTimeNs  = 0;
            StartPostDelayTimeNs = StartDelayTimeNs - EPS_CFG_HP_POLL_CYCLE_TIME_NS;
            // check if new maximum
            if (StartPostDelayTimeNs > MaxStartPostDelayTimeNs)
            {
                // store new maximum
                MaxStartPostDelayTimeNs = StartPostDelayTimeNs;
            }
            #if defined (EPS_CFG_HP_POLL_MAX_START_DELAY_NS)
            if // StartPostDelay exceeds maximum limit?
               (StartPostDelayTimeNs > EPS_CFG_HP_POLL_MAX_START_DELAY_NS)
            {
                #if defined (EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL)
	            EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL,
	                "eps_tasks_hp_poll_thread_proc_internal: StartPostDelay exceeds maximum, StartPostDelayNs(%u) MaxLimitStartPostDelayNs(%u) FixPollCycleTimeNs(%u) LastSleepTimeNs(%u)",
	                StartPostDelayTimeNs, EPS_CFG_HP_POLL_MAX_START_DELAY_NS, EPS_CFG_HP_POLL_CYCLE_TIME_NS, SleepTimeNs);
                EPS_FATAL(0);
                //leave loop
				break;
                #elif !defined (EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL)
                StartPostDelayOverrunCtr++;
	            EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_WARN,
	                "eps_tasks_hp_poll_thread_proc_internal: StartPostDelay exceeds maximum, StartPostDelayNs(%u) MaxLimitStartPostDelayNs(%u) FixPollCycleTimeNs(%u) LastSleepTimeNs(%u)",
	                StartPostDelayTimeNs, EPS_CFG_HP_POLL_MAX_START_DELAY_NS, EPS_CFG_HP_POLL_CYCLE_TIME_NS, SleepTimeNs);
                #endif //EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL
            }
            #endif //EPS_CFG_HP_POLL_MAX_START_DELAY_NS
        }
        #endif //EPS_CFG_HP_POLL_DEBUG_MEASURE_ON

        // CBF
		pThread->ThreadArgs.pCbf(pThread->ThreadArgs.uParam, pThread->ThreadArgs.pArgs);

        // read and store current StopTime
        StopTimeNs = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;

        #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
        CbfRunTimeNs = StopTimeNs - StartTimeNs;
        // check if new maximum
        if (CbfRunTimeNs > MaxCbfRunTimeNs)
        {
            // store new maximum
            MaxCbfRunTimeNs = CbfRunTimeNs;
        }
        #endif //EPS_CFG_HP_POLL_DEBUG_MEASURE_ON

        // calculate difference against last reference time
        DiffTimeNs = StopTimeNs - LastReferenceTimeNs;

        // calculate temporary time for faster calculations
        TmpTimeNs = ((LSA_UINT64)2 * EPS_CFG_HP_POLL_CYCLE_TIME_NS) - EPS_CFG_HP_POLL_RUNTIME_COMPENSATION_NS;

        if // cycle overrun occurs?
           (DiffTimeNs >= TmpTimeNs)
        {
            //****** poll cycle overrun ******//

            CycleOverrunCtr++;

            #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
            // difference times are already calculated above
            #elif !defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
            // calculate several difference times
            StartDelayTimeNs = StartTimeNs - LastReferenceTimeNs;
            if // trigger is too early?
               (StartDelayTimeNs < EPS_CFG_HP_POLL_CYCLE_TIME_NS)
            {
                StartPreDelayTimeNs  = EPS_CFG_HP_POLL_CYCLE_TIME_NS - StartDelayTimeNs;
                StartPostDelayTimeNs = 0;
            }
            else // trigger is too late
            {
                StartPreDelayTimeNs  = 0;
                StartPostDelayTimeNs = StartDelayTimeNs - EPS_CFG_HP_POLL_CYCLE_TIME_NS;
            }
            CbfRunTimeNs = StopTimeNs - StartTimeNs;
            #endif //EPS_CFG_HP_POLL_DEBUG_MEASURE_ON

            #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON) && defined (EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL)
	        EPS_SYSTEM_TRACE_05(0, LSA_TRACE_LEVEL_FATAL,
	            "eps_tasks_hp_poll_thread_proc_internal: Poll Thread Cycle Overrun, StartPreDelayNs(%u) StartPostDelayNs(%u) CbfRunTimeNs(%u) FixPollCycleTimeNs(%u) CycleOverrunCtr(%u)",
	            StartPreDelayTimeNs, StartPostDelayTimeNs, CbfRunTimeNs, EPS_CFG_HP_POLL_CYCLE_TIME_NS, CycleOverrunCtr);
            if //timer tick overrun occurs?
               (   (StartTimeNs < LastReferenceTimeNs)
                || (StopTimeNs  < StartTimeNs))
            {
	            EPS_SYSTEM_TRACE_06(0, LSA_TRACE_LEVEL_FATAL,
	                "eps_tasks_hp_poll_thread_proc_internal: Timer Tick Overrun, LastReferenceTimeNs(High:0x%x)(Low:0x%x) StartTimeNs(High:0x%x)(Low:0x%x) StopTimeNs(High:0x%x)(Low:0x%x)",
	                (LSA_UINT32)(LastReferenceTimeNs>>32), (LSA_UINT32)LastReferenceTimeNs, (LSA_UINT32)(StartTimeNs>>32), (LSA_UINT32)StartTimeNs, (LSA_UINT32)(StopTimeNs>>32), (LSA_UINT32)StopTimeNs);
            }
            EPS_FATAL(0);
            //leave loop
			break;
            #else
	        EPS_SYSTEM_TRACE_05(0, LSA_TRACE_LEVEL_WARN,
	            "eps_tasks_hp_poll_thread_proc_internal: Poll Thread Cycle Overrun, StartPreDelayNs(%u) StartPostDelayNs(%u) CbfRunTimeNs(%u) FixPollCycleTimeNs(%u) CycleOverrunCtr(%u)",
	            StartPreDelayTimeNs, StartPostDelayTimeNs, CbfRunTimeNs, EPS_CFG_HP_POLL_CYCLE_TIME_NS, CycleOverrunCtr);
            if //timer tick overrun occurs?
               (   (StartTimeNs < LastReferenceTimeNs)
                || (StopTimeNs  < StartTimeNs))
            {
	            EPS_SYSTEM_TRACE_06(0, LSA_TRACE_LEVEL_WARN,
	                "eps_tasks_hp_poll_thread_proc_internal: Timer Tick Overrun, LastReferenceTimeNs(High:0x%x)(Low:0x%x) StartTimeNs(High:0x%x)(Low:0x%x) StopTimeNs(High:0x%x)(Low:0x%x)",
	                (LSA_UINT32)(LastReferenceTimeNs>>32), (LSA_UINT32)LastReferenceTimeNs, (LSA_UINT32)(StartTimeNs>>32), (LSA_UINT32)StartTimeNs, (LSA_UINT32)(StopTimeNs>>32), (LSA_UINT32)StopTimeNs);
            }
            // calculate new sleep time
            SleepTimeNs = EPS_CFG_HP_POLL_CYCLE_TIME_NS - EPS_CFG_HP_POLL_RUNTIME_COMPENSATION_NS;
            // update LastReferenceTime
            LastReferenceTimeNs = StopTimeNs;
            #endif
        }
        else
        {
            //****** normal operation ******//

            // calculate new sleep time
            SleepTimeNs = TmpTimeNs - DiffTimeNs;

            // update LastReferenceTime
            LastReferenceTimeNs += EPS_CFG_HP_POLL_CYCLE_TIME_NS;
        }

		//eps_tasks_usleep(uTimeUs);
        {
	        struct timespec SleepSpec;

            EPS_ASSERT(SleepTimeNs < (LSA_UINT64)0x80000000);

            //Attention: tv_nsec is of type long!
	        SleepSpec.tv_sec  = 0;
	        SleepSpec.tv_nsec = (long)SleepTimeNs;

	        EPS_POSIX_NANOSLEEP(&SleepSpec, LSA_NULL);
        }

        #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON) && defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_CYCLIC_ON)
        LastStartTimeNs = StartTimeNs;
        #endif

		if (g_pEpsThreads->bFatalError) //On fatal poll thread has to be stopped!
		{
			pThread->ThreadArgs.bPollThreadAlive = LSA_FALSE;
            //leave loop
			break;
		}
	} //while-loop

    #if defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON) && defined (EPS_CFG_HP_POLL_MAX_START_DELAY_NS) && (!defined (EPS_CFG_HP_POLL_DEBUG_NEG_CHECKS_TO_FATAL))
	EPS_SYSTEM_TRACE_11(0, LSA_TRACE_LEVEL_WARN,
	    "eps_tasks_hp_poll_thread_proc_internal: leaving poll thread, MaxStartPreDelayTimeNs(%u) MaxStartPostDelayTimeNs(%u) MaxCbfRunTimeNs(%u) FixPollCycleTimeNs(%u) StartPreDelayCtr(High:0x%x)(Low:0x%x) StartPostDelayCtr(High:0x%x)(Low:0x%x) CycleOverrunCtr(%u) StartPreDelayOverrunCtr(%u) StartPostDelayOverrunCtr(%u)",
	    MaxStartPreDelayTimeNs, MaxStartPostDelayTimeNs, MaxCbfRunTimeNs, EPS_CFG_HP_POLL_CYCLE_TIME_NS, (LSA_UINT32)(StartPreDelayCtr>>32), (LSA_UINT32)StartPreDelayCtr, (LSA_UINT32)(StartPostDelayCtr>>32), (LSA_UINT32)StartPostDelayCtr, CycleOverrunCtr, StartPreDelayOverrunCtr, StartPostDelayOverrunCtr);
    #elif defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
	EPS_SYSTEM_TRACE_09(0, LSA_TRACE_LEVEL_WARN,
	    "eps_tasks_hp_poll_thread_proc_internal: leaving poll thread, MaxStartPreDelayTimeNs(%u) MaxStartPostDelayTimeNs(%u) MaxCbfRunTimeNs(%u) FixPollCycleTimeNs(%u) StartPreDelayCtr(High:0x%x)(Low:0x%x) StartPostDelayCtr(High:0x%x)(Low:0x%x) CycleOverrunCtr(%u)",
	    MaxStartPreDelayTimeNs, MaxStartPostDelayTimeNs, MaxCbfRunTimeNs, EPS_CFG_HP_POLL_CYCLE_TIME_NS, (LSA_UINT32)(StartPreDelayCtr>>32), (LSA_UINT32)StartPreDelayCtr, (LSA_UINT32)(StartPostDelayCtr>>32), (LSA_UINT32)StartPostDelayCtr, CycleOverrunCtr);
    #elif !defined (EPS_CFG_HP_POLL_DEBUG_MEASURE_ON)
	EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_WARN,
	    "eps_tasks_hp_poll_thread_proc_internal: leaving poll thread, FixPollCycleTimeNs(%u) CycleOverrunCtr(%u)",
	    EPS_CFG_HP_POLL_CYCLE_TIME_NS, CycleOverrunCtr);
    #endif

    bHpPollThreadAlreadyStarted = LSA_FALSE;
}

#endif //EDDS_CFG_HW_INTEL || EDDS_CFG_HW_PACKET32 || defined (EDDS_CFG_HW_KSZ88XX)

/**
 * 
 * @param pThread
 * @return
 */
static LSA_UINT16 eps_tasks_start_thread_internal(EPS_THREAD_PTR_TYPE pThread)
{
	LSA_INT ret_val;
	struct sched_param th_param; 
	pthread_attr_t th_attr; 
	char thread_name_id[100];
	
    if (eps_strlen((const LSA_CHAR*)pThread->pName) > EPS_TASKS_MAX_THREAD_NAME_SIZE)
    {
        EPS_FATAL("eps_tasks_start_thread_internal(): Threadname is too long!");
    }

	// Init Sync event if needed
	if( pThread->Startup.bWaitTillStarted )
	{
		ret_val = eps_alloc_event(&pThread->Startup.hStartedEvent);
		EPS_ASSERT(ret_val == LSA_RET_OK);
	}

	pThread->eState = eThStarting;

	// Init thread attributes
	ret_val = EPS_POSIX_PTHREAD_ATTR_INIT(&th_attr);
	EPS_ASSERT(ret_val == 0);

	// Enable explicitly to set individual scheduling parameters
	EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED(&th_attr, PTHREAD_EXPLICIT_SCHED);

    #if (EPS_PLF == EPS_PLF_LINUX_X86)
	// Set scheduling policy to RR
	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY(&th_attr, SCHED_FIFO);
	EPS_ASSERT(ret_val == 0);
    #endif

	// Set stack and stack size
	#ifdef EPS_CFG_USE_LOCAL_STACK_FOR_THREADS
	pThread->pStack = (LSA_UINT8*)eps_mem_alloc( pThread->uStackSize, LSA_COMP_ID_PNBOARDS,  PSI_MTYPE_IGNORE );
	EPS_ASSERT(pThread->pStack != NULL);
	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSTACK(&th_attr, pThread->pStack, pThread->uStackSize);
	EPS_ASSERT(ret_val == 0);
	#else
	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSTACKSIZE(&th_attr, pThread->uStackSize);
	#endif
	   
	// Set priority
    th_param.sched_priority = pThread->Prio;

	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM(&th_attr, &th_param);
	EPS_ASSERT(ret_val == 0);

    #if (EPS_PLF != EPS_PLF_LINUX_X86)
	// Set scheduling policy
	EPS_ASSERT(pThread->eSchedType != eSchedInvalid);
	if( pThread->eSchedType == eSchedRR )
	{
	    ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY(&th_attr, SCHED_RR);
	}
	else
	{
		ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY(&th_attr, SCHED_FIFO);
	}
	EPS_ASSERT(ret_val == 0);

	// Set name
	ret_val = pthread_attr_setname(&th_attr, pThread->pName);
	EPS_ASSERT(ret_val == 0);
    #endif

	// Set argument to the task data structure used for creation
	ret_val = EPS_POSIX_PTHREAD_CREATE(&(pThread->hOsHandle), &th_attr, eps_tasks_thread_proc_internal, pThread);
	EPS_ASSERT(ret_val == 0);
	
	
	eps_sprintf((LSA_UINT8*)thread_name_id, "Name: %s ID: %d", pThread->pName, pThread->hOsHandle);
	
	EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_tasks_start_thread_internal() => Started Thread: (%s)", thread_name_id);

    #if (EPS_PLF == EPS_PLF_LINUX_X86)
    // Set name
    ret_val = pthread_setname_np(pThread->hOsHandle, pThread->pName);
    EPS_ASSERT(ret_val == 0);
    #endif

	// Wait for sync event if needed
	if( pThread->Startup.bWaitTillStarted )
	{
		eps_wait_event(pThread->Startup.hStartedEvent, PSI_EVENT_NO_TIMEOUT);
		eps_reset_event(pThread->Startup.hStartedEvent);
		ret_val = eps_free_event(pThread->Startup.hStartedEvent);
		EPS_ASSERT(ret_val == LSA_RET_OK);
		pThread->Startup.hStartedEvent = 0;
	}

	return LSA_RET_OK;
}

static EPS_THREAD_PTR_TYPE eps_tasks_start_psi_thread(EPS_PSI_THREAD_INIT_DATA_TYPE InitData)
{
	EPS_THREAD_PTR_TYPE pThread;
	LSA_UINT16 retVal;

	pThread = eps_tasks_create_thread_internal(InitData.pName, InitData.Prio, InitData.eSchedType, 0, EPS_TASKS_PSI_POOL);
	EPS_ASSERT(pThread != LSA_NULL);

	pThread->ThreadArgs.uThreadType   = EPS_TASKS_THREAD_TYPE_PSI;
	pThread->ThreadArgs.uParam        = InitData.uPsiThreadId;
	pThread->Startup.bWaitTillStarted = LSA_TRUE;

	retVal = eps_tasks_start_thread_internal(pThread);
	EPS_ASSERT(retVal == LSA_RET_OK);
	
	return pThread;
}

static LSA_VOID eps_tasks_stop_psi_threads_internal(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_THREAD_PTR_TYPE pThread;
	
	eps_tasks_enter();

	for(i = 0; i < EPS_MAX_PSI_THREADS; i++)
	{
		pThread = &g_pEpsThreads->sPsiThread[i];

		if( (pThread->eState != eThStopped) && (pThread->eState != eThInit) )
		{
			eps_tasks_stop_thread_internal(pThread);
		}
	}
	
	eps_tasks_exit();
}

LSA_VOID eps_tasks_start_psi_threads(LSA_VOID)
{
    EPS_PSI_THREAD_INIT_DATA_PTR_TYPE pInitData = g_pEpsThreads->pPsiThreadInitData;
	LSA_UINT32                        i;
	EPS_THREAD_PTR_TYPE               pThread;

	eps_tasks_enter();

	for(i = 0; i < EPS_MAX_PSI_THREADS; i++)
	{
		pThread = eps_tasks_start_psi_thread(pInitData[i]);   
        #ifndef EPS_CFG_DO_NOT_USE_TGROUPS
		if (pInitData[i].eTGroup != EPS_TGROUP_NONE)
		{
			eps_tasks_group_thread_add_internal(pThread, pInitData[i].eTGroup);
		}
        #endif
	}

	eps_tasks_exit();
}

/**
 * Check if threadpool is empty
 * 
 * @param uPoolId  Pool id to be checked. Can be:
 * - #EPS_TASKS_PSI_POOL 
 * - #EPS_TASKS_USR_POOL
 * @return LSA_TRUE   if all Threads are used
 * @return LSA_FALSE  if there is at least one unused thread left
 */
static LSA_BOOL eps_tasks_check_thread_pool_empty_internal(LSA_UINT32 uPoolId)
{
	EPS_THREAD_PTR_TYPE pThread = LSA_NULL;
	LSA_UINT32 uCnt = 0;
	LSA_UINT32 i;

	switch(uPoolId)
	{
		case EPS_TASKS_PSI_POOL:
			pThread = g_pEpsThreads->sPsiThread;
			uCnt = EPS_MAX_PSI_THREADS;
			break;
		case EPS_TASKS_USR_POOL:
			pThread = g_pEpsThreads->sUsrThread;
			uCnt = EPS_MAX_USR_THREADS;
			break;
		default:
			EPS_FATAL("unknown threadpool to be check for unused threads");
			break;
	}

	for( i = 0; i < uCnt; i++ )
	{
		if(pThread[i].hThread != 0) /* Unused? */
		{
			return LSA_FALSE;
		}
	}

	return LSA_TRUE;
}

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
static LSA_BOOL eps_tasks_check_tgroup_empty_internal(EPS_TGROUP_STRUCT_TYPE tGroup)
{	
	if (tGroup.nr_threads_in_group != 0)
	{
		return LSA_FALSE;
	}	
	
	return LSA_TRUE;	
}
#endif

static EPS_THREAD_PTR_TYPE eps_tasks_get_user_thread(LSA_UINT32 hThread)
{
	EPS_THREAD_PTR_TYPE pThread = LSA_NULL;
	LSA_UINT32 i;

	for ( i = 0; i < EPS_MAX_USR_THREADS; i++ )
	{
		pThread = &g_pEpsThreads->sUsrThread[i];
			
		if (pThread->hThread == hThread)
		{
			return pThread;
		}
	}

	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/

LSA_VOID eps_tasks_init (LSA_VOID)
{
	LSA_UINT16 i, result;
	EPS_ASSERT(g_pEpsThreads == LSA_NULL);

	g_pEpsThreads = &g_EpsThreads;

	eps_memset(g_pEpsThreads, 0, sizeof(*g_pEpsThreads));

	g_pEpsThreads->bFatalError = LSA_FALSE;

	eps_tasks_init_critical_section();

	g_pEpsThreads->pPsiThreadInitData = pnioThreads;

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	//create thread groups
	for (i=1;i<EPS_TGROUP_MAX;i++)
	{
		result = tgroup_create(&g_pEpsThreads->sTGroup[i].OS_tgroup_id, TGROUP_PROCESSING_TIME_QUOTA);
		EPS_ASSERT(result==0);		
	}

	result |= tgroup_set_quota_credit_donation(g_pEpsThreads->sTGroup[EPS_TGROUP_NRT].OS_tgroup_id,g_pEpsThreads->sTGroup[EPS_TGROUP_PSI].OS_tgroup_id);
	result |= tgroup_set_collision_prevention(g_pEpsThreads->sTGroup[EPS_TGROUP_NRT].OS_tgroup_id,1);
	EPS_ASSERT(result==0);

	result = eps_tasks_thrd_asynch_grp_setquota(EPS_TASK_COMMUNICATIONLOAD_DEFAULT,EPS_TASK_COMMUNICATIONLOAD_PEAK_DEFAULT);
	EPS_ASSERT(result!=-1);
#else
	LSA_UNUSED_ARG (i);
	LSA_UNUSED_ARG (result);
#endif
}

LSA_VOID eps_tasks_undo_init (LSA_VOID)
{
	LSA_BOOL bFree;
	LSA_UINT16 i, result;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

	eps_tasks_stop_psi_threads_internal();

	bFree = eps_tasks_check_thread_pool_empty_internal(EPS_TASKS_PSI_POOL);
	EPS_ASSERT(bFree);

	bFree = eps_tasks_check_thread_pool_empty_internal(EPS_TASKS_USR_POOL);
	EPS_ASSERT(bFree);

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	for (i=1;i<EPS_TGROUP_MAX;i++)
	{
		// check if thread groups are empty
		bFree = eps_tasks_check_tgroup_empty_internal(g_pEpsThreads->sTGroup[i]);
		EPS_ASSERT(bFree);

		// delete the thread groups
		result = tgroup_delete(g_pEpsThreads->sTGroup[i].OS_tgroup_id);

		EPS_ASSERT(result == 0);
	}
#else
	LSA_UNUSED_ARG (i);
	LSA_UNUSED_ARG (result);
#endif

	eps_tasks_exit();

	eps_tasks_undo_init_critical_section();

	g_pEpsThreads = LSA_NULL;
}

LSA_UINT32 eps_tasks_get_current_psi_thread_id( LSA_VOID )
{
	LSA_UINT16 idx;
	LSA_UINT32 psi_thread_id = PSI_MAX_TASK_ID;
	
	///@todo BU: Check if curId is valid or not (0xFFFF)
	LSA_UINT32 curId = pthread_self();

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	for ( idx = 0; idx < PSI_MAX_TASK_ID; idx++ )
	{
		if ( curId == g_pEpsThreads->sPsiThread[idx].hOsHandle )
		{
			psi_thread_id = g_pEpsThreads->sPsiThread[idx].ThreadArgs.uParam;
			break;
		}
	}

	return psi_thread_id;
}

LSA_VOID eps_tasks_signal_psi_thread_ready ( LSA_VOID_PTR_TYPE arg )
{
	EPS_THREAD_PTR_TYPE pThread = (EPS_THREAD_PTR_TYPE)arg;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	EPS_ASSERT(pThread != LSA_NULL);
}

LSA_VOID eps_tasks_signal_psi_thread_stopped ( LSA_VOID_PTR_TYPE arg )
{
	EPS_THREAD_PTR_TYPE pThread = (EPS_THREAD_PTR_TYPE)arg;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	EPS_ASSERT(pThread != LSA_NULL);
}

LSA_UINT32 eps_tasks_start_thread ( LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_THREAD_PTR_TYPE pThread;
	LSA_UINT16 retVal;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

	pThread = eps_tasks_create_thread_internal(pName, Prio, eSchedType, 0, EPS_TASKS_USR_POOL);
	EPS_ASSERT(pThread != LSA_NULL);

	pThread->ThreadArgs.uThreadType   = EPS_TASKS_THREAD_TYPE_USR;
	pThread->ThreadArgs.uParam        = uParam;
	pThread->ThreadArgs.pArgs         = pArgs;
	pThread->ThreadArgs.pCbf          = pCbf;
	pThread->Startup.bWaitTillStarted = LSA_TRUE;

	retVal = eps_tasks_start_thread_internal(pThread);
	EPS_ASSERT(retVal == LSA_RET_OK);

	eps_tasks_exit();

	return pThread->hThread;
}

LSA_VOID eps_tasks_stop_thread ( LSA_UINT32 hThread )
{
	EPS_THREAD_PTR_TYPE pThread;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

	pThread = eps_tasks_get_user_thread(hThread);
	EPS_ASSERT(pThread != LSA_NULL);

	eps_tasks_stop_thread_internal(pThread);

	eps_tasks_exit();
}

/**
 * start event thread
 * 
 * creates a thread in #EPS_TASKS_USR_POOL and sets all data up (name, prio, callback function)
 * 
 * @param pName Name of thread
 * @param Prio  Priority of thread
 * @param eSchedType  SchedulingPolicy (RoundRobin or Fifo)
 * @param pCbf
 * @param uParam
 * @param pArgs
 * @return
 */
LSA_UINT32 eps_tasks_start_event_thread ( LSA_CHAR* pName, LSA_INT Prio,EPS_THREAD_SCHEDULING_TYPE eSchedType, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs )
{
#ifdef EPS_TASKS_USE_EVENT_TASKS
	EPS_THREAD_PTR_TYPE pThread;
	LSA_UINT16 retVal;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

	pThread = eps_tasks_create_thread_internal(pName, Prio, eSchedType, 0, EPS_TASKS_USR_POOL);
	EPS_ASSERT(pThread != LSA_NULL);

	pThread->ThreadArgs.uThreadType   = EPS_TASKS_THREAD_TYPE_EVENT;
	pThread->ThreadArgs.uParam        = uParam;
	pThread->ThreadArgs.pArgs         = pArgs;
	pThread->ThreadArgs.pCbf          = pCbf;
	pThread->ThreadArgs.uSignal       = SIGRT1;
	pThread->Startup.bWaitTillStarted = LSA_TRUE;

	retVal = eps_tasks_start_thread_internal(pThread);
	EPS_ASSERT(retVal == LSA_RET_OK);

	eps_tasks_exit();

	return pThread->hThread;
#else
	LSA_UNUSED_ARG(pName);
	LSA_UNUSED_ARG(Prio);
	LSA_UNUSED_ARG(pCbf);
	LSA_UNUSED_ARG(uParam);
	LSA_UNUSED_ARG(pArgs);
    LSA_UNUSED_ARG(eSchedType);

  EPS_FATAL("function not supported when not using event tasks"); /* not supported */
	return 0;
#endif
}

LSA_VOID eps_tasks_trigger_event_thread ( LSA_UINT32 hThread )
{
#ifdef EPS_TASKS_USE_EVENT_TASKS
	EPS_THREAD_PTR_TYPE pThread;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	pThread = eps_tasks_get_user_thread(hThread);
	EPS_ASSERT(pThread != LSA_NULL);
	EPS_ASSERT(pThread->ThreadArgs.uThreadType == EPS_TASKS_THREAD_TYPE_EVENT);

	EPS_POSIX_PTHREAD_KILL(pThread->hOsHandle, pThread->ThreadArgs.uSignal);
#else
	LSA_UNUSED_ARG(hThread);

	EPS_FATAL("eps_tasks_trigger_event_thread(): function not supported when not using event tasks"); /* not supported */
#endif
}

LSA_VOID eps_tasks_stop_event_thread ( LSA_UINT32 hThread )
{
#ifdef EPS_TASKS_USE_EVENT_TASKS
	EPS_THREAD_PTR_TYPE pThread;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

	pThread = eps_tasks_get_user_thread(hThread);
	EPS_ASSERT(pThread != LSA_NULL);
	EPS_ASSERT(pThread->ThreadArgs.uThreadType == EPS_TASKS_THREAD_TYPE_EVENT);

	EPS_POSIX_PTHREAD_KILL(pThread->hOsHandle, SIGRT0);

	eps_tasks_stop_thread_internal(pThread);

	eps_tasks_exit();
#else
	LSA_UNUSED_ARG(hThread);

	EPS_FATAL("eps_tasks_stop_event_thread(): function not supported when not using event tasks"); /* not supported */
#endif
}

LSA_UINT32 eps_tasks_start_poll_thread ( LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, LSA_UINT32 uIntervalMs, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_THREAD_PTR_TYPE pThread;
	LSA_UINT16 retVal;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

    if (pName != NULL)
    {
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_tasks_start_poll_thread() => Thread: (%s)", pName);
    }

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_tasks_start_poll_thread(): uIntervalMs = (%u), Prio = (%d)", uIntervalMs, Prio);

	pThread = eps_tasks_create_thread_internal(pName, Prio, eSchedType, 0, EPS_TASKS_USR_POOL);
	EPS_ASSERT(pThread != LSA_NULL);
	
	pThread->ThreadArgs.uThreadType   = EPS_TASKS_THREAD_TYPE_POLL_USR;
	pThread->ThreadArgs.uParam        = uParam;
	pThread->ThreadArgs.pArgs         = pArgs;
	pThread->ThreadArgs.pCbf          = pCbf;
	pThread->ThreadArgs.uIntervalMs   = uIntervalMs;
	pThread->ThreadArgs.uIntervalUs   = 0;
	pThread->Startup.bWaitTillStarted = LSA_TRUE;

	retVal = eps_tasks_start_thread_internal(pThread);
	EPS_ASSERT(retVal == LSA_RET_OK);

	eps_tasks_exit();

	return pThread->hThread;
}

LSA_UINT32 eps_tasks_start_us_poll_thread ( LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, LSA_UINT32 uIntervalUs, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_THREAD_PTR_TYPE pThread;
	LSA_UINT16 retVal;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

    if(pName != NULL)
    {
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_tasks_start_us_poll_thread(): Thread: (%s)", pName);
    }

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_tasks_start_us_poll_thread(): uIntervalUs = (%u), Prio = (%d)", uIntervalUs, Prio);

	pThread = eps_tasks_create_thread_internal(pName, Prio, eSchedType, 0, EPS_TASKS_USR_POOL);
	EPS_ASSERT(pThread != LSA_NULL);

	pThread->ThreadArgs.uThreadType   = EPS_TASKS_THREAD_TYPE_POLL_USR;
	pThread->ThreadArgs.uParam        = uParam;
	pThread->ThreadArgs.pArgs         = pArgs;
	pThread->ThreadArgs.pCbf          = pCbf;
	pThread->ThreadArgs.uIntervalMs   = 0;
	pThread->ThreadArgs.uIntervalUs   = uIntervalUs;
	pThread->Startup.bWaitTillStarted = LSA_TRUE;

	retVal = eps_tasks_start_thread_internal(pThread);
	EPS_ASSERT(retVal == LSA_RET_OK);

	eps_tasks_exit();

	return pThread->hThread;
}

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
LSA_VOID eps_tasks_group_thread_add ( LSA_UINT32 hThread, EPS_TGROUPS_ENUM_TYPE eTGroup )
{
	EPS_THREAD_PTR_TYPE pThread;

	EPS_ASSERT(eTGroup != EPS_TGROUP_NONE);

	eps_tasks_enter();

	pThread = eps_tasks_get_user_thread(hThread);
	EPS_ASSERT(pThread != LSA_NULL);	

	eps_tasks_group_thread_add_internal(pThread, eTGroup);

	eps_tasks_exit();
}

LSA_VOID   eps_tasks_group_thread_remove ( LSA_UINT32 hThread )
{
	EPS_THREAD_PTR_TYPE pThread;

	eps_tasks_enter();

	pThread = eps_tasks_get_user_thread(hThread);
	EPS_ASSERT(pThread != LSA_NULL);	

	eps_tasks_group_thread_remove_internal(pThread);

	eps_tasks_exit();
}
#endif

LSA_VOID eps_tasks_stop_poll_thread ( LSA_UINT32 hThread )
{
	EPS_THREAD_PTR_TYPE pThread;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

	pThread = eps_tasks_get_user_thread(hThread);
	EPS_ASSERT(pThread != LSA_NULL);

	pThread->ThreadArgs.bPollThreadAlive = LSA_FALSE;

	eps_tasks_stop_thread_internal(pThread);

	eps_tasks_exit();
}

LSA_VOID eps_tasks_sleep( LSA_UINT32 uTimeMs )
{
	struct timespec SleepSpec;

    //Attention: tv_sec and tv_nsec are of type long!
	SleepSpec.tv_sec  = (uTimeMs / 1000);
	SleepSpec.tv_nsec = (uTimeMs % 1000) * 1000000;

	EPS_POSIX_NANOSLEEP(&SleepSpec, LSA_NULL);
}

LSA_VOID eps_tasks_usleep( LSA_UINT32 uTimeUs )
{
	struct timespec SleepSpec;

    //Attention: tv_sec and tv_nsec are of type long!
	SleepSpec.tv_sec  = (uTimeUs / 1000000);
	SleepSpec.tv_nsec = (uTimeUs % 1000000) * 1000;

	EPS_POSIX_NANOSLEEP(&SleepSpec, LSA_NULL);
}

LSA_VOID eps_tasks_nswait( LSA_UINT64 uTimeNs )
{
    int                   RetVal;
	struct timespec       TargetTime;
	struct timespec       CurrentTime;
    LSA_UINT32      const uTimePartSec = (LSA_UINT32)(uTimeNs / (1000*1000*1000));
    LSA_UINT32      const uTimePartNs  = (LSA_UINT32)(uTimeNs % (1000*1000*1000));

    RetVal = EPS_POSIX_CLOCK_GETTIME(CLOCK_REALTIME, &CurrentTime);
	EPS_ASSERT(0 == RetVal);

    // calculate end-time to wait:
    // current absolute time (64bit Nanoseconds) + Time to wait (Nanoseconds)
	TargetTime.tv_sec   = CurrentTime.tv_sec  + uTimePartSec;
	TargetTime.tv_nsec  = CurrentTime.tv_nsec + uTimePartNs;
	TargetTime.tv_sec  += (TargetTime.tv_nsec / (1000*1000*1000));
	TargetTime.tv_nsec %= (1000*1000*1000);

    // targettime has to be in the future, failure on timer overflow
    EPS_ASSERT(   (TargetTime.tv_sec > CurrentTime.tv_sec)
			   || ((TargetTime.tv_sec == CurrentTime.tv_sec) && (TargetTime.tv_nsec >= CurrentTime.tv_nsec)));

    // active wait
    do
    {
        RetVal = EPS_POSIX_CLOCK_GETTIME(CLOCK_REALTIME, &CurrentTime);
	    EPS_ASSERT(0 == RetVal);

    } while (   (CurrentTime.tv_sec < TargetTime.tv_sec)
			 || ((CurrentTime.tv_sec == TargetTime.tv_sec) && (CurrentTime.tv_nsec < TargetTime.tv_nsec)));
}

LSA_VOID eps_tasks_fatal( LSA_VOID )
{
	if (g_pEpsThreads != LSA_NULL)
	{
		g_pEpsThreads->bFatalError = LSA_TRUE;
	}
}

LSA_VOID * eps_tasks_get_os_handle( LSA_UINT32 hThread )
{
	EPS_THREAD_PTR_TYPE pThread;
	LSA_VOID *          hOs = LSA_NULL;

	EPS_ASSERT(g_pEpsThreads != LSA_NULL);

	eps_tasks_enter();

	pThread = eps_tasks_get_user_thread(hThread);
	EPS_ASSERT(pThread != LSA_NULL);

	hOs = (LSA_VOID *)pThread->hOsHandle;

	eps_tasks_exit();

	return hOs;
}

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
LSA_VOID * eps_tasks_get_group_os_handle( EPS_TGROUPS_ENUM_TYPE eTGroup )
{
	return (LSA_VOID *)&g_pEpsThreads->sTGroup[eTGroup].OS_tgroup_id;
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
