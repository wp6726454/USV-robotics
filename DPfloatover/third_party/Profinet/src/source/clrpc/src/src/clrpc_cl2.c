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
/*  F i l e               &F: clrpc_cl2.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of CLRPC-Client, Part 2                                   */
/*                                                                           */
/*  cited OSF sources DCE 1.1 are available from http://www.opengroup.org    */
/*                                                                           */
/*    - Client Timeout callback                                              */
/*    - UDP receive done callback                                            */
/*    - Bind/Rebind common things                                            */
/*    - Delayed Bind Callback                                                */
/*    - Delayed Rebind Callback                                              */
/*    - Delayed Unbind Callback                                              */
/*    - Client Request                                                       */
/*    - Channel-Event Handler                                                */
/*                                                                           */
/*  O v e r v i e w:                                                         */
/*                                                                           */
/*    Request with Client-Opcodes come in via clrpc_cl_request().            */
/*                                                                           */
/*    Bind-Requests are handled especially. See clrpc_cl_bind_delayed().     */
/*                                                                           */
/*    Bind- / Unbind Requests Open / Close the Client Socket. The requests   */
/*    are synchronized with the Open / Close.                                */
/*                                                                           */
/*    Rebind-, Call- and Unbind-Requests are queued directly to the client's */
/*    call-queue and are processed in-order. Rebind and Unbind run after all */
/*    foregoing calls are processed. See clrpc_cl_rebind_delayed() and       */
/*    clrpc_cl_unbind_delayed().                                             */
/*****************************************************************************/

#define CLRPC_MODULE_ID    21
#define LTRC_ACT_MODUL_ID  21

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


#if CLRPC_CFG_MAX_CLIENTS /* compile only, if client-part is configured */

/*=============================================================================
//=== TIMEOUT =================================================================
//===========================================================================*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_timeout(
	CLRPC_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE user_ptr
) {
	CLRPC_LOG_FCT("clrpc_cl_timeout")
	CLRPC_CLIENT_PTR_TYPE client = (CLRPC_CLIENT_PTR_TYPE)user_ptr; /* typecast */

	CLRPC_ASSERT(oneshot);
	CLRPC_ASSERT(client);

	LSA_UNUSED_ARG(oneshot);

	CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< client(%08x) --- TIMEOUT ---"
		, client
		);

	CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);

	/* set timeout-flag and call the event-handler */

	client->timeoutFlag = 1;

	clrpc_cl_event(client);
}

/*=============================================================================
//=== UDP RECEIVE DONE ========================================================
//===========================================================================*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_udp_recv_done(
	CLRPC_CHANNEL_PTR_TYPE channel, /* sanity checked in caller */
	CLRPC_LOWER_RQB_PTR_TYPE lower /* sanity checked in caller */
) {
	CLRPC_LOG_FCT("clrpc_cl_udp_recv_done")
	CLRPC_LL_UDP_DATA_PTR_TYPE param = &lower->args.data;

	CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rsp(%u) data_length(%u)"
		, CLRPC_LL_GET_RESPONSE (lower), param->data_length
		);

#if CLRPC_DEBUG
	if( CLRPC_LL_GET_RESPONSE (lower) == SOCK_RSP_OK && param->data_length >= CLRPC_HEADER_SIZE ) {

		CLRPC_HEADER_PTR_TYPE h = (CLRPC_HEADER_PTR_TYPE)param->buffer_ptr; /* see packing-checks in clrpc_check_sizes() */
		LSA_UNUSED_ARG (h);

		CLRPC_CL_PKT_TRACE_08(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "=== RECV === ip(%08x) port(%u)  server_boot(%u)  serial(%u)  fragnum(%u)  drep0_1_2(%#x %#x %#x)"
			, CLRPC_NTOHL(param->rem_addr.sin_addr.S_un.S_addr)
			, CLRPC_NTOHS(param->rem_addr.sin_port)
			, h->server_boot, ((h->serial_hi << 8) | h->serial_lo), h->fragnum
			, h->drep[0], h->drep[1], h->drep[2]
			);
		CLRPC_CL_PKT_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".RECV ptype(%u)  opnum(%u)  len(%u)  flags(%#x:%c%c%c%c%c%c) flags2(%#x)"
			, h->ptype, h->opnum, h->len
			, h->flags
				, h->flags & CLRPC_PF_FRAG ? 'F' : '-'
				, h->flags & CLRPC_PF_LAST_FRAG ? 'L' : '-'
				, h->flags & CLRPC_PF_NO_FACK ? 'N' : '-'
				, h->flags & CLRPC_PF_MAYBE ? 'm' : '-'
				, h->flags & CLRPC_PF_IDEMPOTENT ? 'i' : '-'
				, h->flags & CLRPC_PF_BROADCAST ? 'b' : '-'
			, h->flags2
			);
		CLRPC_CL_PKT_TRACE_12 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".RECV actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) actseq(%u)"
			, h->actuid.time_low
			, h->actuid.time_mid
			, h->actuid.time_hi_and_version
			, h->actuid.clock_seq_hi_and_reserved
			, h->actuid.clock_seq_low
			, h->actuid.node[0]
			, h->actuid.node[1]
			, h->actuid.node[2]
			, h->actuid.node[3]
			, h->actuid.node[4]
			, h->actuid.node[5]
			, h->seq
			);
		CLRPC_CL_PKT_TRACE_12 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".RECV ifuuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) vers=0x%08x"
			, h->if_id.time_low
			, h->if_id.time_mid
			, h->if_id.time_hi_and_version
			, h->if_id.clock_seq_hi_and_reserved
			, h->if_id.clock_seq_low
			, h->if_id.node[0]
			, h->if_id.node[1]
			, h->if_id.node[2]
			, h->if_id.node[3]
			, h->if_id.node[4]
			, h->if_id.node[5]
			, h->if_vers
			);
		CLRPC_CL_PKT_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".RECV objuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, h->object.time_low
			, h->object.time_mid
			, h->object.time_hi_and_version
			, h->object.clock_seq_hi_and_reserved
			, h->object.clock_seq_low
			, h->object.node[0]
			, h->object.node[1]
			, h->object.node[2]
			, h->object.node[3]
			, h->object.node[4]
			, h->object.node[5]
			);
	}
#endif

	CLRPC_ASSERT(channel->cl.lower.provideCnt > 0);
    channel->cl.lower.provideCnt -= 1;
        
    if (channel->cl.lower.state == CLRPC_CL_LOWER_READY)
    {
        if( CLRPC_LL_GET_RESPONSE (lower) == SOCK_RSP_OK ) {

		    CLRPC_HEADER_PTR_TYPE hdr = (CLRPC_HEADER_PTR_TYPE)param->buffer_ptr; /* see packing-checks in clrpc_check_sizes() */

		    if(
			    param->rem_addr.sin_addr.S_un.S_addr == CLRPC_HTONL(0)
			    || param->rem_addr.sin_addr.S_un.S_addr == CLRPC_HTONL(0xFFFFFFFF)
			    || param->rem_addr.sin_port == CLRPC_HTONS(0)
		    ) {
			    CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				    , "invalid ip(%#x) or port(%u)"
				    , CLRPC_NTOHL(param->rem_addr.sin_addr.S_un.S_addr)
				    , CLRPC_NTOHS(param->rem_addr.sin_port)
				    );
			    /*
			     * NOTE: 0.0.0.0 is a "valid" src-address (e.g. for the DHCP protocol)
			     *       255.255.255.255 is a configuration error at the sender
			     *       port 0 cannot be opened by a user (= dynamic port)
			     *
			     * since we cannot reply to such an address, we have to drop the packet.
			     */
			    goto provide;
		    }

		    if( param->data_length < CLRPC_HEADER_SIZE ) {
			    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				    , "len < CLRPC_HEADER_SIZE, dropped..."
				    );
			    goto provide;
		    }

		    CLRPC_CL_TRACE_07(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			    , "--- RECV --- [%08x:%u] recvlen(%u), vers(%u) ptype(%u) drep0(%#x) bodylen(%u)"
			    , CLRPC_NTOHL(param->rem_addr.sin_addr.S_un.S_addr)
			    , CLRPC_NTOHS(param->rem_addr.sin_port)
			    , param->data_length
			    , hdr->rpc_vers
			    , hdr->ptype
			    , hdr->drep[0]
			    , hdr->len
		    );

		    if( hdr->rpc_vers != CLRPC_HEADER_VERSION ) {
			    /*
			     * actually only the lower 4 bits. see DG.H line 376
			     * will it ever change?
			     */
			    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				    , "version != CLRPC_HEADER_VERSION, dropped..."
				    );
			    goto provide;
		    }

		    if( CLRPC_DREP_INT_DIFFERS(hdr->drep[0]) ) {

			    CLRPC_SWAP_UUID(&hdr->object);

			    CLRPC_SWAP_UUID(&hdr->if_id);

			    CLRPC_SWAP_UUID(&hdr->actuid);

			    CLRPC_SWAP_U32(&hdr->server_boot);

			    CLRPC_SWAP_U32(&hdr->if_vers);

			    CLRPC_SWAP_U32(&hdr->seq);

			    CLRPC_SWAP_U16(&hdr->opnum);

			    CLRPC_SWAP_U16(&hdr->ihint);

			    CLRPC_SWAP_U16(&hdr->ahint);

			    CLRPC_SWAP_U16(&hdr->len);

			    CLRPC_SWAP_U16(&hdr->fragnum);
		    }

		    if( hdr->ptype > CLRPC_PT_MAX ) {
			    /*
			     * actually only the lower 5 bits? see DG.H line 384
			     *
			     * will it ever change?
			     */
			    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				    , "ptype > CLRPC_PT_MAX, dropped..."
				    );
			    goto provide;
		    }

		    if( CLRPC_HEADER_SIZE + hdr->len != param->data_length ) {
			    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				    , "inconsistent length, dropped..."
				    );
			    goto provide;
		    }

		    if(	CLRPC_PT_IS_STOC(hdr->ptype) ) { /* server-to-client */

			    /* look up the client instance */

			    CLRPC_CLIENT_PTR_TYPE client = (CLRPC_CLIENT_PTR_TYPE)clrpc_hash_lookup(
				    &channel->cl.actLUT, &hdr->actuid, CLRPC_NO_HINT
				    ); /* note: ahint is for server-side only!!! */

			    if( CLRPC_PTR_IS_NOT_NULL(client) ) {

				    CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);

				    CLRPC_ASSERT(clrpc_uuid_equal(&hdr->actuid, &client->bnd.act.uuid)); /* bug? inconsistent activity */

				    CLRPC_CL_TRACE_08(client->trace_idx, LSA_TRACE_LEVEL_NOTE
					    , "<<< client(%08x) --- RECV.ind --- [%08x:%u] recvlen(%u), vers(%u) ptype(%u) drep0(%#x) bodylen(%u)"
					    , client
					    , CLRPC_NTOHL(param->rem_addr.sin_addr.S_un.S_addr)
					    , CLRPC_NTOHS(param->rem_addr.sin_port)
					    , param->data_length
					    , hdr->rpc_vers
					    , hdr->ptype
					    , hdr->drep[0]
					    , hdr->len
					    );

				    /* we have an instance, put the packet to the recv-queue */

				    clrpc_queue_enqueue(&client->recvQ, lower);

				    clrpc_cl_event(client);

				    return; /* enqueued! */
			    }
			    else {
				    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					    , "unknown actuid, dropped..."
					    );

				    /*
				     * see OSF sources DGCLSN.C line 155 ccall==NULL
				     * ==> send an ACK to satisfy the server
				     *
				     * see OSF sources see DGCLSN.C line 674
				     * ==> if NOCALL, just drop.
				     *
				     *
				     * we are not nice and don't do anything because we don't have a send-resource...
				     *
				     */
			    }
		    }
		    else { /* not server-to-client, maybe a conversation manager request */

			    if( hdr->ptype == CLRPC_PT_REQUEST ) {

				    /*
				     * Microsoft sets flags2 bit 0x04 which is "reserved" in the OSF DCE docs...
				     */

				    /* conversation manager requests are "single fragment" and "idempotent"
				     * DGLSN.C line 3011: "...the current implementation just can't handle multifrag conv calls"
				     */

				    if( (hdr->flags &
						    ( CLRPC_PF_FRAG /* not fragmented => don't check hdr->fragnum */
						    | CLRPC_PF_MAYBE
						    | CLRPC_PF_IDEMPOTENT
						    | CLRPC_PF_BROADCAST )
					    ) == CLRPC_PF_IDEMPOTENT
					    && clrpc_uuid_equal(&hdr->if_id, &clrpc_conversation_manager_uuid)
					    && hdr->if_vers == CLRPC_MAKE_IF_VERS(3, 0) /* version 3.0 */
					    && hdr->opnum < 2 /* WAY or WAY2, not-supported: AYT, WAYAUTH, WAYAUTHMORE */
					    && hdr->len == CLRPC_WAY_REQ_SIZE /* actuid + boot_time */
				    ) {
					    CLRPC_WAY_REQ_PTR_TYPE req = (CLRPC_WAY_REQ_PTR_TYPE)(hdr+1); /* see packing-checks in clrpc_check_sizes() */
					    CLRPC_CLIENT_PTR_TYPE client;

					    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT     /* client is not initialized */
						    , "conversation manager request"
						    );

					    if( CLRPC_DREP_INT_DIFFERS(hdr->drep[0]) ) {

						    CLRPC_SWAP_UUID(&req->actuid);

						    CLRPC_SWAP_U32(&req->boot_time);
					    }

					    client = (CLRPC_CLIENT_PTR_TYPE)clrpc_hash_lookup(&channel->cl.actLUT, &req->actuid, CLRPC_NO_HINT); /* note: ahint is for server-side only!!! */

					    if( CLRPC_PTR_IS_NOT_NULL(client) ) {

						    CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);

						    CLRPC_ASSERT(clrpc_uuid_equal(&req->actuid, &client->bnd.act.uuid)); /* bug? inconsistent activity */

						    /* put the request to the conv queue and call the event-handler */

						    clrpc_queue_enqueue(&client->convQ, lower);

						    clrpc_cl_event(client);

						    return; /* enqueued! */
					    }
					    else {
						    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							    , "conv: unknown actuid, dropped..."
							    );

						    /*
						     * see OSF sources, CONV.C line 103
						     * ==> return nca_s_bad_actuid
						     *
						     * we don't do anything because no send-resource
						     * the server will timeout anyway
						     */
					    }
				    }
				    else {
					    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						    , "unhandled request, dropped..."
						    );

					    /*
					     * could/should send a reject here
					     *
					     * we don't do anything because no send-resource
					     * the server will timeout anyway
					     */
				    }
			    }
			    else {
#if CLRPC_DEBUG
				    if( hdr->ptype != CLRPC_PT_ACK ) { /* don't show acks */
					    CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						    , "unexpected ptype(%u), dropped..."
						    , hdr->ptype
						    );
				    }
#endif

				    /*
				     * Microsoft sends an ACK for a WAY-response although conversation manager calls are idempotent...
				     * and QUIT too if no valid WAY-response is received after some retries
				     *
				     * The ACK seems to be the "last action" when closing the WAY-client's socket...
				     */

				    /*
				     * OSF sources handle a PING request here too.
				     *
				     * WAY/WAY2 is a single-fragment idempotent call
				     * which will be retransmitted rather than ping-ed...
				     *
				     * Microsoft does not PING, they repeat the request
				     */
			    }
		    } /* endif server-to-client */
	    }
        else 
        {
			/* response handled in clrpc_cl_provide() */
			CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "unhandled rsp(%u)", CLRPC_LL_GET_RESPONSE (lower)
				);
	    }
    }
    else
    {
    /* 907659: 
     * if socket is closing, free rqb immediatly, whatever the response may be in this case */
	    CLRPC_ASSERT(channel->cl.lower.state == CLRPC_CL_LOWER_CLOSING);
        
		CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "socket is closing, freeing recv-resource, %u left"
			, channel->cl.lower.provideCnt
			);
        
		clrpc_ll_free_rqb(channel, &lower);
        
		return; /* freed! */
    }

	/* provide the receive resource again */

provide:
	CLRPC_ASSERT(lower);
	clrpc_cl_provide(channel, lower); /* provide again */
}

/*=============================================================================
//=== BIND / REBIND ===========================================================
//===========================================================================*/

static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_bindRebind(
	CLRPC_CLIENT_PTR_TYPE client, /* sanity checked in caller */
	CLRPC_UPPER_CL_BIND_PTR_TYPE param /* sanity checked in caller */
) 
{
	CLRPC_LOG_FCT("clrpc_cl_bindRebind")

	CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "host(%08x), port(%u)"
		, CLRPC_NTOHL(param->host_ip)
		, CLRPC_NTOHS(param->udp_port)
		);

    if( param->host_ip == /*CLRPC_HTONL*/0x00000000 ) 
    {
		CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "host 0.0.0.0 not valid"
			);
        return CLRPC_ERR_PARAM;
	}

    client->bnd.host_ip  = param->host_ip;
	client->bnd.udp_port = param->udp_port; /* 0 ... use default */

    if( client->bnd.udp_port == 0 ) 
    {

		client->bnd.udp_port = CLRPC_EPM_DEFAULT_PORT; /* note: CLRPC_DETAIL_TYPE::ep_mapper_port was removed (AP00619205) */

		CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "using epm-port(%#x)", CLRPC_NTOHS(client->bnd.udp_port)
			);
	}

    client->bnd.if_uuid          = param->if_uuid;
	client->bnd.if_version_major = param->if_version_major;
	client->bnd.if_version_minor = param->if_version_minor;

	client->bnd.object_uuid      = param->object_uuid;

	/***/

	{
	    CLRPC_UUID_TYPE actuid;

	    clrpc_uuid_create_actuid(client->channel, &actuid); /* see clrpc_cl_call_complete() too */

	    clrpc_hash_entry_setup(&client->bnd.act, &actuid, client/*backlink*/);
	}

	client->bnd.actseq = (LSA_UINT32)-1;

	/***/

	/* client->bnd.hintsUpdated  -- reset on every call */

	client->bnd.ahint = CLRPC_NO_HINT;
	client->bnd.ihint = CLRPC_NO_HINT;

	/*
	 * note that the hints are initialized only once
	 * and that the server can handle wrong hints!
	 */

	/***/

	client->bnd.bound = 0;

	/* client->bnd.serverHost -- see recvQ processing */
	/* client->bnd.serverPort -- see recvQ processing */

	client->bnd.serverBootTime = 0; /* unknown */

	return CLRPC_OK;
}

/*=============================================================================
//=== BIND DELAYED ============================================================
//===========================================================================*/

/*
// create another client instance and bind it to the host/port/interface
//
// A bind-RQB is first queued to the bind-queue; then the
// event handler is called. clrpc_cl_bind_delayed() is called when the common
// client socket is "ready".
//
*/

static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_bind_delayed(
	CLRPC_CHANNEL_PTR_TYPE channel, /* sanity checked in caller */
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
) {
	CLRPC_LOG_FCT("clrpc_cl_bind_delayed")
	CLRPC_CLIENT_PTR_TYPE client;
	CLRPC_LOWER_RQB_PTR_TYPE sndlower;

	CLRPC_ASSERT(upper->args.cl.bind);

	/*
	 * create a client instance and bind it
	 * to a server host (and port) and an interface
	 */

	/* allocate another client */

	CLRPC_ALLOC_LOCAL_MEM(CLRPC_CAST_LOCAL_MEM_PTR_PTR(&client), sizeof(*client));

	if( CLRPC_PTR_IS_NULL(client) ) {
		CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "cannot alloc client-instance"
			);
		return CLRPC_ERR_ALLOC_LOCAL;
	}

	/* allocate a send resource */

	/* must adapt callQ-code and send()-code if more than one resource! */

	clrpc_ll_alloc_rqb(channel, SOCK_OPC_UDP_SEND, client, &sndlower);

	if( CLRPC_PTR_IS_NULL(sndlower) ) {
		LSA_UINT16 retval;

		CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "cannot alloc send-resource"
			);

		CLRPC_FREE_LOCAL_MEM(&retval, client); /* free the client instance */
		CLRPC_ASSERT(retval == LSA_RET_OK);
		LSA_UNUSED_ARG(retval);

		return CLRPC_ERR_ALLOC_LOWER;
	}

	/* initialize */

#if CLRPC_DEBUG
	CLRPC_MEMSET(client, 0xCC, sizeof(*client)); /* to find uninitialized things */
#endif

	CLRPC_ASSERT(&((CLRPC_CLIENT_PTR_TYPE)0)->dispatch_tag == 0); /* must be first in struct */
	CLRPC_SET_DISPATCH_TAG(client->dispatch_tag, clrpc_cl_client_lower_done);

	client->channel   = channel;
	client->trace_idx = channel->trace_idx; /* a copy to avoid the indirection */

	client->state = CLRPC_CL_NO_INSTANCE; /* see memset... */

	client->entered = 0;

	client->again = 0;

	/***/

	{
	LSA_UINT16 response;

	response = clrpc_cl_bindRebind(client, upper->args.cl.bind);

	if( response != CLRPC_OK ) {
		LSA_UINT16 retval;

		CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "failed, rsp(%u)", response
			);

		clrpc_ll_free_rqb(channel, &sndlower);

		CLRPC_FREE_LOCAL_MEM(&retval, client); /* free the client instance */
		CLRPC_ASSERT(retval == LSA_RET_OK);
		LSA_UNUSED_ARG(retval);

		return response;
	}
	}

	/***/

	CLRPC_ASSERT(client->bnd.act.user_ptr == client);

	/***/

	clrpc_queue_init(&client->callQ);

	clrpc_queue_init(&client->cancelQ);

	clrpc_queue_init(&client->convQ);

	clrpc_queue_init(&client->recvQ);

	clrpc_queue_init(&client->sendCnfQ);

	clrpc_oneshot_init(&client->oneshot, clrpc_cl_timeout, client);

	client->timeoutFlag = 0;

	client->call.curr = LSA_NULL;

	/* client->call.nca_status -- init on every call */

	/* client->txq -- init on every call */

	/* client->rxq -- init on every call */

	/* put the send-resource to the send confirmation queue */

	clrpc_queue_enqueue(&client->sendCnfQ, sndlower);

	clrpc_cl_pnio_init(client);

	/* idle now, add the client to the lookup-table */

	client->state = CLRPC_CL_IDLE;

	clrpc_hash_add(&client->channel->cl.actLUT, &client->bnd.act);

	/* return the client handle */

	upper->args.cl.bind->client_id = client;

	if( CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BIND ) {
		return clrpc_cl_query_epm_bind_done(CLRPC_OK, upper);
	}

	return CLRPC_OK;
}

/*=============================================================================
//=== REBIND DELAYED ==========================================================
//===========================================================================*/

/*
// change the binding information (host/port/interface) of a client instance
//
// execution of a rebind-request has to be delayed after all foregoing calls
// are completed. this is achieved by enqueueing the request to the call-queue.
// the client's event handler will call rebind_delayed() when it is ok to change
// the binding information.
//
*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_rebind_delayed(
	CLRPC_CLIENT_PTR_TYPE client, /* sanity checked in caller */
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
) {
	CLRPC_LOG_FCT("clrpc_cl_rebind_delayed")
	LSA_UINT16 response;

	CLRPC_ASSERT(client == (CLRPC_CLIENT_PTR_TYPE)upper->args.cl.bind->client_id)

	/* remove the client from the lookup table */

	clrpc_hash_remove(&client->channel->cl.actLUT, &client->bnd.act.uuid); /* must exist or fatal! */

	/* there could be (now old) data on the convQ or recvQ, remove it */

	while( clrpc_queue_not_empty(&client->convQ) ) {
		CLRPC_LOWER_RQB_PTR_TYPE lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->convQ);
		CLRPC_ASSERT(lower);
		/* recv-resources belong to the channel, not to the client! */
		clrpc_cl_provide(client->channel, lower);
	}

	while( clrpc_queue_not_empty(&client->recvQ) ) {
		CLRPC_LOWER_RQB_PTR_TYPE lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->recvQ);
		CLRPC_ASSERT(lower);
		/* recv-resources belong to the channel, not to the client! */
		clrpc_cl_provide(client->channel, lower);
	}

	/***/

	response = clrpc_cl_bindRebind(client, upper->args.cl.bind);

	if( response == CLRPC_OK ) {

		CLRPC_ASSERT(client->bnd.act.user_ptr == client);

		clrpc_hash_add(&client->channel->cl.actLUT, &client->bnd.act);
	}
	else {
		CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "failed, rsp(%u)"
			, response
			);
	}

	clrpc_callback(client->channel, response, &upper, CLRPC_MODULE_ID, __LINE__);
}

/*=============================================================================
//=== UNBIND DELAYED ==========================================================
//===========================================================================*/

/*
// unbind and destroy a client instance
//
// execution of a unbind-request has to be delayed after all foregoing calls
// are completed. this is achieved by enqueueing the request to the call-queue.
// the client's event handler will call unbind_delayed() then.
//
*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_unbind_delayed(
	CLRPC_CLIENT_PTR_TYPE client, /* sanity checked in caller */
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
) {
	CLRPC_LOG_FCT("clrpc_cl_unbind_delayed")
	CLRPC_CHANNEL_PTR_TYPE channel;
	LSA_UINT16 retval;

	CLRPC_ASSERT(client == (CLRPC_CLIENT_PTR_TYPE)upper->args.cl.unbind->client_id)

	/* remove client from the lookup-table */

	clrpc_hash_remove(&client->channel->cl.actLUT, &client->bnd.act.uuid); /* must exist or fatal! */

	/* free resources in queues */

	while( clrpc_queue_not_empty(&client->cancelQ) ) {
		CLRPC_UPPER_RQB_PTR_TYPE tmp_upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->cancelQ);
		CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(tmp_upper));
		CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "user-error, more cancels after unbind?!"
			);
		clrpc_callback(client->channel, CLRPC_ERR_SEQUENCE, &tmp_upper, CLRPC_MODULE_ID, __LINE__);
	}

	while( clrpc_queue_not_empty(&client->callQ) ) {
		CLRPC_UPPER_RQB_PTR_TYPE tmp_upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->callQ);
		CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(tmp_upper));
		CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "user-error, more calls after unbind?!"
			);
		clrpc_callback(client->channel, CLRPC_ERR_SEQUENCE, &tmp_upper, CLRPC_MODULE_ID, __LINE__);
	}

	while( clrpc_queue_not_empty(&client->convQ) ) {
		CLRPC_LOWER_RQB_PTR_TYPE lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->convQ);
		CLRPC_ASSERT(lower);
		/* recv-resources belong to the channel, not to the client! */
		clrpc_cl_provide(client->channel, lower);
	}

	while( clrpc_queue_not_empty(&client->recvQ) ) {
		CLRPC_LOWER_RQB_PTR_TYPE lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->recvQ);
		CLRPC_ASSERT(lower);
		/* recv-resources belong to the channel, not to the client! */
		clrpc_cl_provide(client->channel, lower);
	}

	while( clrpc_queue_not_empty(&client->sendCnfQ) ) {
		CLRPC_LOWER_RQB_PTR_TYPE lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->sendCnfQ);
		CLRPC_ASSERT(lower);
		/* send-resources belong to the client! */
		clrpc_ll_free_rqb(client->channel, &lower);
	}

	CLRPC_ASSERT( ! clrpc_oneshot_is_running(&client->oneshot)); /* timer must not be running, since instance will be freed! */

	client->timeoutFlag = 0; /* well, just to make things clear... */

	/***/

	clrpc_cl_pnio_undo_init(client);

	/* retrieve the channel before freeing the client instance */

	channel = client->channel;

	CLRPC_ASSERT(channel);

	/* free the client instance */

	client->state = CLRPC_CL_NO_INSTANCE; /* sanity */

	CLRPC_FREE_LOCAL_MEM(&retval, client);
	CLRPC_ASSERT(retval == LSA_RET_OK);
	LSA_UNUSED_ARG(retval);

	/***/

	upper->args.cl.unbind->client_id = LSA_NULL; /* sanity */

	clrpc_callback(channel, CLRPC_OK, &upper, CLRPC_MODULE_ID, __LINE__);
}
/*=============================================================================
//=== CLIENT CALL  ==========================================================
//===========================================================================*/

/*
// handling for CALL, CANCEL and opcodes which are stored in the callQ
*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR 
clrpc_cl_handle_call_request
(
    CLRPC_CHANNEL_PTR_TYPE channel, 
    CLRPC_UPPER_RQB_PTR_TYPE upper
)
{
	CLRPC_CLIENT_PTR_TYPE client;
    
	CLRPC_LOG_FCT("clrpc_cl_handle_call_request")
	
	CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT 
	        , "request for channel(0x%x) upper(0x%x) cl.instanceCnt(%d)"
	        , channel
	        , upper
	        , channel->cl.instanceCnt
	        );
	
	CLRPC_ASSERT(channel->cl.instanceCnt > 0);
	
	
	client = (CLRPC_CLIENT_PTR_TYPE)upper->args.cl.bind->client_id; /* client_id is first in all types! */
	
	if ( CLRPC_PTR_IS_NULL(client) )
	{
		CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
		    , "opc(%u), wrong client_id"
		    , CLRPC_RQB_GET_OPCODE(upper)
		    );
		clrpc_callback(channel, CLRPC_ERR_PARAM, &upper, CLRPC_MODULE_ID, __LINE__);
	}
	else 
	{
		CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		    , "client(0x%x) client->channel(0x%x) state(%d)"
		    , client, client->channel, client->state
		    );
		CLRPC_ASSERT(CLRPC_PTR_ARE_EQUAL(client->channel, channel));
		CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);
		
		if ( CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_CANCEL )
		{
			clrpc_queue_enqueue(&client->cancelQ, upper);
			
			clrpc_cl_event(client);
		}
		else 
		{
			clrpc_queue_enqueue(&client->callQ, upper);
			
			clrpc_cl_event(client);
		}
	}
}
/*=============================================================================
//=== CLIENT REQUEST ==========================================================
//===========================================================================*/

/*
// entry point for all client-opcodes
//
// bind is special and has to be synchronized with GET_ANY_MAC_ADDR for the channel 
//
*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_request(
    CLRPC_CHANNEL_PTR_TYPE channel, /* sanity checked in caller */
    CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
)
{
    LSA_VOID_PTR_TYPE tmp_ptr;

    CLRPC_LOG_FCT("clrpc_cl_request")

    CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
	    , "channel(0x%x) upper(0x%x) opc(%u)", channel, upper, CLRPC_RQB_GET_OPCODE(upper)
	    );

    /* assert that client_id is at the same place in all types! */
    tmp_ptr = &upper->args.cl.bind->client_id;
    CLRPC_ASSERT( LSA_HOST_PTR_ARE_EQUAL(tmp_ptr, &upper->args.cl.unbind->client_id));
    CLRPC_ASSERT( LSA_HOST_PTR_ARE_EQUAL(tmp_ptr, &upper->args.cl.call->client_id));
    CLRPC_ASSERT( LSA_HOST_PTR_ARE_EQUAL(tmp_ptr, &upper->args.cl.cancel->client_id));
    CLRPC_ASSERT( LSA_HOST_PTR_ARE_EQUAL(tmp_ptr, &upper->args.cl.query_epm->u.bind.client_id));
    CLRPC_ASSERT( LSA_HOST_PTR_ARE_EQUAL(tmp_ptr, &upper->args.cl.query_epm->u.unbind.client_id));
    CLRPC_ASSERT( LSA_HOST_PTR_ARE_EQUAL(tmp_ptr, &upper->args.cl.query_epm->u.call.client_id));

    if ( channel->cl.lower.state != CLRPC_CL_LOWER_READY )
    {
        CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
            , "user-error, opc(%u), lower layer not ready!?", CLRPC_RQB_GET_OPCODE(upper)
            );

        /* user did not wait for SET_INTERFACE_PARAM completion */

        clrpc_callback(channel, CLRPC_ERR_SEQUENCE, &upper, CLRPC_MODULE_ID, __LINE__);
    }
    else 
    {
        if ( CLRPC_RQB_GET_OPCODE(upper) != CLRPC_OPC_CL_BIND )
        {
            CLRPC_ASSERT( ! clrpc_uuid_is_nil(&channel->cl.cas_uuid));
        }
        
        switch ( CLRPC_RQB_GET_OPCODE(upper) )
        {
            /*------------------------------*/
            case CLRPC_OPC_CL_QUERY_EPM_BIND:
            /*------------------------------*/
            {
                LSA_UINT16 response = clrpc_cl_query_epm_bind(upper);

                if ( response != CLRPC_OK )
                {
                    CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "Problem with epm-bind, opc(%u), response(%u)", CLRPC_RQB_GET_OPCODE(upper)
                        , response);
                    clrpc_callback(channel, response, &upper, CLRPC_MODULE_ID, __LINE__);

                    break;
                }
                else 
                {
                    /* continues like a "normal" bind() */
                    //break;
                    //lint -fallthrough 
                }
            }
            //lint -fallthrough 
            /*------------------------------*/
            case CLRPC_OPC_CL_BIND:
            /*------------------------------*/
            {
                if ( !clrpc_mac_addr_is_valid (channel->mac_addr) )
                {
                    CLRPC_LOWER_RQB_PTR_TYPE  lower = clrpc_sock_alloc_rqb();

                    if ( CLRPC_PTR_IS_NULL (lower) )
                    {
                        CLRPC_FATAL1(0);
                    }
                    clrpc_request_lower (lower, SOCK_OPC_GET_ANY_MAC_ADDR, & channel->cl.dispatch_tag);

                    /* .cnf triggers channel-event */
                }
                else 
                {
                    CLRPC_ASSERT( ! clrpc_uuid_is_nil(&channel->cl.cas_uuid));
                }
                CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "pushing bind-request to bindQ"
                    );

                clrpc_queue_enqueue(&channel->cl.upper.bindQ, upper);

                clrpc_cl_channel_event(channel);

                break;  
            }
            /*------------------------------*/
            case CLRPC_OPC_CL_REBIND:
            /*------------------------------*/
            case CLRPC_OPC_CL_UNBIND:
            /*------------------------------*/
            case CLRPC_OPC_CL_QUERY_EPM_UNBIND:
            /*------------------------------*/
            {
                CLRPC_CLIENT_PTR_TYPE client;

                client = (CLRPC_CLIENT_PTR_TYPE)upper->args.cl.bind->client_id; /* client_id is first in all types! */

                /* is there an active call?  */
                if (clrpc_queue_not_empty(&client->callQ) || CLRPC_PTR_IS_NOT_NULL(client->call.curr) )
                {
                    CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "a CALL is active, opc(%u), upper(0x%x)", CLRPC_RQB_GET_OPCODE(upper)
                        , client->call.curr 
                        );
                    clrpc_callback(channel, CLRPC_ERR_SEQUENCE, &upper, CLRPC_MODULE_ID, __LINE__);
                }
                else 
                {
                    /* continues like a "normal" call()   */
                    clrpc_cl_handle_call_request(channel, upper);
                }
                break;
            }
            /*------------------------------*/
            case CLRPC_OPC_CL_CALL:
            /*------------------------------*/
            {
                if ((upper->args.cl.call->semantics == CLRPC_SEMANTICS_MAYBE) || 
                    (upper->args.cl.call->semantics == CLRPC_SEMANTICS_BROADCAST) || 
                    (upper->args.cl.call->semantics == CLRPC_SEMANTICS_BROADCAST_MAYBE) 
                   )
                {
                    /* AP00969270: semantics maybe or broadcast are not allowed */
                    CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "wrong semantics, opc(%u), semantics(%u)", CLRPC_RQB_GET_OPCODE(upper)
                        , upper->args.cl.call->semantics
                        );
                    clrpc_callback(channel, CLRPC_ERR_PARAM, &upper, CLRPC_MODULE_ID, __LINE__);
                }
                else
                {
                    CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "call with opnum(%u) pnio_signature(%u)"
                        , upper->args.cl.call->opnum
                        , upper->args.cl.call->pnio_signature
                        );
                    clrpc_cl_handle_call_request(channel, upper);
                }

                break;
            }
            /*------------------------------*/
            case CLRPC_OPC_CL_CANCEL:
            /*------------------------------*/
            {
                clrpc_cl_handle_call_request(channel, upper);

                break;
            }
            /*------------------------------*/
            case CLRPC_OPC_CL_QUERY_EPM_NEXT:
            /*------------------------------*/
            {
                clrpc_cl_query_epm_next(upper);

                /* continues like a "normal" call()   */
                clrpc_cl_handle_call_request(channel, upper);

                break;
            }
            /*------------------------------*/
            case CLRPC_OPC_CL_QUERY_EPM_BREAK:
            /*------------------------------*/
            {
                LSA_UINT16 response = clrpc_cl_query_epm_break(upper);

                if ( response != CLRPC_OK_CANCELLED )
                {
                    CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "query_epm_break could not cancel, opc(%u), response(%u)", CLRPC_RQB_GET_OPCODE(upper)
                        , response
                        );
                    clrpc_callback(channel, response, &upper, CLRPC_MODULE_ID, __LINE__);
                }
                else 
                {
                    /* continues like a "normal" call()  */
                    clrpc_cl_handle_call_request(channel, upper);
                }
                break;
            }
            /*------------------------------*/
            default :
            /*------------------------------*/
            {
                /* a bug in caller  */
                CLRPC_FATAL1( CLRPC_RQB_GET_OPCODE(upper));
                break;
            }
        }
    }
}

//alt    /*=============================================================================
//alt    //=== CLIENT REQUEST ==========================================================
//alt    //===========================================================================*/
//alt    
//alt    /*
//alt    // entry point for all client-opcodes
//alt    //
//alt    // bind is special in that it has to be synchronized with the open/close
//alt    // of the common client-socket.
//alt    //
//alt    */
//alt    
//alt    LSA_VOID  CLRPC_LOCAL_FCT_ATTR
//alt    clrpc_cl_request(
//alt    	CLRPC_CHANNEL_PTR_TYPE channel, /* sanity checked in caller */
//alt    	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
//alt    ) 
//alt    {
//alt    	CLRPC_LOG_FCT("clrpc_cl_request")
//alt    
//alt    	CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
//alt    		, "channel(0x%x) upper(0x%x) opc(%u)", channel, upper, CLRPC_RQB_GET_OPCODE(upper)
//alt    		);
//alt    	CLRPC_ASSERT(
//alt    		CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_BIND
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_REBIND
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_UNBIND
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_CALL
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_CANCEL
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BIND
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_NEXT
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BREAK
//alt    		|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_UNBIND
//alt        ); /* else a bug in caller */
//alt    
//alt    	/* assert that client_id is first in all types! */
//alt    	CLRPC_ASSERT( &((CLRPC_UPPER_CL_BIND_PTR_TYPE)        0)->client_id == 0 );
//alt    	CLRPC_ASSERT( &((CLRPC_UPPER_CL_UNBIND_PTR_TYPE)      0)->client_id == 0 );
//alt    	CLRPC_ASSERT( &((CLRPC_UPPER_CL_CALL_PTR_TYPE)        0)->client_id == 0 );
//alt    	CLRPC_ASSERT( &((CLRPC_UPPER_CL_CANCEL_PTR_TYPE)      0)->client_id == 0 );
//alt    	CLRPC_ASSERT( &((CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE)   0)->u         == 0 );
//alt        
//alt        
//alt        if( channel->cl.lower.state != CLRPC_CL_LOWER_READY ) 
//alt        {
//alt    		CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
//alt    			, "user-error, opc(%u), lower layer not ready!?", CLRPC_RQB_GET_OPCODE(upper)
//alt    			);
//alt    
//alt    		/* user did not wait for SET_INTERFACE_PARAM completion */
//alt    
//alt    		clrpc_callback(channel, CLRPC_ERR_SEQUENCE, &upper, CLRPC_MODULE_ID, __LINE__);
//alt            return;
//alt    	}
//alt        else
//alt        {
//alt            if( CLRPC_RQB_GET_OPCODE(upper) != CLRPC_OPC_CL_BIND ) 
//alt            {
//alt    		    CLRPC_ASSERT( ! clrpc_uuid_is_nil(&channel->cl.cas_uuid));
//alt            }
//alt            
//alt            switch( CLRPC_RQB_GET_OPCODE(upper) ) 
//alt            {
//alt                /*--------------------------------*/
//alt    	        case CLRPC_OPC_CL_QUERY_EPM_BIND:
//alt                /*--------------------------------*/
//alt    		    {
//alt    		        LSA_UINT16 response = clrpc_cl_query_epm_bind(upper);
//alt    
//alt                    if( response != CLRPC_OK ) 
//alt                    {
//alt    			        clrpc_callback(channel, response, &upper, CLRPC_MODULE_ID, __LINE__);
//alt    			        return;
//alt    		        }
//alt                }
//alt    		    /* continues like a "normal" bind() */
//alt    		    //break;
//alt                //lint -fallthrough 
//alt                /*--------------------------------*/
//alt                case CLRPC_OPC_CL_BIND:
//alt                /*--------------------------------*/
//alt                {
//alt                    if( !clrpc_mac_addr_is_valid (channel->mac_addr) ) 
//alt                    {
//alt    			        CLRPC_LOWER_RQB_PTR_TYPE  lower = clrpc_sock_alloc_rqb();
//alt                        if( CLRPC_PTR_IS_NULL (lower) ) 
//alt                        {
//alt    				        CLRPC_FATAL1(0);
//alt    			        }
//alt    			
//alt                        clrpc_request_lower (lower, SOCK_OPC_GET_ANY_MAC_ADDR, & channel->cl.dispatch_tag);
//alt    			
//alt    			        /* .cnf triggers channel-event */
//alt    		        }
//alt                    else 
//alt                    {
//alt    			        CLRPC_ASSERT( ! clrpc_uuid_is_nil(&channel->cl.cas_uuid));
//alt    		        }
//alt                    
//alt                    CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
//alt    	                , "pushing bind-request to bindQ"
//alt                    );
//alt    
//alt    		        clrpc_queue_enqueue(&channel->cl.upper.bindQ, upper);
//alt    
//alt    		        clrpc_cl_channel_event(channel);
//alt                    
//alt                    return;     
//alt                    //break;    "unreachable" because of the return instruction
//alt                }
//alt    
//alt                /*--------------------------------*/
//alt    	        case CLRPC_OPC_CL_QUERY_EPM_NEXT:
//alt                /*--------------------------------*/
//alt                    
//alt    		        clrpc_cl_query_epm_next(upper);
//alt                    
//alt    		        /* continues like a "normal" call() */
//alt    		        break;
//alt    
//alt                /*--------------------------------*/
//alt    	        case CLRPC_OPC_CL_QUERY_EPM_BREAK:
//alt                /*--------------------------------*/
//alt    		    {
//alt    		        LSA_UINT16 response = clrpc_cl_query_epm_break(upper);
//alt    
//alt                    if( response != CLRPC_OK_CANCELLED ) 
//alt                    {
//alt    			        clrpc_callback(channel, response, &upper, CLRPC_MODULE_ID, __LINE__);
//alt    			        return;
//alt    		        }
//alt                }
//alt                    
//alt    		    /* continues like a "normal" call() */
//alt    		    break;
//alt    
//alt                /*--------------------------------*/
//alt    	        case CLRPC_OPC_CL_QUERY_EPM_UNBIND:
//alt                /*--------------------------------*/
//alt                    
//alt    		        /* continues like a "normal" unbind() */
//alt    		        break;
//alt    	    }
//alt            
//alt        }
//alt        
//alt        if( CLRPC_RQB_GET_OPCODE(upper) != CLRPC_OPC_CL_BIND
//alt    		&& CLRPC_RQB_GET_OPCODE(upper) != CLRPC_OPC_CL_QUERY_EPM_BIND
//alt    	) 
//alt    	{
//alt    		CLRPC_CLIENT_PTR_TYPE client;
//alt     
//alt            CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT 
//alt                    , "channel(0x%x) upper(0x%x) cl.instanceCnt(%d)"
//alt                    , channel
//alt                    , upper
//alt                    , channel->cl.instanceCnt
//alt                    );
//alt                    
//alt    		CLRPC_ASSERT(channel->cl.instanceCnt > 0);
//alt            
//alt    
//alt    		client = (CLRPC_CLIENT_PTR_TYPE)upper->args.cl.bind->client_id; /* client_id is first in all types! */
//alt    
//alt            if( CLRPC_PTR_IS_NULL(client) ) 
//alt            {
//alt    			CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
//alt    				, "opc(%u), wrong client_id"
//alt    				, CLRPC_RQB_GET_OPCODE(upper)
//alt    				);
//alt    			clrpc_callback(channel, CLRPC_ERR_PARAM, &upper, CLRPC_MODULE_ID, __LINE__);
//alt    		}
//alt            else 
//alt            {
//alt                CLRPC_CL_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
//alt                    , "client(0x%x) client->channel(0x%x) state(%d)"
//alt                    , client, client->channel, client->state
//alt                    );
//alt    			CLRPC_ASSERT(CLRPC_PTR_ARE_EQUAL(client->channel, channel));
//alt    			CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);
//alt    
//alt                if( CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_CANCEL ) 
//alt                {
//alt    				clrpc_queue_enqueue(&client->cancelQ, upper);
//alt    
//alt    				clrpc_cl_event(client);
//alt    			}
//alt                else 
//alt                {
//alt    				clrpc_queue_enqueue(&client->callQ, upper);
//alt    
//alt    				clrpc_cl_event(client);
//alt    			}
//alt            }
//alt    	}
//alt    }

/*=============================================================================
//=== CLIENT CHANNEL EVENT ====================================================
//===========================================================================*/

/*
// event handler for (client-) channel events
//
// synchronization of bind/unbind requests 
*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_channel_event(
	CLRPC_CHANNEL_PTR_TYPE channel
) 
{
	CLRPC_LOG_FCT("clrpc_cl_channel_event")

    if( channel->cl.entered ) 
    { /* see comment in clrpc_icl.h */
		channel->cl.again = 1;
	}
    else 
    {
		channel->cl.entered = 1;
        do 
        {
			channel->cl.again = 0;

			/*
			 * check the bind queue
			 */

            //if( ! clrpc_uuid_node_is_zero(channel) ) 
            if( clrpc_mac_addr_is_valid(channel->mac_addr) ) 
            {
                while( clrpc_queue_not_empty(&channel->cl.upper.bindQ) ) 
                {
                    if( channel->cl.lower.state == CLRPC_CL_LOWER_READY ) 
                    {
						CLRPC_UPPER_RQB_PTR_TYPE upper;
						LSA_UINT16 response;

						upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&channel->cl.upper.bindQ);
						CLRPC_ASSERT(upper);

						CLRPC_ASSERT(
							CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_BIND
							|| CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BIND
						);

                        if( channel->cl.instanceCnt >= CLRPC_CFG_MAX_CLIENTS ) 
                        {
							response = CLRPC_ERR_RESOURCE;
						}
                        else 
                        {
							response = clrpc_cl_bind_delayed(channel, upper);

                            if( response == CLRPC_OK ) 
                            {
								++channel->cl.instanceCnt;
							}
                        }

                        clrpc_callback(channel, response, &upper, CLRPC_MODULE_ID, __LINE__);
					}
                    else 
                    {
                        CLRPC_FATAL1(channel->cl.lower.state);
						break;
					}
                }
            }
			
			/*
			 * then check the unbind queue
			 */

            while( clrpc_queue_not_empty(&channel->cl.upper.unbindQ) ) 
            {
                if( channel->cl.lower.state == CLRPC_CL_LOWER_READY ) 
                {
					CLRPC_ASSERT(channel->cl.instanceCnt > 0);

					{
					    CLRPC_UPPER_RQB_PTR_TYPE upper;

					    upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&channel->cl.upper.unbindQ);
					    CLRPC_ASSERT(upper);
					    CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(upper->args.cl.unbind->client_id));

					    CLRPC_ASSERT(
						    CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_UNBIND
						    || CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_UNBIND
					    );

					    clrpc_cl_unbind_delayed((CLRPC_CLIENT_PTR_TYPE)(upper->args.cl.unbind->client_id), upper);
					}

					channel->cl.instanceCnt -= 1;

					CLRPC_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "unbind, %u more instances", channel->cl.instanceCnt
						);
				}
                else 
                {
                    CLRPC_FATAL1(channel->cl.lower.state);
					break;
				}
            }
            
			/***/

        } while( channel->cl.again );
		channel->cl.entered = 0;
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
