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
/*  F i l e               &F: cm_gsy.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  GSY (generic sync) things                                                */
/*                                                                           */
/*  - alloc GSY-RQB                                                          */
/*  - free GSY-RQB                                                           */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	41
#define CM_MODULE_ID		41

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_GSY

/*===========================================================================*/

#ifdef GSY_RT_CLASS3_SUPPORT_ON
# error "please, review your gsy_cfg.h: GSY_RT_CLASS3_SUPPORT_ON was removed with version V4.0i30"
#endif

/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_diag_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_sync_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT8 sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
);

/*===========================================================================*/
/*=== GSY CHANNEL-INIT / -UNDO-INIT =========================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_channel_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_gsy_channel_init")

	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	/* sanity-check */

	if( ! (
		channel->lower.info.gsy.SyncId < GSY_MAX_PROTOCOLS /* not: GSY_NUM_PROTOCOLS */
		&& ((1UL << channel->lower.info.gsy.SyncId) & GSY_SYNC_ID_SUPPORTED) != 0
	) ) {
		CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "wrong configuration");
		CM_FATAL();
	}

	switch( channel->lower.info.gsy.SyncId ) {
	case GSY_SYNCID_CLOCK:
	case GSY_SYNCID_TIME:
		break;
	default:
		CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "wrong configuration");
		CM_FATAL();
		break;
	}

	/***/

	channel->usr.gsy.do_sync = LSA_FALSE;
	channel->usr.gsy.is_sync = LSA_FALSE;

	channel->usr.gsy.is_rate_valid = LSA_FALSE;
	channel->usr.gsy.rcv_sync_prio = 0;
	channel->usr.gsy.local_prio    = 0;
	channel->usr.gsy.pll_state     = GSY_PLL_STATE_SYNC_OFF;


	CM_MEMSET(&channel->usr.gsy.master_mac_addr, 0, sizeof(channel->usr.gsy.master_mac_addr));

	/* provide some diag-indication resources (must do it in context of CM-GSY, not CM-PD) */

	cm_gsy_diag_indication(channel, LSA_NULL);

	/* provide some prm-indication resources (must do it in context of CM-GSY, not CM-PD) */

	cm_gsy_prm_indication(channel, LSA_NULL);
}

/*===========================================================================*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_gsy_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_gsy_channel_undo_init")

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
cm_gsy_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_gsy_channel_get")

	channel = channel->lower.channel[CM_PATH_TYPE_GSY];

	if( is_null(channel) ) {
		CM_FATAL(); /* a bug */
	}

	return channel;
}

/*-----------------------------------------------------------------------------
//	alloc lower GSY-RQB that fits for all requests
//---------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr_ptr
)
{
	CM_LOG_FCT ("cm_gsy_alloc_rqb")
	CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get(channel);

	CM_ASSERT (is_not_null(lower_rqb_ptr_ptr));

		/* alloc the RQB */
	CM_GSY_ALLOC_LOWER_RQB (lower_rqb_ptr_ptr, cm_null_user_id, sizeof (CM_GSY_LOWER_RQB_TYPE), gsy_channel->sysptr);

	if (is_null (*lower_rqb_ptr_ptr)) {
		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc GSY RQB, len(%u)"
			, channel->my_handle, sizeof (CM_GSY_LOWER_RQB_TYPE)
			);
		return;
	}

#if CM_DEBUG
	CM_GSY_SET_NEXT_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
	CM_GSY_SET_PREV_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif
}


/*-----------------------------------------------------------------------------
//	free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
)
{
	CM_LOG_FCT ("cm_gsy_free_rqb")
	CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get(channel);

	CM_ASSERT (is_not_null(lower_rqb_ptr));

	{
	LSA_UINT16	rc;
	CM_GSY_FREE_LOWER_RQB (
		& rc,
		lower_rqb_ptr,
		gsy_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
	}
}


/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_LOG_FCT ("cm_gsy_channel_open")
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_GSY);

	CM_ASSERT (are_equal(cm_gsy_channel_get(channel), channel));

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == CM_GSY_INVALID_HANDLE);

	/***/

	cm_gsy_alloc_rqb (channel, & gsy);

	if (is_null (gsy)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	gsy->args.channel.handle = CM_GSY_INVALID_HANDLE;
	gsy->args.channel.handle_upper = channel->my_handle;
	gsy->args.channel.sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	gsy->args.channel.gsy_request_upper_done_ptr = cm_gsy_request_lower_done;

	CM_GSY_SET_USER_ID_HANDLE_DEV_AR (gsy, channel->my_handle, 0, 0);

	CM_GSY_SET_HANDLE (gsy, CM_GSY_INVALID_HANDLE);
	CM_GSY_SET_OPCODE (gsy, GSY_OPC_OPEN_CHANNEL);

	CM_GSY_OPEN_CHANNEL_LOWER (gsy, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy
)
{
	CM_LOG_FCT ("cm_gsy_channel_open_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	rc = CM_GSY_GET_RESPONSE (gsy);

	if (rc == GSY_RSP_OK) {

		channel->lower.handle = gsy->args.channel.handle;
		channel->lower.prealloc.gsy = gsy;
		
		CM_ASSERT(channel->lower.handle != CM_GSY_INVALID_HANDLE);

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) gsy-handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		rc = CM_OK;
	}
	else {

		CM_ASSERT(channel->lower.handle == CM_GSY_INVALID_HANDLE);

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) gsy-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;

		cm_gsy_free_rqb(channel, gsy);
	}

	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_channel_close (
	CM_CHANNEL_PTR_TYPE channel
)
{
	CM_LOG_FCT ("cm_gsy_channel_close")
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_GSY);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	CM_ASSERT (is_not_null(channel->lower.prealloc.gsy));

	gsy = channel->lower.prealloc.gsy;

	CM_GSY_SET_USER_ID_HANDLE_DEV_AR (gsy, channel->my_handle, 0, 0);

	CM_GSY_SET_HANDLE (gsy, channel->lower.handle);
	CM_GSY_SET_OPCODE (gsy, GSY_OPC_CLOSE_CHANNEL);

	CM_GSY_CLOSE_CHANNEL_LOWER (gsy, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy
) {
	CM_LOG_FCT ("cm_gsy_close_channel_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_GSY_GET_RESPONSE (gsy);

	if (rc == GSY_RSP_OK) {

		cm_gsy_free_rqb (channel, gsy);

		channel->lower.handle = CM_GSY_INVALID_HANDLE;
		channel->lower.prealloc.gsy = LSA_NULL;

		CM_GSY_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u)"
			, channel->my_handle
			);

		rc = CM_OK;
	}
	else {

		/* don't invalidate gsy-handle, channel is not closed! */

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) gsy-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}

/*===========================================================================*/
/*=== GSY INFO ==============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_channel_info(
	CM_CHANNEL_PTR_TYPE channel
) {

	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	CM_ASSERT(channel->state == CM_CH_INFO);

	cm_gsy_alloc_rqb(channel, &gsy);

	if( is_null(gsy) ) {
		cm_channel_done(channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	cm_gsy_request_lower(channel, GSY_OPC_CHANNEL_INFO_GET, 0, 0, gsy);
}

/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_channel_info_done(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
) {
	CM_LOG_FCT("cm_gsy_channel_info_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_INFO);

	rc = CM_GSY_GET_RESPONSE (gsy);

	CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "h(%u) gsy-rsp(%u)", channel->my_handle, rc
		);

	if (rc == GSY_RSP_OK) {

		channel->lower.info.gsy = gsy->args.ChannelInfo;

		rc = CM_OK;
	}
	else {

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_gsy_free_rqb (channel, gsy);

	cm_channel_done (channel, rc);
}

/*===========================================================================*/
/*=== DIAG INDICATION =======================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_diag_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
) {
	CM_LOG_FCT("cm_gsy_diag_indication")

	if( channel->lower.info.gsy.SyncId != GSY_SYNCID_CLOCK ) {

		return; /* for now we are only interested in GSY_SYNCID_CLOCK */
	}

	if( is_null(gsy) ) { /* provide */

		cm_gsy_alloc_rqb(channel, &gsy);

		if( is_null(gsy) ) {
			CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot gsy-alloc");
			CM_FATAL();
			return;
		}

		/***/

		{
		GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE GSY_UPPER_MEM_ATTR *ind = &gsy->args.DiagInd;

		ind->Trigger = GSY_TRIGGER_CHECK; /* using the "stateful" mode */

		ind->IndCount = 0; /* sanity (an out-paramter!) */
		ind->SyncId   = GSY_SYNCID_CLOCK; /* sanity (an out-paramter!) */

		ind->StateSync     = channel->usr.gsy.is_sync;
		ind->MasterMacAddr = channel->usr.gsy.master_mac_addr;
		ind->Subdomain     = channel->usr.gsy.do_sync;
		ind->RateValid     = channel->usr.gsy.is_rate_valid;
		ind->RcvSyncPrio   = channel->usr.gsy.rcv_sync_prio;
		ind->LocalPrio     = channel->usr.gsy.local_prio;
		ind->PLLState      = channel->usr.gsy.pll_state;

		ind->DriftInterval = 0; /* don't care for GSY_TRIGGER_CHECK */
		ind->UserValue1    = 0; /* don't care for GSY_TRIGGER_CHECK */
		ind->UserValue2    = 0; /* don't care for GSY_TRIGGER_CHECK */
		}

		cm_gsy_request_lower(channel, GSY_OPC_DIAG_INDICATION_PROVIDE, 0, 0, gsy);
	}
	else { /* handle indication and do re-provide */

		GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE GSY_UPPER_MEM_ATTR *ind = &gsy->args.DiagInd;

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "gsy-diag.IND SyncId(%u) IndCount(%u)"
			, ind->SyncId, ind->IndCount
			);

		if( channel->state >= CM_CH_CLOSE ) {

			CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);

			CM_ASSERT(
				CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK
				|| CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK_CANCEL
				);

			cm_gsy_free_rqb(channel, gsy);
		}
		else if( CM_GSY_GET_RESPONSE(gsy) != GSY_RSP_OK ) {

			CM_FATAL(); /* what? why? */
		}
		else {

			if( ind->SyncId != GSY_SYNCID_CLOCK ) {
				CM_FATAL(); /* see provide */
			}

			/***/

			CM_GSY_TRACE_12(0,LSA_TRACE_LEVEL_NOTE_HIGH
				, "StateSync(%u) Subdomain(%u) MasterMac(%02x-%02x-%02x-%02x-%02x-%02x) RateValid(%u) RcvSyncPrio(%u) LocalPrio(%u) PLLState(%u)"
				, ind->StateSync, ind->Subdomain
				, ind->MasterMacAddr.MacAdr[0], ind->MasterMacAddr.MacAdr[1], ind->MasterMacAddr.MacAdr[2]
				, ind->MasterMacAddr.MacAdr[3], ind->MasterMacAddr.MacAdr[4], ind->MasterMacAddr.MacAdr[5]
				, ind->RateValid, ind->RcvSyncPrio, ind->LocalPrio, ind->PLLState
				);

			channel->usr.gsy.is_sync           = ind->StateSync; /* LSA_TRUE if SyncId is synchronized, else LSA_FALSE */
			channel->usr.gsy.master_mac_addr   = ind->MasterMacAddr;
			channel->usr.gsy.do_sync           = ind->Subdomain; /* LSA_TRUE if Sync-Subdomain configured (LSA_FALSE: no PDSyncData record loaded) */
			channel->usr.gsy.is_rate_valid     = ind->RateValid;
			channel->usr.gsy.rcv_sync_prio     = ind->RcvSyncPrio;
			channel->usr.gsy.local_prio        = ind->LocalPrio;
			channel->usr.gsy.pll_state       = ind->PLLState;

			if( channel->usr.gsy.do_sync ) { /* have PDSyncData */

				/* note: master_mac_addr contains the current master-mac */

				/* note: master_mac_addr may be "all zero" during takeover (best master) */
			}
			else {

				CM_ASSERT(channel->usr.gsy.is_sync == LSA_FALSE); /* sanity */

				channel->usr.gsy.is_sync = LSA_FALSE; /* make it explicit */

				/* note: master_mac_addr contains the last known master-mac */
			}

			if( ! channel->usr.gsy.is_sync ) {

				/* OHA has to send "all zero" when not sync */

				CM_MEMSET(&channel->usr.gsy.master_mac_addr, 0, sizeof(channel->usr.gsy.master_mac_addr));
			}

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

			if( is_null(pd_channel) ) { /* e.g. when closing the pd-channel */

				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "gsy-diag.IND but no pd-channel, ignored (but storing values)"
					);
			}
			else {

				cm_pd_sync_state_changed(pd_channel, /*gsy_*/channel);
			}
			}

			/***/

			cm_gsy_request_lower(channel, GSY_OPC_DIAG_INDICATION_PROVIDE, 0, 0, gsy); /* re-provide (stateful!) */
		}
	}
}

/*===========================================================================*/
/*=== PRM INTERFACE =========================================================*/
/*===========================================================================*/

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_gsy_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
) {
	CM_PORT_UINT mask;

	LSA_UNUSED_ARG(channel);

	mask = ((CM_PORT_UINT)2 << 0/*interface*/) - 1;

	return mask;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
) {
	CM_LOG_FCT("cm_gsy_prm_indication")

	if( is_null(gsy) ) { /* provide */

		LSA_UINT16 max = 2/*!*/;
		LSA_UINT16 cnt;

		for( cnt = 0; cnt < max; ++cnt ) {

			LSA_UINT32 size;

			cm_gsy_alloc_rqb(channel, &gsy);

			if( is_null(gsy) ) {
				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot gsy-alloc-rqb");
				CM_FATAL();
			}

			size = sizeof(GSY_PRM_IND_DATA_TYPE) * GSY_PRM_IND_MAX_ENTRIES;

			if (size > 0xFFFF) { /* always false (see cast below) */
				CM_FATAL();
			}

			CM_GSY_ALLOC_LOWER_MEM (
				CM_CAST_GSY_MEM_PTR_PTR (&gsy->args.PrmInd.pDiag),
				cm_null_user_id,
				CM_CAST_U16(size),
				channel->sysptr
				);

			if (is_null (gsy->args.PrmInd.pDiag)) {
				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot gsy-alloc-mem");
				CM_FATAL();
			}

			/***/

			gsy->args.PrmInd.EddPortId = 0/*interface*/; /* an in-paramter! */

			gsy->args.PrmInd.DiagCnt = 0; /* sanity (an out-paramter!) */

			cm_gsy_request_lower(channel, GSY_OPC_PRM_INDICATION_PROVIDE, 0, 0, gsy);
		}

		if( (CM_PORT_UINT)0x01/*provide_mask*/ != cm_gsy_prm_provide_mask(channel) ) {
			CM_FATAL(); /* a bug */
		}
	}
	else { /* handle indication and do re-provide */

		CM_GSY_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "gsy-prm.IND SyncId(%u) DiagCnt(%u) EddPortId(%u)"
			, channel->lower.info.gsy.SyncId
			, gsy->args.PrmInd.DiagCnt
			, gsy->args.PrmInd.EddPortId
			);

		if( channel->state >= CM_CH_CLOSE ) {

			CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);

			CM_ASSERT(
				CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK
				|| CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK_CANCEL
				);

			CM_ASSERT(is_not_null(gsy->args.PrmInd.pDiag));

			{
			LSA_UINT16 rc;
			CM_GSY_FREE_LOWER_MEM (& rc, gsy->args.PrmInd.pDiag, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
			}

			cm_gsy_free_rqb(channel, gsy);
		}
		else if( CM_GSY_GET_RESPONSE(gsy) != GSY_RSP_OK ) {

			CM_FATAL(); /* what? why? */
		}
		else {

			if( gsy->args.PrmInd.DiagCnt > GSY_PRM_IND_MAX_ENTRIES ) {
				CM_FATAL();
			}

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

			if( is_null(pd_channel) )
			{
				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "gsy-prm.IND but no pd-channel, ignored");
			}
			else
			{
				LSA_UINT8  sync_id = channel->lower.info.gsy.SyncId;

				CM_ASSERT (sync_id == GSY_SYNCID_CLOCK || sync_id == GSY_SYNCID_TIME);

				cm_pd_gsy_prm_indication_done(pd_channel, sync_id, &gsy->args.PrmInd);
			}
			}

			/***/

			/*
			gsy->args.PrmInd.EddPortId = unchanged (an in-parameter!)
			*/

			gsy->args.PrmInd.DiagCnt = 0; /* sanity (an out-paramter!) */

			cm_gsy_request_lower(channel, GSY_OPC_PRM_INDICATION_PROVIDE, 0, 0, gsy); /* re-provide */
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_prepare (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 sync_id
) {
	CM_LOG_FCT("cm_gsy_prm_prepare")
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (is_not_null(gsy));

	/* no args */

	cm_gsy_sync_request_lower (channel, GSY_OPC_PRM_PREPARE, sync_id, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_write (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT8				sync_id,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_LOG_FCT("cm_gsy_prm_write")
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (is_not_null(gsy));

	gsy->args.PrmRecord.EddPortId = edd_port_id;
	gsy->args.PrmRecord.Slot = slot_number;
	gsy->args.PrmRecord.Subslot = subslot_number;
	gsy->args.PrmRecord.RecordIndex = record_index;
	gsy->args.PrmRecord.RecordDataLen = record_data_length;

	/* record_data used as reference, check this */
	gsy->args.PrmRecord.pRecordData = CM_CAST_GSY_MEM_U8_PTR (record_data);

	cm_gsy_sync_request_lower (channel, GSY_OPC_PRM_WRITE, sync_id, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_end (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 sync_id
) {
	CM_LOG_FCT("cm_gsy_prm_end")
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (is_not_null(gsy));

	/* no args */

	cm_gsy_sync_request_lower(channel, GSY_OPC_PRM_END, sync_id, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_commit (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 sync_id
) {
	CM_LOG_FCT("cm_gsy_prm_commit")
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (is_not_null(gsy));

	/* no args */

	cm_gsy_sync_request_lower(channel, GSY_OPC_PRM_COMMIT, sync_id, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_gsy_prm_read (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT8				sync_id,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_LOG_FCT("cm_gsy_prm_read")
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	cm_gsy_alloc_rqb (channel, & gsy);
	if (is_null (gsy)) {
		return CM_ERR_ALLOC_LOWER;
	}

	gsy->args.PrmRecord.EddPortId = edd_port_id;
	gsy->args.PrmRecord.Slot = slot_number;
	gsy->args.PrmRecord.Subslot = subslot_number;
	gsy->args.PrmRecord.RecordIndex = record_index;
	gsy->args.PrmRecord.RecordDataLen = record_data_length;

	/* record_data used as reference, check this */
	gsy->args.PrmRecord.pRecordData = CM_CAST_GSY_MEM_U8_PTR (record_data);

	cm_gsy_sync_request_lower(channel, GSY_OPC_PRM_READ, sync_id, gsy);

	return CM_OK;
}

/*===========================================================================*/
/*=== GSY REQUEST LOWER =====================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
) {
	CM_LOG_FCT ("cm_gsy_request_lower")
	CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get(channel);

	CM_GSY_SET_USER_ID_HANDLE_DEV_AR (gsy, channel->my_handle, device_nr, ar_nr);

	CM_GSY_SET_HANDLE (gsy, gsy_channel->lower.handle);
	CM_GSY_SET_OPCODE (gsy, opcode);

	CM_GSY_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> gsy-rqb(0x%08x) h(%u) opc(%u) from-handle(%u) sync_id(%u)"
		, gsy, CM_GSY_GET_HANDLE (gsy)
		, CM_GSY_GET_OPCODE (gsy), CM_GSY_GET_USER_ID_HANDLE (gsy)
		, CM_GSY_GET_USER_ID_AR (gsy)
		);

	CM_GSY_REQUEST_LOWER (gsy, gsy_channel->sysptr);
}


static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_sync_request_lower (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT8 sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
) {
	CM_LOG_FCT ("cm_gsy_request_lower")

	if (channel->detailptr->path_type != CM_PATH_TYPE_PD) {
		CM_FATAL();
	}

	CM_GSY_SET_USER_ID_HANDLE_DEV_AR (gsy, channel->my_handle, 0/*device_nr*/, sync_id/*alias ar_nr*/);

	channel = cm_channel_find_gsy (channel->detailptr->nic_id, sync_id);

	if (is_null (channel)) {
		CM_FATAL1 (sync_id); /* a bug */
	}

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_GSY);

	CM_GSY_SET_HANDLE (gsy, channel->lower.handle);
	CM_GSY_SET_OPCODE (gsy, opcode);

	CM_GSY_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> gsy-rqb(0x%08x) gsy-opc(%u)  f-h(%u) sync_id(%u)"
		, gsy, CM_GSY_GET_OPCODE (gsy)
		, CM_GSY_GET_USER_ID_HANDLE (gsy), CM_GSY_GET_USER_ID_AR (gsy)
		);

	CM_GSY_REQUEST_LOWER (gsy, channel->sysptr);
}


/*===========================================================================*/
/*=== GSY LOWER DONE =======================================================*/
/*===========================================================================*/

LSA_VOID  CM_GSY_LOWER_IN_FCT_ATTR
cm_gsy_request_lower_done (
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy
) {
	CM_LOG_FCT ("cm_gsy_request_lower_done")
	CM_CHANNEL_PTR_TYPE  channel;
	LSA_UINT8  sync_id;

	if( is_null(gsy) ) {

		CM_GSY_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL,
			"is_null(gsy)"
			);

		CM_GSY_LOWER_RQB_ERROR (gsy);
		return;
	}

	channel = cm_channel_from_handle (CM_GSY_GET_USER_ID_HANDLE (gsy));

	if( is_null (channel) ) {

		CM_GSY_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"invalid handle(%u)",
			CM_GSY_GET_USER_ID_HANDLE (gsy)
			);

		CM_GSY_LOWER_RQB_ERROR (gsy);
		return;
	}

	/***/

	sync_id = CM_GSY_GET_USER_ID_AR (gsy); /* see cm_gsy_sync_request_lower() */

	/***/

	switch (CM_GSY_GET_RESPONSE (gsy)) {
	case GSY_RSP_OK:
	case GSY_RSP_OK_CANCEL:
		CM_GSY_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< gsy-rqb(0x%08x) gsy-opc(%u) gsy-rsp(%u)  t-h(%u) sync_id(%u)"
			, gsy, CM_GSY_GET_OPCODE (gsy), CM_GSY_GET_RESPONSE (gsy)
			, CM_GSY_GET_USER_ID_HANDLE (gsy), sync_id
			);
		break;

	default:
		CM_GSY_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "<<< gsy-rqb(0x%08x) gsy-opc(%u) gsy-rsp(%u)  t-h(%u) sync_id(%u)"
			, gsy, CM_GSY_GET_OPCODE (gsy), CM_GSY_GET_RESPONSE (gsy)
			, CM_GSY_GET_USER_ID_HANDLE (gsy), sync_id
			);
		break;
	}

	/***/

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_GSY:

		switch (CM_GSY_GET_OPCODE (gsy)) {

		case GSY_OPC_OPEN_CHANNEL:
			cm_gsy_channel_open_done (channel, gsy);
			break;

		case GSY_OPC_CLOSE_CHANNEL:
			cm_gsy_channel_close_done (channel, gsy);
			break;

		case GSY_OPC_CHANNEL_INFO_GET:
			cm_gsy_channel_info_done (channel, gsy);
			break;

		case GSY_OPC_DIAG_INDICATION_PROVIDE:
			cm_gsy_diag_indication (channel, gsy);
			break;

		case GSY_OPC_PRM_INDICATION_PROVIDE:
			cm_gsy_prm_indication (channel, gsy);
			break;

		default:
			CM_GSY_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled opcode(%u)",
				CM_GSY_GET_OPCODE (gsy)
				);
			CM_GSY_LOWER_RQB_ERROR (gsy); /* should not happen */
			break;
		}
		break;

	case CM_PATH_TYPE_PD:

		switch (CM_GSY_GET_OPCODE (gsy)) {

		case GSY_OPC_PRM_PREPARE:
			cm_pd_gsy_prm_prepare_done (channel, sync_id, gsy);
			break;

		case GSY_OPC_PRM_WRITE:
			cm_pd_gsy_prm_write_done (channel, sync_id, gsy);
			break;

		case GSY_OPC_PRM_END:
			cm_pd_gsy_prm_end_done (channel, sync_id, gsy);
			break;

		case GSY_OPC_PRM_COMMIT:
			cm_pd_gsy_prm_commit_done (channel, sync_id, gsy);
			break;

		case GSY_OPC_PRM_READ:
			cm_pd_gsy_prm_read_done (channel, sync_id, gsy);
			break;

		default:
			CM_GSY_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)"
				, CM_GSY_GET_OPCODE (gsy)
				);
			CM_GSY_LOWER_RQB_ERROR (gsy); /* should not happen */
			break;
		}
		break;

	default:
		CM_GSY_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "wrong path-type(%u)"
			, channel->detailptr->path_type
			);
		CM_GSY_LOWER_RQB_ERROR (gsy); /* should not happen */
		break;
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ GSY; CM_CFG_USE_GSY=" CM_STRINGIFY(CM_CFG_USE_GSY))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_GSY */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
