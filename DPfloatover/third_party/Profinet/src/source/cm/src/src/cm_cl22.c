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
/*  F i l e               &F: cm_cl22.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for oha                                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	622
#define CM_MODULE_ID		622

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_oha_has_equal_ip_suite (
	CM_CL_DEVICE_PTR_TYPE  dev,
	OHA_IP_TYPE  * CM_LOCAL_MEM_ATTR  ip_addr,
	OHA_IP_TYPE  * CM_LOCAL_MEM_ATTR  ip_subnet_mask,
	OHA_IP_TYPE  * CM_LOCAL_MEM_ATTR  ip_gateway
)
{
	CM_LOG_FCT("cm_cl_oha_has_equal_ip_suite")
	LSA_UINT32 ip, subnet, gateway;

	CM_ASSERT (sizeof(*ip_addr) == sizeof(dev->dev_addr.device_ip_addr));


	CM_MEMCPY (& ip,  ip_addr, sizeof (ip));
	CM_MEMCPY (& subnet, ip_subnet_mask, sizeof (subnet));
	CM_MEMCPY (& gateway, ip_gateway, sizeof (gateway));


	if (ip != dev->dev_addr.device_ip_addr) {
		CM_CL_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
			, "got ip(0x%x) but need ip(0x%x)", ip, dev->dev_addr.device_ip_addr
			);
		return LSA_FALSE;
	}
	{
	LSA_UINT32  local_ip_addr, local_subnet_mask, local_gateway_ip;

	if ( ! cm_oha_get_ip_suite (dev->channel, & local_ip_addr, & local_subnet_mask, & local_gateway_ip))
	{
		CM_CL_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP
			, "CMCL has no own ip-suite => no FSU"
			);
		return LSA_FALSE;
	}
	if (subnet != local_subnet_mask)
	{
		CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) got subnet(0x%x) but need subnet(0x%x)"
			, dev->device_nr, subnet, local_subnet_mask
			);
		return LSA_FALSE;
	}
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_ar_set_oha_ip (
	CM_AR_GRAPH_PTR_TYPE  argr,
	OHA_MAC_TYPE  * CM_LOCAL_MEM_ATTR  mac_addr,
	OHA_IP_TYPE  * CM_LOCAL_MEM_ATTR  ip_addr,
	OHA_IP_TYPE  * CM_LOCAL_MEM_ATTR  ip_subnet_mask,
	OHA_IP_TYPE  * CM_LOCAL_MEM_ATTR  ip_gateway
)
{
	CM_LOG_FCT("cm_cl_ar_set_oha_ip")
	CM_AR_GRAPH_PTR_TYPE  argr_loop;
	CM_MAC_TYPE  dev_mac_addr;
	LSA_UINT32  dev_ip_addr;
	LSA_UINT32  dev_ip_subnet;
	LSA_UINT32  dev_ip_gateway;

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
LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_oha_hello_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	OHA_UPPER_HELLO_PTR_TYPE  hello
)
{
	CM_LOG_FCT ("cm_cl_oha_hello_ind")
	CM_AR_GRAPH_PTR_TYPE  argr;
	LSA_UINT16  device_nr;
	LSA_UINT16  ar_nr;


	if (hello->MacAddr[0] == 0 && hello->MacAddr[1] == 0 && hello->MacAddr[2] == 0
		&&  hello->MacAddr[3] == 0 && hello->MacAddr[4] == 0 && hello->MacAddr[5] == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "hello.ind ignored, no mac-addr");
		return;
	}

	if (hello->IPAddr[0] == 0 && hello->IPAddr[1] == 0
		&& hello->IPAddr[2] == 0 &&  hello->IPAddr[3] == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "hello.ind ignored, no ip-addr");
		return;
	}


	device_nr = CM_CL_U32_GET_DEVICE_NR (hello->FilterID);
	ar_nr = CM_CL_U32_GET_AR_NR (hello->FilterID);

	argr = cm_cl_ar_from_device_ar (channel, device_nr, ar_nr);
	if (is_null (argr)) {
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "hello.ind to unknown ar, FilterID(0x%x)"
			, hello->FilterID
			);
		return;
	}

	if (argr->cm.cl.dev->state != CM_CL_DEV_ST_ACTIVE) {
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) ar(%u): not active, state(%u), ignore hello.ind"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.dev->state
			);
		return;
	}

		/* check ip-suite if we know the right one, AP00492906 */
	if (argr->cm.cl.dev->dev_addr.properties == CM_NARE_MODE_CONFIG_IP_FIXED)
	{
		LSA_BOOL rc = cm_cl_oha_has_equal_ip_suite (argr->cm.cl.dev
			, & hello->IPAddr, & hello->SubnetMask, & hello->DefaultRouterAddr
			);
		if (! rc) {
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) ar(%u): got wrong ip-suite, ignore hello.ind"
				, argr->device_nr, argr->ar_nr
				);
			return;
		}
	}

	switch (argr->cm.cl.state) {
	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
		cm_cl_ar_set_oha_ip (argr
			, & hello->MacAddr
			, & hello->IPAddr, & hello->SubnetMask, & hello->DefaultRouterAddr
			);
		if (argr->cm.cl.state == CM_DST_WF_SCHEDULE) {
			cm_cl_schedule_fsu (argr);
		}
		else {
			/* hello.ind contains the same data as nare_resolve.cnf does */
			cm_cl_arsm (argr);
		}
		break;

	default:
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): hello.ind too late, ignore it in ar_state(%u), wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		break;
	}
}


/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
