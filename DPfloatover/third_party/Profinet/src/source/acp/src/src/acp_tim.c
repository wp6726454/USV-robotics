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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_tim.c                                 :F&  */
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
/*  NOTE: copy/paste from CM                                                 */
/*                                                                           */
/*****************************************************************************/
#define ACP_MODULE_ID      8
#define LTRC_ACT_MODUL_ID  8

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)


/*-----------------------------------------------------------------------------
// oneshot-timer
//---------------------------------------------------------------------------*/

/* statics may not visible within the debugger */
struct acp_oneshot_anchor_tag {
	ACP_LIST_ENTRY_TYPE		dlist; /* anchor of oneshot-list */
	LSA_UINT32				msec_per_tick;
	LSA_UINT16				factor;
	LSA_UINT16				timer_running;
	LSA_TIMER_ID_TYPE		timer_id;
	LSA_UINT16				tick_counter; /* debug */
	LSA_USER_ID_TYPE		user_id;
	ACP_RQB_TYPE			timerSyncRQB;
	/* 'response' is used as 'is-posted-flag', see acp_timeout() and acp_timeout_synchronized() */
} acp_oneshot;


/*---------------------------------------------------------------------------*/

LSA_VOID acp_oneshot_sys_init(
	LSA_UINT16  time_base,
	LSA_UINT16  time_factor
) {
	ACP_LOG_FCT("acp_oneshot_sys_init")
	LSA_UINT16 retval;

	ACP_UPPER_TRACE_02 (0, LSA_TRACE_LEVEL_CHAT
		, "base(%d), factor(%d)"
		, time_base, time_factor
		);

	AcpListInitialize(&acp_oneshot.dlist);

	if (time_factor == 0) {
		ACP_FATAL (time_factor);
		return;
	}

	acp_oneshot.msec_per_tick = time_factor;

	switch( time_base ) {
	case LSA_TIME_BASE_1S:
		acp_oneshot.msec_per_tick *= 10;
		/* fall through */
	case LSA_TIME_BASE_100MS:
		acp_oneshot.msec_per_tick *= 10;
		/* fall through */
	case LSA_TIME_BASE_10MS:
		acp_oneshot.msec_per_tick *= 10;
		/* fall through */
	case LSA_TIME_BASE_1MS:
		break;
	default:
		ACP_FATAL (time_base);
		return;
	}

	acp_oneshot.factor = time_factor;

	acp_oneshot.timer_running = 0;

	ACP_ALLOC_TIMER (&retval, &acp_oneshot.timer_id, LSA_TIMER_TYPE_CYCLIC, time_base);
	if (retval != LSA_RET_OK) { /* what else? */
		ACP_FATAL (retval);
	}

	acp_oneshot.tick_counter = 0;

	acp_oneshot.user_id.uvar32 = 0;

#if ACP_DEBUG
	ACP_RQB_SET_NEXT_RQB_PTR(&acp_oneshot.timerSyncRQB, LSA_NULL); /* sanity */
	ACP_RQB_SET_PREV_RQB_PTR(&acp_oneshot.timerSyncRQB, LSA_NULL); /* sanity */
#endif

	ACP_RQB_SET_HANDLE(&acp_oneshot.timerSyncRQB, ACP_INVALID_HANDLE); /* sanity */
	ACP_RQB_SET_OPCODE(&acp_oneshot.timerSyncRQB, ACP_OPC_INTERNAL_TIMER);

	ACP_RQB_SET_RESPONSE (& acp_oneshot.timerSyncRQB, 0); /* "not-posted" */

}

/*---------------------------------------------------------------------------*/

LSA_VOID acp_oneshot_sys_undo_init(
	LSA_VOID
) {
	ACP_LOG_FCT("acp_oneshot_sys_undo_init")
	LSA_UINT16 retval;


	ACP_ASSERT(AcpListIsEmpty(&acp_oneshot.dlist));
	ACP_ASSERT(acp_oneshot.timer_running == 0);

	ACP_FREE_TIMER(&retval, acp_oneshot.timer_id);
	if (retval != LSA_RET_OK) { /* what else? */
		ACP_FATAL (retval);
	}
}

/*---------------------------------------------------------------------------*/

LSA_VOID acp_oneshot_init(
	ACP_ONESHOT_PTR_TYPE oneshot,
	ACP_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
) {
	ACP_LOG_FCT("acp_oneshot_init")

	ACP_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
		, "user_ptr(%08x)"
		, user_ptr
		);

	ACP_ASSERT(oneshot);
	ACP_ASSERT(callback);

#if ACP_DEBUG
	oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

	oneshot->callback = callback;
	oneshot->user_ptr = user_ptr;

	oneshot->ticks = 0;
	oneshot->running = 0; /* not running */
}

/*---------------------------------------------------------------------------*/

LSA_VOID acp_oneshot_trigger(
	ACP_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
) {
	ACP_LOG_FCT("acp_oneshot_trigger")
	ACP_ONESHOT_PTR_TYPE curr;


	if (oneshot->running) {
		ACP_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
			, "%d msec (retriggering)"
			, timeout_msec
			);
	}
	else {
		ACP_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
			, "%d msec (starting)"
			, timeout_msec
			);
	}

	/* unlink and adjust if running (retrigger) */

	if( oneshot->running ) { /* running */

		ACP_ONESHOT_PTR_TYPE next;

		next = AcpListIterNext(&acp_oneshot.dlist, &oneshot->dlink, ACP_ONESHOT_PTR_TYPE);

		AcpListRemoveEntry(&oneshot->dlink);

#if ACP_DEBUG
		oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

		if( is_not_null(next) ) {
			next->ticks += oneshot->ticks; /* give the ticks to the successor */
		}
	}

	ACP_ASSERT(is_null(oneshot->dlink.Flink) && is_null(oneshot->dlink.Blink)); /* sanity check */

	/***/

	oneshot->ticks = 1 + timeout_msec / acp_oneshot.msec_per_tick; /* at least one tick */

	ACP_ASSERT(oneshot->ticks > 0); /* at least one tick, see acp_timeout() */

	/* traverse the list and insert where appropriate */

	curr = AcpListIterFirst(&acp_oneshot.dlist, ACP_ONESHOT_PTR_TYPE);

	while( is_not_null(curr) ) {

		if( oneshot->ticks < curr->ticks ) {
			/* expires before current */
			break;
		}

		ACP_ASSERT(oneshot->ticks >= curr->ticks);

		oneshot->ticks -= curr->ticks;

		curr = AcpListIterNext(&acp_oneshot.dlist, &curr->dlink, ACP_ONESHOT_PTR_TYPE);
	};

	/* insert before current */

	if( is_not_null(curr) ) {
		ACP_ASSERT(curr->ticks >= oneshot->ticks);
		curr->ticks -= oneshot->ticks;

		AcpListInsertTail(&curr->dlink, &oneshot->dlink);
	}
	else {
		AcpListInsertTail(&acp_oneshot.dlist, &oneshot->dlink)
	}

	oneshot->running = 1;

	if( ! acp_oneshot.timer_running ) { /* start timer */
		LSA_UINT16 retval;

		ACP_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT
			, "starting LSA timer"
			);

		acp_oneshot.user_id.uvar32 += 1;

		ACP_START_TIMER(&retval, acp_oneshot.timer_id, acp_oneshot.user_id, acp_oneshot.factor);

		if (retval != LSA_RET_OK) { /* what else? */
			ACP_FATAL (retval);
		}

		acp_oneshot.timer_running = 1;
	}
}

/*------------------------------------------------------------------------------
//	func: acp_timeout
//	work: lsa-timer callback, post a timer RQB
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_timeout(
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE  user_id
) {
	ACP_LOG_FCT("acp_timeout")
	ACP_ASSERT(acp_oneshot.timer_id == timer_id); /* we use only one timer */

	LSA_UNUSED_ARG (timer_id);

#if ACP_DEBUG
	acp_oneshot.tick_counter += 1;
#endif

	/* too mutch log:
	ACP_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
		, "user-id(%d)..."
		, user_id
		);
	*/

	if( ! acp_oneshot.timer_running || acp_oneshot.user_id.uvar32 != user_id.uvar32) {
		ACP_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT
			, "stopped or old user_id, ignored"
			);
		return;
	}

	if( ACP_RQB_GET_RESPONSE (& acp_oneshot.timerSyncRQB) ) {
		/*too much log:
		ACP_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT
			, "timerSyncRQB posted, this tick is left out (timer too fast?)"
			);
		*/
		return;
	}

	ACP_ASSERT(ACP_RQB_GET_OPCODE(&acp_oneshot.timerSyncRQB) == ACP_OPC_INTERNAL_TIMER);

	ACP_RQB_SET_RESPONSE (& acp_oneshot.timerSyncRQB, 1); /* mark as "is-posted" */

	ACP_REQUEST_LOCAL(&acp_oneshot.timerSyncRQB); /* no sys-ptr, because no channel! */
}

/*------------------------------------------------------------------------------
//	func: acp_timeout_synchronized
//	work: synchronized (by timer RQB) timer callback
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR  acp_timeout_synchronized(
    ACP_UPPER_RQB_PTR_TYPE  rb
) {
	ACP_LOG_FCT("acp_timeout_synchronized")
	ACP_ONESHOT_PTR_TYPE oneshot;

	ACP_ASSERT(rb == &acp_oneshot.timerSyncRQB);
	ACP_ASSERT(ACP_RQB_GET_RESPONSE (rb)); /* "is-posted" */

	/***/

	oneshot = AcpListIterFirst(&acp_oneshot.dlist, ACP_ONESHOT_PTR_TYPE);

	if( is_not_null(oneshot) && oneshot->ticks > 0 && --oneshot->ticks == 0 ) {

		/* fire */

		do {

			/*
			ACP_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT
				, "firing..."
				);
			*/

			AcpListRemoveHead(&acp_oneshot.dlist, oneshot, ACP_ONESHOT_PTR_TYPE);

#if ACP_DEBUG
			oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

			oneshot->running = 0;

			ACP_ASSERT(oneshot->ticks == 0);
			ACP_ASSERT(oneshot->callback);

			oneshot->callback(oneshot, oneshot->user_ptr);

			/*
			 * note: user may call acp_oneshot_fired(), _trigger() or _stop() from the callback
			 */

			oneshot = AcpListIterFirst(&acp_oneshot.dlist, ACP_ONESHOT_PTR_TYPE); /* get the new head */

		} while( is_not_null(oneshot) && oneshot->ticks == 0 );

		if( AcpListIsEmpty(&acp_oneshot.dlist) && acp_oneshot.timer_running ) {
			LSA_UINT16 retval;

			ACP_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT
				, "no more oneshots, stopping LSA timer"
				);

			acp_oneshot.timer_running = 0; /* reset first, then stop */

			ACP_STOP_TIMER(&retval, acp_oneshot.timer_id);

			ACP_ASSERT(retval == LSA_RET_OK);
		}
	}

	/***/

	ACP_RQB_SET_RESPONSE (rb, 0); /* "not-posted" */
}


/*---------------------------------------------------------------------------*/

LSA_UINT16 acp_oneshot_stop(
	ACP_ONESHOT_PTR_TYPE oneshot
) {
	ACP_LOG_FCT("acp_oneshot_stop")

	ACP_UPPER_TRACE_03 (0, LSA_TRACE_LEVEL_CHAT
		, "user_ptr(%08x) running(%d) ticks(%d)"
		, oneshot->user_ptr, oneshot->running, oneshot->ticks
		);

	/* unlink and adjust if running */

	if( oneshot->running ) { /* running */

		ACP_ONESHOT_PTR_TYPE next;

		next = AcpListIterNext(&acp_oneshot.dlist, &oneshot->dlink, ACP_ONESHOT_PTR_TYPE);

		AcpListRemoveEntry(&oneshot->dlink);

#if ACP_DEBUG
		oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

		if( is_not_null(next) ) {
			next->ticks += oneshot->ticks; /* give the ticks to the successor */
		}

		/***/

		oneshot->running = 0;

		oneshot->ticks = 0; /* sanity */

		if( AcpListIsEmpty(&acp_oneshot.dlist) && acp_oneshot.timer_running ) {
			LSA_UINT16 retval;

			ACP_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT
				, "no more oneshots and LSA timer running, stopping LSA timer"
				);

			acp_oneshot.timer_running = 0; /* reset first, then stop */

			ACP_STOP_TIMER(&retval, acp_oneshot.timer_id);

			ACP_ASSERT(retval == LSA_RET_OK);
		}

		ACP_ASSERT(is_null(oneshot->dlink.Flink) && is_null(oneshot->dlink.Blink));

		return ACP_OK;
	}
	else {
		/*
		ACP_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT
			, "already fired"
			);
		*/

		ACP_ASSERT(is_null(oneshot->dlink.Flink) && is_null(oneshot->dlink.Blink));

		return ACP_OK;
	}
}


/*---------------------------------------------------------------------------*/

LSA_UINT16 acp_oneshot_is_running(
	ACP_ONESHOT_PTR_TYPE oneshot
) {
	ACP_LOG_FCT("acp_oneshot_is_running")
	return oneshot->running;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
