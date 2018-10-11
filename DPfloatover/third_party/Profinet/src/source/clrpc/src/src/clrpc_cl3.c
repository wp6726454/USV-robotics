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
/*  F i l e               &F: clrpc_cl3.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of CLRPC-Client, Part 3                                   */
/*                                                                           */
/*  cited OSF sources DCE 1.1 are available from http://www.opengroup.org    */
/*                                                                           */
/*    - Provide Receive Resource                                             */
/*    - Prepare / Send                                                       */
/*    - State Machine                                                        */
/*    - Event Handler                                                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  22
#define CLRPC_MODULE_ID    LTRC_ACT_MODUL_ID        //22

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


#if CLRPC_CFG_MAX_CLIENTS /* compile only, if client-part is configured */

/***/

#ifdef CLRPC_MESSAGE
/* review: should the timeout-flag be reset on retrigger? */
#endif /* CLRPC_MESSAGE */

/*=============================================================================
//=== PROVIDE A RECEIVE RESOURCE ==============================================
//===========================================================================*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_provide(
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_LOWER_RQB_PTR_TYPE lower
) {
	CLRPC_LOG_FCT("clrpc_cl_provide")

	CLRPC_ASSERT(channel);
    CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower));

	CLRPC_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "lower(%08x) sock_fd(%u)"
		, lower
		, lower->args.data.sock_fd
		);

	if( channel->cl.lower.state != CLRPC_CL_LOWER_READY ) {

		/* happens when closing and packets are received */
		CLRPC_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "lower-not-ready, freeing resource"
			);

		clrpc_ll_free_rqb(channel, &lower);
		return;
	}

	/* RQB and params are still ok, only handle is wrong */

	CLRPC_ASSERT(CLRPC_LL_GET_OPCODE(lower) == SOCK_OPC_UDP_RECEIVE);

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(lower->args.data.buffer_ptr));

	CLRPC_ASSERT(lower->args.data.buffer_length == CLRPC_BUFFER_SIZE);

#if CLRPC_DEBUG
	CLRPC_MEMSET(lower->args.data.buffer_ptr, 0xCC, lower->args.data.buffer_length); /* to find uninitialized things */
#endif

	CLRPC_ASSERT(channel->handle_lower != CLRPC_LL_INVALID_HANDLE);

	lower->args.data.data_length = CLRPC_BUFFER_SIZE; /* sock need this !! */

	channel->cl.lower.provideCnt += 1;

    clrpc_request_lower(lower, SOCK_OPC_UDP_RECEIVE, & channel->cl.dispatch_tag);
}

/*=============================================================================
//=== PREPARE FOR SEND ========================================================
//===========================================================================*/

static CLRPC_HEADER_PTR_TYPE CLRPC_LOCAL_FCT_ATTR
clrpc_cl_prepare(
	CLRPC_CLIENT_PTR_TYPE client,
	CLRPC_LOWER_RQB_PTR_TYPE lower,
	LSA_UINT8 ptype
) {
	CLRPC_LL_UDP_DATA_PTR_TYPE param;
	CLRPC_HEADER_PTR_TYPE hdr; /* see packing-checks in clrpc_check_sizes() */

	CLRPC_ASSERT(client);
	CLRPC_ASSERT(lower);

	param = &lower->args.data;

	CLRPC_ASSERT(param->buffer_length == CLRPC_BUFFER_SIZE);

#if CLRPC_DEBUG
	CLRPC_MEMSET(param->buffer_ptr, 0xCC, param->buffer_length); /* to find uninitialized things */
#endif

	hdr = (CLRPC_HEADER_PTR_TYPE)param->buffer_ptr;
	CLRPC_ASSERT(hdr);

	hdr->rpc_vers = CLRPC_HEADER_VERSION;
	hdr->ptype    = ptype;
	hdr->flags    = 0; /* see DG.H line 410, comment on flags */
	hdr->flags2   = 0;

	hdr->drep[0] = CLRPC_DREP_0;
	hdr->drep[1] = CLRPC_DREP_1;
	hdr->drep[2] = CLRPC_DREP_2;

	hdr->actuid = client->bnd.act.uuid;

	hdr->server_boot = client->bnd.serverBootTime;

	hdr->if_id   = client->bnd.if_uuid;
	hdr->if_vers = CLRPC_MAKE_IF_VERS(client->bnd.if_version_major, client->bnd.if_version_minor);

	hdr->opnum = 0;

	hdr->object = client->bnd.object_uuid;

	hdr->fragnum = 0;

	hdr->ihint = client->bnd.ihint;
	hdr->ahint = client->bnd.ahint;

	hdr->serial_hi = (LSA_UINT8)(client->txq.serial >> 8);
	hdr->serial_lo = (LSA_UINT8)client->txq.serial;

	hdr->seq = client->bnd.actseq;

	hdr->auth_proto = 0;

	hdr->len = 0;

	return hdr;
}

/*=============================================================================
//=== SEND ====================================================================
//===========================================================================*/

static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_send(
	CLRPC_CLIENT_PTR_TYPE client,
	CLRPC_LOWER_RQB_PTR_TYPE lower,
	CLRPC_HEADER_PTR_TYPE hdr,
	LSA_UINT16 port
) {
	CLRPC_LOG_FCT("clrpc_cl_send")
	CLRPC_LL_UDP_DATA_PTR_TYPE param;

	CLRPC_ASSERT(client);
	CLRPC_ASSERT(lower);
	CLRPC_ASSERT(client->channel->handle_lower != CLRPC_LL_INVALID_HANDLE);

	CLRPC_ASSERT(CLRPC_LL_GET_OPCODE(lower) == SOCK_OPC_UDP_SEND); /* see clrpc_bind_delayed() */
	//CLRPC_LL_SET_HANDLE(lower, client->channel->handle_lower);
	CLRPC_ASSERT(CLRPC_LL_GET_USER_ID_PTR(lower) == client); /* see clrpc_bind_delayed() */

	param = &lower->args.data;

	CLRPC_ASSERT(hdr == (CLRPC_HEADER_PTR_TYPE)param->buffer_ptr); /* sanity */

	param->sock_fd = client->channel->cl.lower.sock_fd;

	if( ! client->bnd.bound ) {
		CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "using 'unbound' host/port"
			);
		param->rem_addr.sin_family = SOCK_AF_INET;
		param->rem_addr.sin_addr.S_un.S_addr = client->bnd.host_ip;
		param->rem_addr.sin_port = client->bnd.udp_port;
	}
	else {
		param->rem_addr.sin_family = SOCK_AF_INET;
		param->rem_addr.sin_addr.S_un.S_addr = client->bnd.serverHost;
		param->rem_addr.sin_port = client->bnd.serverPort;
	}

	if( port != 0 ) {
		param->rem_addr.sin_port = port;
	}

	CLRPC_ASSERT(param->rem_addr.sin_addr.S_un.S_addr != 0);
	CLRPC_ASSERT(param->rem_addr.sin_port != 0);

	CLRPC_ASSERT(hdr->len <= CLRPC_MUST_RECV_BODY_SIZE);

	param->data_length = CLRPC_HEADER_SIZE + hdr->len;

	CLRPC_ASSERT(param->buffer_length == CLRPC_BUFFER_SIZE);
	CLRPC_ASSERT(param->data_length <= param->buffer_length);

	CLRPC_CL_TRACE_06(client->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> client(%08x) --- SEND.req --- lower(%08x) ip(%08x) port(%u)  ptype(%u) bodylen(%u)"
		, client
		, lower
		, CLRPC_NTOHL(param->rem_addr.sin_addr.S_un.S_addr)
		, CLRPC_NTOHS(param->rem_addr.sin_port)
		, hdr->ptype
		, hdr->len
		);

	{
		CLRPC_HEADER_PTR_TYPE h = (CLRPC_HEADER_PTR_TYPE)lower->args.data.buffer_ptr;
		LSA_UNUSED_ARG (h);

		CLRPC_CL_PKT_TRACE_08(client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "=== SEND === ip(%08x) port(%u)  server_boot(%u)  serial(%u)  fragnum(%u)  drep0_1_2(%#x %#x %#x)"
			, CLRPC_NTOHL(param->rem_addr.sin_addr.S_un.S_addr)
			, CLRPC_NTOHS(param->rem_addr.sin_port)
			, h->server_boot, ((h->serial_hi << 8) | h->serial_lo), h->fragnum
			, h->drep[0], h->drep[1], h->drep[2]
			);
		CLRPC_CL_PKT_TRACE_11 (client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND ptype(%u)  opnum(%u)  len(%u)  flags(%#x:%c%c%c%c%c%c)  flags2(%#x)"
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
		CLRPC_CL_PKT_TRACE_12 (client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)  actseq(%u)"
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
		CLRPC_CL_PKT_TRACE_12 (client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND ifuuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)   vers=0x%08x"
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
		CLRPC_CL_PKT_TRACE_11 (client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND objuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
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

    clrpc_request_lower(lower, SOCK_OPC_UDP_SEND, client);
}

/*=============================================================================
//=== SEND FACK ===============================================================
//===========================================================================*/

static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_send_fack(
	CLRPC_CLIENT_PTR_TYPE client
) {
	CLRPC_LOWER_RQB_PTR_TYPE sndlower;
	CLRPC_HEADER_PTR_TYPE sndhdr;
	CLRPC_FACK_BODY_PTR_TYPE body; /* see packing-checks in clrpc_check_sizes() */

	CLRPC_ASSERT(client);
	CLRPC_ASSERT(clrpc_queue_not_empty(&client->sendCnfQ)); /* caller must have a send-resource! */

	sndlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->sendCnfQ);
	CLRPC_ASSERT(sndlower);

	sndhdr = clrpc_cl_prepare(client, sndlower, CLRPC_PT_FACK);

	sndhdr->fragnum = client->rxq.nextFragnum - 1;

	/*
	 * note: a fack-body is not needed.
	 *       but then the window size cannot be controlled
	 */

	body = (CLRPC_FACK_BODY_PTR_TYPE)(sndhdr+1);

	body->vers = CLRPC_FACK_BODY_VERS;
	body->pad1 = 0;
	body->window_size = CLRPC_MAX_WINDOW_SIZE;
	body->max_tsdu = CLRPC_MUST_RECV_FRAG_SIZE;
	body->max_frag_size = CLRPC_MUST_RECV_FRAG_SIZE;
	body->serial_num = client->rxq.highSerial;
	body->selack_len = 0;
	body->selack[0] = 0;

	sndhdr->len = CLRPC_FACK_BODY_SIZE;

	clrpc_cl_send(client, sndlower, sndhdr, 0);
	/* note: a fack does not increment the serial number */
}

/*=============================================================================
//=== SEND PING ===============================================================
//===========================================================================*/

static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_send_ping(
	CLRPC_CLIENT_PTR_TYPE client
) {
	CLRPC_LOWER_RQB_PTR_TYPE sndlower;
	CLRPC_HEADER_PTR_TYPE sndhdr;

	CLRPC_ASSERT(client);
	CLRPC_ASSERT(clrpc_queue_not_empty(&client->sendCnfQ)); /* caller must have a send-resource! */

	sndlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->sendCnfQ);
	CLRPC_ASSERT(sndlower);

	sndhdr = clrpc_cl_prepare(client, sndlower, CLRPC_PT_PING);

	clrpc_cl_send(client, sndlower, sndhdr, 0);

	client->txq.serial += 1; /* increment serial on each REQUEST, RESPONSE or PING */
}

/*=============================================================================
//=== SEND ACK ================================================================
//===========================================================================*/

static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_send_ack(
	CLRPC_CLIENT_PTR_TYPE client
) {
	CLRPC_LOWER_RQB_PTR_TYPE sndlower;
	CLRPC_HEADER_PTR_TYPE sndhdr;

	CLRPC_ASSERT(client);
	CLRPC_ASSERT(clrpc_queue_not_empty(&client->sendCnfQ)); /* caller must have a send-resource! */

	sndlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->sendCnfQ);
	CLRPC_ASSERT(sndlower);

	sndhdr = clrpc_cl_prepare(client, sndlower, CLRPC_PT_ACK);

	clrpc_cl_send(client, sndlower, sndhdr, 0);
	/* note: an ack does not increment the serial number */
}

/*=============================================================================
//=== SEND QUIT ===============================================================
//===========================================================================*/

static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_send_quit(
	CLRPC_CLIENT_PTR_TYPE client
) {
	CLRPC_LOWER_RQB_PTR_TYPE sndlower;
	CLRPC_HEADER_PTR_TYPE sndhdr;

	CLRPC_ASSERT(client);
	CLRPC_ASSERT(clrpc_queue_not_empty(&client->sendCnfQ)); /* caller must have a send-resource! */

	sndlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->sendCnfQ);
	CLRPC_ASSERT(sndlower);

	sndhdr = clrpc_cl_prepare(client, sndlower, CLRPC_PT_QUIT);

	sndhdr->len = 0; /* orphan-quit (not cancel-quit!) */

	clrpc_cl_send(client, sndlower, sndhdr, 0);
	/* note: a quit does not increment the serial number */
}

/*=============================================================================
//=== CALL COMPLETE ===========================================================
//===========================================================================*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_call_complete(
	CLRPC_CLIENT_PTR_TYPE client,
	LSA_UINT16 response
) {
	CLRPC_LOG_FCT("clrpc_cl_call_complete")
	CLRPC_ASSERT(client);
	CLRPC_ASSERT(client->call.curr);

	CLRPC_CL_TRACE_03(client->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rsp(%u) nca_status(%u), state(%u)"
		, response
		, client->call.nca_status
		, client->state
		);

	/***/

	clrpc_oneshot_stop(&client->oneshot);

	client->timeoutFlag = 0;

	/***/

	if( response != CLRPC_OK || (client->txq.baseFlags & CLRPC_PF_BROADCAST) ) {
		/*
		 * don't use a failed host/port again, the server may have been rebootet
		 *
		 * note: broadcasts are allowed on "broadcast-bindings" only. see callQ.
		 *
		 */

		client->bnd.bound = 0;

		client->bnd.hintsUpdated = 0;
		client->bnd.ahint = CLRPC_NO_HINT;
		client->bnd.ihint = CLRPC_NO_HINT;
	}

	/***/

#ifdef CLRPC_MESSAGE
/*	VV: shouldn't we return responding server/port too? especially when broadcast */
#endif /* CLRPC_MESSAGE */

	client->call.curr->args.cl.call->nca_status = client->call.nca_status;

	/***/

	switch( CLRPC_RQB_GET_OPCODE(client->call.curr) ) {
	case CLRPC_OPC_CL_QUERY_EPM_NEXT:
		response = clrpc_cl_query_epm_next_done(response, client->call.curr);
		break;

	case CLRPC_OPC_CL_QUERY_EPM_BREAK:
		response = clrpc_cl_query_epm_break_done(response, client->call.curr);
		break;

	default:
		CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(client->call.curr) == CLRPC_OPC_CL_CALL);
		break;
	}

	/***/

	if( response == CLRPC_OK && client->call.curr->args.cl.call->pnio_signature ) {
		/*wrong: CLRPC_ASSERT(client->bnd.bound); // case: misbehaving server (AP00304773) */
		response = clrpc_cl_pnio_demarshal(client, client->call.curr);
	}

	/***/

	clrpc_callback(client->channel, response, &client->call.curr, CLRPC_MODULE_ID, __LINE__);

	client->call.curr = LSA_NULL;

	if( client->rxq.recvFragmented ) {
		/* must go into final-state or server may block resources for a while */
		client->state = CLRPC_CL_FINAL;
		clrpc_oneshot_trigger(&client->oneshot, CLRPC_FINAL_TIMEOUT_MSEC);
	}
	else if( (client->txq.baseFlags & (CLRPC_PF_IDEMPOTENT | CLRPC_PF_MAYBE)) ) {
		/* NOTE: Microsoft sends sends ACKs even if idempotent */
		/* sending an ACK on MAYBE causes Microsoft to reply a NOCALL... */
		client->state = CLRPC_CL_IDLE;
	}
	else {
		/* must go into final-state or server may block resources for a while */
		client->state = CLRPC_CL_FINAL;
		clrpc_oneshot_trigger(&client->oneshot, CLRPC_FINAL_TIMEOUT_MSEC);
	}
}

/*=============================================================================
//=== CLIENT STATEMACHINE =====================================================
//===========================================================================*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_statemachine(
    CLRPC_CLIENT_PTR_TYPE client
) {
    CLRPC_LOG_FCT("clrpc_cl_statemachine")

    CLRPC_ASSERT(client);

    CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);

    /*=========================================================================
    === another conversation manager request ==================================
    =========================================================================*/

    while( clrpc_queue_not_empty(&client->sendCnfQ) && clrpc_queue_not_empty(&client->convQ) ) {

        CLRPC_LOWER_RQB_PTR_TYPE rcvlower;
        CLRPC_HEADER_PTR_TYPE rcvhdr;

        CLRPC_LOWER_RQB_PTR_TYPE sndlower;
        CLRPC_HEADER_PTR_TYPE sndhdr;

        CLRPC_WAY_REQ_PTR_TYPE req; /* see packing-checks in clrpc_check_sizes() */
        LSA_UINT32 ncastat;

        /*
         * note that we don't need a current call to answer a conv-man request!
         */

        rcvlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->convQ);
        CLRPC_ASSERT(rcvlower);

        rcvhdr = (CLRPC_HEADER_PTR_TYPE)rcvlower->args.data.buffer_ptr;
        CLRPC_ASSERT(rcvhdr);

        CLRPC_ASSERT(rcvhdr->len == CLRPC_WAY_REQ_SIZE); /* if() in caller */

        req = (CLRPC_WAY_REQ_PTR_TYPE)(rcvhdr+1);

        CLRPC_CL_TRACE_04(client->trace_idx, LSA_TRACE_LEVEL_CHAT
            , "convQ: WAY%u request, boot_time(%u) (known boot_time(%u)), state(%u)..."
            , rcvhdr->opnum + 1 /* well... */
            , req->boot_time
            , client->bnd.serverBootTime
            , client->state
        );

        /* set up for a response or reject */

        sndlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->sendCnfQ);
        CLRPC_ASSERT(sndlower);

#if CLRPC_DEBUG
        CLRPC_MEMSET(sndlower->args.data.buffer_ptr, 0xCC, sndlower->args.data.buffer_length); /* to find uninitialized things */
#endif

        sndhdr = (CLRPC_HEADER_PTR_TYPE)sndlower->args.data.buffer_ptr;
        CLRPC_ASSERT(sndhdr);

        /* fill in the header */

        sndhdr->rpc_vers = CLRPC_HEADER_VERSION;
        sndhdr->ptype    = CLRPC_PT_RESPONSE;
        sndhdr->flags    = 0; /* see DG.H line 410, comment on flags */
        sndhdr->flags2   = 0;

        sndhdr->drep[0] = CLRPC_DREP_0;
        sndhdr->drep[1] = CLRPC_DREP_1;
        sndhdr->drep[2] = CLRPC_DREP_2;

        sndhdr->actuid = rcvhdr->actuid; /* use the activity uuid from the request */

        sndhdr->server_boot = 1; /* "client boot time", but WAY is idempotent, 1 because only EPM may return 0... */

        /*--- these may contain garbage ---
        //sndhdr->if_id = clrpc_uuid_nil_uuid;
        //sndhdr->if_version = 0;
        //sndhdr->opnum = 0;
        //sndhdr->object = clrpc_uuid_nil_uuid;
        */

        sndhdr->fragnum = 0;

        sndhdr->ihint = CLRPC_NO_HINT;
        sndhdr->ahint = CLRPC_NO_HINT;

        sndhdr->serial_hi = sndhdr->serial_lo = 0;

        sndhdr->seq = rcvhdr->seq; /* use the activity sequence from the request*/

        sndhdr->auth_proto = 0;

        /***/

#if 0 /* "client boot time" see comment above */
        if( rcvhdr->server_boot != 0 && rcvhdr->server_boot != clrpc_cl_boot_time ) {

            CLRPC_ERROR_BODY_PTR_TYPE rsp = (CLRPC_ERROR_BODY_PTR_TYPE)(sndhdr+1); /* see packing-checks in clrpc_check_sizes() */

            /*
             * OSF sources DGLSN.C line 3019
             * check boot time
             * send REJECT "nca_s_wrong_boot_time" if mismatch
             */

            CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "convQ: boot time mismatch, send reject wrong-boot-time'"
                );

            sndhdr->ptype = CLRPC_PT_REJECT;

            rsp->nca_status = CLRPC_NCA_WRONG_BOOT_TIME;

            sndhdr->len = sizeof(*rsp);
        }
        else
#endif /* "client boot time" */
        {
            /*
             * OSF sources, CONV.C line 118
             */

            ncastat = 0;

            if( client->bnd.serverBootTime == 0 ) {
                client->bnd.serverBootTime = req->boot_time;
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "convQ: set server-boot-time(%u)", client->bnd.serverBootTime
                    );
            }
            else {
                /* may be already set by a received FACK */

                if( client->bnd.serverBootTime != req->boot_time ) {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "convQ: wrong server-boot-time. responding with 'you crashed'"
                        );
                    ncastat = CLRPC_NCA_YOU_CRASHED;
                }
                else {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "convQ: server-boot-time already known"
                        );
                    /*CLRPC_ASSERT(client->bnd.bound); not true! see recvQ */
                }
            }

            /* add a WAY- or WAY2-body */

            if( rcvhdr->opnum == 0 ) {

                CLRPC_WAY_RSP_PTR_TYPE rsp = (CLRPC_WAY_RSP_PTR_TYPE)(sndhdr+1); /* see packing-checks in clrpc_check_sizes() */

                rsp->actseq  = client->bnd.actseq;
                rsp->ncastat = ncastat;

                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "convQ: responding WAY, actseq(%u)", rsp->actseq
                    );

                sndhdr->len = sizeof(*rsp);
            }
            else {

                CLRPC_WAY2_RSP_PTR_TYPE rsp = (CLRPC_WAY2_RSP_PTR_TYPE)(sndhdr+1); /* see packing-checks in clrpc_check_sizes() */

                CLRPC_ASSERT(rcvhdr->opnum == 1); /* see caller */

                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "convQ: responding WAY2, actseq(%u)", client->bnd.actseq
                    );

                rsp->actseq  = client->bnd.actseq;
                rsp->casuid  = client->channel->cl.cas_uuid;

                rsp->ncastat = ncastat;

                sndhdr->len = sizeof(*rsp);
            }
        }

        /* the way request may come from another socket! (but from the same host...) */

        clrpc_cl_send(client, sndlower, sndhdr, rcvlower->args.data.rem_addr.sin_port);
        /* don't increment serial, this is another activity!!! */

        clrpc_cl_provide(client->channel, rcvlower);

    } /* endwhile convQ */

    /*=========================================================================
    === another receive indication ============================================
    =========================================================================*/

    while( clrpc_queue_not_empty(&client->sendCnfQ) && clrpc_queue_not_empty(&client->recvQ) ) {

        CLRPC_LOWER_RQB_PTR_TYPE rcvlower;
        CLRPC_HEADER_PTR_TYPE rcvhdr;

        rcvlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->recvQ);
        CLRPC_ASSERT(rcvlower);

        rcvhdr = (CLRPC_HEADER_PTR_TYPE)rcvlower->args.data.buffer_ptr;
        CLRPC_ASSERT(rcvhdr);

        CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_CHAT
            , "recvQ: another packet ptype(%u), state(%u)", rcvhdr->ptype, client->state
            );

        CLRPC_ASSERT(CLRPC_PT_IS_STOC(rcvhdr->ptype)); /* or a bug in caller... */

        /*
         * received a packet of server-to-client type
         * (RESPONSE, FAULT, WORKING, NOCALL, REJECT, QUACK and FACK)
         *
         * we tried to keep the logic as close as possible to the OSF sources.
         *
         * however, there are some differences:
         *
         *  - this code is for the client only
         *
         *    OSF sources don't differentiate, which makes things more complicated
         *
         *  - we don't implement callback
         *
         *    1st) because we never saw a definition of what a callback is, and
         *    2nd) because OSF sources DGLSN.C line 2405: "... aren't supporting callbacks anyway"
         *
         *  - our client does no "proxy-calls" (conversation manager)
         *
         *    OSF sources do proxy-calls by means of an "associated" client or server.
         *    maybe things were designed in this (academically) way, because it is mentioned
         *    in DGLSN.C line 2356 that there are callbacks, but "good thing callbacks are not supported"
         *
         *    OSF sources DGCLSN.C line 125, comment on "proxy-calls"
         *    the comment says, that sequence can be greater due to proxy-calls,
         *    but our client does no proxy-calls, so we compare for "equal"
         *    rather than for "less than".
         *
         *  - when a call can complete, it will be completed immediately
         *
         *    OSF sources use another threading model, which makes things more complicated
         *
         *  - fragment size is not adapted
         *
         *    since we support UDP only, we simply never send fragments larger than MUST_RECV_SIZE
         *
         */

        /* OSF sources DGLSN.C line 3151, comment
         *
         * ... perform server boot time and packet sequence filtering.  It is important
         * to note the order in which the filtering is done... In server to client handlers
         * packet sequence is checked first to 'weed out' old packets then server boot time
         * is checked. This is done to prevent old packets from the wrong server instance
         * from causing faults to be signalled.
         */

#ifdef CLRPC_MESSAGE
/* soll wie bei OSF sourcen, eventuell ein ACK gesendet werden?
 * Microsoft Server verhalten sich eh ruhig
 * d.h. der Code wird praktisch nie benötigt
 */
#endif /* CLRPC_MESSAGE */

        if( rcvhdr->ptype == CLRPC_PT_NOCALL ) {

            /*
             * NOCALL is different from the other packet types!
             *
             * OSF sources DGLSN.C line 3137
             * NOCALL is *not* an indication of progress!
             */

            /* OSF sources DGCLSN.C line 641
             *
             * NOCALLs are received in response to pings.
             * Note that in the case of large INs, some of which have already been
             * fack'd, NOCALL simply means that not all the INs have arrived, NOT
             * that the server knows nothing about the call.  Confusing, eh?
             */

            CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: got NOCALL, fragnum(%u) state(%u)", rcvhdr->fragnum, client->state
                );

            /* OSF sources DGCLSN.C line 668
             * Make sure the packet is for the current call, that we are in the
             * receive state, and that the call has not already failed.
             *
             * NOTE: we complete immediately on failure
             *       thus the case "already failed" is caught by "no current call"
             */

            if( CLRPC_PTR_IS_NULL(client->call.curr) /* no current call */
                || rcvhdr->seq != client->bnd.actseq /* or old, see comment on "proxy-calls" */
                || client->state != CLRPC_CL_RECV /* or not in recv-state */
             ) {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: nocall, no current call, wrong sequence or not in recv-state, dropped..."
                    );
                goto provide_and_continue;
            }

            CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(client->call.curr));

            /* OSF sources DGCLSN.C line 687
             * drop if server-boot-time not equal
             */

            if( client->bnd.serverBootTime != 0 && client->bnd.serverBootTime != rcvhdr->server_boot ) {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: nocall, wrong server-boot-time, dropped..."
                    );
                goto provide_and_continue;
            }

            /* OSF sources DGCLSN.C line 695
             * when in the orphan state, a nocall is just as good as a quack
             */

            if( client->state == CLRPC_CL_ORPHAN ) {
                clrpc_cl_call_complete(client, CLRPC_OK_CANCELLED);
                goto provide_and_continue;
            }

            /* OSF sources DGCLSN.C line 705
             * strange situation: nocall and txQ empty, thus nothing to retransmit
             * maybe a late (misrouted) packet
             *
             * NOTE: since we don't send want-fack on last frag, "all acked" means that we got a response
             *       if we got the whole response, we did complete
             *       we cannot retransmit after receiving a response, because the response did overwrite the in-params!
             */

            if( client->rxq.rcvIdx != 0 /* in-params overwritten by out-params */
                || (
                    client->call.curr->args.cl.call->ndr_len != 0
                    && client->txq.ackIdx >= client->call.curr->args.cl.call->ndr_len /* everything acked */
                    )
            ) {
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: nocall, cannot/nothing to retransmit (server bug?) rcvIdx(%u)"
                    , client->rxq.rcvIdx
                    );
                goto provide_and_continue;
            }

            /* OSF sources DGCLSN.C line 713
             * nocall means "stop pinging"
             */

            /* OSF sources DGCLSN.C line 719
             * if there is a body, it must be fack-body
             */

            if( rcvhdr->len == CLRPC_FACK_BODY_SIZE ) {

                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "recvQ: nocall with fack-body..."
                    );

                goto fack_common;
            }

            /* nocall that lacks a fack body */

            /* OSF sources DGCLSN.C line 759
             * If this is an idempotent call (which must mean a single pkt
             * request, though we won't count on that) and we're retransmitting
             * the beginning of a call, then it may have been the case that the
             * call actually ran, but the response was lost.
             */

            /* OSF source fiddles with "cancel" state.
             *
             * NOTE: we do quit only, not cancel
             */

            /* OSF sources DGCLSN.C line 778
             * Since we had begun pinging, we know that everything on the queue
             * has been sent at least once.  However, the receipt of a nocall
             * is an indication from the server that it's missing something.
             * Unfortunately, we have no way of telling what it's missing, so
             * all we can do is begin sending everything on the queue again.
             */

            CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: nocall without body, retransmit everything"
                );

            CLRPC_ASSERT(client->rxq.rcvIdx == 0); /* see above */

            client->txq.sndIdx = client->txq.ackIdx = 0;
            client->txq.sndWndCurr = 0;
            client->txq.sndWndMax = 1; /* PNIO: slow-start */

            client->state = CLRPC_CL_SEND; /* note: rtx-counter is NOT reset */

            goto provide_and_continue;

        } /* endif NOCALL */

        /*
         * begin of "common response"
         */

        /* OSF sources DGCLSN.C line 125, comment on when to send an ACK
         * Make sure the packet is for the current call.  Note: the sequence
         * number may be greater than our current one due to proxy calls.
         *
         * Response packet considerations:
         * If we're in a call and this response is for an old call, send
         * an "ack" just in case the activity ID has been reused on a call
         * to a different server.  If the response is to a previous call
         * then assume that an "ack" was lost and resend it.  (Note that
         * we will only resend an ACK on receipt of the last, or only, fragment
         * of a response;  we do this because when an ACK is lost, the server
         * will retransmit the last blast's worth of data and we don't want
         * to send ACKs for each one.)  Duplicate "ack"s are not a problem
         * because servers will simply ignore them. Note that the seq number
         * can actually be greater than the current call's sequence due to
         * proxy calls.
         *
         * Fault packet considerations:
         * If the fault is to a previous call it may be a duplicate, however,
         * it may also be a valid fault packet that has not been "ack"ed.
         * Valid fault packets may result from activity ID's being rapidly
         * reused after an "ack" has been lost.  Also, the packet may be
         * for this call and we may already be in an error state which may
         * have been because we've already received a fault but for some
         * reason we haven't completed the call yet.  The proper action in
         * all three of the above cases is to send an "ack" to stop the server
         * from resending the fault.  Any extra "ack"s will be ignored by
         * the server.  Note that the seq number can actually be greater
         * than the current call's sequence due to proxy calls.
         */

/*
 * test! does Microsoft server retransmit RESP or FAULTs?
 */

        if( CLRPC_PTR_IS_NULL(client->call.curr) /* no current call */
            || rcvhdr->seq != client->bnd.actseq /* or old, see comment on "proxy-calls" */
            /* case: "fault is a duplicate" impossible, is caught by "no current call" */
         ) {
            if( rcvhdr->ptype == CLRPC_PT_FAULT
                || (rcvhdr->ptype == CLRPC_PT_RESPONSE
                        && ( ! (rcvhdr->flags & CLRPC_PF_FRAG) /* the only frag */
                                || (rcvhdr->flags & CLRPC_PF_LAST_FRAG) /* the last frag */
                            )
                    )
            ) {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: old fault or response, SHOULD (but not doing yet) sending ACK, just dropped..."
                    );
/*
 * test! reply an ACK.
 * for broadcast answers too? they don't need it, but how to identify?
 * reply may not be for the 'bound' host!
 * to do! check if host is 255.255.255.255?
 */

            }
            else {
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: old packet ptype(%u), dropped..."
                    , rcvhdr->ptype
                    );
            }
            goto provide_and_continue;
        }

        CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(client->call.curr));

        if( rcvhdr->ptype == CLRPC_PT_WORKING && client->state != CLRPC_CL_RECV ) {
/*
 * test! which situation is covered by this?
 * has to do with orphan handling below?
 */
            CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                , "recvQ: working but not in recv-state, dropped..."
                );
            goto provide_and_continue;
        }

        /* OSF sources DGCLSN.C line 177
         * If the call has been orphaned, our interest in various pkt types
         * is lessened.
         */

        if( client->state == CLRPC_CL_ORPHAN
            && (rcvhdr->ptype == CLRPC_PT_RESPONSE || rcvhdr->ptype == CLRPC_PT_FACK)
        ) {
/*
 * test! has to do working above?
 * wait for quack?
 */
            CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: orphan-state, ignoring response and fack, dropped..."
                );
            goto provide_and_continue;
        }

        /* OSF sources DGCLSN.C line 191
         * If all the packets has been received for this call, we don't have to
         * do any more work here (in particular, we don't want to check the boot
         * time since in the case of broadcast, we don't want to get upset by
         * responses from other servers which will of course have different
         * boot times).
         *
         * NOTE: we complete immediately
         *       thus this case is caught by "no current call" above
         */

        /* OSF sources DGCLSN.C line 201
         * if wrong server boot time, drop this one
         */

        if( client->bnd.serverBootTime != 0 && client->bnd.serverBootTime != rcvhdr->server_boot ) {
            CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: wrong server-boot-time, dropped..."
                );
            goto provide_and_continue;
        }

        /* OSF sources DGCLSN.C line 207
         * There are several interesting and / or necessary pieces of
         * information that we need to snag from a response packet header.
         * Pick up the hints and the response sequence number (to properly
         * manage our sequence number space in the presence of proxy calls).
         *
         * Now that we've successfully "(re)connected" to the server we also
         * reset the call's com_timeout_knob to be no lower than the default
         * timeout level.
         */

        if( ! client->bnd.hintsUpdated ) { /* flag is reset on every call */
            client->bnd.hintsUpdated = 1;
            client->bnd.ahint = rcvhdr->ahint;
            client->bnd.ihint = rcvhdr->ihint;

            /*
             * OSF sources set the "total timeout" here
             *
             * PNIO: retry-max and retry-interval
             */
        }

        /* OSF sources DGCLSN.C line 228
         *
         * picking up hig_seq not needed, see comment on "proxy-calls" above
         */

        /* OSF sources DGCLSN.C line 240
         * Update our server address binding if necessary.  Also,
         * if we're are updating the remote address ... determine
         * the max_path_tpdu for this path.
         */

        if( ! client->bnd.bound ) { /* "bound" is reset on broadcasts and errors */

            if( rcvhdr->server_boot != 0 ) {

                client->bnd.bound = 1;

                client->bnd.serverHost = rcvlower->args.data.rem_addr.sin_addr.S_un.S_addr;
                client->bnd.serverPort = rcvlower->args.data.rem_addr.sin_port;

                if( client->bnd.serverBootTime != 0 ) {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "recvQ: completing binding, server-boot-time already known"
                        );
                    CLRPC_ASSERT(client->bnd.serverBootTime == rcvhdr->server_boot);/* see some lines above */
                }

                client->bnd.serverBootTime = rcvhdr->server_boot;

                CLRPC_CL_TRACE_03(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "recvQ: completing binding, host(%08x) port(%u) boot-time(%u)"
                    , CLRPC_NTOHL(client->bnd.serverHost)
                    , CLRPC_NTOHS(client->bnd.serverPort)
                    , client->bnd.serverBootTime
                    );

                /*
                 * since we support UDP only, we don't (and must not!) change the frag-size here
                 */
            }
            else {
                /*
                 * the ep-mapper sends reject packets with boot-time 0
                 *
                 * since this is not the server responding, so don't bind to it!
                 */
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: got boot-time==0, a reject from ep-mapper? (ptype(%u))"
                    , rcvhdr->ptype
                    );
            }
        }

        /* OSF sources DGCLSN.C line 267
         * they fiddle with CLRPC_PF2_CANCEL_PENDING flag
         *
         * NOTE: we do quit only, not cancel
         */

        /*
         * end of "common response"
         */

        if( rcvhdr->ptype == CLRPC_PT_FACK ) {
            LSA_UINT16 ackFragnum;
            LSA_UINT32 ackIdx;

            /* OSF sources DGLSN.C line 2326, do_fack()
             * comment on line 2390: "...pretty horrible, eh? Who designed this protocol anyway?"
             *
             * see the comment on callbacks above. this makes things easier.
             */

            CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: got FACK for fragnum(%u)", rcvhdr->fragnum
                );

fack_common:

            /* OSF sources DGLSN.C line 2480, do_fack_common()
             *
             * note that we don't implement selective-acknowledge
             *
             * remove the acked fragments from the rtx-queue
             * and setup for the (re-) transmission of more fragments
             *
             * note that fragment numbers can wrap around
             * thus all calculations need to be done "relative"
             */

            ackFragnum = CLRPC_CAST_U16(client->txq.ackIdx / client->txq.sendSize); /* may wrap around */

            if( CLRPC_FRAGNUM_IS_LTE(ackFragnum, rcvhdr->fragnum) ) {

                ackIdx = client->txq.ackIdx + (rcvhdr->fragnum - ackFragnum + 1) * client->txq.sendSize;

                CLRPC_ASSERT(ackIdx > client->txq.ackIdx);

                if( ackIdx <= client->txq.sndIdx ) {

                    client->txq.ackIdx = ackIdx;

                    CLRPC_CL_TRACE_03(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "recvQ: fack, %u fragments acked (%u/%u bytes)"
                        , (rcvhdr->fragnum - ackFragnum + 1)
                        , ackIdx
                        , client->call.curr->args.cl.call->ndr_len
                        );

                    CLRPC_ASSERT(client->txq.sndWndCurr >= (rcvhdr->fragnum - ackFragnum + 1));

                    client->txq.sndWndCurr -= (rcvhdr->fragnum - ackFragnum + 1);

                    client->call.rtxCnt = 0; /* reset, this is a progress */

/* test! retrigger here? or may a bogus server hang the client? */
                }
                else {
                    /* never ack more than sent (very late ack and already retransmitting?) */
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "recvQ: fack, cannot ack more than sent!"
                        );
                }
            }
            else {
                CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "recvQ: fack for fragnum(%u) is old (ack(%u))"
                    , rcvhdr->fragnum
                    , ackFragnum
                    );
            }

            /*
             * OSF sources DGLSN.C line 2164 do_fack_body()
             * ==> window, frag-size and sel-ack
             *
             * Microsoft server advertise
             *  window_size = 32
             *  max-tsdu = 4096
             *  max-frag-size = 1472
             */

            if( rcvhdr->len == CLRPC_FACK_BODY_SIZE ) {

                CLRPC_FACK_BODY_PTR_TYPE body = (CLRPC_FACK_BODY_PTR_TYPE)(rcvhdr+1); /* see packing-checks in clrpc_check_sizes() */

                if( CLRPC_DREP_INT_DIFFERS(rcvhdr->drep[0]) ) {

                    CLRPC_SWAP_U16(&body->window_size);
                    CLRPC_SWAP_U32(&body->max_tsdu);
                    CLRPC_SWAP_U32(&body->max_frag_size);
                    CLRPC_SWAP_U16(&body->serial_num);

                    /*
                     * selack not converted because we don't use it!!!
                     */
                }

                if( body->vers == CLRPC_FACK_BODY_VERS ) {

                    CLRPC_CL_TRACE_04(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "recvQ: fack-body, wnd-size(%u) max-tsdu(%u) max-frag-size(%u) serial(%u)"
                        , body->window_size
                        , body->max_tsdu
                        , body->max_frag_size
                        , body->serial_num
                        );

                    /*
                     * window-size is counted in kilo-bytes, not fragments!
                     * the sender may close the window (size = 0)!
                     *
                     * NOTE: OSF sources do some complicated calculations
                     *
                     * NOTE: PNIO uses fixed CLRPC_MAX_WINDOW_SIZE
                     *       and CLRPC_MAX_NOFACK_SIZE, see comments there
                     */

#if CLRPC_MAX_WINDOW_SIZE != 5 || CLRPC_MAX_NOFACK_SIZE != 3
# error "who changed that?"
#endif

                    client->txq.sndWndMax =
                        body->window_size >= CLRPC_MAX_WINDOW_SIZE /* lot of space */
                            ? CLRPC_MAX_NOFACK_SIZE /* see comment there */
                            : body->window_size != 0 /* not closed */
                                ? 2 /* trade-off, see OSF sources DGCALL.C line 459 */
                                : 1; /* we do not allow to close the window */

                    CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "recvQ: fack, new send-window(%u) fragments", client->txq.sndWndMax
                        );

                    if( rcvhdr->ptype == CLRPC_PT_NOCALL ) { /* nocall with fack-body means that the last fragment got lost */

                        /* AP00654150 (client did not retransmit the last fragment) */

                        if( client->state == CLRPC_CL_RECV ) { /* sanity */
                            if( client->rxq.rcvIdx == 0 ) { /* sanity (in-params still valid, see above) */

                                client->txq.sndIdx = client->txq.ackIdx;
                                client->txq.sndWndCurr = 0; /* open the send window */
                                client->txq.sndWndMax = 1; /* slow-start */

                                client->state = CLRPC_CL_SEND; /* back to send state */
                            }
                        }
                    }
                }
                else {
                    CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                        , "recvQ: wrong fack-body-version(%u)"
                        , body->vers
                        );
                }
            }
            else {
#if CLRPC_DEBUG
                if( rcvhdr->len == 0 ) {
                    /* CLRPC server does not send a body (which is ok!) */
                }
                else {
                    CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                        , "recvQ: wrong fack-body-size(%u)"
                        , rcvhdr->len
                        );
                }
#endif
            }

            /* start a timer, send-window may still be full or was closed */

            clrpc_oneshot_trigger(&client->oneshot, CLRPC_RTX_TIMEOUT_MSEC);
        }
        else if( rcvhdr->ptype == CLRPC_PT_RESPONSE ) {
            if( client->state == CLRPC_CL_RECV ) {
                LSA_UINT16 serial;

                /*
                 * OSF sources DGCLSN.C line 435
                 */

                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "recvQ: got RESPONSE"
                    );

                /* OSF sources DGCLSN.C line 470
                 * Free any packets we have on the transmit queue still since the
                 * response is (implicitly) ack'ing them.
                 */

                client->txq.ackIdx = client->txq.sndIdx = client->call.curr->args.cl.call->ndr_len;

                /* OSF sources DGCLSN.C line 478
                 * If we were pinging the server, we can stop now.
                 */

                /* nothing to do here */

                /* OSF sources DGCLSN.C line 484
                 * Add the packet to the receive queue.
                 *
                 * DGCALL.C line 1168, rpc__dg_call_recvq_insert()
                 * Insert a receive queue element (packet) into a receive queue in the
                 * right place.  Drop duplicate packets and generate a "fack" if one
                 * is requested (even for duplicates).
                 */

                /* DGCALL.C line 1200
                 * Remember if we're recving frags.  To insulate us from potentially
                 * bogus transmitters, once we're recving frags we stay that way.
                 * It's cheaper and more effective to do this than to "assert" it.
                 */

                client->rxq.recvFragmented |= (rcvhdr->flags & CLRPC_PF_FRAG);

                /* DGCALL.C line 1207
                 * The receiver needs to keep track of the highest serial number it
                 * has seen so far, just in case it ever needs to send a fack but
                 * isn't holding onto an rqe at the time.
                 */

                serial = (rcvhdr->serial_hi << 8) | rcvhdr->serial_lo;

                if( CLRPC_SERIAL_IS_LT(client->rxq.highSerial, serial) ) {
                    client->rxq.highSerial = serial;
                }

                /* DGCALL.C line 1215, tracking frag size
                 * we don't track the largest recv frag size
                 * because we send always <= MUST_RECV_FRAG_SIZE
                 */

                /* DGCALL.C line 1293
                 *
                 * since we don't implement selective acknowledge,
                 * we accept only packets that are in-order.
                 *
                 * note that this strategy is compatible to selective acknowledge
                 */

                if( rcvhdr->fragnum == client->rxq.nextFragnum ) {

                    /* fragment is next-in-order */

                    if( client->rxq.rcvIdx + rcvhdr->len <= client->call.curr->args.cl.call->alloc_len ) {

                        CLRPC_MEMCPY(client->call.curr->args.cl.call->ndr_data + client->rxq.rcvIdx, (rcvhdr+1), rcvhdr->len);

                        client->rxq.rcvIdx += rcvhdr->len;

                        CLRPC_ASSERT(client->rxq.rcvIdx <= client->call.curr->args.cl.call->alloc_len);

                        CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                            , "recvQ: fragment is next-in-order, added %u bytes, rxq.rcvIdx(%u)"
                            , rcvhdr->len
                            , client->rxq.rcvIdx
                            );
                    }
                    else {
                        CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                            , "recvQ: out-param longer than alloc-len"
                            );

                        clrpc_cl_call_complete(client, CLRPC_ERR_OUT_ARGS);

                        clrpc_cl_send_ack(client); /* send ack to terminate the call */

                        goto provide_and_continue;
                    }

                    client->rxq.nextFragnum += 1;

                    client->call.rtxCnt = 0; /* reset, this is a progress */

                    clrpc_oneshot_trigger(&client->oneshot, CLRPC_RTX_TIMEOUT_MSEC);

                    /***/

                    /* DGCALL.C line 1511
                     * The data stream is complete when our queue has inorder data
                     * and it's not a fragmented stream, or if the last inorder
                     * fragment has the last_frag bit set.
                     */

                    if( ! client->rxq.recvFragmented || (rcvhdr->flags & CLRPC_PF_LAST_FRAG) ) {
                        client->rxq.allReceived = 1;
                    }
                }
                else if( CLRPC_FRAGNUM_IS_LT(rcvhdr->fragnum, client->rxq.nextFragnum) ) {
#if CLRPC_DEBUG
                    if( rcvhdr->fragnum == 0 ) {
                        /* Microsoft sends fragnum 0 always twice!? don't show this */
                    }
                    else {
                        CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                            , "recvQ: old fragment (duplicate), ignored."
                            );
                    }
#endif
                }
                else {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                        , "recvQ: fragment is out-of-order, ignored."
                        );

#ifdef CLRPC_MESSAGE
/* VV: sollte man hier nicht ein FACK schicken, damit es sofort weitergeht? */
#endif /* CLRPC_MESSAGE */
                }

                /* DGCALL.C line 1542
                 * If we've got a fragmented receive stream consider sending a fack.
                 * Send a fack if the receive queue is full or if the sender asked
                 * for a fack.  To avoid confusing pre-v2 code, don't send a fack
                 * when the stream is complete.
                 */

                if( client->rxq.allReceived ) {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "recvQ: response complete. calling back..."
                        );

                    /*
                     * we take the DREP from the last (or only) fragment
                     *
                     * this should be ok, because the sender uses the "native"
                     * format which should not change between fragments.
                     */

                    client->call.curr->args.cl.call->drep.drep_char  = (rcvhdr->drep[0] & 0x0F);
                    client->call.curr->args.cl.call->drep.drep_int   = (rcvhdr->drep[0] & 0xF0);
                    client->call.curr->args.cl.call->drep.drep_float = rcvhdr->drep[1];

                    CLRPC_ASSERT(client->rxq.rcvIdx <= client->call.curr->args.cl.call->alloc_len);

                    client->call.curr->args.cl.call->ndr_len = client->rxq.rcvIdx;

                    clrpc_cl_call_complete(client, CLRPC_OK);
                }
                else if( client->rxq.recvFragmented ) {

                    if( ! (rcvhdr->flags & CLRPC_PF_NO_FACK) ) {

                        CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                            , "recvQ: fragmented and not no-fack, sending fack"
                            );

                        /*
                         * DGCALL.C line 98
                         */

                        clrpc_cl_send_fack(client);

                    }
                    else { /* no-fack */

                        /* test! what if we would close the server's window?
                         * note that we don't do it by now...
                         *
                         * MS-Servers do not RTX by themselves.
                         * can ping/fack code below resolve this?
                         */
                    }
                }
            }
            else {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: got response, but not in recv-state, ignored"
                    );
            }
        }
        else if( rcvhdr->ptype == CLRPC_PT_WORKING ) {

            /* OSF sources DGCLSN.C line 582
             * Working's are received in response to pings and mean that the server
             * has received all the in's. Note that the server may still fail the call
             * (e.g., because it doesn't handle the requested interface).
             */

            CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: got WORKING state(%u)", client->state
                );

/*
 * consolidate, this "if" seems to be a duplicate, see "common" above
 */
            if( client->state == CLRPC_CL_RECV ) {

                /* OSF sources DGCLSN.C line 620
                 * If our call is non-idempotent, free any packets we have on the
                 * transmit queue still since the working response is (implicitly)
                 * ack'ing the receipt of them.  Note well that we MUST NOT free
                 * the queue for idempotent calls since the server is allowed to
                 * drop the response in which case a subsequent ping from us will
                 * induce a nocall response and oblige us to retransmit the request.
                 */

                /* ack everything */

                client->txq.ackIdx = client->txq.sndIdx = client->call.curr->args.cl.call->ndr_len;

                /*
                 * since we have no "queue", there is nothing to do here
                 *
                 * NOTE: the "send queue" is ok as long as rxq.rcvIdx == 0
                 */

                /* ping later */

                client->call.rtxCnt = 0; /* reset, making progress */

                if( ++client->call.workingCnt > client->call.curr->args.cl.call->appl_timeout1sec/*pings!*/ ) {

                    CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                        , "recvQ: too many workings(%u), orphaning"
                        , client->call.curr->args.cl.call->appl_timeout1sec/*pings!*/
                        );

                    client->state = CLRPC_CL_ORPHAN;

                    client->call.rtxCnt = CLRPC_MAX_RTX_RETRY;   /* AP00970884: QUIT will not be retransmitted, CLRPC waits only 1 sec */
                    clrpc_cl_send_quit(client);

                    clrpc_oneshot_trigger(&client->oneshot, CLRPC_QUIT_TIMEOUT_MSEC);
                }
                else {

                    clrpc_oneshot_trigger(&client->oneshot, CLRPC_PING_TIMEOUT_MSEC);
                }
            }
            else {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: got working, but not in recv-state, ignored"
                    );
            }
        }
        else if( rcvhdr->ptype == CLRPC_PT_REJECT ) {

            /*
             * OSF sources DGCLSN.C line 288
             */

            CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: got REJECT, state(%u)", client->state
                );

            /* OSF sources DGCLSN.C line 324
             * When in orphan-state, a reject is just as good as a quack
             */

            if( client->state == CLRPC_CL_ORPHAN ) {
                clrpc_cl_call_complete(client, CLRPC_OK_CANCELLED); /* see cancelQ */
                goto provide_and_continue;
            }

            CLRPC_ASSERT(client->state == CLRPC_CL_SEND || client->state == CLRPC_CL_RECV);

            /* OSF sources DGCLSN.C line 335
             * Free any packets we have on the transmit queue still since the
             * fault is (implicitly) ack'ing them.
             */

            client->txq.ackIdx = client->txq.sndIdx = client->call.curr->args.cl.call->ndr_len;

            /* OSF sources DGCLSN.C line 343
             * A reject response terminates the OUTs data stream.
             */

            /* OSF sources DGCLSN.C line 349
             * Get the NCA status from the reject packet body and map it to a local
             * status code.  Blow off the call with this mapped status.
             */

            if( rcvhdr->len == CLRPC_ERROR_BODY_SIZE ) {

                CLRPC_ERROR_BODY_PTR_TYPE body = (CLRPC_ERROR_BODY_PTR_TYPE)(rcvhdr+1); /* see packing-checks in clrpc_check_sizes() */

                if( CLRPC_DREP_INT_DIFFERS(rcvhdr->drep[0]) ) {

                    CLRPC_SWAP_U32(&body->nca_status);
                }

                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "recvQ: reject nca_status(%#x)", body->nca_status
                    );

                /* note that we don't map nca-codes to local codes */

                client->call.nca_status = body->nca_status;
            }
            else {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: wrong reject-body length"
                    );

                client->call.nca_status = CLRPC_NCA_UNSPECIFIC_REJECT; /* default */
            }

            clrpc_cl_call_complete(client, CLRPC_ERR_REJECTED);
        }
        else if( rcvhdr->ptype == CLRPC_PT_FAULT ) {

            /*
             * OSF sources DGCLSN.C line 375
             * Stash the fault pkt away for eventual retrieval by the client
             * stub and signal a call "call_faulted" failure.
             */

            CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: got FAULT, state(%u)", client->state
                );

            /* OSF sources DGCLSN.C line 411
             * When in the orphan state, a fault is just as good as a quack.
             */

            if( client->state == CLRPC_CL_ORPHAN ) {
                clrpc_cl_call_complete(client, CLRPC_OK_CANCELLED); /* see cancelQ */
                goto provide_and_continue;
            }

#if CLRPC_DEBUG
            if( client->txq.ackIdx < client->call.curr->args.cl.call->ndr_len ) {
                /* a fault is a notification from the server-stub (all INs transferred) */
                /* well, an invalid-context-uuid will be rejected without acking the INs */
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: got FAULT, but not all INs acked. state(%u)"
                    , client->state
                    );
            }
#endif

            /* OSF sources DGCLSN.C line 422
             * A fault response terminates the OUTs data stream.
             */

            if( rcvhdr->len == CLRPC_ERROR_BODY_SIZE ) {

                CLRPC_ERROR_BODY_PTR_TYPE body = (CLRPC_ERROR_BODY_PTR_TYPE)(rcvhdr+1); /* see packing-checks in clrpc_check_sizes() */

                if( CLRPC_DREP_INT_DIFFERS(rcvhdr->drep[0]) ) {

                    CLRPC_SWAP_U32(&body->nca_status);
                }

                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "recvQ: fault nca_status(%#x)", body->nca_status
                    );

                /* note that we don't map nca-codes to local codes */

                client->call.nca_status = body->nca_status;
            }
            else {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: wrong fault-body length"
                    );

                client->call.nca_status = CLRPC_NCA_UNSPECIFIC_REJECT; /* default */
            }

            /* terminate the call with an error */

            clrpc_cl_call_complete(client, CLRPC_ERR_FAULTED);
        }
        else if( rcvhdr->ptype == CLRPC_PT_QUACK ) {

            /*
             * OSF sources DGCLSN.C line 881 and 796
             * (their code is more complicated due to multi-threading)
             *
             * we simply complete on the first quack-packet we get, assuming that if we
             * are in the orphan-state (the activity is ok, too!) this is the answer
             * to our quit-packet and not a duplicate or something else.
             */

            CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "recvQ: got QUACK, state(%u)", client->state
                );

            if( client->state == CLRPC_CL_ORPHAN ) {

                /* we do not look at the body, see comment above */

                clrpc_cl_call_complete(client, CLRPC_OK_CANCELLED); /* see cancelQ */
            }
            else {
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "recvQ: got quack, but not in orphan-state, ignored. state(%u)"
                    , client->state
                    );
            }
        }
        else {
            /* obviously a bug in the caller */
            CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                , "recvQ: unknown ptype(%u), ignored. state(%u)"
                , rcvhdr->ptype, client->state
                );
        }

provide_and_continue:
        CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
            , "recvQ: provide-and-continue"
            );
        clrpc_cl_provide(client->channel, rcvlower);

    } /* endwhile recvQ */

    /*=========================================================================
    === another timeout =======================================================
    =========================================================================*/

    if( clrpc_queue_not_empty(&client->sendCnfQ) && client->timeoutFlag ) {

        /* a send resource and a timeout */

        client->timeoutFlag = 0;

        /***/

        CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
            , "timeoutQ: timeout, state(%u)...", client->state
            );

        /*
         * OSF sources: timeout handling is distributed over many places
         *
         * DGCCALL.C line 1533, send ping
         * DGCCALL.C line 1684, cancel timer
         * DGCCALL.C line 1769, recv state timer
         * DGCCALL.C line 1917, orphan timer
         * DGCCALL.C line 1951, timeout timer
         * DGCCALL.C line 2098, ccall timer
         */

        if( client->state == CLRPC_CL_IDLE ) {
            CLRPC_FATAL1(client->state);  /* bug if timeout and idle */
        }

        if( client->state == CLRPC_CL_FINAL ) {

            CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
                , "timeoutQ: final-state, sending ack"
                );
            clrpc_cl_send_ack(client);
            client->state = CLRPC_CL_IDLE;
        }
        else if( ++client->call.rtxCnt > CLRPC_MAX_RTX_RETRY ) {

            CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                , "timeoutQ: too many retries, state(%u)"
                , client->state
                );

            CLRPC_ASSERT( ! client->rxq.allReceived);

            if( client->state == CLRPC_CL_ORPHAN ) {
                clrpc_cl_call_complete(client, CLRPC_OK_CANCELLED);
            }
            else {
                clrpc_cl_call_complete(client, CLRPC_ERR_TIMEOUT);
            }
        }
        else {
            /*
             * send-state: as long as not the last (or only) fragment was transmitted
             *             retransmit from the last "acked" fragment
             *
             * recv-state: when the last (or only) fragment was transmitted, the call could have
             *             been executed. thus query progress by sending a ping
             */

            if( client->state == CLRPC_CL_SEND ) {

                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "timeoutQ: send-state (fack lost?), retransmit retry(%u)..."
                    , client->call.rtxCnt
                    );

                CLRPC_ASSERT( ! (client->txq.baseFlags & CLRPC_PF_BROADCAST)); /* broadcast is a single fragment */

                client->txq.sndIdx = client->txq.ackIdx;
                client->txq.sndWndCurr = 0; /* open the send window */
                client->txq.sndWndMax = 1; /* slow-start */

                /* trigger timer in sendCnfQ below */
            }
            else if( client->state == CLRPC_CL_RECV ) {

                if( (client->txq.baseFlags & CLRPC_PF_BROADCAST) ) {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
                        , "timeoutQ: no broadcast-answer"
                        );
                    clrpc_cl_call_complete(client, CLRPC_ERR_TIMEOUT);
                }
                else {

                    if( client->rxq.rcvIdx == 0 ) { /* nothing received yet */

                        /*
                         * NOTE: ping on single-fragment idempotent calls too
                         *       rather than retransmit the whole request (re-run)
                         *       the server might still buffer the answer
                         *
                         * NOTE: WAY is handled differently, Microsoft does not PING but retranmit
                         */

                        CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                            , "timeoutQ: recv-state, poll with ping, retry(%u)"
                            , client->call.rtxCnt
                            );

                        clrpc_cl_send_ping(client);

                        clrpc_oneshot_trigger(&client->oneshot, CLRPC_PING_TIMEOUT_MSEC);
                    }
                    else { /* first fragment received */

                        CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                            , "timeoutQ: recv-state, poll with fack, retry(%u)"
                            , client->call.rtxCnt
                            );

                        clrpc_cl_send_fack(client);

                        clrpc_oneshot_trigger(&client->oneshot, CLRPC_RTX_TIMEOUT_MSEC);
                    }
                }
            }
            else if( client->state == CLRPC_CL_ORPHAN ) {

                /* AP00970884: we should never reach this code, if rtxCnt is set to CLRPC_MAX_RTX_RETRY every time we send a QUIT */
                
                client->call.rtxCnt = CLRPC_MAX_RTX_RETRY;   /* AP00970884: QUIT will not be retransmitted, CLRPC waits only 1 sec */
                clrpc_cl_send_quit(client);

                clrpc_oneshot_trigger(&client->oneshot, CLRPC_QUIT_TIMEOUT_MSEC);
            }
            else {
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_FATAL
                    , "timeoutQ: unexpected timeout, state(%u)"
                    , client->state
                    );
                CLRPC_FATAL1(client->state); /* a bug */
            }
        }
    } /* endif timeout */

    /*=========================================================================
    === another cancel ========================================================
    =========================================================================*/

    if( clrpc_queue_not_empty(&client->sendCnfQ) && clrpc_queue_not_empty(&client->cancelQ) ) {

        CLRPC_UPPER_RQB_PTR_TYPE upper;

        /*
         * a cancel request -- cancel the current call (if any)
         */

        upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->cancelQ);
        CLRPC_ASSERT(upper);

        if( CLRPC_PTR_IS_NOT_NULL(client->call.curr) ) {

            CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "cancelQ: cancelling current call, pnio_signature(%u) opnum(%u)"
                , client->call.curr->args.cl.call->pnio_signature 
                , client->call.curr->args.cl.call->opnum 
                );

            clrpc_callback(client->channel, CLRPC_OK_CANCELLED, &upper, CLRPC_MODULE_ID, __LINE__); /* started cancellation */

            /***/

            client->state = CLRPC_CL_ORPHAN;
            
            if ((client->call.curr->args.cl.call->pnio_signature) && (client->call.curr->args.cl.call->opnum == CLRPC_PNIO_OPNUM_RELEASE))
            {
                /* AP00970884: Cancelling a RELEASE is "do nothing", no QUIT will be sent */
            }
            else
            {
                client->call.rtxCnt = CLRPC_MAX_RTX_RETRY;   /* AP00970884: QUIT will not be retransmitted, CLRPC waits only 1 sec */

                clrpc_cl_send_quit(client);

                clrpc_oneshot_trigger(&client->oneshot, CLRPC_QUIT_TIMEOUT_MSEC);
            }
        }
        else {

            CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "cancelQ: no current call"
                );

            clrpc_callback(client->channel, CLRPC_OK, &upper, CLRPC_MODULE_ID, __LINE__); /* nothing to do */
        }

    } /* endif cancelQ */

    /*=========================================================================
    === another call/bind/unbind request ======================================
    =========================================================================*/

callQ_again:
    if( clrpc_queue_not_empty(&client->sendCnfQ) && CLRPC_PTR_IS_NULL(client->call.curr) ) {

        CLRPC_UPPER_RQB_PTR_TYPE upper;

        /*
         * a send resource and not already a running call
         */

        /* peek into the call queue to see what is next */

        upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_peek(&client->callQ);

        if( CLRPC_PTR_IS_NOT_NULL(upper) ) {

            switch( CLRPC_RQB_GET_OPCODE(upper) ) {

            case CLRPC_OPC_CL_CALL:
            case CLRPC_OPC_CL_QUERY_EPM_NEXT: /* is a call to lookup() */
            case CLRPC_OPC_CL_QUERY_EPM_BREAK: /* is a call to lookup_handle_free() */
                if( client->state == CLRPC_CL_FINAL ) {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "callQ: call-request and in final-state, new call acks previous"
                        );
                    /* no need to send an ACK first */
                    clrpc_oneshot_stop(&client->oneshot);
                    client->state = CLRPC_CL_IDLE;
                }
                break;

            case CLRPC_OPC_CL_REBIND:
                if( client->state == CLRPC_CL_FINAL ) {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "callQ: rebind-request and in final-state, must send an ACK first"
                        );
                    clrpc_oneshot_stop(&client->oneshot);
                    clrpc_cl_send_ack(client);
                    client->state = CLRPC_CL_IDLE;
                    /* note: no send-resource here! */
                }
                upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->callQ);
                CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_REBIND);
                clrpc_cl_rebind_delayed(client, upper);
                goto callQ_again; /* maybe another request */

            case CLRPC_OPC_CL_UNBIND:
            case CLRPC_OPC_CL_QUERY_EPM_UNBIND:
                if( client->state == CLRPC_CL_FINAL ) {
                    CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                        , "callQ: unbind-request and in final-state, must send an ACK first"
                        );
                    clrpc_oneshot_stop(&client->oneshot);
                    clrpc_cl_send_ack(client);
                    client->state = CLRPC_CL_IDLE;
                }
                if( clrpc_queue_not_empty(&client->sendCnfQ) ) {
                    upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->callQ);
                    CLRPC_ASSERT(
                        CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_UNBIND
                        || CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_UNBIND
                    );
                    clrpc_queue_enqueue(&client->channel->cl.upper.unbindQ, upper);
                    clrpc_cl_channel_event(client->channel);
                    return 0; /* gone, client instance freed! */
                }
                /*
                 * note: if more than 1 send-resource must wait until all back from lower layer!
                 */
                goto callQ_again; /* wait for send-resource */

            default:
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_FATAL
                    , "callQ: unknown delayed opc(%u)"
                    , CLRPC_RQB_GET_OPCODE(upper)
                    );
                CLRPC_FATAL1(CLRPC_RQB_GET_OPCODE(upper)); /* a bug */
                break;
            }

            CLRPC_ASSERT(client->state == CLRPC_CL_IDLE);
            CLRPC_ASSERT( ! clrpc_oneshot_is_running(&client->oneshot));

#if CLRPC_DEBUG
            CLRPC_MEMSET(&client->call, 0xCC, sizeof(client->call)); /* to find uninitialized things */
#endif

            /***/

            client->call.curr = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->callQ);

            CLRPC_ASSERT(client->call.curr);

            CLRPC_ASSERT(
                CLRPC_RQB_GET_OPCODE(client->call.curr) == CLRPC_OPC_CL_CALL
                || CLRPC_RQB_GET_OPCODE(client->call.curr) == CLRPC_OPC_CL_QUERY_EPM_NEXT
                || CLRPC_RQB_GET_OPCODE(client->call.curr) == CLRPC_OPC_CL_QUERY_EPM_BREAK
            );

            /***/

            client->call.nca_status = 0;

            /***/

            
            client->call.rtxCnt = 0;

            /***/

            if( client->call.curr->args.cl.call->appl_timeout1sec == 0 ) {
                client->call.curr->args.cl.call->appl_timeout1sec = 300; /* default */
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: setting appl_timeout1sec to default(%usec)", client->call.curr->args.cl.call->appl_timeout1sec
                    );
            }

            client->call.curr->args.cl.call->appl_timeout1sec = (client->call.curr->args.cl.call->appl_timeout1sec * 1000) / CLRPC_PING_TIMEOUT_MSEC;

            if( client->call.curr->args.cl.call->appl_timeout1sec < 3 ) {
                client->call.curr->args.cl.call->appl_timeout1sec = 3; /* minimum number of pings */
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
                    , "callQ: setting appl_timeout1sec to min(%u pings)"
                    , client->call.curr->args.cl.call->appl_timeout1sec
                    );
            }

            /***/

            client->bnd.actseq += 1; /* a new activity for a new call */

            client->bnd.hintsUpdated = 0; /* allow update on every call; ihint, ahint are not reset! */

            /* initialize the "transmit queue" -- which is not a real queue (for now) */

#if CLRPC_DEBUG
            CLRPC_MEMSET(&client->txq, 0xCC, sizeof(client->txq)); /* to find uninitialized things */
#endif

            client->txq.baseFlags = 0; /* default is at-most-once */

            /***/

            switch( client->call.curr->args.cl.call->semantics ) {
            default:
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "callQ: wrong semantics(%u), defaulting to at-most-once"
                    , client->call.curr->args.cl.call->semantics
                    );
                //lint -fallthrough
            case CLRPC_SEMANTICS_AT_MOST_ONCE:
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: at-most-once"
                    );
                CLRPC_ASSERT(client->txq.baseFlags == 0);
                break;

            case CLRPC_SEMANTICS_IDEMPOTENT:
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: idempotent"
                    );
                client->txq.baseFlags = CLRPC_PF_IDEMPOTENT;
                break;

            case CLRPC_SEMANTICS_MAYBE:
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: maybe"
                    );
                client->txq.baseFlags = (CLRPC_PF_IDEMPOTENT | CLRPC_PF_MAYBE);
                break;

            case CLRPC_SEMANTICS_BROADCAST:
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: broadcast"
                    );
                client->txq.baseFlags = (CLRPC_PF_IDEMPOTENT | CLRPC_PF_BROADCAST);
                client->bnd.bound = 0;

                /* broadcast handling needs more thinking
                 * allow broadcast only on 255.255.255.255 bindings! error else.
                 */
                break;

            case CLRPC_SEMANTICS_BROADCAST_MAYBE:
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: broadcast+maybe"
                    );
                client->txq.baseFlags = (CLRPC_PF_IDEMPOTENT | CLRPC_PF_BROADCAST | CLRPC_PF_MAYBE);
                client->bnd.bound = 0;
                /* broadcast handling needs more thinking
                 * allow broadcast only on 255.255.255.255 bindings! error else.
                 */
                break;
            }

            /***/

            if( ((LSA_UINT32)client->call.curr->args.cl.call->ndr_data & 3) != 0 ) {
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "callQ: ndr_data(%#x) not long-aligned"
                    , client->call.curr->args.cl.call->ndr_data
                    );
                clrpc_cl_call_complete(client, CLRPC_ERR_PARAM); /* looks at base-flags */
                goto callQ_again; /* wait for send-resource */
            }

            if( client->call.curr->args.cl.call->pnio_signature ) {
                LSA_UINT16 response = clrpc_cl_pnio_marshal(client, client->call.curr);
                if( response != CLRPC_OK ) {
                    clrpc_cl_call_complete(client, response); /* looks at base-flags */
                    goto callQ_again; /* wait for send-resource */
                }
            }

            /***/

            client->txq.serial = 0; /* initialized to 0 (zero) when a call begins */

            /***/

			client->txq.sendSize = CLRPC_MUST_RECV_BODY_SIZE_INT; 
            
            client->txq.sendSize &= (~7); /* must be a multiple of 8 */

            CLRPC_CL_TRACE_03(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "callQ: using send-size(%u) + clrpc-hdr(%u) = %u"
                , client->txq.sendSize
                , CLRPC_HEADER_SIZE
                , client->txq.sendSize + CLRPC_HEADER_SIZE
                );

            CLRPC_ASSERT(client->txq.sendSize >= 8 && (client->txq.sendSize % 8) == 0);

            /***/

            if( client->call.curr->args.cl.call->ndr_len > client->txq.sendSize ) {
                client->txq.baseFlags |= CLRPC_PF_FRAG;
            }

            /***/

            client->txq.sndWndCurr = 0;
            client->txq.sndWndMax = 1; /* PNIO: always want fack for first fragment */

            client->txq.sndIdx = 0;
            client->txq.ackIdx = 0;

            /* initialize the "receive queue" -- which is not a real queue (for now) */

#if CLRPC_DEBUG
            CLRPC_MEMSET(&client->rxq, 0xCC, sizeof(client->rxq)); /* to find uninitialized things */
#endif

            client->rxq.recvFragmented = 0;
            client->rxq.nextFragnum = 0;
            client->rxq.highSerial = (LSA_UINT16)-1; /* see recvQ and timeout */

            client->rxq.rcvIdx = 0;
            client->rxq.allReceived = 0;

            /***/

            client->state = CLRPC_CL_SEND;

            /***/

            if( (client->txq.baseFlags & (CLRPC_PF_MAYBE | CLRPC_PF_BROADCAST)) && (client->txq.baseFlags & CLRPC_PF_FRAG) ) {
                /* broadcast handling needs more thinking
                 * allow broadcast only on 255.255.255.255 bindings! error else.
                 */
                CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: maybe- or broadcast-call cannot have in-params longer than send-size(%u)"
                    , client->txq.sendSize
                    );
                clrpc_cl_call_complete(client, CLRPC_ERR_IN_ARGS);
                goto callQ_again; /* not-tested-yet */
#ifdef CLRPC_MESSAGE
/* review: make the big if() a while() and break below! goto becomes continue. to be checked */
#endif /* CLRPC_MESSAGE */
            }

            CLRPC_CL_TRACE_05(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "callQ: calling bound(%u) host(%08x) port(%u) opnum(%u) rtx-timeout(%u msec)"
                , client->bnd.bound
                , CLRPC_NTOHL(client->bnd.bound ? client->bnd.serverHost : client->bnd.host_ip)
                , CLRPC_NTOHS((LSA_UINT16)(client->bnd.bound ? client->bnd.serverPort : client->bnd.udp_port))
                , client->call.curr->args.cl.call->opnum
                , CLRPC_RTX_TIMEOUT_MSEC
                );
        }
        else {
            CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "no other call-request, state(%u)", client->state
                );
            CLRPC_ASSERT(client->state == CLRPC_CL_IDLE || client->state == CLRPC_CL_FINAL);
        }
    } /* endif callQ */

    /*=========================================================================
    === another send ==========================================================
    =========================================================================*/

    /* must adapt code (while-loop) when more than one send-resource */

    if( clrpc_queue_not_empty(&client->sendCnfQ) && CLRPC_PTR_IS_NOT_NULL(client->call.curr) ) {

        /* a send resource and maybe something (more) to send */

        if( client->txq.sndWndCurr < client->txq.sndWndMax /* send window not full */
            && (
                client->txq.sndIdx < client->call.curr->args.cl.call->ndr_len /* not all sent yet */
                || 0 == client->call.curr->args.cl.call->ndr_len /* but no "in-s" is ok too! */
            )
            && client->state != CLRPC_CL_ORPHAN /* not cancelling */
        ) {
            LSA_UINT8 flags;
            CLRPC_UPPER_MEM_U8_PTR_TYPE body;
            CLRPC_LOWER_RQB_PTR_TYPE sndlower;
            CLRPC_HEADER_PTR_TYPE sndhdr;
            LSA_INT len;
            LSA_UINT16 pdu_len;

            CLRPC_ASSERT(client->state == CLRPC_CL_SEND /* obviously */
                || client->state == CLRPC_CL_RECV /* retransmission of last frag, see below too */
            );

            CLRPC_CL_TRACE_04(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "sendQ: more to send, state(%u), sndIdx(%u) len(%u) txq.sendSize(%u)"
                , client->state
                , client->txq.sndIdx
                , client->call.curr->args.cl.call->ndr_len
                , client->txq.sendSize
                );

            body = client->call.curr->args.cl.call->ndr_data + client->txq.sndIdx;

            CLRPC_ASSERT( ! (
                body == client->call.curr->args.cl.call->ndr_data + client->call.curr->args.cl.call->ndr_len
                && client->txq.sndIdx != 0
            )); /* bug if sending empty packet when length is multiple of send-size */

            /* setup for next fragment */

            client->txq.sndIdx += client->txq.sendSize;

            client->txq.sndWndCurr += 1;

            /***/

            flags = client->txq.baseFlags; /* frag-bit and semantics */

            if( client->txq.sndWndCurr < client->txq.sndWndMax ) {
                if( (flags & CLRPC_PF_FRAG) ) {
                    flags |= CLRPC_PF_NO_FACK;
                }
            }

            if( client->txq.sndIdx >= client->call.curr->args.cl.call->ndr_len ) {

                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "sendQ: sending last or only fragment"
                    );

                if( (flags & CLRPC_PF_FRAG) ) {
                    flags |= (CLRPC_PF_LAST_FRAG | CLRPC_PF_NO_FACK);
#ifdef CLRPC_MESSAGE
/* irgendwo steht, dass "alte" server fack auf last nicht können. referenz einfügen */
#endif /* CLRPC_MESSAGE */
                }

                CLRPC_ASSERT(client->state == CLRPC_CL_SEND || client->state == CLRPC_CL_RECV);

                client->state = CLRPC_CL_RECV;
                client->call.workingCnt = 0;
            }

            CLRPC_ASSERT(client->state != CLRPC_CL_RECV
                /* or when in recv-state, it must be the retransmission of the last frag */
                || ! (flags & CLRPC_PF_FRAG) || (flags & CLRPC_PF_LAST_FRAG)
            );

#if CLRPC_DEBUG
            if( ! (flags & CLRPC_PF_NO_FACK) ) {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "sendQ: waiting for a FACK..."
                    );
            }
#endif

            sndlower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue(&client->sendCnfQ);
            CLRPC_ASSERT(sndlower);

            sndhdr = clrpc_cl_prepare(client, sndlower, CLRPC_PT_REQUEST);

            sndhdr->flags = flags;

            sndhdr->opnum = client->call.curr->args.cl.call->opnum;

            sndhdr->fragnum = CLRPC_CAST_U16(client->txq.sndIdx / client->txq.sendSize - 1);

            len = client->call.curr->args.cl.call->ndr_data + client->call.curr->args.cl.call->ndr_len - body;

            if( len > client->txq.sendSize ) 
            {
                pdu_len = client->txq.sendSize;
            }
            else
            {
                pdu_len = (LSA_UINT16)len;
            }

            CLRPC_MEMCPY(sndhdr+1, body, pdu_len);

            sndhdr->len = pdu_len;

            clrpc_cl_send(client, sndlower, sndhdr, 0);

            client->txq.serial += 1; /* increment serial on each REQUEST, RESPONSE or PING */

            if( (client->txq.baseFlags & CLRPC_PF_MAYBE) ) {
                CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                    , "callQ: maybe-call complete"
                    );
                clrpc_cl_call_complete(client, CLRPC_OK);
            }
            else if( (client->txq.baseFlags & CLRPC_PF_BROADCAST) ) {
                clrpc_oneshot_trigger(&client->oneshot, CLRPC_BROADCAST_TIMEOUT_MSEC);
                /* VV: make timeout a factor of call-timeout? */
            }
            else 
            {
                if ((client->call.curr->args.cl.call->pnio_signature) && (client->call.curr->args.cl.call->opnum == CLRPC_PNIO_OPNUM_RELEASE))
                {
                    /* AP00970884: RELEASE in PNIO-Interface: timeout only 1 sec and no retry, like QUIT */
                    client->call.rtxCnt = CLRPC_MAX_RTX_RETRY;   
                    clrpc_oneshot_trigger(&client->oneshot, CLRPC_QUIT_TIMEOUT_MSEC);
                }
                else
                {
                    /* PNIO: timeout has no back-off */

                    clrpc_oneshot_trigger(&client->oneshot
                        , client->state != CLRPC_CL_RECV
                            ? CLRPC_RTX_TIMEOUT_MSEC
                            : CLRPC_PING_TIMEOUT_MSEC /* give server a chance to run */
                    );
                }
            }
        }
        else {
            CLRPC_CL_TRACE_02(client->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "sendQ: send nothing: window-full(%u), all-sent(%u)"
                , client->txq.sndWndCurr >= client->txq.sndWndMax
                , client->txq.sndIdx >= client->call.curr->args.cl.call->ndr_len
                );
        }
    } /* endif sendcnfQ */

    return 1;
}

/*=============================================================================
//=== CLIENT EVENT ============================================================
//===========================================================================*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_event(
	CLRPC_CLIENT_PTR_TYPE client
) {
	CLRPC_LOG_FCT("clrpc_cl_event")

	CLRPC_ASSERT(client);
	CLRPC_ASSERT(client->state >= CLRPC_CL_IDLE && client->state <= CLRPC_CL_FINAL);

	if( client->entered ) { /* see comment in clrpc_icl.h */

		client->again = 1;

		CLRPC_CL_TRACE_06(client->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "reentering! callQ(%u) call.curr(%08x) sendCnfQ(%u) rcvQ(%u) convQ(%u) timeout(%u)"
			, clrpc_queue_not_empty(&client->callQ)
			, client->call.curr
			, clrpc_queue_not_empty(&client->sendCnfQ)
			, clrpc_queue_not_empty(&client->recvQ)
			, clrpc_queue_not_empty(&client->convQ)
			, client->timeoutFlag
			);
	}
	else {

		client->entered = 1;

		/* assert that there is an event */

		CLRPC_ASSERT(
			clrpc_queue_not_empty(&client->recvQ)
			|| clrpc_queue_not_empty(&client->convQ)
			|| client->timeoutFlag
			|| client->call.curr != 0
			|| clrpc_queue_not_empty(&client->callQ)
			|| client->state == CLRPC_CL_IDLE
			|| client->state == CLRPC_CL_FINAL
		);

		do {
			if( client->again ) {
				CLRPC_CL_TRACE_00(client->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "again!"
					);
				/*
				 * note: it is a possible situation that there is NO pending event if "again"!
				 */
			}

			client->again = 0;

			CLRPC_CL_TRACE_08(client->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "[%08x] (PRE) callQ/curr(%u/%u) sendCnfQ(%u) rcvQ(%u) convQ(%u) timeout(%u) running(%u)"
				, client
				, clrpc_queue_not_empty(&client->callQ)
				, client->call.curr != 0
				, clrpc_queue_not_empty(&client->sendCnfQ)
				, clrpc_queue_not_empty(&client->recvQ)
				, clrpc_queue_not_empty(&client->convQ)
				, client->timeoutFlag
				, clrpc_oneshot_is_running(&client->oneshot)
				);

			if( ! clrpc_cl_statemachine(client) ) {
				CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
					, "client gone (instance freed)"
					);
				return;
			}

			/* if there is a running call then there must be a following event other than receive */

			CLRPC_CL_TRACE_07(client->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "(POST) callQ/curr(%u/%u) sendCnfQ(%u) rcvQ(%u) convQ(%u) timeout(%u) running(%u)"
				, clrpc_queue_not_empty(&client->callQ)
				, client->call.curr != 0
				, clrpc_queue_not_empty(&client->sendCnfQ)
				, clrpc_queue_not_empty(&client->recvQ)
				, clrpc_queue_not_empty(&client->convQ)
				, client->timeoutFlag
				, clrpc_oneshot_is_running(&client->oneshot)
				);

			CLRPC_ASSERT(
				/* either no call */
				CLRPC_PTR_IS_NULL(client->call.curr)
				/* or if there is a call, either send-cnf next or timeout next */
				|| (
					! clrpc_queue_not_empty(&client->sendCnfQ) || clrpc_oneshot_is_running(&client->oneshot)
					/* test works only for 1 send-resource... */
				)
				/* or taking another loop (check at last!) */
				|| client->again
			);

		} while( client->again );

		client->entered = 0;
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#else

#ifdef CLRPC_MESSAGE
#pragma CLRPC_MESSAGE("!!! not compiling client")
#endif /* CLRPC_MESSAGE */

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
