/*
 * v3.1 August 30, 2011 ip/icmp.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: icmp.c
 *
 * Copyright  2000 By InterNiche Technologies Inc. All rights reserved
 *
 *
 * MODULE: INET
 *
 * ROUTINES: icmprcv(), icmp_destun_34(), icmp_destun(), 
 * ROUTINES: icmp_du(), icmp_stats(),
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */
/* Portions Copyright 1990-1994 by NetPort Software. */
/* Portions Copyright 1986 by Carnegie Mellon  */
/* Portions Copyright 1984 by the Massachusetts Institute of Technology  */

#define LTRC_ACT_MODUL_ID 22 /* INICHE_TCIP_ITGR added for LSA traces */

#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"
#include "icmp.h"
#include "pmtu.h"
#ifdef TCIP_CFG_VLAN /*INICHE_TCIP_ITGR enable support of VLAN tagged ethernet frames (IEEE 802.1 Q)*/
#include "ether.h"
#endif

struct IcmpMib icmp_mib;   /* storage for MIB statistics */

/* the callback for application to get a peek at icmp du packets */
void (*icmpdu_hook)(PACKET p, struct destun * pdp) = NULL;


/* FUNCTION: icmprcv()
 *
 * ICMP received packet upcall handler. Returns 0 if we processed the 
 * packet, or ENP_NOT_MINE, or a negative error code. 
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: 
 */

#ifdef USER_PING_TSTAMP
extern u_long user_UTCtime();
struct align_stamps
{
   u_long dtstamp[3]; 
};
#endif /* USER_PING_TSTAMP */

int
icmprcv(PACKET p)      /* the incoming packet */
{
   unsigned len;        /* packet length, minus IP & MAC headers */
   ip_addr host;       /* the foreign host */
   struct ip * pip;
   struct ping *  e;
   unsigned short osum;
   unsigned short xsum;
#ifdef FULL_ICMP
   struct redirect * rd;
   struct destun *   pdp;
#endif   /* FULL_ICMP */
   char sav_ch = '\0';
   int i;

   icmp_mib.icmpInMsgs++;                 /* received one more icmp */

#ifdef TCIP_CFG_VLAN /*INICHE_TCIP_ITGR enable support of VLAN tagged ethernet frames (IEEE 802.1 Q)*/
   /* VLAN Info: Tag Protocol Identifier (TPID):    2 Bytes = 0x8100 */
   /*            User_priority:                     3 Bit */
   /*            Canonical Format Indicator (CFI):  1 Bit */
   /*            VLAN Identifier (VID):            12 Bit */
   if((unshort)ET_TYPE_GET(p->nb_buff+ ETHHDR_BIAS) == 0x8100)
   {
      /* remove VLAN Info (4 Bytes) to enable reply in same buffer */
      MEMMOVE(p->nb_prot - 4, p->nb_prot, p->nb_plen);
      p->nb_prot -= 4;
   }
#endif

   pip = ip_head(p);                      /* find IP header */
   len = p->nb_plen - (ip_hlen(pip));     /* strip IP header length */
   host = p->fhost;                       /* filled in by IP layer */

#ifdef   NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
      TCIP_INICHE_TRACE_05(0, LSA_TRACE_LEVEL_CHAT
         ,"ICMP: p[%u] from %u.%u.%u.%u"
         , len, (unsigned)(host&0xff),(unsigned)((host>>8)&0xff),(unsigned)((host>>16)&0xff),(unsigned)(host>>24)
         );
#else
      dprintf("ICMP: p[%u] from %u.%u.%u.%u\n", len, PUSH_IPADDR(host));
#endif
#endif

#if 1/* INICHE_TCIP_ITGR - strip ip header options */
   if (pip->ip_ver_ihl > 0x45 ) /* check ip-header size */
   {
      unsigned optlen;
      /* get the IP header options length in octets */
      optlen = ip_optlen(pip);
      /* remove the stripped options from the IP datagram length */
      pip->ip_len = htons(ntohs(pip->ip_len) - optlen);
      /* and from the IP header length (which will be 5*4 octets long) */
      pip->ip_ver_ihl = (pip->ip_ver_ihl & 0xf0) | 5;
      /* move IP payload down to keep packet alignment*/
      MEMMOVE(p->nb_prot + 20, p->nb_prot + 20 + optlen , len);
      p->nb_plen -= optlen;      /* adjust length */
   }
#endif

   /* INICHE_TCIP_ITGR AP01212677: moved check for headersize to ipdemux.c */
   
   e = (struct ping *)ip_data(pip);       /* finally, extract ICMP header */

   osum = e->pchksum;
   e->pchksum = 0;

   if (len&1)
   {
      sav_ch = *(((char *) e) + len);
      ((char *)e)[len] = 0;
   }

   xsum = ~cksum(e, (len+1)>>1);
   if (len&1) *(((char *) e) + len) = sav_ch;
   if (xsum != osum)
   {
      e->pchksum = osum;
#ifdef   NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
			, "ICMP: Bad xsum %04x should have been %04x"
			, osum, xsum
			);
#else
         dprintf("ICMP: Bad xsum %04x should have been %04x\n",
          osum, xsum);
#endif  
         if (NDEBUG & DUMP) ip_dump(p);
      }
#endif
      icmp_mib.icmpInErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return ENP_BAD_HEADER;
   }

   e->pchksum = osum;

   switch (e->ptype) 
   {
   case ECHOREQ:  /* got ping request, send reply */
      icmp_mib.icmpInEchos++;
#ifdef   NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_ERROR
			, "ICMP: echo reply to %u.%u.%u.%u"
			, (unsigned)(host&0xff),(unsigned)((host>>8)&0xff),(unsigned)((host>>16)&0xff),(unsigned)(host>>24)
			);
#else
         dprintf("ICMP: echo reply to %u.%u.%u.%u\n", PUSH_IPADDR(host));
#endif
#endif
#if 0 /*INICHE_TCIP_ITGR ID 947899, moved to ipdemux.c */
#ifdef TCIP_CFG_IGNORE_PING_BROADCAST_REQUEST /*INICHE_TCIP_ITGR suppress smurf attack*/
      /* test for broadcast, subnet broadcast, multicast in source or destination address */
      if ( (host == INADDR_BROADCAST)
         || IN_MULTICAST(ntohl(host))
         || (host == p->net->n_netbr)   /* AP01132318 */   
		 || (host == p->net->n_netbr42) /* ---'--- */
		 || (host == p->net->n_subnetbr)/* ---'--- */
         || (pip->ip_dest == INADDR_BROADCAST)
         || IN_MULTICAST(ntohl(pip->ip_dest))
         || (pip->ip_dest == p->net->n_netbr)   /* AP01132318, see also ip_rcv_phase2() filter */
         || (pip->ip_dest == p->net->n_netbr42) /* ---'--- */
         || (pip->ip_dest == p->net->n_subnetbr)/* ---'--- */
         )
      {
          LOCK_NET_RESOURCE(FREEQ_RESID);		
          pk_free(p);
          UNLOCK_NET_RESOURCE(FREEQ_RESID);
          return ENP_NOT_MINE;
      }
#endif
#endif
      e->ptype = ECHOREP;
      e->pchksum = 0;
      if (len&1)  /* pad odd length packets for checksum routine */
      {
         sav_ch = *(((char *) e) + len);
         ((char *)e)[len] = 0;
      }

      e->pchksum = ~cksum(e, (len+1)>>1);
      if (len&1) *(((char *) e) + len) = sav_ch;
      /* check to see if the destination is the IPv4 broadcast address,
       * or if the destination is a multicast group address, or if the
       * destination address is the subnet-directed broadcast 
       */
      if ((pip->ip_dest == 0xffffffff) || 
#ifdef IP_MULTICAST
          (IN_MULTICAST(ntohl(pip->ip_dest))) ||
#endif
          (pip->ip_dest == (p->net->n_ipaddr | (~(p->net->snmask)))))
      {
         pip->ip_src = p->net->n_ipaddr;
      }
      else pip->ip_src = pip->ip_dest;

      pip->ip_dest = host;
      icmp_mib.icmpOutEchoReps++;
      icmp_mib.icmpOutMsgs++;
      p->fhost = host;
      p->nb_prot += ip_hlen(pip);      /* move pointer past IP to ICMP */
      p->nb_plen = len;

      i = ip_write(ICMP_PROT, p);
      if (i < 0)
      {
#ifdef   NPDEBUG
         if (NDEBUG & (UPCTRACE))
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "icmp: reply failed"
				);
#else
            dprintf("icmp: reply failed\n");
#endif
#endif
      }
      /* reused p will be freed by net->xxx_send() */
      return 0;
   case ECHOREP:
      icmp_mib.icmpInEchoReps++;
      /* The PING appplication expects the nb_prot field to point to
       * the ICMP header, not the IP as it does here, NOR to the application
       * data like a TCP or UDP app would. Adjust this here:
       */
      p->nb_prot += ip_hlen(pip);      /* move pointer past IP to ICMP */
      p->nb_plen -= ip_hlen(pip);      /* subtract IP header size */
#ifdef PING_APP
      return(pingUpcall(p));
#else    /* not a real ping app; but report ping reply if debugging... */
#ifdef NPDEBUG
      if ((NDEBUG & IPTRACE))
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
			, "Ping (icmp echo) reply from %u.%u.%u.%u, data len %d"
			, (unsigned)(p->fhost&0xff),(unsigned)((p->fhost>>8)&0xff),(unsigned)((p->fhost>>16)&0xff),(unsigned)(p->fhost>>24), p->nb_plen
			);
#else
         dprintf("Ping (icmp echo) reply from %u.%u.%u.%u, data len %d\n", 
          PUSH_IPADDR(p->fhost), p->nb_plen);
#endif
      }
#endif   /* NPDEBUG */
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return(0);
#endif   /* PING_APP */
#ifdef FULL_ICMP
   case DESTIN:
      icmp_mib.icmpInDestUnreachs++;
      pdp = (struct destun *)e;
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_NOTE
			, "ICMP: got dest unreachable type "
			);
		TCIP_INICHE_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
			, "%u on %u.%u.%u.%u "
			, pdp->dcode, (unsigned)(pdp->dip.ip_dest&0xff),(unsigned)((pdp->dip.ip_dest>>8)&0xff),(unsigned)((pdp->dip.ip_dest>>16)&0xff),(unsigned)(pdp->dip.ip_dest>>24)
			);
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "from %u.%u.%u.%u"
			, (unsigned)(host&0xff),(unsigned)((host>>8)&0xff),(unsigned)((host>>16)&0xff),(unsigned)(host>>24)
			);
#else
         dprintf("ICMP: got dest unreachable type ");
         dprintf("%u on %u.%u.%u.%u ", pdp->dcode,
          PUSH_IPADDR(pdp->dip.ip_dest));
         dprintf("from %u.%u.%u.%u\n", PUSH_IPADDR(host));
#endif
      }
#endif   /* NPDEBUG */
      icmp_du(p, pdp);
      break;
   case SOURCEQ:
      icmp_mib.icmpInSrcQuenchs++;
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "ICMP: source quench from %u.%u.%u.%u"
			, (unsigned)(host&0xff),(unsigned)((host>>8)&0xff),(unsigned)((host>>16)&0xff),(unsigned)(host>>24)
			);
#else
         dprintf("ICMP: source quench from %u.%u.%u.%u\n", PUSH_IPADDR(host));
#endif
         if (NDEBUG & DUMP) ip_dump(p);
      }
#endif   /* NPDEBUG */
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      break;
   case REDIR:          /* got an icmp redirect */
      icmp_mib.icmpInRedirects++;
      rd = (struct redirect *)e;
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "ICMP: rcvd redirect for %u.%u.%u.%u "
			, (unsigned)(rd->rdip.ip_dest&0xff),(unsigned)((rd->rdip.ip_dest>>8)&0xff),(unsigned)((rd->rdip.ip_dest>>16)&0xff),(unsigned)(rd->rdip.ip_dest>>24)
			);
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "to %u.%u.%u.%u"
			, (unsigned)(rd->rdgw&0xff),(unsigned)((rd->rdgw>>8)&0xff),(unsigned)((rd->rdgw>>16)&0xff),(unsigned)(rd->rdgw>>24)
			);
#else
         dprintf("ICMP: rcvd redirect for %u.%u.%u.%u ",
          PUSH_IPADDR(rd->rdip.ip_dest));
         dprintf("to %u.%u.%u.%u\n", PUSH_IPADDR(rd->rdgw));
#endif
      }
#endif   /* NPDEBUG */
#if 0 /* INICHE_TCIP_ITGR disable routing for ICMP REDIR */
#ifdef IP_ROUTING
      /* try to add/update route table */
      add_route(rd->rdip.ip_dest, 0xFFFFFFFF, rd->rdgw, 
       net_num(p->net), IPRP_ICMP);
#endif   /* IP_ROUTING */
#endif
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      break;
   case TIMEX:
      icmp_mib.icmpInTimeExcds++;
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
      {
         struct timex * pt =  (struct  timex *)e;
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "ICMP: timex msg from %u.%u.%u.%u"
			, (unsigned)(p->fhost&0xff),(unsigned)((p->fhost>>8)&0xff),(unsigned)((p->fhost>>16)&0xff),(unsigned)(p->fhost>>24)
			);
		TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, " about %u.%u.%u.%u"
			, (unsigned)(pt->tip.ip_dest&0xff),(unsigned)((pt->tip.ip_dest>>8)&0xff),(unsigned)((pt->tip.ip_dest>>16)&0xff),(unsigned)(pt->tip.ip_dest>>24)
			);
#else
         dprintf("ICMP: timex msg from %u.%u.%u.%u\n",
          PUSH_IPADDR(p->fhost));
         dprintf(" about %u.%u.%u.%u\n", PUSH_IPADDR(pt->tip.ip_dest));
#endif
         if (NDEBUG & DUMP) ip_dump(p);
      }
#endif   /* NPDEBUG */
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      break;
   case PARAM:
      icmp_mib.icmpInParmProbs++;
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_NOTE
			, "ICMP: got param problem message"
			);
#else
         dprintf("ICMP: got param problem message\n");
#endif
      if (NDEBUG & DUMP)
      {
         ip_dump(p);
      }
#endif   /* NPDEBUG */
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      break;
   case TIMEREQ:
      icmp_mib.icmpInTimestamps++;
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
		TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_NOTE
			, "ICMP: got timestamp request"
			);
#else
         dprintf("ICMP: got timestamp request\n");
#endif
#endif   /* NPDEBUG */
      e->ptype = TIMEREP;
      e->pchksum = 0;

#ifdef USER_PING_TSTAMP
      {
		  struct tstamp *tsp = (struct tstamp *)e;
          struct align_stamps *sstmp = (struct align_stamps *)&tsp->stamps;

		  /* 
		   * user_UTCtime() must return UTC time in htonl((u_long)t) format.
		   * we set the Receive Timestamp and Transmit Timestamp
		   */
		  sstmp->dtstamp[1] = sstmp->dtstamp[2] = user_UTCtime();
	  }
#endif /* USER_PING_TSTAMP */

      e->pchksum = ~cksum(e, sizeof(struct tstamp)>>1);
      pip->ip_src = pip->ip_dest;
      pip->ip_dest = host;
      icmp_mib.icmpOutMsgs++;
      icmp_mib.icmpOutTimestampReps++;
      p->nb_prot += ip_hlen(pip);      /* move pointer past IP to ICMP */
      p->nb_plen = sizeof(struct tstamp);
      p->fhost = host;
      i = ip_write(ICMP_PROT, p);
      if (i < 0)
      {
#ifdef   NPDEBUG
         if (NDEBUG & UPCTRACE)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_NOTE
			, "icmp: can't send timestamp reply"
			);
#else
            dprintf("icmp: can't send timestamp reply\n");
#endif
#endif   /* NPDEBUG */
      }
      /* re-used packet was pk_free()d by net->send() */
      return (0);
   case INFO:
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_NOTE
			, "icmp: got info request"
			);
#else
         dprintf("icmp: got info request\n");
#endif
#endif   /* NPDEBUG */
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      break;
#endif   /* FULL_ICMP */
   default:
#ifdef   NPDEBUG
      if (NDEBUG & UPCTRACE)
      {
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "icmp: unhandled type %u"
			, e->ptype
			);
#else
         dprintf("icmp: unhandled type %u\n", e->ptype);
#endif
         if (NDEBUG & DUMP) ip_dump(p);
      }
#endif   /* NPDEBUG */
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return ENP_NOT_MINE;
   }
#ifdef FULL_ICMP
   return ENP_NOT_MINE;
#endif   /* FULL_ICMP */
}

#ifdef FULL_ICMP

#ifdef NPDEBUG
static char *dsts[] = 
{
   "net", "host", "protocol", "port", "fragmentation needed",
   "source route failed" };
#endif   /* NPDEBUG */



/* FUNCTION: icmp_destun_34()
 *
 * Format and send an ICMP destination unreachable packet.
 * If type = 3 and code = 4, 'mtu' is stored in the "MTU of
 * next-hop network" field in the ICMP header.
 * 
 * If the type field is 0, the type is assumed to be DESTIN.
 *
 * PARAM1: ip_addr            IP address of host to complain to
 * PARAM2: ip_addr            source address for outgoing ICMP/IP packet header
 * PARAM3: struct ip *        IP header of offending packet
 * PARAM4: unsigned           ICMP type and code fields
 * PARAM5: NET                net interface packet came in on
 * PARAM6: unsigned short     MTU of next-hop network
 *
 * RETURN: none
 */

void
icmp_destun_34(ip_addr host, ip_addr src_addr, struct ip *ip,
               unsigned typecode, NET  net, unsigned short mtu)
{
   PACKET p;
   struct destun *   d;
   struct ip * pip;
   int   i;

#ifdef NPDEBUG
   if (NDEBUG & PROTERR)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
			, "icmp: sending XX dest unreachable to %u.%u.%u.%u"
			, (unsigned)(host&0xff),(unsigned)((host>>8)&0xff),(unsigned)((host>>16)&0xff),(unsigned)(host>>24)
			);
#else
      dprintf("icmp: sending %s dest unreachable to %u.%u.%u.%u\n",
      dsts[typecode & 0xFF], PUSH_IPADDR(host));
#endif
#endif   /* NPDEBUG */

   LOCK_NET_RESOURCE(FREEQ_RESID);
   p = pk_alloc(512 + IPHSIZ);   /* get packet to send icmp dest unreachable */
   UNLOCK_NET_RESOURCE(FREEQ_RESID);

   if (p == NULL)
   {
#ifdef NPDEBUG
      if (NDEBUG & IPTRACE)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "icmp: can't alloc pkt"
			);
#else
         dprintf("icmp: can't alloc pkt\n");
#endif
#endif   /* NPDEBUG */
      icmp_mib.icmpOutErrors++;
      return;
   }

   /* build the addresses in the IP header */
   pip = (struct ip *)p->nb_prot;
   pip->ip_src = src_addr;
   pip->ip_dest = host;

   /* allow space for icmp header */
   p->nb_prot += sizeof(struct ip);
   p->nb_plen -= sizeof(struct ip);
   p->net = net;     /* Put in the interface that this packet came in on */

   d = (struct destun *)p->nb_prot;

   if (typecode & 0xFF00)               /* if the type was sent */
      d->dtype = (char)(typecode >>8);  /* then use it */
   else                                 /* else use default */
      d->dtype = DESTIN;
   d->dcode = (char)(typecode & 0xFF);
   d->dno1 = d->dno2 = 0;
#ifndef ICMP_SUPPRESS_PMTU
   if ((typecode & 0xFF) == DSTFRAG)
      d->dno2 = htons(mtu);
#endif    /* ICMP_SUPPRESS_PMTU */
   MEMCPY(&d->dip, ip, (sizeof(struct ip) + ICMPDUDATA));

   d->dchksum = 0;
   d->dchksum = ~cksum(d, sizeof(struct destun)>>1);

   p->nb_plen =  sizeof(struct destun);
   p->fhost = host;
   i = ip_write(ICMP_PROT, p);
   if (i < 0)
   {
      icmp_mib.icmpOutErrors++;
#ifdef   NPDEBUG
      if (NDEBUG & (IPTRACE|NETERR|PROTERR))
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "ICMP: Can't send dest unreachable"
			);
#else
         dprintf("ICMP: Can't send dest unreachable\n");
#endif
#endif   /* NPDEBUG  */
      return;
   }
   icmp_mib.icmpOutMsgs++;
   icmp_mib.icmpOutDestUnreachs++;
   return;
}



/* FUNCTION: icmp_destun()
 *
 * icmp_destun() - Send an ICMP destination unreachable packet.
 * 
 * PARAM1: ip_addr            IP address of host to complain to
 * PARAM2: ip_addr            source address for outgoing ICMP/IP packet header
 * PARAM3: struct ip *        IP header of offending packet
 * PARAM4: unsigned           ICMP type and code fields
 * PARAM5: NET                net interface packet came in on
 *
 * If the type field is 0, then type is assumed to be DESTIN.
 *
 * RETURN: none
 *
 * Calls icmp_destun_34() with a MTU length of 0.
 */

void
icmp_destun(ip_addr host, ip_addr src_addr,
            struct ip *ip, unsigned typecode, NET  net)
{
   icmp_destun_34(host, src_addr, ip, typecode, net, 0);
}



/* FUNCTION: icmp_timex ()
 *
 * icmp_timex () - Send an ICMP Time Exceeded message.
 * 
 * PARAM1: struct ip * ip - Pointer to IP header of packet in response to which 
 *         an ICMP Time Exceeded message is being sent
 * PARAM2: char code - code of ICMP Time Exceeded message
 *
 * RETURNS: void
 */

void icmp_timex (struct ip * ip, char code) 
{
   PACKET p;
   struct timex * tx;
   struct ip * pip;
   int i;
   u_char icmp_pkt_len;

#ifdef NPDEBUG
   if (NDEBUG & PROTERR)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
			, "icmp: sending ICMP Time Exceeded with code %u to %u.%u.%u.%u"
			, code, (unsigned)((ntohs(ip->ip_src))&0xff),(unsigned)(((ntohs(ip->ip_src))>>8)&0xff),(unsigned)(((ntohs(ip->ip_src))>>16)&0xff),(unsigned)((ntohs(ip->ip_src))>>24)
			);
#else
      dprintf("icmp: sending ICMP Time Exceeded with code %u to %u.%u.%u.%u\n",
       code, PUSH_IPADDR((ntohs(ip->ip_src))));
#endif
#endif   /* NPDEBUG */

   icmp_pkt_len = ICMPTIMEX_HDR_LEN + ip_hlen(ip) + ICMPTIMEX_PAYLOAD_DATA_LEN;
   LOCK_NET_RESOURCE(FREEQ_RESID);
   /* get packet to send ICMP message.  The total length of such a packet is the 
    * length of the "outer" IP header, length of the ICMP header (ICMPTIMEX_HDR_LEN,
    * 8 bytes), length of the "inner" IP header, and length of "user" data 
    * (ICMPTIMEX_PAYLOAD_DATA_LEN, 8 bytes) (just past the "inner" IP header).  The 
    * latter two items are from the packet that the ICMP Time Exceeded message is 
    * being sent in response to. */
   p = pk_alloc(MaxLnh + IPHSIZ + icmp_pkt_len);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);

   if (p == NULL)
   {
#ifdef NPDEBUG
      if (NDEBUG & IPTRACE)
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "icmp: can't alloc pkt"
			);
#else
         dprintf("icmp: can't alloc pkt\n");
#endif
#endif   /* NPDEBUG */
      icmp_mib.icmpOutErrors++;
      return;
   }

   /* build the addresses in the IP header */
   pip = (struct ip *)p->nb_prot;
   /* source IP address of packet is our address (i.e., destination IP address 
    * in the datagram whose reassembly timed out) */
   pip->ip_src = ip->ip_dest;
   /* the destination address is the same as the source IP address of the 
    * datagram whose reassembly timed out */
   pip->ip_dest = ip->ip_src;

   /* move past space for IP header to get to start of ICMP header */
   p->nb_prot += sizeof(struct ip);

   tx = (struct timex *) p->nb_prot;

   tx->ttype = TIMEX;
   tx->tcode = code;
   tx->tno1 = tx->tno2 = 0;
   MEMCPY(&tx->tip, ip, (ip_hlen(ip) + ICMPTIMEX_PAYLOAD_DATA_LEN));

   tx->tchksum = 0;
   tx->tchksum = ~cksum(tx, (icmp_pkt_len>>1));

   p->nb_plen = icmp_pkt_len;
   /* p->fhost is expected to be in network byte order */
   p->fhost = pip->ip_dest;
   i = ip_write(ICMP_PROT, p);
   if (i < 0)
   {
      icmp_mib.icmpOutErrors++;
#ifdef   NPDEBUG
      if (NDEBUG & (IPTRACE|NETERR|PROTERR))
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
			TCIP_INICHE_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "ICMP: Can't send Time Exceeded"
			);
#else
         dprintf("ICMP: Can't send Time Exceeded\n");
#endif
#endif   /* NPDEBUG  */
      return;
   }
   icmp_mib.icmpOutMsgs++;
   icmp_mib.icmpOutTimeExcds++;
   return;
}


/* FUNCTION: icmp_du()
 * 
 *  process an incoming ICMP destination unreachable packet.
 *
 * PARAM1: PACKET p
 * PARAM2: struct destun * pdp
 *
 * RETURNS: void
 */

void  
icmp_du(PACKET p, struct destun * pdp)
{
   /* see if user app wants notification first */
   if (icmpdu_hook)
      icmpdu_hook(p, pdp);

#ifdef INCLUDE_TCP
   /* Tell the sockets layer so it can correct the problem. */
   so_icmpdu(p, pdp);   /* this call should free packet p */
#else

#ifndef IP_PMTU
   /* if it's a datagram-too-big message, ignore it -- As the
    * build isn't using PMTU Discovery this packet is most 
    * probably a Denial of Service Attack.
    */
    if(pdp->dcode == DSTFRAG)
    {
       goto done;
    }
#else  /* IP_PMTU */
   /* if this is a datagram-too-big message, update the Path MTU cache */
   if (pdp->dcode == DSTFRAG)
      pmtucache_set(pdp->dip.ip_dest, htons(pdp->dno2));
#endif

done:
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p); /* else just free packet */
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
#endif   /* INCLUDE_TCP */
}

#endif   /* FULL_ICMP */

#ifdef NET_STATS



/* FUNCTION: icmp_stats()
 * 
 * icmp_stats() - Printf info about icmp MIB statistics.
 *
 * PARAM1: void * pio
 *
 * RETURNS: 
 */

int
icmp_stats(void * pio)
{
   ns_printf(pio,"ICMP layer stats:\n");
   ns_printf(pio,"icmpInMsgs %lu    icmpInErrors %lu\n",
    icmp_mib.icmpInMsgs, icmp_mib.icmpInErrors);
   ns_printf(pio,"In counters: DestUnreach %lu   TimeExceed %lu   ParmProb %lu\n",
    icmp_mib.icmpInDestUnreachs, icmp_mib.icmpInTimeExcds,
    icmp_mib.icmpInParmProbs);
   ns_printf(pio,"SrcQuench %lu   Redirect %lu   Echo(ping) %lu   EchoReps %lu\n",
    icmp_mib.icmpInSrcQuenchs, icmp_mib.icmpInRedirects,
    icmp_mib.icmpInEchos, icmp_mib.icmpInEchoReps);
   ns_printf(pio,"Timestmp  %lu   TStmpRep %lu    AddrMasks %lu   AddrMaskRep %lu\n",
    icmp_mib.icmpInTimestamps, icmp_mib.icmpInTimestampReps,
    icmp_mib.icmpInAddrMasks, icmp_mib.icmpInAddrMaskReps);

   ns_printf(pio,"icmpOutMsgs %lu    icmpOutErrors %lu\n",
    icmp_mib.icmpOutMsgs, icmp_mib.icmpOutErrors);
   ns_printf(pio,"Out counts:  DestUnreach %lu   TimeExceed %lu   ParmProb %lu\n",
    icmp_mib.icmpOutDestUnreachs, icmp_mib.icmpOutTimeExcds,
    icmp_mib.icmpOutParmProbs);
   ns_printf(pio,"SrcQuench %lu   Redirect %lu   Echo(ping) %lu   EchoReps %lu\n",
    icmp_mib.icmpOutSrcQuenchs, icmp_mib.icmpOutRedirects,
    icmp_mib.icmpOutEchos, icmp_mib.icmpOutEchoReps);
   ns_printf(pio,"Timestmp  %lu   TStmpRep %lu    AddrMasks %lu   AddrMaskRep %lu\n",
    icmp_mib.icmpOutTimestamps, icmp_mib.icmpOutTimestampReps,
    icmp_mib.icmpOutAddrMasks, icmp_mib.icmpOutAddrMaskReps);

   return 0;
}

#endif   /* NET_STATS */

/* end of file icmp.c */


