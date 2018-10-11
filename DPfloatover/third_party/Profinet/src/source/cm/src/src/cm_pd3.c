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
/*  F i l e               &F: cm_pd3.c                                  :F&  */
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

#define LTRC_ACT_MODUL_ID  830
#define CM_MODULE_ID       830

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/
/*=== PD Is Ready ===========================================================*/
/*===========================================================================*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_ready (
	CM_CHANNEL_PTR_TYPE usr_channel
) {
	CM_LOG_FCT("cm_pdusr_is_ready")
	CM_CHANNEL_PTR_TYPE channel;

	/* context-switch from CM-CL/MC/SV to CM-PD */

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	if (channel->state != CM_PD_READY) {

		CM_PD_TRACE_01(usr_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			,"cm-pd is not ready, state(%u)"
			, channel->state
			);
		return LSA_FALSE;
	}

	CM_PD_TRACE_00(usr_channel->trace_idx,LSA_TRACE_LEVEL_CHAT, "cm-pd is ready");
	return LSA_TRUE;
}


/*===========================================================================*/
/*=== PD SendClock ==========================================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_parameterized (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 CM_LOCAL_MEM_ATTR *send_clock,
	LSA_BOOL CM_LOCAL_MEM_ATTR *is_reduction_ratio_fixed
) {
	CM_LOG_FCT("cm_pdusr_is_parameterized")
	CM_CHANNEL_PTR_TYPE  channel;

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	if( channel->usr.pd.pdprm.is_parameterized ) {

		*send_clock = channel->usr.pd.clock.send_clock;

		*is_reduction_ratio_fixed = cm_pd_sendclock_prop_rr_is_fixed(channel);

		return LSA_TRUE;
	}
	else {

		*send_clock = 0;

		*is_reduction_ratio_fixed = LSA_FALSE;

		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pdusr_is_send_clock_fixed ( /* returns the send_clock if "fixed", 0 otherwise */
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_BOOL CM_LOCAL_MEM_ATTR *is_reduction_ratio_fixed
) {
	CM_LOG_FCT("cm_pdusr_is_send_clock_fixed")
	CM_CHANNEL_PTR_TYPE channel;

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	*is_reduction_ratio_fixed = cm_pd_sendclock_prop_rr_is_fixed(channel);

	/***/

	if( cm_pd_sendclock_prop_sc_is_fixed (channel) ) {

		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "yes, send_clock is fixed by sendclock_prop"
			);

		return channel->usr.pd.clock.send_clock;
	}
	else if( channel->usr.pd.clock.ar_cnt != 0 ) {

		CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "yes, send_clock is fixed by an existing ar, ar_cnt(%u)"
			, channel->usr.pd.clock.ar_cnt
			);

		return channel->usr.pd.clock.send_clock;
	}
	else {

		CM_PD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "no, send_clock is not fixed"
			);

		return 0; /* cm_pdusr_attach() with another send_clock will succeed */
	}
}


/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pdusr_send_clock_get (
	CM_CHANNEL_PTR_TYPE usr_channel
) {
	CM_LOG_FCT("cm_pdusr_send_clock_get")
	CM_CHANNEL_PTR_TYPE channel;

	/* context-switch from CM-CL/MC/SV to CM-PD */

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "current send_clock(%u) ar_cnt(%u)"
		, channel->usr.pd.clock.send_clock
		, channel->usr.pd.clock.ar_cnt
		);

	return channel->usr.pd.clock.send_clock;
}


/*===========================================================================*/
/*=== get IR_DATA_UUID from PDEV ============================================*/
/*===========================================================================*/

CLRPC_UUID_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdusr_get_ir_data_uuid (
	CM_CHANNEL_PTR_TYPE  usr_channel
) {
	CM_LOG_FCT("cm_pdusr_get_ir_data_uuid")
	CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	return & pd_channel->usr.pd.reclist.prm_data.ir_data_uuid;
}


/*===========================================================================*/
/*=== ATTACH ==============================================*/
/*===========================================================================*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pdusr_attach (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 send_clock
) {
	CM_LOG_FCT("cm_pdusr_attach")
	CM_CHANNEL_PTR_TYPE channel;
	LSA_UINT16 rc;

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REFACTOR: cm_pdusr_attach() so that it may be called with all EDDs.")
#endif /* CM_MESSAGE */

	/* context-switch from CM-CL/MC/SV to CM-PD */

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	if (usr_channel->detailptr->path_type == CM_PATH_TYPE_SERVER) {

		if (! channel->usr.pd.pdprm.is_parameterized) {
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "pdev not parameterized (no bug)"
				);
			/* SV may start with "phy power on" (no pdev remanence) */
		}

		rc = cm_pd_send_clock_set (channel, send_clock, cm_pd_sendclock_prop_get (channel)/*keep existing*/);
	}
	else { /* CL/MC (send-clock must be already prm-ed) */

		if (! channel->usr.pd.pdprm.is_parameterized) {
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "pdev not parameterized"
				);
			/* CL calls cm_pdusr_is_parameterized() when adding a device */
			/* MC calls cm_pdusr_is_parameterized() when adding a device */
			CM_FATAL(); /* a bug */
		}

		if (send_clock != channel->usr.pd.clock.send_clock) {
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "send_clock(%u) != pdev-send_clock(%u)"
				, send_clock, channel->usr.pd.clock.send_clock
				);
			rc = CM_ERR_PARAM;
		}
		else {
			rc = CM_OK;
		}
	}

	if (rc == CM_OK  ||  rc == CM_OK_PENDING) {

		channel->usr.pd.clock.ar_cnt += 1;

		if (channel->usr.pd.clock.ar_cnt == 0) {
			CM_FATAL(); /* a bug */
		}
	}
	else {

		/* not incrementing (not attached) */
	}

	CM_PD_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "send_clock(%u), ar_cnt(%u), rsp(%u)"
		, send_clock, channel->usr.pd.clock.ar_cnt, rc
		);

	return rc;
}

/*===========================================================================*/
/*=== DETACH ================================================================*/
/*===========================================================================*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdusr_detach (
	CM_CHANNEL_PTR_TYPE usr_channel
) {
	CM_LOG_FCT("cm_pdusr_detach")
	CM_CHANNEL_PTR_TYPE channel;

	/* context-switch from CM-CL/MC/SV to CM-PD */

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	if (channel->usr.pd.clock.ar_cnt < 1) {
		CM_FATAL (); /* a bug */
	}

	channel->usr.pd.clock.ar_cnt -= 1;
}

/*===========================================================================*/
/*=== CM-PD SET SEND-CLOCK ==================================================*/
/*===========================================================================*/

LSA_UINT16  CM_LOCAL_FCT_ATTR		/* CM_OK | CM_OK_PENDING | CM_ERR_xxx */
cm_pd_send_clock_set (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  send_clock,
	LSA_UINT16  sendclock_prop
) {
	CM_LOG_FCT("cm_pd_send_clock_set")
	LSA_UINT16  rc;

	if (send_clock == 0) {

		CM_FATAL();
		rc = CM_ERR_PARAM; /* unreachable */
	}
	else if (send_clock == channel->usr.pd.clock.send_clock) { /* same send-clock */

		if (sendclock_prop != channel->usr.pd.clock.sendclock_prop) {

			channel->usr.pd.clock.sendclock_prop = sendclock_prop; /*AP01565798*/
		}

		if (channel->usr.pd.clock.is_pending) {

			rc = CM_OK_PENDING;
		}
		else {

			rc = CM_OK;
		}
	}
	else { /* new send-clock */

		if (channel->usr.pd.clock.is_pending) {

			CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cannot set new send_clock(%u), old is_pending(%u)"
				, send_clock, channel->usr.pd.clock.is_pending
				);

			rc = CM_ERR_PARAM;
		}
		else if (channel->usr.pd.clock.ar_cnt != 0) {

			CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "send_clock is in use, ar_cnt(%u)"
				, channel->usr.pd.clock.ar_cnt
				);

			rc = CM_ERR_PARAM;
		}
		else if (! cm_edd_can_set_send_clock (channel)) {

			CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "EDDS cannot set send_clock(%u), fixed to send_clock(%u)"
				, send_clock, channel->usr.pd.clock.send_clock
				);

			rc = CM_ERR_PARAM;
		}
		else {

			if( cm_edd_set_send_clock(channel, send_clock) ) {

				channel->usr.pd.clock.send_clock = send_clock; /* change PDEV-SendClock */
				channel->usr.pd.clock.sendclock_prop = sendclock_prop; /*AP01565798*/
				channel->usr.pd.clock.is_pending = LSA_TRUE;

				rc = CM_OK_PENDING;
			}
			else {

				CM_PD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cannot cm_edd_set_send_clock()"
					);

				rc = CM_ERR_ALLOC_LOWER;
			}
		}
	}

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "send_clock(%u) rc(%u)", send_clock, rc);
	return rc;
}


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_set_send_clock_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 edd_response,
	LSA_UINT16 send_clock
) {
	CM_LOG_FCT("cm_pd_set_send_clock_done")

	if( ! channel->usr.pd.clock.is_pending ) {
		CM_FATAL();
	}

	channel->usr.pd.clock.is_pending = LSA_FALSE;

	/***/

	if (edd_response != EDD_STS_OK) {

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "set send-clock(%u) failed, edd-rsp(%u)"
			, send_clock, edd_response
			);

		send_clock = 0; /* send-clock is not set */
		channel->usr.pd.clock.send_clock = 0; /* send-clock is not set */
	}
	else {

		CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"set send-clock(%u) succeeded"
			, send_clock
			);

		CM_ASSERT(channel->usr.pd.clock.send_clock == send_clock); /* confirm changed PDEV-SendClock */
	}

	/* announce send-clock changed */

	cm_cmpd_sendclock_changed(channel, send_clock);

#if CM_CFG_MAX_CLIENTS
	{
	CM_CHANNEL_PTR_TYPE cl_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_CLIENT);
	if (is_not_null (cl_channel)) {
		cm_cl_sendclock_changed(cl_channel, send_clock);
	}
	}
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	{
	CM_CHANNEL_PTR_TYPE mc_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_MULTICAST);
	if (is_not_null (mc_channel)) {
		cm_mc_sendclock_changed(mc_channel, send_clock);
	}
	}
#endif

#if CM_CFG_MAX_SERVERS
	{
	CM_CHANNEL_PTR_TYPE sv_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_SERVER);
	if (is_not_null (sv_channel)) {
		cm_sv_sendclock_changed(sv_channel, send_clock);
	}
	}
#endif
}


/*===========================================================================*/
#if CM_CFG_MAX_SERVERS
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdsv_mapping_ok (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	CM_LOG_FCT("cm_pdsv_mapping_ok")
	CM_CHANNEL_PTR_TYPE channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);
	CM_PD_PORT_PTR_TYPE port;

	/* edd_port_id, mod_ident and sub_ident are no longer checked, see comment in caller (CM V4.2i1.2, AP00827094) */

	port = cm_pd_port_from_addr (channel, slot_nr, subslot_nr);

	if( is_null(port) ) {

		return LSA_FALSE; /* mapping does not exist */
	}

	return LSA_TRUE;
}


/*===========================================================================*/
/*=== OWNERSHIP =============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_ownership (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr
) {
	CM_LOG_FCT("cm_pdsv_ownership")
	CM_CHANNEL_PTR_TYPE  channel;
	CM_PD_PRM_PTR_TYPE  pdprm;

	/* context-switch from CM-SV to CM-PD */

	CM_ASSERT(sv_channel->detailptr->path_type == CM_PATH_TYPE_SERVER);
	channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	/***/

	pdprm = & channel->usr.pd.pdprm;

	if (ar_nr != 0) {
		LSA_UINT16  owner;

		CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"assign PDev to dev(%u) ar(%u)", device_nr, ar_nr
			);

		owner = cm_pd_prm_owner_get (pdprm);
		if (owner == CM_PD_OCCUPANT_SV  &&  pdprm->ar_nr == ar_nr) {
			/* done */
		}
		else {
			if (cm_pd_prm_ownership (pdprm, CM_PD_OCCUPANT_SV, device_nr, ar_nr) != CM_OK) {
				CM_FATAL1 (pdprm->occupant);
				return;
			}

			if (! pdprm->channel->usr.pd.reclist.is_valid)
			{
				CM_PD_TRACE_00 (pdprm->channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "prm_ownership failed, reclist not valid"
					);
				CM_FATAL ();
				return;
			}

			cm_pd_reclist_prm_begin (channel, LSA_FALSE);
		}
	}
	else {

		CM_FATAL1 (ar_nr);
	}
}

/*===========================================================================*/
/*=== ALARM CONTROL =========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_alarm_control (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_BOOL enable,
	LSA_BOOL mark_all_as_sent, /* evaluated if enable == false */
	LSA_UINT16 max_alarm_data_length /* evaluated if enable == true */
) {
	CM_LOG_FCT("cm_pdsv_alarm_control")
	CM_CHANNEL_PTR_TYPE channel;

	channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	if (device_nr == 0 || ar_nr == 0)
	{
		CM_FATAL();
	}
	else
	{
		CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"dev(%u) ar(%u)  enable(%u) mark_all_as_sent(%u) max_alarm_data_length(%u)"
			, device_nr, ar_nr, enable, mark_all_as_sent, max_alarm_data_length
			);

		if (enable) /* enable alarms and check for unsent diagnoses */
		{
			LSA_UINT16 port_id;

			channel->usr.pd.almi.server_allowed = LSA_TRUE;
			channel->usr.pd.almi.server_max_alarm_data_length = max_alarm_data_length;

			for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
			{
				CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, port_id);

				cm_pd_diag_update_check (channel, port); /* does cm_sv_pdev_alarm_ind() */
			}
		}
		else /* disable alarms and mark unsent diagnoses as sent */
		{
			channel->usr.pd.almi.server_allowed = LSA_FALSE;
			channel->usr.pd.almi.server_max_alarm_data_length = 0; /* sanity */

			if (mark_all_as_sent)
			{
				cm_pd_diag_move_to_server_diag (channel); /* set diag_list_server := diag_list */
			}
		}
	}
}

/*===========================================================================*/
/*=== ALARM RSP =============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_alarm_rsp (
	CM_CHANNEL_PTR_TYPE sv_channel,
	CM_UPPER_RQB_PTR_TYPE alarm
) {
	CM_LOG_FCT("cm_pdsv_alarm_rsp")
	CM_CHANNEL_PTR_TYPE channel;

	CM_PD_TRACE_03 (sv_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cm_pdsv_alarm_rsp : rqb(0x%X) slot_nr(%u) subslot_nr(0x%x)"
		, alarm, alarm->args.sv.ar_alarm_send->slot_nr, alarm->args.sv.ar_alarm_send->subslot_nr
		);

	/* context-switch from CM-SV to CM-PD */

	CM_ASSERT (sv_channel->detailptr->path_type == CM_PATH_TYPE_SERVER);
	channel = cm_peer_channel_get (sv_channel, CM_PATH_TYPE_PD);

	/* free alarm rqb */

	if (is_not_null (alarm->args.sv.ar_alarm_send->alarm_data)) {
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (& rc, alarm->args.sv.ar_alarm_send->alarm_data, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
	}

	cm_upper_free_rqb (channel, alarm);

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("PD_REVIEW: when implementing trigger-next here, look at possible recursion from cm_sv_pdev_alarm_ind()")
#endif /* CM_MESSAGE */

	if (channel->state == CM_PD_READY) {

		if (! channel->usr.pd.almi.server_wf_alarm_rsp) {
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unexpected: call cm_pdsv_alarm_rsp() but don't waiting for"
				);
			/* NOTE: this may occur,
			//	when cm_sv_ar_almi_send_next() hasn't got an acp-rqb from cm_acp_alloc_rqb()
			//	UPDATE: CM-SV preallocates the acp-rqb since V5.1i4
			*/
			CM_FATAL1 (channel->usr.pd.almi.server_wf_alarm_rsp);
		}
		channel->usr.pd.almi.server_wf_alarm_rsp = LSA_FALSE;

		if (channel->usr.pd.almi.server_allowed)
		{
			LSA_UINT16  i;

			/* AP00678581 */
			for (i = 0; i <= channel->usr.pd.port_count; ++i) {
				CM_PD_PORT_PTR_TYPE  port;

				port = cm_pd_port_from_id__nn (channel, i);

				cm_pd_diag_update_check (channel, port);
			}
		}
	}
}

/*===========================================================================*/
/*=== DIAG, MAINTENANCE AND QUALIFIED =======================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdsv_diag_maint_qual (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 CM_LOCAL_MEM_ATTR *maint_status_ptr
) {
	CM_LOG_FCT("cm_pdsv_diag_maint_qual")
	CM_CHANNEL_PTR_TYPE channel;
	CM_PD_PORT_PTR_TYPE port;
	LSA_BOOL diag_status;

	CM_PD_TRACE_03(sv_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"IN : slot(%u) subslot(%u) h(%u)"
			, slot_nr, subslot_nr, sv_channel->my_handle
			);

	/* context-switch from CM-SV to CM-PD */

	CM_ASSERT(sv_channel->detailptr->path_type == CM_PATH_TYPE_SERVER);
	channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	/***/

	port = cm_pd_port_from_addr (channel, slot_nr, subslot_nr);

	if ( is_null(port) ) {
		CM_FATAL (); /* a bug */
	}

	cm_pd_diag_maint_qual (port, & diag_status, maint_status_ptr);

	return diag_status;
}


/*===========================================================================*/
/*=== DIAG READ =============================================================*/
/*===========================================================================*/
CM_LIST_ENTRY_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdsv_diag_read (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr /* 0x8ipp */
) {
	CM_LOG_FCT("cm_pdsv_diag_read")
	CM_PD_PORT_PTR_TYPE port;
	CM_CHANNEL_PTR_TYPE channel;

	CM_PD_TRACE_03(sv_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		,"IN : slot(%u) subslot(%u) h(%u)"
		, subslot_nr, slot_nr, sv_channel->my_handle
		);

	/* context-switch from CM-SV to CM-PD */

	CM_ASSERT(sv_channel->detailptr->path_type == CM_PATH_TYPE_SERVER);
	channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	/***/

	port = cm_pd_port_from_addr (channel, slot_nr, subslot_nr);
	if ( is_null(port) ) {

		/* not allowed */
		CM_FATAL ();
	}

	return &(port->diag.diag_list);
}

/*===========================================================================*/
/*=== PDSV REQUEST ==========================================================*/
/*===========================================================================*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_request(
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT("cm_pdsv_request")
	CM_CHANNEL_PTR_TYPE channel;

	CM_PD_TRACE_03(sv_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"cm_pdsv_request: dev(%u) ar(%u) cm-opc(%u)"
			, device_nr, ar_nr, opcode
			);

	/* context-switch from CM-SV to CM-PD */

	CM_ASSERT(sv_channel->detailptr->path_type == CM_PATH_TYPE_SERVER);

	channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	/***/

	CM_EDD_SET_USER_ID_HANDLE_DEV_AR (rb, sv_channel->my_handle, device_nr, ar_nr);
#ifdef CM_MESSAGE
/*???should we define a CM_RQB_SET_USER...and map the CM_<lower> to it???*/
/*	e.g.   CM_RQB_SET_USER_IF_U32 (rb, CM_BUILD_U32(device_nr, ar_nr)); */
#endif /* CM_MESSAGE */

	CM_RQB_SET_HANDLE(rb, channel->my_handle);

	CM_RQB_SET_OPCODE(rb, opcode);

	/***/

	switch( opcode ) {

	case CM_OPC_PD_PRM_READ_SERVER:
	case CM_OPC_PD_PRM_WRITE_SERVER:
	case CM_OPC_PD_PRM_END_SERVER:
	case CM_OPC_PD_PRM_RELINQUISH_SERVER:
		break;

	default:
		CM_FATAL();
		break;
	}

	cm_pd_req_queue_add (channel, rb);
}

/*===========================================================================*/
#endif /* CM_CFG_MAX_SERVERS */
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS

LSA_UINT8  CM_LOCAL_FCT_ATTR
cm_pdusr_is_irt_local_allowed (
	CM_CHANNEL_PTR_TYPE usr_channel
) {
	CM_LOG_FCT("cm_pdusr_is_irt_local_allowed")
	LSA_UINT8 reason;
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/* if irdata-record available and local sync and local topo ok => true
	 * else => false
	 */


	/***/

#if CM_CFG_RT_CLASS_3_SUPPORT_ON
	{

	if (pd_channel->usr.pd.pdprm.last_appl_ready_ind != CM_OK
	||	! pd_channel->usr.pd.reclist.prm_data.do_irt)
	{
		reason = CM_AR_REASON_IRDATA;

		CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "not allowed: no IRData given, reason(IRDATA/%d)", reason
			);
	}

	else
	{
		CM_PD_PORT_IF_PTR_TYPE port_if = cm_pd_port_if__nn (pd_channel);

		if (port_if->port.misc.sync_led_info != CM_PD_SYNC_LED_INFO_SYNC) {

			reason = CM_AR_REASON_SYNC; /* sync-not-ok */

			CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "not allowed: reason(SYNC/%d)", reason
				);
		}
		else { /* local-sync-ok */

			LSA_UINT16 port_id;
			CM_PD_PORT_PTR_TYPE port;

			LSA_BOOL any_irt_port;
			LSA_BOOL any_irt_ok;

			/*
			 * local-topo-ok := any irt-port has topo-ok
			 */

			any_irt_port = LSA_FALSE;
			any_irt_ok   = LSA_FALSE;

			for (port_id = 1; port_id <= pd_channel->usr.pd.port_count; ++port_id) {

				port = cm_pd_port_from_id__nn (pd_channel, port_id);

				if( port->misc.sync_led_info == CM_PD_SYNC_LED_INFO_NOT_CONFIGURED ) {
					continue; /* not an irt-port */
				}

				any_irt_port = LSA_TRUE; /* implies: PDIRData written, see where flag is set */

				if ( port->misc.topo_state == CM_PD_TOPO_STATE_OK ) {
					any_irt_ok = LSA_TRUE; /* topo-ok */
					break;
				}
			}

			if( any_irt_port ) {

				if( any_irt_ok ) {

					reason = CM_AR_REASON_NONE; /* topo-ok */

					CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "Ok: reason(%u)", reason
						);
				}
				else {

					reason = CM_AR_REASON_TOPO; /* topo-not-ok */

					CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "not allowed: reason(TOPO/%d)", reason
						);
				}
			}
			else {

				reason = CM_AR_REASON_IRDATA; /* no irdata record, no port configured */

				CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "not allowed: reason(IRDATA/%d), no port configured", reason
					);
			}
		}
	}
	}
#else
	LSA_UNUSED_ARG(pd_channel);
	reason = CM_AR_REASON_IRDATA;
#endif

	return reason;

}

#endif

/*===========================================================================*/
/*=== PDCL REQUEST ==========================================================*/
/*===========================================================================*/
#if CM_CFG_MAX_CLIENTS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdcl_request(
	CM_CHANNEL_PTR_TYPE cl_channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT("cm_pdcl_request")
	CM_CHANNEL_PTR_TYPE channel;

	CM_PD_TRACE_03 (cl_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"cm_pdcl_request: dev(%u) ar(%u) cm-opc(%u)"
			, device_nr, ar_nr, opcode
			);

	/* context-switch from CM-CL to CM-PD */

	CM_ASSERT(cl_channel->detailptr->path_type == CM_PATH_TYPE_CLIENT);

	channel = cm_peer_channel_get(cl_channel, CM_PATH_TYPE_PD);

	/***/

	CM_EDD_SET_USER_ID_HANDLE_DEV_AR (rb, cl_channel->my_handle, device_nr, ar_nr);

	CM_RQB_SET_HANDLE(rb, channel->my_handle);

	CM_RQB_SET_OPCODE(rb, opcode);

	/***/

	switch( opcode ) {

	case CM_OPC_PD_PRM_READ_CLIENT:
		break;

	default:
		CM_FATAL();
		break;
	}

	cm_pd_req_queue_add (channel, rb);
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
