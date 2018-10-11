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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_cmn.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  common functions                                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 14
#define SOCK_MODULE_ID    14

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*-----------------------------------------------------------------------------
// RQB-Queue
//---------------------------------------------------------------------------*/

LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_queue_init(
	SOCK_QUEUE_PTR_TYPE q
) {
	SockListInitialize(&q->anchor);

	/* assertions for sock_queue_enqueue() */

	SOCK_ASSERT( /* we queue upper and lower RQBs, thus the pointers must be of equal size */
		   sizeof(LSA_VOID_PTR_TYPE)        == sizeof(SOCK_LIST_ENTRY_PTR_TYPE)

		&& sizeof(SOCK_UPPER_RQB_PTR_TYPE) == sizeof(SOCK_LIST_ENTRY_PTR_TYPE)
		&& sizeof(SOCK_RQB_GET_NEXT_RQB_PTR((SOCK_UPPER_RQB_PTR_TYPE)0)) == sizeof(SOCK_LIST_ENTRY_PTR_TYPE)
		&& sizeof(SOCK_RQB_GET_PREV_RQB_PTR((SOCK_UPPER_RQB_PTR_TYPE)0)) == sizeof(SOCK_LIST_ENTRY_PTR_TYPE)
	);
}

/*---------------------------------------------------------------------------*/

LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_queue_enqueue(
	SOCK_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem /* upper or lower RQB */
) {
	SOCK_ASSERT( sock_is_null( ((SOCK_LIST_ENTRY_PTR_TYPE)elem)->Flink ));
	SOCK_ASSERT( sock_is_null( ((SOCK_LIST_ENTRY_PTR_TYPE)elem)->Blink ));
	SOCK_ASSERT( sock_is_not_null(elem));

	SockListInsertTail(&q->anchor, (SOCK_LIST_ENTRY_PTR_TYPE)elem);
}

/*---------------------------------------------------------------------------*/

SOCK_UPPER_RQB_PTR_TYPE SOCK_LOCAL_FCT_ATTR
sock_queue_dequeue(
	SOCK_QUEUE_PTR_TYPE q
) {
	SOCK_LIST_ENTRY_PTR_TYPE elem;

	if (SockListIsEmpty(&q->anchor)) {
		return LSA_NULL;
	}

	SockListRemoveHead(&q->anchor, elem, SOCK_LIST_ENTRY_PTR_TYPE);

#if SOCK_DEBUG
	elem->Flink = elem->Blink = LSA_NULL; /* sanity */
#endif

	return (SOCK_UPPER_RQB_PTR_TYPE)elem;
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_queue_empty(
	SOCK_QUEUE_PTR_TYPE q
) {
	return SockListIsEmpty(&q->anchor);
}

/*---------------------------------------------------------------------------*/

LSA_UINT16  SOCK_LOCAL_FCT_ATTR
sock_queue_not_empty(
	SOCK_QUEUE_PTR_TYPE q
) {
	return ! SockListIsEmpty(&q->anchor);
}

/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE  SOCK_LOCAL_FCT_ATTR
sock_queue_peek(
	SOCK_QUEUE_PTR_TYPE q
) {
	return SockListPeekHead(&q->anchor);
}

/*---------------------------------------------------------------------------*/

LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_queue_unlink (
	SOCK_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem
) {
#if SOCK_DEBUG
	{ /* check if in queue */
	SOCK_LIST_ENTRY_PTR_TYPE next;
	next = SockListFirst (& q->anchor, SOCK_LIST_ENTRY_PTR_TYPE);
	while (sock_is_not_null (next)) {
		if (LSA_HOST_PTR_ARE_EQUAL (next, elem)) {
			break;
		}
		next = SockListNext (& q->anchor, next, SOCK_LIST_ENTRY_PTR_TYPE);
	}
	SOCK_ASSERT (sock_is_not_null(next)); /* unlink a non linked */
	}
#else
	LSA_UNUSED_ARG(q);
#endif

	SockListRemoveEntry ((SOCK_LIST_ENTRY_PTR_TYPE)elem);

#if SOCK_DEBUG
	((SOCK_LIST_ENTRY_PTR_TYPE)elem)->Flink = ((SOCK_LIST_ENTRY_PTR_TYPE)elem)->Blink = LSA_NULL; /* sanity */
#endif
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
