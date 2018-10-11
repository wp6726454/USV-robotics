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
/*  F i l e               &F: cm_cl32.c                                 :F&  */
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
/*  CM_OPC_CL_PRM_UPDATE                                                     */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	632
#define CM_MODULE_ID		632

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*------------------------------------------------------------------------------
/-----------------------------------------------------------------------------*/

#define cm_put_BLK_CTRL_reserved1(u16_)				cm_put_u16  (dp, CM_BLK_CTRL, reserved1, (u16_))
#define cm_put_BLK_CTRL_ar_uuid(uuidp_)				cm_put_uuid (dp, CM_BLK_CTRL, ar_uuid, (uuidp_))
#define cm_put_BLK_CTRL_session_key(u16_)			cm_put_u16  (dp, CM_BLK_CTRL, session_key, (u16_))
#define cm_put_BLK_CTRL_alarm_sequence_number(u16_)	cm_put_u16  (dp, CM_BLK_CTRL, reserved2_alarm_sequence_number, (u16_))
#define cm_put_BLK_CTRL_control_command(u16_)		cm_put_u16  (dp, CM_BLK_CTRL, control_command, (u16_))
#define cm_put_BLK_CTRL_control_properties(u16_)	cm_put_u16  (dp, CM_BLK_CTRL, control_properties, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_put_SUBMODULE_LIST_entries(u16_)         cm_put_u16  (dp, CM_BLK_SUBMODULE_LIST, number_of_entries, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_put_SUBMODULE_LIST_ENTRY_api(u32_)        cm_put_u32  (dp, CM_BLK_SUBMODULE_LIST_ENTRY, api, (u32_))
#define cm_put_SUBMODULE_LIST_ENTRY_slot_nr(u16_)    cm_put_u16  (dp, CM_BLK_SUBMODULE_LIST_ENTRY, slot_nr, (u16_))
#define cm_put_SUBMODULE_LIST_ENTRY_subslot_nr(u16_) cm_put_u16  (dp, CM_BLK_SUBMODULE_LIST_ENTRY, subslot_nr, (u16_))

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_prm_begin (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_rpc_prm_begin")
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc = LSA_NULL;
	LSA_UINT16  alloc_len;
	LSA_UINT16  cm_put_assert_line = 0;
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
	LSA_UINT16  sublist_cnt;

	CmListLength (& argr->cm.cl.prm.prm_submod_list, & sublist_cnt);

		/* req: (ControlBlockConnect, SubmoduleListBlock) */
	alloc_len = CLRPC_PNIO_OFFSET
		+ sizeof(CM_BLK_HEADER) + sizeof(CM_BLK_CTRL)
		+ sizeof (CM_BLK_HEADER) + sizeof (CM_BLK_SUBMODULE_LIST) + sublist_cnt * sizeof (CM_BLK_SUBMODULE_LIST_ENTRY)
		;

	{	/* cnf: (ControlBlockConnect, [ModuleDiffBlock]) */
		LSA_UINT16  alloc_cnf = CLRPC_PNIO_OFFSET
			+ sizeof(CM_BLK_HEADER) + sizeof(CM_BLK_CTRL)
			/* worst case (each submodule in another API and module) */
			+ sizeof(CM_BLK_HEADER) + sizeof(CM_RSP_DIFF) + sublist_cnt * (sizeof(CM_RSP_API) + sizeof(CM_RSP_MOD) + sizeof(CM_RSP_SUB))
			;
		if (alloc_cnf > alloc_len)
		{
			alloc_len = alloc_cnf;
		}
	}

	cm_rpc_alloc_call_rqb (channel, alloc_len, & rpc);
	if (is_null (rpc))
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "dev(%u) ar(%u): no mem, do down, prm_mode(%u)", argr->device_nr, argr->ar_nr, cm_cl_prm_mode (argr));
		cm_cl_prm_abort (argr);
		return;
	}

	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_CONTROL;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= channel->usr.cl.appl_timeout1sec;

	rpc->args.cl.call->ndr_len			= 0;
	rpc->args.cl.call->ndr_data[0]		= 0;

		/* marshal */
	{
		CM_RPC_MEM_U8_PTR_TYPE  dp;
		LSA_UINT32  left;

		dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
		left = alloc_len - CLRPC_PNIO_OFFSET;

		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_PRM_BEGIN_REQ);
		cm_put_BLOCK_HEADER_length	(sizeof(LSA_UINT16) + sizeof (CM_BLK_CTRL));
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));


		cm_put_assert (left >= sizeof (CM_BLK_CTRL));

		cm_put_BLK_CTRL_reserved1 (0);
		cm_put_BLK_CTRL_ar_uuid (& argr->ar.ar_uuid);
		cm_put_BLK_CTRL_session_key (argr->cm.cl.connect_counter);
		cm_put_BLK_CTRL_alarm_sequence_number (0);
		cm_put_BLK_CTRL_control_command (CM_CONTROL_COMMAND_PRM_BEGIN);
		cm_put_BLK_CTRL_control_properties (0);	/* fixed data */

		cm_adjust (dp, left, sizeof (CM_BLK_CTRL));

		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_SUBMODULE_LIST_REQ);
		cm_put_BLOCK_HEADER_length	(sizeof(LSA_UINT16) + sizeof (CM_BLK_SUBMODULE_LIST) + sublist_cnt * sizeof (CM_BLK_SUBMODULE_LIST_ENTRY));
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

		cm_put_assert (left >= sizeof (CM_BLK_SUBMODULE_LIST));

		cm_put_SUBMODULE_LIST_entries (sublist_cnt);

		cm_adjust (dp, left, sizeof (CM_BLK_SUBMODULE_LIST));

		if (sublist_cnt > 0)
		{
			CM_LIST_ENTRY_PTR_TYPE  inner_sub;
			LSA_UINT32  api;
			LSA_UINT16  slot_nr;
			LSA_UINT16  subslot_nr;

			for (CmListForeach (inner_sub, & argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
			{
				sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

				api        = sub->mod->ap->api;
				slot_nr    = sub->mod->slot_nr;
				subslot_nr = sub->subslot_nr;
				{
					cm_put_assert (left >= sizeof (CM_BLK_SUBMODULE_LIST_ENTRY));

					cm_put_SUBMODULE_LIST_ENTRY_api        (api);
					cm_put_SUBMODULE_LIST_ENTRY_slot_nr    (slot_nr);
					cm_put_SUBMODULE_LIST_ENTRY_subslot_nr (subslot_nr);

					cm_adjust (dp, left, sizeof (CM_BLK_SUBMODULE_LIST_ENTRY));
				}
			}
		}

		/* left > 0: space for cnf */

		rpc->args.cl.call->ndr_len = CM_CAST_U16(dp - rpc->args.cl.call->ndr_data);
	}

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
	return;

cm_put_assert_failed:

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_put_assert_line
		);
	LSA_UNUSED_ARG (cm_put_assert_line);

	cm_rpc_free_call_rqb (channel, & rpc);

	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_prm_end (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_rpc_prm_end")
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;
	LSA_UINT16  prm_seq = cm_cl_prm_mode (argr);

	switch (prm_seq)
	{
	case CM_CL_PRMMODE_CONNECT:
		cm_cl_rpc_control (channel, argr, LSA_FALSE, 0);
		break;

	case CM_CL_PRMMODE_PLUG:
		{
		LSA_UINT16  alarm_seq_nr = argr->cm.cl.prm_alarm->args.channel.alarm->alarm_sequence;

		cm_cl_rpc_control (channel, argr, LSA_TRUE, alarm_seq_nr);
		}
		break;

	case CM_CL_PRMMODE_AR:
		cm_cl_rpc_control (channel, argr, LSA_FALSE, 0);
		break;

	case CM_CL_PRMMODE_SUBMODULE:
		cm_cl_rpc_control (channel, argr, LSA_FALSE, 0);
		break;

	case CM_CL_PRMMODE_RETURN_OF_SUB:
	default:
		CM_FATAL();
		break;
	}
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
