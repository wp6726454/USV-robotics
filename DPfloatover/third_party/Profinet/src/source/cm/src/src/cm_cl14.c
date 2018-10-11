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
/*  F i l e               &F: cm_cl14.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	614
#define CM_MODULE_ID		614

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_RECORD_READ
// handle opcode CM_OPC_CL_RECORD_READ_QUERY
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_opc_record_read (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_LOG_FCT("cm_cl_opc_record_read")
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_UPPER_CL_RECORD_READ_PTR_TYPE  read;

	CM_ASSERT (is_not_null (channel));
	CM_ASSERT (is_not_null (rb));
	CM_ASSERT (is_not_null (rb->args.void_ptr));

	CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ
		|| CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ_QUERY
		);

	read = rb->args.cl.read;
	read->rec.cm_pnio_err = CM_PNIO_ERR_NONE;
	read->rec.ret_val_1 = 0;
	read->rec.ret_val_2 = 0;
	
	argr = cm_cl_ar_from_device_ar (channel, read->device_nr, read->ar_nr);
	if (is_null (argr)) {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)", read->device_nr, read->ar_nr);
		return CM_ERR_PARAM;
	}

	if (read->rec.record_index == CM_RECORD_INDEX_RPC_TRIGGER) /*AP01304789*/
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_index(0x%x) not allowed", read->rec.record_index);
		read->rec.cm_pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10);
		cm_callback (channel, CM_OK, & rb);
		return CM_OK; /* means, must callback(rb) myself */
	}

	if (read->rec.data_length < CM_RECORD_OFFSET) {
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "!data_length(%d)", read->rec.data_length);
		return CM_ERR_PARAM;
	}
	if (is_null (read->rec.data)) {
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "!data-pointer with data_length(%d)", read->rec.data_length);
		return CM_ERR_PARAM;
	}


	if (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ) {
		read->rec.readquery_length = 0;
	}
	else {
		CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ_QUERY);

		if (CM_RECORD_OFFSET + read->rec.readquery_length > read->rec.data_length) {
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "CM_RECORD_OFFSET(%u) + rec.readquery_length(%u) > rec.data_length(%u)"
				, CM_RECORD_OFFSET, read->rec.readquery_length, read->rec.data_length
				);
			return CM_ERR_PARAM;
		}
	}

	/*
	//	note: implicit-read allows all slot/subslot, so with explicit-read we also want to do
	//		=> no ap/slot/subslot - check
	*/

	if (argr->session_key != read->session_key) {
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "session_key(%d) != dev.session_key(%d): dev(%d) ar(%d)",
				read->session_key, argr->session_key, read->device_nr, read->ar_nr
			);
		return CM_ERR_SESSION;
	}

	{
	LSA_UINT16  rc;

	switch (argr->cm.cl.usr_state) {
	case CM_CL_AR_INDATA:
		rc = CM_OK;
		break;

	case CM_CL_AR_WF_IN:
		switch (argr->cm.cl.state) {
		case CM_DST_WF_SCHEDULE:
		case CM_DST_NARE:
		case CM_DST_REBIND:
		case CM_DST_CONNECT:
		case CM_DST_COM_START:
			rc = CM_ERR_ABORT;
			break;

		case CM_DST_COM_ACTIVATE:
		case CM_DST_PRM:
		case CM_DST_WF_APPL_READY_IND:
		case CM_DST_WF_NEW_DATA:
		case CM_DST_IN_DATA:
			/* queue it and execute it as early as possible */
			rc = CM_OK;
			break;

		case CM_DST_WF_DOWN:
			rc = CM_ERR_SESSION;
			break;

		case CM_DST_PASSIVE:
		default:
			rc = CM_ERR_SEQUENCE;
			break;
		}
		break;

	case CM_CL_AR_WF_DOWN:
		rc = CM_ERR_SESSION;
		break;

	case CM_CL_AR_PASSIVE:
	default:
		rc = CM_ERR_SEQUENCE;
		break;
	}

	switch (rc) {
	case CM_OK:
		break;

	case CM_ERR_ABORT:
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u) currently not readable, state(%u/%u)"
			, read->device_nr, read->ar_nr
			, argr->cm.cl.state, argr->cm.cl.usr_state
			);
		return rc;

	case CM_ERR_SEQUENCE:
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u) not IN_DATA or wrong WF_IN_DATA, state(%u/%u)"
			, read->device_nr, read->ar_nr
			, argr->cm.cl.state, argr->cm.cl.usr_state
			);
		return rc;

	case CM_ERR_SESSION:
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u) session aborted, state(%u/%u)"
			, read->device_nr, read->ar_nr
			, argr->cm.cl.state, argr->cm.cl.usr_state
			);
		return rc;

	default:
		CM_FATAL1(rc);
		return rc;
	}
	}

	CM_ASSERT (argr->cm.cl.dev->rpc_client_id != CM_RPC_INVALID_CLIENT_ID);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%u) ar(%u): record-read: index(0x%x) data_length(%u) query_length(%u)"
		, read->device_nr, read->ar_nr
		, read->rec.record_index
		, read->rec.data_length
		, read->rec.readquery_length
		);

	CmListInsertTail (& argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)rb);

	cm_cl_prm_trigger (argr, 0);

	return CM_OK;
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_RECORD_WRITE
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_opc_record_write (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_LOG_FCT("cm_cl_opc_record_write")
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE  write;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	CM_ASSERT (is_not_null (channel));
	CM_ASSERT (is_not_null (rb));
	CM_ASSERT (is_not_null (rb->args.void_ptr));

	write = rb->args.cl.write;
	write->rec.cm_pnio_err = CM_PNIO_ERR_NONE;
	write->rec.ret_val_1 = 0;
	write->rec.ret_val_2 = 0;

	argr = cm_cl_ar_from_device_ar (channel, write->device_nr, write->ar_nr);
	if (is_null (argr)) {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) ar(%u) not found", write->device_nr, write->ar_nr);
		return CM_ERR_PARAM;
	}

	cm_ar_graph_find_api_slot_subslot (argr, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr, LSA_NULL, LSA_NULL, & sub);
	if (is_null (sub)) {
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) ar(%u):  !found: api(%d) slot(%d) subslot(%d)",
				write->device_nr, write->ar_nr,
				write->rec.api, write->rec.slot_nr, write->rec.subslot_nr
			);
		return CM_ERR_PARAM;
	}

	if (argr->session_key != write->session_key) {
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) ar(%u): session_key(%d) != ar.session_key(%d)",
				write->device_nr, write->ar_nr, write->session_key, argr->session_key
			);
		return CM_ERR_SESSION;
	}

	if (! (argr->cm.cl.state == CM_DST_IN_DATA  ||  argr->cm.cl.state == CM_DST_WF_NEW_DATA/*v2.3*/)) {
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) ar(%u): !IN_DATA, state(%d) usr_state(%d)",
				write->device_nr, write->ar_nr, argr->cm.cl.state, argr->cm.cl.usr_state
			);
		return CM_ERR_SEQUENCE;
	}

	if (write->rec.record_index == 0xE040)
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_index(0xE040) not allowed");
		write->rec.data_length = CM_RECORD_OFFSET;
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10);
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (write->rec.cm_pnio_err);
		cm_callback (channel, CM_OK, &rb);
		return CM_OK/*!*/;
	}

	if (write->rec.data_length > argr->cm.cl.dev->dev_rpc.max_supported_record_length)
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u):  data_length(%u) > max_supported_record_length(%u)"
			, write->device_nr, write->ar_nr
			, write->rec.data_length, argr->cm.cl.dev->dev_rpc.max_supported_record_length
			);
		write->rec.data_length = CM_RECORD_OFFSET;
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record__WriteLengthError(0);
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (write->rec.cm_pnio_err);
		cm_callback (channel, CM_OK, &rb);
		return CM_OK/*!*/;
	}

	CM_ASSERT (argr->cm.cl.dev->rpc_client_id != CM_RPC_INVALID_CLIENT_ID);

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): record-write: index(0x%x)"
		, write->device_nr, write->ar_nr, write->rec.record_index
		);

	CmListInsertTail (& argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)rb);

	cm_cl_prm_trigger (argr, 0);

	return CM_OK;
}


/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
