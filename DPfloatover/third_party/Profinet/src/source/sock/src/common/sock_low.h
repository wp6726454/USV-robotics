#ifndef SOCK_LOW_H
#define SOCK_LOW_H
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
/*  F i l e               &F: sock_low.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/* TCP/IP - stack choice                                                     */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

#if SOCK_CFG_INTERNICHE
	#if !SOCK_CFG_INTERNICHE_DEF_FD /* If SOCK_CFG_INTERNICHE_DEF_FD == 1, the necessary header files have to be included */
	#include "interniche_usr.h"
	#endif

#elif SOCK_CFG_SUN
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/tcp.h>

#elif SOCK_CFG_LINUX386
	#include <sys/socket.h>
	#include <netinet/in.h>

#elif SOCK_CFG_WINSOCK
	#define WIN32_LEAN_AND_MEAN /* Exclude rarely-used stuff */
	#include <winsock2.h>

#elif SOCK_CFG_VXWORKS
	#include <sockLib.h>
	#include <netinet/in.h>

#elif SOCK_CFG_BSDVXW
	#include "sys/socket.h"
	#include "sys/times.h"
	#include "bsdtypes.h"
	#include "netinet/in.h"

#elif SOCK_CFG_FUSION
	/* no additional includes to be done */

#elif SOCK_CFG_TINDK
	#include <usertype.h>
	#include <netmain.h>
	#include <socket.h>

#elif SOCK_CFG_MSPS
	#include "pnio_msps_os.h"

#else
	#error None of the stacks supported by SockIF is choosen!

#endif


/****************************************************************************/
/*::GLOBAL DEFINES
*****************************************************************************/

/* Socket stack specific defines.
----------------------------------------------------------------------------*/
#if SOCK_CFG_INTERNICHE
	#define SOCKIF_AF_INET               AF_INET
	#define SOCKIF_STREAM                SOCK_STREAM
	#define SOCKIF_DGRAM                 SOCK_DGRAM
	#define SOCKIF_FD                    LSA_INT32
	#define SOCKIF_INVALID_FD            -1

	#define SOCKIF_SD_RECV               0
	#define SOCKIF_SD_SEND               1
	#define SOCKIF_SD_BOTH               2

	#define SOCKIF_SOCKET_ERROR          SOCKET_ERROR

	/* If SOCK_CFG_INTERNICHE_DEF_FD == 1, the following SOCKIF_FD_ macros have to be defined! */

	#if !SOCK_CFG_INTERNICHE_DEF_FD
		#define SOCKIF_FD_SET_TYPE       fd_set
		#define SOCKIF_FD_ZERO(s1)       FD_ZERO(s1)
		#define SOCKIF_FD_SET(s1,s2)     FD_SET(s1,s2)
		#define SOCKIF_FD_ISSET(s1,s2)   FD_ISSET(s1,s2)
	#endif

	#define SOCKIF_MAX(_fd1, _fd2)

/****************************************************************************/
#elif SOCK_CFG_SUN || SOCK_CFG_LINUX386
	#define SOCKIF_AF_INET               AF_INET
	#define SOCKIF_STREAM                SOCK_STREAM
	#define SOCKIF_DGRAM                 SOCK_DGRAM
	#define SOCKIF_FD                    LSA_INT32
	#define SOCKIF_INVALID_FD            -1

	#define SOCKIF_SD_RECV               SHUT_RD
	#define SOCKIF_SD_SEND               SHUT_WR
	#define SOCKIF_SD_BOTH               SHUT_RDWR

	#define SOCKIF_SOCKET_ERROR          -1

	#define SOCKIF_FD_SET_TYPE           fd_set
	#define SOCKIF_FD_ZERO(s1)           FD_ZERO(s1)
	#define SOCKIF_FD_SET(s1,s2)         FD_SET(s1,s2)
	#define SOCKIF_FD_ISSET(s1,s2)       FD_ISSET(s1,s2)

	#define SOCKIF_MAX(_fd1, _fd2) if ((_fd2) > (_fd1)) { (_fd1) = (_fd2);}

/****************************************************************************/
#elif SOCK_CFG_VXWORKS || SOCK_CFG_BSDVXW
	#define SOCKIF_AF_INET               AF_INET
	#define SOCKIF_STREAM                SOCK_STREAM
	#define SOCKIF_DGRAM                 SOCK_DGRAM
	#define SOCKIF_FD                    LSA_INT32
	#define SOCKIF_INVALID_FD            -1

	#define SOCKIF_SD_RECV               0
	#define SOCKIF_SD_SEND               1
	#define SOCKIF_SD_BOTH               2

	#define SOCKIF_SOCKET_ERROR          -1

	#define SOCKIF_FD_SET_TYPE           fd_set
	#define SOCKIF_FD_ZERO(s1)           FD_ZERO(s1)
	#define SOCKIF_FD_SET(s1,s2)         FD_SET(s1,s2)
	#define SOCKIF_FD_ISSET(s1,s2)       FD_ISSET(s1,s2)

	#define SOCKIF_MAX(_fd1, _fd2) if ((_fd2) > (_fd1)) { (_fd1) = (_fd2);}

/****************************************************************************/
#elif   SOCK_CFG_TINDK
	#define SOCKIF_AF_INET               AF_INET
	#define SOCKIF_STREAM                SOCK_STREAM
	#define SOCKIF_DGRAM                 SOCK_DGRAM
	#define SOCKIF_FD                    SOCKET
	#define SOCKIF_INVALID_FD            INVALID_SOCKET

	#define SOCKIF_SD_RECV               0
	#define SOCKIF_SD_SEND               1
	#define SOCKIF_SD_BOTH               2

	#define SOCKIF_SOCKET_ERROR          SOCKET_ERROR

	#define SOCKIF_FD_SET_TYPE           fd_set
	#define SOCKIF_FD_ZERO(s1)           FD_ZERO(s1)
	#define SOCKIF_FD_SET(s1,s2)         FD_SET(s1,s2)
	#define SOCKIF_FD_ISSET(s1,s2)       FD_ISSET(s1,s2)
	
	#define SOCKIF_MAX(_fd1, _fd2)
	
/****************************************************************************/
#elif SOCK_CFG_WINSOCK
	#define SOCKIF_AF_INET               AF_INET
	#define SOCKIF_STREAM                SOCK_STREAM
	#define SOCKIF_DGRAM                 SOCK_DGRAM
	#define SOCKIF_FD                    LSA_UINT32
	#define SOCKIF_INVALID_FD            INVALID_SOCKET

	#define SOCKIF_SD_RECV               SD_RECEIVE
	#define SOCKIF_SD_SEND               SD_SEND
	#define SOCKIF_SD_BOTH               SD_BOTH

	#define SOCKIF_SOCKET_ERROR          SOCKET_ERROR

	#define SOCKIF_FD_SET_TYPE           fd_set
	#define SOCKIF_FD_ZERO(s1)           FD_ZERO(s1)
	#define SOCKIF_FD_SET(s1,s2)         FD_SET(s1,s2)
	#define SOCKIF_FD_ISSET(s1,s2)       FD_ISSET(s1,s2)

	#define SOCKIF_MAX(_fd1, _fd2)

/****************************************************************************/
#elif SOCK_CFG_FUSION

	#define AF_INET         2       /* ARPA Internet address, copied from fns_socket.h which cannot be included, due to limited command line length */
	#define SOCK_STREAM     1       /* virtual circuit, copied from fns_socket.h which cannot be included, due to limited command line length  */
	#define SOCK_DGRAM      2       /* datagram, copied from fns_socket.h which cannot be included, due to limited command line length  */
	#define SOCKIF_AF_INET               AF_INET
	#define SOCKIF_STREAM                SOCK_STREAM
	#define SOCKIF_DGRAM                 SOCK_DGRAM
	#define SOCKIF_FD                    int
	#define SOCKIF_INVALID_FD            -1

	#define SOCKIF_SD_RECV               0
	#define SOCKIF_SD_SEND               1
	#define SOCKIF_SD_BOTH               2

	#define SOCKIF_SOCKET_ERROR          -1

	/* some intermediate structure for linger option */
	typedef struct linger
	{
		LSA_INT l_onoff ;
		LSA_INT l_linger ;
	} linger_t ;

	/* as FUSION has a special way of handling fd_set here an intermediate
		* handling is set up.
		* NOTE: use always multiples of 8 for max set size !!!!
		*/
	#define MAX_FD_SET_SIZE 512
	typedef struct
	{
		unsigned char fds_bits [ MAX_FD_SET_SIZE / 8 ] ;
	} f_fd_set ;

	#define SOCKIF_FD_SET_TYPE           f_fd_set
	#define SOCKIF_FD_ZERO(s1)           FD_ZERO(s1)
	#define SOCKIF_FD_SET(s1,s2)         FD_SET(s1,s2)
	#define SOCKIF_FD_ISSET(s1,s2)       FD_ISSET(s1,s2)

	/* for some strange reasons the following is defined at several
		* places, so undefine the stuff, before defining it as needed.
		*/
	#undef FD_SET
	#undef FD_CLR
	#undef FD_ISSET
	#undef FD_ZERO
	#define FD_SET(n, p)                 ((p)->fds_bits[(n)/8] |= (1 << ((n) % 8)))
	#define FD_CLR(n, p)                 ((p)->fds_bits[(n)/8] &= ~(1 << ((n) % 8)))
	#define FD_ISSET(n, p)               ((p)->fds_bits[(n)/8] & (1 << ((n) % 8)))
	#define FD_ZERO(p)                   memset((char *)(p), 0, sizeof(*(p)))

	#define SOCKIF_MAX(_fd1, _fd2) if ((_fd2) > (_fd1)) { (_fd1) = (_fd2);}

	/* as FUSION has some special features for error codes and select
		* handling, some intermediate functions are introduced.
		*/
	struct timeval ;
	void f_init ( void ) ;
	int f_accept( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
	int f_bind ( int fd, SOCK_SOCKADDR_TYPE *address, int addr_len ) ;
	int f_close ( int fd ) ;
	int f_connect ( int fd, SOCK_SOCKADDR_TYPE *address, int addr_len ) ;
	int f_errno ( int fd ) ;
	int f_getpeername ( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
	int f_getsockname ( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
	int f_getsockopt ( int fd, int level, int optname, char *optval, int optlen ) ;
	int f_listen ( int fd, int backlog ) ;
	int f_recv ( int fd, char *buffer, int length, int flags ) ;
	int f_recvfrom( int fd, char *buffer, int length, int flags, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
	int f_select ( int nfd, f_fd_set *rset, f_fd_set *wset, f_fd_set *eset, struct timeval *wait ) ;
	int f_send ( int fd, char *buffer, int length, int flags ) ;
	int f_sendto ( int fd, char *buffer, int length, int flags, SOCK_SOCKADDR_TYPE *address, int addr_len ) ;
	int f_setsockopt ( int fd, int level, int optname, char *optval, int optlen ) ;
	int f_shutdown ( int fd, int how ) ;
	int f_socket ( int family, int type, int protocol ) ;

/* there are some socket options unknown to FUSION. These are simulated
	* in the get/setsockopt intermediate routines. But the socket options
	* have to be defined here.
	*/
#ifndef SO_TYPE /* type of socket */
	#define SO_TYPE        0x1001
#endif
#ifndef SO_ERROR /* last error of socket */
	#define SO_ERROR       0x1002
#endif
#ifndef SO_SNDBUF /* send buffer size of socket */
	#define SO_SNDBUF      0x1003
#endif
#ifndef SO_RCVBUF /* receive buffer size of socket */
	#define SO_RCVBUF      0x1004
#endif
#ifndef SO_NONBLOCK /* blocking/non-blocking mode of socket */
	#define SO_NONBLOCK    0x1005
#endif
#ifndef SO_BROADCAST /* broadcast ability of socket */
	#define SO_BROADCAST   0x1006
#endif

/****************************************************************************/
#elif SOCK_CFG_MSPS

	#define SOCKIF_AF_INET               MSPS_AF_INET
	#define SOCKIF_AF_LOCAL              MSPS_AF_LOCAL
	#define SOCKIF_STREAM                MSPS_SOCK_STREAM
	#define SOCKIF_DGRAM                 MSPS_SOCK_DGRAM
	#define SOCKIF_FD                    MSPS_FD_TYPE
	#define SOCKIF_INVALID_FD            MSPS_INVALID_FD

	#define SOCKIF_SD_RECV               MSPS_SHUT_RD
	#define SOCKIF_SD_SEND               MSPS_SHUT_WR
	#define SOCKIF_SD_BOTH               MSPS_SHUT_RDWR

	#define SOCKIF_SOCKET_ERROR          MSPS_FAILURE
	#define SOCKIF_ERRNO(s)              SockIF_MapError()

	#define SOCKIF_FD_SET_TYPE           MSPS_SOCK_FD_SET
	#define SOCKIF_FD_ZERO(s1)           MSPS_FD_ZERO(s1)
	#define SOCKIF_FD_SET(s1,s2)         MSPS_FD_SET(s1,s2)
	#define SOCKIF_FD_ISSET(s1,s2)       MSPS_FD_ISSET(s1,s2)
	#define SOCKIF_FD_SETSIZE            MSPS_FD_SETSIZE

	#define SOCKIF_MAX(_fd1, _fd2) if ((_fd2) > (_fd1)) { (_fd1) = (_fd2);}

	#ifndef AF_INET /* Due to AP01145901 */
	#define AF_INET                      SOCKIF_AF_INET
	#endif

#endif

/****************************************************************************/
/*::GLOBAL TYPES
*****************************************************************************/

typedef  struct  linger SockIF_Linger;

/* Option types for SockIF_GetSockOpt(), SockIF_SetSockOpt() */
typedef enum
{
	SockOpt_Type,
	SockOpt_Error,
	SockOpt_SendBuffer,
	SockOpt_RecvBuffer,
	SockOpt_NonBlocking,
	SockOpt_ReuseAddr,
	SockOpt_KeepAlive,
	SockOpt_DontRoute,
	SockOpt_Linger,
	SockOpt_Broadcast,
	SockOpt_TCPNoDelay,
	SockOpt_TCPNoAckDelay,
	SockOpt_OOBInline
#if SOCK_CFG_WINSOCK
	,SockOpt_UDP_Connreset
#endif
} SockIF_Option;


/****************************************************************************/
/*::FUNCTION: SockIF_Init                                                   */
/****************************************************************************/
/*!
\descr  The function makes all socket specific initialization and has to be
        called before all other SockIF_... functions.
*****************************************************************************/
LSA_VOID SOCK_LOCAL_FCT_ATTR
SockIF_Init(LSA_VOID);


/****************************************************************************/
/*::FUNCTION: SockIF_Socket                                                 */
/****************************************************************************/
/*!
\param  [in] type
        #SOCKIF_STREAM for TCP or #SOCKIF_DGRAM for UDP connections

\return The socket descriptor or #SOCKIF_INVALID_FD if an error occurred.

\descr  The function creates an unbound socket and returns a socket descriptor.
*****************************************************************************/
SOCKIF_FD SOCK_LOCAL_FCT_ATTR
SockIF_Socket(LSA_INT32 type);


/****************************************************************************/
/*::FUNCTION: SockIF_Bind                                                   */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [in] address
        Points to a #SockIF_AddrIn structure containing the address to be
        bound to the socket. The length and format of the address depend on
        the address family of the socket.
\param  [out] *err
		Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function assigns an address to an unnamed socket. Sockets created
        with SockIF_Socket() are initially unnamed; they are identified only
        by their address family (#SOCKIF_AF).
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Bind(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_Listen                                                 */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [in] backlog
        Maximum length of the queue of pending connections.
\param  [out] *err
		Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function marks a connection-mode socket, specified by the socket
        argument, as accepting connections, and limits the number of
        outstanding connections in the socket's listen queue to the value
        specified by the backlog argument.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Listen(
	SOCKIF_FD sock_fd,
	LSA_INT32 backlog,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_Connect                                                */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [in] address
        Points to a #SockIF_AddrIn structure containing the peer address.
\param  [out] *err
		Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function requests a connection to be made on a socket.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Connect(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_Accept                                                 */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [out] address
        Either a null pointer, or a pointer to a #SockIF_AddrIn structure
        where the address of the connecting socket will be returned.
\param  [out] *err
		Points to int receiving the native result code if not successful.

\return The socket descriptor or #SOCKIF_INVALID_FD if an error occurred.

\descr  The function extracts the first connection on the queue of pending
        connections, creates a new socket with the same socket type protocol
        and address family as the specified socket, and allocates a new
        descriptor for that socket.
*****************************************************************************/
SOCKIF_FD SOCK_LOCAL_FCT_ATTR
SockIF_Accept(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
);


/****************************************************************************/
/*::FUNCTION: SockIF_Close                                                  */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.

\retval LSA_TRUE if the function was successful, LSA_FALSE in case of an error.

\descr  The function will deallocate the specified socket descriptor.
*****************************************************************************/
LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Close(SOCKIF_FD sock_fd);


/****************************************************************************/
/*::FUNCTION: SockIF_Send                                                   */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [in] buffer
        Points to the buffer containing the message to send.
\param  [in] length
        Specifies the length of the message in bytes.
\param  [out] *sent
        Points to int receiving the number of bytes sent
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Sends a message from the specified socket to its peer.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Send(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_INT32 length,
	LSA_INT32* sent,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_Recv                                                   */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [out] buffer
        Pointer to buffer for received data.
\param  [in] length
        Length of the buffer in bytes.
\param  [in] peek
        If true, data is copied into the buffer but not removed from the input
        queue.
\param  [out] *received
        pointer to the length of the message in bytes. If no data is available
        and the peer has performed a graceful shutdown, it returns 0.
        Otherwise, #SOCKIF_SOCKET_ERROR is returned
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Reads data from a connected socket.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Recv(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_INT32 length,
	LSA_BOOL peek,
	LSA_INT32* received,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_SendTo                                                 */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [in] buffer
        Points to the buffer containing the message to send.
\param  [in] length
        Specifies the length of the message in bytes.
\param  [in] address
        Points to a #SockIF_AddrIn structure containing the destination
        address.
\param  [out] *sent
        Points to int receiving the number of bytes sent
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function sends a message through a connection-mode or
        connectionless-mode socket. If the socket is connectionless-mode,
        the message will be sent to the specified address. If the socket is
        connection-mode, the specified address is ignored, making sendto()
        equivalent to send().
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_SendTo(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_UINT32 length,
	SockIF_AddrIn* address,
	LSA_INT32* sent,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_RecvFrom                                               */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [out] buffer
        Points to the buffer containing the message to receive.
\param  [in] length
        Specifies the length of the buffer in bytes.
\param  [in] peek
        True to copy the data into the buffer but remove it not from the input
        queue. False otherwise.
\param  [out] address
        A null pointer, or points to a #SockIF_AddrIn structure in which the
        source address will be stored.
\param  [out] *received
        pointer to the length of the message in bytes. If no data is available
        and the peer has performed a graceful shutdown, it returns 0.
        Otherwise, #SOCKIF_SOCKET_ERROR is returned
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Receives a message from a connection-mode or connectionless-mode socket.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_RecvFrom(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_UINT32 length,
	LSA_BOOL peek,
	SockIF_AddrIn* address,
	LSA_INT32* received,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_Get_Socket_Error                                       */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket decriptor.
\param  [out] socket_error
        The address of variable receiving the socket error.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval LSA_TRUE if the function was successful, LSA_FALSE in case of an error.

\descr  The function retrieves the value for the SO_ERROR option
        for the socket specified by the socket argument.
*****************************************************************************/
LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Get_Socket_Error(
	SOCKIF_FD sock_fd,
	LSA_INT* socket_error,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_SetSockOpt                                             */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket descriptor.
\param  [in] option_name
        A value of the SockIF_Option enumeration type.
\param  [in] option_value
        The address of an option specific variable containing the new option
        value.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval LSA_TRUE if the function was successful, LSA_FALSE in case of an error.

\descr  The function sets the option specified by the option_name argument, to
        the value pointed to by the option_value argument for the socket
        associated with the descriptor specified by the socket argument.
*****************************************************************************/
LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_SetSockOpt(
	SOCKIF_FD sock_fd,
	SockIF_Option option_name,
	LSA_VOID* option_value,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_SetMulticastSockOpts                                             */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Socket descriptor.
\param  [in] IfIndex
        Interface index of interface for outgoing multicast traffic.
\param  [in] group_addr
        Multicast group address in network byte order.
\param  [in] ip_multicast_ttl
        TTL number of sent multicast packets (see documentation for details)
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function sets the necessary multicast options for a multicast enabled
        socket. See documentation of opcode SOCK_OPC_UDP_MULTICAST_OPEN.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_SetMulticastSockOpts(
	SOCKIF_FD sock_fd,
	LSA_UINT32 IfIndex,
	LSA_UINT32 group_addr,
	LSA_UINT8 ip_multicast_ttl,
	int *err
);


/****************************************************************************/
/*::FUNCTION: SockIF_GetSockName                                            */
/****************************************************************************/
/*!
\param  [in] sock_fd
        Specifies the descriptor of the socket.
\param  [out] address
        A pointer to a #SockIF_AddrIn structure where the local address of the
        socket will be returned.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function retrieves the locally-bound name of the specified socket
        and stores this address in the LD_SOCKADDR structure pointed to by the
        address argument.

        If the socket has not been bound to a local name, the value stored in
        the object pointed to by address is unspecified.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_GetSockName(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
);


/****************************************************************************/
/*::FUNCTION: SockIF_Select                                                 */
/****************************************************************************/
/*!
\param  [in] nfds (see 1140333 also )
        The first nfds descriptors are checked in each set; i.e., the 
        descriptors from 0 through nfds-1 in the descriptor sets are examined.
\param  [in,out] readfds
        If the argument is not a null pointer, it points to an object of type
        SOCKIF_FD_SET_TYPE that on input specifies the file descriptors to be
        checked for being ready to read, and on output indicates which socket
        descriptors are ready to read.
\param  [in,out] writefds
        If the argument is not a null pointer, it points to an object of type
        SOCKIF_FD_SET_TYPE that on input specifies the file descriptors to be
        checked for being ready to write, and on output indicates which socket
        descriptors are ready to write.
\param  [in,out] errorfds
        If the argument is not a null pointer, it points to an object of type
        SOCKIF_FD_SET_TYPE that on input specifies the file descriptors to be
        checked for error conditions pending, and on output indicates which
        socket descriptors have error conditions pending.
\param  [in] timeout
        The timeout interval in milliseconds.

\return Upon successful completion, the function returns the total number of
        socket handles that are ready and contained in the SOCKIF_FD_SET
        structures, zero if the time limit expired, or SOCKIF_SOCKET_ERROR if
        an error occurred.

\descr  The function indicates which of the specified file descriptors is
        ready for reading, ready for writing, or has an error condition
        pending. If the specified condition is false for all of the specified
        file descriptors, it blocks, up to the specified timeout interval,
        until the specified condition is true for at least one of the
        specified socket descriptors.

\note   On Win32 systems a select() with no specifed file descriptor returns
        immediately with an error code instead of sleeping the specified time.
*****************************************************************************/
LSA_INT32 SOCK_LOCAL_FCT_ATTR
SockIF_Select(int nfds, SOCKIF_FD_SET_TYPE* readfds, SOCKIF_FD_SET_TYPE* writefds, SOCKIF_FD_SET_TYPE* errorfds, LSA_INT32 timeout);


/****************************************************************************/
/*::FUNCTION: SockIF_MapConnectError                                               */
/****************************************************************************/
/*!
\param  [in] error
        Native error code returned by connect().

\return SOCK response code.

\descr  Maps the stack specific error code to a SOCK response code.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapConnectError(LSA_INT error);


/****************************************************************************/
/*::FUNCTION: SockIF_MapTCPError                                        */
/****************************************************************************/
/*!
\param  [in] error
        Native error code returned by send() or receive().

\return SOCK response code.

\descr  Maps the stack specific error code to a SOCK response code.
*****************************************************************************/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapTCPError(LSA_INT error);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of SOCK_LOW_H */
