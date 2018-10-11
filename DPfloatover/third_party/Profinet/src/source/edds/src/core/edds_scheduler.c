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
/*  F i l e               &F: edds_scheduler.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS Event Scheduler                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who      What                                                */
#ifdef EDDS_MESSAGE
/*  2014-08-13  am2219   initial version.                                    */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID 12
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_ISR */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

#include "edds_int.h"            /* internal header */
/*===========================================================================*/
/* plausibility tests for new EDDS_CFG_CYCLIC_SCHEDULING_MODE                */
/*===========================================================================*/

#if defined(EDDS_CFG_ISR_MODE_STANDARD)
    #error EDDS_CFG_ISR_MODE_STANDARD not allowed with EDDS_CFG_CYCLIC_SCHEDULING_MODE!
#endif /* defined(EDDS_CFG_ISR_MODE_STANDARD) */

#if defined(EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD)
    #error EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD not allowed with EDDS_CFG_CYCLIC_SCHEDULING_MODE!
#endif /* defined(EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) */

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

//lint -esym(550,framesFinished)
//JB 10/12/2014 tmp (debugging)
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TransmitLastCycleFinish(EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_UINT framesFinished = 0;

    /* always stop timeout - timeout is not used anymore! */
    while ( EDD_STS_OK == EDDS_TransmitSts(pDDB,LSA_TRUE))
    {
        ++framesFinished;
    }
}
//lint +esym(550,framesFinished)

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_ReceiveLastCycleFinish(EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_RESULT                      Result;
    EDD_UPPER_MEM_PTR_TYPE          pRxBuffer;
    LSA_UINT32                      RxLen;
    LSA_UINT32                      PortID;
    LSA_INT32                       maxRxLenInCycle;

    /*-------------------------------------------------------------------*/
    /* check for received frames                                         */
    /*-------------------------------------------------------------------*/

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
                        "==> EDDS_LL_RECV(pDDB: 0x%X)",pDDB);

    /* @fixme am2219 Use mbaud rate (100MBit for PNIO or unlimited (limited by descr-rx-frames) */
    maxRxLenInCycle = 125000;

    while ( EDD_STS_OK_NO_DATA != (Result = EDDS_LL_RECV(pDDB,
                                   &pRxBuffer,
                                   &RxLen,
                                   &PortID))
            )
    {
        EDDS_ISR_TRACE_04(0,LSA_TRACE_LEVEL_NOTE,
         "<== EDDS_LL_RECV(pRXBuffer: 0x%X, RxLen: 0x%X, PortID: %d), Result: 0x%X",
         pRxBuffer,
         RxLen,
         PortID,
         Result);

        /* note: it's possible, that one frame will be processed, that is from "next" cycle.
         *       the theoretically maximum of more bytes processed in cycle is : MaxFrameSize - (MinimumFrameSize -1)
         *       ==> 100MBit: 1457 Byte (With 1 VLAN Tag: 1415 byte data; w/o VLAN tag 1419 byte data).
         *
         */
        maxRxLenInCycle -= (7+1);   // preamble + SFD
        maxRxLenInCycle -= (LSA_INT32) RxLen;   // frame length
        maxRxLenInCycle -= (4+12);  // CRC + IFG

        /*---------------------------------------------------------------*/
        /* if data was present we only continue if the data is           */
        /* errorfree. data with error will be ignored.                   */
        /*---------------------------------------------------------------*/

        if ( Result == EDD_STS_OK )
        {
            EDDS_HandleRxFrame(pDDB,&pRxBuffer,RxLen,PortID);

        } /* if (Result == EDD_STS_OK)..*/

        /*-------------------------------------------------------------- */
        /* provide a new buffer to the controller                        */
        /*-------------------------------------------------------------- */

        EDDS_ISR_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
                            "==> EDDS_LL_RECV_PROVIDE(pDDB: 0x%X,pRXBuffer: 0x%X)",
                            pDDB,
                            pRxBuffer);

        if ( EDDS_LL_RECV_PROVIDE(pDDB,pRxBuffer) != EDD_STS_OK )
        {
            EDDS_FatalError( EDDS_FATAL_ERR_ISR,
                             EDDS_MODULE_ID,
                             __LINE__);
        }

        EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,
                            "<== EDDS_LL_RECV_PROVIDE(), Result: EDD_STS_OK");

        #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
        /*-------------------------------------------------------------- */
        /* Rx Overload check.                                            */
        /* If we received too much frames we turn off the receiver. It   */
        /* will be reenabled after a time period. This is to prevent     */
        /* the CPU from heavy network traffic causing denial of service  */
        /*-------------------------------------------------------------- */

        if(EDDS_LL_AVAILABLE(pDDB,disableRX))
        {
            pGlob  = pDDB->pGlob;

            if ( ! pGlob->RxOverload.RxOFF ) /* if not already OFF */
            {
                EDDS_INTERNAL_REQUEST_PTR_TYPE  pRQB;

                if ( ++pGlob->RxOverload.RxCnt >= EDDS_CFG_RX_OVERLOAD_RX_LIMIT )
                {
                    pGlob->RxOverload.RxOFF  = LSA_TRUE;
                    pGlob->RxOverload.Status = EDD_RX_OVERLOAD;
                    pGlob->Errors.RxOverloadCnt++;

                    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                          "EDDS_InterruptRx: Rx-Overload detected. Turning OFF receiver (pDDB: 0x%X)",
                                          pDDB);

                    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
                                        "==> EDDS_LL_RX_OVERLOAD_RX_OFF(pDDB: 0x%X)",
                                        pDDB);

                    EDDS_LL_RX_OVERLOAD_RX_OFF(pDDB);

                    EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,
                                        "<== EDDS_LL_RX_OVERLOAD_RX_OFF()");

                    /* three stages of OFF time. */
                    switch ( pGlob->RxOverload.OFFLevel )
                    {
                        case 0:
                            pGlob->RxOverload.OFFCnt = EDDS_CFG_RX_OVERLOAD_RXOFF_1+1; /* +1 jitter */
                            pGlob->RxOverload.OFFLevel = 1;
                            break;
                        case 1:
                            pGlob->RxOverload.OFFCnt = EDDS_CFG_RX_OVERLOAD_RXOFF_2+1; /* +1 jitter */
                            pGlob->RxOverload.OFFLevel = 2;
                            break;
                        default:
                            pGlob->RxOverload.OFFCnt = EDDS_CFG_RX_OVERLOAD_RXOFF_3+1; /* +1 jitter */
                            break;
                    }

                    pRQB = &pDDB->pGlob->RQBInt_Rx_OverEvent;

                    if ( ! pRQB->InUse )
                    {
                        /* Note that the RQB is already filled up in startup!     */
                        pRQB->InUse = LSA_TRUE;
                        EDDS_DO_INT_REQUEST_RX_OVERLOAD(pRQB->pRQB);
                    }
                }
            }
        }
        #endif

        EDDS_ISR_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,
                            "==> EDDS_LL_RECV(pDDB: 0x%X,&pRXBuffer: 0x%X, &RxLen: 0x%X)",
                            pDDB,
                            &pRxBuffer,
                            &RxLen);

        //if((7+1+59+4+12) >= maxRxLenInCycle)    /* less than minimal frame available */
        if((7+1+1518+4+12) > maxRxLenInCycle)    /* less than maximal frame available */
        {
            break;
        }
    }

    if ( Result == EDD_STS_OK_NO_DATA )
    {
        EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,
                            "<== EDDS_LL_RECV(), Result: EDD_STS_OK_NO_DATA");
    }
}

static LSA_VOID EDDS_HandleRecurringTask(EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    EDDS_LL_RECURRING_TASK(pDDB);
    EDDS_RequestPHYInterrupt(pDDB);
    //EDDS_LL_GET_STATS(pDDB,0,)
}

LSA_VOID EDD_UPPER_IN_FCT_ATTR edds_scheduler(EDDS_HANDLE const hDDB)
{
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;

    EDDS_ENTER_HIGH();

    if(EDD_STS_OK != EDDS_GetDDB(hDDB, &pDDB))
    {
        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                                    EDDS_MODULE_ID,
                                                    __LINE__);
    }

    EDDS_ISR_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN :edds_scheduler(pDDB: 0x%X)",pDDB);

    if(pDDB->pGlob->HWIsSetup)
    {

    EDDS_TransmitLastCycleFinish(pDDB);
    EDDS_ReceiveLastCycleFinish(pDDB);

#if defined(EDDS_CFG_SRT_INCLUDE)
    pDDB->pSRT->CSRT.schedulerCounter++;
    if (0 == (pDDB->pSRT->CSRT.schedulerCounter % pDDB->pSRT->CSRT.schedulerModuloFactor))
    {
        EDDS_CSRTProviderCycleRequest(pDDB);
        EDDS_CSRTConsumerCycleRequest(pDDB);
    }
#else
    EDDS_TransmitTrigger(pDDB); /* Trigger a transmission for this dev.*/
#endif /* !defiend(EDDS_CFG_SRT_INCLUDE) */

#if defined(EDDS_CFG_NRT_INCLUDE)
    EDDS_NRTRxRequestInterrupt(pDDB);
#endif

    EDDS_HandleRecurringTask(pDDB);
    }
    else
    {
    	EDDS_ISR_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"OUT :edds_scheduler(pDDB: 0x%X): waiting for setup finished.",pDDB);
    }

    EDDS_ISR_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT :edds_scheduler(pDDB: 0x%X)",pDDB);

    EDDS_EXIT_HIGH();
}

#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)  */


