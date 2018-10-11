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
/*  F i l e               &F: cm_edd2.c                                 :F&  */
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
/*  - link status indication                                                 */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	402
#define CM_MODULE_ID		402

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_init (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_edd_link_init")

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_EDD);

	{
	LSA_INT  alloc_size = sizeof(*channel->usr.edd.ports) * (channel->lower.info.edd.MaxPortCnt + 1);
	CM_ASSERT (alloc_size < 0xFFFF);
	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& channel->usr.edd.ports), (LSA_UINT16)alloc_size);
	if (is_null (channel->usr.edd.ports))
	{
		CM_FATAL();
	}
	}

#if EDD_PORT_ID_AUTO != 0/*port_id*/
# error "EDD_PORT_ID_AUTO not 0"
#endif

	{
	LSA_UINT16  port_id;
	for (port_id = 0/*auto*/; port_id <= channel->lower.info.edd.MaxPortCnt; ++port_id)
	{
		CM_EDD_PORT_STATUS_PTR_TYPE  ps = & channel->usr.edd.ports[port_id];

		ps->status          = EDD_LINK_UNKNOWN;
		ps->speed           = EDD_LINK_UNKNOWN;
		ps->mode            = EDD_LINK_UNKNOWN;
		ps->irt_port_status = EDD_IRT_NOT_SUPPORTED;
		ps->autoneg         = EDD_AUTONEG_UNKNOWN;

		{
		CM_EDD_LOWER_RQB_PTR_TYPE  edd;
		EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  link;

		cm_edd_alloc_rqb (channel, EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT, &edd);
		if (is_null(edd))
		{
			CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot edd-alloc");
			CM_FATAL();
		}

		link = CM_EDD_GET_PPARAM (edd, EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE);
		link->PortID = port_id;
		link->Status = ps->status;
		link->Speed = ps->speed;
		link->Mode = ps->mode;
		link->IRTPortStatus = ps->irt_port_status;
		link->Autoneg = ps->autoneg;

		if (port_id == EDD_PORT_ID_AUTO)
		{
			cm_edd_request_lower (channel, EDD_SRV_LINK_STATUS_IND_PROVIDE, 0, 0, edd);
		}
		else
		{
			cm_edd_request_lower (channel, EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT, 0, 0, edd);
		}
		}
	}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_edd_link_undo_init")
	LSA_UINT16  rc;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_EDD);


	CM_FREE_LOCAL_MEM (& rc, channel->usr.edd.ports);
	CM_ASSERT (rc == LSA_OK);
	channel->usr.edd.ports = LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_data_changed (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  port_id,
	LSA_UINT8  status,
	LSA_UINT8  speed,
	LSA_UINT8  mode,
	LSA_UINT8  irt_port_status,
	LSA_UINT8  autoneg
	)
{
	CM_LOG_FCT("cm_edd_link_data_changed")
	CM_EDD_PORT_STATUS_PTR_TYPE  ps = & channel->usr.edd.ports[port_id];
	CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];
	LSA_BOOL  link_changed;
	LSA_UINT8 prev_status;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_EDD);

	if (port_id > channel->lower.info.edd.MaxPortCnt)
	{
		CM_FATAL();
	}

	prev_status = ps->status; /* save */

	if (ps->status != status  ||  ps->speed != speed  ||  ps->mode != mode  ||  ps->autoneg != autoneg)
	{
		CM_EDD_TRACE_09 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "port_id(%u) Status/Speed/Mode/AutoNeg changed: old(%u/%u/%u/%u) -> new(%u/%u/%u/%u)"
			, port_id
			, ps->status, ps->speed, ps->mode, ps->autoneg, status, speed, mode, autoneg
			);

		link_changed = LSA_TRUE;

		ps->status  = status;
		ps->speed   = speed;
		ps->mode    = mode;
		ps->autoneg = autoneg;
	}
	else
	{
		link_changed = LSA_FALSE;
	}

	/***/

	if (link_changed)
	{
		if (is_not_null (pd_channel))
		{
				/* internal */
			if (port_id == EDD_PORT_ID_AUTO)
			{
				cm_pd_link_status_changed (pd_channel, ps->status, prev_status);
			}

				/* user: CM_OPC_PD_LINK_STATUS_INFO.IND */
			cm_pd_link_status_trigger_user (pd_channel, port_id);
		}
	}

	if (irt_port_status != 0xFF) /* means unknown, see caller */
	{
		CM_ASSERT (EDD_IRT_NOT_SUPPORTED != 0xFF);
		CM_ASSERT (EDD_IRT_PORT_INACTIVE != 0xFF);
		CM_ASSERT (EDD_IRT_PORT_ACTIVE_TX_UP != 0xFF);
		CM_ASSERT (EDD_IRT_PORT_ACTIVE_TX_RX != 0xFF);

		if (ps->irt_port_status != irt_port_status)
		{
			CM_EDD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "port_id(%u) EDD-IRTPortStatus changed: old(%u) new(%u)"
				, port_id, ps->irt_port_status, irt_port_status
				);

			ps->irt_port_status = irt_port_status;

			/***/

			if (is_not_null (pd_channel))
			{
				cm_pd_irt_port_status_changed(pd_channel, port_id, ps->irt_port_status);
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
	)
{
	CM_LOG_FCT("cm_edd_link_indication")

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_EDD);
	CM_ASSERT (is_not_null(edd));

	if (channel->state >= CM_CH_CLOSE)
	{
		CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "channel is closing, freeing rqb");

		CM_ASSERT (CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK
			|| CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK_CANCEL
			);
		cm_edd_free_rqb (channel, edd);
	}
	else if (CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK)
	{
		CM_FATAL1(CM_EDD_GET_RESPONSE(edd)); /* what? why? */
	}
	else
	{
		EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  link;

		link = CM_EDD_GET_PPARAM (edd, EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE);
		CM_ASSERT (is_not_null(link));

		if (link->PortID == EDD_PORT_ID_AUTO)
		{
			cm_edd_link_data_changed (channel
				, link->PortID, link->Status, link->Speed, link->Mode, 0xFF, EDD_AUTONEG_UNKNOWN
				);

			cm_edd_request_lower (channel, EDD_SRV_LINK_STATUS_IND_PROVIDE, 0, 0, edd);
		}
		else
		{
			cm_edd_link_data_changed (channel
				, link->PortID, link->Status, link->Speed, link->Mode, link->IRTPortStatus, link->Autoneg
				);

			cm_edd_request_lower (channel, EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT, 0, 0, edd);
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_link_status_is_up (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_LOG_FCT("cm_link_status_is_up")
	CM_CHANNEL_PTR_TYPE  edd_channel;

	edd_channel = cm_edd_channel_get (channel);
	CM_ASSERT (is_not_null (edd_channel->usr.edd.ports));

	if (edd_channel->usr.edd.ports[0].status == EDD_LINK_UP)
	{
		CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "yes, link is up");
		return LSA_TRUE;
	}

	CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "no, link is down or unknown");
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_link_status_matches_ar_properties (
	CM_CHANNEL_PTR_TYPE channel,
	EDD_UPPER_GET_LINK_STATUS_PTR_TYPE edd_link_status,
	LSA_UINT32 ar_properties
	)
{
	CM_LOG_FCT("cm_link_status_matches_ar_properties")
	LSA_UINT8 status;
	LSA_UINT8 speed;
	LSA_UINT8 mode;

	if( is_not_null(edd_link_status) ) {

		status = edd_link_status->Status;
		speed  = edd_link_status->Speed;
		mode   = edd_link_status->Mode;
	}
	else {

		CM_CHANNEL_PTR_TYPE  edd_channel = cm_edd_channel_get (channel);
		CM_EDD_PORT_STATUS_PTR_TYPE  ps = & edd_channel->usr.edd.ports[0/*PortID-auto*/];

		status = ps->status;
		speed  = ps->speed;
		mode   = ps->mode;
	}

	/***/

	CM_EDD_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "ar_prop(0x%x) edd-status(%u) edd-speed(%u) edd-mode(%u)"
		, ar_properties, status, speed, mode
		);

	/***/

	if( ! (status == EDD_LINK_UP || status == EDD_LINK_UP_CLOSED) ) { /* TIA 1103362: "closed" is ok too (lasts < 100ms) */

		CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "link not up or closed"
			);
		return LSA_FALSE;
	}

	if( mode != EDD_LINK_MODE_FULL ) {

		CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "link not full duplex"
			);
		return LSA_FALSE;
	}

	/* up and full duplex */

	LSA_UNUSED_ARG(ar_properties); /* note: the field "DataRate" was removed from the ar_properties */

	if(
		speed == EDD_LINK_UNKNOWN /* note: always false, see cm_edd_link_indication() */
		|| speed == EDD_LINK_SPEED_10
	) {

		CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "link speed(%u) not 100 Mb/s fdx or more"
			, speed
			);
		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
