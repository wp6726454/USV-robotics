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
/*  F i l e               &F: cm_acp2.c                                 :F&  */
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
/*  - frame add                                                              */
/*  - frame control                                                          */
/*  - frame remove                                                           */
/*  - frame get apdu status                                                  */
/*  - alarm add                                                              */
/*  - alarm remove                                                           */
/*  - done-callbacks                                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  21
#define CM_MODULE_ID       21

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*===========================================================================*/
/*=== FRAME ADD =============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_add")
	LSA_BOOL  is_adv, is_sysred;

	CM_ASSERT(io_cr->cm.acp_id == ACP_INVALID_ACP_ID);

	/***/

	is_adv = (io_cr->cm.backlink->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) ? LSA_TRUE : LSA_FALSE;
	is_sysred = (io_cr->cm.backlink->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED) ? LSA_TRUE : LSA_FALSE;

	/***/

	CM_ACP_TRACE_06(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "FrameAdd: frame_id(0x%04x) is_cons(%u) is_multi(%u) is_adv(%u) is_sysred(%u) len(%u)"
		, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast, is_adv, is_sysred
		, io_cr->data_length
		);

	CM_ACP_TRACE_07(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  frame_id(0x%04x) mac(%02x-%02x-%02x-%02x-%02x-%02x)"
		, io_cr->frame_id
		, io_cr->multicast_mac.mac[0], io_cr->multicast_mac.mac[1], io_cr->multicast_mac.mac[2]
		, io_cr->multicast_mac.mac[3], io_cr->multicast_mac.mac[4], io_cr->multicast_mac.mac[5]
		);

	CM_ACP_TRACE_08(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  frame_id(0x%04x) send_clock/rr/phase %u/%u/%u adaption to %u/%u/%u, seq(%u)"
		, io_cr->frame_id
		, io_cr->send_clock, io_cr->reduction_ratio, io_cr->phase
		, io_cr->cm.adpt.send_clock, io_cr->cm.adpt.reduction_ratio, io_cr->cm.adpt.phase
		, io_cr->cm.adpt.sequence
		);

	if (io_cr->cm.is_consumer)
	{
		ACP_UPPER_CONSUMER_ADD_PTR_TYPE  cons_add = & acp->args->cons.add;

		cons_add->cons_id = ACP_INVALID_ACP_ID; /* out, id of consumer */

		if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1_UDP )
		{
			cons_add->properties = EDD_CSRT_CONS_PROP_RTCLASS_UDP;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3 )
		{
			if( is_adv )
			{
				cons_add->properties = EDD_CSRT_CONS_PROP_RTCLASS_3;
			}
			else
			{
				cons_add->properties = EDD_CSRT_CONS_PROP_RTCLASS_3 | EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY;
			}
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_2 )
		{
			cons_add->properties = EDD_CSRT_CONS_PROP_RTCLASS_2;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1 )
		{
			cons_add->properties = EDD_CSRT_CONS_PROP_RTCLASS_1;
		}
		else
		{
			CM_FATAL1 (io_cr->iocr_properties);
			return;
		}

		if (is_sysred)
		{
			cons_add->properties |= EDD_CSRT_CONS_PROP_SYSRED;
		}

		cons_add->frame_id					= io_cr->frame_id;

		if (io_cr->cm.subframe_data != 0) /* is-dfp */
		{
			cons_add->properties			|= EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP;
			cons_add->length				= ((io_cr->cm.subframe_data >> 8) & 0xFF);
			cons_add->partial_length		= 0;
			cons_add->partial_offset		= 0;
		}
		else if( io_cr->cm.partial_length == io_cr->data_length  &&  io_cr->cm.partial_offset == 0 )
		{
			cons_add->length				= io_cr->data_length;
			cons_add->partial_length		= 0;
			cons_add->partial_offset		= 0;
		}
		else
		{
			cons_add->length				= io_cr->data_length;
			cons_add->partial_length		= io_cr->cm.partial_length;
			cons_add->partial_offset		= io_cr->cm.partial_offset;
		}

		cons_add->data_hold_factor			= io_cr->cm.adpt.data_hold_factor;

		cons_add->usr_id					= (LSA_UINT32)(io_cr);

		/* fixed cons iodata, same in add and control */
		cons_add->ioparam.app_group				= 0;
		cons_add->ioparam.clear_on_miss			= LSA_FALSE;

		/* set cons iodata with control */
		cons_add->reduction_ratio				= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		cons_add->phase							= EDD_CYCLE_PHASE_UNDEFINED;
		cons_add->ioparam.properties			= EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED;
		cons_add->ioparam.data_offset			= EDD_DATAOFFSET_UNDEFINED;
		cons_add->ioparam.cr_number				= EDD_CRNUMBER_UNDEFINED;
		cons_add->ioparam.forward_mode			= EDD_FORWARDMODE_UNDEFINED;
		cons_add->ioparam.ct_outdatastart		= 0;
		cons_add->ioparam.sf_position			= EDD_SFPOSITION_UNDEFINED;
		cons_add->ioparam.sf_offset				= EDD_SFOFFSET_UNDEFINED;

		/* init cnf */
		cons_add->ioparam.apdu_status_offset	= EDD_DATAOFFSET_INVALID;

		/***/

		cm_acp_request_lower(channel, ACP_OPC_CONSUMER_ADD, io_cr, acp);
	}
	else
	{
		ACP_UPPER_PROVIDER_ADD_PTR_TYPE  prov_add = & acp->args->prov.add;

		prov_add->prov_id = ACP_INVALID_ACP_ID; /* out, id of provider */

		if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1_UDP )
		{
			prov_add->properties = EDD_CSRT_PROV_PROP_RTCLASS_UDP;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3 )
		{
			if( is_adv )
			{
				prov_add->properties = EDD_CSRT_PROV_PROP_RTCLASS_3;
			}
			else
			{
				prov_add->properties = EDD_CSRT_PROV_PROP_RTCLASS_3 | EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY;
			}
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_2 )
		{
			prov_add->properties = EDD_CSRT_PROV_PROP_RTCLASS_2;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1 )
		{
			prov_add->properties = EDD_CSRT_PROV_PROP_RTCLASS_1;
		}
		else
		{
			CM_FATAL1 (io_cr->iocr_properties);
			return;
		}

		if (is_sysred)
		{
			prov_add->properties |= EDD_CSRT_PROV_PROP_SYSRED;
		}

		prov_add->frame_id				= io_cr->frame_id;

		if (io_cr->cm.subframe_data != 0) /* is-dfp */
		{
			prov_add->properties		|= EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP;
			prov_add->length			= ((io_cr->cm.subframe_data >> 8) & 0xFF);
		}
		else
		{
			prov_add->length			= io_cr->cm.partial_length; /* not: data_length */
		}

		if (io_cr->cm.is_multicast)
		{
			if ((prov_add->properties & EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP) == 0
			&&  (prov_add->properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY) == 0)
			{
				prov_add->properties	|= EDD_CSRT_PROV_PROP_MODE_MC_CR;
			}
		}

		prov_add->usr_id				= (LSA_UINT32)(io_cr);

		/* fixed prov iodata, same in add and control */
		prov_add->ioparam.app_group		= 0;

		/* set prov iodata with control */
		prov_add->reduction_ratio		= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		prov_add->phase					= EDD_CYCLE_PHASE_UNDEFINED;
		prov_add->sequence				= EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
		prov_add->ioparam.properties	= EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED;
		prov_add->ioparam.data_offset	= EDD_DATAOFFSET_UNDEFINED;
		prov_add->ioparam.cr_number		= EDD_CRNUMBER_UNDEFINED;
		prov_add->ioparam.sf_position	= EDD_SFPOSITION_UNDEFINED;
		prov_add->ioparam.sf_offset		= EDD_SFOFFSET_UNDEFINED;

		/* init cnf */
		prov_add->ioparam.cw_offset		= EDD_DATAOFFSET_INVALID;

		/***/

		cm_acp_request_lower(channel, ACP_OPC_PROVIDER_ADD, io_cr, acp);
	}
}

/*===========================================================================*/
/*=== FRAME ADD DONE ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_add_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_add_done")

	CM_ASSERT(is_not_null (io_cr));
	CM_ASSERT(io_cr->cm.cr_state == CM_AR_CR_PASSIVE
		|| io_cr->cm.cr_state == CM_AR_MCONS_ADD
		);

	/***/

	if (CM_ACP_GET_RESPONSE(acp) != ACP_OK) {

		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "FrameAddDone: frame_id(0x%04x) add failed, acp-rsp(%u)"
			, io_cr->frame_id, CM_ACP_GET_RESPONSE(acp)
			);

		/*
		 * note: iocr-add can fail because of things CM cannot check:
		 *       wrong edd-data, phase-overload, sequence-not-unique, ...
		 */

		io_cr->cm.acp_id = ACP_INVALID_ACP_ID; /* not added */

		io_cr->cm.cr_state = CM_AR_CR_ERROR; /* sanity */
	}
	else { /* add succeeded */

		if (io_cr->cm.is_consumer) {

			io_cr->cm.acp_id        = acp->args->cons.add.cons_id;

			io_cr->cm.edd_as_offset = acp->args->cons.add.ioparam.apdu_status_offset;

			io_cr->cm.edd_cw_offset = EDD_DATAOFFSET_INVALID;
			io_cr->cm.edd_ds_offset	= EDD_DATAOFFSET_INVALID;
		}
		else {

			io_cr->cm.acp_id        = acp->args->prov.add.prov_id;

			io_cr->cm.edd_as_offset = EDD_DATAOFFSET_INVALID;

			io_cr->cm.edd_cw_offset = acp->args->prov.add.ioparam.cw_offset;
			io_cr->cm.edd_ds_offset	= acp->args->prov.add.ioparam.data_status_offset;
		}
	}

	CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "FrameAddDone: frame_id(0x%04x) is_cons(%u) is_multi(%u) acp_id(%u) cr_state(%u)"
		, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast
		, io_cr->cm.acp_id, io_cr->cm.cr_state
		);

	/***/

	if( io_cr->cm.is_multicast ) {

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("ARCQ-ERROR: change arcq here")
#endif /* CM_MESSAGE */

		if( io_cr->cm.is_consumer ) {

			io_cr->cm.cr_state = CM_AR_MCONS_ADD; /* see cm_arsm_mcons_add_done() */

#if CM_CFG_USE_NARE
			cm_arsm_mcons_add_done(channel, io_cr, acp);
#else
			LSA_UNUSED_ARG(channel);
			CM_FATAL(); /* should not happen */
#endif
		}
		else {

			cm_arcq_step_next (io_cr->cm.backlink, io_cr, acp);
		}
	}
	else /* unicast */
	{
		cm_arcq_step_next (io_cr->cm.backlink, io_cr, acp);
	}
}

/*===========================================================================*/
/*=== FRAME CONTROL =========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_control(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_UINT16  acp_mode,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_control")
	LSA_OPCODE_TYPE  opcode;
	LSA_BOOL  is_adv  = (io_cr->cm.backlink->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) ? LSA_TRUE : LSA_FALSE;
	LSA_BOOL  is_rtc3 = ((io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3) ? LSA_TRUE : LSA_FALSE;

	/***/

	if (io_cr->cm.is_consumer)
	{
		ACP_UPPER_CONSUMER_CONTROL_PTR_TYPE  cons_ctrl = & acp->args->cons.control;

		CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ConsumerControl.req: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) acp_mode(0x%x)"
			, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, acp_mode
			);

		opcode = ACP_OPC_CONSUMER_CONTROL;

		cons_ctrl->cons_id = io_cr->cm.acp_id;
		cons_ctrl->mode = acp_mode;

		/* fixed cons iodata, same in add and control */
		cons_ctrl->ioparam.app_group			= 0;
		cons_ctrl->ioparam.clear_on_miss		= LSA_FALSE;

		/* set cons iodata with control */
		cons_ctrl->reduction_ratio				= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		cons_ctrl->phase						= EDD_CYCLE_PHASE_UNDEFINED;
		cons_ctrl->ioparam.properties			= EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED;
		cons_ctrl->ioparam.data_offset			= EDD_DATAOFFSET_UNDEFINED;
		cons_ctrl->ioparam.cr_number			= EDD_CRNUMBER_UNDEFINED;
		cons_ctrl->ioparam.forward_mode			= EDD_FORWARDMODE_UNDEFINED;
		cons_ctrl->ioparam.ct_outdatastart		= 0;
		cons_ctrl->ioparam.sf_position			= EDD_SFPOSITION_UNDEFINED;
		cons_ctrl->ioparam.sf_offset			= EDD_SFOFFSET_UNDEFINED;

		if (acp_mode & ACP_CONSUMER_SET_ACTIVE)
		{
			/* set with control */
			cons_ctrl->reduction_ratio				= io_cr->cm.adpt.reduction_ratio;
			cons_ctrl->phase						= io_cr->cm.adpt.phase;
			cons_ctrl->ioparam.properties			= io_cr->cm.edd_properties;
			cons_ctrl->ioparam.data_offset			= io_cr->cm.edd_data_offset;
			cons_ctrl->ioparam.cr_number			= io_cr->cm.edd_cr_number;
			cons_ctrl->ioparam.forward_mode			= io_cr->cm.edd_forward_mode;
			cons_ctrl->ioparam.ct_outdatastart		= io_cr->cm.edd_ct_outdatastart;

			if (io_cr->cm.subframe_data != 0) /* is-dfp */
			{
				cons_ctrl->ioparam.sf_position		= io_cr->cm.subframe_data & 0x7F;
				cons_ctrl->ioparam.sf_offset		= io_cr->cm.subframe_offset;
			}

			/* set src-ip-addr */
			if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1_UDP )
			{
				CM_MEMSET (cons_ctrl->src_mac, 0, sizeof(cons_ctrl->src_mac));
				CM_MEMSET (cons_ctrl->src_ip,  0, sizeof(cons_ctrl->src_ip));

				CM_FATAL(); /* UDP not yet supported (AP01195665) , see cm_ar_graph_insert_io_cr() */
			}
			/* or src-mac-addr */
			else
			{
				CM_MAC_PTR_TYPE  mac;
				if( io_cr->cm.is_multicast )
				{
#if CM_CFG_USE_NARE
					mac = &io_cr->cm.mc_addr;
#else
					CM_FATAL();
					return;
#endif
				}
				else
				{
					mac = &io_cr->cm.backlink->ar.eth_mac;
				}
				CM_MEMCPY (cons_ctrl->src_mac, mac->mac, sizeof(cons_ctrl->src_mac));
				CM_MEMSET (cons_ctrl->src_ip,  0, sizeof(cons_ctrl->src_ip));
			}
		}

		if (acp_mode & ACP_CONSUMER_SET_AUTOSTOP_ENABLE)
		{
			/* only a mode-bit */
		}
	}
	else /* provider */
	{
		ACP_UPPER_PROVIDER_CONTROL_PTR_TYPE  prov_ctrl = & acp->args->prov.control;
		CM_MAC_PTR_TYPE  mac;

		if( io_cr->cm.is_multicast )
		{
			mac = &io_cr->multicast_mac;
		}
		else
		{
			if (is_rtc3)
			{
				if (is_adv)
				{
					mac = &io_cr->multicast_mac;
				}
				else
				{
					mac = &io_cr->cm.backlink->ar.eth_mac;
				}
			}
			else
			{
				mac = &io_cr->cm.backlink->ar.eth_mac;
			}
		}

		opcode = ACP_OPC_PROVIDER_CONTROL;

		CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ProviderControl.req: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) acp_mode(0x%x)"
			, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, acp_mode
			);

		prov_ctrl->prov_id = io_cr->cm.acp_id;
		prov_ctrl->mode = acp_mode;

		if (acp_mode & ACP_PROVIDER_SET_AUTOSTOP)
		{
			if (acp_mode & ACP_PROVIDER_SET_ACTIVE)
			{
				prov_ctrl->autostop_cons_id = io_cr->cm.backlink->ar_com.autostop_cons_id;
			}
			else if (acp_mode & ACP_PROVIDER_SET_PASSIVE)
			{
				prov_ctrl->autostop_cons_id = ACP_INVALID_ACP_ID;
			}
			else
			{
				CM_FATAL();
			}
		}

		prov_ctrl->frame_id				= io_cr->frame_id;
		CM_MEMCPY (prov_ctrl->dst_mac	,  mac->mac, sizeof(prov_ctrl->dst_mac));
		CM_MEMSET (prov_ctrl->dst_ip	, 0, sizeof(prov_ctrl->dst_ip));

		/* fixed prov iodata, same in add and control */
		prov_ctrl->ioparam.app_group	= 0;

		/* set prov iodata with control */
		prov_ctrl->reduction_ratio		= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		prov_ctrl->phase				= EDD_CYCLE_PHASE_UNDEFINED;
		prov_ctrl->sequence				= EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
		prov_ctrl->ioparam.properties	= EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED;
		prov_ctrl->ioparam.data_offset	= EDD_DATAOFFSET_UNDEFINED;
		prov_ctrl->ioparam.cr_number	= EDD_CRNUMBER_UNDEFINED;
		prov_ctrl->ioparam.sf_position	= EDD_SFPOSITION_UNDEFINED;
		prov_ctrl->ioparam.sf_offset	= EDD_SFOFFSET_UNDEFINED;

		if (acp_mode & (ACP_PROVIDER_SET_IOPARAM | ACP_PROVIDER_SET_ACTIVE | ACP_PROVIDER_SET_ACTIVE_AUX))
		{
			prov_ctrl->ioparam.properties	= io_cr->cm.edd_properties;
			prov_ctrl->ioparam.data_offset	= io_cr->cm.edd_data_offset;
			prov_ctrl->ioparam.cr_number	= io_cr->cm.edd_cr_number;

			if (io_cr->cm.subframe_data != 0) /* is-dfp */
			{
				prov_ctrl->ioparam.sf_position	= io_cr->cm.subframe_data & 0x7F;
				prov_ctrl->ioparam.sf_offset	= io_cr->cm.subframe_offset;
			}

			prov_ctrl->reduction_ratio		= io_cr->cm.adpt.reduction_ratio;
			prov_ctrl->phase				= io_cr->cm.adpt.phase;
			prov_ctrl->sequence				= io_cr->cm.adpt.sequence;
		}
	}

	cm_acp_request_lower(channel, opcode, io_cr, acp);
}

/*===========================================================================*/
/*=== FRAME CONTROL DONE ====================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_control_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_control_done")

	CM_ASSERT(is_not_null (io_cr));
	LSA_UNUSED_ARG (channel);

	/***/

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK )
	{
		CM_ACP_TRACE_05(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "io_cr(%08x) frame_id(0x%04x) acp_id(%u) acp_mode(0x%x), acp-rsp(%u)"
			, io_cr, io_cr->frame_id, io_cr->cm.acp_id
			, io_cr->cm.is_consumer ? acp->args->cons.control.mode : acp->args->prov.control.mode
			, CM_ACP_GET_RESPONSE(acp)
			);

		/* since we can set data with control (e.g. edd_data_offset) control can fail. if so, start AR abort */
		if (io_cr->cm.is_multicast && io_cr->cm.is_consumer)
		{
#if CM_CFG_USE_NARE
			LSA_UINT16 acp_mode = acp->args->cons.control.mode;

				if ((acp_mode & ACP_CONSUMER_SET_PASSIVE) != 0)
				{
					cm_arsm_mcons_passivate_done (channel, io_cr, acp);
				}
				else if ((acp_mode & ACP_CONSUMER_SET_ACTIVE) != 0)
				{
					cm_arsm_mcons_control_done (channel, io_cr, acp);
				}
				else
				{
					CM_FATAL1 (acp_mode);
				}
#else
				CM_FATAL(); /* should not happen */
#endif
		}
		else
		{
			io_cr->cm.cr_state = CM_AR_CR_ERROR;
			if (io_cr->cm.is_consumer)
			{
				cm_ar_com_down_debug_data_set (io_cr->cm.backlink, & acp->args->cons.control.debug_data);
			}
			else
			{
				cm_ar_com_down_debug_data_set (io_cr->cm.backlink, & acp->args->prov.control.debug_data);
			}
			cm_arcq_step_next (io_cr->cm.backlink, io_cr, acp);
		}
	}
	else /* control succeeded */
	{
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("warning: ARCQ, could refactor the if-s now that the bits are independent!")
#endif /* CM_MESSAGE */

		if( io_cr->cm.is_consumer )
		{
			LSA_UINT16 acp_mode = acp->args->cons.control.mode;

			CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ConsumerControl.cnf: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) acp_mode(0x%x)"
				, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, acp_mode
				);

			if ((acp_mode & ACP_CONSUMER_SET_ACTIVE) != 0)
			{
				if (io_cr->cm.backlink->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED)
				{
					io_cr->cm.cons_data_status = CM_EVENT_APDU_STATUS_INITIAL_SYSRED_VALUE; /* see ACP_STATE_CONSUMER_SYSRED_DEFAULT */
				}
				else
				{
					io_cr->cm.cons_data_status = CM_EVENT_APDU_STATUS_INITIAL_VALUE; /* see ACP_STATE_CONSUMER_DEFAULT */
				}
			}

			if( io_cr->cm.is_multicast ) {

#if CM_CFG_USE_NARE
				if ((acp_mode & ACP_CONSUMER_SET_PASSIVE) != 0)
				{
					cm_arsm_mcons_passivate_done (channel, io_cr, acp);
				}
				else if ((acp_mode & ACP_CONSUMER_SET_ACTIVE) != 0)
				{
					cm_arsm_mcons_control_done (channel, io_cr, acp);
				}
				else
				{
					CM_FATAL1 (acp_mode);
				}
#else
				LSA_UNUSED_ARG (acp_mode);
				CM_FATAL(); /* should not happen */
#endif
			}
			else
			{
				cm_arcq_step_next (io_cr->cm.backlink, io_cr, acp);
			}
		}
		else /* provider */
		{
			LSA_UINT16 acp_mode = acp->args->prov.control.mode;

			LSA_UNUSED_ARG (acp_mode);
			CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ProviderControl.cnf: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) acp_mode(0x%x)"
				, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, acp_mode
				);

			cm_arcq_step_next (io_cr->cm.backlink, io_cr, acp);
		}
	}
}

/*===========================================================================*/
/*=== FRAME REMOVE ==========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_remove")
	LSA_OPCODE_TYPE  opcode;

	CM_ASSERT(io_cr->cm.cr_state == CM_AR_CR_IDLE
		|| io_cr->cm.cr_state == CM_AR_MCONS_REMOVE
		);

	/***/

	/***/

	if (io_cr->cm.acp_id == ACP_INVALID_ACP_ID) {

		CM_FATAL(); /* a bug in caller */
	}

	if (io_cr->cm.is_consumer) {

		CM_ACP_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ConsumerRemove: frame_id(0x%04x) is_cons(%u) is_multi(%u) acp_id(%u)"
			, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast, io_cr->cm.acp_id
			);

		opcode = ACP_OPC_CONSUMER_REMOVE;

		acp->args->cons.remove.cons_id = io_cr->cm.acp_id;
	}
	else {

		CM_ACP_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ProviderRemove: frame_id(0x%04x) is_cons(%u) is_multi(%u) acp_id(%u)"
			, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast, io_cr->cm.acp_id
			);

		opcode = ACP_OPC_PROVIDER_REMOVE;

		acp->args->prov.remove.prov_id = io_cr->cm.acp_id;
	}

	/***/

	io_cr->cm.acp_id = ACP_INVALID_ACP_ID; /* set to invalid here, not in cm_acp_frame_remove_done() */

	cm_acp_request_lower(channel, opcode, io_cr, acp);
}

/*===========================================================================*/
/*=== FRAME REMOVE DONE =====================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_remove_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_remove_done")

	CM_ASSERT(is_not_null(io_cr));
	CM_ASSERT(io_cr->cm.cr_state == CM_AR_CR_IDLE
		|| io_cr->cm.cr_state == CM_AR_MCONS_REMOVE
		);
	CM_ASSERT(io_cr->cm.acp_id == ACP_INVALID_ACP_ID); /* see cm_acp_frame_remove() */
	LSA_UNUSED_ARG (channel);

	/***/

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK ) {

		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "FrameRemoveDone: frame_id(0x%04x) acp-rsp(%u)"
			, io_cr->frame_id, CM_ACP_GET_RESPONSE(acp)
			);

		CM_FATAL(); /* could add, so why not remove? */
	}
	else { /* remove succeeded */

		CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "FrameRemoveDone: frame_id(0x%04x)"
			, io_cr->frame_id
			);

		/***/

		if( io_cr->cm.is_consumer && io_cr->cm.is_multicast ) {

#if CM_CFG_USE_NARE
			cm_arsm_mcons_remove_done(channel, io_cr, acp);
#else
			CM_FATAL(); /* should not happen */
#endif
		}
		else {

			io_cr->cm.cr_state = CM_AR_CR_IDLE;

			cm_arcq_step_next (io_cr->cm.backlink, io_cr, acp);
		}
	}
}

/*===========================================================================*/
/*=== ALARM ADD =============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_alarm_add")
	LSA_OPCODE_TYPE  opcode;

	CM_ACP_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AlarmAdd: alarm_cr(%08X) local_alarm_ref(%u) remote_ref(%u)"
		, alarm_cr, alarm_cr->local_alarm_ref, alarm_cr->remote_alarm_ref
		);

	acp->args->alarm.open.alarm_ref = alarm_cr->local_alarm_ref;
	acp->args->alarm.open.dst_alarm_ref = alarm_cr->remote_alarm_ref;
	acp->args->alarm.open.properties = ACP_AL_PROP_USE_GATEWAY_NO;

	CM_MEMCPY (acp->args->alarm.open.dst_mac, & alarm_cr->cm.backlink->ar.eth_mac, sizeof(acp->args->alarm.open.dst_mac));

	if( (alarm_cr->alcr_properties & CM_ALCR_PROP_TRANSPORT_MASK) == CM_ALCR_PROP_TRANSPORT_UDP ) {
		opcode = ACP_OPC_UDP_ALARM_OPEN;
		CM_MEMCPY (acp->args->alarm.open.dst_ip, &alarm_cr->cm.backlink->ar.host_ip, sizeof(acp->args->alarm.open.dst_ip));
	}
	else {
		opcode = ACP_OPC_ALARM_OPEN;
		CM_MEMSET (acp->args->alarm.open.dst_ip, 0, sizeof(acp->args->alarm.open.dst_ip));
	}

	acp->args->alarm.open.vlan_id      = 0; /* see cm_ar_graph_insert_alarm_cr(), TIA 208236 */
	acp->args->alarm.open.timeout100ms = alarm_cr->timeout100ms;
	acp->args->alarm.open.retry_count  = alarm_cr->retries;

	acp->args->alarm.open.usr_id = (LSA_UINT32)alarm_cr;

	cm_acp_request_lower(channel, opcode, alarm_cr, acp);
}

/*===========================================================================*/
/*=== ALARM ADD DONE ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_add_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_alarm_add_done")
	LSA_UNUSED_ARG (channel);

	CM_ASSERT (is_not_null (alarm_cr));

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK )
	{
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "AlarmAddDone: alarm_cr(%08X) acp-rsp(%u)"
			, alarm_cr, CM_ACP_GET_RESPONSE(acp)
			);

		/* note: should not happen, because CM selects the alarm-ref */

		alarm_cr->cm.cr_state = CM_AR_CR_ERROR; /* see remove */
	}
	else
	{
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "AlarmAddDone: alarm_cr(%08X) acp-rsp(%u)"
			, alarm_cr, CM_ACP_GET_RESPONSE(acp)
			);

		CM_ASSERT (alarm_cr->cm.cr_state == CM_AR_CR_UP);
	}

	/***/

	cm_arcq_step_next (alarm_cr->cm.backlink, alarm_cr, acp);
}

/*===========================================================================*/
/*=== ALARM REMOVE ==========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_alarm_remove")
	LSA_UINT16  vendor_id, device_id;
	LSA_UINT32  pnio_version;

	CM_ASSERT(alarm_cr->cm.cr_state == CM_AR_CR_IDLE);

	vendor_id = device_id = 0;

#if CM_CFG_MAX_CLIENTS
	if (alarm_cr->cm.backlink->ar_com.path_type == CM_PATH_TYPE_CLIENT)
	{
		vendor_id = channel->usr.cl.vendor_id;
		device_id = channel->usr.cl.device_id;
	}
#endif
#if CM_CFG_MAX_SERVERS
	if (alarm_cr->cm.backlink->ar_com.path_type == CM_PATH_TYPE_SERVER)
	{
		vendor_id = alarm_cr->cm.backlink->cm.sv.dev->vendor_id;
		device_id = alarm_cr->cm.backlink->cm.sv.dev->device_id;
	}
#endif

	pnio_version =
		( ((PNIO_VERSION		& 0xF) << 28) 
		| ((PNIO_DISTRIBUTION	& 0xF) << 24)
		| ((PNIO_PROJECT_NUMBER	& 0xF) << 20)
		| ((PNIO_INCREMENT		& 0xF) << 16)
		| (((PNIO_INTEGRATION_COUNTER * 100 + PNIO_GEN_COUNTER) & 0xFFFF) << 0)
		);

	/***/

	CM_ACP_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AlarmRemove: alarm_cr(%08X) local_alarm_ref(%u) reason(%u)  vendor_id(0x%x) device_id(0x%x) pnio_version(0x%x)"
		, alarm_cr, alarm_cr->local_alarm_ref, alarm_cr->cm.backlink->ar_com.down
		, vendor_id, device_id, pnio_version
		);

	acp->args->alarm.close.alarm_ref = alarm_cr->local_alarm_ref;
	acp->args->alarm.close.pnio_status_reason = alarm_cr->cm.backlink->ar_com.down;

	{
		acp->args->alarm.close.error_info.vendor_id			= vendor_id;
		acp->args->alarm.close.error_info.device_id			= device_id;
		acp->args->alarm.close.error_info.pnio_version		= pnio_version;
		CM_MEMCPY (acp->args->alarm.close.error_info.debug_data
			, alarm_cr->cm.backlink->ar_com.down_debug_data, sizeof(acp->args->alarm.close.error_info.debug_data)
			);
	}

	cm_acp_request_lower(channel, ACP_OPC_ALARM_CLOSE, alarm_cr, acp);
}

/*===========================================================================*/
/*=== ALARM REMOVE DONE =====================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_remove_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_alarm_remove_done")

	LSA_UNUSED_ARG (channel);

	CM_ASSERT(is_not_null (alarm_cr));
	CM_ASSERT(alarm_cr->cm.cr_state == CM_AR_CR_IDLE);

	/***/

	CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AlarmRemoveDone: alarm_cr(%08X) acp-rsp(%u)"
		, alarm_cr, CM_ACP_GET_RESPONSE(acp)
		);

	/***/

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK )
	{
		CM_FATAL(); /* could add, so why not remove? */
	}
	else /* remove succeeded */
	{
		/*not!!! alarm_cr->alarm_ref = ACP_INVALID_ALARM_REF;*/
		cm_arcq_step_next (alarm_cr->cm.backlink, alarm_cr, acp);
	}
}

/*===========================================================================*/
/*=== REGISTER / UNREGISTER MULTICAST-MAC ===================================*/
/*===========================================================================*/

#if CM_CFG_USE_NARE

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_multicast_mac(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_MAC_PTR_TYPE  mmac,
	LSA_UINT8 mode, /* ACP_MULTICAST_ENABLE or ACP_MULTICAST_DISABLE */
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_multicast")

	CM_ACP_TRACE_08(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) mmac(%02x-%02x-%02x-%02x-%02x-%02x) mode(%u)"
		, io_cr
		, mmac->mac[0], mmac->mac[1], mmac->mac[2]
		, mmac->mac[3], mmac->mac[4], mmac->mac[5]
		, mode
		);

	acp->args->channel.multicast.mode = mode;

	CM_MEMCPY(acp->args->channel.multicast.mac_addr, mmac->mac, sizeof(acp->args->channel.multicast.mac_addr));

	cm_acp_request_lower(channel, ACP_OPC_MULTICAST, io_cr, acp);
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_multicast_mac_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_multicast_done")
	LSA_UINT16 acp_rsp;
	LSA_UINT8 mode;

	CM_ASSERT(is_not_null(iocr));

	CM_ASSERT(iocr->cm.is_consumer && iocr->cm.is_multicast);

	/***/

	acp_rsp = CM_ACP_GET_RESPONSE(acp);

	mode = acp->args->channel.multicast.mode;

	CM_ACP_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) mode(%u) acp-rsp(%u)"
		, iocr, mode, acp_rsp
		);

	/***/

	switch( mode ) {

	case ACP_MULTICAST_ENABLE:

		if( acp_rsp != ACP_OK ) {

			CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "iocr(%08X) could not mmac-register, acp-rsp(%u)"
				, iocr, acp_rsp
				);
		}

		cm_arsm_mcons_mmac_register_done(channel, iocr, acp);
		break;

	case ACP_MULTICAST_DISABLE:

		if( acp_rsp != ACP_OK ) {

			CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "iocr(%08X) could not mmac-unregister, acp-rsp(%u)"
				, iocr, acp_rsp
				);

			CM_FATAL(); /* could register, so why not unregister? */
		}
		else {

			cm_arsm_mcons_mmac_unregister_done(channel, iocr, acp);
		}
		break;

	default:
		CM_FATAL();
		break;
	}
}

#endif

/*===========================================================================*/
/*=== FRAME GET APDU STATUS =================================================*/
/*===========================================================================*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_acp_frame_get_apdu_status (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_VOID_PTR_TYPE  user_id
) {
	CM_LOG_FCT ("cm_acp_frame_get_apdu_status")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	if( iocr->cm.acp_id == ACP_INVALID_ACP_ID ) {
		CM_ACP_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "!acp-id (not yet added or already removed)");
		return CM_ERR_SESSION;
	}

	cm_acp_alloc_rqb (channel, & acp);
	if (is_null (acp)) {
		CM_ACP_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!acp-rqb");
		return CM_ERR_ALLOC_LOWER;
	}

	CM_ACP_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) frame_id(0x%04x) acp_id(%u) user_id(%08x)"
		, iocr, iocr->frame_id, iocr->cm.acp_id, user_id
		);

	/***/

	acp->args->channel.apdu_status.acp_id = iocr->cm.acp_id;

	cm_acp_request_lower(channel, ACP_OPC_GET_APDU_STATUS, user_id, acp);
	return CM_OK_PENDING;
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_get_apdu_status_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  user_id,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_get_apdu_status_done")
	LSA_UINT16	cycle_counter;
	LSA_UINT8	data_status;
	LSA_UINT8	transfer_status;

	if (CM_ACP_GET_RESPONSE(acp) != ACP_OK)
	{
		cycle_counter	= 0;
		data_status		= 0;
		transfer_status	= 0;

		CM_ACP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "acp_id(%u)  rsp(%u) != ACP_OK"
			, acp->args->channel.apdu_status.acp_id
			, CM_ACP_GET_RESPONSE(acp)
			);
	}
	else
	{
		if( acp->args->channel.apdu_status.present ) {

			cycle_counter	= acp->args->channel.apdu_status.cycle_count;
			data_status		= acp->args->channel.apdu_status.data_status;
			transfer_status	= acp->args->channel.apdu_status.transfer_status;
		}
		else { /* returned values are undefined */

			cycle_counter	= 0;
			data_status		= 0;
			transfer_status	= 0;
		}

		CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "acp_id(%u)  present(%u) cycle_counter(0x%x) data_status(0x%x) transfer_status(0x%x)"
			, acp->args->channel.apdu_status.acp_id
			, acp->args->channel.apdu_status.present
			, cycle_counter, data_status, transfer_status
			);
	}

	cm_acp_free_rqb (channel, &acp);

	/***/

	switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_frame_get_apdu_status_done (user_id, cycle_counter, data_status, transfer_status);
		break;
#endif

	default:
		CM_FATAL1 (channel->detailptr->path_type); /* a bug */
		LSA_UNUSED_ARG (user_id);
		LSA_UNUSED_ARG (transfer_status);
		LSA_UNUSED_ARG (data_status);
		LSA_UNUSED_ARG (cycle_counter);
		break;
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
