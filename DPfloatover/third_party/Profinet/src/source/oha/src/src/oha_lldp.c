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
/*  F i l e               &F: oha_lldp.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA LLDP functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  9
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_LLDP */

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

LSA_BOOL   OHA_LldpIsSinglePortRequestActive(
    OHA_HDB_LLDP_PTR_TYPE       pHDB)
{
	LSA_UINT16 PortId;
	LSA_BOOL   SinglePortRequestActive = LSA_FALSE;

	/* the single port request flags are at index 1...n */
	for(PortId = 1; PortId <= OHA_CFG_MAX_PORTS; PortId++)
	{
		if(pHDB->StartStopActive[PortId])   /* a single port request is active ? */
		{
			OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_LldpIsSinglePortRequestActive (Handle: 0x%X,LLDPHandle: 0x%X, PortId 0x%X is active!)",pHDB->ThisHandle,pHDB->LldpHandle,PortId);
			SinglePortRequestActive = LSA_TRUE;
		}
	}
	return(SinglePortRequestActive);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_lldp_request_lower_done                 +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for LLDP-requests.                       +*/
/*+               Fills Error-Structure in NDB with LLDP-Response           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LLDP_LOWER_IN_FCT_ATTR oha_lldp_request_lower_done(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_LLDP_PTR_TYPE   pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN :oha_lldp_request_lower_done(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /* LLDP-request always done from an LLDP-Channel. !           */

    pHDB = OHAGetLldpHDBFromHandle(OHA_LLDP_RQB_GET_HANDLE(pRQB));

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->Params.pDetail);

    switch (OHA_LLDP_RQB_GET_OPCODE(pRQB) )
    {
        case LLDP_OPC_OPEN_CHANNEL:
            OHA_LLDPOpenChannelDone(pRQB,pHDB);
            break;
        case LLDP_OPC_CLOSE_CHANNEL:
            OHA_LLDPCloseChannelDone(pRQB,pHDB);
            break;
        case LLDP_OPC_INDICATION_PROVIDE:
            OHA_LLDPRecvDone(pRQB,pHDB);
            break;
        case LLDP_OPC_GET_CONFIG:
            OHA_LLDPGetConfigDone(pRQB,pHDB);
            break;
        case LLDP_OPC_SET_GLOBALS:
            OHA_LLDPSetGlobalsDone(pRQB,pHDB);
            break;
        case LLDP_OPC_SET:
            OHA_LLDPSetDone(pRQB,pHDB);
            break;
        case LLDP_OPC_START:
            OHA_LLDPStartDone(pRQB,pHDB);
            break;
        case LLDP_OPC_STOP:
            OHA_LLDPStopDone(pRQB,pHDB);
            break;
        case LLDP_OPC_STATS:
            OHA_LLDPGetStatsDone(pRQB,pHDB);
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_lldp_request_lower_done()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPChannelDone                         +*/
/*+  Input                      OHA_HDB_LLDP_PTR_TYPE         pHDB          +*/
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
/*+               LLDP-Channels. This function is called after a lower      +*/
/*+               layer called back the OHA (e.g. after LLDP finished his   +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_LLDPChannelDone(
    OHA_HDB_LLDP_PTR_TYPE       pHDB,
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
                          "IN : OHA_LLDPChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB,Response);

    Finish = LSA_FALSE; /* dont finish request by default */
    Release = LSA_FALSE; /* dont release PATH-Info by default */

    OHA_ASSERT_NULL_PTR(pHDB);

    pRQB    = pHDB->pOCReq;
    Cbf     = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
	pSys    = pHDB->Params.pSys;
	pDetail = pHDB->Params.pDetail;
	NICId   = pHDB->Params.pDetail->Params.Lldp.NICId;

    OHA_ASSERT_NULL_PTR(pRQB);

    switch (pHDB->State)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a LLDP-Open Channel request.                        */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_LLDP_RUNNING:
            if ( Response == OHA_OK)
            {
                /* OHA_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail; */

                /* Get MaxSender from LLDP                      */
                pHDB->State  = OHA_HANDLE_STATE_GET_CONFIG_LLDP_RUNNING;

                Response = OHA_LLDPGetConfig(pHDB);
                if ( Response != OHA_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP Issuing GetConfigRequest failed.");

                    pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_LLDP_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Closing Channel because of error.");

                    if ( OHA_LLDPCloseChannel(pHDB->LldpHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close LLDP Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        OHA_FATAL(OHA_FATAL_ERR_LLDP);
                    }
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open LLDP. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a GET-CONFIG LLDP-request.                          */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_GET_CONFIG_LLDP_RUNNING:
            if ( Response == OHA_OK)
            {
				LSA_UINT32	TxInterval = 0;
				LSA_UINT32	ReInitDelay = 0;
				LSA_UINT32	TxDelay     = 0;
				LSA_UINT32	TxHoldMultiplier = 0;

				(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_INTERVAL, (OHA_COMMON_MEM_U32_PTR_TYPE)&TxInterval, LSA_NULL, LSA_NULL);
				(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_REINIT_DELAY, (OHA_COMMON_MEM_U32_PTR_TYPE)&ReInitDelay, LSA_NULL, LSA_NULL);
				(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_DELAY, (OHA_COMMON_MEM_U32_PTR_TYPE)&TxDelay, LSA_NULL, LSA_NULL);
				(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_HOLD_MULTIPLIER, (OHA_COMMON_MEM_U32_PTR_TYPE)&TxHoldMultiplier, LSA_NULL, LSA_NULL);

                /* Set Globals of LLDP      */
                pHDB->State  = OHA_HANDLE_STATE_SET_LLDP_RUNNING;

                /* write the TTL to the local LLDP DB */
                Response = OHA_DbStoreLocalTTL(NICId, (LSA_UINT16)TxInterval,
                                               OHA_LLDP_TTL_FACTOR);
				if ( Response == OHA_OK)
                {
					Response = OHA_LLDPSetGlobals(NICId, 
												  OHA_HANDLE_TYPE_SYS,  
												  TxInterval, 
												  ReInitDelay,       /* Integer32 (1..10 ), DEFVAL { '0' } */
												  TxDelay,           /* Integer32 (1..8192 ), DEFVAL { 2 } */
												  TxHoldMultiplier); /* Integer32 (2..10 ), DEFVAL { 4 }   */
				}
				if ( Response != OHA_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP Issuing SetGlobalsRequest failed.");

                    pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_LLDP_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Closing Channel because of set error.");

                    if ( OHA_LLDPCloseChannel(pHDB->LldpHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close LLDP Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        OHA_FATAL(OHA_FATAL_ERR_LLDP);
                    }
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"LLDP GetConfig request failed. Response: 0x%X", Response);

                pHDB->OpenResponse = Response; /* save for later */

                pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_LLDP_RUNNING;

                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Closing Channel because of error");

                if ( OHA_LLDPCloseChannel(pHDB->LldpHandle,pHDB->Params.pSys) != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close LLDP Channel().");
                    OHA_FATAL(OHA_FATAL_ERR_LLDP);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SET-GLOBALS LLDP-request.                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_SET_LLDP_RUNNING:
            if ( Response == OHA_OK)
            {
                OHA_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail;

                pHDB->State  = OHA_HANDLE_STATE_OPEN; /* for DB write */

                /* The LLDP agent shall send LLDP frames for this port (default) */
				/* AP01201309: see oha_init_defvals() */

                /* NOTE: Providing Recv-Indication Request must be          */
                /*       the last action before going into State "open".    */
                /*       So only Indciations while in Open-state must be    */
                /*       handled and reprovided!                            */

                /* provide LLDP-Recv-Requests, depending on Ports * MaxSenderPerPort. Two per Port */
                /* Response = OHA_LLDPRecvProvide(pHDB,(pHDB->PortCnt * OHA_CFG_LLDP_RECV_RESOURCE_PER_PORT)); */
                if ( Response == OHA_OK )
                {
                    Response = OHA_LLDPRecvProvide(pHDB, pDetail->Params.Lldp.NumOfRecv);
                }

                if ( Response == OHA_OK )
                {
                    /* Channel is open now, no more actions needed. */
                    pHDB->PendingPortId = 0;
                    pHDB->RequestState = OHA_LLDP_STATE_CLOSE; /* initialize */
                    Finish = LSA_TRUE;
                }
                else
                {
                    /* Any Recv-Request failed. Cancel Recv-Requests (by CloseChannel) */
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"LLDP Receive Provide request failed. Response: 0x%X", Response);

                    pHDB->OpenResponse = Response; /* save for later */

                    pHDB->State = OHA_HANDLE_STATE_CLOSE_ERROR_LLDP_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Closing Channel because of error");

                    if ( OHA_LLDPCloseChannel(pHDB->LldpHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close LLDP Channel().");
                        OHA_FATAL(OHA_FATAL_ERR_LLDP);
                    }
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"LLDP GetConfig request failed. Response: 0x%X", Response);

                pHDB->OpenResponse = Response; /* save for later */

                pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_LLDP_RUNNING;

                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Closing Channel because of error");

                if ( OHA_LLDPCloseChannel(pHDB->LldpHandle,pHDB->Params.pSys) != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close LLDP Channel().");
                    OHA_FATAL(OHA_FATAL_ERR_LLDP);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a LLDP-Close-Channel because of error within Open    */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_ERROR_LLDP_RUNNING:
            if ( Response == OHA_OK)
            {
                Response = pHDB->OpenResponse;  /* get Error-Response  */

                /* release PATH-Info ! */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close LLDP Channel().");
                OHA_FATAL(OHA_FATAL_ERR_LLDP);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a LLDP-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_LLDP_RUNNING:

            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot close LLDP. Response: 0x%X", Response);
                /* is this a fatal error ?                      */
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a LLDP-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_STOP_LLDP_RUNNING:

            if ( Response == OHA_OK)
            {
                Response = OHA_LLDPCloseChannel(pHDB->LldpHandle,pHDB->Params.pSys);
                if ( Response == OHA_OK )
                {
                    /* Request is underway now.. */
                    pHDB->State  = OHA_HANDLE_STATE_CLOSE_LLDP_RUNNING;
                }
            }
            else
            {
                /* is this a fatal error ?                      */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Stop LLDP. Response: 0x%X", Response);
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
        OHA_LLDPReleaseHDB(pHDB);
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

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPChannelDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPOpenChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA LLDP-Channel handle                          +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to LLDP. The Request will be  +*/
/*+               finished by calling the LLDP-Callbackfunction.            +*/
/*+               oha_lldp_request_lower_done() if OHA_OK.                  +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB  = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);


    UserId.uvar32 = 0;
    OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_OPEN_CHANNEL);

        pRQB->args.channel.handle_upper  = OHAHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.lldp_request_upper_done_ptr = oha_lldp_request_lower_done;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
        OHA_LLDP_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"LLDP-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPOpenChannel. Rsp: 0x%X",
                          Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPOpenChannelDone                     +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE   pRQB          +*/
/*+                             OHA_HDB_LLDP_PTR_TYPE         pHDB          +*/
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
/*+               - save LLDP-Channel                                       +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPOpenChannelDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16       Response = OHA_OK;
    LSA_UINT16       RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB));

    if ( OHA_LLDP_RQB_GET_RESPONSE(pRQB) == LLDP_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Open Channel successful");
        /* save LLDP-Channel in HDB */
        pHDB->LldpHandle = pRQB->args.channel.handle;

    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-Open Channel failed.[0x%X]",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_LLDPChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPOpenChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPCloseChannel                        +*/
/*+  Input                      LSA_HANDLE_TYPE       LLDPHandle            +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  LLDPHandle            : LLDP handle to close                           +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to LLDP. The Request will be +*/
/*+               finished by calling the LLDP-Callbackfunction.            +*/
/*+               oha_lldp_request_lower_done()                             +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPCloseChannel(
    LSA_HANDLE_TYPE       LLDPHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_LLDP_LOWER_RQB_PTR_TYPE pRQB   = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPCloseChannel(LLDPHandle: 0x%X)",
                          LLDPHandle);

    UserId.uvar32 = 0;
    OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {

        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_CLOSE_CHANNEL);
        OHA_LLDP_RQB_SET_HANDLE(pRQB,LLDPHandle);

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_CLOSE_CHANNEL (LLDPHandle: 0x%X,pRQB: 0x%X)",LLDPHandle,pRQB);
        OHA_LLDP_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"LLDP-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPCloseChannel(Result: 0x%X)", Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPCloseChannelDone                    +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_HDB_LLDP_PTR_TYPE        pHDB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPCloseChannelDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response = OHA_OK;
    LSA_UINT16               RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB));

    if ( OHA_LLDP_RQB_GET_RESPONSE(pRQB) == LLDP_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Close Channel successful");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-Close Channel failed.[0x%X]",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    /* Could all allocated Receive-RQBs be freed ? */
    OHA_ASSERT(pHDB->RxPendingCnt == 0);

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_LLDPChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPCloseChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetHandleByNIC                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_LLDP_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a LLDP HDB with NICId. If none is found   +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_LLDPGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_LLDP_PTR_TYPE pHDB      =   OHA_GET_HLLDP_PTR(0);
    LSA_UINT32            i         =   0;
    LSA_BOOL              Found     =   LSA_FALSE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetHandleByNIC(NICId: 0x%X)", NICId);

    while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
    {
        if (( pHDB->State == OHA_HANDLE_STATE_OPEN || pHDB->State == OHA_HANDLE_STATE_SET_LLDP_RUNNING) &&
            ( pHDB->Params.pDetail->Params.Lldp.NICId == NICId  ))
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

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetHandleByNIC(pHDB:  0x%X)", pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetHandleByIndex                    +*/
/*+  Input/Output          :    LSA_UINT16       Index                      +*/
/*+  Result                :    OHA_HDB_LLDP_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceId      : InterfaceId from Table   1...n                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a LLDP HDB with NICId. If none is found   +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_LLDPGetHandleByIndex(
    LSA_UINT16 Index)
{
    OHA_HDB_LLDP_PTR_TYPE pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetHandleByIndex(Index: 0x%X)",
                          Index);
    if (Index && (Index <= OHA_CFG_MAX_NICS))    /* 0 is not valid */
    {
        pHDB = OHA_GET_HLLDP_PTR(Index-1);
        if ( pHDB && (pHDB->State == OHA_HANDLE_STATE_OPEN) )
        {
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetHandleByIndex(pHDB:  0x%X)", pHDB);
            return(pHDB);
        }
        else
        {
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetHandleByIndex(pHDB:  0x%X)", pHDB);
            return(LSA_NULL);
        }
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetHandleByIndex()");
    return(LSA_NULL);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetHandle                           +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_LLDP_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceId      : InterfaceId from Table   1...n                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a LLDP HDB with NICId. If none is found   +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_LLDPGetHandleByIf(
    LSA_UINT16 IfId)
{
    OHA_HDB_LLDP_PTR_TYPE pHDB = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetHandleByIf(IfId: 0x%X)",
                          IfId);

    if (IfId >= EDD_INTERFACE_ID_MIN && IfId <= EDD_INTERFACE_ID_MAX) /* 0 is not valid */
    {
		LSA_UINT16 NicID = OHA_DB_GET_NIC(IfId);

		if(NicID != OHA_INVALID_NIC_ID)
		{
		    pHDB = OHA_LLDPGetHandleByNIC(NicID);
			if(OHA_IS_NULL(pHDB))
			{
				OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetHandleByIf(pHDB: 0x%X, NicID: 0x%X)", pHDB, NicID);
			}
		}
	}

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetHandleByIf(pHDB:  0x%X)", pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPCheckNICUsed                        +*/
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
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_LLDPCheckNICUsed(
    LSA_UINT16 NICId)
{
    OHA_HDB_LLDP_PTR_TYPE  pHDB  =  OHA_GET_HLLDP_PTR(0);
    LSA_UINT32            i     =   0;
    LSA_BOOL              Found =   LSA_FALSE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPCheckNICUsed(NICId: 0x%X)",
                          NICId);
    if (NICId != OHA_INVALID_NIC_ID)
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS ) )
        {
            if (( pHDB->State != OHA_HANDLE_STATE_FREE ) &&
                ( pHDB->Params.pDetail->Params.Lldp.NICId == NICId  ))
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

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPCheckNICUsed(Found:  0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPReleaseHDB                          +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE pHDB                  +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBLldp    : Pointer to User Handle to release                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release LLDP -HDB                                         +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPReleaseHDB(
    OHA_HDB_LLDP_PTR_TYPE pHDB)
{
    /* LSA_UINT32            i     =   0; */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

#if 0
    for (i = 0; i < OHA_CFG_MAX_PORTS; i++)  /* over all ports */
    {
        /* Last remote TOPO indication existing ? */
        if (! LSA_HOST_PTR_ARE_EQUAL(pHDB->pPeerTopoInd[i], LSA_NULL))
        {
            LSA_UINT16 FreeStat;
            if (! LSA_HOST_PTR_ARE_EQUAL(pHDB->pPeerTopoInd[i]->pRecordData, LSA_NULL))
            {
                OHA_FREE_LOCAL_MEM(&FreeStat, pHDB->pPeerTopoInd[i]->pRecordData);
                OHA_MEM_FREE_FAIL_CHECK(FreeStat);
            }
            OHA_FREE_LOCAL_MEM(&FreeStat, pHDB->pPeerTopoInd[i]);
            OHA_MEM_FREE_FAIL_CHECK(FreeStat);
        }
    }
#endif

    pHDB->State = OHA_HANDLE_STATE_FREE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPReleaseHDB()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetFreeHDB                          +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL..pHDBLldp   +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA LLDP  -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free LLDP -HDB                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPGetFreeHDB(
    OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBLldp)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found = LSA_FALSE;
    OHA_HDB_LLDP_PTR_TYPE      pHDB  = OHA_GET_HLLDP_PTR(0);
    LSA_UINT32                i     = 0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetFreeHDB(pHDB: 0x%X)",
                          pHDBLldp);

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
        *pHDBLldp = pHDB;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPGetFreeHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBLldp);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPRecvProvide                         +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                             LSA_UINT32                   Cnt            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Cnt                  : Number of Recv-Request to send to LLDP          +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends Indication-Resources to LLDP                        +*/
/*+               (LLDP_OPC_INDICATION_PROVIDE)                             +*/
/*+                                                                         +*/
/*+               Allocates RQB and does LLDP-Requests                      +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe RECV-Request pending     +*/
/*+                     within LLDP!                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPRecvProvide(
    OHA_HDB_LLDP_PTR_TYPE    pHDB,
    LSA_UINT32               Cnt)
{
    OHA_LLDP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status;
    LSA_USER_ID_TYPE            UserId;
    OHA_LLDP_LOWER_MEM_PTR_TYPE pTLV = LSA_NULL;  /* Pointer to TLV-Frame memory. */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPRecvProvide(pHDB: 0x%X, Cnt: 0x%X)",
                          pHDB,Cnt);

    OHA_ASSERT_NULL_PTR(pHDB);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Receive-Request to LLDP                                 */
    /*---------------------------------------------------------------------------*/

    pHDB->RxPendingCnt = 0; /* no Rx pending within LLDP */

    while ((Status == OHA_OK) && (Cnt))
    {
        OHA_LLDP_ALLOC_LOWER_MEM(&pTLV,
                                 UserId,
                                 sizeof(LLDP_UPPER_TLV_TYPE),  /* LLDP_MAX_TLV_DATA_SIZE */
                                 pSys);

        if ( LSA_HOST_PTR_ARE_EQUAL(pTLV, LSA_NULL) )
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPRecvProvide(). Allocation TLV memory failed.");
        }
        else
        {
            /* allocate RQB */
            OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
            {
                OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_INDICATION_PROVIDE);
                OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle);

                pRQB->args.IndProvide.pTLV = (LLDP_UPPER_TLV_PTR_TYPE)pTLV;

                OHA_MEMSET(pRQB->args.IndProvide.MACAddr, 0, sizeof(LLDP_MAC_TYPE));

                OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_INDICATION_PROVIDE (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);

                OHA_LLDP_REQUEST_LOWER(pRQB,pSys);
                pHDB->RxPendingCnt++;  /* number of Rx-Request pending within LLDP */
            }
            else
            {
                LSA_UINT16 RetVal = LSA_RET_OK;

                Status = OHA_ERR_RESOURCE;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-Allocating lower RQB-memory failed!");
                OHA_LLDP_FREE_LOWER_MEM(&RetVal,pTLV,pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        Cnt--;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPRecvProvide(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeRemoteSender                        +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Database elements if allocated.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeRemoteSender(
    OHA_LLDP_SENDER_PTR_TYPE pRemoteSender,
    OHA_LOCAL_MEM_U16_PTR_TYPE pCountOfDeletedElem)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FreeRemoteSender(pRemoteSender: 0x%X)", pRemoteSender);

    OHA_ASSERT_NULL_PTR(pCountOfDeletedElem);

    if (OHA_IS_NOT_NULL(pRemoteSender))
    {
        LSA_UINT16 CountOfDeletedElem = 0; /* count of all deleted elements */
        LSA_UINT16 Count = 0;   /* deleted elements of one type */

        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pChassisId, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pPortId, &Count);
        CountOfDeletedElem += Count;
        Count = 0;

        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pTTL, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pPortDescr, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pSystemName, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pSystemDescr, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pSystemCap, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pMgmAddr, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pOrgSpec_Others, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pUnknown_Optionals, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pOrgSpec_802_1, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pOrgSpec_802_3, &Count);
        CountOfDeletedElem += Count;
        Count = 0;
        oha_delete_lldp_DbElement(&pRemoteSender->DbLLDPDU.pOrgSpec_Pno, &Count);
        CountOfDeletedElem += Count;

        if (OHA_IS_NOT_NULL(pCountOfDeletedElem))
        {
            *pCountOfDeletedElem = CountOfDeletedElem;
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_FreeRemoteSender()");

    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetSenderStatus                     +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_LLDP_HDB_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT8                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Converts LLDP-SenderStatus to OhaSenderStatus:            +*/
/*+               If a second sender appears the SenderStatus is set to     +*/
/*+               MultipleSender and vice versa                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT8 OHA_LOCAL_FCT_ATTR OHA_LLDPGetSenderStatus(
    OHA_HDB_LLDP_PTR_TYPE                       pHDB,
    LLDP_UPPER_INDICATION_PROVIDE_PTR_TYPE      pRecv,
    LSA_UINT8                                   OhaSenderStatus)
{
    LSA_UINT8       SenderStatus = OhaSenderStatus;
    LSA_UINT16      SenderId;

	OHA_ASSERT_NULL_PTR(pHDB);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetSenderStatus(pHDB: 0x%X,pRecv: 0x%X,OhaSenderStatus: 0x%X)",
                          pHDB,pRecv,OhaSenderStatus);

    OHA_ASSERT_NULL_PTR(pRecv);

    /* if a multiple sender disappears, determine the sender status for OHA */
    /* This sender is present:                                */
    /* look for further sender in the remote-sender-array !!! */
    if (SenderStatus == OHA_STATUS_DATA_VALID ||
        SenderStatus == OHA_STATUS_DATA_INVALID)
    {
        for (SenderId = 0; SenderId <= LLDP_MAX_PORT_SENDER; SenderId++) /* check it */
        {
            if (pRecv->SenderId && (SenderId != pRecv->SenderId-1) )  /* not the (actual) port-sender */
            {
                OHA_LLDP_SENDER_PTR_TYPE pRemoteSender = &pHDB->pDB->LldpData[pRecv->PORTId-1].RemoteSender[SenderId];
                if (pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID ||
                    pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID)
                {
                    SenderStatus = OHA_STATUS_MULTIPLE_SENDER;
                    break;                                          /* that's it */
                }
            }
        }
    }
    /* This sender is no more present:                        */
    /* look for further sender in the remote-sender-array !!! */
    else if (SenderStatus == OHA_STATUS_NOT_PRESENT)
    {
        for (SenderId = 0; SenderId <= LLDP_MAX_PORT_SENDER; SenderId++) /* check it */
        {
            if (pRecv->SenderId && (SenderId != pRecv->SenderId-1) )  /* not the (actual) port-sender */
            {
                OHA_LLDP_SENDER_PTR_TYPE pRemoteSender = &pHDB->pDB->LldpData[pRecv->PORTId-1].RemoteSender[SenderId];
                if (pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID ||
                    pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID)
                {
                    if (SenderStatus == OHA_STATUS_NOT_PRESENT)
                    {
                        SenderStatus = pRemoteSender->SnmpSenderStatus; /* turn to data(in)valid */
                    }
                    else if (SenderStatus == OHA_STATUS_DATA_INVALID ||
                             SenderStatus == OHA_STATUS_DATA_VALID)     /* already turn to data(in)valid */
                    {
                        SenderStatus = OHA_STATUS_MULTIPLE_SENDER;      /* it's multiple too ! */
                        break;                                          /* that's it */
                    }
                }
            }
        }
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPRecvProvide(SenderStatus: 0x%X)",SenderStatus);
    return(SenderStatus);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetSenderID                         +*/
/*+  Input                      OHA_LLDP_HDB_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+  PORTId                     the port id                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Converts LLDP-SenderID                                    +*/
/*+               If a second sender disappears the SenderID is set to a    +*/
/*+               valid one and vice versa                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPGetSenderID(
    OHA_HDB_LLDP_PTR_TYPE       pHDB,
    LSA_UINT16                  PORTId)
{
    LSA_UINT16      SenderId = 0; /* invalid */
    LSA_UINT16      i;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetSenderStatus(pHDB: 0x%X,PORTId: 0x%X)",
                          pHDB,PORTId);

    OHA_ASSERT_NULL_PTR(pHDB);

    if (PORTId == 0 || PORTId > OHA_CFG_MAX_PORTS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    for (i = 0; i < LLDP_MAX_PORT_SENDER; i++)
    {
        OHA_LLDP_SENDER_PTR_TYPE pRemoteSender = &pHDB->pDB->LldpData[PORTId-1].RemoteSender[i];
        if (pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID ||
            pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID)
        {
            if (SenderId == 0)  /* the first sender we've found */
            {
                SenderId = (LSA_UINT16)(i+1);
            }
            else    /* more than one sender */
            {
                SenderId = 0;
                break;      /* return "invalid" lldp sender */
            }
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPGetSenderID(SenderId: 0x%X)",SenderId);
    return(SenderId);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsEqualAliasName                        +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LSA_TRUE  - the AliasName for SetFilter is equal.         +*/
/*+               LSA_FALSE - the AliasName for SetFilter is not equal.     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_IsEqualAliasName(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortID,
    LSA_UINT16                  SenderId,
	LLDP_TLV_GET_INFO_TYPE OHA_LOCAL_MEM_ATTR *pTLVInfo
)
{
    OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);
    LSA_BOOL IsEqual = LSA_FALSE;

    LSA_UINT8 SenderStatus;

    OHA_COMMON_MEM_U8_PTR_TYPE ChassisPtr = LSA_NULL;
    LSA_UINT16 ChassisLen     = 0;
    LSA_UINT8  ChassisSubType = 0;

	OHA_COMMON_MEM_U8_PTR_TYPE PortPtr = LSA_NULL;
	LSA_UINT16 PortLen        = 0;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsEqualAliasName(NicID: 0x%X, PortID: 0x%X, SenderId: 0x%X)",
                          NICId, PortID, SenderId);

	OHA_ASSERT_NULL_PTR(pTLVInfo);

    OHA_ASSERT_NULL_PTR(pHDBLldp);
    OHA_ASSERT_NULL_PTR(pHDBLldp->pDB);

    OHA_ASSERT (PortID && (PortID <= OHA_CFG_MAX_PORTS));     /* PortID 1 ... n */
    OHA_ASSERT (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER));

	OHA_ASSERT_NULL_PTR(pTLVInfo->ChassisIDString);
	OHA_ASSERT_NULL_PTR(pTLVInfo->PortIDString);

	if (pTLVInfo->ChassisIDSubtype != LLDP_TLV_CHA_SUB_LOCALLY_ASSIGNED ||
		pTLVInfo->PortIDSubtype    != LLDP_TLV_PORT_SUB_LOCALLY_ASSIGNED)
	{
		OHA_PROGRAM_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "~~~No (valid) actual alias present PORTId: 0x%X, SenderId: 0x%X, PortID-Subtype: 0x%X,, ChassisID-Subtype: 0x%X", 
							 PortID,SenderId,pTLVInfo->ChassisIDSubtype,pTLVInfo->PortIDSubtype);

		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsEqualAliasName(UpdateSend: 0x%X)", IsEqual);
		return(IsEqual);
	}

    /* set the sender-status of the port-senders */
    SenderStatus = pHDBLldp->pDB->LldpData[PortID-1].RemoteSender[SenderId-1].SnmpSenderStatus;

    if (SenderStatus == OHA_STATUS_DATA_VALID || SenderStatus == OHA_STATUS_DATA_INVALID)
    {
        (LSA_VOID)oha_db_read_rem_chassis_name(pHDBLldp->pDB->NicId,
                                               PortID, SenderId,
                                               &ChassisSubType,
                                               &ChassisPtr, &ChassisLen);

        if (ChassisLen != 0 && OHA_IS_NOT_NULL(ChassisPtr))
        {
			/* look at the PortID for the AliasName */
			(LSA_VOID)oha_db_read_rem_port_name(pHDBLldp->pDB->NicId,
												   PortID, SenderId,
												   LSA_NULL,
												   &PortPtr, &PortLen);

			if (PortLen != 0 && OHA_IS_NOT_NULL(PortPtr))
			{
				if (ChassisSubType != OHA_CHASSIS_ID_LOCALLY_ASSIGNED)
				{
					OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "~~~No (valid) previous alias present PORTId: 0x%X, SenderId: 0x%X, ChassisID-Subtype: 0x%X", 
										 PortID,SenderId,ChassisSubType);

					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsEqualAliasName(UpdateSend: 0x%X)", IsEqual);
					return(IsEqual);
				}

				if (ChassisLen == pTLVInfo->ChassisIDLen &&
					PortLen    == pTLVInfo->PortIDLen)
				{
					if (OHA_MEMCMP(ChassisPtr, pTLVInfo->ChassisIDString, ChassisLen))
					{
						if (OHA_MEMCMP(PortPtr, pTLVInfo->PortIDString, PortLen))
						{
							IsEqual = LSA_TRUE;
						}
					}
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsEqualAliasName(UpdateSend: 0x%X)", IsEqual);
    return(IsEqual);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsValidNeighbour                        +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LSA_TRUE  - the neighbour has a valid NoS and PortID      +*/
/*+               LSA_FALSE - the neighbour has an invalid NoS or PortID    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_IsValidNeighbour(
	LLDP_TLV_GET_INFO_TYPE OHA_LOCAL_MEM_ATTR *pTLVInfo,
    LSA_BOOL allow_upper_case
)
{
    OHA_COMMON_MEM_U8_PTR_TYPE pChassis;
    LSA_UINT16 ChassisLen;
	OHA_COMMON_MEM_U8_PTR_TYPE pPort;
	LSA_UINT16 PortLen;

    LSA_BOOL MultipleIfMode = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsValidNeighbour(pTLVInfo: 0x%X, allow_upper_case: 0x%X)", pTLVInfo, allow_upper_case);

	OHA_ASSERT_NULL_PTR(pTLVInfo);
	OHA_ASSERT_NULL_PTR(pTLVInfo->ChassisIDString);
	OHA_ASSERT_NULL_PTR(pTLVInfo->PortIDString);

	if (pTLVInfo->ChassisIDLen      > OHA_MAX_CHASSIS_STRLEN ||
		pTLVInfo->PortIDLen         > OHA_MAX_PORT_STRLEN    ||
		pTLVInfo->ChassisIDSubtype != LLDP_TLV_CHA_SUB_LOCALLY_ASSIGNED ||
		pTLVInfo->PortIDSubtype    != LLDP_TLV_PORT_SUB_LOCALLY_ASSIGNED)
	{
		OHA_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "~~~No (valid) actual neighbour present. ChassisIDLen: 0x%X, PortIDLen: 0x%X, PortID-Subtype: 0x%X, ChassisID-Subtype: 0x%X", 
							 pTLVInfo->ChassisIDLen, pTLVInfo->PortIDLen, pTLVInfo->ChassisIDSubtype, pTLVInfo->PortIDSubtype);
		OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsValidNeighbour()");
		return(LSA_FALSE);
	}

	/* MultipleInterfaceMode.NameOfDevice(0): NameOfPort */
	pChassis   = pTLVInfo->ChassisIDString;
	ChassisLen = pTLVInfo->ChassisIDLen;

	/* MultipleInterfaceMode.NameOfDevice(1): NameOfPort + "." + [NameOfStation] */
	for(PortLen=0, pPort=pTLVInfo->PortIDString; PortLen < pTLVInfo->PortIDLen; PortLen++, pPort++)
	{
		if(*pPort == '.')
		{
			MultipleIfMode = LSA_TRUE; /* V2.3 mode */
			pChassis   = ++pPort;
			ChassisLen = (LSA_UINT16)(pTLVInfo->PortIDLen - (PortLen + 1));
			break;
		}
	}

	/* check, if PortID is valid */
	if (!oha_is_valid_port_name(PortLen, (OHA_COMMON_MEM_U8_PTR_TYPE)(&pTLVInfo->PortIDString)))
	{
		OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "Not a vaild remote LLDP_PortID present (PortLen: 0x%X, MultipleIfMode: 0x%X)!", PortLen, MultipleIfMode);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsValidNeighbour(MultipleIfMode: 0x%X)", MultipleIfMode);
		return(LSA_FALSE);
	}

	/* check, if it's a RFC-name */
	if (!oha_is_valid_station_name(ChassisLen, pChassis, allow_upper_case, allow_upper_case))
	{
		OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "Not a vaild remote LLDP_ChassisID present (ChassisLen: 0x%X, MultipleIfMode: 0x%X)!", ChassisLen, MultipleIfMode);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsValidNeighbour(MultipleIfMode: 0x%X)", MultipleIfMode);
		return(LSA_FALSE);
	}

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsValidNeighbour(MultipleIfMode: 0x%X)", MultipleIfMode);
    return(LSA_TRUE);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPRecvDone                            +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             OHA_LLDP_HDB_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LLDP Recv-Request done (Indication received)              +*/
/*+                                                                         +*/
/*+               Copy received frame infos and content to the DataBase     +*/
/*+               within global management (Frame) for further handling.    +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides Receveive-Request to LLDP                      +*/
/*+                                                                         +*/
/*+               If Recv-Request was canceled we free the RQB/Frame and    +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPRecvDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16                                  RetVal = LSA_RET_OK;
    LSA_BOOL                                    Reprovide = LSA_TRUE;
    LLDP_UPPER_INDICATION_PROVIDE_PTR_TYPE      pRecv;
    /* OHA_LLDP_LOWER_MEM_PTR_TYPE              pTLV;   */ /* Pointer to TLV-Frame memory. */
    /* OHA_LLDP_PCTRL_PTR_TYPE                  pPCtrl; */
    /* OHA_COMMON_MEM_U8_PTR_TYPE               pTemp;  */
    LSA_BOOL                                    RemoteChanges = LSA_FALSE;
	LSA_UINT16                                  NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDP_RecvDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    pRecv = &pRQB->args.IndProvide;
    /* pTLV = (OHA_LLDP_LOWER_MEM_PTR_TYPE) pRQB->args.IndProvide.pTLV;          */

    OHA_ASSERT_NULL_PTR(pRecv);
    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->pDB);
    OHA_ASSERT_NULL_PTR(pRecv->pTLV);  /* no frame-buffer for TLV's */

	NICId = pHDB->Params.pDetail->Params.Lldp.NICId;

    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_INDICATION_PROVIDE done (Handle: 0x%X,Rsp: 0x%X,State: 0x%X)",
                       pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB),pHDB->State);

    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the frame and dont reprovide it. This is the case if we are    */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid frame while closing, so CANCEL   */
    /*       may not be enough to catch all rqbs. We have to check the State too */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Recv-Request will be provided in the last */
    /*       State while opening the channel.                                    */

    if ( pHDB->State == OHA_HANDLE_STATE_OPEN )
    {
        switch (OHA_LLDP_RQB_GET_RESPONSE(pRQB))
        {
            case LLDP_RSP_OK:
            {
                /* ---------------------------------------------*/
                /* Successfully done the request.               */
                /* ---------------------------------------------*/
                /* copy framedata to temp-buffer */
                if (pRecv->PORTId && (pRecv->PORTId <= pHDB->pDB->PortCnt)) /* pHDB->PortCnt */
                {
                    OHA_LLDP_SENDER_PTR_TYPE pRemoteSender = LSA_NULL;
                    LSA_UINT8   SenderStatus;       /* OHA_STATUS_NOT_PRESENT */

					LSA_UINT8   Prev_SnmpSenderStatus = OHA_STATUS_NOT_PRESENT; /* see AP01001236: minimize the changes */
					LSA_BOOL    IsEqualAliasName      = LSA_FALSE;              /* for DCP_OPC_FILTER_SET */

                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "~~~LLDPRecvDone: PORTId: 0x%X,SenderId: 0x%X", pRecv->PORTId,pRecv->SenderId);

                    if (pRecv->SenderId && (pRecv->SenderId <= LLDP_MAX_PORT_SENDER)) /* check it */
                    {           /* the (actual) port-sender */
                        pRemoteSender = &pHDB->pDB->LldpData[pRecv->PORTId-1].RemoteSender[pRecv->SenderId-1];
                    }

                    /* store the LLDP event and update statistics */
                    (LSA_VOID)OHA_LLDPUpdateStats(pHDB->pDB, pRecv->PORTId, pRecv->SenderId, pRecv->Event);

                    if (pRecv->SenderId && (pRecv->SenderId <= LLDP_MAX_PORT_SENDER)) /* check it */
                    {   /* write event to DB */
                        OHA_ASSERT_NULL_PTR(pRemoteSender);
                        pRemoteSender->LldpEvent = pRecv->Event;
						Prev_SnmpSenderStatus    = pRemoteSender->SnmpSenderStatus;
                    }

                    switch (pRecv->Event)
                    {
                        case LLDP_EVENT_DATA_VALID:        /* new data */
                        {
                            /* evaluate the TLV */
                            OHA_ASSERT_NULL_PTR(pRecv->pTLV);

                            if ((pRecv->TLVLength && pRecv->SenderId && (pRecv->TLVLength <= LLDP_MAX_TLV_DATA_SIZE)))
                            {
                                if (pRecv->SenderId <= LLDP_MAX_PORT_SENDER /*pHDB->pDB->PortMaxSender*/) /* check it */
                                {
                                    LSA_UINT16              Resp;   /* LLDP_RSP_OK, LLDP_RSP_ERR_PARAM, LLDP_RSP_ERR_TLV_MISMATCH */
                                    LLDP_TLV_GET_INFO_TYPE  TLVInfo;

                                    LSA_UINT32              Length = pRecv->TLVLength;  /* AP00988767: starting length = length of TLV-chain */
                                    OHA_LLDP_LOWER_MEM_PTR_TYPE   pTLV;  /* Pointer to TLV-Frame memory. */

                                    pTLV = (OHA_LLDP_LOWER_MEM_PTR_TYPE) pRQB->args.IndProvide.pTLV;
                                    OHA_ASSERT_NULL_PTR(pTLV);

                                    SenderStatus   = OHA_STATUS_DATA_INVALID;

                                    OHA_ASSERT_NULL_PTR(pRemoteSender);
                                    OHA_MEMCPY(&pRemoteSender->SenderMacAddr, &pRecv->MACAddr, OHA_MAC_ADDR_SIZE);

                                    if (pRecv->Event == LLDP_EVENT_DATA_VALID)
                                    {
                                        TLVInfo.ChassisIDLen = 0;
                                        TLVInfo.ChassisIDSubtype = 0;
                                        TLVInfo.PortIDLen = 0;
                                        TLVInfo.PortIDSubtype = 0;

                                        /* TLV-Info enthält ChassisID, PortID and TTL           */
                                        Resp  = lldp_TLVGetChassisPortTTL(&pTLV,&Length,&TLVInfo);
                                        /* pTLV zeigt hinter TTL-TLV, Length enthält Restlänge  */
                                        if ( Resp == LLDP_RSP_OK)
                                        {
                                            LSA_BOOL ValidNeighbour = OHA_IsValidNeighbour(&TLVInfo, OHA_UPPER_CASES_ALLOWED(NICId));

											/* AliasNameValue := LLDP_PortID + "." + LLDP_ChassisID */
											IsEqualAliasName = OHA_IsEqualAliasName(NICId, 
																					pRecv->PORTId, pRecv->SenderId, &TLVInfo);

                                            /* if (pRemoteSender->LLDPDU.pNameOfChassis && (TLVInfo.ChassisIDLen <= OHA_MAX_CHASSIS_STRLEN)) */
                                            if (TLVInfo.ChassisIDLen <= OHA_MAX_CHASSIS_STRLEN)
                                            {
                                                RetVal = oha_db_write_rem_lldp_element(
                                                             NICId,
                                                             pRecv->PORTId,
                                                             pRecv->SenderId,
                                                             OHA_LLDP_TYPE_CHASSIS_ID,   /* type of variable */
                                                             TLVInfo.ChassisIDSubtype,   /* subtype of variable */
                                                             0,                          /* OUI, if subtype=127 */
                                                             (OHA_COMMON_MEM_U8_PTR_TYPE)(&TLVInfo.ChassisIDString),
                                                             TLVInfo.ChassisIDLen);

                                                if (RetVal != LSA_RET_OK)
                                                {
                                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPRecvDone: oha_db_write_rem_lldp_element(ChassisID) failed (RetVal: 0x%X).", RetVal);
                                                }
                                            }
                                            /* if (pRemoteSender->LLDPDU.pNameOfPort && (TLVInfo.PortIDLen <= OHA_MAX_PORT_STRLEN)) */
                                            if (TLVInfo.PortIDLen <= OHA_MAX_PORT_STRLEN)
                                            {
                                                RetVal = oha_db_write_rem_lldp_element(
                                                             NICId,
                                                             pRecv->PORTId,
                                                             pRecv->SenderId,
                                                             OHA_LLDP_TYPE_PORT_ID,    /* type of variable */
                                                             TLVInfo.PortIDSubtype,    /* subtype of variable */
                                                             0,                        /* OUI, if subtype=127 */
                                                             (OHA_COMMON_MEM_U8_PTR_TYPE)(&TLVInfo.PortIDString),
                                                             TLVInfo.PortIDLen);

                                                if (RetVal != LSA_RET_OK)
                                                {
                                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPRecvDone: oha_db_write_rem_lldp_element(PortID) failed (RetVal: 0x%X).", RetVal);
                                                }
                                            }
                                            /* pRemoteSender->LLDPDU.TimeToLive = TLVInfo.TimeToLive;                  */
                                            RetVal = oha_db_write_rem_lldp_element(NICId,
                                                                                   pRecv->PORTId,
                                                                                   pRecv->SenderId,
                                                                                   OHA_LLDP_TYPE_TTL,      /* type of variable    */
                                                                                   0,                      /* subtype of variable */
                                                                                   0,                      /* OUI, if subtype=127 */
                                                                                   (OHA_COMMON_MEM_U8_PTR_TYPE)(&TLVInfo.TimeToLive),
                                                                                   sizeof(TLVInfo.TimeToLive));

                                            if (RetVal != LSA_RET_OK)
                                            {
                                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPRecvDone: oha_db_write_rem_lldp_element(TimeToLive) failed (RetVal: 0x%X).", RetVal);
                                            }
                                            if (ValidNeighbour)
                                            {   /* check sum of the strings <= 255 */
                                                if ((TLVInfo.ChassisIDLen + TLVInfo.PortIDLen) <= OHA_MAX_STATION_NAME_LEN)
                                                {                                                     /* the 3rd (last) mandatory Option   */
                                                    SenderStatus = OHA_STATUS_DATA_VALID;  /* entry of data in the database */
                                                }

                                                /* only change by a new connection - not by LldpUpdateSend() */
                                                if (pRemoteSender->SnmpSenderStatus != OHA_STATUS_DATA_VALID &&
                                                    pRemoteSender->SnmpSenderStatus != OHA_STATUS_MULTIPLE_SENDER)
                                                {
                                                    pRemoteSender->OidRemEntry.RemIndex++;    /* new connection */
                                                }
                                                /* only change by a new connection - not by LldpUpdateSend() */
                                                if (pRemoteSender->OidRemEntry.RemTimeMark == 0)
                                                {
                                                    pRemoteSender->OidRemEntry.RemTimeMark = OHA_GET_SYS_UPTIME(); /* set timestamp since startup */
                                                }
                                            }

                                            /* rewind */
		                                    Length = pRecv->TLVLength;  /* AP00988767: starting length = length of TLV-chain */
                                            pTLV = (OHA_LLDP_LOWER_MEM_PTR_TYPE) pRQB->args.IndProvide.pTLV;   /* Pointer to TLV-Frame memory. */
                                            OHA_ASSERT_NULL_PTR(pTLV);

                                            {
                                                /* if a multiple sender disappears, determine the sender status for LLDP: */
                                                LSA_UINT8 OhaSenderStatus = OHA_LLDPGetSenderStatus(pHDB, pRecv, SenderStatus);

                                                /* TLV enthält weitere Parameter     */
                                                Resp  = OhaLldpGetParams(NICId, pRecv->PORTId,
                                                                         pRecv->SenderId, OhaSenderStatus, &pTLV, &Length, &RemoteChanges);
                                                if (Resp != LSA_RET_OK)
                                                {
                                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPRecvDone: OhaLldpGetParams failed (Resp: 0x%X).", Resp);
                                                }
                                            }
                                        }
                                    }

                                    /* write the SNMP status for the remote sender(s) */
                                    pRemoteSender->SnmpSenderStatus = SenderStatus;
                                }
                                else  /* there are MultipleSender */
                                {
                                    SenderStatus = OHA_STATUS_MULTIPLE_SENDER;
                                }
                            }
                            else
                            {
                                SenderStatus = LLDP_EVENT_DATA_INVALID;
                                if (OHA_IS_NOT_NULL(pRemoteSender)) /* delete the sender if existing */
                                {
                                    /* write the SNMP status for the remote sender(s) */
                                    pRemoteSender->SnmpSenderStatus = OHA_STATUS_DATA_INVALID;
                                    pRemoteSender->OidRemEntry.RemTimeMark = 0; /* reset */
                                }
                            }
                        }
                        break;
                        case LLDP_EVENT_TOO_MANY_SENDER:  /* no more supported sender (SenderId = 0)! */
                            SenderStatus = OHA_STATUS_MULTIPLE_SENDER;  /* ignore */
                            break;
                            /* LLDP_EVENT_SHUTDOWN: pRecv->TLVLength is NULL ! */
                        case LLDP_EVENT_DATA_INVALID:     /* Timeout or Link Down */
                        case LLDP_EVENT_SHUTDOWN:         /* ShutDown */
                            SenderStatus = OHA_STATUS_NOT_PRESENT;

                            if (OHA_IS_NOT_NULL(pRemoteSender)) /* delete the sender if existing */
                            {
                                LSA_UINT16 CountOfDeletedElem = 0;  /* count statistics */

                                if (OHA_IS_NOT_NULL(&pRemoteSender->DbLLDPDU.pOrgSpec_Pno))
                                {
                                    if (!(OhaListIsEmpty(&pRemoteSender->DbLLDPDU.pOrgSpec_Pno)) )
                                    {
                                        RemoteChanges = LSA_TRUE;
                                    }
                                }

                                /* write the SNMP status for the remote sender(s) */
                                pRemoteSender->SnmpSenderStatus = OHA_STATUS_NOT_PRESENT;
                                pRemoteSender->OidRemEntry.RemTimeMark      = 0; /* reset */

                                OHA_FreeRemoteSender(pRemoteSender, &CountOfDeletedElem);
                                OHA_MEMSET(pRemoteSender, 0, sizeof(OHA_LLDP_SENDER_TYPE));

                                /* reset the parameter LldpEvent! */
                                pRemoteSender->LldpEvent = pRecv->Event;

                                /* count statistics */
                                pHDB->pDB->LldpData[pRecv->PORTId-1].PortStats.RxPortTLVsDiscardedTotal += CountOfDeletedElem;
                            }
                            break;

                            /* AP01522547: loopback function removed */
                        default:
                            SenderStatus = LLDP_EVENT_DATA_INVALID; /* ignore */
                            OHA_FATAL(OHA_FATAL_ERR_LLDP);
                            break;
                    }

                    /* if a multiple sender disappears, determine the sender status for LLDP: */
                    SenderStatus = OHA_LLDPGetSenderStatus(pHDB, pRecv, SenderStatus);

                    /* write the LLDP status for the remote sender(s) */
                    pHDB->pDB->LldpData[pRecv->PORTId-1].LldpSenderStatus = SenderStatus;

                    /* if a multiple sender disappears, determine the sender ID for LLDP: */
                    if (pRecv->SenderId && (pRecv->SenderId <= LLDP_MAX_PORT_SENDER))
                    {
                        LSA_UINT16 LldpSenderId = OHA_LLDPGetSenderID(pHDB, pRecv->PORTId);

						pHDB->pDB->LldpData[pRecv->PORTId-1].LldpSenderId = LldpSenderId;
                    }

                    /* store the last indication in the HDB */
                    pHDB->LastInd.Status = SenderStatus;
                    pHDB->LastInd.PortId = pRecv->PORTId;
                    OHA_MEMCPY(&pHDB->LastInd.SenderMacAddr, &pRecv->MACAddr, OHA_MAC_ADDR_SIZE);

                    if (pRemoteSender)
                    {
						/* for AP01001236: minimize the changes (SnmpSenderStatus or AliasName) */
						if ((Prev_SnmpSenderStatus != pRemoteSender->SnmpSenderStatus) ||
							(!IsEqualAliasName))
						{
							/* update the DCP frame filter if filter is active */
							RetVal = OHA_DCPUpdateFilter(NICId, 0xffffffff, OHA_HANDLE_TYPE_LLDP);
							if (RetVal != OHA_OK)
							{
								OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN, "OUT: OHA_LLDPRecvDone - DCPUpdateFilter(RetVal: 0x%X)", RetVal);
								OHA_ASSERT(RetVal == OHA_OK);
							}
						}

                        /* if OnlySyncForwarding we have to send the RTClass3PortState = OFF, IRDATAUUID = 0 and REDperiod = OFF */
                        /* that means => update (local) LLDP-Tlv LLDP_PNIO_PTCPSTATUS and if changed it's to send */
                        /*            => update (local) LLDP-Tlv LLDP_PNIO_PORTSTATUS and if changed it's to send */
                        if (oha_update_ptcp_status(NICId, pRecv->PORTId, LSA_FALSE))
                        {
                            RemoteChanges = LSA_TRUE;
                        }
                        if (oha_update_port_status(NICId, pRecv->PORTId, LSA_FALSE))
                        {
                            RemoteChanges = LSA_TRUE;
                        }

                        if (RemoteChanges && (pHDB->RequestState == OHA_LLDP_STATE_START ||  /* LLDP-Set is active ? */
											  pHDB->RequestState == OHA_LLDP_STATE_RE_START ||
											  pHDB->RequestState == OHA_LLDP_STATE_WF_START))  /* AP01070629, AP01137963 */
                        {
                            RetVal = OHA_LLDPSet(pHDB, pRecv->PORTId, OHA_HANDLE_TYPE_SYS); /* send them */
                            if (RetVal != LSA_RET_OK)
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPRecvDone: OHA_LLDPSet() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                    }

                    OHA_LldpIndicateEventAllUsers(pHDB, pRecv->PORTId);         /* indication to all users */
                }
                else
                {
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"LLDP Receive - wrong PortId (0x%X)",pRecv->PORTId);
                    OHA_FATAL(OHA_FATAL_ERR_LLDP);
                }
            }
            break;

            case LLDP_RSP_OK_CANCEL:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> OHA_HANDLE_STATE_OPEN  */
                /*       while closing!                             */

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"LLDP Receive canceled (0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"LLDP-ERROR: LLDP_OPC_INDICATION_PROVIDE failed.(0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
                OHA_FATAL(OHA_FATAL_ERR_LLDP);
                break;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"LLDP Receive while not in open-state. discard and dont reprovide.");
        Reprovide = LSA_FALSE;
    }

    if ( Reprovide )
    {
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_INDICATION_PROVIDE (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);
        OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle); /* set handle */
        OHA_LLDP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        pHDB->RxPendingCnt--;  /* number of Recv-Request pending within LLDP */

        OHA_LLDP_FREE_LOWER_MEM(&RetVal,pRQB->args.IndProvide.pTLV,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);

        OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPRecvDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetConfig                           +*/
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
/*+  Description: Get Config from LLDP (e.g. MaxSender)                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPGetConfig(
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    OHA_LLDP_LOWER_RQB_PTR_TYPE                 pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                            pSys;
    LSA_UINT16                                  Status = OHA_OK;
    LSA_USER_ID_TYPE                            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetConfig(pHandle: 0x%X)", pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_GET_CONFIG);
        OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_GET_CONFIG (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);
        OHA_LLDP_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_FATAL,"LLDP-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
        OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPGetConfig(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetConfigDone                       +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPGetConfigDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{

    LSA_UINT16      Response = OHA_OK;
    LSA_UINT16      RetVal   = LSA_RET_OK;
    LSA_UINT16      NICId    = pHDB->Params.pDetail->Params.Lldp.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetConfigDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);


    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_GET_CONFIG done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB));

    switch (OHA_LLDP_RQB_GET_RESPONSE(pRQB))
    {
        case LLDP_RSP_OK:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            OHA_ASSERT_NULL_PTR(pDB);

            /* are there more ports as defined ? */
            if (pRQB->args.GetConfig.PORTCnt > OHA_CFG_MAX_PORTS)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"PortCnt from LLDP (0x%X) > OHA_CFG_MAX_PORTS.",pRQB->args.GetConfig.PORTCnt);
                Response = OHA_ERR_RESOURCE;
            }
            /* are there more portsender as defined ?  */
            /* never mind, one is minimum for PROFINET */
            /* if (pRQB->args.GetConfig.PORTMaxSender > LLDP_MAX_PORT_SENDER) */
            /* {                                                            */
            /*  OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"PORTMaxSender from LLDP (0x%X) > LLDP_MAX_PORT_SENDER.",pRQB->args.GetConfig.PORTMaxSender); */
            /*  Response = OHA_ERR_RESOURCE;                                */
            /* }                                                            */

            if (Response == OHA_OK)
            {
                pDB->PortCnt = pRQB->args.GetConfig.PORTCnt;
                pDB->PortMaxSender = pRQB->args.GetConfig.PORTMaxSender;
                pHDB->pDB = pDB;            /* supply the pointer to the database */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-GET CONFIG PortCnt: %d PORTMaxSender: %d",pDB->PortCnt,pDB->PortMaxSender);
            }
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-ERROR: GET CONFIG failed.(0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_LLDPChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetConfigDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPSetGlobals                          +*/
/*+  Input/Output          :    LSA_UINT16             NicId                +*/
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
/*+  Description: Set Globals of LLDP (TxInterval, ReInitDelay, TxDelay)    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPSetGlobals(
    LSA_UINT16              NicId,
    LSA_UINT16              Orginator,
    LSA_UINT32				TxInterval,
	LSA_UINT32				ReInitDelay,
    LSA_UINT32				TxDelay,
    LSA_UINT32				TxHoldMultiplier
)
{
    OHA_LLDP_LOWER_RQB_PTR_TYPE                 pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                            pSys;
    LSA_UINT16                                  Status = OHA_OK;
    LSA_USER_ID_TYPE                            UserId;
	OHA_HDB_LLDP_PTR_TYPE                       pHDB  = OHA_LLDPGetHandleByNIC(NicId);

    OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPSetGlobals(NicId: 0x%X, Orginator: 0x%X, TxInterval: 0x%X, ReInitDelay: 0x%X, TxDelay: 0x%X, TxHoldMultiplier: 0x%X)", 
						  NicId, Orginator, TxInterval, ReInitDelay, TxDelay, TxHoldMultiplier);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->pDB);

    /* check range */
    if (TxInterval < OHA_LLDP_TX_INTERVAL_MIN || TxInterval > OHA_LLDP_TX_INTERVAL_MAX)  /* TxInterval (5..32768 seconds) */
	{
		Status = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,"TxInterval (0x%X) must be 5..32768 seconds!", TxInterval);
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
			                  "OUT: OHA_LLDPSetGlobals(Status: 0x%X)",Status);
	    return(Status);
	}

	if (TxHoldMultiplier < OHA_LLDP_TTL_FACTOR_MIN || TxHoldMultiplier > OHA_LLDP_TTL_FACTOR_MAX)   /* TxHoldMultiplier (2..10 seconds) */
	{
		Status = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,"TxHoldMultiplier (0x%X) must be 2..10 seconds!", TxHoldMultiplier);
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
			                  "OUT: OHA_LLDPSetGlobals(Status: 0x%X)",Status);
	    return(Status);
	}

	/* Time To Live (2 0ctets) = TxInterval * TxHoldMultiplier */
	if(TxInterval * TxHoldMultiplier > 0xffff)
	{
		Status = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,"Overflow: TimeToLive(0x%X) = TxInterval(0x%X) * TxHoldMultiplier(0x%X) has more than 2 octets!",TxInterval * TxHoldMultiplier, TxInterval, TxHoldMultiplier);
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
			                  "OUT: OHA_LLDPSetGlobals(Status: 0x%X)",Status);
	    return(Status);
	}

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_SET_GLOBALS);
        OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle);
        OHA_LLDP_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_SET_GLOBALS (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);

		pHDB->pDB->TempTxHoldMultiplier     = TxHoldMultiplier;
		pRQB->args.SetGlobals.MsgTxInterval = TxInterval;
		pRQB->args.SetGlobals.ReInitDelay   = ReInitDelay;
		pRQB->args.SetGlobals.TxDelay       = TxDelay;

        OHA_LLDP_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_FATAL,"LLDP-Allocating RQB-memory for global set failed!");
        Status = OHA_ERR_RESOURCE;
        OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPSetGlobals(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPSetGlobalsDone                      +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Set Globals done handling                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPSetGlobalsDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16      Orginator;
    LSA_UINT16      Response = OHA_OK;
    LSA_UINT16      RetVal   = LSA_RET_OK;
    LSA_UINT16      NICId    = pHDB->Params.pDetail->Params.Lldp.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPSetGlobalsDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    Orginator  = OHA_LLDP_RQB_GET_USERID_UVAR16(pRQB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_SET_GLOBALS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB));
    OHA_ASSERT_NULL_PTR(pHDB->pDB);

    switch (OHA_LLDP_RQB_GET_RESPONSE(pRQB))
    {
        case LLDP_RSP_OK:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            OHA_PROGRAM_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-SET GLOBALS done, NICId: 0x%X, TxInterval: %d, ReInitDelay: %d, TxDelay: %d",
								 NICId, pRQB->args.SetGlobals.MsgTxInterval,pRQB->args.SetGlobals.ReInitDelay,pRQB->args.SetGlobals.TxDelay);

			/* write the TTL to the local LLDP DB */
			{
				LSA_UINT8  LldpTTL[LLDP_TLV_TTL_SIZE];
				OHA_UPPER_MEM_U8_PTR_TYPE pLldpTTL = LldpTTL;

				/* Time To Live = TxInterval * TxHoldMultiplier */
				OHA_PUT16_HTON (pLldpTTL, 0, (LSA_UINT16)(pRQB->args.SetGlobals.MsgTxInterval * pHDB->pDB->TempTxHoldMultiplier));

				/* write to LLDP-DB (TimeToLive) */
				RetVal = OHA_DB_WRITE_LLDP_ELEMENT(NICId, 0, OHA_LLDP_TYPE_TTL,
												   0,/*SubType*/ 0,
												   LldpTTL,/* pVariable */
												   LLDP_TLV_TTL_SIZE /* Length */);
				if (RetVal != LSA_RET_OK)
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSetGlobalsDone: LLDP_OPC_SET_GLOBALS(TTL) failed (RetVal: 0x%X).", RetVal);
					Response = OHA_ERR_PARAM;
				}

				/* we update all ports (TTL = TxHoldMultiplier * TxInterval) */
				RetVal = OHA_LLDPUpdateSend(NICId, OHA_CTRL_RESET_TO_FACTORY, OHA_HANDLE_TYPE_SYS);
				if (RetVal != LSA_RET_OK)
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSetGlobalsDone: LLDP_OPC_SET_GLOBALS(UpdateSend) failed (RetVal: 0x%X).", RetVal);
					Response = OHA_ERR_PARAM;
				}
			}
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-ERROR: SET GLOBALS failed.(0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    /* further actions to be done */
    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_SYS:   /* it's an internal Open/Close(Lldp)Channel */
			OHA_LLDPChannelDone(pHDB,Response); /* further actions taken in channel handler */
            break;

        case OHA_HANDLE_TYPE_SOCK:  /* SNMP-Set from cable */
        {
			LSA_HANDLE_TYPE			Handle	= (LSA_HANDLE_TYPE)Orginator;
			OHA_HDB_SOCK_PTR_TYPE	pHDBSock = OHAGetSockHDBFromHandle(Handle); /* not OHA_SOCKGetHandleByNIC()! */

            if (OHA_IS_NOT_NULL(pHDBSock))  /* not closed */
            {
				OHA_HDB_AGNT_PTR_TYPE pSubAgnt = &pHDBSock->LldpAgent;

				OHA_ASSERT(pSubAgnt->StatsPendCnt);
                pSubAgnt->StatsPendCnt--;   /* decrement the active requests */

                if (pSubAgnt->StatsPendCnt == 0)    /* got all necessary statistics or set the globals ? */
                {
					OHA_ASSERT(pSubAgnt->RequestState == OHA_SNMP_STATE_WF_STATISTICS);

					OHA_SNMPSockCheckPending(pHDBSock, OHA_LLDP_MIB_NODE, Response); /* further actions taken in request handler */
					pSubAgnt->RequestState = OHA_SNMP_STATE_START;
                }
            }
        }
        break;

        case OHA_HANDLE_TYPE_USER:  /* from OpcWriteOid */
        {
            LSA_HANDLE_TYPE         Handle  = (LSA_HANDLE_TYPE)Orginator;
            OHA_HDB_USER_PTR_TYPE   pHDBUser = OHA_GetUserHDBFromHandle(Handle);

            if (OHA_IS_NOT_NULL(pHDBUser))  /* not closed */
            {
                if (pHDBUser->StatsPendCnt == 0)    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

                pHDBUser->StatsPendCnt--;   /* decrement the active requests */

                if (pHDBUser->StatsPendCnt == 0)    /* got all necessary statis ? */
                {
                    OHA_SNMPUserCheckPending(pHDBUser, Response); /* further actions taken in request handler */
                }
            }
        }
        break;

        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPSetGlobalsDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpIndication                   +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_LLDP_INDICATION        +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.LldpInd                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide Indication Resources                   +*/
/*+                                                                         +*/
/*+               Queues the RQB wthin User-Handlemanagement                +*/
/*+               If this is the first RQB we check if some indications     +*/
/*+               pending to be indicated. If so we indicate them, if not   +*/
/*+               we queue the request.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    /* LSA_UINT16                  Response = OHA_OK;  */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpIndication(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

    /* queue the request */
    OHA_RQB_PUT_BLOCK_TOP(pHDB->LldpIndReq.pBottom,
                          pHDB->LldpIndReq.pTop,
                          pRQB);

    pHDB->LldpIndReqPending++;

    /* if this is the first Indication Resource, we check if the  */
    /* last indication is pending                                 */

    if ( pHDB->pLastLldpInd )
    {
        LSA_UINT16 NicId = pHDB->Params.pDetail->Params.User.NICId; /* get the LLDP-LDB for this NIC-ID */
        OHA_HDB_LLDP_PTR_TYPE pHDBLldp  = OHA_LLDPGetHandleByNIC(NicId);

        if (pHDBLldp)
        {
            (LSA_VOID)OHA_LldpIndicateEvent(pHDBLldp, pHDB);
        }
    }

    /* if (Response != OHA_OK)                      */
    /* {                                            */
    /*  OHA_UserRequestFinish(pHDB,pRQB,Response);  */
    /* }                                            */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpIndication()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPSet                                 +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+  Input/Output          :    LSA_UINT16                   PortId         +*/
/*+  Input/Output          :    LSA_UINT8                    Orginator      +*/
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
/*+  Description: Get Config from LLDP (e.g. MaxSender)                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPSet(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   Orginator)
{
    OHA_LLDP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = LSA_RET_OK;
    LSA_UINT16                  RetVal = LSA_RET_OK;
    LSA_USER_ID_TYPE            UserId;
    LSA_UINT16                  NicId = pHDB->Params.pDetail->Params.Lldp.NICId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPSet(pHDB: 0x%X, PortId: 0x%X, Orginator: 0x%X)", pHDB, PortId, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->pDB);

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    if ((!PortId) || (PortId > OHA_CFG_MAX_PORTS))
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    else
    {
		switch (Orginator) /* AP01001236 */
		{
			case OHA_HANDLE_TYPE_LLDP: /* user driven LldpSet */
			case OHA_HANDLE_TYPE_USER:
				pHDB->Set_IsActual[PortId-1] = LSA_TRUE;
				break;
			case OHA_HANDLE_TYPE_SYS: /* update only */
			case OHA_HANDLE_TYPE_DCP:
				if(pHDB->Set_PendingCnt[PortId-1] == 0)
				{
					pHDB->Set_IsActual[PortId-1] = LSA_TRUE;
				}
				else /* Set(s) already active, do not update */
				{
					pHDB->Set_IsActual[PortId-1] = LSA_FALSE;
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,"OHA_LLDPSet already pending (PortId: 0x%X, PendingCnt: 0x%X).", PortId, pHDB->Set_PendingCnt[PortId-1]);
				    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPSet(Status: 0x%X)",Status);
				    return(Status);
				}
				break;
			default:
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				break;
		}

        OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
        {
            OHA_LLDP_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
            /* allocate NodePtr memory */
            OHA_LLDP_ALLOC_LOWER_MEM(&MemPtr, UserId, LLDP_MAX_TLV_DATA_SIZE, pSys);

            if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
            {
                LSA_UINT16              Response;
                LLDP_TLV_SET_INFO_TYPE  TLVSetInfo;
                LSA_UINT32              TLength = 0;
                LSA_UINT8               SubType = 0;
                OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                LSA_UINT16               Len    = 0;

                TLVSetInfo.pChassisIDString = LSA_NULL;
                TLVSetInfo.pPortIDString = LSA_NULL;
                TLVSetInfo.PortIDLen = 0;

				/* read the chassisname */
				TLVSetInfo.ChassisIDLen = OHA_DB_READ_LLDP_CHASSIS_ID(NicId,
																	  (OHA_COMMON_MEM_U8_PTR_TYPE*)(&TLVSetInfo.pChassisIDString),
																	  &TLVSetInfo.ChassisIDSubtype);

				(LSA_VOID)oha_db_read_loc_lldp_element(NicId, 0, OHA_LLDP_TYPE_TTL, &SubType, 0,
														&pVar, &Len);
				OHA_ASSERT_NULL_PTR(pVar);
				OHA_ASSERT(Len == LLDP_TLV_TTL_SIZE);

                TLVSetInfo.TimeToLive         = OHA_GET16_NTOH (pVar, 0);
		        OHA_ASSERT(TLVSetInfo.TimeToLive);

                /* read the port name */
                TLVSetInfo.PortIDLen = OHA_DB_READ_LLDP_PORT_ID(NicId, PortId, (OHA_COMMON_MEM_U8_PTR_TYPE*)(&TLVSetInfo.pPortIDString));
                TLVSetInfo.PortIDSubtype   = LLDP_TLV_PORT_SUB_LOCALLY_ASSIGNED;       /* take the 7 */

                if (TLVSetInfo.ChassisIDLen && (!LSA_HOST_PTR_ARE_EQUAL(TLVSetInfo.pChassisIDString, LSA_NULL)) &&
                    TLVSetInfo.PortIDLen    && (!LSA_HOST_PTR_ARE_EQUAL(TLVSetInfo.pPortIDString, LSA_NULL)))
                {
                    LLDP_UPPER_MEM_PTR_TYPE     pTLV = (LLDP_UPPER_MEM_PTR_TYPE)MemPtr;

                    pRQB->args.Set.pTLV          = pTLV;  /* supply here, it will be changed !!! */
                    Response = OhaLldpSetChassisPortTTL(&pTLV,
                                                        &TLength,
                                                        &TLVSetInfo,
														pHDB->pDB->MultipleIfModeValid);
                    if ( Response == LSA_RET_OK)
                    {
                        /* port description, if existing */
                        LSA_UINT16 RetVal;
						LSA_UINT32 TxEnable = OHA_LLDP_TLVS_TX_DISABLE;

						(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_LLDP_TLVS, 
																 &TxEnable, LSA_NULL, LSA_NULL);

						if(TxEnable & OHA_LLDP_PORT_DESC_TX)
						{
							/* read from LLDP-DB (Port Description) */
							pVar = LSA_NULL;
							Len = 0;
							SubType = 0;
							(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_PORT_DESCR,
																   &SubType, 0, &pVar, &Len);

							/* set the Port Description for the lldp send request */
							if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) && Len && Len <= LLDP_TLV_PORT_DESCR_MAX_SIZE)
							{
								LLDP_PARAMS_PORT_DESCR_TYPE TLVParams;

								TLVParams.PortDescriptionLen = Len;
								OHA_MEMCPY(&TLVParams.PortDescription, pVar, Len);

								RetVal = lldp_TLVSetPortDescr(&pTLV, &TLength, &TLVParams);
								if (RetVal != LLDP_RSP_OK)
								{
									OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSet: lldp_TLVSetPortDescr() failed (RetVal: 0x%X).", RetVal);
								}
							}
						}

						if(TxEnable & OHA_LLDP_SYS_NAME_TX)
						{
							/* read from LLDP-DB (System Name) */
							pVar = LSA_NULL;
							Len = 0;
							SubType = 0;
							(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_SYSTEM_NAME,
																   &SubType, 0, &pVar, &Len);

							/* set the System Name for the lldp send request */
							if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) && Len && Len <= LLDP_TLV_SYSTEM_NAME_MAX_SIZE)
							{
								LLDP_PARAMS_SYSTEM_NAME_TYPE TLVParams;

								TLVParams.SystemNameLen = Len;
								OHA_MEMCPY(&TLVParams.SystemName, pVar, Len);
								RetVal = lldp_TLVSetSystemName(&pTLV, &TLength, &TLVParams);
								if (RetVal != LLDP_RSP_OK)
								{
									OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSet: lldp_TLVSetSystemName() failed (RetVal: 0x%X).", RetVal);
								}
							}
						}

						if(TxEnable & OHA_LLDP_SYS_DESC_TX)
						{
							/* read from LLDP-DB (System Descr) */
							pVar = LSA_NULL;
							Len = 0;
							SubType = 0;
							(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_SYSTEM_DESCR,
																   &SubType, 0, &pVar, &Len);

							/* set the System Description for the lldp send request */
							if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) && Len && Len <= LLDP_TLV_SYSTEM_DESCR_MAX_SIZE)
							{
								LLDP_PARAMS_SYSTEM_DESCR_TYPE TLVParams;

								TLVParams.SystemDescrLen = Len;
								OHA_MEMCPY(&TLVParams.SystemDescr, pVar, Len);
								RetVal = lldp_TLVSetSystemDescr(&pTLV, &TLength, &TLVParams);
								if (RetVal != LLDP_RSP_OK)
								{
									OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSet: lldp_TLVSetSystemDescr() failed (RetVal: 0x%X).", RetVal);
								}
							}
						}

						if(TxEnable & OHA_LLDP_SYS_CAP_TX)
						{
							/* read from LLDP-DB (System Capabilities) */
							pVar = LSA_NULL;
							Len = 0;
							SubType = 0;
							(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_SYSTEM_CAP,
																   &SubType, 0, &pVar, &Len);

							/* set the System Capabilities for the lldp send request */
							{
								LLDP_PARAMS_SYSTEM_CAP_TYPE TLVParams;

								if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) && Len == LLDP_TLV_SYSTEM_CAP_SIZE)
								{
									TLVParams.SystemCapabilities  = OHA_GET16_NTOH (pVar, 0);
									pVar += 2;      /* 2nd word */
									TLVParams.EnabledCapabilities = OHA_GET16_NTOH (pVar, 0);
								}
								else
								{
									TLVParams.SystemCapabilities  = 0;
									TLVParams.EnabledCapabilities = 0;
								}
								RetVal = lldp_TLVSetSystemCap(&pTLV, &TLength, &TLVParams);
								if (RetVal != LLDP_RSP_OK)
								{
									OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSet: lldp_TLVSetSystemCap() failed (RetVal: 0x%X).", RetVal);
								}
							}
						}

						(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_LLDP_MAN_ADDR, 
																 &TxEnable, LSA_NULL, LSA_NULL);

						if(TxEnable == OHA_LLDP_TX_ENABLE)
						{
							/* read from DB (IP Address) */
							pVar = LSA_NULL;
							Len = (LSA_UINT16)OHA_DB_READ_IP_SUITE(NicId, &pVar, LSA_NULL, LSA_NULL);

							/* set the Management Address for the lldp send request */
							if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) && (Len == OHA_IP_ADDR_SIZE))
							{
								LLDP_PARAMS_MGM_ADDR_TYPE TLVParams;
								LSA_UINT8 LocManAddrOID[32];
								LSA_UINT LocManAddrOID_Len = sizeof(LocManAddrOID);

								TLVParams.MgmtAddrStringLen = 0; /* init. */

								/* create object id reflecting this lldpLocManAddr table entry */
								(LSA_VOID)oha_lldp_objid_to_asn1(LocManAddrOID, &LocManAddrOID_Len);

								/* AP01116719: MAC if no IP existing */
								if (OHA_MEMCMP(pVar,(OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP,OHA_IP_ADDR_SIZE)) /* IP is 0.0.0.0 ? */
								{
									/* read from DB (MAC Address) */
									Len = (LSA_UINT16)OHA_DB_READ_MAC_ADDR(NicId, &pVar);
									if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) && Len == OHA_MAC_ADDR_SIZE) /* no MAC */
									{
										TLVParams.MgmtAddrStringLen = OHA_MAC_ADDR_SIZE + sizeof(TLVParams.MgmtAddrSubtype);
										TLVParams.MgmtAddrSubtype   = OHA_ADDR_FAMILY_MAC_ADDR; /* MAC address */
										OHA_MEMCPY(&TLVParams.MgmtAddress, pVar, Len);
									}
								}
								else
								{
									TLVParams.MgmtAddrStringLen = OHA_IP_ADDR_SIZE + sizeof(TLVParams.MgmtAddrSubtype);
									TLVParams.MgmtAddrSubtype   = OHA_ADDR_FAMILY_IPV4; /* IP version 4 */
									OHA_MEMCPY(&TLVParams.MgmtAddress, pVar, Len);
								}

								if(TLVParams.MgmtAddrStringLen)	/* mngmt address written ? */
								{
									OHA_HDB_EDD_PTR_TYPE  pHDBEdd = OHA_EDDGetHandleByNIC(pHDB->pDB->NicId);

									TLVParams.InterfaceNumberingSubtype = LLDP_TLV_MGMADDR_IFNUM_IFINDEX;
									if (OHA_IS_NOT_NULL(pHDBEdd)) /* AP01423589: The ifIndex of the layer 2 interface */
									{
										TLVParams.InterfaceNumber = pHDBEdd->Mib2IfIndex[0]; /* 0 ... interface */
									}
									else
									{
										TLVParams.InterfaceNumber = 0;
									}
									TLVParams.OIDStringLen      = (LSA_UINT8)(LocManAddrOID_Len);

									/* copy oid to pdu */
									OHA_MEMCPY(TLVParams.OIDIdentifier, LocManAddrOID, LocManAddrOID_Len);

									RetVal = lldp_TLVSetMgmAddr(&pTLV, &TLength, &TLVParams);
									if (RetVal != LLDP_RSP_OK)
									{
										OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSet: lldp_TLVSetMgmAddr() failed (RetVal: 0x%X).", RetVal);
									}
								}
							}
						}

                        /* unknown optional TLV's (Type 9...126) */
                        RetVal = OhaLldpSetUnknownOptionals(NicId, PortId, &pTLV, &TLength);
                        if (RetVal != LSA_RET_OK)
                        {
                            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                                  "OHA_LLDPSet() failed by optional TLV's (Type 9...126). (RetVal: 0x%X)",RetVal);
                        }

                        /* PPNO specific TLV's (port delay, port status) */
                        RetVal = OhaLldpSetPNOSpecifics(NicId, PortId, &pTLV, &TLength);
                        if (RetVal != LSA_RET_OK)
                        {
                            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                                  "OHA_LLDPSet() failed by manufactural specific TLV's. (RetVal: 0x%X)",RetVal);
                        }

                        /* organizationally defined other TLV's */
                        RetVal = OhaLldpSetOrgDefOthers(NicId, PortId, &pTLV, &TLength);
                        if (RetVal != LSA_RET_OK)
                        {
                            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                                  "OHA_LLDPSet() failed by organizationally defined other TLV's. (RetVal: 0x%X)",RetVal);
                        }

                        /* IEEE 802.1 extensions (VLanID) */
                        RetVal = OhaLldpSetIEEE802_1Extensions(NicId, PortId, &pTLV, &TLength);
                        if (RetVal != LSA_RET_OK)
                        {
                            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                                  "OHA_LLDPSet() failed by IEEE 802.1 TLV's. (RetVal: 0x%X)",RetVal);
                        }

                        /* IEEE 802.3 extensions (MauType,...) */
                        RetVal = OhaLldpSetIEEE802_3Extensions(NicId, PortId, &pTLV, &TLength);
                        if (RetVal != LSA_RET_OK)
                        {
                            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                                  "OHA_LLDPSet() failed by IEEE 802.3 TLV's. (RetVal: 0x%X)",RetVal);
                        }
                    }

                    if ( Response == LSA_RET_OK)
                    {
                        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_SET);
                        OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle);
                        OHA_LLDP_RQB_SET_USERID_UVAR16(pRQB,Orginator);

                        pRQB->args.Set.PORTId        = PortId;
                        pRQB->args.Set.TLVLength     = TLength;

						pHDB->Set_PendingCnt[PortId-1]++; /* AP01001236 */

                        OHA_LOWER_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH
                                           ,">>>: LLDP-Request: LLDP_OPC_SET (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X) PortId(%u)"
                                           ,pHDB->ThisHandle,pHDB->LldpHandle,pRQB, PortId
                                          );
                        OHA_LLDP_REQUEST_LOWER(pRQB,pSys);
                    }
                    else
                    {
                        /* ... free data buffer of the Receive-RQB */
                        OHA_LLDP_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
                        OHA_MEM_FREE_FAIL_CHECK(RetVal);
                        OHA_LLDP_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
                        OHA_MEM_FREE_FAIL_CHECK(RetVal);
                        Status = Response;
                        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                              "lldp_TLVSetChassisPortTTL() failed. (0x%X)",pTLV);
                    }
                }
                else
                {
                    /* ... free data buffer of the Receive-RQB */
                    OHA_LLDP_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
                    OHA_MEM_FREE_FAIL_CHECK(RetVal);
                    OHA_LLDP_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
                    OHA_MEM_FREE_FAIL_CHECK(RetVal);
                    Status = LSA_RET_ERR_PARAM;
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                          "READ_DB(Chassis, Port failed. (0x%X)",MemPtr);
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSet(). Allocation TLV memory failed.");
                OHA_LLDP_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
                Status = LSA_RET_ERR_RESOURCE;
                OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
            }
        }
        else
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_FATAL,"LLDP-Allocating RQB-memory failed!");
            Status = LSA_RET_ERR_RESOURCE;
            OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
        }
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPSet(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPSetDone                             +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPSetDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              Orginator;
    OHA_LLDP_LOWER_MEM_PTR_TYPE MemPtr;  /* Pointer to LLDP-Frame memory. */
    LSA_UINT16              PortId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPSetDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    Orginator  = OHA_LLDP_RQB_GET_USERID_UVAR16(pRQB);

	PortId     = pRQB->args.Set.PORTId; /* AP01001236 */
    if ((!PortId) || (PortId > OHA_CFG_MAX_PORTS))
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_SET done (Handle: 0x%X,Rsp: 0x%X, PortId: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB),PortId);

	OHA_ASSERT(pHDB->Set_PendingCnt[PortId-1]);
	pHDB->Set_PendingCnt[PortId-1]--; /* AP01001236 */

    switch (OHA_LLDP_RQB_GET_RESPONSE(pRQB))
    {
        case LLDP_RSP_OK:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-ERROR: GET CONFIG failed.(0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    /* ... free data buffer of the Receive-RQB */
    MemPtr = pRQB->args.Set.pTLV;
    OHA_LLDP_FREE_LOWER_MEM(&RetVal,MemPtr,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

	if(!pHDB->Set_IsActual[PortId-1]) /* AP01001236 */
	{
		if(pHDB->Set_PendingCnt[PortId-1] == 0) /* update now */
		{
            if (pHDB->RequestState == OHA_LLDP_STATE_START ||  /* LLDP-Set is possible ? */
				pHDB->RequestState == OHA_LLDP_STATE_RE_START || 
				pHDB->RequestState == OHA_LLDP_STATE_WF_START)  /* AP01070629, AP01137963 */
			{
				RetVal = OHA_LLDPSet(pHDB, PortId, OHA_HANDLE_TYPE_SYS);
				if (RetVal != LSA_RET_OK)
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPSetDone: OHA_LLDPSet failed (RetVal: 0x%X).", RetVal);
				}
			}
		}
	}

    switch (Orginator)
    {
        case OHA_HANDLE_TYPE_LLDP:
            OHA_LLDPRequestHandler(pHDB, LSA_NULL, Response); /* further actions taken in request handler */
            break;
        case OHA_HANDLE_TYPE_USER:    /* do nothing: */
            if (pHDB->StartingPortId == 0)  OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

            pHDB->StartingPortId--; /* decrement the active requests */

            if (pHDB->StartingPortId == 0)  /* all ports with link up are set ? */
            {
                OHA_LLDPRequestHandler(pHDB, LSA_NULL, Response); /* further actions taken in request handler */
            }
            break;
        case OHA_HANDLE_TYPE_SYS:     /* do nothing: */
        case OHA_HANDLE_TYPE_DCP:     /* only updates the send-data by change of NameOfStation */
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPSetDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPStartAll                            +*/
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
/*+  Description: - Starts the sending of the local neighbour-data for      +*/
/*+                 ports to start (AdminStatus = "DoSend") or              +*/
/*+               - Sets the local neighbour-data for ports to stop         +*/
/*+                 (AdminStatus = "DoNotSend")                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPStartAll(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    LSA_UINT16                   Orginator)
{
    LSA_UINT16              i, RetVal = OHA_OK;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
	LSA_UINT16              NicID;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPStartAll(pHandle: 0x%X, Orginator: 0x%X)",
                          pHDBLldp, Orginator);

    OHA_ASSERT_NULL_PTR(pHDBLldp);
    OHA_ASSERT_NULL_PTR(pHDBLldp->pDB);
    OHA_ASSERT_NULL_PTR(pHDBLldp->Params.pDetail);

	NicID    = pHDBLldp->Params.pDetail->Params.Lldp.NICId;
    pHDBEdd  = OHA_EDDGetHandleByNIC(NicID);
    OHA_ASSERT_NULL_PTR(pHDBEdd);

    LSA_UNUSED_ARG(Orginator);
    pHDBLldp->StartingPortId = 0;

    for (i = 0; i < pHDBLldp->pDB->PortCnt; i++)
    {
        LSA_UINT16 LinkState;
        LSA_UINT16 SendMode;
		LSA_UINT32 AdminStatus = 0;

        RetVal = OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicID, (LSA_UINT16)(i+1), OHA_CTRL_ADMIN_STATUS, 
												 &AdminStatus, LSA_NULL, LSA_NULL);
		OHA_ASSERT(RetVal == LSA_RET_OK);

        LinkState = OHA_GetLinkStatus(pHDBEdd->LinkStatus[i].LinkStatus);

        /* only activate send if the AdminStatus is txOnly(1) or txAndRx(3) ! */
        SendMode = OHA_AdminStatusToSendMode(AdminStatus);

        /* LLDP-START (activate lldp send) or LLDP-SET (passivate lldp send) */
        if (SendMode == OHA_LLDP_SEND_ON)
        {
            /* only activate send if the link is up ! */
            if (LinkState == OHA_PORT_LINK_STATE_UP)
            {
                RetVal = OHA_LLDPStart(pHDBLldp, (LSA_UINT16)(i+1), (LSA_UINT16)AdminStatus, OHA_HANDLE_TYPE_LLDP);  /* next action is LLDP_START */
                if (RetVal != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPStart: OHA_LLDPStart() failed (RetVal: 0x%X).", RetVal);
                    break;
                }

                pHDBLldp->StartingPortId++; /* count the active requests */
            }
        }
        else    /* only LLDP-SET (passivate lldp send) */
        {
            RetVal = OHA_LLDPSet(pHDBLldp, (LSA_UINT16)(i+1), OHA_HANDLE_TYPE_USER); /* send them */
            if (RetVal != LSA_RET_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPStart: OHA_LLDPSet() failed (RetVal: 0x%X).", RetVal);
            }

            pHDBLldp->StartingPortId++; /* count the active requests */
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPStart(RetVal: 0x%X)",RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPStart                               +*/
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
/*+  Description: Starts the sending of the local neighbour-data for one    +*/
/*+               port                                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPStart(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   AdminStatus,
    LSA_UINT16                   Orginator)
{
    OHA_LLDP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = OHA_OK;
    LSA_USER_ID_TYPE            UserId;
    LSA_UINT8                   Mode = 0;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPStart(pHandle: 0x%X, PortId: 0x%X, Orginator: 0x%X, AdminStatus: 0x%X)",
                          pHDB, PortId, Orginator, AdminStatus);

    OHA_ASSERT_NULL_PTR(pHDB);
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

	switch(AdminStatus)
	{
		case OHA_LLDP_SEND_ONLY:
			Mode = LLDP_TX_MODE;
		break;
		case OHA_LLDP_RECV_ONLY:
			Mode = LLDP_RX_MODE;
		break;
		case OHA_LLDP_SEND_RECV:
			Mode = LLDP_RXTX_MODE;
		break;
		case OHA_LLDP_DISABLED:
		default:
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPStart failed (AdminStatus: 0x%X).", AdminStatus);
			OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
		break;
	}

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_START);
        OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle);
        OHA_LLDP_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        pRQB->args.Start.PORTId    = PortId;
        pRQB->args.Start.Mode      = Mode;

        if (!pHDB->StartStopActive[PortId]) /* request is inactive ? */
        {
            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_START (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);
            pHDB->StartStopActive[PortId] = LSA_TRUE;
            OHA_LLDP_REQUEST_LOWER(pRQB,pSys);
        }
        else    /* no send */
        {
            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Queue: LLDP_OPC_START (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);
            OHA_RQB_PUT_BLOCK_TOP(pHDB->LldpRQBs[PortId].pBottom,
                                  pHDB->LldpRQBs[PortId].pTop,
                                  pRQB);
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_FATAL,"LLDP-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
        OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPStart(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPStartDone                           +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPStartDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              Orginator;
    LSA_UINT16              RetVal = LSA_RET_OK;
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pBottomRQB;
    LSA_UINT16              PortId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPStartDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_SET done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB));

    Orginator  = OHA_LLDP_RQB_GET_USERID_UVAR16(pRQB);
    PortId     = pRQB->args.Start.PORTId;

    OHA_ASSERT_NULL_PTR(pHDB);
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    switch (OHA_LLDP_RQB_GET_RESPONSE(pRQB))
    {
        case LLDP_RSP_OK:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-ERROR: LLDP SET failed.(0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    /* call next request if queued */
    OHA_RQB_REM_BLOCK_BOTTOM(pHDB->LldpRQBs[PortId].pBottom,
                             pHDB->LldpRQBs[PortId].pTop,
                             pBottomRQB);

    if (OHA_IS_NOT_NULL(pBottomRQB))
    {
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_START (Handle: 0x%X,LLDPHandle: 0x%X,pBottomRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pBottomRQB);
        OHA_LLDP_REQUEST_LOWER(pBottomRQB, pHDB->Params.pSys);
    }
    else
    {
        pHDB->StartStopActive[PortId] = LSA_FALSE;

        /* AP01010944 / TFS 828801: call next request (all ports) if no other single port requests queued */
		if (!OHA_LldpIsSinglePortRequestActive(pHDB))
		{
			OHA_RQB_REM_BLOCK_BOTTOM(pHDB->LldpRQBs[0].pBottom,
									 pHDB->LldpRQBs[0].pTop,
									 pBottomRQB);

			if (OHA_IS_NOT_NULL(pBottomRQB)) /* request (all ports) is queued ? */
			{
				OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE_HIGH,">>>: LLDP-Request: LLDP_OPC_STOP-AllPorts (Handle: 0x%X,LLDPHandle: 0x%X,pBottomRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pBottomRQB);
				OHA_LLDP_REQUEST_LOWER(pBottomRQB, pHDB->Params.pSys);
			}
		}
    }

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_LLDP:
            if (pHDB->StartingPortId == 0)  OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

            pHDB->StartingPortId--; /* decrement the active requests */

            if (pHDB->StartingPortId == 0)  /* all ports with link up are sending ? */
            {
                OHA_LLDPRequestHandler(pHDB, LSA_NULL, Response); /* further actions taken in request handler */
            }
        break;
        case OHA_HANDLE_TYPE_EDD:     /* only updates the send-data by change to link up status */
            break;
        case OHA_HANDLE_TYPE_USER:  /* OpcLldpControl or PrmCommit from the User */
		if(OHA_IS_NOT_NULL(pHDB->UserRQBs.pBottom))
		{
            OHA_LLDPUserCheckPending(pHDB, Response);
		}
		else  /* from OpcWriteOid */
        {
            LSA_HANDLE_TYPE         Handle  = (LSA_HANDLE_TYPE)Orginator;
            OHA_HDB_USER_PTR_TYPE   pHDBUser = OHA_GetUserHDBFromHandle(Handle);

            if (OHA_IS_NOT_NULL(pHDBUser))  /* not closed */
            {
                if (pHDBUser->StatsPendCnt == 0)    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

                pHDBUser->StatsPendCnt--;   /* decrement the active requests */

                if (pHDBUser->StatsPendCnt == 0)    /* got all necessary statis ? */
                {
                    OHA_SNMPUserCheckPending(pHDBUser, Response); /* further actions taken in request handler */
                }
            }
        }
        break;
        case OHA_HANDLE_TYPE_SOCK:  /* SNMP-Set from cable */
        {
			LSA_HANDLE_TYPE			Handle	= (LSA_HANDLE_TYPE)Orginator;
			OHA_HDB_SOCK_PTR_TYPE	pHDBSock = OHAGetSockHDBFromHandle(Handle); /* not OHA_SOCKGetHandleByNIC()! */

            if (OHA_IS_NOT_NULL(pHDBSock))  /* not closed */
            {
				OHA_HDB_AGNT_PTR_TYPE pSubAgnt = &pHDBSock->LldpAgent;

				OHA_ASSERT(pSubAgnt->StatsPendCnt);
                pSubAgnt->StatsPendCnt--;   /* decrement the active requests */

                if (pSubAgnt->StatsPendCnt == 0)    /* got all necessary statistics or set the globals ? */
                {
					OHA_ASSERT(pSubAgnt->RequestState == OHA_SNMP_STATE_WF_STATISTICS);

					OHA_SNMPSockCheckPending(pHDBSock, OHA_LLDP_MIB_NODE, Response); /* further actions taken in request handler */
					pSubAgnt->RequestState = OHA_SNMP_STATE_START;
                }
            }
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPStartDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPStop                                +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                             LSA_UINT16                   Orginator      +*/
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
/*+  Description: Stops the sending of the local neighbour-data for all     +*/
/*+               ports                                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPStop(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   AdminStatus,
    LSA_UINT16                   Orginator)
{
    OHA_LLDP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = OHA_OK;
    LSA_USER_ID_TYPE            UserId;
    LSA_UINT8                   Mode = 0;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPStop(pHDB: 0x%X, PortId: 0x%X, AdminStatus: 0x%X, Orginator: 0x%X)", pHDB, PortId, Orginator, AdminStatus);

    OHA_ASSERT_NULL_PTR(pHDB);
    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS && PortId != LLDP_PORT_ID_ALL))
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

	switch(AdminStatus)
	{
		case OHA_LLDP_SEND_ONLY:
			Mode = LLDP_TX_MODE;
		break;
		case OHA_LLDP_RECV_ONLY:
			Mode = LLDP_RX_MODE;
		break;
		case OHA_LLDP_SEND_RECV:
			Mode = LLDP_RXTX_MODE;
		break;
		case OHA_LLDP_DISABLED:
		default:
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPStop failed (AdminStatus: 0x%X).", AdminStatus);
			OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
		break;
	}

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_STOP);
        OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle);
        OHA_LLDP_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        pRQB->args.Stop.PORTId    = PortId;
        pRQB->args.Stop.Mode      = Mode;

        /* special: queue "0" by stop of ALL_PORTS ! */
        if (PortId == LLDP_PORT_ID_ALL)
        {
            PortId = 0;

			/* AP01010944: if a (stop) request to any port (1...n) is active, */
			/* we could get a sequence error from LLDP-Stop(AllPorts), so we  */
			/* queue the request in the Queue '0' (for all ports).            */
			if (OHA_LldpIsSinglePortRequestActive(pHDB))   /* no send */
			{
				OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE_HIGH,">>>: LLDP-Queue: LLDP_OPC_STOP-AllPorts (Handle: 0x%X,LLDPHandle: 0x%X, pRQB 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);
				OHA_RQB_PUT_BLOCK_TOP(pHDB->LldpRQBs[0].pBottom,
									  pHDB->LldpRQBs[0].pTop,
									  pRQB);

				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
									  "OUT: OHA_LLDPStop(Status: 0x%X)",Status);
				return(Status);
			}
		}

        if (!pHDB->StartStopActive[PortId]) /* request is inactive ? */
        {
            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_STOP (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);
            pHDB->StartStopActive[PortId] = LSA_TRUE;
            OHA_LLDP_REQUEST_LOWER(pRQB,pSys);
        }
        else    /* no send */
        {
            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Queue: LLDP_OPC_STOP (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);
            OHA_RQB_PUT_BLOCK_TOP(pHDB->LldpRQBs[PortId].pBottom,
                                  pHDB->LldpRQBs[PortId].pTop,
                                  pRQB);
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_FATAL,"LLDP-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
        OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPStop(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPStopDone                            +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPStopDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              Orginator;
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pBottomRQB;
    LSA_UINT16              PortId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPStopDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    Orginator  = OHA_LLDP_RQB_GET_USERID_UVAR16(pRQB);
    PortId     = pRQB->args.Stop.PORTId;

    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_STOP done (Handle: 0x%X,Rsp: 0x%X, Orginator: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB),Orginator);

    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS && PortId != LLDP_PORT_ID_ALL))
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    switch (OHA_LLDP_RQB_GET_RESPONSE(pRQB))
    {
        case LLDP_RSP_OK:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-ERROR: LLDP_OPC_STOP failed.(0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    /* special: queue "0" by stop of ALL_PORTS ! */
    if (PortId == LLDP_PORT_ID_ALL)
    {
        PortId = 0;
    }

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    /* call next request if queued */
    OHA_RQB_REM_BLOCK_BOTTOM(pHDB->LldpRQBs[PortId].pBottom,
                             pHDB->LldpRQBs[PortId].pTop,
                             pBottomRQB);

    if (OHA_IS_NOT_NULL(pBottomRQB))
    {
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_STOP (Handle: 0x%X,LLDPHandle: 0x%X,pBottomRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pBottomRQB);
        OHA_LLDP_REQUEST_LOWER(pBottomRQB, pHDB->Params.pSys);
    }
    else
    {
        pHDB->StartStopActive[PortId] = LSA_FALSE;

		/* AP01010944: call next request (all ports) if no other single port requests queued */
		if (!OHA_LldpIsSinglePortRequestActive(pHDB))
		{
			OHA_RQB_REM_BLOCK_BOTTOM(pHDB->LldpRQBs[0].pBottom,
									 pHDB->LldpRQBs[0].pTop,
									 pBottomRQB);

			if (OHA_IS_NOT_NULL(pBottomRQB)) /* request (all ports) is queued ? */
			{
				OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE_HIGH,">>>: LLDP-Request: LLDP_OPC_STOP-AllPorts (Handle: 0x%X,LLDPHandle: 0x%X,pBottomRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pBottomRQB);
				OHA_LLDP_REQUEST_LOWER(pBottomRQB, pHDB->Params.pSys);
			}
		}
    }

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_SYS:   /* it's an internal STOP by Close(Lldp)Channel */
            OHA_LLDPChannelDone(pHDB,OHA_OK); /* further actions taken in channel handler */
            break;
        case OHA_HANDLE_TYPE_USER:    /* STOP from User */
            if (PortId == 0) /* all ports are stopped by OhaOpcLldpControl */
            {
                OHA_LLDPRequestHandler(pHDB, LSA_NULL, Response); /* further actions taken in request handler */
            }
            else if(OHA_IS_NOT_NULL(pHDB->UserRQBs.pBottom)) /* a single port is stopped by OhaOpcPortControl */
            {
                OHA_LLDPUserCheckPending(pHDB, Response);
            }
			else  /* from OpcWriteOid */
			{
				LSA_HANDLE_TYPE         Handle  = (LSA_HANDLE_TYPE)Orginator;
				OHA_HDB_USER_PTR_TYPE   pHDBUser = OHA_GetUserHDBFromHandle(Handle);

				if (OHA_IS_NOT_NULL(pHDBUser))  /* not closed */
				{
					if (pHDBUser->StatsPendCnt == 0)    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

					pHDBUser->StatsPendCnt--;   /* decrement the active requests */

					if (pHDBUser->StatsPendCnt == 0)    /* got all necessary statis ? */
					{
						OHA_SNMPUserCheckPending(pHDBUser, Response); /* further actions taken in request handler */
					}
				}
			}
            break;
        case OHA_HANDLE_TYPE_LLDP:  /* it's an internal STOP by OhaPrmCommit (PeerToPeerBoundary=OHA_PTP_SEND_LLDP_OFF) */
            OHA_LLDPRequestHandler(pHDB, LSA_NULL, Response); /* further actions taken in request handler */
            break;
        case OHA_HANDLE_TYPE_DCP:   /* it's an internal STOP by update send and wrong data */
        case OHA_HANDLE_TYPE_EDD:   /* it's an internal STOP by link down */
            break;                      /* no further action */
        case OHA_HANDLE_TYPE_SOCK:  /* SNMP-Set from cable */
        {
			LSA_HANDLE_TYPE			Handle	= (LSA_HANDLE_TYPE)Orginator;
			OHA_HDB_SOCK_PTR_TYPE	pHDBSock = OHAGetSockHDBFromHandle(Handle); /* not OHA_SOCKGetHandleByNIC()! */

            if (OHA_IS_NOT_NULL(pHDBSock))  /* not closed */
            {
				OHA_HDB_AGNT_PTR_TYPE pSubAgnt = &pHDBSock->LldpAgent;

				OHA_ASSERT(pSubAgnt->StatsPendCnt);
                pSubAgnt->StatsPendCnt--;   /* decrement the active requests */

                if (pSubAgnt->StatsPendCnt == 0)    /* got all necessary statistics or set the globals ? */
                {
					OHA_ASSERT(pSubAgnt->RequestState == OHA_SNMP_STATE_WF_STATISTICS);

					OHA_SNMPSockCheckPending(pHDBSock, OHA_LLDP_MIB_NODE, Response); /* further actions taken in request handler */
					pSubAgnt->RequestState = OHA_SNMP_STATE_START;
                }
            }
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPStopDone(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPUpdateSend                          +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+                             LSA_UINT32       ControlStatus              +*/
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
/*+               send data of LLDP if LLDP-Send is active                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateSend(
    LSA_UINT16 NICId,
    LSA_UINT32 ControlStatus,
    LSA_UINT16 Orginator)
{
    LSA_UINT16  PortCnt;
    LSA_UINT16  Response = LSA_RET_OK;
    /* Check if we have a LLDP-Channel for NIC-ID.*/
    OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPUpdateSend(NICId: 0x%X, ControlStatus: 0x%X, Orginator: 0x%X)", NICId,ControlStatus,Orginator);

    /* update the LLDP-send data if LLDP-Send is active */
    if ((ControlStatus & OHA_CTRL_STATION_NAME) ||
        (ControlStatus & OHA_CTRL_IP_SUITE) ||
        (ControlStatus & OHA_CTRL_RESET_TO_FACTORY) ||
		(ControlStatus & OHA_CTRL_RESET_COMMUNICATION) )
    {
        if (pHDBLldp && (pHDBLldp->RequestState == OHA_LLDP_STATE_START ||  /* LLDP-Send is active ? */
						 pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START ||
						 pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START ))  /* AP01070629, AP01137963 */
        {
            for (PortCnt = 1; PortCnt <= pHDBLldp->pDB->PortCnt; PortCnt++)
            {
                Response = OHA_LLDPSet(pHDBLldp, PortCnt, Orginator); /* send them */
                /* could not set (all data), stop the running send ???  */
                /* if (Response != LSA_RET_OK)                          */
                /*  OHA_LLDPStop(pHDBLldp, PortCnt, Orginator); */ /* this port */
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdateSend(Response: 0x%X)", Response);
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPUpdateSendGlobal                    +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+                             LSA_UINT16       Orginator                  +*/
/*+  Result                :    LSA_UINT16       OHA_OK, OHA_ERR_LL         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Update the send data of LLDP on all interfaces            +*/
/*+               if LLDP-Send is active                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateSendGlobal(LSA_VOID)
{
	LSA_UINT16  i;

    OHA_HDB_LLDP_PTR_TYPE pHDBLldp;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPUpdateSendGlobal()");

    /* Check if we have a LLDP-Channel for NIC-ID.*/
    for (i=1; i <= OHA_CFG_MAX_NICS; i++) /* for all Nic's */
    {
		pHDBLldp = OHA_LLDPGetHandleByIndex(i);

		if(OHA_IS_NOT_NULL(pHDBLldp))
		{
			OHA_DETAIL_PTR_TYPE pDetail = pHDBLldp->Params.pDetail;
			
			if(OHA_IS_NOT_NULL(pDetail))
			{
				LSA_UINT16 NicID  = pDetail->Params.Lldp.NICId;

				if(NicID != OHA_INVALID_NIC_ID)
				{
					LSA_UINT16  RetVal = OHA_LLDPUpdateSend(NicID,
															OHA_CTRL_RESET_TO_FACTORY,
															OHA_HANDLE_TYPE_SYS);
                	if (RetVal != OHA_OK)
                	{
                    	OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE, "OHA_LLDPUpdateSendGlobal - LLDPUpdateSend(RetVal: 0x%X, IfNicID: 0x%X)", RetVal, pDetail->Params.Lldp.NICId);
                	}
            	}
			}
		}
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdateSendGlobal()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpGetParams                                     +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER ..    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to TLV element to analyse+*/
/*+                         out:Pointer behind TLV analysed (next TLV)      +*/
/*+  pTLength             : in: Length of TLV chain before TLV read         +*/
/*+                         out:remaining length of TLV-chain.              +*/
/*+                                                                         +*/
/*+  Result:           OHA_OK                                               +*/
/*+                    OHA_ERR_PARAM                                        +*/
/*+                    OHA_ERR_FAILED                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets (read) Parameters from a TLV-element beginning at    +*/
/*+               *ppTLV and updates pTLVParams with the TLV specific       +*/
/*+               parameters. *ppTLV and pTLength will be modified so they  +*/
/*+               can be used for further calls.                            +*/
/*+                                                                         +*/
/*+               OHA_ERR_FAILED is set if the TLV-chain is                 +*/
/*+               corrupted or a TLV- total length is less then             +*/
/*+               the sum of contained TLV-Elements. No further             +*/
/*+               processing possible. User should discard the whole TLV    +*/
/*+                                                                         +*/
/*+               If pTLength returns 0, no more TLVs present.              +*/
/*+                                                                         +*/
/*+        Note:  Optional TLVs are not validated on reception!             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpGetParams(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,
    LSA_UINT8                   OhaSenderStatus,
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    LSA_BOOL *                  pRemoteChanges
)
{
    LLDP_TLV_GET_EX_PARAMS_TYPE TLVParams;
    LSA_UINT16 Response;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpGetParams(ppTLV: 0x%X, pTLength: 0x%X)", ppTLV, pTLength);

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        LSA_UINT16 Resp = LLDP_RSP_OK;

		LSA_BOOL   RemotePortDescr     = LSA_FALSE; /* AP01101853 */
		LSA_BOOL   RemoteSystemName    = LSA_FALSE;
		LSA_BOOL   RemoteSystemDescr   = LSA_FALSE;
		LSA_BOOL   RemoteSysCap        = LSA_FALSE; /* AP01319678 */
		LSA_BOOL   RemoteManAdr        = LSA_FALSE;

        LSA_BOOL   RemotePnoDelay      = LSA_FALSE;
        LSA_BOOL   RemotePnoPortStatus = LSA_FALSE;
        LSA_BOOL   RemotePnoPortStatusMrp = LSA_FALSE;
        LSA_BOOL   RemoteChassisMacAddr = LSA_FALSE;
        LSA_BOOL   RemotePnoPtcpStatus  = LSA_FALSE;
        LSA_BOOL   RemoteMacPhyCfgStatus = LSA_FALSE;
        LSA_BOOL   RemoteMaxFrameSize   = LSA_FALSE;
        LSA_BOOL   RemotePnoOthers      = LSA_FALSE;

        LSA_BOOL   OrgSpecOthers        = LSA_FALSE; /* 1062467 */
        LSA_BOOL   UnknownOptionals     = LSA_FALSE; /* 1062467 */

        Response = OHA_OK;

        while (Resp == LLDP_RSP_OK ||
               Resp == LLDP_RSP_ERR_TLV_UNKNOWN ||
               Resp == LLDP_RSP_ERR_TLV_PARAM_ERROR)
        {
            TLVParams.Type = 0; /* init */
            TLVParams.Params.PortDescr.PortDescriptionLen = 0;
            TLVParams.Params.SystemName.SystemNameLen = 0;
            TLVParams.Params.SystemDescr.SystemDescrLen = 0;
            TLVParams.Params.VLANName.VLANNameLength = 0;
            TLVParams.Params.ProtIdentity.Length = 0;
            TLVParams.Params.OrgUnknown.OUI[0] = 0;
            TLVParams.Params.OrgUnknown.Subtype = 0;
            TLVParams.Params.OrgUnknown.GroupDefInfoStringLen = 0;
            TLVParams.Params.UnknownOptional.Type = 0;
            TLVParams.Params.UnknownOptional.ValueLen = 0;

            /* TLV-Info enthält LinkAggregationStatus           */
            Resp  = lldp_TLVGetParams(ppTLV,pTLength,&TLVParams);
            /* pTLV zeigt hinter TLVParams, Length enthält Restlänge  */
            if ( Resp == LLDP_RSP_OK)
            {
                if (TLVParams.Type == LLDP_PARAM_TYPE_LLDPU_END || (*pTLength == 0))
                {
                    break;
                }
                else
                {
                    switch (TLVParams.Type)
                    {
                        case LLDP_PARAM_TYPE_LLDPU_END:
                        case LLDP_PARAM_TYPE_CHASSIS_ID:
                        case LLDP_PARAM_TYPE_PORT_ID:
                        case LLDP_PARAM_TYPE_TTL:
                            /* already handled by lldp_TLVGetChassisPortTTL() */
                            break;
                        case LLDP_PARAM_TYPE_PORT_DESCR:
                            if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.PortDescr.PortDescription,LSA_NULL)) &&
                                 TLVParams.Params.PortDescr.PortDescriptionLen && TLVParams.Params.PortDescr.PortDescriptionLen <= LLDP_TLV_PORT_DESCR_MAX_SIZE)
                            {
                                /* write to remote LLDP-DB (PortDescription) */
                                RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                       LLDP_TLV_TYPE_PORT_DESCR,
                                                                       0, /*SubType*/ 0, /* OUI */
                                                                       TLVParams.Params.PortDescr.PortDescription,/* pVariable */
                                                                       TLVParams.Params.PortDescr.PortDescriptionLen);       /* Length */
                                if (RetVal == LSA_RET_OK)
                                {
									RemotePortDescr = LSA_TRUE;
								}
								else
								{
                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PortDescription) failed (RetVal: 0x%X).", RetVal);
                                }
                            }
                            break;
                        case LLDP_PARAM_TYPE_SYSTEM_NAME:
                            if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.SystemName.SystemName,LSA_NULL)) &&
                                 TLVParams.Params.SystemName.SystemNameLen && TLVParams.Params.SystemName.SystemNameLen <= LLDP_TLV_SYSTEM_NAME_MAX_SIZE)
                            {
                                /* write to remote LLDP-DB (SystemName) */
                                RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                       LLDP_TLV_TYPE_SYSTEM_NAME,
                                                                       0, /*SubType*/ 0, /* OUI */
                                                                       TLVParams.Params.SystemName.SystemName,/* pVariable */
                                                                       TLVParams.Params.SystemName.SystemNameLen);   /* Length */
                                if (RetVal == LSA_RET_OK)
                                {
									RemoteSystemName = LSA_TRUE;
								}
								else
                                {
                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(SystemName) failed (RetVal: 0x%X).", RetVal);
                                }
                            }
                            break;
                        case LLDP_PARAM_TYPE_SYSTEM_DESCR:
                            if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.SystemDescr.SystemDescr,LSA_NULL)) &&
                                 TLVParams.Params.SystemDescr.SystemDescrLen && TLVParams.Params.SystemDescr.SystemDescrLen <= LLDP_TLV_PORT_DESCR_MAX_SIZE)
                            {
                                /* write to remote LLDP-DB (SystemDescription) */
                                RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                       LLDP_TLV_TYPE_SYSTEM_DESCR,
                                                                       0, /*SubType*/ 0, /* OUI */
                                                                       TLVParams.Params.SystemDescr.SystemDescr,/* pVariable */
                                                                       TLVParams.Params.SystemDescr.SystemDescrLen);     /* Length */
                                if (RetVal == LSA_RET_OK)
                                {
									RemoteSystemDescr = LSA_TRUE;
								}
								else
                                {
                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(SystemDescription) failed (RetVal: 0x%X).", RetVal);
                                }
                            }
                            break;
                        case LLDP_PARAM_TYPE_SYSTEM_CAP:
                        {
                            LSA_UINT8   SysCap[LLDP_TL_SIZE + LLDP_TLV_SYSTEM_CAP_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = SysCap;
                            LSA_UINT32  Len = 0;

                            /* rebuild the value field for writing to the database */
                            RetVal = lldp_TLVSetSystemCap(&pVar, &Len, &TLVParams.Params.SystemCap);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len == (LLDP_TL_SIZE + LLDP_TLV_SYSTEM_CAP_SIZE))
                                {
                                    /* write to remote LLDP-DB (SystemCapabilities) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_SYSTEM_CAP,
                                                                           0, /*SubType*/ 0, /* OUI */
                                                                           SysCap + LLDP_TL_SIZE,/* pVariable */
                                                                           LLDP_TLV_SYSTEM_CAP_SIZE); /* Length */
                                    if (RetVal == LSA_RET_OK)
									{
										RemoteSysCap = LSA_TRUE;
									}
									else
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(SystemCapabilities) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetSystemCap() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_MGM_ADDR:
                        {
                            LSA_UINT8   MgmAddr[LLDP_TL_SIZE + LLDP_TLV_MGM_ADDR_MAX_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = MgmAddr;
                            LSA_UINT32  Len = 0;

                            /* rebuild the value field for writing to the database */
                            RetVal= lldp_TLVSetMgmAddr(&pVar, &Len, &TLVParams.Params.MgmAddr);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len > LLDP_TL_SIZE)
                                {
                                    /* write to remote LLDP-DB (MgmAddress) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_MGM_ADDR,
                                                                           0, /*SubType*/ 0, /* OUI */
                                                                           MgmAddr + LLDP_TL_SIZE,/* pVariable */
                                                                           (LSA_UINT16)(Len - LLDP_TL_SIZE)); /* Length */
                                    if (RetVal == LSA_RET_OK)
									{
										RemoteManAdr = LSA_TRUE;
									}
									else
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(MgmAddress) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetMgmAddr() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;

                        case LLDP_PARAM_TYPE_IEEE_802_1_PORT_VLAN_ID:
                        {
                            LSA_UINT8   VlanID[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_1_PORT_VLAN_ID_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = VlanID;
                            LSA_UINT32  Len = 0;

                            /* rebuild the value field for writing to the database */
                            RetVal = lldp_TLVSetPortVLANID(&pVar, &Len, &TLVParams.Params.PortVLANID);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len == LLDP_TL_SIZE + LLDP_TLV_IEEE_802_1_PORT_VLAN_ID_SIZE)
                                {
                                    /* write to remote LLDP-DB (PortVLANID) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                           LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID, /*SubType*/
                                                                           LLDP_TLV_ORG_OUI_IEEE_802_1, /* OUI */
                                                                           VlanID + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                           (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PortVLANID) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetPortVLANID() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_IEEE_802_1_PPVID:
                        {
                            LSA_UINT8   PpvID[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_1_PPVID_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = PpvID;
                            LSA_UINT32  Len = 0;

                            /* rebuild the value field for writing to the database */
                            RetVal = lldp_TLVSetPPVID(&pVar, &Len, &TLVParams.Params.PPVID);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len == LLDP_TL_SIZE + LLDP_TLV_IEEE_802_1_PPVID_SIZE)
                                {
                                    /* write to remote LLDP-DB (PPVID) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                           LLDP_TLV_IEEE_802_1_TYPE_PPVID, /*SubType*/
                                                                           LLDP_TLV_ORG_OUI_IEEE_802_1, /* OUI */
                                                                           PpvID + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                           (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PPVID) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetPPVID() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_IEEE_802_1_VLAN_NAME:
                        {
                            LSA_UINT8   VLanName[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = VLanName;
                            LSA_UINT32  Len = 0;

                            if (TLVParams.Params.VLANName.VLANNameLength >= LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_VLAN_NAME_LEN &&
                                TLVParams.Params.VLANName.VLANNameLength <= LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_VLAN_NAME_LEN)
                            {
                                /* rebuild the value field for writing to the database */
                                RetVal = lldp_TLVSetVLANName(&pVar, &Len, &TLVParams.Params.VLANName);
                                if (RetVal == LLDP_RSP_OK)
                                {   /* LLDP adds the TL-header */
                                    if (Len > LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE)
                                    {
                                        /* write to remote LLDP-DB (VLANName) */
                                        RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                               LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                               LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME, /*SubType*/
                                                                               LLDP_TLV_ORG_OUI_IEEE_802_1, /* OUI */
                                                                               VLanName + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                               (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                        if (RetVal != LSA_RET_OK)
                                        {
                                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(VLANName) failed (RetVal: 0x%X).", RetVal);
                                        }
                                    }
                                }
                                else
                                {
                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetVLANName() failed (RetVal: 0x%X).", RetVal);
                                }
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_IEEE_802_1_PROTOCOL_IDENTITY:
                        {
                            LSA_UINT8   ProtocolID[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = ProtocolID;
                            LSA_UINT32  Len = 0;

                            if (TLVParams.Params.ProtIdentity.Length)
                            {
                                /* rebuild the value field for writing to the database */
                                RetVal = lldp_TLVSetProtIdentity(&pVar, &Len, &TLVParams.Params.ProtIdentity);
                                if (RetVal == LLDP_RSP_OK)
                                {   /* LLDP adds the TL-header */
                                    if (Len > LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE)
                                    {
                                        /* write to remote LLDP-DB (ProtIdentity) */
                                        RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                               LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                               LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY, /*SubType*/
                                                                               LLDP_TLV_ORG_OUI_IEEE_802_1, /* OUI */
                                                                               ProtocolID + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                               (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                        if (RetVal != LSA_RET_OK)
                                        {
                                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(ProtIdentity) failed (RetVal: 0x%X).", RetVal);
                                        }
                                    }
                                }
                                else
                                {
                                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetProtIdentity() failed (RetVal: 0x%X).", RetVal);
                                }
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_IEEE_802_3_MAC_PHY_CFG_STATUS:
                        {
                            LSA_UINT8   MacPhyCfgStatus[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = MacPhyCfgStatus;
                            LSA_UINT32  Len = 0;

                            RemoteMacPhyCfgStatus = LSA_TRUE;

                            /* rebuild the value field for writing to the database */
                            RetVal = lldp_TLVSetMACPHYConfigStatus(&pVar, &Len, &TLVParams.Params.MACPHYConfigStatus);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len == LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_SIZE)
                                {
                                    /* write to remote LLDP-DB (MACPHYConfigStatus) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                           LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS, /*SubType*/
                                                                           LLDP_TLV_ORG_OUI_IEEE_802_3, /* OUI */
                                                                           MacPhyCfgStatus + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                           (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(MACPHYConfigStatus) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetMACPHYConfigStatus() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_IEEE_802_3_POWER_VIA_MDI:
                        {
                            LSA_UINT8   PowerViaMDI[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = PowerViaMDI;
                            LSA_UINT32  Len = 0;

                            /* rebuild the value field for writing to the database */
                            RetVal = lldp_TLVSetPowerViaMDI(&pVar, &Len, &TLVParams.Params.PowerViaMDI);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len == LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_SIZE)
                                {
                                    /* write to remote LLDP-DB (PowerViaMDI) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                           LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI, /*SubType*/
                                                                           LLDP_TLV_ORG_OUI_IEEE_802_3, /* OUI */
                                                                           PowerViaMDI + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                           (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PowerViaMDI) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetPowerViaMDI() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_IEEE_802_3_LINK_AGGREGATION:
                        {
                            LSA_UINT8   LinkAggregation[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = LinkAggregation;
                            LSA_UINT32  Len = 0;

                            /* rebuild the value field for writing to the database */
                            RetVal = lldp_TLVSetLinkAgg(&pVar, &Len, &TLVParams.Params.LinkAgg);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len == LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_SIZE)
                                {
                                    /* write to remote LLDP-DB (LinkAggregation) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                           LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION, /*SubType*/
                                                                           LLDP_TLV_ORG_OUI_IEEE_802_3, /* OUI */
                                                                           LinkAggregation + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                           (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(LinkAggregation) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetLinkAgg() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_IEEE_802_3_MAX_FRAME_SIZE:
                        {
                            LSA_UINT8   MaxFrameSize[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_SIZE];
                            LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = MaxFrameSize;
                            LSA_UINT32  Len = 0;

                            RemoteMaxFrameSize = LSA_TRUE;

                            /* rebuild the value field for writing to the database */
                            RetVal = lldp_TLVSetMaxFrameSize(&pVar, &Len, &TLVParams.Params.MaxFrameSize);
                            if (RetVal == LLDP_RSP_OK)
                            {   /* LLDP adds the TL-header */
                                if (Len == LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_SIZE)
                                {
                                    /* write to remote LLDP-DB (MaxFrameSize) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                                           LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE, /*SubType*/
                                                                           LLDP_TLV_ORG_OUI_IEEE_802_3, /* OUI */
                                                                           MaxFrameSize + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + LLDP_TL_SIZE,/* pVariable */
                                                                           (LSA_UINT16)(Len - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE - LLDP_TL_SIZE)); /* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(MaxFrameSize) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: lldp_TLVSetMaxFrameSize() failed (RetVal: 0x%X).", RetVal);
                            }
                        }
                        break;
                        case LLDP_PARAM_TYPE_ORG_SPECIFIC:
                        {
                            LSA_UINT32 Oui;

                            Oui = ( (LSA_UINT32) ( ((LSA_UINT32)TLVParams.Params.OrgUnknown.OUI[0] << 16) +
                                                   ((LSA_UINT32)TLVParams.Params.OrgUnknown.OUI[1] << 8 ) +
                                                   ((LSA_UINT32)TLVParams.Params.OrgUnknown.OUI[2] ) ));
                            if ((Oui == OHA_LLDP_TYPE_ORG_OUI_PNO) ||   /* AP00376089 */
                                (Oui == OHA_LLDP_TYPE_ORG_OUI_PNO_1) || /* receive "01-0e-cf"as valid PNIO_OUI "00-0e-cf" */
                                (Oui == OHA_LLDP_TYPE_ORG_OUI_PNO_2) || /* receive "02-0e-cf"as valid PNIO_OUI "00-0e-cf" */
                                (Oui == OHA_LLDP_TYPE_ORG_OUI_PNO_3))   /* receive "03-0e-cf"as valid PNIO_OUI "00-0e-cf" */
                            {
                                OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
                                LSA_UINT16 Len     = 0;
                                /* port delay */
                                if (TLVParams.Params.OrgUnknown.Subtype == OHA_LLDP_SUB_TYPE_PNO_DELAY)
                                {
                                    RemotePnoDelay = LSA_TRUE;

                                    if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.OrgUnknown.GroupDefInfoString,LSA_NULL)) &&
                                         TLVParams.Params.OrgUnknown.GroupDefInfoStringLen == OHA_LLDP_PNO_DELAY_SIZE)
                                    {
                                        LSA_UINT8 GroupDefInfoString[OHA_LLDP_PNO_DELAY_SIZE];

                                        /* store the delays */
                                        OHA_MEMCPY(GroupDefInfoString, TLVParams.Params.OrgUnknown.GroupDefInfoString, OHA_LLDP_PNO_DELAY_SIZE);

                                        /* AP00353236: In case of MultipleNeighbours there will be set: RemoteRx=0, RemoteTx=0 and CableDelay=0 */
                                        /* This state remains as long as MultipleNeighbours are existing.    */
                                        /* No actualization of LLDP-send will be done.   */
                                        if (OhaSenderStatus == OHA_STATUS_MULTIPLE_SENDER)
                                        {
                                            OHA_MEMSET(GroupDefInfoString, 0, sizeof(LSA_UINT32));
                                            OHA_MEMSET(&GroupDefInfoString[OHA_LLDP_OFFSET_TXDELAY_LOC], 0, sizeof(LSA_UINT32));
                                            OHA_MEMSET(&GroupDefInfoString[OHA_LLDP_OFFSET_CABLEDELAY_LOC], 0, sizeof(LSA_UINT32));
                                        }

                                        /* are there delays existing ? */
                                        (LSA_VOID)oha_db_read_rem_lldp_element(NicId, PortId, SenderId,
                                                                               OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                               &TLVParams.Params.OrgUnknown.Subtype,
                                                                               OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                               &NamePtr, &Len);

                                        /* are the delays different from the last one ? */
                                        if ((!LSA_HOST_PTR_ARE_EQUAL(NamePtr, LSA_NULL)) && (Len == OHA_LLDP_PNO_DELAY_SIZE))
                                        {   /* compare RxDelayLoc, TxDelayLoc, CableDelay ? */
                                            if (OHA_MEMCMP(NamePtr, &GroupDefInfoString, sizeof(LSA_UINT32)) &&
                                                OHA_MEMCMP(NamePtr+OHA_LLDP_OFFSET_TXDELAY_LOC, &GroupDefInfoString[OHA_LLDP_OFFSET_TXDELAY_LOC], sizeof(LSA_UINT32)))
                   	                            /* AP00871051: only comparism of RxDelay and TxDelay for a LLDP send */
                                                /* OHA_MEMCMP(NamePtr+OHA_LLDP_OFFSET_CABLEDELAY_LOC, &GroupDefInfoString[OHA_LLDP_OFFSET_CABLEDELAY_LOC], sizeof(LSA_UINT32))) */
                                            {
                                                *pRemoteChanges = LSA_FALSE;
                                            }
                                            else
                                            {
                                                *pRemoteChanges = LSA_TRUE;
                                            }
                                        }
                                        else
                                        {
                                            *pRemoteChanges = LSA_TRUE;
                                        }

                                        /* write to remote LLDP-DB (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
                                        RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                               OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                               OHA_LLDP_SUB_TYPE_PNO_DELAY,  /*SubType*/
                                                                               OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                               GroupDefInfoString,/* pVariable */
                                                                               OHA_LLDP_PNO_DELAY_SIZE);     /* Length */
                                        if (RetVal != LSA_RET_OK)
                                        {
                                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PortDelays) failed (RetVal: 0x%X).", RetVal);
                                        }
                                    }
                                }           /* port status */
                                else if (TLVParams.Params.OrgUnknown.Subtype == OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS)
                                {
                                    RemotePnoPortStatus = LSA_TRUE;

                                    if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.OrgUnknown.GroupDefInfoString,LSA_NULL)) &&
                                         TLVParams.Params.OrgUnknown.GroupDefInfoStringLen == OHA_LLDP_PNO_PORT_STATUS_SIZE)
                                    {
                                        /* write to remote LLDP-DB (PortStatus) */
                                        RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                               OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                               OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS,  /*SubType*/
                                                                               OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                               TLVParams.Params.OrgUnknown.GroupDefInfoString,/* pVariable */
                                                                               OHA_LLDP_PNO_PORT_STATUS_SIZE);       /* Length */
                                        if (RetVal != LSA_RET_OK)
                                        {
                                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PortStatus) failed (RetVal: 0x%X).", RetVal);
                                        }
                                    }
                                }           /* MRT port status */
                                else if (TLVParams.Params.OrgUnknown.Subtype == OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP)
                                {
                                    RemotePnoPortStatusMrp = LSA_TRUE;

                                    if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.OrgUnknown.GroupDefInfoString,LSA_NULL)) &&
                                         TLVParams.Params.OrgUnknown.GroupDefInfoStringLen == OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE)
                                    {
                                        /* write to remote LLDP-DB (PortStatus) */
                                        RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                               OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                               OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP,
                                                                               OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                               TLVParams.Params.OrgUnknown.GroupDefInfoString,/* pVariable */
                                                                               OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE);       /* Length */
                                        if (RetVal != LSA_RET_OK)
                                        {
                                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PortStatus) failed (RetVal: 0x%X).", RetVal);
                                        }
                                    }
                                }           /* Chassis MACAddr */
                                else if (TLVParams.Params.OrgUnknown.Subtype == OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR)
                                {
                                    RemoteChassisMacAddr = LSA_TRUE;

                                    if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.OrgUnknown.GroupDefInfoString,LSA_NULL)) &&
                                         TLVParams.Params.OrgUnknown.GroupDefInfoStringLen == OHA_LLDP_PNO_CHASSIS_MAC_ADR_SIZE)
                                    {
                                        /* write to remote LLDP-DB (PortStatus) */
                                        RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                               OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                               OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR,  /*SubType*/
                                                                               OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                               TLVParams.Params.OrgUnknown.GroupDefInfoString,/* pVariable */
                                                                               OHA_LLDP_PNO_CHASSIS_MAC_ADR_SIZE);       /* Length */
                                        if (RetVal != LSA_RET_OK)
                                        {
                                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(ChassisMacAddr) failed (RetVal: 0x%X).", RetVal);
                                        }
                                    }
                                }           /* PTCP status */
                                else if (TLVParams.Params.OrgUnknown.Subtype == OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS)
                                {
                                    RemotePnoPtcpStatus = LSA_TRUE;

                                    if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.OrgUnknown.GroupDefInfoString,LSA_NULL)) &&
                                         TLVParams.Params.OrgUnknown.GroupDefInfoStringLen == OHA_LLDP_PNO_PTCP_STATUS_SIZE)
                                    {
                                        /* write to remote LLDP-DB (PtcpStatus) */
                                        RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                               OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                               OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS,
                                                                               OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                               TLVParams.Params.OrgUnknown.GroupDefInfoString,/* pVariable */
                                                                               OHA_LLDP_PNO_PTCP_STATUS_SIZE);       /* Length */
                                        if (RetVal != LSA_RET_OK)
                                        {
                                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PtcpStatus) failed (RetVal: 0x%X).", RetVal);
                                        }
                                    }
                                }
                                else    /* other PNO subtype */
                                {
                                    if (!RemotePnoOthers)
                                    {
                                        (LSA_VOID)oha_db_delete_rem_lldp_pno_others(NicId, PortId, SenderId);
                                    }
                                    RemotePnoOthers = LSA_TRUE;

                                    /* write to remote LLDP-DB (PtcpStatus) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                           TLVParams.Params.OrgUnknown.Subtype,
                                                                           OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                           TLVParams.Params.OrgUnknown.GroupDefInfoString,/* pVariable */
                                                                           TLVParams.Params.OrgUnknown.GroupDefInfoStringLen); /* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(PtcpStatus) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                            else    /* organizationally defined TLV's (OUI) */
                            {
                                if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.OrgUnknown.GroupDefInfoString,LSA_NULL)) &&
                                     TLVParams.Params.OrgUnknown.GroupDefInfoStringLen &&
                                     TLVParams.Params.OrgUnknown.Subtype &&
                                     (TLVParams.Params.OrgUnknown.GroupDefInfoStringLen <= LLDP_TLV_ORG_SPECIFIC_MAX_GROUP_DEFINED_ISTRING_SIZE))
                                {
                                    if (!OrgSpecOthers)
                                    {
                                        (LSA_VOID)oha_db_delete_rem_lldp_orgspec_others(NicId, PortId, SenderId);
                                    }
                                    OrgSpecOthers = LSA_TRUE;

                                    /* write to remote LLDP-DB (OrgUnknown) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                           TLVParams.Params.OrgUnknown.Subtype,  /*SubType*/
                                                                           Oui,
                                                                           TLVParams.Params.OrgUnknown.GroupDefInfoString,/* pVariable */
                                                                           TLVParams.Params.OrgUnknown.GroupDefInfoStringLen);/* Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(OrgUnknown) failed (RetVal: 0x%X).", RetVal);
                                    }
                                }
                            }
                        }
                        break;

                        case LLDP_PARAM_TYPE_UNKNOWN_OPTIONAL:
                        {
                            if (TLVParams.Params.UnknownOptional.Type > LLDP_TLV_TYPE_MGM_ADDR &&
                                TLVParams.Params.UnknownOptional.Type < LLDP_TLV_TYPE_ORG_SPECIFIC) /* Type 9 ...126, unknown optional TLV's */
                            {
                                if ( (!LSA_HOST_PTR_ARE_EQUAL(TLVParams.Params.UnknownOptional.Value,LSA_NULL)) &&
                                     TLVParams.Params.UnknownOptional.ValueLen > LLDP_TLV_ORG_SPECIFIC_MIN_SIZE &&
                                     TLVParams.Params.UnknownOptional.ValueLen <= LLDP_LEN_MAX_SIZE)
                                {
                                    OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicId);
                                    OHA_ASSERT_NULL_PTR(pHDBLldp);

                                    if (!UnknownOptionals)
                                    {
                                        (LSA_VOID)oha_db_delete_rem_lldp_unknown_optionals(NicId, PortId, SenderId);
                                    }
                                    UnknownOptionals = LSA_TRUE;

                                    /* write to remote LLDP-DB (UnknownOptional) */
                                    RetVal = oha_db_write_rem_lldp_element(NicId, PortId, SenderId,
                                                                           TLVParams.Params.UnknownOptional.Type,
                                                                           0, /*SubType*/ 0, /* OUI */
                                                                           &TLVParams.Params.UnknownOptional.Value[LLDP_TLV_ORG_SPECIFIC_MIN_SIZE], /* pVariable */
                                                                           (LSA_UINT16)(TLVParams.Params.UnknownOptional.ValueLen - LLDP_TLV_ORG_SPECIFIC_MIN_SIZE)); /* netto Length */
                                    if (RetVal != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: oha_db_write_rem_lldp_element(UnknownOptional) failed (RetVal: 0x%X).", RetVal);
                                    }

                                    /* count statistics */
                                    if (OHA_IS_NOT_NULL(pHDBLldp->pDB))
                                    {
                                        if (PortId && (PortId <= pHDBLldp->pDB->PortCnt))
                                        {
                                            pHDBLldp->pDB->LldpData[PortId-1].PortStats.RxPortTLVsUnrecognizedTotal++;
                                        }
                                    }
                                }
                            }
                            else    /* continue */
                            {
                                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: Invalid Type occurred, (Type = %d) !",
                                                     TLVParams.Params.UnknownOptional.Type);
                            }
                        }
                        break;

                        default:    /* continue */
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: Invalid ParamsType occurred, (TLVParams.Type = %d) !", TLVParams.Type);
                            break;
                    }
                }
            }
            else if (Resp == LLDP_RSP_ERR_TLV_UNKNOWN ||
                     Resp == LLDP_RSP_ERR_TLV_PARAM_ERROR)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OhaLldpGetParams: TlvUnknown or TlyParamError occurred, (Resp = %d) !", Response);
            }
            else    /* invalid frame */
            {
                Response = OHA_ERR_FAILED;
                break;
            }
        }

		if (!RemotePortDescr)     /* AP01101853 */
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, LLDP_TLV_TYPE_PORT_DESCR, 0, 0);
        }
		if (!RemoteSystemName)    /* AP01101853 */
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, LLDP_TLV_TYPE_SYSTEM_NAME, 0, 0);
        }
		if (!RemoteSystemDescr)   /* AP01101853 */
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_SYSTEM_DESCR, 0, 0);
        }
		if (!RemoteSysCap)       /* AP01319678 */
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_SYSTEM_CAP, 0, 0);
        }
		if (!RemoteManAdr)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, LLDP_TLV_TYPE_MGM_ADDR, 0, 0);
        }

        /* if there were no Org-TLV's (anymore) we have to delete the remote elements in the DB */
        if (!RemotePnoDelay)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                     OHA_LLDP_SUB_TYPE_PNO_DELAY, OHA_LLDP_TYPE_ORG_OUI_PNO);
        }
        if (!RemotePnoPortStatus)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                     OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS, OHA_LLDP_TYPE_ORG_OUI_PNO);
        }
        if (!RemotePnoPortStatusMrp)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                     OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP, OHA_LLDP_TYPE_ORG_OUI_PNO);
        }
        if (!RemoteChassisMacAddr)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                     OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR, OHA_LLDP_TYPE_ORG_OUI_PNO);
        }
        if (!RemotePnoPtcpStatus)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                     OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS, OHA_LLDP_TYPE_ORG_OUI_PNO);
        }
        if (!RemoteMacPhyCfgStatus)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                     LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS, LLDP_TLV_ORG_OUI_IEEE_802_3);
        }
        if (!RemoteMaxFrameSize)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_element(NicId, PortId, SenderId, LLDP_TLV_TYPE_ORG_SPECIFIC,
                                                     LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE, LLDP_TLV_ORG_OUI_IEEE_802_3);
        }
        if (!RemotePnoOthers)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_pno_others(NicId, PortId, SenderId);
        }
        if (!OrgSpecOthers)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_orgspec_others(NicId, PortId, SenderId);
        }
        if (!UnknownOptionals)
        {
            (LSA_VOID)oha_db_delete_rem_lldp_unknown_optionals(NicId, PortId, SenderId);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpGetParams(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetStats                            +*/
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
/*+  Description: Read statistics from one port                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPGetStats(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   Orginator)
{
    OHA_LLDP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetStats(pHDB: 0x%X, PortId: 0x%X, Orginator: 0x%X)", pHDB, PortId, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->pDB);

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    if (PortId == 0 || PortId > pHDB->pDB->PortCnt)
    {
        Status = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_LLDPGetStats(Status: 0x%X)",Status);
        return(Status);
    }

    if (pHDB->RequestState != OHA_LLDP_STATE_START)
    {
        Status = OHA_ERR_SEQUENCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_LLDPGetStats(Status: 0x%X)",Status);
        return(Status);
    }

    OHA_LLDP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(LLDP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_STATS);
        OHA_LLDP_RQB_SET_HANDLE(pRQB,pHDB->LldpHandle);
        OHA_LLDP_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        pRQB->args.Stats.PORTId    = PortId;
        pRQB->args.Stats.Reset     = LSA_FALSE;

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: LLDP-Request: LLDP_OPC_STATS (Handle: 0x%X,LLDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->LldpHandle,pRQB);

        OHA_LLDP_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_FATAL,"LLDP-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
        OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_LLDPGetStats(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPGetStatsDone                        +*/
/*+  Input                      OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPGetStatsDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              PortId;
    LSA_UINT16              Orginator;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPGetStatsDone(pRQB: 0x%X,pHDB: 0x%X)", pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: LLDP_OPC_STATS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_LLDP_RQB_GET_RESPONSE(pRQB));

    Orginator  = OHA_LLDP_RQB_GET_USERID_UVAR16(pRQB);

    switch (OHA_LLDP_RQB_GET_RESPONSE(pRQB))
    {
        case LLDP_RSP_OK:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            OHA_ASSERT_NULL_PTR(pHDB->pDB);
            PortId = pRQB->args.Stats.PORTId;

            if (PortId != 0 && PortId <= pHDB->pDB->PortCnt) /* pHDB->PortCnt */
            {
                pHDB->pDB->LldpData[PortId-1].PortStats.RxPortFramesDiscardedTotal = pRQB->args.Stats.Cntr.RxFramesDiscardedTotal;
                pHDB->pDB->LldpData[PortId-1].PortStats.RxPortFramesErrors = pRQB->args.Stats.Cntr.RxFramesInErrors;
                pHDB->pDB->LldpData[PortId-1].PortStats.RxPortFramesTotal = pRQB->args.Stats.Cntr.RxFramesInTotal;
                pHDB->pDB->LldpData[PortId-1].PortStats.TxPortFramesTotal = pRQB->args.Stats.Cntr.TxFramesOutTotal;
                pHDB->pDB->LldpData[PortId-1].PortStats.RxPortAgeoutsTotal = pRQB->args.Stats.Cntr.RxAgeoutsTotal;
            }
            Response = OHA_OK;
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-ERROR: GET STATISTICS failed.(0x%X).",OHA_LLDP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_LLDP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_SOCK:  /* from cable */
        {
			LSA_HANDLE_TYPE			Handle	= (LSA_HANDLE_TYPE)Orginator;	/* AP00817094 */
			OHA_HDB_SOCK_PTR_TYPE	pHDBSock = OHAGetSockHDBFromHandle(Handle);
			/* OHA_HDB_SOCK_PTR_TYPE pHDBSock = OHA_SOCKGetHandleByNIC(pHDB->Params.pDetail->Params.Lldp.NICId); */

            if (OHA_IS_NOT_NULL(pHDBSock))  /* not closed */
            {
				OHA_HDB_AGNT_PTR_TYPE pSubAgnt = &pHDBSock->LldpAgent;

                if (pSubAgnt->StatsPendCnt == 0)    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

                pSubAgnt->StatsPendCnt--;   /* decrement the active requests */

                if (pSubAgnt->StatsPendCnt == 0)    /* got all necessary statistics ? */
                {
                    /* OHA_SOCKRequestHandler(pHDBSock, pHDB, LSA_NULL, Response); */
					OHA_ASSERT(pSubAgnt->RequestState == OHA_SNMP_STATE_WF_STATISTICS);

					OHA_SNMPSockCheckPending(pHDBSock, OHA_LLDP_MIB_NODE, Response); /* further actions taken in request handler */
					pSubAgnt->RequestState = OHA_SNMP_STATE_START;
                }
            }
        }
        break;

        case OHA_HANDLE_TYPE_USER:  /* from OpcReadOid */
        {
            LSA_HANDLE_TYPE         Handle  = (LSA_HANDLE_TYPE)Orginator;
            OHA_HDB_USER_PTR_TYPE   pHDBUser = OHA_GetUserHDBFromHandle(Handle);

            if (OHA_IS_NOT_NULL(pHDBUser))  /* not closed */
            {
                if (pHDBUser->StatsPendCnt == 0)    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

                pHDBUser->StatsPendCnt--;   /* decrement the active requests */

                if (pHDBUser->StatsPendCnt == 0)    /* got all necessary statistics ? */
                {
                    OHA_SNMPUserCheckPending(pHDBUser, Response); /* further actions taken in request handler */
                }
            }
        }
        break;

        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPGetStatsDone(Response: 0x%X)", Response);
}

/*=============================================================================
 * function name:  OHA_LLDPCompareGlobals
 * function:       compares the parameters of the given objects for changes
 * parameters:     TmpSetOptions (LLDP.MIB option flags)
 *                 WithRema (compare the remanence too or not)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_LLDPCompareGlobals(
	LSA_UINT32                   TmpSetOptions,
    LSA_BOOL                     MakeRema,
    LSA_BOOL                     CheckRema)
{
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;
	LSA_BOOL  IsDifferent = LSA_FALSE;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPCompareGlobals(TmpSetOptions: 0x%X, MakeRema: 0x%X, CheckRema: 0x%X)", TmpSetOptions, MakeRema, CheckRema);
	OHA_ASSERT_NULL_PTR(pDbLldpConfig);

	if(TmpSetOptions & OHA_CTRL_TX_INTERVAL)
	{
		if(pDbLldpConfig->TxInterval.TmpSetVal != pDbLldpConfig->TxInterval.Value)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pDbLldpConfig->TxInterval.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}

	if(TmpSetOptions & OHA_CTRL_REINIT_DELAY)
	{
		if(pDbLldpConfig->LldpReinitDelay.TmpSetVal != pDbLldpConfig->LldpReinitDelay.Value)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pDbLldpConfig->LldpReinitDelay.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}

	if(TmpSetOptions & OHA_CTRL_TX_DELAY)
	{
		if(pDbLldpConfig->LldpTxDelay.TmpSetVal != pDbLldpConfig->LldpTxDelay.Value)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pDbLldpConfig->LldpTxDelay.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}

	if(TmpSetOptions & OHA_CTRL_TX_HOLD_MULTIPLIER)
	{
		if(pDbLldpConfig->TxHoldMultiplier.TmpSetVal != pDbLldpConfig->TxHoldMultiplier.Value)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pDbLldpConfig->TxHoldMultiplier.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}

	if(TmpSetOptions & OHA_CTRL_NOTIFICATION_INTERVAL)
	{
		if(pDbLldpConfig->NotificationInterval.TmpSetVal != pDbLldpConfig->NotificationInterval.Value)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pDbLldpConfig->NotificationInterval.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPCompareGlobals(0x%X)", IsDifferent);
    return(IsDifferent);
}

/*=============================================================================
 * function name:  OHA_LLDPUpdateGlobals
 * function:       reades the parameters of the mib object
 * parameters:     NicId
                   SetOptions (LLDP.MIB option flags)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateGlobals(
    LSA_UINT16                   NicID,
	LSA_UINT32                   TmpSetOptions,
    LSA_UINT16                   Orginator)
{
	LSA_UINT16  RetVal;
	OHA_COMMON_MEM_U32_PTR_TYPE  pValue = LSA_NULL;
	LSA_UINT32	TxInterval = 0, ReinitDelay = 0, TxDelay = 0, TxHoldMultiplier = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPUpdateGlobals(NicID: 0x%X, TmpSetOptions:  0x%X)", NicID, TmpSetOptions);

	if(TmpSetOptions & OHA_CTRL_TX_INTERVAL)
	{
		RetVal = oha_db_read_temp_lldp_mib_value(OHA_CTRL_TX_INTERVAL, &pValue, LSA_NULL);
		OHA_ASSERT_NULL_PTR(pValue);
        TxInterval = *pValue;
	}
	else
	{
		RetVal = OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_INTERVAL, &TxInterval, LSA_NULL, LSA_NULL);
	}
	if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

	if(TmpSetOptions & OHA_CTRL_REINIT_DELAY)
	{
		RetVal = oha_db_read_temp_lldp_mib_value(OHA_CTRL_REINIT_DELAY, &pValue, LSA_NULL);
		OHA_ASSERT_NULL_PTR(pValue);
		ReinitDelay = *pValue;
	}
	else
	{
		RetVal = OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_REINIT_DELAY, &ReinitDelay, LSA_NULL, LSA_NULL);
	}
	if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

	if(TmpSetOptions & OHA_CTRL_TX_DELAY)
	{
		RetVal = oha_db_read_temp_lldp_mib_value(OHA_CTRL_TX_DELAY, &pValue, LSA_NULL);
		OHA_ASSERT_NULL_PTR(pValue);
		TxDelay = *pValue;
	}
	else
	{
		RetVal = OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_DELAY, &TxDelay, LSA_NULL, LSA_NULL);
	}
	if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

	if(TmpSetOptions & OHA_CTRL_TX_HOLD_MULTIPLIER)
	{
		RetVal = oha_db_read_temp_lldp_mib_value(OHA_CTRL_TX_HOLD_MULTIPLIER, &pValue, LSA_NULL);
		OHA_ASSERT_NULL_PTR(pValue);
		TxHoldMultiplier = *pValue;
	}
	else
	{
		RetVal = OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_HOLD_MULTIPLIER, &TxHoldMultiplier, LSA_NULL, LSA_NULL);
	}
	if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

	if(TmpSetOptions & OHA_CTRL_LLDP_SET_GLOBALS) /* execute */
	{
		RetVal = OHA_LLDPSetGlobals(NicID, Orginator /*OHA_HANDLE_TYPE_LLDP*/,  
									TxInterval, ReinitDelay,
									TxDelay, TxHoldMultiplier);
	}
	if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdateGlobals(0x%X)", RetVal);
    return(RetVal);
}

/*=========================================================================================
 * function name:  OHA_LLDPUpdatePorts
 * function:       updates sending and receiving LLDP according to the mib object options
 * parameters:     NicId
                   PortID
                   SetPortOptions (LLDP.MIB port option flags)

 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM

  +------------------++--------------+--------------+--------------+--------------+
  | OldAdminStatus \ ||  txOnly      |  rxOnly      |  txAndRx     |  Disabled    | 
  | NewAdminStatus   ||              |              |              |  (StartUp)   | 
  +==================++==============+==============+==============+==============+
  | txOnly           ||  no action   |  Stop(rx)    |  Stop(rx)    |              | 
  |                  ||              |  Start(tx)   |              |  Start(tx)   | 
  +------------------++--------------+--------------+--------------+--------------+
  | rxOnly           ||  Stop(tx)    |  no action   |  Stop(tx)    |              | 
  |                  ||  Start(rx)   |              |              |  Start(rx)   | 
  +------------------++--------------+--------------+--------------+--------------+
  | txAndRx          ||              |              |  no action   |              | 
  |                  ||  Start(rxtx) |  Start(rxtx) |              |  Start(rxtx) | 
  +------------------++--------------+--------------+--------------+--------------+
  | Disabled         ||              |              |  Stop(rxtx)  |  no action   | 
  | (Shutdown)       ||  Stop(tx)    |  Stop(rx)    |              |              | 
  +------------------++--------------+--------------+--------------+--------------+

 *========================================================================================*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPUpdatePorts(
    LSA_UINT16                   NicID,
    LSA_UINT16                   PortID,
    LSA_UINT16                   Orginator)
{
	LSA_UINT16                   RetVal;
	OHA_COMMON_MEM_U32_PTR_TYPE  pValue = LSA_NULL;
	LSA_UINT32	OldAdminStatus, NewAdminStatus;
    OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicID);
    OHA_HDB_EDD_PTR_TYPE  pHDBEdd  = OHA_EDDGetHandleByNIC(NicID);

	LSA_UINT32	StartAdminStatus   = 0; /* invalid */
	LSA_UINT32	StopAdminStatus    = 0; /* invalid */

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPUpdatePorts(NicID: 0x%X, PortID: 0x%X, Orginator:  0x%X)", NicID, PortID, Orginator);
	OHA_ASSERT_NULL_PTR(pHDBLldp);
	OHA_ASSERT_NULL_PTR(pHDBEdd);

	OldAdminStatus = 0;
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicID, PortID, OHA_CTRL_ADMIN_STATUS, 
											 &OldAdminStatus, LSA_NULL, LSA_NULL);

	RetVal = oha_db_read_temp_lldp_mib_port_value(NicID, PortID, OHA_CTRL_ADMIN_STATUS, &pValue, LSA_NULL);
	OHA_ASSERT(RetVal == LSA_RET_OK);
	OHA_ASSERT_NULL_PTR(pValue);

	NewAdminStatus = *pValue;

	OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_LOW,"OHA_LLDPUpdatePorts (NewAdminStatus: 0x%X, OldAdminStatus: 0x%X).", NewAdminStatus, OldAdminStatus);

	if(NewAdminStatus != OldAdminStatus)	/* otherwise no action */
	{
		switch(NewAdminStatus)
		{
			case OHA_LLDP_SEND_ONLY:
				if(OldAdminStatus == OHA_LLDP_RECV_ONLY || OldAdminStatus == OHA_LLDP_DISABLED)
				{
					StartAdminStatus = NewAdminStatus;
				}
				if(OldAdminStatus == OHA_LLDP_RECV_ONLY || OldAdminStatus == OHA_LLDP_SEND_RECV)
				{
					StopAdminStatus = OHA_LLDP_RECV_ONLY;
				}
			break;
			case OHA_LLDP_RECV_ONLY:
				if(OldAdminStatus == OHA_LLDP_SEND_ONLY || OldAdminStatus == OHA_LLDP_DISABLED)
				{
					StartAdminStatus = NewAdminStatus;
				}
				if(OldAdminStatus == OHA_LLDP_SEND_ONLY || OldAdminStatus == OHA_LLDP_SEND_RECV)
				{
					StopAdminStatus = OHA_LLDP_SEND_ONLY;
				}
			break;
			case OHA_LLDP_SEND_RECV: /* restart sending or receiving of the lldp frame */
				StartAdminStatus = NewAdminStatus;
			break;
			case OHA_LLDP_DISABLED: /* e.g. shutdown, stop the running status */
				StopAdminStatus = OldAdminStatus;
			break;
			default:
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"MIB_DB-ERROR: oha_db_read_temp_lldp_mib_value failed.(AdminStatus: 0x%X).",RetVal);
				RetVal = OHA_ERR_FAILED;
			break;
		}
	}

	OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_LOW,"OHA_LLDPUpdatePorts (RequestState: 0x%X, StartAdminStatus: 0x%X, StopdminStatus: 0x%X).", pHDBLldp->RequestState, StartAdminStatus, StopAdminStatus);

	if (!(pHDBLldp->RequestState == OHA_LLDP_STATE_START ||
		  pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START ||
		  pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START)) /* LLDP-Start is possible ? */
	{
		StartAdminStatus = 0;	/* cannot do a start */
	}

	if(StopAdminStatus)
	{
		if(StartAdminStatus)	/* if a start request follows, the stop request fades out */
		{
			RetVal = OHA_LLDPStop(pHDBLldp, PortID, (LSA_UINT16)StopAdminStatus, pHDBEdd->ThisHandle);
		}
		else
		{
			RetVal = OHA_LLDPStop(pHDBLldp, PortID, (LSA_UINT16)StopAdminStatus, Orginator);
		}
		if (RetVal != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPUpdatePorts: OHA_LLDPStop failed (Status: 0x%X).", RetVal);
		}
		else
		{
			RetVal = OHA_OK_ACTIVE;
		}
	}

	if(StartAdminStatus)
	{
		RetVal = OHA_LLDPStart(pHDBLldp, PortID, (LSA_UINT16)StartAdminStatus, Orginator);
		if (RetVal != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPUpdatePorts: OHA_LLDPSart failed (RetVal: 0x%X).", RetVal);
		}
		else
		{
			RetVal = OHA_OK_ACTIVE;
		}
	}

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdatePorts(0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_LLDPSetMibOptions
 * function:       updates the global mib objects
 * parameters:     IfOptions (LLDP.MIB option flags)
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPSetMibOptions(
	LSA_UINT32                   IfSetOptions,
	LSA_BOOL					 IsRema)
{
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPSetMibOptions(IfOptions: 0x%X, IsRema: 0x%X)", IfSetOptions, IsRema);

	if(IfSetOptions & OHA_CTRL_TX_INTERVAL)
	{
		pDbLldpConfig->TxInterval.Value = pDbLldpConfig->TxInterval.TmpSetVal;
		pDbLldpConfig->TxInterval.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if(IfSetOptions & OHA_CTRL_REINIT_DELAY)
	{
		pDbLldpConfig->LldpReinitDelay.Value = pDbLldpConfig->LldpReinitDelay.TmpSetVal;
		pDbLldpConfig->LldpReinitDelay.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if(IfSetOptions & OHA_CTRL_TX_DELAY)
	{
		pDbLldpConfig->LldpTxDelay.Value = pDbLldpConfig->LldpTxDelay.TmpSetVal;
		pDbLldpConfig->LldpTxDelay.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if(IfSetOptions & OHA_CTRL_TX_HOLD_MULTIPLIER)
	{
		pDbLldpConfig->TxHoldMultiplier.Value = pDbLldpConfig->TxHoldMultiplier.TmpSetVal;
		pDbLldpConfig->TxHoldMultiplier.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if(IfSetOptions & OHA_CTRL_NOTIFICATION_INTERVAL)
	{
		pDbLldpConfig->NotificationInterval.Value = pDbLldpConfig->NotificationInterval.TmpSetVal;
		pDbLldpConfig->NotificationInterval.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPSetMibOptions()");
    return;
}

/*=============================================================================
 * function name:  OHA_LLDPSetMibPortOptions
 * function:       updates the port mib objects
 * parameters:     PortOptions (LLDP.MIB port option flags)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPSetMibPortOptions(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
	LSA_UINT32                  PortSetOptions,
	LSA_BOOL					IsRema)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = OHA_OK;
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPSetMibPortOptions(NicId: 0x%X, PortId: 0x%X, PortSetOptions: 0x%X, IsRema: 0x%X)",
                          NicId, PortId, PortSetOptions, IsRema);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
		RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_LLDPSetMibPortOptions(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }
    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
    {
		RetVal = OHA_ERR_PARAM;
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_LLDPSetMibPortOptions(Invalid PortId:  0x%X)", PortId);
        return (RetVal);
    }
	
	pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;

	if (PortSetOptions & OHA_CTRL_ADMIN_STATUS) /* lldpPortConfigTable "lldpPortConfigAdminStatus" */
	{
		pPortConfig->AdminStatus.Value = pPortConfig->AdminStatus.TmpSetVal;
		pPortConfig->AdminStatus.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_NOTIFICATION) /* lldpPortConfigTable "lldpPortConfigNotificationEnable" */
	{
		pPortConfig->NotificationEnable.Value = pPortConfig->NotificationEnable.TmpSetVal;
		pPortConfig->NotificationEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_LLDP_TLVS) /* lldpPortConfigTLVsTxEnable  */
	{
		pPortConfig->TLVsTxEnable.Value = pPortConfig->TLVsTxEnable.TmpSetVal;
		pPortConfig->TLVsTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_LLDP_MAN_ADDR) /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
	{
		pPortConfig->ManAddrTxEnable.Value = pPortConfig->ManAddrTxEnable.TmpSetVal;
		pPortConfig->ManAddrTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_PNO_DELAY)    /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
	{
		pPortConfig->PnoDelayTxEnable.Value = pPortConfig->PnoDelayTxEnable.TmpSetVal;
		pPortConfig->PnoDelayTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_PNO_PORT_STATUS) /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"  */
	{
		pPortConfig->PnoPortStatusTxEnable.Value = pPortConfig->PnoPortStatusTxEnable.TmpSetVal;
		pPortConfig->PnoPortStatusTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_PNO_ALIAS)    /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
	{
		pPortConfig->PnoAliasTxEnable.Value = pPortConfig->PnoAliasTxEnable.TmpSetVal;
		pPortConfig->PnoAliasTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_PNO_MRP)      /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
	{
		pPortConfig->PnoMrpTxEnable.Value = pPortConfig->PnoMrpTxEnable.TmpSetVal;
		pPortConfig->PnoMrpTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_PNO_PTCP)     /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
	{
		pPortConfig->PnoPtcpTxEnable.Value = pPortConfig->PnoPtcpTxEnable.TmpSetVal;
		pPortConfig->PnoPtcpTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_DOT3_TLVS) /* lldpXdot3PortConfigTLVsTxEnable */
	{
		pPortConfig->Dot3TLVsTxEnable.Value = pPortConfig->Dot3TLVsTxEnable.TmpSetVal;
		pPortConfig->Dot3TLVsTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_DOT1_VLAN_ID) /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
	{
		pPortConfig->Dot1VlanIdTxEnable.Value = pPortConfig->Dot1VlanIdTxEnable.TmpSetVal;
		pPortConfig->Dot1VlanIdTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_DOT1_VLAN_NAME)	 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
	{
		pPortConfig->Dot1VlanNameTxEnable.Value = pPortConfig->Dot1VlanNameTxEnable.TmpSetVal;
		pPortConfig->Dot1VlanNameTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_DOT1_PROTO_VLAN) /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
	{
		pPortConfig->Dot1PortProtoVlanTxEnable.Value = pPortConfig->Dot1PortProtoVlanTxEnable.TmpSetVal;
		pPortConfig->Dot1PortProtoVlanTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}
	if (PortSetOptions & OHA_CTRL_DOT1_PROTOCOL) /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */
	{
		pPortConfig->Dot1ProtocolTxEnable.Value = pPortConfig->Dot1ProtocolTxEnable.TmpSetVal;
		pPortConfig->Dot1ProtocolTxEnable.Remanent = (IsRema) ? LSA_TRUE : LSA_FALSE;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPSetMibPortOptions(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_LLDPComparePorts
 * function:       compares the parameters of the given objects for changes
 * parameters:     TmpSetOptions (LLDP.MIB option flags)
 *                 WithRema (compare the remanence too or not)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_LLDPComparePorts(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
	LSA_UINT32                  PortSetOptions,
    LSA_BOOL                    MakeRema,
    LSA_BOOL                    CheckRema)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;
	LSA_BOOL  IsDifferent = LSA_FALSE;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPComparePorts(NicId: 0x%X, PortId: 0x%X, PortSetOptions: 0x%X, MakeRema: 0x%X, CheckRema: 0x%X)", NicId, PortId, PortSetOptions, MakeRema, CheckRema);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "OUT: OHA_LLDPComparePorts(No such interface, NicID: 0x%X)", NicId);
        return (IsDifferent);
    }

	OHA_ASSERT(!(PortId == 0 || (PortId > OHA_CFG_MAX_PORTS)));
	pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;
	OHA_ASSERT_NULL_PTR(pPortConfig);

	if (PortSetOptions & OHA_CTRL_ADMIN_STATUS) /* lldpPortConfigTable "lldpPortConfigAdminStatus" */
	{
		if(pPortConfig->AdminStatus.Value != pPortConfig->AdminStatus.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->AdminStatus.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}

	if (PortSetOptions & OHA_CTRL_NOTIFICATION) /* lldpPortConfigTable "lldpPortConfigNotificationEnable" */
	{
		if(pPortConfig->NotificationEnable.Value != pPortConfig->NotificationEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->NotificationEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_LLDP_TLVS) /* lldpPortConfigTLVsTxEnable  */
	{
		if(pPortConfig->TLVsTxEnable.Value != pPortConfig->TLVsTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->TLVsTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_LLDP_MAN_ADDR) /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
	{
		if(pPortConfig->ManAddrTxEnable.Value != pPortConfig->ManAddrTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->ManAddrTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_PNO_DELAY)    /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
	{
		if(pPortConfig->PnoDelayTxEnable.Value != pPortConfig->PnoDelayTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->PnoDelayTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_PNO_PORT_STATUS) /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"  */
	{
		if(pPortConfig->PnoPortStatusTxEnable.Value != pPortConfig->PnoPortStatusTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->PnoPortStatusTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_PNO_ALIAS)    /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
	{
		if(pPortConfig->PnoAliasTxEnable.Value != pPortConfig->PnoAliasTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->PnoAliasTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_PNO_MRP)      /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
	{
		if(pPortConfig->PnoMrpTxEnable.Value != pPortConfig->PnoMrpTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->PnoMrpTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_PNO_PTCP)     /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
	{
		if(pPortConfig->PnoPtcpTxEnable.Value != pPortConfig->PnoPtcpTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->PnoPtcpTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_DOT3_TLVS) /* lldpXdot3PortConfigTLVsTxEnable */
	{
		if(pPortConfig->Dot3TLVsTxEnable.Value != pPortConfig->Dot3TLVsTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->Dot3TLVsTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_DOT1_VLAN_ID) /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
	{
		if(pPortConfig->Dot1VlanIdTxEnable.Value != pPortConfig->Dot1VlanIdTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->Dot1VlanIdTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_DOT1_VLAN_NAME)	 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
	{
		if(pPortConfig->Dot1VlanNameTxEnable.Value != pPortConfig->Dot1VlanNameTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->Dot1VlanNameTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_DOT1_PROTO_VLAN) /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
	{
		if(pPortConfig->Dot1PortProtoVlanTxEnable.Value != pPortConfig->Dot1PortProtoVlanTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->Dot1PortProtoVlanTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}
	if (PortSetOptions & OHA_CTRL_DOT1_PROTOCOL) /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */
	{
		if(pPortConfig->Dot1ProtocolTxEnable.Value != pPortConfig->Dot1ProtocolTxEnable.TmpSetVal)
		{
			return(IsDifferent = LSA_TRUE);
		}
		if(CheckRema && (MakeRema != pPortConfig->Dot1ProtocolTxEnable.Remanent))
		{
			return(IsDifferent = LSA_TRUE);
		}
	}

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPComparePorts(0x%X)", IsDifferent);
    return(IsDifferent);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPPrepareStats                        +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK, OHA_ERR_RESOURCE                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads necessary query access from LLDP-DB, look for query +*/
/*+               resources, returns the count of user queries if existing  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPPrepareStats(
    LSA_UINT16                   Orginator,
	OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfQueries,
	LSA_BOOL                     MakeRema
	)
{
    LSA_UINT16              CountOfQueries = 0;
    LSA_UINT16              RetVal = OHA_OK;
    LSA_UINT16              do_send;	

    OHA_STATS_INFO_PTR_TYPE pStatsInfo = oha_snmp_get_stats_info();

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPPrepareStats(Orginator: 0x%X, pCountOfQueries: 0x%X, MakeRema: 0x%X)", Orginator, pCountOfQueries, MakeRema);

	OHA_ASSERT_NULL_PTR(pCountOfQueries);

	*pCountOfQueries = 0; /* init */

	/* look for query access */
	OHA_ASSERT_NULL_PTR(pStatsInfo);

	/* first we check (do_work = 0) and if ok (do_work = 1), we send the query.ind(s) to the user(s) */
	for(do_send = 0; do_send < 2; do_send++)
    {
        LSA_UINT16  ifs, ports;
		OHA_HDB_LLDP_PTR_TYPE pHDBLldp;
		LSA_UINT32 GlobQueryOptions = OHA_GetGlobQueryOptions();

		CountOfQueries = 0;
		GlobQueryOptions &= pStatsInfo->GlobOptions; /* only the corresponding options */

	    /* are all the necessary user query resources existing ? */
		/**************************************/
		/*   look for If query resource(s)    */
		/**************************************/
		if(GlobQueryOptions)
		{
			/* something changed for global (query) indication ? */
			if (OHA_LLDPCompareGlobals(pStatsInfo->GlobOptions, MakeRema, LSA_TRUE))
			{
				for (ifs=0; ifs < OHA_CFG_MAX_NICS; ifs++)
				{
					pHDBLldp = OHA_LLDPGetHandleByIndex((LSA_UINT16)(ifs+1)); 
					if(OHA_IS_NULL(pHDBLldp))
					{
						continue;
					}

					OHA_ASSERT(pHDBLldp->QueryIfPendCnt == 0);
				
					if(OHA_IS_NOT_NULL(pHDBLldp->pHDBUserQueryIf)) /* there's a query user ? */
					{
						if(OHA_IS_NULL(pHDBLldp->pHDBUserQueryIf->LldpConfigInd.pBottom)) /* ind resource(s) existing */
						{
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "No query resource existing for IfQueryUser (%Xh)", pHDBLldp->pHDBUserQueryIf);
							RetVal = OHA_ERR_RESOURCE; /* not this query If user resource existing */
							OHA_ASSERT(!do_send);   /* only by the check */
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
							return(RetVal);
						}

						if(do_send)	/* send an If query to the user */
						{
							RetVal = OHA_LldpQueryConfigEvent(pHDBLldp->pHDBUserQueryIf, 
															  pStatsInfo->GlobOptions, MakeRema);
							OHA_ASSERT(RetVal == LSA_RET_OK);

							pHDBLldp->QueryIfPendCnt++;
						}
						CountOfQueries++;
					}
				}

				/* at least one query If user shall exist */
				if(CountOfQueries == 0)
				{
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "No query user existing for GlobQueryOptions (%Xh)", GlobQueryOptions);
					RetVal = OHA_ERR_RESOURCE; /* no query If user existing anyway */
					OHA_ASSERT(!do_send);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}
			}
		}

		/**************************************************/
		/*  look for port queries and their resource(s)   */
		/**************************************************/
        for (ifs=0; ifs < OHA_CFG_MAX_NICS; ifs++)
        {
			LSA_UINT32 IfOptions = pStatsInfo->IfOptions[ifs];
			LSA_UINT16 NicID;

			pHDBLldp = OHA_LLDPGetHandleByIndex((LSA_UINT16)(ifs+1)); 
			if(OHA_IS_NULL(pHDBLldp))
			{
				continue;
			}

			OHA_ASSERT(pHDBLldp->QueryPortPendCnt == 0);
			NicID = pHDBLldp->Params.pDetail->Params.Lldp.NICId;

			if(IfOptions)
			{
				RetVal = OHA_LLDPSetTempIfOptions(NicID, IfOptions); /* update the MIB-DB */
				OHA_ASSERT(RetVal == OHA_OK);
				pStatsInfo->IfOptions[ifs] = 0;	/* delete the if-options */
			}

            for (ports=0; ports < OHA_CFG_MAX_PORTS; ports++)
            {
				LSA_UINT32 PortOptions = pStatsInfo->PortOptions[ifs][ports];
				LSA_UINT32 PortQueryOptions = OHA_GetPortQueryOptions(NicID, (LSA_UINT16)(ports+1));

				PortQueryOptions &= PortOptions; /* only the corresponding options */
#if 0
				if(PortOptions & OHA_CTRL_ADMIN_STATUS)
				{
					if(OHA_IS_NOT_NULL(pHDBLldp->UserRQBs.pBottom))
					{
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "No set of admin status during an active user request (Opcode: %Xh)", GetOpcode(pHDBLldp->UserRQBs.pBottom));
						RetVal = OHA_ERR_SEQUENCE;
						OHA_ASSERT(!do_send);
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
				}
#endif
				if(PortOptions & OHA_CTRL_LLDP_TLVS)	/* check range of mib object lldpPortConfigTLVsTxEnable */
				{
					OHA_COMMON_MEM_U32_PTR_TYPE  pValue = LSA_NULL;
					RetVal = oha_db_read_temp_lldp_mib_port_value(NicID, (LSA_UINT16)(ports+1), OHA_CTRL_LLDP_TLVS, &pValue, LSA_NULL);
					OHA_ASSERT(RetVal == LSA_RET_OK);
					OHA_ASSERT_NULL_PTR(pValue);
					if((LSA_UINT8)*pValue & 0x0f)
					{
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "Mib object lldpPortConfigTLVsTxEnable out of range (*pValue: %Xh)", *pValue);
						RetVal = OHA_ERR_PARAM;
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
				}
				if(PortOptions & OHA_CTRL_DOT3_TLVS)	/* check range of mib object lldpXdot3PortConfigTLVsTxEnable */
				{
					OHA_COMMON_MEM_U32_PTR_TYPE  pValue = LSA_NULL;
					RetVal = oha_db_read_temp_lldp_mib_port_value(NicID, (LSA_UINT16)(ports+1), OHA_CTRL_DOT3_TLVS, &pValue, LSA_NULL);
					OHA_ASSERT(RetVal == LSA_RET_OK);
					OHA_ASSERT_NULL_PTR(pValue);
					if((LSA_UINT8)*pValue & 0x0f)
					{
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "Mib object lldpXdot3PortConfigTLVsTxEnable out of range (*pValue: %Xh)", *pValue);
						RetVal = OHA_ERR_PARAM;
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
				}

                if (PortQueryOptions)
                {
					/* something changed for port (query) indication ? */
					if (OHA_LLDPComparePorts(NicID, (LSA_UINT16)(ports+1),
											 PortOptions, MakeRema, LSA_TRUE))
					{
						if(OHA_IS_NULL(pHDBLldp->pHDBUserQueryPort)) /* there's no query user ? */
						{
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "No query user existing for PortQueryOptions (%Xh)", PortQueryOptions);
							RetVal = OHA_ERR_RESOURCE; /* not this query port user existing */
							OHA_ASSERT(!do_send);
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
							return(RetVal);
						}
						if(OHA_IS_NULL(pHDBLldp->pHDBUserQueryPort->LldpPortConfigInd.pBottom)) /* ind resource(s) existing */
						{
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "No query resource existing for PortQueryUser (%Xh)", pHDBLldp->pHDBUserQueryPort);
							RetVal = OHA_ERR_RESOURCE; /* not this query port user resource existing */
							OHA_ASSERT(!do_send);
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
							return(RetVal);
						}

						if(do_send)	/* send a port query to the user */
						{
							/* AP01369756: NULL if only portspecific user-queries existing */
							RetVal = OHA_LldpQueryPortConfigEvent(pHDBLldp->pHDBUserQueryPort, (LSA_UINT16)(ports+1),
																  PortOptions);
							OHA_ASSERT(RetVal == LSA_RET_OK);

							pHDBLldp->QueryPortPendCnt++;
						}
						CountOfQueries++;
						/* break; */
					}
				}
			}
		}
	}

	*pCountOfQueries = CountOfQueries;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPPrepareStats(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPDoStats                             +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : CountOfStatsNeeded                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads necessary statistics from LLDP or                   +*/
/*+               Writes the parameters to LLDP                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPDoStats(
    LSA_UINT16                   Orginator,
	LSA_UINT8                    MessageType,
	LSA_BOOL                     IsRema)
{
    LSA_UINT16              CountOfStats = 0;
	LSA_UINT16              RetVal;

    OHA_STATS_INFO_PTR_TYPE pStatsInfo = oha_snmp_get_stats_info();

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPDoStats(Orginator: 0x%X, MessageType: 0x%X, IsRema: 0x%X)", Orginator, MessageType, IsRema);

	OHA_ASSERT_NULL_PTR(pStatsInfo);

    /* statistic data (asynchronous) from LLDP_GET_STATS needed ? */
    if (oha_snmp_get_lldp_statistics())
    {
        LSA_UINT16 ifs, ports;
		LSA_UINT32 GlobOptions = pStatsInfo->GlobOptions;

        for (ifs=0; ifs < OHA_CFG_MAX_NICS; ifs++)
        {
			OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByIndex((LSA_UINT16)(ifs+1));
			LSA_UINT16 NicID;
			LSA_BOOL   RemaUpdate = LSA_FALSE;

			if(OHA_IS_NULL(pHDBLldp))
			{
				continue;
			}
			NicID = pHDBLldp->Params.pDetail->Params.Lldp.NICId;

			switch(MessageType)
			{
				case OHA_SNMP_GET_REQ:
				case OHA_SNMP_GETNEXT_REQ:
				break;
				case OHA_SNMP_SET_REQ:
				{
					/* the global Options 'TxInterval', 'ReinitDelay', 'TxHoldMultiplier' and 'TxDelay' leads to a LLDP_SET_GLOBALS first */
					if(GlobOptions & OHA_CTRL_LLDP_SET_GLOBALS) /* async call to do ? */
					{
						/* something changed for LldpSetGlobals execution ? */
						if (OHA_LLDPCompareGlobals(GlobOptions & OHA_CTRL_LLDP_SET_GLOBALS, IsRema, LSA_FALSE))
						{
							RetVal = OHA_LLDPUpdateGlobals(NicID, GlobOptions, Orginator);
							if (RetVal == OHA_OK)
							{
								CountOfStats++;
							}
							else
							{
								OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_LLDPDoStats: OHA_LLDPUpdateGlobals failed (RetVal 0x%X)", RetVal);
								break;
							}
						}
					}
					if(GlobOptions)
					{
						/* something changed for Globals indication ? */
						if (OHA_LLDPCompareGlobals(GlobOptions, IsRema, LSA_TRUE))
						{
							OHA_LLDPSetMibOptions(GlobOptions, IsRema); /* update the MIB-DB */

							/* first indicate the user(s) */
							OHA_LldpIndicateConfigEventAllUsers(pHDBLldp, GlobOptions, Orginator);
							/* update the REMA-DB */
							RemaUpdate = LSA_TRUE;
						}
					}
				}
				break;
				default:
			        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				break;
			}

            for (ports=0; ports < OHA_CFG_MAX_PORTS; ports++)
            {
				LSA_UINT32 PortOptions = pStatsInfo->PortOptions[ifs][ports];

                if (PortOptions)
                {
                    switch(MessageType)
					{
						case OHA_SNMP_GET_REQ:
						case OHA_SNMP_GETNEXT_REQ:
							RetVal = OHA_LLDPGetStats(pHDBLldp, (LSA_UINT16)(ports+1), Orginator);
							if (RetVal == OHA_OK) /* async call done */
							{
								CountOfStats++; /* one lldp request underway */
							}
						break;
						case OHA_SNMP_SET_REQ:
							/* something changed for port execution ? */
							if (OHA_LLDPComparePorts(NicID, (LSA_UINT16)(ports+1), PortOptions, IsRema, LSA_FALSE))
							{
								/* the port Options 'AdminStatus' may leads to a LLDP_START or LLDP_STOP first */
								if(PortOptions & OHA_CTRL_ADMIN_STATUS) /* execute first */
								{
									if(OHA_IS_NOT_NULL(pHDBLldp->UserRQBs.pBottom))
									{
										OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);	/* an OpcLldpControl or PrmCommit is active, should have been rejected */
									}
									if(pHDBLldp->RequestState == OHA_LLDP_STATE_START) /* LLDP-Start is possible ? */
									{
										RetVal = OHA_LLDPUpdatePorts(NicID, (LSA_UINT16)(ports+1), Orginator);
										if (RetVal == OHA_OK_ACTIVE)
										{
											CountOfStats++; /* one lldp request (start/stop) underway */
										}
										else if (RetVal != OHA_OK)
										{
											OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_LLDPDoStats: OHA_LLDPUpdatePorts failed (RetVal 0x%X)", RetVal);
											break;
										}
									}
								}
							}
							/* something changed for port indication ? */
							if (OHA_LLDPComparePorts(NicID, (LSA_UINT16)(ports+1), PortOptions, IsRema, LSA_TRUE))
							{
								(LSA_VOID)OHA_LLDPSetMibPortOptions(NicID, (LSA_UINT16)(ports+1), PortOptions, IsRema); /* update the MIB-DB */

								/* first indicate the user(s) */
								OHA_LldpIndicatePortConfigEventAllUsers(pHDBLldp, (LSA_UINT16)(ports+1), PortOptions, 
																		Orginator);
								/* update the REMA-DB */
								RemaUpdate = LSA_TRUE;

								if(PortOptions & OHA_CTRL_LLDP_TX_PORTS) /* TxEnable's: LldpSet(ThisPort) */
								{
									/* we update this port (TTL = TxHoldMultiplier * TxInterval) */
									if (pHDBLldp->RequestState == OHA_LLDP_STATE_START ||
										pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START ||
										pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START) /* LLDP-Set is possible ? */
									{
										RetVal = OHA_LLDPSet(pHDBLldp, (LSA_UINT16)(ports+1), OHA_HANDLE_TYPE_SYS); /* send them */
										if (RetVal != LSA_RET_OK)
										{
											OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"OHA_LLDPDoStats: OHA_LLDPSet() failed (RetVal: 0x%X).", RetVal);
										}
									}
								}
							}
						break;
						default:
				            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
						break;
					}
                }
            }

#ifndef OHA_CFG_NO_REMA
			if(RemaUpdate)
			{
				/* update the REMA-DB */
				(LSA_VOID)oha_rema_snmp_update(NicID);
			}
#else
			LSA_UNUSED_ARG(RemaUpdate);
#endif
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPDoStats(OHA_LLDPDoStats: 0x%X)", CountOfStats);
    return(CountOfStats);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LLDPUpdateStats                         +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_PARAM                                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Updates LLDP statistic data                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateStats(
    OHA_DB_IF_PTR_TYPE        pDB,
    LSA_UINT16                PORTId,
    LSA_UINT16                SenderId,
    LSA_UINT16                Event
)
{
    LSA_UINT16              Status = OHA_OK;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPUpdateStats(pDB: 0x%X, PORTId: 0x%X, SenderId: 0x%X, Event: 0x%X)", pDB, PORTId, SenderId, Event);

    OHA_ASSERT_NULL_PTR(pDB);

    /* LLDP: TooManySender, if SenderId = 0 ? */
    if ((SenderId == 0 && Event == LLDP_EVENT_TOO_MANY_SENDER) ||
        SenderId > LLDP_MAX_PORT_SENDER)
    {
        pDB->LldpStatistics.RemTabDrops++;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pDB->NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdateStats(Status: 0x%X)", Status);
        return(Status);
    }

    if (SenderId == 0 ||  /* check it */
        PORTId   == 0 || PORTId   > OHA_CFG_MAX_PORTS)
    {
        Status = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pDB->NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdateStats(Status: 0x%X)",Status);
        return(Status);
    }

    switch (Event)
    {
        case LLDP_EVENT_DATA_VALID:     /* new or changed data                  */
            if (pDB->LldpData[PORTId-1].RemoteSender[SenderId-1].LldpEvent != LLDP_EVENT_DATA_VALID)
            {
                pDB->LldpStatistics.LastChangeTime = OHA_GET_SYS_UPTIME();
                pDB->LldpStatistics.RemTabInserts++;
            }
            else    /* LldpEvent == LLDP_EVENT_DATA_VALID */
            {
                /* if (pDB->LldpStatistics.RemTabInserts == 0) *//* initial value */
				if (pDB->LldpData[PORTId-1].RemoteSender[SenderId-1].SnmpSenderStatus == 0) /* initial value */
                {
                    pDB->LldpStatistics.LastChangeTime = OHA_GET_SYS_UPTIME();
                    pDB->LldpStatistics.RemTabInserts++;
                }
            }
            break;
        case LLDP_EVENT_SHUTDOWN:       /* shutdown received. data invalid      */
            if (pDB->LldpData[PORTId-1].RemoteSender[SenderId-1].LldpEvent != LLDP_EVENT_SHUTDOWN &&
                pDB->LldpData[PORTId-1].RemoteSender[SenderId-1].LldpEvent != LLDP_EVENT_DATA_INVALID)
            {
                pDB->LldpStatistics.LastChangeTime = OHA_GET_SYS_UPTIME();
                pDB->LldpStatistics.RemTabDeletes++;
            }
            break;
        case LLDP_EVENT_TOO_MANY_SENDER: /* too many sender.                    */
            if (pDB->LldpData[PORTId-1].RemoteSender[SenderId-1].LldpEvent != LLDP_EVENT_TOO_MANY_SENDER)
            {
                pDB->LldpStatistics.RemTabDrops++;
            }
            break;
        case LLDP_EVENT_DATA_INVALID:   /* data invalid because of TTL exceeded */
            if (pDB->LldpData[PORTId-1].RemoteSender[SenderId-1].LldpEvent != LLDP_EVENT_DATA_INVALID)
            {
                pDB->LldpStatistics.LastChangeTime = OHA_GET_SYS_UPTIME();
                pDB->LldpStatistics.RemTabAgeouts++;
            }
            break;
        default:
            Status = OHA_ERR_PARAM;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pDB->NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdateStats(Status: 0x%X)",Status);
            return(Status);
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pDB->NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUpdateStats(Status: 0x%X)", Status);
    return(Status);
}

/*=============================================================================
 * function name:  OHA_LLDPSetTempIfOptions
 * function:       converts the interface specific mib objects and writes them to the temporary PortConfig 
 * parameters:     IfOptions (LLDP.MIB port option flags)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPSetTempIfOptions(
    LSA_UINT16                  NicId,
	LSA_UINT32                  IfSetOptions)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = OHA_OK;
    OHA_STATS_INFO_PTR_TYPE  pStatsInfo = oha_snmp_get_stats_info();

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPSetTempIfOptions(NicId: 0x%X, IfSetOptions: 0x%X)",
                          NicId, IfSetOptions);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
		RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPSetTempIfOptions(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }

	OHA_ASSERT_NULL_PTR(pStatsInfo);

	if (IfSetOptions & OHA_CTRL_LLDP_MAN_ADDR) /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
	{
		LSA_UINT16 AllPortsCnt = oha_snmp_count_if_ports(); /* all ports */
		LSA_UINT16 port;

		/* from temporary mib setting, we have the temp data storage in TempTxPortTable ! */
		/* A set of ports that are identified by a PortList, in which each port is represented as a bit. */

		/* find the ports for this interface */
        for (port = 0; (port < 255) && (port < AllPortsCnt); port++)
        {
		    LSA_UINT  PortId = 0;
			LSA_UINT  inter_face   = oha_snmp_get_if_id(port+1, &PortId);
	        OHA_DB_IF_PTR_TYPE pDB = OHA_GetIf((LSA_UINT16)inter_face);
            LSA_UINT8 Help         = pDbIf->TempTxPortTable[port/8];

            Help <<= (port % 8);
			Help &= 0x80;	/* the highest bit */

            /* get the corresponding port "bits" only */
			if(OHA_IS_NOT_NULL(pDB) && pDB->NicId == NicId)	/* port belongs to this interface ? */
			{
				OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;
				LSA_UINT32 TxEnable = (Help) ? OHA_LLDP_TX_ENABLE : OHA_LLDP_TX_DISABLE;
	
				OHA_ASSERT(inter_face && (inter_face <= OHA_CFG_MAX_NICS));
				OHA_ASSERT(PortId && (PortId <= OHA_CFG_MAX_PORTS));

				/* write the temp. value to the mib database */
				pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;
				pPortConfig->ManAddrTxEnable.TmpSetVal = TxEnable;

				/* set the port options too */ /* AP01467385: port -> PortId-1 */
				pStatsInfo->PortOptions[inter_face-1][PortId-1] |= OHA_CTRL_LLDP_MAN_ADDR;
            }
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPSetTempIfOptions(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*****************************************************************************/
/*  end of file OHA_LLDP.C                                                   */
/*****************************************************************************/

