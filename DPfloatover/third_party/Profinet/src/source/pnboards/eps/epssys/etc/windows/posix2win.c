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
/*  F i l e               &F: posix2win.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS (Posix) Interface port to Windows                                    */
/*                                                                           */
/*****************************************************************************/

#include <eps_sys.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <process.h>   /* _beginthreadex() */
#include "posix2win.h"


#define P2W_TIMER_COUNT_MAX     2

// Symbolic priority

typedef struct 
{ 
	UINT              period;
	UINT              timer_ID;	
	LARGE_INTEGER     freq;
	struct sigevent  *pEvp;
} POSIX2WIN_TIMER;

typedef struct
{
    POSIX2WIN_TIMER  posix2win_timer[P2W_TIMER_COUNT_MAX];
    LSA_UINT32       timer_create_count; 
} P2W_TIMER_STORE, *P2W_TIMER_STORE_PTR_TYPE;

typedef struct
{
	LSA_BOOL  bUsed;
	uint32_t  hOsThread;
	HANDLE    hThreadStoppedEvent;
	void      *pThreadArgs;
	void      *(*pStartRoutineCbf)(void*);
} P2W_THREAD_TYPE, *P2W_THREAD_PTR_TYPE;

#define P2W_CFG_MAX_THREADS 50

typedef struct
{
	LSA_BOOL         bInitialised;
	pthread_mutex_t  hLocCritical;
	pthread_mutex_t* hCritical;
	P2W_THREAD_TYPE  Thread[P2W_CFG_MAX_THREADS];
} P2W_THREAD_STORE, *P2W_THREAD_STORE_PTR_TYPE;

static P2W_THREAD_STORE g_P2wThreads = {0};
static P2W_TIMER_STORE g_P2wTimer = {0};

/**
 * Determines the length of the string s
 *
 * @param s String to be inspected
 *
 * @return Length of the string s
 */
static size_t eps_lib_strlen(const char *s)
{
	size_t len = 0;

	// Determine length
	while (*s != '\0')
	{
		len++;
		s++;
	}

	return len;
}

/**
 * Check if name contains only allowed characters.
 *
 * @param name Name to check
 *
 * @return 0 if name is ok, -1 otherwise
 */
static int eps_lib_checkname (const char *name)
{
	int i;
	int name_len;

	// Check character set
	name_len = eps_lib_strlen(name);
	for(i=0; i<name_len; i++)
	{
		if (   !((name[i]>='A') && (name[i]<='Z'))
			&& !((name[i]>='a') && (name[i]<='z'))
			&& !((name[i]>='0') && (name[i]<='9'))
			&&  (name[i]!='.')
			&&  (name[i]!='_')
			&&  (name[i]!='-'))
		{
			// return error code
			return -1;
		}
	}
	return 0;
}

/**
 * Copies the string s2 into the string s1 up to size
 *
 * @param s1   String to fill
 * @param s2   String to copy into s1
 * @param size Number of bytes to copy
 *
 * @return s1
 */
static char *eps_lib_strncpy(char * s1, const char * s2, size_t size)
{
	char *ret = s1;
	size_t cnt = 0;

	while(cnt < size)
	{
		*s1 = *s2;
		if ('\0' == *s2)
		{
			return ret;
		}
		s1++;
		s2++;
		cnt++;
	}
	return ret;
}

/**
 * Description.
 *
 * @param rqtp time to wait
 * @param rmtp timespec structure where remaining time is written to
 *
 * @return
 */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{  
	int sleep_time_ms;
	LSA_UNUSED_ARG(rmtp);

	if ((rqtp->tv_nsec<0) || (rqtp->tv_nsec>=MAX_NANO_SEC))
	{
	    errno=EINVAL;
		return -1;
	}

	sleep_time_ms = rqtp->tv_nsec / 1000000 + rqtp->tv_sec * 1000;

	if (sleep_time_ms == 0)
	{
		sleep_time_ms = 1;
	}

	Sleep(sleep_time_ms);
		
	return 0;

}

/**
 * Description.
 *
 * @param mutex Mutex to be destroyed
 *
 * @return 0 or error code
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	DeleteCriticalSection( mutex );

	return 0;
}

/*
 * Description.
 *
 * @param mutex Mutex to be initialized
 * @param attr Attribute set for mutex
 *
 * @return 0 or error code
 */
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr)
{  	
	InitializeCriticalSection( mutex );

	LSA_UNUSED_ARG(attr);

	return 0;
}

/**
 * Description.
 *
 * @param mutex Mutex to be locked
 *
 * @return 0 or error code
 */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	EnterCriticalSection( mutex );

	return 0;
}

/**
 * Description.
 *
 * @param mutex Mutex to be unlocked
 *
 * @return 0 or error code
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	LeaveCriticalSection( mutex );

	return 0;
}

/**
 * Initializes the given thread attributes.
 * The results of otherwise referencing the object after it has been instantiated
 * are undefined.
 * The pthread_attr_init() function shall initialize the thread attribute
 * object attr such as if a pthread_attr_setinheritsched() function  with the
 * inheritsched flag set to PTHREAD_EXPLICIT_SCHED was executed. This specifies that
 * when used by pthread_create() the  thread scheduling attributes shall be set to
 * the  values from this attributes object.
 * All other attributes are not set to a defined value, they must be set explicitly
 * by the corresponding pthread_attr_setstack(), pthread_attr_setschedparam()
 * and pthread_attr_setschedpolicy()
 * \n
 * The cpuset member of the attribute is set so that the thread could run on all
 * CPUs.
 *
 * @see #pthread_attr_setstack
 * @see #pthread_attr_setschedparam
 * @see #pthread_attr_setschedpolicy
 *
 * @param attr Pointer to the thread attribute object to be initialized
 *
 * @return 0 if the creation of the thread was successful, an errorcode otherwise.
 *          Errors:
 *          - #ENOMEM Insufficient memory exists to initialize the thread attributes object.
 */
int pthread_attr_init(pthread_attr_t *attr)
{
	eps_memset(attr,0,sizeof(pthread_attr_t));  
	// Set the inheritsched to INHERIT. This specifies that
	// when pthread_create() is called, the scheduling attributes are set to the
	// ones of the calling thread. The values specified in this attribute object will have no effect.
	attr->inheritsched = PTHREAD_INHERIT_SCHED;
	// per default all new threads are joinable
	attr->detachstate = PTHREAD_CREATE_JOINABLE;
	attr->stacksize = EPS_SYS_THREAD_STACK_SIZE_BYTE;
	// set the stack address to NULL, it will then be allocated using the kernel malloc
	attr->stackaddr=NULL;

    attr->schedparam.sched_priority = P2W_SYS_THREAD_DEFAULT_PRIO;
    attr->schedpolicy               = P2W_SYS_THREAD_DEFAULT_SCHED;

    attr->stack_dynamically_allocated = LSA_FALSE;

	eps_lib_strncpy(attr->name, "", OS_THREAD_NAME_LEN);

	return 0;
}

/**
 * Sets the inheritsched attribute in the \c attr argument.
 * When the attributes objects are used by pthread_create(), the
 * \c inheritsched attribute determines how the other scheduling attributes of
 * the created thread shall be initialized.
 *
 * @param attr The thread attribute object where \c inheritsched should be stored in
 * @param inheritsched The inheritsched value to be stored in \c attr.
 *      Possible values are:
 *          - #PTHREAD_INHERIT_SCHED
 *          - #PTHREAD_EXPLICIT_SCHED
 *
 * @return 0 if the operation was successful, an errorcode otherwise.
 *          Errors:
 *              - #EINVAL The value of inheritsched is not valid
 */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
	// only PTHREAD_EXPLICIT_SCHED is used
	// test if the value of inheritsched is valid
	if ((inheritsched != PTHREAD_INHERIT_SCHED ) &&
	    (inheritsched != PTHREAD_EXPLICIT_SCHED)  )
	{
		printf("pthread_attr_setinheritsched(EINVAL)");
		return EINVAL;
	}

	attr->inheritsched = inheritsched;

	// everything is ok
	return 0;
}

/**
//  * Set the stacksize of the given thread attributes.
//  *
//  * @param attr The thread attributes object in which the stacksize should be stored
//  * @param stacksize The stacksize to be stored within the given attribute object
//  *
//  * @return 0 if the stacksize was successfully set, an errorcode otherwise
//  *             - #EINVAL if the value of \c stacksize is less than #EPS_SYS_THREAD_STACK_MIN
//  */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
	if (stacksize < PTHREAD_STACK_MIN)
	{
		return EINVAL;
	}

	// check alignment of stacksize
	// NOTE: Stack address is assumed to be aligned due to checks in ptherad_attr_setstack
	if ((stacksize & (EPS_SYS_PROC_BYTES -1)) != 0)
	{
		return EINVAL;
	}

	attr->stacksize = stacksize;

	return 0;
}

/**
 * Sets the given scheduling parameter \c param in the given thread attribute object.
 *
 * @param attr The thread attribute object in which the scheduling parameters should be set
 * @param param The scheduling parameter to be set in \c attr
 *
 * @return 0 if the scheduling parameter was successfully set, otherwise an error number.
 *          - #EINVAL if the value of param.sched_priority is not within 0 and #RT_PMAX
 */
int pthread_attr_setschedparam(pthread_attr_t * attr,
                               const struct sched_param * param)
{
	// test if the value of param.sched_priority is valid
    if (  ((param->sched_priority < 0) || (param->sched_priority > WIN_PMAX)) 
        && (param->sched_priority != WIN_PIR))
	{
		return ENOTSUP;
	}

	eps_memcpy((void*)&attr->schedparam, (void*)param, sizeof(struct sched_param));

	return 0;
}

/**
 * Sets the thread creation stack attributes \c stackaddr and \c stacksize.
 *
 * @param attr The thread attribute object to store the stack attributes in
 * @param stackaddr The address of the stack
 * @param stacksize The size of the stack
 *
 * @return 0 if successfully completed, an errorcode otherwise
 *              - #EINVAL if the stacksize is less than #ADN_SYS_THREAD_STACK_MIN or exceeds
 *                  an implementation-defined limit
 *              - #EINVAL if the stack begin of end has't got proper alignment
 */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                          size_t stacksize)
{
	volatile unsigned long *test_addr;
	// check the size to be at least PTHREAD_STACK_MIN
	if (stacksize < PTHREAD_STACK_MIN)
	{
		return EINVAL;
	}

	// check alignment of stack begin and end
	if (((uintptr_t)stackaddr & (EPS_SYS_PROC_BYTES - 1)) != 0 )
	{
		return EINVAL;
	}

	// check alignment of stacksize
	if ((stacksize & (EPS_SYS_PROC_BYTES -1)) != 0)
	{
		return EINVAL;
	}

	// test begin of stack
	test_addr=(unsigned long*)stackaddr;
	*test_addr=*test_addr;

	// test end of stack
	test_addr=(unsigned long*)((uintptr_t)stackaddr + (stacksize-EPS_SYS_PROC_BYTES));
	*test_addr=*test_addr;

	attr->stackaddr=stackaddr;
	attr->stacksize=stacksize;

	return 0;
}


/**
 * Sets the given scheduling policy within the given thread attribute object.
 *
 * @param attr The thread attributes object in which the scheduling policy should
 *      be set
 * @param policy The scheduling policy to be set within \c attr
 *
 * @return 0 if setting the policy was successful, an errorcode otherwise.
 *          Errors:
 *              - #EINVAL if the value of policy is not valid
 */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
	// Test whether the scheduling policy is valid
	if ((policy!=SCHED_FIFO ) &&
	    (policy!=SCHED_RR   ) &&
	    (policy!=SCHED_OTHER) )
	{
		printf("pthread_attr_setschedpolicy(EINVAL)");
		return EINVAL;
	}

	attr->schedpolicy = policy;

	// everything was ok
	return 0;
}

/**
 * Sets the thread's name.
 *
 * @param attr The thread attribute object to store the name in
 * @param name The name
 *
 * @return 0 if successfully completed, an errorcode otherwise
 *    - #EINVAL The name argument contains illegal characters.
 *    - #ENAMETOOLONG The length of the name argument is longer than {OS_THREAD_NAME_LEN}.
 */
int pthread_attr_setname(pthread_attr_t * attr, const char *name)
{
	// Check pointer
	if (name == NULL) 
	{
		// return error code
		return EINVAL;
	}

	// Check name length
	if (eps_lib_strlen(name) >= OS_THREAD_NAME_LEN)
	{
		// return error code
		return ENAMETOOLONG;
	}

	// Check character set
	if (0 != eps_lib_checkname(name))
	{
		// return error code
		return EINVAL;
	}

	// Set name
	eps_lib_strncpy(attr->name, name, OS_THREAD_NAME_LEN);

	return 0;
}

int pthread_attr_setcpuaffinity(pthread_attr_t *attr, const cpu_set_t * cpuset)
{

    LSA_UNUSED_ARG(attr);

    if(*cpuset != 1)  
    {
        EPS_FATAL("Only first Core supported!");
    }
    return 0;
}

/** Getting new Thread from Threadpool
 * 
 * If there are enough Threads left, the P2W_THREAD_PTR_TYPE is returned
 * 
 * @return P2W_THREAD_PTR_TYPE reference to free thread
 * @return LSA_NULL if no thread free and EPS_FATAL
 */
static P2W_THREAD_PTR_TYPE p2w_get_new_thread(LSA_VOID)
{
	LSA_UINT32 i;

	EnterCriticalSection(g_P2wThreads.hCritical);

	for(i=0; i<P2W_CFG_MAX_THREADS; i++)
	{
		if(g_P2wThreads.Thread[i].bUsed == LSA_FALSE)
		{
			g_P2wThreads.Thread[i].bUsed = LSA_TRUE;

			LeaveCriticalSection(g_P2wThreads.hCritical);

			return &g_P2wThreads.Thread[i];
		}
	}

	LeaveCriticalSection(g_P2wThreads.hCritical);

	EPS_FATAL("No More Thread Resources");

	return LSA_NULL;
}

static LSA_VOID p2w_thread_free(P2W_THREAD_PTR_TYPE pThread)
{
	EPS_ASSERT(pThread != LSA_NULL);

	EnterCriticalSection(g_P2wThreads.hCritical);

	pThread->bUsed = LSA_FALSE;

	LeaveCriticalSection(g_P2wThreads.hCritical);
}

static unsigned __stdcall eps_win_thread_proc(void * arg) 
{
	P2W_THREAD_PTR_TYPE pArg = (P2W_THREAD_PTR_TYPE)arg;

	pArg->pStartRoutineCbf(pArg->pThreadArgs);

	SetEvent( pArg->hThreadStoppedEvent );

	// threads ends
	_endthreadex(0);
	return ( 0 );
}

void pthread_init(void)
{
	if(g_P2wThreads.bInitialised == LSA_FALSE)
	{
		g_P2wThreads.hCritical = &g_P2wThreads.hLocCritical;
		InitializeCriticalSection(g_P2wThreads.hCritical);

		g_P2wThreads.bInitialised = LSA_TRUE;
	}
}

/** Creates and starts a new thread in Windows
 * 
 * Fetches a new thread out of pool and sets the specified parameters.
 * Thread gets started after that.
 * 
 * @param thread pointer to created thread
 * @param attr thread_paramteres (e.g. priority)
 * @param start_routine function pointer to start_routine of thread
 * @param arg arguments given to the start_routine
 * @return 0
 */
int pthread_create(pthread_t * thread,
                   const pthread_attr_t * attr,
                   void *(*start_routine)(void*), void * arg)
{
	int ret_val = 0, prio=THREAD_PRIORITY_NORMAL;
	BOOL  set_state;
	HANDLE thread_handle;
	P2W_THREAD_PTR_TYPE pThread;

	pthread_init();

	pThread = p2w_get_new_thread();
	EPS_ASSERT(pThread != LSA_NULL);

	switch (attr->schedparam.sched_priority)
	{
    case WIN_P1: 
			prio = THREAD_PRIORITY_LOWEST;
			break;
    case WIN_P15: 
			prio = THREAD_PRIORITY_NORMAL;
			break;
    case WIN_P27:
    case WIN_P28:
			prio = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
    case WIN_P30: 
			prio = THREAD_PRIORITY_HIGHEST;
			break;
    case WIN_P31: 
			prio = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		default:
			EPS_FATAL("Unsupported schedule priority.");
			break;
	}
	
    pThread->hThreadStoppedEvent = CreateEvent( NULL, LSA_FALSE, LSA_FALSE, NULL );
	pThread->pStartRoutineCbf = start_routine;
	pThread->pThreadArgs = arg;
	
	thread_handle = (HANDLE)_beginthreadex( // starting the thread
			NULL,
			attr->stacksize, 
			(eps_win_thread_proc), 
			pThread,
			0, 
      &pThread->hOsThread );


	EPS_ASSERT ( thread_handle != 0 );
	
	// set configured priority
	set_state = SetThreadPriority( thread_handle, prio );
	EPS_ASSERT( set_state );

	*thread = (pthread_t)pThread;

	// return
	return ret_val;
}

int pthread_join(pthread_t thread, void** th_ret)
{
	P2W_THREAD_PTR_TYPE pThread = (P2W_THREAD_PTR_TYPE)thread;

	EPS_ASSERT(pThread != LSA_NULL);

	LSA_UNUSED_ARG(th_ret); 	//(LaM)ToDo

	// Wait until target thread terminates. 
	WaitForSingleObject( pThread->hThreadStoppedEvent, INFINITE );
	CloseHandle( pThread->hThreadStoppedEvent );

	p2w_thread_free(pThread);

	return 0;
}

pthread_t pthread_self(void)
{
	LSA_UINT32 curId  = GetCurrentThreadId(); 
	LSA_UINT32 i;

	EnterCriticalSection(g_P2wThreads.hCritical);

	for(i=0; i<P2W_CFG_MAX_THREADS; i++)
	{
		if(g_P2wThreads.Thread[i].bUsed == LSA_TRUE)
		{
			if(g_P2wThreads.Thread[i].hOsThread == curId)
			{
				LeaveCriticalSection(g_P2wThreads.hCritical);

				return (LSA_UINT32)(&g_P2wThreads.Thread[i]);
			}
		}
	}

	LeaveCriticalSection(g_P2wThreads.hCritical);

	return 0xffff;
}

/**
 * The pthread_kill() function shall request that a signal be delivered to
 * the specified thread.
 * As in kill(), if sig is zero, error checking shall be performed but no
 * signal shall actually be sent.
 *
 * @see #eps_sysc_pthread_kill
 *
 * @param thread Thread the signal should be delivered to
 * @param sig The signal that should be delivered
 *
 * @return 0 upon successful completion, otherwise
 *    - #ESRCH No thread could be found corresponding to that specified
 *              by the given thread ID.
 *    - #EINVAL The value of the sig argument is an invalid or
 *              unsupported signal number.
 *
 */
int pthread_kill(pthread_t thread, int sig)
{
	//not used yet
	LSA_UNUSED_ARG(thread);
	LSA_UNUSED_ARG(sig);

	EPS_FATAL("Function not implemented yet.");
	return -1;
}

static void CALLBACK eps_mmtimer_callback( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	struct sigevent * evp = (struct sigevent *)dwUser;

	LSA_UNUSED_ARG(uID);
	LSA_UNUSED_ARG(uMsg);
	LSA_UNUSED_ARG(dw1);
	LSA_UNUSED_ARG(dw2);

	evp->sigev_notify_function(evp->sigev_value);
}

/**
 * Description.
 *
 * @param clockid ID of clock
 * @param evp     Event pointer
 * @param timerid ID of timer
 *
 * @return
 */
int timer_create(clockid_t clockid, struct sigevent * evp,
                 timer_t * timerid)
{
	TIMECAPS tc;
	MMRESULT rc;
		
    //LSA_UNUSED_ARG(timerid);
	LSA_UNUSED_ARG(clockid);

    EPS_ASSERT (g_P2wTimer.timer_create_count < P2W_TIMER_COUNT_MAX);	

    g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].pEvp = (struct sigevent *)malloc(sizeof(struct sigevent));
    EPS_ASSERT(g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].pEvp != LSA_NULL);
    *g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].pEvp = *evp;

    if ( !QueryPerformanceFrequency (& g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].freq )) // get frequency
	{
        g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].freq.QuadPart = 0;
	}

    EPS_ASSERT( g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].freq.QuadPart != 0 );	

	// set granularity to 1msec
	rc = timeGetDevCaps(&tc, sizeof(TIMECAPS));
	EPS_ASSERT (rc == TIMERR_NOERROR);

	EPS_ASSERT( tc.wPeriodMin == 1); /* must: 1 msec ! */

    g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].period = tc.wPeriodMin;
    rc = timeBeginPeriod(g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].period);
	EPS_ASSERT (rc == TIMERR_NOERROR);		
	
	// start the MultiMediaTimer
    g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].timer_ID = timeSetEvent (1, 1, eps_mmtimer_callback, (DWORD_PTR)g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].pEvp, TIME_PERIODIC);
    EPS_ASSERT( g_P2wTimer.posix2win_timer[g_P2wTimer.timer_create_count].timer_ID != 0 );

    *timerid=(timer_t)g_P2wTimer.timer_create_count;

    g_P2wTimer.timer_create_count++;

	return 0;
}

/**
 * Description.
 *
 * @param timerid ID of timer
 * @param flags   Flags, TIMER_ABSTIME or 0
 * @param value   structure with new value
 * @param ovalue  structure for old value
 *
 * @return
 */
int timer_settime(timer_t timerid, int flags,
                  const struct itimerspec * value,
                  struct itimerspec * ovalue)
{
  int ret_val = 0;

  LSA_UNUSED_ARG(flags);
  LSA_UNUSED_ARG(ovalue);
  LSA_UNUSED_ARG(value);
  LSA_UNUSED_ARG(timerid);

  return ret_val;
}

/**
 * Description.
 *
 * @param timerid ID of timer to delete
 *
 * @return
 */
int timer_delete(timer_t timerid)
{	
	// stop the MultiMediaTimer
    MMRESULT rc = timeKillEvent (g_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].timer_ID);
	EPS_ASSERT( rc == TIMERR_NOERROR );

    g_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].timer_ID = 0; // see MSDN and eps_mmtimer_callback()

	// reset the granularity
    timeEndPeriod( g_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].period );

    free(g_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].pEvp);
    g_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].pEvp = LSA_NULL;

    g_P2wTimer.timer_create_count--;

	return 0;
}

/**
 * Initializes a semaphore.
 *
 * @param sem Semaphore to be initialized
 * @param pshared If non-zero semaphore can be used by more than one process
 * @param value Start value of semaphore
 *
 * @return 0 upon successful completion, -1 otherwise. Additionally in case of an
 *    error, \c errno is set to 
 *      - #EINVAL The value argument exceeds #SEM_VALUE_MAX
 *      - #ENOSPC A resource required to initialize the semaphore has been exhausted
 */
int sem_init(sem_t *sem, int pshared, unsigned value)
{
    LSA_UNUSED_ARG(value);
	LSA_UNUSED_ARG(pshared);

    *sem = CreateEvent( NULL, LSA_FALSE/*auto*/, LSA_FALSE, NULL );
	EPS_ASSERT( *sem != LSA_NULL );

	return 0;
}


/**
 * Marks a semaphore as destroyed
 *
 * @param sem Semaphore to be destroyed
 *
 * @return 0 or error code, if semaphore still in use
 */
int sem_destroy(sem_t *sem)
{	
	BOOL rc = CloseHandle( *sem );
	EPS_ASSERT( rc );

	return 0;
}

/**
 * Lock a semaphore and wait forever, if it is not available.
 *
 * @param sem Semaphore to be locked
 *
 * @return The and sem_wait() returns zero if the calling process successfully performed 
 *    the semaphore lock operation on the semaphore designated by sem. If the call was 
 *    unsuccessful, the state of the semaphore is unchanged, and the function returns 
 *    a value of -1 and set #errno to indicate the error:
 *      - #EINVAL: The \c sem argument does not refer to a valid semaphore.
 */
int sem_wait(sem_t *sem)
{
	DWORD result=0;
	int rc=0;
		
	result = WaitForSingleObject( *sem, INFINITE );
	if (result != WAIT_OBJECT_0)
	{
        rc = -1;
		EPS_ASSERT( result == WAIT_OBJECT_0 );
	}
	result = ResetEvent( *sem );
	EPS_ASSERT( result );

	return rc;
}

/**
 * Lock a semaphore and wait until a definite absolute time, if it is not available.
 *
 * @param sem Semaphore to wait for until a certain absolute time
 * @param abs_timeout Absolute timeout
 *
 * @return The sem_timedwait() function returns zero if the calling process successfully performed the semaphore lock operation 
 *    on the semaphore designated by \csem. 
 *    If the call was unsuccessful, the state of the semaphore is unchanged, and the function shall return a value of -1 
 *    and set #errno to indicate the error:
 *      - #EINVAL: The \c sem argument does not refer to a valid semaphore.
 *      - #EINVAL: The thread would have blocked, and the \b abs_timeout parameter specified a nanoseconds field value less than 
 *          zero or greater than or equal to 1000 million.
 *      - #ETIMEDOUT: The semaphore could not be locked before the specified timeout expired. 
 */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    DWORD dwMilliSeconds = 0;
	DWORD result=0;
	int rc=0;

    dwMilliSeconds = abs_timeout->tv_sec * 1000;
		
	result = WaitForSingleObject( *sem, dwMilliSeconds );
	if (result == WAIT_OBJECT_0)
        // wait ok
	{
        rc = ResetEvent( *sem );
	    EPS_ASSERT( rc );
	}
    else if(result == WAIT_TIMEOUT)
        // timeout occured
    {
        rc = -1;
    }
    else
        // other errors
    {
        rc = -1;
        EPS_ASSERT( result == WAIT_OBJECT_0 );
    }

	return rc;
}

/**
 * Release a semaphore.
 *
 * @param sem Semaphore to be released
 *
 * @return If successful, the sem_post() function returns zero; otherwise, 
 *    the function returns -1 and sets #errno to indicate the error:
 *    - #EINVAL: The sem argument does not refer to a valid semaphore.    
 */
int sem_post(sem_t *sem)
{
	BOOL rc=0;

	rc = SetEvent( *sem );
	EPS_ASSERT( rc );

	return 0;
}

/**
 * Returns the current value \c tp for the specified clock, \c clock_id.
 *
 * @param clock_id ID of the clock which time should be returned
 * @param tp #timespec structure where the current time is stored into
 *
 * @return 0 upon successful completion, -1 otherwise. Additionally in case of an error
 *    #errno is set to
 *      - #EINVAL if the \c clock_id argument does not specify a known clock
 */
int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
  	static/*!*/ LARGE_INTEGER freq;
	static/*!*/ LARGE_INTEGER base;
	LARGE_INTEGER now;

	LSA_UNUSED_ARG(clock_id);

	if( freq.QuadPart == 0 ) 
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&base);
	}

	QueryPerformanceCounter(&now);

    now.QuadPart= (now.QuadPart-base.QuadPart) * (1000000000 / freq.QuadPart);  //return in nsec

    tp->tv_sec  = (LONG)(now.QuadPart / 1000000000); 
    tp->tv_nsec = (LONG)(now.QuadPart % 1000000000);

	return 0;
}

/**
 * Obtains the current time, expressed as seconds and microseconds since the 
 * Epoch, and store it in the timeval structure pointed to by \c tp. 
 * 
 * If tzp is not a null pointer, the behavior is unspecified. 
 * 
 * @param tp Object where the time should be stored 
 * @param tzp Not used. Should always be \c NULL
 *
 * @return Always 0
 */
int gettimeofday(struct timeval * tp, void * tzp)
{
	static/*!*/ LARGE_INTEGER freq;
	static/*!*/ LARGE_INTEGER base;
	LARGE_INTEGER now;

	LSA_UNUSED_ARG(tzp);

	if( freq.QuadPart == 0 ) 
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&base);
	}

	QueryPerformanceCounter(&now);

	now.QuadPart= (now.QuadPart-base.QuadPart) * 1000000 / freq.QuadPart;  //return in usec

	tp->tv_sec  = (LONG)(now.QuadPart / 1000000); 
	tp->tv_usec = (LONG)(now.QuadPart - (tp->tv_sec * 1000000)) ;

	return 0;
}
