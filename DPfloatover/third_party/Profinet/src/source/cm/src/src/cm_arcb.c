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
/*  F i l e               &F: cm_arcb.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  arcb things common for client and multicast                              */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	11
#define CM_MODULE_ID		11

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcb_iocr_list_from_argr_iocr_list (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_ENTRY_PTR_TYPE  cb_iocr_list,
	CM_LIST_ENTRY_PTR_TYPE  ar_iocr_list
	)
{
	CM_LOG_FCT ("cm_arcb_iocr_list_from_argr_iocr_list")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

	CmListInitialize (cb_iocr_list);

	for (CmListForeach (iocr, ar_iocr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		CM_UPPER_ARCB_IOCR_PTR_TYPE  cb_iocr;

		if (   iocr->iocr_type == CM_IOCR_TYPE_MULTICAST_PROVIDER
			|| iocr->iocr_type == CM_IOCR_TYPE_MULTICAST_CONSUMER)
		{
			continue;
		}

		CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& cb_iocr)
			, cm_null_user_id, sizeof (*cb_iocr), channel->sysptr
			);
		if (is_null (cb_iocr)) {
			CM_AR_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "no mem");
			CM_FATAL ();
			return;
		}

		cb_iocr->iocr_type				= iocr->iocr_type;
		cb_iocr->iocr_ref				= iocr->iocr_ref;
		cb_iocr->iocr_properties		= iocr->iocr_properties;

		cb_iocr->frame_id				= iocr->frame_id;
		cb_iocr->data_length			= iocr->data_length;

		cb_iocr->send_clock				= iocr->send_clock;
		cb_iocr->reduction_ratio		= iocr->reduction_ratio;
		cb_iocr->phase					= iocr->phase;
		cb_iocr->frame_send_offset		= iocr->frame_send_offset;

		cb_iocr->data_hold_factor		= iocr->data_hold_factor;

		cb_iocr->iocr_tag_header		= 0xC000; /* const, see cm_ar_graph_insert_io_cr(), TIA 208236 */
		cb_iocr->iocr_multicast_mac		= iocr->multicast_mac;

		CmListInitialize (& cb_iocr->related_ioapi_list);

		cb_iocr->edd_properties			= iocr->cm.edd_properties;
		cb_iocr->edd_data_offset		= iocr->cm.edd_data_offset;
		cb_iocr->edd_cr_number			= iocr->cm.edd_cr_number;
		cb_iocr->edd_forward_mode		= iocr->cm.edd_forward_mode;
		cb_iocr->edd_ct_outdatastart	= iocr->cm.edd_ct_outdatastart;

		cb_iocr->apdu_status_length		= iocr->cm.apdu_status_length;
		cb_iocr->partial_offset			= iocr->cm.partial_offset;
		cb_iocr->partial_length			= iocr->cm.partial_length;

		cb_iocr->edd_apdustatus_offset	= iocr->cm.edd_as_offset;
		cb_iocr->edd_cw_offset			= iocr->cm.edd_cw_offset;
		cb_iocr->edd_datastatus_offset	= iocr->cm.edd_ds_offset;

		CmListInsertTail (cb_iocr_list, & cb_iocr->link);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcb_iocr_list_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_ENTRY_PTR_TYPE  cb_iocr_list
	)
{
	while (! CmListIsEmpty (cb_iocr_list))
	{
		CM_UPPER_ARCB_IOCR_PTR_TYPE  cb_iocr;
		LSA_UINT16  rc;

		CmListRemoveHead (cb_iocr_list, cb_iocr, CM_UPPER_ARCB_IOCR_PTR_TYPE);

		CM_FREE_UPPER_MEM (& rc, cb_iocr, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
	}
}

/*----------------------------------------------------------------------------*/
#endif /* CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS */


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
