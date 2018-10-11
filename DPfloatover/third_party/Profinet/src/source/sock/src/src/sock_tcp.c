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
/*  F i l e               &F: sock_tcp.c                                :F&  */
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

#define LTRC_ACT_MODUL_ID 5
#define SOCK_MODULE_ID    5

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*------------------------------------------------------------------------------
// SOCK_OPC_LISTEN: create listening server socket at passed address/port
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_tcp_listen(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_listen")

	SOCKIF_FD       fd;
	int	native_error;
	LSA_RESPONSE_TYPE response;
	SOCK_RQB_ARGS_LISTEN_PTR_TYPE listen;

	listen = & rqb->args.listen;

	if ((fd = SockIF_Socket(SOCKIF_STREAM)) == SOCKIF_INVALID_FD)
	{
		response = SOCK_RSP_ERR_RESOURCE;
		
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
			, "TCP socket open failed, response(%d)"
			, response
			);
	}
	else
	{
		SOCK_SOCKET_PTR soc = LSA_NULL;

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "TCP socket open ok, fd(0x%x)"
			, fd
			);

		soc = sock_socket_alloc(channel, fd, SOCKIF_STREAM);

		if ( sock_is_null(soc) )
		{
			response = SOCK_RSP_ERR_RESOURCE;

			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "socket struct alloc failed, fd(0x%x), response(%d)"
				, fd, response
				);
		}
		else
		{
			LSA_INT16       backlog;
			SockIF_AddrIn   *loc_addr_p;

			loc_addr_p = &listen->sockaddr;

			response = SockIF_Bind(soc->sock_fd, loc_addr_p, &native_error);

			if (response != SOCK_RSP_OK)
			{
				SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
					, "bind() failed for TCP fd(0x%x), native_error(%d) response(%d)"
					, soc->sock_fd, native_error, response
					);

				sock_socket_free(soc);
			}
			else
			{
				SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "bind() ok for TCP fd(0x%x)"
					, soc->sock_fd
					);

				soc->rem_addr.sin_family = SOCKIF_AF_INET;

				response = SockIF_GetSockName(soc->sock_fd, &listen->sockaddr, &native_error);

				if (response != SOCK_RSP_OK)
				{
					SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "getsockname() failed for fd(0x%x), native_error(%d), resonse(%d)"
							, soc->sock_fd, native_error, response
							);

						sock_socket_free(soc);
				}
				else
				{
					listen->sock_fd = soc->dev_handle;     /* return internal handle (not socket descriptor!) */

					soc->loc_addr = listen->sockaddr;

					backlog = listen->backlog;

					response = SockIF_Listen(fd, backlog, &native_error);

					if (response != SOCK_RSP_OK)
					{
						SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
							, "listen() failed for fd(0x%x), native_error(%d), resonse(%d)"
							, soc->sock_fd, native_error, response
							);

						sock_socket_free(soc);
					}
					else
					{
						SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
							, "fd: 0x%x SRV LISTEN   OK"
							, fd
							);

						soc->state = SOCK_CN_STATE_LISTENING;

						SOCK_PROTOCOL_TRACE_03(0,LSA_TRACE_LEVEL_NOTE
							, "listening socket 0x%x for own Port %d with internal ConnectionStructure %d created "
							, soc->sock_fd, SOCK_HTONS(soc->loc_addr.sin_port), soc->dev_handle
							);
					}
				}
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// activate select for listening socket
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_tcp_enable_accept(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_tcp_enable_accept")

	SOCK_PROTOCOL_TRACE_03(0, LSA_TRACE_LEVEL_NOTE
		, "wait for connects at sockets fd(0x%x), Port(%d), socket handle(%d)"
		, soc->sock_fd, SOCK_NTOHS(soc->loc_addr.sin_port), soc->dev_handle
		);

	if (!sock_data.sel_status)
	{
		sock_data.sel_status = LSA_TRUE;
		SOCK_START_SELECT();
	}

	return;
}


/*------------------------------------------------------------------------------
// create new connection, return rqb
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_tcp_accept(
	SOCK_SOCKET_PTR listening_soc
) {
	SOCK_LOG_FCT("sock_tcp_accept")

	LSA_INT native_error;
	SOCKIF_FD  new_fd;
	SOCK_SOCKET_PTR new_soc;
	SockIF_AddrIn new_addr;
	LSA_RESPONSE_TYPE response;

	if (sock_is_null(listening_soc->accept.curr))
	{
		SOCK_FATAL();
	}

	new_fd = SockIF_Accept(listening_soc->sock_fd, &new_addr, &native_error);

	if (new_fd == SOCKIF_INVALID_FD)
	{
		LSA_INT option_val;

		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
			, "accept() failed on handle(%d), sock_fd(0x%x), native_error(%d), n o t returning accept rqb"
			, listening_soc->dev_handle, listening_soc->sock_fd, native_error
			);

		SockIF_Get_Socket_Error(listening_soc->sock_fd, &option_val, &native_error);  /* eliminate error condtion */

		/* accept notify rqb stays active */
	}
	else
	{
		/* check returned address info and available resources */
		if (new_addr.sin_family != SOCKIF_AF_INET)
		{
			SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "address family not AF_INET for sock_fd(0x%x) CLT ACCEPT   ERR (invalid address)"
				, new_fd
				);

			SockIF_Close(new_fd); /* accept notify rqb stays active */
		}
		else
		{
			SockIF_AddrIn loc_addr;

			response = SockIF_GetSockName(new_fd, &loc_addr, &native_error); /* get local address */

			if (response != SOCK_RSP_OK)
			{
				SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "getsockname failed for sock_fd(0x%X), response(%d), native_error(%d), close socket"
							, new_fd, response, native_error
							);

					SockIF_Close(new_fd); /* accept notify rqb stays active */
			}
			else
			{
				int i;

				/* search InterfaceID in sock_data */
				for(i=0; i<SOCK_MAX_INTERFACE_ID;i++)
				{
					if (sock_data.addr_info[i].CurrentIPSuite.IpAddr.s_addr == loc_addr.sin_addr.s_addr)
					{
						break; /* found */
					}
				}

				if (i == SOCK_MAX_INTERFACE_ID) /* not found -> close socket */
				{
					SOCK_LOWER_TRACE_05(0, LSA_TRACE_LEVEL_ERROR
							, "address(%d.%d.%d.%d) of socket fd(0x%x) not found in sock_data, close socket"
							, loc_addr.sin_addr.S_un.S_un_b.s_b1, loc_addr.sin_addr.S_un.S_un_b.s_b2, loc_addr.sin_addr.S_un.S_un_b.s_b3, loc_addr.sin_addr.S_un.S_un_b.s_b4
							, new_fd
							);

					SockIF_Close(new_fd); /* accept notify rqb stays active */
				}
				else
				{
					new_soc = sock_socket_alloc(listening_soc->channel, new_fd, SOCKIF_STREAM); /* closes new_fd if not successful */

					if (sock_is_null(new_soc))
					{
						SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
							, "socket struct allocation failed for new fd(0x%x), socket closed"
							, new_fd
							);

						/* accept notify rqb stays active */
					}
					else
					{
						SockIF_IPAddr* p_addr;

						SOCK_RQB_ARGS_CONN_PTR_TYPE conn = & (listening_soc->accept.curr->args.conn);

						new_soc->rem_addr = new_addr;
						new_soc->loc_addr = loc_addr;

						new_soc->state = SOCK_CN_STATE_CONNECTED;

						conn->InterfaceID = sock_data.addr_info[i].InterfaceID;

						conn->loc_addr = new_soc->loc_addr;
						conn->rem_addr = new_soc->rem_addr;
						conn->sock_fd = new_soc->dev_handle;

						SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
							, "new connection established, handle(%d)"
							, new_soc->dev_handle
							);

						p_addr = &new_soc->loc_addr.sin_addr;

						SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
							, "local IP(%d.%d.%d.%d), local Port(%d)"
							, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
							, SOCK_HTONS(new_soc->loc_addr.sin_port)
							);

						p_addr = &new_soc->rem_addr.sin_addr;

						SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
							,"remote IP(%d.%d.%d.%d), remote Port(%d)"
							, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
							, SOCK_HTONS(new_soc->rem_addr.sin_port)
							);

						sock_usr_callback(listening_soc->channel, SOCK_RSP_OK, &listening_soc->accept.curr); /* sets accept.curr to LSA_NULL */

						listening_soc->accept.curr = sock_queue_dequeue(&listening_soc->accept.queue);

						if(sock_is_not_null(listening_soc->accept.curr))
						{
							sock_tcp_enable_accept(listening_soc);
						}
					}
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
// SOCK_OPC_OPEN: open socket and allocate socket struct
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_tcp_open(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_open")

	SOCK_SOCKET_PTR soc;
	SOCKIF_FD fd;
	LSA_RESPONSE_TYPE response;

	soc = LSA_NULL;

	if ((fd = SockIF_Socket(SOCKIF_STREAM)) == SOCKIF_INVALID_FD)
	{
		response = SOCK_RSP_ERR_RESOURCE; /* not: SOCK_RSP_ERR_LOWER_LAYER (AP01479148) */
		
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
			, "TCP socket open failed, response(%d)"
			, response
			);
	}
	else
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "TCP socket open ok, fd(0x%x)"
			, fd
			);

		soc = sock_socket_alloc(channel, fd, SOCKIF_STREAM);

		if (sock_is_null(soc) )
		{
			response = SOCK_RSP_ERR_RESOURCE;

			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "socket struct allocation failed for new fd(0x%x), response(%d)"
				, fd, response
				);
		}
		else
		{
			rqb->args.open.sock_fd = soc->dev_handle;

			soc->state = SOCK_CN_STATE_OPENED;

			response = SOCK_RSP_OK;
		}
	}
	return response;
}


/*------------------------------------------------------------------------------
// bind a port number to the socket and initiate TCP connection
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_tcp_connect_with_open(
	SOCK_SOCKET_PTR soc,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_connect_with_open")

	LSA_INT native_error;
	SockIF_AddrIn   *rem_addr_p, *loc_addr_p;
	LSA_RESPONSE_TYPE response;
	SOCK_RQB_ARGS_CONNECT_WITH_OPEN_PTR_TYPE con;

	response = SOCK_RSP_OK_ACTIVE; /* (!)default return value */

	con = &rqb->args.connect_with_open; 

	soc->state = SOCK_CN_STATE_CONNECTING;

	soc->loc_addr.sin_family = SOCKIF_AF_INET;
	soc->loc_addr.sin_port = con->loc_addr.sin_port;
	soc->loc_addr.sin_addr.S_un.S_addr = con->loc_addr.sin_addr.S_un.S_addr;

	loc_addr_p=&soc->loc_addr;   /* avoid warning of Cadul's CXX V401S */

	response = SockIF_Bind(soc->sock_fd, loc_addr_p, &native_error);

	if (response != SOCK_RSP_OK)
	{
		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
			, "bind() failed for TCP socket fd(0x%x), native_error(%d), response(%d)"
			, soc->sock_fd, native_error, response
			);
	}
	else
	{
		response = SockIF_GetSockName(soc->sock_fd, &con->loc_addr, &native_error);

		if (response != SOCK_RSP_OK)
		{
			SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
				, "getsockname() failed for TCP socket fd(0x%x), native_error(%d), response(%d)"
				, soc->sock_fd, native_error, response
				);
		}
		else
		{
			soc->loc_addr.sin_family = SOCKIF_AF_INET;
			soc->loc_addr.sin_port = con->loc_addr.sin_port;
			soc->loc_addr.sin_addr.S_un.S_addr = con->loc_addr.sin_addr.S_un.S_addr;

			soc->rem_addr.sin_family = SOCKIF_AF_INET;
			soc->rem_addr.sin_port = con->rem_addr.sin_port;
			soc->rem_addr.sin_addr.S_un.S_addr = con->rem_addr.sin_addr.S_un.S_addr;

			rem_addr_p=&soc->rem_addr; /* avoid warning of Cadul's CXX V401S */

			response = SockIF_Connect(soc->sock_fd, rem_addr_p, &native_error);

			if (response != SOCK_RSP_OK)
			{
				SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "fd: 0x%x CLT CONNECT  pending..."
					, soc->sock_fd
					);

				if ((response == SOCK_RSP_OK_PROGRESS) || (response == SOCK_RSP_OK_ACTIVE))  /* wait for accept by server */
				{
					SockIF_IPAddr *p_addr;

					soc->connect_rb = rqb;

					p_addr = & soc->rem_addr.sin_addr;

					SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
						, "SOCK_OPC_CONNECT to remote IP %d.%d.%d.%d at rem_Port %d pending -> socket goes into write mask"
						, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
						, SOCK_HTONS(soc->rem_addr.sin_port)
						);

					if (!sock_data.sel_status)
					{
						sock_data.sel_status = LSA_TRUE;
						SOCK_START_SELECT();    /* Initiation for select-Mimic */
					}
				
					response = SOCK_RSP_OK_ACTIVE; /* map 'in progress' */
				}
				else /* return mapped response code */
				{
					SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
						, "connect() failed for fd(0x%x), native_error(%d), response(%d)"
						, soc->sock_fd, native_error, response
						);
				}
			}
			else /* server accept */
			{
				soc->connect_rb = rqb;

				sock_tcp_connect_client_socket(soc);     /* obtain address and return rqb */

				if (!sock_data.sel_status)
				{
					sock_data.sel_status = LSA_TRUE;
					SOCK_START_SELECT();
				}

				response = SOCK_RSP_OK_ACTIVE; /* (!) not SOCK_RSP_OK, see 1160210 */
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CONNECT:bind a port number to the socket and initiate TCP connection
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_tcp_connect(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_connect")

	SOCKIF_FD fd;
	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR soc = LSA_NULL;

	fd = SockIF_Socket(SOCKIF_STREAM);

	if (fd == SOCKIF_INVALID_FD)
	{
		response = SOCK_RSP_ERR_RESOURCE; /* not: SOCK_RSP_ERR_LOWER_LAYER (AP01479148) */

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
			, "TCP socket open failed, response(%d)"
			, response
			);
	}
	else
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "TCP socket open ok, fd(0x%x)"
			, fd
			);

		soc = sock_socket_alloc(channel, fd, SOCKIF_STREAM);

		if ( sock_is_null(soc) )
		{
			response = SOCK_RSP_ERR_RESOURCE;
			
			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "socket struct alloc failed, fd(0x%x), response(%d)"
				, fd, response
				);
		}
		else
		{
			response = sock_tcp_connect_with_open(soc, rqb);

			if( response != SOCK_RSP_OK_ACTIVE)
			{
				sock_socket_free(soc); /* possible since user has no handle yet */
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// return connect request of client socket
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_tcp_connect_client_socket(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_tcp_connect_client_socket")

	SockIF_IPAddr *p_addr;
	SOCK_RQB_ARGS_CONN_PTR_TYPE conn;
	LSA_INT native_error;
	LSA_RESPONSE_TYPE response;

	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
		, "connect() ok for fd(0x%x)"
		, soc->sock_fd
		);

	soc->state = SOCK_CN_STATE_CONNECTED;

	conn = &soc->connect_rb->args.conn;

	response = SockIF_GetSockName(soc->sock_fd, &soc->loc_addr, &native_error);  /* get local address */

	if (response != SOCK_RSP_OK)
	{
		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
			, "getsockname() failed for TCP socket fd(0x%x), native_error(%d), response(%d)"
			, soc->sock_fd, native_error, response
			);
	}
	else
	{
		conn->loc_addr = soc->loc_addr;

		conn->sock_fd = soc->dev_handle;

		SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "tcp connection %d established "
			, soc->dev_handle
			);

		p_addr = &soc->loc_addr.sin_addr;

		SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
			, "local IP(%d.%d.%d.%d), local Port(%d)"
			, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
			, SOCK_HTONS(soc->loc_addr.sin_port)
			);

		p_addr = &soc->rem_addr.sin_addr;

		SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
			,"remote IP(%d.%d.%d.%d), remote Port(%d)"
			, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
			, SOCK_HTONS(soc->rem_addr.sin_port)
			);
	}

	sock_usr_callback(soc->channel, response, &soc->connect_rb);

	return;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CANCEL_CONNECTS
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_tcp_connect_cancel(
	SOCK_CHANNEL_PTR_TYPE channel
) {
	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR soc;
	LSA_INT i;

	response = SOCK_RSP_ERR_RESOURCE;

	for (i=0; i<SOCK_MAX_SOCKETS; i++ )
	{
		if( SOCK_CN_STATE_FREE == sock_data.sockets[i].state )
		{
			continue;
		}

		soc = & sock_data.sockets[i];

		if( sock_are_equal(channel, soc->channel)
			&& soc->state == SOCK_CN_STATE_CONNECTING
			)
		{
			sock_socket_return_connect_resource(soc, SOCK_RSP_ERR_LOC_ABORT);

			response = SOCK_RSP_OK;
		}
	}
	return response;
}


/*------------------------------------------------------------------------------
// read from tcp socket
//
// returns	SOCK_RSP_OK_ACTIVE:		put socket into select set
//			SOCK_RSP_ERR_REM_ABORT: disable socket
//			SOCK_RSP_ERR_RESOURCE:  disable socket
//			SOCK_RESP_ERR_LOC_NET:  disable socket
//          SOCK_RSP_OK:			send success, return rqb
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_tcp_receive(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_tcp_receive")

	LSA_BOOL peek = (data->flags & SOCK_PEEK) ? LSA_TRUE : LSA_FALSE;

	LSA_RESPONSE_TYPE response;

	data->rem_addr = soc->rem_addr;

	for(;;)
	{
		LSA_INT native_error;
		LSA_INT32 bytes_to_read = data->data_length - soc->recv.bytes_transferred;
		LSA_INT32 bytes_offset  = data->offset      + soc->recv.bytes_transferred;
		LSA_INT32 bytes_read;

		response = SockIF_Recv( soc->sock_fd
								, (char*)data->buffer_ptr + bytes_offset
								, bytes_to_read
								, peek
								, &bytes_read
								, &native_error 
								);

		if( response != SOCK_RSP_OK ) /* failed */
		{
			if( response == SOCK_RSP_OK_ACTIVE ) /* would block -> attempt read in next select call */
			{
				SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
				, "socket handle(%d) stays in read mask"
				, soc->dev_handle
				);

				break; /* wait for next select cycle */
			}
			else /* tcp socket error */
			{
				SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
					, "SockIF_Recv on fd(%d) failed, native_error(%d), response(%dx)"
					, soc->sock_fd, native_error, response
					);

				data->data_length = (LSA_UINT16)soc->recv.bytes_transferred; /* bytes received so far */

				break;
			}
		}
		else /* SOCK_RSP_OK */
		{
			if( bytes_read == 0 ) /* connection was closed by peer */
			{
				SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
					, "SockIF_Recv returned 0 on tcp socket handle(%d)"
					, soc->dev_handle
					);

				data->data_length = (LSA_UINT16)soc->recv.bytes_transferred; /* bytes received so far */

					response = SOCK_RSP_ERR_REM_ABORT;

				break;
			}
			else /* bytes_read > 0 */
			{
				soc->recv.bytes_transferred += bytes_read;

				if( soc->recv.bytes_transferred > data->data_length )
				{
						sock_fatal(SOCK_MODULE_ID, SOCK__LINE__); /* unreachable */
				}
				else if( (data->flags & SOCK_READ_ALL) != 0 )
				{
					if( soc->recv.bytes_transferred == data->data_length )
					{
						SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
							, "socket handle(%d) %d bytes received"
							, soc->dev_handle, data->data_length
							);

						data->data_length = (LSA_UINT16)soc->recv.bytes_transferred; /* AP01228536 */

						break;
					}
					else
					{
						continue; /* more data needed */
					}
				}
				else /* not SOCK_READ_ALL */
				{
					SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
						, "socket handle(%d) %d bytes received -without flag SOCK_READ_ALL-"
						, soc->dev_handle, bytes_read
						);

					data->data_length = (LSA_UINT16)soc->recv.bytes_transferred;

					break;
				}
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// send to tcp socket
//
// returns	SOCK_RSP_OK_ACTIVE:		put socket into select set
//			SOCK_RSP_ERR_REM_ABORT: disable socket
//			SOCK_RSP_ERR_RESOURCE:  disable socket
//			SOCK_RESP_ERR_LOC_NET:  disable socket
//          SOCK_RSP_OK:			send success, return rqb
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_tcp_send(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_tcp_send")

	LSA_INT native_error;
	LSA_INT32 bytes_to_write = data->data_length - soc->send.bytes_transferred;
	LSA_INT32 bytes_offset   = data->offset      + soc->send.bytes_transferred;
	LSA_INT32 bytes_written;

	LSA_RESPONSE_TYPE response = SockIF_Send( soc->sock_fd
											  , (char *)data->buffer_ptr + bytes_offset
											  , bytes_to_write
											  , &bytes_written
											  , &native_error
											  );

	if( response != SOCK_RSP_OK ) /* failed */
	{
		if( response == SOCK_RSP_OK_ACTIVE ) /* would block -> put socket into select set */
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
				, "pending send on socket handle(%d), data_length(%d) to send, send returned SockErr_WouldBlock, bytes_transferred(%d) so far"
				, soc->dev_handle, data->data_length, soc->send.bytes_transferred
				);
		}
		else
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
				, "SockIF_Send failed on dev_handle(%d), native_error(%d), response(%d)"
				, soc->dev_handle, native_error, response
				);

			data->data_length = (LSA_UINT16)soc->send.bytes_transferred; /* return actually written data to user */
		}
	}
	else /* SOCK_RSP_OK -> bytes_written >= 0 */
	{
		soc->send.bytes_transferred += bytes_written;

		if (soc->send.bytes_transferred == data->data_length)
		{
			SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "socket handle(%d): send returned %d, return rqb"
			, soc->dev_handle, soc->send.bytes_transferred
			);

		}
		else
		{
			if( soc->send.bytes_transferred < data->data_length ) /* treats bytes_written == 0 also, see AP01233761 */
			{
				SOCK_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
					, "pending send on socket handle(%d), data_length(%d) to send, send returned %d, bytes_transferred(%d) so far"
					, soc->dev_handle, data->data_length, bytes_written, soc->send.bytes_transferred
					);

				response = SOCK_RSP_OK_ACTIVE; /* would block -> put socket into select set */
			}
			else  /* soc->send.bytes_transferred > data->data_length:  written more than requested */
			{
				sock_fatal(SOCK_MODULE_ID, SOCK__LINE__);
			}
		}
	}

	return response;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
