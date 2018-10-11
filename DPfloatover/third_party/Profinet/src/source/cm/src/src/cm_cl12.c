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
/*  F i l e               &F: cm_cl12.c                                 :F&  */
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
/*  CM_OPC_CL_AR_PRM_BEGIN_RSP                                               */
/*  CM_OPC_CL_AR_PRM_INDATA_RSP                                              */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	612
#define CM_MODULE_ID		612

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_prm_update_alloc_submod_rec (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update
	)
{
	CM_LOG_FCT("cm_cl_prm_update_alloc_submod_rec")
	CM_AR_GRAPH_AP_PTR_TYPE  ap = LSA_NULL;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod = LSA_NULL;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = LSA_NULL;
	CM_UPPER_ARCB_RECORD_PTR_TYPE  arcb_rec;

	CmListInitialize (& prm_update->cm_internal_list);

	cm_ar_graph_find_api_slot_subslot (argr
		, prm_update->prm_submod.api, prm_update->prm_submod.slot_nr, prm_update->prm_submod.subslot_nr
		, & ap, & mod, & sub
		);
	CM_ASSERT (is_not_null (sub));

	for (CmListForeach (arcb_rec, & prm_update->prm_submod.prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
	{
		CM_AR_GRAPH_RECORD_PTR_TYPE  rec;
		LSA_UINT16  block_field;

		block_field = cm_cl_list_record_append (& prm_update->cm_internal_list
			, ap->api, mod->slot_nr, sub->subslot_nr
			, arcb_rec->record_index, arcb_rec->transfer_sequence, LSA_FALSE, LSA_FALSE, arcb_rec->record_data_len, arcb_rec->record_data
			, & rec
			);
		if (block_field != 0) /* only 'no mem' */
		{
			cm_ar_graph_free_list_local_mem (& prm_update->cm_internal_list);
			return LSA_FALSE;
		}
	}

	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_PRM_UPDATE
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_opc_prm_update (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_cl_opc_prm_update")
	CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update = rb->args.cl.prm_update;
	CM_AR_GRAPH_AP_PTR_TYPE  ap = LSA_NULL;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod = LSA_NULL;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = LSA_NULL;
	CM_AR_GRAPH_PTR_TYPE  argr;

	if (channel->state != CM_CL_READY)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PrmUpdate: wrong channel.state(%u)", channel->state);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

		/* param check */

	argr = cm_cl_ar_from_device_ar (channel, prm_update->device_nr, prm_update->ar_nr);
	if (is_null (argr))
	{
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}
	if (argr->ar.ar_type != CM_AR_TYPE_SINGLE_SYSRED)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) ar(%u) only allowed with CM_AR_TYPE_SINGLE_SYSRED"
			, argr->device_nr, argr->ar_nr
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	switch (prm_update->prm_mode)
	{
	case CM_CL_PRMMODE_AR:
		break;

	case CM_CL_PRMMODE_SUBMODULE:
		cm_ar_graph_find_api_slot_subslot (argr
			, prm_update->prm_submod.api, prm_update->prm_submod.slot_nr, prm_update->prm_submod.subslot_nr
			, & ap, & mod, & sub
			);
		if (is_null (sub))
		{
			CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PrmUpdate: dev(%u) ar(%u) submodule(%u/%u/%u) not found"
				, argr->device_nr, argr->ar_nr
				, prm_update->prm_submod.api, prm_update->prm_submod.slot_nr, prm_update->prm_submod.subslot_nr
				);
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("warning: CM_CL_PRMMODE_SUBMODULE not yet allowed")
# pragma CM_MESSAGE("warning: CM_CL_PRMMODE_SUBMODULE and transfer_seq not tested yet")
/*
//	note: must update cm_cl_acp_alarm_indication_reject()
//		reject ROSAL only if submodule is affected
//
*/
#endif /* CM_MESSAGE */
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) ar(%u) submodule(%u/%u/%u) not yet allowed !"
			, argr->device_nr, argr->ar_nr
			, prm_update->prm_submod.api, prm_update->prm_submod.slot_nr, prm_update->prm_submod.subslot_nr
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;

	default:
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "PrmUpdate: wrong prm_mode(0x%x)", prm_update->prm_mode);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (! cm_cl_prm_request_can_queue (argr, prm_update->prm_mode))
	{
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) ar(%u) a previous PrmUpdate exist, arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	if (prm_update->prm_mode == CM_CL_PRMMODE_SUBMODULE)
	{
		if (! cm_cl_prm_update_alloc_submod_rec (argr, prm_update))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PrmUpdate:  dev(%u) ar(%u) no mem for prm-record-update-list"
				, argr->device_nr, argr->ar_nr
				);
			cm_callback (channel, CM_ERR_RESOURCE, & rb);
			return;
		}
	}

	CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "PrmUpdate: dev(%u) ar(%u)  Request:  prm_mode(0x%x)  submodule(%u/%u/%u)  prm_seq(%u)"
		, argr->device_nr, argr->ar_nr, prm_update->prm_mode
		, prm_update->prm_submod.api, prm_update->prm_submod.slot_nr, prm_update->prm_submod.subslot_nr
		, cm_cl_prm_mode (argr)
		);

		/* kick state machine */
	switch (argr->cm.cl.state) {
	case CM_DST_PASSIVE:
	case CM_DST_WF_DOWN:

	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
	case CM_DST_REBIND:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:

			/* only update submodule-rec-list */
		if (prm_update->prm_mode == CM_CL_PRMMODE_SUBMODULE)
		{
			cm_ar_graph_free_list_local_mem (& sub->cl.prm_record_list);
			CmListAppend (& sub->cl.prm_record_list, & prm_update->cm_internal_list);
		}
		cm_callback (channel, CM_OK, & rb);
		break;

	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
	case CM_DST_WF_NEW_DATA:
	case CM_DST_IN_DATA:

		cm_cl_prm_request (argr, prm_update->prm_mode/*CM_CL_PRMMODE_AR or CM_CL_PRMMODE_SUBMODULE*/, rb);
		break;

	default:
		CM_FATAL1(argr->cm.cl.state);
		break;
	}
}


/*------------------------------------------------------------------------------
//	handle opcode CM_OPC_CL_AR_PRM_BEGIN_RSP
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_opc_ar_prm_begin_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_cl_opc_ar_prm_begin_rsp")
	CM_UPPER_EVENT_PTR_TYPE  upper_event = rb->args.channel.event;
	CM_AR_GRAPH_PTR_TYPE  argr;

	argr = cm_cl_ar_from_device_ar (channel, upper_event->device_nr, upper_event->ar_nr);
	if (is_null (argr))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)", upper_event->device_nr, upper_event->ar_nr);
		CM_FATAL();
		return;
	}

	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): Prm-Begin.rsp, wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	cm_cl_prm_trigger (argr, CM_CL_WF_PRM_BEGIN_RSP);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_PRM_INDATA_RSP
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_opc_ar_prm_indata_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_cl_opc_ar_prm_indata_rsp")
	CM_UPPER_EVENT_PTR_TYPE  upper_event = rb->args.channel.event;
	CM_AR_GRAPH_PTR_TYPE  argr;

	argr = cm_cl_ar_from_device_ar (channel, upper_event->device_nr, upper_event->ar_nr);
	if (is_null (argr))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)", upper_event->device_nr, upper_event->ar_nr);
		CM_FATAL();
		return;
	}

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): Prm-InData.rsp, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

		/* free event-memory */
	cm_pd_diff_list_free (channel, & upper_event->u.cl.prm_indata.diff_list);
	if (upper_event->u.cl.prm_indata.mod_diff_block_length > 0)
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.cl.prm_indata.mod_diff_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
	}

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

	cm_cl_prm_trigger (argr, CM_CL_WF_PRM_INDATA_RSP);

		/* kick state-machine */
	switch (argr->cm.cl.state) {
	case CM_DST_WF_NEW_DATA:
		if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
		{
			cm_cl_arsm (argr);
		}
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	default:
		/* only provide */
		break;
	}
}


/*------------------------------------------------------------------------------
//	handle callbacks from the prm-statemachine
//----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_start_cnf (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_cl_prm_start_cnf")

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:
		argr->cm.cl.prm.prm_keep_upd = LSA_NULL; /* cm_cl_prm_request() set to argr-pointer, to occupy prm-sm and distinguish between PRM-Connect and PRM-Update */
		return;

	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_RETURN_OF_SUB:
		return;

	case CM_CL_PRMMODE_AR:
		if (argr->cm.cl.alarm_enabled_state == CM_CL_ALARM_ENABLED_FORWARD)
		{
			cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_PBE);
		}

		CM_ASSERT (CM_RQB_GET_OPCODE(argr->cm.cl.prm.prm_keep_upd) == CM_OPC_CL_PRM_UPDATE);
		cm_callback (argr->ar_com.channel, CM_OK, & argr->cm.cl.prm.prm_keep_upd);
		return;

	case CM_CL_PRMMODE_SUBMODULE:
		if (argr->cm.cl.alarm_enabled_state == CM_CL_ALARM_ENABLED_FORWARD)
		{
			/* submodule-granular ? */
		}

		CM_ASSERT (CM_RQB_GET_OPCODE(argr->cm.cl.prm.prm_keep_upd) == CM_OPC_CL_PRM_UPDATE);
		cm_callback (argr->ar_com.channel, CM_OK, & argr->cm.cl.prm.prm_keep_upd);
		return;

	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_begin_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_cl_prm_begin_ind")

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): Prm-Begin.ind, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
		argr->cm.cl.state = CM_DST_PRM;
		return;

	case CM_CL_PRMMODE_RETURN_OF_SUB:
		argr->cm.cl.state = CM_DST_WF_APPL_READY_IND;
		return;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
		argr->cm.cl.state = CM_DST_PRM;
		cm_cl_ar_event (argr, CM_OPC_CL_AR_PRM_BEGIN_IND);
		return;

	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_end_req (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_cl_prm_end_req")

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): pre Prm-End.req, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:

			/* aktivate provider with PrmEnd.req */
		if (argr->cm.cl.state == CM_DST_PRM)
		{
			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT)
			{
				;
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3)
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE);
				cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE, ACP_PROVIDER_SET_STATION_OK);
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				;
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3)
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE);
				cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE, ACP_PROVIDER_SET_STATION_OK);
			}

				/* ApplReady.ind may recv earlier then PrmEnd.cnf, so we should accept alarm here */
			cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_DELAY);
		}
		return;

	case CM_CL_PRMMODE_PLUG:
		return;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
		return;

	case CM_CL_PRMMODE_RETURN_OF_SUB: /* not called, because CM_CL_DO_RPC_PRM_END not set */
	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_up_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_cl_prm_up_ind")

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): Prm-UP.ind (aka Prm-End.cnf), prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
		if (argr->cm.cl.state == CM_DST_PRM)
		{
			cm_cl_arsm (argr);
		}
		return;

	case CM_CL_PRMMODE_RETURN_OF_SUB:
		if (argr->cm.cl.state == CM_DST_WF_APPL_READY_IND || argr->cm.cl.state == CM_DST_WF_NEW_DATA)
		{
			cm_cl_arsm (argr);
		}
		return;

	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_idle_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_cl_prm_idle_ind")

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): IDLE,  prmmode(Idle) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_IDLE:
		/*AP01292310*/
		if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
		{
			/* argr->cm.cl.state = keep state */
			cm_cl_arsm (argr); /* continue connect */
		}
		else
		{
			argr->cm.cl.state = CM_DST_IN_DATA;
			/* cm_cl_arsm (argr); * no trigger */
		}
		return;

	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_RETURN_OF_SUB:
	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_down_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_cl_prm_down_ind")
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): Prm-DOWN.ind, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
	cm_cl_arsm_down (argr, &dinfo);
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
