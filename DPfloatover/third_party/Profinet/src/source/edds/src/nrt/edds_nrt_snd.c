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
/*  F i l e               &F: edds_nrt_snd.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDS. Transmit           */
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
/*  12.02.04    JS    added support for multiple send priorities (3)         */
/*  05.04.04    JS    changes to prevent compiler warnings                   */
/*  15.06.04    JS    always use send-prio                                   */
/*  18.06.04    JS    added PortID rangecheck.                               */
/*  31.01.05    JS    LTRC support                                           */
/*  06.07.05    JS    added multiport support                                */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
/*  17.06.08    JS    removed extPaddingEnable. EDDS does not pad anymore.   */
/*  25.11.11    MA    EDD_NRT_SEND_PRIO_ORG -> EDD_NRT_SEND_PRIO_MGMT_LOW    */
/*                                           and EDD_NRT_SEND_PRIO_MGMT_HIGH */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  129
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_NRT_SND */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_NRT_INCLUDE

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTTxQuery                             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_UINT32                     Prio         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ... *pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pPortID      +*/
/*+                             LSA_UINT32                     MaxLength    +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Prio       : Priority to test for pendings sends (0,1,2)               +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+  pLength    : Address for LSA_UINT32 of PortID        (output)          +*/
/*+  MaxLength  : Max Framelength to provide (input)                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA           No pending tx-requests       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will look for pending tx-requests for this  +*/
/*+               device and returns a pointer to the tx-buffer if one is   +*/
/*+               present and is <= MaxLength                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB,pBuffer,pLength,pPortID)
//JB 17/11/2014 pDDB checked where called - other: set in function
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTTxQuery(
    EDDS_LOCAL_DDB_PTR_TYPE                           pDDB,
    LSA_UINT32                                        Prio,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR       *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                       pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                       pPortID,
    LSA_UINT32                                        MaxLength)

{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDD_UPPER_RQB_PTR_TYPE          pRQB;
    EDDS_LOCAL_HDB_PTR_TYPE         pHDB;


    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTTxQuery(pDDB: 0x%X, Prio: %d, MaxLength: %d)",
                           pDDB,
                           Prio,
                           MaxLength);

    pNRTComp    = pDDB->pNRT;

    pRQB = pNRTComp->Tx[Prio].Req.pBottom;

    /*----------------------------------------------------------------- */
    /* Something present and < MaxLength? and allowed to send more?     */
    /*----------------------------------------------------------------- */

    if ( (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) &&
         ( pNRTComp->Tx[Prio].InProgCnt < pNRTComp->Tx[Prio].InProgCntMax  ) &&
         (((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->Length <= MaxLength ))
    {

        EDDS_RQB_REM_BLOCK_BOTTOM(pNRTComp->Tx[Prio].Req.pBottom,
                                  pNRTComp->Tx[Prio].Req.pTop,
                                  pRQB);

        /*------------------------------------------------------------- */
        /* Queue request in "inprogress" queue.                         */
        /*------------------------------------------------------------- */

        EDDS_RQB_PUT_BLOCK_TOP( pNRTComp->TxInProg.pBottom,
                                pNRTComp->TxInProg.pTop,
                                pRQB);
        /*------------------------------------------------------------- */
        /* Manipulating of framebuffer is only allowed for NRT-Send     */
        /*------------------------------------------------------------- */

        if (EDD_SRV_NRT_SEND == EDD_RQB_GET_SERVICE(pRQB))
        {
            EDDS_NRT_SEND_HOOK(pDDB->hSysDev, pRQB);
        }

        /*------------------------------------------------------------- */
        /* No parameter check here. already done when request is queued */
        /*------------------------------------------------------------- */
        *pBuffer = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->pBuffer;
        *pLength = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->Length;
        *pPortID = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->PortID;

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        if(!pDDB->pGlob->ZeroCopyInterface)
        {
            /* copy Ethernet frame from upper layer into transfer buffer */
            EDDS_MEMCOPY_UPPER(pDDB->pGlob->pTxTransferMem[pDDB->pGlob->TxFreeIdx],*pBuffer,*pLength);
            /* set *pBuffer to transfer buffer (for EDDS_LL_SEND)*/
            *pBuffer = pDDB->pGlob->pTxTransferMem[pDDB->pGlob->TxFreeIdx];
        }
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */


        EDDS_NRT_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,
                              "EDDS_NRTTxQuery: Prio: %d,pBuffer: %Xh, Length: %d, PortID: %d",
                              Prio,
                              *pBuffer,
                              *pLength,
                              *pPortID);

        Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE(pRQB), &pHDB); /* get handle */

        /* we dont expect an error here because the handle has to be valid */

        if ( Status != EDD_STS_OK )
        {
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                            EDDS_MODULE_ID,
                            __LINE__);
        }

        Status = EDD_STS_OK;
        pNRTComp->Tx[Prio].InProgCnt++;  /* one more transmit in progress at all     */
        pHDB->pNRT->TxInProgCnt++;       /* one more transmit in progress for handle */
        pHDB->pNRT->TxQueueReqCnt--;
    }
    else Status = EDD_STS_OK_NO_DATA;


    EDDS_NRT_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTTxQuery(*pBuffer: 0x%X, Length: %d, Port: %d), Status: 0x%X",
                           *pBuffer,
                           *pLength,
                           *pPortID,
                           Status);

    return(Status);

}
//lint +esym(613,pDDB,pBuffer,pLength,pPortID)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTTxReady                             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_UINT32                     Prio         +*/
/*+                             LSA_RESULT                     TxStatus     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Prio       : Priority of sended frame (0,1,2)                          +*/
/*+  TxStatus   : tx-status of "inprogress" tx. put into RQB                +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA                                        +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishes the first pending tx-request in "inprogress"     +*/
/*+               queue. Unqueuing the reqeuest and calling the handles     +*/
/*+               call-back-function.                                       +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               EDD_STS_OK_NO_DATA is returned, if nothing is pending.    +*/
/*+               EDD_STS_ERR_PARAM  is returned, if handle is invalid.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 17/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
//@fixme am2219 NEA6.0 function refactoring
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTTxReady(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_UINT32              Prio,
    LSA_RESULT              TxStatus)

{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDD_UPPER_RQB_PTR_TYPE          pRQB;
    EDDS_LOCAL_HDB_PTR_TYPE         pHDB;

    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTTxReady(pDDB: 0x%X, Prio: %d, TxStatus: %d)",
                           pDDB,
                           Prio,
                           TxStatus);

    pNRTComp    = pDDB->pNRT;

    EDDS_RQB_REM_BLOCK_BOTTOM(pNRTComp->TxInProg.pBottom,
                              pNRTComp->TxInProg.pTop,
                              pRQB);


    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) ) /* something present ?*/
    {

        pNRTComp->Tx[Prio].InProgCnt--; /* transmit ready */

        Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE(pRQB), &pHDB);

        if ( Status != EDD_STS_OK )
        {

            EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,
                                  "EDDS_NRTTxReady. Invalid Handle (Hdl: 0x%X)",
                                  EDD_RQB_GET_HANDLE(pRQB));

            Status = EDD_STS_ERR_PARAM;  /* handle not present */
        }
        else
        {

            EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,
                                  "EDDS_NRTTxReady. Hdl: 0x%X, pRQB: 0x%X, TxStatus: 0x%X",
                                  pHDB->Handle,
                                  pRQB,
                                  TxStatus);

            pHDB->pNRT->TxInProgCnt--;
            EDDS_RequestFinish(pHDB,pRQB,TxStatus);

            /* If cancel is in progress and this was the last */
            /* tx for this channel we continue with part 2 of */
            /* cancel-handling..                              */

            if (( pHDB->pNRT->CancelInProgress  ) &&
                ( ! pHDB->pNRT->TxInProgCnt     ) &&
                ( ! pHDB->pNRT->CancelRQB.InUse ))
            {
                pHDB->pNRT->CancelRQB.InUse = LSA_TRUE;
                EDDS_DO_NRT_CANCEL_PART2(pHDB->pNRT->CancelRQB.pRQB);
            }

            Status = EDD_STS_OK;

        }

    }
    else Status = EDD_STS_OK_NO_DATA;

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTTxReady(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTSend                                +*/
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
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_ALIGN                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles EDD_SRV_NRT_SEND Request                          +*/
/*+               Checks for Parameters and if ok queues request in tx-queue+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 17/11/2014 pRQB and pHDB are already checked when this function is called
//WARNING: be careful when using this function, make sure not to call it with pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTSend(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDD_UPPER_NRT_SEND_PTR_TYPE         pRQBSnd;
#if EDDS_TX_NRT_FRAME_BUFFER_ALIGN
    LSA_UINT32                          Addr;
#endif
    LSA_UINT32                          Prio;
    EDDS_LOCAL_DDB_PTR_TYPE     		pDDB			= pHDB->pDDB;

    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTSend(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);


    Status     = EDD_STS_OK;
    pRQBSnd    = (EDD_UPPER_NRT_SEND_PTR_TYPE) pRQB->pParam;

    if (  (pRQBSnd->Length < EDD_NRT_MIN_SND_LEN ) ||
          (pRQBSnd->Length > EDD_NRT_MAX_SND_LEN ) ||
          (pRQBSnd->PortID > pDDB->pGlob->HWParams.Caps.PortCnt    ) || /* we support AUTO */
          (LSA_HOST_PTR_ARE_EQUAL( pRQBSnd->pBuffer, LSA_NULL)) )
    {

        EDDS_UPPER_TRACE_03(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_NRTSend: Invalid RQB parameter(s). Length: %d, PortID: %d, pBuffer: 0x%X",
                            pRQBSnd->Length,
                            pRQBSnd->PortID,
                            pRQBSnd->pBuffer);

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

#if EDDS_TX_NRT_FRAME_BUFFER_ALIGN

        /* check for buffer-alignment */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        EDDS_PTR_TO_ADDR_UPPER(pDDB->hSysDev,(EDD_UPPER_MEM_PTR_TYPE)pRQBSnd->pBuffer,&Addr);
#else
        Addr = (LSA_UINT32)pRQBSnd->pBuffer;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

        if ( ((LSA_UINT32) (Addr)) & EDDS_TX_NRT_FRAME_BUFFER_ALIGN )
        {
            EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_NRTRecv: Invalid alignment of pBuffer address");

            Status = EDD_STS_ERR_ALIGN;
        }
#endif
    }

    if ( Status == EDD_STS_OK)
    {

        /* --------------------------------------------------------------*/
        /* Determine Priority to use for sending. default to norm.       */
        /* --------------------------------------------------------------*/

        switch (pRQBSnd->Priority)
        {
                #if EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG != 0
			case EDD_NRT_SEND_PRIO_MGMT_LOW:	/*LLDP*/
				/* fall through */
			case EDD_NRT_SEND_PRIO_MGMT_HIGH:	/*GSY, MRP*/
                Prio = EDDS_NRT_TX_PRIO_3;         /* used for ORG      */
                break;
                #endif
            case EDD_NRT_SEND_PRIO_ASRT_HIGH:
                Prio = EDDS_NRT_TX_PRIO_2;         /* used for ASRT high*/
                break;
            case EDD_NRT_SEND_PRIO_ASRT_LOW:
                Prio = EDDS_NRT_TX_PRIO_1;         /* used for ASRT low */
                break;
            default:
                Prio = EDDS_NRT_TX_PRIO_0;         /* lowest prio. TCP  */
                break;
        }

        /* --------------------------------------------------------------*/
        /* insert src-mac in frame if configured.                        */
        /* --------------------------------------------------------------*/

        if  ( pHDB->InsertSrcMAC )
        {
            LSA_UINT32 i;


            EDDS_NRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                                  "EDDS_NRTSend: Insert SRC-Mac.");;

            for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
            {
                ((EDD_UPPER_MEM_U8_PTR_TYPE) pRQBSnd->pBuffer)[6+i] = pDDB->pGlob->HWParams.MACAddress.MacAdr[i];
            }

        }

        /* Because this sequence is done in low-prio context and must not be */
        /* interrupted by high-prio context, we have to use EDDS_ENTER_HIGH() */

        EDDS_ENTER_HIGH();

        EDDS_NRT_TRACE_04(0,LSA_TRACE_LEVEL_NOTE,
                              "EDDS_NRTSend: Send RQB queued: Hdl: 0x%X, pRQB: 0x%X, pBuffer: 0x%X, Len: %d",
                              pHDB->Handle,
                              pRQB,
                              pRQBSnd->pBuffer,
                              pRQBSnd->Length);

        EDDS_RQB_PUT_BLOCK_TOP( pDDB->pNRT->Tx[Prio].Req.pBottom,
                                pDDB->pNRT->Tx[Prio].Req.pTop,
                                pRQB);

        pHDB->pNRT->TxQueueReqCnt++;

        /* --------------------------------------------------------------*/
        /*  Trigger transmission                                         */
        /* --------------------------------------------------------------*/

        #if !defined(EDDS_CFG_SEND_CYCLE_BASED) && !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        EDDS_TransmitTrigger(pDDB); /* Trigger a transmission for this dev.*/
        /* This may cause a query for this     */
        /* Tx if no transmission is in progress*/
        #endif

        EDDS_EXIT_HIGH();
    }

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTSend(), Status: 0x%X",
                           Status);


    return(Status);

}
//lint +esym(613,pRQB,pHDB)

#endif

/*****************************************************************************/
/*  end of file NRT_SND.C                                                    */
/*****************************************************************************/
