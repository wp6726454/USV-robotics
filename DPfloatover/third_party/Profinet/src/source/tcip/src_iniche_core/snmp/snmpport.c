/*
 * v3.1 August 30, 2011 snmp/snmpport.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: snmpport.c
 *
 * Copyright 1997-2004 By InterNiche Technologies Inc. All rights reserved
 *
 * Contains port-dependent routines common to all SNMP Agents.
 *
 * Here is a list of port-dependent files 
 * SNMP Core - snmp\snmpport.c, snmpport.h
 * SNMPv1    - snmpv1\v1port.c, snmpv1\v1sock.c 
 * SNMPv2c   - n/a (uses SNMPv1 port)
 * SNMPv3    - snmpv3\v3port.h, snmpv3\v3port.c  snmpv3\v3portau.c
 *
 * 2/15/2002 - Moved SNMPv1 porting stuff to snmpv1\v1port.c -AT-
 *
 * MODULE: SNMP
 *
 * ROUTINES: snmp_init(), snmp_cleanup(), snmp_check(), 
 * ROUTINES: SNMPERROR(), GetUptime(), tk_snmp(), 
 * ROUTINES: snmp_listen_sock(), snmp_listen_sock6(), 
 * ROUTINES: snmp_send_trap(), ip_myaddr(), 
 *
 * PORTABLE: no
 */

#define LTRC_ACT_MODUL_ID 18 /* INICHE_TCIP_ITGR added for LSA traces */

#include "snmpport.h"
#include "npsnmp.h"
#include "snmp_imp.h"

#ifdef INCLUDE_SNMP

#ifndef SUPERLOOP
#ifndef OSPORT_H
#error Need to define OSPORT_H
#endif
/*INICHE_TCIP_ITGR #include OSPORT_H*/
#endif   /* SUPERLOOP */

#ifdef MIB_COUNTERS
struct snmp_mib   SnmpMib;
#endif

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
extern int  v1_init(void);
extern void v1_cleanup(void);
extern void v1_check(void);
#endif 

#ifdef INCLUDE_SNMPV3
extern int   v3_init(void);
extern void  v3_cleanup(void);
extern void v3_check(void);
#endif   /* INCLUDE_SNMPV3 */

#include "syslog.h"

#ifdef IP_V6
#include "socket6.h"
#endif /* SNMP_SOCKETS */


/* FUNCTION: snmp_init()
 * 
 * Common init routine for all snmp agents.
 * Call v1_init() to initialize SNMPv1, SNMPv2c.
 * Call v3_init() to initialize SNMPv3.
 *
 * PARAM1: 
 *
 * RETURNS: 0 on success, else error code.
 *
 */

int 
snmp_init(void)
{
   int e=0;

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
   e = v1_init();
   if (e != SUCCESS)
   {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
      TCIP_INICHE_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
              "Error %d initializing SNMPv1 agent.",e);
#else
      dprintf("Error %d initializing SNMPv1 agent.\n",e);
#endif
      return e;
   }
#endif   /* INCLUDE_SNMPV1 || INCLUDE_SNMPV2C */

#ifdef INCLUDE_SNMPV3
   e=v3_init();
   if ( e != SUCCESS )
   {
      dprintf("Error %d starting SNMPv3 agent.\n",e);
      return e;
   }
#endif
   return e;
}

/* FUNCTION: snmp_cleanup()
 * 
 * Cleanup all resources used by SNMP.
 * This function should be called during system shutdown.
 * Call v1_cleanup() for SNMPV1, SNMPv2c.
 * Call v3_cleanup() for SNMPV3.
 *
 * PARAM1: 
 *
 * RETURNS: 
 */

void
snmp_cleanup(void)
{
#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
   v1_cleanup();
#endif 
#ifdef INCLUDE_SNMPV3
   v3_cleanup();
#endif
}

/* FUNCTION: snmp_check()
 * 
 * Handle SNMP activity.
 * Process all pending requests and send the responses.
 *
 * If SNMPv3 is used, then call v3_check(). It will
 * process all SNMPv1, SNMPv2c, SNMPv3 requests.
 * If SNMPv3 is not used, then call v1_check(). It will
 * process all SNMPv1, SNMPv2c requests. 
 *
 * PARAM1: 
 *
 * RETURNS: 
 */

void 
snmp_check(void)
{
#ifdef INCLUDE_SNMPV3
      v3_check();       /* give agent some cycles, may block */
#else
      v1_check();       /* give agent some cycles, may block */
#endif
}


/* FUNCTION: GetUptime()
 * 
 * GetUptime() - reutrn a value suitable for the sysUptime variable 
 * in the mib-2 system group. 
 *
 * PARAM1: 
 *
 * RETURNS: 
 */

u_long
GetUptime(void) /* INICHE_TCIP_ITGR void added */
{
   return((cticks*100L)/TPS);
}


#ifndef SNMPERROR_ALREADY /* already have error handler in system */

char *   snmp_err_text  =  NULL;
#ifdef NPDEBUG /* INICHE_TCIP_ITGR #ifdef added, see AP01537961 */
extern   unsigned NDEBUG;  /* Debugging options */
#endif
#ifndef UPCTRACE             /* defined in net.h */
#define UPCTRACE    0x400    /* trace upcall progress */
#endif

/* FUNCTION: SNMPERROR()
 * 
 * SNMPERROR(msg) - This is a NON-FATAL error handler. Appropriate 
 * behaviour seems to be to send message msg to console (if we have 
 * one) or log to file, etc. Is meant to be #defined in as 
 * SNMPERROR(string). 
 *
 * PARAM1: char * msg
 *
 * RETURNS: 
 */

int 
SNMPERROR(char * msg)
{
#ifdef NPDEBUG
   if (NDEBUG & UPCTRACE)
   {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "SNMP ERROR: XX" /* AP01366119 \n removed */
				);
#else
      dprintf("SNMP ERROR: %s\n", msg);
#endif
#ifdef INICHE_SYSLOG
      syslog(LOG_WARNING|LOG_LOCAL1, "SNMP ERROR: %s",msg);
#endif
   }
#endif   /* NPDEBUG */
   snmp_err_text = msg;       /* leave for polling routine to print */
   return -1;
}
#endif   /* SNMPERROR_ALREADY */

#if 0 /*INICHE_TCIP_ITGR check SNMP port in component SOCK*/
#ifndef SUPERLOOP

TK_OBJECT(to_snmp);
TK_ENTRY(tk_snmp);
long     snmp_wakes =  0;

struct inet_taskinfo snmptask = {
      &to_snmp,
      "snmp agent",
      tk_snmp,
      NET_PRIORITY,
      APP_STACK_SIZE,
};

/* The application thread works on a "controlled polling" basis:
 * it wakes up periodically and polls for work.
 */

/* FUNCTION: tk_snmp()
 * 
 * PARAM1: n/a
 *
 * RETURNS: n/a
 */

TK_ENTRY(tk_snmp)
{
   int e=0;
   while (!iniche_net_ready)
      TK_SLEEP(1);

   e = snmp_init();
   if (e == SUCCESS)
      exit_hook(snmp_cleanup);
   else
      dprintf("Error %d initializing SNMP agent.\n",e);

   for (;;)
   {
      snmp_check();     /* give agent some cycles, may block */
      TK_SLEEP(1);      /* let system run */
      snmp_wakes++; /* count wakeups */

      if (net_system_exit)
         break;
   }
   TK_RETURN_OK();
}

#endif   /* SUPERLOOP */
#endif


#if 0 /*INICHE_TCIP_ITGR create snmp socket in component SOCK*/
#ifdef SNMP_SOCKETS

/* FUNCTION: snmp_listen_sock()
 * 
 * Open a IPv4 listen socket for the mentioned port.
 *
 * It starts a IPv4 listen on a socket and returns it.
 *
 * PARAM1: none
 *
 * RETURNS: 
 * SUCCESS or error code.
 */


SOCKTYPE
snmp_listen_sock(int port, SOCKTYPE *lsock)
{
   struct sockaddr_in sin;
   int e;   /* error holder */
   SOCKTYPE listensock;

   listensock = socket(AF_INET, SOCK_DGRAM, 0);
   if (listensock == INVALID_SOCKET)
      return -1;

   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = INADDR_ANY;
   sin.sin_port = htons(port); /* port swapped for intel */

   /* After doing bind(), we can send/receive on UDP socket */
   e = bind(listensock, (struct sockaddr*)&sin, sizeof(sin));
   if (e != 0)
   {
      e = t_errno(listensock);
      dprintf("error %d binding v3 agent\n", e);
      return -2;
   }

#ifdef INICHE_TCPIP
   /* make it a non-blocking socket */
   setsockopt(listensock, SOL_SOCKET, SO_NBIO, NULL,0);
#else
   /* WSAAsyncSelect() informs windows about "what message to send to
    * which window when a packet arrives" */
   e=WSAAsyncSelect(listensock,ViewhWnd,WM_V3_MSG,FD_READ);
   if (e != 0)
   {
      e = WSAGetLastError();
      dprintf("error %d WSAAsyncSelect() v3 agent\n", e);
      return -3;
   }
#endif   /* INICHE_TCPIP */

   *lsock = listensock;
   return SUCCESS;
}


/* FUNCTION: snmp_listen_sock6()
 * 
 * Open a IPv6 listen socket for the mentioned port.
 *
 * It starts a IPv6 listen on a socket and returns it.
 *
 * PARAM1: none
 *
 * RETURNS: 
 * SUCCESS or error code.
 */


#ifdef IP_V6
SOCKTYPE
snmp_listen_sock6(int port, SOCKTYPE *lsock)
{
struct sockaddr_in6 sin6;
int e;   /* error holder */
SOCKTYPE listensock = INVALID_SOCKET;

   /* start up an V3 port listen */
   listensock = socket(AF_INET6, SOCK_DGRAM, 0);
   if (listensock == INVALID_SOCKET)
      return -1;

   IP6CPY(&sin6.sin6_addr, &ip6unspecified) ;
   sin6.sin6_family = AF_INET6;
   sin6.sin6_port = htons(port); /* port swapped for intel */

   /* After doing bind(), we can send/receive on UDP socket */
   e = bind(listensock, (struct sockaddr *)&sin6, sizeof(sin6));
   if (e != 0)
   {
      e = t_errno(listensock);
      dprintf("error %d binding v3 agent\n", e);
      return -2;
   }

   /* make it a non-blocking socket */
   setsockopt(listensock, SOL_SOCKET, SO_NBIO, NULL,0);

   *lsock = listensock;
   return SUCCESS;
}
#endif /* IP_V6 */


/* FUNCTION: snmp_send_trap()
 * 
 * Send a SNMP TRAP out the UDP TRAP port on InterNiche IP stack.
 * Used by SNMPv1/SNMPv2c if ENABLE_SNMP_TRAPS is enabled.
 * Used by SNMPv3 if V3_USE_NOTIFY is enabled.
 *
 * Supports both IPv4 and IPv6.
 *
 * If TRAP port number is not present in addr, then use
 * the default of SNMP_TRAP_PORT.
 *
 * When SNMP_UDP_TRAPS is defined, traps are sent over UDP
 * to send standard v1/v2c targets. If some other transport is to be used
 * for sending trap such as IPX or Appletalk,  then implement it here.
 *
 * PARAM1: u_char * data           - data to be sent
 * PARAM2: int      len            - length of data to be sent
 * PARAM3: struct sockaddr *addr   - address of remote host/mgr
 *
 * RETURNS: 
 * 0 or error code.
 */


int
snmp_send_trap(u_char * data, int len, struct sockaddr * addr)
{
#ifdef SNMP_UDP_TRAPS /* send UDP datagram on TRAP port */
SOCKTYPE trap_sock;
int e=0;

   trap_sock = socket(addr->sa_family, SOCK_DGRAM, 0);
   if(trap_sock == INVALID_SOCKET)
      return -1;

   if (addr->sa_family == AF_INET)
   {
      if (!((struct sockaddr_in *)addr)->sin_port)
         ((struct sockaddr_in *)addr)->sin_port = htons(SNMP_TRAP_PORT);
      e = sendto(trap_sock, (char*)data, len, 0, addr, 
             sizeof(struct sockaddr_in));
   }
#ifdef IP_V6
   else
   {
      if (((struct sockaddr_in6 *)addr)->sin6_port)
         ((struct sockaddr_in6 *)addr)->sin6_port = htons(SNMP_TRAP_PORT);
      e = sendto(trap_sock, (char*)data, len, 0, addr, 
             sizeof(struct sockaddr_in6));
   }
#endif

   socketclose(trap_sock);

   if(e != len)
      return ENP_SENDERR;
   else
      return 0;
#endif   /* SNMP_UDP_TRAPS */
}

#else /* SNMP_SOCKETS not defined, use LIGHTWEIGHT API */


int
snmp_send_trap(u_char * data, int len, struct sockaddr * addr)
{
#ifdef SNMP_UDP_TRAPS /* send UDP datagram on TRAP port */
   PACKET pkt;

   if (addr->sa_family == AF_INET)
   {
      struct sockaddr_in *sin;
      sin = (struct sockaddr_in *)addr;
   
      pkt = udp_alloc(len,0);
      if (!pkt)
         return ENP_NOMEM;
      pkt->nb_plen = len;
      
      memcpy(pkt->nb_prot, data, len);
      pkt->fhost = sin->sin_addr.s_addr;
   
      /* underlying stack will free the packet once its sent */
      /* udp_send() expects ports in host endian, not network endian */
      return udp_send(htons(sin->sin_port), SNMP_TRAP_PORT, pkt);
   }
#ifdef IP_V6
   else if (addr->sa_family == AF_INET6)
   {
      struct sockaddr_in6 *sin;
      sin = (struct sockaddr_in6 *)addr;
   
      pkt = udp6_alloc(len,0);
      if (!pkt)
         return ENP_NOMEM;
      pkt->nb_plen = len;
      memcpy(pkt->nb_prot, data, len);
   
      /* underlying stack will free the packet once its sent */
      /* udp_send() expects ports in host endian, not network endian */
      return udp6_send(&sin->sin6_addr, htons(sin->sin6_port), 
         SNMP_TRAP_PORT, pkt);
   }
#endif
   else
   {
      return -1;  /* address family not supported */
   }
   
#endif   /* SNMP_UDP_TRAPS */
}

#endif   /* else of SNMP_SOCKETS */
#endif

/* FUNCTION: ip_myaddr()
 * 
 * Find out the local IPv4 address.
 * That is, IPv4 address of the interface on which this packet
 * will go.
 *
 * For IPv6 address, we return 0.
 * Porting effort is required to find out the IPv4 address
 * corresponding to a IPv6 address.
 *
 * PARAM1: struct sockaddr *addr 
 *
 * RETURNS: 
 * 0 or error code.
 */

ip_addr ip_myaddr(struct sockaddr *ipaddr)
{
   ip_addr ipad = 0;
   if (ipaddr->sa_family == AF_INET)
   {
      struct sockaddr_in * addr = (struct sockaddr_in *)ipaddr;
      ipad = ip_mymach(addr->sin_addr.s_addr);
      
   }
   return ipad;
}

/*
 *Function Name:convert_oid()
 *
 *Parameters:
 *
 *Description:
 *
 *Returns:
 *
 */
int convert_oid(char *s, unsigned long *oid)
{
    char *cp = s;
    int len  = 0;

    while (*cp)
    {
        oid[len++] = atoi(cp);
        while ((*cp) && (*cp != '.'))
        {
            cp++;
        }
        if (*cp == '.')
        {
            cp++;
        }
    }
    return(len);
}  /* convert_oid() */

#if 0 /*INICHE_TCIP_ITGR no trap*/
#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
int setup_snmpv1_trap(int trap_num, char *ip_addr, char *community)
{
    extern unsigned long convert_ip(const char *p);
    struct sockaddr_in *addr;
    if(trap_num < 0 || trap_num >= MAX_TRAP_TARGETS)
    {
        return(-1);
    }
    addr = (struct sockaddr_in *)&(trap_targets[trap_num].ipaddr);
    /* You may need to swap bytes here -- TBD */
    addr->sin_addr.s_addr = convert_ip((const char *)ip_addr);
    strcpy(trap_targets[trap_num].community, community);
    trap_targets[trap_num].commLen = strlen(community);
    trap_targets[trap_num].ipaddr.sa_family = AF_INET;
    return(0);
}

int snmp_enterprise_trap(char *oid_val, int specificType, int specificVarCount, struct trapVar *specificVars, int version)
{
   extern oid      trap_sys_id[MAX_OID_LEN];
   extern unsigned trap_sys_id_len ;
   extern int      trap_sys_id_flag;
   int status       = 0;
   trap_sys_id_flag = 1;
   trap_sys_id_len  = convert_oid(oid_val, trap_sys_id);
   status = snmp_trap(SNMP_TRAP_ENTERPRISESPECIFIC, specificType, specificVarCount, specificVars, version);
   return(status);
}
#endif

#endif   /*  INCLUDE_SNMP */
#endif

/* end of file snmpport.c */



