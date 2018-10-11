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
/*  F i l e               &F: cm_arsm3.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  AR state-machines                                                        */
/*  - m-consumer state-machine                                               */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  73
#define CM_MODULE_ID       73

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP
#if CM_CFG_USE_NARE

/*===========================================================================*/

enum cm_cr_mc_flags_enum {

	CM_CR_MC_WF_MASK    = 0x0007, /* bit 0,1,2 */
	CM_CR_MC_WF_UPPER   = 0x0001,
	CM_CR_MC_WF_RESOLVE = 0x0002,
	CM_CR_MC_WF_CANCEL  = 0x0004,

	CM_CR_MC_RESOLVED   = 0x0008, /* bit 3 */

	CM_CR_MC_TELL_MASK  = 0x00F0, /* bit 4,5,6,7 */
	CM_CR_MC_TELL_INFO  = 0x0010,
	CM_CR_MC_TELL_DOWN  = 0x0020,
	CM_CR_MC_TELL_UP    = 0x0040,
	CM_CR_MC_TOLD_UP    = 0x0080
};

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_resolve(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
); /* forward */

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
); /* forward */

/*===========================================================================*/
/*=== MCONS UPPER-INDICATION ================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_upper_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr,
	LSA_UINT16 nare_rsp,
	CM_MAC_PTR_TYPE mac,
	LSA_UINT32 ip,
	LSA_UINT32 subnet_mask,
	LSA_UINT32 gateway_ip
) {
	CM_LOG_FCT("cm_arsm_mcons_upper_indication")
	LSA_BOOL wait;

	CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) cr_state(%u) mc_flags(0x%x)"
		, iocr
		, iocr->cm.cr_state
		, iocr->cm.mc_flags
		);

	/***/

	if( (iocr->cm.mc_flags & CM_CR_MC_WF_UPPER) != 0 ) {
		/* response pending */
		return;
	}

	/***/

	wait = LSA_FALSE; /* true: wait for the upper-response */

	/*
	 * TELL flags
	 *
	 *  told tell tell tell | meaning
	 *    up   up down info |
	 *     0    0   0   0   | nothing to tell or all told
	 *     0    0   0   1   | tell addr-info / resolved
	 *     0    0   1   0   | tell addr-info / miss
	 *     0    0   1   1   | not possible
	 *     0    1   0   0   | tell in-data
	 *     0    1   0   1   | not possible
	 *     0    1   1   0   | tell addr-info / miss (in-data not told)
	 *     0    1   1   1   | not possible
	 *     1    0   0   0   | nothing to tell (in-data told)
	 *     1    0   0   1   | not possible
	 *     1    0   1   0   | tell offline
	 *     1    0   1   1   | not possible
	 *     1    1   0   0   | not possible
	 *     1    1   0   1   | not possible
	 *     1    1   1   0   | not possible
	 *     1    1   1   1   | not possible
	 *
	 */

	switch( (iocr->cm.mc_flags & CM_CR_MC_TELL_MASK) ) {

	case 0:
		/* nothing to tell */
		break;

	case CM_CR_MC_TELL_INFO:

		/* addr-info.ind (resolve) */

		iocr->cm.mc_flags &= ~CM_CR_MC_TELL_INFO;

		switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_SERVERS && CM_CFG_SV_AR_HAS_MCONS
		case CM_PATH_TYPE_SERVER:
			wait = cm_sv_mcons_indication(channel, iocr, CM_OPC_SV_EVENT_MCONS_ADDR_INFO_IND, nare_rsp, mac, ip, 0);
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			wait = cm_mc_mcons_nare_indication(channel, iocr, nare_rsp, mac, ip, subnet_mask, gateway_ip);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			LSA_UNUSED_ARG (nare_rsp);
			LSA_UNUSED_ARG (mac);
			LSA_UNUSED_ARG (ip);
			LSA_UNUSED_ARG (gateway_ip);
			LSA_UNUSED_ARG (subnet_mask);
			break;
		}
		break;

	case CM_CR_MC_TELL_DOWN:

		/* addr-info.ind (miss) */

		iocr->cm.mc_flags &= ~CM_CR_MC_TELL_DOWN;

		switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_SERVERS && CM_CFG_SV_AR_HAS_MCONS
		case CM_PATH_TYPE_SERVER:
			wait = cm_sv_mcons_indication(channel, iocr, CM_OPC_SV_EVENT_MCONS_ADDR_INFO_IND, 0, LSA_NULL, 0, 0);
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			cm_mc_ar_com_mcr_down(channel, iocr, CM_AR_MCONS_WF_UP, CM_AR_REASON_MISS);
			CM_ASSERT( ! wait);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
		break;

	case (CM_CR_MC_TELL_DOWN | CM_CR_MC_TELL_UP):

		/* user did not see in-data, same as addr-info.ind (miss) */

		iocr->cm.mc_flags &= ~(CM_CR_MC_TELL_DOWN | CM_CR_MC_TELL_UP);

		switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_SERVERS && CM_CFG_SV_AR_HAS_MCONS
		case CM_PATH_TYPE_SERVER:
			wait = cm_sv_mcons_indication(channel, iocr, CM_OPC_SV_EVENT_MCONS_ADDR_INFO_IND, 0, LSA_NULL, 0, 0);
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			cm_mc_ar_com_mcr_down(channel, iocr, CM_AR_MCONS_WF_UP, CM_AR_REASON_MISS);
			CM_ASSERT( ! wait);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
		break;

	case (CM_CR_MC_TELL_DOWN | CM_CR_MC_TOLD_UP):

		/* offline.ind */

		iocr->cm.mc_flags &= ~(CM_CR_MC_TELL_DOWN | CM_CR_MC_TOLD_UP);

		switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_SERVERS && CM_CFG_SV_AR_HAS_MCONS
		case CM_PATH_TYPE_SERVER:
			wait = cm_sv_mcons_indication(channel, iocr, CM_OPC_SV_EVENT_MCONS_OFFLINE_IND, 0, LSA_NULL, 0, 0);
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			cm_mc_ar_com_mcr_down(channel, iocr, CM_AR_MCONS_UP, CM_AR_REASON_MISS);
			CM_ASSERT( ! wait);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
		break;

	case CM_CR_MC_TELL_UP:

		/* in-data.ind */

		iocr->cm.mc_flags &= ~CM_CR_MC_TELL_UP;
		iocr->cm.mc_flags |= CM_CR_MC_TOLD_UP;

		switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_SERVERS && CM_CFG_SV_AR_HAS_MCONS
		case CM_PATH_TYPE_SERVER:
			wait = cm_sv_mcons_indication(channel, iocr, CM_OPC_SV_EVENT_MCONS_IN_DATA_IND, 0, LSA_NULL, 0, 0);
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			cm_mc_ar_com_mcr_up(channel, iocr->cm.backlink);
			CM_ASSERT( ! wait);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
		break;

	case CM_CR_MC_TOLD_UP:

		/* look for apdu-status changes */

		switch (channel->detailptr->path_type) {
#if CM_CFG_MAX_SERVERS && CM_CFG_SV_AR_HAS_MCONS
		case CM_PATH_TYPE_SERVER:
			{
			LSA_UINT16 delta = CM_CAST_U16((~ iocr->cm.sv.data_status_ind) & iocr->cm.cons_data_status);

			if( delta != 0 ) {

				wait = cm_sv_mcons_indication(channel, iocr, CM_OPC_SV_EVENT_MCONS_APDU_STATUS_IND, 0, LSA_NULL, 0, delta);

				if( wait ) {

					iocr->cm.sv.data_status_ind = iocr->cm.cons_data_status;
				}
			}
			}
			break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			cm_mc_ar_com_mcr_apdu_status_check(channel, iocr);
			CM_ASSERT( ! wait);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
		break;

	default:
		CM_FATAL();
		break;
	}

	if( wait ) {

		iocr->cm.mc_flags |= CM_CR_MC_WF_UPPER;
	}
	else {

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("MCONS: delayed indication because not yet in-data?")
/*todo, when started before in-data*/
#endif /* CM_MESSAGE */

		cm_arsm_mcons_done(channel, iocr);
	}
}

/*===========================================================================*/
/*=== MCONS UPPER-RESPONSE ==================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_upper_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_upper_response")

	CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) cr_state(%u) mc_flags(0x%x)"
		, iocr
		, iocr->cm.cr_state
		, iocr->cm.mc_flags
		);

	/***/

	if( (iocr->cm.mc_flags & CM_CR_MC_WF_UPPER) == 0 ) {
		/* unexpected */
		CM_FATAL();
	}

	iocr->cm.mc_flags &= ~CM_CR_MC_WF_UPPER;

	/***/

	if( iocr->cm.do_stop ) {

		cm_arsm_mcons_done(channel, iocr);
	}
	else {

		if( (iocr->cm.mc_flags & CM_CR_MC_TELL_INFO) != 0 ) {
			/* cannot be addr-info/nare */
			CM_FATAL();
		}

		cm_arsm_mcons_upper_indication(channel, iocr, 0, LSA_NULL, 0, 0, 0);
	}
}

/*===========================================================================*/
/*=== MCONS START ===========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_start(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_start")

	CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) cr_state(%u) mc_flags(0x%x)"
		, iocr
		, iocr->cm.cr_state
		, iocr->cm.mc_flags
		);

	/***/

	if( ! (
		iocr->cm.cr_state == CM_AR_CR_IDLE
		&& ! iocr->cm.do_stop
		&& iocr->cm.mc_flags == 0
	) ) {
		/* wrong state or flags */
		CM_FATAL();
	}

	/***/

	iocr->cm.cr_state = CM_AR_MCONS_WF_LINK_UP;

	if( cm_link_status_is_up(channel) ) {

		cm_arsm_mcons_link_up(channel, iocr);
	}
}

/*===========================================================================*/
/*=== MCONS LINK-UP =========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_link_up(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_link_up")

	CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X)", iocr
		);

	if( iocr->cm.cr_state == CM_AR_MCONS_WF_LINK_UP ) {

		iocr->cm.cr_state = CM_AR_MCONS_NARE;

		cm_arsm_mcons_resolve(channel, iocr);
	}
}

/*===========================================================================*/
/*=== MCONS RESOLVE =========================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_resolve(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_resolve")
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_AR_GRAPH_MCR_PTR_TYPE  ar_mcr;
	LSA_UINT8 nare_mode;

	CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08x)", iocr
		);

	/***/

	CM_ASSERT(iocr->cm.cr_state == CM_AR_MCONS_NARE);

	if( (iocr->cm.mc_flags & CM_CR_MC_TELL_MASK) != 0 ) {
		/* unexpected */
		CM_FATAL();
	}

	/***/

	argr = iocr->cm.backlink;

	/***/

	ar_mcr = cm_ar_graph_find_mcr(argr, iocr->iocr_ref);

	if( is_null(ar_mcr) ) {
		CM_FATAL(); /* should not happen, see consistency checks */
	}

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: find+assert ersetzen durch get_mcr__not_null()")*/
/*???auch an anderer stelle!*/
#endif /* CM_MESSAGE */

	/***/

	switch( (ar_mcr->mcr_properties & CM_MCR_PROP_PROTOCOL_MASK) ) {

	case CM_MCR_PROP_PROTOCOL_DNS:
		nare_mode = NARE_MODE_RESOLVE_DNS;
		break;

	case CM_MCR_PROP_PROTOCOL_DCP:
		nare_mode = NARE_MODE_RESOLVE_DCP;
		break;

	default:
		CM_FATAL(); /* a bug */
		nare_mode = 0; /* unreachable */
		break;
	}

	/***/

	if(
		cm_nare_resolve(
			channel, 0/*not used*/,
			nare_mode,
			LSA_FALSE,
			ar_mcr->provider_name_length,
			ar_mcr->provider_name,
			argr->device_nr,
			argr->ar_nr,
			iocr
			)
	) {

		if( (iocr->cm.mc_flags & CM_CR_MC_WF_RESOLVE) != 0 ) {
			/* unexpected */
			CM_FATAL();
		}

		iocr->cm.mc_flags |= CM_CR_MC_WF_RESOLVE;
	}
	else {

		/* cannot alloc nare-rqb, do not addr-info.ind */

		CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr(%08X) cannot cm_nare_resolve(), retry after 1sec"
			, iocr
			);

		iocr->cm.cr_state = CM_AR_MCONS_NARE_PAUSE;

		cm_oneshot_trigger(&iocr->cm.mc_timer, 1000);
	}
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_timeout(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE   user_ptr
) {
	CM_LOG_FCT("cm_arsm_mcons_timeout")
	CM_CHANNEL_PTR_TYPE  channel;
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

	/***/

	channel = (CM_CHANNEL_PTR_TYPE)user_ptr;

	iocr = CM_CONTAINING_STRUCT(oneshot, CM_AR_GRAPH_IO_CR_PTR_TYPE, cm.mc_timer);

	CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08x)", iocr
		);

	/***/

	switch( iocr->cm.cr_state ) {

	case CM_AR_MCONS_NARE_PAUSE:
		if( (iocr->cm.mc_flags & CM_CR_MC_WF_UPPER) != 0 ) {
			/* strange, upper-response still outstanding, wait another second */
			cm_oneshot_trigger(&iocr->cm.mc_timer, 1000);
		}
		else {
			iocr->cm.cr_state = CM_AR_MCONS_NARE;
			cm_arsm_mcons_resolve(channel, iocr);
		}
		break;

	default:
		CM_FATAL(); /* a bug */
		break;
	}
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_resolve_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
) {
	CM_LOG_FCT("cm_arsm_mcons_resolve_done")
	LSA_UINT16  nare_rsp;
	CM_MAC_TYPE mac;
	LSA_UINT32  ip;
	LSA_UINT32  gateway_ip;
	LSA_UINT32  subnet_mask;

	/***/

	CM_ASSERT(is_not_null(iocr));

	nare_rsp = CM_NARE_GET_RESPONSE(nare);

	CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X), nare_rsp(%u)"
		, iocr, nare_rsp
		);

	/***/

	if( (iocr->cm.mc_flags & CM_CR_MC_WF_RESOLVE) == 0 ) {
		/* unexpected */
		CM_FATAL();
	}

	iocr->cm.mc_flags &= ~CM_CR_MC_WF_RESOLVE;

	/***/

	if( nare_rsp == NARE_RSP_OK ) {

		iocr->cm.mc_flags |= CM_CR_MC_RESOLVED;
	}
	else {

		iocr->cm.mc_flags &= ~CM_CR_MC_RESOLVED;
	}

	/***/

	CM_ASSERT(sizeof(nare->args.ResConf.Res.MACAddr) == sizeof(mac.mac));

	CM_MEMCPY(mac.mac, nare->args.ResConf.Res.MACAddr, sizeof(mac.mac));

	CM_ASSERT(sizeof(nare->args.ResConf.Res.IPAddr) == sizeof(ip));

	CM_MEMCPY(&ip, nare->args.ResConf.Res.IPAddr, sizeof(ip));

	CM_ASSERT(sizeof(nare->args.ResConf.Res.SubMask) == sizeof(subnet_mask));

	CM_MEMCPY(&subnet_mask, nare->args.ResConf.Res.SubMask, sizeof(subnet_mask));

	CM_ASSERT(sizeof(nare->args.ResConf.Res.DefRouter) == sizeof(gateway_ip));

	CM_MEMCPY(&gateway_ip, nare->args.ResConf.Res.DefRouter, sizeof(gateway_ip));

	/***/

	switch( iocr->cm.cr_state ) {

	case CM_AR_MCONS_NARE:

		if( nare_rsp == NARE_RSP_OK ) {

			CM_AR_TRACE_10(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "iocr(%08X) resolved mac(%02x-%02x-%02x-%02x-%02x-%02x) ip(%#08x) subnet_mask(%#08x) gateway_ip(%#08x)"
				, iocr
				, mac.mac[0], mac.mac[1], mac.mac[2]
				, mac.mac[3], mac.mac[4], mac.mac[5]
				, CM_NTOH32(ip), CM_NTOH32(subnet_mask), CM_NTOH32(gateway_ip)
				);

			/***/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("BEAUTIFY: passivate on offline")
#endif /* CM_MESSAGE */

if( cm_ar_com_is_down(iocr->cm.backlink) ) {

	iocr->cm.cr_state = CM_AR_CR_IDLE;
	cm_arsm_mcons_done(channel, iocr);
	break;
}

			/***/

			{
			CM_ACP_LOWER_RQB_PTR_TYPE acp;

			cm_acp_alloc_rqb (channel, & acp);

			if( is_null(acp) ) {
				CM_FATAL(); /* FATAL@acp-alloc: can make non-fatal? */
				break;
			}

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("warning: ARCQ, mcons auf ARCQ umstellen.")
#endif /* CM_MESSAGE */

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: acp-rqb-passing, abstract by acp_get__not_null()?")*/
#endif /* CM_MESSAGE */

			iocr->cm.cr_state = CM_AR_MCONS_ADD;

			iocr->cm.mc_addr = mac;
			cm_acp_frame_add(channel, iocr, acp);
			}

			/*
			 * NOTE: we do not wait for the response here!
			 */

			if( (iocr->cm.mc_flags & CM_CR_MC_TELL_MASK) != 0 ) { /* note: not only TELL_INFO */
				/* unexpected */
				CM_FATAL();
			}

			iocr->cm.mc_flags |= CM_CR_MC_TELL_INFO;

			cm_arsm_mcons_upper_indication(channel, iocr, nare_rsp, &mac, ip, subnet_mask, gateway_ip);
		}
		else { /* cnf(-) */

			CM_ASSERT(nare_rsp != NARE_RSP_OK_CANCEL); /* actually a bug */

			CM_AR_TRACE_09(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "iocr(%08X) nare failed, rsp(%u) mac(%02x-%02x-%02x-%02x-%02x-%02x) ip(%04x)"
				, iocr
				, nare_rsp
				, mac.mac[0], mac.mac[1], mac.mac[2]
				, mac.mac[3], mac.mac[4], mac.mac[5]
				, ip
				);

			/***/

			{
			CM_AR_GRAPH_MCR_PTR_TYPE  ar_mcr;
			LSA_UINT16 pause_msec;

			ar_mcr = cm_ar_graph_find_mcr(iocr->cm.backlink, iocr->iocr_ref);

			if( is_null(ar_mcr) ) {
				CM_FATAL(); /* should not happen, see consistency checks */
			}

			pause_msec = CM_CAST_U16(CM_MCR_PROP_GET_FACTOR_1SEC(ar_mcr->mcr_properties) * 1000/*msec*/);

			/***/

			CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "iocr(%08X), NARE.CNF(-), pausing for %u msec"
				, iocr
				, pause_msec
			);

			iocr->cm.cr_state = CM_AR_MCONS_NARE_PAUSE;

			cm_oneshot_trigger(&iocr->cm.mc_timer, pause_msec);
			}

			/***/

			if( (iocr->cm.mc_flags & CM_CR_MC_TELL_MASK) != 0 ) { /* note: not only TELL_INFO */
				/* unexpected */
				CM_FATAL();
			}

			iocr->cm.mc_flags |= CM_CR_MC_TELL_INFO;

			cm_arsm_mcons_upper_indication(channel, iocr, nare_rsp, &mac, ip, subnet_mask, gateway_ip);
		}
		break;

	case CM_AR_MCONS_NARE_CANCEL:
		CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X), NARE.CNF but was cancelled", iocr
			);
		cm_arsm_mcons_done(channel, iocr);
		break;

	default:
		CM_FATAL(); /* a bug */
		break;
	}

	cm_nare_free_rqb(channel, nare);
}

/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_is_resolved(
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_is_resolved")

	if( iocr->cm.mc_flags & CM_CR_MC_RESOLVED ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_resolve_cancel_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
) {
	CM_LOG_FCT("cm_arsm_mcons_resolve_cancel_done")

	CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "iocr(%08X) nare-cancelled, rsp(%u)"
		, iocr, CM_NARE_GET_RESPONSE(nare)
		);

	/***/

	if( (iocr->cm.mc_flags & CM_CR_MC_WF_CANCEL) == 0 ) {
		/* unexpected */
		CM_FATAL();
	}

	iocr->cm.mc_flags &= ~CM_CR_MC_WF_CANCEL;

	/***/

	switch( iocr->cm.cr_state ) {

	case CM_AR_MCONS_NARE_CANCEL:
		if( CM_NARE_GET_RESPONSE(nare) != NARE_RSP_OK ) {
			CM_NARE_LOWER_RQB_ERROR(nare); /* not as documented */
		}
		cm_arsm_mcons_done(channel, iocr);
		break;

	default:
		CM_FATAL();
		break;
	}

	cm_nare_free_rqb(channel, nare);
}

/*===========================================================================*/
/*=== MCONS-ADD DONE ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_add_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_arsm_mcons_add_done")

	CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X), rsp(%u)"
		, iocr, CM_ACP_GET_RESPONSE(acp)
		);

	CM_ASSERT(iocr->cm.cr_state == CM_AR_MCONS_ADD);

	/***/

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK ) {

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("THINK: mcons-add on startup, mcons-update-srcmac after resolve")
#endif /* CM_MESSAGE */

		CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "iocr(%08X) cannot add, rsp(%u) --> telling down and go to passive-state"
			, iocr, CM_ACP_GET_RESPONSE(acp)
			);

		cm_acp_free_rqb(channel, &acp);

		/***/

		iocr->cm.cr_state = CM_AR_CR_IDLE;

		cm_ar_com_down(iocr->cm.backlink, CM_AR_REASON_FRAME);

		cm_arsm_mcons_done(channel, iocr);
	}
	else {

		iocr->cm.cr_state = CM_AR_MCONS_REGISTER;

		cm_acp_multicast_mac(channel, iocr, &iocr->multicast_mac, ACP_MULTICAST_ENABLE, acp);
	}
}

/*===========================================================================*/
/*=== MMAC-REGISTER DONE ====================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_mmac_register_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_arsm_mcons_mmac_register_done")

	CM_ASSERT(is_not_null(iocr));

	CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) acp-rsp(%u)"
		, iocr, CM_ACP_GET_RESPONSE(acp)
		);

	CM_ASSERT(iocr->cm.cr_state == CM_AR_MCONS_REGISTER);

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK ) {

		CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X) mmac-register failed", iocr
			);

		cm_ar_com_down_debug_data_fmt (iocr->cm.backlink, CM_MODULE_ID, __LINE__, CM_AR_REASON_MMAC, iocr->frame_id);
		cm_ar_com_down(iocr->cm.backlink, CM_AR_REASON_MMAC);

		/***/

		iocr->cm.cr_state = CM_AR_CR_IDLE;

		cm_acp_frame_remove(channel, iocr, acp);
	}
	else { /* mmac-register succeeded */

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("BEAUTIFY: passivate on offline")
#endif /* CM_MESSAGE */

if( cm_ar_com_is_down(iocr->cm.backlink) ) {

	cm_acp_free_rqb(channel, &acp);

	iocr->cm.cr_state = CM_AR_MCONS_WF_UP;
	cm_arsm_mcons_done(channel, iocr);
	return;
}

		iocr->cm.cr_state = CM_AR_MCONS_ACT_RST;
		iocr->cm.is_rir = LSA_FALSE;

		cm_acp_frame_control(channel, iocr, (ACP_CONSUMER_SET_ACTIVE | ACP_CONSUMER_SET_TO_UNKNOWN_STATE), acp);
	}
}

/*===========================================================================*/
/*=== MCONS-CONTROL DONE ====================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_control_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_arsm_mcons_control_done")

	CM_ASSERT(is_not_null(iocr));

	/***/

	CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) acp_id(%u) reset done, waiting for up"
		, iocr, iocr->cm.acp_id
		);

	/***/

	CM_ASSERT(CM_ACP_GET_RESPONSE(acp) == ACP_OK); /* see caller */

	cm_acp_free_rqb(channel, &acp);

	/***/

	CM_ASSERT(iocr->cm.cr_state == CM_AR_MCONS_ACT_RST);

	iocr->cm.cr_state = CM_AR_MCONS_WF_UP;

	/***/

	cm_arsm_mcons_done(channel, iocr);
}

/*===========================================================================*/
/*=== MCONS UP ==============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_up(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_up")

	CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) cr_state(%u)"
		, iocr, iocr->cm.cr_state
		);

	switch( iocr->cm.cr_state ) {

	case CM_AR_MCONS_WF_UP:

		CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X) UP", iocr
			);

		iocr->cm.cr_state = CM_AR_MCONS_UP;

		/***/

		if( (iocr->cm.mc_flags & CM_CR_MC_TELL_UP) != 0 ) {
			/* unexpected */
			CM_FATAL();
		}

		iocr->cm.mc_flags |= CM_CR_MC_TELL_UP;

		cm_arsm_mcons_upper_indication(channel, iocr, 0, LSA_NULL, 0, 0, 0);
		break;

	default:
/*?fatal if "up"?*/
		CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X) cr_state(%u) not WF_UP, already stopping?"
			, iocr, iocr->cm.cr_state
			);
		break;
	}
}

/*===========================================================================*/
/*=== MCONS IS-UP ===========================================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_is_up(
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr
) {
	CM_LOG_FCT("cm_arsm_mcons_is_up")

	CM_ASSERT(io_cr->cm.is_consumer && io_cr->cm.is_multicast);

	if( io_cr->cm.cr_state == CM_AR_MCONS_UP ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== MCONS DOWN ============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_down(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_down")

	CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) cr_state(%u)"
		, iocr, iocr->cm.cr_state
		);

	switch( iocr->cm.cr_state ) {

	case CM_AR_MCONS_WF_UP:
	case CM_AR_MCONS_UP:

		CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X) DOWN", iocr
			);

		iocr->cm.cr_state = CM_AR_MCONS_DOWN;

		/***/

		if( (iocr->cm.mc_flags & CM_CR_MC_TELL_DOWN) != 0 ) {
			/* unexpected */
			CM_FATAL();
		}

		iocr->cm.mc_flags |= CM_CR_MC_TELL_DOWN;

		cm_arsm_mcons_upper_indication(channel, iocr, 0, LSA_NULL, 0, 0, 0);
		break;

	default:
		CM_AR_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X) cr_state(%u) not WF_UP or UP, already stopping?"
			, iocr, iocr->cm.cr_state
			);
		break;
	}
}

/*===========================================================================*/
/*=== MCONS APDUSTATUS-CHANGED ==============================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_apdu_status_changed(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_apdu_status_changed")

	CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X)", iocr
		);

	cm_arsm_mcons_upper_indication(channel, iocr, 0, LSA_NULL, 0, 0, 0);
}

/*===========================================================================*/
/*=== MMAC-UNREGISTER DONE ==================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_mmac_unregister_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT ("cm_arsm_mcons_mmac_unregister_done")

	CM_ASSERT(is_not_null(iocr));

	CM_ASSERT(CM_ACP_GET_RESPONSE(acp) == ACP_OK); /* see caller */

	CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X)", iocr
		);

	/***/

	CM_ASSERT(iocr->cm.cr_state == CM_AR_MCONS_UNREGISTER);

	iocr->cm.cr_state = CM_AR_MCONS_REMOVE;

	/* v5.0: must passivate first */
	cm_acp_frame_control (channel, iocr, ACP_CONSUMER_SET_PASSIVE, acp);
}

/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_passivate_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_arsm_mcons_passivate_done")

	CM_ASSERT(is_not_null(iocr));
	CM_ASSERT(CM_ACP_GET_RESPONSE(acp) == ACP_OK); /* see caller */

	/***/

	CM_ASSERT(iocr->cm.cr_state == CM_AR_MCONS_REMOVE);

	cm_acp_frame_remove(channel, iocr, acp);
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_remove_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_arsm_mcons_remove_done")

	CM_ASSERT(is_not_null(iocr));

	/***/

	CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X)", iocr
		);

	/***/

	CM_ASSERT(CM_ACP_GET_RESPONSE(acp) == ACP_OK); /* see caller */

	cm_acp_free_rqb(channel, &acp);

	/***/

	iocr->cm.mc_flags &= ~CM_CR_MC_TELL_MASK;

	/***/

	if( iocr->cm.do_stop ) {

		iocr->cm.cr_state = CM_AR_CR_IDLE;

		cm_arsm_mcons_done(channel, iocr);
	}
	else {

		iocr->cm.cr_state = CM_AR_MCONS_WF_LINK_UP;

		if( cm_link_status_is_up(channel) ) {

			cm_arsm_mcons_link_up(channel, iocr);
		}
	}
}

/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_stop(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_stop")

	CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) cr_state(%u) mc_flags(0x%x)"
		, iocr, iocr->cm.cr_state, iocr->cm.mc_flags
		);

	/***/

	if( iocr->cm.do_stop ) {
		CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X) stop called twice", iocr
			);
		CM_FATAL(); /* a bug */
	}

	iocr->cm.do_stop = LSA_TRUE;

	/***/

	if( iocr->cm.cr_state == CM_AR_MCONS_NARE ) {

		CM_AR_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "resolve pending, cancel"
			);

		CM_ASSERT((iocr->cm.mc_flags & CM_CR_MC_WF_RESOLVE) != 0);

		if( cm_nare_resolve_cancel(channel, iocr->cm.backlink->device_nr, iocr->cm.backlink->ar_nr, iocr) ) {

			CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "iocr(%08X) cancelling nare-request", iocr
				);

			CM_ASSERT((iocr->cm.mc_flags & CM_CR_MC_WF_CANCEL) == 0);

			iocr->cm.mc_flags |= CM_CR_MC_WF_CANCEL;

			iocr->cm.cr_state = CM_AR_MCONS_NARE_CANCEL;
		}
		else {

			CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "iocr(%08X) cannot cm_nare_resolve_cancel(), must wait for nare-completion", iocr
				);
		}
	}
	else {

		CM_ASSERT(iocr->cm.cr_state != CM_AR_MCONS_NARE_CANCEL);

		cm_arsm_mcons_done(channel, iocr);
	}
}

/*===========================================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
) {
	CM_LOG_FCT("cm_arsm_mcons_done")

	CM_ASSERT(iocr->cm.is_multicast);

	CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) cr_state(%u) mc_flags(0x%x)"
		, iocr, iocr->cm.cr_state, iocr->cm.mc_flags
		);

	/***/

	if( (iocr->cm.mc_flags & CM_CR_MC_WF_MASK) != 0 ) {
		CM_AR_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "iocr(%08X) waiting-for-something, cr_state(%u) mc_flags(0x%x)"
			, iocr, iocr->cm.cr_state, iocr->cm.mc_flags
			);
		return;
	}

	if( iocr->cm.do_stop ) {

		switch( iocr->cm.cr_state ) {

		case CM_AR_CR_IDLE:
			break;

		case CM_AR_MCONS_WF_LINK_UP:
			iocr->cm.cr_state = CM_AR_CR_IDLE;
			break;

		case CM_AR_MCONS_NARE_CANCEL:
			iocr->cm.cr_state = CM_AR_CR_IDLE;
			break;

		case CM_AR_MCONS_NARE_PAUSE:
			cm_oneshot_stop(&iocr->cm.mc_timer);
			iocr->cm.cr_state = CM_AR_CR_IDLE;
			break;

		case CM_AR_MCONS_WF_UP:
		case CM_AR_MCONS_UP:
			iocr->cm.cr_state = CM_AR_MCONS_DOWN; /* see below */
			break;

		case CM_AR_MCONS_DOWN:
			/* see below */
			break;

		default:
			/* must wait */
			return;
		}

		if( iocr->cm.cr_state == CM_AR_CR_IDLE ) {

			CM_ASSERT(iocr->cm.do_stop);

			iocr->cm.do_stop = LSA_FALSE;

			iocr->cm.mc_flags = 0;

			/***/

			cm_ar_com_mcr_stop_done(iocr->cm.backlink, iocr);
			return;
		}
	}

	if( iocr->cm.cr_state == CM_AR_MCONS_DOWN ) {

		CM_ACP_LOWER_RQB_PTR_TYPE acp;

		cm_acp_alloc_rqb (channel, & acp);

		if( is_null(acp) ) {
			CM_FATAL(); /* FATAL@acp-alloc: can make non-fatal? */
			return;
		}

		/***/

		iocr->cm.cr_state = CM_AR_MCONS_UNREGISTER;

		cm_acp_multicast_mac(channel, iocr, &iocr->multicast_mac, ACP_MULTICAST_DISABLE, acp);
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE("***** not compiling m-consumer (CM_CFG_USE_NARE=" CM_STRINGIFY(CM_CFG_USE_NARE) ")")
#  endif /* CM_MESSAGE */
#endif

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
