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
/*  F i l e               &F: oha_dcp.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA DCP functions                                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_DCP */

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


/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_DCPChannelDone(
    OHA_HDB_DCP_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPCloseChannel(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPCloseChannelDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_dcp_request_lower_done                  +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for DCP-requests.                        +*/
/*+               Fills Error-Structure in NDB with DCP-Response            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DCP_LOWER_IN_FCT_ATTR oha_dcp_request_lower_done(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_DCP_PTR_TYPE   pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN :oha_dcp_request_lower_done(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /* DCP-request always done from an DCP-Channel. !           */

    pHDB     = OHAGetDcpHDBFromHandle(OHA_DCP_RQB_GET_HANDLE(pRQB));

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
    }

    switch (OHA_DCP_RQB_GET_OPCODE(pRQB) )
    {
        case DCP_OPC_OPEN_CHANNEL:
            OHA_DCPOpenChannelDone(pRQB,pHDB);
            break;
        case DCP_OPC_CLOSE_CHANNEL:
            OHA_DCPCloseChannelDone(pRQB,pHDB);
            break;
        case DCP_OPC_SEND_RSP:
            OHA_DCPSendDone(pRQB,pHDB);
            break;
        case DCP_OPC_RECV_IND:
            OHA_DCPRecvDone(pRQB,pHDB);
            break;
        case DCP_OPC_CANCEL_RECV:
            /* Letzter Callback nach CancelRecv.Req() */
            OHA_DCPCancelReceiveDone(pRQB,pHDB);
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_request_lower_done()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPChannelDone                          +*/
/*+  Input                      OHA_HDB_DCP_PTR_TYPE         pHDB           +*/
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
/*+  Description: Handler for Open and Close-Channel requests to OHA for   +*/
/*+               DCP-Channels. This function is called after a lower       +*/
/*+               layer called back the OHA (e.g. after DCP finished his   +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_DCPChannelDone(
    OHA_HDB_DCP_PTR_TYPE       pHDB,
    LSA_UINT16                 Response)
{

    OHA_UPPER_RQB_PTR_TYPE       pRQB;
    LSA_UINT16                   PathStat = LSA_RET_OK;
    LSA_BOOL                     Finish;
    LSA_BOOL                     Release;
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE             pSys;
	OHA_DETAIL_PTR_TYPE          pDetail;
	LSA_UINT16                   NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB,Response);

    Finish = LSA_FALSE; /* dont finish request by default */
    Release = LSA_FALSE; /* dont release PATH-Info by default */

    OHA_ASSERT_NULL_PTR(pHDB);

    pRQB         = pHDB->pOCReq;
    pHDB->pOCReq = LSA_NULL;    /* we finish all requests here. so reset pOCReq */
    /* to signal no more request pending.           */

    Cbf     = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
	pSys    = pHDB->Params.pSys;
	pDetail = pHDB->Params.pDetail;
	NICId   = pHDB->Params.pDetail->Params.Dcp.NICId;

    OHA_ASSERT_NULL_PTR(pRQB);

    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_DCPChannelDone done (NICId: 0x%X,Rsp: 0x%X,State: 0x%X)",NICId,Response,pHDB->DcpState);

    switch (pHDB->DcpState)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a DCP-Open Channel request.                         */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_DCP_RUNNING:
            if ( Response == OHA_OK)
            {
                OHA_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail;

                /* NOTE: Providing Recv-Indication Request must be          */
                /*       the last action before going into State "open".    */
                /*       So only Indciations while in Open-state must be    */
                /*       handled and reprovided!                            */
                pHDB->RxPendingCnt = 0; /* no Rx pending within DCP */

                /* provide DCP-Recv-Requests. Channel.Params.Dcp.NumOfRecv. */
                Response = OHA_DCPRecvProvide(pHDB, pDetail->Params.Dcp.NumOfRecv, 0);
                if ( Response == OHA_OK )
                {
                    /* provide the Ressources for FastStartUp (Hello.Ind) */
                    Response = OHA_DCPRecvProvide(pHDB, pDetail->Params.Dcp.NumOfRecvFSU, DCP_HELLO);
                }

                if ( Response == OHA_OK )
                {
                    /* no more actions needed. Channel is open now. */
                    pHDB->pDB = OHA_DB_GET_INTERFACE(NICId);
                    OHA_ASSERT_NULL_PTR(pHDB->pDB);
                    pHDB->DcpState  = OHA_HANDLE_STATE_OPEN;
                    /* pHDB->RequestState = OHA_DCP_STATE_FILTER_STOP; */ /* initialize */
                    pHDB->QueryState   = OHA_DCP_STATE_QUERY_STOP;  /* initialize */
                }
                else
                {
                    /* Any Recv-Request failed. Cancel Recv-Requests (then CloseChannel) */

                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP Receive Provide request failed. Response: 0x%X", Response);

                    pHDB->OpenResponse = Response; /* save for later */

                    pHDB->DcpState = OHA_HANDLE_STATE_CLOSE_ERROR_DCP_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Closing Channel because of error");

                    /* first cancel the Receive-RQB's, then close the channel !               */
                    /* if ( OHA_DCPCloseChannel(pHDB->DcpHandle,pHDB->Params.pSys) != OHA_OK) */
                    if ( OHA_DCPCancelReceive(pHDB->DcpHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Cancel DCP Receive().");
                        OHA_FATAL(OHA_FATAL_ERR_DCP);
                    }
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open DCP. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }

            Finish = LSA_TRUE;

            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a DCP-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_DCP_RUNNING:

            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot close DCP. Response: 0x%X", Response);
            }

            Finish = LSA_TRUE;
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
        /* we release the HDB. Handle closed now        */
        OHA_DCPReleaseHDB(pHDB);
	}

    if ( Finish )  /* finish the request ? */
    {
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

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPChannelDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPOpenChannel                          +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA DCP-Channel handle                           +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to DCP. The Request will be   +*/
/*+               finished by calling the DCP-Callbackfunction.             +*/
/*+               oha_dcp_request_lower_done() if OHA_OK.                   +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);
    Result = OHA_OK;

    UserId.uvar32 = 0;
    OHA_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_OPEN_CHANNEL);

        pRQB->args.channel.handle_upper  = OHAHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.dcp_request_upper_done_ptr = oha_dcp_request_lower_done;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
        OHA_DCP_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPOpenChannel. Result: 0x%X",
                          Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPOpenChannelDone                      +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB           +*/
/*+                             OHA_HDB_DCP_PTR_TYPE         pHDB           +*/
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
/*+               - save DCP-Channel                                        +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPOpenChannelDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB)
{
    LSA_UINT16       Response   = OHA_OK;
    LSA_UINT16       RetVal     = LSA_RET_OK;
    LSA_UINT16       NICId      = pHDB->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_DCP_RQB_GET_RESPONSE(pRQB));

    if ( OHA_DCP_RQB_GET_RESPONSE(pRQB) == DCP_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Open Channel successful");
        /* save DCP-Channel in HDB */
        pHDB->DcpHandle = pRQB->args.channel.handle;

    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Open Channel failed.[0x%X]",OHA_DCP_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

	if (Response == OHA_OK)
	{
        Response = OHA_EDD_SetFilter(NICId, OHA_SET_FILTER, OHA_HANDLE_TYPE_SYS);  /* action is SET_FILTER */
        if (Response != OHA_OK)
        {
			pHDB->RequestState = OHA_DCP_STATE_FILTER_STOP;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Set Frame Filter for DCP Open Channel(Response: 0x%X).", Response);

			/* close the channel */
			pHDB->DcpState = OHA_HANDLE_STATE_CLOSE_DCP_RUNNING;
			if ( OHA_DCPCloseChannel(pHDB->DcpHandle,pHDB->Params.pSys) != OHA_OK)
			{
				OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close DCP Channel().");
				OHA_FATAL(OHA_FATAL_ERR_DCP);
			}
        }
		else
		{
			pHDB->RequestState = OHA_DCP_STATE_FILTER_WF_START;
		}
	}
	else
	{
	    OHA_DCPChannelDone(pHDB,Response); /* further actions taken in channel handler */
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPOpenChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPCloseChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       DCPHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  DCPHandle            : DCP handle to close                             +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to DCP. The Request will be  +*/
/*+               finished by calling the DCP-Callbackfunction.             +*/
/*+               oha_dcp_request_lower_done()                              +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPCloseChannel(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys)
{
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB   = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPCloseChannel(DCPHandle: 0x%X)",
                          DCPHandle);

    UserId.uvar32 = 0;
    OHA_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {

        OHA_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_CLOSE_CHANNEL);
        OHA_DCP_RQB_SET_HANDLE(pRQB,DCPHandle);

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_CLOSE_CHANNEL (DCPHandle: 0x%X,pRQB: 0x%X)",DCPHandle,pRQB);
        OHA_DCP_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPCloseChannel(Result:  0x%X)", Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPCloseChannelDone                     +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                             OHA_HDB_DCP_PTR_TYPE        pHDB            +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPCloseChannelDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response = OHA_OK;
    LSA_UINT16               RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_DCP_RQB_GET_RESPONSE(pRQB));

    if ( OHA_DCP_RQB_GET_RESPONSE(pRQB) == DCP_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Close Channel successful");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Close Channel failed.[0x%X]",OHA_DCP_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    /* Could all allocated Response- and Receive-RQBs be freed ? */
    OHA_ASSERT(pHDB->RxPendingCnt == 0);
    OHA_ASSERT(pHDB->TxPendingCnt == 0);

    OHA_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_DCPChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPCloseChannelDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPRecvProvide                          +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                             LSA_UINT32                   Cnt            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Cnt                  : Number of Recv-Request to send to DCP           +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends Indication-Resources to DCP                         +*/
/*+               (dcp_OPC_RECV_IND)                                        +*/
/*+                                                                         +*/
/*+               Allocates RQB and does DCP-Requests                       +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe RECV-Request pending     +*/
/*+                     within DCP!                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPRecvProvide(
    OHA_HDB_DCP_PTR_TYPE     pHDB,
    LSA_UINT32               Cnt,
    LSA_UINT8                ServiceId)
{
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB   = LSA_NULL;
    OHA_DCP_LOWER_MEM_PTR_TYPE   MemPtr = LSA_NULL;  /* Pointer to TLV-Frame memory. */
    LSA_SYS_PTR_TYPE             pSys;
    LSA_UINT16                   Status;
    LSA_USER_ID_TYPE             UserId;
    LSA_UINT16                   RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPRecvProvide(pHDB: 0x%X, Cnt: 0x%X, ServiceId: 0x%X)",
                          pHDB,Cnt,ServiceId);

    OHA_ASSERT_NULL_PTR(pHDB);

    Status  = OHA_OK;
    pSys   =  pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Receive-Request to DCP                                  */
    /*---------------------------------------------------------------------------*/

    while ((Status == OHA_OK) && (Cnt))
    {
        /* allocate RQB */
        OHA_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
        {
            OHA_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_RECV_IND);
            OHA_DCP_RQB_SET_HANDLE(pRQB,pHDB->DcpHandle);
            DCP_UPPER_RQB_RECV_IND_ADDR_PTR_SET(pRQB, LSA_NULL);
            DCP_UPPER_RQB_RECV_IND_DATA_PTR_SET(pRQB, LSA_NULL);

            OHA_DCP_ALLOC_LOWER_MEM(&MemPtr, UserId,
                                    DCP_MAX_REQ_DATA_SIZE,  /* MAX_TLV_DATA_SIZE */
                                    pSys);
            if ( LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
            {
                Status = OHA_ERR_RESOURCE;
                OHA_DCP_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_DCPRecvProvide(). Allocation TLV memory failed.");
            }
            else
            {
                /* ...set allocated datablock to the Receive-RB */
                DCP_UPPER_RQB_RECV_IND_DATA_PTR_SET(pRQB, (DCP_USR_BYTE_PTR)MemPtr);

                OHA_DCP_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_MAC_ADDR_SIZE, pSys);
                if ( LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
                {
                    Status = OHA_ERR_RESOURCE;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_DCPRecvProvide(). Allocation ADDR memory failed.");
                    MemPtr = DCP_UPPER_RQB_RECV_IND_DATA_PTR_GET(pRQB);

                    OHA_DCP_FREE_LOWER_MEM(&RetVal,MemPtr,pSys);
                    OHA_MEM_FREE_FAIL_CHECK(RetVal);
                    OHA_DCP_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
                    OHA_MEM_FREE_FAIL_CHECK(RetVal);
                }
                else
                {
                    /* ...set address to buffer of the datablock */
                    DCP_UPPER_RQB_RECV_IND_ADDR_PTR_SET(pRQB, (DCP_USR_BYTE_PTR)MemPtr);
                    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_RECV_IND (Handle: 0x%X,DCPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DcpHandle,pRQB);

                    DCP_UPPER_RQB_RESP_REQ_SERVICE_ID_SET(pRQB, ServiceId);

                    OHA_DCP_REQUEST_LOWER(pRQB,pSys);
                    pHDB->RxPendingCnt++;  /* number of Rx-Request pending within DCP */
                }
            }
        }
        else
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower RQB-memory failed!");
        }
        Cnt--;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPRecvProvide(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPRecvDone                             +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB           +*/
/*+                             OHA_DCP_HDB_PTR_TYPE         pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: DCP Recv-Request done (Indication received)               +*/
/*+                                                                         +*/
/*+               Copy received frame infos and content to the DataBase     +*/
/*+               within global management (Frame) for further handling.    +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides Receveive-Request to DCP-Server                +*/
/*+                                                                         +*/
/*+               If Recv-Request was canceled we free the RQB/Frame and    +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPRecvDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB)
{
    /* LSA_UINT16                               Response  = OHA_OK; */
    LSA_UINT16                                  RetVal;
    LSA_BOOL                                    Reprovide = LSA_TRUE;
    /* OHA_DCP_PCTRL_PTR_TYPE                   pPCtrl;             */
    /* OHA_COMMON_MEM_U8_PTR_TYPE               pTemp;              */
    DCP_RQB_ARGS_RECV_IND DCP_UPPER_MEM_ATTR *  pRecv;
    LSA_UINT16                                  NicID;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCP_RecvDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

	NicID = pHDB->Params.pDetail->Params.Dcp.NICId;

    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_RECV_IND done (Handle: 0x%X,Rsp: 0x%X,State: 0x%X)",pHDB->ThisHandle,OHA_DCP_RQB_GET_RESPONSE(pRQB),pHDB->DcpState);

    pRecv = &pRQB->args.RecvInd;

    /* if we are in DCP-OPEN-State and a User-Channel is opened, we handle the   */
    /* Request. If not we are not open and                                       */
    /* we discard the frame and dont reprovide it. This is the case if we are    */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid frame while closing, so CANCEL   */
    /*       may not be enough to catch all rqbs. We have to check the State too */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Recv-Request will be provided in the last */
    /*       State while opening the channel.                                    */

    if ( pHDB->DcpState == OHA_HANDLE_STATE_OPEN )
    {
        /* are there DCP Users ? */
        if (OHA_GET_GLOB_PTR()->UsedUserCnt)
        {
            switch (DCP_UPPER_RQB_RESPONSE_GET(pRQB))
            {
                case DCP_RSP_OK:
                    /* ---------------------------------------------*/
                    /* Successfully done the request.               */
                    /* ---------------------------------------------*/
                    switch (pRecv->ServiceId)
                    {
                            /* handle the remote request */
                        case DCP_IDENTIFY:   /* 5 */
                            RetVal = oha_dcp_Identify(pRQB, pHDB);
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"a DCP_IDENTIFY is done (RetVal: 0x%X)", RetVal);
                            break;
                        case DCP_GET:       /* 3 */
                            RetVal = oha_dcp_Get(pRQB, pHDB);
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"a DCP_GET is done (RetVal: 0x%X)", RetVal);
                            break;
                        case DCP_SET:       /* 4 */
                        {
                            OHA_COMMON_MEM_U8_PTR_TYPE   IpSuitePtr = 0;

                            if (pHDB->QueryState != OHA_DCP_STATE_QUERY_STOP)  /* active User query for a DcpSet response */
                            {                  /* the user has to respond a DcpSet    */
                                /* meanwhile a further DcpSet received - REJECT    */
                                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"a previous DCP-SET is to respond by the User. Reject and reprovide.");
                                break;
                            }
                            pHDB->QueryState = OHA_DCP_STATE_QUERY_SCAN;       /* scanning the frame */

                            /* delete temporary objects before parsing */
                            (LSA_VOID)oha_db_delete_temp_ip_address(NicID);
                            (LSA_VOID)oha_db_delete_temp_station_name(NicID); /* AP01337555, AP01348107 */

							/* init the check options */
							OHA_MEMSET(&pHDB->CheckSetInd, 0, sizeof(OHA_DCP_INDICATION_TYPE));

							RetVal = oha_dcp_ParseSet(pRQB, pHDB, &pHDB->CheckSetInd); /* are there parameters to set ? */
				            if (RetVal == OHA_ERR_PROTOCOL)       /* TIA 743037: malformed packet */
							{
			                    pHDB->QueryState = OHA_DCP_STATE_QUERY_STOP;   /* reset state */
								break;
							}

                            switch (pHDB->CheckSetInd.Event)
                            {
                                case OHA_MODE_NO_SET:       /* an OHA or USER controlled Dcp-Set, only respond the remote server negative */
                                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"an Error occurred (Mode: 0x%X, Reason: 0x%X)", pHDB->CheckSetInd.Event, pHDB->CheckSetInd.Reason);
                                    break;
                                case OHA_EVENT_REMOTE_SET:          /* an OHA controlled Dcp-Set */
                                case OHA_MODE_EMERGENCY_SET:        /* an OHA controlled Dcp-Set as emergency Set */
                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"an OHA controlled DCP_SET is to do (Mode: 0x%X)", pHDB->CheckSetInd.Event);

									/* AP00846402: In case of a Reset to Factory via DCP the OHA-database (NameOfStation, IP address) is not deleted */
									if ( (pHDB->CheckSetInd.Options & OHA_CTRL_RESET_TO_FACTORY) ||
										 (pHDB->CheckSetInd.Options & OHA_CTRL_RESET_COMMUNICATION) )
									{
										/* AP01337555, AP01348107: No further checks */
										/* now we have an asynchronous call to SOCK */
										RetVal = OHA_SOCKSetIpAddress(NicID, DCP_UPPER_RQB_HANDLE_GET(pRQB),
																		(OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
																		(OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
																		(OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
																		LSA_FALSE);
										if (RetVal != OHA_OK)
										{
											(LSA_VOID)oha_db_delete_temp_ip_address(NicID);
											/* response must be negative, we will take the negative default reason code OHA_DCP_REASON_DS_NOT_SET */
											pHDB->CheckSetInd.Event  = OHA_MODE_NO_SET;  /* No set of parameters, only respond the remote dcp-server */
											pHDB->CheckSetInd.Reason = OHA_DCP_REASON_DS_NOT_SET;  /* we take the negative default value, not OK ! */
											OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_WARN,"DCP-Set(ResetToFactory) of Set-IP to 0.0.0.0 failed!");
											break;
										}
										else
										{
                                            pHDB->pLowerDcpSetRqb = pRQB;  /* store this LowerRqb, respond a DcpSet by SockSetIp.Cnf */
                                            pHDB->RxPendingCnt--;   /* number of Recv-Request pending within DCP */
                                            OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPRecvDone() - ResetToFactory active for DCP_SET");
                                            return;
										}
									}
                                    /* if the option IP-Suite is set there must have been stored a temporary IpAddress in oha_dcp_ParseSet() */
                                    if (pHDB->CheckSetInd.Options & OHA_CTRL_IP_SUITE)  /* ip address is to set */
                                    {
                                        (LSA_VOID)oha_db_read_temp_ip_address(NicID, &IpSuitePtr, LSA_NULL, LSA_NULL);

                                        if (OHA_IS_NOT_NULL(IpSuitePtr))
                                        {
                                            /* we have an asynchronous call to SOCK */
                                            RetVal = OHA_SOCKSetIpAddress(NicID, DCP_UPPER_RQB_HANDLE_GET(pRQB),
                                                                          IpSuitePtr, IpSuitePtr+OHA_IP_ADDR_SIZE, IpSuitePtr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,
                                                                          (LSA_BOOL)((pHDB->CheckSetInd.Event == OHA_MODE_EMERGENCY_SET) ? LSA_TRUE : LSA_FALSE));
                                            if (RetVal != OHA_OK)   /* local SockSetIpAddress fails */
                                            {
                                                (LSA_VOID)oha_db_delete_temp_ip_address(NicID);
												/* response must be negative, we will take the negative default reason code OHA_DCP_REASON_DS_NOT_SET */
												pHDB->CheckSetInd.Event  = OHA_MODE_NO_SET;  /* No set of parameters, only respond the remote dcp-server */
												pHDB->CheckSetInd.Reason = OHA_DCP_REASON_DS_NOT_SET;  /* we take the negative default value, not OK ! */
												OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_WARN,"DCP-Set of Set-IP failed!");
												break;
	                                        }
                                            else
                                            {
                                                pHDB->pLowerDcpSetRqb = pRQB;  /* store this LowerRqb, respond a DcpSet by SockSetIp.Cnf */
                                                pHDB->RxPendingCnt--;   /* number of Recv-Request pending within DCP */
                                                OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPRecvDone() - SockSetIp active for DCP_SET");
                                                return;
                                            }
                                        }
                                    }

#ifndef OHA_CFG_NO_NARE
                                    {
                                        /* cancel running check requests */
                                        LSA_UINT32 CheckOptions = (pHDB->CheckSetInd.CheckIpEnabled) ? OHA_CTRL_IP_SUITE : 0;
                                        LSA_BOOL Finish = OHA_NARECheckAddressCancel(NicID,
                                         pHDB->CheckSetInd.Options | CheckOptions,
                                         pHDB->ThisHandle);
                                        if (!Finish)
                                        {
                                            pHDB->pLowerDcpSetRqb = pRQB;  /* store this LowerRqb, respond a DcpSet by SockSetIp.Cnf */
                                            pHDB->RxPendingCnt--;   /* number of Recv-Request pending within DCP */
                                            OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPRecvDone() - SockSetIp active for DCP_SET");
                                            return;
                                        }
                                    }
#endif
                                    break;
                                case OHA_EVENT_QUERY_SET:   /* user-query for a DcpSet (Query-Set) */
                                    OHA_ASSERT(pHDB->QueryState == OHA_DCP_STATE_QUERY_SCAN);
                                    pHDB->QueryState = OHA_DCP_STATE_QUERY_USER;  /* a user query is active */
                                    pHDB->pLowerDcpSetRqb = pRQB;  /* store this LowerRqb, respond a DcpSet by the user */
                                    pHDB->RxPendingCnt--;   /* number of Recv-Request pending within DCP */
                                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPRecvDone() - USER Query for DCP_SET");
                                    return;
#if 0
                                case OHA_MODE_SET:          /* a USER controlled Dcp-Set */
                                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"a USER controlled DCP_SET is done");
                                    break;
#endif
                                default:
                                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"invalid DCP_SET option");
                                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                                    break;
                            }

                            /* AP00xxxxxx: Wait for DCP-Set.Con for calling oha_dcp_confirm_remote_set() */
                            RetVal = oha_dcp_Set(pRQB, pHDB, &pHDB->CheckSetInd);
                            if (RetVal != LSA_RET_OK) /* could not respond, no resource */
                            {
                                pHDB->QueryState = OHA_DCP_STATE_QUERY_STOP;   /* reset state */
                            }
                        }
                        break;
                        case DCP_HELLO:   /* 6 */
                            RetVal = oha_dcp_Hello(pRQB, pHDB);
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"a DCP_HELLO is done (RetVal: 0x%X)", RetVal);
                            break;
                        default:        /* service not supported - ignore   */
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"DCP Receive Service not supported (0x%X).",pRecv->ServiceId);
                            break;
                    }
                    break;

                case DCP_RSP_OK_CANCEL:
                    /* we canceled the request (e.g. on channel close ) */
                    /* Note: typically we dont get here, because the    */
                    /*       channel state is <> OHA_HANDLE_STATE_OPEN  */
                    /*       while closing!                             */

                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"OHA Receive canceled (0x%X).",DCP_UPPER_RQB_RESPONSE_GET(pRQB));
                    Reprovide = LSA_FALSE;
                    break;

                    /* Eine DCP-PDU mit unbekannten Protokollelementen wurde emp-fangen. */
                case DCP_RSP_ERR_PROTOCOL:
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_FATAL,"OHA-ERROR: OHA_OPC_RECV_IND failed.(0x%X).",DCP_UPPER_RQB_RESPONSE_GET(pRQB));
                    break;

                default:
                    /* DCP_RSP_ERR_RESSOURCE    No Timer, lower RQB or memory available */
                    /* DCP_RSP_ERR_LOWER_LAYER  Error occurred at lower Receive */
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA-ERROR: OHA_OPC_RECV_IND failed.(0x%X).",DCP_UPPER_RQB_RESPONSE_GET(pRQB));
                    break;
            }
        }
        else
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"DCP Receive while in open-state but without any user. discard and reprovide.");
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,"DCP Receive while not in open-state. discard and dont reprovide.");
        Reprovide = LSA_FALSE;
    }

    if ( Reprovide )
    {
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: OHA_OPC_RECV_IND (Handle: 0x%X,DCPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DcpHandle,pRQB);
        OHA_DCP_RQB_SET_HANDLE(pRQB,pHDB->DcpHandle); /* set handle */

        OHA_DCP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        pHDB->RxPendingCnt--;  /* number of Recv-Request pending within DCP */

        oha_dcp_free_rcv_rqb(pHDB, pRQB);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPRecvDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPCancelReceive                        +*/
/*+  Input                      LSA_HANDLE_TYPE       DCPHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  DCPHandle            : DCP handle to calcel receive                    +*/
/*+  pSys                 : Sys-Ptr for cancel receive lower                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Cancel Receive Request to DCP. The Request will be +*/
/*+               finished by calling the DCP-Callbackfunction.             +*/
/*+               oha_dcp_request_lower_done()                              +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple close request at a time.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPCancelReceive(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys)
{
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB    = LSA_NULL;
    LSA_UINT16                  Result  = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPCancelReceive(DCPHandle: 0x%X)",
                          DCPHandle);

    UserId.uvar32 = 0;
    OHA_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {

        /* Get back all Receive-RQBs with CANCEL,
         * continue with the Callback... */
        DCP_UPPER_RQB_CANCEL_RECV_COUNT_SET(pRQB, 0);
        OHA_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_CANCEL_RECV);
        OHA_DCP_RQB_SET_HANDLE(pRQB,DCPHandle);

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_CANCEL_RECV (DCPHandle: 0x%X,pRQB: 0x%X)",DCPHandle,pRQB);
        OHA_DCP_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPCancelReceive()");
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPCancelReceiveDone                    +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                             OHA_HDB_DCP_PTR_TYPE        pHDB            +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPCancelReceiveDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB)
{
    LSA_UINT16       RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPCancelReceiveDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_CANCEL_RECV done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_DCP_RQB_GET_RESPONSE(pRQB));

    switch (OHA_DCP_RQB_GET_RESPONSE(pRQB))
    {
        case DCP_RSP_OK:
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Cancel Receive successful");
            break;
        case DCP_RSP_ERR_PARAM:
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"No DCP-Cancel Receives found");
            break;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Cancel Receive failed.[0x%X]",OHA_DCP_RQB_GET_RESPONSE(pRQB));
            break;
    }

    OHA_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    /* Could all allocated Response- and Receive-RQBs be freed ? */
    /* OHA_ASSERT(pHDB->RxPendingCnt == 0); */ /* moved to close-channel */
    /* OHA_ASSERT(pHDB->TxPendingCnt == 0); */

	/* EDDx: automatically deactivation of a DCP-Filter at LSA-Close-Channel */
    /* close the channel immediate */
    if ( OHA_DCPCloseChannel(pHDB->DcpHandle,pHDB->Params.pSys) != OHA_OK)
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close DCP Channel().");
        OHA_FATAL(OHA_FATAL_ERR_DCP);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPCancelReceiveDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPSendDone                             +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                             OHA_HDB_DCP_PTR_TYPE        pHDB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Send-Response done handling.                              +*/
/*+                                                                         +*/
/*+              - frees RQB memory                                         +*/
/*+              - call global state handling for further actions           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPSendDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB)
{
    LSA_UINT16      Response;
    LSA_INT         ServiceId;
    LSA_UINT16      NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPSendDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    NICId     = pHDB->Params.pDetail->Params.Dcp.NICId;
    ServiceId = DCP_UPPER_RQB_RESP_REQ_SERVICE_ID_GET(pRQB);
    Response  = OHA_DCP_RQB_GET_RESPONSE(pRQB);

    /* AP00xxxxxx: Waited for DCP-Set.Con for calling OHA_REPORT_DCP_SET_RES() and oha_dcp_confirm_remote_set() */
    if (ServiceId == DCP_SET)
    {
        OHA_UPPER_DCP_INDICATION_PTR_TYPE pSetInd = &pHDB->CheckSetInd;

        if (pSetInd->Options) /* parameters set ? */ /* changes made ? */
        {
            LSA_UINT8 Event = pSetInd->Event; /* store event for OHA_REPORT_DCP_SET_RES */

            /* Signalize the end of the SET-Requests: DCP-Indication (EventSet), Rtf included */
            OHA_REPORT_DCP_SET_RES(NICId, *pSetInd, DCP_UPPER_RQB_RESP_REQ_DATA_PTR_GET(pRQB), DCP_MAX_RSP_DATA_SIZE);
            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                                  "OUT: oha_dcp_Set-Report (ChangeOf: 0x%X, Option: 0x%X, SubOption: 0x%X)",
                                  pSetInd->Options, pSetInd->Type, pSetInd->SubType);
            pSetInd->Event  = Event;
        }

		/* AP00846402: In case of a Reset to Factory via DCP the OHA-database (NameOfStation, IP address) is not deleted */
        switch (pHDB->QueryState)
        {
            case OHA_DCP_STATE_QUERY_SCAN: /* an OHA controlled DCP-Set */
                oha_dcp_confirm_remote_set(pHDB, &pHDB->CheckSetInd);
                pHDB->QueryState = OHA_DCP_STATE_QUERY_STOP;   /* reset state */
                break;

            case OHA_DCP_STATE_QUERY_USER: /* a user query (OhaOpcDcpResponse) is running */
                OHA_ASSERT_NULL_PTR(pHDB->pUpperRespRqb);
                OHA_FinishDcpResponse(pHDB, pHDB->pUpperRespRqb, OHA_OK);
                break;

            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP_SET send state failed (QueryState: 0x%X).", pHDB->QueryState);
                break;
        }
    }

    /* in case of a send response error (DCP_OPC_SEND_RSP, Response!=DCP_RSP_OK) */
    /* a remote request from a DCP client could not be responded - no assertion  */
    /* DCP_RSP_OK           No error                                            */
    /* DCP_RSP_ERR_PARAM    Invalid parameter (e.g. sending list too long) */
    /* DCP_RSP_ERR_RESSOURCE    No lower RQB or memory available           */
    /* DCP_RSP_ERR_LOWER_LAYER  Error occurred at lower Sending            */
    /* OHA_ASSERT (DCP_RSP_OK == Response); */
    if (DCP_RSP_OK != Response)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP_SET send request failed (ServiceId: 0x%X, Response: 0x%X).", ServiceId, Response);
    }

    oha_dcp_free_rsp_rqb(pHDB, pRQB);
    pHDB->TxPendingCnt--;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPSendDone(Status: 0x%X)",Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDUpdateFilter                         +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+                             LSA_UINT16       ControlStatus              +*/
/*+                             LSA_UINT16       Orginator                  +*/
/*+  Result                :    LSA_UINT16       OHA_OK, OHA_ERR_LL         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if there is a change of NameOfStation. Update the  +*/
/*+               filter data of DCP if DCD-filter is active                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPUpdateFilter(
    LSA_UINT16 NICId,
    LSA_UINT32 ControlStatus,
    LSA_UINT16 Orginator)
{
    LSA_UINT16  Response = OHA_OK;
    /* Check if we have a DCP-Channel for NIC-ID.*/
    OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NICId);
    OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPUpdateFilter(NICId: 0x%X,ControlStatus: 0x%X,Orginator: 0x%X)",
                          NICId,ControlStatus,Orginator);

    /* update the DCP-filter if DCP-filter is active */
    if (ControlStatus & OHA_CTRL_STATION_NAME)
    {
        if (OHA_IS_NOT_NULL(pHDBEdd) && OHA_IS_NOT_NULL(pHDBDcp) && 
			(pHDBDcp->RequestState == OHA_DCP_STATE_FILTER_START)) /* DCP-Filter is active ? */
        {
			OHA_EDD_LOWER_RQB_PTR_TYPE pBlock = pHDBEdd->LowerDcpFilterRQBs.pBottom;

			/* an update (set) filter is only called if no queued request exists,   */
			/* otherwies the queued request will update the filter by the execution */
			if(OHA_IS_NOT_NULL(pBlock)) /* AP01022055, AP01256689 */
			{
				LSA_UINT32 BlockCount;
				for (BlockCount = 0; OHA_IS_NOT_NULL(pBlock); BlockCount++)
				{
					pBlock = OHA_RQB_GET_NEXT_RQB_PTR(pBlock);
				}
				OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Filter-Queue-Update-Details (pHDBEdd: 0x%X, Orginator: 0x%X, QueuekCount: 0x%X)",pHDBEdd,Orginator,BlockCount);
			}
			else
			{
	            Response = OHA_EDD_SetFilter(NICId, OHA_SET_FILTER, Orginator);  /* action is SET_FILTER */
			}
        }
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPUpdateFilter(Response: 0x%X)",Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadQueryElements                +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                        :    OHA_HDB_PTR_TYPE            pHDB            +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_READ_QUERY_ELEMENTS    +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.QueryElem                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read actual parameters from the dcp-set frame  +*/
/*+                                                                         +*/
/*+               Fills RQB with station data and confirms request.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadQueryElements(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    OHA_HDB_DCP_PTR_TYPE      pHDBDcp;
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId;
    OHA_QUERY_ELEM_PTR_TYPE   pQueryElement;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadQueryElements(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

    pQueryElement = &pRQB->args.QueryElem;
    NICId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID and the params. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId); /* EDD and DCP channel for this request necessary */
    pHDBDcp  = OHA_DCPGetHandleByNIC(NICId);

    if (LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL) ||
        LSA_HOST_PTR_ARE_EQUAL(pHDBDcp, LSA_NULL) ||
        LSA_HOST_PTR_ARE_EQUAL(pQueryElement->pElements, LSA_NULL) ||
        pQueryElement->Count == 0)
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_READ_ELEMENT failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else if ( (pHDBDcp->QueryState != OHA_DCP_STATE_QUERY_USER) ||
              (pHDBDcp->DcpState != OHA_HANDLE_STATE_OPEN ||
               LSA_HOST_PTR_ARE_EQUAL(pHDBDcp->pLowerDcpSetRqb, LSA_NULL)) )
    {
        /* no DCP-Channel opened or no lower RQB to reprovide */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_READ_ELEMENT failed. No DCP-Channel opened or DCP-Indication present (NIC-ID: 0x%X)",NICId);
    }
    else
    {
        (LSA_VOID)oha_dcp_QuerySet(pHDBDcp->pLowerDcpSetRqb, pHDBDcp, pQueryElement); /* are there parameters to set ? */
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadQueryElements(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestHelloIndication                  +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_HELLO_INDICATION       +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error       Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE       args.HelloInd                               +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide Hello Indication Resources             +*/
/*+                                                                         +*/
/*+               Queues the RQB wthin User-Handlemanagement                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestHelloIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestHelloIndication(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);

    /* queue the request */
    OHA_RQB_PUT_BLOCK_TOP(pHDBUser->HelloIndReq.pBottom,
                          pHDBUser->HelloIndReq.pTop,
                          pRQB);

    pHDBUser->HelloIndReqPending++;

    /* if this is the first Indication Resource, no check if some */
    /* indications outstanding ! */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestHelloIndication(HelloIndReqPending: 0x%X)", pHDBUser->HelloIndReqPending);
}

/*****************************************************************************/
/*  end of file OHA_DCP.C                                                    */
/*****************************************************************************/

