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
/*  F i l e               &F: cm_mrp.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MRP (Media Redundancy Protocol) things                                   */
/*                                                                           */
/*  - alloc MRP-RQB                                                          */
/*  - free MRP-RQB                                                           */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*  - prm interface .req                                                     */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	42
#define CM_MODULE_ID		42

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_MRP

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_MRP_LOWER_RQB_PTR_TYPE mrp /* provide: LSA_NULL */
);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_MRP_LOWER_RQB_PTR_TYPE mrp
);

/*===========================================================================*/
/*=== MRP CHANNEL-INIT / -UNDO-INIT =========================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_channel_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_mrp_channel_init")

	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	/* provide some PRM indication resources (must do it in context of CM-MRP, not CM-PD) */

	cm_mrp_prm_indication(channel, LSA_NULL);
}

/*===========================================================================*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_mrp_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_mrp_channel_undo_init")

	/* NOTE: caller wipes out channel->usr.xxx when returning CM_OK */

	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	if( is_not_null(channel->lower.channel[CM_PATH_TYPE_PD]) ) {
		CM_FATAL(); /* cm-pd not unlinked */
	}

	return CM_OK;
}

/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/
CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_mrp_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_mrp_channel_get")

	channel = channel->lower.channel[CM_PATH_TYPE_MRP];

	if( is_null(channel) ) {
		CM_FATAL(); /* a bug */
	}

	return channel;
}

/*-----------------------------------------------------------------------------
//	alloc lower MRP-RQB that fits for all requests
//---------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	*lower_rqb_ptr_ptr
)
{
	CM_LOG_FCT ("cm_mrp_alloc_rqb")
	CM_CHANNEL_PTR_TYPE mrp_channel = cm_mrp_channel_get(channel);

	CM_ASSERT (is_not_null(lower_rqb_ptr_ptr));

		/* alloc the RQB */
	CM_MRP_ALLOC_LOWER_RQB (lower_rqb_ptr_ptr, cm_null_user_id, sizeof (CM_MRP_LOWER_RQB_TYPE), mrp_channel->sysptr);

	if (is_null (*lower_rqb_ptr_ptr)) {
		CM_MRP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc MRP RQB, len(%u)"
			, channel->my_handle, sizeof (CM_MRP_LOWER_RQB_TYPE)
			);
		return;
	}

#if CM_DEBUG
	CM_MRP_SET_NEXT_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
	CM_MRP_SET_PREV_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif
}


/*-----------------------------------------------------------------------------
//	free the RQB
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
)
{
	CM_LOG_FCT ("cm_mrp_free_rqb")
	CM_CHANNEL_PTR_TYPE mrp_channel = cm_mrp_channel_get(channel);
	LSA_UINT16	rc;

	CM_ASSERT (is_not_null(lower_rqb_ptr));

	CM_MRP_FREE_LOWER_RQB (
		& rc,
		lower_rqb_ptr,
		mrp_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
}


/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_LOG_FCT ("cm_mrp_channel_open")
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_MRP);

	CM_ASSERT (are_equal(cm_mrp_channel_get(channel), channel));

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == CM_MRP_INVALID_HANDLE);

	/***/

	cm_mrp_alloc_rqb (channel, & mrp);

	if (is_null (mrp)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_MRP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	mrp->args.channel.open.handle = CM_MRP_INVALID_HANDLE;
	mrp->args.channel.open.handle_upper = channel->my_handle;
	mrp->args.channel.open.sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	mrp->args.channel.open.pCbf = cm_mrp_request_lower_done;

	CM_MRP_SET_USER_ID_HANDLE_DEV_AR (mrp, channel->my_handle, 0, 0);

	CM_MRP_SET_HANDLE (mrp, CM_MRP_INVALID_HANDLE);
	CM_MRP_SET_OPCODE (mrp, MRP_OPC_OPEN_CHANNEL);

	CM_MRP_OPEN_CHANNEL_LOWER (mrp, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp
)
{
	CM_LOG_FCT ("cm_mrp_channel_open_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	if (CM_MRP_GET_RESPONSE (mrp) == MRP_RSP_OK) {

		channel->lower.handle = mrp->args.channel.open.handle;
		channel->lower.prealloc.mrp = mrp;

		CM_ASSERT(channel->lower.handle != CM_MRP_INVALID_HANDLE);

		CM_MRP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		rc = CM_OK;
	}
	else {

		CM_ASSERT(channel->lower.handle == CM_MRP_INVALID_HANDLE);

		CM_MRP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) mrp-rsp(%u)"
			, channel->my_handle, CM_MRP_GET_RESPONSE (mrp)
			);

		rc = CM_ERR_LOWER_LAYER;

		cm_mrp_free_rqb(channel, mrp);
	}

	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_channel_close (
	CM_CHANNEL_PTR_TYPE channel
)
{
	CM_LOG_FCT ("cm_mrp_channel_close")
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_MRP);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	CM_ASSERT (is_not_null(channel->lower.prealloc.mrp));

	mrp = channel->lower.prealloc.mrp;

	CM_MRP_SET_USER_ID_HANDLE_DEV_AR (mrp, channel->my_handle, 0, 0);

	CM_MRP_SET_HANDLE (mrp, channel->lower.handle);
	CM_MRP_SET_OPCODE (mrp, MRP_OPC_CLOSE_CHANNEL);

	CM_MRP_CLOSE_CHANNEL_LOWER (mrp, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp
) {
	CM_LOG_FCT ("cm_mrp_close_channel_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_MRP_GET_RESPONSE (mrp);

	if (rc == MRP_RSP_OK) {

		cm_mrp_free_rqb (channel, mrp);

		channel->lower.handle = CM_MRP_INVALID_HANDLE;
		channel->lower.prealloc.mrp = LSA_NULL;

		CM_MRP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) mrp-closed"
			, channel->my_handle
			);

		rc = CM_OK;
	}
	else {

		/* don't invalidate mrp-handle, channel is not closed! */

		CM_MRP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) mrp-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}


/*===========================================================================*/
/*=== MRP INFO ==============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_channel_info(
	CM_CHANNEL_PTR_TYPE channel
) {
	cm_channel_done(channel, CM_OK); /* note: recursion!*/
}


/*===========================================================================*/
/*=== PRM INTERFACE =========================================================*/
/*===========================================================================*/

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_mrp_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
) {
	CM_PORT_UINT mask;

	channel = cm_edd_channel_get(channel); /* get from EDD-channel */

	mask = ((CM_PORT_UINT)2 << channel->lower.info.edd.MaxPortCnt) - 1;

	return mask;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_MRP_LOWER_RQB_PTR_TYPE mrp /* provide: LSA_NULL */
) {
	CM_LOG_FCT("cm_mrp_prm_indication")

	if( is_null(mrp) ) { /* provide */

		CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
		CM_PORT_UINT provide_mask = 0;
		LSA_UINT16 port_id;

		for( port_id = 0; port_id <= edd_channel->lower.info.edd.MaxPortCnt; ++port_id ) {

			LSA_UINT16 max = 2/*!*/;
			LSA_UINT16 cnt;

			for( cnt = 0; cnt < max; ++cnt ) {

				cm_mrp_alloc_rqb(channel, &mrp);

				if( is_null(mrp) ) {
					CM_MRP_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot mrp-alloc");
					CM_FATAL();
				}

				mrp->args.state.prm_ind.edd_port_id = port_id; /* an in-paramter! */

				mrp->args.state.prm_ind.diag_cnt = 0; /* sanity (an out-paramter!) */

				cm_mrp_request_lower(channel, MRP_OPC_PRM_INDICATION, 0, 0, mrp);
			}

			provide_mask |= ((CM_PORT_UINT)1 << port_id);
		}

		if( provide_mask != cm_mrp_prm_provide_mask(channel) ) {
			CM_FATAL(); /* a bug */
		}
	}
	else { /* handle indication and do re-provide */

		CM_MRP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "mrp-prm.IND edd_port_id(%u) diag_cnt(%u)"
			, mrp->args.state.prm_ind.edd_port_id
			, mrp->args.state.prm_ind.diag_cnt
			);

		if( channel->state >= CM_CH_CLOSE ) {

			CM_MRP_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);

			CM_ASSERT(
				CM_MRP_GET_RESPONSE(mrp) == MRP_RSP_OK
				|| CM_MRP_GET_RESPONSE(mrp) == MRP_RSP_OK_CANCEL
				);

			cm_mrp_free_rqb(channel, mrp);
		}
		else if( CM_MRP_GET_RESPONSE(mrp) != MRP_RSP_OK ) {

			CM_FATAL(); /* what? why? */
		}
		else {

			if( mrp->args.state.prm_ind.diag_cnt > MRP_PRM_INDICATION_DIAG_MAX_ENTRIES ) {
				CM_FATAL();
			}

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

			if( is_null(pd_channel) )
			{
				CM_MRP_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "mrp-prm.IND but no pd-channel, ignored");
			}
			else
			{
				cm_pd_mrp_prm_indication_done(pd_channel, &mrp->args.state.prm_ind);
			}
			}

			/***/

			/*
			mrp->args.state.prm_ind.edd_port_id = unchanged (an in-paramter!)
			*/

			mrp->args.state.prm_ind.diag_cnt = 0; /* sanity (an out-paramter!) */

			cm_mrp_request_lower(channel, MRP_OPC_PRM_INDICATION, 0, 0, mrp); /* re-provide */
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_prepare (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_mrp_prm_prepare")
	CM_CHANNEL_PTR_TYPE  mrp_channel = cm_mrp_channel_get(channel);
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp = mrp_channel->lower.prealloc.mrp;

	CM_ASSERT (is_not_null(mrp));

	mrp->args.ctrl.prm_prepare.dummy = 1;

	cm_mrp_request_lower (channel, MRP_OPC_PRM_PREPARE, 0, 0, mrp);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_write (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_LOG_FCT("cm_mrp_prm_write")
	CM_CHANNEL_PTR_TYPE  mrp_channel = cm_mrp_channel_get(channel);
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp = mrp_channel->lower.prealloc.mrp;

	CM_ASSERT (is_not_null(mrp));

	mrp->args.ctrl.prm_write.edd_port_id = edd_port_id;
	mrp->args.ctrl.prm_write.slot_number = slot_number;
	mrp->args.ctrl.prm_write.subslot_number = subslot_number;
	mrp->args.ctrl.prm_write.record_index = record_index;
	mrp->args.ctrl.prm_write.record_data_length = record_data_length;

	/* record_data used as reference, check this */
	mrp->args.ctrl.prm_write.record_data = CM_CAST_MRP_MEM_U8_PTR (record_data);

	cm_mrp_request_lower (channel, MRP_OPC_PRM_WRITE, 0, 0, mrp);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_mrp_prm_read (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_LOG_FCT("cm_mrp_prm_read")
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp;

	cm_mrp_alloc_rqb (channel, & mrp);
	if (is_null (mrp)) {
		return CM_ERR_ALLOC_LOWER;
	}

	mrp->args.ctrl.prm_write.edd_port_id = edd_port_id;
	mrp->args.ctrl.prm_write.slot_number = slot_number;
	mrp->args.ctrl.prm_write.subslot_number = subslot_number;
	mrp->args.ctrl.prm_write.record_index = record_index;
	mrp->args.ctrl.prm_write.record_data_length = record_data_length;

	/* record_data used as reference, check this */
	mrp->args.ctrl.prm_write.record_data = CM_CAST_MRP_MEM_U8_PTR (record_data);

	cm_mrp_request_lower (channel, MRP_OPC_PRM_READ, 0, 0, mrp);
	return CM_OK;
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_end (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_mrp_prm_end")
	CM_CHANNEL_PTR_TYPE  mrp_channel = cm_mrp_channel_get(channel);
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp = mrp_channel->lower.prealloc.mrp;

	CM_ASSERT (is_not_null(mrp));

	mrp->args.ctrl.prm_end.dummy = 2;

	cm_mrp_request_lower (channel, MRP_OPC_PRM_END, 0, 0, mrp);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_commit (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_mrp_prm_commit")
	CM_CHANNEL_PTR_TYPE  mrp_channel = cm_mrp_channel_get(channel);
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp = mrp_channel->lower.prealloc.mrp;

	CM_ASSERT (is_not_null(mrp));

	mrp->args.ctrl.prm_commit.dummy = 3;

	cm_mrp_request_lower (channel, MRP_OPC_PRM_COMMIT, 0, 0, mrp);
}

/*===========================================================================*/
/*=== MRP REQUEST LOWER =====================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_MRP_LOWER_RQB_PTR_TYPE mrp
) {
	CM_LOG_FCT ("cm_mrp_request_lower")
	CM_CHANNEL_PTR_TYPE mrp_channel = cm_mrp_channel_get(channel);

	CM_MRP_SET_USER_ID_HANDLE_DEV_AR (mrp, channel->my_handle, device_nr, ar_nr);

	CM_MRP_SET_HANDLE (mrp, mrp_channel->lower.handle);
	CM_MRP_SET_OPCODE (mrp, opcode);

	CM_MRP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> mrp-rqb(0x%08x) mrp-opc(%u) f-h(%u) f-dev(%u) f-ar(%u)"
		, mrp, CM_MRP_GET_OPCODE (mrp)
		, CM_MRP_GET_USER_ID_HANDLE (mrp), CM_MRP_GET_USER_ID_DEV (mrp), CM_MRP_GET_USER_ID_AR (mrp)
		);

	CM_MRP_REQUEST_LOWER (mrp, mrp_channel->sysptr);
}

/*===========================================================================*/
/*=== MRP LOWER DONE ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_MRP_LOWER_IN_FCT_ATTR
cm_mrp_request_lower_done (
	CM_MRP_LOWER_RQB_PTR_TYPE  mrp
) {
	CM_LOG_FCT ("cm_mrp_request_lower_done")
	CM_CHANNEL_PTR_TYPE  channel;

	if( is_null(mrp) ) {
		CM_MRP_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL, "is_null(mrp)");
		CM_MRP_LOWER_RQB_ERROR (mrp);
		return;
	}

	channel = cm_channel_from_handle (CM_MRP_GET_USER_ID_HANDLE (mrp));

	if( is_null (channel) ) {

		CM_MRP_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"invalid handle(%u)",
			CM_MRP_GET_USER_ID_HANDLE (mrp)
			);
		CM_MRP_LOWER_RQB_ERROR (mrp);
		return;
	}

	switch (CM_MRP_GET_RESPONSE (mrp)) {
	case MRP_RSP_OK:
	case MRP_RSP_OK_CANCEL:
		CM_MRP_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< mrp-rqb(0x%08x) mrp-opc(%u) mrp-rsp(%u) t-h(%u) t-dev(%u) t-ar(%u)"
			, mrp, CM_MRP_GET_OPCODE (mrp), CM_MRP_GET_RESPONSE (mrp)
			, CM_MRP_GET_USER_ID_HANDLE (mrp), CM_MRP_GET_USER_ID_DEV (mrp), CM_MRP_GET_USER_ID_AR (mrp)
			);
		break;

	case MRP_RSP_ERR_PRM_INDEX:
	/*case MRP_RSP_ERR_PRM_PORTID:*/
		CM_MRP_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH /* AP00431325: don't trace with error-level if "role off" etc. */
			, "<<< mrp-rqb(0x%08x) mrp-opc(%u) mrp-rsp(%u) t-h(%u) t-dev(%u) t-ar(%u)"
			, mrp, CM_MRP_GET_OPCODE (mrp), CM_MRP_GET_RESPONSE (mrp)
			, CM_MRP_GET_USER_ID_HANDLE (mrp), CM_MRP_GET_USER_ID_DEV (mrp), CM_MRP_GET_USER_ID_AR (mrp)
			);
		break;

	default:
		CM_MRP_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "<<< mrp-rqb(0x%08x) mrp-opc(%u) mrp-rsp(%u) t-h(%u) t-dev(%u) t-ar(%u)"
			, mrp, CM_MRP_GET_OPCODE (mrp), CM_MRP_GET_RESPONSE (mrp)
			, CM_MRP_GET_USER_ID_HANDLE (mrp), CM_MRP_GET_USER_ID_DEV (mrp), CM_MRP_GET_USER_ID_AR (mrp)
			);
		break;
	}

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_MRP:

		switch (CM_MRP_GET_OPCODE (mrp)) {

		case MRP_OPC_OPEN_CHANNEL:
			cm_mrp_channel_open_done (channel, mrp);
			break;

		case MRP_OPC_CLOSE_CHANNEL:
			cm_mrp_channel_close_done (channel, mrp);
			break;

		case MRP_OPC_PRM_INDICATION:
			cm_mrp_prm_indication(channel, mrp);
			break;

		default:
			CM_MRP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_MRP_GET_OPCODE (mrp)
				);
			CM_MRP_LOWER_RQB_ERROR (mrp); /* should not happen */
			break;
		}
		break;

	case CM_PATH_TYPE_PD:

		switch (CM_MRP_GET_OPCODE (mrp)) {

		case MRP_OPC_PRM_PREPARE:
			cm_pd_mrp_prm_prepare_done (channel, mrp);
			break;

		case MRP_OPC_PRM_WRITE:
			cm_pd_mrp_prm_write_done (channel, mrp);
			break;

		case MRP_OPC_PRM_END:
			cm_pd_mrp_prm_end_done (channel, mrp);
			break;

		case MRP_OPC_PRM_COMMIT:
			cm_pd_mrp_prm_commit_done (channel, mrp);
			break;

		case MRP_OPC_PRM_READ:
			cm_pd_mrp_prm_read_done (channel, mrp);
			break;

		default:
			CM_MRP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_MRP_GET_OPCODE (mrp)
				);
			CM_MRP_LOWER_RQB_ERROR (mrp); /* should not happen */
			break;
		}
		break;

	default:
		CM_MRP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "wrong path-type(%u)"
			, channel->detailptr->path_type
			);
		CM_MRP_LOWER_RQB_ERROR (mrp); /* should not happen */
		break;
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ MRP; CM_CFG_USE_MRP=" CM_STRINGIFY(CM_CFG_USE_MRP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_MRP */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
