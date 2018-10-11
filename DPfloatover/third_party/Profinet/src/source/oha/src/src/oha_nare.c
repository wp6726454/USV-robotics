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
/*  F i l e               &F: oha_nare.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA NARE functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  8
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_NARE */

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

#ifndef OHA_CFG_NO_NARE

LSA_VOID OHA_LOCAL_FCT_ATTR oha_nare_free_resolve_rqb(OHA_HDB_NARE_PTR_TYPE pHDB,
                                                      OHA_NARE_LOWER_RQB_PTR_TYPE pRQB);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_nare_request_lower_done                 +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for NARE-requests.                       +*/
/*+               Fills Error-Structure in NDB with NARE-Response           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_NARE_LOWER_IN_FCT_ATTR oha_nare_request_lower_done(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_NARE_PTR_TYPE   pHDB;
    LSA_UINT16              Orginator;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN :oha_nare_request_lower_done(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);
    Orginator = OHA_NARE_RQB_GET_USERID_UVAR16(pRQB);

    /* NARE-request always done from an NARE-Channel. !         */

    pHDB     = OHAGetNareHDBFromHandle(OHA_NARE_RQB_GET_HANDLE(pRQB));

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
    }

    switch (OHA_NARE_RQB_GET_OPCODE(pRQB) )
    {
        case NARE_OPC_OPEN_CHANNEL:
            OHA_NAREOpenChannelDone(pRQB,pHDB);
            break;
        case NARE_OPC_CLOSE_CHANNEL:
            OHA_NARECloseChannelDone(pRQB,pHDB);
            break;
            /* check IP */
        case NARE_OPC_TEST_IP:
            switch (OHA_HANDLE_GET_TYPE(Orginator))
            {
                    /* from the USER (OhaOpcResolveAddress) */
                case OHA_HANDLE_TYPE_USER:
                    OHA_NARETestIpDone(pRQB,pHDB);
                    break;
                    /* from cyclic check IP (internal, see OHA_RequestCyclicTimer()) */
                case OHA_HANDLE_TYPE_NARE:
                    OHA_NARETestCyclicIpDone(pRQB,pHDB);
                    break;
                    /* from OHA himself (OhaOpcSetAddress, OhaOpcDcpResponse) */
                case OHA_HANDLE_TYPE_SYS:
                    OHA_NARECheckIpDone(pRQB,pHDB);
                    break;
                default:
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    break;
            }
            break;

        case NARE_OPC_TEST_IP_CANCEL:
            OHA_NARETestIpCancelDone(pRQB,pHDB);
            break;

        case NARE_OPC_HELLO:
        case NARE_OPC_HELLO_STOP:
            OHA_NAREHelloDone(pRQB,pHDB);
            break;

        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_nare_request_lower_done()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREChannelDone                         +*/
/*+  Input                      OHA_HDB_NARE_PTR_TYPE        pHDB           +*/
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
/*+               NARE-Channels. This function is called after a lower      +*/
/*+               layer called back the OHA (e.g. after NARE finished his   +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_NAREChannelDone(
    OHA_HDB_NARE_PTR_TYPE       pHDB,
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
                          "IN : OHA_NAREChannelDone(pHDB: 0x%X, Response: 0x%X)",
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
	NICId   = pHDB->Params.pDetail->Params.Nare.NICId;

    OHA_ASSERT_NULL_PTR(pRQB);

    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_NAREChannelDone done (NICId: 0x%X,Rsp: 0x%X,State: 0x%X)",NICId,Response,pHDB->State);

    switch (pHDB->State)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a NARE-Open Channel request.                        */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_NARE_RUNNING:
            if ( Response == OHA_OK)
            {
                /* OHA_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail; */

                /************************************************/
                /* no more actions needed. Channel is open now. */
                /************************************************/
                pHDB->pDB = OHA_DB_GET_INTERFACE(NICId);
                OHA_ASSERT_NULL_PTR(pHDB->pDB);
                pHDB->State  = OHA_HANDLE_STATE_OPEN;

                pHDB->IpCheck.UseDelay = LSA_TRUE; /* AP01436352: delay ARP probes only first time */
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open NARE. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }

            Finish = LSA_TRUE;

            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a NARE-Close-Channel request                        */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_NARE_RUNNING:

            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot close NARE. Response: 0x%X", Response);
                /* is this a fatal error ?                      */
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
        /* we release the HDB                           */
        OHA_NAREReleaseHDB(pHDB);
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
                          "OUT: OHA_NAREChannelDone()");
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREOpenChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA NARE-Channel handle                          +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to NARE. The Request will be  +*/
/*+               finished by calling the NARE-Callbackfunction.            +*/
/*+               oha_nare_request_lower_done() if OHA_OK.                  +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB  = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);

    UserId.uvar32 = 0;
    OHA_NARE_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(NARE_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_NARE_RQB_SET_OPCODE(pRQB,NARE_OPC_OPEN_CHANNEL);

        pRQB->args.channel.handle_upper  = OHAHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.nare_request_upper_done_ptr = oha_nare_request_lower_done;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: NARE-Request: NARE_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
        OHA_NARE_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"NARE-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREOpenChannel. Result: 0x%X",
                          Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREOpenChannelDone                     +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE   pRQB          +*/
/*+                             OHA_HDB_NARE_PTR_TYPE         pHDB          +*/
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
/*+               - save NARE-Channel                                       +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NAREOpenChannelDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB)
{
    LSA_UINT16          Response = OHA_OK;
    LSA_UINT16          RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: NARE_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_NARE_RQB_GET_RESPONSE(pRQB));

    if ( OHA_NARE_RQB_GET_RESPONSE(pRQB) == NARE_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"NARE-Open Channel successful");
        /* save NARE-Channel in HDB */
        pHDB->NareHandle = pRQB->args.channel.handle;

    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-Open Channel failed.[0x%X]",OHA_NARE_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_NARE_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_NAREChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREOpenChannelDone(Response: 0x%X)", Response);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECloseChannel                        +*/
/*+  Input                      LSA_HANDLE_TYPE       NAREHandle            +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NAREHandle            : NARE handle to close                           +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to NARE. The Request will be +*/
/*+               finished by calling the NARE-Callbackfunction.            +*/
/*+               oha_nare_request_lower_done()                             +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARECloseChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB  = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECloseChannel(NAREHandle: 0x%X)",
                          NAREHandle);

    UserId.uvar32 = 0;
    OHA_NARE_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(NARE_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {

        OHA_NARE_RQB_SET_OPCODE(pRQB,NARE_OPC_CLOSE_CHANNEL);
        OHA_NARE_RQB_SET_HANDLE(pRQB,NAREHandle);

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: NARE-Request: NARE_OPC_CLOSE_CHANNEL (NAREHandle: 0x%X,pRQB: 0x%X)",NAREHandle,pRQB);
        OHA_NARE_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"NARE-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NARECloseChannel(Result: 0x%X)", Result);
    return(Result);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECloseChannelDone                    +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_HDB_NARE_PTR_TYPE        pHDB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARECloseChannelDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response = OHA_OK;
    LSA_UINT16               RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: NARE_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_NARE_RQB_GET_RESPONSE(pRQB));

    if ( OHA_NARE_RQB_GET_RESPONSE(pRQB) == NARE_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"NARE-Close Channel successful");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-Close Channel failed.[0x%X]",OHA_NARE_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_NARE_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_NAREChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NARECloseChannelDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREGetHandleByNIC                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_NARE_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a NARE HDB with NICId. If none is found   +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_NARE_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_NAREGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_NARE_PTR_TYPE  pHDB  = OHA_GET_HNARE_PTR(0);
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREGetHandleByNIC(NICId: 0x%X)",
                          NICId);
    while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
    {
        if (( pHDB->State == OHA_HANDLE_STATE_OPEN ) &&
            ( pHDB->Params.pDetail->Params.Nare.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDB++;
            i++;
        }
    }

    if ( ! Found ) pHDB = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NAREGetHandleByNIC(pHDB:  0x%X)", pHDB);
    return(pHDB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECheckNICUsed                        +*/
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
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_NARECheckNICUsed(
    LSA_UINT16 NICId)
{
    OHA_HDB_NARE_PTR_TYPE  pHDB  =  OHA_GET_HNARE_PTR(0);
    LSA_UINT32            i     =   0;
    LSA_BOOL              Found =   LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECheckNICUsed(NICId: 0x%X)",
                          NICId);
    if (NICId != OHA_INVALID_NIC_ID)
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS ) )
        {
            if (( pHDB->State != OHA_HANDLE_STATE_FREE ) &&
                ( pHDB->Params.pDetail->Params.Nare.NICId == NICId  ))
            {
                Found = LSA_TRUE;
            }
            else
            {
                pHDB++;
                i++;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARECheckNICUsed(Found:  0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREReleaseHDB                          +*/
/*+  Input/Output          :    OHA_HDB_NARE_PTR_TYPE pHDB                  +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBNare    : Pointer to User Handle to release                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release NARE -HDB                                         +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NAREReleaseHDB(
    OHA_HDB_NARE_PTR_TYPE pHDB)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    if (OHA_IS_NOT_NULL(pHDB->IpCheck.pCheckRQB))
    {
        oha_nare_free_resolve_rqb(pHDB, pHDB->IpCheck.pCheckRQB);
        pHDB->IpCheck.pCheckRQB = LSA_NULL;
    }

    pHDB->State = OHA_HANDLE_STATE_FREE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREReleaseHDB()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREGetFreeHDB                          +*/
/*+  Input/Output          :    OHA_HDB_NARE_PTR_TYPE OHA_LOCAL..pHDBNare   +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA NARE  -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free NARE -HDB                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREGetFreeHDB(
    OHA_HDB_NARE_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBNare)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found = LSA_FALSE;
    OHA_HDB_NARE_PTR_TYPE      pHDB  = OHA_GET_HNARE_PTR(0);
    LSA_UINT32                i     = 0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREGetFreeHDB(pHDB: 0x%X)",
                          pHDBNare);

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    while ( (! Found)  &&  ( i < OHA_CFG_MAX_NICS))
    {
        if ( pHDB->State == OHA_HANDLE_STATE_FREE )
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
        pHDB->State = OHA_HANDLE_STATE_CLOSED;
        *pHDBNare = pHDB;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_NAREGetFreeHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBNare);
    return(Status);
}

/*****************************************************************************/
/* Local function: oha_nare_free_resolve_rqb()                               */
/* free a lower Resolve-RQB for NARE                                         */
/*****************************************************************************/
LSA_VOID OHA_LOCAL_FCT_ATTR oha_nare_free_resolve_rqb(OHA_HDB_NARE_PTR_TYPE pHDB,
                                                      OHA_NARE_LOWER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16      RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_nare_free_resolve_rqb(pHDB: 0x%X, pRQB: 0x%X)",
                          pHDB, pRQB);

    OHA_ASSERT_NULL_PTR(pHDB);

    if (OHA_IS_NULL(pRQB))
    {
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: oha_nare_free_resolve_rqb()");
        return;
    }

    switch (OHA_NARE_RQB_GET_OPCODE(pRQB))
    {
        case NARE_OPC_RESOLVE_CONFIG:
            if (OHA_IS_NOT_NULL(pRQB->args.ResConf.pStationName))
            {
                OHA_NARE_FREE_LOWER_MEM(&RetVal, pRQB->args.ResConf.pStationName, pHDB->Params.pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
            }
            break;
        case NARE_OPC_TEST_IP:
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_NARE_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_nare_free_resolve_rqb(RetVal: 0x%X)", RetVal);
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREGetResolveConfigRes                 +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to User-HDB for this Channel.       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get cyclic Config done handling                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREGetResolveConfigRes(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB)
{
    LSA_UINT16 Response;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREGetResolveConfigRes(pRQBNare: 0x%X)", pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);

    switch (OHA_NARE_RQB_GET_RESPONSE(pRQB))
    {
        case NARE_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
            break;
        case NARE_RSP_ERR_DCP_MULTIPLE_STATIONS:
            Response = OHA_ERR_MULTIPLE_STATIONS;
            break;
        case NARE_RSP_ERR_MULTIPLE_IP_USE:
            Response = OHA_ERR_MULTIPLE_IP;
            break;
        case NARE_RSP_ERR_DCP_STATION_NOT_FOUND:
            Response = OHA_ERR_STATION_NOT_FOUND;
            break;
        case NARE_RSP_ERR_DCP_SET_FAILED:
        case NARE_RSP_ERR_DCP_SET_TIMEOUT:
        case NARE_RSP_ERR_DCP_IDENT_TLV_ERROR:
        case NARE_RSP_ERR_DNS_FAILED:
        case NARE_RSP_ERR_NO_OWN_IP:
            Response = OHA_ERR_FAILED;
            break;
        case NARE_RSP_OK_CANCEL:
            Response = OHA_OK_CANCEL;
            break;
        default:
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"NARE-ERROR: GET CONFIG cyclic failed.(0x%X).",OHA_NARE_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NAREGetResolveConfigRes(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECopyResplveConfig                   +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to User-HDB for this Channel.       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get cyclic Config done handling                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARECopyResplveConfig(
    OHA_UPPER_RQB_PTR_TYPE       pUserRQB,
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB)
{
    LSA_UINT16 Response;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECopyResplveConfig(pRQBNare: 0x%X,pUserRQB: 0x%X)", pUserRQB,pRQB);

    OHA_ASSERT_NULL_PTR(pUserRQB);
    OHA_ASSERT_NULL_PTR(pRQB);

    Response = OHA_NAREGetResolveConfigRes(pRQB);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARECopyResplveConfig(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREGetTestIpRes                        +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to User-HDB for this Channel.       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get cyclic Config done handling                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREGetTestIpRes(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB)
{
    LSA_UINT16 Response;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREGetTestIpRes(pRQBNare: 0x%X)", pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);

    switch (OHA_NARE_RQB_GET_RESPONSE(pRQB))
    {
        case NARE_RSP_ERR_IP_IN_USE:   /* IP-Addr found     */
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
        }
        break;
        case NARE_RSP_OK:
            Response = OHA_ERR_STATION_NOT_FOUND;
            break;
        case NARE_RSP_ERR_NO_OWN_IP:
            Response = OHA_ERR_FAILED;
            break;
        case NARE_RSP_OK_CANCEL:
            Response = OHA_OK_CANCEL;
            break;
        default:
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"NARE-ERROR: TEST IP failed.(0x%X).",OHA_NARE_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NAREGetTestIpRes(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECopyTestIp                          +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to User-HDB for this Channel.       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get cyclic Config done handling                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARECopyTestIp(
    OHA_UPPER_RQB_PTR_TYPE       pUserRQB,
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB)
{
    LSA_UINT16 Response;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECopyTestIp(pRQBNare: 0x%X,pUserRQB: 0x%X)", pUserRQB,pRQB);

    OHA_ASSERT_NULL_PTR(pUserRQB);
    OHA_ASSERT_NULL_PTR(pRQB);

    Response = OHA_NAREGetTestIpRes(pRQB);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARECopyTestIp(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NareUserCheckPending                    +*/
/*+  Input/Output          :    OHA_HDB_NARE_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to NARE-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NAREUserCheckPending(
    OHA_HDB_NARE_PTR_TYPE   pHDBNare,
    LSA_UINT16              Orginator,
    LSA_UINT16              Response)
{
    OHA_HDB_USER_PTR_TYPE     pHDBUser;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDBNare->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREUserCheckPending(pHDBNare: 0x%X, Response: 0x%X)",
                          pHDBNare, Response);

    /* orginator must be a user */
    OHA_ASSERT (OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_USER);

    /* check the handles */
    if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    if ( pHDBNare->UserRQBCnt[OHA_HANDLE_IDX(Orginator)] != 0 ) /* User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBNare->UserRQBs[OHA_HANDLE_IDX(Orginator)].pBottom,
                                 pHDBNare->UserRQBs[OHA_HANDLE_IDX(Orginator)].pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
        pHDBNare->UserRQBCnt[OHA_HANDLE_IDX(Orginator)]--;

        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);

        OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREUserCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CancelCyclicNare                        +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel handles a NARE-Request,then withdraw +*/
/*+               the NARE-Request and delete this USER entry.              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelCyclicNare(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish
)
{
    LSA_UINT16                  i;
    OHA_HDB_NARE_PTR_TYPE       pHDBNare = OHA_GET_HNARE_PTR(0);
    LSA_BOOL                    Finish   = LSA_TRUE;    /* finish the running CloseChannel */
    LSA_UINT16                  Status   = OHA_OK;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CancelCyclicNare(pHDBUser: 0x%X)",
                          pHDBUser);

    /* look for used Nare-Channels */
    for (i=0; (i < OHA_CFG_MAX_NICS) && (Finish) && (Status == OHA_OK); i++, pHDBNare++)
    {
        /* is a ResolveAddress active by this user ? */
        OHA_CHECK_INFO_PTR_TYPE pInfo = &pHDBUser->IpCheckInfo[i];

        /* cancel cyclic action check IP only (AP01522573) */
        {
            if (OHA_IS_NOT_NULL(pInfo->pCheckRQB))  /* Nare-RCTRL-Blocks pending ? */
            {
                if (OHA_IS_NOT_NULL(pHDBNare) &&
                    pHDBNare->State == OHA_HANDLE_STATE_OPEN) /* NARE-channel is opened ? */
                {
                    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB;

                    if (!pInfo->Underway)   /* request is inactive ? */
                    {
                        /* we can free the stored rqb */
                        pRQB = pInfo->pCheckRQB;
                        pInfo->pCheckRQB = LSA_NULL;

                        /* free the NARE Rqb */
                        if (OHA_IS_NOT_NULL(pRQB))
                        {
                            oha_nare_free_resolve_rqb(pHDBNare, pRQB);
                        }
                    }
                    else        /* request is active we have to withdraw the request */
                    {
                        pInfo->State = OHA_CHECK_STATE_WITHDRAW;

                        /* IpCheckInfo */
                        /* we can cancel the running user request ResolveAddress */
                        (LSA_VOID)OHA_NARETestIpCancel(pHDBNare, pHDBUser->ThisHandle, (LSA_UINT32)pHDBNare->ThisHandle);

                        Finish = LSA_FALSE; /* cannot close syncronous */
                    }
                }
                else    /* no corresponding NARE channel */
                {
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }
            }
        }
    }

    *pFinish = Finish;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_CancelCyclicNare(Status=0x%X)", Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARETestIp                              +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Orginator            : User or remote DCP client                       +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts the sending of the local neighbour-data for all    +*/
/*+               ports                                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARETestIp(
    LSA_UINT16                   NicID,
    OHA_COMMON_MEM_U8_PTR_TYPE   IpAddrPtr,
    LSA_UINT                     AddrLen,
    LSA_UINT16                   Orginator,
    LSA_UINT32                   RequestID)
{
    OHA_HDB_NARE_PTR_TYPE       pHDB   = OHA_NAREGetHandleByNIC(NicID);
    OHA_NARE_LOWER_RQB_PTR_TYPE pRQB   = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARETestIp(NicID: 0x%X, Orginator: 0x%X, RequestID: 0x%X)", NicID, Orginator, RequestID);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT(AddrLen == NARE_IP_ADDR_SIZE);

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_NARE_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(NARE_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
		/* AP00842368 Checking the IP-address by NARE (Gratuitous ARP): Sender IP must be 0.0.0.0 */

        OHA_NARE_RQB_SET_OPCODE(pRQB,NARE_OPC_TEST_IP);
        OHA_NARE_RQB_SET_HANDLE(pRQB,pHDB->NareHandle);
        OHA_NARE_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        /* need a reference for NARE_OPC_RESOLVE_CONFIG_CANCEL  */
        pRQB->args.TestIP.RequestID = RequestID;
        pRQB->args.TestIP.UseDelay  = pHDB->IpCheck.UseDelay; /* if true an initial delay will be used */

		pHDB->IpCheck.UseDelay = LSA_FALSE; /* AP01436352: delay ARP probes only first time */

		OHA_MEMCPY (pRQB->args.TestIP.LocalIPAddr, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP, NARE_IP_ADDR_SIZE);
        OHA_MEMCPY (pRQB->args.TestIP.IPAddr, IpAddrPtr, AddrLen);
        OHA_MEMSET (pRQB->args.TestIP.MACAddr, 0, NARE_MAC_ADDR_SIZE);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: NARE-Request: NARE_OPC_TEST_IP (Handle: 0x%X,NAREHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->NareHandle,pRQB);
        OHA_NARE_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"NARE-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NARETestIp(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARETestIpDone                          +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Get Config done handling                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARETestIpDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              Orginator = OHA_NARE_RQB_GET_USERID_UVAR16(pRQB);
    OHA_UPPER_RQB_PTR_TYPE  pUserRQB;
    OHA_HDB_USER_PTR_TYPE   pHDBUser;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARETestIpDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: NARE_OPC_TEST_IP done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_NARE_RQB_GET_RESPONSE(pRQB));

    /* orginator must be a user */
    OHA_ASSERT (OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_USER);

    /* check the handle */
    if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    /* check the nare handle */
    if (OHA_HANDLE_IDX(pHDB->ThisHandle) > OHA_CFG_MAX_NICS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    pUserRQB  = pHDB->UserRQBs[OHA_HANDLE_IDX(Orginator)].pBottom;
    OHA_ASSERT_NULL_PTR(pUserRQB);

    pHDBUser = OHA_GetUserHDBFromHandle(OHA_RQB_GET_HANDLE(pUserRQB));

    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* fill the indication-rqb */
    Response = OHA_NARECopyTestIp(pUserRQB, pRQB);

    /* we have to look at the check interval for a cyclic check of IP */
    if (pHDBUser->Params.pDetail->Params.User.CheckInterval != 0)
    {
        OHA_CHECK_INFO_PTR_TYPE pInfo = &pHDBUser->IpCheckInfo[OHA_HANDLE_IDX(pHDB->ThisHandle)];

        /* store the NARE Rqb for cyclic check opeation */
        /* set orginator to HandleTypeNare              */
        /* start the (cyclic) check-interval (in sec)   */
        if (pInfo->State != OHA_CHECK_STATE_WITHDRAW &&
            OHA_NARE_RQB_GET_RESPONSE(pRQB) != NARE_RSP_OK_CANCEL)
        {
            OHA_CHECK_INFO_PTR_TYPE pInfoOther = &pHDBUser->NameCheckInfo[OHA_HANDLE_IDX(pHDB->ThisHandle)];
            LSA_UINT32 AlternateFactor;

            /* if check NameOfStation is running and this is the first check we alternate the check requests: */
            /* check NameOfStation / check IP / check NameOfStation and so on ... */
            AlternateFactor = (pInfoOther->State == OHA_CHECK_STATE_START) ? 2 : 1;

            pInfo->Interval = pHDBUser->Params.pDetail->Params.User.CheckInterval * OHA_CHECK_GRID * AlternateFactor;
            pInfoOther->Interval = pInfo->Interval;

            OHA_NARE_RQB_SET_USERID_UVAR16(pRQB,pHDB->ThisHandle);  /* set orginator = NARE */

            /* only one cyclic request must be active */
            if (OHA_IS_NOT_NULL(pInfo->pCheckRQB))
            {
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            }
            /* store the request */
            pInfo->pCheckRQB = pRQB;

            pInfo->Underway = LSA_FALSE;

            /* set timer counter for next send */
            pInfo->NextSend = OHA_GET_GLOB_PTR()->CyclicTimerCnt + pInfo->Interval + 1;

            pInfo->State = OHA_CHECK_STATE_START;

            /* store the last response */
            pInfo->LastResponse = Response;

            /* do not free the NARE Rqb */
            OHA_NAREUserCheckPending(pHDB, Orginator, Response);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARETestIpDone(Response: 0x%X)", Response);
            return;
        }
    }

    /* free the NARE Rqb */
    oha_nare_free_resolve_rqb(pHDB, pRQB);

    OHA_NAREUserCheckPending(pHDB, Orginator, Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARETestIpDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARETestCyclicIpDone                    +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Get Config done handling                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARETestCyclicIpDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB)
{
    LSA_UINT16              Orginator = OHA_NARE_RQB_GET_USERID_UVAR16(pRQB);
    LSA_UINT32              i;
    OHA_HDB_USER_PTR_TYPE   pHDBUser  = OHA_GET_HUSER_PTR(0);
    LSA_BOOL                UserFound = LSA_FALSE;
    OHA_CHECK_INFO_PTR_TYPE pInfo     = LSA_NULL;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARETestCyclicIpDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: NARE_OPC_TEST_IP done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_NARE_RQB_GET_RESPONSE(pRQB));

    /* orginator must be NARE */
    if (OHA_HANDLE_GET_TYPE(Orginator) != OHA_HANDLE_TYPE_NARE)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    /* check the nare handle */
    if (OHA_HANDLE_IDX(pHDB->ThisHandle) > OHA_CFG_MAX_NICS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    /* look for the corresponding user */
    for (i=0; i<OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
        /* only if the CheckInterval is in use */
        if (OHA_IS_NULL(pHDBUser->Params.pDetail) ||
            pHDBUser->Params.pDetail->Params.User.CheckInterval == 0)
        {
            continue;
        }

        pInfo = &pHDBUser->IpCheckInfo[OHA_HANDLE_IDX(pHDB->ThisHandle)];

        if ( OHA_IS_NOT_NULL(pInfo->pCheckRQB) &&
             LSA_HOST_PTR_ARE_EQUAL(pRQB, pInfo->pCheckRQB) )
        {
            UserFound = LSA_TRUE;
            break;  /* found the corresponding rqb */
        }
    }

    if (UserFound)
    {
        OHA_UPPER_RQB_PTR_TYPE  pUserRQB;
        LSA_UINT16              Response;

        pInfo->Underway = LSA_FALSE;

        switch (pInfo->State)
        {
            case OHA_CHECK_STATE_START:     /* get user check-indication rqb */

#ifndef OHA_INDICATE_EVERY_CYCLIC_CHECK
                Response = OHA_NAREGetTestIpRes(pRQB);

                /* indicate only the changes */
                if (pInfo->LastResponse != Response)
#endif
                {
                    OHA_RQB_REM_BLOCK_BOTTOM(
                        pHDBUser->CheckIndReq.pBottom,
                        pHDBUser->CheckIndReq.pTop,
                        pUserRQB);

                    if (! LSA_HOST_PTR_ARE_EQUAL(pUserRQB, LSA_NULL)) /* user-IndRqb's exists ? */
                    {
                        /* fill the indication-rqb */
                        Response = OHA_NARECopyTestIp(pUserRQB, pRQB);

                        /* store the last check indication */
                        pInfo->LastResponse = Response;

                        pHDBUser->CheckIndReqPending--;

                        OHA_UserRequestFinish(pHDBUser,pUserRQB,Response);
                    }
                }
                break;
            case OHA_CHECK_STATE_WITHDRAW:  /* fade out, no further actions here */
            {
                OHA_NARE_LOWER_RQB_PTR_TYPE  pCancelRQB;

                /* we have to free the stored rqb */
                pCancelRQB = pInfo->pCheckRQB;
                pInfo->pCheckRQB = LSA_NULL;

                if (!LSA_HOST_PTR_ARE_EQUAL(pCancelRQB, pRQB))
                {
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                oha_nare_free_resolve_rqb(pHDB, pRQB);
                pInfo->State = OHA_CHECK_STATE_START;
            }
            break;
            case OHA_CHECK_STATE_STOP:
            default:
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }
    else
    {
        oha_nare_free_resolve_rqb(pHDB, pRQB);
        /* where is the Nare-Rqb ? */
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARETestCyclicIpDone(UserFound: 0x%X)", UserFound);
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECheckIpDone                         +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Check IP done handling for a local check                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARECheckIpDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              NICId = pHDB->Params.pDetail->Params.Nare.NICId;
    LSA_UINT16              Orginator;
    LSA_BOOL                Finish = LSA_TRUE;
    LSA_BOOL                LastIpConflictStatus = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECheckIpDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    Orginator = OHA_NARE_RQB_GET_USERID_UVAR16(pRQB);
    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: OHA TEST_IP done (Handle: 0x%X,Rsp: 0x%X,Orginator: 0x%X)",pHDB->ThisHandle,OHA_NARE_RQB_GET_RESPONSE(pRQB),Orginator);
    OHA_ASSERT(Orginator == OHA_HANDLE_TYPE_SYS);

    (LSA_VOID)oha_db_read_ip_conflict_status(NICId, &LastIpConflictStatus);

    OHA_ASSERT(pHDB->IpCheck.Underway);
    pHDB->IpCheck.Underway = LSA_FALSE;

    /* we've cancelled a running check (e.g. a user.req is present) */
    if (pHDB->IpCheck.State == OHA_CHECK_STATE_WITHDRAW)
    {
        Response = OHA_OK_CANCEL;
        oha_nare_free_resolve_rqb(pHDB, pRQB);

        pHDB->IpCheck.State = OHA_CHECK_STATE_STOP; /* no further checks after a non conflicting detection */
        pHDB->IpCheck.pCheckRQB = LSA_NULL;

        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARECheckIpDone(Response: 0x%X)", Response);
        return;
    }

    switch (OHA_NARE_RQB_GET_RESPONSE(pRQB))
    {
        case NARE_RSP_ERR_IP_IN_USE:   /* IP-Addr found     */
            Response = OHA_ERR_MULTIPLE_IP;
            oha_db_write_ip_conflict_status(NICId, LSA_TRUE); /* set the conflict flag */
            break;
        case NARE_RSP_OK:
            Response = OHA_OK;
            break;
        case NARE_RSP_ERR_NO_OWN_IP:
            Response = OHA_ERR_FAILED;
            break;
        case NARE_RSP_OK_CANCEL:
            Response = OHA_OK_CANCEL;
            break;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-ERROR: OHA TEST IP failed.(0x%X).",OHA_NARE_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    if (Response == OHA_ERR_MULTIPLE_IP)    /* we've to delete the IpAddress to {0,0,0,0} */
    {
        OHA_CHECK_INFO_PTR_TYPE pInfo = &pHDB->IpCheck;

        /* we have to look at the check interval for a cyclic check of NoS */
        if (pInfo->Interval != 0)
        {
            /* store the NARE Rqb for cyclic check opeation */
            /* start the (cyclic) check-interval (in sec)   */
            if (pInfo->State != OHA_CHECK_STATE_WITHDRAW &&
                OHA_NARE_RQB_GET_RESPONSE(pRQB) != NARE_RSP_OK_CANCEL)
            {
                /* store the request */
                pInfo->pCheckRQB = pRQB;

                Finish = LSA_FALSE;

                /* set timer counter for next send */
                pInfo->NextSend = OHA_GET_GLOB_PTR()->CyclicTimerCnt + (pInfo->Interval * OHA_CHECK_GRID);

                /* store the last response */
                pInfo->LastResponse = Response;
            }
        }

        if (!LastIpConflictStatus)  /* change from non-conflict to conflict */
        {
            LSA_UINT16 RetVal;
            LSA_BOOL   IpIsEmergency                    = LSA_FALSE;
            OHA_COMMON_MEM_U8_PTR_TYPE IpAdrPtr         = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr    = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtrPtr = LSA_NULL;

            /* delete the real ip address */
            (LSA_VOID)oha_db_delete_real_ip_suite(NICId);

            /* get the emergency flag */
			/* AP01270055: OHA_ADDR_INFO_TYPE: Missing information by AddressConflict of an EmergencyIP */
            OhaGetConflictIp(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr, &IpIsEmergency);

            /* we have an asynchronous call to SOCK */
            RetVal = OHA_SOCKSetIpAddress(NICId, pHDB->ThisHandle,
                                          (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
                                          (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
                                          (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
                                          IpIsEmergency);
            if (RetVal != OHA_OK)   /* local SockSetIpAddress fails */
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Local SOCK-SetIpAddress failed.[0x%X]",RetVal);
            }
        }
    }
    else
    {
        oha_db_write_ip_conflict_status(NICId, LSA_FALSE); /* reset the conflict flag */

        if (LastIpConflictStatus)   /* change from conflict to non-conflict */
        {
            LSA_UINT16 RetVal;
            LSA_BOOL   IpIsEmergency                    = LSA_FALSE;
            OHA_COMMON_MEM_U8_PTR_TYPE IpAdrPtr         = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr    = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtrPtr = LSA_NULL;

            /* get the (real) ip address */
            OhaGetConflictIp(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr, &IpIsEmergency); /* RealIP */

            OHA_ASSERT_NULL_PTR(IpAdrPtr);
            OHA_ASSERT_NULL_PTR(SubnetMaskPtr);
            OHA_ASSERT_NULL_PTR(RouterAddrPtrPtr);

            /* set the (real) ip address */
            (LSA_VOID)oha_db_set_real_ip_suite(NICId, IpAdrPtr, SubnetMaskPtr, RouterAddrPtrPtr);

            /* we have an asynchronous call to SOCK */
            RetVal = OHA_SOCKSetIpAddress(NICId, pHDB->ThisHandle,
                                          IpAdrPtr,
                                          SubnetMaskPtr,
                                          RouterAddrPtrPtr,
                                          IpIsEmergency);

            if (RetVal != OHA_OK)   /* local Sock(Re)SetIpAddress fails */
            {
                (LSA_VOID)oha_db_delete_real_ip_suite(NICId);
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Local SOCK-(Re)SetIpAddress failed.[0x%X]",RetVal);
            }
        }
    }

    if (Finish)
    {
        oha_nare_free_resolve_rqb(pHDB, pRQB);

        pHDB->IpCheck.State = OHA_CHECK_STATE_STOP; /* no further checks after a non conflicting detection */
        pHDB->IpCheck.pCheckRQB = LSA_NULL;
    }

    /* build the AddressInfo.Ind */
    OHA_AddressIndicateEventAllUser(NICId);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARECheckIpDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARETestIpCancel                        +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Orginator            : User or remote DCP client                       +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Stops the sending of the caclic IP test                   +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_NARE_LOWER_RQB_PTR_TYPE OHA_NARETestIpCancel(
    OHA_HDB_NARE_PTR_TYPE       pHDB,
    LSA_UINT16                  Orginator,
    LSA_UINT32                  RequestID)
{
    OHA_NARE_LOWER_RQB_PTR_TYPE pRQB   = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARETestIpCancel(pHandle: 0x%X, Orginator: 0x%X; ,RequestID: 0x%X)", pHDB, Orginator, RequestID);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_NARE_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(NARE_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_NARE_RQB_SET_OPCODE(pRQB,NARE_OPC_TEST_IP_CANCEL);
        OHA_NARE_RQB_SET_HANDLE(pRQB,pHDB->NareHandle);
        OHA_NARE_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        pRQB->args.TestIPCancel.RequestID = RequestID; /* reference for NARE_OPC_TEST_IP_CANCEL  */

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_NOTE,">>>: NARE-Request: NARE_OPC_TEST_IP_CANCEL (Handle: 0x%X,NAREHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->NareHandle,pRQB);
        OHA_NARE_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NARETestIpCancel(Status: 0x%X)",Status);
    return(pRQB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARETestIpCancelDone                    +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Resolve Config done handling                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARETestIpCancelDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              Orginator;
    LSA_HANDLE_TYPE         Handle;
    LSA_BOOL                DoClose = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARETestIpCancelDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: NARE_OPC_RESOLVE_CONFIG_CANCEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_NARE_RQB_GET_RESPONSE(pRQB));

    OHA_ASSERT_NULL_PTR(pRQB);

    Orginator  = OHA_NARE_RQB_GET_USERID_UVAR16(pRQB);

    Handle     = (LSA_HANDLE_TYPE)Orginator;

    /* check the nare handle */
    if (OHA_HANDLE_IDX(pHDB->ThisHandle) > OHA_CFG_MAX_NICS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    switch (OHA_NARE_RQB_GET_RESPONSE(pRQB))
    {
        case NARE_RSP_OK:
        case NARE_RSP_ERR_NO_OWN_IP:
        case NARE_RSP_ERR_IP_IN_USE:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;

            switch (OHA_HANDLE_GET_TYPE(Handle))
            {
                case OHA_HANDLE_TYPE_USER:  /* it's a User CloseChannel */
                {
                    OHA_HDB_USER_PTR_TYPE  pHDBUser = OHA_GetUserHDBFromHandle(Handle);
                    OHA_ASSERT_NULL_PTR(pHDBUser);

                    if (LSA_HOST_PTR_ARE_EQUAL(pRQB, pHDB->IpCheck.pCancelRQB))
                    {
                        /* the ip address was set, a check-IP (ethernet) had been cancelled */
                        /* before we indicate the user! */
                        OHA_UserSetAddrPending (pHDBUser, OHA_OK);
                    }
                    else
                    {
                        if (pHDB->State != OHA_HANDLE_STATE_FREE)
                        {
                            /* look at all internal check requests */
                            if (pHDBUser->Params.pDetail->Params.User.CheckInterval != 0)
                            {
                                OHA_CHECK_INFO_PTR_TYPE pInfoIp = &pHDBUser->IpCheckInfo[OHA_HANDLE_IDX(pHDB->ThisHandle)];
                                OHA_CHECK_INFO_PTR_TYPE pInfoName = &pHDBUser->NameCheckInfo[OHA_HANDLE_IDX(pHDB->ThisHandle)];

                                if (pInfoIp->State != OHA_CHECK_STATE_WITHDRAW &&
                                    pInfoName->State != OHA_CHECK_STATE_WITHDRAW)
                                {
                                    OHA_USERCloseChannelDone(pHDBUser,OHA_OK); /* no further withdraw active */
                                }
                            }
                            else
                            {
                                OHA_USERCloseChannelDone(pHDBUser,OHA_OK); /* further actions taken in channel handler */
                            }
                        }
                    }
                }
                break;

                case OHA_HANDLE_TYPE_NARE:  /* if it's a (cyclic (re-))check, fade out */
                    if (LSA_HOST_PTR_ARE_EQUAL(pRQB, pHDB->IpCheck.pCancelRQB))
                    {
                        if (OHA_IS_NOT_NULL(pHDB->pOCReq) && OHA_RQB_GET_OPCODE(pHDB->pOCReq) == OHA_OPC_CLOSE_CHANNEL)
                        {
                            DoClose = LSA_TRUE; /* a CloseSystemChannel */
                        }
                    }
                    break;
                case OHA_HANDLE_TYPE_DCP: /* an OHA controlled Dcp-Set */
                    (LSA_UINT16)OHA_DoDcpSetAddr(OHA_DCPGetHandleByNIC(pHDB->Params.pDetail->Params.Nare.NICId));
                    break;
                default:
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    break;
            }
            break;
        case NARE_RSP_ERR_OPCODE:
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_FATAL,"NARE-ERROR: GET CONFIG failed.(0x%X).",OHA_NARE_RQB_GET_RESPONSE(pRQB));
            OHA_FATAL(OHA_FATAL_ERR_NARE);
            Response = OHA_ERR_LL;
            break;
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB, pHDB->IpCheck.pCancelRQB))
    {
        pHDB->IpCheck.pCancelRQB = LSA_NULL;
    }
    OHA_NARE_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    if (DoClose)
    {
        LSA_UINT16 Status = OHA_NARECloseChannel(pHDB->NareHandle,pHDB->Params.pSys);

        if ( Status != OHA_OK )
        {
            OHA_UPPER_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Nare.NICId), LSA_TRACE_LEVEL_ERROR,"OHA_NARETestIpCancelDone(): OHA_NARECloseChannel() failed! (Status: 0x%X)",Status);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NARETestIpCancelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeCyclicNareRqbs                      +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel handles a NARE-Request,then delete   +*/
/*+               the NARE-Requests                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_FreeCyclicNareRqbs(
    OHA_HDB_USER_PTR_TYPE        pHDBUser
)
{
    LSA_UINT16                  i;
    OHA_HDB_NARE_PTR_TYPE       pHDBNare = OHA_GET_HNARE_PTR(0);
    LSA_BOOL                    Finish   = LSA_TRUE;    /* finish the running CloseChannel */
    LSA_UINT16                  Status   = OHA_OK;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FreeCyclicNareRqbs(pHDBUser: 0x%X)",
                          pHDBUser);

    /* look for used Nare-Channels */
    for (i=0; (i < OHA_CFG_MAX_NICS) && (Finish) && (Status == OHA_OK); i++, pHDBNare++)
    {
        /* is a ResolveAddress active by this user ? */
        OHA_CHECK_INFO_PTR_TYPE pInfo;
        LSA_UINT16              j;

        /* cancel both cyclic actions: check NameOfStation and check IP */
        for (j=0, pInfo = &pHDBUser->NameCheckInfo[i]; j<2; pInfo = &pHDBUser->IpCheckInfo[i], j++)
        {
            if (OHA_IS_NOT_NULL(pInfo->pCheckRQB))  /* Nare-RCTRL-Blocks pending ? */
            {
                if (OHA_IS_NOT_NULL(pHDBNare) &&
                    pHDBNare->State == OHA_HANDLE_STATE_OPEN) /* NARE-channel is opened ? */
                {
                    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB;

                    if (!pInfo->Underway)   /* request is inactive ? */
                    {
                        /* we can free the stored rqb */
                        pRQB = pInfo->pCheckRQB;
                        pInfo->pCheckRQB = LSA_NULL;

                        /* free the NARE Rqb */
                        if (OHA_IS_NOT_NULL(pRQB))
                        {
                            oha_nare_free_resolve_rqb(pHDBNare, pRQB);
                        }
                    }
                    else    /* request must have been withdrawed */
                    {
                        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    }
                }
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_FreeCyclicNareRqbs(Status=0x%X)", Status);
    return(Status);
}

/*****************************************************************************/
/* Local function: oha_nare_free_hello_rqb()                                 */
/* free a lower Hello-RQB for NARE                                           */
/*****************************************************************************/
LSA_VOID OHA_LOCAL_FCT_ATTR oha_nare_free_hello_rqb(
    OHA_HDB_NARE_PTR_TYPE pHDB,
    OHA_NARE_LOWER_RQB_PTR_TYPE pRQB)
{
    /* OHA_NARE_LOWER_MEM_PTR_TYPE  MemPtr; */
    LSA_UINT16                  RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_nare_free_hello_rqb(pHDB: 0x%X, pRQB: 0x%X)",
                          pHDB, pRQB);

    if (OHA_IS_NOT_NULL(pRQB))
    {
        /* free the allocated buffer */
        if (OHA_IS_NOT_NULL(pRQB->args.Hello.pTLV))
        {
            OHA_NARE_FREE_LOWER_MEM(&RetVal, pRQB->args.Hello.pTLV, pHDB->Params.pSys);
        }
        OHA_NARE_FREE_LOWER_RQB(&RetVal, pRQB, pHDB->Params.pSys);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_nare_free_hello_rqb()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREHelloSend                           +*/
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
/*+  Description: Starts the sending of the "hello" request                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREHelloSend(
    LSA_UINT16                  NICId,
    LSA_UINT32                  HelloRetry,
    LSA_UINT32                  HelloInterval,
    LSA_UINT32                  HelloDelay
)
{
    OHA_HDB_NARE_PTR_TYPE       pHDB;
    OHA_NARE_LOWER_RQB_PTR_TYPE pRQB   = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREHelloSend(HelloRetry: 0x%X, HelloInterval: 0x%X)",
                          HelloRetry, HelloInterval);

    pHDB = OHA_NAREGetHandleByNIC(NICId);

    if (OHA_IS_NULL(pHDB)) /* no (more) NARE system channel */
    {
        Status = OHA_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_NAREHelloSend(Status: 0x%X)",Status);
        return(Status);
    }

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

#if 0   /* some checks ??? */
    if (ControlStatus == OHA_START_SEND && (HelloRetry == 0 || HelloInterval == 0))
    {
        Status = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"Could not send HELLO.Req: HelloRetry(0x%X), HelloInterval(0x%X)", HelloRetry, HelloInterval);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NAREHelloSend(Status: 0x%X)",Status);
        return(Status);
    }
#endif

    OHA_NARE_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(NARE_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LSA_INT Len;
        LSA_INT                 OptCnt   = 0;
        OHA_NARE_LOWER_MEM_PTR_TYPE pMem = LSA_NULL;

        OHA_MEMSET(pRQB, 0, sizeof(NARE_RQB_TYPE));

        OHA_NARE_ALLOC_LOWER_MEM(&pMem,UserId,DCP_MAX_RSP_DATA_SIZE,pSys);
        if (OHA_IS_NULL(pMem))
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-Allocating RQB-memory failed!");
            Status = OHA_ERR_RESOURCE;
            oha_nare_free_hello_rqb(pHDB, pRQB);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NAREHelloSend(Status: 0x%X)",Status);
            return(Status);
        }

        /* Sendepuffer ausfüllen */
        Len = oha_dcp_HelloReqSet(OHA_DCPGetHandleByNIC(NICId), (DCP_USR_RSP_DATA_PTR)pMem, 0, &OptCnt);
        if (Len == 0)
        {
            Status = OHA_ERR_DATA_INVALID;
        }

        pRQB->args.Hello.pTLV       = pMem;
        pRQB->args.Hello.OptCnt     = OptCnt;

		/* AP00940587: FSHelloRetry was about 1 too small. */
        pRQB->args.Hello.SendCnt      = HelloRetry + 1;
        pRQB->args.Hello.SendInterval = HelloInterval;
        pRQB->args.Hello.Delay        = HelloDelay;
        pRQB->args.Hello.TLVLength    = Len;	/* AP01264774, AP01264224 */

        if (Status == OHA_OK)
        {
            OHA_NARE_RQB_SET_OPCODE(pRQB, NARE_OPC_HELLO);
            OHA_NARE_RQB_SET_HANDLE(pRQB,pHDB->NareHandle);
            OHA_NARE_RQB_SET_USERID_UVAR16(pRQB,0xffff);

            pHDB->NareRQBCnt++;
            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: NARE-Request: NARE_OPC_HELLO (Handle: 0x%X,NAREHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->NareHandle,pRQB);
            OHA_NARE_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            oha_nare_free_hello_rqb(pHDB, pRQB);
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREHelloSend(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREHelloStop                           +*/
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
/*+  Description: Stops the cyclic sending of the "hello" request           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREHelloStop(
    LSA_UINT16                  NICId,
    LSA_UINT16                  Orginator
)
{
    OHA_HDB_NARE_PTR_TYPE       pHDB;
    OHA_NARE_LOWER_RQB_PTR_TYPE pRQB   = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREHelloStop(HelloRetry: 0x%X, HelloInterval: 0x%X)",
                          NICId, Orginator);

    pHDB = OHA_NAREGetHandleByNIC(NICId);

    if (OHA_IS_NULL(pHDB)) /* no (more) NARE system channel */
    {
        Status = OHA_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_NAREHelloSend(Status: 0x%X)",Status);
        return(Status);
    }

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_NARE_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(NARE_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_MEMSET(pRQB, 0, sizeof(NARE_RQB_TYPE));

        /* discard sending Hello */
        pRQB->args.HelloStop.RequestID = 0;

        OHA_NARE_RQB_SET_OPCODE(pRQB, NARE_OPC_HELLO_STOP);
        OHA_NARE_RQB_SET_HANDLE(pRQB,pHDB->NareHandle);
        OHA_NARE_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        pHDB->NareRQBCnt++;
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: NARE-Request: NARE_OPC_HELLO (Handle: 0x%X,NAREHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->NareHandle,pRQB);
        OHA_NARE_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREHelloSend(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoHello                                 +*/
/*+  Input/Output          :    LSA_UINT16      NICId                       +*/
/*+                        :    LSA_UINT16      AdditionalDelay [10ms]      +*/
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
/*+  Description: Handles the sending of the "hello" request                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DoHello(
    LSA_UINT16                  NICId,
    LSA_UINT16                  AdditionalDelay
)
{
    LSA_UINT32 DeviceMode    = 0;
    LSA_UINT32 HelloRetry    = 0;
    LSA_UINT32 HelloInterval = 0;
    LSA_UINT32 HelloDelay    = 0;
    LSA_UINT16 Status        = OHA_OK;
    OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPDoHello(NICId: 0x%X, AdditionalDelay: 0x%X)", NICId, AdditionalDelay);

    if (OHA_IS_NULL(pHDBDcp)) /* no (more) DCP system channel */
    {
        Status = OHA_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoHello(Status: 0x%X)",Status);
        return(Status);
    }

    RetVal = OHA_DB_READ_DEVICE_INITIATIVE(NICId, &DeviceMode, &HelloRetry, &HelloInterval, &HelloDelay);

    if (RetVal == LSA_RET_OK)
    {
        if ((DeviceMode & OHA_DCP_HELLO_ON) || (DeviceMode & OHA_DCP_HELLO_ON_DELAY))   /* HelloEnable */
        {
            if (OHA_EDDIsAtLeastOnePortUp(NICId))   /* LinkUp ? */
            {
				/* AP00895104: HelloDelay for Fast Startup is valued around factor 10 too high */
                /* 1073828: different behavior of FSHelloMode and FSHelloDelay */
                if (DeviceMode & OHA_DCP_HELLO_ON_DELAY)
                {
                Status = OHA_NAREHelloSend(NICId, HelloRetry, HelloInterval/10, HelloDelay/10 + AdditionalDelay);  /* HelloRetry, HelloInterval (10ms) */
                }
                else
                {
                    Status = OHA_NAREHelloSend(NICId, HelloRetry, HelloInterval/10, AdditionalDelay);  /* HelloRetry, HelloInterval (10ms) */
                }
                if (Status != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_DoHello(). Send HELLO.Req failed.");
                }

                /* set state */
                pHDBDcp->HelloState = OHA_DCP_STATE_HELLO_SENT;
            }
            else
            {
                pHDBDcp->HelloState = OHA_DCP_STATE_HELLO_IS_TO_SEND;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DoHello(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NAREHelloDone                           +*/
/*+  Input                      OHA_NARE_LOWER_RQB_PTR_TYPE pRQB            +*/
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
/*+  Description: Send-Hallo done handling.                                 +*/
/*+                                                                         +*/
/*+              - frees RQB memory                                         +*/
/*+              - call global state handling for further actions           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NAREHelloDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB)
{
    LSA_UINT16      Response;
    LSA_OPCODE_TYPE LowerOpcode;
    LSA_UINT16      Orginator;
    LSA_UINT16      NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NAREHelloDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);
	NICId = pHDB->Params.pDetail->Params.Nare.NICId;

    OHA_ASSERT_NULL_PTR(pRQB);

    LowerOpcode = OHA_NARE_RQB_GET_OPCODE(pRQB);

    Orginator  = OHA_EDD_RQB_GET_USERID_UVAR16(pRQB);

    switch (OHA_NARE_RQB_GET_OPCODE(pRQB))
    {
        case NARE_OPC_HELLO:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,"HELLO.Con (Response 0x%X)",OHA_NARE_RQB_GET_RESPONSE(pRQB));
            break;
        case NARE_OPC_HELLO_STOP:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,"HELLO_STOP.Con (Response 0x%X)",OHA_NARE_RQB_GET_RESPONSE(pRQB));
            break;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,"HELLO_STOP.Con: No HELLO.Con (Opcode 0x%X).",OHA_NARE_RQB_GET_OPCODE(pRQB));
            break;
    }

    switch (OHA_NARE_RQB_GET_RESPONSE(pRQB))
    {
        case NARE_RSP_OK:
        case NARE_RSP_OK_CANCEL:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"NARE-ERROR: HELLO failed.(0x%X).",OHA_NARE_RQB_GET_RESPONSE(pRQB));

            Response = OHA_ERR_LL;
            break;
    }

    oha_nare_free_hello_rqb(pHDB, pRQB);
    pHDB->NareRQBCnt--;

    if (LowerOpcode == NARE_OPC_HELLO_STOP) /* handled by PrmEnd */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;

        /* orginator is the user handle */
        OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)Orginator);

        OHA_ASSERT_NULL_PTR(pHDBUser);

        /* now Hello Stop by PrmEnd is done */
        OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->TopoUserReq.pBottom,
                                 pHDBUser->TopoUserReq.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was > 0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
        OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PRM_END);

        pHDBUser->TopoReqPendingCnt--; /* Number of pend. Req's (TOPO)*/
        OHA_ASSERT(pHDBUser->TopoReqPendingCnt == 0);

#ifndef OHA_CFG_NO_REMA
        (LSA_UINT16)oha_rema_prm_end(NICId);
#endif

        /* finally do the PrmEnd */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Handle Prm-End successful");

        pHDBUser->PrmState = OHA_PRM_STATE_END;     /* a PrmEnd is done */
        OHA_UserRequestFinish(pHDBUser,pRQBUser,OHA_OK);

		/* patch for AP00916191: */
		/* OhaPrmEnd is done and the new IRDataUUID is different, send LLDP as {0} until OhaPrmCommit */
		oha_update_sync_status(NICId);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NAREHelloDone(Response: 0x%X)",Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECheckAddress                        +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Orginator            : User or remote DCP client                       +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK        check is active                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calls the check of NameOfStation and IP-address           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARECheckAddress(
    LSA_UINT16          NicID,
    LSA_UINT32          CheckOptions,
    LSA_UINT32          Options,
    LSA_UINT16          CheckInterval)
{
    LSA_UINT16      RetVal = OHA_OK;
    OHA_HDB_NARE_PTR_TYPE pHDBNare = OHA_NAREGetHandleByNIC(NicID);

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECheckAddress(NicID: 0x%X, CheckOptions: 0x%X, Options: 0x%X, CheckInterval: 0x%X)", NicID, CheckOptions, Options, CheckInterval);

    OHA_ASSERT_NULL_PTR(pHDBNare);

    /* initialize the ethernet check parameters */
    if (Options & OHA_CTRL_IP_SUITE)
    {
        pHDBNare->IpCheck.State    = OHA_CHECK_STATE_STOP;
        pHDBNare->IpCheck.Interval = CheckInterval;
        if (OHA_IS_NOT_NULL(pHDBNare->IpCheck.pCheckRQB))
        {
            oha_nare_free_resolve_rqb(pHDBNare, pHDBNare->IpCheck.pCheckRQB);
            pHDBNare->IpCheck.pCheckRQB = LSA_NULL;
        }
    }

    /* is the IP-Suite to check (ethernet) */
    if ((CheckOptions & OHA_CTRL_IP_SUITE) &&
        (Options & OHA_CTRL_IP_SUITE))
    {
        OHA_COMMON_MEM_U8_PTR_TYPE  IpAddrPtr     = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE  SubnetAddrPtr = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE  RouterAddrPtr = LSA_NULL;

        (LSA_VOID)OHA_DB_READ_IP_SUITE(NicID, &IpAddrPtr, &SubnetAddrPtr, &RouterAddrPtr);
        OHA_ASSERT_NULL_PTR(IpAddrPtr);
        OHA_ASSERT_NULL_PTR(SubnetAddrPtr);
        OHA_ASSERT_NULL_PTR(RouterAddrPtr);

        /* check ip address if not null */
        if (!(OHA_MEMCMP(IpAddrPtr, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE) &&
              OHA_MEMCMP(SubnetAddrPtr, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE) &&
              OHA_MEMCMP(RouterAddrPtr, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE)))
        {
            LSA_UINT8 IpSuite[OHA_IP_SUITE_SIZE];
            LSA_UINT16 RetVal;

            OHA_MEMCPY(IpSuite, IpAddrPtr, OHA_IP_ADDR_SIZE);
            OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE], SubnetAddrPtr, OHA_IP_ADDR_SIZE);
            OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE], RouterAddrPtr, OHA_IP_ADDR_SIZE);

            OHA_ASSERT(OHA_IS_NULL(pHDBNare->IpCheck.pCheckRQB));
            RetVal = OHA_NARETestIp(NicID, IpSuite,
                                    NARE_IP_ADDR_SIZE, OHA_HANDLE_TYPE_SYS, (LSA_UINT32)OHA_HANDLE_TYPE_SYS);  /* no user */
            if (RetVal == OHA_OK)
            {
                OHA_ASSERT(!pHDBNare->IpCheck.Underway);

                /* OhaOpcSetAddress or OhaOpcDcpResponse */
                pHDBNare->IpCheck.Underway = LSA_TRUE;
                pHDBNare->IpCheck.State    = OHA_CHECK_STATE_START;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"Request OHA_NARETestIp() failed (RetVal: 0x%X)", RetVal);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NARECheckAddress(RetVal: 0x%X)",RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_NARECheckAddressCancel                  +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Orginator            : User or remote DCP client                       +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE        cancel is not active            +*/
/*+                         LSA_FALSE       cancel is active                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Cancels a running check of NameOfStation and IP-address   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_NARECheckAddressCancel(
    LSA_UINT16          NicID,
    LSA_UINT32          CancelOptions,
    LSA_UINT16          Orginator)
{
    LSA_BOOL        Finish = LSA_TRUE;
    OHA_HDB_NARE_PTR_TYPE pHDBNare = OHA_NAREGetHandleByNIC(NicID); /* NARE channel necessary */

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_NARECheckAddressCancel(NicID: 0x%X, CancelOptions: 0x%X, Orginator: 0x%X)", NicID, CancelOptions, Orginator);

    OHA_ASSERT_NULL_PTR(pHDBNare);

    if (CancelOptions & OHA_CTRL_IP_SUITE)
    {
        /* the ip address is to set, first a check-IP (ethernet) is to cancel */
        if (pHDBNare->IpCheck.Underway)
        {
            /* the ip address is to set, cancel first and queue the request */
            OHA_ASSERT(OHA_IS_NULL(pHDBNare->IpCheck.pCancelRQB));
            pHDBNare->IpCheck.pCancelRQB = OHA_NARETestIpCancel(pHDBNare, Orginator, (LSA_UINT32)OHA_HANDLE_TYPE_SYS);
            Finish = LSA_FALSE;
            pHDBNare->IpCheck.State = OHA_CHECK_STATE_WITHDRAW;
        }
        else
        {
            /* trace the state */
            pHDBNare->IpCheck.State = OHA_CHECK_STATE_STOP;
        }
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_LOW,"OHA_NARECheckAddressCancel() info (IpCheck.State: 0x%X)", pHDBNare->IpCheck.State);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_NARECheckAddressCancel(Response: 0x%X)",Finish);
    return(Finish);
}

#endif /* OHA_CFG_NO_NARE */

/*****************************************************************************/
/*  end of file OHA_NARE.C                                                   */
/*****************************************************************************/

