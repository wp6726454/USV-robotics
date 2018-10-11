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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_cl13.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  opcodes for client                                                       */
/*                                                                           */
/*  CM_OPC_CL_LOGBOOK_READ                                                   */
/*  CM_OPC_CL_RECORD_READ_IMPLICIT                                           */
/*  CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY                                     */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	613
#define CM_MODULE_ID		613

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_LOGBOOK_READ
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_opc_logbook_read (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_LOG_FCT ("cm_cl_opc_logbook_read")
	CM_UPPER_CL_LOGBOOK_READ_PTR_TYPE  lbr;
	LSA_UINT16  rc;

	CM_ASSERT (is_not_null (channel));
	CM_ASSERT (is_not_null (rb));

	lbr = rb->args.cl.logbook_read;

	if (lbr->entry_count > 0  &&  is_null (lbr->entry)) {
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (! (1 <= lbr->device_nr && lbr->device_nr <= channel->usr.cl.dev_cnt)) {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%d) wrong: valid(1..%d)", lbr->device_nr, channel->usr.cl.dev_cnt
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: logbook_read")
#endif /* CM_MESSAGE */

	CM_GET_CYCLE_COUNTER (& rc, channel->sysptr, & lbr->curr_time_stamp);
	lbr->entry_count = 0;

	if (rc == LSA_RET_OK) {
		cm_callback (channel, CM_OK, & rb);
	}
	else {
		cm_callback (channel, CM_ERR_LOWER_LAYER, & rb);
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_RECORD_READ_IMPLICIT
//		- nare-resolve
//		- rpc-bind
//		- rpc-call(read_implicit)
//		- rpc-unbind
//		- CM_OPC_CL_RECORD_READ_IMPLICIT.cnf
//----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CL_IMPLICIT_READ

/*----------------------------------------------------------------------------*/
CM_CL_READ_IMPLICIT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_cl_ri_from_device_nr__nn (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  device_nr
	)
{
	CM_LOG_FCT("cm_cl_ri_from_device_nr__nn")
	CM_CL_READ_IMPLICIT_PTR_TYPE  ri;

	if (! (1 <= device_nr && device_nr <= CM_CFG_MAX_CL_IMPLICIT_READ))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) not in valid range (1..%u)"
			, device_nr, CM_CFG_MAX_CL_IMPLICIT_READ
			);
		CM_FATAL1 (device_nr);
		return LSA_NULL;
	}

	ri = & channel->usr.cl.read_impl[device_nr - 1];
	CM_ASSERT (/* CM_RIST_FREE <= ri->state && */ ri->state <= CM_RIST_UNBIND);
	return ri;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_ri_one_running (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_cl_ri_one_running")
	LSA_UINT16  i;

	for (i = 1; i <= CM_CFG_MAX_CL_IMPLICIT_READ; ++i)
	{
		CM_CL_READ_IMPLICIT_PTR_TYPE  ri = cm_cl_ri_from_device_nr__nn (channel, i);

		if (ri->state != CM_RIST_FREE)
		{
			return LSA_TRUE;
		}
	}
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_opc_record_read_implicit (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_cl_opc_record_read_implicit")
	CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE  arg;
	LSA_UINT16  i;
	LSA_UINT16  addr_mode_prop;
	LSA_UINT8  nare_mode;
	LSA_UINT32  local_ip_addr, local_subnet, local_gateway;

	CM_CL_READ_IMPLICIT_PTR_TYPE  ri;

		/* param check */
	if (channel->state != CM_CL_READY)
	{
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	arg = rb->args.cl.read_implicit;

	arg->lower_response = LSA_OK;

	arg->rec.ret_val_1 = 0;
	arg->rec.ret_val_2 = 0;
	arg->rec.cm_pnio_err = CM_PNIO_ERR_NONE;

	addr_mode_prop = CM_CAST_U16 (arg->dev_addr.properties & CM_NARE_MODE_MASK);

	switch (addr_mode_prop)
	{
#if CM_CFG_USE_NARE
	case CM_NARE_MODE_RESOLVE_DCP:
	case CM_NARE_MODE_RESOLVE_DNS:

		if (addr_mode_prop == CM_NARE_MODE_RESOLVE_DCP)
		{
			nare_mode = NARE_MODE_RESOLVE_DCP;
		}
		else
		{
			nare_mode = NARE_MODE_RESOLVE_DNS;
		}
		if (! cm_is_valid_station_name (arg->dev_addr.device_name_length, arg->dev_addr.device_name))
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "invalid device_name, device_name_length(%u), dev_addr.properties(%u)"
				, arg->dev_addr.device_name_length, arg->dev_addr.properties
				);
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}
		break;
#endif
	case CM_NARE_MODE_USE_IP:

		nare_mode = 0xFF; /* unknown mode */
		if (arg->dev_addr.device_ip_addr == 0) {
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "strange ip-addr(0x%x), dev_addr.properties(%u)"
				, arg->dev_addr.device_ip_addr, arg->dev_addr.properties
				);
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}
		break;

	default:
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (arg->rec.data_length < CM_RECORD_OFFSET || is_null (arg->rec.data))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "data_length(%u) < CM_RECORD_OFFSET(%u) || data == 0"
			, arg->rec.data_length, CM_RECORD_OFFSET
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ_IMPLICIT)
	{
		arg->rec.readquery_length = 0;
	}
	else
	{
		CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY);

		if (CM_RECORD_OFFSET + arg->rec.readquery_length > arg->rec.data_length)
		{
			CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "CM_RECORD_OFFSET(%u) + rec.readquery_length(%u) > rec.data_length(%u)"
				, CM_RECORD_OFFSET, arg->rec.readquery_length, arg->rec.data_length
				);
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}
	}

	if (! cm_oha_get_ip_suite (channel, & local_ip_addr, & local_subnet, & local_gateway))
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "have no local ip_addr");
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

		/* alloc free resource */
	ri = LSA_NULL;
	for (i = 1; i <= CM_CFG_MAX_CL_IMPLICIT_READ; ++i)
	{
		ri = cm_cl_ri_from_device_nr__nn (channel, i);
		if (ri->state == CM_RIST_FREE)
		{
				/* init ri-struct */
			ri->device_nr = i;
			ri->state = CM_RIST_NARE;
			ri->wf = 0;
			ri->upper = rb; /* keep */
			ri->rpc_client_id = CM_RPC_INVALID_CLIENT_ID;
			break;
		}
		else
		{
			ri = LSA_NULL;
		}
	}
	if (is_null (ri))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no free resource, CM_CFG_MAX_CL_IMPLICIT_READ(%u)", CM_CFG_MAX_CL_IMPLICIT_READ
			);
		cm_callback (channel, CM_ERR_RESOURCE, & rb);
		return;
	}

	CM_CL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "rec-read-impl(%u): addr-prop(%u) index(0x%x) data_length(%u) query_length(%u)"
		, ri->device_nr
		, arg->dev_addr.properties
		, arg->rec.record_index
		, arg->rec.data_length
		, arg->rec.readquery_length
		);

#if CM_CFG_USE_NARE
	if (nare_mode == NARE_MODE_RESOLVE_DCP  ||  nare_mode == NARE_MODE_RESOLVE_DNS)
	{
		LSA_BOOL ok = cm_nare_resolve (channel, local_ip_addr
			, nare_mode, LSA_TRUE
			, CM_CAST_U8(arg->dev_addr.device_name_length), arg->dev_addr.device_name
			, CM_CAST_U16(ri->device_nr | CM_CL_DEV_SUBSYS_READ_IMPL), 0, ri
			);
		if (! ok) /* only alloc-error */
		{
			CM_FATAL1 (ok);
			return;
		}
	}
	else
	{
		CM_ASSERT (addr_mode_prop == CM_NARE_MODE_USE_IP);
		cm_cl_ri_nare_resolve_done (channel, ri, LSA_NULL);
	}
#else

	cm_cl_ri_nare_resolve_done (channel, ri, LSA_NULL);
#endif
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_ri_nare_resolve_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_READ_IMPLICIT_PTR_TYPE  ri,
	LSA_VOID_PTR_TYPE /*CM_NARE_LOWER_RQB_PTR_TYPE*/  nare_alias
	)
{
	CM_LOG_FCT("cm_cl_ri_nare_resolve_done")
	CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE  arg;
	LSA_UINT16  rsp;

	CM_ASSERT (ri->state == CM_RIST_NARE);
	CM_ASSERT (is_not_null (ri->upper));

	arg = ri->upper->args.cl.read_implicit;

	if (is_not_null (nare_alias))
	{
#if CM_CFG_USE_NARE
		CM_NARE_LOWER_RQB_PTR_TYPE  nare = (CM_NARE_LOWER_RQB_PTR_TYPE)nare_alias;

		rsp = CM_NARE_GET_RESPONSE (nare);

		CM_MEMCPY (& arg->dev_addr.device_ip_addr
			, nare->args.ResConf.Res.IPAddr, sizeof (arg->dev_addr.device_ip_addr)
			);

		cm_nare_free_rqb (channel, nare);

		if (rsp == NARE_RSP_OK  &&  arg->dev_addr.device_ip_addr == 0)
		{
			rsp = NARE_RSP_ERR_LL; /* ! */
		}
		CM_ASSERT (NARE_RSP_OK == LSA_RSP_OK); /* see nare_usr.h */
#else
		CM_FATAL ();
		rsp = LSA_RSP_ERR_PARAM; /* ! */
#endif
	}
	else
	{
		CM_ASSERT ((arg->dev_addr.properties & CM_NARE_MODE_MASK) == CM_NARE_MODE_USE_IP);
		rsp = LSA_RSP_OK;
	}

	if (rsp != LSA_RSP_OK)
	{
		arg->lower_response = rsp;
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rec-read-impl(%u): rsp(%d) ip-addr(0x%x)"
			, ri->device_nr, rsp, arg->dev_addr.device_ip_addr
			);

		cm_callback (channel, CM_ERR_LOWER_NARE, & ri->upper);

		ri->state = CM_RIST_FREE;
		if (channel->state == CM_CL_UNREGISTER)
		{
			cm_cl_delete_done (channel, 0/*trigger*/);
		}
		return;
	}

	if (channel->state == CM_CL_UNREGISTER)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "rec-read-impl(%u): cancelled because of cm_cl_delete()"
			, ri->device_nr
			);
		cm_callback (channel, CM_OK_CANCELLED, & ri->upper);

		ri->state = CM_RIST_FREE;
		cm_cl_delete_done (channel, 0/*trigger*/);
		return;
	}

	ri->state = CM_RIST_BIND;
	{
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc))
	{
		cm_callback (channel, CM_ERR_ALLOC_LOWER, & ri->upper);
		ri->state = CM_RIST_FREE;
		/* if (channel->state == CM_CL_UNREGISTER) ... done above */
		return;
	}

	ri->rpc_client_id					= CM_RPC_INVALID_CLIENT_ID;

	rpc->args.cl.bind->client_id		= CM_RPC_INVALID_CLIENT_ID; /* out */

	rpc->args.cl.bind->host_ip			= arg->dev_addr.device_ip_addr;
	rpc->args.cl.bind->udp_port			= 0;

	cm_rpc_get_IPNIO_Device(
		& rpc->args.cl.bind->if_uuid,
		& rpc->args.cl.bind->if_version_major,
		& rpc->args.cl.bind->if_version_minor,
		LSA_NULL, LSA_NULL
		);

	cm_rpc_make_object_uuid (& rpc->args.cl.bind->object_uuid,
			arg->dev_rpc.vendor_id, arg->dev_rpc.device_id, arg->dev_rpc.instance
		);

	CM_CL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rec-read-impl(%u): bind to ip-addr(%u), obj_uuid.vendor_id(%u) .device_id(%u) .instance(%u)"
		, ri->device_nr, arg->dev_addr.device_ip_addr
		, arg->dev_rpc.vendor_id, arg->dev_rpc.device_id, arg->dev_rpc.instance
		);

	cm_rpc_request_lower (channel
		, rpc, CLRPC_OPC_CL_BIND, CM_CAST_U16(ri->device_nr | CM_CL_DEV_SUBSYS_READ_IMPL), 0
		);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_ri_rpc_bind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_READ_IMPLICIT_PTR_TYPE  ri,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT("cm_cl_ri_rpc_bind_done")
	CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE  arg;
	LSA_UINT16  appl_timeout1sec;

	CM_ASSERT (ri->state == CM_RIST_BIND);
	CM_ASSERT (is_not_null (ri->upper));

	arg = ri->upper->args.cl.read_implicit;

	if (CM_RPC_GET_RESPONSE (rpc) != CLRPC_OK)
	{
		arg->lower_response = CM_RPC_GET_RESPONSE (rpc);
		cm_rpc_free_rqb (channel, & rpc);

		cm_callback (channel, CM_ERR_LOWER_RPC, & ri->upper);

		ri->state = CM_RIST_FREE;
		if (channel->state == CM_CL_UNREGISTER)
		{
			cm_cl_delete_done (channel, 0/*trigger*/);
		}
		return;
	}

	ri->rpc_client_id = rpc->args.cl.bind->client_id;
	CM_ASSERT (ri->rpc_client_id != CM_RPC_INVALID_CLIENT_ID);

	appl_timeout1sec = arg->dev_rpc.appl_timeout1sec;
	if (appl_timeout1sec == 0)
	{
		appl_timeout1sec = channel->usr.cl.appl_timeout1sec;
	}

	/* reuse rpc */

	ri->state = CM_RIST_CALL;
	{
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_INT  dp_left;

	rpc->args.cl.call->client_id		= ri->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_READ_IMPLICIT;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= appl_timeout1sec;

	rpc->args.cl.call->alloc_len		= arg->rec.data_length;
	rpc->args.cl.call->ndr_len			= CM_RECORD_OFFSET + arg->rec.readquery_length;
	rpc->args.cl.call->ndr_data			= arg->rec.data; /* zero copy */


		/* marshal args to PNIO-net-format */
	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;

	dp_left = CM_RECORD_OFFSET - CLRPC_PNIO_OFFSET;

	cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_READ_REQ);
	cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_BLK_READ_WRITE)));
	cm_put_BLOCK_HEADER_version	(0x0100);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

	cm_put_REQ_READ_seq_number (0); /* const */
	cm_put_REQ_READ_ar_uuid (& cm_uuid_nil_uuid);
	cm_put_REQ_READ_api (arg->rec.api);
	cm_put_REQ_READ_slot_nr (arg->rec.slot_nr);
	cm_put_REQ_READ_subslot_nr (arg->rec.subslot_nr);
	cm_put_REQ_READ_padding_1;
	cm_put_REQ_READ_index (arg->rec.record_index);
	cm_put_REQ_READ_record_data_length (arg->rec.data_length - CM_RECORD_OFFSET);

	cm_put_REQ_READ_IMPL_target_ar_uuid (& arg->target_uuid);
	cm_put_REQ_READ_IMPL_padding_2;

#ifdef CM_DEBUG
	cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));
	CM_ASSERT (dp_left == 0);
#endif
	LSA_UNUSED_ARG (dp); /* lint */
	LSA_UNUSED_ARG (dp_left); /* lint */

	CM_CL_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rec-read-impl(%u): rpc-client(%08x) opnum(%u) submodule(%u/%u/%u) rec_index(0x%x)"
		, ri->device_nr, rpc->args.cl.call->client_id, rpc->args.cl.call->opnum
		, arg->rec.api, arg->rec.slot_nr, arg->rec.subslot_nr, arg->rec.record_index
		);
	CM_CL_TRACE_12 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rec-read-impl(%u): target_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ri->device_nr
		, arg->target_uuid.time_low, arg->target_uuid.time_mid, arg->target_uuid.time_hi_and_version
		, arg->target_uuid.clock_seq_hi_and_reserved, arg->target_uuid.clock_seq_low
		, arg->target_uuid.node[0], arg->target_uuid.node[1], arg->target_uuid.node[2]
		, arg->target_uuid.node[3], arg->target_uuid.node[4], arg->target_uuid.node[5]
		);

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);
	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(ri->device_nr | CM_CL_DEV_SUBSYS_READ_IMPL), 0
		);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_ri_rpc_call_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_READ_IMPLICIT_PTR_TYPE  ri,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT("cm_cl_ri_rpc_call_done")
	CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE  arg;
	LSA_UINT16  error_cause;
	LSA_UINT32  nca_status;

	CM_ASSERT (ri->state == CM_RIST_CALL);
	CM_ASSERT (is_not_null (ri->upper));

	arg = ri->upper->args.cl.read_implicit;

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_READ
		, & arg->lower_response, & error_cause, & arg->rec.cm_pnio_err, & nca_status
		);

		/* bad rpc ? */
	if (arg->lower_response != CLRPC_OK)
	{
		ri->state = CM_RIST_UNDO_BIND;
	}

		/* enough data ? */
	else if (rpc->args.cl.call->ndr_len < CM_RECORD_OFFSET)
	{
		arg->lower_response = CLRPC_ERR_ALLOC_LOWER;
		arg->rec.cm_pnio_err = CM_PNIO_FAULTY_RTA (CM_AR_REASON_PROT);
		ri->state = CM_RIST_UNDO_BIND;
	}

		/* parse returned data */
	else
	{
		CM_RPC_MEM_U8_PTR_TYPE  dp;
		LSA_INT  dp_left;
		LSA_UINT16  block_type, block_length, block_version;
		CLRPC_UUID_TYPE  ar_uuid;
		LSA_UINT16  slot_nr, subslot_nr, index, seq_number;
		LSA_UINT32  api, record_data_length;
		LSA_UINT16  padding_1;

		dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
		dp_left = rpc->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

		cm_get_BLOCK_HEADER_type (& block_type);
		cm_get_BLOCK_HEADER_length (& block_length);
		cm_get_BLOCK_HEADER_version (& block_version);

		cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

		cm_get_RSP_READ_seq_number (& seq_number);
		cm_get_RSP_READ_ar_uuid (& ar_uuid);
		cm_get_RSP_READ_api (& api);
		cm_get_RSP_READ_slot_nr (& slot_nr);
		cm_get_RSP_READ_subslot_nr (& subslot_nr);
		cm_get_RSP_READ_padding_1 (& padding_1);
		cm_get_RSP_READ_index (& index);
		cm_get_RSP_READ_record_data_length (& record_data_length);

		cm_get_RSP_READ_additional_value_1 (& arg->rec.ret_val_1);
		cm_get_RSP_READ_additional_value_2 (& arg->rec.ret_val_2);

		cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));
		LSA_UNUSED_ARG (dp); /* lint */

		if (dp_left < 0
		||	block_type != CM_BLOCK_TYPE_READ_RSP
		||	block_length != 60
		||	block_version != 0x0100
		||	seq_number != 0
		||	! cm_uuid_equal (& ar_uuid, & cm_uuid_nil_uuid)
		||	api != arg->rec.api
		||	slot_nr != arg->rec.slot_nr
		||	subslot_nr != arg->rec.subslot_nr
		||	padding_1 != 0
		||	index != arg->rec.record_index
		||	record_data_length + CM_RECORD_OFFSET < CM_RECORD_OFFSET /* wrap around check; does this work with int == u64 ? */
		||	record_data_length + CM_RECORD_OFFSET > rpc->args.cl.call->ndr_len
		)
		{
			CM_CL_TRACE_12 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "rec-read-impl(%u): parse error: block.type(0x%x).version(0x%x) slot(%d) subslot(%d) index(%d) sequence(%d) length(%d) rv1(%d) rv2(%d) ndr_len(%d) padding_1(%d)"
				, ri->device_nr, block_type, block_version
				, slot_nr, subslot_nr, index
				, seq_number, record_data_length, arg->rec.ret_val_1, arg->rec.ret_val_2
				, rpc->args.cl.call->ndr_len, padding_1
				);
			arg->rec.cm_pnio_err = CM_PNIO_FAULTY_RTA (CM_AR_REASON_PROT);
			ri->state = CM_RIST_UNDO_BIND;
		}
		else
		{
				/* good, everything goes ok */
			arg->rec.data_length = CM_RECORD_OFFSET + record_data_length;

			ri->state = CM_RIST_UNBIND;
		}
	}

	/* reuse rpc */

	{
	rpc->args.cl.unbind->client_id = ri->rpc_client_id;

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rec-read-impl(%u): state(%u) unbind.client_id(%08x)"
		, ri->device_nr, ri->state, ri->rpc_client_id
		);
	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_UNBIND, CM_CAST_U16(ri->device_nr | CM_CL_DEV_SUBSYS_READ_IMPL), 0
		);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_ri_rpc_unbind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_READ_IMPLICIT_PTR_TYPE  ri,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT("cm_cl_ri_rpc_unbind_done")

	CM_ASSERT (ri->state == CM_RIST_UNBIND || ri->state == CM_RIST_UNDO_BIND);
	CM_ASSERT (is_not_null (ri->upper));

	if (CM_RPC_GET_RESPONSE (rpc) != CLRPC_OK)
	{
		CM_FATAL1 (CM_RPC_GET_RESPONSE (rpc));
		return;
	}

	cm_rpc_free_rqb (channel, & rpc);

	CM_ASSERT (CM_RQB_GET_OPCODE (ri->upper) == CM_OPC_CL_RECORD_READ_IMPLICIT
		|| CM_RQB_GET_OPCODE (ri->upper) == CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY
		);

	switch (ri->state)
	{
	case CM_RIST_UNBIND:
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "rec-read-impl(%u): good.  rec.data_length(%u)"
			, ri->device_nr, ri->upper->args.cl.read_implicit->rec.data_length
			);
		cm_callback (channel, CM_OK, & ri->upper);
		break;

	case CM_RIST_UNDO_BIND:
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "rec-read-impl(%u): bad.  lower_rsp(%u) pnio_status(0x%x)"
			, ri->device_nr
			, ri->upper->args.cl.read_implicit->lower_response
			, ri->upper->args.cl.read_implicit->rec.cm_pnio_err
			);
		cm_callback (channel, CM_ERR_LOWER_RPC, & ri->upper);
		break;

	default:
		CM_FATAL1 (ri->state);
		break;
	}

	ri->state = CM_RIST_FREE;
	if (channel->state == CM_CL_UNREGISTER)
	{
		cm_cl_delete_done (channel, 0/*trigger*/);
	}
}


#else /*----------------------------------------------------------------------*/

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("no CM_CFG_MAX_CL_IMPLICIT_READ resources defined => CM_ERR_RESOURCE")
#endif /* CM_MESSAGE */

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_opc_record_read_implicit (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	cm_callback (channel, CM_ERR_RESOURCE, & rb);
}

#endif


/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
