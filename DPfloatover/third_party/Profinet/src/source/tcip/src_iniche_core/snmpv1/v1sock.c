/*
 * v3.1 August 30, 2011 snmpv1/v1sock.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: snmpsock.c (derived from snmpport.c)
 *
 * Copyright 1993-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved
 *
 * Contains port-dependent routines for SNMPv1 Agent.
 * This file is used if SNMP_SOCKETS is enabled. That is,
 * - When SNMP_SOCKETS is enabled, v1sock.c is used. v1port is not used.
 * - When SNMP_SOCKETS is disabled, v1sock.c is not used. v1port is used.
 *
 * Here is a list of port-dependent files that need to be modified for
 * porting SNMPv1.
 * SNMP Core - snmp\snmpport.c, snmpport.h
 * SNMPv1    - snmpv1\v1port.c, snmpv1\v1sock.c 
 *
 * 1/19/93   - Created from snmp_age.c John Bartas
 * 10/3/93   - Modified for MIB2 demo. -JB-
 * 10/3/97   - Modified for sockets. -JB-
 * 2/15/2002 - Moved from snmp\snmpsock.c to snmpv1\v1sock.c -AT-
 *
 * MODULE: SNMP
 *
 * ROUTINES: v1_init(), snmp_nvram_init(), v1_check(), v1_cleanup(), 
 * ROUTINES: v1_checksock(), 
 *
 * PORTABLE: no
 */



/* NetPort port general includes */

#include "snmpport.h"

#ifdef INCLUDE_SNMP

#ifdef SNMP_SOCKETS
/* this file uses the sockets API to get at UDP. The lightweight API
implementation is in v1port.c */

#ifdef INICHE_TCPIP
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"
#include "udp.h"

#include "nptcp.h"
#include "mbuf.h"      /* BSD-ish Sockets includes */
#include "socket.h"
#include "sockvar.h"
#include "sockcall.h"
#include "tcpport.h"    /* for SOCKTYPE and API prototypes */
#else
#include "nptcp.h"
#endif

#include "asn1.h"
#include "npsnmp.h"
#include "snmp_imp.h"

#ifdef INCLUDE_NVPARMS
#include "nvparms.h"
extern int snmp_nvram_init();
#endif /* INCLUDE_NVPARMS */

u_char snmpbuf[SNMPSIZ] = {0};   /* global buffer for packet building */
u_char inbuf[SNMPSIZ]; /* buffer for socket reads */
int agent_on = FALSE;      /* TRUE if agent is active  */
SOCKTYPE v1sock = INVALID_SOCKET;
#define close(s)     t_socketclose(s)
/* functions to open listen socket, from snmpport.c */
SOCKTYPE snmp_listen_sock (int port, SOCKTYPE *lsock);
SOCKTYPE snmp_listen_sock6(int port, SOCKTYPE *lsock);

#ifdef SNMP_TEST_SP_TRAP
extern void snmp_test_sp_trap(int version);
#endif

#ifdef IP_V6
#include "socket6.h"
SOCKTYPE v1sock6 = INVALID_SOCKET;
#endif

#ifdef INCLUDE_SNMPV3
#define v1_checksock(s)      ;    /* not needed */ 
#else
void v1_checksock(SOCKTYPE sock);
#endif


/* FUNCTION: v1_init()
 * 
 * Startup the snmp agent (SNMPv1, SNMPv2c). 
 *
 * 1. Read parameters from config file (webport.nv)
 * 2. Initialize the SNMP agent. 
 * 3. Send a ColdStart trap.
 *
 * PARAM1: 
 *
 * RETURNS: 
 * Returns 0 if OK, else non-zero. 
 */

int
v1_init(void)
{
#ifdef INCLUDE_NVPARMS
   snmp_nvram_init();
#endif

   /* by default, enable SNMP traps - increment value from 0 to 1 */
   SNMP_MIB_CTR(snmpEnableAuthenTraps);  /* 1=enabled, 2=disabled. */

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
#ifdef ENABLE_SNMP_TRAPS

   /* NOTE: a separate trap buffer is  provided.
    */
   trap_buffer_len = SNMPSIZ;
#else
   /* disable SNMP traps - increment value from 1 to 2 */
   SNMP_MIB_CTR(snmpEnableAuthenTraps);  /* 1=enabled, 2=disabled. */
#endif /* not ENABLE_SNMP_TRAPS */
#endif /* INCLUDE_SNMPV1 or INCLUDE_SNMPV2C */


#if 0 /* INICHE_TCIP_ITGR create snmp socket in component SOCK */
   /* If SNMPv3 is enabled, it will listen on port SNMP_PORT and 
    * receive all the packets. Hence no need to bind over here .
    */
#ifndef INCLUDE_SNMPV3
   /* open UDP connections (or sockets, or whatever) here */
   snmp_listen_sock(SNMP_PORT,&v1sock);
   agent_on = TRUE;

#ifdef IP_V6
   /* open IPv6 socket for listening */
   snmp_listen_sock6(SNMP_PORT, &v1sock6);
#endif /* IP_V6 */
   
#ifdef SNMP_TEST_SP_TRAP
   /* Trap uses 1 buffer to send a trap. So if there are multiple
    * traps, we need to send them at intervals */
   snmp_test_sp_trap(SNMP_VERSION_1);
#else    /* Test generic trap */
   /* NOT warmstart, see rfc1175, pg28 */
   snmp_trap(SNMP_TRAP_COLDSTART, 0, 0, NULL,SNMP_VERSION_1);
#endif

#endif /* ifndef of INCLUDE_SNMPV3 */
#endif

   return(0);   /* return OK code */
}

int snmp_inck = 0;   /* reentry flag */

/* FUNCTION: v1_check()
 * 
 * Handle SNMPv1/v2c activity.
 * Process all pending requests and send the responses.
 *
 * PARAM1: 
 *
 * RETURNS: 
 */

void
v1_check()
{
#if 0 /* INICHE_TCIP_ITGR check SNMP port in module SOCK */
   if(snmp_inck)
      return;
   snmp_inck++;

   /* if SNMPv3 is enabled, it will check for received packets. 
    * hence no need to do it over here.
    */
   v1_checksock(v1sock); /* see if we have rcvd anything on UDPv4 socket */
#ifdef IP_V6
   v1_checksock(v1sock6);/* see if we have rcvd anything on UDPv6 socket */
#endif

   snmp_inck--;
#endif
}

/* FUNCTION: v1_cleanup()
 * 
 * Cleanup all resources used by SNMP.
 * Close all the sockets opened by SNMP.
 * This function should be called during system shutdown.
 *
 * PARAM1: 
 *
 * RETURNS: Returns 0 if OK, else non-zero. 
 */

int
v1_cleanup()
{
#if 0 /* INICHE_TCIP_ITGR cleanup in module SOCK */
#ifndef INCLUDE_SNMPV3

   close(v1sock);
#ifdef IP_V6
   close(v1sock6);
#endif

#endif /* INCLUDE_SNMPV3 */

#endif
  return 0;
}
#if 0 /* INICHE_TCIP_ITGR check SNMP port in module SOCK */

#ifndef INCLUDE_SNMPV3
/* FUNCTION: v1_checksock()
 * 
 * v1_checksock() - Check if any SNMP requests have been
 * received on the socket. If yes, process them, and send the response.
 *
 * PARAM1: SOCKTYPE sock
 *
 * RETURNS: -
 */

void
v1_checksock(SOCKTYPE sock)
{
struct sockaddr sa;
int e, len=0, sa_len=sizeof(struct sockaddr) ;

   /* check for received snmp packet */
   if(sock != INVALID_SOCKET)
   {
      len = recvfrom(sock, (char*)inbuf, 
         SNMPSIZ, 0, &sa, &sa_len);
      if(len < 0)
      {
         e = t_errno(sock);
         if(e == EWOULDBLOCK) /* no data received */
            return;
         else
         {
            dprintf("snmp: socket read error %d\n", e);
            return;
         }
      }
      else if(len == 0)
      {
         dtrap(); /* socket closed? */
         return;
      }
      /* else (len > 0) */
   }

   if(agent_on)   /* agent is active */
   {
      LOCK_NET_RESOURCE(SNMP_RESID);
      e = snmp_agt_parse(inbuf, len, snmpbuf, SNMPSIZ);
   }
   else   /* agent set inactive by application, just bail out */
      return;  /* agent not active, just punt data */

   if(e)      /* parse worked, send response packet */
   {
      char *sendBuf;
      
      len = e;    /* e is length of snmp reply */
      sendBuf = (char *)npalloc(len);
      
      if (sendBuf == NULL)
      {
         UNLOCK_NET_RESOURCE(SNMP_RESID);
         return;
      }
      MEMCPY(sendBuf, snmpbuf, len);
      UNLOCK_NET_RESOURCE(SNMP_RESID);
      
      if (sa.sa_family == AF_INET)
         sa_len = sizeof (struct sockaddr_in);
#ifdef IP_V6
      else
         sa_len = sizeof (struct sockaddr_in6);
#endif
      
      e = sendto(sock, (char*)sendBuf, len, 0, &sa, sa_len);
      npfree(sendBuf);
      
      if(e != len)
      {
         e = t_errno(sock);
         dprintf("snmp: socket send error \n");
         dtrap();
      }
   }
   else
      UNLOCK_NET_RESOURCE(SNMP_RESID);
}

#endif /* INCLUDE_SNMPV3 */

#endif
#endif /* SNMP_SOCKETS */
#endif /* INCLUDE_SNMP */

/* end of file v1sock.c */

