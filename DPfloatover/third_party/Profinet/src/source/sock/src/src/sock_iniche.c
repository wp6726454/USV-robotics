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
/*  F i l e               &F: sock_iniche.c                             :F&  */
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

#define LTRC_ACT_MODUL_ID 4
#define SOCK_MODULE_ID    4

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_INTERNICHE
/*****************************************************************************/
/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
SockIF_Init(
	LSA_VOID
) {
	if( sizeof(SockIF_AddrIn) != sizeof(struct sockaddr) ) {
		SOCK_FATAL();
	}
}

/*----------------------------------------------------------------------------*/

SOCKIF_FD SOCK_LOCAL_FCT_ATTR
SockIF_Socket(
	LSA_INT32 type
) {
	return t_socket(PF_INET, type, 0);
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Bind(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT *err
) {
	if( t_bind(sock_fd, (struct sockaddr*)address, sizeof(*address)) < 0)
	{
		*err = t_errno(sock_fd);

		switch(*err)
		{
		case EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case ENOMEM:
		case ENP_RESOURCE:
		case ENOBUFS:
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
	if( t_listen(sock_fd, backlog) < 0 )
	{
		*err = t_errno(sock_fd);

		switch(*err)
		{
		case EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapConnectError(
	LSA_INT error
) {
	switch(error)
	{
	case EINPROGRESS:
		return SOCK_RSP_OK_PROGRESS;
	
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENP_RESOURCE:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case ECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case ENETUNREACH:
	case ETIMEDOUT:
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
	if( t_connect(sock_fd, (struct sockaddr*)address, sizeof(*address)) < 0 )
	{
		*err = t_errno(sock_fd);

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

	new_fd  = t_accept(sock_fd, (struct sockaddr*)address, &address_len);

	if (new_fd == SOCKIF_INVALID_FD)
	{
		*err = t_errno(sock_fd); /* for caller trace only */
	}

	return new_fd;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Close(
	SOCKIF_FD sock_fd
) {
	if( t_socketclose(sock_fd) != 0 )
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
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ECONNABORTED:
	case ECONNRESET:
	case ESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	case EPIPE: /* ETIMEDOUT error in socket, but t_send() and t_recv() yield EPIPE  */
	case ETIMEDOUT: /* connection dropped by stack's timer */
		return SOCK_RSP_ERR_REM_NET;

	case ENOMEM:
	case ENP_RESOURCE:
	case ENOBUFS:
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
	/* note: The interniche stack can return 0 instead of error EWOULDBLOCK */

	*sent = t_send( sock_fd
					, (char*)buffer
					, length
					, MSG_DONTWAIT
					);

	if (*sent < 0)
	{
		*err = t_errno(sock_fd);

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
	*received = t_recv( sock_fd
						, (char*)buffer
						, length
						, peek ? MSG_PEEK : 0
						);

	if (*received < 0)
	{
		*err = t_errno(sock_fd);

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
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENP_RESOURCE:
	case ENOBUFS:
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
	*sent = t_sendto( sock_fd
					  , (char*)buffer
					  , length
					  , MSG_DONTWAIT
					  , (struct sockaddr*)address
					  , sizeof(struct sockaddr)
					  );

	if (*sent < 0)
	{
		*err = t_errno(sock_fd);

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
	LSA_INT   address_len = sizeof(struct sockaddr);

	*received = t_recvfrom( sock_fd
							, (char*)buffer
							, length
							, peek ? MSG_PEEK : 0
							, (struct sockaddr*)address
							, &address_len
							);

	if (*received < 0)
	{
		*err = t_errno(sock_fd);

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
	LSA_INT32 len = sizeof(LSA_INT);
	
	int ret = t_getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, socket_error, len);

	if (ret < 0)
	{
		*err = t_errno(sock_fd);

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
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_NONBLOCK, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_TCPNoDelay:
		ret = t_setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_TCPNoAckDelay:
		ret = t_setsockopt(sock_fd, IPPROTO_TCP, TCP_NOACKDELAY, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_ReuseAddr:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_KeepAlive:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_KEEPALIVE, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_DontRoute:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_DONTROUTE, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_SendBuffer:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_RecvBuffer:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_Linger:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, option_value, sizeof(SockIF_Linger));
		break;

	case SockOpt_Broadcast:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_OOBInline:
		ret = t_setsockopt(sock_fd, SOL_SOCKET, SO_OOBINLINE, option_value, sizeof(LSA_INT32));
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
		*err = t_errno(sock_fd);

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
#if SOCK_CFG_ENABLE_MULTICASTS
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_SetMulticastSockOpts(
	SOCKIF_FD sock_fd,
	LSA_UINT32 IfIndex,
	LSA_UINT32 group_addr,
	LSA_UINT8 ip_multicast_ttl,
	int *err
) {
	SOCK_LOG_FCT("SockIF_SetMulticastSockOpts")

	struct ip_mreq stMreq;
	LSA_UINT8 mc_loop = 0;

	*err = 0;
	stMreq.imr_interface = IfIndex;
	stMreq.imr_multiaddr = group_addr;

	if(t_setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &stMreq, sizeof(stMreq)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_ADD_MEMBERSHIP option");
		*err = t_errno(sock_fd);
	}
	else if( t_setsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_IF, &IfIndex, sizeof(IfIndex)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_IF option");
		*err = t_errno(sock_fd);
	}
	else if(t_setsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &mc_loop, sizeof(mc_loop)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_LOOP option");
		*err = t_errno(sock_fd);
	}
	else if(t_setsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_TTL, &ip_multicast_ttl, sizeof(ip_multicast_ttl)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_TTL option");
		*err = t_errno(sock_fd);
	}

	switch(*err)
	{
	case 0:
		return SOCK_RSP_OK;
	
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE; /* memory allocation failed */
	
	case EINVAL:
		return SOCK_RSP_ERR_PARAM; /* not a mc address used for IP_ADD_MEMBERSHIP */

	/* should never happen: */
	case EADDRNOTAVAIL:
	case EADDRINUSE:
	case ETOOMANYREFS:
	case EOPNOTSUPP:
	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}
#endif

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_GetSockName(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
) {

	LSA_INT   address_len = sizeof(*address);

	if( t_getsockname(sock_fd, (struct sockaddr*)address, &address_len) < 0 )
	{
		*err = t_errno(sock_fd);

		switch(*err)
		{
		case ENOMEM:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
		
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
	LSA_UNUSED_ARG(nfds);

	return t_select(readfds, writefds, errorfds, (TPS * timeout) / 1000);
}

/*****************************************************************************/
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
