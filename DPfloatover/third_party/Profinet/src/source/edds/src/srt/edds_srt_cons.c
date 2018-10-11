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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_srt_cons.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDS. Consumer          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  06.06.02    JS    initial version.                                       */
/*  17.10.03    JS    Support for Consumer-Control "SetToUnknownState"       */
/*  03.12.03    JS    Always wait at least one check-cycle after consumer    */
/*                    state was SetToUnknown before indicating the state.    */
/*  04.12.03    JS    Added CycleCounter-Support of Consumer                 */
/*                    Dont check bits 1,6,7 anymore in DataStatus.           */
/*  20.01.04    JS    pBuffer-Parameter removed from consumer-add            */
/*  21.01.04    JS    Changed CycleBaseFactor and ReductionRatio interpretion*/
/*  04.03.04    JS    Back2Back check added.                                 */
/*  10.03.04    JS    added support for tasking-compiler V2.0                */
/*  05.04.04    JS    changes to prevent compiler warnings                   */
/*                    Using LSA_UNUSED_ARG                                   */
/*  27.07.04    JS    EDDS_ENTER_MIDDLE and EDDS_EXIT_MIDDLE added to support */
/*                    middle-prio with consumer check                        */
/*  01.10.04    JS    Bufix. Wrong structure access if JITTER-check-enabled  */
/*  24.11.04    JS    added AcceptDataInvalid parameter                      */
/*                    added EDDS_CFG_ALWAYS_LOCK_CONSUMER_SCORE_CHECK        */
/*  08.12.04    JS    WatchdogFactor and DataHoldFactor range check changed  */
/*                    Rounding up if not a multiple of MIN                   */
/*  31.01.05    JS    LTRC support                                           */
/*  01.03.05    JS    added more parameter checks on consumer add (currently */
/*                    inactive)                                              */
/*  10.03.05    JS    Made CycleCntStamp different for xrt/udp (16/32Bit)    */
/*  15.03.05    JS    changed consumer check to receive stamp compare.       */
/*  08.08.05    JS    added SkipCycleStamp for better (less jitter) behaviour*/
/*                    after a "settounknown". Now more independend from      */
/*                    Consumer timer jitter.                                 */
/*  10.08.05    JS    set SkipCycleStamp if EDDS_CSRT_CONS_SKIP_STATE3       */
/*  11.10.05    JS    changed a memory attribute to avoid compiler warning   */
/*  10.11.05    JS    Added CycleCnt to EDDS_CSRTIndicationFill              */
/*  25.01.06    JS    DataStatus will be stored even if DataInvalid.         */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  26.07.07    JS    added  EDDS_CFG_CSRT_OVERLOAD_TRACE_LEVEL              */
/*  02.06.08    JS    setup new parameter APDUStatusOffset with 0 (not used) */
/*  30.01.09    JS    consumer events changed from UINT16 to UINT32          */
/*  24.09.09    JS    removed support for EDD_CSRT_ID_ALL                    */
/*                    removed support for AcceptDataInvalid                  */
/*                    added new CONSUMER_CONTROL User IF                     */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  146
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SRT_CONS */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

#include "edds_iobuf_int.h"

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_SRT_INCLUDE

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

#undef EDDS_SAVE_DATASTATUS_ONLY_IF_GOOD

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTRxIndicateCyclicXRT                  +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             LSA_UINT32                     Length       +*/
/*+                             LSA_UINT16                     FrameID      +*/
/*+                             LSA_BOOL                       VLANTagPresent*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB           : Pointer to DeviceDescriptionBlock                     +*/
/*+  pBuffer        : Address for pointer to rx-buffer (input and output)   +*/
/*+  Length         : Length of received data.(input)                       +*/
/*+  FrameID        : valid cyclic FrameID                                  +*/
/*+  VLANTagPresent : LSA_TRUE if VLAN-Tag is present in frame              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called after a cyclic SRT-Frame was recv.+*/
/*+               The function is called within int-handler and has to      +*/
/*+               free the buffer and provide a new one.                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pBuffer,pDDB,pCSRTComp,pCSRTTable)
//JB 18/11/2014 checked before this function is called - pCSRTComp is taken from pDDB - pCSRTTable set LSA_NULL and initialized
//WARNING: be careful when using this function, make sure not to use pBuffer or pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTRxIndicateCyclicXRT(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBuffer,
LSA_UINT32 Length,
LSA_UINT16 FrameID,
LSA_BOOL VLANTagPresent)

{

	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	EDDS_CSRT_CONSUMER_TYPE_PTR pCSRTTable;
	EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pCSRTFrameTable;
	LSA_UINT32 Len;
	LSA_UINT8 DataStatus;
	EDD_UPPER_MEM_U8_PTR_TYPE pEpilog;
#ifndef EDDS_CFG_SRT_DONT_CHECK_CYCLE_COUNTER
	LSA_UINT16 CycleCounter;
	LSA_UINT16 Delta;
#endif
	LSA_BOOL Good;
	LSA_UINT16 ConsumerId;

#define DATA_STATUS_GOOD_MASK     (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_GOOD_VALUE    (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_STATE_MASK    (EDD_CSRT_DSTAT_BIT_STATE)
#define DATA_STATUS_STATE_VALUE   (EDD_CSRT_DSTAT_BIT_STATE)

	EDDS_ISR_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,
	"IN :EDDS_RTRxIndicateCyclicXRT(pDDB: 0x%X,*pBuffer: 0x%X, Len: %d, FId: 0x%X)",
	pDDB,
	*pBuffer,
	Length,
	FrameID);

	pCSRTComp = &pDDB->pSRT->CSRT;
	pCSRTFrameTable = LSA_NULL;
	pCSRTTable = LSA_NULL;

	/* check FrameID for valid range. FrameID is within valid CSRT range on entry. */

	if (( FrameID >= pCSRTComp->Cons.ConsumerFrameIDBase1) &&
	( FrameID <= pCSRTComp->Cons.ConsumerFrameIDTop1 ))
	{
		ConsumerId = FrameID-pCSRTComp->Cons.ConsumerFrameIDBase1;
		pCSRTFrameTable = &pCSRTComp->Cons.pConsFrameTbl[ConsumerId];
		pCSRTTable = &pCSRTComp->Cons.pConsTbl[ConsumerId];
	}
	else
	{
		if (( FrameID >= pCSRTComp->Cons.ConsumerFrameIDBase2) &&
		( FrameID <= pCSRTComp->Cons.ConsumerFrameIDTop2 ))
		{
			ConsumerId = FrameID-pCSRTComp->Cons.ConsumerFrameIDBase2;
			pCSRTFrameTable = &pCSRTComp->Cons.pConsFrameTbl2[ConsumerId];
			pCSRTTable = &pCSRTComp->Cons.pConsTbl2[ConsumerId];
		}
	}

	if ( ! LSA_HOST_PTR_ARE_EQUAL(pCSRTFrameTable,LSA_NULL))
	{
		/* Framelength to check for. if VLAN-Tag not present adjust length.         */

		if (! VLANTagPresent) Len = pCSRTFrameTable->FrameLength - EDD_VLAN_TAG_SIZE;
		else Len = pCSRTFrameTable->FrameLength;

		/* we use optimized access to the MAC-address! The start of the MAC-address */
		/* has to be DWORD-aligned. So the Rx-Buffer has to be DWORD-aligned!       */

		if (pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_0) {
			if(( (pCSRTFrameTable->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_XRT ) &&
			( Length == Len ) &&
			(
					( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[0] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6/*+0*/] )
					&& ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[1] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+1] )
					&& ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[2] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+2] )
					&& ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[3] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+3] )
					&& ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[4] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+4] )
					&& ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[5] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+5] )
			)
			)
			{

				/* check CycleCounter, DataStatus + Transfer-State                 */
				/* cannot be accessed together because of alignment !              */
				/* (maybe not neccessary for system without alignment requirements */
				/*  but we dont do different handling now and dont use MACROs to   */
				/*  be sure whats going on here..)                                 */

				/* set pEpilog to last Byte of frame (TransferStatus)              */
				pEpilog = &((EDD_UPPER_MEM_U8_PTR_TYPE)(*pBuffer))[Length-1];

				/* check TransferStatus and move pEpilog to DataStatus */
				if ( *pEpilog-- == 0 )
				{
					LSA_BOOL IsBackup /*= LSA_FALSE*/;
					LSA_BOOL IsDataValid /*= LSA_FALSE*/;

					/* Read DataStatus */
					DataStatus = *pEpilog--;

					IsBackup = !( (DataStatus & DATA_STATUS_STATE_MASK) == DATA_STATUS_STATE_VALUE );
					IsDataValid = ( (DataStatus & DATA_STATUS_GOOD_MASK) == DATA_STATUS_GOOD_VALUE );

					if(   IsDataValid ||
						(!IsDataValid && IsBackup) )  //lint !e774 JB 05/12/2014 readability
                                          //@fixme NEA6.0 
                                          //AD 12/12/2014 after discussion with TP, the condition
                                          //IsDataValid || (!isDataValid && IsBackup) could be simplified to
                                          //(IsDataValid || isBackup) or the condition could completly removed because is always true 
                                          //and then we can remove the lint comment, 
                                      
					{
						Good = LSA_TRUE;

						/* pEpilog now points to last byte of CycleCounter */
						/* read CycleCounter (Big-Endian!)                 */
						CycleCounter = (LSA_UINT16) ((((LSA_UINT16)*(pEpilog-1)) << 8) + *pEpilog);

						/* check CycleCounter only if this is not the */
						/* first frame after activate.                */

						if ( pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_3 )
						{
							/* The CycleCounter is older as the current one, if the value   */
							/* is within 4096 digits behind the current one. because of the */
							/* wraparound we have to do a little math..                     */
							/*                                                              */
							/*                                                              */
							/* 0                                 0xFFFF                     */
							/* |--------xxxxxxO...........N--------|                        */
							/*          |older|                                             */
							/*           4096                                               */
							/*                                                              */
							/* |..N-----xxxxxxO....................|                        */
							/*          |older|                                             */
							/*           4096                                               */
							/* |xxxxO..............N-------------xx|                        */
							/*  lder|                            |o                         */
							/*                                                              */
							/*  N: new Counter                                              */
							/*  O: old Counter                                              */
							/*                                                              */
							/*  Delta = (LSA_UINT16) (N-O)                                  */
							/*                                                              */
							/*  Note: a CycleCounter equal (Delta == 0) is handled as old   */
							Delta = CycleCounter - pCSRTFrameTable->CycleCounter;
							if ( (Delta == 0) || (Delta > 0xF000) ) /* between -4095..0 */
							{
								Good = LSA_FALSE;
								pCSRTComp->Stats.ConsRxCycleCounterOld++;
							}
							if(EDDS_SRT_CONS_STATE_ON_3 != pCSRTFrameTable->State)
							{
								pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame after SetToUnknown was received */
							}
						}
						else
						{
							pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame received */
						}

						if ( Good )  // old/new frame ok
						{
							pCSRTFrameTable->CycleCounter = CycleCounter; /* save actual CycleCounter*/
#ifndef EDDS_SAVE_DATASTATUS_ONLY_IF_GOOD
							pCSRTFrameTable->DataStatus = DataStatus; /* save actual DataStatus  */
#endif
							pCSRTFrameTable->StampAddr.xrt.CycleCntStamp = (LSA_UINT16) pCSRTComp->Prov.ProviderCycle.CycleCounter;

#ifdef EDDS_CFG_SRT_USE_RXCNT_STATISTIC
							pCSRTComp->Stats.RxCnt++;
#endif

							if( IsDataValid )
							{
								LSA_UINT16 EDDS;
								LSA_UINT8* pValue;
								EDDS_EXCHANGE_TYPE Temp;
								EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;

								pCr = &pCSRTComp->Cons.pConsCrTbl[pCSRTTable->CrNumber - 1]; // 1 .. ConsumerCnt

								// NOTE:
								// Information like index and new flag is always stored in one byte in
								// the EXCHANGE variables at its lowest addresses. This applies to EDDS and
								// IOBuffer when accessing these variables. Access to this byte by byte
								// pointer does not need any consideration of endianess.
								// example:
								// Value stored in memory for Xchange with reference to buffer 1 and
								// new flag is 0x8100. Index and new flag can be retrieved by accessing
								// Xchange with its address casted to a byte pointer independently from
								// host endianess.
								//
								// index = (*((LSA_UINT8*)&Xchange)) & ~MASK

								// set reference in EDDS to received buffer
								EDDS = *((LSA_UINT8*)&pCr->EDDS);
								pCr->pBuffer[EDDS] = *pBuffer;

								Temp = pCr->EDDS;

								// set new flag in Temp
								pValue = ((LSA_UINT8*)&Temp);
								*pValue |= EDDS_IOBUFFER_MASK_NEW_FLAG;

								Temp = EDDS_IOBUFFER_EXCHANGE( &pCr->XChange, Temp );

								// reset new flag in User
								pValue = ((LSA_UINT8*)&Temp);
								*pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;

								pCr->EDDS = Temp;

								// return buffer referenced in EDDS as new receive resource
								// NOTE: Buffer that is currently used in EDDS as receive resource must be
								//       set to NULL!
								EDDS = *((LSA_UINT8*)&pCr->EDDS);
								*pBuffer = pCr->pBuffer[EDDS];
								pCr->pBuffer[EDDS] = LSA_NULL;
							}		// exchange buffer --> only when data valid

						}					      // old/new frame check
					}					      // data valid
				}					      // transfer status != 0
			}					      // length and mac check
			else
			{
				pCSRTComp->Stats.ConsRxWrongXRTLenOrMAC++;
			}
		}
	}

	EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
	"OUT:EDDS_RTRxIndicateCyclicXRT(*pBuffer: 0x%X)",
	*pBuffer);
}
//lint +esym(613,pBuffer,pDDB,pCSRTComp,pCSRTTable)

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTRxIndicateCyclicUDP                  +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...     pUDPData     +*/
/*+                             EDDS_IP_HEADER_TYPE           *pHeader      +*/
/*+                             LSA_UINT32                     UDPLen       +*/
/*+                             LSA_UINT16                     FrameID      +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB           : Pointer to DeviceDescriptionBlock                     +*/
/*+  pBuffer        : Address for pointer to rx-buffer (input and output)   +*/
/*+  pUDPData       : Pointer to UDP-Data start within pBuffer              +*/
/*+  pHeader        : Pointer to IPHeader (32-Bit aligned! not in frame!!)  +*/
/*+  UDPLen         : Length of UDP-User Data                               +*/
/*+  FrameID        : valid cyclic FrameID                                  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE         No free rx-resources         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called after a cyclic xRT over UDP frame +*/
/*+               was received. IP/UDP-Header already checked.              +*/
/*+               The function is called within int-handler and has to      +*/
/*+               free the buffer and provide a new one.                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTRxIndicateCyclicUDP(
EDDS_LOCAL_DDB_PTR_TYPE pDDB,
EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBuffer,
EDD_UPPER_MEM_PTR_TYPE pUDPData,
EDDS_IP_HEADER_TYPE * pHeader,
LSA_UINT32 UDPLen,
LSA_UINT16 FrameID)
{

	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	EDDS_CSRT_CONSUMER_TYPE_PTR pCSRTTable;
	EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pCSRTFrameTable;
	EDD_UPPER_MEM_PTR_TYPE pTemp;
	LSA_UINT8 DataStatus;
	EDD_UPPER_MEM_U8_PTR_TYPE pEpilog;
	LSA_UINT16 CycleCounter;
	LSA_UINT16 Delta;
	LSA_BOOL Good;

#define DATA_STATUS_GOOD_MASK     (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_GOOD_VALUE    (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_STATE_MASK    (EDD_CSRT_DSTAT_BIT_STATE)
#define DATA_STATUS_STATE_VALUE   (EDD_CSRT_DSTAT_BIT_STATE)

	EDDS_ISR_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,
	"IN :EDDS_RTRxIndicateCyclicUDP(pDDB: 0x%X,*pBuffer: 0x%X, Len: %d, FId: 0x%X)",
	pDDB,
	*pBuffer,
	UDPLen,
	FrameID);

	pCSRTComp = &pDDB->pSRT->CSRT;
	pCSRTFrameTable = LSA_NULL;
	pCSRTTable = LSA_NULL;

	/* check FrameID for valid range   */
	/* UDP only possible within array2 */

	if (( FrameID >= pCSRTComp->Cons.ConsumerFrameIDBase2) &&
	( FrameID <= pCSRTComp->Cons.ConsumerFrameIDTop2 ))
	{

		pCSRTFrameTable = &pCSRTComp->Cons.pConsFrameTbl2[FrameID-pCSRTComp->Cons.ConsumerFrameIDBase2];
		pCSRTTable = &pCSRTComp->Cons.pConsTbl2[FrameID-pCSRTComp->Cons.ConsumerFrameIDBase2];

		if (pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_0 ) {
			if(( (pCSRTFrameTable->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_UDP ) &&
			( pCSRTFrameTable->FrameLength == UDPLen ) &&
			( pCSRTFrameTable->StampAddr.udp.SrcIP.dw == pHeader->SrcIP.dw ))
			{

				/* check CycleCounter, DataStatus + Transfer-State                 */
				/* cannot be accessed together because of alignment !              */
				/* (maybe not neccessary for system without alignment requirements */
				/*  but we dont do different handling now and dont use MACROs to   */
				/*  be sure whats going on here..)                                 */

				/* set pEpilog to last Byte of frame (TransferStatus)              */
				pEpilog = &((EDD_UPPER_MEM_U8_PTR_TYPE) pUDPData)[UDPLen-1];

				/* check TransferStatus and move pEpilog to DataStatus */
				if ( *pEpilog-- == 0 )
				{
					LSA_BOOL IsBackup = LSA_FALSE;
					LSA_BOOL IsDataValid = LSA_FALSE;

					/* Read DataStatus */
					DataStatus = *pEpilog--;

					IsBackup = !( (DataStatus & DATA_STATUS_STATE_MASK) == DATA_STATUS_STATE_VALUE );
					IsDataValid = ( (DataStatus & DATA_STATUS_GOOD_MASK) == DATA_STATUS_GOOD_VALUE );

					if( IsDataValid ||
					(!IsDataValid && IsBackup) ) //lint !e774 JB 05/12/2014 readability
                                       //@fixme NEA6.0 
                                       //AD 12/12/2014 after discussion with TP, the condition
                                       //IsDataValid || (!isDataValid && IsBackup) could be simplified to
                                       //(IsDataValid || isBackup) or the condition could completly removed because is always true 
                                       //and then we can remove the lint comment, 
					{
						Good = LSA_TRUE;

						/* pEpilog now points to last byte of CycleCounter */
						/* read CycleCounter (Big-Endian!)                 */
						CycleCounter = (((LSA_UINT16)*(pEpilog-1)) << 8) + *pEpilog;

						/* check CycleCounter only if this is not the */
						/* first frame after activate.                */

						if (pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_3)
						{

							/* The CycleCounter is older as the current one, if the value   */
							/* is within 4096 digits behind the current one. because of the */
							/* wraparound we have to do a little math..                     */
							/*                                                              */
							/*                                                              */
							/* 0                                 0xFFFF                     */
							/* |--------xxxxxxO...........N--------|                        */
							/*          |older|                                             */
							/*           4096                                               */
							/*                                                              */
							/* |..N-----xxxxxxO....................|                        */
							/*          |older|                                             */
							/*           4096                                               */
							/* |xxxxO..............N-------------xx|                        */
							/*  lder|                            |o                         */
							/*                                                              */
							/*  N: new Counter                                              */
							/*  O: old Counter                                              */
							/*                                                              */
							/*  Delta = (LSA_UINT16) (N-O)                                  */
							/*                                                              */
							/*  Note: a CycleCounter equal (Delta == 0) is handled as old   */

							Delta = CycleCounter - pCSRTFrameTable->CycleCounter;
							if ( (Delta == 0) || (Delta > 0xF000) ) /* between -4095..0 */
							{
								Good = LSA_FALSE;
								pCSRTComp->Stats.ConsRxCycleCounterOld++;
							}
							if(EDDS_SRT_CONS_STATE_ON_3 != pCSRTFrameTable->State)
							{
								pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame after SetToUnknown was received */
							}
						}
						else
						{
							pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame received */
						}

						if ( Good ) // old/new frame check
						{
							pCSRTFrameTable->CycleCounter = CycleCounter; /* save actual CycleCounter*/
#ifndef EDDS_SAVE_DATASTATUS_ONLY_IF_GOOD
							pCSRTFrameTable->DataStatus = DataStatus; /* save actual DataStatus  */
#endif
							pCSRTFrameTable->StampAddr.udp.CycleCntStamp = pCSRTComp->Prov.ProviderCycle.CycleCounter;

#ifdef EDDS_CFG_SRT_USE_RXCNT_STATISTIC
							pCSRTComp->Stats.RxCnt++;
#endif

							if( IsDataValid )
							{
								LSA_UINT16 EDDS;
								LSA_UINT8* pValue;
								EDDS_EXCHANGE_TYPE Temp;
								EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;

								pCr = &pCSRTComp->Cons.pConsCrTbl[pCSRTTable->CrNumber - 1]; // 1 .. ConsumerCnt

								// set reference in EDDS to received buffer
								EDDS = *((LSA_UINT8*)&pCr->EDDS);
								pCr->pBuffer[EDDS] = *pBuffer;

								Temp = pCr->EDDS;

								// set new flag in Temp
								pValue = ((LSA_UINT8*)&Temp);
								*pValue |= EDDS_IOBUFFER_MASK_NEW_FLAG;

								Temp = EDDS_IOBUFFER_EXCHANGE( &pCr->XChange, Temp );

								// reset new flag in User
								pValue = ((LSA_UINT8*)&Temp);
								*pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;

								pCr->EDDS = Temp;

								// return buffer referenced in EDDS as new receive resource
								// NOTE: Buffer that is currently used in EDDS as receive resource must be
								//       set to NULL!
								EDDS = *((LSA_UINT8*)&pCr->EDDS);
								*pBuffer = pCr->pBuffer[EDDS];
								pCr->pBuffer[EDDS] = LSA_NULL;
							}        // exchange buffer --> only when data valid

						}                // old/new frame check
					}                // data valid
				}                // transfer status != 0
			}                // length and mac check
			else
			{
				pCSRTComp->Stats.ConsRxWrongUDPLenOrIP++;
			}
		}
	}

	EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
	"OUT:EDDS_RTRxIndicateCyclicUDP(*pBuffer: 0x%X)",
	*pBuffer);
}

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :  EDDS_CSRTConsumerHandleClearOnMiss            +*/
/*+  Input/Output          :  EDDS_DDB_COMP_CSRT_TYPE_PTR       pCSRTComp   +*/
/*+                           EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame  +*/
/*+                           EDDS_CSRT_CONSUMER_TYPE_PTR       pCons       +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to CSRT-Management.                               +*/
/*+  pConsFrame : Pointer to Consumer management                            +*/
/*+  pCons      : Pointer to Consumer management                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles a MISS of a consumer.               +*/
/*+                                                                         +*/
/*+               If the ClearOnMiss flag is set for this consumer a null   +*/
/*+               buffer with New flag is exchanged. Otherwise nothing is   +*/
/*+               done.                                                     +*/
/*+                                                                         +*/
/*+               The consumer should have already received valid frames.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp, pConsFrame, pCons)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: Be careful when using this function, make sure not to use any null ptr's!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CSRTConsumerHandleClearOnMiss(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame,
		EDDS_CSRT_CONSUMER_TYPE_PTR pCons) {
	/* ClearOnMiss flag set? */
	if (pConsFrame->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS) {
		LSA_UINT16 EDDS;
		LSA_UINT8* pValue;
		EDDS_EXCHANGE_TYPE Temp;
		EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;
		EDD_UPPER_MEM_U8_PTR_TYPE pBuffer;
		LSA_UINT16 Len = 0;

		/* get frame length without VLAN tag depending on consumer type */
		if (pConsFrame->Type & EDDS_CSRT_CONSUMER_TYPE_XRT) {
			Len = EDDS_CSRT_FRAME_IO_START_WITHOUT_VLAN + pCons->DataLen;
		}
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		else if
		{
			Len = EDDS_CSRT_FRAME_IO_START_WITHOUT_VLAN +
			sizeof(EDDS_IP_HEADER_TYPE) + sizeof(EDDS_UDP_HEADER_TYPE) + sizeof(EDDS_FRAME_ID_TYPE) +
			pCons->DataLen; /* 4 bytes APDU status */
		}
#endif
		else {
			EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,
					"EDDS_CSRTConsumerHandleClearOnMiss: Illegal consumer type %d",
					pConsFrame->Type);
			EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
			EDDS_MODULE_ID,
			__LINE__);
		}

		/* clear buffer */
		/* NOTE: clear + 4 bytes for APDU status --> TransferStatus = 0 */
		EDDS_MEMSET_UPPER(pCSRTComp->Cons.pClearOnMissBuffer, 0, Len + 4);

		/* setup APDU status in ClearOnMiss buffer */
		pBuffer =
				(EDD_UPPER_MEM_U8_PTR_TYPE) pCSRTComp->Cons.pClearOnMissBuffer;
		pBuffer[Len++] = (LSA_UINT8) (pConsFrame->CycleCounter >> 8);
		pBuffer[Len++] = (LSA_UINT8) (pConsFrame->CycleCounter & 0xFF);
		pBuffer[Len] = (LSA_UINT8) (pConsFrame->DataStatus);

		/* setup frame without VLAN tag */
		/* NOTE: IOBuffer determines the offset to IO data depending on present VLAN tag */
		((EDD_UPPER_MEM_U16_PTR_TYPE ) pCSRTComp->Cons.pClearOnMissBuffer)[EDDS_FRAME_LEN_TYPE_WORD_OFFSET] =
				(LSA_UINT16) ~EDDS_VLAN_TAG;

		/* get consumer Cr */
		pCr = &pCSRTComp->Cons.pConsCrTbl[pCons->CrNumber - 1]; // 1 .. ConsumerCnt

		/* set reference in EDDS to received buffer */
		EDDS = *((LSA_UINT8*) &pCr->EDDS);
		pCr->pBuffer[EDDS] = pCSRTComp->Cons.pClearOnMissBuffer;

		/* set new flag in EDDS entry */
		Temp = pCr->EDDS;
		pValue = ((LSA_UINT8*) &Temp);
		*pValue |= EDDS_IOBUFFER_MASK_NEW_FLAG;

		/* exchange EDDS with XChange entry */
		Temp = EDDS_IOBUFFER_EXCHANGE(&pCr->XChange, Temp);

		/* reset new flag in User */
		pValue = ((LSA_UINT8*) &Temp);
		*pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
		pCr->EDDS = Temp;

		/* return buffer referenced in EDDS as new ClearOnMiss buffer */
		/* NOTE: Buffer that is currently used in EDDS must be set to NULL! */
		EDDS = *((LSA_UINT8*) &pCr->EDDS);
		pCSRTComp->Cons.pClearOnMissBuffer = pCr->pBuffer[EDDS];
		pCr->pBuffer[EDDS] = LSA_NULL;
	}
}
//lint +esym(613,pCSRTComp, pConsFrame, pCons)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTConsumerActualizeState             +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR    pCSRTComp    +*/
/*+                             LSA_UINT32                     ConsumerID   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to CSRT-Management.                               +*/
/*+  ConsIdx    : ConsumerID to actualize state for                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function actualizes the ActStatusFlag within Consumer+*/
/*+               management if not the first check for this consumer.      +*/
/*+                                                                         +*/
/*+               It checks the Score-Flag and DataStatus for the consumer  +*/
/*+               After this function EDDS_CSRTConsumerCheckState should be +*/
/*+               called to get events.                                     +*/
/*+                                                                         +*/
/*+               The consumer should be active (not checked)               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CSRTConsumerActualizeState(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		LSA_UINT32 ConsumerID) {

	EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
	EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
	LSA_UINT32 Delta;

	pCons = &pCSRTComp->Cons.pConsTbl[ConsumerID];
	pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[ConsumerID];

	EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,
			"EDDS_CSRTConsumerActualizeState: ConsID %d (state %d)", ConsumerID,
			pConsFrame->State);

	/* check if we have to skip here. if EDDS_CSRT_CONS_SKIP_STATE2 we have  */
	/* to wait at least one check cycle so we check time stamps to be sure   */
	/* (because consumer timer event may jitter to much!)                    */
	if ((pConsFrame->State < EDDS_SRT_CONS_STATE_ON_0)
			|| (pCSRTComp->Prov.ProviderCycle.CycleCounter
					- pCons->SkipCycleStamp <= pCons->WatchdogCycleCnt)) {
		//pConsFrame->State = pConsFrame->State;
	} else {

		/* PAST: */
		/* Because we may reset the  EDDS_SRT_CONS_STATE_ACCEPT_DATA_INVALID flag*/
		/* we have to LOCK the access against ISR (read modify write). Further   */
		/* we have to be sure, that if resetting the flag we also reset we dont  */
		/* get a new Score-Flag for a frame with INVALID. So we have to lock the */
		/* whole sequence (testing score, resetting State-INVALID-Flag)          */
		/** @todo:
		 * 	remove EDDS_LOCK() due no reset of State-INVALID-flag?
		 */

		/* Additionally it is more accurate to do the lock alltime because       */
		/* between checking score-flag and resetting it there may be a time gap  */
		/* (by task switch etc.) which may result in receiving a new frame which */
		/* will be ignored.                                                      */

		EDDS_LOCK();

		/* Get the Delta in Cylce since last receive (only valid if "Score" set */
		/* checked below )                                                      */

		/* Important: CycleCntStamp must be read together or before CycleCounter */
		/*            to avoid interruption problem (it is possible, that        */
		/*            we are interupted here by ProviderCycle and incoming       */
		/*            frames! We lock access here, because reading the two       */
		/*            counters together gets the best accurate result.           */

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		if( (pCons->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_UDP )
		{
			/* UDP Consumer have a 32 Bit Stamp!*/
			Delta = pCSRTComp->Prov.ProviderCycle.CycleCounter - pConsFrame->StampAddr.udp.CycleCntStamp;
		}
		else
#endif
		{
			/* xRT Consumer have a 16 Bit Stamp!*/
			Delta = (LSA_UINT16) (pCSRTComp->Prov.ProviderCycle.CycleCounter
					- pConsFrame->StampAddr.xrt.CycleCntStamp);
		}

		/* Score is set after we received a good frame, reset on MISS*/

		if (Delta > pCons->WatchdogCycleCnt) {
			switch (pConsFrame->State) {
			case EDDS_SRT_CONS_STATE_ON_0:
			case EDDS_SRT_CONS_STATE_ON_2:
				/* do nothing */
				break;
			case EDDS_SRT_CONS_STATE_ON_1:
				pConsFrame->State = EDDS_SRT_CONS_STATE_ON_MNS;
				break;
			case EDDS_SRT_CONS_STATE_ON_AGAIN:
			case EDDS_SRT_CONS_STATE_ON_3:
			case EDDS_SRT_CONS_STATE_ON_4:
				pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_MISS;
				EDDS_CSRTConsumerHandleClearOnMiss(pCSRTComp, pConsFrame,
						pCons);
				break;
			default:
				EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,
						"EDDS_CSRTConsumerActualizeState: Illegal state %d, ConsID: %d",
						pConsFrame->State, ConsumerID);
				EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
				EDDS_MODULE_ID,
				__LINE__);
				break;
			}
			EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,
					"EDDS_CSRTConsumerActualizeState: ConsID %d -->state %d",
					ConsumerID, pConsFrame->State);
		} else {
			/* Data-Status only valid if ScoreFlag is set !! */
			/* Copy Data-Status bits ot ActStatBits          */

			pCons->ActStatusFlags &= ~EDDS_CSRT_CONS_STAT_DSTAT_MSK;
			pCons->ActStatusFlags |= (pConsFrame->DataStatus
					& EDDS_CSRT_CONS_STAT_DSTAT_MSK);
		}

		EDDS_UNLOCK();
	}

}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTConsumerCheckState                 +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR    pCSRTComp    +*/
/*+                             LSA_UINT32                     ConsumerID   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to CSRT-Management.                               +*/
/*+  ConsIdx    : ConsumerID to actualize state for                         +*/
/*+                                                                         +*/
/*+  Result     : LSA_FALSE no more indication resources present.           +*/
/*+               LSA_TRUE  more indication resources present.              +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Looks if consumer indication events are present which     +*/
/*+               have not been signaled to user yet and send them (if      +*/
/*+               indication resources are present).                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_CSRTConsumerProcessState(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		LSA_UINT32 ConsumerID) {

	LSA_UINT8 Act, Ind, Change;
	LSA_UINT32 Event = 0;
	LSA_UINT32 EventStatus = 0;

	EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame =
			&pCSRTComp->Cons.pConsFrameTbl[ConsumerID];

	Act = pCSRTComp->Cons.pConsTbl[ConsumerID].ActStatusFlags;
	Ind = pCSRTComp->Cons.pConsTbl[ConsumerID].IndStatusFlags;

	/* for every status we have on bit. we check if this bit has  changed since */
	/* last indication. this is the case if the bit in act and ind is different */
	/* with act xor ind we get all bits set which differs..                     */

	Change = Act ^ Ind; /* bit is 1 if changed */

	/* check MISS/AGAIN change */

	switch (pConsFrame->State) {
	case EDDS_SRT_CONS_STATE_OFF_MNS:
		pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_0;
		Event = EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
		break;

	case EDDS_SRT_CONS_STATE_OFF_MISS:
		/* MISS -> Provider shall Autostop now
		 *
		 * EDDS_RTConsumerStop() does:
		 * pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_1;
		 *
		 */
		EDDS_RTConsumerStop(pCSRTComp, pConsFrame);
		Event = EDD_CSRT_CONS_EVENT_MISS;
		/* no break, fall through */
		//lint -fallthrough JB 18/11/2014
	case EDDS_SRT_CONS_STATE_OFF_1:
		EventStatus = EDD_CSRT_CONS_EVENT_MISS;
		break;

	case EDDS_SRT_CONS_STATE_ON_MNS:
		pConsFrame->State = EDDS_SRT_CONS_STATE_ON_2;
		Event = EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
		/* no break, fall through */
		//lint -fallthrough JB 18/11/2014
	case EDDS_SRT_CONS_STATE_OFF_0:
	case EDDS_SRT_CONS_STATE_ON_0: /* default value after activate */
	case EDDS_SRT_CONS_STATE_ON_1: /* default value after activate */
	case EDDS_SRT_CONS_STATE_ON_2:
		EventStatus = EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
		break;

	case EDDS_SRT_CONS_STATE_ON_AGAIN:
		pConsFrame->State = EDDS_SRT_CONS_STATE_ON_3;
		Event = EDD_CSRT_CONS_EVENT_AGAIN;
		/* no break, fall through */
		//lint -fallthrough JB 18/11/2014
	case EDDS_SRT_CONS_STATE_ON_3:
	case EDDS_SRT_CONS_STATE_ON_4:
		EventStatus = EDD_CSRT_CONS_EVENT_AGAIN;
		break;

		/** Set EventStatus, these states are processed in
		 * - EDDS_CSRTConsumerControlLow
		 * - EDDS_CSRTConsumerActualizeState
		 * - EDDS_RTGetAPDUStatus
		 *
		 * --> EDDS_SRT_CONS_STATE_ON_0, EDDS_SRT_CONS_STATE_ON_1, EDDS_SRT_CONS_STATE_ON_2,
		 * --> EDDS_SRT_CONS_STATE_ON_3, EDDS_SRT_CONS_STATE_ON_4
		 * --> EDDS_SRT_CONS_STATE_OFF_0, EDDS_SRT_CONS_STATE_OFF_1
		 */

	default:
		EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,
				"EDDS_CSRTConsumerProcessState: Illegal state %d, ConsID: %d",
				pConsFrame->State, ConsumerID);
		EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
		EDDS_MODULE_ID,
		__LINE__);
		break;
	}

	/* check redundancy bit 0/1 change      */
	/* 0: STOP , 1: RUN           */
	if (Change & EDD_CSRT_DSTAT_BIT_REDUNDANCY) {
		if (Act & EDD_CSRT_DSTAT_BIT_REDUNDANCY)
			Event |= EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS;
		else
			Event |= EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS;
	}
	/* set EventState */
	if (Act & EDD_CSRT_DSTAT_BIT_REDUNDANCY)
		EventStatus |= EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS;
	else
		EventStatus |= EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS;

	/* check PRIMARY/BACKUP change */
	/* 0: BACKUP, 1: PRIMARY       */
	if (Change & EDD_CSRT_DSTAT_BIT_STATE) {
		if (Act & EDD_CSRT_DSTAT_BIT_STATE)
			Event |= EDD_CSRT_CONS_EVENT_PRIMARY;
		else
			Event |= EDD_CSRT_CONS_EVENT_BACKUP;
	}
	/* set EventState */
	if (Act & EDD_CSRT_DSTAT_BIT_STATE)
		EventStatus |= EDD_CSRT_CONS_EVENT_PRIMARY;
	else
		EventStatus |= EDD_CSRT_CONS_EVENT_BACKUP;

	/* check STOP/RUN change      */
	/* 0: STOP , 1: RUN           */
	if (Change & EDD_CSRT_DSTAT_BIT_STOP_RUN) {
		if (Act & EDD_CSRT_DSTAT_BIT_STOP_RUN)
			Event |= EDD_CSRT_CONS_EVENT_RUN;
		else
			Event |= EDD_CSRT_CONS_EVENT_STOP;
	}
	/* set EventState */
	if (Act & EDD_CSRT_DSTAT_BIT_STOP_RUN)
		EventStatus |= EDD_CSRT_CONS_EVENT_RUN;
	else
		EventStatus |= EDD_CSRT_CONS_EVENT_STOP;

	/* check StationFailure change */
	/* 0: Failure, 1: OK           */

	if (Change & EDD_CSRT_DSTAT_BIT_STATION_FAILURE) {
		if (Act & EDD_CSRT_DSTAT_BIT_STATION_FAILURE)
			Event |= EDD_CSRT_CONS_EVENT_STATION_OK;
		else
			Event |= EDD_CSRT_CONS_EVENT_STATION_FAILURE;
	}
	/* set EventState */
	if (Act & EDD_CSRT_DSTAT_BIT_STATION_FAILURE)
		EventStatus |= EDD_CSRT_CONS_EVENT_STATION_OK;
	else
		EventStatus |= EDD_CSRT_CONS_EVENT_STATION_FAILURE;

	if (0 != Event) {
		EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
				"EDDS_CSRTConsumerProcessState: Event (ConsumerID: %d, Event: %Xh)",
				ConsumerID, Event);

		/* fill indication into requestblock (function will indicate */
		/* it to user if the rqb is full!                            */

		if (!EDDS_CSRTIndicationFill(pCSRTComp,
				pCSRTComp->Cons.pConsTbl[ConsumerID].UserID, Event, EventStatus,
				pCSRTComp->Cons.pConsFrameTbl[ConsumerID].CycleCounter)) {

			EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,
					"EDDS_CSRTConsumerProcessState: No indication resources present!");

			/* no more indication resources present, skip further tests */

			return LSA_FALSE;
		} else {
			/* Indication was signaled to user. actualize state */

			pCSRTComp->Cons.pConsTbl[ConsumerID].IndStatusFlags =
					pCSRTComp->Cons.pConsTbl[ConsumerID].ActStatusFlags;

			return LSA_TRUE;
		}
	}

	return LSA_TRUE;
}

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTConsumerTimerCbf                   +*/
/*+  Input/Output          :    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB     +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to Internal RQB used for this timer event.        +*/
/*+               (this is a pointer to ConsTimerRQB in consumer mgmgent)   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Consumer Cycle-Timer Callbackfunction.                    +*/
/*+               Called after the Cycletimer elapsed.                      +*/
/*+                                                                         +*/
/*+               We do trigger the output MACRO                            +*/
/*+               EDDS_DO_CONSUMER_TIMER_REQUEST()                          +*/
/*+               if not already underway.                                  +*/
/*+                                                                         +*/
/*+               This timer is used to start consumer indication checks.   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIntRQB)
//JB 05/12/2014 cannot be a null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CSRTConsumerTimerCbf(
	EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB)
{

	EDDS_LOCAL_DDB_PTR_TYPE pDDB;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;

	pDDB = ((EDDS_UPPER_DEV_INTERNAL_PTR_TYPE) pIntRQB->pRQB->pParam)->hDDB;
	pCSRTComp = &pDDB->pSRT->CSRT;

	if ( pCSRTComp->Cons.ConsumerCycle.CycleInProgress )
	{
		/* we are not ready with previous checking.. */

		EDDS_CRT_TRACE_01(0,EDDS_CFG_CSRT_OVERLOAD_TRACE_LEVEL,
				"EDDS_CSRTConsumerTimerCbf: Overload occured! (pDDB: 0x%X)",
				pDDB);

		pCSRTComp->Stats.ConsCycleOverloadCnt++;
	}

	if ( ! pIntRQB->InUse )
	{
		pCSRTComp->Cons.ConsumerCycle.CycleInProgress = LSA_TRUE;

		/* Note that the RQB is already filled up in startup!     */
		pIntRQB->InUse = LSA_TRUE;
		EDDS_DO_CONSUMER_TIMER_REQUEST(pIntRQB->pRQB);
	}
	else
	{
		/* we are not able to handle the timer request since last */
		/* timer occurance. we drop the event an set a statistic  */
		/* counter                                                */

		pCSRTComp->Stats.ConsCycleCounterDropCnt++;
	}

}
//lint +esym(613,pIntRQB)

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTIndicationFill                     +*/
/*+  Input                 :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    LSA_UINT16                   Type           +*/
/*+                             LSA_UINT16                   ID             +*/
/*+                             LSA_UINT32                   UserID         +*/
/*+                             LSA_UINT32                   Event          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+  UserID     : UserID from add consumer/provider                         +*/
/*+  Event      : Indication Event                                          +*/
/*+                                                                         +*/
/*+  Result     : LSA_TRUE: Event was filled in Indication resource         +*/
/*+               LSA_FALSE:No indication resource present                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Fills event in indication resource. if resource is full   +*/
/*+               it will  be indicated to user.                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_CSRTIndicationFill(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		LSA_UINT32 UserID,
		LSA_UINT32 Event,
		LSA_UINT32 EventStatus,
		LSA_UINT16 CycleCounter) {

	EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE pInd;
	EDDS_LOCAL_HDB_PTR_TYPE pHDB;
	LSA_BOOL Status;
	EDD_UPPER_RQB_PTR_TYPE pRQB;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_CSRTIndicationFill(pCSRTComp: 0x%X, UserID: 0x%X, Event: 0x%X)",
			pCSRTComp, UserID, Event);

	pRQB = pCSRTComp->Indication.RqbQueue.pBottom;

	if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) {
		Status = LSA_TRUE;

		pInd = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE) pRQB->pParam;
		pInd->Data[pInd->Count].Event = Event;
		pInd->Data[pInd->Count].EventStatus = EventStatus;
		pInd->Data[pInd->Count].UserID = UserID;
		pInd->Data[pInd->Count].CycleCnt = CycleCounter;
		pInd->Data[pInd->Count].DebugInfo[0] = 0x00;
		pInd->Data[pInd->Count].DebugInfo[1] = 0x00;
		pInd->Data[pInd->Count].DebugInfo[2] = (CycleCounter & 0xFF00) >> 8;
		pInd->Data[pInd->Count].DebugInfo[3] = (CycleCounter & 0x00FF)/*>>0*/;

		EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
				"EDDS_CSRTIndicationFill: Event: 0x%X, UserID: 0x%X", Event,
				UserID);

		if (++pInd->Count == EDD_CFG_CSRT_MAX_INDICATIONS) {

			EDDS_RQB_REM_BLOCK_BOTTOM(pCSRTComp->Indication.RqbQueue.pBottom,
					pCSRTComp->Indication.RqbQueue.pTop, pRQB);

			if (EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE(pRQB), &pHDB) != EDD_STS_OK) {
				EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
				EDDS_MODULE_ID,
				__LINE__);
			} else
				EDDS_RequestFinish(pHDB, pRQB, EDD_STS_OK);
		}
	} else {
		EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,
				"EDDS_CSRTIndicationFill: No SRT indication resource present");

		Status = LSA_FALSE;
	}

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_CSRTIndicationFill(), Status: 0x%X", Status);
	return (Status);

}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTIndicationFinish                   +*/
/*+  Input                 :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+  Result     : LSA_TRUE:   Indicationblock had entrys and was indicated  +*/
/*+               LSA_FALSE:  No indications in block                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if an indications block (resource) contains entrys +*/
/*+               and indicate it to user if so.                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CSRTIndicationFinish(
	EDDS_DDB_COMP_CSRT_TYPE_PTR             pCSRTComp)
{
	EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE     pInd;
	EDDS_LOCAL_HDB_PTR_TYPE                 pHDB;
	LSA_BOOL                                Status;
	EDD_UPPER_RQB_PTR_TYPE                  pRQB;

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_CSRTIndicationFinish(pCSRTComp: 0x%X)", pCSRTComp);

	pRQB = pCSRTComp->Indication.RqbQueue.pBottom;

	Status = LSA_FALSE;

	if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) {
		pInd = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

		if (pInd->Count) /* indications in block ?  */
		{
			Status = LSA_TRUE;

			EDDS_RQB_REM_BLOCK_BOTTOM(pCSRTComp->Indication.RqbQueue.pBottom,
					pCSRTComp->Indication.RqbQueue.pTop, pRQB);

			if (EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE(pRQB), &pHDB) != EDD_STS_OK) {
				EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
				EDDS_MODULE_ID,
				__LINE__);
			} else
				EDDS_RequestFinish(pHDB, pRQB, EDD_STS_OK);
		}
	}

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_CSRTIndicationFinish(), Status: 0x%X", Status);

}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTConsumerCycleRequest               +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Consumer Cycle Timer request.                             +*/
/*+               This function is called after a consumer-timer-request    +*/
/*+               occured (triggered by EDDS_DO_CONSUMER_TIMER_REQUEST)     +*/
/*+                                                                         +*/
/*+               This request will start a new consumer check cycle.       +*/
/*+                                                                         +*/
/*+               We caclulate all knots for this cycle to check and look   +*/
/*+               if all frames arrived. if not we signal an indication if  +*/
/*+               indication resources present.                             +*/
/*+                                                                         +*/
/*+      Note:    there is one cycle "jitter" with the test. so it is       +*/
/*+               possible, that a consumer is added and checked at once    +*/
/*+               which result in an errorneous situation, because the      +*/
/*+               cycle wasnt really done. So the first Cycle of a consumer +*/
/*+               will be ignored. This results in an first check cycle of  +*/
/*+               CheckCycle <= Cycle <= CheckCycle*2 for the consumer.     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CSRTConsumerCycleRequest(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB) {

	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	LSA_UINT32 Interval, Mask;
	LSA_UINT32 KnotIndex;
	LSA_UINT16 ConsIdx;
	LSA_BOOL IndResourcePresent;
	LSA_BOOL AllDone;
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
	LSA_BOOL Run;
#endif

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_CSRTConsumerCycleRequest(pDDB: 0x%X)", pDDB);

	pCSRTComp = &pDDB->pSRT->CSRT;

	AllDone = LSA_FALSE;
	IndResourcePresent = LSA_TRUE;

	/*----------------------------------------------------------------------------*/
	/* Optional check for jitter detection. (if the time since last check is      */
	/* much shorter then the ConsumerCheckCycle). This can occur if we were       */
	/* delayed for too long. As result we may do two successive checks within     */
	/* a very short time which may result in detection of errorneous MISS         */
	/* detection.                                                                 */
	/* So the Systemadaption can verify if enough time elapsed since last test    */
	/* and if not we skip this test.                                              */
	/*----------------------------------------------------------------------------*/

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK

	/* Systemadaption can check if the time since last call is long enough        */
	/* (for example: if we have a 3ms cycle it can check if at least 2ms elapsed) */

	Run = LSA_FALSE;

	EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT(
			pCSRTComp->ConsCycleTimer.TimerID,
			pCSRTComp->ConsCycleTimer.BaseTimeNS,
			&pCSRTComp->Cons.ConsumerCycle.TimerValue,
			&Run);

	if ( ! Run )
	{

		EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,
				"EDDS_CSRTConsumerCycleRequest: Jitter drop (current cycle: 0x%X)",
				pCSRTComp->Cons.ConsumerCycle.CycleCounter);

		pCSRTComp->Stats.ConsCycleJitterDrop++;
		pCSRTComp->ConsTimerRQB.InUse = LSA_FALSE; /* RQB is free for further use now*/
	}
	else
#endif
	{

		pCSRTComp->Cons.ConsumerCycle.CycleCounter++;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		pCSRTComp->ConsTimerRQB.InUse = LSA_FALSE; /* RQB is free for further use now*/
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

		EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"EDDS_CSRTConsumerCycleRequest: Starting consumer cycle (0x%X)",
				pCSRTComp->Cons.ConsumerCycle.CycleCounter);

		Interval = pCSRTComp->Cons.ConsumerCycle.MinInterval;
		Mask = Interval - 1;
		KnotIndex = (Mask)
				+ ((pCSRTComp->Cons.ConsumerCycle.CycleCounter) & Mask);

		/*----------------------------------------------------------------------------*/
		/* get ConsIdx of first Consumer. Check if KnotElement present (may not)      */
		/* If not we can advance to next Knot. (MinInterval not updated yet)          */
		/*----------------------------------------------------------------------------*/

		if (pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx
				== EDDS_SRT_IDX_NOT_USED) {
			ConsIdx = EDDS_SRT_CHAIN_IDX_END;
		} else {
			ConsIdx =
					pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ConsumerCheckList.BottomIdx;
		}

		/*----------------------------------------------------------------------------*/
		/* loop until all consumers for this cycle checked.                           */
		/* one consumer can indicate more then one status!                            */
		/*----------------------------------------------------------------------------*/

		while (!AllDone) {
			/* check if we have to switch to next knot */

			if (ConsIdx == EDDS_SRT_CHAIN_IDX_END) {
				/* check if we have done all knots, if  not switch to next one   */

				if (Interval < pCSRTComp->Cons.ConsumerCycle.MaxInterval) {
					Interval = Interval * 2;
					Mask = Interval - 1;
					KnotIndex = Mask
							+ ((pCSRTComp->Cons.ConsumerCycle.CycleCounter)
									& Mask);

					EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
							"EDDS_CSRTConsumerCycleRequest: Switch to KnotIDX: %d, Int: %d, CyCnt: %d",
							KnotIndex, Interval,
							pCSRTComp->Cons.ConsumerCycle.CycleCounter);

					/* if knot not used set to SRT_CHAIN_IDX_END */
					if (pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx
							== EDDS_SRT_IDX_NOT_USED) {
						ConsIdx = EDDS_SRT_CHAIN_IDX_END;
					} else {
						ConsIdx =
								pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ConsumerCheckList.BottomIdx;
					}

				} else
					AllDone = LSA_TRUE;
			} else /* valid index */
			{
				/* only check active consumers which watchdogcounter reached 0  */

				if ((pCSRTComp->Cons.pConsFrameTbl[ConsIdx].State
						>= EDDS_SRT_CONS_STATE_ON_0)
						&& ((--pCSRTComp->Cons.pConsTbl[ConsIdx].WatchdogCnt)
								== 0)) {

					/* reload counter */

					pCSRTComp->Cons.pConsTbl[ConsIdx].WatchdogCnt =
							pCSRTComp->Cons.pConsTbl[ConsIdx].WatchdogFactor;

					/* actualize the Status. Checks for score and DataStatus */

					EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
							"EDDS_CSRTConsumerCycleRequest: ConsumerActualizeState (ConsIdx: %d)",
							ConsIdx);

					EDDS_CSRTConsumerActualizeState(pCSRTComp, ConsIdx);

					/* check for Statuschanges and indicate them if present */

					if (IndResourcePresent) {
						IndResourcePresent = EDDS_CSRTConsumerProcessState(
								pCSRTComp, ConsIdx);
					}

				}

				/* next consumer */
				ConsIdx = pCSRTComp->Cons.pConsTbl[ConsIdx].CheckLink.Next;
			}
		} /* while */

		EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"EDDS_CSRTConsumerCycleRequest: Consumer cycle finished");

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK_END
		EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END(pCSRTComp->ConsCycleTimer.TimerID,
				&pCSRTComp->Cons.ConsumerCycle.TimerValue);
#endif
#endif

	} /* else */

	/* --------------------------------------------------------------------------*/
	/* Finish the indication request handling. This will indicate the rqb if     */
	/* something is filled.                                                      */
	/* --------------------------------------------------------------------------*/

	EDDS_CSRTIndicationFinish(pCSRTComp);

	/* if we havent enough resources we set a flag that indications pending ..   */

	if (!IndResourcePresent)
		pCSRTComp->Indication.ConsIndicationPending = LSA_TRUE;

	pCSRTComp->Cons.ConsumerCycle.CycleInProgress = LSA_FALSE;

	EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_CSRTConsumerCycleRequest()");

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerCheckIndication              +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks all consumers for pending indications.             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerCheckIndication(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB) {

	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	LSA_UINT16 ConsIdx;
	LSA_BOOL IndResourcePresent;

	EDDS_CRT_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTConsumerCheckIndication(pDDB: 0x%X)", pDDB);

	pCSRTComp = &pDDB->pSRT->CSRT;

	IndResourcePresent = LSA_TRUE;

	if (pCSRTComp->Indication.ConsIndicationPending) {
		LSA_UINT32 count = pCSRTComp->Cons.ConsumerCnt;
		pCSRTComp->Indication.ConsIndicationPending = LSA_FALSE;

		/*------------------------------------------------------------------------*/
		/* loop until all used consumers which are active are checked or we have  */
		/* no more indication resources..                                         */
		/*------------------------------------------------------------------------*/

		ConsIdx = pCSRTComp->Cons.ConsumerUsed.BottomIdx;

		while ((ConsIdx != EDDS_SRT_CHAIN_IDX_END) && (IndResourcePresent)) {
			EDDS_CRT_TRACE_02(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
					"    EDDS_RTConsumerCheckIndication(ConsIdx=%d,State=%d)",
					ConsIdx, pCSRTComp->Cons.pConsFrameTbl[ConsIdx].State);

			if (pCSRTComp->Cons.pConsFrameTbl[ConsIdx].State
					!= EDDS_SRT_CONS_STATE_INIT) {
				/* check if we have events to be signaled to user */
				/*  and send them ! */

				IndResourcePresent = EDDS_CSRTConsumerProcessState(pCSRTComp,
						ConsIdx);
				EDDS_CRT_TRACE_02(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
						"    EDDS_CSRTConsumerProcessState(ConsIdx=%d): new State=%d",
						ConsIdx, pCSRTComp->Cons.pConsFrameTbl[ConsIdx].State);
			}

			/* next consumer is actual? --> throw a fatal error! */
			/* more consumer in list than added? --> throw a fatal error! */
			if ((0 == count--)
					|| (ConsIdx
							== pCSRTComp->Cons.pConsTbl[ConsIdx].UsedFreeLink.Next)) {
				EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
				EDDS_MODULE_ID,
				__LINE__);
			}
			/* next consumer */
			ConsIdx = pCSRTComp->Cons.pConsTbl[ConsIdx].UsedFreeLink.Next;

		} /* while */

		/* ----------------------------------------------------------------------*/
		/* Finish the indication request handling. This will indicate the rqb if */
		/* something is filled (neccessary if the rqb is not full)               */
		/* ----------------------------------------------------------------------*/

		EDDS_CSRTIndicationFinish(pCSRTComp);

	}

	/* if we havent enough resources we set a flag that indications pending ..   */

	if (!IndResourcePresent)
		pCSRTComp->Indication.ConsIndicationPending = LSA_TRUE;

	EDDS_CRT_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTConsumerCheckIndication(Pending?: %d)",
			pDDB->pSRT->CSRT.Indication.ConsIndicationPending);

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerCheckIndRequest(pDDB)        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Consumer Indication Check request                         +*/
/*+               This function is called after a EDDS_SRV_DEV_TRIGGER      +*/
/*+               request with EDDS_TRIGGER_CONS_INDICATION occurred.       +*/
/*+               This function looks for all pending consumer indications  +*/
/*+               and try to indicate them to the user.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked before function call
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerCheckIndRequest(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB) {

	pDDB->pSRT->CSRT.ConsIndTriggerRQB.InUse = LSA_FALSE; /* RQB is free for further use now*/

	EDDS_RTConsumerCheckIndication(pDDB);

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerSetMinMaxRatioUsed           +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to start CSRT-management                 +*/
/*+                                                                         +*/
/*+  Result     : void                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs KnotTable for the CycleReductionRatio for the     +*/
/*+               maximum ConsumerCnt and sets the value within             +*/
/*+               the consumer cycle control                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerSetMinMaxRatioUsed(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp) {

	LSA_UINT32 Max;
	LSA_UINT32 Min;
	LSA_UINT32 i;

	Min = EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX - 1;
	Max = EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN - 1; //lint !e778 JB 18/11/2014 define-based behaviour

	for (i = EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN - 1; //lint !e778 JB 18/11/2014 define-based behaviour
			i < EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX; i++) 
			{
		/* something present ? */
		if (pCSRTComp->Cons.ConsRatioInfo[i].Cnt) {
			if (i < Min)
				Min = i;
			if (i > Max)
				Max = i;
		}
	}

	if (Min > Max) /* the case if nothing present at all. set Min = Max */
	{
		Min = Max;
	}

	/* Note: We may be interrupted by cyclic check routine here. So it may   */
	/*       be possible that Max is updated, but Min not. This dont realy   */
	/*       care, because its only the border till we check for consumers   */
	/*       and it dont matters if we check more or less. So we dont        */
	/*       protect this sequence.                                          */

	pCSRTComp->Cons.ConsumerCycle.MaxInterval = (LSA_UINT16) EDDSGet2Potenz(
			Max);
	pCSRTComp->Cons.ConsumerCycle.MinInterval = (LSA_UINT16) EDDSGet2Potenz(
			Min);

}
//lint +esym(613,pCSRTComp)

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 previously taken from DDB
//WARNING: be careful when calling this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerInsert(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		LSA_UINT16 Idx) {
	LSA_RESULT Status;
	EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
	EDDS_CSRT_KNOT_TYPE_PTR pKnot;
	EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
	LSA_UINT32 KnotTableIdx;
	LSA_UINT32 Interval; /* == reduction ratio */
	LSA_UINT16 ReductionFactor; /* 1..x */
	LSA_UINT16 n;

	pCons = &pCSRTComp->Cons.pConsTbl[Idx];
	/* interval is the reduction Ratio */

	ReductionFactor = pCons->CycleReductionNr + 1;
	Interval = pCons->CycleReductionRatio;

	if ((EDD_CYCLE_PHASE_UNDEFINED == ReductionFactor)) {
		ReductionFactor = EDDS_RTGetAndCheckReductionFactor(
				pCons->CycleReductionRatio,
				(LSA_BOOL) (pCons->Type == EDDS_CSRT_PROVIDER_TYPE_UDP));
		pCons->CycleReductionNr = ReductionFactor - 1;
	}

	pCSRTComp->Cons.ConsRatioInfo[pCons->CycleReductionNr].Cnt++;
	EDDS_RTConsumerSetMinMaxRatioUsed(pCSRTComp);

	/* check for valid CyclePhase and Frame not already in use */

	if ((pCons->CyclePhase > Interval)) {

		EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,
				"EDDS_RTConsumerAdd:Invalid CylcePhase (ID: %d, Phase: %d)",
				Idx, pCons->CyclePhase);

		Status = EDD_STS_ERR_PARAM;
	} else {
		/* calculate Knot-Table index          */
		/* (Interval-1) + (CyclePhase-1)       */
		/* Get Knot with filled in KnotElement */
		Status = EDDS_RTKnotGet(pCSRTComp, Interval + pCons->CyclePhase - 2);
	}

	if (EDD_STS_OK == Status) {
		/*------------------------------------------------------------- */
		/* Put consumer to checklist in knot management (at end of list)*/
		/* sequence in list currently not supported!                    */
		/*------------------------------------------------------------- */

		/* calculate Knot-Table index    */
		/* (Interval-1) + (CyclePhase-1) */
		/* example for Interval = 4 and CyclePhase =2 : Idx = 4 */

		KnotTableIdx = Interval + pCons->CyclePhase - 2;

		pKnot = &pCSRTComp->Knot.pKnotTbl[KnotTableIdx];
		pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

		pCons->KnotTableIdx = KnotTableIdx;
		pCons->CheckLink.Next = EDDS_SRT_CHAIN_IDX_END;
		pCons->CheckLink.Prev = pKnotElement->ConsumerCheckList.TopIdx;

		n = pKnotElement->ConsumerCheckList.TopIdx; /* current top idx */
		pKnotElement->ConsumerCheckList.TopIdx = Idx; /* new top idx     */

		/* Note: No protection against MIDDLE context needed because      */
		/*       MIDDLE context only accesses this parameter for chain    */
		/*       handling.                                                */

		if (n == EDDS_SRT_CHAIN_IDX_END) /* it was nothing in chain. */
		{
			pKnotElement->ConsumerCheckList.BottomIdx = Idx;
		} else {
			pCSRTComp->Cons.pConsTbl[n].CheckLink.Next = Idx;
		}

		pKnotElement->ConsumerCnt++;
	}
	return Status;
}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerAdd                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_CONSUMER_ADD_TYPE                    +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_CONSUMER_ADD_TYPE                                         +*/
/*+                                                                         +*/
/*+  FrameID                 : FrameID for Provider.                        +*/
/*+  SrcMAC                  : Source MAC address (only XRT)                +*/
/*+  SrcIP                   : Source IP (only xRTOverUDP)                  +*/
/*+  DataLen                 : DataLen of I/O-Data in frame.                +*/
/*+  CycleReductionRatio     : spezifies CycleTime based on CycleBaseFactor +*/
/*+                            (1,2,4,8,16,32,64,128..)                     +*/
/*+  CyclePhase              : spezifies Phase within cycle.                +*/
/*+  WatchdogFactor          : spezifies watchog time                       +*/
/*+  DataHoldFactor          : spezifies datahold time                      +*/
/*+  UserID                  : UserID for Event-indications.                +*/
/*+  ConsumerID              : returned ConsumerID.                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_NO_LOCAL_IP                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to add a consumer (XRT and XRTOverUDP)            +*/
/*+                                                                         +*/
/*+               First we check if free consumer is present. if not we     +*/
/*+               return EDD_STS_ERR_RESOURCE. Then we check for several    +*/
/*+               valid parameters.                                         +*/
/*+                                                                         +*/
/*+               Next we queue the consumer within "used-chain" and put    +*/
/*+               the consumer to the desired Knot-Check-Chain. (depends    +*/
/*+               on CycleReductionRation and CyclePhase).                  +*/
/*+                                                                         +*/
/*+               We do not preinitialize the receive-frames.               +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerAdd(EDD_UPPER_RQB_PTR_TYPE pRQB,
		EDDS_LOCAL_HDB_PTR_TYPE pHDB) {

	LSA_UINT16 Idx;
	LSA_RESULT Status;
	EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE pRqbParam;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
	EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
	LSA_UINT16 Prev;
	LSA_UINT16 Next;
	LSA_UINT16 ReductionFactor = 0; /* 1..x */
	LSA_UINT16 RtClassProp;
	LSA_BOOL Udp;
#if 0
	LSA_UINT32 MaxTimeMS;
#endif
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;

	EDDS_CRT_TRACE_03(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTConsumerAdd(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)", pRQB,
			pDDB, pHDB);

	Status = EDD_STS_OK;
	Udp = LSA_FALSE;
	Idx = 1;

	pRqbParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE) pRQB->pParam;

	pCSRTComp = &pDDB->pSRT->CSRT;
	RtClassProp = pRqbParam->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK;

	/* ---------------------------------------------------- */
	/* check Properties for RT classes                      */
	/* ---------------------------------------------------- */
	if (RtClassProp != EDD_CSRT_CONS_PROP_RTCLASS_1
			&& RtClassProp != EDD_CSRT_CONS_PROP_RTCLASS_2) {
		EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
				"EDDS_RTConsumerAdd: wrong RT class");
		Status = EDD_STS_ERR_PARAM;
	} /* check Properties for RT classes */

	/* ---------------------------------------------------- */
	/* check Properties flags                               */
	/* ---------------------------------------------------- */
	if (EDD_STS_OK == Status
			&& ((pRqbParam->Properties & EDD_CSRT_CONS_PROP_SYSRED)
					|| (pRqbParam->Properties
							& EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP)
					|| (pRqbParam->Properties
							& EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY))) {
		EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
				"EDDS_RTConsumerAdd: unsupported property flag");
		Status = EDD_STS_ERR_PARAM;
	} /* check Properties flags */

	/*---------------------------------------------------------------------------*/
	/* check for xRToverUdp or XRT Provider ADD.                                 */
	/*---------------------------------------------------------------------------*/
	if (EDD_STS_OK == Status) {
		if ((EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_SRT_CONSUMER_ADD )
				&& ( EDD_CSRT_CONS_PROP_RTCLASS_UDP == RtClassProp)) {
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
			if (( pRqbParam->DataLen < EDD_CSRT_UDP_DATALEN_MIN ) ||
					( pRqbParam->DataLen > EDD_CSRT_UDP_DATALEN_MAX ) ||
					( pRqbParam->FrameID == 0 ) ||
					( pRqbParam->FrameID < pCSRTComp->Cons.ConsumerFrameIDBase2) ||
					( pRqbParam->FrameID > pCSRTComp->Cons.ConsumerFrameIDTop2 )
			)
			{

				EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
						"EDDS_RTConsumerAdd:Invalid RQB parameter(s)");

				Status = EDD_STS_ERR_PARAM;
			}
			else
			{
				if (pDDB->pGlob->HWParams.IPAddress.dw == 0 ) /* no IP present ? */
				{
					EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
							"EDDS_RTConsumerAdd:Local IP not set yet!");

					Status = EDD_STS_ERR_NO_LOCAL_IP;
				}
			}

			Udp = LSA_TRUE;
			Idx = 2; /* UDP always within table 2 */
#else
			Status = EDD_STS_ERR_PARAM; /* should not occure because already checked outside */
#endif
		} else /* XRT */
		{
			if ((pRqbParam->DataLen < EDD_CSRT_DATALEN_MIN)
					|| (pRqbParam->DataLen > EDD_CSRT_DATALEN_MAX)
					|| (pRqbParam->FrameID == 0)) {
				EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,
						LSA_TRACE_LEVEL_ERROR,
						"EDDS_RTConsumerAdd: Invalid RQB parameter(s)");
				Status = EDD_STS_ERR_PARAM;
			} else {
				/* ---------------------------------------------------- */
				/* check FrameID                                        */
				/* --> FrameID must match RTClass                       */
				/* --> check FrameID area (RTClass boundary/configured  */
				/* ---------------------------------------------------- */
				switch (RtClassProp) {
				case EDD_CSRT_CONS_PROP_RTCLASS_1: {
					if ((EDD_SRT_FRAMEID_CSRT_START2 > pRqbParam->FrameID)
							|| (EDD_SRT_FRAMEID_CSRT_STOP2 < pRqbParam->FrameID)) {
						Status = EDD_STS_ERR_PARAM;
					} else {
						if ((pRqbParam->FrameID
								>= pCSRTComp->Cons.ConsumerFrameIDBase2)
								&& (pRqbParam->FrameID
										<= pCSRTComp->Cons.ConsumerFrameIDTop2)) {
							Idx = 2; /* table 2 */
						} else {
							EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,
									LSA_TRACE_LEVEL_ERROR,
									"EDDS_RTConsumerAdd: Invalid FrameID: 0x%X)",
									pRqbParam->FrameID);
							Status = EDD_STS_ERR_PARAM;
						}
					}
				}
					break;

				case EDD_CSRT_CONS_PROP_RTCLASS_2: {
					if ((EDD_SRT_FRAMEID_CSRT_START1 > pRqbParam->FrameID)
							|| (EDD_SRT_FRAMEID_CSRT_STOP1 < pRqbParam->FrameID)) {
						Status = EDD_STS_ERR_PARAM;
					} else {
						if ((pRqbParam->FrameID
								>= pCSRTComp->Cons.ConsumerFrameIDBase1)
								&& (pRqbParam->FrameID
										<= pCSRTComp->Cons.ConsumerFrameIDTop1)) {
							Idx = 1; /* table 1 */
						} else {
							EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,
									LSA_TRACE_LEVEL_ERROR,
									"EDDS_RTConsumerAdd: Invalid FrameID: 0x%X)",
									pRqbParam->FrameID);
							Status = EDD_STS_ERR_PARAM;
						}
					}
				}
					break;

				default: {
					EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,
							LSA_TRACE_LEVEL_ERROR,
							"[H:--] EDDS_RTConsumerAdd(): invalid RTClassProperty (0x%X)",
							RtClassProp);
					Status = EDD_STS_ERR_PARAM;
				}
					break;
				} /* check FrameID */
			}

			Udp = LSA_FALSE;
		}
	}

	/*---------------------------------------------------------------------------*/
	/* get ReductionFactor(1..x) from ReductionRatio. 0 signals invalid Ratio    */
	/*---------------------------------------------------------------------------*/
	if ((EDD_STS_OK == Status)
			&& (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
					!= pRqbParam->CycleReductionRatio)) {
		ReductionFactor = EDDS_RTGetAndCheckReductionFactor(
				pRqbParam->CycleReductionRatio, Udp);

		if (0 == ReductionFactor) {
			EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTConsumerAdd: wrong CycleReductionRatio");
			Status = EDD_STS_ERR_PARAM;
		}
	} /* check CycleReductionRatio */

	/* ---------------------------------------------------- */
	/* check CyclePhase                                     */
	/* ---------------------------------------------------- */
	if ((EDD_STS_OK == Status)
			&& (EDD_CYCLE_PHASE_UNDEFINED != pRqbParam->CyclePhase)
			&& (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
					!= pRqbParam->CycleReductionRatio)) {
		if (pRqbParam->CyclePhase > pRqbParam->CycleReductionRatio) {
			EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTConsumerAdd: wrong CyclePhase");
			Status = EDD_STS_ERR_PARAM;
		}
	} /* check CyclePhase */

	/* ---------------------------------------------------- */
	/* check DataHoldFactor                                 */
	/* ---------------------------------------------------- */
	if ((EDD_STS_OK == Status)
			&& ((pRqbParam->DataHoldFactor
					< EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN)
					|| (pRqbParam->DataHoldFactor
							> EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MAX))) {
		EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
				"EDDS_RTConsumerAdd: wrong DataHoldFactor");
		Status = EDD_STS_ERR_PARAM;
	} /* check DataHoldFactor */

	if (EDD_STS_OK == Status) {
		/* calculate consumer table idx */
		if (Idx == 2) /* table 2 */
		{
			Idx = ( pRqbParam->FrameID - pCSRTComp->Cons.ConsumerFrameIDBase2 )
					+ pCSRTComp->Cfg.ConsumerCntClass2;
		} else /* table 1 */
		{
			Idx = pRqbParam->FrameID - pCSRTComp->Cons.ConsumerFrameIDBase1;
		}

		EDDS_ASSERT(Idx >= pCSRTComp->Cons.ConsumerCnt);

		pCons = &pCSRTComp->Cons.pConsTbl[Idx];
		pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[Idx];

		if (EDDS_CSRT_CONSUMER_TYPE_NOT_USED != pCons->Type) {
			EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTConsumerAdd: ConsumerID %d (FrameID 0x%x) already in use.",
					Idx, pRqbParam->FrameID);
			Status = EDD_STS_ERR_PARAM;
		}

		if (Status == EDD_STS_OK) {
			/*------------------------------------------------------------- */
			/* copy parameter to management structure                       */
			/*------------------------------------------------------------- */

			pConsFrame->CycleCounter = 0;
			pConsFrame->State = EDDS_SRT_CONS_STATE_INIT;

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
			if ( Udp )
			{
				EDD_MAC_ADR_TYPE _temp = { {0,0,0,0,0,0}};
				pCons->Type = EDDS_CSRT_CONSUMER_TYPE_UDP;

				/* FrameLength will be used as UDPLen for xRToverUDP */
				pConsFrame->FrameLength = pRqbParam->DataLen +
				sizeof(EDDS_FRAME_ID_TYPE) +
				EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;
				pConsFrame->StampAddr.udp.SrcIP = _temp;
			}
			else
#endif
			{
				EDD_MAC_ADR_TYPE _temp = { { 0, 0, 0, 0, 0, 0 } };
				pCons->Type = EDDS_CSRT_CONSUMER_TYPE_XRT;
				pConsFrame->FrameLength = pRqbParam->DataLen +
				EDDS_CSRT_FRAME_IO_DATA_PROLOG_SIZE +
				EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;
				pConsFrame->StampAddr.xrt.SrcMAC = _temp;
			}

			pConsFrame->Type = pCons->Type
					| (pRqbParam->IOParams.ClearOnMISS ?
							EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS : 0);
			pCons->KnotTableIdx = 0xFFFFFFFF; /* not enqueued yet. */

			pCons->CyclePhase = pRqbParam->CyclePhase;
			pCons->CycleReductionRatio = pRqbParam->CycleReductionRatio;
			pCons->CycleReductionNr = ReductionFactor - 1;

			/* convert to a multiple of EDD_SRT_CONSUMER_WATCHDOG_FACTOR_MIN               */
			/* we round up by one if not a muliple of EDD_SRT_CONSUMER_WATCHDOG_FACTOR_MIN */
			pCons->WatchdogFactor = pRqbParam->DataHoldFactor
					/ EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN;
			if (pRqbParam->DataHoldFactor % EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN)
				pCons->WatchdogFactor++;

			if ( EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
					== pRqbParam->CycleReductionRatio) {
				/* save WatchdogFactor for later calculation in EDDS_RTConsumerControl() */
				pCons->WatchdogCycleCnt = pRqbParam->DataHoldFactor;
			} else {
				/* Number of Provider Cycle Counts for Consumer check */
				pCons->WatchdogCycleCnt = pRqbParam->CycleReductionRatio
						* pRqbParam->DataHoldFactor;
			}

			/* convert to a multiple of EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN               */
			/* we round up by one if not a muliple of EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN */
			pCons->DataHoldFactor = pRqbParam->DataHoldFactor
					/ EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN;
			if (pRqbParam->DataHoldFactor % EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN)
				pCons->DataHoldFactor++;

			pCons->WatchdogCnt = pCons->WatchdogFactor;
			pCons->DataHoldCnt = pCons->DataHoldFactor;
			pCons->ProviderAutoStopCnt = 0;
			pCons->ProviderAutoStopEnabled = LSA_FALSE;
			pCons->DataLen = pRqbParam->DataLen;
			pCons->UserID = pRqbParam->UserID;

			pRqbParam->ConsumerID = Idx; /* Consumer ID is the index within */
			/* consumer table                  */

			pCSRTComp->Cons.ConsumerUsedCnt++;

			/*------------------------------------------------------------- */
			/* Remove Consumer from free chain list                         */
			/*------------------------------------------------------------- */

			Prev = pCons->UsedFreeLink.Prev;
			Next = pCons->UsedFreeLink.Next;

			if ((Prev == EDDS_SRT_CHAIN_IDX_END)
					|| (Next == EDDS_SRT_CHAIN_IDX_END))

					{
				if (Prev == EDDS_SRT_CHAIN_IDX_END) {
					if (Next == EDDS_SRT_CHAIN_IDX_END) {
						/* only one in list */

						pCSRTComp->Cons.ConsumerFree.BottomIdx =
								EDDS_SRT_CHAIN_IDX_END;
						pCSRTComp->Cons.ConsumerFree.TopIdx =
								EDDS_SRT_CHAIN_IDX_END;
					} else {
						/* first one in list, but not only one */
						pCSRTComp->Cons.ConsumerFree.BottomIdx = Next;
						pCSRTComp->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
					}
				} else {
					/* last one in list but not only one */
					pCSRTComp->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
					pCSRTComp->Cons.ConsumerFree.TopIdx = Prev;
				}
			} else {
				/* in middle of list */
				pCSRTComp->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
				pCSRTComp->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
			}

			/*------------------------------------------------------------- */
			/* Put Consumer to used-chain list (end)                        */
			/*------------------------------------------------------------- */

			/* nothing in queue ? */
			if (pCSRTComp->Cons.ConsumerUsed.TopIdx == EDDS_SRT_CHAIN_IDX_END) {
				pCSRTComp->Cons.ConsumerUsed.BottomIdx = Idx;
				pCSRTComp->Cons.ConsumerUsed.TopIdx = Idx;
				pCons->UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
				pCons->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
			} else {
				pCons->UsedFreeLink.Prev = pCSRTComp->Cons.ConsumerUsed.TopIdx;
				pCons->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
				pCSRTComp->Cons.pConsTbl[pCSRTComp->Cons.ConsumerUsed.TopIdx].UsedFreeLink.Next =
						Idx;
				pCSRTComp->Cons.ConsumerUsed.TopIdx = Idx;
			}

			if (!( EDD_CYCLE_PHASE_UNDEFINED == pCons->CyclePhase ||
			EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pCons->CycleReductionRatio)) {
				Status = EDDS_RTConsumerInsert(pCSRTComp, Idx);
			}

			EDDS_UPPER_TRACE_03(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
					"EDDS_RTConsumerAdd: CID: %d, Ratio: %d, Phase: %d", Idx,
					pCons->CycleReductionRatio, pCons->CyclePhase);

		} else {
			EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_UNEXP,
					"EDDS_RTConsumerAdd: EDDS_RTKnotGet failed. (0x%X)",
					Status);
		}
	}

	LSA_UNUSED_ARG(pHDB);

	EDDS_CRT_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTConsumerAdd(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SRTConsumerRemove                           +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ConsumerID      +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  ConsumerID   : ID of consumer to remove                                +*/
/*+  KnotTableIdx : Index in Knottable where consumer is queued in checklist+*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes a consumer from Knot-checklist and set it to      +*/
/*+               "not" InUse.                                              +*/
/*+                                                                         +*/
/*+               Ids and Idx must be valid ! not checked.                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTConsumerRemove(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
		LSA_UINT16 ConsumerID,
		LSA_UINT32 KnotTableIdx) {

	EDDS_CSRT_KNOT_TYPE_PTR pKnot;
	EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
	LSA_UINT16 Prev;
	LSA_UINT16 Next;

	if (0xFFFFFFFF != KnotTableIdx) {
		pKnot = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
		EDDS_ASSERT(pKnot->KnotElementIdx >= pCSRT->Knot.KnotElementCnt);
		pKnotElement = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

		/*---------------------------------------------------------------------------*/
		/* First we remove the consumer from the Knotmanagment checklist.            */
		/*---------------------------------------------------------------------------*/

		/* four  cases:                                                              */
		/* consumer is the first in list (but not the only one)                      */
		/* consumer is the last in list (but not the only one)                       */
		/* consumer is the only one in list                                          */
		/* consumer is inside linked list                                            */

		/* check if first or last or only in list */

		Prev = pCSRT->Cons.pConsTbl[ConsumerID].CheckLink.Prev;
		Next = pCSRT->Cons.pConsTbl[ConsumerID].CheckLink.Next;

		if ((Prev == EDDS_SRT_CHAIN_IDX_END)
				|| (Next == EDDS_SRT_CHAIN_IDX_END))

				{
			if (Prev == EDDS_SRT_CHAIN_IDX_END) {
				if (Next == EDDS_SRT_CHAIN_IDX_END) {
					/* only one in list */
					pKnotElement->ConsumerCheckList.BottomIdx =
							EDDS_SRT_CHAIN_IDX_END;
					pKnotElement->ConsumerCheckList.TopIdx =
							EDDS_SRT_CHAIN_IDX_END;

				} else {
					/* first one in list, but not only one */
					pKnotElement->ConsumerCheckList.BottomIdx = Next;
					pCSRT->Cons.pConsTbl[Next].CheckLink.Prev = Prev;
				}
			} else {
				/* last one in list but not only one */
				pCSRT->Cons.pConsTbl[Prev].CheckLink.Next = Next;
				pKnotElement->ConsumerCheckList.TopIdx = Prev;
			}
		} else {
			/* in middle of list */
			pCSRT->Cons.pConsTbl[Prev].CheckLink.Next = Next;
			pCSRT->Cons.pConsTbl[Next].CheckLink.Prev = Prev;
		}

		pKnotElement->ConsumerCnt--; /* Number of Consumers in KnotList */

		EDDS_RTKnotFree(pCSRT, KnotTableIdx); /* remove KnotElement if unused yet */
	}

	/*---------------------------------------------------------------------------*/
	/* Now we can remove  the consumer from the InUse-chain.                     */
	/*---------------------------------------------------------------------------*/

	Prev = pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev;
	Next = pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Next;

	if ((Prev == EDDS_SRT_CHAIN_IDX_END) || (Next == EDDS_SRT_CHAIN_IDX_END))

	{
		if (Prev == EDDS_SRT_CHAIN_IDX_END) {
			if (Next == EDDS_SRT_CHAIN_IDX_END) {
				/* only one in list */

				pCSRT->Cons.ConsumerUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
				pCSRT->Cons.ConsumerUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END;
			} else {
				/* first one in list, but not only one */
				pCSRT->Cons.ConsumerUsed.BottomIdx = Next;
				pCSRT->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
			}
		} else {
			/* last one in list but not only one */
			pCSRT->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
			pCSRT->Cons.ConsumerUsed.TopIdx = Prev;
		}
	} else {
		/* in middle of list */
		pCSRT->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
		pCSRT->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
	}

	/*---------------------------------------------------------------------------*/
	/* Put this consumer to the end of the free-chain..                          */
	/*---------------------------------------------------------------------------*/

	pCSRT->Cons.pConsTbl[ConsumerID].Type = EDDS_CSRT_CONSUMER_TYPE_NOT_USED; /* this consumer is no more in use */
	pCSRT->Cons.ConsumerUsedCnt--;

	if (pCSRT->Cons.ConsumerFree.TopIdx == EDDS_SRT_CHAIN_IDX_END) /* nothing in chain ? */
	{
		pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev =
				EDDS_SRT_CHAIN_IDX_END;
		pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev =
				EDDS_SRT_CHAIN_IDX_END;
		pCSRT->Cons.ConsumerFree.BottomIdx = ConsumerID;
		pCSRT->Cons.ConsumerFree.TopIdx = ConsumerID;
	} else {
		pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev =
				pCSRT->Cons.ConsumerFree.TopIdx;
		pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Next =
				EDDS_SRT_CHAIN_IDX_END;
		pCSRT->Cons.pConsTbl[pCSRT->Cons.ConsumerFree.TopIdx].UsedFreeLink.Next =
				ConsumerID;
		pCSRT->Cons.ConsumerFree.TopIdx = ConsumerID;
	}

}
//lint +esym(613,pCSRT)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerRemove                       +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE                 +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE                                      +*/
/*+                                                                         +*/
/*+  ConsumerID : Valid ConsumerID                                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to remove a  consumer.                            +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+               A consumer  has to be in use and no buffers               +*/
/*+               locked (EDD_STS_ERR_SEQUENCE if not)                      +*/
/*+                                                                         +*/
/*+               Consumers may be active on remove. will be deactivated.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRemove(
		EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB) {

	LSA_RESULT Status;
	EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE pRqbParam;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
	LSA_UINT16 CycleReductionNr;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTConsumerRemove(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
			pRQB, pDDB, pHDB);

	Status = EDD_STS_OK;
	pCons = LSA_NULL;

	pRqbParam = (EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE) pRQB->pParam;
	pCSRTComp = &pDDB->pSRT->CSRT;

	/* check for valid ID-range. ConsumerID is idx within table */

	if (pRqbParam->ConsumerID >= pCSRTComp->Cons.ConsumerCnt) {
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
				"EDDS_RTConsumerRemove:Invalid Consumer ID (0x%X)",
				pRqbParam->ConsumerID);

		Status = EDD_STS_ERR_PARAM;
	} else {
		pCons = &pCSRTComp->Cons.pConsTbl[pRqbParam->ConsumerID];

		/* Consumer must be in use */
		if ( EDDS_CSRT_CONSUMER_TYPE_NOT_USED == pCons->Type) {
			EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTConsumerRemove: Consumer not inUse! (0x%X)",
					pRqbParam->ConsumerID);

			Status = EDD_STS_ERR_PARAM;
		} else {
			/* Consumer can only be removed if it has no AutoStop Providers! */
			if (pCons->ProviderAutoStopCnt) {
				EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
						"EDDS_RTConsumerRemove: Consumer already has AutoStop Providers! can not be removed! (0x%X)",
						pRqbParam->ConsumerID);

				Status = EDD_STS_ERR_SEQUENCE;
			}

			if (Status == EDD_STS_OK) {

				EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,
						"EDDS_RTConsumerRemove: CID: %d",
						pRqbParam->ConsumerID);

				CycleReductionNr = pCons->CycleReductionNr;

				/* deactivate Consumer first */

				EDDS_RTConsumerStop(pCSRTComp,
						&pCSRTComp->Cons.pConsFrameTbl[pRqbParam->ConsumerID]);

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
				EDDS_RTConsumerCheckTimerSwitch(pCSRTComp);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

				SRTConsumerRemove(pCSRTComp, pRqbParam->ConsumerID,
						pCons->KnotTableIdx);

				if (0xFFFF != CycleReductionNr) {
					pCSRTComp->Cons.ConsRatioInfo[CycleReductionNr].Cnt--;
				}

				EDDS_RTConsumerSetMinMaxRatioUsed(pCSRTComp);
			}
		}
	}

	LSA_UNUSED_ARG(pHDB);

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTConsumerRemove(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pRQB,pHDB)

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerCheckTimerSwitch             +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR       pCSRT     +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if the Consumer Timer have to be Started or Stopped+*/
/*+               Shall be called after a Consumer is started or stopped    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerCheckTimerSwitch(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT)
{

	LSA_USER_ID_TYPE Dummy;
	LSA_UINT16 TimeMS;
	LSA_UINT16 RetVal;

	if (pCSRT->Cons.ConsumerActCnt)
	{
		/*-----------------------------------------------------------------------*/
		/* If a consumer was activated and the timer is not active we start it.  */
		/*-----------------------------------------------------------------------*/

		if ( ! pCSRT->ConsCycleTimer.InProgress )
		{
			Dummy.uvar32 = pCSRT->ConsCycleTimer.EDDSTimerID;

			/* note that we can only use a multiple of ms. this was checked */
			/* on init!! TimeMS have to be > 0!!                            */

			TimeMS = (LSA_UINT16) (pCSRT->ConsCycleTimer.BaseTimeNS / (LSA_UINT32)1000000UL); //L or LSA_UINT32 cast?

			if (! TimeMS ) TimeMS = 1; /* just for sure */

			pCSRT->Cons.ConsumerCycle.CycleCounter = 0; /* start with Counter 0 */

			pCSRT->ConsCycleTimer.InProgress = LSA_TRUE;

			EDDS_START_TIMER(&RetVal,
					pCSRT->ConsCycleTimer.TimerID,
					Dummy,
					TimeMS);

			if ( RetVal != LSA_RET_ERR_PARAM )
			{
				pCSRT->ConsCycleTimer.InProgress = LSA_TRUE;
			}
			else
			{
				EDDS_FatalError(EDDS_FATAL_ERR_TIMER_START,
						EDDS_MODULE_ID,
						__LINE__);
			}

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
			EDDS_CSRT_CONSUMER_CYCLE_START(pCSRT->ConsCycleTimer.TimerID,
					&pCSRT->Cons.ConsumerCycle.TimerValue);
#endif

		}
	}
	else
	{
		/*-----------------------------------------------------------------------*/
		/* If no consumer running anymore the timer is deactivated               */
		/*-----------------------------------------------------------------------*/

		if ( pCSRT->ConsCycleTimer.InProgress )
		{
			EDDS_STOP_TIMER(&RetVal,
					pCSRT->ConsCycleTimer.TimerID);

			pCSRT->ConsCycleTimer.InProgress = LSA_FALSE;
		}
	}
}
//lint +esym(613,pCSRT)

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerStop                         +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR       pCSRT     +*/
/*+                             EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame+*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  pConsFrame : a Consumer in use                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Stops a Consumer                                          +*/
/*+               Note: After calling this function                         +*/
/*                      EDDS_RTConsumerCheckTimerSwitch shall be called too +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT,pConsFrame)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerStop(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
		EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame) {
	switch (pConsFrame->State) {
	case EDDS_SRT_CONS_STATE_ON_0:
	case EDDS_SRT_CONS_STATE_ON_1:
	case EDDS_SRT_CONS_STATE_ON_2:
	case EDDS_SRT_CONS_STATE_ON_3:
	case EDDS_SRT_CONS_STATE_ON_4:
	case EDDS_SRT_CONS_STATE_ON_AGAIN:
	case EDDS_SRT_CONS_STATE_ON_MNS:
		pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_0;
		pCSRT->Cons.ConsumerActCnt--;
		pConsFrame->CycleCounter = 0;
		break;
	case EDDS_SRT_CONS_STATE_OFF_MISS:
		pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_1;
		break;
	case EDDS_SRT_CONS_STATE_OFF_MNS:
	case EDDS_SRT_CONS_STATE_OFF_0:
	case EDDS_SRT_CONS_STATE_OFF_1:
		/* do nothing */
		break;
	default:
		EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_FATAL,
				"EDDS_RTConsumerStop: Illegal state %d, pCSRT %p, pConsFrame %p",
				pConsFrame->State, pCSRT, pConsFrame);
		EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
		EDDS_MODULE_ID,
		__LINE__);
		break;
	}
}
//lint +esym(613,pCSRT,pConsFrame)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerControlLow                   +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                 ConsID           +*/
/*+                             LSA_UINT16                 Mode             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  ConsID     : Valid ConsumerID                                          +*/
/*+  Mode       : see service EDD_SRV_SRT_CONSUMER_CONTROL                  +*/
/*+               EDD_CONS_CONTROL_MODE_ACTIVATE                            +*/
/*+               EDD_CONS_CONTROL_MODE_PASSIVATE                           +*/
/*+               EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN                      +*/
/*+               EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE                +*/
/*+               EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE               +*/
/*+                                                                         +*/
/*+  Note: The default-state after activating a not active consumer with    +*/
/*+        SetToUnknownState = FALSE is the MISS-state.                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Control a consumer. Note that Mode must be consistent     +*/
/*+               on calling (e.g. Activate/passivate shall not be set both)+*/
/*+               The consumer must be inUse!                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerControlLow(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
		LSA_UINT16 ConsID,
		LSA_UINT16 Mode) {

	EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
	EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;

	/*---------------------------------------------------------------------------*/
	/* Now set consumer status..                                                 */
	/*---------------------------------------------------------------------------*/

	pCons = &pCSRT->Cons.pConsTbl[ConsID];
	pConsFrame = &pCSRT->Cons.pConsFrameTbl[ConsID];

	EDDS_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW,
			"EDDS_RTConsumerControlLow: ConsID: %d(state %d), Mode: 0x%X",
			ConsID, pConsFrame->State, Mode);

	EDDS_ENTER_MIDDLE();

	/* --------------------------------------------------------------------------*/
	/* disable Provider autostop first if set.                                   */
	/* --------------------------------------------------------------------------*/

	if (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE) {
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"EDDS_RTConsumerControlLow: ProvAutoStop Disable. ConsID: %d.",
				ConsID);

		pCons->ProviderAutoStopEnabled = LSA_FALSE;
	}

	/* --------------------------------------------------------------------------*/
	/* if we should stop do it                                                   */
	/* --------------------------------------------------------------------------*/

	if (Mode & EDD_CONS_CONTROL_MODE_PASSIVATE) {
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"EDDS_RTConsumerControlLow: Deactivate Consumer %d", ConsID);

		EDDS_RTConsumerStop(pCSRT, pConsFrame);
	}

	/* --------------------------------------------------------------------------*/
	/* if we should start do it and handle SetToUnknown                          */
	/* --------------------------------------------------------------------------*/

	if (Mode & EDD_CONS_CONTROL_MODE_ACTIVATE) {

		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"EDDS_RTConsumerControlLow: Activate Consumer %d.", ConsID);

		/* only if not already active */

		if (pConsFrame->State < EDDS_SRT_CONS_STATE_ON_0) {
			pCSRT->Cons.ConsumerActCnt++;

			/* Setup Check-Counter with 1, because we will drop */
			/* the first check and reload the counter with the  */
			/* real watchdog-count later.                       */

			pCons->WatchdogCnt = 1;
			pCons->SkipCycleStamp = pCSRT->Prov.ProviderCycle.CycleCounter;

			/* Set default status-flags for consumer.          */

			pCons->IndStatusFlags = EDDS_CSRT_CONS_STAT_DEFAULT;
			pCons->ActStatusFlags = EDDS_CSRT_CONS_STAT_DEFAULT;

#if 0 /* am2219 */
			if ( Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN )
			{

				EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,
						"EDDS_RTConsumerControlLow: SetToUnknown1. ConsID: %d.",
						ConsID);

				/*am2219*/
				pCons->SkipCheckFlag = EDDS_CSRT_CONS_SKIP_STATE2;
				pCons->SkipCycleStamp = pCSRT->Prov.ProviderCycle.CycleCounter;
				/*am2219*/
				pConsFrame->State = EDDS_SRT_CONS_STATE_ON_1;
			}
			else
			{
				pConsFrame->State = EDDS_SRT_CONS_STATE_ON_0;
				/*am2219*/
				pCons->SkipCheckFlag = EDDS_CSRT_CONS_SKIP_NONE;
				/*am2219*/
			}
#else
			pConsFrame->State = EDDS_SRT_CONS_STATE_ON_0;
#endif

		}
	}

	if (Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN) {
		switch (pConsFrame->State) {
		/* pCons->SkipCycleStamp  = pCSRT->Prov.ProviderCycle.CycleCounter; */
		case EDDS_SRT_CONS_STATE_OFF_0:
			pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_MNS;
			break;
		case EDDS_SRT_CONS_STATE_ON_0:
		case EDDS_SRT_CONS_STATE_ON_1:
			pConsFrame->State = EDDS_SRT_CONS_STATE_ON_1;
			break;
		case EDDS_SRT_CONS_STATE_ON_2:
			pConsFrame->State = EDDS_SRT_CONS_STATE_ON_MNS;
			break;
		case EDDS_SRT_CONS_STATE_ON_3:
		case EDDS_SRT_CONS_STATE_ON_4:
			pConsFrame->State = EDDS_SRT_CONS_STATE_ON_4;
			break;
		case EDDS_SRT_CONS_STATE_OFF_1:
			pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_MISS;
			break;
		case EDDS_SRT_CONS_STATE_OFF_MISS:
		case EDDS_SRT_CONS_STATE_OFF_MNS:
		case EDDS_SRT_CONS_STATE_ON_AGAIN:
		case EDDS_SRT_CONS_STATE_ON_MNS:
			/* do nothing, event will be sent anyway */
			break;
		default:
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,
					"EDDS_RTConsumerControlLow: SetToUnknown in illegal state %d, ConsID: %d",
					pConsFrame->State, ConsID);
			EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
			EDDS_MODULE_ID,
			__LINE__);
			break;
		}

		EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"EDDS_RTConsumerControlLow: ConsID: %d -->state %d", ConsID,
				pConsFrame->State);

		pCSRT->Indication.ConsIndicationPending = LSA_TRUE;

	}

	/* set Provider Autostop after activting consumer because Provider handling */
	/* can interrupt this and should not be enabled before consumer is active   */

	if (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE) {
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"EDDS_RTConsumerControlLow: ProvAutoStop Enable. ConsID: %d.",
				ConsID);

		pCons->ProviderAutoStopEnabled = LSA_TRUE;
	}

	/* ----------------------------------------------------------------------*/
	/* Trigger the consumer indication check                                 */
	/* if indication resources present.                                      */
	/* ----------------------------------------------------------------------*/
	if ((pCSRT->Indication.ConsIndicationPending)
			&& (!LSA_HOST_PTR_ARE_EQUAL(pCSRT->Indication.RqbQueue.pBottom,
					LSA_NULL))) {
		if (!pCSRT->ConsIndTriggerRQB.InUse) {
			pCSRT->ConsIndTriggerRQB.InUse = LSA_TRUE;
			EDDS_DO_CONSUMER_IND_REQUEST(pCSRT->ConsIndTriggerRQB.pRQB);
		}
	}

	EDDS_EXIT_MIDDLE();

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	/*-----------------------------------------------------------------------*/
	/* Now check if the checktimer has  to be stopped or started.            */
	/* If a consumer was activated and the timer is not active we start it.  */
	/*-----------------------------------------------------------------------*/

	EDDS_RTConsumerCheckTimerSwitch(pCSRT);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

}
//lint +esym(613,pCSRT)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerControl                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE                +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE                                     +*/
/*+                                                                         +*/
/*+  ConsumerID :        Valid ConsumerID                                   +*/
/*+  Activate   :        LSA_TRUE : activate provider                       +*/
/*+                      LSA_FALSE: deaktivate provider                     +*/
/*+  SetToUnknownState:  only used if Activate = LSA_TRUE                   +*/
/*+                      LSA_TRUE : set the actual consumer state to UNKNOWN+*/
/*+                                 for the MISS/AGAIN-indication.          +*/
/*+                                 So the next consumer-check-cycle will   +*/
/*+                                 cause an AGAIN or MISS indication,      +*/
/*+                                 depending on frame received or net after+*/
/*+                                 setting the state to UNKNOWN.           +*/
/*+                      LSA_FALSE: do nothing with the actual state        +*/
/*+                                                                         +*/
/*+                      Note: Does only effect the MISS/AGAIN indication!  +*/
/*+                                                                         +*/
/*+  Note: The default-state after activating a not active consumer with    +*/
/*+        SetToUnknownState = FALSE is the MISS-state.                     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to control a consumer.                            +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerControl(
		EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB) {

	LSA_RESULT Status;
	EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE pRqbParam;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTConsumerControl(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
			pRQB, pDDB, pHDB);

	Status = EDD_STS_OK;

	pRqbParam = (EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE) pRQB->pParam;
	pCSRT = &pDDB->pSRT->CSRT;

	if ((pRqbParam->ConsumerID >= pCSRT->Cons.ConsumerCnt)
			|| ( EDDS_CSRT_CONSUMER_TYPE_NOT_USED
					== pCSRT->Cons.pConsTbl[pRqbParam->ConsumerID].Type)) {
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
				"EDDS_RTConsumerControl: Invalid Consumer ID (0x%X)",
				pRqbParam->ConsumerID);

		Status = EDD_STS_ERR_PARAM;
	} else {
		EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
		EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
		pCons = &pCSRT->Cons.pConsTbl[pRqbParam->ConsumerID];
		pConsFrame = &pCSRT->Cons.pConsFrameTbl[pRqbParam->ConsumerID];

		/* ------------------------------------------------------------------*/
		/* Activate/Deactivate check                                         */
		/* ------------------------------------------------------------------*/
		if (((pRqbParam->Mode & EDD_CONS_CONTROL_MODE_ACTIVATE)
				&& (pRqbParam->Mode & EDD_CONS_CONTROL_MODE_PASSIVATE))
				|| ((pRqbParam->Mode & EDD_CONS_CONTROL_MODE_PASSIVATE)
						&& (pRqbParam->Mode
								& EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN))

				) {
			EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTConsumerControl: Invalid Activate/Deactivate Mode: (0x%X))!",
					pRqbParam->Mode);

			Status = EDD_STS_ERR_PARAM;
		} else if (pRqbParam->Mode & EDD_CONS_CONTROL_MODE_ACTIVATE) {
			LSA_UINT16 CrNumber;
			LSA_BOOL doInsert = LSA_FALSE;
			EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr = LSA_NULL;

			CrNumber = pRqbParam->IOParams.CRNumber;

			if ( EDD_CRNUMBER_NOT_USED == CrNumber
					|| pCSRT->Cons.ConsumerCnt < CrNumber) {
				EDDS_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,
						"EDDS_RTConsumerControl: Invalid cr number!");

				Status = EDD_STS_ERR_PARAM;
			} else {
				LSA_UINT16 DataLen;

				pCr = &pCSRT->Cons.pConsCrTbl[CrNumber - 1]; // 1 .. ConsumerCnt

				DataLen = LE_TO_H_S(pCr->DataLen);  // ensure correct endianess!

				if (DataLen != pCons->DataLen || // data length of io data must match
						pCr->Type != pCons->Type || // matching consumer type --> from allocate at IOBuffer
						!pCr->CrUsed)         // Cr must be allocated previously
						{
					EDDS_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,
							"EDDS_RTConsumerControl: Invalid input parameter!");

					Status = EDD_STS_ERR_PARAM;
				}
			}

			if (Status == EDD_STS_OK) {
				// enter CrNumber
				pCons->CrNumber = CrNumber;

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
				if( (pCons->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_UDP )
				{
					pConsFrame->StampAddr.udp.SrcIP = pRqbParam->SrcIP;
				} else
#endif
				{
					pConsFrame->StampAddr.xrt.SrcMAC = pRqbParam->SrcMAC;
				}

				/* check CycleReductionRatio */
				if (/*(EDD_STS_OK == Status) 
						&&*/ (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
								== pCons->CycleReductionRatio)) { 
					LSA_BOOL IsUdp = (pCons->Type & EDDS_CSRT_CONSUMER_TYPE_XRT)
							!= EDDS_CSRT_CONSUMER_TYPE_XRT;

					/* CycleReductionRatio still undefined on activation */
					if (0
							== EDDS_RTGetAndCheckReductionFactor(
									pRqbParam->CycleReductionRatio, IsUdp)) {
						Status = EDD_STS_ERR_PARAM;
					} else {
						pCons->CycleReductionRatio =
								pRqbParam->CycleReductionRatio;

						/* Number of Provider Cycle Counts for Consumer check */

						/*@note if CycleReductionRatio was EDD_CYCLE_REDUCTION_RATIO_UNDEFINED on
						 * EDDS_RTConsumerAdd, pCons->WatchdogCycleCnt holds temporary the
						 * pRqbParam->WatchdogFactor
						 */
						pCons->WatchdogCycleCnt *=
								pRqbParam->CycleReductionRatio;
						doInsert = LSA_TRUE;
					}
				}

				/* check CyclePhase */
				if ((EDD_STS_OK == Status)
						&& (EDD_CYCLE_PHASE_UNDEFINED == pCons->CyclePhase)) {
					LSA_UINT16 CyclePhase;
					CyclePhase = pRqbParam->CyclePhase;

					/* CyclePhase still undefined on activation or out of boundary*/
					if ((EDD_CYCLE_PHASE_UNDEFINED == CyclePhase)
							|| (CyclePhase > pCons->CycleReductionRatio)) {
						Status = EDD_STS_ERR_PARAM;
					} else {
						pCons->CyclePhase = CyclePhase;
						doInsert = LSA_TRUE;
					}
				}

				if ((EDD_STS_OK == Status) && doInsert) {
					Status = EDDS_RTConsumerInsert(pCSRT,
							pRqbParam->ConsumerID);
				}
			}
		}

		/* ------------------------------------------------------------------*/
		/* Provider AUTOSTOP                                                 */
		/* ------------------------------------------------------------------*/
		if ((pRqbParam->Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE)
				&& (pRqbParam->Mode
						& EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE)) {
			EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTConsumerControl: Invalid Provider Autostop Mode: (0x%X))!",
					pRqbParam->Mode);

			Status = EDD_STS_ERR_PARAM;
		}

		if (Status == EDD_STS_OK) {
			EDDS_RTConsumerControlLow(pCSRT, pRqbParam->ConsumerID,
					pRqbParam->Mode);
		}
	}

	LSA_UNUSED_ARG(pHDB);

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTConsumerControl(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pRQB,pHDB)

#endif

/*****************************************************************************/
/*  end of file SRT_CONS.C                                                   */
/*****************************************************************************/

