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
/*  F i l e               &F: cm_acp3.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-AR-ACP                                                                */
/*  - alarm things                                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  22
#define CM_MODULE_ID       22

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_params (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  alarm_reject,
	LSA_UINT32  alarm_priority
)
{
	channel->acp.alarm_reject = alarm_reject;

	/***/

	if( (alarm_priority & CM_ALARM_PLUG_GROUP) != 0 ) {

		alarm_priority |= CM_ALARM_PLUG_GROUP; /* set all if any is set */
	}

	if( (alarm_priority & CM_ALARM_DIAG_GROUP) != 0 ) {

		alarm_priority |= CM_ALARM_DIAG_GROUP; /* set all if any is set */
	}

	channel->acp.alarm_priority = alarm_priority;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_acp_alarm_bit_from_type (
	LSA_UINT16  alarm_type
)
{
	CM_LOG_FCT ("cm_acp_alarm_bit_from_type")
	LSA_UINT32  alarm_bit;

	/*
	 * bit 0 .. "vendor specific" Alarm
	 * bit 1 - 31 .. (1UL << CM_ALARM_TYPE_xxx)
	 */

	if( alarm_type < 1 || alarm_type > 31 ) {

		alarm_bit = (1UL << 0); /* use "vendor specific" */
	}
	else {

		alarm_bit = (1UL << alarm_type);
	}

	return alarm_bit;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_acp_alarm_prio_from_type (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  alarm_type
)
{
	CM_LOG_FCT ("cm_acp_alarm_prio_from_type")
	LSA_UINT32  alarm_bit;
	LSA_UINT16  alarm_priority;

	/*
	 * bit set: alarm send with high priority
	 */

	alarm_bit = cm_acp_alarm_bit_from_type (alarm_type);

#if CM_ALARM_PRIORITY_LOW != 0 || CM_ALARM_PRIORITY_HIGH != 1
# error "check this!"
#endif

	if ((channel->acp.alarm_priority & alarm_bit) != 0) {
		alarm_priority = CM_ALARM_PRIORITY_HIGH;
	}
	else {
		alarm_priority = CM_ALARM_PRIORITY_LOW;
	}

	return alarm_priority;
}

/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_alarm_type_is_internal ( /* user is not allowed to send this alarm-type */
	LSA_UINT16  alarm_type
)
{
	CM_LOG_FCT ("cm_acp_alarm_type_is_internal")
	LSA_UINT32  alarm_bit;

	if (alarm_type < 0x0020) {

		switch( alarm_type ) {

		case 0x0000: /* reserved */
			return LSA_TRUE;

		case CM_ALARM_TYPE_MCR_MISMATCH:              /* internal, from m-consumer */
		case CM_ALARM_TYPE_MEDIA_REDUNDANCY:          /* internal, from CM-PD */
		case CM_ALARM_TYPE_PORT_DATA_CHANGED:         /* internal, from CM-PD */
		case CM_ALARM_TYPE_SYNC_DATA_CHANGED:         /* internal, from CM-PD */
		case CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM: /* internal, from CM-PD */
		case CM_ALARM_TYPE_TIME_DATA_CHANGED:         /* internal, from CM-PD */
		case CM_ALARM_TYPE_DFP_PROBLEM:               /* internal, from CM-PD */
		case CM_ALARM_TYPE_MRPD_PROBLEM:              /* internal, from CM-PD */
		case CM_ALARM_TYPE_MULTIPLE_INTERFACE:        /* internal, from CM-PD */
			return LSA_TRUE;

		default:
			break;
		}

		alarm_bit = cm_acp_alarm_bit_from_type (alarm_type);

		if ((alarm_bit & CM_ALARM_PLUG_GROUP) != 0) {
			return LSA_TRUE;
		}
	}
	else if (alarm_type < 0x0100) {
		/* manufacturer or profile specific */
	}
	else {
		/* reserved */
		return LSA_TRUE;
	}

	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_alarm_type_is_supported (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  alarm_type
)
{
	CM_LOG_FCT ("cm_alarm_type_is_supported")
	LSA_UINT32  alarm_bit;

	if (alarm_type < 0x0020) {

		if (alarm_type == 0x0000) {
			return LSA_FALSE;
		}

		alarm_bit = cm_acp_alarm_bit_from_type (alarm_type);

		if ((channel->acp.alarm_reject & alarm_bit) != 0) {
			return LSA_FALSE;
		}
	}
	else if (alarm_type < 0x0080) { /* manufacturer specific, use bit 0 */

#if CM_ALARM_MANUFACTURER_SPECIFIC_GROUP != 1UL
#error "check this"
#endif

		if ((channel->acp.alarm_reject & CM_ALARM_MANUFACTURER_SPECIFIC_GROUP) != 0) {
			return LSA_FALSE;
		}
	}
	else if (alarm_type < 0x0100) { /* profiles */

		/* ok */
	}
	else { /* reserved */

		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_send (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_OPCODE_TYPE  opcode,
	CM_UPPER_RQB_PTR_TYPE  upper,
	LSA_UINT16  upper_alarm_tag,
	LSA_UINT16  upper_alarm_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  upper_alarm_data,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_acp_alarm_send")
	CM_CHANNEL_PTR_TYPE  channel = ar_graph->ar_com.channel;
	CM_UPPER_ALARM_PTR_TYPE  ad; /* common alarm data struct */
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr;

	CM_ASSERT (opcode == ACP_OPC_ALARM_SEND || opcode == ACP_OPC_ALARM_ACK_SEND);

	ad = upper->args.channel.alarm;
#if CM_CFG_MAX_CLIENTS
	CM_ASSERT (are_equal (upper->args.cl.alarm_send, ad));
#endif
#if CM_CFG_MAX_SERVERS
	CM_ASSERT (are_equal (upper->args.sv.ar_alarm_send, ad));
#endif

	CM_ASSERT (ad->alarm_priority == CM_ALARM_PRIORITY_LOW || ad->alarm_priority == CM_ALARM_PRIORITY_HIGH);

		/* is there an alarm_cr ? */
	alarm_cr = ar_graph->alarm_cr;
	if (is_null (alarm_cr)) {
		CM_ACP_LOWER_RQB_ERROR (acp);
		return;
	}

	acp->args->alarm.data.alarm_ref = alarm_cr->local_alarm_ref;
	acp->args->alarm.data.priority = ad->alarm_priority;
	acp->args->alarm.data.usr_id = (LSA_UINT32)alarm_cr;

	acp->args->alarm.data.api = ad->api;
	acp->args->alarm.data.alarm_type = ad->alarm_type;
	acp->args->alarm.data.slot_nr = ad->slot_nr;
	acp->args->alarm.data.subslot_nr = ad->subslot_nr;

	CM_ASSERT (ad->alarm_sequence <= 0x07FF);

	ACP_SET_ALARM_SPECIFIER (& acp->args->alarm.data.alarm_specifier,
			ad->alarm_sequence, ad->diag_channel_available,
			ad->diag_generic_available, ad->diag_submod_available,
			ad->ar_diagnosis_state
		);

	if (opcode == ACP_OPC_ALARM_ACK_SEND) {

		acp->args->alarm.data.pnio_status = ad->cm_pnio_err;

		CM_ASSERT(upper_alarm_data_length == 0);
		CM_ASSERT(is_null(upper_alarm_data));

		CM_ACP_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "alarm_ack_send:alarm_cr(%08x) local_alarm_ref(%u) pnio(%08x)"
			, alarm_cr, alarm_cr->local_alarm_ref, ad->cm_pnio_err
			);

		cm_acp_request_lower (channel, opcode, alarm_cr, acp);
		return;
	}

	acp->args->alarm.data.module_ident		= ad->mod_ident;
	acp->args->alarm.data.submodule_ident	= ad->sub_ident;

	acp->args->alarm.data.user_max_length	= upper_alarm_data_length; /* not: channel->acp.max_alarm_data_length */

	acp->args->alarm.data.user_tag			= upper_alarm_tag; /* not: ad->alarm_... */
	acp->args->alarm.data.user_length		= upper_alarm_data_length; /* not: ad->alarm_... */
	acp->args->alarm.data.user_data			= upper_alarm_data; /* not: ad->alarm_... */

	CM_ACP_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "alarm_send:alarm_cr(%08x) local_alarm_ref(%u)"
		, alarm_cr, alarm_cr->local_alarm_ref
		);

	cm_acp_request_lower (channel, opcode, alarm_cr, acp);

	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_send_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_acp_alarm_send_done")

	if (is_null (alarm_cr)) {
		CM_ACP_LOWER_RQB_ERROR (acp);
		return;
	}

	if (CM_ACP_GET_RESPONSE (acp) != ACP_OK) {
		CM_ACP_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "down");
		cm_ar_com_down_debug_data_fmt(alarm_cr->cm.backlink, CM_MODULE_ID, __LINE__, CM_AR_REASON_ALSND, CM_ACP_GET_RESPONSE (acp));
		cm_ar_com_down(alarm_cr->cm.backlink, CM_AR_REASON_ALSND);
	}

	switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_alarm_send_done (channel, alarm_cr->cm.backlink, acp);
		break;
#endif
#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_ar_almi_send_done (alarm_cr->cm.backlink, acp);
		break;
#endif
	default:
		CM_FATAL(); /* a bug */
		break;
	}
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_ack_send_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_acp_alarm_ack_send_done")

	CM_ASSERT (is_not_null (alarm_cr));


	if (CM_ACP_GET_RESPONSE (acp) == ACP_OK) {
		CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rsp(%u)", CM_ACP_GET_RESPONSE (acp)
			);
	}
	else {
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "rsp(%u)", CM_ACP_GET_RESPONSE (acp));
		cm_ar_com_down_debug_data_fmt(alarm_cr->cm.backlink, CM_MODULE_ID, __LINE__, CM_AR_REASON_ALACK, CM_ACP_GET_RESPONSE (acp));
		cm_ar_com_down(alarm_cr->cm.backlink, CM_AR_REASON_ALACK);
	}

	switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_alarm_ack_send_done (channel, alarm_cr->cm.backlink, acp);
		break;
#endif
#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_ar_alarm_ack_send_done (alarm_cr->cm.backlink, acp);
		break;
#endif
	default:
		CM_FATAL(); /* a bug */
		break;
	}

	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_acp_alarm_indication")
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al;
	CM_AR_GRAPH_PTR_TYPE  ar_graph;
	LSA_UINT16  prio;
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr;
	LSA_BOOL  reject;
	CM_UPPER_RQB_PTR_TYPE reject_upper;
	CM_ACP_LOWER_RQB_PTR_TYPE reject_acp;

	acp_al = & acp->args->alarm.data;

	alarm_cr = (CM_AR_GRAPH_ALARM_CR_PTR_TYPE)acp_al->usr_id;

	if (is_null (alarm_cr)) {
		CM_ACP_LOWER_RQB_ERROR (acp);
		return; /* unreachable */
	}

	prio = acp_al->priority;
	CM_ASSERT (prio == ACP_ALARM_PRIORITY_LOW || prio == ACP_ALARM_PRIORITY_HIGH);
/* see provide:
	CM_ASSERT(ACP_ALARM_PRIORITY_LOW == CM_ALARM_PRIORITY_LOW);
	CM_ASSERT(ACP_ALARM_PRIORITY_HIGH == CM_ALARM_PRIORITY_HIGH);
*/

	ar_graph = alarm_cr->cm.backlink;
	CM_ASSERT (is_not_null (ar_graph));

	/***/

	reject = LSA_FALSE;
	reject_upper = LSA_NULL;
	reject_acp = LSA_NULL;

	if( cm_ar_com_is_down (ar_graph) ) {
		CM_ACP_TRACE_00 (ar_graph->trace_idx, LSA_TRACE_LEVEL_WARN, "already down");
		goto free_and_provide_;
	}

		/* alarm goes to error-state */
	if (CM_ACP_GET_RESPONSE (acp) != ACP_OK) {
		CM_ACP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "alarm_indication: !ok, acp-rsp(%u) pnio_status(0x%08x)"
			, CM_ACP_GET_RESPONSE (acp), acp_al->pnio_status
			);

		if (CM_ACP_GET_RESPONSE (acp) == ACP_ERR_ASRT) { /* AP00678183 */
			/*
			 * a) ACP got an alarm but could not send the asrt-ack (pnio_status is 0)
			 * b) ACP could not parse the asrt-data (pnio_status is 0)
			 * c) ACP got an asrt-error (pnio_status is taken from the asrt-error)
			 */
			if (ar_graph->ar_com.down == CM_AR_REASON_NONE) { /* should make a cm_ar_com_down3()... */
				ar_graph->ar_com.down_pnio_status = acp_al->pnio_status;
			}
		}

		cm_ar_com_down_debug_data_fmt (ar_graph, CM_MODULE_ID, __LINE__, CM_AR_REASON_ASRT, ar_graph->ar_com.down_pnio_status);
		cm_ar_com_down (ar_graph, CM_AR_REASON_ASRT);
		goto free_and_provide_;
	}

	if (acp_al->user_length == 0xFFFF) {
		/* truncation is a protocol violation */
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "alarm data truncated, more data then user_max_length(%u)", acp_al->user_max_length
			);
		cm_ar_com_down_debug_data_fmt (ar_graph, CM_MODULE_ID, __LINE__, CM_AR_REASON_ALLEN, acp_al->user_max_length);
		cm_ar_com_down(ar_graph, CM_AR_REASON_ALLEN);
		goto free_and_provide_;
	}

	switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		reject = cm_cl_acp_alarm_indication_reject (channel, ar_graph, prio, acp, &reject_upper, &reject_acp);
		if (! reject) {
			cm_cl_acp_alarm_indication_no_check (channel, ar_graph, prio, acp);
		}
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		reject = cm_sv_ar_alarm_indication (ar_graph, prio, acp, &reject_upper, &reject_acp);
		break;
#endif

	default:
		CM_FATAL(); /* a bug */
		break;
	}

free_and_provide_:

		/* now free acp-rqb, without the alarm-data-mem */
	cm_acp_free_rqb (channel, & acp);

		/* provide one resource again */
	cm_acp_alloc_rqb (channel, & acp);
	if (is_null (acp)) {
		CM_ACP_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!lower mem 4 acp");

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REVIEW: Pufferkreislauf, sequence free -> alloc, problem if cnf(-)")
#endif /* CM_MESSAGE */
		return;
	}
	CM_ACP_ALLOC_ALARM_MEM (CM_CAST_ACP_MEM_PTR_PTR (& acp->args->alarm.data.user_data),
			channel->acp.max_alarm_data_length, channel->sysptr
		);
	if (is_null (acp->args->alarm.data.user_data)) {
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "h(%u) cannot alloc ACP ALARM MEM(%u)"
			, channel->my_handle, channel->acp.max_alarm_data_length
			);

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REVIEW: Pufferkreislauf, sequence free -> alloc, problem if cnf(-)")
#endif /* CM_MESSAGE */
		cm_acp_free_rqb (channel, & acp);
		return;
	}

	acp->args->alarm.data.user_max_length = channel->acp.max_alarm_data_length;

	acp->args->alarm.data.priority = prio; /* rqb is re-allocated! */

	cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, alarm_cr, acp);

	/***/

	if( reject ) {

		CM_ACP_TRACE_00 (ar_graph->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "rejecting");

		CM_ASSERT(is_not_null(reject_upper));
		CM_ASSERT(is_not_null(reject_acp));

		CM_ASSERT(
			reject_upper->args.channel.alarm->cm_pnio_err == CM_PNIO_ERR_AlarmTypeNotSupported
		||	reject_upper->args.channel.alarm->cm_pnio_err == CM_PNIO_ERR_AlarmAndWrongSubmoduleState
		);

		cm_acp_alarm_send (ar_graph, ACP_OPC_ALARM_ACK_SEND, reject_upper, 0, 0, LSA_NULL, reject_acp);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_copy_ack (
	CM_UPPER_RQB_PTR_TYPE  cm,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	LSA_OPCODE_TYPE cm_opcode
)
{
	CM_LOG_FCT("cm_acp_alarm_copy_ack")
	CM_UPPER_ALARM_PTR_TYPE  cm_al;
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al;

	cm_al = cm->args.channel.alarm;

	acp_al = & acp->args->alarm.data;

	/***/

	CM_RQB_SET_OPCODE (cm, cm_opcode);

	cm_al->alarm_priority = acp_al->priority;


		/* copy data */

	cm_al->api				= acp_al->api;
	cm_al->slot_nr			= acp_al->slot_nr;
	cm_al->subslot_nr		= acp_al->subslot_nr;

	cm_al->alarm_type		= acp_al->alarm_type;

	ACP_GET_ALARM_SPECIFIER (
			& cm_al->alarm_sequence, & cm_al->diag_channel_available,
			& cm_al->diag_generic_available, & cm_al->diag_submod_available,
			& cm_al->ar_diagnosis_state, acp_al->alarm_specifier
		);
	cm_al->reserved			= 0;

	cm_al->mod_ident		= acp_al->module_ident;
	cm_al->sub_ident		= acp_al->submodule_ident;

	/***/

	cm_al->alarm_tag		= 0;
	cm_al->alarm_data		= 0;
	cm_al->alarm_data_length= 0;

	/***/

	cm_al->cm_pnio_err		= CM_PNIO_ERR_MAKE (0xff, 0xff, 0xff, 0xff); /* docu says: do not evaluate */

	/***/

	CM_ACP_TRACE_07(0, LSA_TRACE_LEVEL_CHAT,
		"dev/ar(%u/%u) al_type(%u) api(%u) slot(%u) subslot(%u) alarm_tag(%u)",
		cm_al->device_nr, cm_al->ar_nr,
		cm_al->alarm_type,
		cm_al->api, cm_al->slot_nr, cm_al->subslot_nr,
		cm_al->alarm_tag
		);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_copy (
	CM_UPPER_RQB_PTR_TYPE  cm,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	LSA_OPCODE_TYPE cm_opcode
)
{
	CM_LOG_FCT("cm_acp_alarm_copy")
	CM_UPPER_ALARM_PTR_TYPE  cm_al;
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al;

	cm_al = cm->args.channel.alarm;

	acp_al = & acp->args->alarm.data;

	CM_ASSERT(cm_al->alarm_priority == acp->args->alarm.data.priority);

	/***/

	cm_acp_alarm_copy_ack (cm, acp, cm_opcode);

	/***/

	cm_al->alarm_tag		= acp_al->user_tag;

	if (acp_al->user_length == 0) {
		cm_al->alarm_data_length = CM_ALARM_OFFSET_USI;
	}
	else {
		cm_al->alarm_data_length = CM_CAST_U16 (CM_ALARM_OFFSET_DATA + acp_al->user_length);
	}

		/* copy pointer to cm-user, clear acp pointer, alarm_rsp must free ! */
	cm_al->alarm_data		= acp_al->user_data;
	acp_al->user_data		= LSA_NULL;

	/***/

	CM_ACP_TRACE_07(0, LSA_TRACE_LEVEL_CHAT,
		"dev/ar(%u/%u) al_type(%u) api(%u) slot(%u) subslot(%u) alarm_tag(%u)",
		cm_al->device_nr, cm_al->ar_nr,
		cm_al->alarm_type,
		cm_al->api, cm_al->slot_nr, cm_al->subslot_nr,
		cm_al->alarm_tag
		);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_ack_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_acp_alarm_ack_indication")
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr;
	LSA_UINT16  prio;

	alarm_cr = (CM_AR_GRAPH_ALARM_CR_PTR_TYPE)acp->args->alarm.data.usr_id;
	if (is_null (alarm_cr)) {
		CM_ACP_LOWER_RQB_ERROR (acp);
		return;
	}

	prio = acp->args->alarm.data.priority;
	CM_ASSERT (prio == CM_ALARM_PRIORITY_LOW || prio == CM_ALARM_PRIORITY_HIGH);

	if( ! cm_ar_com_is_down (alarm_cr->cm.backlink) ) {

		CM_ASSERT (CM_ACP_GET_RESPONSE (acp) == ACP_OK);

		switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_CLIENTS
		case CM_PATH_TYPE_CLIENT:
			cm_cl_alarm_ack_indication (channel, alarm_cr->cm.backlink, prio, acp);
			break;
#endif
#if CM_CFG_MAX_SERVERS
		case CM_PATH_TYPE_SERVER:
			cm_sv_ar_almi_ack_indication (alarm_cr->cm.backlink, prio, acp);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}

		/* provide the resource again */
	CM_ASSERT (acp->args->alarm.data.priority == prio);

	cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*user-id*/, acp);
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_acp_alarm_ack_matches (
	CM_UPPER_RQB_PTR_TYPE  upper,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_LOG_FCT ("cm_acp_alarm_ack_matches")
	CM_UPPER_ALARM_PTR_TYPE  ad;
	LSA_UINT16  alarm_specifier;


	CM_ASSERT (CM_ACP_GET_RESPONSE (acp) == ACP_OK);


	ad = upper->args.channel.alarm;
#if CM_CFG_MAX_CLIENTS
	CM_ASSERT (are_equal (ad, upper->args.cl.alarm_send));
#endif
#if CM_CFG_MAX_SERVERS
	CM_ASSERT (are_equal (ad, upper->args.sv.ar_alarm_send));
#endif

	ad->cm_pnio_err = acp->args->alarm.data.pnio_status;

	CM_ACP_TRACE_07(0, LSA_TRACE_LEVEL_CHAT
			, "rsp(%u) type(%u) api(%u) slot(%u) sub(%u) seq(%u) pnio(%#x)"
			, CM_ACP_GET_RESPONSE (acp)
			, ad->alarm_type, ad->api, ad->slot_nr, ad->subslot_nr
			, ad->alarm_sequence, ad->cm_pnio_err
		);

		/* if header doesn't match: protocol-error */
	ACP_SET_ALARM_SPECIFIER (& alarm_specifier,
			ad->alarm_sequence, ad->diag_channel_available,
			ad->diag_generic_available, ad->diag_submod_available,
			ad->ar_diagnosis_state
		);

	if (! (ad->alarm_type == acp->args->alarm.data.alarm_type
		&&	ad->api == acp->args->alarm.data.api
		&&	ad->slot_nr == acp->args->alarm.data.slot_nr
		&&	ad->subslot_nr == acp->args->alarm.data.subslot_nr
		&&	alarm_specifier == acp->args->alarm.data.alarm_specifier))
	{
		CM_ACP_TRACE_10 (0, LSA_TRACE_LEVEL_UNEXP,
				"alarm-header doesn't match: type(%u/%u) api(%u/%u) slot(%u/%u) subslot(%u/%u) al_spec(0x%x/0x%x)",
				ad->alarm_type, acp->args->alarm.data.alarm_type,
				ad->api, acp->args->alarm.data.api,
				ad->slot_nr, acp->args->alarm.data.slot_nr,
				ad->subslot_nr, acp->args->alarm.data.subslot_nr,
				alarm_specifier, acp->args->alarm.data.alarm_specifier
			);
		return LSA_FALSE;
	}


	CM_ACP_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT, "ok");
	return LSA_TRUE;
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
