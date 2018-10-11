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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_tim.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Oneshot timer                                                            */
/*                                                                           */
/*  NOTE: copy/paste from CLRPC                                              */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	3
#define CM_MODULE_ID		3

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*-----------------------------------------------------------------------------
// one-shot timer
//---------------------------------------------------------------------------*/

static struct cm_oneshot_anchor_tag {
	CM_LIST_ENTRY_TYPE dlist; /* anchor of oneshot-list */
	LSA_UINT32 msec_per_tick;
	LSA_UINT16 factor;
	LSA_UINT16 timer_running;
	LSA_TIMER_ID_TYPE timer_id;
	LSA_USER_ID_TYPE user_id;
	CM_RQB_TYPE timerSyncRQB; /* 'response' is used as 'is-posted-flag', see cm_timeout() and cm_timeout_synchronized() */
} cm_oneshot;

/*---------------------------------------------------------------------------*/

LSA_VOID cm_oneshot_sys_init(
	LSA_UINT16 time_base,
	LSA_UINT16 time_factor
) {
	CM_LOG_FCT("cm_oneshot_sys_init")
	LSA_UINT16 retval;

	CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
		, "base(%u), factor(%u)", time_base, time_factor
		);

	CmListInitialize(&cm_oneshot.dlist);

	if (time_factor == 0) {
		CM_FATAL ();
		return;
	}

	cm_oneshot.msec_per_tick = cm_oneshot.factor = time_factor;

	switch( time_base ) {
	case LSA_TIME_BASE_1S:
		cm_oneshot.msec_per_tick *= 1000UL;
		break;
	case LSA_TIME_BASE_100MS:
		cm_oneshot.msec_per_tick *= 100UL;
		break;
	case LSA_TIME_BASE_10MS:
		cm_oneshot.msec_per_tick *= 10UL;
		break;
	case LSA_TIME_BASE_1MS:
		break;
	default:
		CM_FATAL(); /* not supported */
		break;
	}

	cm_oneshot.timer_running = 0;

#if CM_DEBUG
	CM_RQB_SET_NEXT_RQB_PTR(&cm_oneshot.timerSyncRQB, LSA_NULL); /* sanity */
	CM_RQB_SET_PREV_RQB_PTR(&cm_oneshot.timerSyncRQB, LSA_NULL); /* sanity */
#endif

	CM_RQB_SET_HANDLE(&cm_oneshot.timerSyncRQB, CM_INVALID_HANDLE); /* sanity */
	CM_RQB_SET_OPCODE(&cm_oneshot.timerSyncRQB, CM_OPC_TIMER);

	CM_RQB_SET_RESPONSE (& cm_oneshot.timerSyncRQB, 0); /* "not-posted" */

	CM_ALLOC_TIMER(&retval, &cm_oneshot.timer_id, LSA_TIMER_TYPE_CYCLIC, time_base);

	if( retval != LSA_RET_OK ) {
		CM_FATAL(); /* must configure more LSA-timers */
		return; /* unreachable... */
	}

	cm_oneshot.user_id.uvar32 = 0;
}

/*---------------------------------------------------------------------------*/

LSA_VOID cm_oneshot_sys_undo_init(
	LSA_VOID
) {
	CM_LOG_FCT("cm_oneshot_sys_undo_init")
	LSA_UINT16 retval;

	CM_ASSERT(CmListIsEmpty(&cm_oneshot.dlist));
	CM_ASSERT(cm_oneshot.timer_running == 0);

	CM_FREE_TIMER(&retval, cm_oneshot.timer_id);

	if( retval != LSA_RET_OK ) {
		CM_FATAL(); /* could not allocate */
		return; /* unreachable... */
	}
}

/*---------------------------------------------------------------------------*/

LSA_VOID cm_oneshot_init(
	CM_ONESHOT_PTR_TYPE oneshot,
	CM_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
) {
	CM_LOG_FCT("cm_oneshot_init")

	CM_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "user_ptr(%08x)", user_ptr
		);

	CM_ASSERT(oneshot);
	CM_ASSERT(callback);

#if CM_DEBUG
	oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

	oneshot->callback = callback;
	oneshot->user_ptr = user_ptr;

	oneshot->ticks = 0;
	oneshot->running = 0; /* not running */
}

/*---------------------------------------------------------------------------*/

LSA_VOID cm_oneshot_trigger(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
) {
	CM_LOG_FCT("cm_oneshot_trigger")
	CM_ONESHOT_PTR_TYPE curr;

	CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
		, "%u msec running(%u)"
		, timeout_msec
		, oneshot->running
		);

	/* unlink and adjust if running (retrigger) */

	if( oneshot->running ) { /* running */

		CM_ONESHOT_PTR_TYPE next;

		next = CmListNext(&cm_oneshot.dlist, &oneshot->dlink, CM_ONESHOT_PTR_TYPE);

		CmListRemoveEntry(&oneshot->dlink);

#if CM_DEBUG
		oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

		if( is_not_null(next) ) {
			next->ticks += oneshot->ticks; /* give the ticks to the successor */
		}
	}

	CM_ASSERT(is_null(oneshot->dlink.Flink) && is_null(oneshot->dlink.Blink)); /* sanity check */

	/***/

	oneshot->ticks = 1 + timeout_msec / cm_oneshot.msec_per_tick; /* at least one tick */

	CM_ASSERT(oneshot->ticks > 0); /* at least one tick, see cm_timeout() */

	/* traverse the list and insert where appropriate */

	curr = CmListFirst(&cm_oneshot.dlist, CM_ONESHOT_PTR_TYPE);

	while( is_not_null(curr) ) {

		if( oneshot->ticks < curr->ticks ) {
			/* expires before current */
			break;
		}

		CM_ASSERT(oneshot->ticks >= curr->ticks);

		oneshot->ticks -= curr->ticks;

		curr = CmListNext(&cm_oneshot.dlist, &curr->dlink, CM_ONESHOT_PTR_TYPE);
	};

	/* insert before current */

	if( is_not_null(curr) ) {

		CM_ASSERT(curr->ticks >= oneshot->ticks);

		curr->ticks -= oneshot->ticks;

		CmListInsertTail(&curr->dlink, &oneshot->dlink);
	}
	else {
		CmListInsertTail(&cm_oneshot.dlist, &oneshot->dlink)
	}

	oneshot->running = 1;

	if( ! cm_oneshot.timer_running ) { /* start timer */
		LSA_UINT16 retval;

		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "starting LSA timer"
			);

		cm_oneshot.user_id.uvar32 += 1;

		CM_START_TIMER(&retval, cm_oneshot.timer_id, cm_oneshot.user_id, cm_oneshot.factor);

		if( retval != LSA_RET_OK ) {
			CM_FATAL(); /* why did it fail? */
			return; /* unreachable... */
		}

		cm_oneshot.timer_running = 1;
	}
}

/*-Export-----------------------------------------------------------------------
//	func: cm_timeout
//	work: lsa-timer callback, post a timer RQB
//----------------------------------------------------------------------------*/
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR  cm_timeout(
	LSA_TIMER_ID_TYPE  timer_id,
	LSA_USER_ID_TYPE  user_id
) {
	CM_LOG_FCT("cm_timeout")
	CM_ASSERT(cm_oneshot.timer_id == timer_id); /* we use only one timer */

	LSA_UNUSED_ARG (timer_id); /* no warning */

	/*CM_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "user-id(%u)...", user_id
		);*/

	if( ! cm_oneshot.timer_running || cm_oneshot.user_id.uvar32 != user_id.uvar32) {
		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "stopped or old user_id, ignored"
			);
		return;
	}

	if( CM_RQB_GET_RESPONSE (& cm_oneshot.timerSyncRQB) ) {
#if 0 /* too noisy */
		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "timerSyncRQB posted, this tick is left out (timer too fast?)"
			);
#endif
		return;
	}

	CM_ASSERT(CM_RQB_GET_OPCODE(&cm_oneshot.timerSyncRQB) == CM_OPC_TIMER);

	CM_RQB_SET_RESPONSE (& cm_oneshot.timerSyncRQB, 1); /* mark as "is-posted" */

	CM_REQUEST_LOCAL(&cm_oneshot.timerSyncRQB); /* no sys-ptr, because no channel! */
}

/*-Export-----------------------------------------------------------------------
//	func: cm_timeout_synchronized
//	work: synchronized (by timer RQB) timer callback
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR  cm_timeout_synchronized(
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_timeout_synchronized")
	CM_ONESHOT_PTR_TYPE oneshot;

	/*CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
		, "..."
		);*/

	CM_ASSERT(rb == &cm_oneshot.timerSyncRQB);
	CM_ASSERT(CM_RQB_GET_RESPONSE (rb)); /* "is-posted" */

	/***/

	oneshot = CmListFirst(&cm_oneshot.dlist, CM_ONESHOT_PTR_TYPE);

	if( is_not_null(oneshot) && oneshot->ticks > 0 && --oneshot->ticks == 0 ) {

		/* fire */

		do {

			CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
				, "firing..."
				);

			CmListRemoveHead(&cm_oneshot.dlist, oneshot, CM_ONESHOT_PTR_TYPE);

#if CM_DEBUG
			oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

			oneshot->running = 0;

			CM_ASSERT(oneshot->ticks == 0);
			CM_ASSERT(oneshot->callback);

			oneshot->callback(oneshot, oneshot->user_ptr);

			/*
			 * note: user may call
			 *  cm_oneshot_fired(), cm_oneshot_trigger() or cm_oneshot_stop()
			 * from the callback
			 */

			oneshot = CmListFirst(&cm_oneshot.dlist, CM_ONESHOT_PTR_TYPE); /* get the new head */

		} while( is_not_null(oneshot) && oneshot->ticks == 0 );

		if( CmListIsEmpty(&cm_oneshot.dlist) && cm_oneshot.timer_running ) {
			LSA_UINT16 retval;

			CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
				, "no more oneshots, stopping LSA timer"
				);

			cm_oneshot.timer_running = 0; /* reset first, then stop */

			CM_STOP_TIMER(&retval, cm_oneshot.timer_id);

			CM_ASSERT(retval == LSA_RET_OK);
		}
	}

	/***/

	CM_RQB_SET_RESPONSE (rb, 0); /* "not-posted" */
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 cm_oneshot_stop(
	CM_ONESHOT_PTR_TYPE oneshot
) {
	CM_LOG_FCT("cm_oneshot_stop")

	CM_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "user_ptr(%08x) running(%u) ticks(%u)"
		, oneshot->user_ptr
		, oneshot->running
		, oneshot->ticks
	);

	/* unlink and adjust if running */

	if( oneshot->running ) { /* running */

		CM_ONESHOT_PTR_TYPE next;

		next = CmListNext(&cm_oneshot.dlist, &oneshot->dlink, CM_ONESHOT_PTR_TYPE);

		CmListRemoveEntry(&oneshot->dlink);

#if CM_DEBUG
		oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

		if( is_not_null(next) ) {
			next->ticks += oneshot->ticks; /* give the ticks to the successor */
		}

		/***/

		oneshot->running = 0;

		oneshot->ticks = 0; /* sanity */

		if( CmListIsEmpty(&cm_oneshot.dlist) && cm_oneshot.timer_running ) {
			LSA_UINT16 retval;

			CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
				, "no more oneshots and LSA timer running, stopping LSA timer"
				);

			cm_oneshot.timer_running = 0; /* reset first, then stop */

			CM_STOP_TIMER(&retval, cm_oneshot.timer_id);

			CM_ASSERT(retval == LSA_RET_OK);
		}

		CM_ASSERT(is_null(oneshot->dlink.Flink) && is_null(oneshot->dlink.Blink));

		return CM_OK;
	}
	else {

		/*CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "already fired"
			);*/

		CM_ASSERT(is_null(oneshot->dlink.Flink) && is_null(oneshot->dlink.Blink));

		return CM_OK;
	}
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 cm_oneshot_is_running(
	CM_ONESHOT_PTR_TYPE oneshot
) {
	CM_LOG_FCT("cm_oneshot_is_running")
	return oneshot->running;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
