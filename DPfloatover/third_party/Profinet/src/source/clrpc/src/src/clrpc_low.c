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
/*  F i l e               &F: clrpc_low.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of CLRPC "Lower Done"                                     */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    2
#define LTRC_ACT_MODUL_ID  2

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


/*------------------------------------------------------------------------------
//	lower alloc and free
//----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_ll_alloc_rqb (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	LSA_OPCODE_TYPE				opcode,
	LSA_VOID_PTR_TYPE			user_id,
	CLRPC_LOWER_RQB_PTR_TYPE	*lower_rqb_ptr_ptr
	)
{
	CLRPC_LOG_FCT("clrpc_ll_alloc_rqb")
	LSA_UINT16 rc;
    CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();
    
#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("VV: drop user_id and create new function clrpc_ll_request (.., user_id, ....)")
/* see clrpc_sock_alloc_rqb() below */
#endif /* CLRPC_MESSAGE */

	CLRPC_ASSERT(channel);
	CLRPC_ASSERT(lower_rqb_ptr_ptr);
    CLRPC_ASSERT(clrpc_system_channel_is_ready() || CLRPC_PTR_ARE_EQUAL(channel, & clrpc_channels[0]));

	/* alloc the RQB */

	CLRPC_ALLOC_LOWER_RQB (CLRPC_CAST_LOWER_RQB_PTR_PTR (lower_rqb_ptr_ptr)
		, clrpc_null_user_id, sizeof(CLRPC_LOWER_RQB_TYPE), sys_channel->sysptr
		);

	if (CLRPC_PTR_IS_NULL (*lower_rqb_ptr_ptr)) {
		CLRPC_LOWER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cannot alloc RQB, sizeof(%u)"
			, sizeof(CLRPC_LOWER_RQB_TYPE)
			);
		return;
	}

#if CLRPC_DEBUG
	CLRPC_LL_SET_NEXT_RQB_PTR(*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
	CLRPC_LL_SET_PREV_RQB_PTR(*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif

	CLRPC_LL_SET_OPCODE (*lower_rqb_ptr_ptr, opcode);
	CLRPC_LL_SET_USER_ID_PTR (*lower_rqb_ptr_ptr, user_id);

	/* alloc the buffer and attach them to the RQB */

	if (opcode == SOCK_OPC_UDP_SEND || opcode == SOCK_OPC_UDP_RECEIVE) {
		SOCK_UPPER_MEM_PTR_TYPE  mem;

		CLRPC_ALLOC_LOWER_MEM (CLRPC_CAST_LOWER_MEM_PTR_PTR (& mem)
			, clrpc_null_user_id, CLRPC_BUFFER_SIZE, sys_channel->sysptr
			);

		if (CLRPC_PTR_IS_NULL (mem)) {
			CLRPC_LOWER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cannot alloc mem, sizeof(%u)"
				, CLRPC_BUFFER_SIZE
				);
			CLRPC_FREE_LOWER_RQB (&rc, *lower_rqb_ptr_ptr, sys_channel->sysptr);
			CLRPC_ASSERT(rc == LSA_RET_OK); /* don't care in release version */
			*lower_rqb_ptr_ptr = LSA_NULL; /* report failure */
			return;
		}

		(*lower_rqb_ptr_ptr)->args.data.buffer_ptr = mem;
		(*lower_rqb_ptr_ptr)->args.data.buffer_length = CLRPC_BUFFER_SIZE; /* see alloc, above */

		if (opcode == SOCK_OPC_UDP_RECEIVE) {
			(*lower_rqb_ptr_ptr)->args.data.data_length = CLRPC_BUFFER_SIZE; /* sock need this !! */
		}

		(*lower_rqb_ptr_ptr)->args.data.offset = 0;

		(*lower_rqb_ptr_ptr)->args.data.flags = 0; /* sanity */
	}
}

/*----------------------------------------------------------------------------*/

CLRPC_LOWER_RQB_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sock_alloc_rqb (
	LSA_VOID
	)
{
	CLRPC_LOG_FCT("clrpc_sock_alloc_rqb")
	CLRPC_CHANNEL_PTR_TYPE  channel = clrpc_system_channel();
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr;

	CLRPC_ALLOC_LOWER_RQB (CLRPC_CAST_LOWER_RQB_PTR_PTR (& lower_rqb_ptr)
		, clrpc_null_user_id, sizeof(CLRPC_LOWER_RQB_TYPE), channel->sysptr
		);

	if (CLRPC_PTR_IS_NULL (lower_rqb_ptr))
	{
		CLRPC_LOWER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cannot alloc RQB, sizeof(%u)", sizeof(CLRPC_LOWER_RQB_TYPE)
			);
		return LSA_NULL;
	}

#if CLRPC_DEBUG
	CLRPC_LL_SET_NEXT_RQB_PTR(lower_rqb_ptr, LSA_NULL); /* needed by clrpc_queue_enqueue() */
	CLRPC_LL_SET_PREV_RQB_PTR(lower_rqb_ptr, LSA_NULL); /* needed by clrpc_queue_enqueue() */
#endif

	return lower_rqb_ptr;
}

/*----------------------------------------------------------------------------*/

CLRPC_LOWER_RQB_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sock_alloc_rxtx_rqb (
	LSA_VOID
	)
{
	CLRPC_LOG_FCT("clrpc_sock_alloc_rxtx_rqb")
	CLRPC_CHANNEL_PTR_TYPE  channel = clrpc_system_channel();
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr;
	SOCK_UPPER_MEM_PTR_TYPE  mem;
	LSA_UINT16  rc;

	CLRPC_ALLOC_LOWER_RQB (CLRPC_CAST_LOWER_RQB_PTR_PTR (& lower_rqb_ptr)
		, clrpc_null_user_id, sizeof(CLRPC_LOWER_RQB_TYPE), channel->sysptr
		);

	if (CLRPC_PTR_IS_NULL (lower_rqb_ptr))
	{
		CLRPC_LOWER_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "cannot alloc RQB, sizeof(%u)", sizeof(CLRPC_LOWER_RQB_TYPE)
			);
		return LSA_NULL;
	}

	/* alloc the buffer and attach them to the RQB */

	CLRPC_ALLOC_LOWER_MEM (CLRPC_CAST_LOWER_MEM_PTR_PTR (& mem)
		, clrpc_null_user_id, CLRPC_BUFFER_SIZE, channel->sysptr
		);

	if (CLRPC_PTR_IS_NULL (mem)) {
		CLRPC_LOWER_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "cannot alloc mem, sizeof(%u)", CLRPC_BUFFER_SIZE
			);
		CLRPC_FREE_LOWER_RQB (&rc, lower_rqb_ptr, channel->sysptr);
		CLRPC_ASSERT(rc == LSA_RET_OK); /* don't care in release version */
		return LSA_NULL;
	}

	lower_rqb_ptr->args.data.buffer_ptr = mem;
	lower_rqb_ptr->args.data.buffer_length = CLRPC_BUFFER_SIZE; /* see alloc, above */

	lower_rqb_ptr->args.data.data_length = CLRPC_BUFFER_SIZE; /* UDP_RECEICE: sock need this !! */

	lower_rqb_ptr->args.data.offset = 0;
	lower_rqb_ptr->args.data.flags = 0; /* sanity */


	CLRPC_LL_SET_NEXT_RQB_PTR(lower_rqb_ptr, LSA_NULL); /* needed by clrpc_queue_enqueue() */
	CLRPC_LL_SET_PREV_RQB_PTR(lower_rqb_ptr, LSA_NULL); /* needed by clrpc_queue_enqueue() */

	CLRPC_LL_SET_OPCODE (lower_rqb_ptr, SOCK_OPC_UDP_SEND); /* see clrpc_ll_free_rqb() */
	
	return lower_rqb_ptr;
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_ll_free_rqb (
	CLRPC_CHANNEL_PTR_TYPE  channel,
    CLRPC_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr
	)
{
	LSA_UINT16	rc;
    CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();

	LSA_UNUSED_ARG(channel);
	CLRPC_ASSERT(lower_rqb_ptr);
	CLRPC_ASSERT(*lower_rqb_ptr);

	if (CLRPC_LL_GET_OPCODE (*lower_rqb_ptr) == SOCK_OPC_UDP_SEND
	|| CLRPC_LL_GET_OPCODE (*lower_rqb_ptr) == SOCK_OPC_UDP_RECEIVE)
	{
		CLRPC_FREE_LOWER_MEM (&rc, (*lower_rqb_ptr)->args.data.buffer_ptr, sys_channel->sysptr);
		CLRPC_ASSERT(rc == LSA_RET_OK); /* don't care in release version */
	}

	CLRPC_FREE_LOWER_RQB (&rc, *lower_rqb_ptr, sys_channel->sysptr);
	CLRPC_ASSERT(rc == LSA_RET_OK); /* don't care in release version */

	*lower_rqb_ptr = LSA_NULL; /* wipe out */
}


/*------------------------------------------------------------------------------
//	request lower
//----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_request_lower(
    CLRPC_LOWER_RQB_PTR_TYPE  lower,
    LSA_OPCODE_TYPE  opcode,
    LSA_VOID_PTR_TYPE  user_id
) {
	CLRPC_LOG_FCT("clrpc_request_lower")
	CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();

	CLRPC_ASSERT (CLRPC_ST_OPEN <= sys_channel->state  &&  sys_channel->state <= CLRPC_ST_CLOSE);
	CLRPC_ASSERT (sys_channel->detailptr->path_type == CLRPC_PATH_TYPE_SYSTEM);

	CLRPC_LL_SET_OPCODE      (lower, opcode);
	CLRPC_LL_SET_HANDLE      (lower, sys_channel->handle_lower);
	CLRPC_LL_SET_USER_ID_PTR (lower, user_id);

	CLRPC_REQUEST_LOWER (lower, sys_channel->sysptr);
}


/*------------------------------------------------------------------------------
//	request lower done
//----------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOWER_IN_FCT_ATTR
clrpc_request_lower_done(
    CLRPC_LOWER_RQB_PTR_TYPE  lower
) {
	CLRPC_LOG_FCT("clrpc_request_lower_done")
	CLRPC_CHANNEL_PTR_TYPE channel;

	if( CLRPC_PTR_IS_NULL(lower) ) {
		CLRPC_LOWER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no RQB"
			);
		CLRPC_LOWER_RQB_ERROR(lower); /* may return!? */
		return;
	}

#if CLRPC_DEBUG
	CLRPC_LL_SET_NEXT_RQB_PTR(lower, LSA_NULL); /* sanity */
	CLRPC_LL_SET_PREV_RQB_PTR(lower, LSA_NULL); /* sanity */
#endif

	if( ! (/*CLRPC_LL_GET_HANDLE(lower) >= 0 &&*/ CLRPC_LL_GET_HANDLE(lower) < CLRPC_CFG_MAX_CHANNELS) ) {
		CLRPC_LOWER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "wrong handle"
			);
		CLRPC_LOWER_RQB_ERROR(lower); /* may return!? */
		return;
	}

	channel = &clrpc_channels[CLRPC_LL_GET_HANDLE(lower)];

	CLRPC_LOWER_TRACE_04 (0, LSA_TRACE_LEVEL_CHAT
		, "<<< CLRPC_REQ_LOWER DONE --- lower(%08x) opc(%u) rsp(%u), user_id(%08x)"
		, lower, CLRPC_LL_GET_OPCODE(lower)
		, CLRPC_LL_GET_RESPONSE(lower), CLRPC_LL_GET_USER_ID_PTR (lower)
		);


	switch( CLRPC_LL_GET_OPCODE(lower) ) {
	/*-----------------------------------------------------------------------*/
	case SOCK_OPC_OPEN_CHANNEL:
		CLRPC_ASSERT(channel->state == CLRPC_ST_OPEN);
		CLRPC_ASSERT(channel->handle_lower == CLRPC_LL_INVALID_HANDLE); /* sanity */

		if( CLRPC_LL_GET_RESPONSE (lower) == SOCK_RSP_OK ) {
			channel->handle_lower = lower->args.channel.handle; /* store */
			clrpc_ll_free_rqb(channel, &lower);
			clrpc_channel_done(channel, CLRPC_OK);
		}
		else {
			clrpc_ll_free_rqb(channel, &lower);
			clrpc_channel_done(channel, CLRPC_ERR_LOWER_LAYER);
			/* nothing after done! */
		}
		break;

	/*-----------------------------------------------------------------------*/
	case SOCK_OPC_CLOSE_CHANNEL:
		CLRPC_ASSERT(channel->state == CLRPC_ST_CLOSE);

		if( CLRPC_LL_GET_RESPONSE (lower) == SOCK_RSP_OK ) {
			channel->handle_lower = CLRPC_LL_INVALID_HANDLE; /* sanity */
			clrpc_ll_free_rqb(channel, &lower);
			clrpc_channel_done(channel, CLRPC_OK);
			/* nothing after done! */
		}
		else {
			clrpc_ll_free_rqb(channel, &lower);
			clrpc_channel_done(channel, CLRPC_ERR_LOWER_LAYER);
		}
		break;

	/*-----------------------------------------------------------------------*/
	default:
		{
		LSA_VOID_PTR_TYPE user_ptr = CLRPC_LL_GET_USER_ID_PTR (lower);
		if( CLRPC_PTR_IS_NULL(user_ptr) )
		{
			CLRPC_FATAL1 (0);
			return;
		}
		/* dispatch to client or server respectively */
		CLRPC_DISPATCH(user_ptr, channel, user_ptr, lower);
		}
		break;
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
