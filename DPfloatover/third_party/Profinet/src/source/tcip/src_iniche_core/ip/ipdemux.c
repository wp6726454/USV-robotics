/*
 * v3.1 August 30, 2011 ip/ipdemux.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: ipdemux.c
 *
 * Copyright 1997- 2001 By InterNiche Technologies Inc. All rights reserved
 *
 *
 * MODULE: INET
 *
 * ROUTINES: ip_rcv(), ip_demux(), ip_stats(),
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */
/* Portions Copyright 1990-1996 by NetPort Software. 
 * Portions Copyright 1986 by Carnegie Mellon 
 * Portions Copyright 1984,1985 by the Massachusetts Institute of 
 * Technology 
 */

#define LTRC_ACT_MODUL_ID 23 /* INICHE_TCIP_ITGR added for LSA traces */

#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"

#ifdef IP_MULTICAST
#include "../ipmc/igmp.h"
#ifdef USE_IGMPV3
#include "nptcp.h"
#include "msf.h"
#endif /* USE_IGMPV3 */
#endif

#ifdef INCLUDE_TCP
#define  TCP_PROT 6
extern   int tcp_rcv(PACKET pkt);
#endif

#ifdef USE_IPFILTER
int ipf_filter(PACKET p, int dir);
#endif

#ifdef NATRT
extern unsigned char do_nat (PACKET p
#ifdef IP_FRAGMENTS
                             , PACKET ** pktpp,
                             unsigned short int * pkt_countp
#endif
);
extern void nat_free (void * ptr);
extern   ip_addr  nat_inaddr; /* NAT's default internet address */
#endif   /* NATRT */

#ifdef IP_FRAGMENTS  /* in case we fragment natrouted pkt */
int   ip_fragment(PACKET p, ip_addr firsthop);
#endif   /* IP_FRAGMENTS */

#ifdef IP_RAW
int   ip_raw_input(PACKET P);
#endif   /* IP_RAW */

#ifdef INCLUDE_6TO4
extern int  v6t_rcv(PACKET p);
#endif   /* INCLUDE_6TO4 */

#ifdef IPSEC
void  ipsec_forward(PACKET *p);
void  ipsec_inbound(PACKET *p);
extern u_char ipsec_bypass_v4;
#endif   /* IPSEC */

#ifdef RF_SIMULATION
extern int rfsim_routing;   /* Bool, TRUE if we're doing this */
extern int rfsim_send(void * vpkt, u_long firsthop);
#endif /* RF_SIMULATION */

int ip_rcv_phase2 (PACKET p);

/* FUNCTION: ip_rcv()
 *
 * This is the IP receive upcall routine. It handles packets received 
 * by network ISRs, etc. verifies their Ip headers and does the 
 * upcall to the upper layer that should receive the packet. Returns 
 * 0 if packet was processed succesfully, ENP_NOT_MINE if not for me, 
 * or a negative error code if packet was badly formed. 
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: 
 */

int
ip_rcv(PACKET p)
{
   struct ip * pip;     /* the internet header */
   unsigned short csum; /* packet checksum */
   unsigned short tempsum;
   NET nt;
   unsigned hdrlen;  /* length of IP header including options */
   unsigned len;     /* total length including IP header */
   unsigned short int num_pkts;
   PACKET * pktp;
   unsigned char i;
   int rcvrc = -1;
#ifdef NATRT
   unsigned char rc_ret;
#endif


#ifdef NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
	TCIP_INICHE_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
		, "ip_rcv: got packet, len:%d, if:%d"
		, p->nb_plen, net_num(p->net)
		);
#else
      dprintf("ip_rcv: got packet, len:%d, if:%d\n",
         p->nb_plen, net_num(p->net));
#endif
#endif

   IN_PROFILER(PF_IP, PF_ENTRY);

   nt = p->net;      /* which interface it came in on */
   ip_mib.ipInReceives++;

#if 1 /* INICHE_TCIP_ITGR check revised */
   if( p->nb_plen < sizeof(struct ip) )
   {
      goto bad_header;
   }
#endif

   pip = ip_head(p);

   len = htons(pip->ip_len);

   /* test received MAC len against IP header len */
   if (p->nb_plen < len)
   {
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "ip_rcv: bad pkt len"
			);
#else
         dprintf("ip_rcv: bad pkt len\n");
#endif
         if (NDEBUG & DUMP) ip_dump(p);
            }
#endif
bad_header: /* INICHE_TCIP_ITGR label added */
      ip_mib.ipInHdrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      IN_PROFILER(PF_IP, PF_EXIT);
      return(ENP_BAD_HEADER);
   }

   /* use length from IP header; MAC value may be padded */
   p->nb_plen = len;       /* fix pkt len */

   if ( ((pip->ip_ver_ihl & 0xf0) >> 4) != IP_VER)
   {
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "ip_rcv: bad version number"
			);
#else
         dprintf("ip_rcv: bad version number\n");
#endif
         if (NDEBUG & DUMP) ip_dump(p);
      }
#endif
      ip_mib.ipInHdrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      IN_PROFILER(PF_IP, PF_EXIT);
      return(ENP_BAD_HEADER);
   }

   csum = pip->ip_chksum;
   pip->ip_chksum = 0;
   hdrlen = ip_hlen(pip);

#if 1  /* INICHE_TCIP_ITGR AP01212677: check headerlen before passing to the upper layer */
   if( p->nb_plen <= hdrlen )
   {
      goto bad_header;
   }
#endif

   tempsum = ~cksum(pip, hdrlen >> 1);

   if (csum != tempsum)
   {
      pip->ip_chksum = csum;
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "ip_rcv: bad xsum"
			);
#else
         dprintf("ip_rcv: bad xsum\n");
#endif
         if (NDEBUG & DUMP) ip_dump(p);
      }
#endif
      ip_mib.ipInHdrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      IN_PROFILER(PF_IP, PF_EXIT);
      return(ENP_BAD_HEADER);
   }

   pip->ip_chksum = csum;

#ifdef USE_IPFILTER
   /* Do IP filtering. If packet is accepted, ipf_filter() returns
    * SUCCESS. Discard the packet for all other return values 
    */
   if (ipf_filter(p,1) != SUCCESS)  /* 1 - inbound pkt */
   {
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      IN_PROFILER(PF_IP, PF_EXIT);
      return ENP_NO_ROUTE;
   }
#endif

    /* start off by assuming that we will only process the "current" packet;
     * these values may get overwritten by the NAT module if it returns
     * more than one packet back to this function (ip_rcv ()) */
    num_pkts = 1;
    pktp = &p;

#ifdef NATRT   /* is this part of Network Address Translation box? */
   {
      IN_PROFILER(PF_NAT, PF_ENTRY);
      if ((rc_ret = do_nat(p
#ifdef IP_FRAGMENTS
                             , &pktp,
                             &num_pkts
#endif
                             )) == 0)
      {
         IN_PROFILER(PF_NAT, PF_EXIT);
         return 0;
      }
      IN_PROFILER(PF_NAT, PF_EXIT);
   }
   /* NAT has returned one (or more) packets back to the stack for processing 
    * by other modules */
#endif   /* NATRT */

   /* we need to process 'num_pkts' packets.  Pointers to these packets are stored
    * in storage @ 'pktp' */
   for (i = 0; i < num_pkts; ++i)
   {
      rcvrc = ip_rcv_phase2 (*(pktp + i));
   }

#ifdef NATRT
   /* free any allocated storage */
   if (pktp != &p) nat_free (pktp);
#endif /* NATRT */

   /* when multiple packets are processed in the loop above, the return code
    * contains the return code for the last packet */
   return rcvrc;
}

#ifdef USE_IGMPV3
#define DROP_MCAST_PKT(p, mm) { \
   (mm)++; \
   LOCK_NET_RESOURCE(FREEQ_RESID); \
   pk_free(p); \
   UNLOCK_NET_RESOURCE(FREEQ_RESID); \
   IN_PROFILER(PF_IP, PF_EXIT); \
   return (ENP_NOT_MINE); \
}
#endif /* USE_IGMPV3 */

int ip_rcv_phase2 (PACKET p)
{

   struct ip * pip;     /* the internet header */
   NET nt;
   unsigned short tempsum;

   pip = ip_head(p);
   nt = p->net;      /* which interface it came in on */

#ifdef IP_MULTICAST

   if (IN_MULTICAST(ntohl(pip->ip_dest))) 
   {
      /* If we are acting as a multicast router, all multicast incoming
       * packets will be passed to a multicast forwarding function.
       * For release 1.5, there is no multicast router functionality
       * added.
       */
      struct in_multi * inm;
      /*
       * See if we belong to the destination multicast group on the
       * arrival interface.
       */
      inm = lookup_mcast(pip->ip_dest, nt);
      if (inm == NULL) 
      {
         ip_mib.ipInAddrErrors++;
         LOCK_NET_RESOURCE(FREEQ_RESID);
         pk_free(p);
         UNLOCK_NET_RESOURCE(FREEQ_RESID);
         IN_PROFILER(PF_IP, PF_EXIT);
         return (ENP_NOT_MINE);
      }
      else
#ifdef USE_IGMPV3
      {
         if (pip->ip_prot == IPPROTO_IGMP)
         {
            /*
             * Validate IP Time-to-Live
             */
            if (pip->ip_time != 1)
            {
               DROP_MCAST_PKT(p, ip_mib.ipInHdrErrors);
            }
         }
         else
         {
            /*
             * See if we accept the source based on MSF condition on the
             * arrival interface.
             */
            struct in_addr_source *ias;

            if (inm->inm_source == NULL)
            {
               DROP_MCAST_PKT(p, ip_mib.ipInDiscards);
            }
            if (inm->inm_source->ims_grpjoin > 0)
               goto ours;
            if (inm->inm_source->ims_mode == MCAST_INCLUDE)
            {
               SLIST_FOREACH(inm->inm_source->ims_cur->head, ias)
               {
                  if (ias->addr == pip->ip_src)
                     goto ours;
                  else if (ias->addr > pip->ip_src)
                     break;
               }
               DROP_MCAST_PKT(p, ip_mib.ipInDiscards);
            }
            else
            { 
               /* MCAST_EXCLUDE */
               SLIST_FOREACH(inm->inm_source->ims_cur->head, ias)
               {
                  if (ias->addr == pip->ip_src)
                  {
                     DROP_MCAST_PKT(p, ip_mib.ipInDiscards);
                  }
                  else if (ias->addr > pip->ip_src)
                     break;
               }
            }
         }
      }
#endif /* USE_IGMPV3 */
         goto ours;
   }
#endif   /* IP_MULTICAST */

#if 1 /* INICHE_TCIP_ITGR if net interface holds 0 ip address interface is not configured */
       if(nt->n_ipaddr == 0)
       {
           ip_mib.ipOutDiscards++; 
           LOCK_NET_RESOURCE(FREEQ_RESID);
           pk_free(p);
           UNLOCK_NET_RESOURCE(FREEQ_RESID);
           IN_PROFILER(PF_IP, PF_EXIT);
           return(ENP_NOT_MINE);
       }
#endif

#if 1 /* INICHE_TCIP_ITGR drop packets with broadcast or multicast source address, see ID 947899 and 951374 */
	if (pip->ip_prot == TCP_PROT || pip->ip_prot == ICMP_PROT)
	{
		if ((pip->ip_src == INADDR_BROADCAST)
			|| IN_MULTICAST(ntohl(pip->ip_src))
			|| (pip->ip_src == nt->n_netbr) /* ICMP_PROT: AP01132318 */
			|| (pip->ip_src == nt->n_netbr42)
			|| (pip->ip_src == nt->n_subnetbr)
			|| (pip->ip_dest == INADDR_BROADCAST)
			|| IN_MULTICAST(ntohl(pip->ip_dest))
			|| (pip->ip_dest == nt->n_netbr)   /* ICMP_PROT: AP01132318, see also filter below*/
			|| (pip->ip_dest == nt->n_netbr42) /* ---'--- */
			|| (pip->ip_dest == nt->n_subnetbr)/* ---'--- */
			)
		{
			ip_mib.ipInAddrErrors++;
			LOCK_NET_RESOURCE(FREEQ_RESID);
			pk_free(p);
			UNLOCK_NET_RESOURCE(FREEQ_RESID);
			IN_PROFILER(PF_IP, PF_EXIT);
			return(ENP_NOT_MINE);
		}
	}
#endif

       if ((pip->ip_dest != nt->n_ipaddr) &&  /* Quick check on our own addr */
       (pip->ip_dest != 0xffffffffL) &&   /* Physical cable broadcast addr*/
       (pip->ip_dest != nt->n_netbr) &&   /* All subnet broadcast */
       (pip->ip_dest != nt->n_netbr42) && /* All subnet bcast (4.2bsd) */
       (pip->ip_dest != nt->n_subnetbr) &&/* Our subnet broadcast */
       (nt->n_ipaddr & ~nt->snmask))      /* Know our own host address? */
   {
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "ip_rcv: got pkt not for me; for %u.%u.%u.%u"
			, (unsigned)(pip->ip_dest&0xff),(unsigned)((pip->ip_dest>>8)&0xff),(unsigned)((pip->ip_dest>>16)&0xff),(unsigned)(pip->ip_dest>>24)
			);
#else
         dprintf("ip_rcv: got pkt not for me; for %u.%u.%u.%u\n",
       PUSH_IPADDR(pip->ip_dest));
#endif
#endif

/*#ifdef IP_ROUTING INICHE_TCIP_ITGR do not route */    /* if multi-homed router, try to route */
#if 0 /* INICHE_TCIP_ITGR do not route */
      /* Do routing only if ipForwarding is enabled in the IP MIB. This
       * is the switch for routing whether SNMP is used or not.
       */
      if (ip_mib.ipForwarding == 2)
      {
         ip_mib.ipOutDiscards++; /* Is this the right counter for these? */
         LOCK_NET_RESOURCE(FREEQ_RESID);
         pk_free(p);
         UNLOCK_NET_RESOURCE(FREEQ_RESID);
         IN_PROFILER(PF_IP, PF_EXIT);
         return ENP_NO_ROUTE;
      }
      else
      {
         ip_addr firsthop;       /* fake nexthop for passing to iproute */
         NET outnet;
#ifdef IP_FRAGMENTS
         int err;
#endif

         ip_mib.ipForwDatagrams++;  /* Count MIB-2 route attempts here */
         if (pip->ip_time <= 1)     /* Time to Live (hopcount) expired? */
         {
            ip_mib.ipOutDiscards++; /* Is this the right counter for these? */
#ifdef FULL_ICMP
            icmp_destun(pip->ip_src, p->net->n_ipaddr, pip, (TIMEX <<8), p->net);
#endif   /* FULL_ICMP */
            LOCK_NET_RESOURCE(FREEQ_RESID);
            pk_free(p);
            UNLOCK_NET_RESOURCE(FREEQ_RESID);
            IN_PROFILER(PF_IP, PF_EXIT);
            return ENP_NO_ROUTE;
         }

         p->fhost = pip->ip_dest;   /* set packet's target IP in net endian */
         if ((outnet = iproute(p->fhost, &firsthop)) == NULL)  /* find route */
         {
            ip_mib.ipOutNoRoutes++; /* count unroutable pkts */
            LOCK_NET_RESOURCE(FREEQ_RESID);
            pk_free(p);
            UNLOCK_NET_RESOURCE(FREEQ_RESID);
            IN_PROFILER(PF_IP, PF_EXIT);
            return ENP_NO_ROUTE;
         }

#ifdef STRICT_ROUTE
         /* make sure packet won't route back out iface it came in on. This
          * used to be an absolute rule of IP routing, however newer technologies
          * like NAT sometimes need to break this rule. We recommend you leave
          * this code disabled unless you have a special need for it.
          */
         if (outnet == p->net)
         {
            ip_mib.ipOutNoRoutes++;
            LOCK_NET_RESOURCE(FREEQ_RESID);
            pk_free(p);
            UNLOCK_NET_RESOURCE(FREEQ_RESID);
            IN_PROFILER(PF_IP, PF_EXIT);
            return(ENP_NOT_MINE);
         }
#endif   /* STRICT_ROUTE */

         /* Check to see if the packet was is addressed to one of our IP
          * addresses other than the interface it was received on. If so,
          * routing should have returned that interface and we can trap this
          * situation by checking the interfaces IP address.
          */
         if(pip->ip_dest == outnet->n_ipaddr)
            goto ours;

         /* Make sure the packet is not a subnet broadcast for either the
          * source or destination network.
          */
         if((pip->ip_dest == outnet->n_netbr) ||
            (pip->ip_dest == p->net->n_netbr))
         {
            goto ours;            
         }

         /* Routed OK, prepare to send */
         p->net = outnet;           /* set iface to send on */

#ifdef IPSEC
         IN_PROFILER(PF_IPSEC, PF_ENTRY);
         ipsec_forward(&p);
         IN_PROFILER(PF_IPSEC, PF_EXIT);
         if (p == NULL)
         {
            IN_PROFILER(PF_IP, PF_EXIT);
            return ENP_LOGIC;
         }
         if (!(p->fhost))
         {
            pip = ip_head(p);
            p->fhost = pip->ip_dest;
            if ((outnet = iproute(p->fhost, &firsthop)) == NULL)
            {
               ip_mib.ipOutNoRoutes++;
               LOCK_NET_RESOURCE(FREEQ_RESID);
               pk_free(p);
               UNLOCK_NET_RESOURCE(FREEQ_RESID);
               IN_PROFILER(PF_IP, PF_EXIT);
               return ENP_NO_ROUTE;
            }
         }
#endif   /* IPSEC */

         /* see if packet is too big for media of dest net */
         if ((p->nb_plen + p->net->n_lnh) > (unsigned)outnet->n_mtu)
         {
#ifdef IP_FRAGMENTS
            /* see if we're not supposed to fragment it */
            if (ntohs(pip->ip_flgs_foff) & IP_FLG_DF)
            {
#ifdef FULL_ICMP
               icmp_destun_34(pip->ip_src, p->net->n_ipaddr, pip, DSTFRAG,
                              p->net, (unsigned short)outnet->n_mtu);
#endif   /* FULL_ICMP */
               ip_mib.ipFragFails++;
               LOCK_NET_RESOURCE(FREEQ_RESID);
               pk_free(p);
               UNLOCK_NET_RESOURCE(FREEQ_RESID);
               IN_PROFILER(PF_IP, PF_EXIT);
               return ENP_LOGIC;
            }

            /* fall to here if we're going to fragment it. */
            pip->ip_time--;            /* datagram's hop count */
            err = ip_fragment(p, firsthop);
            IN_PROFILER(PF_IP, PF_EXIT);
            return(err);
#else /* IP fragments not supported? Bad news....*/
            dtrap(); /* this should be caught by programmers during development */
#ifdef FULL_ICMP
            icmp_destun_34(pip->ip_src, p->net->n_ipaddr, pip, DSTFRAG,
                           p->net, (unsigned short)outnet->n_mtu);
#endif   /* FULL_ICMP */
            LOCK_NET_RESOURCE(FREEQ_RESID);
            pk_free(p);
            UNLOCK_NET_RESOURCE(FREEQ_RESID);
            return ENP_LOGIC;
#endif   /* IP_FRAGMENTS */
         }
         pip->ip_time--;            /* datagram's hop count */
         pip->ip_chksum = IPXSUM;   /* clear checksum field for summing */
         pip->ip_chksum = ~cksum(pip, 10);   /* new xsum */
         IN_PROFILER(PF_IP, PF_EXIT);
#ifdef RF_SIMULATION
         if(rfsim_routing)
            return(rfsim_send(p, firsthop));
#endif   /* RF_SIMULATION */
         return(ip2mac(p, firsthop));
      }
#else /* not IP_ROUTING, punt the packet */
      ip_mib.ipInAddrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      IN_PROFILER(PF_IP, PF_EXIT);
      return(ENP_NOT_MINE);
#endif
   }

/* #if defined (IP_MULTICAST) || defined (IP_ROUTING) INICHE_TCIP_ITGR do not route */
#if defined (IP_MULTICAST)
ours:
#endif

   /* Test for fragment: */
   tempsum = htons(pip->ip_flgs_foff); /* borrow cksum variable */
   if ((tempsum & IP_FLG_MF) ||  /* IP flag for "More Fragments" set? */
       (tempsum & ~IP_FLG_MASK))  /* or offset to last frag? */
   {
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "ip_rcv: fragment from %u.%u.%u.%u"
			, (unsigned)(pip->ip_src&0xff),(unsigned)((pip->ip_src>>8)&0xff),(unsigned)((pip->ip_src>>16)&0xff),(unsigned)(pip->ip_src>>24)
			);
#else
         dprintf("ip_rcv: fragment from %u.%u.%u.%u\n", 
          PUSH_IPADDR(pip->ip_src));
#endif
         ip_dump(p);
      }
#endif
      ip_mib.ipReasmReqds++;     /* got a reassemble request; ie a frag */
#ifdef IP_FRAGMENTS
      return(ip_reassm(p));
#else
      ip_mib.ipReasmFails++;     /* we don't do these */
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      IN_PROFILER(PF_IP, PF_EXIT);
      return ENP_NOT_MINE;    /* perhaps should be ENP_BAD_HEADER? */
#endif
   }

   IN_PROFILER(PF_IP, PF_EXIT);

#ifdef IPSEC
   /* Since ip_rcv may be called re-entrantly, ensure that we EXIT from
    * profiler before calling IPSEC
    */
   IN_PROFILER(PF_IPSEC, PF_ENTRY);
   if (!ipsec_bypass_v4)
   {
   ipsec_inbound(&p);
   }
   IN_PROFILER(PF_IPSEC, PF_EXIT);
   if (p == NULL)
   {
      return ENP_LOGIC;
   }
#endif /* IPSEC */

   return(ip_demux(p));    /* demux to correct to upper layer */
}

/* FUNCTION: ip_demux()
 *
 * ip_demux() - actual upcall from IP to upper layers. This called by 
 * ip_rcv to pass the packets to the upper layers. This is seperate 
 * ip_rcv because it may need to be called by IP fragment reassembly 
 * logic. 
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: Same return values as ip_rcv(). 
 */

int
ip_demux(PACKET p)
{
   struct ip * pip;  /* the internet header */
   int   err;

   /* The packet is verified; the header is correct. Now we have
    * to demultiplex it among our internet connections.
    */
   pip = (struct ip *)(p->nb_prot);

#ifdef NPDEBUG
   /* make sure the caller set p->nb_prot */
   if(pip->ip_ver_ihl != 0x45)
   {
      if((pip->ip_ver_ihl < 0x45) ||
         (pip->ip_ver_ihl > 0x47))
      {
#if 1   /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "ip_demux: bad IP type 0x%x"
			, pip->ip_ver_ihl
			);
#else
         dprintf("ip_demux: bad IP type 0x%x\n", pip->ip_ver_ihl);
#endif
         return ENP_LOGIC;
      }
   }
#endif /* NPDEBUG */

   /* for profiling purposes count the upper layers (UDP, ICMP) in the IP
    * profile bucket. TCP will insert it's own nested profile calls 
    */
   IN_PROFILER(PF_IP, PF_ENTRY);
   p->fhost = pip->ip_src;

#ifdef NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
   {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_05(0, LSA_TRACE_LEVEL_CHAT
			, "ip_demux: pkt prot %u from %u.%u.%u.%u"
			, pip->ip_prot, (unsigned)(pip->ip_src&0xff),(unsigned)((pip->ip_src>>8)&0xff),(unsigned)((pip->ip_src>>16)&0xff),(unsigned)(pip->ip_src>24)
			);
#else
      dprintf("ip_demux: pkt prot %u from %u.%u.%u.%u\n",
       pip->ip_prot, PUSH_IPADDR(pip->ip_src));
#endif
      if (NDEBUG & DUMP) ip_dump(p);
   }
#endif

   switch (pip->ip_prot)
   {
   case UDP_PROT:
      ip_mib.ipInDelivers++;
      err = udpdemux(p);
      break;
   case ICMP_PROT:
      ip_mib.ipInDelivers++;
      err = icmprcv(p);
      break;
#if defined (IP_MULTICAST) && (defined (IGMP_V1) || defined (IGMP_V2) || defined (USE_IGMPV3))
   case IGMP_PROT:
      ip_mib.ipInDelivers++;
      err = igmp_input(p);
      break;
#endif   /* IP_MULTICAST and (IGMPv1 or IGMPv2) */
#ifdef INCLUDE_TCP
   case TCP_PROT:
      ip_mib.ipInDelivers++;
      err = tcp_rcv(p);
      break;
#endif   /* TCP support */
#ifdef IPV6_TUNNEL
   case 0x29:     /* IPv6 tunnel type */
      err = v6t_rcv(p);
      break;
#endif /* IPV6_TUNNEL */
   default: /* unknown upper protocol */
#ifdef IP_RAW
      ip_mib.ipInDelivers++;
      err = ip_raw_input(p);
      break;
#else  /* IP_RAW */
#if 0 /*INICHE_TCIP_ITGR (AP00385533): don't increment twice (see below) */
      ip_mib.ipUnknownProtos++;
#endif
      /* set err so we can tell if the packet was handled by an
       * upcall or not.
       */
      err = ENP_PARAM;
      break;
#endif /* IP_RAW */
   }

   IN_PROFILER(PF_IP, PF_EXIT);

   if(err != ENP_PARAM)
   {
      return err;
   }
   /* fall to here if transport protocol not supported */

#ifdef FULL_ICMP
   /* nobody's listening for this packet. Unless it was broadcast or 
    * multicast, send a destination unreachable. 
    */
   if ((pip->ip_dest != 0xffffffffL) &&   /* Physical cable broadcast addr*/
#ifdef IP_MULTICAST
       (!(IN_MULTICAST(ntohl(pip->ip_dest)))) && /* multicast address */
#endif /* IP_MULTICAST */
       (pip->ip_dest != p->net->n_netbr) &&   /* All subnet broadcast */
       (pip->ip_dest != p->net->n_netbr42) && /* All subnet bcast (4.2bsd) */
       (pip->ip_dest != p->net->n_subnetbr) &&   /* Our subnet broadcast */
       (p->net->n_ipaddr ^ p->net->n_subnetbr))  /* Know our own host address? */
   {

#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "ip_demux: unhandled prot %u"
			, pip->ip_prot
			);
#else
         dprintf("ip_demux: unhandled prot %u\n", pip->ip_prot);
#endif
         if (NDEBUG & DUMP) ip_dump(p);
      }
#endif   /* NPDEBUG */
      icmp_destun(pip->ip_src, p->net->n_ipaddr, pip, DSTPROT, p->net);
   }
#endif   /* FULL_ICMP */

   ip_mib.ipUnknownProtos++;
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   return ENP_NOT_MINE;
}

#ifdef NET_STATS

/* FUNCTION: ip_stats()
 * 
 * PARAM1: void * pio
 *
 * RETURNS: 
 */

int
ip_stats(void * pio)
{
   ns_printf(pio,"IP MIB statistics:\n");
   ns_printf(pio,"Gateway: %s     default TTL: %lu\n",
     (ip_mib.ipForwarding == 1)?"YES":"NO", ip_mib.ipDefaultTTL);
   ns_printf(pio,"rcv:  total: %lu    header err: %lu    address err: %lu\n",
     ip_mib.ipInReceives, ip_mib.ipInHdrErrors, ip_mib.ipInAddrErrors);
   ns_printf(pio,"rcv:  unknown Protocls: %lu    delivered: %lu\n",
     ip_mib.ipUnknownProtos, ip_mib.ipInDelivers);
   ns_printf(pio,"send: total: %lu    discarded: %lu     No routes: %lu\n",
     ip_mib.ipOutRequests, ip_mib.ipOutDiscards, ip_mib.ipOutNoRoutes);
   ns_printf(pio,"Routing; forwarded: %lu    discarded: %lu\n",
     ip_mib.ipForwDatagrams, ip_mib.ipRoutingDiscards);
   ns_printf(pio,"Recvd fragments: %lu, Frames reassembled: %lu\n",
     ip_mib.ipReasmReqds, ip_mib.ipReasmOKs);
   ns_printf(pio,"Pkts fragmented: %lu, Fragments sent: %lu, dropped: %lu\n",
     ip_mib.ipFragOKs, ip_mib.ipFragCreates, ip_mib.ipFragFails);
   ns_printf(pio,"Reasm.Timeout: %lu seconds,  Reasm.Errors: %lu\n",
     ip_mib.ipReasmTimeout, ip_mib.ipReasmFails);

   return 0;
}

#endif   /* NET_STATS */

/* end of file ipdemux.c */

