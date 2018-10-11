/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: eps_timer.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: LSA_PNIO_Pxx.xx.xx.xx_xx.xx.xx.xx         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: xxxx-xx-xx                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Timer Interface                                                      */
/*                                                                           */
/*****************************************************************************/

#include <string.h>
#include <errno.h>

#include <psi_inc.h>  
#include <psi_cfg.h>  

#include <eps_rtos.h>
#include <eps_sys.h>
#include <eps_locks.h>

#include <eps_timer.h>
#include <eps_tasks.h>
#include <eps_trc.h>

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#define LTRC_ACT_MODUL_ID 20038

//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------
typedef enum TimerStatus
{
    TS_FREE = 0,
    TS_RUNNING,
    TS_STOPPED
}TimerStatus;

typedef struct TimerRecord
{
    struct TimerRecord *  next;       // MUST BE AT OFFSET 0
    struct TimerRecord *  prior;
    TimerStatus           status;
    LSA_UINT16            timer_id;
    LSA_UINT16            timer_type;
    LSA_UINT16            time_base;
    EPS_TIMER_CBF         callback;
    LSA_USER_ID_TYPE      user_id;
    LSA_UINT32            target;     // value of 1ms timer tick at next timeout
    LSA_UINT32            preset;     // period - number of 1 ms timer ticks
}TimerRecord;

typedef enum {
    eThInit        = 0,
    eThRun         = 1,
    eThReqStop     = 2,
    eThStopped     = 3
}EPS_TIMER_STATE_TYPE;

typedef struct TimerState
{
    volatile EPS_TIMER_STATE_TYPE eState;
    LSA_UINT16            lock_id;
    LSA_UINT32            last_tick;
    LSA_UINT32            next_10ms_tick;
    LSA_UINT32            next_100ms_tick;
    LSA_UINT32            next_1s_tick;
    LSA_UINT32            next_10s_tick;
    LSA_UINT32            next_100s_tick;
    struct TimerRecord *  first1msTimer;
    struct TimerRecord *  first10msTimer;
    struct TimerRecord *  first100msTimer;
    struct TimerRecord *  first1sTimer;
    struct TimerRecord *  first10sTimer;
    struct TimerRecord *  first100sTimer;
    struct TimerRecord    timers[200];
    
}TimerState;

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------
static TimerState   state0 ; // ATTR_LOCATE_IN_DTCM
static TimerState   state1;  // ATTR_LOCATE_IN_DTCM
static timer_t      tid0;
static timer_t      tid1;

//------------------------------------------------------------------------------
// Implementation - LSA TIMER FOR THREAD GROUP 0
//------------------------------------------------------------------------------

//******************************************************************************
//  Function: internal_timer_stop_tgroup0
//
//  Description:
//
//      Stop a timer and remove it from its timer list.
//
//  Parameters:
//
//      timer       -- in:  timer to be stopped and removed from the list
//
//  Return:
//
//      None
//******************************************************************************
static void internal_timer_stop_tgroup0(
    TimerRecord *   timer )
{
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE, "eps_timer internal_stop0 %d", timer->timer_id);

    if (NULL == timer->prior)
    {
        if (LSA_TIME_BASE_1MS == timer->time_base)
        {
            state0.first1msTimer = timer->next;
        }
        else if (LSA_TIME_BASE_10MS == timer->time_base)
        {
            state0.first10msTimer = timer->next;
        }
        else if (LSA_TIME_BASE_100MS == timer->time_base)
        {
            state0.first100msTimer = timer->next;
        }
        else if (LSA_TIME_BASE_1S == timer->time_base)
        {
            state0.first1sTimer = timer->next;
        }
        else if (LSA_TIME_BASE_10S == timer->time_base)
        {
            state0.first10sTimer = timer->next;
        }
        else /* LSA_TIME_BASE_100S */
        {
            state0.first100sTimer = timer->next;
        }
    }
    else
    {
        (timer->prior)->next = timer->next;
    }
    if (NULL != timer->next)
    {
        (timer->next)->prior = timer->prior;
    }
    timer->status = TS_STOPPED;
}

//******************************************************************************
//  Function: internal_timer_timeout_tgroup0
//
//  Description:
//
//      Implement the time-keeping algorithm. The implementation assumes a small
//      number of timers. It runs in IRQ state0 in 1 ms tick.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
//#pragma ghs nothumb
static void internal_timer_timeout_tgroup0( union sigval sig )
{
    register TimerRecord * timer;
    register uint32_t      cur_tick;
    struct itimerspec its;
    
    LSA_UNUSED_ARG(sig);
    
    // if Shutdown is started, ignore this function
    if (state0.eState == eThReqStop)
    {
    	memset( &its, 0, sizeof(its) );

    	EPS_POSIX_TIMER_SETTIME((timer_t)tid0, 0, &its, NULL); // the timer disarm itself

        // Other thread is cleaning up this thread
        // the cleanup thread waits until this thread is stopped
        // and cleans this thread afterwards.
        state0.eState = eThStopped;
        return;
    }

    if ((state0.eState == eThStopped) || (state0.eState == eThInit))
    {
    	return; // timer can be triggered one time again after delete(eThStopped) and while init(eThInit)
    }

    eps_enter_critical_section(state0.lock_id);

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer internal_timer_timeout_tgroup0");
    cur_tick = state0.last_tick + 1;
    state0.last_tick = cur_tick;
    timer = (TimerRecord *)&state0.first1msTimer;

    if ( !PSI_FATAL_ERROR_OCCURED() )
    {
        while (NULL != (timer = timer->next) )
        {
            if (cur_tick == timer->target)
            {
      			EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                timer->target = cur_tick + timer->preset;
                if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                {
                    (*timer->callback) (timer->timer_id, timer->user_id);
                    continue;
                }
                internal_timer_stop_tgroup0(timer);
                (*timer->callback) (timer->timer_id, timer->user_id);
            }
        }

        if (cur_tick == state0.next_10ms_tick)
        {
            state0.next_10ms_tick = cur_tick + 10;
            timer = (TimerRecord *)&state0.first10msTimer;
            while (NULL != (timer = timer->next) )
            {
                if (cur_tick == timer->target)
                {
        			EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                    timer->target = cur_tick + timer->preset;
                    if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                    {
                        (*timer->callback) (timer->timer_id, timer->user_id);
                        continue;
                    }
                    internal_timer_stop_tgroup0(timer);
                    (*timer->callback) (timer->timer_id, timer->user_id);
                }
            }

            if (cur_tick == state0.next_100ms_tick)
            {
                state0.next_100ms_tick = cur_tick + 100;
                timer = (TimerRecord *)&state0.first100msTimer;
                while (NULL != (timer = timer->next) )
                {
                    if (cur_tick == timer->target)
                    {
                        EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                        timer->target = cur_tick + timer->preset;
                        if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                        {
                            (*timer->callback) (timer->timer_id, timer->user_id);
                            continue;
                        }
                        internal_timer_stop_tgroup0(timer);
                        (*timer->callback) (timer->timer_id, timer->user_id);
                    }
                }

                if (cur_tick == state0.next_1s_tick)
                {
                    state0.next_1s_tick = cur_tick + 1000;
                    timer = (TimerRecord *)&state0.first1sTimer;
                    while (NULL != (timer = timer->next) )
                    {
                        if (cur_tick == timer->target)
                        {
                            EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                            timer->target = cur_tick + timer->preset;
                            if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                            {
                                (*timer->callback) (timer->timer_id, timer->user_id);
                                continue;
                            }
                            internal_timer_stop_tgroup0(timer);
                            (*timer->callback) (timer->timer_id, timer->user_id);
                        }
                    }
                }

                if (cur_tick == state0.next_10s_tick)
                {
                    state0.next_10s_tick = cur_tick + 10000;
                    timer = (TimerRecord *)&state0.first10sTimer;
                    while (NULL != (timer = timer->next) )
                    {
                        if (cur_tick == timer->target)
                        {
                            EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                            timer->target = cur_tick + timer->preset;
                            if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                            {
                                (*timer->callback) (timer->timer_id, timer->user_id);
                                continue;
                            }
                            internal_timer_stop_tgroup0(timer);
                            (*timer->callback) (timer->timer_id, timer->user_id);
                        }
                    }
                }

                if (cur_tick == state0.next_100s_tick)
                {
                    state0.next_100s_tick = cur_tick + 100000;
                    timer = (TimerRecord *)&state0.first100sTimer;
                    while (NULL != (timer = timer->next) )
                    {
                        if (cur_tick == timer->target)
                        {
                            EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                            timer->target = cur_tick + timer->preset;
                            if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                            {
                                (*timer->callback) (timer->timer_id, timer->user_id);
                                continue;
                            }
                            internal_timer_stop_tgroup0(timer);
                            (*timer->callback) (timer->timer_id, timer->user_id);
                        }
                    }
                }
            }
        }
    }
    else
    {
        LSA_UINT32 i;
        
        EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer internal_timer_timeout_tgroup0 - else case");
      
        for (i = 0; i < countof(state0.timers); i++)
	    {
            if(TS_FREE != state0.timers[i].status)
            {
                if (TS_RUNNING == state0.timers[i].status)
                {
                    internal_timer_stop_tgroup0(&state0.timers[i]);
                }
                eps_timer_free_tgroup0(state0.timers[i].timer_id);
            }
	    }

        eps_timer_undo_init_tgroup0();
    }
    
    eps_exit_critical_section(state0.lock_id);
}
//#pragma ghs thumb

//******************************************************************************
//  Function: eps_timer_init_tgroup0
//
//  Description:
//
//      This function will initialize rhe IRQ-based timer subsystem.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      LSA_RET_OK            The timer subsystem was initialized successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of resources
//******************************************************************************
LSA_UINT16 eps_timer_init_tgroup0( LSA_VOID )
{
    pthread_attr_t     thread_attr;
    struct sched_param sched_param;
    struct sigevent    event;
    struct itimerspec  timeout;
    int                ret;
    LSA_UINT16         i;

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_init_tgroup0 - begin");

    // initialize the state0 record
    memset( &state0, 0, sizeof(state0) );

    ret = eps_alloc_critical_section( &state0.lock_id, LSA_FALSE );
    if (ret != LSA_RET_OK)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    
    state0.next_10ms_tick   = 10;
    state0.next_100ms_tick  = 100;
    state0.next_1s_tick     = 1000;
    state0.next_10s_tick    = 10000;
    state0.next_100s_tick   = 100000;
    for (i = 0; i < countof(state0.timers); i++)
    {
        state0.timers[i].timer_id = i;
    }

    // initialize the "thread" attributes (we don't use a thread)
    memset( &thread_attr, 0, sizeof( thread_attr ) );
    ret = EPS_POSIX_PTHREAD_ATTR_INIT( &thread_attr );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if (EPS_PLF == EPS_PLF_LINUX_X86)
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY( &thread_attr, /*SCHED_OTHER atw12r13*/ SCHED_FIFO );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif

    // initialize scheduling parameters
    memset( &sched_param, 0, sizeof( sched_param ) );
    sched_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH;
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM( &thread_attr, &sched_param );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if (EPS_PLF == EPS_PLF_LINUX_X86)
    sched_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH+1;
    ret = sched_setscheduler( 0, SCHED_FIFO, &sched_param );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif

    // set schedule_parameters as explicit, otherwise they are taken from this thread
   	ret = EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED( &thread_attr, PTHREAD_EXPLICIT_SCHED );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if (EPS_PLF != EPS_PLF_LINUX_X86)
    //set thread name
    ret = pthread_attr_setname( &thread_attr, "TIMER_TGROUP0" );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif

    // TODO need Threadname for Linux
  
    // fill event data and create the timer
    memset( &event, 0, sizeof( event ) );
    event.sigev_notify = SIGEV_THREAD;

    #if (EPS_PLF == EPS_PLF_LINUX_X86) // TODO -> move SIGNO to Posix-IF
    event.sigev_signo = SIGRTMIN;
    #else
    event.sigev_signo = SIGRT15;
    #endif

    event.sigev_notify_function   = &internal_timer_timeout_tgroup0;
    event.sigev_notify_attributes = &thread_attr;
    event.sigev_value.sival_ptr   = NULL;
    ret = EPS_POSIX_TIMER_CREATE( CLOCK_MONOTONIC, &event, (timer_t*)&tid0 );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // start the timer - 1 ms cyclic timeout
    timeout.it_value.tv_sec     = 0;
    timeout.it_value.tv_nsec    = 1000000;
    timeout.it_interval.tv_sec  = 0;
    timeout.it_interval.tv_nsec = 1000000;
    ret = EPS_POSIX_TIMER_SETTIME( (timer_t)tid0, 0, &timeout, NULL );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_init_tgroup0 - done");

    return LSA_RET_OK;
}

LSA_VOID eps_timer_undo_init_tgroup0( LSA_VOID )
{
	LSA_UINT32 i;

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_undo_init_tgroup0 - begin, Timer-State=%u", state0.eState);

    // Tell the timeout function to stop it's work.
    state0.eState = eThReqStop;

    // Synchronize with other thread/timer wait until it is stopped
    while(state0.eState != eThStopped)
    {
        eps_tasks_sleep(1);
    }
    
    eps_enter_critical_section( state0.lock_id );
	for (i = 0; i < countof(state0.timers); i++)
	{
		EPS_ASSERT(state0.timers[i].status == TS_FREE);
	}

	EPS_POSIX_TIMER_DELETE( (timer_t)tid0 );
    eps_exit_critical_section( state0.lock_id );
    // do the actual clean up after timer is stopped!
	eps_free_critical_section( state0.lock_id );
    state0.lock_id = 0;
	
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_undo_init_tgroup0 - done, Timer-State=%u", state0.eState);
}

//******************************************************************************
//  Function: eps_timer_allocate_tgroup0
//
//  Description:
//
//      This function will allocate an IRQ-based timer.
//
//  Parameters:
//
//      timer_id_ptr -- out:  points to returned value of timer's identifier
//      timer_type   -- in:   Type of timer -
//
//                                  LSA_TIMER_TYPE_ONE_SHOT or
//                                  LSA_TIMER_TYPE_CYCLIC
//
//      time_base    -- in:   Units of preset at start --
//
//                                  LSA_TIME_BASE_1MS
//                                  LSA_TIME_BASE_10MS,
//                                  LSA_TIME_BASE_100MS or
//                                  LSA_TIME_BASE_1S
//
//      callback     -- in:   pointer to function called on timeout.
//
//                              NOTE: Callback function is called in IRQ state.
//
//  Return:
//
//      LSA_RET_OK            The timer was allocated successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of timer resources
//      LSA_RET_ERR_PARAM     The function failed due to a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_allocate_tgroup0(
  LSA_TIMER_ID_TYPE * timer_id_ptr,
  LSA_UINT16          timer_type,
  LSA_UINT16          time_base,
  EPS_TIMER_CBF       callback )
{
  LSA_UINT16 i;
  
  eps_enter_critical_section(state0.lock_id);

  EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_allocate0 (0x%X, %d, %d, 0x%X)",timer_id_ptr,timer_type,time_base,callback);

  if ( ( (timer_type == LSA_TIMER_TYPE_ONE_SHOT) ||
         (timer_type == LSA_TIMER_TYPE_CYCLIC) ) &&
       ( (time_base == LSA_TIME_BASE_1MS) ||
         (time_base == LSA_TIME_BASE_10MS) ||
         (time_base == LSA_TIME_BASE_100MS) ||
         (time_base == LSA_TIME_BASE_1S) ||
         (time_base == LSA_TIME_BASE_10S) ||
         (time_base == LSA_TIME_BASE_100S) ) &&
         (NULL != callback) )
  {
    for (i = 0; i < countof(state0.timers); i++)
    {
      if (TS_FREE == state0.timers[i].status)
      {
        register TimerRecord * timer = &(state0.timers[i]);

        timer->prior = NULL;
        timer->next = NULL;
        timer->status = TS_STOPPED;
        timer->timer_type = timer_type;
        timer->time_base = time_base;
        timer->callback = callback;
        memset(&(timer->user_id), 0, sizeof(timer->user_id));
        timer->target = 0;
        timer->preset = 0;
        *timer_id_ptr = timer->timer_id;
        EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_allocate0 -- success, ID = %d",timer->timer_id);
		        
		eps_exit_critical_section(state0.lock_id);
		
		EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_allocate_tgroup0 - timer_id = %d", timer->timer_id);
		
        return LSA_RET_OK;
      }
    }
    EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,"eps_timer_allocate0 (0x%X, %d, %d, 0x%X) -- failed, no timers available",timer_id_ptr,timer_type,time_base,callback);    
	eps_exit_critical_section(state0.lock_id);
    return LSA_RET_ERR_NO_TIMER;
  }
  EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_allocate0 (0x%X, %d, %d, 0x%X) -- failed, parameter error",timer_id_ptr,timer_type,time_base,callback);
  eps_exit_critical_section(state0.lock_id);
  return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_start_tgroup0
//
//  Description:
//
//      This function will start timing on a timer allocated using
//      eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to start
//      user_id      -- in:   Caller's identifier for the timeout instance
//      time         -- in:   timeout time in units of time_base at
//                            eps_timer_allocate
//
//  Return:
//
//      LSA_RET_OK                  The timer was stopped and has been started.
//      LSA_RET_OK_TIMER_RESTARTED  The timer was running and has been restarted
//      LSA_RET_ERR_PARAM           The function failed due to a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_start_tgroup0(
    LSA_TIMER_ID_TYPE   timer_id,
    LSA_USER_ID_TYPE    user_id,
    LSA_UINT16          time )
{
  eps_enter_critical_section(state0.lock_id);  
    
  EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_start0 (%d, 0x%X, %d)",timer_id,user_id.void_ptr,time);
  
  if ( (timer_id < countof(state0.timers)) &&
       (0 < time) )
  {
    register TimerRecord * timer = &(state0.timers[timer_id]);

    if ( (TS_STOPPED == timer->status) ||
         (TS_RUNNING == timer->status) )
    {
      TimerRecord ** anchor = NULL;

      timer->user_id = user_id;
      if (LSA_TIME_BASE_1MS == timer->time_base)
      {
        timer->preset = time;
        timer->target = state0.last_tick + timer->preset + 1;
        anchor = (TimerRecord **)&state0.first1msTimer;
      }
      else if (LSA_TIME_BASE_10MS == timer->time_base)
      {
        timer->preset = time * 10;
        timer->target = state0.next_10ms_tick + timer->preset;
        anchor = (TimerRecord **)&state0.first10msTimer;
      }
      else if (LSA_TIME_BASE_100MS == timer->time_base)
      {
        timer->preset = time * 100;
        timer->target = state0.next_100ms_tick + timer->preset;
        anchor = (TimerRecord **)&state0.first100msTimer;
      }
      else if (LSA_TIME_BASE_1S == timer->time_base)
      {
        timer->preset = time * 1000;
        timer->target = state0.next_1s_tick + timer->preset;
        anchor = (TimerRecord **)&state0.first1sTimer;
      }
      else if (LSA_TIME_BASE_10S == timer->time_base)
      {
        timer->preset = time * 10000;
        timer->target = state0.next_10s_tick + timer->preset;
        anchor = (TimerRecord **)&state0.first10sTimer;
      }
      else /* LSA_TIME_BASE 100S */
      {
        timer->preset = time * 100000;
        timer->target = state0.next_100s_tick + timer->preset;
        anchor = (TimerRecord **)&state0.first100sTimer;
      }
      if (TS_STOPPED == timer->status)
      {
        timer->prior = NULL;
        timer->next = *anchor;
        if (NULL != *anchor)
        {
          (*anchor)->prior = timer;
        }
        *anchor = timer;
        timer->status = TS_RUNNING;
        state0.eState = eThRun;
        EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start0 -- succeeded, timer started");        
		eps_exit_critical_section(state0.lock_id);
        return LSA_RET_OK;
      }
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start0 -- succeeded, timer restarted");
	  eps_exit_critical_section(state0.lock_id);
      //return LSA_RET_OK_TIMER_RESTARTED;
      return LSA_RET_OK; //@@@EDDI_StartTimer()?!
    }
  }

  if(state0.timers == LSA_NULL)
  {
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_FATAL,">< eps_timer_start0 - TIMER INVALID");
      EPS_FATAL("eps_timer_start0 - TIMER INVALID");
      return LSA_RET_ERR_RESOURCE;
  }
  else
  {
      register TimerRecord * timer = &(state0.timers[timer_id]);
      
      EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_ERROR,">< eps_timer_start0 (timer_id = %d, user_id.void_ptr = 0x%X, time = %d, Status = %d) -- failed, parameter error)",timer_id, user_id.void_ptr, time, timer->status);
      eps_exit_critical_section(state0.lock_id);
      return LSA_RET_ERR_PARAM;
  }

}

//******************************************************************************
//  Function: eps_timer_stop_tgroup0
//
//  Description:
//
//      This function will stop timing on a timer started using eps_timer_start.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to stop
//
//  Return:
//
//      LSA_RET_OK                    The timer was stopped.
//      LSA_RET_OK_TIMER_NOT_RUNNING  The timer was not running
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_stop_tgroup0(
    LSA_TIMER_ID_TYPE   timer_id )
{
  eps_enter_critical_section(state0.lock_id);

  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_stop0 (%d)",timer_id);
  if (timer_id < countof(state0.timers))
  {
    register TimerRecord * timer = &(state0.timers[timer_id]);    

    if (TS_STOPPED == timer->status)
    {
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop0 -- succeeded, timer was not running");
	  eps_exit_critical_section(state0.lock_id);
      return LSA_RET_OK_TIMER_NOT_RUNNING;
    }
    else if (TS_RUNNING == timer->status)
    {
      internal_timer_stop_tgroup0(timer);
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop0 -- succeeded");
	  eps_exit_critical_section(state0.lock_id);
      return LSA_RET_OK;
    }
  }
  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_stop0 (%d) -- failed, parameter error",timer_id);
  eps_exit_critical_section(state0.lock_id);
  return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_free_tgroup0
//
//  Description:
//
//      This function will release a timer allocated using eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to release
//
//  Return:
//
//      LSA_RET_OK                    The timer was released.
//      LSA_RET_ERR_TIMER_IS_RUNNING  The timer is running and was not released
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_free_tgroup0(
    LSA_TIMER_ID_TYPE   timer_id )
{    
  eps_enter_critical_section(state0.lock_id);

  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_free0 (%d)",timer_id);
  if (timer_id < countof(state0.timers))
  {
    register TimerRecord * timer = &(state0.timers[timer_id]);

    if (TS_STOPPED == timer->status)
    {
      timer->status = TS_FREE;
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_free0 -- succeeded");      
	  eps_exit_critical_section(state0.lock_id);	  
	  
	  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_free_tgroup0 - timer_id = %d", timer_id);  
	  
      return LSA_RET_OK;
    }
    else if (TS_RUNNING == timer->status)
    {
      EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free0 (%d) -- failed, timer is running",timer_id);      
	  eps_exit_critical_section(state0.lock_id);
      return LSA_RET_ERR_TIMER_IS_RUNNING;
    }
  }
  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free0 (%d) -- failed, parameter error",timer_id);
  eps_exit_critical_section(state0.lock_id);
  return LSA_RET_ERR_PARAM;
}

//------------------------------------------------------------------------------
// Implementation - LSA TIMER FOR THREAD GROUP 1
//------------------------------------------------------------------------------

//******************************************************************************
//  Function: internal_timer_stop_tgroup1
//
//  Description:
//
//      Stop a timer and remove it from its timer list.
//
//  Parameters:
//
//      timer       -- in:  timer to be stopped and removed from the list
//
//  Return:
//
//      None
//******************************************************************************
static void internal_timer_stop_tgroup1(
    TimerRecord *   timer )
{
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE, "eps_timer internal_stop1 %d", timer->timer_id);

    if (NULL == timer->prior)
    {
        if (LSA_TIME_BASE_1MS == timer->time_base)
        {
            state1.first1msTimer = timer->next;
        }
        else if (LSA_TIME_BASE_10MS == timer->time_base)
        {
            state1.first10msTimer = timer->next;
        }
        else if (LSA_TIME_BASE_100MS == timer->time_base)
        {
            state1.first100msTimer = timer->next;
        }
        else if (LSA_TIME_BASE_1S == timer->time_base)
        {
            state1.first1sTimer = timer->next;
        }
        else if (LSA_TIME_BASE_10S == timer->time_base)
        {
            state1.first10sTimer = timer->next;
        }
        else /* LSA_TIME_BASE_100S */
        {
            state1.first100sTimer = timer->next;
        }
    }
    else
    {
        (timer->prior)->next = timer->next;
    }
    if (NULL != timer->next)
    {
        (timer->next)->prior = timer->prior;
    }
    timer->status = TS_STOPPED;
}

//******************************************************************************
//  Function: internal_timer_timeout_tgroup1
//
//  Description:
//
//      Implement the time-keeping algorithm. The implementation assumes a small
//      number of timers. It runs in IRQ state1 in 1 ms tick.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
//#pragma ghs nothumb
static void internal_timer_timeout_tgroup1( union sigval sig )
{
    register TimerRecord * timer;
    register uint32_t      cur_tick;
    struct itimerspec its;

    LSA_UNUSED_ARG(sig);
    
    // if Shutdown is started, ignore this function
    if (state1.eState == eThReqStop)
    {
    	memset( &its, 0, sizeof(its) );

    	EPS_POSIX_TIMER_SETTIME((timer_t)tid1, 0, &its, NULL); // the timer disarm itself
        // Other thread is cleaning up this thread
        // the cleanup thread waits until this thread is stopped
        // and cleans this thread afterwards.
        state1.eState = eThStopped;

        return;
    }

    if ((state1.eState == eThStopped) || (state1.eState == eThInit))
    {
    	return; // timer can be triggered one time again after delete(eThStopped) and while init(eThInit)
    }

    eps_enter_critical_section(state1.lock_id);

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer internal_timer_timeout_tgroup1");
    cur_tick = state1.last_tick + 1;
    state1.last_tick = cur_tick;
    timer = (TimerRecord *)&state1.first1msTimer;
    
    if ( !PSI_FATAL_ERROR_OCCURED() )
    {
        while (NULL != (timer = timer->next) )
        {
            if (cur_tick == timer->target)
            {
      			EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer1 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                timer->target = cur_tick + timer->preset;
                if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                {
                    (*timer->callback) (timer->timer_id, timer->user_id);
                    continue;
                }
                internal_timer_stop_tgroup1(timer);
                (*timer->callback) (timer->timer_id, timer->user_id);
            }
        }

        if (cur_tick == state1.next_10ms_tick)
        {
            state1.next_10ms_tick = cur_tick + 10;
            timer = (TimerRecord *)&state1.first10msTimer;
            while (NULL != (timer = timer->next) )
            {
                if (cur_tick == timer->target)
                {
        			EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer1 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                    timer->target = cur_tick + timer->preset;
                    if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                    {
                        (*timer->callback) (timer->timer_id, timer->user_id);
                        continue;
                    }
                    internal_timer_stop_tgroup1(timer);
                    (*timer->callback) (timer->timer_id, timer->user_id);
                }
            }

            if (cur_tick == state1.next_100ms_tick)
            {
                state1.next_100ms_tick = cur_tick + 100;
                timer = (TimerRecord *)&state1.first100msTimer;
                while (NULL != (timer = timer->next) )
                {
                    if (cur_tick == timer->target)
                    {
                        EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer1 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                        timer->target = cur_tick + timer->preset;
                        if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                        {
                            (*timer->callback) (timer->timer_id, timer->user_id);
                            continue;
                        }
                        internal_timer_stop_tgroup1(timer);
                        (*timer->callback) (timer->timer_id, timer->user_id);
                    }
                }

                if (cur_tick == state1.next_1s_tick)
                {
                    state1.next_1s_tick = cur_tick + 1000;
                    timer = (TimerRecord *)&state1.first1sTimer;
                    while (NULL != (timer = timer->next) )
                    {
                        if (cur_tick == timer->target)
                        {
                            EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer1 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                            timer->target = cur_tick + timer->preset;
                            if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                            {
                                (*timer->callback) (timer->timer_id, timer->user_id);
                                continue;
                            }
                            internal_timer_stop_tgroup1(timer);
                            (*timer->callback) (timer->timer_id, timer->user_id);
                        }
                    }
                }

                if (cur_tick == state1.next_10s_tick)
                {
                    state1.next_10s_tick = cur_tick + 10000;
                    timer = (TimerRecord *)&state1.first10sTimer;
                    while (NULL != (timer = timer->next) )
                    {
                        if (cur_tick == timer->target)
                        {
                            EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer1 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                            timer->target = cur_tick + timer->preset;
                            if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                            {
                                (*timer->callback) (timer->timer_id, timer->user_id);
                                continue;
                            }
                            internal_timer_stop_tgroup1(timer);
                            (*timer->callback) (timer->timer_id, timer->user_id);
                        }
                    }
                }

                if (cur_tick == state1.next_100s_tick)
                {
                    state1.next_100s_tick = cur_tick + 100000;
                    timer = (TimerRecord *)&state1.first100sTimer;
                    while (NULL != (timer = timer->next) )
                    {
                        if (cur_tick == timer->target)
                        {
                            EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer1 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                            timer->target = cur_tick + timer->preset;
                            if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                            {
                                (*timer->callback) (timer->timer_id, timer->user_id);
                                continue;
                            }
                            internal_timer_stop_tgroup1(timer);
                            (*timer->callback) (timer->timer_id, timer->user_id);
                        }
                    }
                }
            }
        }
    }
    else
    {
        LSA_UINT32 i;
        
        EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer internal_timer_timeout_tgroup1 - else case");
      
        for (i = 0; i < countof(state1.timers); i++)
        {
            if(TS_FREE != state1.timers[i].status)
            {
                if(TS_RUNNING == state1.timers[i].status)
                {
                    internal_timer_stop_tgroup1(&state1.timers[i]);
                }
                eps_timer_free_tgroup1(state1.timers[i].timer_id);
            }
        }

        eps_timer_undo_init_tgroup1();
    }
    
    eps_exit_critical_section(state1.lock_id);
}
//#pragma ghs thumb

//******************************************************************************
//  Function: eps_timer_init_tgroup1
//
//  Description:
//
//      This function will initialize rhe IRQ-based timer subsystem.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      LSA_RET_OK            The timer subsystem was initialized successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of resources
//******************************************************************************
LSA_UINT16 eps_timer_init_tgroup1( LSA_VOID )
{
    pthread_attr_t     thread_attr;
    struct sched_param sched_param;
    struct sigevent    event;
    struct itimerspec  timeout;
    int                ret;
    LSA_UINT16         i;

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_init_tgroup1 - begin");
    // initialize the state1 record
    memset( &state1, 0, sizeof(state1) );

    ret = eps_alloc_critical_section( &state1.lock_id, LSA_FALSE );
    if (ret != LSA_RET_OK)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    state1.next_10ms_tick 	= 10;
    state1.next_100ms_tick 	= 100;
    state1.next_1s_tick 	= 1000;
    state1.next_10s_tick 	= 10000;
    state1.next_100s_tick 	= 100000;

    for (i = 0; i < countof(state1.timers); i++)
    {
        state1.timers[i].timer_id = i;
    }

    // initialize the "thread" attributes (we don't use a thread)
    memset( &thread_attr, 0, sizeof( thread_attr ) );
    ret = EPS_POSIX_PTHREAD_ATTR_INIT( &thread_attr );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if (EPS_PLF == EPS_PLF_LINUX_X86)
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY( &thread_attr, /*SCHED_OTHER atw12r13*/ SCHED_FIFO );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif

    // initialize scheduling parameters
    memset( &sched_param, 0, sizeof( sched_param ) );

    sched_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH;
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM( &thread_attr, &sched_param );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if (EPS_PLF == EPS_PLF_LINUX_X86)
    sched_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH + 1;
    ret = sched_setscheduler( 0, SCHED_FIFO, &sched_param );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif

    // set schedule_parameters as explicit, otherwise they are taken from this thread
    ret = EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED( &thread_attr, PTHREAD_EXPLICIT_SCHED );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if (EPS_PLF != EPS_PLF_LINUX_X86)
    //set thread name
    ret = pthread_attr_setname( &thread_attr, "TIMER_TGROUP1" );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif

    // TODO need Threadname for Linux

    // fill event data and create the timer
    memset( &event, 0, sizeof( event ) );
    event.sigev_notify = SIGEV_THREAD;

    #if (EPS_PLF == EPS_PLF_LINUX_X86)
    event.sigev_signo = SIGRTMIN;
    #else
    event.sigev_signo = SIGRT15;
    #endif

    event.sigev_notify_function   = &internal_timer_timeout_tgroup1;
    event.sigev_notify_attributes = &thread_attr;
    event.sigev_value.sival_ptr   = NULL;
    ret = EPS_POSIX_TIMER_CREATE( CLOCK_MONOTONIC, &event, (timer_t*)&tid1 );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // start the timer - 1 ms cyclic timeout
    timeout.it_value.tv_sec     = 0;
    timeout.it_value.tv_nsec    = 1000000;
    timeout.it_interval.tv_sec  = 0;
    timeout.it_interval.tv_nsec = 1000000;
    ret = EPS_POSIX_TIMER_SETTIME( (timer_t)tid1, 0, &timeout, NULL );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_init_tgroup1 - done");

    return LSA_RET_OK;
}

LSA_VOID eps_timer_undo_init_tgroup1( LSA_VOID )
{
	LSA_UINT32 i;

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_undo_init_tgroup1 - begin, Timer-State=%u", state1.eState);
    // Tell the timeout function to stop it's work.
    state1.eState = eThReqStop;

    // Synchronize with other thread/timer wait until it is stopped
    while(state1.eState != eThStopped)
    {
        eps_tasks_sleep(1);
    }

    eps_enter_critical_section( state1.lock_id );
	for (i = 0; i < countof(state1.timers); i++)
	{
		EPS_ASSERT(state1.timers[i].status == TS_FREE);
	}

	EPS_POSIX_TIMER_DELETE( (timer_t)tid1 );
    eps_exit_critical_section( state1.lock_id );
    // do the actual clean up after timer is stopped!
	eps_free_critical_section( state1.lock_id );
    state1.lock_id = 0;
	
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_undo_init_tgroup1 - done, Timer-State=%u", state1.eState);
}


//******************************************************************************
//  Function: eps_timer_allocate_tgroup1
//
//  Description:
//
//      This function will allocate an IRQ-based timer.
//
//  Parameters:
//
//      timer_id_ptr -- out:  points to returned value of timer's identifier
//      timer_type   -- in:   Type of timer -
//
//                                  LSA_TIMER_TYPE_ONE_SHOT or
//                                  LSA_TIMER_TYPE_CYCLIC
//
//      time_base    -- in:   Units of preset at start --
//
//                                  LSA_TIME_BASE_1MS
//                                  LSA_TIME_BASE_10MS,
//                                  LSA_TIME_BASE_100MS or
//                                  LSA_TIME_BASE_1S
//
//      callback     -- in:   pointer to function called on timeout.
//
//                              NOTE: Callback function is called in IRQ state1.
//
//  Return:
//
//      LSA_RET_OK            The timer was allocated successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of timer resources
//      LSA_RET_ERR_PARAM     The function failed due to a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_allocate_tgroup1(
  LSA_TIMER_ID_TYPE * timer_id_ptr,
  LSA_UINT16          timer_type,
  LSA_UINT16          time_base,
  EPS_TIMER_CBF       callback )
{  
  LSA_UINT16 i;
  
  eps_enter_critical_section(state1.lock_id);

  EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_allocate1 (0x%X, %d, %d, 0x%X)",timer_id_ptr,timer_type,time_base,callback);

  if ( ( (timer_type == LSA_TIMER_TYPE_ONE_SHOT) ||
         (timer_type == LSA_TIMER_TYPE_CYCLIC) ) &&
       ( (time_base == LSA_TIME_BASE_1MS) ||
         (time_base == LSA_TIME_BASE_10MS) ||
         (time_base == LSA_TIME_BASE_100MS) ||
         (time_base == LSA_TIME_BASE_1S) ||
         (time_base == LSA_TIME_BASE_10S) ||
         (time_base == LSA_TIME_BASE_100S) ) &&
         (NULL != callback) )
  {
    for (i = 0; i < countof(state1.timers); i++)
    {
      if (TS_FREE == state1.timers[i].status)
      {
        register TimerRecord * timer = &(state1.timers[i]);

        timer->prior = NULL;
        timer->next = NULL;
        timer->status = TS_STOPPED;
        timer->timer_type = timer_type;
        timer->time_base = time_base;
        timer->callback = callback;
        memset(&(timer->user_id), 0, sizeof(timer->user_id));
        timer->target = 0;
        timer->preset = 0;
        *timer_id_ptr = timer->timer_id;
        EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_allocate1 -- success, ID = %d",timer->timer_id);
		        
		eps_exit_critical_section(state1.lock_id);
		
		EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_allocate_tgroup1 - timer_id = %d", timer->timer_id);
		
        return LSA_RET_OK;
      }
    }
    EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,"eps_timer_allocate1 (0x%X, %d, %d, 0x%X) -- failed, no timers available",timer_id_ptr,timer_type,time_base,callback);    
	eps_exit_critical_section(state1.lock_id);
    return LSA_RET_ERR_NO_TIMER;
  }
  EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_allocate1 (0x%X, %d, %d, 0x%X) -- failed, parameter error",timer_id_ptr,timer_type,time_base,callback);
  eps_exit_critical_section(state1.lock_id);
  return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_start_tgroup1
//
//  Description:
//
//      This function will start timing on a timer allocated using
//      eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to start
//      user_id      -- in:   Caller's identifier for the timeout instance
//      time         -- in:   timeout time in units of time_base at
//                            eps_timer_allocate
//
//  Return:
//
//      LSA_RET_OK                  The timer was stopped and has been started.
//      LSA_RET_OK_TIMER_RESTARTED  The timer was running and has been restarted
//      LSA_RET_ERR_PARAM           The function failed due to a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_start_tgroup1(
  LSA_TIMER_ID_TYPE   timer_id,
  LSA_USER_ID_TYPE    user_id,
  LSA_UINT16          time )
{
  eps_enter_critical_section(state1.lock_id);  
    
  EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_start1 (%d, 0x%X, %d)",timer_id,user_id.void_ptr,time);
  
  if ( (timer_id < countof(state1.timers)) &&
       (0 < time) )
  {
    register TimerRecord * timer = &(state1.timers[timer_id]);    

    if ( (TS_STOPPED == timer->status) ||
         (TS_RUNNING == timer->status) )
    {
      TimerRecord ** anchor = NULL;

      timer->user_id = user_id;
      if (LSA_TIME_BASE_1MS == timer->time_base)
      {
        timer->preset = time;
        timer->target = state1.last_tick + timer->preset + 1;
        anchor = (TimerRecord **)&state1.first1msTimer;
      }
      else if (LSA_TIME_BASE_10MS == timer->time_base)
      {
        timer->preset = time * 10;
        timer->target = state1.next_10ms_tick + timer->preset;
        anchor = (TimerRecord **)&state1.first10msTimer;
      }
      else if (LSA_TIME_BASE_100MS == timer->time_base)
      {
        timer->preset = time * 100;
        timer->target = state1.next_100ms_tick + timer->preset;
        anchor = (TimerRecord **)&state1.first100msTimer;
      }
      else if (LSA_TIME_BASE_1S == timer->time_base)
      {
        timer->preset = time * 1000;
        timer->target = state1.next_1s_tick + timer->preset;
        anchor = (TimerRecord **)&state1.first1sTimer;
      }
      else if (LSA_TIME_BASE_10S == timer->time_base)
      {
        timer->preset = time * 10000;
        timer->target = state1.next_10s_tick + timer->preset;
        anchor = (TimerRecord **)&state1.first10sTimer;
      }
      else /* LSA_TIME_BASE 100S */
      {
        timer->preset = time * 100000;
        timer->target = state1.next_100s_tick + timer->preset;
        anchor = (TimerRecord **)&state1.first100sTimer;
      }
      if (TS_STOPPED == timer->status)
      {
        timer->prior = NULL;
        timer->next = *anchor;
        if (NULL != *anchor)
        {
          (*anchor)->prior = timer;
        }
        *anchor = timer;
        timer->status = TS_RUNNING;
        state1.eState = eThRun;
        EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start1 -- succeeded, timer started");        
		eps_exit_critical_section(state1.lock_id);
        return LSA_RET_OK;
      }
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start1 -- succeeded, timer restarted");
	  eps_exit_critical_section(state1.lock_id);
      //return LSA_RET_OK_TIMER_RESTARTED;
      return LSA_RET_OK; //@@@EDDI_StartTimer()?!
    }
  }
  
  if(state1.timers == LSA_NULL)
  {
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_FATAL,">< eps_timer_start1 - TIMER INVALID");
      EPS_FATAL("eps_timer_start1 - TIMER INVALID");
      return LSA_RET_ERR_RESOURCE;
  }
  else
  {
      register TimerRecord * timer = &(state1.timers[timer_id]);
      
      EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_ERROR,">< eps_timer_start1 (timer_id = %d, user_id.void_ptr = 0x%X, time = %d, Status = %d) -- failed, parameter error)",timer_id, user_id.void_ptr, time, timer->status);
      eps_exit_critical_section(state1.lock_id);
      return LSA_RET_ERR_PARAM;
  }
}

//******************************************************************************
//  Function: eps_timer_stop_tgroup1
//
//  Description:
//
//      This function will stop timing on a timer started using eps_timer_start.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to stop
//
//  Return:
//
//      LSA_RET_OK                    The timer was stopped.
//      LSA_RET_OK_TIMER_NOT_RUNNING  The timer was not running
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_stop_tgroup1(
    LSA_TIMER_ID_TYPE   timer_id )
{
  eps_enter_critical_section(state1.lock_id);

  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_stop1 (%d)",timer_id);
  if (timer_id < countof(state1.timers))
  {
    register TimerRecord * timer = &(state1.timers[timer_id]);    

    if (TS_STOPPED == timer->status)
    {
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop1 -- succeeded, timer was not running");
	  eps_exit_critical_section(state1.lock_id);
      return LSA_RET_OK_TIMER_NOT_RUNNING;
    }
    else if (TS_RUNNING == timer->status)
    {
      internal_timer_stop_tgroup1(timer);
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop1 -- succeeded");
	  eps_exit_critical_section(state1.lock_id);
      return LSA_RET_OK;
    }
  }
  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,">< eps_timer_stop1 (%d) -- failed, parameter error",timer_id);
  eps_exit_critical_section(state1.lock_id);
  return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_free_tgroup1
//
//  Description:
//
//      This function will release a timer allocated using eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to release
//
//  Return:
//
//      LSA_RET_OK                    The timer was released.
//      LSA_RET_ERR_TIMER_IS_RUNNING  The timer is running and was not released
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_free_tgroup1(
    LSA_TIMER_ID_TYPE   timer_id )
{     
  eps_enter_critical_section(state1.lock_id);

  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_free1 (%d)",timer_id);
  if (timer_id < countof(state1.timers))
  {
    register TimerRecord * timer = &(state1.timers[timer_id]);

    if (TS_STOPPED == timer->status)
    {
      timer->status = TS_FREE;
      EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_free1 -- succeeded");      
	  eps_exit_critical_section(state1.lock_id);	  
	  
	  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_free_tgroup1 - timer_id = %d", timer_id);
	  
      return LSA_RET_OK;
    }
    else if (TS_RUNNING == timer->status)
    {
      EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free1 (%d) -- failed, timer is running",timer_id);      
	  eps_exit_critical_section(state1.lock_id);
      return LSA_RET_ERR_TIMER_IS_RUNNING;
    }
  }
  EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free1 (%d) -- failed, parameter error",timer_id);
  eps_exit_critical_section(state1.lock_id);
  return LSA_RET_ERR_PARAM;
}

/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
