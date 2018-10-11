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
/*  F i l e               &F: cm_oha.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  OHA (option handler) things                                              */
/*                                                                           */
/*  - alloc OHA-RQB                                                          */
/*  - free OHA-RQB                                                           */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	50
#define CM_MODULE_ID		50

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*---------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha /* provide: LSA_NULL */
);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_hello_indication(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha /* provide: LSA_NULL */
);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_address_indication(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha /* provide: LSA_NULL */
);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_OHA_LOWER_RQB_PTR_TYPE edd
);


/*===========================================================================*/
/*=== OHA CHANNEL-INIT / -UNDO-INIT =========================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_channel_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_oha_channel_init")

	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_OHA);

	/***/

	channel->usr.oha.name_of_station_length = 0;

	CM_MEMSET(&channel->usr.oha.IPSuite, 0, sizeof(channel->usr.oha.IPSuite)); /* no current IP suite (0/0/0) */
	channel->usr.oha.is_IPAddr_null = LSA_TRUE;

	channel->usr.oha.is_IPSuite_conflicting = LSA_FALSE;
	channel->usr.oha.is_IPSuite_emergency   = LSA_FALSE;

	/* provide some ip-address-indication resources (must do it in context of CM-OHA, not CM-CL) */

	cm_oha_address_indication (channel, LSA_NULL);

	/* provide some prm-indication resources (must do it in context of CM-OHA, not CM-PD) */

	cm_oha_prm_indication (channel, LSA_NULL);

	/* provide some hello-indication resources (must do it in context of CM-OHA, not CM-CL) */

	cm_oha_hello_indication (channel, LSA_NULL);
}

/*===========================================================================*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_oha_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_oha_channel_undo_init")

	/* NOTE: caller wipes out channel->usr.xxx when returning CM_OK */

	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	if( is_not_null(channel->lower.channel[CM_PATH_TYPE_PD]) ) {
		CM_FATAL(); /* cm-pd not unlinked */
	}

	if( is_not_null(channel->lower.channel[CM_PATH_TYPE_SERVER]) ) {
		CM_FATAL(); /* cm-sv not unlinked */
	}

	return CM_OK;
}


/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/
CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_oha_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_oha_channel_get")

	switch( channel->detailptr->path_type ) {
	case CM_PATH_TYPE_OHA:
	case CM_PATH_TYPE_PD:
	case CM_PATH_TYPE_CLIENT:
	case CM_PATH_TYPE_MULTICAST:
	case CM_PATH_TYPE_SERVER:
		channel = channel->lower.channel[CM_PATH_TYPE_OHA];
		break;

	default:
		CM_FATAL1(channel->detailptr->path_type);
		channel = LSA_NULL;
		break;
	}

	if( is_null(channel) ) {
		CM_FATAL(); /* a bug */
	}

	return channel;
}


/*-----------------------------------------------------------------------------
//	alloc lower OHA-RQB that fits for all requests
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	*lower_rqb_ptr_ptr
)
{
	CM_LOG_FCT ("cm_oha_alloc_rqb")
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(channel);

		/* alloc the RQB */
	CM_OHA_ALLOC_LOWER_RQB (lower_rqb_ptr_ptr, cm_null_user_id, sizeof (CM_OHA_LOWER_RQB_TYPE), oha_channel->sysptr);

	if (is_null (*lower_rqb_ptr_ptr)) {
		CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) cannot alloc OHA RQB, len(%u)"
			, channel->my_handle, sizeof (CM_OHA_LOWER_RQB_TYPE)
			);
		return;
	}

#if CM_DEBUG
	CM_OHA_SET_NEXT_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
	CM_OHA_SET_PREV_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif
}


/*-----------------------------------------------------------------------------
//	free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
)
{
	CM_LOG_FCT ("cm_oha_free_rqb")
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(channel);
	LSA_UINT16	rc;

	CM_OHA_FREE_LOWER_RQB (
		& rc,
		lower_rqb_ptr,
		oha_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
}


/*-----------------------------------------------------------------------------
//	memcmp (because there is no CM_MEMCMP)
//---------------------------------------------------------------------------*/
static LSA_INT32  CM_LOCAL_FCT_ATTR
cm_oha_mem_compare (
	CM_COMMON_MEM_U8_PTR_TYPE  m1,
	CM_COMMON_MEM_U8_PTR_TYPE  m2,
	LSA_UINT16  length
	)
{
	LSA_UINT16  i;

	for (i = 0; i < length; ++i)
	{
		if (m1[i] != m2[i])
		{
			return 1;
		}
	}
	return 0;
}


/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_LOG_FCT ("cm_oha_channel_open")
	CM_OHA_LOWER_RQB_PTR_TYPE  oha;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_OHA);

	CM_ASSERT (are_equal(cm_oha_channel_get(channel), channel));

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == CM_OHA_INVALID_HANDLE);

	/***/

	cm_oha_alloc_rqb (channel, & oha);

	if (is_null (oha)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	oha->args.channel.handle = CM_OHA_INVALID_HANDLE;
	oha->args.channel.handle_upper = channel->my_handle;
	oha->args.channel.sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	oha->args.channel.oha_request_upper_done_ptr = cm_oha_request_lower_done;

	CM_OHA_SET_USER_ID_HANDLE_DEV_AR (oha, channel->my_handle, 0, 0);

	CM_OHA_SET_HANDLE (oha, CM_OHA_INVALID_HANDLE);
	CM_OHA_SET_OPCODE (oha, OHA_OPC_OPEN_CHANNEL);

	CM_OHA_OPEN_CHANNEL_LOWER (oha, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha
)
{
	CM_LOG_FCT ("cm_oha_channel_open_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	rc = CM_OHA_GET_RESPONSE (oha);

	if (rc == OHA_OK) {

		channel->lower.handle = oha->args.channel.handle;
		channel->lower.prealloc.oha = oha;

		CM_ASSERT(channel->lower.handle != CM_OHA_INVALID_HANDLE);

		CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) oha-handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		rc = CM_OK;
	}
	else {

		CM_ASSERT(channel->lower.handle == CM_OHA_INVALID_HANDLE);

		CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) oha-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;

		cm_oha_free_rqb(channel, oha);
	}

	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_channel_close (
	CM_CHANNEL_PTR_TYPE channel
)
{
	CM_LOG_FCT ("cm_oha_channel_close")
	CM_OHA_LOWER_RQB_PTR_TYPE  oha;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_OHA);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	CM_ASSERT (is_not_null(channel->lower.prealloc.oha));

	oha = channel->lower.prealloc.oha;

	CM_OHA_SET_USER_ID_HANDLE_DEV_AR (oha, channel->my_handle, 0, 0);

	CM_OHA_SET_HANDLE (oha, channel->lower.handle);
	CM_OHA_SET_OPCODE (oha, OHA_OPC_CLOSE_CHANNEL);

	CM_OHA_CLOSE_CHANNEL_LOWER (oha, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha
) {
	CM_LOG_FCT ("cm_oha_close_channel_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_OHA_GET_RESPONSE (oha);

	if (rc == OHA_OK) {

		cm_oha_free_rqb (channel, oha);

		channel->lower.handle = CM_OHA_INVALID_HANDLE;
		channel->lower.prealloc.oha = LSA_NULL;

		CM_OHA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) rsp(OK)", channel->my_handle
			);

		rc = CM_OK;
	}
	else {
		/* don't invalidate oha-handle, channel is not closed! */
		CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rsp(%u)", channel->my_handle, CM_OHA_GET_RESPONSE (oha)
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	get channel info
//---------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_channel_info(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_oha_channel_info")
	CM_OHA_LOWER_RQB_PTR_TYPE oha;

	CM_ASSERT (channel->state == CM_CH_INFO);

	cm_oha_alloc_rqb (channel, & oha);

	if (is_null (oha)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	cm_oha_request_lower(channel, OHA_OPC_GET_CONFIG, 0, 0, oha);
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_channel_info_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha
) {
	CM_LOG_FCT ("cm_oha_channel_info_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_INFO);

	rc = CM_OHA_GET_RESPONSE(oha);

	if (rc == OHA_OK) {

		CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) oha-rsp(%u)"
			, channel->my_handle, rc
			);

		channel->lower.info.oha = oha->args.Config;

		rc = CM_OK;
	}
	else {

		CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) oha-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_oha_free_rqb (channel, oha);

	/*
	 *  transition CM-CH::INFO --> CM-CH::READY
	 */

	CM_ASSERT(channel->state == CM_CH_INFO);

	cm_channel_done (channel, rc);
}

/*===========================================================================*/
/*=== SET_IDENT / UseCase set within CreatePD ===============================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_set_ident (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_OEM_DATA_PTR_TYPE  oem_data
) {
	CM_LOG_FCT("cm_oha_set_ident")
	CM_CHANNEL_PTR_TYPE  oha_channel = cm_oha_channel_get(channel);
	CM_OHA_LOWER_RQB_PTR_TYPE  oha = oha_channel->lower.prealloc.oha;
	CM_ASSERT (is_not_null(oha));

	{
	OHA_UPPER_IDENT_PTR_TYPE  oha_ident = & oha->args.Ident;

	oha_ident->Options = OHA_CTRL_OEM_DEVICE_ID;

	oha_ident->OEMVendorID = oem_data->vendor_id;
	oha_ident->OEMDeviceID = oem_data->device_id;
	}

	cm_oha_request_lower (channel, OHA_OPC_SET_IDENT, 0, 0, oha);
}

/*===========================================================================*/
/*=== IP-SUITE AND NAME-OF-STATION HELPER ===================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_ip_suite_is_notnull_nonconflicting_and_expected (
	CM_CHANNEL_PTR_TYPE  user_channel
	)
{
	CM_CHANNEL_PTR_TYPE  oha_channel = cm_oha_channel_get (user_channel);

	if ( oha_channel->usr.oha.is_IPAddr_null )
	{
		return LSA_FALSE; /* is 0.0.0.0 */
	}

	if ( oha_channel->usr.oha.is_IPSuite_conflicting )
	{
		return LSA_FALSE; /* is conflicting */
	}

	if ( oha_channel->usr.oha.is_IPSuite_emergency)
	{
		return LSA_FALSE; /* is the emergency IP suite (i.e. not the "expected" IP suite) */
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_get_ip_suite (
	CM_CHANNEL_PTR_TYPE  user_channel,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  * ip_addr,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  * subnet_mask,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  * gateway_ip
	)
{
	CM_CHANNEL_PTR_TYPE  oha_channel = cm_oha_channel_get (user_channel);

	CM_ASSERT(OHA_IP_ADDR_SIZE == 4)

	if ( oha_channel->usr.oha.is_IPAddr_null )
	{
		return LSA_FALSE; /* 0.0.0.0 is invalid (the other cannot answer the call) */
	}

	CM_MEMCPY (ip_addr, oha_channel->usr.oha.IPSuite.IpAddr, sizeof (LSA_UINT32));
	CM_MEMCPY (subnet_mask, oha_channel->usr.oha.IPSuite.NetMask, sizeof (LSA_UINT32));
	CM_MEMCPY (gateway_ip, oha_channel->usr.oha.IPSuite.Gateway, sizeof (LSA_UINT32));

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_name_of_station_is_valid (
	CM_CHANNEL_PTR_TYPE  user_channel
	)
{
	CM_CHANNEL_PTR_TYPE  oha_channel = cm_oha_channel_get (user_channel);

	if (oha_channel->usr.oha.name_of_station_length == 0)
	{
		return LSA_FALSE;
	}

	/* note: functionality "detect conflicting NoS" was removed from OHA with V5.3i2.4 (AP01549948) */

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_get_name_of_station (
	CM_CHANNEL_PTR_TYPE  user_channel,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  * nos_length,
	CM_COMMON_MEM_U8_PTR_TYPE  CM_LOCAL_MEM_ATTR  * nos
	)
{
	LSA_BOOL is_valid = cm_oha_name_of_station_is_valid (user_channel);
	CM_CHANNEL_PTR_TYPE  oha_channel = cm_oha_channel_get (user_channel);

	*nos_length	= oha_channel->usr.oha.name_of_station_length;
	*nos		= oha_channel->usr.oha.name_of_station;

	return is_valid;
}


/*===========================================================================*/
/*=== ADDRESS INDICATION ====================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_address_indication(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha /* provide: LSA_NULL */
) {
	CM_LOG_FCT("cm_oha_address_indication")

	if( is_null(oha) ) /* provide */
	{
		cm_oha_alloc_rqb (channel, &oha);

		if( is_null(oha) ) {
			CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot oha-alloc");
			CM_FATAL();
		}

		CM_OHA_ALLOC_LOWER_MEM (CM_CAST_OHA_MEM_PTR_PTR (& oha->args.AddrInfo.NoS.pName)
			, cm_null_user_id, OHA_MAX_STATION_NAME_LEN, channel->sysptr
			);
		if( is_null (oha->args.AddrInfo.NoS.pName) ) {
			CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot oha-alloc-lower-mem");
			CM_FATAL();
		}

		oha->args.AddrInfo.NoS.Length = channel->usr.oha.name_of_station_length;
		CM_MEMCPY(oha->args.AddrInfo.NoS.pName, channel->usr.oha.name_of_station, channel->usr.oha.name_of_station_length);

		oha->args.AddrInfo.CurrentIPSuite = channel->usr.oha.IPSuite;

		oha->args.AddrInfo.CurrentIPSuiteIsEmergency = channel->usr.oha.is_IPSuite_emergency;
		oha->args.AddrInfo.CurrentIPSuiteConflictDetected = channel->usr.oha.is_IPSuite_conflicting;

		oha->args.AddrInfo.ExpectedIPSuite = oha->args.AddrInfo.CurrentIPSuite; /* just a value */
		oha->args.AddrInfo.EmergencyIPSuite = oha->args.AddrInfo.CurrentIPSuite; /* just a value */

		oha->args.AddrInfo.FactoryReset = LSA_FALSE;

		cm_oha_request_lower(channel, OHA_OPC_ADDR_INFO_IND, 0, 0, oha);
	}
	else /* handle indication and do re-provide */
	{
		if (channel->state >= CM_CH_CLOSE)
		{
			LSA_UINT16  rc;

			CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "channel is closing, freeing rqb");

			CM_ASSERT (CM_OHA_GET_RESPONSE(oha) == OHA_OK  ||  CM_OHA_GET_RESPONSE(oha) == OHA_OK_CANCEL);

			CM_OHA_FREE_LOWER_MEM (&rc, oha->args.AddrInfo.NoS.pName, channel->sysptr);
			CM_ASSERT (rc == LSA_OK);

			cm_oha_free_rqb (channel, oha);
		}
		else if (CM_OHA_GET_RESPONSE(oha) != OHA_OK)
		{
			CM_FATAL(); /* what? why? */
		}
		else
		{
			LSA_BOOL  ip_changed, nos_changed, reset_to_factory;

			OHA_UPPER_ADDR_INFO_PTR_TYPE  addr = & oha->args.AddrInfo;
			if (is_null (addr)) {
				CM_FATAL(); /* we provide this */
			}

			if (addr->NoS.Length > OHA_MAX_STATION_NAME_LEN) {
				CM_FATAL();
			}

			/***/

			ip_changed = LSA_FALSE;

			if (cm_oha_mem_compare (
				(CM_COMMON_MEM_U8_PTR_TYPE)(&channel->usr.oha.IPSuite),
				(CM_COMMON_MEM_U8_PTR_TYPE)(&addr->CurrentIPSuite), sizeof(addr->CurrentIPSuite)) != 0)
			{
				CM_OHA_TRACE_12 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "ip-suite-changed:  ip-addr(%u.%u.%u.%u) netmask(%u.%u.%u.%u) gateway(%u.%u.%u.%u)"
					, addr->CurrentIPSuite.IpAddr[0],  addr->CurrentIPSuite.IpAddr[1],  addr->CurrentIPSuite.IpAddr[2],  addr->CurrentIPSuite.IpAddr[3]
					, addr->CurrentIPSuite.NetMask[0], addr->CurrentIPSuite.NetMask[1], addr->CurrentIPSuite.NetMask[2], addr->CurrentIPSuite.NetMask[3]
					, addr->CurrentIPSuite.Gateway[0], addr->CurrentIPSuite.Gateway[1], addr->CurrentIPSuite.Gateway[2], addr->CurrentIPSuite.Gateway[3]
					);

				channel->usr.oha.IPSuite = addr->CurrentIPSuite; /* update */
				ip_changed = LSA_TRUE;

				{
				OHA_IP_TYPE zeroIpAddr = {0};
				if( cm_oha_mem_compare (channel->usr.oha.IPSuite.IpAddr, zeroIpAddr, sizeof(zeroIpAddr)) == 0 ) {
					channel->usr.oha.is_IPAddr_null = LSA_TRUE;
				}
				else {
					channel->usr.oha.is_IPAddr_null = LSA_FALSE;
				}
				}
			}

			if (channel->usr.oha.is_IPSuite_emergency != addr->CurrentIPSuiteIsEmergency)
			{
				CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "ip-suite-changed:  old-emergency(%u)  new-emergency(%u)"
					, channel->usr.oha.is_IPSuite_emergency, addr->CurrentIPSuiteIsEmergency
					);

				channel->usr.oha.is_IPSuite_emergency = addr->CurrentIPSuiteIsEmergency; /* update */
				ip_changed = LSA_TRUE;
			}

			if (channel->usr.oha.is_IPSuite_conflicting != addr->CurrentIPSuiteConflictDetected)
			{
				CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "ip-suite-changed:  old-conflicting(%u)  new-conflicting(%u)"
					, channel->usr.oha.is_IPSuite_conflicting, addr->CurrentIPSuiteConflictDetected
					);

				channel->usr.oha.is_IPSuite_conflicting = addr->CurrentIPSuiteConflictDetected; /* update */
				ip_changed = LSA_TRUE;
			}

			/***/

			nos_changed = LSA_FALSE;

			if (channel->usr.oha.name_of_station_length != addr->NoS.Length
			||  cm_oha_mem_compare (channel->usr.oha.name_of_station, addr->NoS.pName, addr->NoS.Length) != 0
			)
			{
				CM_OHA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "name-of-station-changed:  old-length(%u)  new-length(%u)"
					, channel->usr.oha.name_of_station_length, addr->NoS.Length
					);
				CM_OHA_TRACE_12 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "name-of-station changed:  NoS(%c%c%c%c%c%c%c%c%c%c%c%c..)[trunc at 12 chars]"
					, addr->NoS.Length >  0 ? addr->NoS.pName[ 0] : ' '
					, addr->NoS.Length >  1 ? addr->NoS.pName[ 1] : ' '
					, addr->NoS.Length >  2 ? addr->NoS.pName[ 2] : ' '
					, addr->NoS.Length >  3 ? addr->NoS.pName[ 3] : ' '
					, addr->NoS.Length >  4 ? addr->NoS.pName[ 4] : ' '
					, addr->NoS.Length >  5 ? addr->NoS.pName[ 5] : ' '
					, addr->NoS.Length >  6 ? addr->NoS.pName[ 6] : ' '
					, addr->NoS.Length >  7 ? addr->NoS.pName[ 7] : ' '
					, addr->NoS.Length >  8 ? addr->NoS.pName[ 8] : ' '
					, addr->NoS.Length >  9 ? addr->NoS.pName[ 9] : ' '
					, addr->NoS.Length > 10 ? addr->NoS.pName[10] : ' '
					, addr->NoS.Length > 11 ? addr->NoS.pName[11] : ' '
					);

				channel->usr.oha.name_of_station_length = addr->NoS.Length; /* update */
				CM_MEMCPY (channel->usr.oha.name_of_station, addr->NoS.pName, addr->NoS.Length); /* update */
				nos_changed = LSA_TRUE;
			}

			/***/

			reset_to_factory = LSA_FALSE;

			if (addr->FactoryReset)
			{
				CM_OHA_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "ResetToFactory: oha-addr.ind"
					);
				reset_to_factory = LSA_TRUE;
			}

			/***/

			{
				CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

				if (is_not_null(pd_channel))
				{
					if (reset_to_factory) /* RTF takes precedence over NoS changed */
					{
						cm_pd_rtf_nos_ip_changed (pd_channel, CM_AR_REASON_DCP_RESET);
					}
					else if (nos_changed) /* NoS changed takes precedence over IP-Suite changed */
					{
						cm_pd_rtf_nos_ip_changed (pd_channel, CM_AR_REASON_DCP_NAME);
					}
					else if (ip_changed)
					{
						cm_pd_rtf_nos_ip_changed (pd_channel, CM_AR_REASON_IPSUITE);
					}
					else
					{
						/* do nothing */
					}
				}
			}

			/***/

			cm_oha_request_lower(channel, OHA_OPC_ADDR_INFO_IND, 0, 0, oha); /* re-provide */
		}
	}
}


/*===========================================================================*/
/*=== CONTROL (LOCK/UNLOCK) IP-SUITE ========================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_control_ip_suite(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 oha_mode, /* OHA_LOCK_SET or OHA_UNLOCK_SET */
	CM_OHA_LOWER_RQB_PTR_TYPE oha
) {
	CM_LOG_FCT("cm_oha_control_ip_suite")

	CM_OHA_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) oha_mode(%u)"
		, argr->device_nr, argr->ar_nr
		, oha_mode
		);

	if( is_null(oha) || CM_OHA_GET_OPCODE(oha) != 0/*unused*/ ) {

		CM_FATAL(); /* a bug */
	}
	else {

		OHA_UPPER_DCP_CONTROL_PTR_TYPE param = &oha->args.DcpControl;

		param->Mode = oha_mode;
		param->Options = OHA_CTRL_IP_SUITE;
		param->Type = 0;
		param->SubType = 0;
		param->pAddrInfo->NoS.Length = 0;
		param->pAddrInfo->NoS.pName = LSA_NULL; /* see allocation of RQB */

		cm_oha_request_lower(argr->ar_com.channel, OHA_OPC_DCP_CONTROL, argr->device_nr, argr->ar_nr, oha);
	}
}

/*===========================================================================*/
/*=== WRITE PTCP-DATA =======================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_write_ptcp_data(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL do_sync,
	LSA_BOOL is_sync,
	OHA_MAC_TYPE CM_LOCAL_MEM_ATTR *master_mac
) {
	CM_LOG_FCT("cm_oha_write_ptcp_data")
	CM_OHA_LOWER_RQB_PTR_TYPE oha;

	cm_oha_alloc_rqb(channel, &oha);

	if( is_null(oha) ) {
		CM_FATAL(); /* todo: make non-fatal */
	}

	oha->args.PtcpData.DoSync = do_sync;
	oha->args.PtcpData.IsSync = is_sync;
	CM_MEMCPY(&oha->args.PtcpData.MasterSourceAddress[0], &(*master_mac)[0], sizeof(oha->args.PtcpData.MasterSourceAddress));

	CM_OHA_TRACE_08(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "DoSync(%u) IsSync(%u) MasterMac(%02x-%02x-%02x-%02x-%02x-%02x)"
		, oha->args.PtcpData.DoSync
		, oha->args.PtcpData.IsSync
		, oha->args.PtcpData.MasterSourceAddress[0], oha->args.PtcpData.MasterSourceAddress[1], oha->args.PtcpData.MasterSourceAddress[2]
		, oha->args.PtcpData.MasterSourceAddress[3], oha->args.PtcpData.MasterSourceAddress[4], oha->args.PtcpData.MasterSourceAddress[5]
		);

	cm_oha_request_lower(channel, OHA_OPC_WRITE_PTCP_DATA, 0, 0, oha);
}

/*===========================================================================*/
/*=== HELLO INDICATION ======================================================*/
/*===========================================================================*/

#if CM_CFG_MAX_CLIENTS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_register_client (
	CM_CHANNEL_PTR_TYPE  oha_channel,
	CM_CHANNEL_PTR_TYPE  cl_channel /* LSA_NULL..unregister */
) {
	CM_ASSERT (oha_channel->detailptr->path_type == CM_PATH_TYPE_OHA);

	if (is_not_null (cl_channel)) {
		CM_ASSERT (is_null (oha_channel->lower.channel[CM_PATH_TYPE_CLIENT]));
		oha_channel->lower.channel[CM_PATH_TYPE_CLIENT] = cl_channel; /* register */
	}
	else {
		CM_ASSERT (is_not_null (oha_channel->lower.channel[CM_PATH_TYPE_CLIENT]));
		oha_channel->lower.channel[CM_PATH_TYPE_CLIENT] = LSA_NULL; /* unregister */
	}
}

#endif

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_hello_indication(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha /* provide: LSA_NULL */
) {
	CM_LOG_FCT("cm_oha_hello_indication")

	if( is_null(oha) ) /* provide */
	{
		LSA_UINT16 max = channel->detailptr->u.oha.hello_resource_count;
		LSA_UINT16 cnt;

		for( cnt = 0; cnt < max; ++cnt )
		{
			cm_oha_alloc_rqb(channel, &oha);

			if( is_null(oha) ) {
				CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot oha-alloc");
				CM_FATAL();
			}

			CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_OHA); /* see sysptr in alloc/free */

			CM_OHA_ALLOC_LOWER_MEM (CM_CAST_OHA_MEM_PTR_PTR (& oha->args.HelloInd.pStationName)
				, cm_null_user_id, OHA_MAX_STATION_NAME_LEN, channel->sysptr
				);
			if (is_null (oha->args.HelloInd.pStationName)) {
				CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot oha-alloc-lower hello.ind");
				CM_FATAL();
			}

			oha->args.HelloInd.StationNameLen	= OHA_MAX_STATION_NAME_LEN;

			oha->args.HelloInd.FilterID			= 0; /* sanity */

			cm_oha_request_lower (channel, OHA_OPC_HELLO_INDICATION, 0, 0, oha);
		}
	}
	else /* handle indication and do re-provide */
	{
		if( channel->state >= CM_CH_CLOSE )
		{
			LSA_UINT16  rc;

			CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);
			CM_ASSERT(
				CM_OHA_GET_RESPONSE(oha) == OHA_OK
				|| CM_OHA_GET_RESPONSE(oha) == OHA_OK_CANCEL
				);

			CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_OHA); /* see sysptr in alloc/free */

			CM_OHA_FREE_LOWER_MEM (&rc, oha->args.HelloInd.pStationName, channel->sysptr);
			CM_ASSERT (rc == LSA_OK);
			cm_oha_free_rqb(channel, oha);
		}
		else if( CM_OHA_GET_RESPONSE(oha) != OHA_OK )
		{
			CM_FATAL(); /* what? why? */
		}
		else
		{
			OHA_UPPER_HELLO_PTR_TYPE ind = &oha->args.HelloInd;

			CM_OHA_TRACE_12 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "hello.ind: FilterID(0x%x) StationNameLen(%u) IP(%u.%u.%u.%u) MAC(%02x-%02x-%02x-%02x-%02x-%02x)"
				, ind->FilterID
				, ind->StationNameLen
				, ind->IPAddr[0], ind->IPAddr[1], ind->IPAddr[2], ind->IPAddr[3]
				, ind->MacAddr[0], ind->MacAddr[1], ind->MacAddr[2]
				, ind->MacAddr[3], ind->MacAddr[4], ind->MacAddr[5]
				);
			LSA_UNUSED_ARG (ind);

#if CM_CFG_MAX_CLIENTS
			{
			CM_CHANNEL_PTR_TYPE cl_channel = channel->lower.channel[CM_PATH_TYPE_CLIENT];

			if( is_not_null(cl_channel) ) {

				cm_cl_oha_hello_ind (cl_channel, ind);
			}
			else {

				/* late, ignore */
			}
			}
#endif

			oha->args.HelloInd.StationNameLen = OHA_MAX_STATION_NAME_LEN;
			cm_oha_request_lower (channel, OHA_OPC_HELLO_INDICATION, 0, 0, oha); /* re-provide */
		}
	}
}

/*===========================================================================*/
/*=== PRM INTERFACE =========================================================*/
/*===========================================================================*/

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_oha_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
) {
	CM_PORT_UINT mask;

	channel = cm_edd_channel_get(channel); /* get from EDD-channel */

	mask = ((CM_PORT_UINT)2 << channel->lower.info.edd.MaxPortCnt) - 1;

#if 0/*AP01116719: now provide the interface too*/
	mask &= ~0x01; /* not providing on the interface */
#endif

	return mask;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  oha /* provide: LSA_NULL */
) {
	CM_LOG_FCT("cm_oha_prm_indication")

	if (is_null (oha)) { /* provide */

		CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
		CM_PORT_UINT provide_mask = 0;
		LSA_UINT16 port_id;

		for (port_id = 0; port_id <= edd_channel->lower.info.edd.MaxPortCnt; port_id++)
		{
			LSA_UINT16 max = 2/*!*/;
			LSA_UINT16 cnt;

			for( cnt = 0; cnt < max; ++cnt )
			{
				cm_oha_alloc_rqb (channel, & oha);

				if (is_null (oha)) {
					CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot oha-alloc-rqb");
					CM_FATAL();
				}

				CM_OHA_ALLOC_LOWER_MEM (
					CM_CAST_OHA_MEM_PTR_PTR (& oha->args.PrmInd.diag_ptr)
					, cm_null_user_id
					, (sizeof(OHA_PRM_DIAG_ENTRY_TYPE) * OHA_TOPO_MAX_IND_ENTRIES)
					, channel->sysptr
					);

				if (is_null (oha->args.PrmInd.diag_ptr)) {
					CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot oha-alloc-mem");
					CM_FATAL();
				}

				oha->args.PrmInd.edd_port_id = port_id; /* an in-paramter! */

				oha->args.PrmInd.diag_cnt = 0; /* sanity (an out-paramter!) */

				cm_oha_request_lower (channel, OHA_OPC_PRM_INDICATION, 0, 0, oha);
			}

			provide_mask |= ((CM_PORT_UINT)1 << port_id);
		}

		if( provide_mask != cm_oha_prm_provide_mask(channel) ) {
			CM_FATAL(); /* a bug */
		}
	}
	else { /* handle indication and do re-provide */

		CM_OHA_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "oha-prm.IND edd_port_id(%u) diag_cnt(%u)"
			, oha->args.PrmInd.edd_port_id
			, oha->args.PrmInd.diag_cnt
			);

		if( channel->state >= CM_CH_CLOSE ) {

			CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);

			CM_ASSERT(
				CM_OHA_GET_RESPONSE(oha) == OHA_OK
				|| CM_OHA_GET_RESPONSE(oha) == OHA_OK_CANCEL
				);

			{
			LSA_UINT16  rc;
			CM_OHA_FREE_LOWER_MEM (& rc, oha->args.PrmInd.diag_ptr, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
			LSA_UNUSED_ARG (rc);
			}

			cm_oha_free_rqb(channel, oha);
		}
		else if( CM_OHA_GET_RESPONSE(oha) != OHA_OK ) {

			CM_FATAL(); /* what? why? */
		}
		else {

			if( oha->args.PrmInd.diag_cnt > OHA_TOPO_MAX_IND_ENTRIES ) {
				CM_FATAL();
			}

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

			if( is_null(pd_channel) )
			{
				CM_OHA_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "oha-prm.IND but no pd-channel, ignored");
			}
			else
			{
				cm_pd_oha_prm_indication_done (pd_channel, &oha->args.PrmInd);
			}
			}

			/***/

			/*
			oha->args.PrmInd.edd_port_id = unchanged (an in-paramter!)
			*/

			oha->args.PrmInd.diag_cnt = 0; /* sanity (an out-paramter!) */

			cm_oha_request_lower (channel, OHA_OPC_PRM_INDICATION, 0, 0, oha);
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_prepare (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_oha_prm_prepare")
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(channel);
	CM_OHA_LOWER_RQB_PTR_TYPE  oha = oha_channel->lower.prealloc.oha;

	CM_ASSERT (is_not_null(oha));

	/* oha->args.PrmControl.is_local = don't care */

	cm_oha_request_lower (channel, OHA_OPC_PRM_PREPARE, 0, 0, oha);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_write (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_BOOL				mk_rema,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_LOG_FCT("cm_oha_prm_write")
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(channel);
	CM_OHA_LOWER_RQB_PTR_TYPE  oha = oha_channel->lower.prealloc.oha;

	CM_ASSERT (is_not_null(oha));

	oha->args.PrmData.edd_port_id = edd_port_id;
	oha->args.PrmData.slot_number = slot_number;
	oha->args.PrmData.subslot_number = subslot_number;
	oha->args.PrmData.record_index = record_index;
	oha->args.PrmData.record_data_length = record_data_length;
	oha->args.PrmData.mk_rema = mk_rema;

	/* record_data used as reference, check this */
	oha->args.PrmData.record_data = CM_CAST_OHA_MEM_U8_PTR (record_data);

	cm_oha_request_lower (channel, OHA_OPC_PRM_WRITE, 0, 0, oha);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_oha_prm_read (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_LOG_FCT("cm_oha_prm_read")
	CM_OHA_LOWER_RQB_PTR_TYPE  oha;

	cm_oha_alloc_rqb (channel, & oha);

	if (is_null (oha)) {
		return CM_ERR_ALLOC_LOWER;
	}

	oha->args.PrmData.edd_port_id = edd_port_id;
	oha->args.PrmData.slot_number = slot_number;
	oha->args.PrmData.subslot_number = subslot_number;
	oha->args.PrmData.record_index = record_index;
	oha->args.PrmData.record_data_length = record_data_length;

	/* record_data used as reference, check this */
	oha->args.PrmData.record_data = CM_CAST_OHA_MEM_U8_PTR (record_data);

	cm_oha_request_lower (channel, OHA_OPC_PRM_READ, 0, 0, oha);
	return CM_OK;
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_end (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT("cm_oha_prm_end")
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(channel);
	CM_OHA_LOWER_RQB_PTR_TYPE  oha = oha_channel->lower.prealloc.oha;

	CM_ASSERT (is_not_null(oha));

	/* oha->args.PrmControl.is_local = don't care */

	cm_oha_request_lower (channel, OHA_OPC_PRM_END, 0, 0, oha);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_commit (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  is_local
) {
	CM_LOG_FCT("cm_oha_prm_commit")
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(channel);
	CM_OHA_LOWER_RQB_PTR_TYPE  oha = oha_channel->lower.prealloc.oha;

	CM_ASSERT (is_not_null(oha));

	oha->args.PrmControl.is_local = is_local;

	/* AP01504165: PrmControl.do_rtf removed, function parameter "do_reset_to_factory" removed */

	cm_oha_request_lower (channel, OHA_OPC_PRM_COMMIT, 0, 0, oha);
}


/*===========================================================================*/
/*=== OHA REQUEST LOWER =====================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_OHA_LOWER_RQB_PTR_TYPE oha
) {
	CM_LOG_FCT ("cm_oha_request_lower")
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(channel);

	CM_OHA_SET_USER_ID_HANDLE_DEV_AR (oha, channel->my_handle, device_nr, ar_nr);

	CM_OHA_SET_HANDLE (oha, oha_channel->lower.handle);
	CM_OHA_SET_OPCODE (oha, opcode);

	CM_OHA_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> oha-rqb(0x%08x) oha-opc(%u) f-h(%u) f-dev(%u) f-ar(%u)"
		, oha, CM_OHA_GET_OPCODE (oha)
		, CM_OHA_GET_USER_ID_HANDLE (oha), CM_OHA_GET_USER_ID_DEV (oha), CM_OHA_GET_USER_ID_AR (oha)
		);

	CM_OHA_REQUEST_LOWER (oha, oha_channel->sysptr);
}

/*===========================================================================*/
/*=== OHA LOWER DONE ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_OHA_LOWER_IN_FCT_ATTR
cm_oha_request_lower_done (
	CM_OHA_LOWER_RQB_PTR_TYPE  oha
) {
	CM_LOG_FCT ("cm_oha_request_lower_done")
	CM_CHANNEL_PTR_TYPE  channel;

	if( is_null(oha) ) {

		CM_OHA_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL,
			"is_null(oha)"
			);

		CM_OHA_LOWER_RQB_ERROR (oha);
		return;
	}

	channel = cm_channel_from_handle (CM_OHA_GET_USER_ID_HANDLE (oha));

	if( is_null (channel) ) {

		CM_OHA_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"invalid handle(%u)",
			CM_OHA_GET_USER_ID_HANDLE (oha)
			);

		CM_OHA_LOWER_RQB_ERROR (oha);
		return;
	}

	switch (CM_OHA_GET_RESPONSE (oha)) {
	case OHA_OK:
	case OHA_OK_CANCEL:
		CM_OHA_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< oha-rqb(0x%08x) oha-opc(%u) oha-rsp(%u)  t-h(%u) t-dev(%u) t-ar(%u)"
			, oha, CM_OHA_GET_OPCODE (oha), CM_OHA_GET_RESPONSE (oha)
			, CM_OHA_GET_USER_ID_HANDLE (oha), CM_OHA_GET_USER_ID_DEV (oha), CM_OHA_GET_USER_ID_AR (oha)
			);
		break;

	default:
		CM_OHA_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "<<< oha-rqb(0x%08x) oha-opc(%u) oha-rsp(%u)  t-h(%u) t-dev(%u) t-ar(%u)"
			, oha, CM_OHA_GET_OPCODE (oha), CM_OHA_GET_RESPONSE (oha)
			, CM_OHA_GET_USER_ID_HANDLE (oha), CM_OHA_GET_USER_ID_DEV (oha), CM_OHA_GET_USER_ID_AR (oha)
			);
		break;
	}

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_OHA:

		switch (CM_OHA_GET_OPCODE (oha)) {

		case OHA_OPC_OPEN_CHANNEL:
			cm_oha_channel_open_done (channel, oha);
			break;

		case OHA_OPC_CLOSE_CHANNEL:
			cm_oha_channel_close_done (channel, oha);
			break;

		case OHA_OPC_GET_CONFIG:
			cm_oha_channel_info_done(channel, oha);
			break;

		case OHA_OPC_ADDR_INFO_IND:
			cm_oha_address_indication(channel, oha);
			break;

		case OHA_OPC_PRM_INDICATION:
			cm_oha_prm_indication (channel, oha);
			break;

		case OHA_OPC_HELLO_INDICATION:
			cm_oha_hello_indication(channel, oha);
			break;

		default:
			CM_OHA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled opcode(%u)",
				CM_OHA_GET_OPCODE (oha)
				);
			CM_OHA_LOWER_RQB_ERROR (oha); /* should not happen */
			break;
		}
		break;

	case CM_PATH_TYPE_PD:

		switch (CM_OHA_GET_OPCODE (oha)) {

		case OHA_OPC_SET_IDENT:
			{
			LSA_UINT16 rsp = CM_OHA_GET_RESPONSE(oha);
			cm_pd_oha_set_ident_done(channel, rsp);
			}
			break;

		case OHA_OPC_WRITE_PTCP_DATA:
			{
			LSA_UINT16 rsp = CM_OHA_GET_RESPONSE(oha);
			cm_oha_free_rqb(channel, oha);
			cm_pd_oha_write_ptcp_data_done (channel, rsp);
			}
			break;

		case OHA_OPC_PRM_PREPARE:
			cm_pd_oha_prm_prepare_done (channel, oha);
			break;

		case OHA_OPC_PRM_WRITE:
			cm_pd_oha_prm_write_done (channel, oha);
			break;

		case OHA_OPC_PRM_END:
			cm_pd_oha_prm_end_done (channel, oha);
			break;

		case OHA_OPC_PRM_COMMIT:
			cm_pd_oha_prm_commit_done (channel, oha);
			break;

		case OHA_OPC_PRM_READ:
			cm_pd_oha_prm_read_done(channel, oha);
			break;

		default:
			CM_OHA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled opcode(%u)",
				CM_OHA_GET_OPCODE (oha)
				);
			CM_OHA_LOWER_RQB_ERROR (oha); /* should not happen */
			break;
		}
		break;

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:

		switch (CM_OHA_GET_OPCODE (oha)) {

		case OHA_OPC_DCP_CONTROL:
			cm_sv_ar_control_ip_suite_done(channel, CM_OHA_GET_USER_ID_DEV(oha), CM_OHA_GET_USER_ID_AR(oha), oha);
			break;

		default:
			CM_OHA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)"
				, CM_OHA_GET_OPCODE (oha)
				);
			CM_OHA_LOWER_RQB_ERROR (oha); /* should not happen */
			break;
		}
		break;
#endif

	default:
		CM_OHA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "wrong path-type(%u)"
			, channel->detailptr->path_type
			);
		CM_OHA_LOWER_RQB_ERROR (oha); /* should not happen */
		break;
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
