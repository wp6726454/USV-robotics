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
/*  C o m p o n e n t     &C: OHA (Option HAndler)                      :C&  */
/*                                                                           */
/*  F i l e               &F: oha_snmx.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA SNMPX-LowerLayer interface functions         */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  19
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_SNMX */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles */
#include "oha_int.h"            /* internal header */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

#ifdef OHA_CFG_USE_SNMPX

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

SNMPX_OID_TYPE OHA_LldpTreeOid[] = {OHA_LLDP_MIB_TREE};

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
/*+  Functionname          :    oha_snmpx_request_lower_done                +*/
/*+  Input                      OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for SNMPX-requests.                      +*/
/*+               Fills Error-Structure in NDB with SNMPX-Response          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_SNMPX_LOWER_IN_FCT_ATTR oha_snmpx_request_lower_done(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_SNMPX_PTR_TYPE   pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN :oha_snmpx_request_lower_done(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /* SNMPX-request always done from a SNMPX-Channel. !        */

    pHDB = OHAGetSnmpxHDBFromHandle(OHA_SNMPX_RQB_GET_HANDLE(pRQB));

    /* save response from lower layer in global error-structure */

    OHA_GET_GLOB_PTR()->LLError = pRQB->err;

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
    }

    switch (OHA_SNMPX_RQB_GET_OPCODE(pRQB) )
    {
        case SNMPX_OPC_OPEN_CHANNEL:
            OHA_SNMPXOpenChannelDone(pRQB,pHDB);
            break;
        case SNMPX_OPC_CLOSE_CHANNEL:
            OHA_SNMPXCloseChannelDone(pRQB,pHDB);
            break;
#if 0
        case SNMPX_OPC_CONTROL_MASTER:
            OHA_SNMPXControlAgentDone(pRQB,pHDB);
            break;
#endif
        case SNMPX_OPC_REGISTER_AGENT:
            OHA_SNMPXRegisterAgentDone(pRQB,pHDB);
            break;
        case SNMPX_OPC_UNREGISTER_AGENT:
            OHA_SNMPXUnregisterAgentDone(pRQB,pHDB);
            break;
        case SNMPX_OPC_PROVIDE_AGENT:
            OHA_SNMPXOidProvideDone(pRQB,pHDB);
            break;
        case SNMPX_OPC_RECEIVE_AGENT:
            /*  OHA_SNMPXOidReceiveDone(pRQB,pHDB);  */
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_snmpx_request_lower_done()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXOpenChannel                        +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA SNMPX-Channel handle                         +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to SNMPX. The Request will be +*/
/*+               finished by calling the SNMPX-Callbackfunction.           +*/
/*+               oha_snmpx_request_lower_done() if OHA_OK.                 +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);
    Result = OHA_OK;

    UserId.uvar32 = 0;
    OHA_SNMPX_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SNMPX_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SNMPX_RQB_SET_OPCODE(pRQB,SNMPX_OPC_OPEN_CHANNEL);

        pRQB->err.lsa_component_id = LSA_COMP_ID_UNUSED;

        pRQB->args.Channel.handle_upper  = OHAHandle;
        pRQB->args.Channel.sys_path      = SysPath;
        pRQB->args.Channel.snmpx_request_upper_done_ptr = oha_snmpx_request_lower_done;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: SNMPX-Request: SNMPX_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
        OHA_SNMPX_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXOpenChannel. Result: 0x%X",
                          Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXOpenChannelDone                    +*/
/*+  Input                      OHA_SNMPX_LOWER_RQB_PTR_TYPE   pRQB         +*/
/*+                             OHA_HDB_SNMPX_PTR_TYPE         pHDB         +*/
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
/*+               - save SNMPX-Channel                                      +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXOpenChannelDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response;
    LSA_UINT16               RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    Response = OHA_OK;

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: SNMPX_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SNMPX_RQB_GET_RESPONSE(pRQB));

    if ( OHA_SNMPX_RQB_GET_RESPONSE(pRQB) == SNMPX_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-Open Channel successful");
        /* save SNMPX-Channel in HDB */
        pHDB->SnmpxHandle = pRQB->args.Channel.handle;

    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-Open Channel failed.[0x%X]",OHA_SNMPX_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_SNMPX_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SNMPXChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXOpenChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXCloseChannel                       +*/
/*+  Input                      LSA_HANDLE_TYPE       SNMPXHandle           +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXHandle          : SNMPX handle to close                           +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to SNMPX. The Request will be+*/
/*+               finished by calling the SNMPX-Callbackfunction.           +*/
/*+               oha_snmpx_request_lower_done()                            +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXCloseChannel(
    LSA_HANDLE_TYPE       SNMPXHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXCloseChannel(SNMPXHandle: 0x%X)",
                          SNMPXHandle);
    Result = OHA_OK;

    UserId.uvar32 = 0;
    OHA_SNMPX_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SNMPX_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {

        OHA_SNMPX_RQB_SET_OPCODE(pRQB,SNMPX_OPC_CLOSE_CHANNEL);
        OHA_SNMPX_RQB_SET_HANDLE(pRQB,SNMPXHandle);

        pRQB->err.lsa_component_id = LSA_COMP_ID_UNUSED;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: SNMPX-Request: SNMPX_OPC_CLOSE_CHANNEL (SNMPXHandle: 0x%X,pRQB: 0x%X)",SNMPXHandle,pRQB);
        OHA_SNMPX_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXCloseChannel(Result: 0x%X)", Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXCloseChannelDone                   +*/
/*+  Input                      OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                             OHA_HDB_SNMPX_PTR_TYPE        pHDB          +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXCloseChannelDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response;
    LSA_UINT16               RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXCloseChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);
    Response = OHA_OK;

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: SNMPX_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SNMPX_RQB_GET_RESPONSE(pRQB));

    if ( OHA_SNMPX_RQB_GET_RESPONSE(pRQB) == SNMPX_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-Close Channel successful");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX-Close Channel failed.[0x%X]",OHA_SNMPX_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    /* Could all allocated Receive-RQBs be freed ? */
    OHA_ASSERT(pHDB->RxPendingCnt == 0);

    OHA_SNMPX_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SNMPXChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXCloseChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXChannelDone                        +*/
/*+  Input                      OHA_HDB_SNMPX_PTR_TYPE       pHDB           +*/
/*+                             LSA_UINT16                   Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  Response   : Responsecode of actual action taken (depends on state)    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handler for Open and Close-Channel requests to OHA for    +*/
/*+               SNMPX-Channels. This function is called after a lower     +*/
/*+               layer called back the OHA (e.g. after SNMPX finished his  +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_SNMPXChannelDone(
    OHA_HDB_SNMPX_PTR_TYPE     pHDB,
    LSA_UINT16                 Response)
{

    OHA_UPPER_RQB_PTR_TYPE       pRQB;
    LSA_UINT16                   PathStat = LSA_RET_OK;
    LSA_BOOL                     Finish   = LSA_FALSE; /* dont finish request by default */
    LSA_BOOL                     Release  = LSA_FALSE; /* dont release PATH-Info by default */
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE             pSys;
	OHA_DETAIL_PTR_TYPE          pDetail;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB,Response);

    OHA_ASSERT_NULL_PTR(pHDB);

    pRQB   = pHDB->pOCReq;
    Cbf    = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
	pSys   = pHDB->Params.pSys;
	pDetail = pHDB->Params.pDetail;

    OHA_ASSERT_NULL_PTR(pRQB);
	
    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SNMPXChannelDone done (NICId: 0x%X,Rsp: 0x%X,State: 0x%X)",pHDB->Params.pDetail->Params.Snmpx.NICId,Response,pHDB->SnmpxState);

    switch (pHDB->SnmpxState)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a SNMPX-Open Channel request.                       */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_SNMPX_RUNNING:
            if ( Response == OHA_OK)
            {
                /* OHA_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail; */

                /* register the SubAgent in OHA for the LLDP-MIB  */
                pHDB->SnmpxState  = OHA_HANDLE_STATE_REGISTER_SNMPX_RUNNING;

                Response = OHA_SNMPXRegisterAgent(pHDB);

                if ( Response != OHA_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX Issuing SnmpxRegisterAgent-Request failed.");

                    pHDB->SnmpxState  = OHA_HANDLE_STATE_CLOSE_ERROR_SNMPX_RUNNING;

                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-Closing Channel because of error.");

                    if ( OHA_SNMPXCloseChannel(pHDB->SnmpxHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SNMPX Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        OHA_FATAL(OHA_FATAL_ERR_SNMPX);
                    }
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open SNMPX. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a OPEN-POR SNMPX-request.                           */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_REGISTER_SNMPX_RUNNING:

            if ( Response == OHA_OK)
            {
                /* NOTE: Providing Recv-Indication Request must be          */
                /*       the last action before going into State "open".    */
                /*       So only Indciations while in Open-state must be    */
                /*       handled and reprovided!                            */

                /* provide SNMPX-Recv-Requests */
                Response = OHA_SNMPXRecvProvide(pHDB, OHA_SNMP_NUM_OF_RECV);
            }

            if ( Response == OHA_OK )
            {
                /* no more actions needed. Channel is open now. */
                pHDB->SnmpxState  = OHA_HANDLE_STATE_OPEN;
                pHDB->RequestState  = OHA_SNMPX_STATE_STOP;
                pHDB->RxPendingCnt = 0; /* no Rx pending within SNMPX */
                Finish = LSA_TRUE;
            }
            else
            {
                /* normally there should be done a Unregister Agent now, but the registered oids */
                /* are deleted by CloseCHannel too */

                /* Request OpenChannel or Receive Provide failed. Cancel Recv-Requests (by CloseChannel) */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX Receive Provide request failed. Response: 0x%X", Response);

                pHDB->OpenResponse = Response; /* save for later */

                pHDB->SnmpxState = OHA_HANDLE_STATE_CLOSE_ERROR_SNMPX_RUNNING;

                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-Closing Channel because of error");
                if ( OHA_SNMPXCloseChannel(pHDB->SnmpxHandle,pHDB->Params.pSys) != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SNMPX Channel().");
                    OHA_FATAL(OHA_FATAL_ERR_SNMPX);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SNMPX-Close-Channel because of error within Open  */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_ERROR_SNMPX_RUNNING:
            if ( Response == OHA_OK)
            {
                Response = pHDB->OpenResponse;  /* get Error-Response  */

                /* release PATH-Info ! */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"Cannot Close SNMPX Channel().");
                OHA_FATAL(OHA_FATAL_ERR_SNMPX);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SNMPX-Close-Channel request                       */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_SNMPX_RUNNING:

            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                /* is this a fatal error ?                      */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SNMPX Channel(). Response: 0x%X", Response);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a Unregister Agent SNMPX-request.                   */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_UNREGISTER_SNMPX_RUNNING:

            if ( Response == OHA_OK)
            {
                Response = OHA_SNMPXCloseChannel(pHDB->SnmpxHandle,pHDB->Params.pSys);
                if ( Response == OHA_OK )
                {
                    /* Request is underway now.. */
                    pHDB->SnmpxState  = OHA_HANDLE_STATE_CLOSE_SNMPX_RUNNING;
                }
            }
            else
            {
                /* is this a fatal error ?                      */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SNMPX. Response: 0x%X", Response);
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Other states not expected within this function.                   */
            /*-------------------------------------------------------------------*/
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */

    if ( Release )  /* release the PATH-Info ? */
    {
        /* we release the HDB  */
        OHA_SNMPXReleaseHDB(pHDB);
    }

    if ( Finish )  /* finish the request ? */
    {
        pHDB->pOCReq = LSA_NULL;  /* to signal no more request pending.      */

        OHA_RQBSetResponse(pRQB,Response);
        /* upper - handler already set */
        OHA_UPPER_TRACE_03(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Response);
        OHA_CallCbf(Cbf,pRQB,pSys);
    }

    if ( Release )  /* release the PATH-Info ? */
    {
		OHA_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);

		if (PathStat != LSA_RET_OK ) OHA_FATAL(OHA_FATAL_ERR_RELEASE_PATH_INFO);
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXCheckNICUsed                       +*/
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
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_SNMPXCheckNICUsed(
    LSA_UINT16 NICId)
{
    OHA_HDB_SNMPX_PTR_TYPE  pHDB  = OHA_GET_HSNMPX_PTR(0);
    LSA_BOOL              Found;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdxNic(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXCheckNICUsed(NICId: 0x%X)",
                          NICId);
    LSA_UNUSED_ARG(NICId); /* prevent compiler warning for not accessing */

    {
        if ( pHDB->SnmpxState != OHA_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            Found = LSA_FALSE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPXCheckNICUsed(Found:  0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXReleaseHDB                         +*/
/*+  Input/Output          :    OHA_HDB_SNMPX_PTR_TYPE pHDB                 +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBSnmpx   : Pointer to User Handle to release                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release SNMPX -HDB                                        +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXReleaseHDB(
    OHA_HDB_SNMPX_PTR_TYPE pHDB)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    pHDB->SnmpxState = OHA_HANDLE_STATE_FREE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXReleaseHDB()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXGetFreeHDB                         +*/
/*+  Input/Output          :    OHA_HDB_SNMPX_PTR_TYPE OHA_LOCAL..pHDBSnmpx +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA SNMPX  -channel handle to get pointer to HDB for.     +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free SNMPX -HDB                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXGetFreeHDB(
    OHA_HDB_SNMPX_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBSnmpx)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found;
    OHA_HDB_SNMPX_PTR_TYPE    pHDB  = OHA_GET_HSNMPX_PTR(0);
    LSA_UINT32                i     = 0;
    LSA_USER_ID_TYPE          UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXGetFreeHDB(pHDBSnmpx: 0x%X)",
                          pHDBSnmpx);
    UserId.uvar32 = 0;

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    {
        if ( pHDB->SnmpxState == OHA_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            Found = LSA_FALSE;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..                  */
    /* --------------------------------------------------------------------------*/
    if ( Found )
    {
        pHDB->SnmpxState = OHA_HANDLE_STATE_CLOSED;
        *pHDBSnmpx = pHDB;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_SNMPXGetFreeHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBSnmpx);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXRecvProvide                        +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                             LSA_UINT32                   Cnt            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Cnt                  : Number of Recv-Request to send to SNMPX         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends Indication-Resources to SNMPX                       +*/
/*+               (SNMPX_OPC_OID_PROVIDE)                                   +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SNMPX-Requests                     +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe RECV-Request pending     +*/
/*+                     within SNMPX!                                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXRecvProvide(
    OHA_HDB_SNMPX_PTR_TYPE    pHDB,
    LSA_UINT32               Cnt)
{
    OHA_SNMPX_LOWER_RQB_PTR_TYPE            pRQB;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    LSA_UINT16                              RetVal;
    OHA_SNMPX_LOWER_MEM_PTR_TYPE            pRcv;  /* Pointer for Frame memory. */
    OHA_SNMPX_LOWER_MEM_PTR_TYPE            pOid;  /* Pointer for OID. */
    SNMPX_UPPER_OBJECT_PTR_TYPE             snmp_recv;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXRecvProvide(pHDB: 0x%X, Cnt: 0x%X)",
                          pHDB,Cnt);

    OHA_ASSERT_NULL_PTR(pHDB);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Receive-Request to SNMPX                                */
    /*---------------------------------------------------------------------------*/

    pHDB->SnmpxRQBCnt = 0; /* no Rx pending within SNMPX */

    while ((Status == OHA_OK) && (Cnt))
    {
        OHA_SNMPX_ALLOC_LOWER_MEM(&pRcv,
                                  UserId,
                                  SNMPX_SNMP_MAX_LEN,
                                  pSys);

        OHA_SNMPX_ALLOC_LOWER_MEM(&pOid,
                                  UserId,
                                  SNMPX_MAX_OID_LEN * sizeof(SNMPX_OID_TYPE),
                                  pSys);

        if ( LSA_HOST_PTR_ARE_EQUAL(pRcv, LSA_NULL) || LSA_HOST_PTR_ARE_EQUAL(pOid, LSA_NULL))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SNMPXRecvProvide(). Allocation receive memory failed.");
        }
        else
        {
            /* allocate RQB */
            OHA_SNMPX_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SNMPX_RQB_TYPE),pSys);

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
            {
                /* provide recv resources */
                OHA_SNMPX_RQB_SET_OPCODE(pRQB,SNMPX_OPC_PROVIDE_AGENT);
                OHA_SNMPX_RQB_SET_HANDLE(pRQB,pHDB->SnmpxHandle);

                pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;

                OHA_LOWER_TRACE_03(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SNMPX-Request: SNMPX_OPC_SNMP_RECEIVE_REQ (Handle: 0x%X,SNMPXHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SnmpxHandle,pRQB);

                snmp_recv = &pRQB->args.Object;
                OHA_ASSERT (snmp_recv);

                snmp_recv->TransactionID = 0;   /* don't cares */
                snmp_recv->MessageType   = 0;
                snmp_recv->VarType       = 0;
                snmp_recv->AccessControl = 0;
                snmp_recv->SnmpError     = 0;
                snmp_recv->SnmpVersion   = 0;

                snmp_recv->pOid          = (SNMPX_UPPER_OID_PTR_TYPE)pOid;
                snmp_recv->OidLen        = SNMPX_MAX_OID_LEN;
                snmp_recv->pVarValue     = pRcv;
                snmp_recv->VarLength     = SNMPX_SNMP_MAX_LEN;

                OHA_SNMPX_REQUEST_LOWER(pRQB,pSys);
                pHDB->SnmpxRQBCnt++;  /* number of RQBs pending within SNMPX */
                pHDB->RxPendingCnt++; /* number of Rx-Request pending within SNMPX */
            }
            else
            {
                Status = OHA_ERR_RESOURCE;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-Allocating lower RQB-memory failed!");
                OHA_SNMPX_FREE_LOWER_MEM(&RetVal,pRcv,pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        Cnt--;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXRecvProvide(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXOidProvideDone                     +*/
/*+  Input                      OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                             OHA_HDB_SNMPX_PTR_TYPE        pHDB          +*/
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
/*+              - frees RQB memory                                         +*/
/*+              - call global state handling for close channel for further +*/
/*+                actions.                                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXOidProvideDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB)
{
    LSA_UINT16               RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXOidProvideDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SNMPX_OPC_CANCEL_RECV done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SNMPX_RQB_GET_RESPONSE(pRQB));

    switch (OHA_SNMPX_RQB_GET_RESPONSE(pRQB))
    {
        case SNMPX_RSP_OK:
        case SNMPX_RSP_ERR_LOC_ABORT:
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-Cancel Receive successful");
            break;
        case SNMPX_RSP_ERR_PARAM:
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"No SNMPX-Cancel Receives found");
            break;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-Cancel Receive failed.[0x%X]",OHA_SNMPX_RQB_GET_RESPONSE(pRQB));
            break;
    }

    OHA_SNMPX_FREE_LOWER_MEM(&RetVal,pRQB->args.Object.pOid,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SNMPX_FREE_LOWER_MEM(&RetVal,pRQB->args.Object.pVarValue,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SNMPX_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    /* Could all allocated Response- and Receive-RQBs be freed ? */
    /* OHA_ASSERT(pHDB->RxPendingCnt == 0); */ /* moved to close-channel */
    /* OHA_ASSERT(pHDB->TxPendingCnt == 0); */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPXOidProvideDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXRegisterAgent                      +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: register the OHA SubAgent for the LLDP-MIB                +*/
/*+               (SNMPX_OPC_REGISTER_AGENT)                                +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SNMPX-Requests                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXRegisterAgent(
    OHA_HDB_SNMPX_PTR_TYPE    pHDB)
{
    OHA_SNMPX_LOWER_RQB_PTR_TYPE            pRQB;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    SNMPX_UPPER_REGISTER_PTR_TYPE           snmp_register;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXOpenPort(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SNMPX_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SNMPX_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SNMPX_RQB_SET_OPCODE(pRQB,SNMPX_OPC_REGISTER_AGENT);
        OHA_SNMPX_RQB_SET_HANDLE(pRQB,pHDB->SnmpxHandle);

        pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SNMPX-Request: SNMPX_OPC_REGISTER_AGENT (Handle: 0x%X,SNMPXHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SnmpxHandle,pRQB);

        snmp_register = &pRQB->args.Register;
        snmp_register->Priority = 0;
        snmp_register->pOid     = OHA_LldpTreeOid;
        snmp_register->OidLen   = OHA_LLDP_MIB_TREE_LEN;

        OHA_SNMPX_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXRegisterAgent(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXRegisterAgentDone                  +*/
/*+  Input                      OHA_SNMPX_LOWER_RQB_PTR_TYPE pRQB           +*/
/*+                             OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Register Agent for the LLDP-MIB done handling             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXRegisterAgentDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB)
{
    LSA_UINT16                      Response;
    LSA_UINT16                      RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXOpenPortDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SNMPX_OPC_REGISTER_AGENT done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SNMPX_RQB_GET_RESPONSE(pRQB));

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    Response  = OHA_OK;

    switch (OHA_SNMPX_RQB_GET_RESPONSE(pRQB))
    {
        case SNMPX_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            OHA_ASSERT (SNMPX_OPC_REGISTER_AGENT == OHA_SNMPX_RQB_GET_OPCODE(pRQB));
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-ERROR: SNMPX_OPC_REGISTER_AGENT failed.(0x%X).",OHA_SNMPX_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_SNMPX_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SNMPXChannelDone(pHDB,Response); /* further actions taken in channel handler */
    /* OHA_SNMPXRequestHandler(pHDB, LSA_NULL, Response); */ /* further actions taken in request handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPXRegisterAgentDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXUnregisterAgent                    +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: register the OHA SubAgent for the LLDP-MIB                +*/
/*+               (SNMPX_OPC_REGISTER_AGENT)                                +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SNMPX-Requests                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXUnregisterAgent(
    OHA_HDB_SNMPX_PTR_TYPE    pHDB)
{
    OHA_SNMPX_LOWER_RQB_PTR_TYPE            pRQB;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    SNMPX_UPPER_REGISTER_PTR_TYPE           snmp_register;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXOpenPort(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SNMPX_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SNMPX_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SNMPX_RQB_SET_OPCODE(pRQB,SNMPX_OPC_UNREGISTER_AGENT);
        OHA_SNMPX_RQB_SET_HANDLE(pRQB,pHDB->SnmpxHandle);

        pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SNMPX-Request: SNMPX_OPC_UNREGISTER_AGENT (Handle: 0x%X,SNMPXHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SnmpxHandle,pRQB);

        snmp_register = &pRQB->args.Register;
        snmp_register->Priority = 0;
        snmp_register->pOid     = OHA_LldpTreeOid;
        snmp_register->OidLen   = OHA_LLDP_MIB_TREE_LEN;

        OHA_SNMPX_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXUnregisterAgent(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXUnregisterAgentDone                +*/
/*+  Input                      OHA_SNMPX_LOWER_RQB_PTR_TYPE pRQB           +*/
/*+                             OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Unregister Agent for the LLDP-MIB done handling           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXUnregisterAgentDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB)
{
    LSA_UINT16                      Response;
    LSA_UINT16                      RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXOpenPortDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SNMPX_OPC_UNREGISTER_AGENT done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SNMPX_RQB_GET_RESPONSE(pRQB));

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    Response  = OHA_OK;

    switch (OHA_SNMPX_RQB_GET_RESPONSE(pRQB))
    {
        case SNMPX_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            OHA_ASSERT (SNMPX_OPC_UNREGISTER_AGENT == OHA_SNMPX_RQB_GET_OPCODE(pRQB));
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-ERROR: SNMPX_OPC_UNREGISTER_AGENT failed.(0x%X).",OHA_SNMPX_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_SNMPX_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SNMPXChannelDone(pHDB,Response); /* further actions taken in channel handler */
    /* OHA_SNMPXRequestHandler(pHDB, LSA_NULL, Response); */ /* further actions taken in request handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPXUnregisterAgentDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestSnmpxControl                     +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_START/STOP_SNMP         +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.SnmpxControl                            +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to activate/deactivate the SNMP-Agent by the user +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestSnmpxControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16                Response = OHA_OK;
    OHA_HDB_SNMPX_PTR_TYPE     pHDBSnmpx;
    LSA_BOOL                  Finish = LSA_TRUE; /* finish request by default */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestSnmpxControl(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    /* Check if we have SNMPX-Channel, only one over all NIC's. */
    pHDBSnmpx  = OHA_SNMPXGetHandleByNIC(0);  /* only EDD and SNMPX for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBSnmpx,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_START/STOP_SNMP failed. No Lower Channel for SNMPX");
    }
    else
    {
        OHA_UPPER_SNMPX_CONTROL_PTR_TYPE control = &pRQB->args.SnmpxControl;

        switch ( control->Mode )
        {
            case OHA_CTRL_START_SNMPX:
                if (pHDBSnmpx->UserRQBCnt) /* no queueing of user-requests in this version */
                {
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_START_SNMP failed. Already one User-Request active");
                    Response = OHA_ERR_SEQUENCE;
                }
                else
                {
                    Finish = LSA_FALSE;  /* further actions */
                }
                break;
            case OHA_CTRL_STOP_SNMPX:
                if (pHDBSnmpx->UserRQBCnt) /* no queueing of user-requests in this version */
                {
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_STOP_SNMP failed. Already one User-Request active");
                    Response = OHA_ERR_SEQUENCE;
                }
                else
                {
                    Finish = LSA_FALSE;  /* further actions */
                }
                break;
            default:
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX-Control failed.[0x%X]",OHA_ERR_SYNTAX);
                Response = OHA_ERR_SYNTAX;
                break;
        }
    }

    if ( Finish )  /* finish the request ? */
    {
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestSnmpxControl(Request finished)");
    }
    else
    {
        /* queue the request in the SNMPX-HDB */
        OHA_RQB_PUT_BLOCK_TOP(pHDBSnmpx->UserRQBs.pBottom,
                              pHDBSnmpx->UserRQBs.pTop,
                              pRQB);
        pHDBSnmpx->UserRQBCnt++;
        OHA_SNMPXRequestHandler(pHDBSnmpx, pRQB, OHA_OK);  /* call the snmpx-handler */
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestSnmpxControl(Request queued)");
    }
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXUserCheckPending                   +*/
/*+  Input/Output          :    OHA_HDB_SNMPX_PTR_TYPE     pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SNMPX-HDB                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXUserCheckPending(
    OHA_HDB_SNMPX_PTR_TYPE   pHDBSnmpx,
    LSA_UINT16              Response)
{
    OHA_HDB_USER_PTR_TYPE     pHDBUser;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXUserCheckPending(pHDBSnmpx: 0x%X, Response: 0x%X)",
                          pHDBSnmpx, Response);

    if ( pHDBSnmpx->UserRQBCnt ) /* User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBSnmpx->UserRQBs.pBottom,
                                 pHDBSnmpx->UserRQBs.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
        pHDBSnmpx->UserRQBCnt--;

        /* control = &pRQBUser->args.SockControl; */
        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);
        OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPXUserCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXRequestHandler                     +*/
/*+                        :    OHA_HDB_SNMPX_PTR_TYPE    pHDB              +*/
/*+                        :    LSA_UINT16               Response           +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQBUser   : Pointer to User-RCTRL-Block                               +*/
/*+  Response   : Response of last action. if <> OHA_OK the Request         +*/
/*+               will be finished with this error. Set to OHA_OK if        +*/
/*+               not error present (e.g. on first call)                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Statemachine for OHA-User-Requests (ControlMasterAgent).  +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXRequestHandler(
    OHA_HDB_SNMPX_PTR_TYPE          pHDBSnmpx,
    OHA_UPPER_RQB_PTR_TYPE          pRQBUser,
    LSA_UINT16                      Response)
{
    LSA_BOOL                  Ready = LSA_FALSE;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SnmpxRequestHandler(pHDBSnmpx: 0x%X, pRQBUser: 0x%X, Response: 0x%X)",
                          pHDBSnmpx, pRQBUser, Response);

    /* -----------------------------------------------------------------------*/
    /* forward in statemachine till an asynchroneous request is underway which*/
    /* we have to wait for.                                                   */
    /* -----------------------------------------------------------------------*/
    while (! Ready)
    {
        Ready = LSA_TRUE;      /* Default: break loop because request underway */

        switch (pHDBSnmpx->RequestState)
        {
            case OHA_SNMPX_STATE_STOP:   /* initial, after OpenChannel: Agent is in Deact */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-SM [%X]: STATE_STOP (Resp: %Xh)",pHDBSnmpx->ThisHandle,Response);

                /* comes from the user ? */
                /* stopped state and a User-STOP_SNMP => no action */
                if (pRQBUser && (pRQBUser->args.SnmpxControl.Mode == OHA_CTRL_STOP_SNMPX))
                {
                    /* check for pending user-requests and confirm  */
                    OHA_SNMPXUserCheckPending(pHDBSnmpx, OHA_OK);
                    break;
                }

                /* User-OHA_OPC_START_SNMP => activate the SNMP-agent */
                if (pRQBUser && (pRQBUser->args.SnmpxControl.Mode == OHA_CTRL_START_SNMPX))
                {
                    /* Activate the SNMP-Agent   */
                    LSA_UINT16 RetVal = OHA_SNMPXControlAgent(pRQBUser, pHDBSnmpx);
                    if ( RetVal != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX Issuing SnmpxAgentActivate-Request failed.");
                        /* check for pending user-requests and confirm  */
                        OHA_SNMPXUserCheckPending(pHDBSnmpx, RetVal);
                    }
                    else
                    {
                        pHDBSnmpx->RequestState = OHA_SNMPX_STATE_WF_START;
                    }
                    break;
                }
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            case OHA_SNMPX_STATE_WF_START:   /* ControlStart.Req is active    */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-SM [%X]: STATE_WF_START (Resp: %Xh)",pHDBSnmpx->ThisHandle,Response);

                /* is there an error during Control-Start ? */
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX SnmpxAgentActivate request failed. Response: 0x%X", Response);

                    pHDBSnmpx->RequestState = OHA_SNMPX_STATE_STOP;
                }
                else
                {
                    /* no more actions needed. Agent is started now. */
                    pHDBSnmpx->RequestState = OHA_SNMPX_STATE_START; /* initialize */
                }

                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_SNMPXUserCheckPending(pHDBSnmpx, Response);
                break;

            case OHA_SNMPX_STATE_WF_STOP:     /* OHA_OPC_STOP_SNMP.Req is active   */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-SM [%X]: STATE_WF_STOP (Resp: %Xh)",pHDBSnmpx->ThisHandle,Response);

                /* is there an error during DEACTIVATE_AGENT ? */
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX SnmpxAgentDeactivate request failed. Response: 0x%X", Response);

                    pHDBSnmpx->RequestState = OHA_SNMPX_STATE_START;
                }
                else
                {
                    pHDBSnmpx->RequestState = OHA_SNMPX_STATE_STOP;
                }
                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_SNMPXUserCheckPending(pHDBSnmpx, Response);
                break;

            case OHA_SNMPX_STATE_START:      /* SNMP-agent is active     */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SNMPX-SM [%X]: STATE_START (Resp: %Xh)",pHDBSnmpx->ThisHandle,Response);

                if (pRQBUser && (pRQBUser->args.SnmpxControl.Mode == OHA_CTRL_STOP_SNMPX))
                {
                    LSA_UINT16 RetVal = OHA_SNMPXControlAgent(pRQBUser, pHDBSnmpx);
                    if (RetVal != OHA_OK)
                    {                           /* confirm to the user, if exist */
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SNMPX Issuing SnmpxAgentDeactivate-Request failed.");
                        /* check for pending user-requests   */
                        OHA_SNMPXUserCheckPending(pHDBSnmpx, Response);
                    }
                    else
                    {
                        pHDBSnmpx->RequestState = OHA_SNMPX_STATE_WF_STOP;
                    }
                    break;
                }
                /* started state and a User-START => no action */
                if (pRQBUser && (pRQBUser->args.SnmpxControl.Mode == OHA_CTRL_START_SNMPX))
                {
                    /* check for pending user-requests and confirm  */
                    OHA_SNMPXUserCheckPending(pHDBSnmpx, OHA_OK);
                    break;
                }

                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_FATAL,"OHA_SnmpxRequestHandler(): Invalid RState (%Xh)",pHDBSnmpx->RequestState);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SnmpxRequestHandler(), state %Xh", pHDBSnmpx->RequestState);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXGetHandleByNIC                     +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_SNMPX_PTR_TYPE                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a SNMPX HDB with NICId. If none is found  +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_SNMPX_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SNMPXGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_SNMPX_PTR_TYPE  pHDB   =    OHA_GET_HSNMPX_PTR();
    LSA_UINT32             i      = 0;
    LSA_BOOL               Found;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdxNic(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXGetHandleByNIC(NICId: 0x%X)",
                          NICId);

    LSA_UNUSED_ARG(NICId); /* prevent compiler warning for not accessing */

    /* we have only one SNMPX channel!  */
    {
        /* SNMPX-Handle independend of NICId! */
        if ( pHDB->SnmpxState == OHA_HANDLE_STATE_OPEN )
        {
            Found = LSA_TRUE;
        }
        else
        {
            Found = LSA_FALSE;
        }
    }

    if ( ! Found ) pHDB = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPXGetHandleByNIC(pHDB:  0x%X)", pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXControlAgent                       +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE     pRQB           +*/
/*+                        :    SNMPX_HDB_PTR_TYPE           pHDB           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      SNMPX_OPC_CONTROL_MASTER        +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.SockControl                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to activate/deactivate the SNMP-Agent by the user +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_SNMPXControlAgent(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_SNMPX_PTR_TYPE   pHDBSnmpx)
{
    LSA_UINT16                    Response = OHA_OK;
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQBSnmpx;
    LSA_USER_ID_TYPE              UserId;
    LSA_UINT8                     SnmpxControlMode = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXControlAgent(pRQB: 0x%X, pHDBSnmpx: 0x%X)",
                          pRQB, pHDBSnmpx);

    UserId.uvar32 = 0;

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBSnmpx,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"Request SNMPX_OPC_CONTROL_MASTER failed. No Lower Channel for SNMPX");
    }
    else
    {
        OHA_UPPER_SNMPX_CONTROL_PTR_TYPE control = &pRQB->args.SnmpxControl;

        switch ( control->Mode )
        {
            case OHA_CTRL_START_SNMPX:
                SnmpxControlMode = SNMPX_CTRL_START;
                break;
            case OHA_CTRL_STOP_SNMPX:
                SnmpxControlMode = SNMPX_CTRL_STOP;
                break;
            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-Control failed.[0x%X]",OHA_ERR_SYNTAX);
                Response = OHA_ERR_SYNTAX;
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                                      "OUT: OHA_SNMPXControlAgent(Request finished)");
                return(Response);
        }
    }

    /* allocate RQB */
    OHA_SNMPX_ALLOC_LOWER_RQB(&pRQBSnmpx,UserId,sizeof(SNMPX_RQB_TYPE),pHDBSnmpx->Params.pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBSnmpx, LSA_NULL) )
    {
        OHA_SNMPX_RQB_SET_OPCODE(pRQBSnmpx,SNMPX_OPC_CONTROL_MASTER);
        OHA_SNMPX_RQB_SET_HANDLE(pRQBSnmpx,pHDBSnmpx->SnmpxHandle);

        pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SNMPX-Request: SNMPX_OPC_CONTROL_MASTER (Handle: 0x%X,SNMPXHandle: 0x%X,pRQB: 0x%X)",pHDBSnmpx->ThisHandle,pHDBSnmpx->SnmpxHandle,pRQB);

        pRQBSnmpx->args.Control.Mode = SnmpxControlMode;

        OHA_SNMPX_REQUEST_LOWER(pRQBSnmpx,pHDBSnmpx->Params.pSys);
    }
    else
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-Allocating lower RQB-memory failed!");
    }


    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdxNic(pHDBSnmpx->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SNMPXControlAgent(Request finished)");
    return(Response);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPXControlAgentDone                   +*/
/*+  Input                      OHA_SNMPX_LOWER_RQB_PTR_TYPE pRQB           +*/
/*+                             OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Register Agent for the LLDP-MIB done handling             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXControlAgentDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB)
{
    LSA_UINT16                      Response;
    LSA_UINT16                      RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPXControlAgentDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SNMPX_OPC_CONTROL_MASTER done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SNMPX_RQB_GET_RESPONSE(pRQB));

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    Response  = OHA_OK;

    switch (OHA_SNMPX_RQB_GET_RESPONSE(pRQB))
    {
        case SNMPX_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            OHA_ASSERT (SNMPX_OPC_CONTROL_MASTER == OHA_SNMPX_RQB_GET_OPCODE(pRQB));
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdxNic(pHDB->Params.pDetail->Params.Snmpx.NICId), LSA_TRACE_LEVEL_UNEXP,"SNMPX-ERROR: SNMPX_OPC_CONTROL_MASTER failed.(0x%X).",OHA_SNMPX_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_SNMPX_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SNMPXRequestHandler(pHDB, LSA_NULL, Response); /* further actions taken in request handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPXControlAgentDone(Response: 0x%X)", Response);
}
#endif

#endif    /* OHA_CFG_USE_SNMPX */

/*****************************************************************************/
/*  end of file OHA_SNMPX.C                                                  */
/*****************************************************************************/

