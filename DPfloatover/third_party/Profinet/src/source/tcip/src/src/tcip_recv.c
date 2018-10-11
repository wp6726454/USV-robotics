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
/*  F i l e               &F: tcip_recv.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  NRT receive functionality                                                */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  5
#define TCIP_MODULE_ID     5

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/

#include "ether.h"

#if ETHHDR_BIAS != 0
#error "ETHHDR_BIAS must be 0, EDD_SRV_NRT_RECV needs 4 byte alignment" 
#endif

#if (ETHHDR_BIAS + EDD_IEEE_FRAME_LENGTH) > EDD_FRAME_BUFFER_LENGTH
#erorr "check this!"
#endif

#define TCIP_RECV_BUFFER_SIZE  EDD_FRAME_BUFFER_LENGTH /* see EDD_SRV_NRT_RECV */

/*===========================================================================*/

LSA_VOID tcip_AllocRcvBuf( TCIP_CHANNEL_PTR_TYPE channel)
{
	EDD_UPPER_MEM_PTR_TYPE pRxFrame; /* Pointer to Rx-Frame memory. */
	
	EDD_UPPER_RQB_PTR_TYPE pRQB = tcip_AllocRQB( channel->sysptr, EDD_SRV_NRT_RECV, TCIP_RECV_BUFFER_SIZE, &pRxFrame);

	if( is_null(pRQB) )
	{
		TCIP_FATAL();
	}
	else
	{
		EDD_UPPER_NRT_RECV_PTR_TYPE pParam = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_NRT_RECV_PTR_TYPE );
		PACKET                      pkt;

		pParam->pBuffer = pRxFrame;
		pParam->RequestID  = 0; /* not used */
		pParam->RequestCnt = 0;

		pkt = (PACKET)NB_ALLOC(sizeof(*pkt)); /* compare to pk_init() */

		if( is_null(pkt) )
		{
			TCIP_FATAL();
		}
		else
		{
			pkt->pRQB = pRQB; /* store the RQB in the packet */

			TCIP_EDD_SET_USER_ID_PTR(pRQB, pkt); /* store packet in the userID of the RQB */

			TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_RECV);

			tcip_edd_nrt_recv(pRQB, channel);
		}
	}
}

LSA_VOID tcip_FreeRcvBuf( TCIP_CHANNEL_PTR_TYPE channel, TCIP_EDD_LOWER_RQB_TYPE  *pRQB)
{
	LSA_VOID_PTR_TYPE               pParam = LSA_NULL;
	LSA_UINT16                      ret16_val;
	PACKET                          pkt;

	pkt = TCIP_RQB_GET_USER_ID_PTR(pRQB);
	
	NB_FREE(pkt);

	pParam = TCIP_EDD_GET_PPARAM( pRQB, LSA_VOID_PTR_TYPE );

	if ( is_not_null(pParam) )
	{
		EDD_UPPER_MEM_PTR_TYPE pFrame;

		pFrame = ((EDD_UPPER_NRT_RECV_PTR_TYPE)pParam)->pBuffer;

		if ( is_not_null(pFrame) )
		{   
			TCIP_EDD_FREE_BUFFER_MEM(&ret16_val, pFrame, channel->sysptr);
			TCIP_ASSERT(ret16_val == LSA_RET_OK);
		}
	}

	tcip_FreeRQB(channel->sysptr, pRQB );	
}

/*
 *  
 */
LSA_VOID tcip_edd_nrt_recv_done ( TCIP_EDD_LOWER_RQB_TYPE  *pRQB, TCIP_CHANNEL_PTR_TYPE channel )
{
	LSA_UINT16 response = TCIP_EDD_GET_RESPONSE(pRQB);

	channel->edd.pending_rcv_reqests --;

	if( TCIP_CH_OPEN != channel->state ) /* closing */
	{
		TCIP_ASSERT(response == EDD_STS_OK || response == EDD_STS_OK_CANCEL);

		tcip_FreeRcvBuf( channel, pRQB);
	}
	else if( response != EDD_STS_OK )
	{
		TCIP_FATAL(); /* not as documented */
	}
	else /* ok */
	{
		EDD_UPPER_NRT_RECV_PTR_TYPE pRecvParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_RECV_PTR_TYPE);
    
		if( pRecvParam->FrameFilter == EDD_NRT_FRAME_ARP )
		{
			/* peek at "opcode" at offset 6, 16 bits in network byte order (ARP reply = 2, see arp.h, struct arp_hdr) */

			if( pRecvParam->UserDataLength >= 8 )
			{
				LSA_UINT8 *op = &pRecvParam->pBuffer[pRecvParam->UserDataOffset + 6];

				if( op[0] == 0 && op[1] == 2 )
				{
					TCIP_ARP_RESPONSE_RECEIVED(channel->detailptr->group_id, pRecvParam->pBuffer, pRecvParam->IOCount);
				}
			}
		}

		/***/

		if( pRecvParam->FrameFilter == EDD_NRT_FRAME_IP_TCP ) /* see 947895, do not allow TCP frames with broadcast bit */
		{
			LSA_UINT8 mac1 = pRecvParam->pBuffer[ETHHDR_BIAS];

			if (mac1 & 0x01) /* check broadcast bit */
			{
				tcip_edd_nrt_recv(pRQB, channel); /* reprovide */

				return;
			}
		}

		/***/

		{
		PACKET pkt = (PACKET)TCIP_RQB_GET_USER_ID_PTR(pRQB); /* get the packet */

		if( is_null(pkt) )
		{
			TCIP_FATAL();
		}
		else
		{
			pkt->nb_buff   = (char *)pRecvParam->pBuffer;
			pkt->nb_blen   = TCIP_RECV_BUFFER_SIZE; /* must be set to the allocated buffer size AP00832626 */
			pkt->nb_prot   = pkt->nb_buff + ETHHDR_SIZE;
			pkt->nb_plen   = pRecvParam->IOCount - ETHHDR_SIZE;
			pkt->nb_tstamp = cticks;

			pkt->inuse     = 1; /* see pk_alloc() */
			pkt->flags     = 0;

			pkt->net       = channel->p_if_data->port[0/*!*/].ifp; /* not: pRecvParam->PortID, see AP00819817 */

			{ /* AP00788431: NF_NBPROT flag bit set --> set pkt->type and pkt->nb_prot */
			char *   eth;
			eth = (pkt->nb_buff + ETHHDR_BIAS);

#ifdef TCIP_CFG_VLAN /*INICHE_TCIP_ITGR enable support of VLAN tagged ethernet frames (IEEE 802.1 Q)*/
            /* VLAN Info: Tag Protocol Identifier (TPID):    2 Bytes = 0x8100 */
            /*            User_priority:                     3 Bit */
            /*            Canonical Format Indicator (CFI):  1 Bit */
            /*            VLAN Identifier (VID):            12 Bit */
            if((unshort)ET_TYPE_GET(eth) == 0x8100)
            {
               /* skip VLAN Info (4 Bytes) */
               pkt->type = htons((unshort)ET_TYPE_GET(eth + 4));
               pkt->nb_prot = pkt->nb_buff + ETHHDR_SIZE /*+ ETHHDR_BIAS see 1220027*/ + 4; /*AP00922404 */
            }
			else
#endif
			{
#ifdef IEEE_802_3
				/* see if it's got snap baggage */
				if (ET_TYPE_GET(eth) <= 0x0600)
				{
				   struct snap_hdr *snap;
				   snap = (struct snap_hdr *)(pkt->nb_buff + ETHHDR_SIZE);
				   pkt->type = (unshort)(snap->type);
				   pkt->nb_prot = pkt->nb_buff + pkt->net->n_lnh;
				}
				else
				{
				   pkt->type = htons((unshort)ET_TYPE_GET(eth));
				   pkt->nb_prot = pkt->nb_buff + ETHHDR_SIZE;
				}
#else
			pkt->type = htons((unshort)ET_TYPE_GET(eth));
            pkt->nb_prot = pkt->nb_buff + pkt->net->n_lnh;
#endif
			}
			}
			
			/***/

			LOCK_NET_RESOURCE(RXQ_RESID);
			putq(&rcvdq, (q_elt)pkt); /* give packet to InterNiche stack */
			UNLOCK_NET_RESOURCE(RXQ_RESID);

			channel->edd.in_work_rcv_rqbs_number += 1;

			SignalPktDemux();
		}
		}
	}
}


/*---------------------------------------------------------------------------*/
/* returns the given packet after stack processing                           */
/*---------------------------------------------------------------------------*/

void tcip_iniche_recv_done(void * pkt_)
{
	PACKET                          pkt = (PACKET)pkt_;
	EDD_RQB_TYPE                    *pRQB = pkt->pRQB;
	TCIP_CHANNEL_PTR_TYPE           channel;
	
	channel = tcip_channel_from_handle(TCIP_EDD_GET_HANDLE(pRQB));

	if( is_null(channel) || TCIP_EDD_GET_SERVICE(pRQB) != EDD_SRV_NRT_RECV )
	{
		TCIP_FATAL();
	}
	else
	{
		/* 
		 *  if channel not open (closing in progress) free rqb
		 */
		channel->edd.in_work_rcv_rqbs_number -= 1; /* must be 0 before chanel closes */

		if( channel->state == TCIP_CH_CLOSING )
		{
			tcip_FreeRcvBuf(channel, pRQB);
			
			tcip_edd_close_channel(channel);
		}
		else
		{
			tcip_edd_nrt_recv(pRQB, channel);
		}
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
