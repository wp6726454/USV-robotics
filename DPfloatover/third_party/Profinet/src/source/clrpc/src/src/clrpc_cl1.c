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
/*  F i l e               &F: clrpc_cl1.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of CLRPC-Client, Part 1                                   */
/*                                                                           */
/*    - Init Channel                                                         */
/*    - Undo Init Channel                                                    */
/*    - Lower Done                                                           */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    20
#define LTRC_ACT_MODUL_ID  20

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


#if CLRPC_CFG_MAX_CLIENTS /* compile only, if client-part is configured */

/*=============================================================================
//=== INIT CHANNEL ============================================================
//===========================================================================*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_init_channel(
	CLRPC_CHANNEL_PTR_TYPE channel
) {
	CLRPC_LOG_FCT("clrpc_cl_init_channel")
	CLRPC_ASSERT(channel);

	CLRPC_ASSERT(channel->state == CLRPC_ST_IDLE);

	/* initialize */

#if CLRPC_DEBUG
	CLRPC_MEMSET(&channel->cl, 0xCC, sizeof(channel->cl)); /* to find uninitialized things */
#endif

	CLRPC_SET_DISPATCH_TAG(channel->cl.dispatch_tag, clrpc_cl_channel_lower_done);
	
	/***/

	channel->cl.entered = 0;
	channel->cl.again = 0;


	/***/

	clrpc_queue_init(&channel->cl.upper.bindQ);
	clrpc_queue_init(&channel->cl.upper.unbindQ);

	/***/

	channel->cl.lower.state = CLRPC_CL_LOWER_CLOSED;

	channel->cl.lower.rqb = LSA_NULL; /* see channel-event */
	channel->cl.lower.done = LSA_NULL;

	channel->cl.lower.sock_fd = CLRPC_LL_INVALID_SOCKET;

	/* channel->cl.lower.host -- set together with 'sock_fd' */
	/* channel->cl.lower.port -- set together with 'sock_fd' */

	channel->cl.lower.provideCnt = 0;

	/***/

	channel->cl.instanceCnt = 0;

	clrpc_hash_init(&channel->cl.actLUT);

	/***/

	channel->cl.cas_uuid = clrpc_uuid_nil_uuid;	/* 'cas_uuid' is set after query-mac! */
}

/*=============================================================================
//=== UNDO INIT CHANNEL =======================================================
//===========================================================================*/

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_undo_init_channel(
	CLRPC_CHANNEL_PTR_TYPE channel
) {
	CLRPC_LOG_FCT("clrpc_cl_undo_init_channel")

	CLRPC_ASSERT(channel);

	CLRPC_ASSERT(channel->state == CLRPC_ST_CLOSE);

	/* sequence error if still some clients around */

	if( channel->cl.instanceCnt != 0 ) {
		CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "%u client-instances still around (did not wait for unbind?)"
			, channel->cl.instanceCnt
			);
		return 0; /* FAILED */
	}

	/* cleanup */

	clrpc_hash_undo_init(&channel->cl.actLUT);

	return 1; /* OK */
}

/*=============================================================================
//=== LOWER DONE ==============================================================
//===========================================================================*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_channel_lower_done(
	CLRPC_CHANNEL_PTR_TYPE  sys_channel,
	LSA_VOID_PTR_TYPE  instance_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
) {
	CLRPC_LOG_FCT("clrpc_cl_channel_lower_done")
	CLRPC_CHANNEL_PTR_TYPE channel = CLRPC_CONTAINING_STRUCT (instance_ptr, CLRPC_CHANNEL_PTR_TYPE, cl.dispatch_tag);
	LSA_UINT16 response = CLRPC_LL_GET_RESPONSE(lower);

	LSA_UNUSED_ARG (sys_channel);
	
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(channel));
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower));

    switch( CLRPC_LL_GET_OPCODE(lower) ) 
    {

	    /*-----------------------------------------------------------------------*/
	    case SOCK_OPC_UDP_OPEN:
		    CLRPC_ASSERT(channel->state == CLRPC_ST_READY);

		    CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			    , "<<< SOCK_OPC_UDP_OPEN.cnf, channel(%08x) sock_fd(%u) rsp(%u)"
			    , channel, lower->args.udp_open.sock_fd, response
			    );

		    CLRPC_ASSERT(CLRPC_PTR_IS_NULL(channel->cl.lower.done));

		    channel->cl.lower.done = lower;

		    clrpc_cl_udp_open_socket_cnf(channel);
		    //clrpc_cl_channel_event(channel);
		    break;

	    /*-----------------------------------------------------------------------*/
	    case SOCK_OPC_CLOSE:
		    CLRPC_ASSERT(channel->state == CLRPC_ST_READY || channel->state == CLRPC_ST_CLOSE);

		    CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			    , "<<< SOCK_OPC_CLOSE.cnf, channel(%08x) sock_fd(%u) rsp(%u)"
			    , channel, lower->args.close.sock_fd, response
			    );

		    CLRPC_ASSERT(CLRPC_PTR_IS_NULL(channel->cl.lower.done));

		    channel->cl.lower.done = lower;

		    clrpc_cl_close_socket_cnf(channel);
		    //clrpc_cl_channel_event(channel);
		    break;

	    /*-----------------------------------------------------------------------*/
	    case SOCK_OPC_GET_ANY_MAC_ADDR:
            CLRPC_ASSERT(channel->state == CLRPC_ST_READY || channel->state == CLRPC_ST_CLOSE);
            if (response != SOCK_RSP_OK) 
            {
			    CLRPC_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				    , "SOCK_OPC_GET_ANY_MAC_ADDR.cnf: response(%u) != OK", response
				    );
                if( clrpc_queue_not_empty(&channel->cl.upper.bindQ) ) 
                {
				    CLRPC_UPPER_RQB_PTR_TYPE upper;

				    upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&channel->cl.upper.bindQ);
				    CLRPC_ASSERT(upper);

				    CLRPC_ASSERT(
					    CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_BIND
					    || CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BIND
				    );

                    clrpc_callback(channel, CLRPC_ERR_LOWER_LAYER, &upper, CLRPC_MODULE_ID, __LINE__);
			    }
                else
                {
			        CLRPC_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				        , "upper.bindQ empty"
				        );
                        
                    /* bindQ empty, BIND was confirmed with a positive GET_ANY_MAC_ADDR-confirmation earlier */
			    }
                clrpc_ll_free_rqb(channel, & lower);
            }
            else
            {
		        CLRPC_CL_TRACE_08(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			        , "<<< SOCK_OPC_GET_ANY_MAC_ADDR.cnf, channel(%08x), rsp(%u), mac_addr(%02x-%02x-%02x-%02x-%02x-%02x)"
			        , channel, response
			        , lower->args.get_any_mac_addr.MacAddr.Adr[0], lower->args.get_any_mac_addr.MacAddr.Adr[1]
			        , lower->args.get_any_mac_addr.MacAddr.Adr[2], lower->args.get_any_mac_addr.MacAddr.Adr[3]
			        , lower->args.get_any_mac_addr.MacAddr.Adr[4], lower->args.get_any_mac_addr.MacAddr.Adr[5]
			        );

		        clrpc_uuid_node_set(channel, lower->args.get_any_mac_addr.MacAddr.Adr);
		        clrpc_ll_free_rqb(channel, & lower);
		        clrpc_cl_channel_event(channel);
            }
            break;

	    /*-----------------------------------------------------------------------*/
	    case SOCK_OPC_UDP_RECEIVE:
		    CLRPC_ASSERT(channel->state == CLRPC_ST_READY || channel->state == CLRPC_ST_CLOSE);
		    CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower->args.data.buffer_ptr));

		    CLRPC_CL_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			    , "<<< SOCK_OPC_UDP_RECEIVE.cnf, channel(%08x) sock_fd(%u) data_len(%u) rsp(%u)"
			    , channel, lower->args.data.sock_fd, lower->args.data.data_length
			    , response
			    );

		    clrpc_cl_udp_recv_done(channel, lower);
		    break;

	    /*-----------------------------------------------------------------------*/
	    default:
		    CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			    , "unhandled opcode(%u)", CLRPC_LL_GET_OPCODE(lower)
			    );
		    CLRPC_LOWER_RQB_ERROR(lower); /* may return!? */
		    break;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_udp_open_socket_cnf(
	CLRPC_CHANNEL_PTR_TYPE channel
) 
{
	CLRPC_LOG_FCT("clrpc_cl_udp_open_socket_cnf")
    CLRPC_LOWER_RQB_PTR_TYPE lower_rqb_ptr;
    LSA_UINT16 rcv_resource_count = 0;
    
    if( channel->cl.lower.state == CLRPC_CL_LOWER_OPENING ) 
    {
	    CLRPC_ASSERT(CLRPC_LL_GET_OPCODE(channel->cl.lower.done) == SOCK_OPC_UDP_OPEN);

        if( CLRPC_LL_GET_RESPONSE (channel->cl.lower.done) != SOCK_RSP_OK ) 
        {
		    CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			    , "open socket failed, rsp(%u)"
			    , CLRPC_LL_GET_RESPONSE (channel->cl.lower.done)
			    );

		    clrpc_ll_free_rqb(channel, &channel->cl.lower.done);

		    channel->cl.lower.done = LSA_NULL;

            clrpc_clear_interface_param_int(channel);
            
            CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(channel->upper));
		    clrpc_callback(channel, CLRPC_ERR_LOWER_LAYER, &channel->upper, CLRPC_MODULE_ID, __LINE__);
        
		    channel->cl.lower.state = CLRPC_CL_LOWER_CLOSED;
	    }
        else 
        {
		    CLRPC_LL_UDP_OPEN_PTR_TYPE param;

		    param = &channel->cl.lower.done->args.udp_open;

		    CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			    , "socket opened, host(%08x) port(%u) sock_fd(%u)"
			    , CLRPC_NTOHL(param->loc_addr.sin_addr.S_un.S_addr)
			    , CLRPC_NTOHS(param->loc_addr.sin_port)
			    , param->sock_fd
			    );

		    channel->cl.lower.sock_fd  = param->sock_fd;

		    channel->cl.lower.udp_port = param->loc_addr.sin_port; /* local port, just for information */

		    CLRPC_ASSERT(CLRPC_PTR_IS_NULL(channel->cl.lower.rqb));
		    channel->cl.lower.rqb = channel->cl.lower.done; /* keep for close, see below */
		    channel->cl.lower.done = LSA_NULL;

		    channel->cl.lower.state = CLRPC_CL_LOWER_READY;
            
		    /* provide some receive resources */
            while (clrpc_queue_not_empty(&channel->tmp_rcv_queue))
            {
                lower_rqb_ptr = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&channel->tmp_rcv_queue);
                CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower_rqb_ptr));
            
    		    lower_rqb_ptr->args.data.sock_fd = channel->cl.lower.sock_fd;
			    clrpc_cl_provide(channel, lower_rqb_ptr);
                rcv_resource_count++;
            }
            
            CLRPC_ASSERT(rcv_resource_count == channel->upper->args.channel.set_interface_param->client_receive_resources);
            
            clrpc_callback(channel, CLRPC_OK, &channel->upper, CLRPC_MODULE_ID, __LINE__);
	    }
    }
    else
    {
		CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "open socket cnf in wrong state=%d"
			, channel->cl.lower.state
			);

        CLRPC_FATAL1(channel->cl.lower.state);
    }
}
/*----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_close_socket_cnf(
	CLRPC_CHANNEL_PTR_TYPE channel
) 
{
	CLRPC_LOG_FCT("clrpc_cl_close_socket_cnf")
    
    if( channel->cl.lower.state == CLRPC_CL_LOWER_CLOSING ) 
    {
		CLRPC_ASSERT(CLRPC_LL_GET_OPCODE(channel->cl.lower.done) == SOCK_OPC_CLOSE);

		/* we opened it, so how can the close fail? */

		CLRPC_ASSERT(CLRPC_LL_GET_RESPONSE (channel->cl.lower.done) == SOCK_RSP_OK);

		CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "socket closed"
			);

		clrpc_ll_free_rqb(channel, &channel->cl.lower.done);

		channel->cl.lower.done = LSA_NULL;

		/***/

		CLRPC_ASSERT(channel->cl.lower.provideCnt == 0); /* or else an error in the SOCK-Module */

		channel->cl.lower.sock_fd = CLRPC_LL_INVALID_SOCKET; /* sanity */

		channel->cl.lower.state = CLRPC_CL_LOWER_CLOSED;
        
        
        clrpc_clear_interface_param_next_step(channel);
	}
    else
    {
		CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "close socket cnf in wrong state=%d"
			, channel->cl.lower.state
			);

        CLRPC_FATAL1(channel->cl.lower.state);
    }
}
/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_client_lower_done(
	CLRPC_CHANNEL_PTR_TYPE  sys_channel,
	LSA_VOID_PTR_TYPE instance_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
) {
	CLRPC_LOG_FCT("clrpc_cl_client_lower_done")
	CLRPC_CLIENT_PTR_TYPE  client = (CLRPC_CLIENT_PTR_TYPE)instance_ptr;

	LSA_UNUSED_ARG(sys_channel);
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower));

	switch( CLRPC_LL_GET_OPCODE(lower) ) {

	/*-----------------------------------------------------------------------*/
	case SOCK_OPC_UDP_SEND:
		CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(client));
		CLRPC_ASSERT(client->channel->state == CLRPC_ST_READY || client->channel->state == CLRPC_ST_CLOSE);
		CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);
		CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower->args.data.buffer_ptr));

		CLRPC_CL_TRACE_04(client->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< SOCK_OPC_UDP_SEND.cnf, client(%08x) sock_fd(%u) data_len(%u) rsp(%u)"
			, client, lower->args.data.sock_fd, lower->args.data.data_length
			, CLRPC_LL_GET_RESPONSE(lower)
			);

		/* we don't care whether sending did actually work, we have a timeout-mechanism anyway */

		/*don't!!! CLRPC_ASSERT(CLRPC_LL_GET_RESPONSE (lower) == SOCK_RSP_OK);*/

		/* put the RQB on the send-confirmation-queue and call the event-handler */

		clrpc_queue_enqueue(&client->sendCnfQ, lower);

		clrpc_cl_event(client);
		break;

	/*-----------------------------------------------------------------------*/
	default:
		CLRPC_CL_TRACE_01(sys_channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unhandled opcode(%u)", CLRPC_LL_GET_OPCODE(lower)
			);
		CLRPC_LOWER_RQB_ERROR(lower); /* may return!? */
		break;
	}
}

/*=============================================================================
//===========================================================================*/

#else

#ifdef CLRPC_MESSAGE
#pragma CLRPC_MESSAGE("!!! not compiling client")
#endif /* CLRPC_MESSAGE */

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
