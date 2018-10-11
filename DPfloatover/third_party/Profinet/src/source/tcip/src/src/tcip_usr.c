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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_usr.c                                :F&  */
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
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  2
#define TCIP_MODULE_ID     2

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/
/*===========================================================================*/

static LSA_VOID  TCIP_UPPER_IN_FCT_ATTR tcip_rebuild_route_table(LSA_VOID);

/*------------------------------------------------------------------------------
//	sets the response of the given request-block and calls the
//        request-done-fct (given in TCIP_OPC_OPEN_CHANNEL)
//----------------------------------------------------------------------------*/
LSA_VOID  TCIP_LOCAL_FCT_ATTR
tcip_callback( TCIP_CHANNEL_PTR_TYPE channel,
			   LSA_UINT16 response,
			   TCIP_UPPER_RQB_PTR_TYPE TCIP_LOCAL_MEM_ATTR *upper_ptr )
{
	TCIP_LOG_FCT("tcip_callback")
	TCIP_UPPER_RQB_PTR_TYPE  rb;

	TCIP_ASSERT (is_not_null (upper_ptr));
	TCIP_ASSERT (is_not_null (*upper_ptr));

	rb = *upper_ptr;
	*upper_ptr = LSA_NULL;		/* sanity - never use this again */

	/*
	 *  memorize successful channel open
	 */
	if( TCIP_OK == response )
	{
		if( TCIP_OPC_OPEN_CHANNEL == TCIP_RQB_GET_OPCODE(rb))
		{
			tcip_data.tcip_channels_opened ++;
		}
	}

	TCIP_RQB_SET_HANDLE (rb, channel->handle_upper);
	TCIP_RQB_SET_RESPONSE (rb, response);

	TCIP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< rqb(0x%08x) h(%u) opc(%u) rsp(%u) f-h(%u)"
		, rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE (rb), TCIP_RQB_GET_RESPONSE (rb), channel->my_handle
		);

	TCIP_REQUEST_UPPER_DONE (channel->callback, rb, channel->sysptr);
}

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
//	func: tcip_callback_and_free_channel: release all channel specific memory
//----------------------------------------------------------------------------*/

LSA_VOID TCIP_LOCAL_FCT_ATTR tcip_callback_and_free_channel(
	TCIP_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 response,
	TCIP_UPPER_RQB_PTR_TYPE rb
) {
	TCIP_LOG_FCT ("tcip_callback_and_free_channel")

	TCIP_UPPER_CALLBACK_FCT_PTR_TYPE  callback = channel->callback;
	TCIP_DETAIL_PTR_TYPE  detailptr = channel->detailptr;
	LSA_SYS_PTR_TYPE  sysptr = channel->sysptr;

	tcip_data.tcip_channels_opened --;

	/*
	 * NOTE: the previously used sequence was wrong!
	 *       > tcip_callback(channel, TCIP_OK, &rb);
	 *       > channel->state = TCIP_CST_FREE;
	 *
	 * ==> even if the callback posts into a queue, a call
	 *     to tcip_undo_init() can occur (due to preemption)
	 *     before we have the chance to reset the in-use flag
	 */

	TCIP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< rqb(0x%08x) h(%u) opc(%u) rsp(%u) f-h(%u) channel-free"
		, rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE (rb), TCIP_RQB_GET_RESPONSE (rb), channel->my_handle
		);


	TCIP_RQB_SET_HANDLE (rb, channel->handle_upper);
	TCIP_RQB_SET_RESPONSE (rb, response);

#if TCIP_DEBUG
	TCIP_MEMSET(channel, 0xDD, sizeof(*channel)); /* wipe out */
#endif

	channel->state = TCIP_CH_FREE;				/* free the channel ... */

	TCIP_REQUEST_UPPER_DONE (callback, rb, sysptr); /* ... before calling back! */

	{
	LSA_UINT16  rc;
	TCIP_RELEASE_PATH_INFO (& rc, sysptr, detailptr);
	TCIP_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *	get channel from handle or LSA_NULL if out-of-range
 */
TCIP_CHANNEL_PTR_TYPE  TCIP_LOCAL_FCT_ATTR tcip_channel_from_handle( LSA_HANDLE_TYPE handle)
{
	TCIP_LOG_FCT ("tcip_channel_from_handle")

	if( handle < TCIP_MAX_CHANNELS )
	{
		/* see tcip_open_channel() */
		return &tcip_data.tcip_channels[handle];
	}

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "handle(%u) out of range", handle);

	return LSA_NULL;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *	 returns arp channel pointer with passed group if found, otherwise LSA_NULL
 *   group id value must match, value itself not important
 */
static TCIP_CHANNEL_PTR_TYPE tcip_find_arp_channel_opened(LSA_UINT8 p_group_id )
{
	TCIP_CHANNEL_PTR_TYPE pChRet = LSA_NULL;
	LSA_HANDLE_TYPE hCh;

	for( hCh = 0; hCh < TCIP_MAX_CHANNELS; hCh++)
	{
		TCIP_CHANNEL_PTR_TYPE pCh = &tcip_data.tcip_channels[hCh];

		if( TCIP_CH_OPEN != pCh->state )
		{
			continue;
		}

		if( ( TCIP_PATH_TYPE_ARP == pCh->detailptr->path_type) &&
			( p_group_id == pCh->detailptr->group_id ) )
		{
			pChRet = pCh;
			break;
		}
	}
	return pChRet;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  returns opt (ICMP, UDP or TCP) channel pointer with passed group id if found, otherwise LSA_NULL
 */
static TCIP_CHANNEL_PTR_TYPE tcip_find_opt_channel_opened(LSA_UINT8 p_group_id)
{
	TCIP_CHANNEL_PTR_TYPE pChRet = LSA_NULL;
	LSA_HANDLE_TYPE hCh;

	for( hCh = 0; hCh < TCIP_MAX_CHANNELS; hCh++)
	{
		TCIP_CHANNEL_PTR_TYPE pCh = &tcip_data.tcip_channels[hCh];
		LSA_UINT8 ch_path_type;

		if( TCIP_CH_OPEN != pCh->state )
		{
			continue;
		}

		ch_path_type = pCh->detailptr->path_type;

		if( ( p_group_id == pCh->detailptr->group_id  )
			&& ( TCIP_PATH_TYPE_ICMP == ch_path_type
				|| TCIP_PATH_TYPE_UDP == ch_path_type
				|| TCIP_PATH_TYPE_TCP == ch_path_type
				)
		  )

		{
			pChRet = pCh;
			break;
		}
	}
	return pChRet;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  initialize send - and receive resources for one channel
 */
LSA_VOID tcip_init_send_recv_resources(TCIP_CHANNEL_PTR_TYPE channel )
{
	TCIP_LOG_FCT("tcip_init_send_recv_resources")
	LSA_UINT32 i;
	LSA_UINT32 nr_of_recv = channel->detailptr->number_of_recv_resources;
	LSA_UINT32 nr_of_send = channel->detailptr->number_of_send_resources;

	channel->edd.pending_rcv_reqests = 0;   /* initial state*/

	/*
	 * initialize send resources for channel
	 */

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "providing %u send-resources", nr_of_send);

	tcip_initEddSendRQBList((EDD_UPPER_RQB_PTR_TYPE*)&( channel->edd.send_rqbs),
		               nr_of_send,
					   channel->edd.handle,
					   channel->sysptr);


	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "providing %u recv-resources", nr_of_recv);

	for ( i= 0; i < nr_of_recv; i++ )  /* provide receive resources for channel      */
	{
		tcip_AllocRcvBuf(channel);
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  initialize arp channel: create interfaces and send-recv resources
 */
LSA_VOID TCIP_LOCAL_FCT_ATTR tcip_init_arp_channel(TCIP_CHANNEL_PTR_TYPE channel)
{

	if( 0 == tcip_data.tcip_channels_opened ) /* first (of all) ARP channels */
	{
		tcip_iniche_create_interfaces(channel->p_if_data->nets_count);

		/***/

		{
		LSA_USER_ID_TYPE uUserId = { 0 };
		LSA_UINT16 retVal;
		TCIP_START_TIMER(&retVal, tcip_data.tcip_TimerID, uUserId, (TPS/10));
		TCIP_ASSERT(LSA_RET_OK == retVal);
		}
	}

	/***/

	channel->edd.pEdd = tcip_AllocRQB(channel->sysptr, EDD_SRV_GET_STATISTICS_ALL, 0, LSA_NULL);

	TCIP_ASSERT( is_not_null(channel->edd.pEdd) );

	/***/

	tcip_iniche_activate_interfaces(channel); /* "activate" interfaces */

	tcip_edd_ext_link_status_provide(channel);

	tcip_init_send_recv_resources(channel);
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  initialize optional channels (icmp, udp and tcp):
 *  set interface pointer of arp channel;
 *  set channel pointer in net-structs of interface
 */
LSA_VOID TCIP_LOCAL_FCT_ATTR tcip_init_opt_channel(TCIP_CHANNEL_PTR_TYPE channel)
{
	TCIP_CHANNEL_PTR_TYPE arp_channel = tcip_find_arp_channel_opened(channel->detailptr->group_id);

	if( is_null(arp_channel) )
	{
		TCIP_FATAL();
	}
	else
	{

		/*
		 * set interface struct pointer of arp channel for this channel also
		 */

		channel->p_if_data = arp_channel->p_if_data; /*all channel of one interface have the same pointer*/

		/***/

		tcip_init_send_recv_resources(channel);      /* init send-receive resources for new channel */
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *	deinit arp channel: remove interface entries, release interface struct
 */
LSA_VOID TCIP_LOCAL_FCT_ATTR tcip_deinit_arp_channel(TCIP_CHANNEL_PTR_TYPE channel)
{
	tcip_iniche_deactivate_interfaces(channel);

	{
	LSA_UINT16 retval;
	TCIP_FREE_LOCAL_MEM(&retval, channel->p_if_data);
	TCIP_ASSERT(LSA_RET_OK == retval);
	}

	channel->p_if_data = LSA_NULL;

	/*
	 *  if last arp channel is closed, stop timer and clear reasm queue
	 */
	if( 1 == tcip_data.tcip_channels_opened )
	{
		{
		LSA_UINT16 retVal;
		TCIP_STOP_TIMER(&retVal, tcip_data.tcip_TimerID);
		TCIP_ASSERT(LSA_RET_OK == retVal);
		}
//TODO: review stopping a cyclic timer, re-read prefix_sys.h

		/***/

		tcip_iniche_delete_interfaces();
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
// reset channel's interface pointer
 //----------------------------------------------------------------------------*/
LSA_VOID TCIP_LOCAL_FCT_ATTR tcip_deinit_opt_channel(TCIP_CHANNEL_PTR_TYPE channel)
{
	/*
	 * set interface struct pointer to zero
	 */
	channel->p_if_data = LSA_NULL;
}

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
//	create a tcip instance and open a channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID TCIP_UPPER_IN_FCT_ATTR tcip_open_channel ( TCIP_UPPER_RQB_PTR_TYPE  rb )
{
	TCIP_LOG_FCT("tcip_open_channel")

	TCIP_UPPER_OPEN_CHANNEL_PTR_TYPE open;
	TCIP_CHANNEL_PTR_TYPE channel;

	/* sanity checks */

	if (is_null (rb) || TCIP_RQB_GET_OPCODE (rb) != TCIP_OPC_OPEN_CHANNEL)
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "no rqb(%08X) or wrong opcode or no args", rb);
		TCIP_FATAL();
		return;
	}

#if (TCIP_DEBUG == 1)
	TCIP_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	TCIP_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	open = &rb->args.open; /* shortcut */

	open->handle = TCIP_INVALID_HANDLE; /* sanity */

	if (open->tcip_request_upper_done_ptr == 0) /* AP01382171: dont' use macro is_null() */
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "no callback function given" );
		TCIP_FATAL();
		return;
	}

	/* get path info and search a free tcip-handle*/

	channel = LSA_NULL;

	{
	TCIP_DETAIL_PTR_TYPE detailptr = LSA_NULL;
	LSA_SYS_PTR_TYPE sysptr;
	LSA_UINT16 rc;
	LSA_HANDLE_TYPE my_handle;
	LSA_RESPONSE_TYPE response;

	TCIP_GET_PATH_INFO (&rc, &sysptr, &detailptr, open->sys_path);

	if (rc != LSA_RET_OK || is_null (detailptr))
	{
		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "tcip_get_path_info: rc(%u) detailptr(%08x)", rc, detailptr);
		TCIP_FATAL();
		return;
	}

	for (my_handle = 0; my_handle < TCIP_MAX_CHANNELS; ++my_handle)
	{
		channel = &tcip_data.tcip_channels[my_handle];
		if (channel->state == TCIP_CH_FREE) {
			#if TCIP_DEBUG
				TCIP_MEMSET(channel, 0xCC, sizeof(*channel)); /* to find uninitialized things */
				channel->state = TCIP_CH_FREE;
			#endif
			break; /* found */
		}
	}

	if ( my_handle >= TCIP_MAX_CHANNELS || is_null(channel) /* sanity */ )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "no free channel found, TCIP_MAX_CHANNELS(%u)", TCIP_MAX_CHANNELS );
		response = TCIP_ERR_RESOURCE;
	}
	else if (detailptr->number_of_recv_resources == 0) /* receiving over all channels */
	{
		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "number_of_recv_resources 0 on path_type(%d), sys_path(%d)", detailptr->path_type, open->sys_path);
		response = TCIP_ERR_SYS_PATH;
	}
	else 
	{
		TCIP_CHANNEL_PTR_TYPE pChannelArp = tcip_find_arp_channel_opened(detailptr->group_id);  /*look for open ARP channel with group id of channel to be opened*/
		
		if (TCIP_PATH_TYPE_ARP == detailptr->path_type) /* ARP channel */
		{
			if (is_not_null(pChannelArp))
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "open ARP channel with same group_id(%d) found, sys_path(%d)", detailptr->group_id, open->sys_path);
				response = TCIP_ERR_SEQUENCE;
			}
			else if (detailptr->number_of_send_resources == 0)
			{
				TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "number_of_send_resources must not be 0 for ARP channel, sys_path(%d)", open->sys_path);
				response = TCIP_ERR_SYS_PATH;
			}
			else
			{
				response = TCIP_OK;
			}
		}
		else /* ICMP, UDP, TCP channel */
		{
			if (is_null(pChannelArp)) /* ARP-channel must be opened first for channel group, otherwise sequence error */
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "no open ARP channel with group_id(%d) found, sys_path(%d)", detailptr->group_id, open->sys_path);
				response = TCIP_ERR_SEQUENCE;
			}
			else if (detailptr->number_of_send_resources != 0) /* no sending over ICMP, UDP and TCP channel */
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "number_of_send_resources not 0 on path_type(%d), sys_path(%d)", detailptr->path_type, open->sys_path);
				response = TCIP_ERR_SYS_PATH;
			}
			else
			{
				response = TCIP_OK;
			}
		}
	}

	if (response != TCIP_OK)
	{
		TCIP_RQB_SET_RESPONSE (rb, response);
		TCIP_RQB_SET_HANDLE (rb, open->handle_upper);
		TCIP_REQUEST_UPPER_DONE (open->tcip_request_upper_done_ptr, rb, sysptr);
		TCIP_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
		TCIP_ASSERT (rc == LSA_RET_OK);
		return;
	}

	channel->state			= TCIP_CH_ALLOCATED;
	channel->my_handle		= my_handle;
	channel->handle_upper	= open->handle_upper;
	channel->syspath        = open->sys_path;
	channel->callback		= open->tcip_request_upper_done_ptr;
	channel->sysptr			= sysptr;
	channel->detailptr		= detailptr;
	channel->trace_idx		= 0; /* sanity, see below */
	channel->p_if_data      = LSA_NULL;

	/*
	 *   init lower data
	 */
	channel->edd.handle     = (LSA_HANDLE_TYPE)-1;
	channel->edd.pending_rcv_reqests = 0;
	channel->edd.in_work_rcv_rqbs_number = 0;
	channel->edd.send_pkts.first_out  = LSA_NULL;
	channel->edd.send_pkts.last_pkt   = LSA_NULL;
	channel->edd.send_pkts.cur_len = 0;
	channel->edd.send_pkts.max_len = 0; /* see tcip_edd_get_params_done() */
	channel->edd.send_rqbs  = LSA_NULL;
	channel->edd.get_statistics_pending = LSA_FALSE;
	channel->edd.pEdd = LSA_NULL;
	}

	TCIP_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "tcip_open_channel: h(%u) group_id(%u) path_type(%u)"
		, channel->my_handle, channel->detailptr->group_id, channel->detailptr->path_type
		);

	/***/

	open->handle = channel->my_handle;

	channel->curr = rb;

	/*
	 * open the lower layer
	 */

	channel->edd.handle = TCIP_EDD_INVALID_HANDLE;

	switch( channel->detailptr->path_type )
	{
	case TCIP_PATH_TYPE_ARP:
	case TCIP_PATH_TYPE_ICMP:
	case TCIP_PATH_TYPE_UDP:
	case TCIP_PATH_TYPE_TCP:
 		tcip_open_channel_all(channel);
		break;

	default:
		TCIP_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong path_type(%u)", channel->detailptr->path_type );

		tcip_callback_and_free_channel (channel, TCIP_ERR_SYS_PATH, channel->curr);
		break;
	}
}


/*------------------------------------------------------------------------------
//	close the channel after closing the channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID TCIP_UPPER_IN_FCT_ATTR
tcip_close_channel (
	TCIP_UPPER_RQB_PTR_TYPE  rb
) {
	TCIP_LOG_FCT("tcip_close_channel")
	TCIP_CHANNEL_PTR_TYPE channel;

	/* sanity checks */

	channel = is_null(rb) ? LSA_NULL : tcip_channel_from_handle(TCIP_RQB_GET_HANDLE(rb));

	if (is_null(channel))
	{
		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "no rqb(%08X) or invalid handle(%u)", rb, (is_null(rb) ? 0 : TCIP_RQB_GET_HANDLE(rb)) );
		TCIP_FATAL();
		return;
	}

#if TCIP_DEBUG
	TCIP_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	TCIP_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	TCIP_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			">>> rqb(0x%08x) h(%u) opc(%u)",
			rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE (rb)
		);

	if ( ( TCIP_RQB_GET_OPCODE (rb) != TCIP_OPC_CLOSE_CHANNEL ) ||
		 ( channel->state != TCIP_CH_OPEN )  )
	{
		TCIP_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
			"h(%u) wrong opcode or state(%u) not ready",
			channel->my_handle, channel->state );
		tcip_callback (channel, TCIP_ERR_SEQUENCE, & rb);
		return;
	}

	channel->curr  = rb;

	/* sanity check: arp channel is the last one to be closed for one channel group */

	if( TCIP_PATH_TYPE_ARP == channel->detailptr->path_type )
	{
		TCIP_CHANNEL_PTR_TYPE pChannelOpt;

		pChannelOpt = tcip_find_opt_channel_opened(channel->detailptr->group_id);

		if( is_not_null(pChannelOpt) )   /* check: optional channel open for this group id ? --> sequence error */
		{
			TCIP_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
				"arp channel close: opt channel with same group_id(%d) found", channel->detailptr->group_id );
			tcip_callback(channel, TCIP_ERR_SEQUENCE, &channel->curr );
			return;
		}
	}

	/***/

	channel->state = TCIP_CH_CLOSING;

	tcip_iniche_flush_reasm_cache(channel);  /* (!)function disregards channel assignment of packets */

	/***/

	/* edd channel is closing -->  de-init interface functionality */

	if( TCIP_PATH_TYPE_ARP != channel->detailptr->path_type )
	{
		tcip_deinit_opt_channel(channel);
	}
	else
	{
		tcip_deinit_arp_channel(channel);

#if TCIP_CFG_ENABLE_MULTICAST
		/* remove entry for all hosts group (224.0.0.1 -> MAC: 01-00-5E-00-00-01 */

		tcip_edd_srv_multicast(channel, 0, htonl(0xE0000001), TCIP_SRV_MULTICAST_DISABLE);

		return; /* not yet nrt_cancel, see tcip_edd_srv_multicast_done() */
#endif
	}

	/* close the lower layer; NOTE: EDD indication resources are cancelled before calling back */

	tcip_edd_nrt_cancel( channel );
}


/*------------------------------------------------------------------------------
//	receives a request from the user, dispatches it to the subsystem
//----------------------------------------------------------------------------*/
LSA_VOID TCIP_UPPER_IN_FCT_ATTR
tcip_request (
	TCIP_UPPER_RQB_PTR_TYPE  rb
) {
	TCIP_LOG_FCT("tcip_request")

	if( is_null(rb) )
	{
		TCIP_FATAL();
	}
	else
	{
#if TCIP_DEBUG
		TCIP_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
		TCIP_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

		if( TCIP_RQB_GET_OPCODE(rb) == TCIP_OPC_TIMER )
		{
			TCIP_EXCHANGE_LONG(&rb->args.timer.in_use, 0);

			tcip_iniche_ctick();
		}
		else
		{
			TCIP_CHANNEL_PTR_TYPE channel = tcip_channel_from_handle(TCIP_RQB_GET_HANDLE(rb));

			if( is_null(channel) )
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,
					"wrong handle(%u), opc(%u)",
					TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE(rb)
					);

				TCIP_FATAL();
			}
			else
			{
				TCIP_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
					">>> rqb(0x%08x) h(%u) opc(%u)",
					rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE(rb)
					);

				TCIP_FATAL(); /* TCIP has no other opcodes */
			}
		}
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 * function:       sets the ip address for a network interface
 * documentation:  LSA_TCIP_Detailspec.doc
 */
LSA_BOOL TCIP_UPPER_IN_FCT_ATTR
tcip_set_ip_suite(
    LSA_UINT32  interface_id,
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask,
	LSA_UINT32	gateway
)
{
	int i;

	/*
	 *  find interface with matching interface id
	 */
	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch;
		TCIP_IF_DATA_PTR_TYPE p_if_data;

		ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN != ch->state
			|| ch->detailptr->path_type != TCIP_PATH_TYPE_ARP
			)
		{
			continue;
		}

		TCIP_ASSERT(is_not_null(ch->p_if_data));
		p_if_data = ch->p_if_data;

		/*
		 *  look for matching interface id
		 */
		if( interface_id == p_if_data->edd_interface_id )
		{
			int port;

			for( port=0; port <= p_if_data->port_count; port ++ )
			{
				NET ifp = p_if_data->port[port].ifp;
				int ret;

				ret = ni_set_ipsuite(ifp, ip_address, subnet_mask, gateway );

				if( ret != 0 )
				{
					TCIP_FATAL1(ret);
				}
			}

			break;
		}
	}

	/***/

	tcip_rebuild_route_table(); /* AP01256605 */

	/***/

	return LSA_TRUE;
}

/*---------------------------------------------------------------------------*/

static LSA_VOID TCIP_UPPER_IN_FCT_ATTR
tcip_add_route(
   RTMIB	rtp,        /* route table entry */
   ip_addr  dest,       /* ultimate destination */
   ip_addr  mask,       /* net mask, 0xFFFFFFFF if dest is host address */
   ip_addr  nexthop,    /* where to forward to */
   long     type,       /* ipRouteType; 3=direct 4=indirect  */
   NET      ifp
) {
   rtp->ifp = ifp;
   rtp->ipRouteDest = dest;
   rtp->ipRouteIfIndex = (long)(if_netnumber(ifp)) + 1; /* set interface number */
   rtp->ipRouteMetric1 = ip_mib.ipDefaultTTL;
   rtp->ipRouteMetric2 = -1;
   rtp->ipRouteMetric3 = -1;
   rtp->ipRouteMetric4 = -1;
   rtp->ipRouteNextHop = nexthop;
   rtp->ipRouteProto = IPRP_LOCAL;                 /* icmp, or whatever */
   rtp->ipRouteType = type;
   rtp->ipRouteAge = cticks;        /* timestamp it */
   rtp->ipRouteMask = mask;
   rtp->ipRouteMetric5 = -1;
}

/*---------------------------------------------------------------------------*/

/*
 *   tcip_rebuild_route_table()
 *   deletes ip route table and rebuilds it
 */
static LSA_VOID TCIP_UPPER_IN_FCT_ATTR
tcip_rebuild_route_table (
	LSA_VOID
 ) {
	int i;
	unsigned int route_index;

	LSA_BOOL gateway_added = LSA_FALSE;

	for(route_index=0; route_index < ipRoutes; route_index++)
	{
		rt_mib[route_index].ipRouteNextHop = 0;
	}

	route_index = 0;

	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN == ch->state
			&& ch->detailptr->path_type == TCIP_PATH_TYPE_ARP
			&& is_not_null(ch->p_if_data)
			)
		{
			NET ifp = ch->p_if_data->port[0].ifp; /* 0 = the interface */

			if( ifp->n_ipaddr != 0 )
			{
				if( route_index < ipRoutes) /* sanity */
				{
					tcip_add_route(&rt_mib[route_index] /* append route for subnet */
						, (ifp->n_ipaddr & ifp->snmask)
						, ifp->snmask
						, ifp->n_ipaddr
						, 3 /* direct route */
						, ifp
						);

					route_index++;

					if ( ifp->n_defgw != 0 /* add one default gateway if present */
						 && !gateway_added
						 && route_index < ipRoutes /* sanity */
						)
					{
						tcip_add_route(&rt_mib[route_index] /* append route for default gateway, see AP01256605 */
							, 0
							, 0
							, ifp->n_defgw
							, 4 /* indirect route */
							, ifp
							);

						route_index++;

						gateway_added = LSA_TRUE;
					}
				}
			}
		}
	}
}


/*===========================================================================*/
/*===========================================================================*/

/*
 * function:       returns the mac address for an existing network interface
 * documentation:  LSA_TCIP_Detailspec.doc
 */
LSA_BOOL  TCIP_UPPER_IN_FCT_ATTR
tcip_get_any_interface_mac_addr(
    LSA_UINT32* p_interface_id,
	LSA_UINT8*	p_mac_address,
	LSA_UINT16	mac_address_length
)
{
	int i;

	if( is_null(p_interface_id) )
	{
		return LSA_FALSE;
	}

	if( is_null(p_mac_address) )
	{
		return LSA_FALSE;
	}

	if( mac_address_length != 6 )
	{
		return LSA_FALSE;
	}

	/*
	 *  find any interface
	 */
	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch;

		ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN != ch->state )
		{
			continue;
		}

		TCIP_ASSERT(is_not_null(ch->p_if_data));

		*p_interface_id = ch->p_if_data->edd_interface_id;

		TCIP_MEMCPY(p_mac_address, ch->p_if_data->port[0/*!*/].MACAddr.MacAdr, mac_address_length);

		break;
	}

	if( TCIP_MAX_CHANNELS == i )  /* nothing found */
	{
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

#if TCIP_CFG_ENABLE_MULTICAST

/*---------------------------------------------------
// enable / disable multicast mac in EDDx MAC table
//--------------------------------------------------*/
LSA_RESPONSE_TYPE TCIP_UPPER_IN_FCT_ATTR
tcip_srv_multicast(
	LSA_INT32 dev_handle,
	LSA_UINT32 interface_id,
	LSA_UINT32 mc_address, /* (!) network byte order */
	LSA_UINT8 mode
) {
	TCIP_LOG_FCT("tcip_srv_multicast")

	int i;
	TCIP_CHANNEL_PTR_TYPE ch = LSA_NULL;

	/* find EDDx channel for local address */

	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN == ch->state
			&& ch->detailptr->path_type == TCIP_PATH_TYPE_ARP
			&& is_not_null(ch->p_if_data)
			)
		{
			if (ch->p_if_data->edd_interface_id == interface_id)
			{
				break; /* found */
			}
		}
	}

	if( i == TCIP_MAX_CHANNELS ) /* not found */
	{
		return TCIP_ERR_PARAM;
	}

	/***/

	/* add entry for group specified by user */

	tcip_edd_srv_multicast(ch, dev_handle, mc_address, mode);

	return TCIP_OK;
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
