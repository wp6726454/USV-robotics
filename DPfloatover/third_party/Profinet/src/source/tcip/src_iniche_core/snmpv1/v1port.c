/*
 * v3.1 August 30, 2011 snmpv1/v1port.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: v1port.c
 *
 * Portions Copyright 1993-96 by NetPort Software. All rights reserved. 
 * Copyright 1997-2004 By InterNiche Technologies Inc. All rights reserved
 *
 * Contains port-dependent routines for SNMPv1 Agent.
 * This file is used if SNMP_SOCKETS is disabled. That is,
 * - When SNMP_SOCKETS is enabled, v1sock.c is used. v1port is not used.
 * - When SNMP_SOCKETS is disabled, v1sock.c is not used. v1port is used.
 *
 * Here is a list of port-dependent files that need to be modified for
 * porting SNMPv1.
 * SNMP Core - snmp\snmpport.c, snmpport.h
 * SNMPv1    - snmpv1\v1port.c, snmpv1\v1sock.c 
 *
 * 2/15/2002 - Moved from snmp\snmpport.c to snmpv1\v1port.c -AT-
 *
 * MODULE: SNMP
 *
 * ROUTINES: v1_init(), snmp_upc(), v1_udp6_upcall(), v1_cleanup(), 
 *
 * PORTABLE: no
 */


/* general includes */

#include "ipport.h"

#ifdef INCLUDE_SNMP

#ifndef SNMP_SOCKETS
/* this file uses lightweight API to UDP, socketes implementation is
 * in snmpsock.c 
 */
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"
#include "udp.h"

#include "snmpport.h"

#include "asn1.h"
#include "npsnmp.h"
#include "snmp_imp.h"

#include "in_utils.h"
#ifdef INCLUDE_NVPARMS
#include "nvparms.h"
extern int snmp_nvram_init();
#endif   /* INCLUDE_NVPARMS */

u_char snmpbuf[SNMPSIZ]  =  {0};  /* global buffer for packet building */
int    agent_on =  TRUE;    /* TRUE if agent is active  */

#ifdef SNMP_TEST_SP_TRAP
extern void snmp_test_sp_trap(int version);
#endif

/* If SNMPv3 is enabled, then it processes all IPv4/IPv6 packets.
 * So enable IPv6 support only when SNMPv3 is not used.
 */
#if (!defined(INCLUDE_SNMPV3) && defined(IP_V6))
#include "udp.h"
UDPCONN v1u6_conn = NULL;
#define V1U6_DATA  ((void*)0x01234567)  /* tag to pass to udp6_open() */
int     v1_udp6_upcall(PACKET pkt, void * data); 
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
   snmp_nvram_init();   /* update SNMP paras with those read from nv file */
#endif

   /* by default, enable SNMP traps - increment value from 0 to 1 */
   SNMP_MIB_CTR(snmpEnableAuthenTraps);  /* 1=enabled, 2=disabled. */

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
#ifdef ENABLE_SNMP_TRAPS

   /* NOTE: a separate trap buffer is provided.
    */
   trap_buffer_len = SNMPSIZ;

#ifdef SNMP_TEST_SP_TRAP
   /* Trap uses 1 buffer to send a trap. So if there are multiple
    * traps, we need to send them at intervals */
   snmp_test_sp_trap(SNMP_VERSION_1);
#else    /* Test generic trap */
   snmp_trap(SNMP_TRAP_COLDSTART, 0, 0, NULL,SNMP_VERSION_1);
#endif

#else
   /* disable SNMP traps - increment value from 1 to 2 */
   SNMP_MIB_CTR(snmpEnableAuthenTraps);  /* 1=enabled, 2=disabled. */
#endif   /* not ENABLE_SNMP_TRAPS */
#endif /* INCLUDE_SNMPV1 or INCLUDE_SNMPV2C */

#if (!defined(INCLUDE_SNMPV3) && defined(IP_V6))
   /* Install callback to receive requests over UDPv6 */

   v1u6_conn = udp6_open(0L,    /* wildcard foriegn host */
      0,                        /* any foreign port */
      SNMP_PORT,                /* local port to listen on */
      v1_udp6_upcall,              /* callback to receive incoming requests */
      V1U6_DATA);               /* for sanity check */

   if (!v1u6_conn)
      return ENP_RESOURCE;
#endif

   return(0);     /* return OK code */
}


#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))

/* FUNCTION: snmp_upc()
 * 
 * snmpupc() - upcalled by NetPort UDP layer when it detects an SNMP 
 * packet. The packet is run throught snmp and a reply is generated 
 * if appropriate. Since these agents may be daisychained, NOSUCH 
 * responses can be supressed as a runtime option and the 
 * ENP_NOT_MINE code returned to the stack, which may then hand the 
 * packet to another agent, or, ultimatly, another IP stack attached 
 * to an NDIS or ODI driver. Returns: 0 if we processed the packet 
 * OK, ENP_NOT_MINE if it was not for us and we aren't generating 
 * NOSUCHs, or a negative error code from the ENP_ series. 
 *
 * PARAM1: PACKET p
 * PARAM2: unshort lport
 *
 * RETURNS: 
 */

int
snmp_upc(PACKET p, unshort lport)      /* packet with nb_prot set to SNMP data */
{
   u_char * data;
   int   e;
   ip_addr fhost;       /* host (who sent us the packet) */
   int   len;        /* scratch lenth hold */

   data = (u_char*)p->nb_prot;
   len    = p->nb_plen;
   fhost = p->fhost;

#ifdef NPDEBUG
   if (NDEBUG & UPCTRACE)
   {
      dprintf("snmp: got packet len %d from %lx\n", len,fhost);
      if (NDEBUG & DUMP)
      {
         hexdump(NULL,data, len);
         dprintf("\n\r");
      }
   }
#endif

   if (agent_on)  /* agent is active */
   {
      LOCK_NET_RESOURCE(SNMP_RESID);
      e = snmp_agt_parse(data, len, snmpbuf, SNMPSIZ);
   }
   else     /* agent set inactive by application, just bail out */
      return ENP_NOT_MINE; /* udp will send dest unreachable & free pkt */

   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);       /* free received packet, alloc another if we send */
   UNLOCK_NET_RESOURCE(FREEQ_RESID);

   if (e)      /* parse worked, send response packet */
   {
      len = e;    /* length of response packet */
      p = udp_alloc((unsigned)len,0);   /* get packet for sending */
      if (!p)
      {
         UNLOCK_NET_RESOURCE(SNMP_RESID);
         return(ENP_NOBUFFER);
      }

      /* build & send response packet */
      MEMCPY(p->nb_prot, snmpbuf, len);
      UNLOCK_NET_RESOURCE(SNMP_RESID);
      p->nb_plen = len;
      p->fhost = fhost;
      udp_send(lport, 161, p);
      SNMP_MIB_CTR(snmpOutPkts);
      return 0;
   }
   else        /* parse failed */
   {
      UNLOCK_NET_RESOURCE(SNMP_RESID);
      SNMPERROR("snmpagent(): snmp_agt_parse failed\n");
      return ENP_BAD_HEADER;
   }
}

/* FUNCTION: v1_udp6_upcall()
 *
 * v1_udp6_upcall() - SNMPv3 agent UDP callback. Called from stack 
 * whenever we get a SNMP request on UDPv6. Returns 0 or error code. 
 * 
 * PARAM1: PACKET pkt
 * PARAM2: void * data
 *
 * RETURNS: If the processing was sucessfull, the packet is freed and 0 is 
 * returned. Otherwise the packet is NOT free'ed and an error code is 
 * returned
 */

#if (!defined(INCLUDE_SNMPV3) && defined(IP_V6))

int
v1_udp6_upcall(PACKET p, void * data)
{
   struct sockaddr_in6 fhost;
   struct udp *pup = (struct udp *)(p->nb_prot - sizeof(struct udp));
   u_short src_port = pup->ud_srcp;
   u_char * data;
   int   e;
   int   len;        /* scratch lenth hold */

   if (data != V1U6_DATA)
   {
      dtrap();
      return ENP_LOGIC;    /* internal logic error */
   }

   data = (u_char*)p->nb_prot;
   len    = p->nb_plen;
   fhost->sin6_family = AF_INET6;
   IP6CPY(&fhost->sin6_addr, &p->ip6_hdr->ip_src);

#ifdef NPDEBUG
   if (NDEBUG & UPCTRACE)
   {
      dprintf("snmp: got packet len %d from %lx\n", len,fhost);
      if (NDEBUG & DUMP)
      {
         hexdump(NULL,data, len);
         dprintf("\n\r");
      }
   }
#endif

   if (agent_on)  /* agent is active */
   {
      LOCK_NET_RESOURCE(SNMP_RESID);
      e = snmp_agt_parse(data, len, snmpbuf, SNMPSIZ);
   }
   else     /* agent set inactive by application, just bail out */
      return ENP_NOT_MINE; /* udp will send dest unreachable & free pkt */

   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);       /* free received packet, alloc another if we send */
   UNLOCK_NET_RESOURCE(FREEQ_RESID);

   if (e)      /* parse worked, send response packet */
   {
      len = e;    /* length of response packet */
      p = udp6_alloc((unsigned)len,0);   /* get packet for sending */
      if (!p)
      {
         UNLOCK_NET_RESOURCE(SNMP_RESID);
         return(ENP_NOBUFFER);
      }

      /* build & send response packet */
      MEMCPY(p->nb_prot, snmpbuf, len);
      UNLOCK_NET_RESOURCE(SNMP_RESID);
      p->nb_plen = len;
      udp6_send(&fhost->sin6_addr,lport, 161, p);
      SNMP_MIB_CTR(snmpOutPkts);
      return 0;
   }
   else        /* parse failed */
   {
      UNLOCK_NET_RESOURCE(SNMP_RESID);
      SNMPERROR("snmpagent(): snmp_agt_parse failed\n");
      return ENP_BAD_HEADER;
   }
}

#endif /* IP_V6 */


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
v1_cleanup(void)
{
  return 0;
}

#endif /* INCLUDE_SNMPV1 or INCLUDE_SNMPV2C */


#endif   /* not SNMP_SOCKETS */

#endif /*  INCLUDE_SNMP */

/* end of file v1port.c */



