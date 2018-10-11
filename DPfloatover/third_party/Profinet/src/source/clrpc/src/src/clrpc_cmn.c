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
/*  C o m p o n e n t     &C: CLRPC (ConnectionLess RPC)                :C&  */
/*                                                                           */
/*  F i l e               &F: clrpc_cmn.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Common things                                                            */
/*                                                                           */
/*  - alloc RQB for lower layer                                              */
/*  - queue routines                                                         */
/*  - doubly linked list routines                                            */
/*  - one-shot timer routines                                                */
/*  - hash routines                                                          */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    30
#define LTRC_ACT_MODUL_ID  30

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


/*-----------------------------------------------------------------------------
// RQB-Queue
//---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_queue_init(
	CLRPC_QUEUE_PTR_TYPE q
) {
	CLRPC_LIST_INITIALIZE(&q->anchor);

	/* assertions for clrpc_queue_enqueue() */

	CLRPC_ASSERT( /* we queue upper and lower RQBs, thus the pointers must be of equal size */
		   sizeof(LSA_VOID_PTR_TYPE)        == sizeof(CLRPC_LIST_ENTRY_PTR_TYPE)

		&& sizeof(CLRPC_UPPER_RQB_PTR_TYPE) == sizeof(CLRPC_LIST_ENTRY_PTR_TYPE)
		&& sizeof(CLRPC_RQB_GET_NEXT_RQB_PTR((CLRPC_UPPER_RQB_PTR_TYPE)0)) == sizeof(CLRPC_LIST_ENTRY_PTR_TYPE)
		&& sizeof(CLRPC_RQB_GET_PREV_RQB_PTR((CLRPC_UPPER_RQB_PTR_TYPE)0)) == sizeof(CLRPC_LIST_ENTRY_PTR_TYPE)

		&& sizeof(CLRPC_LOWER_RQB_PTR_TYPE) == sizeof(CLRPC_LIST_ENTRY_PTR_TYPE)
		&& sizeof(CLRPC_LL_GET_NEXT_RQB_PTR((CLRPC_LOWER_RQB_PTR_TYPE)0)) == sizeof(CLRPC_LIST_ENTRY_PTR_TYPE)
		&& sizeof(CLRPC_LL_GET_PREV_RQB_PTR((CLRPC_LOWER_RQB_PTR_TYPE)0)) == sizeof(CLRPC_LIST_ENTRY_PTR_TYPE)
	);
}

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_queue_enqueue(
	CLRPC_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem /* upper or lower RQB */
) {
	CLRPC_ASSERT( CLRPC_PTR_IS_NULL( ((CLRPC_LIST_ENTRY_PTR_TYPE)elem)->Flink ));
	CLRPC_ASSERT( CLRPC_PTR_IS_NULL( ((CLRPC_LIST_ENTRY_PTR_TYPE)elem)->Blink ));
	CLRPC_ASSERT( CLRPC_PTR_IS_NOT_NULL(elem));

	CLRPC_LIST_INSERT_TAIL(&q->anchor, (CLRPC_LIST_ENTRY_PTR_TYPE)elem);
}

/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_queue_dequeue(
	CLRPC_QUEUE_PTR_TYPE q
) {
	CLRPC_LIST_ENTRY_PTR_TYPE elem;

	if (CLRPC_LIST_IS_EMPTY(&q->anchor)) {
		return LSA_NULL;
	}

	CLRPC_LIST_REMOVE_HEAD(&q->anchor, elem, CLRPC_LIST_ENTRY_PTR_TYPE);

#if CLRPC_DEBUG
	elem->Flink = elem->Blink = LSA_NULL; /* sanity */
#endif

	return elem;
}

/*---------------------------------------------------------------------------*/

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_queue_not_empty(
	CLRPC_QUEUE_PTR_TYPE q
) {
	return ! CLRPC_LIST_IS_EMPTY(&q->anchor);
}

/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_queue_peek(
	CLRPC_QUEUE_PTR_TYPE q
) {
	return CLRPC_LIST_PEEK_HEAD(&q->anchor);
}

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_queue_unlink (
	CLRPC_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem
) {
#if CLRPC_DEBUG
	{ /* check if in queue */
	CLRPC_LIST_ENTRY_PTR_TYPE next;
	next = CLRPC_LIST_FIRST (& q->anchor, CLRPC_LIST_ENTRY_PTR_TYPE);
	while (CLRPC_PTR_IS_NOT_NULL (next)) {
		if (LSA_HOST_PTR_ARE_EQUAL (next, elem)) {
			break;
		}
		next = CLRPC_LIST_NEXT (& q->anchor, next, CLRPC_LIST_ENTRY_PTR_TYPE);
	}
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL(next)); /* unlink a non linked */
	}
#else
	LSA_UNUSED_ARG(q);
#endif

	CLRPC_LIST_REMOVE_ENTRY ((CLRPC_LIST_ENTRY_PTR_TYPE)elem);

#if CLRPC_DEBUG
	((CLRPC_LIST_ENTRY_PTR_TYPE)elem)->Flink = ((CLRPC_LIST_ENTRY_PTR_TYPE)elem)->Blink = LSA_NULL; /* sanity */
#endif
}


/*-----------------------------------------------------------------------------
// one-shot timer
//---------------------------------------------------------------------------*/


LSA_VOID clrpc_oneshot_sys_init(
	LSA_UINT16 time_base,
	LSA_UINT16 time_factor
) {
	CLRPC_LOG_FCT ("clrpc_oneshot_sys_init")
	LSA_UINT16 retval;

	CLRPC_LIST_INITIALIZE(&clrpc_oneshot.dlist);

	clrpc_oneshot.msec_per_tick = clrpc_oneshot.factor = time_factor;

	switch( time_base ) {
	case LSA_TIME_BASE_100MS:
		clrpc_oneshot.msec_per_tick *= 100;
		break;
	case LSA_TIME_BASE_10MS:
		clrpc_oneshot.msec_per_tick *= 10;
		break;
	case LSA_TIME_BASE_1MS:
		clrpc_oneshot.msec_per_tick *= 1;
		break;
	default:
		CLRPC_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL
			, "strange time_base(%u)"
			, time_base
			);
		CLRPC_FATAL1(time_base);
		break;
	}

	if( clrpc_oneshot.msec_per_tick > 500/*msec*/) {
		CLRPC_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL
			, "unusable milli-seconds per tick(%u)"
			, clrpc_oneshot.msec_per_tick
			);
		/*
		 * recommendation
		 *	CLRPC_INIT_TYPE.oneshot_lsa_time_base   = LSA_TIME_BASE_100MS
		 *	CLRPC_INIT_TYPE.oneshot_lsa_time_factor = 1 // or 2 or 3
		 */
		CLRPC_FATAL1(clrpc_oneshot.msec_per_tick);
		return;
	}

	clrpc_oneshot.timer_running = 0;

#if CLRPC_DEBUG
	CLRPC_RQB_SET_NEXT_RQB_PTR(&clrpc_oneshot.timerSyncRQB, LSA_NULL); /* sanity */
	CLRPC_RQB_SET_PREV_RQB_PTR(&clrpc_oneshot.timerSyncRQB, LSA_NULL); /* sanity */
#endif

	CLRPC_RQB_SET_HANDLE(&clrpc_oneshot.timerSyncRQB, CLRPC_INVALID_HANDLE); /* sanity */
	CLRPC_RQB_SET_OPCODE(&clrpc_oneshot.timerSyncRQB, CLRPC_OPC_TIMER);

	CLRPC_RQB_SET_RESPONSE (& clrpc_oneshot.timerSyncRQB, 0); /* "not-posted" */

	CLRPC_ALLOC_TIMER(&retval, &clrpc_oneshot.timer_id, LSA_TIMER_TYPE_CYCLIC, time_base);
	CLRPC_ASSERT(retval == LSA_RET_OK); /* what else? */

	clrpc_oneshot.user_id.uvar32 = 0;
}

/*---------------------------------------------------------------------------*/

LSA_VOID clrpc_oneshot_sys_undo_init(
	LSA_VOID
) {
	LSA_UINT16 retval;

	CLRPC_ASSERT(CLRPC_LIST_IS_EMPTY(&clrpc_oneshot.dlist));
	CLRPC_ASSERT(clrpc_oneshot.timer_running == 0);

	CLRPC_FREE_TIMER(&retval, clrpc_oneshot.timer_id);
	CLRPC_ASSERT(retval == LSA_RET_OK); /* what else? */
}

/*---------------------------------------------------------------------------*/

LSA_VOID clrpc_oneshot_init(
	CLRPC_ONESHOT_PTR_TYPE oneshot,
	CLRPC_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
) {
	CLRPC_LOG_FCT("clrpc_oneshot_init")

	CLRPC_ASSERT(oneshot);
	CLRPC_ASSERT(callback);

#if CLRPC_DEBUG
	oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

	oneshot->callback = callback;
	oneshot->user_ptr = user_ptr;

	oneshot->ticks = 0;
	oneshot->running = 0; /* not running */
}

/*---------------------------------------------------------------------------*/

LSA_VOID clrpc_oneshot_trigger(
	CLRPC_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
) {
	CLRPC_LOG_FCT("clrpc_oneshot_trigger")
	CLRPC_ONESHOT_PTR_TYPE curr;

	CLRPC_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
		, "timeout(%u msec), running(%u)"
		, timeout_msec
		, oneshot->running
		);

	/* unlink and adjust if running (retrigger) */

	if( oneshot->running ) { /* running */

		CLRPC_ONESHOT_PTR_TYPE next;

		next = CLRPC_LIST_NEXT(&clrpc_oneshot.dlist, &oneshot->dlink, CLRPC_ONESHOT_PTR_TYPE);

		CLRPC_LIST_REMOVE_ENTRY(&oneshot->dlink);

#if CLRPC_DEBUG
		oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

		if( CLRPC_PTR_IS_NOT_NULL(next) ) {
			next->ticks += oneshot->ticks; /* give the ticks to the successor */
		}
	}

	CLRPC_ASSERT(CLRPC_PTR_IS_NULL(oneshot->dlink.Flink) && CLRPC_PTR_IS_NULL(oneshot->dlink.Blink)); /* sanity check */

	/***/

	oneshot->ticks = 1 + timeout_msec / clrpc_oneshot.msec_per_tick; /* at least one tick */

	CLRPC_ASSERT(oneshot->ticks > 0); /* at least one tick, see clrpc_timeout() */

	/* traverse the list and insert where appropriate */

	curr = CLRPC_LIST_FIRST(&clrpc_oneshot.dlist, CLRPC_ONESHOT_PTR_TYPE);

	while( CLRPC_PTR_IS_NOT_NULL(curr) ) {

		if( oneshot->ticks < curr->ticks ) {
			/* expires before current */
			break;
		}

		CLRPC_ASSERT(oneshot->ticks >= curr->ticks);

		oneshot->ticks -= curr->ticks;

		curr = CLRPC_LIST_NEXT(&clrpc_oneshot.dlist, &curr->dlink, CLRPC_ONESHOT_PTR_TYPE);
	};

	/* insert before current */

	if( CLRPC_PTR_IS_NOT_NULL(curr) ) {

		CLRPC_ASSERT(curr->ticks >= oneshot->ticks);

		curr->ticks -= oneshot->ticks;

		CLRPC_LIST_INSERT_TAIL(&curr->dlink, &oneshot->dlink);
	}
	else {
		CLRPC_LIST_INSERT_TAIL(&clrpc_oneshot.dlist, &oneshot->dlink)
	}

	oneshot->running = 1;

	if( ! clrpc_oneshot.timer_running ) { /* start timer */
		LSA_UINT16 retval;

		CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "starting LSA timer"
			);

		clrpc_oneshot.user_id.uvar32 += 1;

		CLRPC_START_TIMER(&retval, clrpc_oneshot.timer_id, clrpc_oneshot.user_id, clrpc_oneshot.factor);

		CLRPC_ASSERT(retval == LSA_RET_OK); /* what else? */

		clrpc_oneshot.timer_running = 1;
	}
}

/*-Export-----------------------------------------------------------------------
//	func: clrpc_timeout
//	work: lsa-timer callback, post a timer RQB
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR
clrpc_timeout(
    LSA_UINT16        timer_id,
    LSA_USER_ID_TYPE  user_id
) {
	CLRPC_LOG_FCT("clrpc_timeout")
	CLRPC_ASSERT(clrpc_oneshot.timer_id == timer_id); /* we use only one timer */

	LSA_UNUSED_ARG(timer_id);

	/*CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "user-id(%u)...", user_id);*/

	if( ! clrpc_oneshot.timer_running || clrpc_oneshot.user_id.uvar32 != user_id.uvar32) {
		CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "stopped or old user_id, ignored"
			);
		return;
	}

	if( CLRPC_RQB_GET_RESPONSE (& clrpc_oneshot.timerSyncRQB) ) {
		CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "timerSyncRQB posted, this tick is left out (timer too fast?)"
			);
		return;
	}

	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(&clrpc_oneshot.timerSyncRQB) == CLRPC_OPC_TIMER);

	CLRPC_RQB_SET_RESPONSE (& clrpc_oneshot.timerSyncRQB, 1); /* mark as "is-posted" */

	CLRPC_REQUEST_LOCAL(&clrpc_oneshot.timerSyncRQB); /* no sys-ptr, because no channel! */
}

/*-Export-----------------------------------------------------------------------
//	func: clrpc_timeout_synchronized
//	work: synchronized (by timer RQB) timer callback
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_timeout_synchronized(
    CLRPC_UPPER_RQB_PTR_TYPE  rb
) {
	CLRPC_LOG_FCT("clrpc_timeout_synchronized")
	CLRPC_ONESHOT_PTR_TYPE oneshot;

	/*CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "...");*/

	CLRPC_ASSERT(rb == &clrpc_oneshot.timerSyncRQB);
	CLRPC_ASSERT(CLRPC_RQB_GET_RESPONSE (rb)); /* "is-posted" */

	/***/

	oneshot = CLRPC_LIST_FIRST(&clrpc_oneshot.dlist, CLRPC_ONESHOT_PTR_TYPE);

	if( CLRPC_PTR_IS_NOT_NULL(oneshot) && oneshot->ticks > 0 && --oneshot->ticks == 0 ) {

		/* fire */

		do {

			CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
				, "firing..."
				);

			CLRPC_LIST_REMOVE_HEAD(&clrpc_oneshot.dlist, oneshot, CLRPC_ONESHOT_PTR_TYPE);

#if CLRPC_DEBUG
			oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

			oneshot->running = 0;

			CLRPC_ASSERT(oneshot->ticks == 0);
			CLRPC_ASSERT(oneshot->callback);

			oneshot->callback(oneshot, oneshot->user_ptr);

			/*
			 * note: user may call clrpc_oneshot_fired(), clrpc_oneshot_trigger() or clrpc_oneshot_stop() from the callback
			 */

			oneshot = CLRPC_LIST_FIRST(&clrpc_oneshot.dlist, CLRPC_ONESHOT_PTR_TYPE); /* get the new head */

		} while( CLRPC_PTR_IS_NOT_NULL(oneshot) && oneshot->ticks == 0 );

		if( CLRPC_LIST_IS_EMPTY(&clrpc_oneshot.dlist) && clrpc_oneshot.timer_running ) {
			LSA_UINT16 retval;

			CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
				, "no more oneshots, stopping LSA timer"
				);

			clrpc_oneshot.timer_running = 0; /* reset first, then stop */

			CLRPC_STOP_TIMER(&retval, clrpc_oneshot.timer_id);

			CLRPC_ASSERT(retval == LSA_RET_OK);
		}
	}

	/***/

	CLRPC_RQB_SET_RESPONSE (rb, 0); /* "not-posted" */
}

/*---------------------------------------------------------------------------*/

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_oneshot_stop(
	CLRPC_ONESHOT_PTR_TYPE oneshot
) {
	CLRPC_LOG_FCT("clrpc_oneshot_stop")

	CLRPC_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "user_ptr(%08x) running(%u) ticks(%u)"
		, oneshot->user_ptr
		, oneshot->running
		, oneshot->ticks
		);

	/* unlink and adjust if running */

	if( oneshot->running ) { /* running */

		CLRPC_ONESHOT_PTR_TYPE next;

		next = CLRPC_LIST_NEXT(&clrpc_oneshot.dlist, &oneshot->dlink, CLRPC_ONESHOT_PTR_TYPE);

		CLRPC_LIST_REMOVE_ENTRY(&oneshot->dlink);

#if CLRPC_DEBUG
		oneshot->dlink.Flink = oneshot->dlink.Blink = LSA_NULL; /* sanity */
#endif

		if( CLRPC_PTR_IS_NOT_NULL(next) ) {
			next->ticks += oneshot->ticks; /* give the ticks to the successor */
		}

		/***/

		oneshot->running = 0;

		oneshot->ticks = 0; /* sanity */

		if( CLRPC_LIST_IS_EMPTY(&clrpc_oneshot.dlist) && clrpc_oneshot.timer_running ) {
			LSA_UINT16 retval;

			CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
				, "no more oneshots and LSA timer running, stopping LSA timer"
				);

			clrpc_oneshot.timer_running = 0; /* reset first, then stop */

			CLRPC_STOP_TIMER(&retval, clrpc_oneshot.timer_id);

			CLRPC_ASSERT(retval == LSA_RET_OK);
		}

		CLRPC_ASSERT(CLRPC_PTR_IS_NULL(oneshot->dlink.Flink) && CLRPC_PTR_IS_NULL(oneshot->dlink.Blink));

		return CLRPC_OK;
	}
	else {
		/*CLRPC_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "already fired");*/

		CLRPC_ASSERT(CLRPC_PTR_IS_NULL(oneshot->dlink.Flink) && CLRPC_PTR_IS_NULL(oneshot->dlink.Blink));

		return CLRPC_OK;
	}
}

/*---------------------------------------------------------------------------*/

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_oneshot_is_running(
	CLRPC_ONESHOT_PTR_TYPE oneshot
) {
	return oneshot->running;
}

/*------------------------------------------------------------------------------
// HASH functions
//----------------------------------------------------------------------------*/

static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_adjust (
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_HASH_ENTRY_PTR_TYPE entry
); /* forward */


LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_init (
	CLRPC_HASH_PTR_TYPE  hash
) {
	CLRPC_LOG_FCT("clrpc_hash_init")
	LSA_UINT16 i;

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(hash));

	for( i = 0; i < CLRPC_HASH_BUCKETS_MAX; ++i ) {

		CLRPC_LIST_INITIALIZE(&hash->bucket[i]);
	}

	CLRPC_LIST_INITIALIZE(&hash->iter_list);
}

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_undo_init (
	CLRPC_HASH_PTR_TYPE  hash
) {
	CLRPC_LOG_FCT("clrpc_hash_undo_init")
	LSA_UINT16 i;

	LSA_UNUSED_ARG(hash);

	for( i = 0; i < CLRPC_HASH_BUCKETS_MAX; ++i ) {
		/* it is a bug, if not all entries already removed */
		CLRPC_ASSERT(CLRPC_LIST_IS_EMPTY(&hash->bucket[i]));
		/* nothing to do */
	}

	CLRPC_ASSERT(CLRPC_LIST_IS_EMPTY(&hash->iter_list));
}

/*---------------------------------------------------------------------------*/

CLRPC_HASH_ENTRY_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_find (
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_UUID_PTR_TYPE  uuid,
	LSA_UINT16 hint
) {
	CLRPC_LOG_FCT("clrpc_hash_find")
	CLRPC_LIST_ENTRY_PTR_TYPE bucket;
	CLRPC_HASH_ENTRY_PTR_TYPE entry;

	bucket = &hash->bucket[CLRPC_HASH_HINT_TO_INDEX(hint)];

	entry = CLRPC_LIST_FIRST(bucket, CLRPC_HASH_ENTRY_PTR_TYPE);

	while( CLRPC_PTR_IS_NOT_NULL(entry) ) {

		if( clrpc_uuid_equal(&entry->uuid, uuid) ) {
			return entry;
		}

		entry = CLRPC_LIST_NEXT(bucket, &entry->dlink, CLRPC_HASH_ENTRY_PTR_TYPE);
	};

	return LSA_NULL;
}

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_entry_setup ( /* prepare this entry for adding */
	CLRPC_HASH_ENTRY_PTR_TYPE  entry,
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_VOID_PTR_TYPE user_ptr
) {
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(entry));
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(user_ptr)); /* cannot store null in hash, see below */

#if CLRPC_DEBUG
	entry->dlink.Flink = entry->dlink.Blink = LSA_NULL; /* sanity */
#endif

	entry->uuid = *uuid;
	entry->user_ptr = user_ptr;
}

/*---------------------------------------------------------------------------*/

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_add (
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_HASH_ENTRY_PTR_TYPE entry /* uuid and user_ptr must be set (user_ptr NOT NULL!!!)*/
) {
	CLRPC_LOG_FCT("clrpc_hash_add")
	LSA_UINT16 hint;

	CLRPC_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "hash(%08x) entry(%08x) user_ptr(%08x)"
		, hash, entry, entry->user_ptr
		);

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(hash));
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(entry)); /* the entry is allocated/freed by the user */

	if( CLRPC_PTR_IS_NULL(entry->user_ptr) ) {
		CLRPC_FATAL1(0); /* or lookup/iterate does not what you meant!!! */
	}

	CLRPC_ASSERT(CLRPC_PTR_IS_NULL(entry->dlink.Flink)); /* sanity */
	CLRPC_ASSERT(CLRPC_PTR_IS_NULL(entry->dlink.Blink)); /* sanity */

#if 0 /* CLRPC_NO_HINT is wrong (2005.06.06) */
	CLRPC_ASSERT(CLRPC_PTR_IS_NULL(clrpc_hash_find(hash, &entry->uuid, CLRPC_NO_HINT))); /* never twice! fatal!!! */
#endif

	hint = clrpc_uuid_to_hash(&entry->uuid);

	if( CLRPC_PTR_IS_NOT_NULL(clrpc_hash_find(hash, &entry->uuid, hint)) ) {
		CLRPC_FATAL1(hint); /* never twice! fatal!!! */
	}

	CLRPC_LIST_INSERT_TAIL(&hash->bucket[CLRPC_HASH_HINT_TO_INDEX(hint)], &entry->dlink);

	return hint; /* the hint */
}

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_remove (
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_UUID_PTR_TYPE uuid /* must exist or fatal */
) {
	CLRPC_LOG_FCT("clrpc_hash_remove")
	LSA_UINT16 hint;
	CLRPC_HASH_ENTRY_PTR_TYPE entry;

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(hash));
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(uuid));

	hint = clrpc_uuid_to_hash(uuid);

	entry = clrpc_hash_find(hash, uuid, hint);

#if CLRPC_DEBUG
	if (CLRPC_PTR_IS_NULL (entry)) {
		LSA_UINT16  idx;
		for (idx = 0; idx < CLRPC_HASH_BUCKETS_MAX; ++idx) { /* see CLRPC_HASH_HINT_TO_INDEX */
			entry = clrpc_hash_find (hash, uuid, idx);
			if (CLRPC_PTR_IS_NOT_NULL (entry)) {
				CLRPC_FATAL1(idx); /* why found? */
			}
		}
	}
#endif

	if( CLRPC_PTR_IS_NULL(entry) ) {
		CLRPC_FATAL1(0); /* entry-to-be-removed must exist! */
	}

	clrpc_hash_iter_adjust(hash, entry); /* iterator may point to the entry-to-be-removed */

	CLRPC_LIST_REMOVE_ENTRY(&entry->dlink);

#if CLRPC_DEBUG
	entry->dlink.Flink = entry->dlink.Blink = LSA_NULL; /* sanity */
#endif
}

/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_lookup ( /* returns the user_ptr for convenience! */
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_UINT16  hint
) {
	CLRPC_LOG_FCT("clrpc_hash_lookup")
	CLRPC_HASH_ENTRY_PTR_TYPE entry;

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(hash));
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(uuid));

	/*
	 * OSF sources file COMIF.C line 1019 do a trickier lookup
	 * they code the hint as (bucket-index, entry-index)
	 *
	 * optimize in a later version
	 */

	hint = clrpc_uuid_to_hash(uuid); /* for now, ignore the supplied hint */

	entry = clrpc_hash_find(hash, uuid, hint);

	if( CLRPC_PTR_IS_NOT_NULL(entry) ) {
		return entry->user_ptr;
	}

	return LSA_NULL; /* not found */
}

/*----------------------------------------------------------------------------*/

#if CLRPC_DEBUG
LSA_UINT32  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_count_entries (
	CLRPC_HASH_PTR_TYPE  hash
) {
	CLRPC_LOG_FCT("clrpc_hash_count_entries")
	LSA_UINT32  cnt;
	LSA_UINT16  hint;
	CLRPC_LIST_ENTRY_PTR_TYPE  bucket;
	CLRPC_HASH_ENTRY_PTR_TYPE  entry;

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (hash));

	cnt = 0;

	for (hint = 0; hint < CLRPC_HASH_BUCKETS_MAX; ++hint) {
		bucket = &hash->bucket[hint];

		entry = CLRPC_LIST_FIRST(bucket, CLRPC_HASH_ENTRY_PTR_TYPE);

		while (CLRPC_PTR_IS_NOT_NULL (entry)) {
			cnt += 1;

			entry = CLRPC_LIST_NEXT(bucket, &entry->dlink, CLRPC_HASH_ENTRY_PTR_TYPE);
		}
	}

	CLRPC_UPPER_TRACE_02 (0, LSA_TRACE_LEVEL_CHAT
		, "hash(0x%x)  entries_count(%u)", hash, cnt
		);
	return cnt;
}
#endif


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_setup ( /* prepare this iterator for attaching */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
) {
#if CLRPC_DEBUG
	iter->dlink.Flink = iter->dlink.Blink = 0; /* sanity */
#else
	LSA_UNUSED_ARG(iter);
#endif
	/*nothing by now*/
}


/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_attach ( /* attach this iterator to the hash */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter,
	CLRPC_HASH_PTR_TYPE  hash
) {
	CLRPC_LOG_FCT("clrpc_hash_iter_attach")

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(iter));
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(hash));

#if CLRPC_DEBUG
	{ /* may attach only once */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter_;
	for(
		iter_ = CLRPC_LIST_FIRST(&hash->iter_list, CLRPC_HASH_ITERATOR_PTR_TYPE);
		CLRPC_PTR_IS_NOT_NULL(iter_);
		iter_ = CLRPC_LIST_NEXT(&hash->iter_list, &iter_->dlink, CLRPC_HASH_ITERATOR_PTR_TYPE)
	) {
		CLRPC_ASSERT(iter_ != iter);
	}
	}
#endif

	CLRPC_ASSERT(CLRPC_PTR_IS_NULL(iter->dlink.Flink)); /* sanity */
	CLRPC_ASSERT(CLRPC_PTR_IS_NULL(iter->dlink.Blink)); /* sanity */

	iter->hash = hash;
	iter->entry = LSA_NULL;
	iter->bucket = 0;
	CLRPC_LIST_INSERT_TAIL(&iter->hash->iter_list, &iter->dlink);
}


/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_detach ( /* detach this iterator from the hash */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
) {
	CLRPC_LOG_FCT("clrpc_hash_iter_detach")

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(iter));

#if CLRPC_DEBUG
	{ /* may detach only if exists */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter_;
	for(
		iter_ = CLRPC_LIST_FIRST(&iter->hash->iter_list, CLRPC_HASH_ITERATOR_PTR_TYPE);
		CLRPC_PTR_IS_NOT_NULL(iter_);
		iter_ = CLRPC_LIST_NEXT(&iter->hash->iter_list, &iter_->dlink, CLRPC_HASH_ITERATOR_PTR_TYPE)
	) {
		if( iter_ == iter ) break;
	}
	CLRPC_ASSERT(iter_ == iter);
	}
#endif

	CLRPC_LIST_REMOVE_ENTRY(&iter->dlink);

#if CLRPC_DEBUG
	iter->dlink.Flink = iter->dlink.Blink = LSA_NULL; /* sanity */
#endif
}


/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_reset (
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
) {
	CLRPC_LOG_FCT("clrpc_hash_iter_reset")
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(iter));

	iter->entry = LSA_NULL;

	for (iter->bucket = 0; iter->bucket < CLRPC_HASH_BUCKETS_MAX; ++iter->bucket) {

		iter->entry = CLRPC_LIST_FIRST (& iter->hash->bucket[iter->bucket], CLRPC_HASH_ENTRY_PTR_TYPE);

		if (CLRPC_PTR_IS_NOT_NULL (iter->entry)) break;
	}
}

/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_next ( /* returns the user_ptr for convenience! */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
) {
	CLRPC_LOG_FCT("clrpc_hash_iter_next")
	LSA_VOID_PTR_TYPE  ptr = LSA_NULL;

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(iter));

	if (CLRPC_PTR_IS_NOT_NULL (iter->entry)) { /* return this and prepare for next */

		ptr = iter->entry->user_ptr;

		iter->entry = CLRPC_LIST_NEXT (& iter->hash->bucket[iter->bucket], & iter->entry->dlink, CLRPC_HASH_ENTRY_PTR_TYPE);

		if (CLRPC_PTR_IS_NULL (iter->entry)) { /* next bucket */

			for (iter->bucket += 1; iter->bucket < CLRPC_HASH_BUCKETS_MAX; ++iter->bucket) {

				iter->entry = CLRPC_LIST_FIRST (& iter->hash->bucket[iter->bucket], CLRPC_HASH_ENTRY_PTR_TYPE);

				if (CLRPC_PTR_IS_NOT_NULL (iter->entry)) break;
			}
		}
	}

	return ptr;
}


/*---------------------------------------------------------------------------*/

static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_adjust (
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_HASH_ENTRY_PTR_TYPE entry
) {
	CLRPC_LOG_FCT("clrpc_hash_iter_adjust")
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter;

	for(
		iter = CLRPC_LIST_FIRST(&hash->iter_list, CLRPC_HASH_ITERATOR_PTR_TYPE);
		CLRPC_PTR_IS_NOT_NULL(iter);
		iter = CLRPC_LIST_NEXT(&hash->iter_list, &iter->dlink, CLRPC_HASH_ITERATOR_PTR_TYPE)
	) {
		if( iter->entry == entry ) { /* iterator points to the entry-to-be-removed */
			clrpc_hash_iter_next(iter);
		}
	}
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
