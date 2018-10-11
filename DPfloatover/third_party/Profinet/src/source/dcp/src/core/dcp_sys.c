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
/*  F i l e               &F: dcp_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System-Interface                                                         */
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
/*  2003-02-03  P01.00.00.00_00.03.00.00  lrg  3.Increment:                  */
/*              New: dcp_ChClean(), dcp_TimerClean()                         */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*  2003-05-05  P01.00.00.00_00.05.00.00  lrg  5.Increment: SHOW_LOCATION    */
/*  2003-06-05  P01.00.00.00_00.06.00.00  lrg  6.Increment:                  */
/*              dcp_FatalError() removed                                     */
/*  2003-06-16  P01.00.00.00_00.06.00.02  lrg  6.Increment:                  */
/*              dcp_timeout() TimerPtr mit LSA_NULL initialisiert            */
/*  2003-07-30  P01.00.00.00_00.08.00.00  lrg  8.Increment: Client channel   */
/*              Bei Identify-Filter DetailefType und SequenceNumber          */
/*              im PT2_MODE nicht antworten.                                 */
/*  2003-09-29  P01.00.00.00_00.11.00.00  lrg 11.Increment: Mehrere IDENTIFY */
/*              gleichzeitig durch Timer-Array.                              */
/*  2003-10-10  P01.00.00.00_00.11.00.01 lrg Fehlerbehebung:                 */
/*              dcp_TraceIn(),dcp_TraceOut(),dcp_TraceSR(): LSA_UNUSED_ARG() */
/*              Index vor dem Vergleich mit der Feldlaenge erhoehen          */
/*  2003-12-15  P02.00.00.00_00.01.00.00 dsc  New version of timeout hand-   */
/*              ling introduced: DCP_DO_TIMEOUT_EXTERN                       */
/*  2004-01-30  P02.00.00.00_00.02.00.00 lrg ENTER/EXIT in dcp_timeout_sys() */
/*  2004-02-12  P02.00.00.00_00.02.00.01 lrg dcp_TraceSR() ChPtr = LSA_NULL  */
/*  2004-03-03  P02.00.00.00_00.02.00.02 lrg dcp_init():                     */
/*              SysPtr nicht initialisieren                                  */
/*  2004-04-22  P02.00.00.00_00.02.05.02 lrg 220404lrg002: Bei Aufruf der    */
/*              Timeout-Eingangsfunktionen nach close_channel() statt FATAL_ */
/*              ERROR() nur ERROR-Trace                                      */
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg  Version 3                     */
/*  2004-10-06  P03.02.00.00_00.02.00.00 061004lrg001 Die xid in den Client- */
/*              Channelstrukturen wird nur noch in dcp_init() initialisiert, */
/*              damit in einer OPEN/IDENTIFY/CLOSE/OPEN/IDENTIFY-Sequenz     */
/*              nicht in beiden IDENTIFY-Requests die gleiche Transaction-ID */
/*              benutzt wird.                                                */
/*             	061004lrg002 Timer-RQB durch die UserID 0 in der Argument-   */
/*              Struktur als unbenutzt markieren.                            */
/*  2004-10-13  P03.02.00.00_00.02.00.01  lrg  High- und Low-Byte der MAC-   */
/*              Adresse zur Berechnung der Faltungskonstanten vertauscht:    */
/*              DCP_MAC_RAND_IDX4 (4) wird zu DCP_MAC_RAND_IDX_LOW (5) und   */
/*              DCP_MAC_RAND_IDX5 (5) wird zu DCP_MAC_RAND_IDX_HIGH (4).     */
/*  2004-12-17  P03.02.00.00_00.02.02.00  lrg  dcp_FindUpper(): return()     */
/*              hinter TRACE. dcp_InitTimerRqb():                            */
/*              DCP_UPPER_RQB_USERID_SET() --> DCP_UPPER_RQB_USER32_SET()    */
/*  2004-12-21  P03.02.00.00_00.02.02.01  lrg  dcp_ChJoin(): wegen           */
/*              AllocOnRequest #ifdef DCP_CLIENT_MODE #else #endif eingefügt */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  Copyright-Text                */
/*  2005-01-13  P03.03.00.00_00.03.02.00  130105lrg001: dcp_ChJoin()         */
/*              Rbl-Zaehler initialisieren                                   */
/*  2005-02-16  P03.04.00.00_00.04.00.00  lrg  LSA-Trace eingebaut           */
/*              LSA_UNUSED_ARG() fuer Parameter, die ohne Trace unused sind  */
/*  2005-04-15  P03.05.00.00_00.04.00.01  150405lrg001: Free Timer-RQB       */
/*                                        if channel is closed               */
/*  2006-06-30  P03.12.00.00_00.01.01.02  lrg  LSA_GLOB 2.1.0.0_0.7.2.1      */
/*  2007-03-07  P04.00.00.00_00.01.01.02  070307lrg001: Neuer Dienst: HELLO  */
/*              ArtsRQ AP00417455: Enter/Exit in dcp_timeout_sys() entfernt  */
/*  2007-04-24  P04.00.00.00_00.10.01.02 240407lrg001:DCP_MAX_LOW_CHANNELS*2 */
/*  2007-09-12  P04.00.00.00_00.30.01.02 120907lrg001: Nicht mehr FATAL_ERROR*/
/*              in gsy_TimerStop() bei TIMER_NOT_RUNNING: kann bei DCP_DO_   */
/*              TIMEOUT_EXTERN passieren wenn Timer-RQB schon unterwegs ist. */
/*  2007-12-05  P04.00.00.00_00.60.01.02 051207lrg001: ArtsRQ AP00552010     */
/*              Beim CloseChannel auf Sendeconfirmations warten.             */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2009-03-03  P04.01.00.00_00.03.01.02 030309lrg001: ArtsRQ AP00791553     */
/*              "Zweiter Client-Kanal für DCP führt zu Fehler in dcp_sys.c"  */
/*  2010-08-24  P05.00.00.00_00.01.01.02 240810lrg001: AP00999022 UPPER_DONE */
/*  2011-01-18  P05.00.00.00_00.03.12.02 lrg: AP01017495 Index-Trace         */
/*  2011-06-29  S05.01.00.00_00.03.03.02 lrg: AP01138397 DCP_xxx_TIMER_COUNT */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: cast                     */
/*  2011-10-17  T05.01.00.00_00.04.17.02 AP01257857: LTRC_... -> LSA_...     */
/*  2012-01-11  T05.01.00.00_00.05.13.01                                     */
/*              AP01297962: remove DCP_CLIENT_MODE and DCP_SERVER_MODE       */
/*  2012-05-10  P05.00.00.00_00.05.93.02 AP01349915: check for ch open state */
/*  2012-06-12  P05.01.00.00_00.05.44.02 AP01368289: LSA trace               */
/*  2013-03-04  P05.02.00.00_00.04.17.02 AP01503994: free upper 2 bit of XID */
/*              DCP_MAX_CHANNELS = 63                                        */
/*  2013-05-16  P05.03.00.00_00.01.10.01 AP01509782 DCP channel details und  */
/*              Konfigurationsschalter bereinigen                            */
/*  2013-06-17  P05.03.00.00_00.01.10.01 AP01387412 DCP Timer Optimierung    */
/*  2013-09-20  P05.03.00.00_00.02.14.01 742724 - DCP ASU Ident timeout takes*/
/*              600ms when DCP_DO_TIMEOUT_EXTERN is not defined              */
/*  2013-10-29  P05.03.00.00_00.02.17.01 836880 - DCP Endlosschleife bei     */
/*              gleichzeitiger Nutzung von 10ms-Timern und 100ms-Timern      */
/*  2014-05-26  P05.04.00.00_00.01.09.01 1027120 Korrektur der PSI-Konfigu-  */
/*              ration im DPC bezüglich DCP_DO_TIMEOUT_EXTERN                */
/*  2014-06-25  P05.03.00.00_00.03.09.01 1064327: ServerSetWait (WACK)       */
/*  2014-08-25  P05.03.00.00_00.03.18.01 1104556: Undo ServerSetWait (WACK)  */
/*  2014-09-03  P05.04.00.00_00.01.22.01 1089622: dcp_TimerTabExec() nicht   */
/*              im Timer-Kontext ausführen                                   */
/*  2015-05-04  P05.04.00.00_00.02.00.26 1305962/1308196: Remove setting     */
/*              UserId to zero                                               */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */
 
/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID	1		/* DCP_MODULE_ID_SYS */
#define DCP_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "dcp_inc.h"
#include "dcp_int.h"
DCP_FILE_SYSTEM_EXTENSION(DCP_MODULE_ID)

/*****************************************************************************/
/* External LSA access function: dcp_init()                                  */
/* Initialise data structures                                                */
/*****************************************************************************/
LSA_UINT16  DCP_SYSTEM_IN_FCT_ATTR  dcp_init(
LSA_VOID)
{
	LSA_HANDLE_TYPE     ChId;
	DCP_CHA_PTR  		ChPtr;
	LSA_UINT16          RetVal = LSA_RET_OK;

	DCP_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "--> dcp_init() &dcp_data=%x DCP_MAX_LOW_CHANNELS=%d DCP_MAX_CHANNELS=%d", 
						&dcp_data, DCP_MAX_LOW_CHANNELS, DCP_MAX_CHANNELS);

	DCP_MEMSET_LOCAL(&dcp_data, 0x00, sizeof(dcp_data));

	/* Always Use static channel tables
	*/
	dcp_data.ChListPtr = dcp_data.ChList;
	dcp_data.ChLowListPtr = dcp_data.ChLowList;

	if (DCP_MAX_CHANNELS > 63)	//AP01503994
	{
		RetVal = LSA_RET_ERR_RESOURCE;
		DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_init() DCP_MAX_CHANNELS=%d (>63)", DCP_MAX_CHANNELS);
	}
	/* Initialise data:
	*/
	if(RetVal == LSA_RET_OK)
	{
		/* LowerChannel structures:
		*/
		for (ChId = 0; ChId < DCP_MAX_LOW_CHANNELS*2; ChId++)	//240407lrg001: *2 wegen FSU-Channel
		{
			dcp_data.ChLowList[ChId].ChServerPtr = LSA_NULL;
			dcp_data.ChLowList[ChId].ChClientPtr = LSA_NULL;
			dcp_data.ChLowList[ChId].OpenPu.VoidPtr = LSA_NULL;
			dcp_data.ChLowList[ChId].State = DCP_CHA_STATE_FREE;
			dcp_data.ChLowList[ChId].Handle = ChId + 1;
			dcp_data.ChLowList[ChId].QueueRecv.FirstPu.VoidPtr = LSA_NULL;
			dcp_data.ChLowList[ChId].QueueRecv.LastPu.VoidPtr  = LSA_NULL;
		}

		/* UpperChannel structures:
		*/
		for(ChId = 0; ChId < DCP_MAX_CHANNELS; ChId++)
		{
			ChPtr = (DCP_CHA_PTR)(dcp_data.ChListPtr + ChId);

			/* Handles 1...DCP_MAX_CHANNELS
			*/
			ChPtr->State 	 = DCP_CHA_STATE_FREE;
			ChPtr->Handle    = ChId + 1;
//tbd		ChPtr->CancelUid  = DCP_NOSERVICE;
			ChPtr->CancelRbu = LSA_NULL;
			ChPtr->DetailPtr = LSA_NULL;
			ChPtr->CbfPtr    = LSA_NULL;
			ChPtr->ChLowPtr  = LSA_NULL;
			ChPtr->ChNextPtr = LSA_NULL;
			ChPtr->QueueUpper.FirstPtr = LSA_NULL;
			ChPtr->QueueUpper.LastPtr  = LSA_NULL;
			ChPtr->QueueRetry.FirstPtr = LSA_NULL;
			ChPtr->QueueRetry.LastPtr  = LSA_NULL;
			ChPtr->QueueRsp.FirstPtr = LSA_NULL;
			ChPtr->QueueRsp.LastPtr  = LSA_NULL;
			ChPtr->QueueInd.FirstPtr = LSA_NULL;
			ChPtr->QueueInd.LastPtr  = LSA_NULL;
			ChPtr->QueueGet.FirstPtr = LSA_NULL;
			ChPtr->QueueGet.LastPtr  = LSA_NULL;
			ChPtr->QueueSet.FirstPtr = LSA_NULL;
			ChPtr->QueueSet.LastPtr  = LSA_NULL;
			ChPtr->QueueHello.FirstPtr = LSA_NULL;
			ChPtr->QueueHello.LastPtr  = LSA_NULL;
			ChPtr->QueueIdentify.FirstPtr = LSA_NULL;
			ChPtr->QueueIdentify.LastPtr  = LSA_NULL;
			ChPtr->QueueLower.FirstPu.VoidPtr = LSA_NULL;
			ChPtr->QueueLower.LastPu.VoidPtr  = LSA_NULL;
			ChPtr->QueueCancel.FirstPu.VoidPtr = LSA_NULL;
			ChPtr->QueueCancel.LastPu.VoidPtr  = LSA_NULL;
			ChPtr->TimerQueue.FirstPtr = LSA_NULL;
			ChPtr->TimerQueue.LastPtr  = LSA_NULL;
			ChPtr->TimerPtr = LSA_NULL;
			ChPtr->Identify.QueueUpper.FirstPtr = LSA_NULL;
			ChPtr->Identify.QueueUpper.LastPtr  = LSA_NULL;
			ChPtr->Identify.QueueNotify.FirstPtr = LSA_NULL;
			ChPtr->Identify.QueueNotify.LastPtr  = LSA_NULL;
			ChPtr->Identify.QueueSend.FirstPu.VoidPtr = LSA_NULL;
			ChPtr->Identify.QueueSend.LastPu.VoidPtr  = LSA_NULL;
			ChPtr->Hello.QueueUpper.FirstPtr = LSA_NULL;
			ChPtr->Hello.QueueUpper.LastPtr  = LSA_NULL;
			ChPtr->Hello.QueueNotify.FirstPtr = LSA_NULL;
			ChPtr->Hello.QueueNotify.LastPtr  = LSA_NULL;
			ChPtr->Hello.QueueSend.FirstPu.VoidPtr = LSA_NULL;
			ChPtr->Hello.QueueSend.LastPu.VoidPtr  = LSA_NULL;
			ChPtr->Set.QueueUpper.FirstPtr = LSA_NULL;
			ChPtr->Set.QueueUpper.LastPtr  = LSA_NULL;
			ChPtr->Set.QueueNotify.FirstPtr = LSA_NULL;
			ChPtr->Set.QueueNotify.LastPtr  = LSA_NULL;
			ChPtr->Set.QueueSend.FirstPu.VoidPtr = LSA_NULL;
			ChPtr->Set.QueueSend.LastPu.VoidPtr  = LSA_NULL;
			ChPtr->Get.QueueUpper.FirstPtr = LSA_NULL;
			ChPtr->Get.QueueUpper.LastPtr  = LSA_NULL;
			ChPtr->Get.QueueNotify.FirstPtr = LSA_NULL;
			ChPtr->Get.QueueNotify.LastPtr  = LSA_NULL;
			ChPtr->Get.QueueSend.FirstPu.VoidPtr = LSA_NULL;
			ChPtr->Get.QueueSend.LastPu.VoidPtr  = LSA_NULL;
		}
		dcp_data.Timer100ms.Queue.FirstPtr = LSA_NULL;
		dcp_data.Timer100ms.Queue.LastPtr = LSA_NULL;
		dcp_data.Timer100ms.RbuPtr = LSA_NULL;
		dcp_data.Timer10ms.Queue.FirstPtr = LSA_NULL;
		dcp_data.Timer10ms.Queue.LastPtr = LSA_NULL;
		dcp_data.Timer10ms.RbuPtr = LSA_NULL;

		/* Allocate LSA timers
		*/
		RetVal = dcp_TimerTabInit();
		if (LSA_RET_OK != RetVal)
		{
			dcp_TimerTabExit();
		}
	}
	DCP_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "<-- dcp_init() RetVal=%d", RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* External LSA access function: dcp_undo_init()                             */
/* Undo initialising                                                         */
/*****************************************************************************/
LSA_UINT16  DCP_SYSTEM_IN_FCT_ATTR  dcp_undo_init(
LSA_VOID)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	DCP_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "--> dcp_undo_init() &dcp_data=%x", 
						&dcp_data);

	/* Using static channel data structures - nothing to do
	*/

	/* Stop and free LSA timers
	*/
	dcp_TimerTabExit();

	DCP_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<-- dcp_undo_init() &dcp_data=%x RetVal=%d", 
						&dcp_data, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Function: dcp_TimerTabInit()                                              */
/* Allocate and start client/server LSA timers.                              */
/*****************************************************************************/
LSA_UINT16  DCP_LOCAL_FCT_ATTR  dcp_TimerTabInit(LSA_VOID)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	DCP_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabInit()"); 

	if (0 == dcp_data.Timer100ms.State)
	{
		/* Allocate cyclic 100ms LSA timer
		*/
		DCP_ALLOC_TIMER(&RetVal, &dcp_data.Timer100ms.Id, DCP_TIMER_TYPE_CYCLIC, DCP_TIME_BASE_100MS);
		if (LSA_RET_OK == RetVal)
		{
			dcp_data.Timer100ms.State = 1;
			dcp_data.Timer100ms.RbuPtr = LSA_NULL;
			dcp_data.Timer100ms.RbuInUse = 0;

#ifdef DCP_DO_TIMEOUT_EXTERN
			/* Allocate timer RQB
			*/
			DCP_ALLOC_UPPER_RQB_LOCAL(&dcp_data.Timer100ms.RbuPtr, sizeof(DCP_RQB_TYPE));
			if (!LSA_HOST_PTR_ARE_EQUAL(dcp_data.Timer100ms.RbuPtr, LSA_NULL))
			{
				/* Initialise timer request RQB
				*/
				DCP_UPPER_RQB_OPCODE_SET(dcp_data.Timer100ms.RbuPtr, DCP_OPC_TIMER);
				DCP_UPPER_RQB_RESPONSE_SET(dcp_data.Timer100ms.RbuPtr, DCP_RSP_OK_ACTIVE);
				DCP_UPPER_RQB_PREV_SET(dcp_data.Timer100ms.RbuPtr, LSA_NULL);
				DCP_UPPER_RQB_NEXT_SET(dcp_data.Timer100ms.RbuPtr, LSA_NULL);

				dcp_data.Timer100ms.RbuPtr->args.Timer.UserID.void_ptr = LSA_NULL;
			}
			else
			{
				RetVal = LSA_RET_ERR_RESOURCE;
			}
#endif
			DCP_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "--- dcp_TimerTabInit() Timer100ms.Id=%u Timer100ms.RbuPtr=%x",
					dcp_data.Timer100ms.Id, dcp_data.Timer100ms.RbuPtr);
		}
	}
	else
	{
		RetVal = LSA_RET_ERR_SEQUENCE;
		DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabInit() Timer100ms.State=%u", 
						dcp_data.Timer100ms.State);
	}
#ifdef DCP_CLIENT_HELLO_ENABLE
	
	if (LSA_RET_OK == RetVal)
	{
		if (0 == dcp_data.Timer10ms.State)
		{
			/* Allocate cyclic 10ms LSA timer
			*/
			DCP_ALLOC_TIMER(&RetVal, &dcp_data.Timer10ms.Id, DCP_TIMER_TYPE_CYCLIC, DCP_TIME_BASE_10MS);
			if (LSA_RET_OK == RetVal)
			{
				dcp_data.Timer10ms.State = 1;
				dcp_data.Timer10ms.RbuPtr = LSA_NULL;
				dcp_data.Timer10ms.RbuInUse = 0;

#ifdef DCP_DO_TIMEOUT_EXTERN
				/* Allocate timer RQB
				*/
				DCP_ALLOC_UPPER_RQB_LOCAL(&dcp_data.Timer10ms.RbuPtr, sizeof(DCP_RQB_TYPE));
				if (!LSA_HOST_PTR_ARE_EQUAL(dcp_data.Timer10ms.RbuPtr, LSA_NULL))
				{
					/* Initialise timer request RQB
					*/
					DCP_UPPER_RQB_OPCODE_SET(dcp_data.Timer10ms.RbuPtr, DCP_OPC_TIMER);      
					DCP_UPPER_RQB_RESPONSE_SET(dcp_data.Timer10ms.RbuPtr, DCP_RSP_OK_ACTIVE);
					DCP_UPPER_RQB_PREV_SET(dcp_data.Timer10ms.RbuPtr, LSA_NULL);
					DCP_UPPER_RQB_NEXT_SET(dcp_data.Timer10ms.RbuPtr, LSA_NULL);

					dcp_data.Timer10ms.RbuPtr->args.Timer.UserID.void_ptr = LSA_NULL;
				}
				else
				{
					RetVal = LSA_RET_ERR_RESOURCE;
				}
#endif //DCP_DO_TIMEOUT_EXTERN
				DCP_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "--- dcp_TimerTabInit() Timer10ms.Id=%u Timer10ms.RbuPtr=%X",
						dcp_data.Timer10ms.Id, dcp_data.Timer10ms.RbuPtr);
			}
		}
		else
		{
			RetVal = LSA_RET_ERR_SEQUENCE;
			DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabInit() dcp_data.Timer10ms.State=%u", 
						dcp_data.Timer10ms.State);
		}
	}
#endif //DCP_CLIENT_HELLO_ENABLE
	
	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabInit() Timer100ms.State=%u Timer10ms.State=%u RetVal=0x%x", 
			 dcp_data.Timer100ms.State, dcp_data.Timer10ms.State, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Function: dcp_TimerTabExit()                                              */
/* Stop and free LSA timers.                                                 */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerTabExit(LSA_VOID)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	DCP_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabExit() Timer100ms.State=%u Timer10ms.State=%u", 
						dcp_data.Timer100ms.State, dcp_data.Timer10ms.State);

	if (dcp_data.Timer100ms.State > 1)
	{
		/*	Stop 100ms LSA timer
		*/
		DCP_STOP_TIMER(&RetVal, dcp_data.Timer100ms.Id);
		if (LSA_RET_OK != RetVal)
		{
			DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabExit() RetVal=0x%x from DCP_STOP_TIMER(100ms)", 
						RetVal);
		}
		dcp_data.Timer100ms.State = 1;
	}
	if (1 == dcp_data.Timer100ms.State)
	{
		/*	Free 100ms LSA timer
		*/
		DCP_FREE_TIMER(&RetVal, dcp_data.Timer100ms.Id);
		if (LSA_RET_OK != RetVal)
		{
			DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabExit() RetVal=0x%x from DCP_FREE_TIMER(100ms)", 
						RetVal);
		}
#ifdef DCP_DO_TIMEOUT_EXTERN
		if (!LSA_HOST_PTR_ARE_EQUAL(dcp_data.Timer100ms.RbuPtr, LSA_NULL))
		{
			DCP_FREE_UPPER_RQB_LOCAL(&RetVal, dcp_data.Timer100ms.RbuPtr);
			if (LSA_RET_OK != RetVal)
			{
				DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabExit() RetVal=0x%x from DCP_FREE_UPPER_RQB_LOCAL(100ms)", 
								RetVal);
			}
		}
#endif
		dcp_data.Timer100ms.State = 0;
	}
	if (dcp_data.Timer10ms.State > 1)
	{
		/*	Stop 10ms LSA timer
		*/
		DCP_STOP_TIMER(&RetVal, dcp_data.Timer10ms.Id);
		if (LSA_RET_OK != RetVal)
		{
			DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabExit() RetVal=0x%x from DCP_STOP_TIMER(10ms)", 
						RetVal);
		}
		dcp_data.Timer10ms.State = 1;
	}
	if (1 == dcp_data.Timer10ms.State)
	{
		/*	Free 10ms LSA timer
		*/
		DCP_FREE_TIMER(&RetVal, dcp_data.Timer10ms.Id);
		if (LSA_RET_OK != RetVal)
		{
			DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabExit() RetVal=0x%x from DCP_FREE_TIMER(10ms)", 
						RetVal);
		}
#ifdef DCP_DO_TIMEOUT_EXTERN
		if (!LSA_HOST_PTR_ARE_EQUAL(dcp_data.Timer10ms.RbuPtr, LSA_NULL))
		{
			DCP_FREE_UPPER_RQB_LOCAL(&RetVal, dcp_data.Timer10ms.RbuPtr);
			if (LSA_RET_OK != RetVal)
			{
				DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabExit() RetVal=0x%x from DCP_FREE_UPPER_RQB_LOCAL(10ms)", 
							RetVal);
			}
		}
#endif
		dcp_data.Timer10ms.State = 0;
	}

	DCP_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabExit()");
}

/*****************************************************************************/
/* Function: dcp_TimerTabFindId()                                            */
/* Find a timer block in queues of LSA timers by channel handle and TimerId  */
/*****************************************************************************/
DCP_TIMER_PTR_TYPE  DCP_LOCAL_FCT_ATTR  dcp_TimerTabFindId(
DCP_CHA_PTR		ChPtr,
LSA_UINT16		TimerId)
{
	DCP_TIMER_PTR_TYPE	TimerPtr;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabFindId() ChPtr=%X Handle=%u TimerId=%u", 
						ChPtr, ChPtr->Handle, TimerId);

	TimerPtr = dcp_data.Timer10ms.Queue.FirstPtr;
	while (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		if ((TimerPtr->Handle == ChPtr->Handle) && (TimerPtr->Id == TimerId))
		{
			break;
		}
		TimerPtr = TimerPtr->NextPtr;
	}
	if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		TimerPtr = dcp_data.Timer100ms.Queue.FirstPtr;
		while (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
		{
			if ((TimerPtr->Handle == ChPtr->Handle) && (TimerPtr->Id == TimerId))
			{
				break;
			}
			TimerPtr = TimerPtr->NextPtr;
		}
	}
	
	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabFindId() TimerPtr=%X", 
						TimerPtr);
	return(TimerPtr);
}

/*****************************************************************************/
/* Function: dcp_TimerTabFindAddr()                                          */
/* Find a timer block in queues of LSA timers by MAC address & transact. ID  */
/*****************************************************************************/
DCP_TIMER_PTR_TYPE  DCP_LOCAL_FCT_ATTR  dcp_TimerTabFindAddr(
DCP_CHA_PTR		ChPtr,
LSA_UINT32		TransactId,
DCP_MAC_ADR		SrcMacAdr)
{
	DCP_TIMER_PTR_TYPE	TimerPtr;

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabFindAddr() ChPtr=%X Handle=%u TransactId=0x%08x Queue.Count=%u", 
						ChPtr, ChPtr->Handle, TransactId, dcp_data.Timer100ms.Queue.Count);

	TimerPtr = dcp_data.Timer100ms.Queue.FirstPtr;
	while (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		if ((TimerPtr->Handle == ChPtr->Handle) && (TimerPtr->CallId == DCP_TIMER_IDENTIFY) 
		&&  (TimerPtr->Tract.IdentifyData.TransactId == TransactId)
		&&  DCP_CMP_MAC_ADDR(TimerPtr->Tract.IdentifyData.ClientAdr, SrcMacAdr))
		{
			break;
		}
		TimerPtr = TimerPtr->NextPtr;
	}
	
	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabFindAddr() TimerPtr=%X", 
						TimerPtr);
	return(TimerPtr);
}

/*****************************************************************************/
/* Function: dcp_TimerTabAdd()                                               */
/* Add timer block to LSA timer queue and start LSA timer if first           */
/*****************************************************************************/
LSA_UINT16  DCP_LOCAL_FCT_ATTR  dcp_TimerTabAdd(
DCP_TIMER_PTR_TYPE			TimerPtr)
{
	LSA_USER_ID_TYPE	UserId;
	DCP_TIMER_TAB		*TabPtr;
	LSA_UINT16			RetVal = LSA_RET_OK;
	
	DCP_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabAdd() TimerPtr=%X TimerId=%u", 
						TimerPtr, TimerPtr->Id);

	if (LSA_TIME_BASE_10MS == TimerPtr->Base)
	{
		TabPtr = &dcp_data.Timer10ms;
	}
	else
	{
		TabPtr = &dcp_data.Timer100ms;
	}
	if ((0 != TabPtr->Queue.Count) && (DCP_TIMER_IDENTIFY != TimerPtr->CallId))
	{
		/* 836880: LSA timer is running: increment count to avoid too short timeout at client and client hold timers
		*/
		TimerPtr->Ext.CountAct++;
	}
	dcp_TimerEnque(&TabPtr->Queue, TimerPtr);
	if (1 == TabPtr->Queue.Count)
	{
		/* First DCP timer to handle: start LSA timer
		*/
		UserId.void_ptr = TabPtr;
		DCP_START_TIMER(&RetVal, TabPtr->Id, UserId, 1);
		TabPtr->State = 2;
		if (LSA_RET_OK != RetVal)
		{
			DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabAdd() RetVal=0x%x from DCP_START_TIMER()", 
						RetVal);
		}
	}
	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabAdd() RetVal=0x%x Base=%u Queue.Count=%u", 
						RetVal, TimerPtr->Base, TabPtr->Queue.Count);
	return(RetVal);
}

/*****************************************************************************/
/* Function: dcp_TimerTabRemove()                                            */
/* Remove timer block from LSA timer queue and stop LSA timer if empty       */
/*****************************************************************************/
LSA_UINT16  DCP_LOCAL_FCT_ATTR  dcp_TimerTabRemove(
DCP_TIMER_PTR_TYPE			TimerPtr)
{
	DCP_TIMER_TAB	*TabPtr;
	LSA_UINT16		RetVal = LSA_RET_OK;
	
	DCP_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabRemove() TimerPtr=%X TimerId=%u", 
						TimerPtr, TimerPtr->Id);

	if (LSA_TIME_BASE_10MS == TimerPtr->Base)
	{
		TabPtr = &dcp_data.Timer10ms;
	}
	else
	{
		TabPtr = &dcp_data.Timer100ms;
	}
	dcp_TimerDeque(&TabPtr->Queue, TimerPtr);
	if (0 == TabPtr->Queue.Count)
	{
		/* No DCP timer left to handle: stop LSA timer
		*/
		DCP_STOP_TIMER(&RetVal, TabPtr->Id);
		TabPtr->State = 1;
		if (LSA_RET_OK != RetVal)
		{
			DCP_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerTabRemove() RetVal=0x%x from DCP_STOP_TIMER()", 
						RetVal);
		}
	}
	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabRemove() RetVal=0x%x Base=%u Queue.Count=%u", 
						RetVal, TimerPtr->Base, TabPtr->Queue.Count);
	return(RetVal);
}

/*****************************************************************************/
/* Function: dcp_TimerTabStart()                                             */
/* Start an allocated client/server timer slot                               */
/*****************************************************************************/
LSA_UINT16  DCP_LOCAL_FCT_ATTR  dcp_TimerTabStart(
DCP_TIMER_PTR_TYPE								TimerPtr)
{
	LSA_UINT16	RetVal = LSA_RET_ERR_PARAM;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabStart() TimerPtr=%X TimerId=%u Time=%u", 
			TimerPtr, TimerPtr->Id, TimerPtr->Time);

	if (TimerPtr->Ext.InUse && (TimerPtr->Ext.CountAct != 0))
	{
		RetVal = LSA_RET_OK_TIMER_RESTARTED;
	}
	else
	{
		RetVal = LSA_RET_OK;
	}
	TimerPtr->Running = LSA_TRUE;
	TimerPtr->Ext.InUse = LSA_TRUE;
	TimerPtr->Ext.CountStart = TimerPtr->Time * TimerPtr->Ext.Factor; 
	TimerPtr->Ext.CountAct = TimerPtr->Ext.CountStart;
	if(LSA_RET_OK == RetVal)
	{
		RetVal = dcp_TimerTabAdd(TimerPtr);
	}

	DCP_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabStart() RetVal=0x%x Factor=%u: CountStart=%u CountAct=%u", 
						RetVal, TimerPtr->Ext.Factor, TimerPtr->Ext.CountStart, TimerPtr->Ext.CountAct);
	return(RetVal);
}

/*****************************************************************************/
/* Function: dcp_TimerTabStop()                                              */
/* Stop a client/server timer slot                                           */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerTabStop(
DCP_CHA_PTR					ChPtr,
DCP_TIMER_PTR_TYPE			TimerPtr,
LSA_UINT16					*RetValPtr)
{
	LSA_UINT16		RetVal = LSA_RET_ERR_PARAM;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerTabStop() ChPtr=%X TimerPtr=%X TimerId=%u", 
						ChPtr, TimerPtr, TimerPtr->Id);

	if (TimerPtr->Ext.InUse)
	{
		/* Remove timer block from LSA timer queue
		*/
		RetVal = dcp_TimerTabRemove(TimerPtr);
		TimerPtr->Ext.InUse = LSA_FALSE;

		DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- dcp_TimerTabStop() TimerId=%u CountAct=%u", 
						TimerPtr->Id, TimerPtr->Ext.CountAct);
		if (TimerPtr->Ext.CountAct == 0)
		{
			RetVal = LSA_RET_OK_TIMER_NOT_RUNNING;
		}
		else
		{
			RetVal = LSA_RET_OK;
		}
		TimerPtr->Ext.CountStart = 0; 
		TimerPtr->Ext.CountAct = 0;
	}

	*RetValPtr = RetVal;
	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerTabStop() RetVal=0x%x", 
						*RetValPtr);
}

/*****************************************************************************/
/* Function: dcp_TimerTabExec()                                              */
/* Execute LSA timer:                                                        */
/* call inernal timer function dcp_timeout_int() for every elapsed DCP timer */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerTabExec(
LSA_UINT16										TimerNo,
DCP_TIMER_TAB*									TabPtr)
{
	DCP_TIMER_PTR_TYPE 	TimerPtr, NextPtr;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "  > dcp_TimerTabExec() TimerNo=%u TabPtr=%X Queue.Count=%u", 
			TimerNo, TabPtr, TabPtr->Queue.Count);

	/* Check all DCP internal timer in queue for timeout
	*/
	TimerPtr = TabPtr->Queue.FirstPtr;
	while (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		/* 742724 - DCP ASU Ident timeout takes 600ms when DCP_DO_TIMEOUT_EXTERN is not defined
		* Load NextPtr before dcp_TimerTabRemove() in dcp_timeout_int()
		*/
		NextPtr = TimerPtr->NextPtr;
		if ((TimerPtr->Ext.InUse)
		&&  (TimerPtr->Ext.CountAct != 0))
		{
			TimerPtr->Ext.CountAct--;
			DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "--- dcp_TimerTabExec() TimerPtr=%X TimerId=%u CountAct=%u", 
					TimerPtr, TimerPtr->Id, TimerPtr->Ext.CountAct);
			if (TimerPtr->Ext.CountAct == 0)
			{
				/* Timeout
				*/
				if (TimerPtr->Ext.Cyclic)
				{
					/* Reload cyclic timer
					*/
					TimerPtr->Ext.CountAct = TimerPtr->Ext.CountStart;
					DCP_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "--- dcp_TimerTabExec() TimerId=%u reloaded CountAct=%u", 
							TimerPtr->Id, TimerPtr->Ext.CountAct);
				}
				/* Call internal timeout function
				*/
				dcp_timeout_int(TimerPtr);
			}
		}
		TimerPtr = NextPtr;
	}
	DCP_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "  < dcp_TimerTabExec() TimerNo=%u", 
			TimerNo);

	LSA_UNUSED_ARG(TimerNo);
}

/*****************************************************************************/
/* Function: dcp_timeout_int()                                               */
/* Handle elapsed DCP timer                                                  */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_timeout_int(
DCP_TIMER_PTR_TYPE 			TimerPtr)
{
	DCP_CHA_PTR 			ChPtr;
	DCP_LOWER_RQB_PTR_TYPE	RbPu;
	LSA_UINT16				RetVal;
	LSA_UINT8				Uid = DCP_NOSERVICE;
	LSA_UINT32				TraceIdx = 0;
	DCP_MAC_ADR				NilAddr = {0,0,0,0,0,0};

	DCP_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_timeout_int() TimerPtr=%X TimerId=%u", 
					TimerPtr, TimerPtr->Id);

	ChPtr = dcp_ChGet(TimerPtr->Handle);

	if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
	{
		TraceIdx = ChPtr->TraceIdx;
		DCP_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_timeout_int() ChPtr=%X CallId=%u", 
						ChPtr, TimerPtr->CallId);

		switch (TimerPtr->CallId)
		{

		/* Server Timeouts:
		*/
		case DCP_TIMER_LOCK:
			
			/* client hold timer elapsed:
			 * Finish Multirequest Lock by setting SAM to NIL
			*/
			DCP_FUNCTION_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- dcp_timeout_int() ChPtr=%X ClientHoldTime end for ...%02X-%02X-%02X", 
						ChPtr, ChPtr->ClientHoldAddr[3], ChPtr->ClientHoldAddr[4], ChPtr->ClientHoldAddr[5]);
			DCP_DS_SET_ADDR(ChPtr->ClientHoldAddr, NilAddr, DCP_MAC_ADR_SIZE);
			break;

		case DCP_TIMER_IDENTIFY:
			break;

		/* Client Request Timeouts:
		*/
		case DCP_TIMER_IDENTREQ:
			Uid = DCP_IDENTIFY;
			break;

		case DCP_TIMER_SETREQ:
			Uid = DCP_SET;
			break;

		case DCP_TIMER_GETREQ:
			Uid = DCP_GET;
			break;

		case DCP_TIMER_HELLOREQ:
			Uid = DCP_HELLO;
			break;

		default:
			/* Invalid CallId !?!
			*/
			DCP_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_timeout_int() ChPtr=%X invalid CallId=%u", 
							ChPtr, TimerPtr->CallId);
			dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_TIMER, TimerPtr, TimerPtr->CallId);
		}

		if (TimerPtr->Running)
		{
			if (!TimerPtr->Ext.Cyclic)
			{
				/* Remove one shot timer from timer queue
				*/
				dcp_TimerTabRemove(TimerPtr);

				/* OneShot timer is not longer active: put back timer block into channel data
				*/
				TimerPtr->Running = LSA_FALSE;
				if (TimerPtr->Single)
				{
					ChPtr->TimerPtr = TimerPtr;
				}
				else
				{
					dcp_TimerEnque(&ChPtr->TimerQueue, TimerPtr);
				}
			}
			if (Uid != DCP_NOSERVICE)
			{
				/* Client-Timeout
				*/
				RbPu = TimerPtr->RbPu;
				TimerPtr->RbPu.VoidPtr = LSA_NULL;
				dcp_ClientTimeout(ChPtr, RbPu, TimerPtr->RbPtr, TimerPtr, Uid);
			}
			else if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr->RbPu.VoidPtr, LSA_NULL))
			{
				/* Server-Timeout:
					* Send away Send-RQB if ready
					* AP01349915: check for channel open state
				*/
				RbPu = TimerPtr->RbPu;
				TimerPtr->RbPu.VoidPtr = LSA_NULL;
				if (ChPtr->State == DCP_CHA_STATE_OPEN)
				{
					dcp_DataSend(ChPtr, RbPu, TimerPtr->SendLen);
				}
				else
				{
					DCP_ERROR_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_timeout_int() ChPtr=%X TimerPtr=%X Id=%d: channel state=0x%x", 
									ChPtr, TimerPtr, TimerPtr->Id, ChPtr->State);
					dcp_FreeLowerRb(ChPtr, RbPu);
				}
			}
		}
		else
		{
			/* Timer not started or stopped !?!
				* 280404lrg002: may be on ENTER() delay
				* 120907lrg001: may be after dcp_TimerStop if DCP_DO_TIMEOUT_EXTERN is defined
			*/
			dcp_TimerTabStop(ChPtr, TimerPtr, &RetVal);
			DCP_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_timeout_int() ChPtr=%X TimerPtr=%X: Ignoring timeout TimerId=%d", 
							ChPtr, TimerPtr, TimerPtr->Id);
		}
	}
	else
	{
		/* 220404lrg002: may be after close_channel at cyclic timer
		 * dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CH_PTR, UserId.void_ptr, TimerId);
		*/
		DCP_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE, "*** dcp_timeout_int() TimerPtr=%X: Timeout without channel TimerId=%d", 
						TimerPtr, TimerPtr->Id);
	}

	DCP_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_timeout_int() ChPtr=%X TimerPtr=%X TimerId=%d", 
						ChPtr, TimerPtr, TimerPtr->Id);
}

/*****************************************************************************/
/* External LSA access function: dcp_timeout()                               */
/* LSA timer elapsed                                                         */
/*****************************************************************************/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_timeout(
LSA_UINT16			TimerNo,
LSA_USER_ID_TYPE	UserId)
{
	DCP_TIMER_TAB		*TabPtr = (DCP_TIMER_TAB*)(UserId.void_ptr);

#ifndef DCP_DO_TIMEOUT_EXTERN
	/* 1027120: Use ENTER/EXIT only if no context change will be done by DCP_DO_TIMER_REQUEST()
	*/
	DCP_ENTER();
#endif
	DCP_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "--> dcp_timeout() TimerNo=%u UserId=0x%08x", 
					TimerNo, UserId.uvar32);

	if (LSA_HOST_PTR_ARE_EQUAL(TabPtr, &dcp_data.Timer100ms)
	||  LSA_HOST_PTR_ARE_EQUAL(TabPtr, &dcp_data.Timer10ms))
	{

#ifndef DCP_DO_TIMEOUT_EXTERN
		/* 1089622: call dcp_TimerTabExec() only if no context change will be done by DCP_DO_TIMER_REQUEST()
		*/
		dcp_TimerTabExec(TimerNo, TabPtr);
#else
		dcp_TimerSendRQB(TimerNo, UserId);
#endif
	}
	else
	{
		DCP_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_timeout() TimerNo=%u invalid TimerTabPtr=%X in UserId", 
				TimerNo, TabPtr);
	}
	DCP_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "<-- dcp_timeout() TimerNo=%u UserId=0x%08x", 
					TimerNo, UserId.uvar32);
#ifndef DCP_DO_TIMEOUT_EXTERN
	DCP_EXIT();
#endif
}


#ifdef DCP_DO_TIMEOUT_EXTERN
/*****************************************************************************/
/* Function: dcp_TimerSendRQB()                                              */
/* Send Timer RQB for context change                                         */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerSendRQB(
LSA_UINT16										TimerNo,
LSA_USER_ID_TYPE								UserId)
{
	DCP_TIMER_TAB		*TabPtr = (DCP_TIMER_TAB*)(UserId.void_ptr);
    DCP_UPPER_RQB_PTR_TYPE RbTimerPtr = LSA_NULL; 

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "  > dcp_TimerSendRQB() TimerNo=%u UserId=0x%08x Queue.Count=%u", 
			TimerNo, UserId.uvar32, TabPtr->Queue.Count);

	/* Look for unused timer RQB ptr
	*/
	/* 1089622: Check RbuInUse for 0 = RQB not in use.
	*/
	if ((0 == TabPtr->RbuInUse) && !LSA_HOST_PTR_ARE_EQUAL(TabPtr->RbuPtr, LSA_NULL))
	{
		/* Get timer RQB ptr
		*/
		TabPtr->RbuInUse = 1;
		RbTimerPtr = TabPtr->RbuPtr;
		RbTimerPtr->args.Timer.UserID = UserId; 
		RbTimerPtr->args.Timer.TimerID = TimerNo; 
		DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "--- dcp_TimerSendRQB() TimerNo=%u UserId=0x%08x send pRQB=%X", 
						TimerNo, UserId.uvar32, RbTimerPtr);
		/* Send local timer RQB to DCP 
		*/
		DCP_DO_TIMER_REQUEST(RbTimerPtr);
	}
	else
	{
		/* AP01257857: LTRC_TRACE_LEVEL_ERROR -> LSA_TRACE_LEVEL_ERROR
		*/
		DCP_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerSendRQB() No timer RQB: dropping TimerNo=%u UserId=%08x", 
						TimerNo, UserId.uvar32);
	}
	DCP_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "  < dcp_TimerSendRQB() TimerNo=%u", 
			TimerNo);
}

/*****************************************************************************/
/* Internal access function: dcp_TimeoutRequest()                            */
/* LSA timer elapsed                                                         */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimeoutRequest(
DCP_UPPER_RQB_PTR_TYPE  RbuPtr)
{
	DCP_TIMER_TAB	*TabPtr = (DCP_TIMER_TAB*)(RbuPtr->args.Timer.UserID.void_ptr);
	LSA_UINT16		TimerNo = RbuPtr->args.Timer.TimerID;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "  > dcp_TimeoutRequest() RbuPtr=%X TimerNo=%u UserId=0x%08x", 
						RbuPtr, TimerNo, RbuPtr->args.Timer.UserID.uvar32);

	if (LSA_HOST_PTR_ARE_EQUAL(TabPtr, &dcp_data.Timer100ms)
	||  LSA_HOST_PTR_ARE_EQUAL(TabPtr, &dcp_data.Timer10ms))
	{
		/* 1089622: Set RbuInUse to 0 = RQB not in use.
		 * 1305962/1308196: Remove setting UserId to zero
		*/
		TabPtr->RbuInUse = 0;

		/* 1089622: call dcp_TimerTabExec() after context change
		*/
		dcp_TimerTabExec(TimerNo, TabPtr);
	}
	else
	{
		DCP_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimeoutRequest() TimerNo=%u invalid TimerTabPtr=%X in UserId", 
				TimerNo, TabPtr);
	}
	DCP_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "  < dcp_TimeoutRequest() RbuPtr=%X", 
						RbuPtr);
}
#endif

/*****************************************************************************/
/* Internal access function: dcp_TimerAlloc()                                */
/* Allocate DCP internal timer block                                         */
/*****************************************************************************/
DCP_TIMER_PTR_TYPE  DCP_LOCAL_FCT_ATTR  dcp_TimerAlloc(
DCP_CHA_PTR			ChPtr,
DCP_UINT16			Type,
DCP_UINT16			Base)
{
	DCP_TIMER_PTR_TYPE	TimerPtr = LSA_NULL;
	LSA_UINT16 			RetVal = LSA_RET_OK;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerAlloc() ChPtr=%X Type=%d Base=%d", 
						ChPtr, Type, Base);

	DCP_ALLOC_LOCAL_MEM((DCP_LOCAL_MEM_PTR_TYPE *)&TimerPtr, sizeof(DCP_TIMER));
	if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		RetVal = LSA_RET_ERR_RESOURCE;
		DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** dcp_TimerAlloc() ChPtr=%X DCP_ALLOC_LOCAL_MEM length=%u failed",
				ChPtr, sizeof(DCP_TIMER));
	}
	else
	{

		/* Check timer base
		*/
		switch (Base)
		{
		case LSA_TIME_BASE_10MS:
		case LSA_TIME_BASE_100MS:
			break;

		default:
			/* Invalid timer base
			*/
			RetVal = LSA_RET_ERR_PARAM;
			DCP_ERROR_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerAlloc() ChPtr=%X: Invalid timer Base=%u)", 
						ChPtr, Base);
			dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_TIMER, LSA_NULL, Base);
		}

		if (LSA_RET_OK == RetVal)
		{
			TimerPtr->Id = ++ChPtr->TimerCount;
			TimerPtr->Handle = ChPtr->Handle;
			TimerPtr->Base = Base;
			TimerPtr->NextPtr = LSA_NULL;
			TimerPtr->PrevPtr = LSA_NULL;
			TimerPtr->Running = LSA_FALSE;
			TimerPtr->Single = LSA_FALSE;
			TimerPtr->CallId = 0;
			TimerPtr->Time = 0;
			TimerPtr->SendLen = 0;
			TimerPtr->RbPu.VoidPtr = LSA_NULL;
			TimerPtr->RbPtr = LSA_NULL;
			
			if (Type ==  LSA_TIMER_TYPE_CYCLIC)
			{
				TimerPtr->Ext.Cyclic = LSA_TRUE;
			}
			else
			{
				TimerPtr->Ext.Cyclic = LSA_FALSE;
			}
			TimerPtr->Ext.Factor = 1;
			TimerPtr->Ext.InUse = LSA_FALSE;
			TimerPtr->Ext.CountAct = 0;
			TimerPtr->Ext.CountStart = 0;
			TimerPtr->Tract.Txs.TxPu.TxEthPtr = LSA_NULL;
			TimerPtr->Tract.Txs.RbuPtr = LSA_NULL;
		}
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerAlloc() TimerPtr=%X TimerId=%u RetVal=0x%x", 
						TimerPtr, TimerPtr->Id, RetVal);
	return(TimerPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerGet()                                  */
/* Preserve DCP internal timer                                               */
/*****************************************************************************/
DCP_TIMER_PTR_TYPE  DCP_LOCAL_FCT_ATTR  dcp_TimerGet(
DCP_CHA_PTR				ChPtr,
LSA_UINT8				CallId,
LSA_UINT16				Time,
DCP_TIMER_QUEUE			*QueuePtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu,
DCP_UPPER_RQB_PTR_TYPE	RbPtr)
{
	DCP_TIMER_PTR_TYPE 	TimerPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_05(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerGet() ChPtr=%X RblPtr=%X Time=%u QueuePtr=%X CallId=%u", 
						ChPtr, RblPu.VoidPtr, Time, QueuePtr, CallId);

	if ((CallId == DCP_TIMER_LOCK) || (CallId == DCP_TIMER_HELLOREQ))
	{
		/* Get free single timer structure
		*/
		TimerPtr = ChPtr->TimerPtr;
		if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL)
		||  (TimerPtr->Running))
		{
			TimerPtr = LSA_NULL;
		}
		else
		{
			ChPtr->TimerPtr = LSA_NULL;
		}
	}
	else
	{
		/* Find free timer structure in channel queue
		*/
		TimerPtr = ChPtr->TimerQueue.FirstPtr;
		while (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
		{
			if ((!TimerPtr->Running)
			&&  (!ChPtr->DetailPtr->ClientCh
			  || LSA_HOST_PTR_ARE_EQUAL(TimerPtr->Tract.Txs.RbuPtr, LSA_NULL)))
			{
				dcp_TimerDeque(&ChPtr->TimerQueue , TimerPtr);
				break;
			}
			TimerPtr = TimerPtr->NextPtr;
        }
	}

	if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		TimerPtr->CallId = CallId;
		if (Time != 0)
			TimerPtr->Time = Time;
		TimerPtr->RbPu = RblPu;
		TimerPtr->RbPtr = RbPtr;

		if (!LSA_HOST_PTR_ARE_EQUAL(QueuePtr, LSA_NULL))
		{
			/* Store timer in service queue for later start
			*/
			dcp_TimerEnque(QueuePtr, TimerPtr);
		}
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerGet() ChPtr=%X RbuPtr=%X TimerPtr=%X", 
						ChPtr, RbPtr, TimerPtr);
	return (TimerPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerPut()                                  */
/* Free preserved DCP internal timer                                         */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerPut(
DCP_CHA_PTR				ChPtr,
DCP_TIMER_PTR_TYPE		TimerPtr,
DCP_TIMER_QUEUE			*QueuePtr)
{
	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerPut() ChPtr=%X TimerPtr=%X QueuePtr=%X", 
						ChPtr, TimerPtr, QueuePtr);

	if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		if (!LSA_HOST_PTR_ARE_EQUAL(QueuePtr, LSA_NULL))
		{
			/* Remove timer from queue
			*/
			dcp_TimerDeque(QueuePtr, TimerPtr);
		}
		if (ChPtr->DetailPtr->ClientCh)
		{
			TimerPtr->Tract.Txs.RbuPtr = LSA_NULL;
		}
		if (TimerPtr->Single)
		{
			/* Put to single timer
			*/
			ChPtr->TimerPtr = TimerPtr;
		}
		else
		{
			/* Queue to channel queue
			*/
			dcp_TimerEnque(&ChPtr->TimerQueue , TimerPtr);
		}
	}
	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerPut() ChPtr=%X ChTimerPtr=%X Queue.count=%u", 
						ChPtr, ChPtr->TimerPtr, ChPtr->TimerQueue.Count);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerFind()                                 */
/* Find DCP internal timer in queues                                         */
/*****************************************************************************/
DCP_TIMER_PTR_TYPE  DCP_LOCAL_FCT_ATTR  dcp_TimerFind(
DCP_CHA_PTR				ChPtr,
LSA_UINT8				TimerId,
DCP_TIMER_QUEUE			*QueuePtr)
{
	DCP_TIMER_PTR_TYPE 	TimerPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerFind() ChPtr=%X TimerId=%u QueuePtr=%X", 
						ChPtr, TimerId, QueuePtr);

	if (!LSA_HOST_PTR_ARE_EQUAL(QueuePtr, LSA_NULL))
	{
		/* Find timer id in queue
		*/
		DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  - dcp_TimerFind() First=%X Last=%X Count=%u", 
							QueuePtr->FirstPtr, QueuePtr->LastPtr, QueuePtr->Count);
		TimerPtr = QueuePtr->FirstPtr;
		while (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
		{
			if ((TimerPtr->Id == TimerId) && (TimerPtr->Handle == ChPtr->Handle))
			{
				/* Found
				*/
				break;
			}
			TimerPtr = TimerPtr->NextPtr;
		}
	}
	if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		/* Find timer id in active queues
		*/
		TimerPtr = dcp_TimerTabFindId(ChPtr, TimerId);
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerFind() ChPtr=%X TimerPtr=%X", 
						ChPtr, TimerPtr);
	return(TimerPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerStart()                                */
/* Start DCP internal timer                                                  */
/*****************************************************************************/
LSA_UINT16  DCP_LOCAL_FCT_ATTR  dcp_TimerStart(
DCP_CHA_PTR				ChPtr,
DCP_TIMER_PTR_TYPE 		TimerPtr,
LSA_UINT8				TimerId,
DCP_TIMER_QUEUE			*QueuePtr,
LSA_UINT16				Time,
DCP_LOWER_RQB_PTR_TYPE	RblPu)
{
	LSA_UINT16			RetVal = LSA_RET_ERR_RESOURCE;

	DCP_FUNCTION_TRACE_05(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerStart() ChPtr=%X RblPtr=%X Time=%u QueuePtr=%X TimerId=%u", 
						ChPtr, RblPu.VoidPtr, Time, QueuePtr, TimerId);

	if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		if (LSA_HOST_PTR_ARE_EQUAL(QueuePtr, LSA_NULL))
		{
			/* Find timer id in active queues
			*/
			TimerPtr = dcp_TimerTabFindId(ChPtr, TimerId);
		}
		else
		{
			/* Find timer id in given queue
			*/
			TimerPtr = dcp_TimerFind(ChPtr, TimerId, QueuePtr);
		}
	}

	if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		if (!LSA_HOST_PTR_ARE_EQUAL(QueuePtr, LSA_NULL))
		{
			/* remove timer block from queue
			*/
			dcp_TimerDeque(QueuePtr , TimerPtr);
		}
		if (Time != 0)
		{
			/* set timer count
			*/
			TimerPtr->Time = Time;
		}
		TimerPtr->RbPu = RblPu;

		/* Start timer now
		*/
		RetVal = dcp_TimerTabStart(TimerPtr); 
	}

	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerStart() ChPtr=%X TimerPtr=%X RetVal=0x%x", 
						ChPtr, TimerPtr, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerStop()                                 */
/* Stop DCP internal timer                                                   */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerStop(
DCP_CHA_PTR			ChPtr,
DCP_TIMER_PTR_TYPE	TimerPtr,
LSA_UINT8			TimerId,
DCP_TIMER_QUEUE		*QueuePtr)
{
	LSA_UINT16		RetVal = LSA_RET_OK;

	DCP_FUNCTION_TRACE_05(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerStop() ChPtr=%X Handle=%u TimerPtr=%X TimerId=%u QueuePtr=%X", 
						ChPtr, ChPtr->Handle, TimerPtr, TimerId, QueuePtr);

	if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		TimerPtr = dcp_TimerTabFindId(ChPtr, TimerId);
	}
	if (LSA_HOST_PTR_ARE_EQUAL(TimerPtr, LSA_NULL))
	{
		RetVal = LSA_RET_OK_TIMER_NOT_RUNNING;
	}
	else
	{
		if (TimerPtr->Running)
		{
			/* dcp_timeout_int() checks for "Running"
			*/ 
			TimerPtr->Running = LSA_FALSE;
			dcp_TimerTabStop(ChPtr, TimerPtr, &RetVal);
	
			/* 120907lrg001: TIMER_NOT_RUNNING may appear if DCP_DO_TIMEOUT_EXTERN is defined
			 * Timer RQB is on the run. Timeout will be dropped in dcp_timeout_int()
			*/ 
			if ((RetVal != LSA_RET_OK)
			&&  (RetVal != LSA_RET_OK_TIMER_NOT_RUNNING))
			{
				DCP_ERROR_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** dcp_TimerStop() ChPtr=%X, ChPtr->Handle=%u TimerId=%u: RetVal=0x%x from dcp_TimerTabStop()", 
						ChPtr, ChPtr->Handle, TimerPtr->Id, RetVal);
				dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_TIMER_STOP, ChPtr, TimerPtr->Id);
			}
			else
			{
				if (LSA_HOST_PTR_ARE_EQUAL(QueuePtr, LSA_NULL))
				{
					/* Put timer block to channel data
					*/
					if (TimerPtr->Single)
					{
						ChPtr->TimerPtr = TimerPtr;
					}
					else
					{
						dcp_TimerEnque(&ChPtr->TimerQueue, TimerPtr);
					}
				}
				else
				{
					/* Put timer block to user queue
					*/
					dcp_TimerEnque(QueuePtr, TimerPtr);
				}
			}
		}
	}

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerStop() TimerPtr=%X RetVal=0x%x", 
						TimerPtr, RetVal);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerFree()                                 */
/* Free one allocated DCP Timer and stored lower RB                          */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerFree(
DCP_CHA_PTR			ChPtr,
DCP_TIMER_PTR_TYPE	TimerPtr)
{
	LSA_UINT16 RetVal = LSA_RET_OK;

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerFree() ChPtr=%X TimerPtr=%X Id=%u TimerPtr->RbPu.VoidPtr=%X", 
						ChPtr, TimerPtr, TimerPtr->Id, TimerPtr->RbPu.VoidPtr);

	if (!LSA_HOST_PTR_ARE_EQUAL(TimerPtr->RbPu.VoidPtr, LSA_NULL))
	{
		/* Free lower RB - if present
		*/ 
		 dcp_FreeLowerRb(ChPtr, TimerPtr->RbPu);
		 TimerPtr->RbPu.VoidPtr = LSA_NULL;
	}
	/* Free Timer memory block
	*/
	DCP_FREE_LOCAL_MEM(&RetVal, (DCP_LOCAL_MEM_PTR_TYPE)TimerPtr);
	if (RetVal != LSA_RET_OK)
	{
		DCP_ERROR_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** dcp_TimerFree() TimePtr=%X TimerId=%u RetVal=0x%x", 
							TimerPtr, TimerPtr->Id, RetVal);
		dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_TIMER_FREE, 
						ChPtr, TimerPtr->Id);
	}
	ChPtr->TimerCount--;

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerFree() ChPtr=%X imerCount=%u", 
						ChPtr, ChPtr->TimerCount);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerCleanAll()                             */
/* Free all allocated DCP Timer and stored lower RB on a channel             */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerCleanAll(
DCP_CHA_PTR	ChPtr)
{
	DCP_TIMER_PTR_TYPE	TimerPtr;
	LSA_UINT8			TimerId;

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerCleanAll() ChPtr=%X TimerCount=%u Queue.Count=%u TimerPtr=%X", 
						ChPtr, ChPtr->TimerCount, ChPtr->TimerQueue.Count, ChPtr->TimerPtr);

	/* Stop all Timers of this channel
	*/ 
	for (TimerId = ChPtr->TimerCount; TimerId > 0; TimerId--)
	{
		dcp_TimerStop(ChPtr, LSA_NULL, TimerId, LSA_NULL);
	}
	
	/* Free all Timers from queue
	*/ 
	while (ChPtr->TimerQueue.Count != 0)
	{
		TimerPtr = ChPtr->TimerQueue.FirstPtr;
		dcp_TimerDeque(&ChPtr->TimerQueue, TimerPtr);
		dcp_TimerFree(ChPtr, TimerPtr);
	}

	/* Free single timer
	*/ 
	if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->TimerPtr, LSA_NULL))
	{
		dcp_TimerFree(ChPtr, ChPtr->TimerPtr);
		ChPtr->TimerPtr = LSA_NULL;
	}

	DCP_FUNCTION_TRACE_04(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerCleanAll() ChPtr=%X TimerCount=%u Queue.Count=%u TimerPtr=%X", 
						ChPtr, ChPtr->TimerCount, ChPtr->TimerQueue.Count, ChPtr->TimerPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerCalc()                                 */
/* Calculate address dependend time constant for Server Broadcast Response   */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_TimerCalc(
DCP_CHA_PTR ChPtr)
{
	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerCalc() ChPtr=%X", 
						ChPtr);

	/* Calculate delay constant K_addr for Multicast Responses
	 * T_delay = t_basis * ( K_addr % Faltung) 	[sec]
	*/
	ChPtr->ChLowPtr->K_addr = DCP_WORD(ChPtr->ChLowPtr->LocalMacAdr[DCP_MAC_RAND_IDX_LOW],
									   ChPtr->ChLowPtr->LocalMacAdr[DCP_MAC_RAND_IDX_HIGH]);

	DCP_FUNCTION_TRACE_02(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerCalc() ChPtr=%X K_Addr=0x%x", 
						ChPtr, ChPtr->ChLowPtr->K_addr);
}

/*****************************************************************************/
/* Internal access function: dcp_ChJoin()                                    */
/* Find Channel structure with same LowerID as stored in NewChPtr->DetailPtr */
/*****************************************************************************/
LSA_UINT16  DCP_LOCAL_FCT_ATTR  dcp_ChJoin(
DCP_CHA_PTR 	NewChPtr)
{
	LSA_HANDLE_TYPE	ChId;
	DCP_CHA_PTR 	ChPtr = LSA_NULL;
	DCP_CHL_PTR 	ChlPtr = LSA_NULL;
	LSA_UINT16		RespVal = DCP_RSP_ERR_RESOURCE;
	LSA_BOOL		ChFound = LSA_TRUE;

	DCP_FUNCTION_TRACE_03(NewChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ChJoin() NewChPtr=%X ClientCh=%u LowerId=%u", 
						NewChPtr, NewChPtr->DetailPtr->ClientCh, NewChPtr->DetailPtr->LowerId);

	for (ChId = 0; ChId < DCP_MAX_CHANNELS; ChId++)
	{
		ChPtr = (DCP_CHA_PTR)(dcp_data.ChListPtr + ChId);
		if ((ChPtr->State != DCP_CHA_STATE_FREE)
		&& !LSA_HOST_PTR_ARE_EQUAL(ChPtr, NewChPtr)
		&& !LSA_HOST_PTR_ARE_EQUAL(ChPtr->DetailPtr, LSA_NULL)
		&& (ChPtr->DetailPtr->LowerId == NewChPtr->DetailPtr->LowerId))
		{
			/* Found channel with the same LowerID
			*/
			ChFound = LSA_TRUE;	//030309lrg001

			if ((DCP_DEV_FSU == NewChPtr->DetailPtr->DevId)
			||  (DCP_DEV_FSU == ChPtr->DetailPtr->DevId))
			{
				/* New or existing FSU channel
				*/
				if ((DCP_DEV_FSU == NewChPtr->DetailPtr->DevId)
				&&  (DCP_DEV_FSU == ChPtr->DetailPtr->DevId))
				{
					/* 2 FSU channels with the same LowerID
					*/
					RespVal = DCP_RSP_ERR_REF;
				}
				else if (DCP_DEV_FSU == NewChPtr->DetailPtr->DevId)
				{
					/* RespVal remains DCP_RSP_ERR_RESOURCE for 
					 * allocation of a lower channel block below
					*/
					if (!ChPtr->DetailPtr->ClientCh)
					{
						/* Connect existing server channel to new FSU channel
						 * using the (at server channel unused) ChNextPtr.
					 	*/
						ChPtr->ChNextPtr = NewChPtr;
						NewChPtr->ChNextPtr = ChPtr;
						NewChPtr->TraceIdx = ChPtr->TraceIdx;
					}
					else if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChLowPtr, LSA_NULL)
						  &&  !LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChLowPtr->ChServerPtr, LSA_NULL))
					{
						/* Connect existing server channel by the lower channel 
						 * of the found client channel with the new FSU channel
						*/
						ChPtr->ChLowPtr->ChServerPtr->ChNextPtr = NewChPtr;
						NewChPtr->ChNextPtr = ChPtr->ChLowPtr->ChServerPtr;
						NewChPtr->TraceIdx = ChPtr->ChLowPtr->TraceIdx;
					}
				}
				else
				{
					/* New server or Client channel finds existing FSU channel:
					 * continue searching for an other user channel
					*/
					ChFound = LSA_FALSE;
					if (!NewChPtr->DetailPtr->ClientCh)
					{
						/* Connect new Server channel to existing FSU channel
						*/
						ChPtr->ChNextPtr = NewChPtr;
						NewChPtr->ChNextPtr = ChPtr;
						NewChPtr->TraceIdx = ChPtr->TraceIdx;
					}
				}
			}
			else
			{
				/* Not a new or existing FSU channel
			 	*/
				if ((ChPtr->DetailPtr->AllocOnRequest) != (NewChPtr->DetailPtr->AllocOnRequest))
				{
					/* The allocation strategie for lower RecvRQBs has to be the same
					 * at all upper Channels on the same lower Channel
					*/
					RespVal = DCP_RSP_ERR_CONFIG;
				}
				else if (!NewChPtr->DetailPtr->ClientCh)
				{
					/* New Server channel
					*/
					if (LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChLowPtr->ChServerPtr, LSA_NULL))
					{
						/* Existing Client channel: add Server channel
						*/
						ChPtr->ChLowPtr->ChServerPtr = NewChPtr;
						RespVal = DCP_RSP_OK;
						if (LSA_HOST_PTR_ARE_EQUAL(NewChPtr->ChNextPtr, LSA_NULL))
						{
							/* Continue searching for FSU channel
							*/
							ChFound = LSA_FALSE;
						}
					}
					else
					{
						/* Already an existing Server channel on lower channel
						*/
						RespVal = DCP_RSP_ERR_REF;
					}
				}
				else
				{
					/* New Client channel
					*/
					DCP_CHA_PTR PrevClientPtr = ChPtr->ChLowPtr->ChClientPtr;

					if (LSA_HOST_PTR_ARE_EQUAL(PrevClientPtr, LSA_NULL))
					{
						/* First Client channel
						*/
						ChPtr->ChLowPtr->ChClientPtr = NewChPtr;
					}
					else
					{
						/* Append Client channel to the End of the chain
						*/
						while (!LSA_HOST_PTR_ARE_EQUAL(PrevClientPtr->ChNextPtr, LSA_NULL))
							PrevClientPtr = PrevClientPtr->ChNextPtr;
						PrevClientPtr->ChNextPtr = NewChPtr;
					}
					RespVal = DCP_RSP_OK;
				}

				if (DCP_RSP_OK == RespVal)
				{
					/* Connect new upper channel to the lower channel
					 * of the existing upper channel
					*/
					ChPtr->ChLowPtr->ChCount++;
					NewChPtr->ChLowPtr = ChPtr->ChLowPtr;
					NewChPtr->TraceIdx = ChPtr->ChLowPtr->TraceIdx;
				}
			}
			if (ChFound)
				break;
		}
	}

	if (DCP_RSP_ERR_RESOURCE == RespVal)
	{
		/* No matching lower Channel found:
		 * allocate a new one
		*/ 
		for (ChId = 0; ChId < DCP_MAX_LOW_CHANNELS*2; ChId++)	//240407lrg001: *2 because of FSU Channel
		{
			ChlPtr = (DCP_CHL_PTR)(dcp_data.ChLowListPtr + ChId);
			if (ChlPtr->State == DCP_CHA_STATE_FREE)
			{
				/* Connect new upper channel to the 
				 * new lower channel
				 * 130105lrg001: initialise Rbl counter
				 * 120307lrg001: set ChCount to 1 instead of ++
				*/
				ChlPtr->ChCount = 1;
				ChlPtr->RecvRblAct = 0;
				ChlPtr->RecvRblClient = 0;
				ChlPtr->TraceIdx = 0;
				NewChPtr->TraceIdx = 0;
				NewChPtr->ChLowPtr = ChlPtr;
				if (!NewChPtr->DetailPtr->ClientCh
				||  (DCP_DEV_FSU == NewChPtr->DetailPtr->DevId))
					ChlPtr->ChServerPtr = NewChPtr;
				else
					ChlPtr->ChClientPtr = NewChPtr;
				ChlPtr->State = DCP_CHA_STATE_CLOSED;
				ChlPtr->OpenPu.VoidPtr = LSA_NULL;
				RespVal = DCP_RSP_OK_ACTIVE;
				break;
			}
		}
	}

	DCP_FUNCTION_TRACE_03(NewChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ChJoin() ChLowPtr=%X ChPtr=%X, RespVal=0x%x", 
						NewChPtr->ChLowPtr, ChPtr, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: dcp_ChAlloc()                                   */
/* Allocate channel strukture                                                */
/*****************************************************************************/
DCP_CHA_PTR  DCP_LOCAL_FCT_ATTR  dcp_ChAlloc(
DCP_UPPER_RQB_PTR_TYPE  RbuPtr,
LSA_SYS_PTR_TYPE		SysPtr,
DCP_DETAIL_PTR_TYPE		DetailPtr)
{
	LSA_HANDLE_TYPE	ChId;
	DCP_CHA_PTR 	ChPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_ChAlloc() RbuPtr=%X", 
						RbuPtr);


	for (ChId = 0; ChId < DCP_MAX_CHANNELS; ChId++)
	{
		ChPtr = (DCP_CHA_PTR)(dcp_data.ChListPtr + ChId);
		if (ChPtr->State == DCP_CHA_STATE_FREE)
		{
			/* Found free channel: initialise the structure
			 * 240810lrg001: SysPtr, DetailPtr
			*/
			ChPtr->State = DCP_CHA_STATE_OPENING;
			ChPtr->SysPtr = SysPtr;
			ChPtr->DetailPtr = DetailPtr;
			ChPtr->SysPath = DCP_UPPER_RQB_SYSPATH_GET(RbuPtr);
			ChPtr->HandleUpper = DCP_UPPER_RQB_HANDLE_UPPER_GET(RbuPtr);
			ChPtr->CbfPtr = DCP_UPPER_RQB_CBF_GET(RbuPtr);
			ChPtr->ChLowPtr = LSA_NULL;
			ChPtr->ChNextPtr = LSA_NULL;
			ChPtr->QueueRecvCnt = 0;
			ChPtr->ServerIndReady = LSA_FALSE;
			ChPtr->SendReqCount = 0;			//051207lrg001
			ChPtr->WaitSendCnf = LSA_FALSE;		//051207lrg001
			ChPtr->TraceIdx = 0;

			/* Set upper handle in request block
			 */
			DCP_UPPER_RQB_HANDLE_SET(RbuPtr, ChPtr->Handle);
			break;
		}
		else
			ChPtr = LSA_NULL;
	}

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_ChAlloc() RbuPtr=%X ChPtr=%X ChId=%d", 
						RbuPtr, ChPtr, ChId);
	return(ChPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_ChClean()                                   */
/* Free allocated channel ressources (timer and request blocks)              */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_ChClean(
DCP_CHA_PTR ChPtr)
{
	DCP_LOWER_RQB_PTR_TYPE 	RblPu;
	DCP_UPPER_RQB_PTR_TYPE 	RbuPtr;
	DCP_CHA_PTR				PrevClientPtr;
	
	DCP_FUNCTION_TRACE_03(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ChClean() ChPtr=%X QueueUpper.Count=%u QueueLower.Count=%u", 
						ChPtr, ChPtr->QueueUpper.Count, ChPtr->QueueLower.Count);
	
	/* Free allocated timer
	*/
	dcp_TimerCleanAll(ChPtr);

	/* Free lower RQBs
	*/ 
	while (ChPtr->QueueLower.Count != 0)
	{
		RblPu = ChPtr->QueueLower.FirstPu;
		dcp_DequeLower(&ChPtr->QueueLower, RblPu, ChPtr);
		dcp_FreeLowerRb(ChPtr, RblPu);
	}
	while (ChPtr->QueueCancel.Count != 0)
	{
		RblPu = ChPtr->QueueCancel.FirstPu;
		dcp_DequeLower(&ChPtr->QueueCancel, RblPu, ChPtr);
		dcp_FreeLowerRb(ChPtr, RblPu);
	}
	if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChLowPtr, LSA_NULL))
	{
		while ((ChPtr->ChLowPtr->QueueRecv.Count != 0)
		&&      ((ChPtr->ChLowPtr->ChCount == 1) || (ChPtr->QueueRecvCnt != 0)))
		{
			RblPu = ChPtr->ChLowPtr->QueueRecv.FirstPu;
			dcp_DequeLower(&ChPtr->ChLowPtr->QueueRecv, RblPu, ChPtr);
			dcp_FreeLowerRb(ChPtr, RblPu);
			if (ChPtr->QueueRecvCnt != 0)
			{
				ChPtr->QueueRecvCnt--;
			}
		}
	}
	
	if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->DetailPtr, LSA_NULL)
	&&  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
	{
		if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChNextPtr, LSA_NULL))
		{
			/* Closing FSU channel with connected server channel:
			 * Get Upper Hello-Indication RQBs from SERVER Queue and
			 * give it to the User with Responsecode DCP_RSP_OK_CANCEL
			*/
			while (ChPtr->ChNextPtr->QueueHello.Count != 0)
			{
				RbuPtr = ChPtr->ChNextPtr->QueueHello.FirstPtr;
				dcp_GetUpperQ(&ChPtr->ChNextPtr->QueueHello, RbuPtr);
				dcp_RequestDone(ChPtr->ChNextPtr, RbuPtr, DCP_RSP_OK_CANCEL);
			}
		}
	}
	else
	{
		/* Get RQBs from SERVER queues and give it to 
		 * the User with Responsecode DCP_RSP_OK_CANCEL
		*/
		while (ChPtr->QueueInd.Count != 0)
		{
			RbuPtr = ChPtr->QueueInd.FirstPtr;
			dcp_GetUpperQ(&ChPtr->QueueInd, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->QueueHello.Count != 0)
		{
			RbuPtr = ChPtr->QueueHello.FirstPtr;
			dcp_GetUpperQ(&ChPtr->QueueHello, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->QueueIdentify.Count != 0)
		{
			RbuPtr = ChPtr->QueueIdentify.FirstPtr;
			dcp_GetUpperQ(&ChPtr->QueueIdentify, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->QueueSet.Count != 0)
		{
			RbuPtr = ChPtr->QueueSet.FirstPtr;
			dcp_GetUpperQ(&ChPtr->QueueSet, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->QueueGet.Count != 0)
		{
			RbuPtr = ChPtr->QueueGet.FirstPtr;
			dcp_GetUpperQ(&ChPtr->QueueGet, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->QueueRsp.Count != 0)
		{
			RbuPtr = ChPtr->QueueRsp.FirstPtr;
			dcp_GetUpperQ(&ChPtr->QueueRsp, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->CancelRbu, LSA_NULL))
		{
			RbuPtr = ChPtr->CancelRbu;
			ChPtr->CancelRbu = LSA_NULL;
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		/* Free lower RQBs
		*/ 
		while (ChPtr->Identify.QueueSend.Count != 0)
		{
			RblPu = ChPtr->Identify.QueueSend.FirstPu;
			dcp_DequeLower(&ChPtr->Identify.QueueSend, RblPu, ChPtr);
			dcp_FreeLowerRb(ChPtr, RblPu);
		}
		while (ChPtr->Set.QueueSend.Count != 0)
		{
			RblPu = ChPtr->Set.QueueSend.FirstPu;
			dcp_DequeLower(&ChPtr->Set.QueueSend, RblPu, ChPtr);
			dcp_FreeLowerRb(ChPtr, RblPu);
		}
		while (ChPtr->Get.QueueSend.Count != 0)
		{
			RblPu = ChPtr->Get.QueueSend.FirstPu;
			dcp_DequeLower(&ChPtr->Get.QueueSend, RblPu, ChPtr);
			dcp_FreeLowerRb(ChPtr, RblPu);
		}
		while (ChPtr->Hello.QueueSend.Count != 0)
		{
			RblPu = ChPtr->Hello.QueueSend.FirstPu;
			dcp_DequeLower(&ChPtr->Hello.QueueSend, RblPu, ChPtr);
			dcp_FreeLowerRb(ChPtr, RblPu);
		}

		/* Get RQBs from upper client queues and give it to 
		 * the User with Responsecode DCP_RSP_OK_CANCEL
		*/
		while (ChPtr->QueueRetry.Count != 0)
		{
			RbuPtr = ChPtr->QueueRetry.FirstPtr;
			dcp_GetUpperQ(&ChPtr->QueueRetry, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->Identify.QueueUpper.Count != 0)
		{
			RbuPtr = ChPtr->Identify.QueueUpper.FirstPtr;
			dcp_GetUpperQ(&ChPtr->Identify.QueueUpper, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->Identify.QueueNotify.Count != 0)
		{
			RbuPtr = ChPtr->Identify.QueueNotify.FirstPtr;
			dcp_GetUpperQ(&ChPtr->Identify.QueueNotify, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->Set.QueueUpper.Count != 0)
		{
			RbuPtr = ChPtr->Set.QueueUpper.FirstPtr;
			dcp_GetUpperQ(&ChPtr->Set.QueueUpper, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->Get.QueueUpper.Count != 0)
		{
			RbuPtr = ChPtr->Get.QueueUpper.FirstPtr;
			dcp_GetUpperQ(&ChPtr->Get.QueueUpper, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
		while (ChPtr->Hello.QueueUpper.Count != 0)
		{
			RbuPtr = ChPtr->Hello.QueueUpper.FirstPtr;
			dcp_GetUpperQ(&ChPtr->Hello.QueueUpper, RbuPtr);
			dcp_RequestDone(ChPtr, RbuPtr, DCP_RSP_OK_CANCEL);
		}
	}

	if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChLowPtr, LSA_NULL))
	{
		/* Cut connection to Lower Channel
		*/
		if (!ChPtr->DetailPtr->ClientCh
		||  (ChPtr->DetailPtr->DevId == DCP_DEV_FSU))
		{
			if (LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChLowPtr->ChServerPtr, ChPtr))
			{
				ChPtr->ChLowPtr->ChServerPtr = LSA_NULL;
			}

			/* Disconnect server channel from FSU channel
			*/
			if (!LSA_HOST_PTR_ARE_EQUAL(ChPtr->ChNextPtr, LSA_NULL))
			{
				ChPtr->ChNextPtr->ChNextPtr = LSA_NULL;
				ChPtr->ChNextPtr = LSA_NULL;
			}
		}
		else
		{
			/* Take client channel out of chain
			*/
			PrevClientPtr = ChPtr->ChLowPtr->ChClientPtr;
			if (LSA_HOST_PTR_ARE_EQUAL(PrevClientPtr, ChPtr))
			{
				ChPtr->ChLowPtr->ChClientPtr = ChPtr->ChNextPtr;
			}
			else
			{
				while (!LSA_HOST_PTR_ARE_EQUAL(PrevClientPtr->ChNextPtr, ChPtr))
				{
					PrevClientPtr = PrevClientPtr->ChNextPtr;
				}
				PrevClientPtr->ChNextPtr = ChPtr->ChNextPtr;
			}
		}
		ChPtr->ChLowPtr->ChCount--;
		if (ChPtr->ChLowPtr->ChCount == 0)
		{
			ChPtr->ChLowPtr->State = DCP_CHA_STATE_FREE;
		}
		ChPtr->ChLowPtr = LSA_NULL;
	}

	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ChClean() ChPtr=%X", 
						ChPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_ChFree()                                    */
/* Free allocated channel structure                                          */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR  dcp_ChFree(
DCP_CHA_PTR  ChPtr)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ChFree() ChPtr=%X", 
						ChPtr);
	
	/* If the channel pointer is valid...
	*/
	if ((ChPtr >= (DCP_CHA_PTR)dcp_data.ChListPtr) 
	&&  (ChPtr <= (DCP_CHA_PTR)(dcp_data.ChListPtr + DCP_MAX_CHANNELS)))
	{
		/* ...and no more RQBs are in the queues... 
		*/
		if ((ChPtr->QueueUpper.Count != 0)
		||  (ChPtr->QueueLower.Count != 0))
		{
			dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CH_FREE, ChPtr, 0);
		}

		/* 240810lrg001: DCP_RELEASE_PATH_INFO after callback
		*/
		DCP_RELEASE_PATH_INFO(&RetVal, ChPtr->SysPtr, ChPtr->DetailPtr);
		if (LSA_RET_OK != RetVal)
		{
			dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_REL_PATH, ChPtr->DetailPtr, 0);
		}

		/* ...the channel structure is marked as FREE
		*/
		ChPtr->State = DCP_CHA_STATE_FREE;
		ChPtr->TraceIdx = 0;
	}
	else
	{
		dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, DCP_ERR_CH_PTR, ChPtr, 0);
	}

	DCP_FUNCTION_TRACE_01(ChPtr->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ChFree() ChPtr=%X", 
						ChPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_ChLowGet()                                  */
/* Get pointer to lower channel structure by handle                          */
/*****************************************************************************/
DCP_CHL_PTR DCP_LOCAL_FCT_ATTR  dcp_ChLowGet(
LSA_HANDLE_TYPE  Handle)
{
	LSA_HANDLE_TYPE	ChId = 0;
	DCP_CHL_PTR		ChPtr = LSA_NULL;
	LSA_UINT32		TraceIdx = 0;

	DCP_FUNCTION_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ChLowGet() Handle=%d", 
						Handle);

	if (Handle > 0)
	{
		ChId = Handle - 1;
		if(ChId < DCP_MAX_LOW_CHANNELS*2)	//240407lrg001: *2 because of FSU channel
		{
			/* ...build pointer to lower channel structure
			*/
			ChPtr = (DCP_CHL_PTR)(dcp_data.ChLowListPtr + ChId);

			/* Pointer is invalid if channel is not allocated
			*/ 
			if (ChPtr->State == DCP_CHA_STATE_FREE)
			{
				ChPtr = LSA_NULL;
			}
			else
			{
				TraceIdx = ChPtr->TraceIdx;
			}
		}
	}
	DCP_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ChLowGet() ChPtr=%X ChId=%d", 
						ChPtr, ChId);
    return(ChPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_ChGet()                                     */
/* Get pointer to channel structure by handle                                */
/*****************************************************************************/
DCP_CHA_PTR  DCP_LOCAL_FCT_ATTR  dcp_ChGet(
LSA_HANDLE_TYPE  Handle)
{
	LSA_HANDLE_TYPE	ChId = 0;
	DCP_CHA_PTR  	ChPtr = LSA_NULL;
	LSA_UINT32		TraceIdx = 0;

	DCP_FUNCTION_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE, "  > dcp_ChGet() Handle=%d", 
						Handle);

	/* If the handle is valid...
	*/
	if (Handle > 0)
	{
		ChId = Handle - 1;
		if(ChId < DCP_MAX_CHANNELS)
		{
			/* ...build pointer to lower channel structure
			*/
			ChPtr = (DCP_CHA_PTR)(dcp_data.ChListPtr + ChId);

			/* Pointer is invalid if channel is not allocated
			*/ 
			if (ChPtr->State == DCP_CHA_STATE_FREE)
			{
				ChPtr = LSA_NULL;
			}
			else
			{
				TraceIdx = ChPtr->TraceIdx;
			}
	    }
	}
	DCP_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE, "  < dcp_ChGet() ChPtr=%X ChId=%d", 
						ChPtr, ChId);
    return(ChPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_PutUpperQ()                                 */
/* Store upper RQB in queue                                                  */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_PutUpperQ(
DCP_UPPER_QUEUE		  *	QueuePtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	DCP_UPPER_RQB_PTR_TYPE  LastPtr;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_PutUpperQ() RbuPtr=%X QueuePtr=%X Count=%u", 
						RbuPtr, QueuePtr, QueuePtr->Count);

	/* Append RQB to the upper Queue
	*/
	LastPtr = QueuePtr->LastPtr;

	/* Mark new element as Queue end
	*/
	DCP_UPPER_RQB_NEXT_SET(RbuPtr, LSA_NULL);
	DCP_UPPER_RQB_PREV_SET(RbuPtr, LastPtr);
	QueuePtr->LastPtr = RbuPtr;
	
	/* If the queue was empty... 
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(LastPtr, LSA_NULL))
	{
		/* ...the last element is also the first one
		*/
		QueuePtr->FirstPtr = RbuPtr;
	}
	else
	{
		/* Connect the new last to the old last element
		*/
		DCP_UPPER_RQB_NEXT_SET(LastPtr, RbuPtr);
	}
	/* Queue now contains one more element
	*/
	QueuePtr->Count++;
	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_PutUpperQ() RbuPtr=%X QueuePtr=%X Count=%u", 
						RbuPtr, QueuePtr, QueuePtr->Count);
}

/*****************************************************************************/
/* Internal access function: dcp_EnqueUpper()	                             */
/* Store upper RQB in channel queue and return channel pointer               */
/*****************************************************************************/
DCP_CHA_PTR DCP_LOCAL_FCT_ATTR dcp_EnqueUpper(
DCP_UPPER_RQB_PTR_TYPE RbuPtr)
{
	DCP_CHA_PTR ChPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_EnqueUpper() RbuPtr=%X", 
						RbuPtr);

	ChPtr = dcp_ChGet(DCP_UPPER_RQB_HANDLE_GET(RbuPtr));
	if(!LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
	{
		/* Put RQB to the upper Queue
		*/
		dcp_PutUpperQ(&ChPtr->QueueUpper, RbuPtr);
	}

	DCP_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_EnqueUpper() RbuPtr=%X ChPtr=%X", 
						RbuPtr, ChPtr);
	return(ChPtr);
}

/*****************************************************************************/
/* Internal access function: dcp_GetUpperQ()	                             */
/* Remove upper Request block from Queue                                     */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_GetUpperQ(
DCP_UPPER_QUEUE		  *	QueuePtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	DCP_UPPER_RQB_PTR_TYPE  PrevPtr;
	DCP_UPPER_RQB_PTR_TYPE  NextPtr;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_GetUpperQ() RbuPtr=%X QueuePtr=%X Count=%u", 
						RbuPtr, QueuePtr, QueuePtr->Count);

	NextPtr = DCP_UPPER_RQB_NEXT_GET(RbuPtr);
	PrevPtr = DCP_UPPER_RQB_PREV_GET(RbuPtr);
	
	/* If a predecessor is present... 
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(PrevPtr, LSA_NULL))
	{
		/* ...set its successor to next element
		*/
		DCP_UPPER_RQB_NEXT_SET(PrevPtr, NextPtr);
	
	}
	else
	{
		/* The successor becomes first element of the queue
		*/
		QueuePtr->FirstPtr = NextPtr;
	
	}
	/* If a successor is present... 
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
	{
		/* ...set its predecessor to previous element
		*/
		DCP_UPPER_RQB_PREV_SET(NextPtr, PrevPtr);
	
	}
	else
	{
		/* The predecessor becomes last element of the queue
		*/
		QueuePtr->LastPtr = PrevPtr;
	
	}
	/* Delete chain pointers of removed RQB
	*/
	DCP_UPPER_RQB_PREV_SET(RbuPtr, LSA_NULL);
	DCP_UPPER_RQB_NEXT_SET(RbuPtr, LSA_NULL);
	
	/* Queue now contains one less element
	*/
	QueuePtr->Count--;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_GetUpperQ() RbuPtr=%X QueuePtr=%X Count=%u", 
						RbuPtr, QueuePtr, QueuePtr->Count);
}

/*****************************************************************************/
/* Internal access function: dcp_FindUpper()		                         */
/* Find upper Request block in a Queue                                       */
/*****************************************************************************/
LSA_BOOL    DCP_LOCAL_FCT_ATTR dcp_FindUpper(
DCP_UPPER_QUEUE		  *	QueuePtr,
DCP_UPPER_RQB_PTR_TYPE	RbuPtr)
{
	DCP_UPPER_RQB_PTR_TYPE  NextPtr;
	LSA_BOOL				RbFound = LSA_TRUE;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_FindUpper() RbuPtr=%X QueuePtr=%X Count=%u", 
						RbuPtr, QueuePtr, QueuePtr->Count);

	/* Search queue for RQB
	*/
	NextPtr = QueuePtr->FirstPtr;
	while (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, RbuPtr)
	   &&  !LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
	{
		NextPtr = DCP_UPPER_RQB_NEXT_GET(NextPtr);
	}

	if (LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
		RbFound = LSA_FALSE;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_FindUpper() RbuPtr=%X QueuePtr=%X RbFound=%u", 
						RbuPtr, QueuePtr, RbFound);
	return(RbFound);
}

/*****************************************************************************/
/* Internal access function: dcp_DequeUpper()		                         */
/* Find and dequeue upper RQB                                                */
/*****************************************************************************/
LSA_VOID    DCP_LOCAL_FCT_ATTR dcp_DequeUpper(
DCP_UPPER_RQB_PTR_TYPE RbuPtr)
{
	DCP_CHA_PTR ChPtr = LSA_NULL;

	DCP_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_DequeUpper() RbuPtr=%X", 
						RbuPtr);

	ChPtr = dcp_ChGet(DCP_UPPER_RQB_HANDLE_GET(RbuPtr));
    if(!LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL))
	{
		/* Find Request block in the Queue...
		*/
		if (dcp_FindUpper(&ChPtr->QueueUpper, RbuPtr))
		{
			/* ...and if found: remove it
			*/
			dcp_GetUpperQ(&ChPtr->QueueUpper, RbuPtr);
		}
	}

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_DequeUpper() RbuPtr=%X ChPtr=%X Count=%u", 
						RbuPtr, ChPtr, ChPtr->QueueUpper.Count);
}

/*****************************************************************************/
/* Internal access function: dcp_EnqueLower()	                             */
/* Store lower request block in queue                                        */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_EnqueLower(
DCP_LOWER_QUEUE		  *	QueuePtr,
DCP_LOWER_RQB_PTR_TYPE	RblPu,
DCP_CHA_PTR 			ChPtr)
{
	DCP_ETH_RQB_PTR_TYPE  LastPtr;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_EnqueLower() RbEthPtr=%X QueuePtr=%X ChPtr=%X", 
						RblPu.RbEthPtr, QueuePtr, ChPtr);

	if ((LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL)) 
	||  ((ChPtr->DetailPtr->DevId != DCP_DEV_ETH) && (ChPtr->DetailPtr->DevId != DCP_DEV_FSU)))
	{
	    dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, 0, ChPtr, 0);
	}
	else
	{
		/* Append RQB to the lower Queue
		*/
		LastPtr = QueuePtr->LastPu.RbEthPtr;

		/* Mark new element as Queue end
		*/
		DCP_ETH_RQB_NEXT_SET(RblPu.RbEthPtr, LSA_NULL);
		DCP_ETH_RQB_PREV_SET(RblPu.RbEthPtr, LastPtr);
		QueuePtr->LastPu.RbEthPtr = RblPu.RbEthPtr;
		
		/* If the queue was empty... 
		*/
		if (LSA_HOST_PTR_ARE_EQUAL(LastPtr, LSA_NULL))
		{
			/* ...the last element is also the first one
			*/
			QueuePtr->FirstPu.RbEthPtr = RblPu.RbEthPtr;
		
		}
		else
		{
			/* Connect the new last to the old last element
			*/
			DCP_ETH_RQB_NEXT_SET(LastPtr, RblPu.RbEthPtr);
		
		}
		/* Queue now contains one more element
		*/
		QueuePtr->Count++;
	}

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_EnqueLower() RbEthPtr=%X QueuePtr=%X Count=%u", 
						RblPu.RbEthPtr, QueuePtr, QueuePtr->Count);
}

/*****************************************************************************/
/* Internal access function: dcp_DequeLower()		                         */
/* Remove lower Request block from Queue                                     */
/*****************************************************************************/
LSA_VOID    DCP_LOCAL_FCT_ATTR dcp_DequeLower(
DCP_LOWER_QUEUE		  *	QueuePtr,
DCP_LOWER_RQB_PTR_TYPE 	RblPu,
DCP_CHA_PTR 			ChPtr)
{
	DCP_ETH_RQB_PTR_TYPE	PrevPtr;
	DCP_ETH_RQB_PTR_TYPE  	NextPtr;

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_DequeLower() RbEthPtr=%X QueuePtr=%X ChPtr=%X", 
						RblPu.RbEthPtr, QueuePtr, ChPtr);

	if ((LSA_HOST_PTR_ARE_EQUAL(ChPtr, LSA_NULL)) 
	||  ((ChPtr->DetailPtr->DevId != DCP_DEV_ETH) && (ChPtr->DetailPtr->DevId != DCP_DEV_FSU)))
	{
	    dcp_ErrorInternal(DCP_MODULE_ID, __LINE__, 0, ChPtr, 0);
	}
	else
	{
		/* Find Request block in the Queue...
		*/
		NextPtr = QueuePtr->FirstPu.RbEthPtr;
		while (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, RblPu.RbEthPtr)
		   &&  !LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
		{
			NextPtr = DCP_ETH_RQB_NEXT_GET(NextPtr);
		}
		/* ...and if found: remove it
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
		{
			NextPtr = DCP_ETH_RQB_NEXT_GET(RblPu.RbEthPtr);
			PrevPtr = DCP_ETH_RQB_PREV_GET(RblPu.RbEthPtr);
			
			/* If a predecessor is present... 
			*/
			if (!LSA_HOST_PTR_ARE_EQUAL(PrevPtr, LSA_NULL))
			{
				/* ...set its successor to next element
				*/
				DCP_ETH_RQB_NEXT_SET(PrevPtr, NextPtr);
			
			}
			else
			{
				/* The successor becomes first element of the queue
				*/
				QueuePtr->FirstPu.RbEthPtr = NextPtr;
			
			}
			/* If a successor is present... 
			*/
			if (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
			{
				/* ...set its predecessor to previous element
				*/
				DCP_ETH_RQB_PREV_SET(NextPtr, PrevPtr);
			
			}
			else
			{
			/* The predecessor becomes last element of the queue
				*/
				QueuePtr->LastPu.RbEthPtr = PrevPtr;
			
			}
			/* Delete chain pointers of removed RQB
			*/
			DCP_ETH_RQB_PREV_SET(RblPu.RbEthPtr, LSA_NULL);
			DCP_ETH_RQB_NEXT_SET(RblPu.RbEthPtr, LSA_NULL);
			
			/* Queue now contains one less element
			*/
			QueuePtr->Count--;
		}
	}

	DCP_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_DequeLower() RbEthPtr=%X QueuePtr=%X Count=%u", 
						RblPu.RbEthPtr, QueuePtr, QueuePtr->Count);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerEnque()                                */
/* Store lower timer block in queue                                          */
/*****************************************************************************/
LSA_VOID DCP_LOCAL_FCT_ATTR dcp_TimerEnque(
DCP_TIMER_QUEUE		  *	QueuePtr,
DCP_TIMER_PTR_TYPE		TimerPtr)
{
	DCP_TIMER_PTR_TYPE  LastPtr;

	DCP_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerEnque() TimerPtr=%X QueuePtr=%X First=%X Last=%X Count=%u", 
			TimerPtr, QueuePtr, QueuePtr->FirstPtr, QueuePtr->LastPtr, QueuePtr->Count);

	/* Append RQB to the timer Queue
	*/
	LastPtr = QueuePtr->LastPtr;

	/* Mark new element as Queue end
	*/
	TimerPtr->NextPtr = LSA_NULL;
	TimerPtr->PrevPtr = LastPtr;
	QueuePtr->LastPtr = TimerPtr;
	
	/* If the queue was empty... 
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(LastPtr, LSA_NULL))
	{
		/* ...the last element is also the first one
		*/
		QueuePtr->FirstPtr = TimerPtr;
	
	}
	else
	{
		/* Connect the new last to the old last element
		*/
		LastPtr->NextPtr =  TimerPtr;
	
	}
	/* Queue now contains one more element
	*/
	QueuePtr->Count++;

	DCP_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerEnque() TimerPtr=%X QueuePtr=%X First=%X Last=%X Count=%u", 
			TimerPtr, QueuePtr, QueuePtr->FirstPtr, QueuePtr->LastPtr, QueuePtr->Count);
}

/*****************************************************************************/
/* Internal access function: dcp_TimerDeque()                                */
/* Remove timer block from queue                                             */
/*****************************************************************************/
LSA_BOOL	DCP_LOCAL_FCT_ATTR dcp_TimerDeque(
DCP_TIMER_QUEUE			*QueuePtr,
DCP_TIMER_PTR_TYPE 		TimerPtr)
{
	DCP_TIMER_PTR_TYPE	PrevPtr;
	DCP_TIMER_PTR_TYPE	NextPtr;
	LSA_BOOL			Found = LSA_FALSE;

	DCP_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_NOTE, "  > dcp_TimerDeque() TimerPtr=%X QueuePtr=%X First=%X Last=%X Count=%u", 
			TimerPtr, QueuePtr, QueuePtr->FirstPtr, QueuePtr->LastPtr, QueuePtr->Count);

	/* Find timer block in the Queue...
	*/
	NextPtr = QueuePtr->FirstPtr;
	while (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, TimerPtr)
	   &&  !LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
	{
		NextPtr = NextPtr->NextPtr;
	}
	/* ...and if found: remove it
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
	{
		Found = LSA_TRUE;
		NextPtr = TimerPtr->NextPtr;
		PrevPtr = TimerPtr->PrevPtr;
		
		/* If a predecessor is present... 
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(PrevPtr, LSA_NULL))
		{
			/* ...set its successor to next element
			*/
			PrevPtr->NextPtr = NextPtr;
		
		}
		else
		{
			/* The successor becomes first element of the queue
			*/
			QueuePtr->FirstPtr = NextPtr;
		
		}
		/* If a successor is present... 
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(NextPtr, LSA_NULL))
		{
			/* ...set its predecessor to previous element
			*/
			NextPtr->PrevPtr = PrevPtr;
		
		}
		else
		{
			/* The predecessor becomes last element of the queue
			*/
			QueuePtr->LastPtr = PrevPtr;
		
		}
		/* Delete chain pointers of removed timer block
		*/
		TimerPtr->PrevPtr = LSA_NULL;
		TimerPtr->NextPtr = LSA_NULL;
		
		/* Queue now contains one less element
		*/
		QueuePtr->Count--;
	}

	DCP_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "  < dcp_TimerDeque() TimerPtr=%X QueuePtr=%X Count=%u Found=%u", 
			TimerPtr, QueuePtr, QueuePtr->Count, Found);
	
	return(Found);
}

/*****************************************************************************/
/* Internal access function: dcp_ErrorUser()                                 */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR dcp_ErrorUser(
LSA_INT				Module,
LSA_INT				Line,
LSA_INT  			ErrNum,
LSA_VOID_PTR_TYPE	Pointer,
LSA_UINT16			Size)
{
	dcp_data.Error.lsa_component_id = LSA_COMP_ID_DCP;
	dcp_data.Error.module_id = (LSA_UINT16)Module;
	dcp_data.Error.line = (LSA_UINT16)Line;
	dcp_data.Error.error_data_length = Size;
	dcp_data.Error.error_code[0] = ErrNum;
	dcp_data.Error.error_code[1] = 0;
	dcp_data.Error.error_code[2] = 0;
	dcp_data.Error.error_code[3] = 1;
	dcp_data.Error.error_data_ptr = Pointer;

	DCP_ERROR_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** dcp_ErrorUser() Size=%d Ptr=%X Module=%d Line=%d", 
						Size, Pointer, Module, Line);
	if ((Size != 0) && !LSA_HOST_PTR_ARE_EQUAL(Pointer, LSA_NULL))
		DCP_ERROR_TRACE_BYTE_ARRAY(0, LSA_TRACE_LEVEL_FATAL, "*** dcp_ErrorUser()", Pointer, Size);

	DCP_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &dcp_data.Error);
}

/*****************************************************************************/
/* Internal access function: dcp_ErrorLower()                                */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR dcp_ErrorLower(
LSA_INT				Module,
LSA_INT				Line,
LSA_INT  			ErrNum,
LSA_VOID_PTR_TYPE	Pointer,
LSA_UINT16			Size)
{
	dcp_data.Error.lsa_component_id = LSA_COMP_ID_DCP;
	dcp_data.Error.module_id = (LSA_UINT16)Module;
	dcp_data.Error.line = (LSA_UINT16)Line;
	dcp_data.Error.error_data_length = Size;
	dcp_data.Error.error_code[0] = ErrNum;
	dcp_data.Error.error_code[1] = 0;
	dcp_data.Error.error_code[2] = 0;
	dcp_data.Error.error_code[3] = 3;
	dcp_data.Error.error_data_ptr = Pointer;

	DCP_ERROR_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** dcp_ErrorLower() Size=%d Ptr=%X Module=%d Line=%d", 
						Size, Pointer, Module, Line);
	if ((Size != 0) && !LSA_HOST_PTR_ARE_EQUAL(Pointer, LSA_NULL))
		DCP_ERROR_TRACE_BYTE_ARRAY(0, LSA_TRACE_LEVEL_FATAL, "*** dcp_ErrorLower()", Pointer, Size);

    DCP_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &dcp_data.Error);
}

/*****************************************************************************/
/* Internal access function: dcp_ErrorInternal()                             */
/*****************************************************************************/
LSA_VOID  DCP_LOCAL_FCT_ATTR dcp_ErrorInternal(
LSA_INT				Module,
LSA_INT				Line,
LSA_INT  			ErrNum,
LSA_VOID_PTR_TYPE	Pointer,
LSA_UINT32			Value)
{
	dcp_data.Error.lsa_component_id = LSA_COMP_ID_DCP;
	dcp_data.Error.module_id = (LSA_UINT16)Module;
	dcp_data.Error.line = (LSA_UINT16)Line;
	dcp_data.Error.error_code[0] = ErrNum;
	dcp_data.Error.error_code[1] = Value;
	dcp_data.Error.error_code[2] = 0;
	dcp_data.Error.error_code[3] = 2;
	dcp_data.Error.error_data_ptr = Pointer;
	dcp_data.Error.error_data_length = 	0;

	DCP_ERROR_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** dcp_ErrorInternal() Value=%d Ptr=%X Module=%d Line=%d", 
						Value, Pointer, Module, Line);

	DCP_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &dcp_data.Error);
}

/*****************************************************************************/
/*  end of file DCP_SYS.C                                                    */
/*****************************************************************************/
