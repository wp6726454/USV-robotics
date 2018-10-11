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
/*  F i l e               &F: cm_pd5.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-PhysicalDevice things                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  850
#define CM_MODULE_ID       850

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/

static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_diag_equalize (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  alarm,
	LSA_OPCODE_TYPE  opcode,
	LSA_UINT16  max_alarm_data_length,
	CM_PD_PORT_PTR_TYPE  port,
	CM_LIST_ENTRY_PTR_TYPE  diag_list,
	CM_LIST_ENTRY_PTR_TYPE  merge_to
);

static CM_PD_DIAG_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diag_lookup (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,

	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT16  channel_properties_mask,
	LSA_UINT16  channel_properties
);


/*===========================================================================*/
/*=== alarm initiator init ==================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_almi_init (
	CM_CHANNEL_PTR_TYPE	channel
	)
{
	CM_LOG_FCT("cm_pd_almi_init")

	channel->usr.pd.almi.server_allowed = LSA_FALSE;
	channel->usr.pd.almi.server_max_alarm_data_length = 0; /* sanity */
	channel->usr.pd.almi.server_wf_alarm_rsp = LSA_FALSE;

	channel->usr.pd.almi.user_allowed = LSA_FALSE; /*AP00942517; was true*/
	channel->usr.pd.almi.user_sequence_nr = 0;

	channel->usr.pd.almi.appl_ready_ind_pending = CM_ERR_NOT_YET; /* TFS 1102312: was 0 */
}


/*===========================================================================*/
/*=== build alarm ===========================================================*/
/*===========================================================================*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_alarm_build (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  alarm,
	LSA_OPCODE_TYPE  opcode, /* CM_OPC_SV_AR_ALARM_PDEV_GROUP or CM_OPC_PD_ALARM_IND */
	LSA_UINT16  max_alarm_data_length,
	CM_PD_PORT_PTR_TYPE  port,
	CM_PD_DIAG_PTR_TYPE  diag
) {
	CM_LOG_FCT("cm_pd_alarm_build")
	CM_UPPER_ALARM_PTR_TYPE  param;
	CM_UPPER_MEM_U8_PTR_TYPE  alarm_data;
	LSA_UINT16  alarm_data_len;
	LSA_UINT16  block_length_offset;
	LSA_UINT16  usi_offset;
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;


	CM_ASSERT (cm_pd_port_is_mapped (port)); /* see unmap */

	switch (diag->channel_properties & CM_CHPR_SPEC_MASK) {
	case CM_CHPR_SPEC_APP:
		CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			,"appearing-alarm(%u): opcode(%u) submodule(0/%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x)"
			, diag->alarm_type
			, opcode, port->addr.slot_nr, port->addr.subslot_nr
			, diag->channel_error_type, diag->ext_channel_error_type
			, diag->channel_properties
			);
		break;

	case CM_CHPR_SPEC_DIS:
		CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			,"disappearing-alarm(%u): opcode(%u) submodule(0/%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x)"
			, diag->alarm_type
			, opcode, port->addr.slot_nr, port->addr.subslot_nr
			, diag->channel_error_type, diag->ext_channel_error_type
			, diag->channel_properties
			);
		break;

	case CM_CHPR_SPEC_DBO:
		CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			,"disappearing-but-other-remain-alarm(%u): opcode(%u) submodule(0/%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x)"
			, diag->alarm_type
			, opcode, port->addr.slot_nr, port->addr.subslot_nr
			, diag->channel_error_type, diag->ext_channel_error_type
			, diag->channel_properties
			);
		break;

	default:
		CM_FATAL1 (diag->channel_properties);
		return;
	}

	switch( opcode ) {

#if CM_CFG_MAX_SERVERS
	case CM_OPC_SV_AR_ALARM_PDEV_GROUP:

		param = alarm->args.sv.ar_alarm_send;
		CM_ASSERT (is_not_null (param));

		CM_RQB_SET_HANDLE (alarm, CM_INVALID_HANDLE);
		CM_RQB_SET_USER_ID_PTR (alarm, LSA_NULL);

		alarm_data_len = 0;
		alarm_data = param->alarm_data;
		break;
#endif

	case CM_OPC_PD_ALARM_IND:

		param = alarm->args.channel.alarm;
		CM_ASSERT (is_not_null (param));

		alarm_data_len = 0;
		alarm_data = param->alarm_data;
		break;

	default:
		CM_FATAL1 (opcode);
		return; /* unreacheable */
	}
	CM_ASSERT (is_not_null (alarm_data));

	CM_RQB_SET_OPCODE (alarm, opcode);

	/***/

	param->device_nr              = 0;           /* user: don't evaluate */
	param->ar_nr                  = 0;           /* user: don't evaluate */
	param->session_key            = pdprm->end_counter;
	param->api                    = 0; /*  */
	param->slot_nr                = port->addr.slot_nr;
	param->subslot_nr             = port->addr.subslot_nr;

	param->alarm_type             = diag->alarm_type;

	param->alarm_priority         = CM_ALARM_PRIORITY_LOW;
	param->alarm_sequence         = 0;			/* don't care */
	param->diag_channel_available = LSA_FALSE;	/* channel diagnosis available */
	param->diag_generic_available = 0;			/* don't care */
	param->diag_submod_available  = LSA_FALSE;	/* diagnosis available*/
	param->ar_diagnosis_state     = 0;			/* don't care */
	param->reserved               = 0;			/* don't care */
	param->mod_ident              = port->addr.mod_ident;	/* AP00329730 */
	param->sub_ident              = port->addr.sub_ident;	/* AP00329730 */
	param->cm_pnio_err            = 0;			/* don't care */

	cm_pd_diag_maint_qual (port, & param->diag_submod_available, & param->maintenance_status);

	if (param->diag_submod_available) {
		param->diag_channel_available = LSA_TRUE;
	}

/*	param->alarm_tag = 0; // see below */

	/*
	//	AlarmNotification-PDU ::=
	//		BlockHeader, AlarmType, API, SlotNumber, SubslotNumber
	//		, ModuleIdentNumber, SubmoduleIdentNumber, AlarmSpecifier
	//		, [MaintenanceItem], [AlarmItem]
	//
	//	MaintenanceItem ::=
	//		BlockType(0x0F00), BlockLength, BlockVersion(1.0), ReservedU16, MaintenanceStatusU32
	*/

	/* if CM-PD-alarm: simulate alarm-PDU-Header */

	if (opcode == CM_OPC_PD_ALARM_IND) {

		LSA_UINT16 alarm_specifier;

		param->alarm_sequence = channel->usr.pd.almi.user_sequence_nr; /* AP00632258 */
		CM_ALARM_SEQUENCE_NR_INCREMENT (channel->usr.pd.almi.user_sequence_nr);

		ACP_SET_ALARM_SPECIFIER (& alarm_specifier
			, param->alarm_sequence
			, param->diag_channel_available
			, param->diag_generic_available
			, param->diag_submod_available
			, param->ar_diagnosis_state
			);

		CM_PUT16_HTON (alarm_data, alarm_data_len, 0x0002); /* ACP_ALARM_BLOCK_REQUEST_LOW */
		alarm_data_len += 2;

		block_length_offset = alarm_data_len;
		CM_PUT16_HTON (alarm_data, alarm_data_len, 0xCCCC); /* block_length, see below */
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, 0x0100); /* ACP_ALARM_BLOCK_VERSION */
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, diag->alarm_type);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, param->api);
		alarm_data_len += 4;

		CM_PUT16_HTON (alarm_data, alarm_data_len, param->slot_nr);
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, param->subslot_nr);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, param->mod_ident);
		alarm_data_len += 4;

		CM_PUT32_HTON (alarm_data, alarm_data_len, param->sub_ident);
		alarm_data_len += 4;

		CM_PUT16_HTON (alarm_data, alarm_data_len, alarm_specifier);
		alarm_data_len += 2;

		if (param->maintenance_status != 0) {

			param->alarm_tag = CM_ALARM_USI_MAINTENANCE;

			CM_PUT16_HTON (alarm_data, alarm_data_len, CM_ALARM_USI_MAINTENANCE);
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, CM_BLOCK_TYPE_MAINTENANCE);
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, 2+2+4); /* block_length */
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, 0x0100); /* block_version */
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, 0); /* reserved */
			alarm_data_len += 2;

			CM_PUT32_HTON (alarm_data, alarm_data_len, param->maintenance_status);
			alarm_data_len += 4;

			usi_offset = alarm_data_len;
			CM_PUT16_HTON (alarm_data, alarm_data_len, 0xCCCC); /* USI, see below */
			alarm_data_len += 2;
		}
		else {

			param->alarm_tag = 0;

			usi_offset = alarm_data_len;
			CM_PUT16_HTON (alarm_data, alarm_data_len, 0xCCCC); /* USI, see below */
			alarm_data_len += 2;
		}
	}
	else {
		block_length_offset = 0;
		param->alarm_tag = 0;
		usi_offset = 0;
	}

	/*	AlarmItem ::=
	//		UserStructureIdentifier(=0x8000), ChannelDiagnosisData*
	//		or
	//		UserStructureIdentifier(=0x8002), ExtChannelDiagnosisData*
	//
	//	ChannelDiagnosisData ::=
	//		ChannelNumber, ChannelProperties, ChannelErrorType
	//
	//	ExtChannelDiagnosisData ::=
	//		ChannelNumber, ChannelProperties, ChannelErrorType
	//		, ExtChannelErrorType, ExtChannelAddValue
	*/

	CM_PUT16_HTON (alarm_data, alarm_data_len, 0x8000/*ChannelNumber = subslot*/);
	alarm_data_len += 2;

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("REVIEW: when to use ChannelProperties.Specifier 'All subsequent disappears'?")
#endif /* CM_MESSAGE */

	CM_PUT16_HTON (alarm_data, alarm_data_len, diag->channel_properties);
	alarm_data_len += 2;

	CM_PUT16_HTON (alarm_data, alarm_data_len, diag->channel_error_type);
	alarm_data_len += 2;

	if ((diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP) {

		if (param->alarm_tag == 0) {
			param->alarm_tag = 0x8002; /* UserStructureIdentifier */
		}

		if (usi_offset != 0) {
			CM_PUT16_HTON (alarm_data, usi_offset, 0x8002);
		}

		CM_PUT16_HTON (alarm_data, alarm_data_len, diag->ext_channel_error_type);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, diag->ext_channel_add_value);
		alarm_data_len += 4;
	}
	else { /* disappearing */

		if (param->alarm_tag == 0) {
			param->alarm_tag = 0x8002; /* UserStructureIdentifier */
		}

		if (usi_offset != 0) {
			CM_PUT16_HTON (alarm_data, usi_offset, 0x8002);
		}

		CM_PUT16_HTON (alarm_data, alarm_data_len, diag->ext_channel_error_type);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, diag->ext_channel_add_value);
		alarm_data_len += 4;
	}

	/***/

	if (alarm_data_len > max_alarm_data_length) {
		CM_FATAL1 (alarm_data_len); /* caller must check */
	}

	if (block_length_offset != 0) {
		CM_PUT16_HTON (alarm_data, block_length_offset, CM_CAST_U16(alarm_data_len - (2+2))); /* block without (block_type,block_length) */
	}

	param->alarm_data_length      = alarm_data_len;

	return;
}


/*===========================================================================*/
/*=== Alarm Provide =========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_alarm_provide_event (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_pd_alarm_provide_event")

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_PD);

	if (channel->usr.pd.req.alarm_ind_count == 0)
	{
		if (channel->usr.pd.req.delete_is_in_queue
		&&	is_not_null (channel->usr.pd.req.current)
		&&	CM_RQB_GET_OPCODE (channel->usr.pd.req.current) == CM_OPC_PD_DELETE)
		{
			if (channel->state == CM_PD_READY)
			{
				cm_pd_delete (channel, channel->usr.pd.req.current);
			}
			return;
		}
	}

	if (channel->usr.pd.almi.user_allowed) {
		LSA_UINT16  i;

		for (i = 0; i <= channel->usr.pd.port_count; ++i) {
			CM_PD_PORT_PTR_TYPE  port;

			port = cm_pd_port_from_id__nn (channel, i);

			cm_pd_diag_update_check (channel, port);
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_alarm_provide (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_alarm_provide")

	CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "IN : rb(%08x)", rb
	);

	CM_ASSERT(CM_RQB_GET_HANDLE(rb) == channel->my_handle);

	CM_ASSERT(
		CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_ALARM_IND
		|| CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_ALARM_RSP
	);

	CM_ASSERT(rb->args.channel.alarm->alarm_priority < 2);

	/***/

	CM_RQB_SET_OPCODE(rb, CM_OPC_ALARM_IND_RES_PROVIDE);

	if( is_not_null(rb->args.channel.alarm->alarm_data) ) {

		LSA_UINT16 rc;

			/* must be alloced buffer from alarm_ind */
		CM_FREE_UPPER_MEM (& rc,
				rb->args.channel.alarm->alarm_data, channel->sysptr
			);
		CM_ASSERT (rc == LSA_RET_OK);

		rb->args.channel.alarm->alarm_data = LSA_NULL;
		rb->args.channel.alarm->alarm_data_length = 0;
	}
	else {
		CM_ASSERT(0);
	}

	cm_alarm_provide (channel, rb);

	cm_pd_alarm_provide_event (channel);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_alarm_user_enable (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  user_enable
) {
	CM_LOG_FCT("cm_pd_alarm_user_enable")

	if (user_enable)
	{
		if (channel->usr.pd.req.delete_is_in_queue)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "want PD-user alarm.ind but also want to close, do not enable"
				);
		}
		else
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "enable PD-user alarm.ind");
			channel->usr.pd.almi.user_allowed = LSA_TRUE;
		}
		cm_pd_alarm_provide_event (channel);
	}
	else
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "disable PD-user alarm.ind"
			);

		channel->usr.pd.almi.user_allowed = LSA_FALSE;
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_move_to_user_diag (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_pd_diag_move_to_user_diag")
	LSA_UINT16  port_id;

	for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id) {
		CM_PD_PORT_PTR_TYPE  port;
		CM_PD_DIAG_PTR_TYPE  diag;

		port = cm_pd_port_from_id__nn (channel, port_id);

		while (! CmListIsEmpty (& port->diag.diag_list_user))
		{
			diag = CmListFirst (& port->diag.diag_list_user, CM_PD_DIAG_PTR_TYPE);
			cm_pd_diag_update_remove (& port->diag.diag_list_user, diag);
		}

		for (CmListForeach (diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE))
		{
			cm_pd_diag_update_add (& port->diag.diag_list_user
				, diag->channel_properties
				, diag->channel_error_type, diag->ext_channel_error_type
				, diag->ext_channel_add_value, diag->alarm_type
				);
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_move_to_server_diag (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_pd_diag_move_to_server_diag")
	LSA_UINT16  port_id;

	for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id) {
		CM_PD_PORT_PTR_TYPE  port;
		CM_PD_DIAG_PTR_TYPE  diag;

		port = cm_pd_port_from_id__nn (channel, port_id);

		while (! CmListIsEmpty (& port->diag.diag_list_server))
		{
			diag = CmListFirst (& port->diag.diag_list_server, CM_PD_DIAG_PTR_TYPE);
			cm_pd_diag_update_remove (& port->diag.diag_list_server, diag);
		}

		for (CmListForeach (diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE))
		{
			cm_pd_diag_update_add (& port->diag.diag_list_server
				, diag->channel_properties
				, diag->channel_error_type, diag->ext_channel_error_type
				, diag->ext_channel_add_value, diag->alarm_type
				);
		}
	}
}

/*===========================================================================*/
/*=== DIAG LIST =============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_start (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
) {
	CM_LOG_FCT("cm_pd_diag_update_start")

	LSA_UNUSED_ARG (channel);

	CM_ASSERT (port->diag.update_allowed);
	port->diag.update_allowed = LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
) {
	CM_LOG_FCT("cm_pd_diag_update_done")

	LSA_UNUSED_ARG (channel);

	CM_ASSERT (! port->diag.update_allowed);
	port->diag.update_allowed = LSA_TRUE;

#if CM_DEBUG
	{
	CM_PD_DIAG_PTR_TYPE  diag;

	CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "diag_list for port(%u)", port->port_id
		);
	for (CmListForeach (diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE)) {
		CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "   ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x) add_value(0x%x) alarm_type(%u)"
			, diag->channel_error_type, diag->ext_channel_error_type
			, diag->channel_properties
			, diag->ext_channel_add_value
			, diag->alarm_type
			);
	}
	}
#endif

	/***/

#if CM_CFG_MAX_SERVERS
	cm_sv_pdev_diag_changed(channel, port->port_id);
#endif

	/***/

	cm_pd_diag_update_check (channel, port);
}


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_SERVERS
static CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdsv_alarm_upper_alloc__nn (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_pd_alarm_upper_alloc__nn")
	CM_UPPER_RQB_PTR_TYPE  alarm;

	alarm = cm_upper_alloc_rqb (channel);
	if (is_null (alarm)) {
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "no mem, cm_upper_alloc_rqb()");
		CM_FATAL ();
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& alarm->args.sv.ar_alarm_send->alarm_data)
		, cm_null_user_id, channel->usr.pd.almi.server_max_alarm_data_length, channel->sysptr
		);
	if (is_null (alarm->args.channel.alarm->alarm_data)) {
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "no mem, max_alarm_data_length(%u)"
			, channel->usr.pd.almi.server_max_alarm_data_length
			);
		CM_FATAL ();
	}

	return alarm;
}
#endif


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_check (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
) {
	CM_LOG_FCT("cm_pd_diag_update_check")

	if (! port->diag.update_allowed) {
		return;
	}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: argument which tell us the checking part: sv, pd or both; see caller")
#endif /* CM_MESSAGE */


#if CM_CFG_MAX_SERVERS
	if (channel->usr.pd.almi.server_allowed) {

		if (channel->usr.pd.almi.server_wf_alarm_rsp) {
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "server_allowed for PDevAlarm.ind, but waiting for cm_pdsv_alarm_rsp()"
				);
		}
		else {
			CM_UPPER_RQB_PTR_TYPE  alarm;
			LSA_UINT16  rc;
			LSA_BOOL  again;

			alarm = cm_pdsv_alarm_upper_alloc__nn (channel);

			do {
				alarm->args.sv.ar_alarm_send->alarm_data_length = 0; /* but alarm_data != 0 */

				rc = cm_pd_diag_equalize (channel
					, alarm, CM_OPC_SV_AR_ALARM_PDEV_GROUP, channel->usr.pd.almi.server_max_alarm_data_length
					, port, & port->diag.diag_list, & port->diag.diag_list_server
					);

				if (rc == CM_OK) {
					LSA_BOOL wait;

						/* thinking: a flag for recursion prevention, see current fatal in cm_pdsv_alarm_rsp() */

					wait = cm_sv_pdev_alarm_ind (channel
						, channel->usr.pd.pdprm.device_nr, alarm
						);

					if (wait) {
						again = LSA_FALSE;
						channel->usr.pd.almi.server_wf_alarm_rsp = LSA_TRUE;
						/* see cm_pdsv_alarm_rsp() */
					}
					else {
						/* next difference, same rqb */
						again = LSA_TRUE;
					}
				}
				else {
					/* no difference found */
					CM_ASSERT (rc == CM_OK_CANCELLED);
					CM_FREE_UPPER_MEM (& rc, alarm->args.sv.ar_alarm_send->alarm_data, channel->sysptr);
					CM_ASSERT (rc == LSA_RET_OK);
					cm_upper_free_rqb (channel, alarm);
					again = LSA_FALSE;
				}

				if (again  &&  ! channel->usr.pd.almi.server_allowed) {
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("PD-REVIEW: there is a recursion via alarm-send / down / cm_sv_ar_almi_alarm_confirmation()")
#endif /* CM_MESSAGE */
					CM_FREE_UPPER_MEM (& rc, alarm->args.sv.ar_alarm_send->alarm_data, channel->sysptr);
					CM_ASSERT (rc == LSA_RET_OK);
					cm_upper_free_rqb (channel, alarm);
					again = LSA_FALSE;
				}
			} while (again);
		}
	}
#endif

	if (channel->usr.pd.almi.user_allowed) {

		CM_UPPER_RQB_PTR_TYPE  alarm;
		LSA_UINT16  rc;

		if (! cm_pd_port_is_mapped (port)) {
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "port(%u): no mapping exists", port->port_id
				);
			return;
		}

		alarm = cm_res_get_alarm (channel, CM_ALARM_PRIORITY_LOW);

		if (is_not_null (alarm)) {

			CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& alarm->args.channel.alarm->alarm_data)
				, cm_null_user_id, channel->acp.max_alarm_data_length, channel->sysptr
				);
			if (is_null (alarm->args.channel.alarm->alarm_data)) {
				CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "no alarm_mem, size(%u)", channel->acp.max_alarm_data_length);
				cm_alarm_provide (channel, alarm);
				alarm = LSA_NULL;
			}
		}

		if (is_null (alarm)) {
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "no indication resource for alarm to user"
				);
			return;
		}

		alarm->args.channel.alarm->alarm_data_length = 0; /* but alarm_data != 0 */

		rc = cm_pd_diag_equalize (channel
			, alarm, CM_OPC_PD_ALARM_IND, channel->acp.max_alarm_data_length
			, port, & port->diag.diag_list, & port->diag.diag_list_user
			);

		if (rc == CM_OK) {
			channel->usr.pd.req.alarm_ind_count += 1;
			cm_callback (channel, CM_OK, & alarm);
			/* see cm_pd_alarm_provide() */
		}
		else {

			CM_ASSERT (rc == CM_OK_CANCELLED);
			CM_FREE_UPPER_MEM (& rc, alarm->args.channel.alarm->alarm_data, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
			alarm->args.channel.alarm->alarm_data = LSA_NULL;
			cm_alarm_provide (channel, alarm);
		}
	}
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_diag_equalize (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  alarm,
	LSA_OPCODE_TYPE  opcode,
	LSA_UINT16  max_alarm_data_length,
	CM_PD_PORT_PTR_TYPE  port,
	CM_LIST_ENTRY_PTR_TYPE  diag_list,
	CM_LIST_ENTRY_PTR_TYPE  merge_to
) {
	CM_LOG_FCT("cm_pd_diag_equalize")
	CM_PD_DIAG_PTR_TYPE  diag;

	CM_PD_DIAG_PTR_TYPE  add_to_merge;
	CM_PD_DIAG_PTR_TYPE  remove_from_merge;
	CM_PD_DIAG_PTR_TYPE  more_diag;

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: Sammelmeldung, see 22. conclave")
/*
Sammelmeldung:
Die Subkomponenten liefern alle Aenderungen in EINER Indikation an den CMPD.
Der CMPD gene-riert aus dieser Indikation einen ChannelErrorType-richtigen(AlarmType)
Alarm abhaengig von der Kapazitaet des Transportkanals. Sollten mehrere ChannelErrorTypes
in dieser Indication geliefert werden, so werden auch mehrere ChannelErrorType-richtige
Alarme geliefert.
Allgemein ist dabei eine Zusammenfassung der Diagnoseobjekte ueber mehrere Indikations zur
Re-duzierung der Alarmlast sinnvoll.
*/
#endif /* CM_MESSAGE */

	add_to_merge = LSA_NULL;
	remove_from_merge = LSA_NULL;
	more_diag = LSA_NULL;

	for (CmListForeach (diag, diag_list, CM_PD_DIAG_PTR_TYPE)) {
		CM_PD_DIAG_PTR_TYPE  e;

		e = cm_pd_diag_lookup (merge_to
			, diag->channel_error_type, diag->ext_channel_error_type
			, CM_CHPR_MAINT_MASK, diag->channel_properties
			);
		if (is_not_null (e)) {
			/* ok */
		}
		else {
			if (is_null (add_to_merge)) {
				add_to_merge = diag;
				break;
			}
		}
	}

	if (is_null (add_to_merge)) {
		for (CmListForeach (diag, merge_to, CM_PD_DIAG_PTR_TYPE)) {
			CM_PD_DIAG_PTR_TYPE  e;

			e = cm_pd_diag_lookup (diag_list
				, diag->channel_error_type, diag->ext_channel_error_type
				, CM_CHPR_MAINT_MASK, diag->channel_properties
				);
			if (is_not_null (e)) {
				more_diag = diag; /* an appearing diag exists within diag_list and merge_to */
			}
			else {
				if (is_null (remove_from_merge)) {
					remove_from_merge = diag;
				}
				else {
					more_diag = diag; /* because we can not notify this disappering diag now, we have to signal other remains */
					break;
				}
			}
		}
	}

	if (is_not_null (add_to_merge)) {

		CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "port(%u): add_to_merge(0x%x/0x%x)"
			, port->port_id
			, add_to_merge->channel_error_type, add_to_merge->ext_channel_error_type
			);

			/* alarm add_to_merge */
		cm_pd_alarm_build (channel
			, alarm, opcode, max_alarm_data_length
			, port, add_to_merge
			);

			/* add_to_merge */
		cm_pd_diag_update_add (merge_to
			, add_to_merge->channel_properties
			, add_to_merge->channel_error_type, add_to_merge->ext_channel_error_type
			, add_to_merge->ext_channel_add_value, add_to_merge->alarm_type
			);
		return CM_OK;
	}

	else if (is_not_null (remove_from_merge)) {

			/* alarm remove_from_merge + DISAPPEARING */
		if (is_null (more_diag)) {
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "port(%u): remove_from_merge(0x%x/0x%x)  no more_diag"
				, port->port_id
				, remove_from_merge->channel_error_type
				, remove_from_merge->ext_channel_error_type
				);

			remove_from_merge->channel_properties = CM_CAST_U16 (
				(remove_from_merge->channel_properties & ~ CM_CHPR_SPEC_MASK)
				| CM_CHPR_SPEC_DIS
				);
		}

			/* alarm remove_from_merge + DISAPPEARING but other errors remain [AP00627740] */
		else {
			CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "port(%u): remove_from_merge(0x%x/0x%x)  more_diag(0x%x/0x%x)"
				, port->port_id
				, remove_from_merge->channel_error_type
				, remove_from_merge->ext_channel_error_type
				, more_diag->channel_error_type
				, more_diag->ext_channel_error_type
				);

			remove_from_merge->channel_properties = CM_CAST_U16 (
				(remove_from_merge->channel_properties & ~ CM_CHPR_SPEC_MASK)
				| CM_CHPR_SPEC_DBO
				);
		}

		cm_pd_alarm_build (channel
			, alarm, opcode, max_alarm_data_length
			, port, remove_from_merge
			);

			/* remove_from_merge */
		cm_pd_diag_update_remove (merge_to, remove_from_merge);
		return CM_OK;
	}

	else {
		/* list's are equal */

		if (is_not_null (more_diag)) {
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "port(%u): more_diag(0x%x/0x%x) exists"
				, port->port_id
				, more_diag->channel_error_type, more_diag->ext_channel_error_type
				);
		}
		else {
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "port(%u): no more_diag exists"
				, port->port_id
				);
		}

		return CM_OK_CANCELLED;
	}
}


/*----------------------------------------------------------------------------*/
static CM_PD_DIAG_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diag_lookup (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,

	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT16  channel_properties_mask,
	LSA_UINT16  channel_properties
) {
	CM_LOG_FCT("cm_pd_diag_lookup")

	CM_PD_DIAG_PTR_TYPE  diag;

	channel_properties &= channel_properties_mask;

	for (CmListForeach (diag, diag_list, CM_PD_DIAG_PTR_TYPE))
	{
		if (channel_error_type != 0
		&&	channel_error_type != diag->channel_error_type)
		{
			continue;
		}

		if (ext_channel_error_type != 0
		&&	ext_channel_error_type != diag->ext_channel_error_type)
		{
			continue;
		}

		if (channel_properties_mask != 0
		&&	channel_properties != (diag->channel_properties & channel_properties_mask))
		{
			continue;
		}
		return diag;
	}
	return LSA_NULL; /* not found */
}

/*----------------------------------------------------------------------------*/
CM_PD_DIAG_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_add (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,

	LSA_UINT16  channel_properties,
	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT32  ext_channel_add_value,
	LSA_UINT16  alarm_type
) {
	CM_LOG_FCT("cm_pd_diag_update_add")
	CM_PD_DIAG_PTR_TYPE  diag;

	CM_ASSERT (sizeof(*diag) < 0xFFFF); /* LSA-macros can handle only 64K */

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(&diag), CM_CAST_U16(sizeof(*diag)));
	if (is_null (diag)) {
		CM_PD_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL, "cannot alloc-local, size(%u)", sizeof(*diag));
		CM_FATAL();
		return LSA_NULL;
	}

	/* key == ChErr + ExtChErr + Maint */
	diag->alarm_type				= alarm_type;
	diag->channel_properties		= channel_properties;
	diag->channel_error_type		= channel_error_type;
	diag->ext_channel_error_type	= ext_channel_error_type;
	diag->ext_channel_add_value		= ext_channel_add_value;

	CmListInsertTail (diag_list, & diag->link);

	return diag;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_remove (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,
	CM_PD_DIAG_PTR_TYPE  diag_rm
) {
	CM_LOG_FCT("cm_pd_diag_update_remove")

	CM_ASSERT (is_not_null (diag_rm));

	LSA_UNUSED_ARG (diag_list);
#if CM_DEBUG
	{
	CM_PD_DIAG_PTR_TYPE diag;

	for (CmListForeach (diag, diag_list, CM_PD_DIAG_PTR_TYPE)) {
		if (are_equal (diag, diag_rm)) {
			break;
		}
	}
	CM_ASSERT (is_not_null (diag));
	}
#endif

	CmListRemoveEntry (& diag_rm->link);
	{
	LSA_UINT16 rc;
	CM_FREE_LOCAL_MEM (& rc, diag_rm);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG (rc);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port,

	LSA_UINT16  channel_properties,
	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT32  ext_channel_add_value
	)
{
	CM_LOG_FCT("cm_pd_diag_update")
	CM_PD_DIAG_PTR_TYPE  diag;
	LSA_BOOL  appearing;
	LSA_UINT16  alarm_type;

	LSA_UNUSED_ARG (channel);

	switch (channel_properties & CM_CHPR_SPEC_MASK) {
	case CM_CHPR_SPEC_APP:
		appearing = LSA_TRUE;
		break;

	case CM_CHPR_SPEC_DIS:
		appearing = LSA_FALSE;
		break;

	case CM_CHPR_SPEC_RSVD:
	case CM_CHPR_SPEC_DBO:
	default:
		CM_FATAL1 (channel_properties);
		return;
	}

	switch (channel_error_type) {
	case 0x8000: /* Data transmission impossible */
		alarm_type = CM_ALARM_TYPE_PORT_DATA_CHANGED;
		break;

	case 0x8001: /* Remote mismatch */
		alarm_type = CM_ALARM_TYPE_PORT_DATA_CHANGED;
		break;

	case 0x8002: /* Media redundancy mismatch */
		alarm_type = CM_ALARM_TYPE_MEDIA_REDUNDANCY;
		break;

	case 0x8003: /* Sync mismatch */
		alarm_type = CM_ALARM_TYPE_SYNC_DATA_CHANGED;
		break;

	case 0x8007: /* Fiber optic mismatch */
		alarm_type = CM_ALARM_TYPE_PORT_DATA_CHANGED;
		break;

	case 0x8008: /* Network component function mismatch */
		alarm_type = CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM;
		break;

	case 0x8009: /* Time mismatch */
		alarm_type = CM_ALARM_TYPE_TIME_DATA_CHANGED;
		break;

	case 0x800B: /* Media redundancy with planned duplication mismatch */
		alarm_type = CM_ALARM_TYPE_MRPD_PROBLEM;
		break;

	/* note: update cm_alarm_type_is_internal() when adding new alarm types */
	case 0x800D: /* Multiple interface mismatch */
		alarm_type = CM_ALARM_TYPE_MULTIPLE_INTERFACE;
		break;

	case 0x800A: /* Dynamic frame packing function mismatch */
		alarm_type = CM_ALARM_TYPE_DFP_PROBLEM;
		break;

	case 0x8004: /* IsochronousMode mismatch */
	case 0x8005: /* Multicast CR mismatch */
	case 0x8006: /* reserved */
	default:
		CM_FATAL1(channel_error_type);
		return;
	}

	CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "port(%u): appearing(%u) alarm_type(%u)  ch_prop(0x%x) ch_err_type(0x%x) ext_ch_err_type(0x%x) ext_ch_add_value(0x%x)"
		, port->port_id, appearing, alarm_type
		, channel_properties, channel_error_type
		, ext_channel_error_type, ext_channel_add_value
		);

	diag = cm_pd_diag_lookup (& port->diag.diag_list
		, channel_error_type, ext_channel_error_type, CM_CHPR_MAINT_MASK, channel_properties
		);

	if (appearing) {

			/* old exists */
		if (is_not_null(diag)) {

			/* replace values */

			CM_ASSERT (diag->alarm_type == alarm_type);

			diag->channel_properties     = channel_properties;
			diag->channel_error_type     = channel_error_type;
			diag->ext_channel_error_type = ext_channel_error_type;
			diag->ext_channel_add_value  = ext_channel_add_value;
		}

			/* old does not exit */
		else {

			/* alloc and insert new diagnosis */
			diag = cm_pd_diag_update_add (& port->diag.diag_list
				, channel_properties
				, channel_error_type, ext_channel_error_type
				, ext_channel_add_value, alarm_type
				);
			CM_ASSERT (is_not_null (diag));
		}
	}

		/* disappearing */
	else {

			/* old exists */
		if (is_not_null(diag) ) {

			cm_pd_diag_update_remove (& port->diag.diag_list, diag);
		}

			/* old does not exist */
		else {

			CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "port(%u): appearing(%u) alarm_type(%u)  ch_prop(0x%x) ch_err_type(0x%x) ext_ch_err_type(0x%x) ext_ch_add_value(0x%x)"
				, port->port_id, appearing, alarm_type
				, channel_properties, channel_error_type
				, ext_channel_error_type, ext_channel_add_value
				);

			CM_FATAL(); /* a bug in the subcomponent */
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_clear (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
	)
{
	CM_LOG_FCT("cm_pd_diag_clear")

	LSA_UNUSED_ARG (channel);

	while (! CmListIsEmpty (& port->diag.diag_list))
	{
		CM_PD_DIAG_PTR_TYPE diag = CmListFirst (& port->diag.diag_list, CM_PD_DIAG_PTR_TYPE);

		cm_pd_diag_update_remove (& port->diag.diag_list, diag);
	}
}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("REFACTOR: make a clear-diag_list for User and Server too (now many while/remove loops)")
/*search for loops with cm_pd_diag_update_remove*/
#endif /* CM_MESSAGE */

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_maint_qual (
	CM_PD_PORT_PTR_TYPE  port,
	LSA_BOOL CM_LOCAL_MEM_ATTR  *diag_status_ptr,
	LSA_UINT32 CM_LOCAL_MEM_ATTR  *maint_status_ptr
) {
	CM_LIST_ENTRY_PTR_TYPE diag_list;
	CM_PD_DIAG_PTR_TYPE diag;
	LSA_BOOL diag_status;
	LSA_UINT32 maint_status;

	diag_list = &port->diag.diag_list;

	CM_ASSERT(is_not_null(diag_list));

	diag_status  = LSA_FALSE;
	maint_status = 0;

	for( CmListEach(diag, diag_list, CM_PD_DIAG_PTR_TYPE) ) {

		if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

			switch( (diag->channel_properties & CM_CHPR_MAINT_MASK) ) {

			case CM_CHPR_MAINT_ERR:
				diag_status = LSA_TRUE;
				break;

			case CM_CHPR_MAINT_REQ:
				maint_status |= CM_MAINTENANCE_STATUS_REQUIRED;
				break;

			case CM_CHPR_MAINT_DEM:
				maint_status |= CM_MAINTENANCE_STATUS_DEMANDED;
				break;

			case CM_CHPR_MAINT_QUA:
				CM_FATAL(); /* the pdev does not have qualified diagnoses (22. conclave) */
				break;

			default:
				CM_FATAL(); /* a bug */
				break;
			}
		}
		else {

			/* don't want to know */
		}
	}

	/***/

	*diag_status_ptr = diag_status;
	*maint_status_ptr = maint_status;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_set_topo_state (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
) {
	CM_LOG_FCT ("cm_pd_set_topo_state")
	LSA_BOOL  topo_mismatch;
	LSA_BOOL  delta;

	CM_ASSERT (port->port_id != 0);

	topo_mismatch = LSA_FALSE;

	{
	CM_PD_DIAG_PTR_TYPE  diag;

	for (CmListForeach (diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE))
	{
		if (diag->channel_error_type == OHA_DATA_TRANSMISSION_IMPOSSIBLE /* 0x8000, data transmission impossible */)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "channel_error_type(0x8000)/ext(0x%x) data transmission impossible found => topo mismatch"
				, diag->ext_channel_error_type
				);
			topo_mismatch = LSA_TRUE;
			break;
		}

		if (diag->channel_error_type == OHA_REMOTE_MISMATCH /* 0x8001, remote mismatch */)
		{
			if (diag->ext_channel_error_type == OHA_PEER_RT_CLASS_3_MISMATCH /* 0x8002, RT Class 3 mismatch */)
			{
				/* must ignore, because we would repair 'RTClass3 mismatch', see cm_pd_irt_condition_changed() */
			}
			else if (diag->ext_channel_error_type == OHA_PEER_PTCP_MISMATCH /* 0x8008, PTCP mismatch */)
			{
				/* ignore? mismatch comes after ~300ms. the synchronation needs ~1sec ... */
			}
			else
			{
				CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "channel_error_type(0x8001)/ext(0x%x) remote mismatch found => topo mismatch"
					, diag->ext_channel_error_type
					);
				topo_mismatch = LSA_TRUE;
				break;
			}
		}
	}
	}

	delta = LSA_FALSE;

	switch (port->misc.topo_state)
	{
	case CM_PD_TOPO_STATE_UNKNOWN:
		if (topo_mismatch) {
			port->misc.topo_state = CM_PD_TOPO_STATE_NOK;
		}
		else {
			port->misc.topo_state = CM_PD_TOPO_STATE_OK;
		}
		delta = LSA_TRUE;
		break;

	case CM_PD_TOPO_STATE_OK:
		if (topo_mismatch) {
			port->misc.topo_state = CM_PD_TOPO_STATE_NOK;
			delta = LSA_TRUE;
		}
		break;

	case CM_PD_TOPO_STATE_NOK:
		if (! topo_mismatch) {
			port->misc.topo_state = CM_PD_TOPO_STATE_OK;
			delta = LSA_TRUE;
		}
		break;

	default:
		CM_FATAL();
		break;
	}

	if( delta ) {

		LSA_UNUSED_ARG (channel); /* CM-SV only */

		CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"signalling port(%u) topo_state(%u)"
			, port->port_id, port->misc.topo_state
			);

#if CM_CFG_MAX_CLIENTS
		{
		CM_CHANNEL_PTR_TYPE cl_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_CLIENT);

		if (is_not_null(cl_channel)) {

			cm_cl_topo_changed(cl_channel);
		}
		}
#endif

		/***/

#if CM_CFG_MAX_MULTICAST_CHANNELS
		{
		CM_CHANNEL_PTR_TYPE mc_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_MULTICAST);

		if (is_not_null(mc_channel)) {

			cm_mc_pdev_changed(mc_channel);
		}
		}
#endif

	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
