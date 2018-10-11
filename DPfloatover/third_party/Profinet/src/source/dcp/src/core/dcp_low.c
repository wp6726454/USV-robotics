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
/*  C o m p o n e n t     &C: DCP (Discovery & Configuration Protocol)  :C&  */
/*                                                                           */
/*  F i l e               &F: dcp_low.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Lower Interface (to Layer 2)                                             */
/*                                                                           */
/*****************************************************************************/
#ifdef DCP_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2002-07-17  P00.00.00.00_00.01.00.00  lrg  file creation                 */
/*  2002-10-31  P01.00.00.00_00.01.01.01  lrg  1.Increment:Identify Response */
/*  2002-12-04  P01.00.00.00_00.02.00.00  lrg  2.Increment:Get/Set Response  */
/*  2003-03-20  P01.00.00.00_00.03.00.01  lrg  RELEASE_PATH_INFO() vor Upper */
/*              Callback aber nach freigeben der lower RBs                   */
/*  2003-04-10  P01.00.00.00_00.03.00.02  lrg  dcp_DataRecv():               */
/*              Empfangslaenge im lower RB auf Maximum setzen                */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*  2003-05-09  P01.00.00.00_00.04.00.01  lrg  dcp_OpenLowerError(): Fehler- */
/*              behandlung ergaenzt; dcp_CloseLowerFin(): Fehlerbehandlung   */
/*              bei close_channel_lower-Fehler                               */
/*  2003-06-02  P01.00.00.00_00.06.00.00   lrg  6.Increment:                 */
/*              dcp_CloseLowerFin(): Nach dcp_ChFree() nicht den Kanalstatus */
/*              setzen.                                                      */
/*  2003-07-30  P01.00.00.00_00.08.00.00  lrg  8.Increment: Client channel   */
/*  2003-09-29  P01.00.00.00_00.11.00.00  lrg 11.Increment: Mehrere IDENTIFY */
/*              gleichzeitig: Timerindex in User-ID.                         */
/*  2003-12-15  P02.00.00.00_00.01.00.00  lrg 1.Increment der Version 2      */
/*  2003-02-12  P02.00.00.00_00.02.00.00  lrg Serverfunktionen nur im        */
/*              DCP_SERVER_MODE aufrufen                                     */
/*  2004-03-05  P02.00.00.00_00.02.02.02  lrg 040305lrg001                   */
/*              Bei Empfang einer Client-Response ohne Clientkanal oder      */
/*              Server-Request ohne Serverkanal:                             */
/*              ignorieren und neuen Receive runtergeben.                    */
/*  2004-03-26  P02.00.00.00_00.02.02.03  lrg 040326lrg001                   */
/*              dcp_request_lower_eth_done() Bei Empfang den OFFSET der      */ 
/*              DCP-PDU wg. VlanTag beachten                                 */
/*  2004-04-22  P02.00.00.00_00.02.05.02  lrg 220404lrg003: OpenPu nicht     */
/*              mit NULL ueberschreiben (bei Join Server zu Client-Channel)  */
/*  2004-04-20  P02.00.00.00_00.03.00.00  lrg  Servers lower Receive RQBs    */
/*              beim open channel nicht runtergeben, sondern queuen.         */   
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg  Version 3                     */
/*  2004-10-11  P03.02.00.00_00.02.00.00  lrg  011004lrg001: support for     */
/*              more than 1 primary IDENTIFY filter ( e.g. Alias names)      */
/*              dcp_LowerFilterSet() mit FilterCount versorgt                */
/*              111004lrg001: Eingestellte Broadcastadr. wird zurückgesetzt. */
/*  2004-12-17  P03.02.00.00_00.02.02.00  lrg  171204lrg001: New parameter   */
/*              RequestCount for dcp_ClientInd()                             */
/*  2004-12-21  P03.02.00.00_00.02.02.01  lrg  dcp_request_lower_eth_done(): */
/*              Definition von Xid auch ohne DCP_CLIENT_MODE                 */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  Copyright-Text                */
/*  2005-01-12  P03.03.00.00_00.03.02.00  lrg  120105lrg001: New parameter   */
/*              RequestCount for dcp_ServerInd()                             */
/*  2005-01-13  P03.03.00.00_00.03.02.01  lrg  130105lrg001: New parameter   */
/*              Client Recv-RQB-Zaehler dekrementieren                       */
/*  2005-02-16  P03.04.00.00_00.04.00.00  lrg  LSA-Trace eingebaut           */
/*              LSA_UNUSED_ARG() fuer Parameter, die ohne Trace unused sind  */
/*  2006-06-30  P03.12.00.00_00.01.01.02  lrg  LSA_GLOB 2.1.0.0_0.7.2.1      */
/*  2006-07-28  P03.12.00.00_00.25.02.02  280706lrg001: Lower Memoryblock    */
/*              fuer EDD-Identify-Filter in dcp_ChOpen anfordern             */
/*  2007-03-16  P04.00.00.00_00.01.01.02  lrg  FSU-Channel                   */
/*              280307lrg001: Interface MAC-Adresse vom EDD holen            */
/*  2007-12-05  P04.00.00.00_00.60.01.02 051207lrg001: ArtsRQ AP00552010     */
/*              Beim CloseChannel auf Sendeconfirmations warten.             */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2010-08-24  P05.00.00.00_00.01.01.02 240810lrg001: AP00999022 UPPER_DONE */
/*  2010-11-23  P05.00.00.00_00.02.01.02 231110lrg001: AP00988516            */
/*              DCP-Identify-Filter on confirmation                          */
/*  2011-01-18  P05.00.00.00_00.03.12.02 lrg: AP01017495 Index-Trace         */
/*  2011-08-03  T05.00.00.00_00.05.44.02 020811lrg: AP01222264               */
/*              Check Client TimerIdx and Handle from XID                    */
/*  2011-08-25  S05.01.00.00_00.04.03.02 AP00696784: Comments in english     */
/*  2011-10-17  T05.01.00.00_00.04.17.02 AP01257857: LTRC_... -> LSA_...     */
/*              Do not free RQB/MEMORY in dcp_lower_*, only call FATAL       */
/*  2012-01-11  T05.01.00.00_00.05.13.01                                     */
/*              AP01297962: remove DCP_CLIENT_MODE and DCP_SERVER_MODE       */
/*  2012-03-08  S05.02.00.00_00.01.16.01 AP01138373: DCP_ETH_FILTER_SET      */
/*              Entfernen des DCP Identify Filter Service in DCP             */
/*  2013-03-04  P05.02.00.00_00.04.17.02 AP01503994: free upper 2 bit of XID */
/*              DCP_MAX_CHANNELS = 63, lower RQB Makros dcp_cfg.h->dcp_low.h */
/*  2013-05-16  P05.03.00.00_00.01.10.01 AP01509782 DCP channel details und  */
/*              Konfigurationsschalter bereinigen                            */
/*  2013-07-18  P05.03.00.00_00.01.10.01 AP01567955 DCP: Warnings bei        */
/*              Generierung des PnDriver entfernen                           */
/*  2013-09-11  P05.03.00.00_00.02.13.01 Request 740218 - Es werden im       */
/*              AllocOnRequest-Mode pro Client-Identify zu viele Recv-RQBs   */
/*              allokiert                                                    */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	3		/* DCP_MODULE_ID_LOW */
#define DCP_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "dcp_inc.h"
#include "dcp_int.h"
DCP_FILE_SYSTEM_EXTENSION(DCP_MODULE_ID)

/*****************************************************************************/
/* External LSA access function: dcp_request_lower_eth_done()                */
/* Lower Ethernet RQB returns to DCP after service execution.                */
/*****************************************************************************/
LSA_VOID DCP_LOWER_IN_FCT_ATTR dcp_request_lower_eth_done(
DCP_ETH_RQB_PTR_TYPE  RblPtr)
{
	LSA_OPCODE_TYPE			Opcode;
	DCP_ETH_SERVICE_TYPE   	Service;
	DCP_LOWER_RQB_PTR_TYPE	RblPu;
	DCP_ETH_SEND_PTR		SndPtr;
	DCP_ETH_RECV_PTR		RcvPtr;
	LSA_USER_ID_TYPE       	UserId;
	LSA_UINT16				Response;
	DCP_CHL_PTR				ChLowPtr;
	DCP_CHA_PTR  			ChPtr = LSA_NULL;
	DCP_PDU				*	PduPtr = LSA_NULL;		/* 040326lrg001 */
	LSA_UINT32				Xid = 0;
	LSA_UINT32				TraceIdx = 0;
	LSA_UINT8				Uid = 0;
	LSA_UINT16		    	RecvLen = 0;
	LSA_UINT16				DataLen = 0;

    DCP_ENTER();

	RblPu.RbEthPtr = RblPtr;

 	/* Read Opcode, Service, UserID und Response from lower RQB
	*/
	Opcode = DCP_ETH_RQB_OPCODE_GET(RblPtr);
	Service = DCP_ETH_RQB_SERVICE_GET(RblPtr);
	UserId.uvar32 = DCP_ETH_RQB_USERID_UVAR32_GET(RblPtr);
	Response = DCP_ETH_RQB_RESPONSE_GET(RblPtr);

	DCP_LOWER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE, ">>> dcp_request_lower_eth_done() RblPtr=%X UserPtr=%X Response=0x%x",
					RblPtr, UserId.void_ptr, Response);

	switch (Response)
	{
	case DCP_ETH_RSP_OK:
		Response = DCP_RSP_OK;
		break;
	case DCP_ETH_RSP_CANCEL:
		Response = DCP_RSP_OK_CANCEL;
		break;
	default:
		Response = DCP_RSP_ERR_LOWER_LAYER;

		 /* Todo: support Error structure
		  * 
		if (LSA_COMP_ID_UNUSED != DCP_ETH_RQB_ERR_COMPID_GET(_RbPtr))
		    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_CONFIG, RblPtr, 
		    				sizeof(DCP_ETH_RQB_TYPE));
		*/ 
	}

	ChLowPtr = dcp_ChLowGet(DCP_ETH_RQB_HANDLE_GET(RblPtr));

	if (!LSA_HOST_PTR_ARE_EQUAL(ChLowPtr, LSA_NULL))
	{
		TraceIdx = ChLowPtr->TraceIdx;
		DCP_LOWER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE,"  > dcp_request_lower_eth_done() Opcode=%u Service=0x%x UserId=0x%x", 
					Opcode, Service, UserId.uvar32);

		if ((Opcode != DCP_ETH_OPC_REQUEST) || (Service != DCP_ETH_SEND))
		{
			/* Build Upper Channel Pointer from User-ID
			*/
			ChPtr = dcp_ChGet(UserId.uvar8_array[DCP_HANDLE_UID]);
			if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL)
			&& (Service == DCP_ETH_RECV))
			{
				/* Build temporary channel pointer to free RQB
				*/
				if (!LSA_HOST_PTR_ARE_EQUAL(ChLowPtr->ChServerPtr, LSA_NULL))
					ChPtr = ChLowPtr->ChServerPtr;
				else if (!LSA_HOST_PTR_ARE_EQUAL(ChLowPtr->ChClientPtr, LSA_NULL))
					ChPtr = ChLowPtr->ChClientPtr;
				else
					dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CONFIG, ChPtr, UserId.uvar32);
			}
		}

		switch (Opcode)
		{
		case DCP_ETH_OPC_OPEN_CHANNEL:
			dcp_OpenLowerGet(ChPtr, RblPu);		/* 280307lrg001 */
			break;

		case DCP_ETH_OPC_CLOSE_CHANNEL:

			/* Finish closing channel
			*/
			dcp_CloseLowerFin(ChPtr, RblPu);
			break;

		case DCP_ETH_OPC_REQUEST:

			/* Decode lower service
			*/
			switch (Service)
			{
			/* 280307lrg001
			*/
			case DCP_ETH_GET_PARAMS:
				dcp_OpenLowerSet(ChPtr, RblPu);
				break;

			case DCP_ETH_MULTICAST:
				if (ChLowPtr->State == DCP_CHA_STATE_CLOSING)		/* 111004lrg001 */
				{
					Response = dcp_CloseLower(ChPtr, RblPu);		/* 111004lrg001 */
				}
				else
				{
					Response = dcp_OpenLowerFin(ChPtr, RblPu);
				}
				break;

			case DCP_ETH_RECV:
				ChPtr->ChLowPtr->RecvRblAct--;
				
				if (Response == DCP_RSP_OK)
				{
					RcvPtr = (DCP_ETH_RECV_PTR)DCP_ETH_RQB_RECV_PTR_GET(RblPtr);
					RecvLen = DCP_ETH_RQB_RECV_LEN_GET(RblPtr);
					DataLen = DCP_ETH_RQB_RECV_DATA_LEN_GET(RblPtr);

					DCP_SNDRCV_TRACE_BYTE_ARRAY(TraceIdx, LSA_TRACE_LEVEL_NOTE, "==> dcp recv:", (LSA_UINT8*)RcvPtr, RecvLen);
					PduPtr = (DCP_PDU*)((LSA_UINT8*)RcvPtr + DCP_ETH_RQB_RECV_DATA_OFFSET_GET(RblPtr) + 2);

					/* Check DCP PDU // 120913lrg740218 moved from dcp_ServerInd()
					*/
					if ((DataLen < (DCP_HEADER_SIZE + 2)) || (RecvLen > sizeof(DCP_ETH_FRAME)))
					{
						/* This is not a DCP Frame  
						*/
						DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_request_lower_eth_done() ChLowPtr=%X RecvLen=%u Invalid DCP frame size=%u", 
								ChLowPtr, RecvLen, DataLen);
						ChPtr = LSA_NULL;
						Response = DCP_RSP_ERR_SYNTAX;
					}
					else
					{
						if (DCP_IS_REQUEST(PduPtr))
						{
							ChPtr = ChLowPtr->ChServerPtr;
						}
						else
						{
							DCP_SWAPD(&PduPtr->Rsp.Hdr.XidHigh, &Xid);
	
							ChPtr = dcp_ChGet(DCP_HIBYTE(DCP_HIWORD(Xid & DCP_CLIENT_XID_MASK)));
	
							/* 020811lrg: Check Client TimerId and Handle from XID
							 * AP01503994: max. DCP_MAX_CHANNELS = 63 to free uppermost 2 bits of XID (upper channel handle)
							*/
							if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL)
							|| (DCP_LOBYTE(DCP_HIWORD(Xid)) >= ChPtr->TimerCount)
							|| ((Xid >> 30) != DCP_CLIENT_XID_PNIO)
							|| (!LSA_HOST_PTR_ARE_EQUAL(ChLowPtr, ChPtr->ChLowPtr)))
							{
								DCP_ERROR_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** dcp_request_lower_eth_done() Invalid Xid=0x%08x in response [%x%x%x]", 
												Xid,0,0,0);
								ChPtr = LSA_NULL;
								Response = DCP_RSP_ERR_SYNTAX;
							}
						}
					}
				}
				if (ChLowPtr->State == DCP_CHA_STATE_CLOSING)
				{
					/* Closing channel: do not process any Request
					 * Free Lower Requestblock
					 * 020811lrg: try to get a channel for dcp_FreeLowerRb
					*/
					if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
					{
						if (!LSA_HOST_PTR_ARE_EQUAL(ChLowPtr->ChServerPtr, LSA_NULL))
						{
							ChPtr = ChLowPtr->ChServerPtr;
						}
						else if (!LSA_HOST_PTR_ARE_EQUAL(ChLowPtr->ChClientPtr, LSA_NULL))
						{
							ChPtr = ChLowPtr->ChClientPtr;
						}
					}
					if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
					{
						dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CONFIG, ChLowPtr->ChServerPtr, Xid);
					}
					else
					{
						dcp_FreeLowerRb(ChPtr, RblPu);
					}
				}
				else if ((Response == DCP_RSP_ERR_SYNTAX)
					  ||  ((Response == DCP_RSP_OK) && !DCP_IS_REQUEST(PduPtr))
					  ||  ((Response == DCP_RSP_OK_CANCEL) && !LSA_HOST_PTR_ARE_EQUAL(ChPtr, ChLowPtr->ChServerPtr)) 
					  ||  ((Response == DCP_RSP_ERR_LOWER_LAYER) && (UserId.uvar8_array[DCP_CLIENT_UID] != DCP_NOSERVICE)))
				{
					/* Response to Client Request or invalid DCP frame
					*/
					if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
					{
						if (Response == DCP_RSP_OK)
						{
							Uid = PduPtr->Rsp.Hdr.ServiceId;
						}
						else
						{
							Uid = UserId.uvar8_array[DCP_CLIENT_UID];
							Xid = UserId.uvar8_array[DCP_INDEX_UID];
						}
						/* 171204lrg001: RequestCount of lower Receive Queue added
						*/
						dcp_ClientInd(ChPtr, RblPu, Response, Uid, Xid, DCP_ETH_RQB_RECV_CNT_GET(RblPtr));
					}
					else
					{
						/* 040305lrg001: No client channel open:
						 * ignore Response and send new receive request
						 * 221204lrg001: load ServerPtr only in Server Mode 
						*/
						ChPtr = ChLowPtr->ChServerPtr;
						if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
						{
							ChPtr = ChLowPtr->ChClientPtr;
						}
						if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
						{
							dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CONFIG, ChLowPtr->ChServerPtr, UserId.uvar32);
						}
						else
						{
							/* 130105lrg001: decrement Client Recv-RQB count
							*/
							if (UserId.uvar8_array[DCP_CLIENT_UID] != 0)
							{
								ChPtr->ChLowPtr->RecvRblClient--;
							}
							dcp_DataRecv(ChPtr,	RblPu, UserId.uvar8_array[DCP_INDEX_UID], 
										UserId.uvar8_array[DCP_CLIENT_UID]);
						}
					}
				}
				else
				{
					/* DCP-Server Request or no Client configured: 
					 * pass the indication to server
					*/
					if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
					{
//						if (ChPtr->DetailPtr->DevId == DCP_DEV_FSU)
//							ChPtr = ChPtr->ChNextPtr;
						if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
							dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_DEVICE, ChLowPtr->ChServerPtr, UserId.uvar32);
						else
							/* 120105lrg001: RequestCount of lower Receive Queue added
							*/
							dcp_ServerInd(ChPtr, RblPu, DCP_ETH_RQB_RECV_CNT_GET(RblPtr));
					}
					else
					{
						/* 040304lrg001: no Server channel oüen oder no Server configured:
						 * ignore Request and send new receive request
						 * 221204lrg001: load ClientPtr only in client mode
						*/
						ChPtr = ChLowPtr->ChClientPtr;
						if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
							dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CONFIG, ChLowPtr->ChServerPtr, UserId.uvar32);
						else
						{
							/* 130105lrg001: decrement Client Recv-RQB count
							*/
							if (UserId.uvar8_array[DCP_CLIENT_UID] != 0)
								ChPtr->ChLowPtr->RecvRblClient--;
							dcp_DataRecv(ChPtr,	RblPu, UserId.uvar8_array[DCP_INDEX_UID], 
										UserId.uvar8_array[DCP_CLIENT_UID]);
						}
					}
				}
				break;
	
			case DCP_ETH_SEND:
				SndPtr = (DCP_ETH_SEND_PTR)DCP_ETH_RQB_SEND_PTR_GET(RblPtr);
				PduPtr = &SndPtr->Pdu;
				if (DCP_IS_REQUEST(PduPtr))
 					ChPtr = dcp_ChGet(UserId.uvar8_array[DCP_HANDLE_UID]);
				else
					ChPtr = ChLowPtr->ChServerPtr;

				if (LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
					dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CONFIG, ChLowPtr->ChServerPtr, UserId.uvar32);
				else
				{
					ChPtr->SendReqCount--;							//051207lrg001
					if (ChLowPtr->State == DCP_CHA_STATE_CLOSING)
					{
						/* Channel is closing: free lower Requestblock
						*/
						dcp_FreeLowerRb(ChPtr, RblPu);
					}
					else if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr, ChLowPtr->ChServerPtr))
					{
						/* Send confirmation of a Client Request
						*/
						DCP_SWAPD(&SndPtr->Pdu.Req.Hdr.XidHigh, &Xid);
						dcp_ClientConf(ChPtr, RblPu, Response, Xid, SndPtr->Pdu.Req.Hdr.ServiceId);
					}
					else
					{
						/* DCP-Server Channel: pass Confirmation to server
						*/
						dcp_ServerConf(ChPtr, RblPu);
					}
				}
				break;

			case DCP_ETH_CANCEL:
				if (ChLowPtr->State == DCP_CHA_STATE_CLOSING)
				{
					dcp_CloseLowerSet(ChPtr, RblPu);							/* 111004lrg001 */
				}
				else if (DCP_NOSERVICE != UserId.uvar8_array[DCP_CLIENT_UID])
				{
					/* Call Client function
					*/
					dcp_ClientCancel(ChPtr, RblPu, Response, 
								UserId.uvar8_array[DCP_CLIENT_UID], UserId.uvar8_array[DCP_INDEX_UID]);
				}
				else
				{
					/* Server-Cancel confirmation
					*/
					dcp_ServerCancel(ChPtr, RblPu);
				}
				break;

 			default:
			    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_BAD_SERVICE, RblPtr, 
			    				sizeof(DCP_ETH_RQB_TYPE));
			}
			break;
			
			/* End of: case DCP_ETH_OPC_REQUEST
			*/

		default:
		    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_BAD_OPCODE, RblPtr, 
		    				sizeof(DCP_ETH_RQB_TYPE));
		}
	}
	else
	{
		/* No channel
		*/
	    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_CH_PTR, RblPtr, 
	    				sizeof(DCP_ETH_RQB_TYPE));
	}

	DCP_LOWER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE, "<<< dcp_request_lower_eth_done() RblPtr=%X, ChPtr=%X Service=0x%x", 
					RblPtr, ChPtr, Service);
    DCP_EXIT();
}

/*****************************************************************************/
/* Internal access function: dcp_OpenLowerStart()                            */
/* Allocate lower request block.                                             */
/*****************************************************************************/

LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_OpenLowerStart(
DCP_CHA_PTR  			ChPtr)
{
	LSA_USER_ID_TYPE        UserId;
	DCP_LOWER_RQB_PTR_TYPE	RblPu;
	LSA_UINT16 				RbuRespVal = DCP_RSP_OK_ACTIVE;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_OpenLowerStart() ChPtr=%X Handle=%d DevId=%d", 
						ChPtr, ChPtr->Handle, ChPtr->DetailPtr->DevId);

	RblPu.RbEthPtr = LSA_NULL;
	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		UserId.uvar32 = 0;
		UserId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;
		UserId.uvar8_array[DCP_CLIENT_UID] = DCP_NOSERVICE;
		UserId.uvar8_array[DCP_RBTYPE_UID] = DCP_LOWER_OPEN;

		/* Allocate Lower Requestblock
		*/
		DCP_ALLOC_LOWER_RQB(&RblPu, UserId, sizeof(DCP_LOWER_RQB_TYPE), ChPtr->SysPtr);

		/* If the lower RB has been delivered synchronously...
		*/
		if(!LSA_HOST_PTR_ARE_EQUAL(RblPu.RbEthPtr, LSA_NULL))
		{
			/* ...put it into the channels queue and request lower parameter block
			*/
			DCP_ETH_RQB_USERID_UVAR32_SET(RblPu.RbEthPtr, UserId.uvar32);
			dcp_EnqueLower(&ChPtr->QueueLower, RblPu, ChPtr);
			RbuRespVal = dcp_OpenLowerMem(ChPtr);
		}
		else
		{
			/* Next step: get RQB asynchronously by dcp_lower_rqb()
			 * is currently not supported!
			*/
			RbuRespVal = DCP_RSP_ERR_RESOURCE;
		}
	}
	else
	{
		/* currently only Ethernet is supported
		*/
		RbuRespVal = DCP_RSP_ERR_CONFIG;
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_OpenLowerStart() ChPtr=%X RbEthPtr=%X RespVal=0x%x", 
						ChPtr, RblPu.RbEthPtr, RbuRespVal);
	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal access function: dcp_OpenLowerMem()                              */
/* Allocate lower data/params block.                                         */
/*****************************************************************************/

LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_OpenLowerMem(
DCP_CHA_PTR  			ChPtr)
{
	LSA_USER_ID_TYPE        UserId;
	DCP_LOWER_RQB_PTR_TYPE	RblPu;
	DCP_LOWER_MEM_PTR_TYPE	MemPu;
	LSA_UINT16 				RbuRespVal = DCP_RSP_OK_ACTIVE;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_OpenLowerMem() ChPtr=%X Handle=%d DevId=%d", 
						ChPtr, ChPtr->Handle, ChPtr->DetailPtr->DevId);

	MemPu.MemEthPtr = LSA_NULL;
	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		UserId.uvar32 = 0;
		UserId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;
		UserId.uvar8_array[DCP_CLIENT_UID] = DCP_NOSERVICE;
		UserId.uvar8_array[DCP_RBTYPE_UID] = DCP_LOWER_OPEN;

		/* Request lower memory
		*/
		DCP_ALLOC_LOWER_MEM(&MemPu, UserId, sizeof(DCP_ETH_PAR_TYPE), ChPtr->SysPtr);

		/* ...put pointer to the memory or NULL into queued RQB 
		*/
		RblPu.RbEthPtr = ChPtr->QueueLower.FirstPu.RbEthPtr;
		DCP_ETH_RQB_PPARAM_SET(RblPu.RbEthPtr, MemPu.MemEthPtr);

		/* If the lower PB has been delivered synchronously...
		*/
		if(!LSA_HOST_PTR_ARE_EQUAL(MemPu.MemEthPtr, LSA_NULL))
		{
			/* ...open lower channel
			*/
			dcp_OpenLowerCh(ChPtr);
		}
		else
		{
			/* Next step: get RQB asynchronously by dcp_lower_mem()
			 * is Currently not supported!
			*/
			RbuRespVal = DCP_RSP_ERR_RESOURCE;
		}
	}
	else
	{
		/* currently only Ethernet is supported
		*/
		RbuRespVal = DCP_RSP_ERR_CONFIG;
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_OpenLowerMem() ChPtr=%X MemEthPtr=%X RespVal=0x%x", 
						ChPtr, MemPu.MemEthPtr, RbuRespVal);
	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: dcp_OpenLowerCh()                                      */
/* Open Lower Channel.                                                       */
/*****************************************************************************/

LSA_VOID DCP_LOCAL_FCT_ATTR dcp_OpenLowerCh(
DCP_CHA_PTR  			ChPtr)
{
	DCP_LOWER_RQB_PTR_TYPE	RblPu;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_OpenLowerCh() ChPtr=%X RbEthPtr=%X DevId=%d", 
						ChPtr, ChPtr->QueueLower.FirstPu.RbEthPtr, ChPtr->DetailPtr->DevId);

	RblPu.RbEthPtr = LSA_NULL;
	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		ChPtr->ChLowPtr->State = DCP_CHA_STATE_OPENING;
		RblPu.RbEthPtr = ChPtr->QueueLower.FirstPu.RbEthPtr;
		
		/* Fill lower requst block for open_channel()
		*/
		DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_OPEN_CHANNEL);
		DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, 0);
		DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->Handle);
		DCP_ETH_RQB_HANDLE_UPPER_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->Handle);
		DCP_ETH_RQB_SYS_PATH_SET(RblPu.RbEthPtr, ChPtr->SysPath);
		DCP_ETH_RQB_CBF_PTR_SET(RblPu.RbEthPtr, dcp_request_lower_eth_done);
		DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);

		/* Get requst block from queue and send it to lower layer
		*/
		dcp_DequeLower(&ChPtr->QueueLower, RblPu, ChPtr);
		DCP_OPEN_CHANNEL_LOWER(RblPu, ChPtr->SysPtr);

		/* At callback:  dcp_request_lower_eth_done() and dcp_OpenLowerGet()
		*/
	}
	/* else: currently only Ethernet is supported
	*/
 	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_OpenLowerCh() ChPtr=%X RbEthPtr=%X", 
 						ChPtr, RblPu.RbEthPtr);
}

/*****************************************************************************/
/* Internal function: dcp_OpenLowerGet()                                     */
/* Get local interface MAC address. (280307lrg001)                           */
/*****************************************************************************/

LSA_VOID DCP_LOCAL_FCT_ATTR dcp_OpenLowerGet(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{
	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_OpenLowerGet() ChPtr=%X RbEthPtr=%X DevId=%d", 
						ChPtr, RblPu.RbEthPtr, ChPtr->DetailPtr->DevId);

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		if (DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr) == DCP_ETH_RSP_OK)
		{
			/* Store lower channel handle in channel data structure
			*/
			ChPtr->ChLowPtr->State = DCP_CHA_STATE_OPEN;
			ChPtr->ChLowPtr->HandleLower = DCP_ETH_RQB_HANDLE_LOWER_GET(RblPu.RbEthPtr);

			/* Fill Lower Requstblock for REQUEST, Service: GetParams
			*/
			DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
			DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
			DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, DCP_ETH_GET_PARAMS);
			DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);

			/* Send Lower Request
			*/
			DCP_REQUEST_LOWER(RblPu, ChPtr->SysPtr);

			/* On Callback: dcp_request_lower_eth_done() and dcp_OpenLowerSet()
			*/
		}
		else
		{
			/* Error opening Lower Layer Channel
			*/
			dcp_OpenLowerError(ChPtr, RblPu, DCP_RSP_ERR_LOWER_LAYER);
		}
	}
	/* else: currently only Ethernet is supported
	*/
	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_OpenLowerGet() ChPtr=%X RbEthPtr=%X", 
						ChPtr, RblPu.RbEthPtr);
}

/*****************************************************************************/
/* Internal function: dcp_OpenLowerSet()                                     */
/* Activate Lower Channel Multicast address.                                 */
/*****************************************************************************/

LSA_VOID DCP_LOCAL_FCT_ATTR dcp_OpenLowerSet(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_OpenLowerSet() ChPtr=%X RbEthPtr=%X DevId=%d", 
						ChPtr, RblPu.RbEthPtr, ChPtr->DetailPtr->DevId);

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		if (DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr) == DCP_ETH_RSP_OK)
		{
			/* Copy local interface MAC address and trace index from parameter block to channel data
			*/
			DCP_ETH_RQB_GETPARAMS_MACADDR_GET(RblPu.RbEthPtr, ChPtr->ChLowPtr->LocalMacAdr);
			ChPtr->ChLowPtr->TraceIdx = DCP_ETH_RQB_GETPARAMS_TRACEIDX_GET(RblPu.RbEthPtr);
			ChPtr->TraceIdx = ChPtr->ChLowPtr->TraceIdx;

			if(DCP_DEV_FSU != ChPtr->DetailPtr->DevId)
			{
				/* Enable IDENTIFY Multicast address
				*/
	 			DCP_MAC_ADR IdentifyAddr = DCP_IDENTIFY_MC_ADDR;
				DCP_ETH_RQB_MULTICAST_ENABLE(RblPu.RbEthPtr, IdentifyAddr);

				/* Calculate time constant for Identify from locale MAC address
				*/
	            dcp_TimerCalc(ChPtr);
			}
			else
			{
				/* Enable HELLO Multicast address
				*/
	 			DCP_MAC_ADR HelloAddr = DCP_HELLO_MC_ADDR;
				DCP_ETH_RQB_MULTICAST_ENABLE(RblPu.RbEthPtr, HelloAddr);
			}

			/* Fill Lower Requstblock for REQUEST, Service: Multicast Set
			*/
			DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
			DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
			DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, DCP_ETH_MULTICAST);
			DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);

			/* Send Lower Request
			*/
			DCP_REQUEST_LOWER(RblPu, ChPtr->SysPtr);

			/* On Callback: dcp_request_lower_eth_done() and dcp_OpenLowerFin()
			*/
		}
		else
		{
			/* Error reading parameters of lower channel 
			*/
			dcp_OpenLowerError(ChPtr, RblPu, DCP_RSP_ERR_LOWER_LAYER);
		}
	}
	/* else: currently only Ethernet is supported
	*/
	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_OpenLowerSet() ChPtr=%X RbEthPtr=%X", 
						ChPtr, RblPu.RbEthPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_OpenLowerFin()                              */
/* Finalize opening Lower Channel.                                           */
/*****************************************************************************/

LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_OpenLowerFin(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{
	LSA_INT 	i, NumRbu;
	LSA_UINT16 	RbuRespVal = DCP_RSP_OK;


	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_OpenLowerFin() ChPtr=%X RbEthPtr=%X DevId=%d", 
						ChPtr, RblPu.RbEthPtr, ChPtr->DetailPtr->DevId);

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		if ((!LSA_HOST_PTR_ARE_EQUAL(RblPu.RbEthPtr, LSA_NULL))
		&& (DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr) != DCP_ETH_RSP_OK))
		{
			/* Error setting Multicast address
			*/
			RbuRespVal = DCP_RSP_ERR_LOWER_LAYER;
		}
		else
		{
			if (!ChPtr->DetailPtr->ClientCh
			||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
			{
				/* Server- or FSU channel:
				 * allocate Lower receive resources
				*/
				if (!ChPtr->DetailPtr->AllocOnRequest)
				{
					if (ChPtr->DetailPtr->DevId == DCP_DEV_FSU)
					{
						NumRbu = ChPtr->DetailPtr->ServerHelloResources;
					}
					else
					{
						NumRbu = DCP_NUM_SERVER_IDENTIFY_RESOURCES
								+ DCP_NUM_SERVER_SET_RESOURCES
								+ DCP_NUM_SERVER_GET_RESOURCES;
					}
					for (i = 0; i < NumRbu; i++)
					{
						/* Allocate Receive Requestblock with data buffer and queue it
						*/
						RbuRespVal = dcp_AllocEthRecv(ChPtr, 0, DCP_NOSERVICE, LSA_NULL);
						if (DCP_RSP_OK == RbuRespVal)
							ChPtr->QueueRecvCnt++;
						else
							break;
					}
					if ((RbuRespVal == DCP_RSP_OK)
					&&  (ChPtr->DetailPtr->DevId != DCP_DEV_FSU))
					{
						/* Allocate Lower Request-/Data blocks for SEND REQUEST
						 * and queue it to LowerQueue
						*/
						NumRbu = DCP_NUM_SERVER_IDENTIFY_RESOURCES
								+ DCP_NUM_SERVER_SET_RESOURCES
								+ DCP_NUM_SERVER_GET_RESOURCES;
						for (i = 0; i < NumRbu; i++)
						{
							RbuRespVal = dcp_AllocEthSend(ChPtr, 0, DCP_NOSERVICE);
							if (DCP_RSP_OK != RbuRespVal)
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				/* Allocate Lower Requestblocks for Client functions
				*/
				RbuRespVal = dcp_ClientRqb(ChPtr);
			}
		}
 	}
	else
	{
		/* currently only Ethernet is supported
		*/
		RbuRespVal = DCP_RSP_ERR_CONFIG;
	}

	if (RbuRespVal == DCP_RSP_OK)
	{
		/* Store Lower OpenChannelRB for channel close
		 * and give upper OpenChannelRB back to user
		 * 220404lrg003: Do not overwrite OpenPu with NULL
		 * (at Join Server- to Client Channel)
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.RbEthPtr, LSA_NULL))
		{
			ChPtr->ChLowPtr->OpenPu = RblPu;
		}
		dcp_RequestDone(ChPtr, LSA_NULL, RbuRespVal);
	}
	else if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.RbEthPtr, LSA_NULL))
	{
		dcp_OpenLowerError(ChPtr, RblPu, RbuRespVal);
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_OpenLowerFin() ChPtr=%X RbEthPtr=%X QueueRecvCnt=%d", 
						ChPtr, RblPu.RbEthPtr, ChPtr->QueueRecvCnt);

	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: dcp_AllocEthSend()                                     */
/* Allocate Lower Send Requstblock with Parameter block and Data block       */
/*****************************************************************************/
LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_AllocEthSend(
DCP_CHA_PTR  			ChPtr,
LSA_UINT8				Tidx,
LSA_UINT8				Uid)
{
	DCP_LOWER_RQB_PTR_TYPE		NewPu;
	DCP_LOWER_MEM_PTR_TYPE		MemPu;
	DCP_LOWER_TXMEM_PTR_TYPE	TxMemPu;
	LSA_USER_ID_TYPE        	UserId;
	DCP_LOWER_QUEUE			*	QueuePtr = LSA_NULL;
	LSA_UINT16 					RetVal = LSA_RET_OK;
	LSA_UINT16 					RbuRespVal = DCP_RSP_OK;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_AllocEthSend() ChPtr=%X Tidx=%d Uid=%d", 
						ChPtr, Tidx, Uid);

	UserId.uvar32 = 0;
	UserId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;
	UserId.uvar8_array[DCP_CLIENT_UID] = Uid;
	UserId.uvar8_array[DCP_RBTYPE_UID] = DCP_LOWER_SEND;
	UserId.uvar8_array[DCP_INDEX_UID] = Tidx;

	/* Choose queue depending on UserId
	*/
	switch (Uid)
	{
	case DCP_HELLO:
		QueuePtr = &ChPtr->Hello.QueueSend;
		break;
	case DCP_IDENTIFY:
		QueuePtr = &ChPtr->Identify.QueueSend;
		break;
	case DCP_SET:
		QueuePtr = &ChPtr->Set.QueueSend;
		break;
	case DCP_GET:
		QueuePtr = &ChPtr->Get.QueueSend;
		break;
	case DCP_NOSERVICE:
		QueuePtr = &ChPtr->QueueLower;
		break;
	default:
		/* Invalid Uid
		 * AP01567955 DCP: Warnings bei Generierung des PnDriver entfernen
		*/
		RbuRespVal = DCP_RSP_ERR_PARAM;
		dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CONFIG, ChPtr, Uid);
	}

	if(RbuRespVal == DCP_RSP_OK)
	{
		/* Allocate Requestblock
		*/
		DCP_ALLOC_LOWER_RQB(&NewPu, UserId, sizeof(DCP_LOWER_RQB_TYPE), ChPtr->SysPtr);

		/* If the lower RB has been delivered synchronously...
		*/
		if(!LSA_HOST_PTR_ARE_EQUAL(NewPu.RbEthPtr, LSA_NULL))
		{
			/* ...put it into the queue and request lower parameter block
			*/
			DCP_ETH_RQB_USERID_UVAR32_SET(NewPu.RbEthPtr, UserId.uvar32);
			DCP_ETH_RQB_PPARAM_SET(NewPu.RbEthPtr, LSA_NULL);
			dcp_EnqueLower(QueuePtr, NewPu, ChPtr);
			DCP_ALLOC_LOWER_MEM(&MemPu, UserId, sizeof(DCP_ETH_PAR_TYPE), ChPtr->SysPtr);

			/* If the lower parameter block has been delivered synchronously...
			*/
			if(!LSA_HOST_PTR_ARE_EQUAL(MemPu.MemEthPtr, LSA_NULL))
			{
				/* ...put pointer to the PB into queued RQB and allocate data block
				*/
				DCP_ETH_RQB_PPARAM_SET(NewPu.RbEthPtr, MemPu.MemEthPtr);
				DCP_ETH_RQB_SEND_DATA_SET(NewPu.RbEthPtr, LSA_NULL, 0);
				DCP_ALLOC_LOWER_TXMEM(&TxMemPu, UserId, DCP_ETH_MEM_MAX_LEN, ChPtr->SysPtr);

				/* If the lower DB has been delivered synchronously...
				*/
				if(!LSA_HOST_PTR_ARE_EQUAL(TxMemPu.TxEthPtr, LSA_NULL))
				{
					/* ...set data pointer of the RQB to allocated data block
					 *  and ser Service to SEND
					*/
					DCP_ETH_RQB_SEND_DATA_SET(NewPu.RbEthPtr, TxMemPu.TxEthPtr, DCP_ETH_MEM_MAX_LEN);
					DCP_ETH_RQB_SERVICE_SET(NewPu.RbEthPtr, DCP_ETH_SEND);
				}
				else
				{
					/* Next step: get RQB asynchronously by dcp_lower_tx_mem()
					 * is Currently not supported!
					*/
					dcp_DequeLower(QueuePtr, NewPu, ChPtr);
					MemPu.MemEthPtr = DCP_ETH_RQB_PPARAM_GET(NewPu.RbEthPtr);
					DCP_FREE_LOWER_MEM(&RetVal, MemPu, ChPtr->SysPtr);
					DCP_FREE_LOWER_RQB(&RetVal, NewPu, ChPtr->SysPtr);
					RbuRespVal = DCP_RSP_ERR_RESOURCE;
				}
			}
			else
			{
				/* Next step: get RQB asynchronously by dcp_lower_mem()
				 * is Currently not supported!
				*/
				dcp_DequeLower(QueuePtr, NewPu, ChPtr);
				DCP_FREE_LOWER_RQB(&RetVal, NewPu, ChPtr->SysPtr);
				RbuRespVal = DCP_RSP_ERR_RESOURCE;
			}
		}
		else
		{
			/* Next step: get RQB asynchronously by dcp_lower_rqb()
			 * is Currently not supported!
			*/
			RbuRespVal = DCP_RSP_ERR_RESOURCE;
		}
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_AllocEthSend() ChPtr=%X RespVal=0x%x", 
						ChPtr, RbuRespVal);
	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal access function: dcp_AllocEthRecv()                              */
/* Allocate Lower Recv Requstblock with parameter block and data block       */
/*****************************************************************************/
LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_AllocEthRecv(
DCP_CHA_PTR  			ChPtr,
LSA_UINT8				Tidx,
LSA_UINT8				Uid,
DCP_ETH_RQB_PTR_TYPE	RbPtr)
{
	DCP_LOWER_RQB_PTR_TYPE		NewPu;
	DCP_LOWER_MEM_PTR_TYPE		MemPu;
	DCP_LOWER_RXMEM_PTR_TYPE	RxMemPu;
	LSA_USER_ID_TYPE        	UserId;
	DCP_LOWER_QUEUE			*	QueuePtr = LSA_NULL;
	LSA_UINT16 					RetVal = LSA_RET_OK;
	LSA_UINT16 					RbuRespVal = DCP_RSP_OK;

	UserId.uvar32 = 0;
	UserId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;
	UserId.uvar8_array[DCP_CLIENT_UID] = Uid;
	UserId.uvar8_array[DCP_RBTYPE_UID] = DCP_LOWER_RECV;
	UserId.uvar8_array[DCP_INDEX_UID] = Tidx;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_AllocEthRecv() ChPtr=%X RbPtr=%X UserId.uvar32=0x%08x", 
						ChPtr, RbPtr, UserId.uvar32);

	NewPu.RbEthPtr = RbPtr;

	/* Destination queue is always the Lower Receive Queue
	*/
	QueuePtr = &(ChPtr->ChLowPtr->QueueRecv);

	if(LSA_HOST_PTR_ARE_EQUAL(NewPu.RbEthPtr, LSA_NULL))
	{
		/* Allocate Requestblock
		*/
		DCP_ALLOC_LOWER_RQB(&NewPu, UserId, sizeof(DCP_LOWER_RQB_TYPE), ChPtr->SysPtr);
		MemPu.MemEthPtr = LSA_NULL;
	}
	else
		/* Get parameter block from given RQB
		*/
		MemPu.MemEthPtr = DCP_ETH_RQB_PPARAM_GET(RbPtr);

	/* If the lower RB already exists or has been delivered synchronously...
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(NewPu.RbEthPtr, LSA_NULL))
	{
		/* Set User ID and queue Requestblock
		*/
		DCP_ETH_RQB_USERID_UVAR32_SET(NewPu.RbEthPtr, UserId.uvar32);
		DCP_ETH_RQB_PPARAM_SET(NewPu.RbEthPtr, LSA_NULL);
		dcp_EnqueLower(QueuePtr, NewPu, ChPtr);
		if (LSA_HOST_PTR_ARE_EQUAL(MemPu.MemEthPtr, LSA_NULL))
		{
			/* ...if no parameter block exists: allocate it
			*/
			DCP_ALLOC_LOWER_MEM(&MemPu, UserId, sizeof(DCP_ETH_PAR_TYPE), ChPtr->SysPtr);
		}

		/* If the lower PB already exists or has been delivered synchronously...
		*/
		if(!LSA_HOST_PTR_ARE_EQUAL(MemPu.MemEthPtr, LSA_NULL))
		{
			/* ...put pointer to the PB into queued RQB 
			* and allocate data block
			*/
			DCP_ETH_RQB_PPARAM_SET(NewPu.RbEthPtr, MemPu.MemEthPtr);
			DCP_ETH_RQB_RECV_DATA_SET(NewPu.RbEthPtr, LSA_NULL, 0);
			DCP_ALLOC_LOWER_RXMEM(&RxMemPu, UserId, DCP_ETH_MEM_MAX_LEN, ChPtr->SysPtr);

			/* If the lower DB has been delivered synchronously...
			*/
			if(!LSA_HOST_PTR_ARE_EQUAL(RxMemPu.RxEthPtr, LSA_NULL))
			{
				/* ...set data pointer of the RQB to allocated data block
				 *  and set Service to RECV
				*/
				DCP_ETH_RQB_RECV_PTR_SET(NewPu.RbEthPtr, RxMemPu.RxEthPtr);
				DCP_ETH_RQB_OPCODE_SET(NewPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
				DCP_ETH_RQB_SERVICE_SET(NewPu.RbEthPtr, DCP_ETH_RECV);
				DCP_ETH_RQB_RECV_LEN_SET(NewPu.RbEthPtr, DCP_ETH_MEM_MAX_LEN);
			}
			else
			{
				/* Next step: get RQB asynchronously by dcp_lower_rx_mem()
				 * is Currently not supported!
				*/
				dcp_DequeLower(QueuePtr, NewPu, ChPtr);
				MemPu.MemEthPtr = DCP_ETH_RQB_PPARAM_GET(NewPu.RbEthPtr);
				DCP_FREE_LOWER_MEM(&RetVal, MemPu, ChPtr->SysPtr);
				DCP_FREE_LOWER_RQB(&RetVal, NewPu, ChPtr->SysPtr);
				RbuRespVal = DCP_RSP_ERR_RESOURCE;
			}
		}
		else
		{
			/* Next step: get RQB asynchronously by dcp_lower_mem()
			 * is Currently not supported!
			*/
			dcp_DequeLower(QueuePtr, NewPu, ChPtr);
			DCP_FREE_LOWER_RQB(&RetVal, NewPu, ChPtr->SysPtr);
			RbuRespVal = DCP_RSP_ERR_RESOURCE;
		}
	}
	else
	{
		/* Next step: get RQB asynchronously by dcp_lower_rqb()
		 * is Currently not supported!
		*/
		RbuRespVal = DCP_RSP_ERR_RESOURCE;
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_AllocEthRecv() ChPtr=%X RespVal=0x%x", 
						ChPtr, RbuRespVal);
	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal access function: dcp_AllocLower()                                */
/* Allocate Lower Recv-/Send-Requstblock with parameter block and data block */
/* or Lower Cancel-Requstblock with parameter block                          */
/*****************************************************************************/
LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_AllocLower(
DCP_CHA_PTR  			ChPtr,
LSA_UINT8				Tidx,
LSA_UINT8				Uid,
LSA_BOOL				SendRb)
{
	LSA_UINT16			RespVal = DCP_RSP_ERR_CONFIG;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_AllocLower() ChPtr=%X Uid=%u SendRb=%u", 
						ChPtr, Uid, SendRb);

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		if (SendRb)
			RespVal = dcp_AllocEthSend(ChPtr, Tidx, Uid);
		else
			RespVal = dcp_AllocEthRecv(ChPtr, Tidx, Uid, LSA_NULL);
	}
	/* else: currently only Ethernet is supported
	*/
	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_AllocLower() ChPtr=%X Tidx=%u RespVal=0x%x", 
						ChPtr, Tidx, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: dcp_OpenLowerError()                                   */
/* Undo open Lower Channel.                                                  */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_OpenLowerError(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu,
LSA_UINT16 				RbuRespVal)
{
	DCP_UPPER_RQB_PTR_TYPE  RbuPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_OpenLowerError() ChPtr=%X RbEthPtr=%X RbuRespVal=0x%x", 
						ChPtr, RblPu.RbEthPtr, RbuRespVal);

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		if (ChPtr->ChLowPtr->State == DCP_CHA_STATE_OPEN)
		{
			/* Close Lower Channel
			*/
			ChPtr->ChLowPtr->State = DCP_CHA_STATE_ERROR;
			dcp_CloseLower(ChPtr, RblPu);
		}
		else
		{
			/* On error:
			 * free Lower RB and 
			 * get first Requstblock from Upper Queue
			*/
			dcp_FreeLowerRb(ChPtr, RblPu);
			RbuPtr = ChPtr->QueueUpper.FirstPtr;
			if (LSA_HOST_PTR_ARE_EQUAL(RbuPtr, LSA_NULL))
			{
				/* No Requestblock to confirm ?!?
				*/
				dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_NO_RBU, LSA_NULL, 0);
			}
			dcp_DequeUpper(RbuPtr);

			/* Clean channel
			 * 240810lrg001: and callback before RELEASE_PATH_INFO() in dcp_ChFree()
			*/ 
			dcp_ChClean(ChPtr);
			dcp_RequestDone(ChPtr, RbuPtr, RbuRespVal);
			dcp_ChFree(ChPtr);
		}
	}
	/* else: currently only Ethernet is supported
	*/
	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_OpenLowerError() ChPtr=%X RbuPtr=%X", 
						ChPtr, RbuPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_CancelLower()                               */
/* If CancelAll is TRUE, the lower Channel is prepared for being closed by   */
/* sending of CANCEL requests. Else end of a client request at timeout.      */
/*****************************************************************************/
LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_CancelLower(
DCP_CHA_PTR  			ChPtr,
LSA_BOOL				CancelAll,
DCP_TIMER_PTR_TYPE		TimerPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{
	LSA_USER_ID_TYPE	UserId;
	DCP_LOWER_TXMEM_PTR_TYPE	TxPu;
	LSA_BOOL			SendRbUsed	= LSA_FALSE;
	LSA_UINT16 			RetVal		= LSA_RET_OK;
	LSA_UINT16 			RespVal 	= DCP_RSP_OK_ACTIVE;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_CancelLower() ChPtr=%X CancelAll=%u TimerPtr=%X", 
						ChPtr, CancelAll, TimerPtr);

	UserId.uvar32 = 0;
	UserId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;

	if (CancelAll)
	{
		/* Use Open Channel RB, if stored
		*/
		RblPu = ChPtr->ChLowPtr->OpenPu;
		if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.VoidPtr, LSA_NULL))
		{
			ChPtr->ChLowPtr->OpenPu.VoidPtr = LSA_NULL;
		}
		else if (ChPtr->QueueLower.Count != 0)
		{
			/* Use Lower Send Requestblock
			*/
			SendRbUsed = LSA_TRUE;
			RblPu = ChPtr->QueueLower.FirstPu;
			dcp_DequeLower(&ChPtr->QueueLower, RblPu, ChPtr);
		}
	}
	else
	{
		/* Send RB given as parameter
		*/
		SendRbUsed = LSA_TRUE;
	}
	
	if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.VoidPtr, LSA_NULL))
	{
		if (CancelAll)
		{
			ChPtr->ChLowPtr->State = DCP_CHA_STATE_CLOSING;
		}
		if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
		||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
		{
			if (SendRbUsed)
			{
				/* Free tx memory
				*/
				TxPu.TxEthPtr = DCP_ETH_RQB_SEND_PTR_GET(RblPu.RbEthPtr);
				if (CancelAll)
				{
					DCP_FREE_LOWER_TXMEM(&RetVal, TxPu, ChPtr->SysPtr);
					if (LSA_RET_OK != RetVal)
					{
				    	dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_FREE_LOWER_MEM, TxPu.TxEthPtr, 0);
					}
				}
				else if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
				{
					/* Store send buffer and write TimerId to UserId
					*/
					UserId = DCP_ETH_RQB_USERID_GET(RblPu.RbEthPtr);
					UserId.uvar8_array[DCP_RBTYPE_UID] = DCP_LOWER_RECV;
					UserId.uvar8_array[DCP_INDEX_UID] = TimerPtr->Id;
					UserId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;
					TimerPtr->Tract.Txs.TxPu = TxPu;
				}
			}

			/* Setup CANCEL Request
			*/
			DCP_ETH_RQB_USERID_UVAR32_SET(RblPu.RbEthPtr, UserId.uvar32);
			DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);
			DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
			DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
			DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, DCP_ETH_CANCEL);
			DCP_ETH_RQB_CANCEL_ID_SET(RblPu.RbEthPtr, UserId.uvar32);
			if (CancelAll)
			{
				DCP_ETH_RQB_CANCEL_MODE_SET(RblPu.RbEthPtr, DCP_ETH_CANCEL_MODE_ALL);
			}
			else
			{
				DCP_ETH_RQB_CANCEL_MODE_SET(RblPu.RbEthPtr, DCP_ETH_CANCEL_MODE_UID);
			}

			/* Send CANCEL Request
			*/
			DCP_REQUEST_LOWER(RblPu, ChPtr->SysPtr);
		}
		else
		{
			/* currently only Ethernet is supported
			*/
			if (CancelAll)
			{
				dcp_FreeLowerRb(ChPtr, RblPu);
			}
			DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_CancelLower() ChPtr=%X DCP_ERR_DEVICE=0x%x", 
							ChPtr, DCP_ERR_DEVICE);
		}
	}
	else
	{
		RespVal = DCP_RSP_ERR_RESOURCE;
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_CancelLower() ChPtr=%X RespVal=0x%x", 
						ChPtr, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal function: dcp_CloseLowerSet()                                    */
/* 111004lrg001: disable multicast address.                                  */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_CloseLowerSet(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{
	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_CloseLowerSet() ChPtr=%X RbEthPtr=%X Opcode=%d", 
					ChPtr, RblPu.RbEthPtr, DCP_ETH_RQB_OPCODE_GET(RblPu.RbEthPtr));

		if(DCP_DEV_FSU != ChPtr->DetailPtr->DevId)
		{
			/* Disable IDENTIFY Multicast address
			*/
 			DCP_MAC_ADR IdentifyAddr = DCP_IDENTIFY_MC_ADDR;
 			DCP_ETH_RQB_MULTICAST_DISABLE(RblPu.RbEthPtr, IdentifyAddr);
		}
		else
		{
			/* Disable HELLO Multicast address
			*/
 			DCP_MAC_ADR HelloAddr = DCP_HELLO_MC_ADDR;
 			DCP_ETH_RQB_MULTICAST_DISABLE(RblPu.RbEthPtr, HelloAddr);
		}
		
		/* Fill Lower Requstblock for REQUEST, Service: Multicast disable
		*/
		DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
		DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
		DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, DCP_ETH_MULTICAST);
		DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);

		/* Send Lower Request
		*/
		DCP_REQUEST_LOWER(RblPu, ChPtr->SysPtr);

		/* Next Step: dcp_request_lower_eth_done() and dcp_CloseLower()
		*/
	}
	else
	{
		/* currently only Ethernet is supported
		*/
		dcp_FreeLowerRb(ChPtr, RblPu);
		DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_CloseLowerSet() ChPtr=%X DCP_ERR_DEVICE=0x%x", 
						ChPtr, DCP_ERR_DEVICE);
	}

	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_CloseLowerSet() ChPtr=%X", 
						ChPtr);
}

/*****************************************************************************/
/* Internal function: dcp_CloseLower()                                       */
/* Close Lower Channel.                                                      */
/*****************************************************************************/
LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_CloseLower(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{
	LSA_UINT16 RetVal = DCP_RSP_OK_ACTIVE;

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_CloseLower() ChPtr=%X RbEthPtr=%X Opcode=%d", 
					ChPtr, RblPu.RbEthPtr, DCP_ETH_RQB_OPCODE_GET(RblPu.RbEthPtr));

		/* Setup CLOSE Request
		*/
		DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_CLOSE_CHANNEL);
		DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, 0);
		DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
		DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);

		/* Send CLOSE Request
		*/
		DCP_CLOSE_CHANNEL_LOWER(RblPu, ChPtr->SysPtr);
	}
	else
	{
		/* currently only Ethernet is supported
		*/
		dcp_FreeLowerRb(ChPtr, RblPu);
		DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_CloseLower() ChPtr=%X DCP_ERR_DEVICE=0x%x", 
							ChPtr, DCP_ERR_DEVICE);
	}
	
	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_CloseLower() ChPtr=%X", 
						ChPtr);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: dcp_CloseLowerFin()                                    */
/* Lower Channel is closed: clean up                                         */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_CloseLowerFin(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{
	LSA_UINT16 RetVal = LSA_RET_OK;
	LSA_UINT16 RespVal = DCP_RSP_OK;
	LSA_BOOL   ChRemove = LSA_TRUE;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_CloseLowerFin() ChPtr=%X RbEthPtr=%X ChLowState=0x%x", 
					ChPtr, RblPu.RbEthPtr, ChPtr->ChLowPtr->State);

	if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.VoidPtr, LSA_NULL))
	{
		if (DCP_CHA_STATE_ERROR == ChPtr->ChLowPtr->State)
		{
			/* Error on dcp_open_channel()
			*/
			RespVal = DCP_RSP_ERR_LOWER_LAYER;
		}
		else
		{
			RespVal = DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr);
			if (DCP_ETH_RSP_OK != RespVal)
			{
				RespVal = DCP_RSP_ERR_LOWER_LAYER;
				ChRemove = LSA_FALSE;
			}
		}
	}

	if (ChRemove)
	{
		/* Free Lower Requestblock and clean up channel structure
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.VoidPtr, LSA_NULL))
		{
			dcp_FreeLowerRb(ChPtr, RblPu);
		}
		dcp_ChClean(ChPtr);
	}
	else
	{
		/* Save Lower RB for close_channel() and reset state
		*/ 
		ChPtr->ChLowPtr->OpenPu = RblPu;
		ChPtr->State = DCP_CHA_STATE_OPEN;
	}

	/* Confirm actual upper RB to User
	*/
	dcp_RequestDone(ChPtr, LSA_NULL, RespVal);

	if (ChRemove)
	{
		/* Free channel structure
		*/
		dcp_ChFree(ChPtr);
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_CloseLowerFin() ChPtr=%X RespVal=0x%x RetVal=0x%x", 
						ChPtr, RespVal, RetVal);
	LSA_UNUSED_ARG(RetVal);
}

/*****************************************************************************/
/* Internal access function: dcp_DataSend()                                  */
/* Send DCP data block.                                                      */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_DataSend(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE  RblPu,
LSA_INT					SendLen)
{
	if (ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	{
		DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_DataSend() ChPtr=%X RbEthPtr=%X SendLen=%d", 
							ChPtr, RblPu.RbEthPtr, SendLen);

		ChPtr->SendReqCount++;	//051207lrg001

		DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
		DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
		DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, DCP_ETH_SEND);
		DCP_ETH_RQB_SEND_LEN_SET(RblPu.RbEthPtr, SendLen);
		DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);

		DCP_SNDRCV_TRACE_BYTE_ARRAY(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<== dcp send data:", DCP_ETH_RQB_SEND_PTR_GET(RblPu.RbEthPtr), SendLen);
		DCP_REQUEST_LOWER(RblPu, ChPtr->SysPtr);
	}
	else
		/* currently only Ethernet is supported
		*/
		DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_DataSend() ChPtr=%X SendLen=%d DCP_ERR_DEVICE=0x%x", 
						ChPtr, SendLen, DCP_ERR_DEVICE);

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_DataSend() ChPtr=%X SendReqCount=%u", 
						ChPtr, ChPtr->SendReqCount);
}

/*****************************************************************************/
/* Internal access function: dcp_DataRecv()                                  */
/* Put receive request to Lower Layer.                                       */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_DataRecv(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE  RblPu,
LSA_UINT8				Tidx,
LSA_UINT8				Uid)
{
	LSA_USER_ID_TYPE UserId;

	UserId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;
	UserId.uvar8_array[DCP_CLIENT_UID] = Uid;
	UserId.uvar8_array[DCP_RBTYPE_UID] = DCP_LOWER_RECV;
	UserId.uvar8_array[DCP_INDEX_UID] = Tidx;

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_DataRecv() ChPtr=%X RbEthPtr=%X UserId.uvar32=0x%x", 
							ChPtr, RblPu.RbEthPtr, UserId.uvar32);

		DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
		DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
		DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, DCP_ETH_RECV);
		DCP_ETH_RQB_RECV_LEN_SET(RblPu.RbEthPtr, DCP_ETH_MEM_MAX_LEN);
		DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);
		DCP_ETH_RQB_USERID_UVAR32_SET(RblPu.RbEthPtr, UserId.uvar32);
		DCP_ETH_RQB_RECV_ID_SET(RblPu.RbEthPtr, UserId.uvar32);

		DCP_REQUEST_LOWER(RblPu, ChPtr->SysPtr);
		ChPtr->ChLowPtr->RecvRblAct++;

		/* 120105lrg001: increment Client Recv-RQB count
		*/
		if (Uid != 0)
			ChPtr->ChLowPtr->RecvRblClient++;
	}
	else
		/* currently only Ethernet is supported
		*/
		DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_DataRecv() ChPtr=%X DCP_ERR_DEVICE=0x%x", 
						ChPtr, DCP_ERR_DEVICE);

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_DataRecv() ChPtr=%X RecvRblClient=%d RecvRblAct=%d", 
						ChPtr, ChPtr->ChLowPtr->RecvRblClient, ChPtr->ChLowPtr->RecvRblAct);
}

/*****************************************************************************/
/* Internal access function: dcp_CancelRecv()                                */
/* Send CANCEL Request for lower receive RQBs.                               */
/*****************************************************************************/
LSA_UINT16 DCP_LOCAL_FCT_ATTR dcp_CancelRecv(
DCP_CHA_PTR  			ChPtr,
LSA_UINT8				Tidx,
LSA_UINT8				Uid)
{
	DCP_LOWER_RQB_PTR_TYPE	RblPu;
	LSA_USER_ID_TYPE		CancelId;
	LSA_UINT16 				RespVal = DCP_RSP_OK_ACTIVE;

	CancelId.uvar8_array[DCP_HANDLE_UID] = ChPtr->Handle;
	CancelId.uvar8_array[DCP_CLIENT_UID] = Uid;
	CancelId.uvar8_array[DCP_RBTYPE_UID] = DCP_LOWER_RECV;
	CancelId.uvar8_array[DCP_INDEX_UID] = Tidx;

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_CancelRecv() ChPtr=%X CancelId=0x%x", 
						ChPtr, CancelId.uvar32);

	/* Use Open Channel RQB for Cancel Request
	*/
	RblPu = ChPtr->ChLowPtr->OpenPu;
	if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.VoidPtr, LSA_NULL))
	{
		ChPtr->ChLowPtr->OpenPu.VoidPtr = LSA_NULL;
		if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
		||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
		{
			/* Setup CANCEL Request
			*/
			DCP_ETH_RQB_ERR_COMPID_SET(RblPu.RbEthPtr, LSA_COMP_ID_UNUSED);
			DCP_ETH_RQB_OPCODE_SET(RblPu.RbEthPtr, DCP_ETH_OPC_REQUEST);
			DCP_ETH_RQB_HANDLE_SET(RblPu.RbEthPtr, ChPtr->ChLowPtr->HandleLower);
			DCP_ETH_RQB_SERVICE_SET(RblPu.RbEthPtr, DCP_ETH_CANCEL);
			DCP_ETH_RQB_USERID_UVAR32_SET(RblPu.RbEthPtr, CancelId.uvar32);
			DCP_ETH_RQB_CANCEL_ID_SET(RblPu.RbEthPtr, CancelId.uvar32);
			DCP_ETH_RQB_CANCEL_MODE_SET(RblPu.RbEthPtr, DCP_ETH_CANCEL_MODE_UID);

			/* Send CANCEL Request
			*/
			DCP_REQUEST_LOWER(RblPu, ChPtr->SysPtr);
		}
		else
		{
			/* currently only Ethernet is supported
			*/
			DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_CancelRecv() ChPtr=%X DCP_ERR_DEVICE=0x%x", 
							ChPtr, DCP_ERR_DEVICE);
		}
	}
	else
		RespVal = DCP_RSP_ERR_RESOURCE;

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_CancelRecv() ChPtr=%X RespVal=0x%x", 
						ChPtr, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: dcp_FreeLowerRb()                               */
/* Free Requestblock, Parameterblock and data buffer.                        */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_FreeLowerRb(
DCP_CHA_PTR  			ChPtr,
DCP_LOWER_RQB_PTR_TYPE  RblPu)
{
	DCP_LOWER_MEM_PTR_TYPE		MemPu;
	DCP_LOWER_TXMEM_PTR_TYPE	TxPu;
	DCP_LOWER_RXMEM_PTR_TYPE	RxPu;
	LSA_UINT16 					RetVal = LSA_RET_OK;

	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_FreeLowerRb() ChPtr=%X RbEthPtr=%X Service=0x%x", 
							ChPtr, RblPu.RbEthPtr, DCP_ETH_RQB_SERVICE_GET(RblPu.RbEthPtr));

		MemPu.MemEthPtr = DCP_ETH_RQB_PPARAM_GET(RblPu.RbEthPtr);
		if(!LSA_HOST_PTR_ARE_EQUAL(MemPu.MemEthPtr, LSA_NULL))
		{
			if (DCP_ETH_RECV == DCP_ETH_RQB_SERVICE_GET(RblPu.RbEthPtr))
			{
				/* Free Receive Memory
				*/
				RxPu.RxEthPtr = DCP_ETH_RQB_RECV_PTR_GET(RblPu.RbEthPtr);
				if(!LSA_HOST_PTR_ARE_EQUAL(RxPu.RxEthPtr, LSA_NULL))
				{
					DCP_FREE_LOWER_RXMEM(&RetVal, RxPu, ChPtr->SysPtr);
					if (LSA_RET_OK != RetVal)
					    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_FREE_LOWER_MEM, RxPu.RxEthPtr, 0);
				}
			}
			else if (DCP_ETH_SEND == DCP_ETH_RQB_SERVICE_GET(RblPu.RbEthPtr))
			{
				/* Free Transmit Memory
				*/
				TxPu.TxEthPtr = DCP_ETH_RQB_SEND_PTR_GET(RblPu.RbEthPtr);
				if(!LSA_HOST_PTR_ARE_EQUAL(TxPu.TxEthPtr, LSA_NULL))
				{
					DCP_FREE_LOWER_TXMEM(&RetVal, TxPu, ChPtr->SysPtr);
					if (LSA_RET_OK != RetVal)
					    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_FREE_LOWER_MEM, TxPu.TxEthPtr, 0);
				}
			}
			/* Free Parameter Memory
			*/
			DCP_FREE_LOWER_MEM(&RetVal, MemPu, ChPtr->SysPtr);
			if (LSA_RET_OK != RetVal)
			    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_FREE_LOWER_MEM, MemPu.MemEthPtr, 0);
		}

		DCP_FREE_LOWER_RQB(&RetVal, RblPu, ChPtr->SysPtr);
		if (LSA_RET_OK != RetVal)
		    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_FREE_LOWER_RQB, RblPu.RbEthPtr, 0);
	}
	else
		/* currently only Ethernet is supported
		*/
		DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_FreeLowerRb() ChPtr=%X DCP_ERR_DEVICE=0x%x", 
						ChPtr, DCP_ERR_DEVICE);

	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_FreeLowerRb() ChPtr=%X", 
						ChPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_LowerRqbComplete()                          */
/* Check for complete allocation of a Requestblock.                          */
/*****************************************************************************/
LSA_BOOL	DCP_LOCAL_FCT_ATTR dcp_LowerRqbComplete(
DCP_CHA_PTR				ChPtr, 
DCP_LOWER_RQB_PTR_TYPE	RblPu, 
LSA_UINT8				Typ)
{
	LSA_BOOL Complete = LSA_TRUE;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_LowerRqbComplete() ChPtr=%X RbEthPtr=%X Typ=0x%x", 
						ChPtr, RblPu.RbEthPtr, Typ);

	/* Only used on asynchronous allocation of RQB
	*/

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_LowerRqbComplete() ChPtr=%X RbEthPtr=%X Complete=%u", 
						ChPtr, RblPu.RbEthPtr, Complete);

	LSA_UNUSED_ARG(ChPtr);
	LSA_UNUSED_ARG(RblPu);
	LSA_UNUSED_ARG(Typ);

	return(Complete);
}

/*****************************************************************************/
/* External LSA access function: dcp_lower_rqb()                             */
/* Get Lower Requestblock asynchronously.                                    */
/*****************************************************************************/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_rqb(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
DCP_LOWER_RQB_PTR_TYPE   RblPtr)
{
	DCP_CHA_PTR ChPtr = dcp_ChGet(UserId.uvar8_array[DCP_HANDLE_UID]);

	/* currently not supported
	*/
    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_ASYNC_PTR, ChPtr, Len);

	LSA_UNUSED_ARG(Len);
	LSA_UNUSED_ARG(RblPtr);
}

/*****************************************************************************/
/* External LSA access function: dcp_lower_mem()                             */
/* Get Lower parameter block asynchronously.                                 */
/*****************************************************************************/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_mem(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
DCP_LOWER_MEM_PTR_TYPE   DataPtr)
{
	DCP_CHA_PTR ChPtr = dcp_ChGet(UserId.uvar8_array[DCP_HANDLE_UID]);

	/* currently not supported
	*/
    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_ASYNC_PTR, ChPtr, Len);

	LSA_UNUSED_ARG(Len);
	LSA_UNUSED_ARG(DataPtr);
}

/*****************************************************************************/
/* External LSA access function: dcp_lower_tx_mem()                          */
/* Get Lower send data block asynchronously.                                 */
/*****************************************************************************/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_tx_mem(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
DCP_LOWER_TXMEM_PTR_TYPE DataPtr)
{
	DCP_CHA_PTR ChPtr = dcp_ChGet(UserId.uvar8_array[DCP_HANDLE_UID]);

	/* currently not supported
	*/
    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_ASYNC_PTR, ChPtr, Len);

	LSA_UNUSED_ARG(Len);
	LSA_UNUSED_ARG(DataPtr);
}

/*****************************************************************************/
/* External LSA access function: dcp_lower_rx_mem()                          */
/* Get Lower recv data block asynchronously.                                 */
/*****************************************************************************/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_rx_mem(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
DCP_LOWER_RXMEM_PTR_TYPE DataPtr)
{
	DCP_CHA_PTR ChPtr = dcp_ChGet(UserId.uvar8_array[DCP_HANDLE_UID]);

	/* currently not supported
	*/
    dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_ASYNC_PTR, ChPtr, Len);

	LSA_UNUSED_ARG(Len);
	LSA_UNUSED_ARG(DataPtr);
}

/*-----------------------------------------------------------------------------
* EOF dcp_low.c
*/
