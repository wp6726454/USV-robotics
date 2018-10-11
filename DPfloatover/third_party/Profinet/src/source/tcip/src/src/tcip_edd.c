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
/*  F i l e               &F: tcip_edd.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EDDx integration                                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  3
#define TCIP_MODULE_ID     3

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/
/*===========================================================================*/

static LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_get_params(
	TCIP_CHANNEL_PTR_TYPE channel
);

static LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_get_portparams(
	TCIP_CHANNEL_PTR_TYPE channel
);

static LSA_VOID
tcip_edd_nrt_cancel_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
);

#if TCIP_CFG_ENABLE_MULTICAST /* OBSD_ITGR -- avoid warning: 'tcip_edd_srv_multicast_done' declared 'static' but never defined */
static LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_srv_multicast_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_TYPE  *pRQB
);
#endif

/*===========================================================================*/
/*===========================================================================*/

EDD_UPPER_RQB_PTR_TYPE tcip_AllocRQB(LSA_SYS_PTR_TYPE pSys, LSA_UINT32 service, int sizeBuffer, EDD_UPPER_MEM_PTR_TYPE* ppFrame  )                                              
{
	LSA_USER_ID_TYPE         UserId = { 0 };
	EDD_UPPER_RQB_PTR_TYPE   pRQB;

	TCIP_EDD_ALLOC_LOWER_RQB(&pRQB, UserId, sizeof(TCIP_EDD_LOWER_RQB_TYPE), pSys);

	if( is_not_null(pRQB) )
	{	  
		LSA_UINT16 length;
		LSA_VOID *pParam;
		LSA_UINT16 RetVal;

		switch( service ) {

		case 0: /* open- and close-channel, note: close doesn't have parameters */
			length = sizeof(EDD_RQB_OPEN_CHANNEL_TYPE);
			break;

		case EDD_SRV_GET_STATISTICS_ALL:
			length = sizeof(EDD_RQB_GET_STATISTICS_ALL_TYPE);
			break;

		case EDD_SRV_NRT_CANCEL:
			length = sizeof(EDD_RQB_NRT_CANCEL_TYPE);
			break;

		case EDD_SRV_LINK_STATUS_IND_PROVIDE:
			length = sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE);
			break;

		case EDD_SRV_NRT_RECV:
			length = sizeof(EDD_RQB_NRT_RECV_TYPE);
			break;

		case EDD_SRV_NRT_SEND:
			length = sizeof(EDD_RQB_NRT_SEND_TYPE);
			break;

		case EDD_SRV_GET_PARAMS:
			length = sizeof(EDD_RQB_GET_PARAMS_TYPE);
			break;

		case EDD_SRV_GET_PORT_PARAMS:
			length = sizeof(EDD_RQB_GET_PORT_PARAMS_TYPE);
			break;

		case EDD_SRV_MULTICAST:
			length = sizeof(EDD_RQB_MULTICAST_TYPE);
			break;

		default:
			length = 0;
			TCIP_FATAL();
			break;
		}
		
		TCIP_EDD_ALLOC_LOWER_MEM(&pParam, UserId, length, pSys);

		if( is_null(pParam) )
		{
			TCIP_EDD_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
			pRQB = LSA_NULL;
		}
		else
		{
			EDD_RQB_SET_PPARAM( pRQB, pParam );

			if ( sizeBuffer != 0 )
			{
				/* 
				 *   alloc shared memory param block for RX/TX    
				 */
				TCIP_EDD_ALLOC_BUFFER_MEM( ppFrame, (LSA_UINT16)sizeBuffer, pSys);
				
				if( is_null(*ppFrame) )
				{
					LSA_UINT16 ret;
					TCIP_EDD_FREE_LOWER_MEM(&ret, pParam, pSys );
					TCIP_EDD_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
					pRQB = LSA_NULL;
				}
			}
		}
		
	}

	return ( pRQB );
}

/*===========================================================================*/

LSA_VOID tcip_FreeRQB( LSA_SYS_PTR_TYPE pSys, EDD_RQB_TYPE *pRQB )
{
	TCIP_LOG_FCT("tcip_FreeRQB")
	
	LSA_VOID_PTR_TYPE pParam = LSA_NULL;
	LSA_UINT16 ret16_val;

	pParam = TCIP_EDD_GET_PPARAM( pRQB, LSA_VOID_PTR_TYPE );

	if ( is_not_null(pParam) )
	{
		TCIP_EDD_FREE_LOWER_MEM(&ret16_val, pParam, pSys);
		TCIP_ASSERT(ret16_val == LSA_RET_OK);
	}

	TCIP_EDD_FREE_LOWER_RQB(&ret16_val, pRQB, pSys);
	TCIP_ASSERT(ret16_val == LSA_RET_OK);
}

/*===========================================================================*/
/*===========================================================================*/

/*
 * open channel for icmp, udp, tcp and arp
 */
LSA_VOID tcip_open_channel_all(TCIP_CHANNEL_PTR_TYPE channel )
{
	LSA_USER_ID_TYPE                            UserId = { 0 };
	TCIP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
	TCIP_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE    pOpen;
	
	TCIP_EDD_ALLOC_LOWER_RQB(&pRQB, UserId, sizeof(EDD_RQB_TYPE), channel->sysptr );
	TCIP_ASSERT( is_not_null(pRQB) );  

	{
	TCIP_EDD_LOWER_MEM_PTR_TYPE tmp; /* AP01308013, use tmp var to prevent GNU compiler warning */
	TCIP_EDD_ALLOC_LOWER_MEM(&tmp, UserId, sizeof(EDD_RQB_OPEN_CHANNEL_TYPE), channel->sysptr);
	TCIP_ASSERT( is_not_null(tmp) );
	pOpen = tmp;
	}

	TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_OPEN_CHANNEL, 0);
	pRQB->pParam = pOpen;

	pOpen->Cbf = tcip_edd_request_lower_done;

	pOpen->SysPath = channel->syspath;       /* same syspath as "upper (!)  */

	pOpen->HandleUpper = channel->my_handle; /*index of channel in work */

	TCIP_EDD_OPEN_CHANNEL_LOWER(pRQB, channel->sysptr );
}

/*===========================================================================*/

/*
 * callback for all channel open procedures
 */
static LSA_VOID tcip_edd_open_channel_done(TCIP_CHANNEL_PTR_TYPE ch, TCIP_EDD_LOWER_RQB_PTR_TYPE  pRQB)
{
	TCIP_LOG_FCT("tcip_edd_open_channel_done")
	TCIP_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE pOpen = TCIP_EDD_GET_PPARAM(pRQB, TCIP_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE);
	LSA_UINT16 uRetval;

	ch->edd.handle = pOpen->Handle;  /* lower's handle */

	if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "tcip_edd_open_channel_tcp_done: unexpected return code from EDDx(%X)", TCIP_EDD_GET_RESPONSE(pRQB));
		TCIP_FATAL();
	}

	TCIP_EDD_FREE_LOWER_MEM(&uRetval, pRQB->pParam, ch->sysptr);

	TCIP_EDD_FREE_LOWER_RQB(&uRetval, pRQB, ch->sysptr);

	/*
	 *  query edd for interface params only for arp
	 */
	if( TCIP_PATH_TYPE_ARP == ch->detailptr->path_type ) 
	{
		tcip_edd_get_params(ch);
	}
	else
	{
		/*
		 * initialize channel data structures and set channel pointer in interface structs
		 */
		tcip_init_opt_channel(ch);		

		ch->state = TCIP_CH_OPEN;

		tcip_callback(ch, TCIP_OK, &ch->curr);  /* return rqb now */
	}
}

/*===========================================================================*/

/*
 *  called when edd arp channel was opened successfully
 */
static LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_get_params(
	TCIP_CHANNEL_PTR_TYPE channel
) {
	TCIP_LOG_FCT("tcip_edd_get_params")
	
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB = tcip_AllocRQB(channel->sysptr, EDD_SRV_GET_PARAMS, 0, 0);
	TCIP_ASSERT(is_not_null(pRQB));

	/***/

	TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_REQUEST, EDD_SRV_GET_PARAMS);
	TCIP_EDD_SET_HANDLE(pRQB, channel->edd.handle);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PARAMS (EDDHandle: 0x%X,pRQB: 0x%X)",channel->edd.handle, pRQB );
	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
}

/*===========================================================================*/

/*
 *  callback for EDD_SRV_GET_PARAMS
 */
LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_get_params_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
) {
	TCIP_LOG_FCT("tcip_edd_get_params_done")

	TCIP_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_GET_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",channel->edd.handle,TCIP_EDD_GET_RESPONSE(pRQB));

	if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
	{
		TCIP_FATAL();
	}
	else
	{
		TCIP_EDD_RQB_GET_PARAMS_PTR_TYPE p_eddpars = TCIP_EDD_GET_PPARAM(pRQB, TCIP_EDD_RQB_GET_PARAMS_PTR_TYPE);

		channel->trace_idx = p_eddpars->TraceIdx;

		if (
			p_eddpars->MaxInterfaceCntOfAllEDD < 1
			|| p_eddpars->MaxInterfaceCntOfAllEDD > EDD_CFG_MAX_INTERFACE_CNT
			|| p_eddpars->MaxPortCntOfAllEDD < p_eddpars->MaxPortCnt
			|| p_eddpars->MaxPortCntOfAllEDD > EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE
			|| p_eddpars->MaxPortCnt > EDD_CFG_MAX_PORT_CNT )
		{
			TCIP_FATAL(); /* misconfiguration */
		}
		else
		{
			TCIP_IF_DATA_PTR_TYPE p_if_data;

			/*
			 * create and init interface data struct and store pointer in this channel
			 */
			{
			TCIP_LOCAL_MEM_PTR_TYPE tmp; /* AP01308013, use tmp var to prevent GNU compiler warning */
			TCIP_ALLOC_LOCAL_MEM(&tmp, sizeof(*p_if_data));
			p_if_data = tmp;
			}

			if( is_null(p_if_data) )
			{
				TCIP_FATAL();
			}
			else
			{
				p_if_data->nets_count = p_eddpars->MaxInterfaceCntOfAllEDD + p_eddpars->MaxPortCntOfAllEDD;

				p_if_data->edd_interface_id  = p_eddpars->InterfaceID;

				p_if_data->port_count = p_eddpars->MaxPortCnt;

				p_if_data->port[0].MACAddr = p_eddpars->MACAddr;

				p_if_data->port[0].nominal_bandwidth = 100000000; /* 100MBps is an innocent value */

				p_if_data->port[0].ifp = LSA_NULL;

				channel->edd.send_pkts.max_len = MAXPACKETS/p_eddpars->MaxInterfaceCntOfAllEDD; /* limit send queue len, see 946704 */

				channel->p_if_data = p_if_data;

				/***/

				TCIP_UPPER_TRACE_08(0, LSA_TRACE_LEVEL_NOTE_LOW,
					"PortCnt: %u, MAC: %02X-%02X-%02X-%02X-%02X-%02X, nom-bandwidth: %u",
					p_if_data->port_count,
					p_if_data->port[0].MACAddr.MacAdr[0],
					p_if_data->port[0].MACAddr.MacAdr[1],
					p_if_data->port[0].MACAddr.MacAdr[2],
					p_if_data->port[0].MACAddr.MacAdr[3],
					p_if_data->port[0].MACAddr.MacAdr[4],
					p_if_data->port[0].MACAddr.MacAdr[5],
					p_if_data->port[0].nominal_bandwidth
					);

				/***/

				tcip_FreeRQB(channel->sysptr, pRQB );

				tcip_edd_get_portparams(channel);   /* async - will return rqb channel->curr*/
			}
		}
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  retrieve the port parameters from eddx
 */
static LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_get_portparams(
	TCIP_CHANNEL_PTR_TYPE channel
) {
	TCIP_LOG_FCT("tcip_edd_get_portparams")
	
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB = tcip_AllocRQB(channel->sysptr, EDD_SRV_GET_PORT_PARAMS, 0, 0);
	TCIP_ASSERT(is_not_null(pRQB));

	/***/

	TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_GET_PORT_PARAMS);
	TCIP_EDD_SET_HANDLE( pRQB, channel->edd.handle);

	{
	TCIP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE pPortParams = TCIP_EDD_GET_PPARAM(pRQB, TCIP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE);          

	pPortParams->PortID = 1;   /* start with first port */

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PORT_PARAMS (EDDHandle: 0x%X, PortID: %d, pRQB: 0x%X)", channel->edd.handle, pPortParams->PortID, pRQB);
	}

	TCIP_EDD_REQUEST_LOWER( pRQB, channel->sysptr);
}

/*===========================================================================*/

/*
 * callback for EDD_SRV_GET_PORT_PARAMS
 */
LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_get_portparams_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
) {
	TCIP_LOG_FCT("tcip_edd_get_portparams_done")

	if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
	{
		TCIP_FATAL();
	}
	else
	{
		TCIP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE pPortParams = TCIP_EDD_GET_PPARAM(pRQB, TCIP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE);
		TCIP_IF_DATA_PTR_TYPE p_if_data = channel->p_if_data;

		/* save the "port MAC-address" */

		p_if_data->port[pPortParams->PortID].MACAddr = pPortParams->MACAddr;

		/* derive the "nominal bandwith" from the PHY's advertised capabilities */

		{
		LSA_UINT32 nbw;

		if( pPortParams->AutonegCapAdvertised &
			( EDD_AUTONEG_CAP_1000BASEX
			| EDD_AUTONEG_CAP_1000BASEXFD
			| EDD_AUTONEG_CAP_1000BASET
			| EDD_AUTONEG_CAP_1000BASETFD) )
		{
			nbw = 1000000000;
		}
		else if( pPortParams->AutonegCapAdvertised &
			( EDD_AUTONEG_CAP_100BASET4
			| EDD_AUTONEG_CAP_100BASETX
			| EDD_AUTONEG_CAP_100BASETXFD
			| EDD_AUTONEG_CAP_100BASET2
			| EDD_AUTONEG_CAP_100BASET2FD) )
		{
			nbw = 100000000;
		}
		else /* report every thing else as 10MBps */
		{
			nbw = 10000000;
		}

		p_if_data->port[pPortParams->PortID].nominal_bandwidth = nbw;
		}

		/***/

		TCIP_UPPER_TRACE_08(0, LSA_TRACE_LEVEL_NOTE_LOW,
			"Port: %u, MAC: %02X-%02X-%02X-%02X-%02X-%02X, nom-bandwidth: %u",
			pPortParams->PortID,
			pPortParams->MACAddr.MacAdr[0],
			pPortParams->MACAddr.MacAdr[1],
			pPortParams->MACAddr.MacAdr[2],
			pPortParams->MACAddr.MacAdr[3],
			pPortParams->MACAddr.MacAdr[4],
			pPortParams->MACAddr.MacAdr[5],
			p_if_data->port[pPortParams->PortID].nominal_bandwidth
			);

		/***/

		p_if_data->port[pPortParams->PortID].PhyStatus = pPortParams->PhyStatus; /* see 1201670 */

		/*
		 * next port
		 */
			
		pPortParams->PortID++; 

		if( pPortParams->PortID <= p_if_data->port_count )  /* do next port */
		{
			TCIP_EDD_SET_HANDLE( pRQB, channel->edd.handle);

			TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PORT_PARAMS (EDDHandle: 0x%X, PortID: %d, pRQB: 0x%X)", channel->edd.handle, pPortParams->PortID, pRQB);

			TCIP_EDD_REQUEST_LOWER( pRQB, channel->sysptr);
		}
		else
		{
			tcip_FreeRQB(channel->sysptr, pRQB );

			/***/
			
			TCIP_ASSERT( TCIP_PATH_TYPE_ARP == channel->detailptr->path_type );

#if TCIP_CFG_ENABLE_MULTICAST 

			/* add entry for all hosts group (224.0.0.1 -> MAC: 01-00-5E-00-00-01 */
	
			tcip_edd_srv_multicast(channel, 0, htonl(0xE0000001), TCIP_SRV_MULTICAST_ENABLE);		

#else
			tcip_init_arp_channel(channel); /* initialization of "first" channel */

			channel->state = TCIP_CH_OPEN;

			tcip_callback(channel, TCIP_OK, &channel->curr);  /* return rqb now */

#endif
		}
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *   sends nrt receive rqb and increases of pending requests counter
 */
LSA_VOID tcip_edd_nrt_recv( EDD_RQB_TYPE *pRQB, TCIP_CHANNEL_PTR_TYPE channel )
{
	TCIP_LOG_FCT("tcip_edd_nrt_recv")

	if (TCIP_CH_CLOSING == channel->state)
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"sending nrt receive when channel->edd.handle(%u) is closing", channel->edd.handle );

		TCIP_FATAL();
	}
	else
	{
		channel->edd.pending_rcv_reqests ++;
	
		TCIP_EDD_SET_HANDLE(pRQB, channel->edd.handle);
		TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *   cancel NRT_RECV requests in channel 
 */
LSA_VOID tcip_edd_nrt_cancel( TCIP_CHANNEL_PTR_TYPE channel )
{
	TCIP_LOG_FCT("tcip_edd_nrt_cancel")
	
	EDD_UPPER_NRT_CANCEL_PTR_TYPE pParam;
	EDD_UPPER_RQB_PTR_TYPE  pRQB = tcip_AllocRQB( channel->sysptr, EDD_SRV_NRT_CANCEL, 0, 0);
	TCIP_ASSERT( pRQB != 0 );

	pParam = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_NRT_CANCEL_PTR_TYPE );
	pParam->Mode = EDD_NRT_CANCEL_MODE_ALL;
	pParam->RequestID = 0;

	TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_CANCEL);
	TCIP_EDD_SET_HANDLE( pRQB, channel->edd.handle);

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"REQ: EDD_SRV_NRT_CANCEL, handle(%u) ", channel->edd.handle );

	TCIP_EDD_REQUEST_LOWER( pRQB, channel->sysptr);
}

/*===========================================================================*/

/*
 * treat EDD_SRV_NRT_CANCEL opcode confirmation
 */
static LSA_VOID tcip_edd_nrt_cancel_done(TCIP_CHANNEL_PTR_TYPE channel, TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB)
{
	TCIP_LOG_FCT("tcip_edd_nrt_cancel_done")

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"REQ: EDD_SRV_NRT_CANCEL done RSP(%u/%#x)",
										TCIP_EDD_GET_RESPONSE(pRQB), TCIP_EDD_GET_RESPONSE(pRQB));

	tcip_FreeRQB( channel->sysptr, pRQB );

	/*
	 *  check if last receive resource has been cancelled
	 */

	if( (channel->edd.pending_rcv_reqests != 0) ||
		(TCIP_CH_CLOSING != channel->state    )   )
	{
		TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
			, "channel->state(%d) not TCIP_CH_CLOSE(which is '3') or channel->edd.pending_rcv_reqests(%d) not 0 when EDD_SRV_NRT_CANCEL done on channel->edd.handle(%u)"
			, channel->state, channel->edd.pending_rcv_reqests, channel->edd.handle
			);

		TCIP_FATAL();
	}

	tcip_edd_close_channel(channel);
}

/*===========================================================================*/

/*
 *   cancel resources of this channel
 */
LSA_VOID tcip_edd_close_channel(TCIP_CHANNEL_PTR_TYPE channel )
{
	/*
	 * do not close channel until statistic update completes (see tcip_edd_get_mib2_statistics_done)
	 */
	if( !channel->edd.get_statistics_pending	/* statistic update must be finished and  */
		&& channel->edd.in_work_rcv_rqbs_number == 0 ) /* interniche stack must have returned all receive packets */
	{
		TCIP_EDD_LOWER_RQB_PTR_TYPE     pRQB;
		LSA_USER_ID_TYPE                UserId;

		/*
		 * free pre-allocated statistic rqb
		 */
		if( TCIP_PATH_TYPE_ARP == channel->detailptr->path_type )
		{
			tcip_FreeRQB(channel->sysptr, channel->edd.pEdd );	
		}

		UserId.uvar32 = 0;

		TCIP_EDD_ALLOC_LOWER_RQB( &pRQB, UserId, sizeof(EDD_RQB_TYPE), channel->sysptr);

		TCIP_ASSERT( is_not_null(pRQB) );

		TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_CLOSE_CHANNEL, 0);
		TCIP_EDD_SET_HANDLE(pRQB, channel->edd.handle);

		TCIP_EDD_CLOSE_CHANNEL_LOWER( pRQB, channel->sysptr);
	}
}

/*===========================================================================*/

/* 
 *   function: tcip_edd_close_channel_done()                                 
 */
static LSA_VOID tcip_edd_close_channel_done(TCIP_CHANNEL_PTR_TYPE ch, TCIP_EDD_LOWER_RQB_PTR_TYPE  pRQB)
{
	LSA_UINT16          uRetval;

	uRetval = LSA_RQB_GET_RESPONSE(pRQB);

	TCIP_ASSERT(uRetval == EDD_STS_OK);

	TCIP_EDD_FREE_LOWER_RQB(&uRetval, pRQB, ch->sysptr);

	tcip_cancel_send_resources(ch);

	/*
	 *  send channel close confirmation
	 */
	
	tcip_callback_and_free_channel(ch, TCIP_OK, ch->curr );
}

/*===========================================================================*/

#if TCIP_CFG_ENABLE_MULTICAST
/* 
 *   function: tcip_edd_srv_multicast()                                 
 */
LSA_VOID TCIP_LOCAL_FCT_ATTR
tcip_edd_srv_multicast(
	TCIP_CHANNEL_PTR_TYPE channel,
	LSA_INT32 dev_handle,
	LSA_UINT32 mc_address,
	LSA_UINT8 mode
) {
	TCIP_LOG_FCT("tcip_edd_srv_multicast")
	
	EDD_UPPER_MULTICAST_PTR_TYPE pSrvMulti;

	EDD_UPPER_RQB_PTR_TYPE  pRQB = tcip_AllocRQB( channel->sysptr, EDD_SRV_MULTICAST, 0, 0);
	if( is_null(pRQB) )
	{
		TCIP_FATAL();
	}

	pSrvMulti = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_MULTICAST_PTR_TYPE );

	if (mode == TCIP_SRV_MULTICAST_ENABLE )
	{
		pSrvMulti->Mode = EDD_MULTICAST_ENABLE;
	}
	else
	{
		pSrvMulti->Mode = EDD_MULTICAST_DISABLE;
	}

	mc_address = ntohl(mc_address); /* see shift operations below */

	pSrvMulti->MACAddr.MacAdr[0] = 0x01;
	pSrvMulti->MACAddr.MacAdr[1] = 0x00;
	pSrvMulti->MACAddr.MacAdr[2] = 0x5E;
	pSrvMulti->MACAddr.MacAdr[3] = (LSA_UINT8)((mc_address >> 16) & 0x7F);
	pSrvMulti->MACAddr.MacAdr[4] = (LSA_UINT8)(mc_address >> 8);
	pSrvMulti->MACAddr.MacAdr[5] = (LSA_UINT8)(mc_address >> 0);

	TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_REQUEST, EDD_SRV_MULTICAST);
	TCIP_EDD_SET_HANDLE(pRQB, channel->edd.handle);

	EDD_RQB_SET_USERID_UVAR32(pRQB, dev_handle);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_MULTICAST (EDDHandle: 0x%X,pRQB: 0x%X)",channel->edd.handle, pRQB );

	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
}

/*===========================================================================*/

/*----------------------------------------
// tcip_edd_srv_multicast callback
------------------------------------------*/
LSA_VOID tcip_edd_srv_multicast_done(
    TCIP_CHANNEL_PTR_TYPE channel,
    TCIP_EDD_LOWER_RQB_TYPE  *pRQB
) {
	TCIP_LOG_FCT("tcip_iniche_srv_multicast_done")

	LSA_RESPONSE_TYPE result;
	LSA_INT32 dev_handle = (LSA_INT32)EDD_RQB_GET_USERID_UVAR32(pRQB);
	EDD_UPPER_MULTICAST_PTR_TYPE pSrvMulti = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_MULTICAST_PTR_TYPE );

	if (channel->state == TCIP_CH_ALLOCATED) /* return open channel rqb now, see tcip_edd_get_portparams_done() */
	{
		if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
		{
		TCIP_FATAL(); /* (!) must succeed */
		}

		tcip_init_arp_channel(channel); /* initialization of "first" channel */

		channel->state = TCIP_CH_OPEN;

		tcip_callback(channel, TCIP_OK, &channel->curr);
	}
	else if (channel->state == TCIP_CH_CLOSING) /* proceed with channel close, see tcip_close_channel() */
	{
		if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
		{
			TCIP_FATAL(); /* (!) must succeed */
		}

		tcip_edd_nrt_cancel( channel );
	}
	else
	{
		if (pSrvMulti->Mode == EDD_MULTICAST_ENABLE)
		{
			if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
			{
				TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "tcip_iniche_srv_multicast_done: unexpected return code from EDDx(%X)", TCIP_EDD_GET_RESPONSE(pRQB));
			}

			switch(TCIP_EDD_GET_RESPONSE(pRQB))
			{
			case EDD_STS_OK:
				result = TCIP_OK;
				break;
			case EDD_STS_ERR_PARAM:
				result = TCIP_ERR_PARAM;
				break;
			case EDD_STS_ERR_RESOURCE:
				result = TCIP_ERR_RESOURCE;
				break;
			default:
				result = TCIP_ERR_LOWER_LAYER;
			}

			TCIP_SRV_MULTICAST_DONE(result, dev_handle, TCIP_SRV_MULTICAST_ENABLE);
		}
		else /* EDD_MULTICAST_DISABLE */
		{
			if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
			{
				TCIP_FATAL(); /* (!) must succeed */
			}

			TCIP_SRV_MULTICAST_DONE(TCIP_OK, dev_handle, TCIP_SRV_MULTICAST_DISABLE);
		}
	}
	tcip_FreeRQB(channel->sysptr, pRQB);
}

#endif

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID  TCIP_EDD_LOWER_IN_FCT_ATTR
tcip_edd_request_lower_done(
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
) {
	if( is_null(pRQB) )
	{
		TCIP_FATAL();
	}
	else
	{
		TCIP_CHANNEL_PTR_TYPE ch = tcip_channel_from_handle(TCIP_EDD_GET_HANDLE(pRQB));

		if( is_null(ch) )
		{
			TCIP_FATAL();
		}
		else if( TCIP_EDD_GET_OPCODE(pRQB) == EDD_OPC_REQUEST ) /* fast path */
		{
			switch( TCIP_EDD_GET_SERVICE(pRQB) )
			{
			case EDD_SRV_GET_PARAMS:
				tcip_edd_get_params_done(ch, pRQB);
				break;
			
			case EDD_SRV_GET_PORT_PARAMS:
				tcip_edd_get_portparams_done(ch, pRQB);
				break;

			case EDD_SRV_NRT_SEND:
				tcip_edd_nrt_send_done(pRQB, ch);
				break;

			case EDD_SRV_NRT_RECV:
				tcip_edd_nrt_recv_done(pRQB, ch);
				break;

			case EDD_SRV_NRT_CANCEL:
				tcip_edd_nrt_cancel_done(ch, pRQB);
				break;

			case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
				tcip_edd_ext_link_status_indication(ch, pRQB);
				break;

			case EDD_SRV_GET_STATISTICS_ALL:
				tcip_edd_get_mib2_statistics_done(ch, pRQB);
				break;

#if TCIP_CFG_ENABLE_MULTICAST
			case EDD_SRV_MULTICAST:
				tcip_edd_srv_multicast_done(ch, pRQB);
				break;
#endif

			default:
				TCIP_FATAL();
				break;
			}
		}
		else /* slow path */
		{
			switch( TCIP_EDD_GET_OPCODE(pRQB) )
			{
			case EDD_OPC_OPEN_CHANNEL:
				tcip_edd_open_channel_done(ch, pRQB);
				break;
			
			case EDD_OPC_CLOSE_CHANNEL:
				tcip_edd_close_channel_done(ch, pRQB);
				break;
			
			default:
				TCIP_FATAL();
				break;
			}
		}
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
