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
/*  F i l e               &F: acp_opc.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the various OPCODEs                                           */
/*                                                                           */
/*****************************************************************************/
#define ACP_MODULE_ID      5
#define LTRC_ACT_MODUL_ID  5

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)


/* APDU-status: check if one of the available set-bits is given */
#define ACP_PROVIDER_APDU_STATUS \
	( ACP_PROVIDER_SET_STATION_FAILURE	| ACP_PROVIDER_SET_STATION_OK )


/*------------------------------------------------------------------------------
//	add one rqb at the end of the list
//		return 1 on first element, 0 else
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_add_to_rqb_list (
	ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_UPPER_RQB_PTR_TYPE  rqb
	)
{
	ACP_LOG_FCT("acp_add_to_rqb_list")
	ACP_UPPER_RQB_PTR_TYPE first, last;

	ACP_ASSERT (is_not_null (rqb_list));
	ACP_ASSERT (is_not_null (rqb));

	ACP_ENTER ();

	ACP_RQB_SET_NEXT_RQB_PTR (rqb, LSA_NULL);
	ACP_RQB_SET_PREV_RQB_PTR (rqb, LSA_NULL);

	if (is_null (*rqb_list)) {
		ACP_RQB_SET_PREV_RQB_PTR (rqb, rqb);
		*rqb_list = rqb;

		ACP_EXIT ();
		return 1;
	}

	first = *rqb_list;
	last = (ACP_UPPER_RQB_PTR_TYPE)ACP_RQB_GET_PREV_RQB_PTR(first);
	ACP_ASSERT (is_not_null (last));

	ACP_RQB_SET_PREV_RQB_PTR (first, rqb);
	ACP_RQB_SET_NEXT_RQB_PTR (last, rqb);

	ACP_EXIT ();
	return 0;
}


/*------------------------------------------------------------------------------
//	lower variant, see check in acp_init()
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_add_to_lower_rqb_list (
	ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_LOWER_RQB_PTR_TYPE  rqb
	)
{
	return acp_add_to_rqb_list (
			(ACP_UPPER_RQB_PTR_TYPE *) rqb_list,
			(ACP_UPPER_RQB_PTR_TYPE) rqb
		);
}


/*------------------------------------------------------------------------------
//	get one rqb from the beginning of the list
//		return 0 if there is no (more) rqb, else 1
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_get_from_rqb_list (
	ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb
	)
{
	ACP_LOG_FCT("acp_get_from_rqb_list")

	ACP_ASSERT (is_not_null (rqb_list));
	ACP_ASSERT (is_not_null (rqb));

	ACP_ENTER ();

	*rqb = LSA_NULL;
	if (is_null (*rqb_list)) {
		ACP_EXIT ();
		return 0;
	}

	*rqb = (*rqb_list);
	*rqb_list = (ACP_UPPER_RQB_PTR_TYPE) ACP_RQB_GET_NEXT_RQB_PTR (*rqb_list);

	if (is_not_null (*rqb_list)) {
		ACP_ASSERT (is_not_null (ACP_RQB_GET_PREV_RQB_PTR (*rqb)));
		ACP_RQB_SET_PREV_RQB_PTR (*rqb_list, ACP_RQB_GET_PREV_RQB_PTR (*rqb));
	}

	ACP_EXIT ();
	return 1;
}


/*------------------------------------------------------------------------------
//	lower variant, see check in acp_init()
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_get_from_lower_rqb_list (
	ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb
	)
{
	/* see check in acp_init() */
	return acp_get_from_rqb_list (
			(ACP_UPPER_RQB_PTR_TYPE *) rqb_list,
			(ACP_UPPER_RQB_PTR_TYPE *) rqb
		);
}


/*------------------------------------------------------------------------------
//	free resource of one channel
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_free_resource (ACP_CHANNEL_PTR  channel)
{
	ACP_LOG_FCT("acp_free_resource")
	ACP_UPPER_RQB_PTR_TYPE  upper;

	while (acp_get_from_rqb_list (& channel->provide, & upper)) {
		ACP_ASSERT (is_not_null (upper));
		acp_callback_async (channel, ACP_OK_CANCEL, & upper);
	}

	while (! AcpListIsEmpty (& channel->link_status_list)) {
		AcpListRemoveHead (& channel->link_status_list, upper, ACP_UPPER_RQB_PTR_TYPE);
		ACP_ASSERT (is_not_null (upper));
		acp_callback_async (channel, ACP_OK_CANCEL, & upper);
	}

	if (is_not_null (channel->overload_ind)) {
		acp_callback_async (channel, ACP_OK_CANCEL, & channel->overload_ind);
		ACP_ASSERT (is_null (channel->overload_ind));
	}

	while (acp_get_from_rqb_list (& channel->alarm_provide[ACP_ALARM_PRIORITY_LOW], & upper)) {
		ACP_ASSERT (is_not_null (upper));
		acp_callback_async (channel, ACP_OK_CANCEL, & upper);
	}

	while (acp_get_from_rqb_list (& channel->alarm_provide[ACP_ALARM_PRIORITY_HIGH], & upper)) {
		ACP_ASSERT (is_not_null (upper));
		acp_callback_async (channel, ACP_OK_CANCEL, & upper);
	}

	if (channel->detailptr->channel_type == ACP_CHANNEL_TYPE_SYSTEM) {
		ACP_LOWER_RQB_PTR_TYPE	nrt;

		while (! AcpListIsEmpty (& channel->lower->q_or_send.nrt_send_list)) {
			AcpListRemoveHead (& channel->lower->q_or_send.nrt_send_list, nrt, ACP_LOWER_RQB_PTR_TYPE);
			ACP_ASSERT (is_not_null (nrt));
			acp_nrt_send_free (channel->lower, & nrt);
		}
	}

	return;
}


/*------------------------------------------------------------------------------
//	set to default / passive
//----------------------------------------------------------------------------*/
static LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_frame_set_passive (ACP_LOCAL_FRAME_PTR  frame)
{
	if (acp_is_consumer(frame))
	{
		if (acp_is_sysred(frame))
		{
			frame->st_signaled = ACP_STATE_CONSUMER_SYSRED_DEFAULT;
			frame->st_internal = ACP_STATE_CONSUMER_SYSRED_DEFAULT;
		}
		else
		{
			frame->st_signaled = ACP_STATE_CONSUMER_DEFAULT;
			frame->st_internal = ACP_STATE_CONSUMER_DEFAULT;
		}
	}
	else
	{
		frame->st_signaled = ACP_STATE_PROVIDER_DEFAULT;
		frame->st_internal = ACP_STATE_PROVIDER_DEFAULT;
	}
}


/*------------------------------------------------------------------------------
//	search an empty frame
//----------------------------------------------------------------------------*/
ACP_LOCAL_FRAME_PTR  ACP_LOCAL_FCT_ATTR
acp_frame_new (ACP_CHANNEL_PTR  channel, LSA_UINT16 frametyp)
{
	ACP_LOG_FCT("acp_frame_new")
	ACP_LOCAL_FRAME_PTR  frame;
	LSA_UINT16 i;

	for (i = 1; i < channel->max_frames; ++i) {
		if (channel->frames[i].state == ACP_FS_NO) {	/* .state needs for StateMachine */
			frame			= & channel->frames[i];
			frame->acp_id	= i;		/* important: set current acp_id !! */
			frame->channel	= channel;
			frame->edd_id	= 0xFFFE;	/* see EDD_CONS_ID_INVALID, EDD_PROV_ID_INVALID*/
			frame->usr_id	= 0;
			frame->options	= frametyp;

			acp_frame_set_passive (frame);
			/* check: frame->state = ACP_FS_RESERVED */
			ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "acp_id(%d) type(%d)"
				, frame->acp_id, frametyp
				);
			return frame;
		}
	}
	ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "!frame, max(%d)"
		, channel->max_frames
		);
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	returns the frame-pointer with the ID acp_id and an open state  or LSA_NULL.
//----------------------------------------------------------------------------*/
ACP_LOCAL_FRAME_PTR  ACP_LOCAL_FCT_ATTR
acp_frame_find (ACP_CHANNEL_PTR  channel, LSA_UINT16 acp_id)
{
	ACP_LOG_FCT("acp_frame_find")
	if (1 <= acp_id && acp_id < channel->max_frames) {
		if (channel->frames[acp_id].state == ACP_FS_NO) {
			return LSA_NULL;
		}
		return & channel->frames[acp_id];
	}
	ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "not found acp_id(%d) max(%d)"
		, acp_id
		, channel->max_frames
		);
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	returns the count of frames added to the channel
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_frame_count (ACP_CHANNEL_PTR  channel)
{
	LSA_UINT16 cnt = 0;
	LSA_UINT16 i;
	for (i = 1; i < channel->max_frames; ++i) {
		if (channel->frames[i].state != ACP_FS_NO) {
			++cnt;
		}
	}
	return cnt;
}


/*------------------------------------------------------------------------------
//	add a srt-frame in lower layer (EDD)
//----------------------------------------------------------------------------*/

LSA_INT16  ACP_LOCAL_FCT_ATTR
acp_edd_frame_add (ACP_CHANNEL_PTR  channel, ACP_LOCAL_FRAME_PTR  frame)
{
	ACP_LOG_FCT("acp_edd_frame_add")
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_LOWER_RQB_PTR_TYPE  lower;

	upper = channel->upper;
	ACP_ASSERT (is_not_null (upper));
	ACP_ASSERT (is_not_null (upper->args));

	if (channel->max_group_id == 0) {
		ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "ch.max_group_id == 0"
			);
		return ACP_ERR_SEQUENCE;
	}

	if (! (channel->detailptr->type.user.group_id < channel->max_group_id)) {
		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "! ch.group_id(%d) < ch.max_group_id(%d)"
			, channel->detailptr->type.user.group_id
			, channel->max_group_id
			);
		return ACP_ERR_RESOURCE;
	}

	if (acp_is_provider (frame))
	{
		ACP_UPPER_PROVIDER_ADD_PTR_TYPE  padd = & upper->args->prov.add;
		LSA_UINT16  rtclass = padd->properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK;

		switch (rtclass) 
		{
		case EDD_CSRT_PROV_PROP_RTCLASS_1:
		case EDD_CSRT_PROV_PROP_RTCLASS_2:
		case EDD_CSRT_PROV_PROP_RTCLASS_3:
		case EDD_CSRT_PROV_PROP_RTCLASS_UDP:
			ACP_UPPER_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "ProviderAdd/RTClass%u: acp_id(%u) frame_id(%#x) length(%u) properties(%#x) rr(%u) ph(%u) sequence(%u)"
				, rtclass, frame->acp_id, padd->frame_id, padd->length
				, padd->properties, padd->reduction_ratio, padd->phase, padd->sequence
				);
			break;

		default:
			return ACP_ERR_PARAM;
		}
	}
	else
	{
		ACP_UPPER_CONSUMER_ADD_PTR_TYPE  cadd = & upper->args->cons.add;
		LSA_UINT16  rtclass = cadd->properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK;

		switch (rtclass) 
		{
		case EDD_CSRT_CONS_PROP_RTCLASS_1:
		case EDD_CSRT_CONS_PROP_RTCLASS_2:
		case EDD_CSRT_CONS_PROP_RTCLASS_3:
		case EDD_CSRT_CONS_PROP_RTCLASS_UDP:
			ACP_UPPER_TRACE_09 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "ConsumerAdd/RTClass%u: acp_id(%u) frame_id(%#x) length(%u) partial_length(%u) properties(%#x) rr(%u) ph(%u) data_hold_factor(%u)"
				, rtclass, frame->acp_id, cadd->frame_id, cadd->length, cadd->partial_length
				, cadd->properties, cadd->reduction_ratio, cadd->phase, cadd->data_hold_factor
				);
			break;

		default:
			return ACP_ERR_PARAM;
		}
	}

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		return ACP_ERR_ALLOC_LOWER_RQB;
	}

	if (acp_is_provider (frame))
	{
		ACP_UPPER_PROVIDER_ADD_PTR_TYPE  padd = & upper->args->prov.add;
		ACP_LL_SRT_PROVIDER_ADD_PTR_TYPE  edd_padd;

		edd_padd = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_ADD_PTR_TYPE);
		ACP_ASSERT (is_not_null (edd_padd));

		edd_padd->FrameID					= padd->frame_id;
		edd_padd->Properties				= padd->properties;
		edd_padd->DataLen					= padd->length;
		edd_padd->CycleReductionRatio		= padd->reduction_ratio;
		edd_padd->CyclePhase				= padd->phase;
		edd_padd->CyclePhaseSequence		= padd->sequence;
		edd_padd->UserID					= (LSA_UINT32)frame;
		edd_padd->GroupID					= channel->detailptr->type.user.group_id;
		edd_padd->ProviderID				= EDD_PROV_ID_INVALID; /* out */

			/* EDDx specific */
		edd_padd->IOParams.BufferProperties	= padd->ioparam.properties;
		edd_padd->IOParams.CRNumber			= padd->ioparam.cr_number;
		edd_padd->IOParams.AppGroup			= padd->ioparam.app_group;
		edd_padd->IOParams.SFPosition		= padd->ioparam.sf_position;
		edd_padd->IOParams.CWOffset			= EDD_DATAOFFSET_INVALID; /* out */
		edd_padd->IOParams.SFOffset			= padd->ioparam.sf_offset;
		edd_padd->IOParams.DataStatusOffset	= EDD_DATAOFFSET_INVALID; /* out */
		edd_padd->IOParams.DataOffset		= padd->ioparam.data_offset;

			/* need some local things, set positive result */
		frame->usr_id	= padd->usr_id;
		padd->prov_id	= frame->acp_id;

			/* EDDx */
		acp_ll_request (channel->lower, lower, EDD_SRV_SRT_PROVIDER_ADD, frame);
	}
	else
	{
		ACP_UPPER_CONSUMER_ADD_PTR_TYPE  cadd = & upper->args->cons.add;
		ACP_LL_SRT_CONSUMER_ADD_PTR_TYPE  edd_cadd;

		edd_cadd = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_CONSUMER_ADD_PTR_TYPE);
		ACP_ASSERT (is_not_null (edd_cadd));

		edd_cadd->FrameID					= cadd->frame_id;
		edd_cadd->Properties				= cadd->properties;
		edd_cadd->DataLen					= cadd->length;
		edd_cadd->Partial_DataLen			= cadd->partial_length;
		edd_cadd->Partial_DataOffset		= cadd->partial_offset;
		edd_cadd->CycleReductionRatio		= cadd->reduction_ratio;
		edd_cadd->CyclePhase				= cadd->phase;
		edd_cadd->DataHoldFactor			= cadd->data_hold_factor;
		edd_cadd->UserID					= (LSA_UINT32)frame;
		edd_cadd->ConsumerID				= EDD_CONS_ID_INVALID; /* out */

			/* EDDx specific */
		edd_cadd->IOParams.BufferProperties		= cadd->ioparam.properties;
		edd_cadd->IOParams.CRNumber				= cadd->ioparam.cr_number;
		edd_cadd->IOParams.AppGroup				= cadd->ioparam.app_group;
		edd_cadd->IOParams.SFPosition			= cadd->ioparam.sf_position;
		edd_cadd->IOParams.ForwardMode			= cadd->ioparam.forward_mode;
		edd_cadd->IOParams.CutThrough_OutDataStart
												= cadd->ioparam.ct_outdatastart;
		edd_cadd->IOParams.SFOffset				= cadd->ioparam.sf_offset;
		edd_cadd->IOParams.DataOffset			= cadd->ioparam.data_offset;
		edd_cadd->IOParams.APDUStatusOffset		= EDD_DATAOFFSET_INVALID; /* out */
		edd_cadd->IOParams.ClearOnMISS			= cadd->ioparam.clear_on_miss;

			/* need some local things, set positive result */
		frame->usr_id	= cadd->usr_id;
		cadd->cons_id	= frame->acp_id;

			/* EDDx */
		acp_ll_request (channel->lower, lower, EDD_SRV_SRT_CONSUMER_ADD, frame);
	}

	return ACP_OK;
}


/*------------------------------------------------------------------------------
//	removes a frame in lower layer (EDD)
//----------------------------------------------------------------------------*/
LSA_INT16  ACP_LOCAL_FCT_ATTR
acp_edd_frame_remove (ACP_CHANNEL_PTR  channel, ACP_LOCAL_FRAME_PTR  frame)
{
	ACP_LOG_FCT("acp_edd_frame_remove")
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_SRT_PROVIDER_REMOVE_PTR_TYPE  edd_prm;
	ACP_LL_SRT_CONSUMER_REMOVE_PTR_TYPE  edd_crm;

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		return ACP_ERR_ALLOC_LOWER_RQB;
	}
	if (acp_is_provider (frame)) {
		edd_prm = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_REMOVE_PTR_TYPE);

		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "ProviderRemove: acp_id(%u) edd_id(%u)"
			, frame->acp_id, frame->edd_id
			);
		edd_prm->ProviderID = frame->edd_id;
		acp_ll_request (channel->lower, lower, EDD_SRV_SRT_PROVIDER_REMOVE, frame);
	}
	else {
		edd_crm = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_CONSUMER_REMOVE_PTR_TYPE);

		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "ConsumerRemove: acp_id(%u) edd_id(%u)"
			, frame->acp_id, frame->edd_id
			);
		edd_crm->ConsumerID = frame->edd_id;
		acp_ll_request (channel->lower, lower, EDD_SRV_SRT_CONSUMER_REMOVE, frame);
	}

	return ACP_OK;
}


/*------------------------------------------------------------------------------
//	controls a frame in lower layer
//----------------------------------------------------------------------------*/
LSA_INT16  ACP_LOCAL_FCT_ATTR
acp_edd_frame_control (ACP_CHANNEL_PTR  channel, ACP_LOCAL_FRAME_PTR  frame)
{
	ACP_LOG_FCT("acp_edd_frame_control")
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_LOWER_RQB_PTR_TYPE  lower;

	upper = channel->upper;
	ACP_ASSERT (is_not_null (upper));
	ACP_ASSERT (is_not_null (upper->args));

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		return ACP_ERR_ALLOC_LOWER_RQB;
	}

	if (acp_is_provider (frame))
	{
		ACP_UPPER_PROVIDER_CONTROL_PTR_TYPE  acp_pcntrl;
		ACP_LL_SRT_PROVIDER_CONTROL_PTR_TYPE  edd_pcntrl;

		acp_pcntrl = & upper->args->prov.control;
		edd_pcntrl = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_CONTROL_PTR_TYPE);

		ACP_UPPER_TRACE_10 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "ProviderControl: acp_id(%u)  mode(%#x) autostop_cons_id(%u) data_offset(0x%x) mac(%02x:%02x:%02x:%02x:%02x:%02x)"
			, frame->acp_id, acp_pcntrl->mode
			, acp_pcntrl->autostop_cons_id, acp_pcntrl->ioparam.data_offset
			, acp_pcntrl->dst_mac[0], acp_pcntrl->dst_mac[1], acp_pcntrl->dst_mac[2]
			, acp_pcntrl->dst_mac[3], acp_pcntrl->dst_mac[4], acp_pcntrl->dst_mac[5]
			);

		edd_pcntrl->ProviderID	= frame->edd_id;
		edd_pcntrl->Mode		= 0;

		if (acp_pcntrl->mode & ACP_PROVIDER_SET_ACTIVE) {
			edd_pcntrl->Mode	|= EDD_PROV_CONTROL_MODE_ACTIVATE;
		}
		else if (acp_pcntrl->mode & ACP_PROVIDER_SET_PASSIVE) {
			edd_pcntrl->Mode	|= EDD_PROV_CONTROL_MODE_PASSIVATE;

			if (acp_pcntrl->mode & ACP_PROVIDER_RESET_TO_FACTORY)
			{
				edd_pcntrl->Mode	|= EDD_PROV_CONTROL_MODE_REMOVE_ACW;
			}
		}

		if (acp_pcntrl->mode & ACP_PROVIDER_SET_IOPARAM)
		{
			edd_pcntrl->Mode	|= EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS;
		}

		if (acp_pcntrl->mode & ACP_PROVIDER_SET_ACTIVE_AUX) {
			edd_pcntrl->Mode	|= EDD_PROV_CONTROL_MODE_ACTIVATE_AUX;
		}
		else if (acp_pcntrl->mode & ACP_PROVIDER_SET_PASSIVE_AUX) {
			edd_pcntrl->Mode	|= EDD_PROV_CONTROL_MODE_PASSIVATE_AUX;
		}

		if (acp_pcntrl->mode & ACP_PROVIDER_SET_AUTOSTOP)
		{
			edd_pcntrl->Mode	|= EDD_PROV_CONTROL_MODE_AUTOSTOP;

			if (acp_pcntrl->autostop_cons_id == ACP_INVALID_ACP_ID)
			{
				edd_pcntrl->AutoStopConsumerID = EDD_CONSUMER_ID_REMOVE_AS;
			}
			else
			{
				ACP_LOCAL_FRAME_PTR  autostop_cons = acp_frame_find (channel, acp_pcntrl->autostop_cons_id);
				if (is_null (autostop_cons))
				{
					ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "ProviderControl: acp_id(%u)  autostop_cons_id(%u) not found"
						, frame->acp_id, acp_pcntrl->autostop_cons_id
						);
					acp_ll_free_rqb (channel->lower, lower);
					return ACP_ERR_PARAM;
				}
				edd_pcntrl->AutoStopConsumerID = autostop_cons->edd_id;
			}
		}
		else
		{
			edd_pcntrl->AutoStopConsumerID = EDD_CONS_ID_INVALID;
		}

		edd_pcntrl->FrameID			= acp_pcntrl->frame_id;

		ACP_MEMCPY (edd_pcntrl->DstMAC.MacAdr, acp_pcntrl->dst_mac, EDD_MAC_ADDR_SIZE);
		ACP_MEMCPY (edd_pcntrl->DstIP.IPAdr, acp_pcntrl->dst_ip, EDD_IP_ADDR_SIZE);
		
		edd_pcntrl->Status			= 0;
		edd_pcntrl->Mask			= 0;

		if (acp_pcntrl->mode & ACP_PROVIDER_APDU_STATUS)
		{
			edd_pcntrl->Mode		|= EDD_PROV_CONTROL_MODE_STATUS;

			if (acp_pcntrl->mode & ACP_PROVIDER_SET_STATION_FAILURE) {
				edd_pcntrl->Mask	|= EDD_CSRT_DSTAT_BIT_STATION_FAILURE;
			}
			if (acp_pcntrl->mode & ACP_PROVIDER_SET_STATION_OK) {
				edd_pcntrl->Mask	|= EDD_CSRT_DSTAT_BIT_STATION_FAILURE;
				edd_pcntrl->Status	|= EDD_CSRT_DSTAT_BIT_STATION_FAILURE;
			}
		}

		edd_pcntrl->CycleReductionRatio			= acp_pcntrl->reduction_ratio;
		edd_pcntrl->CyclePhase					= acp_pcntrl->phase;
		edd_pcntrl->CyclePhaseSequence			= acp_pcntrl->sequence;

		edd_pcntrl->IOParams.BufferProperties	= acp_pcntrl->ioparam.properties;
		edd_pcntrl->IOParams.CRNumber			= acp_pcntrl->ioparam.cr_number;
		edd_pcntrl->IOParams.AppGroup			= acp_pcntrl->ioparam.app_group;
		edd_pcntrl->IOParams.SFPosition			= acp_pcntrl->ioparam.sf_position;
		edd_pcntrl->IOParams.SFOffset			= acp_pcntrl->ioparam.sf_offset;
		edd_pcntrl->IOParams.DataOffset			= acp_pcntrl->ioparam.data_offset;

		acp_ll_request (channel->lower, lower, EDD_SRV_SRT_PROVIDER_CONTROL, frame);
	}
	else
	{
		ACP_UPPER_CONSUMER_CONTROL_PTR_TYPE  acp_ccntrl;
		ACP_LL_SRT_CONSUMER_CONTROL_PTR_TYPE  edd_ccntrl;

		acp_ccntrl				= & upper->args->cons.control;
		edd_ccntrl				= ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_CONSUMER_CONTROL_PTR_TYPE);

		ACP_UPPER_TRACE_09 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "ConsumerControl: acp_id(%u) mode(%#x) src_mac(%02x-%02x-%02x-%02x-%02x-%02x) data_offset(0x%x)"
			, frame->acp_id, acp_ccntrl->mode
			, acp_ccntrl->src_mac[0], acp_ccntrl->src_mac[1], acp_ccntrl->src_mac[2]
			, acp_ccntrl->src_mac[3], acp_ccntrl->src_mac[4], acp_ccntrl->src_mac[5]
			, acp_ccntrl->ioparam.data_offset
			);

		edd_ccntrl->ConsumerID	= frame->edd_id;
		edd_ccntrl->Mode		= 0;

		if (acp_ccntrl->mode & ACP_CONSUMER_SET_ACTIVE)
		{
			edd_ccntrl->Mode	|= EDD_CONS_CONTROL_MODE_ACTIVATE;

			edd_ccntrl->SrcMAC.MacAdr[0] = acp_ccntrl->src_mac[0];
			edd_ccntrl->SrcMAC.MacAdr[1] = acp_ccntrl->src_mac[1];
			edd_ccntrl->SrcMAC.MacAdr[2] = acp_ccntrl->src_mac[2];
			edd_ccntrl->SrcMAC.MacAdr[3] = acp_ccntrl->src_mac[3];
			edd_ccntrl->SrcMAC.MacAdr[4] = acp_ccntrl->src_mac[4];
			edd_ccntrl->SrcMAC.MacAdr[5] = acp_ccntrl->src_mac[5];

			edd_ccntrl->SrcIP.IPAdr[0] = acp_ccntrl->src_ip[0];
			edd_ccntrl->SrcIP.IPAdr[1] = acp_ccntrl->src_ip[1];
			edd_ccntrl->SrcIP.IPAdr[2] = acp_ccntrl->src_ip[2];
			edd_ccntrl->SrcIP.IPAdr[3] = acp_ccntrl->src_ip[3];
		}
		else if (acp_ccntrl->mode & ACP_CONSUMER_SET_PASSIVE)
		{
			edd_ccntrl->Mode	|= EDD_CONS_CONTROL_MODE_PASSIVATE;
			
			if (acp_ccntrl->mode & ACP_CONSUMER_RESET_TO_FACTORY)
			{
				edd_ccntrl->Mode	|= EDD_CONS_CONTROL_MODE_REMOVE_ACW;
			}
		}

		if (acp_ccntrl->mode & ACP_CONSUMER_SET_TO_UNKNOWN_STATE)
		{
			edd_ccntrl->Mode	|= EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN;
		}

		if (acp_ccntrl->mode & ACP_CONSUMER_SET_AUTOSTOP_ENABLE)
		{
			edd_ccntrl->Mode	|= EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE;
		}
		else if (acp_ccntrl->mode & ACP_CONSUMER_SET_AUTOSTOP_DISABLE)
		{
			edd_ccntrl->Mode	|= EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE;
		}

		edd_ccntrl->CycleReductionRatio			= acp_ccntrl->reduction_ratio;
		edd_ccntrl->CyclePhase					= acp_ccntrl->phase;

		edd_ccntrl->IOParams.AppGroup			= acp_ccntrl->ioparam.app_group;
		edd_ccntrl->IOParams.ForwardMode		= acp_ccntrl->ioparam.forward_mode;
		edd_ccntrl->IOParams.CutThrough_OutDataStart
												= acp_ccntrl->ioparam.ct_outdatastart;
		edd_ccntrl->IOParams.ClearOnMISS		= acp_ccntrl->ioparam.clear_on_miss;
		edd_ccntrl->IOParams.BufferProperties	= acp_ccntrl->ioparam.properties;
		edd_ccntrl->IOParams.CRNumber			= acp_ccntrl->ioparam.cr_number;
		edd_ccntrl->IOParams.DataOffset			= acp_ccntrl->ioparam.data_offset;
		edd_ccntrl->IOParams.SFPosition			= acp_ccntrl->ioparam.sf_position;
		edd_ccntrl->IOParams.SFOffset			= acp_ccntrl->ioparam.sf_offset;

		acp_ll_request (channel->lower, lower, EDD_SRV_SRT_CONSUMER_CONTROL, frame);
	}

	return ACP_OK;
}


/*------------------------------------------------------------------------------
//	control the bits off _all_ providers in the channel
//----------------------------------------------------------------------------*/
LSA_INT16  ACP_LOCAL_FCT_ATTR
acp_edd_global_provider_state (ACP_CHANNEL_PTR  channel, LSA_UINT16 mode)
{
	ACP_LOG_FCT("acp_edd_global_provider_state")
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_SRT_PROVIDER_SET_STATE_PTR_TYPE  edd_glob;

	if (channel->max_group_id == 0)
	{
		if ( (channel->detailptr->type.user.nr_of_consumers_class1_unicast == 0
			&&	channel->detailptr->type.user.nr_of_consumers_class1_multicast == 0
		&&	channel->detailptr->type.user.nr_of_consumers_class2_unicast == 0
		&&	channel->detailptr->type.user.nr_of_consumers_class2_multicast == 0
		&&	channel->detailptr->type.user.nr_of_consumers_class3 == 0
		&&	channel->detailptr->type.user.nr_of_providers_class1_2 == 0
		&&	channel->detailptr->type.user.nr_of_providers_class3 == 0))
		{
			ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "special case: no srt "
				);
			acp_callback (channel, ACP_OK);
			return ACP_OK;
		}
		ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "ch.max_group_id == 0"
			);
		return ACP_ERR_SEQUENCE;
	}
	if (! (channel->detailptr->type.user.group_id < channel->max_group_id)) {
		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "! ch.group_id(%d) < ch.max_group_id(%d)"
			, channel->detailptr->type.user.group_id
			, channel->max_group_id
			);
		return ACP_ERR_RESOURCE;
	}

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		return ACP_ERR_ALLOC_LOWER_RQB;
	}

	edd_glob = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_SET_STATE_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_glob));

	edd_glob->GroupID		= channel->detailptr->type.user.group_id;
	edd_glob->Status		= 0;
	edd_glob->Mask			= 0;
	if (mode & ACP_GLOBAL_PROVIDER_SET_STOP) {
		edd_glob->Mask		|= EDD_CSRT_DSTAT_BIT_STOP_RUN;
	}
	if (mode & ACP_GLOBAL_PROVIDER_SET_RUN) {
		edd_glob->Mask		|= EDD_CSRT_DSTAT_BIT_STOP_RUN;
		edd_glob->Status	|= EDD_CSRT_DSTAT_BIT_STOP_RUN;
	}

	acp_ll_request (channel->lower, lower, EDD_SRV_SRT_PROVIDER_SET_STATE, channel);
	return ACP_OK;
}


/*------------------------------------------------------------------------------
//	requests the channel-info-data from EDD
//----------------------------------------------------------------------------*/
LSA_INT16  ACP_LOCAL_FCT_ATTR
acp_edd_channel_info (ACP_CHANNEL_PTR  channel)
{
	ACP_LOG_FCT("acp_edd_channel_info")
	ACP_LOWER_RQB_PTR_TYPE  lower;

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		return ACP_ERR_ALLOC_LOWER_RQB;
	}

	ACP_ASSERT (is_not_null (ACP_LL_GET_PPARAM (lower, LSA_VOID_PTR_TYPE)));

	acp_ll_request (channel->lower, lower, EDD_SRV_GET_PARAMS, channel);
	return ACP_OK;
}


/*------------------------------------------------------------------------------
//	check for an open frame-indication, if open: indicate
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_check_frame_indication (		/* ret: 0..no (more) indication resources, 1..else */
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	LSA_UINT8 ACP_LOCAL_MEM_ATTR  *debug_data
	)
{
	ACP_LOG_FCT("acp_check_frame_indication")
	ACP_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT32	delta_event;

	ACP_ASSERT (is_not_null (channel));
	ACP_ASSERT (is_not_null (frame));

	if (frame->st_internal == frame->st_signaled) {
		return 1;
	}

	delta_event = ((~ frame->st_signaled) & (frame->st_internal));

		/* TRACE: frame_id is missing, should add frame->frame_id */
	if (delta_event & (EDD_CSRT_CONS_EVENT_MISS | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED))
	{
		if (is_not_null (debug_data))
		{
			EDD_UPPER_MEM_U8_PTR_TYPE  dd = debug_data;
			LSA_UINT32 dd1, dd2, dd3, dd4;
			acp_dp_get_u32 (dd1, dd);
			acp_dp_get_u32 (dd2, dd);
			acp_dp_get_u32 (dd3, dd);
			acp_dp_get_u32 (dd4, dd);
		
			ACP_LL_TRACE_09 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
				, "MISS: acp_id(%u)  delta(0x%x)  st_intern(0x%x)  st_user(0x%x)  cycle_counter(%u) DebugData(0x%08x-%08x-%08x-%08x)"
				, frame->acp_id, delta_event, frame->st_internal, frame->st_signaled, frame->cycle_counter
				, dd1, dd2, dd3, dd4
				);
		}
		else
		{
			ACP_LL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
				, "MISS: acp_id(%u)  delta(0x%x)  st_intern(0x%x)  st_user(0x%x)  cycle_counter(%u)"
				, frame->acp_id, delta_event, frame->st_internal, frame->st_signaled, frame->cycle_counter
				);
		}
	}
	else if (delta_event & EDD_CSRT_CONS_EVENT_AGAIN)
	{
		ACP_LL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "AGAIN: acp_id(%u)  delta(0x%x)  st_intern(0x%x)  st_user(0x%x)"
			, frame->acp_id, delta_event, frame->st_internal, frame->st_signaled
			);
	}
	else
	{
		ACP_LL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "APDU: acp_id(%u)  delta(0x%x)  st_intern(0x%x)  st_user(0x%x)"
			, frame->acp_id, delta_event, frame->st_internal, frame->st_signaled
			);
	}

	acp_get_from_rqb_list (& channel->provide, & upper);
	if (is_null (upper))
	{
		ACP_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "no (more) ind res, acp_id(%d) st_internal(0x%x) st_signaled(0x%x)"
			, frame->acp_id, frame->st_internal, frame->st_signaled
			);
		return 0;
	}

	ACP_ASSERT (is_not_null (upper->args));

		/* put data inside */
	ACP_RQB_SET_OPCODE (upper, ACP_OPC_STATE_INDICATION);

	upper->args->state.ind.acp_id		= frame->acp_id;
	upper->args->state.ind.usr_id		= frame->usr_id;
	upper->args->state.ind.cycle_count	= frame->cycle_counter;
	upper->args->state.ind.state_event	= delta_event; /*32bit*/ /* not frame->st_internal */
	upper->args->state.ind.total_state	= frame->st_internal;

	if (is_not_null (debug_data))
	{
		ACP_MEMCPY (upper->args->state.ind.debug_data, debug_data, ACP_ERROR_INFO_DATA_SIZE);
	}
	else
	{
		/* my happen with lack of ind resources */
		LSA_UINT16  i;
		for (i = 0; i < ACP_ERROR_INFO_DATA_SIZE; ++i) {
			upper->args->state.ind.debug_data[i] = 0;
		}
	}
	
		/* ok, this state should known to the user */
	frame->st_signaled = frame->st_internal;

		/* notify the user */
	acp_callback_async (channel, ACP_OK, & upper);

	return (LSA_UINT16)((is_not_null (channel->provide)) ? 1 : 0);
}


/*------------------------------------------------------------------------------
//	add new indication resources, check for open drop-indication
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_state_provide (ACP_CHANNEL_PTR  channel)
{
	LSA_UINT16 i;
	LSA_UINT16 ind_resource;
	ACP_LOCAL_FRAME_PTR  frame;
	ACP_UPPER_RQB_PTR_TYPE  upper;

		/* have to check all frames */
	upper = channel->upper;
	channel->upper = LSA_NULL;

		/* optimisation: only needed wenn pushing the first rqb */
	if (acp_add_to_rqb_list (& channel->provide, upper)) {
		for (i = 1; i < channel->max_frames; ++i) {
			frame = & channel->frames[i];
			if (frame->state == ACP_FS_NO) {
				continue;
			}
			if (frame->st_internal != frame->st_signaled) {
				ind_resource = acp_check_frame_indication (channel, frame, LSA_NULL);
				if (! ind_resource) {
					break;
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
//	add new indication resources for link-status indication,
//		if first, starting edd-rqb-cycle
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_link_status_provide (
	ACP_CHANNEL_PTR  channel
	)
{
	ACP_LOG_FCT("acp_op_link_status_provide")
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_LINK_STATUS_PTR  acp_link;
	LSA_UINT16  rc;

	upper = channel->upper;
	channel->upper = LSA_NULL;

		/* check param */
	if (upper->args->channel.link.port_id != ACP_PORT_ID_AUTO) {
		if (channel->max_eth_ports == 0) {
			acp_callback_async (channel, ACP_ERR_SEQUENCE, & upper); /* must call first channel_info */
			return;
		}
		if (! (1 <= upper->args->channel.link.port_id
			&& upper->args->channel.link.port_id <= channel->max_eth_ports))
		{
			acp_callback_async (channel, ACP_ERR_PARAM, & upper); /* out-of-range */
			return;
		}
	}

		/* is there an existing acp-edd cycle ? */
	acp_link = acp_link_status_find (channel->lower, upper->args->channel.link.port_id);

		/* need a new one */
	if (is_null (acp_link)) {
		rc = acp_link_status_alloc (channel->lower, upper);
		if (rc != ACP_OK) {
			acp_callback_async (channel, rc, & upper);
			return;
		}
	}

		/* use the existing one */
	else {
		rc = acp_link_status_compare_set (channel->lower, acp_link, upper);
		if (rc == ACP_OK) {
			ACP_RQB_SET_OPCODE (upper, ACP_OPC_LINK_STATUS_INDICATION);
			acp_callback_async (channel, rc, & upper);
			return;
		}
		ACP_ASSERT (rc == ACP_OK_CANCEL);
	}

	AcpListInsertTail (& channel->link_status_list, (ACP_LIST_ENTRY_PTR_TYPE)upper);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_link_status_provide_done (
	ACP_LOWER_PTR  channel_lower,
    ACP_LOWER_RQB_PTR_TYPE  edd
	)
{
	ACP_LOG_FCT("acp_op_link_status_provide_provide")
	LSA_UINT16  i;
	ACP_CHANNEL_PTR  channel;
	LSA_UINT16  rsp;
	ACP_LL_LINK_STATUS_PROVIDE_PTR_TYPE  edd_link;
	ACP_LINK_STATUS_PTR  acp_link;

	rsp = ACP_LL_GET_RESPONSE (edd);
	edd_link = ACP_LL_GET_PPARAM (edd, ACP_LL_LINK_STATUS_PROVIDE_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_link));

	acp_link = acp_link_status_find (channel_lower, edd_link->PortID);
	if (is_null (acp_link)) {
		ACP_FATAL (edd_link->PortID);
		return;
	}

	switch (rsp) {
	case EDD_STS_OK_CANCEL:
			/* cancel all corresponding upper-rqb, cancel acp_link to lower */
		{
		ACP_UPPER_RQB_PTR_TYPE  upper, next;
		LSA_UINT16  rc;

		for (i = 0; i < ACP_CFG_MAX_CHANNELS; ++i) {
			channel = & acp_channels[i];
			if (channel->state != ACP_CHANNEL_STATE_READY) {
				continue;
			}
			if (! are_equal (channel->lower, channel_lower)) {
				continue;
			}

			for (next = LSA_NULL, upper = AcpListIterFirst (&channel->link_status_list, ACP_UPPER_RQB_PTR_TYPE);
				is_not_null (upper);
				upper = next)
			{
				next = AcpListIterNext (& channel->link_status_list, (ACP_LIST_ENTRY_PTR_TYPE)upper, ACP_UPPER_RQB_PTR_TYPE);

				if (upper->args->channel.link.port_id == acp_link->port_id) {
					AcpListRemoveEntry ((ACP_LIST_ENTRY_PTR_TYPE)upper);
					ACP_ASSERT (ACP_RQB_GET_OPCODE (upper) == ACP_OPC_LINK_STATUS_IND_RES_PROVIDE);
					acp_callback_async (channel, ACP_OK_CANCEL, & upper);
				}
			}
		}

		AcpListRemoveEntry (& acp_link->link);
		ACP_FREE_LOCAL_MEM (& rc, acp_link);
		ACP_ASSERT (rc == LSA_RET_OK);

		acp_ll_free_rqb (channel_lower, edd);
		}
		break;

	case EDD_STS_OK:
			/* indicate all corresponding upper-rqb */
		{
		ACP_UPPER_RQB_PTR_TYPE  upper, next;
		LSA_UINT16  rc;

		if (edd_link->Status == EDD_LINK_UNKNOWN) {
			ACP_FATAL (edd_link->Status);
			return;
		}

		acp_link->status = edd_link->Status;
		acp_link->speed = edd_link->Speed;
		acp_link->mode = edd_link->Mode;

		if (edd_link->Status == EDD_LINK_DOWN) {
			acp_link->speed = ACP_LINK_SPEED_UNKNOWN;
			acp_link->mode = ACP_LINK_MODE_UNKNOWN;

			ACP_LL_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "port_id(%u): Link goes down"
				, acp_link->port_id
				);
		}
		else {
			ACP_LL_TRACE_04 (channel_lower->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "port_id(%u): Link goes up, status(%u) speed(%u) mode(%u)"
				, acp_link->port_id
				, acp_link->status, acp_link->speed, acp_link->mode
				);
		}

		for (i = 0; i < ACP_CFG_MAX_CHANNELS; ++i) {
			channel = & acp_channels[i];
			if (channel->state != ACP_CHANNEL_STATE_READY) {
				continue;
			}
			if (! are_equal (channel->lower, channel_lower)) {
				continue;
			}

			for (next = LSA_NULL, upper = AcpListIterFirst (&channel->link_status_list, ACP_UPPER_RQB_PTR_TYPE);
				is_not_null (upper);
				upper = next)
			{
				next = AcpListIterNext (& channel->link_status_list, (ACP_LIST_ENTRY_PTR_TYPE)upper, ACP_UPPER_RQB_PTR_TYPE);

				if (upper->args->channel.link.port_id == acp_link->port_id) {

					rc = acp_link_status_compare_set (channel_lower, acp_link, upper);
					if (rc == ACP_OK_CANCEL) {
						continue;
					}
					ACP_ASSERT (rc == ACP_OK);

					AcpListRemoveEntry ((ACP_LIST_ENTRY_PTR_TYPE)upper);
					ACP_ASSERT (ACP_RQB_GET_OPCODE (upper) == ACP_OPC_LINK_STATUS_IND_RES_PROVIDE);
					ACP_RQB_SET_OPCODE (upper, ACP_OPC_LINK_STATUS_INDICATION);
					acp_callback_async (channel, rc, & upper);
				}
			}
		}

			/* if lower is not closing: provide the rqb again */
		if (channel_lower->count_of_open_channel != 0xFFFF)
		{
			acp_ll_request (channel_lower, edd, EDD_SRV_LINK_STATUS_IND_PROVIDE, channel_lower);
		}
			/* else free */
		else
		{
			AcpListRemoveEntry (& acp_link->link);
			ACP_FREE_LOCAL_MEM (& rc, acp_link);
			ACP_ASSERT (rc == LSA_RET_OK);

			acp_ll_free_rqb (channel_lower, edd);
		}
		}
		break;

	default:
		acp_ll_free_rqb (channel_lower, edd);
		ACP_FATAL (rsp);
		break;
	}
}


/*------------------------------------------------------------------------------
//	maps the edd_response-code to an acp-response-code
//----------------------------------------------------------------------------*/
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_rsp_from_edd_rsp (
	LSA_UINT16  edd_rsp
	)
{
	switch (edd_rsp) {
	case EDD_STS_OK:			return ACP_OK;
	case EDD_STS_OK_CANCEL:		return ACP_OK_CANCEL;
	case EDD_STS_ERR_PARAM:		return ACP_ERR_EDD_PARAM;
	case EDD_STS_ERR_RESOURCE:	return ACP_ERR_EDD_RESOURCE;
	default:					return ACP_ERR_LOWER_LAYER;
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_CHANNEL_INFO
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_channel_info (
	ACP_CHANNEL_PTR  channel
	)
{
	LSA_UINT16 rc;

	rc = acp_edd_channel_info (channel);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_check_range (
	LSA_UINT16  edd_cnt,
	LSA_UINT16  edd_base,
	LSA_UINT16  acp_cnt,
	LSA_UINT16  acp_base
	)
{
	if (acp_cnt == 0) {
		return LSA_TRUE;
	}
	if (edd_cnt == 0) {
		return LSA_FALSE;
	}
	if (! (edd_base <= acp_base)) {
		return LSA_FALSE;
	}
	if (! (acp_base + acp_cnt <= edd_base + edd_cnt)) {
		return LSA_FALSE;
	}
	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_CHANNEL_INFO
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_channel_info_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_channel_info_done")
	ACP_LL_SRT_PARAM_PTR_TYPE  edd_params;
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_CHANNEL_UPPER_INFO_PTR_TYPE  info;
	LSA_UINT16  rsp;
	LSA_UINT16  udp_port;

	ACP_PUT16_HTON (& udp_port, 0, ACP_IANA_PNIO_UDP_UNICAST_PORT);

	rsp = ACP_LL_GET_RESPONSE (lower);
	edd_params = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PARAM_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_params));

	if (rsp != EDD_STS_OK) {
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rsp(%d)"
			, rsp
			);
		acp_ll_free_rqb (channel->lower, lower);
		acp_callback (channel, ACP_ERR_LOWER_LAYER);
		return;
	}

	if (! acp_check_range (edd_params->ConsumerCntClass1
			, edd_params->ConsumerFrameIDBaseClass1
			, channel->detailptr->type.user.nr_of_consumers_class1_unicast
			, channel->detailptr->type.user.frame_id_base_class1_unicast
			))
	{
		ACP_FATAL (channel->detailptr->type.user.frame_id_base_class1_unicast);
		return;
	}
	if (! acp_check_range (edd_params->ConsumerCntClass1
			, edd_params->ConsumerFrameIDBaseClass1
			, channel->detailptr->type.user.nr_of_consumers_class1_multicast
			, channel->detailptr->type.user.frame_id_base_class1_multicast
			))
	{
		ACP_FATAL (channel->detailptr->type.user.frame_id_base_class1_multicast);
		return;
	}
	if (! acp_check_range (edd_params->ConsumerCntClass2
			, edd_params->ConsumerFrameIDBaseClass2
			, channel->detailptr->type.user.nr_of_consumers_class2_unicast
			, channel->detailptr->type.user.frame_id_base_class2_unicast
			))
	{
		ACP_FATAL (channel->detailptr->type.user.frame_id_base_class2_unicast);
		return;
	}
	if (! acp_check_range (edd_params->ConsumerCntClass2
			, edd_params->ConsumerFrameIDBaseClass2
			, channel->detailptr->type.user.nr_of_consumers_class2_multicast
			, channel->detailptr->type.user.frame_id_base_class2_multicast
			))
	{
		ACP_FATAL (channel->detailptr->type.user.frame_id_base_class2_multicast);
		return;
	}
	if (edd_params->ConsumerCntClass3 /* AP01457872 */
		< channel->detailptr->type.user.nr_of_consumers_class3)
	{
		ACP_FATAL (channel->detailptr->type.user.nr_of_consumers_class3);
		return;
	}
	if (edd_params->ConsumerCntClass12Combined) {
		if (edd_params->ConsumerCntClass1 != edd_params->ConsumerCntClass2) {
			ACP_FATAL (edd_params->ConsumerCntClass1);
			return;
		}
		if (channel->detailptr->type.user.nr_of_consumers_class1_unicast
			!= channel->detailptr->type.user.nr_of_consumers_class2_unicast)
		{
			ACP_FATAL (channel->detailptr->type.user.nr_of_consumers_class1_unicast);
			return;
		}
		if (channel->detailptr->type.user.nr_of_consumers_class1_multicast
			!= channel->detailptr->type.user.nr_of_consumers_class2_multicast)
		{
			ACP_FATAL (channel->detailptr->type.user.nr_of_consumers_class1_multicast);
			return;
		}
	}

	upper = channel->upper;
	if (is_null (upper)) {
		ACP_FATAL (channel);
		return;
	}

	info = & upper->args->channel.info;

	info->hardware_type				= edd_params->HardwareType;
	info->cycle_base_factor			= edd_params->CycleBaseFactor;

	info->edd_io_datalen_min		= edd_params->DataLenMin;
	info->edd_io_datalen_max		= edd_params->DataLenMax;

	info->edd_io_udp_datalen_min	= edd_params->DataLenMinUDP;
	info->edd_io_udp_datalen_max	= edd_params->DataLenMaxUDP;

	info->frame_id_base_class1_unicast		= channel->detailptr->type.user.frame_id_base_class1_unicast;
	info->nr_of_consumer_class1_unicast		= channel->detailptr->type.user.nr_of_consumers_class1_unicast;
	info->frame_id_base_class1_multicast	= channel->detailptr->type.user.frame_id_base_class1_multicast;
	info->nr_of_consumer_class1_multicast	= channel->detailptr->type.user.nr_of_consumers_class1_multicast;

	info->frame_id_base_class2_unicast		= channel->detailptr->type.user.frame_id_base_class2_unicast;
	info->nr_of_consumer_class2_unicast		= channel->detailptr->type.user.nr_of_consumers_class2_unicast;
	info->frame_id_base_class2_multicast	= channel->detailptr->type.user.frame_id_base_class2_multicast;
	info->nr_of_consumer_class2_multicast	= channel->detailptr->type.user.nr_of_consumers_class2_multicast;

	info->nr_of_provider_class1_2			= channel->detailptr->type.user.nr_of_providers_class1_2;
	info->consumer_class1_2_is_combined		= edd_params->ConsumerCntClass12Combined;

	info->nr_of_consumer_class3		= channel->detailptr->type.user.nr_of_consumers_class3;
	info->nr_of_provider_class3		= channel->detailptr->type.user.nr_of_providers_class3;

	ACP_MEMCPY (info->eth_mac_addr,	  edd_params->MACAddr.MacAdr, ACP_MAC_ADDR_SIZE);
	ACP_MEMCPY (channel->mac_addr,	  edd_params->MACAddr.MacAdr, ACP_MAC_ADDR_SIZE);

	info->eth_interface_id			= (LSA_UINT16) edd_params->InterfaceID;

	channel->trace_idx				= (LSA_UINT16) edd_params->TraceIdx;
	if (channel->lower->trace_idx == 0) {
		channel->lower->trace_idx	= (LSA_UINT16) edd_params->TraceIdx;
	}
	else {
		ACP_ASSERT (channel->lower->trace_idx == (LSA_UINT16)edd_params->TraceIdx);
	}

	info->eth_ports_max				= edd_params->MaxPortCnt;
	channel->max_eth_ports			= edd_params->MaxPortCnt;

	ACP_MEMCPY (info->ip_addr,		  edd_params->LocalIP.IPAdr, ACP_IP_ADDR_SIZE);
	ACP_MEMCPY (channel->ip_addr,	  edd_params->LocalIP.IPAdr, ACP_IP_ADDR_SIZE);

	info->udp_port					= udp_port; /* network byte order */

	info->alarm_ref_base			= channel->detailptr->type.user.alarm_ref_base;
	info->alarm_nr_of_ref			= channel->detailptr->type.user.alarm_nr_of_ref;

	info->provider_max_phase_byte	= edd_params->ProviderCyclePhaseMaxByteCnt;
	info->provider_max_phase_count	= edd_params->ProviderCyclePhaseMaxCnt;

	channel->max_group_id			= edd_params->ProviderGroupCnt;
	ACP_ASSERT (edd_params->ProviderGroupCnt <= EDD_CFG_CSRT_MAX_PROVIDER_GROUP);

	if (channel->detailptr->type.user.group_id >= edd_params->ProviderGroupCnt) {
		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "detail.group_id(%d) >= edd.GroupCnt(%d)"
			, channel->detailptr->type.user.group_id
			, edd_params->ProviderGroupCnt
			);
		channel->max_group_id = 0;	/* acp_edd_frame_add() returns a ERR_SEQUENCE */

		if ( !(channel->detailptr->type.user.nr_of_consumers_class1_unicast == 0
		&&	channel->detailptr->type.user.nr_of_consumers_class1_multicast == 0
		&&	channel->detailptr->type.user.nr_of_consumers_class2_unicast == 0
		&&	channel->detailptr->type.user.nr_of_consumers_class2_multicast == 0
		&&	channel->detailptr->type.user.nr_of_consumers_class3 == 0
		&&	channel->detailptr->type.user.nr_of_providers_class1_2 == 0
		&&	channel->detailptr->type.user.nr_of_providers_class3 == 0))
		{
			acp_ll_free_rqb (channel->lower, lower);
			acp_callback (channel, ACP_ERR_LOWER_LAYER);
			return;
		}
	}

	info->kram_info.kram_base_addr				= edd_params->KRAMInfo.KRAMBaseAddr;

    info->kram_info.offset_process_image_start	= edd_params->KRAMInfo.offset_ProcessImageStart;
    info->kram_info.offset_async_in_start		= edd_params->KRAMInfo.offset_async_in_start;
    info->kram_info.size_async_in				= edd_params->KRAMInfo.size_async_in;
    info->kram_info.offset_async_out_start		= edd_params->KRAMInfo.offset_async_out_start;
    info->kram_info.size_async_out				= edd_params->KRAMInfo.size_async_out;
    info->kram_info.offset_process_image_end	= edd_params->KRAMInfo.offset_ProcessImageEnd;

	acp_ll_free_rqb (channel->lower, lower);
	acp_callback (channel, ACP_OK);
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_PROVIDER_ADD
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_add (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	LSA_UINT16  rc;

	rc = acp_edd_frame_add (channel, frame);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_PROVIDER_ADD
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_add_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_provider_add_done")
	ACP_LL_SRT_PROVIDER_ADD_PTR_TYPE  edd_padd;
	LSA_UINT16  rsp;
	LSA_UINT16  edd_prov_id;
	LSA_UINT32  edd_cw_offset;
	LSA_UINT32  edd_data_status_offset;

	rsp = ACP_LL_GET_RESPONSE (lower);

	edd_padd = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_ADD_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_padd));

	edd_prov_id = edd_padd->ProviderID;
	edd_cw_offset = edd_padd->IOParams.CWOffset;
	edd_data_status_offset = edd_padd->IOParams.DataStatusOffset;

	acp_ll_free_rqb (channel->lower, lower);

	if (rsp == EDD_STS_OK) {
		ACP_UPPER_PROVIDER_ADD_PTR_TYPE  padd = & channel->upper->args->prov.add;

		frame->edd_id = edd_prov_id;
		frame->state = ACP_FS_PROV_PASSIVE;

		padd->ioparam.edd_prov_id = edd_prov_id;
		padd->ioparam.cw_offset = edd_cw_offset;
		padd->ioparam.data_status_offset = edd_data_status_offset;

		ACP_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ProviderAddDone: acp_id(%u) edd_id(%u)  cw_offset(0x%x) ds_offset(0x%x)"
			, frame->acp_id, frame->edd_id, edd_cw_offset, edd_data_status_offset
			);
		acp_callback (channel, ACP_OK);
	}
	else {
		frame->state = ACP_FS_NO;

		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ProviderAddDone: acp_id(%u) rsp(%d)"
			, frame->acp_id, rsp
			);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_PROVIDER_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_control (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	ACP_LOG_FCT("acp_op_provider_control")
	LSA_UINT16  rc;
	LSA_UINT16  prov_mode;

	ACP_ASSERT (channel->upper);
	ACP_ASSERT (channel->upper->args);

	prov_mode = channel->upper->args->prov.control.mode;
	rc = (LSA_UINT16)(prov_mode
		& (ACP_PROVIDER_SET_ACTIVE | ACP_PROVIDER_SET_PASSIVE
			| ACP_PROVIDER_SET_ACTIVE_AUX | ACP_PROVIDER_SET_PASSIVE_AUX
			| ACP_PROVIDER_APDU_STATUS | ACP_PROVIDER_SET_AUTOSTOP
			| ACP_PROVIDER_SET_IOPARAM))
		;
	if ((rc != 0)
	&&  (prov_mode &  ACP_PROVIDER_SET_IOPARAM) != 0
	&&	(prov_mode != ACP_PROVIDER_SET_IOPARAM))
	{
		rc = 0;
	}
	if (rc == 0)
	{
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "prov.ctrl.mode(0x%x) unknown"
			, channel->upper->args->prov.control.mode
			);
		acp_callback (channel, ACP_ERR_PARAM);
		return;
	}

	rc = acp_edd_frame_control (channel, frame);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_PROVIDER_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_control_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_provider_control_done")
	LSA_UINT16  rsp = ACP_LL_GET_RESPONSE (lower);
	ACP_LL_SRT_PROVIDER_CONTROL_PTR_TYPE  edd_pcntrl = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_CONTROL_PTR_TYPE);

	ACP_ASSERT (is_not_null (edd_pcntrl));

	if (rsp == EDD_STS_OK)
	{
		if (edd_pcntrl->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE) {
			frame->state = ACP_FS_PROV_ACTIVE;
		}
		else if (edd_pcntrl->Mode & EDD_PROV_CONTROL_MODE_PASSIVATE) {
			acp_frame_set_passive (frame);
			frame->state = ACP_FS_PROV_PASSIVE;
		}

		acp_ll_free_rqb (channel->lower, lower);
		acp_callback (channel, ACP_OK);
	}
	else
	{
		ACP_UPPER_RQB_PTR_TYPE  upper = channel->upper;
		
		ACP_MEMCPY (upper->args->prov.control.debug_data, edd_pcntrl->DebugInfo, sizeof(upper->args->prov.control.debug_data));

		ACP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "acp_id(%d) rsp(%d) st(%d) sts(%x) sti(%x)"
			, frame->acp_id, rsp
			, frame->state, frame->st_signaled, frame->st_internal
			);
		acp_ll_free_rqb (channel->lower, lower);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_PROVIDER_XCHANGE_BUF
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_xbuf (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	ACP_LOG_FCT("acp_op_provider_xbuf")
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_SRT_PROVIDER_XCHANGE_BUF_PTR_TYPE  edd_xbuf;

	ACP_ASSERT (is_not_null (channel->upper));
	ACP_ASSERT (is_not_null (frame));

	upper = channel->upper;

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		acp_callback (channel, ACP_ERR_ALLOC_LOWER_RQB);
		return;
	}

	edd_xbuf = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_XCHANGE_BUF_PTR_TYPE);

	edd_xbuf->ProviderID	= frame->edd_id;
	edd_xbuf->NewOffset		= upper->args->prov.xbuf.new_offset;
	edd_xbuf->OldOffset		= upper->args->prov.xbuf.old_offset;
	edd_xbuf->DataLen		= upper->args->prov.xbuf.data_len;

	acp_ll_request (channel->lower, lower, EDD_SRV_SRT_PROVIDER_XCHANGE_BUF, frame);
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_PROVIDER_XCHANGE_BUF
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_xbuf_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_provider_xbuf_done")
	ACP_LL_SRT_PROVIDER_XCHANGE_BUF_PTR_TYPE  edd_xbuf;
	LSA_UINT16  rsp;

	LSA_UNUSED_ARG (frame);

	rsp = ACP_LL_GET_RESPONSE (lower);
	edd_xbuf = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_XCHANGE_BUF_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_xbuf));

	acp_ll_free_rqb (channel->lower, lower);

	acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_GLOBAL_PROVIDER_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_global_provider_control (
	ACP_CHANNEL_PTR  channel
	)
{
	ACP_LOG_FCT("acp_op_global_provider_control")
	LSA_UINT16  rc;

	ACP_ASSERT (channel->upper);
	ACP_ASSERT (channel->upper->args);

	rc = acp_edd_global_provider_state (channel,
			channel->upper->args->global_prov.control.mode
		);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_GLOBAL_PROVIDER_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_global_provider_control_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_global_provider_control_done")
	LSA_UINT16  rsp;

	rsp = ACP_LL_GET_RESPONSE (lower);
	/* should we do anything here ? channel->global_producer_state = x */
	acp_ll_free_rqb (channel->lower, lower);

	if (rsp == ACP_OK) {
		ACP_UPPER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ok"
			);
		acp_callback (channel, ACP_OK);
	}
	else {
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rsp(%d)"
			, rsp
			);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_PROVIDER_CHANGE_PHASE
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_change_phase (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	ACP_LOG_FCT("acp_op_provider_change_phase")
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_SRT_PROVIDER_CHANGE_PHASE_PTR_TYPE  edd_phase;

	ACP_ASSERT (channel->upper);
	ACP_ASSERT (channel->upper->args);

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		acp_callback_async (channel, ACP_ERR_ALLOC_LOWER_RQB, & channel->upper);
		return ;
	}

	ACP_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "acp_id(%d) edd_id(%d) phase(%d) sequence(%d)"
		, frame->acp_id
		, frame->edd_id
		, channel->upper->args->prov.change_phase.phase
		, channel->upper->args->prov.change_phase.sequence
		);

	edd_phase						= ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PROVIDER_CHANGE_PHASE_PTR_TYPE);

	edd_phase->ProviderID			= frame->edd_id;
	edd_phase->CyclePhase			= channel->upper->args->prov.change_phase.phase;
	edd_phase->CyclePhaseSequence	= channel->upper->args->prov.change_phase.sequence;

	acp_ll_request (channel->lower, lower, EDD_SRV_SRT_PROVIDER_CHANGE_PHASE, frame);
	return;
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_PROVIDER_CHANGE_PHASE
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_change_phase_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_provider_change_phase_done")
	LSA_UINT16  rsp;

	LSA_UNUSED_ARG (frame);

	rsp = ACP_LL_GET_RESPONSE (lower);

	acp_ll_free_rqb (channel->lower, lower);

	if (rsp == EDD_STS_OK) {
		acp_callback (channel, ACP_OK);
	}
	else {
		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "acp_id(%d) rsp(%d)"
			, frame->acp_id
			, rsp
			);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
	return;
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_PROVIDER_REMOVE
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_remove (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	LSA_UINT16  rc;

	rc = acp_edd_frame_remove (channel, frame);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_PROVIDER_REMOVE
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_remove_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_provider_remove_done")
	LSA_UINT16  rsp;

	rsp = ACP_LL_GET_RESPONSE (lower);

	acp_ll_free_rqb (channel->lower, lower);

	if (rsp == EDD_STS_OK) {
		frame->state = ACP_FS_NO;

		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "acp_id(%d) fst(%d)"
			, frame->acp_id
			, frame->state
			);
		acp_callback (channel, ACP_OK);
	}
	else {
		ACP_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "acp_id(%d) rsp(%d) st(%d) edd_id(%d)"
			, frame->acp_id
			, rsp
			, frame->state
			, frame->edd_id
			);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_CONSUMER_ADD
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_add (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	LSA_UINT16  rc;

	rc = acp_edd_frame_add (channel, frame);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_CONSUMER_ADD
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_add_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_consumer_add_done")
	ACP_LL_SRT_CONSUMER_ADD_PTR_TYPE  edd_cadd;
	LSA_UINT16  rsp;
	LSA_UINT16  cons_id;
	LSA_UINT32  apdu_status_offset;

	rsp = ACP_LL_GET_RESPONSE (lower);

	edd_cadd = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_CONSUMER_ADD_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_cadd));

	cons_id = edd_cadd->ConsumerID;
	apdu_status_offset = edd_cadd->IOParams.APDUStatusOffset;

	acp_ll_free_rqb (channel->lower, lower);

	if (rsp == EDD_STS_OK) {
		ACP_UPPER_CONSUMER_ADD_PTR_TYPE  cadd = & channel->upper->args->cons.add;

		frame->edd_id = cons_id;
		frame->state = ACP_FS_CONS_PASSIVE;

		cadd->ioparam.apdu_status_offset = apdu_status_offset;

		ACP_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ConsumerAddDone: acp_id(%d) edd_id(%d) apdu_status_offset(%#x)"
			, frame->acp_id, frame->edd_id, apdu_status_offset
			);
		acp_callback (channel, ACP_OK);
	}
	else {
		frame->state = ACP_FS_NO;

		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ConsumerAddDone: acp_id(%d) rsp(%d)"
			, frame->acp_id
			, rsp
			);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_CONSUMER_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_control (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	ACP_LOG_FCT("acp_op_consumer_control")
	LSA_UINT16  rc;

	ACP_ASSERT (channel->upper);
	ACP_ASSERT (channel->upper->args);

	rc = acp_edd_frame_control (channel, frame);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_CONSUMER_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_control_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_consumer_control_done")
	LSA_UINT16  rsp = ACP_LL_GET_RESPONSE (lower);
	ACP_LL_SRT_CONSUMER_CONTROL_PTR_TYPE  edd_ccntrl = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_CONSUMER_CONTROL_PTR_TYPE);

	ACP_ASSERT (is_not_null (edd_ccntrl));

	if (rsp == EDD_STS_OK)
	{
		if (edd_ccntrl->Mode & EDD_CONS_CONTROL_MODE_ACTIVATE) {
			frame->state = ACP_FS_CONS_ACTIVE;
		}
		else if (edd_ccntrl->Mode & EDD_CONS_CONTROL_MODE_PASSIVATE) {
			acp_frame_set_passive (frame);
			frame->state = ACP_FS_CONS_PASSIVE;
		}

		if (edd_ccntrl->Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN)
		{
			frame->st_signaled = (LSA_UINT16)
					(frame->st_signaled &  (~(ACP_STATE_MISS | ACP_STATE_MISS_NOT_STOPPED | ACP_STATE_AGAIN)))
				;
			frame->st_internal = (LSA_UINT16)
					(frame->st_internal &  (~(ACP_STATE_MISS | ACP_STATE_MISS_NOT_STOPPED| ACP_STATE_AGAIN)))
				;
		}

		acp_ll_free_rqb (channel->lower, lower);
		acp_callback (channel, ACP_OK);
	}
	else
	{
		ACP_UPPER_RQB_PTR_TYPE  upper = channel->upper;
		
		ACP_MEMCPY (upper->args->cons.control.debug_data, edd_ccntrl->DebugInfo, sizeof(upper->args->cons.control.debug_data));

		ACP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "acp_id(%d) rsp(%d) st(%d) sts(%x) sti(%x)"
			, frame->acp_id, rsp
			, frame->state, frame->st_signaled, frame->st_internal
			);

		acp_ll_free_rqb (channel->lower, lower);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_CONSUMER_XCHANGE_BUF
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_xbuf (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	ACP_LOG_FCT("acp_op_consumer_xbuf")
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_SRT_CONSUMER_XCHANGE_BUF_PTR_TYPE  edd_xbuf;

	ACP_ASSERT (is_not_null (channel->upper));
	ACP_ASSERT (is_not_null (frame));

	upper = channel->upper;

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		acp_callback (channel, ACP_ERR_ALLOC_LOWER_RQB);
		return;
	}

	edd_xbuf = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_CONSUMER_XCHANGE_BUF_PTR_TYPE);

	edd_xbuf->ConsumerID	= frame->edd_id;
	edd_xbuf->NewOffset		= upper->args->cons.xbuf.new_offset;
	edd_xbuf->OldOffset		= upper->args->cons.xbuf.old_offset;
	edd_xbuf->DataLen		= upper->args->cons.xbuf.data_len;
	edd_xbuf->CopyUserData	= upper->args->cons.xbuf.data_copy;

	acp_ll_request (channel->lower, lower, EDD_SRV_SRT_CONSUMER_XCHANGE_BUF, frame);
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_CONSUMER_XCHANGE_BUF
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_xbuf_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_consumer_xbuf_done")
	ACP_LL_SRT_CONSUMER_XCHANGE_BUF_PTR_TYPE  edd_xbuf;
	LSA_UINT16  rsp;
	LSA_UINT32  apdu_status_offset;

	LSA_UNUSED_ARG (frame);

	rsp = ACP_LL_GET_RESPONSE (lower);
	edd_xbuf = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_CONSUMER_XCHANGE_BUF_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_xbuf));
	apdu_status_offset = edd_xbuf->APDUStatusOffset;

	acp_ll_free_rqb (channel->lower, lower);

	rsp = acp_rsp_from_edd_rsp (rsp);
	if (rsp == ACP_OK) {
		channel->upper->args->cons.xbuf.apdu_status_offset = apdu_status_offset;
	}

	acp_callback (channel, rsp);
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_CONSUMER_REMOVE
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_remove (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	LSA_UINT16  rc;

	rc = acp_edd_frame_remove (channel, frame);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_CONSUMER_REMOVE
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_remove_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_consumer_remove_done")
	LSA_UINT16  rsp;

	rsp = ACP_LL_GET_RESPONSE (lower);

	acp_ll_free_rqb (channel->lower, lower);

	if (rsp == EDD_STS_OK) {
		frame->state = ACP_FS_NO;

		ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "acp_id(%d) fst(%d)"
			, frame->acp_id
			, frame->state
			);
		acp_callback (channel, ACP_OK);
	}
	else {
		ACP_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "acp_id(%d) rsp(%d) st(%d) edd_id(%d)"
			, frame->acp_id
			, rsp
			, frame->state
			, frame->edd_id
			);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_GET_APDU_STATUS
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_get_apdu_status (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	)
{
	ACP_LOG_FCT("acp_op_get_apdu_status")
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_GET_APDU_STATUS_PTR_TYPE  edd_apdu;

	ACP_ASSERT (channel->upper);
	ACP_ASSERT (channel->upper->args);

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		acp_callback_async (channel, ACP_ERR_ALLOC_LOWER_RQB, & channel->upper);
		return ;
	}

	ACP_UPPER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "acp_id(%d) is_consumer(%d)"
		, frame->acp_id
		, acp_is_consumer (frame)
		);

	edd_apdu = ACP_LL_GET_PPARAM (lower, ACP_LL_GET_APDU_STATUS_PTR_TYPE);

	if (acp_is_consumer (frame)) {
		edd_apdu->Type = EDD_CSRT_TYPE_CONSUMER;
	}
	else {
		edd_apdu->Type = EDD_CSRT_TYPE_PROVIDER;
	}
	edd_apdu->ID = frame->edd_id;

	acp_ll_request (channel->lower, lower, EDD_SRV_SRT_GET_APDU_STATUS, frame);
	return;
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_GET_APDU_STATUS
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_get_apdu_status_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_get_apdu_status_done")
	ACP_LL_GET_APDU_STATUS_PTR_TYPE  edd_apdu;
	ACP_UPPER_GET_APDU_STATUS_PTR_TYPE  apdu;
	LSA_UINT16  rsp;

	LSA_UNUSED_ARG (frame);

	rsp = ACP_LL_GET_RESPONSE (lower);

	if (rsp != EDD_STS_OK) {
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rsp(%d)"
			, rsp
			);
		acp_ll_free_rqb (channel->lower, lower);
		acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
		return;
	}

	edd_apdu = ACP_LL_GET_PPARAM (lower, ACP_LL_GET_APDU_STATUS_PTR_TYPE);

	ACP_ASSERT (is_not_null (channel->upper));
	apdu = & channel->upper->args->channel.apdu_status;

	if (edd_apdu->Present) {
		apdu->present = LSA_TRUE;
		apdu->cycle_count = edd_apdu->APDUStatus.CycleCnt;
		apdu->data_status = edd_apdu->APDUStatus.DataStatus;
		apdu->transfer_status = edd_apdu->APDUStatus.TransferStatus;
		ACP_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "acp_id(%d) present(TRUE) cycle(%d) ds(0x%x) ts(0x%x)"
			, apdu->acp_id
			, apdu->cycle_count
			, apdu->data_status
			, apdu->transfer_status
			);
	}
	else {
		apdu->present = LSA_FALSE;
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "acp_id(%d) present(FALSE)"
			, apdu->acp_id
			);
	}

	acp_ll_free_rqb (channel->lower, lower);
	acp_callback (channel, ACP_OK);
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_MULTICAST
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_multicast (
	ACP_CHANNEL_PTR  channel
	)
{
	ACP_LOG_FCT("acp_op_multicast")
	ACP_UPPER_MULTICAST_PTR_TYPE  acp_mc;
	ACP_LOWER_RQB_PTR_TYPE  lower;
	ACP_LL_MULTICAST_PTR_TYPE  edd_mc;

	ACP_ASSERT (is_not_null (channel->upper));
	ACP_ASSERT (is_not_null (channel->upper->args));

	acp_mc = & channel->upper->args->channel.multicast;

	if (! (acp_mc->mode == ACP_MULTICAST_ENABLE || acp_mc->mode == ACP_MULTICAST_DISABLE)) {
		acp_callback_async (channel, ACP_ERR_PARAM, & channel->upper);
		return;
	}

	lower = acp_ll_alloc_rqb (channel->lower);
	if (is_null (lower)) {
		acp_callback_async (channel, ACP_ERR_ALLOC_LOWER_RQB, & channel->upper);
		return ;
	}

	ACP_UPPER_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "mode(%d) mac_addr(%02x-%02x-%02x-%02x-%02x-%02x)"
		, acp_mc->mode
		, acp_mc->mac_addr[0]
		, acp_mc->mac_addr[1]
		, acp_mc->mac_addr[2]
		, acp_mc->mac_addr[3]
		, acp_mc->mac_addr[4]
		, acp_mc->mac_addr[5]
		);

	edd_mc = ACP_LL_GET_PPARAM (lower, ACP_LL_MULTICAST_PTR_TYPE);

	edd_mc->Mode = acp_mc->mode;
	ACP_MEMCPY (edd_mc->MACAddr.MacAdr, acp_mc->mac_addr, sizeof (edd_mc->MACAddr.MacAdr)); /* .MacAdr added, MACAddr may be padded (AP01453882) */

	acp_ll_request (channel->lower, lower, EDD_SRV_MULTICAST, channel);

	return;
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_MULTICAST
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_multicast_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LOG_FCT("acp_op_multicast_done")
	LSA_UINT16  rsp;

	rsp = ACP_LL_GET_RESPONSE (lower);

	ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rsp(%d)"
		, rsp
		);

	acp_ll_free_rqb (channel->lower, lower);
	acp_callback (channel, acp_rsp_from_edd_rsp (rsp));
}


/*------------------------------------------------------------------------------
//	handle Opcode ACP_OPC_OVERLOAD_IND_RES_PROVIDE
//----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_overload_provide (
	ACP_CHANNEL_PTR  channel
	)
{
	ACP_LOG_FCT("acp_op_overload_provide")
	ACP_UPPER_OVERLOAD_PTR_TYPE  overload;

	if (is_not_null (channel->overload_ind)) {
		acp_callback_async (channel, ACP_ERR_SEQUENCE, & channel->upper);
		return;
	}

	overload = & channel->upper->args->channel.overload;

	channel->overload_ind = channel->upper;
	channel->upper = LSA_NULL;

	if (! channel->lower->overload_edd_running) {
		acp_callback_async (channel, ACP_ERR_ALLOC_LOWER_RQB, & channel->overload_ind);
		return;
	}

	acp_overload_check (channel->lower, 0, 0);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_overload_check (
	ACP_LOWER_PTR  channel_lower,
	LSA_UINT32  add_rx,
	LSA_UINT32  add_tx
	)
{
	ACP_LOG_FCT("acp_overload_check")
	LSA_UINT16  i;

	ACP_UPPER_TRACE_04 (channel_lower->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "lower.rx(%u) add_rx(%u)   lower.tx(%u)  add_tx(%d)"
		, channel_lower->overload.rx_counter, add_rx
		, channel_lower->overload.tx_counter, add_tx
		);

		/* something to add ? */
	if (add_rx > 0) {
		channel_lower->overload.rx_counter += add_rx;
	}
	if (add_tx > 0) {
		channel_lower->overload.tx_counter += add_tx;
	}

		/* something to indicate ? */
	for (i = 0; i < ACP_CFG_MAX_CHANNELS; ++i) {
		ACP_CHANNEL_PTR  channel;
		ACP_UPPER_OVERLOAD_PTR_TYPE  upper_overload;
		LSA_BOOL  do_callback;

		channel = & acp_channels[i];
		if (channel->state != ACP_CHANNEL_STATE_READY) {
			continue;
		}
		if (! are_equal (channel->lower, channel_lower)) {
			continue;
		}
		if (is_null (channel->overload_ind)) {
			continue;
		}

		upper_overload = & channel->overload_ind->args->channel.overload;
		do_callback = LSA_FALSE;

		if (upper_overload->rx_counter != channel_lower->overload.rx_counter) {
			upper_overload->rx_counter = channel_lower->overload.rx_counter;

			do_callback = LSA_TRUE;
		}
		if (upper_overload->tx_counter != channel_lower->overload.tx_counter) {
			upper_overload->tx_counter = channel_lower->overload.tx_counter;

			do_callback = LSA_TRUE;
		}

		if (do_callback) {
			ACP_RQB_SET_OPCODE (channel->overload_ind, ACP_OPC_OVERLOAD_INDICATION);
			acp_callback_async (channel, ACP_OK, & channel->overload_ind);
		}
	}
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
