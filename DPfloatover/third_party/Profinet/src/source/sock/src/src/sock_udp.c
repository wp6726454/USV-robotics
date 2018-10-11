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
/*  F i l e               &F: sock_udp.c                                :F&  */
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

#define LTRC_ACT_MODUL_ID 1
#define SOCK_MODULE_ID    1

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*------------------------------------------------------------------------------
// sock_udp_open_socket: opens udp socket and binds to loc_addr
//----------------------------------------------------------------------------*/
static LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_udp_open_socket(
	SOCK_CHANNEL_PTR_TYPE channel,
	SockIF_AddrIn* loc_addr,
	SOCK_SOCKET_PTR* soc_ptr
) {
	SOCK_LOG_FCT("sock_udp_open_socket")

	LSA_RESPONSE_TYPE response;
	
	SOCKIF_FD fd = SockIF_Socket(SOCKIF_DGRAM);
	
	if (fd == SOCKIF_INVALID_FD)
	{
		SOCK_LOWER_TRACE_00(0, LSA_TRACE_LEVEL_WARN
			, "unable to create UDP socket"
			);
		response = SOCK_RSP_ERR_RESOURCE;
	}
	else
	{
		SOCK_SOCKET_PTR soc = sock_socket_alloc(channel, fd, SOCKIF_DGRAM);

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "fd: 0x%x UDP SOCKET   OK", fd);

		if ( sock_is_null(soc) )
		{
			SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "fd: 0x%x UDP DEVICE   ERR", fd);

			response = SOCK_RSP_ERR_RESOURCE;
		}
		else
		{
			int native_error;

			response = SockIF_Bind(soc->sock_fd, loc_addr, &native_error);
			
			if (response != SOCK_RSP_OK) /* handle bind error (AP00604854) */
			{		
				SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
					, "bind() failed for UDP socket fd(0x%x), native_error(%d), response(%d)"
					, soc->sock_fd, native_error, response
					);

				sock_socket_free(soc);
			}
			else
			{
				SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "fd: 0x%x UDP BIND OK", fd);

				response = SockIF_GetSockName(soc->sock_fd, loc_addr, &native_error);

				if (response != SOCK_RSP_OK)
				{
					SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
						, "getsockname() failed for TCP socket fd(0x%x), native_error(%d), response(%d)"
						, soc->sock_fd, native_error, response
						);
				}
				else
				{
					soc->loc_addr = *loc_addr;

					*soc_ptr = soc;

					SOCK_PROTOCOL_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE
						, "udp socket 0x%x created, port(%d), dev_handle(%d)"
						, soc->sock_fd, SOCK_HTONS(soc->loc_addr.sin_port), soc->dev_handle
						);
				}
			}
		}
	}
	return response;
}

/*------------------------------------------------------------------------------
// SOCK_OPC_UDP_OPEN
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_udp_open(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_udp_open")

	SOCK_SOCKET_PTR soc;
	
	SOCK_RQB_ARGS_UDP_OPEN_PTR_TYPE open = &rqb->args.udp_open;

	LSA_RESPONSE_TYPE response = sock_udp_open_socket( channel
													   , &open->loc_addr
													   , &soc
													   );

	if( response == SOCK_RSP_OK )
	{
		soc->state = SOCK_CN_STATE_CONNECTED;

		open->sock_fd = soc->dev_handle;
	}

	return response;
}

/*------------------------------------------------------------------------------
// SOCK_OPC_UDP_MULTICAST_OPEN
//----------------------------------------------------------------------------*/
#if SOCK_CFG_ENABLE_MULTICASTS
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_udp_open_multicast(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_udp_open_multicast")

	LSA_UINT32 IfIndex = 0;
	SOCK_SOCKET_PTR soc = LSA_NULL;
	SOCK_RQB_ARGS_UDP_OPEN_MULTICAST_PTR_TYPE open_mc = & rqb->args.udp_open_multicast;
	LSA_RESPONSE_TYPE response = SOCK_RSP_OK;

	SOCK_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
		, "SOCK_OPC_UDP_MULTICAST_OPEN interface_id(%d) loc_port(%d) group_addr(0x%x) multicast_ttl(%d)"
		, open_mc->interface_id
		, open_mc->loc_port
		, SOCK_NTOHL(open_mc->group_addr.s_addr)
		, open_mc->multicast_ttl
		);

	if( open_mc->interface_id < 1
		|| open_mc->interface_id > 16
		|| open_mc->loc_port == 0
		|| open_mc->group_addr.s_addr == 0
		|| open_mc->multicast_ttl == 0
		)
	{
		response = SOCK_RSP_ERR_PARAM;
	}
	else
	{
		LSA_BOOL retval = tcip_get_mib2_ifindex(open_mc->interface_id, 0, &IfIndex);

		if(!retval)
		{
			response = SOCK_RSP_ERR_PARAM;
		}
		else
		{
			SockIF_AddrIn local_address;

			local_address.sin_addr.s_addr = 0; /* any */
			local_address.sin_port = open_mc->loc_port; /* (!) needed */
			local_address.sin_family = SOCK_AF_INET;

			response = sock_udp_open_socket(channel, &local_address, &soc);

			if (response != SOCK_RSP_OK)
			{
				SOCK_LOWER_TRACE_01 (0, LSA_TRACE_LEVEL_ERROR
					, "sock_udp_open_socket not successful, response(%d)"
					, response
					);
			}
			else
			{
				int native_error;

				response = SockIF_SetMulticastSockOpts(soc->sock_fd, IfIndex, open_mc->group_addr.s_addr, open_mc->multicast_ttl, &native_error);

				if (response != SOCK_RSP_OK)
				{
					sock_socket_free(soc);

					SOCK_LOWER_TRACE_05(0, LSA_TRACE_LEVEL_ERROR
						, "native errno(%d) when setting multicast socket options, dev_handle(%d), interface_id(%d), group_addr(0x%x), response(%d)"
						, native_error
						, soc->dev_handle
						, open_mc->interface_id
						, SOCK_NTOHL(open_mc->group_addr.s_addr)
						, response
						);
				}
				else
				{
					LSA_RESPONSE_TYPE ret = tcip_srv_multicast(soc->dev_handle
																, open_mc->interface_id
																, open_mc->group_addr.s_addr
																, TCIP_SRV_MULTICAST_ENABLE
																);

					if( ret != TCIP_OK )
					{
						SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "tcip_srv_multicast dev_handle(%d), interface_id(%d), group_addr(0x%x) failed"
							, soc->dev_handle
							, open_mc->interface_id
							, SOCK_NTOHL(open_mc->group_addr.s_addr)
							);

						sock_socket_free(soc);

						response = SOCK_RSP_ERR_PARAM;
					}
					else
					{
						soc->open_close_rb = rqb; /* store rqb, see TCIP_EDD_SRV_MULTICAST_DONE */

						response = SOCK_RSP_OK_ACTIVE;
					}
				}
			}
		}
	}

	return response;
}
#endif

/*------------------------------------------------------------------------------
// local function: reads data from udp socket
// returns:  SOCK_RSP_OK	   : data was successfully read from socket
//           SOCK_RSP_OK_ACTIVE: read was blocked
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_udp_read(
	LSA_INT32* bytes_read,
	SOCK_SOCKET_PTR soc,
	LSA_VOID* buffer,
	LSA_UINT32 length,
	LSA_BOOL peek,
	SockIF_AddrIn* address
) {
	SOCK_LOG_FCT("sock_udp_read")

	LSA_INT native_error;
	LSA_INT32 bytes_received;

	LSA_RESPONSE_TYPE  response = SockIF_RecvFrom( soc->sock_fd
												   , buffer
												   , length
												   , peek
												   , address
												   , &bytes_received
												   , &native_error
												   );

	if( response != SOCK_RSP_OK ) /* failed */
	{
		if( response != SOCK_RSP_OK_ACTIVE )
		{
			LSA_INT sock_option;
			LSA_INT option_error;

			SockIF_Get_Socket_Error( soc->sock_fd, &sock_option, &option_error); /* reset of so_error -> socket will be available again */

			SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
				, "SockIF_Get_Socket_Error on udp fd(0x%x) because recvfrom() returned native_error(%d), response(%d)"
				, soc->sock_fd, native_error, response
				);

			response = SOCK_RSP_OK_ACTIVE; /* would block -> wait for next select cycle */
		}
	}
	else /* SOCK_RSP_OK */
	{
		*bytes_read = bytes_received; /* return number of bytes read */
	}

	return response;
}


/*------------------------------------------------------------------------------
// receive from udp socket
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_udp_receive(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_udp_receive")

	LSA_RESPONSE_TYPE response;

	for(;;)
	{
		LSA_INT native_error;
		LSA_INT32 bytes_read;

		response = SockIF_RecvFrom( soc->sock_fd
									, (char*)data->buffer_ptr + data->offset
									, data->data_length
									, (data->flags & SOCK_PEEK) ? LSA_TRUE : LSA_FALSE
									, &data->rem_addr
									, &bytes_read
									, &native_error
									);

		if( response != SOCK_RSP_OK ) /* failed */
		{
			if( response == SOCK_RSP_OK_ACTIVE ) /* next attempt in next read */
			{
				break;
			}
			else  /* remove error condition by getsockopt */
			{
				LSA_INT sock_option;
				LSA_INT option_error;

				SockIF_Get_Socket_Error( soc->sock_fd, &sock_option, &option_error); /* reset of so_error -> socket will be available again */

				SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
					, "udp socket handle(%d): recvfrom() returned native_error(%d), response(%d)"
					, soc->sock_fd, native_error, response
					);

				continue;  /* try again immediately */
			}
		}
		else  /* SOCK_RSP_OK */
		{
			data->data_length = (LSA_UINT16)bytes_read;   /* length of this telegram */

			break;
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// send to udp socket
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_udp_send(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_udp_send")

	SockIF_AddrIn *rem_addr_p;
	LSA_INT32 bytes_written;
	LSA_RESPONSE_TYPE response;
	LSA_INT native_error;

	if( soc->rem_addr.sin_port != 0 /* was "connected", see SOCK_OPC_UDP_CONNECT */
		&& data->rem_addr.sin_port == 0 ) /* use "connected" destination address */
	{
		rem_addr_p = 0;
	}
	else
	{
		rem_addr_p = &data->rem_addr;
	}

	response = SockIF_SendTo( soc->sock_fd
							  , (char *)data->buffer_ptr + data->offset
							  , data->data_length
							  , rem_addr_p
							  , &bytes_written
							  , &native_error
							  );

	if( response != SOCK_RSP_OK ) /* failed */
	{
		if( response == SOCK_RSP_OK_ACTIVE ) /* put socket into select set */
		{
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
				, "udp socket handle(%d): pending send"
				, soc->dev_handle
				);
		}
		else
		{
			LSA_INT option_val;
			LSA_INT option_error; 

			SockIF_Get_Socket_Error(soc->sock_fd, &option_val, &option_error); /* remove error condition but inform user */

			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
				, "udp socket handle(%d): sendto() returned native_error(%d), response(%d)"
				, soc->dev_handle, native_error, response
				);
		}
	}
	else /* bytes_written >= 0 */
	{
		SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "socket handle(%d): send ok, bytes_written(%d)"
			, soc->sock_fd, bytes_written
			);

		if ( bytes_written <= data->data_length )
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
				, "socket handle(%d): bytes(%d) of requested(%d) sent, return rqb"
				, soc->dev_handle, bytes_written, data->data_length
				);

			data->data_length = (LSA_UINT16)bytes_written; /* return amount  */
		}
		else /* bytes_written > data->data_length: written more than requested */
		{
			sock_fatal(SOCK_MODULE_ID, SOCK__LINE__);
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_UDP_CONNECT
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_udp_connect(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_UDP_CONN_PTR_TYPE udp_connect
) {
	SOCK_LOG_FCT("sock_udp_connect")

	LSA_RESPONSE_TYPE response;

	if( soc->rem_addr.sin_port != 0 ) /* connected socket has valid remote port */
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "socket handle(%d) is already connected"
			, udp_connect->sock_fd
			);

		response = SOCK_RSP_ERR_SEQUENCE;
	}
	else if( sock_is_not_null(soc->send.curr)
			 || sock_is_not_null(soc->recv.curr) )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "send or receive requests pending for socket handle(%d)"
			, udp_connect->sock_fd
			);

		response = SOCK_RSP_ERR_SEQUENCE;
	}
	else if(udp_connect->rem_addr.sin_family != SOCK_AF_INET
	     || udp_connect->rem_addr.sin_addr.s_addr == 0
			 || udp_connect->rem_addr.sin_port == 0 )
	{
		SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
			, "invalid parameter(s): address(0x%x):port(%d)"
			, SOCK_NTOHL(udp_connect->rem_addr.sin_addr.s_addr)
			, SOCK_NTOHS(udp_connect->rem_addr.sin_port)
			);

		response = SOCK_RSP_ERR_PARAM;
	}
	else
	{
		LSA_INT native_error;
		SockIF_AddrIn rem_addr;

		rem_addr.sin_family = udp_connect->rem_addr.sin_family;
		rem_addr.sin_port = udp_connect->rem_addr.sin_port;
		rem_addr.sin_addr.s_addr = udp_connect->rem_addr.sin_addr.s_addr;

		response = SockIF_Connect(soc->sock_fd, &rem_addr, &native_error);

		if (response != SOCK_RSP_OK)
		{
			LSA_INT error_val;
			LSA_INT option_error;

			SockIF_Get_Socket_Error(soc->sock_fd, &error_val, &option_error); /* eliminate error condtion */

			SOCK_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_ERROR
				, "connect of udp socket handle(%d) to address(0x%x):port(%d) failed with native error(%d)"
				, soc->sock_fd
				, SOCK_HTONL(rem_addr.sin_addr.s_addr)
				, SOCK_HTONS(rem_addr.sin_port)
				, error_val
				);

			if( response != SOCK_RSP_ERR_RESOURCE ) /* do not use error mapping */
			{
				response = SOCK_RSP_ERR_LOC_NET; /* for UDP connect, only local reason is possible */
			}
		}
		else
		{
			soc->rem_addr.sin_family = udp_connect->rem_addr.sin_family;
			soc->rem_addr.sin_port = udp_connect->rem_addr.sin_port; /* indicates connected state of udp socket, see above */
			soc->rem_addr.sin_addr.s_addr = udp_connect->rem_addr.sin_addr.s_addr;

			response = SOCK_RSP_OK;
		}
	}

	return response;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
