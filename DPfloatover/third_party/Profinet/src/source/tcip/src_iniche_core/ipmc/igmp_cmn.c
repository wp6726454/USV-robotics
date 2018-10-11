/*
 * v3.1 August 30, 2011 ipmc/igmp_cmn.c
 * Distributed per ref EMEA2011-3
 */
/* igmp_cmn.c 
 *
 * This file contains all of the code that is shared between IGMPv1, IGMPv2, 
 * and IGMPv3 protocol implementations.
 */

#include "ipport.h"

#if defined (IP_MULTICAST) && (defined (IGMP_V1) || defined (IGMP_V2) || defined (USE_IGMPV3))

#include "tcpport.h"

#include "../tcp/in_pcb.h"
#include "../tcp/protosw.h"

#include "igmp.h"
#include "igmp2.h"
#ifdef USE_IGMPV3
#include "igmp3.h"
#endif /* USE_IGMPV3 */
#include "igmp_cmn.h"

/* number of running IGMP timers at any given instant */
u_long igmp_timers_are_running = 0;
/* time in ticks for the next invocation of IGMP timeout routine */
u_long igmp_cticks = 0;
/* IGMPv1 and v2 statistics data structure */
struct igmp_stats igmpstats;
/* IP address for all-hosts multicast address group (224.0.0.1) */
ip_addr igmp_all_hosts_group;
/* IP address for all-routers multicast address group (224.0.0.2) */
ip_addr igmp_all_rtrs_group;

#ifdef IN_MENUS

#include "menu.h"
/* array of menu options for IGMP */
struct menu_op igmpv123_menus[] = 
{
   { "igmp",     stooges,                "IGMPv1/2/3 commands" },
   { "igmpcfg",  igmp_cfg_mode,          "configure IGMP mode of interface" },
   { "igmpstat", igmp_print_stats,       "display IGMPv1/v2/v3 statistics" },
#ifdef USE_IGMPV3
   { "igmp3cfg", igmpv3_config_param,    "configure IGMPv3 parameters" },
   { "igmp3mem", igmpv3_print_memstatus, "display IGMPv3 memory useage" },
#ifdef IGMPV3_TEST
   { "igmp3tst", igmpv3_cmd_run,         "execute IGMPv3 test commands" },
#endif /* IGMPV3_TEST */
#endif /* USE_IGMPV3 */
   { NULL, NULL, NULL }
};

/* CLI error strings */
char * igmp_cfg_err_str [] =
{
"input <ok>",
"bad value",
"missing parameter",
"bad parameter"
};

char * igmp_notsupp_str = "protocol not supported in build";
#endif   /* IN_MENUS */

/* prototypes */
void igmp_send (u_char type, struct in_multi * inm);
int igmp_validate (PACKET p);
int igmp_print_stats (void * pio);
int igmp_process_mode_transition (NET netp, u_char newmode, void * pio);

/* externs */
extern int igmpv1_input (PACKET p);
extern int igmpv2_input (PACKET p);
#ifdef IGMP_V2
extern u_char igmpv2_chk4_rtr_alert_opt (struct ip * pip);
#endif
#ifdef USE_IGMPV3
extern u_long state_change_timers_are_running;
extern u_long interface_timers_are_running;
#endif

#define LTRC_ACT_MODUL_ID 30 /* INICHE_TCIP_ITGR added for LSA traces */

/* FUNCTION: igmp_init ()
 * 
 * This function initializes the IGMP module.  It sets
 * up each link to operate in the appropriate mode
 * (IGMPv1 or IGMPv2).  It also initializes the count
 * of running IGMP timers.  (A IGMP timer is defined
 * as running of the inm_timer field of the owning 
 * multicast address data structure) is greater than zero.)  
 * It also sets up the IGMP timeout routine (igmp_fasttimo ())
 * to be invoked once every 200 ms.
 *
 * INPUT: None.
 *
 * OUTPUT: None.
 */

int igmp_init(void)
{
#if defined (IGMP_V2) || defined (USE_IGMPV3)
   NET ifp;
#endif

   /*
    * To avoid byte-swapping the same value over and over again.
    */
   igmp_all_hosts_group = htonl(INADDR_ALLHOSTS_GROUP);
   igmp_all_rtrs_group = htonl(INADDR_ALLRTRS_GROUP);

#if defined (IGMP_V2) || defined (USE_IGMPV3)
   for (ifp = (NET) netlist.q_head; ifp; ifp = ifp->n_next)
   {
      /* set countdown timer for presence of IGMPv1 router (used when admin 
       * mode is IGMPv2 or IGMPv3)
       */
      ifp->igmp_timer1 = 0;
   }
#endif

#ifdef IN_MENUS
   install_menu(igmpv123_menus);
#endif   /* IN_MENUS */

   /*
    * Call igmp_fasttimo PR_FASTHZ (5) times per second
    */
   igmp_cticks = cticks + TPS/PR_FASTHZ;

   /* there are no timers running initially */
   igmp_timers_are_running = 0;

#ifdef USE_IGMPV3
   /* perform IGMPv3-related initialization */
   igmpv3_init ();
#endif

   return IGMP_OK;
}


/* FUNCTION: igmp_input ()
 * 
 * This function is invoked by igmp_demux () to provide a
 * received IGMP packet to the IGMP module.  It first
 * validates the received packet via igmp_validate ().
 * If the validation succeeds, this function then hands
 * off the packet to the IGMPv1 or IGMPv2 protocol
 * modules for further processing.  The decision of which
 * protocol module to hand the packet off to is determined
 * by the operating mode that is configured for that link.
 *
 * The IGMPv1 and IGMPv2 protocol modules are responsible
 * for returning the received packet buffer back to the
 * free pool (after they have processed the packet).
 *
 * INPUT: Received packet.
 *
 * OUTPUT: This function can return any one of the following
 * four values: (1) return code from igmp_validate (), (2) 
 * return code from igmpv1_input (), (3) return code from 
 * igmpv2_input (), or (4) IGMP_ERR.  The latter value is 
 * returned if the operating mode is not correctly configured
 * to a valid IGMP operating mode.
 */

int igmp_input (PACKET p)
{
   u_char mode;
   int rc;
     
   ++igmpstats.igmp_total_rcvd;
   
   /* validate the received packet; if validation fails,
    * drop the packet and return */
   if ((rc = igmp_validate (p)) != IGMP_OK) goto end;

#ifdef USE_IGMPV3
   /* when using IGMPv3, update the operating mode on the ingress link */
   if (p->net->igmp_oper_mode == IGMP_MODE_V3)
   {
      igmpv3_update_operating_mode (p);
   }
#endif

   /* determine the operating mode for IGMP on the ingress link */
   mode = p->net->igmp_oper_mode;
   
   /* feed packet to IGMPv1 or IGMPv2 or IGMPv3 code based on the operating
    * mode of the ingress link */
   switch (mode)
   {
#ifdef IGMP_V1   
      case IGMP_MODE_V1:   
         return (igmpv1_input (p)); 
#endif
#ifdef IGMP_V2         
      case IGMP_MODE_V2:       
         return (igmpv2_input (p));
#endif
#ifdef USE_IGMPV3
      case IGMP_MODE_V3:
         return (igmpv3_input (p));
#endif
      default:
         ++igmpstats.igmp_bad_oper_mode;     
         rc = IGMP_ERR;
         break;
   }
   
end:   
   /* return packet buffer back to free pool */
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
      
   return rc;
}

/* FUNCTION: igmp_chk_old_version_querier_present_timo ()
 * 
 * This function checks for the "presence" of the v1 (and v2 querier, if appropriate)
 * on the link.  Links that are operating in v2 mode will check for the presence of
 * the v1 querier, whereas links that are operating in v3 mode will check for the
 * presence of both v1 and v2 queriers.
 *
 * INPUT: None.
 *
 * OUTPUT: None.
 */

void igmp_chk_old_version_querier_present_timo (void)
{
   NET ifp;

   for (ifp = (NET)(netlist.q_head); ifp; ifp = ifp->n_next)
   {
#ifdef IGMP_V2
      /* process v1 router present timer (for v2-mode links) */
      if (((ifp->igmp_admin_mode == IGMP_MODE_V2)) && (ifp->igmp_oper_mode == IGMP_MODE_V1))
      {
         if (ifp->igmp_timer1)
         {
            if (--ifp->igmp_timer1 == 0)
            {
            /* we haven't heard from the IGMPv1 router for a duration
             * greater than or equal to Version 1 Router Present Timeout 
             * (IGMPv1_RTR_PRESENT_TMO), and switch back to an operational  
             * mode of IGMPv2.
             */
            igmp_del_all_existing_igmp_timers (ifp);
            igmp_upd_oper_mode_transition_stats (ifp, IGMP_MODE_V2);
            ifp->igmp_oper_mode = IGMP_MODE_V2;
            }
         }
      }
#endif

#ifdef USE_IGMPV3
      igmpv3_querier_present_timo(ifp);
#endif
   }

   return;
}

/* FUNCTION: igmp_fasttimo ()
 * 
 * This function is invoked by inet_timer () to periodically 
 * service the IGMPv1, IGMPv2 and IGMPV3 modules. If IGMPv3 is
 * enabled, it decrements Older Version Querier Present Timers,
 * if any. It traverses thru' all of the interfaces in a system,
 * and decrements all running IGMP timer and State-Change timer
 * per group.
 *
 * A IGMP timer is defined as running if the inm_timer field of
 * the owning multicast address data structure is greater than
 * zero. If the decremented timer reaches zero, this function
 * sends out a IGMP report for the relevant multicast group. If
 * IGMPv3 is snabled, this report is a IGMPv3 Current-State report
 * sent in response to a IGMPv3 Query.
 *
 * A state-change timer is defined as running in IGMPV3 mode if the
 * inm_source->ims_timer field of the owning multicast address data
 * structure is greater than zero. If the decremented timer reaches
 * zero, this fucntion retransmits a State-Change Report. 
 * 
 * If the link is running in IGMPv2 mode and also has a 
 * v1 router "present", this function checks to determine
 * if it has heard from the router in the last IGMPv1_RTR_PRESENT_TMO 
 * seconds.  If it hasn't, the v1 router is marked "absent".
 * Subsequent query requests from the v1 router will again
 * lead the v1 router to be marked as present on the link.
 *
 * Once a timer has expired, this function decrements the
 * count of running timers by one.  This function also sets
 * up the tick counter for its next invocation (by inet_timer ())
 * (which is 200 ms into the future). 
 *
 * INPUT: None.
 *
 * OUTPUT: None.
 */

void igmp_fasttimo (void)
{
   struct in_multi * inm;
   NET ifp;

   LOCK_NET_RESOURCE (NET_RESID);

   /* Setup time for the next call into igmp_fasttimo ()
    * (200 ms later). */
   igmp_cticks = cticks + TPS/PR_FASTHZ;

   igmp_chk_old_version_querier_present_timo ();

   /* check to determine if any work needs to be done */
   if (
       !igmp_timers_are_running
#ifdef USE_IGMPV3
       && !interface_timers_are_running 
       && !state_change_timers_are_running
#endif
      )
   {
      UNLOCK_NET_RESOURCE (NET_RESID);
      return;
   }

   for (ifp = (NET)(netlist.q_head); igmp_timers_are_running && ifp; ifp = ifp->n_next)
   {
      /* process each of the multicast groups registered on an interface */
      for (inm = ifp->mc_list; inm; inm = inm->inm_next)
      {
         /* skip IPv6 entries */
         if (inm->inm_addr == 0) 
            continue;

         if (inm->inm_timer == 0)   /* timer not set */
         {
            /* do nothing */
         }
         else if (--inm->inm_timer == 0)  /* timer expired */
         {
            /* send membership report in appropriate format */
            if (inm->inm_netp->igmp_oper_mode == IGMP_MODE_V1)
            {
               igmp_send (IGMP_HOST_MEMBERSHIP_REPORT, inm);
            }
            else if (inm->inm_netp->igmp_oper_mode == IGMP_MODE_V2)
            {
               igmp_send (IGMPv2_MEMBERSHIP_REPORT, inm);
               /* for IGMPv2, indicate that we were the last to send 
                * a Report for this multicast group.
                */
               inm->last2send_report = IGMP_TRUE;
            }
#ifdef USE_IGMPV3
            else
            {
               igmpv3_process_inm_timeout (inm);
            }
#endif
            
            /* decrement the count of running IGMP timers */
            if (--igmp_timers_are_running == 0)
               break;
         }
         else
         {
            /* timer is still counting down */
         }
      } 
   }

#ifdef USE_IGMPV3
   /* perform additional IGMPv3-related timeout processing */
   igmpv3_fasttimo ();
#endif

   UNLOCK_NET_RESOURCE (NET_RESID);

   return;
}

/* FUNCTION: igmp_send ()
 * 
 * This function is used by the IGMPv1 and IGMPv2 protocol modules
 * to create and transmit a packet of a particular type.  IGMPv2
 * Membership Reports and Leave Group messages have the IP Router
 * Alert option appended to them.
 *
 * IGMPv2 Leave Group messages are addressed to the all-routers
 * multicast group; all other messages are addressed to the
 * relevant group itself.  The egress interface for the message
 * is the interface on which the multicast group address data
 * structure resides.
 *
 * When sending a packet that requires options in the IP header,
 * this function invokes ip_write2 () to transmit the outgoing
 * packet.  The third parameter to ip_write2 () is a list of
 * IP options.  The list is terminated by the EOL option.  If
 * the list of IP options only contains the EOL option, the
 * ip_write2 () function does not include any options in the
 * outgoing packet.  (Note that the stack does not "fully" 
 * support IP options.)
 *
 * INPUT: (1) Type of IGMP packet to be transmitted
 *        (2) Pointer to multicast group address structure for
 *            which IGMP message is being sent
 *
 * OUTPUT: None.
 */

void igmp_send (u_char type, struct in_multi * inm)
{
   PACKET p;
   struct igmp * igmp;
   struct ip_moptions * imop;
   struct ip_moptions simo;
   struct ip * pip;
   int i;
   u_char * tmpp;
   u_char opts [2] = {IP_RTR_ALERT_OPT, EOL_OPT};
   u_char * optp;
   u_char reqd_len;

   /* compute length of buffer required for outgoing packet.
    * also account for the length of the IP Router Alert 
    * option, if required. */   
#ifdef USE_IGMPV3
   reqd_len = MaxLnh + sizeof (struct ip) + IGMP_MINLEN;
#else
   reqd_len = MaxLnh + sizeof (struct ip) + sizeof (struct igmp);
#endif
   if ((type == IGMPv2_LEAVE_GROUP) || 
       (type == IGMPv2_MEMBERSHIP_REPORT))
   {
      reqd_len += IP_RTR_ALERT_OPT_SIZE;
   }

   /* obtain a packet to send the IGMP message */
   LOCK_NET_RESOURCE (FREEQ_RESID);
   p = pk_alloc (reqd_len);
   UNLOCK_NET_RESOURCE (FREEQ_RESID);
   
   /* log an error and return if the allocation fails */   
   if (!p)
   {
      ++igmpstats.igmp_pkt_alloc_fail;
      return;
   }

   /* Need to fill in the source and destination ip addresses */
   pip = (struct ip *) p->nb_prot;
   pip->ip_src = inm->inm_netp->n_ipaddr;
   /* Leave Group messages are sent to the all-routers multicast group */
   if (type == IGMPv2_LEAVE_GROUP)
      /* igmp_all_rtrs_group is already in network byte order */
      pip->ip_dest = igmp_all_rtrs_group;
   else
      pip->ip_dest = inm->inm_addr;
   
   p->fhost = pip->ip_dest;

   tmpp = (((u_char *) p->nb_prot) + sizeof (struct ip));

   /* when transmitting an IGMP packet, our IGMP module will insert
    * data for the Router Alert option in the following types of
    * packets: Version 2 Membership Report (0x16) and Leave Group 
    * (0x17) */
   if ((type == IGMPv2_LEAVE_GROUP) || 
       (type == IGMPv2_MEMBERSHIP_REPORT))
   {
      /* provide space for ip_write2 () to write option-related data */
      tmpp += IP_RTR_ALERT_OPT_SIZE;
      optp = &(opts [0]); /* one option (IP Router Alert) */
   }
   /* outgoing packet does not require any options */
   else 
      optp = &(opts [1]);
   /* point to the start of the IGMP header */
   igmp = (struct igmp *) tmpp;
   
   igmp->igmp_type = type;
   igmp->igmp_code = 0;
   
   /* all messages (Report or Leave) have Group Address field 
    * set to the group being reported or left */
   igmp->igmp_group = inm->inm_addr;
   igmp->igmp_cksum = 0;
   igmp->igmp_cksum = ~cksum((void*)igmp, IGMP_MINLEN>>1);

   imop = &simo;
   MEMSET(imop, 0, sizeof(simo));
   imop->imo_multicast_netp = inm->inm_netp;
   imop->imo_multicast_ttl = 1;
   /* we do not want our own reports to be looped back */
   imop->imo_multicast_loop = 0;

   /* set nb_prot to point to the beginning of the IGMP data,
    * and nb_plen to the length of the IGMP data, and attach
    * the multicast options structure to the outgoing packet */
   p->nb_prot = (char *) tmpp;
#ifdef USE_IGMPV3
   p->nb_plen = IGMP_MINLEN;
#else
   p->nb_plen = sizeof(struct igmp);
#endif
   p->imo = imop;
   
   i = ip_write2 (IGMP_PROT, p, optp);

   if (type == IGMP_HOST_MEMBERSHIP_REPORT)
      ++igmpstats.igmp_v1_reports_sent;
#ifdef IGMP_V2
   else if (type == IGMPv2_LEAVE_GROUP)
      ++igmpstats.igmpv2mode_v2_leave_msgs_sent;
   else if (type == IGMPv2_MEMBERSHIP_REPORT)
      ++igmpstats.igmpv2mode_v2_reports_sent;
#endif
}

/* FUNCTION: igmp_joingroup ()
 * 
 * This function is used to send the first IGMP Membership Report
 * (IGMPv1 or IGMPv2, as appropriate) for a group on the device.
 * Membership in the 224.0.0.1 group is not reported.  After
 * sending the report, this function also starts a timer that will
 * trigger a second transmission of the report (unless it is
 * supressed by a received report).
 *
 * INPUT: (1) Pointer to multicast group address structure for
 *            which IGMP Membership Report message is being sent
 *
 * OUTPUT: None.
 */
 
void igmp_joingroup(struct in_multi * inm)
{
   NET ifp;

   /* extract the network interface to which this multicast
    * address is "attached" */
   ifp = inm->inm_netp;

   if (inm->inm_addr == igmp_all_hosts_group)
   {
      inm->inm_timer = 0;
   }
   else 
   {
      /* send unsolicited membership report in appropriate format */
      if (ifp->igmp_oper_mode == IGMP_MODE_V1)
      {
         igmp_send (IGMP_HOST_MEMBERSHIP_REPORT, inm);
         /* set a delay timer (with a duration of 
          * IGMP_MAX_HOST_REPORT_DELAY) for a second unsolicited report */
         inm->inm_timer = (unsigned) IGMP_RANDOM_DELAY(inm->inm_addr);
      }
      else
      {
         igmp_send (IGMPv2_MEMBERSHIP_REPORT, inm);
         /* the delay time duration is the Unsolicited Report Interval */
         inm->inm_timer = (unsigned) IGMPv2_RANDOM_DELAY ((UNSOLIC_RPT_INTERVAL * PR_FASTHZ), inm->inm_addr);
      }
      
      /* for IGMPv2, indicate that we were the last to send 
       * a Report for this multicast group (relevant for 
       * IGMPv2 only). */
      if (ifp->igmp_oper_mode == IGMP_MODE_V2)
      {
         inm->last2send_report = IGMP_TRUE;
      }

      ++igmp_timers_are_running;
   }
   
   return;
}

/* FUNCTION: igmp_leavegroup ()
 * 
 * This function is used to send the IGMPv2 Leave Group message.
 * for a group on the device.  This occurs when the reference
 * count for a group's membership becomes zero.  This function
 * will only send a Leave Group message when the link (on which
 * the multicast address group structure resides) is configured 
 * for IGMPv2 operation and has not been downgraded to IGMPv1
 * (because of the presence of a IGMPv1 router on the link).
 *
 * It is possible that we skip sending a Leave Group message 
 * even though we are the only member of a group.  Consider
 * the case where the host that last responded to a query has since
 * then left the group.  If we decide to leave immediately after
 * that, we will skip sending the leave message.  In such a
 * scenario, the Group Membership Interval timer in the IGMP
 * router will help the router determine when there are no
 * members of the group on that link.
 *
 * INPUT: (1) Pointer to multicast group address structure for
 *            which IGMPv2 Leave Group message is being sent.
 *
 * OUTPUT: None.
 */

void igmp_leavegroup (struct in_multi * inm)
{
   NET ifp;
   
   ifp = inm->inm_netp;
   
   if (ifp->igmp_oper_mode == IGMP_MODE_V2)
   {
      if (inm->last2send_report == IGMP_TRUE)
         igmp_send (IGMPv2_LEAVE_GROUP, inm);
   }
   
   return;
}

/* FUNCTION: igmp_validate ()
 * 
 * This function is used to validate the contents of a 
 * received packet.  It performs the following tests on
 * a received message.
 *
 * (a) validate the length of IGMP message
 * (b) validate the checksum
 * (c) validate that an IGMPv1 Host Membership Query, an  
 *     IGMPv2 General Query or an IGMPv3 General Query is sent
 *     to the all hosts-address
 * (d) validate that IGMPv2 Group-Specific Query carries
 *     a valid multicast group address, and is not addressed
 *     to 224.0.0.1 multicast group
 * (e) validate that the Group Address field in a received 
 *     IGMP Membership Report is identical to the destination
 *     address field in the IP header, and that the group
 *     address is a valid multicast group address
 * (f) validate that a received IGMPv2 General Query, IGMPv2 
 *     Group-Specific Query, IGMPv2 Membership Report, and 
 *     IGMPv2 Leave Group message includes the IP Router
 *     Alert option.
 *
 * Note.  This function does not free the packet even if
 * the validation fails.
 *
 * INPUT: (1) Received IGMP packet.
 *
 * OUTPUT: This function returns ENP_BAD_HEADER if the
 *         validation fails; otherwise, it returns
 *         IGMP_OK.
 */

int igmp_validate (PACKET p)
{
   struct ip * pip;
   int igmplen;
   struct igmp * igmp;
   u_short osum;
   u_short xsum;  
   u_char type;
   ip_addr mcgrp_addr;
   u_char resp_time;
#if defined(IGMP_V2) || defined(USE_IGMPV3)
   u_char qv = 0;       /* query version */
#endif
#ifdef USE_IGMPV3
   u_short num_src = 0; /* number of sources */
#endif

   pip = ip_head (p);

   /* compute length of IGMP packet (after accounting for IP header, 
    * including the IP Router Alert option (if present)) */
   igmplen = p->nb_plen - ip_hlen (pip);

   /* validate length (IGMP_MINLEN is 8 bytes) */
   if (igmplen < IGMP_MINLEN)
   {
      ++igmpstats.igmp_badlen_rcvd;
      return ENP_BAD_HEADER;
   }

   /* validate checksum */
   igmp = (struct igmp *) (ip_data (pip));
   osum = igmp->igmp_cksum;
   igmp->igmp_cksum = 0;
   xsum = ~cksum(igmp, igmplen>>1);
   if (xsum != osum)
   {
      igmp->igmp_cksum = osum;
      ++igmpstats.igmp_badsum_rcvd;
      return ENP_BAD_HEADER;
   }
   
   /* extract the IGMP packet type, Group Address, and Max Response Time 
    * (unused for IGMPv1) fields from received packet */
   type = igmp->igmp_type;
   mcgrp_addr = ntohl(igmp->igmp_group); 
   resp_time = igmp->igmp_code;
      
   if (type == IGMP_HOST_MEMBERSHIP_QUERY)
   {
#ifdef USE_IGMPV3
      if (igmplen == IGMP_MINLEN)
      {
         if (resp_time == 0)
            qv = IGMP_QUERY_V1;
         else
            qv = IGMP_QUERY_V2;
      }
      else if (igmplen >= IGMPv3_QUERY_MINLEN)
      {
         qv = IGMP_QUERY_V3;
         num_src = ntohs(igmp->igmp_numsrc);
      }
      else
      {
         /* igmplen > 8 && igmplen < 12 */
         ++igmpstats.igmp_badlen_rcvd;
         return ENP_BAD_HEADER;
      }

      /* Validate IGMPv3 Query packet length */
      if ((qv == IGMP_QUERY_V3) &&
          (igmplen != (int)(IGMPv3_QUERY_MINLEN + (num_src * sizeof(ip_addr)))))
      {
         ++igmpstats.igmp_badlen_rcvd;
         return ENP_BAD_HEADER;
      }

      if ((qv == IGMP_QUERY_V1) ||
          ((qv == IGMP_QUERY_V2) && (mcgrp_addr == 0)) ||
          ((qv == IGMP_QUERY_V3) && (mcgrp_addr == 0) && (num_src == 0)))
#else
      if ((resp_time == 0) || /* IGMPv1 Query */
          ((resp_time > 0) && (mcgrp_addr == 0))) /* IGMPv2 General Query */     
#endif
      {
         /* if this is a IGMPv1 Host Membership Query or a IGMPv2/ v3 
          * General Query, it must be addressed to the all-hosts 
          * group */
         if (pip->ip_dest != igmp_all_hosts_group) 
         {
            ++igmpstats.igmp_bad_queries_rcvd;
            return ENP_BAD_HEADER;
         }     
      }
      
#ifdef USE_IGMPV3
      if ((qv != IGMP_QUERY_V1) && (mcgrp_addr != 0))
#else
      if ((resp_time > 0) && (mcgrp_addr != 0))
#endif
      {
         /* this is a IGMPv2/ v3 Group-Specific Query. */
         if (p->net->igmp_oper_mode == IGMP_MODE_V1)
         {
            /* IGMPv1 RFC 1112 suggests ignoring "Unused" and "Group Address"
             * fields in a received Query. Doing so and processing the Query
             * would result in sending report for all joined groups including
             * the ones not queried. As report in response to an IGMPv2/v3
             * General Query would refresh router's forwarding state, we will
             * drop this packet. 
             *
             * IGMPv1 code does not understand a IGMPv2/v3 Group-Specific Query 
             */
            return ENP_BAD_HEADER; 
         }
         
         /* check to make sure that the group address field carries
          * a valid multicast address; if it doesn't, we
          * drop the packet.  Also drop packets that
          * carry the multicast address for the all-hosts
          * group. */
#if defined(IGMP_V2) || defined(USE_IGMPV3)
         if ((!IN_MULTICAST(mcgrp_addr)) ||
             /* igmp_all_hosts_group is already in network byte order */
             (igmp->igmp_group == igmp_all_hosts_group))
         {
            if (qv == IGMP_QUERY_V2)
               ++igmpstats.igmp_v2_bad_grp_specific_queries_rcvd;
#ifdef USE_IGMPV3
            else if (num_src == 0)
               ++igmpstats.igmp_v3_bad_grp_specific_queries_rcvd;
            else
               ++igmpstats.igmp_v3_bad_grp_src_specific_queries_rcvd;
#endif
            /* caller will free received packet */
            return ENP_BAD_HEADER;
         }
#endif /* V2 or V3 */
      }      
   }         
   
   /* check to ensure that a received IGMPv1 or v2 Report has the 
    * same IP host group address in its IP destination field and 
    * its IGMP group address field, and that the group address is
    * a valid multicast address */
   if ((type == IGMP_HOST_MEMBERSHIP_REPORT) ||
       (type == IGMPv2_MEMBERSHIP_REPORT))
   {
      if ((igmp->igmp_group != pip->ip_dest) ||
          (!IN_MULTICAST(mcgrp_addr)))
      {
         ++igmpstats.igmp_bad_reports_rcvd;
         return ENP_BAD_HEADER;    
      }
   }      

   /*
    * After receiving an IGMP packet, our IGMP module will check for the
    * presence of the Router Alert option in the following types of
    * packets.  Packets that do not have the option will be discarded.

    * (a) Version 2 General Query (0x11) (differentiated from a Version 1
          Host Membership Query by having a Max Response Time > 0)
    * (b) Version 2 Group-Specific Query (0x11)
    * (c) Version 2 Membership Report (0x16)
    * (d) Version 2 Leave Group (0x17)
    * (d) Version 3 Query (0x11)

    * Version 1 Host Membership Reports and Version 1 Host Membership Query
    * packets will not be checked for the IP Router Alert option.
    */
#if defined(IGMP_V2) || defined(USE_IGMPV3)
   if ((type == IGMPv2_LEAVE_GROUP) || 
       (type == IGMPv2_MEMBERSHIP_REPORT) ||
#ifdef USE_IGMPV3
       ((type == IGMP_HOST_MEMBERSHIP_QUERY) && (qv != IGMP_QUERY_V1))
#else
       ((type == IGMP_HOST_MEMBERSHIP_QUERY) && (igmp->igmp_code > 0))
#endif /* USE_IGMPV3 */
      )
   {
      if (!igmpv2_chk4_rtr_alert_opt (pip))
      { 
         if (!((type == IGMP_HOST_MEMBERSHIP_QUERY) && (qv == IGMP_QUERY_V3)))
            ++igmpstats.igmp_v2_rtr_alert_missing;
#ifdef USE_IGMPV3
         else
            ++igmpstats.igmp_v3_rtr_alert_missing;
#endif /* USE_IGMPV3 */
         return ENP_BAD_HEADER;
      }
   }
#endif /* V2 || V3 */

   /* validation successful */
   return IGMP_OK;
}

/* FUNCTION: igmp_upd_oper_mode_transition_stats ()
 * 
 * This function updates host compatibility step-up/ step-down event
 * statistics.  It expects that the current operating mode is unchanged
 * prior to its invocation.
 *
 * INPUT: (1) Pointer to network interface experiencing a transition
 *            in the IGMP mode.
 *        (2) New operating mode 
 *
 * RETURN: None 
 */

void igmp_upd_oper_mode_transition_stats (NET netp, u_char newmode)
{
   u_char curmode;

   /* extract the current operating mode for IGMP on the link */
   curmode = netp->igmp_oper_mode;

   switch (curmode)
   {
      case IGMP_MODE_V1:
         switch (newmode)
         {
#ifdef IGMP_V2
            case IGMP_MODE_V2:
               igmpstats.igmp_oper_mode_stepped_up_from_v1_to_v2++;
               break;
#endif
#ifdef USE_IGMPV3
            case IGMP_MODE_V3:
               igmpstats.igmp_oper_mode_stepped_up_from_v1_to_v3++;
               break;
#endif
         }
         break;
#ifdef IGMP_V2
      case IGMP_MODE_V2:
         switch (newmode)
         {
            case IGMP_MODE_V1:
               igmpstats.igmp_oper_mode_stepped_down_from_v2_to_v1++;
               break;
#ifdef USE_IGMPV3
            case IGMP_MODE_V3:
               igmpstats.igmp_oper_mode_stepped_up_from_v2_to_v3++;
               break;
#endif
         }
         break; 
#endif
#ifdef USE_IGMPV3
      case IGMP_MODE_V3:
         switch (newmode)
         {
            case IGMP_MODE_V1:
               igmpstats.igmp_oper_mode_stepped_down_from_v3_to_v1++;
               break;
            case IGMP_MODE_V2:
               igmpstats.igmp_oper_mode_stepped_down_from_v3_to_v2++;
               break;
         }
         break; 
#endif
   }
#ifdef NPDEBUG
#if 1 /* INICHE_TCIP_ITGR added LSA trace */
	TCIP_INICHE_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
						, "IGMP: transitioning operational mode on link ifIndex(%d) from IGMPv%d to IGMPv%d..."
						, netp->mib.ifIndex, curmode, newmode);
#else
   dprintf("IGMP: transitioning operational mode on link %s from IGMPv%d to IGMPv%d...\n", netp->name, curmode, newmode); 
#endif
#endif
}

#ifdef IN_MENUS

/* FUNCTION: igmp_print_stats ()
 * 
 * This function is invoked by the CLI command parser in response
 * to the user's "igmp" command.  It prints out various statistics
 * related to the IGMPv1, IGMPv2 and IGMPv3 modules.  Note that these
 * statistics are for the protocol module (and are across all
 * links).  The statistics are grouped into four categories:
 * receive-related, transmit-related, error-related, filtering API-related, 
 * and miscellaneous.
 *
 * INPUT: Pointer to I/O device where the output will be directed
 *        to.
 *
 * OUTPUT: This function always returns IGMP_OK.
 */
   
int igmp_print_stats (void * pio)
{  
   NET ifp;
   struct igmp_params igmp_nparams;
   struct igmp_stats lcligmpstats;
   u_long lcligmp_timers_are_running;
#ifdef USE_IGMPV3
   u_long lclinterface_timers_are_running;
   u_long lclstate_change_timers_are_running;
#endif
  
   for (ifp = (NET)(netlist.q_head); ifp; ifp = ifp->n_next)
   {
      if (!(ifp->n_flags & NF_MCAST))
         continue;

      LOCK_NET_RESOURCE (NET_RESID);
      igmp_nparams.igmp_admin_mode = ifp->igmp_admin_mode;
      igmp_nparams.igmp_oper_mode = ifp->igmp_oper_mode;
#if defined (IGMP_V2) || defined (USE_IGMPV3)
      igmp_nparams.igmp_timer1 = ifp->igmp_timer1;
#endif
#ifdef USE_IGMPV3
      igmp_nparams.igmpv3_timer2 = ifp->igmpv3_timer2;
      igmp_nparams.igmpv3_timer3 = ifp->igmpv3_timer3;
      igmp_nparams.igmpv3_uri = ifp->igmpv3_uri;
      igmp_nparams.igmpv3_qrv = ifp->igmpv3_qrv;
      igmp_nparams.igmpv3_qqi = ifp->igmpv3_qqi;
      igmp_nparams.igmpv3_qri = ifp->igmpv3_qri;
#endif
      UNLOCK_NET_RESOURCE (NET_RESID);

      ns_printf (pio, "\nInterface %s:\n", ifp->name);
      ns_printf (pio, "admin mode: IGMPv%u, operational mode: IGMPv%u\n",  
                 igmp_nparams.igmp_admin_mode, 
                 igmp_nparams.igmp_oper_mode);

#if defined (IGMP_V2) || defined (USE_IGMPV3)
      if ((ifp->igmp_admin_mode == IGMP_MODE_V2) || (ifp->igmp_admin_mode == IGMP_MODE_V3))
      {
         if (igmp_nparams.igmp_timer1)
            ns_printf (pio, "v1 querier present timer: %lu seconds\n", igmp_nparams.igmp_timer1 / PR_FASTHZ);
         else
            ns_printf (pio, "v1 querier: absent\n");
      }
#endif

#ifdef USE_IGMPV3
      /* print IGMPv3 timers */
      if (ifp->igmp_admin_mode == IGMP_MODE_V3)
      {
         if (igmp_nparams.igmpv3_timer2)
            ns_printf (pio, "v2 querier present timer: %lu seconds\n", igmp_nparams.igmpv3_timer2 / PR_FASTHZ);
         else
            ns_printf (pio, "v2 querier: absent\n");


         ns_printf (pio, "v3 general query (interface) timer: %lu ticks (%lu seconds)\n",
                    (igmp_nparams.igmpv3_timer3 * TPS)/ PR_FASTHZ, igmp_nparams.igmpv3_timer3 / PR_FASTHZ);

         ns_printf (pio, "v3 unsolicited report interval: %1lu seconds\n", igmp_nparams.igmpv3_uri);
         ns_printf (pio, "v3 querier robustness variable: %1lu \n", igmp_nparams.igmpv3_qrv);
         ns_printf (pio, "v3 querier query interval: %1lu seconds\n", igmp_nparams.igmpv3_qqi);
         ns_printf (pio, "v3 query response interval: %1lu seconds\n", igmp_nparams.igmpv3_qri);
      }
#endif  /* USE_IGMPV3 */
   }

   /* print various statistics */
   LOCK_NET_RESOURCE (NET_RESID);
   lcligmp_timers_are_running = igmp_timers_are_running;
#ifdef USE_IGMPV3
   lclinterface_timers_are_running = interface_timers_are_running;
   lclstate_change_timers_are_running = state_change_timers_are_running;
#endif
   MEMCPY (&lcligmpstats, &igmpstats, sizeof (struct igmp_stats));
   UNLOCK_NET_RESOURCE (NET_RESID);

   /* rx statistics */  
   ns_printf (pio, "\n[Rx]\n");
   ns_printf (pio, "IGMP messages rcvd: %lu\n", lcligmpstats.igmp_total_rcvd);
   ns_printf (pio, "IGMPv1 Host Membership Queries rcvd (by v1-mode links): %lu\n", lcligmpstats.igmpv1mode_v1_queries_rcvd);
   ns_printf (pio, "IGMPv1 Host Membership Reports rcvd: %lu\n", lcligmpstats.igmpv1mode_v1_reports_rcvd);
   ns_printf (pio, "IGMP Host Membership Reports rcvd causing timer cancellation: %lu\n", lcligmpstats.igmpv1mode_v1_reports_rcvd_canceled_timer);

#ifdef IGMP_V2      
   ns_printf (pio, "IGMPv1 Host Membership Queries rcvd (by v2-mode links): %lu\n", lcligmpstats.igmpv2mode_v1_queries_rcvd);
   ns_printf (pio, "IGMPv2 General Queries rcvd: %lu, Group-Specific Queries rcvd: %lu\n", lcligmpstats.igmpv2mode_v2_general_queries_rcvd, lcligmpstats.igmpv2mode_v2_grp_specific_queries_rcvd);
   ns_printf (pio, "IGMP Host Membership Reports rcvd causing timer cancellation: %lu\n", lcligmpstats.igmpv2mode_v12_reports_rcvd_canceled_timer);   
   ns_printf (pio, "IGMP Host Membership Reports rcvd with no local timer: %lu\n", lcligmpstats.igmpv2mode_v12_reports_rcvd_no_timer);
   ns_printf (pio, "IGMPv2 Leave Group messages rcvd: %lu\n", lcligmpstats.igmpv2mode_v2_leave_msgs_rcvd);
#endif

#ifdef USE_IGMPV3
   ns_printf (pio, "IGMPv1 Host Membership Queries rcvd (by v3-mode links): %lu\n", lcligmpstats.igmpv3mode_v1_queries_rcvd);
   ns_printf (pio, "IGMPv2 General Queries rcvd (by v3-mode links): %lu\n", lcligmpstats.igmpv3mode_v2_general_queries_rcvd);
   ns_printf (pio, "IGMPv3 General Queries rcvd: %lu, Group-Specific Queries rcvd: %lu\n", lcligmpstats.igmpv3mode_v3_general_queries_rcvd, lcligmpstats.igmpv3mode_v3_grp_specific_queries_rcvd);
   ns_printf (pio, "IGMPv3 Group-and-Source-Specific Queries rcvd: %lu\n", lcligmpstats.igmpv3mode_v3_grp_src_specific_queries_rcvd);
#endif /* USE_IGMPV3 */
   
   /* tx statistics */
   ns_printf (pio, "\n[Tx]\n");
   ns_printf (pio, "IGMPv1 Host Membership Reports sent: %lu\n", lcligmpstats.igmp_v1_reports_sent);

#ifdef IGMP_V2
   ns_printf (pio, "IGMPv2 Leave Group messages sent: %lu, Membership Reports sent: %lu\n", lcligmpstats.igmpv2mode_v2_leave_msgs_sent, lcligmpstats.igmpv2mode_v2_reports_sent);
#endif

#ifdef USE_IGMPV3
   ns_printf (pio, "IGMPv3 Host Membership Reports sent: %lu\n", lcligmpstats.igmpv3mode_v3_reports_sent);
   ns_printf (pio, "IGMPv3 truncated Host Membership Reports sent: %lu\n", lcligmpstats.igmpv3mode_v3_truncated_reports_sent);
#endif /* USE_IGMPV3 */

   /* error statistics */
   ns_printf (pio, "\n[Error]\n");
   ns_printf (pio, "IGMP packets rcvd with bad length: %lu, bad checksum: %lu\n", lcligmpstats.igmp_badlen_rcvd, lcligmpstats.igmp_badsum_rcvd);
   ns_printf (pio, "Packet buffer allocation failures: %lu, Bad IGMP Oper Mode config: %lu\n", lcligmpstats.igmp_pkt_alloc_fail, lcligmpstats.igmp_bad_oper_mode);
   ns_printf (pio, "Bad IGMP Queries rcvd: %lu, Bad IGMP Reports rcvd: %lu\n", lcligmpstats.igmp_bad_queries_rcvd, lcligmpstats.igmp_bad_reports_rcvd); 
   ns_printf (pio, "IGMP packets of unknown type rcvd by v1-mode links: %lu\n", lcligmpstats.igmpv1mode_unknown_pkttype);

#ifdef IGMP_V2
   ns_printf (pio, "Bad IGMPv2 Group-Specific Queries rcvd: %lu\n", lcligmpstats.igmp_v2_bad_grp_specific_queries_rcvd);
   ns_printf (pio, "IGMPv2 Group-Specific Queries rcvd with unknown Group Address: %lu\n", lcligmpstats.igmpv2mode_v2_unknown_grp_specific_queries_rcvd);
   ns_printf (pio, "IGMP Membership Reports rcvd with unknown Group Address: %lu\n", lcligmpstats.igmpv2mode_v12_unknown_grp_reports_rcvd);
   ns_printf (pio, "Number of IGMPv2 messages rcvd without Router Alert option: %lu\n", lcligmpstats.igmp_v2_rtr_alert_missing);
   ns_printf (pio, "IGMP packets of unknown type rcvd by v2-mode links: %lu\n", lcligmpstats.igmpv2mode_unknown_pkttype);
#endif

#ifdef USE_IGMPV3
   ns_printf (pio, "Bad IGMPv3 Group-Specific Queries rcvd: %lu\n", lcligmpstats.igmp_v3_bad_grp_specific_queries_rcvd);
   ns_printf (pio, "Bad IGMPv3 Group-and-Source-Specific Queries rcvd: %lu\n", lcligmpstats.igmp_v3_bad_grp_src_specific_queries_rcvd);
   ns_printf (pio, "Number of IGMPv3 Queries rcvd without Router Alert option: %lu\n", lcligmpstats.igmp_v3_rtr_alert_missing);  
   ns_printf (pio, "IGMP packets of unknown type rcvd by v3-mode links: %lu\n", lcligmpstats.igmpv3mode_unknown_pkttype);
#endif /* USE_IGMPV3 */

   /* Filtering-specific statistics */
#ifdef USE_IGMPV3
   ns_printf (pio, "\n[Src Filter]\n");
   ns_printf (pio, "Number of source filters added: %lu, deleted: %lu\n", lcligmpstats.igmpv3_filter_added_count, lcligmpstats.igmpv3_filter_deleted_count);
   ns_printf (pio, "Number of source filter additions/deletions that failed: %lu\n", lcligmpstats.igmpv3_filter_failed_count);
#endif /* USE_IGMPV3 */

   /* miscellaneous statistics */
   ns_printf (pio, "\n[Misc]\n");
   ns_printf (pio, "Timers running: INM %lu", lcligmp_timers_are_running);
#ifdef USE_IGMPV3
   ns_printf (pio, ", interface %lu, state change <%s>\n", lclinterface_timers_are_running, (lclstate_change_timers_are_running ? "YES" : "NO"));
#else
   ns_printf (pio, "\n");
#endif

#ifdef IGMP_V2
   ns_printf (pio, "IGMP protocol version step-down: v2->v1: %lu", lcligmpstats.igmp_oper_mode_stepped_down_from_v2_to_v1);
#ifdef USE_IGMPV3
   ns_printf (pio, ", v3->v1: %lu, v3->v2: %lu\n", lcligmpstats.igmp_oper_mode_stepped_down_from_v3_to_v1, lcligmpstats.igmp_oper_mode_stepped_down_from_v3_to_v2);
#else
   ns_printf (pio, "\n");
#endif
#endif

#ifdef IGMP_V2
   ns_printf (pio, "IGMP protocol version step-up: v1->v2: %lu", lcligmpstats.igmp_oper_mode_stepped_up_from_v1_to_v2);
#ifdef USE_IGMPV3
   ns_printf (pio, ", v1->v3: %lu, v2->v3: %lu\n", lcligmpstats.igmp_oper_mode_stepped_up_from_v1_to_v3, lcligmpstats.igmp_oper_mode_stepped_up_from_v2_to_v3);
#else
   ns_printf (pio, "\n");
#endif
#endif

   return IGMP_OK;
}

/* FUNCTION: igmp_cfg_mode ()
 * 
 * This function is invoked by the CLI command parser in response to the user's 
 * "igmpcfg" command.  It invokes igmp_process_mode_transition () to change the 
 * IGMP mode of the specified link.  A sample invocation of this CLI command is 
 * shown below.
 *
 * INET> igmpcfg mode <interface id> <IGMP version number>
 *
 * INPUT: Pointer to I/O device where the output will be directed
 *        to.
 *
 * OUTPUT: This function returns IGMP_OK if the configuration was successful;
 * otherwise, it returns IGMP_ERR.
 */

int igmp_cfg_mode (void * pio)
{
   char * cp;
   char * spp;
   int rc1;
   NET netp;
   char * termp;
   u_long mode;
   int err = 0;  

   /* extract the next argument, if any from the command line */
   cp = nextarg (((GEN_IO)pio)->inbuf);

   if (cp && (*cp))
   {
      /* check to see if the next argument is a legal one */
      spp = strchr (cp, ' ');
      if (spp) 
      {
         *spp = 0;
      }
      else
      {
         err = IGMP_CFG_MISSING_PARAM;
         goto exit;
      }

      /* check for presence of "mode" parameter */
      rc1 = stricmp (cp, "mode");
      if (rc1)
      {
         err = IGMP_CFG_BAD_PARAM;
         goto exit;
      }

      *spp = ' ';

      /* extract interface identifier */
      cp = nextarg (cp);
      if (!(cp && (*cp)))
      {
         err = IGMP_CFG_MISSING_PARAM;
         goto exit;
      }

      spp = strchr (cp, ' ');
      if (spp) 
      {
         *spp = 0;
      }
      else
      {
         err = IGMP_CFG_MISSING_PARAM;
         goto exit;
      } 

      netp = if_netbytext(pio, cp);
      if (!netp)
      {
         err = IGMP_CFG_BAD_VALUE;
         goto exit;
      }

      *spp = ' ';

      /* finally, extract IGMP version */
      cp = nextarg (cp);
      if (!(cp && (*cp)))
      {
         err = IGMP_CFG_MISSING_PARAM;
         goto exit;
      }   

      mode = strtoul (cp, &termp, 10);
      if (*termp == '\0')
      {
         if (((*cp) != '-') && (mode >= IGMP_MODE_V1) && (mode <= IGMP_MODE_V3))
         {
            ns_printf (pio, "Configuring interface %s [%u.%u.%u.%u] to run IGMPv%u...\n", 
                       netp->name, PUSH_IPADDR (netp->n_ipaddr), mode);
            LOCK_NET_RESOURCE (NET_RESID);
            err = igmp_process_mode_transition (netp, (u_char) mode, pio);
            UNLOCK_NET_RESOURCE (NET_RESID);
         }
         else
         {
            err = IGMP_CFG_BAD_VALUE;
            goto exit;
         }
      }
      else 
      {
         err = IGMP_CFG_BAD_VALUE;
         goto exit;
      }

   }
   else 
   {
      err = IGMP_CFG_MISSING_PARAM;
   }

exit:
   if (err) 
   { 
      ns_printf (pio, "Error: %s\n", igmp_cfg_err_str [err]);
      ns_printf (pio, "Useage: igmpcfg mode <interface id> <IGMP version number>\n");
      return IGMP_ERR;
   }
   
   return IGMP_OK;
}

/* FUNCTION: igmp_process_mode_transition ()
 * 
 * This function is invoked from igmp_cfg_mode () to update the IGMP mode of the
 * specified link to the new value.  Switching to the new mode requires that the
 * protocol cancel any active timers, reset operational parameters, etc.
 *
 * INPUT: (1) Pointer to network link data structure of the link whose IGMP mode
 *            is being changed.
 *        (2) New value of the IGMP mode
 *        (3) Pointer to I/O device where the output will be directed to.
 *
 * OUTPUT: This function returns IGMP_OK if the mode was successfully changed;
 * otherwise, it returns IGMP_CFG_BAD_VALUE.  The latter occurs when the user
 * attempts to transition to a mode that is not supported in the build.
 */

int igmp_process_mode_transition (NET netp, u_char newmode, void * pio)
{
   u_char old_oper_mode;

   /* check to ensure that new mode (protocol) is supported in build */
#ifndef IGMP_V1
   if (newmode == IGMP_MODE_V1)
      goto err;
#endif
#ifndef IGMP_V2
   if (newmode == IGMP_MODE_V2)
      goto err;
#endif
#ifndef USE_IGMPV3
   if (newmode == IGMP_MODE_V3)
      goto err;
#endif

   /* is new mode identical to current admin mode? */
   if (netp->igmp_admin_mode == newmode)
   {
      /* requested mode identical to existing, no action taken */
      return IGMP_OK;
   }
   
   old_oper_mode = netp->igmp_oper_mode;
   netp->igmp_admin_mode = newmode;

   /* cleanup data structures */
   switch (old_oper_mode)
   {
      case IGMP_MODE_V1:
      case IGMP_MODE_V2:
      igmp_del_all_existing_igmp_timers (netp);
      break;

#ifdef USE_IGMPV3
      case IGMP_MODE_V3:
      igmp_cancel_pending_response (netp);
      igmpv3_chk_and_upd_state_change_timers_var ();
      break;
#endif
   }

   /* cleanup all IGMP-related fields in the network data structure */
#if defined (IGMP_V2) || defined (USE_IGMPV3)
   netp->igmp_timer1 = 0;
#endif
#ifdef USE_IGMPV3
   igmpv3_parm_init (netp);
#endif

   igmp_upd_oper_mode_transition_stats (netp, newmode);

   /* finally, set the new operating mode */
   netp->igmp_oper_mode = newmode;
   
   return IGMP_OK;

#if !defined(IGMP_V1) || !defined (IGMP_V2) || !defined (USE_IGMPV3)
err:
   ns_printf (pio, "IGMPv%u %s\n", newmode, igmp_notsupp_str);
   return IGMP_CFG_BAD_VALUE;
#endif
}

#endif /* IN_MENUS */

/* FUNCTION: igmp_del_all_existing_igmp_timers ()
 * 
 * This function deletes all active timers on a given network interface.  These
 * timers were started earlier in response to a v1 or v2 query.
 *
 * INPUT: (1) Pointer to network link data structure of the link whose timers
 *            need to be deleted.
 *
 * OUTPUT: None.
 */

void igmp_del_all_existing_igmp_timers (NET netp)
{
   struct in_multi * inm;

   for (inm = netp->mc_list; inm; inm = inm->inm_next)
   {
       /* skip all IPv6 entries - they are indicated by 
        * an IPv4 address field of 0 */
       if (!(inm->inm_addr)) continue;
       if (inm->inm_timer) 
       {
          inm->inm_timer = 0;
          --igmp_timers_are_running;
       }
   } /* end FOR (iterate thru' mc_list on net) */

   return;
}

#endif /* IP_MULTICAST and (IGMPv1 or IGMPv2 or IGMPv3) */
