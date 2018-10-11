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
/*  F i l e               &F: edds_tx.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Tx-handling functions                       */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  05.02.03    JS    Optional MACRO-Call on tx-timeout for debugging        */
/*  12.02.04    JS    added support for multiple send priorities (3)         */
/*  10.03.04    JS    added support for tasking-compiler V2.0                */
/*  05.04.04    JS    nested comments corrected                              */
/*  05.04.04    JS    changes to prevent compiler warnings                   */
/*  16.04.04    JS    always use different nrt-send prios/queues             */
/*  23.06.04    JS    added #ifdef EDDS_CFG_NRT_INCLUDE                      */
/*  21.01.05    JS    added Rx-Overload support                              */
/*  31.01.05    JS    LTRC support                                           */
/*  06.07.05    JS    added multiport support                                */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  15.12.06    JS    added  EDDS_CFG_ORG_PRIO_IS_HIGHEST (undocumented yet) */
/*                    (if defined, ORG-Prio NRT will be send prior of cyclic */
/*                     frames, if not, cyclic frames have highest priority)  */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  6
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SEQ */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_OPTIMIZE_TASKING
#if defined (TOOL_CHAIN_TASKING_TRICORE) && (C_COMPILER >= TSK_V20_0)
#pragma optimize 3
#else
#pragma optimize 2
#endif
#endif

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitTimeout                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called when a transmit timeout occured.  +*/
/*+               We reinit the tx-unit of the low-level device.            +*/
/*+               We cancel all sends in progress with "timeout"-error      +*/
/*+                                                                         +*/
/*+               After this a new send (EDDS_TransmitTrigger) is initiated +*/
/*+               if EDDS_CFG_SEND_TIMEOUT_FATAL is not defined. if defined, +*/
/*+               a fatal-error is signaled.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TransmitTimeout(EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    #ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_ENTER_HIGH();
    #endif

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitTimeout(pDDB: 0x%X)",
                           pDDB);

    pGlob =  pDDB->pGlob;

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                          "EDDS_TransmitTimeOUT:Transmit timeout occured!");

    pGlob->TxStats.TxTimeout++;                              /* statistics */
    pGlob->TxTimer.InProgress = LSA_FALSE;
    pGlob->RQBInt_Tx_Tout.InUse = LSA_FALSE;

    /* --------------------------------------------------------------------------*/
    /* for debugging purpose a MACRO can be called on timeout to dump registers  */
    /* --------------------------------------------------------------------------*/

    #ifdef EDDS_CFG_TIMEOUT_OCCURED_MACRO_CALL
    EDDS_TX_TIMEOUT_OCCURED(pDDB->hDDB);
    #endif

    /* --------------------------------------------------------------------------*/
    /* Reinit Transmit unit. after this the transmitter is idle and waits for    */
    /* new transmit requests.                                                    */
    /* --------------------------------------------------------------------------*/

    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_REINIT_TX(pDDB: 0x%X)",pDDB);

    EDDS_LL_REINIT_TX(pDDB);      /* we ignore errors here and always try a new  */  //lint !e534 JB 11/11/2014 see comment
    /* send. may cause further timeouts            */

    EDDS_LOWER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_REINIT_TX()");


    /* --------------------------------------------------------------------------*/
    /* Cancel all pending sends with EDD_STS_ERR_TIMEOUT.                        */
    /* --------------------------------------------------------------------------*/

    while ( pGlob->pTxInfo[pGlob->TxCheckIdx].InUse  )
    {

        /* -----------------------------------------------------------------*/
        /* Now call the component the sent frame was from. so the component */
        /* can finish the actions neccessary with the sending of the frame  */
        /* -----------------------------------------------------------------*/

        switch (pGlob->pTxInfo[pGlob->TxCheckIdx].TxCompID)
        {
                #ifdef EDDS_CFG_NRT_INCLUDE
            case EDDS_TXCOMPID_NRT_PRIO_0:
                pDDB->pGlob->TxAcycInProgCnt--;
                EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_0,EDD_STS_ERR_TIMEOUT);
                break;
            case EDDS_TXCOMPID_NRT_PRIO_1:
                pDDB->pGlob->TxAcycInProgCnt--;
                EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_1,EDD_STS_ERR_TIMEOUT);
                break;
            case EDDS_TXCOMPID_NRT_PRIO_2:
                pDDB->pGlob->TxAcycInProgCnt--;
                EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_2,EDD_STS_ERR_TIMEOUT);
                break;
            case EDDS_TXCOMPID_NRT_PRIO_3:
                pDDB->pGlob->TxAcycInProgCnt--;
                EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_3,EDD_STS_ERR_TIMEOUT);
                break;
                #endif

                #ifdef EDDS_CFG_SRT_INCLUDE
            case EDDS_TXCOMPID_SRT_Z_XRT:
				if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
				{
					EDDS_RTTxReadyCyclicXRT(pDDB,EDD_STS_ERR_TIMEOUT);
				}
				break;
                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            case EDDS_TXCOMPID_SRT_Z_UDP:
				if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_XRT_OVER_UDP)
				{
					EDDS_RTTxReadyCyclicUDP(pDDB,EDD_STS_ERR_TIMEOUT);
				}
                break;
                #endif

                #endif

            default:
                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                EDDS_MODULE_ID,
                                __LINE__);
                break;
        }


        pGlob->pTxInfo[pGlob->TxCheckIdx].InUse = LSA_FALSE;
        pGlob->TxInProgCnt--;

        /* -----------------------------------------------------------------*/
        /* advance to next entry.                                           */
        /* -----------------------------------------------------------------*/

        if ( pGlob->TxCheckIdx < (pGlob->TxBuffCnt-1) )
        {
            pGlob->TxCheckIdx++;
        }
        else
        {
            pGlob->TxCheckIdx = 0;
        }

    } /* while */

    #ifdef EDDS_CFG_SEND_TIMEOUT_FATAL
    EDDS_FatalError(EDDS_FATAL_ERR_TIMEOUT_SEND,
                    EDDS_MODULE_ID,
                    __LINE__); /* does not return!*/

    #else
    #ifndef EDDS_CFG_SEND_CYCLE_BASED
	if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SEND_CYCLE_BASED)
	{
		EDDS_TransmitTrigger(pDDB);

	    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
	                           "OUT:EDDS_TransmitTimeout()");
	}
    #endif
    #ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_EXIT_HIGH();
    #endif
    #endif
}
//lint +esym(613,pDDB)
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitSts                            +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_BOOL                       StopTimeout  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+  StopTimeout  : LSA_TRUE : Stop Timeouttimer if running                 +*/
/*+                 LSA_FALSE: Dont Stop Timeouttimer                       +*/
/*+                                                                         +*/
/*+  Result:        EDD_STS_OK              : frame sent finished           +*/
/*+                 EDD_STS_OK_NO_DATA      : no frame was sent             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function checks for finished transmit.  It have to be+*/
/*+               called after a transmit interrupt occured.                +*/
/*+               If a frame was transmitted, the corresponding component   +*/
/*+               (SRT,NRT etc) is called so it can do its finishing tasks  +*/
/*+               (e.g. inidication to user)                                +*/
/*+                                                                         +*/
/*+               This function should be called as long as it returns      +*/
/*+               EDD_STS_OK. After each call with EDD_STS_OK returnd,      +*/
/*+               EDDS_TransmitTrigger() should be called.                  +*/
/*+                                                                         +*/
/*+               Note that the timeout-timer is stopped if a frame was     +*/
/*+               sent and StopTimeout is set (should be set on first call) +*/
/*+               and restarted if no more frames are present but           +*/
/*+               we still wait for frames to be sent and the timeout timer +*/
/*+               is stopped.                                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_TransmitSts(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB,
    LSA_BOOL                        StopTimeout)
{
    LSA_RESULT                      Status;
	EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_USER_ID_TYPE                Dummy;
	LSA_UINT16                      RetVal;
#endif
	
    EDDS_CORE_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitSts(pDDB: 0x%X,Stop: %d)",
                           pDDB,
                           StopTimeout);

    pGlob =  pDDB->pGlob;

    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SEND_STS(pDDB: 0x%X)",pDDB);

    if ( (Status = EDDS_LL_SEND_STS(pDDB)) != EDD_STS_OK_NO_DATA )
    {
        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND_STS(). Status: 0x%X",Status);

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        /* ---------------------------------------------------------------------*/
        /* Stop timeout-timer because something was transmitted.                */
        /* ---------------------------------------------------------------------*/

        if ((   StopTimeout                 ) &&
            (   pGlob->TxTimer.InUse        ) &&
            (   pGlob->TxTimer.InProgress   ))
        {
            EDDS_STOP_TIMER(&RetVal,pGlob->TxTimer.TimerID);
            pGlob->TxTimer.InProgress = LSA_FALSE;
        }

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

        /* ---------------------------------------------------------------------*/
        /* check if there is a transmission underway (should always be true)    */
        /* ---------------------------------------------------------------------*/

        if ( pGlob->pTxInfo[pGlob->TxCheckIdx].InUse )
        {

            /* -----------------------------------------------------------------*/
            /* Now call the component the sent frame was from. so the component */
            /* can finish the actions neccessary with the sending of the frame  */
            /* -----------------------------------------------------------------*/

            switch (pGlob->pTxInfo[pGlob->TxCheckIdx].TxCompID)
            {
                    #ifdef EDDS_CFG_NRT_INCLUDE
                case EDDS_TXCOMPID_NRT_PRIO_0:
                    pDDB->pGlob->TxAcycInProgCnt--;
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_0,Status);
                    break;
                case EDDS_TXCOMPID_NRT_PRIO_1:
                    pDDB->pGlob->TxAcycInProgCnt--;
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_1,Status);
                    break;
                case EDDS_TXCOMPID_NRT_PRIO_2:
                    pDDB->pGlob->TxAcycInProgCnt--;
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_2,Status);
                    break;
                case EDDS_TXCOMPID_NRT_PRIO_3:
                    pDDB->pGlob->TxAcycInProgCnt--;
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_3,Status);
                    break;
                    #endif

                    #ifdef EDDS_CFG_SRT_INCLUDE
                case EDDS_TXCOMPID_SRT_Z_XRT:
                	if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
                	{
                		Status = EDDS_RTTxReadyCyclicXRT(pDDB,Status);
                	}else
                	{
                		Status = EDD_STS_ERR_NOT_SUPPORTED;
                	}
                    break;

                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                case EDDS_TXCOMPID_SRT_Z_UDP:
                    Status = EDDS_RTTxReadyCyclicUDP(pDDB,Status);
                    break;
                    #endif

                    #endif

                default:
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
                    break;
            }


            pGlob->pTxInfo[pGlob->TxCheckIdx].InUse = LSA_FALSE;
            pGlob->TxInProgCnt--;

            /* -----------------------------------------------------------------*/
            /* advance to next entry.                                           */
            /* -----------------------------------------------------------------*/

            if ( pGlob->TxCheckIdx < (pGlob->TxBuffCnt-1) )
            {
                pGlob->TxCheckIdx++;
            }
            else
            {
                pGlob->TxCheckIdx = 0;
            }

        }
        else pGlob->Errors.TxResource++;    /* should not occur  */

        Status = EDD_STS_OK;  //lint !e838 JB 02/12/2014 on purpose

    }
    else
    {
        EDDS_LOWER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND_STS(). Status: EDD_STS_OK_NO_DATA");

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        /* ---------------------------------------------------------------------*/
        /* Restart timeout-timer if we still awaiting something and the         */
        /* timeout timer is not started.                                        */
        /* (Note that this is in the else branch to avoid unneccessary restart  */
        /*  of the timer when done everytime, which will  be formally correct,  */
        /*  if further sends are ready.)                                        */
        /* ---------------------------------------------------------------------*/

        if ( pGlob->pTxInfo[pGlob->TxCheckIdx].InUse )
        {
            if ((   pGlob->TxTimer.InUse ) &&
                ( ! pGlob->TxTimer.InProgress ))
            {
                Dummy.uvar32 = pGlob->TxTimer.EDDSTimerID;
                EDDS_START_TIMER(&RetVal,
                                 pGlob->TxTimer.TimerID,
                                 Dummy,
                                 pGlob->TxToutCnt);

                if ( RetVal != LSA_RET_ERR_PARAM )
                {
                    pGlob->TxTimer.InProgress = LSA_TRUE;
                }

            }
        }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitSts(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pDDB)

#ifdef EDDS_CFG_ORG_PRIO_IS_HIGHEST
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitCheckForSend                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pPortID      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pTxCompID    +*/
/*+                             LSA_UINT32                     MaxLength    +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                 : Pointer to DDB                                  +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+  pPortID    : Address for LSA_UINT32 of PortID        (output)          +*/
/*+  pTxCompID  : Address for LSA_UINT32 of Component ID (input/output)     +*/
/*+             : Input:  ComponentID to start test for pending tx-requests +*/
/*+               Output: ComponentID a TxFrame was found if something pres.+*/
/*+                                                                         +*/
/*+               Typically the first call uses TXCOMPID_SRT_Z and further  +*/
/*+               calls dont change the returned value. So further calls    +*/
/*+               continue at the last component a valid frame was present. +*/
/*+                                                                         +*/
/*+  MaxLengthA : Max length of frame for NRT/aSRT-Frames                   +*/
/*+                                                                         +*/
/*+  Result     :   LSA_TRUE : Something was present for sending            +*/
/*+                 LSA_FALSE: Nothing was present for sending.             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function checks if a component has something to      +*/
/*+               send.                                                     +*/
/*+                                                                         +*/
/*+               This is done in an priorized order:                       +*/
/*+                                                                         +*/
/*+               Component checked:             pTxCompID:                 +*/
/*+                                                                         +*/
/*+               NRT Prio 3                     TXCOMPID_NRT_PRIO_3        +*/
/*+               zyclic  SRT                    TXCOMPID_SRT_Z_XRT         +*/
/*+               zyclic  SRT                    TXCOMPID_SRT_Z_UDP         +*/
/*+               NRT Prio 2                     TXCOMPID_NRT_PRIO_2        +*/
/*+               NRT Prio 1                     TXCOMPID_NRT_PRIO_1        +*/
/*+               NRT Prio 0                     TXCOMPID_NRT_PRIO_0        +*/
/*+                                                                         +*/
/*+               Check starts with the component (=Prio) given in          +*/
/*+               pTxCompID. On first call use TXCOMPID_NRT_PRIO_3,further  +*/
/*+               call within one sendcycle use the returned value, so      +*/
/*+               the function continues at the point of return.            +*/
/*+                                                                         +*/
/*+               e.g.:                                                     +*/
/*+                                                                         +*/
/*+               TxCompID = TXCOMPID_NRT_PRIO_3                            +*/
/*+                                                                         +*/
/*+               while (free send resources)                               +*/
/*+               {                                                         +*/
/*+                 while (EDDS_TransmitCheckForSend(....,&TxCompID))       +*/
/*+                 {                                                       +*/
/*+                   .. send the pBuffer returned                          +*/
/*+                 }                                                       +*/
/*+               }                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_TransmitCheckForSend(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pPortID,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pTxCompID,
    LSA_UINT32                                    MaxLengthAcyclic)
{
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitCheckForSend(pDDB: 0x%X,pTxCompID: 0x%X,MaxLen: %d)",
                           pDDB,
                           pTxCompID,
                           MaxLengthAcyclic);

    switch ( *pTxCompID )
    {
        case EDDS_TXCOMPID_NRT_PRIO_3:

            #ifdef EDDS_CFG_NRT_INCLUDE
            #if EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG != 0

            if (pDDB->pGlob->TxAcycInProgCnt  < EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE )
            {
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_3,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_3;
                    pDDB->pGlob->TxAcycInProgCnt++;
                    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                           "OUT:EDDS_TransmitCheckForSend(NRT_PRIO_3 send)");
                    return(LSA_TRUE); /* dirty but a litte bit faster */
                } /* else we fall through to next level .. */
            } /* else we fall through to next level .. */

            #endif
            #endif
            /* fall through */

            #ifdef EDDS_CFG_SRT_INCLUDE
        case  EDDS_TXCOMPID_SRT_Z_XRT:
        	if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
        	{
				if ( EDDS_RTTxQueryCyclicXRT(pDDB,pBuffer,pLength) == EDD_STS_OK )
				{
					*pPortID   = EDD_PORT_ID_AUTO;
					*pTxCompID = EDDS_TXCOMPID_SRT_Z_XRT;
					EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
										   "OUT:EDDS_TransmitCheckForSend(SRT_Z_XRT send)");
					return(LSA_TRUE); /* dirty but a litte bit faster */
				}
        	}
            /* fall through */

            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
        case  EDDS_TXCOMPID_SRT_Z_UDP:
        	if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_XRT_OVER_UDP)
        	{
				if ( EDDS_RTTxQueryCyclicUDP(pDDB,pBuffer,pLength) == EDD_STS_OK )
				{
					*pPortID   = EDD_PORT_ID_AUTO;
					*pTxCompID = EDDS_TXCOMPID_SRT_Z_UDP;
					EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
										   "OUT:EDDS_TransmitCheckForSend(SRT_Z_UDP send)");
					return(LSA_TRUE); /* dirty but a litte bit faster */
				}
        	}
            #endif
            #endif
            /* fall through */

            #ifdef EDDS_CFG_NRT_INCLUDE
        case EDDS_TXCOMPID_NRT_PRIO_2:
            if (pDDB->pGlob->TxAcycInProgCnt  < EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE )
            {

                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_2,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_2;
                    pDDB->pGlob->TxAcycInProgCnt++;
                    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                           "OUT:EDDS_TransmitCheckForSend(NRT_PRIO_2 send)");
                    return(LSA_TRUE); /* dirty but a litte bit faster */
                } /* else we fall through to next level .. */
            } /* else we fall through to next level .. */


        case EDDS_TXCOMPID_NRT_PRIO_1:
            if (pDDB->pGlob->TxAcycInProgCnt  < EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE )
            {
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_1,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_1;
                    pDDB->pGlob->TxAcycInProgCnt++;
                    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                           "OUT:EDDS_TransmitCheckForSend(NRT_PRIO_1 send)");
                    return(LSA_TRUE); /* dirty but a litte bit faster */
                } /* else we fall through to next level .. */
            }  /* else we fall through to next level .. */


        case EDDS_TXCOMPID_NRT_PRIO_0:
            if (pDDB->pGlob->TxAcycInProgCnt  < EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE )
            {
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_0,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_0;
                    pDDB->pGlob->TxAcycInProgCnt++;
                    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                           "OUT:EDDS_TransmitCheckForSend(NRT_PRIO_0 send)");

                    return(LSA_TRUE); /* dirty but a litte bit faster */
                }  /* else we fall through to next level .. */
            }  /* else we fall through to next level .. */
            #endif

        default:
            break;
    }



    #ifndef EDDS_CFG_NRT_INCLUDE
    LSA_UNUSED_ARG(MaxLengthAcyclic);
    #endif

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitCheckForSend(nothing to send)");

    return(LSA_FALSE);

}

#else

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitCheckForSend                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pPortID      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pTxCompID    +*/
/*+                             LSA_UINT32                     MaxLength    +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                 : Pointer to DDB                                  +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+  pPortID    : Address for LSA_UINT32 of PortID        (output)          +*/
/*+  pTxCompID  : Address for LSA_UINT32 of Component ID (input/output)     +*/
/*+             : Input:  ComponentID to start test for pending tx-requests +*/
/*+               Output: ComponentID a TxFrame was found if something pres.+*/
/*+                                                                         +*/
/*+               Typically the first call uses TXCOMPID_SRT_Z and further  +*/
/*+               calls dont change the returned value. So further calls    +*/
/*+               continue at the last component a valid frame was present. +*/
/*+                                                                         +*/
/*+  MaxLengthA : Max length of frame for NRT/aSRT-Frames                   +*/
/*+                                                                         +*/
/*+  Result     :   LSA_TRUE : Something was present for sending            +*/
/*+                 LSA_FALSE: Nothing was present for sending.             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function checks if a component has something to      +*/
/*+               send.                                                     +*/
/*+                                                                         +*/
/*+               This is done in an priorized order:                       +*/
/*+                                                                         +*/
/*+               Component checked:             pTxCompID:                 +*/
/*+                                                                         +*/
/*+               zyclic  SRT                    TXCOMPID_SRT_Z_XRT         +*/
/*+               zyclic  SRT                    TXCOMPID_SRT_Z_UDP         +*/
/*+               NRT Prio 3                     TXCOMPID_NRT_PRIO_3        +*/
/*+               NRT Prio 2                     TXCOMPID_NRT_PRIO_2        +*/
/*+               NRT Prio 1                     TXCOMPID_NRT_PRIO_1        +*/
/*+               NRT Prio 0                     TXCOMPID_NRT_PRIO_0        +*/
/*+                                                                         +*/
/*+               Check starts with the component (=Prio) given in          +*/
/*+               pTxCompID. On first call use TXCOMPID_SRT_Z_xxx,on further+*/
/*+               call within one sendcycle use the returned value, so      +*/
/*+               the function continues at the point of return.            +*/
/*+                                                                         +*/
/*+               e.g.:                                                     +*/
/*+                                                                         +*/
/*+               TxCompID = TXCOMPID_SRT_Z_XRT;                            +*/
/*+                                                                         +*/
/*+               while (free send resources)                               +*/
/*+               {                                                         +*/
/*+                 while (EDDS_TransmitCheckForSend(....,&TxCompID))       +*/
/*+                 {                                                       +*/
/*+                   .. send the pBuffer returned                          +*/
/*+                 }                                                       +*/
/*+               }                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB,pTXCompID,pPortID)
//JB 11/11/2014 pDDB checked within EDDS_ENTER_CHECK_REQUEST
//JB 11/11/2014 pTXComp checked before calling this function - pPortID out param
//WARNING be careful when using this function, make sure not to use pDDB/pTXCompID as null ptr
EDDS_STATIC LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_TransmitCheckForSend(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pPortID,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pTxCompID,
    LSA_UINT32                                    MaxLengthAcyclic)
{
    LSA_BOOL    DoSend;


    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitCheckForSend(pDDB: 0x%X,pTxCompID: 0x%X,MaxLen: %d)",
                           pDDB,
                           pTxCompID,
                           MaxLengthAcyclic);

    /* --------------------------------------------------------------------------*/
    /* a little speed up for cyclic SRT which is the most important part.        */
    /* --------------------------------------------------------------------------*/

#ifdef EDDS_CFG_SRT_INCLUDE
	if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
	{
		switch ( *pTxCompID ) //lint !e613 this comment should not be neceassy #JB_LINTERROR
		{
			case  EDDS_TXCOMPID_SRT_Z_XRT:
				if ( EDDS_RTTxQueryCyclicXRT(pDDB,pBuffer,pLength) == EDD_STS_OK )
				{
					*pPortID = EDD_PORT_ID_AUTO;
					return(LSA_TRUE); /* dirty but a litte bit faster */
				}
				/* no break, fall through */
				//lint -fallthrough
				//JB 12/11/2014 see comment above
	#ifdef EDDS_XRT_OVER_UDP_INCLUDE
			case  EDDS_TXCOMPID_SRT_Z_UDP:
				if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_XRT_OVER_UDP)
				{
					if ( EDDS_RTTxQueryCyclicUDP(pDDB,pBuffer,pLength) == EDD_STS_OK )
					{
						*pPortID   = EDD_PORT_ID_AUTO;
						*pTxCompID = EDDS_TXCOMPID_SRT_Z_UDP;  //@fixme am2219 remove this line
						return(LSA_TRUE); /* dirty but a litte bit faster */
					}
				}
				/* no break, fall through */
				//lint -fallthrough
				//JB 12/11/2014 see comment above
	#endif
			default:
				*pPortID = EDD_PORT_ID_AUTO; 
				break;
		}
	}
#endif

    /* --------------------------------------------------------------------------*/
    /* Now we check for acyclic frame to send.                                   */
    /* --------------------------------------------------------------------------*/

    DoSend = LSA_FALSE;

#ifdef EDDS_CFG_NRT_INCLUDE
    /* check if we are allowed to send acylic frames ..                      */
    /* Note: for aSRT it is possible that some sends are pending even if the */
    /*       HW isnt setup any more. this is a very very unlikely situation  */
    /*       on shutdown but possible. So we check here if HW is setup.      */
    /*       (possible for internal global ERROR-Frames). For NRT its not    */
    /*       possible but doesnt matter.                                     */
    /* update: aSRT was removed from EDDS but check doesnt matter.           */

    if ((pDDB->pGlob->TxAcycInProgCnt  < EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE ) &&
        (pDDB->pGlob->HWIsSetup))
    {
        switch ( *pTxCompID )  //lint !e613 this comment should not be neceassy #JB_LINTERROR
        {
            case EDDS_TXCOMPID_SRT_Z_XRT:
            case EDDS_TXCOMPID_SRT_Z_UDP:
                /* fall through */
#if EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG != 0
            case EDDS_TXCOMPID_NRT_PRIO_3:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_3,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_3;
                    pDDB->pGlob->TxAcycInProgCnt++;
                    break;
                } /* else we fall through to next level .. */

                /* no break, fall through */
#endif
            case EDDS_TXCOMPID_NRT_PRIO_2:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_2,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_2; //lint !e613 this comment should not be neceassy #JB_LINTERROR
                    pDDB->pGlob->TxAcycInProgCnt++;
                    break;
                } /* else we fall through to next level .. */
				//lint -fallthrough
				//JB 12/11/2014 see comment above

                /* no break, fall through */
            case EDDS_TXCOMPID_NRT_PRIO_1:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_1,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_1; //lint !e613 this comment should not be neceassy #JB_LINTERROR
                    pDDB->pGlob->TxAcycInProgCnt++;
                    break;
                } /* else we fall through to next level .. */
				//lint -fallthrough
				//JB 12/11/2014 see comment above

                /* no break, fall through */
            case EDDS_TXCOMPID_NRT_PRIO_0:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_0,pBuffer,pLength,pPortID,MaxLengthAcyclic) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_0; //lint !e613 this comment should not be neceassy #JB_LINTERROR
                    pDDB->pGlob->TxAcycInProgCnt++;
                    break;
                }

                break;  /* no more azyclic resources. break at all */
            default:
                break;
        } /* switch */
    }
#else
LSA_UNUSED_ARG(MaxLengthAcyclic);
#endif


    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitCheckForSend(DoSend: %d)",
                           DoSend);

    return(DoSend);

}
//lint +esym(613,pDDB,pTXCompID,pPortID)

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitTrigger                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function checks if a new transmit can be initiated   +*/
/*+               (if a transmit buffer is free) and if so it will query    +*/
/*+               all components (NRT,SRT etc) for pending transmit request +*/
/*+               This is done in an priorized order:                       +*/
/*+                                                                         +*/
/*+               zyclic  SRT                                               +*/
/*+               azyklic SRT highprio                                      +*/
/*+               azyklic SRT lowprio                                       +*/
/*+               NRT                                                       +*/
/*+                                                                         +*/
/*+               This function should be called after a frame was transm-  +*/
/*+               mitted (after transmitbuffers become free) or  when a     +*/
/*+               component gets a new transmit-request from user.          +*/
/*+                                                                         +*/
/*+               If there are no free transmitbuffers, nothing is done.    +*/
/*+                                                                         +*/
/*+               Note: this function must not be called from isr-handler   +*/
/*+                     because there may be conflicts with reentrance      +*/
/*+                     when the function is called in request-handler too. +*/
/*+                                                                         +*/
/*+               If a new frame is send, and the timeout is not already    +*/
/*+               running, we start the timeout.                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked within EDDS_ENTER_CHECK_REQUEST
//WARNING: be careful while using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TransmitTrigger(EDDS_LOCAL_DDB_PTR_TYPE     pDDB) 
{

    EDD_UPPER_MEM_PTR_TYPE          pBuffer;
    LSA_UINT32                      Length;
    LSA_BOOL                        Present;
    LSA_UINT32                      TxCompID;
    LSA_RESULT                      Status;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;
    LSA_UINT32                      MaxLengthAcyclic;
    LSA_UINT32                      PortID;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_UINT16                      RetVal;
	LSA_USER_ID_TYPE                Dummy;
#endif


    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitTrigger(pDDB: 0x%X)",
                           pDDB);

    Present  = LSA_FALSE;

    pGlob    =  pDDB->pGlob;

    /* --------------------------------------------------------------------------*/
    /* In blocked mode we limit the max-acyclic Data len depending on linespeed. */
    /* --------------------------------------------------------------------------*/

    #ifdef EDDS_CFG_SEND_CYCLE_BASED
    MaxLengthAcyclic = pGlob->TxMaxAcylic;
    #else
    //@fixme am2219 define a "no limit" value, e.g. #define EDDS_MAXLENGTHACYCLIC 0xFFFFFFFFUL
    MaxLengthAcyclic = 0xFFFFFFFFUL;                                 /* no limit */
    #endif

    ///@fixme am2219 use % of baud; unlimited if link is down
    MaxLengthAcyclic = 0xFFFFFFFFUL;                                 /* no limit */  //lint !e838 JB 10/12/2014 tmp

    /* --------------------------------------------------------------------------*/
    /* Now look for free resources and pending send requests..                   */
    /* --------------------------------------------------------------------------*/

    #ifdef EDDS_CFG_SEND_BLOCKED
    if (! pGlob->TxInProgCnt)     /* only if no transmit is in progress anymore */
    {
        #endif

        /* -----------------------------------------------------------------------*/
        /* Query as long as free tx-resources are present and something is to send*/
        /* -----------------------------------------------------------------------*/

        #ifdef EDDS_CFG_ORG_PRIO_IS_HIGHEST
        TxCompID = EDDS_TXCOMPID_NRT_PRIO_3; /* start with ORG-prio  */
        #else
        TxCompID = EDDS_TXCOMPID_SRT_Z_XRT; /* start with cyclic SRT */
        #endif

        while (( ! pGlob->pTxInfo[pGlob->TxFreeIdx].InUse  ) &&
               ( EDDS_TransmitCheckForSend(pDDB,&pBuffer,&Length,&PortID,&TxCompID,MaxLengthAcyclic) ))
        {
            /* if not a cyclic Frame decrement MaxLengthAcyclic */
            if (( TxCompID != EDDS_TXCOMPID_SRT_Z_XRT ) && ( TxCompID != EDDS_TXCOMPID_SRT_Z_UDP ))
            {
                MaxLengthAcyclic-= Length;
            }

            Present  = LSA_TRUE;

            pGlob->pTxInfo[pGlob->TxFreeIdx].TxCompID = TxCompID;
            pGlob->pTxInfo[pGlob->TxFreeIdx].InUse    = LSA_TRUE;

            /* -----------------------------------------------------------------*/
            /* advance to next entry.                                           */
            /* -----------------------------------------------------------------*/

            if ( pGlob->TxFreeIdx < (pGlob->TxBuffCnt-1) )
            {
                pGlob->TxFreeIdx++;
            }
            else
            {
                pGlob->TxFreeIdx = 0;
            }

            #ifdef EDDS_CFG_DO_INTERNAL_FATAL_CHECK
            if  (( Length == 0 ) ||
                 ( LSA_HOST_PTR_ARE_EQUAL(pBuffer, LSA_NULL) ))
            {
                EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,
                                EDDS_MODULE_ID,
                                __LINE__);
            }
            #endif

            EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SEND(pDDB: 0x%X,pBuffer: 0x%X,Len: %d,Port: %d)",
                                pDDB,
                                pBuffer,
                                Length,
                                PortID);

            Status = EDDS_LL_SEND(pDDB,pBuffer,Length,PortID);

            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND(). Status: 0x%X",Status);

            /* -----------------------------------------------------------------*/
            /* we dont expect an error here. if so something goes wrong inside  */
            /* the driver which has to be fixed, so we signal an fatal error    */
            /* -----------------------------------------------------------------*/

            switch (Status)
            {
                case  EDD_STS_OK:

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                    /* -----------------------------------------------------*/
                    /*  a new frame is send. start timer for timeout if not */
                    /*  already running and configured                      */
                    /* -----------------------------------------------------*/

                    if ((   pGlob->TxTimer.InUse ) &&
                        ( ! pGlob->TxTimer.InProgress ))
                    {

                        Dummy.uvar32 = pGlob->TxTimer.EDDSTimerID;

                        EDDS_START_TIMER(&RetVal,
                                         pGlob->TxTimer.TimerID,
                                         Dummy,
                                         pGlob->TxToutCnt);

                        if ( RetVal != LSA_RET_ERR_PARAM )
                        {
                            pGlob->TxTimer.InProgress = LSA_TRUE;
                        }
                    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

                    pGlob->TxInProgCnt++;

                    break;

                case  EDD_STS_ERR_RESOURCE:

                    EDDS_FatalError(EDDS_FATAL_ERR_SEND,
                                    EDDS_MODULE_ID,
                                    __LINE__); /* does not return!*/

                    break;

                case  EDD_STS_ERR_PARAM:
                default:
                    EDDS_FatalError(EDDS_FATAL_ERR_SEND,
                                    EDDS_MODULE_ID,
                                    __LINE__); /* does not return!*/

                    break;

            } /* switch */

        } /* while */

        #ifdef EDDS_CFG_SEND_BLOCKED
        if ( Present )
        {
            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SEND_TRIGGER(pDDB: 0x%X)",pDDB);

            EDDS_LL_SEND_TRIGGER(pDDB);

            EDDS_LOWER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND_TRIGGER()");
        }
        #endif /* EDDS_CFG_SEND_BLOCKED */


        #ifdef EDDS_CFG_SEND_BLOCKED
    }
    #endif /* EDDS_CFG_SEND_BLOCKED */

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitTrigger(Present: %d)",
                           Present);


}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitShutdown                       +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    -----------                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after the LLIF was shutdown. (in Shutdown-Request) +*/
/*+                                                                         +*/
/*+               We stop Tx-Timer and finish all pending tx                +*/
/*+                                                                         +*/
/*+               Note that all handles already closed, so no user waits    +*/
/*+               for this tx to be ready!                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 pDDB checked within EDDS_GetDDB
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TransmitShutdown(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB)
{
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_UINT16                      RetVal;
#endif
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitShutdown(pDDB: 0x%X)",
                           pDDB);

    pGlob =  pDDB->pGlob;

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    if ( pGlob->TxTimer.InUse   )
    {
        /* doesnt matter if the timer is in progress or not */
        EDDS_STOP_TIMER(&RetVal,pGlob->TxTimer.TimerID);
        pGlob->TxTimer.InProgress = LSA_FALSE;
    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    /* --------------------------------------------------------------------------*/
    /* Now we are no more interrupted by high-prio context, because tx is stopped*/
    /* --------------------------------------------------------------------------*/
	while ( pGlob->pTxInfo[pGlob->TxCheckIdx].InUse  )
	{
		/* -----------------------------------------------------------------*/
		/* Now call the component the sent frame was from. so the component */
		/* can finish the actions neccessary with the sending of the frame  */
		/* Note: Currently it is not possible that there are any NRT-Request*/
		/*       pending on shutdown!                                       */
		/* -----------------------------------------------------------------*/

		switch (pGlob->pTxInfo[pGlob->TxCheckIdx].TxCompID)
		{
				#ifdef EDDS_CFG_NRT_INCLUDE
			case EDDS_TXCOMPID_NRT_PRIO_0:
				EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_0,EDD_STS_OK);
				break;
			case EDDS_TXCOMPID_NRT_PRIO_1:
				EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_1,EDD_STS_OK);
				break;
			case EDDS_TXCOMPID_NRT_PRIO_2:
				EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_2,EDD_STS_OK);
				break;
			case EDDS_TXCOMPID_NRT_PRIO_3:
				EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_3,EDD_STS_OK);
				break;
				#endif

				#ifdef EDDS_CFG_SRT_INCLUDE
			case EDDS_TXCOMPID_SRT_Z_XRT:
				if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
				{
					EDDS_RTTxReadyCyclicXRT(pDDB,EDD_STS_OK);
				}
				break;

				#ifdef EDDS_XRT_OVER_UDP_INCLUDE
			case EDDS_TXCOMPID_SRT_Z_UDP:
				if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_XRT_OVER_UDP)
				{
					EDDS_RTTxReadyCyclicUDP(pDDB,EDD_STS_OK);
				}
				break;
				#endif

				#endif

			default:
				EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
								EDDS_MODULE_ID,
								__LINE__);
				break;
		}

		pGlob->pTxInfo[pGlob->TxCheckIdx].InUse = LSA_FALSE;

		/* -----------------------------------------------------------------*/
		/* advance to next entry.                                           */
		/* -----------------------------------------------------------------*/

		if ( pGlob->TxCheckIdx < (pGlob->TxBuffCnt-1) )
		{
			pGlob->TxCheckIdx++;
		}
		else
		{
			pGlob->TxCheckIdx = 0;
		}

	} /* while */

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitTrigger()");

}
//lint +esym(613,pDDB)

/*****************************************************************************/
/*  end of file edds_tx.c                                                    */
/*****************************************************************************/

