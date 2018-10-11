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
/*  F i l e               &F: cm_res.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  resource things                                                          */
/*                                                                           */
/*  - init                                                                   */
/*  - provide event + alarm                                                  */
/*  - get event + alarm                                                      */
/*  - do-reserve                                                             */
/*  - undo-reserve                                                           */
/*  - withdraw                                                               */
/*  - cancel                                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID	8
#define CM_MODULE_ID		8

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/

#define CM_DO_RES_RESERVE 1
#define CM_DO_RES_ALLOC   2
#define CM_DO_RES_COUNT   3


/*===========================================================================*/
/*=== RESOURCES: INIT =======================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_res_init")

	CmListInitialize(&channel->res.eventQ);

	channel->res.withdraw_cnt_events = 0;

	/***/

	CmListInitialize(&channel->res.alarmQ[0]); /* low prio */
	CmListInitialize(&channel->res.alarmQ[1]); /* high prio */

	channel->res.withdraw_cnt_alarms[0] = 0;
	channel->res.withdraw_cnt_alarms[1] = 0;

	/***/

	channel->res.provider_cnt[0] = 0;
	channel->res.provider_cnt[1] = 0;

	{
	LSA_UINT16  i;

	for (i = 0; i < CM_RANGE_ID_MAX; ++i) {
		cm_range_init(&channel->res.id_range[i]);
	}
	}

	channel->res.consumer_cnt[0] = 0;
	channel->res.consumer_cnt[1] = 0;
}

/*===========================================================================*/
/*=== RESOURCES: UNDO_INIT ==================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_undo_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_res_undo_init")
	LSA_UINT16  i;

	for (i = 0; i < CM_RANGE_ID_MAX; ++i) {
		cm_range_undo_init (& channel->res.id_range[i]);
	}

	CM_ASSERT (channel->res.provider_cnt[0] == 0);
	CM_ASSERT (channel->res.provider_cnt[1] == 0);

	CM_ASSERT (channel->res.consumer_cnt[0] == 0);
	CM_ASSERT (channel->res.consumer_cnt[1] == 0);
}

/*===========================================================================*/
/*=== EVENT: PROVIDE INDICATION RESOURCE ====================================*/
/*===========================================================================*/

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: rename -> cm_res_provide_event")*/
#endif /* CM_MESSAGE */

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_event_provide(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT("cm_event_provide")

	CM_ASSERT(CM_RQB_GET_OPCODE(rb) == CM_OPC_EVENT_IND_RES_PROVIDE);
	CM_ASSERT(is_not_null(rb->args.channel.event));

	if( channel->res.withdraw_cnt_events != 0 ) {
		channel->res.withdraw_cnt_events -= 1;
		cm_callback(channel, CM_OK_CANCELLED, &rb);
	}
	else {
		CmListInsertTail(&channel->res.eventQ, (CM_LIST_ENTRY_PTR_TYPE)rb);
	}
}

/*===========================================================================*/
/*=== EVENT: GET INDICATION RESOURCE FROM POOL ==============================*/
/*===========================================================================*/

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: rename -> cm_res_get_event")*/
#endif /* CM_MESSAGE */

CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_event_get(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_event_get")
	CM_UPPER_RQB_PTR_TYPE rb;

	if( CmListIsEmpty(& channel->res.eventQ) ) {
		return LSA_NULL;
	}

	CmListRemoveHead(& channel->res.eventQ, rb, CM_UPPER_RQB_PTR_TYPE);
	CM_ASSERT(rb);
	CM_ASSERT(CM_RQB_GET_OPCODE(rb) == CM_OPC_EVENT_IND_RES_PROVIDE);

	return rb;
}

/*===========================================================================*/
/*=== ALARM: PROVIDE INDICATION RESOURCE ====================================*/
/*===========================================================================*/

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: rename -> cm_res_provide_alarm")*/
#endif /* CM_MESSAGE */

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_alarm_provide(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT("cm_alarm_provide")

	CM_ASSERT(
		CM_RQB_GET_OPCODE(rb) == CM_OPC_ALARM_IND_RES_PROVIDE
	);

#ifdef CM_MESSAGE
/*???check if have params attached!!! add param-error in the doc!!!*/
#endif /* CM_MESSAGE */

	if( rb->args.channel.alarm->alarm_priority > 1 ) {
		CM_AR_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "prio out of range");
		cm_callback(channel, CM_ERR_PARAM, &rb);
		return;
	}

#ifdef CM_MESSAGE
/*???check state == CL/SV_READY, if not err-seq*/
/*???will change for zero-copy possibility*/
/*??? see cm_acp_alarm_indication() */
#endif /* CM_MESSAGE */

	if( is_not_null (rb->args.channel.alarm->alarm_data) ) {
		CM_AR_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW, "buffer not correctly freed");
#ifdef CM_MESSAGE
/*???BW, was ELOG, Pufferkreislauf re-view!!!*/
/* ??? Kap 4.7: rest dont care */
#endif /* CM_MESSAGE */
	}

	/***/

	if( channel->res.withdraw_cnt_alarms[rb->args.channel.alarm->alarm_priority] != 0 ) {
		channel->res.withdraw_cnt_alarms[rb->args.channel.alarm->alarm_priority] -= 1;
		cm_callback(channel, CM_OK_CANCELLED, &rb);
	}
	else {
		CmListInsertTail(&channel->res.alarmQ[rb->args.channel.alarm->alarm_priority], (CM_LIST_ENTRY_PTR_TYPE)rb);
	}
}

/*===========================================================================*/
/*=== ALARM: GET INDICATION RESOURCE FROM POOL ==============================*/
/*===========================================================================*/

CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_res_get_alarm(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 prio
) {
	CM_LOG_FCT("cm_res_get_alarm")
	CM_UPPER_RQB_PTR_TYPE rb;

	CM_ASSERT(prio < 2);

	if( CmListIsEmpty(& channel->res.alarmQ[prio]) ) {
		return LSA_NULL;
	}

	CmListRemoveHead(& channel->res.alarmQ[prio], rb, CM_UPPER_RQB_PTR_TYPE);
	CM_ASSERT(rb);
	CM_ASSERT(CM_RQB_GET_OPCODE(rb) == CM_OPC_ALARM_IND_RES_PROVIDE);
	CM_ASSERT(rb->args.channel.alarm->alarm_priority == prio);

	return rb;
}

/*===========================================================================*/
/*=== RESOURCES: WITHDRAW ===================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_withdraw(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_res_withdraw")
	CM_UPPER_IND_RES_WITHDRAW_PTR_TYPE param;

	param = rb->args.channel.withdraw;

	CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ev(%u) al_low(%u) al_high(%u)", param->events, param->alarms_low, param->alarms_high
		);

	/***/

	channel->res.withdraw_cnt_events    += param->events;
	channel->res.withdraw_cnt_alarms[0] += param->alarms_low;
	channel->res.withdraw_cnt_alarms[1] += param->alarms_high;

	cm_callback(channel, CM_OK, &rb); /* first .CNF then cancel... */

	/***/

	for( ; channel->res.withdraw_cnt_events != 0; --channel->res.withdraw_cnt_events ) {
		CM_UPPER_RQB_PTR_TYPE event;
		event = cm_event_get(channel);
		if( is_null(event) ) {
			break; /* macro may de-ref more than once! */
		}
		cm_callback(channel, CM_OK_CANCELLED, &event);
	}

	for( ; channel->res.withdraw_cnt_alarms[0] != 0; --channel->res.withdraw_cnt_alarms[0] ) {
		CM_UPPER_RQB_PTR_TYPE  alarm;
		alarm = cm_res_get_alarm(channel, CM_ALARM_PRIORITY_LOW);
		if( is_null(alarm) ) {
			break; /* macro may de-ref more than once! */
		}
		cm_callback(channel, CM_OK_CANCELLED, &alarm);
	}

	for( ; channel->res.withdraw_cnt_alarms[1] != 0; --channel->res.withdraw_cnt_alarms[1] ) {
		CM_UPPER_RQB_PTR_TYPE  alarm;
		alarm = cm_res_get_alarm(channel, CM_ALARM_PRIORITY_HIGH);
		if( is_null(alarm) ) {
			break; /* macro may de-ref more than once! */
		}
		cm_callback(channel, CM_OK_CANCELLED, &alarm);
	}
}

/*===========================================================================*/
/*=== RESOURCES: CANCEL =====================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_cancel(
	CM_CHANNEL_PTR_TYPE channel
) {

	for(;;) {
		CM_UPPER_RQB_PTR_TYPE event;
		event = cm_event_get(channel);
		if( is_null(event) ) {
			break; /* macro may de-ref more than once! */
		}
		cm_callback(channel, CM_OK_CANCELLED, &event);
	}

	for(;;) {
		CM_UPPER_RQB_PTR_TYPE  alarm;
		alarm = cm_res_get_alarm(channel, CM_ALARM_PRIORITY_LOW);
		if( is_null (alarm) ) {
			break; /* macro may de-ref more than once! */
		}
		cm_callback(channel, CM_OK_CANCELLED, & alarm);
	}

	for(;;) {
		CM_UPPER_RQB_PTR_TYPE  alarm;
		alarm = cm_res_get_alarm(channel, CM_ALARM_PRIORITY_HIGH);
		if( is_null (alarm) ) {
			break; /* macro may de-ref more than once! */
		}
		cm_callback(channel, CM_OK_CANCELLED, & alarm);
	}
}

/*===========================================================================*/
/*=== ALLOCATE A FRAME RESOURCE =============================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_res_frame_alloc (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 CM_COMMON_MEM_ATTR * frame_id_ptr /* consumer only */
) {
	CM_LOG_FCT("cm_res_frame_alloc")
	LSA_UINT16 do_res;
	LSA_UINT16 cons_idx;
	LSA_UINT16 cnt_idx;
	LSA_UINT16 nr_of_providers;
	LSA_UINT16 nr_of_consumers;
	LSA_BOOL  rc;

	/***/

	switch (rt_class) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_1_MULTICAST;
			do_res = CM_DO_RES_RESERVE;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_1_UNICAST;
			do_res = CM_DO_RES_ALLOC;
		}

		cnt_idx = 0;

		nr_of_providers = channel->lower.info.acp.nr_of_provider_class1_2;

		nr_of_consumers = CM_CAST_U16 (
				  channel->lower.info.acp.nr_of_consumer_class1_unicast
				+ channel->lower.info.acp.nr_of_consumer_class1_multicast
			);
		if( ! channel->lower.info.acp.consumer_class1_2_is_combined ) {
			nr_of_consumers +=
					  channel->lower.info.acp.nr_of_consumer_class2_unicast
					+ channel->lower.info.acp.nr_of_consumer_class2_multicast
				;
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_2:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_2_MULTICAST;
			do_res = CM_DO_RES_RESERVE;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_2_UNICAST;
			do_res = CM_DO_RES_ALLOC;
		}

		cnt_idx = 0;

		nr_of_providers = channel->lower.info.acp.nr_of_provider_class1_2;

		nr_of_consumers = CM_CAST_U16 (
				  channel->lower.info.acp.nr_of_consumer_class2_unicast
				+ channel->lower.info.acp.nr_of_consumer_class2_multicast
			);
		if( ! channel->lower.info.acp.consumer_class1_2_is_combined ) {
			nr_of_consumers +=
					  channel->lower.info.acp.nr_of_consumer_class1_unicast
					+ channel->lower.info.acp.nr_of_consumer_class1_multicast
				;
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		cons_idx = CM_RANGE_ID_MAX; /* not used */
		do_res = CM_DO_RES_COUNT;

		cnt_idx = 1;

		nr_of_providers = channel->lower.info.acp.nr_of_provider_class3;

		nr_of_consumers = channel->lower.info.acp.nr_of_consumer_class3;
		break;

	default:
		CM_FATAL(); /* a bug, see caller */
		return LSA_FALSE; /* unreachable */
	}

	/***/

	if( is_consumer ) {

		CM_ASSERT(is_not_null(frame_id_ptr));

		if( channel->res.consumer_cnt[cnt_idx] >= nr_of_consumers ) {

			CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "rt_class(%u) no cons-resource, nr_of_consumers(%u), class1_2_is_combined(%u)"
				, rt_class, nr_of_consumers, channel->lower.info.acp.consumer_class1_2_is_combined
				);

			rc = LSA_FALSE;
		}
		else {

			switch( do_res ) {
			case CM_DO_RES_COUNT:

				rc = LSA_TRUE;
				break;

			case CM_DO_RES_RESERVE:

				rc = cm_range_reserve_id  (&channel->res.id_range[cons_idx], *frame_id_ptr);
				break;

			case CM_DO_RES_ALLOC:
				rc = cm_range_alloc_id (&channel->res.id_range[cons_idx], frame_id_ptr);
				break;

			default:
				CM_FATAL1(do_res);
				return LSA_FALSE; /* unreachable */
			}

			if( rc ) {

				channel->res.consumer_cnt[cnt_idx] += 1;
			}
		}
	}
	else {

		/* not: CM_ASSERT(is_null(frame_id_ptr)); */
		/* not: CM_ASSERT(do_res); */

		if( channel->res.provider_cnt[cnt_idx] >= nr_of_providers ) {

			CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "rt_class(%u) no prov-resource, nr_of_providers(%u)"
				, rt_class, nr_of_providers
				);

			rc = LSA_FALSE;
		}
		else {

			channel->res.provider_cnt[cnt_idx] += 1;

			CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "rt_class(%u) prov-total(%u)"
				, rt_class, channel->res.provider_cnt[cnt_idx]
				);

			rc = LSA_TRUE;
		}
	}

	/***/

	CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rc(%u)", rc
		);

	return rc;
}

/*===========================================================================*/
/*=== FREE A FRAME RESOURCE =================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_frame_free (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 frame_id /* consumer only */
) {
	CM_LOG_FCT("cm_res_frame_free")
	LSA_UINT16 cons_idx;
	LSA_UINT16 cnt_idx;

	/***/

	switch (rt_class) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_1_MULTICAST;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_1_UNICAST;
		}
		cnt_idx = 0;
		break;

	case CM_IOCR_PROP_RT_CLASS_2:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_2_MULTICAST;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_2_UNICAST;
		}
		cnt_idx = 0;
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		cons_idx = CM_RANGE_ID_MAX; /* not used, only counting */
		cnt_idx = 1;
		break;

	default:
		CM_FATAL(); /* a bug, see caller */
		return; /* unreachable */
	}

	/***/

	if( is_consumer ) {

		if( rt_class != CM_IOCR_PROP_RT_CLASS_3 ) {

			cm_range_free_id (&channel->res.id_range[cons_idx], frame_id);
		}

		if( channel->res.consumer_cnt[cnt_idx] < 1 ) {

			CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "rt_class(%u)", rt_class
				);

			CM_FATAL(); /* a bug */
		}
		else {

			channel->res.consumer_cnt[cnt_idx] -= 1;
		}
	}
	else {

		if( channel->res.provider_cnt[cnt_idx] < 1 ) {

			CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "rt_class(%u)", rt_class
				);

			CM_FATAL(); /* a bug */
		}
		else {

			channel->res.provider_cnt[cnt_idx] -= 1;
		}
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
