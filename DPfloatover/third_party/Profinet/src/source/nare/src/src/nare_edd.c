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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_edd.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE EDD functions                               */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  8
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_EDD */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "nare_inc.h"            /* NARE headerfiles */
#include "nare_int.h"            /* internal header */

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

const NARE_MAC_TYPE NARE_EDDBroadcastMac = {0xff,0xff,0xff,0xff,0xff,0xff};
const NARE_MAC_TYPE NARE_EDDNullMac      = {0,0,0,0,0,0};

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/
LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDNrtSendDone(
    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_EDD_PTR_TYPE        pHDB);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_ARPResponseFrameIn                     +*/
/*+  Input                      LSA_UINT16                      NICId       +*/
/*+                             NARE_EDD_LOWER_MEM_PTR_TYPE     pFrame      +*/
/*+                             LSA_UINT16                      Framelen    +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NIC-ID from EDD-Systemchannel                             +*/
/*+  pFrame     : ARP-Response Frame.                                       +*/
/*+               we check for valid ARP-Frame and discard all others.      +*/
/*+  FrameLen   : Length of Frame                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: ARP-Sniffer input function.                               +*/
/*+                                                                         +*/
/*+               Called by Systemadaption with ARP-Response Frames to be   +*/
/*+               sniffed by NARE.                                          +*/
/*+                                                                         +*/
/*+               We check if the frame contains a response we waiting for  +*/
/*+               (ip-address within response) if so we check the MAC and   +*/
/*+               if we got a response from a MAC we dont expect or we got  +*/
/*+               to responses from different MACs we set a error-flag.     +*/
/*+                                                                         +*/
/*+               If we should test a single ip for use we set error-flag   +*/
/*+               if we got at least one response.                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_SYSTEM_IN_FCT_ATTR nare_ARPResponseFrameIn(
    LSA_UINT16                      NICId,
    NARE_EDD_LOWER_MEM_PTR_TYPE     pFrame,
    LSA_UINT16                      FrameLen)
{
    NARE_ARP_FRAME_HEADER_VLAN_PTR_TYPE   pArpFrame;
    NARE_ARP_FRAME_CONTENT_PTR_TYPE       pArpContent;
    LSA_UINT32                            j;
    NARE_HDB_EDD_PTR_TYPE                 pHDB;
    NARE_EDD_RCTRL_PTR_TYPE               pRCtrl;
    NARE_UNION_IP_TYPE                    SenderIP;
    NARE_UNION_MAC_TYPE                   SenderMAC;
    LSA_UINT32                            Length;

#ifdef NARE_CFG_USE_ENTER_EXIT_WITH_SNIFFER
    NARE_ENTER();
#endif

    pArpFrame   = (NARE_ARP_FRAME_HEADER_VLAN_PTR_TYPE)pFrame;
    pHDB        = NARE_EDDGetHandleByNIC(NICId);
    Length      = 0xFFFF;    /* abort length flag */
    pArpContent = LSA_NULL;  /* prevent compiler warning with tasking */
    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        NARE_FATAL(NARE_FATAL_ERR_NIC_INVALID);
    }
    /* --------------------------------------------------------------------------*/
    /* First we check for valid Type. We support VLAN-Tags.                      */
    /* --------------------------------------------------------------------------*/
    if ( (pArpFrame->VLANType == NARE_HTONS(NARE_ARP_TYPE)))
    {
        pArpContent = (NARE_ARP_FRAME_CONTENT_PTR_TYPE) (((NARE_EDD_LOWER_MEM_U8_PTR_TYPE)pArpFrame)
                                                         + sizeof(NARE_ARP_FRAME_HEADER_TYPE));
        Length = NARE_ARP_TX_FRAME_LEN;
    }
    else
    {  /* always handle a VLAN tag AP01513622 */
        if ( (pArpFrame->VLANType == NARE_HTONS(NARE_VLAN_TYPE)) &&
             (pArpFrame->Type     == NARE_HTONS(NARE_ARP_TYPE )))
        {
            pArpContent = (NARE_ARP_FRAME_CONTENT_PTR_TYPE) (((NARE_EDD_LOWER_MEM_U8_PTR_TYPE)pArpFrame)
                                                             + sizeof(NARE_ARP_FRAME_HEADER_VLAN_TYPE));
            Length = NARE_ARP_TX_FRAME_VLAN_LEN;
        }
    }
    /* --------------------------------------------------------------------------*/
    /* If it is a ARP-Frame we continue.                                         */
    /* --------------------------------------------------------------------------*/
    if ( FrameLen >= Length )
    {

        if ((pArpContent->Hrd == NARE_HTONS(NARE_ARP_HRD_ETHERNET)) &&
            (pArpContent->Pro == NARE_HTONS(NARE_ARP_PRO_IP)) &&
            (pArpContent->Hln == NARE_ARP_HLN) &&
            (pArpContent->Pln == NARE_ARP_PLN) &&
            (pArpContent->Op  == NARE_HTONS(NARE_ARP_OP_RESPONSE)) )
        {
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"ARP-Sniffer-Frame received (pFrame: %Xh):",pFrame);
            SenderMAC.w.MAC1234   = 0;         /* prevent compiler warning */
            SenderMAC.w.MAC56     = 0;         /* prevent compiler warning */
            SenderIP.w.IP1234     = 0;         /* prevent compiler warning */
            /* Copy MAC/IP from slow frame-memory to faster one */
            /* and for optimized access method for compare.     */
            NARE_COPY_MAC_ADDR_ST_LEM(SenderMAC.b.MACAddr,pArpContent->Sha);
            NARE_COPY_IP_ADDR_ST_LEM(SenderIP.b.IPAddr,pArpContent->Spa);
            pRCtrl =  &(*pHDB->EDDRCtrl.pTable)[0];
            /* loop through all RCTRL-Blocks */
            for (j=0; j<pHDB->EDDRCtrl.Cnt; j++)
            {   /* if this RCTRL is in use and sniffer active..*/
                if ( pRCtrl->InUse && pRCtrl->Sniffer.DoSniff )
                {   /* if this is a IP we waiting for answer...*/
                    if ( SenderIP.w.IP1234 == pRCtrl->Sniffer.DestIP.w.IP1234)
                    {
                        if ( pRCtrl->Sniffer.Mode == NARE_EDD_ARP_SNIFF_MODE_MULTIPLE )
                        {   /* if we already have a response or we await a special MAC */
                            if ( pRCtrl->Sniffer.CurMACValid )
                            {
                                /* if this MAC differs from the one we await or the last*/
                                /* one we received, we have multiple IP-Situation.      */
                                /* This means either we got a response with a MAC we    */
                                /* dont expect or we got at least two answers with      */
                                /* different MACs                                       */
                                /* due to problems with antgen changed:
                                if (( pRCtrl->Sniffer.CurMAC.w.MAC1234 != SenderMAC.w.MAC1234 ) ||
                                    ( pRCtrl->Sniffer.CurMAC.w.MAC56   != SenderMAC.w.MAC56   )) to */
                                if( (pRCtrl->Sniffer.CurMAC.b.MACAddr[0] != SenderMAC.b.MACAddr[0])  ||
                                    (pRCtrl->Sniffer.CurMAC.b.MACAddr[1] != SenderMAC.b.MACAddr[1]) ||
                                    (pRCtrl->Sniffer.CurMAC.b.MACAddr[2] != SenderMAC.b.MACAddr[2]) ||
                                    (pRCtrl->Sniffer.CurMAC.b.MACAddr[3] != SenderMAC.b.MACAddr[3]) ||
                                    (pRCtrl->Sniffer.CurMAC.b.MACAddr[4] != SenderMAC.b.MACAddr[4]) ||
                                    (pRCtrl->Sniffer.CurMAC.b.MACAddr[5] != SenderMAC.b.MACAddr[5])
                                )
                                {
                                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-ARP Sniffer: Multiple IP usage mismatch occured",pHDB->ThisHandle,pRCtrl->ThisIdx);
                                    /* set error-flag for main-handling */
                                    pRCtrl->Sniffer.MultipleRsp = LSA_TRUE;
                                }
                            }
                            else
                            {   /* first response and no MAC to test yet. save this MAC */
                                pRCtrl->Sniffer.CurMACValid = LSA_TRUE;
                                pRCtrl->Sniffer.CurMAC.w.MAC1234 = SenderMAC.w.MAC1234;
                                pRCtrl->Sniffer.CurMAC.w.MAC56   = SenderMAC.w.MAC56;
                            }
                        }
                        else /* mode : NARE_EDD_ARP_SNIFF_MODE_SINGLE */
                        {
                            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-ARP Sniffer: Test IP is in use.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                            /* save on MAC-Addr to return it to user.. */
                            if ( ! pRCtrl->Sniffer.CurMACValid )
                            {
                                pRCtrl->Sniffer.CurMACValid = LSA_TRUE;
                                pRCtrl->Sniffer.CurMAC.w.MAC1234 = SenderMAC.w.MAC1234;
                                pRCtrl->Sniffer.CurMAC.w.MAC56   = SenderMAC.w.MAC56;
                            }
                            /* we dont await any response. so this is a error */
                            pRCtrl->Sniffer.MultipleRsp = LSA_TRUE;
                        }
                    }
                }
                pRCtrl++;
            }
        }
    }
    /* give frame back to systemadaption */
    NARE_ARP_RESPONSE_FRAME_OUT(pHDB->Params.pDetail->Params.Edd.NICId,pFrame,FrameLen);
#ifdef NARE_CFG_USE_ENTER_EXIT_WITH_SNIFFER
    NARE_EXIT();
#endif
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDTimerCB                             +*/
/*+  Input                      NARE_INTERNAL_REQUEST_PTR_TYPE   pIntRQB    +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to internal RQB-mgm.                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles Timerevent for EDD. Called by NARE-Timerhandler   +*/
/*+                                                                         +*/
/*+               It can be configured if we should call the output macro   +*/
/*+               NARE_DO_TIMER_REQUEST for doing a context change or if    +*/
/*+               we should do the handling inside the function.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDTimerCB(
    NARE_INTERNAL_REQUEST_PTR_TYPE   pIntRQB)
{
#ifndef NARE_CFG_HANDLE_DO_TIMEOUT_INTERN
    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup! */
        pIntRQB->InUse = LSA_TRUE;
        NARE_DO_TIMER_REQUEST(pIntRQB->pRQB);
    }
#else
    NARE_EDDTimerEventARP();
#endif
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDFinishARP                           +*/
/*+  Input                      NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+                             NARE_EDD_RCTRL_PTR_TYPE      pRCtrl         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  pRCtrl     : Pointer RCTRl-Block with request to finish                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishs ARP Sniffing for this RCTRL-Block.                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDFinishARP(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_EDD_RCTRL_PTR_TYPE      pRCtrl)
{
    NARE_RSP                 Response;
    NARE_USER_RCTRL_PTR_TYPE pRCtrlUser;

    pRCtrl->Sniffer.DoSniff = LSA_FALSE;
    pRCtrlUser = pRCtrl->pRCtrl;
    NARE_ASSERT_NULL_PTR(pRCtrlUser);
    Response = NARE_RSP_OK;
    switch ( NARE_RQB_GET_OPCODE(pRCtrlUser->pRQB))
    {
        case NARE_OPC_RESOLVE_CONFIG:
            if ( pRCtrl->Sniffer.MultipleRsp )  /* Multiple Stations detected by Sniffer */
            {   /* copy the Sniffer-Mac/IP to the Response-field of user-RQB */
                /* so we give the user a MAC/IP for error-tracking.          */
                /* (this maybe the MAC already put into the resp-field)      */
                if ( ( pRCtrlUser->QueueType == NARE_RCTRL_QTYPE_EDD_RUNNING ) &&  /* only if user-request */
                     ( pRCtrl->Sniffer.CurMACValid )) /* should always be */
                {
                    NARE_COPY_MAC_ADDR_UR_LOC(pRCtrlUser->pRQB->args.ResConf.Res.MACAddr,pRCtrl->Sniffer.CurMAC.b.MACAddr);
                    NARE_COPY_IP_ADDR_UR_LOC(pRCtrlUser->pRQB->args.ResConf.Res.IPAddr,pRCtrl->Sniffer.DestIP.b.IPAddr);
                }
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"EDD[%X:%d]-ARP finished. Multiple IP usage!!",pHDB->ThisHandle,pRCtrl->ThisIdx);
                Response = NARE_RSP_ERR_MULTIPLE_IP_USE;
            }
            else
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-ARP finished. No multiple/test-IP usage.",pHDB->ThisHandle,pRCtrl->ThisIdx);
            }
            break;

        case NARE_OPC_TEST_IP:
            if ( pRCtrl->Sniffer.MultipleRsp )  /* One or Multiple Stations detected by Sniffer */
            {   /* copy errorneous MAC to RQB */
                if ( pRCtrl->Sniffer.CurMACValid ) /* should always be */
                {
                    NARE_COPY_MAC_ADDR_UR_LOC(pRCtrlUser->pRQB->args.TestIP.MACAddr,pRCtrl->Sniffer.CurMAC.b.MACAddr);
                }
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"EDD[%X:%d]-ARP finished. IP in use!!",pHDB->ThisHandle,pRCtrl->ThisIdx);
                Response = NARE_RSP_ERR_IP_IN_USE;
            }
            else
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-ARP finished. No multiple/test-IP usage.",pHDB->ThisHandle,pRCtrl->ThisIdx);
            }
            break;

        case NARE_OPC_RESOLVE_IP:
            if ( pRCtrl->Sniffer.CurMACValid )  /* at least one response */
            {   /* copy MAC to RQB */
                NARE_COPY_MAC_ADDR_UR_LOC(pRCtrlUser->pRQB->args.ResolveIP.MACAddr,pRCtrl->Sniffer.CurMAC.b.MACAddr);
                if ( pRCtrl->Sniffer.MultipleRsp ) /* if more then one response -> error */
                {
                    Response = NARE_RSP_ERR_IP_IN_USE;
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"EDD[%X:%d]-ARP finished. Resolve IP multiple use!!",pHDB->ThisHandle,pRCtrl->ThisIdx);
                }
                else
                {
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-ARP finished. Resolve IP success.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                }
            }
            else
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-ARP finished. Resolve IP no answer.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                Response = NARE_RSP_ERR_IP_RESOLVE_NO_ANSWER;
            }
            break;

        default:
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_EDDFinishARP(): Invalid opcode (%Xh)",NARE_RQB_GET_OPCODE(pRCtrlUser->pRQB));
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */

    /* release the RCTRL-Block */
    NARE_EDDReleaseRCTRL(pHDB,pRCtrl);
    switch ( pRCtrlUser->QueueType )
    {
        case NARE_RCTRL_QTYPE_EDD_RUNNING:
            /* call the main statemachine     */
            NARE_UserStateMachine(pRCtrlUser,Response,LSA_TRUE);
            break;

        case NARE_RCTRL_QTYPE_EDD_OPEN:
            /* further actions taken in channel handler */
            NARE_EDDChannelDone(pHDB,Response);
            break;

        default:
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDTimerEventARP                       +*/
/*+  Input                      ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Timer-Event for ARP occured.                              +*/
/*+                                                                         +*/
/*+               Checks all Handles and all RCTRL-Blocks which are InUse   +*/
/*+               and flagged for sniffing.                                 +*/
/*+                                                                         +*/
/*+               If one timed out we finish the RCTRL-Block handling and   +*/
/*+               check the results of Sniffing. After that we advance to   +*/
/*+               the next state.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDTimerEventARP(LSA_VOID)
{
    LSA_UINT32                 i,j;
    NARE_HDB_EDD_PTR_TYPE      pHDB;
    NARE_EDD_RCTRL_PTR_TYPE    pRCtrl;
    LSA_UINT16                 Counter;
    NARE_HDB_EDD_INFO_PTR_TYPE pHEDD;

    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDTimerEventARP()");
    Counter = ++(NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter); /* advance Timer Counter */
    pHEDD = NARE_MGM_GET_HEDD_PTR();
    pHDB  = &pHEDD->HDB[0];
    /* ---------------------------------------------------------------------------*/
    /* check all channels and all used and sniff-flagged RCTRL-Blocks for ready   */
    /* ---------------------------------------------------------------------------*/
    for (i = 0; i < pHEDD->HandleCnt; i++)
    {   /* timer may be used if OPEN or if we are currently checking IP while  */
        /* opening                                                             */
        if (( pHDB->State == NARE_HANDLE_STATE_OPEN ) ||
            ( pHDB->State == NARE_HANDLE_STATE_SNIFF_OWN_IP_EDD_RUNNING ))
        {
            pRCtrl =  &(*pHDB->EDDRCtrl.pTable)[0];
            for (j=0; j<pHDB->EDDRCtrl.Cnt; j++)
            {
                if ( pRCtrl->InUse && pRCtrl->Sniffer.DoSniff )
                {   /* first check whether to send the next ARP */
                    if ((pRCtrl->Sniffer.TimerEndCounter == Counter) && (pRCtrl->pARP_RQB != LSA_NULL))
                    {
                        if (pRCtrl->ARP_IO_DONE == LSA_TRUE)
                        {
                            if(( pRCtrl->Sniffer.Mode == NARE_EDD_ARP_SNIFF_MODE_SINGLE ) && (pRCtrl->Sniffer.MultipleRsp == LSA_TRUE))
                            {   /* terminate sequence  */
                                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"NARE_EDDTimerEventARP: terminating ARP Rqb at 0x%x",pRCtrl->pARP_RQB);
                                if(pRCtrl->pARP_RQB != LSA_NULL)
                                {   /*we have  to free these blocks */
                                    LSA_UINT16               RetVal;
                                    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB = pRCtrl->pARP_RQB;
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
                                    NARE_EDD_FREE_LOWER_TXMEM(&RetVal,((NARE_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE)pRQB->pParam)->pBuffer,pHDB->Params.pSys);
                                    NARE_MEM_FREE_FAIL_CHECK(RetVal);
#endif
                                    NARE_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
                                    NARE_MEM_FREE_FAIL_CHECK(RetVal);

                                    NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
                                    NARE_MEM_FREE_FAIL_CHECK(RetVal);
                                    pRCtrl->pARP_RQB = LSA_NULL;
                                }
                                if ( ! --pHDB->ARPSnifferCnt )
                                {
                                    NARE_SYSTEM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"-->: EDD[%X:%d]-Stopping Sniffer for NIC: 0x%X.",pHDB->ThisHandle,pRCtrl->ThisIdx,pHDB->Params.pDetail->Params.Edd.NICId);
                                    NARE_ARP_RESPONSE_FRAME_CONTROL(pHDB->Params.pDetail->Params.Edd.NICId,LSA_FALSE);
                                }
                                /* ----------------------------------------------------*/
                                /* If no one uses the timer anymore we stop it         */
                                /* ----------------------------------------------------*/
                                if (( ! --pHEDD->ARPTimer.UserCnt ))
                                {
                                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Stopping ARP Timer");
                                    NARETimerStop(pHEDD->ARPTimer.ID);
                                }
                                NARE_EDDFinishARP(pHDB,pRCtrl);
                            }
                            else
                            {
                                pRCtrl->ARP_IO_DONE = LSA_FALSE;
                                /* Send ARP to EDD */
                                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"--> EDD sending ARP,  next arp-request TimeCount: %d: 0x%x pRQB: %p",Counter,Counter,pRCtrl->pARP_RQB);
                                NARE_EDD_REQUEST_LOWER(pRCtrl->pARP_RQB,pRCtrl->pHDBEDD->Params.pSys);
                                pRCtrl->Sniffer.TimerEndCounter = Counter + 1; /* real time will be set in io-done */
                            }
                        }
                        else
                        {
                            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDD ARP send request stil not done ! pRQB: %p",pRCtrl->pARP_RQB);
                            pRCtrl->Sniffer.TimerEndCounter++;
                        }
                    }
                    else
                    {
                        /* if time elapsed or we already received a response with NARE_EDD_ARP_SNIFF_MODE_SINGLE */
                        /* NARE_EDD_ARP_SNIFF_MODE_SINGLE is used with NARE_OPC_TEST_IP                          */   
                        if (( pRCtrl->Sniffer.TimerEndCounter == Counter ) ||
                            (( pRCtrl->Sniffer.Mode == NARE_EDD_ARP_SNIFF_MODE_SINGLE ) && ( pRCtrl->Sniffer.MultipleRsp ))
                            ) 
                        {   /* special situation. we timed out but      */
                            /* the confirmation from EDD for ARP- send  */
                            /* request still missing. we continue       */
                            /* sniffing till send is ready.             */
                            if ( ! pRCtrl->ARP_IO_DONE )
                            {   /* Note that this may also occur if we  */
                                /* abort sniffing, but nrt not send yet */
                                /* if so, this is no error              */
                                if ( ! pRCtrl->pRCtrl->DoCancel )
                                {
                                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Can not finish sniffing because NRT-Send not finished.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                                }
                                /* wait one more tick */
                                if (pRCtrl->Sniffer.TimerEndCounter == Counter)
                                {
                                    pRCtrl->Sniffer.TimerEndCounter = Counter + 1;
                                }
                            }
                            else
                            {   /* ----------------------------------------------------*/
                                /* this RCTRL is ready. Do finish-handling             */
                                /* ----------------------------------------------------*/
                                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Finished sniffing.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                                /* ----------------------------------------------------*/
                                /* if no more need sniffing for this channel we stop it */
                                /* ----------------------------------------------------*/
                                if ( ! --pHDB->ARPSnifferCnt )
                                {
                                    NARE_SYSTEM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"-->: EDD[%X:%d]-Stopping Sniffer for NIC: 0x%X.",pHDB->ThisHandle,pRCtrl->ThisIdx,pHDB->Params.pDetail->Params.Edd.NICId);
                                    NARE_ARP_RESPONSE_FRAME_CONTROL(pHDB->Params.pDetail->Params.Edd.NICId,LSA_FALSE);
                                }
                                /* ----------------------------------------------------*/
                                /* If no one uses the timer anymore we stop it         */
                                /* ----------------------------------------------------*/
                                if (( ! --pHEDD->ARPTimer.UserCnt ))
                                {
                                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Stopping ARP Timer");
                                    NARETimerStop(pHEDD->ARPTimer.ID);
                                }
                                if(pRCtrl->pARP_RQB != LSA_NULL)
                                {   /*we have  to free these blocks */
                                    LSA_UINT16               RetVal;
                                    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB = pRCtrl->pARP_RQB;
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
                                    NARE_EDD_FREE_LOWER_TXMEM(&RetVal,((NARE_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE)pRQB->pParam)->pBuffer,pHDB->Params.pSys);
                                    NARE_MEM_FREE_FAIL_CHECK(RetVal);
#endif

                                    NARE_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
                                    NARE_MEM_FREE_FAIL_CHECK(RetVal);

                                    NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
                                    NARE_MEM_FREE_FAIL_CHECK(RetVal);
                                    pRCtrl->pARP_RQB = LSA_NULL;
                                }
                                NARE_EDDFinishARP(pHDB,pRCtrl);
                            }
                        }
                    }
                }
                    pRCtrl++;
                }
            }
            pHDB++;
        }
        NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDTimerEventARP()");
    }

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDTimerEventARP                       +*/
/*+  Input                      NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Aborts Sniffing of all RCTRL-Blocks Cancel is set within  +*/
/*+               User-RCTRL.                                               +*/
/*+                                                                         +*/
/*+               We set the Sniffer-End-Timer to actual Timer+1.           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDAbortSniffing(
    NARE_HDB_EDD_PTR_TYPE       pHDB)
{
    LSA_UINT32                 j;
    NARE_EDD_RCTRL_PTR_TYPE    pRCtrl;

    pRCtrl =  &(*pHDB->EDDRCtrl.pTable)[0];
    if (NARE_MGM_GET_HEDD_PTR()->ARPTimer.UserCnt) /* only if Sniffer-Timer runs*/
    {
        for (j=0; j<pHDB->EDDRCtrl.Cnt; j++)
        {
            if (( pRCtrl->InUse ) &&
                ( pRCtrl->Sniffer.DoSniff ) &&
                ( pRCtrl->pRCtrl->DoCancel))
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Set End-Counter of sniffer to cancel.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                /* this will cause to abort sniffing with next timer tick */
                pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16)((NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter)+1);
            }
            pRCtrl++;
        }
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDFindAndFinishSniff                  +*/
/*+  Input                      NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE     pRCtrl         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_TRUE:  Found and finished               +*/
/*+                             LSA_FALSE: Not found or not ready to finish +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Search EDD RCtrls for pRCtrl and if found and             +*/
/*+               currently sniffing and already all ARPs send we           +*/
/*+               finish the request.                                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL  NARE_LOCAL_FCT_ATTR NARE_EDDFindAndFinishSniff(
    NARE_HDB_EDD_PTR_TYPE       pHDB,
    NARE_USER_RCTRL_PTR_TYPE    pRCtrl)

{
    LSA_UINT32                 j;
    LSA_BOOL                   Found;
    LSA_BOOL                   Finished;
    NARE_EDD_RCTRL_PTR_TYPE    pEDDRCtrl;
    NARE_HDB_EDD_INFO_PTR_TYPE pHEDD;

    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDFindAndFinishSniff()");
    Found    = LSA_FALSE;
    Finished = LSA_FALSE;
    pHEDD = NARE_MGM_GET_HEDD_PTR();
    /* ---------------------------------------------------------------------------*/
    /* check all channels and all used and sniff-flagged RCTRL-Blocks for ready   */
    /* ---------------------------------------------------------------------------*/
    pEDDRCtrl =  &(*pHDB->EDDRCtrl.pTable)[0];
    if ( pHEDD->ARPTimer.UserCnt) /* only if Sniffer-Timer runs*/
    {
        j = 0;
        while (( j<pHDB->EDDRCtrl.Cnt ) && ( ! Found )) /* till found or all checked */
        {
            if ( pEDDRCtrl->InUse &&
                 LSA_HOST_PTR_ARE_EQUAL(pRCtrl,pEDDRCtrl->pRCtrl))
            {
                Found = LSA_TRUE;
                if ( pEDDRCtrl->Sniffer.DoSniff &&
                     pEDDRCtrl->ARP_IO_DONE)
                {
                    /* ----------------------------------------------------*/
                    /* this RCTRL is ready. Do finish-handling             */
                    /* ----------------------------------------------------*/
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-canceled sniffing.",pHDB->ThisHandle,pEDDRCtrl->ThisIdx);
                    /* ----------------------------------------------------*/
                    /* if no more need sniffing for this channel we stop it*/
                    /* ----------------------------------------------------*/
                    NARE_ASSERT_TRUE(pHDB->ARPSnifferCnt== 0)
                    if ( ! --pHDB->ARPSnifferCnt )
                    {
                        NARE_SYSTEM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"-->: EDD[%X:%d]-Stopping Sniffer for NIC: 0x%X.",pHDB->ThisHandle,pRCtrl->ThisIdx,pHDB->Params.pDetail->Params.Edd.NICId);
                        NARE_ARP_RESPONSE_FRAME_CONTROL(pHDB->Params.pDetail->Params.Edd.NICId,LSA_FALSE);
                    }
                    /* ----------------------------------------------------*/
                    /* If no one uses the timer anymore we stop it         */
                    /* ----------------------------------------------------*/
                    NARE_ASSERT_TRUE(pHEDD->ARPTimer.UserCnt == 0)
                    if ( ! --pHEDD->ARPTimer.UserCnt )
                    {
                        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Stopping ARP Timer");
                        NARETimerStop(pHEDD->ARPTimer.ID);
                    }
                    if(pEDDRCtrl->pARP_RQB != LSA_NULL)
                    {   /*we have  to free these blocks */
                        LSA_UINT16               RetVal;
                        NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB = pEDDRCtrl->pARP_RQB;
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
                        NARE_EDD_FREE_LOWER_TXMEM(&RetVal,((NARE_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE)pRQB->pParam)->pBuffer,pHDB->Params.pSys);
                        NARE_MEM_FREE_FAIL_CHECK(RetVal);
#endif

                        NARE_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
                        NARE_MEM_FREE_FAIL_CHECK(RetVal);

                        NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
                        NARE_MEM_FREE_FAIL_CHECK(RetVal);
                        pEDDRCtrl->pARP_RQB = LSA_NULL;
                    }
                    /* release the RCTRL-Block */
                    NARE_EDDReleaseRCTRL(pHDB,pEDDRCtrl);
                    /* call state machine      */
                    NARE_UserStateMachine(pRCtrl,NARE_RSP_OK_CANCEL,LSA_TRUE);
                    Finished = LSA_TRUE;
                }
            }
            pEDDRCtrl++;
            j++;
        }
    }
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT: NARE_EDDFindAndFinishSniff(Finished: 0x%X)",Finished);
    return(Finished);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDChannelDone                         +*/
/*+  Input                      NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+                             NARE_RSP                     Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  Response   : Responsecode of actual action taken (depends on state)    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handler for Open and Close-Channel requests to NARE for   +*/
/*+               EDD-Channels. This function is called after a lower       +*/
/*+               layer called back the NARE (e.g. after EDD finished his   +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDChannelDone(
    NARE_HDB_EDD_PTR_TYPE       pHDB,
    NARE_RSP                    Response)
{
    NARE_UPPER_RQB_PTR_TYPE     pRQB;
    LSA_UINT16                  PathStat;
    LSA_UINT32                  Finish;
    LSA_VOID                    LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR, Cbf) (NARE_UPPER_RQB_PTR_TYPE pRQB);
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT32                  TraceIdx;

    Finish = NOT_FINISHED; /* dont finish request by default */
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDChannelDone(pHDB: 0x%X, Response: 0x%X)",pHDB,Response);
    TraceIdx = pHDB->TraceIdx;  /* save it, for the HDB may get freed */
    pRQB   = pHDB->pOCReq;
    Cbf    = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe freed!*/
    pSys  = pHDB->Params.pSys;
    NARE_ASSERT_NULL_PTR(pRQB);
    switch (pHDB->State)
    {       /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Open Channel request.                         */
            /* Next Get MAC from EDD                                             */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_OPEN_EDD_RUNNING:
            if ( Response == NARE_RSP_OK)
            {   /* Get MAC-Address from EDD                   . */
                pHDB->State  = NARE_HANDLE_STATE_GET_MAC_EDD_RUNNING;
                Response = NARE_EDDGetMACAddr(pHDB);
                if ( Response != NARE_RSP_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD Issuing GetMACRequest failed.");
                    pHDB->State  = NARE_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error.");
                    if ( NARE_EDDCloseChannel(pHDB->EDDHandle,pHDB->Params.pSys) != NARE_RSP_OK)
                    {
                        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        NARE_FATAL(NARE_FATAL_ERR_EDD);
                    }
                }
            }
            else
            {
                Finish = FINISHED_REL_SYSPTR;
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a GET-MAC EDD-request.                              */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_GET_MAC_EDD_RUNNING:
            if ( Response == NARE_RSP_OK)
            {
                /* no more actions needed. Channel is open now. */
                pHDB->State  = NARE_HANDLE_STATE_OPEN;
                Finish = FINISHED_OK;
            }
            else
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD GetMAC request failed.");
                pHDB->OpenResponse = Response; /* save for later */
                pHDB->State  = NARE_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");
                if ( NARE_EDDCloseChannel(pHDB->EDDHandle,pHDB->Params.pSys) != NARE_RSP_OK)
                {
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                    NARE_FATAL(NARE_FATAL_ERR_EDD);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Close-Channel because of error within Open    */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING:
            if ( Response == NARE_RSP_OK)
            {   /* we will release the HDB  */
                Finish = FINISHED_REL_SYSPTR;
                Response = pHDB->OpenResponse;  /* get Error-Response  */
            }
            else
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                Finish = FINISHED_OK;
                NARE_FATAL(NARE_FATAL_ERR_EDD);
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_CLOSE_EDD_RUNNING:
            if ( Response == NARE_RSP_OK)
            {   /* release PATH-Info !                          */
                Finish = FINISHED_REL_SYSPTR;
            }
            else
            {   /* is this a fatal error ?                      */
                Finish = FINISHED_REL_SYSPTR;
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Other states not expected within this function.                   */
            /*-------------------------------------------------------------------*/
        default:
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */

    if ( Finish != NOT_FINISHED )  /* finish the request ? */
    {
        LSA_SYS_PTR_TYPE pSys = pHDB->Params.pSys;
        NARE_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail;

        pHDB->pOCReq = LSA_NULL;  /* to signal no more request pending.      */
        NARE_RQB_SET_RESPONSE(pRQB,Response);
        /* upper - handler already set */
        NARE_UPPER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",
                                                                                    NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),Response);
        
        if (Finish == FINISHED_REL_SYSPTR)
        {   /* we have to release the HDB prior to calling the CBF          */
            NARE_EDDReleaseHDB(pHDB);
        }
        NARECallCbf(Cbf, pRQB, pSys);
        if (Finish == FINISHED_REL_SYSPTR)
        {
            /* release PATH-Info !                          */
            NARE_RELEASE_PATH_INFO(&PathStat, pSys, pDetail);
            if (PathStat != LSA_RET_OK ) 
            {
                NARE_FATAL(NARE_FATAL_ERR_RELEASE_PATH_INFO);
            }
        } 
   }
    NARE_FUNCTION_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDGetHandleByNIC                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    NARE_HDB_EDD_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a EDD HDB with NICId. If none is found    +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_HDB_EDD_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_EDDGetHandleByNIC(
    LSA_UINT16 NICId)
{
    NARE_HDB_EDD_PTR_TYPE pHDB;
    LSA_UINT32            i;
    LSA_BOOL              Found;

    i     = NARE_MGM_GET_HEDD_PTR()->HandleCnt;
    pHDB  = NARE_MGM_GET_HEDD_PTR()->HDB;
    Found = LSA_FALSE;
    while ( (! Found ) && ( i ) )
    {
        if ((( pHDB->State == NARE_HANDLE_STATE_OPEN ) ||
             ( pHDB->State == NARE_HANDLE_STATE_SNIFF_OWN_IP_EDD_RUNNING )) &&
             ( pHDB->Params.pDetail->Params.Edd.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDB++;
            i--;
        }
    }
    if ( ! Found ) 
    {
        pHDB = LSA_NULL;
    }
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDCheckNICUsed                        +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if NIC is already used by some channel (maybe      +*/
/*+               channel currently in opening-state)                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL NARE_LOCAL_FCT_ATTR NARE_EDDCheckNICUsed(
    LSA_UINT16 NICId)
{
    NARE_HDB_EDD_PTR_TYPE pHDB;
    LSA_UINT32            i;
    LSA_BOOL              Found;

    i     = NARE_MGM_GET_HEDD_PTR()->HandleCnt;
    pHDB  = NARE_MGM_GET_HEDD_PTR()->HDB;
    Found = LSA_FALSE;

    while ( (! Found ) && ( i ) )
    {
        if (( pHDB->State != NARE_HANDLE_STATE_FREE ) &&
            ( pHDB->Params.pDetail->Params.Edd.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDB++;
            i--;
        }
    }
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDGetFreeHDB                          +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE NARE_LOCAL..pHDBEdd   +*/
/*+                             LSA_SYS_PTR_TYPE          pSys              +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : NARE EDD  -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free EDD -HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_EDD_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBEdd,
    LSA_SYS_PTR_TYPE          pSys)
{
    NARE_RSP                  Status;
    LSA_BOOL                  Found;
    NARE_HDB_EDD_PTR_TYPE     pHDB;
    LSA_UINT32                DSize;
    LSA_UINT32                i;
    NARE_EDD_RCTRL_PTR_TYPE   pRCtrl;
    LSA_UINT16                FreeStat;
    LSA_USER_ID_TYPE          UserId;
#ifdef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    LSA_UINT16                AllocLength;
#endif

    pHDB     = (NARE_MGM_GET_HEDD_PTR()->HDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDGetFreeHDB(pHDB: 0x%X)",pHDBEdd);
    i        = 0;
    Found    = LSA_FALSE;
    Status   = NARE_RSP_OK;
    FreeStat = 0;
    UserId.uvar32 = 0;
    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    while ( (! Found)  &&  ( i < NARE_MGM_GET_HEDD_PTR()->HandleCnt))
    {
        if ( pHDB->State == NARE_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            i++;
            pHDB++;
        }
    }
    /* --------------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..                  */
    /* --------------------------------------------------------------------------*/
    if ( Found )
    {
        DSize = sizeof(NARE_EDD_RCTRL_TYPE) * NoOfParallelRQBs;

        /* allocate control-block memory */
        NARE_ALLOC_LOCAL_MEM((NARE_LOCAL_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *)&pHDB->EDDRCtrl.pTable,DSize);
        if ( LSA_HOST_PTR_ARE_EQUAL(pHDB->EDDRCtrl.pTable, LSA_NULL) )
        {
            Status = NARE_RSP_ERR_RESOURCE;
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"NARE_EDDGetFreeHDB(). Allocating memory failed.");
        }
        if ( Status == NARE_RSP_OK)
        {
            /* ------------------------------------------------------------------*/
            /* Setup RCTRL-Blocks for EDD                                        */
            /* ------------------------------------------------------------------*/
            /* initialize memory with 0 */
            NARE_MEMSET_LOCAL(pHDB->EDDRCtrl.pTable,0,DSize);
            /* initialize EDD RCTRL-Block management  */
            pHDB->EDDRCtrl.Cnt    = NoOfParallelRQBs;
            pHDB->EDDRCtrl.Free   = NoOfParallelRQBs;
            pHDB->EDDRCtrl.Used   = 0;
            pHDB->EDDRCtrl.pFree.pBottom  = LSA_NULL;
            pHDB->EDDRCtrl.pFree.pTop     = LSA_NULL;
            pRCtrl = &(* pHDB->EDDRCtrl.pTable)[0]; /* pointer to first element */
            for ( i=0; i< NoOfParallelRQBs; i++)
            {
                NARE_PUT_BLOCK_TOP(pHDB->EDDRCtrl.pFree.pBottom,
                    pHDB->EDDRCtrl.pFree.pTop,
                    pRCtrl);
                pRCtrl->InUse     = LSA_FALSE;
                pRCtrl->ThisIdx   = (LSA_UINT16)i;
                pRCtrl->ARP_IO_DONE = LSA_FALSE;
                pRCtrl->Sniffer.DoSniff = LSA_FALSE;
                pRCtrl++;
            }
            NARE_MGM_GET_HEDD_PTR()->UsedCnt++;

#ifdef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
            /*-------------------------------------------------------------------*/
            /* allocate one tx-frame buffer for each RCTRL-Block                 */
            /* so we dont need to allocate it with every send..                  */
            /*-------------------------------------------------------------------*/
            pRCtrl = &(* pHDB->EDDRCtrl.pTable)[0]; /* pointer to first element  */
            /* Because of a EDDI restriction the BufferLength have to be a       */
            /* multiple of 4 Bytes. So we extend the Buffer if needed..          */
            /* This is neccessary with EDDI because of HW-Padding of short       */
            /* frames workaround.                                                */
            AllocLength = (LSA_UINT16)((NARE_ARP_TX_FRAME_LEN + 3) & ~0x3);
            i = 0;
            while (( Status == NARE_RSP_OK ) && ( i<NoOfParallelRQBs))
            {
                NARE_EDD_ALLOC_LOWER_TXMEM(&pRCtrl->pTxFrame,
                    UserId,
                    AllocLength,
                    pSys);
                if ( LSA_HOST_PTR_ARE_EQUAL(pRCtrl->pTxFrame, LSA_NULL) )
                {
                    Status = NARE_RSP_ERR_RESOURCE;
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"NARE_EDDGetFreeHDB(). Allocating memory failed.");
                }
                else
                {
                    pRCtrl++;
                    i++;
                }
            }
            /*-------------------------------------------------------------------*/
            /* on error we free all allocated memory                             */
            /*-------------------------------------------------------------------*/
            if ( Status != NARE_RSP_OK )
            {
                pRCtrl = &(* pHDB->EDDRCtrl.pTable)[0]; /* pointer to first element */
                /* Pointers not allocated are LSA_NULL */
                for ( i=0; i<pHDB->SizeOfEDDRCtrlQue; i++)
                {
                    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl->pTxFrame, LSA_NULL) )
                    {
                        NARE_EDD_FREE_LOWER_TXMEM(&FreeStat, pRCtrl->pTxFrame,pSys);
                        NARE_MEM_FREE_FAIL_CHECK(FreeStat);
                    }
                    pRCtrl++;
                }
                NARE_FREE_LOCAL_MEM(&FreeStat, pHDB->EDDRCtrl.pTable);
                NARE_MEM_FREE_FAIL_CHECK(FreeStat);
            }
#endif
            if ( Status == NARE_RSP_OK )
            {
                pHDB->State = NARE_HANDLE_STATE_CLOSED;
                *pHDBEdd = pHDB;
            }
        }
    }
    else
    {
        Status = NARE_RSP_ERR_RESOURCE;
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"NARE_EDDGetFreeHDB(). No more free handles.");
    }
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",Status,*pHDBEdd);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDReleaseHDB                          +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE pHDB                  +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to User Handle to release                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release EDD -HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to NARE_HANDLE_STATE_FREE               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDReleaseHDB(
    NARE_HDB_EDD_PTR_TYPE pHDB)
{
    LSA_UINT16   FreeStat;

#ifdef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    LSA_UINT32                i;
    NARE_EDD_RCTRL_PTR_TYPE   pRCtrl;
#endif

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDReleaseHDB(pHDB: 0x%X)",pHDB);
    NARE_ASSERT_NULL_PTR(pHDB->EDDRCtrl.pTable);
    FreeStat = 0;
#ifdef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    /* --------------------------------------------------------------------------*/
    /* Free all TxFrame-Buffers                                                  */
    /* --------------------------------------------------------------------------*/
    pRCtrl = &(* pHDB->EDDRCtrl.pTable)[0]; /* pointer to first element */

    /* Pointers not allocated are LSA_NULL */
    for ( i=0; i<pHDB->SizeOfEDDRCtrlQue; i++)
    {
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl->pTxFrame, LSA_NULL) )
        {
            NARE_EDD_FREE_LOWER_TXMEM(&FreeStat, pRCtrl->pTxFrame,pHDB->Params.pSys);
            NARE_MEM_FREE_FAIL_CHECK(FreeStat);
        }
        pRCtrl++;
    }
#endif

    NARE_FREE_LOCAL_MEM(&FreeStat, pHDB->EDDRCtrl.pTable);
    NARE_MEM_FREE_FAIL_CHECK(FreeStat);
    pHDB->EDDRCtrl.pTable = LSA_NULL;

    pHDB->EDDRCtrl.Cnt    = 0;
    pHDB->EDDRCtrl.Free   = 0;
    pHDB->EDDRCtrl.Used   = 0;
    pHDB->EDDRCtrl.pFree.pBottom  = LSA_NULL;
    pHDB->EDDRCtrl.pFree.pTop     = LSA_NULL;
    pHDB->State = NARE_HANDLE_STATE_FREE;
    NARE_MGM_GET_HEDD_PTR()->UsedCnt--;
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDReleaseHDB()");
    pHDB->TraceIdx        = 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDGetRCTRLByIdx                       +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE             pHDB      +*/
/*+                             LSA_UINT32                        Idx       +*/
/*+  Result                :    NARE_EDD_RCTRL_PTR_TYPE                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD -HDB                                       +*/
/*+  Idx        : Index in RCTRl-block table.                               +*/
/*+                                                                         +*/
/*+  Result     : Pointer to RCTRL-Block                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a EDD -RCTRL-Block by index               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_EDD_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_EDDGetRCTRLByIdx(
    NARE_HDB_EDD_PTR_TYPE pHDB,
    LSA_UINT32            Idx)
{
    NARE_EDD_RCTRL_PTR_TYPE  pRCtrl;

    NARE_ASSERT_NULL_PTR(pHDB);
    pRCtrl = LSA_NULL; /* prevent compiler warning with tasking */
    if ( Idx < pHDB->EDDRCtrl.Cnt )
    {
        pRCtrl = &(*pHDB->EDDRCtrl.pTable)[Idx];
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"Invalid EDD RCTRL-Index.");
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    return(pRCtrl);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDGetFreeRCTRL                        +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE             pHDB      +*/
/*+  Result                :    NARE_EDD_RCTRL_PTR_TYPE                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD -HDB                                       +*/
/*+                                                                         +*/
/*+  Result     : Pointer to free RCTRL-Block or LSA_NULL if none free      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free EDD -RCTRL-Block                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_EDD_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_EDDGetFreeRCTRL(
    NARE_HDB_EDD_PTR_TYPE pHDB)
{
    NARE_EDD_RCTRL_PTR_TYPE  pRCtrl;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_REM_BLOCK_BOTTOM(pHDB->EDDRCtrl.pFree.pBottom,
                          pHDB->EDDRCtrl.pFree.pTop,
                          pRCtrl);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl, LSA_NULL) )
    {
        pHDB->EDDRCtrl.Used++;
        pHDB->EDDRCtrl.Free--;
        pRCtrl->InUse = LSA_TRUE;
    }
    return(pRCtrl);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDReleaseRCTRL                        +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE              pHDB     +*/
/*+                             NARE_EDD_RCTRL_PTR_TYPE            pRCtrl   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD-HDB                                        +*/
/*+  pRCtrl     : Pointer to EDD-RCTRL-Block to release                     +*/
/*+                                                                         +*/
/*+  Result     : --                                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Releases an EDD-RCTRL-Block                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDReleaseRCTRL(
    NARE_HDB_EDD_PTR_TYPE      pHDB,
    NARE_EDD_RCTRL_PTR_TYPE    pRCtrl)
{
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_ASSERT_NULL_PTR(pRCtrl);
    if ( pRCtrl->InUse )
    {  /* Consistenzcheck validates RCtrlblock. Check Poitner with "ThisIdx" */
        if ( ! LSA_HOST_PTR_ARE_EQUAL(&(*pHDB->EDDRCtrl.pTable)[pRCtrl->ThisIdx],pRCtrl))
        {   /* unexpected. ThisIdx and Pointer not matching.*/
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
        }
        /* put into free-list */
        NARE_PUT_BLOCK_TOP(pHDB->EDDRCtrl.pFree.pBottom,
                           pHDB->EDDRCtrl.pFree.pTop,
                           pRCtrl);
        pHDB->EDDRCtrl.Used--;
        pHDB->EDDRCtrl.Free++;
        pRCtrl->InUse = LSA_FALSE;
    }
    else
    {   /* unexpected. Pointer not found in list */
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDOpenChannelDone                     +*/
/*+  Input                      NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
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

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDOpenChannelDone(
    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16               RetVal;
    NARE_RSP                 Response;

    NARE_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDOpenChannelDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB, pHDB);
    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: EDD_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_EDD_RQB_GET_RESPONSE(pRQB));
    if ( NARE_EDD_RQB_GET_RESPONSE(pRQB) == EDD_STS_OK)
    {
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Open Channel successful.");
        /* save channel */
        pHDB->EDDHandle = ((NARE_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE )pRQB->pParam)->Handle;
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"EDD-Open Channel failed.[0x%X].",NARE_EDD_RQB_GET_RESPONSE(pRQB));
        Response = NARE_RSP_ERR_LL;
    }
    NARE_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDOpenChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDCloseChannelDone                    +*/
/*+  Input                      NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
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

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDCloseChannelDone(
    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16               RetVal;
    NARE_RSP                 Response;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);

    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: EDD_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_EDD_RQB_GET_RESPONSE(pRQB));
    if ( NARE_EDD_RQB_GET_RESPONSE(pRQB) == EDD_STS_OK )
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Close Channel successful.");
    }
    else
    {
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD-Close Channel failed.[0x%X].",NARE_EDD_RQB_GET_RESPONSE(pRQB));
        Response = NARE_RSP_ERR_LL;
    }
    NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDCloseChannelDone() pHDP = 0x%x",pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDGetMACDone                          +*/
/*+  Input                      NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get MAC done handling                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDGetMACDone(
    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_EDD_PTR_TYPE        pHDB)
{
    NARE_RSP                 Response;
    LSA_UINT16               RetVal;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN : NARE_EDDGetMACDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);
    NARE_LOWER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_GET_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = NARE_RSP_OK;
    switch (NARE_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            NARE_COPY_MAC_ADDR_LOC_LER(&pHDB->MACAddr, ((NARE_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr);
            NARE_PROGRAM_TRACE_06(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PARAMS successful.: %02X-%02X-%02X-%02X-%02X-%02X",
                                  pHDB->MACAddr[0],
                                  pHDB->MACAddr[1],
                                  pHDB->MACAddr[2],
                                  pHDB->MACAddr[3],
                                  pHDB->MACAddr[4],
                                  pHDB->MACAddr[5]);
            pHDB->TraceIdx = ((NARE_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->TraceIdx;
            break;

        default:
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: GET PARAMS failed.(0x%X).",NARE_EDD_RQB_GET_RESPONSE(pRQB));
            Response = NARE_RSP_ERR_LL;
            break;
    }
    NARE_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDGetMACDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDNrtSendDone                         +*/
/*+  Input                      NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Done Sending NRT-Frame (actual an ARP-Request)            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDNrtSendDone(
    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_EDD_PTR_TYPE        pHDB)
{
    NARE_RSP                 Response;
    NARE_EDD_RCTRL_PTR_TYPE  pRCtrl;
    LSA_BOOL                 CallStateMachine;
    LSA_BOOL                 Resend;
    NARE_USER_RCTRL_PTR_TYPE pRCtrlUser;
    LSA_UINT16               RetVal;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDNrtSendDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    NARE_LOWER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_NRT_SEND done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_EDD_RQB_GET_RESPONSE(pRQB));

    Response         = NARE_RSP_OK;
    CallStateMachine = LSA_FALSE;
    Resend           = LSA_FALSE;

    pRCtrl = NARE_EDDGetRCTRLByIdx(pHDB,NARE_EDD_RQB_GET_USERID_UVAR32(pRQB));
    NARE_ASSERT_NULL_PTR(pRCtrl);
    pRCtrlUser = pRCtrl->pRCtrl;
    NARE_ASSERT_NULL_PTR(pRCtrlUser);
    pRCtrl->ARP_IO_DONE = LSA_TRUE; /* we 've got the rqb back from EDD */ 
    switch (NARE_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"NARE_EDDNrtSendDone [Handle %X:%d]-ARP-Sent successful. [doCancel =%d]",pHDB->ThisHandle,pRCtrl->ThisIdx,pRCtrl->pRCtrl->DoCancel);
            /* if we should Burst ARPs we send the next one but only if we should not cancel */
            if (( pRCtrl->BurstCnt > 1 ) && ( ! pRCtrl->pRCtrl->DoCancel ))
            {
                pRCtrl->SentARPs ++;
                pRCtrl->BurstCnt--;
                /* check whether the RQ is a test_ip */
                if(NARE_RQB_GET_OPCODE(pRCtrlUser->pRQB) == NARE_OPC_TEST_IP)
                {
                    pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16) (NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter + pRCtrl->RandomTime + 10);  /* add a second  */
                }
                else
                {
                    if ((pRCtrl->SentARPs & 0x01 ) == 1) 
                    {   /* delay for odd sends is  ARPSniffTimeout1 * 100 msec  */
                        pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16) NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter + pHDB->Params.pDetail->Params.Edd.ARPSniffTimeout1;
                    }
                    else
                    {   /* delay for even sends is  ARPSniffTimeout2 * 100 msec */
                        pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16) NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter + pHDB->Params.pDetail->Params.Edd.ARPSniffTimeout2;
                    }
                }
                NARE_PROGRAM_TRACE_04(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Send next ARP-Request in %d msec from burst [remain: %d].",pHDB->ThisHandle,
                    pRCtrl->ThisIdx,(pRCtrl->Sniffer.TimerEndCounter-(NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter))*100,pRCtrl->BurstCnt-1);

                /* set the right handle.                        */
                /* all other params still ok.                   */
                NARE_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);  /* Prepare for sending */
                pRCtrl->pARP_RQB = pRQB;                        /* Send is done in the timer routine */
                pRCtrl->pHDBEDD  = pHDB;
/*
                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Send next ARP-Request from burst [remain: %d].",pHDB->ThisHandle,pRCtrl->ThisIdx,pRCtrl->BurstCnt-1);
                NARE_LOWER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_NRT_SEND (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
                NARE_EDD_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
*/
                Resend  = LSA_TRUE;
            }
            else
            {   /* Setup EndTime for sniffing again. We add 1 tick to   */
                /* avoid ending "at once"                               */
                if(NARE_RQB_GET_OPCODE(pRCtrlUser->pRQB) == NARE_OPC_TEST_IP)
                {
                    pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16)( NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter + 20); /* Wait 2 seconds for incomming ARPs AP01248034 */
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDNrtSendDone: no more ARPs still waiting 2 sec for incomming responses");
                }
                else
                {
                    pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16)( NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter +
                                                      pHDB->Params.pDetail->Params.Edd.ARPSniffTime + 1);
                }
            }
            break;

        case EDD_STS_ERR_TX:
        case EDD_STS_ERR_TIMEOUT:
            /* ---------------------------------------------*/
            /* On Error we continue                         */
            /* ---------------------------------------------*/
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"EDD[%X:%d]-ERROR: NRT-SEND failed, but continue (0x%X).",pHDB->ThisHandle,pRCtrl->ThisIdx,NARE_EDD_RQB_GET_RESPONSE(pRQB));
            break;

        default:
            /* ---------------------------------------------*/
            /* On Error we abort.                           */
            /* ---------------------------------------------*/
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"EDD[%X:%d]-ERROR: NRT-SEND failed (0x%X).",pHDB->ThisHandle,pRCtrl->ThisIdx,NARE_EDD_RQB_GET_RESPONSE(pRQB));
            Response = NARE_RSP_ERR_LL;
            CallStateMachine  = LSA_TRUE;
            break;
    }
    if ( ! Resend )
    {
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
        NARE_EDD_FREE_LOWER_TXMEM(&RetVal,((NARE_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE)pRQB->pParam)->pBuffer,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
#endif
        NARE_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        pRCtrl->pARP_RQB = LSA_NULL;
    }
    if ((CallStateMachine) || ( pRCtrl->pRCtrl->DoCancel )) /* on error or cancel we call statemachine */
    {
        pRCtrl->Sniffer.DoSniff = LSA_FALSE; /* stop sniffing */
        /* turn off sniffer if this is the only one           */
        /* using the sniffer.                                 */
        if ( ! --pHDB->ARPSnifferCnt )
        {
            NARE_SYSTEM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"-->: EDD[%X:%d]-Stopping Sniffer, NIC: 0x%X.",pHDB->ThisHandle,pRCtrl->ThisIdx,pHDB->Params.pDetail->Params.Edd.NICId);
            NARE_ARP_RESPONSE_FRAME_CONTROL(pHDB->Params.pDetail->Params.Edd.NICId,LSA_FALSE);
        }
        /* decrement UserCnt and stop timer if no one needs it*/
        /* anymore                                            */
        if (( ! --NARE_MGM_GET_HEDD_PTR()->ARPTimer.UserCnt ))
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Stopping ARP Timer.");
            NARETimerStop(NARE_MGM_GET_HEDD_PTR()->ARPTimer.ID);
        }
        /* release the RCTRL-Block */
        NARE_EDDReleaseRCTRL(pHDB,pRCtrl);
        switch ( pRCtrlUser->QueueType )
        {
            case NARE_RCTRL_QTYPE_EDD_RUNNING:
                /* call the main statemachine     */
                NARE_UserStateMachine(pRCtrlUser,Response,LSA_TRUE);
                break;

            case NARE_RCTRL_QTYPE_EDD_OPEN:
                /* further actions taken in channel handler */
                NARE_EDDChannelDone(pHDB,Response);
                break;

            default:
                NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDNrtSendDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDRQBCopyLLErrorLocal                 +*/
/*+  Input                      LSA_RQB_LOCAL_ERROR_TYPE  NA...* pLLError   +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copys LL-Error to local structure.                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_EDDRQBCopyLLErrorLocal(
    LSA_RQB_LOCAL_ERROR_TYPE   NARE_EDD_LOWER_RQB_ATTR * pLLError)
{
    /* copy only if something filled in.. */
    if ( pLLError->lsa_component_id != LSA_COMP_ID_UNUSED)
    {
        NARE_MGM_GET_NDB_PTR()->LLError = * pLLError;
    }
    else 
    {
        NARE_MGM_GET_NDB_PTR()->LLError.lsa_component_id = LSA_COMP_ID_UNUSED;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_edd_request_lower_done                 +*/
/*+  Input                      NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB           +*/
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

LSA_VOID NARE_EDD_LOWER_IN_FCT_ATTR nare_edd_request_lower_done(
    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB)
{
    NARE_HDB_EDD_PTR_TYPE   pHDB;

    NARE_ENTER();
    NARE_ASSERT_NULL_PTR(pRQB);
    /* EDD-request always done from an EDD-Channel. !           */
    pHDB     = NAREGetEddHDBFromHandle(NARE_EDD_RQB_GET_HANDLE(pRQB));

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        NARE_FATAL(NARE_FATAL_ERR_HANDLE_INVALID);
    }
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN : nare_edd_request_lower_done(pRQB: 0x%X)", pRQB);
    switch (NARE_EDD_RQB_GET_OPCODE(pRQB) )
    {
        case EDD_OPC_OPEN_CHANNEL:
            NARE_EDDOpenChannelDone(pRQB,pHDB);
            break;

        case EDD_OPC_CLOSE_CHANNEL:
            NARE_EDDCloseChannelDone(pRQB,pHDB);
            break;

        case EDD_OPC_REQUEST:
            switch ( NARE_EDD_RQB_GET_SERVICE(pRQB))
            {
                case EDD_SRV_NRT_SEND:
                    NARE_EDDNrtSendDone(pRQB,pHDB);
                    break;

                case EDD_SRV_GET_PARAMS:
                    NARE_EDDGetMACDone(pRQB,pHDB);
                    break;

                default:
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;
            }
            break;

            default:
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT: nare_edd_request_lower_done()");
    NARE_EXIT();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDStartSniffer                        +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE      pHDB             +*/
/*+                             NARE_EDD_RCTRL_PTR_TYPE    pRCtrl           +*/
/*+                             LSA_UINT8                  Mode             +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD -HDB                                       +*/
/*+  pRCtrl     : Pointer to EDD-RCTRL-Block containing User-RCTRL-Block    +*/
/*+               with valid Bstate.                                        +*/
/*+  Mode       : Sniffing Mode                                             +*/
/*+               NARE_EDD_ARP_SNIFF_MODE_MULTIPLE                          +*/
/*+               NARE_EDD_ARP_SNIFF_MODE_SINGLE                            +*/
/*+                                                                         +*/
/*+  Result:      NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts ARP-Sniffer.                                       +*/
/*+               The Sniffer-Mgm within pRCtrl has to be setup before      +*/
/*+               calling!!                                                 +*/
/*+                                                                         +*/
/*+               Sniffer.CurMAC      : Current MAC (0 if not present)      +*/
/*+               Sniffer.DestIP      : IP to test for.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDStartSniffer(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_EDD_RCTRL_PTR_TYPE      pRCtrl,
    LSA_UINT8                    Mode)
{
    NARE_RSP                    Status;

    NARE_FUNCTION_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDStartSniffer(pHDB: 0x%X, PRCtrl: 0x%X (Idx: %d))",pHDB,pRCtrl,pRCtrl->ThisIdx);
    Status = NARE_RSP_OK;
    pRCtrl->Sniffer.Mode = Mode;
    if ( Mode == NARE_EDD_ARP_SNIFF_MODE_MULTIPLE )
    {   /* check for multiple response or a response <> curent mac */
        if ( (pRCtrl->Sniffer.CurMAC.b.MACAddr[0] != 0 ) &&
             (pRCtrl->Sniffer.CurMAC.b.MACAddr[1] != 0 ) &&
             (pRCtrl->Sniffer.CurMAC.b.MACAddr[2] != 0 ) &&
             (pRCtrl->Sniffer.CurMAC.b.MACAddr[3] != 0 ) &&
             (pRCtrl->Sniffer.CurMAC.b.MACAddr[4] != 0 ) &&
             (pRCtrl->Sniffer.CurMAC.b.MACAddr[5] != 0 )
             )
        {
            pRCtrl->Sniffer.CurMACValid = LSA_TRUE;
        }
        else
        {
            pRCtrl->Sniffer.CurMACValid = LSA_FALSE;
        }
    }
    else 
    {
        pRCtrl->Sniffer.CurMACValid = LSA_FALSE; /* not used */
    }
    NARE_PROGRAM_TRACE_06(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Sniffing for IP: %d.%d.%d.%d",
                          pHDB->ThisHandle,pRCtrl->ThisIdx,
                          pRCtrl->Sniffer.DestIP.b.IPAddr[0],
                          pRCtrl->Sniffer.DestIP.b.IPAddr[1],
                          pRCtrl->Sniffer.DestIP.b.IPAddr[2],
                          pRCtrl->Sniffer.DestIP.b.IPAddr[3]);

    NARE_PROGRAM_TRACE_08(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Sniffing for MAC: %02X-%02X-%02X-%02X-%02X-%02X",
                          pHDB->ThisHandle,pRCtrl->ThisIdx,
                          pRCtrl->Sniffer.CurMAC.b.MACAddr[0],
                          pRCtrl->Sniffer.CurMAC.b.MACAddr[1],
                          pRCtrl->Sniffer.CurMAC.b.MACAddr[2],
                          pRCtrl->Sniffer.CurMAC.b.MACAddr[3],
                          pRCtrl->Sniffer.CurMAC.b.MACAddr[4],
                          pRCtrl->Sniffer.CurMAC.b.MACAddr[5]);

    NARE_PROGRAM_TRACE_06(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD[%X:%d]-Sniffing local IP: %d.%d.%d.%d",
                          pHDB->ThisHandle,pRCtrl->ThisIdx,
                          pRCtrl->Sniffer.LocalIP.b.IPAddr[0],
                          pRCtrl->Sniffer.LocalIP.b.IPAddr[1],
                          pRCtrl->Sniffer.LocalIP.b.IPAddr[2],
                          pRCtrl->Sniffer.LocalIP.b.IPAddr[3]);
    /* Activate Sniffing before sending the request   */
    /* because responses may come in before request is*/
    /* finished (depends on systemintegration)        */
    pRCtrl->Sniffer.MultipleRsp = LSA_FALSE;
    pRCtrl->Sniffer.DoSniff     = LSA_TRUE;
    if (! pHDB->ARPSnifferCnt )
    {
        NARE_SYSTEM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD[%X:%d]-Starting Sniffer",pHDB->ThisHandle,pRCtrl->ThisIdx);
        NARE_ARP_RESPONSE_FRAME_CONTROL(pHDB->Params.pDetail->Params.Edd.NICId,LSA_TRUE);
    }
    pHDB->ARPSnifferCnt++;
    Status = NARE_EDDSendARP(pHDB,
                             pRCtrl,
                             (NARE_LOCAL_MEM_IP_PTR_TYPE) &pRCtrl->Sniffer.DestIP.b.IPAddr,
                             (NARE_LOCAL_MEM_IP_PTR_TYPE) &pRCtrl->Sniffer.LocalIP.b.IPAddr);
    if ( Status == NARE_RSP_OK )
    {   /* Setup EndTime for sniffing. We add 1 tick to   */
        /* avoid ending "at once"                         */
        /* Note: we setup the Counter again after the     */
        /*       frame was send.                          */
        LSA_UINT16  NextARPWaitTime;

        if((NARE_RQB_GET_OPCODE(pRCtrl->pRCtrl->pRQB) == NARE_OPC_TEST_IP))
        {
            /* calculate randomised waittime */
            LSA_UINT32 iMAC = (pHDB->MACAddr[4]<<8) + pHDB->MACAddr[5];
            pRCtrl->RandomTime = (iMAC % 10) + 1; /* avoid ending at once */
            NARE_SYSTEM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"RandomTime calculated to be %d",pRCtrl->RandomTime);
            if(pRCtrl->pRCtrl->pRQB->args.TestIP.UseDelay)
            {   /* This would be the first one wait 2..3.seconds   */
                NextARPWaitTime = (LSA_UINT16)(pRCtrl->RandomTime + 20);
                NARE_SYSTEM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"Sniffertimeout set to Random + 20  [NextARPWaitTime = %d]",NextARPWaitTime);
                pRCtrl->pRCtrl->pRQB->args.TestIP.UseDelay = LSA_FALSE;

            }
            else
            {
                NextARPWaitTime = (LSA_UINT16)(pRCtrl->RandomTime);
                NARE_SYSTEM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"Sniffertimeout set to Random [NextARPWaitTime = %d]",NextARPWaitTime);
            }
            pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16) NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter + NextARPWaitTime;
        }
        else
        {
            pRCtrl->Sniffer.TimerEndCounter = (LSA_UINT16) ( NARE_MGM_GET_HEDD_PTR()->ARPTimer.Counter +
                                                             pHDB->Params.pDetail->Params.Edd.ARPSniffTime + 1);
        }
        /* Start the ARP-Timer if not already running     */
        if (! NARE_MGM_GET_HEDD_PTR()->ARPTimer.UserCnt )
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Starting ARP Timer");
            NARETimerStart(NARE_MGM_GET_HEDD_PTR()->ARPTimer.ID,1);
        }
        NARE_MGM_GET_HEDD_PTR()->ARPTimer.UserCnt++;
    }
    else
    {
        pRCtrl->Sniffer.DoSniff = LSA_FALSE; /* stop sniffing */
        /* On error turn off sniffer if this is the only one  */
        /* using the sniffer.                                 */
        if ( ! --pHDB->ARPSnifferCnt )
        {
            NARE_SYSTEM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD[%X:%d]-Stopping Sniffer, NIC: 0x%X",pHDB->ThisHandle,pRCtrl->ThisIdx,pHDB->Params.pDetail->Params.Edd.NICId);
            NARE_ARP_RESPONSE_FRAME_CONTROL(pHDB->Params.pDetail->Params.Edd.NICId,LSA_FALSE);
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDStartSniffer()");
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDTrigger                             +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE     pHDB              +*/
/*+                             NARE_EDD_RCTRL_PTR_TYPE   pRCtrl            +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD -HDB                                       +*/
/*+  pRCtrl     : Pointer to EDD-RCTRL-Block containing User-RCTRL-Block    +*/
/*+               with valid Bstate.                                        +*/
/*+                                                                         +*/
/*+  Result:      NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Depending on the State of the User-RCTRL-Block the        +*/
/*+               neccessary EDD-Request will be executed.                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDTrigger(
    NARE_HDB_EDD_PTR_TYPE     pHDB,
    NARE_EDD_RCTRL_PTR_TYPE   pRCtrl)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    NARE_RSP                  Status;

    NARE_FUNCTION_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDTrigger(pHDB: 0x%X, PRCtrl: 0x%X (Idx: %d))",pHDB,pRCtrl,pRCtrl->ThisIdx);
    Status = NARE_RSP_OK;
    pRCtrlUser = pRCtrl->pRCtrl;
    /* Depending on RState we do the proper EDD-Request  */
    switch (*pRCtrlUser->pRState)
    {
        case NARE_RCTRL_RSTATE_EDD_ARP:
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD[%X:%d]-Calling NARE_EDDSendARP()",pHDB->ThisHandle,pRCtrl->ThisIdx);
            /* Note: The Cfg.IPAddr is the Address to resolve */
            /*       this is the IP the IO-Device should be   */
            /*       asigned or already has. Res.MAC may      */
            /*       contain the MAC address we excpect. MAC  */
            /*       may be 0 if not used.                    */
            /* Copy MAC/IP to Sniffer Management for faster   */
            /* access. MAC may not be present if set to 0.    */
            /* (if we resolve only with DNS)                  */
            NARE_COPY_MAC_ADDR_LOC_UR(pRCtrl->Sniffer.CurMAC.b.MACAddr,pRCtrlUser->pRQB->args.ResConf.Res.MACAddr);
            NARE_COPY_IP_ADDR_LOC_UR(pRCtrl->Sniffer.DestIP.b.IPAddr,pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr);
            NARE_COPY_IP_ADDR_LOC_UR(pRCtrl->Sniffer.LocalIP.b.IPAddr,pRCtrlUser->pRQB->args.ResConf.LocalIPAddr);
            /* Number of ARP-Requests to send.      */
            pRCtrl->BurstCnt = 1;
            Status = NARE_EDDStartSniffer(pHDB,pRCtrl,NARE_EDD_ARP_SNIFF_MODE_MULTIPLE);
            break;

        case NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP:
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD[%X:%d]-Calling NARE_EDDSendARP() to IP-test.",pHDB->ThisHandle,pRCtrl->ThisIdx);
            /* Copy MAC/IP to Sniffer Management for faster   */
            /* access. MAC is 0 because not used.             */
            /* IP is the address we have to test.             */
            pRCtrl->Sniffer.CurMAC.w.MAC1234 = 0;
            pRCtrl->Sniffer.CurMAC.w.MAC56   = 0;
            NARE_COPY_IP_ADDR_LOC_UR(pRCtrl->Sniffer.DestIP.b.IPAddr,pRCtrlUser->pRQB->args.TestIP.IPAddr);
            NARE_COPY_IP_ADDR_LOC_UR(pRCtrl->Sniffer.LocalIP.b.IPAddr,pRCtrlUser->pRQB->args.TestIP.LocalIPAddr);
            /* Number of ARP-Requests to send.      */
            /* Note: BurstCnt of 0 is the same as 1 */
/* AP01248034
            pRCtrl->BurstCnt = pHDB->Params.pDetail->Params.Edd.ARPRetryCnt + 1;   */
            pRCtrl->BurstCnt = 3; /* due to RFC is set to 3!  we count all ARPs    */
            pRCtrl->SentARPs = 0;
            Status = NARE_EDDStartSniffer(pHDB,pRCtrl,NARE_EDD_ARP_SNIFF_MODE_SINGLE);
            break;

        case NARE_RCTRL_RSTATE_EDD_RES_IP_ARP:
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD[%X:%d]-Calling NARE_EDDSendARP() to Resolve IP.",pHDB->ThisHandle,pRCtrl->ThisIdx);
            /* Copy MAC/IP to Sniffer Management for faster   */
            /* access. MAC is 0 because not used.             */
            /* IP is the address we have to test.             */
            pRCtrl->Sniffer.CurMAC.w.MAC1234 = 0;
            pRCtrl->Sniffer.CurMAC.w.MAC56   = 0;
            NARE_COPY_IP_ADDR_LOC_UR(pRCtrl->Sniffer.DestIP.b.IPAddr,pRCtrlUser->pRQB->args.TestIP.IPAddr);
            NARE_COPY_IP_ADDR_LOC_UR(pRCtrl->Sniffer.LocalIP.b.IPAddr,pRCtrlUser->pRQB->args.TestIP.LocalIPAddr);
            /* Number of ARP-Requests to send.      */
            /* Note: BurstCnt of 0 is the same as 1 */
            pRCtrl->BurstCnt = pHDB->Params.pDetail->Params.Edd.ARPRetryCnt;
            Status = NARE_EDDStartSniffer(pHDB,pRCtrl,NARE_EDD_ARP_SNIFF_MODE_MULTIPLE);
            break;

        default:
            /* unexpected state */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDD[%X:%d]-Unexpected RState (0x%X)",pHDB->ThisHandle,pRCtrl->ThisIdx,*pRCtrlUser->pRState);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDTrigger(), Status: 0x%X",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDCheckAndStartPending                +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE              pHDB     +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD -HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RCTRL-Block pending for EDD. If so check   +*/
/*+               for free EDD-RCTRL-Blocks. As long as one is free and     +*/
/*+               User-RCTRL-Blocks present start handling this Blocks by   +*/
/*+               doing the EDD-Request. The type of EDD-Request depends on +*/
/*+               the State of the User-RCTRL-Block.                        +*/
/*+                                                                         +*/
/*+               If starting of a EDD-request fails we finish the request. +*/
/*+                                                                         +*/
/*+               This function should be called after a EDD-Request was    +*/
/*+               finished, or a new User-RCTRL-Block was queued in EDD     +*/
/*+               pending-queue.                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDCheckAndStartPending(
    NARE_HDB_EDD_PTR_TYPE     pHDB)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    NARE_EDD_RCTRL_PTR_TYPE   pRCtrlEdd;
    LSA_BOOL                  Ready;
    NARE_RSP                  Status;
    NARE_HDB_USER_PTR_TYPE    pHDBUser;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDCheckAndStartPending(pHDB: 0x%X)",pHDB);
    Status = NARE_RSP_OK;
    Ready  = LSA_FALSE;
    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Checking for pending requests (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);

    while (! Ready )
    {
        if ( pHDB->PendRCtrlCnt ) /* User-RCTRL-Blocks pending ? */
        {
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-requests pending (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
            if (pHDB->EDDRCtrl.Free ) /* Free EDD-RCTRL-Blocks ? */
            {
                NARE_REM_BLOCK_BOTTOM(pHDB->PendRCtrl.pBottom,
                                      pHDB->PendRCtrl.pTop,
                                      pRCtrlUser);
                /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
                NARE_ASSERT_NULL_PTR(pRCtrlUser);
                pRCtrlUser->QueueType = NARE_RCTRL_QTYPE_EDD_RUNNING;
                pHDB->PendRCtrlCnt--;
                pRCtrlEdd = NARE_EDDGetFreeRCTRL(pHDB);
                /* because Free was >0 we dont expect LSA_NULL here         */
                NARE_ASSERT_NULL_PTR(pRCtrlEdd);
                /* save pointer to user-RCTRL-Block within EDD-RCTRL-block  */
                /* for later access. EDD-requests getting the ThisIdx from  */
                /* the EDD-RCTRL-Block as user-id                           */
                pRCtrlEdd->pRCtrl = pRCtrlUser;
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Free RCTRL-Found [Hdl: 0x%X,Idx: %d]. Triggering EDD Request",pHDB->ThisHandle,pRCtrlEdd->ThisIdx);
                Status = NARE_EDDTrigger(pHDB,pRCtrlEdd);
                /* if failed we finish the user-request*/
                if ( Status != NARE_RSP_OK )
                {   /* release the EDD-RCTRL-Block */
                    NARE_EDDReleaseRCTRL(pHDB,pRCtrlEdd);
                    pHDBUser = NAREGetUserHDBFromHandle(NARE_RQB_GET_HANDLE(pRCtrlUser->pRQB));
                    NARE_ASSERT_NULL_PTR(pHDBUser);
                    /* release RCtrl-Block and finish the request */
                    NARE_UserReady(pHDBUser,pRCtrlUser,Status);
                }
            }
            else
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-No free EDD-RCTRL (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
                Ready = LSA_TRUE;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-No requests pending  (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
            Ready = LSA_TRUE;
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDCheckAndStartPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDApplyRCtrl                          +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE      pHDB             +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD -HDB                                       +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block to handle.                    +*/
/*+               Must contain valid Bstate which selects the action to do. +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Give a User-RCTRL-Block to EDD to handle the Request.     +*/
/*+               If no free EDD-Resources present the request remains      +*/
/*+               queued until resources are feed. If resources are free    +*/
/*+               the request is handled (doing a EDD-request)              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDApplyRCtrl(
    NARE_HDB_EDD_PTR_TYPE     pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser)
{
    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Queuing block (pHDB: 0x%X,Handle: 0x%X,pRCtrlUser: 0x%X)",pHDB,pHDB->ThisHandle,pHDB->ThisHandle);
    NARE_PUT_BLOCK_TOP(pHDB->PendRCtrl.pBottom,
                       pHDB->PendRCtrl.pTop,
                       pRCtrlUser);
    pRCtrlUser->QueueType = NARE_RCTRL_QTYPE_EDD_PENDING;
    pHDB->PendRCtrlCnt++;
    NARE_EDDCheckAndStartPending(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDRemoveRCtrl                         +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE      pHDB             +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to EDD -HDB                                       +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block to handle.                    +*/
/*+               Must contain valid Bstate which selects the action to do. +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removed RCTRL-Block from pending-queue.                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDRemoveRCtrl(
    NARE_HDB_EDD_PTR_TYPE     pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlHelp;
    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Removing block (pHDB: 0x%X, Handle: 0x%X,pRCtrlUser: 0x%X)",pHDB,pHDB->ThisHandle,pRCtrlUser);
    NARE_REM_BLOCK(pHDB->PendRCtrl.pBottom,
                   pHDB->PendRCtrl.pTop,
                   pRCtrlUser,
                   pRCtrlHelp);
    pHDB->PendRCtrlCnt--;
    NARE_ASSERT_NULL_PTR(pRCtrlHelp);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDOpenChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       NAREHandle            +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NAREHandle           : NARE EDD-Channel handle                         +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to EDD. The Request will be   +*/
/*+               finished by calling the EDD-Callbackfunktion.             +*/
/*+               nare_edd_request_lower_done() if NARE_RSP_OK.             +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{
    NARE_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    NARE_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE    pOpen;
    NARE_RSP                                    Result;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    NARE_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDOpenChannel(NAREHandle: 0x%X)",NAREHandle);
    Result = NARE_RSP_OK;
    UserId.uvar32 = 0;
    NARE_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(EDD_RQB_OPEN_CHANNEL_TYPE),pSys);
        pOpen = (NARE_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE)pAlloc;
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pOpen, LSA_NULL) )
        {
            NARE_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_OPEN_CHANNEL);
            NARE_EDD_RQB_SET_SERVICE(pRQB,0);
            pRQB->pParam       = pOpen;
            pOpen->HandleUpper = NAREHandle;
            pOpen->SysPath     = SysPath;
            pOpen->Cbf         = nare_edd_request_lower_done;
            
            NARE_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",NAREHandle,pRQB);
            NARE_EDD_OPEN_CHANNEL_LOWER(pRQB,pSys);
        }
        else
        {
            NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
            NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
            Result = NARE_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDOpenChannel. Rsp: 0x%X",Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDCloseChannel                        +*/
/*+  Input                      LSA_HANDLE_TYPE       EDDHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+                             NARE_RSP_ERR_RESOURCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  EDDHandle            : EDD handle to close                             +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to EDD. The Request will be  +*/
/*+               finished by calling the EDD-Callbackfunktion.             +*/
/*+               nare_edd_request_lower_done()                             +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDCloseChannel(
    LSA_HANDLE_TYPE       EDDHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    NARE_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    NARE_RSP                                    Result;
    LSA_USER_ID_TYPE                            UserId;

    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDCloseChannel(EDDHandle: 0x%X)",EDDHandle);
    Result = NARE_RSP_OK;
    UserId.uvar32 = 0;

    NARE_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_CLOSE_CHANNEL);
        NARE_EDD_RQB_SET_SERVICE(pRQB,0);
        NARE_EDD_RQB_SET_HANDLE(pRQB,EDDHandle);
        pRQB->pParam = LSA_NULL;
        NARE_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_OPC_CLOSE_CHANNEL (EDDHandle: 0x%X,pRQB: 0x%X)",EDDHandle,pRQB);
        NARE_EDD_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDCloseChannel()");
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    NARE_EDDFillARPRequest                             +*/
/*+  Input/Output   :    NARE_EDD_LOWER_MEM_PTR_TYPE            pTxFrame    +*/
/*+                      NARE_MAC_TYPE   NARE_LOCAL_MEM_ATTR   *pOwnMacAddr +*/
/*+                      NARE_LOCAL_MEM_IP_PTR_TYPE             pIPAddr     +*/
/*+                      NARE_LOCAL_MEM_IP_PTR_TYPE             pLocalIPAddr+*/
/*+                                                                         +*/
/*+  Result         :    NARE_RESULT                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId                : NICId of device.                                +*/
/*+  pTxFrame             : Pointer to Framebuffer to be filled             +*/
/*+                         (at least room for NARE_ARP_TX_FRAME_LEN bytes) +*/
/*+                         (has to be 32-Bit aligned!!!)                   +*/
/*+  pOwnMacAddr          : Pointer to own MAC address                      +*/
/*+  pIPAddr              : Pointer to IP address to resolve                +*/
/*+  pLocalIPAddr         : Pointer to local IP address (maybe 0.0.0.0)     +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_NO_OWN_IP                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Fills ARP-Request Frame.                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP  NARE_EDDFillARPRequest(
    NARE_EDD_LOWER_MEM_PTR_TYPE            pTxFrame,
    NARE_MAC_TYPE   NARE_LOCAL_MEM_ATTR   *pOwnMacAddr,
    NARE_LOCAL_MEM_IP_PTR_TYPE             pIPAddr,
    NARE_LOCAL_MEM_IP_PTR_TYPE             pLocalIPAddr)
{
    NARE_ARP_FRAME_PTR_TYPE pArpFrame;
    NARE_RSP                Result;
    pArpFrame = (NARE_ARP_FRAME_PTR_TYPE)pTxFrame;
    Result    = NARE_RSP_OK;
    /* set broadcastaddress as destination MAC */
    NARE_COPY_MAC_ADDR_LEM_GLB(pArpFrame->DstMacAddr,&NARE_EDDBroadcastMac);
    NARE_COPY_MAC_ADDR_LEM_LOC(pArpFrame->SrcMacAddr,pOwnMacAddr);
    pArpFrame->Type = NARE_HTONS(NARE_ARP_TYPE);
    pArpFrame->Hrd  = NARE_HTONS(NARE_ARP_HRD_ETHERNET);
    pArpFrame->Pro  = NARE_HTONS(NARE_ARP_PRO_IP);
    pArpFrame->Hln  = NARE_ARP_HLN;
    pArpFrame->Pln  = NARE_ARP_PLN;
    pArpFrame->Op   = NARE_HTONS(NARE_ARP_OP_REQUEST);
    NARE_COPY_MAC_ADDR_LEM_LOC(pArpFrame->Sha,pOwnMacAddr);
    NARE_COPY_MAC_ADDR_LEM_GLB(pArpFrame->Tha,&NARE_EDDNullMac); /* don't care: shoud be 0 due to RFC 5227 */
    NARE_COPY_IP_ADDR_LEM_LOC(pArpFrame->Spa,pLocalIPAddr);      /* local ip */
    NARE_COPY_IP_ADDR_LEM_LOC(pArpFrame->Tpa,pIPAddr);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDGetMACAddr                          +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to EDD-HDB                              +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get Params from EDD (e.g. MAC address)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDGetMACAddr(
    NARE_HDB_EDD_PTR_TYPE        pHDB)
{
    NARE_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    NARE_EDD_RQB_GET_PARAMS_PTR_TYPE            pGetParams;
    LSA_SYS_PTR_TYPE                            pSys;
    NARE_RSP                                    Result;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    NARE_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDGetMACAddr(pHandle: 0x%X)",pHDB);
    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    NARE_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(EDD_RQB_GET_PARAMS_TYPE),pSys);
        pGetParams = (NARE_EDD_RQB_GET_PARAMS_PTR_TYPE)pAlloc;
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pGetParams, LSA_NULL) )
        {
            NARE_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            NARE_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_GET_PARAMS);
            NARE_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            pRQB->pParam       = pGetParams;
            NARE_LOWER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PARAMS (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            NARE_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
            Result = NARE_RSP_ERR_RESOURCE;
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDGetMACAddr()");
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_EDDSendARP                             +*/
/*+  Input/Output          :    NARE_HDB_EDD_PTR_TYPE        pHDB           +*/
/*+                             NARE_EDD_RCTRL_PTR_TYPE      pRCtrl         +*/
/*+                             NARE_LOCAL_MEM_IP_PTR_TYPE   pIPAddr        +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to EDD-HDB                              +*/
/*+  pRCtrl               : EDD RCTRl-block pointer                         +*/
/*+  pIPAddr              : Pointer to IP address to resolve                +*/
/*+  pLocalIPAddr         : Pointer to local IP address                     +*/
/*+                         (0.0.0.0 if not present)                        +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends ARP-Frame with IP and own MAC/IP from HDB.          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDSendARP(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_EDD_RCTRL_PTR_TYPE      pRCtrl,
    NARE_LOCAL_MEM_IP_PTR_TYPE   pIPAddr,
    NARE_LOCAL_MEM_IP_PTR_TYPE   pLocalIPAddr)
{
    NARE_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
    NARE_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE        pSend;
    LSA_SYS_PTR_TYPE                            pSys;
    NARE_RSP                                    Result;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal;
    NARE_EDD_LOWER_MEM_PTR_TYPE                 pTxFrame;  /* Pointer to Tx-Frame memory. */
    NARE_EDD_LOWER_MEM_PTR_TYPE                 pAlloc;
    #ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    LSA_UINT16                                  AllocLength;
    #endif

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_EDDSendARP(pHandle: 0x%X)",pHDB);
    NARE_ASSERT_NULL_PTR(pRCtrl);
    NARE_ASSERT_NULL_PTR(pIPAddr);
    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    /*---------------------------------------------------------------------------*/
    /* allocate or get preallocated Framebuffer ptr.                             */
    /*---------------------------------------------------------------------------*/
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
        /* Because of a EDDI restriction the BufferLength have to be a      */
    /* multiple of 4 Bytes. So we extend the Buffer if needed..         */
    /* This is neccessary with EDDI because of HW-Padding of short      */
    /* frames workaround.                                               */
    AllocLength = (LSA_UINT16)((NARE_ARP_TX_FRAME_LEN + 3) & ~0x3); 
    NARE_EDD_ALLOC_LOWER_TXMEM(&pTxFrame,
                               UserId,
                               AllocLength,
                               pSys);
    if ( LSA_HOST_PTR_ARE_EQUAL(pTxFrame, LSA_NULL) )
    {
        Result = NARE_RSP_ERR_RESOURCE;
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"NARE_EDDSendARP(). Allocation pTxFrame memory failed.");
    }
#else
    pTxFrame = pRCtrl->pTxFrame; /* get Frame-Pointer from RCTRL-Block */
    NARE_ASSERT_NULL_PTR(pTxFrame);
#endif
    /*---------------------------------------------------------------------------*/
    /* Allocate EDD-Send RQB, fill ARP-Request and send request to EDD           */
    /*---------------------------------------------------------------------------*/
    if ( Result == NARE_RSP_OK )
    {
        LSA_UINT8 ReqOpcode;

        ReqOpcode = NARE_RQB_GET_OPCODE(pRCtrl->pRCtrl->pRQB);
       /* Fill ARP-Request with own MAC/IP and IO-Device IP */
        Result = NARE_EDDFillARPRequest(pTxFrame,
                                        &pHDB->MACAddr,
                                        pIPAddr,
                                        pLocalIPAddr);
        if ( Result == NARE_RSP_OK)
        {
            NARE_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);
            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
            {
                NARE_EDD_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(EDD_RQB_NRT_SEND_TYPE),pSys);
                pSend = (NARE_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE)pAlloc;
                if ( ! LSA_HOST_PTR_ARE_EQUAL(pSend, LSA_NULL) )
                {
                    NARE_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
                    NARE_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_NRT_SEND);
                    NARE_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
                    /* we use User-ID for RCtrl-identification */
                    NARE_EDD_RQB_SET_USERID_UVAR32(pRQB,pRCtrl->ThisIdx);
                    pRQB->pParam       = pSend;
                    pSend->Length      = NARE_ARP_TX_FRAME_LEN;
                    pSend->pBuffer     = (EDD_UPPER_MEM_U8_PTR_TYPE)pTxFrame;
                    pSend->Priority    = EDD_NRT_SEND_PRIO_0;
                    pSend->PortID      = EDD_PORT_ID_AUTO;
                    pSend->TxFrmGroup  = EDD_NRT_TX_GRP_ARP;

                    pRCtrl->pARP_RQB    = pRQB;
                    pRCtrl->pHDBEDD     = pHDB;
                    pRCtrl->ARP_IO_DONE = LSA_FALSE;
                    NARE_LOWER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_NRT_SEND (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
                    /* AP01248034 : use an initial delay If opc = NARE_OPC_TEST_IP and UseDelay == TRUE */
                    /* so we skip sending in this case ! */
                    if((ReqOpcode == NARE_OPC_TEST_IP) && (pRCtrl->pRCtrl->pRQB->args.TestIP.UseDelay))
                    {
                        pRCtrl->ARP_IO_DONE = LSA_TRUE;  /* the real request is skipped, so wait for the timer */
                    }
                    else
                    {
                        NARE_EDD_REQUEST_LOWER(pRQB,pSys);
                        NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Sending ARP (Handle: 0x%X,pRQB: 0x%X, pFrame: 0x%X)",pHDB->ThisHandle,pRQB,pTxFrame);
                    }
                }
                else
                {
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
                    NARE_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
                    NARE_MEM_FREE_FAIL_CHECK(RetVal);
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
                    NARE_EDD_FREE_LOWER_TXMEM(&RetVal,pTxFrame,pSys);
                    NARE_MEM_FREE_FAIL_CHECK(RetVal);
#endif
                    Result = NARE_RSP_ERR_RESOURCE;
                }
            }
            else
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
                NARE_EDD_FREE_LOWER_TXMEM(&RetVal,pTxFrame,pSys);
                NARE_MEM_FREE_FAIL_CHECK(RetVal);
#endif
                Result = NARE_RSP_ERR_RESOURCE;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"EDD-Filling ARP-Frame failed (no own IP-Addr)!");
#ifndef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
            NARE_EDD_FREE_LOWER_TXMEM(&RetVal,pTxFrame,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
#endif
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_EDDSendARP()");
    return(Result);
}

/*****************************************************************************/
/*  end of file NARE_EDD.C                                                    */
/*****************************************************************************/

