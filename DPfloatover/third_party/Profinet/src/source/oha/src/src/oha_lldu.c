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
/*  F i l e               &F: oha_lldu.c                                :F&  */
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

#define LTRC_ACT_MODUL_ID  13
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_LLDU */

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

LSA_UINT16 OHA_LLDPCountQueriesPending (LSA_VOID)
{
    LSA_UINT16 ifs;
    LSA_UINT16 CountQueries = 0;

    for (ifs=0; ifs < OHA_CFG_MAX_NICS; ifs++)
    {
		OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByIndex((LSA_UINT16)(ifs+1));

		if(OHA_IS_NULL(pHDBLldp))
		{
			continue;
		}

		CountQueries += pHDBLldp->QueryIfPendCnt;
		CountQueries += pHDBLldp->QueryPortPendCnt;
	}

	return(CountQueries);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UserCheckPending                        +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to LLDP-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPUserCheckPending(
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp,
    LSA_UINT16              Response)
{
    OHA_HDB_USER_PTR_TYPE   pHDBUser;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              NICId;

	OHA_ASSERT_NULL_PTR(pHDBLldp);
	NICId = pHDBLldp->Params.pDetail->Params.Lldp.NICId;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LLDPUserCheckPending(pHDBLldp: 0x%X, Response: 0x%X)",
                          pHDBLldp, Response);

    if ( pHDBLldp->UserRQBCnt ) /* User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBLldp->UserRQBs.pBottom,
                                 pHDBLldp->UserRQBs.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
        pHDBLldp->UserRQBCnt--;

        /* control = &pRQBUser->args.LldpControl; */
        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);
        OHA_ASSERT_NULL_PTR(pHDBUser);

        pHDBUser->LldpReqPendingCnt--; /* Number of pend. Req's (LLDP)*/

        /* what shall happen with this RQB */
        switch (OHA_RQB_GET_OPCODE(pRQBUser))
        {
            case OHA_OPC_PORT_CONTROL:
            {
                LSA_UINT16 PortId = pRQBUser->args.PortControl.PortId;
                OHA_ASSERT(PortId > 0 && PortId <= OHA_CFG_MAX_PORTS);

                /* set the new admin status */
				RetVal = OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_ADMIN_STATUS, 
														  pRQBUser->args.PortControl.Mode, sizeof(LSA_UINT32), LSA_FALSE); /* default */
				OHA_ASSERT(RetVal == LSA_RET_OK);
            }
            OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);  /* confirm the user */
            break;
            case OHA_OPC_TEST_LLDP_CONTROL:
                OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);  /* confirm the user */
                break;
            case OHA_OPC_PRM_COMMIT:
                /* free internal RQB for LldpSend */
                OHA_FREE_UPPER_RQB_LOCAL(&RetVal, pRQBUser);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
                break;
            default:    /* invalid user rquest */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_LLDPUserCheckPending(): Invalid user request (%Xh)",OHA_RQB_GET_OPCODE(pRQBUser));
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }

        /* the last pending lldp request and close channel active */
        if (pHDBUser->LldpReqPendingCnt == 0)
        {
            if (OHA_IS_NOT_NULL(pHDBUser->pOCReq) &&
                OHA_RQB_GET_OPCODE(pHDBUser->pOCReq) == OHA_OPC_CLOSE_CHANNEL)
            {
                OHA_USERCloseChannelDone(pHDBUser, OHA_OK);
            }
        }
    }

    /* #ifdef OHA_LLDP_SEND_BY_PRM_COMMIT */
    while ( pHDBLldp->UserRQBCnt ) /* further User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

        pRQBUser = pHDBLldp->UserRQBs.pBottom;

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);

        /* control = &pRQBUser->args.LldpControl; */
        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);
        OHA_ASSERT_NULL_PTR(pHDBUser);

        /* is no open/close user-channel runnning ? */
        if ( (pHDBUser->State == OHA_HANDLE_STATE_OPEN) &&
             LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) )
        {

            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"OHA_LLDPUserCheckPending(): queued request called");
            OHA_LLDPRequestHandler(pHDBLldp, pRQBUser, OHA_OK);  /* call the lldp-handler */
            OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUserCheckPending()");
            return;
        }

        /* a close channel or (re)open channel is running */
        /* we've to cancel the requests                   */
        OHA_RQB_REM_BLOCK_BOTTOM(pHDBLldp->UserRQBs.pBottom,
                                 pHDBLldp->UserRQBs.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
        pHDBLldp->UserRQBCnt--;

        /* control = &pRQBUser->args.LldpControl; */
        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);

        pHDBUser->LldpReqPendingCnt--; /* Number of pend. Req's (LLDP)*/

        /* what shall happen with this RQB */
        switch (OHA_RQB_GET_OPCODE(pRQBUser))
        {
            case OHA_OPC_TEST_LLDP_CONTROL:
                OHA_UserRequestFinish(pHDBUser,pRQBUser,OHA_OK_CANCEL); /* cancel the user request */
                break;
            case OHA_OPC_PORT_CONTROL:
                OHA_UserRequestFinish(pHDBUser,pRQBUser,OHA_OK_CANCEL); /* cancel the user request */
                break;
            case OHA_OPC_PRM_COMMIT:
                /* free internal RQB for LldpSend */
                OHA_FREE_UPPER_RQB_LOCAL(&RetVal, pRQBUser);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
                break;
            default:    /* invalid user rquest */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_LLDPUserCheckPending(): Invalid user request (%Xh)",OHA_RQB_GET_OPCODE(pRQBUser));
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }

        /* the last pending lldp request and close channel active */
        if (pHDBUser->LldpReqPendingCnt == 0)
        {
            if (OHA_IS_NOT_NULL(pHDBUser->pOCReq) &&
                OHA_RQB_GET_OPCODE(pHDBUser->pOCReq) == OHA_OPC_CLOSE_CHANNEL)
            {
                OHA_USERCloseChannelDone(pHDBUser, OHA_OK);
            }
        }
    }
    /* #endif */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LLDPUserCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpControl                      +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_TEST_LLDP_CONTROL       +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.LldpControl                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to control the LLDP for sending local neighbour-  +*/
/*+               data                                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
    LSA_BOOL                Finish = LSA_FALSE; /* dont finish request by default */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpControl(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;
    /* Check if we have LLDP and a EDD-Channel for NIC-ID. */
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);  /* only EDD and LLDP for this request necessary */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_TEST_LLDP_CONTROL failed. No Lower Channel for this NicId (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }
    else if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL))
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_TEST_LLDP_CONTROL failed. No LLDP-Channel present (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }
    else if (OHA_IS_NULL(pHDBLldp->pDB) || pHDBLldp->pDB->PortCnt == 0)
    {
        Response = OHA_ERR_PARAM;
        /* Error: no ports. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_TEST_LLDP_CONTROL failed. No Ports present (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }
    /* #ifndef OHA_LLDP_SEND_BY_PRM_COMMIT */
    /* else if(pHDBLldp->UserRQBCnt)  *//* no queueing of user-requests in this version */
    /* {                                                                                */
    /*  Finish = LSA_TRUE;                                                              */
    /*      OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_TEST_LLDP_CONTROL failed. Already one User-Request active for this NicId (NIC-ID: 0x%X)",pRQB->args.LldpControl.NicId); */
    /*  Response = OHA_ERR_SEQUENCE;                                                    */
    /* }                                                                                */
    /* #endif */
    else
    {
        OHA_UPPER_LLDP_CONTROL_PTR_TYPE control = &pRQB->args.LldpControl;

        switch (control->Mode)
        {
            case OHA_START_SEND:  /* start sending of the local neighbour-data */
                /* if there's not registered this intercace, EddGetPortParams is not called. */
                /* We need this info for the mib (e.g. Auto-Neg. Advertised Cap, Operational MAU Type) */
                if (!OHA_TOPOIsIfRegistered(pHDBLldp->pDB->InterfaceID))
                {
                    /* get the TopoRequestHandler for calling OHA_EDDGetPortParams */
                    OHA_RQB_PUT_BLOCK_TOP(pHDBUser->TopoUserReq.pBottom,
                                          pHDBUser->TopoUserReq.pTop,
                                          pRQB);
                    pHDBUser->TopoReqPendingCnt++;      /* Number of pend. User-TOPO-requests */
                    OHA_TOPORequestHandler(pHDBUser, NICId, OHA_OK);  /* call the topo-handler */

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpControl(Response: 0x%X)", Response);
                    return;
                }
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Control Start-Send successful");
                break;
            case OHA_STOP_SEND:  /* start sending of the local neighbour-data */
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Control Stop-Send successful");
                break;
            case OHA_RESET_SEND:  /* updates the local neighbour-data */
                /* if there's not registered this intercace, EddGetPortParams is not called. */
                /* We need this info for the mib (e.g. Auto-Neg. Advertised Cap, Operational MAU Type) */
                if (!OHA_TOPOIsIfRegistered(pHDBLldp->pDB->InterfaceID))
                {
                    /* get the TopoRequestHandler for calling OHA_EDDGetPortParams */
                    OHA_RQB_PUT_BLOCK_TOP(pHDBUser->TopoUserReq.pBottom,
                                          pHDBUser->TopoUserReq.pTop,
                                          pRQB);
                    pHDBUser->TopoReqPendingCnt++;      /* Number of pend. User-TOPO-requests */
                    OHA_TOPORequestHandler(pHDBUser, NICId, OHA_OK);  /* call the topo-handler */

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpControl(Response: 0x%X)", Response);
                    return;
                }
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Control Reset successful");
                break;
            default:
                Finish = LSA_TRUE;
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"LLDP-Control failed.[Mode: 0x%X]",control->Mode);
                Response = OHA_ERR_SYNTAX;
                break;
        }
    }

    if ( Finish )  /* finish the request ? */
    {
        OHA_UserRequestFinish(pHDBUser,pRQB,Response); /*an error occurred */
    }
    else
    {
        OHA_ASSERT_NULL_PTR(pHDBLldp);
        /* queue the request in the LLDP-HDB */
        OHA_RQB_PUT_BLOCK_TOP(pHDBLldp->UserRQBs.pBottom,
                              pHDBLldp->UserRQBs.pTop,
                              pRQB);
        pHDBLldp->UserRQBCnt++;
        pHDBUser->LldpReqPendingCnt++;      /* Number of pend. User-LLDP-requests */
        if ( pHDBLldp->UserRQBCnt == 1) /* no further User-RCTRL-Blocks pending ? */
        {
            OHA_LLDPRequestHandler(pHDBLldp, pRQB, OHA_OK);  /* call the lldp-handler */
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpControl(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPortControl                      +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PORT_CONTROL            +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PortControl                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to enable/disable a port for sending local lldp-  +*/
/*+               data (default: enable)                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPortControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId;
    LSA_UINT16              PortId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
    LSA_BOOL                Finish = LSA_FALSE; /* dont finish request by default */
    OHA_UPPER_PORT_CONTROL_PTR_TYPE control;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPortControl(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
    PortId  = pRQB->args.PortControl.PortId;
    control = &pRQB->args.PortControl;
    /* Check if we have LLDP and a EDD-Channel for NIC-ID. */
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);  /* only EDD and LLDP for this request necessary */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PORT_CONTROL failed. No Lower Channel for this NicId (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }
    else if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL))
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PORT_CONTROL failed. No LLDP-Channel present (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }
    else if (OHA_IS_NULL(pHDBLldp->pDB) || pHDBLldp->pDB->PortCnt == 0)
    {
        Response = OHA_ERR_PARAM;
        /* Error: no ports. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PORT_CONTROL failed. No Ports present (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }

    if (!Finish)
    {
        OHA_ASSERT_NULL_PTR(pHDBLldp);
        if (PortId == 0 || PortId > pHDBLldp->pDB->PortCnt)
        {
            Response = OHA_ERR_PARAM;  /* Portid is wrong */
            /* Error on getting handle. Abort. */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PORT_CONTROL failed. Portid is wrong (Port-ID: 0x%X)",pRQB->args.PortControl.PortId);
            Finish = LSA_TRUE;
        }
        else
        {
            switch (control->Mode)
            {
                case OHA_LLDP_SEND_ON:  /* enables sending of lldp-data  */
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Control Enable-Send successful");
                    break;
                case OHA_LLDP_SEND_OFF:  /* disables sending of lldp-data */
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-Control Disable-Send successful");
                    break;
                default:
                    Finish = LSA_TRUE;
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"PORT-Control failed. AdminStatus is wrong (AdminStatus: 0x%X)", pRQB->args.PortControl.Mode);
                    Response = OHA_ERR_PARAM;
                    break;
            }
        }
    }

    if ( Finish )  /* finish the request ? */
    {
        OHA_UserRequestFinish(pHDBUser,pRQB,Response); /*an error occurred */
    }
    else
    {
        OHA_ASSERT_NULL_PTR(pHDBLldp);
        /* queue the request in the LLDP-HDB */
        OHA_RQB_PUT_BLOCK_TOP(pHDBLldp->UserRQBs.pBottom,
                              pHDBLldp->UserRQBs.pTop,
                              pRQB);

        pHDBLldp->UserRQBCnt++;
        pHDBUser->LldpReqPendingCnt++;      /* Number of pend. User-LLDP-requests */
        if ( pHDBLldp->UserRQBCnt == 1) /* no further User-RCTRL-Blocks pending ? */
        {
            OHA_LLDPRequestHandler(pHDBLldp, pRQB, OHA_OK);  /* call the lldp-handler */
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPortControl(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestGetNeighbour                     +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_GET_NEIGHBOUR           +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Neighbour                               +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to get the LLDP neighbour-data for one port of    +*/
/*+               a NIC                                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestGetNeighbour(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestGetNeighbour(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;
    /* Check if we have LLDP and a EDD-Channel for NIC-ID. */
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);  /* only EDD and LLDP for this request necessary */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_GET_NEIGHBOUR failed. No Lower Channel for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL))
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_GET_NEIGHBOUR failed. No EDD-Channel present (NIC-ID: 0x%X)",NICId);
    }
    else
    {
        OHA_UPPER_GET_NEIGHBOUR_PTR_TYPE pNeighbour = &pRQB->args.Neighbour;

        if (OHA_IS_NOT_NULL(pHDBLldp->pDB) && pNeighbour->PortId && (pNeighbour->PortId <= pHDBLldp->pDB->PortCnt))  /* pHDB->PortCnt */
        {
            LSA_UINT16 SenderId = pHDBLldp->pDB->LldpData[pNeighbour->PortId-1].LldpSenderId;
            LSA_UINT8  SenderStatus = pHDBLldp->pDB->LldpData[pNeighbour->PortId-1].LldpSenderStatus;

            /* TFS 1210097: If the are more stations connected to 1 Port the OHA don't respond */
			/* with the response OHA_STATUS_MULTIPLE_SENDER(13) but with OHA_STATUS_NOT_PRESENT(10) */
            pNeighbour->Status = (SenderStatus) ? SenderStatus : (LSA_UINT8)OHA_STATUS_NOT_PRESENT;   /* entry of presence */

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* check it */
            {
                OHA_UPPER_LLDPDU_PTR_TYPE pLLDPDU = pNeighbour->pLLDPDU;

                OHA_MEMCPY(&pRQB->args.Neighbour.SenderMacAddr,
                           &pNeighbour->SenderMacAddr, OHA_MAC_ADDR_SIZE);
                if ((SenderStatus == OHA_STATUS_DATA_VALID ||
                     SenderStatus == OHA_STATUS_DATA_INVALID) &&
                    OHA_IS_NOT_NULL(pNeighbour->pLLDPDU))  /* presence and a pLLDPDU exists in the RQB */
                {
                    /* read Chassis ID   */
                    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                    LSA_UINT16 Len = 0;
                    LSA_UINT8  SubType = 0xff;

                    LSA_UINT16 Response = oha_db_read_rem_chassis_name(NICId, pNeighbour->PortId, SenderId,
                                                                       &pLLDPDU->TypeOfChassis,    /* subtype of variable, suboption */
                                                                       &pVar,
                                                                       &pLLDPDU->ChassisNameLen);

                    if ((Response == LSA_RET_OK) && pLLDPDU->pNameOfChassis && OHA_IS_NOT_NULL(pVar) &&
                        (pLLDPDU->ChassisNameLen <= OHA_MAX_CHASSIS_STRLEN))
                    {
                        OHA_MEMCPY(pLLDPDU->pNameOfChassis, pVar, pLLDPDU->ChassisNameLen);
                    }

                    /* read Port ID  */
                    pVar = LSA_NULL;
                    Response = oha_db_read_rem_port_name(NICId, pNeighbour->PortId, SenderId,
                                                            &pLLDPDU->TypeOfPort,    /* subtype of variable, suboption */
                                                            &pVar,
                                                            &pLLDPDU->PortNameLen);

                    if ((Response == LSA_RET_OK) && pLLDPDU->pNameOfPort && OHA_IS_NOT_NULL(pVar) &&
                        (pLLDPDU->PortNameLen <= OHA_MAX_PORT_STRLEN))
                    {
                        OHA_MEMCPY(pLLDPDU->pNameOfPort, pVar, pLLDPDU->PortNameLen);
                    }

                    /* read TTL  */
                    pVar = LSA_NULL;
                    Response = oha_db_read_rem_lldp_element(NICId, pNeighbour->PortId, SenderId,
                                                            OHA_LLDP_TYPE_TTL,
                                                            &SubType, 0,
                                                            &pVar,
                                                            &Len);
                    if ((Response == LSA_RET_OK) && OHA_IS_NOT_NULL(pVar) &&
                        (Len == sizeof(LSA_UINT16)))
                    {
                        pLLDPDU->TimeToLive  = *((OHA_COMMON_MEM_U16_PTR_TYPE)(pVar));
                    }
                }
            }
        }
        else
        {
            Response = OHA_ERR_PARAM;  /* Portid or SenderId is wrong */
            /* Error on getting handle. Abort. */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_GET_NEIGHBOUR failed. Portid or SenderId is wrong (NIC-ID: 0x%X)",NICId);
        }
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestGetNeighbour(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPGetOidElement                       +*/
/*+                        :    OHA_UPPER_GET_OID_PTR_TYPE pSnmpOid         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQBUser   : Pointer to User-GetOid-Block                              +*/
/*+  Response   : Response of last action. if <> OHA_OK the Request         +*/
/*+               will be finished with this error. Set to OHA_OK if        +*/
/*+               not error present (e.g. on first call)                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: reads an OID from the MIB and writesi sto the RQB         +*/
/*+               The RCTRL-Block contains all information about the        +*/
/*+               requested OID.                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT32 OHA_LOCAL_FCT_ATTR OHA_SNMPGetOidElement(
    OHA_UPPER_GET_OID_PTR_TYPE pSnmpOid,
	OHA_OID                    Node)
{
    OHA_ASSERT_NULL_PTR(pSnmpOid);

    /* now attempt to retrieve the variable on the local entity */
    pSnmpOid->SnmpError = oha_control_snmp_oid(Node, pSnmpOid->pOidName, 
											   &pSnmpOid->OidNameLen, /* AP01256836 */
                                               pSnmpOid->pVarValue, &pSnmpOid->VarType, 
											   &pSnmpOid->VarLength, &pSnmpOid->AccessControl,
                                               pSnmpOid->MessageType, pSnmpOid->SnmpVersion);

    OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,"OHA_SNMPGetOidElement: Node 0x%X, MessageType 0x%X, SnmpError (0x%X)", Node, pSnmpOid->MessageType, pSnmpOid->SnmpError);

	return (pSnmpOid->SnmpError);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpRequestHandler                      +*/
/*+                        :    OHA_HDB_LLDP_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
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
/*+  Description: Statemaching for LLDP-User-Requests.                      +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPRequestHandler(
    OHA_HDB_LLDP_PTR_TYPE           pHDBLldp,
    OHA_UPPER_RQB_PTR_TYPE          pRQBUser,
    LSA_UINT16                      Response)
{
    /* OHA_UPPER_LLDP_CONTROL_PTR_TYPE control;  */
    /* LSA_BOOL                  CopyLLError;    */
    /* LSA_BOOL                  SetPending;     */
    /* LSA_HANDLE_TYPE           Handle = OHA_RQB_GET_HANDLE(pRQBUser); */
    LSA_BOOL                    Ready = LSA_FALSE;
    LSA_UINT8                   LldpMode;
    LSA_UINT16                  NICId;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpRequestHandler(pHDBLldp: 0x%X, pRQBUser: 0x%X, Response: 0x%X)",
                          pHDBLldp, pRQBUser, Response);

	OHA_ASSERT_NULL_PTR(pHDBLldp);
	NICId = pHDBLldp->Params.pDetail->Params.Lldp.NICId;

    /* what should happen with LLDP */
    if (OHA_IS_NOT_NULL(pRQBUser))
    {
        switch (OHA_RQB_GET_OPCODE(pRQBUser))
        {
            case OHA_OPC_TEST_LLDP_CONTROL:
                LldpMode = pRQBUser->args.LldpControl.Mode;
                break;
            case OHA_OPC_PRM_COMMIT:    /* a PrmCommit starts LldpSend */
                LldpMode = OHA_RESET_SEND;
                break;
            case OHA_OPC_PORT_CONTROL:
                if (pRQBUser->args.PortControl.Mode == OHA_LLDP_SEND_ON)
                {
                    LldpMode = OHA_START_SEND;
                    break;
                }
                if (pRQBUser->args.PortControl.Mode == OHA_LLDP_SEND_OFF)
                {
                    LldpMode = OHA_STOP_SEND;
                    break;
                }

                LldpMode = 0;   /* why ? */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_LldpRequestHandler(): Invalid PortControl-Mode (%Xh)",pRQBUser->args.PortControl.Mode);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
            default:    /* invalid user rquest */
                LldpMode = 0;
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_LldpRequestHandler(): Invalid user request (%Xh)",OHA_RQB_GET_OPCODE(pRQBUser));
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }
    else
    {
        LldpMode = 0;
    }

    /* -----------------------------------------------------------------------*/
    /* forward in statemachine till an asynchroneous request is underway which*/
    /* we have to wait for.                                                   */
    /* -----------------------------------------------------------------------*/
    while (! Ready)
    {
        Ready = LSA_TRUE;      /* Default: break loop because request underway */

        switch (pHDBLldp->RequestState)
        {
            case OHA_LLDP_STATE_STOP:
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-SM [%X]: STATE_STOPPED (Resp: %Xh)",pHDBLldp->ThisHandle,Response);

                /* comes from the user ? */
                if (pRQBUser && OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PORT_CONTROL)
                {
                    /* if LLDP send is not active (no PrmCommit is done) there is no further action, o.k. */
                    OHA_LLDPUserCheckPending(pHDBLldp, OHA_OK);
                    break;
                }

                /* stopped state and a User-STOP => no action */
                if (pRQBUser && (LldpMode == OHA_STOP_SEND))
                {
                    /* check for pending user-requests and confirm  */
                    OHA_LLDPUserCheckPending(pHDBLldp, OHA_OK);
                    break;
                }
                /* User-Start => starts with LLDP_START */
                if (pRQBUser && (LldpMode == OHA_START_SEND))
                {
                    LSA_UINT16 RetVal = OHA_LLDPStartAll(pHDBLldp, OHA_HANDLE_TYPE_LLDP);  /* action is LLDP_START */
                    if (RetVal != OHA_OK)
                    {
                        /* check for pending user-requests and confirm  */
                        OHA_LLDPUserCheckPending(pHDBLldp, RetVal);
                    }
                    else
                    {
                        /* no port was to start by StartAll() ? */
                        if (pHDBLldp->StartingPortId == 0)  /* no link is up */
                        {
                            pHDBLldp->RequestState = OHA_LLDP_STATE_START;
                            /* check for pending user-requests and confirm  */
                            OHA_LLDPUserCheckPending(pHDBLldp, RetVal);
                        }
                        else
                        {
                            pHDBLldp->RequestState = OHA_LLDP_STATE_WF_START;
                        }
                    }
                    break;
                }
                /* User-Restart => starts with LLDP_SET's */
                if (OHA_IS_NOT_NULL(pRQBUser) && (LldpMode == OHA_RESET_SEND))
                {
                    pHDBLldp->RequestState = OHA_LLDP_STATE_CLOSE;
                    Ready = LSA_FALSE;     /* continue with the SM */
                    break;
                }
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_LldpRequestHandler(): Invalid RState (%Xh)",pHDBLldp->RequestState);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            case OHA_LLDP_STATE_WF_START:   /* LldpStart is done */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-SM [%X]: STATE_WF_START (Resp: %Xh)",pHDBLldp->ThisHandle,Response);

                if (pRQBUser && OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PORT_CONTROL)
                {
                    /* if LLDP send is not active (no PrmCommit is done) there is no further action, o.k. */
                    OHA_LLDPUserCheckPending(pHDBLldp, OHA_OK);
                    break;
                }

                /* is there an error during LLDP_START ? */
                if (Response != OHA_OK)
                {
                    pHDBLldp->RequestState = OHA_LLDP_STATE_CLOSE;
                }
                else
                {
                    pHDBLldp->RequestState = OHA_LLDP_STATE_START;
                }
                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_LLDPUserCheckPending(pHDBLldp, Response);
                break;

            case OHA_LLDP_STATE_WF_STOP:   /* LldpStop is done */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-SM [%X]: STATE_WF_STOP (Resp: %Xh)",pHDBLldp->ThisHandle,Response);

                if (pRQBUser && OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PORT_CONTROL)
                {
                    /* if LLDP send is not active (no PrmCommit is done) there is no further action, o.k. */
                    OHA_LLDPUserCheckPending(pHDBLldp, OHA_OK);
                    break;
                }

                /* is there an error during LLDP_STOP ? */
                if (Response != OHA_OK)
                {
                    pHDBLldp->RequestState = OHA_LLDP_STATE_START;
                }
                else
                {
                    pHDBLldp->RequestState = OHA_LLDP_STATE_STOP;
                }
                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_LLDPUserCheckPending(pHDBLldp, Response);
                break;

            case OHA_LLDP_STATE_START:
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-SM [%X]: STATE_START (Resp: %Xh)",pHDBLldp->ThisHandle,Response);

                if (pRQBUser && OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PORT_CONTROL)
                {
                    LSA_UINT16 LinkState;
                    LSA_UINT16 ActualSendMode;
                    LSA_UINT16 PortId            = pRQBUser->args.PortControl.PortId;
                    OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
					LSA_UINT32 AdminStatus = 0;

                    OHA_ASSERT(PortId > 0 && PortId <= OHA_CFG_MAX_PORTS);
                    (LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_ADMIN_STATUS, 
															 &AdminStatus, LSA_NULL, LSA_NULL);
                    ActualSendMode = OHA_AdminStatusToSendMode(AdminStatus);
                    LinkState      = OHA_GetLinkStatus(pHDBEdd->LinkStatus[PortId-1].LinkStatus);

                    /* if ther's no change of the send status - no action, o.k. */
                    if (pRQBUser->args.PortControl.Mode == ActualSendMode)
                    {
                        OHA_LLDPUserCheckPending(pHDBLldp, Response);
                        break;
                    }

                    /* only activate send if the link is up ! */
                    if (LldpMode == OHA_START_SEND) /* start send */
                    {
                        if (LinkState == OHA_PORT_LINK_STATE_UP)
                        {
                            Response = OHA_LLDPStart(pHDBLldp, PortId, (LSA_UINT16)AdminStatus, OHA_HANDLE_TYPE_USER);  /* action is PORT_START */
                            if (Response != OHA_OK)
                            {
                                /* confirm to the user */
                                OHA_LLDPUserCheckPending(pHDBLldp, Response);
                            }
                        }
                        else         /* no action, o.k. */
                        {
                            OHA_LLDPUserCheckPending(pHDBLldp, Response);
                            break;
                        }
                    }
                    else if (LldpMode == OHA_STOP_SEND) /* stop send at any rate */
                    {
                        Response = OHA_LLDPStop(pHDBLldp, PortId, OHA_LLDP_SEND_RECV, OHA_HANDLE_TYPE_USER);      /* action is PORT_STOP */
                        if (Response != OHA_OK)
                        {
                            /* confirm to the user */
                            OHA_LLDPUserCheckPending(pHDBLldp, Response);
                        }
                    }
                    else
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_LldpRequestHandler(): Invalid PortControl.Mode (%Xh)",pRQBUser->args.PortControl.Mode);
                        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    }
                    break;
                }

                if (OHA_IS_NOT_NULL(pRQBUser) && (LldpMode == OHA_STOP_SEND))
                {
                    LSA_UINT16 RetVal = OHA_LLDPStop(pHDBLldp, LLDP_PORT_ID_ALL, OHA_LLDP_SEND_RECV, OHA_HANDLE_TYPE_USER);
                    if (RetVal != OHA_OK)
                    {                           /* confirm to the user, if exist */
                        /* check for pending user-requests   */
                        OHA_LLDPUserCheckPending(pHDBLldp, Response);
                    }
                    else
                    {
                        pHDBLldp->RequestState = OHA_LLDP_STATE_WF_STOP;
                    }
                    break;
                }
                /* User-Restart => starts with LLDP_SET's */
                if (OHA_IS_NOT_NULL(pRQBUser) && (LldpMode == OHA_RESET_SEND))
                {
                    /* a LLDP_SET is to do again for all ports */
                    /* counter is SetPendingCnt */
                    /* o.k., let's start the LLDP_SET's */
                    pHDBLldp->PendingPortId = 0;
                    pHDBLldp->RequestState = OHA_LLDP_STATE_RE_START; /* OHA_LLDP_STATE_CLOSE; */ /* re-initialize */
                    Ready = LSA_FALSE;     /* continue with the SM */
                    break;
                }
                /* started state and a User-START => no action */
                if (OHA_IS_NOT_NULL(pRQBUser) && (LldpMode == OHA_START_SEND))
                {
                    /* check for pending user-requests and confirm  */
                    OHA_LLDPUserCheckPending(pHDBLldp, OHA_OK);
                    break;
                }
                /* from OHA_OPC_SET_ADDRESS(NameOfStation), updates the send-data => no action */
                /* if (pRQBUser && (OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_TEST_LLDP_CONTROL))  */
                /* {                                                */
                /*  OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);          */
                /* }                                                */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_LldpRequestHandler(): Invalid RState (%Xh)",pHDBLldp->RequestState);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            case OHA_LLDP_STATE_CLOSE:       /* initial state after OpenChannel */
                /* User-Start => starts with LLDP_SET's (AP01070629, AP01137963) */
                if (OHA_IS_NOT_NULL(pRQBUser) && (LldpMode == OHA_RESET_SEND))
                {
                    /* a LLDP_SET is to do for all ports */
                    /* counter is SetPendingCnt */
                    /* o.k., let's start the LLDP_SET's */
                    pHDBLldp->RequestState = OHA_LLDP_STATE_RE_START; /* initialize */
                }
            case OHA_LLDP_STATE_RE_START:    /* change configuration (umprojektieren) */
                OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,"LLDP-SM [%X:%d]: STATE_CLOSED",pHDBLldp->ThisHandle,Response);

                /* comes from the user ? */
                if (pRQBUser && OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PORT_CONTROL)
                {
                    /* if LLDP send is not active (no PrmCommit is done) there is no further action, o.k. */
                    OHA_LLDPUserCheckPending(pHDBLldp, OHA_OK);
                    break;
                }
                /* closed state and a User-STOP => no action */
                if (OHA_IS_NOT_NULL(pRQBUser) && (LldpMode == OHA_STOP_SEND))
                {
                    /* check for pending user-requests   */
                    OHA_LLDPUserCheckPending(pHDBLldp, OHA_OK);
                    break;
                }

                /* is there an error during LLDP_SET ? */
                if (Response != OHA_OK)
                {                           /* confirm to the user, if exist */
                    /* check for pending user-requests   */
                    OHA_LLDPUserCheckPending(pHDBLldp, Response);
                    break;
                }

                /* CONTROL OF THE PORTS BY PD_PORT_DATA_CHECK (PEER_TO_PEER_BOUNDARY): */
                /* The LLDP agent shall send or not send LLDP frames for this port.    */

                /* Every PrmCommit now is executed with a OHA_RESET_SEND (Configuration in Run). */
                /* First we have to stop those ports, which should not send (anymore). */
                /* Ports, which should send must bet set (again) and started if they are not running before */

                /* To keep it simple:                                    */
                /* All ports, which should send are started at any rate. */
                /* All ports, which should not send are set at any rate. */

                /* a LLDP_SET is to do for all ports */
                /* counter is SetPendingCnt */
                /* o.k., let's continue the LLDP_SET's */
                pHDBLldp->PendingPortId++;
                /* are all SET's already done ? */
                if (pHDBLldp->PendingPortId > pHDBLldp->pDB->PortCnt)
                {
                    /* => must be continued with a LLDP_START or a LLDP_SET */
                    /* SECOND ACTION: LLDP-START (activate lldp send) or LLDP-SET (passivate lldp send) */

                    LSA_UINT16 RetVal = OHA_LLDPStartAll(pHDBLldp, OHA_HANDLE_TYPE_LLDP);  /* next action is LLDP_START */
                    if (RetVal != OHA_OK)
                    {
                        /* check for pending user-requests and confirm  */
                        OHA_LLDPUserCheckPending(pHDBLldp, RetVal);
                    }
                    else
                    {
                        /* no port was to start by StartAll() ? */
                        if (pHDBLldp->StartingPortId == 0)  /* no link is up */
                        {
                            pHDBLldp->RequestState = OHA_LLDP_STATE_START;
                            /* check for pending user-requests and confirm  */
                            OHA_LLDPUserCheckPending(pHDBLldp, RetVal);
                        }
                        else
                        {
                            pHDBLldp->RequestState = OHA_LLDP_STATE_WF_START;
                        }
                    }
                }
                else
                {
                    /* FIRST ACTION: LLDP-SET (activate lldp send) or LLDP-STOP (passivate lldp send) */
                    LSA_UINT16 RetVal;
					LSA_UINT32 AdminStatus = 0;
					LSA_UINT16 SendMode;

                    (LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, pHDBLldp->PendingPortId, OHA_CTRL_ADMIN_STATUS, 
															 &AdminStatus, LSA_NULL, LSA_NULL);

                    SendMode = OHA_AdminStatusToSendMode(AdminStatus);
                    if (SendMode == OHA_LLDP_SEND_ON)   /* activate lldp send */
                    {
                        RetVal = OHA_LLDPSet(pHDBLldp, pHDBLldp->PendingPortId, OHA_HANDLE_TYPE_LLDP);  /* next port (LLDP_OPC_SET) */
                    }
                    else    /* LLDP-STOP (passivate lldp send) */
                    {
                        RetVal = OHA_LLDPStop(pHDBLldp, pHDBLldp->PendingPortId, OHA_LLDP_SEND_RECV, OHA_HANDLE_TYPE_LLDP);   /* action is PORT_STOP */
                    }
                    if (RetVal != OHA_OK)                     /* an error occurred */
                    {
                        /* check for pending user-requests and confirm the user  */
                        OHA_LLDPUserCheckPending(pHDBLldp, RetVal);
                    }
                }
                break;

            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_LldpRequestHandler(): Invalid RState (%Xh)",pHDBLldp->RequestState);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpRequestHandler(), state %Xh", pHDBLldp->RequestState);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpIndicateEvent                       +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE        pHDB           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to LLDP-HDB                             +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpIndicateEvent(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    OHA_HDB_USER_PTR_TYPE        pHDBUser
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIndicateEvent(pHDBLldp: 0x%X, pHDBUser: 0x%X)",
                          pHDBLldp, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* get indication rqb */
    OHA_RQB_REM_BLOCK_BOTTOM(
        pHDBUser->LldpIndReq.pBottom,
        pHDBUser->LldpIndReq.pTop,
        pRQB);

    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb exists ? */
    {
        OHA_UPPER_LLDP_INDICATION_PTR_TYPE  pLldpInd = &pRQB->args.LldpInd;

        pLldpInd->Status    = pHDBLldp->LastInd.Status;

        pLldpInd->PortId    = pHDBLldp->LastInd.PortId;

        OHA_MEMCPY(&pRQB->args.LldpInd.SenderMacAddr,
                   &pHDBLldp->LastInd.SenderMacAddr, OHA_MAC_ADDR_SIZE);

#if 0
        /* write mandatories to Indication-RQB */
        if (pHDBLldp->LastInd.Status && pLldpInd->pLLDPDU)  /* last change could be written to DataBase */
        {                                          /* and a pLLDPDU exists in the IndRQB */
            OHA_LLDP_SENDER_PTR_TYPE pDbSender  = &pHDBLldp->pDB->LldpData[pHDBLldp->LastInd.PortId].RemoteSender[pHDBLldp->LastInd.SenderId];
            OHA_UPPER_LLDPDU_PTR_TYPE pLLDPDU = pLldpInd->pLLDPDU;

            if (OHA_IS_NOT_NULL(pLLDPDU->pNameOfChassis) && OHA_IS_NOT_NULL(pDbSender->LLDPDU.pNameOfChassis) &&
                (pDbSender->LLDPDU.ChassisNameLen <= OHA_MAX_CHASSIS_STRLEN))
            {
                OHA_MEMCPY(pLLDPDU->pNameOfChassis, pDbSender->LLDPDU.pNameOfChassis, pDbSender->LLDPDU.ChassisNameLen);
            }
            pLLDPDU->ChassisNameLen = pDbSender->LLDPDU.ChassisNameLen;
            pLLDPDU->TypeOfChassis = pDbSender->LLDPDU.TypeOfChassis;
            if (OHA_IS_NOT_NULL(pLLDPDU->pNameOfPort) && OHA_IS_NOT_NULL(pDbSender->LLDPDU.pNameOfPort) &&
                (pDbSender->LLDPDU.PortNameLen <= OHA_MAX_PORT_STRLEN))
            {
                OHA_MEMCPY(pLLDPDU->pNameOfPort, pDbSender->LLDPDU.pNameOfPort, pDbSender->LLDPDU.PortNameLen);
            }
            pLLDPDU->PortNameLen = pDbSender->LLDPDU.PortNameLen;
            pLLDPDU->TypeOfPort  = pDbSender->LLDPDU.TypeOfPort;
            pLLDPDU->TimeToLive  = pDbSender->LLDPDU.TimeToLive;
        }
#endif

        pHDBUser->LldpIndReqPending--;
        pHDBUser->pLastLldpInd = LSA_NULL;

        OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        /* if there are no indication-rqb's: indicated by pLastLldpInd    */
        /* the data of the last indication is stored in the Lldp-Channel  */
        pHDBUser->pLastLldpInd = &pHDBLldp->LastInd;
    }
    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDBLldp->Params.pDetail->Params.Lldp.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIndicateEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpIndicateEventAllUsers               +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE        pHDB           +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to LLDP-HDB                             +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE : indicated                            +*/
/*+                         LSA_FALSE: could not indicate                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Indication to all users.                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LldpIndicateEventAllUsers(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    LSA_UINT16                   PortId
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_EDD_PTR_TYPE        pHDBEdd;
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_UINT16                  NICId;

	OHA_ASSERT_NULL_PTR(pHDBLldp);
	NICId = pHDBLldp->Params.pDetail->Params.Lldp.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIndicateEventAllUsers(pHDBLldp: 0x%X, PortId: 0x%X)",
                          pHDBLldp, PortId);

    pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    /* control the port state machine */
    if (OHA_IS_NOT_NULL(pHDBEdd) && OHA_IS_NOT_NULL(pHDBEdd->pHDBUserPrm))
    {
        LSA_UINT16 RetVal = OHA_DoRemotePortState(pHDBEdd->pHDBUserPrm, PortId);

        if (RetVal != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_LldpIndicateEventAllUsers(RetVal: 0x%X). OHA_DoRemotePortState() failed", RetVal);
        }
    }

    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        /* is a DcpSet controlled by the user ? */
        if ( OHA_IS_NOT_NULL(pHDBUser) && pHDBUser->State == OHA_HANDLE_STATE_OPEN) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
	            (LSA_VOID)OHA_LldpIndicateEvent(pHDBLldp, pHDBUser);

				/* check the TOPO and store the indication in the USER-HDB as LastTopoWriteInd */
				/* indicate the peers only ! */
				(LSA_VOID)OHA_TopoIndicatePort(pHDBUser, PortId, LSA_TRUE, LSA_FALSE);
			}
		}

        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIndicateEventAllUsers()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpConfigEventAllUsers                 +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE        pHDB           +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to LLDP-HDB                             +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE : indicated                            +*/
/*+                         LSA_FALSE: could not indicate                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Indication to all users.                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LldpIndicateConfigEventAllUsers(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_UINT16                  NICId;
	LSA_UINT32                  GlobQueryOptions = OHA_GetGlobQueryOptions();

	OHA_ASSERT_NULL_PTR(pHDBLldp);
	NICId = pHDBLldp->Params.pDetail->Params.Lldp.NICId;
	GlobQueryOptions &= Options; /* only the corresponding options */

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIndicateConfigEventAllUsers(pHDBLldp: 0x%X, Options: 0x%X, Orginator: 0x%X)",
                          pHDBLldp, Options, Orginator);

    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        /* is a  controlled by the user ? */
        if ( OHA_IS_NOT_NULL(pHDBUser) && pHDBUser->State == OHA_HANDLE_STATE_OPEN) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
				/* do not indicate the query user again */
				if(!(GlobQueryOptions && LSA_HOST_PTR_ARE_EQUAL(pHDBUser, pHDBLldp->pHDBUserQueryIf)))
				{
		            (LSA_VOID)OHA_LldpIndicateConfigEvent(pHDBUser, Options, Orginator);
				}
			}
		}

        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIndicateConfigEventAllUsers()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpIndicatePortConfigEventAllUsers     +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE        pHDB           +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to LLDP-HDB                             +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE : indicated                            +*/
/*+                         LSA_FALSE: could not indicate                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Indication to all users.                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LldpIndicatePortConfigEventAllUsers(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    LSA_UINT16                   PortId,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_UINT16                  NICId;
    LSA_UINT32                  QueryOptions;

	OHA_ASSERT_NULL_PTR(pHDBLldp);
	OHA_ASSERT_NULL_PTR(PortId >0 && PortId <= OHA_CFG_MAX_PORTS);
	NICId = pHDBLldp->Params.pDetail->Params.Lldp.NICId;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIndicatePortConfigEventAllUsers(pHDBLldp: 0x%X, Options: 0x%X, Orginator: 0x%X)",
                          pHDBLldp, Options, Orginator);

	QueryOptions = OHA_GetPortQueryOptions(NICId, PortId);
	QueryOptions &= Options; /* only the corresponding options */

    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        /* is a  controlled by the user ? */
        if ( OHA_IS_NOT_NULL(pHDBUser) && pHDBUser->State == OHA_HANDLE_STATE_OPEN) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
				/* do not indicate the query user again */
				if(!(QueryOptions && LSA_HOST_PTR_ARE_EQUAL(pHDBUser, pHDBLldp->pHDBUserQueryPort)))
				{
		            (LSA_VOID)OHA_LldpIndicatePortConfigEvent(pHDBUser, PortId, Options, Orginator);
				}
			}
		}

        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIndicatePortConfigEventAllUsers()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestControlSnmpOid                   +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_CONTROL_LLDP/MRP_OID    +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.SnmpOid                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get an Oid entry                                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestControlSnmpOid(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;

    OHA_UPPER_GET_OID_PTR_TYPE pSnmpOid = &pRQB->args.SnmpOid;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestControlSnmpOid(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

	if (!(pSnmpOid->pOidName && pSnmpOid->OidNameLen && pSnmpOid->OidNameLen<=OHA_MAX_OID_LEN))
    {
        Response = OHA_ERR_PARAM;               /* Error: no OID Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_CONTROL_XXX_OID (0x%X) failed. No OID data present", OHA_RQB_GET_OPCODE(pRQB));
    }

    if (Response == OHA_OK)
    {
        /* set exact based on message type  */
        switch (pSnmpOid->SnmpVersion)
        {
            case OHA_SNMP_VERSION_1:
            case OHA_SNMP_VERSION_2:
            case OHA_SNMP_VERSION_3:
                break;
            default:
                Response = OHA_ERR_PARAM;
                break;
        }
    }

    if (Response == OHA_OK)
    {
		LSA_HANDLE_TYPE Agent = (OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_LLDP_CONTROL_OID) ? (LSA_HANDLE_TYPE)OHA_HANDLE_TYPE_LLDP : (LSA_HANDLE_TYPE)OHA_HANDLE_TYPE_MRP;
		OHA_HDB_USER_PTR_TYPE pHDBSnmpUser = OHA_USERIsAgentActive(Agent, LSA_NULL);

        if (pHDBUser->UserReqPendCnt != 0) /* queue the request, if any is active */
        {
            /* because UserReqPendCnt was >0 we dont expect LSA_NULL here */
            OHA_ASSERT_NULL_PTR(pHDBUser->SnmpUserReq.pBottom);
#if 0
            /* queue the user-rqb */
            OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SnmpUserReq.pBottom,
                                  pHDBUser->SnmpUserReq.pTop,
                                  pRQB);

            pHDBUser->UserReqPendCnt++;  /* number of RQBs pending within LLDP */
#endif
            Response = OHA_ERR_SEQUENCE;       /* Error: no queueing of user rqbs */
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_CONTROL_XXX_OID (0x%X) failed. No queueing of user rqbs", OHA_RQB_GET_OPCODE(pRQB));
        }
		else if(OHA_IS_NOT_NULL(pHDBSnmpUser))	/* any other user active ? */
		{
            Response = OHA_ERR_SEQUENCE;       /* Error: no queueing of any other user snmp rqbs */
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_CONTROL_XXX_OID (0x%X) failed. No queueing of other user rqbs", OHA_RQB_GET_OPCODE(pRQB));
		}
		else if(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_LLDP_CONTROL_OID && /* snmp_set, snmp_get or snmp_getnext is active */
				OHA_IS_NOT_NULL(OHA_SOCKIsAgentActive(OHA_HANDLE_TYPE_LLDP)))

		{
            Response = OHA_ERR_SEQUENCE;       /* Error: a snmp telegram is active */
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_CONTROL_LLDP_OID (0x%X) failed. An LLDP agent is active", OHA_RQB_GET_OPCODE(pRQB));
		}
		else if(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_MRP_CONTROL_OID && /* snmp_set, snmp_get or snmp_getnext is active */
				OHA_IS_NOT_NULL(OHA_SOCKIsAgentActive(OHA_HANDLE_TYPE_MRP)))

		{
            Response = OHA_ERR_SEQUENCE;       /* Error: a snmp telegram is active */
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_CONTROL_MRP_OID (0x%X) failed. An MRP agent is active", OHA_RQB_GET_OPCODE(pRQB));
		}
    }

    if (Response == OHA_OK)
    {
        LSA_UINT16  CountOfStats   = 0;	/* LLDP statistics, MRP status */
        LSA_UINT16  CountOfQueries = 0; /* User queries (LLDP-Set)     */
		LSA_UINT32	SnmpErrorCode;

		/* we store the OID, otherwise it's overwritten in case of a GetNext.Req */
		OHA_OID  TempOidName[OHA_MAX_OID_LEN];
		LSA_UINT32 OidNameLen = pSnmpOid->OidNameLen;  /* AP01256836 */
			
		OHA_OIDCPY(&TempOidName, pSnmpOid->pOidName, OidNameLen);

		if(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_LLDP_CONTROL_OID)
		{
	        SnmpErrorCode = OHA_SNMPGetOidElement(pSnmpOid, OHA_LLDP_MIB_NODE);
			if(SnmpErrorCode == OHA_SNMP_ERR_NOERROR)
			{
				LSA_UINT16 RetVal = OHA_OK;
				if(pSnmpOid->MessageType == OHA_SNMP_SET_REQ)
				{
					RetVal = OHA_LLDPPrepareStats(pHDBUser->ThisHandle, &CountOfQueries, LSA_TRUE);
				}
				if (RetVal != OHA_OK) /* no user ind resource(s) for query */
				{
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "OUT: OHA_RequestControlSnmpOid(RetVal 0x%X): User query running or cancelled.", RetVal);
					Response = OHA_ERR_RESOURCE;
					/* create the get response(-) for the set operation */
					pSnmpOid->SnmpError = OHA_SNMP_ERR_NOSUCHNAME;

				    OHA_UserRequestFinish(pHDBUser,pRQB,Response);
				    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestControlSnmpOid(Response: 0x%X)", Response);
					return;
				}
				if (CountOfQueries == 0) /* no user queries, continue */ 
				{
					/* statistic data (asynchronous) from LLDP_GET_STATS required ? */
					CountOfStats = OHA_LLDPDoStats(pHDBUser->ThisHandle, pSnmpOid->MessageType, LSA_TRUE); /* OHA_HANDLE_TYPE_USER */
				}
	
				OHA_ASSERT(CountOfQueries == 0 || CountOfStats == 0);
                if (pHDBUser->StatsPendCnt != 0) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* request is not queued */
				if (CountOfStats)
				{
					pHDBUser->StatsPendCnt = CountOfStats;  /* pending LLDP_OPC_STATS */
				}
				if(CountOfStats || CountOfQueries)
				{
					if(pSnmpOid->MessageType != OHA_SNMP_SET_REQ)
					{
						/* reset the user-rqb, will be called again */
						pSnmpOid->OidNameLen = OidNameLen;
						OHA_OIDCPY(pSnmpOid->pOidName, &TempOidName, OidNameLen);
					}

					/* lock the agent */

					/* queue the user-rqb */
					OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SnmpUserReq.pBottom,
										  pHDBUser->SnmpUserReq.pTop,
										  pRQB);

		            pHDBUser->UserReqPendCnt++;  /* number of RQBs pending within LLDP */

					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestControlSnmpOid-LLDP(Response: 0x%X)", Response);
					return;
				}
			}
		}
		else if(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_MRP_CONTROL_OID)
		{
	        SnmpErrorCode = OHA_SNMPGetOidElement(pSnmpOid, OHA_MRP_MIB_NODE);

			if(SnmpErrorCode == OHA_SNMP_ERR_NOERROR)
			{
#ifndef OHA_CFG_NO_MRP
#ifndef OHA_CFG_NO_MRP_MIB
				/* status data (asynchronous) from MRP_OPC_GET_STATUS required ? */
				CountOfStats = OHA_MRPCountStats(pHDBUser->ThisHandle, pSnmpOid->MessageType); /* OHA_HANDLE_TYPE_USER */
				if (CountOfStats)
				{
					OHA_ASSERT(pHDBUser->StatsPendCnt == 0);
					pHDBUser->StatsPendCnt = CountOfStats;  /* pending MRP_OPC_GET_STATUS */

					/* reset the user-rqb, will be called again */
					pSnmpOid->OidNameLen = OidNameLen;
					OHA_OIDCPY(pSnmpOid->pOidName, &TempOidName, OidNameLen);

					/* queue the user-rqb */
					OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SnmpUserReq.pBottom,
										  pHDBUser->SnmpUserReq.pTop,
										  pRQB);

					pHDBUser->UserReqPendCnt++;  /* number of RQBs pending within MRP */

					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestControlSnmpOid-MRP(Response: 0x%X)", Response);
					return;
				}
#endif
#endif
			}
		}
		else
		{
            Response = OHA_ERR_PARAM;
		}
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestControlSnmpOid(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpStartSend                    +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_COMMIT              +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmControl                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Internal Request to start the LLDP for sending local      +*/
/*+               neighbour-data                                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpStartSend(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              NICId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    LSA_BOOL                Finish = LSA_FALSE; /* dont finish request by default */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpStartSend(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* look for the corresponding NicId */
    NICId    = pHDBUser->Params.pDetail->Params.User.NICId;
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL))
    {
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestLldpStartSend failed. No LLDP-Channel present (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }
    else if (OHA_IS_NULL(pHDBLldp->pDB) || pHDBLldp->pDB->PortCnt == 0)
    {
        /* Error: no ports. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestLldpStartSend failed. No Ports present (NIC-ID: 0x%X)",NICId);
        Finish = LSA_TRUE;
    }
    /* #ifndef OHA_LLDP_SEND_BY_PRM_COMMIT */
    /* else if(pHDBLldp->UserRQBCnt) */ /* no queueing of user-requests in this version */
    /* {                                                                                */
    /*  Finish = LSA_TRUE;                                                              */
    /*      OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestLldpStartSend failed. Already one User-Request active for this NicId (NIC-ID: 0x%X)",pRQB->args.LldpControl.NicId);*/
    /* }                                                                                */
    /* #endif */

    if ( Finish )  /* an error occurred */
    {
        LSA_UINT16  RetVal = LSA_RET_OK;
        /* free internal RQB for LldpSend */
        OHA_FREE_UPPER_RQB_LOCAL(&RetVal, pRQB);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);

        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    else
    {
        OHA_ASSERT_NULL_PTR(pHDBLldp);
        /* queue the request in the LLDP-HDB */
        OHA_RQB_PUT_BLOCK_TOP(pHDBLldp->UserRQBs.pBottom,
                              pHDBLldp->UserRQBs.pTop,
                              pRQB);
        pHDBLldp->UserRQBCnt++;
        pHDBUser->LldpReqPendingCnt++;      /* Number of pend. User-LLDP-requests */

        if ( pHDBLldp->UserRQBCnt == 1) /* no further User-RCTRL-Blocks pending ? */
        {
            OHA_LLDPRequestHandler(pHDBLldp, pRQB, OHA_OK);  /* call the lldp-handler */
        }
    }

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpStartSend()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SNMPUserCheckPending                    +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to USER-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPUserCheckPending(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response)
{
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SNMPUserCheckPending(pHDBUser: 0x%X, Response: 0x%X)",
                          pHDBUser, Response);

    if ( pHDBUser->UserReqPendCnt ) /* User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
		OHA_OID                 MibNode;
		LSA_OPCODE_TYPE         Opcode;

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->SnmpUserReq.pBottom,
                                 pHDBUser->SnmpUserReq.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
		Opcode = OHA_RQB_GET_OPCODE(pRQBUser);

		OHA_ASSERT(pHDBUser->UserReqPendCnt);
        pHDBUser->UserReqPendCnt--;

        switch(Opcode)
		{
			case OHA_OPC_LLDP_CONTROL_OID:
				MibNode = OHA_LLDP_MIB_NODE;
			break;
			case OHA_OPC_MRP_CONTROL_OID:
				MibNode = OHA_MRP_MIB_NODE;
			break;
			case OHA_OPC_PRM_WRITE:
				if(pRQBUser->args.PrmData.record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA ||
				   pRQBUser->args.PrmData.record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
				{
					MibNode = OHA_LLDP_MIB_NODE;
					break;
				}
				else if (pRQBUser->args.PrmData.record_index == OHA_RECORD_INDEX_MULTIPLE_SNMP)	/* AP01219785 */
				{
		            OHA_PrmWriteSNMP(pRQBUser, pHDBUser);	/* SysName,... */
					return;
				}
			default:
				MibNode = 0;
	            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			break;
		}

        /* is no open/close user-channel runnning ? */
        if ( (pHDBUser->State == OHA_HANDLE_STATE_OPEN) &&
             LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) )
        {
			if(Opcode == OHA_OPC_LLDP_CONTROL_OID ||
				Opcode == OHA_OPC_MRP_CONTROL_OID) /* it's a ControlOid */
			{
				OHA_UPPER_GET_OID_PTR_TYPE pSnmpOid = &pRQBUser->args.SnmpOid;
				if(pSnmpOid->MessageType != OHA_SNMP_SET_REQ)
				{
					if(OHA_SNMPGetOidElement(&pRQBUser->args.SnmpOid, MibNode) != OHA_SNMP_ERR_NOERROR) /* finally */
					{
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "OUT: OHA_RequestControlSnmpOid-MRP(snmp err_code: 0x%X)", pRQBUser->args.SnmpOid.SnmpError);
					}
				}
			}

			/****** snmp-set(-) ******/
			if (MibNode == OHA_LLDP_MIB_NODE && oha_snmp_get_lldp_statistics() == 0) /* negative user query (set) */ 
			{
				Response = OHA_OK_CANCEL;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "OUT: OHA_SNMPUserCheckPending(RetVal 0x%X): User query running or cancelled.", Response);

				if(Opcode == OHA_OPC_LLDP_CONTROL_OID ||
					Opcode == OHA_OPC_MRP_CONTROL_OID) /* it's a ControlOid */
				{
					OHA_UPPER_GET_OID_PTR_TYPE pSnmpOid = &pRQBUser->args.SnmpOid;
					/* create the get response(-) for the set operation */
					pSnmpOid->SnmpError = OHA_SNMP_ERR_NOSUCHNAME;
				}
			}
            OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);  /* confirm the user */
        }
        else
        {
            OHA_UserRequestFinish(pHDBUser,pRQBUser,OHA_OK_CANCEL); /* cancel the user request */
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SNMPUserCheckPending()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CancelLldpUser                          +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel handles a LLDP-Request set Flag TRUE +*/
/*+               LLDP-Requests are:                                        +*/
/*+                      local LldpStartSend ... OHA_OPC_PRM_COMMIT         +*/
/*+                      user LldpStartSend  ... OHA_OPC_LLDP_CONROL        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelLldpUser(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish
)
{
    LSA_BOOL                    Finish   = LSA_TRUE;
    LSA_UINT16                  Status   = OHA_OK;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CancelLldpUser(pHDBUser: 0x%X)",
                          pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pFinish);

    if (pHDBUser->LldpReqPendingCnt)
    {
        Finish = LSA_FALSE;
    }

    *pFinish = Finish;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_CancelLldpUser(Status=0x%X)", Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpConfigIndication             +*/
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
/*+     LSA_OPCODE_TYPE         opcode:     OHA_OPC_LLDP_CONFIG_INDICATION  +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.LldpInd                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide Indication Resources                   +*/
/*+                                                                         +*/
/*+               Queues the RQB wthin User-Handlemanagement                +*/
/*+               If this is the first RQB we check if some config ind.     +*/
/*+               pending to be indicated. If so we indicate them, if not   +*/
/*+               we queue the request.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpConfigIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
	LSA_UINT16            NicId = pHDB->Params.pDetail->Params.User.NICId;
	OHA_HDB_LLDP_PTR_TYPE pHDBLldp;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpConfigIndication(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

	pHDBLldp = OHA_LLDPGetHandleByNIC(NicId);

    if (OHA_IS_NULL(pHDBLldp))
    {
        /* Error on getting LLDP channel. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"Request OHA_OPC_LLDP_CONFIG_INDICATION failed. No LLDP channel for NicId (0x%X) existing (anymore)", NicId);

		OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_SEQUENCE);
	    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpConfigIndication()");
		return;
	}

    /* queue the request */
    OHA_RQB_PUT_BLOCK_TOP(pHDB->LldpConfigInd.pBottom,
                          pHDB->LldpConfigInd.pTop,
                          pRQB);

    pHDB->LldpConfigIndPending++;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpConfigIndication()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpConfigResponse               +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_LLDP_CONFIG_RESPONSE    +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.LldpConfig                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to respond the LldpConfig.Indication              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpConfigResponse(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId    = pHDBUser->Params.pDetail->Params.User.NICId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_UPPER_LLDP_CONFIG_PTR_TYPE indication = &pRQB->args.LldpConfig;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpConfigResponse(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    /* Check if we have an LLDP-Channel for this NIC-ID. */
	pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL) ||
         pHDBLldp->QueryIfPendCnt == 0) /* look for outstanding user queries */
    {
        /* no LLDP-Channel opened or no query running */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_CONFIG_RESPONSE failed. No LLDP-Channel opened or query present (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestLldpConfigResponse(Response: 0x%X)", Response);
        return;
    }

    if (indication->Mode != OHA_MODE_NO_SET &&
		indication->Mode != OHA_MODE_SET)
    {
        Response = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_CONFIG_RESPONSE failed. Mode (0x%X) is invalid !", indication->Mode);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestLldpConfigResponse(Response: 0x%X)", Response);
        return;
	}

    if (oha_snmp_get_lldp_statistics() == 0) /* look for (query) access(es) */
	{
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
	}

	OHA_ASSERT(pHDBLldp->QueryIfPendCnt);
	pHDBLldp->QueryIfPendCnt--;

    if (indication->Mode == OHA_MODE_NO_SET)
	{
		/* pStatsInfo->GlobOptions = 0; */	/* for SNMP(-) response, create identical */
		oha_snmp_delete_lldp_statistics();		/* none of the mib object(s) is set ! */
	}

	if(OHA_LLDPCountQueriesPending() == 0)	/* all user queries responded */
	{
		LSA_BOOL IsRema = LSA_FALSE;
		OHA_HDB_USER_PTR_TYPE  pHDBSnmpUser = OHA_USERIsAgentActive(OHA_HANDLE_TYPE_LLDP, &IsRema);
		OHA_HDB_AGNT_PTR_TYPE  pSubAgnt = OHA_SOCKIsAgentActive(OHA_HANDLE_TYPE_LLDP);
		LSA_UINT16 CountOfStats;

		/********************** look for an active Snmp_Set *********************/
        if (OHA_IS_NOT_NULL(pSubAgnt))  /* active */
        {
		    OHA_HDB_SOCK_PTR_TYPE pHDBSock = OHA_SOCKGetHandleGlobal();

			OHA_ASSERT(pSubAgnt->RequestState == OHA_SNMP_STATE_WF_USER);
			OHA_ASSERT_NULL_PTR(pHDBSock);

			/* no more user query, continue */
			CountOfStats = OHA_LLDPDoStats(pHDBSock->ThisHandle, OHA_SNMP_SET_REQ, LSA_TRUE);

            if (pSubAgnt->StatsPendCnt != 0) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* request is not queued */
			if(CountOfStats != 0)	/* no further actions */
			{
				pSubAgnt->StatsPendCnt = CountOfStats;  /* pending LLDP_OPC_STATS */
				pSubAgnt->RequestState = OHA_SNMP_STATE_WF_STATISTICS;
			}
			else
			{
				OHA_SNMPSockCheckPending(pHDBSock, OHA_LLDP_MIB_NODE, Response); /* further actions taken in request handler */
				pSubAgnt->RequestState = OHA_SNMP_STATE_START;
			}
        }
		else
		/****************** look for the user of an active OhaOpcControlLldpOid.Req **************/

		if(OHA_IS_NOT_NULL(pHDBSnmpUser))	/* is there a OHA_OPC_LLDP_CONTROL_OID in the snmp queued ? */
		{
			CountOfStats = OHA_LLDPDoStats(pHDBSnmpUser->ThisHandle, OHA_SNMP_SET_REQ, IsRema); /* OHA_HANDLE_TYPE_USER */

            if (pHDBSnmpUser->StatsPendCnt != 0) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* request is not queued */
			if(CountOfStats != 0)	/* no further actions here */
			{
				pHDBSnmpUser->StatsPendCnt = CountOfStats;  /* pending LLDP_OPC_SET_GLOBALS,... */
			}
			else
			{
				OHA_SNMPUserCheckPending(pHDBSnmpUser, Response); /* further actions taken in request handler */
			}
		}
		else
		{
			Response = OHA_ERR_FAILED;  /* no query to respond */
			OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW, "LldpGetStats.Con: No query active!");
		}
	}

	indication->Options = (LSA_UINT16)oha_snmp_get_lldp_config_options();
	OHA_UserRequestFinish(pHDBUser, pRQB, Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestLldpConfigResponse(Response: 0x%X)", Response);
	return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpQueryConfigEvent                    +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpQueryConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    LSA_UINT32                   Options,
	LSA_BOOL                     MakeRema
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIndicateConfigEvent(pHDBUser: 0x%X, Options: 0x%X, MakeRema: 0x%X)",
                          pHDBUser, Options, MakeRema);

    OHA_ASSERT_NULL_PTR(pHDBUser);
	OHA_ASSERT(Options);

    /* get indication rqb */
    OHA_RQB_REM_BLOCK_BOTTOM(
        pHDBUser->LldpConfigInd.pBottom,
        pHDBUser->LldpConfigInd.pTop,
        pRQB);

    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb exists ? */
    {
        OHA_UPPER_LLDP_CONFIG_PTR_TYPE  pLldpConfigInd = &pRQB->args.LldpConfig;
		OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;

		pLldpConfigInd->Mode    = OHA_EVENT_QUERY_SET;
		pLldpConfigInd->Options = (LSA_UINT16)Options;
		pLldpConfigInd->RemaOptions = (MakeRema) ? (LSA_UINT16)Options : (LSA_UINT16)0;

		pLldpConfigInd->Config.TxInterval = (Options & OHA_CTRL_TX_INTERVAL) ? 
											(LSA_UINT16)pDbLldpConfig->TxHoldMultiplier.TmpSetVal :
											(LSA_UINT16)pDbLldpConfig->TxHoldMultiplier.Value;
		pLldpConfigInd->Config.ReinitDelay = (Options & OHA_CTRL_REINIT_DELAY) ?
											(LSA_UINT16)pDbLldpConfig->LldpReinitDelay.TmpSetVal :
											(LSA_UINT16)pDbLldpConfig->LldpReinitDelay.Value;
		pLldpConfigInd->Config.TxDelay     = (Options & OHA_CTRL_TX_DELAY) ?
											(LSA_UINT16)pDbLldpConfig->LldpTxDelay.TmpSetVal :
											(LSA_UINT16)pDbLldpConfig->LldpTxDelay.Value;
		pLldpConfigInd->Config.TxHoldMultiplier = (Options & OHA_CTRL_TX_HOLD_MULTIPLIER) ?
											(LSA_UINT16)pDbLldpConfig->TxHoldMultiplier.TmpSetVal :
											(LSA_UINT16)pDbLldpConfig->TxHoldMultiplier.Value;
		pLldpConfigInd->Config.NotificationInterval = (Options & OHA_CTRL_NOTIFICATION_INTERVAL) ?
											(LSA_UINT16)pDbLldpConfig->NotificationInterval.TmpSetVal :
											(LSA_UINT16)pDbLldpConfig->NotificationInterval.Value;
        pHDBUser->LldpConfigIndPending--;

        OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDBUser->Params.pDetail->Params.User.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIndicateConfigEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpIndicateConfigEvent                 +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpIndicateConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIndicateConfigEvent(pHDBUser: 0x%X, Options: 0x%X)",
                          pHDBUser, Options);

    OHA_ASSERT_NULL_PTR(pHDBUser);
	OHA_ASSERT(Options);

    /* get indication rqb */
    OHA_RQB_REM_BLOCK_BOTTOM(
        pHDBUser->LldpConfigInd.pBottom,
        pHDBUser->LldpConfigInd.pTop,
        pRQB);

    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb exists ? */
    {
        OHA_UPPER_LLDP_CONFIG_PTR_TYPE  pLldpConfigInd = &pRQB->args.LldpConfig;
		OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;
		LSA_UINT8 Mode;

		switch(OHA_HANDLE_GET_TYPE(Orginator))
		{
			case OHA_HANDLE_TYPE_SOCK:
				Mode = OHA_EVENT_REMOTE_SET;
			break;
			case OHA_HANDLE_TYPE_USER:
				Mode = OHA_EVENT_LOCAL_SET;
			break;
			default:
				Mode = 0;
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			break;
		}

		pLldpConfigInd->Mode        = Mode;
		pLldpConfigInd->Options     = (LSA_UINT16)Options;
		pLldpConfigInd->RemaOptions = (LSA_UINT16)Options; /* an SNMP-Set is remanent ! */

		pLldpConfigInd->Config.TxInterval           = (LSA_UINT16)pDbLldpConfig->TxHoldMultiplier.Value;
		pLldpConfigInd->Config.ReinitDelay          = (LSA_UINT16)pDbLldpConfig->LldpReinitDelay.Value;
		pLldpConfigInd->Config.TxDelay              = (LSA_UINT16)pDbLldpConfig->LldpTxDelay.Value;
		pLldpConfigInd->Config.TxHoldMultiplier     = (LSA_UINT16)pDbLldpConfig->TxHoldMultiplier.Value;
		pLldpConfigInd->Config.NotificationInterval = (LSA_UINT16)pDbLldpConfig->NotificationInterval.Value;
        pHDBUser->LldpConfigIndPending--;

        OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDBUser->Params.pDetail->Params.User.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIndicateConfigEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpQueryPortConfigEvent                +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpQueryPortConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
	LSA_UINT16					 PortId,
    LSA_UINT32                   Options
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;
    OHA_DB_IF_PTR_TYPE          pDbIf  = OHA_DB_GET_INTERFACE(pHDBUser->Params.pDetail->Params.User.NICId);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpQueryPortConfigEvent(pHDBUser: 0x%X, PortId: 0x%X, Options: 0x%X)",
                          pHDBUser, PortId, Options);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pDbIf);
	OHA_ASSERT(Options);
    OHA_ASSERT(PortId > 0 && PortId <= OHA_CFG_MAX_PORTS);

    /* get indication rqb */
    OHA_RQB_REM_BLOCK_BOTTOM(
        pHDBUser->LldpPortConfigInd.pBottom,
        pHDBUser->LldpPortConfigInd.pTop,
        pRQB);

    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb exists ? */
    {
	    OHA_UPPER_LLDP_PORT_CONFIG_PTR_TYPE  pPortConfigInd = &pRQB->args.LldpPortConfig;
		OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;

		pPortConfigInd->Mode            = OHA_EVENT_QUERY_SET;
		pPortConfigInd->PortOptions     = (LSA_UINT16)Options;
		pPortConfigInd->PortID          = PortId;
		pPortConfigInd->PortOptionsRema = (LSA_UINT16)Options; /* an SNMP-Set is remanent ! */

		pPortConfigInd->PortConfig.AdminStatus = (Options & OHA_CTRL_ADMIN_STATUS) ?
												 (LSA_UINT8)pPortConfig->AdminStatus.TmpSetVal :
												 (LSA_UINT8)pPortConfig->AdminStatus.Value;
		pPortConfigInd->PortConfig.NotificationEnable = (Options & OHA_CTRL_NOTIFICATION) ?
													 (LSA_UINT8)pPortConfig->NotificationEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->NotificationEnable.Value;
		pPortConfigInd->PortConfig.TLVsTxEnable = (Options & OHA_CTRL_LLDP_TLVS) ?
													 (LSA_UINT8)pPortConfig->TLVsTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->TLVsTxEnable.Value;
		pPortConfigInd->PortConfig.ManAddrTxEnable = (Options & OHA_CTRL_LLDP_MAN_ADDR) ?
													 (LSA_UINT8)pPortConfig->ManAddrTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->ManAddrTxEnable.Value;

		pPortConfigInd->PortConfig.PnoDelayTxEnable = (Options & OHA_CTRL_PNO_DELAY) ?
													 (LSA_UINT8)pPortConfig->PnoDelayTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->PnoDelayTxEnable.Value;
		pPortConfigInd->PortConfig.PnoPortStatusTxEnable = (Options & OHA_CTRL_PNO_PORT_STATUS) ?
													 (LSA_UINT8)pPortConfig->PnoPortStatusTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->PnoPortStatusTxEnable.Value;
		pPortConfigInd->PortConfig.PnoAliasTxEnable = (Options & OHA_CTRL_PNO_ALIAS) ?
													 (LSA_UINT8)pPortConfig->PnoAliasTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->PnoAliasTxEnable.Value;
		pPortConfigInd->PortConfig.PnoMrpTxEnable = (Options & OHA_CTRL_PNO_MRP) ?
													 (LSA_UINT8)pPortConfig->PnoMrpTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->PnoMrpTxEnable.Value;
		pPortConfigInd->PortConfig.PnoPtcpTxEnable = (Options & OHA_CTRL_PNO_PTCP) ?
													 (LSA_UINT8)pPortConfig->PnoPtcpTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->PnoPtcpTxEnable.Value;

		pPortConfigInd->PortConfig.Dot1VlanIdTxEnable = (Options & OHA_CTRL_DOT1_VLAN_ID) ?
													 (LSA_UINT8)pPortConfig->Dot1VlanIdTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->Dot1VlanIdTxEnable.Value;
		pPortConfigInd->PortConfig.Dot1VlanNameTxEnable = (Options & OHA_CTRL_DOT1_VLAN_NAME) ?
													 (LSA_UINT8)pPortConfig->Dot1VlanNameTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->Dot1VlanNameTxEnable.Value;
		pPortConfigInd->PortConfig.Dot1PortProtoVlanTxEnable = (Options & OHA_CTRL_DOT1_PROTO_VLAN) ?
													 (LSA_UINT8)pPortConfig->Dot1PortProtoVlanTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->Dot1PortProtoVlanTxEnable.Value;
		pPortConfigInd->PortConfig.Dot1ProtocolTxEnable = (Options & OHA_CTRL_DOT1_PROTOCOL) ?
													 (LSA_UINT8)pPortConfig->Dot1ProtocolTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->Dot1ProtocolTxEnable.Value;

		pPortConfigInd->PortConfig.Dot3TLVsTxEnable = (Options & OHA_CTRL_DOT3_TLVS) ?
													 (LSA_UINT8)pPortConfig->Dot3TLVsTxEnable.TmpSetVal :
													 (LSA_UINT8)pPortConfig->Dot3TLVsTxEnable.Value;

        pHDBUser->LldpPortConfigIndPending--;

        OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDBUser->Params.pDetail->Params.User.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpQueryPortConfigEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_LldpIndicatePortConfigEvent             +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a LLDP-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpIndicatePortConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
	LSA_UINT16					 PortId,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;
    OHA_DB_IF_PTR_TYPE          pDbIf  = OHA_DB_GET_INTERFACE(pHDBUser->Params.pDetail->Params.User.NICId);

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIndicatePortConfigEvent(pHDBUser: 0x%X, PortId: 0x%X, Options: 0x%X, Orginator: 0x%X)",
                          pHDBUser, PortId, Options, Orginator);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pDbIf);
	OHA_ASSERT(Options);
    OHA_ASSERT(PortId > 0 && PortId <= OHA_CFG_MAX_PORTS);

    /* get indication rqb */
    OHA_RQB_REM_BLOCK_BOTTOM(
        pHDBUser->LldpPortConfigInd.pBottom,
        pHDBUser->LldpPortConfigInd.pTop,
        pRQB);

    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb exists ? */
    {
	    OHA_UPPER_LLDP_PORT_CONFIG_PTR_TYPE  pPortConfigInd = &pRQB->args.LldpPortConfig;
		OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;
		LSA_UINT8 Mode;

		switch(OHA_HANDLE_GET_TYPE(Orginator))
		{
			case OHA_HANDLE_TYPE_SOCK:
				Mode = OHA_EVENT_REMOTE_SET;
			break;
			case OHA_HANDLE_TYPE_USER:
				Mode = OHA_EVENT_LOCAL_SET;
			break;
			default:
				Mode = 0;
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			break;
		}

		pPortConfigInd->Mode            = Mode;
		pPortConfigInd->PortOptions     = (LSA_UINT16)Options;
		pPortConfigInd->PortID          = PortId;
		pPortConfigInd->PortOptionsRema = (LSA_UINT16)Options; /* an SNMP-Set is remanent ! */

		pPortConfigInd->PortConfig.AdminStatus =  (LSA_UINT8)pPortConfig->AdminStatus.Value;
		pPortConfigInd->PortConfig.NotificationEnable = (LSA_UINT8)pPortConfig->NotificationEnable.Value;
		pPortConfigInd->PortConfig.TLVsTxEnable     = (LSA_UINT8)pPortConfig->TLVsTxEnable.Value;
		pPortConfigInd->PortConfig.ManAddrTxEnable  = (LSA_UINT8)pPortConfig->ManAddrTxEnable.Value;

		pPortConfigInd->PortConfig.PnoDelayTxEnable = (LSA_UINT8)pPortConfig->PnoDelayTxEnable.Value;
		pPortConfigInd->PortConfig.PnoPortStatusTxEnable = (LSA_UINT8)pPortConfig->PnoPortStatusTxEnable.Value;
		pPortConfigInd->PortConfig.PnoAliasTxEnable = (LSA_UINT8)pPortConfig->PnoAliasTxEnable.Value;
		pPortConfigInd->PortConfig.PnoMrpTxEnable   = (LSA_UINT8)pPortConfig->PnoMrpTxEnable.Value;
		pPortConfigInd->PortConfig.PnoPtcpTxEnable  = (LSA_UINT8)pPortConfig->PnoPtcpTxEnable.Value;

		pPortConfigInd->PortConfig.Dot1VlanIdTxEnable   = (LSA_UINT8)pPortConfig->Dot1VlanIdTxEnable.Value;
		pPortConfigInd->PortConfig.Dot1VlanNameTxEnable = (LSA_UINT8)pPortConfig->Dot1VlanNameTxEnable.Value;
		pPortConfigInd->PortConfig.Dot1PortProtoVlanTxEnable = (LSA_UINT8)pPortConfig->Dot1PortProtoVlanTxEnable.Value;
		pPortConfigInd->PortConfig.Dot1ProtocolTxEnable = (LSA_UINT8)pPortConfig->Dot1ProtocolTxEnable.Value;

		pPortConfigInd->PortConfig.Dot3TLVsTxEnable = (LSA_UINT8)pPortConfig->Dot3TLVsTxEnable.Value;

        pHDBUser->LldpPortConfigIndPending--;

        OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDBUser->Params.pDetail->Params.User.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIndicatePortConfigEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpPortConfigIndication         +*/
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
/*+     LSA_OPCODE_TYPE         opcode: OHA_OPC_LLDP_POT_CONFIG_INDICATION  +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.LldpPortInd                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide Indication Resources                   +*/
/*+                                                                         +*/
/*+               Queues the RQB wthin User-Handlemanagement                +*/
/*+               If this is the first RQB we check if some config ind.     +*/
/*+               pending to be indicated. If so we indicate them, if not   +*/
/*+               we queue the request.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpPortConfigIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
	LSA_UINT16            NicId = pHDB->Params.pDetail->Params.User.NICId;
	OHA_HDB_LLDP_PTR_TYPE pHDBLldp;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpPortConfigIndication(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

	pHDBLldp = OHA_LLDPGetHandleByNIC(NicId);

    if (OHA_IS_NULL(pHDBLldp))
    {
        /* Error on getting LLDP channel. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"Request OHA_OPC_LLDP_PORT_CONFIG_INDICATION failed. No LLDP channel for NicId (0x%X) existing (anymore)", NicId);

		OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_SEQUENCE);
	    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpPortConfigIndication()");
		return;
	}

    /* queue the request */
    OHA_RQB_PUT_BLOCK_TOP(pHDB->LldpPortConfigInd.pBottom,
                          pHDB->LldpPortConfigInd.pTop,
                          pRQB);

    pHDB->LldpPortConfigIndPending++;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpPortConfigIndication()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpPortConfigResponse           +*/
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
/*+     LSA_OPCODE_TYPE        opcode:  OHA_OPC_LLDP_PORT_CONFIG_RESPONSE   +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.LldpConfig                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to respond the LldpConfig.Indication              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpPortConfigResponse(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId    = pHDBUser->Params.pDetail->Params.User.NICId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_UPPER_LLDP_PORT_CONFIG_PTR_TYPE indication = &pRQB->args.LldpPortConfig;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpPortConfigResponse(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    /* Check if we have an LLDP-Channel for this NIC-ID. */
	pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL) ||
         pHDBLldp->QueryPortPendCnt == 0) /* look for outstanding user queries */
    {
        /* no LLDP-Channel opened or no query running */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_PORT_CONFIG_RESPONSE failed. No LLDP-Channel opened or query present (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestLldpPortConfigResponse(Response: 0x%X)", Response);
        return;
    }

    if (indication->PortID == 0 || indication->PortID > pHDBLldp->pDB->PortCnt)
    {
        Response = OHA_ERR_PARAM;  /* Portid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_PORT_CONFIG_RESPONSE failed. Portid is wrong (Port-ID: 0x%X)",indication->PortID);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestLldpPortConfigResponse(Response: 0x%X)", Response);
        return;
    }

    if (indication->Mode != OHA_MODE_NO_SET &&
		indication->Mode != OHA_MODE_SET)
    {
        Response = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_PORT_CONFIG_RESPONSE failed. Mode (0x%X) is invalid !", indication->Mode);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestLldpPortConfigResponse(Response: 0x%X)", Response);
        return;
	}

    if (oha_snmp_get_lldp_statistics() == 0) /* look for (query) access(es) */
	{
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
	}

	OHA_ASSERT(pHDBLldp->QueryPortPendCnt);
	pHDBLldp->QueryPortPendCnt--;

    if (indication->Mode == OHA_MODE_NO_SET)
	{
		/* for SNMP(-) response, create identical */
		oha_snmp_delete_lldp_statistics();		/* none of the mib object(s) is set ! */
	}

	if(OHA_LLDPCountQueriesPending() == 0)	/* all user queries responded */
	{
		LSA_BOOL IsRema = LSA_FALSE;
		OHA_HDB_USER_PTR_TYPE  pHDBSnmpUser = OHA_USERIsAgentActive(OHA_HANDLE_TYPE_LLDP, &IsRema);
		OHA_HDB_AGNT_PTR_TYPE  pSubAgnt = OHA_SOCKIsAgentActive(OHA_HANDLE_TYPE_LLDP);
		LSA_UINT16 CountOfStats;

		/********************** look for an active Snmp_Set *********************/
        if (OHA_IS_NOT_NULL(pSubAgnt))  /* active */
        {
		    OHA_HDB_SOCK_PTR_TYPE pHDBSock = OHA_SOCKGetHandleGlobal();

			OHA_ASSERT(pSubAgnt->RequestState == OHA_SNMP_STATE_WF_USER);
			OHA_ASSERT_NULL_PTR(pHDBSock);

			/* no more user query, continue */
			CountOfStats = OHA_LLDPDoStats(pHDBSock->ThisHandle, OHA_SNMP_SET_REQ, LSA_TRUE);

            if (pSubAgnt->StatsPendCnt != 0) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* request is not queued */
			if(CountOfStats != 0)	/* no further actions */
			{
				pSubAgnt->StatsPendCnt = CountOfStats;  /* pending LLDP_OPC_STATS */
				pSubAgnt->RequestState = OHA_SNMP_STATE_WF_STATISTICS;
			}
			else
			{
				OHA_SNMPSockCheckPending(pHDBSock, OHA_LLDP_MIB_NODE, Response); /* further actions taken in request handler */
				pSubAgnt->RequestState = OHA_SNMP_STATE_START;
			}
        }
		else

		/****************** look for the user of an active OhaOpcControlLldpOid.Req **************/
		if(OHA_IS_NOT_NULL(pHDBSnmpUser))	/* is there a OHA_OPC_LLDP_CONTROL_OID in the snmp queued ? */
		{
			CountOfStats = OHA_LLDPDoStats(pHDBSnmpUser->ThisHandle, OHA_SNMP_SET_REQ, IsRema); /* OHA_HANDLE_TYPE_USER */

            if (pHDBSnmpUser->StatsPendCnt != 0) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* request is not queued */
			if(CountOfStats != 0)	/* no further actions here */
			{
				pHDBSnmpUser->StatsPendCnt = CountOfStats;  /* pending LLDP_OPC_SET_GLOBALS,... */
			}
			else
			{
				OHA_SNMPUserCheckPending(pHDBSnmpUser, Response); /* further actions taken in request handler */
			}
		}
		else
		{
			Response = OHA_ERR_FAILED;  /* no query to respond */
			OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW, "LldpGetStats.Con: No query active!");
		}
	}

	indication->PortOptions = oha_snmp_get_lldp_port_config_options(NICId, indication->PortID);
	OHA_UserRequestFinish(pHDBUser, pRQB, Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestLldpPortConfigResponse(Response: 0x%X)", Response);
	return;
}

/*****************************************************************************/
/*  end of file OHA_LLDU.C                                                   */
/*****************************************************************************/

