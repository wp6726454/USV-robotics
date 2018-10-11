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
/*  F i l e               &F: cm_usr.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the LSA user-interface                                        */
/*                                                                           */
/*  - helper for callback                                                    */
/*  - channel open/close done callback                                       */
/*  - open channel                                                           */
/*  - close channel                                                          */
/*  - request                                                                */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	2
#define CM_MODULE_ID		2

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#ifdef CM_MESSAGE
#pragma CM_MESSAGE("DEFERRED: pragmas with VVs are now under comment. search for VV: to find them.")
/* remove this pragma when the last VV has been realized */
#endif /* CM_MESSAGE */

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("DEFERRED: pragmas with FATAL@xxx converted into comments. search for FATAL@ to find them.")
/* remove this pragma when the last fatal is avoided */
#endif /* CM_MESSAGE */

/*---------------------------------------------------------------------------*/

CM_CHANNEL_TYPE cm_channels[CM_MAX_CHANNELS]; /* the channel table */

/*---------------------------------------------------------------------------*/

static LSA_VOID CM_UPPER_IN_FCT_ATTR
cm_request_pd (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
); /* forward */


/*---------------------------------------------------------------------------*/


LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channels_sys_init(
	LSA_VOID
) {
	CM_LOG_FCT("cm_channels_sys_init")
	LSA_UINT16 i;

		/* init the channel table */
	for (i = 0; i < CM_MAX_CHANNELS; ++i) {
		cm_channels[i].state = CM_CH_FREE;
	}
}


/*---------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channels_sys_undo_init(
	LSA_VOID
) {
	CM_LOG_FCT("cm_channels_sys_undo_init")
	LSA_UINT16 i;

	for (i = 0; i < CM_MAX_CHANNELS; ++i) {
		CM_ASSERT(cm_channels[i].state == CM_CH_FREE);
	}
}


/*------------------------------------------------------------------------------
//	sets the response of the given request-block and calls the
//        request-done-fct (given in CM_OPC_OPEN_CHANNEL)
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_callback(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 response,
	CM_UPPER_RQB_PTR_TYPE CM_LOCAL_MEM_ATTR *upper_ptr
) {
	CM_LOG_FCT ("cm_callback")
	CM_UPPER_RQB_PTR_TYPE  rb;

	rb = *upper_ptr;
	*upper_ptr = LSA_NULL;

	CM_RQB_SET_HANDLE (rb, channel->handle_upper);
	CM_RQB_SET_RESPONSE (rb, response);

	CM_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< cm-rqb(0x%08x) h(%u) cm-opc(%u) cm-rsp(%u) f-h(%u)"
		, rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb), CM_RQB_GET_RESPONSE (rb), channel->my_handle
		);

	CM_REQUEST_UPPER_DONE (channel->callback, rb, channel->sysptr);
}


/*------------------------------------------------------------------------------
//	func: cm_callback_and_free_channel
//----------------------------------------------------------------------------*/

static LSA_VOID cm_callback_and_free_channel(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 result,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT ("cm_callback_and_free_channel")

	/***/

#if CM_CFG_USE_ACP
	cm_overload_undo_init(channel);
#endif

	cm_res_undo_init(channel);

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: make cleanup of lower table")*/
#endif /* CM_MESSAGE */

#if 0 /* channels are not all zero here! */
	{
	LSA_UINT16 t;

	for( t = 0; t < CM_PATH_TYPE_ANY_MAX; ++t ) {

		CM_ASSERT(channel->lower.channel[t] == LSA_NULL);
	}
	}
#endif

	/***/

	CM_RQB_SET_HANDLE (rb, channel->handle_upper);
	CM_RQB_SET_RESPONSE (rb, result);

	CM_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< cm-rqb(0x%08x) h(%u) cm-opc(%u) cm-rsp(%u) channel-free"
		, rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb), CM_RQB_GET_RESPONSE (rb)
		);

	/*
	 * NOTE: the following sequence is wrong!
	 *       > cm_callback(channel, CM_OK, &rb);
	 *       > channel->state = CM_CH_FREE;
	 *
	 * ==> even if the callback posts into a queue, a call
	 *     to cm_undo_init() can occur (due to preemption)
	 *     before we have the chance to reset the in-use flag
	 */

	{
	CM_UPPER_CALLBACK_FCT_PTR_TYPE  callback = channel->callback;
	CM_DETAIL_PTR_TYPE  detailptr = channel->detailptr;
	LSA_SYS_PTR_TYPE  sysptr = channel->sysptr;
	LSA_UINT16  rc;

#if CM_DEBUG
	CM_MEMSET(channel, 0xDD, sizeof(*channel)); /* wipe out */
#endif

	channel->callback = 0;
	channel->detailptr = LSA_NULL;
	channel->state = CM_CH_FREE; /* free the channel ... */

	CM_REQUEST_UPPER_DONE (callback, rb, sysptr); /* ... and then call back! */

	CM_RELEASE_PATH_INFO (& rc, sysptr, detailptr);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
	}
}


/*------------------------------------------------------------------------------
//	allocate a channel
//----------------------------------------------------------------------------*/

static CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_channel_alloc(
	CM_UPPER_OPEN_CHANNEL_PTR_TYPE open,
	LSA_SYS_PTR_TYPE sysptr,
	CM_DETAIL_PTR_TYPE detailptr
) {
	CM_LOG_FCT ("cm_channel_alloc")
	LSA_HANDLE_TYPE handle;

	CM_ASSERT(CM_MAX_CHANNELS < CM_INVALID_HANDLE); /* not #if because of type-cast! */

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE channel = &cm_channels[handle];

		if( channel->state == CM_CH_FREE ) {

#if CM_DEBUG
			CM_MEMSET(channel, 0xCC, sizeof(*channel)); /* to find uninitialized things */
#endif
			channel->state			= CM_CH_ALLOCATED;
			channel->my_handle		= handle;
			channel->handle_upper	= open->handle_upper;
			channel->callback		= open->cm_request_upper_done_ptr;
			channel->sysptr			= sysptr;
			channel->detailptr		= detailptr;
			channel->trace_idx		= 0; /* sanity, will be set later */

			/* the rest is set up in cm_open_channel() */

			return channel;
		}
	}

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	get channel from handle or LSA_NULL if out-of-range
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_channel_from_handle(
	LSA_HANDLE_TYPE handle
) {

	if( handle < CM_MAX_CHANNELS ) {

		CM_CHANNEL_PTR_TYPE channel = &cm_channels[handle]; /* see cm_channel_alloc() */

		if( channel->state >= CM_CH_OPEN ) {

			return channel; /* note: valid detailptr and callback-function (AP01361477) */
		}
	}

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	find a channel for this nic with path_type (and sync_id)
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_channel_find_path_type(
	LSA_UINT8  nic_id,
	LSA_UINT8  path_type
) {
	CM_LOG_FCT ("cm_channel_find_path_type")
	LSA_HANDLE_TYPE handle;

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	channel = cm_channel_from_handle(handle);

		if(
			is_not_null(channel)
			&& channel->state >= CM_CH_OPEN /* always true */
			&& is_not_null(channel->detailptr) /* always true */
		) { /* is open */

			if( channel->detailptr->nic_id == nic_id ) { /* same nic */

				if( channel->detailptr->path_type == path_type ) {

					return channel; /* found */
				}
			}
		}
	}

	return LSA_NULL; /* not found */
}

/*------------------------------------------------------------------------------
//	find a GSY channel for this nic
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_channel_find_gsy(
	LSA_UINT8  nic_id,
	LSA_UINT8  sync_id
) {
	CM_LOG_FCT ("cm_channel_find_gsy")
	LSA_HANDLE_TYPE handle;

	if (sync_id == GSY_SYNCID_TIME)
	{
		return LSA_NULL; /* GSY_SYNCID_TIME will be removed with AP01504849 */
	}

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	channel = cm_channel_from_handle(handle);

		if(
			is_not_null(channel)
			&& channel->state >= CM_CH_READY /* after "get info" */
			&& is_not_null(channel->detailptr) /* always true */
		) { /* is open */

			if( channel->detailptr->nic_id == nic_id ) { /* same nic */

				if( channel->detailptr->path_type == CM_PATH_TYPE_GSY ) {

					if( channel->lower.info.gsy.SyncId == sync_id ) {

						return channel; /* found */
					}
				}
			}
		}
	}

	return LSA_NULL; /* not found */
}

#endif

/*------------------------------------------------------------------------------
//	check if all system channels are opened
//----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_channel_sys_opened(
	LSA_UINT8   nic_id,
	LSA_UINT8   usr_path_type
) {
	CM_LOG_FCT ("cm_channel_sys_opened")
	LSA_UINT32 sys = 0;
	LSA_HANDLE_TYPE handle;

	CM_ASSERT(CM_PATH_TYPE_ANY_MAX < 32); /* see shift-operator */

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	channel = cm_channel_from_handle(handle);

		if(
			is_not_null(channel)
			&& channel->state >= CM_CH_READY
			&& channel->state < CM_CH_CLOSE
			&& is_not_null(channel->detailptr) /* always true */
		) { /* is open */

			if( channel->detailptr->nic_id == nic_id ) { /* same nic */

				switch( channel->detailptr->path_type ) {

				case CM_PATH_TYPE_EDD:
#if CM_CFG_USE_MRP
				case CM_PATH_TYPE_MRP:
#endif
#if CM_CFG_USE_POF
				case CM_PATH_TYPE_POF:
#endif
				case CM_PATH_TYPE_OHA:
#if CM_CFG_USE_NARE
				case CM_PATH_TYPE_NARE:
#endif
				case CM_PATH_TYPE_RPC:
					sys |= (1UL << channel->detailptr->path_type);
					break;

#if CM_CFG_USE_GSY
				case CM_PATH_TYPE_GSY: /* note: more than 1 GSY possible */
					if( channel->lower.info.gsy.SyncId == GSY_SYNCID_CLOCK ) {
						sys |= (1UL << channel->detailptr->path_type);
					}
					break;
#endif

				default:
					break;
				}
			}
		}
	}

	/***/

	{
	LSA_UINT32 bits = 0;

	bits |= (1UL << CM_PATH_TYPE_EDD);
#if CM_CFG_USE_MRP
	bits |= (1UL << CM_PATH_TYPE_MRP);
#endif
#if CM_CFG_USE_GSY
	bits |= (1UL << CM_PATH_TYPE_GSY);
#endif
#if CM_CFG_USE_POF
	bits |= (1UL << CM_PATH_TYPE_POF);
#endif
	bits |= (1UL << CM_PATH_TYPE_OHA);

	switch( usr_path_type ) {
	case CM_PATH_TYPE_PD:
		bits |= (1UL << CM_PATH_TYPE_RPC); /* cm-pd starts the endpoint-mapper */
		break;

	case CM_PATH_TYPE_CLIENT:
#if CM_CFG_USE_NARE
		bits |= (1UL << CM_PATH_TYPE_NARE);
#endif
		bits |= (1UL << CM_PATH_TYPE_RPC);
		break;

	case CM_PATH_TYPE_MULTICAST:
#if CM_CFG_USE_NARE
		bits |= (1UL << CM_PATH_TYPE_NARE);
#endif
		break;

	case CM_PATH_TYPE_SERVER:
#if CM_CFG_USE_NARE
		bits |= (1UL << CM_PATH_TYPE_NARE);
#endif
		bits |= (1UL << CM_PATH_TYPE_RPC);
		break;

	default:
		CM_FATAL();
		break;
	}

	if( (sys & bits) != bits ) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
			, "not all system channels open, open(0x%x) needed(0x%x)"
			, sys, bits
			);
		return LSA_FALSE;
	}
	}

	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
//	assign the system channels
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channel_sys_assign(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT ("cm_channel_sys_assign")
	LSA_HANDLE_TYPE handle;

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	sys_channel = cm_channel_from_handle(handle);

		if(
			is_not_null(sys_channel)
			&& sys_channel->state >= CM_CH_READY
			&& sys_channel->state < CM_CH_CLOSE
			&& is_not_null(sys_channel->detailptr) /* always true */
		) { /* is open */

			if( sys_channel->detailptr->nic_id == channel->detailptr->nic_id ) { /* same nic */

				switch( sys_channel->detailptr->path_type ) {

				case CM_PATH_TYPE_EDD:
#if CM_CFG_USE_MRP
				case CM_PATH_TYPE_MRP:
#endif
#if CM_CFG_USE_POF
				case CM_PATH_TYPE_POF:
#endif
				case CM_PATH_TYPE_OHA:
#if CM_CFG_USE_NARE
				case CM_PATH_TYPE_NARE:
#endif
				case CM_PATH_TYPE_RPC:
					channel->lower.channel[sys_channel->detailptr->path_type] = sys_channel;
					break;

#if CM_CFG_USE_GSY
				case CM_PATH_TYPE_GSY: /* note: comes here for each sync-id, but always assigns "clock" */
					channel->lower.channel[sys_channel->detailptr->path_type] = cm_channel_find_gsy(channel->detailptr->nic_id, GSY_SYNCID_CLOCK);
					break;
#endif

				default:
					break;
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
//	connect the CL/MC/SV channel with the PD channel
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_ACP

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_peer_connect(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT ("cm_peer_connect")
	CM_CHANNEL_PTR_TYPE pd = cm_channel_find_path_type(channel->detailptr->nic_id, CM_PATH_TYPE_PD);

	if( is_null(pd) ) {
		CM_FATAL(); /* a bug */
	}

	CM_ASSERT(is_null(channel->lower.channel[channel->detailptr->path_type]));
	CM_ASSERT(is_null(pd->lower.channel[channel->detailptr->path_type]));

	channel->lower.channel[channel->detailptr->path_type] = pd;

	pd->lower.channel[channel->detailptr->path_type] = channel;

	CM_UPPER_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "usr(%u) path_type(%u) connected to pd(%u) path_type(%u)"
		, channel->my_handle, channel->detailptr->path_type, pd->my_handle, pd->detailptr->path_type
		);
}

#endif

/*------------------------------------------------------------------------------
//	disconnect the CL/MC/SV channel from the PD channel
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_ACP

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_peer_disconnect(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_LOG_FCT ("cm_peer_disconnect")
	CM_CHANNEL_PTR_TYPE pd;

	pd = channel->lower.channel[channel->detailptr->path_type];

	if( is_null(pd) ) {
		CM_FATAL(); /* a bug */
	}

	pd->lower.channel[channel->detailptr->path_type] = LSA_NULL; /* disconnect */

	channel->lower.channel[channel->detailptr->path_type] = LSA_NULL; /* sanity */

	CM_UPPER_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "usr(%u) path_type(%u) disconnected from pd(%u) path_type(%u)"
		, channel->my_handle, channel->detailptr->path_type, pd->my_handle, pd->detailptr->path_type
		);
}

#endif

/*------------------------------------------------------------------------------
//	get peer of the CL/MC/SV or PD channel
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_peer_channel_get(
	CM_CHANNEL_PTR_TYPE channel, /* PD, CL, MC or SV */
	LSA_UINT8 peer_path_type
		/* PD: CM_PATH_TYPE_CLIENT, CM_PATH_TYPE_MULTICAST or CM_PATH_TYPE_SERVER
		 * CL: CM_PATH_TYPE_PD, CM_PATH_TYPE_CLIENT
		 * MC: CM_PATH_TYPE_PD
		 * SV: CM_PATH_TYPE_PD
		 */
) {
	CM_LOG_FCT("cm_peer_channel_get")
	CM_CHANNEL_PTR_TYPE pd;

	switch( peer_path_type ) {

	case CM_PATH_TYPE_PD:
		if (channel->detailptr->path_type == CM_PATH_TYPE_PD) {

			pd = channel;
		}
		else {

			CM_ASSERT(
				channel->detailptr->path_type == CM_PATH_TYPE_CLIENT
				|| channel->detailptr->path_type == CM_PATH_TYPE_MULTICAST
				|| channel->detailptr->path_type == CM_PATH_TYPE_SERVER
			);

			pd = channel->lower.channel[channel->detailptr->path_type];

			if( is_null(pd) ) {
				CM_FATAL(); /* a bug */
			}
		}
		break;

	case CM_PATH_TYPE_CLIENT:
		if (channel->detailptr->path_type == CM_PATH_TYPE_CLIENT)
		{
			pd = channel;
		}
		else
		{
			CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_PD);
			pd = channel->lower.channel[peer_path_type]; /* may be null */
		}
		break;

	case CM_PATH_TYPE_MULTICAST:
	case CM_PATH_TYPE_SERVER:
		CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_PD);
		pd = channel->lower.channel[peer_path_type]; /* may be null */
		break;

	default:
		CM_FATAL(); /* a bug */
		pd = LSA_NULL;
		break;
	}

	return pd;
}


/*------------------------------------------------------------------------------
//	check configuration
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channel_check(
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_BOOL is_channel_to_acp
) {
	CM_LOG_FCT ("cm_channel_check")
	LSA_UINT32 interface_id;
	LSA_UINT16 port_cnt;

	/*
	 * check if configuration is ok (assume EDD is "right")
	 */

	{
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(usr_channel);

	interface_id = edd_channel->lower.info.edd.InterfaceID;
	port_cnt     = edd_channel->lower.info.edd.MaxPortCnt;
	}

	if( ! (
		(interface_id >= 1 && interface_id <= 16)
		&&
		(port_cnt >= 1 && port_cnt <= 255)
	) ) {

		CM_UPPER_TRACE_00(usr_channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "misconfiguration: EDD"
			);

		CM_FATAL(); /* misconfiguration */
	}

	/***/

	if( is_channel_to_acp ) {

		if(
			interface_id != usr_channel->lower.info.acp.eth_interface_id
			|| port_cnt != usr_channel->lower.info.acp.eth_ports_max
		) {

			CM_UPPER_TRACE_00(usr_channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "misconfiguration: ACP != EDD"
				);

			CM_FATAL(); /* misconfiguration */
		}
	}

	/***/

#if CM_CFG_USE_GSY
	CM_ASSERT(is_not_null(cm_gsy_channel_get(usr_channel))); /* no interface / port info */
#endif

	/***/

	{
	CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get(usr_channel);

	if(
		interface_id != oha_channel->lower.info.oha.InterfaceID
		|| port_cnt != oha_channel->lower.info.oha.PortCnt
	) {

		CM_UPPER_TRACE_00(usr_channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "misconfiguration: OHA != EDD"
			);

		CM_FATAL(); /* misconfiguration */
	}
	}

	/***/

	/* with N-IF: RPC has no meaningfull interface-id anymore */
}


/*------------------------------------------------------------------------------
//	check if all user channels are closed
//----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_channel_usr_closed(
	LSA_UINT8   nic_id,
	LSA_UINT8   sys_path_type
) {
	CM_LOG_FCT ("cm_channel_usr_closed")
	LSA_UINT16 cnt = 0;
	LSA_HANDLE_TYPE handle;

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	channel = cm_channel_from_handle(handle);

		if(
			is_not_null(channel)
			&& channel->state != CM_CH_FREE /* i.e., detailptr valid */
			&& is_not_null(channel->detailptr) /* always true */
		) { /* is used */

			if( channel->detailptr->nic_id == nic_id ) { /* same nic */

				switch( sys_path_type ) {
				case CM_PATH_TYPE_RPC:
#if CM_CFG_USE_NARE
				case CM_PATH_TYPE_NARE:
#endif
					switch( channel->detailptr->path_type ) {
					case CM_PATH_TYPE_CLIENT:
					case CM_PATH_TYPE_MULTICAST:
					case CM_PATH_TYPE_SERVER:
						cnt += 1;
						break;
					default:
						break;
					}
					break;

				case CM_PATH_TYPE_OHA:
				case CM_PATH_TYPE_EDD:
#if CM_CFG_USE_MRP
				case CM_PATH_TYPE_MRP:
#endif
#if CM_CFG_USE_GSY
				case CM_PATH_TYPE_GSY:
#endif
#if CM_CFG_USE_POF
				case CM_PATH_TYPE_POF:
#endif
					switch( channel->detailptr->path_type ) {
					case CM_PATH_TYPE_PD:
					case CM_PATH_TYPE_CLIENT:
					case CM_PATH_TYPE_MULTICAST:
					case CM_PATH_TYPE_SERVER:
						cnt += 1;
						break;
					default:
						break;
					}
					break;

				default:
					break;
				}
			}
		}
	}

	if( cnt != 0 ) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE
			, "not all user channels closed, %u still open, sys_path_type(%u)"
			, cnt, sys_path_type
			);
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*------------------------------------------------------------------------------
//	callback for client & server to notify completion of open/close channel
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channel_done (
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT32			response
) {
	CM_LOG_FCT ("cm_channel_done")

	if (response != CM_OK) {
		CM_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) state(%u) rsp(%u)"
			, channel->my_handle, channel->state, response
			);
	}
	else {
		CM_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) state(%u) rsp(%u)"
			, channel->my_handle, channel->state, response
			);
	}

	switch (channel->state) {

/*------------------------------------------------------------------------------
// OPEN
//----------------------------------------------------------------------------*/
	case CM_CH_OPEN:
		if (response != CM_OK) {
			/* transition CM-CH-OPEN --> CM-CH-NOI */
			cm_callback_and_free_channel (channel, CM_ERR_LOWER_LAYER, channel->usr.ch.curr);
		}
		else {
			/* transition CM-CH-OPEN --> CM-CH-INFO */
			channel->state = CM_CH_INFO;

			switch( channel->detailptr->path_type ) {

			case CM_PATH_TYPE_EDD:
				cm_edd_channel_info(channel);
				break;

#if CM_CFG_USE_MRP
			case CM_PATH_TYPE_MRP:
				cm_mrp_channel_info(channel);
				break;
#endif

#if CM_CFG_USE_GSY
			case CM_PATH_TYPE_GSY:
				cm_gsy_channel_info(channel);
				break;
#endif

#if CM_CFG_USE_NARE
			case CM_PATH_TYPE_NARE:
				cm_nare_channel_info(channel);
				break;
#endif

#if CM_CFG_USE_POF
			case CM_PATH_TYPE_POF:
				cm_pof_channel_info(channel);
				break;
#endif

			case CM_PATH_TYPE_OHA:
				cm_oha_channel_info(channel);
				break;

			case CM_PATH_TYPE_RPC:
				cm_rpc_set_interface_param(channel);
				break;

			case CM_PATH_TYPE_PD:
#ifdef CM_MESSAGE
/*???recursion!*/
#endif /* CM_MESSAGE */
				cm_channel_done(channel, CM_OK);
				break;

#if CM_CFG_USE_ACP
			case CM_PATH_TYPE_CLIENT:
			case CM_PATH_TYPE_MULTICAST:
			case CM_PATH_TYPE_SERVER:
				cm_acp_channel_info(channel);
				break;
#endif

			default:
				CM_FATAL(); /* a bug */
				break;
			}
		}
		return;

	case CM_CH_INFO:
		if (response != CM_OK) {
			CM_FATAL(); /* fatal, open succeeded */
		}
		else {
			/* transition CM-CH-INFO --> CM-CH-READY */
			channel->state = CM_CH_READY;
			CM_ASSERT (is_not_null(channel->usr.ch.curr));
			CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.ch.curr) == CM_OPC_OPEN_CHANNEL);
			channel->usr.ch.curr->args.channel.open->handle = channel->my_handle; /* set the result */
			cm_callback (channel, CM_OK, & channel->usr.ch.curr);

			/* after channel->usr.ch.curr is no longer used, we call pd/cl/mc-init */

#if CM_DEBUG
			CM_MEMSET(&channel->usr, 0xCC, sizeof(channel->usr)); /* wipe-out */
#endif

			switch( channel->detailptr->path_type ) {

			case CM_PATH_TYPE_EDD:
				cm_edd_channel_init(channel);
				break;

#if CM_CFG_USE_MRP
			case CM_PATH_TYPE_MRP:
				cm_mrp_channel_init(channel);
				break;
#endif

#if CM_CFG_USE_GSY
			case CM_PATH_TYPE_GSY:
				cm_gsy_channel_init(channel);
				break;
#endif

#if CM_CFG_USE_NARE
			case CM_PATH_TYPE_NARE:
				/* nothing */
				break;
#endif

#if CM_CFG_USE_POF
			case CM_PATH_TYPE_POF:
				cm_pof_channel_init(channel);
				break;
#endif

			case CM_PATH_TYPE_OHA:
				cm_oha_channel_init(channel);
				break;

			case CM_PATH_TYPE_RPC:
				/* nothing */
				break;

			case CM_PATH_TYPE_PD:
				cm_channel_check(channel, LSA_FALSE);
				cm_pd_channel_init(channel);
				break;

#if CM_CFG_MAX_CLIENTS
			case CM_PATH_TYPE_CLIENT:
				cm_peer_connect(channel);
				cm_channel_check(channel, LSA_TRUE);
				cm_cl_channel_init(channel);
				break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
			case CM_PATH_TYPE_MULTICAST:
				cm_peer_connect(channel);
				cm_channel_check(channel, LSA_TRUE);
				cm_mc_channel_init(channel);
				break;
#endif

#if CM_CFG_MAX_SERVERS
			case CM_PATH_TYPE_SERVER:
				cm_peer_connect(channel);
				cm_channel_check(channel, LSA_TRUE);
				/* note: channel->usr.sv.* is initialized in sv-create */
				break;
#endif

			default:
				CM_FATAL(); /* a bug */
				break;
			}
		}
		return;

/*------------------------------------------------------------------------------
// CLOSE
//----------------------------------------------------------------------------*/

	case CM_CH_CLOSE:
		if (response != CM_OK) {
			CM_FATAL(); /* a bug */
		}
		else {
			/* transition CM-CH-CLOSE --> CM-CH-NOI */
			CM_ASSERT (is_not_null(channel->usr.ch.curr));
			CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.ch.curr) == CM_OPC_CLOSE_CHANNEL);

			/*
			 * now it is ok to cancel all indication resources
			 */

#if CM_CFG_USE_ACP
			cm_overload_cancel(channel, CM_OK_CANCELLED);
#endif

			cm_res_cancel(channel);

			cm_callback_and_free_channel (channel, CM_OK, channel->usr.ch.curr);
		}
		return;

/*------------------------------------------------------------------------------
// DEFAULT
//----------------------------------------------------------------------------*/
	default:
		CM_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "h(%u) unknown state(%u)"
			, channel->my_handle, channel->state
			);
		CM_FATAL();
		return;
	}
}


/*------------------------------------------------------------------------------
//	create a CM instance and open a channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID CM_UPPER_IN_FCT_ATTR
cm_open_channel (
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT ("cm_open_channel")
	CM_UPPER_OPEN_CHANNEL_PTR_TYPE open;
	CM_CHANNEL_PTR_TYPE channel;

		/* sanity checks */

	if (
		is_null (rb)
		|| CM_RQB_GET_OPCODE (rb) != CM_OPC_OPEN_CHANNEL
		|| is_null (rb->args.void_ptr)
	) {
		CM_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
			, "no rqb(%08X) or wrong opcode or no args"
			, rb
			);
		CM_UPPER_RQB_ERROR (rb);
		return;
	}

#if CM_DEBUG
	CM_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CM_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	open = rb->args.channel.open; /* shortcut */

	open->handle = CM_INVALID_HANDLE; /* sanity */

	if (open->cm_request_upper_done_ptr == 0) {/* note: must not use is_null(), 
											   compiler warning 揻unction/data pointer conversion in expression?(AP01382171 ) */
		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL
			, "no callback function given"
			);
		CM_UPPER_RQB_ERROR (rb);
		return;
	}

		/* get path info and allocate a cm-handle */

	{
	CM_DETAIL_PTR_TYPE detailptr = LSA_NULL;
	LSA_SYS_PTR_TYPE sysptr;
	LSA_UINT16 rc;

	CM_GET_PATH_INFO (&rc, &sysptr, &detailptr, open->sys_path);

	if (rc != LSA_RET_OK || is_null (detailptr)) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
			, "cm_get_path_info: rc(%u) detailptr(%08x)", rc, detailptr
			);
		CM_UPPER_RQB_ERROR (rb); /* cannot call back, see CM_REQUEST_UPPER_DONE() */
		return;
	}

	channel = cm_channel_alloc(open, sysptr, detailptr);

	if( is_null(channel) ) {

		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "no free channel found"
			);

		CM_RQB_SET_RESPONSE (rb, CM_ERR_RESOURCE);
		CM_RQB_SET_HANDLE (rb, open->handle_upper);
		CM_REQUEST_UPPER_DONE (open->cm_request_upper_done_ptr, rb, sysptr);

		CM_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
		CM_ASSERT (rc == LSA_RET_OK);
		return;
	}
	}

	CM_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cm_open_channel: h(%u) nic_id(%u) path_type(%u)"
		, channel->my_handle, channel->detailptr->nic_id, channel->detailptr->path_type
		);

	/***/

	channel->usr.ch.curr = LSA_NULL;

	cm_res_init(channel);

#if CM_CFG_USE_ACP
	cm_overload_init (channel);
#endif

	channel->acp.max_alarm_data_length = 0; /* see below */

	channel->acp.alarm_reject = 0xFFFFFFFF; /* see cm_cl_create() */
	channel->acp.alarm_priority = 0;

	/***/

	channel->lower.handle = 0; /* sanity */

	{
	LSA_UINT16 path_type;

	for( path_type = 0; path_type < CM_PATH_TYPE_ANY_MAX; ++path_type ) {

		channel->lower.channel[path_type] = LSA_NULL;

		if( path_type <= CM_PATH_TYPE_SYS_MAX ) {

			if( path_type == channel->detailptr->path_type ) { /* make a link to 'self' */

				channel->lower.channel[path_type] = channel; /* usage see alloc/free/request-lower */
			}
		}
	}
	}

	/* check for duplicates */

	{
	CM_CHANNEL_PTR_TYPE existing_channel = cm_channel_find_path_type(channel->detailptr->nic_id, channel->detailptr->path_type);

#if CM_CFG_USE_GSY
#if CM_CFG_MAX_PTCPSUBDOMAINS > 1
	if (channel->detailptr->path_type == CM_PATH_TYPE_GSY) {

		existing_channel = LSA_NULL; /* cannot check here if more than one sync-id */
	}
#endif
#endif

	if (is_not_null(existing_channel)) {

		CM_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "channel with nic_id(%u) path_type(%u) already exists"
			, channel->detailptr->nic_id, channel->detailptr->path_type
			);

		cm_callback_and_free_channel(channel, CM_ERR_SEQUENCE, rb);
		return;
	}
	}

	/* check if edd-channel exists */

	if (channel->detailptr->path_type != CM_PATH_TYPE_EDD) {

		CM_CHANNEL_PTR_TYPE edd_channel = cm_channel_find_path_type(channel->detailptr->nic_id, CM_PATH_TYPE_EDD);

		if (is_null(edd_channel)) {

			CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "edd-channel with nic_id(%u) not opened (see documentation)"
				, channel->detailptr->nic_id
				);

			cm_callback_and_free_channel(channel, CM_ERR_SEQUENCE, rb);
			return;
		}
		else { /* make a link to the edd-channel */

			channel->lower.channel[CM_PATH_TYPE_EDD] = edd_channel;
			channel->trace_idx = edd_channel->trace_idx;
		}
	}

	/***/

	channel->state = CM_CH_OPEN;

	channel->usr.ch.curr = rb; /* keep */

	/*
	 * open the lower layer
	 */

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_EDD:
		channel->lower.handle = CM_EDD_INVALID_HANDLE;
		cm_edd_channel_open (channel);
		break;

#if CM_CFG_USE_MRP
	case CM_PATH_TYPE_MRP:
		channel->lower.handle = CM_MRP_INVALID_HANDLE;
		cm_mrp_channel_open (channel);
		break;
#endif

#if CM_CFG_USE_GSY
	case CM_PATH_TYPE_GSY:
		channel->lower.handle = CM_GSY_INVALID_HANDLE;
		cm_gsy_channel_open (channel);
		break;
#endif

#if CM_CFG_USE_NARE
	case CM_PATH_TYPE_NARE:
		channel->lower.handle = CM_NARE_INVALID_HANDLE;
		cm_nare_channel_open (channel);
		break;
#endif

#if CM_CFG_USE_POF
	case CM_PATH_TYPE_POF:
		channel->lower.handle = CM_POF_INVALID_HANDLE;
		cm_pof_channel_open (channel);
		break;
#endif

	case CM_PATH_TYPE_OHA:
		channel->lower.handle = CM_OHA_INVALID_HANDLE;
		cm_oha_channel_open (channel);
		break;

	case CM_PATH_TYPE_RPC:
		channel->lower.handle = CM_RPC_INVALID_HANDLE;
		cm_rpc_channel_open (channel);
		break;

	case CM_PATH_TYPE_PD:
		channel->lower.handle = 0/*CM_xxx_INVALID_HANDLE*/;

		if (! cm_channel_sys_opened(channel->detailptr->nic_id, channel->detailptr->path_type)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);

			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		cm_channel_sys_assign(channel);

		cm_channel_done(channel, CM_OK); /* no lower layer */
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		channel->lower.handle = ACP_INVALID_HANDLE;

		if (! cm_channel_sys_opened(channel->detailptr->nic_id, channel->detailptr->path_type)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);
			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		channel->acp.max_alarm_data_length = channel->detailptr->u.cl.acp.max_alarm_data_length; /*AP00342171*/

		if (! (CM_ALARM_DATA_LENGTH_MIN <= channel->acp.max_alarm_data_length
				&& channel->acp.max_alarm_data_length <= CM_ALARM_DATA_LENGTH_MAX))
		{
			CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!(%u <= max_alarm_data_length(%u) <= %u)"
				, CM_ALARM_DATA_LENGTH_MIN, channel->acp.max_alarm_data_length, CM_ALARM_DATA_LENGTH_MAX
				);
			cm_callback_and_free_channel (channel, CM_ERR_SYS_PATH, channel->usr.ch.curr);
			break;
		}

		cm_channel_sys_assign(channel);

		cm_acp_channel_open (channel);
		break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		channel->lower.handle = ACP_INVALID_HANDLE;

		if (! cm_channel_sys_opened(channel->detailptr->nic_id, channel->detailptr->path_type)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);

			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		channel->acp.max_alarm_data_length = 0;

		cm_channel_sys_assign(channel);

		cm_acp_channel_open (channel);
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		channel->lower.handle = ACP_INVALID_HANDLE;

		if (! cm_channel_sys_opened(channel->detailptr->nic_id, channel->detailptr->path_type)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);
			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		channel->acp.max_alarm_data_length = channel->detailptr->u.sv.acp.max_alarm_data_length;

		if (! (CM_ALARM_DATA_LENGTH_MIN <= channel->acp.max_alarm_data_length
				&& channel->acp.max_alarm_data_length <= CM_ALARM_DATA_LENGTH_MAX))
		{
			CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!(%u <= max_alarm_data_length(%u) <= %u)"
				, CM_ALARM_DATA_LENGTH_MIN, channel->acp.max_alarm_data_length, CM_ALARM_DATA_LENGTH_MAX
				);
			cm_callback_and_free_channel (channel, CM_ERR_SYS_PATH, channel->usr.ch.curr);
			break;
		}

		cm_channel_sys_assign(channel);

		cm_acp_channel_open (channel);
		break;
#endif

	default:
		CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong path_type(%u)", channel->detailptr->path_type
			);
		cm_callback_and_free_channel (channel, CM_ERR_SYS_PATH, channel->usr.ch.curr);
		break;
	}
}


/*------------------------------------------------------------------------------
//	close the channel after closing the channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID CM_UPPER_IN_FCT_ATTR
cm_close_channel (
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT ("cm_close_channel")
	CM_CHANNEL_PTR_TYPE channel;
	LSA_UINT16 response;

		/* sanity checks */

	channel = is_null(rb) ? LSA_NULL : cm_channel_from_handle(CM_RQB_GET_HANDLE(rb));

	if (is_null(channel)) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
			, "no rqb(%08X) or invalid handle(%u)"
			, rb
			, (is_null(rb) ? 0 : CM_RQB_GET_HANDLE(rb))
			);
		CM_UPPER_RQB_ERROR (rb);
		return;
	}

#if CM_DEBUG
	CM_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CM_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			">>> cm-rqb(0x%08x) h(%u) cm-opc(%u)",
			rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb)
		);

	/* note: do not check rb->args! (don't care) */

	if (
		CM_RQB_GET_OPCODE (rb) != CM_OPC_CLOSE_CHANNEL
		|| channel->state != CM_CH_READY
	) {
		CM_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) wrong opcode or state(%u) not ready"
			, channel->my_handle, channel->state
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	/*
	 * check if can close
	 */

	if( channel->detailptr->path_type <= CM_PATH_TYPE_SYS_MAX ) {

		if (! cm_channel_usr_closed(channel->detailptr->nic_id, channel->detailptr->path_type)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all user-channels closed"
				);
			cm_callback(channel, CM_ERR_SEQUENCE, &rb);
			return;
		}
	}

	response = CM_OK;

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_EDD:
		response = cm_edd_channel_undo_init (channel);
		break;

#if CM_CFG_USE_MRP
	case CM_PATH_TYPE_MRP:
		response = cm_mrp_channel_undo_init (channel);
		break;
#endif

#if CM_CFG_USE_GSY
	case CM_PATH_TYPE_GSY:
		response = cm_gsy_channel_undo_init (channel);
		break;
#endif

#if CM_CFG_USE_NARE
	case CM_PATH_TYPE_NARE:
		break;
#endif

#if CM_CFG_USE_POF
	case CM_PATH_TYPE_POF:
		response = cm_pof_channel_undo_init (channel);
		break;
#endif

	case CM_PATH_TYPE_OHA:
		response = cm_oha_channel_undo_init (channel);
		break;

	case CM_PATH_TYPE_RPC:
		break;

	case CM_PATH_TYPE_PD:
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("TODO: check if cl/mc/sv open, else err-seq")
#endif /* CM_MESSAGE */
		response = cm_pd_channel_undo_init (channel);
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		response = cm_cl_channel_undo_init (channel);
		break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		response = cm_mc_channel_undo_init (channel);
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		/* nothing */
		break;
#endif

	default:
		CM_FATAL(); /* a bug */
		break;
	}

	if( response != CM_OK ) {
		CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "undo_init returned rsp(%u)"
			, response
			);
		cm_callback(channel, response, &rb);
		return;
	}

#if CM_DEBUG
	CM_MEMSET(&channel->usr, 0xDD, sizeof(channel->usr)); /* wipe-out */
#endif

	/*
	 * close the lower layer
	 *
	 * NOTE: indication resources are cancelled before calling back, see cm_channel_done()
	 */

	channel->state = CM_CH_CLOSE;

	channel->usr.ch.curr = rb; /* keep */

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_EDD:
		if( ! cm_edd_set_send_clock (channel, 0) ) { /* must do that before closing the channel */
			CM_FATAL();
		}
		break;

#if CM_CFG_USE_MRP
	case CM_PATH_TYPE_MRP:
		cm_mrp_channel_close (channel);
		break;
#endif

#if CM_CFG_USE_GSY
	case CM_PATH_TYPE_GSY:
		cm_gsy_channel_close (channel);
		break;
#endif

#if CM_CFG_USE_NARE
	case CM_PATH_TYPE_NARE:
		cm_nare_channel_close (channel);
		break;
#endif

#if CM_CFG_USE_POF
	case CM_PATH_TYPE_POF:
		cm_pof_channel_close (channel);
		break;
#endif

	case CM_PATH_TYPE_OHA:
		cm_oha_channel_close (channel);
		break;

	case CM_PATH_TYPE_RPC:
		cm_rpc_channel_close (channel);
		break;

	case CM_PATH_TYPE_PD:
		cm_channel_done (channel, CM_OK);
		break;

#if CM_CFG_USE_ACP
	case CM_PATH_TYPE_CLIENT:
	case CM_PATH_TYPE_MULTICAST:
	case CM_PATH_TYPE_SERVER:
		cm_peer_disconnect(channel);
		cm_acp_channel_close (channel);
		break;
#endif

	default:
		CM_FATAL(); /* a bug */
		break;
	}
}


/*------------------------------------------------------------------------------
//	receives a request from the user, dispatches it to the subsystem
//----------------------------------------------------------------------------*/
LSA_VOID CM_UPPER_IN_FCT_ATTR
cm_request (
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT ("cm_request")
	CM_CHANNEL_PTR_TYPE channel;

	if (is_null(rb)) {
		CM_UPPER_RQB_ERROR (rb);
		return;
	}

#if CM_DEBUG
	CM_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CM_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	/*
	 * timer is special
	 */

	if (CM_RQB_GET_OPCODE (rb) == CM_OPC_TIMER) {
		cm_timeout_synchronized(rb);
		return;
	}

	channel = cm_channel_from_handle(CM_RQB_GET_HANDLE (rb));

	if( is_null(channel) ) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
			, "wrong handle(%u), opc(%u)", CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE(rb)
			);
		CM_UPPER_RQB_ERROR (rb);
		return;
	}

	CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			">>> cm-rqb(0x%08x) h(%u) cm-opc(%u)",
			rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE(rb)
		);

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_PD:
		cm_request_pd (channel, rb);
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_request (channel, rb);
		break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		cm_mc_request (channel, rb);
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_opc_dispatch(channel, rb);
		break;
#endif

	default:
		CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "not a user-channel, path_type(%u)", channel->detailptr->path_type
			);
		CM_UPPER_RQB_ERROR (rb);
		break;
	}
}


/*------------------------------------------------------------------------------
//	dispatch common requests
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_request_pd_cl_mc (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT ("cm_request_pd_cl_mc")

	switch( CM_RQB_GET_OPCODE(rb) ) {

	case CM_OPC_CHANNEL_INFO:
		cm_channel_info(channel, rb);
		return;

	case CM_OPC_OVERLOAD_INFO:
		switch( channel->detailptr->path_type ) {

		case CM_PATH_TYPE_PD:
			cm_callback(channel, CM_ERR_NOT_YET, &rb);
			return;

		default:
#if CM_CFG_USE_ACP
			cm_overload_info(channel, rb);
#else
			CM_FATAL();
#endif
			break;
		}
		return;

	case CM_OPC_EVENT_IND_RES_PROVIDE:
		cm_event_provide (channel, rb);
		return;

	case CM_OPC_ALARM_IND_RES_PROVIDE:
		rb->args.channel.alarm->alarm_data_length = 0; /* "don't care", see cm_alarm_provide() */
		rb->args.channel.alarm->alarm_data = LSA_NULL;
		cm_alarm_provide (channel, rb);

		if (channel->detailptr->path_type == CM_PATH_TYPE_PD) { /*AP00348616*/
			cm_pd_alarm_provide_event (channel);
		}
		return;

	case CM_OPC_IND_RES_WITHDRAW:
		cm_res_withdraw(channel, rb);
		return;

	default:
		CM_FATAL();
		return;
	}
}


/*------------------------------------------------------------------------------
//	dispatch to CM-PD
//----------------------------------------------------------------------------*/

static LSA_VOID CM_UPPER_IN_FCT_ATTR
cm_request_pd (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT ("cm_request_pd")

	if (is_null(rb->args.void_ptr)) { /* args must be there always - with some exceptions */

		switch( CM_RQB_GET_OPCODE(rb) ) {

		case CM_OPC_PD_DELETE:
		case CM_OPC_PD_PRM_BEGIN:
			break;

		default:
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "no args"
				);
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}
	}

	/***/

	switch( CM_RQB_GET_OPCODE(rb) ) {

	case CM_OPC_CHANNEL_INFO:
	case CM_OPC_OVERLOAD_INFO:
	case CM_OPC_EVENT_IND_RES_PROVIDE:
	case CM_OPC_ALARM_IND_RES_PROVIDE:
	case CM_OPC_IND_RES_WITHDRAW:
		cm_request_pd_cl_mc(channel, rb);
		return;

	case CM_OPC_PD_CREATE:
		cm_pd_opc_create(channel, rb);
		return;

	case CM_OPC_PD_DELETE:
		cm_pd_opc_delete(channel, rb);
		return;

	case CM_OPC_PD_REMAP:
		cm_pd_opc_remap(channel, rb);
		return;

	case CM_OPC_PD_LINK_STATUS_INFO:
		cm_pd_opc_link_status_info(channel, rb);
		return;

	case CM_OPC_PD_SYNC_LED_INFO:
		cm_pd_opc_sync_led_info(channel, rb);
		return;

	case CM_OPC_PD_PRM_READ:
		cm_pd_opc_prm_read(channel, rb);
		return;

	case CM_OPC_PD_PRM_BEGIN:
		cm_pd_opc_prm_prepare(channel, rb);
		return;

	case CM_OPC_PD_PRM_WRITE:
		cm_pd_opc_prm_write(channel, rb);
		return;

	case CM_OPC_PD_PRM_END:
		cm_pd_opc_prm_end(channel, rb);
		return;

	case CM_OPC_PD_EVENT_APPL_READY_RSP:
		cm_pd_opc_appl_ready_rsp(channel, rb);
		return;

	case CM_OPC_PD_ALARM_RSP:
		cm_pd_opc_alarm_rsp(channel, rb);
		return;

#if CM_CFG_MAX_CLIENTS
	case CM_OPC_PD_PRM_READ_CLIENT:
		cm_pnio_c_read_implicit_rsp (channel/*PD*/, rb);
		return;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_OPC_PD_PRM_READ_SERVER:
	case CM_OPC_PD_PRM_WRITE_SERVER:
	case CM_OPC_PD_PRM_END_SERVER:
	case CM_OPC_PD_PRM_RELINQUISH_SERVER:
		cm_sv_pdev_request_done(channel/*PD*/, rb);
		return;
#endif

	default:
		CM_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unhandled opcode(%u), path-type(%u)"
			, CM_RQB_GET_OPCODE(rb), channel->detailptr->path_type
			);

		CM_UPPER_RQB_ERROR(rb);
		return;
	}
}


/*------------------------------------------------------------------------------
//	dispatch common requests
//----------------------------------------------------------------------------*/

LSA_VOID CM_UPPER_IN_FCT_ATTR
cm_channel_info (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT ("cm_channel_info")
	CM_UPPER_CHANNEL_INFO_PTR_TYPE param = rb->args.channel.info;
	LSA_UINT16 rsp;

	switch( channel->detailptr->path_type ) {

	case CM_PATH_TYPE_CLIENT:
	case CM_PATH_TYPE_MULTICAST:
	case CM_PATH_TYPE_SERVER:
		{
		ACP_CHANNEL_UPPER_INFO_PTR_TYPE  acp = &channel->lower.info.acp;

		param->hardware_type				= acp->hardware_type;

		param->interface_id					= acp->eth_interface_id;
		CM_MEMCPY (param->mac_addr.mac		, acp->eth_mac_addr, sizeof(param->mac_addr.mac));
		param->port_cnt						= acp->eth_ports_max;

		param->process_image_base_addr		= acp->kram_info.kram_base_addr;
		param->process_image_start_offset	= acp->kram_info.offset_process_image_start;
		param->process_image_end_offset		= acp->kram_info.offset_process_image_end;

		rsp = CM_OK;
		}
		break;

	case CM_PATH_TYPE_PD:
		{
		CM_CHANNEL_PTR_TYPE  edd_channel = cm_edd_channel_get(channel);

		param->hardware_type				= edd_channel->lower.info.edd.HardwareType;

		param->interface_id					= CM_CAST_U16(edd_channel->lower.info.edd.InterfaceID);
		CM_MEMCPY (param->mac_addr.mac		, edd_channel->lower.info.edd.MACAddress.MacAdr, sizeof(param->mac_addr.mac));
		param->port_cnt						= edd_channel->lower.info.edd.MaxPortCnt;

		param->process_image_base_addr		= 0;
		param->process_image_start_offset	= 0;
		param->process_image_end_offset		= 0;

		rsp = CM_OK;
		}
		break;

	default:
		rsp = CM_ERR_NOT_YET; /* unreachable */
		break;
	}

	cm_callback(channel, rsp, &rb);
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
