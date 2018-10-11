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
/*  F i l e               &F: cm_cl21.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for acp                                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	621
#define CM_MODULE_ID		621

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

#define CM_CL_ALPMI_INIT			0x00
#define CM_CL_ALPMI_SEND_DONE		0x01
#define CM_CL_ALPMI_ACK_DONE		0x02


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_acp_global_provider_control (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  apdu_status
)
{
	CM_LOG_FCT ("cm_cl_acp_global_provider_control")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	cm_acp_alloc_rqb (channel, & acp);
	if (is_null (acp)) {
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!acp-rqb");
		return CM_ERR_ALLOC_LOWER;
	}

	acp->args->global_prov.control.mode = 0;

	if ((apdu_status & CM_CL_CONTROL_APDU_STATUS_STOP)) {
		acp->args->global_prov.control.mode |= ACP_GLOBAL_PROVIDER_SET_STOP;
	}
	else if ((apdu_status & CM_CL_CONTROL_APDU_STATUS_RUN)) {
		acp->args->global_prov.control.mode |= ACP_GLOBAL_PROVIDER_SET_RUN;
	}

	cm_acp_request_lower (channel, ACP_OPC_GLOBAL_PROVIDER_CONTROL, LSA_NULL/*user-id*/, acp);
	return CM_OK;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_acp_global_provider_control_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_cl_acp_global_provider_control_done")
	LSA_UINT16  rc;

	rc = CM_ACP_GET_RESPONSE (acp);
	CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "state(%d) rsp(%d)"
		, channel->state
		, rc
		);
	cm_acp_free_rqb (channel, & acp);

	if (channel->state == CM_CL_GLOBAL_CTRL)
	{
		if (rc == ACP_OK)
		{
			/*
			 * transitions CM-CL::GLOBAL_CTRL --> CM-CL::REG_IFC
			 */
			channel->state = CM_CL_REG_IFC;
			cm_cl_rpc_register_if_obj (channel);
			/* => cm_cl_rpc_register_if_obj_done() */
		}
		else
		{
			/*
			 * transition CM-CL::GLOBAL_CTRL --> CM-CH::READY
			 */
			channel->state = CM_CH_READY;
			cm_cl_client_free (channel);
			cm_callback (channel, CM_ERR_LOWER_LAYER, & channel->usr.cl.upper);
		}
	}
	else
	{
		CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.cl.upper_apdu) == CM_OPC_CL_CONTROL);
		cm_callback (channel,
				CM_CAST_U16 ((rc == ACP_OK) ? CM_OK : CM_ERR_LOWER_LAYER),
				& channel->usr.cl.upper_apdu
			);

		if (is_not_null (channel->usr.cl.upper))
		{
			cm_cl_delete_done (channel, 0/*event*/);
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_provide (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_alarm_provide")
	LSA_UINT16  rc;

	if (is_not_null (upper->args.channel.alarm->alarm_data))
	{
			/* must free alloced buffer from alarm_ind, see  */
		CM_ACP_FREE_ALARM_MEM (& rc, upper->args.channel.alarm->alarm_data, argr->ar_com.channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		upper->args.channel.alarm->alarm_data = LSA_NULL;
		upper->args.channel.alarm->alarm_data_length = 0;
	}
	CM_RQB_SET_OPCODE (upper, CM_OPC_CL_DEVICE_PROVIDE_ALARM);
	cm_cl_dev_alarm_provide (argr->cm.cl.dev, upper);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_set_enabled_state (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  enabled_state
	)
{
	CM_LOG_FCT ("cm_cl_alarm_set_enabled_state")
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;

	switch (enabled_state) {
	case CM_CL_ALARM_ENABLED_IGNORE:
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): alarm_enabled_state(IGNORE)", argr->device_nr, argr->ar_nr
			);
		argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_IGNORE;
		break;

	case CM_CL_ALARM_ENABLED_DELAY:
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) ar(%u): alarm_enabled_state(DELAY)", argr->device_nr, argr->ar_nr
			);
		argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_DELAY;
		break;

	case CM_CL_ALARM_ENABLED_FORWARD:
		if (argr->cm.cl.alarm_enabled_state != CM_CL_ALARM_ENABLED_FORWARD)
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) ar(%u): alarm_enabled_state(FORWARD), was(%u)", argr->device_nr, argr->ar_nr, argr->cm.cl.alarm_enabled_state
				);

			argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_FORWARD;

				/* check queued alarms */
			cm_cl_alarm_indication (channel, argr, CM_ALARM_PRIORITY_LOW);
			cm_cl_alarm_indication (channel, argr, CM_ALARM_PRIORITY_HIGH);
		}
		break;

	case CM_CL_ALARM_ENABLED_PBE:
		if (argr->cm.cl.alarm_enabled_state != CM_CL_ALARM_ENABLED_PBE)
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) ar(%u): alarm_enabled_state(PRM-BEGIN-END)", argr->device_nr, argr->ar_nr
				);

			argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_PBE;
		}
		break;

	default:
		CM_FATAL1( enabled_state);
		return;
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_mcm_alarm_append_affected_submods (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_RQB_PTR_TYPE  upper,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	LSA_UINT16  common_diag_info
	)
{
	CM_LOG_FCT ("cm_cl_mcm_alarm_append_affected_submods")
/*	for all mcr containing alarm->api, alarm->slot, alarm->sub:
//		foreach ioapi of this mcr:
//			foreach iodata of this ioapi:
//				append ioapi->api, iodata->slot, iodata->subslot to diff_list
*/

	CM_AR_GRAPH_IO_CR_PTR_TYPE   mcr;
	CM_UPPER_ALARM_PTR_TYPE alarm;

	alarm = upper->args.channel.alarm;
	CM_ASSERT (CmListIsEmpty (& alarm->u.cl.diff_list));

	CM_ASSERT (alarm->alarm_type == CM_ALARM_TYPE_MCR_MISMATCH);
	CM_ASSERT (common_diag_info == CM_SUB_STATE_DIAG_INFO_AVAILABLE
		|| common_diag_info == CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE
		);

	for (CmListForeach (mcr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {
		CM_AR_GRAPH_IOAPI_PTR_TYPE  ioapi;
		CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
		LSA_BOOL  do_append;

		switch (mcr->iocr_type) {
		case CM_IOCR_TYPE_MULTICAST_PROVIDER:
		case CM_IOCR_TYPE_MULTICAST_CONSUMER:
			break;
		default:
			continue;
		}

		do_append = LSA_FALSE;
		for (CmListForeach (ioapi, & mcr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {
			if (ioapi->api != api) {
				continue;
			}
			for (CmListForeach (iodata, & ioapi->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE))
			{
				if (iodata->slot_nr == slot_nr && iodata->subslot_nr == subslot_nr) {
					do_append = LSA_TRUE;
					break;
				}
			}
		}
		if (! do_append) {
			continue;
		}

		for (CmListForeach (ioapi, & mcr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE))
		{
			for (CmListForeach (iodata, & ioapi->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE))
			{
				CM_AR_GRAPH_AP_PTR_TYPE  ap;
				CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

				cm_ar_graph_find_api_slot_subslot (argr,
						ioapi->api, iodata->slot_nr, iodata->subslot_nr,
						& ap, & mod, & sub
					);

				if (is_not_null (sub))
				{
					CM_UPPER_CL_DIFF_AP_PTR_TYPE  diff_ap;
					CM_UPPER_CL_DIFF_MODULE_PTR_TYPE  diff_mod;
					CM_UPPER_CL_DIFF_SUBMODULE_PTR_TYPE  diff_sub;

					CM_ASSERT (is_not_null (ap)); CM_ASSERT (is_not_null (mod));

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: check: really? setting diag-state from alarm to -all- related submodules")
#endif /* CM_MESSAGE */
					CM_SUB_STATE_SET_DIAG_INFO (sub->ist.sub_state, common_diag_info);

					diff_ap = cm_pd_diff_ap_find_append (channel, & alarm->u.cl.diff_list,
							ap->api
						);
					diff_mod = cm_pd_diff_module_find_append (channel, & diff_ap->module_list,
							mod->slot_nr, mod->ist.mod_ident
						);
					diff_sub = cm_pd_diff_submodule_find_append (channel, & diff_mod->submodule_list,
							sub->subslot_nr, sub->ist.sub_state, sub->ist.sub_ident
						);
					LSA_UNUSED_ARG (diff_sub);
				}
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_alarm_param_ok (
	LSA_UINT32  ar_properties,
	LSA_UINT16  alarm_type,
	LSA_UINT16  subslot_nr,
	CM_AR_GRAPH_AP_PTR_TYPE  ap,
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub
)
{
	CM_LOG_FCT ("cm_cl_alarm_param_ok")

		/* api and slot_nr must exist */
	if (is_null (ap)) {
		return LSA_FALSE;
	}
	if (is_null (mod)) {
		return LSA_FALSE;
	}

	switch (alarm_type)
	{
	case CM_ALARM_TYPE_PULL:
		if ((ar_properties & CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED) == 0)
		{
			if (is_null (sub)) {
				if (subslot_nr != 0) {
					return LSA_FALSE;
				}
				/* is a pull_module */
			}
			else
			{
				CM_ASSERT (subslot_nr != 0); /* see cm_ar_graph_insert_submodule() */
			}
		}
		else
		{
			if (is_null (sub)) {
				return LSA_FALSE;
			}
		}
		break;

	case CM_ALARM_TYPE_PULL_MODULE:
		if ((ar_properties & CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED) == 0)
		{
			return LSA_FALSE;
		}
		/* subslot_nr not checked */
		break;

	default:
		if (is_null (sub)) {
			return LSA_FALSE;
		}
		break;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_alarm_type_and_submod_state_matches (
	LSA_UINT16  alarm_type,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub
)
{
	CM_LOG_FCT ("cm_cl_alarm_type_and_submod_state_matches")
	LSA_UINT32  always_true;

	always_true = CM_ALARM_PLUG_GROUP | (1UL << CM_ALARM_TYPE_MCR_MISMATCH);

	if ((1UL << alarm_type) & always_true) {
		return LSA_TRUE;
	}

	CM_ASSERT (is_not_null (sub));
	switch (sub->ist.sub_state & CM_SUB_STATE_IDENT_MASK) {
	case CM_SUB_STATE_IDENT_OK:
	case CM_SUB_STATE_IDENT_SUBSTITUTE:
		return LSA_TRUE;

	case CM_SUB_STATE_IDENT_WRONG:
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:
	default:
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_set_sub_state (
	CM_UPPER_ALARM_PTR_TYPE  alarm,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub
	)
{
	CM_LOG_FCT ("cm_cl_alarm_set_sub_state")
		/*
		 * PNIO-Spec, AlarmSpecifier.XXXDiagnosis, Usage within AlarmType
		 *
		 * Diagnosis, Redundancy, multicast communication mismatch, Port data change notification,
		 * Sync data change notification, isochronous mode problem notification,
		 * Network component problem notification, Time data changed notification
		 */

		if (alarm->diag_submod_available) {
			CM_SUB_STATE_SET_DIAG_INFO (sub->ist.sub_state, CM_SUB_STATE_DIAG_INFO_AVAILABLE);
		}
		else {
			CM_SUB_STATE_SET_DIAG_INFO (sub->ist.sub_state, CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE);
		}

		if (alarm->maintenance_status & CM_MAINTENANCE_STATUS_REQUIRED) {
			sub->ist.sub_state = (sub->ist.sub_state & (~CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK))
				| CM_SUB_STATE_MAINTENANCE_REQUIRED
				;
		}
		else {
			sub->ist.sub_state = (sub->ist.sub_state & (~CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK))
				| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED
				;
		}

		if (alarm->maintenance_status & CM_MAINTENANCE_STATUS_DEMANDED) {
			sub->ist.sub_state = (sub->ist.sub_state & (~CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK))
				| CM_SUB_STATE_MAINTENANCE_DEMANDED
				;
		}
		else {
			sub->ist.sub_state = (sub->ist.sub_state & (~CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK))
				| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED
				;
		}

		if (alarm->maintenance_status & CM_MAINTENANCE_STATUS_QUA_BITS) {
			sub->ist.sub_state = (sub->ist.sub_state & (~CM_SUB_STATE_QUALIFIED_INFO_MASK))
				| CM_SUB_STATE_QUALIFIED_INFO_AVAILABLE
				;
		}
		else {
			sub->ist.sub_state = (sub->ist.sub_state & (~CM_SUB_STATE_QUALIFIED_INFO_MASK))
				| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE
				;
		}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_set_wait_for (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_ALARM_PTR_TYPE  alarm
	)
{
	if (alarm->alarm_priority == CM_ALARM_PRIORITY_LOW)
	{
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARM_LOW_RSP);
	}
	else
	{
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARM_HIGH_RSP);
	}
}


/*----------------------------------------------------------------------------*/
/* please note
**	AP00219532: deliver submodule_state to cm-user
**  AP00841926: never generate CM_MOD_STATE_WRONG_MODULE internally
*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  alarm_prio
)
{
	CM_LOG_FCT ("cm_cl_alarm_indication")
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_UPPER_ALARM_PTR_TYPE  alarm;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	upper = argr->cm.cl.alarm[alarm_prio].early_alarm;
	if (is_null (upper))
	{
		/* no stored alarm */
		return;
	}

	alarm = upper->args.channel.alarm;

	switch (argr->cm.cl.alarm_enabled_state) {
	case CM_CL_ALARM_ENABLED_IGNORE:
		/* abort */
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): alarm_enabled_state(ignore)  alarm_type(0x%x) state(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr
			, alarm->alarm_type
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
		cm_cl_arsm_down (argr, &dinfo);
		return;

	case CM_CL_ALARM_ENABLED_DELAY:
		if (! cm_ar_com_is_up (argr)
		&&  argr->cm.cl.state == CM_DST_WF_NEW_DATA
		&&	cm_cl_prm_mode(argr) == CM_CL_PRMMODE_IDLE/*! means connect in this state */
		&&	(argr->cm.cl.sumode == CM_SUMODE_AdvRT || argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3))
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "dev(%u) ar(%u): alarm_enable_state(delay) alarm_type(0x%x), send PreData.ind to enable Alarm-handling"
				, argr->device_nr, argr->ar_nr
				, alarm->alarm_type
				);
			/* trigger PreData.ind, trigger does CM_CL_ALARM_ENABLED_FORWARD */
			cm_cl_prm_ardy_timeout_stop (argr);
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);
			cm_cl_arsm (argr);
		}
		else
		{
			/* keep, because too early */
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "dev(%u) ar(%u): alarm_enable_state(delay) alarm_type(0x%x)"
				, argr->device_nr, argr->ar_nr
				, alarm->alarm_type
				);
		}
		return;

	case CM_CL_ALARM_ENABLED_FORWARD:
	case CM_CL_ALARM_ENABLED_PBE: /*AP01549770*/
		/* handle alarm */
		argr->cm.cl.alarm[alarm_prio].early_alarm = LSA_NULL;
		break;

	default:
		CM_FATAL1(argr->cm.cl.alarm_enabled_state);
		return;
	}

	cm_ar_graph_find_api_slot_subslot (argr,
			alarm->api, alarm->slot_nr, alarm->subslot_nr,
			& ap, & mod, & sub
		);

	/* checks are done in cm_cl_acp_alarm_indication_reject() */

	CM_CL_TRACE_12 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev(%u) ar(%u): alarm_type(0x%x) api(%u) slot(%u=0x%x) subslot(%u=0x%x)  diag[channel(%u) generic(%u) submod(%u)]  maintenance(%x)"
		, argr->device_nr, argr->ar_nr
		, alarm->alarm_type, alarm->api
		, alarm->slot_nr, alarm->mod_ident, alarm->subslot_nr, alarm->sub_ident
		, alarm->diag_channel_available, alarm->diag_generic_available, alarm->diag_submod_available
		, alarm->maintenance_status
		);

		/* handle alarm */
	switch (alarm->alarm_type)
	{
	case CM_ALARM_TYPE_DIAGNOSIS:
	case CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS:
	case CM_ALARM_TYPE_MEDIA_REDUNDANCY:
/*	case CM_ALARM_TYPE_MCR_MISMATCH: see below */
	case CM_ALARM_TYPE_PORT_DATA_CHANGED:
	case CM_ALARM_TYPE_SYNC_DATA_CHANGED:
	case CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM:
	case CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM:
	case CM_ALARM_TYPE_TIME_DATA_CHANGED:
	case CM_ALARM_TYPE_DFP_PROBLEM:
	case CM_ALARM_TYPE_MRPD_PROBLEM:
	case CM_ALARM_TYPE_MULTIPLE_INTERFACE:
			/* handle diag_info, see AlarmSpecifier */
		cm_cl_alarm_set_sub_state (alarm, sub);

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_MCR_MISMATCH:
			/* handle diag_info, see AlarmSpecifier */
		cm_cl_alarm_set_sub_state (alarm, sub);

		cm_cl_mcm_alarm_append_affected_submods (channel,
				argr, upper, ap->api, mod->slot_nr, sub->subslot_nr,
				CM_CAST_U16(sub->ist.sub_state & CM_SUB_STATE_DIAG_INFO_MASK)
			);
#ifdef CM_MESSAGE
# pragma CM_MESSAGE ("TODO: check: should alarm->api, alarm->slot_nr, alarm->subslot_nr set to 0")
#endif /* CM_MESSAGE */
			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PULL:

			/* subslot != 0 .. pull submodule */
		if (is_not_null (sub)) {
			/* mod->ist.mod_state = mod->ist.mod_state;  keep: more than one submod possible */

			sub->ist.sub_state = CM_SUB_STATE_PULLED;
			sub->ist.sub_ident = 0;
		}

			/* subslot == 0 .. pull module => pull all submodule */
			/* take care: plug alarm recv only for one subslot,
						so (maybe) we must update mod_state to */
		else {
			mod->ist.mod_ident = 0;

			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				sub->ist.sub_state = CM_SUB_STATE_PULLED;
				sub->ist.sub_ident = 0;
			}
		}

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PULL_MODULE:

		mod->ist.mod_ident = 0;

		for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
		{
			sub->ist.sub_state = CM_SUB_STATE_PULLED;
			sub->ist.sub_ident = 0;
		}

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR:

			/* no more the owner of this submodule */
		sub->ist.sub_state = CM_SUB_STATE_PULLED;	/* review: or only set_ar_info */
		CM_SUB_STATE_SET_AR_INFO (sub->ist.sub_state, CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR);

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PLUG:
	case CM_ALARM_TYPE_RELEASED:

		/* mod_ident and sub_(ident|state) is set with cm_cl_prm_record_list_build(), see queueing of cm_cl_prm_request() */

			/* keep plug-alarm */
			/* notify cm-user is done on transition WF_NEW_DATA -> IN_DATA */
		CM_ASSERT (is_null (argr->cm.cl.prm_alarm));
		argr->cm.cl.prm_alarm = upper;

			/* start prm-machine */
		/* argr->cm.cl.state = CM_DST_PRM; */
		cm_cl_prm_request (argr, CM_CL_PRMMODE_PLUG, upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PLUG_WRONG:

		mod->ist.mod_ident = alarm->mod_ident;

		sub->ist.sub_ident = alarm->sub_ident;
		sub->ist.sub_state = CM_SUB_STATE_WRONG;

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_RETURN_OF_SUBMODULE:

			/* keep mod_state and sub_state, as is */

			/* keep return_of_submodule-alarm */
			/* notify cm-user is done on transition WF_NEW_DATA -> IN_DATA */
		CM_ASSERT (is_null (argr->cm.cl.prm_alarm));
		argr->cm.cl.prm_alarm = upper;

			/* start wf-new-data, block other prm-sequences */
		/* argr->cm.cl.state = CM_DST_WF_APPL_READY_IND; */
		cm_cl_prm_request (argr, CM_CL_PRMMODE_RETURN_OF_SUB, upper);
		break;

/*----------------------------------------------------------------------------*/
	default:
		CM_ASSERT (((1UL << alarm->alarm_type) & CM_ALARM_DIAG_GROUP) == 0);

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm);
		cm_callback (channel, CM_OK, & upper);
		break;
	}
	return;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_alarm_maintenance_is_consistence (
	LSA_UINT16  user_tag,
	LSA_UINT16  user_length,
	CM_ACP_LOWER_MEM_PTR_TYPE  user_data,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  * maintenance_ptr
	)
{
	CM_LOG_FCT("cm_cl_alarm_maintenance_is_consistence")

	if (is_not_null (maintenance_ptr)) {
		*maintenance_ptr = 0;
	}

	if (user_length == 0) {
		/* maintenance not possible */
		return LSA_TRUE;
	}

	if (user_tag != CM_ALARM_USI_MAINTENANCE) {
		/* no maintenance prefix */
		return LSA_TRUE;
	}

	if (user_length < 2+2+2+2+4) {
		CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "wrong size(%d), must >= 2+2+2+2+4", user_length
			);
		return LSA_FALSE;
	}

	{
	CM_ACP_MEM_U8_PTR_TYPE  dp;

	LSA_UINT16  block_type, block_length, block_version, reserved;
	LSA_UINT32  maintenance;

	dp = (CM_ACP_MEM_U8_PTR_TYPE)user_data + CM_ALARM_OFFSET_DATA;

	block_type		= CM_GET16_NTOH (dp, 0);
	block_length	= CM_GET16_NTOH (dp, 2);
	block_version	= CM_GET16_NTOH (dp, 4);
	reserved		= CM_GET16_NTOH (dp, 6);
	maintenance		= CM_GET32_NTOH (dp, 8);

	if (block_type != CM_BLOCK_TYPE_MAINTENANCE  || block_length != 8
	|| block_version != 0x0100  ||  reserved != 0
	|| (maintenance & CM_MAINTENANCE_STATUS_RESERVED) != 0 /* since PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01545793 item 11 */
	) {
		CM_CL_TRACE_05 (0, LSA_TRACE_LEVEL_UNEXP
			, "maintenance-block-mismatch: something wrong: block_type(0x%x) block_length(%u) block_version(0x%x) res(%u) maintenance(0x%x)"
			, block_type, block_length, block_version, reserved
			, maintenance
			);
		return LSA_FALSE;
	}

	if (is_not_null (maintenance_ptr)) {
		*maintenance_ptr = maintenance;
	}
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_prm_update_reject_alarm (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  alarm_type
	)
{
/*	else if (acp_al->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE
//	&&	(cm_cl_prm_mode(argr) == CM_CL_PRMMODE_AR  ||  ! cm_cl_prm_request_can_queue (argr, CM_CL_PRMMODE_RETURN_OF_SUB)))
*/

	if (alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE)
	{
		if (argr->cm.cl.alarm_enabled_state == CM_CL_ALARM_ENABLED_PBE) /*AP01528984*/
		{
			return LSA_TRUE;
		}

		if (argr->cm.cl.prm.prm_seq_next == CM_CL_PRMMODE_AR)
		{
			/* AP01253553: this means, ROSAL is old, cancel it */  /* taken from cm_cl_prm_request_can_queue() */
			return LSA_TRUE;
		}
		/* may be changed with CM_CL_PRMMODE_SUBMODULE */
	}
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_acp_alarm_indication_reject (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prio,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR  * reject_upper,
	CM_ACP_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR  * reject_acp
	)
{
	CM_LOG_FCT ("cm_cl_acp_alarm_indication_reject")
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al;
	LSA_UINT32  pnio_err_do_reject;
	LSA_UINT16  next_recv_sequence_nr;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	*reject_upper = LSA_NULL;
	*reject_acp = LSA_NULL;

	acp_al = & acp->args->alarm.data;
	CM_ASSERT (prio == acp_al->priority);

	{
	LSA_UINT16  alarm_sequence;
	LSA_BOOL  diag_channel_available;
	LSA_BOOL  diag_generic_available;
	LSA_BOOL  diag_submod_available;
	LSA_BOOL  ar_diagnosis_state;

	ACP_GET_ALARM_SPECIFIER (& alarm_sequence
		, & diag_channel_available, & diag_generic_available, & diag_submod_available, & ar_diagnosis_state
		, acp_al->alarm_specifier
		);
	LSA_UNUSED_ARG (diag_channel_available);
	LSA_UNUSED_ARG (diag_generic_available);
	LSA_UNUSED_ARG (diag_submod_available);
	LSA_UNUSED_ARG (ar_diagnosis_state);

	CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) skey(%d) alarm_type(0x%x) prio(%u) al_seq_nr(%d) api(%u) slot(%d) subslot(%d)"
		, argr->device_nr, argr->ar_nr
		, argr->session_key, acp_al->alarm_type, acp_al->priority, alarm_sequence
		, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
		);

	if (prio == CM_ALARM_PRIORITY_HIGH  &&  (argr->alarm_cr->alcr_properties & CM_ALCR_PROP_PRIO_MASK) == CM_ALCR_PROP_PRIO_ONLY_LOW)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) ar(%u): alarm_indication: ignored; CM_ALARM_PRIORITY_HIGH and alcr_properties wants CM_ALCR_PROP_PRIO_ONLY_LOW"
			, argr->device_nr, argr->ar_nr
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_PROT));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;
	}

		/* check expected sequence_nr */
	if (argr->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED)
	{
		argr->cm.cl.alarm[prio].recv_sequence_nr = alarm_sequence; /* SRD, v2.3_Ed4: The SequenceNumber may start for each AR of an AR set with _every switchover_ with an arbitrary value. */
	}
	else if (argr->cm.cl.alarm[prio].recv_sequence_nr == CM_ALARM_SEQUENCE_UNDEF)
	{
		argr->cm.cl.alarm[prio].recv_sequence_nr = alarm_sequence; /* AP01334245, Rev16: The Alarm receiver shall accept an arbitrary value as start value */
	}
	else if (argr->cm.cl.alarm[prio].recv_sequence_nr != alarm_sequence)
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u) expected.seq_nr(%d) != recv.seq_nr(%d)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.cl.alarm[prio].recv_sequence_nr, alarm_sequence
			);

		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT), argr->cm.cl.alarm[prio].recv_sequence_nr);
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;
	}
	next_recv_sequence_nr = argr->cm.cl.alarm[prio].recv_sequence_nr;
	CM_ALARM_SEQUENCE_NR_INCREMENT (next_recv_sequence_nr);

		/* keep check, an alarm is running */
	if (is_not_null (argr->cm.cl.alarm[prio].early_alarm)
	/*AP01287831*/
	|| (prio == CM_ALARM_PRIORITY_LOW   &&  CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_WF_ALARM_LOW_RSP))
	|| (prio == CM_ALARM_PRIORITY_HIGH  &&  CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_WF_ALARM_HIGH_RSP)))
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u) previous alarm running, protocol fault; alarm-prio(%u) seq(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr
			, prio, alarm_sequence, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;
	}
	}

	if (argr->cm.cl.alarm_enabled_state == CM_CL_ALARM_ENABLED_IGNORE)
	{
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u) currently in alarm_state(IGNORE), state(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		/* down previously done in cm_cl_alarm_indication() */
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;
	}

		/* if maintenance: check consistency */
	if (! cm_cl_alarm_maintenance_is_consistence (acp_al->user_tag, acp_al->user_length, acp_al->user_data, LSA_NULL))
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): maintenace-alarm not consistence, alarm_type(0x%x)"
			, argr->device_nr, argr->ar_nr, acp_al->alarm_type
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;
	}

	if (acp_al->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE  &&  ! cm_cl_prm_request_can_queue (argr, CM_CL_PRMMODE_RETURN_OF_SUB))
	{
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): sequence error: cann't queue alarm_type(0x%x), wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr, acp_al->alarm_type
			, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;
	}

	{
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	cm_ar_graph_find_api_slot_subslot (argr, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr, & ap, & mod, & sub);

		/* check if alarm is related to ar */
	if (! cm_cl_alarm_param_ok (argr->ar.ar_properties, acp_al->alarm_type, acp_al->subslot_nr, ap, mod, sub))
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): unknown submodule: alarm_type(0x%x) api(%u) slot(%u) subslot(%u)"
			, argr->device_nr, argr->ar_nr
			, acp_al->alarm_type, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;
	}

		/* check if alarm is to be rejected */
	if (! cm_alarm_type_is_supported (channel, acp_al->alarm_type))
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): internal reject, reject alarm_type(0x%x)  alarm_reject(0x%x)"
			, argr->device_nr, argr->ar_nr
			, acp_al->alarm_type, channel->acp.alarm_reject
			);
		pnio_err_do_reject = CM_PNIO_ERR_AlarmTypeNotSupported;
	}

		/* check if submodule.state is proper / substituted */
	else if (! cm_cl_alarm_type_and_submod_state_matches (acp_al->alarm_type, sub))
	{
		CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): alarm_type(0x%x) and submod.state(0x%x) doesn't match, submodule(%u/%u/%u)"
			, argr->device_nr, argr->ar_nr
			, acp_al->alarm_type, is_not_null (sub) ? sub->ist.sub_state : 0xFFFF
			, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
			);
		pnio_err_do_reject = CM_PNIO_ERR_AlarmAndWrongSubmoduleState;
	}

		/* check if ROSAL does overlap with PrmUpdate */
#if 0 /*AP01528984*/
	else if (acp_al->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE
	&&	(cm_cl_prm_mode(argr) == CM_CL_PRMMODE_AR  ||  ! cm_cl_prm_request_can_queue (argr, CM_CL_PRMMODE_RETURN_OF_SUB)))
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): alarm_type(0x%x/ROSAL) submodule(%u/%u/%u), cann't queue because of running PrmUpdate-Sequence"
			, argr->device_nr, argr->ar_nr, acp_al->alarm_type
			, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
			);
		pnio_err_do_reject = CM_PNIO_ERR_AlarmAndWrongSubmoduleState;
	}
#else
		/* works only with CM_CL_PRMMODE_AR, need changes with CM_CL_PRMMODE_SUBMODULE */
	else if (cm_cl_prm_update_reject_alarm (argr, acp_al->alarm_type))
	{
		CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): alarm_type(0x%x) submodule(%u/%u/%u), cann't queue because of running PrmUpdate-Sequence, wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr, acp_al->alarm_type
			, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
			, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		pnio_err_do_reject = CM_PNIO_ERR_AlarmAndWrongSubmoduleState;
	}
#endif

	else
	{
		pnio_err_do_reject = CM_PNIO_ERR_NONE;
	}
	}

		/* prepare next expected */
	argr->cm.cl.alarm[prio].recv_sequence_nr = next_recv_sequence_nr;

	if (pnio_err_do_reject != CM_PNIO_ERR_NONE)
	{
		CM_UPPER_RQB_PTR_TYPE  cm_ack;
		CM_UPPER_ALARM_PTR_TYPE  cm_al;
		CM_ACP_LOWER_RQB_PTR_TYPE  acp_ack;

		if (is_not_null (argr->cm.cl.alarm[prio].ack))
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): a ack is running, alarm_type(0x%x)"
				, argr->device_nr, argr->ar_nr
				, argr->cm.cl.alarm[prio].ack->args.cl.alarm_ack_send->alarm_type
				);

			CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT), pnio_err_do_reject);
			cm_cl_arsm_down (argr, &dinfo);
			return LSA_FALSE;
		}

		cm_ack = cm_upper_alloc_rqb (channel);
		if (is_null (cm_ack))
		{
			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
			cm_cl_arsm_down (argr, &dinfo);
			return LSA_FALSE;
		}
		cm_al = cm_ack->args.channel.alarm;

		cm_acp_alloc_rqb (channel, & acp_ack);
		if (is_null (acp_ack))
		{
			cm_upper_free_rqb (channel, cm_ack);

			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
			cm_cl_arsm_down (argr, &dinfo);
			return LSA_FALSE;
		}

		cm_acp_alarm_copy_ack (cm_ack, acp, CM_INTERNAL_OPC_CL_ALARM_ACK_SEND);

		CmListInitialize (& cm_al->u.cl.diff_list); /* sanity */
		cm_al->u.cl.mod_diff_block_length = 0;
		cm_al->u.cl.mod_diff_block = LSA_NULL;

		cm_al->cm_pnio_err = pnio_err_do_reject;

			/* keep internal */
		CM_ASSERT (is_null (argr->cm.cl.alarm[prio].ack));
		argr->cm.cl.alarm[prio].ack = cm_ack;

		*reject_upper = cm_ack;
		*reject_acp = acp_ack;
		return LSA_TRUE;
	}

	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_acp_alarm_indication_no_check (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prio,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
	)
{
	CM_LOG_FCT ("cm_cl_acp_alarm_indication_no_check")
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_UPPER_ALARM_PTR_TYPE  alarm;
	LSA_UINT32  maintenance_status;

	if (cm_ar_com_is_down (argr)) {
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): ar is_down"
			, argr->device_nr, argr->ar_nr
			);
		return;
	}

	/*
	//	param-checking and internal rejects already done before calling this function
	*/

		/* get upper */
	upper = cm_cl_dev_alarm_get (argr->cm.cl.dev, prio);
	if (is_null (upper))
	{
		CM_CL_DOWN_INFO_TYPE  dinfo;

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u) !upper-rqb"
			, argr->device_nr, argr->ar_nr
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	alarm = upper->args.channel.alarm;

	alarm->device_nr	= argr->device_nr;
	alarm->ar_nr		= argr->ar_nr;
	alarm->session_key	= argr->session_key;

	CmListInitialize (& alarm->u.cl.diff_list);
	alarm->u.cl.mod_diff_block_length = 0;
	alarm->u.cl.mod_diff_block = LSA_NULL;

	/* get maintenance_status prior then alarm_copy */
	if (! cm_cl_alarm_maintenance_is_consistence (
			acp->args->alarm.data.user_tag, acp->args->alarm.data.user_length, acp->args->alarm.data.user_data,
			& maintenance_status
	)) {
		CM_FATAL (); /* see cm_cl_acp_alarm_indication_reject() */
		return;
	}

	cm_acp_alarm_copy (upper, acp, CM_OPC_CL_ALARM_IND);

	alarm->maintenance_status = maintenance_status;


	CM_ASSERT (is_null (argr->cm.cl.alarm[prio].early_alarm));
	argr->cm.cl.alarm[prio].early_alarm = upper;
	cm_cl_alarm_indication (channel, argr, prio);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_send_next (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prio
	)
{
	CM_LOG_FCT ("cm_cl_alarm_send")
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	upper = CmListFirst (& argr->cm.cl.alarm[prio].alarm_list, CM_UPPER_RQB_PTR_TYPE);
	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): prio(%d) upper(%08x)"
		, argr->device_nr
		, argr->ar_nr
		, prio
		, upper
		);

	if (is_null (upper)) {
		return;
	}

	CM_RQB_SET_RESPONSE (upper, CM_CL_ALPMI_INIT); /* state of alarm_send.rqb */

	cm_acp_alloc_rqb (channel, & acp);
	if (is_null (acp)) {
		CM_CL_DOWN_INFO_TYPE  dinfo;

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALSND));
		cm_cl_arsm_down (argr, &dinfo);
		/* cancel alarm-send-queue in cm_cl_ar_com_down */
		return;
	}

	CM_ASSERT (upper->args.cl.alarm_send->alarm_priority == prio);

	upper->args.cl.alarm_send->alarm_sequence = argr->cm.cl.alarm[prio].send_sequence_nr;
	CM_ALARM_SEQUENCE_NR_INCREMENT (argr->cm.cl.alarm[prio].send_sequence_nr);

	if (((1UL << upper->args.cl.alarm_send->alarm_type) & CM_ALARM_DIAG_GROUP) != 0)
	{
		upper->args.cl.alarm_send->ar_diagnosis_state = argr->cm.cl.station_problem;
	}

	cm_acp_alarm_send (argr, ACP_OPC_ALARM_SEND, upper
		, upper->args.cl.alarm_send->alarm_tag
		, upper->args.cl.alarm_send->alarm_data_length
		, upper->args.cl.alarm_send->alarm_data
		, acp
		);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_send_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_cl_alarm_send_done")
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  prio;
	LSA_UINT16  rsp;
	LSA_UINT16  state;

	CM_ASSERT (is_not_null (argr));
	CM_ASSERT (is_not_null (acp));

	rsp = CM_ACP_GET_RESPONSE (acp);
	prio = acp->args->alarm.data.priority;
	CM_ASSERT (prio == CM_ALARM_PRIORITY_LOW || prio == CM_ALARM_PRIORITY_HIGH);

	upper = CmListFirst (& argr->cm.cl.alarm[prio].alarm_list, CM_UPPER_RQB_PTR_TYPE);
	if (is_null (upper)) {
		CM_CL_DOWN_INFO_TYPE  dinfo;

		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!upper, acp(0x%08x)", acp);
		cm_acp_free_rqb (channel, & acp);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALSND));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	cm_acp_free_rqb (channel, & acp);

	state = CM_RQB_GET_RESPONSE (upper);
	state = state | CM_CL_ALPMI_SEND_DONE;
	CM_RQB_SET_RESPONSE (upper, state);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): rsp(%d) upper(%08x) state(%x)"
		, argr->device_nr
		, argr->ar_nr
		, rsp
		, upper
		, state
		);

		/* case: good */
	if (rsp == ACP_OK) {
		if ((state & CM_CL_ALPMI_SEND_DONE)  &&  (state & CM_CL_ALPMI_ACK_DONE)) {
			CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)upper);

			cm_callback (channel, CM_OK, & upper);

				/* kick-next */
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: dev should decide, not iter here")
#endif /* CM_MESSAGE */
			cm_cl_alarm_send_next (channel, argr, prio);
			return;
		}
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "wf ack.done"
			);
		return;
	}

		/* case: bad */
		/* ar goes down */
	CM_ASSERT (cm_ar_com_is_down (argr));

	CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)upper);
	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "rsp(%d) upper(0x%08x)", rsp, upper);

	if (rsp == ACP_ERR_ASRT  ||  rsp == ACP_OK_CANCEL) {
		cm_callback (channel, CM_ERR_SESSION, & upper);
	}
	else {
		cm_callback (channel, CM_ERR_LOWER_LAYER, & upper);
	}
	/* cancel alarm-send-queue in cm_cl_ar_com_down */
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_ack_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prio,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_cl_alarm_ack_indication")
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  state;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_ASSERT (is_not_null (argr));
	CM_ASSERT (is_not_null (acp));

	CM_ASSERT (prio == CM_ALARM_PRIORITY_LOW || prio == CM_ALARM_PRIORITY_HIGH);

	upper = CmListFirst (& argr->cm.cl.alarm[prio].alarm_list, CM_UPPER_RQB_PTR_TYPE);
	if (is_null (upper)) {
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!upper, acp(0x%08x)", acp);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALACK));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	if (! cm_acp_alarm_ack_matches (upper, acp)) {
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!matches, acp(0x%08x)", acp);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALACK));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	state = CM_RQB_GET_RESPONSE (upper);
	state = state | CM_CL_ALPMI_ACK_DONE;
	CM_RQB_SET_RESPONSE (upper, state);

	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): upper(%08x) state(%x)"
		, argr->device_nr
		, argr->ar_nr
		, upper
		, state
		);

	if ((state & CM_CL_ALPMI_SEND_DONE)  &&  (state & CM_CL_ALPMI_ACK_DONE)) {

		CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)upper);
		cm_callback (channel, CM_OK, & upper);

				/* kick-next */
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: dev should decide, not iter here")
#endif /* CM_MESSAGE */
		cm_cl_alarm_send_next (channel, argr, prio);
	}
	else {
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, ".cnf-sequence-bug: wf send.done"
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_alarm_ack_send_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_cl_alarm_ack_send_done")
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  prio;
	LSA_UINT16  rsp;

	CM_ASSERT (is_not_null (argr));
	CM_ASSERT (is_not_null (acp));

	prio = acp->args->alarm.data.priority;
	upper = argr->cm.cl.alarm[prio].ack;
	argr->cm.cl.alarm[prio].ack = LSA_NULL;

	switch (CM_ACP_GET_RESPONSE (acp)) {
	case ACP_OK:
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rsp(OK) prio(%d)"
			, prio
			);
		rsp = CM_OK;
		break;

	case ACP_OK_CANCEL:
	case ACP_ERR_ASRT:
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "rsp(%d) prio(%d)", CM_ACP_GET_RESPONSE (acp), prio);
		rsp = CM_ERR_SESSION;
		break;

	default:
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "rsp(%d) prio(%d)", CM_ACP_GET_RESPONSE (acp), prio);
		rsp = CM_ERR_LOWER_LAYER;
		break;
	}

	cm_acp_free_rqb (channel, & acp);

	if (is_null (upper))
	{
		CM_CL_DOWN_INFO_TYPE  dinfo;

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALACK));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

		/* check internal reject, see cm_cl_alarm_indication() */
	if (CM_RQB_GET_OPCODE (upper) == CM_INTERNAL_OPC_CL_ALARM_ACK_SEND)
	{
			/* must free internal upper */
		cm_upper_free_rqb (channel, upper);
	}
	else
	{
		CM_ASSERT (CM_RQB_GET_OPCODE(upper) == CM_OPC_CL_ALARM_ACK_SEND);

			/* else a cm-user alarm-ack-send */
		cm_callback (channel, rsp, & upper);
	}

		/* check for queued alarm.ind */
	cm_cl_alarm_indication (channel, argr, prio);
}

/*---------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_link_status_changed (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL is_up,
	LSA_BOOL is_transient /* LSA_TRUE: the "down" is transient (some 100ms) */
)
{
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (is_up);
	LSA_UNUSED_ARG (is_transient);

	/*
	 * note: client uses schedule-timeout for checking link-state up
	 */

	/*
	 * note: do not cm_cl_ar_set_to_down(LINK_DOWN), wait for miss.ind instead
	 */
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
