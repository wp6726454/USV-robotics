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
/*  F i l e               &F: cm_edd.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EDD things                                                               */
/*                                                                           */
/*  - alloc EDD-RQB                                                          */
/*  - free EDD-RQB                                                           */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID 40
#define CM_MODULE_ID 40

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/

static LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_prm_indication(
    CM_CHANNEL_PTR_TYPE channel,
    CM_EDD_LOWER_RQB_PTR_TYPE edd /* provide: LSA_NULL */
    );

/*===========================================================================*/
/*=== EDD CHANNEL-INIT / -UNDO-INIT =========================================*/
/*===========================================================================*/

LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_channel_init(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_channel_init")

  if (channel->state != CM_CH_READY) {
    CM_FATAL(); /* see caller */
  }

  /* sanity-check */

  if (channel->lower.info.edd.MaxPortCnt < 1 /* at least 1 port */
      ||
      channel->lower.info.edd.MaxPortCnt >
          EDD_CFG_MAX_PORT_CNT /* wrong configuration */
      ) {
    CM_FATAL(); /* wrong configuration */
  }

  if (channel->lower.info.edd.CycleBaseFactor == 0) {
    CM_FATAL(); /* wrong configuration */
  }

#if CM_CFG_RT_CLASS_3_SUPPORT_ON
#if 0 /* check is wrong for N-IF/EDDI+EDDS (AP01207245) */
	if( (channel->lower.info.edd.HardwareType & EDD_HW_TYPE_FEATURE_TIMESTAMP_MSK) != EDD_HW_TYPE_FEATURE_TIMESTAMP_SUPPORT ) {
		CM_FATAL(); /* wrong configuration, IRT needs synchronization (which needs timestamping) */
	}
#endif
#endif

  /***/

  cm_edd_link_init(channel);

  /* provide some PRM indication resources (must do it in context of CM-EDD, not
   * CM-PD) */

  cm_edd_prm_indication(channel, LSA_NULL);
}

/*===========================================================================*/

LSA_UINT16 CM_LOCAL_FCT_ATTR
cm_edd_channel_undo_init(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_channel_undo_init")

  /* NOTE: caller wipes out channel->usr.xxx when returning CM_OK */

  if (channel->state != CM_CH_READY) {
    CM_FATAL(); /* see caller */
  }

  if (is_not_null(channel->lower.channel[CM_PATH_TYPE_PD])) {
    CM_FATAL(); /* cm-pd not unlinked */
  }

  cm_edd_link_undo_init(channel);

  return CM_OK;
}

/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE CM_LOCAL_FCT_ATTR
cm_edd_channel_get(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_channel_get")

  channel = channel->lower.channel[CM_PATH_TYPE_EDD];

  if (is_null(channel)) {
    CM_FATAL(); /* a bug */
  }

  return channel;
}

/*-----------------------------------------------------------------------------
//	can set send-clock
//---------------------------------------------------------------------------*/

LSA_BOOL CM_LOCAL_FCT_ATTR
cm_edd_can_set_send_clock(CM_CHANNEL_PTR_TYPE channel) {
  channel = cm_edd_channel_get(channel);

  switch ((channel->lower.info.edd.HardwareType & EDD_HW_TYPE_USED_MSK)) {
    case EDD_HW_TYPE_USED_STANDARD_MAC:
      return LSA_FALSE;

    case EDD_HW_TYPE_USED_ERTEC_400:
    case EDD_HW_TYPE_USED_ERTEC_200:
    case EDD_HW_TYPE_USED_SOC:
    case EDD_HW_TYPE_USED_ERTEC_200P:
      return LSA_TRUE;

    default:
      CM_FATAL(); /* unknown hardware, update! */
      return LSA_FALSE;
  }
}

/*-----------------------------------------------------------------------------
//	can "partial data access" (consumer-iocrs)
//---------------------------------------------------------------------------*/

LSA_BOOL CM_LOCAL_FCT_ATTR cm_edd_can_partial_data(CM_CHANNEL_PTR_TYPE channel,
                                                   LSA_UINT32 rt_class) {
  LSA_UINT32 mask;
  LSA_UINT32 supp;

  channel = cm_edd_channel_get(channel);

  switch (rt_class) {
    case CM_IOCR_PROP_RT_CLASS_1:
    case CM_IOCR_PROP_RT_CLASS_2:
      mask = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_MSK;
      supp = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_SUPPORT;
      break;

    case CM_IOCR_PROP_RT_CLASS_3:
      mask = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_MSK;
      supp = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_SUPPORT;
      break;

    case CM_IOCR_PROP_RT_CLASS_1_UDP:
      mask = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_MSK;
      supp = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_SUPPORT;
      break;

    default:
      CM_FATAL();       /* a bug, see caller */
      return LSA_FALSE; /* unreachable */
  }

  if ((channel->lower.info.edd.HardwareType & mask) == supp) {
    return LSA_TRUE;
  }

  return LSA_FALSE;
}

/*-----------------------------------------------------------------------------
//	can "autopadding" (provider-iocrs)
//---------------------------------------------------------------------------*/

LSA_BOOL CM_LOCAL_FCT_ATTR cm_edd_can_autopadding(CM_CHANNEL_PTR_TYPE channel,
                                                  LSA_UINT32 rt_class) {
  LSA_UINT32 mask;
  LSA_UINT32 supp;

  channel = cm_edd_channel_get(channel);

  switch (rt_class) {
    case CM_IOCR_PROP_RT_CLASS_1:
    case CM_IOCR_PROP_RT_CLASS_2:
      mask = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_MSK;
      supp = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_SUPPORT;
      break;

    case CM_IOCR_PROP_RT_CLASS_3:
      mask = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_MSK;
      supp = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_SUPPORT;
      break;

    case CM_IOCR_PROP_RT_CLASS_1_UDP:
      mask = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_MSK;
      supp = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_SUPPORT;
      break;

    default:
      CM_FATAL();       /* a bug, see caller */
      return LSA_FALSE; /* unreachable */
  }

  if ((channel->lower.info.edd.HardwareType & mask) == supp) {
    return LSA_TRUE;
  }

  return LSA_FALSE;
}

/*-----------------------------------------------------------------------------
//	get length of APDUStatus (0 if APDUStatus is not in front of the C_SDU)
//---------------------------------------------------------------------------*/

LSA_UINT16 CM_LOCAL_FCT_ATTR
cm_edd_get_apdu_status_length(CM_CHANNEL_PTR_TYPE channel) {
  LSA_UINT16 apdu_status_length;

  channel = cm_edd_channel_get(channel);

  if ((channel->lower.info.edd.HardwareType &
       EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE_MSK) ==
      EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE) {
    apdu_status_length =
        0; /* APDUStatus is in the KRAM, C_SDU is in the PAEARAM */
  } else {
    apdu_status_length = 4; /* APDUStatus and C_SDU are in the KRAM (first
                               APDUStatus, then C_SDU) */
  }

  return apdu_status_length;
}

/*-----------------------------------------------------------------------------
//	alloc lower EDD-RQB that fits for all requests
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_edd_alloc_rqb(CM_CHANNEL_PTR_TYPE channel,
                 EDD_SERVICE service, /* use 0 for open- and close-channel */
                 CM_EDD_LOWER_RQB_PTR_TYPE *lower_rqb_ptr_ptr) {
  CM_LOG_FCT("cm_edd_alloc_rqb")
  CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
  LSA_UINT16 length;

  CM_ASSERT(is_not_null(lower_rqb_ptr_ptr));

  /* alloc the RQB */
  CM_EDD_ALLOC_LOWER_RQB(lower_rqb_ptr_ptr, cm_null_user_id,
                         sizeof(CM_EDD_LOWER_RQB_TYPE), edd_channel->sysptr);

  if (is_null(*lower_rqb_ptr_ptr)) {
    CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
                    "h(%u) cannot alloc EDD RQB, len(%u)", channel->my_handle,
                    sizeof(CM_EDD_LOWER_RQB_TYPE));
    return;
  }

  switch (service) {
    case 0: /* special case, one for all, see open/close/prm */
      length = LSA_MAX(sizeof(EDD_RQB_OPEN_CHANNEL_TYPE),
                       sizeof(EDD_RQB_GET_LINK_STATUS_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_GET_PARAMS_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_GET_PORT_PARAMS_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_SENDCLOCK_CHANGE_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_PRM_INDICATION_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_PRM_PREPARE_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_PRM_WRITE_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_PRM_END_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_PRM_COMMIT_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_PRM_READ_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_NRT_SET_DCP_HELLO_FILTER_TYPE));
      length = LSA_MAX(length, sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE));
      break;

    case EDD_SRV_GET_LINK_STATUS:
      length = sizeof(EDD_RQB_GET_LINK_STATUS_TYPE);
      break;

    case EDD_SRV_GET_PARAMS:
      length = sizeof(EDD_RQB_GET_PARAMS_TYPE);
      break;

    case EDD_SRV_GET_PORT_PARAMS:
      length = sizeof(EDD_RQB_GET_PORT_PARAMS_TYPE);
      break;

    case EDD_SRV_SENDCLOCK_CHANGE:
      length = sizeof(EDD_RQB_SENDCLOCK_CHANGE_TYPE);
      break;

    case EDD_SRV_PRM_INDICATION:
      length = sizeof(EDD_RQB_PRM_INDICATION_TYPE);
      break;

    case EDD_SRV_PRM_PREPARE:
      length = sizeof(EDD_RQB_PRM_PREPARE_TYPE);
      break;

    case EDD_SRV_PRM_WRITE:
      length = sizeof(EDD_RQB_PRM_WRITE_TYPE);
      break;

    case EDD_SRV_PRM_END:
      length = sizeof(EDD_RQB_PRM_END_TYPE);
      break;

    case EDD_SRV_PRM_COMMIT:
      length = sizeof(EDD_RQB_PRM_COMMIT_TYPE);
      break;

    case EDD_SRV_PRM_READ:
      length = sizeof(EDD_RQB_PRM_READ_TYPE);
      break;

    case EDD_SRV_NRT_SET_DCP_HELLO_FILTER:
      length = sizeof(EDD_RQB_NRT_SET_DCP_HELLO_FILTER_TYPE);
      break;

    case EDD_SRV_LINK_STATUS_IND_PROVIDE:
    case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT: /* uses the same type */
      length = sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE);
      break;

    default:
      length = 0;
      CM_FATAL(); /* a bug */
      break;
  }

  CM_EDD_ALLOC_LOWER_MEM(CM_CAST_EDD_MEM_PTR_PTR(&(*lower_rqb_ptr_ptr)->pParam),
                         cm_null_user_id, length, edd_channel->sysptr);

  if (is_null((*lower_rqb_ptr_ptr)->pParam)) {
    LSA_UINT16 rc;

    CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
                    "h(%u) cannot alloc EDD MEM, len(%u)", channel->my_handle,
                    length);

    CM_EDD_FREE_LOWER_RQB(&rc, (*lower_rqb_ptr_ptr), edd_channel->sysptr);
    CM_ASSERT(rc == LSA_RET_OK);
    LSA_UNUSED_ARG(rc);

    *lower_rqb_ptr_ptr = LSA_NULL;
    return;
  }

#if CM_DEBUG
  CM_EDD_SET_NEXT_RQB_PTR(*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
  CM_EDD_SET_PREV_RQB_PTR(*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif
}

/*-----------------------------------------------------------------------------
//	free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_free_rqb(
    CM_CHANNEL_PTR_TYPE channel, CM_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr) {
  CM_LOG_FCT("cm_edd_free_rqb")
  CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
  LSA_UINT16 rc;

  CM_ASSERT(is_not_null(lower_rqb_ptr));
  CM_ASSERT(is_not_null(lower_rqb_ptr->pParam));

  CM_EDD_FREE_LOWER_MEM(&rc, lower_rqb_ptr->pParam, edd_channel->sysptr);
  CM_ASSERT(rc == LSA_RET_OK);

  CM_EDD_FREE_LOWER_RQB(&rc, lower_rqb_ptr, edd_channel->sysptr);
  CM_ASSERT(rc == LSA_RET_OK);

  LSA_UNUSED_ARG(rc);
}

/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_channel_open(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_channel_open")
  CM_EDD_LOWER_RQB_PTR_TYPE edd;

  CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_EDD);

  CM_ASSERT(are_equal(cm_edd_channel_get(channel), channel));

  CM_ASSERT(channel->state == CM_CH_OPEN);

  CM_ASSERT(channel->lower.handle == CM_EDD_INVALID_HANDLE);

  /***/

  cm_edd_alloc_rqb(channel, 0 /*open*/, &edd);

  if (is_null(edd)) {
    cm_channel_done(channel, CM_ERR_ALLOC_LOWER);
    return;
  }

  CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
                  "h(%u) sys_path(%u)", channel->my_handle,
                  channel->usr.ch.curr->args.channel.open->sys_path);

  {
    EDD_UPPER_OPEN_CHANNEL_PTR_TYPE args =
        CM_EDD_GET_PPARAM(edd, EDD_UPPER_OPEN_CHANNEL_PTR_TYPE);

    args->Handle = CM_EDD_INVALID_HANDLE;
    args->HandleUpper = channel->my_handle;
    args->SysPath = channel->usr.ch.curr->args.channel.open->sys_path;
    args->Cbf = cm_edd_request_lower_done;
  }

  CM_EDD_SET_USER_ID_HANDLE_DEV_AR(edd, channel->my_handle, 0, 0);

  CM_EDD_SET_HANDLE(edd, CM_EDD_INVALID_HANDLE);
  CM_EDD_SET_OPCODE(edd, EDD_OPC_OPEN_CHANNEL, 0);

  CM_EDD_OPEN_CHANNEL_LOWER(edd, channel->sysptr);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_channel_open_done(
    CM_CHANNEL_PTR_TYPE channel, CM_EDD_LOWER_RQB_PTR_TYPE edd) {
  CM_LOG_FCT("cm_edd_channel_open_done")
  LSA_UINT16 rc;

  CM_ASSERT(channel->state == CM_CH_OPEN);

  if (CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK) {
    EDD_UPPER_OPEN_CHANNEL_PTR_TYPE args =
        CM_EDD_GET_PPARAM(edd, EDD_UPPER_OPEN_CHANNEL_PTR_TYPE);

    channel->lower.handle = args->Handle;
    channel->lower.prealloc.edd = edd;

    CM_ASSERT(channel->lower.handle != CM_EDD_INVALID_HANDLE);

    CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
                    "h(%u) handle(%u)", channel->my_handle,
                    channel->lower.handle);

    rc = CM_OK;
  } else {
    CM_ASSERT(channel->lower.handle == CM_EDD_INVALID_HANDLE);

    CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
                    "h(%u) edd-rsp(%u)", channel->my_handle,
                    CM_EDD_GET_RESPONSE(edd));

    rc = CM_ERR_LOWER_LAYER;

    cm_edd_free_rqb(channel, edd);
  }

  cm_channel_done(channel, rc);
}

/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_channel_close(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_channel_close")
  CM_EDD_LOWER_RQB_PTR_TYPE edd;

  CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_EDD);

  CM_ASSERT(channel->state == CM_CH_CLOSE);

  CM_ASSERT(is_not_null(channel->lower.prealloc.edd));

  edd = channel->lower.prealloc.edd;

  CM_EDD_SET_USER_ID_HANDLE_DEV_AR(edd, channel->my_handle, 0, 0);

  CM_EDD_SET_HANDLE(edd, channel->lower.handle);
  CM_EDD_SET_OPCODE(edd, EDD_OPC_CLOSE_CHANNEL, 0);

  CM_EDD_CLOSE_CHANNEL_LOWER(edd, channel->sysptr);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_channel_close_done(
    CM_CHANNEL_PTR_TYPE channel, CM_EDD_LOWER_RQB_PTR_TYPE edd) {
  CM_LOG_FCT("cm_edd_close_channel_done")
  LSA_UINT16 rc;

  CM_ASSERT(channel->state == CM_CH_CLOSE);

  rc = CM_EDD_GET_RESPONSE(edd);

  if (rc == EDD_STS_OK) {
    cm_edd_free_rqb(channel, edd);

    channel->lower.handle = CM_EDD_INVALID_HANDLE;
    channel->lower.prealloc.edd = LSA_NULL;

    CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "h(%u)",
                    channel->my_handle);

    rc = CM_OK;
  } else {
    /* don't invalidate edd-handle, channel is not closed! */

    CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
                    "h(%u) edd-rsp(%u)", channel->my_handle, rc);

    rc = CM_ERR_LOWER_LAYER;
  }

  cm_channel_done(channel, rc);
}

/*===========================================================================*/
/*=== EDD INFO ==============================================================*/
/*===========================================================================*/

LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_channel_info(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_channel_info")
  CM_EDD_LOWER_RQB_PTR_TYPE edd;

  CM_ASSERT(channel->state == CM_CH_INFO);

  cm_edd_alloc_rqb(channel, EDD_SRV_GET_PARAMS, &edd);

  if (is_null(edd)) {
    cm_channel_done(channel, CM_ERR_ALLOC_LOWER);
    return;
  }

  cm_edd_request_lower(channel, EDD_SRV_GET_PARAMS, 0, 0, edd);
}

/*===========================================================================*/

static LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_channel_info_done(
    CM_CHANNEL_PTR_TYPE channel, CM_EDD_LOWER_RQB_PTR_TYPE edd) {
  CM_LOG_FCT("cm_edd_channel_info_done")
  LSA_UINT16 rc;

  CM_ASSERT(channel->state == CM_CH_INFO);

  rc = CM_EDD_GET_RESPONSE(edd);

  CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "h(%u) edd-rsp(%u)",
                  channel->my_handle, rc);

  if (rc == EDD_STS_OK) {
    EDD_UPPER_GET_PARAMS_PTR_TYPE args =
        CM_EDD_GET_PPARAM(edd, EDD_UPPER_GET_PARAMS_PTR_TYPE);

    channel->trace_idx = args->TraceIdx;

    channel->lower.info.edd.InterfaceID = args->InterfaceID;
    channel->lower.info.edd.HardwareType = args->HardwareType;
    channel->lower.info.edd.MACAddress = args->MACAddr;
    channel->lower.info.edd.MaxPortCnt = args->MaxPortCnt;
    channel->lower.info.edd.CycleBaseFactor = args->CycleBaseFactor;
    channel->lower.info.edd.MRPSupportedRole = args->MRPSupportedRole;

    rc = CM_OK;
  } else {
    rc = CM_ERR_LOWER_LAYER;
  }

  cm_edd_free_rqb(channel, edd);

  cm_channel_done(channel, rc);
}

/*===========================================================================*/
/*=== SENDCLOCK: RANGE-CHECK ================================================*/
/*===========================================================================*/

LSA_BOOL CM_LOCAL_FCT_ATTR cm_edd_send_clock_range_ok(
    CM_CHANNEL_PTR_TYPE channel, LSA_UINT16 send_clock,
    LSA_BOOL CM_LOCAL_MEM_ATTR
        *is_power_of_two /* LSA_NULL: do not allow non-power-of-two values */
    ) {
  CM_LOG_FCT("cm_edd_send_clock_range_ok")

  /***/

  if (send_clock <
      1) { /* note: SCF < 8 is supported since V5.0i2 (AP01035332) */

    return LSA_FALSE;
  }

  if (cm_edd_can_set_send_clock(channel)) {
    /* PNIO-Spec: The value range is from 1 to 128 (31.25 µs to 4000 µs) */

    if (send_clock > 128) {
      CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
                      "send_clock(%u) > 128 (4ms)", send_clock);

      return LSA_FALSE;
    }
  } else { /* Standard MAC */

    /* AP00527828: 256, 512 and 1024 shall be possible too */

    if (send_clock > 1024) {
      CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
                      "send_clock(%u) > 1024 (32ms)", send_clock);

      return LSA_FALSE;
    }
  }

  /***/

  switch (send_clock) {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
    case 512:
    case 1024:
      if (is_not_null(is_power_of_two)) {
        *is_power_of_two = LSA_TRUE;
      }
      break;

    default: /* non-power-of-two */

      if (is_null(is_power_of_two)) { /* flag: non-power-of-two values are not
                                         allowed */

        CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
                        "send_clock(%u) not a power of two", send_clock);

        return LSA_FALSE;
      }

      *is_power_of_two = LSA_FALSE;

      /***/

      if (!cm_edd_can_set_send_clock(channel)) {
        CM_EDD_TRACE_01(
            channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
            "send_clock(%u) non-power-of-two but cannot change the send_clock",
            send_clock);

        return LSA_FALSE;
      }
      break;
  }

  return LSA_TRUE;
}

/*===========================================================================*/
/*=== EDD SYNC_SENDCLOCK_CHANGE =============================================*/
/*===========================================================================*/

LSA_BOOL CM_LOCAL_FCT_ATTR cm_edd_set_send_clock(CM_CHANNEL_PTR_TYPE channel,
                                                 LSA_UINT16 send_clock) {
  CM_LOG_FCT("cm_edd_set_send_clock")
  CM_EDD_LOWER_RQB_PTR_TYPE edd;
  EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE args;

  /*
   * note: StandardMAC EDD (>= V3.12) supports this service too
   */

  cm_edd_alloc_rqb(channel, EDD_SRV_SENDCLOCK_CHANGE, &edd);

  if (is_null(edd)) {
    return LSA_FALSE;
  }

  args = CM_EDD_GET_PPARAM(edd, EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE);

  /***/

  if (send_clock == 0) {
    CM_ASSERT(channel->state == CM_CH_CLOSE);

    args->CycleBaseFactor = 0; /* special */
  } else {
    args->CycleBaseFactor = send_clock;
  }

  cm_edd_request_lower(channel, EDD_SRV_SENDCLOCK_CHANGE, 0, 0, edd);

  return LSA_TRUE; /* pending */
}

/*===========================================================================*/
/*=== PRM INTERFACE =========================================================*/
/*===========================================================================*/

CM_PORT_UINT CM_LOCAL_FCT_ATTR
cm_edd_prm_provide_mask(CM_CHANNEL_PTR_TYPE channel) {
  CM_PORT_UINT mask;

  channel = cm_edd_channel_get(channel);

  mask = ((CM_PORT_UINT)2 << channel->lower.info.edd.MaxPortCnt) - 1;

  return mask;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_prm_indication(
    CM_CHANNEL_PTR_TYPE channel,
    CM_EDD_LOWER_RQB_PTR_TYPE edd /* provide: LSA_NULL */
    ) {
  CM_LOG_FCT("cm_edd_prm_indication")

  if (is_null(edd)) { /* provide */

    CM_PORT_UINT provide_mask = 0;
    LSA_UINT16 port_id;

    for (port_id = 0; port_id <= channel->lower.info.edd.MaxPortCnt;
         ++port_id) {
      LSA_UINT16 max = 2 /*!*/;
      LSA_UINT16 cnt;

      for (cnt = 0; cnt < max; ++cnt) {
        cm_edd_alloc_rqb(channel, EDD_SRV_PRM_INDICATION, &edd);

        if (is_null(edd)) {
          CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                          "cannot edd-alloc");
          CM_FATAL();
        }

        {
          EDD_UPPER_PRM_INDICATION_PTR_TYPE ind =
              CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_INDICATION_PTR_TYPE);

          ind->edd_port_id = port_id; /* in-paramter! */

          ind->diag_cnt = 0; /* sanity (an out-paramter!) */
        }

        cm_edd_request_lower(channel, EDD_SRV_PRM_INDICATION, 0, 0, edd);
      }

      provide_mask |= ((CM_PORT_UINT)1 << port_id);
    }

    if (provide_mask != cm_edd_prm_provide_mask(channel)) {
      CM_FATAL(); /* a bug */
    }
  } else { /* handle indication and do re-provide */

    EDD_UPPER_PRM_INDICATION_PTR_TYPE ind =
        CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_INDICATION_PTR_TYPE);

    CM_ASSERT(is_not_null(ind));

    CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT,
                    "edd-prm.IND edd_port_id(%u) diag_cnt(%u)",
                    ind->edd_port_id, ind->diag_cnt);

    if (channel->state >= CM_CH_CLOSE) {
      CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
                      "channel is closing, freeing rqb");

      CM_ASSERT(CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK ||
                CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK_CANCEL);

      cm_edd_free_rqb(channel, edd);
    } else if (CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK) {
      CM_FATAL(); /* what? why? */
    } else {
      if (ind->diag_cnt > EDD_SRV_PRM_INDICATION_DIAG_MAX_ENTRIES) {
        CM_FATAL();
      }

      /***/

      {
        CM_CHANNEL_PTR_TYPE pd_channel =
            channel->lower.channel[CM_PATH_TYPE_PD];

        if (is_null(pd_channel)) {
          CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT,
                          "edd-prm.IND but no pd-channel, ignored");
        } else {
          cm_pd_edd_prm_indication_done(pd_channel, ind);
        }
      }

      /***/

      /*
      ind->edd_port_id = unchanged (an in-paramter!)
      */

      ind->diag_cnt = 0; /* sanity (an out-paramter!) */

      cm_edd_request_lower(channel, EDD_SRV_PRM_INDICATION, 0, 0,
                           edd); /* re-provide */
    }
  }
}

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_prm_prepare(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_prm_prepare")
  CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
  CM_EDD_LOWER_RQB_PTR_TYPE edd = edd_channel->lower.prealloc.edd;
  EDD_UPPER_PRM_PREPARE_PTR_TYPE param;

  CM_ASSERT(is_not_null(edd));

  param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_PREPARE_PTR_TYPE);

  param->dummy = 1;

  cm_edd_request_lower(channel, EDD_SRV_PRM_PREPARE, 0, 0, edd);
}

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_prm_write(
    CM_CHANNEL_PTR_TYPE channel, LSA_BOOL is_local, LSA_UINT16 edd_port_id,
    LSA_UINT16 slot_number, LSA_UINT16 subslot_number, LSA_UINT32 record_index,
    LSA_UINT32 record_data_length, CM_UPPER_MEM_PTR_TYPE record_data) {
  CM_LOG_FCT("cm_edd_prm_write")

  CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
  CM_EDD_LOWER_RQB_PTR_TYPE edd = edd_channel->lower.prealloc.edd;

  EDD_UPPER_PRM_WRITE_PTR_TYPE param;

  CM_ASSERT(is_not_null(edd));

  param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_WRITE_PTR_TYPE);

  param->Local = is_local;
  param->edd_port_id = edd_port_id;
  param->slot_number = slot_number;
  param->subslot_number = subslot_number;
  param->record_index = record_index;
  param->record_data_length = record_data_length;

  /* record_data used as reference, check this memory attr */
  param->record_data = CM_CAST_EDD_MEM_U8_PTR(record_data);

  cm_edd_request_lower(channel, EDD_SRV_PRM_WRITE, 0, 0, edd);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 CM_LOCAL_FCT_ATTR cm_edd_prm_read(
    CM_CHANNEL_PTR_TYPE channel, LSA_UINT16 edd_port_id, LSA_UINT16 slot_number,
    LSA_UINT16 subslot_number, LSA_UINT32 record_index,
    LSA_UINT32 record_data_length, CM_UPPER_MEM_PTR_TYPE record_data) {
  CM_LOG_FCT("cm_edd_prm_read")
  CM_EDD_LOWER_RQB_PTR_TYPE edd;
  EDD_UPPER_PRM_READ_PTR_TYPE param;

  cm_edd_alloc_rqb(channel, EDD_SRV_PRM_READ, &edd);
  if (is_null(edd)) {
    return CM_ERR_ALLOC_LOWER;
  }

  param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_READ_PTR_TYPE);

  param->edd_port_id = edd_port_id;
  param->slot_number = slot_number;
  param->subslot_number = subslot_number;
  param->record_index = record_index;
  param->record_data_length = record_data_length;

  /* record_data used as reference, check this */
  param->record_data = CM_CAST_EDD_MEM_U8_PTR(record_data);

  cm_edd_request_lower(channel, EDD_SRV_PRM_READ, 0, 0, edd);
  return CM_OK;
}

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_prm_end(CM_CHANNEL_PTR_TYPE channel,
                                          LSA_BOOL is_local) {
  CM_LOG_FCT("cm_edd_prm_end")
  CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
  CM_EDD_LOWER_RQB_PTR_TYPE edd = edd_channel->lower.prealloc.edd;
  EDD_UPPER_PRM_END_PTR_TYPE param;

  CM_ASSERT(is_not_null(edd));

  param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_END_PTR_TYPE);

  param->Local = is_local;

  cm_edd_request_lower(channel, EDD_SRV_PRM_END, 0, 0, edd);
}

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_prm_commit(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_prm_commit")
  CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
  CM_EDD_LOWER_RQB_PTR_TYPE edd = edd_channel->lower.prealloc.edd;
  EDD_UPPER_PRM_COMMIT_PTR_TYPE param;

  CM_ASSERT(is_not_null(edd));

  param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_COMMIT_PTR_TYPE);

  /* sanity: set out-params to zero */
  param->RsvIntervalRed.RxEndNs = 0;
  param->RsvIntervalRed.RxLocalTransferEndNs = 0;
  param->RsvIntervalRed.TxEndNs = 0;
  param->RsvIntervalRed.TxLocalTransferStartNs = 0;
  param->RsvIntervalRed.TxLocalTransferEndNs = 0;

  cm_edd_request_lower(channel, EDD_SRV_PRM_COMMIT, 0, 0, edd);
}

/*===========================================================================*/
/*=== EDD_SRV_NRT_SET_DCP_HELLO_FILTER ======================================*/
/*===========================================================================*/

#if CM_CFG_MAX_CLIENTS

LSA_UINT16 CM_LOCAL_FCT_ATTR cm_edd_set_dcp_hello_filter(
    CM_CHANNEL_PTR_TYPE channel, LSA_UINT8 mode, LSA_UINT32 user_id,
    EDD_UPPER_MEM_U8_PTR_TYPE name_of_station,
    LSA_UINT32 name_of_station_length) {
  CM_LOG_FCT("cm_edd_set_dcp_hello_filter")
  CM_EDD_LOWER_RQB_PTR_TYPE edd;
  EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE sf;

  cm_edd_alloc_rqb(channel, EDD_SRV_NRT_SET_DCP_HELLO_FILTER, &edd);

  if (is_null(edd)) {
    return CM_ERR_ALLOC_LOWER;
  }

  sf = CM_EDD_GET_PPARAM(edd, EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE);

  sf->Mode = mode;
  sf->UserID = user_id;
  sf->pNameOfStation = name_of_station;
  sf->NameOfStationLen = name_of_station_length;

  cm_edd_request_lower(channel, EDD_SRV_NRT_SET_DCP_HELLO_FILTER, 0, 0, edd);

  return CM_OK_PENDING;
}

#endif

/*===========================================================================*/
/*=== GET-LINK-STATUS =======================================================*/
/*===========================================================================*/

LSA_BOOL CM_LOCAL_FCT_ATTR cm_edd_get_link_status(CM_CHANNEL_PTR_TYPE channel) {
  CM_LOG_FCT("cm_edd_get_link_status")
  CM_EDD_LOWER_RQB_PTR_TYPE edd;

  cm_edd_alloc_rqb(channel, EDD_SRV_GET_LINK_STATUS, &edd);

  if (is_null(edd)) {
    return LSA_FALSE;
  } else {
    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE param =
        CM_EDD_GET_PPARAM(edd, EDD_UPPER_GET_LINK_STATUS_PTR_TYPE);

    param->PortID = EDD_PORT_ID_AUTO;

    cm_edd_request_lower(channel, EDD_SRV_GET_LINK_STATUS, 0, 0, edd);

    return LSA_TRUE;
  }
}

/*===========================================================================*/
/*=== EDD REQUEST LOWER =====================================================*/
/*===========================================================================*/

LSA_VOID CM_LOCAL_FCT_ATTR cm_edd_request_lower(CM_CHANNEL_PTR_TYPE channel,
                                                EDD_SERVICE service,
                                                LSA_UINT16 device_nr,
                                                LSA_UINT16 ar_nr,
                                                CM_EDD_LOWER_RQB_PTR_TYPE edd) {
  CM_LOG_FCT("cm_edd_request_lower")
  CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);

  CM_EDD_SET_USER_ID_HANDLE_DEV_AR(edd, channel->my_handle, device_nr, ar_nr);

  CM_EDD_SET_HANDLE(edd, edd_channel->lower.handle);
  CM_EDD_SET_OPCODE(edd, EDD_OPC_REQUEST, service);

  CM_EDD_TRACE_06(
      channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
      ">>> edd-rqb(0x%08x) edd-h(%u) edd-svc(0x%x) f-h(%u) f-dev(%u) f-ar(%u)",
      edd, CM_EDD_GET_HANDLE(edd), CM_EDD_GET_SERVICE(edd),
      CM_EDD_GET_USER_ID_HANDLE(edd), CM_EDD_GET_USER_ID_DEV(edd),
      CM_EDD_GET_USER_ID_AR(edd));

  CM_EDD_REQUEST_LOWER(edd, edd_channel->sysptr);
}

/*===========================================================================*/
/*=== EDD LOWER DONE =======================================================*/
/*===========================================================================*/

LSA_VOID CM_EDD_LOWER_IN_FCT_ATTR
cm_edd_request_lower_done(CM_EDD_LOWER_RQB_PTR_TYPE edd) {
  CM_LOG_FCT("cm_edd_request_lower_done")
  CM_CHANNEL_PTR_TYPE channel;

  if (is_null(edd)) {
    CM_EDD_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "is_null(edd)");

    CM_EDD_LOWER_RQB_ERROR(edd);
    return;
  }

  channel = cm_channel_from_handle(CM_EDD_GET_USER_ID_HANDLE(edd));

  if (is_null(channel)) {
    CM_EDD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "invalid handle(%u)",
                    CM_EDD_GET_USER_ID_HANDLE(edd));

    CM_EDD_LOWER_RQB_ERROR(edd);
    return;
  }

  /***/

  switch (CM_EDD_GET_RESPONSE(edd)) {
    case EDD_STS_OK:
    case EDD_STS_OK_CANCEL:
      CM_EDD_TRACE_06(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
                      "<<< edd-rqb(0x%08x) edd-svc(0x%x) edd-rsp(%u) t-h(%u) "
                      "t-dev(%u) t-ar(%u)",
                      edd, CM_EDD_GET_SERVICE(edd), CM_EDD_GET_RESPONSE(edd),
                      CM_EDD_GET_USER_ID_HANDLE(edd),
                      CM_EDD_GET_USER_ID_DEV(edd), CM_EDD_GET_USER_ID_AR(edd));
      break;

    default:
      CM_EDD_TRACE_06(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
                      "<<< edd-rqb(0x%08x) edd-svc(0x%x) edd-rsp(%u) t-h(%u) "
                      "t-dev(%u) t-ar(%u)",
                      edd, CM_EDD_GET_SERVICE(edd), CM_EDD_GET_RESPONSE(edd),
                      CM_EDD_GET_USER_ID_HANDLE(edd),
                      CM_EDD_GET_USER_ID_DEV(edd), CM_EDD_GET_USER_ID_AR(edd));
      break;
  }

  /***/

  switch (channel->detailptr->path_type) {
    case CM_PATH_TYPE_EDD:

      switch (CM_EDD_GET_OPCODE(edd)) {
        case EDD_OPC_OPEN_CHANNEL:
          cm_edd_channel_open_done(channel, edd);
          break;

        case EDD_OPC_CLOSE_CHANNEL:
          cm_edd_channel_close_done(channel, edd);
          break;

        case EDD_OPC_REQUEST:

          switch (CM_EDD_GET_SERVICE(edd)) {
            case EDD_SRV_GET_PARAMS:
              cm_edd_channel_info_done(channel, edd);
              break;

            case EDD_SRV_PRM_INDICATION:
              cm_edd_prm_indication(channel, edd);
              break;

            case EDD_SRV_SENDCLOCK_CHANGE: /* "set-send-clock 0" before closing
                                              the edd-channel */
              if (CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK) {
                CM_FATAL();
              } else {
                cm_edd_free_rqb(channel, edd);

                CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT,
                                "h(%u) set-sendclock(0) done",
                                channel->my_handle);

                cm_edd_channel_close(channel);
              }
              break;

            case EDD_SRV_LINK_STATUS_IND_PROVIDE:
            case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
              cm_edd_link_indication(channel, edd);
              break;

            default:
              CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                              "unhandled service(%u)", CM_EDD_GET_SERVICE(edd));
              CM_EDD_LOWER_RQB_ERROR(edd); /* should not happen */
              break;
          }
          break;

        default:
          CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                          "unhandled opcode(%u)", CM_EDD_GET_OPCODE(edd));
          CM_EDD_LOWER_RQB_ERROR(edd); /* should not happen */
          break;
      }
      break;

    case CM_PATH_TYPE_PD:

      switch (CM_EDD_GET_OPCODE(edd)) {
        case EDD_OPC_REQUEST:

          switch (CM_EDD_GET_SERVICE(edd)) {
            case EDD_SRV_GET_PORT_PARAMS:
              cm_pd_get_port_params_done(channel, edd);
              break;

            case EDD_SRV_SENDCLOCK_CHANGE: {
              LSA_UINT16 rsp = CM_EDD_GET_RESPONSE(edd);
              EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE param =
                  CM_EDD_GET_PPARAM(edd, EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE);
              LSA_UINT16 send_clock = param->CycleBaseFactor;

              cm_edd_free_rqb(channel, edd);

              cm_pd_set_send_clock_done(channel, rsp, send_clock);
            } break;

            case EDD_SRV_PRM_PREPARE:
              cm_pd_edd_prm_prepare_done(channel, edd);
              break;

            case EDD_SRV_PRM_WRITE:
              cm_pd_edd_prm_write_done(channel, edd);
              break;

            case EDD_SRV_PRM_END:
              cm_pd_edd_prm_end_done(channel, edd);
              break;

            case EDD_SRV_PRM_COMMIT:
              cm_pd_edd_prm_commit_done(channel, edd);
              break;

            case EDD_SRV_PRM_READ:
              cm_pd_edd_prm_read_done(channel, edd);
              break;

            case EDD_SRV_GET_LINK_STATUS:
              cm_pd_edd_prm_link_status_done(channel, edd);
              break;

            default:
              CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                              "unhandled service(%u)", CM_EDD_GET_SERVICE(edd));
              CM_EDD_LOWER_RQB_ERROR(edd); /* should not happen */
              break;
          }
          break;

        default:
          CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                          "unhandled opcode(%u)", CM_EDD_GET_OPCODE(edd));
          CM_EDD_LOWER_RQB_ERROR(edd); /* should not happen */
          break;
      }
      break;

#if CM_CFG_MAX_CLIENTS

    case CM_PATH_TYPE_CLIENT:

      switch (CM_EDD_GET_OPCODE(edd)) {
        case EDD_OPC_REQUEST:

          switch (CM_EDD_GET_SERVICE(edd)) {
            case EDD_SRV_NRT_SET_DCP_HELLO_FILTER: {
              EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE sf;

              sf = CM_EDD_GET_PPARAM(
                  edd, EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE);

              cm_cl_edd_set_dcp_hello_filter_done(channel,
                                                  CM_EDD_GET_RESPONSE(edd), sf);

              cm_edd_free_rqb(channel, edd);
            } break;

            default:
              CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                              "unhandled service(%u)", CM_EDD_GET_SERVICE(edd));
              CM_EDD_LOWER_RQB_ERROR(edd); /* should not happen */
              break;
          }
          break;

        default:
          CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                          "unhandled opcode(%u)", CM_EDD_GET_OPCODE(edd));
          CM_EDD_LOWER_RQB_ERROR(edd); /* should not happen */
          break;
      }
      break;
#endif

    default:
      CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
                      "wrong path-type(%u)", channel->detailptr->path_type);
      CM_EDD_LOWER_RQB_ERROR(edd); /* should not happen */
      break;
  }
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
