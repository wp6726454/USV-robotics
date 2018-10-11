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
/*  F i l e               &F: nare_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE user-interface functions with RQBs          */
/*                                                                           */
/*                          - nare_open_channel()                            */
/*                          - nare_request()                                 */
/*                          - nare_close_channel()                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  9
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_TIM */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "nare_inc.h"            /* NARE headerfiles */
#include "nare_int.h"            /* internal header */

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* State-sequences RState within User-RCtrl-Block points to depending on mode*/
/*---------------------------------------------------------------------------*/

/* states for Opcode: NARE_OPC_RESOLVE_CONFIG Mode: NARE_MODE_CONFIG_NOS_FIXED */
const LSA_UINT8 NareStatesModeCNF[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_DCP_IDENT,
    NARE_RCTRL_RSTATE_EDD_ARP,
    NARE_RCTRL_RSTATE_DCP_SET,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_RESOLVE_CONFIG Mode: NARE_MODE_CONFIG_IP_FIXED */
const LSA_UINT8 NareStatesModeCIF[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_DCP_IDENT,
    NARE_RCTRL_RSTATE_EDD_ARP,
    NARE_RCTRL_RSTATE_DCP_SET,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_RESOLVE_CONFIG Mode: NARE_MODE_CONFIG_IP_DNS   */
const LSA_UINT8 NareStatesModeCID[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_SOCK_GHBN,
    NARE_RCTRL_RSTATE_DCP_IDENT,
    NARE_RCTRL_RSTATE_EDD_ARP,
    NARE_RCTRL_RSTATE_DCP_SET,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_RESOLVE_CONFIG Mode: NARE_MODE_RESOLVE_DCP     */
const LSA_UINT8 NareStatesModeRDCP[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_DCP_IDENT,
    NARE_RCTRL_RSTATE_EDD_ARP,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_RESOLVE_CONFIG Mode: NARE_MODE_RESOLVE_DNS     */
const LSA_UINT8 NareStatesModeRDNS[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_SOCK_GHBN,
    NARE_RCTRL_RSTATE_EDD_ARP,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_TEST_IP         */
const LSA_UINT8 NareStatesTestIP[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_RESOLVE_IP      */
const LSA_UINT8 NareStatesResolveIP[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_EDD_RES_IP_ARP,
    NARE_RCTRL_RSTATE_READY,
};

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

#ifndef NARE_CFG_DONT_VALIDATE_IP_SUITE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREValidateIPSuite                         +*/
/*+  Input/Output          :    NARE_IP_TYPE   IPAddr                       +*/
/*+                             NARE_IP_TYPE   SubMask                      +*/
/*+                             NARE_IP_TYPE   DefRouter                    +*/
/*+  Result                :    LSA_TRUE: OK                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if IP-Suite is valid (as defined by PNIO norm)     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL  NARE_LOCAL_FCT_ATTR NAREValidateIPSuite(
    NARE_IP_TYPE  IPAddr,
    NARE_IP_TYPE  SubMask,
    NARE_IP_TYPE  DefRouter)
{
    LSA_UINT32                Msk;
    LSA_UINT32                AktMsk1;
    LSA_UINT32                AktMsk2;
    LSA_BOOL                  Status;

    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"IN : NAREValidateIPSuite()");
    Status = LSA_TRUE;
    /* --------------------------------------------------------------------------*/
    /* check IPAddr                                                              */
    /* --------------------------------------------------------------------------*/
    /* Not allowed: 127.0.0.0/8 e.g. 7F.xx.xx.xx */
    /* Not allowed: 224.0.0.0/4 e.g. Ex.xx.xx.xx */
    if ( (IPAddr[0] == 0x7F) || ((IPAddr[0] & 0xE0) == 0xE0))
    {
        NARE_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,"Invalid IPAddress: %d.%d.%d.%d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
        Status = LSA_FALSE;
    }
    /* --------------------------------------------------------------------------*/
    /* check DefRouter                                                           */
    /* --------------------------------------------------------------------------*/
    /* Not allowed: 127.0.0.0/8 e.g. 7F.xx.xx.xx */
    /* Not allowed: 224.0.0.0/4 e.g. Ex.xx.xx.xx */
    if ( (DefRouter[0] == 0x7F) || ((DefRouter[0] & 0xE0) == 0xE0))
    {
        NARE_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,"Invalid DefRouter Address: %d.%d.%d.%d",DefRouter[0],DefRouter[1],DefRouter[2],DefRouter[3]);
        Status = LSA_FALSE;
    }
    /* --------------------------------------------------------------------------*/
    /* subnet mask                                                               */
    /* --------------------------------------------------------------------------*/
    /* a subnetmask is valid, if a bitfield pair '1,0' exists which has on the   */
    /* "left" only bits '1' and on the "right" only bits '0'                     */
    Msk   = ((LSA_UINT32)SubMask[0] << 24) +
            ((LSA_UINT32)SubMask[1] << 16) +
            ((LSA_UINT32)SubMask[2] << 8 ) +
             (LSA_UINT32)SubMask[3];
    AktMsk1 = 0x8000000l;
    AktMsk2 = 0xFFFFFFFl;
    /* search first transition from 1 to 0 */
    while ((AktMsk1) && (Msk & AktMsk1 ))
    {
        AktMsk1 >>= 1;
        AktMsk2 >>= 1;
    }
    /* no more 1 allowed behind first transition */
    if ( AktMsk2 & Msk )
    {
        NARE_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,"Invalid Subnetmask: %d.%d.%d.%d",SubMask[0],SubMask[1],SubMask[2],SubMask[3]);
        Status = LSA_FALSE;
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NAREValidateIPSuite()");
    return(Status);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_CheckIpSuiteIdentically                +*/
/*+  Input/Output          :    NARE_CONFIG_TYPE  NARE_xx_ATTR * pConfig    +*/
/*+                             NARE_RESOLVE_TYPE NARE_xx_ATTR * pResolve   +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pConfig    : Pointer to IP Suite to configure                          +*/
/*+  pResolve   : Pointer to IP Suite actually set                          +*/
/*+                                                                         +*/
/*+  Result:      LSA_TRUE  IP Suite is identically                         +*/
/*+               LSA_FALSE IP Suite is different                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if the IP suites are identically                   +*/
/*+                                                                         +*/
/*+               We check if the combination of DefRouter and IPAddr       +*/
/*+               is the same.                                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+    Default:   Identically if all three parameters match:                +*/
/*+                                                                         +*/
/*+    if NARE_DEF_ROUTER_DEEP_CHECK is defined (deactivated by default)    +*/
/*+               * If IPAddr is identically and                            +*/
/*+                 * Defrouter and SubMask is identically                  +*/
/*+                 OR                                                      +*/
/*+                 * resolve DefRouter == 0 and config DefRouter ==        +*/
/*+                                              config IPAddr              +*/
/*+                 OR                                                      +*/
/*+                 * config  DefRouter == 0 and resolve DefRouter ==       +*/
/*+                                              resolve IPAddr             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               Note: We dont check if the combination of Subnetmask and  +*/
/*+                     DefaultRouter is valid!                             +*/
/*+                     If is recommended, that if the subnetmask is 0.0.0.0+*/
/*+                     the default router shall be 0.0.0.0 or the IPAddr   +*/
/*+               Note: If the DefaultRouter is the IPAddr or 0.0.0.0 the   +*/
/*+                     Subnetmask don´t care.                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL NARE_LOCAL_FCT_ATTR NARE_CheckIpSuiteIdentically(
    NARE_CONFIG_TYPE          NARE_UPPER_MEM_ATTR * pConfig,
    NARE_RESOLVE_TYPE         NARE_UPPER_MEM_ATTR * pResolve)
{
    LSA_BOOL Identically;

    NARE_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"IN : NARE_CheckIpSuiteIdentically(pConfig: 0x%X, pResolve: 0x%X)",pConfig,pResolve);

    Identically = LSA_FALSE;
    /* checks if Ip-Suite is identically */
    /* If IP is identically */
    if ( NARE_IP_ADDR_EQUAL_UR_UR(pConfig->IPAddr,pResolve->IPAddr))
    {
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"NARE_CheckIpSuiteIdentically: IP identically");
        /* If Subnetmask/router is identically */
        if (   ( NARE_IP_ADDR_EQUAL_UR_UR(pConfig->SubMask,pResolve->SubMask))
               && ( NARE_IP_ADDR_EQUAL_UR_UR(pConfig->DefRouter,pResolve->DefRouter)))
        {
            NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"NARE_CheckIpSuiteIdentically: SubMask,DefRouter identically");
            Identically = LSA_TRUE;
        }
        else
        {
            /* default: NOT defined! */
#ifdef NARE_DEF_ROUTER_DEEP_CHECK
            /* Note: If subnetmask == 0.0.0.0,                          */
            /* DefRouter has to be 0.0.0.0 or IPAddr                    */
            /* This is not verified.                                    */
            
            /* If ResolveDefRouter == 0.0.0.0 && ConfigDefRouter == ConfigIpAddr */
            /* ==> no router                                                     */
            if (   ( NARE_IP_ADDR_ZERO_UR(pResolve->DefRouter))
                && ( NARE_IP_ADDR_EQUAL_UR_UR(pConfig->DefRouter,pConfig->IPAddr)))
            {
                NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"NARE_CheckIpSuiteIdentically: ResRouter is 0 and CfgRouter = CfgIP");
                /* subnetmask is don´t care */
                Identically = LSA_TRUE;
            }
            else
            {
                /* If ConfigDefRouter == 0.0.0.0 && ResolveDefRouter == ResolveIPAddr*/
                /* ==> no router                                                     */
                if (   ( NARE_IP_ADDR_ZERO_UR(pConfig->DefRouter))
                    && ( NARE_IP_ADDR_EQUAL_UR_UR(pResolve->DefRouter,pResolve->IPAddr)))
                {
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"NARE_CheckIpSuiteIdentically: CfgRouter is 0 and ResRouter = ResIP");
                    /* subnetmask is don´t care */
                    Identically = LSA_TRUE;
                }
                else
                {
                    /* If ConfigIP == ConfigDefRouter && ResolveDefRouter == ConfigDefRouter     */
                    /* ==> no router                          */
                    if (   ( NARE_IP_ADDR_EQUAL_UR_UR(pConfig->DefRouter,pConfig->IPAddr))
                        && ( NARE_IP_ADDR_EQUAL_UR_UR(pConfig->DefRouter,pResolve->DefRouter)))
                    {
                        if ( NARE_IP_ADDR_EQUAL_UR_UR(pResolve->DefRouter,pConfig->DefRouter))
                        {
                            NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"NARE_CheckIpSuiteIdentically: CfgRouter equal ResRouter equal IP");
                            /* subnetmask is don´t care */
                            Identically = LSA_TRUE;
                        }
                    }
                }
            }
#endif
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"NARE_CheckIpSuiteIdentically: IP NOT identically");
    }
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_CheckIpSuiteIdentically(Identically: 0x%X)",Identically);
    return(Identically);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserReady                              +*/
/*+  Input                      NARE_HDB_USER_PTR_TYPE       pHDB           +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE     pRCtrlUser     +*/
/*+                             NARE_RSP                     Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  RCtrlUser  : Pointer to User-RCTRL-Block                               +*/
/*+  Response   : Responsecode to finish request                             */
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees the RCtrlUser and finished the User-Request with    +*/
/*+               Response by calling the Channel-Call-Back.                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_UserReady(
    NARE_HDB_USER_PTR_TYPE      pHDB,
    NARE_USER_RCTRL_PTR_TYPE    pRCtrlUser,
    NARE_RSP                    Response)
{
    NARE_UPPER_RQB_PTR_TYPE  pRQB;

    pRQB = pRCtrlUser->pRQB;
    /* release the user-RCtrl-block      */
    NARE_UserReleaseRCTRL(pHDB,pRCtrlUser);
    /* finish the request */
    NAREUserRequestFinish(pHDB,pRQB,Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserStateMachine                       +*/
/*+  Input/Output          :    NARE_USER_RCTRL_PTR_TYPE       pRCtrlUser   +*/
/*+                             NARE_RSP                       Response     +*/
/*+                             LSA_BOOL                       CheckPending +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block                               +*/
/*+  Response   : Response of last action. if <> NARE_RSP_OK the Request    +*/
/*+               will be finished with this error. Set to NARE_RSP_OK if   +*/
/*+               not error present (e.g. on first call)                    +*/
/*+  CheckPending:LSA_TRUE if NARE_UserCheckPending(pHDB) should be called  +*/
/*+                        after a request was finised.                     +*/
/*+               LSA_FALSE if not.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Statemaching for User-Requests.                           +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+               CheckPending should be set to LSA_TRUE if called within   +*/
/*+               NARE_UserCheckPending() to avoid multiple reentrant and   +*/
/*+               therefore increasing of stack. it is not neccessary to    +*/
/*+               call NARE_UserCheckPending() if we are called from it.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserStateMachine(
    NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser,
    NARE_RSP                   Response,
    LSA_BOOL                   CheckPending)
{
    NARE_HDB_USER_PTR_TYPE     pHDB;
    LSA_UINT8                  ActState;
    LSA_BOOL                   StartPending;
    NARE_UPPER_RQB_PTR_TYPE    pRQB;
    NARE_UPPER_RQB_PTR_TYPE    pRQBCancel;
    LSA_BOOL                   Ready;
    LSA_BOOL                   Cancel;

    StartPending = LSA_FALSE;
    pRQB         = pRCtrlUser->pRQB;
    NARE_ASSERT_NULL_PTR(pRQB);
    pHDB = NAREGetUserHDBFromHandle(NARE_RQB_GET_HANDLE(pRQB));
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_UserStateMachine(pRCtrlUser: 0x%X, Resp: 0x%X)",pRCtrlUser,Response);
    ActState = *pRCtrlUser->pRState;
    /*---------------------------------------------------------------------------*/
    /* finish handling before advance to next state                              */
    /*---------------------------------------------------------------------------*/
    switch (ActState)
    {
        case NARE_RCTRL_RSTATE_START:
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Start",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
            break;

        case NARE_RCTRL_RSTATE_SOCK_GHBN:
#ifndef NARE_CFG_NO_SOCK
            /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: SOCK-GetHostByName ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            /* On success Cfg.IPAddr is set with the resolved IP-Address  */
            /* We copy this IP-Address to the Res-field. The Res-field    */
            /* may be overwritten later if we do a DCP-Identity or ARP    */
            /* failes. But for mode NARE_MODE_RESOLVE_DNS this will be    */
            /* the result if no futher error occure with ARP.             */
            if ( Response == NARE_RSP_OK)
            {
                NARE_COPY_IP_ADDR_UR_UR(&pRQB->args.ResConf.Res.IPAddr,
                                        &pRQB->args.ResConf.Cfg.IPAddr);
 #ifndef NARE_CFG_DONT_VALIDATE_IP_SUITE
                if (  pRQB->args.ResConf.Mode == NARE_MODE_CONFIG_IP_DNS )
                {
                    /* we now check if the IPSuite to be set is valid */
                    if ( ! NAREValidateIPSuite(pRQB->args.ResConf.Cfg.IPAddr,
                                               pRQB->args.ResConf.Cfg.SubMask,
                                               pRQB->args.ResConf.Cfg.DefRouter))
                    {
                        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR,"ResConf-Request(DNS). Invalid IP-Suite parameters!");
                        Response = NARE_RSP_ERR_INVALID_IP_PARAMS;
                    }
                }
 #endif
            }
            /* after finishing a SOCK-Request we check and start further  */
            /* possible pending SOCK-Request already in SOCK-queue.       */
            /* This may result in finishing user-request if starting SOCK */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */

            NARE_SOCKCheckAndStartPending(pHDB->pHDBSock);
            StartPending = LSA_TRUE;
#else
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid RState (%Xh)",*pRCtrlUser->pRState);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
#endif
            break;

        case NARE_RCTRL_RSTATE_DCP_IDENT:
            /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: DCP-Ident ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            /* The result is within Res-field of RQB. We copy the IP      */
            /* to the cnf-field if mode <> NARE_MODE_CONFIG_IP_FIXED      */
            /* and <> NARE_MODE_CONFIG_IP_DNS because we use this IP for  */
            /* ARP-Check later. In modes config-modes the ip is filled    */
            /* either by user (IP_FIXED) or from DNS-Call (IP_DNS)        */
            if (( Response == NARE_RSP_OK) &&
                ( pRQB->args.ResConf.Mode != NARE_MODE_CONFIG_IP_FIXED ) &&
                ( pRQB->args.ResConf.Mode != NARE_MODE_CONFIG_IP_DNS   ))
            {
                NARE_COPY_IP_ADDR_UR_UR(&pRQB->args.ResConf.Cfg.IPAddr,
                                        &pRQB->args.ResConf.Res.IPAddr);
                NARE_COPY_IP_ADDR_UR_UR(&pRQB->args.ResConf.Cfg.SubMask,
                                        &pRQB->args.ResConf.Res.SubMask);
                NARE_COPY_IP_ADDR_UR_UR(&pRQB->args.ResConf.Cfg.DefRouter,
                                        &pRQB->args.ResConf.Res.DefRouter);
            }
            /* after finishing a DCP-Request we check and start further   */
            /* possible pending DCP-Request already in DCP-queue.         */
            /* This may result in finishing user-request if starting DCP  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */
            NARE_DCPCheckAndStartPending(pHDB->pHDBDcp);
            StartPending = LSA_TRUE;
            break;
        case NARE_RCTRL_RSTATE_DCP_SET:
            /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */

            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: DCP-Set ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);

            /* after finishing a DCP-Request we check and start further   */
            /* possible pending DCP-Request already in DCP-queue.         */
            /* This may result in finishing user-request if starting DCP  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */

            NARE_DCPCheckAndStartPending(pHDB->pHDBDcp);
            StartPending = LSA_TRUE;
            break;

        case NARE_RCTRL_RSTATE_EDD_ARP:
            /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */

            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);

            /* After ARP-Request is ready (including the sniffing) we     */
            /* check and start further possible pending EDD-Requests      */
            /* already in EDD-queue.                                      */
            /* This may result in finishing user-request if starting EDD  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */

            NARE_EDDCheckAndStartPending(pHDB->pHDBEdd);
            StartPending = LSA_TRUE;
            break;

        case NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP:
            /* Note: This state only for NARE_OPC_TEST_IP */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP for test-IP ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            /* After ARP-Request is ready (including the sniffing) we     */
            /* check and start further possible pending EDD-Requests      */
            /* already in EDD-queue.                                      */
            /* This may result in finishing user-request if starting EDD  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */

            NARE_EDDCheckAndStartPending(pHDB->pHDBEdd);
            StartPending = LSA_TRUE;
            break;

        case NARE_RCTRL_RSTATE_EDD_RES_IP_ARP:
            /* Note: This state only for NARE_OPC_RESOLVE_IP */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP for Resolve-IP ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            /* After ARP-Request is ready (including the sniffing) we     */
            /* check and start further possible pending EDD-Requests      */
            /* already in EDD-queue.                                      */
            /* This may result in finishing user-request if starting EDD  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */
            NARE_EDDCheckAndStartPending(pHDB->pHDBEdd);
            StartPending = LSA_TRUE;
            break;

        default:
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid RState (%Xh)",*pRCtrlUser->pRState);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    }
    /*---------------------------------------------------------------------------*/
    /* On error we dont advance to next state, instead we finish the request.    */
    /* On cancel we abort too.                                                   */
    /*---------------------------------------------------------------------------*/
    if (( Response != NARE_RSP_OK ) || ( pRCtrlUser->DoCancel))
    {
        Cancel       = LSA_FALSE;
        pRQBCancel   = LSA_NULL;
        if ( pRCtrlUser->DoCancel  )
        {
            pRQBCancel = pRCtrlUser->pCancelRQB; /* save the CancelRQB */
            Response   = NARE_RSP_OK_CANCEL;
            switch ( NARE_RQB_GET_OPCODE(pRQB) )
            {
                case NARE_OPC_RESOLVE_CONFIG:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: NARE_OPC_RESOLVE_CONFIG Canceling",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* RequestID temporary used as Cancel-Pending Counter. if 0 this was */
                    /* the last Reqeust to be canceled, so the Cancel is ready.          */
                    if ( ! --pRQBCancel->args.ResConfCancel.RequestID )
                    {
                        Cancel   = LSA_TRUE;
                        /* restore RequestID */
                        pRQBCancel->args.ResConfCancel.RequestID = pRQB->args.ResConf.RequestID;
                    }
                    break;

                case NARE_OPC_TEST_IP:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: NARE_OPC_TEST_IP Canceling",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* RequestID temporary used as Cancel-Pending Counter. if 0 this was */
                    /* the last Reqeust to be canceled, so the Cancel is ready.          */
                    if ( ! --pRQBCancel->args.TestIPCancel.RequestID )
                    {
                        Cancel   = LSA_TRUE;
                        /* restore RequestID */
                        pRQBCancel->args.TestIPCancel.RequestID = pRQB->args.TestIP.RequestID;
                    }
                    break;

                default:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid opcode on cancel (%d)",NARE_RQB_GET_OPCODE(pRQB));
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;
            }
        }
        else
        {
            if (Response == NARE_RSP_OK_CANCEL)
            {
                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Request Canceling (Opcode: 0x%X)",pHDB->ThisHandle,pRCtrlUser->ThisIdx, NARE_RQB_GET_OPCODE(pRQB));
            }
            else
            {
                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Response failure (0x%X)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            }
        }
        /* release RCtrl-Block and finish the request */
        NARE_UserReady(pHDB,pRCtrlUser,Response);
        /* Finish Cancel-request */
        if ( Cancel )
        {
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Finishing Cancel-request (RequestID: 0x%X)",pRQBCancel->args.ResConfCancel.RequestID);
            NAREUserRequestFinish(pHDB,pRQBCancel,NARE_RSP_OK);
        }
        StartPending = LSA_TRUE;
    }
    else
    {
        Ready = LSA_FALSE;
        /* -----------------------------------------------------------------------*/
        /* forward in statemachine till an asynchroneous request is underway which*/
        /* we have to wait for.                                                   */
        /* -----------------------------------------------------------------------*/
        while (! Ready)
        {
            pRCtrlUser->pRState++; /* switch to next state */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Next State: %d",pHDB->ThisHandle,pRCtrlUser->ThisIdx,*pRCtrlUser->pRState);
            Ready = LSA_TRUE;      /* Default: break loop because request underway*/
            /*--------------------------------------------------------------------*/
            /* do handling for this state                                         */
            /*--------------------------------------------------------------------*/
            switch (*pRCtrlUser->pRState )
            {
                case NARE_RCTRL_RSTATE_SOCK_GHBN:
#ifndef NARE_CFG_NO_SOCK
                    /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: SOCK-GetHostByName request",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* Try to start a SOCK -request. This may cause a immediate   */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    NARE_SOCKApplyRCtrl(pHDB->pHDBSock,pRCtrlUser);
                    StartPending = LSA_TRUE;
#else
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid RState (%Xh)",*pRCtrlUser->pRState);
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
#endif
                    break;

                case NARE_RCTRL_RSTATE_DCP_IDENT:
                    /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: DCP-Ident request",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* Try to start a DCP-request. This may cause a immidiate     */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    NARE_DCPApplyRCtrl(pHDB->pHDBDcp,pRCtrlUser);
                    StartPending = LSA_TRUE;
                    break;

                case NARE_RCTRL_RSTATE_EDD_ARP:
                    /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP request",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* If "CheckIP" is TRUE we always do an IP check if we have   */
                    /* a IP address to check. see below                           */
                    /* If "CheckIP" is FALSE we only check IP if it has to be SET */
                    if (! pRCtrlUser->pRQB->args.ResConf.CheckIP)
                    {
                        if (( pRCtrlUser->pRQB->args.ResConf.Mode == NARE_MODE_CONFIG_IP_FIXED ) ||
                            ( pRCtrlUser->pRQB->args.ResConf.Mode == NARE_MODE_CONFIG_IP_DNS   ))
                        {
                            /* we check if the device already has the correct IP setting  */
                            /* If so we dont do an IP check.                              */
                            /* Note that cfg.IPAddr must contain the desired address and  */
                            /*           res.IPAddr the actual address.                   */
                            if ( NARE_CheckIpSuiteIdentically(&pRCtrlUser->pRQB->args.ResConf.Cfg,
                                                              &pRCtrlUser->pRQB->args.ResConf.Res))
                            {
                                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: ARP-skipped (IP already set)",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                                Ready = LSA_FALSE; /* skip ARP */
                            }
                        }
                        else
                        {
                            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: ARP-skipped (no CONFIG)",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                            Ready = LSA_FALSE; /* skip ARP with other Modes */
                        }
                    }
                    /* Try to start a EDD-ARP-request. This may cause a immediate */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    /* Note that cfg.IPAddr must contain the address to ARP for   */
                    if (Ready)
                    {
                        if (! NARE_IP_ADDR_ZERO_UR(pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr))
                        {
                            NARE_EDDApplyRCtrl(pHDB->pHDBEdd,pRCtrlUser);
                            StartPending = LSA_TRUE;
                        }
                        else
                        {
                            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: ARP-skipped (IP is 0)",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                            Ready = LSA_FALSE; /* skipp ARP-Sniffer because no IP present */
                        }
                    }
                    break;

                case NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP:
                    /* Note: This state only for NARE_OPC_TEST_IP */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP request for testing IP",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* Try to start a EDD-ARP-request. This may cause a immediate */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    /* Note that cfg.IPAddr must contain the address to ARP for   */
                    NARE_EDDApplyRCtrl(pHDB->pHDBEdd,pRCtrlUser);
                    StartPending = LSA_TRUE;
                    break;

                case NARE_RCTRL_RSTATE_EDD_RES_IP_ARP:
                    /* Note: This state only for NARE_OPC_RESOLVE_IP */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP request for resolving IP",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* Try to start a EDD-ARP-request. This may cause a immediate */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    /* Note that cfg.IPAddr must contain the address to ARP for   */
                    NARE_EDDApplyRCtrl(pHDB->pHDBEdd,pRCtrlUser);
                    StartPending = LSA_TRUE;
                    break;

                case NARE_RCTRL_RSTATE_DCP_SET:
                    /* Note: This state only for NARE_OPC_RESOLVE_CONFIG */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: DCP-Set request",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* we check if we need to do a DCP-Set request. If the actual */
                    /* IP-Address of the IO-Device is equal to the desired one    */
                    /* we dont need a DCP-Set for IP-Parameters                   */
                    /* Note that cfg.IPAddr must contain the desired address and  */
                    /*           res.IPAddr the actual address.                   */
                    pRCtrlUser->DCPSetMode = NARE_RCTRL_DCP_SET_NONE;
                    /* With NARE_MODE_CONFIG_NOS_FIXED we never set an IP-Address */
                    if (pRCtrlUser->pRQB->args.ResConf.Mode != NARE_MODE_CONFIG_NOS_FIXED)
                    {
                        if ( ! NARE_CheckIpSuiteIdentically(&pRCtrlUser->pRQB->args.ResConf.Cfg,
                                                            &pRCtrlUser->pRQB->args.ResConf.Res))
                        {
                            pRCtrlUser->DCPSetMode |=  NARE_RCTRL_DCP_SET_IP;
                        }
                    }
                    /* if the station name is not already set we have to set the  */
                    /* stationname (Note: we dont get here if the name is already */
                    /* set with another stationname because we set the error-code */
                    /* NARE_RSP_ERR_STATION_NAME_DIFFERENT!                       */
                    if ( pRCtrlUser->DCPStationNameState != NARE_RCTRL_DCP_STATION_NAME_SET_EQUAL )
                    {
                        pRCtrlUser->DCPSetMode |=  NARE_RCTRL_DCP_SET_STATIONNAME;
                    }
                    if ( pRCtrlUser->DCPSetMode != NARE_RCTRL_DCP_SET_NONE ) /* something to set ? */
                    {
                        /* Try to start a DCP-request. This may cause a immediate */
                        /* finishing off the request because of no resources. so  */
                        /* it is possible, that a user-RCTRL-Block gets free. So  */
                        /* we have to check for queued user-request later..       */
                        NARE_DCPApplyRCtrl(pHDB->pHDBDcp,pRCtrlUser);
                        StartPending = LSA_TRUE;
                    }
                    else
                    {
                        NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: DCP-Set skipped (IP already set)",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                        Ready = LSA_FALSE; /* skipp DCP.Set and go to next state */
                    }
                    break;

                case NARE_RCTRL_RSTATE_READY:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: User-Request successfully finished",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* The request is ready. we finish it and check for further   */
                    /* pending user-requests.                                     */
                    NARE_UserReady(pHDB,pRCtrlUser,Response);
                    StartPending = LSA_TRUE;
                    break;

                default:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid RState (%Xh)",*pRCtrlUser->pRState);
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;
            } /* switch */
        } /* while */
    }
    /*---------------------------------------------------------------------------*/
    /* if a user-request was finished we have to check for already pending       */
    /* user-request which can be startet (queued in user-handle). Within         */
    /* NARE_UserCheckPending() we call NARE_UserStateMachine() again             */
    /* (reentrance) but with "CheckPending = FALSE" because we already try       */
    /* to start pending requests.                                                */
    /*---------------------------------------------------------------------------*/
    if ( StartPending && CheckPending )
    {
        /* check for pending user-requests   */
        NARE_UserCheckPending(pHDB);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_UserStateMachine()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserCheckPending                       +*/
/*+  Input/Output          :    NARE_HDB_USER_PTR_TYPE             pHDB     +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserCheckPending(
    NARE_HDB_USER_PTR_TYPE    pHDB)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    LSA_BOOL                  Ready;
    NARE_UPPER_RQB_PTR_TYPE   pRQB;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_UserCheckPending(pHDB: 0x%X)",pHDB);
    Ready = LSA_FALSE;
    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-Checking for pending USER-requests (pHDB: 0x%X)",pHDB);
    while (! Ready )
    {
        if ( pHDB->UserReqPendCnt ) /* User-RQB pending ? */
        {
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-requests pending (pHDB: 0x%X)",pHDB);
            if (pHDB->UserRCtrl.Free) /* Free USER-RCTRL-Blocks ? */
            {
                NARE_RQB_REM_BLOCK_BOTTOM(pHDB->UserReq.pBottom,
                                          pHDB->UserReq.pTop,
                                          pRQB);
                /* because UserReqPendCnt was >0 we dont expect LSA_NULL here */
                NARE_ASSERT_NULL_PTR(pRQB);
                pHDB->UserReqPendCnt--;
                pRCtrlUser= NARE_UserGetFreeRCTRL(pHDB);
                /* because Free was >0 we dont expect LSA_NULL here         */
                NARE_ASSERT_NULL_PTR(pRCtrlUser);
                /* Setup RCTRL-Block with init-values for state-maching      */
                pRCtrlUser->pRQB   = pRQB;
                /* Setup States to start of Statesequence depending on opcode*/
                switch ( NARE_RQB_GET_OPCODE(pRQB))
                {
                    case NARE_OPC_RESOLVE_CONFIG:
                        switch ( pRQB->args.ResConf.Mode )
                        {
                            case NARE_MODE_CONFIG_IP_DNS:
                                pRCtrlUser->pRState = &NareStatesModeCID[0];
                                break;

                            case NARE_MODE_CONFIG_IP_FIXED:
                                pRCtrlUser->pRState = &NareStatesModeCIF[0];
                                break;

                            case NARE_MODE_RESOLVE_DCP:
                                pRCtrlUser->pRState = &NareStatesModeRDCP[0];
                                break;

                            case NARE_MODE_RESOLVE_DNS:
                                pRCtrlUser->pRState = &NareStatesModeRDNS[0];
                                break;

                            case NARE_MODE_CONFIG_NOS_FIXED:
                                pRCtrlUser->pRState = &NareStatesModeCNF[0];
                                break;

                            default:
                                /* should already be checked.*/
                                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"ResConf-Request. Wrong Mode (0x%X)",pRQB->args.ResConf.Mode);
                                NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                                break;
                        }
                        break;

                    case NARE_OPC_TEST_IP:
                        pRCtrlUser->pRState = &NareStatesTestIP[0];
                        break;

                    case NARE_OPC_RESOLVE_IP:
                        pRCtrlUser->pRState = &NareStatesResolveIP[0];
                        break;

                    default:
                        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"Unexpected RQB-Opcode (0x%X)",NARE_RQB_GET_OPCODE(pRQB));
                        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                        break;
                }
                /* Call with LSA_FALSE, because we already in a loop */
                /* handling all user-request, so we dont need to call*/
                /* us again within NARE_UserStateMachine().          */
                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-Free User RCTRL-Found [Idx: %d]. Starting-Statemachine.",pRCtrlUser->ThisIdx);
                NARE_UserStateMachine(pRCtrlUser,NARE_RSP_OK,LSA_FALSE);
            }
            else
            {
                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-No free USER-RCTRL (pHDB: 0x%X)",pHDB);
                Ready = LSA_TRUE;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-No requests pending (pHDB: 0x%X)",pHDB);
            Ready = LSA_TRUE;
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_UserCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestTimer                            +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     NARE_OPC_TIMER                  +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response    Response                        +*/
/*+     NARE_RQB_ERROR_TYPE     Error       Addition errorcodes from LowerL.+*/
/*+     NARE_RQB_ARGS_TYPE      args.Timer: NARE_RQB_TIMER_TYPE             +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : NARE-Timer ID                                            +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from NARE within nare_timeout().        +*/
/*+               Handles Timer-events within NARE main context.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestTimer(
    NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestTimer(pRQB: 0x%X)",pRQB);
    switch (pRQB->args.Timer.UserID )
    {
        case NARE_TIMER_USER_ID_ARP:
            /* NARE_DBG_ACT("NARERequestTimer RQB received.\r\n",0,0,0); */
            NARETimerRQBFree(pRQB->args.Timer.TimerID);
            NARE_EDDTimerEventARP();
            break;

        default:
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"NARERequestTimer. Invalid UserID (0x%X)",pRQB->args.Timer.UserID);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;

    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestTimer()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestResolveConfigCancel              +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_RESOLVE_CONFIG_CANCEL +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.ResConfCancel   .... LVE_CONFIG_CANC.. +*/
/*+                                                                         +*/
/*+     args.ResConfCancel                                                  +*/
/*+                                                                         +*/
/*+     RequestID         : Request ID spezifing Request to cancel          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles ResolveConfigCancel Request from user.            +*/
/*+                                                                         +*/
/*+               We search for all requests with the spezified RequestID.  +*/
/*+               We cancel all of these requests. If the requests already  +*/
/*+               has a cancel pending, we skip it, because it already will +*/
/*+               be canceled.                                              +*/
/*+                                                                         +*/
/*+               * Checks User-Request queue.                              +*/
/*+               * Checks DCP/EDD/SOCK-Pending queues.                     +*/
/*+               * Checks if cancel have to be delayed because of action   +*/
/*+                 currently in progress (e.g. lowerlayer request running) +*/
/*+                                                                         +*/
/*+               If the RCTRL-block is in use and running we set a flag    +*/
/*+               to signal a cancel is pending. the cancel is done later.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestResolveConfigCancel(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32                i;
    NARE_RSP                  Response;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp2;
    LSA_UINT32                RequestID;
    NARE_USER_RCTRL_PTR_TYPE  pRCtrl;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestResolveConfigCancel(pRQB: 0x%X)",pRQB);
    Response = NARE_RSP_OK;
    RequestID = pRQB->args.ResConfCancel.RequestID;
    /*---------------------------------------------------------------------------*/
    /* we use the RequestID within RQB as counter of pending request to be       */
    /* canceled. will be restored to RequestID on confirmation.                  */
    /*---------------------------------------------------------------------------*/
    pRQB->args.ResConfCancel.RequestID = 0; /* 0 = no queued cancel */
    /* --------------------------------------------------------------------------*/
    /* First we check if the request is pending with USER-handle mgm             */
    /* This request can canceled at once.                                        */
    /* --------------------------------------------------------------------------*/
    pRQBHelp  = pHDB->UserReq.pBottom;
    while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBHelp, LSA_NULL) ) /* search all */
    {
        if (( NARE_RQB_GET_OPCODE(pRQBHelp) == NARE_OPC_RESOLVE_CONFIG ) &&
            ( pRQBHelp->args.ResConf.RequestID == RequestID ))
        {
            NARE_RQB_REM_BLOCK(pHDB->UserReq.pBottom,
                               pHDB->UserReq.pTop,
                               pRQBHelp,
                               pRQBHelp2);
            NARE_ASSERT_NULL_PTR(pRQBHelp2); /* Help2 has to be the same value as Help */
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Canceling User-Request1 (RequestID: 0x%X)",pRQBHelp->args.ResConf.RequestID);
            pHDB->UserReqPendCnt--;
            pRQBHelp = NARE_RQB_GET_NEXT_RQB_PTR(pRQBHelp);
            NAREUserRequestFinish(pHDB,pRQBHelp2,NARE_RSP_OK_CANCEL);
        }
        else
        {
            pRQBHelp = NARE_RQB_GET_NEXT_RQB_PTR(pRQBHelp);
        }
    }
    /* --------------------------------------------------------------------------*/
    /* Now search within Usr RCTRL-Block for Request                             */
    /* --------------------------------------------------------------------------*/

    i = 0;
    pRCtrl =  &(*pHDB->UserRCtrl.pTable)[0];

    while ( i < pHDB->UserRCtrl.Cnt )
    {
        if (( pRCtrl->BState == NARE_RCTRL_BSTATE_USED )    &&
            ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl->pRQB,LSA_NULL)) &&    /* should be!*/
            ( NARE_RQB_GET_OPCODE(pRCtrl->pRQB) == NARE_OPC_RESOLVE_CONFIG ) &&
            ( pRCtrl->pRQB->args.ResConf.RequestID == RequestID) &&
            ( ! pRCtrl->DoCancel))  /* not already a Cancel running ? */
        {

            /* -------------------------------------------------------------*/
            /* if found check if the request is currently pending. if so we */
            /* can cancel it                                                */
            /* -------------------------------------------------------------*/

            switch (pRCtrl->QueueType)
            {
                    /* if the request is pending we can cancel it here */

                case NARE_RCTRL_QTYPE_DCP_PENDING:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Canceling User-Request2 (RequestID: 0x%X)",pRCtrl->pRQB->args.ResConf.RequestID);
                    NARE_DCPRemoveRCtrl(pHDB->pHDBDcp,pRCtrl); /* remove from DCP-queue */
                    NARE_UserReady(pHDB,pRCtrl,NARE_RSP_OK_CANCEL);
                    break;

                case NARE_RCTRL_QTYPE_EDD_PENDING:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Canceling User-Request3 (RequestID: 0x%X)",pRCtrl->pRQB->args.ResConf.RequestID);
                    NARE_EDDRemoveRCtrl(pHDB->pHDBEdd,pRCtrl); /* remove from EDD-queue */
                    NARE_UserReady(pHDB,pRCtrl,NARE_RSP_OK_CANCEL);
                    break;

#ifndef NARE_CFG_NO_SOCK
                case NARE_RCTRL_QTYPE_SOCK_PENDING:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Canceling User-Request4 (RequestID: 0x%X)",pRCtrl->pRQB->args.ResConf.RequestID);
                    NARE_SOCKRemoveRCtrl(pHDB->pHDBSock,pRCtrl); /* remove from SOCK-queue */
                    NARE_UserReady(pHDB,pRCtrl,NARE_RSP_OK_CANCEL);
                    break;
#endif
                    /* if the request is running we set a flag for     */
                    /* later cancel. This flag will be checked in      */
                    /* statemachine                                    */

                case NARE_RCTRL_QTYPE_DCP_RUNNING:
                case NARE_RCTRL_QTYPE_EDD_RUNNING:
#ifndef NARE_CFG_NO_SOCK
                case NARE_RCTRL_QTYPE_SOCK_RUNNING:
#endif
                    /* Note that we may cancel ARP-Sniffing here.  */
                    /* This is currently not implemented.          */
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Cancel not possible yet. Queuing it (RequestID: 0x%X)",pRCtrl->pRQB->args.ResConf.RequestID);
                    pRCtrl->pCancelRQB = pRQB;
                    pRCtrl->DoCancel   = LSA_TRUE;
                    pRQB->args.ResConfCancel.RequestID++;  /* counter for pending requests */
                    break;

                case NARE_RCTRL_QTYPE_NONE:
                default:
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;
            } /* switch */
        }
        i++;
        pRCtrl++;
    } /* while ()  */
    /* --------------------------------------------------------------------------*/
    /* Now search within EDD-RCTRL-Blocks. Because Sniffing maybe running.       */
    /* --------------------------------------------------------------------------*/
    NARE_EDDAbortSniffing(pHDB->pHDBEdd);
    /*----------------------------------------------------------------------------*/
    /* If no Cancel is queued we can finish the Cancel-Request here.              */
    /*----------------------------------------------------------------------------*/
    if ( ! pRQB->args.ResConfCancel.RequestID ) /* if no queued cancel. */
    {
        pRQB->args.ResConfCancel.RequestID = RequestID; /* restore RequestID */
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Finishing Cancel-request (RequestID: 0x%X)",pRQB->args.ResConfCancel.RequestID);
        NAREUserRequestFinish(pHDB,pRQB,Response);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestResolveConfigCancel()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestResolveConfigNotify              +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_RESOLVE_CONFIG_NOTIFY +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.ResConfCancel   .... LVE_CONFIG_NOTIFY +*/
/*+                                                                         +*/
/*+     args.ResConfNotify                                                  +*/
/*+                                                                         +*/
/*+     NotifyID          : NotifID from corresponding RESOLVE_CONFIG req.  +*/
/*+     Res               : Resolve-parameters                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles ResolveConfigNotify Request from user.            +*/
/*+                                                                         +*/
/*+               The request is queued within NARE and used for indication +*/
/*+               if a identify response comes in.                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestResolveConfigNotify(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestResolveConfigNotify(pRQB: 0x%X)",pRQB);
    /* queue the request */
    NARE_RQB_PUT_BLOCK_TOP(pHDB->NotifyReq.pBottom,
                           pHDB->NotifyReq.pTop,
                           pRQB);
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestResolveConfigNotify()");
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestResolveConfig                    +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_RESOLVE_CONFIG        +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.ResConf NARE_RQB_RESOLVE_CONFIG_TYPE   +*/
/*+                                                                         +*/
/*+     args.ResConf:                                                       +*/
/*+                                                                         +*/
/*+     StationName       : StationName to use search for                   +*/
/*+     StationNameLen    : Length of StationName in bytes                  +*/
/*+     RequestID         : Request ID. used for cancel.                    +*/
/*+     Mode;             : Mode of operation                               +*/
/*+     Cfg               : Config-paramters                                +*/
/*+     Res               : Resolve-parameters                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles ResolveConfig-Request from user                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestResolveConfig(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32                i;
    NARE_RSP                  Response;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestResolveConfig(pRQB: 0x%X)",pRQB);

    pRQB->args.ResConf.AliasUsed  = 0; /* Initialize return value */
    pRQB->args.ResConf.StationNameSetEqual = LSA_FALSE; /* Initialize return value */

    Response = NARE_RSP_OK;
    /* check parameters of request */
    if (pRQB->args.ResConf.ExtendedMode > NARE_EXT_MODE_CHECK_DEVICE_ID)    /* invalid extended mode */
    {
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"ResConf-Request. invalid extended mode 0x%x",pRQB->args.ResConf.ExtendedMode);
        Response = NARE_RSP_ERR_PARAM;
    }
    else
    {
        switch ( pRQB->args.ResConf.Mode )
        {
        case NARE_MODE_CONFIG_IP_DNS:
        case NARE_MODE_RESOLVE_DNS:
#ifdef NARE_CFG_NO_SOCK
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"ResConf-Request. Mode not supported (0x%X).",pRQB->args.ResConf.Mode);
            Response = NARE_RSP_ERR_PARAM;
            break;
#endif

        case NARE_MODE_CONFIG_IP_FIXED:
        case NARE_MODE_RESOLVE_DCP:
        case NARE_MODE_CONFIG_NOS_FIXED:
            /* we need a StationName for this Mode ! */
            if ( ( pRQB->args.ResConf.StationNameLen == 0 )
                || ( LSA_HOST_PTR_ARE_EQUAL(pRQB->args.ResConf.pStationName, LSA_NULL)))
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"ResConf-Request. No StationName in RQB");
                Response = NARE_RSP_ERR_PARAM;
            }
            else
            {
#ifndef NARE_CFG_DONT_VALIDATE_IP_SUITE
                /* Note: DNS config will be checked later */
                /* Note: NARE_MODE_CONFIG_NOS_FIXED has no IP-Address */
                if ( ( pRQB->args.ResConf.Mode == NARE_MODE_CONFIG_IP_FIXED ) &&
                    ( ! NAREValidateIPSuite(pRQB->args.ResConf.Cfg.IPAddr,
                    pRQB->args.ResConf.Cfg.SubMask,
                    pRQB->args.ResConf.Cfg.DefRouter)))
                {
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR,"ResConf-Request. Invalid IP-Suite parameters!");
                    Response = NARE_RSP_ERR_INVALID_IP_PARAMS;
                }
#endif
            }
            if ( Response == NARE_RSP_OK )
            {
#ifndef NARE_CFG_NO_ALIAS_SUPPORT
                /* validate ALIAS-name parameters (not use with NARE_MODE_RESOLVE_DNS) */
                if (( pRQB->args.ResConf.Mode != NARE_MODE_RESOLVE_DNS ) &&
                    ( pRQB->args.ResConf.AliasCnt ))
                {
                    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->args.ResConf.pStationNameAlias, LSA_NULL))
                    {
                        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"ResConf-Request. Alias mismatch in RQB");
                        Response = NARE_RSP_ERR_PARAM;
                    }
                    else
                    {
                        LSA_UINT32 i;
                        for ( i=0; i< pRQB->args.ResConf.AliasCnt; i++)
                        {
                            if (( pRQB->args.ResConf.pStationNameAlias[i].NameLen == 0 )
                                || ( LSA_HOST_PTR_ARE_EQUAL(pRQB->args.ResConf.pStationNameAlias[i].pName,LSA_NULL)))
                            {
                                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"ResConf-Request. Alias mismatch in RQB");
                                Response = NARE_RSP_ERR_PARAM;
                            }
                        }
                    }
                }
#endif
            }
            break;

        default:
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"ResConf-Request. Wrong Mode (0x%Xh)",pRQB->args.ResConf.Mode);
            Response = NARE_RSP_ERR_PARAM;
            break;
        }
    }
    if ( Response == NARE_RSP_OK )
    {
        /* init out-params */
        for ( i=0; i<NARE_IP_ADDR_SIZE; i++)
        {
            pRQB->args.ResConf.Res.IPAddr[i]     = 0;
            pRQB->args.ResConf.Res.DefRouter[i]  = 0;
            pRQB->args.ResConf.Res.SubMask[i]    = 0;
        }
        for ( i=0; i<NARE_MAC_ADDR_SIZE; i++)
        {
            pRQB->args.ResConf.Res.MACAddr[i] = 0;
        }
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"ResConf-Request. Queuing block (pRQB: 0x%X)",pRQB);

        /* queue the request */
        NARE_RQB_PUT_BLOCK_TOP(pHDB->UserReq.pBottom,
                               pHDB->UserReq.pTop,
                               pRQB);
        pHDB->UserReqPendCnt++;
        /* check for pending requests and try to execute it */
        NARE_UserCheckPending(pHDB);
    }
    else
    {
        NAREUserRequestFinish(pHDB,pRQB,Response);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestResolveConfig()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestTestIP                           +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_TEST_IP               +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.TestIP                                 +*/
/*+                                                                         +*/
/*+     args.TestIP                                                         +*/
/*+                                                                         +*/
/*+     IPAddr            : IP-Address to test                              +*/
/*+     MACAddr           : Found MAC if IP present                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles TestIP- Request from user.                        +*/
/*+                                                                         +*/
/*+               Responses:  NARE_RSP_OK                                   +*/
/*+                           NARE_RSP_ERR_NO_OWN_IP                        +*/
/*+                           NARE_RSP_ERR_IP_IN_USE                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestTestIP(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32                i;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestTestIP(pRQB: 0x%X)",pRQB);
    /* init out-params */
    for ( i=0; i<NARE_MAC_ADDR_SIZE; i++)
    {
        pRQB->args.TestIP.MACAddr[i] = 0;
    }
    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"TestIP-Request. Queuing block (pRQB: 0x%X).",pRQB);
    /* queue the request */
    NARE_RQB_PUT_BLOCK_TOP(pHDB->UserReq.pBottom,
                           pHDB->UserReq.pTop,
                           pRQB);
    pHDB->UserReqPendCnt++;
    /* check for pending requests and try to execute it */
    NARE_UserCheckPending(pHDB);
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestTestIP()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestTestIPCancel                     +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_TEST_IP_CANCEL        +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.TestIP                                 +*/
/*+                                                                         +*/
/*+     args.TestIPCancel                                                   +*/
/*+                                                                         +*/
/*+     RequestID         : Request ID for NARE internal use (no in/out)    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles TestIP- Cancel Request from user.                 +*/
/*+                                                                         +*/
/*+               We cancel all current running Test IP requests no already +*/
/*+               cancelling form a previous cancel request.                +*/
/*+                                                                         +*/
/*+               * Checks User-Request queue.                              +*/
/*+               * Checks EDD-Pending queues.                              +*/
/*+               * Checks if cancel have to be delayed because of action   +*/
/*+                 currently in progress (e.g. lowerlayer request running) +*/
/*+                                                                         +*/
/*+               If the RCTRL-block is in use and running we set a flag    +*/
/*+               to signal a cancel is pending. the cancel is done later.  +*/
/*+                                                                         +*/
/*+               Responses:  NARE_RSP_OK                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestTestIPCancel(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32                i;
    NARE_RSP                  Response;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp2;
    LSA_UINT32                RequestID;
    NARE_USER_RCTRL_PTR_TYPE  pRCtrl;


    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestTestIPCancel(pRQB: 0x%X)",pRQB);
    Response = NARE_RSP_OK;
    RequestID = pRQB->args.TestIPCancel.RequestID;
    /*---------------------------------------------------------------------------*/
    /* we use the RequestID within RQB as counter of pending request to be       */
    /* canceled. will be restored to RequestID on confirmation.                  */
    /*---------------------------------------------------------------------------*/
    pRQB->args.TestIPCancel.RequestID = 0; /* 0 = no queued cancel */
    /* --------------------------------------------------------------------------*/
    /* First we check if the request is pending with USER-handle mgm             */
    /* This request can canceled at once.                                        */
    /* --------------------------------------------------------------------------*/
    pRQBHelp  = pHDB->UserReq.pBottom;
    while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBHelp, LSA_NULL) ) /* search all */
    {
        if (( NARE_RQB_GET_OPCODE(pRQBHelp) == NARE_OPC_TEST_IP ) &&
            ( pRQBHelp->args.TestIP.RequestID == RequestID ))
        {
            NARE_RQB_REM_BLOCK(pHDB->UserReq.pBottom,
                               pHDB->UserReq.pTop,
                               pRQBHelp,
                               pRQBHelp2);

            NARE_ASSERT_NULL_PTR(pRQBHelp2); /* Help2 has to be the same value as Help */
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Canceling TestIP User-Request1");
            pHDB->UserReqPendCnt--;
            pRQBHelp = NARE_RQB_GET_NEXT_RQB_PTR(pRQBHelp);
            NAREUserRequestFinish(pHDB,pRQBHelp2,NARE_RSP_OK_CANCEL);
        }
        else
        {
            pRQBHelp = NARE_RQB_GET_NEXT_RQB_PTR(pRQBHelp);
        }
    }
    /* --------------------------------------------------------------------------*/
    /* Now search within Usr RCTRL-Block for Request                             */
    /* --------------------------------------------------------------------------*/
    i = 0;
    pRCtrl =  &(*pHDB->UserRCtrl.pTable)[0];
    while ( i < pHDB->UserRCtrl.Cnt )
    {
        if (( pRCtrl->BState == NARE_RCTRL_BSTATE_USED )    &&
            ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl->pRQB,LSA_NULL)) &&    /* should be!*/
            ( NARE_RQB_GET_OPCODE(pRCtrl->pRQB) == NARE_OPC_TEST_IP ) &&
            ( pRCtrl->pRQB->args.TestIP.RequestID == RequestID) &&
            ( ! pRCtrl->DoCancel))  /* not already a Cancel running ? */
        {
            /* -------------------------------------------------------------*/
            /* if found check if the request is currently pending. if so we */
            /* can cancel it                                                */
            /* -------------------------------------------------------------*/
            switch (pRCtrl->QueueType)
            {
                    /* if the request is pending we can cancel it here */
                case NARE_RCTRL_QTYPE_EDD_PENDING:
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Canceling TestIP User-Request2");
                    NARE_EDDRemoveRCtrl(pHDB->pHDBEdd,pRCtrl); /* remove from EDD-queue */
                    NARE_UserReady(pHDB,pRCtrl,NARE_RSP_OK_CANCEL);
                    break;

                case NARE_RCTRL_QTYPE_EDD_RUNNING:

                    /* check if the request is currently in sniffing and can be */
                    /* finished (can be finished if already ready with sending  */
                    /* arps ). can not be finised if currently sending arps     */
                    /* and waiting for edd for send done.                       */

                    if ( ! NARE_EDDFindAndFinishSniff(pHDB->pHDBEdd,pRCtrl))
                    {
                        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"TestIP Cancel not possible yet. Queuing it");
                        /* if the request is running we set a flag for     */
                        /* later cancel. This flag will be checked in      */
                        /* statemachine                                    */
                        pRCtrl->pCancelRQB = pRQB;
                        pRCtrl->DoCancel   = LSA_TRUE;
                        pRQB->args.TestIPCancel.RequestID++;  /* counter for pending requests */
                    }
                    else
                    {
                        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"TestIP canceled while sniffing");
                    }
                    break;

                case NARE_RCTRL_QTYPE_DCP_PENDING:
                case NARE_RCTRL_QTYPE_SOCK_PENDING:
                case NARE_RCTRL_QTYPE_DCP_RUNNING:
                case NARE_RCTRL_QTYPE_SOCK_RUNNING:
                case NARE_RCTRL_QTYPE_NONE:
                default:
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;

            } /* switch */
        }
        i++;
        pRCtrl++;
    } /* while ()  */
    /* --------------------------------------------------------------------------*/
    /* Now search within EDD-RCTRL-Blocks and flag all still present blocks      */
    /* because they currently used for sending ARPs. This should not be          */
    /* neccessary because we will cancel them after send is finished anyway.     */
    /* --------------------------------------------------------------------------*/
    NARE_EDDAbortSniffing(pHDB->pHDBEdd);
    /*----------------------------------------------------------------------------*/
    /* If no Cancel is queued we can finish the Cancel-Request here.              */
    /*----------------------------------------------------------------------------*/
    if ( ! pRQB->args.TestIPCancel.RequestID ) /* if no queued cancel. */
    {
        pRQB->args.TestIPCancel.RequestID = RequestID; /* restore RequestID */
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Finishing TestIP Cancel-request");
        NAREUserRequestFinish(pHDB,pRQB,Response);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestTestIPCancel()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestResolveIP                        +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_RESOLVE_IP            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.ResolveIP                              +*/
/*+                                                                         +*/
/*+     args.ResolveIP                                                      +*/
/*+                                                                         +*/
/*+     IPAddr            : IP-Address to test                              +*/
/*+     MACAddr           : Resolved MAC if IP present                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles ResolveIP-Request from user.                      +*/
/*+                                                                         +*/
/*+               Responses:  NARE_RSP_OK                                   +*/
/*+                           NARE_RSP_ERR_NO_OWN_IP                        +*/
/*+                           NARE_RSP_ERR_MULTIPLE_IP_USE                  +*/
/*+                           NARE_RSP_ERR_IP_RESOLVE_NO_ANSWER             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestResolveIP(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32              i;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestResolveIP(pRQB: 0x%X)",pRQB);
    /* init out-params */
    for ( i=0; i<NARE_MAC_ADDR_SIZE; i++)
    {
        pRQB->args.ResolveIP.MACAddr[i] = 0;
    }
    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"ResolveIP-Request. Queuing block (pRQB: 0x%X).",pRQB);
    /* queue the request */
    NARE_RQB_PUT_BLOCK_TOP(pHDB->UserReq.pBottom,
                           pHDB->UserReq.pTop,
                           pRQB);
    pHDB->UserReqPendCnt++;
    /* check for pending requests and try to execute it */
    NARE_UserCheckPending(pHDB);
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestResolveIP()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestHello                            +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_HELLO                 +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.Hello                                  +*/
/*+                                                                         +*/
/*+     args.Hello                                                          +*/
/*+                                                                         +*/
/*+     SendInterval            : send interval in 10ms ticks               +*/
/*+     SendCnt                 : number of sends                           +*/
/*+     pTLV                    : pointer to HELLO TLV chain                +*/
/*+     TLVLen                  : length of TLV chain                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles HELLO request from user.                          +*/
/*+                                                                         +*/
/*+               Note: Only one user per DCP channel can use HELLO requests+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestHello(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_RSP                  Status;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARERequestHello(pRQB: 0x%X)",pRQB);
    Status = NARE_RSP_OK;
    /* Parametercheck */
    if (( LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Hello.pTLV,LSA_NULL )) ||
        ( pRQB->args.Hello.SendCnt == 0 ) ||
        ( pRQB->args.Hello.OptCnt  == 0 ))
    {
        Status = NARE_RSP_ERR_PARAM;
    }
    else
    {
        /* Do a DCP Hello (note: only if not already a hello is running. */
        /* checked inside!)                                              */
        Status = NARE_DCPHello(pHDB->pHDBDcp,
                               pHDB,
                               pRQB->args.Hello.SendInterval,
                               pRQB->args.Hello.SendCnt,
                               pRQB->args.Hello.Delay,
                               pRQB->args.Hello.pTLV,
                               pRQB->args.Hello.OptCnt);
        /* If OK the Request to DCP is underway */
        if ( Status == NARE_RSP_OK )
        {
            pHDB->Hello.pRQB   = pRQB;
            pHDB->Hello.Status = NARE_USR_HELLO_IN_PROGRESS;
        }
    }
    if ( Status != NARE_RSP_OK )
    {
        NAREUserRequestFinish(pHDB,pRQB,Status);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestHello()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestHelloStop                        +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_HELLO_STOP            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.HelloStop                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles HELLO_STOP request from user.                     +*/
/*+                                                                         +*/
/*+               Note: Only one user per DCP channel can use HELLO requests+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARERequestHelloStop(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_RSP                  Status;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestHelloStop(pRQB: 0x%X)",pRQB);
    Status = NARE_DCPHelloStop(pHDB->pHDBDcp,pHDB);
    /* If OK the Request to DCP is underway */
    if ( Status == NARE_RSP_OK )
    {
        pHDB->Hello.pRQB   = pRQB;
        pHDB->Hello.Status = NARE_USR_HELLO_IN_PROGRESS;
    }
    else
    {
        NAREUserRequestFinish(pHDB,pRQB,Status);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestHelloStop()");
}

/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_open_channel                           +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       NARE_OPC_OPEN_CHANNEL         +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     NARE_RSP                Response      -                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error         -                             +*/
/*+     NARE_RQB_ARGS_TYPE      Args.Channel: NARE_RQB_OPEN_CHANNEL_TYPE    +*/
/*+                                                                         +*/
/*+     Args.Channel:                                                       +*/
/*+                                                                         +*/
/*+     LSA_HANDLE_TYPE         nandle:       channel-handle of NARE (ret)  +*/
/*+     LSA_HANDLE_TYPE         handle_upper  channel-handle of user        +*/
/*+     LSA_SYS_PATH_TYPE       sys_path      system-path of channel        +*/
/*+     LSA_VOID                LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR,        +*/
/*+                                         nare_request_upper_done_ptr)    +*/
/*+                                         (NARE_UPPER_RQB_PTR_TYPE pRQB)  +*/
/*+                                             callback-function           +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user. This  +*/
/*+                                         is HandleUpper from Args.Channel+*/
/*+     NARE_RSP                Response    NARE_RSP_OK                     +*/
/*+                                         NARE_RSP_ERR_RESOURCE           +*/
/*+                                         NARE_RSP_ERR_SYS_PATH           +*/
/*+                                         NARE_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a channel                                 +*/
/*+                                                                         +*/
/*+               Within this request the output - macro NARE_GET_PATH_INFO +*/
/*+               is used to get further channel-descriptons and parameters +*/
/*+                                                                         +*/
/*+               After a successful open a channel-handle is returned      +*/
/*+               in Args.Channel.Handle for use with further requests.     +*/
/*+                                                                         +*/
/*+               HandleUpper:                                              +*/
/*+                                                                         +*/
/*+               This handle will be stored within NARE and return as      +*/
/*+               handle on request-confirmation (in RQB-header).           +*/
/*+                                                                         +*/
/*+               cbf:                                                      +*/
/*+                                                                         +*/
/*+               The request is always finished by calling the spezified   +*/
/*+               callback-function. This callback-funktion will also be    +*/
/*+               used for all other other future requests for this channel.+*/
/*+                                                                         +*/
/*+               SysPath:                                                  +*/
/*+                                                                         +*/
/*+               The SysPath variable will not be used inside but given to +*/
/*+               systemadaption via NARE_GET_PATH_INFO.                    +*/
/*+                                                                         +*/
/*+               This function is most likely done asynchroneous because   +*/
/*+               of calling lower layer open channel (DCP or SOCK)         +*/
/*+               depending on channel type spezified in CDP (detail-ptr)   +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro NARE_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               NARE_RSP_ERR_OPCODE                                       +*/
/*+               NARE_RSP_ERR_PARAM                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_UPPER_IN_FCT_ATTR  nare_open_channel(NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    NARE_RSP                  Status;
    LSA_UINT32                Finish;
    LSA_SYS_PTR_TYPE          pSys;
    NARE_DETAIL_PTR_TYPE      pDetail;
    LSA_UINT16                PathStat;
    NARE_HDB_USER_PTR_TYPE    pHDBUser;
    NARE_HDB_DCP_PTR_TYPE     pHDBDcp;
    NARE_HDB_EDD_PTR_TYPE     pHDBEdd;
#ifndef NARE_CFG_NO_SOCK
    NARE_HDB_SOCK_PTR_TYPE    pHDBSock;
#endif
    LSA_BOOL                  PathOpen;

    NARE_ENTER();
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_open_channel(pRQB: 0x%X)",pRQB);
    Status   = NARE_RSP_OK;
    Finish   = LSA_FALSE;
    PathOpen = LSA_FALSE;
    {
        LSA_SYS_PTR_TYPE tmp = {0};
        pSys  = tmp; /* pSys may not be a pointer ! */
    }
    pDetail  = LSA_NULL;
    NARE_ASSERT_NULL_PTR(pRQB);
    /*----------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                   */
    /*----------------------------------------------------------------------------*/
    NARE_RQB_SET_HANDLE(pRQB,pRQB->args.channel.handle_upper); /* set upper handle */
    /*----------------------------------------------------------------------------*/
    /* Checks some parameters                                                     */
    /*----------------------------------------------------------------------------*/
    if ( NARE_RQB_GET_OPCODE(pRQB) != NARE_OPC_OPEN_CHANNEL )
    {  
        NARE_RQB_SET_RESPONSE(pRQB, NARE_RSP_ERR_OPCODE);
        Status = NARE_RSP_ERR_OPCODE;
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,">>>: nare_open_channel(): Invalid RQB-Opcode (0x%X)",NARE_RQB_GET_OPCODE(pRQB));
        NARE_RQB_ERROR(pRQB);
    }
    else
    {
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_OPEN_CHANNEL (Handle: 0x%X)",NARE_RQB_GET_HANDLE(pRQB));
        if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->args.channel.nare_request_upper_done_ptr, LSA_NULL) )
        { /* AP00999037: throw fatal if NULL callback is given */
            NARE_FATAL(NARE_FATAL_ERR_NULL_PTR);
        }
    }
    /*----------------------------------------------------------------------------*/
    /* if all ok handle request                                                   */
    /*----------------------------------------------------------------------------*/
    if (Status == NARE_RSP_OK)
    {
        Finish = LSA_TRUE;
        /*------------------------------------------------------------------------*/
        /* Call LSA Output-macro..                                                */
        /*------------------------------------------------------------------------*/
        NARE_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling NARE_GET_PATH_INFO.");
        NARE_GET_PATH_INFO(&PathStat, &pSys, &pDetail, pRQB->args.channel.sys_path);
        if ( PathStat != LSA_RET_OK)
        {
            NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"<--: NARE_GET_PATH_INFO failed (Rsp: 0x%X).",PathStat);
            NARE_FATAL(NARE_FATAL_ERR_PATHINFO);
        }
        else
        {
            NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"<--: NARE_GET_PATH_INFO success (Detail-Ptr: 0x%X).",pDetail);
            if (LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL))
            {
                Status = NARE_RSP_ERR_PARAM;
                NARE_FATAL(NARE_FATAL_ERR_PATHINFO);
            }
            else 
            {
                PathOpen = LSA_TRUE;
            }
        }
        /*------------------------------------------------------------------------*/
        /* Get Infos from Detail-Ptr (CDB) and start handling..                   */
        /*------------------------------------------------------------------------*/
        if ( Status == NARE_RSP_OK )
        {
            switch (pDetail->PathType)
            {       /* -----------------------------------------------------------*/
                    /* User-Channel.                                              */
                    /* -----------------------------------------------------------*/
                case NARE_PATH_TYPE_USER:
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening USER-Channel");
                    /* check the detail ptr for correct parameter AP01513629*/
                    if (pDetail->Params.User.MaxRUser == 0)
                    {
                        Status = NARE_RSP_ERR_PARAM;
                        break;
                    }
                    /* Check if we have DCP and SOCK -Channel for NIC-ID. */
                    /* SOCK-Channel is NICId independend                  */
                    pHDBDcp  = NARE_DCPGetHandleByNIC(pDetail->Params.User.NICId);
                    pHDBEdd  = NARE_EDDGetHandleByNIC(pDetail->Params.User.NICId);
#ifndef NARE_CFG_NO_SOCK
                    pHDBSock = NARE_SOCKGetHandleByNIC(pDetail->Params.User.NICId);
#endif
#ifndef NARE_CFG_NO_SOCK
                    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL) ||
                         LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) ||
                         LSA_HOST_PTR_ARE_EQUAL(pHDBSock, LSA_NULL))
#else
                    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL) ||
                         LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL))
#endif
                    {
                        Status = NARE_RSP_ERR_SEQUENCE;
                        /* Error on getting handle. Abort. */
                        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE USER-channel failed. No DCP/EDD or SOCK-Channel present (NIC-ID: 0x%X)",pDetail->Params.User.NICId);
                    }
                    else
                    {   /* Get a free, initialized HDB */
                        Status = NARE_UserGetFreeHDB(pDetail->Params.User.MaxRUser,&pHDBUser);
                        if ( Status == NARE_RSP_OK )
                        {
                            NARE_ASSERT_NULL_PTR(pHDBUser);
                            pHDBUser->Params.pSys          = pSys;
                            pHDBUser->Params.pDetail       = pDetail;
                            pHDBUser->Params.Cbf           = pRQB->args.channel.nare_request_upper_done_ptr;
                            pHDBUser->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBUser->Params.HandleUpper   = pRQB->args.channel.handle_upper;
                            /* set our handle in RQB */
                            pRQB->args.channel.handle      = pHDBUser->ThisHandle;
                            pHDBUser->pHDBDcp              = pHDBDcp;
                            pHDBDcp->UserCnt++;
                            pHDBUser->pHDBEdd              = pHDBEdd;
                            pHDBEdd->UserCnt++;
#ifndef NARE_CFG_NO_SOCK
                            pHDBUser->pHDBSock             = pHDBSock;
                            pHDBSock->UserCnt++;
#endif
                            pHDBUser->Hello.Status         = NARE_USR_HELLO_IDLE;
                            pHDBUser->Hello.pRQB           = LSA_NULL;
                            pHDBUser->TraceIdx             = pHDBEdd->TraceIdx;
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open NARE USER-channel failed. Cannot get USER handle! (Rsp: 0x%X).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* DCP -Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case NARE_PATH_TYPE_DCP:
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening DCP- SystemChannel");
                    /* check the detail ptr for correct parameter AP01513629*/
                    if (pDetail->Params.Dcp.MaxRDCP == 0)
                    {
                        Status = NARE_RSP_ERR_PARAM;
                        break;
                    }
                    /* Check if NIC-ID already used by a DCP-Channel. If so abort */
                    if ( NARE_DCPCheckNICUsed(pDetail->Params.Dcp.NICId))
                    {
                        Status = NARE_RSP_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE DCP-channel failed. NIC-ID already used (ID: 0x%X)",pDetail->Params.Dcp.NICId);
                    }
                    else
                    {   /* Get a free, initialized HDB */
                        Status = NARE_DCPGetFreeHDB(pDetail->Params.Dcp.MaxRDCP,&pHDBDcp,pSys);
                        if ( Status == NARE_RSP_OK )
                        {
                            NARE_ASSERT_NULL_PTR(pHDBDcp);
                            pHDBDcp->Params.pSys          = pSys;
                            pHDBDcp->Params.pDetail       = pDetail;
                            pHDBDcp->Params.Cbf           = pRQB->args.channel.nare_request_upper_done_ptr;
                            pHDBDcp->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBDcp->Params.HandleUpper   = pRQB->args.channel.handle_upper;
                            pHDBDcp->NoOfRCtrlCnt         = pDetail->Params.Dcp.MaxRDCP;
                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBDcp->ThisHandle;
                            /* Do a DCP- open channel request */
                            Status = NARE_DCPOpenChannel(pHDBDcp->ThisHandle,pHDBDcp->Params.SysPath,pHDBDcp->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {   /* Request is underway now.. */
                                pHDBDcp->State  = NARE_HANDLE_STATE_OPEN_DCP_RUNNING;
                                pHDBDcp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish          = LSA_FALSE;
                            }
                            else
                            {
                                NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE-DCP channel failed. (Rsp: %Xh)",Status);
                                NARE_DCPReleaseHDB(pHDBDcp);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open NARE-DCP channel failed. Cannot get DCP handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* EDD -Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case NARE_PATH_TYPE_EDD:
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening EDD- SystemChannel");
                    if (pDetail->Params.Edd.MaxREDD == 0)
                    {
                        Status = NARE_RSP_ERR_PARAM;
                        break;
                    }
                    /* Check if NIC-ID already used by a EDD-Channel. If so abort */
                    if ( NARE_EDDCheckNICUsed(pDetail->Params.Edd.NICId))
                    {
                        Status = NARE_RSP_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE EDD-channel failed. NIC-ID already used (ID: %Xh)",pDetail->Params.Edd.NICId);
                    }
                    else
                    {   /* Get a free, initialized HDB */
                        Status = NARE_EDDGetFreeHDB(pDetail->Params.Edd.MaxREDD, &pHDBEdd,pSys);
                        if ( Status == NARE_RSP_OK )
                        {
                            NARE_ASSERT_NULL_PTR(pHDBEdd);
                            pHDBEdd->Params.pSys          = pSys;
                            pHDBEdd->Params.pDetail       = pDetail;
                            pHDBEdd->Params.Cbf           = pRQB->args.channel.nare_request_upper_done_ptr;
                            pHDBEdd->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBEdd->Params.HandleUpper   = pRQB->args.channel.handle_upper;
                            pHDBEdd->SizeOfEDDRCtrlQue    = pDetail->Params.Edd.MaxREDD;
                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBEdd->ThisHandle;
                            /* Do a EDD- open channel request */
                            Status = NARE_EDDOpenChannel(pHDBEdd->ThisHandle,pHDBEdd->Params.SysPath,pHDBEdd->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {
                                /* Request is underway now.. */
                                pHDBEdd->State  = NARE_HANDLE_STATE_OPEN_EDD_RUNNING;
                                pHDBEdd->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish          = LSA_FALSE;
                            }
                            else
                            {
                                NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE-EDD channel failed. (Rsp: %Xh)",Status);
                                NARE_EDDReleaseHDB(pHDBEdd);
                            }
                        }
                        else
                        {   /* Error on getting handle. Abort. */
                            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open NARE-EDD channel failed. Cannot get EDD handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* SOCK-Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case NARE_PATH_TYPE_SOCK:
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening SOCK- SystemChannel");
#ifndef NARE_CFG_NO_SOCK
                    if (pDetail->Params.Sock.MaxRSOCK == 0)
                    {
                        Status = NARE_RSP_ERR_PARAM;
                        break;
                    }

                    /* Get a free, initialized HDB */
                    Status = NARE_SOCKGetFreeHDB(pDetail->Params.Sock.MaxRSOCK, &pHDBSock);
                    if ( Status == NARE_RSP_OK )
                    {
                        NARE_ASSERT_NULL_PTR(pHDBSock);

                        pHDBSock->Params.pSys          = pSys;
                        pHDBSock->Params.pDetail       = pDetail;
                        pHDBSock->Params.Cbf           = pRQB->args.channel.nare_request_upper_done_ptr;
                        pHDBSock->Params.SysPath       = pRQB->args.channel.sys_path;
                        pHDBSock->Params.HandleUpper   = pRQB->args.channel.handle_upper;
                        pHDBSock->NoOfRCtrlCnt         = pDetail->Params.Sock.MaxRSOCK;
                        /* set our handle in RQB */
                        pRQB->args.channel.handle     = pHDBSock->ThisHandle;
                        /* Do a SOCK- open channel request */
                        Status = NARE_SOCKOpenChannel(pHDBSock->ThisHandle,pHDBSock->Params.SysPath,pHDBSock->Params.pSys);
                        if ( Status == NARE_RSP_OK )
                        {
                            /* Request is underway now.. */
                            pHDBSock->State  = NARE_HANDLE_STATE_OPEN_SOCK_RUNNING;
                            pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                            Finish           = LSA_FALSE;
                        }
                        else
                        {
                            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE-SOCK channel failed. (Rsp: %Xh)",Status);
                            NARE_SOCKReleaseHDB(pHDBSock);
                        }
                    }
#else
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open channel failed. SOCK not supported yet!");
                    Status = NARE_RSP_ERR_PARAM;
#endif
                    break;

                default:
                    NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Unknown PathType in CDB (Type: %Xh)",pDetail->PathType);
                    Status = NARE_RSP_ERR_PARAM;
                    break;
            }/* switch */
        }
    }
    /*----------------------------------------------------------------------------*/
    /* Should we finish the request ?                                             */
    /*----------------------------------------------------------------------------*/
    if ( Finish )  /* finish the request ? */
    {
        /* upper - handle already set */
        NARE_RQB_SET_RESPONSE(pRQB,Status);
        NARE_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),Status);
        NARECallCbf(pRQB->args.channel.nare_request_upper_done_ptr,pRQB,pSys);
        /*----------------------------------------------------------------------------*/
        /* On error, if Syspath was opened we close the path                          */
        /*----------------------------------------------------------------------------*/
        if ( ( Status != NARE_RSP_OK ) &&  ( PathOpen)  )
        {
            NARE_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling NARE_RELEASE_PATH_INFO.");
            NARE_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);
            if ( PathStat != LSA_RET_OK)
            {
                NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"<--: NARE_RELEASE_PATH_INFO failed (Rsp: 0x%X).",PathStat);
                NARE_FATAL(NARE_FATAL_ERR_RELEASE_PATH_INFO);
            }
        }
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_open_channel()");
    NARE_EXIT();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_close_channel                          +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       NARE_OPC_CLOSE_CHANNEL        +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     NARE_RSP                Response      -                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error         -                             +*/
/*+     NARE_RQB_ARGS_TYPE      Args          -                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user.       +*/
/*+     NARE_RSP                Response    NARE_RSP_OK                     +*/
/*+                                         NARE_RSP_ERR_RESOURCE           +*/
/*+                                         NARE_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes a channel.                                         +*/
/*+               Close lower-layer channel if neccessary. (depends on      +*/
/*+               channel type). Releases Path-info. Note that this         +*/
/*+               function is done asynchron because of calling lower layer +*/
/*+               functions with callback. On error we may finish synchron. +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro NARE_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               NARE_RSP_ERR_OPCODE                                       +*/
/*+               NARE_RSP_ERR_PARAM                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_UPPER_IN_FCT_ATTR  nare_close_channel(NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    NARE_RSP                  Status;
    LSA_HANDLE_TYPE           Handle;
    LSA_UINT32                Finish;
    LSA_HANDLE_TYPE           HandleUpper;
    NARE_HDB_USER_PTR_TYPE    pHDBUser = LSA_NULL;
    NARE_HDB_DCP_PTR_TYPE     pHDBDcp;
    NARE_HDB_EDD_PTR_TYPE     pHDBEdd;
    LSA_UINT16                PathStat;
    LSA_SYS_PTR_TYPE          pSys;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp;
#ifndef NARE_CFG_NO_SOCK
    NARE_HDB_SOCK_PTR_TYPE    pHDBSock;
#endif
    LSA_VOID                  LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR, Cbf) (NARE_UPPER_RQB_PTR_TYPE pRQB);

    NARE_ENTER();
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_close_channel(pRQB: 0x%X)",pRQB);
    Status      = NARE_RSP_OK;
    Finish      = NOT_FINISHED;
    HandleUpper = 0;
    Handle      = 0;
    Cbf         = LSA_NULL;
    NARE_ASSERT_NULL_PTR(pRQB);
    /*---------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                  */
    /*---------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
    if ( NARE_RQB_GET_OPCODE(pRQB) != NARE_OPC_CLOSE_CHANNEL )
    {
        NARE_RQB_SET_RESPONSE(pRQB, NARE_RSP_ERR_OPCODE);
        Status = NARE_RSP_ERR_OPCODE;
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,">>>: nare_close_channel(): Invalid RQB-Opcode (0x%X)",NARE_RQB_GET_OPCODE(pRQB));
        NARE_RQB_ERROR(pRQB);
    }
    else
    {
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_CLOSE_CHANNEL (Handle: 0x%X)",NARE_RQB_GET_HANDLE(pRQB));
        /*-----------------------------------------------------------------------*/
        /* Check if it is allowed to close this channel.                         */
        /*-----------------------------------------------------------------------*/
        Handle = NARE_RQB_GET_HANDLE(pRQB);
        switch ( NARE_HANDLE_GET_TYPE(Handle))
        {
                /* ------------------------------------------------------------------*/
                /* USER Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case NARE_HANDLE_TYPE_USER:
                pHDBUser = NAREGetUserHDBFromHandle(Handle);
                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser, LSA_NULL) )
                {
                    Finish      = FINISHED_REL_SYSPTR;	/* normal close */
                    Cbf         = pHDBUser->Params.Cbf;
                    pSys        = pHDBUser->Params.pSys;
                    HandleUpper = pHDBUser->Params.HandleUpper;
                    NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
                    /* abort all NOTIFY-requests */
                    while (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser->NotifyReq.pBottom, LSA_NULL) )
                    {
                        NARE_RQB_REM_BLOCK_BOTTOM(pHDBUser->NotifyReq.pBottom,
                                                  pHDBUser->NotifyReq.pTop,
                                                  pRQBHelp);
                        /* finish the request */
                        NAREUserRequestFinish(pHDBUser,pRQBHelp,NARE_RSP_OK_CANCEL);
                    }
                    if (( pHDBUser->UserRCtrl.Used ) ||
                        ( pHDBUser->UserReqPendCnt ) ||
                        ( pHDBUser->Hello.Status != NARE_USR_HELLO_IDLE))
                    {
                        NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. User requests pending");
                        Status = NARE_RSP_ERR_SEQUENCE;
						Finish = FINISHED_OK;
                    }
                    else
                    {
                        pHDBUser->pHDBDcp->UserCnt--;
                        pHDBUser->pHDBEdd->UserCnt--;
#ifndef NARE_CFG_NO_SOCK
                        pHDBUser->pHDBSock->UserCnt--;
#endif
                    }
                }
                else
                {
                    NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid NARE-User Handle! (0x%X)",Handle);
                    Status = NARE_RSP_ERR_PARAM;
                    NARE_RQB_SET_RESPONSE(pRQB, Status);
                    NARE_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* DCP -Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case NARE_HANDLE_TYPE_DCP:
                pHDBDcp = NAREGetDcpHDBFromHandle(Handle);
                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBDcp, LSA_NULL) )
                {
                    Finish      = LSA_TRUE;
                    Cbf         = pHDBDcp->Params.Cbf;
                    pSys        = pHDBDcp->Params.pSys;
                    HandleUpper = pHDBDcp->Params.HandleUpper;
                    NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
                    if (( pHDBDcp->UserCnt ) ||                          /* some user-channel uses this DCP-channel ?    */
                        ( pHDBDcp->Hello.Status != NARE_DCP_HELLO_IDLE)) /* should not occur if all user channel closed! */
                    {
                        NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Channel still in use by user-channels");
                        Status = NARE_RSP_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBDcp->pOCReq, LSA_NULL))
                        {
                            NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Close already running.");
                            Status = NARE_RSP_ERR_SEQUENCE;
                        }
                        else
                        {
                            Status = NARE_DCPCloseChannel(pHDBDcp->DCPHandle,pHDBDcp->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {
                                /* Request is underway now.. */
                                pHDBDcp->State  = NARE_HANDLE_STATE_CLOSE_DCP_RUNNING;
                                pHDBDcp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                        }
                    }
                }
                else
                {
                    NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid NARE-DCP Handle! (0x%X)",Handle);
                    Status = NARE_RSP_ERR_PARAM;
                    NARE_RQB_SET_RESPONSE(pRQB, Status);
                    NARE_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* EDD -Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case NARE_HANDLE_TYPE_EDD:
                pHDBEdd = NAREGetEddHDBFromHandle(Handle);
                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL) )
                {
                    Finish      = LSA_TRUE;
                    Cbf         = pHDBEdd->Params.Cbf;
                    pSys        = pHDBEdd->Params.pSys;
                    HandleUpper = pHDBEdd->Params.HandleUpper;
                    NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
                    if ( pHDBEdd->UserCnt ) /* some user-channel uses this EDD-channel ?*/
                    {
                        NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Channel still in use by user-channels");
                        Status = NARE_RSP_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd->pOCReq, LSA_NULL))
                        {
                            NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Close already running");
                            Status = NARE_RSP_ERR_SEQUENCE;
                        }
                        else
                        {
                            Status = NARE_EDDCloseChannel(pHDBEdd->EDDHandle,pHDBEdd->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {
                                /* Request is underway now.. */
                                pHDBEdd->State  = NARE_HANDLE_STATE_CLOSE_EDD_RUNNING;
                                pHDBEdd->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                        }
                    }
                }
                else
                {
                    NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid NARE-EDD Handle! (0x%X)",Handle);
                    Status = NARE_RSP_ERR_PARAM;
                    NARE_RQB_SET_RESPONSE(pRQB, Status);
                    NARE_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* SOCK-Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case NARE_HANDLE_TYPE_SOCK: /* SOCK not supported yet */
#ifndef NARE_CFG_NO_SOCK
                pHDBSock = NAREGetSockHDBFromHandle(Handle);
                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSock, LSA_NULL) )
                {
                    Finish      = LSA_TRUE;
                    Cbf         = pHDBSock->Params.Cbf;
                    pSys        = pHDBSock->Params.pSys;
                    HandleUpper = pHDBSock->Params.HandleUpper;
                    NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
                    if ( pHDBSock->UserCnt ) /* some user-channel uses this DCP-channel ?*/
                    {
                        NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Channel still in use by user-channels");
                        Status = NARE_RSP_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSock->pOCReq, LSA_NULL))
                        {
                            NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Close already running.");
                            Status = NARE_RSP_ERR_SEQUENCE;
                        }
                        else
                        {
                            Status = NARE_SOCKCloseChannel(pHDBSock->SOCKHandle,pHDBSock->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {
                                /* Request is underway now.. */
                                pHDBSock->State  = NARE_HANDLE_STATE_CLOSE_SOCK_RUNNING;
                                pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                        }
                    }
                }
                else
                {
                    NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid NARE-SOCK Handle! (0x%X)",Handle);
                    Status = NARE_RSP_ERR_PARAM;
                    NARE_RQB_SET_RESPONSE(pRQB, Status);
                    NARE_RQB_ERROR(pRQB);
                }
#else
                NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): SOCK Handle error! (0x%X)",Handle);
                Status = NARE_RSP_ERR_PARAM;
                NARE_RQB_SET_RESPONSE(pRQB, Status);
                NARE_RQB_ERROR(pRQB);
#endif
                break;

            default:
                NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid Handle (0x%X)",Handle);
                Status = NARE_RSP_ERR_PARAM;
                NARE_RQB_SET_RESPONSE(pRQB, Status);
                NARE_RQB_ERROR(pRQB);
                break;
        }
    }
	/*---------------------------------------------------------------------------*/
	/* Finish the request.                                                       */
	/*---------------------------------------------------------------------------*/
	if ( Finish != NOT_FINISHED)
	{
		NARE_DETAIL_PTR_TYPE	pDetail;
		NARE_RQB_SET_RESPONSE(pRQB,Status);
		NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
        NARE_ASSERT_NULL_PTR(pHDBUser);
		NARE_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),Status);
		/* take the values out of the HDB */
		pSys    = pHDBUser->Params.pSys;
		pDetail = pHDBUser->Params.pDetail;
		if (Finish == FINISHED_REL_SYSPTR)
		{	/* we release the HDB. Handle closed now        */
			NARE_UserReleaseHDB(pHDBUser);
		}
		NARECallCbf(Cbf,pRQB,pSys);
		if (Finish == FINISHED_REL_SYSPTR)
		{	/* release PATH-Info !                          */
			NARE_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);
			if (PathStat != LSA_RET_OK ) 
			{
				NARE_FATAL(NARE_FATAL_ERR_RELEASE_PATH_INFO);
			}
		}
	}
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_close_channel()");
	NARE_EXIT();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_request                                +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       varies:                       +*/
/*+                                           NARE_OPC_RESOLVE_CONFIG       +*/
/*+                                           NARE_OPC_TIMER                +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     NARE_RSP                Response      -                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error         -                             +*/
/*+     NARE_RQB_ARGS_TYPE      Args          varies depending on opcode    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     upper-handle from open_channel  +*/
/*+     NARE_RSP                Response    NARE_RSP_OK                     +*/
/*+                                         NARE_RSP_ERR_RESOURCE           +*/
/*+                                         NARE_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given on open_channel-request.             +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro NARE_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               NARE_RSP_ERR_OPCODE                                       +*/
/*+               NARE_RSP_ERR_PARAM                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_UPPER_IN_FCT_ATTR  nare_request(NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_HANDLE_TYPE           Handle;
    NARE_HDB_USER_PTR_TYPE    pHDB;

    NARE_ENTER();
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_request(pRQB: 0x%X)",pRQB);
    NARE_ASSERT_NULL_PTR(pRQB);
    Handle   = NARE_RQB_GET_HANDLE(pRQB);
    /* TIMER-Request needs special handling  */
    if (  NARE_RQB_GET_OPCODE(pRQB) == NARE_OPC_TIMER )
    {
        NARERequestTimer(pRQB);
    }
    else
    {
        pHDB = NAREGetUserHDBFromHandle(Handle);
        if (! LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
        {
            switch ( NARE_RQB_GET_OPCODE(pRQB))
            {
                case NARE_OPC_RESOLVE_CONFIG:
                    NARE_UPPER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_RESOLVE_CONFIG (pRQB: 0x%X, Handle: 0x%X, Mode: %d)",pRQB,NARE_RQB_GET_HANDLE(pRQB),pRQB->args.ResConf.Mode);
                    NARERequestResolveConfig(pRQB,pHDB);
                    break;

                case NARE_OPC_RESOLVE_CONFIG_CANCEL:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_RESOLVE_CONFIG_CANCEL (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestResolveConfigCancel(pRQB,pHDB);
                    break;

                case NARE_OPC_RESOLVE_CONFIG_NOTIFY:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_RESOLVE_CONFIG_NOTIFY (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestResolveConfigNotify(pRQB,pHDB);
                    break;

                case NARE_OPC_TEST_IP:
                    NARE_UPPER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_TEST_IP (pRQB: 0x%X, Handle: 0x%X Delay: 0x%x)",pRQB,NARE_RQB_GET_HANDLE(pRQB), pRQB->args.TestIP.UseDelay);
                    NARERequestTestIP(pRQB,pHDB);
                    break;

                case NARE_OPC_TEST_IP_CANCEL:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_TEST_IP_CANCEL (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestTestIPCancel(pRQB,pHDB);
                    break;

                case NARE_OPC_RESOLVE_IP:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_RESOLVE_IP (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestResolveIP(pRQB,pHDB);
                    break;

                case NARE_OPC_HELLO:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_HELLO (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestHello(pRQB,pHDB);
                    break;

                case NARE_OPC_HELLO_STOP:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_HELLO_STOP (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestHelloStop(pRQB,pHDB);
                    break;

                default:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR,">>>: nare_request(): Invalid RQB-Opcode (pRQB: 0x%X, Opcode: 0x%X)",pRQB,NARE_RQB_GET_OPCODE(pRQB));
                    NAREUserRequestFinish(pHDB,pRQB,NARE_RSP_ERR_OPCODE);
                    break;
            } /* switch */
        } /* if */
        else
        {
            /* wrong handle */
            NARE_RQB_SET_RESPONSE(pRQB, NARE_RSP_ERR_PARAM);
            NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,">>>: nare_request(): Invalid Handle (0x%X)",Handle);
            NARE_RQB_ERROR(pRQB);
        }
    } /* else */
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_request()");
    NARE_EXIT();
}

/*****************************************************************************/
/*  end of file NARE_USR.C                                                    */
/*****************************************************************************/

