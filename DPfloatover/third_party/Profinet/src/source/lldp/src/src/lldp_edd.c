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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_edd.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  LLDP EDD functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  20.04.04    JS    initial version.                                       */
/*  29.06.04    JS    little change with err-handling                        */
/*                    buxfix. wrong sizes on alloc.                          */
/*  26.07.04    JS    all link-up-states will be mapped to LINK_UP           */
/*  22.09.04    JS    LINK_UNKNOWN will be mapped to LINK_DOWN               */
/*  15.11.04    JS    use LLDP_RX_FRAME_BUFFER_LENGTH                        */
/*  26.04.04    JS    bufix with asserts enabled                             */
/*  15.07.05    JS    bufix. set EDD_STS_ERR_RESOURCE if allocation failed.  */
/*  06.10.05    JS    "DoSend" added.                                        */
/*  21.05.06    JS    some casts to avoid compiler warnings with gcc         */
/*  21.06.06    JS    added PortMAC Address support                          */
/*  27.06.06    JS    added  LLDP_FILE_SYSTEM_EXTENSION                      */
/*  01.09.06    JS    changed LLDP-Send Prio to be configurable.             */
/*  29.05.08    JS    changes to avoid C++ errors with void asignments       */
/*  05.02.09    JS    removed parameter "DoCheck" (now always TRUE)          */
/*                    added parameter TxFrmGroup                             */
/*  13.05.09    JS    moved LLDP_ConstructFrame() from lldp_usr.c            */
/*                    to only use LLDP_BroadcastMac[] within this file       */
/*  18.09.09    JS    added LLDP_CheckTLVEqual()                             */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  5
#define LLDP_MODULE_ID     LTRC_ACT_MODUL_ID /* LLDP_MODULE_ID_LLDP_EDD */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lldp_inc.h"            /* LLDP headerfiles */
#include "lldp_int.h"            /* internal header */

LLDP_FILE_SYSTEM_EXTENSION(LLDP_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

const LSA_UINT8 LLDP_BroadcastMac[LLDP_MAC_ADDR_SIZE]= {0x01,0x80,0xC2,0x00,0x00,0x0E};

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_CheckTLVEqual                          +*/
/*+  Input                      LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame         +*/
/*+                             LLDP_COMMON_MEM_U8_PTR_TYPE  pTLV           +*/
/*+                             LSA_UINT32                   TLVLength      +*/
/*+                                                                         +*/
/*+  Result                :    LSA_TRUE : TLV chain is equal               +*/
/*+                             LSA_FALSE: TLV chain is noz equal           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if TLV is equal to current one within frame        +*/
/*+                                                                         +*/
/*+               * set Type                                                +*/
/*+               * set TLV-Chain                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL LLDP_LOCAL_FCT_ATTR LLDP_CheckTLVEqual(
    LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame,
    LLDP_COMMON_MEM_U8_PTR_TYPE  pTLV,
    LSA_UINT32                   TLVLength)
{

    LSA_BOOL Result = LSA_FALSE;

    /* check if TLVChain equal to the one within Frame */
    if ( LLDP_MEMCMP_COMMON(
          (LLDP_COMMON_MEM_PTR_TYPE) &pFrame[LLDP_FRAME_TLV_OFFSET],
          (LLDP_COMMON_MEM_PTR_TYPE) pTLV,
          TLVLength))
    {
        Result = LSA_TRUE;
    }

#ifdef LLDP_CFG_NO_EQUAL_CHECK
    Result = LSA_FALSE;        
#endif

    return(Result);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_ConstructFrame                         +*/
/*+  Input                      LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame         +*/
/*+                             LLDP_COMMON_MEM_U8_PTR_TYPE  pTLV           +*/
/*+                             LSA_UINT32                   TLVLength      +*/
/*+                             LLDP_COMMON_MEM_U8_PTR_TYPE  pSrcMAC        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Constructs LLDP-Frame with TLV. Result is put into pFrame +*/
/*+                                                                         +*/
/*+               * set SRC-MAC                                             +*/
/*+               * set DST-MAC                                             +*/
/*+               * set Type                                                +*/
/*+               * set TLV-Chain                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_ConstructFrame(
    LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame,
    LLDP_COMMON_MEM_U8_PTR_TYPE  pTLV,
    LSA_UINT32                   TLVLength,
    LLDP_COMMON_MEM_U8_PTR_TYPE  pSrcMAC)
{

    LLDP_COMMON_MEM_U8_PTR_TYPE  pTemp;

    /* to prevent tasking error we use a temp-var */
    pTemp = (LLDP_COMMON_MEM_U8_PTR_TYPE) LLDP_BroadcastMac;

    LLDP_COPY_MAC_ADDR_COMMON(
        &pFrame[LLDP_FRAME_DST_MAC_OFFSET],
        pTemp);

    LLDP_COPY_MAC_ADDR_COMMON(
        &pFrame[LLDP_FRAME_SRC_MAC_OFFSET],
        pSrcMAC);

    pFrame[LLDP_FRAME_TYPE_OFFSET]   = (LSA_UINT8) (LLDP_ETHERTYPE >> 8);
    pFrame[LLDP_FRAME_TYPE_OFFSET+1] = (LSA_UINT8) (LLDP_ETHERTYPE & 0xFF);

    LLDP_MEMCOPY_COMMON(
        (LLDP_COMMON_MEM_PTR_TYPE) &pFrame[LLDP_FRAME_TLV_OFFSET],
        (LLDP_COMMON_MEM_PTR_TYPE) pTLV,
        TLVLength);

    LLDP_ASSERT_FALSE((TLVLength <= LLDP_MAX_TLV_DATA_SIZE));
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDFreePortMgm                         +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE                 pHDB      +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD HDB                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Free EDD Port management                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDFreePortMgm(
    LLDP_HDB_PTR_TYPE         pHDB)
{

    LSA_UINT16                FreeStat;
    LLDP_EDD_PCTRL_PTR_TYPE   pPCtrl;
    LSA_UINT32                i;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDFreePortMgm(pHDB: 0x%X)",pHDB);
    FreeStat = 0;
    /* --------------------------------------------------------------------------*/
    /* Release Port-Memory                                                       */
    /* --------------------------------------------------------------------------*/
    pPCtrl = pHDB->pPCtrl;
    if (! LSA_HOST_PTR_ARE_EQUAL(pPCtrl, LSA_NULL) )
    {
        pPCtrl = pHDB->pPCtrl;
        for ( i=0; i<pHDB->PortCnt; i++)
        {   /* free rx-mem */
            if (!LSA_HOST_PTR_ARE_EQUAL(pPCtrl->pRx, LSA_NULL) )
            {
                LLDP_FREE_LOCAL_MEM(&FreeStat,pPCtrl->pRx);
                LLDP_MEM_FREE_FAIL_CHECK(FreeStat);
                pPCtrl->pRx = LSA_NULL;
            }
            #ifdef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
            /* free tx-frame-mem */
            if (!LSA_HOST_PTR_ARE_EQUAL(pPCtrl->Tx.pFrame, LSA_NULL) )
            {
                LLDP_EDD_FREE_LOWER_TXMEM(&FreeStat, pPCtrl->Tx.pFrame,pHDB->Params.pSys);
                LLDP_MEM_FREE_FAIL_CHECK(FreeStat);
                pPCtrl->Tx.pFrame = LSA_NULL;
            }
            #endif
            pPCtrl++;
        }
        /* ----------------------------------------------------------------------*/
        /* Free Port management                                                  */
        /* ----------------------------------------------------------------------*/
        LLDP_FREE_LOCAL_MEM(&FreeStat, pHDB->pPCtrl);
        LLDP_MEM_FREE_FAIL_CHECK(FreeStat);
        pHDB->pPCtrl = LSA_NULL;
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDFreePortMgm()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDInitPortMgm                         +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE             pHDB          +*/
/*+                             LSA_UINT32                    PortCnt       +*/
/*+                             LSA_UINT32                    PortMaxSender +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB         : Pointer to EDD HDB                                      +*/
/*+  PortCnt      : Number of Ports for HDB (1..)                           +*/
/*+  PortMaxSender: Number of max supported sender per port (1..)           +*/
/*+                                                                         +*/
/*+  Result     : LLDP_RSP_OK                                               +*/
/*+               LLDP_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes HDB with PORT-Managment (memory)              +*/
/*+               The PortCnt and PortMaxSender has to be set in pHDB       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDInitPortMgm(
    LLDP_HDB_PTR_TYPE     pHDB,
    LSA_UINT32            PortCnt,
    LSA_UINT32            PortMaxSender)
{

    LLDP_RSP                  Status;
    LLDP_EDD_PCTRL_PTR_TYPE   pPCtrl;
    LSA_UINT32                DSize;
    LSA_UINT32                i,j;

    #ifdef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    LSA_USER_ID_TYPE          UserId;
    #endif

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDInitPortMgm(pHDB: 0x%X,PortCnt: %d,MaxSender: %d)",pHDB,PortCnt,PortMaxSender);

    LLDP_ASSERT(PortCnt == 0);
    LLDP_ASSERT(PortMaxSender == 0);

    Status   = LLDP_RSP_OK;

    DSize = sizeof(LLDP_EDD_PCTRL_TYPE) * PortCnt;

    LLDP_ALLOC_LOCAL_MEM((LLDP_LOCAL_MEM_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *)&pHDB->pPCtrl,DSize);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDB->pPCtrl, LSA_NULL) )
    {
        Status = LLDP_RSP_ERR_RESOURCE;
        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"LLDP_EDDInitPortMgm(). Allocating memory failed.");
    }
    else
    {
        /* initialize memory with 0 */
        LLDP_MEMSET_LOCAL(pHDB->pPCtrl,0,DSize);
        pPCtrl            = pHDB->pPCtrl;
        i = 0;

#ifdef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
        UserId.uvar32 = 0;
#endif
        while (( Status == LLDP_RSP_OK ) && ( i<pHDB->PortCnt))
        {
            pPCtrl->ThisIdx    = i;

            pPCtrl->Tx.State    = LLDP_TX_FRAME_STATE_STOP;
            pPCtrl->Tx.Underway = LSA_FALSE;
            pPCtrl->Tx.DataNew  = LSA_FALSE;
            pPCtrl->Tx.DoSend   = LSA_FALSE;

            pPCtrl->LinkStatus.Status = EDD_LINK_UP;
            pPCtrl->RxStoped          = LSA_TRUE;
            pPCtrl->LinkStatus.Mode   = EDD_LINK_UNKNOWN;
            pPCtrl->LinkStatus.Speed  = EDD_LINK_UNKNOWN;
            
            pPCtrl->TooManyState      = LLDP_RX_FRAME_NOT_TOO_MANY;

            /* allocate PortMaxSender (=RxCnt) Sender resources */

            DSize = sizeof(LLDP_RX_FRAME_INFO_TYPE) * PortMaxSender;

            LLDP_ALLOC_LOCAL_MEM((LLDP_LOCAL_MEM_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *)&pPCtrl->pRx,DSize);

            if (!LSA_HOST_PTR_ARE_EQUAL(pPCtrl->pRx, LSA_NULL) )
            {
                /* initialize memory with 0 */
                LLDP_MEMSET_LOCAL(pPCtrl->pRx,0,DSize);

                for (j=0; j<PortMaxSender; j++)
                {
                    pPCtrl->pRx[j].ThisIdx              = j;
                    pPCtrl->pRx[j].State                = LLDP_RX_FRAME_STATE_DATA_INVALID;
                    pPCtrl->pRx[j].Ind.StateIndicated   = LLDP_RX_FRAME_STATE_DATA_INVALID;
                    pPCtrl->pRx[j].Ind.IndLastInvalid   = LSA_FALSE;
                    pPCtrl->pRx[j].Ind.Changed          = LSA_FALSE;
                }
            }
            else
            {
                Status = LLDP_RSP_ERR_RESOURCE;
            }

            #ifdef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN

            if ( Status == LLDP_RSP_OK )
            {
                /* allocate transmit frame buffers */

                LLDP_EDD_ALLOC_LOWER_TXMEM(&pPCtrl->Tx.pFrame,
                                           UserId,
                                           LLDP_MAX_FRAME_LENGTH,
                                           pHDB->Params.pSys);

                if ( LSA_HOST_PTR_ARE_EQUAL(pPCtrl->Tx.pFrame, LSA_NULL) )
                {
                    Status = LLDP_RSP_ERR_RESOURCE;
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"LLDP_EDDInitPortMgm(). Allocating memory failed.");
                }
                else
                {
                    pPCtrl++;
                    i++;
                }
            }
            #else
            pPCtrl++;
            i++;
            #endif
        }

        /*-----------------------------------------------------------------------*/
        /* on error we free all allocated memory                                 */
        /*-----------------------------------------------------------------------*/

        if ( Status != LLDP_RSP_OK )
        {
            LLDP_EDDFreePortMgm(pHDB);
        }
    }
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDInitPortMgm(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_ConstructShutdown                      +*/
/*+  Input                      LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame         +*/
/*+                             LSA_UINT32                   FrameLength    +*/
/*+                                                                         +*/
/*+  Result:  Length of Shutdown frame                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Constructs LLDP-Shutdown frame from current Data-Frame    +*/
/*+                                                                         +*/
/*+               * replaces TTL-Value to 0                                 +*/
/*+               * inserts END-TLV behind TTL-TLV                          +*/
/*+               * calculates new length                                   +*/
/*+                                                                         +*/
/*+               A shutdown frame only contains chassis-id,port-id,ttl and +*/
/*+               end-tlv (if needed)                                       +*/
/*+                                                                         +*/
/*+               Note: We assume pFrame is a valid LLDP-Frame with valid   +*/
/*+                     TLVs (e.g chassis-id,port-id,ttl) and a MAC-Header  +*/
/*+                     without VLAN-Tag!                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_UINT32 LLDP_LOCAL_FCT_ATTR LLDP_ConstructShutdown(
    LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame,
    LSA_UINT32                   FrameLength)
{

    LLDP_COMMON_MEM_U8_PTR_TYPE pTLV;
    LSA_UINT32                  Type;
    LSA_UINT32                  Length;
    LLDP_COMMON_MEM_U8_PTR_TYPE pValue;
    LLDP_RSP                    Response;
    LSA_UINT32                  TLength;

    pTLV    = &pFrame[LLDP_FRAME_TLV_OFFSET];  /* start of TLV chain */

    TLength = FrameLength - LLDP_FRAME_TLV_OFFSET;  /* TLV-Length */

    Response = LLDP_RSP_OK;

    /* read TLV-chain till TTL-TLV */
    do
    {
        Response = LLDP_TLVGetCommon(
                       (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLV,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE) &TLength,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE) &Type,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE) &Length,
                       (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);
    }
    while ((Response == LLDP_RSP_OK) && (Type != LLDP_TLV_TYPE_TTL ));

    /* pTLV points behind TTL-TLV                                  */
    /* TLength has the remaining Length of the frame               */
    /* calculate the current length of the frame                   */
    /* (header + all tlv till TTL (chassis,port,ttl)               */

    TLength = FrameLength - TLength;

    if ( Response == LLDP_RSP_OK )
    {
        /* set TTL to 0 */
        pValue[0] = 0;
        pValue[1] = 0;

        /* append END-TLV behind TTL */
        LLDP_TLVSetCommon(
            &pTLV,
            &TLength,
            LLDP_TLV_TYPE_LLDPDU_END,
            0,
            LSA_NULL);
        /* TLength= Header+chassis-id+port-id+ttl+end-tlv */
        /* Note: because chassis-id and port-id is max 511 bytes each,*/
        /*       the total frame size should never reach the allowed  */
        /*       max ethernet frame size. we insert an assert here..  */
        LLDP_ASSERT(TLength > LLDP_MAX_FRAME_LENGTH);
    }
    else
    {
        LLDP_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_FATAL,"LLDP_ConstructShutdown. TTL not found in frame!");
        LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
    }
    return(TLength);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDRecvProvide                         +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LSA_UINT32                   Cnt            +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Cnt                  : Number of NRT-Recv-Request to send to EDD       +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends NRT-Recv-Requests to EDD                            +*/
/*+               (EDD_SRV_NRT_RECV)                                        +*/
/*+                                                                         +*/
/*+               Allocates RQB and Rx-Frame memory and does EDD-Requests   +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe RECV-Request pending     +*/
/*+                     within EDD!                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDRecvProvide(
    LLDP_HDB_PTR_TYPE        pHDB,
    LSA_UINT32               Cnt)
{


    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_EDD_LOWER_RQB_NRT_RECV_PTR_TYPE        pRecv;
    LSA_SYS_PTR_TYPE                            pSys;
    LLDP_RSP                                    Status;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pRxFrame;  /* Pointer to Rx-Frame memory. */
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDRecvProvide(pHDB: 0x%X, Cnt: 0x%X)",pHDB,Cnt);
    Status = LLDP_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    /*---------------------------------------------------------------------------*/
    /* allocate and send Rx-Request to EDD                                       */
    /*---------------------------------------------------------------------------*/
    pHDB->RxPendingCnt = 0; /* no Rx pending within EDD */
    while ((Status == LLDP_RSP_OK) && (Cnt))
    {

        LLDP_EDD_ALLOC_LOWER_RXMEM(&pRxFrame,
                                   UserId,
                                   LLDP_RX_FRAME_BUFFER_LENGTH,
                                   pSys);

        if ( LSA_HOST_PTR_ARE_EQUAL(pRxFrame, LSA_NULL) )
        {
            Status = LLDP_RSP_ERR_RESOURCE;
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"LLDP_EDDRecvProvide(). Allocation RxFrame memory failed.");
        }
        else
        {
            /* allocate RQB */
            LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

            if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
            {
                LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(EDD_RQB_NRT_RECV_TYPE),pSys);
                pRecv = (LLDP_EDD_LOWER_RQB_NRT_RECV_PTR_TYPE) pAlloc;
                if (!LSA_HOST_PTR_ARE_EQUAL(pRecv, LSA_NULL) )
                {
                    LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
                    LLDP_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_NRT_RECV);
                    LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
                    pRQB->pParam       = pRecv;
                    pRecv->pBuffer     = (EDD_UPPER_MEM_U8_PTR_TYPE) pRxFrame;
                    pRecv->RequestID   = 0; /* not used */

                    LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_NRT_RECV (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
                    LLDP_EDD_REQUEST_LOWER(pRQB,pSys);
                    pHDB->RxPendingCnt++;  /* number of Rx-Request pending within EDD */
                }
                else
                {
                    Status = LLDP_RSP_ERR_RESOURCE;
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower EDD memory failed!");
                    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
                    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
                    LLDP_EDD_FREE_LOWER_RXMEM(&RetVal,pRxFrame,pSys);
                    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
                }
            }
            else
            {
                Status = LLDP_RSP_ERR_RESOURCE;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower EDD RQB-memory failed!");
                LLDP_EDD_FREE_LOWER_RXMEM(&RetVal,pRxFrame,pSys);
                LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        Cnt--;
    }
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDRecvProvide(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDLinkIndProvide                      +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LSA_UINT32                   PortCnt        +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  PortCnt              : Number of Ports                                 +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends NRT-Link Indication Provide Requests to EDD         +*/
/*+               (EDD_SRV_LINK_STATUS_IND_PROVIDE) for PortCnt ports.      +*/
/*+                                                                         +*/
/*+               Allocates RQB and does EDD-Requests                       +*/
/*+                                                                         +*/
/*+               Note: currently we only have on Ind-Resource. Later we may+*/
/*+                     have one per Port (if Edd support this)             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDLinkIndProvide(
    LLDP_HDB_PTR_TYPE        pHDB,
    LSA_UINT32               PortCnt)
{


    LLDP_EDD_LOWER_RQB_PTR_TYPE                   pRQB;
    LLDP_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE pLink;
    LSA_SYS_PTR_TYPE                              pSys;
    LLDP_RSP                                      Status;
    LSA_USER_ID_TYPE                              UserId;
    LSA_UINT16                                    RetVal;
    LSA_UINT16                                    PortID;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                   pAlloc;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDLinkIndProvide(pHDB: 0x%X)",pHDB);
    Status = LLDP_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    PortID = 1;  /* first port */

    /*---------------------------------------------------------------------------*/
    /* allocate and send Request to EDD                                          */
    /*---------------------------------------------------------------------------*/

    pHDB->LinkIndPendingCnt = 0;  /* number of Link-Ind-Request pending within EDD */

    while ((Status == LLDP_RSP_OK) && (PortCnt))
    {
        /* allocate RQB */
        LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
        if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
        {
            LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE),pSys);
            pLink = (LLDP_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pAlloc;
            if (!LSA_HOST_PTR_ARE_EQUAL(pLink, LSA_NULL) )
            {
                LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
                LLDP_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_LINK_STATUS_IND_PROVIDE);
                LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
                pRQB->pParam       = pLink;
                pLink->Status      = EDD_LINK_UNKNOWN;
                pLink->Speed       = EDD_LINK_UNKNOWN;
                pLink->Mode        = EDD_LINK_UNKNOWN;
                pLink->PortID      = PortID;
                LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_LINK_STATUS_IND_PROVIDE (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
                LLDP_EDD_REQUEST_LOWER(pRQB,pSys);
                pHDB->LinkIndPendingCnt++;  /* number of Link-Ind-Request pending within EDD */
            }
            else
            {
                Status = LLDP_RSP_ERR_RESOURCE;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower EDD memory failed!");
                LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
                LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        else
        {
            Status = LLDP_RSP_ERR_RESOURCE;
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower EDD RQB-memory failed!");
        }
        PortCnt--;
        PortID++;
    } /* while */
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDLinkIndProvide(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDGetParams                           +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get Params from EDD (e.g. MAC address)                    +*/
/*+               (Sends EDD_SRV_GET_MAC_ADDR request)                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDGetParams(
    LLDP_HDB_PTR_TYPE        pHDB)
{


    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_EDD_RQB_GET_PARAMS_PTR_TYPE            pGetParams;
    LSA_SYS_PTR_TYPE                            pSys;
    LLDP_RSP                                    Status;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDGetParams(pHandle: 0x%X)",pHDB);


    Status = LLDP_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(*pGetParams),pSys);
        pGetParams = (LLDP_EDD_RQB_GET_PARAMS_PTR_TYPE) pAlloc;
        if (!LSA_HOST_PTR_ARE_EQUAL(pGetParams, LSA_NULL) )
        {
            LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            LLDP_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_GET_PARAMS);
            LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            pRQB->pParam = pGetParams;
            LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PARAMS (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            LLDP_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            Status = LLDP_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        Status = LLDP_RSP_ERR_RESOURCE;
    }
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDGetParams(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDSetMulticast                        +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LSA_UINT8                    Mode           +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Mode                 : Mode:                                           +*/
/*+                         EDD_MULTICAST_ENABLE                            +*/
/*+                         EDD_MULTICAST_DISABLE                           +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set Multicast address for LLDP                            +*/
/*+               (Sends EDD_SRV_MULTICAST request)                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDSetMulticast(
    LLDP_HDB_PTR_TYPE        pHDB,
    LSA_UINT8                Mode)
{


    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_EDD_RQB_MULTICAST_PTR_TYPE             pMC;
    LSA_SYS_PTR_TYPE                            pSys;
    LLDP_RSP                                    Status;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    LLDP_COMMON_MEM_U8_PTR_TYPE                 pTemp;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDSetMulticast(pHandle: 0x%X)",pHDB);

    Status = LLDP_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(*pMC),pSys);
        pMC = (LLDP_EDD_RQB_MULTICAST_PTR_TYPE) pAlloc;
        if (!LSA_HOST_PTR_ARE_EQUAL(pMC, LSA_NULL) )
        {
            LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            LLDP_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_MULTICAST);
            LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            pMC->Mode = Mode;
            /* to prevent tasking error we use a temp-var */
            pTemp = (LLDP_COMMON_MEM_U8_PTR_TYPE) LLDP_BroadcastMac;
            LLDP_COPY_MAC_ADDR_COMMON(pMC->MACAddr.MacAdr,pTemp);
            pRQB->pParam       = pMC;
            LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_MULTICAST (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            LLDP_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            Status = LLDP_RSP_ERR_RESOURCE;
        }
    }
    else 
    {
        Status = LLDP_RSP_ERR_RESOURCE;
    }
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDSetMulticast(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDGetPortParams                       +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LSA_UINT32                   PortID         +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get PortParams from EDD (e.g. MAC address)                +*/
/*+               (Sends EDD_SRV_GET_PORT_PARAMS request)                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDGetPortParams(
    LLDP_HDB_PTR_TYPE        pHDB,
    LSA_UINT32               PortID)
{

    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE       pGetPortParams;
    LSA_SYS_PTR_TYPE                            pSys;
    LLDP_RSP                                    Status;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDGetPortParams(pHandle: 0x%X,PortID: 0x%X)",pHDB,PortID);

    Status = LLDP_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(*pGetPortParams),pSys);
        pGetPortParams = (LLDP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE)pAlloc;
        if (!LSA_HOST_PTR_ARE_EQUAL(pGetPortParams, LSA_NULL) )
        {
            LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            LLDP_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_GET_PORT_PARAMS);
            LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            pGetPortParams->PortID = (LSA_UINT16) PortID;
            pRQB->pParam       = pGetPortParams;
            LLDP_LOWER_TRACE_04(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PORT_PARAMS (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X,PortID: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB,PortID);
            LLDP_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            Status = LLDP_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        Status = LLDP_RSP_ERR_RESOURCE;
    }
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDGetPortParams(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDNrtCancel                           +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Send CANCEL for NRT-Requests                              +*/
/*+               (Sends EDD_SRV_NRT_CANCEL request)                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDNrtCancel(
    LLDP_HDB_PTR_TYPE        pHDB)
{
    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_EDD_LOWER_RQB_NRT_CANCEL_PTR_TYPE      pCancel;
    LSA_SYS_PTR_TYPE                            pSys;
    LLDP_RSP                                    Status;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDNrtCancel(pHandle: 0x%X)",pHDB);
    Status = LLDP_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(*pCancel),pSys);
        pCancel = (LLDP_EDD_LOWER_RQB_NRT_CANCEL_PTR_TYPE)pAlloc;
        if (!LSA_HOST_PTR_ARE_EQUAL(pCancel, LSA_NULL) )
        {
            LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            LLDP_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_NRT_CANCEL);
            LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            pRQB->pParam  = pCancel;
            pCancel->Mode = EDD_NRT_CANCEL_MODE_ALL;
            pCancel->RequestID = 0; /* not used */
            LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_NRT_CANCEL (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            LLDP_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            Status = LLDP_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        Status = LLDP_RSP_ERR_RESOURCE;
    }
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDNrtCancel(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDOpenChannelDone                     +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Open Channel done handling.                               +*/
/*+                                                                         +*/
/*+               - save EDD-Channel                                        +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDOpenChannelDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{

    LSA_UINT16               RetVal;
    LLDP_RSP                 Response;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDOpenChannelDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB, pHDB);
    Response = LLDP_RSP_OK;

    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    switch ( LLDP_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Open Channel successful.");
            /* save channel */
            pHDB->EDDHandle = ((LLDP_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE )pRQB->pParam)->Handle;
            break;
        case EDD_STS_ERR_RESOURCE:
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Open Channel failed. No resources[0x%X].",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            Response = LLDP_RSP_ERR_RESOURCE;
            break;
        default:
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Open Channel failed.[0x%X].",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            Response = LLDP_RSP_ERR_LL;
            break;
    }
    LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_UserChannelStateMachine(pHDB,Response); /* further actions taken in channel handler */
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDOpenChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDCloseChannelDone                    +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Close Channel done handling.                              +*/
/*+                                                                         +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDCloseChannelDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{

    LSA_UINT16               RetVal;
    LLDP_RSP                 Response;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    Response = LLDP_RSP_OK;
    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));
    if ( LLDP_EDD_RQB_GET_RESPONSE(pRQB) == EDD_STS_OK )
    {
        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Close Channel successful. [released TraceID %d]",pHDB->TraceIdx);
        pHDB->TraceIdx = 0;
    }
    else
    {
        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Close Channel failed.[0x%X].",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
        Response = LLDP_RSP_ERR_LL;
    }
    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_UserChannelStateMachine(pHDB,Response); /* further actions taken in channel handler */
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDCloseChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDGetParamsDone                       +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get Params done handling                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDGetParamsDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{

    LLDP_RSP                 Response;
    LSA_UINT16               RetVal;

    LLDP_FUNCTION_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDGetParamsDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    LLDP_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_GET_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = LLDP_RSP_OK;
    switch (LLDP_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/

            LLDP_COPY_MAC_ADDR_LOC_LER(&pHDB->MACAddr,((LLDP_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr);
            pHDB->PortCnt = ((LLDP_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MaxPortCnt;
            LLDP_PROGRAM_TRACE_06(0,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PARAMS MAC: %2X-%2X-%2X-%2X-%2X-%2X",
                                  pHDB->MACAddr[0],
                                  pHDB->MACAddr[1],
                                  pHDB->MACAddr[2],
                                  pHDB->MACAddr[3],
                                  pHDB->MACAddr[4],
                                  pHDB->MACAddr[5]);
            pHDB->TraceIdx = ((LLDP_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->TraceIdx;
            LLDP_PROGRAM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PARAMS PortCnt: %d [trace ID: %d]",pHDB->PortCnt, pHDB->TraceIdx);
            break;

        default:
            LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: GET PARAMS failed.(0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            Response = LLDP_RSP_ERR_LL;
            break;
    }
    LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_UserChannelStateMachine(pHDB,Response); /* further actions taken in channel handler */
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDGetParamsDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDSetMulticastDone                    +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set Multicast done handling                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDSetMulticastDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{
    LLDP_RSP                 Response;
    LSA_UINT16               RetVal;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDSetMulticastDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_MULTICAST done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = LLDP_RSP_OK;
    switch (LLDP_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD_SRV_MULTICAST successful.");
            break;

        default:
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: EDD_SRV_MULTICAST failed.(0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            Response = LLDP_RSP_ERR_LL;
            break;
    }
    LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_UserChannelStateMachine(pHDB,Response); /* further actions taken in channel handler */
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDSetMulticastDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDGetPortParamsDone                   +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get Params done handling                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDGetPortParamsDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{

    LLDP_RSP                                 Response;
    LSA_UINT16                               RetVal;
    LLDP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE    pGetPortParams;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDGetPortParamsDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);

    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_GET_PORT_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = LLDP_RSP_OK;

    switch (LLDP_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/

            pGetPortParams = (LLDP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE)pRQB->pParam;
            LLDP_ASSERT_NULL_PTR(pGetPortParams);
            LLDP_ASSERT(pGetPortParams->PortID == 0);
            LLDP_ASSERT(pGetPortParams->PortID > pHDB->PortCnt);

            /* check if portMAC present (<> 0) */
            if (( pGetPortParams->MACAddr.MacAdr[0] == 0) &&
                ( pGetPortParams->MACAddr.MacAdr[1] == 0) &&
                ( pGetPortParams->MACAddr.MacAdr[2] == 0) &&
                ( pGetPortParams->MACAddr.MacAdr[3] == 0) &&
                ( pGetPortParams->MACAddr.MacAdr[4] == 0) &&
                ( pGetPortParams->MACAddr.MacAdr[5] == 0))
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PORT PARAMS PortMAC is NOT present! using interface MAC");
                /* store InterfaceMAC address */
                LLDP_COPY_MAC_ADDR_LOC_LER(&pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr,pHDB->MACAddr);
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PORT PARAMS PortMAC is present");
                /* store PortMAC address */
                LLDP_COPY_MAC_ADDR_LOC_LER(&pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr,pGetPortParams->MACAddr.MacAdr);
            }
            LLDP_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PORT PARAMS MAC: %2X-%2X-%2X-%2X-%2X-%2X",
                                  pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr[0],
                                  pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr[1],
                                  pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr[2],
                                  pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr[3],
                                  pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr[4],
                                  pHDB->pPCtrl[pGetPortParams->PortID-1].MACAddr[5]);
            break;

        default:
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: GET PORT PARAMS failed.(0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            Response = LLDP_RSP_ERR_LL;
            break;
    }
    LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_UserChannelStateMachine(pHDB,Response); /* further actions taken in channel handler */
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDGetPortParamsDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDNrtCancelDone                       +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Cancel done handling                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDNrtCancelDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{
    LLDP_RSP                 Response;
    LSA_UINT16               RetVal;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDNrtCancelDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_NRT_CANCEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = LLDP_RSP_OK;
    switch (LLDP_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            break;

        default:
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: Cancel failed.(0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            Response = LLDP_RSP_ERR_LL;
            break;
    }
    LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_UserChannelStateMachine(pHDB,Response); /* further actions taken in channel handler */
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDNrtCancelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDNrtSendDone                         +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Done Sending NRT-Frame (actually an LLDP-Frame)           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDNrtSendDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{
    LLDP_RSP                 Response;
    LSA_UINT16               RetVal;
    LLDP_EDD_PCTRL_PTR_TYPE  pPCtrl;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDNrtSendDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_NRT_SEND done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = LLDP_RSP_OK;
    /* user-id used as Index to PortControl array */
    if (LLDP_EDD_RQB_GET_USERID_UVAR32(pRQB) >= pHDB->PortCnt )
    {
        LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
    }
    pHDB->TxPendingCnt--;
    pPCtrl = &pHDB->pPCtrl[LLDP_EDD_RQB_GET_USERID_UVAR32(pRQB)];
    pPCtrl->Tx.Underway = LSA_FALSE;
    switch (LLDP_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Send successful.",pHDB->ThisHandle,pPCtrl->ThisIdx);
            pPCtrl->Stats.TxFramesOutTotal++;
            break;

        case EDD_STS_ERR_TX:
        case EDD_STS_ERR_TIMEOUT:
            /* ---------------------------------------------*/
            /* On Error we continue                         */
            /* ---------------------------------------------*/
            LLDP_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD[%X:%d]-ERROR: NRT-SEND failed, but continue (0x%X).",pHDB->ThisHandle,pPCtrl->ThisIdx,LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            break;

        default:
            /* ---------------------------------------------*/
            /* On unexpected error we signal a fatal error  */
            /* ---------------------------------------------*/
            LLDP_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD[%X:%d]-ERROR: NRT-SEND failed unexpected (0x%X).",pHDB->ThisHandle,pPCtrl->ThisIdx,LLDP_EDD_RQB_GET_RESPONSE(pRQB));
            LLDP_FATAL(LLDP_FATAL_ERR_EDD);
            break;
    }
    #ifndef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    LLDP_EDD_FREE_LOWER_TXMEM(&RetVal,((LLDP_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE)pRQB->pParam)->pBuffer,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    #endif
    LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    /* Call send done handling */
    LLDP_UserHandleTxDone(pHDB,pPCtrl);
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDNrtSendDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDNrtRecvDone                         +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: EDD Recv-Request done (Frame received)                    +*/
/*+                                                                         +*/
/*+               Copy received frame infos and content to temporary buffer +*/
/*+               within global management (RxFrame) for further handling.  +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides Receveive-Request to EDD                       +*/
/*+                                                                         +*/
/*+               If Recv-Request was canceled we free the RQB/Frame and    +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDNrtRecvDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{
    LLDP_RSP                                    Response;
    LSA_UINT16                                  RetVal;
    LSA_BOOL                                    Reprovide;
    LLDP_GLOB_PTR_TYPE                          pGlob;
    LLDP_EDD_LOWER_RQB_NRT_RECV_PTR_TYPE        pRecv;
    LLDP_EDD_PCTRL_PTR_TYPE                     pPCtrl;
    LLDP_COMMON_MEM_U8_PTR_TYPE                 pTemp;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDNrtRecvDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_NRT_RECV done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = LLDP_RSP_OK;
    Reprovide        = LSA_TRUE;
    pRecv = (LLDP_EDD_LOWER_RQB_NRT_RECV_PTR_TYPE) pRQB->pParam;
    LLDP_ASSERT_NULL_PTR(pRecv);
    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the frame and dont reprovide it. This is the case if we are    */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid frame while closing, so CANCEL   */
    /*       may not be enough to catch all rqbs. We have to check the State too */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Recv-Request will be provided in the last */
    /*       State while opening the channel.                                    */
    if ( pHDB->State == LLDP_HANDLE_STATE_OPEN )
    {
        switch (LLDP_EDD_RQB_GET_RESPONSE(pRQB))
        {
            case EDD_STS_OK:
                /* ---------------------------------------------*/
                /* Successfully done the request.               */
                /* ---------------------------------------------*/
                /* copy framedata to temp-buffer */
                pGlob = LLDP_MGM_GET_GLOB_PTR();
                if (( pRecv->PortID > 0 ) && ( pRecv->PortID <= pHDB->PortCnt ))
                {
                    if ( pRecv->IOCount > LLDP_RX_FRAME_BUFFER_LENGTH )
                    {
                        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"Unexpected Rx-Framelength (0x%X)!",pRecv->IOCount);
                    }
                    else
                    {
                        pPCtrl = &pHDB->pPCtrl[pRecv->PortID-1];
                        LLDP_PROGRAM_TRACE_05(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-LLDP-Frame Received (Handle: 0x%X, EDD-Handle: 0x%X,Port: 0x%X, pFrame: 0x%X, Len: 0x%X)",
                                              pHDB->ThisHandle,
                                              pHDB->EDDHandle,
                                              pPCtrl->ThisIdx,
                                              pRecv->pBuffer,
                                              pRecv->IOCount);
                        /* check if this is the LLDP-Multicast MAC. If not discard */
                        /* frame (see llpd-spec)                                   */
                        /* to prevent tasking error we use a temp-var */
                        pTemp = (LLDP_COMMON_MEM_U8_PTR_TYPE) LLDP_BroadcastMac;
                        if ( LLDP_MEMCMP_COMMON(
                                 (LLDP_COMMON_MEM_PTR_TYPE) &pRecv->pBuffer[LLDP_FRAME_DST_MAC_OFFSET],
                                 (LLDP_COMMON_MEM_PTR_TYPE) pTemp,
                                 LLDP_MAC_ADDR_SIZE) )
                        {
                            pPCtrl->Stats.RxFramesInTotal++;
                            /* Handle the received frame */
                            LLDP_UserHandleRxFrame(pHDB,
                                                   pPCtrl,
                                                   pRecv->pBuffer,
                                                   pRecv->UserDataOffset,
                                                   pRecv->UserDataLength);
                        }
                        else
                        {
                            /* invalid DST-MAC. discard frame */
                            pPCtrl->Stats.RxInvalidDstMAC++;
                        }
                    } /* else */
                }
                break;

            case EDD_STS_ERR_RX:
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,"EDD Receive error.(0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
                break;

            case EDD_STS_OK_CANCEL:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> LLDP_HANDLE_STATE_OPEN */
                /*       while closing!                             */
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDD Receive canceled (0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            default:
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDD-ERROR: EDD_SRV_NRT_RECV failed.(0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
                LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                break;
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDD Receive while not in open-state. discard and dont reprovide.");
        Reprovide = LSA_FALSE;
    }

    if ( Reprovide )
    {
        LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_NRT_RECV (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
        LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle); /* set handle */
        LLDP_EDD_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        pHDB->RxPendingCnt--;  /* number of Rx-Request pending within EDD */
        LLDP_EDD_FREE_LOWER_RXMEM(&RetVal,pRecv->pBuffer,pHDB->Params.pSys);
        LLDP_MEM_FREE_FAIL_CHECK(RetVal);
        LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
        LLDP_MEM_FREE_FAIL_CHECK(RetVal);
        LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDNrtRecvDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDLinkIndDone                         +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: EDD LinkStatus Indication Request done (Link changed)     +*/
/*+                                                                         +*/
/*+               Copy link-status to port-managment                        +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides request to EDD                                 +*/
/*+                                                                         +*/
/*+               If Request was canceled we free the RQB and               +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_EDDLinkIndDone(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    LLDP_HDB_PTR_TYPE            pHDB)
{
    LLDP_RSP                                         Response;
    LSA_UINT16                                       RetVal;
    LSA_BOOL                                         Reprovide;
    LLDP_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE    pLink;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDLinkIndDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_LINK_STATUS_IND_PROVIDE done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,LLDP_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = LLDP_RSP_OK;
    Reprovide        = LSA_TRUE;
    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the request and dont reprovide it. This is the case if we are  */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid rqb while closing, so CANCEL     */
    /*       may not be enough to catch the rqb. We have to check the State too  */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Ind-Request will be provided in the last  */
    /*       State while opening the channel.                                    */
    if ( pHDB->State == LLDP_HANDLE_STATE_OPEN )
    {
        switch (LLDP_EDD_RQB_GET_RESPONSE(pRQB))
        {
            case EDD_STS_OK:
                /* ---------------------------------------------*/
                /* Successfully done the request.               */
                /* ---------------------------------------------*/
                pLink = (LLDP_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;
                LLDP_PROGRAM_TRACE_05(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDD Link (Ch: %d,Port: %d): St/Sp/M: %d/%d/%d",pHDB->ThisHandle,pLink->PortID,pLink->Status,pLink->Speed,pLink->Mode);
                if (( pLink->PortID > 0 ) && (pLink->PortID <= pHDB->PortCnt ))
                {
                    /* all EDD-Link-Up-States will be mapped to EDD_LINK_UP */
                    if (( pLink->Status != EDD_LINK_DOWN ) && ( pLink->Status != EDD_LINK_UNKNOWN ))
                    {
                        pHDB->pPCtrl[pLink->PortID-1].LinkStatus.Status = EDD_LINK_UP;
                    }
                    else
                    {
                        pHDB->pPCtrl[pLink->PortID-1].LinkStatus.Status = EDD_LINK_DOWN;
                    }
                    pHDB->pPCtrl[pLink->PortID-1].LinkStatus.Speed  = pLink->Speed;
                    pHDB->pPCtrl[pLink->PortID-1].LinkStatus.Mode   = pLink->Mode;
                    /* Handle the Link Status Change for this port */
                    LLDP_UserHandleLinkInd(pHDB,&pHDB->pPCtrl[pLink->PortID-1]);
                }
                break;

            case EDD_STS_OK_CANCEL:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> LLDP_HANDLE_STATE_OPEN */
                /*       while closing!                             */
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDD LinkInd canceled (0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            default:
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDD-ERROR: EDD_SRV_LINK_STATUS_IND_PROVIDE failed.(0x%X).",LLDP_EDD_RQB_GET_RESPONSE(pRQB));
                LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                break;
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDD LinkInd while not in open-state. discard and don't reprovide.");
        Reprovide = LSA_FALSE;
    }
    if ( Reprovide )
    {
        LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_LINK_STATUS_IND_PROVIDE (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
        LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle); /* set handle */
        LLDP_EDD_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        pHDB->LinkIndPendingCnt--;  /* number of LinkStatus-Request pending within EDD */
        LLDP_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
        LLDP_MEM_FREE_FAIL_CHECK(RetVal);
        LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        LLDP_MEM_FREE_FAIL_CHECK(RetVal);
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDLinkIndDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDRQBCopyLLErrorLocal                 +*/
/*+  Input                      LSA_RQB_LOCAL_ERROR_TYPE  NA...* pLLError   +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copys LL-Error to local structur if something present.    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_EDDRQBCopyLLErrorLocal(
    LSA_RQB_LOCAL_ERROR_TYPE   LLDP_EDD_LOWER_RQB_ATTR * pLLError)
{
    /* copy only if something filled in.. */
    if ( pLLError->lsa_component_id != LSA_COMP_ID_UNUSED)
    {
        LLDP_MGM_GET_GLOB_PTR()->LLError = * pLLError;
    }
    else
    {
        LLDP_MGM_GET_GLOB_PTR()->LLError.lsa_component_id = LSA_COMP_ID_UNUSED;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    lldp_edd_request_lower_done                 +*/
/*+  Input                      LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for EDD-requests.                        +*/
/*+               Fills Error-Structure in NDB with EDD-Response            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_EDD_LOWER_IN_FCT_ATTR lldp_edd_request_lower_done(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB)
{

    LLDP_HDB_PTR_TYPE   pHDB;

    LLDP_ENTER();
    LLDP_ASSERT_NULL_PTR(pRQB);
    /* EDD-request always done from an EDD-Channel. !           */
    pHDB = LLDP_GetUserHDBFromHandle(LLDP_EDD_RQB_GET_HANDLE(pRQB));
    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : lldp_edd_request_lower_done(pRQB: 0x%X)",pRQB);
    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        LLDP_FATAL(LLDP_FATAL_ERR_HANDLE_INVALID);
    }
    switch (LLDP_EDD_RQB_GET_OPCODE(pRQB) )
    {
        case EDD_OPC_OPEN_CHANNEL:
            LLDP_EDDOpenChannelDone(pRQB,pHDB);
            break;
        case EDD_OPC_CLOSE_CHANNEL:
            LLDP_EDDCloseChannelDone(pRQB,pHDB);
            break;
        case EDD_OPC_REQUEST:
            switch ( LLDP_EDD_RQB_GET_SERVICE(pRQB))
            {
                case EDD_SRV_NRT_SEND:
                    LLDP_EDDNrtSendDone(pRQB,pHDB);
                    break;
                case EDD_SRV_NRT_RECV:
                    LLDP_EDDNrtRecvDone(pRQB,pHDB);
                    break;
                case EDD_SRV_LINK_STATUS_IND_PROVIDE:
                    LLDP_EDDLinkIndDone(pRQB,pHDB);
                    break;
                case EDD_SRV_GET_PARAMS:
                    LLDP_EDDGetParamsDone(pRQB,pHDB);
                    break;
                case EDD_SRV_GET_PORT_PARAMS:
                    LLDP_EDDGetPortParamsDone(pRQB,pHDB);
                    break;
                case EDD_SRV_MULTICAST:
                    LLDP_EDDSetMulticastDone(pRQB,pHDB);
                    break;
                case EDD_SRV_NRT_CANCEL:
                    LLDP_EDDNrtCancelDone(pRQB,pHDB);
                    break;
                default:
                    LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
                    break;
            }
            break;
        default:
            LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
            break;
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: lldp_edd_request_lower_done()");
    LLDP_EXIT();
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDOpenChannel                         +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE       pHDB                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : User HDB                                        +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to EDD. The Request will be   +*/
/*+               finished by calling the EDD-Callbackfunktion.             +*/
/*+               lldp_edd_request_lower_done() if LLDP_RSP_OK.             +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDOpenChannel(
    LLDP_HDB_PTR_TYPE        pHDB)
{
    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE    pOpen;
    LLDP_RSP                                    Result;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDOpenChannel(pHDB: 0x%X)",pHDB);
    Result = LLDP_RSP_OK;
    UserId.uvar32 = 0;
    LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pHDB->Params.pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(EDD_RQB_OPEN_CHANNEL_TYPE),pHDB->Params.pSys);
        pOpen = (LLDP_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE)pAlloc;
        if (!LSA_HOST_PTR_ARE_EQUAL(pOpen, LSA_NULL) )
        {
            LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_OPEN_CHANNEL);
            LLDP_EDD_RQB_SET_SERVICE(pRQB,0);
            pRQB->pParam       = pOpen;
            pOpen->HandleUpper = pHDB->ThisHandle;
            pOpen->SysPath     = pHDB->Params.SysPath;
            pOpen->Cbf         = lldp_edd_request_lower_done;
            LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pRQB);
            LLDP_EDD_OPEN_CHANNEL_LOWER(pRQB,pHDB->Params.pSys);
        }
        else
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
            LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
            LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            Result = LLDP_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
        Result = LLDP_RSP_ERR_RESOURCE;
    }
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDOpenChannel. Rsp: 0x%X",Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDCloseChannel                        +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE       pHDB                +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+                             LLDP_RSP_ERR_RESOURCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+  pHDB                 : User HDB                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to EDD. The Request will be  +*/
/*+               finished by calling the EDD-Callbackfunktion.             +*/
/*+               LLDP_edd_request_lower_done()                             +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDCloseChannel(
    LLDP_HDB_PTR_TYPE        pHDB)
{
    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_RSP                                    Result;
    LSA_USER_ID_TYPE                            UserId;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDCloseChannel(pHDB: 0x%X)",pHDB);
    Result = LLDP_RSP_OK;
    UserId.uvar32 = 0;
    LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pHDB->Params.pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_CLOSE_CHANNEL);
        LLDP_EDD_RQB_SET_SERVICE(pRQB,0);
        LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
        pRQB->pParam = LSA_NULL;
        LLDP_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_OPC_CLOSE_CHANNEL (EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->EDDHandle,pRQB);
        LLDP_EDD_CLOSE_CHANNEL_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
        Result = LLDP_RSP_ERR_RESOURCE;
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDCloseChannel()");
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_EDDSendLLDPFrame                       +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl         +*/
/*+                             LSA_BOOL                     Shutdown       +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pPCtrl               : PortControlBlock                                +*/
/*+  Shutdown             : LSA_FALSE: Send user-Data frame                 +*/
/*+                         LSA_TRUE:  Send Shutdown frame                  +*/
/*+                                                                         +*/
/*+  Result               : LLDP_RSP_OK                                     +*/
/*+                         LLDP_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends LLDP-Frame for port.                                +*/
/*+                                                                         +*/
/*+               If Shutdown = LSA_FALSE, we use the actual user data frame+*/
/*+               to send (which may have changed since last send)          +*/
/*+                                                                         +*/
/*+               If Shutdown = LSA_TRUE we send a Shutdown frame.          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               Note: The function must only be called if no send is      +*/
/*+                     underway! (pPCtrl->Tx.Underway)                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDSendLLDPFrame(
    LLDP_HDB_PTR_TYPE            pHDB,
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl,
    LSA_BOOL                     Shutdown)
{
    LLDP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    LLDP_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE        pSend;
    LSA_SYS_PTR_TYPE                            pSys;
    LLDP_RSP                                    Result;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    LSA_UINT32                                  FrameLen;
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pTxFrame;  /* Pointer to Tx-Frame memory. */
    LLDP_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_EDDSendLLDPFrame(pHDB: 0x%X, pPCtrl: 0x%X)",pHDB,pPCtrl);

    LLDP_ASSERT_NULL_PTR(pPCtrl);
    LLDP_ASSERT(pPCtrl->Tx.Underway);

    Result = LLDP_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    pPCtrl->Tx.DoSend = LSA_FALSE;  /* we try to send, so reset the flag */

    /*---------------------------------------------------------------------------*/
    /* allocate or get preallocated Framebuffer ptr.                             */
    /*---------------------------------------------------------------------------*/
    #ifndef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    LLDP_EDD_ALLOC_LOWER_TXMEM(&pTxFrame,
                               UserId,
                               (LSA_UINT16) pPCtrl->Tx.DataLen,
                               pSys);

    if ( LSA_HOST_PTR_ARE_EQUAL(pTxFrame, LSA_NULL) )
    {
        Result = LLDP_RSP_ERR_RESOURCE;
        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"LLDP_EDDSendLLDPFrame(). Allocation pTxFrame memory failed.");
        pPCtrl->Stats.TxNoMemResources++;
    }

    #else
    pTxFrame = pPCtrl->Tx.pFrame; /* get Frame-Pointer from PCTRL-Block */
    LLDP_ASSERT_NULL_PTR(pTxFrame);
    #endif

    /*---------------------------------------------------------------------------*/
    /* Allocate EDD-Send RQB, fill Request and send request to EDD               */
    /*---------------------------------------------------------------------------*/
    if ( Result == LLDP_RSP_OK )
    {
        /* copy Frame to Sendbuffer. If we have a static buffer we only need to  */
        /* copy if we have new data.                                             */
        #ifdef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
        if ( pPCtrl->Tx.DataNew )
            #endif
        {
            LLDP_MEMCOPY_COMMON(
                (LLDP_COMMON_MEM_PTR_TYPE) pTxFrame,
                (LLDP_COMMON_MEM_PTR_TYPE) pPCtrl->Tx.Frame,
                pPCtrl->Tx.DataLen);
        }
        pPCtrl->Tx.DataNew = LSA_FALSE;
        FrameLen           = pPCtrl->Tx.DataLen;
        if ( Shutdown )
        {
            /* construct the Shutdown frame from the current frame        */
            /* Note. DataNew will be set to LSA_TRUE if port is restarted */
            FrameLen = LLDP_ConstructShutdown((LLDP_COMMON_MEM_U8_PTR_TYPE)pTxFrame,pPCtrl->Tx.DataLen);
            LLDP_ASSERT((FrameLen == 0 ));
        }
        LLDP_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
        if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
        {
            LLDP_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(EDD_RQB_NRT_SEND_TYPE),pSys);
            pSend = (LLDP_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE)pAlloc;
            if (!LSA_HOST_PTR_ARE_EQUAL(pSend, LSA_NULL) )
            {
                LLDP_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
                LLDP_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_NRT_SEND);
                LLDP_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
                /* we use User-ID for PCtrl-identification */
                LLDP_EDD_RQB_SET_USERID_UVAR32(pRQB,pPCtrl->ThisIdx);
                pRQB->pParam       = pSend;
                pSend->Length      = pPCtrl->Tx.DataLen;
                pSend->pBuffer     = (EDD_UPPER_MEM_U8_PTR_TYPE) pTxFrame;

                pSend->Priority    = EDD_NRT_SEND_PRIO_MGMT_LOW;
                pSend->PortID      = (LSA_UINT16) pPCtrl->ThisIdx+1;
                pSend->TxFrmGroup  = EDD_NRT_TX_GRP_LLDP;
                pPCtrl->Tx.Underway = LSA_TRUE; /* will be set to FALSE on confirmation */
                pHDB->TxPendingCnt++;
                LLDP_PROGRAM_TRACE_05(pHDB->TraceIdx,
                                      LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Sending LLDP-Frame(Handle: 0x%X, EDD-Handle: 0x%X,Port: 0x%X, pFrame: 0x%X, Len: 0x%X)",
                                      pHDB->ThisHandle,
                                      pHDB->EDDHandle,
                                      pPCtrl->ThisIdx,
                                      pTxFrame,
                                      pPCtrl->Tx.DataLen);
                LLDP_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_NRT_SEND (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
                LLDP_EDD_REQUEST_LOWER(pRQB,pSys);
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
                LLDP_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
                LLDP_MEM_FREE_FAIL_CHECK(RetVal);
                #ifndef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
                LLDP_EDD_FREE_LOWER_TXMEM(&RetVal,pTxFrame,pSys);
                LLDP_MEM_FREE_FAIL_CHECK(RetVal);
                #endif
                Result = LLDP_RSP_ERR_RESOURCE;
                pPCtrl->Stats.TxNoMemResources++;
            }
        }
        else
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            #ifndef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
            LLDP_EDD_FREE_LOWER_TXMEM(&RetVal,pTxFrame,pSys);
            LLDP_MEM_FREE_FAIL_CHECK(RetVal);
            #endif
            Result = LLDP_RSP_ERR_RESOURCE;
            pPCtrl->Stats.TxNoMemResources++;
        }
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_EDDSendLLDPFrame()");
    return(Result);
}

/*****************************************************************************/
/*  end of file LLDP_EDD.C                                                    */
/*****************************************************************************/

