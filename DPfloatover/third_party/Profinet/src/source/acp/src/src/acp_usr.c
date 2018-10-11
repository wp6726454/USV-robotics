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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the user LSA interface                                        */
/*                                                                           */
/*****************************************************************************/
#define ACP_MODULE_ID		3
#define LTRC_ACT_MODUL_ID	3

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)


/*------------------------------------------------------------------------------
//	func: acp_callback
//	work: takes the request-block of the last request, sets the result, and
//		call the callback-function (notify the user)
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_callback (ACP_CHANNEL_PTR channel, LSA_UINT32 result)
{
	ACP_LOG_FCT("acp_callback")

		/* now requests to acp given in the callback are simple queued (channel->upper is locked) */
	acp_callback_async (channel, result, & channel->upper);
	ACP_ASSERT (is_null (channel->upper));

		/* and then served */
	acp_run_request (channel);
	return;
}


/*------------------------------------------------------------------------------
//	free all mem alloc in acp_open_channel()
//----------------------------------------------------------------------------*/
static LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_free_channel_mem (
	ACP_CHANNEL_PTR channel
	)
{
	ACP_LOG_FCT("acp_free_channel_mem")
	LSA_UINT16 rc;

	if (is_not_null (channel->frames)) {
		ACP_FREE_LOCAL_MEM (&rc, channel->frames);
		ACP_ASSERT (rc == LSA_RET_OK);
		channel->frames = LSA_NULL;
	}

	if (is_not_null (channel->alarms)) {
		ACP_FREE_LOCAL_MEM (&rc, channel->alarms);
		ACP_ASSERT (rc == LSA_RET_OK);
		channel->alarms = LSA_NULL;
	}

	ACP_ASSERT (is_null (channel->detailptr));
/* cannot check - unknown type:
	ACP_ASSERT (is_null (channel->sysptr));
*/

	channel->lower = LSA_NULL;  /* sanity */
}


/*------------------------------------------------------------------------------
//	func: acp_callback_and_free_channel
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_callback_and_free_channel (
	ACP_CHANNEL_PTR  channel,
	LSA_UINT32  result,
	ACP_UPPER_RQB_PTR_TYPE  rb
	)
{
	ACP_LOG_FCT("acp_callback_and_free_channel")
	ACP_UPPER_CALLBACK_FCT_PTR_TYPE  callback;
	ACP_DETAIL_PTR_TYPE  detailptr;
	LSA_SYS_PTR_TYPE  sysptr;
	LSA_UINT16  rc;

	ACP_ASSERT (is_not_null (rb));
	ACP_ASSERT (is_not_null (channel));
	ACP_ASSERT (is_not_null (channel->callback));
	/*
	 * NOTE: the previously used sequence was wrong!
	 *       > acp_callback_async (channel, ACP_OK, & rb);
	 *       > channel->state = ACP_CHANNEL_STATE_FREE;
	 *
	 * ==> even if the callback posts into a queue, a call
	 *     to undo_init() can occurr (due to pre-emption)
	 *     before we have chance to reset the state flag
	 */

	ACP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			"<<< rqb(0x%08x) h(%d) opc(%d) rsp(%d)  h-to(%d)  [acp-channel-closed]",
			rb, channel->my_handle, ACP_RQB_GET_OPCODE (rb), result, channel->handle_upper
		);

	ACP_RQB_SET_HANDLE (rb, channel->handle_upper);

	callback			= channel->callback;
	channel->callback	= LSA_NULL;

	detailptr			= channel->detailptr;
	channel->detailptr	= LSA_NULL;
	sysptr				= channel->sysptr;
/* cannot init:
	channel->sysptr		= LSA_NULL;
*/

	acp_free_channel_mem (channel);
	channel->state = ACP_CHANNEL_STATE_FREE; /* free channel ... */

	ACP_RQB_SET_RESPONSE (rb, (LSA_UINT16)result);

	ACP_REQUEST_UPPER_DONE (callback, rb, sysptr);

	ACP_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
	ACP_ASSERT (rc == LSA_RET_OK);
}


/*------------------------------------------------------------------------------
//	func: acp_callback_async
//	work: set the response of the given request-block and calls the
//		request-done-fct (given by ACP_OPC_OPEN_CHANNEL)
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_callback_async (
	ACP_CHANNEL_PTR channel,
	LSA_UINT32 result,
	ACP_UPPER_RQB_PTR_TYPE ACP_LOCAL_MEM_ATTR * upper
	)
{
	ACP_LOG_FCT("acp_callback_async")
	ACP_UPPER_RQB_PTR_TYPE  upper_tmp;

	ACP_ASSERT (is_not_null (upper));
	ACP_ASSERT (is_not_null (*upper));

	upper_tmp = *upper;	/* AP00224167 */
	*upper = LSA_NULL;

	switch (result) {
	case ACP_OK:
	case ACP_OK_CANCEL:
		ACP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "<<< rqb(0x%08x) h(%d) opc(%d) rsp(%d)  to-h(%d)"
			, upper_tmp, channel->my_handle
			, ACP_RQB_GET_OPCODE (upper_tmp), result, channel->handle_upper
			);
		break;

	case ACP_ERR_ASRT:
		ACP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "<<< rqb(0x%08x) h(%d) opc(%d) rsp(%d)  to-h(%d)"
			, upper_tmp, channel->my_handle
			, ACP_RQB_GET_OPCODE (upper_tmp), result, channel->handle_upper
			);
		break;

	default:
		ACP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "<<< rqb(0x%08x) h(%d) opc(%d) rsp(%d)  to-h(%d)"
			, upper_tmp, channel->my_handle
			, ACP_RQB_GET_OPCODE (upper_tmp), result, channel->handle_upper
			);
		break;
	}

	ACP_RQB_SET_HANDLE (upper_tmp, channel->handle_upper);
	ACP_RQB_SET_RESPONSE (upper_tmp, (LSA_UINT16)result);

	ACP_REQUEST_UPPER_DONE (channel->callback, upper_tmp, channel->sysptr);
}


/*------------------------------------------------------------------------------
// work: returns pointer to channel-struct if channel_handle is Ok, else LSA_NULL
//----------------------------------------------------------------------------*/
ACP_CHANNEL_PTR  ACP_LOCAL_FCT_ATTR
acp_channel_from_handle (
	LSA_HANDLE_TYPE  channel_handle
	)
{
	ACP_LOG_FCT("acp_channel_from_handle")

	if (1 <= channel_handle  &&  channel_handle <= ACP_CFG_MAX_CHANNELS) {
		return &acp_channels[channel_handle - 1];
	}

	ACP_UPPER_TRACE_02 (0, LSA_TRACE_LEVEL_ERROR
		, "! 1 <= channel_handel(%d) < max(%d)"
		, channel_handle, ACP_CFG_MAX_CHANNELS
		);
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// work: counts the number of channels with the given contitions
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_channel_count (
	LSA_UINT16  channel_type,
	LSA_UINT16  nic_id
	)
{
	ACP_LOG_FCT("acp_channel_count")
	LSA_UINT16  i;
	LSA_UINT16  cnt;

	cnt = 0;
	for (i = 0; i < ACP_CFG_MAX_CHANNELS; ++i)
	{
		ACP_CHANNEL_PTR  channel = & acp_channels[i];

		if (channel->state != ACP_CHANNEL_STATE_READY) {
			continue;
		}
		if (channel->detailptr->channel_type == channel_type
			&&  channel->detailptr->nic_id == nic_id)
		{
			++cnt;
		}
	}

	return cnt;
}


/*------------------------------------------------------------------------------
//	work: creating a lower instanz
//		count_of_open_channel:
//		if 0:			lower not used
//		if 1..N:		lower in use
//		if 0xFF00:		lower reserved, first channel with this nic_id, must edd_open_channel()
//		if 0xFFFF:		lower reserved, waiting for edd_close_channel.cnf, do not use for a new open
//----------------------------------------------------------------------------*/
#if ACP_CFG_MAX_CHANNELS >= 0xFF00
# error "config-error: check this ..."
#endif

ACP_LOWER_PTR  ACP_LOCAL_FCT_ATTR
acp_open_lower_layer (
	LSA_UINT16  nic_id,
	LSA_SYS_PTR_TYPE  sysptr
	)
{
	ACP_LOG_FCT("acp_open_lower_layer")
	ACP_LOWER_PTR  lower;
	LSA_UINT16  i;

	for (i = 0; i < ACP_CFG_MAX_CHANNELS; ++i) {
		lower = & acp_lowers[i];
		if (lower->count_of_open_channel == 0) {
			lower->nic_id = nic_id;
			lower->count_of_open_channel = 0xFF00;  /* nid_id is reserved */

			lower->handle_lower = 0;
			lower->trace_idx = 0; /* the "global" trace_idx */

			lower->sysptr = sysptr;

				/* Link-Status */
			AcpListInitialize (& lower->lower_link_list);

				/* overload */
			lower->overload_edd_running = LSA_FALSE;
			lower->overload.rx_counter = 0;
			lower->overload.tx_counter = 0;

				/* q_or_send */
			AcpListInitialize (& lower->q_or_send.nrt_send_list);
			AcpListInitialize (& lower->q_or_send.data_list);
			AcpListInitialize (& lower->q_or_send.ack_list);

			return lower;
		}
	}
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	work: searching a lower instanz
//----------------------------------------------------------------------------*/
ACP_LOWER_PTR  ACP_LOCAL_FCT_ATTR
acp_channel_find_lower (
	LSA_UINT16  nic_id
	)
{
	ACP_LOG_FCT("acp_channel_find_lower")
	ACP_LOWER_PTR  lower;
	LSA_UINT16  i;

	for (i = 0; i < ACP_CFG_MAX_CHANNELS; ++i) {
		lower = & acp_lowers[i];
		if (lower->count_of_open_channel >= 0xFF00) {
			ACP_UPPER_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
				, "ignore this entry, open-or-close is running, i(%d)"
				, i
				);
			continue;
		}
		if (lower->nic_id == nic_id) {
			return lower;
		}
	}
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	func: acp_open_channel
//	work: create an ACP instanz, open or reuse an communication-channel to the
//			lower layer
//----------------------------------------------------------------------------*/
LSA_VOID ACP_UPPER_IN_FCT_ATTR
acp_open_channel (
	ACP_UPPER_RQB_PTR_TYPE  rb
	)
{
	ACP_LOG_FCT("acp_open_channel")
	LSA_HANDLE_TYPE  my_handle;
	ACP_CHANNEL_PTR  channel;
	LSA_UINT16  rc;
	LSA_UINT32  long_size;
	ACP_UPPER_OPEN_CHANNEL_PTR_TYPE  open;
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_OPEN_CHANNEL_PTR_TYPE  edd_open;
	LSA_UINT16  cnt;
	ACP_DETAIL_PTR_TYPE  detailptr;
	LSA_SYS_PTR_TYPE  sysptr;

		/* param check */
	if (is_null (rb)  ||  ACP_RQB_GET_OPCODE (rb) != ACP_OPC_OPEN_CHANNEL) {
		ACP_FATAL (0);
	}

	open = & rb->args->channel.open;

	if (is_null (open) || is_null (open->acp_request_upper_done_ptr)) {
		ACP_FATAL (0);
	}

		/* get required infos */
	detailptr = LSA_NULL;
	ACP_GET_PATH_INFO (& rc, & sysptr, & detailptr, open->sys_path);
	if (rc != LSA_RET_OK || is_null(detailptr)) {
		ACP_FATAL (0); /*AP00998998*/
	}

		/* searching a free acp-handle */
	channel = LSA_NULL;
	for (my_handle = 0; my_handle < ACP_CFG_MAX_CHANNELS; ++my_handle) {
		channel = & acp_channels[my_handle];
		if (channel->state == ACP_CHANNEL_STATE_FREE) {
			channel->state = ACP_CHANNEL_STATE_RESERVED;
			break;
		}
	}
	if (my_handle >= ACP_CFG_MAX_CHANNELS) {
		ACP_RQB_SET_RESPONSE (rb, ACP_ERR_RESOURCE);
		ACP_RQB_SET_HANDLE (rb, open->handle_upper);
		ACP_REQUEST_UPPER_DONE (open->acp_request_upper_done_ptr, rb, sysptr);
		ACP_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
		ACP_ASSERT (rc == LSA_RET_OK);
		return;
	}

		/* set bad result */
	ACP_RQB_SET_RESPONSE (rb, ACP_ERR_RESOURCE);
	ACP_RQB_SET_HANDLE (rb, ACP_INVALID_HANDLE);
	open->handle = ACP_INVALID_HANDLE;

		/* channel with indx my_handle is used */

	my_handle += 1;				/* working with indx >= 1, 0 is ACP_INVALID_HANDLE */

	channel->my_handle			= my_handle;
	channel->state				= ACP_CHANNEL_STATE_RESERVED;

	channel->sysptr				= sysptr;
	channel->detailptr			= detailptr;

	channel->trace_idx			= 0; /* the "global" trace_idx */

	channel->mac_addr[0]		= 0;
	channel->mac_addr[1]		= 0;
	channel->mac_addr[2]		= 0;
	channel->mac_addr[3]		= 0;
	channel->mac_addr[4]		= 0;
	channel->mac_addr[5]		= 0;

	channel->max_eth_ports		= 0;

	channel->ip_addr[0]			= 0;
	channel->ip_addr[1]			= 0;
	channel->ip_addr[2]			= 0;
	channel->ip_addr[3]			= 0;

	channel->callback			= open->acp_request_upper_done_ptr;
	channel->handle_upper		= open->handle_upper;
	channel->lower				= LSA_NULL;

	channel->upper				= rb;
	channel->upper_list			= LSA_NULL;
	channel->upper_is_busy		= LSA_FALSE;

	channel->max_group_id		= 0;
	channel->max_frames			= 0;
	channel->frames				= LSA_NULL;
	channel->provide			= LSA_NULL;
	AcpListInitialize (			& channel->link_status_list);
	channel->overload_ind		= LSA_NULL;
	channel->do_event_prov_drop	= 0;

	channel->max_alarms			= 0;
	channel->alarms				= LSA_NULL;

	channel->alarm_provide[ACP_ALARM_PRIORITY_LOW]	= LSA_NULL;
	channel->alarm_provide[ACP_ALARM_PRIORITY_HIGH]	= LSA_NULL;
	channel->alarm_lower[ACP_ALARM_PRIORITY_LOW]	= LSA_NULL;
	channel->alarm_lower[ACP_ALARM_PRIORITY_HIGH]	= LSA_NULL;

	ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "acp_handle(%d)"
		, channel->my_handle
		);

	switch (channel->detailptr->channel_type) {
	case ACP_CHANNEL_TYPE_SYSTEM:
		cnt = acp_channel_count (ACP_CHANNEL_TYPE_SYSTEM, channel->detailptr->nic_id);
		if (cnt > 0) {
			ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "cnt(%d) > 0: sys-channel for nic_id(%d) already open"
				, cnt
				, channel->detailptr->nic_id
				);
			acp_callback_and_free_channel (channel, ACP_ERR_SEQUENCE, channel->upper);
			return;
		}

		channel->lower = acp_open_lower_layer (channel->detailptr->nic_id, sysptr);
		if (is_null (channel->lower)) {
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!open lower, nic_id(%d)"
				, channel->detailptr->nic_id
				);
			acp_callback_and_free_channel (channel, ACP_ERR_SYS_PATH, channel->upper);
			return;
		}

			/* has to open the lower layer, callback is done in request_done() */
		lower = acp_ll_alloc_rqb (channel->lower);
		if (is_null (lower)) {
			ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!rqb 4 edd_open"
				);
			channel->lower->count_of_open_channel = 0; /* free lower */
			acp_callback_and_free_channel (channel, ACP_ERR_ALLOC_LOWER_RQB, channel->upper);
			return;
		}
		ACP_LL_SET_OPCODE (lower, EDD_OPC_OPEN_CHANNEL);
		ACP_LL_SET_SERVICE (lower, 0);
		ACP_LL_SET_HANDLE (lower, 0);
		ACP_LL_SET_USERID_PTR (lower, channel);

		edd_open = ACP_LL_GET_PPARAM (lower, ACP_LL_OPEN_CHANNEL_PTR_TYPE);
		edd_open->Cbf			= acp_request_lower_done;
		edd_open->SysPath		= open->sys_path;
		edd_open->HandleUpper	= my_handle;

		ACP_RQB_SET_HANDLE (rb, my_handle); /* see _alloc_ troubles in acp_channel_open_done() */

		ACP_OPEN_CHANNEL_LOWER (lower, channel->lower->sysptr);
		return;

	case ACP_CHANNEL_TYPE_USER:
		cnt = acp_channel_count (ACP_CHANNEL_TYPE_SYSTEM, channel->detailptr->nic_id);
		if (cnt == 0) {
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "sys-channel for nic_id(%d) not open"
				, channel->detailptr->nic_id
				);
			acp_callback_and_free_channel (channel, ACP_ERR_SEQUENCE, channel->upper);
			return;
		}

			/* searching lower, same nic_id leeds to the same ethernet-nic */
		channel->lower = acp_channel_find_lower (channel->detailptr->nic_id);
		if (is_null (channel->lower)) {
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!find lower, nic_id(%d)"
				, channel->detailptr->nic_id
				);
			acp_callback_and_free_channel (channel, ACP_ERR_SYS_PATH, channel->upper);
			return;
		}

		if (channel->detailptr->type.user.group_id >= EDD_CFG_CSRT_MAX_PROVIDER_GROUP) {
			ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "detailptr.group_id(%d) >= max(%d)"
				, channel->detailptr->type.user.group_id
				, EDD_CFG_CSRT_MAX_PROVIDER_GROUP
				);
			acp_callback_and_free_channel (channel, ACP_ERR_SYS_PATH, channel->upper);
			return;
		}

		ACP_UPPER_TRACE_10 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "sys_path(%d) nic_id(%d) group_id(%d) class1_unicast(0x%x/%d) class1_multicast(0x%x/%d) class1_2_prov(%d) base_aref(%d) nr_aref(%d)"
			, open->sys_path
			, channel->detailptr->nic_id
			, channel->detailptr->type.user.group_id
			, channel->detailptr->type.user.frame_id_base_class1_unicast
			, channel->detailptr->type.user.nr_of_consumers_class1_unicast
			, channel->detailptr->type.user.frame_id_base_class1_multicast
			, channel->detailptr->type.user.nr_of_consumers_class1_multicast
			, channel->detailptr->type.user.nr_of_providers_class1_2
			, channel->detailptr->type.user.alarm_ref_base
			, channel->detailptr->type.user.alarm_nr_of_ref
			);

			/* calculate needed space, add 1 because index is beginning at 1 */
		long_size = 1;
		long_size += channel->detailptr->type.user.nr_of_consumers_class1_unicast;
		long_size += channel->detailptr->type.user.nr_of_consumers_class1_multicast;
		long_size += channel->detailptr->type.user.nr_of_consumers_class2_unicast;
		long_size += channel->detailptr->type.user.nr_of_consumers_class2_multicast;
		long_size += channel->detailptr->type.user.nr_of_providers_class1_2;
		long_size += channel->detailptr->type.user.nr_of_consumers_class3;
		long_size += channel->detailptr->type.user.nr_of_providers_class3;
		channel->max_frames = (LSA_UINT16)long_size;

			/* alloc mem for frame-data */
		long_size = sizeof(ACP_LOCAL_FRAME) * channel->max_frames;
		ACP_ALLOC_LOCAL_MEM (ACP_CAST_LOCAL_MEM_PTR_PTR (& channel->frames), long_size);
		if (is_null (channel->frames))
		{
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!local_mem 4 frames size(%d)", long_size
				);
			acp_callback_and_free_channel (channel, ACP_ERR_ALLOC_LOCAL_MEM, channel->upper);
			return;
		}

		for (rc = 1; rc < channel->max_frames; ++rc) {
			channel->frames[rc].state = ACP_FS_NO;
		}

			/* alloc mem for alarm-data */
		long_size = sizeof(ACP_LOCAL_ALARM_TYPE)/*172Bytes*/ * channel->detailptr->type.user.alarm_nr_of_ref;
		if (long_size > 0)
		{
			ACP_ALLOC_LOCAL_MEM (ACP_CAST_LOCAL_MEM_PTR_PTR (& channel->alarms), long_size);
			if (is_null (channel->alarms))
			{
				ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "!local_mem 4 alarms, size(%d)", long_size
					);
				acp_callback_and_free_channel (channel, ACP_ERR_ALLOC_LOCAL_MEM, channel->upper);
				return;
			}

			long_size = channel->detailptr->type.user.alarm_ref_base + channel->detailptr->type.user.alarm_nr_of_ref;
			if (long_size > 0xffff) {
				ACP_FATAL (long_size); /* alarm_ref out of range */
			}

			for (rc = 0; rc < channel->detailptr->type.user.alarm_nr_of_ref; ++rc)
			{
				/* ALARM */
				channel->alarms[rc].alarm_ref = ACP_CAST_U16 (channel->detailptr->type.user.alarm_ref_base + rc);
				channel->alarms[rc].state = ACP_ALARM_ST_FREE;
				/* RTA */
				channel->alarms[rc].low.state = ACP_RTA_ST_FREE;
				channel->alarms[rc].high.state = ACP_RTA_ST_FREE;
			}
		}

			/* set result and acp_open_channel.cnf(+) */
		open->handle = channel->my_handle;
		channel->lower->count_of_open_channel += 1;
		channel->state = ACP_CHANNEL_STATE_READY;
		acp_callback (channel, ACP_OK);
		return;

	default:
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "!channel_type(%d)"
			, channel->detailptr->channel_type
			);
		acp_callback_and_free_channel (channel, ACP_ERR_SYS_PATH, channel->upper);
		return;
	}
}


/*------------------------------------------------------------------------------
//	func: acp_close_channel
//	work: closes an ACP instanz, (maybe) close the channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID ACP_UPPER_IN_FCT_ATTR
acp_close_channel (
	ACP_UPPER_RQB_PTR_TYPE  rb
	)
{
	ACP_LOG_FCT("acp_close_channel")
	ACP_CHANNEL_PTR  channel;
	ACP_LOWER_RQB_PTR_TYPE  lower;
	LSA_UINT16  cnt;

	ACP_ASSERT (is_not_null (rb));

	if (ACP_RQB_GET_OPCODE (rb) == ACP_OPC_CLOSE_CHANNEL) {

		channel = acp_channel_from_handle (ACP_RQB_GET_HANDLE (rb));
		if (is_null (channel)) {
			ACP_FATAL (ACP_RQB_GET_HANDLE (rb));
		}

		if (is_not_null (channel->upper)) {
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "req running, opc(%d)"
				, ACP_RQB_GET_OPCODE (channel->upper)
				);
			acp_callback_async (channel, ACP_ERR_SEQUENCE, & rb);
			return;
		}
		channel->upper = rb;
	}

		/* comes from acp_open_channel_done() */
	else if (ACP_RQB_GET_OPCODE (rb) == ACP_OPC_OPEN_CHANNEL) {

		channel = acp_channel_from_handle (ACP_RQB_GET_HANDLE (rb));
		if (is_null (channel)) {
			ACP_FATAL (ACP_RQB_GET_HANDLE (rb));
		}

		ACP_ASSERT (are_equal (rb, channel->upper));
		ACP_ASSERT (channel->state == ACP_CHANNEL_STATE_READY);
	}

	else {
		ACP_FATAL (ACP_RQB_GET_OPCODE (rb));
		return; /* never */
	}

	ACP_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "acp_handle(%d) state(%d) lower.cnt(0x%x) channel_type(%d)"
		, channel->my_handle
		, channel->state
		, channel->lower->count_of_open_channel
		, channel->detailptr->channel_type
		);

	if (channel->state != ACP_CHANNEL_STATE_READY) {
		acp_callback (channel, ACP_ERR_SEQUENCE);
		return;
	}

	switch (channel->detailptr->channel_type) {
	case ACP_CHANNEL_TYPE_SYSTEM:
			/* system-channel must be the last channel */
		cnt = acp_channel_count (ACP_CHANNEL_TYPE_USER, channel->detailptr->nic_id);
		if (cnt > 0) {
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "cnt(%d) open user-channel"
				, cnt
				);
			acp_callback_async (channel, ACP_ERR_SEQUENCE, & channel->upper);
			return;
		}

			/* lock this channel->lower for new acp_open_channel, see acp_channel_find_lower() */
		ACP_ASSERT (channel->lower->count_of_open_channel == 1);
		channel->lower->count_of_open_channel = 0xFFFF;

			/* no more acp-instanz, close the lower */
		lower = acp_ll_alloc_rqb (channel->lower);
		if (is_null (lower)) {
			ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!rqb 4 edd_close"
				);
			acp_callback_async (channel, ACP_ERR_ALLOC_LOWER_RQB, & channel->upper);
			return;
		}

			/* must cancel nrt_recv first */
		{
		ACP_LL_NRT_CANCEL_PTR_TYPE  nrt_cancel;

		nrt_cancel = ACP_LL_GET_PPARAM (lower, ACP_LL_NRT_CANCEL_PTR_TYPE);
		nrt_cancel->Mode = EDD_NRT_CANCEL_MODE_ALL;
		nrt_cancel->RequestID = 0;

		acp_ll_request (channel->lower, lower, EDD_SRV_NRT_CANCEL, channel);
		}
		return;

	case ACP_CHANNEL_TYPE_USER:
			/* if there are added cons or prod => sequence-error */
		if (acp_frame_count (channel) > 0) {
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "open frames(%d)"
				, acp_frame_count (channel)
				);
			acp_callback (channel, ACP_ERR_SEQUENCE);
			return;
		}

			/* if there are open alarms => sequence-error */
		if (acp_alarm_count_open (channel) > 0) {
			ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "open alarms(%d)"
				, acp_alarm_count_open (channel)
				);
			acp_callback (channel, ACP_ERR_SEQUENCE);
			return;
		}

		channel->lower->count_of_open_channel -= 1;
		ACP_ASSERT (channel->lower->count_of_open_channel > 0);
		acp_free_resource (channel);
		acp_callback_and_free_channel (channel, ACP_OK, channel->upper);
		return;

	default:
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "!channel_type(%d)"
			, channel->detailptr->channel_type
			);
		acp_callback_and_free_channel (channel, ACP_ERR_SYS_PATH, channel->upper);
		return;
	}
}


/*------------------------------------------------------------------------------
//	func: acp_request
//	work: receives an request from the user, queue the request
//----------------------------------------------------------------------------*/
LSA_VOID ACP_UPPER_IN_FCT_ATTR
acp_request (
	ACP_UPPER_RQB_PTR_TYPE  rb
	)
{
	ACP_LOG_FCT("acp_request")
	ACP_CHANNEL_PTR  channel;

	if (is_null (rb)) {
		ACP_FATAL (0);
	}

		/* timer is special */
	if (ACP_RQB_GET_OPCODE (rb) == ACP_OPC_INTERNAL_TIMER) {
		acp_timeout_synchronized (rb);
		return;
	}

	channel = acp_channel_from_handle (ACP_RQB_GET_HANDLE (rb));
	if (is_null (channel)) {
		ACP_FATAL (ACP_RQB_GET_HANDLE (rb));
	}

	if (acp_add_to_rqb_list (&(channel->upper_list), rb) == 0) {
		ACP_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW,
				">>> rqb(0x%08x) h(%d) queueing,  upper_is_busy(%u)",
				rb, ACP_RQB_GET_HANDLE (rb), channel->upper_is_busy
			);
	}

	acp_run_request (channel);
}


/*------------------------------------------------------------------------------
//	func: acp_run_request
//	work: run one queued request
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_run_request (
	ACP_CHANNEL_PTR channel
	)
{
	ACP_LOG_FCT("acp_run_request")
	ACP_LOCAL_FRAME_PTR  frame;
	ACP_UPPER_RQB_PTR_TYPE  rb;

	ACP_ASSERT (channel);

		/* end if while loop is running */
	if (channel->upper_is_busy)
	{
		return;
	}
	channel->upper_is_busy = LSA_TRUE;

		/* end if one request is currently running, must call again later */
	while (is_null (channel->upper)) {

			/* get one request from list */
		(LSA_VOID) acp_get_from_rqb_list (& channel->upper_list, & channel->upper);
		rb = channel->upper;

			/* end if no (more) request is pending */
		if (is_null (channel->upper)) {
/*			ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "no (more) rqb pending"
			);
*/
			channel->upper_is_busy = LSA_FALSE;
			return;
		}

		ACP_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
				">>> rqb(0x%08x) h(%d) opc(%d)",
				rb, channel->my_handle, ACP_RQB_GET_OPCODE (rb)
			);

			/* common check */
		if (is_null (rb->args)) {
			ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!rb->args"
				);
			acp_callback (channel, ACP_ERR_PARAM);
			break;
		}

		switch (ACP_RQB_GET_OPCODE (rb)) {
		case ACP_OPC_CHANNEL_INFO:
			acp_op_channel_info (channel);
			break;

		case ACP_OPC_STATE_IND_RES_PROVIDE:
			acp_op_state_provide (channel);
			break;

		case ACP_OPC_LINK_STATUS_IND_RES_PROVIDE:
			acp_op_link_status_provide (channel);
			break;

		case ACP_OPC_OVERLOAD_IND_RES_PROVIDE:
			acp_op_overload_provide (channel);
			break;

		case ACP_OPC_MULTICAST:
			acp_op_multicast (channel);
			break;

		case ACP_OPC_NOP:
			acp_callback (channel, ACP_OK);
			break;

/*----------------------------------------------------------------------------*/

		case ACP_OPC_PROVIDER_ADD:
			if ((rb->args->prov.add.properties & EDD_CSRT_PROV_PROP_SYSRED) != 0) {
				frame = acp_frame_new (channel, ACP_FO_PROVIDER_SYSRED);
			}
			else {
				frame = acp_frame_new (channel, ACP_FO_PROVIDER_SRT);
			}
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_RESOURCE);
				break;
			}
			acp_op_provider_add (channel, frame);
			break;

		case ACP_OPC_PROVIDER_CONTROL:
			frame = acp_frame_find (channel, rb->args->prov.control.prov_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_provider_control (channel, frame);
			break;

		case ACP_OPC_PROVIDER_XCHANGE_BUF:
			frame = acp_frame_find (channel, rb->args->prov.xbuf.prov_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_provider_xbuf (channel, frame);
			break;

		case ACP_OPC_GLOBAL_PROVIDER_CONTROL:
			acp_op_global_provider_control (channel);
			break;

		case ACP_OPC_PROVIDER_CHANGE_PHASE:
			frame = acp_frame_find (channel, rb->args->prov.change_phase.prov_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_provider_change_phase (channel, frame);
			break;

		case ACP_OPC_PROVIDER_REMOVE:
			frame = acp_frame_find (channel, rb->args->prov.remove.prov_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_provider_remove (channel, frame);
			break;

/*----------------------------------------------------------------------------*/

		case ACP_OPC_CONSUMER_ADD:
			if ((rb->args->cons.add.properties & EDD_CSRT_CONS_PROP_SYSRED) != 0) {
				frame = acp_frame_new (channel, ACP_FO_CONSUMER_SYSRED);
			}
			else {
				frame = acp_frame_new (channel, ACP_FO_CONSUMER_SRT);
			}
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_RESOURCE);
				break;
			}
			acp_op_consumer_add (channel, frame);
			break;

		case ACP_OPC_CONSUMER_CONTROL:
			frame = acp_frame_find (channel, rb->args->cons.control.cons_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_consumer_control (channel, frame);
			break;

		case ACP_OPC_CONSUMER_XCHANGE_BUF:
			frame = acp_frame_find (channel, rb->args->cons.xbuf.cons_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_consumer_xbuf (channel, frame);
			break;

		case ACP_OPC_CONSUMER_REMOVE:
			frame = acp_frame_find (channel, rb->args->cons.remove.cons_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_consumer_remove (channel, frame);
			break;

/*----------------------------------------------------------------------------*/

		case ACP_OPC_GET_APDU_STATUS:
			frame = acp_frame_find (channel, rb->args->channel.apdu_status.acp_id);
			if (is_null (frame)) {
				acp_callback (channel, ACP_ERR_PARAM);
				break;
			}
			acp_op_get_apdu_status (channel, frame);
			break;

/*----------------------------------------------------------------------------*/

		case ACP_OPC_ALARM_OPEN:
			acp_op_alarm_open (channel, LSA_FALSE);
			break;

		case ACP_OPC_UDP_ALARM_OPEN:
			acp_op_alarm_open (channel, LSA_TRUE);
			break;

		case ACP_OPC_ALARM_CLOSE:
			acp_op_alarm_close (channel);
			break;

		case ACP_OPC_ALARM_SEND:
			channel->upper = LSA_NULL;
			acp_op_alarm_send (channel, rb);
			break;

		case ACP_OPC_ALARM_ACK_SEND:
			channel->upper = LSA_NULL;
			acp_op_alarm_ack_send (channel, rb);
			break;

		case ACP_OPC_ALARM_IND_RES_PROVIDE:
			channel->upper = LSA_NULL;
			acp_op_alarm_provide (channel, rb);
			break;

/*----------------------------------------------------------------------------*/
		case ACP_OPC_STATE_INDICATION:
		case ACP_OPC_LINK_STATUS_INDICATION:
		case ACP_OPC_OVERLOAD_INDICATION:
			ACP_FATAL (ACP_RQB_GET_OPCODE (rb));
			break;

		default:
			acp_callback (channel, ACP_ERR_UNSUPPORTED);
			break;
		}
	}

	channel->upper_is_busy = LSA_FALSE;
	return;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

