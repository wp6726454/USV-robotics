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
/*  F i l e               &F: oha_topo.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA TOPO functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  18
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_TOPO */

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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TopoUserCheckPending                    +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser   : Pointer to User-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_TOPOUserCheckPending(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response)
{
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TOPOUserCheckPending(pHDBUser: 0x%X, Response: 0x%X)",
                          pHDBUser, Response);
    if ( pHDBUser->TopoReqPendingCnt ) /* User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->TopoUserReq.pBottom,
                                 pHDBUser->TopoUserReq.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);

        pHDBUser->TopoReqPendingCnt--; /* Number of pend. Req's (TOPO)*/
        OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TOPOUserCheckPending()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TopoRequestHandler                      +*/
/*+                        :    OHA_HDB_USER_PTR_TYPE      pHDB             +*/
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
/*+  Description: Statemaching for TOPO-User-Requests.                      +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_TOPORequestHandler(
    OHA_HDB_USER_PTR_TYPE           pHDBUser,
    LSA_UINT16                      NICId,
    LSA_UINT16                      Response)
{
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
    LSA_OPCODE_TYPE         Opcode;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TOPORequestHandler(pHDBUser: 0x%X, NICId: 0x%X, Response: 0x%X)",
                          pHDBUser, NICId, Response);

    OHA_ASSERT_NULL_PTR(pHDBUser);
	OHA_ASSERT(NICId == pHDBUser->Params.pDetail->Params.User.NICId); /* AP01022057 */

    pRQBUser = pHDBUser->TopoUserReq.pBottom;

    OHA_ASSERT_NULL_PTR(pRQBUser);

    Opcode = OHA_RQB_GET_OPCODE(pRQBUser);

    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    OHA_ASSERT_NULL_PTR(pHDBLldp);

    /* a EDD_SRV_GET_PORT_PARAMS is to do for all ports */
    /* counter is SetPendingCnt */
    /* o.k., let's continue the EDD_SRV_GET_PORT_PARAMS's */
    pHDBUser->PendingPortId++;

    /* are all EDD_SRV_GET_PORT_PARAMS's already done ? */
    if (pHDBUser->PendingPortId > pHDBLldp->pDB->PortCnt)
    {
        /* now EddGetPortParams for all ports is done */
        pHDBUser->PendingPortId = 0;

#if 0
        if (Opcode == OHA_OPC_TOPO_CONTROL)
        {
            OHA_ASSERT(pRQBUser->args.TopoControl.Mode == OHA_REGISTER_TOPO);

            /* check for pending user-requests and confirm  */
            OHA_TOPOUserCheckPending(pHDBUser, Response);
            pHDBUser->TopoState    = OHA_TOPO_STATE_OPEN; /* channel is registered      */
            /* pHDBUser->PrmState  = OHA_PRM_STATE_INIT; */
            pHDBUser->PrmState     = OHA_PRM_STATE_PREPARE;

            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"TOPO-Control Register-Topo successful");
        }
#endif

        if (Opcode == OHA_OPC_PRM_COMMIT)
        {
            OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->TopoUserReq.pBottom,
                                     pHDBUser->TopoUserReq.pTop,
                                     pRQBUser);

            /* because PendRCtrlCnt was > 0 we dont expect LSA_NULL here */
            OHA_ASSERT_NULL_PTR(pRQBUser);
            OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PRM_COMMIT);

            pHDBUser->TopoReqPendingCnt--; /* Number of pend. Req's (TOPO)*/
            OHA_ASSERT(pHDBUser->TopoReqPendingCnt == 0);

            if (Response == OHA_OK)
            {
                /* AP00968621 no SetIp (0.0.0.0) by PrmCommit anymore */
                {
                    /* finally do the PrmCommit */
                    OHA_PrmCommit(pRQBUser, pHDBUser);

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Handle Prm-Commit successful");
                }
            }
            else
            {
                OHA_DoPrmPrepare(pHDBUser);

                /* finally the user-indication */
                OHA_UserRequestFinish(pHDBUser,pRQBUser,OHA_ERR_PRM_CONSISTENCY);

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Write REMA for Prm-Commit failed (Response: 0x%X)!", Response);
            }
        }
#if 0
        else if (Opcode == OHA_OPC_PRM_END)
        {
            /* now EddGetPortParams for PrmEnd is done */

            OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->TopoUserReq.pBottom,
                                     pHDBUser->TopoUserReq.pTop,
                                     pRQBUser);

            /* because PendRCtrlCnt was > 0 we dont expect LSA_NULL here */
            OHA_ASSERT_NULL_PTR(pRQBUser);
            OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PRM_END);

            pHDBUser->TopoReqPendingCnt--; /* Number of pend. Req's (TOPO)*/
            OHA_ASSERT(pHDBUser->TopoReqPendingCnt == 0);

            /* finally do the PrmEnd */
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Handle Prm-End successful");

            pHDBUser->PrmState = OHA_PRM_STATE_END;     /* a PrmEnd is done */
            OHA_UserRequestFinish(pHDBUser,pRQBUser,OHA_OK);
        }
#endif
        else if (Opcode == OHA_OPC_TEST_LLDP_CONTROL)
        {
            /* now EddGetPortParams for LldpControl(Start) is done */

            OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->TopoUserReq.pBottom,
                                     pHDBUser->TopoUserReq.pTop,
                                     pRQBUser);

            /* because PendRCtrlCnt was > 0 we dont expect LSA_NULL here */
            OHA_ASSERT_NULL_PTR(pRQBUser);
            OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_TEST_LLDP_CONTROL);

            pHDBUser->TopoReqPendingCnt--; /* Number of pend. Req's (TOPO)*/
            OHA_ASSERT(pHDBUser->TopoReqPendingCnt == 0);

            /* finally do the LldpControl(Start) */
            /* queue the request in the LLDP-HDB */
            OHA_RQB_PUT_BLOCK_TOP(pHDBLldp->UserRQBs.pBottom,
                                  pHDBLldp->UserRQBs.pTop,
                                  pRQBUser);
            pHDBLldp->UserRQBCnt++;
            pHDBUser->LldpReqPendingCnt++;      /* Number of pend. User-LLDP-requests */
            if ( pHDBLldp->UserRQBCnt == 1) /* no further User-RCTRL-Blocks pending ? */
            {
                OHA_LLDPRequestHandler(pHDBLldp, pRQBUser, OHA_OK);  /* call the lldp-handler */
            }
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Handle LldpControl-Start successful");
        }
        else
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_TOPORequestHandler: Invalid Opcode (0x%X)", Opcode);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }
    else
    {
        LSA_UINT16 RetVal = OHA_EDDGetPortParams(NICId, pHDBUser->PendingPortId,
                                                 (LSA_UINT16)(OHA_RQB_GET_HANDLE(pRQBUser)));  /* next port (LLDP_OPC_SET) */
        if (RetVal != OHA_OK)                     /* an error occurred */
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"EDDGetPortParams failed during Operation (Opcode (0x%X)!", Opcode);
            /* check for pending user-requests (topo)   */
            OHA_TOPOUserCheckPending(pHDBUser, RetVal);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TOPORequestHandler(), state %Xh", pHDBUser->TopoRequestState);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SetLastSpecifier                        +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: sets the last (indicated) alarm specifier                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SetLastSpecifier(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_UINT16              Specifier,
    LSA_UINT16              DiagIndex)
{
    LSA_UINT16      RetVal = OHA_OK;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SetLastSpecifier(pHDBUser: 0x%X, PortId: %u, Specifier: 0x%X, DiagIndex: 0x%X)",
                          pHDBUser, PortId, Specifier, DiagIndex);

    OHA_ASSERT_NULL_PTR(pHDBUser);

    if (PortId > OHA_CFG_MAX_PORTS)  /* pHDB->PortCnt */
    {
        RetVal = OHA_ERR_PARAM;  /* Portid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"OHA_SetLastSpecifier failed. Portid is wrong (PORT-ID: 0x%X)",PortId);
        return(RetVal);
    }

    if (DiagIndex == 0 || DiagIndex > OHA_TOPO_MAX_IND_ENTRIES)
    {
        RetVal = OHA_ERR_PARAM;  /* DiagIndex is wrong */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_SetLastSpecifier: Invalid DiagIndex (0x%X)", DiagIndex);
        return(RetVal);
    }

    /* only ErrorAppears (1) and DisappearsAndOk (2) */
    if (!(Specifier == OHA_TOPO_ERROR_APPEARS || Specifier == OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY))
    {
        RetVal = OHA_ERR_PARAM;  /* DiagIndex is wrong */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_SetLastSpecifier: Invalid Specifier (0x%X)", Specifier);
        return(RetVal);
    }

	if (PortId == 0)	/* interface diagnosis */
	{
		switch (DiagIndex)
		{
			case OHA_DIAG_INTERFACE_GATEWAY:   /* ( 1) gateway mismatch  */
				pHDBUser->TopoLastIndStati[PortId].LastSpecifierInterface_Gateway = Specifier;
				break;
			case OHA_DIAG_INTERFACE_NOS:       /* ( 2) NoS mismatch      */
				pHDBUser->TopoLastIndStati[PortId].LastSpecifierInterface_NoS = Specifier;
				break;
			case OHA_DIAG_INTERFACE_IP:        /* ( 3) IP mismatch       */
				pHDBUser->TopoLastIndStati[PortId].LastSpecifierInterface_IP = Specifier;
				break;
			case OHA_DIAG_INTERFACE_NOD:       /* ( 4) NoD mismatch      */
				pHDBUser->TopoLastIndStati[PortId].LastSpecifierInterface_NoD = Specifier;
				break;
			default:                        /* unknown diagnosis */
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_SetLastSpecifier: Invalid DiagIndex for interface (0x%X)", DiagIndex);
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				break;
		}

		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SetLastSpecifier(RetVal: 0x%X)", RetVal);
		return(RetVal);
	}

    switch (DiagIndex)  /* DiagIndex */
    {
        case OHA_DIAG_LINK:             /* ( 1) link status              */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierLink = Specifier;
            break;
        case OHA_DIAG_MAU:              /* ( 2) MAUType                  */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierMau = Specifier;
            break;
        case OHA_DIAG_DELAY:            /* ( 3) cable delay              */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierDelay = Specifier;
            break;
        case OHA_DIAG_PEER_NONE:        /* ( 4) No Peer Detected         */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_None = Specifier;
            break;
        case OHA_DIAG_PEER_CHASSIS:     /* ( 5) neighbour (ChassisID)    */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_Chassis = Specifier;
            break;
        case OHA_DIAG_PEER_PORT:        /* ( 6) neighbour (PortID)       */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_Port = Specifier;
            break;
        case OHA_DIAG_PEER_MRP_UUID:    /* ( 7) neighbour MRP_DomainUUID */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_MRP_domain = Specifier;
            break;
        case OHA_DIAG_PEER_MAU:         /* ( 8) neighbour MAUType        */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_MAU = Specifier;
            break;
        case OHA_DIAG_PEER_CABLEDELAY:  /* ( 9) Peer CableDelay          */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_CableDelay = Specifier;
            break;
        case OHA_DIAG_PEER_RT_CLASS3:   /* (10) Peer RT_Class3           */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_RT_Class3 = Specifier;
            break;
        case OHA_DIAG_PEER_PTCP:        /* (11) Peer PTCP_Status         */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_PTCP_status = Specifier;
            break;
        case OHA_DIAG_PEER_PREAMBLE:    /* (12) Peer PreambleLength         */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_PreambleLength = Specifier;
            break;
        case OHA_DIAG_PEER_FRAGMENT:    /* (13) Peer Fragmentation          */
            pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_Fragmentation = Specifier;
            break;
        default:                        /* unknown diagnosis */
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_SetLastSpecifier: Invalid DiagIndex for port (0x%X)", DiagIndex);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SetLastSpecifier(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TopoIndicateEvent                       +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE        pHDB           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a TOPO-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicateEvent(
    OHA_LAST_PRM_IND_DATA_PTR_TYPE      pThisInd,
    OHA_HDB_USER_PTR_TYPE               pHDBUser
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TopoIndicateEvent(pThisInd: 0x%X, pHDBUser: 0x%X)",
                          pThisInd, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* if the topocheck is started    */
    if (pHDBUser->TopoState == OHA_TOPO_STATE_START &&
        (!LSA_HOST_PTR_ARE_EQUAL(pThisInd, LSA_NULL)) )
    {
        OHA_ASSERT(pThisInd->PortId <= OHA_CFG_MAX_PORTS);

        /* are there valid diag entries ? */
        if ((pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_MAU-1]) || /* port diag */
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_DELAY-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_CHASSIS-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_PORT-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_MRP_UUID-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_NONE-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_MAU-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_CABLEDELAY-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_LINK-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_RT_CLASS3-1]) ||
            (pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_PTCP-1]) ||
			(pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_PREAMBLE-1]) ||
			(pThisInd->PortId != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_FRAGMENT-1]) ||

            (pThisInd->PortId == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_GATEWAY-1]) || /* If diag */
            (pThisInd->PortId == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_NOS-1]) ||
            (pThisInd->PortId == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_NOD-1]) ||
            (pThisInd->PortId == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_IP-1]))
        {
            /* is this port activated for ind's */
            LSA_UINT16 diag_cnt  = 0;
            LSA_UINT16 ChannelProperties;
            LSA_UINT16 Return;

            /* get indication rqb */
            OHA_RQB_REM_BLOCK_BOTTOM(
                pHDBUser->TopoIndReq[pThisInd->PortId].pBottom,
                pHDBUser->TopoIndReq[pThisInd->PortId].pTop,
                pRQB);

            if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb exists ? */
            {
                OHA_UPPER_PRM_INDICATION_PTR_TYPE  pTopoInd = &pRQB->args.PrmInd;
                LSA_UINT16 i;
                OHA_UPPER_PRM_DIAG_ENTRY_PTR_TYPE pRecordData = pTopoInd->diag_ptr;

                for (i=0; i < OHA_TOPO_MAX_IND_ENTRIES; i++)
                {
                    if (pThisInd->DiagValid[i])
                    {
                        diag_cnt++;
                        if (!LSA_HOST_PTR_ARE_EQUAL(pTopoInd->diag_ptr, LSA_NULL))
                        {
                            OHA_MEMCPY(pRecordData, &pThisInd->DiagData[i], sizeof(OHA_PRM_DIAG_ENTRY_TYPE));
                            pRecordData++;
                        }

                        OHA_GET_DIAG_ERROR(ChannelProperties, pThisInd->DiagData[i].ChannelProperties);
                        OHA_ASSERT(ChannelProperties!=0);   /* only ErrorAppears (1) and DisappearsAndOk (2) */

                        /* !!! set the LastSpecifier by a done confirmation now !!! */
                        Return = OHA_SetLastSpecifier(pHDBUser, pThisInd->PortId, ChannelProperties, (LSA_UINT16)(i+1));
                        if (Return != OHA_OK)
                        {
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_SetLastSpecifier(Ret=0x%X) failed!", Return);
                            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                        }
                    }
                }

                pTopoInd->diag_cnt = diag_cnt;
                /* pTopoInd->edd_port_id = pThisInd->PortId; */ /* IN */
                pHDBUser->TopoIndReqPending--;
                /* pHDBUser->pLastTopoInd = LSA_NULL; */

                /* delete the local indication structure for this user */
                /* delete collected events (in case of no TopoRqbs too) */
                OHA_FreeTopoPortInd(pHDBUser, pThisInd->PortId);

                OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
            }
            else
            {
                RetVal = LSA_RET_ERR_RESOURCE;
                /* if there are no indication-rqb's:           */
                /* collect events (in case of no TopoRqbs too) */
                /* pHDBUser->pLastTopoInd = pThisInd;          */
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TopoIndicateEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TopoIndicateEmptyEvent                  +*/
/*+  Input/Output          :    OHA_HDB_LLDP_PTR_TYPE        pHDB           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a TOPO-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicateEmptyEvent(
    LSA_UINT16                      PortId,
    OHA_HDB_USER_PTR_TYPE           pHDBUser
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TopoIndicateEmptyEvent(PortId: %u, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* if the topocheck is started    */
    if (pHDBUser->TopoState == OHA_TOPO_STATE_START)
    {
        OHA_ASSERT(PortId <= OHA_CFG_MAX_PORTS);

        /* get indication rqb */
        OHA_RQB_REM_BLOCK_BOTTOM(
            pHDBUser->TopoIndReq[PortId].pBottom,
            pHDBUser->TopoIndReq[PortId].pTop,
            pRQB);

        if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb exists ? */
        {
            OHA_UPPER_PRM_INDICATION_PTR_TYPE  pTopoInd = &pRQB->args.PrmInd;

            pTopoInd->diag_cnt = 0; /* no event */
            /* pTopoInd->edd_port_id = PortId; */ /* IN */
            pHDBUser->TopoIndReqPending--;
            /* pHDBUser->pLastTopoInd = LSA_NULL; */

			/* delete the local indication structure for this user */
			OHA_FreeTopoPortInd(pHDBUser, PortId);

            OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
        }
        else
        {
            RetVal = LSA_RET_ERR_RESOURCE;
            /* if there are no indication-rqb's:           */
            /* collect events (in case of no TopoRqbs too) */
            /* pHDBUser->pLastTopoInd = pThisInd;          */
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TopoIndicateEmptyEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsRemoteCheck                           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: check if any "peer" diagnosis is set                      +*/
/*+               LSA_FALSE: no diagnosis is set                            +*/
/*+               LSA_TRUE:  diagnosis is set                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_IsRemoteCheck(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId)
{
    LSA_BOOL        RetVal  = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsRemoteCheck(PortId: %u, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check if there are peer diagnosis expected */
    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];
        LSA_UINT16           NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_HDB_EDD_PTR_TYPE pHDBEdd;

        /* are there peers expected in PDPortDataCheck: [CheckPeers] ? */
        if (OHA_IS_NOT_NULL(pTopoDB))
        {
            if (pTopoDB->NumberOfPeers != 0)    /* NoPeersDetected, PeerStationName, PeerPortName */
            {
                RetVal = LSA_TRUE;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteCheck(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            if (pTopoDB->MAUTypeDifferenceExisting &&       /* PeerMauType */
                (pTopoDB->MAUTypeMode & OHA_MAU_TYPE_DIFFERENCE_CHECK_ON))
            {
                RetVal = LSA_TRUE;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteCheck(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            if (pTopoDB->DelayDifferenceExisting &&         /* PeerCableDelay */
                (pTopoDB->DelayMode & OHA_DELAY_DIFFERENCE_CHECK_ON))
            {
                RetVal = LSA_TRUE;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteCheck(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            /* Ausnahme (AP00430591): "Peer PTCP Mismatch"  */
            /* if (pTopoDB->DelayDifferenceExisting &&      */ /* PeerPtcpStatus */
            /*  (pTopoDB->DelayMode & OHA_SYNC_DIFFERENCE_CHECK_ON)) */
            /* {     */
            /*  RetVal = LSA_TRUE;     */
            /*     OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteCheck(RetVal: 0x%X)", RetVal);     */
            /*     return(RetVal);     */
            /* }     */
        }

#ifndef OHA_CFG_NO_MRP
		{
			OHA_HDB_MRP_PTR_TYPE pHDBMrp = OHA_MRPGetHandleByNIC(NICId);
			if (OHA_IS_NOT_NULL(pHDBMrp))
			{
	            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
				LSA_UINT16 Instance;

				OHA_ASSERT_NULL_PTR(pDB);
				Instance = pDB->LldpData[PortId-1].MrpInstance;

				if ((Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT) && 
					pHDBMrp->MrpStateInd[Instance-1].topoCtrlMode != MRP_USR_ENUM_TOPO_CTRL_MAX)   /* MRP_DomainMismatch, RRT_Status */
				{
					if (pHDBMrp->MrpStateInd[Instance-1].topoCtrlMode == MRP_USR_ENUM_TOPO_CTRL_ADD_CHECK) /* AP00507069 */
					{
						if (PortId == pHDBMrp->MrpStateInd[Instance-1].rPort1 ||	/* is it a ring port ? */
							PortId == pHDBMrp->MrpStateInd[Instance-1].rPort2)		 /* AP01138078 */
						{
							RetVal = LSA_TRUE;
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteCheck(RetVal: 0x%X)", RetVal);
							return(RetVal);
						}
					}
				}
			}
		}
#endif

        pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
        if (OHA_IS_NOT_NULL(pHDBEdd))
        {
            LSA_UINT16 Local_IRTPortStatus = pHDBEdd->LinkStatus[PortId-1].IRTPortStatus;

            if (!(Local_IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF))   /* Peer_RT_Class3 */
                /* && pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_RT_Class3 == OHA_ALARM_SPECIFIER_INITIAL_VALUE)) *//* AP00571466 */
            {
                RetVal = LSA_TRUE;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteCheck(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteCheck(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsRemoteMrpPeer_None                    +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: check if any MRP - "peer" diagnosis is set                +*/
/*+               LSA_FALSE: no diagnosis is to set                         +*/
/*+               LSA_TRUE:  diagnosis is to set                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef OHA_CFG_NO_MRP
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_IsRemoteMrpPeer_None(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId)
{
    LSA_BOOL        RetVal  = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsRemoteMrpPeer_None(PortId: %u, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check if there are peer diagnosis expected */
    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        LSA_UINT16              NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_HDB_MRP_PTR_TYPE    pHDBMrp = OHA_MRPGetHandleByNIC(NICId);
        OHA_DB_IF_PTR_TYPE      pDB     = OHA_DB_GET_INTERFACE(NICId);
		LSA_UINT16              Instance;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);
        OHA_ASSERT_NULL_PTR(pHDBMrp);
		OHA_ASSERT_NULL_PTR(pDB);

		Instance = pDB->LldpData[PortId-1].MrpInstance;

		if (!(Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT))
		{
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteMrpPeer_None(No Instance: 0x%X)", Instance);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_IsRemoteMrpPeer_None(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

        switch (pHDBMrp->MrpStateInd[Instance-1].topoCtrlMode)
        {
            case MRP_USR_ENUM_TOPO_CTRL_ADD_CHECK: /* just build diagnosis */
                break;

                /* special case: a removal of mrp-data leads to disappearing events */
            case MRP_USR_ENUM_TOPO_CTRL_NONE: /* former OHA_MRP_REMOVE */
            case MRP_USR_ENUM_TOPO_CTRL_ADD:   /* do not build diagnosis */
                /* has there ever been a (valid) MRP_domain written ? */
                if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_None != OHA_ALARM_SPECIFIER_INITIAL_VALUE)
                {
                    RetVal = LSA_TRUE;
                }
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteMrpPeer_None(Mode: 0x%X)", pHDBMrp->MrpStateInd[Instance-1].topoCtrlMode);
                break;

            default:    /* no MRP data written */
                OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteMrpPeer_None(Mode: Initial)");
                break;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsRemoteMrpPeer_None(RetVal: 0x%X)", RetVal);
    return(RetVal);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TopoIndicatePort                        +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks the TOPO, prepares the indication in the USER-HDB  +*/
/*+               and indicates a TOPO-Event to one user                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicatePort(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL                CheckPeers,
    LSA_BOOL                CheckLinkStatus
)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TopoIndicatePort(PortId: %u, pHDBUser: 0x%X, CheckPeers: 0x%X, CheckLinkStatus: 0x%X)",
                          PortId, pHDBUser, CheckPeers, CheckLinkStatus);
    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT(CheckPeers | CheckLinkStatus);   /* check Peers and/or Link */

    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "ParamError (PortId): OHA_TopoIndicatePort()");
        RetVal = OHA_ERR_PARAM; /* no valid PortId */
        return(RetVal);
    }

    /* is a topo check for this port started ? */
    if (pHDBUser && pHDBUser->TopoState == OHA_TOPO_STATE_START) /* topo check for this user is started ? */
    {
        /* look for Port-Indications */
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

        /* look for the corresponding NicId */
        LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

        /* is a topo check for this port started ? */
        /* delete the local indication structure for this user if existing */
        /* collect the events in case of no TopoRqbs, call is in OHA_TopoIndicateEvent() */
        /* OHA_FreeTopoPortInd(pHDBUser, (LSA_UINT16)(i+1)); */

        /* indicate already if Topo started and if topo data provided */
        if (!LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL))
        {
            /* (i)  compare the remote peers if existing */
            if (CheckPeers)
            {
                LSA_BOOL        RemoteMatch = LSA_FALSE;

                /* Rule for remote: if a check for a REMOTE diagnosis group is activated, this diag will be  */
                /* exchanged by RemoteMismatch/NoPeerDetected. That means, if the chek of PeerMAUTypMismatch */
                /* or PeerCableDelayMismatch,... is activated, so the check of RemoteMismatch/NoPeerDetected */
                /* is aactivated implicitly. */
                /* If there ocurrs a RemoteMismatch/NoPeerDetected diagnosis (signalized by LLDP, or even    */
                /* by a "hurrying" LinkDown), so this disgnosis will be evaluated as "higher" and only this  */
                /* this diagosis is built.                                   */
                /* Exception of the rule (AP00430591): "Peer PTCP Mismatch"  */
                if (OHA_IsRemoteCheck(pHDBUser, PortId))
                {
                    /* compare if "No Peer Detected" */
                    /* Presuppositions: in the Record PDPortDataCheck exists the Subblock CheckPeers,...     */
                    RetVal = OHA_CompareTopoPeer_None(pHDBUser, PortId, &RemoteMatch);

                    if (RetVal == OHA_OK)
                    {
                        LSA_UINT16  ExtCHannelErrorType = OHA_PEER_NONE_DETECTED;
                        LSA_UINT16  ChannelProperties = (RemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_NONE: %d",PortId,RemoteMatch);

                        /* indicate only the changes */
                        if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_None != ChannelProperties)
                        {
                            (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                                OHA_REMOTE_MISMATCH,
                                                                ExtCHannelErrorType,
                                                                ChannelProperties,
                                                                OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                                OHA_DIAG_PEER_NONE);
                        }
                    }
                }
#ifndef OHA_CFG_NO_MRP
                else if (OHA_IsRemoteMrpPeer_None(pHDBUser, PortId)) /* special case: a removal or only (re)write of mrp-data may lead to a disappearing event */
                {
                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort (Mrp-Special): PortId: %d, OHA_DIAG_PEER_NONE: %d",PortId,RemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_None != OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            OHA_PEER_NONE_DETECTED,
                                                            OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY,
                                                            OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                            OHA_DIAG_PEER_NONE);
                    }
                }
#endif

                /* are there peers expected ? */
                if (pTopoDB->NumberOfPeers != 0)
                {
                    /* (i) compare the peer chassis if existing */
                    /* Peer Chassis ID mismatch: Voraussetzung ist: Im Record PDPortDataCheck existiert der Subblock CheckPeers */
                    RetVal = OHA_CompareTopoPeer_Chassis(pHDBUser, PortId, &RemoteMatch);

                    if (RetVal == OHA_OK)
                    {
                        LSA_UINT16  ExtCHannelErrorType = OHA_PEER_STATION_NAME_MISMATCH;
                        LSA_UINT16  ChannelProperties = (RemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_CHASSIS: %d",PortId,RemoteMatch);

                        /* indicate only the changes */
                        if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_Chassis != ChannelProperties)
                        {
                            (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                                OHA_REMOTE_MISMATCH,
                                                                ExtCHannelErrorType,
                                                                ChannelProperties,
                                                                OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                                OHA_DIAG_PEER_CHASSIS);
                        }
                    }

                    /* (ii) compare the peer port if existing */
                    /* Peer Port ID mismatch: Voraussetzung ist: Im Record PDPortDataCheck existiert der Subblock CheckPeers */
                    RetVal = OHA_CompareTopoPeer_Ports(pHDBUser, PortId, &RemoteMatch);

                    if (RetVal == OHA_OK)
                    {
                        LSA_UINT16  ExtCHannelErrorType = OHA_PEER_PORT_NAME_MISMATCH;
                        LSA_UINT16  ChannelProperties = (RemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_PORT: %d",PortId,RemoteMatch);

                        /* indicate only the changes */
                        if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_Port != ChannelProperties)
                        {
                            (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                                OHA_REMOTE_MISMATCH,
                                                                ExtCHannelErrorType,
                                                                ChannelProperties,
                                                                OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                                OHA_DIAG_PEER_PORT);
                        }
                    }
                }

#ifndef OHA_CFG_NO_MRP
                /* (iii) compare the Peer MRP domain if existing */
                /* Peer MRP-Domain ID mismatch: Voraussetzung ist: Im Record PDInterfaceMrpDataCheck existiert der Eintrag MRP_Check.MRP_DoaminUUID = true */
                RetVal = OHA_CompareTopoPeer_MRP_domain(pHDBUser, PortId, &RemoteMatch);

                if (RetVal == OHA_OK)
                {
                    LSA_UINT16  ExtCHannelErrorType = OHA_PEER_MRP_DOMAIN_MISMATCH;
                    LSA_UINT16  ChannelProperties = (RemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_MRP_UUID: %d",PortId,RemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_MRP_domain != ChannelProperties)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            ExtCHannelErrorType,
                                                            ChannelProperties,
                                                            OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                            OHA_DIAG_PEER_MRP_UUID);
                    }
                }
#endif

                /* (viii) compare the Peer PTCP status if existing */
                /* Peer PTCP-Status mismatch: Voraussetzung ist: Sync/IRDATA-Record (SubdomainID, IRDATAUUID) */
                RetVal = OHA_CompareTopoPeer_PTCP_Status(pHDBUser, PortId, &RemoteMatch);

                if (RetVal == OHA_OK)
                {
                    LSA_UINT16  ExtCHannelErrorType = OHA_PEER_PTCP_MISMATCH;
                    LSA_UINT16  ChannelProperties = (RemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_PTCP: %d",PortId,RemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_PTCP_status != ChannelProperties)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            ExtCHannelErrorType,
                                                            ChannelProperties,
                                                            OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                            OHA_DIAG_PEER_PTCP);
                    }
                }
            }

            /* expected peer delay difference demanded ? */
            if (pTopoDB->DelayDifferenceExisting &&
                (pTopoDB->DelayMode & OHA_DELAY_DIFFERENCE_CHECK_ON))
            {
                LSA_BOOL        LocalOrRemoteMatch = LSA_FALSE;
                LSA_UINT32      ExtCHannelAddValue = 0;

                /* (iv) compare the Peer CableDelay if peers existing */
                RetVal = OHA_CompareTopoPeer_CableDelay(pHDBUser, PortId, &LocalOrRemoteMatch, &ExtCHannelAddValue);

                if (RetVal == OHA_OK)
                {
                    LSA_UINT16  ExtCHannelErrorType = OHA_PEER_CABLEDELAY_MISMATCH;
                    LSA_UINT16  ChannelProperties = (LocalOrRemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_CABLEDELAY: %d",PortId,LocalOrRemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_CableDelay != ChannelProperties)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            ExtCHannelErrorType,
                                                            ChannelProperties,
                                                            ExtCHannelAddValue,
                                                            OHA_DIAG_PEER_CABLEDELAY);
                    }
                }
            }

            /* expected peer mau type difference demanded ? */
            if (pTopoDB->MAUTypeDifferenceExisting &&
                (pTopoDB->MAUTypeMode & OHA_MAU_TYPE_DIFFERENCE_CHECK_ON))
            {
                LSA_BOOL        LocalOrRemoteMatch = LSA_FALSE;

                /* (vi) Peer MAU Type mismatch, ChannelErrorType = "Remote mismatch" */
                /* Peer MAUType mismatch: Check always, if the neighbor supports LLDP_PNIO_SubType = "Port Status" */
                RetVal = OHA_CompareTopoPeer_MAU_Type(pHDBUser, PortId, &LocalOrRemoteMatch);

                if (RetVal == OHA_OK)
                {
                    LSA_UINT16  ExtCHannelErrorType = OHA_PEER_MAUTYPE_MISMATCH;
                    LSA_UINT16  ChannelProperties = (LocalOrRemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_MAU: %d",PortId,LocalOrRemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_MAU != ChannelProperties)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            ExtCHannelErrorType,
                                                            ChannelProperties,
                                                            OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                            OHA_DIAG_PEER_MAU);
                    }
                }
            }

            /* DATA TRANSMISSION IMPOSSIBLE */
            /* (ii) indicate the LinkStatus (in the DB) if changed */
            if (CheckLinkStatus)
            {
                LSA_BOOL        LocalMatch = LSA_FALSE;

                /* Port State mismatch - Link down: Necessary ist the Subblock CheckLinkState in the Record PDPortDataCheck */
                /* Rule for local check: If any check of the LOCAL group is activated, the diagnosis will be exchanged by the LinkUp / LinkDown diag. */
                /* That means, if the MAUType- or the CableDelay-check is activated, also the check for LinkUp / LinkDown is activated implicitly.    */
                /* If a LinkDown occurrs, this diagnosis is validated as "higher" and only this diagnosis will be indicated. */
                if (pTopoDB->LinkStateExisting || pTopoDB->MAUType != 0 || pTopoDB->DelayExisting)
                {
                    OHA_HDB_EDD_PTR_TYPE    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

                    if (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
                    {
                        /* (i) is the link status existing */
                        if (pHDBEdd->LinkStatus[PortId-1].LinkStatus != EDD_LINK_UNKNOWN)
                        {
                            /* write LinkState to the ExtCHannelAddValue */
                            LSA_UINT16  LinkState = OHA_GetLinkStatus(pHDBEdd->LinkStatus[PortId-1].LinkStatus);
                            LSA_UINT16  ExtCHannelErrorType = OHA_PORT_STATE_MISMATCH;
                            LSA_UINT16  ChannelProperties = (LinkState == OHA_PORT_LINK_STATE_UP) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_LINK: %d",PortId,LinkState);

                            /* indicate only the changes */
                            if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierLink != ChannelProperties)
                            {
                                (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                                    OHA_DATA_TRANSMISSION_IMPOSSIBLE,
                                                                    ExtCHannelErrorType,
                                                                    ChannelProperties,
                                                                    OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                                    OHA_DIAG_LINK);
                            }
                        }
                    }
                }

                /* MAUType Mismatch: Necessary is the Subblock CheckMauType in the Record PDPortDataCheck */
                if (pTopoDB->MAUType != 0)
                {
                    /* (iii) compare the local MAUType (in the DB) if existing */
                    RetVal = OHA_CompareTopoMAU(pHDBUser, PortId, &LocalMatch);
                    if (RetVal == OHA_OK)
                    {
                        LSA_UINT16  ExtCHannelErrorType = OHA_MAU_TYPE_MISMATCH;
                        LSA_UINT16  ChannelProperties = (LocalMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_MAU: %d",PortId,LocalMatch);

                        /* indicate only the changes */
                        if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierMau != ChannelProperties)
                        {
                            (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                                OHA_DATA_TRANSMISSION_IMPOSSIBLE,
                                                                ExtCHannelErrorType,
                                                                ChannelProperties,
                                                                OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                                OHA_DIAG_MAU);
                        }
                    }
                }

                /* Line delay mismatch: Necessary is the Subblock CheckPropagationDelay in the Record PDPortDataCheck */
                if (pTopoDB->DelayExisting)
                {
                    /* (iv) compare the Delay if existing */
                    RetVal = OHA_CompareTopoDelay(pHDBUser, PortId, &LocalMatch);
                    if (RetVal == OHA_OK)
                    {
                        LSA_UINT16  ExtCHannelErrorType = OHA_LINE_DELAY_MISMATCH;
                        LSA_UINT16  ChannelProperties = (LocalMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_DELAY: %d",PortId,LocalMatch);

                        /* indicate only the changes */
                        if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierDelay != ChannelProperties)
                        {
                            (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                                OHA_DATA_TRANSMISSION_IMPOSSIBLE,
                                                                ExtCHannelErrorType,
                                                                ChannelProperties,
                                                                OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                                OHA_DIAG_DELAY);
                        }
                    }
                }
            }

            /* (vii) compare the Peer RT-Class3 mismatch if IRT supported */

            /* SPECIAL DIAGNOSIS: Peer RT-Class3 mismatch, ChannelErrorType = "Remote mismatch" */
            /* RT Class 3 Mismatch: Necessary is: In the record IRData there is at minimum one check-option for this port */
            /* check only if local IRT is supported (IRTPortStatus != OHA_PORT_STATUS_CLASS3_OFF) */
            {
                LSA_BOOL        LocalOrRemoteMatch = LSA_FALSE;

                RetVal = OHA_CompareTopoPeer_RT_Class3(pHDBUser, PortId, &LocalOrRemoteMatch);

                if (RetVal == OHA_OK)
                {
                    LSA_UINT16  ExtCHannelErrorType = OHA_PEER_RT_CLASS_3_MISMATCH;
                    LSA_UINT16  ChannelProperties = (LocalOrRemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_DIAG_PEER_RT_CLASS3: %d",PortId,LocalOrRemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_RT_Class3 != ChannelProperties)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            ExtCHannelErrorType,
                                                            ChannelProperties,
                                                            OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                            OHA_DIAG_PEER_RT_CLASS3);
                    }
                }

	            /* (viii) compare the Peer Preamble Length mismatch if IRT supported */
                RetVal = OHA_CompareTopoPeer_PreambleLength(pHDBUser, PortId, &LocalOrRemoteMatch);

                if (RetVal == OHA_OK)
                {
                    LSA_UINT16  ExtCHannelErrorType = OHA_PEER_PREAMBLE_LENGTH_MISMATCH;
                    LSA_UINT16  ChannelProperties = (LocalOrRemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_PEER_PREAMBLE_LENGTH_MISMATCH: %d",PortId,LocalOrRemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_PreambleLength != ChannelProperties)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            ExtCHannelErrorType,
                                                            ChannelProperties,
                                                            OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                            OHA_DIAG_PEER_PREAMBLE);
                    }
                }

	            /* (ix) compare the Peer fragmentation mismatch if IRT supported */
                RetVal = OHA_CompareTopoPeer_Fragmentation(pHDBUser, PortId, &LocalOrRemoteMatch);

                if (RetVal == OHA_OK)
                {
                    LSA_UINT16  ExtCHannelErrorType = OHA_PEER_FRAGMENTATION_MISMATCH;
                    LSA_UINT16  ChannelProperties = (LocalOrRemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

                    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicatePort: PortId: %d, OHA_PEER_FRAGMENTATION_MISMATCH: %d",PortId,LocalOrRemoteMatch);

                    /* indicate only the changes */
                    if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_Fragmentation != ChannelProperties)
                    {
                        (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                            OHA_REMOTE_MISMATCH,
                                                            ExtCHannelErrorType,
                                                            ChannelProperties,
                                                            OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                            OHA_DIAG_PEER_FRAGMENT);
                    }
                }
            }

            if (OHA_IS_NOT_NULL(pHDBUser->pPortTopoInd[PortId]))
            {
                (LSA_VOID)OHA_TopoIndicateEvent(pHDBUser->pPortTopoInd[PortId], pHDBUser);
            }
            else
            {
                /* duty: if a PrmCommit is done, the subcomponent has to indicate the new diag state */
                /* by an indication. If this is an empty one we have to indicate too by diag_cnt = 0 */
                /*  (i) A PrmCommit is done if CheckPeers and CheckLinkStatus are True */
                /* (ii) The indication is empty, if pHDBUser->pPortTopoInd[PortId] is NULL */
                if (CheckPeers && CheckLinkStatus)
                {
					RetVal = OHA_TopoIndicateEmptyEvent(PortId, pHDBUser);
					if (RetVal != LSA_RET_OK)	/* could not confirm the user */
					{
						RetVal = OHA_AllocTopoPortInd(pHDBUser, PortId);
						if (RetVal != LSA_RET_OK)	/* could not alloc */
						{
							OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "Could not allocate for empty PRM diag: PortId: 0x%x", PortId);
						}
					}
                }
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TopoIndicatePort(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TopoIndicateInterface                   +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks the interface TOPO, prepares the indication in the +*/
/*+               USER-HDB and indicates a TOPO-Event to one user           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicateInterface(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL                BuildEmptyEvent)
{
    LSA_UINT16      RetVal  = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TopoIndicateInterface(pHDBUser: 0x%X, BuildEmptyEvent: 0x%X",
                          pHDBUser, BuildEmptyEvent);

    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* is a topo check for this interface started ? */
    if (pHDBUser && pHDBUser->TopoState == OHA_TOPO_STATE_START) /* topo check for this user is started ? */
    {
        /* look for the corresponding NicId */
        LSA_UINT16 NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
        LSA_BOOL   IfMatch = LSA_FALSE;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

		/* (i) compare the interface gateways if existing */
        RetVal = OHA_CompareTopoInterface_Gateway(pHDBUser, &IfMatch);

        if (RetVal == OHA_OK)
        {
			LSA_UINT16  ExtCHannelErrorType = OHA_STANDARD_GATEWAY_MISMATCH;
			LSA_UINT16  ChannelProperties = (IfMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicateInterface: NICId: %d, OHA_DIAG_INTERFACE_GATEWAY: %d",NICId,IfMatch);

            /* indicate only the changes */
            if (pHDBUser->TopoLastIndStati[0].LastSpecifierInterface_Gateway != ChannelProperties)
            {
                (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, 0,
                                                    OHA_MULTIPLE_INTERFACE_MISMATCH,
                                                    ExtCHannelErrorType,
                                                    ChannelProperties,
                                                    0,
                                                    OHA_DIAG_INTERFACE_GATEWAY);
            }
		}

		/* (ii) compare the interface NameOfStation if existing */
        RetVal = OHA_CompareTopoInterface_NoS(pHDBUser, &IfMatch);

        if (RetVal == OHA_OK)
        {
			LSA_UINT16  ExtCHannelErrorType = OHA_NAME_OF_STATION_MISMATCH;
			LSA_UINT16  ChannelProperties = (IfMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicateInterface: NICId: %d, OHA_DIAG_INTERFACE_NOS: %d",NICId,IfMatch);

            /* indicate only the changes */
            if (pHDBUser->TopoLastIndStati[0].LastSpecifierInterface_NoS != ChannelProperties)
            {
                (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, 0,
                                                    OHA_MULTIPLE_INTERFACE_MISMATCH,
                                                    ExtCHannelErrorType,
                                                    ChannelProperties,
                                                    0,
                                                    OHA_DIAG_INTERFACE_NOS);
            }
		}

		/* (iii) compare the interface IP address if existing */
        RetVal = OHA_CompareTopoInterface_IP(pHDBUser, &IfMatch);

        if (RetVal == OHA_OK)
        {
			LSA_UINT16  ExtCHannelErrorType = OHA_IP_ADDRESS_MISMATCH;
			LSA_UINT16  ChannelProperties = (IfMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicateInterface: NICId: %d, OHA_DIAG_INTERFACE_IP: %d",NICId,IfMatch);

            /* indicate only the changes */
            if (pHDBUser->TopoLastIndStati[0].LastSpecifierInterface_IP != ChannelProperties)
            {
                (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, 0,
                                                    OHA_MULTIPLE_INTERFACE_MISMATCH,
                                                    ExtCHannelErrorType,
                                                    ChannelProperties,
                                                    0,
                                                    OHA_DIAG_INTERFACE_IP);
            }
        }

		/* (iv) compare the interface NameOfDevice if existing */
        RetVal = OHA_CompareTopoInterface_NoD(pHDBUser, &IfMatch);

        if (RetVal == OHA_OK)
        {
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
			/* see PNIO draft: Coding of the field ExtChannelAddValue for ChannelErrorType "Multiple intzerface mismatch */
			LSA_UINT32  ExtCHannelAddValue  = (pDB->MultipleIfModeValid & OHA_NAME_OF_DEVICE_MASK) ? 2 : 1; 
			LSA_UINT16  ExtCHannelErrorType = OHA_INTERFACE_MODE_MISMATCH;
			LSA_UINT16  ChannelProperties = (IfMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_TopoIndicateInterface: NICId: %d, OHA_DIAG_INTERFACE_NOD: %d",NICId,IfMatch);

            /* indicate only the changes */
            if (pHDBUser->TopoLastIndStati[0].LastSpecifierInterface_NoD != ChannelProperties)
            {
                (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, 0,
                                                    OHA_MULTIPLE_INTERFACE_MISMATCH,
                                                    ExtCHannelErrorType,
                                                    ChannelProperties,
                                                    ExtCHannelAddValue,
                                                    OHA_DIAG_INTERFACE_NOD);
            }
		}

        if (OHA_IS_NOT_NULL(pHDBUser->pPortTopoInd[0]))
        {
            (LSA_VOID)OHA_TopoIndicateEvent(pHDBUser->pPortTopoInd[0], pHDBUser);
        }
        else
        {
            /* duty: if a PrmCommit is done, the subcomponent has to indicate the new diag state */
            /* by an indication. If this is an empty one we have to indicate too by diag_cnt = 0 */
            if (BuildEmptyEvent)
            {
				RetVal = OHA_TopoIndicateEmptyEvent(0, pHDBUser);
				if (RetVal != LSA_RET_OK)	/* could not confirm the user */
				{
					RetVal = OHA_AllocTopoPortInd(pHDBUser, 0);
					if (RetVal != LSA_RET_OK)	/* could not alloc */
					{
						OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "Could not allocate for empty interface PRM diag: NICId: 0x%x", NICId);
					}
                }
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TopoIndicateInterface(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_None                    +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares if peers exists                                  +*/
/*+               LSA_FALSE: no peers exists                                +*/
/*+               LSA_TRUE:  peers exists                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_None(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_None(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check if there are peers expected */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        /* OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1]; */

        *pCompare = LSA_FALSE;

        /* are there peers expected in PDPortDataCheck: [CheckPeers] ? */
        /* if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB,  LSA_NULL) || pTopoDB->NumberOfPeers == 0) */
        /* {                           */
        /*  RetVal = OHA_ERR_RESOURCE; */
        /* }                           */
        /* else                        */
        {
            LSA_UINT16 CountOfRealPeers;

            /* are there real peers ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(pHDBUser->Params.pDetail->Params.User.NICId, PortId);

            if (CountOfRealPeers != 0)
            {
                *pCompare = LSA_TRUE;
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_None(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_Chassis                 +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the peer (real with expected)                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_Chassis(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_Chassis(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

        *pCompare = LSA_FALSE;

        /* peers expected ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB,  LSA_NULL) || pTopoDB->NumberOfPeers == 0)
        {
            RetVal = OHA_ERR_RESOURCE;
        }
        else
        {
            LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;
            LSA_UINT16 CountOfRealPeers;

            /* how many real peers are there ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

            /* are there peers to check (no peers or link down) ? */
            if (CountOfRealPeers == 0)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                /* it'll be finally reduced to a NoPeerDetected diagnosis */
                *pCompare = LSA_TRUE;
                OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Chassis(NicID: 0x%X, RetVal: 0x%X)", pHDBUser->Params.pDetail->Params.User.NICId, RetVal);
                return(RetVal);
            }

            /* are there peers to check ? */
            if (CountOfRealPeers == pTopoDB->NumberOfPeers)
            {
                LSA_UINT16      i, j;
                LSA_UINT16      PeersFound = 0;

                /* look for the corresponding NicId */
                if (NICId != OHA_INVALID_NIC_ID)
                {
                    /* check TOPO-DB entries */
                    for (i = 0; i < pTopoDB->NumberOfPeers && i < LLDP_MAX_PORT_SENDER; i++)  /* over all expected peers */
                    {
                        OHA_UPPER_PEER_PTR_TYPE pRemotePeer = &pTopoDB->PeerSender[i];

                        if (LSA_HOST_PTR_ARE_EQUAL(pRemotePeer->pPeerChassisID, LSA_NULL) ||
                            pRemotePeer->LengthPeerChassisID == 0)
                        {
                            break;
                        }

                        /* compare with the LLDP-Base (real) */
                        for (j = 0; j < LLDP_MAX_PORT_SENDER; j++)  /* over all real peers */
                        {
                            OHA_COMMON_MEM_U8_PTR_TYPE pVarChassis = LSA_NULL;
                            LSA_UINT16 LenChassis = 0;
                            LSA_UINT8  SubTypeChassis = 0xff;

                            if (LSA_RET_OK == oha_db_read_rem_chassis_name(NICId, PortId, (LSA_UINT16)(j+1),
                                                                           &SubTypeChassis,
                                                                           &pVarChassis, &LenChassis))
                            {
                                if (!(LSA_HOST_PTR_ARE_EQUAL(pVarChassis, LSA_NULL) ||
                                      LenChassis == 0))
                                {
                                    /* now compare length and strings */
                                    if (LenChassis == pRemotePeer->LengthPeerChassisID)
                                    {
                                        LSA_BOOL Found = oha_namecmp(pRemotePeer->pPeerChassisID, pVarChassis, LenChassis, OHA_UPPER_CASES_ALLOWED(NICId));
                                        if (Found)
                                        {
                                            PeersFound++;
                                        }
                                    }
                                }
                            }
                        }
                        /* entries found, same as count of expected peers */
                        if (PeersFound == CountOfRealPeers)
                        {
                            *pCompare = LSA_TRUE;
                        }
                    }
                }
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Chassis(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_Ports                   +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the peer (real with expected)                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_Ports(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_Ports(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

        *pCompare = LSA_FALSE;

        /* peers expected ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB,  LSA_NULL) || pTopoDB->NumberOfPeers == 0)
        {
            RetVal = OHA_ERR_RESOURCE;
        }
        else
        {
            LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;
            LSA_UINT16 CountOfRealPeers;

            /* how many real peers are there ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

            /* are there peers to check (no peers or link down) ? */
            if (CountOfRealPeers == 0)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                /* it'll be finally reduced to a NoPeerDetected diagnosis */
                *pCompare = LSA_TRUE;
                OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Ports(NICId: 0x%X, RetVal: 0x%X)", NICId, RetVal);
                return(RetVal);
            }

            if (CountOfRealPeers == pTopoDB->NumberOfPeers)
            {
                LSA_UINT16      i, j;
                LSA_UINT16      PeersFound = 0;

                /* look for the corresponding NicId */
                if (NICId != OHA_INVALID_NIC_ID)
                {
                    /* check TOPO-DB entries */
                    for (i = 0; i < pTopoDB->NumberOfPeers && i < LLDP_MAX_PORT_SENDER; i++)  /* over all expected peers */
                    {
                        OHA_UPPER_PEER_PTR_TYPE pRemotePeer = &pTopoDB->PeerSender[i];

                        if (LSA_HOST_PTR_ARE_EQUAL(pRemotePeer->pPeerPortID, LSA_NULL)    ||
                            pRemotePeer->LengthPeerPortID == 0)
                        {
                            break;
                        }

                        /* compare with the LLDP-Base (real) */
                        for (j = 0; j < LLDP_MAX_PORT_SENDER; j++)  /* over all real peers */
                        {
                            OHA_COMMON_MEM_U8_PTR_TYPE pVarPort = LSA_NULL;
                            LSA_UINT16 LenPort = 0;

                            if (LSA_RET_OK == oha_db_read_rem_port_name(NICId, PortId, (LSA_UINT16)(j+1),
                                                                           LSA_NULL,
                                                                           &pVarPort, &LenPort))
                            {
                                if (!(LSA_HOST_PTR_ARE_EQUAL(pVarPort, LSA_NULL)    ||
                                      LenPort == 0))
                                {
                                    /* now compare length and strings */
                                    if (LenPort == pRemotePeer->LengthPeerPortID)
                                    {
                                        LSA_BOOL Found = oha_namecmp(pRemotePeer->pPeerPortID, pVarPort, LenPort, OHA_UPPER_CASES_ALLOWED(NICId));
                                        if (Found)
                                        {
                                            PeersFound++;
                                        }
                                    }
                                }
                            }
                        }
                        /* entries found, same as count of expected peers */
                        if (PeersFound == CountOfRealPeers)
                        {
                            *pCompare = LSA_TRUE;
                        }
                    }
                }
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Ports(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_MRP_domain              +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the peer (real with expected)                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef OHA_CFG_NO_MRP
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_MRP_domain(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_MRP_domain(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        LSA_UINT16              NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_HDB_MRP_PTR_TYPE    pHDBMrp = OHA_MRPGetHandleByNIC(NICId);
        OHA_DB_IF_PTR_TYPE      pDB     = OHA_DB_GET_INTERFACE(NICId);

        LSA_UINT16              CountOfRealPeers;
		LSA_UINT16              Instance;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);
        OHA_ASSERT_NULL_PTR(pHDBMrp);
		OHA_ASSERT_NULL_PTR(pDB);

        *pCompare = LSA_FALSE;

		Instance = pDB->LldpData[PortId-1].MrpInstance;

		if (!(Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT))
		{
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MRP_domain(No Instance: 0x%X)", Instance);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MRP_domain(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

        switch (pHDBMrp->MrpStateInd[Instance-1].topoCtrlMode)
        {
            case MRP_USR_ENUM_TOPO_CTRL_ADD_CHECK: /* build diagnosis */
                break;

                /* special case: a removal of mrp-data leads to disappearing events */
            case MRP_USR_ENUM_TOPO_CTRL_NONE: /* former OHA_MRP_REMOVE */
            case MRP_USR_ENUM_TOPO_CTRL_ADD:   /* do not build diagnosis */
                /* has there ever been a (valid) MRP_domain written ? */
                if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_MRP_domain != OHA_ALARM_SPECIFIER_INITIAL_VALUE)
                {
                    *pCompare = LSA_TRUE;
                }
                else    /* never a (valid) MRP_domain written */
                {
                    RetVal = OHA_ERR_RESOURCE;
                }
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MRP_domain(Mode: 0x%X)", pHDBMrp->MrpStateInd[Instance-1].topoCtrlMode);
                return(RetVal);

            default:    /* no MRP data written */
                OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MRP_domain(Mode: OHA_MRP_REMOVE)");
                RetVal = OHA_ERR_RESOURCE;
                return(RetVal);
        }

        /* TFS 1148233, 1148237: No "Peer MRP domain mismatch" alarm is sent (if MRP_DomainUUID is ffffffff-ffff-ffff-ffff-ffffffffffff) */

        /* how many real peers are there ? */
        CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

        /* are there peers to check (no peers or link down) ? */
        if (CountOfRealPeers == 0)
        {
            /* an existing diagnosis object must DISAPPEAR -> return TRUE */
            /* it'll be finally reduced to a NoPeerDetected diagnosis */
            *pCompare = LSA_TRUE;

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MRP_domain(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* is it a ring port ? */
        if (PortId != pHDBMrp->MrpStateInd[Instance-1].rPort1 &&      /* AP0113878 */
			PortId != pHDBMrp->MrpStateInd[Instance-1].rPort2)
        {
            /* an existing diagnosis object must DISAPPEAR -> return TRUE */
            *pCompare = LSA_TRUE;

            OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MRP_domain(NICId: 0x%X, RetVal: 0x%X)", NICId, RetVal);
            return(RetVal);
        }

        {
            OHA_DB_IF_PTR_TYPE  pDB     = OHA_DB_GET_INTERFACE(NICId);
            LSA_UINT16          SenderId;

            OHA_ASSERT_NULL_PTR(pDB);

            SenderId = pDB->LldpData[PortId-1].LldpSenderId;

            /* if there's no sender, SenderId is "0" */
            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                LSA_UINT8  SenderStatus = pDB->LldpData[PortId-1].LldpSenderStatus;

                /* is there a valid sender ? */
                if (SenderStatus == OHA_STATUS_DATA_VALID ||
                    SenderStatus == OHA_STATUS_DATA_INVALID)
                {
                    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                    LSA_UINT16 Len = 0;
                    LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP;

                    (LSA_VOID)oha_db_read_rem_lldp_element(pDB->NicId, PortId, SenderId,
                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           &pVar, &Len);

                    if (OHA_IS_NOT_NULL(pVar) && (Len == OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE))
                    {
                        /* now compare length and strings */
                        LSA_BOOL Found = OHA_MEMCMP(pHDBMrp->MrpStateInd[Instance-1].domainUUID, pVar, OHA_UUID_SIZE);
                        if (Found)
                        {
                            *pCompare = LSA_TRUE; /* -> disappearing event */
                        }
                    }
                }
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MRP_domain(RetVal: 0x%X)", RetVal);
    return(RetVal);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_PTCP_Status             +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the peer (real with expected)                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_PTCP_Status(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_PTCP_Status(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        LSA_UINT16 CountOfRealPeers;
        OHA_DB_TOPO_PTR_TYPE    pTopoDB;
        LSA_UINT16              NICId   = pHDBUser->Params.pDetail->Params.User.NICId;

        OHA_COMMON_MEM_U8_PTR_TYPE pLocVar = LSA_NULL;
        LSA_UINT16 LocLen  = 0;
        LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS;
        LSA_BOOL IsWireless;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

        IsWireless = OHA_IsWirelessPort(NICId, PortId);
        pTopoDB = pHDBUser->pTopoDataBase[PortId-1];
        OHA_ASSERT_NULL_PTR(pTopoDB);

        /* we have to delay an appearing diagnosis, if there's a time to wait for */
        if (pHDBUser->PtcpTimerInUse[PortId-1])
        {
            RetVal = OHA_ERR_SEQUENCE;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* expected sync difference diagnosis demanded ? */
        /* (i) in the record PDPortDataCheck the Subblock CheckSyncMode the check must be "ON"! */
        if (!(pTopoDB->DelayDifferenceExisting &&
              (pTopoDB->DelayMode & OHA_SYNC_DIFFERENCE_CHECK_ON)))
        {
            RetVal = OHA_ERR_PARAM;
            OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "PtcpStatus diag not activated");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* read the actual LLDP-Tlv LLDP_PNIO_PTCPSTATUS */
        (LSA_VOID)oha_db_read_loc_lldp_element(NICId, PortId,
                                               OHA_LLDP_TYPE_ORG_SPECIFIC,
                                               &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                               &pLocVar, &LocLen);

        /* has there ever been a (valid) PTCP_Status written ? */
        if (OHA_IS_NULL(pLocVar) || LocLen != OHA_LLDP_PNO_PTCP_STATUS_SIZE)
        {
            /* special case: a removal of ptcp-data leads to disappearing events */
            if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_PTCP_status != 0)
            {
                *pCompare = LSA_TRUE;
            }
            else    /* never a (valid) local PTCP_Status written */
            {
                RetVal = OHA_ERR_RESOURCE;
            }
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        *pCompare = LSA_FALSE;

        /* how many real peers are there ? */
        CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

        /* are there peers to check (no peers or link down) ? */
        if (CountOfRealPeers == 0)
        {
            /* an existing diagnosis object must DISAPPEAR -> return TRUE */
            /* it'll be finally reduced to a NoPeerDetected diagnosis     */
            *pCompare = LSA_TRUE;

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* a wireless port always has a boundary */
        if (IsWireless)
        {
            /* an existing diagnosis object must DISAPPEAR -> return TRUE */
            /* it'll be finally reduced to a NoPeerDetected diagnosis     */
            *pCompare = LSA_TRUE;

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "Peer_PTCP_Status: port (0x%X) is wireless - no ptcp diagnosis", PortId);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        {
            /* look for the corresponding NicId */
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            LSA_UINT16 SenderId;

            OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);
            OHA_ASSERT_NULL_PTR(pDB);

            SenderId = pDB->LldpData[PortId-1].LldpSenderId;

            /* if there's no sender, SenderId in "0" */
            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                LSA_UINT8  SenderStatus = pDB->LldpData[PortId-1].LldpSenderStatus;

                /* is there a valid sender ? */
                if (SenderStatus == OHA_STATUS_DATA_VALID ||
                    SenderStatus == OHA_STATUS_DATA_INVALID)
                {
                    OHA_COMMON_MEM_U8_PTR_TYPE pRemVar = LSA_NULL;
                    LSA_UINT16 RemLen = 0;

                    SubType = OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS;

                    (LSA_VOID)oha_db_read_rem_lldp_element(pDB->NicId, PortId, SenderId,
                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           &pRemVar, &RemLen);

                    if (OHA_IS_NOT_NULL(pRemVar) && (RemLen == OHA_LLDP_PNO_PTCP_STATUS_SIZE))
                    {
                        /* now we are able to compare the PTCP_Status - compare length and strings  */
                        /* PtcpStatus: PTCP_MasterSourceAddress + PTCP_SubdomainUUID */

                        /* (ii) no SyncBoundary set for this port ? */
                        if (OHA_IS_NULL(pTopoDB) ||     /* no records written (no boundary) - check further params */
                            ((pTopoDB->AdjustTopoData.DomainBoundaryIngress & OHA_SYNC_BOUNDARY_MASK) == OHA_SYNC_BOUNDARY_OFF) ||
                            ((pTopoDB->AdjustTopoData.DomainBoundaryEgress & OHA_SYNC_BOUNDARY_MASK) == OHA_SYNC_BOUNDARY_OFF) ) /* False: do not block the multicast MAC */
                        {
                            /* local and remote PTCP_SubdomainUUID must be identical if there's no SyncBoundary */
                            /* (iii) both ports are placed in the same PTCPSubdomainUUID ? */
                            LSA_BOOL Equal = OHA_MEMCMP(pLocVar + OHA_MAC_ADDR_SIZE,
                                                        pRemVar + OHA_MAC_ADDR_SIZE,
                                                        OHA_UUID_SIZE);
                            if (Equal)
                            {
                                /* IsSync (local and remote PTCP_MasterSourceAddress != 0) ? */
                                LSA_BOOL EqualLocal  = OHA_MEMCMP(pLocVar, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_MAC, OHA_MAC_ADDR_SIZE);
                                LSA_BOOL EqualRemote = OHA_MEMCMP(pRemVar, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_MAC, OHA_MAC_ADDR_SIZE);

                                /* (iv) check, if both ports "knows" their SyncMaster! */
                                /* Only check if two neigbor-ports uses the same PTCPSubDomainUUID */
                                /* and both do know their PTCPSyncMaster (PTCPMasterSourceAddress != 00:00:00:00:00:00). */
                                if (EqualLocal) /* local MasterSourceAddress is Null */
                                {
                                    *pCompare = LSA_TRUE;
                                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "no local sync master (PTCP_MasterSourceAddress = {0})");
                                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
                                    return(RetVal);
                                }
                                if (EqualRemote)    /* remote MasterSourceAddress is Null */
                                {
                                    *pCompare = LSA_TRUE;
                                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "no remote sync master (PTCP_MasterSourceAddress = {0})");
                                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
                                    return(RetVal);
                                }

                                /* check PTCP_MasterSourceAddress between local and remote ? */

                                /* local and remote PTCP_MasterSourceAddress must be identical */
                                /* check: the SyncMaster of both ports must be the same! */
                                Equal = OHA_MEMCMP(pLocVar, pRemVar, OHA_MAC_ADDR_SIZE);
                                if (Equal)
                                {
                                    *pCompare = LSA_TRUE;
                                }
                                else
                                {
                                    /* CreateDiagnosis: 0x8001 (remote mismatch), 0x8008 (peer ptcp mismatch), */
                                    /*                  ERROR (MR = false, MD = false, ERROR)   */

                                    /* This diagnosis only shows a real configuration error, which leads SyncSlaves */
                                    /* to synchronize with different SyncMaster! */
                                    /* The check of the SyncBoundary is recommended, becaus in the future the PTCPSubdomainUUID */
                                    /* will be a CheckSum and not a UUID in the conventional manner! */
                                    *pCompare = LSA_FALSE;
                                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "Non blocking the multicast MAC: PTCP-MasterSourceAddresses are different");
                                }
                            }
                            else
                            {
                                /* If there are two different PTCPSubdomainUUIDs (at two neigbor-ports) without a parametrized */
                                /* SyncBoundary that is no reason for building the diagnosis */
                                /* "Remote mismatch" - "Peer PTCP mismatch" (0x8008). */
                                OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "Non Blocking the multicast MAC: PTCP_SubdomainUUID's are different - no mismatch");
                                *pCompare = LSA_TRUE;
                            }
                        }
                        else    /* if there's SyncBoundary (block the multicast MAC) - no further check */
                        {
                            /* Also a parametrized SyncBoundary can separate two IRT-boundaries, therfore no check is */
                            /* to activate, if there is a SyncBoundary (SYNC_ID = 0) parametrized for the local port. */
                            *pCompare = LSA_TRUE;
                        }
                    }
                    else    /* no (valid) remote PTCP status TLV sent - no check possible */
                    {
                        *pCompare = LSA_TRUE;   /* an existing diagnosis must disappear */
                    }
                }
                else    /* no (valid) remote PTCP status TLV sent - no check possible */
                {
                    *pCompare = LSA_TRUE;   /* an existing diagnosis must disappear */
                }
            }
            else
            {
                RetVal = OHA_ERR_PARAM;
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PTCP_Status(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_MAU_Type                +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the MAU type (local with remote)                 +*/
/*+                                                                         +*/
/*+  AP01246342: If the local and the remote autonegotiation status are     +*/
/*+              different, the diagnosis is built too.                     +*/
/*+                                                                         +*/
/*+              AUTONEG	AUTONEG	   => OK                                +*/
/*+              AUTONEG	FIX	       => Error                             +*/
/*+              FIX	    FIX	       => OK                                +*/
/*+              FIX	    AUTONEG	   => Error                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_MAU_Type(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_MAU_Type(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];
        LSA_UINT16 Local_MAU_Type;
        OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

        OHA_ASSERT_NULL_PTR(pHDBEdd);

        *pCompare = LSA_FALSE;

        /* is there a valid MAU_Type and an expected Topo ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL) || (!pTopoDB->MAUTypeDifferenceExisting))
        {
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MAU_Type(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* is the Link Up ? */
        {
            LSA_UINT16  CountOfRealPeers;

            Local_MAU_Type = pHDBEdd->LinkStatus[PortId-1].MAUType;

            /* how many real peers are there ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

            /* are there peers to check (no peers or link down) ? */
            if (CountOfRealPeers == 0)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                /* it'll be finally reduced to a NoPeerDetected diagnosis */
                *pCompare = LSA_TRUE;

                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MAU_Type(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }

        {
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            LSA_UINT16 SenderId;

            OHA_ASSERT_NULL_PTR(pDB);

            SenderId = pDB->LldpData[PortId-1].LldpSenderId;

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                LSA_UINT8  SenderStatus = pDB->LldpData[PortId-1].LldpSenderStatus;

                /* is there a valid sender ? */
                if (SenderStatus == OHA_STATUS_DATA_VALID ||
                    SenderStatus == OHA_STATUS_DATA_INVALID)
                {
                    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                    LSA_UINT16 Len = 0;
                    LSA_UINT8 SubType = LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS;

                    (LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId,
                                                           SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
                                                           &pVar, &Len);

                    if (OHA_IS_NOT_NULL(pVar) &&
                        (Len == LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE +
                         LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE +
                         LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_MAUT_TYPE_SIZE))
                    {
                        /* now we are able to compare the MAU_Type */
                        LSA_UINT16 Remote_MAU_Type = OHA_GET16_NTOH (pVar,
                                                                     LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE +
                                                                     LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE);

                        LSA_INT Local_Autoneg_Status  = (pHDBEdd->LinkStatus[PortId-1].SpeedModeConfigured == EDD_LINK_AUTONEG) ?
                                                         LLDP_TLV_MAC_PHY_AUTONEG_ENABLED : 0;

                        LSA_INT Remote_Autoneg_Status = OHA_GET8_NTOH(pVar, 0) & LLDP_TLV_MAC_PHY_AUTONEG_ENABLED;

                        /* AP01246342: only if the (local and remote) MauType are the same */
                        /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                        if (Local_MAU_Type == Remote_MAU_Type)
                        {
                            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, 
                                                  "OUT: OHA_CompareTopoPeer_MAU_Type(LocalAutonegStatus: 0x%X, RemoteAutonegStatus: 0x%X, MediaType: 0x%X)", 
                                                  Local_Autoneg_Status, Remote_Autoneg_Status, pHDBEdd->LinkStatus[PortId-1].MediaType);

                            /* local and the remote autonegotiation status identical ? */
                            if(Local_Autoneg_Status == Remote_Autoneg_Status)
                            {
                                *pCompare = LSA_TRUE;
                            }

                            /* AP01555283: Deactivate diagnosis for the LLDP Autoneg-Bit by POF/FO */
                            if(pHDBEdd->LinkStatus[PortId-1].MediaType == EDD_MEDIATYPE_FIBER_OPTIC_CABLE)
                            {
                                *pCompare = LSA_TRUE;
                            }
                        }
                        else
                        {
                            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, 
                                                  "OUT: OHA_CompareTopoPeer_MAU_Type(Local_MAU_Type: 0x%X, Remote_MAU_Type: 0x%X)", 
                                                  Local_MAU_Type, Remote_MAU_Type);
                        }
                    }
                    else
                    {
                        /* if the MauType is unknown */
                        /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                        *pCompare = LSA_TRUE;
                    }
                }
                else
                {
                    /* if the MauType is unknown */
                    /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                    *pCompare = LSA_TRUE;
                }
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_MAU_Type(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_CableDelay              +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the local cable delay with remote cable delay    +*/
/*+      Is the difference between CableDelayLocal and CableDelayRemote     +*/
/*+      greater than 50 ns, an error wil be generated.                     +*/
/*+                                                                         +*/
/*+      Die Grenze 50ns ergibt sich aus folgender Kalkulation:             +*/
/*+           Auflösung:            10ns                                    +*/
/*+           ERTEC400 Sendejitter: 20ns                                    +*/
/*+           ERTEC200 Sendejitter: 00ns                                    +*/
/*+           PHY Jitter:           10ns                                    +*/
/*+                                                                         +*/
/*+           Auflösung + Sendejitter + 2xPHY = 50ns                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_CableDelay(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare,
    LSA_UINT32          *   pExtCHannelAddValue)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_CableDelay(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    *pExtCHannelAddValue = 0;

    /* check the LineDelay */
    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        /* look for the corresponding NicId */
        LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);

        *pCompare = LSA_FALSE;  /* not o.k. */

        if (NICId == OHA_INVALID_NIC_ID || OHA_IS_NULL(pDB))
        {
            RetVal = OHA_ERR_PARAM;
            OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_CableDelay(NICId: 0x%X, RetVal: 0x%X)", NICId, RetVal);
            return(RetVal);
        }

        /* expected delay existing ? */
        /* is local delay info supported (EDD_GET_PARAM-HwType) ? */
        if  (!OHA_EDD_TIMESTAMP_SUPPORTED(pDB->HardwareType) )
        {
            RetVal = OHA_ERR_PARAM;
            OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_CableDelay(NICId: 0x%X, RetVal: 0x%X)", NICId, RetVal);
            return(RetVal);
        }

        {
            LSA_UINT16 SenderId;
            LSA_UINT16 CountOfRealPeers;

            /* how many real peers are there ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

            /* are there peers to check (no peers or link down) ? */
            /* no peers (anymore) or link down (now) leads to disappearing events */
            if (CountOfRealPeers == 0)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                /* it'll be finally reduced to a NoPeerDetected diagnosis */
                *pCompare = LSA_TRUE;

                OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_CableDelay(NICId: 0x%X, RetVal: 0x%X)", NICId, RetVal);
                return(RetVal);
            }

            SenderId = pDB->LldpData[PortId-1].LldpSenderId;

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
                LSA_UINT8  SenderStatus = pDB->LldpData[PortId-1].LldpSenderStatus;
                LSA_UINT32 Local_CableDelay;

                OHA_ASSERT_NULL_PTR(pHDBEdd);

                Local_CableDelay = pHDBEdd->LinkStatus[PortId-1].CableDelay;

                /* is there a valid sender ? */
                if (SenderStatus == OHA_STATUS_DATA_VALID ||
                    SenderStatus == OHA_STATUS_DATA_INVALID)
                {
                    LSA_UINT32 Remote_CableDelay = 0;
                    LSA_UINT32 CableDelay_Difference;
                    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                    LSA_UINT16 Len = 0;
                    LSA_UINT8 SubType;

                    /* read from LLDP-DB (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
                    SubType  = OHA_LLDP_SUB_TYPE_PNO_DELAY;
                    (LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId, SenderId,
                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           &pVar, &Len);

                    if (pVar && (Len == OHA_LLDP_PNO_DELAY_SIZE))
                    {               /* what the remote says */
                        Remote_CableDelay = OHA_LLDP_GET_U32(pVar+(OHA_LLDP_IDX_CABLEDELAY_LOC * sizeof(LSA_UINT32)));
                    }
                    /* is the remote delay supported ? */
                    if (Remote_CableDelay != 0 &&   /* is the remote delay measured ? */
                        Local_CableDelay != 0)      /* is the local delay measured  ? */
                    {
                        CableDelay_Difference = (Remote_CableDelay > Local_CableDelay) ?
                                                Remote_CableDelay - Local_CableDelay :
                                                Local_CableDelay  - Remote_CableDelay;

                        /* is the difference > 50 ns ? */
                        *pCompare = (CableDelay_Difference > 50) ? LSA_FALSE : LSA_TRUE;
                        *pExtCHannelAddValue = CableDelay_Difference;
                    }
                    else
                    {
                        /* if the delays are not measured */
                        /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                        *pCompare = LSA_TRUE;
                    }
                }
                else
                {
                    /* if the delays cannot be measured */
                    /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                    *pCompare = LSA_TRUE;
                }
            }
            else
            {
                RetVal = OHA_ERR_PARAM;
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_CableDelay(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoDelay                        +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the delay (real with expected)                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoDelay(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoDelay(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the LineDelay */
    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

        *pCompare = LSA_FALSE;

        /* expected delay existing ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL) || (!pTopoDB->DelayExisting))
        {
            RetVal = OHA_ERR_PARAM;
        }
        else
        {
            /* look for the corresponding NicId */
            LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;

            if (NICId != OHA_INVALID_NIC_ID)
            {
                OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

                LSA_UINT16  LinkState = OHA_GetLinkStatus(pHDBEdd->LinkStatus[PortId-1].LinkStatus);

                /* if the Link is down -> return "no MAU can be checked" */
                if (LinkState != OHA_PORT_LINK_STATE_UP)
                {
                    /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                    /* it'll be finally reduced to a LinkDown diagnosis */
                    *pCompare = LSA_TRUE;

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoDelay(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                /* real delay existing ? */
                if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL) &&
                     pHDBEdd->LinkStatus[PortId-1].DelayExisting))
                {
                    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
                    if (!LSA_HOST_PTR_ARE_EQUAL(pDB, LSA_NULL))
                    {
                        /* LineDelay = LineDelay * Resolution */
                        /* EDD-delay has to be less than the User-delay */
                        if (pTopoDB->LineDelayFactor >=
                            (pHDBEdd->LinkStatus[PortId-1].LineDelay * pDB->Resolution))
                        {
                            *pCompare = LSA_TRUE;
                        }
                    }
                }
                else
                {
                    RetVal = OHA_ERR_PARAM;
                }
            }
            else
            {
                RetVal = OHA_ERR_PARAM;
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoDelay(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoSpeed                        +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the speed (real with expected)                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoSpeed(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoSpeed(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the MAUType */
    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

        *pCompare = LSA_FALSE;

        /* expected delay existing ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL) || (!pTopoDB->MAUType))
        {
            RetVal = OHA_ERR_PARAM;
        }
        else
        {
            /* look for the corresponding NicId */
            LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;

            if (NICId != OHA_INVALID_NIC_ID)
            {
                OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

                /* real delay existing ? */
                if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL)) &&
                    pHDBEdd->LinkStatus[PortId-1].Speed != EDD_LINK_UNKNOWN)
                {
                    /* get Speed from the MAUType */
                    if (pHDBEdd->LinkStatus[PortId-1].Speed == OHA_GetSpeedFromMauType(pTopoDB->MAUType))
                    {
                        *pCompare = LSA_TRUE;
                    }
                }
                else
                {
                    RetVal = OHA_ERR_PARAM;
                }
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoSpeed(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoMode                         +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the mode (real with expected)                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoMode(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoMode(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the MAUType */
    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

        *pCompare = LSA_FALSE;

        /* expected delay existing ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL) || (!pTopoDB->MAUType))
        {
            RetVal = OHA_ERR_PARAM;
        }
        else
        {
            /* look for the corresponding NicId */
            LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;

            if (NICId != OHA_INVALID_NIC_ID)
            {
                OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

                /* real delay existing ? */
                if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL)) &&
                    pHDBEdd->LinkStatus[PortId-1].Mode != EDD_LINK_UNKNOWN)
                {
                    /* get Speed from the MAUType */
                    if (pHDBEdd->LinkStatus[PortId-1].Mode == OHA_GetModeFromMauType(pTopoDB->MAUType))
                    {
                        *pCompare = LSA_TRUE;
                    }
                }
                else
                {
                    RetVal = OHA_ERR_PARAM;
                }
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoMode(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrepareTopoIndication                   +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: builds a TOPO-Indication in the User HDB.                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_PrepareTopoIndication(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_UINT16              ChannelErrorType,
    LSA_UINT16              ExtCHannelErrorType,
    LSA_UINT16              ChannelSpecifier,
    LSA_UINT32              ExtCHannelAddValue,
    LSA_UINT16              DiagIndex)
{
    LSA_UINT16      RetVal;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrepareTopoIndication(pHDBUser: 0x%X, PortId: 0x%X, ChannelErrorType: 0x%X, ExtCHannelErrorType: 0x%X, ChannelSpecifier: 0x%X)",
                          pHDBUser, PortId, ChannelErrorType, ExtCHannelErrorType, ChannelSpecifier);

    OHA_ASSERT_NULL_PTR(pHDBUser);

    if (DiagIndex == 0 || DiagIndex > OHA_TOPO_MAX_IND_ENTRIES)
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_PrepareTopoIndication: Invalid DiagIndex (0x%X)", DiagIndex);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
	if(PortId == 0 && DiagIndex > OHA_TOPO_MAX_INTERFACE_IND_ENTRIES)
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_PrepareTopoIndication: Invalid DiagIndex for interface (0x%X)", DiagIndex);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    if (PortId > OHA_CFG_MAX_PORTS)  /* pHDB->PortCnt */
    {
        RetVal = OHA_ERR_PARAM;  /* Portid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"OHA_PrepareTopoIndication failed. Portid is wrong (PORT-ID: 0x%X)",PortId);
        return(RetVal);
    }

    RetVal = OHA_AllocTopoPortInd(pHDBUser, PortId);

    if (RetVal == OHA_OK)
    {
        OHA_LAST_PRM_IND_DATA_PTR_TYPE pLastInd = pHDBUser->pPortTopoInd[PortId];
        OHA_ASSERT_NULL_PTR(pLastInd);

        /* ExtChannelDiagnosisData: add on a diag entry */
        /* ChannelNumber;        */ /* 0x8000 */
        /* ChannelProperties;    */ /* ChannelProperties.Type_Bit0_7 := 0, */
        /* ChannelProperties.Accumulative_Bit8 := 0, */
        /* ChannelProperties.MaintenanceRequired_Bit9  := depending on the component */
        /* ChannelProperties.MaintenanceDemanded_Bit10 := depending on the component */
        /* ChannelProperties.Specifier_Bit11_12 := Appearing(1) or Disappearing(2) */
        /* ChannelProperties.Direction_Bit13_15 =: 0 */
        /* ChannelErrorType;    */ /* 0x8002 */
        /* ExtChannelErrorType; */ /* 0x8000..0x8003 */
        /* ExtChannelAddValue;  */ /* 0 */
        /* as_channel_diag;     only by "disappearing": */
        /* LSA_FALSE: CM-PD Alarm uses the Coding of the ExtChannelDiagnosis */
        /* LSA_TRUE:  CM-PD Alarm uses the (shorter) Coding of the ChannelDiagnosis. */
        /*            In this case ExtChannelErrorType and ExtChannelAddValue are "don't care" (set to 0). */

        pLastInd->DiagData[DiagIndex-1].ChannelNumber       = OHA_TOPO_CHANNEL_NUMBER; /* 0x8000 = Submodule */
        pLastInd->DiagData[DiagIndex-1].ChannelErrorType    = ChannelErrorType;
        pLastInd->DiagData[DiagIndex-1].ExtChannelErrorType = ExtCHannelErrorType;
        pLastInd->DiagData[DiagIndex-1].ExtChannelAddValue  = ExtCHannelAddValue;

        OHA_SET_DIAG_ERROR(pLastInd->DiagData[DiagIndex-1].ChannelProperties, ChannelSpecifier);
 
		/* AP01238084:ChannelErrorType=Multiple interface mismatch (0x800D) --> MaintenanceDemanded */
		if (ChannelErrorType == OHA_MULTIPLE_INTERFACE_MISMATCH)
		{
			/* the qualifier of "Multiple interface mismatch" is ERROR (no bit set) */
			if (ExtCHannelErrorType != OHA_INTERFACE_MODE_MISMATCH)
			{
				pLastInd->DiagData[DiagIndex-1].ChannelProperties |= 0x400;        /* Bit10 */
			}
		}

        pLastInd->DiagValid[DiagIndex-1] = LSA_TRUE;

        /* do not set by building the local diagnosis, set it by successful indication of the diagnosis */
        /* OHA_SetLastSpecifier(pHDBUser, PortId, ChannelSpecifier, DiagIndex); */
    }
    else
    {
        RetVal = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrepareTopoIndication(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IndicateEventAllUsers                   +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE        pHDB            +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to EDD-HDB                              +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE : indicated                            +*/
/*+                         LSA_FALSE: could not indicate                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a TOPO-Indication to all users.                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateEventAllUsers(
    LSA_UINT16                  NicID,
    LSA_UINT16                  PortId
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);
    OHA_HDB_EDD_PTR_TYPE        pHDBEdd;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateEventAllUsers(NicID: 0x%X, PortId:  0x%X)",
                          NicID, PortId);

    pHDBEdd = OHA_EDDGetHandleByNIC(NicID);

    /* control the port state machine */
    if (OHA_IS_NOT_NULL(pHDBEdd) && OHA_IS_NOT_NULL(pHDBEdd->pHDBUserPrm))
    {
        LSA_UINT16 RetVal = OHA_DoRemotePortState(pHDBEdd->pHDBUserPrm, PortId);

        if (RetVal != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_WARN,"OHA_LldpIndicateEventAllUsers(RetVal: 0x%X). OHA_DoRemotePortState() failed", RetVal);
        }
    }


    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        /* is a DcpSet controlled by the user ? */
        if ( OHA_IS_NOT_NULL(pHDBUser) && pHDBUser->State == OHA_HANDLE_STATE_OPEN) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NicID == pHDBUser->Params.pDetail->Params.User.NICId)
			{
				/* check the TOPO and store the indication in the USER-HDB as LastTopoWriteInd */
				/* indicate MAU-Type, delay, link */
				(LSA_VOID)OHA_TopoIndicatePort(pHDBUser, PortId, LSA_TRUE, LSA_FALSE);
			}
		}

        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateEventAllUsers()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_update_mrp_port_status                  +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends the LLDP_PNIO_MRPPORTSTATUS by the following rules: +*/
/* (i) No TLV is sent if no MRP-record is written and MRP_PortState = OFF    */
/* (ii) Send a TLV if MRP_PortState != OFF                                   */
/*      - if no MRP-record is written, send the MRP default UUID (0xFFFFF...)*/
/*      - if a  MRP-record is written, send the written MRP UUID             */
/* (iii) Send a TLV at any rate, if a MRP-record is written (send the        */
/*       written MRP UUID)                                                   */
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef OHA_CFG_NO_MRP
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_update_mrp_port_status(
    OHA_HDB_MRP_PTR_TYPE        pHDBMrp,
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortID)
{
    LSA_UINT16 Status;
    OHA_UPPER_MEM_U8_PTR_TYPE pDomainUUID;
    LSA_UINT8  MRP_DefaultUUID[sizeof(OHA_UUID_TYPE)];
    LSA_BOOL UpdateSend = LSA_FALSE;

    OHA_COMMON_MEM_U8_PTR_TYPE VarPtr = LSA_NULL;
    LSA_UINT16 VarLen  = 0;
    LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP;
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
	LSA_UINT16 Instance;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_update_mrp_port_status(pHDB: 0x%X, NicID: 0x%X, PortID: 0x%X)",
                          pHDBMrp, NICId, PortID);

	OHA_ASSERT_NULL_PTR(pDB);

	Instance = pDB->LldpData[PortID-1].MrpInstance;

	OHA_ASSERT(Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT);

    /* read the actual LLDP-Tlv LLDP_PNIO_MRPPORTSTATUS */
    (LSA_VOID)oha_db_read_loc_lldp_element(NICId, PortID,
                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                           &VarPtr, &VarLen);

    /* the DomainUUID: if no UUID provided by WriteMrpData we take the default-UUID (0xFF's) */
    if (OHA_CheckDomainUUID(pHDBMrp->MrpStateInd[Instance-1].domainUUID)) /* is there a (valid) UUID written */
    {
        pDomainUUID = pHDBMrp->MrpStateInd[Instance-1].domainUUID;
    }
    else
    {
        /* we have to create a Tlv with default UUID */
        OHA_MEMSET(MRP_DefaultUUID, 0xFF, sizeof(OHA_UUID_TYPE));
        pDomainUUID = MRP_DefaultUUID;
    }

    /* Sending rules of the LLDP-Tlv LLDP_PNIO_MRPPORTSTATUS:                    */
    /* (i) No TLV is sent if no MRP-record is written and MRP_PortState = OFF    */
    /* (ii) Send a TLV if MRP_PortState != OFF                                   */
    /*      - if no MRP-record is written, send the MRP default UUID (0xFFFFF...)*/
    /*      - if a  MRP-record is written, send the written MRP UUID             */
    /* (iii) Send a TLV at any rate, if a MRP-record is written (send the        */
    /*       written MRP UUID)                                                   */

    /* Update the OHA-Database, if a DomainUUID is to send */
    if (pHDBMrp->MrpStateInd[Instance-1].mrpStatus != MRP_OPERATION_OFF) /* OHA_PORTSTATE_MRP_OFF */
    {
        /* if no Tlv had been sent or the new Tlv is different */
        if (OHA_IS_NULL(VarPtr) ||
            (!OHA_MEMCMP(VarPtr, pDomainUUID, sizeof(OHA_UUID_TYPE)))  )
        {
            Status = oha_write_mrp_port_status(NICId, PortID, pDomainUUID, 0);
            if (Status != LSA_RET_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"oha_update_mrp_port_status: OHA_LLDPSet failed (Status: 0x%X).", Status);
            }
            UpdateSend = LSA_TRUE;
        }
    }
    else    /* there is nothing to send (anymore) */
    {
        if (OHA_IS_NOT_NULL(VarPtr)) /* if a Tlv had been sent */
        {
            /* delete the lldp object in the database */
            (LSA_VOID)oha_db_delete_loc_lldp_element(NICId, PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                     OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP,
                                                     OHA_LLDP_TYPE_ORG_OUI_PNO);
            UpdateSend = LSA_TRUE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_update_mrp_port_status(UpdateSend: 0x%X)", UpdateSend);
    return(UpdateSend);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_RT_Class3               +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the peer IRT status (real with expected)         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_RT_Class3(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_RT_Class3(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
        LSA_UINT16 Local_IRTPortStatus;

        *pCompare = LSA_FALSE;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

        /* is there a Topo-DB ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL))
        {
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_RT_Class3(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* is the Link Up ? */
        {
            LSA_UINT16  CountOfRealPeers;
            OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

            OHA_ASSERT_NULL_PTR(pHDBEdd);

            Local_IRTPortStatus = pHDBEdd->LinkStatus[PortId-1].IRTPortStatus;

            /* SPECIAL: Peer RT-Class3 mismatch, ChannelErrorType = "Remote mismatch" */
            /* check only if local IRT is supported (IRTPortStatus != OHA_PORT_STATUS_CLASS3_OFF) */
            if (Local_IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF)
                /* && pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_RT_Class3 == OHA_ALARM_SPECIFIER_INITIAL_VALUE)*//* AP00571466 */
            {
                /* RetVal = OHA_ERR_FAILED; */ /* AP00571466 */
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                *pCompare = LSA_TRUE;

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,"OHA_CompareTopoPeer_RT_Class3() - IRT not supported (0x%X) !", Local_IRTPortStatus);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_RT_Class3(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            /* how many real peers are there ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

            /* are there peers to check (no peers or link down) ? */
            if (CountOfRealPeers == 0)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                /* it'll be finally reduced to a NoPeerDetected diagnosis */
                *pCompare = LSA_TRUE;

                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_RT_Class3(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }

        {
            /* look for the corresponding NicId */
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            LSA_UINT16 SenderId;

            OHA_ASSERT_NULL_PTR(pDB);

            SenderId = pDB->LldpData[PortId-1].LldpSenderId;

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                LSA_UINT8  SenderStatus = pDB->LldpData[PortId-1].LldpSenderStatus;

                /* is there a valid sender ? */
                if (SenderStatus == OHA_STATUS_DATA_VALID ||
                    SenderStatus == OHA_STATUS_DATA_INVALID)
                {
                    /* read the PNO-PortStatus */
                    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                    LSA_UINT16 Len = 0;
                    LSA_UINT8  SubType  = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;

                    (LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId, SenderId,
                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           &pVar, &Len);

                    if (OHA_IS_NOT_NULL(pVar) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
                    {
                        /* now we are able to compare the IRTPortStatus */
                        LSA_UINT16  Remote_IRTPortStatus;

                        pVar += 2;               /* 2nd word */
                        Remote_IRTPortStatus = OHA_GET16_NTOH (pVar, 0);

                        /* Bit 3 - 15: RTClass3_PortStatus.reserved */
                        /* This field shall be set according to the */
                        /* Convention for the encoding of reserved bits and octets */
                        Remote_IRTPortStatus &= OHA_PORT_STATUS_CLASS3_MASK;

                        if (!(Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF  ||
                              Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_UP   ||
                              Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_RUNNING))
                        {
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_CompareTopoPeer_RT_Class3() - Remote_C3_PortState out of range (0x%X) !",Remote_IRTPortStatus);
                            Remote_IRTPortStatus  = OHA_PORT_STATUS_CLASS3_OFF;
                        }

                        /* both states must be running */
                        if (Local_IRTPortStatus == OHA_PORT_STATUS_CLASS3_RUNNING &&
                            Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_RUNNING)
                        {
                            *pCompare = LSA_TRUE;
                        }
                        else
                        {
                            OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_CompareTopoPeer_RT_Class3() - Remote_C3_PortState (0x%X) and Local_C3_PortState (0x%X) different!",Remote_IRTPortStatus,Local_IRTPortStatus);
                        }
                    }
                    else
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_CompareTopoPeer_RT_Class3() - Remote PortStatus-Tlv not existing!");
                    }
                }
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_RT_Class3(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPtcpWriteData                    +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_WRITE_PTCP_DATA         +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PtcpData                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write the expected PTCP data                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPtcpWriteData(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId;
    LSA_UINT16              PortId;
    OHA_UPPER_PTCP_DATA_PTR_TYPE pPtcpData;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_HDB_GSY_PTR_TYPE    pHDBGsy;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPtcpWriteData(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pRQB);

    pPtcpData = &pRQB->args.PtcpData;

    OHA_ASSERT (pPtcpData->DoSync == LSA_FALSE || pPtcpData->DoSync == LSA_TRUE); /* sanity */
    OHA_ASSERT (pPtcpData->IsSync == LSA_FALSE || pPtcpData->IsSync == LSA_TRUE); /* sanity */

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    /* Check if we have a LLDP-Channel for NIC-ID. */
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);  /* only LLDP for this request necessary */
    pHDBGsy  = OHA_GSYGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL) || LSA_HOST_PTR_ARE_EQUAL(pHDBLldp->pDB,  LSA_NULL) ||
         LSA_HOST_PTR_ARE_EQUAL(pHDBGsy,  LSA_NULL) )
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PTCP_DATA failed. No LLDP- or GSY-Channel present (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }

#if 0 /* do not look at temp-buffer */
    if (pHDBGsy->SyncDataWrite.Mode == 0)
    {
        Response = OHA_ERR_SEQUENCE;
        /* if there is no sync record written -> SequenceError */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PTCP_DATA failed. Got no PDSyncData record by PrmWrite (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }
#endif

    if (NICId != pHDBLldp->pDB->NicId) /* AP01022057 */
    {
        Response = OHA_ERR_RESOURCE;
        /* Error: no ports. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PTCP_DATA failed. No valid NicID for this interface (0x%X)",pHDBLldp->pDB->InterfaceID);

        OHA_UserRequestFinish(pHDBUser,pRQB,Response); /*an error occurred */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPtcpWriteData(Response: 0x%X)", Response);
        return;
    }

    if (pHDBGsy->SyncDataCommit.Mode == 0)
    {
        Response = OHA_ERR_SEQUENCE;
        /* if there are no SyncData (no PrmCommit is done) -> SequenceError */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PTCP_DATA failed. No PrmCommit of a (written) PDSyncData record is done (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }

    /* if there's no record written, no TLV is to send => if there was a record written, the TLV is to remove */
    if (pPtcpData->DoSync)
    {
        /*********************************************************************/
        /* first the check (SubdomainID, MasterSourceAddress, ...)           */
        /*********************************************************************/
        if (pPtcpData->IsSync)
        {
#if 0
            /* check the SubdomainID */
            if (!OHA_CheckDomainUUID(pPtcpData->SubdomainID))  /* is there a valid UUID ( <> 0) ? */
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_WRITE_PTCP_DATA-Write: IRDataID failed.");
                Response = LSA_RET_ERR_PARAM;
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPtcpWriteData(Response: 0x%X)", Response);
                return;
            }
#endif
#if 0       /* for sync master change (takeover): no check ! */
            /* check the MasterSourceAddress != 0, is depending on IsSync */
            if (OHA_MEMCMP(pPtcpData->MasterSourceAddress, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_MAC, OHA_MAC_ADDR_SIZE))
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_WRITE_PTCP_DATA-Write: MasterSourceAddress failed.");
                Response = LSA_RET_ERR_PARAM;
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPtcpWriteData(Response: 0x%X)", Response);
                return;
            }
#endif
        }
        else
        {
            /* check the MasterSourceAddress == 0, is depending on IsNotSync */
            if (!OHA_MEMCMP(pPtcpData->MasterSourceAddress, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_MAC, OHA_MAC_ADDR_SIZE))
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_WRITE_PTCP_DATA-Write: MasterSourceAddress failed.");
                Response = LSA_RET_ERR_PARAM;
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPtcpWriteData(Response: 0x%X)", Response);
                return;
            }
        }
    }

    /* store the PTCP params and confirm */
    /* we get the master source address and what's to do from the user */
    pHDBGsy->SyncDataCommit.Mode = (pPtcpData->DoSync) ? (LSA_UINT16)OHA_SYNC_ADD: (LSA_UINT16)OHA_SYNC_REMOVE;

    if (pPtcpData->DoSync)
    {
        OHA_MEMCPY(&pHDBGsy->SyncDataCommit.MasterSourceAddress, pPtcpData->MasterSourceAddress, sizeof(pPtcpData->MasterSourceAddress));
    }

    /* we handle the PTCP timer in case of OHA_OPC_WRITE_PTCP_DATA (and OHA_OPC_PRM_COMMIT) */
    /* STOP if running and START in case of DoSync */
    for (PortId = 0; PortId < OHA_CFG_MAX_PORTS && PortId < pHDBLldp->pDB->PortCnt; PortId++)  /* over all ports */
    {
        /* stop the PTCP timer if running */
        if (pHDBUser->PtcpTimerInUse[PortId])
        {
            /* stop the PTCP timer */
            pHDBUser->PtcpTimerInUse[PortId] = LSA_FALSE;
            OHA_TimerStop(pHDBUser->PtcpTimerID[PortId]);
        }

        /* (re)start the timer in case of DoSync */
        if (pHDBGsy->SyncDataCommit.Mode == OHA_SYNC_ADD)
        {
            /* (re)start the timer if a delay is existing */
            if (pHDBUser->PtcpDelayTime[PortId] != 0)
            {
                OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId];

                /* expected sync difference diagnosis demanded ? */
                if (OHA_IS_NOT_NULL(pTopoDB) && pTopoDB->DelayDifferenceExisting &&
                    (pTopoDB->DelayMode & OHA_SYNC_DIFFERENCE_CHECK_ON))
                {
                    /* now start the PTCP timer and set the timeout "active" */
                    pHDBUser->PtcpTimerInUse[PortId] = LSA_TRUE;
                    OHA_TimerStart(pHDBUser->PtcpTimerID[PortId], pHDBUser->PtcpDelayTime[PortId]);
                }
            }
        }
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    /* update, send the PTCP-status TLV and build the PTCP diagnosis */
    oha_update_sync_status(NICId);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPtcpWriteData(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_update_sync_status                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+ Description: Add or remove the TLV LLDP_PNIO_PTCPSTATUS and do LldpSend +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID oha_update_sync_status(LSA_UINT16 NICId)
{
    LSA_UINT16              PortID;

    OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBLldp);
    OHA_ASSERT_NULL_PTR(pHDBLldp->pDB);

    /* add or remove the TLV ptcp status and update sending if active */
    /* loop over all ports, IsSync is interface global */
    for (PortID = 1; PortID <= OHA_CFG_MAX_PORTS && PortID <= pHDBLldp->pDB->PortCnt; PortID++)
    {
        LSA_BOOL        PtcpDifference;

        /* determines, if the LLDP-TLV LLDP_PNIO_PTCPSTATUS is to send */
        PtcpDifference = oha_update_ptcp_status(NICId, PortID, LSA_FALSE);

        if (PtcpDifference) /* update LLDP data by LLDPSet() */
        {
            if (pHDBLldp->RequestState == OHA_LLDP_STATE_START ||
                pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START ||
                pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START) /* LLDP-Set is possible ? */
            {
                LSA_UINT16 Status = OHA_LLDPSet(pHDBLldp, PortID, OHA_HANDLE_TYPE_SYS); /* send them */
                if (Status != LSA_RET_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"oha_update_sync_status: OHA_LLDPSet failed (Status: 0x%X).", Status);
                }
            }

            /* check the TOPO and store the indication in the USER-HDB as LastTopoWriteInd */
            /* Handle the PTCP Status Change for this port */
            OHA_IndicateEventAllUsers(NICId, PortID);
        }
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_update_ptcp_status                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LSA_TRUE  - the TLV LLDP_PNIO_PTCPSTATUS is to update.    +*/
/*+               LSA_FALSE - the TLV LLDP_PNIO_PTCPSTATUS is not to update.+*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_update_ptcp_status(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortID,
    LSA_BOOL                    OnlySyncForwarding)
{
    LSA_BOOL UpdateSend = LSA_FALSE;
    LSA_UINT16 Status;

    OHA_COMMON_MEM_U8_PTR_TYPE VarPtr = LSA_NULL;
    LSA_UINT16 VarLen  = 0;
    LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS;

    OHA_HDB_GSY_PTR_TYPE  pHDBGsy  = OHA_GSYGetHandleByNIC(NICId);
	OHA_HDB_EDD_PTR_TYPE  pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_update_ptcp_status(NicID: 0x%X, PortID: 0x%X, OnlySyncForwarding: 0x%X)",
                          NICId, PortID, OnlySyncForwarding);

    /* PortID 1 ... n */
    OHA_ASSERT (PortID && (PortID <= OHA_CFG_MAX_PORTS));
    OHA_ASSERT_NULL_PTR(pHDBGsy);
    OHA_ASSERT_NULL_PTR(pHDBEdd);

    /* update the frames offset */
    OHA_UpdateFrameOffset(NICId, PortID, OnlySyncForwarding);

    /* read the actual LLDP-Tlv LLDP_PNIO_PTCPSTATUS */
    (LSA_VOID)oha_db_read_loc_lldp_element(NICId, PortID,
                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                           &VarPtr, &VarLen);

    /* write the Tlv to the OHA-Database */
    if (pHDBGsy->SyncDataCommit.Mode == OHA_SYNC_ADD)
    {
        OHA_FRAME_OFFSET_PTR_TYPE   pFrameOffset = &pHDBGsy->FrameOffsetValid[PortID-1];
        OHA_ADJUST_PTR_TYPE         pAdjustData  = &pHDBGsy->AdjustData[PortID-1];

        OHA_UPPER_MEM_U8_PTR_TYPE   pMasterSourceAddress;
        OHA_UPPER_MEM_U8_PTR_TYPE   pSubdomainUUID;
        OHA_UPPER_MEM_U8_PTR_TYPE   pIRDataUUID;

        OHA_MAC_TYPE  NullMasterSourceAddr;
        OHA_UUID_TYPE NullSubdomainID;
        OHA_UUID_TYPE NullIRDataID;

        LSA_BOOL IsWireless = OHA_IsWirelessPort(NICId, PortID);

        OHA_MEMSET(NullMasterSourceAddr, 0, sizeof(OHA_MAC_TYPE));
        OHA_MEMSET(NullSubdomainID, 0, sizeof(OHA_UUID_TYPE));
        OHA_MEMSET(NullIRDataID, 0, sizeof(OHA_UUID_TYPE));

        /* AP00418772 - if a DomainBoundary of the port is set, the MSAddress, PTCPSubdomainUUID */
        /* and the IRDATAUUID is always set to "0" */
        /* a wireless port always has a boundary */
        if (IsWireless)
        {
            pMasterSourceAddress = NullMasterSourceAddr;
            pSubdomainUUID       = NullSubdomainID;
            pIRDataUUID          = NullIRDataID;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "RtClass3_TopoState (PortId: 0x%X): IsWireless port, UUIDs are set to Null (=OK)!", PortID);
        }
        else
        {
            /* False: do not block the multicast MAC */
            if (((pAdjustData->DomainBoundaryIngress & OHA_SYNC_BOUNDARY_MASK) == OHA_SYNC_BOUNDARY_OFF) ||
                ((pAdjustData->DomainBoundaryEgress & OHA_SYNC_BOUNDARY_MASK) == OHA_SYNC_BOUNDARY_OFF) )
            {
                pMasterSourceAddress = pHDBGsy->SyncDataCommit.MasterSourceAddress;
                pSubdomainUUID       = pHDBGsy->SyncDataCommit.SubdomainID;
                pIRDataUUID          = pHDBGsy->SyncDataCommit.IRDataID;
            }
            else    /* a DomainBoundary is set */
            {
                pMasterSourceAddress = NullMasterSourceAddr;
                pSubdomainUUID       = NullSubdomainID;
                pIRDataUUID          = NullIRDataID;
            }
        }

        /* Anpassung der PortStateMachine (AP00539134) */
        /* - Is only SyncForwarding parametrized for an (IRT)Port, so an LLDP-send will be done with IRDATAUUID == 0 (and RTC3-PortState==OFF). */
        if (OnlySyncForwarding || 			/* sync forwarding only  */
			(pHDBEdd->LinkStatus[PortID-1].IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF)) /* IRT not supported: set IRDATAUUID = {0} */
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "RtClass3_TopoState (PortId: 0x%X): OnlySyncForwarding port, Remote_IRDataUUID is set to Null (=OK)!", PortID);
            pIRDataUUID          = NullIRDataID;
        }

		/* patch for AP00916191: */
		/* OhaPrmEnd is done and the new IRDataUUID is different, send LLDP as {0} until OhaPrmCommit */
		if (OHA_IS_NOT_NULL(pHDBEdd->pHDBUserPrm))	/* PRM-User existing ? */
		{
			if (pHDBEdd->pHDBUserPrm->PrmState == OHA_PRM_STATE_END)
			{
				if (!OHA_MEMCMP(pHDBGsy->SyncDataWrite.IRDataID, /* written by PrmWrite (= temp buffer) */
								pIRDataUUID, 
								OHA_UUID_SIZE))
				{
					OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "PRM_END (PortId: 0x%X) is done and the new IRDataUUID is different, Remote_IRDataUUID is set to Null (=OK)!", PortID);
					pIRDataUUID = NullIRDataID;
				}
			}
		}

        /* if no Tlv exists or the new Tlv is different */
        /* LLDP_PNIO_PTCPSTATUS-data: PTCP_MasterSourceAddress, PTCP_SubdomainUUID, IRDataUUID,        */
        /*     LLDP_LengthOfPeriod, LLDP_RedPeriodBegin, LLDP_OrangePeriodBegin, LLDP_GreenPeriodBegin */
        if (OHA_IS_NULL(VarPtr) || VarLen != OHA_LLDP_PNO_PTCP_STATUS_SIZE)
        {
            UpdateSend = LSA_TRUE;
        }
        else
        {
            if ((!OHA_MEMCMP(VarPtr, pMasterSourceAddress, OHA_MAC_ADDR_SIZE)) ||
                (!OHA_MEMCMP(VarPtr+OHA_MAC_ADDR_SIZE, pSubdomainUUID, OHA_UUID_SIZE)) ||
                (!OHA_MEMCMP(VarPtr+OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE, pIRDataUUID, OHA_UUID_SIZE)) )
            {
                UpdateSend = LSA_TRUE;
            }
            else
            {
                OHA_COMMON_MEM_U8_PTR_TYPE pPtcpOffset = VarPtr+OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE+OHA_UUID_SIZE;
                LSA_UINT32 TmpLen    = OHA_LLDP_GET_U32(pPtcpOffset);
                LSA_UINT32 TmpRed    = OHA_LLDP_GET_U32(pPtcpOffset+sizeof(LSA_UINT32));
                LSA_UINT32 TmpOrange = OHA_LLDP_GET_U32(pPtcpOffset+2*sizeof(LSA_UINT32));
                LSA_UINT32 TmpGreen  = OHA_LLDP_GET_U32(pPtcpOffset+3*sizeof(LSA_UINT32));

                if (pFrameOffset->LengthOfPeriod    != TmpLen ||
                    pFrameOffset->RedPeriodBegin    != TmpRed ||
                    pFrameOffset->OrangePeriodBegin != TmpOrange ||
                    pFrameOffset->GreenPeriodBegin  != TmpGreen)
                {
                    UpdateSend = LSA_TRUE;
                }
            }
        }
        if (UpdateSend)
        {
            OHA_ASSERT(pHDBGsy->SyncDataCommit.Mode);

            Status = oha_write_ptcp_status(NICId, PortID,
                                           pMasterSourceAddress,
                                           pSubdomainUUID,
                                           pIRDataUUID,
                                           pFrameOffset);
            if (Status != LSA_RET_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"oha_update_ptcp_status: OHA_LLDPSet failed (Status: 0x%X).", Status);
            }
        }
    }
    else /* OHA_SYNC_REMOVE or reinitializing the DB */ /* remove the Tlv from the OHA-Database */
    {
        if (OHA_IS_NOT_NULL(VarPtr)) /* if a Tlv exists */
        {
            /* delete the lldp object in the database */
            (LSA_VOID)oha_db_delete_loc_lldp_element(NICId, PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                     OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS,
                                                     OHA_LLDP_TYPE_ORG_OUI_PNO);
            UpdateSend = LSA_TRUE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_update_ptcp_status(UpdateSend: 0x%X)", UpdateSend);
    return(UpdateSend);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RemoteChassisIdIsIdentical              +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Input/Output          :    LSA_UINT16       PortID                     +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+ Description: LSA_TRUE  - local and remote StationName are identical     +*/
/*+              LSA_FALSE - local and remote StationName are NOT identical +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_RemoteChassisIdIsIdentical(LSA_UINT16 NICId, LSA_UINT16 PortId, LSA_UINT16 SenderId,
										OHA_DB_TOPO_PTR_TYPE pTopoDB)
{
    OHA_COMMON_MEM_U8_PTR_TYPE pRemoteChassisID = LSA_NULL;
	LSA_UINT16 VarLen   = 0;
	LSA_UINT8  SubType  = 0xff;
	LSA_BOOL   IsIdentical = LSA_FALSE;
	OHA_UPPER_PEER_PTR_TYPE pRemotePeer;

	OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RemoteChassisIdIsIdentical(NICId: 0x%X, PortId: 0x%X, SenderId: 0x%X, pPeer: 0x%X)",
                          NICId, PortId, SenderId, pTopoDB);

	if (SenderId == 0 || SenderId > LLDP_MAX_PORT_SENDER)	 /* none or more than 1 */
	{
		OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "No (valid) Sender present(SenderId: 0x%X, PortId: 0x%X)", SenderId, PortId);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RemotePortIdIsIdentical(IsIdentical: 0x%X)", IsIdentical);
		return(IsIdentical);
	}
	
	OHA_ASSERT_NULL_PTR(pTopoDB);

	/* AP01273560: Leitungslängen-Tausch mit BANY bei SCF9, DFP */
	/* pRemotePeer = &pTopoDB->PeerSender[SenderId-1]; */

	if(pTopoDB->NumberOfPeers == 1)	/* only one allowed for IRT */
	{
		pRemotePeer = &pTopoDB->PeerSender[0];
	}
	else
	{
		pRemotePeer = LSA_NULL;
	}

	(LSA_VOID)oha_db_read_rem_chassis_name(NICId, PortId, SenderId,
										   &SubType,
										   &pRemoteChassisID, &VarLen);

	if (OHA_IS_NOT_NULL(pRemoteChassisID) && VarLen == 0)
	{
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemoteChassisIdIsIdentical (PortId: 0x%X): No valid Tlv LLDPChassisID present", PortId);
		pRemoteChassisID = LSA_NULL;
	}
	else if (OHA_IS_NOT_NULL(pRemotePeer) &&
			 OHA_IS_NOT_NULL(pRemotePeer->pPeerChassisID) &&
			 pRemotePeer->LengthPeerChassisID != 0)
    {
        if (OHA_IS_NOT_NULL(pRemoteChassisID))
        {
            /* now compare length and strings */
            if (VarLen == pRemotePeer->LengthPeerChassisID)
            {
                IsIdentical = oha_namecmp(pRemotePeer->pPeerChassisID, pRemoteChassisID, VarLen, OHA_UPPER_CASES_ALLOWED(NICId));
                if (!IsIdentical)
                {
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemoteChassisIdIsIdentical (PortId: 0x%X): Remote_NAME and Expected_NAME not identical", PortId);
                }
            }
            else
            {
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemoteChassisIdIsIdentical (PortId: 0x%X): Remote_NAME and Expected_NAME not identical", PortId);
            }
        }
        else
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemoteChassisIdIsIdentical (PortId: 0x%X): No Tlv LLDPChassis present", PortId);
        }
    }
    else
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemoteChassisIdIsIdentical (PortId: 0x%X): No (valid) Expected_NAME present!", PortId);
    }

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RemoteChassisIdIsIdentical(IsIdentical: 0x%X)", IsIdentical);
	return(IsIdentical);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RemotePortIdIsIdentical                 +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Input/Output          :    LSA_UINT16       PortID                     +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LSA_TRUE  - local and remote PortName are identical       +*/
/*+               LSA_FALSE - local and remote PortName are NOT identical   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_RemotePortIdIsIdentical(LSA_UINT16 NICId, LSA_UINT16 PortId, LSA_UINT16 SenderId,
										    OHA_DB_TOPO_PTR_TYPE pTopoDB)
{
    OHA_COMMON_MEM_U8_PTR_TYPE pRemotePortID = LSA_NULL;
	LSA_UINT16 VarLen   = 0;
	OHA_UPPER_PEER_PTR_TYPE pRemotePeer;

	LSA_BOOL   IsIdentical = LSA_FALSE;

	OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                         "IN : OHA_RemotePortIdIsIdentical(NICId: 0x%X, PortId: 0x%X, SenderId: 0x%X, pTopoDB: 0x%X)",
                          NICId, PortId, SenderId, pTopoDB);

	OHA_ASSERT_NULL_PTR(pTopoDB);

	if (SenderId == 0 || SenderId > LLDP_MAX_PORT_SENDER)	 /* none or more than 1 */
	{
		OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "No (valid) Sender present(SenderId: 0x%X, PortId: 0x%X)", SenderId, PortId);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RemotePortIdIsIdentical(IsIdentical: 0x%X)", IsIdentical);
		return(IsIdentical);
	}

	/* AP01273560: Leitungslängen-Tausch mit BANY bei SCF9, DFP */
	/* pRemotePeer = &pTopoDB->PeerSender[SenderId-1]; */

	if(pTopoDB->NumberOfPeers == 1)	/* only one allowed for IRT */
	{
		pRemotePeer = &pTopoDB->PeerSender[0];
	}
	else
	{
		pRemotePeer = LSA_NULL;
	}

	(LSA_VOID)oha_db_read_rem_port_name(NICId, PortId, SenderId, LSA_NULL, &pRemotePortID, &VarLen);
	if (OHA_IS_NOT_NULL(pRemotePortID) && VarLen == 0)
	{
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OHA_RemotePortIdIsIdentical (PortId: 0x%X): No valid Tlv LLDPPortID present", PortId);
		pRemotePortID = LSA_NULL;
	}
	else if (OHA_IS_NOT_NULL(pRemotePeer) &&
			 OHA_IS_NOT_NULL(pRemotePeer->pPeerPortID) &&
	         pRemotePeer->LengthPeerPortID != 0)
    {
        if (OHA_IS_NOT_NULL(pRemotePortID))
        {
            /* now compare length and strings */
            if (VarLen == pRemotePeer->LengthPeerPortID)
            {
                IsIdentical = oha_namecmp(pRemotePeer->pPeerPortID, pRemotePortID, VarLen, OHA_UPPER_CASES_ALLOWED(NICId));
                if (IsIdentical)
                {
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemotePortIdIsIdentical (PortId: 0x%X): Remote_PORT and Expected_PORT identical", PortId);
                }
            }
            else
            {
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemotePortIdIsIdentical (PortId: 0x%X): Remote_PORT and Expected_PORT not identical", PortId);
            }
        }
        else
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemotePortIdIsIdentical (PortId: 0x%X): No (valid) Tlv LLDPPort present", PortId);
        }
    }
    else
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_RemotePortIdIsIdentical (PortId: 0x%X): No (valid) Expected_PORT present!", PortId);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RemotePortIdIsIdentical(IsIdentical: 0x%X)", IsIdentical);
	return(IsIdentical);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MAUTypeIsFullDuplex                     +*/
/*+  Input/Output          :    LSA_UINT16       MAUType                    +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  MAUType                                                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LSA_TRUE  - MAUType is valid for RTC3 (full duplex)       +*/
/*+               LSA_FALSE - MAUType is not valid for RTC3                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_BOOL OHA_MAUTypeIsFullDuplex(LSA_UINT16 MAUType)
{
	LSA_BOOL MAUTypeIsFullDuplex;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MAUTypeIsFullDuplex(MAUType: %u)", MAUType);

	if (MAUType > LLDP_TLV_MAU_TYPE_1000BASETFD) /* "unknown" MAUType */
	{
		MAUTypeIsFullDuplex = LSA_TRUE; /* assume FD and >= 100Mbps */
	}
	else
	{
		switch(MAUType)
		{
		case LLDP_TLV_MAU_TYPE_100BASETXFD:
		case LLDP_TLV_MAU_TYPE_100BASEFXFD:
		case LLDP_TLV_MAU_TYPE_100BASET2FD:
		case LLDP_TLV_MAU_TYPE_1000BASEXFD:
		case LLDP_TLV_MAU_TYPE_1000BASELXFD:
		case LLDP_TLV_MAU_TYPE_1000BASESXFD:
		case LLDP_TLV_MAU_TYPE_1000BASECXFD:
		case LLDP_TLV_MAU_TYPE_1000BASETFD:
			MAUTypeIsFullDuplex = LSA_TRUE; /* FD and >= 100Mbps */
			break;

		default:
			MAUTypeIsFullDuplex = LSA_FALSE;
			break;
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MAUTypeIsFullDuplex(MAUTypeIsValid: %u)", MAUTypeIsFullDuplex);
	return(MAUTypeIsFullDuplex);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetRemotePortState                      +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  PortId               : Port                                            +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: determies the remote PortState for the service            +*/
/*+               EDD_SRV_SET_REMOTE_PO_STATE                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_GetRemotePortState(
	LSA_UINT16              NICId,
    LSA_UINT16              PortId,
	OHA_DB_TOPO_PTR_TYPE    pTopoDB,
    EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE pRemotePortState)
{
	LSA_UINT16 SenderId;

	LSA_UINT32 Local_LineDelayNs;
    LSA_UINT32 Local_CableDelayNs;
	LSA_UINT32 Remote_CableDelayNs;

	OHA_UPPER_MEM_U8_PTR_TYPE pExpected_IRDataUUID;
    OHA_UPPER_MEM_U8_PTR_TYPE pLocal_PTCP_SubdomainUUID;
    OHA_UPPER_MEM_U8_PTR_TYPE pLocal_SyncMaster_MAC;

    OHA_COMMON_MEM_U8_PTR_TYPE pRemote_IRDataUUID;
    OHA_COMMON_MEM_U8_PTR_TYPE pRemote_PTCP_SubdomainUUID;
    OHA_COMMON_MEM_U8_PTR_TYPE pRemote_SyncMaster_MAC;

	LSA_UINT32 Remote_ReservedInterval;
	LSA_UINT16 Local_IRTPortStatus;
	LSA_UINT16 Remote_IRTPortStatus;
	LSA_UINT16 Remote_MAUType;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetRemotePortState(NICId: 0x%X, PortId: %u, pTopoDB: 0x%X)",
                          NICId, PortId, pTopoDB);

	OHA_ASSERT(PortId > 0 && PortId <= OHA_CFG_MAX_PORTS);
    OHA_ASSERT_NULL_PTR(pTopoDB);
    OHA_ASSERT_NULL_PTR(pRemotePortState);

    {
	OHA_HDB_EDD_PTR_TYPE pHDBEdd;
    OHA_HDB_GSY_PTR_TYPE pHDBGsy;
    OHA_DB_IF_PTR_TYPE   pDB;

    pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBEdd);

    pHDBGsy = OHA_GSYGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBGsy);

    pDB = OHA_DB_GET_INTERFACE(NICId);
    OHA_ASSERT_NULL_PTR(pDB);

	SenderId = pDB->LldpData[PortId-1].LldpSenderId;
	OHA_ASSERT(SenderId <= LLDP_MAX_PORT_SENDER);

	if (SenderId == 0 || pDB->LldpData[PortId-1].LldpSenderStatus == OHA_STATUS_NOT_PRESENT)
	{
		OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u): No (valid) Sender present(SenderId: %u)", PortId, SenderId);
		SenderId = 0; /* not present */
	}
	else if (pDB->LldpData[PortId-1].LldpSenderStatus == OHA_STATUS_MULTIPLE_SENDER)
	{
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u): multiple Sender", PortId);
		SenderId = 0; /* multiple sender */
	}
	else /* OHA_STATUS_DATA_VALID or OHA_STATUS_DATA_INVALID */
	{
		/* there is a valid neighbour (i.e., a single sender) */
	}

    Local_LineDelayNs   = pHDBEdd->LinkStatus[PortId-1].LineDelay * pDB->Resolution; /* LineDelay * Resolution in ns */
    Local_CableDelayNs  = pHDBEdd->LinkStatus[PortId-1].CableDelay;
	Local_IRTPortStatus = pHDBEdd->LinkStatus[PortId-1].IRTPortStatus;

	OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): Local_IRTPortStatus(0x%X)"
		, PortId, Local_IRTPortStatus
		);

    pExpected_IRDataUUID      = pHDBGsy->SyncDataCommit.IRDataID;  /* Expected_ IRDataUUID */
    pLocal_PTCP_SubdomainUUID = pHDBGsy->SyncDataCommit.SubdomainID;  /* Local_PTCP_SubdomainUUID */
    pLocal_SyncMaster_MAC     = pHDBGsy->SyncDataCommit.MasterSourceAddress;  /* Local_SyncMaster_MAC */

	if (OHA_MEMCMP(pExpected_IRDataUUID, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_UUID, OHA_UUID_SIZE))
	{
		pExpected_IRDataUUID = LSA_NULL; /* {0} is the same as "no UUID" */
	}

	if (OHA_MEMCMP(pLocal_PTCP_SubdomainUUID, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_UUID, OHA_UUID_SIZE))
	{
		pLocal_PTCP_SubdomainUUID = LSA_NULL; /* {0} is the same as "no UUID" */
	}

	if (OHA_MEMCMP(pLocal_SyncMaster_MAC, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_MAC, OHA_MAC_ADDR_SIZE))
	{
		pLocal_SyncMaster_MAC = LSA_NULL; /* {0} is the same as "no MAC" */
	}
	}

	/***/

	Remote_ReservedInterval    = 0;
	pRemote_IRDataUUID         = LSA_NULL;
	pRemote_PTCP_SubdomainUUID = LSA_NULL;
	pRemote_SyncMaster_MAC     = LSA_NULL;

	if (SenderId != 0)
	{
	    OHA_COMMON_MEM_U8_PTR_TYPE pRemotePtcpStatus = LSA_NULL;
		LSA_UINT16 VarLen = 0;
		LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS;

	    /* read the remote LLDP-Tlv LLDP_PNIO_PTCPSTATUS */
		(LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId, SenderId,
											   OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
											   &pRemotePtcpStatus, &VarLen);

		if (OHA_IS_NOT_NULL(pRemotePtcpStatus) && VarLen == OHA_LLDP_PNO_PTCP_STATUS_SIZE)
		{
            /* LLDP_PNIO_PTCPSTATUS-data: PTCP_MasterSourceAddress, PTCP_SubdomainUUID, IRDataUUID,        */
            /*     LLDP_LengthOfPeriod, LLDP_RedPeriodBegin, LLDP_OrangePeriodBegin, LLDP_GreenPeriodBegin */

			OHA_COMMON_MEM_U8_PTR_TYPE pPtcpOffset = pRemotePtcpStatus+OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE+OHA_UUID_SIZE;
			Remote_ReservedInterval = OHA_LLDP_GET_U32(pPtcpOffset+2*sizeof(LSA_UINT32)); /* orange phase */

            /* Remote_IRDataUUID */
            pRemote_IRDataUUID = pRemotePtcpStatus+OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE;

            /* Remote_PTCP_SubdomainUUID */
			pRemote_PTCP_SubdomainUUID = pRemotePtcpStatus+OHA_MAC_ADDR_SIZE;

			pRemote_SyncMaster_MAC = pRemotePtcpStatus;

			if (OHA_MEMCMP(pRemote_IRDataUUID, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_UUID, OHA_UUID_SIZE))
			{
				pRemote_IRDataUUID = LSA_NULL; /* {0} is the same as "no UUID" */
			}
			else if (OHA_IS_NOT_NULL(pExpected_IRDataUUID))
			{
				if (OHA_MEMCMP(pRemote_IRDataUUID, pExpected_IRDataUUID, OHA_UUID_SIZE)) /* Remote_IRDataUUID == Expected_IRDataUUID */
				{
					pRemote_IRDataUUID = pExpected_IRDataUUID; /* see usage and tracing */
				}
			}

			if (OHA_MEMCMP(pRemote_PTCP_SubdomainUUID, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_UUID, OHA_UUID_SIZE))
			{
				pRemote_PTCP_SubdomainUUID = LSA_NULL; /* {0} is the same as "no UUID" */
			}
			else if (OHA_IS_NOT_NULL(pLocal_PTCP_SubdomainUUID)) /* exists */
			{
				if (OHA_MEMCMP(pRemote_PTCP_SubdomainUUID, pLocal_PTCP_SubdomainUUID, OHA_UUID_SIZE)) /* Remote_PTCP_SubdomainUUID == Local_PTCP_SubdomainUUID */
				{
					pRemote_PTCP_SubdomainUUID = pLocal_PTCP_SubdomainUUID; /* see usage and tracing */
				}
			}

			if (OHA_MEMCMP(pRemote_SyncMaster_MAC, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_MAC, OHA_MAC_ADDR_SIZE))
			{
				pRemote_SyncMaster_MAC = LSA_NULL; /* {0} is the same as "no MAC" */
			}
			else if (OHA_IS_NOT_NULL(pLocal_SyncMaster_MAC)) /* exists */
			{
				if (OHA_MEMCMP(pRemote_SyncMaster_MAC, pLocal_SyncMaster_MAC, OHA_MAC_ADDR_SIZE)) /* pRemote_SyncMaster_MAC == pLocal_SyncMaster_MAC */
				{
					pRemote_SyncMaster_MAC = pLocal_SyncMaster_MAC; /* see usage and tracing */
				}
			}
		}
		else
		{
			OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u): No (valid) Tlv LLDP_PNIO_PTCPSTATUS present (PtcpLen: %u)", PortId, VarLen);
		}
	}

	/***/

	Remote_CableDelayNs = 0;

	if (SenderId != 0)
	{
	    OHA_COMMON_MEM_U8_PTR_TYPE pRemotePnoDelay = LSA_NULL;
		LSA_UINT16 VarLen   = 0;
		LSA_UINT8  SubType  = OHA_LLDP_SUB_TYPE_PNO_DELAY;

		/* read from LLDP-DB (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
		(LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId, SenderId,
											   OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
											   &pRemotePnoDelay, &VarLen);

		if (OHA_IS_NOT_NULL(pRemotePnoDelay) && (VarLen == OHA_LLDP_PNO_DELAY_SIZE))
		{
			Remote_CableDelayNs = OHA_LLDP_GET_U32(pRemotePnoDelay+(OHA_LLDP_IDX_CABLEDELAY_LOC * sizeof(LSA_UINT32)));
		}
		else
		{
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u): No (valid) Tlv LLDP_PNIO_DELAY present", PortId);
		}
	}

	/***/
	
	Remote_MAUType = 0;

	if (SenderId != 0)
	{
	    OHA_COMMON_MEM_U8_PTR_TYPE pMacPhyConfigStatus = LSA_NULL;
		LSA_UINT16 VarLen   = 0;
		LSA_UINT8  SubType  = LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS;

		/* read from LLDP-DB (Autoneg.support status, Autoneg.advertised capability, Operational MAU type) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NICId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
											   &pMacPhyConfigStatus, &VarLen);

		if (OHA_IS_NOT_NULL(pMacPhyConfigStatus) &&
			(VarLen == (LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE +
					 LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE +
					 LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_MAUT_TYPE_SIZE)) )
		{
			Remote_MAUType = OHA_LLDP_GET_U16(pMacPhyConfigStatus+
											  LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE+
											  LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE);
		}
		else
		{
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u): No (valid) Tlv LLDP_MAC_PHY_CFG_STATUS present", PortId);
		}
	}

	/***/

	Remote_IRTPortStatus = OHA_PORT_STATUS_CLASS3_OFF;

	if (SenderId != 0)
	{
		OHA_COMMON_MEM_U8_PTR_TYPE pRemotePnoPortStatus = LSA_NULL;
		LSA_UINT16 VarLen   = 0;
		LSA_UINT8  SubType  = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;

		/* read the remote OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS */
		(LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId, SenderId,
											   OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
											   &pRemotePnoPortStatus, &VarLen);

		if (OHA_IS_NOT_NULL(pRemotePnoPortStatus) && (VarLen == OHA_LLDP_PNO_PORT_STATUS_SIZE))
		{
            Remote_IRTPortStatus = OHA_LLDP_GET_U16 (pRemotePnoPortStatus + 2); /* 2nd word */
		}
        else
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u): No (valid) Tlv LLDP_PNIO_PORTSTATUS present", PortId);
        }
	}

    /****************************************************************************/

	{
	LSA_BOOL DummyDelayResponder;
	LSA_BOOL CableDelayDifferenceLT50ns;
	LSA_BOOL RemoteMAUTypeIsValid;
    LSA_UINT32 SyncId0_TopoState;
    LSA_UINT32 RtClass2_TopoState;
    LSA_UINT32 RtClass3_TopoState;
    LSA_UINT32 RtClass3_RemoteState;
    LSA_UINT32 RtClass3_PDEVTopoState;
    LSA_UINT32 Neighbour_TopoState;

	DummyDelayResponder        = LSA_FALSE;
	CableDelayDifferenceLT50ns = LSA_FALSE;

	if (Local_LineDelayNs != 0) /* local line-delay measured */
	{
		if (Local_CableDelayNs == 0) /* special case 'dummy-delay-responder', see GSY */
		{
			DummyDelayResponder = LSA_TRUE; /* note: must not evaluate Local_LineDelayNs! */
		}
		else
		{
			if (Remote_CableDelayNs != 0) /* and remote cable-delay measured */
			{
				LSA_UINT32 CableDelayDifference;

				CableDelayDifference = (Remote_CableDelayNs > Local_CableDelayNs) ?
										Remote_CableDelayNs - Local_CableDelayNs :
										Local_CableDelayNs - Remote_CableDelayNs;

				if (CableDelayDifference < 50) /* ABS(Local_CableDelay - Remote_CableDelay) < 50ns? */
				{
					CableDelayDifferenceLT50ns = LSA_TRUE;
				}
			}
		}
	}

	OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): DummyDelayResponder(%u) CableDelayDifferenceLT50ns(%u) [Local_LineDelayNs(%u) Local_CableDelayNs(%u) Remote_CableDelayNs(%u)]"
		, PortId, DummyDelayResponder, CableDelayDifferenceLT50ns
		, Local_LineDelayNs, Local_CableDelayNs, Remote_CableDelayNs
		);

	/***/

	RemoteMAUTypeIsValid = OHA_MAUTypeIsFullDuplex(Remote_MAUType);

	OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): RemoteMAUTypeIsValid(%u) [Remote_MAUType(%u)]"
		, PortId, RemoteMAUTypeIsValid, Remote_MAUType
		);

    /****************************************************************************/
    /* SyncId0_TopoState                                                        */
    /****************************************************************************/

    SyncId0_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;

    if (DummyDelayResponder)
    {
        SyncId0_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
    }
	else if( CableDelayDifferenceLT50ns ) /* ABS(Local_CableDelay - Remote_CableDelay) < 50ns */
	{
		if (OHA_IS_NULL(pLocal_PTCP_SubdomainUUID)) /* Local_PTCP_SubdomainUUID == {0} */
		{
			SyncId0_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
		}
		else if (OHA_IS_NULL(pRemote_PTCP_SubdomainUUID)) /* Remote_PTCP_SubdomainUUID == {0} */
		{
			SyncId0_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
		}
		else if (OHA_ARE_EQUAL(pRemote_PTCP_SubdomainUUID, pLocal_PTCP_SubdomainUUID)) /* Remote_PTCP_SubdomainUUID == Local_PTCP_SubdomainUUID */
		{
			if (OHA_IS_NULL(pLocal_SyncMaster_MAC)) /* Local_SyncMaster_MAC == {0} */
			{
				SyncId0_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
			}
			else if (OHA_IS_NULL(pRemote_SyncMaster_MAC)) /* Remote_SyncMaster_MAC == {0} */
			{
				SyncId0_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
			}
			else if (OHA_ARE_EQUAL(pRemote_SyncMaster_MAC, pLocal_SyncMaster_MAC)) /* Remote_SyncMaster_MAC == Local_SyncMaster_MAC */
			{
				SyncId0_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
			}
		}
	}

	OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): SyncId0_TopoState(%u) [DummyDelayResponder() CableDelayDifferenceLT50ns() pLocal_PTCP_SubdomainUUID(%08X) pRemote_PTCP_SubdomainUUID(%08X) pLocal_SyncMaster_MAC(%u) pRemote_SyncMaster_MAC(%u)]"
		, PortId, SyncId0_TopoState
		, pLocal_PTCP_SubdomainUUID, pRemote_PTCP_SubdomainUUID
		, pLocal_SyncMaster_MAC, pRemote_SyncMaster_MAC
		);

    /****************************************************************************/
    /* RtClass2_TopoState: see RTC2PSM (IEC61158 V2.3) and AP00859214           */
    /****************************************************************************/

	RtClass2_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;

	if (CableDelayDifferenceLT50ns) /* ABS(Local_CableDelay - Remote_CableDelay) < 50ns */
	{
		if ((Remote_ReservedInterval & OHA_SYNC_PERIOD_VALID_MASK) != 0) /* Bit 31: LLDP_OrangePeriodBegin.Valid */
		{
			if (OHA_IS_NOT_NULL(pLocal_PTCP_SubdomainUUID) && OHA_IS_NOT_NULL(pRemote_PTCP_SubdomainUUID)) /* both must exist */
			{
				if (OHA_ARE_EQUAL(pRemote_PTCP_SubdomainUUID, pLocal_PTCP_SubdomainUUID)) /* Remote_PTCP_SubdomainUUID == Local_PTCP_SubdomainUUID */
				{
					RtClass2_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
				}
			}
		}
	}

	OHA_FUNCTION_TRACE_06(0, LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): RtClass2_TopoState(%u) [CableDelayDifferenceLT50ns(%u) Remote_ReservedInterval(0x%X) pLocal_PTCP_SubdomainUUID(%08X) pRemote_PTCP_SubdomainUUID(%08X)]"
		, PortId, RtClass2_TopoState
		, CableDelayDifferenceLT50ns, Remote_ReservedInterval
		, pLocal_PTCP_SubdomainUUID, pRemote_PTCP_SubdomainUUID
		);

    /****************************************************************************/
    /* RtClass3_TopoState: see RTC3PSM (IEC61158 V2.3) and AP00859205           */
    /*                                                                          */
    /* Note: the AP00539134 (only sync forwardig) is obsolete (V4.2i2.1)        */
    /****************************************************************************/

    RtClass3_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;

	if (OHA_RemoteChassisIdIsIdentical(NICId, PortId, SenderId, pTopoDB)) /* Remote_NAME == Expected_NAME */
	{
		if (OHA_RemotePortIdIsIdentical(NICId, PortId, SenderId, pTopoDB)) /* Remote_PORT == Expected_PORT */
		{
			if (RemoteMAUTypeIsValid) /* FD and >= 100Mbps */
			{
				if (DummyDelayResponder) /* note: must not evaluate Local_LineDelayNs! */
				{
					RtClass3_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
				}
				else
				{
					if (CableDelayDifferenceLT50ns) /* ABS(Local_CableDelay - Remote_CableDelay) < 50ns */
					{
						if (pTopoDB->LineDelayFactor == 0) /* no CheckLineDelay block */
						{
							RtClass3_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
						}
						else if (Local_LineDelayNs <= pTopoDB->LineDelayFactor) /* Local_LineDelay <= Expected_LineDelay */
						{
							RtClass3_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
						}
					}
				}
			}
		}
		else
		{
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u) RtClass3_TopoState: Remote_PORTID not existing or not the Expected_PORTID", PortId);
		}
	}
	else
	{
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u) RtClass3_TopoState: Remote_CHASSISID not existing or not the Expected_CHASSISID", PortId);
	}

	OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): RtClass3_TopoState(%u) [DummyDelayResponder() CableDelayDifferenceLT50ns(%u) Local_LineDelayNs(%u) Expected_LineDelay(%u) RemoteMAUTypeIsValid(%u)]"
		, PortId, RtClass3_TopoState
		, CableDelayDifferenceLT50ns
		, Local_LineDelayNs, pTopoDB->LineDelayFactor
		, RemoteMAUTypeIsValid
		);

    /****************************************************************************/
    /* RtClass3_PDEVTopoState: see RTC3PSM (IEC61158 V2.3) and AP00859205       */
    /*                                                                          */
    /* Note: the AP00539134 (only sync forwardig) is obsolete (V4.2i2.1)        */
    /****************************************************************************/

    RtClass3_PDEVTopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;

	if (OHA_IS_NOT_NULL(pExpected_IRDataUUID) && /* must exist */
		OHA_IS_NOT_NULL(pRemote_IRDataUUID)   && /* must exist */
		OHA_ARE_EQUAL(pRemote_IRDataUUID, pExpected_IRDataUUID)) /* Remote_IRDataUUID == Expected_IRDataUUID */
	{
		/* note that for the following SyncId0_TopoState cannot be reused (too weak) */
		if (OHA_IS_NOT_NULL(pLocal_PTCP_SubdomainUUID)) /* must exist */
		{
			if (OHA_IS_NULL(pRemote_PTCP_SubdomainUUID) || /* need not exist */
				OHA_ARE_EQUAL(pRemote_PTCP_SubdomainUUID, pLocal_PTCP_SubdomainUUID)) /* Remote_PTCP_SubdomainUUID == Local_PTCP_SubdomainUUID */
			{
				if( DummyDelayResponder || CableDelayDifferenceLT50ns ) /* see RTC3PSM macro "SYNC" */
				{
					/* AP00966961, AP00956283: do not leave RUN in case of a sync master change */ 
					/* check sync master only if RTC3PSM states <> RUN (AP00966961) */
					/* we've the transition local MAC is new and remote MAC is old too, */
					/* so the check of the MAC is obsolete (AP00965617, AP00965799) */
					if (((Remote_IRTPortStatus & OHA_PORT_STATUS_CLASS3_MASK) == OHA_PORT_STATUS_CLASS3_RUNNING) &&
						(Local_IRTPortStatus == OHA_PORT_STATUS_CLASS3_RUNNING))
					{
						RtClass3_PDEVTopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
					}
					else if (OHA_IS_NULL(pLocal_SyncMaster_MAC) || /* need not exist, see "SPSM" and "Sync forwarding rules" */
							 OHA_IS_NULL(pRemote_SyncMaster_MAC) || /* need not exist, see RTC3PSM macro "ENGINEERING" */
							 OHA_ARE_EQUAL(pRemote_SyncMaster_MAC, pLocal_SyncMaster_MAC)) /* Remote_SyncMaster_MAC == Local_SyncMaster_MAC */
					{
						RtClass3_PDEVTopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
					}
				}
			}
		}
	}

	OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): RtClass3_PDEVTopoState(%u) [pExpected_IRDataUUID(%08X) pRemote_IRDataUUID(%08X) pLocal_PTCP_SubdomainUUID(%08X) pRemote_PTCP_SubdomainUUID(%08X)]"
		, PortId, RtClass3_PDEVTopoState
		, pExpected_IRDataUUID, pRemote_IRDataUUID
		, pLocal_PTCP_SubdomainUUID, pRemote_PTCP_SubdomainUUID
		);

    /****************************************************************************/
    /* (ii) Remote RT_CLASS_3 State (OFF, TX, RXTX)                             */
    /****************************************************************************/

    switch (Remote_IRTPortStatus & OHA_PORT_STATUS_CLASS3_MASK) /* only the bits RTClass3_PortStatus.RTClass3 */
    {
        case OHA_PORT_STATUS_CLASS3_OFF:
            RtClass3_RemoteState = EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF;
            break;
        case OHA_PORT_STATUS_CLASS3_UP:
            RtClass3_RemoteState = EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX;
            break;
        case OHA_PORT_STATUS_CLASS3_RUNNING:
            RtClass3_RemoteState = EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX;
            break;
        default:
            OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u) RtClass3_RemoteState: Not a (valid) Remote_IRTPortStatus(0x%X) received!", PortId, Remote_IRTPortStatus);
            RtClass3_RemoteState = EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF;
            break;
    }

	OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): RtClass3_RemoteState(%u) 0=off 1=tx 2=rxtx"
		, PortId, RtClass3_RemoteState
		);

    /****************************************************************************/
    /* Neighbour_TopoState: see RTC3PSM (IEC61158 V2.3),                        */
    /*                      'Truth table for shortening of the preamble'        */
    /****************************************************************************/

    Neighbour_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;

	if (OHA_RemoteChassisIdIsIdentical(NICId, PortId, SenderId, pTopoDB)) /* Remote_NAME == Expected_NAME */
	{
		if (OHA_RemotePortIdIsIdentical(NICId, PortId, SenderId, pTopoDB)) /* Remote_PORT == Expected_PORT */
		{
			if (RemoteMAUTypeIsValid) /* FD and >= 100Mbps */
			{
				if (OHA_IS_NOT_NULL(pExpected_IRDataUUID) && /* must exist */
					OHA_IS_NOT_NULL(pRemote_IRDataUUID)   && /* must exist */
					OHA_ARE_EQUAL(pRemote_IRDataUUID, pExpected_IRDataUUID)) /* Remote_IRDataUUID == Expected_IRDataUUID */
				{
					Neighbour_TopoState = EDD_SET_REMOTE_PORT_STATE_TOPO_OK;
				}
			}
		}
		else
		{
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u) Neighbour_TopoState: Remote_PORTID not existing or not the Expected_PORTID", PortId);
		}
	}
	else
	{
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_GetRemotePortState (PortId: %u) Neighbour_TopoState: Remote_CHASSISID not existing or not the Expected_CHASSISID", PortId);
	}

	OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		, "OHA_GetRemotePortState (PortId: %u): Neighbour_TopoState(%u) [pExpected_IRDataUUID(%08X) pRemote_IRDataUUID(%08X) RemoteMAUTypeIsValid(%u)]"
		, PortId, Neighbour_TopoState
		, pExpected_IRDataUUID, pRemote_IRDataUUID
		, RemoteMAUTypeIsValid
		);

	/***/

    /* pRemotePortState->PortID = set by caller */
    pRemotePortState->SyncId0_TopoState      = SyncId0_TopoState;
    pRemotePortState->RtClass2_TopoState     = RtClass2_TopoState;
    pRemotePortState->RtClass3_TopoState     = RtClass3_TopoState;
    pRemotePortState->RtClass3_PDEVTopoState = RtClass3_PDEVTopoState;
    pRemotePortState->RtClass3_RemoteState   = RtClass3_RemoteState;
    pRemotePortState->Neighbour_TopoState   = Neighbour_TopoState;
	}

	/*
	// EDDI V4.2i2.1, function EDDI_IrtClass3CalcOutputFromInput()
	// 
	// isLocalSyncOk := EDD_SRV_SET_SYNC_STATE(sync/not-sync)
	// isMauTypeOk   := 100M and FDX
	// isPortStateOk := UP or CLOSED (aka blocked, i.e. not discarding)
	// DomainBoundary := (rtc3_tx_cnt == 0 && rtc3_rx_cnt == 0)
	// 
	// if (
	//      ( isLocalSyncOk )
	//   && ( isMauTypeOk )
	//   && ( isPortStateOk )
	//   && (!DomainBoundary )
	//   && ( RtClass3_TopoState == TOPO_OK ) <--- includes CableDelayDifferenceLT50ns
	// ) {
	//   if ( RtClass3_PDEVTopoState == TOPO_MISMATCH )
	//   {
	//     RtClass3_LocalState = UP;
	//   }
	//   else
	//   {
	//     switch( RtClass3_RemoteState )
	//     {
	//     case EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF:
	//       RtClass3_LocalState = UP;
	//       break;
	// 
	//     case EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX:
	//     case EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX:
	//       RtClass3_LocalState = RUN;
	//       break;
	//     }
	//   }
	// }
	*/
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoRemotePortState                       +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  PortId               : Port                                            +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: determies the RemoteDelay and does a                      +*/
/*+               EDD_SRV_SET_REMOTE_PORT_STATE if something has changed    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DoRemotePortState(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId
)
{
    LSA_UINT16      RetVal  = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoRemotePortState(PortId: %u, pHDBUser: 0x%X)",
                          PortId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);

    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "ParamError (PortId): OHA_DoRemotePortState()");
        RetVal = OHA_ERR_PARAM; /* no valid PortId */
        return(RetVal);
    }

    /* is a topo check for this port started ? */
    if (pHDBUser->TopoState == OHA_TOPO_STATE_START) /* topo check for this user is started ? */
    {
        EDD_RQB_SET_REMOTE_PORT_STATE_TYPE ActualPortState = {0,0,0,0,0,0,0};
        LSA_UINT16 RetVal;
        LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
        EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE pLastPortState;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);
        OHA_ASSERT_NULL_PTR(pHDBEdd);

        if (OHA_IS_NULL(pHDBUser->pTopoDataBase[PortId-1]))
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_DoRemotePortState(): No TopoDataBase existing!");
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoRemotePortState(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        pLastPortState = &pHDBEdd->LastRemotePortState[PortId-1];

        ActualPortState.PortID = PortId; /* rest is filled in by OHA_GetRemotePortState() */

        OHA_GetRemotePortState(NICId, PortId, pHDBUser->pTopoDataBase[PortId-1], &ActualPortState);

        /* is there no difference between the last told port state and the actual port state ? */
        if (pLastPortState->RtClass2_TopoState   == ActualPortState.RtClass2_TopoState &&
            pLastPortState->RtClass3_RemoteState == ActualPortState.RtClass3_RemoteState &&
            pLastPortState->RtClass3_TopoState   == ActualPortState.RtClass3_TopoState &&
            pLastPortState->RtClass3_PDEVTopoState == ActualPortState.RtClass3_PDEVTopoState &&
            pLastPortState->SyncId0_TopoState    == ActualPortState.SyncId0_TopoState &&
			pLastPortState->Neighbour_TopoState  == ActualPortState.Neighbour_TopoState)
        {
			RetVal = OHA_OK;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,"No difference between actual and last port state - O.K.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoRemotePortState(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* store the actual remote port state */
        *pLastPortState = ActualPortState;

        /* ... and tell it to the EDD */
        RetVal = OHA_EDDSetRemotePortState(NICId, ActualPortState);
        if (RetVal != OHA_OK)
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_DoRemotePortState(): SetRemotePortState failed.");
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoRemotePortState(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsWirelessPort                          +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Input/Output          :    LSA_UINT16       PortID                     +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LSA_TRUE  - the port is wireless.                         +*/
/*+               LSA_FALSE - the port is not a wireless one.               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_IsWirelessPort(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortId)
{
    LSA_BOOL IsWireless;

    OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsWirelessPort(NicID: 0x%X, PortId: %u)", NICId, PortId);

    OHA_ASSERT_NULL_PTR(pHDBEdd);

    /* PortID 1 ... n */
    OHA_ASSERT (PortId && (PortId <= OHA_CFG_MAX_PORTS));

    /* at least from PDIRData record */
    IsWireless = pHDBEdd->LinkStatus[PortId-1].IsWireless;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsWirelessPort(OnlyForwarding: 0x%X)", IsWireless);
    return(IsWireless);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoInterface_Gateway            +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compare the interface gateway for mismatch                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_Gateway(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16  RetVal  = OHA_OK;
    LSA_UINT16  CountOfInterfaces = OHA_GetCountOfInterfaces();

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoInterface_Gateway(pHDBUser: 0x%X, pCompare: 0x%X)",
                          pHDBUser, pCompare);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* an existing diagnosis object must DISAPPEAR -> return (default) TRUE */
    *pCompare = LSA_TRUE;

    /* check the interfaces if more then one existing */
	if (CountOfInterfaces > 1)
    {
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
		OHA_COMMON_MEM_U8_PTR_TYPE ThisIpAddrPtr  = LSA_NULL;
		OHA_COMMON_MEM_U8_PTR_TYPE ThisSubnetPtr  = LSA_NULL;
		OHA_COMMON_MEM_U8_PTR_TYPE ThisGatewayPtr = LSA_NULL;

		(LSA_VOID) OHA_DB_READ_IP_SUITE(NICId, &ThisIpAddrPtr, &ThisSubnetPtr, &ThisGatewayPtr);

        /* is there a (valid) IP address ? */
		if (OHA_IS_NULL(ThisIpAddrPtr) || OHA_MEMCMP(ThisIpAddrPtr, (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE))
        {
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_Gateway(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* is there a (valid) gateway address ? */
		if (OHA_IS_NULL(ThisGatewayPtr) || OHA_MEMCMP(ThisGatewayPtr, (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE))
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_Gateway(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
		
        /* is the gateway address valid ? */
		/* AP01184808: HW-Config setting, if IP-address is the same as the IP-address, the router is not used (legacy) */
		if (OHA_MEMCMP(ThisGatewayPtr, ThisIpAddrPtr, OHA_IP_ADDR_SIZE))
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_Gateway(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		/* now we compare with the other interfaces */
		{
			OHA_HDB_EDD_PTR_TYPE  pHDBEdd = OHA_GET_HEDD_PTR(0);
			LSA_UINT32            i = 0;

			while (i < OHA_CFG_MAX_NICS)
			{
				if ( pHDBEdd->State == OHA_HANDLE_STATE_OPEN )
				{
					OHA_ASSERT_NULL_PTR(pHDBEdd->Params.pDetail);
					
					if (NICId != pHDBEdd->Params.pDetail->Params.Edd.NICId) /* not for this If */
					{
						OHA_COMMON_MEM_U8_PTR_TYPE IpAddrPtr  = LSA_NULL;
						OHA_COMMON_MEM_U8_PTR_TYPE SubnetPtr  = LSA_NULL;
						OHA_COMMON_MEM_U8_PTR_TYPE GatewayPtr = LSA_NULL;

						(LSA_VOID)OHA_DB_READ_IP_SUITE(pHDBEdd->Params.pDetail->Params.Edd.NICId, &IpAddrPtr, &SubnetPtr, &GatewayPtr);
						
						if(OHA_IS_NOT_NULL(IpAddrPtr) && OHA_IS_NOT_NULL(GatewayPtr) && 
							(!OHA_MEMCMP(GatewayPtr, (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE)))
						{
							/* AP01184808: HW-Config setting, if IP-address is the same as the IP-address, the router is not used (legacy) */
							if(!OHA_MEMCMP(GatewayPtr, IpAddrPtr, OHA_IP_ADDR_SIZE))
							{
								/* AP01072235: only one gateway is allowed */
								if(!OHA_MEMCMP(GatewayPtr, ThisGatewayPtr, OHA_IP_ADDR_SIZE))
								{
									OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "CompareTopoInterface: Different Gateways (ThisNICId: 0x%X, NICId: 0x%X)", NICId, pHDBEdd->Params.pDetail->Params.Edd.NICId);
									*pCompare = LSA_FALSE;
									break;
								}
							}
						}
					}
				}
				i++;
				pHDBEdd++;
			}
		}
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface(RetVal: 0x%X, CountOfIfs: 0x%X)", RetVal, CountOfInterfaces);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoInterface_NoS                +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compare the interfaces NameOfStation for mismatch         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_NoS(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16  RetVal  = OHA_OK;
    LSA_UINT16  CountOfInterfaces = OHA_GetCountOfInterfaces();

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoInterface_NoS(pHDBUser: 0x%X, pCompare: 0x%X)",
                          pHDBUser, pCompare);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* an existing diagnosis object must DISAPPEAR -> return (default) TRUE */
    *pCompare = LSA_TRUE;

    /* check the interfaces if more then one existing */
	if (CountOfInterfaces > 1)
    {
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
		OHA_COMMON_MEM_U8_PTR_TYPE ThisNamePtr  = LSA_NULL;

		LSA_INT ThisNameLen = OHA_DB_READ_STATION_NAME(NICId, &ThisNamePtr);

        /* is there a (valid) NameOfStation ? */
		if (ThisNameLen == 0 || OHA_IS_NULL(ThisNamePtr))
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_NoS(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		/* now we compare NoS with the other interfaces */
		{
			OHA_HDB_EDD_PTR_TYPE  pHDBEdd = OHA_GET_HEDD_PTR(0);
			LSA_UINT32            i = 0;
			while (i < OHA_CFG_MAX_NICS)
			{
				if ( pHDBEdd->State == OHA_HANDLE_STATE_OPEN )
				{
					OHA_ASSERT_NULL_PTR(pHDBEdd->Params.pDetail);
					
					if (NICId != pHDBEdd->Params.pDetail->Params.Edd.NICId) /* not for this If */
					{
						OHA_COMMON_MEM_U8_PTR_TYPE NamePtr  = LSA_NULL;
						LSA_INT Len = OHA_DB_READ_STATION_NAME(pHDBEdd->Params.pDetail->Params.Edd.NICId, &NamePtr);
						
						if(OHA_IS_NOT_NULL(NamePtr) && Len == ThisNameLen)
						{
							if(OHA_MEMCMP(NamePtr, ThisNamePtr, Len))
							{
								OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "CompareTopoInterface: Same NoS (ThisNICId: 0x%X, NICId: 0x%X)", NICId, pHDBEdd->Params.pDetail->Params.Edd.NICId);
							    *pCompare = LSA_FALSE;
								break;
							}
						}
					}
				}
				i++;
				pHDBEdd++;
			}
		}
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_NoS(RetVal: 0x%X, CountOfIfs: 0x%X)", RetVal, CountOfInterfaces);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoInterface_IP                 +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compare the interfaces IP address for mismatch            +*/
/*+  Interfaces must be in different subnets (for IP stack receive).        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_IP(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16  RetVal  = OHA_OK;
    LSA_UINT16  CountOfInterfaces = OHA_GetCountOfInterfaces();

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoInterface_IP(pHDBUser: 0x%X, pCompare: 0x%X)",
                          pHDBUser, pCompare);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* an existing diagnosis object must DISAPPEAR -> return (default) TRUE */
    *pCompare = LSA_TRUE;

    /* check the interfaces if more then one existing */
	if (CountOfInterfaces > 1)
    {
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
		OHA_COMMON_MEM_U8_PTR_TYPE ThisIpAddrPtr  = LSA_NULL;
		OHA_COMMON_MEM_U8_PTR_TYPE ThisSubnetPtr  = LSA_NULL;

		(LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &ThisIpAddrPtr, &ThisSubnetPtr, LSA_NULL);

        /* is there a (valid) IP address ? */
		if (OHA_IS_NULL(ThisIpAddrPtr) || OHA_IS_NULL(ThisSubnetPtr) || 
			OHA_MEMCMP(ThisIpAddrPtr, (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE))
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_IP(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		/* now we compare with the other interfaces */
		{
			OHA_HDB_EDD_PTR_TYPE  pHDBEdd = OHA_GET_HEDD_PTR(0);
			LSA_UINT32            i = 0;

			LSA_UINT32 this_ip_addr = OHA_GET32_NTOH(ThisIpAddrPtr,0);
			LSA_UINT32 this_subnet  = OHA_GET32_NTOH(ThisSubnetPtr,0);

			while (i < OHA_CFG_MAX_NICS)
			{
				if ( pHDBEdd->State == OHA_HANDLE_STATE_OPEN )
				{
					OHA_ASSERT_NULL_PTR(pHDBEdd->Params.pDetail);
					
					if (NICId != pHDBEdd->Params.pDetail->Params.Edd.NICId) /* not for this If */
					{
						OHA_COMMON_MEM_U8_PTR_TYPE IpAddrPtr  = LSA_NULL;
						OHA_COMMON_MEM_U8_PTR_TYPE SubnetPtr  = LSA_NULL;

						(LSA_VOID)OHA_DB_READ_IP_SUITE(pHDBEdd->Params.pDetail->Params.Edd.NICId, &IpAddrPtr, &SubnetPtr, LSA_NULL);
						
						if(OHA_IS_NOT_NULL(IpAddrPtr) && OHA_IS_NOT_NULL(SubnetPtr) && 
							(!OHA_MEMCMP(IpAddrPtr, (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE)))
						{
							LSA_UINT32 ip_addr = OHA_GET32_NTOH(IpAddrPtr,0);
							LSA_UINT32 subnet  = OHA_GET32_NTOH(SubnetPtr,0);

							/* AP01145324: different interfaces must be in different subnets (necessary for IP stack receive path) */
							/* IFx_Subnet = IFx_IPAddress & IFx_SubnetMask */

							/* 1) check their own subnet mask on their own IP address, if same subnet => error */
							/* if (IF1_Subnet == IF2_Subnet) then FEHLER */
							if((this_ip_addr & this_subnet) == (ip_addr & subnet))
							{
								OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "CompareTopoInterface(check 1): Joint IP (ThisNICId: 0x%X, IP: 0x%X, Subnet: 0x%X, NICId: 0x%X, IP: 0x%X, Subnet: 0x%X)", 
														NICId, this_ip_addr, this_subnet, pHDBEdd->Params.pDetail->Params.Edd.NICId, ip_addr, subnet);
							    *pCompare = LSA_FALSE;
								break;
							}

							/* 2) check this subnet mask on other IP address, if overlap => error */
							/* if (IF1_Subnet == (IF2_Subnet & IF1_SubnetMask)) then FEHLER */
							if((this_ip_addr & this_subnet) == ((ip_addr & subnet) & this_subnet))
							{
								OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "CompareTopoInterface(check 2): Joint IP (ThisNICId: 0x%X, IP: 0x%X, Subnet: 0x%X, NICId: 0x%X, IP: 0x%X, Subnet: 0x%X)", 
														NICId, this_ip_addr, this_subnet, pHDBEdd->Params.pDetail->Params.Edd.NICId, ip_addr, subnet);
							    *pCompare = LSA_FALSE;
								break;
							}

							/* 3) check the other subnet mask on this IP address, if overlap => error */
							/* if  (IF2_Subnet  == (IF1_Subnet & IF2_SubnetMask)) then FEHLER */
							if((ip_addr & subnet) == ((this_ip_addr & this_subnet) & subnet))
							{
								OHA_FUNCTION_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "CompareTopoInterface(check 3): Joint IP (ThisNICId: 0x%X, IP: 0x%X, Subnet: 0x%X, NICId: 0x%X, IP: 0x%X, Subnet: 0x%X)", 
														NICId, this_ip_addr, this_subnet, pHDBEdd->Params.pDetail->Params.Edd.NICId, ip_addr, subnet);
							    *pCompare = LSA_FALSE;
								break;
							}
						}
					}
				}
				i++;
				pHDBEdd++;
			}
		}
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_IP(RetVal: 0x%X, CountOfIfs: 0x%X)", RetVal, CountOfInterfaces);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_PreambleLength          +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the peer Preamble Length (real with expected)    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_PreambleLength(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_PreambleLength(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
        OHA_ASSERT_NULL_PTR(pHDBEdd);

        *pCompare = LSA_FALSE;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

        /* is there a Topo-DB ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL))
        {
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PreambleLength(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* is the Link Up ? */
        {
            LSA_UINT16  CountOfRealPeers;

            /* AP01412678: pcIOX meldet Diagnose bei mehreren LLDP-Nachbarn */
			/* AP01403713: Unerwünschte Diagnose an Port 1 der CPU          */
            LSA_UINT16  Local_IRTPortStatus = pHDBEdd->LinkStatus[PortId-1].IRTPortStatus;

            /* check only if local IRT is supported (IRTPortStatus != OHA_PORT_STATUS_CLASS3_OFF) */
            if (Local_IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                *pCompare = LSA_TRUE;

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,"OHA_CompareTopoPeer_PreambleLength() - IRT not supported (0x%X) !", Local_IRTPortStatus);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PreambleLength(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            /* how many real peers are there ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

            /* are there peers to check (no peers or link down) ? */
            if (CountOfRealPeers == 0)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                /* it'll be finally reduced to a NoPeerDetected diagnosis */
                *pCompare = LSA_TRUE;

                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PreambleLength(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }

        {
            /* look for the corresponding NicId */
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            LSA_UINT16 SenderId;

            OHA_ASSERT_NULL_PTR(pDB);

            SenderId = pDB->LldpData[PortId-1].LldpSenderId;

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                LSA_UINT8  SenderStatus = pDB->LldpData[PortId-1].LldpSenderStatus;

                /* is there a valid sender ? */
                if (SenderStatus == OHA_STATUS_DATA_VALID ||
                    SenderStatus == OHA_STATUS_DATA_INVALID)
                {
                    /* read the PNO-PortStatus */
                    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                    LSA_UINT16 Len = 0;
                    LSA_UINT8  SubType  = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;

                    (LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId, SenderId,
                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           &pVar, &Len);

                    if (OHA_IS_NOT_NULL(pVar) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
                    {
                        /* now we are able to compare the IRTPortStatus */
                        LSA_UINT16  Remote_IRTPortStatus, Remote_IRTPreambleShort;
						LSA_BOOL    Local_IRTPreambleShort = pHDBEdd->LinkStatus[PortId-1].ShortPreamble;

                        pVar += 2;               /* 2nd word */
                        Remote_IRTPortStatus    = OHA_GET16_NTOH (pVar, 0);
						Remote_IRTPreambleShort = OHA_GET16_NTOH (pVar, 0);

                        Remote_IRTPortStatus    &= OHA_PORT_STATUS_CLASS3_MASK;
						Remote_IRTPreambleShort &= OHA_PORT_STATUS_CLASS3_PREAMBLE_MASK;

                        if (!(Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF  ||
                              Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_UP   ||
                              Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_RUNNING))
                        {
                            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"OHA_CompareTopoPeer_PreambleLength(0x%X) - Remote_C3_PortState out of range (0x%X) !", Remote_IRTPreambleShort, Remote_IRTPortStatus);
                        }
						else
						{
							/* both preambles must be identical */
							if ((Remote_IRTPreambleShort && Local_IRTPreambleShort) || 
								((!Remote_IRTPreambleShort) && (!Local_IRTPreambleShort)))
							{
								*pCompare = LSA_TRUE;
							}
							else
							{
								OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_CompareTopoPeer_PreambleLength() - Remote_PreambleShort (0x%X) and Local_PreambleShort (0x%X) different!",Remote_IRTPreambleShort,Local_IRTPreambleShort);
							}
						}
                    }
                    else
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_CompareTopoPeer_PreambleLength() - Remote PortStatus-Tlv not existing!");
                    }
                }
                else
                {
                    /* AP01490571: if the preamble length cannot be read */
                    /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                    *pCompare = LSA_TRUE;
                }
            }
            else
            {
                RetVal = OHA_ERR_PARAM;
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_PreambleLength(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoPeer_Fragmentation           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the peer Fragmentation (real with expected)      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_Fragmentation(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoPeer_Fragmentation(PortId: 0x%X, pHDBUser: 0x%X)",
                          PortId, pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* check the peers */

    if (PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
        OHA_ASSERT_NULL_PTR(pHDBEdd);

        *pCompare = LSA_FALSE;

        OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

        /* is there a Topo-DB ? */
        if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL))
        {
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Fragmentation(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* is the Link Up ? */
        {
            LSA_UINT16  CountOfRealPeers;

            /* AP01412678: pcIOX meldet Diagnose bei mehreren LLDP-Nachbarn */
			/* AP01403713: Unerwünschte Diagnose an Port 1 der CPU          */
            LSA_UINT16  Local_IRTPortStatus = pHDBEdd->LinkStatus[PortId-1].IRTPortStatus;

            /* check only if local IRT is supported (IRTPortStatus != OHA_PORT_STATUS_CLASS3_OFF) */
            if (Local_IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                *pCompare = LSA_TRUE;

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,"OHA_CompareTopoPeer_Fragmentation() - IRT not supported (0x%X) !", Local_IRTPortStatus);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Fragmentation(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            /* how many real peers are there ? */
            CountOfRealPeers = OHA_GetCountOfRealPeers(NICId, PortId);

            /* are there peers to check (no peers or link down) ? */
            if (CountOfRealPeers == 0)
            {
                /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                /* it'll be finally reduced to a NoPeerDetected diagnosis */
                *pCompare = LSA_TRUE;

                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Fragmentation(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }

        {
            /* look for the corresponding NicId */
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            LSA_UINT16 SenderId;

            OHA_ASSERT_NULL_PTR(pDB);

            SenderId = pDB->LldpData[PortId-1].LldpSenderId;

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                LSA_UINT8  SenderStatus = pDB->LldpData[PortId-1].LldpSenderStatus;

                /* is there a valid sender ? */
                if (SenderStatus == OHA_STATUS_DATA_VALID ||
                    SenderStatus == OHA_STATUS_DATA_INVALID)
                {
                    /* read the PNO-PortStatus */
                    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
                    LSA_UINT16 Len = 0;
                    LSA_UINT8  SubType  = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;

                    (LSA_VOID)oha_db_read_rem_lldp_element(NICId, PortId, SenderId,
                                                           OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           &pVar, &Len);

                    if (OHA_IS_NOT_NULL(pVar) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
                    {
                        /* now we are able to compare the IRTPortStatus */
                        LSA_UINT16  Remote_IRTPortStatus, Remote_IRTFragmentation;
						LSA_BOOL    Local_IRTFragmentation = pHDBEdd->LinkStatus[PortId-1].TxFragmentation;

                        pVar += 2;               /* 2nd word */
                        Remote_IRTPortStatus    = OHA_GET16_NTOH (pVar, 0);
						Remote_IRTFragmentation = OHA_GET16_NTOH (pVar, 0);

                        Remote_IRTPortStatus    &= OHA_PORT_STATUS_CLASS3_MASK;
						Remote_IRTFragmentation &= OHA_PORT_STATUS_CLASS3_FRAGMENTATION_MASK;

                        if (!(Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_OFF  ||
                              Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_UP   ||
                              Remote_IRTPortStatus == OHA_PORT_STATUS_CLASS3_RUNNING))
                        {
                            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"OHA_CompareTopoPeer_Fragmentation(0x%X) - Remote_C3_PortState out of range (0x%X) !", Remote_IRTFragmentation, Remote_IRTPortStatus);
                        }
						else
						{
							/* both fragmentations must be identical */
							if ((Remote_IRTFragmentation && Local_IRTFragmentation) || 
								((!Remote_IRTFragmentation) && (!Local_IRTFragmentation)))
							{
								*pCompare = LSA_TRUE;
							}
							else
							{
								OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_CompareTopoPeer_Fragmentation() - Remote_Fragmentation (0x%X) and Local_Fragmentation (0x%X) different!",Remote_IRTFragmentation,Local_IRTFragmentation);
							}
						}
                    }
                    else
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_CompareTopoPeer_Fragmentation() - Remote PortStatus-Tlv not existing!");
                    }
                }
                else
                {
                    /* AP01490571: if the fragmentation cannot be read */
                    /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                    *pCompare = LSA_TRUE;
                }
            }
            else
            {
                RetVal = OHA_ERR_PARAM;
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoPeer_Fragmentation(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DbGetGlobalDiagMode                     +*/
/*+  Input/Output          :    -                                           +*/
/*+  Result                :    Global MultipleInterfaceMode for diagnosis  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+ Description: Gets MultipleInterfaceMode from a Database-Interface-Entry +*/
/*+ MultipleInterfaceMode fron any interface is returned if the bit         +*/
/*+ MultipleInterfaceMode.NameOfDevice is set and the record                +*/
/*+ MPDInterfaceAdjust is written to this interface.                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR OHA_DbGetGlobalDiagMode(
    OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfMode_0,
    OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfMode_1)
{
    OHA_DB_IF_PTR_TYPE    pDB    = &oha_database.IfData[0];
    LSA_UINT32            i      = 0;
    LSA_UINT32            IfMode = 0;
	LSA_UINT16            CountOfMode_0 = 0;
	LSA_UINT16            CountOfMode_1 = 0;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DbGetGlobalDiagMode()");

    while ( i < OHA_CFG_MAX_NICS )
    {
		if (pDB->MultipleIfModeExisting && pDB->MultipleIfModeSetValid)
		{
			if (pDB->MultipleIfModeValid & OHA_NAME_OF_DEVICE_MASK)
			{
				IfMode = OHA_NAME_OF_DEVICE_MASK;
				CountOfMode_1++;
			}
			else
			{
				CountOfMode_0++;
			}
		}
        pDB++;
        i++;
    }

	if(OHA_IS_NOT_NULL(pCountOfMode_0))
	{
		*pCountOfMode_0 = CountOfMode_0;
	}
	if(OHA_IS_NOT_NULL(pCountOfMode_1))
	{
		*pCountOfMode_1 = CountOfMode_1;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DbGetGlobalDiagMode(IfMode: 0x%X)", IfMode);
    return(IfMode);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoInterface_NoD                +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compare the interfaces NameOfDevice mode for mismatch     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_NoD(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16  RetVal  = OHA_OK;
    LSA_UINT16  CountOfInterfaces = OHA_GetCountOfInterfaces();

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoInterface_NoD(pHDBUser: 0x%X, pCompare: 0x%X)",
                          pHDBUser, pCompare);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* an existing diagnosis object must DISAPPEAR -> return (default) TRUE */
    *pCompare = LSA_TRUE;

    /* check the interfaces if more than one opened */
	/* and the interface mode if at minimum one is existing */
	if (CountOfInterfaces > 1)
    {
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;
        OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NICId);

        OHA_ASSERT_NULL_PTR(pDB);

		/* is a diagnosis to build for this interface? */
		/* TIA 833359: only if record PDInterfaceAdjust written (this interface) */
		if (pDB->MultipleIfModeExisting && pDB->MultipleIfModeSetValid)
		{
			LSA_UINT16  CountOfMode_0 = 0;
			LSA_UINT16  CountOfMode_1 = 0;

			(LSA_VOID)OHA_DbGetGlobalDiagMode(&CountOfMode_0, &CountOfMode_1);

			/* are there different NameOfDevice modes set ? */
			if (CountOfMode_0 && CountOfMode_1)
			{
				OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "CompareTopoInterface: Different IEEE NoD modes (Mode_0: 0x%X, Mode_1: 0x%X)", CountOfMode_0, CountOfMode_1);
				*pCompare = LSA_FALSE;
			}
		}
		else
		{
			RetVal = OHA_ERR_PARAM;
		}
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoInterface_NoD(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*****************************************************************************/
/*  end of file OHA_TOPO.C                                                   */
/*****************************************************************************/

