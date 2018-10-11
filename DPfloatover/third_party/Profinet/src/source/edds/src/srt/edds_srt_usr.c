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
/*  F i l e               &F: edds_srt_usr.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDS.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  04.06.02    JS    initial version.                                       */
/*  19.08.03    JS    added support for provider phase masks                 */
/*  23.09.03    JS    ProviderGroupCnt removed from SRT-Init-structure       */
/*                    ProviderGroupCnt now a define.                         */
/*  02.10.03    JS    Added clearing tx-frames with 0 if we should pad with 0*/
/*  27.11.03    JS    Added ProviderCyclePhaseMaxByteCntHard                 */
/*                    Added ProviderCyclePhaseMaxCntHard                     */
/*                    Added EDD_SRV_SRT_PROVIDER_CHANGE_PHASE                */
/*  05.12.03    JS    Added Support of CyclCounter for Consumer.             */
/*  21.01.04    JS    Changed CycleBaseFactor and ReductionRatio interpretion*/
/*  05.04.04    JS    changes to prevent compiler warnings                   */
/*                    Using LSA_UNUSED_ARG                                   */
/*  15.06.04    JS    removed ASRT functions                                 */
/*  27.07.04    JS    EDDS_ENTER_MIDDLE and EDDS_EXIT_MIDDLE added to support */
/*                    middle-prio with consumer check                        */
/*  16.08.04    JS    Added support for xRToverUDP                           */
/*  14.09.04    JS    Added MaxPortCnt                                       */
/*  15.11.04    JS    added InterfaceID                                      */
/*  02.12.04    JS    added parameters to EDD_SRV_SRT_GET_PARAMS             */
/*  07.12.04    JS    added EDDS_SRV_SRT_PROVIDER_XCHANGE_BUFF               */
/*                    added EDDS_SRV_SRT_CONSUMER_XCHANGE_BUFF               */
/*  14.01.05    JS    we now have two consumer tables. one for XRT Class 2   */
/*                    and one for XRT and XRToverUPD (Class1/4)              */
/*  18.01.05    JS    added HardwareType to GET_PARAMS services.             */
/*  31.01.05    JS    LTRC support                                           */
/*  10.03.05    JS    Made CycleCntStamp different for xrt/udp (16/32Bit)    */
/*  22.06.05    JS    addition to HardwareType                               */
/*  25.01.06    JS    GetAPDUStatus for Consumer now returns CycleCounter    */
/*                    of received Consumer, not local CycleCounter           */
/*  19.05.06    JS    Added EDDS_RTSendclockChange                           */
/*  27.06.06    JS    added RsvIntervalRed and RsvIntervalOrange to Params   */
/*  30.06.06    JS    EDDS_RTGetParams now public.                           */
/*  11.08.06    JS    allow cyclebasefactor 0 as valid.                      */
/*  29.09.06    JS    added ReservedIntervalEndMaxNs                         */
/*  14.12.06    JS    removed RsvIntervalOrange and RsvIntervalRed from      */
/*                    EDD_SRV_SRT_GET_PARAMS                                 */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
/*  17.10.07    JS    allow CycleBaseFactors 256,512 and 1024 (=8,16,32ms)   */
/*  12.02.07    JS    bugfix: allow CycleBaseFactor 0 and ignore it within   */
/*                            EDDS_RTSendclockChange(). Access was wrong.    */
/*  02.06.08    JS    added option to configure a faster consumer check      */
/*                    cycle. With this the jitter with consumer MISS         */
/*                    detection can be reduced if system load doent matter.  */
/*                    EDDS_CFG_FAST_CONSUMER_CHECK_CYCLE                     */
/*  10.07.08    JS    added HardwareType flags:                              */
/*                    EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_1_UDP_SUPPORT */
/*                    EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT    */
/*                    EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_3_SUPPORT     */
/*                    EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE             */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  144
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SRT_USR */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

#include "edds_iobuf_int.h"     /* IOBuffer internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_SRT_INCLUDE

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                local constants                            */
/*===========================================================================*/

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotElementGet                       +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets a free KnotElement Index. Returns SRT_CHAIN_IDX_END  +*/
/*+               if no free element present.                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTKnotElementGet(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp) {

	LSA_UINT16 Idx;
	EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			">>> EDDS_RTKnotElementGet(pCSRTComp: 0x%X)", pCSRTComp);

	Idx = pCSRTComp->Knot.KnotElementFree.BottomIdx;

	if (Idx != EDDS_SRT_CHAIN_IDX_END) {

		pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[Idx];

		EDDS_ASSERT(pKnotElement->InUse);

		/*------------------------------------------------------------- */
		/* Remove Knotelement at bottom from free-chain list            */
		/*------------------------------------------------------------- */

		/* if this was the last free element. set Idx to END-tag       */
		if (pKnotElement->UsedFreeLink.Next == EDDS_SRT_CHAIN_IDX_END) {
			pCSRTComp->Knot.KnotElementFree.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
			pCSRTComp->Knot.KnotElementFree.TopIdx = EDDS_SRT_CHAIN_IDX_END;
		} else {
			/* not the last element */
			pCSRTComp->Knot.KnotElementFree.BottomIdx =
					pKnotElement->UsedFreeLink.Next;
			pCSRTComp->Knot.pKnotElementTbl[pKnotElement->UsedFreeLink.Next].UsedFreeLink.Prev =
					EDDS_SRT_CHAIN_IDX_END;
		}

		/*------------------------------------------------------------- */
		/* Put Element to used-chain list (end)                         */
		/*------------------------------------------------------------- */

		/* nothing in queue ? */
		if (pCSRTComp->Knot.KnotElementUsed.TopIdx == EDDS_SRT_CHAIN_IDX_END) {
			pCSRTComp->Knot.KnotElementUsed.BottomIdx = Idx;
			pCSRTComp->Knot.KnotElementUsed.TopIdx = Idx;
			pKnotElement->UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
			pKnotElement->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
		} else {
			pKnotElement->UsedFreeLink.Prev =
					pCSRTComp->Knot.KnotElementUsed.TopIdx;
			pKnotElement->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
			pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.KnotElementUsed.TopIdx].UsedFreeLink.Next =
					Idx;
			pCSRTComp->Knot.KnotElementUsed.TopIdx = Idx;
		}

		pKnotElement->ProviderSendListXRT.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
		pKnotElement->ProviderSendListXRT.TopIdx = EDDS_SRT_CHAIN_IDX_END;
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		pKnotElement->ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
		pKnotElement->ProviderSendListUDP.TopIdx = EDDS_SRT_CHAIN_IDX_END;
#endif
		pKnotElement->ConsumerCheckList.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
		pKnotElement->ConsumerCheckList.TopIdx = EDDS_SRT_CHAIN_IDX_END;
		pKnotElement->ConsumerCnt = 0;
		pKnotElement->ProviderCnt = 0;
		pKnotElement->ProviderByteCnt = 0;
		pKnotElement->InUse = LSA_TRUE;
	}

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"<<< EDDS_RTKnotElementGet(), Idx: %d", Idx);

	return (Idx);
}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotElementFree                      +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             LSA_UINT16                  Index           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees a KnotElement if in use. Dont checks if consumer    +*/
/*+               provider still using it (must be checked outside!)        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTKnotElementFree(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		LSA_UINT16 Index) {

	EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
	LSA_UINT16 Prev;
	LSA_UINT16 Next;

	EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTKnotElementFree(pCSRTComp: 0x%X, Idx: %d)", pCSRTComp,
			Index);

	pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[Index];

	if (pKnotElement->InUse) {
		pKnotElement->InUse = LSA_FALSE;

		/*---------------------------------------------------------------------------*/
		/* Now we can remove  the element  from the InUse-chain.                     */
		/*---------------------------------------------------------------------------*/

		Prev = pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Prev;
		Next = pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Next;

		if ((Prev == EDDS_SRT_CHAIN_IDX_END)
				|| (Next == EDDS_SRT_CHAIN_IDX_END))

				{
			if (Prev == EDDS_SRT_CHAIN_IDX_END) {
				if (Next == EDDS_SRT_CHAIN_IDX_END) {
					/* only one in list */
					pCSRTComp->Knot.KnotElementUsed.BottomIdx =
							EDDS_SRT_CHAIN_IDX_END;
					pCSRTComp->Knot.KnotElementUsed.TopIdx =
							EDDS_SRT_CHAIN_IDX_END;
				} else {
					/* first one in list, but not only one */
					pCSRTComp->Knot.KnotElementUsed.BottomIdx = Next;
					pCSRTComp->Knot.pKnotElementTbl[Next].UsedFreeLink.Prev =
							Prev;
				}
			} else {
				/* last one in list but not only one */
				pCSRTComp->Knot.pKnotElementTbl[Prev].UsedFreeLink.Next = Next;
				pCSRTComp->Knot.KnotElementUsed.TopIdx = Prev;
			}
		} else {
			/* in middle of list */
			pCSRTComp->Knot.pKnotElementTbl[Prev].UsedFreeLink.Next = Next;
			pCSRTComp->Knot.pKnotElementTbl[Next].UsedFreeLink.Prev = Prev;
		}

		/*-----------------------------------------------------------------------*/
		/* Put this element to the end of the free-chain..                       */
		/*-----------------------------------------------------------------------*/

		if (pCSRTComp->Knot.KnotElementFree.TopIdx == EDDS_SRT_CHAIN_IDX_END) /* nothing in chain ? */
		{
			pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Prev =
					EDDS_SRT_CHAIN_IDX_END;
			pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Next =
					EDDS_SRT_CHAIN_IDX_END;
			pCSRTComp->Knot.KnotElementFree.BottomIdx = Index;
			pCSRTComp->Knot.KnotElementFree.TopIdx = Index;
		} else {
			pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Prev =
					pCSRTComp->Knot.KnotElementFree.TopIdx;
			pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Next =
					EDDS_SRT_CHAIN_IDX_END;
			pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.KnotElementFree.TopIdx].UsedFreeLink.Next =
					Index;
			pCSRTComp->Knot.KnotElementFree.TopIdx = Index;
		}

		pKnotElement->InUse = LSA_FALSE;

	}

	EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTKnotElementFree()");

}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotFree                             +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                             EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_RESOURCE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Free a Knot Element and removes it from KnotTable if no   +*/
/*+               consumer/provider usess it anymore.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTKnotFree(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		LSA_UINT32 KnotTableIdx) {

	EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
	LSA_UINT16 Idx;

	EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTKnotFree(pCSRTComp: 0x%X, KTIdx: %d)", pCSRTComp,
			KnotTableIdx);

	EDDS_ASSERT(KnotTableIdx >= pCSRTComp->Knot.KnotCnt);

	Idx = pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx;

	if (Idx != EDDS_SRT_IDX_NOT_USED) /* knot table index used ?*/
	{
		pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[Idx];

		if ((pKnotElement->InUse) && (!pKnotElement->ConsumerCnt)
				&& (!pKnotElement->ProviderCnt)) {
			/* first set to unused, then we free it                  */
			/* so we have no problem with interrupting this sequence */

			pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx =
					EDDS_SRT_IDX_NOT_USED;
			EDDS_RTKnotElementFree(pCSRTComp, Idx);
		}
	}

	EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTKnotElementGet()");
}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotGet                              +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                             EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_RESOURCE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Fills a KnotTableIdx within pKnotTable with a KnotElement +*/
/*+               If none is free a error is returned.                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTKnotGet(
		EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		LSA_UINT32 KnotTableIdx) {

	LSA_UINT32 Idx;
	LSA_RESULT Status;

	EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTKnotGet(pCSRTComp: 0x%X, KTIdx: %d)", pCSRTComp,
			KnotTableIdx);

	Status = EDD_STS_OK;

	EDDS_ASSERT(KnotTableIdx >= pCSRTComp->Knot.KnotCnt);

	/* Not already a Knot element present?*/
	if (pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx
			== EDDS_SRT_IDX_NOT_USED) {
		/* get a free Knot element. We should always because we */
		/* allocated enough in startup.                         */
		Idx = EDDS_RTKnotElementGet(pCSRTComp);
		if (Idx == EDDS_SRT_CHAIN_IDX_END) {
			Status = EDD_STS_ERR_RESOURCE;
			pCSRTComp->Stats.NoMoreKnotElements++;
		} else {
			pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx =
					(LSA_UINT16) Idx;
		}
	}

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTKnotElementGet(), Status: 0x%X", Status);

	return (Status);
}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    EDDS_RTGetAndCheckReductionFactor                 +*/
/*+  Input/Output    :    LSA_UINT16                      ReductionRatio    +*/
/*+                       LSA_BOOL                        Udp               +*/
/*+  Result          :    LSA_UINT16                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ReductionRatio: Provider or Consumer ReductionRatio (1,2,4,8,16..128.. +*/
/*+  Udp           : LSA_TRUE: xRtoverUDP, LSA_FALSE: Xrt                   +*/
/*+                                                                         +*/
/*+  Result:      ReductionFactor.                                          +*/
/*+               0: invalid ReductionRatio                                 +*/
/*+               1..x                                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates the ReductionFactor from the ReductionRatio    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTGetAndCheckReductionFactor(
LSA_UINT16 ReductionRatio,
LSA_BOOL Udp) {

	LSA_UINT16 ReductionFactor;
	LSA_UINT16 Help;
	LSA_UINT32 Min, Max;

	if (Udp) {
		Min = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN;
		Max = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX;
	} else {
		Min = EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN;
		Max = EDD_CSRT_CYCLE_REDUCTION_RATIO_MAX;
	}

	ReductionFactor = 0;

	if ((ReductionRatio >= Min) && (ReductionRatio <= Max)) {
		Help = ReductionRatio;
		ReductionFactor = 1;

		while (!(Help & 1)) /* till first bit is on bit position 0 */
		{
			ReductionFactor++;
			Help >>= 1;
		}

		if (Help != 1)
			ReductionFactor = 0; /* not a valid Ratio */
	}

	return (ReductionFactor);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    SRTAllocRxFrameBuffer                             +*/
/*+  Input/Output    :    EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR *pBufferAddr +*/
/*+                       LSA_UINT32                      BufferSize        +*/
/*+                       LSA_UINT32                      BufferOffset      +*/
/*+  Result          :    LSA_RESULT                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBufferAddr  : Pointer to address for Framebufferpointer               +*/
/*+  BufferSize   : Size of one Receive-buffer                              +*/
/*+  BufferOffset : Offset in Framebuffer where frames will be stored       +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_ALIGN                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates a RxFramebuffer and checks for alignment.       +*/
/*+               (the buffers match the rx-buffer requirements)            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pBufferAddr,pDDB)
//JB 18/11/2014 checked before calling this function
//WARNING: be careful when calling this function, make sure not to use pBufferAddr as null ptr!
//lint -esym(715,pDDB)
//JB 09/12/2014 system adaption (#JB_DISCUSS in code?)
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR SRTAllocRxFrameBuffer(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBufferAddr
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
,
LSA_UINT32 BufferSize,
LSA_UINT32 BufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
)
{

	EDD_UPPER_MEM_PTR_TYPE pBuffer;
	LSA_RESULT Status;
	LSA_UINT32 Addr;
	LSA_UINT16 RetVal;

	Status = EDD_STS_OK;

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&pBuffer,BufferSize);
#else /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
	EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&pBuffer,EDD_FRAME_BUFFER_LENGTH);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

	if (LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
	{
		Status = EDD_STS_ERR_RESOURCE;
		*pBufferAddr = LSA_NULL;
	}
	else
	{

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		EDDS_PTR_TO_ADDR_UPPER(pDDB->hSysDev,(EDD_UPPER_MEM_PTR_TYPE)pBuffer,&Addr);

		Addr += BufferOffset;
#else
		Addr = (LSA_UINT32)pBuffer;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
		if ( (LSA_UINT32)((Addr)) & EDDS_RX_FRAME_BUFFER_ALIGN ) /* alignment check */
		{
			Status = EDD_STS_ERR_ALIGN;
			EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,pBuffer); //lint !e774 JB 09/12/2014 system adaption
			EDDS_FREE_CHECK(RetVal);
		}
		else
		{
			*pBufferAddr = ((EDD_UPPER_MEM_U8_PTR_TYPE)pBuffer)
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			+ BufferOffset
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
			;
		}
	}

	return(Status);

}
//lint +esym(715,pDDB)
//lint +esym(613,pBufferAddr,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    SRTAllocTxFrameBuffer                             +*/
/*+  Input/Output    :    EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR *pBufferAddr +*/
/*+                       LSA_UINT32                      BufferSize        +*/
/*+  Result          :    LSA_RESULT                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBufferAddr  : Pointer to address for Framebufferpointer               +*/
/*+  BufferSize   : Size of one Transmit-buffer                             +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_ALIGN                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates a TxFramebuffer (DWORD-Aligned)                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pBufferAddr,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pBufferAddr as null ptr!
//lint -esym(715,pDDB)
//JB 09/12/2014 system adaption (#JB_DISCUSS in code?)
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR SRTAllocTxFrameBuffer(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBufferAddr,
LSA_UINT32 BufferSize)
{

	EDD_UPPER_MEM_PTR_TYPE pBuffer;
	LSA_RESULT Status;
	LSA_UINT32 Addr;
	LSA_UINT16 RetVal;

	Status = EDD_STS_OK;

	EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&pBuffer,BufferSize);

	if (LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
	{
		Status = EDD_STS_ERR_RESOURCE;
	}
	else
	{

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		EDDS_PTR_TO_ADDR_UPPER(pDDB->hSysDev,(EDD_UPPER_MEM_PTR_TYPE)pBuffer,&Addr);
#else
		Addr = (LSA_UINT32)pBuffer;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */


		if ( (LSA_UINT32)((Addr)) & EDDS_TX_SRT_FRAME_BUFFER_ALIGN ) /* alignment check */
		{
			Status = EDD_STS_ERR_ALIGN;
			EDDS_FREE_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,pBuffer);  //lint !e774 JB 09/12/2014 system adaption
			EDDS_FREE_CHECK(RetVal);
		}
		else
		{

			*pBufferAddr = pBuffer;
		}

	}

	return(Status);

}
//lint +esym(715,pDDB)
//lint +esym(613,pBufferAddr,pDDB)

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    SRTFreeRxFrameBuffer                              +*/
/*+  Input/Output    :    EDD_UPPER_MEM_PTR_TYPE       *pBuffer             +*/
/*+                       LSA_UINT32                    BufferOffset        +*/
/*+  Result          :    LSA_VOID                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuffer      : Pointer to Framebuffer to free                          +*/
/*+  RxBufferOffset Offset in Framebuffer for adjustment.                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees a framebuffer. Checks for LSA_NULL                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTFreeRxFrameBuffer(
EDDS_LOCAL_DDB_PTR_TYPE pDDB,
EDD_UPPER_MEM_PTR_TYPE * pBuffer,
LSA_UINT32 BufferOffset)
{

	EDD_UPPER_MEM_U8_PTR_TYPE pDummy;
	LSA_UINT16 RetVal;

	if (! LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
	{

		/* calculate the pointer used when allocated (- BufferOffset!)   */

		pDummy = ((EDD_UPPER_MEM_U8_PTR_TYPE)(pBuffer) ) - BufferOffset;

		EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,(EDD_UPPER_MEM_PTR_TYPE)pDummy);
		EDDS_FREE_CHECK(RetVal);
	}

}
//lint +esym(613,pDDB)
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    SRTFreeTxFrameBuffer                              +*/
/*+  Input/Output    :    EDD_UPPER_MEM_PTR_TYPE       *pBuffer             +*/
/*+  Result          :    LSA_VOID                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuffer      : Pointer to Framebuffer to free                          +*/
/*+  RxBufferOffset Offset in Framebuffer for adjustment.                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees a framebuffer. Checks for LSA_NULL                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
//lint -esym(715,pDDB)
//JB 09/12/2014 system adaption (#JB_DISCUSS in code?)
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTFreeTxFrameBuffer(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB,
		EDD_UPPER_MEM_PTR_TYPE * pBuffer)
{

	EDD_UPPER_MEM_U8_PTR_TYPE pDummy;
	LSA_UINT16 RetVal;

	if (! LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
	{

		/* calculate the pointer used when allocated (- BufferOffset!)   */

		pDummy = ((EDD_UPPER_MEM_U8_PTR_TYPE)(pBuffer) );

		EDDS_FREE_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,(EDD_UPPER_MEM_PTR_TYPE)pDummy); //lint !e774 !e715 JB 09/12/2014 system adaption
		EDDS_FREE_CHECK(RetVal);
	}

}
//lint +esym(715,pDDB)
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SRTFreeCSRTManagement                       +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DeviceDescriptionBlock                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function frees all memory                            +*/
/*+               It checks for already freed parts in CSRT (LSA_NULL)      +*/
/*+               free errors will be ignored or cause an fatal error       +*/
/*+               (depends on CFG-Variable)                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTFreeCSRTManagement(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB) {

	LSA_UINT16 RetVal = LSA_RET_OK;
	LSA_UINT32 i;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"IN :SRTFreeCSRTManagement(pDDB: 0x%X)", pDDB);

	pCSRTComp = &pDDB->pSRT->CSRT;

	/*----------------------------------------------------------------------------*/
	/* Free Provider management part                                              */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvTbl, LSA_NULL)) {
		EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Prov.pProvTbl);
		EDDS_FREE_CHECK(RetVal);

		pCSRTComp->Prov.pProvTbl = LSA_NULL;
	}

	/*----------------------------------------------------------------------------*/
	/* Free provider cr management part                                           */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvCrTbl, LSA_NULL)) {
		/* free provider-frame buffers. LSA_NULL is checked ! */
		LSA_UINT16 result;

		for (i = 0; i < pCSRTComp->Cfg.ProviderCnt; i++) {
			SRTFreeTxFrameBuffer(pDDB,
					(EDD_UPPER_MEM_PTR_TYPE) pCSRTComp->Prov.pProvCrTbl[i].pBuffer[0]);
			pCSRTComp->Prov.pProvCrTbl[i].pBuffer[0] = LSA_NULL;

			SRTFreeTxFrameBuffer(pDDB,
					(EDD_UPPER_MEM_PTR_TYPE) pCSRTComp->Prov.pProvCrTbl[i].pBuffer[1]);
			pCSRTComp->Prov.pProvCrTbl[i].pBuffer[1] = LSA_NULL;

			SRTFreeTxFrameBuffer(pDDB,
					(EDD_UPPER_MEM_PTR_TYPE) pCSRTComp->Prov.pProvCrTbl[i].pBuffer[2]);
			pCSRTComp->Prov.pProvCrTbl[i].pBuffer[2] = LSA_NULL;
		}

		EDDS_IOBUFFER_FREE_MNGM_MEM(pDDB->hSysDev, &result,
				(LSA_VOID*) pCSRTComp->Prov.pProvCrTbl);
	}

	/*----------------------------------------------------------------------------*/
	/* Free provider group table                                                  */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvGroupTbl, LSA_NULL)) {
		EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Prov.pProvGroupTbl);
		EDDS_FREE_CHECK(RetVal);

		pCSRTComp->Prov.pProvGroupTbl = LSA_NULL;
	}

	/*----------------------------------------------------------------------------*/
	/* Free consumer management part (part one)                                   */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsTbl, LSA_NULL)) {
		EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Cons.pConsTbl);
		EDDS_FREE_CHECK(RetVal);

		pCSRTComp->Cons.pConsTbl = LSA_NULL;
		pCSRTComp->Cons.pConsTbl2 = LSA_NULL;
	}

	/*----------------------------------------------------------------------------*/
	/* Free consumer management part (part two)                                   */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsFrameTbl, LSA_NULL)) {
		EDDS_FREE_LOCAL_FAST_MEM(&RetVal, pCSRTComp->Cons.pConsFrameTbl);
		EDDS_FREE_CHECK(RetVal);

		pCSRTComp->Cons.pConsFrameTbl = LSA_NULL;
		pCSRTComp->Cons.pConsFrameTbl2 = LSA_NULL;
	}

	/*----------------------------------------------------------------------------*/
	/* Frees consumer cr management part                                          */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsCrTbl, LSA_NULL)) {
		/* free provider-frame buffers. LSA_NULL is checked ! */
		LSA_UINT16 result;

		for (i = 0; i < pCSRTComp->Cons.ConsumerCnt; i++) {
			LSA_UINT8 idx;

			// NOTE:
			// Buffers of the internal EDDS receive resources and IO buffers (2 buffers allocated
			// in SRTInitCSRTManagement) are exchanged among each other. In other words: Buffers
			// in pBuffer can be IO buffers and/or buffers from the internal receive rescources
			// of the EDDS. The buffer in pBuffer that is currently used as receive buffer is
			// always NULL. Only 1 buffer in pBuffer is used as receive resource.
			// Because all buffers from the internal receive resource and IO buffers are allocated
			// from the same memory it doesnt matter who free's the buffers as long as all buffers
			// are freed at shutdown. Simply free the buffers referenced in pBuffer. Possible
			// IO buffers previously transfered as receive resource are freed when the EDDS free's
			// it's internal receive resources.

			EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr = &pCSRTComp->Cons.pConsCrTbl[i];

			for (idx = 0; idx < 3; idx++) {
				if ( LSA_NULL != pCr->pBuffer[idx]) {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
					SRTFreeRxFrameBuffer(pDDB,(EDD_UPPER_MEM_PTR_TYPE)pCr->pBuffer[idx], pDDB->pGlob->RxBufferOffset);
#else /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
					EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev, &RetVal,
							(EDD_UPPER_MEM_PTR_TYPE)pCr->pBuffer[idx]);
					EDDS_FREE_CHECK(RetVal);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

					pCSRTComp->Cons.pConsCrTbl[i].pBuffer[idx] = LSA_NULL;
				}
			}
		}

		EDDS_IOBUFFER_FREE_MNGM_MEM(pDDB->hSysDev, &result,
				(LSA_VOID*) pCSRTComp->Cons.pConsCrTbl);
	}

	/*----------------------------------------------------------------------------*/
	/* Free management info block for IOBuffer                                    */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->pIOBufferManagement, LSA_NULL)) {
		LSA_UINT16 result;

		EDDS_IOBUFFER_FREE_MNGM_MEM(pDDB->hSysDev, &result,
				pCSRTComp->pIOBufferManagement);
	}

	/*----------------------------------------------------------------------------*/
	/* Frees consumer ClearOnMiss buffer                                          */
	/*----------------------------------------------------------------------------*/
	if ( LSA_NULL != pCSRTComp->Cons.pClearOnMissBuffer) {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		SRTFreeRxFrameBuffer(pDDB,(EDD_UPPER_MEM_PTR_TYPE)pCSRTComp->Cons.pClearOnMissBuffer, pDDB->pGlob->RxBufferOffset);
#else /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
		EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev, &RetVal,
				(EDD_UPPER_MEM_PTR_TYPE)pCSRTComp->Cons.pClearOnMissBuffer);
		EDDS_FREE_CHECK(RetVal);
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
		pCSRTComp->Cons.pClearOnMissBuffer = LSA_NULL;
	}

	/*----------------------------------------------------------------------------*/
	/* Frees knot-table-management                                                */
	/*----------------------------------------------------------------------------*/

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotTbl, LSA_NULL)) {

		EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Knot.pKnotTbl);
		EDDS_FREE_CHECK(RetVal);

		pCSRTComp->Knot.pKnotTbl = LSA_NULL;
	}

	if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotElementTbl, LSA_NULL)) {

		EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Knot.pKnotElementTbl);
		EDDS_FREE_CHECK(RetVal);

		pCSRTComp->Knot.pKnotElementTbl = LSA_NULL;
	}

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

	/*----------------------------------------------------------------------------*/
	/* Free  timers                                                               */
	/*----------------------------------------------------------------------------*/

	if ( pCSRTComp->ConsCycleTimer.InUse)
	{
		pCSRTComp->ConsCycleTimer.InUse = LSA_FALSE;
		EDDS_TimerFree(pCSRTComp->ConsCycleTimer.EDDSTimerID);
	}

	if ( pCSRTComp->ProvCycleTimer.InUse)
	{
		pCSRTComp->ProvCycleTimer.InUse = LSA_FALSE;
		EDDS_TimerFree(pCSRTComp->ProvCycleTimer.EDDSTimerID);
	}

	/*----------------------------------------------------------------------------*/
	/* Free internal timer RQBs                                                   */
	/*----------------------------------------------------------------------------*/

	/* free local RQBs. Note that we dont need to check for NULL */
	/* pointer here, this is done inside free function.          */

	EDDS_FreeInternalRQB( pCSRTComp->ConsTimerRQB.pRQB);
	pCSRTComp->ConsTimerRQB.pRQB = LSA_NULL;
	EDDS_FreeInternalRQB( pCSRTComp->ProvTimerRQB.pRQB);
	pCSRTComp->ProvTimerRQB.pRQB = LSA_NULL;
	EDDS_FreeInternalRQB( pCSRTComp->ProvIndTriggerRQB.pRQB);
	pCSRTComp->ProvIndTriggerRQB.pRQB = LSA_NULL;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
	EDDS_FreeInternalRQB(pCSRTComp->ConsIndTriggerRQB.pRQB);
	pCSRTComp->ConsIndTriggerRQB.pRQB = LSA_NULL;

	EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT:SRTFreeCSRTManagement()");

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SRTInitCSRTManagement                       +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                             EDDS_DDB_COMP_CSRT_TYPE_PTR   pCSRTComp     +*/
/*+                             EDDS_CSRT_INI_PTR_TYPE        pCSRTIni      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DeviceDescriptionBlock                       +*/
/*+  pCSRTComp    : Pointer to CSRT Management which will be filled         +*/
/*+  pCSRTIni     : Pointer to init-parameter                               +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NO_TIMER                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function allocates and initializes all structures    +*/
/*+               within  CSRTComp.  CSRTComp has to be preinitialized with +*/
/*+               0 on entry!!                                              +*/
/*+                                                                         +*/
/*+               Consumer-management in fast memory                        +*/
/*+               Consumer-management in normal memory                      +*/
/*+               Provider-management in normal memory                      +*/
/*+               Knot    -management in normal memory                      +*/
/*+               All transferbuffers for consumer/provider will be alloc.  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp,pCSRTIni,pMgmInfoBlock,pDDB)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB) - pMgmInfoBlock = pCSRTComp-> ...
//WARNING: be careful when calling this function, make sure not to use pCSRTComp or pCSRTInit as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR SRTInitCSRTManagement(
		EDDS_LOCAL_DDB_PTR_TYPE pDDB, EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
		EDDS_CSRT_INI_PTR_TYPE pCSRTIni) {

	LSA_RESULT Status;
	LSA_UINT32 Size;
	LSA_UINT16 i;
	LSA_UINT32 Help;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	LSA_UINT16 RetVal;
#endif

	EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR pMgmInfoBlock = LSA_NULL;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :SRTInitCSRTManagement(pDDB: 0x%X,pCSRTComp: 0x%X, pCSRTIni: 0x%X)",
			pDDB, pCSRTComp, pCSRTIni);

	Status = EDD_STS_OK;

	/*-----------------------------------------------------------------------*/
	/* check for valid parameters                                            */
	/* cyclic SRT                                                            */
	/*   - ConsumerCnt have to be > 0                                        */
	/*   - ProviderCnt have to be > 0 & <= EDDS_SRT_PROVIDER_CNT_MAX         */
	/*   - ProviderCnt+ConsumerCnt is limit to 0xFFFF (save space)           */
	/*   - CycleBaseFactor     range (only 32,64,128..1024  supported yet)   */
	/*                         Note: if we change this, the timestamp calc   */
	/*                               may needs to be changed!!               */
	/*   - FrameID-Range for Consumer have to be in valid range              */
	/*   - ProviderCycleDropCnt range                                        */
	/*                                                                       */
	/* Note: We limit ProviderCyclePhaseMaxByteCnt to 0xFFFF because we use  */
	/*       a 16-Bit variable (ProviderByteCntAboveMax) internal to save    */
	/*       space within Knot-Table!                                        */
	/*-----------------------------------------------------------------------*/

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	pCSRTComp->schedulerModuloFactor = pCSRTIni->CycleBaseFactor / (32 * pDDB->pGlob->schedulerCycle);
#endif

	if (((pCSRTIni->ConsumerCntClass2 == 0)
			&& (pCSRTIni->ConsumerCntClass1 == 0)) || /* both 0 not allowed */
	(pCSRTIni->ProviderCnt == 0)
			|| (pCSRTIni->ProviderCnt > EDDS_SRT_PROVIDER_CNT_MAX)
			||
#if 0 /* am2219 */
			( pCSRTIni->ProviderCnt + pCSRTIni->ConsumerCntClass2 + pCSRTIni->ConsumerCntClass1 > 0xFFFE ) || /* we limit KnotElements*/
			( pCSRTIni->ProviderCyclePhaseMaxByteCnt > 0xFFFF ) || /* we limit MaxByte Cnt because ProviderByteCntAboveMax is 16 Bit */
#endif /* am2219 */
			((pCSRTIni->CycleBaseFactor != 32) && /*  1ms */
			(pCSRTIni->CycleBaseFactor != 64) && /*  2ms */
			(pCSRTIni->CycleBaseFactor != 128) && /*  4ms */
			(pCSRTIni->CycleBaseFactor != 256) && /*  8ms */
			(pCSRTIni->CycleBaseFactor != 512) && /* 16ms */
			(pCSRTIni->CycleBaseFactor != 1024) /* 32ms */
			)
			|| ((pCSRTIni->ConsumerCntClass2 != 0)
					&& /* if class2 present, check range */
					((pCSRTIni->ConsumerFrameIDBaseClass2
							< EDD_SRT_FRAMEID_CSRT_START1)
							|| (pCSRTIni->ConsumerFrameIDBaseClass2
									+ pCSRTIni->ConsumerCntClass2 - 1
									> EDD_SRT_FRAMEID_CSRT_STOP1)))
			|| ((pCSRTIni->ConsumerCntClass1 != 0)
					&& /* if class1/4 present, check range */
					((pCSRTIni->ConsumerFrameIDBaseClass1
							< EDD_SRT_FRAMEID_CSRT_START2)
							|| (pCSRTIni->ConsumerFrameIDBaseClass1
									+ pCSRTIni->ConsumerCntClass1 - 1
									> EDD_SRT_FRAMEID_CSRT_STOP2)))
			|| (pCSRTIni->ProviderCycleDropCnt
					> EDDS_SRT_PROVIDER_CYCLE_DROP_MAX)
#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			|| (pCSRTIni->CycleBaseFactor / 32 < pDDB->pGlob->schedulerCycle)
	        // schedulerModuloFactor: check for 1,2,4,8,16,32 ... value
			|| !(pCSRTComp->schedulerModuloFactor && !(pCSRTComp->schedulerModuloFactor & (pCSRTComp->schedulerModuloFactor-1)))
#endif
			) {
		Status = EDD_STS_ERR_PARAM;
	} else {
		/* store values in management structure */

		pCSRTComp->HandleCnt = 0;
		pCSRTComp->Cfg = *pCSRTIni;
		pCSRTComp->schedulerCounter = 0;

		/* calculate derived values..           */

		if (pCSRTComp->Cfg.ConsumerCntClass2) {
			pCSRTComp->Cons.ConsumerFrameIDBase1 =
					pCSRTComp->Cfg.ConsumerFrameIDBaseClass2;
			pCSRTComp->Cons.ConsumerFrameIDTop1 =
					pCSRTComp->Cfg.ConsumerFrameIDBaseClass2
							+ pCSRTComp->Cfg.ConsumerCntClass2 - 1;
		} else {
			pCSRTComp->Cons.ConsumerFrameIDBase1 = 0; /* not used */
			pCSRTComp->Cons.ConsumerFrameIDTop1 = 0; /* not used */
		}

		if (pCSRTComp->Cfg.ConsumerCntClass1) {
			pCSRTComp->Cons.ConsumerFrameIDBase2 =
					pCSRTComp->Cfg.ConsumerFrameIDBaseClass1;
			pCSRTComp->Cons.ConsumerFrameIDTop2 =
					pCSRTComp->Cfg.ConsumerFrameIDBaseClass1
							+ pCSRTComp->Cfg.ConsumerCntClass1 - 1;
		} else {
			pCSRTComp->Cons.ConsumerFrameIDBase2 = 0; /* not used */
			pCSRTComp->Cons.ConsumerFrameIDTop2 = 0; /* not used */
		}

		pCSRTComp->Cons.ConsumerCnt = pCSRTComp->Cfg.ConsumerCntClass2
				+ pCSRTComp->Cfg.ConsumerCntClass1; /* > 0! */

		/* calculate cyclebasetime in ns. (1000* CycleBaseFactor * 31.25)        */

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

		pCSRTComp->ProvCycleTimer.BaseTimeNS = pCSRTComp->Cfg.CycleBaseFactor * EDDS_CSRT_CYCLE_BASE_TIME_NS;

		/* EDDS_CFG_FAST_CONSUMER_CHECK_CYCLE (not a documented feature yet!)    */
		/* Option to configure a faster consumer check cycle. With this          */
		/* the jitter with consumer MISS detection can be reduced if system load */
		/* doesnt matter. The consumer is checked more often for a MISS          */

#ifndef EDDS_CFG_FAST_CONSUMER_CHECK_CYCLE
		pCSRTComp->ConsCycleTimer.BaseTimeNS = pCSRTComp->ProvCycleTimer.BaseTimeNS * EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN;
#else
		pCSRTComp->ConsCycleTimer.BaseTimeNS = pCSRTComp->ProvCycleTimer.BaseTimeNS;
#endif

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

		/* calculate shift factor to calculate 31.25us based time stamp from CycleCounter */
		/* so we can calculate the Timestamp like this:                                   */
		/* Timestamp =ActualCycleCounter << CycleStampAdjustShift                         */
		/* NOTE: This assumes we have 2-potenz timebase (e.g.1,2,4 ms). If this changes   */
		/*       we cant shift but have to multiply!                                      */

		Help = pCSRTComp->Cfg.CycleBaseFactor;
		i = 0;
		while (!(Help & 0x1)) {
			i++;
			Help >>= 1;
		}

		pCSRTComp->CycleStampAdjustShift = i;

		/* @fixme am2219 respect CycleBaseFactor with edds_scheduler cycle */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

		/* we currently only support multiple of 1 ms. we check this again.  */

		if (( pCSRTComp->ProvCycleTimer.BaseTimeNS / (LSA_UINT32) 1000000 ) == 0 ) Status = EDD_STS_ERR_PARAM;

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

		/* init some vars for indication status */

		pCSRTComp->Indication.ConsCheckIndications = LSA_FALSE;
		pCSRTComp->Indication.ConsIndicationPending = LSA_FALSE;
		pCSRTComp->Indication.ConsOverloadOccured = LSA_FALSE;
		pCSRTComp->Indication.ProvFirstSendOccured = LSA_FALSE;
		pCSRTComp->Indication.ProvIndicationPending = LSA_FALSE;
		pCSRTComp->Indication.ProvOverloadOccured = LSA_FALSE;

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		pCSRTComp->IPParams.TTL = EDD_DEFAULT_TTL;
#endif

	}

	/*----------------------------------------------------------------------------*/
	/* Allocate management info block for IOBuffer                                */
	/*----------------------------------------------------------------------------*/

	EDDS_IOBUFFER_ALLOC_MNGM_MEM(pDDB->hSysDev, (EDD_UPPER_MEM_PTR_TYPE*)&pCSRTIni->hIOBufferManagement,
	sizeof(EDDS_CSRT_MGM_INFOBLOCK_TYPE) );

	if (LSA_HOST_PTR_ARE_EQUAL(pCSRTIni->hIOBufferManagement, LSA_NULL)) {
		Status = EDD_STS_ERR_RESOURCE;
	} else {
		pCSRTComp->pIOBufferManagement = pCSRTIni->hIOBufferManagement;

		pMgmInfoBlock =
				(EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR) pCSRTIni->hIOBufferManagement;
		pMgmInfoBlock->ConsumerCrCnt = H_TO_LE_S(
				(LSA_UINT16)pCSRTComp->Cons.ConsumerCnt); // convert to LittleEndian
		pMgmInfoBlock->ProviderCrCnt = H_TO_LE_S(pCSRTComp->Cfg.ProviderCnt); // convert to LittleEndian
	}

	/*----------------------------------------------------------------------------*/
	/* Initialize consumer parts                                                  */
	/*----------------------------------------------------------------------------*/

	if (Status == EDD_STS_OK) {

		pCSRTComp->Cons.ConsumerCycle.CycleInProgress = LSA_FALSE;
		pCSRTComp->Cons.ConsumerCycle.MinInterval =
				EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN; /* dont care */
		pCSRTComp->Cons.ConsumerCycle.MaxInterval =
				EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN; /* dont care */

		/* allocate consumer parts in fast memory. */

		/* We use one array holding all Consumers of both arrays.        */
		/* The array2 Consumers are located behind the array1 Consumers  */
		/* so the ConsumerID is still the index into the table           */
		Size = sizeof(EDDS_CSRT_CONSUMER_FRAME_TYPE)
				* pCSRTComp->Cons.ConsumerCnt;

		EDDS_ALLOC_LOCAL_FAST_MEM(
				((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Cons.pConsFrameTbl),
				Size);

		if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsFrameTbl, LSA_NULL)) {
			Status = EDD_STS_ERR_RESOURCE;
		} else {
			EDDS_MEMSET_LOCAL(pCSRTComp->Cons.pConsFrameTbl, 0, Size);
			/* Help-Pointer for better accesss/debug */
			pCSRTComp->Cons.pConsFrameTbl2 =
					&pCSRTComp->Cons.pConsFrameTbl[pCSRTComp->Cfg.ConsumerCntClass2];
		}

		/* allocate consumer parts in normal memory */

		if (Status == EDD_STS_OK) {
			/* We use one array holding both xRT and xRToverUDP Consumers    */
			/* The xRToverUDP Consumers are located behind the xRT Consumers */
			/* so the ConsumerID is still the index into the table           */
			Size = sizeof(EDDS_CSRT_CONSUMER_TYPE)
					* pCSRTComp->Cons.ConsumerCnt;

			EDDS_ALLOC_LOCAL_MEM(
					((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Cons.pConsTbl), Size);

			if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsTbl, LSA_NULL)) {
				Status = EDD_STS_ERR_RESOURCE;
			} else {
				EDDS_MEMSET_LOCAL(pCSRTComp->Cons.pConsTbl, 0, Size);
				/* Help-Pointer for better accesss/debug */
				pCSRTComp->Cons.pConsTbl2 =
						&pCSRTComp->Cons.pConsTbl[pCSRTComp->Cfg.ConsumerCntClass2];
			}
		}

		if (Status == EDD_STS_OK) {
			/* init parts of management. we init the free-chain with */
			/* all consumer.                                         */

			pCSRTComp->Cons.ConsumerFree.BottomIdx = 0;
			pCSRTComp->Cons.ConsumerFree.TopIdx =
					(LSA_UINT16) (pCSRTComp->Cons.ConsumerCnt - 1);

			pCSRTComp->Cons.ConsumerUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */
			pCSRTComp->Cons.ConsumerUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */

			for (i = 0; i < pCSRTComp->Cons.ConsumerCnt; i++) {

				pCSRTComp->Cons.pConsTbl[i].UsedFreeLink.Next = i + 1;

				if (i == 0)
					pCSRTComp->Cons.pConsTbl[i].UsedFreeLink.Prev =
							EDDS_SRT_CHAIN_IDX_END;
				else
					pCSRTComp->Cons.pConsTbl[i].UsedFreeLink.Prev = i - 1;

				pCSRTComp->Cons.pConsTbl[i].Type =
						EDDS_CSRT_CONSUMER_TYPE_NOT_USED;
				pCSRTComp->Cons.pConsFrameTbl[i].State =
						EDDS_SRT_CONS_STATE_INIT;
			}

			/* last chain element */

			pCSRTComp->Cons.pConsTbl[pCSRTComp->Cons.ConsumerCnt - 1].UsedFreeLink.Next =
					EDDS_SRT_CHAIN_IDX_END;

		}

		/* allocate consumer parts for CR in shared memory */

		if (Status == EDD_STS_OK) {
			Size = sizeof(EDDS_CSRT_CONSUMER_CR_TYPE)
					* pCSRTComp->Cons.ConsumerCnt;

			EDDS_IOBUFFER_ALLOC_MNGM_MEM(pDDB->hSysDev, (EDD_UPPER_MEM_PTR_TYPE*)&pCSRTComp->Cons.pConsCrTbl, Size );

			if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsCrTbl, LSA_NULL)) {
				Status = EDD_STS_ERR_RESOURCE;
			} else {
				// set all values to 0
				EDDS_MEMSET_LOCAL(pCSRTComp->Cons.pConsCrTbl, 0, Size);
				// set consumer Cr Table
				pMgmInfoBlock->pConsumerCrTable = pCSRTComp->Cons.pConsCrTbl;
			}
		}

		/* ----------------------------------------------------------------------*/
		/* Allocate consumer-frame-buffers. Two buffers for each consumer.       */
		/* Buffers are located in shared memory.                                 */
		/* ----------------------------------------------------------------------*/

		i = 0;
		while (i < pCSRTComp->Cons.ConsumerCnt) {
			// all values were set to 0 on initialization
			//pCSRTComp->Cons.pConsCrTbl[i].CrUsed   = LSA_FALSE;
			//pCSRTComp->Cons.pConsCrTbl[i].IsLocked = LSA_FALSE;
			//pCSRTComp->Cons.pConsCrTbl[i].Type     = EDDS_CSRT_CONSUMER_TYPE_NOT_USED;

			// To be aware of endianess issues take byte that is pointed to
			// by User, XChange and EDDS. Access to byte by address is endianess
			// independent. All needed informations are stored in that byte.

			//pCSRTComp->Cons.pConsCrTbl[i].User    = 0;
			//pCSRTComp->Cons.pConsCrTbl[i].XChange = 0;
			//pCSRTComp->Cons.pConsCrTbl[i].EDDS    = 0;

			// NOTE:
			// Information like index and new flag is always stored in one byte in
			// the EXCHANGE variable at its base address. This applies to EDDS and
			// IOBuffer when accessing these variables. Access to this byte by byte
			// pointer does not need any consideration of endianess.
			//
			// example:
			// Value stored in memory for Xchange with reference to buffer 1 and
			// new flag is 0x8100. Index and new flag can be retrieved by accessing
			// Xchange with its address casted to a byte pointer independently from
			// host endianess.
			//
			// index = (*((LSA_UINT8*)&Xchange)) & ~MASK

			// *((LSA_UINT8*)&pCSRTComp->Cons.pConsCrTbl[i].User)    = 0;
			*((LSA_UINT8*) &pCSRTComp->Cons.pConsCrTbl[i].XChange) = 1;
			*((LSA_UINT8*) &pCSRTComp->Cons.pConsCrTbl[i].EDDS) = 2;

			if (Status == EDD_STS_OK) {
				Status = SRTAllocRxFrameBuffer(pDDB,
						&pCSRTComp->Cons.pConsCrTbl[i].pBuffer[0]
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
						,
						pDDB->pGlob->RxBufferSize,
						pDDB->pGlob->RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
						);
			}
			if (Status == EDD_STS_OK) {
				Status = SRTAllocRxFrameBuffer(pDDB,
						&pCSRTComp->Cons.pConsCrTbl[i].pBuffer[1]
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
						,
						pDDB->pGlob->RxBufferSize,
						pDDB->pGlob->RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
						);
			}

			// pCSRTComp->Cons.pConsCrTbl[i].pBuffer[2] = LSA_NULL;

			i++;
		}

		/*----------------------------------------------------------------------------*/
		/* Allocate ClearOnMiss buffer                                                */
		/*----------------------------------------------------------------------------*/
		if (Status == EDD_STS_OK) {
			Status = SRTAllocRxFrameBuffer(pDDB,
					&pCSRTComp->Cons.pClearOnMissBuffer
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
					,
					pDDB->pGlob->RxBufferSize,
					pDDB->pGlob->RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
					);
		}

	}

	/*----------------------------------------------------------------------------*/
	/* Initialize provider parts                                                  */
	/*----------------------------------------------------------------------------*/

	if (Status == EDD_STS_OK) {

		/* Set statevariables of Sender             */
		pCSRTComp->Prov.ProviderCycle.Xrt.Type = EDDS_CSRT_PROVIDER_TYPE_XRT;
		pCSRTComp->Prov.ProviderCycle.Xrt.ActProvIdx = EDDS_SRT_CHAIN_IDX_END;
		pCSRTComp->Prov.ProviderCycle.Xrt.MinInterval =
				EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN;
		pCSRTComp->Prov.ProviderCycle.Xrt.MaxInterval =
				EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN;

		/* setup some variables (already 0) */
		//pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress    = LSA_FALSE;
		//pCSRTComp->Prov.ProviderCycle.Xrt.CycleSendStarted   = LSA_FALSE;
		//pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend        = LSA_FALSE;
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		/* Set statevariables of Sender             */
		pCSRTComp->Prov.ProviderCycle.Udp.Type = EDDS_CSRT_PROVIDER_TYPE_UDP;
		pCSRTComp->Prov.ProviderCycle.Udp.ActProvIdx = EDDS_SRT_CHAIN_IDX_END;
		pCSRTComp->Prov.ProviderCycle.Udp.MinInterval = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN;
		pCSRTComp->Prov.ProviderCycle.Udp.MaxInterval = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN;

		/* setup some variables (already 0) */
		//pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress    = LSA_FALSE;
		//pCSRTComp->Prov.ProviderCycle.Udp.CycleSendStarted   = LSA_FALSE;
		//pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend        = LSA_FALSE;
#endif
		/* allocate provider parts in normal memory */

		Size = sizeof(EDDS_CSRT_PROVIDER_TYPE) * pCSRTComp->Cfg.ProviderCnt;

		EDDS_ALLOC_LOCAL_MEM(
				((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Prov.pProvTbl), Size);

		if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvTbl, LSA_NULL)) {
			Status = EDD_STS_ERR_RESOURCE;
		} else {
			EDDS_MEMSET_LOCAL(pCSRTComp->Prov.pProvTbl, 0, Size);

			/* init parts of management. we init the free-chain with */
			/* all consumer.                                         */

			pCSRTComp->Prov.ProviderFree.BottomIdx = 0;
			pCSRTComp->Prov.ProviderFree.TopIdx = pCSRTComp->Cfg.ProviderCnt
					- 1;

			pCSRTComp->Prov.ProviderUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */
			pCSRTComp->Prov.ProviderUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */

			for (i = 0; i < pCSRTComp->Cfg.ProviderCnt; i++) {

				pCSRTComp->Prov.pProvTbl[i].UsedFreeLink.Next = i + 1;

				if (i == 0)
					pCSRTComp->Prov.pProvTbl[i].UsedFreeLink.Prev =
							EDDS_SRT_CHAIN_IDX_END;
				else
					pCSRTComp->Prov.pProvTbl[i].UsedFreeLink.Prev = i - 1;

				pCSRTComp->Prov.pProvTbl[i].Type =
						EDDS_CSRT_PROVIDER_TYPE_NOT_USED;
				pCSRTComp->Prov.pProvTbl[i].Properties = 0;
				pCSRTComp->Prov.pProvTbl[i].Active = LSA_FALSE;
			}

			/* last chain element */

			pCSRTComp->Prov.pProvTbl[pCSRTComp->Cfg.ProviderCnt - 1].UsedFreeLink.Next =
					EDDS_SRT_CHAIN_IDX_END;

		}

		/* allocate provider parts for CR in shared memory */

		if (Status == EDD_STS_OK) {
			Size = sizeof(EDDS_CSRT_PROVIDER_CR_TYPE)
					* pCSRTComp->Cfg.ProviderCnt;

			EDDS_IOBUFFER_ALLOC_MNGM_MEM(pDDB->hSysDev, (EDD_UPPER_MEM_PTR_TYPE*)&pCSRTComp->Prov.pProvCrTbl, Size );

			if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvCrTbl, LSA_NULL)) {
				Status = EDD_STS_ERR_RESOURCE;
			} else {
				// set all values to 0
				EDDS_MEMSET_LOCAL(pCSRTComp->Prov.pProvCrTbl, 0, Size);
				// set provider Cr table
				pMgmInfoBlock->pProviderCrTable = pCSRTComp->Prov.pProvCrTbl;
			}
		}

		/* ----------------------------------------------------------------------*/
		/* allocate provider-frame-buffers. three buffers for each provider.     */
		/* (we allocate for provider the same buffers which we use for consumer  */
		/*  so we are able to exchange provider with consumer-buffers if needed) */
		/* ----------------------------------------------------------------------*/

		i = 0;
		while (i < pCSRTComp->Cfg.ProviderCnt) {
			// already 0
			//pCSRTComp->Prov.pProvCrTbl[i].CrUsed   = LSA_FALSE;
			//pCSRTComp->Prov.pProvCrTbl[i].IsLocked = LSA_FALSE;
			//pCSRTComp->Prov.pProvCrTbl[i].Type     = EDDS_CSRT_PROVIDER_TYPE_NOT_USED;

			// To be aware of endianess issues take byte that is pointed to
			// by User, XChange and EDDS. Access to byte by address is endianess
			// independent. All needed informations are stored in that byte.

			//pCSRTComp->Prov.pProvCrTbl[i].User    = 0;
			//pCSRTComp->Prov.pProvCrTbl[i].XChange = 0;
			//pCSRTComp->Prov.pProvCrTbl[i].EDDS    = 0;
			//pCSRTComp->Prov.pProvCrTbl[i].Latest  = 0;

			// NOTE:
			// Information like index and new flag is always stored in one byte in
			// the EXCHANGE variable at its base address. This applies to EDDS and
			// IOBuffer when accessing these variables. Access to this byte by byte
			// pointer does not need any consideration of endianess.
			//
			// example:
			// Value stored in memory for Xchange with reference to buffer 1 and
			// new flag is 0x8100. Index and new flag can be retrieved by accessing
			// Xchange with its address casted to a byte pointer independently from
			// host endianess.
			//
			// index = (*((LSA_UINT8*)&Xchange)) & ~MASK

			//*((LSA_UINT8*)&pCSRTComp->Prov.pProvCrTbl[i].User)    = 0;
			*((LSA_UINT8*) &pCSRTComp->Prov.pProvCrTbl[i].XChange) = 1;
			*((LSA_UINT8*) &pCSRTComp->Prov.pProvCrTbl[i].EDDS) = 2;
			*((LSA_UINT8*) &pCSRTComp->Prov.pProvCrTbl[i].Latest) = 1;

			if (Status == EDD_STS_OK) {
				Status = SRTAllocTxFrameBuffer(pDDB,
						&pCSRTComp->Prov.pProvCrTbl[i].pBuffer[0],
						EDD_FRAME_BUFFER_LENGTH);
			}

			if (Status == EDD_STS_OK) {
				Status = SRTAllocTxFrameBuffer(pDDB,
						&pCSRTComp->Prov.pProvCrTbl[i].pBuffer[1],
						EDD_FRAME_BUFFER_LENGTH);
			}

			if (Status == EDD_STS_OK) {
				Status = SRTAllocTxFrameBuffer(pDDB,
						&pCSRTComp->Prov.pProvCrTbl[i].pBuffer[2],
						EDD_FRAME_BUFFER_LENGTH);
			}

			i++;
		}

	}

	/* Alloc and initialize ProviderGroup-Array */

	if (Status == EDD_STS_OK) {

		/* allocate provider Group-Array in normal memory */

		Size = sizeof(EDDS_CSRT_PROV_GROUP_TYPE)
				* EDD_CFG_CSRT_MAX_PROVIDER_GROUP;

		EDDS_ALLOC_LOCAL_MEM(
				((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Prov.pProvGroupTbl),
				Size);

		if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvGroupTbl, LSA_NULL)) {
			Status = EDD_STS_ERR_RESOURCE;
		} else {
			/* Initialize Group-data. */

			EDDS_MEMSET_LOCAL(pCSRTComp->Prov.pProvGroupTbl,
					EDD_CSRT_DSTAT_GLOBAL_INIT_VALUE, Size);

			// the same initialization as a loop
			//for (i=0; i<EDD_CFG_CSRT_MAX_PROVIDER_GROUP; i++)
			//{
			//    /* Initialize Global Provider State for all Groups */
			//
			//    pCSRTComp->Prov.pProvGroupTbl[i].ProviderDataStatus  = EDD_CSRT_DSTAT_GLOBAL_INIT_VALUE;
			//}
		}
	}

	/*----------------------------------------------------------------------------*/
	/* Initialize knot-management for provider and consumer                       */
	/*----------------------------------------------------------------------------*/

	if (Status == EDD_STS_OK) {
		/* caclulate number of Knot-table-entrys:           */
		/* The number is fixed.                             */
		/* 2^(EDDS_CSRT_Cycle_REDUCTION_RATIO_FACTOR_MAX)-1 */

		pCSRTComp->Knot.KnotCnt =
				EDDSGet2Potenz(EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX) - 1;

		Size = sizeof(EDDS_CSRT_KNOT_TYPE) * pCSRTComp->Knot.KnotCnt;

		EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,
				"SRTInitCSRTManagement: Allocating %d knots",
				pCSRTComp->Knot.KnotCnt);

		EDDS_ALLOC_LOCAL_MEM(
				((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Knot.pKnotTbl), Size);

		if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotTbl, LSA_NULL)) {
			Status = EDD_STS_ERR_RESOURCE;
		} else {
			for (i = 0; i < pCSRTComp->Knot.KnotCnt; i++) {
				pCSRTComp->Knot.pKnotTbl[i].KnotElementIdx =
						EDDS_SRT_IDX_NOT_USED;
				pCSRTComp->Knot.pKnotTbl[i].ProviderCntAboveMax = 0;
				pCSRTComp->Knot.pKnotTbl[i].ProviderByteCntAboveMax = 0;
			}

			/* we allocate ConsumerCnt+ProviderCnt Knotelements. If this is more then */
			/* Knots we use KnotCnt (because this is the maximum we need.             */

			pCSRTComp->Knot.KnotElementCnt = pCSRTComp->Cons.ConsumerCnt
					+ pCSRTComp->Cfg.ProviderCnt + 1; /* one more for changephase handling */

			if (pCSRTComp->Knot.KnotElementCnt > pCSRTComp->Knot.KnotCnt)
				pCSRTComp->Knot.KnotElementCnt = pCSRTComp->Knot.KnotCnt;

			Size = sizeof(EDDS_CSRT_KNOT_ELEMENT_TYPE)
					* pCSRTComp->Knot.KnotElementCnt;

			EDDS_ALLOC_LOCAL_MEM(
					((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Knot.pKnotElementTbl),
					Size);

			if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotElementTbl,
					LSA_NULL)) {
				Status = EDD_STS_ERR_RESOURCE;
			} else {
				EDDS_MEMSET_LOCAL(pCSRTComp->Knot.pKnotElementTbl, 0, Size);

				pCSRTComp->Knot.KnotElementFree.BottomIdx = 0;
				pCSRTComp->Knot.KnotElementFree.TopIdx =
						(LSA_UINT16) (pCSRTComp->Knot.KnotElementCnt - 1);

				pCSRTComp->Knot.KnotElementUsed.BottomIdx =
						EDDS_SRT_CHAIN_IDX_END; /* no knot element used yet */
				pCSRTComp->Knot.KnotElementUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END; /* no knot element used yet */

				/* initialize send and check-lists */
				for (i = 0; i < pCSRTComp->Knot.KnotElementCnt; i++) {
					pCSRTComp->Knot.pKnotElementTbl[i].InUse = LSA_FALSE;

					pCSRTComp->Knot.pKnotElementTbl[i].UsedFreeLink.Next = i
							+ 1;

					if (i == 0)
						pCSRTComp->Knot.pKnotElementTbl[i].UsedFreeLink.Prev =
								EDDS_SRT_CHAIN_IDX_END;
					else
						pCSRTComp->Knot.pKnotElementTbl[i].UsedFreeLink.Prev = i
								- 1;

					pCSRTComp->Knot.pKnotElementTbl[i].ConsumerCheckList.BottomIdx =
							EDDS_SRT_CHAIN_IDX_END;
					pCSRTComp->Knot.pKnotElementTbl[i].ConsumerCheckList.TopIdx =
							EDDS_SRT_CHAIN_IDX_END;
					pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListXRT.BottomIdx =
							EDDS_SRT_CHAIN_IDX_END;
					pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListXRT.TopIdx =
							EDDS_SRT_CHAIN_IDX_END;
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
					pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
					pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListUDP.TopIdx = EDDS_SRT_CHAIN_IDX_END;
#endif
				}

				/* last chain element */
				pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.KnotElementCnt
						- 1].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
			}
		}
	}

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

	/*----------------------------------------------------------------------------*/
	/* Alloc internal RQBs for Timer                                              */
	/*----------------------------------------------------------------------------*/

	if ( Status == EDD_STS_OK)
	{
		pCSRTComp->ProvTimerRQB.pRQB = EDDS_AllocInternalRQB();

		if ( LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->ProvTimerRQB.pRQB,LSA_NULL) )
		{
			Status = EDD_STS_ERR_RESOURCE;
		}
		else EDDS_SetupInternalRQBParams( &pCSRTComp->ProvTimerRQB,
				EDDS_SRV_DEV_TRIGGER,
				0, /* dont care  */
				pDDB,
				EDDS_TRIGGER_PROVIDER_CYCLE,
				0);
	}

	if ( Status == EDD_STS_OK)
	{
		pCSRTComp->ConsTimerRQB.pRQB = EDDS_AllocInternalRQB();

		if ( LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->ConsTimerRQB.pRQB,LSA_NULL) )
		{
			Status = EDD_STS_ERR_RESOURCE;
		}
		else EDDS_SetupInternalRQBParams( &pCSRTComp->ConsTimerRQB,
				EDDS_SRV_DEV_TRIGGER,
				0, /* dont care */
				pDDB,
				EDDS_TRIGGER_CONSUMER_CYCLE,0);
	}

	/*----------------------------------------------------------------------------*/
	/* alloc internal RQB  for triggering a provider indication check.            */
	/*----------------------------------------------------------------------------*/

	if ( Status == EDD_STS_OK)
	{
		pCSRTComp->ProvIndTriggerRQB.pRQB = EDDS_AllocInternalRQB();

		if ( LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->ProvIndTriggerRQB.pRQB,LSA_NULL) )
		{
			Status = EDD_STS_ERR_RESOURCE;
		}
		else EDDS_SetupInternalRQBParams( &pCSRTComp->ProvIndTriggerRQB,
				EDDS_SRV_DEV_TRIGGER,
				0, /* dont care */
				pDDB,
				EDDS_TRIGGER_PROV_INDICATION,0);
	}
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

	/*----------------------------------------------------------------------------*/
	/* alloc internal RQB  for triggering a consumer indication check.            */
	/*----------------------------------------------------------------------------*/

	if (Status == EDD_STS_OK) {
		pCSRTComp->ConsIndTriggerRQB.pRQB = EDDS_AllocInternalRQB();

		if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->ConsIndTriggerRQB.pRQB,
				LSA_NULL)) {
			Status = EDD_STS_ERR_RESOURCE;
		} else
			EDDS_SetupInternalRQBParams(&pCSRTComp->ConsIndTriggerRQB,
			EDDS_SRV_DEV_TRIGGER, 0, /* dont care */
			pDDB,
			EDDS_TRIGGER_CONS_INDICATION, 0);
	}

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	/*----------------------------------------------------------------------------*/
	/* Alloc Timer                                                                */
	/*----------------------------------------------------------------------------*/

	if ( Status == EDD_STS_OK)
	{

#ifndef EDDS_CFG_DBG_LONG_PROV_TIMER

		EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
				"--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
				LSA_TIMER_TYPE_CYCLIC,
				LSA_TIME_BASE_1MS);

		EDDS_ALLOC_TIMER(&RetVal,
				(LSA_TIMER_ID_TYPE EDDS_LOCAL_MEM_ATTR *)&pCSRTComp->ProvCycleTimer.TimerID,
				LSA_TIMER_TYPE_CYCLIC,
				LSA_TIME_BASE_1MS);
#else
		EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
				"--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
				LSA_TIMER_TYPE_CYCLIC,
				LSA_TIME_BASE_1S);

		EDDS_ALLOC_TIMER(&RetVal,
				(LSA_TIMER_ID_TYPE EDDS_LOCAL_MEM_ATTR *)&pCSRTComp->ProvCycleTimer.TimerID,
				LSA_TIMER_TYPE_CYCLIC,
				LSA_TIME_BASE_1S);
#endif

		if ( RetVal == LSA_RET_OK)
		{

			EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
					"<-- Calling EDDS_ALLOC_TIMER success. TimerID: %d.",
					pCSRTComp->ProvCycleTimer.TimerID);

			EDDS_TimerSet(pCSRTComp->ProvCycleTimer.TimerID,
					&pCSRTComp->ProvTimerRQB,
					EDDS_CSRTProviderTimerCbf,
					&pCSRTComp->ProvCycleTimer.EDDSTimerID);

			pCSRTComp->ProvCycleTimer.InProgress = LSA_FALSE;
			pCSRTComp->ProvCycleTimer.InUse = LSA_TRUE;
		}
		else
		{
			EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<-- Calling EDDS_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
			Status = EDD_STS_ERR_NO_TIMER;
		}

	}

	if ( Status == EDD_STS_OK)
	{

		EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
				"--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
				LSA_TIMER_TYPE_CYCLIC,
				LSA_TIME_BASE_1MS);

		EDDS_ALLOC_TIMER(&RetVal,
				(LSA_TIMER_ID_TYPE EDDS_LOCAL_MEM_ATTR *)&pCSRTComp->ConsCycleTimer.TimerID,
				LSA_TIMER_TYPE_CYCLIC,
				LSA_TIME_BASE_1MS);

		if ( RetVal == LSA_RET_OK)
		{

			EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
					"<-- Calling EDDS_ALLOC_TIMER success. TimerID: %d.",
					pCSRTComp->ConsCycleTimer.TimerID);

			EDDS_TimerSet(pCSRTComp->ConsCycleTimer.TimerID,
					&pCSRTComp->ConsTimerRQB,
					EDDS_CSRTConsumerTimerCbf,
					&pCSRTComp->ConsCycleTimer.EDDSTimerID);

			pCSRTComp->ConsCycleTimer.InProgress = LSA_FALSE;
			pCSRTComp->ConsCycleTimer.InUse = LSA_TRUE;

		}
		else
		{
			EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<-- Calling EDDS_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
			Status = EDD_STS_ERR_NO_TIMER;
		}

	}

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:SRTInitCSRTManagement(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pCSRTComp,pCSRTIni,pMgmInfoBlock,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTInitComponent                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_COMP_SRT_INI        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_CMP_SRT_INI_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_COMP_SRT_INI           +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDDS_STS_ERR_RESOURCES          +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will initialize the SRT-Component management+*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be allocated and put into the DDB structure.         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the component is     +*/
/*+               already initialized for this device.                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//check pDDB
//lint -esym(613,pRQB)
//JB 18/11/2014 checked where called
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTInitComponent(EDD_UPPER_RQB_PTR_TYPE pRQB) {

	LSA_RESULT Status;
	EDDS_UPPER_CMP_SRT_INI_PTR_TYPE pRqbCMP;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB;
	EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pSRTComp;
	LSA_UINT16 RetVal;

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTInitComponent(pRQB: 0x%X)", pRQB);

	if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL)) {
		EDDS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,
				"EDDS_RTInitComponent: RQB->pParam is NULL!");
		EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
		EDDS_RQB_ERROR(pRQB);
	} else {

		pRqbCMP = (EDDS_UPPER_CMP_SRT_INI_PTR_TYPE) pRQB->pParam;

		/*-----------------------------------------------------------------------*/
		/* Get DDB.                                                              */
		/*-----------------------------------------------------------------------*/

		Status = EDDS_GetDDB(pRqbCMP->hDDB, &pDDB);

		if (Status == EDD_STS_OK) {
			/* check if not already initialized. If so report error */

			if (!LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL)) {
				EDDS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,
						"EDDS_RTInitComponent: SRT already initialized!");
				Status = EDD_STS_ERR_SEQUENCE;
			} else {

				EDDS_ALLOC_LOCAL_FAST_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pSRTComp),
						sizeof(EDDS_DDB_COMP_SRT_TYPE));

				if (LSA_HOST_PTR_ARE_EQUAL(pSRTComp, LSA_NULL)) {
					EDDS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,
							"EDDS_RTInitComponent: Allocating SRT-memory failed!");
					Status = EDD_STS_ERR_RESOURCE;
				} else {
					/* init with 0 */

					EDDS_MEMSET_LOCAL(pSRTComp, 0,
							sizeof(EDDS_DDB_COMP_SRT_TYPE));

					pDDB->pSRT = pSRTComp;

					/*----------------------------------------------------*/
					/* Setup cSRT parts..                                 */
					/*----------------------------------------------------*/

					Status = SRTInitCSRTManagement(pDDB, &pSRTComp->CSRT,
							&pRqbCMP->CSRT);

					if (Status != EDD_STS_OK) {

						EDDS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,
								"EDDS_RTInitComponent: SRT initalization failed! (0x%X)",
								Status);

						SRTFreeCSRTManagement(pDDB);

						EDDS_FREE_LOCAL_FAST_MEM(&RetVal, pSRTComp);
						EDDS_FREE_CHECK(RetVal);

						pDDB->pSRT = LSA_NULL;

					}

				}

			}
		} else {
			EDDS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTInitComponent: Invalid hDDB (0x%X)!",
					pRqbCMP->hDDB);
			Status = EDD_STS_ERR_PARAM; /* if DDB not found  or invalid params */
		}

		EDDS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,
				"<-- Request: EDDS_SRV_DEV_COMP_SRT_INI finished. Response: 0x%X",
				Status);

		EDDS_RQB_SET_STATUS(pRQB, Status);
		EDDS_CallCbf(pRqbCMP->Cbf, pRQB);
	}

	EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTInitComponent()");

}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTRelComponent                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_COMP_SRT_REL       +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_CMP_SRT_REL_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_COMP_NRT_REL           +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDDS_STS_ERR_RESOURCES          +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will release the SRT-Component management   +*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be freed.                                            +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the any channel      +*/
/*+               uses this component and the device is still setup         +*/
/*+               (device-shutdown has to be called first!)                 +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//check pDDB
//lint -esym(613,pRQB)
//JB 18/11/2014 checked where called
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTRelComponent(EDD_UPPER_RQB_PTR_TYPE pRQB) {

	LSA_RESULT Status;
	EDDS_UPPER_CMP_SRT_REL_PTR_TYPE pRqbCMP;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB;
	LSA_UINT16 RetVal;

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTRelComponent(pRQB: 0x%X)", pRQB);

	if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL)) {
		EDDS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,
				"EDDS_RTRelComponent: RQB->pParam is NULL!");
		EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
		EDDS_RQB_ERROR(pRQB);
	} else {
		pRqbCMP = (EDDS_UPPER_CMP_SRT_REL_PTR_TYPE) pRQB->pParam;

		Status = EDDS_GetDDB(pRqbCMP->hDDB, &pDDB);

		if (Status == EDD_STS_OK) {
			/* check if initialized. If not return with ok */

			if (!LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL)) {

				/* check for open channels using this component. */
				/* if open channels, return with error.          */
				/* If hardware is setup (i.e. no device-shutdown */
				/* called yet) we return with error.             */

				if ((!pDDB->pGlob->HWIsSetup)
						&& (!pDDB->pSRT->CSRT.HandleCnt)) {
					/* Free all CSRT and ASRT resources */

					SRTFreeCSRTManagement(pDDB);

					EDDS_FREE_LOCAL_FAST_MEM(&RetVal, pDDB->pSRT);
					EDDS_FREE_CHECK(RetVal);

					pDDB->pSRT = LSA_NULL;

				} else {
					EDDS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,
							"EDDS_RTRelComponent: Handles still open and/or hardware still setup!");
					Status = EDD_STS_ERR_SEQUENCE;
				}

			} else
				Status = EDD_STS_OK;
		} else {
			EDDS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
					"EDDS_RTRelComponent: Invalid hDDB (0x%X)!", pRqbCMP->hDDB);
			Status = EDD_STS_ERR_PARAM; /* if DDB not found */
		}

		EDDS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,
				"<-- Request: EDDS_SRV_DEV_COMP_SRT_REL finished. Response: 0x%X",
				Status);

		EDDS_RQB_SET_STATUS(pRQB, Status);
		EDDS_CallCbf(pRqbCMP->Cbf, pRQB);
	}

	EDDS_CRT_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTRelComponent()");

}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTOpenChannelCyclic                    +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+                             EDDS_UPPER_CDB_CSRT_PTR_TYPE pCDB           +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel (<= EDDS_CFG_MAX_CHANNELS)        +*/
/*+  pCDB       : Pointer to filled ChannelDescriptionBlock parameters      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_CHANNEL_USE aSRT already in used by a channel +*/
/*+               EDDS_STS_ERR_RESOURCES  Alloc failed                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Opens Channel for cyclic SRT. The Handle will specify     +*/
/*+               the channel and pDDB the Device. pCDB has all SRT         +*/
/*+               parameters to use for this channel                        +*/
/*+                                                                         +*/
/*+               It is not checked if the handle is already open!          +*/
/*+               The SRT-Management for the device has to be setup!        +*/
/*+                                                                         +*/
/*+               Only on Channel can use cyclic SRT!                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTOpenChannelCyclic(
		EDDS_LOCAL_HDB_PTR_TYPE pHDB) {
	LSA_RESULT Status;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;
	LSA_HANDLE_TYPE Handle = pHDB->Handle;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	LSA_USER_ID_TYPE Dummy;
	LSA_UINT16 RetVal;
	LSA_UINT16 TimeMS;
#endif

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTOpenChannelCyclic(pDDB: 0x%X, pHDB: 0x%X, Hdl: %d)",
			pDDB, pHDB, Handle);

	pCSRTComp = &pDDB->pSRT->CSRT;
	Status = EDD_STS_OK;

	/*---------------------------------------------------------------------------*/
	/* we support only one cSRT-Channel for each device !                        */
	/*---------------------------------------------------------------------------*/

	if (pCSRTComp->HandleCnt)
		Status = EDD_STS_ERR_CHANNEL_USE;
	else {
		pCSRTComp->HandleCnt++; /* cnt of currently open handles  */

#ifdef EDDS_MESSAGE
		/*--------------------------------------------------------------------*/
		/* Start provider-timer here because this cycle will be needed for    */
		/* NRT for some BUG-Workarounds for CPU317 and for CycleStamps.       */
		/*--------------------------------------------------------------------*/
#endif

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		if ( Status == EDD_STS_OK)
		{
			Dummy.uvar32 = pCSRTComp->ProvCycleTimer.EDDSTimerID;

			/* note that we can only use a multiple of ms. this was checked */
			/* on init!! TimeMS have to be > 0!!                            */

			TimeMS = (LSA_UINT16) (pCSRTComp->ProvCycleTimer.BaseTimeNS / (LSA_UINT32)1000000UL);

			if (! TimeMS ) TimeMS = 1; /* just for sure */

			pCSRTComp->Prov.ProviderCycle.CycleCounter = 0; /* start with Counter 0 */
			pCSRTComp->ProvCycleTimer.InProgress = LSA_TRUE;

#ifndef EDDS_CFG_DBG_LONG_PROV_TIMER
			EDDS_START_TIMER(&RetVal,
					pCSRTComp->ProvCycleTimer.TimerID,
					Dummy,
					TimeMS);
#else
			EDDS_START_TIMER(&RetVal,
					pCSRTComp->ProvCycleTimer.TimerID,
					Dummy,
					10); /* 10 seconds */
#endif
			if ( RetVal != LSA_RET_ERR_PARAM )
			{
				pCSRTComp->ProvCycleTimer.InProgress = LSA_TRUE;
			}
			else
			{
				EDDS_FatalError(EDDS_FATAL_ERR_TIMER_START,
						EDDS_MODULE_ID,
						__LINE__);
			}
		}
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

	}

	//Handle = 0; /* not used yet. to prevent compiler warning */
	//LSA_UNUSED_ARG...

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTOpenChannelCyclic(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTCloseChannelCyclic                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel to be closed                      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE         Channel has pending requests +*/
/*+               EDD_STS_ERR_RESOURCE         Error on freeing memory      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an  cyclic SRT-Channel. if there are any pending   +*/
/*+               requests the channel can not be closed.                   +*/
/*+                                                                         +*/
/*+               This function is only called if the handle uses aSRT!     +*/
/*+               (not checked!)                                            +*/
/*+               The SRT-Management for the device has to be setup!        +*/
/*+                                                                         +*/
/*+               Note:  If EDD_STS_ERR_RESOURCE is returned the channel is +*/
/*+                      closed but we had an error while freeing memory.   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTCloseChannelCyclic(
		EDDS_LOCAL_HDB_PTR_TYPE pHDB) {
	LSA_RESULT Status;
	EDD_UPPER_RQB_PTR_TYPE pRQB;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	LSA_UINT16 RetVal;
#endif
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;
	LSA_HANDLE_TYPE Handle = pHDB->Handle;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTCloseChannelCyclic(pDDB: 0x%X, pHDB: 0x%X, Hdl: %d)",
			pDDB, pHDB, Handle);

	Status = EDD_STS_OK;

	pCSRTComp = &pDDB->pSRT->CSRT;

	/* ----------------------------------------------------------------------*/
	/* check for something in use.                                           */
	/* ----------------------------------------------------------------------*/

	if (pCSRTComp->Cons.ConsumerUsedCnt || pCSRTComp->Prov.ProviderUsedCnt) {
		Status = EDD_STS_ERR_SEQUENCE;
	} else {
		/* ------------------------------------------------------------------*/
		/* Cancel all Inidication RQBs                                       */
		/* ------------------------------------------------------------------*/

		while (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Indication.RqbQueue.pBottom,
				LSA_NULL)) {
			EDDS_RQB_REM_BLOCK_BOTTOM(pCSRTComp->Indication.RqbQueue.pBottom,
					pCSRTComp->Indication.RqbQueue.pTop, pRQB);

			if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) {
				EDDS_RequestFinish(pHDB, pRQB, EDD_STS_OK_CANCEL);
			}
		}

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		/*--------------------------------------------------------------------*/
		/* Stops Timer                                                        */
		/*--------------------------------------------------------------------*/

		if ( pCSRTComp->ProvCycleTimer.InProgress )
		{
			EDDS_STOP_TIMER(&RetVal,
					pCSRTComp->ProvCycleTimer.TimerID);

			pCSRTComp->ProvCycleTimer.InProgress = LSA_FALSE;
		}
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */


		pCSRTComp->HandleCnt--; /* cnt of currently open handles */

	}

	LSA_UNUSED_ARG(Handle);

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTCloseChannelCyclic(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTIndicationProvide                    +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Queues new inidication rqb for cyclic SRT. Checks if      +*/
/*+               indications pending to be done and, if so, search for     +*/
/*+               all pending indications and indicate them.                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTIndicationProvide(
		EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB) {

	EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE pInd;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTIndicationProvide(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
			pRQB, pDDB, pHDB);

	EDDS_ENTER_MIDDLE();

	/* Queue the new indication resource (the whole RQB!)                */

	EDDS_RQB_PUT_BLOCK_TOP(pDDB->pSRT->CSRT.Indication.RqbQueue.pBottom,
			pDDB->pSRT->CSRT.Indication.RqbQueue.pTop, pRQB);

	pInd = ((EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE) pRQB->pParam);
	pInd->Count = 0; /* setup Count with 0 */
	pInd->DebugInfo[0] = EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDS;
	pInd->DebugInfo[1] = 0;
	pInd->DebugInfo[2] = 0;
	pInd->DebugInfo[3] = 0;
	pInd->DebugInfo[4] = 0;
	pInd->DebugInfo[5] = 0;
	pInd->DebugInfo[6] = 0;
	pInd->DebugInfo[7] = 0;
	pInd->DebugInfo[8] = 0;
	pInd->DebugInfo[9] = 0;
	pInd->DebugInfo[10] = 0;
	pInd->DebugInfo[11] = 0;

	/* Now look if any indications are pending and search for them if so */
	/* and indicate them.                                                */
	/* First we look for Consumer indications                            */
	/* After this, if still indication resources present, we look for    */
	/* pending provider indications.                                     */

	EDDS_RTConsumerCheckIndication(pDDB);

	if (!(LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT->CSRT.Indication.RqbQueue.pBottom,
			LSA_NULL))) {
		EDDS_RTProviderCheckIndication(pDDB);
	}

	EDDS_EXIT_MIDDLE();

	LSA_UNUSED_ARG(pHDB);

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTIndicationProvide(), Status: 0x%X", EDD_STS_OK);

	return (EDD_STS_OK );

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTGetAPDUStatus                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get APDU Status for spezified provider or consumer        +*/
/*+               For a consumer there may be no status present, because no +*/
/*+               frame was received yet.                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB,pParam)
//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTGetAPDUStatus(
		EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB) {

	LSA_RESULT Status;
	EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE pRqbParam;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	EDDS_CSRT_PROVIDER_TYPE_PTR pProv;
	EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
	EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTGetAPDUStatus(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
			pRQB, pDDB, pHDB);

	Status      = EDD_STS_ERR_PARAM;
	pRqbParam   = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE) pRQB->pParam;
	pCSRTComp   = &pDDB->pSRT->CSRT;

	switch (pRqbParam->Type) {
	case EDD_CSRT_TYPE_PROVIDER:
		if (pRqbParam->ID < pCSRTComp->Cfg.ProviderCnt) {
			pProv = &pCSRTComp->Prov.pProvTbl[pRqbParam->ID];
			if ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED != pProv->Type) {
				pRqbParam->Present = LSA_TRUE;

				if (pProv->Type == EDDS_CSRT_PROVIDER_TYPE_XRT)
					pRqbParam->APDUStatus.CycleCnt =
							(LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter)
									<< pCSRTComp->CycleStampAdjustShift);
				else
					pRqbParam->APDUStatus.CycleCnt =
							(LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter)
									<< (pCSRTComp->CycleStampAdjustShift - 5));

				pRqbParam->APDUStatus.DataStatus =
						(((pProv->DataStatus
								| pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus)
								& EDD_CSRT_DSTAT_WIN_MASK)
								| (pProv->DataStatus
										& pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus));
				pRqbParam->APDUStatus.TransferStatus = 0;
				Status = EDD_STS_OK;
			}
		}
		break;
	case EDD_CSRT_TYPE_CONSUMER:
		if (pRqbParam->ID < pCSRTComp->Cons.ConsumerCnt) {

			pCons = &pCSRTComp->Cons.pConsTbl[pRqbParam->ID];
			pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[pRqbParam->ID];

			if ( EDDS_CSRT_CONSUMER_TYPE_NOT_USED != pCons->Type) {
				/* Only if we already received a valid Consumer frame */
				/* State is set within ISR-Handler on receiving the   */
				/* first valid frame.                                 */
				if (pConsFrame->State >= EDDS_SRT_CONS_STATE_ON_3) {
					pRqbParam->Present = LSA_TRUE;

					EDDS_LOCK();

#if 0
					if( (pCons->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_XRT )
					{
						pRqbParam->APDUStatus.CycleCnt = pConsFrame->StampAddr.xrt.CycleCntStamp << pCSRTComp->CycleStampAdjustShift;
					} else {
						/* udp */
						pRqbParam->APDUStatus.CycleCnt = (LSA_UINT16) (pConsFrame->StampAddr.udp.CycleCntStamp << (pCSRTComp->CycleStampAdjustShift-5)); /* 1ms based */
					}
#else
					pRqbParam->APDUStatus.CycleCnt = pConsFrame->CycleCounter;
#endif
					pRqbParam->APDUStatus.DataStatus = pConsFrame->DataStatus;
					EDDS_UNLOCK();

					pRqbParam->APDUStatus.TransferStatus = 0;
					Status = EDD_STS_OK;
				} else /* no valid frame received yet */
				{
					pRqbParam->Present = LSA_FALSE;
					pRqbParam->APDUStatus.CycleCnt = 0;
					pRqbParam->APDUStatus.DataStatus = 0;
					pRqbParam->APDUStatus.TransferStatus = 0;
					Status = EDD_STS_OK;
				}
			}
		}
		break;
	default:
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,
				"EDDS_RTGetAPDUStatus(): Invalid Type (0x%X)!",
				pRqbParam->Type);
		break;
	}

	LSA_UNUSED_ARG(pHDB);

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTGetAPDUStatus(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pRQB,pHDB,pParam)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTSendclockChange                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles EDD_SRV_SENDCLOCK_CHANGE request.                 +*/
/*+               We cant change sendclock so we only check if the desired  +*/
/*+               sendclock matches the current one.                        +*/
/*+               Changing the limits not supported and not checked.        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB,pRqbParam)
//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB - pRqbParam taken from pDDB (taken from pHDB)
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTSendclockChange(
		EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB) {

	LSA_RESULT Status;
	EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE pRqbParam;
	EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
	EDDS_LOCAL_DDB_PTR_TYPE pDDB = pHDB->pDDB;

	EDDS_CRT_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTSendclockChange(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
			pRQB, pDDB, pHDB);

	Status = EDD_STS_OK;
	pRqbParam = (EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE) pRQB->pParam;
	pCSRTComp = &pDDB->pSRT->CSRT;

	if ((LSA_HOST_PTR_ARE_EQUAL(pRqbParam, LSA_NULL))
			|| ((pCSRTComp->Cfg.CycleBaseFactor != pRqbParam->CycleBaseFactor)
					&& (pRqbParam->CycleBaseFactor != 0))) {
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
				"EDDS_RTSendclockChange: Invalid parameter CycleBaseFactor %d",
				pRqbParam->CycleBaseFactor);
		Status = EDD_STS_ERR_PARAM;
	}

	LSA_UNUSED_ARG(pHDB);

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTSendclockChange(), Status: 0x%X", Status);

	return (Status);

}
//lint +esym(613,pRQB,pHDB,pRqbParam)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTRequestCyclic                        +*/
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
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles all cSRT-Requests with OPCODE-Type  +*/
/*+               EDD_OPC_REQUEST and service EDDS_SRV_SRT_xxxx.            +*/
/*+                                                                         +*/
/*+               If we have a parameter error we will set EDD_STS_ERR_PARAM+*/
/*+               and call the handle-callbackfunction and abort the request+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SERVICE is set if we get an invalid Service.  +*/
/*+               EDD_STS_ERR_CHANNEL_USE is set if the service is invalid  +*/
/*+               for this channel (i.e. not used by this channel)          +*/
/*+                                                                         +*/
/*+               If the request is valid, it is queued or executed.        +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                     SRT-Component already initialized (not checked)     +*/
/*+                     Channel uses this component (not checked)           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTRequestCyclic(EDD_UPPER_RQB_PTR_TYPE pRQB,
		EDDS_LOCAL_HDB_PTR_TYPE pHDB) {

	LSA_RESULT Status;
	LSA_BOOL Indicate;

	EDDS_CRT_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
			"IN :EDDS_RTRequestCyclic(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

	Indicate = LSA_TRUE;
	Status = EDD_STS_OK;

	/*---------------------------------------------------------------------------*/
	/* Alle requests need an param-ptr, so check it.                             */
	/*---------------------------------------------------------------------------*/

	if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL)) {
		EDDS_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,
				">>> Request: Service: 0x%X, RQB->pParam is NULL",
				EDD_RQB_GET_SERVICE(pRQB));
		Status = EDD_STS_ERR_PARAM;
	} else {
		switch (EDD_RQB_GET_SERVICE(pRQB)) {
		/*-------------------------------------------------------------------*/
		/* Provide indication resources                                      */
		/* On success the rqb will stay within edds!                         */
		/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_IND_PROVIDE :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_IND_PROVIDE (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTIndicationProvide(pRQB, pHDB);
			if (Status == EDD_STS_OK)
				Indicate = LSA_FALSE;
			break;
			/*-------------------------------------------------------------------*/
			/* set provider state                                                */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_PROVIDER_SET_STATE :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_PROVIDER_SET_STATE (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTProviderSetState(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* add a provider                                                    */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_PROVIDER_ADD :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_PROVIDER_ADD (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTProviderADD(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* control provider                                                  */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_PROVIDER_CONTROL :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_PROVIDER_CONTROL (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTProviderControl(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* remove  provider                                                  */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_PROVIDER_REMOVE :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_PROVIDER_REMOVE (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTProviderRemove(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* add consumer                                                      */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_CONSUMER_ADD :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_CONSUMER_ADD (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTConsumerAdd(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* control consumer                                                  */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_CONSUMER_CONTROL :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_CONSUMER_CONTROL (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTConsumerControl(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* remove consumer                                                   */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_CONSUMER_REMOVE :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_CONSUMER_REMOVE (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTConsumerRemove(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* provider change phase                                             */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_PROVIDER_CHANGE_PHASE :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_PROVIDER_CHANGE_PHASE (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTProviderChangePhase(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* get apdu-status                                                   */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_GET_APDU_STATUS :
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
					">>> Request: EDD_SRV_SRT_GET_APDU_STATUS (pRQB: 0x%X, Handle: 0x%X)",
					pRQB, EDD_RQB_GET_HANDLE(pRQB));
			Status = EDDS_RTGetAPDUStatus(pRQB, pHDB);
			break;
			/*-------------------------------------------------------------------*/
			/* Unknown/Unsupported Request                                       */
			/*-------------------------------------------------------------------*/
		case EDD_SRV_SRT_PROVIDER_XCHANGE_BUF :
		case EDD_SRV_SRT_CONSUMER_XCHANGE_BUF :
		default:
			EDDS_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,
					">>> EDDS_RTRequestCyclic: Invalid Service (pRQB: 0x%X, Service: 0x%X)",
					pRQB, EDD_RQB_GET_SERVICE(pRQB));
			Status = EDD_STS_ERR_SERVICE;
			break;

		} /* switch */
	} /* if */

	if (Indicate) {
		EDDS_RequestFinish(pHDB, pRQB, Status);
	}

	EDDS_CRT_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
			"OUT:EDDS_RTRequestCyclic(), Status: 0x%X", EDD_STS_OK);

}
//lint +esym(613,pRQB,pHDB)

#endif

/*****************************************************************************/
/*  end of file SRT_USR.C                                                    */
/*****************************************************************************/

