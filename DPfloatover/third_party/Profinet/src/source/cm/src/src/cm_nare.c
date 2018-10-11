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
/*  F i l e               &F: cm_nare.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  NARE (name and address resolution) things                                */
/*                                                                           */
/*  - alloc NARE-RQB                                                         */
/*  - free NARE-RQB                                                          */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	30
#define CM_MODULE_ID		30

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_NARE

/*---------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_nare_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
);

/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/
CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_nare_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_nare_channel_get")

	channel = channel->lower.channel[CM_PATH_TYPE_NARE];

	if( is_null(channel) ) {
		CM_FATAL(); /* a bug */
	}

	return channel;
}

/*-----------------------------------------------------------------------------
//	alloc lower NARE-RQB that fits for all requests
//---------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_NARE_LOWER_RQB_PTR_TYPE	*lower_rqb_ptr_ptr
)
{
	CM_LOG_FCT ("cm_nare_alloc_rqb")
	CM_CHANNEL_PTR_TYPE nare_channel = cm_nare_channel_get(channel);

	CM_ASSERT (is_not_null(channel));
	CM_ASSERT (is_not_null(lower_rqb_ptr_ptr));

		/* alloc the RQB */
	CM_NARE_ALLOC_LOWER_RQB (lower_rqb_ptr_ptr, cm_null_user_id, sizeof (CM_NARE_LOWER_RQB_TYPE), nare_channel->sysptr);
	if (is_null (*lower_rqb_ptr_ptr)) {
		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc NARE RQB, len(%u)"
			, channel->my_handle, sizeof (CM_NARE_LOWER_RQB_TYPE)
			);
		return;
	}

#if CM_DEBUG
	CM_NARE_SET_NEXT_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
	CM_NARE_SET_PREV_RQB_PTR (*lower_rqb_ptr_ptr, LSA_NULL); /* sanity */
#endif
}


/*-----------------------------------------------------------------------------
//	free the RQB
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
)
{
	CM_LOG_FCT ("cm_nare_free_rqb")
	LSA_UINT16	rc;

	CM_ASSERT (is_not_null(channel));
	CM_ASSERT (is_not_null(lower_rqb_ptr));

	CM_NARE_FREE_LOWER_RQB (
		& rc,
		lower_rqb_ptr,
		cm_nare_channel_get(channel)->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
}


/*-----------------------------------------------------------------------------
//	free a NARE_OPC_RESOLVE_CONFIG-RQB and the attached alias-array
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_free_rqb_alias (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
)
{
	CM_LOG_FCT ("cm_nare_free_rqb_alias")

	CM_ASSERT (is_not_null (channel));
	CM_ASSERT (is_not_null (lower_rqb_ptr));
	CM_ASSERT (CM_NARE_GET_OPCODE(lower_rqb_ptr) == NARE_OPC_RESOLVE_CONFIG);

	if (is_not_null (lower_rqb_ptr->args.ResConf.pStationNameAlias))
	{
		LSA_UINT16	rc;
		CM_NARE_FREE_LOWER_MEM (& rc, lower_rqb_ptr->args.ResConf.pStationNameAlias, cm_nare_channel_get(channel)->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG(rc);

		lower_rqb_ptr->args.ResConf.pStationNameAlias = LSA_NULL;
	}

	cm_nare_free_rqb (channel, lower_rqb_ptr);
}


/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_LOG_FCT ("cm_nare_channel_open")
	CM_NARE_LOWER_RQB_PTR_TYPE  nare;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_NARE);

	CM_ASSERT (are_equal(cm_nare_channel_get(channel), channel));

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == CM_NARE_INVALID_HANDLE);

	/***/

	cm_nare_alloc_rqb (channel, & nare);

	if (is_null (nare)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	nare->args.channel.handle = CM_NARE_INVALID_HANDLE;
	nare->args.channel.handle_upper = channel->my_handle;
	nare->args.channel.sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	nare->args.channel.nare_request_upper_done_ptr = cm_nare_request_lower_done;

	CM_NARE_SET_USER_ID_HANDLE_DEV_AR (nare, channel->my_handle, 0, 0);

	CM_NARE_SET_HANDLE (nare, CM_NARE_INVALID_HANDLE);
	CM_NARE_SET_OPCODE (nare, NARE_OPC_OPEN_CHANNEL);

	CM_NARE_OPEN_CHANNEL_LOWER (nare, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  nare
)
{
	CM_LOG_FCT ("cm_nare_channel_open_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	if (CM_NARE_GET_RESPONSE (nare) == NARE_RSP_OK) {

		channel->lower.handle = nare->args.channel.handle;

		CM_ASSERT(channel->lower.handle != CM_NARE_INVALID_HANDLE);

		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		rc = CM_OK;

#if CM_CFG_MAX_CLIENTS
		/* provide NARE_OPC_RESOLVE_CONFIG_NOTIFY for "accelerated mode" */
		{
		LSA_UINT16 max = channel->detailptr->u.nare.resolve_notify_resource_count;
		LSA_UINT16 cnt;

		for (cnt = 0; cnt < max; ++cnt) {
			CM_NARE_LOWER_RQB_PTR_TYPE  n;

			cm_nare_alloc_rqb (channel, & n);

			if( is_null(n) ) {
				CM_FATAL(); /* must not ignore this! */
			}

			cm_nare_request_lower (channel, NARE_OPC_RESOLVE_CONFIG_NOTIFY, 0, 0, n);
		}
		}
#endif
	}
	else {

		CM_ASSERT(channel->lower.handle == CM_NARE_INVALID_HANDLE);

		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) nare-rsp(%u)"
			, channel->my_handle, CM_NARE_GET_RESPONSE (nare)
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_nare_free_rqb (channel, nare);
	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_channel_close (
	CM_CHANNEL_PTR_TYPE channel
)
{
	CM_LOG_FCT ("cm_nare_channel_close")
	CM_NARE_LOWER_RQB_PTR_TYPE  nare;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_NARE);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	cm_nare_alloc_rqb (channel, & nare);
	if (is_null (nare)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_NARE_SET_USER_ID_HANDLE_DEV_AR (nare, channel->my_handle, 0, 0);

	CM_NARE_SET_HANDLE (nare, channel->lower.handle);
	CM_NARE_SET_OPCODE (nare, NARE_OPC_CLOSE_CHANNEL);

	CM_NARE_CLOSE_CHANNEL_LOWER (nare, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  nare
) {
	CM_LOG_FCT ("cm_nare_close_channel_done")
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_NARE_GET_RESPONSE (nare);

	cm_nare_free_rqb (channel, nare);

	if (rc == NARE_RSP_OK) {

		channel->lower.handle = CM_NARE_INVALID_HANDLE;

		CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u)"
			, channel->my_handle
			);

		rc = CM_OK;
	}
	else {

		/* don't invalidate nare-handle, channel is not closed! */

		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) nare-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_resolve_notify_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  nare
)
{
	CM_LOG_FCT ("cm_nare_resolve_notify_done")

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_NARE);

	if (channel->state >= CM_CH_CLOSE ) {
		CM_NARE_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "channel is closing, freeing rqb"
			);
		CM_ASSERT(
			CM_NARE_GET_RESPONSE(nare) == NARE_RSP_OK
			|| CM_NARE_GET_RESPONSE(nare) == NARE_RSP_OK_CANCEL
			);
		cm_nare_free_rqb (channel, nare);
	}
	else if (CM_NARE_GET_RESPONSE(nare) != NARE_RSP_OK) {
		CM_FATAL1(CM_NARE_GET_RESPONSE(nare)); /* what? why? */
	}
	else {
#if CM_CFG_MAX_CLIENTS
		CM_CHANNEL_PTR_TYPE  cl_channel = channel->lower.channel[CM_PATH_TYPE_CLIENT];
		NARE_UPPER_RESOLVE_CONFIG_NOTIFY_PTR_TYPE  notify = & nare->args.Notify;

		if (is_not_null (cl_channel))
		{
			cm_cl_nare_notify_ind (cl_channel, notify);
		}
		else {
			/* late, ignore */
		}
#endif
		cm_nare_request_lower (channel, NARE_OPC_RESOLVE_CONFIG_NOTIFY, 0, 0, nare);
	}
}


/*===========================================================================*/
/*=== NARE INFO =============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_nare_channel_info(
	CM_CHANNEL_PTR_TYPE channel
) {
	cm_channel_done(channel, CM_OK); /* note: recursion!*/
}


/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_nare_register_client (
	CM_CHANNEL_PTR_TYPE  nare_channel,
	CM_CHANNEL_PTR_TYPE  cl_channel /* LSA_NULL..unregister */
) {
	CM_ASSERT (nare_channel->detailptr->path_type == CM_PATH_TYPE_NARE);

	if (is_not_null (cl_channel)) {
		CM_ASSERT (is_null (nare_channel->lower.channel[CM_PATH_TYPE_CLIENT]));
		nare_channel->lower.channel[CM_PATH_TYPE_CLIENT] = cl_channel; /* register */
	}
	else {
		CM_ASSERT (is_not_null (nare_channel->lower.channel[CM_PATH_TYPE_CLIENT]));
		nare_channel->lower.channel[CM_PATH_TYPE_CLIENT] = LSA_NULL; /* unregister */
	}
}

#endif

/*----------------------------------------------------------------------------*/

LSA_BOOL CM_LOCAL_FCT_ATTR
cm_nare_resolve (			/* called by cm_arsm_mcons_resolve() and cm_cl_record_read_implicit() */
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32  local_ip_addr,	/* used if do_check_ip == TRUE */
	LSA_UINT8  mode,
	LSA_BOOL  do_check_ip,
	LSA_UINT8  name_length,
	CM_COMMON_MEM_U8_PTR_TYPE  name_ptr,
	LSA_UINT16  device_nr,
	LSA_UINT16  ar_nr,
	LSA_VOID_PTR_TYPE  cancel_request_id
	)
{
	CM_LOG_FCT ("cm_nare_resolve")
	CM_NARE_LOWER_RQB_PTR_TYPE  nare;

	CM_ASSERT (mode == NARE_MODE_RESOLVE_DCP  ||  mode == NARE_MODE_RESOLVE_DNS);

	/***/

	cm_nare_alloc_rqb (channel, & nare);
	if (is_null (nare)) {
		return LSA_FALSE; /* cannot alloc */
	}

	CM_NARE_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ip_addr(%#08X)  mode(%u) searching for station-name with length(%u)"
		, CM_NTOH32(local_ip_addr), mode, name_length
		);

	/***/

	CM_ASSERT(NARE_IP_ADDR_SIZE == sizeof(local_ip_addr));
	CM_MEMCPY (nare->args.ResConf.LocalIPAddr, & local_ip_addr, NARE_IP_ADDR_SIZE);

	nare->args.ResConf.pStationName   = name_ptr; /* use quasi-static ar_graph-pointer, lifetime of ar */

	nare->args.ResConf.StationNameLen = name_length;
	nare->args.ResConf.Mode           = mode;
	nare->args.ResConf.CheckIP        = do_check_ip;
	nare->args.ResConf.SetIPRemanent  = LSA_FALSE;
	nare->args.ResConf.Tolerance      = 0;
	nare->args.ResConf.RequestID      = (LSA_UINT32)cancel_request_id;

	nare->args.ResConf.Notify         = LSA_FALSE; /* no notify.ind wanted */
	nare->args.ResConf.NotifyID       = 0;

	nare->args.ResConf.pStationNameAlias = LSA_NULL; /* see cm_nare_free_rqb_alias() */
	nare->args.ResConf.AliasCnt          = 0; /* see cm_nare_free_rqb_alias() */
	nare->args.ResConf.AliasUsed         = 0; /* cnf */

	nare->args.ResConf.StationNameSetEqual = LSA_FALSE; /* cnf */


	nare->args.ResConf.ExtendedMode              = NARE_EXT_MODE_LEGACY;
	nare->args.ResConf.AllowStationNameOverwrite = LSA_FALSE;
	nare->args.ResConf.DeviceID                  = 0;
	nare->args.ResConf.VendorID                  = 0;
	nare->args.ResConf.InstanceID                = 0;

	/***/

	CM_MEMSET (nare->args.ResConf.Cfg.IPAddr, 0, NARE_IP_ADDR_SIZE);
	CM_MEMSET (nare->args.ResConf.Cfg.SubMask, 0, NARE_IP_ADDR_SIZE);
	CM_MEMSET (nare->args.ResConf.Cfg.DefRouter, 0, NARE_IP_ADDR_SIZE);

	/***/

	CM_MEMSET (nare->args.ResConf.Res.IPAddr, 0, NARE_IP_ADDR_SIZE); /* sanity */
	CM_MEMSET (nare->args.ResConf.Res.SubMask, 0, NARE_IP_ADDR_SIZE); /* sanity */
	CM_MEMSET (nare->args.ResConf.Res.DefRouter, 0, NARE_IP_ADDR_SIZE); /* sanity */
	CM_MEMSET (nare->args.ResConf.Res.MACAddr, 0, NARE_MAC_ADDR_SIZE); /* sanity */

	/***/

	cm_nare_request_lower(channel, NARE_OPC_RESOLVE_CONFIG, device_nr, ar_nr, nare); /* cm_nare_resolve() */

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

LSA_BOOL CM_LOCAL_FCT_ATTR
cm_nare_resolve_alias (				/* called by CM-CL-AR */
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  local_ip_addr,	/* used if do_check_ip == TRUE */
	LSA_UINT8  mode,
	LSA_BOOL  do_check_ip,
	LSA_UINT8  name_length,
	CM_COMMON_MEM_U8_PTR_TYPE  name_ptr,
	LSA_UINT32  ip_addr,
	LSA_UINT32  subnet_mask,
	LSA_UINT32  def_router,
	LSA_BOOL  set_ip_remanent,
	LSA_UINT16  ident_timeout,
	LSA_UINT16  device_nr,
	LSA_UINT16  ar_nr,
	LSA_UINT32  notify_id,
	LSA_VOID_PTR_TYPE  cancel_request_id,
	CM_LIST_ENTRY_PTR_TYPE  alias_list, /* may LSA_NULL */
	LSA_BOOL  nos_overwrite,
	LSA_UINT16  vendor_id,
	LSA_UINT16  device_id,
	LSA_UINT16  instance_id
)
{
	CM_LOG_FCT ("cm_nare_resolve_alias")
	CM_NARE_LOWER_RQB_PTR_TYPE  nare;
	NARE_UPPER_ALIAS_PTR_TYPE  alias_array;
	LSA_UINT32  alias_cnt;
	LSA_UINT32  size;

	if (is_null (alias_list))
	{
		alias_cnt = 0;
		size = 0;
	}
	else
	{
		CmListLength (alias_list, & alias_cnt);
		size = sizeof (*alias_array) * alias_cnt;
	}

	if (size > 0xFFFF)
	{
		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "wrong: alias_cnt(%u) or size(%u) > 0xFFFF"
			, alias_cnt, size
			);
		return LSA_FALSE; /* not allowed from nare */
	}

	cm_nare_alloc_rqb (channel, & nare);
	if (is_null (nare)) {
		return LSA_FALSE; /* cannot alloc */
	}

	CM_NARE_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cm_nare_resolve_alias: mode(%u) ip_addr(0x%08x) alias_cnt(%u) do_check_ip(%u) notify_id(0x%x)"
		, mode, CM_NTOH32(ip_addr), alias_cnt, do_check_ip, notify_id
		);

	/***/

	CM_ASSERT(NARE_IP_ADDR_SIZE == sizeof(local_ip_addr));
	CM_MEMCPY (nare->args.ResConf.LocalIPAddr, & local_ip_addr, NARE_IP_ADDR_SIZE);

	/***/

	nare->args.ResConf.pStationName   = name_ptr; /* use quasi-static ar_graph-pointer, lifetime of ar */

	nare->args.ResConf.StationNameLen = name_length;
	nare->args.ResConf.Mode           = mode;
	nare->args.ResConf.CheckIP        = do_check_ip;
	nare->args.ResConf.SetIPRemanent  = set_ip_remanent;
	nare->args.ResConf.Tolerance      = ident_timeout;
	nare->args.ResConf.RequestID      = (LSA_UINT32)cancel_request_id;

	nare->args.ResConf.Notify         = (notify_id != 0) ? LSA_TRUE : LSA_FALSE;
	nare->args.ResConf.NotifyID       = notify_id;

	nare->args.ResConf.pStationNameAlias = LSA_NULL; /* see cm_nare_free_rqb_alias() */
	nare->args.ResConf.AliasCnt          = 0; /* see cm_nare_free_rqb_alias() */
	nare->args.ResConf.AliasUsed         = 0; /* cnf */

	nare->args.ResConf.StationNameSetEqual = LSA_FALSE; /* cnf */

	if (vendor_id != 0) /* see http://www.profibus.com/IM/Man_ID_Table.xml */
	{
		nare->args.ResConf.ExtendedMode = NARE_EXT_MODE_CHECK_DEVICE_ID;
		nare->args.ResConf.VendorID     = vendor_id;
		nare->args.ResConf.DeviceID     = device_id;
		nare->args.ResConf.InstanceID   = instance_id;
	}
	else
	{
		nare->args.ResConf.ExtendedMode = NARE_EXT_MODE_LEGACY;
		nare->args.ResConf.VendorID     = 0;
		nare->args.ResConf.DeviceID     = 0;
		nare->args.ResConf.InstanceID   = 0;
	}

	nare->args.ResConf.AllowStationNameOverwrite = nos_overwrite;

	if (alias_cnt > 0)
	{
		CM_CL_NARE_ALIAS_PTR_TYPE  alias;

		CM_NARE_ALLOC_LOWER_MEM (CM_CAST_NARE_MEM_PTR_PTR (& alias_array)
			, cm_null_user_id, (LSA_UINT16)size
			, cm_nare_channel_get(channel)->sysptr
			);
		if (is_null (alias_array))
		{
			cm_nare_free_rqb (channel, nare);
			return LSA_FALSE; /* cannot alloc */
		}

		nare->args.ResConf.AliasCnt = 0;
		nare->args.ResConf.pStationNameAlias = alias_array;

		for (CmListForeach (alias, alias_list, CM_CL_NARE_ALIAS_PTR_TYPE))
		{
			CM_ASSERT (alias->name_length <= 255);
			if(alias->name_length > 0)
			{
				alias_array->NameLen		= (LSA_UINT8) alias->name_length;
				alias_array->pName			= alias->name; /* use quasi-static ar_graph-pointer */
				alias_array					+= 1; /* next array */
				nare->args.ResConf.AliasCnt	+= 1;
			}
		}
	}

	/***/

	CM_MEMCPY (nare->args.ResConf.Cfg.IPAddr, & ip_addr, NARE_IP_ADDR_SIZE);
	CM_MEMCPY (nare->args.ResConf.Cfg.SubMask, & subnet_mask, NARE_IP_ADDR_SIZE);
	CM_MEMCPY (nare->args.ResConf.Cfg.DefRouter, & def_router, NARE_IP_ADDR_SIZE);

	/***/

	CM_MEMSET (nare->args.ResConf.Res.IPAddr, 0, NARE_IP_ADDR_SIZE); /* sanity */
	CM_MEMSET (nare->args.ResConf.Res.SubMask, 0, NARE_IP_ADDR_SIZE); /* sanity */
	CM_MEMSET (nare->args.ResConf.Res.DefRouter, 0, NARE_IP_ADDR_SIZE); /* sanity */
	CM_MEMSET (nare->args.ResConf.Res.MACAddr, 0, NARE_MAC_ADDR_SIZE); /* sanity */

	/***/

	cm_nare_request_lower(channel, NARE_OPC_RESOLVE_CONFIG, device_nr, ar_nr, nare);  /* cm_nare_resolve_alias() */

	return LSA_TRUE;
}

#endif

/*===========================================================================*/

LSA_BOOL CM_LOCAL_FCT_ATTR
cm_nare_resolve_cancel(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_VOID_PTR_TYPE cancel_request_id
) {
	CM_LOG_FCT ("cm_nare_resolve_cancel")
	CM_NARE_LOWER_RQB_PTR_TYPE  nare;

	CM_NARE_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cancel_request_id(%08x)", cancel_request_id
		);

	/***/

	cm_nare_alloc_rqb (channel, & nare);

	if (is_null (nare)) {

		CM_NARE_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
			"cannot alloc nare-rqb"
			);

		return LSA_FALSE; /* cannot alloc */
	}

	/***/

	nare->args.ResConfCancel.RequestID = (LSA_UINT32)cancel_request_id;

	/***/

	cm_nare_request_lower(channel, NARE_OPC_RESOLVE_CONFIG_CANCEL, device_nr, ar_nr, nare);

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== NARE REQUEST LOWER ====================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_nare_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
) {
	CM_LOG_FCT ("cm_nare_request_lower")

	CM_NARE_SET_USER_ID_HANDLE_DEV_AR (nare, channel->my_handle, device_nr, ar_nr);

	CM_NARE_SET_HANDLE (nare, cm_nare_channel_get(channel)->lower.handle);
	CM_NARE_SET_OPCODE (nare, opcode);

	CM_NARE_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> nare-rqb(0x%08x) nare-opc(%u) f-h(%u) f-dev(%u) f-ar(%u)"
		, nare, CM_NARE_GET_OPCODE (nare)
		, CM_NARE_GET_USER_ID_HANDLE (nare), CM_NARE_GET_USER_ID_DEV (nare), CM_NARE_GET_USER_ID_AR (nare)
		);

	CM_NARE_REQUEST_LOWER (nare, cm_nare_channel_get(channel)->sysptr);
}


/*===========================================================================*/
/*=== NARE LOWER DONE =======================================================*/
/*===========================================================================*/

LSA_VOID  CM_NARE_LOWER_IN_FCT_ATTR
cm_nare_request_lower_done (
	CM_NARE_LOWER_RQB_PTR_TYPE  nare
) {
	CM_LOG_FCT ("cm_nare_request_lower_done")
	CM_CHANNEL_PTR_TYPE  channel;

	if( is_null(nare) ) {

		CM_NARE_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL,
			"is_null(nare)"
			);

		CM_NARE_LOWER_RQB_ERROR (nare);
		return;
	}

	channel = cm_channel_from_handle (CM_NARE_GET_USER_ID_HANDLE (nare));

	if( is_null (channel) ) {

		CM_NARE_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"invalid handle(%u)",
			CM_NARE_GET_USER_ID_HANDLE (nare)
			);

		CM_NARE_LOWER_RQB_ERROR (nare);
		return;
	}

	switch (CM_NARE_GET_RESPONSE (nare)) {
	case NARE_RSP_OK:
	case NARE_RSP_OK_CANCEL:
	case NARE_RSP_ERR_DCP_STATION_NOT_FOUND:
		CM_NARE_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< nare-rqb(0x%08x) nare-opc(%u) nare-rsp(%u) t-h(%u) t-dev(%u) t-ar(%u)"
			, nare, CM_NARE_GET_OPCODE (nare), CM_NARE_GET_RESPONSE (nare)
			, CM_NARE_GET_USER_ID_HANDLE (nare), CM_NARE_GET_USER_ID_DEV (nare), CM_NARE_GET_USER_ID_AR (nare)
			);
		break;

	default:
		CM_NARE_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "<<< nare-rqb(0x%08x) nare-opc(%u) nare-rsp(%u) t-h(%u) t-dev(%u) t-ar(%u)"
			, nare, CM_NARE_GET_OPCODE (nare), CM_NARE_GET_RESPONSE (nare)
			, CM_NARE_GET_USER_ID_HANDLE (nare), CM_NARE_GET_USER_ID_DEV (nare), CM_NARE_GET_USER_ID_AR (nare)
			);
		break;
	}

	switch (channel->detailptr->path_type) {

	case CM_PATH_TYPE_NARE:

		switch (CM_NARE_GET_OPCODE (nare)) {

		case NARE_OPC_OPEN_CHANNEL:
			cm_nare_channel_open_done (channel, nare);
			break;

		case NARE_OPC_CLOSE_CHANNEL:
			cm_nare_channel_close_done (channel, nare);
			break;

		case NARE_OPC_RESOLVE_CONFIG_NOTIFY:
			cm_nare_resolve_notify_done (channel, nare);
			break;

		default:
			CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_NARE_GET_OPCODE (nare)
				);
			CM_FATAL1 (CM_NARE_GET_OPCODE (nare));
			break;
		}
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:

		switch (CM_NARE_GET_OPCODE (nare)) {

		case NARE_OPC_RESOLVE_CONFIG:
			cm_cl_nare_resolve_done (channel, nare);
			break;

		case NARE_OPC_RESOLVE_CONFIG_CANCEL:
			cm_cl_nare_resolve_cancel_done (channel, nare);
			break;

		default:
			CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_NARE_GET_OPCODE (nare)
				);
			CM_FATAL1 (CM_NARE_GET_OPCODE (nare));
			break;
		}
		break;
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_SERVER:
	case CM_PATH_TYPE_MULTICAST:

		switch (CM_NARE_GET_OPCODE (nare)) {

		case NARE_OPC_RESOLVE_CONFIG:
			CM_ASSERT (nare->args.ResConf.AliasCnt == 0);
			if( nare->args.ResConf.RequestID != 0 ) {
				CM_AR_GRAPH_IO_CR_PTR_TYPE iocr = (CM_AR_GRAPH_IO_CR_PTR_TYPE)nare->args.ResConf.RequestID;
				CM_ASSERT(iocr->cm.backlink->device_nr == CM_NARE_GET_USER_ID_DEV(nare));
				CM_ASSERT(iocr->cm.backlink->ar_nr == CM_NARE_GET_USER_ID_AR(nare));
				cm_arsm_mcons_resolve_done(channel, iocr, nare);
			}
			else {
				CM_NARE_LOWER_RQB_ERROR(nare); /* not as specified */
			}
			break;

		case NARE_OPC_RESOLVE_CONFIG_CANCEL:
			cm_arsm_mcons_resolve_cancel_done(channel, (CM_AR_GRAPH_IO_CR_PTR_TYPE)nare->args.ResConfCancel.RequestID, nare);
			break;

		default:
			CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_NARE_GET_OPCODE (nare)
				);
			CM_FATAL1 (CM_NARE_GET_OPCODE (nare));
			break;
		}
		break;
#endif

	default:
		CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unknown path_type(%u)", channel->detailptr->path_type
			);
		CM_FATAL1 (channel->detailptr->path_type);
		break;
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ NARE; CM_CFG_USE_NARE=" CM_STRINGIFY(CM_CFG_USE_NARE))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_NARE */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
