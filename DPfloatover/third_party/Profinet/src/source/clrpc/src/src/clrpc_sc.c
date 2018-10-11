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
/*  F i l e               &F: clrpc_sc.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the ServerCall part                                           */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    11
#define LTRC_ACT_MODUL_ID  11

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


	/* compile only, when server-part is needed */
#if CLRPC_CFG_MAX_SERVERS

/*----------------------------------------------------------------------------*/

static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_init (
	CLRPC_SCALL_PTR_TYPE  scall
	);
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_wf_way (
	CLRPC_SCALL_PTR_TYPE  scall
	);
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_wf_answer (
	CLRPC_SCALL_PTR_TYPE  scall
	);
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_free (
	CLRPC_SCALL_PTR_TYPE  scall
	);
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_ping (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	);


/*------------------------------------------------------------------------------
//	clrpc_sc_oneshot_callback
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_oneshot_callback (
	CLRPC_ONESHOT_PTR_TYPE  oneshot,
	LSA_VOID_PTR_TYPE user_ptr
	)
{
	CLRPC_LOG_FCT("clrpc_sc_oneshot_callback")
	CLRPC_SCALL_PTR_TYPE  scall = (CLRPC_SCALL_PTR_TYPE)user_ptr; /* typecast */

	LSA_UNUSED_ARG (oneshot);

	CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) timeout"
		, scall->id
		);
	clrpc_sc_dispatch (scall, LSA_NULL, LSA_NULL, 1, LSA_NULL);
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_indy
//	work: recv a INDY.req from the client
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_indy (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_HEADER_PTR_TYPE  pkt)
{
	CLRPC_LOG_FCT("clrpc_sc_indy")
	CLRPC_UUID_PTR_TYPE  cas_uuid;

	if (! (CLRPC_IF_VERS_MAJOR (pkt->if_vers) == 1
		&& CLRPC_IF_VERS_MINOR (pkt->if_vers) == 0
		&& pkt->opnum == 0
		&& pkt->len < sizeof (*cas_uuid)))
	{
		CLRPC_SV_TRACE_05 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cid(%u) funny indy, V(%u.%u) opnum(%u) len(%u)"
			, scall->id
			, CLRPC_IF_VERS_MAJOR (pkt->if_vers), CLRPC_IF_VERS_MINOR (pkt->if_vers)
			, pkt->opnum, pkt->len
			);
		return;
	}

	cas_uuid = CLRPC_PKT_GET_BODY (pkt, CLRPC_UUID_PTR_TYPE);
	if (CLRPC_DREP_INT_DIFFERS (pkt->drep[0])) {
		CLRPC_SWAP_UUID (cas_uuid);
	}

	CLRPC_SV_TRACE_12 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) cas_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, scall->id
		, cas_uuid->time_low, cas_uuid->time_mid, cas_uuid->time_hi_and_version
		, cas_uuid->clock_seq_hi_and_reserved, cas_uuid->clock_seq_low
		, cas_uuid->node[0], cas_uuid->node[1], cas_uuid->node[2]
		, cas_uuid->node[3], cas_uuid->node[4], cas_uuid->node[5]
		);

#if 0 /* no context handles in CLRPC */
#error
/*
	if (scall->ctx_count == 0) {
		CLRPC_SV_TRACE_00 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "client error? indy and no ctx"
			);
		return;
	}

	clrpc_oneshot_retrigger (scall->XXX, CLRPC_SV_TIMEOUT_CONTEXT_RUNDOWN);
*/
#else
	LSA_UNUSED_ARG(scall);
#endif
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_prepare_send_frag
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_prepare_send_frag (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_HEADER_PTR_TYPE  pkt_send
	)
{
	CLRPC_LOG_FCT("clrpc_sc_prepare_send_frag")
	LSA_VOID_PTR_TYPE body;
	LSA_UINT8  flags;
	LSA_INT32  len;

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) state(%u) fragnum(%u)"
		, scall->id
		, scall->state
		, scall->send.frag_curr_fragnum
		);

	CLRPC_ASSERT ((scall->state & (CLRPC_CST_SEND | CLRPC_CST_WF_ACK)));
	CLRPC_ASSERT ((scall->send.frag_curr_fragnum <= scall->send.frag_last_fragnum));

		/* calculation flags and len and next_state */
	len = scall->up_call->ndr_len - scall->send.frag_curr_fragnum * CLRPC_MUST_RECV_BODY_SIZE;
	flags = scall->send.frag_flags | CLRPC_PF_NO_FACK | CLRPC_PF_LAST_FRAG;
	scall->send.next_state = CLRPC_CST_WF_ACK;
	scall->send.again = 0;

	/* there is more date do send, so keep in state send */
    if (len < 0)
    {
		CLRPC_FATAL1(len); /* wrong length */
    }
    else if (len > CLRPC_MUST_RECV_BODY_SIZE) 
    {
		len = CLRPC_MUST_RECV_BODY_SIZE;
		flags = scall->send.frag_flags | CLRPC_PF_NO_FACK;
        if (scall->state != CLRPC_CST_WF_ACK) 
        {
			scall->send.next_state = CLRPC_CST_SEND;
		}

			/* the last frag of the window size needs a fack, drop NO_FACK */
        if (scall->send.frag_curr_fragnum
			== (LSA_UINT16)(scall->send.frag_ack_fragnum + scall->send.frag_window_size))
		{
			flags = scall->send.frag_flags;
		}
			/* or we send again */
        else 
        {
			scall->send.again = 1;
		}
	}

		/* set pkt */
    pkt_send->fragnum = scall->send.frag_curr_fragnum;
	pkt_send->flags = flags;

	pkt_send->len = (LSA_UINT16)len;
	body = CLRPC_PKT_GET_BODY (pkt_send, LSA_VOID_PTR_TYPE);

	CLRPC_MEMCPY (body, scall->up_call->ndr_data + scall->send.frag_curr_fragnum * CLRPC_MUST_RECV_BODY_SIZE, len);

		/* timeout in state send, means no fack recv => die */
    if (! scall->send.again) 
    {
		clrpc_oneshot_trigger (& scall->oneshot, CLRPC_CFG_SCALL_DIE_TIMEOUT_MSEC);
	}

		/* prepare next fragnum */
    scall->send.frag_curr_fragnum += 1;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_prepare_way_req
//	work: set up a way-request
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_prepare_way_req (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_HEADER_PTR_TYPE  pkt_send
	)
{
	CLRPC_LOG_FCT("clrpc_sc_prepare_way_req")
	CLRPC_WAY_REQ_PTR_TYPE  way_body;

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) state(%u) way.state(%u)"
		, scall->id
		, scall->state
		, scall->way.state
		);

	CLRPC_PKT_SET_IF_ID (pkt_send, & clrpc_conversation_manager_uuid);
	pkt_send->if_vers = CLRPC_MAKE_IF_VERS (3, 0); /* version 3.0 */
	CLRPC_PKT_SET_ACTUID (pkt_send, & scall->way.hashed_act.uuid);
	pkt_send->opnum = 1;	/* way2 */
	pkt_send->flags = CLRPC_PF_IDEMPOTENT;
	pkt_send->len = CLRPC_WAY_REQ_SIZE;

	way_body = CLRPC_PKT_GET_BODY (pkt_send, CLRPC_WAY_REQ_PTR_TYPE);
	CLRPC_MEMCPY (& way_body->actuid, & scall->hashed_act.uuid, sizeof(way_body->actuid));
	way_body->boot_time = scall->server->server_boot;

	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_prepare_quack
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_prepare_quack (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_HEADER_PTR_TYPE  pkt_send
	)
{
	CLRPC_LOG_FCT("clrpc_sc_prepare_quack")
	CLRPC_QUACK_BODY_PTR_TYPE  quack_body;

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) state(%u) cancel_id(%u)"
		, scall->id
		, scall->state
		, scall->send.cancel_id
		);

	scall->send.next_state = CLRPC_CST_ORPHAN;

	if (scall->send.cancel_id != (LSA_UINT32)(-1)) {
		scall->send.next_state = CLRPC_CST_ORPHAN;

		quack_body = CLRPC_PKT_GET_BODY (pkt_send, CLRPC_QUACK_BODY_PTR_TYPE);
		quack_body->vers = 0;
		quack_body->cancel_id = scall->send.cancel_id;
		quack_body->server_is_accepting = 1;

		pkt_send->len = CLRPC_QUACK_BODY_SIZE_ONWIRE; /* not: CLRPC_QUACK_BODY_SIZE_PADDED */
	}
}


/*------------------------------------------------------------------------------
//	clrpc_sc_send_prepare
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_send_prepare (
	CLRPC_SCALL_PTR_TYPE  scall,
    CLRPC_LOWER_RQB_PTR_TYPE  send
	)
{
	CLRPC_LOG_FCT("clrpc_sc_send_prepare")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_send;
	CLRPC_HEADER_PTR_TYPE  pkt_send;

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) SEND_PREPARE: state(%u) ptype(%u)"
		, scall->id
		, scall->state
		, scall->send.ptype
		);

	if (scall->state & (CLRPC_CST_FREE | CLRPC_CST_ORPHAN)) {
			/* seems to late, ack should be arrived in the meantime */
			/* way not dequeue'ed? is this a pgm-fatal? */
		return CLRPC_OK_CANCELLED;
	}

		/* send only one times, or the ptype says something different */
	scall->send.again = 0;	/* default: send only a single paket */
	scall->send.next_state = scall->state;	/* default: keep in current state */

		/* get pointer inside the buffer */
	CLRPC_PKT_GET_FROM_SEND (send, udp_send, pkt_send);

	CLRPC_UDP_SEND_INIT (udp_send);

	if (CLRPC_PTR_IS_NOT_NULL (scall->up_call)) {
		clrpc_pkt_init (pkt_send, scall->send.ptype,
				& scall->server->if_uuid,
				CLRPC_MAKE_IF_VERS (scall->server->if_major, scall->server->if_minor),
				scall->up_call->opnum, & scall->up_call->object_id,
				& scall->hashed_act.uuid, scall->seq
			);
	}
	else {
		clrpc_pkt_init (pkt_send, scall->send.ptype,
				& scall->server->if_uuid,
				CLRPC_MAKE_IF_VERS (scall->server->if_major, scall->server->if_minor),
				0, & clrpc_uuid_nil_uuid,
				& scall->hashed_act.uuid, scall->seq
			);
	}

		/* set up ptype */
	switch (scall->send.ptype) {
	case CLRPC_PT_REQUEST: /* client -> server */
			/* server send a way2 */
		clrpc_sc_prepare_way_req (scall, pkt_send);
		scall->send.serial += 1;
		CLRPC_PKT_SET_SERIAL (pkt_send, scall->send.serial);
		break;

	case CLRPC_PT_PING: /* client -> server */
		CLRPC_ASSERT (0); /* "pgm error, never send" */
		break;

	case CLRPC_PT_RESPONSE: /* server -> client */
		clrpc_sc_prepare_send_frag (scall, pkt_send);
		scall->send.serial += 1;
		CLRPC_PKT_SET_SERIAL (pkt_send, scall->send.serial);
		break;

	case CLRPC_PT_FAULT: /* server -> client */
		clrpc_pkt_set_error_body (pkt_send, scall->send.reject_fault_error);
		scall->send.next_state = CLRPC_CST_INIT;
		break;

	case CLRPC_PT_WORKING: /* server -> client */
		/* send only common header */
		break;

	case CLRPC_PT_NOCALL: /* server -> client */
		/* uses clrpc_sv_send_nocall */
		CLRPC_ASSERT (0); /* "pgm error, never send" */
		break;

	case CLRPC_PT_REJECT: /* server -> client */
		clrpc_pkt_set_error_body (pkt_send, scall->send.reject_fault_error);
		scall->send.next_state = CLRPC_CST_INIT;
		break;

	case CLRPC_PT_ACK: /* client -> server */
		CLRPC_ASSERT (0); /* "pgm error, never send" */
		break;

	case CLRPC_PT_QUIT: /* client -> server */
		CLRPC_ASSERT (0); /* "pgm error, never send" */
		break;

	case CLRPC_PT_FACK: /* both directions  */
		pkt_send->fragnum = scall->recv.fragnum;
		clrpc_pkt_set_fack_body (pkt_send, 1/*?window_size*/, scall->send.fack_serial);	/* == recv.serial */
		break;

	case CLRPC_PT_QUACK:/* server -> client */
		/* no body means orphan the scall */
		clrpc_sc_prepare_quack (scall, pkt_send);
		break;

	default:
		CLRPC_ASSERT (0); /* "pgm error" */
		break;
	}

		/* prepare common pkt_send. should we alway use recv.host.ip/.port ? */
	if (CLRPC_UDP_IP(udp_send) == 0 || CLRPC_UDP_PORT(udp_send) == 0) {
		CLRPC_UDP_IP(udp_send) = scall->recv.host;
		CLRPC_UDP_PORT(udp_send) = scall->recv.port;
	}
	return CLRPC_OK;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_post_to_send_queue
//----------------------------------------------------------------------------*/
static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_post_to_send_queue (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_post_to_send_queue")

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) state(%u)"
		, scall->id
		, scall->state
		);

#if 0 /* see comment in _trace */
	CLRPC_ASSERT (scall->send.in_queue == 0);
	scall->send.in_queue = 1;
	clrpc_sv_pkt_send_or_queue (scall->server, scall);
#else
	if (scall->send.in_queue) {
		CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "cid(%u) with state(%u) wants to re-queue or send another ptype(%u)."
			  " e.g. a lack of send-resources."
			, scall->id, scall->state, scall->send.ptype
			);

		clrpc_queue_unlink (& scall->server->send_scall_queue, scall);
		scall->send.in_queue = 0;
	}
	scall->send.in_queue = 1;
	clrpc_sv_pkt_send_or_queue (scall->server, scall);
#endif
}


/*------------------------------------------------------------------------------
//	clrpc_sc_send_done
//----------------------------------------------------------------------------*/
static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_send_done (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_send_done")

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) state(%u) next_state(%u)"
		,  scall->id
		, scall->state
		, scall->send.next_state
		);

	scall->send.in_queue = 0;

		/* switch do new state */
	if (scall->state != scall->send.next_state) {
		switch (scall->send.next_state) {
		case CLRPC_CST_INIT:
			clrpc_sc_do_init (scall);
			break;

		case CLRPC_CST_WF_ACK:
			clrpc_sc_do_wf_ack (scall);
			break;

		case CLRPC_CST_ORPHAN:
			clrpc_sc_do_orphan (scall);
			break;

		default:
			CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unknown next_state(%u)", scall->send.next_state
				);
			CLRPC_FATAL1(scall->send.next_state); /* unknown next_state" */
		}
	}

		/* or there is to queue a next send-order */
	if (scall->send.again) {
		clrpc_sc_post_to_send_queue (scall);
	}

	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_send_working
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_send_working (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_send_working")

	CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) send.ptype = PT_WORKING"
		, scall->id
		);

	scall->send.ptype = CLRPC_PT_WORKING;

	clrpc_sc_post_to_send_queue (scall);
}


/*------------------------------------------------------------------------------
//	clrpc_sc_send_fack
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_send_fack (
	CLRPC_SCALL_PTR_TYPE  scall,
	LSA_UINT16  serial
	)
{
	CLRPC_LOG_FCT("clrpc_sc_send_fack")

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) send.ptype = PT_FACK  fack_serial(%u)"
		, scall->id
		, serial
		);

	scall->send.ptype = CLRPC_PT_FACK;
	scall->send.fack_serial = serial;

	clrpc_sc_post_to_send_queue (scall);
}


/*------------------------------------------------------------------------------
//	clrpc_sc_send_error
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_send_error (
	CLRPC_SCALL_PTR_TYPE  scall,
	LSA_UINT16  ptype,
	LSA_UINT32  nca_error
	)
{
	CLRPC_LOG_FCT("clrpc_sc_send_error")

#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("todo: build 2 functions")
#endif /* CLRPC_MESSAGE */

	switch (ptype) {
	case CLRPC_PT_FAULT:
		CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cid(%u) send.ptype(%u) = PT_FAULT state(%u) nca_error(%u)"
			, scall->id
			, ptype
			, scall->state
			, nca_error
			);
		break;

	case CLRPC_PT_REJECT:
	default:
		CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cid(%u) send.ptype(%u) = PT_REJECT state(%u) nca_error(%u)"
			, scall->id
			, ptype
			, scall->state
			, nca_error
			);
		break;
	}

	scall->send.ptype = ptype;
	scall->send.reject_fault_error = nca_error;

	clrpc_sc_post_to_send_queue (scall);
}


/*------------------------------------------------------------------------------
//	clrpc_sc_send_way
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_send_way (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_send_way")
	CLRPC_UUID_TYPE actuid;

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) state(%u) way.state(%u)"
		, scall->id
		, scall->state
		, scall->way.state
		);

		/* must prepare way.statemachine first */
	if (scall->way.state == CLRPC_WAY_ST_UNKNOWN)
	{
		LSA_UINT16  cnt;	
			/* need a new actuid */
		for (cnt = 0; cnt < 10; ++cnt)
		{
			clrpc_uuid_create_actuid (scall->server->channel, &actuid);
			if (CLRPC_PTR_IS_NULL (clrpc_hash_lookup (& clrpc_sc_hash, &actuid, CLRPC_NO_HINT)))
			{
				break; /* use this uuid */
			}
		}
		if (cnt >= 10) {
			CLRPC_FATAL1 (cnt);
			return;
		}

			/* register way.actuid with scall */
		clrpc_hash_entry_setup(&scall->way.hashed_act, &actuid, scall);

		clrpc_hash_add (& clrpc_sc_hash, & scall->way.hashed_act);

		scall->way.state = CLRPC_WAY_ST_RUNNING;
	}

		/* prepare to send */
	scall->send.ptype = CLRPC_PT_REQUEST;
	clrpc_sc_post_to_send_queue (scall);
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_send_quack
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_send_quack (
	CLRPC_SCALL_PTR_TYPE  scall,
	LSA_UINT32  cancel_id
	)
{
	CLRPC_LOG_FCT("clrpc_sc_send_quack")

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) send.ptype = PT_QUACK  state(%u) cancel_id(%u)"
		, scall->id
		, scall->state
		, cancel_id
		);

	scall->send.ptype = CLRPC_PT_QUACK;
	scall->send.cancel_id = cancel_id;

	/* a cancel_ack with no body means orphan of the scall */
	clrpc_sc_post_to_send_queue (scall);
}

/*------------------------------------------------------------------------------
// if_object::opnum_count
//----------------------------------------------------------------------------*/

static LSA_BOOL  CLRPC_LOCAL_FCT_ATTR
clrpc_if_obj_opnum_count_ok (
	CLRPC_IF_OBJECT_PTR_TYPE  if_obj,
	LSA_UINT16  has_pnio_signature,
	LSA_UINT16  opnum
	)
{
	CLRPC_LOG_FCT("clrpc_if_obj_opnum_count_ok")

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (if_obj));

	if (! has_pnio_signature) {
		return LSA_TRUE;
	}

	switch (opnum)	{
	case CLRPC_PNIO_OPNUM_CONNECT:
	case CLRPC_PNIO_OPNUM_RELEASE:
	case CLRPC_PNIO_OPNUM_READ:
	case CLRPC_PNIO_OPNUM_WRITE:
	case CLRPC_PNIO_OPNUM_CONTROL:
		return LSA_TRUE;

	case CLRPC_PNIO_OPNUM_READ_IMPLICIT:
		if (if_obj->read_implicit_curr >= if_obj->read_implicit_max) {
			CLRPC_SV_TRACE_01 (0, LSA_TRACE_LEVEL_WARN
				, "maximum read-implicit received(%u)"
				, if_obj->read_implicit_curr
				);
			return LSA_FALSE;
		}
		CLRPC_SV_TRACE_02 (0, LSA_TRACE_LEVEL_CHAT
			, "current running read-implicit(%u), maximum(%u)"
			, if_obj->read_implicit_curr, if_obj->read_implicit_max
			);
		return LSA_TRUE;

	default:
		CLRPC_ASSERT(0); /* wrong opnum-range-check */
		return LSA_TRUE;
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_if_obj_opnum_count_incr (
	CLRPC_IF_OBJECT_PTR_TYPE  if_obj,
	LSA_UINT16  has_pnio_signature,
	LSA_UINT16  opnum
	)
{
	CLRPC_LOG_FCT("clrpc_if_obj_opnum_count_incr")

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (if_obj));

	if (has_pnio_signature) {
		switch (opnum) {
		case CLRPC_PNIO_OPNUM_CONNECT:
		case CLRPC_PNIO_OPNUM_RELEASE:
		case CLRPC_PNIO_OPNUM_READ:
		case CLRPC_PNIO_OPNUM_WRITE:
		case CLRPC_PNIO_OPNUM_CONTROL:
			CLRPC_ASSERT(! (if_obj->ar_count_curr >= if_obj->ar_count_max));
			if_obj->ar_count_curr += 1;
			break;

		case CLRPC_PNIO_OPNUM_READ_IMPLICIT:
			CLRPC_ASSERT(! (if_obj->read_implicit_curr >= if_obj->read_implicit_max));
			if_obj->read_implicit_curr += 1;
			break;

		default:
			CLRPC_ASSERT(0);
			break;
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_if_obj_opnum_count_decr (
	CLRPC_IF_OBJECT_PTR_TYPE  if_obj,
	LSA_UINT16  has_pnio_signature,
	LSA_UINT16  opnum
	)
{
	CLRPC_LOG_FCT("clrpc_if_obj_opnum_count_decr")

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (if_obj));

	if (has_pnio_signature) {
		switch (opnum) {
		case CLRPC_PNIO_OPNUM_CONNECT:
		case CLRPC_PNIO_OPNUM_RELEASE:
		case CLRPC_PNIO_OPNUM_READ:
		case CLRPC_PNIO_OPNUM_WRITE:
		case CLRPC_PNIO_OPNUM_CONTROL:
			CLRPC_ASSERT(if_obj->ar_count_curr > 0);
			if_obj->ar_count_curr -= 1;
			break;

		case CLRPC_PNIO_OPNUM_READ_IMPLICIT:
			CLRPC_ASSERT(if_obj->read_implicit_curr > 0);
			if_obj->read_implicit_curr -= 1;
			break;

		default:
			CLRPC_ASSERT(0);
			break;
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_opnum_count_decr (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_UPPER_RQB_PTR_TYPE  upper
	)
{
	clrpc_if_obj_opnum_count_decr (& scall->server->obj
		, scall->server->pnio_signature, upper->args.sv.call->opnum
		);
}


/*------------------------------------------------------------------------------
//	clrpc_sc_do_init
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_init (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_init")
	CLRPC_UPPER_RQB_PTR_TYPE  upper;
	CLRPC_LOWER_RQB_PTR_TYPE  lower;

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "cid(%u) curr-state(%u) new-state(CST_INIT)"
		, scall->id
		, scall->state
		);

	/*
	//	on change: check this sequence against clrpc_sc_do_free()
	*/

/* not so good: see clrpc_sc_ev_request() new req
	CLRPC_ASSERT (scall->upper_wf == 0);
	CLRPC_ASSERT (scall->upper_drop == 0);
*/

		/* (re)start timer for self destruction */
	clrpc_oneshot_trigger (& scall->oneshot, CLRPC_CFG_SCALL_DIE_TIMEOUT_MSEC);
	scall->oneshot_queue = 0;

		/* free reserved resources */
	if (CLRPC_PTR_IS_NOT_NULL (scall->upper)) {
		clrpc_sc_opnum_count_decr (scall, scall->upper);
		clrpc_sv_provide_enqueue (scall->server, & scall->upper);
		scall->up_call = LSA_NULL;
	}

		/* stop way */
	if (scall->way.state == CLRPC_WAY_ST_RUNNING) {
		scall->way.state = CLRPC_WAY_ST_UNKNOWN;
		clrpc_hash_remove (& clrpc_sc_hash, & scall->way.hashed_act.uuid);
	}

		/* drop from send-queue */
	if (scall->send.in_queue) {
		clrpc_queue_unlink (& scall->server->send_scall_queue, scall);
		scall->send.in_queue = 0;
	}

		/* free lower queue */
	lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->recv.queue);
	while (CLRPC_PTR_IS_NOT_NULL (lower)) {
		clrpc_sv_ll_recv_provide (scall->server, lower);
		lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->recv.queue);
	}

		/* free upper queue */
	upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->send.answer_queue);
	while (CLRPC_PTR_IS_NOT_NULL (upper)) {
		clrpc_sc_opnum_count_decr (scall, upper);
		clrpc_sv_provide_enqueue (scall->server, & upper);
		upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->send.answer_queue);
	}

		/* done */
	scall->state = CLRPC_CST_INIT;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_do_recv
//----------------------------------------------------------------------------*/
static LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_recv (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_recv")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt;
	CLRPC_IF_OBJECT_PTR_TYPE  if_obj;

	CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt);

	CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) curr-state(%u) new-state(CST_RECV) seq(%u) fragnum(%u)"
		, scall->id, scall->state, scall->seq, scall->recv.fragnum
		);
	CLRPC_SV_TRACE_06 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, " pkt.opnum(%u).seq(%u).sem(%u).fragnum(%u).flags(0x%x).len(%u)"
		, pkt->opnum, pkt->seq, clrpc_pkt_get_semantics (pkt), pkt->fragnum, pkt->flags, pkt->len
		);

		/* (re)init */
	if (scall->state == CLRPC_CST_INIT) {

			/* init some elements on every new request */
		scall->seq			= CLRPC_CMP32 (pkt->seq, >, scall->seq) ? pkt->seq : scall->seq; /* maybe sets older value, keep bigger */
		scall->send.serial	= (LSA_UINT16)(-1);

		scall->recv.fragnum	= 0xFFFF;
		scall->recv.host	= CLRPC_UDP_IP(udp_recv);
		scall->recv.port	= CLRPC_UDP_PORT(udp_recv);

			/* is opnum in range ? */
		if (pkt->opnum > scall->server->opnum_max) {
			CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cid(%u) pkt.opnum(%u) > opnum_max(%u)"
				, scall->id, pkt->opnum, scall->server->opnum_max

				);
			clrpc_sv_send_reject (scall->server, lower, CLRPC_NCA_OP_RANGE_ERR);
			return;
		}

		if_obj = & scall->server->obj; /* AP01068070 */

			/* check for allowed parallel opcodes
			//	increment is done with dequeue(upper)
			//	decrement is done with enqueue(upper)
			//----------------------------------------------------------------*/
		if (! clrpc_if_obj_opnum_count_ok (if_obj, scall->server->pnio_signature, pkt->opnum)) {
			clrpc_sv_send_reject (scall->server, lower, CLRPC_NCA_SERVER_TOO_BUSY);
			return;
		}

			/* reserve an rqb for upper indication */
		CLRPC_ASSERT (CLRPC_PTR_IS_NULL (scall->upper));
		scall->upper = clrpc_sv_provide_dequeue (scall->server);
		if (CLRPC_PTR_IS_NULL (scall->upper)) {
			CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "sid(%u): no upper indication resource for cid(%u), send too_busy"
				, scall->server->id, scall->id
				);
			clrpc_sv_send_reject (scall->server, lower, CLRPC_NCA_SERVER_TOO_BUSY);
			return;
		}

			/* now only clrpc_sc_.. calls are allowed */
		scall->state		= CLRPC_CST_RECV;

			/* write data to reserved upper rqb */
#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("VV: drop this ugly up_call...")
#endif /* CLRPC_MESSAGE */
		CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->upper->args.sv.call));
		scall->up_call = scall->upper->args.sv.call;

		scall->up_call->server_id		= scall->server->id;
		scall->up_call->scall_id		= scall;

		scall->up_call->usr_id			= if_obj->usr_id;
		scall->up_call->host_ip			= CLRPC_UDP_IP(udp_recv);
		scall->up_call->udp_port		= CLRPC_UDP_PORT(udp_recv);
		scall->up_call->opnum			= pkt->opnum;
		scall->up_call->semantics		= clrpc_pkt_get_semantics (pkt);
		scall->up_call->drep.drep_char	= pkt->drep[0] & 0x0F;
		scall->up_call->drep.drep_int	= pkt->drep[0] & 0xF0;
		scall->up_call->drep.drep_float	= pkt->drep[1];
		CLRPC_MEMCPY (& scall->up_call->object_id
										, & pkt->object, sizeof (scall->up_call->object_id));
		scall->up_call->alloc_len		= 0;
		scall->up_call->ndr_len			= 0;
		scall->up_call->ndr_data		= LSA_NULL;
		scall->up_call->pnio_status		= CLRPC_PNIO_STATUS_INVALID;

			/* increment running opnum-count
			//	decrement with clrpc_sv_provide_enqeue (scall->upper)
			//----------------------------------------------------------------*/
		clrpc_if_obj_opnum_count_incr (if_obj
			, scall->server->pnio_signature, scall->up_call->opnum
			);

		CLRPC_ASSERT (scall->server->alloc_len <= 0xFFFF); /* see clrpc_sv_op_register_internal() */
		if (! scall->server->pnio_signature) {
			scall->up_call->alloc_len	= scall->server->alloc_len;
		}
		else {
			scall->up_call->alloc_len	= clrpc_sv_pnio_fetch_alloc_len (pkt);
			if (scall->up_call->alloc_len == 0) {
				CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong alloc_len, max(%u), reject call, from.ip(0x08%x)"
					, scall->server->alloc_len,	CLRPC_UDP_IP(udp_recv)
					);
				CLRPC_ASSERT (scall->up_call->alloc_len == 0); /* must ! */
				clrpc_sc_send_error (scall, CLRPC_PT_FAULT, CLRPC_NCA_FAULT_REMOTE_NO_MEMORY);
				return;
			}
				/* dynamically restrict to max allowed size */
			if (scall->up_call->alloc_len > scall->server->alloc_len) {
				CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "resize call.alloc_len(%u) to server.alloc_len(%u)"
					, scall->up_call->alloc_len, scall->server->alloc_len
					);
				scall->up_call->alloc_len = scall->server->alloc_len;
			}
		}

			/* alloc data for the call */
		if (scall->up_call->alloc_len > 0) {
			CLRPC_ASSERT (scall->up_call->alloc_len <= 0xFFFF);
			CLRPC_ALLOC_UPPER_MEM (CLRPC_CAST_UPPER_MEM_PTR_PTR (& scall->up_call->ndr_data)
				, clrpc_null_user_id
				, (LSA_UINT16)scall->up_call->alloc_len, scall->server->channel->sysptr
				);
			if (CLRPC_PTR_IS_NULL (scall->up_call->ndr_data)) {
				scall->up_call->alloc_len = 0;
				CLRPC_SV_TRACE_00 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "no upper mem, drop pkt, try later on rpc-retry"
					);
				clrpc_sc_do_init (scall);
				return;
			}
				/* ndr_data must aligned to long (at least) */
			if (((LSA_UINT32)(scall->up_call->ndr_data) & 3) != 0) {
				CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ndr_data(%08x) wrong alignment, reject call"
					, scall->up_call->ndr_data
					);
				clrpc_sc_send_error (scall, CLRPC_PT_FAULT, CLRPC_NCA_FAULT_REMOTE_NO_MEMORY);
				return;
			}
		}

		CLRPC_SV_TRACE_07 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cid(%u) from.ip(%#x) from.port(%u) opnum(%u) drep(%x) sem(%u) alloc_len(%u)"
			, scall->id
			, CLRPC_NTOHL (scall->up_call->host_ip)
			, CLRPC_NTOHS (scall->up_call->udp_port)
			, scall->up_call->opnum
			, scall->up_call->drep.drep_char & scall->up_call->drep.drep_int
			, scall->up_call->semantics, scall->up_call->alloc_len
			);
		CLRPC_SV_TRACE_12 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cid(%u) obj_id(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, scall->id
			, scall->up_call->object_id.time_low, scall->up_call->object_id.time_mid, scall->up_call->object_id.time_hi_and_version
			, scall->up_call->object_id.clock_seq_hi_and_reserved, scall->up_call->object_id.clock_seq_low
			, scall->up_call->object_id.node[0], scall->up_call->object_id.node[1], scall->up_call->object_id.node[2]
			, scall->up_call->object_id.node[3], scall->up_call->object_id.node[4], scall->up_call->object_id.node[5]
			);
	}

	CLRPC_ASSERT (scall->state & CLRPC_CST_RECV);
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->upper));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->up_call));

		/* (re)start timer for self destruction */
	clrpc_oneshot_trigger (& scall->oneshot, CLRPC_CFG_SCALL_DIE_TIMEOUT_MSEC);

/* AP00642069: because of lost FACK or rerun, first check fragnum, then check alloc_len */

		/* if frags: check order */
	if ((pkt->flags & CLRPC_PF_FRAG)
	&& (LSA_UINT16)(scall->recv.fragnum + 1) != pkt->fragnum)	/* critical: 0xFFFF + 1 */
	{
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "frags out of order scall.fragnr(%u) + 1 != pkt.fragnr(%u), drop pkt"
			, scall->recv.fragnum, pkt->fragnum
			);
			/* always send fack, if desired, even in out of order */
		if (! (pkt->flags & CLRPC_PF_NO_FACK)) {
			clrpc_sc_send_fack (scall, CLRPC_PKT_GET_SERIAL (pkt));
		}
		return;
	}
	scall->recv.fragnum = pkt->fragnum;

		/* single-req and frag add here data to the reserved upper rqb */
		/* check: does it fit in the alloc range ? */
	if (scall->up_call->ndr_len + pkt->len > scall->up_call->alloc_len) {
		CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "more data (hold(%u) + pkt.len(%u)) than alloc_len(%u)"
			, scall->up_call->ndr_len, pkt->len, scall->up_call->alloc_len
			);
		clrpc_sc_send_error (scall, CLRPC_PT_FAULT, CLRPC_NCA_FAULT_REMOTE_NO_MEMORY);
		return;
	}

		/* now check if we have do send an answer
		//	because of the limitation of the send_queue, we can only send one pkt.
		//	case 1. must send a fack
		//	case 2. must send a way
		//	case 3. must send a fack & way
		//	case 4. none
		//
		//	on case 3. we send a fack and keep the way until the state wf_way is
		//	reaching or case 2. or 4. is happening.
		*/
		/* if frags: must send a fack ? */
	if ((pkt->flags & CLRPC_PF_FRAG)  &&  !(pkt->flags & CLRPC_PF_NO_FACK)) {
		clrpc_sc_send_fack (scall, CLRPC_PKT_GET_SERIAL (pkt));
	}
		/* at-most-once: if we don't know the way state, we had to start a reguest to the client */
	else if (scall->up_call->semantics == CLRPC_SEMANTICS_AT_MOST_ONCE
	&& scall->way.state == CLRPC_WAY_ST_UNKNOWN)
	{
		clrpc_sc_send_way (scall);
	}

		/* !! this is the only real work !! */
	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "copy pktlen(%u) to offset(%u) "
		, pkt->len
		, scall->up_call->ndr_len
		);
	CLRPC_MEMCPY (scall->up_call->ndr_data + scall->up_call->ndr_len, pkt + 1, pkt->len);
	scall->up_call->ndr_len += pkt->len;

		/* special frag-case: there is more frag-data, so keep in state */
	if ((pkt->flags & CLRPC_PF_FRAG) && !(pkt->flags & CLRPC_PF_LAST_FRAG)) {
		return;
	}

		/* only single-REQUEST and LAST_FRAG should reach here */
	clrpc_sc_do_wf_way (scall);
	return;
}


/*------------------------------------------------------------------------------------
//	clrpc_sc_do_wf_way
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_wf_way (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_wf_way")

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "cid(%u) curr-state(%u) new-state(CST_WF_WAY) way.state(%u)"
		, scall->id
		, scall->state
		, scall->way.state
		);

	CLRPC_ASSERT (scall->state & CLRPC_CST_RECV);

	scall->state = CLRPC_CST_WF_WAY;

		/* stop recv-timeout */
	clrpc_oneshot_stop (& scall->oneshot);

	if (scall->up_call->semantics == CLRPC_SEMANTICS_AT_MOST_ONCE
	&& scall->way.state != CLRPC_WAY_ST_OK)
	{
		/* if way.state == UNKNOWN: timeout will send the first time, see clrpc_sc_ev_timeout() */
		CLRPC_SV_TRACE_00 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "keep in state, wait for response"
			);
		scall->way.timeout_cnt = 0;
#if 0 /* testcase: can't send within timeout */
# ifdef CLRPC_MESSAGE
#  pragma CLRPC_MESSAGE("error: testcase: timeout ist passiert")
# endif /* CLRPC_MESSAGE */
		clrpc_oneshot_trigger (& scall->oneshot, 1);
#else
		clrpc_oneshot_trigger (& scall->oneshot, CLRPC_RTX_TIMEOUT_MSEC);
#endif

		return;
	}

	clrpc_sc_do_wf_answer (scall);
}


/*------------------------------------------------------------------------------
//	clrpc_sc_do_wf_answer
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_wf_answer (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_wf_answer")

	CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "cid(%u) curr-state(%u) new-state(CST_WF_ANSWER) upper_wf(%u) upper_drop(%u)"
		, scall->id
		, scall->state
		, scall->upper_wf
		, scall->upper_drop
		);

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->upper));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->up_call));

		/* stop timeout, user has not timing restrictions */
	clrpc_oneshot_stop (& scall->oneshot);

	if (scall->up_call->semantics == CLRPC_SEMANTICS_AT_MOST_ONCE) {
		CLRPC_ASSERT (scall->way.state == CLRPC_WAY_ST_OK);
	}

		/* fill the upper rqb */
	CLRPC_RQB_SET_OPCODE (scall->upper, CLRPC_OPC_SV_CALL_INDICATION);
	scall->up_call->server_id = scall->server->id;
	scall->up_call->scall_id = scall;

		/* call the manager */
	scall->state = CLRPC_CST_WF_ANSWER;

	if (scall->upper_wf == 0) {
		LSA_BOOL is_maybe;

		scall->upper_wf = 1;

		is_maybe = (scall->up_call->semantics == CLRPC_SEMANTICS_MAYBE
				|| scall->up_call->semantics == CLRPC_SEMANTICS_BROADCAST_MAYBE)
			;

			/* pnio */
		if (scall->server->pnio_signature) {
			LSA_UINT32  nca_status = clrpc_sv_pnio_demarshal (scall, scall->upper);
			if (nca_status != 0) {
				CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cid(%u) nca_status(%u), send reject"
					, scall->id, nca_status
					);
				clrpc_sc_send_error (scall, CLRPC_PT_REJECT, nca_status);
				return;
			}
		}

			/* do the callback */
		if (scall->server->is_endpoint_mapper > 0) {
			clrpc_epm_callback (scall, CLRPC_OK, & scall->upper);
		}
		else {
			clrpc_callback (scall->server->channel, CLRPC_OK, & scall->upper, CLRPC_MODULE_ID, __LINE__);
		}
		scall->up_call = LSA_NULL;

			/* on maybe: work is done
			//	the user must answer, but answer does only provide,
			//	see clrpc_sv_op_call_response()
			//	do not switch to init-state, see upper_wf
			//-----------------------------------------------------------*/
		if (is_maybe) {
			scall->upper_drop = 1;
		}
	}
	else {
			/* must wait for 'old' response */
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "cid(%u) must wait for 'old' response"
			, scall->id
			);
		CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->upper));
		CLRPC_ASSERT (scall->upper_drop > 0);
	}
}


/*-------------------------------------------------------------------------------------
//	clrpc_sc_do_send
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_send (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_send")
	CLRPC_ASSERT ((scall->state == CLRPC_CST_WF_ANSWER));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->upper));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->up_call));

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "cid(%u) curr-state(%u) new-state(CST_SEND) upper.rsp(%u)"
		, scall->id
		, scall->state
		, CLRPC_RQB_GET_RESPONSE (scall->upper)
		);

		/* we are now in state send */
	scall->state = CLRPC_CST_SEND;

		/* the user want to send a reject */
	if (CLRPC_RQB_GET_RESPONSE (scall->upper) == CLRPC_ERR_REJECTED) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
			, "cid(%u) should send reject, we send fault"
			, scall->id
			);
        /* AP00651106: Microsoft RPC repeats the call when receiving a REJECT with CLRPC_NCA_UNSPECIFIC_REJECT for seconds.
           To stop this storm of telegramms we use the FAULT with CLRPC_NCA_FAULT_UNSPECIFIC instead. */
		//clrpc_sc_send_error (scall, CLRPC_PT_REJECT, CLRPC_NCA_UNSPECIFIC_REJECT);
		clrpc_sc_send_error (scall, CLRPC_PT_FAULT, CLRPC_NCA_FAULT_UNSPECIFIC);
		return;
	}
		/* the user want to send a fault */
	if (CLRPC_RQB_GET_RESPONSE (scall->upper) == CLRPC_ERR_FAULTED) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
			, "cid(%u) send fault"
			, scall->id
			);
			/* prepare fault */
		clrpc_sc_send_error (scall, CLRPC_PT_FAULT, CLRPC_NCA_FAULT_UNSPECIFIC);
		return;
	}

		/* prepare send */
	scall->send.ptype = CLRPC_PT_RESPONSE;
	scall->send.frag_window_size = CLRPC_CFG_SV_SEND_WINDOW_SIZE;
	scall->send.frag_ack_fragnum = (LSA_UINT16)(-1);
	scall->send.frag_curr_fragnum = (LSA_UINT16)(scall->send.frag_ack_fragnum + 1);

		/* data fits in one paket */
	if (scall->up_call->ndr_len <= CLRPC_MUST_RECV_BODY_SIZE) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "one pkt, len(%u)"
			, scall->up_call->ndr_len
			);
		scall->send.frag_flags = 0;
		scall->send.frag_last_fragnum = 0;
	}

		/* else must send fragments */
	else {
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "frag pkt, len(%u) FRAG_SIZE(%u)"
			, scall->up_call->ndr_len
			, CLRPC_MUST_RECV_BODY_SIZE
			);
		scall->send.frag_flags = CLRPC_PF_FRAG;
		scall->send.frag_last_fragnum = CLRPC_CAST_U16(scall->up_call->ndr_len / CLRPC_MUST_RECV_BODY_SIZE);
	}

		/* queue send request */
	clrpc_sc_post_to_send_queue (scall);
}


/*------------------------------------------------------------------------------
//	clrpc_sc_do_wf_ack
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_wf_ack (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_wf_ack")

	CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) curr-state(%u) new-state(CST_WF_ACK) semantics(%u) frag_flags(0x%x)"
		, scall->id, scall->state, scall->up_call->semantics, scall->send.frag_flags
		);

	CLRPC_ASSERT (scall->state & (CLRPC_CST_SEND | CLRPC_CST_WF_ANSWER));

		/* keep resources if "at-most-once" or (answer doesn't fit in one frag / AP00820286) */
	if ((scall->up_call->semantics == CLRPC_SEMANTICS_AT_MOST_ONCE)
	||	(scall->send.frag_flags & CLRPC_PF_FRAG))
	{
		/* do not free reserved resources, because of re-send the answer, see ping, see fack */

		/* set timer for self-destruction */
		clrpc_oneshot_trigger (& scall->oneshot, CLRPC_CFG_SCALL_DIE_TIMEOUT_MSEC);
		scall->state = CLRPC_CST_WF_ACK;
	}
	else
	{
		clrpc_sc_do_init (scall); /* orphan not needed, see clrpc_sv_op_unregister_internal() */
	}
}


/*------------------------------------------------------------------------------
//	clrpc_sc_do_orphan
//----------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_orphan (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_orphan")

	CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "cid(%u) curr-state(%u) new-state(CST_ORPHAN) upper_wf(%u) way(%u)"
		, scall->id
		, scall->state
		, scall->upper_wf
		, scall->way.state
		);

	if (scall->way.state == CLRPC_WAY_ST_RUNNING) {
		scall->way.state = CLRPC_WAY_ST_UNKNOWN;
		clrpc_hash_remove (& clrpc_sc_hash, & scall->way.hashed_act.uuid);
	}

	scall->state = CLRPC_CST_ORPHAN;
	if (scall->upper_wf > 0)
	{
		/* keep state orphan */
		scall->upper_drop = 1;
	}
	else
	{
		clrpc_sc_do_free (scall);
	}
}


/*------------------------------------------------------------------------------
//	clrpc_sc_do_free
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_free (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_do_free")
	CLRPC_UPPER_RQB_PTR_TYPE  upper;
	CLRPC_LOWER_RQB_PTR_TYPE  lower;

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "cid(%u) curr-state(%u) new-state(CST_FREE)"
		, scall->id
		, scall->state
		);

	/*
	//	on change: check this sequence against clrpc_sc_do_init()
	*/

	CLRPC_ASSERT (CLRPC_PTR_ARE_EQUAL (scall->self, scall));
	CLRPC_ASSERT (scall->state != CLRPC_CST_FREE);

	CLRPC_ASSERT (scall->upper_wf == 0);

		/* stop timer (if any) in timeout_queue */
	clrpc_oneshot_stop (& scall->oneshot);
	scall->oneshot_queue = 0;

		/* free reserved resources */
	if (CLRPC_PTR_IS_NOT_NULL (scall->upper)) {
		clrpc_sc_opnum_count_decr (scall, scall->upper);
		clrpc_sv_provide_enqueue (scall->server, & scall->upper);
		scall->up_call = LSA_NULL;
	}

		/* stop way */
	if (scall->way.state == CLRPC_WAY_ST_RUNNING) {
		scall->way.state = CLRPC_WAY_ST_UNKNOWN;
		clrpc_hash_remove (& clrpc_sc_hash, & scall->way.hashed_act.uuid);
	}

		/* drop from send-queue */
	if (scall->send.in_queue) {
		clrpc_queue_unlink (& scall->server->send_scall_queue, scall);
		scall->send.in_queue = 0;
	}

		/* free lower queue */
	lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->recv.queue);
	while (CLRPC_PTR_IS_NOT_NULL (lower)) {
		clrpc_sv_ll_recv_provide (scall->server, lower);
		lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->recv.queue);
	}

		/* free upper queue */
	upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->send.answer_queue);
	while (CLRPC_PTR_IS_NOT_NULL (upper)) {
		clrpc_sc_opnum_count_decr (scall, upper);
		clrpc_sv_provide_enqueue (scall->server, & upper);
		upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->send.answer_queue);
	}

		/* scall done */
	clrpc_hash_remove (& clrpc_sc_hash, & scall->hashed_act.uuid);

	scall->state = CLRPC_CST_FREE; /* mem free is done in clrpc_sc_dispatch() */
}


/*------------------------------------------------------------------------------
//	clrpc_sc_ev_request
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_request (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sc_ev_request")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt;

	CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt);

	CLRPC_SV_TRACE_10 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) REQUEST: state(%u) seq:pkt(%u)sc(%u)"
		 " op(%u) sem(%u) fragnum:pkt(%u)sc(%u) flags(%u) len(%u)"
		, scall->id
		, scall->state
		, pkt->seq
		, scall->seq
		, pkt->opnum
		, clrpc_pkt_get_semantics (pkt)
		, pkt->fragnum
		, scall->recv.fragnum
		, pkt->flags
		, pkt->len
		);


		/* check server_boot */
	if (pkt->server_boot == 0) {
		pkt->server_boot = scall->server->server_boot;
	}
	if (pkt->server_boot != scall->server->server_boot) {
		CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cid(%u) pkt.boot(%u) != sc.boot(%u)"
			, scall->id, pkt->server_boot, scall->server->server_boot
			);
		clrpc_sv_send_reject (scall->server, lower, CLRPC_NCA_WRONG_BOOT_TIME);
		return CLRPC_PROVIDE_LOWER;
	}

		/* check INDY */
	if (clrpc_uuid_equal (& clrpc_convc_uuid, & pkt->if_id)) {
		clrpc_sc_indy (scall, pkt);
		return CLRPC_PROVIDE_LOWER;
	}

		/* an old pkt, drop it except maybe-calls */
	if (CLRPC_CMP32 (pkt->seq, <, scall->seq)) {
		if (! (pkt->flags & CLRPC_PF_MAYBE)) {
			CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "cid(%u) pkt.seq(%u) < sc.seq(%u), drop pkt"
				, scall->id
				, pkt->seq
				, scall->seq
				);
			return CLRPC_PROVIDE_LOWER;
		}
			/* can we work it now, or have to wait ? */
		if (! (scall->state & (CLRPC_CST_INIT | CLRPC_CST_WF_ACK))) {
			return CLRPC_QUEUE_AGAIN;
		}
		clrpc_sc_do_init (scall);
	}

		/* is it a new req: check the old state */
	else if (CLRPC_CMP32 (pkt->seq, >, scall->seq)) {
			/* init:		init(), normal case: scall.seq starts with -1
			// recv:		init(), we must have missed a quit,  see dgslsn.c(963)
			// wf_way:		init(), we must have missed a quit
			// wf_answer:	init(), we must have missed a quit, but upper indicated to the user
			// send:		init(), we must have missed a quit
			// wf_ack:		init(), normal case: each new req is an implicit ack
			// orphan:		reject
			*/
		if (scall->state == CLRPC_CST_ORPHAN) {
			CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cid(%u), state(orphan), send reject"
				, scall->id
				);
            /* AP00651106: Microsoft RPC repeats the call when receiving a REJECT with CLRPC_NCA_UNSPECIFIC_REJECT for seconds.
           To stop this storm of telegramms we use CLRPC_NCA_SERVER_TOO_BUSY instead. */
			clrpc_sv_send_reject (scall->server, lower, CLRPC_NCA_SERVER_TOO_BUSY);
			return CLRPC_PROVIDE_LOWER;
		}

		if (scall->state == CLRPC_CST_WF_ANSWER) {
			CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
				, "cid(%u), state(wf_answer), set upper_drop!  old(%u)"
				, scall->id, scall->upper_drop
				);
			/* the scall->upper is posted to the user, must wait for user-response */
			CLRPC_ASSERT (scall->upper_wf > 0);
			scall->upper_drop = 1; /* a call is indicated to the user, this call.rsp must drop */
		}
		else {
			CLRPC_ASSERT ((scall->upper_wf == 0  &&  scall->upper_drop == 0)
					||	  (scall->upper_wf == 1  &&  scall->upper_drop == 1)
				);
		}

		clrpc_sc_do_init (scall);
	}

		/* req for current scall */
	else {
			/* init:		idem:(re)run,init()		atmo:fragnum==0:reject(nca_s_proto_error) else:ignore
			// recv:		fall down, check later, normal frags-recv case
			// wf_way:		all pkt recv, ?ignore, see send
			// wf_answer:	all pkt recv, ?ignore, see send
			// send:		'Since we're already transmitting, just ignore this'
			// wf_ack:		restart sending, see ping
			// orphan:		ignore
			*/
		if (scall->state & (CLRPC_CST_WF_WAY | CLRPC_CST_WF_ANSWER | CLRPC_CST_SEND | CLRPC_CST_ORPHAN)) {
			CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
				, "cid(%u), equal seq, state(%u), ignore pkt"
				, scall->id, scall->state
				);
			return CLRPC_PROVIDE_LOWER;
		}
		else if (scall->state == CLRPC_CST_INIT) {
			if (! (pkt->flags & CLRPC_PF_IDEMPOTENT)) {
				if ((pkt->flags & CLRPC_PF_FRAG)  &&  (pkt->fragnum != 0)) {
					CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
						, "cid(%u), equal seq, fragnum(%u) state(%u), ignore pkt"
						, scall->id, pkt->fragnum, scall->state
						);
					return CLRPC_PROVIDE_LOWER;
				}
				CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cid(%u), atmo renum, send reject(proto_error)"
					, scall->id
					);
				clrpc_sv_send_reject (scall->server, lower, CLRPC_NCA_PROTO_ERROR);
				return CLRPC_PROVIDE_LOWER;
			}
		}
		else if (scall->state == CLRPC_CST_WF_ACK) {
			return clrpc_sc_ev_ping (scall, lower);
		}
		else {
			CLRPC_ASSERT (scall->state == CLRPC_CST_RECV);
		}
	}

	clrpc_sc_do_recv (scall, lower);
	return CLRPC_PROVIDE_LOWER;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_ev_ping
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_ping (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sc_ev_ping")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt;

	CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt);

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "cid(%u) PING: state(%u)"
		, scall->id
		, scall->state
		);

		/* is it a newer paket ? (A packet must have been lost.) */
	if (CLRPC_CMP32 (pkt->seq, >, scall->seq)) {
		CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "cid(%u) pkt.seq(%u) > curr.seq(%u), send nocall"
			, scall->id, pkt->seq, scall->seq
			);
		clrpc_sv_send_nocall (scall->server, lower, 0xFFFF);
		return CLRPC_PROVIDE_LOWER;
	}

		/* is it an old paket ? */
	if (CLRPC_CMP32 (pkt->seq, <, scall->seq)) {
		CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "cid(%u) pkt.seq(%u) < curr.seq(%u), drop pkt"
			, scall->id, pkt->seq, scall->seq
			);
		return CLRPC_PROVIDE_LOWER;
	}

	scall->recv.host	= CLRPC_UDP_IP(udp_recv);
	scall->recv.port	= CLRPC_UDP_PORT(udp_recv);

		/* a ping for the current scall */
	switch (scall->state) {
	case CLRPC_CST_INIT:
	case CLRPC_CST_ORPHAN:
		/* nocall */
		clrpc_sv_send_nocall (scall->server, lower, scall->recv.fragnum);
		break;

	case CLRPC_CST_RECV:
		/* nocall with fack-body */
		clrpc_sv_send_nocall_fack_body (scall->server, lower, scall->recv.fragnum,
				1/*?window_size*/, CLRPC_PKT_GET_SERIAL (pkt)
			);
		break;

	case CLRPC_CST_WF_WAY:
	case CLRPC_CST_WF_ANSWER:
		/* working */
		clrpc_sc_send_working (scall);
		break;

	case CLRPC_CST_SEND:
	case CLRPC_CST_WF_ACK:
		/* response */
		CLRPC_ASSERT (scall->send.ptype == CLRPC_PT_RESPONSE);
		scall->send.frag_curr_fragnum = (LSA_UINT16)(scall->send.frag_ack_fragnum + 1);
		clrpc_sc_post_to_send_queue (scall);
		break;

	default:
	case CLRPC_CST_FREE:
		CLRPC_ASSERT (0); /* "pgm error" */
		break;
	}

	return CLRPC_PROVIDE_LOWER;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_ev_quit
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_quit (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sc_ev_quit")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt_recv;
	CLRPC_QUIT_BODY_PTR_TYPE  quit_body;

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "cid(%u) QUIT: state(%u)"
		, scall->id
		, scall->state
		);

	CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt_recv);
	quit_body = CLRPC_PKT_GET_BODY (pkt_recv, CLRPC_QUIT_BODY_PTR_TYPE);

		/* ignore old quit */
	if (CLRPC_CMP32 (pkt_recv->seq, <, scall->seq)) {
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "pkt.seq(%u) < seq(%u), ignore"
			, pkt_recv->seq, scall->seq
			);
		return CLRPC_PROVIDE_LOWER;
	}

	scall->recv.host = CLRPC_UDP_IP(udp_recv);
	scall->recv.port = CLRPC_UDP_PORT(udp_recv);

		/* if it is a orphan quit */
	if (pkt_recv->len == 0) {
		clrpc_sc_send_quack (scall, (LSA_UINT32)(-1));
		return CLRPC_PROVIDE_LOWER;
	}

		/* else it's a cancel quit */
	if (pkt_recv->len != CLRPC_QUIT_BODY_SIZE) {
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "pkt.len(%u) != QUIT.len(%u)"
			, pkt_recv->len
			, CLRPC_QUIT_BODY_SIZE
			);
		return CLRPC_PROVIDE_LOWER;
	}

	if (CLRPC_DREP_INT_DIFFERS (pkt_recv->drep[0])) {
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "need swap! pkt.drep(0x%x) != (0x%x)"
			, pkt_recv->drep[0]
			, CLRPC_DREP_0
			);
		CLRPC_SWAP_U32 (& quit_body->vers);
		CLRPC_SWAP_U32 (& quit_body->cancel_id);
	}

	if (quit_body->vers != 0) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "vers(%u) != 0"
			, quit_body->vers
			);
		return CLRPC_PROVIDE_LOWER;
	}

		/* ignore old cancel_id */
	if (CLRPC_CMP32 (quit_body->cancel_id, <, scall->recv.cancel_id)) {
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
			, "quit.cancel_id(%u) < cancel_id(%u), ignore"
			, quit_body->cancel_id
			, scall->recv.cancel_id
			);
		return CLRPC_PROVIDE_LOWER;
	}

	scall->recv.cancel_id = quit_body->cancel_id;

	CLRPC_SV_TRACE_03 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) pkt.len(%u) cancel_id(%u)"
		, scall->id
		, pkt_recv->len
		, scall->recv.cancel_id
		);

	clrpc_sc_send_quack (scall, quit_body->cancel_id);

	return CLRPC_PROVIDE_LOWER;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_ev_ack
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_ack (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sc_ev_ack")

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "cid(%u) ACK: state(%u)"
		, scall->id
		, scall->state
		);

	LSA_UNUSED_ARG(lower);

	clrpc_sc_do_orphan (scall);
	return CLRPC_PROVIDE_LOWER;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_ev_fack
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_fack (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sc_ev_fack")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt_recv;

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "cid(%u) FACK: state(%u)"
		, scall->id
		, scall->state
		);

	CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt_recv);

	if (pkt_recv->seq != scall->seq) {
		CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cid(%u) state(%u) pkt.seq(%u) != seq(%u), ignore pkt"
			, scall->id
			, scall->state
			, pkt_recv->seq
			, scall->seq
			);
		return CLRPC_PROVIDE_LOWER;
	}

	scall->recv.host = CLRPC_UDP_IP(udp_recv);
	scall->recv.port = CLRPC_UDP_PORT(udp_recv);

#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("ignore fack-body")
#endif /* CLRPC_MESSAGE */

	switch (scall->state) {
	case CLRPC_CST_SEND:
	case CLRPC_CST_WF_ACK:
		{
		LSA_UINT16  keep_curr_fragnum = scall->send.frag_curr_fragnum;

		CLRPC_ASSERT (scall->send.ptype == CLRPC_PT_RESPONSE);

		scall->send.frag_ack_fragnum = pkt_recv->fragnum;
		scall->send.frag_curr_fragnum = (LSA_UINT16)(scall->send.frag_ack_fragnum + 1);
		CLRPC_SV_TRACE_05 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cid(%u) state(%u)  fragnums: ack(%u) curr(%u) last(%u)"
			, scall->id, scall->state
			, scall->send.frag_ack_fragnum, scall->send.frag_curr_fragnum
			, scall->send.frag_last_fragnum
			);
		if (keep_curr_fragnum > scall->send.frag_curr_fragnum) /*a trace for AP00820286*/
		{
			CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
				, "cid(%u) state(%u)  keep_curr_fragnum(%u) acked_fragnum(%u), a frag was lost"
				, scall->id, scall->state
				, keep_curr_fragnum, scall->send.frag_curr_fragnum
				);
		}

			/* there could be a fack, even if the server doesn't ask for one */
		if (scall->send.frag_ack_fragnum < scall->send.frag_last_fragnum) {
				/* queue next send request */
			clrpc_sc_post_to_send_queue (scall);
		}
		}
		break;

	default:
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cid(%u) state(%u) unexpected, ignore pkt"
			, scall->id, scall->state
			);
		break;
	}
	return CLRPC_PROVIDE_LOWER;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_ev_response
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_response (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sc_ev_response")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt_recv;
	CLRPC_WAY2_RSP_PTR_TYPE  way_body;

	CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "cid(%u) RESPONSE: state(%u) seq(%u) way.state(%u)"
		, scall->id
		, scall->state
		, scall->seq
		, scall->way.state
		);

#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("todo: check clrpc_conversation_manager_uuid first")
#endif /* CLRPC_MESSAGE */

	CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt_recv);
	way_body = CLRPC_PKT_GET_BODY (pkt_recv, CLRPC_WAY2_RSP_PTR_TYPE);

	CLRPC_SV_TRACE_05 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) pkt.opnum(%u) .len(%u) .way.actseq(%u) .way.ncastat(%u)"
		, scall->id, pkt_recv->opnum, pkt_recv->len
		, way_body->actseq, way_body->ncastat
		);
	CLRPC_SV_TRACE_12 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) .way.casuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, scall->id
		, way_body->casuid.time_low, way_body->casuid.time_mid, way_body->casuid.time_hi_and_version
		, way_body->casuid.clock_seq_hi_and_reserved, way_body->casuid.clock_seq_low
		, way_body->casuid.node[0], way_body->casuid.node[1], way_body->casuid.node[2]
		, way_body->casuid.node[3], way_body->casuid.node[4], way_body->casuid.node[5]
		);

	if (pkt_recv->len != CLRPC_WAY2_RSP_SIZE) {
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "pkt.len(%u) != WAY2.len(%u)"
			, pkt_recv->len
			, CLRPC_WAY2_RSP_SIZE
			);
		return CLRPC_PROVIDE_LOWER;
	}

	if (CLRPC_DREP_INT_DIFFERS (pkt_recv->drep[0])) {
		CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "need swap! pkt.drep(0x%x) != (0x%x)"
			, pkt_recv->drep[0]
			, CLRPC_DREP_0
			);
		CLRPC_SWAP_U32 (& way_body->ncastat);
		CLRPC_SWAP_U32 (& way_body->actseq);
	}

	clrpc_oneshot_stop (& scall->oneshot);
	clrpc_hash_remove (& clrpc_sc_hash, & scall->way.hashed_act.uuid);

		/* check if previous recv pkt-seq is equal to the way-answer
		// see dgsct.c(848) and  dgexec.c(693)
		*/
        /* AP01499241: DCE-Specification: A sequence number may be reused for a given activity 
        identifier, if the sequence number space is exhausted. If sequence numbers wrap around 
        and are reused, the implementation must assure that these are unambiguous. Less than 
        half the space of sequence numbers may be used for concurrently pending calls.*/
	if ((way_body->ncastat == 0) && CLRPC_CMP32(scall->seq, >=, way_body->actseq)) 
    {
		scall->way.state = CLRPC_WAY_ST_OK;

			/* switch to next state */
		if (scall->state == CLRPC_CST_WF_WAY) {
    	    clrpc_sc_do_wf_answer (scall);
		}
		else {
			CLRPC_ASSERT (scall->state == CLRPC_CST_RECV);
		}
		return CLRPC_PROVIDE_LOWER;
	}

		/* some error: send reject */
	CLRPC_SV_TRACE_04 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "cid(%u) ncastat(%u) scall.seq(%u) way.actseq(%u) bad way, reject"
		, scall->id
		, way_body->ncastat
		, scall->seq
		, way_body->actseq
		);
	scall->way.state = CLRPC_WAY_ST_ERROR;
	clrpc_sc_send_error (scall, CLRPC_PT_REJECT, CLRPC_NCA_WHO_ARE_YOU_FAILED);
	return CLRPC_PROVIDE_LOWER;
}


/*------------------------------------------------------------------------------
//	clrpc_sc_ev_timeout
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_ev_timeout (
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sc_ev_timeout")

	CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "cid(%u) TIMEOUT: state(%u)"
		, scall->id
		, scall->state
		);

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->server));
	CLRPC_ASSERT (CLRPC_PTR_ARE_EQUAL (scall->self, scall));

	switch (scall->state) {
	case CLRPC_CST_INIT:
		clrpc_sc_do_orphan (scall);
		break;

	case CLRPC_CST_RECV:
		clrpc_sc_do_orphan (scall);
		break;

	case CLRPC_CST_WF_WAY:
		scall->way.timeout_cnt += 1;
		if (scall->way.timeout_cnt > CLRPC_MAX_RTX_RETRY) {
			CLRPC_SV_TRACE_00 (scall->trace_idx, LSA_TRACE_LEVEL_WARN
				, "way request timeout, send reject"
				);
            /* AP00651106: Microsoft RPC repeats the call when receiving a REJECT with CLRPC_NCA_UNSPECIFIC_REJECT for seconds.
           To stop this storm of telegramms we use CLRPC_NCA_WHO_ARE_YOU_FAILED instead. */
			clrpc_sc_send_error (scall, CLRPC_PT_REJECT, CLRPC_NCA_WHO_ARE_YOU_FAILED);
		}
		else {
			clrpc_sc_send_way (scall);
			clrpc_oneshot_trigger (& scall->oneshot, CLRPC_RTX_TIMEOUT_MSEC);
		}
		break;

	case CLRPC_CST_WF_ANSWER:
		CLRPC_ASSERT (0); /* pgm error: user must not timeout-checked */
		break;

	case CLRPC_CST_SEND:
		/* means: waiting for a fack, but get none */
		clrpc_sc_do_orphan (scall);
		break;

	case CLRPC_CST_WF_ACK:
		clrpc_sc_do_orphan (scall);
		break;

	case CLRPC_CST_ORPHAN:
		clrpc_sc_do_orphan (scall); /* orphan keep orphan if upper_wf */
		break;

	default:
		CLRPC_ASSERT (0); /* "pgm error" */
		break;
	}
}


/*------------------------------------------------------------------------------
//	clrpc_sc_dispatch
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_dispatch (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_UPPER_RQB_PTR_TYPE  upper,
	CLRPC_LOWER_RQB_PTR_TYPE  lower,
	LSA_UINT32  oneshot,
	CLRPC_LOWER_RQB_PTR_TYPE  send
	)
{
	CLRPC_LOG_FCT("clrpc_sc_dispatch")
	CLRPC_UPPER_SV_CALL_PTR_TYPE  up_call;
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt;
	LSA_UINT16  rc;
	CLRPC_SERVER_PTR_TYPE  server;

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->server));
	CLRPC_ASSERT (CLRPC_PTR_ARE_EQUAL (scall->self, scall));

	CLRPC_ASSERT (1 == ((CLRPC_PTR_IS_NULL (lower)?0:1) + (oneshot?1:0) + (CLRPC_PTR_IS_NULL (upper)?0:1) + (CLRPC_PTR_IS_NULL (send)?0:1)));

	CLRPC_SV_TRACE_07 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cid(%u) DISPATCH: busy(%u) in_queue(%u)  upper(%08x) lower(%08x) oneshot(%u) send(%08x)"
		, scall->id, scall->busy, scall->send.in_queue
		, upper, lower, oneshot, send
		);

	server = scall->server;

		/* if busy, only queueing, work later */
	if (scall->busy) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cid(%u) => busy.queueing"
			, scall->id
			);
		if (CLRPC_PTR_IS_NOT_NULL (upper)) {
			clrpc_queue_enqueue (& scall->send.answer_queue,  upper);
		}
		else if (CLRPC_PTR_IS_NOT_NULL (lower)) {
			clrpc_queue_enqueue (& scall->recv.queue,  lower);
		}
		else if (oneshot) {
			scall->oneshot_queue = oneshot;
		}
		else if (CLRPC_PTR_IS_NOT_NULL (send)) {
			clrpc_queue_enqueue (& scall->send.send_queue,  send);
		}
		return;
	}
	if (scall->send.in_queue) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cid(%u) => sending.queueing"
			, scall->id
			);
		if (CLRPC_PTR_IS_NOT_NULL (upper)) {
			clrpc_queue_enqueue (& scall->send.answer_queue,  upper);
			return;
		}
		if (CLRPC_PTR_IS_NOT_NULL (lower)) {
			clrpc_queue_enqueue (& scall->recv.queue,  lower);
			return;
		}
	}

		/* now we are busy ! */
	scall->busy = 1;

		/* handle current and all queued events */
	do {

		scall->state_pre = scall->state; /* debug */

			/* if unregister ? */
		if (server->do_unregister) {
				/* free */
			if (CLRPC_PTR_IS_NOT_NULL (send)) {
				clrpc_sv_event_send_done (server, send);
			}
			if (CLRPC_PTR_IS_NOT_NULL (lower)) {
				clrpc_sv_ll_recv_provide (server, lower);
			}
			if (CLRPC_PTR_IS_NOT_NULL (upper)) {
				CLRPC_ASSERT (scall->upper_wf > 0);
				scall->upper_wf = 0;
				scall->upper_drop = 0;
				clrpc_sc_opnum_count_decr (scall, upper);
				clrpc_sv_provide_enqueue (server, & upper);

			}
			clrpc_sc_do_orphan (scall);
			CLRPC_ASSERT (scall->state == CLRPC_CST_FREE || scall->state == CLRPC_CST_ORPHAN);
		}

			/* handle send resources */
		else if (CLRPC_PTR_IS_NOT_NULL (send)) {
				/* send-prepare goes ok */
			if (clrpc_sc_send_prepare (scall, send) == CLRPC_OK) {
#if 0//testcase/AP00820286
				#pragma CLRPC_MESSAGE("error: testcase/AP00820286")
				{
					static int done = 0;
					if (!done && scall->send.frag_curr_fragnum == 2 && scall->up_call->opnum == CLRPC_PNIO_OPNUM_READ)
					{
						//skip send
						done = 1;
						scall->send.frag_flags = scall->send.frag_flags;
					}
					else
					{
						clrpc_sv_pkt_send (server, send);
					}
				}
#else
				clrpc_sv_pkt_send (server, send);
#endif
				clrpc_sc_send_done (scall);
			}
				/* or push back sending resource */
			else {
				scall->send.in_queue = 0; /* AP01167296 */
				clrpc_sv_event_send_done (server, send);
			}
		}

			/* handle timeout */
		else if (oneshot) {
			clrpc_sc_ev_timeout (scall);
		}

			/* handle lower rqb */
		else if (CLRPC_PTR_IS_NOT_NULL (lower)) {
			CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt);

			switch (pkt->ptype) {
			case CLRPC_PT_REQUEST:
				rc = clrpc_sc_ev_request (scall, lower);
				break;

			case CLRPC_PT_PING:
				rc = clrpc_sc_ev_ping (scall, lower);
				break;

			case CLRPC_PT_QUIT:
				rc = clrpc_sc_ev_quit (scall, lower);
				break;

			case CLRPC_PT_ACK:
				rc = clrpc_sc_ev_ack (scall, lower);
				break;

			case CLRPC_PT_FACK:
				rc = clrpc_sc_ev_fack (scall, lower);
				break;

			case CLRPC_PT_RESPONSE:
				rc = clrpc_sc_ev_response (scall, lower);
				break;

			default:
				CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "unknown ptype(%u)"
					, pkt->ptype
					);
				rc = CLRPC_PROVIDE_LOWER;
				break;
			}

				/* the clrpc_sc_ev_ function says, what we have do to */
			if (rc == CLRPC_PROVIDE_LOWER) {
				clrpc_sv_ll_recv_provide (server, lower);
			}
			else if (rc == CLRPC_QUEUE_AGAIN) {
				clrpc_queue_enqueue (& scall->recv.queue,  lower);
			}
		}

			/* handle upper rqb: must be OPC_SV_CALL_RESPONSE */
		else if (CLRPC_PTR_IS_NOT_NULL (upper)) {
			LSA_UINT16 orig_drop;

			CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE (upper) == CLRPC_OPC_SV_CALL_RESPONSE);

			up_call = upper->args.sv.call;

			CLRPC_ASSERT (scall->upper_wf > 0);
			scall->upper_wf = 0;

			orig_drop = scall->upper_drop;
			if (scall->upper_drop > 0) {
					/* semantics == CLRPC_SEMANTICS_MAYBE or CLRPC_SEMANTICS_BROADCAST_MAYBE
					//	will also set upper_drop */
				clrpc_sc_opnum_count_decr (scall, upper);
				clrpc_sv_provide_enqueue (server, & upper);
				scall->upper_drop = 0;
			}

			switch (scall->state) {
			case CLRPC_CST_INIT: /* maybe was running */
			case CLRPC_CST_RECV: /* new call still working */
			case CLRPC_CST_WF_WAY: /* new call still working */
				CLRPC_ASSERT (orig_drop > 0);
				break;

			case CLRPC_CST_WF_ANSWER:
				if (CLRPC_PTR_IS_NOT_NULL (scall->upper)) {
					CLRPC_ASSERT (orig_drop > 0);
						/* call manager with new call */
					clrpc_sc_do_wf_answer (scall);
				}
				else {
						/* prepare scall and change state to send */
					if (orig_drop == 0) {
						scall->upper = upper;
						scall->up_call = up_call;
						clrpc_sc_do_send (scall);
					}
						/* should have been a maybe-call */
					else {
						clrpc_sc_do_init (scall);
					}
				}
				break;

			case CLRPC_CST_ORPHAN: /* has recv a orphan-quit, must die now */
				if (CLRPC_PTR_IS_NOT_NULL (upper)) { /* see upper_drop above  */
					clrpc_sc_opnum_count_decr (scall, upper);
					clrpc_sv_provide_enqueue (server, & upper);
				}
				clrpc_sc_do_orphan (scall);
				break;

			case CLRPC_CST_SEND:
			case CLRPC_CST_WF_ACK:
			case CLRPC_CST_FREE:
			default:
				CLRPC_FATAL1 (scall->state);
				break;
			}
		}


			/* scall - die in the meantime ? see sc_do_free() */
		if (scall->state == CLRPC_CST_FREE) {
			clrpc_sv_scall_free (server, & scall);
			/* ASSERT (scall == NULL) */
#if CLRPC_DEBUG
			/* consistency check of hash */
			{LSA_UINT32  cnt = clrpc_hash_count_entries (& clrpc_sc_hash);cnt=cnt;}
#endif
			return;
		}

			/* something recv in the meantime ? */
		send = LSA_NULL;
		upper = LSA_NULL;
		lower = LSA_NULL;
		oneshot = 0;

#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("todo: check order of reaktivating queued elements.")
/* Ein Timeout kann einen Sendeauftrag überholen,
//	wenn der scall aus Resourcenmangel so lange in der send-queue hängt.
//	siehe auch: clrpc_sc_post_to_send_queue()
//	testcase: überlastete EB200
 */
#endif /* CLRPC_MESSAGE */

		if (scall->oneshot_queue) {
			oneshot = scall->oneshot_queue;
			scall->oneshot_queue = 0;	/* oneshot == 0 .. not valid id */
		}
		else if (clrpc_queue_not_empty (& scall->send.send_queue)) {
			CLRPC_ASSERT (scall->send.in_queue);
			send = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->send.send_queue);
#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("check: send.in_queue = 0 is done in clrpc_sc_send_done(), too late?")
#endif /* CLRPC_MESSAGE */
		}
		else if (! scall->send.in_queue
		&&  clrpc_queue_not_empty (& scall->send.answer_queue))
		{
			upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->send.answer_queue);
		}
		else if (! scall->send.in_queue
		&&  clrpc_queue_not_empty (& scall->recv.queue))
		{
			lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& scall->recv.queue);
		}
		else {
			scall->busy = 0;
		}
		if (scall->busy) {
			CLRPC_SV_TRACE_06 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "clrpc_sc_dispatch:cid(%u) sending(%u) loop again"
				  ": upper(%08x) lower(%08x) oneshot(%u) send(%08x)"
				, scall->id
				, scall->send.in_queue
				, upper
				, lower
				, oneshot
				, send
				);
		}
		else {
			CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "clrpc_sc_dispatch:cid(%u) sending(%u) done."
				, scall->id
				, scall->send.in_queue
				);
		}
	} while (scall->busy);
	return;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* CLRPC_CFG_MAX_SERVERS */
