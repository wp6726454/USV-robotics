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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: eps_events.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Event Api                                                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20004
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include "psi_inc.h"
#include "psi_cfg.h"   // required LSA base settings
#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"
#include "eps_trc.h"
#include "eps_sys.h"
#include "eps_locks.h"

#include "eps_rtos.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*-----------------------------------------------------------------*/
/* Defines                                                         */
/*-----------------------------------------------------------------*/

#define EPS_WAIT_EVENT_MAX  80

/*-----------------------------------------------------------------*/
/* Types                                                           */
/*-----------------------------------------------------------------*/

typedef struct
 {
	LSA_BOOL             is_used;
	LSA_UINT16           sema_id;
	sem_t                event_sema;
 }EPS_EVENT_ENTRY_TYPE, *EPS_EVENT_ENTRY_PTR_TYPE ;


typedef struct  
 { 
	LSA_BOOL                is_running;                           // Initialized yes/no
	LSA_UINT16              enter_exit;                           // Own reentrance lock
	EPS_EVENT_ENTRY_TYPE    events[EPS_WAIT_EVENT_MAX+1];         // Wait event list
} EPS_WAIT_EVENT_INSTANCE_TYPE, *EPS_WAIT_EVENT_INSTANCE_PTR_TYPE;


/*-----------------------------------------------------------------*/
/* Global data                                                     */
/*-----------------------------------------------------------------*/

static EPS_WAIT_EVENT_INSTANCE_TYPE eps_events = {0};

/*-----------------------------------------------------------------*/
/* Implementation of eps events (Wait events)                      */
/*-----------------------------------------------------------------*/

LSA_VOID eps_events_init( LSA_VOID )
{
	LSA_UINT16        entry;
	LSA_RESPONSE_TYPE retVal;

	EPS_ASSERT( !eps_events.is_running );

	retVal = eps_alloc_critical_section(&eps_events.enter_exit, LSA_FALSE);
	EPS_ASSERT( retVal == LSA_RET_OK );

	for ( entry = 0; entry <= EPS_WAIT_EVENT_MAX; entry++ )
	{
		eps_events.events[entry].is_used = LSA_FALSE;
		eps_events.events[entry].sema_id = 0;
	}

	eps_events.is_running = LSA_TRUE; // Now initialized
}

/*-----------------------------------------------------------------*/
LSA_VOID eps_events_undo_init( LSA_VOID )
{
	LSA_UINT16 entry;

	EPS_ASSERT( eps_events.is_running );

	for ( entry = 0; entry <= EPS_WAIT_EVENT_MAX; entry++ )
	{
		// check all user wait events are freed 
		EPS_ASSERT( !eps_events.events[entry].is_used );
	}

	eps_events.is_running = LSA_FALSE;

	eps_free_critical_section(eps_events.enter_exit);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
	"eps_events_undo_init(): locks cleanup done, max locks(%u)", 
	EPS_WAIT_EVENT_MAX );
}

/*-----------------------------------------------------------------*/
/* Implementation of event adaption                                */
/*-----------------------------------------------------------------*/

LSA_RESPONSE_TYPE eps_alloc_event( LSA_UINT16 *event_id_ptr)
{
	LSA_UINT16        entry;
	LSA_RESPONSE_TYPE retVal = LSA_RET_ERR_RESOURCE;
	int retValSem;

	EPS_ASSERT( event_id_ptr != LSA_NULL );
	EPS_ASSERT( eps_events.is_running );

	eps_enter_critical_section(eps_events.enter_exit);

	// Note: 0 is reserved for EPS sys wait event
	for ( entry = 0; entry <= EPS_WAIT_EVENT_MAX; entry++ )
	{
    
		if ( !eps_events.events[entry].is_used )
		{
			retValSem =  EPS_POSIX_SEM_INIT(&eps_events.events[entry].event_sema, 0, 0);
			EPS_ASSERT(retValSem == 0);

			eps_events.events[entry].sema_id = entry;
      
			eps_events.events[entry].is_used = LSA_TRUE;

			*event_id_ptr = entry;

			retVal = LSA_RET_OK;
			break;
		}
	}

	eps_exit_critical_section(eps_events.enter_exit);

	return ( retVal );
}

/*-----------------------------------------------------------------*/

LSA_RESPONSE_TYPE eps_free_event( LSA_UINT16 event_id )
{
	LSA_RESPONSE_TYPE retVal = LSA_RET_OK;
	int retValSem;

	EPS_ASSERT( eps_events.is_running );

	eps_enter_critical_section(eps_events.enter_exit);

	// Note: 0 is reserved for EPS sys wait event
	if ( ( event_id >= 0 ) && 
	     ( event_id <= EPS_WAIT_EVENT_MAX ) &&          // in range ?
	     ( eps_events.events[event_id].is_used == LSA_TRUE ))  // allocated ?
	{

		retValSem = EPS_POSIX_SEM_DESTROY(&eps_events.events[event_id].event_sema);
		EPS_ASSERT( retValSem == 0 );
		eps_events.events[event_id].sema_id = 0;

		eps_events.events[event_id].is_used = LSA_FALSE;
    
		retVal = LSA_RET_OK;
	}
	else
	{
		retVal = LSA_RET_ERR_PARAM;
	}

	eps_exit_critical_section(eps_events.enter_exit);

	return ( retVal );
}

/*-----------------------------------------------------------------*/
LSA_VOID eps_set_event( LSA_UINT16 event_id )
{
	int rc = -1;

	EPS_ASSERT( eps_events.is_running );
	EPS_ASSERT( (event_id >= 0 ) && (event_id <= EPS_WAIT_EVENT_MAX ));  // in range
  
	// unlock a semaphore 
	while (!(rc == 0))
	{
		rc = EPS_POSIX_SEM_POST(&eps_events.events[event_id].event_sema);
	}

}

/*-----------------------------------------------------------------*/
LSA_VOID eps_reset_event( LSA_UINT16 event_id )
{
	int rc = -1;

	EPS_ASSERT( eps_events.is_running );
	EPS_ASSERT( (event_id >= 0 ) && (event_id <= EPS_WAIT_EVENT_MAX));   // in range

	while (!(rc == 0))
	{
		rc = EPS_POSIX_SEM_DESTROY(&eps_events.events[event_id].event_sema);
	}
  
	rc = -1;
	while (!(rc == 0))
	{
		rc = EPS_POSIX_SEM_INIT(&eps_events.events[event_id].event_sema,0,0);
	}
}

/*-----------------------------------------------------------------*/
LSA_RESPONSE_TYPE eps_wait_event( LSA_UINT16 event_id, LSA_UINT16 wait_sec )
{
    LSA_RESPONSE_TYPE retVal = LSA_RET_OK;
	int rc;
  
	EPS_ASSERT( eps_events.is_running );
	EPS_ASSERT( (event_id >= 0 ) && (event_id <= EPS_WAIT_EVENT_MAX));  // in range

	if  (PSI_EVENT_NO_TIMEOUT == wait_sec)
	    // wait forever
	{
	    rc = EPS_POSIX_SEM_WAIT(&eps_events.events[event_id].event_sema);
	}
	else
	    // wait with timeout
	{
	    struct timespec WaitSpec;

        #if (EPS_PLF == EPS_PLF_LINUX_X86)
	        //Attention: tv_sec and tv_nsec are of type long!
            EPS_POSIX_CLOCK_GETTIME(CLOCK_REALTIME, &WaitSpec);
	        WaitSpec.tv_sec  += wait_sec;
        #else
            //Attention: tv_sec and tv_nsec are of type long!
	        WaitSpec.tv_sec  = wait_sec;
	        WaitSpec.tv_nsec = 0;
        #endif
	    
	    rc = EPS_POSIX_SEM_TIMEDWAIT(&eps_events.events[event_id].event_sema, &WaitSpec);
	}
	
    if  (rc == -1)
        // error occured (e.g. timeout)
    {
        retVal = LSA_RET_ERR_RESOURCE;
    }
	
	return retVal;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
