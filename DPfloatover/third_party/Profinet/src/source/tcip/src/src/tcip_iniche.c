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
/*  F i l e               &F: tcip_iniche.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  InterNiche integration                                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  6
#define TCIP_MODULE_ID     6

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/

#include "ether.h"
#include "arp.h"
#include "ip.h"
#include "nptcp.h"
#include "q.h"
#include "tcp_timr.h"
#include "ip_reasm.h"

#if TCIP_CFG_ENABLE_MULTICAST
#include "igmp_cmn.h"
#include "igmp.h"
#endif

/*===========================================================================*/

extern IREP h_ireq; /* head pointer to IP reassembly entry (IRE) queue */

unsigned long cticks; /* see nsref_complete.htm, chapter "Timers and Multitasking" */

static int tcip_inside_pktdemux;

static struct tcip_mem_tag {

	EDD_MAC_ADR_TYPE addr; /* memory for ifPhysAddress */

	char descr[TCIP_MAX_MIBII_IFSTRING_LEN + 1]; /* memory for IfDescr */

} tcip_mem[TCIP_NETS];

/*===========================================================================*/

#define NETPORT_BUFFERS_MAX  ((NUMBIGBUFS + NUMLILBUFS)*2 + 3)

typedef struct bufferStoreStruct {
	PACKET bigPackets[NUMBIGBUFS];                    /* List of big packets            */
	char*  bigBuffers[NUMBIGBUFS];                    /* List of big packet buffers     */
	PACKET lilPackets[NUMLILBUFS];                    /* List of small packets          */
	char*  lilBuffers[NUMLILBUFS];                    /* List of small packet buffers   */
	struct mbuf* netportBuffers[NETPORT_BUFFERS_MAX]; /* List of Netport buffers        */
} BUFFER_STORE_TYPE;

static BUFFER_STORE_TYPE tcip_Buffers;

/*===========================================================================*/

char* post_task_setup(void)
{
	/* do nothing */
	return NULL;
}

/*---------------------------------------------------------------------------*/

void clock_c(void)
{
	/* do nothing */
}

/*---------------------------------------------------------------------------*/

void clock_init(void)
{
	/* do nothing */
}

/*---------------------------------------------------------------------------*/
#ifndef INICHE_TIMERS
/* AP00834747 */
#error "check this: INICHE_TIMERS must be defined!"
#endif

LSA_VOID tcip_iniche_ctick( LSA_VOID )
{
	cticks++;

	inet_timer(); /* Trigger ipstack internal timers and checks */
}

/*===========================================================================*/

char *npalloc( unsigned size )
{
	void *ptr;
	TCIP_ALLOC_LOCAL_MEM(&ptr, (LSA_UINT16)size);
	if(is_not_null(ptr))
	{
		TCIP_MEMSET(ptr, 0, size); /* documentation: buffers returned from npalloc() are assumed to be pre-initialized to all zeros */
	}
	return( (char*)ptr );
}

/*---------------------------------------------------------------------------*/

void npfree( void *ptr )
{
	LSA_UINT16  retval;
	TCIP_FREE_LOCAL_MEM(&retval, ptr);
	TCIP_ASSERT(retval == LSA_RET_OK);
}

/*===========================================================================*/

#if TCIP_CFG_COPY_ON_SEND == 0

char * npalloc_ethmem(unsigned size) /* alloc & return memory that is accessible by the ethernet controller */
{
	LSA_SYS_PTR_TYPE sys_ptr = { 0 }; /* user must not use it */
	void *ptr;
	TCIP_EDD_ALLOC_BUFFER_MEM(&ptr, (LSA_UINT16)size, sys_ptr);
	return ptr;
}

void npfree_ethmem(void* ptr) /* free memory allocated with npalloc_ethmem() */
{
	LSA_SYS_PTR_TYPE sys_ptr = { 0 }; /* user must not use it */
	LSA_UINT16  retval;
	TCIP_EDD_FREE_BUFFER_MEM(&retval, ptr, sys_ptr);
	TCIP_ASSERT(retval == LSA_RET_OK);
}

#endif

/*===========================================================================*/

static LSA_VOID tcip_iniche_store_allocated_buffers(void)
{
	int lvi;
	PACKET tmp;
	struct mbuf* pNetBuf;

	/* Store the allocated Packets and buffers from big buffer queue    */
	TCIP_ASSERT(bigfreeq.q_len == NUMBIGBUFS);
	tmp = (PACKET)bigfreeq.q_head;
	for ( lvi = 0; lvi < NUMBIGBUFS; lvi++ )
	{
		tcip_Buffers.bigPackets[lvi] = tmp;
		if ( tmp != LSA_NULL )
		{
			tcip_Buffers.bigBuffers[lvi] = tmp->nb_buff;
			tmp = tmp->next;
		}
	}

	/* Store the allocated Packets and buffers from small buffer queue  */
	TCIP_ASSERT(lilfreeq.q_len == NUMLILBUFS);
	tmp = (PACKET)lilfreeq.q_head;
	for ( lvi = 0; lvi < NUMLILBUFS; lvi++ )
	{
		tcip_Buffers.lilPackets[lvi] = tmp;
		if ( tmp != LSA_NULL )
		{
			tcip_Buffers.lilBuffers[lvi] = tmp->nb_buff;
			tmp = tmp->next;
		}
	}

	/* Store the allocated netport packet buffer (used for UDP)    */
	TCIP_ASSERT(mfreeq.q_len == NETPORT_BUFFERS_MAX);
	pNetBuf = (struct mbuf*)mfreeq.q_head;
	for ( lvi = 0; lvi < NETPORT_BUFFERS_MAX; lvi++ )
	{
		tcip_Buffers.netportBuffers[lvi] = pNetBuf;
		pNetBuf = pNetBuf->next;
	}
}

/*---------------------------------------------------------------------------*/

static LSA_VOID tcip_iniche_free_allocated_buffers(void)
{
	int lvi;

	/* Free the stored allocated Packets and big buffers    */
	for ( lvi = 0; lvi < NUMBIGBUFS; lvi++ )
	{
		if ( tcip_Buffers.bigBuffers[lvi] != LSA_NULL)
		{
			void *ptr = tcip_Buffers.bigBuffers[lvi];
#ifdef NPDEBUG
			ptr = (char*)ptr - ALIGN_TYPE; /* what a mess, see pk_init() */
			TCIP_ASSERT(*(long*)ptr == 'MMMM');
#endif
			BB_FREE(ptr);
			tcip_Buffers.bigBuffers[lvi] = LSA_NULL;
		}
		if ( tcip_Buffers.bigPackets[lvi] != LSA_NULL )
		{
			NB_FREE(tcip_Buffers.bigPackets[lvi]);
			tcip_Buffers.bigPackets[lvi] = LSA_NULL;
		}
	}

	/* Free the stored allocated Packets and small buffers  */
	for ( lvi = 0; lvi < NUMLILBUFS; lvi++ )
	{
		if ( tcip_Buffers.lilBuffers[lvi] != LSA_NULL)
		{
			void *ptr = tcip_Buffers.lilBuffers[lvi];
#ifdef NPDEBUG
			ptr = (char*)ptr - ALIGN_TYPE; /* what a mess, see pk_init() */
			TCIP_ASSERT(*(long*)ptr == 'MMMM');
#endif
			LB_FREE(ptr);
			tcip_Buffers.lilBuffers[lvi] = LSA_NULL;
		}

		if ( tcip_Buffers.lilPackets[lvi] != LSA_NULL )
		{
			NB_FREE(tcip_Buffers.lilPackets[lvi]);
			tcip_Buffers.lilPackets[lvi] = LSA_NULL;
		}
	}

	/* Free the stored allocated netport buffers    */
	for ( lvi = 0; lvi < NETPORT_BUFFERS_MAX; lvi++ )
	{
		if ( tcip_Buffers.netportBuffers[lvi] != LSA_NULL )
		{
			NB_FREE(tcip_Buffers.netportBuffers[lvi]);
			tcip_Buffers.netportBuffers[lvi] = LSA_NULL;
		}
	}
}

/*===========================================================================*/

#if TCIP_CFG_ENABLE_MULTICAST
static int tcip_mcastlist(struct in_multi * multi_ptr)
{
	/* note: do nothng here, see tcip_srv_multicast() */
	LSA_UNUSED_ARG(multi_ptr);
	return 0;
}
#endif

/*===========================================================================*/

static int tcip_iniche_create_device(NET ifp, void * bindinfo)
{
	struct tcip_mem_tag *mem = bindinfo; /* see tcip_iniche_setup() */

	ifp->n_local  = 0; /* usage see tcip_iniche_activate_interfaces() */
	ifp->n_local2 = 0; /* unused */
	ifp->n_snbits = 0; /* nsref_complete.htm: n_snbits is currently unused */

	ifp->n_flags |= NF_BCAST;        /* Set bcast flag bit for ethernet    */
	ifp->n_flags |= NF_MCAST;        /* Set mcast flag bit for ethernet    */
	ifp->n_flags |= NF_NBPROT;       /* Set nbprot flag bit for ethernet   */

#if EDD_MAC_ADDR_SIZE != 6
#error "check this!"
#endif

	TCIP_ASSERT(ETHHDR_SIZE == (EDD_MAC_ADDR_SIZE + EDD_MAC_ADDR_SIZE + ETHHDR_BIAS/*see 1220027*/ + 2)); /* ETHHDR_SIZE is sizeof() */

#if ET_MAXLEN != (EDD_IEEE_FRAME_LENGTH - EDD_VLAN_TAG_SIZE)
#error "check this!"
#endif

	ifp->n_hal = EDD_MAC_ADDR_SIZE;  /* hardware address length            */
	ifp->n_lnh = ETHHDR_SIZE;        /* space reserved for ethernet header */
	ifp->n_mtu = ET_MAXLEN;          /* max frame size                     */

	ifp->n_ipaddr   = 0; /* 0.0.0.0 (no ip address) */
	ifp->snmask     = 0; /* 0.0.0.0 (invalid) */
	ifp->n_defgw    = 0; /* 0.0.0.0 (no gateway) */

	ifp->n_netbr    = 0; /* sanity */
	ifp->n_netbr42  = 0; /* sanity */
	ifp->n_subnetbr = 0; /* sanity */

#if TCIP_CFG_ENABLE_MULTICAST
	ifp->n_mcastlist     = tcip_mcastlist;

	ifp->igmp_admin_mode = IGMP_MODE_V2;
	ifp->igmp_oper_mode  = ifp->igmp_admin_mode;

	ifp->igmp_timer1     = 0; /* compare to igmp_init() */
#endif

	/***/

	ifp->n_init     = NULL;
	ifp->n_close    = NULL;
	ifp->n_stats    = NULL;
	ifp->n_reg_type = NULL;
	ifp->n_setstate = NULL;
	ifp->pkt_send   = tcip_iniche_pkt_send;
	ifp->raw_send   = NULL; /* using ifp->pkt_send */

	/***/

	{
	IFMIB mib = &ifp->mib;

	TCIP_ASSERT(mib == ifp->n_mib); /* see ni_create() */

	mib->ifIndex = if_netnumber(ifp); /* note: zero-based, see declaration (SNMP-code adds 1, see rfc1213.c) */

	mem->descr[0] = 0; /* empty string */
	mib->ifDescr  = mem->descr;

	mib->ifAdminStatus = NI_DOWN; /* see ni_create() */
	mib->ifOperStatus  = NI_DOWN; /* see tcip_edd_ext_link_status_indication() */

	mib->ifType  = ETHERNET;
	mib->ifMtu   = EDD_NRT_MAX_SND_LEN; /* not: ET_MAXLEN */
	mib->ifSpeed = 0;

	TCIP_MEMSET(&mem->addr, 0, sizeof(mem->addr));
	mib->ifPhysAddress = &mem->addr.MacAdr[0]; /* see tcip_iniche_activate_interfaces() */
	}

	return 0;
}

/*===========================================================================*/

/*
 *  to be called once for ipstack "start"
 */
LSA_VOID tcip_iniche_setup(LSA_VOID)
{
	TCIP_LOG_FCT("tcip_iniche_setup")

	TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"creating interfaces");

	tcip_inside_pktdemux = 0; /* initialize it in case of restart  */

	cticks = 0;

	agent_on = FALSE; /* Turn off interniche internal SNMP agent */

	MaxLnh = ETHHDR_SIZE;
	MaxMtu = ET_MAXLEN;

	netmain_init(); /* main init of iniche stack; allocates the packet queues  */

	tcip_iniche_store_allocated_buffers();
}

/*---------------------------------------------------------------------------*/

LSA_VOID tcip_iniche_create_interfaces( LSA_UINT32 nets_count )
{
	TCIP_LOG_FCT("tcip_iniche_create_interfaces")

	/*
	 *  create TCIP_NETS interfaces with admin-status "down"
	 */
	if( nets_count > TCIP_NETS )
	{
		TCIP_FATAL();
	}
	else
	{
		LSA_UINT32 cnt;

		for ( cnt = 0; cnt < nets_count; cnt++ )
		{
			struct tcip_mem_tag *mem = &tcip_mem[cnt];
			char name[IF_NAMELEN] = "???"; /* only for IN_MENUS */
			NET ifp;

			if ( ni_create(& ifp, tcip_iniche_create_device, name, mem) != 0)
			{
				TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "could not ni_create() for net %d", cnt);
				TCIP_FATAL();
			}

			ni_set_state(ifp, NI_DOWN); /* ni_create() sets NI_UP */
			ni_set_ipsuite(ifp, 0,0,0);

#if TCIP_CFG_ENABLE_MULTICAST
			in_addmulti(&igmp_all_hosts_group, ifp, 4);
#endif
		}
	}
}

/*---------------------------------------------------------------------------*/

LSA_VOID tcip_iniche_delete_interfaces( LSA_VOID )
{
	TCIP_LOG_FCT("tcip_iniche_delete_interfaces")

	/*
	 *  delete the TCIP interfaces in reverse order
	 */

	for(;;)
	{
		NET last = 0;

		{
		NET ifp;
		for (ifp = (NET)(netlist.q_head); ifp; ifp = ifp->n_next) /* see loop in if_getbynum() */
		{
			if( ifp->pkt_send == tcip_iniche_pkt_send ) /* is a TCIP interface */
			{
				if( ifp->n_local != 0 ) /* must be free */
				{
					TCIP_FATAL();
				}

				last = ifp; /* save and keep looping */
			}
		}
		}

		if( last == 0 )
		{
			break; /* all done */
		}
		else
		{
#if TCIP_CFG_ENABLE_MULTICAST
			struct in_multi * inm = lookup_mcast(igmp_all_hosts_group, last);
			if (inm != NULL)
			{
				in_delmulti(inm);
			}
#endif
			if (ni_delete(last) != 0)
			{
				TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"TCP/IP-stack-Error at removing interface ifIndex %d", last->mib.ifIndex);
				TCIP_FATAL();
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

/*
 *  to be called once for ipstack "shutdown"
 */
LSA_VOID tcip_iniche_teardown(LSA_VOID)
{
	TCIP_LOG_FCT("tcip_iniche_teardown")

	/*
	 * Free the stored allocated buffers and packages
	 */
	tcip_iniche_free_allocated_buffers();

	/* Cleanup routing table buffer */
#ifdef IP_ROUTING
	if ( rt_mib != LSA_NULL )
	{
		RT_FREE( rt_mib );
	}
#endif /* IP_ROUTING */

	/*
	 *  Initialize the IPSTACK queues at shutdown (for possible restart)
	 */
	TCIP_MEMSET( &bigfreeq, 0, sizeof(bigfreeq));
	TCIP_MEMSET( &lilfreeq, 0, sizeof(lilfreeq));
	TCIP_MEMSET( &mfreeq,   0, sizeof(mfreeq));
	TCIP_MEMSET( &netlist,  0, sizeof(netlist));
}

/*===========================================================================*/

/*
 *  activate interfaces of this channel-group
 */
LSA_VOID tcip_iniche_activate_interfaces( TCIP_CHANNEL_PTR_TYPE arp_channel )
{
	TCIP_IF_DATA_PTR_TYPE p_if_data = arp_channel->p_if_data;
	LSA_UINT16 port_id;

	/*
	 *  find free net struct for interface and ports
	 */
	for( port_id = 0/*!*/; port_id <= p_if_data->port_count; port_id ++ )
	{
		NET ifp;
		for (ifp = (NET)(netlist.q_head); ifp; ifp = ifp->n_next) /* see loop in if_getbynum() */
		{
			if( ifp->pkt_send == tcip_iniche_pkt_send ) /* is a TCIP interface */
			{
				if( ifp->n_local == 0 ) /* free */
				{
					ifp->n_local = arp_channel; /* used */

					break; /* found */
				}
			}
		}

		if( is_null(ifp) )
		{
			TCIP_FATAL(); /* a bug, free interfaces must be found */
		}
		else
		{
			p_if_data->port[port_id].ifp = ifp;

			/* note: by looking at the interniche code, ifPhysAddress must be "stable" */
			TCIP_MEMCPY(ifp->mib.ifPhysAddress, &p_if_data->port[port_id].MACAddr.MacAdr[0], sizeof(tcip_mem[0].addr));

			ifp->mib.ifSpeed = p_if_data->port[port_id].nominal_bandwidth; /* "nominal" until updated via edd-link.ind */

			ni_set_state(ifp, NI_UP); /* ifAdminStatus is always up */

			if( port_id != 0 ) /* -> only ports, interface (port_id == 0) see below */
			{
				ifp->mib.ifOperStatus = NI_DOWN; /* ifOperStatus of ports is down until link indication, see tcip_edd_ext_link_status_indication() */

				if (p_if_data->port[port_id].PhyStatus == EDD_PHY_STATUS_ENABLED) /* see 1201670 */
				{
					ifp->mib.ifAdminStatus = NI_UP;
				}
				else
				{
					ifp->mib.ifAdminStatus = NI_DOWN;
				}
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

/*
 *  remove interfaces of this channel-group
 */
LSA_VOID tcip_iniche_deactivate_interfaces( TCIP_CHANNEL_PTR_TYPE arp_channel )
{
	TCIP_LOG_FCT("tcip_iniche_deinit_interfaces")
	TCIP_IF_DATA_PTR_TYPE p_if_data = arp_channel->p_if_data;
	LSA_UINT16 port_id;

	/*
	 *  reset all nets belonging to this interface
	 */
	for ( port_id = 0/*!*/; port_id <= p_if_data->port_count; port_id++ )
	{
		NET ifp = p_if_data->port[port_id].ifp;

		ifp->n_local = 0; /* free */

		ni_set_ipsuite(ifp, 0,0,0);

		ifp->mib.ifDescr[0] = 0;
		ifp->mib.ifSpeed = 0;

		ni_set_state(ifp, NI_DOWN); /* ifAdminStatus and ifOperStatus go down */

		TCIP_MEMSET(ifp->mib.ifPhysAddress, 0, sizeof(tcip_mem[0].addr));
	}

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,
		"interfaces of interface_id %u deinitialized", arp_channel->p_if_data->edd_interface_id);
}

/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
// delete content of reassembly cache
//--------------------------------------------------------------------------*/
LSA_VOID tcip_iniche_flush_reasm_cache(TCIP_CHANNEL_PTR_TYPE channel)
{
	IREP tmpp;
	IREP nxt_tmpp;

	channel->edd.get_statistics_pending ++;  

	/* Explanation: if pk_free() is called by ip_reasm_delete_ire(), tcip_iniche_recv_done() will be called.
	   Since state of channel == TCIP_CH_CLOSING, tcip_iniche_recv_done() calls tcip_edd_close_channel().
	   Incrementing get_statistics_pending here prevents tcip_edd_close_channel() from returning the close rqb.
	*/


	LOCK_NET_RESOURCE (NET_RESID);

	for (tmpp = h_ireq; tmpp; tmpp = nxt_tmpp)
	{	
		nxt_tmpp = tmpp->next; /* save the next pointer for the IRE that may be deleted */

		ip_reasm_delete_ire (tmpp);
		
		++ip_mib.ipReasmFails;
	}

	UNLOCK_NET_RESOURCE(NET_RESID);

	/* To enable tcip_edd_close_channel(), decrement again. See explanation above. */

	channel->edd.get_statistics_pending --; 
}

/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* preventing stack-overflow with indirect calling of pktdemux() */

void SignalPktDemux(void)
{
	if(tcip_inside_pktdemux != 0)  /* check re-entrancy flag                       */
		return;               /* do not reenter pktdemux(), packet will wait. */
	tcip_inside_pktdemux++;        /* set re-entrany flag                          */
	pktdemux();               /* process low level packet input               */
	tcip_inside_pktdemux--;        /* clear re-entracy flag                        */

	TCIP_TRIGGER_SELECT();
}

/*===========================================================================*/

LSA_UINT16 TCIP_UPPER_IN_FCT_ATTR
tcip_set_timeparams(
	LSA_UINT32 keepalivetime,
	LSA_UINT32 connectiontimeout,
	LSA_UINT32 retransmittimeout,
	LSA_UINT32 retransmitcount,
	LSA_UINT32 no_change_value
) {
	/* note: range-check is done by caller */

	if( keepalivetime != no_change_value )
	{
		tcp_keepidle = (keepalivetime * PR_SLOWHZ) / 1000;
	}

	if( connectiontimeout != no_change_value )
	{
		tcip_ConnectionTimeout = (connectiontimeout * PR_SLOWHZ) / 1000;
	}

	if( retransmittimeout != no_change_value )
	{
		tcp_keepintvl = (retransmittimeout * PR_SLOWHZ) / 1000;

		tcip_RetransmitTimeout = (retransmittimeout * PR_SLOWHZ) / 1000;
//TODO hinterfragen: warum wurde nicht einfach TCPTV_REXMTMAX=65.5sec gesetzt?
	}

	if( retransmitcount != no_change_value )
	{
		tcip_RetransmitCntMax = retransmitcount;
	}

	return LSA_RET_OK;
}

/*===========================================================================*/

LSA_VOID TCIP_UPPER_IN_FCT_ATTR
tcip_get_timeparams(
	LSA_UINT32 *p_keepalivetime,
	LSA_UINT32 *p_connectiontimeout,
	LSA_UINT32 *p_retransmittimeout,
	LSA_UINT32 *p_retransmitcount
) {
	*p_keepalivetime     = tcp_keepidle           * (1000 / PR_SLOWHZ);
	*p_connectiontimeout = tcip_ConnectionTimeout * (1000 / PR_SLOWHZ);
	*p_retransmittimeout = tcp_keepintvl          * (1000 / PR_SLOWHZ);
	*p_retransmitcount   = tcip_RetransmitCntMax;
}


/* N-IF change: review */
#if 0
/*----------------------------------------------------------------------------/

  tcip_CntrlTimeParams(TCIP_UPPER_RQB_PTR_TYPE rqb)
  range checking has to be done prior to a call!

/----------------------------------------------------------------------------*/
LSA_UINT16 tcip_CntrlTimeParams(TCIP_UPPER_RQB_PTR_TYPE rqb)
{
	LSA_UINT16      retval = TCIP_RSP_OK;

	switch(rqb->args.timeparam_rq.timeopt)
	{
	case TCIP_SET_TIMEPARAMS:
	case TCIP_SET_TIMEPARAMS_CLOSE_TCP_CONN:
		{
			if(rqb->args.timeparam_rq.keepalivetime != TCIP_NO_CHANGE_TIMEPARMS)
			{ /* default time before probing  */
				tcp_keepidle = rqb->args.timeparam_rq.keepalivetime*PR_SLOWHZ/1000;
			}
			if (rqb->args.timeparam_rq.retranstimeout != TCIP_NO_CHANGE_TIMEPARMS)
			{  /* default probe interval */
				tcp_keepintvl = rqb->args.timeparam_rq.retranstimeout*PR_SLOWHZ/1000;
			}
			if(rqb->args.timeparam_rq.connectiontimeout != TCIP_NO_CHANGE_TIMEPARMS)
			{ /* Connect Timeout: maximum time to establish a connection   */
				tcip_ConnectionTimeout   = rqb->args.timeparam_rq.connectiontimeout*PR_SLOWHZ/1000;
			}
			if(rqb->args.timeparam_rq.retransmitcount != TCIP_NO_CHANGE_TIMEPARMS)
			{ /* max probes before drop   */
				tcip_RetransmitCntMax    = rqb->args.timeparam_rq.retransmitcount;
			}
			if(rqb->args.timeparam_rq.retranstimeout != TCIP_NO_CHANGE_TIMEPARMS)
			{ /* max REXMT time value   */
				tcip_RetransmitTimeout   = rqb->args.timeparam_rq.retranstimeout*PR_SLOWHZ/1000;
			}
		}
		/*  !!! no break, because TCIP_SET_TIMEPARAMS implies TCIP_GET_TIMEPARAMS !!!  */
	case TCIP_GET_TIMEPARAMS:
		{
			/* default time before probing  */
			rqb->args.timeparam_rq.keepalivetime     = tcp_keepidle * (1000/PR_SLOWHZ);
			/* default probe interval */
			rqb->args.timeparam_rq.retranstimeout    = tcp_keepintvl * (1000/PR_SLOWHZ);
			/* Connect Timeout: maximum time to establish a connection   */
			rqb->args.timeparam_rq.connectiontimeout = tcip_ConnectionTimeout * (1000/PR_SLOWHZ);
			/* max probes before drop   */
			rqb->args.timeparam_rq.retransmitcount   = tcip_RetransmitCntMax;
			/* max REXMT time value   */
			rqb->args.timeparam_rq.retranstimeout    = tcip_RetransmitTimeout * (1000/PR_SLOWHZ);
		}
	}
	return (retval);
}
#endif

/*===========================================================================*/

LSA_FATAL_ERROR_TYPE tcip_iniche_fatal_var; /* for some debuggers that have problems with the stack-trace */

void tcip_iniche_panic(char *file, int line)
{
	TCIP_LOG_FCT("tcip_iniche_panic")

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "file(0x%x) line(%u)"
		, file, line
		);

	tcip_iniche_fatal_var.lsa_component_id = LSA_COMP_ID_CM;
	tcip_iniche_fatal_var.module_id        = TCIP_MODULE_ID;
	tcip_iniche_fatal_var.line             = (LSA_UINT16)__LINE__;

	tcip_iniche_fatal_var.error_code[0]    = (LSA_UINT32)file;
	tcip_iniche_fatal_var.error_code[1]    = (LSA_UINT32)line;
	tcip_iniche_fatal_var.error_code[2]    = 0;
	tcip_iniche_fatal_var.error_code[3]    = 0;

	tcip_iniche_fatal_var.error_data_length = 0;
	tcip_iniche_fatal_var.error_data_ptr    = LSA_NULL;

	TCIP_FATAL_ERROR(sizeof(tcip_iniche_fatal_var), &tcip_iniche_fatal_var);
	/* not reached */
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
