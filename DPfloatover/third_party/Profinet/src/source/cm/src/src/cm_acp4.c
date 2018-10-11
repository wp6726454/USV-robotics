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
/*  F i l e               &F: cm_acp4.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  - cycle drop                                                             */
/*  - frame indications                                                      */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  23
#define CM_MODULE_ID       23

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*===========================================================================*/
/*=== OVERLOAD ==============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_overload_init")

	CmListInitialize(&channel->overload.overload_list);
	channel->overload.acp_overload_running = LSA_FALSE;
	channel->overload.send_overload_counter = 0;
	channel->overload.recv_overload_counter = 0;
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_undo_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_overload_undo_init")

	LSA_UNUSED_ARG(channel);

	CM_ASSERT(channel->overload.acp_overload_running == LSA_FALSE);
	CM_ASSERT(CmListIsEmpty(&channel->overload.overload_list));
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_cancel(
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  rsp
) {
	CM_LOG_FCT("cm_overload_cancel")

	CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rsp(%u)", rsp
		);

	while( ! CmListIsEmpty(&channel->overload.overload_list) ) {

		CM_UPPER_RQB_PTR_TYPE rb;

		CmListRemoveHead(&channel->overload.overload_list, rb, CM_UPPER_RQB_PTR_TYPE);

		cm_callback(channel, rsp, &rb);
	}
}


/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_overload_check(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE curr
) {
	CM_LOG_FCT("cm_acp_overload_check")

	if( is_null(curr) ) {

		curr = CmListFirst(&channel->overload.overload_list, CM_UPPER_RQB_PTR_TYPE);
	}
	else {

		CmListInsertTail(&channel->overload.overload_list, (CM_LIST_ENTRY_PTR_TYPE)curr);
	}

	while( is_not_null(curr) ) {

		CM_UPPER_RQB_PTR_TYPE delta = LSA_NULL;

		if( curr->args.channel.overload->send_overload_counter != channel->overload.send_overload_counter
		||  curr->args.channel.overload->recv_overload_counter != channel->overload.recv_overload_counter )
		{
			delta = curr;

			delta->args.channel.overload->send_overload_changed = LSA_FALSE;

			if( delta->args.channel.overload->send_overload_counter != channel->overload.send_overload_counter ) {
				delta->args.channel.overload->send_overload_counter = channel->overload.send_overload_counter;
				delta->args.channel.overload->send_overload_changed = LSA_TRUE;
			}

			delta->args.channel.overload->recv_overload_changed = LSA_FALSE;

			if( delta->args.channel.overload->recv_overload_counter != channel->overload.recv_overload_counter ) {
				delta->args.channel.overload->recv_overload_counter = channel->overload.recv_overload_counter;
				delta->args.channel.overload->recv_overload_changed = LSA_TRUE;
			}
		}

		curr = CmListNext(&channel->overload.overload_list, (CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

		/***/

		if( is_not_null(delta) ) {

			CmListRemoveEntry((CM_LIST_ENTRY_PTR_TYPE)delta);

			cm_callback(channel, CM_OK, &delta);
		}
	}
}

/*===========================================================================*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_acp_overload_info (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_acp_overload_info")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	cm_acp_alloc_rqb (channel, & acp);

	if (is_null (acp)) {
		CM_ACP_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no acp-resource"
			);
		return LSA_FALSE;
	}

	acp->args->channel.overload.rx_counter = 0;
	acp->args->channel.overload.tx_counter = 0;

	cm_acp_request_lower (channel, ACP_OPC_OVERLOAD_IND_RES_PROVIDE, channel, acp);
	return LSA_TRUE;
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_overload_info_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_acp_overload_info_done")

	CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "acp-rsp(%u)", CM_ACP_GET_RESPONSE (acp)
		);

	CM_ASSERT (channel->overload.acp_overload_running == LSA_TRUE);

	if( channel->state >= CM_CH_CLOSE ) {

		/* note: ACP_OPC_OVERLOAD_IND_RES_PROVIDE and ACP_OPC_OVERLOAD_INDICATION possible */

		CM_ACP_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "channel is closing, freeing acp-rqb"
			);

		CM_ASSERT(
			CM_ACP_GET_RESPONSE(acp) == ACP_OK
			|| CM_ACP_GET_RESPONSE(acp) == ACP_OK_CANCEL
		);

		cm_acp_free_rqb(channel, &acp);

		channel->overload.acp_overload_running = LSA_FALSE;
	}
	else {

		if( ! (
			CM_ACP_GET_OPCODE (acp) == ACP_OPC_OVERLOAD_INDICATION
			&& CM_ACP_GET_RESPONSE (acp) == ACP_OK
		) ) {
			CM_FATAL(); /* not as documented */
			return; /* unreachable... */
		}

		cm_acp_overload_indication (channel
			, acp->args->channel.overload.tx_counter
			, acp->args->channel.overload.rx_counter
			);

		cm_acp_request_lower (channel, ACP_OPC_OVERLOAD_IND_RES_PROVIDE, channel, acp);
	}
}


/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_info(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT("cm_overload_info")

	if( ! (
		channel->state >= CM_CH_READY
		&& channel->state < CM_CH_CLOSE
	) ) {

		CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong channel-state(%u)"
			, channel->state
			);

		cm_callback(channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	/***/

	if (! channel->overload.acp_overload_running) {

		if (! cm_acp_overload_info (channel)) {
			cm_callback (channel, CM_ERR_ALLOC_LOWER, & rb);
			return;
		}

		channel->overload.acp_overload_running = LSA_TRUE;
	}

	/***/

	cm_acp_overload_check(channel, rb);
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_overload_indication(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32  tx_counter,
	LSA_UINT32  rx_counter
) {
	CM_LOG_FCT("cm_acp_overload_indication")

	channel->overload.send_overload_counter = tx_counter;
	channel->overload.recv_overload_counter = rx_counter;

	CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "send_overload_counter(%u) recv_overload_counter(%u)"
		, channel->overload.send_overload_counter
		, channel->overload.recv_overload_counter
		);

	/***/

	cm_acp_overload_check(channel, LSA_NULL);
}

/*===========================================================================*/
/*=== FRAME INDICATION ======================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_indication(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_acp_frame_indication")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr;

	io_cr = (CM_AR_GRAPH_IO_CR_PTR_TYPE)acp->args->state.ind.usr_id; /* see add_cons/add_prov */

	if (is_null (io_cr))
	{
		CM_ACP_LOWER_RQB_ERROR(acp); /* should not happen */
	}
	else
	{
		CM_AR_GRAPH_PTR_TYPE  argr = io_cr->cm.backlink;

		if (io_cr->cm.acp_id == ACP_INVALID_ACP_ID)
		{
			/* late, see cm_acp_frame_remove() */

			CM_ACP_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "FrameIndication: dev(%u) ar(%u) acp_id already invalid, ignored"
				, argr->device_nr, argr->ar_nr
				);
		}
		else
		{
			LSA_UINT32  state_event   = acp->args->state.ind.state_event;
#if 0/*todo*/
			LSA_UINT32  total_state   = acp->args->state.ind.total_state; /* todo: check functionality */
#endif
			LSA_UINT16  cycle_counter = acp->args->state.ind.cycle_count;

			CM_ASSERT (io_cr->cm.acp_id == acp->args->state.ind.acp_id);

			/***/

			if (io_cr->cm.is_multicast )
			{
#if CM_CFG_USE_NARE

				if ((state_event & (ACP_STATE_AGAIN)) != 0)
				{
					cm_arsm_mcons_up(channel, io_cr);
				}
				else if ((state_event & (ACP_STATE_MISS | ACP_STATE_MISS_NOT_STOPPED)) != 0)
				{
					cm_arsm_mcons_down(channel, io_cr);
				}

				/* note: entering-red == again-red */
#else
				CM_FATAL(); /* should not happen */
#endif
			}
			else /* unicast */
			{
				LSA_BOOL  do_com_check = LSA_FALSE;

				if ((state_event & (ACP_STATE_AGAIN)) != 0)
				{
					if( io_cr->cm.cr_state == CM_AR_CR_WF_UP )
					{
						CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
							, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), AGAIN, cycle_counter(0x%x)"
							, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
							);

						io_cr->cm.cr_state = CM_AR_CR_UP;

						do_com_check = LSA_TRUE;
					}
					else
					{
						/* again after miss or after reset.req or passive */
						CM_ACP_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), AGAIN, state(%u) not WF_UP, ignoring! cycle_counter(0x%x)"
							, argr->device_nr, argr->ar_nr, io_cr->frame_id, io_cr->cm.cr_state, cycle_counter
							);
					}
				}
				else if ((state_event & (ACP_STATE_MISS | ACP_STATE_MISS_NOT_STOPPED)) != 0)
				{
					if (io_cr->cm.cr_state == CM_AR_CR_WF_UP ||	io_cr->cm.cr_state == CM_AR_CR_UP)
					{
						if ((state_event & (ACP_STATE_MISS)) != 0)
						{
							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), MISS, last good cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);
						}
						else
						{
							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), MISS_NOT_STOPPED, last good cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);
						}

						io_cr->cm.cr_state = CM_AR_CR_DOWN;
						cm_ar_com_down_debug_data_set (argr, & acp->args->state.ind.debug_data);
						cm_ar_com_down2 (argr, CM_AR_REASON_MISS, cycle_counter);
					}
					else
					{
						CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
							, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), ignore miss in cr-state(%u)"
							, argr->device_nr, argr->ar_nr, io_cr->frame_id, io_cr->cm.cr_state
							);
					}
				}

				/***/

				if (! cm_ar_com_is_down (argr))
				{
					if ((state_event & (ACP_STATE_RED_ENTERING)) != 0) /* note: ACP_STATE_RED_LEAVING removed with V5.3i1.2 (AP01448175) */
					{
						if (! io_cr->cm.is_rir)
						{
							io_cr->cm.is_rir = LSA_TRUE;

							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), Entering RecvInRed, cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);

							do_com_check = LSA_TRUE;
						}
						else
						{
							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), Entering RecvInRed ignored!, cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);
						}
					}
				}

				if (do_com_check)
				{
					(LSA_VOID) cm_ar_com_check (argr);
				}
			}

			/***/

			if (! cm_ar_com_is_down (argr))
			{
				state_event &= (
					ACP_STATE_STOP                        | ACP_STATE_RUN |
					ACP_STATE_STATION_FAILURE             | ACP_STATE_STATION_OK |
					ACP_STATE_BACKUP                      | ACP_STATE_PRIMARY |
					ACP_STATE_SYSRED_NO_PRIMARY_AR_EXISTS | ACP_STATE_SYSRED_PRIMARY_AR_EXISTS
					);

				if( state_event != 0)
				{
					cm_ar_com_cons_apdu_status_changed (argr, io_cr, state_event);
				}
			}
		}

		/* provide again */
		cm_acp_request_lower (channel, ACP_OPC_STATE_IND_RES_PROVIDE, 0/*user-id*/, acp);
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
