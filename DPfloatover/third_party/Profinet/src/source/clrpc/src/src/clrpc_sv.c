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
/*  F i l e               &F: clrpc_sv.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the Server part                                               */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    10
#define LTRC_ACT_MODUL_ID  10

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


	/* compile only, when server-part is needed */
#if CLRPC_CFG_MAX_SERVERS

	/* forward */
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_lower_done (
	CLRPC_CHANNEL_PTR_TYPE  sys_channel,	/* must same as clrpc_system_channel() */
	LSA_VOID_PTR_TYPE  instance_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
	);

static CLRPC_SCALL_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_scall_alloc (
    LSA_UINT32 CLRPC_LOCAL_MEM_ATTR * error_code_ptr,
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_UUID_PTR_TYPE  actuid,
	LSA_UINT32  seq
	);

	/* global hash for connecting an activity-uuid to one scall */
CLRPC_HASH_TYPE  clrpc_sc_hash;

	/* global server data */
CLRPC_SERVER_TYPE  clrpc_servers[CLRPC_CFG_MAX_SERVERS];


#define CLRPC_TEST_TRACE_UUID	0

#if CLRPC_TEST_TRACE_UUID/*trace-test*/
#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("error: trace-test for UUIDs")
#endif /* CLRPC_MESSAGE */
struct clrpc_trace_uuid_tag {
	LSA_UINT32		data1;		LSA_UINT32		data2;
	LSA_UINT32		data3;		LSA_UINT32		data4;
};
typedef struct clrpc_trace_uuid_tag CLRPC_COMMON_MEM_ATTR *CLRPC_TRACE_UUID_PTR_TYPE;
#endif


/*------------------------------------------------------------------------------
//	clrpc_epm_find_server
//	search the interface-object-uuid-pair with major.minor in the server database
//----------------------------------------------------------------------------*/
CLRPC_SERVER_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_find_server (
	LSA_UINT16  find_method,
	CLRPC_UUID_PTR_TYPE  if_id,
	LSA_UINT16  if_vers_major,
	LSA_UINT16  if_vers_minor,
	CLRPC_UUID_PTR_TYPE  obj_id
	)
{
	CLRPC_LOG_FCT("clrpc_epm_find_server")
	CLRPC_SERVER_PTR_TYPE  server;
	LSA_UINT16  i;
	/*
	**	1)	if_uuid==, version==, obj_uuid==
	**	2)  if_uuid==, version=~, obj_uuid==
	**	3)  if_uuid==, version==, nil_uuid==
	**	4)  if_uuid==, version=~, nil_uuid==
	*/
	for (i = 0; i < CLRPC_CFG_MAX_SERVERS; ++i)
	{
		server = & clrpc_servers[i];
		if (server->state != CLRPC_SST_READY)
		{
			continue;
		}
		if (! clrpc_uuid_equal (& server->if_uuid, if_id))
		{
			continue;
		}
		switch (find_method)
		{
		case 1:
			if (server->if_major == if_vers_major
			&&  server->if_minor == if_vers_minor
			&&	clrpc_uuid_equal (& server->obj.object_uuid, obj_id))
			{
				return server;
			}
			break;
		
		case 2:
			if (server->if_major == if_vers_major
			&&  server->if_minor  > if_vers_minor
			&&	clrpc_uuid_equal (& server->obj.object_uuid, obj_id))
			{
				return server;
			}
			break;
		
		case 3:
			if (server->if_major == if_vers_major
			&&  server->if_minor == if_vers_minor
			&&	clrpc_uuid_equal (& server->obj.object_uuid, & clrpc_uuid_nil_uuid))
			{
				return server;
			}
			break;
		
		case 4:
		default:
			if (server->if_major == if_vers_major
			&&  server->if_minor  > if_vers_minor
			&&	clrpc_uuid_equal (& server->obj.object_uuid, & clrpc_uuid_nil_uuid))
			{
				return server;
			}
			break;
		}
	}
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	clrpc_epm_lookup_server
//	search the interface-object-uuid-pair with major.* in the server database
//----------------------------------------------------------------------------*/
CLRPC_SERVER_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_lookup_server (
	CLRPC_UUID_PTR_TYPE  if_id,
	LSA_UINT16  if_vers_major,
	LSA_UINT16  if_vers_minor,
	CLRPC_UUID_PTR_TYPE  obj_id
	)
{
	CLRPC_LOG_FCT("clrpc_epm_lookup_server")
	CLRPC_SERVER_PTR_TYPE  server;
	LSA_UINT16  find_method;

	for (find_method = 1; find_method <= 4; ++find_method)
	{
		server = clrpc_epm_find_server (find_method, if_id, if_vers_major, if_vers_minor, obj_id);
		if (CLRPC_PTR_IS_NOT_NULL (server))
		{
			CLRPC_SV_TRACE_02 (0, LSA_TRACE_LEVEL_CHAT
				, "epm_lookup_server found with find_method(%u) sid(%u)", find_method, server->id
				);
			return server;
		}
	}

		/* interface/object not found => big LSA traces */
	CLRPC_SV_TRACE_11 (0, LSA_TRACE_LEVEL_WARN
		, "epm_lookup_server did not find server with interface_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, if_id->time_low, if_id->time_mid, if_id->time_hi_and_version
		, if_id->clock_seq_hi_and_reserved, if_id->clock_seq_low
		, if_id->node[0], if_id->node[1], if_id->node[2]
		, if_id->node[3], if_id->node[4], if_id->node[5]
		);
	CLRPC_SV_TRACE_11 (0, LSA_TRACE_LEVEL_WARN
		, " and object_id(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, obj_id->time_low, obj_id->time_mid, obj_id->time_hi_and_version
		, obj_id->clock_seq_hi_and_reserved, obj_id->clock_seq_low
		, obj_id->node[0], obj_id->node[1], obj_id->node[2]
		, obj_id->node[3], obj_id->node[4], obj_id->node[5]
		);

	{
	LSA_UINT16  i;
	for (i = 0; i < CLRPC_CFG_MAX_SERVERS; ++i)
	{
		server = & clrpc_servers[i];
		if (server->state != CLRPC_SST_READY) {
			continue;
		}
		CLRPC_SV_TRACE_11 (0, LSA_TRACE_LEVEL_WARN
			, "existing if_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, server->if_uuid.time_low, server->if_uuid.time_mid, server->if_uuid.time_hi_and_version
			, server->if_uuid.clock_seq_hi_and_reserved, server->if_uuid.clock_seq_low
			, server->if_uuid.node[0], server->if_uuid.node[1], server->if_uuid.node[2]
			, server->if_uuid.node[3], server->if_uuid.node[4], server->if_uuid.node[5]
			);
		CLRPC_SV_TRACE_11 (0, LSA_TRACE_LEVEL_WARN
			, "  with object_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, server->obj.object_uuid.time_low, server->obj.object_uuid.time_mid, server->obj.object_uuid.time_hi_and_version
			, server->obj.object_uuid.clock_seq_hi_and_reserved, server->obj.object_uuid.clock_seq_low
			, server->obj.object_uuid.node[0], server->obj.object_uuid.node[1], server->obj.object_uuid.node[2]
			, server->obj.object_uuid.node[3], server->obj.object_uuid.node[4], server->obj.object_uuid.node[5]
			);
#if CLRPC_TEST_TRACE_UUID/*trace-test*/
		{
			CLRPC_TRACE_UUID_PTR_TYPE  truid = (CLRPC_TRACE_UUID_PTR_TYPE)(&server->if_uuid);
			CLRPC_TRACE_UUID_PTR_TYPE  troid = (CLRPC_TRACE_UUID_PTR_TYPE)(&server->obj.object_uuid);

			CLRPC_SV_TRACE_09 (0, LSA_TRACE_LEVEL_WARN
				, "existing interface_id(%08x-%08x-%08x-%08x) if_vers(%x) with object_id(%08x-%08x-%08x-%08x)"
				, truid->data1, truid->data2, CLRPC_NTOHL(truid->data3), CLRPC_NTOHL(truid->data4)
				, CLRPC_MAKE_IF_VERS(server->if_major, server->if_minor)
				, troid->data1, troid->data2, troid->data3, troid->data4
				);
		}		
#endif
	}
	}
		/* no server found */
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_sock_open (
	CLRPC_SERVER_PTR_TYPE  server
	)
{
	CLRPC_LOG_FCT("clrpc_sv_sock_open")
	CLRPC_LOWER_RQB_PTR_TYPE  lower;
	CLRPC_LL_UDP_OPEN_PTR_TYPE  sock_open;

		/* open a new sock */
	lower = clrpc_sock_alloc_rqb ();
	if (CLRPC_PTR_IS_NULL (lower))
	{
		return CLRPC_ERR_ALLOC_LOWER;
	}

	sock_open = & lower->args.udp_open;

	sock_open->loc_addr.sin_family           = SOCK_AF_INET;
	sock_open->loc_addr.sin_addr.S_un.S_addr = 0;
	sock_open->loc_addr.sin_port             = server->port;

	clrpc_request_lower (lower, SOCK_OPC_UDP_OPEN, server);
	return CLRPC_OK;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_sock_close (
	CLRPC_SERVER_PTR_TYPE  server
	)
{
	CLRPC_LOG_FCT("clrpc_sv_sock_close")
	CLRPC_LOWER_RQB_PTR_TYPE  lower;
	CLRPC_LL_UDP_CLOSE_PTR_TYPE  sock_close;

	CLRPC_ASSERT (server->do_unregister);

		/* close the sock */
	lower = clrpc_sock_alloc_rqb ();
	if (CLRPC_PTR_IS_NULL (lower))
	{
		return CLRPC_ERR_ALLOC_LOWER;
	}

	sock_close = & lower->args.close;
	sock_close->sock_fd = server->sock_fd;

	clrpc_request_lower (lower, SOCK_OPC_CLOSE, server);
	return CLRPC_OK;
}


/*------------------------------------------------------------------------------------
//	clrpc_pkt_init
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_init (
	CLRPC_HEADER_PTR_TYPE  hdr,
	LSA_UINT16  ptype,
	CLRPC_UUID_PTR_TYPE  if_id,
	LSA_UINT32  if_vers,
	LSA_UINT16  opnum,
	CLRPC_UUID_PTR_TYPE  obj_uuid,
	CLRPC_UUID_PTR_TYPE  actuid,
	LSA_UINT32  seq
	)
{
	hdr->rpc_vers = CLRPC_HEADER_VERSION;
	hdr->ptype = (LSA_UINT8)ptype;
	hdr->flags = 0;
	hdr->flags2 = 0;
	hdr->drep[0] = CLRPC_DREP_0;
	hdr->drep[1] = CLRPC_DREP_1;
	hdr->drep[2] = CLRPC_DREP_2;
	CLRPC_MEMCPY (& hdr->actuid, actuid, sizeof hdr->actuid);
	hdr->server_boot = 0;
	CLRPC_MEMCPY (& hdr->if_id, if_id, sizeof hdr->if_id);
	hdr->if_vers = if_vers;
	hdr->opnum = opnum;
	CLRPC_MEMCPY (& hdr->object, obj_uuid, sizeof hdr->object);
	hdr->fragnum = 0;
	hdr->ihint = CLRPC_NO_HINT;
	hdr->ahint = CLRPC_NO_HINT;
	hdr->serial_hi = 0;
	hdr->serial_lo = 0;
	hdr->seq = seq;
	hdr->auth_proto = 0;
	hdr->len = 0;
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_pkt_is_sane
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_is_sane (
	CLRPC_HEADER_PTR_TYPE  pkt,
	LSA_UINT32  recv_len
	)
{
	CLRPC_LOG_FCT("clrpc_pkt_is_sane")

	if (recv_len < sizeof(CLRPC_HEADER_TYPE)) {
		CLRPC_SV_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
			, "recv_len(%u) < sizeof(pkt)(%u)"
			, recv_len
			, sizeof(CLRPC_HEADER_TYPE)
			);
		return 0;
	}
	if ((pkt->rpc_vers & 0x0f) != CLRPC_HEADER_VERSION) {
		CLRPC_SV_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
			, "rpc_vers(%u) != (%u)"
			, pkt->rpc_vers & 0x0f
			, CLRPC_HEADER_VERSION
			);
		return 0;
	}
	if (pkt->ptype > CLRPC_PT_MAX) {
		CLRPC_SV_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "wrong ptype(%u)"
			, pkt->ptype
			);
		return 0;
	}
	if (recv_len != sizeof(CLRPC_HEADER_TYPE) + pkt->len) {
		CLRPC_SV_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
			, "wrong pkt.len(%u) + hdr(%u) != recv_len(%u)"
			, pkt->len
			, sizeof(CLRPC_HEADER_TYPE)
			, recv_len
			);
		return 0;
	}
    
    /* flags for semantics are only valid in REQUEST */
    if (pkt->ptype == CLRPC_PT_REQUEST) 
    {
        LSA_UINT8 pkt_semantic;
        
        pkt_semantic = clrpc_pkt_get_semantics(pkt);
        if (( pkt_semantic == CLRPC_SEMANTICS_MAYBE) || 
            ( pkt_semantic == CLRPC_SEMANTICS_BROADCAST) || 
            ( pkt_semantic == CLRPC_SEMANTICS_BROADCAST_MAYBE) 
           )
        {
            /* AP00969270: semantics MAYBE or BROADCAST are not allowed */
		    CLRPC_SV_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			    , "wrong semantics in pkt.flags(0x%x)"
			    , pkt->flags
			    );
		    return 0;
        }
    }

    return 1;
}


/*------------------------------------------------------------------------------
//	clrpc_pkt_get_semantics
//----------------------------------------------------------------------------*/
LSA_UINT8  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_get_semantics (
	CLRPC_HEADER_PTR_TYPE  pkt
	)
{
	/* order of if is important, because a maybe (and broadcast) has also
		set the idempotent flag
		Servers do not send responses for maybe or broadcast/maybe requests.
	 */

	if (pkt->flags & CLRPC_PF_MAYBE) {
		if (pkt->flags & CLRPC_PF_BROADCAST) {
			return CLRPC_SEMANTICS_BROADCAST_MAYBE;
		}
		return CLRPC_SEMANTICS_MAYBE;
	}

	if (pkt->flags & CLRPC_PF_BROADCAST) {
		return CLRPC_SEMANTICS_BROADCAST;
	}

	if (pkt->flags & CLRPC_PF_IDEMPOTENT) {
		return CLRPC_SEMANTICS_IDEMPOTENT;
	}

	return CLRPC_SEMANTICS_AT_MOST_ONCE;
}


/*------------------------------------------------------------------------------
//	clrpc_pkt_swap_hdr_inplace
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_swap_hdr_inplace (
	CLRPC_HEADER_PTR_TYPE  pkt
	)
{
	CLRPC_SWAP_UUID (& pkt->object);
	CLRPC_SWAP_UUID (& pkt->if_id);
	CLRPC_SWAP_UUID (& pkt->actuid);
	CLRPC_SWAP_U32 (& pkt->server_boot);
	CLRPC_SWAP_U32 (& pkt->if_vers);
	CLRPC_SWAP_U32 (& pkt->seq);
	CLRPC_SWAP_U16 (& pkt->opnum);
	CLRPC_SWAP_U16 (& pkt->ihint);
	CLRPC_SWAP_U16 (& pkt->ahint);
	CLRPC_SWAP_U16 (& pkt->len);
	CLRPC_SWAP_U16 (& pkt->fragnum);
}


/*------------------------------------------------------------------------------
//	clrpc_sv_pkt_send_or_queue
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pkt_send_or_queue (
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_SCALL_PTR_TYPE  scall
	)
{
	CLRPC_LOG_FCT("clrpc_sv_pkt_send_or_queue")
    CLRPC_LOWER_RQB_PTR_TYPE  send;
	CLRPC_ASSERT (server);

	CLRPC_SV_TRACE_03 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) scall(%08x) ptype(%u)"
		, server->id
		, scall
		, scall ? scall->send.ptype : -1
		);

		/* if a scall is given */
	if (CLRPC_PTR_IS_NOT_NULL (scall)) {
			/* queue the scall, make this scall the last to send */
		clrpc_queue_enqueue (& server->send_scall_queue, scall);
		scall = LSA_NULL;
	}

		/* when there are send resources */
	send = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& server->send_rqb_queue);
	if (CLRPC_PTR_IS_NULL (send)) {
		CLRPC_SV_TRACE_00 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "no send-resource, waiting"
			);
		return;
	}

		/* and a scall is waiting */
	scall = (CLRPC_SCALL_PTR_TYPE)clrpc_queue_dequeue (& server->send_scall_queue);

	if (CLRPC_PTR_IS_NOT_NULL (scall)) {
			/* dispatch to scall */
		CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dequeue cid(%u)"
			, scall->id
			);
		clrpc_sc_dispatch (scall, LSA_NULL, LSA_NULL, 0, send);
	}
	else {
			/* or push back sending resource */
		clrpc_queue_enqueue (& server->send_rqb_queue, send);
	}
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_pkt_send
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pkt_send (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  send
	)
{
	CLRPC_LOG_FCT("clrpc_sv_pkt_send")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_send;
	CLRPC_HEADER_PTR_TYPE  pkt;

	CLRPC_PKT_GET_FROM_SEND (send, udp_send, pkt);

	udp_send->data_length = sizeof(*pkt) + pkt->len;
	CLRPC_ASSERT (udp_send->data_length <= CLRPC_MUST_RECV_FRAG_SIZE);

	CLRPC_SV_TRACE_06 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> server_id(%u) --- SEND.req --- lower(%08x) ip_addr(%08x:%u) len(%u) ptype(%u)"
		, server->id
		, send
		, CLRPC_NTOHL(udp_send->rem_addr.sin_addr.S_un.S_addr)
		, CLRPC_NTOHS(udp_send->rem_addr.sin_port)
		, udp_send->data_length
		, pkt->ptype
		);

		/* no server_boot: else this paket will be dropped without look inside */

	if (server->is_endpoint_mapper == 0) {
		pkt->server_boot = server->server_boot;
	}

		/* trace outgoing pakets */
#if CLRPC_DEBUG
	{
		CLRPC_SV_PKT_TRACE_08 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "=== SEND --- ip(%08x) port(%u)  server_boot(%u)  serial(%u)  fragnum(%u)  drep0_1_2(%#x %#x %#x)"
			, CLRPC_NTOHL(udp_send->rem_addr.sin_addr.S_un.S_addr)
			, CLRPC_NTOHS(udp_send->rem_addr.sin_port)
			, pkt->server_boot, ((pkt->serial_hi << 8) | pkt->serial_lo), pkt->fragnum
			, pkt->drep[0], pkt->drep[1], pkt->drep[2]
			);
		CLRPC_SV_PKT_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND ptype(%u)  opnum(%u)  len(%u)  flags(%#x:%c%c%c%c%c%c)  flags2(%#x)"
			, pkt->ptype, pkt->opnum, pkt->len
			, pkt->flags
				, pkt->flags & CLRPC_PF_FRAG ? 'F' : '-'
				, pkt->flags & CLRPC_PF_LAST_FRAG ? 'L' : '-'
				, pkt->flags & CLRPC_PF_NO_FACK ? 'N' : '-'
				, pkt->flags & CLRPC_PF_MAYBE ? 'm' : '-'
				, pkt->flags & CLRPC_PF_IDEMPOTENT ? 'i' : '-'
				, pkt->flags & CLRPC_PF_BROADCAST ? 'b' : '-'
			, pkt->flags2
			);
		CLRPC_SV_PKT_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)  actseq(%u)"
			, pkt->actuid.time_low, pkt->actuid.time_mid, pkt->actuid.time_hi_and_version
			, pkt->actuid.clock_seq_hi_and_reserved, pkt->actuid.clock_seq_low
			, pkt->actuid.node[0], pkt->actuid.node[1], pkt->actuid.node[2]
			, pkt->actuid.node[3], pkt->actuid.node[4], pkt->actuid.node[5]
			, pkt->seq
			);
		CLRPC_SV_PKT_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND ifuuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) vers=0x%08x"
			, pkt->if_id.time_low, pkt->if_id.time_mid, pkt->if_id.time_hi_and_version
			, pkt->if_id.clock_seq_hi_and_reserved, pkt->if_id.clock_seq_low
			, pkt->if_id.node[0], pkt->if_id.node[1], pkt->if_id.node[2]
			, pkt->if_id.node[3], pkt->if_id.node[4], pkt->if_id.node[5]
			, pkt->if_vers
			);
		CLRPC_SV_PKT_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, ".SEND objuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, pkt->object.time_low, pkt->object.time_mid, pkt->object.time_hi_and_version
			, pkt->object.clock_seq_hi_and_reserved, pkt->object.clock_seq_low
			, pkt->object.node[0], pkt->object.node[1], pkt->object.node[2]
			, pkt->object.node[3], pkt->object.node[4], pkt->object.node[5]
			);
	}
#endif

	udp_send->sock_fd = server->sock_fd;
	CLRPC_ASSERT (udp_send->rem_addr.sin_addr.S_un.S_addr != 0);
	CLRPC_ASSERT (udp_send->rem_addr.sin_port != 0);

	clrpc_request_lower (send, SOCK_OPC_UDP_SEND, server);
}


/*------------------------------------------------------------------------------
//	clrpc_pkt_set_fack_body
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_set_fack_body (
	CLRPC_HEADER_PTR_TYPE  pkt_send,
	LSA_UINT16  window_size,
	LSA_UINT16  serial_num
	)
{
	CLRPC_LOG_FCT("clrpc_pkt_set_fack_body")
	CLRPC_FACK_BODY_PTR_TYPE  fack;

	CLRPC_SV_TRACE_02 (0, LSA_TRACE_LEVEL_CHAT
		, "window_size(%u) serial_num(%u)"
		, window_size
		, serial_num
		);

	pkt_send->len = CLRPC_FACK_BODY_SIZE;
	CLRPC_PKT_SET_SERIAL (pkt_send, serial_num);

	fack = CLRPC_PKT_GET_BODY (pkt_send, CLRPC_FACK_BODY_PTR_TYPE);

	fack->vers = CLRPC_FACK_BODY_VERS;
	fack->pad1 = 0xCC;
	fack->window_size = window_size;
	fack->max_tsdu = CLRPC_MUST_RECV_FRAG_SIZE;
	fack->max_frag_size = CLRPC_MUST_RECV_FRAG_SIZE;
	fack->serial_num = serial_num;
	fack->selack_len = 0;

	return;
}


/*------------------------------------------------------------------------------
//	clrpc_pkt_set_error_body
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_set_error_body (
	CLRPC_HEADER_PTR_TYPE  pkt_send,
	LSA_UINT32  error_code
	)
{
	CLRPC_LOG_FCT("clrpc_pkt_set_error_body")
	CLRPC_ERROR_BODY_PTR_TYPE  error_body;

	pkt_send->len = CLRPC_ERROR_BODY_SIZE;

	error_body = CLRPC_PKT_GET_BODY (pkt_send, CLRPC_ERROR_BODY_PTR_TYPE);
	CLRPC_ASSERT ((char*)error_body == (char*)pkt_send + 80);

	error_body->nca_status = error_code;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_send_reject
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_send_reject (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  recv,
	LSA_UINT32  error_code
	)
{
	CLRPC_LOG_FCT("clrpc_sv_send_reject")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
    CLRPC_LOWER_RQB_PTR_TYPE  send;
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_send;
	CLRPC_HEADER_PTR_TYPE  pkt_send;
	CLRPC_HEADER_PTR_TYPE  pkt_recv;

	CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "sid(%u) sv_send_reject error_code(%u)"
		, server->id
		, error_code
		);

	CLRPC_PKT_GET_FROM_RECV (recv, udp_recv, pkt_recv);

		/* no reject on maybe or broadcast */
	if (pkt_recv->flags & (CLRPC_PF_MAYBE | CLRPC_PF_BROADCAST)) {
		CLRPC_SV_TRACE_00 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "no reject on maybe or broadcast"
			);
		return;
	}

	send = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& server->send_rqb_queue);
	if (CLRPC_PTR_IS_NULL (send)) {
		CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "clrpc-server has no resource to send a REJECT as anwser to opnum(%u) seq(%u)"
			, pkt_recv->opnum, pkt_recv->seq
			);
		CLRPC_SV_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "recv.actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, pkt_recv->actuid.time_low
			, pkt_recv->actuid.time_mid
			, pkt_recv->actuid.time_hi_and_version
			, pkt_recv->actuid.clock_seq_hi_and_reserved
			, pkt_recv->actuid.clock_seq_low
			, pkt_recv->actuid.node[0]
			, pkt_recv->actuid.node[1]
			, pkt_recv->actuid.node[2]
			, pkt_recv->actuid.node[3]
			, pkt_recv->actuid.node[4]
			, pkt_recv->actuid.node[5]
			);
		return;
	}

	CLRPC_PKT_GET_FROM_SEND (send, udp_send, pkt_send);

	clrpc_pkt_init (pkt_send, CLRPC_PT_REJECT,
			& pkt_recv->if_id, pkt_recv->if_vers, pkt_recv->opnum, & pkt_recv->object,
			& pkt_recv->actuid, pkt_recv->seq
		);
	clrpc_pkt_set_error_body (pkt_send, error_code);

	CLRPC_UDP_SEND_INIT (udp_send);
	CLRPC_UDP_IP(udp_send) = CLRPC_UDP_IP(udp_recv);
	CLRPC_UDP_PORT(udp_send) = CLRPC_UDP_PORT(udp_recv);
	clrpc_sv_pkt_send (server, send);
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_send_nocall
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_send_nocall (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  recv,
	LSA_UINT16  last_known_fragnum
	)
{
	CLRPC_LOG_FCT("clrpc_sv_send_nocall")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt_recv;

    CLRPC_LOWER_RQB_PTR_TYPE  send;
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_send;
	CLRPC_HEADER_PTR_TYPE  pkt_send;

	CLRPC_PKT_GET_FROM_RECV (recv, udp_recv, pkt_recv);

	CLRPC_SV_TRACE_05 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) to.ip(%#x) to.port(%u) seq(%u) last_known_fragnum(%u)"
		, server->id
		, CLRPC_NTOHL (CLRPC_UDP_IP(udp_recv)), CLRPC_NTOHS (CLRPC_UDP_PORT(udp_recv))
		, pkt_recv->seq, last_known_fragnum
		);
	CLRPC_SV_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, pkt_recv->actuid.time_low, pkt_recv->actuid.time_mid, pkt_recv->actuid.time_hi_and_version
		, pkt_recv->actuid.clock_seq_hi_and_reserved, pkt_recv->actuid.clock_seq_low
		, pkt_recv->actuid.node[0], pkt_recv->actuid.node[1], pkt_recv->actuid.node[2]
		, pkt_recv->actuid.node[3], pkt_recv->actuid.node[4], pkt_recv->actuid.node[5]
		);

	send = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& server->send_rqb_queue);
	if (CLRPC_PTR_IS_NULL (send)) {
		CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "clrpc-server has no resource to send a NOCALL as anwser to opnum(%u) seq(%u)"
			, pkt_recv->opnum, pkt_recv->seq
			);
		CLRPC_SV_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "recv.actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, pkt_recv->actuid.time_low
			, pkt_recv->actuid.time_mid
			, pkt_recv->actuid.time_hi_and_version
			, pkt_recv->actuid.clock_seq_hi_and_reserved
			, pkt_recv->actuid.clock_seq_low
			, pkt_recv->actuid.node[0]
			, pkt_recv->actuid.node[1]
			, pkt_recv->actuid.node[2]
			, pkt_recv->actuid.node[3]
			, pkt_recv->actuid.node[4]
			, pkt_recv->actuid.node[5]
			);
		return;
	}

	CLRPC_PKT_GET_FROM_SEND (send, udp_send, pkt_send);

	CLRPC_UDP_SEND_INIT (udp_send);
	CLRPC_UDP_IP(udp_send) = CLRPC_UDP_IP(udp_recv);
	CLRPC_UDP_PORT(udp_send) = CLRPC_UDP_PORT(udp_recv);

	clrpc_pkt_init (pkt_send, CLRPC_PT_NOCALL,
			& pkt_recv->if_id, pkt_recv->if_vers,
			pkt_recv->opnum, & pkt_recv->object,
			& pkt_recv->actuid, pkt_recv->seq
		);

	pkt_send->fragnum = last_known_fragnum;

	clrpc_sv_pkt_send (server, send);
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_send_nocall_fack_body
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_send_nocall_fack_body (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  recv,
	LSA_UINT16  last_known_fragnum,
	LSA_UINT16  window_size,
	LSA_UINT16  serial_num
	)
{
	CLRPC_LOG_FCT("clrpc_sv_send_nocall_fack_body")
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	CLRPC_HEADER_PTR_TYPE  pkt_recv;

    CLRPC_LOWER_RQB_PTR_TYPE  send;
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_send;
	CLRPC_HEADER_PTR_TYPE  pkt_send;

	CLRPC_PKT_GET_FROM_RECV (recv, udp_recv, pkt_recv);

	CLRPC_SV_TRACE_07 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) to.ip(%#x) to.port(%u) seq(%u) window_size(%u) serial(%u) last_known_fragnum(%u)"
		, server->id
		, CLRPC_NTOHL (CLRPC_UDP_IP(udp_recv)), CLRPC_NTOHS (CLRPC_UDP_PORT(udp_recv))
		, pkt_recv->seq, window_size, serial_num, last_known_fragnum
		);
	CLRPC_SV_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, server->id
		, pkt_recv->actuid.time_low, pkt_recv->actuid.time_mid, pkt_recv->actuid.time_hi_and_version
		, pkt_recv->actuid.clock_seq_hi_and_reserved, pkt_recv->actuid.clock_seq_low
		, pkt_recv->actuid.node[0], pkt_recv->actuid.node[1], pkt_recv->actuid.node[2]
		, pkt_recv->actuid.node[3], pkt_recv->actuid.node[4], pkt_recv->actuid.node[5]
		);

	send = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& server->send_rqb_queue);
	if (CLRPC_PTR_IS_NULL (send)) {
		CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "clrpc-server has no resource to send a NOCALL+FACK_BODY as anwser to opnum(%u) seq(%u)"
			, pkt_recv->opnum, pkt_recv->seq
			);
		CLRPC_SV_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "recv.actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, pkt_recv->actuid.time_low
			, pkt_recv->actuid.time_mid
			, pkt_recv->actuid.time_hi_and_version
			, pkt_recv->actuid.clock_seq_hi_and_reserved
			, pkt_recv->actuid.clock_seq_low
			, pkt_recv->actuid.node[0]
			, pkt_recv->actuid.node[1]
			, pkt_recv->actuid.node[2]
			, pkt_recv->actuid.node[3]
			, pkt_recv->actuid.node[4]
			, pkt_recv->actuid.node[5]
			);
		return;
	}

	CLRPC_PKT_GET_FROM_SEND (send, udp_send, pkt_send);

	CLRPC_UDP_SEND_INIT (udp_send);
	CLRPC_UDP_IP(udp_send) = CLRPC_UDP_IP(udp_recv);
	CLRPC_UDP_PORT(udp_send) = CLRPC_UDP_PORT(udp_recv);

	clrpc_pkt_init (pkt_send, CLRPC_PT_NOCALL,
			& pkt_recv->if_id, pkt_recv->if_vers,
			pkt_recv->opnum, & pkt_recv->object,
			& pkt_recv->actuid, pkt_recv->seq
		);

	pkt_send->fragnum = last_known_fragnum;

	clrpc_pkt_set_fack_body (pkt_send, window_size, serial_num);

	clrpc_sv_pkt_send (server, send);
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_provide_enqueue
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_provide_enqueue (
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_UPPER_RQB_PTR_TYPE  *upper
	)
{
	CLRPC_LOG_FCT("clrpc_sv_provide_enqueue")
	CLRPC_UPPER_SV_CALL_PTR_TYPE  up_call;
	LSA_UINT16  rc;

	CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) upper(0x%08x)"
		, server->id, *upper
		);

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL ((*upper)->args.sv.call));
	up_call = (*upper)->args.sv.call;

		/* free mem of call.indication */
	if (CLRPC_PTR_IS_NOT_NULL (up_call->ndr_data))
	{
		CLRPC_FREE_UPPER_MEM (& rc, up_call->ndr_data, server->channel->sysptr);
		CLRPC_ASSERT(rc == LSA_RET_OK);
		up_call->ndr_data = LSA_NULL;
		up_call->alloc_len = 0;
	}
	CLRPC_ASSERT (CLRPC_PTR_IS_NULL (up_call->ndr_data));
	CLRPC_ASSERT (up_call->alloc_len == 0);

	CLRPC_RQB_SET_OPCODE ((*upper), CLRPC_OPC_SV_CALL_IND_PROVIDE);
	clrpc_queue_enqueue (& server->provided_queue, (*upper));

	*upper = LSA_NULL;
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_provide_dequeue
//----------------------------------------------------------------------------*/
CLRPC_UPPER_RQB_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_provide_dequeue (
	CLRPC_SERVER_PTR_TYPE  server
	)
{
	CLRPC_LOG_FCT("clrpc_sv_provide_dequeue")
	CLRPC_UPPER_RQB_PTR_TYPE  upper;

	upper = (CLRPC_UPPER_RQB_PTR_TYPE)clrpc_queue_dequeue (& server->provided_queue);

	CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) upper(0x%08x)"
		, server->id
		, upper
		);
	return upper;
}


/*------------------------------------------------------------------------------------
//	clrpc_sv_event_send_done
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_event_send_done (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sv_event_send_done")

		/* queue to free send-resources */
	clrpc_queue_enqueue (& server->send_rqb_queue, lower);

		/* look if there is one scall who is waiting for a send-resource */
	clrpc_sv_pkt_send_or_queue (server, LSA_NULL);
}


/*----------------------------------------------------------------------------*/
CLRPC_IF_OBJECT_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_if_obj_lookup (
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_UUID_PTR_TYPE  object_uuid
	)
{
	CLRPC_IF_OBJECT_PTR_TYPE  if_obj = LSA_NULL;

	if (clrpc_uuid_equal (& server->obj.object_uuid, object_uuid))
	{
		if_obj = & server->obj;
	}
	else if (clrpc_uuid_equal (& server->obj.object_uuid, & clrpc_uuid_nil_uuid)) /* AP01068070 */
	{
		if_obj = & server->obj;
	}
	else
	{
		if_obj = LSA_NULL;
	}

	return if_obj;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_event_recv
//----------------------------------------------------------------------------*/
static LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_event_recv (
    CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
    )
{
    CLRPC_LOG_FCT("clrpc_sv_event_recv")
    CLRPC_HEADER_PTR_TYPE  pkt;
    CLRPC_SCALL_PTR_TYPE  scall;
    CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
    CLRPC_IF_OBJECT_PTR_TYPE  if_obj;


    /* convert lower to rpc */
    CLRPC_PKT_GET_FROM_RECV (lower, udp_recv, pkt);

    CLRPC_SV_TRACE_05 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
        , "<<< server_id(%u) --- RECV.ind --- lower(%08x)  ip_addr(%08x:%u)  len(%u)"
        , server->id
        , lower
        , CLRPC_NTOHL(udp_recv->rem_addr.sin_addr.S_un.S_addr)
        , CLRPC_NTOHS(udp_recv->rem_addr.sin_port)
        , udp_recv->data_length
        );


    /* check ip-address and port */
    if(
        udp_recv->rem_addr.sin_addr.S_un.S_addr == CLRPC_HTONL(0)
        || udp_recv->rem_addr.sin_addr.S_un.S_addr == CLRPC_HTONL(0xFFFFFFFF)
        || udp_recv->rem_addr.sin_port == CLRPC_HTONS(0)
    ) {
        CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
            , "invalid ip(%#x) or port(%u)"
            , CLRPC_NTOHL(udp_recv->rem_addr.sin_addr.S_un.S_addr)
            , CLRPC_NTOHS(udp_recv->rem_addr.sin_port)
            );
        /*
         * NOTE: 0.0.0.0 is a "valid" src-address (e.g. for the DHCP protocol)
         *       255.255.255.255 is a configuration error at the sender
         *       port 0 cannot be opened by a user (= dynamic port)
         *
         * since we cannot reply to such an address, we have to drop the packet.
         */
        return CLRPC_PROVIDE_LOWER;
    }

        /* swap header if data representation differ */
    if (CLRPC_DREP_INT_DIFFERS (pkt->drep[0])) {
        CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
            , "need swap! pkt.drep(0x%x) != (0x%x)"
            , pkt->drep[0]
            , CLRPC_DREP_0
            );
        clrpc_pkt_swap_hdr_inplace (pkt);
    }

        /* sanity */
    if (! clrpc_pkt_is_sane (pkt, udp_recv->data_length)) {
        return CLRPC_PROVIDE_LOWER;
    }

        /* trace incoming pakets */
#if CLRPC_DEBUG
    {
        CLRPC_SV_PKT_TRACE_08 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
            , "=== RECV --- ip(%08x) port(%u)  server_boot(%u)  serial(%u)  fragnum(%u)  drep0_1_2(%#x %#x %#x)"
            , CLRPC_NTOHL(udp_recv->rem_addr.sin_addr.S_un.S_addr)
            , CLRPC_NTOHS(udp_recv->rem_addr.sin_port)
            , pkt->server_boot, ((pkt->serial_hi << 8) | pkt->serial_lo), pkt->fragnum
            , pkt->drep[0], pkt->drep[1], pkt->drep[2]
            );
        CLRPC_SV_PKT_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
            , ".RECV ptype(%u)  opnum(%u)  len(%u)  flags(%#x:%c%c%c%c%c%c)  flags2(%#x)"
            , pkt->ptype, pkt->opnum, pkt->len
            , pkt->flags
                , pkt->flags & CLRPC_PF_FRAG ? 'F' : '-'
                , pkt->flags & CLRPC_PF_LAST_FRAG ? 'L' : '-'
                , pkt->flags & CLRPC_PF_NO_FACK ? 'N' : '-'
                , pkt->flags & CLRPC_PF_MAYBE ? 'm' : '-'
                , pkt->flags & CLRPC_PF_IDEMPOTENT ? 'i' : '-'
                , pkt->flags & CLRPC_PF_BROADCAST ? 'b' : '-'
            , pkt->flags2
            );
        CLRPC_SV_PKT_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
            , ".RECV actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)  actseq(%u)"
            , pkt->actuid.time_low, pkt->actuid.time_mid, pkt->actuid.time_hi_and_version
            , pkt->actuid.clock_seq_hi_and_reserved, pkt->actuid.clock_seq_low
            , pkt->actuid.node[0], pkt->actuid.node[1], pkt->actuid.node[2]
            , pkt->actuid.node[3], pkt->actuid.node[4], pkt->actuid.node[5]
            , pkt->seq
            );
        CLRPC_SV_PKT_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
            , ".RECV ifuuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) vers=0x%08x"
            , pkt->if_id.time_low, pkt->if_id.time_mid, pkt->if_id.time_hi_and_version
            , pkt->if_id.clock_seq_hi_and_reserved, pkt->if_id.clock_seq_low
            , pkt->if_id.node[0], pkt->if_id.node[1], pkt->if_id.node[2]
            , pkt->if_id.node[3], pkt->if_id.node[4], pkt->if_id.node[5]
            , pkt->if_vers
            );
        CLRPC_SV_PKT_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
            , ".RECV objuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
            , pkt->object.time_low, pkt->object.time_mid, pkt->object.time_hi_and_version
            , pkt->object.clock_seq_hi_and_reserved, pkt->object.clock_seq_low
            , pkt->object.node[0], pkt->object.node[1], pkt->object.node[2]
            , pkt->object.node[3], pkt->object.node[4], pkt->object.node[5]
            );
    }
#endif

    /* look for existing actuid */
    scall = (CLRPC_SCALL_PTR_TYPE)clrpc_hash_lookup (& clrpc_sc_hash, & pkt->actuid, pkt->ahint);

    if (CLRPC_PTR_IS_NOT_NULL (scall))
    {
        /* for this activity uuid exists an scall-entry */
        
        CLRPC_SV_PKT_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
            , ".RECV ifuuid scall(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) "
            , scall->server->if_uuid.time_low, scall->server->if_uuid.time_mid, scall->server->if_uuid.time_hi_and_version
            , scall->server->if_uuid.clock_seq_hi_and_reserved, scall->server->if_uuid.clock_seq_low
            , scall->server->if_uuid.node[0], scall->server->if_uuid.node[1], scall->server->if_uuid.node[2]
            , scall->server->if_uuid.node[3], scall->server->if_uuid.node[4], scall->server->if_uuid.node[5]
            );
            
        /* AP01452173 (WAY2-Antwort von PCIOX wird von CLRPC nicht akzeptiert)
        */
        if (pkt->ptype == CLRPC_PT_RESPONSE )
        {
            /* for a received RESPONSE on the server side the actuid is all we have to check, 
            the interface-uuid does not have to be filled out by the client, so we can't check it */
            /* "dispatch" is at the end of the function */
        }
        
        /* AP00951641 (korrektes EPM-Handling)
           AP01003562 (Handling of rpc-calls with different interface-uuid within one activity-uuid) 
           
           within the same activity-uuid the user could have changed the interface-uuid 
           ==> check this */
        else if (clrpc_uuid_equal(& scall->server->if_uuid, & pkt->if_id) || 
            clrpc_uuid_equal(& pkt->if_id, & clrpc_conversation_manager_uuid)) 
        {
            /* the interface has not changed or it was the conversation manager */
            
                /*AP00469935*/
                /* dispatch this pkt  if there is an obj_uuid for this call
                //	each pkt must hold the obj_uuid, because all pkt may send to the epm
                //	and the epm uses if_uuid and obj_uuid to find the desired server
                //----------------------------------------------------------------*/
            if_obj = clrpc_sv_if_obj_lookup (scall->server, & pkt->object);
            if (CLRPC_PTR_IS_NULL (if_obj))
            {
                CLRPC_SV_TRACE_00 (server->trace_idx, LSA_TRACE_LEVEL_WARN
                    , "object_uuid unregistered in the meantime, send nca_unknown_if"
                    );
                clrpc_sc_do_orphan (scall);
                if (scall->state == CLRPC_CST_FREE) 
                {
                    clrpc_sv_scall_free (scall->server, & scall);
                }
                else 
                {
                    CLRPC_ASSERT (scall->upper_wf > 0);
                    CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
                        , "cid(%u) upper_wf(%u) wait for upper.rsp, free is done in clrpc_sc_dispatch()"
                        , scall->id
                        , scall->upper_wf
                        );
                }

                clrpc_sv_send_reject (server, lower, CLRPC_NCA_UNKNOWN_IF);
                return CLRPC_PROVIDE_LOWER;
            }
        }
        else
        {
            CLRPC_SERVER_PTR_TYPE  server_forward;
            /* the interface has changed ==> search the appropriate server 
                AP00951641 (korrektes EPM-Handling)
                AP01003562 (Handling of rpc-calls with different interface-uuid within one activity-uuid) 
            */
            
            /* only with a new REQUEST we accept the change of the interface */
            if (pkt->ptype != CLRPC_PT_REQUEST) 
            {
                CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "ptype(%u) != REQUEST and interface changed, drop!"
                    , pkt->ptype
                    );
                return CLRPC_PROVIDE_LOWER;
            }
            
            /* all fragments of one call have to address the same interface */
            /* unfragmented or fragmented with fragnum 0 */
            if ((pkt->flags & CLRPC_PF_FRAG)  &&  pkt->fragnum != 0) 
            {
                CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "fragnum(%u) != 0 and interface changed!, drop"
                    , pkt->fragnum
                    );
                return CLRPC_PROVIDE_LOWER;
            }
            
                /* server must be the endpoint-mapper, else send reject */
            if (server->is_endpoint_mapper == 0) 
            {
                CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "sid(%u) != endpoint-mapper, send reject!"
                    , server->id
                    );
                clrpc_sv_send_reject (server, lower, CLRPC_NCA_UNKNOWN_IF);
                return CLRPC_PROVIDE_LOWER;
            }
            
            /* must find desired service in server-database, else send reject */
            server_forward = clrpc_epm_lookup_server (
                & pkt->if_id,
                CLRPC_IF_VERS_MAJOR (pkt->if_vers), CLRPC_IF_VERS_MINOR (pkt->if_vers),
                & pkt->object
                );
            if (CLRPC_PTR_IS_NULL (server_forward)) 
            {
                CLRPC_SV_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "forward not found pkt.version(%#x)"
                    " pkt.if_id(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x),"
                    " send reject!"
                    , pkt->if_vers
                    , pkt->if_id.time_low, pkt->if_id.time_mid, pkt->if_id.time_hi_and_version
                    , pkt->if_id.clock_seq_hi_and_reserved, pkt->if_id.clock_seq_low
                    , pkt->if_id.node[0], pkt->if_id.node[1], pkt->if_id.node[2]
                    , pkt->if_id.node[3], pkt->if_id.node[4], pkt->if_id.node[5]
                    );
                clrpc_sv_send_reject (server, lower, CLRPC_NCA_UNKNOWN_IF);
                return CLRPC_PROVIDE_LOWER;
            }

            CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
                , "FORWARDING from sid(%u) to sid(%u), interface changed"
                , server->id
                , server_forward->id
                );
            
            
            if (server_forward->scall_cnt >= CLRPC_CFG_MAX_SCALLS) 
            {
                CLRPC_SV_TRACE_02 (server_forward->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "sid(%u) max scalls(%u)"
                    , server_forward->id
                    , server_forward->scall_cnt
                    );
                clrpc_sv_send_reject (server, lower, CLRPC_NCA_SERVER_TOO_BUSY);
                return CLRPC_PROVIDE_LOWER;
            }
            

            if (CLRPC_PTR_IS_NOT_NULL(scall->upper))
            {
                /* upper ressource in scall came from old server, after switching, we need ressources from the new server */
                CLRPC_UPPER_RQB_PTR_TYPE    tmp_upper_rqb_ptr;
                
                tmp_upper_rqb_ptr = clrpc_sv_provide_dequeue(server_forward);
                if (CLRPC_PTR_IS_NULL (tmp_upper_rqb_ptr)) 
                {
                    CLRPC_SV_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
                        , "sid(%u): no upper indication resource for cid(%u), send too_busy"
                        , server_forward->id, scall->id
                        );
                    clrpc_sv_send_reject (server, lower, CLRPC_NCA_SERVER_TOO_BUSY);
                    return CLRPC_PROVIDE_LOWER;
                }
                
                tmp_upper_rqb_ptr->args.sv.call->server_id   = server_forward->id;
                tmp_upper_rqb_ptr->args.sv.call->opnum = pkt->opnum;
                
                clrpc_sv_provide_enqueue(server, & scall->upper);
                scall->upper = tmp_upper_rqb_ptr;
                scall->up_call = tmp_upper_rqb_ptr->args.sv.call;
                
                clrpc_if_obj_opnum_count_incr (& server_forward->obj
                    , server_forward->pnio_signature, scall->up_call->opnum
                    );
                clrpc_if_obj_opnum_count_decr (& server->obj
                    , server->pnio_signature, scall->up_call->opnum
                    );
            }
            CLRPC_ASSERT (server->scall_cnt > 0);
            server_forward->scall_cnt ++;
            server->scall_cnt --;
            server_forward->scall_next_id++;
            scall->id = server_forward->scall_next_id;
            scall->trace_idx = server_forward->trace_idx;
    
            scall->server = server_forward;
        }
    }
    else
    {
        LSA_UINT32 error_code = 0;
        
            /* there is no scall for this activity uuid */

            /* a ping, it seems we missed a request, send nocall */
        if (pkt->ptype == CLRPC_PT_PING) {
            /* should first check boot_time, but no server known; cl send a request => boot_time is checked */
            CLRPC_SV_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
                , "PING actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x), send nocall"
                , pkt->actuid.time_low, pkt->actuid.time_mid, pkt->actuid.time_hi_and_version
                , pkt->actuid.clock_seq_hi_and_reserved, pkt->actuid.clock_seq_low
                , pkt->actuid.node[0], pkt->actuid.node[1], pkt->actuid.node[2]
                , pkt->actuid.node[3], pkt->actuid.node[4], pkt->actuid.node[5]
                );
            clrpc_sv_send_nocall (server, lower, 0xFFFF);
            return CLRPC_PROVIDE_LOWER;
        }

            /* for a new scall, we accept only requests */
        if (! (pkt->ptype == CLRPC_PT_REQUEST)) {
            CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                , "ptype(%u) != REQUEST, drop!"
                , pkt->ptype
                );
            return CLRPC_PROVIDE_LOWER;
        }

            /* unfragmented or fragmented with fragnum 0 */
        if ((pkt->flags & CLRPC_PF_FRAG)  &&  pkt->fragnum != 0) {
            CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                , "fragnum(%u) != 0, drop!"
                , pkt->fragnum
                );
            return CLRPC_PROVIDE_LOWER;
        }

            /* and our registerd interface */
        if (! clrpc_uuid_equal (& server->if_uuid, & pkt->if_id)) {
                /* forwarding
                //	if the server is the endpoint-mapper, we try to forward
                //	the paket to the wanted service
                //	forwarding from every-port to every-port is possible, but not wanted
                //-----------------------------------------------------------*/
            CLRPC_SERVER_PTR_TYPE  server_forward;

            CLRPC_SV_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "sid(%u) if_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) != "
                , server->id
                , server->if_uuid.time_low, server->if_uuid.time_mid, server->if_uuid.time_hi_and_version
                , server->if_uuid.clock_seq_hi_and_reserved, server->if_uuid.clock_seq_low
                , server->if_uuid.node[0], server->if_uuid.node[1], server->if_uuid.node[2]
                , server->if_uuid.node[3], server->if_uuid.node[4], server->if_uuid.node[5]
                );
            CLRPC_SV_TRACE_11 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
                , "pkt.if_id(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
                , pkt->if_id.time_low, pkt->if_id.time_mid, pkt->if_id.time_hi_and_version
                , pkt->if_id.clock_seq_hi_and_reserved, pkt->if_id.clock_seq_low
                , pkt->if_id.node[0], pkt->if_id.node[1], pkt->if_id.node[2]
                , pkt->if_id.node[3], pkt->if_id.node[4], pkt->if_id.node[5]
                );

                /* server must be the endpoint-mapper, else send reject */
            if (server->is_endpoint_mapper == 0) {
                CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "sid(%u) != endpoint-mapper, send reject!"
                    , server->id
                    );
                clrpc_sv_send_reject (server, lower, CLRPC_NCA_UNKNOWN_IF);
                return CLRPC_PROVIDE_LOWER;
            }

                /* must find desired service in server-database, else send reject */
            server_forward = clrpc_epm_lookup_server (
                    & pkt->if_id,
                    CLRPC_IF_VERS_MAJOR (pkt->if_vers), CLRPC_IF_VERS_MINOR (pkt->if_vers),
                    & pkt->object
                );
            if (CLRPC_PTR_IS_NULL (server_forward)) {
                CLRPC_SV_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                    , "forward not found pkt.version(%#x)"
                      " pkt.if_id(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x),"
                      " send reject!"
                    , pkt->if_vers
                    , pkt->if_id.time_low, pkt->if_id.time_mid, pkt->if_id.time_hi_and_version
                    , pkt->if_id.clock_seq_hi_and_reserved, pkt->if_id.clock_seq_low
                    , pkt->if_id.node[0], pkt->if_id.node[1], pkt->if_id.node[2]
                    , pkt->if_id.node[3], pkt->if_id.node[4], pkt->if_id.node[5]
                    );
                clrpc_sv_send_reject (server, lower, CLRPC_NCA_UNKNOWN_IF);
                return CLRPC_PROVIDE_LOWER;
            }

                /* a note on forwarding and resource provide:
                //	Q: will the resources float from one server to an other ?
                //	A: NO, because on every clrpc_sv_event_recv we write the receive-handle and the
                //	receive-sock_fd to the lower-rqb (see SOCK_OPC_UDP_RECV)
                //	clrpc_sv_ll_recv_provide() uses this information to provide
                //	the right server with the lower-rqb
                //-----------------------------------------------------------*/
            CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
                , "FORWARDING from sid(%u) to sid(%u)"
                , server->id
                , server_forward->id
                );
            server = server_forward;
        }

            /* with a matching version */
        if (! (server->if_major == CLRPC_IF_VERS_MAJOR (pkt->if_vers)
            &&  server->if_minor >= CLRPC_IF_VERS_MINOR (pkt->if_vers)))
        {
            CLRPC_SV_TRACE_04 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
                , "version mismatch(%u.%u) != (%u.%u), send reject!"
                , server->if_major
                , server->if_minor
                , CLRPC_IF_VERS_MAJOR (pkt->if_vers)
                , CLRPC_IF_VERS_MINOR (pkt->if_vers)
                );
            clrpc_sv_send_reject (server, lower, CLRPC_NCA_UNKNOWN_IF);
            return CLRPC_PROVIDE_LOWER;
        }

            /* if there is a object_id, it must match.
            //	at this stage, the port is known, this means: the interface must match.
            //	only a test for the existence of the object_id is needed
            //-----------------------------------------------------------*/
        if_obj = clrpc_sv_if_obj_lookup (server, & pkt->object);
        if (CLRPC_PTR_IS_NULL (if_obj)) {
            CLRPC_SV_TRACE_00 (server->trace_idx, LSA_TRACE_LEVEL_WARN
                , "obj_uuid not found after server forwarding, strange"
                );
                /* OSF: send simple nca_s_unk_if, see dgfwd.c(220) */
            clrpc_sv_send_reject (server, lower, CLRPC_NCA_UNKNOWN_IF);
            return CLRPC_PROVIDE_LOWER;
        }

            /* now we are ready to create a new scall entry */
        scall = clrpc_sv_scall_alloc (&error_code, server, & pkt->actuid, pkt->seq);
        if (CLRPC_PTR_IS_NULL (scall)) {
                /* PNIO: send server busy or protocol_error */
            CLRPC_ASSERT(error_code != 0);
            clrpc_sv_send_reject (server, lower, error_code);
            return CLRPC_PROVIDE_LOWER;
        }

    }
        /* and delegate the work */
    clrpc_sc_dispatch (scall, LSA_NULL, lower, 0, LSA_NULL);
    return CLRPC_OK;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_ll_recv_provide
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_ll_recv_provide (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sv_ll_recv_provide")
	CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (server));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (lower));

	CLRPC_SV_TRACE_03 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "recv.provide: lower(%08x) lower.sock_fd(%u)  server.sock_fd(%u)"
		, lower, lower->args.data.sock_fd
		, server->sock_fd
		);

	CLRPC_ASSERT (lower->args.data.buffer_length == CLRPC_BUFFER_SIZE);
	lower->args.data.data_length = CLRPC_BUFFER_SIZE; /* sock need this !! */

#ifdef CLRPC_MESSAGE
# pragma CLRPC_MESSAGE("todo: drop server here, make this confusing buffer circulation clear")
#endif /* CLRPC_MESSAGE */
	/*
	Note: clrpc_sv_lower_done() must have done this things
	CLRPC_LL_SET_HANDLE (lower, server->channel->handle_lower);
	udp_recv = CLRPC_LL_GET_PPARAM (lower); CLRPC_ASSERT (udp_recv);
	udp_recv->sock_fd = server->sock_fd;
	*/

	CLRPC_LL_SET_OPCODE (lower, SOCK_OPC_UDP_RECEIVE);

	CLRPC_REQUEST_LOWER (lower, sys_channel->sysptr); /* don't use server->channel, may wrong channel to lower */

/* do not use, because of user_id: keep original user_id of EPM!
	clrpc_request_lower (lower, SOCK_OPC_UDP_RECEIVE, server);
*/
}


/*------------------------------------------------------------------------------
//	clrpc_sv_scall_alloc
// *error_code_ptr is valid only when the returned pointer is NULL
//----------------------------------------------------------------------------*/
static CLRPC_SCALL_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_scall_alloc (
    LSA_UINT32 CLRPC_LOCAL_MEM_ATTR * error_code_ptr,
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_UUID_PTR_TYPE  actuid,
	LSA_UINT32  seq
	)
{
	CLRPC_LOG_FCT("clrpc_sv_scall_alloc")
	CLRPC_SCALL_PTR_TYPE  scall;

    *error_code_ptr = 0;
    
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (server));

	CLRPC_SV_TRACE_03 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) scall_cnt(%u) CLRPC_CFG_MAX_SCALLS(%u)"
		, server->id, server->scall_cnt, CLRPC_CFG_MAX_SCALLS
		);
	CLRPC_SV_TRACE_12 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) actuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, server->id
		, actuid->time_low, actuid->time_mid, actuid->time_hi_and_version
		, actuid->clock_seq_hi_and_reserved, actuid->clock_seq_low
		, actuid->node[0], actuid->node[1], actuid->node[2]
		, actuid->node[3], actuid->node[4], actuid->node[5]
		);

	if (clrpc_uuid_is_nil (actuid)) {
		CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "sid(%u) nil-actuid"
			, server->id
			);
        *error_code_ptr = CLRPC_NCA_PROTO_ERROR;
		return LSA_NULL;
	}

	if (server->scall_cnt >= CLRPC_CFG_MAX_SCALLS) {
		CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "sid(%u) max scalls(%u)"
			, server->id
			, server->scall_cnt
			);
        *error_code_ptr = CLRPC_NCA_SERVER_TOO_BUSY;
		return LSA_NULL;
	}

	CLRPC_ALLOC_LOCAL_MEM (CLRPC_CAST_LOCAL_MEM_PTR_PTR(& scall), sizeof(*scall));
	if (CLRPC_PTR_IS_NULL (scall)) {
		CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "sid(%u) !scalls, no mem size(%u)"
			, server->id, sizeof(*scall)
			);
        *error_code_ptr = CLRPC_NCA_SERVER_TOO_BUSY;
		return LSA_NULL;
	}

	server->scall_next_id += 1;
	server->scall_cnt += 1;

	CLRPC_SV_TRACE_03 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) new cid(%u)(%08x)"
		, server->id
		, server->scall_next_id
		, scall
		);

		/* setup a scall */
	scall->state = CLRPC_CST_INIT;		/* reservation */
	scall->state_pre = 0xFFFF;
	scall->server = server;

	scall->trace_idx = server->trace_idx;
	scall->id = server->scall_next_id;
	scall->self = scall;

#if CLRPC_DEBUG
	scall->linkage.Flink = scall->linkage.Blink = LSA_NULL;
#endif

	clrpc_hash_entry_setup (& scall->hashed_act, actuid, scall);
	scall->seq = seq - 1;
	scall->ahint = clrpc_hash_add (& clrpc_sc_hash, & scall->hashed_act);
	scall->busy = 0;

	scall->upper = LSA_NULL;
	scall->up_call = LSA_NULL;
	scall->upper_wf = 0;
	scall->upper_drop = 0;

		/* timeout */
	scall->oneshot_queue = 0;
	clrpc_oneshot_init (& scall->oneshot, clrpc_sc_oneshot_callback, scall);

		/* recv */
	clrpc_queue_init (& scall->recv.queue);
	scall->recv.host = 0;
	scall->recv.port = 0;
	scall->recv.fragnum = 0;
	scall->recv.cancel_id = (LSA_UINT32)-1;

		/* send */
	clrpc_queue_init (& scall->send.answer_queue);
	clrpc_queue_init (& scall->send.send_queue);
	scall->send.in_queue = 0;
	scall->send.serial = (LSA_UINT16)-1;
	scall->send.ptype = (LSA_UINT16)-1;
	scall->send.next_state = 0;
	scall->send.again = 0;

	scall->send.frag_flags = 0;
	scall->send.frag_ack_fragnum = 0;
	scall->send.frag_window_size = 0;
	scall->send.frag_curr_fragnum = 0;
	scall->send.frag_last_fragnum = 0;

	scall->send.fack_serial = 0;

	scall->send.reject_fault_error = 0;

	scall->send.cancel_id = 0;

		/* way */
	scall->way.state = CLRPC_WAY_ST_UNKNOWN;
	scall->way.timeout_cnt = 0;

		/* start timer for self destruction */
	clrpc_oneshot_trigger (& scall->oneshot, CLRPC_CFG_SCALL_DIE_TIMEOUT_MSEC);

	return scall;
}


/*------------------------------------------------------------------------------
//	clrpc_sv_scall_free
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOWER_IN_FCT_ATTR
clrpc_sv_scall_free (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_SCALL_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR * scall
	)
{
	CLRPC_LOG_FCT("clrpc_sv_scall_free")
	LSA_UINT16  rc;

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (server));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (*scall));
	CLRPC_ASSERT (CLRPC_PTR_ARE_EQUAL ((*scall)->self, (*scall)));

	CLRPC_SV_TRACE_04 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) cid(%u)(%08x)  scall_cnt(%u)"
		, server->id, (*scall)->id, *scall, server->scall_cnt
		);

	CLRPC_ASSERT ((*scall)->state == CLRPC_CST_FREE);

	CLRPC_ASSERT (server->scall_cnt > 0);
	server->scall_cnt -= 1;
	(*scall)->self = LSA_NULL;

	CLRPC_FREE_LOCAL_MEM (& rc, *scall);
	CLRPC_ASSERT(rc == LSA_RET_OK);
	*scall = LSA_NULL;

		/* now we can do the callback for op_unregister */
	if (server->do_unregister == 2 && server->scall_cnt == 0)
	{
		//LSA_UINT16  rc;
		CLRPC_SV_TRACE_00 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "last scall free, do unregister callback"
			);
		CLRPC_ASSERT (server->upper);

		rc = clrpc_sv_sock_close (server);
		if (rc != CLRPC_OK)
		{
			CLRPC_ASSERT (server->state != CLRPC_SST_FREE);
			clrpc_callback (server->channel, rc, & server->upper, CLRPC_MODULE_ID, __LINE__);
		}
	}
}


/*------------------------------------------------------------------------------
//	clrpc_sv_init
//	initialise the server part, must call once on startup
//----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_init (
	LSA_VOID
	)
{
	LSA_UINT16  id;

#if CLRPC_INVALID_SERVER_ID < CLRPC_CFG_MAX_SERVERS
# error "check CLRPC_CFG_MAX_SERVERS"
#endif

	/*
	//	see:	CLRPC_RQB_SET_PREV_RQB_PTR (server->upper, channel);	// !CAST!
	//			channel = CLRPC_RQB_GET_PREV_RQB_PTR (server->upper);	// !CAST!
	*/
	if (sizeof(CLRPC_UPPER_RQB_PTR_TYPE) != sizeof(CLRPC_CHANNEL_PTR_TYPE)) {
		CLRPC_UPPER_RQB_ERROR (LSA_NULL);
	}

	for (id = 0; id < CLRPC_CFG_MAX_SERVERS; ++id) {
		clrpc_servers[id].state = CLRPC_SST_FREE;
	}

	clrpc_hash_init (& clrpc_sc_hash);
	clrpc_hash_init (& clrpc_ctx_hash);

	clrpc_sv_pnio_init();

	return LSA_RET_OK; /* not CLRPC_OK */
}


/*------------------------------------------------------------------------------
//	clrpc_sv_undo_init
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_undo_init(
    LSA_VOID
	)
{
	clrpc_sv_pnio_undo_init ();

#if CLRPC_DEBUG
	CLRPC_ASSERT (clrpc_hash_count_entries (& clrpc_sc_hash) == 0);
	CLRPC_ASSERT (clrpc_hash_count_entries (& clrpc_ctx_hash) == 0);
#endif

	clrpc_hash_undo_init (& clrpc_ctx_hash);

	clrpc_hash_undo_init (& clrpc_sc_hash);
}


/*------------------------------------------------------------------------------
// init the server channel part
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_init_channel (
	CLRPC_CHANNEL_PTR_TYPE channel
	)
{
	channel->sv.upper = LSA_NULL;
}


/*------------------------------------------------------------------------------
// checking if closing is Ok
//----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_undo_init_channel (
	CLRPC_CHANNEL_PTR_TYPE  channel
	)
{
	CLRPC_LOG_FCT("clrpc_sv_undo_init_channel")
	LSA_UINT16  id;

	for (id = 0; id < CLRPC_CFG_MAX_SERVERS; ++id) {
		CLRPC_SERVER_PTR_TYPE  server = & clrpc_servers[id];
		if (server->state != CLRPC_SST_FREE) {
			if (CLRPC_PTR_ARE_EQUAL (server->channel, channel)) {
				CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "sid(%u) not unregistered, state(%u)"
					, server->id, server->state
					);
				return 0;
			}
		}
	}
	return 1; /* == close channel allowed */
}


/*------------------------------------------------------------------------------
//	CLRPC_OPC_SV_REGISTER_IF_OBJ
//
//	work:
//		. create a new server instance (a interface-objec-uuid pair)
//		. open a own sock-fd for this pair
//		. provide recv-resource
//----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_register_if_obj (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	)
{
	CLRPC_LOG_FCT("clrpc_sv_op_register_if_obj")
	LSA_UINT16  rc;

		/* more param-check is done in clrpc_sv_op_register_internal() */

	if (! clrpc_system_channel_is_ready ())
	{
		CLRPC_SV_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "system-channel not ready yet"
			);
		clrpc_callback (channel, CLRPC_ERR_SEQUENCE, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}

	if (channel->detailptr->path_type != CLRPC_PATH_TYPE_USER)
	{
		CLRPC_SV_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "only allowed within CLRPC_PATH_TYPE_USER"
			);
		clrpc_callback (channel, CLRPC_ERR_SYS_PATH, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}
    
    if (!clrpc_mac_addr_is_valid(channel->mac_addr))
    {
		CLRPC_SV_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "only allowed after CLRPC_OPC_SET_INTERFACE_PARAM with mac_addr"
			);
		clrpc_callback (channel, CLRPC_ERR_SEQUENCE, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
    }

		/* !CAST! keep original upper channel */
    CLRPC_RQB_SET_PREV_RQB_PTR (rb, (CLRPC_UPPER_RQB_PTR_TYPE)((LSA_VOID *)channel));

	rc = clrpc_sv_op_register_internal (rb);
	if (rc != CLRPC_OK)
	{
		clrpc_callback (channel, rc, & rb, CLRPC_MODULE_ID, __LINE__);
	}
}


/*----------------------------------------------------------------------------*/
LSA_UINT32 CLRPC_LOCAL_FCT_ATTR clrpc_build_server_boot_time()
{
    LSA_UINT64 act_boot_time;
    LSA_UINT16  ret_val;

    CLRPC_GET_100NS_COUNTER(&ret_val, &act_boot_time);
    if (ret_val != LSA_RET_OK)
    {
        CLRPC_FATAL1(ret_val);
    }
    else
    {
        /* boot_time is in seconds */
        act_boot_time = act_boot_time / CLRPC_100NS_PER_SEC; 
        if ((LSA_UINT32) act_boot_time <= clrpc_data.last_boot_time)
        {
            clrpc_data.last_boot_time ++;
        }
        else
        {
            clrpc_data.last_boot_time = (LSA_UINT32) act_boot_time;   
        }
            
        /* 0 has a special meaning, is not allowed as server_boot_time */
        if (clrpc_data.last_boot_time == 0)
        {
            clrpc_data.last_boot_time ++;
        }
    }
    return(clrpc_data.last_boot_time);
}
/*----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_register_internal (
	CLRPC_UPPER_RQB_PTR_TYPE	rb_register /* CLRPC_OPC_SV_REGISTER_IF_OBJ or CLRPC_OPC_EPM_START */
	)
{
	CLRPC_LOG_FCT("clrpc_sv_op_register_internal")
	LSA_UINT16  id;
	CLRPC_CHANNEL_PTR_TYPE  orig_upper_channel;
	
	CLRPC_SERVER_PTR_TYPE  server;
	CLRPC_CHANNEL_PTR_TYPE  server_channel;

	CLRPC_UPPER_RQB_PTR_TYPE  upper;
	CLRPC_UUID_TYPE  if_uuid;
	LSA_UINT16  if_major, if_minor, udp_port, opnum_max, pnio_signature;
	LSA_UINT16  is_endpoint_mapper, recv_resource_count, send_resource_count;
	LSA_UINT32  alloc_len;
    LSA_UINT32  trace_idx;

	CLRPC_UUID_TYPE  obj_uuid;
	LSA_UINT8  obj_annotation[CLRPC_MAX_ANNOTATION_SIZE];
	LSA_VOID_PTR_TYPE  obj_usr_id;

		/* search new server instance */
    for (id = 0; id < CLRPC_CFG_MAX_SERVERS; ++id) 
    {
        if (clrpc_servers[id].state == CLRPC_SST_FREE) 
        {
			break;
		}
	}
    if (id >= CLRPC_CFG_MAX_SERVERS) 
    {
		CLRPC_SV_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "no (more) server_resources, max(%u)"
			, CLRPC_CFG_MAX_SERVERS
			);
		return CLRPC_ERR_RESOURCE;
	}

    orig_upper_channel = (CLRPC_CHANNEL_PTR_TYPE) ((LSA_VOID *)CLRPC_RQB_GET_PREV_RQB_PTR (rb_register));
    if (CLRPC_PTR_IS_NULL (orig_upper_channel)) 
    {
		CLRPC_UPPER_RQB_ERROR (rb_register);
		// unreachable code:  return CLRPC_ERR_PARAM;
	}

		/* register a new interface */
    if (CLRPC_RQB_GET_OPCODE(rb_register) == CLRPC_OPC_SV_REGISTER_IF_OBJ)
	{
		CLRPC_UPPER_SV_REGISTER_IF_OBJ_PTR_TYPE  reg = rb_register->args.sv.reg_if_obj;

		server_channel			= orig_upper_channel;
	    trace_idx			    = server_channel->trace_idx;

		upper					= rb_register;

		if_uuid					= reg->if_uuid;
		if_major				= reg->if_version_major;
		if_minor				= reg->if_version_minor;
		is_endpoint_mapper		= 0;

		udp_port				= reg->udp_port;
		alloc_len				= reg->alloc_len;
		opnum_max				= reg->opnum_max;
		pnio_signature			= reg->pnio_signature;
		recv_resource_count		= reg->recv_resource_count;
		send_resource_count		= reg->send_resource_count;

		obj_uuid						= reg->object_uuid;
		CLRPC_MEMCPY (obj_annotation	, reg->annotation, CLRPC_MAX_ANNOTATION_SIZE);
		obj_usr_id						= reg->usr_id;

			/* set result */
		reg->server_id = id;

		CLRPC_SV_TRACE_12 (trace_idx, LSA_TRACE_LEVEL_NOTE
			, "register_if: sid(%u) if_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, id
			, if_uuid.time_low, if_uuid.time_mid, if_uuid.time_hi_and_version
			, if_uuid.clock_seq_hi_and_reserved, if_uuid.clock_seq_low
			, if_uuid.node[0], if_uuid.node[1], if_uuid.node[2]
			, if_uuid.node[3], if_uuid.node[4], if_uuid.node[5]
			);
		CLRPC_SV_TRACE_07 (trace_idx, LSA_TRACE_LEVEL_NOTE
			, "register_if: sid(%u) udp.port(%u) alloc_len(%u) opnum_max(%u) pnio_sig(%u)"
				" send_res_cnt(%u) recv_res_cnt(%u)"
			, id, udp_port, alloc_len, opnum_max, pnio_signature
			, send_resource_count, recv_resource_count
			);
	}

		/* or register the endpoint-mapper interface */
    else
	{
		CLRPC_UPPER_EPM_START_PTR_TYPE  epm = rb_register->args.epm.start;
		CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE(rb_register) == CLRPC_OPC_EPM_START);

		server_channel			= clrpc_system_channel();
	    trace_idx			    = 0;
        
        /* copy mac_addr to system_channel for context_handle of epm */
        CLRPC_MEMCPY(server_channel->mac_addr, orig_upper_channel->mac_addr, sizeof(server_channel->mac_addr));

		upper					= rb_register;

		if_uuid					= clrpc_endpoint_mapper_uuid;
		if_major				= 3;
		if_minor				= 0;
		is_endpoint_mapper		= 1;

		udp_port				= CLRPC_EPM_DEFAULT_PORT; /* note: CLRPC_DETAIL_TYPE::ep_mapper_port was removed (AP00619205) */
		alloc_len				= CLRPC_EPM_ALLOC_LEN;
		opnum_max				= 4;
		pnio_signature			= 0;
		recv_resource_count		= epm->recv_resource_count;
		send_resource_count		= epm->send_resource_count;

		obj_uuid						= clrpc_uuid_nil_uuid;
		CLRPC_MEMCPY (obj_annotation	, epm->annotation, CLRPC_MAX_ANNOTATION_SIZE);
		obj_usr_id						= LSA_NULL;

			/* set result */
		epm->server_id = id;

		CLRPC_EPM_TRACE_12 (trace_idx, LSA_TRACE_LEVEL_NOTE
			, "epm_start: sid(%u) if_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, id
			, if_uuid.time_low, if_uuid.time_mid, if_uuid.time_hi_and_version
			, if_uuid.clock_seq_hi_and_reserved, if_uuid.clock_seq_low
			, if_uuid.node[0], if_uuid.node[1], if_uuid.node[2]
			, if_uuid.node[3], if_uuid.node[4], if_uuid.node[5]
			);
		CLRPC_EPM_TRACE_04 (trace_idx, LSA_TRACE_LEVEL_NOTE
			, "epm_start: sid(%u) udp.port(%u) send_res_cnt(%u) recv_res_cnt(%u)"
			, id, udp_port, send_resource_count, recv_resource_count
			);
	}

		/* common parameter check */
    if (recv_resource_count < 1) 
    {
		CLRPC_SV_TRACE_01 (trace_idx, LSA_TRACE_LEVEL_ERROR
			, "too few recv_res(%u)", recv_resource_count
			);
		return CLRPC_ERR_PARAM;
	}
    if (send_resource_count < 1) 
    {
		CLRPC_SV_TRACE_01 (trace_idx, LSA_TRACE_LEVEL_ERROR
			, "too few send_res(%u)", send_resource_count
			);
		return CLRPC_ERR_PARAM;
	}
    if (alloc_len > 0xFFFF) 
    {
		CLRPC_SV_TRACE_01 (trace_idx, LSA_TRACE_LEVEL_ERROR
			, "not supported: alloc_len(%u) > 0xFFFF", alloc_len
			);
		return CLRPC_ERR_PARAM;
	}
    server = clrpc_epm_find_server (1, & if_uuid, if_major, if_minor, & obj_uuid);
    if (CLRPC_PTR_IS_NOT_NULL (server)) 
    {
		CLRPC_SV_TRACE_01 (trace_idx, LSA_TRACE_LEVEL_ERROR
			, "interface / version / obj_uuid already registered as server_id sid(%u)"
			, server->id
			);
		return CLRPC_ERR_PARAM;
	}	

		/* alloc a new server if */
    server = & clrpc_servers[id];

	server->channel				= server_channel;
	server->trace_idx			= trace_idx;
	server->state				= CLRPC_SST_IDLE;
	server->id					= id;
	server->upper				= upper;

	CLRPC_MEMCPY (& server->if_uuid, & if_uuid, sizeof (server->if_uuid));
	server->if_major			= if_major;
	server->if_minor			= if_minor;
	server->is_endpoint_mapper	= is_endpoint_mapper;

	server->port				= udp_port;
	server->alloc_len			= alloc_len;
	server->opnum_max			= opnum_max;
	server->pnio_signature		= pnio_signature;

    server->server_boot = clrpc_build_server_boot_time();
	server->scall_next_id		= 0;
	server->scall_cnt			= 0;
	server->do_unregister		= 0;

		/* the object uuid */
	server->obj.object_uuid				= obj_uuid;
	CLRPC_MEMCPY (server->obj.annotation, obj_annotation, CLRPC_MAX_ANNOTATION_SIZE);
	server->obj.usr_id					= obj_usr_id;

		/* sanity */
	server->obj.annotation[CLRPC_MAX_ANNOTATION_SIZE-1] = 0;

		/* valid for pnio_signature only */
	server->obj.ar_count_max		= 0xFFFF; /* no limit */
	server->obj.ar_count_curr		= 0;
	server->obj.read_implicit_max	= CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID;
	server->obj.read_implicit_curr	= 0;

	clrpc_queue_init (& server->provided_queue);
	clrpc_queue_init (& server->send_rqb_queue);
	clrpc_queue_init (& server->send_scall_queue);

	CLRPC_SET_DISPATCH_TAG (server->dispatch_tag, clrpc_sv_lower_done);

	if (clrpc_sv_sock_open (server) != CLRPC_OK)
	{
		server->state = CLRPC_SST_FREE;
		return CLRPC_ERR_ALLOC_LOWER;
	}
	return CLRPC_OK;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_register_internal_done (
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sv_op_register_internal_done")
	CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();
	CLRPC_CHANNEL_PTR_TYPE  orig_upper_channel;
	CLRPC_LL_UDP_OPEN_PTR_TYPE  sock_open;
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;
	LSA_UINT32  response = CLRPC_LL_GET_RESPONSE (lower);
	LSA_UINT16  i;
	LSA_UINT16  recv_res_cnt, send_res_cnt;

		 /* !CAST! get original upper channel */
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (server->upper));

	orig_upper_channel = (CLRPC_CHANNEL_PTR_TYPE) ((LSA_VOID *)CLRPC_RQB_GET_PREV_RQB_PTR (server->upper));
    if (CLRPC_PTR_IS_NULL (orig_upper_channel)) 
    {
		CLRPC_UPPER_RQB_ERROR (server->upper);
		return;
	}
	/* orig_upper_channel may differ from server->channel */

    sock_open = & lower->args.udp_open;
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (sock_open));

	CLRPC_ASSERT (server->state == CLRPC_SST_IDLE);
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (server->upper));
    if (server->is_endpoint_mapper > 0)
	{
		CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE (server->upper) == CLRPC_OPC_EPM_START);
		recv_res_cnt = server->upper->args.epm.start->recv_resource_count;
		send_res_cnt = server->upper->args.epm.start->send_resource_count;
	}
    else 
    {
		CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE (server->upper) == CLRPC_OPC_SV_REGISTER_IF_OBJ);
		recv_res_cnt = server->upper->args.sv.reg_if_obj->recv_resource_count;
		send_res_cnt = server->upper->args.sv.reg_if_obj->send_resource_count;
	}

    if (response != SOCK_RSP_OK)
	{
		CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_ERROR, "response(%u) != OK", response);

		clrpc_ll_free_rqb (sys_channel, &lower);

		server->sock_fd = CLRPC_LL_INVALID_SOCKET;
		server->state = CLRPC_SST_FREE;

        if (server->is_endpoint_mapper > 0)
		{
			CLRPC_UPPER_RQB_PTR_TYPE  next = CLRPC_RQB_GET_NEXT_RQB_PTR(server->upper);

			clrpc_callback (orig_upper_channel, CLRPC_ERR_LOWER_LAYER, & server->upper, CLRPC_MODULE_ID, __LINE__);

				/* finish the queued epm_start requests, see AP01017769 */
            while (CLRPC_PTR_IS_NOT_NULL (next))
			{
				CLRPC_UPPER_RQB_PTR_TYPE  orig_upper_rb = next;
				next = CLRPC_RQB_GET_NEXT_RQB_PTR(next);
				
				orig_upper_channel = (CLRPC_CHANNEL_PTR_TYPE) ((LSA_VOID *)CLRPC_RQB_GET_PREV_RQB_PTR (orig_upper_rb));
                if (CLRPC_PTR_IS_NULL (orig_upper_channel)) 
                {
					CLRPC_UPPER_RQB_ERROR (orig_upper_rb);
				}
                clrpc_callback (orig_upper_channel, CLRPC_ERR_LOWER_LAYER, & orig_upper_rb, CLRPC_MODULE_ID, __LINE__);
			}
        }
		else
		{
			clrpc_callback (orig_upper_channel, CLRPC_ERR_LOWER_LAYER, & server->upper, CLRPC_MODULE_ID, __LINE__);
		}
		return;
	}

    server->sock_fd = sock_open->sock_fd;
	server->port = sock_open->loc_addr.sin_port;
	CLRPC_SV_TRACE_03 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "sid(%u) port(%u) sock_fd(%u)"
		, server->id, CLRPC_NTOHS(server->port), server->sock_fd
		);

    clrpc_ll_free_rqb (sys_channel, &lower);

		/* set out-arg */
	if (server->is_endpoint_mapper > 0)
	{
		server->upper->args.epm.start->udp_port = server->port;
	}
	else
	{
		server->upper->args.sv.reg_if_obj->udp_port = server->port;
	}

		/* provide recv resources */
	for (i = 0; i < recv_res_cnt; ++i)
	{
		lower = clrpc_sock_alloc_rxtx_rqb ();
        if (CLRPC_PTR_IS_NULL (lower)) 
        {
			CLRPC_FATAL1(0); /* alloc-error not handled */
		}

        CLRPC_LL_SET_USER_ID_PTR (lower, server);

			/* set lower info, see clrpc_sv_ll_recv_provide() */
		CLRPC_LL_SET_HANDLE (lower, sys_channel->handle_lower);
		udp_recv = & lower->args.data;
		udp_recv->sock_fd = server->sock_fd;

		clrpc_sv_ll_recv_provide (server, lower);
	}

		/* provide send resources */
    for (i = 0; i < send_res_cnt; ++i)
    {
        lower = clrpc_sock_alloc_rxtx_rqb ();
        if (CLRPC_PTR_IS_NULL (lower)) 
        {
	        CLRPC_FATAL1(0); /* alloc-error not handled */
        }

        clrpc_queue_enqueue (& server->send_rqb_queue, lower);
    }

		/* if endpoint-mapper:
		//	we must provide some local upper indication resources
		*/
    if (server->is_endpoint_mapper > 0)
	{
		clrpc_epm_provide (sys_channel, server, recv_res_cnt);

			/* epm-server ready now */
		server->state = CLRPC_SST_READY;

		{
		    CLRPC_UPPER_RQB_PTR_TYPE  next = CLRPC_RQB_GET_NEXT_RQB_PTR(server->upper);

		    clrpc_callback (orig_upper_channel, CLRPC_OK, & server->upper, CLRPC_MODULE_ID, __LINE__);

			    /* finish the queued epm_start requests, see AP01017769 */
            while (CLRPC_PTR_IS_NOT_NULL (next))
		    {
			    CLRPC_UPPER_RQB_PTR_TYPE  orig_upper_rb = next;
			    next = CLRPC_RQB_GET_NEXT_RQB_PTR(next);

			    orig_upper_channel = (CLRPC_CHANNEL_PTR_TYPE) ((LSA_VOID *)CLRPC_RQB_GET_PREV_RQB_PTR (orig_upper_rb));
                if (CLRPC_PTR_IS_NULL (orig_upper_channel)) 
                {
				    CLRPC_UPPER_RQB_ERROR (orig_upper_rb);
			    }
                clrpc_op_epm_start (orig_upper_channel, orig_upper_rb);
		    }
        }
	}
	else
	{
		clrpc_channel_set_trace_idx (orig_upper_channel, server->trace_idx);

			/* server ready now */
		server->state = CLRPC_SST_READY;
		clrpc_callback (orig_upper_channel, CLRPC_OK, & server->upper, CLRPC_MODULE_ID, __LINE__);
	}
}


/*------------------------------------------------------------------------------
//	CLRPC_OPC_SV_UNREGISTER_IF_OBJ
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_unregister_if_obj (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	)
{
	if (channel->detailptr->path_type != CLRPC_PATH_TYPE_USER)
	{
		clrpc_callback (channel, CLRPC_ERR_SYS_PATH, & rb, CLRPC_MODULE_ID, __LINE__);
	}
	else {
		LSA_UINT16  rc = clrpc_sv_op_unregister_internal (channel, rb);
		if (rc != CLRPC_OK)
		{
			clrpc_callback (channel, rc, & rb, CLRPC_MODULE_ID, __LINE__);
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_unregister_internal (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	)
{
	CLRPC_LOG_FCT("clrpc_sv_op_unregister_internal")
	CLRPC_SERVER_PTR_TYPE  server;
	CLRPC_SCALL_PTR_TYPE  scall;

	CLRPC_UPPER_SV_UNREGISTER_IF_OBJ_PTR_TYPE  unreg = rb->args.sv.unreg_if_obj;

	if (/*unreg->server_id < 0 ||*/ unreg->server_id >= CLRPC_CFG_MAX_SERVERS) {
		CLRPC_SV_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong server_id(%u)", unreg->server_id
			);
		return CLRPC_ERR_PARAM;
	}
	server = &clrpc_servers[unreg->server_id];
	if (server->state != CLRPC_SST_READY) {
		CLRPC_SV_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "sid(%u) wrong state(%u)", unreg->server_id, server->state
			);
		return CLRPC_ERR_SEQUENCE;
	}

	CLRPC_SV_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "sid(%u) is_endpoint_mapper(%u)"
		, unreg->server_id
		, server->is_endpoint_mapper
		);

		/* keep upper */
	if (CLRPC_PTR_IS_NOT_NULL (server->upper))
	{
		CLRPC_SV_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "sid(%u) upper(0x%x).opcode(%u) running"
			, unreg->server_id, server->upper, CLRPC_RQB_GET_OPCODE(server->upper)
			);
		return CLRPC_ERR_SEQUENCE;
	}
	server->upper = rb;

		/* !CAST! keep original upper channel */
	CLRPC_RQB_SET_PREV_RQB_PTR (rb, (CLRPC_UPPER_RQB_PTR_TYPE)((LSA_VOID *)channel));


		/* AP00820599: unregister server from pending epm-ctx */
	clrpc_epm_unregister_server (server);

		/* check all scalls completed, done in ..lower_done */
	server->do_unregister = 1;

		/* orphan scalls waiting for sending */
	scall = (CLRPC_SCALL_PTR_TYPE)clrpc_queue_dequeue (& server->send_scall_queue);
	while (CLRPC_PTR_IS_NOT_NULL (scall))
	{
		clrpc_sc_do_orphan (scall);
		clrpc_sv_scall_free (server, & scall);

		scall = (CLRPC_SCALL_PTR_TYPE)clrpc_queue_dequeue (& server->send_scall_queue);
	}

		/* orphan scall in state (CLRPC_CST_INIT, CLRPC_CST_WF_ACK) */
	{
		CLRPC_HASH_ITERATOR_TYPE  iter;

		clrpc_hash_iter_setup (& iter);
		clrpc_hash_iter_attach (& iter, & clrpc_sc_hash);
		clrpc_hash_iter_reset (& iter);

		for (scall = (CLRPC_SCALL_PTR_TYPE)clrpc_hash_iter_next (& iter);
			CLRPC_PTR_IS_NOT_NULL (scall);
			scall = (CLRPC_SCALL_PTR_TYPE)clrpc_hash_iter_next (& iter))
		{
			if (CLRPC_PTR_ARE_EQUAL (server, scall->server))
			{
                if ((scall->state == CLRPC_CST_WF_ACK) || 
                    (scall->state == CLRPC_CST_INIT) || 
                    (scall->state == CLRPC_CST_RECV) || 
                    (scall->state == CLRPC_CST_WF_WAY))
				{
					clrpc_sc_do_orphan (scall);
					clrpc_sv_scall_free (server, & scall);
				}
			}
		}
		clrpc_hash_iter_detach (& iter);
	}

	if (server->scall_cnt == 0)
	{
		LSA_UINT16  rc;
		rc = clrpc_sv_sock_close (server);
		if (rc != CLRPC_OK)
		{
			return rc;
		}
	}
	else
	{
			/* must wait for scall completion */
		CLRPC_SV_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "unregister-scall_cnt(%u), wait for end"
			, server->scall_cnt
			);
		server->do_unregister = 2;
	}
	return CLRPC_OK;
}


/*------------------------------------------------------------------------------
//	CLRPC_OPC_SV_CALL_IND_PROVIDE
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_call_ind_provide (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	)
{
	CLRPC_LOG_FCT("clrpc_sv_op_call_ind_provide")
	CLRPC_SERVER_PTR_TYPE  server;
	CLRPC_UPPER_SV_CALL_PTR_TYPE  up_call;

	if (channel->detailptr->path_type != CLRPC_PATH_TYPE_USER) {
		CLRPC_SV_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong path_type(%u)"
			, channel->detailptr->path_type
			);
		clrpc_callback (channel, CLRPC_ERR_SYS_PATH, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}

	if (CLRPC_PTR_IS_NULL (rb->args.sv.call)) {
		CLRPC_UPPER_RQB_ERROR (rb);
		return;
	}
	up_call = rb->args.sv.call;

		/* argument check */
	if (CLRPC_PTR_IS_NOT_NULL (up_call->ndr_data)) {
		CLRPC_SV_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "sid(%u) ndr_data != NULL"
			, up_call->server_id
			);
		clrpc_callback (channel, CLRPC_ERR_PARAM, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}
	if (up_call->alloc_len != 0) {
		CLRPC_SV_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "sid(%u) ndr_len(%u) != 0"
			, up_call->server_id, up_call->alloc_len
			);
		clrpc_callback (channel, CLRPC_ERR_PARAM, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}

		/* searching server */
	if (/*up_call->server_id < 0 ||*/ up_call->server_id >= CLRPC_CFG_MAX_SERVERS) {
		CLRPC_SV_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong server_id(%u)"
			, up_call->server_id
			);
		clrpc_callback (channel, CLRPC_ERR_PARAM, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}
	server = & clrpc_servers[up_call->server_id];
	if (server->state != CLRPC_SST_READY) {
		CLRPC_SV_TRACE_02 (server->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "sid(%u) wrong state(%u)"
			, up_call->server_id, server->state
			);
		clrpc_callback (channel, CLRPC_ERR_SEQUENCE, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}

	clrpc_sv_provide_enqueue (server, & rb);
	/* work done, callback as indication */
	return;
}


/*------------------------------------------------------------------------------
//	CLRPC_OPC_SV_CALL_RESPONSE
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_call_response (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	)
{
	CLRPC_LOG_FCT("clrpc_sv_op_call_response")
	CLRPC_SCALL_PTR_TYPE  scall;

	LSA_UNUSED_ARG(channel);

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (rb->args.sv.call)); /* see caller */

	CLRPC_SV_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "rsp(%u) opnum(%u) len(%u) semantics(%u)"
		, CLRPC_RQB_GET_RESPONSE (rb)
		, rb->args.sv.call->opnum
		, rb->args.sv.call->ndr_len
		, rb->args.sv.call->semantics
		);

	scall = (CLRPC_SCALL_PTR_TYPE)(rb->args.sv.call->scall_id);

		/* is it really a scall-ptr ? */
	if (! CLRPC_PTR_ARE_EQUAL (scall->self, scall)) {
		CLRPC_SV_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "RQB.args.sv.call.scall_id(%08x) != scall->self"
			, scall
			);
		CLRPC_UPPER_RQB_ERROR (rb);
		return;
	}

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (scall->server));

		/* check: is server in state ready ? */
	if (! (scall->server->state == CLRPC_SST_READY)) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "server.state(%u) != READY"
			, scall->server->state
			);
		CLRPC_UPPER_RQB_ERROR (rb);
		return;
	}

		/* pnio */
	if (scall->server->pnio_signature) {
		LSA_UINT16 err = clrpc_sv_pnio_marshal(scall, rb);
		if (err) {
			/* fault(nca_status) i.a. unsepcific z.B len < PNIO_OFFSET */
			CLRPC_RQB_SET_RESPONSE (rb, CLRPC_ERR_FAULTED);
		}
	}

	clrpc_sc_dispatch (scall, rb, LSA_NULL, 0, LSA_NULL);
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_request_lower_done() delegate to clrpc_sv_lower_done()
//----------------------------------------------------------------------------*/
static LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_lower_done (
	CLRPC_CHANNEL_PTR_TYPE  sys_channel,	/* must same as clrpc_system_channel() */
	LSA_VOID_PTR_TYPE  instance_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
	)
{
	CLRPC_LOG_FCT("clrpc_sv_lower_done")
	CLRPC_SERVER_PTR_TYPE  server = (CLRPC_SERVER_PTR_TYPE)instance_ptr; /* typecast */
	LSA_UINT32  response;
	CLRPC_UPPER_RQB_PTR_TYPE  upper;
	CLRPC_LL_UDP_DATA_PTR_TYPE  udp_recv;

	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (sys_channel));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (server));
	CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (lower));

	response = CLRPC_LL_GET_RESPONSE (lower);

	CLRPC_SV_TRACE_04 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "h(%u) rsp(%u) lower(%08x) opcode(%u)"
		, CLRPC_LL_GET_HANDLE (lower), response, lower, CLRPC_LL_GET_OPCODE (lower)
		);

	switch (CLRPC_LL_GET_OPCODE (lower)) {
/*----------------------------------------------------------------------------*/
	case SOCK_OPC_UDP_OPEN:	/* IF_REGISTER, EPM_START */
		clrpc_sv_op_register_internal_done (server, lower);
		return;

/*----------------------------------------------------------------------------*/
	case SOCK_OPC_CLOSE:	/* IF_UNREGISTER, EPM_STOP */
		{
		LSA_UINT16  rc;
		CLRPC_CHANNEL_PTR_TYPE  orig_upper_channel;

			 /* !CAST! get original upper channel */
		CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (server->upper));
		orig_upper_channel = (CLRPC_CHANNEL_PTR_TYPE) ((LSA_VOID *)CLRPC_RQB_GET_PREV_RQB_PTR (server->upper));

		if (CLRPC_PTR_IS_NULL (orig_upper_channel)) {
			CLRPC_LOWER_RQB_ERROR (lower);
			return;
		}
		/* orig_upper_channel may differ from server->channel */

		clrpc_ll_free_rqb (sys_channel, &lower);

		CLRPC_ASSERT (server->do_unregister);
		if (server->is_endpoint_mapper > 0) {
			CLRPC_ASSERT (sys_channel->detailptr->path_type == CLRPC_PATH_TYPE_SYSTEM);
			CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE(server->upper) == CLRPC_OPC_EPM_STOP);
		}
		else {
			CLRPC_ASSERT (CLRPC_RQB_GET_OPCODE(server->upper) == CLRPC_OPC_SV_UNREGISTER_IF_OBJ);
		}

		if (response != SOCK_RSP_OK) {
			CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "response(%u) != OK", response
				);
			clrpc_callback (orig_upper_channel, CLRPC_ERR_LOWER_LAYER, & server->upper, CLRPC_MODULE_ID, __LINE__);
			return;
		}

		server->sock_fd = CLRPC_LL_INVALID_SOCKET;

			/* free upper provided resources */
		upper = clrpc_sv_provide_dequeue (server);
		while (CLRPC_PTR_IS_NOT_NULL (upper)) {
			if (server->is_endpoint_mapper > 0) {
				/* see clrpc_epm_provide_and_register_obj() */
				CLRPC_FREE_UPPER_MEM (& rc, CLRPC_RQB_GET_ARGS(upper), sys_channel->sysptr);
				CLRPC_ASSERT (rc == LSA_RET_OK);
				CLRPC_FREE_UPPER_RQB (& rc, upper, sys_channel->sysptr);
				CLRPC_ASSERT (rc == LSA_RET_OK);
			}
			else {
				clrpc_callback (orig_upper_channel, CLRPC_OK_CANCELLED, & upper, CLRPC_MODULE_ID, __LINE__);
			}
			upper = clrpc_sv_provide_dequeue (server);
		}

			/* free lower send resources */
		lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& server->send_rqb_queue);
		while (CLRPC_PTR_IS_NOT_NULL (lower)) {
			clrpc_ll_free_rqb (sys_channel, & lower);
			lower = (CLRPC_LOWER_RQB_PTR_TYPE)clrpc_queue_dequeue (& server->send_rqb_queue);
		}

		CLRPC_ASSERT (server->scall_cnt == 0);
		clrpc_callback (orig_upper_channel, CLRPC_OK, & server->upper, CLRPC_MODULE_ID, __LINE__);
		server->state = CLRPC_SST_FREE;
		}
		return;

/*----------------------------------------------------------------------------*/
//	case SOCK_OPC_GET_ANY_MAC_ADDR:
//		if (response != SOCK_RSP_OK) {
//			CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_FATAL
//				, "response(%u) != OK", response
//				);
//			CLRPC_FATAL1(response);
//		}
//		clrpc_uuid_node_set (server->channel, lower->args.get_any_mac_addr.MacAddr.Adr);
//		clrpc_ll_free_rqb (sys_channel, &lower);
//		return;
//		
/*----------------------------------------------------------------------------*/
	case SOCK_OPC_UDP_SEND:
		if (response != SOCK_RSP_OK) {
			CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "response(%u) != OK"
				, response
				);
		}
		if (server->do_unregister) {
			clrpc_ll_free_rqb (sys_channel, & lower);
			return;
		}

		CLRPC_SV_TRACE_03 (server->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< server_id(%u) --- SEND.cnf --- lower(%08x)  rsp(%u)"
			, server->id, lower, response
			);

		clrpc_sv_event_send_done (server, lower);
		return;

/*----------------------------------------------------------------------------*/
	case SOCK_OPC_UDP_RECEIVE:
		if (server->do_unregister) {
			CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "sid(%u) do_unreg"
				, server->id
				);
			clrpc_ll_free_rqb (sys_channel, & lower);
			return;
		}

		CLRPC_SV_TRACE_03 (server->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "recv.ind: lower(%08x) lower.sock_fd(%u)  server.sock_fd(%u)"
			, lower, lower->args.data.sock_fd
			, server->sock_fd
			);

			/* note: handle and sock_fd to the lower-rqb, is needed
			//	by clrpc_sv_ll_recv_provide() */
		CLRPC_LL_SET_HANDLE (lower, sys_channel->handle_lower);
		udp_recv = & lower->args.data; CLRPC_ASSERT (udp_recv);
		udp_recv->sock_fd = server->sock_fd;

		if (response != SOCK_RSP_OK) {
			CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "response(%u) != OK"
				, response
				);
			clrpc_sv_ll_recv_provide (server, lower);
			return;
		}

		if (clrpc_sv_event_recv (server, lower) != CLRPC_OK) {
			clrpc_sv_ll_recv_provide (server, lower);
		}
		return;
	}
/*----------------------------------------------------------------------------*/
	CLRPC_SV_TRACE_01 (server->trace_idx, LSA_TRACE_LEVEL_FATAL
		, "unhandled opcode(%u)"
		, CLRPC_LL_GET_OPCODE (lower)
		);
	CLRPC_LOWER_RQB_ERROR (lower);
	return;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* CLRPC_CFG_MAX_SERVERS */
