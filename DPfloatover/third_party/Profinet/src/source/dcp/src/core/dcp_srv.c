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
/*  F i l e               &F: dcp_srv.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Server functions of dcp protocol                                         */
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
/*  2002-11-06  P01.00.00.00_00.02.00.00  lrg  2.Increment:Get/Set Response  */
/*  2003-02-03  P01.00.00.00_00.03.00.00  lrg  3.Increment:                  */
/*              Call DCP_TRACE_SET_RSP()                                     */
/*              Call DCP_WRITE_TCPIP_STATUS(DCP_SPH_TCPIP_STATUS) in PT2_MODE*/
/*  2003-03-21  P01.00.00.00_00.03.00.01  lrg                                */
/*              dcp_RspHdrSet() ServiceType |= DCP_ROLE_RSP -> =             */
/*  2003-04-10  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*              Datensatzfehler bei GET: nicht mehr Header schicken (PT2)    */
/*              PBC-Erweiterungen: Database 128, DCP_STATION_TYPE_CHECK()    */
/*              Bei SET falsche SyntaxID: DCP_RESULT_DS_NOT_SET              */
/*  2003-05-05  P01.00.00.00_00.05.00.00  lrg  5. Increment: SHOW_LOCATION   */
/*              neue Funktion: dcp_ShowLocation()                            */
/*  2003-05-16  Neuer Identify mit Faltungsfaktor wird unterstützt           */
/*              Filter "Stationsname" im Identify-Request                    */
/*              Unbekannter Service wird mit Bit2 im Servicetype quittiert   */
/*  2003-05-26  P01.00.00.00_00.06.00.00  lrg  6.Increment: Database 8       */
/*              wird unterstützt.                                            */
/*  2003-06-02  P01.00.00.00_00.06.00.01  lrg  6.Increment: Identify mit     */
/*              mehreren Filtern gleichzeitig ist möglich.                   */
/*              Filter 04 DetailedTypeOfStation wird unterstützt             */
/*              Rückgabe des TCP-Datensatzes in der Identify.Responese.      */
/*              Vor Lesen der TCP-Datensatzwerte wird der Status mit         */
/*              DCP_READ_TCPIP_STATUS() abgefragt. Ist dieser = 0, werden    */
/*              Default-Werte in der Get/Identify-Response gesendet.         */
/*  2003-06-16  P01.00.00.00_00.06.00.02  lrg  6.Increment:                  */
/*              dcp_SetDatasetIdent() init. DsDataLen wg. Compilerwarnung    */
/*  2003-07-18  P01.00.00.00_00.07.00.00  lrg  7.Increment:                  */
/*              DCP_READ_PROFINET_ROLE(): Database 8, Dataset 4 unterstuetzt */
/*  2003-07-23  P01.00.00.00_00.07.02.00  PROFINET_ROLE immer senden (0)     */
/*              Neuer  Filter/Identblock DCP_IDENT_DS_SEQ                    */
/*  2003-07-30  P01.00.00.00_00.08.00.00  lrg  8.Increment: Client channel   */
/*  2003-08-26  P01.00.00.00_00.09.00.00  lrg  9.Increment: Profibus-        */
/*              Datenbasis 0001 unterstuetzen                                */
/*              Fehlerbehebung in dcp_GetDatasetPbc(): DataLen nur bei       */
/*              Result = DCP_RESULT_OK setzen.                               */
/*  2003-09-01  P01.00.00.00_00.10.00.00  lrg 10.Increment: DHCP-            */
/*              Datensatz 03 unterstuetzen                                   */
/*  2003-09-29  P01.00.00.00_00.11.00.00  lrg 11.Increment: Mehrere IDENTIFY */
/*              gleichzeitig durch Timer-Array. Filter "Sequence Number" neu */
/*  2003-10-10  P01.00.00.00_00.11.00.01  lrg Fehlerbehebung:                */
/*              dcp_IdentifyRspSet(): LSA_INT NameLen fuer                   */
/*              den Rueckgabewert von DCP_STATION_NAME_GET() benutzen        */
/*  2003-10-23  P02.00.00.00_00.01.00.00  lrg  1.Increment Version 2: 		 */
/*              Neue DCP-Header mit Xid und Length						     */
/*  2003-12-23  P02.00.00.00_00.01.01.00  lrg  Kleinbuchstaben vergleichen   */
/*              bei Identify NameOfStation-Filter.						     */
/*  2003-02-09  P02.00.00.00_00.02.00.00  lrg  sizeof(DCP_...)->DCP_..._SIZE */
/*  2003-02-11  P02.00.00.00_00.02.00.01  lrg PBC-Database-Erweiterung       */
/*              Modulfunktionen nur im DCP_SERVER_MODE vorhanden		     */
/*  2003-02-13  P02.00.00.00_00.02.00.02  lrg sizeof(dataset)->dataset_SIZE  */
/*  2004-02-23  P02.00.00.00_00.02.00.03  lrg  dcp_CheckReq()                */
/*              Frame-ID ueberprüfen und DataOffset beruecksichtigen         */
/*  2004-03-03  P02.00.00.00_00.02.00.04  lrg  #ifdef DCP_REQ_LOCAL_DATA     */
/*              Response -> dcp_Response alternativ im Datenseg. statt Stack */
/*              dcp_SetOptPbc() Laengenpruefung bei DCP_DSID_PBCSWITCH       */
/*              korrigiert (= -> ==)                                         */
/*  2004-03-09  P02.00.00.00_00.02.00.05  lrg  Tricore-Compiler-Warning  wg. */
/*              fehlendem break im Switch entfernt (kein Fehler, weil im     */
/*              folgenden default: nur ein break; war).                      */
/*  2004-03-12  P02.00.00.00_00.02.00.06  lrg  dcp_GetOptPbus() Laenge 0+2   */
/*              Datensatz existent, aber nicht initialisiert (120304lrg001)  */
/*              dcp_IdentifyRspSet() TypeOfStation mitliefern (120304lrg002) */
/*              dcp_SetOptControl()                                          */
/*              DCP_RESULT_NO_ACTION -> DCP_RESULT_DS_NOT_SET                */
/*  2004-03-22  P02.00.00.00_00.02.00.07  lrg  dcp_Set() RetVal1 fuer        */
/*              DCP_LOCK_END(), Status im Result mit DCP_SWAP16() schreiben  */
/*  2004-04-20  P02.00.00.00_00.02.05.02  lrg dcp_ServerInd(): Pruefung des  */ 
/*              Empfangsframes auf max. Länge vor Kopie in lokale Struktur   */
/*  2004-04-20  P02.00.00.00_00.03.00.00  lrg dcp_ServerRequest()            */ 
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg  Version 3: dcp_ServerClose()  */
/*  2004-10-01  P03.02.00.00_00.02.00.00  lrg  011004lrg001: support for     */
/*              more than 1 primary IDENTIFY filter ( e.g. Alias names)      */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  Copyright-Text                */
/*  2005-01-12  P03.03.00.00_00.03.02.00  lrg  120105lrg001: New parameter   */
/*              RequestCount for dcp_ServerInd()                             */
/*  2005-01-14  P03.03.00.00_00.03.02.01  lrg  140105lrg001: Do not change   */
/*              user ID of lower RQB at dcp_ServerInd() ->dcp_DataRecv()     */
/*  2005-02-16  P03.04.00.00_00.04.00.00  lrg  LSA-Trace eingebaut           */
/*              LSA_UNUSED_ARG() fuer Parameter, die ohne Trace unused sind  */
/*  2005-03-01  P03.04.00.00_00.04.00.01  lrg  dcp_Get()/DsetLen entfernt    */
/*  2005-04-15  P03.05.00.00_00.04.00.02  lrg  dcp_ServerConf() RbPtr init.  */
/*  2005-10-18  P03.07.00.00_00.04.00.03  071005lrg001: dcp_Identify():      */
/*              Ueberprüfen, ob ein Timer mit gleicher Transaction-ID und    */
/*              Source-Adresse belegt ist. 071005lrg002: dcp_SendRspReq():   */
/*              Ueberpruefen, ob schon ein lower RQB am Timer haengt.        */
/*  2006-06-30  P03.12.00.00_00.01.01.02  lrg  LSA_GLOB 2.1.0.0_0.7.2.1      */
/*  2006-11-24  P03.12.00.00_00.30.01.02  lrg  ArtsRQ AP00372097             */
/*              DCP_ETH_RQB_COMPID_GET replaced by DCP_ETH_RQB_ERR_GET       */
/*  2007-03-16  P04.00.00.00_00.01.01.02  lrg  Hello und Ressorcensteuerung  */
/*  2007-03-23  P04.00.00.00_00.01.01.03  lrg  ArtsRQ AP00430097 GET/SET-    */
/*              Dst-MAC-Addr pruefen: DCP_CMP_MAC_ADDR(HdrPtr->DstMacAdr,... */
/*  2007-12-05  P04.00.00.00_00.60.01.02 051207lrg001: ArtsRQ AP00552010     */
/*              Beim CloseChannel auf Sendeconfirmations warten.             */
/*  2008-04-18  P04.00.00.00_00.80.01.03 AP00609232: Trace modifiziert       */
/*              180408lrg001: Nachdem die RQBs in die Dienst-Queues verteilt */
/*              sind: auch in der gemeinsamen QueueInd nachschauen           */
/*              180408lrg002: Nur dienstspezifische RQBs verwenden           */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2009-01-27  P04.01.00.00_00.02.01.02 270109lrg001: Casts for C++,        */
/*  2010-11-23  P05.00.00.00_00.02.01.02 231110lrg001: AP00988516            */
/*              DCP-Identify-Filter on confirmation                          */
/*  2011-01-18  P05.00.00.00_00.03.12.02 lrg: AP01017495 Index-Trace         */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: AP01205450:              */
/*              Struktur err aus RQB entfernen                               */
/*  2011-08-25  S05.01.00.00_00.04.03.02 AP00696784: Comments in english     */
/*  2012-01-11  T05.01.00.00_00.05.13.01                                     */
/*              AP01297962: remove DCP_CLIENT_MODE and DCP_SERVER_MODE       */
/*  2012-02-09  T05.01.00.00_00.05.25.01 AP01313139: dcp_Response removed    */
/*  2012-03-08  S05.02.00.00_00.01.16.01 AP01138373: DCP_OPC_FILTER_SET      */
/*              Entfernen des DCP Identify Filter Service in DCP             */
/*  2013-05-16  P05.03.00.00_00.01.10.01 AP01509782 DCP channel details und  */
/*              Konfigurationsschalter bereinigen                            */
/*  2013-06-17  P05.03.00.00_00.01.10.01 AP01387412 DCP Timer Optimierung    */
/*  2013-07-12  P05.03.00.00_00.01.10.01 AP01509782 DCP: Verhalten bzgl.     */
/*              TransactionEnd an Norm anpassen.                             */
/*  2013-09-11  P05.03.00.00_00.02.13.01 Request 740218 - Es werden im       */
/*              AllocOnRequest-Mode pro Client-Identify zu viele Recv-RQBs   */
/*              allokiert                                                    */
/*  2014-06-25  P05.03.00.00_00.03.09.01 1064327: Kein FATAL beim Restart    */
/*              des ClientHold Timers (SET-Response) wenn schon abgelaufen   */
/*  2014-08-25  P05.03.00.00_00.03.18.01 1104556: Undo ServerSetWait (WACK)  */
/*  2014-09-10  P05.03.00.00_00.03.18.01 1102112: check and restore client   */
/*              MAC address on SET response                                  */
/*  2014-11-25  P05.03.00.00_00.03.24.01 1157678: DCPBlockLength ist größer  */
/*              als die Frame-Länge                                          */
/*  2015-06-25  P05.04.00.00_00.02.00.36 1364185: dcp_DequeLower fehlt in    */
/*              dcp_SendRspReq                                               */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID	4		/* DCP_MODULE_ID_SRV */
#define DCP_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "dcp_inc.h"
#include "dcp_int.h"
DCP_FILE_SYSTEM_EXTENSION(DCP_MODULE_ID)

/*****************************************************************************/
/* Local data                                                                */
/* ...                                                                       */
/*****************************************************************************/
#ifdef DCP_REQ_LOCAL_DATA
/* Structures for a copy of a DCP Request and building of a DCP Response: 
 * --> else located on stack
*/ 
DCP_ETH_FRAME		dcp_LocalFrame;
//DCP_RSP_DATA		dcp_Response;
#endif

/*****************************************************************************/
/* Internal access function: dcp_ServerRequest()                             */
/* Process a server request from user                                        */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_ServerRequest(
DCP_CHA_PTR				ChPtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	LSA_UINT16		 	RespVal = DCP_RSP_OK_ACTIVE;
	LSA_OPCODE_TYPE		Opcode = DCP_UPPER_RQB_OPCODE_GET(RbuPtr);

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ServerRequest() ChPtr=%X RbuPtr=%X Opcode=%d", 
			ChPtr, RbuPtr, Opcode);

	switch(Opcode)
	{
	case DCP_OPC_RECV_IND:
		RespVal = dcp_RecvIndReq(ChPtr, RbuPtr);
		break;

	case DCP_OPC_SEND_RSP:
		RespVal = dcp_SendRspReq(ChPtr, RbuPtr);
		break;

	case DCP_OPC_CANCEL_RECV:
		RespVal = dcp_CancelRecvReq(ChPtr, RbuPtr);
		break;

	default:
		RespVal = DCP_RSP_ERR_SYNTAX;
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ServerRequest() ChPtr=%X RespVal=0x%x", 
			ChPtr, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: dcp_HelloIndReq()                                      */
/* Process a user Hello Indication Request                                   */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_HelloIndReq(
DCP_CHA_PTR				ChPtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	DCP_LOWER_RQB_PTR_TYPE  RblPu;
	LSA_UINT16		 		RespVal = DCP_RSP_OK;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_HelloIndReq() ChPtr=%X ChNextPtr=%X RbuPtr=%X", 
			ChPtr, ChPtr->ChNextPtr, RbuPtr);

	if (LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChNextPtr, LSA_NULL))
	{
		/* No FSU channel
		*/
		RespVal = DCP_RSP_ERR_CONFIG;
		DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_HelloIndReq() ChPtr=%X ChNextPtr=%X RbuPtr=%X No FSU channel", 
						ChPtr, ChPtr->ChNextPtr, RbuPtr);
	}
	else if (ChPtr->ChNextPtr->DetailPtr->AllocOnRequest)
	{
		/* Allocate Lower Receive RQB and queue it to the Lower RecvQueue of the FSU Channel
		*/
		RespVal = dcp_AllocEthRecv(ChPtr->ChNextPtr, 0, DCP_NOSERVICE, LSA_NULL);
	}
	
	if (RespVal == DCP_RSP_OK)
	{
		if (0 != ChPtr->ChNextPtr->ChLowPtr->QueueRecv.Count)
		{
			/* Move RQB from Upper Channel Queue to Indication Queue
			*/
			RespVal = DCP_RSP_OK_ACTIVE;
			dcp_DequeUpper(RbuPtr);
			dcp_PutUpperQ(&ChPtr->QueueHello, RbuPtr);

			/* Remove Receive RQB from Queue and send it to the lower layer
			*/
			RblPu = ChPtr->ChNextPtr->ChLowPtr->QueueRecv.FirstPu;
			dcp_DequeLower(&ChPtr->ChNextPtr->ChLowPtr->QueueRecv, RblPu, ChPtr->ChNextPtr);
			dcp_DataRecv(ChPtr->ChNextPtr, RblPu, 0, 0);
		}
		else
		{
			/* No Lower Receive RQB
			*/
			RespVal = DCP_RSP_ERR_RESOURCE;
			DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_HelloIndReq() ChPtr=%X ChNextPtr=%X RbuPtr=%X No Lower Recv RQB", 
							ChPtr, ChPtr->ChNextPtr, RbuPtr);
		}
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_HelloIndReq() ChPtr=%X QueueHello.Count=%u RespVal=0x%x", 
			ChPtr, ChPtr->QueueHello.Count, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: dcp_RecvIndReq()                                       */
/* Process a user Receive Indication Request                                 */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_RecvIndReq(
DCP_CHA_PTR				ChPtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	DCP_LOWER_RQB_PTR_TYPE	RblPu;
	LSA_INT					Cnt;
	LSA_UINT16		 		RespVal = DCP_RSP_OK;
	LSA_UINT8				ServiceId = (LSA_UINT8)RbuPtr->args.RecvInd.ServiceId;
	DCP_UPPER_QUEUE	*		QueuePtr = &ChPtr->QueueInd;
	LSA_BOOL				Recv = LSA_TRUE;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_RecvIndReq() ChPtr=%X RbuPtr=%X QueueInd.Count=%u", 
			ChPtr, RbuPtr, ChPtr->QueueInd.Count);

	if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->CancelRbu, LSA_NULL))
	{
		/* Upper Cancel is active
		*/
		RespVal = DCP_RSP_ERR_SEQUENCE;
	}
	else if (DCP_HELLO == ServiceId)
	{
		/* Upper Hello Indication RQB 
		*/
		RespVal = dcp_HelloIndReq(ChPtr, RbuPtr);
	}
	else if (ChPtr->DetailPtr->AllocOnRequest)
	{
		/* Allocate Lower Receive RQB and queue it
		*/
		RespVal = dcp_AllocEthRecv(ChPtr, 0, DCP_NOSERVICE, LSA_NULL);
	}
	//110408lrg001:
	DCP_FUNCTION_TRACE_07(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  - dcp_RecvIndReq(RbuPtr=%X) RespVal=0x%04x ServiceId=%u: QueueInd.Count=%u QueueIdentify.Count=%u QueueRecv.Count=%u ServerIndReady=%u", 
			RbuPtr, RespVal, ServiceId, ChPtr->QueueInd.Count, ChPtr->QueueIdentify.Count, ChPtr->ChLowPtr->QueueRecv.Count, ChPtr->ServerIndReady);

	if (RespVal == DCP_RSP_OK)
	{
		/* Move RQB from Upper Channel Queue to Indication Queue
		*/
		dcp_DequeUpper(RbuPtr);
		RespVal = DCP_RSP_OK_ACTIVE;

		if (ChPtr->ServerIndReady)
		{
			/* Resource control: If the Upper Indication RQB has a valid ServiceID
			 * and the service queue is not "full":
			 * put RQB directly into Service Queue and send a lower Recv RQB to lower layer
			*/
			switch (ServiceId)
			{
			default:
				Recv = LSA_FALSE;
				break;
			case DCP_IDENTIFY:
				if (ChPtr->QueueIdentify.Count < DCP_NUM_SERVER_IDENTIFY_RESOURCES)
					QueuePtr = &ChPtr->QueueIdentify;
				break;
			case DCP_SET:
				if (ChPtr->QueueSet.Count < DCP_NUM_SERVER_SET_RESOURCES)
					QueuePtr = &ChPtr->QueueSet;
				break;
			case DCP_GET:
				if (ChPtr->QueueGet.Count < DCP_NUM_SERVER_GET_RESOURCES)
					QueuePtr = &ChPtr->QueueGet;
				break;
			}

			DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  - dcp_RecvIndReq() ServiceId=%u Queue.Count=%u QueueRecv.Count=%u", 
						ServiceId, QueuePtr->Count, ChPtr->ChLowPtr->QueueRecv.Count);
			if (!Recv)
			{
				dcp_PutUpperQ(QueuePtr, RbuPtr);
			}
			else if (0 != ChPtr->ChLowPtr->QueueRecv.Count)
			{
				/* Remove Receive RQB from Queue and send it to lower layer
				*/
				dcp_PutUpperQ(QueuePtr, RbuPtr);
				RblPu = ChPtr->ChLowPtr->QueueRecv.FirstPu;
				dcp_DequeLower(&ChPtr->ChLowPtr->QueueRecv, RblPu, ChPtr);
				dcp_DataRecv(ChPtr, RblPu, 0, 0);
			}
			else
			{
				/* No Lower Receive RQB
				*/
				dcp_PutUpperQ(&ChPtr->QueueInd, RbuPtr);
				DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_RecvIndReq() ChPtr=%X RbuPtr=%X Lower QueueRecv empty", 
								ChPtr, RbuPtr);
			}
			/* Dispose RQBs from common Indication Queue to Service Queues. TBD? -> 180408lrg001
			*/
		}
		else
		{
			/* Collect RQBs in the common Indication Queue
			 * until the configured sum of resources is reached
			*/
			dcp_PutUpperQ(&ChPtr->QueueInd, RbuPtr);
		}

		if (!ChPtr->ServerIndReady
		&& (DCP_NUM_SERVER_IDENTIFY_RESOURCES + \
			DCP_NUM_SERVER_SET_RESOURCES + \
			DCP_NUM_SERVER_GET_RESOURCES <= ChPtr->QueueInd.Count))
		{
			/* Resource control: configured sum reached:
			 * move RQBs from common Indication Queue to Service Queues
			 * happens only one time - else if a ServiceId is given
			*/
			if ((LSA_INT)ChPtr->ChLowPtr->QueueRecv.Count < \
			   (DCP_NUM_SERVER_IDENTIFY_RESOURCES  \
				+ DCP_NUM_SERVER_SET_RESOURCES     \
				+ DCP_NUM_SERVER_GET_RESOURCES     \
				- ChPtr->ChLowPtr->RecvRblAct      \
				+ ChPtr->ChLowPtr->RecvRblClient))
			{
				/* Not enough lower Receive RQBs
				*/
				RespVal = DCP_RSP_ERR_RESOURCE;
				dcp_GetUpperQ(&ChPtr->QueueInd, RbuPtr);
				dcp_EnqueUpper(RbuPtr);
				DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_RecvIndReq() ChPtr=%X RbuPtr=%X Lower QueueRecv.Count=%u", 
								ChPtr, RbuPtr, ChPtr->ChLowPtr->QueueRecv.Count);
			}
			else
			{
				/* Move upper RQBs to service queues and Receive RQBs from Queue to lower layer
				*/
				for (Cnt = 0; Cnt < DCP_NUM_SERVER_IDENTIFY_RESOURCES; Cnt++)
				{
					RbuPtr = ChPtr->QueueInd.FirstPtr; 
					dcp_GetUpperQ(&ChPtr->QueueInd, RbuPtr);
					dcp_PutUpperQ(&ChPtr->QueueIdentify, RbuPtr);
					if (0 != ChPtr->ChLowPtr->QueueRecv.Count)
					{
						RblPu = ChPtr->ChLowPtr->QueueRecv.FirstPu;
						dcp_DequeLower(&ChPtr->ChLowPtr->QueueRecv, RblPu, ChPtr);
						dcp_DataRecv(ChPtr, RblPu, 0, 0);
					}
				}
				for (Cnt = 0; Cnt < DCP_NUM_SERVER_GET_RESOURCES; Cnt++)
				{
					RbuPtr = ChPtr->QueueInd.FirstPtr; 
					dcp_GetUpperQ(&ChPtr->QueueInd, RbuPtr);
					dcp_PutUpperQ(&ChPtr->QueueGet, RbuPtr);
					if (0 != ChPtr->ChLowPtr->QueueRecv.Count)
					{
						RblPu = ChPtr->ChLowPtr->QueueRecv.FirstPu;
						dcp_DequeLower(&ChPtr->ChLowPtr->QueueRecv, RblPu, ChPtr);
						dcp_DataRecv(ChPtr, RblPu, 0, 0);
					}
				}
				for (Cnt = 0; Cnt < DCP_NUM_SERVER_SET_RESOURCES; Cnt++)
				{
					RbuPtr = ChPtr->QueueInd.FirstPtr; 
					dcp_GetUpperQ(&ChPtr->QueueInd, RbuPtr);
					dcp_PutUpperQ(&ChPtr->QueueSet, RbuPtr);
					if (0 != ChPtr->ChLowPtr->QueueRecv.Count)
					{
						RblPu = ChPtr->ChLowPtr->QueueRecv.FirstPu;
						dcp_DequeLower(&ChPtr->ChLowPtr->QueueRecv, RblPu, ChPtr);
						dcp_DataRecv(ChPtr, RblPu, 0, 0);
					}
				}
				ChPtr->ServerIndReady = LSA_TRUE;
			}
		}
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_RecvIndReq() ChPtr=%X ServerIndReady=%u RespVal=0x%x", 
			ChPtr, ChPtr->ServerIndReady, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: dcp_SendRspReq()                                       */
/* Process a user Send Response Request                                      */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_SendRspReq(
DCP_CHA_PTR				ChPtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	DCP_LOWER_RQB_PTR_TYPE  RblPu, RbtPu;
	DCP_TIMER_PTR_TYPE		TimerPtr;
	DCP_DATASET		*		DsPtr;
	LSA_INT			i;
	LSA_INT			SendLen, DataLen;
	LSA_INT			OptCnt = RbuPtr->args.SendRsp.OptCnt;
	LSA_UINT8		ServiceId = (LSA_UINT8)RbuPtr->args.SendRsp.ServiceId;
	LSA_UINT16		TimerRet, RespVal = DCP_RSP_OK;
	LSA_BOOL		Unlock  = LSA_FALSE;

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_SendRspReq() ChPtr=%X RbuPtr=%X ServiceId=%u OptCnt=%u", 
			ChPtr, RbuPtr, ServiceId, OptCnt);

	if (ChPtr->DetailPtr->AllocOnRequest)
	{
		/* Allocate Lower Send RQB and queue it
		*/
		RespVal = dcp_AllocEthSend(ChPtr, 0, DCP_NOSERVICE);
	}

	if ((ChPtr->QueueLower.Count != 0) && (DCP_RSP_OK == RespVal))
	{
		/* Calculate length of TLV chain
		*/
		for (i = 0, DataLen = 0; i < OptCnt; i++)
		{
			DsPtr = (DCP_DATASET *)(RbuPtr->args.SendRsp.DataPtr + DataLen);
			DataLen += DCP_DS_HDR_SIZE + DCP_SWAP16(DsPtr->Hdr.Len);
			if ((DataLen % 2) != 0)
			{
				DataLen++;
			}
#ifdef DCP_UNLOCK_ON_TLV	//AP01509782
			/* If chain contains a transaction end TLV with DCP_RESULT_OK:
			 * switch off Lock Timer
			*/
			if ((DCP_SET == ServiceId)
			&&  (DCP_TYPE_CTRL_END == DCP_SWAP16(DsPtr->Status))
			&&  (DCP_RESULT_OK == DsPtr->Data.Result.Code))
			{
				Unlock = LSA_TRUE;
			}
#endif
		}
		/* Fill Response Header and copy data from upper RQB
		*/ 
		RblPu = ChPtr->QueueLower.FirstPu;
		SendLen = dcp_RspHdrSet(ChPtr, RblPu, RbuPtr->args.SendRsp.AddrPtr, 
								ServiceId, RbuPtr->args.SendRsp.TransactId, 
								RbuPtr->args.SendRsp.DataPtr, DataLen);
		if (SendLen > 0)
		{
			/* Remove Lower Send RQB from Queue and send Response
			*/
			dcp_DequeLower(&ChPtr->QueueLower, RblPu, ChPtr);
			DCP_ETH_RQB_USER_PTR_SET(RblPu.RbEthPtr, RbuPtr);
			if (ServiceId == DCP_IDENTIFY)
			{
				/* Search for Identify timer structure
				*/
				TimerPtr = dcp_TimerTabFindAddr(ChPtr, RbuPtr->args.SendRsp.TransactId, RbuPtr->args.SendRsp.AddrPtr);
				if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
				{
					/* Timer started and not yet elapsed.
					 * 071005lrg002: if the Timer has no Lower RQB...
					*/
					if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr->RbPu.VoidPtr, LSA_NULL))
					{
						/* Put Lower RQB to timer to be sent when elapsed
						*/
						TimerPtr->RbPu = RblPu;
						TimerPtr->SendLen = SendLen;
					}
					else
					{
						/* ...else Free Lower Send RQB or put it back to queue
						 * and set error Response for user 
						*/
						if (ChPtr->DetailPtr->AllocOnRequest)
						{
							dcp_FreeLowerRb(ChPtr, RblPu);
						}
						else
						{
							dcp_EnqueLower(&ChPtr->QueueLower, RblPu, ChPtr);
						}
						RespVal = DCP_RSP_ERR_SEQUENCE;
						DCP_ERROR_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_WARN, "*** dcp_SendRspReq() ChPtr=%X RbuPtr=%X TimerId=%u: RbPu.VoidPtr=%X", 
								ChPtr, RbuPtr, TimerPtr->Id, TimerPtr->RbPu.VoidPtr);
					}
					/* Do not send response now
					*/
					SendLen = 0;
				}
			}
			else
			{
				/* On GET: refresh Lock Timer
				 * On SET: refresh Lock Timer or switch off at Transaction end
				*/
				DCP_MAC_ADR	NilAddr = {0,0,0,0,0,0};
					
				if (Unlock)
				{
					/* stop client hold timer - ID is always last allocated on channel
					*/
					DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- dcp_SendRspReq()  ChPtr=%X ClientHoldTime end for ...%02X-%02X-%02X", 
							ChPtr, ChPtr->ClientHoldAddr[3], ChPtr->ClientHoldAddr[4], ChPtr->ClientHoldAddr[5]);
					dcp_TimerStop(ChPtr, LSA_NULL, ChPtr->TimerCount, LSA_NULL);
					DCP_DS_SET_ADDR(ChPtr->ClientHoldAddr, NilAddr, DCP_MAC_ADR_SIZE);
				}
				else if ((DCP_CMP_MAC_ADDR(ChPtr->ClientHoldAddr, NilAddr))
					 ||  (DCP_CMP_MAC_ADDR(ChPtr->ClientHoldAddr, RbuPtr->args.SendRsp.AddrPtr)))
				{
					/* (re)start client hold timer - ID is always last allocated on channel
					 * 1064327: call dcp_TimerGet() and DCP_DS_SET_ADDR() with client MAC address
					 * 1104556: undo 1064327 but keep dcp_TimerGet()
					 * 1102112: do not restart client hold timer of another client
					*/
					RbtPu.VoidPtr = LSA_NULL;
					TimerPtr = dcp_TimerGet(ChPtr, DCP_TIMER_LOCK, 0, LSA_NULL, RbtPu, LSA_NULL);
					TimerRet = dcp_TimerStart(ChPtr, TimerPtr, ChPtr->TimerCount, LSA_NULL, (DCP_CLIENT_HOLD_TIME * 10), RbtPu);
					if ((TimerRet != LSA_RET_OK) && (TimerRet != LSA_RET_OK_TIMER_RESTARTED))
					{
						DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_SendRspReq() ChPtr=%X TimerId=%d: Timer start RetVal=0x%x",
										ChPtr, ChPtr->TimerCount, TimerRet);
						dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_TIMER, ChPtr, ChPtr->TimerCount);
					}
					DCP_DS_SET_ADDR(ChPtr->ClientHoldAddr, RbuPtr->args.SendRsp.AddrPtr, DCP_MAC_ADR_SIZE);
					DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- dcp_SendRspReq()  ChPtr=%X ClientHoldTime new for ...%02X-%02X-%02X", 
							ChPtr, ChPtr->ClientHoldAddr[3], ChPtr->ClientHoldAddr[4], ChPtr->ClientHoldAddr[5]);
				}
			}
			if (SendLen != 0)
			{
				/* Send Lower Response now
				*/
				dcp_DataSend(ChPtr, RblPu, SendLen);
			}
		}
		else
		{
			/* Invalid Parameter in upper RQB
			*/
			RespVal = DCP_RSP_ERR_PARAM;
			DCP_ERROR_TRACE_05(ChPtr->TraceIdx, LSA_TRACE_LEVEL_WARN, "*** dcp_SendRspReq() ChPtr=%X RbuPtr=%X ServiceId=%u OptCnt=%u DataLen=%u: Invalid parameter", 
					ChPtr, RbuPtr, ServiceId, OptCnt, DataLen);
			if (ChPtr->DetailPtr->AllocOnRequest)
			{
				/* Free allocated Lower Send RQB
				*/
				dcp_DequeLower(&ChPtr->QueueLower, RblPu, ChPtr);
				dcp_FreeLowerRb(ChPtr, RblPu);
			}
		}
	}
	else
	{
		/* No Lower Receive RQB
		*/
		RespVal = DCP_RSP_ERR_RESOURCE;
		DCP_ERROR_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_WARN, "*** dcp_SendRspReq() ChPtr=%X RbuPtr=%X ServiceId=%u OptCnt=%u: No Lower Send RQB", 
				ChPtr, RbuPtr, ServiceId, OptCnt);
	}

	if (DCP_RSP_OK == RespVal)
	{
		/* Move RQB from Upper Channel Queue to Response Queue
		*/
		dcp_DequeUpper(RbuPtr);
		dcp_PutUpperQ(&ChPtr->QueueRsp, RbuPtr);
		RespVal = DCP_RSP_OK_ACTIVE;
	}
									
	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_SendRspReq() ChPtr=%X RespVal=0x%x", 
			ChPtr, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: dcp_CancelRecvReq()                                    */
/* Process user Cancel Receive Request                                       */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_CancelRecvReq(
DCP_CHA_PTR				ChPtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	LSA_UINT16		 		RespVal = DCP_RSP_OK;
	LSA_INT				Cnt, CancelCount = 0;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_CancelRecvReq() ChPtr=%X RbuPtr=%X CancelRbu=%X", 
						ChPtr, RbuPtr, ChPtr->CancelRbu);

	if (LSA_HOST_PTR_ARE_EQUAL(ChPtr->CancelRbu, LSA_NULL))
	{
		if (0 == RbuPtr->args.Cancel.Count)
		{
			/* Cancel all Recv Requests
			*/
			CancelCount = ChPtr->QueueIdentify.Count + ChPtr->QueueGet.Count + ChPtr->QueueSet.Count;
			if (CancelCount != 0)
			{
				/* No upper Cancel request is active and at least one Request is
				 * in one of the Queues: cancel all receives from lower channel.
				 * Too much cancelled Receive-RQBs will be sent to lower layer
				 * again on receive indication.
				*/
				RespVal = dcp_CancelRecv(ChPtr, 0, 0);
			}
			else if(ChPtr->QueueHello.Count != 0)
			{
				/* Start Hello Cancel on FSU channel now - else 
				 * at lower Cancel Confirmation in dcp_ServerCancel()
				*/
				CancelCount = ChPtr->QueueHello.Count;
				RespVal = dcp_CancelRecv(ChPtr->ChNextPtr, 0, 0);
			}
		}
		else
		{
			/* Only upper RQBs of the common Indication Queue will be cancelled
			 * For these the lower layer has no RQB.
			*/
			if (RbuPtr->args.Cancel.Count < (LSA_INT)ChPtr->QueueInd.Count)
				CancelCount = RbuPtr->args.Cancel.Count;
			else
				CancelCount = ChPtr->QueueInd.Count;
			for (Cnt = 0; Cnt < CancelCount; Cnt++)
			{
				/* Indicate cancelled upper RecvInd-RQB
				*/
				RbPtr = ChPtr->QueueInd.FirstPtr;
				dcp_GetUpperQ(&ChPtr->QueueInd, RbPtr);
		        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
			}
			RbuPtr->args.Cancel.Count = CancelCount;
		}
	}
	else
		RespVal = DCP_RSP_ERR_SEQUENCE;

	if (DCP_RSP_OK_ACTIVE == RespVal)
	{
	    /* Move RQB from Upper Channel Queue to channel structure
		 * und initialise return parameter with CancelCount.
	    */
		dcp_DequeUpper(RbuPtr);
		RbuPtr->args.Cancel.Count = CancelCount;
		ChPtr->CancelRbu = RbuPtr;
		ChPtr->ServerIndReady = LSA_FALSE;
		ChPtr->State = DCP_CHA_STATE_CANCEL;
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_CancelRecvReq() ChPtr=%X RbuPtr=%X RespVal=0x%x", 
						ChPtr, RbuPtr, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal function: dcp_RspHdrSet()                                        */
/* Fill DCP Response header and copy data part                               */
/*****************************************************************************/
LSA_INT 	DCP_LOCAL_FCT_ATTR dcp_RspHdrSet(
DCP_CHA_PTR 			ChPtr,
DCP_LOWER_RQB_PTR_TYPE  RblPu,
DCP_USR_BYTE_PTR		AdrPtr,
LSA_UINT8 				ServiceId,
LSA_UINT32 				TransactId,
DCP_USR_BYTE_PTR		DataPtr,
LSA_INT					DataLen)
{
	DCP_ETH_SEND_PTR		EthFramePtr;
	DCP_ETH_TXBYTE_PTR_TYPE	DstPtr;
	LSA_INT					SendLen = 0;

	if (ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	{
		DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_RspHdrSet() ChPtr=%X RbEthPtr=%X DataLen=%u", 
							ChPtr, RblPu.RbEthPtr, DataLen);

		EthFramePtr = (DCP_ETH_SEND_PTR)DCP_ETH_RQB_SEND_PTR_GET(RblPu.RbEthPtr);

		/* Ethernetheader
		*/
		DCP_ETH_FRAME_MACADR_SET(&EthFramePtr->Hdr.DstMacAdr, AdrPtr);
		DCP_ETH_FRAME_MACADR_SET(&EthFramePtr->Hdr.SrcMacAdr, &ChPtr->ChLowPtr->LocalMacAdr);

		/* SRT Header
		*/
		EthFramePtr->Hdr.VlanTag[0] = DCP_VLANTAG_0;
		EthFramePtr->Hdr.VlanTag[1] = DCP_VLANTAG_1;
		EthFramePtr->Hdr.VlanTag[2] = DCP_VLANTAG_2;
		EthFramePtr->Hdr.VlanTag[3] = DCP_VLANTAG_3;
		EthFramePtr->Hdr.EthType[0] = DCP_ETHTYPE_0;
		EthFramePtr->Hdr.EthType[1] = DCP_ETHTYPE_1;
		EthFramePtr->Hdr.FrameId[0] = DCP_FRAMEID_0;

		if (DCP_IDENTIFY == ServiceId)
			EthFramePtr->Hdr.FrameId[1] = DCP_FRAMEID_MRS;
		else
			EthFramePtr->Hdr.FrameId[1] = DCP_FRAMEID_URR;

		/* DCP-PDU-Header
		*/
		EthFramePtr->Pdu.Rsp.Hdr.ServiceId = ServiceId;
		EthFramePtr->Pdu.Rsp.Hdr.ServiceType = DCP_ROLE_RSP;

		if ((ServiceId != DCP_IDENTIFY) 
		&&  (ServiceId != DCP_GET) 
		&&  (ServiceId != DCP_SET))
			EthFramePtr->Pdu.Rsp.Hdr.ServiceType |= DCP_RESULT_NO_SERVICE;

		EthFramePtr->Pdu.Rsp.Hdr.XidHigh = DCP_SWAP16(DCP_HIWORD(TransactId));
		EthFramePtr->Pdu.Rsp.Hdr.XidLow = DCP_SWAP16(DCP_LOWORD(TransactId));
		EthFramePtr->Pdu.Rsp.Hdr.RspDelayHigh = DCP_RESERVED;
		EthFramePtr->Pdu.Rsp.Hdr.RspDelayLow = DCP_RESERVED;
		EthFramePtr->Pdu.Rsp.Hdr.Length = DCP_SWAP16(DataLen);

		if (DataLen <= DCP_MAX_RSP_DATA_SIZE)
		{
			if (DataLen > 0)
			{
				/* DCP-PDU-Data
				*/
				DstPtr = &EthFramePtr->Pdu.Rsp.Data.Byte[0];
				for (SendLen = 0; SendLen < DataLen; SendLen++)
					DstPtr[SendLen] =  DataPtr[SendLen];				

				SendLen += DCP_ETHDR_SIZE + DCP_HEADER_SIZE;				
			}
			else
				SendLen = DCP_ETHDR_SIZE + DCP_HEADER_SIZE;				
		}
	}
	else
		/* currently only Ethernet is supported
		*/
		DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_RspHdrSet() ChPtr=%X DCP_ERR_DEVICE=0x%x", 
						ChPtr, DCP_ERR_DEVICE);

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_RspHdrSet() ServiceId=%u TransactId=0x%08x SendLen=%d", 
						ServiceId, TransactId, SendLen);
	return(SendLen);
}

/*****************************************************************************/
/* Internal function: dcp_Set()                                              */
/* Process remote SET Request                                                */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_Set(
DCP_REQ_PTR			ReqPtr,
LSA_INT				ReqLen,
DCP_ETHDR		*	HdrPtr,
DCP_CHA_PTR 		ChPtr)
{
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	LSA_INT					ReadLen;
	LSA_UINT8			*	ReadPtr;
	LSA_UINT8			*	WritePtr;
	DCP_DATASET		 	*	DsPtr;
	LSA_INT					OptCount = 0;
	LSA_INT					RestLen = ReqLen - DCP_HEADER_SIZE;
	LSA_UINT16				RetVal = LSA_RET_OK;
	DCP_UPPER_QUEUE		*	QueuePtr = &ChPtr->QueueSet;
	LSA_UINT32				TransactId = DCP_DWORD(DCP_SWAP16(ReqPtr->Hdr.XidLow),DCP_SWAP16(ReqPtr->Hdr.XidHigh));

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_Set() ChPtr=%X XID=0x%08x QueueSet.FirstPtr=%X QueueInd.FirstPtr=%X", 
						ChPtr, TransactId, ChPtr->QueueSet.FirstPtr, ChPtr->QueueInd.FirstPtr);

	/* Get Upper Indication RQB from Queue
	 * and check data length: at least one TLV Header plus Status
	*/
	RbPtr = QueuePtr->FirstPtr;
	if (LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL) && (ChPtr->ServerIndReady))
	{
		
		/* 180408lrg001: After the RQBs are disposed to the service Queues:
		 * also look in the common Queue
		*/
		QueuePtr = &ChPtr->QueueInd;
		RbPtr = QueuePtr->FirstPtr;
		while (!LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
		{
			/* 180408lrg002: look for matching ServiceId
			*/
			if (DCP_SET == RbPtr->args.RecvInd.ServiceId)
				break;
			else
				RbPtr = DCP_UPPER_RQB_NEXT_GET(RbPtr);
		}
	}
	if (LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
	{
		RetVal = LSA_RET_ERR_RESOURCE;
	}
	else if (RestLen < DCP_DS_HDR_SIZE+2)
	{
		RetVal = LSA_RET_ERR_PARAM;
		OptCount = -1;
	}
	else
	{
		/* 1157678: check option TLV chain
		*/
		DsPtr = &ReqPtr->Data.Set.Dataset;
		ReadPtr = (LSA_UINT8*)DsPtr;
		while (RestLen > 0)
		{
			ReadLen = DCP_DS_HDR_SIZE + DCP_SWAP16(DsPtr->Hdr.Len);
			if (RestLen < ReadLen)
			{
				/* 1157678: Truncated dataset
				*/
				RestLen = 0;
				OptCount = 0;
			}
			else
			{
				if ((ReadLen % 2) != 0)
				{
					/* Count also the fillbyte
					*/
					ReadLen++;
				}
				OptCount++;
				RestLen -= ReadLen;
				if (RestLen != 0)
				{
					/* Next dataset in Request 
					*/
					ReadPtr += ReadLen;
					DsPtr = (DCP_DATASET *)ReadPtr;
				}
			}
		}
		if (0 == OptCount)
		{
			RetVal = LSA_RET_ERR_PARAM;
		}
	}
	if (LSA_RET_OK == RetVal)
	{
		/* Set Source MAC Address, Service ID and Transaction ID
		 * in the Indication RQB
		*/
		DCP_COPY_LOCAL_TO_UPPER_MEM(HdrPtr->SrcMacAdr, RbPtr->args.RecvInd.AddrPtr, 
									DCP_MAC_ADR_SIZE, ChPtr->SysPtr);
		RbPtr->args.RecvInd.ServiceId = DCP_SET;
		RbPtr->args.RecvInd.TransactId = TransactId;

		/* Copy Data TLVs to user buffer
		*/
		ReadPtr = (LSA_UINT8*)&ReqPtr->Data.Set.Dataset;
		WritePtr = (LSA_UINT8*)RbPtr->args.RecvInd.DataPtr;
		DCP_COPY_LOCAL_TO_UPPER_MEM(ReadPtr, WritePtr, (LSA_UINT16)(ReqLen - DCP_HEADER_SIZE), ChPtr->SysPtr);
		 
		/* Write count of Data TLVs and
		 * indicate Receive RQB to user
		*/
		DCP_UPPER_RQB_FSU_SET(RbPtr, 0);
		RbPtr->args.RecvInd.OptCnt = OptCount;
		dcp_GetUpperQ(QueuePtr, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK);
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_Set() ChPtr=%X OptCount=%d RetVal=0x%x", 
						ChPtr, OptCount, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: dcp_Get()                                              */
/* Process remote GET Request                                                */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_Get(
DCP_REQ_PTR			ReqPtr,
LSA_INT				ReqLen,
DCP_ETHDR		*	HdrPtr,
DCP_CHA_PTR 		ChPtr)
{
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	LSA_UINT8			*	ReadPtr;
	LSA_UINT8			*	WritePtr;
	DCP_TYPE		 	*	DsPtr;
	LSA_UINT16				RetVal = LSA_RET_OK;
	LSA_INT					RestLen = ReqLen - DCP_HEADER_SIZE;
	LSA_INT					DsetCnt = RestLen/2;
	DCP_UPPER_QUEUE		*	QueuePtr = &ChPtr->QueueGet;
	LSA_UINT32				TransactId = DCP_DWORD(DCP_SWAP16(ReqPtr->Hdr.XidLow),DCP_SWAP16(ReqPtr->Hdr.XidHigh));

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_Get() ChPtr=%X XID=0x%08x QueueGet.FirstPtr=%X QueueInd.FirstPtr=%X", 
						ChPtr, TransactId, ChPtr->QueueGet.FirstPtr, ChPtr->QueueInd.FirstPtr);

	/* Get Upper Indication RQB from Queue
	 * and check for minimum  one requested dataset
	*/
	RbPtr = QueuePtr->FirstPtr;
	if (LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL) && (ChPtr->ServerIndReady))
	{
		
		/* 180408lrg001: After the RQBs are disposed to the service Queues:
		 * also look in the common Queue
		*/
		QueuePtr = &ChPtr->QueueInd;
		RbPtr = QueuePtr->FirstPtr;
		while (!LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
		{
			/* 180408lrg002: look for matching ServiceId
			*/
			if (DCP_GET == RbPtr->args.RecvInd.ServiceId)
				break;
			else
				RbPtr = DCP_UPPER_RQB_NEXT_GET(RbPtr);
		}
	}
	if (LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
		RetVal = LSA_RET_ERR_RESOURCE;
	else if (DsetCnt == 0)
		RetVal = LSA_RET_ERR_PARAM;
	else
	{
		/* Set Source MAC Address, Service ID and Transaction ID
		 * in the Indication RQB
		*/
		DCP_COPY_LOCAL_TO_UPPER_MEM(HdrPtr->SrcMacAdr, RbPtr->args.RecvInd.AddrPtr, 
									DCP_MAC_ADR_SIZE, ChPtr->SysPtr);
		RbPtr->args.RecvInd.ServiceId = DCP_GET;
		RbPtr->args.RecvInd.TransactId = TransactId;

		/* Copy Types List to user buffer
		*/
		DsPtr = ReqPtr->Data.Get.Type;
		ReadPtr = (LSA_UINT8*)DsPtr;
		WritePtr = (LSA_UINT8*)RbPtr->args.RecvInd.DataPtr;
		 
		DCP_COPY_LOCAL_TO_UPPER_MEM(ReadPtr, WritePtr, (LSA_UINT16)RestLen, ChPtr->SysPtr);

		/* Write count of GET Types and
		 * indicate Receive RQB to user
		*/
		DCP_UPPER_RQB_FSU_SET(RbPtr, 0);
		RbPtr->args.RecvInd.OptCnt = DsetCnt;
		dcp_GetUpperQ(QueuePtr, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK);
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_Get() ChPtr=%X RetVal=0x%x", 
					ChPtr, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: dcp_Identify()                                         */
/* Process remote IDENTIFY Request                                           */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_Identify(
DCP_REQ_PTR			ReqPtr,
LSA_INT				ReqLen,
DCP_ETHDR		*	HdrPtr,
DCP_CHA_PTR 		ChPtr)
{
	DCP_TIMER_PTR_TYPE		TimerPtr;
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	DCP_LOWER_RQB_PTR_TYPE  RblPu;
	LSA_UINT8			*	ReadPtr;
	LSA_UINT8			*	WritePtr;
	DCP_DATASET_IDENT 	*	DsPtr;
	LSA_UINT32				Time;
	LSA_INT					ReadLen;
	LSA_INT					OptCount = 0;
	LSA_INT					RestLen = ReqLen - DCP_HEADER_SIZE;
	LSA_UINT32				TransactId= DCP_DWORD(DCP_SWAP16(ReqPtr->Hdr.XidLow),DCP_SWAP16(ReqPtr->Hdr.XidHigh));
	LSA_UINT16				Faltung = 0;
	LSA_UINT16				RetVal = LSA_RET_OK;
	DCP_UPPER_QUEUE		*	QueuePtr = &ChPtr->QueueIdentify;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_Identify() ChPtr=%X QueueIdentify.FirstPtr=%X QueueInd.FirstPtr=%X", 
						ChPtr, ChPtr->QueueIdentify.FirstPtr, ChPtr->QueueInd.FirstPtr);
	//110408lrg001:
	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  - dcp_Identify() TransactId=0x%08x QueueIdentify.Count=%u QueueInd.Count=%u", 
						TransactId, ChPtr->QueueIdentify.Count, ChPtr->QueueInd.Count);

	/* Get Upper Indication RQB from Queue
	*/
	RbPtr = QueuePtr->FirstPtr;
	if (LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL) && (ChPtr->ServerIndReady))
	{
		
		/* 180408lrg001: After the RQBs are disposed to the service Queues:
		 * also look in the common Queue
		*/
		QueuePtr = &ChPtr->QueueInd;
		RbPtr = QueuePtr->FirstPtr;
		while (!LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
		{
			/* 180408lrg002: look for matching ServiceId
			*/
			if (DCP_IDENTIFY == RbPtr->args.RecvInd.ServiceId)
				break;
			else
				RbPtr = DCP_UPPER_RQB_NEXT_GET(RbPtr);
		}
	}
	if (LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
	{
		RetVal = LSA_RET_ERR_RESOURCE;
	}
	else if (RestLen < DCP_DS_IDENT_HDR_SIZE)
	{
		RetVal = LSA_RET_ERR_PARAM;
	}
	else
	{
		/* 1157678: check filter TLV chain
		*/
		DsPtr = &ReqPtr->Data.Identify.Filter;
		ReadPtr = (LSA_UINT8*)DsPtr;
		while (RestLen > 0)
		{
			ReadLen = DCP_DS_HDR_SIZE + DCP_SWAP16(DsPtr->Len);
			if (RestLen < ReadLen)
			{
				/* 1157678: Truncated filter
				*/
				RestLen = 0;
				OptCount = 0;
			}
			else
			{
				if ((ReadLen % 2) != 0)
				{
					/* Count also the fillbyte
					*/
					ReadLen++;
				}
				OptCount++;
				RestLen -= ReadLen;
				if (RestLen != 0)
				{
					/* Next dataset in Request 
					*/
					ReadPtr += ReadLen;
					DsPtr = (DCP_DATASET_IDENT *)ReadPtr;
				}
			}
		}
		if (0 == OptCount)
		{
			RetVal = LSA_RET_ERR_PARAM;
		}
	}
	if (LSA_RET_OK == RetVal)
	{
		/* 071005lrg001: Check for existing Timer with this 
		 * Transaction ID and this Source MAC Address
		*/
        TimerPtr = dcp_TimerTabFindAddr(ChPtr, TransactId, HdrPtr->SrcMacAdr);
		if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
        {
			RetVal = LSA_RET_ERR_SEQUENCE;
			DCP_ERROR_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "*** dcp_Identify() TransactId=0x%x already exists at TimerPtr=%X TimerId=%u", 
					TransactId, TimerPtr, TimerPtr->Id);
        }

		if (LSA_RET_OK == RetVal)
		{
			/* AP01509782: Get Faltung only from PDU: no default value
			*/
			Faltung = DCP_WORD(ReqPtr->Hdr.RspDelayLow, ReqPtr->Hdr.RspDelayHigh);
			if (Faltung == 0)
			{
				RetVal = LSA_RET_ERR_PARAM;
			}
			
			if (LSA_RET_OK == RetVal)
			{
				/* Calculate response delay
				 * T_delay = t_basis * ( K_addr % Faltung) 	[sec]
				*/
				Time = DCP_MULTICAST_DELAY_BASE * (ChPtr->ChLowPtr->K_addr % Faltung);

				if (Time != 0)
				{
					/* Calculate Timer Ticks
					*/
					if (0 == (Time % DCP_MULTICAST_REQ_TIMER_BASE))
					{
						Time = Time / DCP_MULTICAST_REQ_TIMER_BASE;
					}
					else
					{
						/* Round up to next tick
						 */
						Time = (Time / DCP_MULTICAST_REQ_TIMER_BASE) + 1;
					}
					/* Get Timer
					*/
					RblPu.VoidPtr = LSA_NULL;
					TimerPtr = dcp_TimerGet(ChPtr, DCP_TIMER_IDENTIFY, (LSA_UINT16)Time, LSA_NULL, RblPu, LSA_NULL);
					if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
					{
						/* No free Timer
						*/
						RetVal = LSA_RET_ERR_RESOURCE;
						DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "*** dcp_Identify() TransactId=0x%x: no free timer", 
								TransactId);
					}
				}
	
				if (LSA_RET_OK == RetVal)
				{
					if (Time != 0)
					{
						/* Set Identify Timer parameters for the Response
						*/
						DCP_DS_SET_ADDR(TimerPtr->Tract.IdentifyData.ClientAdr, HdrPtr->SrcMacAdr, 
										DCP_MAC_ADR_SIZE);
						TimerPtr->Tract.IdentifyData.TransactId = TransactId;
						RetVal = dcp_TimerTabStart(TimerPtr);
						if (LSA_RET_OK != RetVal)
						{
							DCP_ERROR_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_Identify() RetVal=0x%x from dcp_TimerTabStart() TimerPtr=%X TimerId=%u", 
										RetVal, TimerPtr, TimerPtr->Id);
							
						}
					}
	
					/* Set Source MAC Address, Service ID and Transaction ID
					 * in the Indication RQB
					*/
					DCP_COPY_LOCAL_TO_UPPER_MEM(HdrPtr->SrcMacAdr, RbPtr->args.RecvInd.AddrPtr, 
												DCP_MAC_ADR_SIZE, ChPtr->SysPtr);
					RbPtr->args.RecvInd.ServiceId = DCP_IDENTIFY;
					RbPtr->args.RecvInd.TransactId = TransactId;
	
					/* Copy Data TLVs (filters) to user buffer
					*/
					ReadPtr = (LSA_UINT8*)&ReqPtr->Data.Identify.Filter;
					WritePtr = (LSA_UINT8*)RbPtr->args.RecvInd.DataPtr;
					DCP_COPY_LOCAL_TO_UPPER_MEM(ReadPtr, WritePtr, (LSA_UINT16)(ReqLen - DCP_HEADER_SIZE), ChPtr->SysPtr);
					 
					/* Write count of filter TLVs and
					 * indicate Receive RQB to user
					*/
					DCP_UPPER_RQB_FSU_SET(RbPtr, 0);
					RbPtr->args.RecvInd.OptCnt = OptCount;
					dcp_GetUpperQ(QueuePtr, RbPtr);
					dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK);
				}
			}
		}
	}

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_Identify() ChPtr=%X ReqPtr=%X OptCount=%d RetVal=0x%x", 
						ChPtr, ReqPtr, OptCount, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: dcp_Hello()                                            */
/* Process remote HELLO Request                                              */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_Hello(
DCP_REQ_PTR			ReqPtr,
LSA_INT				ReqLen,
DCP_ETHDR		*	HdrPtr,
LSA_UINT32			FilterInfo,
DCP_CHA_PTR 		ChPtr)
{
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	LSA_INT					ReadLen;
	LSA_UINT8			*	ReadPtr;
	LSA_UINT8			*	WritePtr;
	DCP_DATASET		 	*	DsPtr;
	LSA_INT					OptCount = 0;
	LSA_INT					RestLen = ReqLen - DCP_HEADER_SIZE;
	LSA_UINT16				RetVal = LSA_RET_OK;
	LSA_UINT32				TransactId = DCP_DWORD(DCP_SWAP16(ReqPtr->Hdr.XidLow),DCP_SWAP16(ReqPtr->Hdr.XidHigh));

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_Hello() ChPtr=%X XID=0x%08x FilterInfo=0x%08x QueueHello.Count=%u", 
						ChPtr, TransactId, FilterInfo, ChPtr->QueueHello.Count);

	/* Get Upper Indication RQB from Queue
	 * and check data length: at least one TLV Header plus Status
	*/
	RbPtr = ChPtr->QueueHello.FirstPtr;
	if (LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
	{
		RetVal = LSA_RET_ERR_RESOURCE;
	}
	else if (RestLen < DCP_DS_HDR_SIZE+2)
	{
		RetVal = LSA_RET_ERR_PARAM;
		OptCount = -1;
	}
	else
	{
		/* 1157678: check option TLV chain
		*/
		DsPtr = &ReqPtr->Data.Hello.Dataset;
		ReadPtr = (LSA_UINT8*)DsPtr;
		while (RestLen > 0)
		{
			ReadLen = DCP_DS_HDR_SIZE + DCP_SWAP16(DsPtr->Hdr.Len);
			if (RestLen < ReadLen)
			{
				/* 1157678: Truncated dataset
				*/
				RestLen = 0;
				OptCount = 0;
			}
			else
			{
				if ((ReadLen % 2) != 0)
				{
					/* Count also the fillbyte
					*/
					ReadLen++;
				}
				OptCount++;
				RestLen -= ReadLen;
				if (RestLen != 0)
				{
					/* Next dataset in Request 
					*/
					ReadPtr += ReadLen;
					DsPtr = (DCP_DATASET *)ReadPtr;
				}
			}
		}
		if (0 == OptCount)
		{
			RetVal = LSA_RET_ERR_PARAM;
		}
	}
	if (LSA_RET_OK == RetVal)
	{
		/* Set Source MAC Address, Service ID and Transaction ID
		 * in the Indication RQB
		*/
		DCP_COPY_LOCAL_TO_UPPER_MEM(HdrPtr->SrcMacAdr, RbPtr->args.RecvInd.AddrPtr, 
									DCP_MAC_ADR_SIZE, ChPtr->SysPtr);
		RbPtr->args.RecvInd.ServiceId = DCP_HELLO;
		RbPtr->args.RecvInd.TransactId = TransactId;

		/* Copy Data TLVs to user buffer
		*/
		ReadPtr = (LSA_UINT8*)&ReqPtr->Data.Hello.Dataset;
		WritePtr = (LSA_UINT8*)RbPtr->args.RecvInd.DataPtr;
		DCP_COPY_LOCAL_TO_UPPER_MEM(ReadPtr, WritePtr, (LSA_UINT16)(ReqLen - DCP_HEADER_SIZE), ChPtr->SysPtr);
		 
		/* Write count of Data TLVs and
		 * indicate Receive RQB to user
		*/
		DCP_UPPER_RQB_FSU_SET(RbPtr, 1);
		RbPtr->args.RecvInd.OptCnt = OptCount;
		RbPtr->args.RecvInd.FilterId = FilterInfo;
		dcp_GetUpperQ(&ChPtr->QueueHello, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK);
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_Hello() ChPtr=%X OptCount=%d RetVal=0x%x", 
						ChPtr, OptCount, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: dcp_UnknownService()                                   */
/* Process unknown Service                                                   */
/*****************************************************************************/
LSA_UINT16 	DCP_LOCAL_FCT_ATTR dcp_UnknownService(
DCP_REQ_PTR			ReqPtr,
LSA_INT				ReqLen,
DCP_ETHDR		*	HdrPtr,
DCP_CHA_PTR 		ChPtr)
{
	LSA_UINT16				RetVal = LSA_RET_OK;

	LSA_INT					SendLen;
	DCP_LOWER_RQB_PTR_TYPE  RblPu;

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_UnknownService() ChPtr=%X ReqPtr=%X ReqLen=%d ServiceId=%u", 
						ChPtr, ReqPtr, ReqLen, ReqPtr->Hdr.ServiceId);

	if (ChPtr->DetailPtr->AllocOnRequest)
	{
		/* Allocate Lower Send RQB and queue it
		*/
		RetVal = dcp_AllocEthSend(ChPtr, 0, DCP_NOSERVICE);
	}

	/* If a Send request block is in lower Queue...
	*/
	if (ChPtr->QueueLower.Count > 0)
	{
		/* Remove Request block from Queue and write NULL instead of upper RQB pointer
		 * to UserId for identifikation in dcp_ServerConf()
		*/
		RblPu = ChPtr->QueueLower.FirstPu;
		dcp_DequeLower(&ChPtr->QueueLower, RblPu, ChPtr);
		DCP_ETH_RQB_USER_PTR_SET(RblPu.RbEthPtr, LSA_NULL);

		/* Fill RQB and Send it
		*/
		SendLen = dcp_RspHdrSet(ChPtr, RblPu, HdrPtr->SrcMacAdr, ReqPtr->Hdr.ServiceId, 
								DCP_DWORD(DCP_SWAP16(ReqPtr->Hdr.XidLow),DCP_SWAP16(ReqPtr->Hdr.XidHigh)),
								LSA_NULL, 0);
		dcp_DataSend(ChPtr,	RblPu, SendLen);
	}
	else
	{
		RetVal = LSA_RET_ERR_RESOURCE;
		DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_UnknownService() ChPtr=%X No ressource for error response on ServiceId=%u", 
						ChPtr, ReqPtr->Hdr.ServiceId);
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_UnknownService() ChPtr=%X ReqPtr=%X RetVal=0x%x", 
						ChPtr, ReqPtr, RetVal);

	LSA_UNUSED_ARG(ReqLen);

	return(RetVal);
}
/*****************************************************************************/
/* Internal function: dcp_LockOnClient()                                     */
/* Check whether it is allowed to process the remote request now:            */
/* NO if the server is locked to another client at the moment                */
/* YES: server will be locked to the requesting client. If server is already */ 
/* locked to the requesting client, only the LockTimer is refreshed          */
/* NO: the request will be ignored through RetVal = LSA_FALSE.               */
/*****************************************************************************/
LSA_BOOL 	DCP_LOCAL_FCT_ATTR dcp_LockOnClient(
DCP_ETHDR		*	HdrPtr,
DCP_CHA_PTR 		ChPtr)
{
	DCP_LOWER_RQB_PTR_TYPE  RblPu;
	LSA_UINT16				TimerRet;
	LSA_BOOL				RetVal = LSA_FALSE;
	DCP_MAC_ADR				NilAddr = {0,0,0,0,0,0};
	DCP_TIMER_PTR_TYPE		TimerPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_LockOnClient() ChPtr=%X HdrPtr=%X", 
			ChPtr, HdrPtr);

	if (DCP_CMP_MAC_ADDR(ChPtr->ClientHoldAddr, NilAddr))
	{
		/* Server is not locked: lock to Client and activate client hold timer
		*/
		DCP_DS_SET_ADDR(ChPtr->ClientHoldAddr, HdrPtr->SrcMacAdr, DCP_MAC_ADR_SIZE);
		RetVal = LSA_TRUE;
	}
	else if (DCP_CMP_MAC_ADDR(ChPtr->ClientHoldAddr, HdrPtr->SrcMacAdr))
	{
		/* If already locked to this Client: refresh client hold timer
		*/
		RetVal = LSA_TRUE;
	}
	if (RetVal)
	{
		/* (re)start client hold timer - ID is always last allocated on channel
		*/
		RblPu.VoidPtr = LSA_NULL;
		TimerPtr = dcp_TimerGet(ChPtr, DCP_TIMER_LOCK, 0, LSA_NULL, RblPu, LSA_NULL);
		TimerRet = dcp_TimerStart(ChPtr, TimerPtr, ChPtr->TimerCount, LSA_NULL, (DCP_CLIENT_HOLD_TIME * 10), RblPu);
		if ((TimerRet != LSA_RET_OK) && (TimerRet != LSA_RET_OK_TIMER_RESTARTED))
		{
			DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_LockOnClient() ChPtr=%X TimerId=%d: Timer start RetVal=0x%x",
					ChPtr, ChPtr->TimerCount, RetVal);
			dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_TIMER, ChPtr, ChPtr->TimerCount);
		}
		DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- dcp_LockOnClient: ChPtr=%X ClientHoldTime set for ...%02X-%02X-%02X", 
				ChPtr, ChPtr->ClientHoldAddr[3], ChPtr->ClientHoldAddr[4], ChPtr->ClientHoldAddr[5]);
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_LockOnClient() ChPtr=%X TimerPtr=%X RetVal=0x%x", 
			ChPtr, TimerPtr, RetVal);

	return(RetVal);
}

/*****************************************************************************/
/* Internal function: dcp_CheckReq()                                         */
/* Check DCP Request.                                                        */
/*****************************************************************************/
LSA_UINT8 	DCP_LOCAL_FCT_ATTR dcp_CheckReq(
DCP_REQ_PTR			ReqPtr,
LSA_UINT16			PduLen,
DCP_UINT8			FrameId0,
DCP_UINT8			FrameId1,
DCP_CHA_PTR 		ChPtr)
{
	LSA_UINT16	DataLen = DCP_SWAP16(ReqPtr->Hdr.Length);
	LSA_UINT8	ServiceId = DCP_NOREQUEST;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_CheckReq() ReqPtr=%X PduLen=%u DataLen=%u", 
						ReqPtr, PduLen, DataLen);

	if ((DCP_FRAMEID_0 != FrameId0)
	||  ((FrameId1 == DCP_FRAMEID_HLO) && (ChPtr->DetailPtr->DevId == DCP_DEV_ETH))
	||  ((FrameId1 != DCP_FRAMEID_HLO) && (ChPtr->DetailPtr->DevId == DCP_DEV_FSU)))
	{
		DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_CheckReq() ChPtr=%X Invalid FrameId=0x%04x on channel DevId=%u", 
						ChPtr, DCP_WORD(FrameId1, FrameId0), ChPtr->DetailPtr->DevId);
	}
	else if ((DataLen <= sizeof(DCP_REQ_DATA)) 
	     &&  (DataLen <= (PduLen - DCP_HEADER_SIZE)))
	{
		if ((DCP_ROLE_REQ == (ReqPtr->Hdr.ServiceType & DCP_ROLE_MASK))
		&&  (DCP_FRAMEID_0 == FrameId0))
		{
			ServiceId = ReqPtr->Hdr.ServiceId;
			if (ServiceId == DCP_IDENTIFY)
			{ 
				if (FrameId1 != DCP_FRAMEID_MRQ)
					ServiceId = DCP_NOREQUEST;
			}
			else if (ServiceId == DCP_HELLO)
			{ 
				if (FrameId1 != DCP_FRAMEID_HLO)
					ServiceId = DCP_NOREQUEST;
			}
			else if (FrameId1 != DCP_FRAMEID_URR)
					ServiceId = DCP_NOREQUEST;
		}
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_CheckReq() ChPtr=%X ServiceId=%u", 
						ReqPtr, ServiceId);
	return(ServiceId);
}

/*****************************************************************************/
/* Internal function: dcp_ServerClose()                                      */
/* Process Cancel Indication on CLOSE_CHANNEL.                               */
/*****************************************************************************/
LSA_VOID 	DCP_LOCAL_FCT_ATTR dcp_ServerClose(
DCP_CHA_PTR 			ChPtr)
{
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	DCP_LOWER_RQB_PTR_TYPE  RbPu;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ServerClose() ChPtr=%X QueueCancel.Count=%u QueueInd.Count=%u", 
						ChPtr, ChPtr->QueueCancel.Count, ChPtr->QueueInd.Count);

	/* Remove cancelled lower Receive RQBs from Queue and free it
	*/
	while (ChPtr->QueueCancel.Count != 0)
	{
		RbPu = ChPtr->QueueCancel.FirstPu;
		dcp_DequeLower(&ChPtr->QueueCancel, RbPu, ChPtr);
		dcp_FreeLowerRb(ChPtr, RbPu);
	}

	/* Remove Upper Receive RQBs from Queue and indicate to user
	*/
	while (ChPtr->QueueInd.Count != 0)
	{
		RbPtr = ChPtr->QueueInd.FirstPtr;
		dcp_GetUpperQ(&ChPtr->QueueInd, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
	}
	while (ChPtr->QueueGet.Count != 0)
	{
		RbPtr = ChPtr->QueueGet.FirstPtr;
		dcp_GetUpperQ(&ChPtr->QueueGet, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
	}
	while (ChPtr->QueueSet.Count != 0)
	{
		RbPtr = ChPtr->QueueSet.FirstPtr;
		dcp_GetUpperQ(&ChPtr->QueueSet, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
	}
	while (ChPtr->QueueIdentify.Count != 0)
	{
		RbPtr = ChPtr->QueueIdentify.FirstPtr;
		dcp_GetUpperQ(&ChPtr->QueueIdentify, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
	}
	while (ChPtr->QueueHello.Count != 0)
	{
		RbPtr = ChPtr->QueueHello.FirstPtr;
		dcp_GetUpperQ(&ChPtr->QueueHello, RbPtr);
        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
	}

	if (ChPtr->SendReqCount != 0)		//051207lrg001
	{
		ChPtr->WaitSendCnf = LSA_TRUE;
	}
	else
	{
		/* Proceed closing the channel
		*/
		RbPu.VoidPtr = LSA_NULL;
		dcp_CloseLowerFin(ChPtr, RbPu);
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ServerClose() ChPtr=%X SendReqCount=%d", 
						ChPtr, ChPtr->SendReqCount);
}

/*****************************************************************************/
/* Internal function: dcp_QueueCancel()                                      */
/* Indicate cancelled Upper RQBs from Queue                                  */
/*****************************************************************************/
LSA_INT 	DCP_LOCAL_FCT_ATTR dcp_QueueCancel(
DCP_CHA_PTR 		ChPtr, 
DCP_UPPER_QUEUE	*	QueuePtr,
LSA_INT				CancelCount,
LSA_INT				MaxCount)
{
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	DCP_LOWER_RQB_PTR_TYPE  RbPu;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_QueueCancel() ChPtr=%X QueuePtr=%X Queue.Count=%u", 
						ChPtr, QueuePtr, QueuePtr->Count);

	/* Indicate upper Receive Requests from specified Queue
	*/
	while ((QueuePtr->Count != 0) && (CancelCount < MaxCount))
	{
		if (ChPtr->QueueCancel.Count != 0)
		{
			/* Remove cancelled lower Receive RQB from Queue and...
			*/
			RbPu = ChPtr->QueueCancel.FirstPu;
			dcp_DequeLower(&ChPtr->QueueCancel, RbPu, ChPtr);
			if (ChPtr->DetailPtr->AllocOnRequest)
			{
				/* ...free it if no Recv RBs are configured
				*/
				dcp_FreeLowerRb(ChPtr, RbPu);
			}
			else
			{
				/* ...else put it to the receive Queue
				*/
				dcp_EnqueLower(&ChPtr->ChLowPtr->QueueRecv, RbPu, ChPtr);
			}
		}
		/* Remove Upper Receive RQB from Queue and indicate it to user
		*/
		RbPtr = QueuePtr->FirstPtr;
		dcp_GetUpperQ(QueuePtr, RbPtr);
		CancelCount++;
		if (ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
		else
	        dcp_RequestDone(ChPtr->ChNextPtr, RbPtr, DCP_RSP_OK_CANCEL);
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_QueueCancel() ChPtr=%X Queue.Count=%u CancelCount=%u", 
						ChPtr, QueuePtr->Count, CancelCount);
	return(CancelCount);
}

/*****************************************************************************/
/* Internal access function: dcp_ServerCancel()                              */
/* Process Lower Cancel Confirmation.                                        */
/*****************************************************************************/
LSA_VOID 	DCP_LOCAL_FCT_ATTR dcp_ServerCancel(
DCP_CHA_PTR 			ChPtr, 
DCP_LOWER_RQB_PTR_TYPE  RblPu)
{
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;
	DCP_LOWER_RQB_PTR_TYPE  RbPu;
	LSA_UINT16	Response = DCP_RSP_OK;
	LSA_INT		MaxCount, CancelCount = 0;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ServerCancel() ChPtr=%X RbEthPtr=%X ChState=0x%x", 
						ChPtr, RblPu.RbEthPtr, ChPtr->State);

	/* Store Lower Cancel RQB as Open RQB in channel data again
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChLowPtr->OpenPu.VoidPtr, LSA_NULL))
		ChPtr->ChLowPtr->OpenPu = RblPu;

	if (ChPtr->State == DCP_CHA_STATE_CLOSING)
		dcp_ServerClose(ChPtr);
	else
	{
		if (ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
		{
			if (LSA_HOST_PTR_ARE_EQUAL(ChPtr->CancelRbu, LSA_NULL))
			{
				/* No upper Cancel RQB for Confirmation present!?
				*/
				dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_NO_RBU, 
								RblPu.RbEthPtr, sizeof(DCP_ETH_RQB_TYPE));
			}
			else
			{
				/* If an upper Cancel RQB is present...
				*/
				Response = DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr);
				if (DCP_ETH_RSP_OK != Response)
				{
					/* Pass Lower Layer Error in Cancel Confimation to user
					 * 040811lrg:
					ChPtr->CancelRbu->err = DCP_ETH_RQB_ERR_GET(RblPu.RbEthPtr);
					*/
					Response = DCP_RSP_ERR_LOWER_LAYER;
				}
				else
				{
					/* Indicate upper Receive Requests from Service Indication Queues
					*/
					Response = DCP_RSP_OK;
					MaxCount = ChPtr->CancelRbu->args.Cancel.Count;
					CancelCount = dcp_QueueCancel(ChPtr, &ChPtr->QueueGet, CancelCount, MaxCount);
					CancelCount = dcp_QueueCancel(ChPtr, &ChPtr->QueueSet, CancelCount, MaxCount);
					CancelCount = dcp_QueueCancel(ChPtr, &ChPtr->QueueIdentify, CancelCount, MaxCount);

					while (ChPtr->QueueInd.Count != 0)
					{
						/* Indicate all RecvInd RQBs from common Queue
						*/
						RbPtr = ChPtr->QueueInd.FirstPtr;
						dcp_GetUpperQ(&ChPtr->QueueInd, RbPtr);
				        dcp_RequestDone(ChPtr, RbPtr, DCP_RSP_OK_CANCEL);
						CancelCount++;
					}
				}

				/* Reprovide the too much cancelled lower Receive RQBs
				 * to lower Layer again
				*/
				while (ChPtr->QueueCancel.Count != 0)
				{
					RbPu = ChPtr->QueueCancel.FirstPu;
					dcp_DequeLower(&ChPtr->QueueCancel, RbPu, ChPtr);
					dcp_DataRecv(ChPtr, RbPu, 0, 0);
				}
				if (0 != ChPtr->QueueHello.Count)
				{
					/* Start Hello Cancel on FSU channel now
					*/
#ifdef DCP_MESSAGE
					/* todo: handle neg. Response
					*/
#endif /* DCP_MESSAGE */
					ChPtr->CancelRbu->args.Cancel.Count += ChPtr->QueueHello.Count;
					Response = dcp_CancelRecv(ChPtr->ChNextPtr, 0, 0);
					if (DCP_RSP_OK_ACTIVE == Response)
						ChPtr->ChNextPtr->State = DCP_CHA_STATE_CANCEL;
				}
				if (DCP_RSP_OK_ACTIVE != Response)
				{
					/* Confirm Upper Cancel RQB
					*/
					RbPtr = ChPtr->CancelRbu;
					ChPtr->CancelRbu = LSA_NULL;
					ChPtr->State = DCP_CHA_STATE_OPEN;
					RbPtr->args.Cancel.Count = CancelCount;
			        dcp_RequestDone(ChPtr, RbPtr, Response);
				}
			}
		}
		else if (ChPtr->DetailPtr->DevId == DCP_DEV_FSU)
		{
			/* Hello Cancel on FSU channel finshed
			*/
			if (LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChNextPtr, LSA_NULL)
			||  LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChNextPtr->CancelRbu, LSA_NULL))
			{
				/* No Server channel or no upper Cancel RQB for Confirmation!?
				*/
				DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_ServerCancel() ChPtr=%X ChNextPtr=%X No server channel or no cancel RQB", 
								ChPtr, ChPtr->ChNextPtr);
				dcp_ErrorInternal(DCP_MODULE_ID,__LINE__, DCP_ERR_CH_PTR, ChPtr, (LSA_UINT32)ChPtr->ChNextPtr);
			}
			else
			{
				/* If an upper Cancel RQB is present...
				*/
				Response = DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr);
				if (DCP_ETH_RSP_OK != Response)
				{
					/* Pass Lower Layer Error in Cancel Confimation to user
					 * 040811lrg:
					ChPtr->ChNextPtr->CancelRbu->err = DCP_ETH_RQB_ERR_GET(RblPu.RbEthPtr);
					*/
					Response = DCP_RSP_ERR_LOWER_LAYER;

					/* Reprovide the cancelled lower Receive RQBs
					 * to lower Layer again
					*/
					while (ChPtr->QueueCancel.Count != 0)
					{
						RbPu = ChPtr->QueueCancel.FirstPu;
						dcp_DequeLower(&ChPtr->QueueCancel, RbPu, ChPtr);
						dcp_DataRecv(ChPtr, RbPu, 0, 0);
					}
				}
				else
				{
					/* Indicate the upper Receive Requests from Hello Indication Queue
					*/
					Response = DCP_RSP_OK;
					MaxCount = ChPtr->ChNextPtr->QueueHello.Count;
					CancelCount = dcp_QueueCancel(ChPtr, &ChPtr->ChNextPtr->QueueHello, 0, MaxCount);

					/* Reprovide the too much cancelled lower Receive RQBs
					 * to lower Layer again
					*/
					while (ChPtr->QueueCancel.Count != 0)
					{
						RbPu = ChPtr->QueueCancel.FirstPu;
						dcp_DequeLower(&ChPtr->QueueCancel, RbPu, ChPtr);
						dcp_EnqueLower(&ChPtr->ChLowPtr->QueueRecv, RbPu, ChPtr);
					}
				}

				/* Confirm Upper Cancel RQB
				*/
				RbPtr = ChPtr->ChNextPtr->CancelRbu;
				ChPtr->ChNextPtr->CancelRbu = LSA_NULL;
				ChPtr->State = DCP_CHA_STATE_OPEN;
				ChPtr->ChNextPtr->State = DCP_CHA_STATE_OPEN;
		        dcp_RequestDone(ChPtr->ChNextPtr, RbPtr, Response);
			}
		}
		else
		{
			/* Invalid channel DevId
			*/
			dcp_ErrorInternal(DCP_MODULE_ID,__LINE__,DCP_ERR_CH_PTR,ChPtr,ChPtr->DetailPtr->DevId);
		}

	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ServerCancel() ChPtr=%X CancelCount=%d ChState=0x%x", 
						ChPtr, CancelCount, ChPtr->State);
}

/*****************************************************************************/
/* Internal access function: dcp_ServerConf()                                */
/* Process Send confirmation from lower layer.                               */
/*****************************************************************************/
LSA_VOID 	DCP_LOCAL_FCT_ATTR dcp_ServerConf(
DCP_CHA_PTR 			ChPtr, 
DCP_LOWER_RQB_PTR_TYPE  RblPu)
{
	LSA_UINT16	Response = DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr);
	DCP_UPPER_RQB_PTR_TYPE	RbPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ServerConf() ChPtr=%X RbEthPtr=%X Response=0x%x", 
						ChPtr, RblPu.RbEthPtr, Response);

	if (ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	{
		/* Get Upper RQB pointer from UserId
		 * and read Lower Layer Response Code
		 * 270109lrg001: DCP_UPPER_RQB_PTR_TYPE
		*/
		RbPtr = (DCP_UPPER_RQB_PTR_TYPE)DCP_ETH_RQB_USER_PTR_GET(RblPu.RbEthPtr);

		/* If no Send RBs are configured...
		*/
		if (ChPtr->DetailPtr->AllocOnRequest)
		{
			/* ...free Request block
			*/
			dcp_FreeLowerRb(ChPtr, RblPu);
		}
		else
		{
			/* ...else put RB into lower Queue
			*/
			dcp_EnqueLower(&ChPtr->QueueLower, RblPu, ChPtr);
		}

		if (!LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL))
		{
			/* If the Response is not sent by dcp_UnknownService():
			 * look for Upper RQB-Ptr in the Response Queue
			*/
			if (dcp_FindUpper(&ChPtr->QueueRsp, RbPtr))
			{
				/* Confirm Upper SendRsp RQB
				*/
				if (DCP_ETH_RSP_OK != Response)
				{
					/* Pass Lower Layer Error frm Cancel Confimation to user
					 * 040811lrg:
					RbPtr->err = DCP_ETH_RQB_ERR_GET(RblPu.RbEthPtr);
					*/
					Response = DCP_RSP_ERR_LOWER_LAYER;
				}
				dcp_GetUpperQ(&ChPtr->QueueRsp, RbPtr);
		        dcp_RequestDone(ChPtr, RbPtr, Response);
			}
			else
			{
				/* No upper SendRsp RQB for Confirmation present!?
				*/
				dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_NO_RBU, 
								RblPu.RbEthPtr, sizeof(DCP_ETH_RQB_TYPE));
			}
		}
		if (ChPtr->WaitSendCnf && (ChPtr->SendReqCount == 0))	//051207lrg001
		{
			/* All Confirmations got: proceed closing channel
			*/
			RblPu.VoidPtr = LSA_NULL;
			dcp_CloseLowerFin(ChPtr, RblPu);
		}
	}
	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ServerConf() ChPtr=%X RbPtr=%X SendReqCount=%d", 
						ChPtr, RbPtr, ChPtr->SendReqCount);
}

/*****************************************************************************/
/* Internal access function: dcp_ServerInd()                                 */
/* Start Service by lower Receive Indication.                                */
/*****************************************************************************/
LSA_VOID 	DCP_LOCAL_FCT_ATTR dcp_ServerInd(
DCP_CHA_PTR 			ChPtr, 
DCP_LOWER_RQB_PTR_TYPE  RblPu,
LSA_UINT32				RequestCount)
{
	DCP_UPPER_RQB_PTR_TYPE	 RbPtr;
	DCP_LOWER_RXMEM_PTR_TYPE RxMemPtr;
#ifndef DCP_REQ_LOCAL_DATA
	DCP_ETH_FRAME		dcp_LocalFrame;
#endif
	LSA_UINT8		*	BytePtr;
	DCP_ETHDR		*	HdrPtr;
	DCP_REQ_PTR			ReqPtr;
	LSA_INT				ReqLen;
	LSA_UINT8 			ServiceId;
	LSA_UINT8 			FrameId0;
	LSA_UINT8 			FrameId1;
	LSA_UINT16			DataLength;  
	LSA_UINT16			DataOffset;  
	LSA_UINT16			Response;  
	DCP_ETH_RECV_PTR	EthFramePtr;
	LSA_USER_ID_TYPE	UserId;
	LSA_BOOL			RecvAgain = LSA_FALSE;
	LSA_UINT16 			RetVal = LSA_RET_OK;
	DCP_CHA_PTR 		ChSrvPtr = ChPtr; 

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ServerInd() ChPtr=%X RbEthPtr=%X RequestCount=%u", 
						ChPtr, RblPu.RbEthPtr, RequestCount);

	if (ChPtr->DetailPtr->DevId == DCP_DEV_FSU)
		ChSrvPtr = ChPtr->ChNextPtr;
	if ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
	||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		Response = DCP_ETH_RQB_RESPONSE_GET(RblPu.RbEthPtr);
		if (DCP_ETH_RSP_OK != Response)
		{
			/* Lower Layer Error in Receive-RQB
			*/
			if (DCP_ETH_RSP_CANCEL == Response)
			{
				/* Put canceled RQB to Queue:
				 * will be processed at lower Cancel Indication
				*/
				dcp_EnqueLower(&ChPtr->QueueCancel, RblPu, ChPtr);
				RblPu.RbEthPtr = LSA_NULL;
			}
			else if (ChSrvPtr->QueueInd.Count != 0)
			{
				/* Indicate first Upper RecvInd RQB fom Indication Queue 
				 * with DCP_RSP_ERR_LOWER_LAYER.
				*/
				RbPtr = ChSrvPtr->QueueInd.FirstPtr;
				dcp_GetUpperQ(&ChSrvPtr->QueueInd, RbPtr);

//040811lrg		RbPtr->err = DCP_ETH_RQB_ERR_GET(RblPu.RbEthPtr);
				Response = DCP_RSP_ERR_LOWER_LAYER;
		        dcp_RequestDone(ChSrvPtr, RbPtr, Response);
			}
			else
			{
				/* No upper RQB for Error Indication !?
				*/
				dcp_ErrorLower(DCP_MODULE_ID, __LINE__, DCP_ERR_RECEIVE, 
								RblPu.RbEthPtr, sizeof(DCP_ETH_RQB_TYPE));
			}
		}
		else
		{
			/* Check DCP PDU // 120913lrg740218 moved to dcp_request_lower_eth_done()
			*/
			EthFramePtr = (DCP_ETH_RECV_PTR)DCP_ETH_RQB_RECV_PTR_GET(RblPu.RbEthPtr);
			DataLength = DCP_ETH_RQB_RECV_DATA_LEN_GET(RblPu.RbEthPtr);
			DataOffset = DCP_ETH_RQB_RECV_DATA_OFFSET_GET(RblPu.RbEthPtr);
			
			/* Copy Ethernetframe to local Structure
			*/
			RxMemPtr.RxEthPtr = EthFramePtr;
			DCP_COPY_RXETH_TO_LOCAL_MEM(RxMemPtr.RxEthPtr, &dcp_LocalFrame, 
										(LSA_UINT16)(DataOffset+DataLength), ChPtr->SysPtr);
			HdrPtr = &dcp_LocalFrame.Hdr;
			BytePtr = (LSA_UINT8*)HdrPtr + DataOffset;
			FrameId0 = *BytePtr++;
			FrameId1 = *BytePtr++;
			ReqPtr = (DCP_REQ_PTR)BytePtr;
			ReqLen = DCP_SWAP16(ReqPtr->Hdr.Length) + DCP_HEADER_SIZE;

			/* Decode Request PDU
			*/ 
			ServiceId = dcp_CheckReq(ReqPtr, DataLength, FrameId0, FrameId1, ChPtr);

			switch (ServiceId)
			{
			/* Service not supported: send back error response
			*/ 
			default:
				DCP_ERROR_TRACE_BYTE_ARRAY(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_ServerInd() Unknown service or frame id:", 
											dcp_LocalFrame.Hdr.DstMacAdr, DataOffset+DataLength);
				if (ChPtr->DetailPtr->DevId == DCP_DEV_ETH)
					RetVal = dcp_UnknownService(ReqPtr, ReqLen, HdrPtr, ChPtr);
				RetVal = DCP_RET_ERR_PARAM;
				break;

			/* No DCP Request: ignore it
			*/ 
			case DCP_NOREQUEST:
				DCP_ERROR_TRACE_BYTE_ARRAY(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_ServerInd() Unknown DCP request:", 
											dcp_LocalFrame.Hdr.DstMacAdr, DataOffset+DataLength);
				RetVal = DCP_RET_ERR_PARAM;
				break;

			/* Process Request
			*/
			case DCP_IDENTIFY:
				RetVal = dcp_Identify(ReqPtr, ReqLen, HdrPtr, ChSrvPtr);
				break;

			case DCP_GET:
			case DCP_SET:
				if (DCP_CMP_MAC_ADDR(HdrPtr->DstMacAdr, ChPtr->ChLowPtr->LocalMacAdr))
				{
					/* Process Unicast Request only, if destination MAC Address is equal to the
					 * local Interface MAC Address and the Server is not locked for a
					 * Multi Request Transaction by another Client.
					*/
					if (dcp_LockOnClient(HdrPtr, ChSrvPtr))
					{
						if (ServiceId == DCP_GET)
							RetVal = dcp_Get(ReqPtr, ReqLen, HdrPtr, ChSrvPtr);
						else
							RetVal = dcp_Set(ReqPtr, ReqLen, HdrPtr, ChSrvPtr);
					}
					else
					{
						RetVal = LSA_RET_ERR_SEQUENCE;
					}
				}
				else
				{
					DCP_ERROR_TRACE_BYTE_ARRAY(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_ServerInd() Local MAC Addr:", ChSrvPtr->ChLowPtr->LocalMacAdr, 6);
					DCP_ERROR_TRACE_BYTE_ARRAY(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_ServerInd() Dest. MAC Addr:", HdrPtr->DstMacAdr, 6);
					RetVal = DCP_RET_ERR_PARAM;
				}
				break;

			case DCP_HELLO:
				RetVal = DCP_RET_ERR_PARAM;
				if (ChPtr->DetailPtr->DevId == DCP_DEV_FSU)
				{
					RetVal = dcp_Hello(ReqPtr, ReqLen, HdrPtr, DCP_ETH_RQB_RECV_FFINFO_GET(RblPu.RbEthPtr), ChSrvPtr);
				}
				else
				{
					DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_ServerInd() ChPtr=%X ChNextPtr=%X Hello not on FSU channel", 
								ChPtr, ChPtr->ChNextPtr);
				}
				break;
			}

			if (RetVal != LSA_RET_OK)
			{
				RecvAgain = LSA_TRUE;
				DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_ServerInd() ChPtr=%X ServiceId=%u Service not OK: 0x%x", 
								ChPtr, ServiceId, RetVal);
				DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, " -  dcp_ServerInd() QueueRecv.Count=%u RequestCount=%u RecvRblAct=%d RecvRblClient=%d", 
								ChPtr->ChLowPtr->QueueRecv.Count, RequestCount, ChPtr->ChLowPtr->RecvRblAct, ChPtr->ChLowPtr->RecvRblClient);
			}
		}

		if (!LSA_HOST_PTR_ARE_EQUAL(RblPu.RbEthPtr, LSA_NULL))
		{
			/* 120105lrg001: Reprovide RQB only if there are 
			 * less RQBs in the lower layer than needed for the 
			 * actual Count of Indication-RQBs in the queues.
			*/
			if ((RecvAgain)
			&&  ((RequestCount == 0)
			 ||  ((ChPtr->DetailPtr->DevId == DCP_DEV_FSU) && (RequestCount < ChSrvPtr->QueueHello.Count))
			 ||  ((ChPtr->DetailPtr->DevId == DCP_DEV_ETH) && (RequestCount < (ChSrvPtr->QueueInd.Count \
			 																+ ChSrvPtr->QueueIdentify.Count \
			 																+ ChSrvPtr->QueueGet.Count \
			 																+ ChSrvPtr->QueueSet.Count)))))
			{
				/* Reprovide Lower Receive if no or less than 
				 * Server Indication Requests are in the lower layer.
				 * 140105lrg001: with unchanged UserId
				*/
				UserId.uvar32 = DCP_ETH_RQB_USERID_UVAR32_GET(RblPu.RbEthPtr);
				if (UserId.uvar8_array[DCP_CLIENT_UID] != 0)
					ChPtr->ChLowPtr->RecvRblClient--;
				dcp_DataRecv(ChPtr,	RblPu, UserId.uvar8_array[DCP_INDEX_UID], 
							UserId.uvar8_array[DCP_CLIENT_UID]);
			}
			else if (!ChPtr->DetailPtr->AllocOnRequest)
			{
				/* Lower Receive RQB back to Queue
				*/
				dcp_EnqueLower(&ChPtr->ChLowPtr->QueueRecv, RblPu, ChPtr);
			}
			else
			{
				/* Free Lower Receive RQB
				*/
				dcp_FreeLowerRb(ChPtr, RblPu);
			}
		}
	}
	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ServerInd() ChPtr=%X RetVal=0x%x", 
						ChPtr, RetVal);
}

/*****************************************************************************/
/*  end of file DCP_SRV.C                                                    */
/*****************************************************************************/
