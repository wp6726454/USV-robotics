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
/*  F i l e               &F: sock_bsdvxw.c                             :F&  */
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

#define LTRC_ACT_MODUL_ID 15
#define SOCK_MODULE_ID    15

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_BSDVXW
/*****************************************************************************/

#include "sys/socket.h"
#include "sys/times.h"
#include "bsderrno.h"
#include "bsdtypes.h"
#include "bsdsocketif.h"
#include "bsdsocketdefines.h"
#include "netinet/tcp.h"

/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
SockIF_Init(
	LSA_VOID
) {
	/* empty */
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
	if( bind(sock_fd, (struct sockaddr*)address, sizeof(*address)) < 0 )
	{
		*err = errno;

		switch(*err)
		{
		case bsd_EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case bsd_ENOMEM:
		case bsd_ENOBUFS:
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
		*err = errno;

		switch(*err)
		{
		case bsd_EADDRINUSE:
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
	case bsd_EINPROGRESS:
		return SOCK_RSP_OK_PROGRESS;
	case bsd_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case bsd_ENOMEM:
	case bsd_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case bsd_ECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case bsd_ENETUNREACH:
	case bsd_ETIMEDOUT:
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
	if( connect(sock_fd, (struct sockaddr*)address, sizeof(*address)) < 0 )
	{
		*err = errno;

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
	SOCKIF_FD fd = 0;
	int   addr_len = sizeof(struct sockaddr);
	struct sockaddr addr;

	addr.sa_family = address->sin_family;

	fd = accept(sock_fd, (struct sockaddr*)&addr, &addr_len);

	if (fd == SOCKIF_INVALID_FD)
	{
		*err = errno;
	}
	else
	{
		address->sin_family = addr.sa_family;
		memcpy( (void*)&(address->sin_port), (void*)addr.sa_data, 14);
	}

	return fd;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Close(
	SOCKIF_FD sock_fd
) {
	if( close(sock_fd) != 0 )
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
	case bsd_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case bsd_ECONNABORTED:
	case bsd_ECONNRESET:
	case bsd_ESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	/* map these and any other errors to SOCK_RSP_ERR_LOC_NET */
	case bsd_ENOMEM:
	case bsd_ENOBUFS:
	case bsd_EPIPE: /* socket is not connected */
	case bsd_ENOTCONN: /* -"- */
	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapTCPReceiveError(LSA_INT error)
{
	switch(error)
	{
	case bsd_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case bsd_ECONNABORTED:
	case bsd_ECONNRESET:
	case bsd_ESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	case bsd_EPIPE:
	case bsd_ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	case bsd_ENOMEM:
	case bsd_ENOBUFS:
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
	*sent = send(sock_fd
				 , (char*)buffer
				 , length
				 , 0
				 );

	if (*sent < 0)
	{
		*err = errno;

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
	*received = recv(sock_fd
					 , (char*)buffer
					 , length
					 , peek ? MSG_PEEK : 0
					 );

	if (*received < 0)
	{
		*err = errno;

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
	case bsd_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case bsd_ENOMEM:
	case bsd_ENOBUFS:
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
#if defined IP_ACL || defined SECURITY_SUPPORTED
	aclInsertAddrToPnioArea ( address->sin_addr.S_un.S_addr ) ;
#endif

	*sent = sendto(sock_fd
				   , (char*)buffer
				   , length
				   , 0
				   , (struct sockaddr*)address
				   , sizeof(struct sockaddr)
				   );

	if (*sent < 0)
	{
		*err = errno;

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
	LSA_INT address_len = sizeof(struct sockaddr);

	*received = recvfrom(sock_fd
						 , (char*)buffer
						 , length
						 , peek ? MSG_PEEK : 0
						 , (struct sockaddr*)address
						 , &address_len
						 );

	if (*received < 0)
	{
		*err = errno;

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
	
	int ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, socket_error, &len);

	if (ret < 0)
	{
		*err = errno;

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
	SOCK_LOG_FCT("SockIF_SetSockOpt")

	int ret;

	switch (option_name)
	{
	case SockOpt_NonBlocking:
		ret = nonblocking(sock_fd);
		break;

	case SockOpt_TCPNoDelay:
		ret = setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_TCPNoAckDelay:
		ret = 0;
		break;

	case SockOpt_ReuseAddr:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_KeepAlive:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_KEEPALIVE, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_DontRoute:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_DONTROUTE, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_SendBuffer:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_RecvBuffer:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_Linger:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, option_value, sizeof(SockIF_Linger));
		break;

	case SockOpt_Broadcast:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_OOBInline:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_OOBINLINE, option_value, sizeof(LSA_INT32));
		break;

	default:
		ret = -1; /* prevent compiler warning C4701: potentially uninitialized local variable 'ret' used */

		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
			, "option_name(%d) not supported by this function"
			, option_name
			);

		SOCK_FATAL();

		break;
	}

	if (ret < 0)
	{
		*err = errno;

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
		*err = errno;

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

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	return select(nfds, readfds, writefds, errorfds, &tv);
}

/*****************************************************************************/
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
