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
/*  F i l e               &F: acp_low.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the lower LSA interface                                       */
/*                                                                           */
/*****************************************************************************/
#define ACP_MODULE_ID      4
#define LTRC_ACT_MODUL_ID  4

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)


/*----------------------------------------------------------------------------*/

/* works only with some assumptions, see acp_pls.c(ACP_STATE_INDICATION_TYPE) */
#define ACP_STATE_TOGGLE(edd_event, acp_state_var, to_set, to_reset)	{\
	if ((edd_event) & (to_set)) { \
		acp_state_var |= (to_set); \
		acp_state_var &= ~(to_reset); \
	} \
	else if ((edd_event) & (to_reset)) { \
		acp_state_var |= (to_reset); \
		acp_state_var &= ~(to_set); \
	} \
}



/*------------------------------------------------------------------------------
//	alloc a lower request-block, with an 'one-size-fits-all' pParam
//----------------------------------------------------------------------------*/
ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_FCT_ATTR
acp_ll_alloc_rqb (
	ACP_LOWER_PTR			channel_lower
	)
{
	ACP_LOG_FCT("acp_ll_alloc_rqb")
	ACP_LOWER_RQB_PTR_TYPE  edd;
	LSA_UINT16	rc;

	ACP_ALLOC_LOWER_RQB (ACP_CAST_LOWER_RQB_PTR_PTR (& edd),
			acp_null_usr_id, sizeof(ACP_LOWER_RQB_TYPE), channel_lower->sysptr
		);
	if (is_null (edd)) {
		ACP_LL_TRACE_00 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "!lower_rqb"
			);
		return LSA_NULL;
	}

	rc = sizeof (ACP_LL_USED_DATA_TYPE);

	ACP_ALLOC_LOWER_MEM (ACP_CAST_LOWER_MEM_PTR_PTR (& edd->pParam),
			acp_null_usr_id, rc, channel_lower->sysptr
		);
	if (is_null (edd->pParam)) {
		ACP_LL_TRACE_00 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "!lower_mem"
			);
		ACP_FREE_LOWER_RQB (& rc, edd, channel_lower->sysptr);
		ACP_ASSERT (rc == LSA_RET_OK);
		return LSA_NULL;
	}
	return edd;
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_ll_request (
	ACP_LOWER_PTR  channel_lower,
    ACP_LOWER_RQB_PTR_TYPE  edd,
	EDD_SERVICE  service,
	LSA_VOID_PTR_TYPE  user_id
	)
{
	ACP_LOG_FCT("acp_ll_request")

	ACP_LL_SET_HANDLE (edd, channel_lower->handle_lower);
	ACP_LL_SET_OPCODE (edd, EDD_OPC_REQUEST);
	ACP_LL_SET_SERVICE (edd, service);
	ACP_LL_SET_USERID_PTR (edd, user_id);

	ACP_ASSERT (
			/* not closing */
			channel_lower->count_of_open_channel != 0xFFFF
		||	/* closing and cancel */
			(channel_lower->count_of_open_channel == 0xFFFF  &&  service == EDD_SRV_NRT_CANCEL)
		);

	ACP_LL_TRACE_05 (channel_lower->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW,
			">>> lower(%08x) h(%u) opc(%u) service(0x%x) user_id(%#x)",
			edd, ACP_LL_GET_HANDLE (edd),
			ACP_LL_GET_OPCODE (edd), ACP_LL_GET_SERVICE (edd),
			ACP_LL_GET_USERID_PTR (edd)
		);

	ACP_REQUEST_LOWER (edd, channel_lower->sysptr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_ll_free_rqb (
	ACP_LOWER_PTR			channel_lower,
    ACP_LOWER_RQB_PTR_TYPE	lower_rqb_ptr
	)
{
	ACP_LOG_FCT("acp_ll_free_rqb")
	LSA_UINT16	rc;

	if (is_not_null (ACP_LL_GET_PPARAM (lower_rqb_ptr, LSA_VOID_PTR_TYPE))) {
		ACP_FREE_LOWER_MEM (& rc, ACP_LL_GET_PPARAM (lower_rqb_ptr, ACP_LOWER_MEM_PTR_TYPE), channel_lower->sysptr);
		ACP_ASSERT (rc == LSA_RET_OK);
		ACP_LL_SET_PPARAM (lower_rqb_ptr, LSA_NULL);
	}
	ACP_FREE_LOWER_RQB (& rc, lower_rqb_ptr, channel_lower->sysptr);
	ACP_ASSERT (rc == LSA_RET_OK);
}


/*----------------------------------------------------------------------------*/
ACP_LINK_STATUS_PTR  ACP_LOCAL_FCT_ATTR
acp_link_status_find (
	ACP_LOWER_PTR  channel_lower,
	LSA_UINT16  port_id
	)
{
	ACP_LOG_FCT("acp_link_status_find")
	ACP_LINK_STATUS_PTR  link;

	for (AcpListForeach (link, & channel_lower->lower_link_list, ACP_LINK_STATUS_PTR)) {
		if (port_id == link->port_id) {
			return link;
		}
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_link_status_alloc (
	ACP_LOWER_PTR  channel_lower,
	ACP_UPPER_RQB_PTR_TYPE  upper
	)
{
	ACP_LOG_FCT("acp_link_status_alloc")
	ACP_LINK_STATUS_PTR  acp_link;
	ACP_LOWER_RQB_PTR_TYPE  edd;
	ACP_LL_LINK_STATUS_PROVIDE_PTR_TYPE  edd_link;

	ACP_ALLOC_LOCAL_MEM (ACP_CAST_LOCAL_MEM_PTR_PTR (& acp_link), sizeof(*acp_link));
	if (is_null (acp_link)) {
		return ACP_ERR_ALLOC_LOCAL_MEM;
	}

	edd = acp_ll_alloc_rqb (channel_lower);
	if (is_null (edd)) {
		LSA_UINT16  rc;
		ACP_FREE_LOCAL_MEM (& rc, acp_link);
		ACP_ASSERT (rc == LSA_RET_OK);
		return ACP_ERR_ALLOC_LOWER_RQB;
	}

	acp_link->port_id = upper->args->channel.link.port_id;
	acp_link->status = upper->args->channel.link.status;
	acp_link->speed = upper->args->channel.link.speed;
	acp_link->mode = upper->args->channel.link.mode;

	AcpListInsertTail (& channel_lower->lower_link_list, & acp_link->link);

	edd_link = ACP_LL_GET_PPARAM (edd, ACP_LL_LINK_STATUS_PROVIDE_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_link));

	edd_link->PortID = upper->args->channel.link.port_id;

	edd_link->Status = upper->args->channel.link.status;
	edd_link->Speed = upper->args->channel.link.speed;
	edd_link->Mode = upper->args->channel.link.mode;

	/* sanity AP00754357 */
	edd_link->PhyStatus = EDD_PORT_PRESENT;
	edd_link->MAUType = EDD_MAUTYPE_UNKNOWN;

	acp_ll_request (channel_lower, edd, EDD_SRV_LINK_STATUS_IND_PROVIDE, channel_lower);

	return ACP_OK;
}


/*----------------------------------------------------------------------------*/
/*
// ACP_OK .........  diff exists, current state copyed to upper
// ACP_OK_CANCEL ..  no diff exist, must wait for lower .ind
*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_link_status_compare_set (
	ACP_LOWER_PTR  channel_lower,
	ACP_LINK_STATUS_PTR  acp_link,
	ACP_UPPER_RQB_PTR_TYPE  upper
	)
{
	ACP_LOG_FCT("acp_link_status_compare_set")
	LSA_BOOL  differ;

	LSA_UNUSED_ARG (channel_lower);

	ACP_ASSERT (acp_link->port_id == upper->args->channel.link.port_id);

	if (acp_link->status == ACP_LINK_STATUS_UNKNOWN) {
		differ = LSA_FALSE;
	}
	else if (upper->args->channel.link.status != acp_link->status) {
		differ = LSA_TRUE;
	}
	else {
		ACP_ASSERT (upper->args->channel.link.status == acp_link->status);

		differ = LSA_FALSE;
		if (acp_link->status == ACP_LINK_STATUS_UP) {
			if (upper->args->channel.link.speed != acp_link->speed
			||	upper->args->channel.link.mode != acp_link->mode)
			{
				differ = LSA_TRUE;
			}
		}
	}

	if (differ) {
		upper->args->channel.link.status = acp_link->status;
		upper->args->channel.link.speed = acp_link->speed;
		upper->args->channel.link.mode = acp_link->mode;
		return ACP_OK;
	}
	return ACP_OK_CANCEL;
}


/*------------------------------------------------------------------------------
//	starting lower-cycle for handling Opcode ACP_OPC_OVERLOAD_IND_RES_PROVIDE
//----------------------------------------------------------------------------*/
LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_edd_overload_provide (
	ACP_LOWER_PTR  channel_lower
	)
{
	ACP_LOG_FCT("acp_op_overload_provide")
	ACP_LOWER_RQB_PTR_TYPE  edd;
	ACP_LL_RX_OVERLOAD_PROVIDE_PTR_TYPE  edd_rx_overload;

	ACP_ASSERT (! channel_lower->overload_edd_running);

	edd = acp_ll_alloc_rqb (channel_lower);
	if (is_null (edd)) {
		return LSA_FALSE;
	}

	edd_rx_overload = ACP_LL_GET_PPARAM (edd, ACP_LL_RX_OVERLOAD_PROVIDE_PTR_TYPE);
	edd_rx_overload->Status = EDD_NO_RX_OVERLOAD;

	channel_lower->overload_edd_running = LSA_TRUE;
	acp_ll_request (channel_lower, edd, EDD_SRV_RX_OVERLOAD_IND_PROVIDE, channel_lower);
	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_edd_overload_provide_done (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_overload_provide_done")
	LSA_UINT16  rsp;

	ACP_ASSERT (is_not_null (lower));

	rsp = ACP_LL_GET_RESPONSE (lower);

	if (rsp == EDD_STS_OK_CANCEL
	||	channel_lower->count_of_open_channel == 0xFFFF/* on closing */)
	{
		acp_ll_free_rqb (channel_lower, lower);
		channel_lower->overload_edd_running = LSA_FALSE;
	}
	else if (rsp == EDD_STS_OK)
	{
		ACP_LL_RX_OVERLOAD_PROVIDE_PTR_TYPE  edd_rx_overload = ACP_LL_GET_PPARAM (lower, ACP_LL_RX_OVERLOAD_PROVIDE_PTR_TYPE);

		if (edd_rx_overload->Status == EDD_RX_OVERLOAD) {
			acp_overload_check (channel_lower, 1, 0);
		}
		acp_ll_request (channel_lower
			, lower, EDD_SRV_RX_OVERLOAD_IND_PROVIDE, channel_lower
			);
	}
	else
	{
		ACP_LL_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled EDD_SRV_RX_OVERLOAD_IND_PROVIDE.rsp(%d)", rsp
			);
		ACP_FATAL (rsp);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_channel_open_done (
	ACP_CHANNEL_PTR  channel,
	LSA_UINT32  rsp
	)
{
	ACP_LOG_FCT("acp_channel_open_done")
    ACP_LOWER_RQB_PTR_TYPE  edd;
	LSA_UINT16  i;

	ACP_ASSERT (channel->state == ACP_CHANNEL_STATE_RESERVED);
	ACP_ASSERT (channel->lower->count_of_open_channel >= 0xFF00);
	ACP_ASSERT (is_not_null (channel->upper));
	ACP_ASSERT (ACP_RQB_GET_OPCODE (channel->upper) == ACP_OPC_OPEN_CHANNEL);
	ACP_ASSERT (channel->detailptr->channel_type == ACP_CHANNEL_TYPE_SYSTEM);

	if (rsp != ACP_OK) {
		ACP_LL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "channel.lower.nic_id(%d) cnt(0x%x) rsp(%d)"
			, channel->lower->nic_id
			, channel->lower->count_of_open_channel
			, rsp
			);

		channel->lower->count_of_open_channel = 0;
		channel->lower->nic_id = 0;
		channel->lower->handle_lower = 0;

			/* set result, ready with open, notify the user */
		channel->upper->args->channel.open.handle = ACP_INVALID_HANDLE;
		acp_callback_and_free_channel (channel, rsp, channel->upper);
		return;
	}
	else {
		ACP_LL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "channel.lower.nic_id(%d) cnt(0x%x) rsp(%d)"
			, channel->lower->nic_id
			, channel->lower->count_of_open_channel
			, rsp
			);
	}

		/* set result, ready with open */
	channel->lower->count_of_open_channel = 1;
	channel->upper->args->channel.open.handle = channel->my_handle;
	channel->state = ACP_CHANNEL_STATE_READY; /* channel is usable */


		/* providing RQBs for indications,
		//	for provide we get no answer, so a for loop works
		//-----------------------------------------------------------*/
	for (i = 0; i < channel->detailptr->type.system.max_edd_indication_res; ++i) {
		edd = acp_ll_alloc_rqb (channel->lower);
		if (is_null (edd)) {
			ACP_LL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "!edd-rqb, max_edd_indication_res(%d)"
				, channel->detailptr->type.system.max_edd_indication_res
				);
			acp_close_channel (channel->upper);
			return;
		}
		acp_ll_request (channel->lower, edd, EDD_SRV_SRT_IND_PROVIDE, channel->lower);
	}


		/* providing RQBs for nrt-recv */
	for (i = 0; i < channel->detailptr->type.system.max_edd_nrt_recv_res; ++i) {
		edd = acp_nrt_recv_alloc (channel->lower);
		if (is_null (edd)) {
			ACP_LL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "!edd-rqb, max_edd_nrt_recv_res(%d)"
				, channel->detailptr->type.system.max_edd_nrt_recv_res
				);
			acp_close_channel (channel->upper);
			return;
		}
		acp_nrt_recv_provide (channel->lower, edd);
	}

		/* providing RQBs for RTA-nrt-send */
	for (i = 0; i < channel->detailptr->type.system.max_edd_nrt_send_res; ++i) {
#ifdef ACP_MESSAGE
# pragma ACP_MESSAGE ("always EDD_FRAME_BUFFER_LENGTH ??? detailprt / ...")
#endif /* ACP_MESSAGE */
		edd = acp_nrt_send_alloc (channel->lower, EDD_FRAME_BUFFER_LENGTH);
		if (is_null (edd)) {
			ACP_LL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "!edd-rqb, max_edd_nrt_send_res(%d)"
				, channel->detailptr->type.system.max_edd_nrt_send_res
				);
			acp_close_channel (channel->upper);
			return;
		}
		acp_rta_sm_q_or_send_provide (channel, edd);
	}

	if (! acp_edd_overload_provide (channel->lower)) {
		ACP_LL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "!edd-rqb, edd_overload_provide"
			);
		acp_close_channel (channel->upper);
		return;
	}

		/* all done, notify the user */
	acp_callback (channel, ACP_OK);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID ACP_LOWER_IN_FCT_ATTR
acp_edd_state_indication (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_PTR  channel_lower,
    ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_edd_state_indication")
	LSA_UINT8  debug_data[ACP_ERROR_INFO_DATA_SIZE];
	LSA_UINT16  i;
	ACP_LL_SRT_IND_PROVIDE_PTR_TYPE  edd_ind;
	ACP_LOCAL_FRAME_PTR  frame;
	
	switch (ACP_LL_GET_RESPONSE(lower))
	{
	case EDD_STS_OK:
		edd_ind = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_IND_PROVIDE_PTR_TYPE);
		ACP_ASSERT (is_not_null (edd_ind));

		ACP_MEMCPY (debug_data, edd_ind->DebugInfo, ACP_ERROR_INFO_DATA_SIZE-4);

		ACP_LL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "ind_count(%d)", edd_ind->Count);
		
		for (i = 0; i < edd_ind->Count; ++i)
		{
			LSA_UINT32  edd_event = edd_ind->Data[i].Event;
			LSA_UINT32  edd_total = edd_ind->Data[i].EventStatus;

				/* EDDx internal only, shall never appear at the user-if */
			ACP_ASSERT (! (edd_event & EDD_CSRT_CONS_EVENT_NEGATION));

			frame = (ACP_LOCAL_FRAME_PTR)edd_ind->Data[i].UserID;

			if (edd_event  &  EDD_CSRT_PROV_EVENT_DROP)
			{
				acp_overload_check (channel_lower, 0, 1);
				continue; /* go to next edd-event */
			}
			if (is_null (frame))
			{
				ACP_LL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "event(%u) with no UserID"
					, edd_event
					);
				continue;
			}

			channel = frame->channel; ACP_ASSERT (is_not_null (channel));
			frame->cycle_counter = edd_ind->Data[i].CycleCnt;

#if 0/*new edd_total*/
//eddp v5.0i1 da geht das noch nicht, daher zurück zur bisherigen vorgehensweise
			frame->st_internal = edd_total;
#else

#ifdef ACP_MESSAGE
# pragma ACP_MESSAGE("warning: edd_total currently not implemented")
#endif /* ACP_MESSAGE */

				/* Consumer: dont simple assign, because of missing state_indication */
			ACP_STATE_TOGGLE (edd_event, frame->st_internal,
					EDD_CSRT_CONS_EVENT_STOP, EDD_CSRT_CONS_EVENT_RUN
				);
			ACP_STATE_TOGGLE (edd_event, frame->st_internal,
					EDD_CSRT_CONS_EVENT_STATION_FAILURE, EDD_CSRT_CONS_EVENT_STATION_OK
				);
			ACP_STATE_TOGGLE (edd_event, frame->st_internal,
					EDD_CSRT_CONS_EVENT_BACKUP, EDD_CSRT_CONS_EVENT_PRIMARY
				);
			ACP_STATE_TOGGLE (edd_event, frame->st_internal,
					0, EDD_CSRT_CONS_EVENT_ENTERING_RED /* note: "leaving red" removed with V5.3i1.2 (AP01448175) */
				);
			ACP_STATE_TOGGLE (edd_event, frame->st_internal,
					EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS, EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS
				);

				/* VV: TRACE: frame_id is missing, should keep frame->frame_id from add() */
			if (edd_event & EDD_CSRT_CONS_EVENT_MISS)
			{
				frame->st_internal |= EDD_CSRT_CONS_EVENT_MISS;
				frame->st_internal &= ~ (EDD_CSRT_CONS_EVENT_AGAIN | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED);

				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 0] = edd_ind->Data[i].DebugInfo[0];
				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 1] = edd_ind->Data[i].DebugInfo[1];
				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 2] = edd_ind->Data[i].DebugInfo[2];
				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 3] = edd_ind->Data[i].DebugInfo[3];

/* trace is done in acp_check_frame_indication()
				ACP_LL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "MISS: idx(%d) acp_id(%u)  event(0x%x)  st_intern(0x%x)  st_user(0x%x)"
					, i, frame->acp_id, edd_event, frame->st_internal, frame->st_signaled
					);
*/
			}
			else if (edd_event & EDD_CSRT_CONS_EVENT_AGAIN)
			{
				frame->st_internal |= EDD_CSRT_CONS_EVENT_AGAIN;
				frame->st_internal &= ~ (EDD_CSRT_CONS_EVENT_MISS | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED);

/* trace is done in acp_check_frame_indication()
				ACP_LL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "AGAIN: idx(%d) acp_id(%u)  event(0x%x)  st_intern(0x%x)  st_user(0x%x)"
					, i, frame->acp_id, edd_event, frame->st_internal, frame->st_signaled
					);
*/
			}
			else if (edd_event & EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED)
			{
				frame->st_internal |= EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
				frame->st_internal &= ~ (EDD_CSRT_CONS_EVENT_AGAIN | EDD_CSRT_CONS_EVENT_MISS);

				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 0] = edd_ind->Data[i].DebugInfo[0];
				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 1] = edd_ind->Data[i].DebugInfo[1];
				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 2] = edd_ind->Data[i].DebugInfo[2];
				debug_data[ACP_ERROR_INFO_DATA_SIZE-4 + 3] = edd_ind->Data[i].DebugInfo[3];

/* trace is done in acp_check_frame_indication()
				ACP_LL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "MISS-NOT-STOPPED: idx(%d) acp_id(%u)  event(0x%x)  st_intern(0x%x)  st_user(0x%x)"
					, i, frame->acp_id, edd_event, frame->st_internal, frame->st_signaled
					);
*/
			}
			else
			{
/* trace is done in acp_check_frame_indication()
				ACP_LL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "APDU: idx(%d) acp_id(%u)  event(0x%x)  st_intern(0x%x)  st_user(0x%x)"
					, i, frame->acp_id, edd_event, frame->st_internal, frame->st_signaled
					);
*/
			}

			LSA_UNUSED_ARG(edd_total);
			if (frame->st_internal != edd_total) { /* why? time for an ArtsRQ */ /* see AP01221847(Vereinheitlichen der ConsumerIndikation .EventStatus) */
				LSA_UINT32  acp_delta_event = ((~ frame->st_signaled) & (frame->st_internal));
				LSA_UNUSED_ARG(acp_delta_event);
			}
#endif

				/* notify user */
			acp_check_frame_indication (channel, frame, debug_data);
		}

			/* if lower is not closing: provide the rqb again */
		if (channel_lower->count_of_open_channel != 0xFFFF) {
			acp_ll_request (channel_lower, lower, EDD_SRV_SRT_IND_PROVIDE, channel_lower);
		}
			/* else free */
		else {
			acp_ll_free_rqb (channel_lower, lower);
		}
		break;

	case EDD_STS_OK_CANCEL:
		acp_ll_free_rqb (channel_lower, lower);
		break;

	default:
		ACP_LL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "!!undefined response(%d)"
			, ACP_LL_GET_RESPONSE(lower)
			);
		ACP_FATAL (ACP_LL_GET_RESPONSE(lower));
		break;
	}
}


/*------------------------------------------------------------------------------
//	entry-function for the lower layer.
//		the channel is acquire via the field user_id, and not via the lsa-handle
//		this is nesessary, because more than one upper layer can use the same
//		lower layer (system-channel - behaviour).
//----------------------------------------------------------------------------*/
LSA_VOID ACP_LOWER_IN_FCT_ATTR
acp_request_lower_done (
    ACP_LOWER_RQB_PTR_TYPE  lower		/* in.: the response */
	)
{
	ACP_LOG_FCT("acp_request_lower_done")
	ACP_CHANNEL_PTR  channel;
	LSA_UINT32  rsp;
	LSA_VOID_PTR_TYPE  user_id;
	ACP_LOWER_PTR  channel_lower;
	ACP_LOCAL_FRAME_PTR  frame;

	ACP_LL_OPEN_CHANNEL_PTR_TYPE  edd_open;

	if (is_null (lower)) {
		ACP_FATAL (0);
		return;
	}

	channel = acp_channel_from_handle (ACP_LL_GET_HANDLE (lower));
	if (is_null (channel)) {
		ACP_FATAL (ACP_LL_GET_HANDLE (lower));
		return;
	}

		/* keeps lower state for callback */
	rsp = acp_rsp_from_edd_rsp (ACP_LL_GET_RESPONSE (lower));

	if (rsp == ACP_OK || rsp == ACP_OK_CANCEL) {
		ACP_LL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW,
				"<<< lower(%08x) h(%u) opc(%d) service(0x%x) rsp(0x%x)",
				lower, ACP_LL_GET_HANDLE (lower),
				ACP_LL_GET_OPCODE (lower), ACP_LL_GET_SERVICE (lower),
				ACP_LL_GET_RESPONSE (lower)
			);
	}
	else {
		ACP_LL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP,
				"<<< lower(%08x) h(%u) opc(%d) service(0x%x) rsp(0x%x)",
				lower, ACP_LL_GET_HANDLE (lower),
				ACP_LL_GET_OPCODE (lower), ACP_LL_GET_SERVICE (lower),
				ACP_LL_GET_RESPONSE (lower)
			);
	}

		/* check for all opcodes / services */
	user_id = ACP_LL_GET_USERID_PTR (lower);
	if (is_null (user_id)) {
		ACP_LL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "user_id of RQB(0x%08x) is null", lower);
		ACP_FATAL (user_id);
		return; /* never */
	}

	if (ACP_LL_GET_OPCODE (lower) == EDD_OPC_REQUEST) {

		LSA_VOID_PTR_TYPE param = ACP_LL_GET_PPARAM (lower, LSA_VOID_PTR_TYPE);
		if (is_null (param)) {
			ACP_FATAL ((LSA_UINT32)lower);
			return;
		}

	switch (ACP_LL_GET_SERVICE(lower)) {
/*----------------------------------------------------------------------------*/
	case EDD_SRV_GET_PARAMS:
		channel = (ACP_CHANNEL_PTR) user_id;

		acp_op_channel_info_done (channel, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_CONSUMER_ADD:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_consumer_add_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_PROVIDER_ADD:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_provider_add_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_CONSUMER_REMOVE:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_consumer_remove_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_PROVIDER_REMOVE:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_provider_remove_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_PROVIDER_CONTROL:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_provider_control_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_CONSUMER_CONTROL:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_consumer_control_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_PROVIDER_XCHANGE_BUF:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_provider_xbuf_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_CONSUMER_XCHANGE_BUF:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_consumer_xbuf_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_PROVIDER_SET_STATE:
		channel = (ACP_CHANNEL_PTR) user_id;

		acp_op_global_provider_control_done (channel, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_PROVIDER_CHANGE_PHASE:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_provider_change_phase_done (channel, frame, lower);
		break;

/*-----------------------------------------------------------------------------
// EDD Indication
//---------------------------------------------------------------------------*/

	case EDD_SRV_SRT_IND_PROVIDE:
		channel_lower = (ACP_LOWER_PTR) user_id;

		acp_edd_state_indication (channel, channel_lower, lower);
		break;

/*------------------------------------------------------------------------------
// ALARM / RTA
//----------------------------------------------------------------------------*/

	case EDD_SRV_NRT_SEND:
		{
		acp_nrt_send_done (lower, user_id);
		}
		break;

	case EDD_SRV_NRT_RECV:
		{
		channel_lower = (ACP_LOWER_PTR) user_id;

		if (channel_lower->count_of_open_channel == 0xFFFF		/* channel closing */
		||	ACP_LL_GET_RESPONSE (lower) == EDD_STS_OK_CANCEL
		) {
			acp_nrt_recv_free (channel_lower, & lower);
		}
		else {
			acp_nrt_recv_indication (channel_lower, lower);
		}
		}
		break;

	case EDD_SRV_NRT_CANCEL:
		{
		ACP_LL_NRT_CANCEL_PTR_TYPE  nrt_cancel;
		LSA_UINT16  rc;

		channel = (ACP_CHANNEL_PTR) user_id;

		ACP_ASSERT (channel->lower->count_of_open_channel == 0xFFFF);
		rsp = rsp; /* ignore */

		nrt_cancel = ACP_LL_GET_PPARAM (lower, ACP_LL_NRT_CANCEL_PTR_TYPE);
		ACP_ASSERT (is_not_null (nrt_cancel));
		ACP_FREE_LOWER_MEM (& rc, nrt_cancel, channel->sysptr);
		ACP_ASSERT (rc == LSA_RET_OK);
		ACP_LL_SET_PPARAM (lower, LSA_NULL);

		ACP_LL_SET_OPCODE (lower, EDD_OPC_CLOSE_CHANNEL);
		ACP_LL_SET_SERVICE (lower, 0);
		ACP_LL_SET_USERID_PTR (lower, channel);
		ACP_LL_SET_HANDLE (lower, channel->lower->handle_lower);

			/* real close is done in callback */
		ACP_CLOSE_CHANNEL_LOWER (lower, channel->lower->sysptr);
		}
		break;

/*------------------------------------------------------------------------------
// div
//----------------------------------------------------------------------------*/

	case EDD_SRV_LINK_STATUS_IND_PROVIDE:
		channel_lower = (ACP_LOWER_PTR) user_id;
		acp_op_link_status_provide_done (channel_lower, lower);
		break;

/*----------------------------------------------------------------------------*/

	case EDD_SRV_RX_OVERLOAD_IND_PROVIDE:
		channel_lower = (ACP_LOWER_PTR) user_id;
		acp_edd_overload_provide_done (channel_lower, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_SRT_GET_APDU_STATUS:
		frame = (ACP_LOCAL_FRAME_PTR) user_id;
		channel = frame->channel; ACP_ASSERT (is_not_null (channel));

		acp_op_get_apdu_status_done (channel, frame, lower);
		break;

/*----------------------------------------------------------------------------*/
	case EDD_SRV_MULTICAST:
		channel = (ACP_CHANNEL_PTR) user_id;

		acp_op_multicast_done (channel, lower);
		break;

/*----------------------------------------------------------------------------*/
	default:
		{
		ACP_LL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled service(%d) rsp(%d)",
				ACP_LL_GET_SERVICE (lower), ACP_LL_GET_RESPONSE (lower)
			);
		ACP_FATAL (ACP_LL_GET_SERVICE (lower));
		}
		break;
	}
	}
/*----------------------------------------------------------------------------*/
	else if (ACP_LL_GET_OPCODE (lower) == EDD_OPC_OPEN_CHANNEL) {
		channel = (ACP_CHANNEL_PTR) user_id;
		edd_open = ACP_LL_GET_PPARAM (lower, ACP_LL_OPEN_CHANNEL_PTR_TYPE);
		if (is_null (edd_open)) {
			ACP_FATAL ((LSA_UINT32)lower);
			return;
		}

		ACP_ASSERT (channel->lower->count_of_open_channel == 0xFF00);

			/* set result */
		channel->lower->handle_lower = edd_open->Handle;
		acp_ll_free_rqb (channel->lower, lower);

			/* confirm to upper */
		acp_channel_open_done (channel, rsp);
		return;
	}
/*----------------------------------------------------------------------------*/
	else if (ACP_LL_GET_OPCODE (lower) == EDD_OPC_CLOSE_CHANNEL) {
		channel = (ACP_CHANNEL_PTR) user_id;

		ACP_ASSERT (channel->lower->count_of_open_channel == 0xFFFF);

		if (rsp == ACP_OK) {
			channel->lower->nic_id = 0;
			channel->lower->count_of_open_channel = 0;
			channel->lower->handle_lower = 0;

			acp_ll_free_rqb (channel->lower, lower);

				/* free all provided resourcen */
			acp_free_resource (channel);

			if (ACP_RQB_GET_OPCODE (channel->upper) == ACP_OPC_OPEN_CHANNEL) {
				ACP_LL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "no resource on open_channel"
					);
				rsp = ACP_ERR_RESOURCE;
			}

				/* fin */
			acp_callback_and_free_channel (channel, rsp, channel->upper);
		}
		else {
			channel->lower->count_of_open_channel = 1; /* see acp_close_channel */
			acp_ll_free_rqb (channel->lower, lower);
			acp_callback (channel, rsp); /* report bad result */
		}
		return;
	}
/*----------------------------------------------------------------------------*/
	else {
		ACP_LL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled opcode(%d) service(%d) rsp(%d)",
				ACP_LL_GET_OPCODE (lower), ACP_LL_GET_SERVICE (lower), ACP_LL_GET_RESPONSE (lower)
			);
		ACP_FATAL (ACP_LL_GET_OPCODE (lower));
		return;
	}

	return;
}


#if 0 /* LSA-function, not used */
/*-----------------------------------------------------------------------------
// LSA-function, not used
//---------------------------------------------------------------------------*/
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_lower_rqb(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr)
{
	user_id = user_id;
	length  = length;
	lower_rqb_ptr = lower_rqb_ptr;
	return;
}
/*-----------------------------------------------------------------------------
// LSA-function, not used
//---------------------------------------------------------------------------*/
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_lower_mem(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_MEM_PTR_TYPE     lower_mem_ptr)
{
	user_id = user_id;
	length  = length;
	lower_mem_ptr = lower_mem_ptr;
	return;
}
#endif


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
