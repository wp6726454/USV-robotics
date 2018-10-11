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
/*  F i l e               &F: cm_rpc.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  RPC (remote procedure call) things                                       */
/*                                                                           */
/*  - alloc RPC-RQB                                                          */
/*  - free RPC-RQB                                                           */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	10
#define CM_MODULE_ID		10

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*-----------------------------------------------------------------------------
//	get IPNIO_Controller
//---------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_get_IPNIO_Controller(
	CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR *uuid,
	LSA_UINT16      CM_COMMON_MEM_ATTR *version_major,
	LSA_UINT16      CM_COMMON_MEM_ATTR *version_minor,
	LSA_UINT16      CM_COMMON_MEM_ATTR *opnum_max,     /* may be LSA_NULL */
	LSA_UINT16      CM_COMMON_MEM_ATTR *pnio_signature /* may be LSA_NULL */
) {
	/*
	 * [
	 * uuid(DEA00002-6C97-11D1-8271-00A02442DF7D),
	 * version(1.0)
	 * ] interface IPNIO_Controller
	 */

	uuid->time_low					= 0xDEA00002;
	uuid->time_mid					= 0x6C97;
	uuid->time_hi_and_version		= 0x11D1;
	uuid->clock_seq_hi_and_reserved	= 0x82;
	uuid->clock_seq_low				= 0x71;
	uuid->node[0]					= 0x00;
	uuid->node[1]					= 0xA0;
	uuid->node[2]					= 0x24;
	uuid->node[3]					= 0x42;
	uuid->node[4]					= 0xDF;
	uuid->node[5]					= 0x7D;

	*version_major = 1;
	*version_minor = 0;

	if( is_not_null(opnum_max) ) {

		*opnum_max = CLRPC_PNIO_OPNUM_MAX;
	}

	if( is_not_null(pnio_signature) ) {

		*pnio_signature = 1;
	}
}

/*-----------------------------------------------------------------------------
//	get IPNIO_Device
//---------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_get_IPNIO_Device(
	CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR *uuid,
	LSA_UINT16      CM_COMMON_MEM_ATTR *version_major,
	LSA_UINT16      CM_COMMON_MEM_ATTR *version_minor,
	LSA_UINT16      CM_COMMON_MEM_ATTR *opnum_max,     /* may be LSA_NULL */
	LSA_UINT16      CM_COMMON_MEM_ATTR *pnio_signature /* may be LSA_NULL */
) {
	/*
	 * [
	 * uuid(DEA00001-6C97-11D1-8271-00A02442DF7D),
	 * version(1.0)
	 * ] interface IPNIO_Device
	 */

	uuid->time_low					= 0xDEA00001;
	uuid->time_mid					= 0x6C97;
	uuid->time_hi_and_version		= 0x11D1;
	uuid->clock_seq_hi_and_reserved	= 0x82;
	uuid->clock_seq_low				= 0x71;
	uuid->node[0]					= 0x00;
	uuid->node[1]					= 0xA0;
	uuid->node[2]					= 0x24;
	uuid->node[3]					= 0x42;
	uuid->node[4]					= 0xDF;
	uuid->node[5]					= 0x7D;

	*version_major = 1;
	*version_minor = 0;

	if( is_not_null(opnum_max) ) {

		*opnum_max = CLRPC_PNIO_OPNUM_MAX;
	}

	if( is_not_null(pnio_signature) ) {

		*pnio_signature = 1;
	}
}

/*-----------------------------------------------------------------------------
//	make a PNIO object uuid
//---------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_make_object_uuid(
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_UINT16 vendor_id,
	LSA_UINT16 device_id,
	LSA_UINT16 instance
) {

	/*
	 * DEA00000-6C97-11D1-8271-IIIIDDDDVVVV
	 */

	uuid->time_low                  = 0xDEA00000;
	uuid->time_mid                  = 0x6C97;
	uuid->time_hi_and_version       = 0x11D1;
	uuid->clock_seq_hi_and_reserved = 0x82;
	uuid->clock_seq_low             = 0x71;

	CM_PUT16_HTON(&uuid->node[0], 0, instance);
	CM_PUT16_HTON(&uuid->node[0], 2, device_id);
	CM_PUT16_HTON(&uuid->node[0], 4, vendor_id);
}


/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/
CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_rpc_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_rpc_channel_get")

	channel = channel->lower.channel[CM_PATH_TYPE_RPC];

	if( is_null(channel) ) {
		CM_FATAL(); /* a bug */
	}

	return channel;
}

/*-----------------------------------------------------------------------------
//	alloc lower CLRPC-RQB that fits for all requests
//---------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr_ptr
) {
	CM_LOG_FCT ("cm_rpc_alloc_rqb")
	CM_CHANNEL_PTR_TYPE  rpc_channel = cm_rpc_channel_get(channel);
	CM_RPC_LOWER_MEM_PTR_TYPE  mem;

	CM_ASSERT (is_not_null(channel));
	CM_ASSERT (is_not_null(lower_rqb_ptr_ptr));

		/* alloc the RQB */
	CM_RPC_ALLOC_LOWER_RQB (lower_rqb_ptr_ptr, cm_null_user_id, sizeof(CM_RPC_LOWER_RQB_TYPE), rpc_channel->sysptr);

	if (is_null (*lower_rqb_ptr_ptr)) {
		CM_RPC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc RPC RQB"
			, channel->my_handle
			);
		return;
	}

#if CM_DEBUG
	CM_RPC_SET_NEXT_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
	CM_RPC_SET_PREV_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif

		/* alloc the args and attach them to the RQB */
	CM_RPC_ALLOC_LOWER_MEM (
		CM_CAST_RPC_MEM_PTR_PTR (& mem),
		cm_null_user_id,
		sizeof (CLRPC_ANY_ARGS_TYPE),
		rpc_channel->sysptr
	);

	if (is_null (mem)) {
		LSA_UINT16 rc;
		CM_RPC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc RPC MEM, size(%u)"
			, channel->my_handle, sizeof (CLRPC_ANY_ARGS_TYPE)
			);
		CM_RPC_FREE_LOWER_RQB (
			& rc,
			*lower_rqb_ptr_ptr,
			rpc_channel->sysptr
			);
		CM_ASSERT (rc == LSA_RET_OK); /* don't care in release version */
		LSA_UNUSED_ARG(rc);
		*lower_rqb_ptr_ptr = LSA_NULL; /* report failure */
		return;
	}

	CM_RPC_SET_ARGS (*lower_rqb_ptr_ptr, mem);
}


/*-----------------------------------------------------------------------------
//	free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr
) {
	CM_LOG_FCT ("cm_rpc_free_rqb")
	CM_CHANNEL_PTR_TYPE  rpc_channel = cm_rpc_channel_get(channel);
	LSA_UINT16	rc;

	CM_ASSERT (is_not_null(channel));
	CM_ASSERT (is_not_null(lower_rqb_ptr));

	CM_RPC_FREE_LOWER_MEM (
		& rc,
		CM_RPC_GET_ARGS(*lower_rqb_ptr),
		rpc_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK); /* don't care in release version */

	CM_RPC_FREE_LOWER_RQB (
		& rc,
		*lower_rqb_ptr,
		rpc_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK); /* don't care in release version */

	LSA_UNUSED_ARG(rc);

	*lower_rqb_ptr = LSA_NULL; /* wipe out */
}


/*------------------------------------------------------------------------------
// alloc / free a rpc-rqb for a CLRPC_OPC_CL_OPCODE
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_alloc_call_rqb  (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  len,
	CM_RPC_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr_ptr
) {
	CM_LOG_FCT ("cm_rpc_alloc_call_rqb")
	CM_CHANNEL_PTR_TYPE  rpc_channel = cm_rpc_channel_get(channel);

	cm_rpc_alloc_rqb (channel, lower_rqb_ptr_ptr);
	if (is_null (*lower_rqb_ptr_ptr)) {
		return;
	}

	if (len == 0) {
		CM_FATAL(); /* a bug */
	}

	CM_RPC_SET_OPCODE(*lower_rqb_ptr_ptr, CLRPC_OPC_CL_CALL); /* see cm_rpc_free_call_rqb() */

	CM_RPC_ALLOC_LOWER_MEM ( /* TIA 535103: rpc-mem instead of local-mem */
			CM_CAST_RPC_MEM_PTR_PTR (& (*lower_rqb_ptr_ptr)->args.cl.call->ndr_data),
			cm_null_user_id,
			len,
			rpc_channel->sysptr
		);
	if (is_null ((*lower_rqb_ptr_ptr)->args.cl.call->ndr_data)) {
		CM_RPC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "!mem, len(%u)"
			, len
			);
		cm_rpc_free_rqb (channel, lower_rqb_ptr_ptr);
		return;
	}

	(*lower_rqb_ptr_ptr)->args.cl.call->alloc_len = len;

	return;
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_free_call_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  * lower_rqb_ptr
) {
	CM_LOG_FCT ("cm_rpc_free_call_rqb")
	CM_CHANNEL_PTR_TYPE  rpc_channel = cm_rpc_channel_get(channel);

	CM_ASSERT (CM_RPC_GET_OPCODE(*lower_rqb_ptr) == CLRPC_OPC_CL_CALL);

	if ((*lower_rqb_ptr)->args.cl.call->alloc_len == 0) {
		CM_FATAL(); /* see cm_rpc_alloc_call_rqb() */
	}

	{
	LSA_UINT16	rc;
	CM_ASSERT (is_not_null ((*lower_rqb_ptr)->args.cl.call->ndr_data));
	CM_RPC_FREE_LOWER_MEM (& rc, (*lower_rqb_ptr)->args.cl.call->ndr_data, rpc_channel->sysptr); /* TIA 535103: rpc-mem instead of local-mem */
	CM_ASSERT (rc == LSA_RET_OK); /* don't care in release version */
	LSA_UNUSED_ARG(rc);
	}

	cm_rpc_free_rqb (channel, lower_rqb_ptr);
	return;
}


/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_LOG_FCT ("cm_rpc_channel_open")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_RPC);

	CM_ASSERT (are_equal(cm_rpc_channel_get(channel), channel));

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == CM_RPC_INVALID_HANDLE);

	/***/

	cm_rpc_alloc_rqb (channel, & rpc);

	if (is_null (rpc)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_RPC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	rpc->args.channel.open->handle = CM_RPC_INVALID_HANDLE;
	rpc->args.channel.open->handle_upper = channel->my_handle;
	rpc->args.channel.open->sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	rpc->args.channel.open->clrpc_request_upper_done_ptr = cm_rpc_request_lower_done;

	CM_RPC_SET_USER_ID_HANDLE_DEV_AR(rpc, channel->my_handle, 0, 0);

	CM_RPC_SET_OPCODE (rpc, CLRPC_OPC_OPEN_CHANNEL);
	CM_RPC_SET_HANDLE (rpc, channel->lower.handle);

	CM_RPC_OPEN_CHANNEL_LOWER (rpc, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  CM_COMMON_MEM_ATTR  * rpc
)
{
	CM_LOG_FCT ("cm_rpc_channel_open_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	rc = CM_RPC_GET_RESPONSE (*rpc);

	if (rc == CLRPC_OK) {

		channel->lower.handle = (*rpc)->args.channel.open->handle;

		CM_ASSERT(channel->lower.handle != CM_RPC_INVALID_HANDLE);

		CM_RPC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) rpc-handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		rc = CM_OK;
	}
	else {

		CM_ASSERT(channel->lower.handle == CM_RPC_INVALID_HANDLE);

		CM_RPC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rpc-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_rpc_free_rqb (channel, rpc);
	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_rpc_channel_close (
	CM_CHANNEL_PTR_TYPE channel
)
{
	CM_LOG_FCT ("cm_rpc_channel_close")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_RPC);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_RPC_SET_USER_ID_HANDLE_DEV_AR(rpc, channel->my_handle, 0, 0);

	CM_RPC_SET_OPCODE (rpc, CLRPC_OPC_CLOSE_CHANNEL);
	CM_RPC_SET_HANDLE (rpc, channel->lower.handle);

	CM_RPC_CLOSE_CHANNEL_LOWER (rpc, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_rpc_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  CM_COMMON_MEM_ATTR  * rpc
) {
	CM_LOG_FCT ("cm_rpc_close_channel_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_RPC_GET_RESPONSE (*rpc);

	cm_rpc_free_rqb (channel, rpc);

	if (rc == CLRPC_OK) {

		channel->lower.handle = CM_RPC_INVALID_HANDLE;

		CM_RPC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u)"
			, channel->my_handle
			);

		rc = CM_OK;
	}
	else {

		/* don't invalidate rpc-handle, channel is not closed! */

		CM_RPC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rpc-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}


/*---------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_set_interface_param (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT ("cm_rpc_set_interface_param")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get (channel);

	CM_ASSERT (channel->state == CM_CH_INFO);
	CM_ASSERT (is_not_null (edd_channel));

	/***/

	cm_rpc_alloc_rqb (channel, & rpc);

	if (is_null (rpc)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	rpc->args.channel.set_interface_param->trace_idx = edd_channel->trace_idx;

	CM_MEMCPY (	rpc->args.channel.set_interface_param->mac_addr,
				edd_channel->lower.info.edd.MACAddress.MacAdr,
				sizeof(rpc->args.channel.set_interface_param->mac_addr));

	rpc->args.channel.set_interface_param->client_receive_resources = channel->detailptr->u.rpc.recv_resource_count;

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SET_INTERFACE_PARAM, 0, 0);
}


/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_set_interface_param_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  CM_COMMON_MEM_ATTR  * rpc
) {
	CM_LOG_FCT ("cm_rpc_set_interface_param_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_INFO);

	rc = CM_RPC_GET_RESPONSE (*rpc);

	if (rc == CLRPC_OK) {
		channel->lower.info.rpc = *(*rpc)->args.channel.set_interface_param;
		CM_RPC_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "info stored");
		rc = CM_OK;
	}
	else {
		CM_RPC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rpc-rsp(%u)"
			, channel->my_handle, rc
			);
		rc = CM_ERR_LOWER_LAYER;
	}

	cm_rpc_free_rqb (channel, rpc);
	cm_channel_done (channel, rc);
}


/*===========================================================================*/
/*=== RPC REQUEST LOWER =====================================================*/
/*===========================================================================*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_request_lower (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_OPCODE_TYPE  opcode,
	LSA_UINT16  dev_nr,
	LSA_UINT16  ar_nr
	)
{
	CM_LOG_FCT ("cm_rpc_request_lower")
	CM_CHANNEL_PTR_TYPE  rpc_channel = cm_rpc_channel_get(channel);

	CM_ASSERT(ar_nr <= 255); /* stored in a UVAR8, see CM_RPC_SET_USER_ID_HANDLE_DEV_AR() */

	CM_RPC_SET_USER_ID_HANDLE_DEV_AR (lower_rqb_ptr, channel->my_handle, dev_nr, /*better cast-here!*/ar_nr);

	CM_RPC_SET_OPCODE (lower_rqb_ptr, opcode);
	CM_RPC_SET_HANDLE (lower_rqb_ptr, rpc_channel->lower.handle);

	CM_RPC_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> rpc-rqb(0x%08x) rpc-opc(%u) f-h(%u) f-dev(%u) f-ar(%u)"
		, lower_rqb_ptr, CM_RPC_GET_OPCODE (lower_rqb_ptr)
		, CM_RPC_GET_USER_ID_HANDLE (lower_rqb_ptr), CM_RPC_GET_USER_ID_DEV (lower_rqb_ptr), CM_RPC_GET_USER_ID_AR (lower_rqb_ptr)
		);

	CM_RPC_REQUEST_LOWER (lower_rqb_ptr, rpc_channel->sysptr);
}


/*===========================================================================*/
/*=== RPC REQUEST LOWER DONE ================================================*/
/*===========================================================================*/
LSA_VOID  CM_RPC_LOWER_IN_FCT_ATTR
cm_rpc_request_lower_done (
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
) {
	CM_LOG_FCT ("cm_rpc_request_lower_done")
	CM_CHANNEL_PTR_TYPE  channel;

	if( is_null(rpc) ) {

		CM_RPC_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL,
			"is_null(rpc)"
			);

		CM_RPC_LOWER_RQB_ERROR (rpc);
		return;
	}

	channel = cm_channel_from_handle(CM_RPC_GET_USER_ID_HANDLE(rpc));

	if( is_null(channel) ) {

		CM_RPC_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"invalid handle(%u)",
			CM_RPC_GET_USER_ID_HANDLE (rpc)
			);

		CM_RPC_LOWER_RQB_ERROR (rpc);
		return;
	}

	switch (CM_RPC_GET_RESPONSE (rpc)) {
	case CLRPC_OK:
	case CLRPC_OK_CANCELLED:
		CM_RPC_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< rpc-rqb(0x%08x) rpc-opc(%u) rpc-rsp(%u)  t-h(%u) t-dev(%u) t-ar(%u)"
			, rpc, CM_RPC_GET_OPCODE (rpc), CM_RPC_GET_RESPONSE (rpc)
			, CM_RPC_GET_USER_ID_HANDLE (rpc), CM_RPC_GET_USER_ID_DEV (rpc), CM_RPC_GET_USER_ID_AR (rpc)
			);
		break;

	case CLRPC_ERR_REJECTED: /* e.g. interface not registered, prevent error log */
		CM_RPC_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< rpc-rqb(0x%08x) rpc-opc(%u) rpc-rsp(%u)  t-h(%u) t-dev(%u) t-ar(%u) rejected, maybe interface not registered"
			, rpc, CM_RPC_GET_OPCODE (rpc), CM_RPC_GET_RESPONSE (rpc)
			, CM_RPC_GET_USER_ID_HANDLE (rpc), CM_RPC_GET_USER_ID_DEV (rpc), CM_RPC_GET_USER_ID_AR (rpc)
			);
		break;

	default:
		CM_RPC_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "<<< rpc-rqb(0x%08x) rpc-opc(%u) rpc-rsp(%u)  t-h(%u) t-dev(%u) t-ar(%u)"
			, rpc, CM_RPC_GET_OPCODE (rpc), CM_RPC_GET_RESPONSE (rpc)
			, CM_RPC_GET_USER_ID_HANDLE (rpc), CM_RPC_GET_USER_ID_DEV (rpc), CM_RPC_GET_USER_ID_AR (rpc)
			);
		break;
	}

	switch (channel->detailptr->path_type) {

	case CM_PATH_TYPE_RPC:

		switch (CM_RPC_GET_OPCODE (rpc)) {

		case CLRPC_OPC_OPEN_CHANNEL:
			cm_rpc_channel_open_done (channel, & rpc);
			break;

		case CLRPC_OPC_CLOSE_CHANNEL:
			cm_rpc_channel_close_done (channel, & rpc);
			break;

		case CLRPC_OPC_SET_INTERFACE_PARAM:
			cm_rpc_set_interface_param_done (channel, & rpc);
			break;

		default:
			CM_RPC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)"
				, CM_RPC_GET_OPCODE (rpc)
				);
			CM_RPC_LOWER_RQB_ERROR (rpc); /* should not happen */
			break;
		}
		break;

	case CM_PATH_TYPE_PD:

		switch (CM_RPC_GET_OPCODE (rpc)) {

		case CLRPC_OPC_EPM_START:
			{
			LSA_UINT16 rc = CM_RPC_GET_RESPONSE (rpc);

			CM_RPC_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "rpc-rsp(%u): epm-server_id(%u) epm-udp_port(0x%x)"
				, rc, rpc->args.epm.start->server_id, CM_NTOH16 (rpc->args.epm.start->udp_port)
				);

			CM_ASSERT (channel->usr.pd.rpc.server_id == CLRPC_INVALID_SERVER_ID);

			if( rc == CLRPC_OK ) {

				channel->usr.pd.rpc.server_id = rpc->args.epm.start->server_id;
			}

			cm_rpc_free_rqb (channel, & rpc);

			cm_pd_rpc_epm_start_done(channel, rc);
			}
			break;

		case CLRPC_OPC_EPM_STOP:
			{
			LSA_UINT16 rc = CM_RPC_GET_RESPONSE (rpc);

			CM_ASSERT (channel->usr.pd.rpc.server_id != CLRPC_INVALID_SERVER_ID);

			if( rc == CLRPC_OK ) {

				channel->usr.pd.rpc.server_id = CLRPC_INVALID_SERVER_ID;

				cm_rpc_free_rqb (channel, & rpc);

				cm_pd_rpc_epm_stop_done(channel);
			}
			else {

				CM_FATAL1(rc); /* should not happen */
			}
			}
			break;

		default:
			CM_RPC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)"
				, CM_RPC_GET_OPCODE (rpc)
				);
			CM_RPC_LOWER_RQB_ERROR (rpc); /* should not happen */
			break;
		}
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_rpc_lower_done(channel, rpc);
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_rpc_lower_done(channel, rpc);
		break;
#endif

	default:
		CM_RPC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "wrong path_type(%u)"
			, channel->detailptr->path_type
			);
		CM_FATAL(); /* a bug */
		break;
	}
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
