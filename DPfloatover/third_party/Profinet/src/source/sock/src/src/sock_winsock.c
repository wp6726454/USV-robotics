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
/*  F i l e               &F: sock_winsock.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Portability layer for socket access                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 7
#define SOCK_MODULE_ID    7

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_WINSOCK
/*****************************************************************************/
/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
SockIF_Init(
	LSA_VOID
) {
	SOCK_LOG_FCT("SockIF_Init")

	WORD version;
	WSADATA wasa;

	SOCK_LOWER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "Initialize socket interface ");

	version = MAKEWORD(1, 1);

	if (WSAStartup(version, &wasa) != 0)
	{
		SOCK_LOWER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "### Couldn't initialize the socket interface!");
		SOCK_FATAL();
	}
}

/*----------------------------------------------------------------------------*/

SOCKIF_FD SOCK_LOCAL_FCT_ATTR
SockIF_Socket(
	LSA_INT32 type
) {
	return socket(PF_INET, type, 0);
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Bind(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT *err
) {
	if(bind(sock_fd, (struct sockaddr*)address, sizeof(*address)) < 0)
	{
		*err = WSAGetLastError();

		switch(*err)
		{
		case WSAEADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case WSAENOBUFS:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Listen(
	SOCKIF_FD sock_fd,
	LSA_INT32 backlog,
	LSA_INT* err
) {
	if( listen(sock_fd, backlog) < 0 )
	{
		*err = WSAGetLastError();

		switch(*err)
		{
		case WSAEADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapConnectError
	(LSA_INT error
) {
	switch(error)
	{
	case WSAEINPROGRESS:
		return SOCK_RSP_OK_PROGRESS;

	case WSAEWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case WSAENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case WSAECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case WSAEHOSTUNREACH:
	case WSAENETUNREACH:
	case WSAETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Connect(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
) {
	if(connect(sock_fd, (struct sockaddr*)address, sizeof(*address)) < 0)
{
		*err = WSAGetLastError();

		return SockIF_MapConnectError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

SOCKIF_FD SOCK_LOCAL_FCT_ATTR
SockIF_Accept(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
) {
	SOCKIF_FD new_fd;

	LSA_INT address_len = sizeof(*address);

	new_fd = accept(sock_fd, (struct sockaddr*)address, &address_len);

	if (new_fd == SOCKIF_INVALID_FD)
	{
		*err = WSAGetLastError();
	}

	return new_fd;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Close(
	SOCKIF_FD sock_fd
) {
	if( closesocket(sock_fd) < 0 )
	{
		return LSA_FALSE;
	}
	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapTCPError(
	LSA_INT error
) {
	switch(error)
	{
	case WSAEWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case WSAECONNRESET:
	case WSAESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	case WSAECONNABORTED:
	case WSAENETRESET:
	case WSAEHOSTUNREACH:
	case WSAETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	case WSAENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Send(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_INT32 length,
	LSA_INT32* sent,
	LSA_INT* err
) {
	*sent = send( sock_fd
				  , (const char*)buffer
				  , length
				  , 0
				  );

	if (*sent < 0)
	{
		*err = WSAGetLastError();

		return SockIF_MapTCPError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Recv(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_INT32 length,
	LSA_BOOL peek,
	LSA_INT32* received,
	LSA_INT* err
) {
	*received = recv( sock_fd
					  , (char*)buffer
					  , length
					  , peek ? MSG_PEEK : 0
					  );

	if (*received < 0)
	{
		*err = WSAGetLastError();

		return SockIF_MapTCPError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

static LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapUDPError(
	LSA_INT err
) {
	switch(err) /* note: no ERR_REM_NET possible */
	{
	case WSAEWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case WSAENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_SendTo(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_UINT32 length,
	SockIF_AddrIn* address,
	LSA_INT32* sent,
	LSA_INT* err
) {
	*sent = sendto( sock_fd
					, (const char*)buffer
					, length
					, 0
					,(struct sockaddr*)address
					, sizeof(*address)
					);

	if (*sent < 0)
	{
		*err = WSAGetLastError();

		return SockIF_MapUDPError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_RecvFrom(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_UINT32 length,
	LSA_BOOL peek,
	SockIF_AddrIn* address,
	LSA_INT32* received,
	LSA_INT* err
) {
	LSA_INT   address_len = sizeof(*address);

	*received = recvfrom( sock_fd
						  , (char*)buffer
						  , length
						  , peek ? MSG_PEEK : 0
						  , (struct sockaddr*)address
						  , &address_len
						  );

	if (*received < 0)
	{
		*err = WSAGetLastError();

		return SockIF_MapUDPError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Get_Socket_Error(
	SOCKIF_FD sock_fd,
	LSA_INT* socket_error,
	LSA_INT* err
) {
	LSA_INT   len = sizeof(LSA_INT);

	int  ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, (char*)socket_error, &len);

	if (ret < 0)
	{
		*err = WSAGetLastError();

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_SetSockOpt(
	SOCKIF_FD sock_fd,
	SockIF_Option option_name,
	LSA_VOID* option_value,
	LSA_INT* err
) {
	int ret;

	switch (option_name)
	{
	case SockOpt_NonBlocking:

		ret = ioctlsocket(sock_fd, FIONBIO, (u_long*)option_value);
		break;

	case SockOpt_TCPNoDelay:
		ret = setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, (char*)option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_TCPNoAckDelay:
		ret = 0;
		break;

	case SockOpt_ReuseAddr:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char*)option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_KeepAlive:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_KEEPALIVE, (char*)option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_DontRoute:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_DONTROUTE, (char*)option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_SendBuffer:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, (char*)option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_RecvBuffer:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, (char*)option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_Linger:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, (char*)option_value, sizeof(SockIF_Linger));
		break;

	case SockOpt_Broadcast:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, (char*)option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_OOBInline:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_OOBINLINE, (char*)option_value, sizeof(LSA_INT32));
		break;

#ifdef SIO_UDP_CONNRESET
	case SockOpt_UDP_Connreset:
		DWORD dwBytesReturned = 0;
		BOOL bNewBehavior = LSA_FALSE;
		ret  = WSAIoctl(sock_fd, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior),
				   NULL, 0, &dwBytesReturned,
				   NULL, NULL);
		break;
#endif

	default:
		ret = -1; /* prevent compiler warning C4701: potentially uninitialized local variable 'ret' used */

		SOCK_FATAL();

		break;
	}

	if (ret < 0)
	{
		*err = WSAGetLastError();

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_GetSockName(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
) {

	LSA_INT address_len = sizeof(*address);

	if( getsockname(sock_fd, (struct sockaddr*)address, &address_len) < 0 )
	{
		*err = WSAGetLastError();

		return SOCK_RSP_ERR_LOC_NET;
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_INT32 SOCK_LOCAL_FCT_ATTR
SockIF_Select(
	int nfds,
	SOCKIF_FD_SET_TYPE* readfds,
	SOCKIF_FD_SET_TYPE* writefds,
	SOCKIF_FD_SET_TYPE* errorfds,
	LSA_INT32 timeout
) {
	struct timeval tv;

	LSA_UNUSED_ARG(nfds);

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	return select(0/*ignored*/, readfds, writefds, errorfds, &tv);
}

#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
