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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_socket.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Productive part of the Socket-Interface                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 12
#define SOCK_MODULE_ID    12

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*===========================================================================*/

static LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_handle_except_mask(SOCK_SOCKET_PTR soc);
static LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_handle_read_mask(SOCK_SOCKET_PTR soc);
static LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_handle_write_mask(SOCK_SOCKET_PTR soc);


/*------------------------------------------------------------------------------
// allocate socket structure for the specified open socket descriptor
//----------------------------------------------------------------------------*/
SOCK_SOCKET_PTR SOCK_LOCAL_FCT_ATTR
sock_socket_alloc(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCKIF_FD fd,
	LSA_INT32 type
) {
	SOCK_SOCKET_PTR soc = LSA_NULL;
	LSA_UINT16    handle;
	
	LSA_INT option_ret_val = sock_socket_set_options(channel, fd, type);

	if (option_ret_val != SOCK_RSP_OK)
	{
		SockIF_Close(fd);
		return LSA_NULL;
	}

	/* search for free socket struct */
	for (handle = 0; handle<SOCK_MAX_SOCKETS; handle++  )
	{
		if ( sock_data.sockets[handle].state == SOCK_CN_STATE_FREE )
		{
			break;
		}
	}

	if( handle == SOCK_MAX_SOCKETS)
	{
		SockIF_Close(fd);
		return LSA_NULL;
	}
	else
	{
		soc = &sock_data.sockets[handle];

		/* AP01316092, allow optimization. See also sock_socket_execute_select() */
		sock_data.active_sockets_last++;
		sock_data.active_sockets[sock_data.active_sockets_last] = soc; /* removed in sock_socket_free() */
		soc->active_handle = sock_data.active_sockets_last; /* store index, see sock_socket_free() */

		soc->state = SOCK_CN_STATE_DISABLED; /* initial state */

		soc->channel = channel;

		soc->dev_handle = handle;
		soc->sock_fd = fd;
		soc->type = type;

		soc->loc_addr.sin_family = 0;
		soc->loc_addr.sin_port = 0;
		soc->loc_addr.sin_addr.s_addr = 0;
		{
		int i;
		for(i=0; i<8; i++)
		{
			soc->loc_addr.sin_zero[i] = 0;
		}
		}
		soc->rem_addr = soc->loc_addr;
		
		soc->mcast_interface_id = 0;
		soc->mcast_address = 0;

		soc->abort_resp = 0;

		soc->send.bytes_transferred = 0;
		sock_queue_init(&soc->send.queue);
		soc->send.curr = LSA_NULL;

		soc->recv.bytes_transferred = 0;
		sock_queue_init(&soc->recv.queue);
		soc->recv.curr = LSA_NULL;

		soc->connect_rb = LSA_NULL;
		soc->open_close_rb = LSA_NULL;

		sock_queue_init(&soc->accept.queue);
		soc->accept.curr = LSA_NULL;

		soc->exception_rb = LSA_NULL;

		sock_data.socket_count++;

		return soc;
	}
}


/*------------------------------------------------------------------------------
// free socket structure
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_free(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_free")

	if (soc->sock_fd != SOCKIF_INVALID_FD)
	{
		SockIF_Close(soc->sock_fd);
		soc->sock_fd = SOCKIF_INVALID_FD;
	}

	sock_data.socket_count--;

	soc->state = SOCK_CN_STATE_FREE;

	/* AP01316092, allow optimization */
	if (soc->active_handle == sock_data.active_sockets_last) /* last entry */
	{
		 sock_data.active_sockets_last--;
	}
	else if (sock_data.active_sockets_last > 0 ) /* between first and last: overwrite with last entry  */
	{
		sock_data.active_sockets[soc->active_handle] = sock_data.active_sockets[sock_data.active_sockets_last];
		sock_data.active_sockets_last--;
		sock_data.active_sockets[soc->active_handle]->active_handle = soc->active_handle; /* (!) adapt active_handle for moved entry */
	}
	else if (sock_data.active_sockets_last < 0)
	{
		SOCK_FATAL();  /* (!) cannot delete from empty array */
	}

	soc->channel = LSA_NULL;
}


/*------------------------------------------------------------------------------
// get socket structure pointer from index ("handle")
//----------------------------------------------------------------------------*/
SOCK_SOCKET_PTR SOCK_LOCAL_FCT_ATTR
sock_socket_from_handle(
	LSA_UINT32 handle
) {
	if( handle >= SOCK_MAX_SOCKETS )
	{
		return LSA_NULL;
	}

	if( sock_data.sockets[handle].state == SOCK_CN_STATE_FREE )
	{
		return LSA_NULL;
	}

	return &sock_data.sockets[handle];
}


/*------------------------------------------------------------------------------
// get socket structure pointer from index and check channel assignment
//----------------------------------------------------------------------------*/
SOCK_SOCKET_PTR SOCK_LOCAL_FCT_ATTR
sock_socket_from_channel(
	SOCK_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 handle
) {
	SOCK_SOCKET_PTR soc = sock_socket_from_handle(handle);

	if( sock_is_null(soc))
	{
		return LSA_NULL;
	}
	else if( !sock_are_equal(channel, soc->channel) )
	{
		return LSA_NULL;
	}
	else
	{
		return soc;
	}
}


/*------------------------------------------------------------------------------
// sock_socket_do_select
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_do_select(
	SOCK_UPPER_RQB_PTR_TYPE rqb_ptr
) {
	SOCK_RQB_SET_RESPONSE(rqb_ptr, SOCK_RSP_OK_ACTIVE);

	sock_socket_execute_select(0);
}


/*------------------------------------------------------------------------------
// create read-write-except mask, call select, handle result
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_execute_select(
	LSA_UINT32 timeout
) {
	SOCK_LOG_FCT("sock_socket_execute_select")

	static SOCKIF_FD_SET_TYPE  readMask, writeMask, exceptMask;
	SOCKIF_FD maxfds;
	LSA_INT32 cnt = 0;

	LSA_INT32     i;

	maxfds=0;
	SOCKIF_FD_ZERO (&readMask);
	SOCKIF_FD_ZERO (&writeMask);
	SOCKIF_FD_ZERO (&exceptMask);

	for (i=0; i<=sock_data.active_sockets_last; i++ )
	{
		SOCK_SOCKET_PTR soc = sock_data.active_sockets[i];

		if ((soc->state & SOCK_CN_STATE_ACTIVE_MASK) != 0) /* connected, connecting, listening */
		{
			if (sock_is_null(soc->open_close_rb))
			{
				if (sock_is_not_null(soc->recv.curr)
				   || (sock_data.do_tcp_disconnect_check /* only if timer has set flag, see AP01325244 */
				       && (soc->type == SOCKIF_STREAM)/*!soc->sock_udp*/
					   && (soc->state == SOCK_CN_STATE_CONNECTED)
				      ) /* detect TCP RST and FIN, see AP01300966 */
#if SOCK_CFG_SNMP_ON
				   || (sock_are_equal(soc, sock_data.snmp_dev) && (sock_data.snmp_pending_count == 0))
#endif
				   )
				{
					cnt++;
					SOCKIF_FD_SET (soc->sock_fd, &readMask);
					SOCKIF_MAX(maxfds, soc->sock_fd);
				}
			}

			if (sock_is_not_null(soc->accept.curr)
				&& (sock_data.socket_count < SOCK_MAX_SOCKETS)
				)
			{
				cnt++;
				SOCKIF_FD_SET (soc->sock_fd, &readMask);
				SOCKIF_MAX(maxfds, soc->sock_fd);
			}

			if(sock_is_not_null(soc->send.curr) /* write mask */
			   || sock_is_not_null(soc->connect_rb)
			   )
			{
				cnt++;
				SOCKIF_FD_SET (soc->sock_fd, &writeMask);
				SOCKIF_MAX(maxfds, soc->sock_fd);
			}

			if( (soc->type == SOCKIF_STREAM)/*!soc->sock_udp*/
				&& sock_data.do_tcp_disconnect_check
			  )  /* except mask for tcp sockets */
			{
				cnt++;
				SOCKIF_FD_SET (soc->sock_fd, &exceptMask);
				SOCKIF_MAX(maxfds, soc->sock_fd);
			}
		}
	}

	if (sock_data.do_tcp_disconnect_check) /* reset flag, see AP01325244 */
	{
		LSA_UINT16 ret;
		LSA_USER_ID_TYPE user_id;

		sock_data.do_tcp_disconnect_check = LSA_FALSE;

		user_id.uvar32 = 0;

		SOCK_START_TIMER(&ret, sock_data.tcp_disconnect_check_timer_id, user_id, 1);

		if (ret != LSA_RET_OK)
		{
			SOCK_FATAL();
		}
	}

	if( 0 == cnt )
	{
		return;
	}

	SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "number of events in masks = %d", cnt);

	{
	LSA_INT32     ret_val;

	ret_val = SockIF_Select(maxfds+1, &readMask, &writeMask, &exceptMask, timeout);

	if (SOCKIF_SOCKET_ERROR == ret_val)
	{
		return; /* TODO: error treatment*/
	}
	}

	cnt = 0;

	for (i=0; i<=sock_data.active_sockets_last; i++)
	{
		SOCK_SOCKET_PTR  soc = sock_data.active_sockets[i];

		if ((soc->state & SOCK_CN_STATE_ACTIVE_MASK) != 0)
		{
			if (SOCKIF_FD_ISSET(soc->sock_fd, &exceptMask)) /* (!) exception */
			{
				cnt++;
				sock_socket_handle_except_mask(soc);
				if(soc->type == SOCKIF_STREAM/*!soc->sock_udp*/)
				{
					continue; /* (!) do not check other masks */
				}
			}

			if (SOCKIF_FD_ISSET(soc->sock_fd, &readMask)) /* read mask */
			{
				cnt++;
				sock_socket_handle_read_mask(soc);
			}

			if (SOCKIF_FD_ISSET(soc->sock_fd, &writeMask)) /* write mask */
			{
				cnt++;
				sock_socket_handle_write_mask(soc);
			}
		}
	}

	SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "nunmber of signalled events in masks : %d", cnt);
}


/*------------------------------------------------------------------------------
// sock_socket_handle_connect_error():
// return mapped connect response and close socket if possible
//----------------------------------------------------------------------------*/
static LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_handle_connect_error(
	SOCK_SOCKET_PTR soc,
	LSA_INT sock_error
) {
	SOCK_LOG_FCT("sock_socket_handle_connect_error")

	LSA_OPCODE_TYPE opc = SOCK_RQB_GET_OPCODE(soc->connect_rb); /* (!) */

	LSA_RESPONSE_TYPE response = SockIF_MapConnectError(sock_error);

	SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
		, "connect failed for socket handle(%d), fd(0x%x),native_error(%d), response(%d)"
		, soc->dev_handle, soc->sock_fd, sock_error, response
		);

	sock_usr_callback(soc->channel, response, &soc->connect_rb);

	sock_socket_return_exception_resource(soc, response);

	if( opc == SOCK_OPC_CONNECT ) /* close socket here since user has no handle */
	{
		sock_socket_free(soc);
	}
	else
	{
		soc->state = SOCK_CN_STATE_OPENED; /* reset to opened state */
	}
}


/*------------------------------------------------------------------------------
// sock_socket_handle_except_mask():
// handle situation if socket is in except mask after select call
//----------------------------------------------------------------------------*/
static LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_handle_except_mask(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_handle_except_mask")

	LSA_INT sock_error;
	LSA_INT native_error;
	
	SockIF_Get_Socket_Error(soc->sock_fd, &sock_error, &native_error); /* erase error from socket */

	if(soc->type == SOCKIF_DGRAM)
	{
		return;
	}

	if(sock_is_not_null(soc->connect_rb)) /* connect failed -> return rqb */
	{
		sock_socket_handle_connect_error(soc, sock_error);
	}
	else if(sock_is_not_null(soc->accept.curr)) /* accept() failed -> try again */
	{
		/* if the stack stored the error in the listening socket (InterNiche does this) it was erased before (see getsockopt() above) */

		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
			, "listening socket handle(%d), fd(0x%x), native_error(%d), n o t returning accept rqb"
			, soc->dev_handle, soc->sock_fd, sock_error
			);
	}
	else /* "normal" TCP socket (see above: if UDP -> return) -> cancel send and receive rqb's */
	{
		LSA_RESPONSE_TYPE response = SockIF_MapTCPError(sock_error);
		
		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
		  , "exception on TCP socket handle(%d), fd(0x%x), native_error(%d), returning all rqb's"
		  , soc->dev_handle, soc->sock_fd, sock_error
		  );

		sock_socket_return_all_resources(soc, response); /* -> CN_STATE_DISCONNECTING */
	}
}


/*------------------------------------------------------------------------------
// sock_socket_handle_read_mask():
// handle situation if socket is in read mask after select call
//----------------------------------------------------------------------------*/
static LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_handle_read_mask(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_handle_read_mask")

	if(sock_is_not_null(soc->accept.curr)) /* client connect */
	{
		SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "listening socket_fd(0x%x) in read mask"
			, soc->sock_fd
			);

		sock_tcp_accept(soc);
	}
	else
	{
		if( soc->type == SOCKIF_STREAM/*!soc->sock_udp*/   /* tcp sockets: peek to detect TCP RST and FIN, see AP01300966 */
			&& sock_is_null(soc->recv.curr)
			)
		{
			char buf;
			LSA_INT32 result;
			LSA_INT native_error;

			/* note: remote abort detection will not work if socket has unread data */

			SockIF_Recv( soc->sock_fd, &buf, sizeof(buf), LSA_TRUE, &result, &native_error);

			if( result == SOCKIF_SOCKET_ERROR /* RST */
				|| result == 0 /* FIN */
				)
			{
				sock_socket_return_all_resources(soc, SOCK_RSP_ERR_REM_ABORT);
				return;
			}
		}

		if (sock_is_not_null(soc->recv.curr))
		{
			SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
				, "socket_fd(0x%x) in read mask"
				, soc->sock_fd
				);

			sock_socket_receive(soc);
		}
		

#if SOCK_CFG_SNMP_ON
	else if(sock_are_equal(soc, sock_data.snmp_dev))  /* snmp device: read snmp request and dispatch request to snmp agent */
	{
		LSA_INT32 bytes_read = 0;

		LSA_RESPONSE_TYPE result = sock_udp_read(&bytes_read, soc, sock_data.snmp_recv_buf, SOCK_CFG_SNMP_BUFFER_LEN, LSA_FALSE, &soc->rem_addr);

		if( result == SOCK_RSP_OK)
		{
			if( bytes_read > 0 )
			{
				sock_data.snmp_pending_count = 1;

				sock_snmp_dispatch_request( sock_data.snmp_recv_buf, bytes_read );
			}
		}
		else
		{
			SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "error reading snmp socket, result(%d)", result);
		}
	}
#endif
}
}


/*------------------------------------------------------------------------------
// sock_socket_handle_write_mask():
// handle situation if socket is in write mask after select call
//----------------------------------------------------------------------------*/
static LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_handle_write_mask(
	SOCK_SOCKET_PTR soc
){
	SOCK_LOG_FCT("sock_socket_handle_write_mask")

	if( sock_is_not_null(soc->connect_rb) )
	{
		LSA_INT sock_error;
		LSA_INT native_error;

		SockIF_Get_Socket_Error(soc->sock_fd, &sock_error, &native_error); /* check error condition on socket also, see AP01418351, AP01493449 */

		if( sock_error )
		{
			sock_socket_handle_connect_error(soc, sock_error);
		}
		else
		{
			sock_tcp_connect_client_socket(soc); /* conclude connect */
		}
	}
	else
	{
		SOCK_PROTOCOL_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "pending writes on sock_fd(0x%x), dev_handle(%d)"
			, soc->sock_fd, soc->dev_handle 
			);

		sock_socket_send(soc);

		/***/

		if( sock_is_not_null(soc->open_close_rb) /* pending close active */
			&& sock_is_null(soc->send.curr)
			)
		{
			sock_socket_close(soc);
		}
	}
}


/*------------------------------------------------------------------------------
// sock_socket_close
// close rb present - return if possible
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_close(
SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_close")

#if SOCK_CFG_ENABLE_MULTICASTS
	if ( soc->mcast_interface_id != 0 ) /* release mcast resource */
	{
		LSA_RESPONSE_TYPE resp = tcip_srv_multicast(soc->dev_handle
							, soc->mcast_interface_id
							, soc->mcast_address
							, TCIP_SRV_MULTICAST_DISABLE
							); 
					
		if( resp != TCIP_OK )
		{
			SOCK_FATAL(); /* must not fail */
		}

		soc->state = SOCK_CN_STATE_DISABLED; /* (!) must not use this socket any more */
	}
	else
#endif
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "socket handle(%d) closed by application"
			, soc->dev_handle
			);

		if( soc->state != SOCK_CN_STATE_DISCONNECTING ) /* if not pending close */
		{
			sock_socket_return_all_resources(soc, SOCK_RSP_ERR_LOC_ABORT);
		}

		sock_usr_callback(soc->channel, SOCK_RSP_OK, &soc->open_close_rb);

		sock_socket_free(soc);
	}
}


/*------------------------------------------------------------------------------
// sock_socket_receive() :called if
// (1) resource was provided by user (receive request) or (!)
// (2) when select returns and resource is available
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_receive(
	SOCK_SOCKET_PTR soc
) {
	while( sock_is_not_null(soc->recv.curr) )
	{
		LSA_RESPONSE_TYPE read_response;

		if( soc->recv.curr->args.data.data_length == 0 ) /* 0 byte read always ok */
		{
			read_response = SOCK_RSP_OK;
		}
		else if( soc->type == SOCKIF_DGRAM )
		{
			read_response = sock_udp_receive(soc, &(soc->recv.curr->args.data));
		}
		else
		{
			read_response = sock_tcp_receive(soc, &(soc->recv.curr->args.data));
		}

		if( read_response == SOCK_RSP_OK ) /* return rqb and look for next in queue */
		{
			soc->recv.bytes_transferred = 0; /* reset when rqb returns */

			sock_usr_callback(soc->channel, read_response, &soc->recv.curr); /* soc->recv.curr <- LSA_NULL */

			soc->recv.curr = sock_queue_dequeue(&soc->recv.queue);

		}
		else if( read_response == SOCK_RSP_OK_ACTIVE ) /*  would block */
		{
			break;
		}
		else /* return rqb and all resources with error (TCP only) */
		{
			sock_socket_return_all_resources(soc, read_response); /* sets socket to SOCK_CN_STATE_DISCONNECTING */

			break; /* read error on socket -> no reading from this socket */
		}
	}
}


/*------------------------------------------------------------------------------
// called when
//	(1) send rqb was provided or
//  (2) socket is ready to write after select and send rqb is available
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_send(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_send")

	while( sock_is_not_null(soc->send.curr) )
	{
		LSA_RESPONSE_TYPE send_response;

		if( soc->type == SOCKIF_DGRAM )
		{
			send_response = sock_udp_send(soc, &(soc->send.curr->args.data));
		}
		else /* tcp */
		{
			if( soc->send.curr->args.data.data_length == 0 ) /* 0 byte write always ok */
			{
				send_response = SOCK_RSP_OK;
			}
			else
			{
				send_response = sock_tcp_send(soc, &(soc->send.curr->args.data));

				if (send_response != SOCK_RSP_OK && send_response != SOCK_RSP_OK_ACTIVE) /* send error on tcp socket -> no sending with this socket */
				{
					sock_socket_return_all_resources(soc, send_response); /* sets socket to SOCK_CN_STATE_DISCONNECTING */

					break;
				}
			}
		}

		if( send_response == SOCK_RSP_OK_ACTIVE ) /* would block */
		{
			break;
		}

		/* return rqb and look for next in queue */

		soc->send.bytes_transferred = 0; /* reset when rqb returns */

		sock_usr_callback(soc->channel, send_response, &soc->send.curr); /* (!) sets soc->send.curr to LSA_NULL */

		soc->send.curr = sock_queue_dequeue(&soc->send.queue);
	}
}


/*------------------------------------------------------------------------------
// set socket struct state to SOCK_CN_STATE_DISCONNECTING
// and pass back all user resources
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_return_all_resources(
	SOCK_SOCKET_PTR dev,
	LSA_UINT16 abort_resp
) {
	SOCK_LOG_FCT("sock_socket_return_all_resources")

	dev->abort_resp = abort_resp; /* used for response if rqb arrives for this socket  */

	dev->send.bytes_transferred = 0;
	dev->recv.bytes_transferred = 0;

	dev->state = SOCK_CN_STATE_DISCONNECTING; /* socket is about to close (user must send SOCK_OPC_CLOSE) and has no resources */


#if SOCK_CFG_SNMP_ON

	if (sock_are_equal(dev, sock_data.snmp_dev))
	{
		if (sock_data.snmp_extern_count == 0)
		{
			sock_snmp_deactivate_int();
		}
		else
		{
			sock_data.snmp_intern_auto = LSA_TRUE;
		}
		return;
	}

#endif

	/* pass back all receive rqbs */
	sock_socket_return_receive_resources(dev, abort_resp);

	/* pass back all send rqbs */
	sock_socket_return_send_resources(dev, abort_resp);

	/* pass back connect rqb */
	sock_socket_return_connect_resource(dev, abort_resp);

	/* pass back accept notify rqbs */
	sock_socket_return_accept_notify_resources(dev, abort_resp);

	/* pass back except_notify rqb */
	sock_socket_return_exception_resource(dev, abort_resp);

	return;
}


/*------------------------------------------------------------------------------
// return send resources if present
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_return_send_resources(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_send_resources")

	if( sock_is_not_null(soc->send.curr))
	{
		SOCK_UPPER_RQB_PTR_TYPE rb;

		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
			, "confirm send rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->send.curr, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->send.curr);

		rb = sock_queue_dequeue(&soc->send.queue);

		while ( sock_is_not_null(rb) )
		{
			SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
				, "confirm send rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
				, rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
				);

			sock_usr_callback(soc->channel, response, &rb);

			rb = sock_queue_dequeue(&soc->send.queue);
		}
	}
}


/*------------------------------------------------------------------------------
// return receive resources if present
//----------------------------------------------------------------------------*/
LSA_INT SOCK_LOCAL_FCT_ATTR
sock_socket_return_receive_resources(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_receive_resources")

	LSA_INT num = 0;

	if(sock_is_not_null(soc->recv.curr))
	{
		SOCK_UPPER_RQB_PTR_TYPE rb;

		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW
			, "confirm receive rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->recv.curr, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->recv.curr);

		num++;

		rb = sock_queue_dequeue(&soc->recv.queue);

		while ( sock_is_not_null(rb) )
		{
			SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW
				, "confirm receive rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
				, rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
				);

			sock_usr_callback(soc->channel, response, &rb);

			num++;

			rb = sock_queue_dequeue(&soc->recv.queue);
		}
	}

	return num;
}


/*------------------------------------------------------------------------------
// return connect resource if present
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_return_connect_resource(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_connect_resource")

	if ( sock_is_not_null(soc->connect_rb) ) /* pass back connect_rb if present */
	{
		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
			, "SOCK_OPC_CONNECT(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->connect_rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->connect_rb);
	}
}


/*------------------------------------------------------------------------------
// return accept notify resource if present
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_return_accept_notify_resources(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	if(sock_is_not_null(soc->accept.curr))
	{
		SOCK_UPPER_RQB_PTR_TYPE rb;

		sock_usr_callback(soc->channel, response, &soc->accept.curr);

		rb = sock_queue_dequeue(&soc->accept.queue);

		while ( sock_is_not_null(rb) ) /* return pending SOCK_OPC_ACCEPT_NOTIFY */
		{
			sock_usr_callback(soc->channel, response, &rb);

			rb = sock_queue_dequeue(&soc->accept.queue);
		}
	}
}


/*------------------------------------------------------------------------------
// return except resource if present
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_socket_return_exception_resource(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_exception_resource")

	if ( sock_is_not_null(soc->exception_rb) )
	{
		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW
			, "SOCK_OPC_EXCEPT_NOTIFY(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->exception_rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->exception_rb);
	}
}


/*------------------------------------------------------------------------------
// Sets the socket options for the specified open socket descriptor according
// to details pointers to send_option_val, rcv_option_val, udp may be NULL
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_socket_set_options(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCKIF_FD fd,
	LSA_INT32 type
) {
	SOCK_LOG_FCT("sock_socket_set_options")

	LSA_INT32 option_val;
	SockIF_Linger linger_option;
	LSA_INT native_error;

	/* switch socket to non-blocking mode */

	option_val = 1;
	if (!SockIF_SetSockOpt(fd, SockOpt_NonBlocking, &option_val, &native_error))
	{
		SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_NonBlocking, option_val(%d), native_error(%d)"
							, fd, option_val, native_error
							);

		return SOCK_RSP_ERR_LOC_NET;
	}

	if ( (type == SOCKIF_STREAM) && channel->detail_ptr->socket_option.SO_TCPNODELAY_) /* don't delay sending data */
	{
		option_val = 1;
		if (!SockIF_SetSockOpt(fd, SockOpt_TCPNoDelay, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_TCPNoDelay, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if ( (type == SOCKIF_STREAM) && channel->detail_ptr->socket_option.SO_TCPNOACKDELAY_) /* don't delay sending ACKs */
	{
		option_val = 1;
		if (!SockIF_SetSockOpt(fd, SockOpt_TCPNoAckDelay, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_TCPNoAckDelay, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_REUSEADDR_)
	{
		if (!SockIF_SetSockOpt(fd, SockOpt_ReuseAddr, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_ReuseAddr, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (type == SOCKIF_STREAM) /* set keep alive as requested (in all cases!) AP00938830 */
	{
		if(SOCK_KEEPALIVE_ACTIVE/* == LSA_TRUE */)
		{
			option_val = 1;
		}
		else
		{
			option_val = 0;
		}

		if (!SockIF_SetSockOpt(fd, SockOpt_KeepAlive, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_KeepAlive, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}

		option_val = 1;
		if (!SockIF_SetSockOpt(fd, SockOpt_OOBInline, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_OOBInline, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	option_val = 1;
	if (channel->detail_ptr->socket_option.SO_DONTROUTE_)
	{
		if (!SockIF_SetSockOpt(fd, SockOpt_DontRoute, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_DontRoute, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_SNDBUF_)
	{
		option_val = channel->detail_ptr->send_buffer_max_len;
		if (!SockIF_SetSockOpt(fd, SockOpt_SendBuffer, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_SendBuffer, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_RCVBUF_)
	{
		option_val =  channel->detail_ptr->rec_buffer_max_len;
		if (!SockIF_SetSockOpt(fd, SockOpt_RecvBuffer, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_RecvBuffer, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_LINGER_)
	{
		linger_option.l_linger =  channel->detail_ptr->linger_time;
		linger_option.l_onoff = 1;
		if (!SockIF_SetSockOpt(fd, SockOpt_Linger, &linger_option, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_Linger, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

#if defined SIO_UDP_CONNRESET
	#if SOCK_CFG_WINSOCK  /* check if option available */
	/*
	Windows XP: Controls whether UDP PORT_UNREACHABLE messages are reported. Set to TRUE to enable reporting. Set to FALSE to disable reporting.
	SIO_UDP_CONNRESET
	*/
	if (type == SOCKIF_DGRAM)
	{
		option_val = 0; /* LSA_FALSE */
		if (!SockIF_SetSockOpt(fd, SockOpt_UDP_Connreset, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error SockIF_SetSockOpt(fd(%d), option=SockOpt_UDP_Connreset, option_val(%d), native_error(%d)"
								, fd, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}
	#endif
#endif

	return SOCK_RSP_OK;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
