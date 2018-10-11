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
/*  F i l e               &F: tcip_var.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Initialization of Interniche's global data                               */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  8
#define TCIP_MODULE_ID     8

#include "tcip_int.h"

#include "arp.h"
#include "tcp_timr.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/

LSA_INT32  tcip_ConnectionTimeout;
LSA_INT32  tcip_RetransmitCntMax;
LSA_INT32  tcip_RetransmitTimeout;

/*===========================================================================*/

/*iface.c*/
#ifndef PLLISTLEN
#error "check this"
#endif
extern unshort  protlist[PLLISTLEN]; /* zero means unused entry */

/*asn1.c*/
/*dnsclnt.c*/
#ifdef DNS_CLIENT    
extern unshort  dnsids;  /* seed for unique request IDs */

/* DNS client statistics: */
extern ulong      dnsc_errors;      /* protocol/implementation runtime errors */
extern ulong      dnsc_requests;    /* requests sent */
extern ulong      dnsc_replies;     /* replys received */
extern ulong      dnsc_good;        /* usable replys received */
extern ulong      dnsc_tmos;        /* timeouts */
extern ulong      dnsc_retry;       /* total retries */

#ifdef DNS_CLIENT_UPDT
extern ulong      dnsc_updates;     /* Dynamic DNS Updates sent */
#endif /* DNS_CLIENT_UPDT */

/* pending requests, can be used as a flag to spin dnsc_check() task */
extern unsigned   dnsc_active;

/* retry info, can be overwritten by application code */
extern unsigned dns_firsttry; /* time to first retry, in seconds */
extern unsigned dns_trys;     /* max number of retrys */

extern ip_addr dns_servers[MAXDNSSERVERS]; /* need to be set from NV parameters */

#ifdef  DNS_CLIENT_UPDT
extern char    soa_mname[MAXDNSNAME];
#endif  /* DNS_CLIENT_UPDT */
#endif  /* dns_client */

/*et_arp.c*/
extern struct arptabent *   arpcache;   /* last ARP entry used */
extern struct arptabent  arp_table[MAXARPS];     /* the actual table */

#ifdef IEEE_802_3
#endif
/*icmp.c*/

/* the callback for application to get a peek at icmp du packets */
#ifdef FULL_ICMP
#ifdef NPDEBUG
#endif   /* NPDEBUG */
#endif

/*iface.c*/
extern queue netlist;          /* master list of nets, static & dynamic */
/*in_pcb.c*/
/*ip.c*/
#ifdef IP_ROUTING
#endif   /* IP_ROUTING */
/**ip_reasm.c*/
#ifdef IP_FRAGMENTS
#include <ip_reasm.h>
extern u_long ipr_curr_mem;
/* head pointer to IP reassembly entry (IRE) queue */
extern IREP h_ireq;
/* next invocation of timer tick processing routine */
extern u_long ire_cticks;
#endif
/*ipnet.c*/
#ifdef  LOSSY_IO
/* support for an option that allows us to deliberatly loose packets */
#endif   /* LOSSY_IO */
extern queue    rcvdq;         /* queue of all received (but undemuxed) packets */
/*ipport.c*/
extern int(*port_prep)(int already_found);
/*ipstart.c*/
extern NET   nets[MAXNETS];
extern unsigned ifNumber;        /* number of net interfaces */
/*extern void (*closers[NUMCLOSERS])(void); Define only local */
extern int nclosers;
/*nptcp.c*/
#ifdef INCLUDE_SNMP
#else
#endif
#ifdef TCP_WIN_SCALE
/* default value for window scaling option */
#endif   /* TCP_WIN_SCALE */
/* protocol switch table entry, for TCP: */
#ifdef UDP_SOCKETS
#endif   /* UDP_SOCKETS */
#ifdef NET_STATS
#endif   /* NET_STATS */
extern struct queue   soq;     /* A socket queue */
extern struct queue   mbufq;   /* A netport queue for in-use mbufs */
extern struct queue   mfreeq;  /* A netport queue for free mbufs */

#ifdef MULTI_HOMED
#endif   /* MULTI_HOMED */
/*parseip.c*/

/*pktalloc.c*/
extern queue   bigfreeq;    /* big free buffers */
extern queue   lilfreeq;    /* small free buffers */

extern unsigned lilbufs;      /* number of small bufs to init */
extern unsigned lilbufsiz;    /* big enough for most non-full size packets */
extern unsigned bigbufs;      /* number of big bufs to init */
extern unsigned bigbufsiz;    /* big enough for max. ethernet packet */

#ifdef NPDEBUG
#endif
/*q.c*/
/*rfc1213.c*/
#ifdef INCLUDE_SNMP
extern char     sys_descr[SYS_STRING_MAX];
extern char     sysContact[SYS_STRING_MAX];
extern char     sysLocation[SYS_STRING_MAX];
extern char     sysName[SYS_STRING_MAX];
#endif
/*snmp.c*/
/*snmp_age.c*/
#if (defined(INCLUDE_SNMP) || defined(INCLUDE_SNMPV3))
/* these can't be global in a re-entrant system */
#endif
/*snmp_aut.c*/
/*snmpport.c*/
#if (defined(INCLUDE_SNMP) || defined(INCLUDE_SNMPV3))
/*extern   u_char    varVal[MAX_TRAP_VAR]; Define only local*/ /* the actual variable data */
#endif

/*trap_out.c*/
#if (defined(INCLUDE_SNMP) || defined(INCLUDE_SNMPV3))
#ifdef ENABLE_SNMP_TRAPS   /* whole file is within this ifdef */
#ifdef V1_IP_SNMP_TRAPS
/* trap related agent values from mib2 system group */
/* static   oid   sysDescrOid[] = {1, 3, 6, 1, 2, 1, 1, 1, 0}; */
#endif
#endif
#endif
/*udp.c*/
/*udp_open.c*/
extern UDPCONN firstudp;

/*===========================================================================*/
/*                               code                                        */
/*===========================================================================*/

void tcip_iniche_init_globals(void)
{
	/*iface.c*/
	TCIP_MEMSET(protlist, 0, sizeof(protlist));

	/*et_arp.c*/
	arpcache = (struct arptabent *)NULL;   /* last ARP entry used */
	TCIP_MEMSET(arp_table, 0, sizeof(arp_table));     /* the actual table */
	/*iface.c*/
	TCIP_MEMSET(&netlist, 0, sizeof(netlist));          /* master list of nets, static & dynamic */
	TCIP_MEMSET(&rcvdq, 0, sizeof(rcvdq));         /* queue of all received (but undemuxed) packets */

	/*ipport.c*/
	port_prep = NULL;
	TCIP_MEMSET(nets, 0, sizeof(nets));
	ifNumber = 0;        /* number of net interfaces */
	nclosers = 0;
	TCIP_MEMSET(&soq, 0, sizeof(soq));     /* A socket queue */
	TCIP_MEMSET(&mbufq, 0, sizeof(mbufq));   /* A netport queue for in-use mbufs */
	TCIP_MEMSET(&mfreeq, 0, sizeof(mfreeq));  /* A netport queue for free mbufs */
	/*pktalloc.c*/
	TCIP_MEMSET(&bigfreeq, 0, sizeof(bigfreeq));    /* big free buffers */
	TCIP_MEMSET(&lilfreeq, 0, sizeof(lilfreeq));    /* small free buffers */

	lilbufs = NUMLILBUFS;      /* number of small bufs to init */
	lilbufsiz = LILBUFSIZE;    /* big enough for most non-full size packets */
	bigbufs = NUMBIGBUFS;      /* number of big bufs to init */
	bigbufsiz = BIGBUFSIZE;    /* big enough for max. ethernet packet */

#ifdef IP_FRAGMENTS  /* whole file can be ifdeffed out */
	/* number of bytes currently used by the reassembly process - this is the sum of
	 * the lengths of the buffers used in the reassembly process */
	ipr_curr_mem = 0;
	/* head pointer to IP reassembly entry (IRE) queue */
	h_ireq = 0;
	/* next invocation of timer tick processing routine */
	ire_cticks = 0;
#endif

/* set the default timing params: */
	tcp_keepidle   =  TCPTV_KEEP_IDLE;
	tcp_keepintvl  =  TCPTV_KEEPINTVL; 

	tcip_ConnectionTimeout = (15*PR_SLOWHZ);
	tcip_RetransmitCntMax  = 5;
	tcip_RetransmitTimeout = (64*PR_SLOWHZ);
    
	sys_descr[0] = 0;
	sysContact[0] = 0;
	sysLocation[0] = 0;
	sysName[0] = 0;



#if (defined(INCLUDE_SNMP) || defined(INCLUDE_SNMPV3))
#include SNMPMIB_H_FILE
	SnmpMib.snmpEnableAuthenTraps = 2; /* AP 850537 auth.traps are disabled */
#endif

	firstudp = NULL;

#ifdef DNS_CLIENT    /* ifdef out whole file */
	dnsids   =  0x1234;  /* seed for unique request IDs */

	/* DNS client statistics: */
	dnsc_errors = 0;      /* protocol/implementation runtime errors */
	dnsc_requests = 0;    /* requests sent */
	dnsc_replies = 0;     /* replys received */
	dnsc_good = 0;        /* usable replys received */
	dnsc_tmos = 0;        /* timeouts */
	dnsc_retry = 0;       /* total retries */
	#ifdef DNS_CLIENT_UPDT
	dnsc_updates = 0;     /* Dynamic DNS Updates sent */
	#endif /* DNS_CLIENT_UPDT */
	/* pending requests, can be used as a flag to spin dnsc_check() task */
	dnsc_active = 0;
	/* retry info, can be overwritten by application code */
	dns_firsttry = 4; /* time to first retry, in seconds */
	dns_trys = 5;     /* max number of retrys */
	TCIP_MEMSET(dns_servers, 0, sizeof(dns_servers));
	#ifdef  DNS_CLIENT_UPDT
	TCIP_MEMSET(soa_mname, 0, sizeof(soa_mname));
	#endif  /* DNS_CLIENT_UPDT */
#endif  /* dns_client */

}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
