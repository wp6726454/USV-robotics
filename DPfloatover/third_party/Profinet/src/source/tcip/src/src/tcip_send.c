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
/*  F i l e               &F: tcip_send.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  NRT send functionality                                                   */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  4
#define TCIP_MODULE_ID     4

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/

#include "ether.h"

#if ETHHDR_BIAS != 0
#error "ETHHDR_BIAS must be 0, EDD_SRV_NRT_SEND needs 4 byte alignment" 
#endif

/*===========================================================================*/

static LSA_BOOL tcip_put_pkt(TCIP_PACKET_HOLD_PTR_TYPE pkt_list, PACKET pkt)
{
	TCIP_LOG_FCT("tcip_put_pkt")

	LSA_BOOL ret_val;

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"inserted pkt 0x%x",pkt);

	LOCK_NET_RESOURCE (FREEQ_RESID);

	pkt->next = LSA_NULL;

	if (pkt_list->cur_len == pkt_list->max_len)
	{
		ret_val = LSA_FALSE;
	}
	else
	{
		if( is_null(pkt_list->first_out) )
		{
			pkt_list->first_out = pkt;
		}
		else
		{
			((PACKET)(pkt_list->last_pkt))->next = pkt;
		}

		pkt_list->last_pkt = pkt;

		pkt_list->cur_len++;

		ret_val = LSA_TRUE;
	}

	UNLOCK_NET_RESOURCE (FREEQ_RESID);

	return ret_val;
}

static PACKET tcip_get_pkt(TCIP_PACKET_HOLD_PTR_TYPE pkt_list)
{
	TCIP_LOG_FCT("tcip_get_pkt")

	PACKET pkt;

	LOCK_NET_RESOURCE (FREEQ_RESID);

	pkt = pkt_list->first_out;

	if (is_not_null(pkt) )
	{
		pkt_list->first_out = pkt->next;
		pkt->next = LSA_NULL; /* AP00936006 */

		pkt_list->cur_len--;
	}

	if( is_null(pkt_list->first_out) )
	{
		pkt_list->last_pkt = LSA_NULL;

		pkt_list->cur_len = 0; /* sanity */
	}

	UNLOCK_NET_RESOURCE (FREEQ_RESID);

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"returned pkt 0x%x",pkt);

	return pkt;
}


/*---------------------------------------------------------------------------*/
/*  the RQBs are allocated and single linked, last->next == LSA_NULL         */
/*  the list is handled as a LIFO list,                                      */
/*  the latest used RQB will be the next to be used                          */
/*  this allows simple list routines, for the lists are associated to a      */
/*  edd channel, the start anchor is a parameter                             */
/*---------------------------------------------------------------------------*/
/* queue routine to retrieve a RQB from the preallocated list                */
/* INPUT: SendRqbs head of the queue                                         */
/*---------------------------------------------------------------------------*/
static EDD_RQB_TYPE * tcip_getEddSendRQB (EDD_UPPER_RQB_PTR_TYPE * SendRqbs)
{
	TCIP_LOG_FCT("tcip_getEddSendRQB")
	EDD_RQB_TYPE * pEddSendRQB;

	LOCK_NET_RESOURCE(FREEQ_RESID);

	pEddSendRQB = *SendRqbs;

	if( is_not_null(pEddSendRQB) )
	{
		*SendRqbs = EDD_RQB_GET_NEXT_RQB_PTR(pEddSendRQB);
	}

	UNLOCK_NET_RESOURCE(FREEQ_RESID);

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"returned RQB 0x%x",pEddSendRQB);

	return(pEddSendRQB);
}

/*---------------------------------------------------------------------------*/
/* queue routine to return a RQB back to the preallocated list               */
/* INPUT: SendRqbs head of the queue                                         */
/*        actSendRqb Rqb that is returned, becomes first element of queue    */
/*---------------------------------------------------------------------------*/
static LSA_VOID tcip_putEddSendRQB (EDD_UPPER_RQB_PTR_TYPE *SendRqbs, EDD_RQB_TYPE * actSendRqb)
{
	TCIP_LOG_FCT("tcip_putEddSendRQB")

	LOCK_NET_RESOURCE(FREEQ_RESID);

	if( is_null(*SendRqbs) )
	{
		EDD_RQB_SET_NEXT_RQB_PTR(actSendRqb,LSA_NULL);
	}
	else
	{
		EDD_RQB_SET_NEXT_RQB_PTR(actSendRqb,*SendRqbs);
	}

	*SendRqbs = actSendRqb;

	UNLOCK_NET_RESOURCE(FREEQ_RESID);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"inserted RQB 0x%x next = 0x%x",actSendRqb,TCIP_EDD_GET_NEXT_RQB_PTR(actSendRqb));
}

/*
 *  free send buf and RQB
 */
static LSA_VOID tcip_FreeSndBuf( TCIP_CHANNEL_PTR_TYPE channel, TCIP_EDD_LOWER_RQB_TYPE  *pRQB)
{
	TCIP_LOG_FCT("tcip_FreeSndBuf")
	
	LSA_VOID_PTR_TYPE               pParam = LSA_NULL;
	LSA_UINT16                      ret16_val;

	pParam = TCIP_EDD_GET_PPARAM( pRQB, LSA_VOID_PTR_TYPE );

	if ( is_not_null(pParam) )
	{
		EDD_UPPER_MEM_PTR_TYPE pFrame;

		pFrame = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pParam)->pBuffer;

		if ( is_not_null(pFrame) )
		{   
			TCIP_EDD_FREE_BUFFER_MEM(&ret16_val, pFrame, channel->sysptr);
			TCIP_ASSERT(ret16_val == LSA_RET_OK);
		}
	}

	tcip_FreeRQB(channel->sysptr, pRQB );
}

/*---------------------------------------------------------------------------*/
/* initialisize a queue of EDD Send RQBs                                     */
/* INPUT: SendRqbs head of the queue                                         */
/*        Count  Nr of RQBs to add to queue                                  */
/*        EDD_handle handle of EDD to be used to process the send request    */
/*        pSys sys pointer                                                   */
/*---------------------------------------------------------------------------*/

LSA_VOID tcip_initEddSendRQBList(EDD_UPPER_RQB_PTR_TYPE *SendRqbs,
								LSA_UINT32       Count,
								LSA_HANDLE_TYPE  EDD_handle,
								LSA_SYS_PTR_TYPE pSys)
{
	TCIP_LOG_FCT("tcip_initEddSendRQBList")
	LSA_UINT32 i;

	*SendRqbs = LSA_NULL;    /* there is no one up to now! */

	for (i = 0; i <Count; i++)
	{
		
		EDD_UPPER_RQB_PTR_TYPE pRQB = tcip_AllocRQB( pSys, EDD_SRV_NRT_SEND, 0, LSA_NULL);  /* alloc no frame */

		if( is_null(pRQB) ) 
		{
			TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"allocation of lower RQB failed check send-RQB configuration");
			TCIP_FATAL();
		}
		else
		{
			EDD_UPPER_NRT_SEND_PTR_TYPE pParam;
			pParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE);

#if TCIP_CFG_COPY_ON_SEND
			{
			void * pFrame;
			
			TCIP_EDD_ALLOC_BUFFER_MEM(&pFrame, BIGBUFSIZE, pSys); /* nicht EDD_FRAME_BUFFER_LENGTH, see AP00168784 */
			TCIP_ASSERT( pFrame != LSA_NULL );

			pParam->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)pFrame;
			}
#else
			pParam->pBuffer = LSA_NULL;
#endif
		
			TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_SEND);
			TCIP_EDD_SET_HANDLE( pRQB, EDD_handle);

			tcip_putEddSendRQB(SendRqbs,pRQB);
		}
	}
}


/*===========================================================================*/

/*
 * tcip_cancel_send_resources()                                                 
 * returns all queued PAKETS to the free pkt-list								
 */
LSA_VOID tcip_cancel_send_resources(TCIP_CHANNEL_PTR_TYPE channel)
{
	EDD_RQB_TYPE *pRQB;
	PACKET  pkt;

	while((pkt = tcip_get_pkt(&(channel->edd.send_pkts))) != LSA_NULL)
	{
		LOCK_NET_RESOURCE(FREEQ_RESID);
		pk_free(pkt);
		UNLOCK_NET_RESOURCE(FREEQ_RESID);
	}

	while((pRQB = tcip_getEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs))) != LSA_NULL)
	{
		tcip_FreeSndBuf(channel, pRQB);
	}
}

/*===========================================================================*/

static LSA_VOID tcip_edd_nrt_send (TCIP_CHANNEL_PTR_TYPE  channel, PACKET  pkt, EDD_UPPER_RQB_PTR_TYPE  pRQB)
{
	TCIP_LOG_FCT ("tcip_edd_nrt_send")
	LSA_UINT8  *pSendBuf;
	int        sendLen;
	LSA_UINT32 txFrmGroup;
	LSA_UINT16 ethType;

	pSendBuf   = (LSA_UINT8 *)(pkt->nb_prot  + ETHHDR_BIAS);
	sendLen    = pkt->nb_plen - ETHHDR_BIAS;
	
	ethType = ET_TYPE_GET(pSendBuf);

	if ( ethType == htons(IPTP) ) /* IP message */
	{
		u_char ipProt = ((struct ip*)(pkt->nb_prot+ETHHDR_SIZE))->ip_prot;

		if(ipProt == UDP_PROT)
		{
			txFrmGroup = EDD_NRT_TX_GRP_IP_UDP;
		}
		else if (ipProt == TCPTP)
		{
			txFrmGroup = EDD_NRT_TX_GRP_IP_TCP;
		}
		else /* ICMP or something else */
		{
			txFrmGroup = EDD_NRT_TX_GRP_IP_ICMP;
		}		
	}
	else /* ARP or something else */
	{
		txFrmGroup = EDD_NRT_TX_GRP_ARP;
	}

	{
	EDD_UPPER_NRT_SEND_PTR_TYPE pParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE);

#if TCIP_CFG_COPY_ON_SEND
	TCIP_ASSERT( pParam->pBuffer != LSA_NULL );
	
	if( sendLen > BIGBUFSIZE ) /* do not send */
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "sendLen exceeds BIGBUFSIZE: %d", sendLen);
		TCIP_FATAL();
	}
	else
	{
		TCIP_MEMCPY( pParam->pBuffer, pSendBuf, sendLen);
	}
#else
	pParam->pBuffer    = pSendBuf;
#endif
	pParam->Length     = sendLen;

	pParam->PortID     = EDD_PORT_ID_AUTO;
	pParam->Priority   = EDD_NRT_SEND_PRIO_0; /* for EDDI and EDDS, ignored by EDDP */
	pParam->TxFrmGroup = txFrmGroup;  /* for EDDP, ignored by EDDI and EDDS */
	}

	/***/

	TCIP_EDD_SET_USER_ID_PTR(pRQB, pkt);
	TCIP_EDD_SET_HANDLE( pRQB, channel->edd.handle);

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "RQB 0x%x", pRQB);

	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
	

	return;
}

/*===========================================================================*/

int tcip_iniche_pkt_send(PACKET pkt) /* InterNiche callback */
{
	TCIP_LOG_FCT ("tcip_iniche_pkt_send")
	TCIP_CHANNEL_PTR_TYPE   channel;

	channel = (TCIP_CHANNEL_PTR_TYPE)pkt->net->n_local;

	if( is_null(channel) )
	{
		LOCK_NET_RESOURCE(FREEQ_RESID);
		pk_free(pkt);
		UNLOCK_NET_RESOURCE(FREEQ_RESID);

		return ENP_SENDERR;
	}
	else if( channel->state != TCIP_CH_OPEN ) /* late */
	{
		LOCK_NET_RESOURCE(FREEQ_RESID);
		pk_free(pkt);
		UNLOCK_NET_RESOURCE(FREEQ_RESID);

		return ENP_SENDERR;
	}
	else
	{
		EDD_UPPER_RQB_PTR_TYPE  pRQB;

		/*
		 *  get send RQB from channel's rqb pool 
		 */
		pRQB = tcip_getEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs));

		/* 
		 * if there is currently no free rqb pass packet to pktSendQueue of destination channel
		 */
		if( is_null(pRQB) )
		{
			LSA_BOOL inserted = tcip_put_pkt(&channel->edd.send_pkts, pkt );
			
			if( !inserted ) /* queue has maximum length -> insertion failed, see 946704 */
			{
				TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "packet queue maximum reached, cannot send!");

				LOCK_NET_RESOURCE(FREEQ_RESID);
				pk_free(pkt);
				UNLOCK_NET_RESOURCE(FREEQ_RESID);

				return ENP_SENDERR;
			}
		}
		else
		{
			tcip_edd_nrt_send(channel, pkt, pRQB);
		}

		return 0; /* ok */
	}
}

/*===========================================================================*/

LSA_VOID tcip_edd_nrt_send_done( TCIP_EDD_LOWER_RQB_TYPE  *pRQB,  TCIP_CHANNEL_PTR_TYPE   channel )
{
	TCIP_LOG_FCT ("tcip_edd_nrt_send_done")
	
	PACKET pkt = TCIP_RQB_GET_USER_ID_PTR(pRQB);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "pRQB: 0x%x, handle 0x%x", pRQB, TCIP_EDD_GET_HANDLE(pRQB));

#if !TCIP_CFG_COPY_ON_SEND
	{
	EDD_UPPER_NRT_SEND_PTR_TYPE  pParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE);
	pParam->pBuffer = LSA_NULL;
	TCIP_EDD_SET_USER_ID_PTR(pRQB, LSA_NULL);
	}
#endif

	LOCK_NET_RESOURCE(FREEQ_RESID);
	pk_free(pkt);
	UNLOCK_NET_RESOURCE(FREEQ_RESID);

	/***/

	if( TCIP_CH_OPEN == channel->state )
	{

		pkt = tcip_get_pkt(&(channel->edd.send_pkts));

		if(is_null(pkt)) /* no more packets queued: return Rqb to free Rqb list! */
		{
			tcip_putEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs), pRQB);
		}
		else /* one more packet to send */
		{
			tcip_edd_nrt_send(channel, pkt, pRQB);
		}

		TCIP_TRIGGER_SELECT();
	}
	else /* return Rqb to free Rqb list; NOTE: EDD send resources are cancelled before calling back */
	{
		tcip_putEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs), pRQB);
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
