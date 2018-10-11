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
/*  F i l e               &F: eps_locks.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Locking adaption                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20005
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID

#include "psi_inc.h"
#include "psi_cfg.h"   // required LSA base settings
#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"
#include "eps_trc.h"
#include "eps_sys.h"

#include "eps_rtos.h"


/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/

#define EPS_LOCK_ID_MAX   150

/*---------------------------------------------------------------------------*/
/* Types                                                                     */
/*---------------------------------------------------------------------------*/

typedef struct
 {
    LSA_BOOL            is_used; 
    pthread_mutex_t     enter_exit;
 }EPS_LOCK_ENTRY_TYPE, *EPS_LOCK_ENTRY_PTR_TYPE ;


 typedef struct
 {
   LSA_BOOL                 is_running;                // Initialized yes/no
   pthread_mutex_t          enter_exit;                // Own reentrance lock
   EPS_LOCK_ENTRY_TYPE      locks[EPS_LOCK_ID_MAX+1];   // Lock entry list
 }EPS_LOCK_INSTANCE_TYPE, *EPS_LOCK_INSTANCE_PTR_TYPE;

 
 /*-----------------------------------------------------------------*/
 /* Global data                                                     */
 /*-----------------------------------------------------------------*/
 
 static EPS_LOCK_INSTANCE_TYPE eps_locks = {0};
 
/**
 * Initializes the eps_lock module.
 * 
 * A number of EPS_LOCK_ID_MAX locks are allocated and managed by eps_locks.
 * 
 * @param LSA_VOID
 * @return
 */
 LSA_VOID eps_locks_init( LSA_VOID )
 {
   LSA_UINT16 entry;

	#if (EPS_PLF == EPS_PLF_LINUX_X86)
    pthread_mutexattr_t mutex_attr;
    EPS_ASSERT( !eps_locks.is_running );
    pthread_mutexattr_init(&mutex_attr);
    EPS_ASSERT(0 == pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE));

    pthread_mutex_init(&eps_locks.enter_exit, &mutex_attr);
	#else    
    EPS_ASSERT( !eps_locks.is_running );    
   // guard this code section against mutal access
    EPS_POSIX_PTHREAD_MUTEX_INIT(&eps_locks.enter_exit, 0);
	#endif

   // initialize all locks
   for ( entry = 0; entry <= EPS_LOCK_ID_MAX; entry++ )
   {
     eps_locks.locks[entry].is_used = LSA_FALSE;
   }

   eps_locks.is_running = LSA_TRUE; // Now initialized

   EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
     "eps_locks_init(): locks init done, max locks(%u)", 
     EPS_LOCK_ID_MAX );
 }

/**
 * Undo initialization of the eps_locks implementation
 * @param LSA_VOID
 * @return
 */
 LSA_VOID eps_locks_undo_init( LSA_VOID )
 {
   LSA_UINT16 entry;

   EPS_ASSERT( eps_locks.is_running );

   for ( entry = 0; entry <= EPS_LOCK_ID_MAX; entry++ )
   {
     // check all locks are freed 
     EPS_ASSERT( !eps_locks.locks[entry].is_used )
   }

   eps_locks.is_running = LSA_FALSE;

    EPS_POSIX_PTHREAD_MUTEX_DESTROY( &eps_locks.enter_exit );
   
   EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
     "eps_locks_undo_init(): locks cleanup done, max locks(%u)", 
     EPS_LOCK_ID_MAX );
 }
 
/**
 * This function allocates a critical section using the POSIX API pthread_mutex_init.
 * A recursive lock may be allocated. Recursive means that the same thread may allocate the same lock again without 
 * running into a self deadlock. 
 * This also means, that eps_enter_critical_section can not be used to guard a critical section against mutal access from the same thread.
 * 
 * In the current implementation, the Windows implementation always allocates a recursive lock since the API does not 
 * support non recursive locks.
 * 
 * @see eps_free_critical_section     - this function undoes the allocation
 * @see pthread_mutex_init            - this function is called by EPS.
 * 
 * @param [out] lock_id_ptr           - handle to the lock
 * @param [in] isPtreadMutexRecursive - LSA_TRUE: Recursive lock, LSA_FALSE: non recursive lock
 * @return #LSA_RET_ERR_RESOURCE      - all possible locks are already taken
 * @return #LSA_RET_OK                - Lock was successfully created.
 */
 LSA_RESPONSE_TYPE eps_alloc_critical_section( LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive)
 {
   LSA_UINT16        entry;
   LSA_RESPONSE_TYPE retVal = LSA_RET_ERR_RESOURCE;

    #if (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_WINDOWS_X86)
    isPtreadMutexRecursive = LSA_TRUE;
    #endif  

#ifdef WIN32
	 LSA_UNUSED_ARG(isPtreadMutexRecursive);
#else
   pthread_mutexattr_t mutex_attr;
#endif

   EPS_ASSERT( lock_id_ptr != 0 );
   EPS_ASSERT( eps_locks.is_running );

    EPS_POSIX_PTHREAD_MUTEX_LOCK ( &eps_locks.enter_exit );

   for ( entry = 1; entry <= EPS_LOCK_ID_MAX; entry++ ) // Note: 0 is reserved (init val)
   {
     if ( !eps_locks.locks[entry].is_used )
     {
            #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
        	EPS_POSIX_PTHREAD_MUTEX_INIT( &eps_locks.locks[entry].enter_exit,0 );
			#else
       if(isPtreadMutexRecursive)
       {
         // set mutex attributes to recursive
    	   pthread_mutexattr_init(&mutex_attr);
    	   EPS_ASSERT(0 == pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE));
           EPS_POSIX_PTHREAD_MUTEX_INIT( &eps_locks.locks[entry].enter_exit, &mutex_attr);
       }
       else
       {
         // alloc non recursive lock
          EPS_POSIX_PTHREAD_MUTEX_INIT( &eps_locks.locks[entry].enter_exit,0 );
     }
#endif

       eps_locks.locks[entry].is_used = LSA_TRUE;

       *lock_id_ptr = entry;
       retVal = LSA_RET_OK;
       break;
     }
   }

    EPS_POSIX_PTHREAD_MUTEX_UNLOCK ( &eps_locks.enter_exit );

   return ( retVal );
 }
 
 /*-----------------------------------------------------------------*/
/**
 * Releases the allocated lock in eps_alloc_critical_section
 * 
 * @see eps_alloc_critical_section - alloc function
 * 
 * @param [in] lock_id             - handle to the lock that must be released
 * @return #LSA_RET_OK             - everything is fine
 * @return #LSA_RET_ERR_PARAM      - for some reason, this lock was already released.
 */
LSA_RESPONSE_TYPE eps_free_critical_section( LSA_UINT16 lock_id )
{
  LSA_RESPONSE_TYPE retVal = LSA_RET_OK;

  EPS_ASSERT( eps_locks.is_running );

    EPS_POSIX_PTHREAD_MUTEX_LOCK ( &eps_locks.enter_exit );

  if (( lock_id > 0 ) && 
    ( lock_id <= EPS_LOCK_ID_MAX ) &&           // in range ?
    ( eps_locks.locks[lock_id].is_used ))   // allocated ?
  {
        EPS_POSIX_PTHREAD_MUTEX_DESTROY( &eps_locks.locks[lock_id].enter_exit );

    eps_locks.locks[lock_id].is_used = LSA_FALSE;

    retVal = LSA_RET_OK;
  }
  else
  {
    retVal = LSA_RET_ERR_PARAM;
  }
    EPS_POSIX_PTHREAD_MUTEX_UNLOCK ( &eps_locks.enter_exit );

  return ( retVal );
}

/**
 * Enters a critical section and guards it against mutal access from different threads. For recursive locks, this does _not_ guard
 * against a reentrance within the same thread (this is explicitly allowed in recursive usecase).
 * 
 * Check if the lock was successfully entered. Note that for non recursive locks, we may get a EDEADLK as retval if the same thread 
 * tries to enter this code section _again_ without unlocking the mutex. EPS assumes that the user of the lock would allocate a 
 * recursive lock, in this case a FATAL occurs.
 * 
 * @param [in] lock_id      - handle to the lock
 * @return LSA_VOID
 */
LSA_VOID eps_enter_critical_section( LSA_UINT16 lock_id )
 {
  LSA_INT retVal = 0; //success code in posix API
  EPS_ASSERT( eps_locks.is_running );

    EPS_ASSERT( lock_id <= EPS_LOCK_ID_MAX );
  EPS_ASSERT( eps_locks.locks[lock_id].is_used );

    retVal = EPS_POSIX_PTHREAD_MUTEX_LOCK( &eps_locks.locks[lock_id].enter_exit );
  
  // check retval. Possible values may be EDEADLK == 25 if a non recursive lock was used.
  // EPS_ASSERT (retVal == 0); //z00301au TODO: SelfDeadlock at LOCK_NET_RESOURCE - ASSERT triggers here, commented out.
}

/**
 * Exits a critical section. Note that for recursive locks, a thread has to call the exit function as often as an enter was 
 * called before an other thread may access the ciritical section.
 * 
 * @param [in] lock_id - handle to the lock.
 * @return LSA_VOID
 */
LSA_VOID eps_exit_critical_section( LSA_UINT16 lock_id )
{
  EPS_ASSERT( eps_locks.is_running );

    EPS_ASSERT( lock_id <= EPS_LOCK_ID_MAX );
  EPS_ASSERT( eps_locks.locks[lock_id].is_used );

    EPS_POSIX_PTHREAD_MUTEX_UNLOCK( &eps_locks.locks[lock_id].enter_exit );
}
 /*****************************************************************************/
 /*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
 /*****************************************************************************/

