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
/*  F i l e               &F: cm_cl20.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for nare                                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	620
#define CM_MODULE_ID		620

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_NARE

LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_nare_resolve (
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_PTR_TYPE  argr
)
{
	CM_LOG_FCT ("cm_cl_nare_resolve")
	LSA_UINT8  mode;
	LSA_UINT16  cnt, indata;
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;
	LSA_UINT32  notify_id;
	LSA_BOOL  check_ip;
	LSA_BOOL  set_ip_remanent;
	LSA_UINT32  ident_timeout;
	LSA_UINT32  local_ip_addr, local_subnet_mask, local_gateway_ip;
	LSA_UINT32  device_ip_addr, device_subnet_mask, device_gateway_ip;
	CM_CL_DOWN_INFO_TYPE  dinfo;
	CM_LIST_ENTRY_PTR_TYPE  device_alias_list;
	LSA_UINT16  vendor_id;
	LSA_UINT16  device_id;
	LSA_UINT16  instance_id;
	LSA_BOOL  nos_overwrite;
	CM_UPPER_MEM_U8_PTR_TYPE device_name;
	LSA_UINT16  device_name_length;

	CM_ASSERT (argr->cm.cl.state == CM_DST_NARE);

	cnt = cm_cl_dev_ar_count (dev, LSA_NULL, LSA_NULL, & indata, LSA_NULL);

	if (indata > 0)
	{
		LSA_UNUSED_ARG (cnt);
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u) nare-resolve: use previous resolved ip_addr(%08x),  ar-count(%d) indata(%d)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.dev->dev_addr.device_ip_addr
			, cnt, indata
			);
		CM_ASSERT (argr->cm.cl.state == CM_DST_NARE);
		cm_cl_arsm (argr);
		return;
	}
	
	if (channel->usr.cl.nos_assignment == CM_CL_CONTROL_NOS_ASSIGNMENT_ON)
	{
		device_alias_list =  & dev->dev_addr.device_alias_list;

		switch (dev->dev_addr.properties & CM_NARE_MODE_MASK)
		{
		case CM_NARE_MODE_CONFIG_IP_DNS:
			mode = NARE_MODE_CONFIG_IP_DNS;
			break;

		case CM_NARE_MODE_RESOLVE_DCP:
			if (CmListIsEmpty (& dev->dev_addr.device_alias_list))	/* AP00717602 */
			{
				mode = NARE_MODE_RESOLVE_DCP;
			}
			else
			{
				mode = NARE_MODE_CONFIG_NOS_FIXED;
			}
			break;

		case CM_NARE_MODE_RESOLVE_DNS:
			mode = NARE_MODE_RESOLVE_DNS;
			break;

		case CM_NARE_MODE_CONFIG_IP_FIXED:
			mode = NARE_MODE_CONFIG_IP_FIXED;
			break;

		default:
			CM_FATAL1(dev->dev_addr.properties);
			return;
		}
	}
	else /* CM_CL_CONTROL_NOS_ASSIGNMENT_OFF */
	{
		device_alias_list = LSA_NULL;

		switch (dev->dev_addr.properties & CM_NARE_MODE_MASK)
		{
		case CM_NARE_MODE_CONFIG_IP_DNS:
			mode = NARE_MODE_CONFIG_IP_DNS; /* + no alias, optional set-ip */
			break;

		case CM_NARE_MODE_RESOLVE_DNS:
			mode = NARE_MODE_RESOLVE_DNS; /* + no alias */
			break;

		case CM_NARE_MODE_CONFIG_IP_FIXED:
			mode = NARE_MODE_CONFIG_IP_FIXED; /* + no alias, optional set-ip */
			break;

		case CM_NARE_MODE_RESOLVE_DCP:
			mode = NARE_MODE_RESOLVE_DCP; /* + no alias */
			break;

		default:
			CM_FATAL1(dev->dev_addr.properties);
			return;
		}
	}

	switch (dev->dev_addr.properties & CM_NARE_SETIP_MASK)
	{
	case CM_NARE_SETIP_TEMPORARY:
		set_ip_remanent = LSA_FALSE;
		break;

	case CM_NARE_SETIP_REMANENT:
		set_ip_remanent = LSA_TRUE;
		break;

	default:
		CM_FATAL1(dev->dev_addr.properties);
		return;
	}

	switch (dev->planned_mode & CM_CL_PMODE_IS_ACCELERATED_MASK)
	{
	case CM_CL_PMODE_IS_ACCELERATED_NO:
		notify_id = 0;
		break;

	case CM_CL_PMODE_IS_ACCELERATED_YES:
		notify_id = CM_CL_U32_SET_DEV_AR (argr->device_nr, argr->ar_nr);
		break;

	default:
		CM_FATAL1(dev->planned_mode);
		return;
	}

	switch (dev->planned_mode & CM_CL_PMODE_OPTIMIZED_IP_CHECK_MASK)
	{
	case CM_CL_PMODE_OPTIMIZED_IP_CHECK_NO:
		check_ip = LSA_TRUE;
		break;

	case CM_CL_PMODE_OPTIMIZED_IP_CHECK_YES:
		check_ip = LSA_FALSE;
		break;

	default:
		CM_FATAL1(dev->planned_mode);
		return;
	}

	switch (dev->planned_mode & CM_CL_PMODE_CHECK_DEVICE_ID_MASK)
	{
	case CM_CL_PMODE_CHECK_DEVICE_ID_NO:
		vendor_id = 0;
		device_id = 0;
		instance_id = 0;
		break;

	case CM_CL_PMODE_CHECK_DEVICE_ID_YES:
		vendor_id = dev->dev_rpc.vendor_id;
		device_id = dev->dev_rpc.device_id;
		instance_id = dev->dev_rpc.instance;
		break;

	default:
		CM_FATAL1(dev->planned_mode);
		return;
	}

	switch (dev->planned_mode & CM_CL_PMODE_NOS_OVERWRITE_MASK)
	{
	case CM_CL_PMODE_NOS_OVERWRITE_NO:
		nos_overwrite = LSA_FALSE;
		break;

	case CM_CL_PMODE_NOS_OVERWRITE_YES:
		nos_overwrite = LSA_TRUE;
		break;

	default:
		CM_FATAL1(dev->planned_mode);
		return;
	}


	ident_timeout = CM_CL_DEV_SCAN_CYCLE_10MS (dev->planned_mode);
	if (ident_timeout != 0)
	{
		CM_ASSERT (10 <= ident_timeout && ident_timeout <= 200);
		ident_timeout = ident_timeout * 10;
	}
	if (ident_timeout == 0)
	{
		ident_timeout = 1000;
	}

	if (dev->dev_addr.tailored_device_name_length > 0)
	{
		device_name = dev->dev_addr.tailored_device_name;
		device_name_length = dev->dev_addr.tailored_device_name_length;
	}
	else
	{
		device_name = dev->dev_addr.device_name;
		device_name_length = dev->dev_addr.device_name_length;
	}

	CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) /nare-resolve:  device_name_length(%u) known-ip(0x%x) mode(%u) set_ip_remanent(%u) notify_id(0x%x) check_ip(%u) ident_timeout(%u)msec"
		, argr->device_nr, argr->ar_nr
		, device_name_length, CM_NTOH32(dev->dev_addr.device_ip_addr)
		, mode, set_ip_remanent, notify_id, check_ip, ident_timeout
		);

	if ( ! cm_oha_get_ip_suite (channel, & local_ip_addr, & local_subnet_mask, & local_gateway_ip))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "local ip_addr is not valid, can't resolve"
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_IPSUITE));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	device_ip_addr		= dev->dev_addr.device_ip_addr;
	device_subnet_mask	= local_subnet_mask;
	device_gateway_ip	= local_gateway_ip;
	if (local_gateway_ip == local_ip_addr  ||  local_gateway_ip == 0) /* means "no gateway", see part 6 */
	{
		/* device_gateway_ip = local_ip_addr;	<= this means AP01314238
		**											but this doesn't work anymore with IE/PB-Link, so we must find an other solution */
		device_gateway_ip = device_ip_addr; /* AP01314238_02(Rueckbau) */
	}

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);

	if ( ! cm_nare_resolve_alias (channel, local_ip_addr
		, mode, check_ip
		, CM_CAST_U8 (device_name_length) /* range 1..240 */, device_name
		, device_ip_addr, device_subnet_mask, device_gateway_ip, set_ip_remanent
		, (LSA_UINT16)ident_timeout, argr->device_nr, argr->ar_nr
		, notify_id, argr /* cancel_request_id for nare-cancel */
		, device_alias_list /* may LSA_NULL */
		, nos_overwrite, vendor_id, device_id, instance_id))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_ar_set_nare_ip (
	CM_AR_GRAPH_PTR_TYPE  argr,
	NARE_UPPER_RQB_MAC_PTR_TYPE  mac_addr,
	NARE_UPPER_RQB_IP_PTR_TYPE  ip_addr,
	NARE_UPPER_RQB_IP_PTR_TYPE  ip_subnet_mask,
	NARE_UPPER_RQB_IP_PTR_TYPE  ip_gateway
)
{
	CM_LOG_FCT("cm_cl_ar_set_nare_ip")
	CM_MAC_TYPE  dev_mac_addr;
	LSA_UINT32  dev_ip_addr;
	LSA_UINT32  dev_ip_subnet;
	LSA_UINT32  dev_ip_gateway;

	CM_AR_GRAPH_PTR_TYPE  argr_loop;

	CM_ASSERT (sizeof(*mac_addr) == sizeof(argr->ar.eth_mac));
	CM_ASSERT (sizeof(*ip_addr) == sizeof(argr->ar.host_ip));

	CM_MEMCPY (& dev_mac_addr, mac_addr, sizeof (dev_mac_addr));
	CM_MEMCPY (& dev_ip_addr,  ip_addr, sizeof (dev_ip_addr));
	CM_MEMCPY (& dev_ip_subnet, ip_subnet_mask, sizeof (dev_ip_subnet));
	CM_MEMCPY (& dev_ip_gateway, ip_gateway, sizeof (dev_ip_gateway));

	for (CmListForeach (argr_loop, & argr->cm.cl.dev->ar_list, CM_AR_GRAPH_PTR_TYPE))
	{
		cm_cl_ar_event_set_ip (argr, & dev_mac_addr, dev_ip_addr, dev_ip_subnet, dev_ip_gateway);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_nare_set_dinfo (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  rsp,
	CM_CL_DOWN_INFO_PTR_TYPE  dinfo
	)
{
	switch (rsp)
	{
	case NARE_RSP_OK:
		CM_CL_DINFO_SET (dinfo, 0, 0, 0);
		break;

	case NARE_RSP_ERR_DNS_FAILED:
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IOD_NOT_FOUND, CM_PNIO_FAULTY_CTLDINA(1/*DNS unknown RealStationName*/));
		break;

	case NARE_RSP_ERR_DCP_STATION_NOT_FOUND:
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IOD_NOT_FOUND, CM_PNIO_FAULTY_CTLDINA(4/*DCP no StationName*/));
		break;

	case NARE_RSP_ERR_DCP_MULTIPLE_STATIONS:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_NOS_MULTIPLE, CM_PNIO_FAULTY_CTLDINA(3/*DCP multiple RealStationName*/), & argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case NARE_RSP_ERR_DCP_SET_FAILED:
	case NARE_RSP_ERR_DCP_SET_TIMEOUT:
	case NARE_RSP_ERR_IP_IN_USE:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IP_SET_ABORTED, CM_PNIO_FAULTY_CTLDINA(6/*DCP set error*/), & argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case NARE_RSP_ERR_MULTIPLE_IP_USE:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IP_MULTIPLE, CM_PNIO_FAULTY_CTLDINA(3/*DCP multiple RealStationName*/), & argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case NARE_RSP_ERR_LL:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IP_SET_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active*/), & argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case NARE_RSP_ERR_IP_RESOLVE_NO_ANSWER: /* no ARP answer */
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IOD_NOT_FOUND, CM_PNIO_FAULTY_CTLDINA(5/*No IP address*/));
		break;

	case NARE_RSP_ERR_STATION_NAME_DIFFERENT:
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_NOS_SET_ABORTED, CM_PNIO_FAULTY_CTLDINA(6/*DCP set error*/));
		break;

	case NARE_RSP_ERR_DCP_ID_MISMATCH:
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_VENDORID_ABORTED, CM_PNIO_FAULTY_CTLDINA(6/*DCP set error*/));
		break;

	case NARE_RSP_ERR_NO_OWN_IP: /* not used by nare, 20111018 */
	case NARE_RSP_ERR_NO_TIMER: /* not used by nare, 20111018 */
	case NARE_RSP_ERR_INVALID_IP_PARAMS: /* ERR_PARAM */
	case NARE_RSP_ERR_DCP_IDENT_TLV_ERROR: /* ERR_PROTOCOL */
	default:
		CM_CL_DINFO_SET2 (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_NARE), rsp);
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: docu data2")
#endif /* CM_MESSAGE */
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_nare_resolve_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  nare
)
{
	CM_LOG_FCT ("cm_cl_nare_resolve_done")
	LSA_UINT16  rsp;
	LSA_UINT16  device_nr;

	rsp = CM_NARE_GET_RESPONSE (nare);

	device_nr = CM_CL_GET_DEVICE_NR (CM_NARE_GET_USER_ID_DEV (nare));

	switch (CM_CL_GET_SUBSYS (CM_NARE_GET_USER_ID_DEV (nare)))
	{
	case CM_CL_DEV_SUBSYS_DEV:
		{
		CM_AR_GRAPH_PTR_TYPE  argr;
		CM_CL_DOWN_INFO_TYPE  dinfo;

		argr = cm_cl_ar_from_device_ar (channel, device_nr, CM_NARE_GET_USER_ID_AR (nare));
		if (is_null (argr)) {
			CM_NARE_LOWER_RQB_ERROR (nare);
			return;
		}

		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);

		if (rsp == NARE_RSP_OK)
		{
			if (
				nare->args.ResConf.Res.IPAddr[0] == 0
			&&  nare->args.ResConf.Res.IPAddr[1] == 0
			&&  nare->args.ResConf.Res.IPAddr[2] == 0
			&&  nare->args.ResConf.Res.IPAddr[3] == 0)
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "dev(%u) ar(%u) /nare-resolve-done  with ip(0.0.0.0), internal down"
					, argr->device_nr, argr->ar_nr
					);
				rsp = NARE_RSP_ERR_LL;
			}
			else if (
				nare->args.ResConf.Res.MACAddr[0] == 0
			&&  nare->args.ResConf.Res.MACAddr[1] == 0
			&&  nare->args.ResConf.Res.MACAddr[2] == 0
			&&  nare->args.ResConf.Res.MACAddr[3] == 0
			&&  nare->args.ResConf.Res.MACAddr[4] == 0
			&&  nare->args.ResConf.Res.MACAddr[5] == 0)
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "dev(%u) ar(%u) /nare-resolve-done  with mac(00-00-00-00-00-00), internal down"
					, argr->device_nr, argr->ar_nr
					);
				rsp = NARE_RSP_ERR_LL;
			}
			else if (! nare->args.ResConf.StationNameSetEqual) /* see AP00744413 */
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "dev(%u) ar(%u) /nare-resolve-done  with station-name not-set or not-equal, internal down"
					, argr->device_nr, argr->ar_nr
					);
				rsp = NARE_RSP_ERR_STATION_NAME_DIFFERENT; /* needed by ErrorCause CM_ERROR_CAUSE_NOS_SET_ABORTED */
			}
		}

		switch (argr->cm.cl.state)
		{
		case CM_DST_NARE:
			if (rsp == NARE_RSP_OK)
			{
				cm_cl_ar_set_nare_ip (argr
					, & nare->args.ResConf.Res.MACAddr
					, & nare->args.ResConf.Res.IPAddr
					, & nare->args.ResConf.Res.SubMask
					, & nare->args.ResConf.Res.DefRouter
					);
				CM_CL_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u) ar(%u)  STARTUP /nare-resolve-done  ip(%u.%u.%u.%u)"
					, argr->device_nr, argr->ar_nr
					, nare->args.ResConf.Res.IPAddr[0], nare->args.ResConf.Res.IPAddr[1]
					, nare->args.ResConf.Res.IPAddr[2], nare->args.ResConf.Res.IPAddr[3]
					);
				cm_cl_arsm (argr);
			}
			else
			{
				CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "dev(%u) ar(%u) /nare-resolve-done  nare_rsp(%u), set to down"
					, argr->device_nr, argr->ar_nr, rsp
					);
				cm_cl_ar_set_nare_ip (argr
					, & nare->args.ResConf.Res.MACAddr
					, & nare->args.ResConf.Res.IPAddr
					, & nare->args.ResConf.Res.SubMask
					, & nare->args.ResConf.Res.DefRouter
					); /* AP00525178,AP00529108: set conflicting ip-addr */

				cm_cl_nare_set_dinfo (argr, rsp, &dinfo);
				cm_cl_arsm_down (argr, &dinfo);
			}
			break;

		case CM_DST_WF_DOWN:
			cm_cl_arsm (argr);
			break;

		default:
			if (rsp == NARE_RSP_OK  ||  rsp == NARE_RSP_ERR_DCP_STATION_NOT_FOUND)
			{
				CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "dev(%u) ar(%u) /nare-resolve-done  nare_rsp(%u) and state(%u) => ignore"
					, argr->device_nr, argr->ar_nr, rsp, argr->cm.cl.state
				);
			}
			else
			{
				CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "dev(%u) ar(%u) /nare-resolve-done  nare_rsp(%u) in state(%u), set to down"
					, argr->device_nr, argr->ar_nr, rsp, argr->cm.cl.state
					);

				cm_cl_nare_set_dinfo (argr, rsp, &dinfo);
				cm_cl_arsm_down (argr, &dinfo);
			}
			break;
		}

		cm_nare_free_rqb_alias (channel, nare);
		}
		break;

#if CM_CFG_MAX_CL_IMPLICIT_READ
	case CM_CL_DEV_SUBSYS_READ_IMPL:
		{
		CM_CL_READ_IMPLICIT_PTR_TYPE  ri;

		ri = cm_cl_ri_from_device_nr__nn (channel, device_nr);

		cm_cl_ri_nare_resolve_done (channel, ri, nare);
		}
		break;
#endif

	default:
		CM_FATAL1 (device_nr);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_nare_resolve_cancel_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  nare
)
{
	CM_LOG_FCT ("cm_cl_nare_resolve_cancel_done")
	LSA_UINT16  device_nr;
	LSA_UINT16  ar_nr;
	CM_AR_GRAPH_PTR_TYPE  n_argr;

	device_nr = CM_NARE_GET_USER_ID_DEV (nare);
	ar_nr = CM_NARE_GET_USER_ID_AR (nare);
	n_argr = (CM_AR_GRAPH_PTR_TYPE) nare->args.ResConfCancel.RequestID;

	cm_nare_free_rqb (channel, nare);

	switch (CM_CL_GET_SUBSYS (device_nr))
	{
	case CM_CL_DEV_SUBSYS_DEV:
		{
		CM_AR_GRAPH_PTR_TYPE  argr;

		argr = cm_cl_ar_from_device_ar (channel, CM_CL_GET_DEVICE_NR (device_nr), ar_nr);
		if (is_null (argr)) {
			CM_FATAL1 (device_nr);
			return;
		}
		CM_ASSERT (are_equal (n_argr, argr));
		LSA_UNUSED_ARG (n_argr);

		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_NARE_CANCEL_CNF);

		cm_cl_arsm (argr);
		}
		break;

	case CM_CL_DEV_SUBSYS_READ_IMPL:
	default:
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "wrong dev(%u)", device_nr);
		CM_FATAL1 (device_nr);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_nare_notify_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	NARE_UPPER_RESOLVE_CONFIG_NOTIFY_PTR_TYPE  notify
)
{
	CM_LOG_FCT ("cm_cl_nare_notify_ind")
	CM_AR_GRAPH_PTR_TYPE  argr;
	LSA_UINT16  device_nr;
	LSA_UINT16  ar_nr;

	if (notify->Res.IPAddr[0] == 0 && notify->Res.IPAddr[1] == 0
		&& notify->Res.IPAddr[2] == 0 &&  notify->Res.IPAddr[3] == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "notify.ind ignored, no ip-addr");
		return;
	}

	if (notify->Res.MACAddr[0] == 0 && notify->Res.MACAddr[1] == 0 && notify->Res.MACAddr[2] == 0
		&&  notify->Res.MACAddr[3] == 0 && notify->Res.MACAddr[4] == 0 && notify->Res.MACAddr[5] == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "notify.ind ignored, no mac-addr");
		return;
	}

	device_nr = CM_CL_U32_GET_DEVICE_NR (notify->NotifyID);
	ar_nr = CM_CL_U32_GET_AR_NR (notify->NotifyID);

	argr = cm_cl_ar_from_device_ar (channel, device_nr, ar_nr);
	if (is_null (argr))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "notify.ind to unknown ar, NotifyID(0x%x)", notify->NotifyID
			);
		return;
	}

	switch (argr->cm.cl.state)
	{
	case CM_DST_NARE:
		cm_cl_ar_set_nare_ip (argr
			, & notify->Res.MACAddr
			, & notify->Res.IPAddr, & notify->Res.SubMask, & notify->Res.DefRouter
			);

		/* notify.ind contains the same data as nare_resolve.cnf does */
		CM_CL_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) ar(%u)  STARTUP /accelerated  ip(%u.%u.%u.%u)"
			, argr->device_nr, argr->ar_nr
			, notify->Res.IPAddr[0], notify->Res.IPAddr[1]
			, notify->Res.IPAddr[2], notify->Res.IPAddr[3]
			);
		cm_cl_arsm (argr);
		break;

	default:
		/* ignore notify.ind */
		break;
	}
}

#endif

/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
