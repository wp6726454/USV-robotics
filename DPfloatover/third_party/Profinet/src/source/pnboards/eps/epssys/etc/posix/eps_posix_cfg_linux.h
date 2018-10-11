#ifndef _EPS_POSIX_CFG_LINUX_H_
#define _EPS_POSIX_CFG_LINUX_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: posix2win.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Posix-IF for Linux                                    				 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define EPS_POSIX_PTHREAD_ATTR_INIT  				pthread_attr_init
#define EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED      pthread_attr_setinheritsched
#define EPS_POSIX_PTHREAD_ATTR_SETSTACK             pthread_attr_setstack
#define EPS_POSIX_PTHREAD_ATTR_SETSTACKSIZE         pthread_attr_setstacksize
#define EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM        pthread_attr_setschedparam
#define EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY       pthread_attr_setschedpolicy
#define EPS_POSIX_PTHREAD_ATTR_SETCPUAFFINITY       pthread_attr_setaffinity_np
#define EPS_POSIX_PTHREAD_CREATE                    pthread_create
#define EPS_POSIX_PTHREAD_JOIN                      pthread_join
#define EPS_POSIX_PTHREAD_KILL                      pthread_kill
#define EPS_POSIX_PTHREAD_MUTEX_DESTROY             pthread_mutex_destroy
#define EPS_POSIX_PTHREAD_MUTEX_INIT                pthread_mutex_init
#define EPS_POSIX_PTHREAD_MUTEX_LOCK                pthread_mutex_lock
#define EPS_POSIX_PTHREAD_MUTEX_UNLOCK              pthread_mutex_unlock
#define EPS_POSIX_TIMER_CREATE                      timer_create
#define EPS_POSIX_TIMER_SETTIME                     timer_settime
#define EPS_POSIX_TIMER_DELETE                      timer_delete
#define EPS_POSIX_SEM_INIT                          sem_init
#define EPS_POSIX_SEM_DESTROY                       sem_destroy
#define EPS_POSIX_SEM_WAIT                          sem_wait
#define EPS_POSIX_SEM_TIMEDWAIT                     sem_timedwait
#define EPS_POSIX_SEM_POST                          sem_post
#define EPS_POSIX_CLOCK_GETTIME                     clock_gettime
#define EPS_POSIX_GETTIMEOFDAY                      gettimeofday
#define EPS_POSIX_NANOSLEEP                         nanosleep

#define EPS_POSIX_THREAD_PRIORITY_NORMAL    		51
#define EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL   	52		
#define EPS_POSIX_THREAD_PRIORITY_HIGH           	53	
#define EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE 	99

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of _EPS_POSIX_CFG_LINUX_H_ */
