#ifndef _IPPORT_H_
#define _IPPORT_H_ 1

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/* ipport.h
 *
 * Copyright 2001 by InterNiche Technologies, Inc.
 *
 * All the platform specific defines for each port go here. 
 * All these should be re-examined with each new port of the code.
 *
 * This file for:
 *   IBC32 board w/ 100 Mbit Ethernet 
 *   Tasking TriCore toolchain
 *   TriCore AMOS
 *   (ThreadX is default)
 *
 */

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
/*  F i l e               &F: ipport.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration of the InterNiche stack based on tcip_cfg.h                */
/*                                                                           */
/*****************************************************************************/

//#define IN_UTILS_H /* "in_utils.h" is non-core, hide content */
#if 0 /* see 972718 */
#define HUGE /* otherwise compiler error, see in_utils.h */
#endif
#include "tcip_inc.h" /* the configuration of the "port" */

#include "nptypes.h"

/*===========================================================================*/
/*=== INTERNICHE-DEFINES DERIVED FROM TCIP-DEFINES ==========================*/
/*===========================================================================*/

#if ! defined(TCIP_CFG_MAX_SOCKETS) || defined(FD_SETSIZE)
#error "check this!"
#endif

#define FD_SETSIZE  TCIP_CFG_MAX_SOCKETS /* make all sockets selectable */

/*===========================================================================*/

#if ! defined(TCIP_CFG_MAXARPS) || defined(MAXARPS)
#error "check this!"
#endif

#define MAXARPS  TCIP_CFG_MAXARPS

/*===========================================================================*/

#ifdef ARP_AGEOUT
#error "ARP_AGEOUT is obsolete and must not be defined"
#endif

/*===========================================================================*/

#if ! defined(TCIP_CFG_SOMAXCONN) || defined(SOMAXCONN)
#error "check this!"
#elif TCIP_CFG_SOMAXCONN < 1
#error "check this!"
#endif

#if TCIP_CFG_SOMAXCONN != 0
#define SOMAXCONN  TCIP_CFG_SOMAXCONN
#endif

/*===========================================================================*/

#if defined TCIP_CFG_IP_MAX_MEMBERSHIPS || defined IP_MAX_MEMBERSHIPS
#error "check this!"
#endif

#define IP_MAX_MEMBERSHIPS  1 /* INICHE_TCIP_ITGR only one possible */

#ifndef TCIP_CFG_ENABLE_MULTICAST
#error "check this!"
#endif

#if defined IP_MULTICAST || defined IGMP_V1 || defined IGMP_V2 || defined IGMP_V3
#error "check this!"
#endif

#if TCIP_CFG_ENABLE_MULTICAST
#define IP_MULTICAST    1  /* support end node IP multicast */
#define IGMP_V1
#define IGMP_V2            /* IGMP support */
#define ETHMCAST           /* see et_arp.c */
#endif

/*===========================================================================*/

#ifdef SYS_SERVICES
#error "SYS_SERVICES was renamed to TCIP_CFG_SYS_SERVICES"
#endif

#ifndef TCIP_CFG_SYS_SERVICES
#error "check this!"
#elif (TCIP_CFG_SYS_SERVICES & ~0x7F) != 0
#error "check this!"
#endif

/*===========================================================================*/

#if ! defined(TCIP_CFG_SOC_CHECK_ENABLE) || ! defined(TCIP_CFG_SOC_CHECK_MORE) || defined(SOC_CHECK_ALWAYS)
#error "check this!"
#endif

#if TCIP_CFG_SOC_CHECK_MORE && ! TCIP_CFG_SOC_CHECK_ENABLE
#error "check this!"
#endif

#if TCIP_CFG_SOC_CHECK_MORE != 0
#define SOC_CHECK_ALWAYS /* INICHE_TCIP_ITGR map TCIP to InterNiche */
#endif

#if TCIP_CFG_SOC_CHECK_ENABLE != 0
#define SOC_CHECK(so) /* check with protection (compare to sockcall.h) */ \
   {  struct socket * tmp; \
      LOCK_NET_RESOURCE(NET_RESID); /*INICHE_TCIP_ITGR */ \
      for(tmp = (struct socket *)(&soq); tmp; tmp = tmp->next) \
      {  if(tmp == so) break; } \
      if(tmp != so) \
      { \
       UNLOCK_NET_RESOURCE(NET_RESID); /*INICHE_TCIP_ITGR */ \
       dtrap(); \
         return SOCKET_ERROR; \
      } \
      UNLOCK_NET_RESOURCE(NET_RESID); /*INICHE_TCIP_ITGR */ \
    }
#else
#define SOC_CHECK(so)
#endif

#define  SOC_RANGE(so)  /* null - this version doesn't need range setup */

/*---------------------------------------------------------------------------*/

#if ! defined(TCIP_CFG_QUEUE_CHECK_ENABLE)
#error "check this!"
#elif TCIP_CFG_QUEUE_CHECK_ENABLE != 0
#define QUEUE_CHECKING  1  /* include code to check critical queues */
#endif

/*===========================================================================*/

#if ! defined(TCIP_CFG_NUMBIGBUFS) || ! defined(TCIP_CFG_NUMLILBUFS)
#error "check this!"
#elif defined(NUMBIGBUFS) || defined(NUMLILBUFS) || defined(MAXPACKETS)
#error "check this!"
#endif

#define NUMBIGBUFS  TCIP_CFG_NUMBIGBUFS
#define NUMLILBUFS  TCIP_CFG_NUMLILBUFS
#define MAXPACKETS  (NUMBIGBUFS + NUMLILBUFS) /* see pkalloc.c */

/*===========================================================================*/

#if ! defined(TCIP_CFG_NPDEBUG) || defined(NPDEBUG)
#error "check this!"
#endif

#if TCIP_CFG_NPDEBUG != 0
#define NPDEBUG  1
#endif

/*===========================================================================*/
/*=== INTERNICHE-DEFINES WHICH ARE FIXED FOR TCIP ===========================*/
/*===========================================================================*/

#define ETHHDR_BIAS  0 /* must be 0 for EDD */

#define ALIGN_TYPE  4 /* 32 bit alignment for EDD */

/*===========================================================================*/

#if defined(BIGBUFSIZE) || defined(LILBUFSIZE)
#error "check this!"
#endif

//TODO die 1568 sind legacy! Woher sie kommen ist unklar!
#define BIGBUFSIZE  1568 /* AP00168784 mind. EDD_FRAME_BUFFER_LENGTH + TCPIPHDRSZ + HDRSLEN */
#define LILBUFSIZE  128

#if (BIGBUFSIZE % ALIGN_TYPE) != 0 || (LILBUFSIZE % ALIGN_TYPE) != 0
#error "check this!"
#endif

/*===========================================================================*/

/* use IANA "dynamic or private ports" */

#define	MINSOCKET  49152 /* note: this define is for UDP only, TCP already used IPPORT_RESERVED */

/*===========================================================================*/

/* AP00981331: STRICT_SUBNETTING must be enabled */

#define STRICT_SUBNETTING

/*===========================================================================*/

/* AP00285731: check interface gateway address against interface IP address */

#ifdef TCPIP_CHECK_GATEWAY_ADDRESS
#error "TCPIP_CHECK_GATEWAY_ADDRESS was renamed to TCIP_CFG_CHECK_GATEWAY_ADDRESS"
#endif

#ifdef TCIP_CFG_CHECK_GATEWAY_ADDRESS
#error "check this!"
#endif

#define TCIP_CFG_CHECK_GATEWAY_ADDRESS /* must be enabled */

/*===========================================================================*/

/* AP00270759: option to align tcp/ip headers instead of data */

#ifdef TCPIP_ALIGN_HEADERS
#error "TCPIP_ALIGN_HEADERS was renamed to TCIP_CFG_ALIGN_HEADERS"
#endif

#ifdef TCIP_CFG_ALIGN_HEADERS
#error "check this!"
#endif

#define TCIP_CFG_ALIGN_HEADERS /* must be enabled because of EDD (see ETHHDR_BIAS too) */

/*===========================================================================*/

/* AP00371263: enable support of VLAN tagged ethernet frames (IEEE 802.1 Q) */

#ifdef TCPIP_VLAN
#error "TCPIP_VLAN was renamed to TCIP_CFG_VLAN"
#endif

#ifdef TCIP_CFG_VLAN
#error "check this!"
#endif

#define TCIP_CFG_VLAN /* must be enabled */

/*===========================================================================*/

/* AP00244803: option to ignore ping broadcast request */

#ifdef TCPIP_IGNORE_PING_BROADCAST_REQUEST
#error "TCPIP_IGNORE_PING_BROADCAST_REQUEST was renamed to TCIP_CFG_IGNORE_PING_BROADCAST_REQUEST"
#endif

#ifdef TCIP_CFG_IGNORE_PING_BROADCAST_REQUEST
#error "check this!"
#endif

#define TCIP_CFG_IGNORE_PING_BROADCAST_REQUEST /* must be enabled */

/*===========================================================================*/

/* AP00376450: allow compression of socket receive mbufs */

#ifdef TCPIP_COMPRESS_RCV_MBUFS
#error "TCPIP_COMPRESS_RCV_MBUFS was renamed to TCIP_CFG_COMPRESS_RCV_MBUFS"
#endif

/* AP01265298: make configurable */

#ifndef TCIP_CFG_COMPRESS_RCV_MBUFS
#error "check this!"
#endif

/*===========================================================================*/
/*=== OBSOLETED DEFINES =====================================================*/
/*===========================================================================*/

#ifdef TCPIP_MAX_TIMER
#error "TCPIP_MAX_TIMER is obsolete"
#endif

/*===========================================================================*/
/*=== GLUE BETWEEN INTERNICHE AND TCIP ======================================*/
/*===========================================================================*/

extern unsigned long cticks; /* see tcip_iniche_ctick() */
extern struct queue  mfreeq;    /* A netport queue for free mbufs */
extern int           agent_on;  /* Internal SNMP Agent On/Off */

void netmain_init(void); /* see tcip_iniche_setup() */

void inet_timer(void); /* see tcip_iniche_ctick() */

void tcip_iniche_recv_done(void * pkt); /* see pk_free() */

void tcip_iniche_mib2_written(void * snmp_data); /* see parse_var_op_list() */

void tcip_iniche_panic(char *file, int line); /* cannot use TCIP_FATAL(), needs TCIP_MODULE_ID */

/*===========================================================================*/
/*===========================================================================*/

/*ipport.h******************************/

/**** INICHE_TCIP_ITGR Options ********************************************/

#define TCPIP_RST_ON_SYN_ON 1 /* turn on sending a RST on a SYN frame, see comment of AP01436969 */

/**** NicheStack Options **************************************************/

#define INCLUDE_TCP     1  /* this link will include NetPort TCP w/MIB */
#define INCLUDE_ARP     1  /* use Ethernet ARP */
#define IP_FRAGMENTS    1  /* IP fragment reassembly */
#define MULTI_HOMED     1  /* more than one net interface */
#define NB_CONNECT      1  /* support Non-Blocking connects (TCP, PPP, et al) */
#define DYNAMIC_IFACES  1  /* Support runtime creation of interfaces */
#define C_CHECKSUM      1  /* build portable C Internet checksum routine */
#define DO_DELAY_ACKS   1  /* TCP ACK piggybacking */
#define DOS_RST         1  /* Denial of Service (DOS) attack using RST */
#define DOS_SYN         1  /* Denial of Service (DOS) attack using SYN */
#define PANIC_ALREADY   1  /* Use special panic method */
#define IP_ROUTING      1  /* do real IP routing, not imbedded agent fake route */
#define NATIVE_SPRINTF  1  /* Use sprintf */
#define NATIVE_PRINTF   1  /* use target build environment's printf function */



#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define DHCP_CLIENT     1  /* include DHCP client code */
#define DNS_CLIENT      1  /* include DNS client code */
#define FULL_ICMP       1  /* use all ICMP || ping only */ /* INICHE_TCIP_ITGR ping only, see AP01332334 */

#define BOOTPTAB        1  /* DHCP supports a UNIX-ish bootptab file */
#define BTREE_ROUTES    1  /* keep IP routes in btree for fast lookup */
#define NO_UDP_CKSUM    1  /* don't do UDP checksums */
#define TCP_ECHOTEST    1  /* include a TCP Echo test */
#define TCP_SACK        1  /* Selective ACK per rfc2018 */
#define TCP_TIMESTAMP	1  /* RFC1323 TCP timestamp feature */
#define TCP_WIN_SCALE	1  /* RFC1323 scaling TCP window feature */
#define TCP_ZEROCOPY    1  /* enable zero-copy Sockets extension */
#define BLOCKING_APPS   1  /* use select() in FTP, WebPort, Telnet */
#define DNS_CLIENT_UPDT 1  /* do Dynamic DNS Update */
#define IEEE_802_3      1  /* Support IEEE 802.3 (RFC-1042) */
#define IP_PMTU         1  /* do path MTU discovery */
#define IP_RAW          1  /* build raw sockets support */
#define IP_RAW_ACCESS   1   /* allow direct-receive of IP datagrams */
#define LOSSY_IO        1  /* sabotage packet IO for testing */
#define RAWIPTEST       1   /* Test for raw sockets */
#define ROUTE_TEST      1  /* Pseudo device to test IP/NAT routing */
#define SHARED_IPADDRS  1   /* Add code for one IP address on two ifaces */
#define SUPPORT_SO_FULLMSS   1   /* Socket option for MAXS sized tcp segments */
#define UDP_ECHO_SVR    1  /* built-in UDP Echo server */
#define UDPSTEST        1  /* UDP Echo Sockets test app */
#define USE_IPFILTER    1  /* Use IP Filtering (as per rfc2669) */
#define USE_SLIP        1  /* support SLIP driver iface(s) */
#define HEAPBUFS        1     /* support heap-based buffers */
#define HEAPBUFS_DEBUG  1     /* debug support for heapbufs */

#define USER_SOCKETNAMES 1 /* Map plain BSD socket routine names to Interniche t_" names */

#endif


/**** AllPorts Options *********************************************************/

#define INICHE_TIMERS   1   /* Provide Interval timers */

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define TK_STDIN_DEVICE   1   /* Include stdin (uart) console code */

#endif /* NOT_USED */


/**** MiscLib Options *********************************************************/

#define MEM_WRAPPERS    1  /* heap debug & diagnostic tools */

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define IN_MENUS        1  /* support for InterNiche menu system */
#define NET_STATS       1  /* include statistics printfs */
#define PING_APP        1  /* application provides a pingUpcall */
#define PING_REQ_OUTDEV 1   /* PING stops if console/socket unavailable */
#define INCLUDE_NVPARMS 1  /* non-volatile (NV) parameters logic */
#define MEM_BLOCKS      1  /* list memory heap stats */
#define PKT_CYCLES      1   /* Include Code to measure Packet Cycles */
#define HEAP_STATS      1  /* include the heap statistics menu */
#define PFDEBUG         1   /* Enable profiler debug code */
#define RF_SIMULATION   1   /* simulate wireless networks */
#define TESTMENU        1  /* include optional testing menu */

#endif

/**** SNMP Options *********************************************************/

#define SNMP_SOCKETS    1  /* SNMP over Sockets, not lightweight API */
#define INCLUDE_SNMPV1  1  /* SNMPv1 library, agent & hook */

/* if any SNMP agent is used, then INCLUDE_SNMP should be enabled */
#if (defined(INCLUDE_SNMPV1)||defined(INCLUDE_SNMPV2C)||defined(INCLUDE_SNMPV3))
#define INCLUDE_SNMP    1  /* update SNMP counters in TCPIP stack */
#endif

#ifdef INCLUDE_SNMP
#ifndef SNMP_SOCKETS
#define PREBIND_AGENT 1  /* hardcode SNMP port into UDP */
#endif
#endif

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define INCLUDE_SNMPV2C 1  /* SNMPv2c (community based SNMPv2) agent */
#define INCLUDE_SNMPV3  1  /* SNMPv3 library, agent & hook */

#endif


/**** Configuration *********************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define NPDEBUG         1  /* turn on debugging dprintf()s */
#define SUPERLOOP       1  /* InterNiche superloop system */
#define INICHE_TASKS    1  /* InterNiche multitasking system */

#endif


/**** "net" Options *********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define MAC_LOOPBACK    1  /* use MAC layer pseudo-Ethernet loopback driver */
#define QUEUE_CHECKING  1  /* include code to check critical queues */

#endif


/**** AutoIP Options *********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define USE_UPNP        1  /* Universal Plug & Pray */
#define USE_AUTOIP      1  /* Automatic IPv4 address aquisition */

#endif


/**** FTP Options *********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define FTP_SERVER      1  /* include FTP server code */
#define FTP_CLIENT      1  /* include FTP client code */
#define MAXSENDLOOPS    50  /* MAX number of FTP server send loops */
#define FTP_IDLECONN_TMO 1

#endif


/**** PPP Options *********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define PPP_DHCP_CLIENT 1  /* get IP address via dhcp after IPCP is up */
#define PPP_DNS         0  /* domain nameserver address negotiation */  /* Not allowed to set this define !!! */
#define CHAP_SUPPORT    1  /* CHAP authentication */
#define PPP_CHARIO      1  /* support  character read/write routines */
#define PPP_LOGFILE     1  /* ConPrintf() log to file option */
#define PPP_MENU        1  /* include the PPP debug menu */
#define PPP_PACKETS     0  /* support packet oriented read/write routines */  /* Not allowed to set this define !!! */
#define PPP_VJC         0  /* VJ header compression */  /* Not allowed to set this define !!! */
#define USE_COMPORT     1  /* use nvparms COMPORT numbering  */
#define USE_MODEM       0  /* support PPP/SLIP on modems */  /* Not allowed to set this define !!! */
#define USE_PPP         0  /* support PPP MAC iface(s) */  /* Not allowed to set this define !!! */
#define MSCHAP_SUPPORT  0  /* Microsoft MS-CHAP authentication */  /* Not allowed to set this define !!! */
#define LB_XOVER   1   /* cross 2 loopback lines (units 1 & 2) for test */
#define PPP_OLDFORMS    0  /* Use old (IP-Addresses) option */  /* Not allowed to set this define !!! */
#define PPP_MULTILINK   0  /* PPP Multilink (RFC1990) support */  /* Not allowed to set this define !!! */
#endif


/**** PPPoE Options *********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define USE_PPPOE   1   /* Support PPP over ethernet */

#endif /* NOT_USED */


/**** Syslog Options ******************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define INICHE_SYSLOG   1  /* support syslog client */
#define USE_SYSLOG_QUEUE 1  /* use queue for syslogs */
#define USE_SYSLOG_TASK  1  /* use task (not timer) for syslogs */

#endif /* NOT_USED */


/**** Telnet Options *********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define TELNET_SVR      1  /* include Telnet server code */

#endif


/**** TFTP Options ***********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define TFTP_CLIENT     1  /* include TFTP client code */
#define TFTP_SERVER     1  /* include TFTP server code */

#endif


/**** VFS Options ************************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define VFS_FILES       1  /* include Virtual File System */
#define VFS_UNITTEST    1  /* include VFS test code */
#define VFS_VERBOSE     1  /* VFS printf debugging support */

#endif


/**** Niche-Lite Options *****************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define MINI_IP         1   /* Use Nichelite mini-IP layer */
#define MINI_TCP        1   /* Use Nichelite mini-TCP layer */
#define MINI_TCP_OOSQ   1   /* Handle Out of Sequence TCP Packets */
#define BSDISH_RECV     1   /* Include a BSD recv()-like routine with mini_tcp */
#define BSDISH_SEND     1   /* Include a BSD send()-like routine with mini_tcp */
#define MINI_PING       1   /* Build Light Weight Ping App for Niche Lite */
#define INCLUDE_ICMP    1   /* Include ICMP || ping only */

#endif


/**** Misc Options ***********************************************************/

#ifdef NOT_USED
#error "NOT_USED" should not be defined

#define PRINTF_STDARG   1  /* C compiler supports VARARGS */
#define HT_LOCALFS      1  /* layer VFS on target system's local file system */
#define USE_PROFILER    1  /* profile selected code modules */
#define WEBPORT         1  /* will be linked with WebPort Web (HTTP) Server */
#define HT_USERAUTH     1  /* TRUE to include user authorization code */
#define HT_PERSISTENT   1  /* enable persistent connections in HTTP 1.1 */
#define HTML_COMPRESSION     1 /* TRUE to include HTML tags compression logic */
#define FOREIGN_LANG_SUPPORT 1 /* TRUE to compress around high bit chars */
#define SERVER_PUSH     1  /* Web server "push" code */
#define HT_MD5AUTH      1  /* TRUE to include MD5 code */
#define ENABLE_SSL      1  /* enable SSL for a web server or a browser */
#define USE_CRYPTOENG   1  /* enables cipher menu commands and cipher engine */
#define REMOVE_SSLPRINTF 1 /* Remove SSL printf Messages */
#define IPSEC           1  /* IPSEC */
#define IKE             1  /* IKE */
#define USE_INT_ISDIGIT 1  /* use internal defined isdigit function */
#define POP3_CLIENT     1  /* include POP3 client API code */
#define P3C_APOP        1  /* include POP3 client APOP supporting code */
#define PAP_SUPPORT     1  /* Password authentication */
#define TCP_CIPHERTEST  1  /* include a TCP Cipher test */
#define HT_FILEUPLOAD   1  /* allow file upload via HTML FORMs */
#define NATRT           1  /* enable NAT routing feature */
#define DHCP_SERVER     1  /* bring in DHCP server */
#define SMTP_ALERTS     1  /* send Emailer trouble alerts */
#define IP_V6           1  /* support IPv6 */
#define LINKED_PKTS     1  /* scatter/gather packets */
#define DNS_SERVER      1  /* include DNS server code */
#define RIP_SUPPORT     1  /* enable RIP routing feature */
#define DNS_CLIENT      1  /* include DNS client code */
#define NO_HTMLCOMP     1  /* Build WebServer without HTML Compiler */

#endif


/* Global variables */

#define MUTE_WARNS      1  /* gen extra code to suppress compiler warnings */

/* macros to get rid of "unused argument" warnings. With compilers that
 * can suppress these warnings, define this to nothing.
 */

#define USE_ARG(x)  {x = x;}
#define USE_VOID(x) {x = x;}

/*****************************************************************************/

#ifdef C_CHECKSUM   
#define cksum ccksum
#endif

#if (!defined(USE_CRYPTOENG) && defined(TCP_CIPHERTEST))
#error TCP_CIPHERTEST should not be defined if USE_CRYPTOENG is not used
#endif

#ifdef USE_CRYPTOENG
#define CRYPTENG_COMMANDS 10
#endif

/*****************************************************************************/

#if defined(NO_INET_STACK) || defined(NO_INET_TICK)
#error "check this!"
#elif defined(INICHE_TASKS) || defined(INICHE_SYSLOG)
#error "check this!"
#elif defined(_TASK_H_) || defined(TK_NTASK_H) /* see "task.h" and "tk_ntask.h" */
#error "check this!"
#elif defined(SUPERLOOP)
#error "check this!"
#endif

#define NO_INET_STACK /* no tk_netmain() */
#define NO_INET_TICK  /* no tk_nettick() */
#undef INICHE_SYSLOG  /* no syslog() */
#undef INICHE_TASKS   /* no interniche tasking system */
#undef SUPERLOOP      /* not using superloop method */

/*****************************************************************************/
/*      CRUN time functions                                                  */
/*****************************************************************************/

#define MEMCPY(dst,src,size)  TCIP_MEMCPY((dst),(src),(size))  
#define MEMSET(ptr,val,size)  TCIP_MEMSET((ptr),(val),(size))  
#define MEMMOVE(dst,src,size) TCIP_MEMMOVE((dst),(src),(size)) 
#define MEMCMP(p1,p2,size)    TCIP_MEMCMP((p1),(p2),(size))   

#ifndef INICHE_LIBS   /* use Standard libs strlib (strcat, strcpy, etc) */
//#include <stdlib.h>   /* for atoi(), exit() */
//#include <string.h>
#endif

#define PORT_EXIT_FUNC(code)  dtrap()  /* INICHE_TCIP_ITGR */


/* default setups of some sub-options */

#ifdef VFS_FILES
#define HT_EXTDEV     1  /* allow custom file devices (memdev, null, etc.) */
#define HT_RWVFS      1  /* support read/write VFS */
#define VFS_STRIPPATH 1  /* used to strip path for filenames in vfsfiles.c */

#ifdef NOTDEF   /* unused VFS options */
#define UNIX_VFS      1  /* make VFS look UNIX-like */
#define HT_SYNCDEV    1  /* support sync to flash device */
#endif /* NOTDEF */

#define vfs_lock()       /* define pesky macros away */
#define vfs_unlock()
#endif /* VFS_FILES */


#ifdef MULTI_HOMED
#if 0 /* INICHE_TCIP_ITGR -- allow MULTI_HOMED without IP_ROUTING  */
#define IP_ROUTING    1 /* force IP routing on multi-homed compiles */
#endif
#endif

#ifdef INCLUDE_TCP
#if 0 /* INICHE_TCIP_ITGR ping only, see AP01332334 */
#define FULL_ICMP     1  /* force full ICMP for TCP support */
#endif
#endif

#ifdef  IP_TTL           /* defined in a Microsoft C Compiler file */
#undef  IP_TTL           /* will be redefined */
#endif  

#define IP_TTL        30 /* define IP hop count for this port, INICHE_TCIP_ITGR changed from 64 to 30 */

#ifndef MINI_IP

#if 1 /* INICHE_TCIP_ITGR */
#define STATIC_NETS   1 /* Loopback */
#define TCIP_NETS     (EDD_CFG_MAX_INTERFACE_CNT + EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE)
#define MAXNETS       (STATIC_NETS + TCIP_NETS)  /* max ifaces to support at one time */
#endif

#else

#define MAXNETS       1  /* NicheLite only supports one network interface */

#endif  /* MINI_IP */

#define NAT_SUPPRESS_WARNINGS	1

#ifdef NOTDEF
#define MDM_DTRRESET  1  /* use PC's DTR line to reset modem */
#endif

#ifdef IP_FRAGMENTS

#if 1 /* INICHE_TCIP_ITGR */
#ifdef MAX_FRAG_PKTS
#error "check this!"
#endif
#define MAX_FRAG_PKTS 42 /* INICHE_TCIP_ITGR */
#endif

#ifndef MAX_FRAG_PKTS
#define MAX_FRAG_PKTS 5   max IP pkts we can simultaneously reassemble
#endif /* MAX_FRAG_PKTS */

#endif /* IP_FRAGMENTS */

/* number of entries in IP routing table */
#define RT_TABS      (EDD_CFG_MAX_INTERFACE_CNT + 1) /* INICHE_TCIP_ITGR: number of interfaces + 1 gateway; see also AP01332803 */

#define STK_TOPDOWN   1  /* Intel x86 "top down" stack */

#ifndef TRUE
#define TRUE   LSA_TRUE /* INICHE_TCIP_ITGR */
#endif

#ifndef FALSE
#define FALSE  LSA_FALSE /* INICHE_TCIP_ITGR */
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif   /* NULL */

void clock_init(void);   /* start clock tick counting; called at init time */
void clock_c(void);      /* undo effects of clock_init (i.e. restore ISR vector */

#define TPS      10   /* cticks per second, INICHE_TCIP_ITGR set to 10  */

#define PLLISTLEN 2  /* INICHE_TCIP_ITGR set to 2 (ARP and IP) */

#ifdef MINI_IP
/* permit shutdown of the NicheLite console-mode Win32 application */
void exit_win32(int);
#define  netexit   exit_win32
#endif /* MINI_IP */


/* define Ethernet header size -- this is usually sizeof(struct ethhdr)
 * or the size in bytes of the Ethernet header (and must be the size
 * in bytes for NicheLite) but may be increased to force alignment of
 * the subsequent data (e.g. IP header, ARP header)
 */
#ifndef MINI_IP
#if 0 /* INICHE_TCIP_ITGR let ether.h define it */
#define ETHHDR_SIZE  (sizeof(struct ethhdr))
#endif
#else
#define ETHHDR_SIZE  (14)
#endif

/* INICHE_TCIP_ITGR definitions for endianness 
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321 
extern unsigned long lswap(unsigned long l);  swap bytes in 32 bit long */

#if TCIP_CFG_BIG_ENDIAN == 1 /* INICHE_TCIP_ITGR AP01266753 */

/* big endian target equal to network byte order in ip -> no conversion needed */
#define lswap(x) (x)

#define htonl(l) (l)
#define ntohl(l) (l)

#define htons(s) (s)
#define ntohs(s) (s)

#else 

/* little endian target not equal to network byte order in ip -> conversion needed */

/* convert little/big endian - these should be efficient, 
 * inline code or MACROs
 */

#define lswap(x) ((((x) & 0xff000000) >> 24) | /*INICHE_TCIP_ITGR*/\
                  (((x) & 0x00ff0000) >>  8) | \
                  (((x) & 0x0000ff00) <<  8) | \
                  (((x) & 0x000000ff) << 24))
#define htonl(l) lswap(l)
#define ntohl(l) lswap(l)

/* Amazingly, some compilers really need all these u_short casts: */
#define htons(s) ((u_short)((u_short)(s) >> 8) | (u_short)((u_short)(s) << 8))
#define ntohs(s) htons(s)

#endif

/* A portable macro to check whether a ptr is within a certain range.
 * In an environment where selectors are important, this macro will
 * be altered to reflect that.
 */
 
#define IN_RANGE(p1, len1, p2) \
   (((p1) <= (p2)) && (((p1) + (len1)) > (p2)))
 
#if 0 /* INICHE_TCIP_ITGR */
#pragma warning(disable: 4244 4310 4055 4054 4127) /* stifle idiot warnings */
#endif


char * npalloc(unsigned);  /* alloc & return zeroed out buffer */
void   npfree(void*);      /* not required on embedded systems */

/* define the various IP stack block and buffer allocation routines */
#define  RT_ALLOC(size)   npalloc(size)     /* route block alloc */
#define  RT_FREE(ptr)     npfree(ptr)
#define  NB_ALLOC(size)   npalloc(size)     /* netbuf structure alloc */
#define  NB_FREE(ptr)     npfree(ptr)


#ifndef TCIP_CFG_COPY_ON_SEND
#error "check this!"
#elif TCIP_CFG_COPY_ON_SEND

#define  BB_ALLOC(size)   npalloc(size)     /*INICHE_TCIP_ITGR Big packet buffer alloc */
#define  BB_FREE(ptr)     npfree(ptr)       /*INICHE_TCIP_ITGR */
#define  LB_ALLOC(size)   npalloc(size)     /*INICHE_TCIP_ITGR Little packet buffer alloc */
#define  LB_FREE(ptr)     npfree(ptr)       /*INICHE_TCIP_ITGR*/

#else /* the buffers must be accessible by the ethernet controller */

char * npalloc_ethmem(unsigned);  /* alloc & return memory that is accessible by the ethernet controller */
void   npfree_ethmem(void*);      /* free memory allocated with npalloc_ethmem() */

#define  BB_ALLOC(size)   npalloc_ethmem(size)
#define  BB_FREE(ptr)     npfree_ethmem(ptr)
#define  LB_ALLOC(size)   npalloc_ethmem(size)
#define  LB_FREE(ptr)     npfree_ethmem(ptr)

#endif /* TCIP_CFG_COPY_ON_SEND */


#define  UC_ALLOC(size)   npalloc(size)     /* UDP connection block alloc */
#define  UC_FREE(ptr)     npfree(ptr)

#if 0 /* TCIP does not use misclib/task.c */
#define  TK_ALLOC(size)   npalloc(size)     /* task control block */
#define  TK_FREE(ptr)     npfree(ptr)
#define  STK_ALLOC(size)  npalloc(size)     /* task stack */
#define  STK_FREE(ptr)    npfree(ptr)
#endif

#define  IPR_ALLOC(size)  npalloc(size)
#define  IPR_FREE(ptr)    npfree(ptr)

#ifdef DYNAMIC_IFACES
#define  NET_ALLOC(size)  npalloc(size)     /* struct net */
#define  NET_FREE(ptr)    npfree(ptr)
#define  IFM_ALLOC(size)  npalloc(size)     /* struct IfMib */
#define  IFM_FREE(ptr)    npfree(ptr)
#endif   /* DYNAMIC_IFACES */

#ifdef IP_PMTU
#define  PMTU_ALLOC(size) npalloc(size)     /* path MTU cache */
#define  PMTU_FREE(ptr)   npfree(ptr)
#endif   /* IP_PMTU */

/* NetPort stack generic error codes: generally full success is 0,
 * definite errors are negative numbers, and indeterminate conditions
 * are positive numbers. These may be changed if they conflict with
 * defines in the target system. They apply to the NetPort IP stack,
 * the IPX logic, and the portable SNMP Agent. If you have to change
 * these values, be sure to recompile ALL NetPort sources.
 */

#define SUCCESS          0 /* whatever the call was, it worked */

/* programming errors */
#define ENP_PARAM      -10 /* bad parameter */
#define ENP_LOGIC      -11 /* sequence of events that shouldn't happen */
#define ENP_NOCIPHER   -12 /* No corresponding cipher found for the cipher id */

/* system errors */
#define ENP_NOMEM      -20 /* malloc or calloc failed */
#define ENP_NOBUFFER   -21 /* ran out of free packets */
#define ENP_RESOURCE   -22 /* ran out of other queue-able resource */
#define SEND_DROPPED ENP_RESOURCE /* full queue or similar lack of resource */
#define ENP_BAD_STATE  -23 /* TCP layer error */
#define ENP_TIMEOUT    -24 /* TCP layer error */

#define ENP_NOFILE     -25 /* expected file was missing */
#define ENP_FILEIO     -26 /* file IO error */

/* net errors */
#define ENP_SENDERR    -30 /* send to net failed at low layer */
#define ENP_NOARPREP   -31 /* no ARP for a given host */
#define ENP_BAD_HEADER -32 /* bad header at upper layer (for upcalls) */
#define ENP_NO_ROUTE   -33 /* can't find a reasonable next IP hop */
#define ENP_NO_IFACE   -34 /* can't find a reasonable interface */
#define ENP_HARDWARE   -35 /* detected hardware failure */

/* conditions that are not really fatal OR success: */
#define ENP_SEND_PENDING 1 /* packet queued pending an ARP reply */
#define ENP_NOT_MINE     2 /* packet was not of interest (upcall reply) */

/* ARP holding packet while awaiting a response from fhost */
#define ARP_WAITING   ENP_SEND_PENDING

#define panic(msg) tcip_iniche_panic(__FILE__, __LINE__) /* ignore "msg", the constant string can be looked up in the sources */
#define dtrap()    tcip_iniche_panic(__FILE__, __LINE__)

#define _NPPP            4 /* override ppp_port.h setting */

/*#define NO_CC_PACKING    1  CPU/Compiler combo can't pack structs */

/* ...thus the structs we'd normally define packed are normal */
#define START_PACKED_STRUCT(sname) struct sname {
#define END_PACKED_STRUCT(sname)  };

/* stack supports two methods of protecting its queues and data
 * structs from destructive re-entries by ISRs, multiple tasks, etc. One
 * is to lock a "resource ID" which makes code wait when trying to
 * access shared resource, the other is a CRITICAL SECTION frame. You
 * should use one or the other, not both. See manual for details.
 */

#ifdef INICHE_TASKS
/* 
 * Use the resource locking routines, and map LOCK/UNLOCK_NET_RESOURCE 
 * to them with macros.  Similar to the above case, we use macros to 
 * provide null definitions for ENTER/EXIT_CRITICAL_SECTION.
 */
void LOCK_NET_RESOURCE(int res);
void UNLOCK_NET_RESOURCE(int res);

/* 
 * also define macros for these so that tk_thrdx.h doesn't try to use its
 * own prototypes 
 */
#define LOCK_NET_RESOURCE(x)    LOCK_NET_RESOURCE(x)
#define UNLOCK_NET_RESOURCE(x)  UNLOCK_NET_RESOURCE(x)

/*void ACQUIRE_CE_RESOURCE(int res);
void RELEASE_CE_RESOURCE(int res);*/

/* 
 * also define macros for these so that tk_thrdx.h doesn't try to use its
 * own prototypes 
 */
/*#define ACQUIRE_CE_RESOURCE(x)    ACQUIRE_CE_RESOURCE(x)
#define RELEASE_CE_RESOURCE(x)  RELEASE_CE_RESOURCE(x)*/
/* Remember that in a real multitasking you have to define */
/* either the ENTER_... or the LOCK_.. functions           */
#define ACQUIRE_CE_RESOURCE(x)
#define RELEASE_CE_RESOURCE(x)

#define ENTER_CRIT_SECTION(p); 
#define EXIT_CRIT_SECTION(p);

#else /* INICHE_TASKS */

#ifndef LOCK_NET_RESOURCE /* INICHE_TCIP_ITGR AP01358814 (no prototype if macro) */
void LOCK_NET_RESOURCE(int res);
void UNLOCK_NET_RESOURCE(int res);
#endif

#define ENTER_CRIT_SECTION(p)  /* empty */
#define EXIT_CRIT_SECTION(p)   /* empty */

#endif /* INICHE_TASKS */

/* #define PID port resource IDs as indexes into net-resources[] array */
#define NET_RESID   0    /* stack code resource lock */
#define RXQ_RESID   1    /* received packet queue resource lock */
#define FREEQ_RESID 2    /* free packet queue resource lock */
/* #define CE_RES_ID	3 */

#ifdef VFS_FILES
#define vfs_lock()
#define vfs_unlock()
#endif

#define OSPORT_H  /* INICHE_TCIP_ITGR empty */

#ifdef INICHE_TASKS    /* InterNiche multitasking system */
#include "task.h"      /* NicheTask definitions */

/* RETURN_XXX from task routines: */
void  w32_tk_return(int error);

/* the "return(parm)" is only in this to suppress Compiler warnings */
#define TK_RETURN_OK()     { w32_tk_return(0); return(parm); }  
#define TK_RETURN_ERROR()  w32_tk_return(-1)

#include "tk_ntask.h"	/* Nichetask TK_MACRO definitions */

#define tcp_wakeup(ev)  tk_ev_wake(ev)



#else
/* define SUPERLOOP specific things here */

/* Port routine/macro to wake packet demuxer task after receives have
 * been placed in rcvdq by drivers. Define to NULL for Superloop DOS demo
 */

#if 1 /* INICHE_TCIP_ITGR */

void  SignalPktDemux(void);

#define pre_task_setup()   NULL
char * post_task_setup(void);

#define  TK_THIS         0
#define  tk_yield()      /* empty */
#define  TK_APP_WAKE(ev) /* empty */
#define  TK_WAKE(task)   /* empty */
#define  TK_BLOCK()      /* empty */
#define  TK_SLEEP(tks)   tcip_iniche_panic(__FILE__, __LINE__) /* don't use select() for sleeping */

#define TCPWAKE_ALREADY 1  /*INICHE_TCIP_ITGR use per-port sleep, wakeup */

#ifndef tcp_sleep /* INICHE_TCIP_ITGR AP01358814 (no prototype if macro) */
void tcp_sleep(void * event);
void tcp_wakeup(void * event);
#endif

#endif

#endif /* INICHE_TASKS */


#if 0 /* INICHE_TCIP_ITGR */

/* NetPort printf */
#ifndef _IN_TTYIO_
void dprintf(char * format, ...);
#endif

/* net stats printf with redirectable device: */
int ns_printf (void * pio, char * format, ...);

/* Send startup errors to the right place */
#define initmsg dprintf

#define COMMSIZE   64   /* max bytes in community string */

#define INETNET     0   /* index into nets[] for internet iface */
#define LOCALNET    1   /* index into nets[] for local net iface */

void dputchar(int chr);	/* Windows port putchar substitute */

#ifdef putchar
#undef putchar

#define putchar(_c) dputchar(_c)

#ifdef NATIVE_PRINTF
#define dprintf printf
#else
/* map printfs into device IO system */
#define printf dprintf
#endif
#endif

#endif


int prep_ppp(int);               /* in ..\ppp\sys_np.c */
int prep_ifaces(int firstIface); /* set up interfaces */


/* definitions for the BSD TCP code */
#ifndef LITTLE_ENDIAN /* INICHE_TCIP_ITGR AP01266753 */
#define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN /* INICHE_TCIP_ITGR AP01266753 */
#define BIG_ENDIAN    4321 
#endif

#if LITTLE_ENDIAN==BIG_ENDIAN /* INICHE_TCIP_ITGR AP01266753 */
#error "check this: LITTLE_ENDIAN must not be equal to BIG_ENDIAN"
#endif

/* define the byteordering your cpu uses here: */
#if TCIP_CFG_BIG_ENDIAN == 1 /* INICHE_TCIP_ITGR AP01266753 */
#define BYTE_ORDER    BIG_ENDIAN
#else
#define BYTE_ORDER    LITTLE_ENDIAN
#endif

#define MAX_NVSTRING   128       /* MAX length of an nvparms string */


#if 0 /*INICHE_TCIP_ITGR*/
/* for Microsoft/Intel, define structure pack pragmas away */
#define START_INPACK(name)
#define END_INPACK(name)
#endif

#ifdef MINI_IP
/* pull in some NicheLite definitions */
#include "minip.h"
#endif /* MINI_IP */

struct in6_addr;   /* predecl */
void ip6cpy(struct in6_addr * dest, struct in6_addr * src);
#define IP6CPY(a,b) ip6cpy(a,b)

int ip6eq(struct in6_addr * dest, struct in6_addr * src);
#define IP6EQ(a,b) ip6eq(a,b)

/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* _IPPORT_H_ */

/* end of file ipport.h */
