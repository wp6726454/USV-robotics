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
/*  F i l e               &F: cm_acp1.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  alloc / free acp-RQBs                                                    */
/*  channel open                                                             */
/*  channel info                                                             */
/*  channel close                                                            */
/*  acp lower-done callback function                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  20
#define CM_MODULE_ID       20

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*-----------------------------------------------------------------------------
//	work: consistency-check
//---------------------------------------------------------------------------*/

#if CM_CFG_RT_CLASS_3_SUPPORT_ON != ACP_CFG_RT_CLASS_3_SUPPORT_ON
# error "CM_CFG_RT_CLASS_3_SUPPORT_ON != ACP_CFG_RT_CLASS_3_SUPPORT_ON"
#endif

/*-----------------------------------------------------------------------------
//	work: alloc lower ACP-RQB that fits for all requests
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_ACP_LOWER_RQB_PTR_TYPE	*lower_rqb_ptr_ptr
) {
	CM_LOG_FCT ("cm_acp_alloc_rqb")
	LSA_UINT16 rc;
	ACP_UPPER_ARGS_PTR_TYPE args;

	CM_ASSERT (is_not_null(channel));
	CM_ASSERT (is_not_null(lower_rqb_ptr_ptr));

		/* alloc the RQB */
	CM_ACP_ALLOC_LOWER_RQB (lower_rqb_ptr_ptr, cm_null_user_id, sizeof(CM_ACP_LOWER_RQB_TYPE), channel->sysptr);

	if (is_null (*lower_rqb_ptr_ptr)) {
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP,
				"h(%u) cannot alloc ACP RQB", channel->my_handle
			);
		return;
	}

#if CM_DEBUG
	CM_ACP_SET_NEXT_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
	CM_ACP_SET_PREV_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif

		/* alloc the buffer and attach them to the RQB */
	CM_ACP_ALLOC_LOWER_MEM (CM_CAST_ACP_MEM_PTR_PTR (& args),
			cm_null_user_id, sizeof (ACP_ARGS_TYPE), channel->sysptr
		);
	if (is_null (args)) {
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP,
				"h(%u) cannot alloc ACP RQB", channel->my_handle
			);
		CM_ACP_FREE_LOWER_RQB  (& rc, *lower_rqb_ptr_ptr, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK); /* don't care in release version */
		*lower_rqb_ptr_ptr = LSA_NULL; /* report failure */
		return;
	}

	CM_ACP_SET_ARGS (*lower_rqb_ptr_ptr, args);

	CM_ACP_SET_OPCODE (*lower_rqb_ptr_ptr, 0); /* sanity, see cm_acp_free_rqb() */
}


/*-----------------------------------------------------------------------------
//	work: free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr_ptr
) {
	CM_LOG_FCT ("cm_acp_free_rqb")
	LSA_UINT16	rc;

	CM_ASSERT (is_not_null(channel));
	CM_ASSERT (is_not_null(lower_rqb_ptr_ptr));
	CM_ASSERT (is_not_null(*lower_rqb_ptr_ptr));

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REFACTOR: cm_acp_free_rqb, do not look at opcode, pass-in a flag")
#endif /* CM_MESSAGE */

	switch (CM_ACP_GET_OPCODE (*lower_rqb_ptr_ptr)) {
	case ACP_OPC_ALARM_SEND:
	case ACP_OPC_ALARM_ACK_SEND:
	default:
		/* use upper pointer !!! */
		break;
	case ACP_OPC_ALARM_IND_RES_PROVIDE:
	case ACP_OPC_ALARM_INDICATION:
	case ACP_OPC_ALARM_ACK_INDICATION:
		if (is_not_null ((*lower_rqb_ptr_ptr)->args->alarm.data.user_data)) {
			CM_ACP_FREE_ALARM_MEM (& rc,
					(*lower_rqb_ptr_ptr)->args->alarm.data.user_data, channel->sysptr
				);
			CM_ASSERT (rc == LSA_RET_OK);
		}
		break;
	}

	CM_ACP_FREE_LOWER_MEM (& rc, CM_ACP_GET_ARGS (*lower_rqb_ptr_ptr), channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);

	CM_ACP_FREE_LOWER_RQB (& rc, *lower_rqb_ptr_ptr, channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);

	*lower_rqb_ptr_ptr = LSA_NULL; /* wipe out */
}


/*-----------------------------------------------------------------------------
//	work: open the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_acp_channel_open")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_CLIENT
		|| channel->detailptr->path_type == CM_PATH_TYPE_MULTICAST
		|| channel->detailptr->path_type == CM_PATH_TYPE_SERVER
		);

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == ACP_INVALID_HANDLE);

	cm_acp_alloc_rqb (channel, & acp);
	if (is_null (acp)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_ACP_SET_HANDLE (acp, channel->lower.handle);
	CM_ACP_SET_OPCODE (acp, ACP_OPC_OPEN_CHANNEL);
	CM_ACP_SET_USER_ID_PTR (acp, LSA_NULL);

	acp->args->channel.open.handle = ACP_INVALID_HANDLE; /* sanity */
	acp->args->channel.open.handle_upper = channel->my_handle;
	acp->args->channel.open.sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	acp->args->channel.open.acp_request_upper_done_ptr = cm_acp_request_lower_done;

	CM_ACP_OPEN_CHANNEL_LOWER (acp, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_open_channel_done")

	CM_ASSERT (channel->state == CM_CH_OPEN);

	if (CM_ACP_GET_RESPONSE (acp) == ACP_OK) {

		channel->lower.handle = acp->args->channel.open.handle;
		CM_ASSERT (channel->lower.handle != ACP_INVALID_HANDLE);

		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) rsp(OK) handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		/***/

		cm_channel_done (channel, CM_OK);

		/***/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("NOTE: reusing acp-rqb for link-status/portid-auto")
/*#pragma CM_MESSAGE("VV: defer link-status-internal after get-info?")*/
/*#pragma CM_MESSAGE("VV: pass rqb into done and call it from the channel machine!!!")*/
#endif /* CM_MESSAGE */

		CM_ASSERT(channel->state == CM_CH_INFO);

		cm_acp_free_rqb (channel, & acp);
	}
	else {
		CM_ASSERT (channel->lower.handle == ACP_INVALID_HANDLE); /* sanity */

		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rsp(%u)", channel->my_handle, CM_ACP_GET_RESPONSE (acp)
			);
		cm_acp_free_rqb (channel, & acp);
		cm_channel_done (channel, CM_ERR_LOWER_LAYER);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_nop (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  user_id,
	LSA_UINT32  user_1,
	LSA_UINT32  user_2
) {
	CM_LOG_FCT("cm_acp_nop")
	CM_ACP_LOWER_RQB_PTR_TYPE  nop_acp;

	cm_acp_alloc_rqb (channel, & nop_acp);
	if (is_null (nop_acp)) {
		CM_FATAL ();
		return;
	}

	nop_acp->args->channel.nop.user_1 = user_1;
	nop_acp->args->channel.nop.user_2 = user_2;
	nop_acp->args->channel.nop.user_3 = 0;
	nop_acp->args->channel.nop.user_4 = 0;

	cm_acp_request_lower (channel, ACP_OPC_NOP, user_id, nop_acp);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_nop_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  nop_acp
) {
	CM_LOG_FCT("cm_acp_nop_done")
	LSA_VOID_PTR_TYPE  user_id;
	LSA_UINT32  user_1;
	LSA_UINT32  user_2;

	CM_ASSERT (CM_ACP_GET_RESPONSE (nop_acp) == ACP_OK);

	user_id = CM_ACP_GET_USER_ID_PTR (nop_acp);
	user_1  = nop_acp->args->channel.nop.user_1;
	user_2  = nop_acp->args->channel.nop.user_2;

	cm_acp_free_rqb (channel, & nop_acp);

	/* now callback */
	/* for cast see caller */

	if (are_equal (user_id, (LSA_VOID_PTR_TYPE)(LSA_UINT32)cm_arcq_request_done))
	{
		CM_AR_GRAPH_PTR_TYPE  argr = (CM_AR_GRAPH_PTR_TYPE)user_1;
		CM_ACP_LOWER_RQB_PTR_TYPE  acp = (CM_ACP_LOWER_RQB_PTR_TYPE)user_2;

		cm_arcq_request_done (argr, acp);
	}
#if CM_CFG_MAX_MULTICAST_CHANNELS
	else if (are_equal (user_id, (LSA_VOID_PTR_TYPE)(LSA_UINT32)cm_mc_device_remove_done))
	{
		CM_MC_DEVICE_PTR_TYPE  dev = (CM_MC_DEVICE_PTR_TYPE)user_1;

		CM_MC_WF_RESET (dev->wait_for, CM_MWF__DEVICE_REMOVE_CNF);
		cm_mc_device_remove_done (channel, dev);
	}
#endif
	else
	{
		CM_ACP_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "nop: user_id(0x%x) user_1(%u) user_2(%u)"
			, user_id, user_1, user_2
			);
		CM_FATAL1 ((LSA_UINT32)user_id);
		return;
	}
}


/*-----------------------------------------------------------------------------
//	work: get the channel info
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_channel_info (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT ("cm_acp_channel_info")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	CM_ASSERT (channel->state == CM_CH_INFO);

	cm_acp_alloc_rqb (channel, & acp);
	if (is_null (acp)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	cm_acp_request_lower (channel, ACP_OPC_CHANNEL_INFO, channel, acp);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_channel_info_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_channel_info_done")

	CM_ASSERT (channel->state == CM_CH_INFO);

	if (CM_ACP_GET_RESPONSE (acp) == ACP_OK) {

		CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) rsp(OK)", channel->my_handle
			);

		channel->lower.info.acp = acp->args->channel.info;	/* infos about mac and frame-range */

		channel->lower.info.acp.cycle_base_factor = 0; /* invalidate (sanity) */

		cm_acp_free_rqb (channel, & acp);

		/***/

		if( ! (
			channel->lower.info.acp.eth_interface_id >= 1
			&& channel->lower.info.acp.eth_interface_id <= 16
		) ) {
			CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "lower.info.acp.eth_interface_id(%u) out of range"
				, channel->lower.info.acp.eth_interface_id
				);
			CM_FATAL(); /* configure it correctly and try again, please */
			return;
		}

		if( ! (
			channel->lower.info.acp.eth_ports_max >= 1
			&& channel->lower.info.acp.eth_ports_max <= 255
		) ) {
			CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "lower.info.acp.eth_ports_max(%u) out of range"
				, channel->lower.info.acp.eth_ports_max
				);
			CM_FATAL(); /* configure it correctly and try again, please */
			return;
		}

		/***/

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: move ranges to a function, call it from the channel machine")*/
#endif /* CM_MESSAGE */

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_ALARM_REF],
			channel->lower.info.acp.alarm_ref_base,
			channel->lower.info.acp.alarm_nr_of_ref
			)
		) {
			cm_channel_done (channel, CM_ERR_ALLOC_LOCAL);
			return;
		}

		/***/

		if (channel->lower.info.acp.nr_of_consumer_class1_unicast > 0) {

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_1,
					channel->lower.info.acp.frame_id_base_class1_unicast
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_1,
					CM_CAST_U16(channel->lower.info.acp.frame_id_base_class1_unicast + channel->lower.info.acp.nr_of_consumer_class1_unicast - 1)
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}

		}

		if (channel->lower.info.acp.nr_of_consumer_class1_multicast > 0) {

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_TRUE,
					CM_IOCR_PROP_RT_CLASS_1,
					channel->lower.info.acp.frame_id_base_class1_multicast
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_TRUE,
					CM_IOCR_PROP_RT_CLASS_1,
					CM_CAST_U16(channel->lower.info.acp.frame_id_base_class1_multicast
						+ channel->lower.info.acp.nr_of_consumer_class1_multicast - 1)
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}
		}

		if (channel->lower.info.acp.nr_of_consumer_class2_unicast > 0) {

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_2,
					channel->lower.info.acp.frame_id_base_class2_unicast
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_2,
					CM_CAST_U16(channel->lower.info.acp.frame_id_base_class2_unicast + channel->lower.info.acp.nr_of_consumer_class2_unicast - 1)
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}
		}

		if (channel->lower.info.acp.nr_of_consumer_class2_multicast > 0) {

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_TRUE,
					CM_IOCR_PROP_RT_CLASS_2,
					channel->lower.info.acp.frame_id_base_class2_multicast
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_TRUE,
					CM_IOCR_PROP_RT_CLASS_2,
					CM_CAST_U16(channel->lower.info.acp.frame_id_base_class2_multicast
						+ channel->lower.info.acp.nr_of_consumer_class2_multicast - 1)
			) ) {
				CM_ACP_LOWER_RQB_ERROR(0);
				return;
			}
		}

		/***/

#if CM_CFG_RT_CLASS_3_SUPPORT_ON == 0 /*off*/
		if (channel->lower.info.acp.nr_of_consumer_class3 > 0) {
			CM_FATAL (); /* wrong configuration */
		}
		if (channel->lower.info.acp.nr_of_provider_class3 > 0) {
			CM_FATAL (); /* wrong configuration */
		}
#endif

		/***/

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_CLASS_1_UNICAST],
			channel->lower.info.acp.frame_id_base_class1_unicast,
			channel->lower.info.acp.nr_of_consumer_class1_unicast
			)
		) {
			cm_channel_done (channel, CM_ERR_ALLOC_LOCAL);
			return;
		}

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_CLASS_1_MULTICAST],
			channel->lower.info.acp.frame_id_base_class1_multicast,
			channel->lower.info.acp.nr_of_consumer_class1_multicast
			)
		) {
			cm_channel_done (channel, CM_ERR_ALLOC_LOCAL);
			return;
		}

		/***/

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_CLASS_2_UNICAST],
			channel->lower.info.acp.frame_id_base_class2_unicast,
			channel->lower.info.acp.nr_of_consumer_class2_unicast
			)
		) {
			cm_channel_done (channel, CM_ERR_ALLOC_LOCAL);
			return;
		}

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_CLASS_2_MULTICAST],
			channel->lower.info.acp.frame_id_base_class2_multicast,
			channel->lower.info.acp.nr_of_consumer_class2_multicast
			)
		) {
			cm_channel_done (channel, CM_ERR_ALLOC_LOCAL);
			return;
		}

		/***/

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: move provide to a function, call it from the channel machine")*/
#endif /* CM_MESSAGE */
		{
		LSA_UINT16 max_state;
		LSA_UINT16 max_al_hi;
		LSA_UINT16 max_al_lo;
		LSA_UINT16 max_len;
		LSA_UINT16 i;

		switch( channel->detailptr->path_type ) {
#if CM_CFG_MAX_CLIENTS
		case CM_PATH_TYPE_CLIENT:
			max_state = channel->detailptr->u.cl.acp.state_ind_resource_count;
			max_al_hi = channel->detailptr->u.cl.acp.alarm_high_prio_ind_resource_count;
			max_al_lo = channel->detailptr->u.cl.acp.alarm_low_prio_ind_resource_count;
			max_len   = channel->detailptr->u.cl.acp.max_alarm_data_length;
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			max_state = channel->detailptr->u.mc.state_ind_resource_count;
			max_al_hi = 0;
			max_al_lo = 0;
			max_len   = 0;
			break;
#endif
#if CM_CFG_MAX_SERVERS
		case CM_PATH_TYPE_SERVER:
			max_state = channel->detailptr->u.sv.acp.state_ind_resource_count;
			max_al_hi = channel->detailptr->u.sv.acp.alarm_high_prio_ind_resource_count;
			max_al_lo = channel->detailptr->u.sv.acp.alarm_low_prio_ind_resource_count;
			max_len   = channel->detailptr->u.sv.acp.max_alarm_data_length;
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			return; /* unreachable */
		}

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("warning: TODO review these consistency checks. Ziel: acp-init-value == edd-init-value")
#endif

#if CM_CFG_MAX_SERVERS
# if (CM_DATA_STATUS_INIT | EDD_CSRT_DSTAT_BIT_DATA_VALID) != EDD_CSRT_DSTAT_LOCAL_INIT_VALUE
#  error "who changed that?"
# endif
#endif

		CM_ASSERT(ACP_STATE_CONSUMER_SYSRED_DEFAULT ==  ( ACP_STATE_RUN            | ACP_STATE_STATION_OK            | ACP_STATE_BACKUP             | ACP_STATE_SYSRED_PRIMARY_AR_EXISTS | ACP_STATE_MISS_NOT_STOPPED ));
		CM_ASSERT(ACP_STATE_CONSUMER_DEFAULT ==         ( ACP_STATE_RUN            | ACP_STATE_STATION_OK            | ACP_STATE_PRIMARY            | ACP_STATE_SYSRED_PRIMARY_AR_EXISTS | ACP_STATE_MISS_NOT_STOPPED ));
		CM_ASSERT(CM_EVENT_APDU_STATUS_INITIAL_VALUE == ( CM_EVENT_APDU_STATUS_RUN | CM_EVENT_APDU_STATUS_STATION_OK | CM_EVENT_APDU_STATUS_PRIMARY | CM_EVENT_APDU_STATUS_SYSRED_OK ));

		for( i = 0; i < max_state; ++i ) {
			CM_ACP_LOWER_RQB_PTR_TYPE acp;

			cm_acp_alloc_rqb (channel, &acp);
			if (is_null(acp)) {
				CM_FATAL();
			}

			cm_acp_request_lower (channel, ACP_OPC_STATE_IND_RES_PROVIDE, 0/*user_id*/, acp);
		}


		CM_ASSERT(ACP_ALARM_PRIORITY_LOW == CM_ALARM_PRIORITY_LOW);
		CM_ASSERT(ACP_ALARM_PRIORITY_HIGH == CM_ALARM_PRIORITY_HIGH);

		for( i = 0; i < max_al_hi; ++i ) {
			CM_ACP_LOWER_RQB_PTR_TYPE acp;

			cm_acp_alloc_rqb(channel, &acp);
			if (is_null(acp)) {
				CM_FATAL();
			}

			if (channel->acp.max_alarm_data_length != max_len) {
				CM_FATAL(); /* see cm_open_channel() */
			}

			CM_ACP_ALLOC_ALARM_MEM (CM_CAST_ACP_MEM_PTR_PTR (& acp->args->alarm.data.user_data),
					channel->acp.max_alarm_data_length, channel->sysptr
				);
			if (is_null (acp->args->alarm.data.user_data)) {
				CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "h(%u) cannot alloc ACP ALARM MEM(%u)"
					, channel->my_handle, channel->acp.max_alarm_data_length
					);
				CM_FATAL ();
			}

			acp->args->alarm.data.user_max_length = channel->acp.max_alarm_data_length;
			acp->args->alarm.data.priority = ACP_ALARM_PRIORITY_HIGH;

			cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*user_id*/, acp);
		}

		for( i = 0; i < max_al_lo; ++i ) {
			CM_ACP_LOWER_RQB_PTR_TYPE acp;

			cm_acp_alloc_rqb(channel, &acp);
			if (is_null(acp)) {
				CM_FATAL();
			}

			CM_ASSERT (channel->acp.max_alarm_data_length == max_len); /* see cm_open_channel() */

			CM_ACP_ALLOC_ALARM_MEM (CM_CAST_ACP_MEM_PTR_PTR (& acp->args->alarm.data.user_data),
					channel->acp.max_alarm_data_length, channel->sysptr
				);
			if (is_null (acp->args->alarm.data.user_data)) {
				CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "h(%u) cannot alloc ACP ALARM MEM(%u)"
					, channel->my_handle, channel->acp.max_alarm_data_length
					);
				CM_FATAL ();
			}

			acp->args->alarm.data.user_max_length = channel->acp.max_alarm_data_length;
			acp->args->alarm.data.priority = ACP_ALARM_PRIORITY_LOW;

			cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*user_id*/, acp);
		}
		}

		/***/

		cm_channel_done (channel, CM_OK);
	}
	else {
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rsp(%u)"
			, channel->my_handle, CM_ACP_GET_RESPONSE (acp)
			);
		cm_acp_free_rqb (channel, & acp);
		cm_channel_done (channel, CM_ERR_LOWER_LAYER);
	}
}


/*------------------------------------------------------------------------------
// work: common request
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_VOID_PTR_TYPE  user_id,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
) {
	CM_LOG_FCT ("cm_acp_request_lower")

	CM_ACP_SET_HANDLE (acp, channel->lower.handle);
	CM_ACP_SET_OPCODE (acp, opcode);
	CM_ACP_SET_USER_ID_PTR (acp, user_id);

	CM_ACP_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> acp-rqb(0x%08x) acp-opc(%u) f-h(%u)"
		, acp, CM_ACP_GET_OPCODE (acp), channel->my_handle
		);

	CM_ACP_REQUEST_LOWER (acp, channel->sysptr);
}


/*-----------------------------------------------------------------------------
//	work: close the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_channel_close (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT ("cm_acp_channel_close")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_CLIENT
		|| channel->detailptr->path_type == CM_PATH_TYPE_MULTICAST
		|| channel->detailptr->path_type == CM_PATH_TYPE_SERVER
		);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	cm_acp_alloc_rqb (channel, & acp);
	if (is_null (acp)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_ACP_SET_HANDLE (acp, channel->lower.handle);
	CM_ACP_SET_OPCODE (acp, ACP_OPC_CLOSE_CHANNEL);
	CM_ACP_SET_USER_ID_PTR (acp, LSA_NULL);

	CM_ACP_CLOSE_CHANNEL_LOWER (acp, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_close_channel_done")

	if (CM_ACP_GET_RESPONSE (acp) == ACP_OK) {

		channel->lower.handle = ACP_INVALID_HANDLE;

		CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) rsp(OK)", channel->my_handle
			);
		cm_acp_free_rqb (channel, & acp);
		cm_channel_done (channel, CM_OK);
	}
	else {
		/* don't invalidate acp-handle, channel is not closed! */
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rsp(%u)"
			, channel->my_handle, CM_ACP_GET_RESPONSE (acp)
			);
		cm_acp_free_rqb (channel, & acp);
		cm_channel_done (channel, CM_ERR_LOWER_LAYER);
	}
}

/*===========================================================================*/
/*=== ACP LOWER DONE ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_ACP_LOWER_IN_FCT_ATTR
cm_acp_request_lower_done(
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_request_lower_done")
	CM_CHANNEL_PTR_TYPE  channel;

	if( is_null(acp) )
	{
		CM_ACP_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "is_null(acp)"
			);
		CM_ACP_LOWER_RQB_ERROR (acp);
		return;
	}

	channel = cm_channel_from_handle(CM_ACP_GET_HANDLE(acp));
	if( is_null(channel) )
	{
		CM_ACP_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL
			, "invalid handle(%u)", CM_ACP_GET_HANDLE(acp)
			);
		CM_ACP_LOWER_RQB_ERROR (acp);
		return;
	}

	switch (CM_ACP_GET_RESPONSE(acp)) {
	case ACP_OK:
	case ACP_OK_CANCEL:
		CM_ACP_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< acp-rqb(0x%08x) acp-opc(%u) acp-rsp(%u) t-h(%u)"
			, acp, CM_ACP_GET_OPCODE(acp), CM_ACP_GET_RESPONSE(acp)
			, channel->my_handle
			);
		break;

	case ACP_ERR_ASRT: /* AP00300519: warn only */
		CM_ACP_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "<<< acp-rqb(0x%08x) acp-opc(%u) acp-rsp(%u) t-h(%u)"
			, acp, CM_ACP_GET_OPCODE(acp), CM_ACP_GET_RESPONSE(acp)
			, channel->my_handle
			);
		break;

	default:
		CM_ACP_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "<<< acp-rqb(0x%08x) acp-opc(%u) acp-rsp(%u) t-h(%u)"
			, acp, CM_ACP_GET_OPCODE(acp), CM_ACP_GET_RESPONSE(acp)
			, channel->my_handle
			);
		break;
	}

	switch (CM_ACP_GET_OPCODE (acp)) {
	case ACP_OPC_OPEN_CHANNEL:
		cm_acp_channel_open_done (channel, acp);
		return;

	case ACP_OPC_CLOSE_CHANNEL:
		cm_acp_channel_close_done (channel, acp);
		return;

	case ACP_OPC_CHANNEL_INFO:
		cm_acp_channel_info_done (channel, acp);
		return;

	case ACP_OPC_NOP:
		cm_acp_nop_done (channel, acp);
		return;

	case ACP_OPC_GLOBAL_PROVIDER_CONTROL:
		switch (channel->detailptr->path_type)
		{
#if CM_CFG_MAX_CLIENTS
		case CM_PATH_TYPE_CLIENT:
			cm_cl_acp_global_provider_control_done (channel, acp);
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			cm_mc_acp_global_provider_control_done (channel, acp);
			break;
#endif
#if CM_CFG_MAX_SERVERS
		case CM_PATH_TYPE_SERVER:
			cm_sv_acp_global_provider_control_done (channel, acp);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
		return;

	case ACP_OPC_STATE_IND_RES_PROVIDE:
		if( CM_ACP_GET_RESPONSE(acp) != ACP_OK_CANCEL ) {
			CM_FATAL(); /* what else? */
			return; /* unreachable... */
		}
		CM_ASSERT (channel->state == CM_CH_CLOSE);
		cm_acp_free_rqb(channel, &acp);
		return;

	case ACP_OPC_STATE_INDICATION:
		cm_acp_frame_indication(channel, acp);
		return;

	case ACP_OPC_OVERLOAD_IND_RES_PROVIDE:
	case ACP_OPC_OVERLOAD_INDICATION:
		cm_acp_overload_info_done (channel, acp);
		return;

	case ACP_OPC_CONSUMER_ADD:
	case ACP_OPC_PROVIDER_ADD:
		cm_acp_frame_add_done(channel, CM_CAST_ARGR_IOCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	case ACP_OPC_PROVIDER_CONTROL:
	case ACP_OPC_CONSUMER_CONTROL:
		cm_acp_frame_control_done (channel, CM_CAST_ARGR_IOCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	case ACP_OPC_CONSUMER_REMOVE:
	case ACP_OPC_PROVIDER_REMOVE:
		cm_acp_frame_remove_done(channel, CM_CAST_ARGR_IOCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

#if CM_CFG_MAX_SERVERS
	case ACP_OPC_GET_APDU_STATUS:
		cm_acp_frame_get_apdu_status_done (channel, CM_ACP_GET_USER_ID_PTR(acp), acp);
		return;
#endif

#if CM_CFG_USE_NARE
	case ACP_OPC_MULTICAST:
		cm_acp_multicast_mac_done(channel, CM_CAST_ARGR_IOCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;
#endif

	case ACP_OPC_ALARM_OPEN:
	case ACP_OPC_UDP_ALARM_OPEN:
		cm_acp_alarm_add_done(channel, CM_CAST_ARGR_ALARMCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	case ACP_OPC_ALARM_IND_RES_PROVIDE:
		if( CM_ACP_GET_RESPONSE(acp) != ACP_OK_CANCEL ) {
			CM_FATAL(); /* what else? */
			return; /* unreachable... */
		}
		CM_ASSERT (channel->state == CM_CH_CLOSE);
		cm_acp_free_rqb (channel, & acp);
		return;

	case ACP_OPC_ALARM_SEND:
		cm_acp_alarm_send_done (channel, CM_CAST_ARGR_ALARMCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	case ACP_OPC_ALARM_INDICATION:
		cm_acp_alarm_indication(channel, acp);
		return;

	case ACP_OPC_ALARM_ACK_SEND:
		cm_acp_alarm_ack_send_done (channel, CM_CAST_ARGR_ALARMCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	case ACP_OPC_ALARM_ACK_INDICATION:
		cm_acp_alarm_ack_indication (channel, acp);
		return;

	case ACP_OPC_ALARM_CLOSE:
		cm_acp_alarm_remove_done(channel, CM_CAST_ARGR_ALARMCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	default:
		CM_ACP_LOWER_RQB_ERROR (acp); /* should not happen */
		return;
	}
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
