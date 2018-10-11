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
/*  F i l e               &F: oha_sock.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA SOCK-LowerLayer interface functions          */
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

#define LTRC_ACT_MODUL_ID  4
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_SOCK */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles */
#include "oha_int.h"            /* internal header */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKDoMib2ParamsInd(
    LSA_UINT16                   NicID,
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB);

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
/*+  Functionname          :    oha_sock_request_lower_done                 +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for SOCK-requests.                       +*/
/*+               Fills Error-Structure in NDB with SOCK-Response           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_SOCK_LOWER_IN_FCT_ATTR oha_sock_request_lower_done(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_SOCK_PTR_TYPE   pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN :oha_sock_request_lower_done(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /* SOCK-request always done from a SOCK-Channel. !           */

    pHDB = OHAGetSockHDBFromHandle(OHA_SOCK_RQB_GET_HANDLE(pRQB));

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
    }

    switch (OHA_SOCK_RQB_GET_OPCODE(pRQB) )
    {
        case SOCK_OPC_OPEN_CHANNEL:
            OHA_SOCKOpenChannelDone(pRQB,pHDB);
            break;
        case SOCK_OPC_CLOSE_CHANNEL:
            OHA_SOCKCloseChannelDone(pRQB,pHDB);
            break;
            /* case SOCK_OPC_UDP_OPEN:    */
        case SOCK_OPC_SNMP_AGENT_ACTIVATE:
            OHA_SOCKOpenPortDone(pRQB,pHDB);
            break;
            /* case SOCK_OPC_CLOSE:       */
        case SOCK_OPC_SNMP_AGENT_DEACTIVATE:
            OHA_SOCKClosePortDone(pRQB,pHDB);
            break;
            /* case SOCK_OPC_UDP_RECEIVE: */
        case SOCK_OPC_SNMP_RECEIVE_IND:
        case SOCK_OPC_SNMP_RECEIVE_PROVIDE:
            OHA_SOCKRecvDone(pRQB,pHDB);
            break;
        case SOCK_OPC_SET_ADDR_INFO:
            OHA_SOCKSetIpAddressDone(pRQB,pHDB);
            break;
        case SOCK_OPC_SET_MIB2_SYSGROUP_DATA:
            OHA_SOCKSetMib2ParamsDone(pRQB,pHDB);
			break;
        case SOCK_OPC_SET_MIB2_IFGROUP_DATA:
            OHA_SOCKSetMib2StatusDone(pRQB,pHDB);
            break;
        case SOCK_OPC_MIB2_SYSGROUP_DATA_IND:
            OHA_SOCKMib2ParamsIndDone(pRQB,pHDB);
            break;
        case SOCK_OPC_CNTRL_TIMEPARAMS:
			OHA_RequestControlTcipDataDone(pRQB,pHDB);
            break;
        case SOCK_OPC_GET_MIB2_IFINDEX:
			OHA_SOCKGetMib2StatusDone(pRQB,pHDB);
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_sock_request_lower_done()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKOpenChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA SOCK-Channel handle                          +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to SOCK. The Request will be  +*/
/*+               finished by calling the SOCK-Callbackfunction.            +*/
/*+               oha_sock_request_lower_done() if OHA_OK.                  +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);
    Result = OHA_OK;

    UserId.uvar32 = 0;
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_OPEN_CHANNEL);

        pRQB->args.channel.handle_upper  = OHAHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.sock_request_upper_done_ptr = oha_sock_request_lower_done;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
        OHA_SOCK_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKOpenChannel. Result: 0x%X",
                          Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKOpenChannelDone                     +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE   pRQB          +*/
/*+                             OHA_HDB_SOCK_PTR_TYPE         pHDB          +*/
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
/*+               - save SOCK-Channel                                       +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKOpenChannelDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16       Response;
    LSA_UINT16       RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    Response = OHA_OK;

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    if ( OHA_SOCK_RQB_GET_RESPONSE(pRQB) == SOCK_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Open Channel successful");

        /* save SOCK-Channel in HDB */
        pHDB->SockHandle = pRQB->args.channel.handle;
    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-Open Channel failed.[0x%X]",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    if (Response == OHA_OK)
    {
        Response = OHA_SOCKStartUp(pHDB);
        if (Response == OHA_OK)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKOpenChannelDone(Response: 0x%X)", Response);
            return;
        }
        else
        {
            Response = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
        }
    }

    OHA_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKOpenChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKCloseChannel                        +*/
/*+  Input                      LSA_HANDLE_TYPE       SOCKHandle            +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SOCKHandle            : SOCK handle to close                           +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to SOCK. The Request will be +*/
/*+               finished by calling the SOCK-Callbackfunction.            +*/
/*+               oha_sock_request_lower_done()                             +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKCloseChannel(
    LSA_HANDLE_TYPE       SOCKHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB  = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKCloseChannel(SOCKHandle: 0x%X)",
                          SOCKHandle);

    UserId.uvar32 = 0;
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_CLOSE_CHANNEL);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,SOCKHandle);

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_CLOSE_CHANNEL (SOCKHandle: 0x%X,pRQB: 0x%X)",SOCKHandle,pRQB);
        OHA_SOCK_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKCloseChannel(Result: 0x%X)", Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKCloseChannelDone                    +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_HDB_SOCK_PTR_TYPE        pHDB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKCloseChannelDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16        Response = OHA_OK;
    LSA_UINT16        RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKCloseChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    if ( OHA_SOCK_RQB_GET_RESPONSE(pRQB) == SOCK_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Close Channel successful");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SOCK-Close Channel failed.[0x%X]",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    /* if there is a stored receive-rqb */
	OHA_SOCKFreeAgent(&pHDB->LldpAgent, pHDB);
	OHA_SOCKFreeAgent(&pHDB->MrpAgent, pHDB);

    /* Could all allocated Receive-RQBs be freed ? */
    OHA_ASSERT(pHDB->LldpAgent.RxPendingCnt == 0);
    OHA_ASSERT(pHDB->MrpAgent.RxPendingCnt == 0);

    /* have we got all LLDP_OPC_STATS statistic request from LLDP ? */
    OHA_ASSERT (pHDB->LldpAgent.StatsPendCnt == 0);
    /* have we got all MRP_OPC_STATUS statistic request from MRP ? */
    OHA_ASSERT (pHDB->MrpAgent.StatsPendCnt == 0);

    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKCloseChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKChannelDone                         +*/
/*+  Input                      OHA_HDB_SOCK_PTR_TYPE         pHDB          +*/
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
/*+               SOCK-Channels. This function is called after a lower      +*/
/*+               layer called back the OHA (e.g. after SOCK finished his   +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_SOCKChannelDone(
    OHA_HDB_SOCK_PTR_TYPE       pHDB,
    LSA_UINT16                 Response)
{

    OHA_UPPER_RQB_PTR_TYPE       pRQB;
    LSA_UINT16                   PathStat = LSA_RET_OK;
    LSA_BOOL                     Finish   = LSA_FALSE; /* dont finish request by default */
    LSA_BOOL                     Release  = LSA_FALSE; /* dont release PATH-Info by default */
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE             pSys;
	OHA_DETAIL_PTR_TYPE          pDetail;
	LSA_UINT16                   NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB,Response);

    OHA_ASSERT_NULL_PTR(pHDB);

    pRQB    = pHDB->pOCReq;
    Cbf     = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
	pSys    = pHDB->Params.pSys;
	pDetail = pHDB->Params.pDetail;
	NICId   = pHDB->Params.pDetail->Params.Sock.NICId;

    OHA_ASSERT_NULL_PTR(pRQB);
	
    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SOCKChannelDone done (NICId: 0x%X,Rsp: 0x%X,State: 0x%X)",pHDB->Params.pDetail->Params.Sock.NICId,Response,pHDB->State);

    switch (pHDB->State)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Open Channel request.                        */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_SOCK_RUNNING:
            if ( Response == OHA_OK)
            {
				/* set the global MIBII params first */
				/* The Portstrings (port_id = 1...n) and the string for the IF (port_id = 0) is supported too! */ 
				Response = OHA_SOCKSetMib2Params(NICId, OHA_HANDLE_TYPE_SOCK);
			}

            if ( Response == OHA_OK)
            {
				pHDB->State = OHA_HANDLE_STATE_SETMIB2_SOCK_RUNNING;
			}
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open SOCK. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;

        case OHA_HANDLE_STATE_SETMIB2_SOCK_RUNNING:
#ifndef OHA_SOCK_SNMP_OFF
            if ( Response == OHA_OK)
            {
				Response = OHA_SOCKOpenPort(pHDB, OHA_LLDP_MIB_NODE); /* Activate the LLDP-Agent   */
				if ( Response != OHA_OK)
				{
					OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK AgentActivate-Request failed.");
				}
			}

            if ( Response == OHA_OK)
            {
				pHDB->State = OHA_HANDLE_STATE_OPEN_PORT_RUNNING;
			}
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open SOCK. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;
#endif

            /*-------------------------------------------------------------------*/
            /* Just continue a SOCK-Open Channel request.                        */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_PORT_RUNNING:
            if ( Response == OHA_OK)
            {
                /* provide the MIB2-Indication for handling writeable MIB2 parameters */
                Response = OHA_SOCKMib2ParamsInd(NICId, pHDB->ThisHandle);

				if (OHA_EDDCheckNICUsed(NICId))     /* EDD System channel is opened */
				{
					(LSA_UINT16)oha_db_update_rema_ip_address(NICId);	/* AP00909635 */

#ifndef OHA_CFG_NO_REMA
					(LSA_VOID)oha_rema_station_update(NICId);
					(LSA_VOID)oha_rema_dhcp_update(NICId);
#endif
				}
			}

            if ( Response == OHA_OK)
            {
                /* no more actions needed. Channel is open now. */
                pHDB->State  = OHA_HANDLE_STATE_OPEN;
                Finish = LSA_TRUE;

#ifndef OHA_CFG_NO_REMA
                if ( Response == OHA_OK )
                {
					(LSA_VOID)oha_rema_snmp_update(NICId); /* global NIC */
				}
#endif

#if 0
                /* Open the Port 161 for SNMP                       */
                pHDB->State  = OHA_HANDLE_STATE_OPEN_PORT_RUNNING;

                Response = OHA_SOCKOpenPort(pHDB);

                if ( Response != OHA_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing SockOpenPort-Request failed.");

                    pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Closing Channel because of error.");

                    if ( OHA_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        OHA_FATAL(OHA_FATAL_ERR_SOCK);
                    }
                }
#endif
            }
            else
            {
                /* on error, we copy the LL-Error to the RQB    */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open SOCK. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a OPEN-POR SOCK-request.                            */
            /*-------------------------------------------------------------------*/
#if 0
        case OHA_HANDLE_STATE_OPEN_PORT_RUNNING:
            if ( Response == OHA_OK)
            {
                /* NOTE: Providing Recv-Indication Request must be          */
                /*       the last action before going into State "open".    */
                /*       So only Indciations while in Open-state must be    */
                /*       handled and reprovided!                            */

                /* provide SOCK-Recv-Requests */
                Response = OHA_SOCKRecvProvide(pHDB, OHA_SNMP_NUM_OF_RECV);
            }

                if ( Response == OHA_OK )
                {
                    /* no more actions needed. Channel is open now. */
                    pHDB->State  = OHA_HANDLE_STATE_OPEN;
                    pHDB->RequestState = OHA_SOCK_STATE_CLOSE; /* initialize */
                    Finish = LSA_TRUE;
                }
                else
                {
                    /* Any Recv-Request failed. Cancel Recv-Requests (by CloseChannel) */
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SOCK Receive Provide request failed. Response: 0x%X", Response);

                    pHDB->OpenResponse = Response; /* save for later */

                    pHDB->State = OHA_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Closing Channel because of error");

                    if ( OHA_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel().");
                        OHA_FATAL(OHA_FATAL_ERR_SOCK);
                    }
                }
            break;
#endif
            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Close-Channel because of error within Open   */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING:
            if ( Response == OHA_OK)
            {
                Response = pHDB->OpenResponse;  /* get Error-Response  */

                /* release PATH-Info ! */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel().");
                OHA_FATAL(OHA_FATAL_ERR_SOCK);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Close-Channel request                        */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_SOCK_RUNNING:
        case OHA_HANDLE_STATE_SETIP_SOCK_RUNNING:
            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel(). Response: 0x%X", Response);
                /* is this a fatal error ?                      */
            }
            Finish = LSA_TRUE;
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a CLOSE-PORT SOCK-request.                          */
            /*-------------------------------------------------------------------*/
#if 0
        case OHA_HANDLE_STATE_CLOSE_PORT_RUNNING:

            if ( Response == OHA_OK)
            {
                Response = OHA_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys);
                if ( Response == OHA_OK )
                {
                    /* Request is underway now.. */
                    pHDB->State  = OHA_HANDLE_STATE_CLOSE_SOCK_RUNNING;
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCKET(). Response: 0x%X", Response);
                /* is this a fatal error ?                      */
            }
            break;
#endif
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
        OHA_SOCKReleaseHDB(pHDB);
	}

	if ( Finish )  /* finish the request ? */
    {
        pHDB->pOCReq = LSA_NULL;  /* to signal no more request pending.      */

        OHA_RQBSetResponse(pRQB,Response);
        /* upper - handler already set */
        OHA_UPPER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Response);
        OHA_CallCbf(Cbf,pRQB,pSys);
    }

    if ( Release )  /* release the PATH-Info ? */
    {
        OHA_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);

        if (PathStat != LSA_RET_OK ) OHA_FATAL(OHA_FATAL_ERR_RELEASE_PATH_INFO);
	}

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKChannelDone()");
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKRecvProvide                         +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                             LSA_UINT32                   Cnt            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Cnt                  : Number of Recv-Request to send to SOCK          +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends Indication-Resources to SOCK                        +*/
/*+               (SOCK_OPC_SNMP_RECEIVE_REQ)                               +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Requests                      +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe RECV-Request pending     +*/
/*+                     within SOCK!                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKRecvProvide(
    OHA_HDB_SOCK_PTR_TYPE    pHDB,
	OHA_OID                  MibNode,
    LSA_UINT32               Cnt)
{
    OHA_SOCK_LOWER_RQB_PTR_TYPE                 pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                            pSys;
    LSA_UINT16                                  Status;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal = LSA_RET_OK;
    OHA_SOCK_LOWER_MEM_PTR_TYPE                 pRcv = LSA_NULL;  /* Pointer for Frame memory. */
    /* OHA_SOCK_UDP_DATA_PTR_TYPE               udp_recv; */
    OHA_SOCK_SNMP_RECEIVE_PTR_TYPE              snmp_recv;
	OHA_HDB_AGNT_PTR_TYPE                       pSubAgnt;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKRecvProvide(pHDB: 0x%X, MibNode: 0x%X, Cnt: 0x%X)",
                          pHDB,MibNode,Cnt);

    OHA_ASSERT_NULL_PTR(pHDB);

	OHA_ASSERT(MibNode == OHA_LLDP_MIB_NODE || MibNode == OHA_MRP_MIB_NODE);
	pSubAgnt = (MibNode == OHA_LLDP_MIB_NODE) ? &pHDB->LldpAgent : &pHDB->MrpAgent;

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Receive-Request to SOCK                                 */
    /*---------------------------------------------------------------------------*/

    pSubAgnt->SnmpRQBCnt = 0; /* no Rx pending within SOCK */

    while ((Status == OHA_OK) && (Cnt))
    {
        OHA_SOCK_ALLOC_LOWER_MEM(&pRcv,
                                 UserId,
                                 OHA_UDP_BUFFER_SIZE,
                                 pSys);

        if ( LSA_HOST_PTR_ARE_EQUAL(pRcv, LSA_NULL) )
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SOCKRecvProvide(). Allocation receive memory failed.");
        }
        else
        {
            /* allocate RQB */
            OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
            {
				LSA_UINT16 Orginator = (MibNode == OHA_LLDP_MIB_NODE) ? (LSA_UINT16)OHA_HANDLE_TYPE_LLDP : (LSA_UINT16)OHA_HANDLE_TYPE_MRP;

                /* provide recv resources */
                /* OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_UDP_RECEIVE); */
                OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SNMP_RECEIVE_PROVIDE);
                OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
				OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

                OHA_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SNMP_RECEIVE_REQ (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

                /* udp_recv = &pRQB->args.data;  */
                /* OHA_ASSERT (udp_recv);        */
                snmp_recv = &pRQB->args.snmp_recv;
                OHA_ASSERT (snmp_recv);

                /* udp_recv->sock_fd = pHDB->SockFD; */
                snmp_recv->snmp_handle = pSubAgnt->SnmpHandle;
                /* udp_recv->data_length = OHA_UDP_BUFFER_SIZE; */
                /* udp_recv->buffer_length = OHA_UDP_BUFFER_SIZE; */
                snmp_recv->data_length = OHA_UDP_BUFFER_SIZE; /* sock need this !! */
                snmp_recv->buffer_size = OHA_UDP_BUFFER_SIZE; /* sock need this !! */

                snmp_recv->snmp_done   = LSA_FALSE;   /* SNMP call was relevant for this agent ? */
                /* udp_recv->buffer_ptr = pRcv;  */
                snmp_recv->buffer = pRcv;

                OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
                pSubAgnt->SnmpRQBCnt++;  /* number of RQBs pending within SOCK */
                pSubAgnt->RxPendingCnt++; /* number of Rx-Request pending within SOCK */
            }
            else
            {
                Status = OHA_ERR_RESOURCE;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
                OHA_SOCK_FREE_LOWER_MEM(&RetVal,pRcv,pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        Cnt--;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKRecvProvide(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKRecvDone                            +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_SOCK_HDB_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: SOCK Recv-Request done (Indication received)              +*/
/*+                                                                         +*/
/*+               Copy received frame infos and content to the DataBase     +*/
/*+               within global management (Frame) for further handling.    +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides Receveive-Request to SOCK                      +*/
/*+                                                                         +*/
/*+               If Recv-Request was canceled we free the RQB/Frame and    +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKRecvDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16                                  RetVal    = OHA_OK;
    LSA_BOOL                                    Reprovide = LSA_TRUE;
    /* OHA_SOCK_UDP_DATA_PTR_TYPE               udp_recv; */
    OHA_SOCK_SNMP_RECEIVE_PTR_TYPE              snmp_recv;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  Orginator;
	OHA_HDB_AGNT_PTR_TYPE                       pSubAgnt;
	OHA_OID										MibNode;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCK_RecvDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    UserId.uvar32 = 0;
    OHA_ASSERT_NULL_PTR(pRQB);

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);
	OHA_ASSERT(Orginator == OHA_HANDLE_TYPE_LLDP || Orginator == OHA_HANDLE_TYPE_MRP);
	pSubAgnt = (Orginator == OHA_HANDLE_TYPE_LLDP) ? &pHDB->LldpAgent : &pHDB->MrpAgent;
	MibNode  = (Orginator == OHA_HANDLE_TYPE_LLDP) ? OHA_LLDP_MIB_NODE : OHA_MRP_MIB_NODE;
	
    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_SNMP_RECEIVE_REQ done (Handle: 0x%X,Rsp: 0x%X,State: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB),pHDB->State);

    /* udp_recv = &pRQB->args.data;   */
    snmp_recv = &pRQB->args.snmp_recv;

    snmp_recv->snmp_done = LSA_FALSE;       /* init. */

    /* OHA_ASSERT_NULL_PTR(udp_recv);  */
    /* OHA_ASSERT_NULL_PTR(udp_recv->buffer_ptr); */ /* received data */
    OHA_ASSERT_NULL_PTR(snmp_recv);
    OHA_ASSERT_NULL_PTR(snmp_recv->buffer);  /* received data */

    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the frame and dont reprovide it. This is the case if we are    */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid frame while closing, so CANCEL   */
    /*       may not be enough to catch all rqbs. We have to check the State too */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Recv-Request will be provided in the last */
    /*       State while opening the channel.                                    */

    if ( pHDB->State == OHA_HANDLE_STATE_OPEN && pSubAgnt->RequestState == OHA_SNMP_STATE_START)
    {
        switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
        {
            case SOCK_RSP_OK:
            {
                OHA_SOCK_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL;  /* Pointer to SNMP-Frame memory. */
                /* OHA_SOCK_LOWER_RQB_PTR_TYPE  pResRQB; */
                LSA_INT len = 0;

				if(OHA_USERIsAgentActive((LSA_HANDLE_TYPE)Orginator, LSA_NULL))
				{
				    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: An SNMP User.req is active, could not respond receive (Handle: 0x%X,MibNode: 0x%X)",pHDB->ThisHandle,MibNode);
					break;
				}
                /* ---------------------------------------------*/
                /* Successfully done the request.               */
                /* ---------------------------------------------*/
                /* parse framedata for LLDP-MIB */

                /* alloc the buffer and attach them to the RQB */
                OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_UDP_BUFFER_SIZE, pHDB->Params.pSys);
                /* got the Address-buffer synchronously...  */
                if (!LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
                {
                    LSA_BOOL oha_oid = LSA_FALSE;   /* is it an OID for the OHA-agent ? */
                    len = oha_snmp_agt_parse(MibNode, (LSA_UINT8*)snmp_recv->buffer, snmp_recv->data_length, (LSA_UINT8*)MemPtr, OHA_SNMP_SIZE, &oha_oid);
                    if (len
                        && oha_oid      /* it's an OID for OHA, oha_snmp_agt_parse() has made a dummy-response in snmp_recv->recv_buffer */
                              /* send this packet for responding the remote SNMP manager */
                       )      /* parse worked, send response packet */
                    {
                        /* it's a LLDP-OID, OHA can respond the SNMP-manager */
                        /* OHA_SOCK_FREE_LOWER_MEM(&RetVal, snmp_recv->recv_buffer, pHDB->Params.pSys); */
                        /* OHA_MEM_FREE_FAIL_CHECK(RetVal);  */

                        /* change send buffers  */
                        /* snmp_recv->recv_buffer = MemPtr;  */
                        /* snmp_recv->data_length = len;     */

                        snmp_recv->snmp_done = LSA_TRUE;
                    }
                    else   /* no LLDP-OID */
                    {
                        OHA_SOCK_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
                        OHA_MEM_FREE_FAIL_CHECK(RetVal);
                    }
                }
				else
				{
				    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN,">>>: Could not allocate receive buffer for SNMP (Handle: 0x%X,MibNode: 0x%X)",pHDB->ThisHandle,MibNode);
				}

                /* it's a LLDP-OID, OHA can respond the SNMP-manager */
                if (snmp_recv->snmp_done)
                {
                    /* Receive-RQB is the same for response ! */
                    LSA_UINT16  CountOfStats   = 0;
                    LSA_UINT16  CountOfQueries = 0;
					LSA_UINT8 MsgType = oha_snmp_msg_parse((LSA_UINT8*)snmp_recv->buffer, snmp_recv->data_length);

                    /* statistic data (asynchronous) from LLDP_GET_STATS required ? */
					if(Orginator == OHA_HANDLE_TYPE_LLDP)
					{
						if(MsgType == OHA_SNMP_SET_REQ)
						{
							RetVal = OHA_LLDPPrepareStats(pHDB->ThisHandle, &CountOfQueries, LSA_TRUE);
						}
						if (RetVal != OHA_OK) /* no user ind resource(s) for query */
						{
							OHA_ASSERT(CountOfQueries == 0);
	                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "OUT: OHA_SOCKRecvDone(RetVal 0x%X): User query running or cancelled.", RetVal);
							/* create the get response(-) packet for the set operation */
							oha_snmp_create_ident((LSA_UINT8*)snmp_recv->buffer, 
												  (LSA_UINT8*)MemPtr, 
												  (LSA_UINT)snmp_recv->data_length,
												  (RetVal==OHA_ERR_PARAM) ? OHA_SNMP_ERR_BADVALUE : OHA_SNMP_ERR_NOSUCHNAME);
						}
						else
						{
							if (CountOfQueries == 0) /* no user queries, continue */ 
							{
								CountOfStats = OHA_LLDPDoStats(pHDB->ThisHandle, MsgType, LSA_TRUE);	/* AP00817094 */
							}
						}
					}
#ifndef OHA_CFG_NO_MRP
#ifndef OHA_CFG_NO_MRP_MIB
					else if(Orginator == OHA_HANDLE_TYPE_MRP)
					{
						CountOfStats = OHA_MRPCountStats(pHDB->ThisHandle, MsgType);
					}
#endif
#endif
					OHA_ASSERT(CountOfQueries == 0 || CountOfStats == 0);
                    if (pSubAgnt->StatsPendCnt != 0) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* request is not queued */

                    if (CountOfStats) /* LLDP requests necessary for getting statistics */ 
                    {
						/* store the origin snmp response packet (Snmp_Get or Snmp_GetNext only) and pass again later */
                        pSubAgnt->RequestState = OHA_SNMP_STATE_WF_STATISTICS;  /* get Statistics from LLDP   */
						pSubAgnt->StatsPendCnt = CountOfStats;  /* pending LLDP_OPC_STATS */
					}
					if(CountOfQueries)	/* snmp set query */
					{
						pSubAgnt->RequestState = OHA_SNMP_STATE_WF_USER;  /* query the user(s) for 'Set'/'NoSet' */
					}

					/* build the response packet */
                    if (CountOfStats == 0 || MsgType == OHA_SNMP_SET_REQ)
					{
						/* free response buffer first */
						OHA_SOCK_FREE_LOWER_MEM(&RetVal, snmp_recv->buffer, pHDB->Params.pSys);
						OHA_MEM_FREE_FAIL_CHECK(RetVal);

						/* change send buffers and respond the Ind-Rqb */
						snmp_recv->data_length = (LSA_UINT16)len;
						snmp_recv->buffer      = MemPtr;
						MemPtr                 = LSA_NULL;
					}

					if(OHA_IS_NOT_NULL(MemPtr))
					{
                        OHA_SOCK_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
                        OHA_MEM_FREE_FAIL_CHECK(RetVal);
					}

					if(CountOfQueries || CountOfStats)	/* snmp set query or get statistics */
					{
                        /* SOCK-RCV-Blocks pending ? */
                        if (OHA_IS_NOT_NULL(pSubAgnt->SnmpRQBs.pBottom))    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

                        /* store the rqb for response */
                        OHA_RQB_PUT_BLOCK_TOP(pSubAgnt->SnmpRQBs.pBottom,
                                              pSubAgnt->SnmpRQBs.pTop,
                                              pRQB);

                        pSubAgnt->SnmpRQBCnt--;  /* number of RQBs pending within SOCK */
                        pSubAgnt->RxPendingCnt--; /* number of Recv-Request pending within SOCK */

                        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKRecvDone()");
                        return;
                    }
                }
            }
            break;

            case SOCK_RSP_OK_WITHDRAW:
            case SOCK_RSP_ERR_LOC_ABORT:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> OHA_HANDLE_STATE_OPEN  */
                /*       while closing!                             */

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"SOCK Receive canceled (0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_FATAL,"SOCK_OPC_UDP_RECEIVE failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
                break;
        }
    }
    else if ( pHDB->State == OHA_HANDLE_STATE_OPEN && 
		      (pSubAgnt->RequestState == OHA_SNMP_STATE_WF_STATISTICS || /* AP01343355: warning: suggest parentheses around && within || */
			   pSubAgnt->RequestState == OHA_SNMP_STATE_WF_USER) )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"SOCK Receive while in wait for statistics-state. ignore and reprovide.");
        Reprovide = LSA_TRUE;   /* ignore */
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"SOCK Receive while not in open-state. discard and dont reprovide.");
        Reprovide = LSA_FALSE;
    }

    if ( Reprovide )
    {
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_UDP_RECEIVE (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);
        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SNMP_RECEIVE_RSP);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle); /* set handle */
        OHA_SOCK_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        pSubAgnt->SnmpRQBCnt--;  /* number of RQBs pending within SOCK */
        pSubAgnt->RxPendingCnt--; /* number of Recv-Request pending within SOCK */

        OHA_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.snmp_recv.buffer,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);

        OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKRecvDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKOpenPort                            +*/
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
/*+  Description: Open port 161 for SNMP                                    +*/
/*+               (SOCK_OPC_SNMP_AGENT_ACTIVATE)                            +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Requests                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKOpenPort(
    OHA_HDB_SOCK_PTR_TYPE    pHDB,
	OHA_OID                  MibNode)
{
    OHA_SOCK_LOWER_RQB_PTR_TYPE             pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    /* OHA_SOCK_UDP_OPEN_PTR_TYPE           sock_open; */
    OHA_SOCK_SNMP_ACTIVATE_PTR_TYPE         snmp_act;
	OHA_HDB_AGNT_PTR_TYPE                   pSubAgnt;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKOpenPort(pHDB: 0x%X, MibNode: 0x%X)",
                          pHDB, MibNode);

    OHA_ASSERT_NULL_PTR(pHDB);

	OHA_ASSERT(MibNode == OHA_LLDP_MIB_NODE || MibNode == OHA_MRP_MIB_NODE);
	pSubAgnt = (MibNode == OHA_LLDP_MIB_NODE) ? &pHDB->LldpAgent : &pHDB->MrpAgent;

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
		LSA_UINT16 Orginator = (MibNode == OHA_LLDP_MIB_NODE) ? (LSA_UINT16)OHA_HANDLE_TYPE_LLDP : (LSA_UINT16)OHA_HANDLE_TYPE_MRP;

        /* OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_UDP_OPEN); */
        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SNMP_AGENT_ACTIVATE);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SNMP_AGENT_ACTIVATE (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

        /* sock_open = &pRQB->args.udp_open;              */
        /* sock_open->loc_addr.sin_family = SOCK_AF_INET; */
        /* sock_open->loc_addr.sin_addr.S_un.S_addr = 0;  */
        /* sock_open->loc_addr.sin_port = OHA_SWAP16(OHA_SNMP_PORT); */
        /* sock_open->timer_on = LSA_FALSE;               */
        snmp_act = &pRQB->args.snmp_act;
        snmp_act->type = SOCK_EXTERNER_SNMP_AGENT;

        pSubAgnt->SnmpRQBCnt++;
        OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKOpenPort(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKOpenPortDone                        +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Open Port 161 done handling                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKOpenPortDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16                      Response;
    LSA_UINT16                      RetVal   = LSA_RET_OK;
    /* OHA_SOCK_UDP_OPEN_PTR_TYPE   sock_open; */
    OHA_SOCK_SNMP_ACTIVATE_PTR_TYPE snmp_act;
	OHA_HDB_AGNT_PTR_TYPE           pSubAgnt;
    LSA_UINT16                      Orginator;
    OHA_OID                         MibNode;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKOpenPortDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_SNMP_AGENT_ACTIVATE done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);
	OHA_ASSERT(Orginator == OHA_HANDLE_TYPE_LLDP || Orginator == OHA_HANDLE_TYPE_MRP);
	pSubAgnt = (Orginator == OHA_HANDLE_TYPE_LLDP) ? &pHDB->LldpAgent : &pHDB->MrpAgent;
	MibNode = (Orginator == OHA_HANDLE_TYPE_LLDP) ? OHA_LLDP_MIB_NODE : OHA_MRP_MIB_NODE;

    switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            /* sock_open = &pRQB->args.udp_open; */
            /* OHA_ASSERT_NULL_PTR (sock_open);  */
            snmp_act = &pRQB->args.snmp_act;
            OHA_ASSERT_NULL_PTR (snmp_act);

            /* pHDB->SockFD = sock_open->sock_fd; */       /* lower-layer send sock  */
            /* pHDB->LocalPort = sock_open->loc_addr.sin_port; */   /* the used port */
            pSubAgnt->SnmpHandle = snmp_act->snmp_handle;      /* lower-layer send sock  */

            /* provide SOCK-Recv-Requests */
            Response = OHA_SOCKRecvProvide(pHDB, MibNode, OHA_SNMP_NUM_OF_RECV);
            if ( Response == OHA_OK )
            {
                pSubAgnt->RequestState = OHA_SNMP_STATE_START;
            }
            else
            {
                /* Any Recv-Request failed. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK Receive Provide request for Agent (0x%X) failed.", MibNode);
            }
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_UDP_RECEIVE failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    pSubAgnt->SnmpRQBCnt--;
    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

#ifndef OHA_CFG_NO_MRP_MIB
	if(Orginator == OHA_HANDLE_TYPE_LLDP)
	{
		Response = OHA_SOCKOpenPort(pHDB, OHA_MRP_MIB_NODE); /* Activate the LLDP-Agent   */
		if ( Response != OHA_OK)
		{
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK AgentActivate-Request failed.");
		}
		else
		{
            pHDB->MrpAgent.RequestState = OHA_SNMP_STATE_WF_START;
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKClosePortDone(Response: 0x%X)", Response);
			return;
		}
	}
#endif

    if (! LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL))
    {
        OHA_SOCKChannelDone(pHDB, Response); /* further actions taken in channel handler */
    }
    else
    {
	    OHA_SOCKRequestHandler(pHDB, MibNode, Response); /* further actions taken in request handler */
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKOpenPortDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKClosePort                           +*/
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
/*+  Description: Close port 161 for SNMP                                   +*/
/*+               (SOCK_OPC_CLOSE)                                          +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Close                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKClosePort(
    OHA_HDB_SOCK_PTR_TYPE    pHDB,
	OHA_OID                  MibNode)
{
    OHA_SOCK_LOWER_RQB_PTR_TYPE             pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
	OHA_HDB_AGNT_PTR_TYPE                   pSubAgnt;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKClosePort(pHDB: 0x%X, MibNode: 0x%X)",
                          pHDB,MibNode);

    OHA_ASSERT_NULL_PTR(pHDB);

	OHA_ASSERT(MibNode == OHA_LLDP_MIB_NODE || MibNode == OHA_MRP_MIB_NODE);
	pSubAgnt = (MibNode == OHA_LLDP_MIB_NODE) ? &pHDB->LldpAgent : &pHDB->MrpAgent;

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
		LSA_UINT16 Orginator = (MibNode == OHA_LLDP_MIB_NODE) ? (LSA_UINT16)OHA_HANDLE_TYPE_LLDP : (LSA_UINT16)OHA_HANDLE_TYPE_MRP;

        /* OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_CLOSE); */
        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SNMP_AGENT_DEACTIVATE);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SNMP_AGENT_DEACTIVATE (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

        /* pRQB->args.close.sock_fd = pHDB->SockFD; */
        pRQB->args.snmp_deact.snmp_handle = pSubAgnt->SnmpHandle;

        OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKClosePort(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKClosePortDone                       +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Close Port 161 done handling                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKClosePortDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16                      Response = OHA_OK;
    LSA_UINT16                      RetVal   = LSA_RET_OK;
	OHA_HDB_AGNT_PTR_TYPE           pSubAgnt;
    LSA_UINT16                      Orginator;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKClosePortDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_CLOSE done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);
	OHA_ASSERT(Orginator == OHA_HANDLE_TYPE_LLDP || Orginator == OHA_HANDLE_TYPE_MRP);
	pSubAgnt = (Orginator == OHA_HANDLE_TYPE_LLDP) ? &pHDB->LldpAgent : &pHDB->MrpAgent;

    switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
			pSubAgnt->SnmpHandle = SOCK_NO_HANDLE;
			OHA_SOCKFreeAgent(pSubAgnt, pHDB);
			pSubAgnt->RequestState = OHA_SNMP_STATE_STOP;
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_SNMP_AGENT_DEACTIVATE failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

	if(Orginator == OHA_HANDLE_TYPE_LLDP)
	{
		if (pHDB->MrpAgent.SnmpHandle != SOCK_NO_HANDLE)	/* is the MRP subagent to deactivate ? */
		{
			LSA_UINT16 Status = OHA_SOCKClosePort(pHDB, OHA_MRP_MIB_NODE);

			if(Status != OHA_OK)
			{
				OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK Close Port request for MRP.MIB failed.");
			}
			else
			{
				pHDB->MrpAgent.RequestState = OHA_SNMP_STATE_WF_STOP;
				OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKClosePortDone(Response: 0x%X)", Response);
				return;
			}
		}
	}

	if (! LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL))
	{
    (LSA_VOID)OHA_SOCKShutDown(pHDB);
	}
	else
	{
		OHA_OID MibNode = (Orginator == OHA_HANDLE_TYPE_LLDP) ? OHA_LLDP_MIB_NODE : OHA_MRP_MIB_NODE;
    /* OHA_SOCKChannelDone(pHDB,Response); */ /* further actions taken in channel handler */
        OHA_SOCKRequestHandler(pHDB, MibNode, Response); /* further actions taken in request handler */
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKClosePortDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_InitAddrInfo                            +*/
/*+                             OHA_HDB_SOCK_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId                  :   interface specific NIC                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Initializes the parameters for providing SOCK_OPC_SET_ADDR_INFO.       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_InitAddrInfo(
    LSA_UINT16        NICId)
{
    LSA_USER_ID_TYPE UserId;
    LSA_UINT16       Status = OHA_OK;
    OHA_DB_IF_PTR_TYPE pDB  = OHA_DB_GET_INTERFACE(NICId);
	OHA_HDB_EDD_PTR_TYPE pHDB = OHA_EDDGetHandleByNIC(NICId);
    OHA_SOCK_LOWER_MEM_PTR_TYPE pName = LSA_NULL;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_InitAddrInfo(NICId: 0x%X)", NICId);

    OHA_ASSERT_NULL_PTR(pDB);
    OHA_ASSERT_NULL_PTR(pHDB);
    UserId.uvar32 = 0;

    /* get the NoS params */
    OHA_SOCK_ALLOC_LOWER_MEM(&pName, UserId, OHA_MAX_STATION_NAME_LEN, pHDB->Params.pSys);
    if (OHA_IS_NULL(pName))
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower memory1 failed!");
    }
    else
    {
        OHA_COMMON_MEM_U8_PTR_TYPE NamePtr  = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr  = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pNetMask = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pGateWay = LSA_NULL;

        pHDB->SetAddrInfo.NoS.pName  = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)pName;
        pHDB->SetAddrInfo.NoS.Length = (LSA_UINT16)OHA_DB_READ_STATION_NAME(pHDB->Params.pDetail->Params.Sock.NICId, &NamePtr);

        if (pHDB->SetAddrInfo.NoS.Length && OHA_IS_NOT_NULL(NamePtr))    /* only check, if chassisname present */
        {
            OHA_ASSERT(pHDB->SetAddrInfo.NoS.Length<=OHA_MAX_STATION_NAME_LEN);

            if (pHDB->SetAddrInfo.NoS.Length)
            {
                OHA_MEMCPY(pHDB->SetAddrInfo.NoS.pName, NamePtr, pHDB->SetAddrInfo.NoS.Length);
            }
        }

        /* local storage of the provided IP parameters */
        pHDB->SetAddrInfo.InterfaceID      = pDB->InterfaceID;
        pHDB->SetAddrInfo.CurrentIPSuite.IpAddr.s_addr  = 0;
        pHDB->SetAddrInfo.CurrentIPSuite.NetMask.s_addr = 0;
        pHDB->SetAddrInfo.CurrentIPSuite.Gateway.s_addr = 0;
        pHDB->SetAddrInfo.CurrentIPSuiteIsEmergency      = LSA_FALSE;
        pHDB->SetAddrInfo.CurrentIPSuiteConflictDetected = LSA_FALSE;

        /* read initialized IP-Address, it's the expected IP! */
        (LSA_VOID)OHA_DB_READ_IP_SUITE(pHDB->Params.pDetail->Params.Sock.NICId, &pIpAddr, &pNetMask, &pGateWay);

        if (OHA_IS_NULL(pIpAddr))
        {
            pHDB->SetAddrInfo.ExpectedIPSuite.IpAddr.s_addr  = 0;
        }
        else
        {
            pHDB->SetAddrInfo.ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b1 = *pIpAddr;
            pHDB->SetAddrInfo.ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b2 = *(pIpAddr+1);
            pHDB->SetAddrInfo.ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b3 = *(pIpAddr+2);
            pHDB->SetAddrInfo.ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b4 = *(pIpAddr+3);
        }
        if (OHA_IS_NULL(pNetMask))
        {
            pHDB->SetAddrInfo.ExpectedIPSuite.NetMask.s_addr = 0;
        }
        else
        {
            pHDB->SetAddrInfo.ExpectedIPSuite.NetMask.S_un.S_un_b.s_b1 = *pNetMask;
            pHDB->SetAddrInfo.ExpectedIPSuite.NetMask.S_un.S_un_b.s_b2 = *(pNetMask+1);
            pHDB->SetAddrInfo.ExpectedIPSuite.NetMask.S_un.S_un_b.s_b3 = *(pNetMask+2);
            pHDB->SetAddrInfo.ExpectedIPSuite.NetMask.S_un.S_un_b.s_b4 = *(pNetMask+3);
        }
        if (OHA_IS_NULL(pGateWay))
        {
            pHDB->SetAddrInfo.ExpectedIPSuite.Gateway.s_addr = 0;
        }
        else
        {
            pHDB->SetAddrInfo.ExpectedIPSuite.Gateway.S_un.S_un_b.s_b1 = *pGateWay;
            pHDB->SetAddrInfo.ExpectedIPSuite.Gateway.S_un.S_un_b.s_b2 = *(pGateWay+1);
            pHDB->SetAddrInfo.ExpectedIPSuite.Gateway.S_un.S_un_b.s_b3 = *(pGateWay+2);
            pHDB->SetAddrInfo.ExpectedIPSuite.Gateway.S_un.S_un_b.s_b4 = *(pGateWay+3);
        }

        pHDB->SetAddrInfo.EmergencyIPSuite.IpAddr.s_addr  = 0;
        pHDB->SetAddrInfo.EmergencyIPSuite.NetMask.s_addr = 0;
        pHDB->SetAddrInfo.EmergencyIPSuite.Gateway.s_addr = 0;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_InitAddrInfo(Response: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPSockCheckPending                    +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if Sock-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPSockCheckPending(
    OHA_HDB_SOCK_PTR_TYPE   pHDBSock,
	OHA_OID                 MibNode,
    LSA_UINT16              Response)
{
    OHA_SOCK_LOWER_RQB_PTR_TYPE pRxRQB;
	OHA_HDB_AGNT_PTR_TYPE pSubAgnt;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPSockCheckPending(pHDBSock: 0x%X, MibNode: 0x%X, Response: 0x%X)",
                          pHDBSock, MibNode, Response);

	OHA_ASSERT(MibNode == OHA_LLDP_MIB_NODE || MibNode == OHA_MRP_MIB_NODE);
	pSubAgnt = (MibNode == OHA_LLDP_MIB_NODE) ? &pHDBSock->LldpAgent : &pHDBSock->MrpAgent;
	LSA_UNUSED_ARG(Response);

    OHA_ASSERT_NULL_PTR(pSubAgnt->SnmpRQBs.pBottom);

	OHA_RQB_REM_BLOCK_BOTTOM(pSubAgnt->SnmpRQBs.pBottom,
                             pSubAgnt->SnmpRQBs.pTop,
                             pRxRQB);

    /* queued RQB is a receive rqb */
    OHA_ASSERT((OHA_SOCK_RQB_GET_OPCODE(pRxRQB)==SOCK_OPC_SNMP_RECEIVE_PROVIDE)||
               (OHA_SOCK_RQB_GET_OPCODE(pRxRQB)==SOCK_OPC_SNMP_RECEIVE_IND)||
               (OHA_SOCK_RQB_GET_OPCODE(pRxRQB)==SOCK_OPC_SNMP_RECEIVE_RSP));

    /* a 2nd RQB is queued, why ? */
    if (OHA_IS_NOT_NULL(pSubAgnt->SnmpRQBs.pBottom)) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

    OHA_SOCKParseDone(pRxRQB, pHDBSock);

	/* is a user PRM_WRITE LLDP record queued ? */
	/* could happen, if a SOCK response was active during a PRM_WRITE LLDP record, see OHA_PrmWriteLLDP() */
	{
		OHA_HDB_USER_PTR_TYPE pHDBSnmpUser = OHA_USERIsAgentActive(OHA_HANDLE_TYPE_LLDP, LSA_NULL);

		if(OHA_IS_NOT_NULL(pHDBSnmpUser) && OHA_IS_NOT_NULL(pHDBSnmpUser->SnmpUserReq.pBottom))
		{
		    OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
			OHA_RQB_REM_BLOCK_BOTTOM(pHDBSnmpUser->SnmpUserReq.pBottom,
									 pHDBSnmpUser->SnmpUserReq.pTop,
									 pRQBUser);

			/* finally write the record */
			OHA_PrmWriteLLDP(pRQBUser, pHDBSnmpUser);
		}
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPSockCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKParseDone                           +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_SOCK_HDB_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: SOCK Recv-Request done (Indication parsed)                +*/
/*+                                                                         +*/
/*+               Copy received frame infos and content to the DataBase     +*/
/*+               within global management (Frame) for further handling.    +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides Receveive-Request to SOCK                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKParseDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRxRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    OHA_SOCK_SNMP_RECEIVE_PTR_TYPE  snmp_recv;
	OHA_HDB_AGNT_PTR_TYPE           pSubAgnt;
    LSA_UINT16                      Orginator;
	OHA_OID							MibNode;

	OHA_SOCK_LOWER_MEM_PTR_TYPE		MemPtr = LSA_NULL;  /* Pointer to SNMP-Frame memory. */
    LSA_UINT16						RetVal = LSA_RET_OK;
    LSA_USER_ID_TYPE				UserId;
	LSA_UINT8                       MsgType;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKParseDone(pRxRQB: 0x%X, pHDB: 0x%X)",
                          pRxRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRxRQB);

	OHA_ASSERT(pHDB->State == OHA_HANDLE_STATE_OPEN);

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRxRQB);
	OHA_ASSERT(Orginator == OHA_HANDLE_TYPE_LLDP || Orginator == OHA_HANDLE_TYPE_MRP);
	pSubAgnt = (Orginator == OHA_HANDLE_TYPE_LLDP) ? &pHDB->LldpAgent : &pHDB->MrpAgent;
	MibNode  = (Orginator == OHA_HANDLE_TYPE_LLDP) ? OHA_LLDP_MIB_NODE : OHA_MRP_MIB_NODE;

    snmp_recv = &pRxRQB->args.snmp_recv;
    UserId.uvar32 = 0;

    OHA_ASSERT_NULL_PTR(snmp_recv);
    OHA_ASSERT_NULL_PTR(snmp_recv->buffer);        /* received data */
	MsgType = oha_snmp_msg_parse((LSA_UINT8*)snmp_recv->buffer, snmp_recv->data_length);

    if (OHA_SOCK_RQB_GET_RESPONSE(pRxRQB)!=SOCK_RSP_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
	/* the response for Snmp_Set must have been built ! */ 
	if (MsgType == OHA_SNMP_SET_REQ) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

	/***** snmp-get, snmp-get-next ******/
	/* finally create the Snmp_Get response */
	if(MsgType == OHA_SNMP_GET_REQ || MsgType == OHA_SNMP_GETNEXT_REQ)
	{
		OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_UDP_BUFFER_SIZE, pHDB->Params.pSys);

		if (OHA_IS_NOT_NULL(MemPtr))
		{
			snmp_recv->data_length = (LSA_UINT16)oha_snmp_agt_parse(MibNode, 
																	(LSA_UINT8*)snmp_recv->buffer, snmp_recv->data_length,
																	(LSA_UINT8*)MemPtr, OHA_SNMP_SIZE, LSA_NULL);

			if (snmp_recv->data_length != 0) /* parse worked, send response packet */
			{
				/* free response buffer first */
				OHA_SOCK_FREE_LOWER_MEM(&RetVal, snmp_recv->buffer, pHDB->Params.pSys);
				OHA_MEM_FREE_FAIL_CHECK(RetVal);

				/* change send buffers and respond snmp with the Ind-Rqb */
				snmp_recv->buffer      = MemPtr;
			}
			else
			{
				OHA_SOCK_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
				OHA_MEM_FREE_FAIL_CHECK(RetVal);
			}
		}
		else
		{
			OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN,">>>: Could not allocate receive buffer for SNMP (Handle: 0x%X,MibNode: 0x%X)",pHDB->ThisHandle,MibNode);
			snmp_recv->data_length = 0;
		}
	}

	/****** snmp-set(-) ******/
	if (MibNode == OHA_LLDP_MIB_NODE && oha_snmp_get_lldp_statistics() == 0) /* negative user query (set) */ 
	{
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "OUT: OHA_SOCKParseDone(RetVal 0x%X): User query negative or cancelled.", RetVal);

		OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_UDP_BUFFER_SIZE, pHDB->Params.pSys);
		if (OHA_IS_NOT_NULL(MemPtr))
		{
			/* create the get response(-) packet for the set operation */
			oha_snmp_create_ident((LSA_UINT8*)snmp_recv->buffer, 
								  (LSA_UINT8*)MemPtr, 
								  (LSA_UINT)snmp_recv->data_length,
								  OHA_SNMP_ERR_NOSUCHNAME);

			/* free response buffer first */
            OHA_SOCK_FREE_LOWER_MEM(&RetVal, snmp_recv->buffer, pHDB->Params.pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);

		    /* change send buffers and respond the Ind-Rqb */
			snmp_recv->buffer      = MemPtr;
		}
		else
		{
			OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN,">>>: Could not allocate receive buffer for SNMP (Handle: 0x%X,MibNode: 0x%X)",pHDB->ThisHandle,MibNode);
		}
	}

    /* reprovide */
    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_UDP_RECEIVE (Handle: 0x%X,SOCKHandle: 0x%X,pRxRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRxRQB);
    OHA_SOCK_RQB_SET_OPCODE(pRxRQB,SOCK_OPC_SNMP_RECEIVE_RSP);
    OHA_SOCK_RQB_SET_HANDLE(pRxRQB,pHDB->SockHandle); /* set handle */
    OHA_SOCK_REQUEST_LOWER(pRxRQB,pHDB->Params.pSys);
    pSubAgnt->RxPendingCnt++; /* number of Rx-Request pending within SOCK */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKParseDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKSetNameAddress                      +*/
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
/*+  Description: Set name of station for SOCK address indication           +*/
/*+               (SOCK_OPC_SET_ADDR_INFO)                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetNameAddress(
    LSA_UINT16                      NicID)
{
    LSA_UINT16                 Status  = OHA_OK;
    OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
    LSA_UINT16                 Length;
    OHA_HDB_SOCK_PTR_TYPE      pHDB = OHA_SOCKGetHandleGlobal();
    OHA_HDB_EDD_PTR_TYPE       pHDBEdd = OHA_EDDGetHandleByNIC(NicID);
    LSA_BOOL                   IsIdentical = LSA_TRUE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKSetNameAddress(NicID: 0x%X)", NicID);

	if(OHA_IS_NULL(pHDB))		/* AP00988138 */
	{
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"SOCK-Channel already closed for this NicID(0x%X) !", NicID);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
							  "OUT: OHA_SOCKSetNameAddress(Status: 0x%X)",Status);
		return(Status);
	}

    OHA_ASSERT_NULL_PTR(pHDBEdd);

    /* read NameOfStation */
    Length = (LSA_UINT16)OHA_DB_READ_STATION_NAME(NicID, &NamePtr);
    OHA_ASSERT(Length <= OHA_MAX_STATION_NAME_LEN);

    /* is the NameOfStation different from the provided one ? */
    if (Length != pHDBEdd->SetAddrInfo.NoS.Length)
    {
        IsIdentical = LSA_FALSE;
    }
    if (IsIdentical)
    {
        IsIdentical = OHA_MEMCMP(NamePtr, pHDBEdd->SetAddrInfo.NoS.pName, Length);
    }

    /* prepare SOCK address information */
    if (!IsIdentical)
    {
        LSA_USER_ID_TYPE         UserId;
        OHA_SOCK_LOWER_RQB_PTR_TYPE     pRQB = LSA_NULL;

        UserId.uvar32 = 0;

        /* allocate RQB */
        OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pHDB->Params.pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
        {
            SOCK_RQB_ARGS_SET_ADDR_INFO_TYPE *args = &pRQB->args.set_addr_info;

            OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SET_ADDR_INFO);
            OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
            OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB, pHDB->ThisHandle); /* only address info for SOCK */

            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SET_ADDR_INFO (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

            /* store the NoS parameters local and provide to SOCK */
            args->NoS.Length = Length;
            args->NoS.pName  = LSA_NULL;

            if (Length)
            {
                OHA_SOCK_LOWER_MEM_PTR_TYPE pName = LSA_NULL;
                OHA_SOCK_ALLOC_LOWER_MEM(&pName, UserId, args->NoS.Length, pHDB->Params.pSys);
                if (OHA_IS_NULL(pName))
                {
                    LSA_UINT16 RetVal = LSA_RET_OK;
                    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
                    OHA_MEM_FREE_FAIL_CHECK(RetVal);
                    Status = OHA_ERR_RESOURCE;
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower memory failed!");
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OUT: OHA_SOCKSetNameAddress(Status: 0x%X)",Status);
                    return(Status);
                }
                args->NoS.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)pName;
                OHA_MEMCPY(args->NoS.pName, NamePtr, args->NoS.Length);
            }

            /* ... and store them local */
            OHA_ASSERT_NULL_PTR(pHDBEdd->SetAddrInfo.NoS.pName);
            pHDBEdd->SetAddrInfo.NoS.Length = Length;
            if (Length)
            {
                OHA_MEMCPY(pHDBEdd->SetAddrInfo.NoS.pName, NamePtr, args->NoS.Length);
            }

            /* the IP params are taken from the buffered AddrInfo! */
            args->InterfaceID    = pHDBEdd->SetAddrInfo.InterfaceID;
			OHA_ASSERT(pHDBEdd->SetAddrInfo.InterfaceID != 0);
            args->CurrentIPSuite = pHDBEdd->SetAddrInfo.CurrentIPSuite;
            args->CurrentIPSuiteIsEmergency      = pHDBEdd->SetAddrInfo.CurrentIPSuiteIsEmergency;
            args->CurrentIPSuiteConflictDetected = pHDBEdd->SetAddrInfo.CurrentIPSuiteConflictDetected;
            args->ExpectedIPSuite  = pHDBEdd->SetAddrInfo.ExpectedIPSuite;
            args->EmergencyIPSuite = pHDBEdd->SetAddrInfo.EmergencyIPSuite;

            /* no request SockSetAddrInfo active ? */
            OHA_ASSERT(pHDB->SetIpRQBCnt < 2);  /* no queueing is SOCK */
            if (pHDB->SetIpRQBCnt == 0)
            {
                OHA_SOCK_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
                pHDB->SetIpRQBCnt++;
            }
            else
            {
                /* queue the request in the SOCK-HDB */
                OHA_RQB_PUT_BLOCK_TOP(pHDB->SetIpRQBs.pBottom,
                                      pHDB->SetIpRQBs.pTop,
                                      pRQB);
            }
        }
        else
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory2 failed!");
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKSetNameAddress(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKSetIpAddress                        +*/
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
/*+  Description: Set ip address via SOCK                                   +*/
/*+               (SOCK_OPC_SET_IPADDR)                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetIpAddress(
    LSA_UINT16                      NicID,
    LSA_UINT16                      Orginator,
    OHA_COMMON_MEM_U8_PTR_TYPE      pIpAddr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pNetMask,
    OHA_COMMON_MEM_U8_PTR_TYPE      pGateWay,
    LSA_BOOL                        CurrentIpSuiteIsEmergency)
{
    OHA_HDB_SOCK_PTR_TYPE           pHDB = OHA_SOCKGetHandleGlobal();
    OHA_HDB_EDD_PTR_TYPE            pHDBEdd = OHA_EDDGetHandleByNIC(NicID);
    OHA_SOCK_LOWER_RQB_PTR_TYPE     pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                pSys;
    LSA_UINT16                      Status = OHA_OK;
    LSA_USER_ID_TYPE                UserId;

    OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKSetIpAddress(NicID: 0x%X, Orginator: 0x%X, pIpAddr: 0x%X,pNetMask: 0x%X,pGateWay: 0x%X,CurrentIpSuiteIsEmergency: 0x%X)",
                          NicID, Orginator, pIpAddr, pNetMask, pGateWay, CurrentIpSuiteIsEmergency);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDB,  LSA_NULL) )
    {
        Status = OHA_ERR_SYS_PATH;
        /* No SOCK channel, no call to SOCK. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"Request SOCK_OPC_SET_IPADDR with no Lower Channel for SOCK");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetIpAddress(Status: 0x%X)",Status);
        return(Status);
    }

    OHA_ASSERT_NULL_PTR(pHDBEdd);
    OHA_ASSERT_NULL_PTR(pIpAddr);
    OHA_ASSERT_NULL_PTR(pNetMask);
    OHA_ASSERT_NULL_PTR(pGateWay);

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        SOCK_RQB_ARGS_SET_ADDR_INFO_TYPE *args = &pRQB->args.set_addr_info;
        OHA_DB_IF_PTR_TYPE pDB  = OHA_DB_GET_INTERFACE(NicID);
        OHA_COMMON_MEM_U8_PTR_TYPE pIpSuite  = LSA_NULL;

        OHA_ASSERT_NULL_PTR(pDB);
		OHA_ASSERT(pDB->InterfaceID != 0);

        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SET_ADDR_INFO);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SET_IPADDRESS (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

        args->InterfaceID = pDB->InterfaceID;

        /* take the NoS params from the buffered AddrInfo */
        args->NoS.Length          = pHDBEdd->SetAddrInfo.NoS.Length;
        OHA_ASSERT(args->NoS.Length<=OHA_MAX_STATION_NAME_LEN);
        if (args->NoS.Length)
        {
            OHA_SOCK_LOWER_MEM_PTR_TYPE pName = LSA_NULL;
            OHA_SOCK_ALLOC_LOWER_MEM(&pName, UserId, args->NoS.Length, pHDB->Params.pSys);
            if (OHA_IS_NULL(pName))
            {
                LSA_UINT16 RetVal = LSA_RET_OK;
                OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
                Status = OHA_ERR_RESOURCE;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower memory failed!");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetIpAddress(Status: 0x%X)",Status);
                return(Status);
            }
            args->NoS.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)pName;
            OHA_ASSERT_NULL_PTR(pHDBEdd->SetAddrInfo.NoS.pName);
            OHA_MEMCPY(args->NoS.pName, pHDBEdd->SetAddrInfo.NoS.pName, args->NoS.Length);
        }
        else
        {
            args->NoS.pName = LSA_NULL;
        }

        /***************** forward the CurrentIPSuite ******************/

        args->CurrentIPSuite.IpAddr.S_un.S_un_b.s_b1 = *pIpAddr;
        args->CurrentIPSuite.IpAddr.S_un.S_un_b.s_b2 = *(pIpAddr+1);
        args->CurrentIPSuite.IpAddr.S_un.S_un_b.s_b3 = *(pIpAddr+2);
        args->CurrentIPSuite.IpAddr.S_un.S_un_b.s_b4 = *(pIpAddr+3);

        args->CurrentIPSuite.NetMask.S_un.S_un_b.s_b1 = *pNetMask;
        args->CurrentIPSuite.NetMask.S_un.S_un_b.s_b2 = *(pNetMask+1);
        args->CurrentIPSuite.NetMask.S_un.S_un_b.s_b3 = *(pNetMask+2);
        args->CurrentIPSuite.NetMask.S_un.S_un_b.s_b4 = *(pNetMask+3);

        args->CurrentIPSuite.Gateway.S_un.S_un_b.s_b1 = *pGateWay;
        args->CurrentIPSuite.Gateway.S_un.S_un_b.s_b2 = *(pGateWay+1);
        args->CurrentIPSuite.Gateway.S_un.S_un_b.s_b3 = *(pGateWay+2);
        args->CurrentIPSuite.Gateway.S_un.S_un_b.s_b4 = *(pGateWay+3);

        args->CurrentIPSuiteConflictDetected = pDB->DcpData.IpConflictDetected;

        /* AP01270055: OHA_ADDR_INFO_TYPE: Missing information by AddressConflict of an EmergencyIP */
        args->CurrentIPSuiteIsEmergency = CurrentIpSuiteIsEmergency;

        /***************** forward the ExpectedIpSuite ******************/

        /* in case of a conflict do not delete the expected IP in the indication */
        if (pDB->DcpData.IpConflictDetected)
        {
            (LSA_VOID)oha_db_read_expected_ip_suite(NicID, &pIpSuite, LSA_NULL, LSA_NULL);
            if (OHA_IS_NOT_NULL(pIpSuite))
            {
                args->ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b1 = *pIpSuite;
                args->ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b2 = *(pIpSuite+1);
                args->ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b3 = *(pIpSuite+2);
                args->ExpectedIPSuite.IpAddr.S_un.S_un_b.s_b4 = *(pIpSuite+3);

                pIpSuite += OHA_IP_ADDR_SIZE;
                args->ExpectedIPSuite.NetMask.S_un.S_un_b.s_b1 = *pIpSuite;
                args->ExpectedIPSuite.NetMask.S_un.S_un_b.s_b2 = *(pIpSuite+1);
                args->ExpectedIPSuite.NetMask.S_un.S_un_b.s_b3 = *(pIpSuite+2);
                args->ExpectedIPSuite.NetMask.S_un.S_un_b.s_b4 = *(pIpSuite+3);

                pIpSuite += OHA_IP_ADDR_SIZE;
                args->ExpectedIPSuite.Gateway.S_un.S_un_b.s_b1 = *pIpSuite;
                args->ExpectedIPSuite.Gateway.S_un.S_un_b.s_b2 = *(pIpSuite+1);
                args->ExpectedIPSuite.Gateway.S_un.S_un_b.s_b3 = *(pIpSuite+2);
                args->ExpectedIPSuite.Gateway.S_un.S_un_b.s_b4 = *(pIpSuite+3);
            }
            else
            {
                args->ExpectedIPSuite.IpAddr.s_addr = 0;
                args->ExpectedIPSuite.NetMask.s_addr = 0;
                args->ExpectedIPSuite.Gateway.s_addr = 0;
            }
        }
        else    /* no conflict: ExpectedIpSuite = CurrentIpSuite */
        {
            args->ExpectedIPSuite = args->CurrentIPSuite;
        }

        /***************** forward the EmergencyIpSuite ******************/

        /* in case of a conflict do not delete the emergency IP in the indication */
        if (pDB->DcpData.IpConflictDetected)
        {
            (LSA_VOID)oha_db_read_emergency_ip_suite(NicID, &pIpSuite, LSA_NULL, LSA_NULL);
            if (OHA_IS_NOT_NULL(pIpSuite))
            {
                args->EmergencyIPSuite.IpAddr.S_un.S_un_b.s_b1 = *pIpSuite;
                args->EmergencyIPSuite.IpAddr.S_un.S_un_b.s_b2 = *(pIpSuite+1);
                args->EmergencyIPSuite.IpAddr.S_un.S_un_b.s_b3 = *(pIpSuite+2);
                args->EmergencyIPSuite.IpAddr.S_un.S_un_b.s_b4 = *(pIpSuite+3);

                pIpSuite += OHA_IP_ADDR_SIZE;
                args->EmergencyIPSuite.NetMask.S_un.S_un_b.s_b1 = *pIpSuite;
                args->EmergencyIPSuite.NetMask.S_un.S_un_b.s_b2 = *(pIpSuite+1);
                args->EmergencyIPSuite.NetMask.S_un.S_un_b.s_b3 = *(pIpSuite+2);
                args->EmergencyIPSuite.NetMask.S_un.S_un_b.s_b4 = *(pIpSuite+3);

                pIpSuite += OHA_IP_ADDR_SIZE;
                args->EmergencyIPSuite.Gateway.S_un.S_un_b.s_b1 = *pIpSuite;
                args->EmergencyIPSuite.Gateway.S_un.S_un_b.s_b2 = *(pIpSuite+1);
                args->EmergencyIPSuite.Gateway.S_un.S_un_b.s_b3 = *(pIpSuite+2);
                args->EmergencyIPSuite.Gateway.S_un.S_un_b.s_b4 = *(pIpSuite+3);
            }
            else
            {
                args->EmergencyIPSuite.IpAddr.s_addr = 0;
                args->EmergencyIPSuite.NetMask.s_addr = 0;
                args->EmergencyIPSuite.Gateway.s_addr = 0;
            }
        }
        else    /* no conflict */
        {
            if (CurrentIpSuiteIsEmergency)  /* EmergencyIpSuite = CurrentIpSuite */
            {
                args->EmergencyIPSuite = args->CurrentIPSuite;
            }
            else    /* delete the EmergencyIpSuite */
            {
                args->EmergencyIPSuite.IpAddr.s_addr = 0;
                args->EmergencyIPSuite.NetMask.s_addr = 0;
                args->EmergencyIPSuite.Gateway.s_addr = 0;
            }
        }

#if 0   /* for testing a CloseCHannel() during a SockSetIpAddr() */
        {
            /* or a SetIp/Name during a SockSetIpAddr() */
            void _sleep( long msSleep );
            _sleep(1000);
        }
#endif

        /* no request SockSetAddrInfo active ? */
        OHA_ASSERT(pHDB->SetIpRQBCnt < 2);  /* no queueing is SOCK */
        if (pHDB->SetIpRQBCnt == 0)
        {
            OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
            pHDB->SetIpRQBCnt++;
        }
        else
        {
            /* queue the request in the SOCK-HDB */
            OHA_RQB_PUT_BLOCK_TOP(pHDB->SetIpRQBs.pBottom,
                                  pHDB->SetIpRQBs.pTop,
                                  pRQB);
        }
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKSetIpAddress(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKSetIpAddressDone                    +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Set IpAddress done handling                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKSetIpAddressDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal   = LSA_RET_OK;
    LSA_UINT16              Orginator;
    LSA_UINT16              NICId;
	OHA_HDB_EDD_PTR_TYPE    pHDBEdd;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKSetIpAddressDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_SET_IPADDRESS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    OHA_ASSERT_NULL_PTR(pRQB);

	NICId = OHA_TOPOGetNicByInterface(pRQB->args.set_addr_info.InterfaceID);
	pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBEdd);    /* must be an if-NIC, not a global NIC */

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);

    OHA_ASSERT(OHA_SOCK_RQB_GET_OPCODE(pRQB) == SOCK_OPC_SET_ADDR_INFO);

    OHA_ASSERT(pHDB->SetIpRQBCnt == 1); /* no queueing is SOCK possible */
    pHDB->SetIpRQBCnt--;

    switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
            break;

        default:
            if ((OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_SOCK) || /* If-specific NIC */
				(OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_EDD))
            {
                if ((OHA_IS_NOT_NULL(pHDB->pOCReq) && OHA_RQB_GET_OPCODE(pHDB->pOCReq) == OHA_OPC_OPEN_CHANNEL) ||
                    (OHA_IS_NOT_NULL(pHDBEdd->pOCReq) && OHA_RQB_GET_OPCODE(pHDBEdd->pOCReq) == OHA_OPC_OPEN_CHANNEL)) /* AP01215575 */
                {
                    /* delete the (forwarded) initialized real ip address */
                    (LSA_VOID)oha_db_delete_real_ip_suite(NICId);

					/* AP00891593: OpenSockChannel and SetIp failed, nevertheless the channel has to be opened */
					pRQB->args.set_addr_info.CurrentIPSuite.IpAddr.s_addr  = 0; /* for initializing SetAddrInfo, see below */
					pRQB->args.set_addr_info.CurrentIPSuite.NetMask.s_addr = 0;
					pRQB->args.set_addr_info.CurrentIPSuite.Gateway.s_addr = 0;

					Response = OHA_OK;
					break;
                }
            }

            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_SET_IPADDRESS failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    if (Response == OHA_OK)
    {
        SOCK_RQB_ARGS_SET_ADDR_INFO_TYPE *args = &pRQB->args.set_addr_info;

        /* local storage of the provided IP parameters */
        pHDBEdd->SetAddrInfo.InterfaceID      = args->InterfaceID;
        pHDBEdd->SetAddrInfo.CurrentIPSuite   = args->CurrentIPSuite;
        pHDBEdd->SetAddrInfo.CurrentIPSuiteIsEmergency      = args->CurrentIPSuiteIsEmergency;
        pHDBEdd->SetAddrInfo.CurrentIPSuiteConflictDetected = args->CurrentIPSuiteConflictDetected;
        pHDBEdd->SetAddrInfo.ExpectedIPSuite  = args->ExpectedIPSuite;
        pHDBEdd->SetAddrInfo.EmergencyIPSuite = args->EmergencyIPSuite;
    }

    if (OHA_IS_NOT_NULL(pRQB->args.set_addr_info.NoS.pName))
    {
        OHA_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.set_addr_info.NoS.pName,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }
    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
            /* from the USER (OhaOpcSetAddress) */
        case OHA_HANDLE_TYPE_USER:
        {
            OHA_HDB_USER_PTR_TYPE pHDBUser;

            /* check the handle */
            if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
            {
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            }

            pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(Orginator)));
            OHA_ASSERT(pHDBUser);

            /* continue with SetAddress or DcpResponse */
            OHA_SOCKUserSetIpPending(pHDBUser, Response);
        }
        break;
        case OHA_HANDLE_TYPE_DCP:
        {
            OHA_HDB_DCP_PTR_TYPE  pHDBDcp;
            LSA_BOOL Finish = LSA_TRUE;

            pHDBDcp = OHAGetDcpHDBFromHandle((LSA_HANDLE_TYPE)Orginator);
            OHA_ASSERT(pHDBDcp);
			OHA_ASSERT(NICId == pHDBDcp->Params.pDetail->Params.Dcp.NICId);

            /* the existance of a query-user is depending on query state */
            if (Response != OHA_OK)     /* SockSetIpAddress failed() */
            {
                (LSA_VOID)oha_db_delete_temp_ip_address(NICId);
                pHDBDcp->CheckSetInd.Options &= ~OHA_CTRL_IP_SUITE; /* IP-Address not set */
            }
#ifndef OHA_CFG_NO_NARE
            else
            {
                /* cancel running check requests */
                LSA_UINT32 CheckOptions = (pHDBDcp->CheckSetInd.CheckIpEnabled) ? OHA_CTRL_IP_SUITE : 0;
                Finish = OHA_NARECheckAddressCancel(NICId,
                                                    (LSA_UINT16)(pHDBDcp->CheckSetInd.Options | CheckOptions),
                                                    pHDBDcp->ThisHandle);
            }
#endif
            if (Finish)
            {
                (LSA_UINT16)OHA_DoDcpSetAddr(OHA_DCPGetHandleByNIC(NICId));
            }
        }
        break;
        case OHA_HANDLE_TYPE_SOCK:
        {
            /* from SockOpenChannel() - continue with OpenChannel */
            /* if the channel is closing - no action */
            /* from SOCKSetNameAddress() - only providing NameOfStation (no action) */
            if (OHA_IS_NOT_NULL(pHDB->pOCReq) && OHA_RQB_GET_OPCODE(pHDB->pOCReq) == OHA_OPC_OPEN_CHANNEL)
            {
                OHA_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */
            }
        }
        break;
        case OHA_HANDLE_TYPE_EDD:   /* AP01017785: from EddOpenChannel */
		{
			OHA_EDDChannelDone(pHDBEdd,Response); /* further actions taken in EDD channel handler */

			if (Response == OHA_OK)
			{
				/* set MIBII params for the interface (port = 0) first */
				(LSA_UINT16)OHA_SOCKSetMib2Status(NICId,
												  0,
												  OHA_HANDLE_TYPE_EDD);
			}
		}
		break;

        case OHA_HANDLE_TYPE_NARE:  /* from IP conflict */
        {
            if (Response != OHA_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: IpCpnflict detected, SetIP(0.0.0.0) failed.(0x%X).", Response);
            }
            else    /* indicate the users */
            {
                RetVal = OHA_EDDSetArpFilter(NICId, OHA_HANDLE_TYPE_DCP);
                if (RetVal != OHA_OK)
                {
                    Response = OHA_ERR_FAILED;
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_SOCKSetIpAddressDone - EDDSetArpFilter(RetVal: 0x%X)", RetVal);
                }
                RetVal = OHA_EDDSetIp(NICId, OHA_HANDLE_TYPE_DCP);
                if (RetVal != OHA_OK)
                {
                    Response = OHA_ERR_FAILED;
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_SOCKSetIpAddressDone - EDDSetIp(RetVal: 0x%X)", RetVal);
                }

                /* update the LLDP-send data if LLDP-Send is active */
                RetVal = OHA_LLDPUpdateSend(NICId,
                                            OHA_CTRL_IP_SUITE,
                                            OHA_HANDLE_TYPE_SYS);
                if (RetVal != OHA_OK)
                {
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OHA_SOCKSetIpAddressDone - LLDPUpdateSend(RetVal: 0x%X)", RetVal);
                }

                OHA_AddressIndicateEventAllUser(NICId);/* address changed - indication */
				OHA_DcpIndicateCheckIpEvent(NICId); /* confirm DcpInd users */
            }
        }
        break;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SOCKSetIpAddressDone: Invalid HandleType (0x%X).", OHA_HANDLE_GET_TYPE(Orginator));
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    /* is there a queued request ? */
    if (pHDB->SetIpRQBCnt == 0)
    {
        if (OHA_IS_NOT_NULL(pHDB->SetIpRQBs.pBottom))
        {
            OHA_RQB_REM_BLOCK_BOTTOM(pHDB->SetIpRQBs.pBottom,
                                     pHDB->SetIpRQBs.pTop,
                                     pRQB);
            OHA_ASSERT_NULL_PTR(pRQB);

            OHA_SOCK_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
            pHDB->SetIpRQBCnt++;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetIpAddressDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKStarUp                              +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: StartUp handling                                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKStartUp(
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response = OHA_OK;
	LSA_UINT16              NICId    = pHDB->Params.pDetail->Params.Sock.NICId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKStarUp(pHDB: 0x%X)",
                          pHDB);

	if (OHA_EDDCheckNICUsed(NICId))     /* AP01017785: EDD System channel is opened */
	{
        OHA_COMMON_MEM_U8_PTR_TYPE IpAdrPtr         = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr    = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtrPtr = LSA_NULL;

        /* read IP-Address, Subnet-mask and Default Router Address ...  */
        (LSA_VOID) OHA_DB_READ_IP_SUITE(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr);

        if (OHA_IS_NOT_NULL(IpAdrPtr))  /* no ip address initialized in the database, set to (0,0,0,0) */
        {
            /* initial set of the (local) ip address */
            Response = OHA_SOCKSetIpAddress(NICId,
                                            pHDB->ThisHandle,
                                            IpAdrPtr, SubnetMaskPtr, RouterAddrPtrPtr,
                                            LSA_FALSE);
            if (Response != OHA_OK)
            {
                /* initial set of the (local) ip address fails -> delete the real ip address (0,0,0,0) */
                (LSA_VOID)oha_db_delete_real_ip_suite(NICId);

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-SetIpAddress failed.[0x%X]",Response);
                Response = OHA_ERR_LL;
            }

                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                                      "OUT: OHA_SOCKStarUp(Response: 0x%X)", Response);
                return(Response);
            }
		}

    OHA_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKStarUp(Response: 0x%X)", Response);
	return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKShutDown                            +*/
/*+  Input                      OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Shut Down handling                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKShutDown(
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKShutDown(pHDB: 0x%X)",
                          pHDB);

    /* now we can close the channel */
    Response = OHA_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys);
    if ( Response == OHA_OK )
	{
#ifndef OHA_CFG_NO_REMA
		/* the (last) SOCK channel clears the list, otherwise we have no NicID */
		oha_snmp_undo_init(pHDB->Params.pDetail->Params.Sock.NICId);
#endif
	}
	else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close SockChannel(). Response: 0x%X", Response);
        /* is this a fatal error ?                      */
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKShutDown(Response: 0x%X)", Response);
	return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKFreeMib2Params                      +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    -                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:     Frees MIB2- RQB                                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKFreeMib2Params(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16              RetVal = LSA_RET_OK;
    SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE mib2_params;

    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKFreeMib2Params(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

	switch(OHA_SOCK_RQB_GET_OPCODE(pRQB))
	{
	case SOCK_OPC_SET_MIB2_SYSGROUP_DATA:
        mib2_params = &pRQB->args.set_mib2_sysgroup_data;
		break;
	case SOCK_OPC_MIB2_SYSGROUP_DATA_IND:
        mib2_params = &pRQB->args.mib2_sysgroup_data_ind;
		break;
	default:
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SOCKFreeMib2Params: Invalid Opcode (0x%X).", OHA_SOCK_RQB_GET_OPCODE(pRQB));
		OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
		return;
	}

    if (OHA_IS_NOT_NULL(mib2_params->sysName.pName))
    {
        OHA_SOCK_FREE_LOWER_MEM(&RetVal,mib2_params->sysName.pName,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }
    if (OHA_IS_NOT_NULL(mib2_params->sysDescr.pName))
    {
        OHA_SOCK_FREE_LOWER_MEM(&RetVal,mib2_params->sysDescr.pName,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }
    if (OHA_IS_NOT_NULL(mib2_params->sysContact.pName))
    {
        OHA_SOCK_FREE_LOWER_MEM(&RetVal,mib2_params->sysContact.pName,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }
    if (OHA_IS_NOT_NULL(mib2_params->sysLocation.pName))
    {
        OHA_SOCK_FREE_LOWER_MEM(&RetVal,mib2_params->sysLocation.pName,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }

    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKFreeMib2Params(RetVal: 0x%X)",RetVal);
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKSetMib2Status                       +*/
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
/*+  Description: Set the MIBII params SysName, SysDescr and IfDescr        +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Request                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2Status(
    LSA_UINT16              NicID,			/* interface specific NIC */
    LSA_UINT16              PortID,
    LSA_UINT16              Orginator)
{
    OHA_HDB_SOCK_PTR_TYPE                   pHDB = OHA_SOCKGetHandleGlobal();
    OHA_HDB_EDD_PTR_TYPE                    pHDBEdd = OHA_EDDGetHandleByNIC(NicID);
    OHA_SOCK_LOWER_RQB_PTR_TYPE             pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    SOCK_MIB2_IFGROUP_DATA_PTR_TYPE         mib2_if_params;
    OHA_DB_IF_PTR_TYPE                      pDB;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKSetMib2Status(NicID: 0x%X, PortID: 0x%X, Orginator: 0x%X)",
                          NicID, PortID, Orginator);

	OHA_ASSERT_NULL_PTR(pHDBEdd);

    if (OHA_IS_NULL(pHDB))
	{
		/* SOCK channel is not open */
		Status = OHA_ERR_RESOURCE;
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,
							  "OUT: OHA_SOCKSetMib2Status(Status: 0x%X)",Status);
	    return(Status);
	}

	/* The Portstrings (port_id = 1...n) and the string for the IF (port_id = 0) is supported too! */ 
    if (PortID > OHA_CFG_MAX_PORTS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* check PortID */
    pDB = OHA_DB_GET_INTERFACE(NicID);
    OHA_ASSERT_NULL_PTR(pDB);
    if (pDB->PortCnt > OHA_CFG_MAX_PORTS)
    {
        Status = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: OHA_SOCKSetMib2Status() - no such port (%u)", pDB->PortCnt);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2Status(Status: 0x%X)",Status);
        return(Status);
    }

    if (PortID != 0)	/* not for the interface */
	{
	    switch (OHA_HANDLE_GET_TYPE(Orginator)) /* AP01022052 */
		{
			case OHA_HANDLE_TYPE_USER: /* user driven SetMib2Status */
			case OHA_HANDLE_TYPE_SOCK: /* update only */
				pHDBEdd->SetMib2_IsActual[PortID-1] = LSA_TRUE;
				break;
			case OHA_HANDLE_TYPE_EDD:
				if(pHDBEdd->SetMib2_PendingCnt[PortID-1] == 0)	/* update now */
				{
					pHDBEdd->SetMib2_IsActual[PortID-1] = LSA_TRUE;
				}
				else /* Set(s) already active, update later */
				{
					Status = OHA_OK;
					pHDBEdd->SetMib2_IsActual[PortID-1] = LSA_FALSE;
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"OHA_SOCKSetMib2Status already pending (PortId: 0x%X, PendingCnt: 0x%X).", PortID, pHDBEdd->SetMib2_PendingCnt[PortID-1]);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2Status(Status: 0x%X)",Status);
					return(Status);
				}
				break;
			default:
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				break;
		}
	}

    /* allocate RQB */
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SOCK_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL;

        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SET_MIB2_IFGROUP_DATA);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SET_MIBII_PARAMS (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

        mib2_if_params = &pRQB->args.set_mib2_ifgroup_data;

        mib2_if_params->InterfaceID = pDB->InterfaceID;
        mib2_if_params->PortID      = PortID;
		
        mib2_if_params->ifDescr.Length  = 0;
        mib2_if_params->ifDescr.pName   = LSA_NULL;
        mib2_if_params->ifDescr.Remanent = LSA_FALSE;

        /**************  IfDescr  ******************/
		Status = OHA_DB_READ_IF_DESCR(NicID, PortID,
										&mib2_if_params->ifDescr.pName, &mib2_if_params->ifDescr.Length);
		OHA_ASSERT(Status == LSA_RET_OK);

		if (mib2_if_params->ifDescr.Length != 0)
		{
            OHA_ASSERT_NULL_PTR(mib2_if_params->ifDescr.pName);
		}

		/* AP01559453: For SOCK a NullPointer isn't an empty string */
		{
			/* we need max. length for a dynamic part of the link status string */
			OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, LLDP_TLV_PORT_DESCR_MAX_SIZE, pSys);
			if (OHA_IS_NULL(MemPtr))
			{
				LSA_UINT16 RetVal = LSA_RET_OK;

				Status = OHA_ERR_RESOURCE;
				OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower IfDescr-memory failed!");

				OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
				OHA_MEM_FREE_FAIL_CHECK(RetVal);

				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
									  "OUT: OHA_SOCKSetMib2Status(Status: 0x%X)",Status);
				return(Status);
			}

			OHA_MEMCPY(MemPtr, mib2_if_params->ifDescr.pName, mib2_if_params->ifDescr.Length);
			mib2_if_params->ifDescr.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        }
        /*******************************************/

	    if (PortID != 0)	/* not for the interface */
		{
			OHA_ASSERT_NULL_PTR(pHDBEdd);
			pHDBEdd->SetMib2_PendingCnt[PortID-1]++; /* AP01022052 */
		}
        pHDB->Mib2RQBCnt++;
        OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKSetMib2Status(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKSetMib2StatusDone                   +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Set MIBII params SysName, SysDescr and IfDescr            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2StatusDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16              Orginator;
    LSA_UINT16              Response;
    LSA_UINT16              PortId;
	LSA_UINT16              NICId;
	LSA_UINT16              RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKSetMib2StatusDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);

	/* get interface NIC (from RQB) */
	NICId = OHA_DB_GET_NIC(pRQB->args.set_mib2_ifgroup_data.InterfaceID);

	OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

	PortId  = pRQB->args.set_mib2_ifgroup_data.PortID; /* AP01022052 */
    if (PortId > OHA_CFG_MAX_PORTS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_SET_MIBII_PARAMS done (Handle: 0x%X,Rsp: 0x%X, PortId: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB), PortId);

    switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_SET_MIBII_PARAMS failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    if (OHA_IS_NOT_NULL(pRQB->args.set_mib2_ifgroup_data.ifDescr.pName))
    {
        OHA_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.set_mib2_ifgroup_data.ifDescr.pName,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }

    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    pHDB->Mib2RQBCnt--;

    if (PortId != 0)	/* not for the interface */
	{
	    OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

		OHA_ASSERT(OHA_EDDCheckNICUsed(NICId));
		if(OHA_IS_NOT_NULL(pHDBEdd))
		{
			OHA_ASSERT(pHDBEdd->SetMib2_PendingCnt[PortId-1]);

			pHDBEdd->SetMib2_PendingCnt[PortId-1]--; /* AP01022052 */

			if(!pHDBEdd->SetMib2_IsActual[PortId-1]) /* AP01022052 */
			{
				if(pHDBEdd->SetMib2_PendingCnt[PortId-1] == 0) /* update now */
				{
					if (pHDB->State == OHA_HANDLE_STATE_OPEN) /* SetMib2Status is possible ? */
					{
						(LSA_UINT16)OHA_SOCKSetMib2Status(NICId, PortId, OHA_HANDLE_TYPE_EDD);
					}
				}
			}
		}
		else
		{
	        OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD channel ist closing, no SOCK_OPC_SET_MIBII_PARAMS (Handle: 0x%X,SOCKHandle: 0x%X) anymore",pHDB->ThisHandle,pHDB->SockHandle);
		}
	}

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_SOCK:  /* from SockOpenChannel() - continue with OpenChannel */
            break;
        case OHA_HANDLE_TYPE_USER:  /* from OHA_OPC_PRM_WRITE, OHA_OPC_LLDP_WRITE_OPTION */
			/* check for pending user-requests   */
			{
				OHA_HDB_USER_PTR_TYPE pHDBUser;

				/* check the handle */
				if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
				{
					OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				}

				pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(Orginator)));
				OHA_ASSERT(pHDBUser);
				OHA_ASSERT(NICId == pHDBUser->Params.pDetail->Params.User.NICId);

				/* continue with WriteLldpOption or PrmWrite */
				OHA_SOCKUserSetMib2ParamsPending(pHDBUser, Response);

				/* SockSetMib2Params.Ind during a SockSetMib2Params.Req was igored */
				OHA_ASSERT(!OHA_IsAnyLocalSetMib2Active(NICId));

				if (OHA_IS_NULL(pHDB->pMib2IndRQB))
				{
					/* reprovide the MIB2-Indication for handling writeable MIB2 parameters */
			        OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: Reprovide SOCK_OPC_MIBII_PARAMS_IND (Handle: 0x%X,SOCKHandle: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle);
					if (OHA_OK != OHA_SOCKMib2ParamsInd(NICId, pHDB->ThisHandle))
					{
				        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN,">>>: Could not Reprovide SOCK_OPC_MIBII_PARAMS_IND (Handle: 0x%X,SOCKHandle: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle);
					}
				}
			}
			return;
        case OHA_HANDLE_TYPE_EDD:
        {
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            OHA_ASSERT_NULL_PTR(pDB);
            if (PortId < pDB->PortCnt)	/* AP01328959: ifDescr for  MIB-2  isn't filled correctly if the global (SOCK) channel is opened first */
            {
                (LSA_UINT16)OHA_SOCKSetMib2Status(NICId,
                                                  ++PortId,
                                                  OHA_HANDLE_TYPE_EDD);
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2StatusDone(Response: 0x%X)", Response);
            }
            return; /* no further action, fade out */
        }
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SOCKSetMib2StatusDone: Invalid HandleType (0x%X).", OHA_HANDLE_GET_TYPE(Orginator));
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    if (Response == OHA_OK)
    {
        OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
        OHA_ASSERT_NULL_PTR(pDB);

        /* are MIBII params for next port to set ? */
        if (PortId < pDB->PortCnt)
        {
            (LSA_UINT16)OHA_SOCKSetMib2Status(NICId,
                                              ++PortId,
                                              OHA_HANDLE_TYPE_SOCK);
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2StatusDone(Response: 0x%X)", Response);
            return;
        }
    }

    OHA_SOCKChannelDone(pHDB, Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2StatusDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKUpdateMib2Params                    +*/
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
/*+  Description: Updates the MIBII params SysName, SysDescr and IfDescr    +*/
/*+               for an interface                                          +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Request                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKUpdateMib2Params(
    LSA_UINT16              NicID,	/* if specific NIC */
    LSA_UINT16              Orginator,
    OHA_UPPER_MEM_U8_PTR_TYPE pSysName,
    LSA_UINT16              SysNameLen,
	LSA_BOOL                SysNameRema,
    OHA_UPPER_MEM_U8_PTR_TYPE pSysContact,
    LSA_UINT16              SysContactLen,
    LSA_BOOL                SysContactRema,
    OHA_UPPER_MEM_U8_PTR_TYPE pSysLocation,
    LSA_UINT16              SysLocationLen,
    LSA_BOOL                SysLocationRema
)
{
    OHA_HDB_SOCK_PTR_TYPE                   pHDB = OHA_SOCKGetHandleGlobal();
    OHA_SOCK_LOWER_RQB_PTR_TYPE             pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE        mib2_params;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKUpdateMib2Params(NicID: 0x%X, Orginator: 0x%X)",
                          NicID, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SOCK_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL;

        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SET_MIB2_SYSGROUP_DATA);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SET_MIBII_PARAMS (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

        mib2_params = &pRQB->args.set_mib2_sysgroup_data;

        mib2_params->sysName.Length    = SysNameLen;
        mib2_params->sysName.pName     = LSA_NULL;
        mib2_params->sysName.Remanent  = SysNameRema;
        mib2_params->sysContact.Length   = SysContactLen;
        mib2_params->sysContact.pName    = LSA_NULL;
        mib2_params->sysContact.Remanent = SysContactRema;
        mib2_params->sysLocation.Length   = SysLocationLen;
        mib2_params->sysLocation.pName    = LSA_NULL;
        mib2_params->sysLocation.Remanent = SysLocationRema;

        mib2_params->sysDescr.Length   = 0;
        mib2_params->sysDescr.pName    = LSA_NULL;
        mib2_params->sysDescr.Remanent = LSA_FALSE;

        /**************  SysName  ******************/
        if (mib2_params->sysName.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(pSysName);
        }
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysName.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysName-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKUpdateMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysName.Length != 0)
        {
            OHA_MEMCPY(MemPtr, pSysName, mib2_params->sysName.Length);
		}
        mib2_params->sysName.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/

        /**************  SysContact  *****************/
        if (mib2_params->sysContact.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(pSysContact);
        }
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysContact.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysContact-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKUpdateMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysContact.Length != 0)
        {
            OHA_MEMCPY(MemPtr, pSysContact, mib2_params->sysContact.Length);
		}
        mib2_params->sysContact.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/
		
        /**************  SysLocation  *****************/
        if (mib2_params->sysLocation.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(pSysLocation);
        }
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysLocation.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysLocation-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKUpdateMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysLocation.Length != 0)
        {
            OHA_MEMCPY(MemPtr, pSysLocation, mib2_params->sysLocation.Length);
		}
        mib2_params->sysLocation.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/

        /**************  SysDescr  *****************/
		Status = OHA_DB_READ_SYS_DESCR(&mib2_params->sysDescr.pName,
									&mib2_params->sysDescr.Length);
        OHA_ASSERT(Status == LSA_RET_OK);

        if (mib2_params->sysDescr.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(mib2_params->sysDescr.pName);
        }
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysDescr.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysDescr-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKUpdateMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysDescr.Length != 0)
        {
            OHA_MEMCPY(MemPtr, mib2_params->sysDescr.pName, mib2_params->sysDescr.Length);
		}
        mib2_params->sysDescr.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/

        pHDB->Mib2RQBCnt++;
        OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKUpdateMib2Params(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKMib2ParamsInd                       +*/
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
/*+  Description: MIBII params Indication for SysName, SysLocation and      +*/
/*+               SysContact                                                +*/
/*+               Allocates RQB and does SOCK-Request                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKMib2ParamsInd(
    LSA_UINT16              NicID,
    LSA_UINT16              Orginator)
{
    OHA_HDB_SOCK_PTR_TYPE                   pHDB = OHA_SOCKGetHandleByNIC(NicID);
    OHA_SOCK_LOWER_RQB_PTR_TYPE             pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE        mib2_params;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKMib2ParamsInd(NicID: 0x%X)",
                          NicID);

    OHA_ASSERT_NULL_PTR(pHDB);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SOCK_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL;

        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_MIB2_SYSGROUP_DATA_IND);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_MIBII_PARAMS_IND (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

        mib2_params = &pRQB->args.mib2_sysgroup_data_ind;

        mib2_params->sysDescr.Length   = 0;
        mib2_params->sysDescr.pName    = LSA_NULL;
        mib2_params->sysDescr.Remanent = LSA_FALSE;

        mib2_params->sysName.Length   = 0;
        mib2_params->sysName.pName    = LSA_NULL;
        mib2_params->sysName.Remanent = LSA_FALSE;
        mib2_params->sysContact.Length   = 0;
        mib2_params->sysContact.pName    = LSA_NULL;
        mib2_params->sysContact.Remanent = LSA_FALSE;
        mib2_params->sysLocation.Length   = 0;
        mib2_params->sysLocation.pName    = LSA_NULL;
        mib2_params->sysLocation.Remanent = LSA_FALSE;

		/* SysName, SysContact and SysLocation are writeable mib objects */
        /**************  SysName (0...255 Byte) ******************/
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_MAX_CHASSIS_STRLEN, pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysName-memory failed!");
            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKMib2ParamsInd(Status: 0x%X)",Status);
            return(Status);
        }
        mib2_params->sysName.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;

        /**************  SysContact (0...255 Byte) *****************/
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_MAX_CHASSIS_STRLEN, pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysContact-memory failed!");
            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKMib2ParamsInd(Status: 0x%X)",Status);
            return(Status);
        }
        mib2_params->sysContact.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;

        /**************  SysLocation (0...255 Byte) *****************/
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_MAX_CHASSIS_STRLEN, pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysLocation-memory failed!");
            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKMib2ParamsInd(Status: 0x%X)",Status);
            return(Status);
        }
        mib2_params->sysLocation.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
		OHA_COMMON_MEM_U8_PTR_TYPE pName = LSA_NULL;

        mib2_params = &pRQB->args.mib2_sysgroup_data_ind;

        /**************  SysName  ******************/
        (LSA_VOID)OHA_DB_READ_SYS_NAME(&pName, &mib2_params->sysName.Length, 
										&mib2_params->sysName.Remanent);

        if (mib2_params->sysName.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(pName);
            OHA_MEMCPY(mib2_params->sysName.pName, pName, mib2_params->sysName.Length);
        }

        /**************  SysContact  *****************/
        (LSA_VOID)OHA_DB_READ_SYS_CONTACT(&pName, &mib2_params->sysContact.Length, 
											&mib2_params->sysContact.Remanent);

        if (mib2_params->sysContact.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(pName);
            OHA_MEMCPY(mib2_params->sysContact.pName, pName, mib2_params->sysContact.Length);
        }
		
        /**************  SysLocation  *****************/
        (LSA_VOID)OHA_DB_READ_SYS_LOCATION(&pName, &mib2_params->sysLocation.Length, 
											&mib2_params->sysLocation.Remanent);

        if (mib2_params->sysLocation.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(pName);
            OHA_MEMCPY(mib2_params->sysLocation.pName, pName, mib2_params->sysLocation.Length);
        }

		OHA_ASSERT(OHA_IS_NULL(pHDB->pMib2IndRQB));
		pHDB->pMib2IndRQB = pRQB;	/* store the ind rqb */

        pHDB->Mib2RQBCnt++;
        OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKMib2ParamsInd(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKMib2ParamsIndDone                   +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: MIBII params indication (SysName,SysContact and -Location)+*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKMib2ParamsIndDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_BOOL                Reprovide = LSA_TRUE;
    LSA_UINT16              Response  = SOCK_RSP_OK_WITHDRAW;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKMib2ParamsIndDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_MIBII_PARAMS_IND done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

	OHA_ASSERT_NULL_PTR(pHDB->pMib2IndRQB);

    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the data and dont reprovide it. This is the case if we are     */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid data while closing, so CANCEL    */
    /*       may not be enough to catch all rqbs. We have to check the State too */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Recv-Request will be provided in the last */
    /*       State while opening the channel.                                    */

    if ( pHDB->State == OHA_HANDLE_STATE_OPEN || 
		 pHDB->State == OHA_HANDLE_STATE_OPEN_PORT_RUNNING || 
		 pHDB->State == OHA_HANDLE_STATE_OPEN_SOCK_RUNNING)
    {
		switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
		{
			case SOCK_RSP_OK:
				/* ---------------------------------------------*/
				/* Successfully done the request.               */
				/* ---------------------------------------------*/
				Response = OHA_OK;
				break;

			case SOCK_RSP_ERR_SYNTAX:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_ERROR,"SOCK MIBII Indication provide error (0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            case SOCK_RSP_OK_WITHDRAW:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> OHA_HANDLE_STATE_OPEN  */
                /*       while closing!                             */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"SOCK MIBII Indication canceled (0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

			default:
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_MIBII_PARAMS_IND failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
				Response = OHA_ERR_LL;
				break;
		}
	}
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"SOCK MIBII Indication while not in open-state. discard and dont reprovide.");
        Reprovide = LSA_FALSE;
    }

    if (Response == OHA_OK)
    {
		LSA_UINT16	NicID = pHDB->Params.pDetail->Params.Sock.NICId;
		(LSA_VOID)OHA_SOCKDoMib2ParamsInd(NicID, pRQB);
    }

    if ( Reprovide )
    {
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_MIBII_PARAMS_IND (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle); /* set handle */
        OHA_SOCK_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
		pHDB->pMib2IndRQB = LSA_NULL;	/* remove the ind rqb */
        pHDB->Mib2RQBCnt--;  /* number of RQBs pending within SOCK */
	    OHA_SOCKFreeMib2Params(pRQB, pHDB);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKMib2ParamsIndDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKDoMib2ParamsInd                     +*/
/*+  Input/Output          :    LSA_UINT16             NicID                +*/
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
/*+  Description: MIBII params Indication for SysName, SysLocation and      +*/
/*+               SysContact                                                +*/
/*+               Allocates RQB and does SOCK-Request                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKDoMib2ParamsInd(
    LSA_UINT16                   NicID,
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB)
{
    LSA_UINT16                              Status;
    SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE        mib2_params;

	LSA_BOOL SysNameIsTheSame;
	LSA_BOOL SysContactIsTheSame;
	LSA_BOOL SysLocationIsTheSame;
	LSA_BOOL RemaStatus       = LSA_FALSE;
	LSA_BOOL SomethingChanged = LSA_FALSE;

	OHA_COMMON_MEM_U8_PTR_TYPE pName  = LSA_NULL;
	LSA_UINT16                 Length = 0;

	LSA_UINT16 Ret;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKDoMib2ParamsInd(NicID: 0x%X, pRQB: 0x%X)",
                          NicID, pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);
	OHA_ASSERT(OHA_SOCKCheckNICUsed(NicID)); /* must be the (global) SOCK-Nic */

	mib2_params = &pRQB->args.mib2_sysgroup_data_ind;

	/* SysName, SysContact and SysLocation are writeable mib objects */
    OHA_ASSERT_NULL_PTR(mib2_params->sysName.pName);
    OHA_ASSERT_NULL_PTR(mib2_params->sysContact.pName);
    OHA_ASSERT_NULL_PTR(mib2_params->sysLocation.pName);

    OHA_ASSERT_NULL_PTR(mib2_params->sysName.Length     <= OHA_MAX_CHASSIS_STRLEN);
    OHA_ASSERT_NULL_PTR(mib2_params->sysContact.Length  <= OHA_MAX_CHASSIS_STRLEN);
    OHA_ASSERT_NULL_PTR(mib2_params->sysLocation.Length <= OHA_MAX_CHASSIS_STRLEN);

	/* reprovided params are remanent now ? */
	/* read and compare SysName */
    (LSA_VOID)OHA_DB_READ_SYS_NAME(&pName, &Length, &RemaStatus);
    if (Length != 0) OHA_ASSERT_NULL_PTR(pName);

    SysNameIsTheSame = (Length == mib2_params->sysName.Length) ? LSA_TRUE : LSA_FALSE;
    if (SysNameIsTheSame)
    {
         if (OHA_IS_NOT_NULL(pName))
         {
              SysNameIsTheSame = OHA_MEMCMP(mib2_params->sysName.pName, pName, Length);
         }
    }

	if (!SysNameIsTheSame)
	{
		SomethingChanged = LSA_TRUE;
		OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_HIGH,
							 "OHA-Mib2ParamsInd(NICId=0x%X): SysName is different (Length %u and %u).",
							 NicID, Length, mib2_params->sysName.Length);
	}
	if((RemaStatus && !mib2_params->sysName.Remanent) || 
		(!RemaStatus && mib2_params->sysName.Remanent))
	{
		SomethingChanged = LSA_TRUE;
		OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_HIGH,
							 "OHA-Mib2ParamsInd(NICId=0x%X): Rema(SysName) is different (0x%x).",
							 NicID, mib2_params->sysName.Remanent);
	}

	/* read and compare SysContact */
    (LSA_VOID)OHA_DB_READ_SYS_CONTACT(&pName, &Length, &RemaStatus);
    if (Length != 0) OHA_ASSERT_NULL_PTR(pName);
    SysContactIsTheSame = (Length == mib2_params->sysContact.Length) ? LSA_TRUE : LSA_FALSE;
    if (SysContactIsTheSame)
    {
        if (OHA_IS_NOT_NULL(pName))
        {
            SysContactIsTheSame = OHA_MEMCMP(mib2_params->sysContact.pName, pName, Length);
        }
    }
	if (!SysContactIsTheSame)
	{
		SomethingChanged = LSA_TRUE;
		OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_HIGH,
							 "OHA-Mib2ParamsInd(NICId=0x%X): SysContact is different (Length %u and %u).",
							 NicID, Length, mib2_params->sysContact.Length);
	}
	if((RemaStatus && !mib2_params->sysContact.Remanent) || 
		(!RemaStatus && mib2_params->sysContact.Remanent))
	{
		SomethingChanged = LSA_TRUE;
		OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_HIGH,
							 "OHA-Mib2ParamsInd(NICId=0x%X): Rema(SysContact) is different (0x%x).",
							 NicID, mib2_params->sysContact.Remanent);
	}

	/* read and compare SysLocation */
    Status = OHA_DB_READ_SYS_LOCATION(&pName, &Length, &RemaStatus);
    OHA_ASSERT(Status == LSA_RET_OK);
    if (Length != 0) OHA_ASSERT_NULL_PTR(pName);
    SysLocationIsTheSame = (Length == mib2_params->sysLocation.Length) ? LSA_TRUE : LSA_FALSE;
    if (SysLocationIsTheSame)
    {
        if (OHA_IS_NOT_NULL(pName))
        {
            SysLocationIsTheSame = OHA_MEMCMP(mib2_params->sysLocation.pName, pName, Length);
        }
    }
	if (!SysLocationIsTheSame)
	{
		SomethingChanged = LSA_TRUE;
		OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH,
							 "OHA-Mib2ParamsInd(NICId=0x%X): SysLocation is different (Length %u and %u).",
							 NicID, Length, mib2_params->sysLocation.Length);
	}
	if((RemaStatus && !mib2_params->sysLocation.Remanent) || 
		(!RemaStatus && mib2_params->sysLocation.Remanent))
	{
		SomethingChanged = LSA_TRUE;
		OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,
							 "OHA-Mib2ParamsInd(NICId=0x%X): Rema(SysLocation) is different (0x%x).",
							 NicID, mib2_params->sysLocation.Remanent);
	}

	if (SomethingChanged)
	{
		/* update SysName */
		Ret = OHA_DB_WRITE_SYS_NAME(mib2_params->sysName.pName, 
									mib2_params->sysName.Length,
									mib2_params->sysName.Remanent);
		if (Ret != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA-Mib2ParamsInd: OHA_DB_WRITE_SYS_NAME failed (Ret: 0x%X).", Ret);
		}
	
		/* update SysContact */
		Ret = OHA_DB_WRITE_SYS_CONTACT(mib2_params->sysContact.pName, 
										mib2_params->sysContact.Length,
										mib2_params->sysContact.Remanent);
		if (Ret != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA-Mib2ParamsInd: OHA_DB_WRITE_SYS_CONTACT failed (Ret: 0x%X).", Ret);
		}

		/* update SysLocation */
		Ret = OHA_DB_WRITE_SYS_LOCATION(mib2_params->sysLocation.pName, 
										mib2_params->sysLocation.Length,
										mib2_params->sysLocation.Remanent);
		if (Ret != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA-Mib2ParamsInd: OHA_DB_WRITE_SYS_LOCATION failed (Ret: 0x%X).", Ret);
		}

		/* update LLDP send (all ports, all interfaces) */
		if (!SysNameIsTheSame)		/* only this is a LLDP object */
		{
			OHA_LLDPUpdateSendGlobal();
		}

#ifndef OHA_CFG_NO_REMA							/* global NIC */
		(LSA_VOID)oha_rema_snmp_update(NicID);	/* Write to REMA */
#endif
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKDoMib2ParamsInd(Status: 0x%X)", Status);
    return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKGetMib2Status                       +*/
/*+  Input/Output          :    LSA_UINT16                  NicID           +*/
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
/*+  Description: Get Mib2 status of SOCK                                   +*/
/*+               (SOCK_OPC_GET_MIBII_PARAMS)                               +*/
/*+                                                                         +*/
/*+               Allocates RQB and does the SOCK-Request                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKGetMib2Status(
    LSA_UINT16                 NicID,
	LSA_UINT16                 PortId,
    LSA_UINT16                 Orginator)
{
	OHA_HDB_SOCK_PTR_TYPE pSockHDB = OHA_SOCKGetHandleGlobal();
	OHA_HDB_EDD_PTR_TYPE  pHDBEdd  = OHA_EDDGetHandleByNIC(NicID);
    OHA_DB_IF_PTR_TYPE pDB  = OHA_DB_GET_INTERFACE(NicID);

    OHA_SOCK_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE           pSys;
    LSA_UINT16                 Status;
    LSA_USER_ID_TYPE           UserId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKGetMib2Status(NicID: 0x%X, PortId: 0x%X, Orginator: 0x%X)", NicID, PortId, Orginator);

	if (PortId > OHA_CFG_MAX_PORTS) /* 0 ... interface */
	{
        Status = OHA_ERR_PARAM;
        /* Error on ringport ID. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"Request SOCK_OPC_GET_MIBII_PARAMS failed. Port (%d) is wrong!", PortId);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetMib2Status(Status: 0x%X)",Status);
		return(Status);
    }

    /* Check if we have a SOCK-Channel for this NIC-ID.*/
    if (OHA_IS_NULL(pSockHDB))
    {
        Status = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"Request SOCK_OPC_GET_MIBII_PARAMS failed. No SOCK Channel!");
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetMib2Status(Status: 0x%X)",Status);
		return(Status);
    }

    /* Check if we have a EDD-Channel for this NIC-ID.*/
    if (OHA_IS_NULL(pHDBEdd))
    {
        Status = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"Request SOCK_OPC_GET_MIBII_PARAMS failed. No EDD Channel!");
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetMib2Status(Status: 0x%X)",Status);
		return(Status);
    }

	/* is an open/close SOCK-channel runnning ? */
    if (!(pSockHDB->State == OHA_HANDLE_STATE_OPEN) )
    {
        Status = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"Request SOCK_OPC_GET_MIBII_PARAMS failed. SOCK Channel is closing or not open (State: 0x%X)", pSockHDB->State);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetMib2Status(Status: 0x%X)",Status);
		return(Status);
    }

	OHA_ASSERT_NULL_PTR(pDB);

    Status = OHA_OK;
    pSys   = pSockHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Request to SOCK                                         */
    /*---------------------------------------------------------------------------*/

    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
		SOCK_GET_MIB2_IFINDEX_PTR_TYPE pGetMib2Params = &pRQB->args.get_mib2_ifindex;

		OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_GET_MIB2_IFINDEX);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pSockHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

		pGetMib2Params->InterfaceID = pDB->InterfaceID;
		pGetMib2Params->PortID      = PortId;
		pGetMib2Params->IfIndex     = 0;

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_GET_MIBII_PARAMS (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pSockHDB->ThisHandle,pSockHDB->SockHandle,pRQB);

		OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
		pSockHDB->MrpAgent.SnmpRQBCnt++;  /* number of Requests pending within SOCK */
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKGetMib2Status(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKGetMib2StatusDone                   +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE   pRQB          +*/
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
/*+  Description: Get SOCK Mib2 status confirmation                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKGetMib2StatusDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              Orginator;
	OHA_HDB_EDD_PTR_TYPE    pHDBEdd, pTmpHDBEdd;
	OHA_DB_IF_PTR_TYPE      pDB;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKGetMib2StatusDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

	OHA_ASSERT_NULL_PTR(pRQB);
	OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_GET_MIBII_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);
	OHA_ASSERT(OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_EDD);

	pHDBEdd = OHAGetEddHDBFromHandle((LSA_HANDLE_TYPE)Orginator);
	OHA_ASSERT_NULL_PTR(pHDBEdd);

	OHA_ASSERT(pHDBEdd->PendingPortId <= OHA_CFG_MAX_PORTS);

	/* AP01241158: if meanwhile the EDD channel is closed, we cancel SockGetMib2Params */
	/* AP01248054: OHA causes NULL Ptr exception at startup */
	pTmpHDBEdd = OHA_EDDGetHandleByNIC(pHDBEdd->Params.pDetail->Params.Edd.NICId);
	if(OHA_IS_NULL(pTmpHDBEdd))
	{
	    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
	    OHA_MEM_FREE_FAIL_CHECK(RetVal);

		pHDB->MrpAgent.SnmpRQBCnt--;

        /* now SockGetMib2Params for all ports is cancelled */
        pHDBEdd->PendingPortId = 0;

		Response = OHA_OK_CANCEL;
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_SOCKGetMib2StatusDone(Response: 0x%X)", Response);
		return;
	}

	pDB = OHA_DB_GET_INTERFACE(pHDBEdd->Params.pDetail->Params.Edd.NICId);
	OHA_ASSERT_NULL_PTR(pDB);

    switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
			pHDBEdd->Mib2IfIndex[pHDBEdd->PendingPortId] = pRQB->args.get_mib2_ifindex.IfIndex;
			Response = OHA_OK;
            break;

		case SOCK_RSP_ERR_PARAM:
			pHDBEdd->Mib2IfIndex[pHDBEdd->PendingPortId] = 0;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_WARN,"SOCK-ERROR: SOCK_OPC_GET_MIBII_PARAMS failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_FAILED;
            break;

        default:
			pHDBEdd->Mib2IfIndex[pHDBEdd->PendingPortId] = 0;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_GET_MIBII_PARAMS failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    pHDB->MrpAgent.SnmpRQBCnt--;

    /* are all SOCK_OPC_GET_MIBII_PARAMS's already done ? */
    if (pHDBEdd->PendingPortId >= pDB->PortCnt)
    {
        /* now SockGetMib2Params for all ports is done */
        pHDBEdd->PendingPortId = 0;
	}
	else
	{
        pHDBEdd->PendingPortId++;
		(LSA_UINT16)OHA_SOCKGetMib2Status(pHDBEdd->Params.pDetail->Params.Edd.NICId, 
									  pHDBEdd->PendingPortId, pHDBEdd->ThisHandle);
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetMib2StatusDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKSetMib2Params                       +*/
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
/*+  Description: Set the global MIBII params SysName, SysDescr,            +*/
/*+               SysContact and SysLocation                                +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Request                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2Params(
    LSA_UINT16              NicID, /* the if specific NIC */
    LSA_UINT16              Orginator)
{
    OHA_HDB_SOCK_PTR_TYPE                   pHDB = OHA_SOCKGetHandleGlobal();
    OHA_SOCK_LOWER_RQB_PTR_TYPE             pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE        mib2_params;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKSetMib2Params(NicID: 0x%X, Orginator: 0x%X)",
                          NicID, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);
	LSA_UNUSED_ARG(NicID); /* only for trace */

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_SOCK_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL;

        OHA_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_SET_MIB2_SYSGROUP_DATA);
        OHA_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        OHA_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SET_MIBII_PARAMS (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle,pRQB);

        mib2_params = &pRQB->args.set_mib2_sysgroup_data;

        mib2_params->sysName.Length  = 0;
        mib2_params->sysName.pName   = LSA_NULL;
        mib2_params->sysName.Remanent = LSA_FALSE;
        mib2_params->sysDescr.Length = 0;
        mib2_params->sysDescr.pName  = LSA_NULL;
        mib2_params->sysDescr.Remanent = LSA_FALSE;
        mib2_params->sysContact.Length = 0;
        mib2_params->sysContact.pName  = LSA_NULL;
        mib2_params->sysContact.Remanent = LSA_FALSE;
        mib2_params->sysLocation.Length = 0;
        mib2_params->sysLocation.pName  = LSA_NULL;
        mib2_params->sysLocation.Remanent = LSA_FALSE;

        /**************  SysName  ******************/
        Status = OHA_DB_READ_SYS_NAME(&mib2_params->sysName.pName, 
										 &mib2_params->sysName.Length,
										 &mib2_params->sysName.Remanent);
        OHA_ASSERT(Status == LSA_RET_OK);

        if (mib2_params->sysName.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(mib2_params->sysName.pName);
		}
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysName.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysName-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKSetMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysName.Length != 0)
        {
            OHA_MEMCPY(MemPtr, mib2_params->sysName.pName, mib2_params->sysName.Length);
        }
        mib2_params->sysName.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/

        /**************  SysDescr  *****************/
		(LSA_VOID)OHA_DB_READ_SYS_DESCR(&mib2_params->sysDescr.pName,
										&mib2_params->sysDescr.Length);

        if (mib2_params->sysDescr.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(mib2_params->sysDescr.pName);
        }
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysDescr.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysDescr-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKSetMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysDescr.Length != 0)
        {
            OHA_MEMCPY(MemPtr, mib2_params->sysDescr.pName, mib2_params->sysDescr.Length);
        }
        mib2_params->sysDescr.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/

        /**************  SysContact  *****************/
        (LSA_VOID)OHA_DB_READ_SYS_CONTACT(&mib2_params->sysContact.pName, 
										 &mib2_params->sysContact.Length,
										 &mib2_params->sysContact.Remanent);

        if (mib2_params->sysContact.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(mib2_params->sysContact.pName);
        }
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysContact.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysContact-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKSetMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysContact.Length != 0)
        {
            OHA_MEMCPY(MemPtr, mib2_params->sysContact.pName, mib2_params->sysContact.Length);
        }
        mib2_params->sysContact.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/
		
        /**************  SysLocation  *****************/
        (LSA_VOID)OHA_DB_READ_SYS_LOCATION(&mib2_params->sysLocation.pName, 
										  &mib2_params->sysLocation.Length,
										  &mib2_params->sysLocation.Remanent);

        if (mib2_params->sysLocation.Length != 0)
        {
            OHA_ASSERT_NULL_PTR(mib2_params->sysLocation.pName);
        }
		/* AP01430354: For SOCK a NullPointer isn't an empty string */ 
        OHA_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, (LSA_UINT16)(mib2_params->sysLocation.Length+1), pSys);
        if (OHA_IS_NULL(MemPtr))
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower SysLocation-memory failed!");

            OHA_SOCKFreeMib2Params(pRQB, pHDB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_SOCKSetMib2Params(Status: 0x%X)",Status);
            return(Status);
        }

        if (mib2_params->sysLocation.Length != 0)
        {
            OHA_MEMCPY(MemPtr, mib2_params->sysLocation.pName, mib2_params->sysLocation.Length);
        }
        mib2_params->sysLocation.pName = (LSA_UINT8 SOCK_UPPER_MEM_ATTR *)MemPtr;
        /*******************************************/

        pHDB->Mib2RQBCnt++;
        OHA_SOCK_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKSetMib2Params(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKSetMib2ParamsDone                   +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Set global MIBII params SysName, SysDescr,...             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2ParamsDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16              Orginator;
    LSA_UINT16              Response;
	LSA_UINT16              NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKSetMib2ParamsDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);

	NICId = pHDB->Params.pDetail->Params.Sock.NICId;

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_SET_MIBII_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_SET_MIBII_PARAMS failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_SOCKFreeMib2Params(pRQB, pHDB);
    pHDB->Mib2RQBCnt--;

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_SOCK:  /* from SockOpenChannel() - continue with OpenChannel */
            break;
        case OHA_HANDLE_TYPE_LLDP:  /* from UpdateMib2Params, OHA_DB_WRITE_SYS_NAME, OHA_DB_WRITE_SYS_DESCR or OHA_DB_WRITE_IF_DESCR */
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2ParamsDone(Response: 0x%X)", Response);
            return; /* no action, fade out */
        case OHA_HANDLE_TYPE_USER:  /* from OHA_OPC_PRM_WRITE, OHA_OPC_LLDP_WRITE_OPTION */
			/* check for pending user-requests   */
			{
				OHA_HDB_USER_PTR_TYPE pHDBUser;
				LSA_UINT16 If_NICId;

				/* check the handle */
				if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
				{
					OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				}

				pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(Orginator)));
				OHA_ASSERT(pHDBUser);

				/* continue with WriteLldpOption or PrmWrite */
				OHA_SOCKUserSetMib2ParamsPending(pHDBUser, Response);

				/* SockSetMib2Params.Ind during a SockSetMib2Params.Req was igored */
				If_NICId = pHDBUser->Params.pDetail->Params.User.NICId;
				OHA_ASSERT(!OHA_IsAnyLocalSetMib2Active(If_NICId));

				if (OHA_IS_NULL(pHDB->pMib2IndRQB))
				{
					/* reprovide the MIB2-Indication for handling writeable MIB2 parameters */
			        OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: Reprovide SOCK_OPC_MIBII_PARAMS_IND (Handle: 0x%X,SOCKHandle: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle);
					if (OHA_OK != OHA_SOCKMib2ParamsInd(If_NICId, pHDB->ThisHandle))
					{
				        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN,">>>: Could not Reprovide SOCK_OPC_MIBII_PARAMS_IND (Handle: 0x%X,SOCKHandle: 0x%X)",pHDB->ThisHandle,pHDB->SockHandle);
					}
				}
			}
			return;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SOCKSetMib2ParamsDone: Invalid HandleType (0x%X).", OHA_HANDLE_GET_TYPE(Orginator));
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    if (Response == OHA_OK)
    {
		if(OHA_EDDCheckNICUsed(NICId))
		{
			OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
			OHA_ASSERT_NULL_PTR(pDB);

			/* are MIBII params for port(s) to set ? */
			if (pDB->PortCnt)
			{
				Response = OHA_SOCKSetMib2Status(NICId,
												  0,
												  OHA_HANDLE_TYPE_SOCK);
			    if (Response == OHA_OK)
				{
				OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2ParamsDone(Response: 0x%X)", Response);
				return;
				}
			}
		}
    }

    OHA_SOCKChannelDone(pHDB, Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKSetMib2ParamsDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKControlTcipData                     +*/
/*+  Input/Output          :    LSA_UINT16           NicID                  +*/
/*+                        :    LSA_UINT16           Orginator              +*/
/*+                        :    LSA_UINT16           KeepAliveTime          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to set or get keep alive parameters of an IP stack */
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_SOCKControlTcipData(
    LSA_UINT16                      NicID,
    LSA_UINT16                      Orginator,
	LSA_OPCODE_TYPE                 Opcode,  
    LSA_UINT16                      KeepAliveTime
)
{
    LSA_UINT16                Response;
    OHA_HDB_SOCK_PTR_TYPE     pHDBSock;
	
    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKControlTcipData(NicID: 0x%X, Orginator: 0x%X, Opcode: 0x%X, pKeepAliveTime: 0x%X)",
                          NicID, Orginator, Opcode, KeepAliveTime);

    /* Check if we have the SOCK-Channel, only one over all NIC's. */
    pHDBSock  = OHA_SOCKGetHandleGlobal();  /* only SOCK for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBSock,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ/WRITE(KeepAliveTime) failed. No such Lower Channel for SOCK");
    }
    else
    {
	    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQBSock  = LSA_NULL;
	    LSA_USER_ID_TYPE            UserId;

		UserId.uvar32 = 0;
		OHA_SOCK_ALLOC_LOWER_RQB(&pRQBSock,UserId,sizeof(SOCK_RQB_TYPE),pHDBSock->Params.pSys);
		if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBSock, LSA_NULL) )
		{
			OHA_SOCK_RQB_SET_OPCODE(pRQBSock,SOCK_OPC_CNTRL_TIMEPARAMS);
			OHA_SOCK_RQB_SET_HANDLE(pRQBSock,pHDBSock->SockHandle);
			OHA_SOCK_RQB_SET_USERID_UVAR16(pRQBSock,Orginator);

			if (Opcode == OHA_OPC_PRM_WRITE)
			{
				pRQBSock->args.timeparam_rq.timeopt = SOCK_SET_TIMEPARAMS;
				pRQBSock->args.timeparam_rq.keepalivetime     = KeepAliveTime * 1000; /* in ms */
				pRQBSock->args.timeparam_rq.connectiontimeout = OHA_NO_CHANGE_TIME;
				pRQBSock->args.timeparam_rq.retransmitcount   = OHA_NO_CHANGE_TIME;
				pRQBSock->args.timeparam_rq.retranstimeout    = OHA_NO_CHANGE_TIME;
			}
			else if (Opcode == OHA_OPC_PRM_READ)
			{
				pRQBSock->args.timeparam_rq.timeopt = SOCK_GET_TIMEPARAMS;
				pRQBSock->args.timeparam_rq.keepalivetime     = 0;
				pRQBSock->args.timeparam_rq.connectiontimeout = 0;
				pRQBSock->args.timeparam_rq.retransmitcount   = 0;
				pRQBSock->args.timeparam_rq.retranstimeout    = 0;
			}
			else
			{
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			}

			OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_CNTRL_TIMEPARAMS (SOCKHandle: 0x%X,pRQBSock: 0x%X)",pHDBSock->SockHandle,pRQBSock);
			OHA_SOCK_REQUEST_LOWER(pRQBSock,pHDBSock->Params.pSys);

			Response = OHA_OK;
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKControlTcipData: 0x%X)", Response);
			return(Response);
		}
		else
		{
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory for OHA_OPC_TCIP_CONTROL failed!");
			Response = OHA_ERR_RESOURCE;
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKControlTcipData(Request finished: 0x%X)", Response);
	return(Response);
}

/*****************************************************************************/
/*  end of file OHA_SOCK.C                                                   */
/*****************************************************************************/

