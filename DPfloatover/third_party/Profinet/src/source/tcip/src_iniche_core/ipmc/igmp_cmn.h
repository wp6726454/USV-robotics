/*
 * v3.1 August 30, 2011 ipmc/igmp_cmn.h
 * Distributed per ref EMEA2011-3
 */
/* igmp_cmn.h
 * 
 * Copyright 2006 - 2009 by InterNiche Technologies, Inc.  All rights reserved.
 *
 * Companion header file to igmp_cmn.c.
 */
 
#ifndef IGMP_CMN_H
#define IGMP_CMN_H

#ifdef IP_MULTICAST
 
/* each link has its own IGMP operating mode: it can
 * choose to run either IGMPv1 or IGMPv2 or IGMPv3.
 */ 
#define IGMP_MODE_V1 0x1
#define IGMP_MODE_V2 0x2
#define IGMP_MODE_V3 0x3

/*
 * IGMP Query version
 */
#define IGMP_QUERY_V1 IGMP_MODE_V1
#define IGMP_QUERY_V2 IGMP_MODE_V2
#define IGMP_QUERY_V3 IGMP_MODE_V3


/* If the configuration file for the TCP/IP stack does 
 * not specify a particular IGMP protocol to run on a link, 
 * the software uses the following logic to select the
 * IGMP protocol on that link.
 */
#if defined (USE_IGMPV3)
#define IGMP_MODE_DEFAULT IGMP_MODE_V3
#elif defined (IGMP_V1) && !defined (IGMP_V2)
#define IGMP_MODE_DEFAULT IGMP_MODE_V1
#elif defined (IGMP_V1) && defined (IGMP_V2)
#define IGMP_MODE_DEFAULT IGMP_MODE_V2
#endif

#define IGMP_TRUE 1
#define IGMP_FALSE 0

#define IGMP_ERR -1
#define IGMP_FAIL -1
#define IGMP_OK 0

#define IGMP_CFG_PARAM_OK 0
#define IGMP_CFG_BAD_VALUE 1
#define IGMP_CFG_MISSING_PARAM 2
#define IGMP_CFG_BAD_PARAM 3

/* length of IGMP message */
#define  IGMP_MINLEN 8
 
/*
IGMPv1, IGMPv2, and IGMPv3 statistics data structure:

(1) igmp_total_rcvd: 
Total number of IGMP (v1 or v2) messages received.

(2) igmpv1mode_v1_queries_rcvd:
Total number of IGMPv1 Host Membership Queries received
by device on a link operating in IGMPv1 mode

(3) igmpv1mode_v1_reports_rcvd:
Total number of IGMPv1 Host Membership Reports received
by device on a link operating in IGMPv1 mode.

(4) igmpv1mode_v1_reports_rcvd_canceled_timer:
Total number of IGMPv1 Host Membership Reports received
(by device on a link operating in IGMPv1 mode) for a
group for which a timer was running. 

(5) igmpv2mode_v1_queries_rcvd: 
Number of IGMPv1 Host Membership Queries received by device
on a link operating in IGMPv2 mode.

(6) igmpv2mode_v2_general_queries_rcvd:
Number of IGMPv2 General Queries received by device on a link 
operating in IGMPv2 mode.

(7) igmpv2mode_v2_grp_specific_queries_rcvd:
Number of valid IGMPv2 Group-Specific Queries received by 
device on a link operating in IGMPv2 mode.

(8) igmpv2mode_v12_reports_rcvd_canceled_timer:
Number of IGMPv1/v2 Reports received that resulted in the
cancelation of a local timer.

(9) igmpv2mode_v12_reports_rcvd_no_timer:
Number of IGMPv1/v2 Reports received that did not have a
local timer running.

(10) igmpv2mode_v2_leave_msgs_rcvd:
Number of IGMPv2 Leave Group messages received by device on a
link operating in IGMPv2 mode.

(11) igmpv3mode_v1_queries_rcvd;
Number of IGMPv1 Queries received by device on a link operating
in IGMPv3 mode.

(12) igmpv3mode_v2_general_queries_rcvd;
Number of IGMPv2 General Queries received by device on a link
operating in IGMPv3 mode.

(13) igmpv3mode_v3_general_queries_rcvd;
Number of IGMPv3 General Queries received by device on a link
operating in IGMPv3 mode.

(14) igmpv3mode_v3_grp_specific_queries_rcvd;
Number of IGMPv3 Group-Specific Queries received by device on
a link operating in IGMPv3 mode.

(15) igmpv3mode_v3_grp_src_specific_queries_rcvd;
Number of IGMPv3 Group-Source-Specific Queries received by
device on a link operating in IGMPv3 mode.

(16) igmp_badlen_rcvd:
Total number of IGMP (v1 or v2) messages that do not 
have a length of 8 bytes.

(17) igmp_badsum_rcvd:
Total number of IGMP (v1 or v2) messages that do not 
have the correct checksum.

(18) igmp_pkt_alloc_fail:
Count of failures in allocating packet buffer for outgoing 
IGMPv1/v2 packet.

(19) igmp_bad_oper_mode:
Number of instances when a received packet could not be
forwarded to either of the IGMP protocol modules (v1 or
v2) due to an incorrect configuration of the IGMP operating
mode for the ingress link.

(20) igmp_bad_queries_rcvd:
Number of IGMPv1 Host Membership Query or a IGMPv2 General 
Query messages received that are not addressed to 224.0.0.1.

(21) igmp_bad_reports_rcvd:
Number of IGMPv1/v2 reports where the IP destination address
field is not identical to the IGMP group address or the
IGMP group address is not a multicast address

(22) igmpv1mode_unknown_pkttype:
Number of packets of unknown type (e.g, not IGMPv1 Host
Membership Query or IGMPv1 Host Membership Report) received 
by device on a link opertating in v1 mode.

(23) igmp_v2_bad_grp_specific_queries_rcvd;
This is counter is applicable if IGMPv3 is enabled.
Number of invalid IGMPv2 Group-Specific Queries (e.g, those
having a multicast group id of 224.0.0.1 or an invalid multicast 
address) received.

(24) igmpv2mode_v2_unknown_grp_specific_queries_rcvd:
Number of IGMPv2 Group-Specific Queries received with a group
address that is not registered for the ingress interface.

(25) igmpv2mode_v12_unknown_grp_reports_rcvd:
Number of IGMPv1/v2 Membership Reports received with a group
address that is not registered for the ingress interface.

(26) igmp_v2_rtr_alert_missing
This is counter is applicable if IGMPv3 is enabled.
Number of IGMPv2 General Query, IGMPv2 Group-Specific Query,
IGMPv2 Membership Report, or IGMPv2 Leave Group messages
received without the IP Router Alert option.

(27) igmpv2mode_unknown_pkttype:
Number of packets that are of a type other than a IGMPv1/v2 
Query, IGMPv1/v2 Report, or IGMPv2 Leave Group received by 
device on a link operating in IGMPv2 mode. If IGMPv3 is not enabled,
IGMPv3 Report is also considered as unknown packet.

(28) igmp_v3_bad_grp_specific_queries_rcvd;
This is counter is applicable if IGMPv3 is enabled.
Number of invalid IGMPv3 Group-Specific Queries (e.g, those
having a multicast group id of 224.0.0.1 or an invalid multicast 
address) received.

(29) igmp_v3_bad_grp_src_specific_queries_rcvd;
This is counter is applicable if IGMPv3 is enabled.
Number of invalid IGMPv3 Group-Source-Specific Queries (e.g, those
having a multicast group id of 224.0.0.1 or an invalid multicast 
address) received.

(30) igmp_v3_rtr_alert_missing
This is counter is applicable if IGMPv3 is enabled.
Number of IGMPv3 General Query, IGMPv3 Group-Specific Query,
IGMPv3 Group-Source-Specific Query received without the IP
Router Alert option.

(31) igmpv3mode_unknown_pkttype;
Number of packets that are of a type other than a IGMPv1/v2/v3 
Query, IGMPv1/v2/v3 Report, or IGMPv2 Leave Group received by 
device on a link operating in IGMPv3 mode.

(32) igmp_v1_reports_sent:
Total number of IGMPv1 Host Membership Reports sent by 
device on a link operating in IGMPv1 or IGMPv2 mode.

(33) igmpv2mode_v2_leave_msgs_sent:
Number of IGMPv2 Leave Group messages sent by device on a 
link operating in IGMPv2 mode.

(34) igmpv2mode_v2_reports_sent:
Number of IGMPv2 Membership Report messages sent by device
on a link operating in IGMPv2 mode.

(35) igmp_oper_mode_stepped_up_from_v1_to_v2;
Number of times host compatibility mode of an IGMPv3
enabled link stepped up from IGMPv1 to IGMPv2. This happens
when an IGMPv1 Querier Present timer expires and IGMPv2
Querier Present timer is running.

(36) igmp_oper_mode_stepped_down_from_v2_to_v1;
Number of times host compatibility mode of an IGMPv3
enabled link stepped down from IGMPv2 to IGMPv1. This happens
when an IGMPv2 Querier Present timer is running and an IGMPv1
Query message is received.

(37) igmpv3mode_v3_reports_sent;
Number of IGMPv3 State-Change Report, retransmitted IGMPv3
State-Change Report and Current-State Report messages sent by
device on a link operating in IGMPv2 mode.

(38) igmpv3mode_v3_truncated_reports_sent;
Number of IGMPv3 State-Change Report, retransmitted IGMPv3
State-Change Report and Current-State Report messages truncated
to MTU size and sent by device on a link operating in IGMPv2 mode.
Group record type of such messages can be either CHANGE_TO_EXCLUDE
or MODE_IS_EXCLUDE.

(39) igmp_oper_mode_stepped_up_from_v1_to_v3;
Number of times host compatibility mode of an IGMPv3
enabled link stepped up from IGMPv1 to IGMPv3. This happens
when an IGMPv1 Querier Present timer expires and IGMPv2
Querier Present timer is not running.

(40) igmp_oper_mode_stepped_up_from_v2_to_v3;
Number of times host compatibility mode of an IGMPv3
enabled link stepped up from IGMPv2 to IGMPv3. This happens
when an IGMPv2 Querier Present timer expires and IGMPv1
Querier Present timer is not running.

(41) igmp_oper_mode_stepped_down_from_v3_to_v2;
Number of times host compatibility mode of an IGMPv3 enabled
link stepped down from IGMPv3 to IGMPv2. This happens when no
IGMPv1/ v2 Querier Present timer is running and an IGMPv2
General-Query message is received.

(42) igmp_oper_mode_stepped_down_from_v3_to_v1;
Number of times host compatibility mode of an IGMPv3 enabled
link stepped down from IGMPv3 to IGMPv1. This happens when no
IGMPv1/ v2 Querier Present timer is running and an IGMPv1 Query
message is received.

(43) igmpv3_filter_added_count;
Number of times setsockopt call successfully adds a source filter.
Options that increment this count are: IP_ADD_SOURCE_MEMBERSHIP,
IP_BLOCK_SOURCE, MCAST_JOIN_SOURCE_GROUP, MCAST_BLOCK_SOURCE

(44) igmpv3_filter_deleted_count;
Number of times setsockopt call successfully deletes a source filter.
Options that increment this count are: IP_DROP_SOURCE_MEMBERSHIP,
IP_UNBLOCK_SOURCE, MCAST_LEAVE_SOURCE_GROUP, MCAST_UNBLOCK_SOURCE

(45) igmpv3_filter_failed_count;
Number of times setsockopt call to add or delete a filter fails.
Options that increment this count are: IP_ADD_SOURCE_MEMBERSHIP,
IP_DROP_SOURCE_MEMBERSHIP, IP_BLOCK_SOURCE, IP_UNBLOCK_SOURCE,
MCAST_JOIN_SOURCE_GROUP, MCAST_LEAVE_SOURCE_GROUP, MCAST_BLOCK_SOURCE,
MCAST_UNBLOCK_SOURCE
*/

struct igmp_stats {
/* rx-related statistics */
u_long igmp_total_rcvd;

u_long igmpv1mode_v1_queries_rcvd;
u_long igmpv1mode_v1_reports_rcvd;
u_long igmpv1mode_v1_reports_rcvd_canceled_timer;

#ifdef IGMP_V2
u_long igmpv2mode_v1_queries_rcvd;
u_long igmpv2mode_v2_general_queries_rcvd;
u_long igmpv2mode_v2_grp_specific_queries_rcvd;
u_long igmpv2mode_v12_reports_rcvd_canceled_timer;
u_long igmpv2mode_v12_reports_rcvd_no_timer;
u_long igmpv2mode_v2_leave_msgs_rcvd;
#endif /* IGMP_V2 */

#ifdef USE_IGMPV3
u_long igmpv3mode_v1_queries_rcvd;
u_long igmpv3mode_v2_general_queries_rcvd;
u_long igmpv3mode_v3_general_queries_rcvd;
u_long igmpv3mode_v3_grp_specific_queries_rcvd;
u_long igmpv3mode_v3_grp_src_specific_queries_rcvd;
#endif /* USE_IGMPV3 */

/* error-related statistics */
u_long igmp_badlen_rcvd;
u_long igmp_badsum_rcvd;
u_long igmp_pkt_alloc_fail;
u_long igmp_bad_oper_mode;
u_long igmp_bad_queries_rcvd;
u_long igmp_bad_reports_rcvd;
u_long igmpv1mode_unknown_pkttype;

#ifdef IGMP_V2
u_long igmp_v2_bad_grp_specific_queries_rcvd;
u_long igmpv2mode_v2_unknown_grp_specific_queries_rcvd;
u_long igmpv2mode_v12_unknown_grp_reports_rcvd;
u_long igmp_v2_rtr_alert_missing;
u_long igmpv2mode_unknown_pkttype;
#endif /* IGMP_V2 */

#ifdef USE_IGMPV3
u_long igmp_v3_bad_grp_specific_queries_rcvd;
u_long igmp_v3_bad_grp_src_specific_queries_rcvd;
u_long igmp_v3_rtr_alert_missing;
u_long igmpv3mode_unknown_pkttype;
#endif /* USE_IGMPV3 */

/* tx-related statistics */
u_long igmp_v1_reports_sent;

#ifdef IGMP_V2
u_long igmpv2mode_v2_leave_msgs_sent;
u_long igmpv2mode_v2_reports_sent;
u_long igmp_oper_mode_stepped_up_from_v1_to_v2;
u_long igmp_oper_mode_stepped_down_from_v2_to_v1;
#endif /* IGMP_V2 */

#ifdef USE_IGMPV3
u_long igmpv3mode_v3_reports_sent;
u_long igmpv3mode_v3_truncated_reports_sent;
u_long igmp_oper_mode_stepped_up_from_v1_to_v3;
u_long igmp_oper_mode_stepped_up_from_v2_to_v3;
u_long igmp_oper_mode_stepped_down_from_v3_to_v2;
u_long igmp_oper_mode_stepped_down_from_v3_to_v1;
u_long igmpv3_filter_added_count;
u_long igmpv3_filter_deleted_count;
u_long igmpv3_filter_failed_count;
#endif /* USE_IGMPV3 */
};

/* data structure that contains various IGMP-related parameters from the network link
 * data structure (struct net).  This data structure is currently used only in the 
 * CLI display routine.
 */
struct igmp_params
{
#if defined (IGMP_V1) || defined (IGMP_V2) || defined (USE_IGMPV3)
   /* administrative and operating modes for IGMP on this link - IGMPv1/v2/v3 */
   u_char igmp_admin_mode;
   u_char igmp_oper_mode;
#endif /* IGMPv1 or IGMPv2 or IGMPv3 */
#if defined (IGMP_V2) || defined (USE_IGMPV3)
   u_long igmp_timer1;   /* IGMPv1 Querier Present timer */
#endif /* IGMPv2 or IGMPv3 */
#ifdef USE_IGMPV3
   u_long igmpv3_timer2; /* IGMPv2 Querier Present timer */
   u_long igmpv3_timer3; /* IGMPv3 General Query (interface) timer */
   u_long igmpv3_uri;    /* IGMPv3 Unsolicited Report Interval */
   u_long igmpv3_qrv;    /* Querier Robustness Variable */
   u_long igmpv3_qqi;    /* Querier Interval Variable */
   u_long igmpv3_qri;    /* Querier Response Interval */
#endif/* IGMPv3 */
};

/* function that updates IGMP operating mode transition statistics */
void igmp_upd_oper_mode_transition_stats (NET netp, u_char newmode);
/* function that deletes all IGMPv1/v2 query-initiated timers */
void igmp_del_all_existing_igmp_timers (NET netp);

/* CLI */
int igmp_print_stats (void * pio);
int igmp_cfg_mode (void * pio);

#endif /* IGMP_CMN_H */

#endif /* IP_MULTICAST */
