#ifndef _TCPPORT_H
#define _TCPPORT_H  1

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*
 * FILENAME: tcpport.h
 *
 * Copyright 1997 - 2000 By InterNiche Technologies Inc. All rights reserved
 *
 * TCP port dependent definitions. 
 *
 * MODULE: INET
 *
 *
 * PORTABLE: usually
 */

/* Additional Copyrights: */
/* Portions Copyright 1996 by NetPort Software. All rights reserved. */

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
/*  F i l e               &F: tcpport.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration module                                                     */
/*                                                                           */
/*****************************************************************************/

#include "ipport.h" /* which in turn includes tcip_cfg.h */

/*****************************************************************************/

#ifdef MINI_TCP
#error This file only for use with full-size TCP
#endif

#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"


#ifdef IP_V6
#include "ip6.h"
#include "socket6.h"
#endif   /* IP_V6 */


/* Define a default TCP MSS *Max. Segment Size) which will allow us to send
 * efficiently over our interface(s) without IP fragmenting. We default to
 * regular ethernet, and reduce the size if we have to deal with slower (PPP) or
 * badly designed legacy (802.3) links.
 */

#ifndef  TCP_MSS           /* allow override in ipport.h */
#ifdef   IPSEC
#define  TCP_MSS  0x04da   /* reduce to allow room for IPSEC encapsulation */
#elif  defined(IEEE_802_3)
#define  TCP_MSS  0x05ac   /* reduce for extra overhead of SNAP */
#elif  defined(USE_PPP)
#ifdef PPP_VJC
#define  TCP_MSS  0x0530   /* reduce by another 128 (VJC_HDR_OFFSET) on PPP */
#else
#define  TCP_MSS  0x05b0   /* reduce to avoid fragments on PPP */
#endif   /* PPP_VJC */
#else
#define  TCP_MSS  0x05b4   /* assume normal ethernet */
#endif   /* media type */
#endif   /* TCP_MSS */


/* TCP code options - these need to preceed the nptcp.h include */
#define  UDP_SOCKETS    1  /* support UDP (DGRAM) under sockets API */

#define  SO_SELECT      1  /* support select() call */

#ifdef NOTDEF
TCP options not used in this build:
#define  BSDTCP_STATS   1  /* printfs for extensive BSD statistics */
#define  DO_TCPTRACE    1  /* copius debug tracing of TCP logic */
#endif

#include "nptcp.h"      /* InterNIche<->BSD mappings */
#include "mbuf.h"       /* BSD-ish Sockets includes */


#ifndef CADDRT_ALREADY
typedef char * caddr_t;
#endif

#ifdef INCLUDE_SNMP
struct variable;
#include "snmpport.h"
extern   struct tcp_mib tcpmib;
#else    /* no SNMP, support tcp_mib locally for tcp_stats() */
struct TcpMib
{
   long     tcpRtoAlgorithm;
   long     tcpRtoMin;
   long     tcpRtoMax;
   long     tcpMaxConn;
   u_long   tcpActiveOpens;
   u_long   tcpPassiveOpens;
   u_long   tcpAttemptFails;
   u_long   tcpEstabResets;
   u_long   tcpCurrEstab;
   u_long   tcpInSegs;
   u_long   tcpOutSegs;
   u_long   tcpRetransSegs;
   void *   tcpConnTable;  /*32 bit ptr */
   u_long   tcpInErrs;
   u_long   tcpOutRsts;
};
extern   struct TcpMib  tcpmib; 
#endif   /* INCLUDE_SNMP */

#define  TCP_MIB_INC(varname) {tcpmib.varname++;}
#define  TCP_MIB_DEC(varname) {tcpmib.varname--;}

#ifdef CRUDE_ALLOCS

/* prototype TCP ALLOC and FREE routines  */
struct inpcb * INP_ALLOC(int size);       /* alloc internet control block "inpcb" */
void  INP_FREE(struct inpcb *);           /* free internet control block */
struct mbuf * MBU_ALLOC(int size);        /* alloc mbuf struct */
struct socket * SOC_ALLOC(int size);      /* alloc a socket structure */
void  SOC_FREE(struct socket *);          /* free a socket structure */
struct tcpiphdr * TPH_ALLOC(int size);    /* alloc a TCP/IP header buffer */ 
void  TPH_FREE(struct tcpiphdr *);        /* free a TCP/IP header buffer */ 
struct tcpcb * TCB_ALLOC(int size);       /* ... tcp connection structure */
void  TCB_FREE(struct tcpcb *);
struct ip_socopts * SOCOPT_ALLOC(int size); /* socket options (IP_TOS, IP_TTL_OPT) */
void  SOCOPT_FREE(struct ip_socopts *);

#else /* not so CRUDE_ALLOCS */

/* MAP ALLOC and FREE calls to port's memory manager */
#define  INP_ALLOC(size)      (struct inpcb *)npalloc(size)
#define  INP_FREE(ptr)        npfree((void*)ptr)
#define  MBU_ALLOC(size)      (struct mbuf *)npalloc(size)
#define  SOC_ALLOC(size)      (struct socket *)npalloc(size)
#define  SOC_FREE(ptr)        npfree((void*)ptr)
#define  TPH_ALLOC(size)      (struct tcpiphdr *)npalloc(size)
#define  TPH_FREE(ptr)        npfree((void*)ptr)
#define  TCB_ALLOC(size)      (struct tcpcb *)npalloc(size)
#define  TCB_FREE(ptr)        npfree((void*)ptr)
#define  SOCOPT_ALLOC(size)   (struct ip_socopts *)npalloc(size)
#define  SOCOPT_FREE(ptr)     npfree((void*)ptr)

#endif   /* alloc macros */

#ifdef WEBPORT
extern   int http_init(void);
#endif

#ifdef SO_SELECT
/* define the size of the sockets arrays passed to select(). On UNIX 
 * and winsock this is usually 64, but most embedded systems don't 
 * need more than 1 or 2, and can't always afford to waste the space. 
 * NOTE: These determine the size of set_fd structs, which are often 
 */
#ifndef FD_SETSIZE   /* let h_h files override */
#define  FD_SETSIZE     12
#endif   /* FD_SETSIZE */
#endif   /* SO_SELECT */

#ifdef DO_TCPTRACE
/* void tcp_trace(char *, ...); */
#define  tcp_trace   printf
#endif


/* Calculate mbuf reserved space for MAC + IP + TCP + OPTIONS.
 * This is based on ethernet, 802.3 and and PPPoE since they are the
 * largest of our default MAC header sizes.
 */
 
#ifndef ETHHDR_BIAS
#define ETHHDR_BIAS 0
#endif

/* If we are using RFC-1323 TCP timestamp feature allow space for the
 * option in each packet
 */
#ifdef TCP_TIMESTAMP
#define  RTTMOPT_SIZE   12    /* Actual size plus alignment pad */
#else
#define  RTTMOPT_SIZE   0
#endif /* TCP_TIMESTAMP */


/* Figure out a minimum value */
#ifdef USE_PPPOE
#define  HDRSMINLEN  (64 + ETHHDR_BIAS + RTTMOPT_SIZE + IPSECOPT_SIZE)
#elif  defined(IEEE_802_3)
#define  HDRSMINLEN  (62 + ETHHDR_BIAS + RTTMOPT_SIZE + IPSECOPT_SIZE)
#else
#define  HDRSMINLEN  (54 + ETHHDR_BIAS + RTTMOPT_SIZE + IPSECOPT_SIZE)
#endif

/* Figure out the actual value: minimum value padded out to 
 * a multiple of ALIGN_TYPE
 */
#ifdef TCIP_CFG_ALIGN_HEADERS /*INICHE_TCIP_ITGR align tcp/ip headers instead of data */
    #define  HDRSLEN  HDRSMINLEN
#else
    #if ((HDRSMINLEN & (ALIGN_TYPE - 1)) == 0)
    #define  HDRSLEN  HDRSMINLEN
    #else
    #define  HDRSLEN  (HDRSMINLEN + ALIGN_TYPE - (HDRSMINLEN & (ALIGN_TYPE - 1)))
    #endif
#endif


/* macros to convert between 32-bit long socket descriptors and the 
 * actual socket structure pointers. On most systems they just cast 
 * the values. They are provided to support 48 bit "large model" 
 * 386/pentium builds, which 
 */

#ifndef  LONG2SO
#define  LONG2SO(ln)    ((struct socket *)ln)
#define  SO2LONG(so)    ((long)so)
#define  LONG2MBUF(ln)  ((struct mbuf *)ln)
#define  MBUF2LONG(mb)  ((long)mb)
#endif

/* Now that TCP options are set up, include the sockets definitions */

#include "socket.h"
#include "sockvar.h"
#include "sockcall.h"

/* get a smart TCP_MSS for a socket. If socket is null then the default value
 * frome the TCP_MSS defrinitions above is returned.
 */
int   tcp_mss (struct socket *);

/* map protable "sys_" calls to InterNiche Sockets library */
#ifdef INTEGRITY  /* Intergrity has it's own legacy mapping */
#include "../ghsintl/isockmap.h"

#else    /* InterNiche full-size stack BSD-ish sockets calls */

#ifndef SYSMACRO_ALREADY   /* Allow override from ipport.h */
/* Map ancient NetPort "sys_" socket calls to the newer (version 2.0)
 * "standard" socket calls used in 2.0
 */
#define  sys_closesocket(sock)   t_socketclose(sock)
#define  sys_send(sock, buf,  len,  flags)   t_send(sock,   buf,  len,  flags)
#define  sys_recv(sock, buf,  len,  flags)   t_recv(sock,   buf,  len,  flags)
#define  sys_accept(X,Y,L)       t_accept(X,Y,L)
#define  sys_bind(X,Y,L)         t_bind(X,Y,L)
#define  sys_socket              t_socket
#define  sys_listen              t_listen
#define  sys_errno(sock)         t_errno(sock)
#define  sys_connect(X,Y,L)      t_connect(X,Y,L)
#define  sys_shutdown            t_shutdown

#define SYS_EWOULDBLOCK EWOULDBLOCK

/* Map plain BSD socket routine names to Interniche t_" names. As always,
 * allow this to be overridden by ipport.h_h
 */
#ifndef USER_SOCKETNAMES
#define  socket(x,y,z)           t_socket(x,y,z)
#define  bind(s,a,l)             t_bind(s,a,l)
#define  connect(s,a,l)          t_connect(s,a,l)
#define  listen(s,c)             t_listen(s,c)
#define  send(s, b, l, f)        t_send(s, b, l, f)
#define  recv(s, b, l, f)        t_recv(s, b, l, f)
#define  accept(s,a,l)           t_accept(s, a, l)
#define  sendto(s,b,l,f,a,x)     t_sendto(s,b,l,f,a,x)
#define  recvfrom(s,b,l,f,a,x)   t_recvfrom(s,b,l,f,a,x)
#define  socketclose(s)          t_socketclose(s)
#define  setsockopt(s,l,o,d,x)   t_setsockopt(s,l,o,d,x)
#endif

#endif   /* SYSMACRO_ALREADY */

#endif   /* sockets mapping */


#define  WP_SOCKTYPE long     /* WebPort socket type */
#define  SOCKTYPE    long     /* preferred generic socket type */

/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif   /* _TCPPORT_H */

/* end of file tcpport.h */
