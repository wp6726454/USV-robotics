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
/*  F i l e               &F: clrpc_usr.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the LSA user-interface                                        */
/*                                                                           */
/*  - helper for callback                                                    */
/*  - channel open/close done callback                                       */
/*  - open channel                                                           */
/*  - close channel                                                          */
/*  - request                                                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3
#define CLRPC_MODULE_ID   LTRC_ACT_MODUL_ID

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


/*---------------------------------------------------------------------------*/

CLRPC_CHANNEL_TYPE	clrpc_channels[CLRPC_CFG_MAX_CHANNELS]; /* the channel table */

/*----------------------------------------------------------------------------*/

CLRPC_CHANNEL_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_system_channel(LSA_VOID)
{
	CLRPC_CHANNEL_PTR_TYPE  sys_channel = & clrpc_channels[0];
	return sys_channel;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL  CLRPC_LOCAL_FCT_ATTR
clrpc_system_channel_is_ready(LSA_VOID)
{
	CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();

	if (sys_channel->state == CLRPC_ST_READY) {
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_channel_set_trace_idx (
	CLRPC_CHANNEL_PTR_TYPE channel,
	LSA_UINT32  trace_idx
) {
	if (channel->trace_idx == 0  &&  trace_idx != 0) {
		channel->trace_idx = trace_idx;
	}
}


/*------------------------------------------------------------------------------
//	func: clrpc_callback
//	work: sets the response of the given request-block and calls the
//        request-done-fct (given in CLRPC_OPC_OPEN_CHANNEL)
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_callback (
	CLRPC_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR *upper_ptr,
    LSA_UINT16 module, 
    LSA_INT line
) {
	CLRPC_LOG_FCT("clrpc_callback")
	CLRPC_UPPER_RQB_PTR_TYPE  upper;

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (upper_ptr));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (*upper_ptr));

	upper = *upper_ptr;
	*upper_ptr = LSA_NULL;

	CLRPC_RQB_SET_HANDLE (upper, channel->handle_upper);

	CLRPC_RQB_SET_RESPONSE (upper, response);

	CLRPC_UPPER_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			"<<< rqb(0x%08x) h(%u) opc(%u) rsp(%u)  h-to(%u)in module(%d), line(%d)",
			upper, channel->my_handle, CLRPC_RQB_GET_OPCODE (upper),
			response, channel->handle_upper, 
            module, line
		);

    LSA_UNUSED_ARG(module);
    LSA_UNUSED_ARG(line);
	CLRPC_REQUEST_UPPER_DONE (channel->callback, upper, channel->sysptr);
}


/*------------------------------------------------------------------------------
//	func: clrpc_callback_and_free_channel
//	work: special case
//----------------------------------------------------------------------------*/

static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_callback_and_free_channel(
	CLRPC_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE rb
) {
	CLRPC_LOG_FCT("clrpc_callback_and_free_channel")
	LSA_UINT16  rc;

	/*
	 * NOTE: the previously used sequence was wrong!
	 *       > clrpc_callback(channel, CLRPC_OK, &rb);
	 *       > channel->state = CLRPC_ST_FREE;
	 *
	 * ==> even if the callback posts into a queue, a call
	 *     to undo_init() can occur (due to pre-emption)
	 *     before we have chance to reset the in-use flag
	 */

	CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE  callback = channel->callback;
	LSA_SYS_PTR_TYPE  sysptr = channel->sysptr;
	CLRPC_DETAIL_PTR_TYPE  detailptr = channel->detailptr;

	CLRPC_RQB_SET_HANDLE (rb, channel->handle_upper);
	CLRPC_RQB_SET_RESPONSE (rb, response);

	CLRPC_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			"<<< rqb(0x%08x) h(%u) opc(%u) rsp(%u)  h-to(%u)  [clrpc-channel-closed]",
			rb, channel->my_handle, CLRPC_RQB_GET_OPCODE (rb), response, channel->handle_upper
		);

#if CLRPC_DEBUG
	CLRPC_MEMSET(channel, 0xCC, sizeof(*channel)); /* sanity, wipe out */
#endif

	channel->state = CLRPC_ST_FREE; /* free channel */

	CLRPC_REQUEST_UPPER_DONE (callback, rb, sysptr); /* before calling back */

	CLRPC_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
	CLRPC_ASSERT (rc == LSA_RET_OK);
}

/*------------------------------------------------------------------------------
//	func: clrpc_channel_done
//	work: callback for client & server to notify completion of open/close channel
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_channel_done (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  response
	)
{
	CLRPC_LOG_FCT("clrpc_channel_done")

	if (response == CLRPC_OK) {
		CLRPC_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rsp(%u) state(%u)"
			, response, channel->state
			);
	}
	else {
		CLRPC_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rsp(%u) state(%u)"
			, response, channel->state
			);
	}

	switch (channel->state) {
/*------------------------------------------------------------------------------
// OPEN
//----------------------------------------------------------------------------*/
	case CLRPC_ST_OPEN:
		if (response != CLRPC_OK) {
			channel->state = CLRPC_ST_OPEN_ERROR;
			clrpc_channel_done (channel, CLRPC_OK); /* need CLRPC_OK, see case CLRPC_ST_OPEN_ERROR */
			return;
		}

		channel->state = CLRPC_ST_READY;
		CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE(channel->upper) == CLRPC_OPC_OPEN_CHANNEL);
		clrpc_callback (channel, CLRPC_OK, & channel->upper, CLRPC_MODULE_ID, __LINE__);
		return;

	case CLRPC_ST_OPEN_ERROR:
		CLRPC_ASSERT (response == CLRPC_OK); /* closing of the formerly opened channel must succeed */
#if CLRPC_CFG_MAX_CLIENTS
			/* nothing done after init, so how can it fail? */
			if( ! clrpc_cl_undo_init_channel (channel) ) {
				CLRPC_FATAL1 (0);
			}
#endif
#if CLRPC_CFG_MAX_SERVERS
			/* nothing done after init, so how can it fail? */
			if( ! clrpc_sv_undo_init_channel (channel) ) {
				CLRPC_FATAL1 (0);
			}
#endif
		CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE (channel->upper) == CLRPC_OPC_OPEN_CHANNEL);
		clrpc_callback_and_free_channel (channel, CLRPC_ERR_LOWER_LAYER, channel->upper);
		/* nothing after this!!! user may call clrpc_undo_init() */
		return;

/*------------------------------------------------------------------------------
// CLOSE
//----------------------------------------------------------------------------*/
	case CLRPC_ST_CLOSE:
		if( response != CLRPC_OK ) {
			CLRPC_FATAL1 (response);
		}
		clrpc_callback_and_free_channel(channel, CLRPC_OK, channel->upper);
		/* nothing after this!!! user may call clrpc_undo_init() */
		return;

	default:
		CLRPC_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unknown state(%u)", channel->state
			);
		CLRPC_FATAL1(channel->state); /* a bug */
		return;
	}
}


/*------------------------------------------------------------------------------
//	func: clrpc_open_channel
//	work: create a CLRPC instance and open a communication-channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_UPPER_IN_FCT_ATTR
clrpc_open_channel (
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	)
{
	CLRPC_LOG_FCT("clrpc_open_channel")
	CLRPC_UPPER_OPEN_CHANNEL_PTR_TYPE  open;
	LSA_HANDLE_TYPE  my_handle;
	CLRPC_CHANNEL_PTR_TYPE  channel;
	CLRPC_DETAIL_PTR_TYPE  detailptr;
	LSA_SYS_PTR_TYPE  sysptr;
	LSA_UINT16  rc;

	if (CLRPC_PTR_IS_NULL (rb)) {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no rqb"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

#if CLRPC_DEBUG
	CLRPC_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CLRPC_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	if (CLRPC_RQB_GET_OPCODE (rb) != CLRPC_OPC_OPEN_CHANNEL) {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "wrong opcode"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

	open = rb->args.channel.open; /* shortcut */

	if (CLRPC_PTR_IS_NULL (open)) {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no args"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

	if (open->clrpc_request_upper_done_ptr == 0) {/* note: must not use CLRPC_PTR_IS_NULL(),
											   compiler warning 揻unction/data pointer conversion in expression?(AP01382171 ) */
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no callback function"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

		/* get details and sys-pointer */

	detailptr = LSA_NULL;

	CLRPC_GET_PATH_INFO (&rc, &sysptr, &detailptr, open->sys_path);

	if (rc != LSA_RET_OK || CLRPC_PTR_IS_NULL (detailptr)) {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no sysptr / detailptr"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

		/* searching a free clrpc-handle */

	/* NOTE: must not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */
	CLRPC_ASSERT( ! (CLRPC_INVALID_HANDLE >= 0 && CLRPC_INVALID_HANDLE < CLRPC_CFG_MAX_CHANNELS));

    for (my_handle = 0; my_handle < CLRPC_CFG_MAX_CHANNELS; ++my_handle) 
    {
        if (clrpc_channels[my_handle].state == CLRPC_ST_FREE) 
        {
#if CLRPC_DEBUG
			//CLRPC_MEMSET(&clrpc_channels[my_handle], 0xCC, sizeof(clrpc_channels[my_handle])); /* to find uninitialized things */
			CLRPC_MEMSET(&clrpc_channels[my_handle], 0xCC, sizeof(clrpc_channels[0])); /* to find uninitialized things */
#endif
			clrpc_channels[my_handle].state = CLRPC_ST_IDLE; /* in use now */
			break;
		}
	}

    {
	LSA_BOOL  error_return = LSA_FALSE;

	if (my_handle >= CLRPC_CFG_MAX_CHANNELS)
	{
		CLRPC_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_ERROR
			, "no channel resource, see CLRPC_CFG_MAX_CHANNELS(%u)"
			, CLRPC_CFG_MAX_CHANNELS
			);
		error_return = LSA_TRUE;
	}
	else if (detailptr->path_type == CLRPC_PATH_TYPE_SYSTEM)
	{
		if (my_handle != 0)	/* see clrpc_system_channel() */
		{
			CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR
				, "system-channel must be the first opened channel"
				);
			error_return = LSA_TRUE;
		}
	}
	else if (detailptr->path_type == CLRPC_PATH_TYPE_USER)
	{
		if (! clrpc_system_channel_is_ready ())
		{
			CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR
				, "system-channel not ready yet, must be opened first"
				);
			error_return = LSA_TRUE;
		}
	}

	if (error_return)
	{
		CLRPC_RQB_SET_RESPONSE (rb, CLRPC_ERR_RESOURCE);
		CLRPC_RQB_SET_HANDLE (rb, open->handle_upper);
		CLRPC_REQUEST_UPPER_DONE (open->clrpc_request_upper_done_ptr, rb, sysptr);
		CLRPC_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
		CLRPC_ASSERT (rc == LSA_RET_OK);
		return;
	}
	}

	channel = & clrpc_channels[my_handle];

		/* use channel with index my_handle */

	CLRPC_ASSERT (channel->state == CLRPC_ST_IDLE);

	channel->my_handle		= my_handle;
	channel->handle_upper	= open->handle_upper;
	channel->callback		= open->clrpc_request_upper_done_ptr;

	channel->sysptr			= sysptr;
	channel->detailptr		= detailptr;
	
    clrpc_clear_interface_param_int(channel);
	clrpc_queue_init(&channel->tmp_rcv_queue);
    
	CLRPC_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "my_handle(%u)"
		, my_handle
		);

		/* set the result */
	open->handle = my_handle;

		/* keep rqb for callback */
	channel->upper = rb; /* see clrpc_channel_done() */

		/* initialize */
	channel->handle_lower = CLRPC_LL_INVALID_HANDLE;


		/* initialize client */
#if CLRPC_CFG_MAX_CLIENTS
	clrpc_cl_init_channel(channel);
#endif

		/* initialize server */
#if CLRPC_CFG_MAX_SERVERS
	clrpc_sv_init_channel(channel);
#endif

		/* the user channel is ready */
	if (channel->detailptr->path_type == CLRPC_PATH_TYPE_USER)
	{
		CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();

			/* needed for compatibility reason */
		channel->handle_lower = sys_channel->handle_lower;

		channel->state = CLRPC_ST_READY;
		clrpc_callback (channel, CLRPC_OK, & channel->upper, CLRPC_MODULE_ID, __LINE__);
		return;
	}

		/* the system channel must open the channel to the lower layer SOCK */
	else
	{
		CLRPC_LOWER_RQB_PTR_TYPE lower;

		channel->state = CLRPC_ST_OPEN;

		clrpc_ll_alloc_rqb (channel, SOCK_OPC_OPEN_CHANNEL, 0/*!*/, &lower);

		if (CLRPC_PTR_IS_NULL (lower)) {
			CLRPC_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "could not alloc"
				);
			clrpc_channel_done (channel, CLRPC_ERR_ALLOC_LOWER);
			/* nothing after this! user may call undo_init */
			return;
		}

		CLRPC_LL_SET_HANDLE (lower, CLRPC_INVALID_HANDLE); /* sanity */

		lower->args.channel.sock_request_upper_done_ptr = clrpc_request_lower_done;
		lower->args.channel.sys_path = open->sys_path;
		lower->args.channel.handle_upper = channel->my_handle;

		CLRPC_OPEN_CHANNEL_LOWER (lower, channel->sysptr);

		/* see clrpc_channel_done() */
	}
}

/*------------------------------------------------------------------------------
//	func: clrpc_close_channel_next_step
//	work: close the channel after closing the channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_close_channel_next_step(
    CLRPC_CHANNEL_PTR_TYPE channel
)
{
	CLRPC_LOG_FCT("clrpc_close_channel_next_step")
    CLRPC_LOWER_RQB_PTR_TYPE lower_rqb_ptr;
    
    /* free the tmp_rcv_ressources-Queue if not empty */
    while (clrpc_queue_not_empty(&channel->tmp_rcv_queue))
    {
        lower_rqb_ptr = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&channel->tmp_rcv_queue);
        CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower_rqb_ptr));
        
        clrpc_ll_free_rqb(channel, &lower_rqb_ptr);
    }
    
    if (clrpc_mac_addr_is_valid(channel->mac_addr))
    {
        clrpc_clear_interface_param_next_step(channel);
    }
#if CLRPC_CFG_MAX_CLIENTS
    else if (channel->cl.lower.state != CLRPC_CL_LOWER_CLOSED)
    {
        clrpc_clear_interface_param_next_step(channel);
    }
#endif
    else
    {
		/* close the channel to the lower layer */
	    if (channel->detailptr->path_type == CLRPC_PATH_TYPE_SYSTEM)
	    {
		    CLRPC_LOWER_RQB_PTR_TYPE lower;

		    clrpc_ll_alloc_rqb (channel, SOCK_OPC_CLOSE_CHANNEL, 0/*!*/, &lower);

            if (CLRPC_PTR_IS_NULL (lower)) 
            {
			    CLRPC_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				    , "could not clrpc_ll_alloc_rqb()"
				    );
			    CLRPC_FATAL1 (CLRPC_ERR_ALLOC_LOWER);
			    return;
		    }

            CLRPC_LL_SET_HANDLE (lower, channel->handle_lower);

		    CLRPC_CLOSE_CHANNEL_LOWER (lower, channel->sysptr);

		    /* see clrpc_channel_done() */
	    }
	    else
	    {
		    clrpc_channel_done (channel, CLRPC_OK);
	    }
    }
}
/*------------------------------------------------------------------------------
//	func: clrpc_close_channel
//	work: close the channel after closing the channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_UPPER_IN_FCT_ATTR
clrpc_close_channel (
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	)
{
	CLRPC_LOG_FCT("clrpc_close_channel")
	CLRPC_CHANNEL_PTR_TYPE channel;

	if (CLRPC_PTR_IS_NULL(rb)) {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no rqb"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

#if CLRPC_DEBUG
	CLRPC_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CLRPC_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

    if (CLRPC_RQB_GET_OPCODE (rb) != CLRPC_OPC_CLOSE_CHANNEL) 
    {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "wrong opcode"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

    if (/*CLRPC_RQB_GET_HANDLE (rb) < 0 ||*/ CLRPC_RQB_GET_HANDLE (rb) >= CLRPC_CFG_MAX_CHANNELS) 
    {
		CLRPC_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL
			, "wrong handle(%u)"
			, CLRPC_RQB_GET_HANDLE (rb)
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

    channel = &clrpc_channels[CLRPC_RQB_GET_HANDLE (rb)];

    if (channel->state != CLRPC_ST_READY) 
    { /* another state would be a bug in open */
		CLRPC_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "wrong opcode"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

    if (channel->detailptr->path_type == CLRPC_PATH_TYPE_SYSTEM)
	{
		LSA_UINT16  my_handle;
		for (my_handle = 0; my_handle < CLRPC_CFG_MAX_CHANNELS; ++my_handle)
		{
			if (channel->my_handle != my_handle)
			{
				if (clrpc_channels[my_handle].state != CLRPC_ST_FREE)
				{
					CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
						, "epm channel must be the last channel for close"
						);
					CLRPC_FATAL1 (my_handle);
					return;
				}
			}
		}
	}

		/* keep rqb for callback */
    CLRPC_ASSERT (CLRPC_PTR_IS_NULL (channel->upper));
	channel->upper = rb;

	channel->state = CLRPC_ST_CLOSE;

		/* un-initialize client */
#if CLRPC_CFG_MAX_CLIENTS
    if( ! clrpc_cl_undo_init_channel (channel) ) 
    {
		CLRPC_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "could not undo-init client"
			);
		CLRPC_FATAL1 (CLRPC_CFG_MAX_CLIENTS);
		return;
	}
#endif

		/* un-initialize server */
#if CLRPC_CFG_MAX_SERVERS
    if( ! clrpc_sv_undo_init_channel (channel) ) 
    {
		CLRPC_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "could not undo-init server"
			);
		CLRPC_FATAL1 (CLRPC_CFG_MAX_SERVERS);
		return;
	}
#endif

    clrpc_close_channel_next_step(channel);
}

/*------------------------------------------------------------------------------
//	func: clrpc_clear_interface_param_int
//	work: 
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_clear_interface_param_int (
	CLRPC_CHANNEL_PTR_TYPE      channel
)
{
    channel->trace_idx = 0;
	CLRPC_MEMSET(channel->mac_addr, 0, sizeof(channel->mac_addr));
    channel->mac_addr[0] = CLRPC_MAC_ADDR_INVALID;
#if CLRPC_CFG_MAX_CLIENTS
	channel->cl.cas_uuid = clrpc_uuid_nil_uuid;
#endif
}
/*------------------------------------------------------------------------------
//	func: clrpc_mac_addr_is_valid
//	work: checks the mac_addr
//----------------------------------------------------------------------------*/
LSA_BOOL CLRPC_LOCAL_FCT_ATTR
clrpc_mac_addr_is_valid (
	CLRPC_COMMON_MEM_U8_PTR_TYPE mac_addr
) 
{
    LSA_BOOL ret_val;
    
    if (mac_addr[0] == CLRPC_MAC_ADDR_INVALID)
    {
	    ret_val =  LSA_FALSE;
    }
    else
    {
        ret_val = LSA_TRUE;
    }
    return ret_val;
}

/*------------------------------------------------------------------------------
//	func: clrpc_alloc_rcv_resources
//	work: allocates the receive-resources for the client
//----------------------------------------------------------------------------*/
LSA_BOOL CLRPC_LOCAL_FCT_ATTR clrpc_alloc_rcv_resources(
	CLRPC_CHANNEL_PTR_TYPE      channel
)
{
	CLRPC_LOG_FCT("clrpc_alloc_rcv_resources")
    LSA_BOOL ret_val = LSA_TRUE;
    LSA_UINT16 nr_of_resources;
    CLRPC_LOWER_RQB_PTR_TYPE lower_rqb_ptr;
    
    
	nr_of_resources = channel->upper->args.channel.set_interface_param->client_receive_resources;

//	if( nr_of_resources < 1 ) 
//	{
//	    nr_of_resources = 1; /* at least one */
//	}
	CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "providing %u recv-resources", nr_of_resources
		);

    while( nr_of_resources-- ) 
    {
		CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "allocating a new recv-resource"
			);

		clrpc_ll_alloc_rqb(channel, SOCK_OPC_UDP_RECEIVE, 0/*!*/, &lower_rqb_ptr);

        if( CLRPC_PTR_IS_NULL(lower_rqb_ptr) ) {
			CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "could not alloc"
				);
            
            ret_val = LSA_FALSE;        
            break;
		}
        else
        {
            clrpc_queue_enqueue(&channel->tmp_rcv_queue, lower_rqb_ptr);
        }
	}
    
    if (!ret_val)
    {
        /* not all RQBs could be allocated, so free the allocated before error-return */
		CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "free all new recv-resources"
			);
        while (clrpc_queue_not_empty(&channel->tmp_rcv_queue))
        {
            lower_rqb_ptr = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&channel->tmp_rcv_queue);
            CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower_rqb_ptr));
            
            clrpc_ll_free_rqb(channel, &lower_rqb_ptr);
        }
    }
    
    return(ret_val);
}

/*------------------------------------------------------------------------------
//	func: clrpc_op_set_interface_param
//	work: sets interface parameters in channel data an opens socket for 
//        client functions if wanted
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR 
clrpc_op_set_interface_param(
	CLRPC_CHANNEL_PTR_TYPE      channel,
	CLRPC_UPPER_RQB_PTR_TYPE    rqb_ptr
)
{
	CLRPC_LOG_FCT("clrpc_op_set_interface_param")
    LSA_UINT16 response = CLRPC_OK_ACTIVE;
    
    CLRPC_ASSERT (CLRPC_PTR_IS_NULL (channel->upper));
    channel->upper = rqb_ptr;
    
    if (! clrpc_system_channel_is_ready ())
	{
		CLRPC_SV_TRACE_00 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "system-channel not ready yet"
			);
        response = CLRPC_ERR_SEQUENCE; 
	}
    else if (clrpc_mac_addr_is_valid(channel->mac_addr))
    {
		CLRPC_SV_TRACE_00 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "do not set interface parameters/mac_addr twice"
			);
        response = CLRPC_ERR_SEQUENCE;
    }
    else if ((rqb_ptr->args.channel.set_interface_param->mac_addr[0] & CLRPC_BROADCAST_ADDR) != 0)
    {
		CLRPC_SV_TRACE_00 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "broadcast mac_addr not allowed"
			);
        response = CLRPC_ERR_PARAM;
    }
    else
    {
        channel->trace_idx = rqb_ptr->args.channel.set_interface_param->trace_idx;
        
        if (rqb_ptr->args.channel.set_interface_param->client_receive_resources != 0)
        {
#if CLRPC_CFG_MAX_CLIENTS
            if (!clrpc_alloc_rcv_resources(channel))
            {
		        CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			        , "cannot alloc lower-RCV-RQB"
			        );

                clrpc_clear_interface_param_int(channel);
                response = CLRPC_ERR_ALLOC_LOWER;
            }
            else
            {
                /* clients are enabled */
                if (!clrpc_mac_addr_is_zero (rqb_ptr->args.channel.set_interface_param->mac_addr))
                {
		            clrpc_uuid_node_set(channel, rqb_ptr->args.channel.set_interface_param->mac_addr);
                }
            
                if( channel->cl.lower.state == CLRPC_CL_LOWER_CLOSED ) 
                {
	                CLRPC_LOWER_RQB_PTR_TYPE lower;

	                clrpc_ll_alloc_rqb(channel, SOCK_OPC_UDP_OPEN, 0/*!*/, &lower); /* NOTE: must be usable for close too! */

                    if( CLRPC_PTR_IS_NULL(lower) ) 
                    {
		                CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			                , "cannot alloc lower-RQB"
			                );

                        clrpc_clear_interface_param_int(channel);
                        response = CLRPC_ERR_ALLOC_LOWER;
	                }
                    else 
                    {
		                CLRPC_LL_UDP_OPEN_PTR_TYPE param;

		                CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			                , "opening the client-socket"
			                );

		                param = &lower->args.udp_open;

		                CLRPC_ASSERT(CLRPC_LL_GET_OPCODE(lower) == SOCK_OPC_UDP_OPEN);

		                param->loc_addr.sin_family = SOCK_AF_INET;
		                param->loc_addr.sin_port = 0; /* let UDP choose */
		                param->loc_addr.sin_addr.S_un.S_addr = 0;

                        clrpc_request_lower(lower, SOCK_OPC_UDP_OPEN, & channel->cl.dispatch_tag);
			            /* .cnf triggers channel-event */

		                channel->cl.lower.state = CLRPC_CL_LOWER_OPENING;
	                }
                }
                else
                {
		            CLRPC_SV_TRACE_01 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			            , "wrong socket state lower_state=%d"
			            , channel->cl.lower.state
                        );
                    clrpc_clear_interface_param_int(channel);
                    response = CLRPC_ERR_SEQUENCE;
                }
            }
#else
#if CLRPC_CFG_MAX_CLIENTS
		        CLRPC_SV_TRACE_01 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			        , "do not enable clients without defining CLRPC_CFG_MAX_CLIENTS, cl.lower_state=%d"
			        , channel->cl.lower.state
                    );
#endif
                response = CLRPC_ERR_PARAM;
#endif
        }
        else
        {
            /* clients are not enabled */
            if (clrpc_mac_addr_is_zero (rqb_ptr->args.channel.set_interface_param->mac_addr))
            {
		        CLRPC_SV_TRACE_00 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			        , "mac_addr 0 not allowed without clients"
                    );
                clrpc_clear_interface_param_int(channel);
                response = CLRPC_ERR_PARAM;
            }
            else
            {
		        clrpc_uuid_node_set(channel, rqb_ptr->args.channel.set_interface_param->mac_addr);
                
                response = CLRPC_OK;
            }
        }
    }
    
    if (response != CLRPC_OK_ACTIVE)
    {
		clrpc_callback (channel, response, & channel->upper, CLRPC_MODULE_ID, __LINE__);
    }
}

/*------------------------------------------------------------------------------
//	func: clrpc_op_clear_interface_param
//	work: starts undo for the interface parameters of clrpc_set_interface_param 
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR 
clrpc_op_clear_interface_param(
	CLRPC_CHANNEL_PTR_TYPE      channel,
	CLRPC_UPPER_RQB_PTR_TYPE    rqb_ptr
)
{
	CLRPC_LOG_FCT("clrpc_op_clear_interface_param")
    LSA_UINT16 response = CLRPC_OK_ACTIVE;
#if CLRPC_CFG_MAX_SERVERS
	LSA_UINT16  id;
    LSA_BOOL found = LSA_FALSE;
#endif
    
    CLRPC_ASSERT (CLRPC_PTR_IS_NULL (channel->upper));
    channel->upper = rqb_ptr;
    
    if (! clrpc_system_channel_is_ready ())
	{
		CLRPC_SV_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "system-channel not ready yet"
			);
        response = CLRPC_ERR_SEQUENCE; 
	}
#if CLRPC_CFG_MAX_CLIENTS
    else if( channel->cl.instanceCnt != 0 ) 
    {
		CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "%u client-instances still around (did not wait for unbind?)"
			, channel->cl.instanceCnt
			);
        response = CLRPC_ERR_SEQUENCE; 
    }
#endif
    else 
    {
#if CLRPC_CFG_MAX_SERVERS
        /* search for registered servers */
        for (id = 0; id < CLRPC_CFG_MAX_SERVERS; ++id) 
        {
		    CLRPC_SERVER_PTR_TYPE  server = & clrpc_servers[id];
            if (server->state != CLRPC_SST_FREE) 
            {
                if (CLRPC_PTR_ARE_EQUAL (server->channel, channel)) 
                {
				    CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_ERROR
					    , "sid(%u) not unregistered, state(%u)"
					    , server->id, server->state
					    );
                    found = LSA_TRUE;
                    break;
			    }
            }
	    }
        if (found)
        {
            response = CLRPC_ERR_SEQUENCE;
        }
#endif
    }
    if (response == CLRPC_OK_ACTIVE)
    {
        clrpc_clear_interface_param_next_step(channel);
    }
    else
    {
		clrpc_callback (channel, response, & channel->upper, CLRPC_MODULE_ID, __LINE__);
    }
}
/*------------------------------------------------------------------------------
//	func: clrpc_clear_interface_param_next_step
//	work: makes undo for the interface parameters of clrpc_set_interface_param 
//        and closes the socket for client functions if opened
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR 
clrpc_clear_interface_param_next_step(
	CLRPC_CHANNEL_PTR_TYPE      channel
)
{
	CLRPC_LOG_FCT("clrpc_clear_interface_param_next_step")
    LSA_UINT16 response = CLRPC_OK_ACTIVE;
	CLRPC_UPPER_RQB_PTR_TYPE    rqb_ptr;
    
    rqb_ptr = channel->upper;
    
#if CLRPC_CFG_MAX_CLIENTS
    switch (channel->cl.lower.state)
    {
        case CLRPC_CL_LOWER_READY:
        {
			CLRPC_LOWER_RQB_PTR_TYPE lower;
			CLRPC_LL_UDP_CLOSE_PTR_TYPE param;

			/* we kept a lower RQB it to avoid alloc-problems here */

			CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(channel->cl.lower.rqb)); /* see open-done above */

			lower = channel->cl.lower.rqb;
			channel->cl.lower.rqb = LSA_NULL;

			CLRPC_ASSERT(CLRPC_LL_GET_OPCODE(lower) == SOCK_OPC_UDP_OPEN);

			/* close the socket */
			CLRPC_ASSERT(channel->cl.lower.sock_fd != CLRPC_LL_INVALID_SOCKET);

			param = &lower->args.close;
			param->sock_fd = channel->cl.lower.sock_fd;

            clrpc_request_lower(lower, SOCK_OPC_CLOSE, & channel->cl.dispatch_tag);

			channel->cl.lower.state = CLRPC_CL_LOWER_CLOSING;
            break;
        }
        case CLRPC_CL_LOWER_CLOSED:
        {
            clrpc_clear_interface_param_int(channel);
	        CLRPC_ASSERT(channel->cl.lower.state == CLRPC_CL_LOWER_CLOSED); /* socket is closed */
	        CLRPC_ASSERT(channel->cl.lower.provideCnt == 0); /* socket is closed */
            response = CLRPC_OK;
            break;
        }
        case CLRPC_CL_LOWER_OPENING:
        {
		    CLRPC_SV_TRACE_00 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			    , "wait for answer of SET_INTERFACE_PARAM"
			    );
            response = CLRPC_ERR_SEQUENCE; 
            break;
        }
        case CLRPC_CL_LOWER_CLOSING:
        {
		    CLRPC_SV_TRACE_00 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_ERROR
			    , "do not call CLEAR_INTERFACE_PARAM twice"
			    );
            response = CLRPC_ERR_SEQUENCE; 
            break;
        }
        default:
        {
		    CLRPC_SV_TRACE_01 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_FATAL
			    , "unknown internal state of lower socket: state=%d", channel->cl.lower.state
			    );
            CLRPC_FATAL1(channel->cl.lower.state);
            
            break; 
        }
    }
#else     
    clrpc_clear_interface_param_int(channel);
    response = CLRPC_OK;
#endif

    if (CLRPC_RQB_GET_OPCODE (rqb_ptr) == CLRPC_OPC_CLOSE_CHANNEL) 
    {
        if (response == CLRPC_OK)
        {
            clrpc_close_channel_next_step(channel);
        }
        else if (response != CLRPC_OK_ACTIVE)
        {
	        clrpc_channel_done(channel, response);
        }
    }    
    else if (CLRPC_RQB_GET_OPCODE (rqb_ptr) == CLRPC_OPC_CLEAR_INTERFACE_PARAM)
    {
        if (response != CLRPC_OK_ACTIVE)
        {
    		clrpc_callback (channel, response, & channel->upper, CLRPC_MODULE_ID, __LINE__);
        }
    }
    else
    {
		CLRPC_SV_TRACE_01 (rqb_ptr->args.channel.set_interface_param->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "opcode=%d not possible", CLRPC_RQB_GET_OPCODE (rqb_ptr)
			);
        CLRPC_FATAL1(CLRPC_RQB_GET_OPCODE (rqb_ptr));
    }
}


/*------------------------------------------------------------------------------
//	func: clrpc_request
//	work: receives a request from the user, forwards it to the subsystem
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_UPPER_IN_FCT_ATTR
clrpc_request (
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	)
{
	CLRPC_LOG_FCT("clrpc_request")
	CLRPC_CHANNEL_PTR_TYPE channel;

    if (CLRPC_PTR_IS_NULL(rb)) 
    {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no RQB"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}
    
    CLRPC_UPPER_TRACE_03 (0, LSA_TRACE_LEVEL_NOTE,
        ">>> rqb(0x%08x) h(%u) opc(%u)",
        rb, CLRPC_RQB_GET_HANDLE (rb), CLRPC_RQB_GET_OPCODE (rb)
         );
         

#if CLRPC_DEBUG
    CLRPC_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CLRPC_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

    if (CLRPC_RQB_GET_OPCODE (rb) == CLRPC_OPC_TIMER) 
    {
		clrpc_timeout_synchronized(rb);
		return;
	}

    if (/*CLRPC_RQB_GET_HANDLE (rb) < 0 ||*/ CLRPC_RQB_GET_HANDLE (rb) >= CLRPC_CFG_MAX_CHANNELS) 
    {
		CLRPC_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL
			, "handle(%u) out of range"
			, CLRPC_RQB_GET_HANDLE(rb)
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}

    if (CLRPC_PTR_IS_NULL(CLRPC_RQB_GET_ARGS(rb))) 
    {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "no args"
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
    }

    channel = &clrpc_channels[CLRPC_RQB_GET_HANDLE (rb)];

    if (channel->state != CLRPC_ST_READY) 
    {
		CLRPC_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "wrong channel.state(%u)"
			, channel->state
			);
		CLRPC_UPPER_RQB_ERROR (rb); /* may return!? */
		return;
	}
    
    if (CLRPC_PTR_IS_NOT_NULL(channel->upper)) 
    {
		CLRPC_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "channel not ready with last request in rqb(0x%08x) opc(%u)"
			, channel->upper, CLRPC_RQB_GET_OPCODE(channel->upper)
			);
	    clrpc_callback (channel, CLRPC_ERR_SEQUENCE, & rb, CLRPC_MODULE_ID, __LINE__);
        return;
	}

    switch (CLRPC_RQB_GET_OPCODE (rb)) 
    {
        case CLRPC_OPC_SET_INTERFACE_PARAM:
        {
            clrpc_op_set_interface_param(channel, rb);
            break;
        }
        case CLRPC_OPC_CLEAR_INTERFACE_PARAM:
        {
            clrpc_op_clear_interface_param(channel, rb);
            break;
        }

#if CLRPC_CFG_MAX_CLIENTS
	    case CLRPC_OPC_CL_BIND:
	    case CLRPC_OPC_CL_REBIND:
	    case CLRPC_OPC_CL_UNBIND:
	    case CLRPC_OPC_CL_CALL:
	    case CLRPC_OPC_CL_CANCEL:
	    case CLRPC_OPC_CL_QUERY_EPM_BIND:
	    case CLRPC_OPC_CL_QUERY_EPM_NEXT:
	    case CLRPC_OPC_CL_QUERY_EPM_BREAK:
	    case CLRPC_OPC_CL_QUERY_EPM_UNBIND:
            if (channel->detailptr->path_type != CLRPC_PATH_TYPE_USER) 
            {
			    clrpc_callback (channel, CLRPC_ERR_SYS_PATH, & rb, CLRPC_MODULE_ID, __LINE__);
			    break;
		    }
            clrpc_cl_request(channel, rb);
		    break;
#endif

#if CLRPC_CFG_MAX_SERVERS
	    case CLRPC_OPC_SV_REGISTER_IF_OBJ:
		    clrpc_sv_op_register_if_obj (channel, rb);
		    break;

	    case CLRPC_OPC_SV_UNREGISTER_IF_OBJ:
		    clrpc_sv_op_unregister_if_obj (channel, rb);
		    break;

	    case CLRPC_OPC_SV_CALL_IND_PROVIDE:
		    clrpc_sv_op_call_ind_provide (channel, rb);
		    break;
	    case CLRPC_OPC_SV_CALL_RESPONSE:
		    clrpc_sv_op_call_response (channel, rb);
		    break;

	    case CLRPC_OPC_EPM_START:
		    clrpc_op_epm_start (channel, rb);
		    break;

	    case CLRPC_OPC_EPM_STOP:
		    clrpc_op_epm_stop (channel, rb);
		    break;
#endif

	    default:
		    CLRPC_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			    , "unsupported opcode(%u)"
			    , CLRPC_RQB_GET_OPCODE(rb)
			    );
		    clrpc_callback (channel, CLRPC_ERR_OPCODE, & rb, CLRPC_MODULE_ID, __LINE__);
		    break;
	}
	return;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
