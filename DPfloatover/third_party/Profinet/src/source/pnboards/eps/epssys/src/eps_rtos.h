#ifndef EPS_RTOS_H_
#define EPS_RTOS_H_

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
/*  F i l e               &F: eps_rtos.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS RTOS specific includes for POSIX interface                           */
/*                                                                           */
/*****************************************************************************/

#define _EPSRTOS_QUOTEME(x) #x
#define EPSRTOS_QUOTEME(x) _EPSRTOS_QUOTEME(x) 

#ifndef EPS_RTOS_CFG_INCLUDE_H
#error "EPS_RTOS_CFG_INCLUDE_H not defined in makefiles"
#endif

/* Operating system includes (posix interface) to EPS which needs
   to be defined by compiler args 
   (e.g. -DEPS_RTOS_CFG_INCLUDE_H=eps_posix_cfg_windows.h) */

#include EPSRTOS_QUOTEME(EPS_RTOS_CFG_INCLUDE_H)   
 
/*===========================================================================*/
/*        posix interface                                                    */
/*===========================================================================*/

#ifndef EPS_POSIX_PTHREAD_ATTR_INIT
    #error "EPS_POSIX_PTHREAD_ATTR_INIT not defined" 
#endif
#ifndef EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED
    #error "EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_ATTR_SETSTACK
    #error "EPS_POSIX_PTHREAD_ATTR_SETSTACK not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_ATTR_SETSTACKSIZE
    #error "EPS_POSIX_PTHREAD_ATTR_SETSTACKSIZE not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM
    #error "EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY
    #error "EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_ATTR_SETCPUAFFINITY
    #error "EPS_POSIX_PTHREAD_ATTR_SETCPUAFFINITY not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_CREATE
    #error "EPS_POSIX_PTHREAD_CREATE not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_JOIN
    #error "EPS_POSIX_PTHREAD_JOIN not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_KILL
    #error "EPS_POSIX_PTHREAD_KILL not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_MUTEX_DESTROY
    #error "EPS_POSIX_PTHREAD_MUTEX_DESTROY not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_MUTEX_INIT
    #error "EPS_POSIX_PTHREAD_MUTEX_INIT not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_MUTEX_LOCK
    #error "EPS_POSIX_PTHREAD_MUTEX_LOCK not defined"
#endif
#ifndef EPS_POSIX_PTHREAD_MUTEX_UNLOCK
    #error "EPS_POSIX_PTHREAD_MUTEX_UNLOCK not defined"
#endif
#ifndef EPS_POSIX_TIMER_CREATE
    #error "EPS_POSIX_TIMER_CREATE not defined"
#endif
#ifndef EPS_POSIX_TIMER_SETTIME
    #error "EPS_POSIX_TIMER_SETTIME not defined"
#endif
#ifndef EPS_POSIX_TIMER_DELETE
    #error "EPS_POSIX_TIMER_DELETE not defined"
#endif
#ifndef EPS_POSIX_SEM_INIT
    #error "EPS_POSIX_SEM_INIT not defined"
#endif
#ifndef EPS_POSIX_SEM_DESTROY
    #error "EPS_POSIX_SEM_DESTROY not defined"
#endif
#ifndef EPS_POSIX_SEM_WAIT
    #error "EPS_POSIX_SEM_WAIT not defined"
#endif
#ifndef EPS_POSIX_SEM_POST
    #error "EPS_POSIX_SEM_POST not defined"
#endif
#ifndef EPS_POSIX_CLOCK_GETTIME
    #error "EPS_POSIX_CLOCK_GETTIME not defined"
#endif
#ifndef EPS_POSIX_GETTIMEOFDAY
    #error "EPS_POSIX_GETTIMEOFDAY not defined"
#endif
#ifndef EPS_POSIX_NANOSLEEP
    #error "EPS_POSIX_NANOSLEEP not defined"
#endif

//TODO -> non posix
#if 0
#define EPS_POSIX_PTHREAD_ATTR_SETNAME              pthread_attr_setname    //Windows
#define EPS_POSIX_PTHREAD_ATTR_SETNAME_NP           pthread_setname_np      //Linux
#endif


#ifndef EPS_POSIX_THREAD_PRIORITY_NORMAL
#error "EPS_POSIX_THREAD_PRIORITY_NORMAL not defined"
#endif

#ifndef EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL   
#error "EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL not defined"
#endif

#ifndef EPS_POSIX_THREAD_PRIORITY_HIGH           
#error "EPS_POSIX_THREAD_PRIORITY_HIGH not defined"
#endif

#ifndef EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE 
#error "EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE not defined"
#endif


#endif  /* of EPS_RTOS_H_ */
