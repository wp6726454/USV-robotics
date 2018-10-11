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
/*  F i l e               &F: oha_prm.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA param-interface functions                    */
/*                                                                           */
/*                          - OHA_RequestPrmStart()                          */
/*                          - OHA_RequestPrmWrite()                          */
/*                          - OHA_RequestPrmEnd()                            */
/*                          - OHA_RequestPrmCommit()                         */
/*                          - OHA_RequestPrmRead()                           */
/*                          - OHA_RequestPrmIndication()                     */
/*                                                                           */
/*  +---+                                                       +---+        */
/*  |   | OHA_OPC_PRM_PREPARE (new temporary topology to write) |   |        */
/*  |   |------------------------------------------------------>|   |        */
/*  |   | OHA_OPC_PRM_WRITE (writes temporary topology)         |   |        */
/*  | C |------------------------------------------------------>| O |        */
/*  |   | OHA_OPC_PRM_END (no action, attached to CMPD)         |   |        */
/*  | M |------------------------------------------------------>| H |        */
/*  |   | OHA_OPC_PRM_COMMIT ((re)starts topology)              |   |        */
/*  | P |------------------------------------------------------>| A |        */
/*  |   | OHA_OPC_PRM_READ (reads port records)                 |   |        */
/*  | D |------------------------------------------------------>|   |        */
/*  |   | OHA_OPC_PRM_INDICATION (indicates TOPO changes)       |   |        */
/*  |   |<------------------------------------------------------|   |        */
/*  +---+                                                       +---+        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  36
#define OHA_MODULE_ID      LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_PRM */

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

LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_PrmReadPDEVStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/

/* write a PRM BlockHeader */
LSA_VOID oha_prm_write_header(OHA_UPPER_MEM_U8_PTR_TYPE pRecordData, LSA_UINT16 block_type, 
							  LSA_UINT32 block_length, LSA_UINT16 block_version,
							  LSA_BOOL block_alignment)
{
	OHA_ASSERT_NULL_PTR(pRecordData);
	OHA_PDU_SET_U16(pRecordData, block_type);
	/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
	OHA_PDU_SET_U16(pRecordData, (LSA_UINT16)(block_length - 2*sizeof(LSA_UINT16)));
	OHA_PDU_SET_U16(pRecordData, block_version);
	if(block_alignment)
	{
        OHA_PDU_SET_U16 (pRecordData, 0x0);         /* 2 padding bytes    */
	}
    LSA_UNUSED_ARG(pRecordData);    /* not used anymore */
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPrmPrepare                       +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_PREPARE             +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmPrepare                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to prepare PRM for write                          +*/
/*+               Stop and Remove the Topo (all ports)                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmPrepare(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId    = pHDBUser->Params.pDetail->Params.User.NICId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmPrepare(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    if (pHDBUser->TopoState == OHA_TOPO_STATE_CLOSE) /* channel is not registered ? */
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_PREPARE failed. User-Channel not registered for Topo");
        Response = OHA_ERR_SEQUENCE;
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmPrepare(Response: 0x%X)", Response);
        return;
    }

    /* Check if we have LLDP and a EDD-Channel for NIC-ID. */
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);  /* only EDD and LLDP for this request necessary */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_PREPARE failed. No EDD-Channel for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if ( LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL))
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_PREPARE failed. No LLDP-Channel present (NIC-ID: 0x%X)",NICId);
    }
    else
    {
        OHA_DoPrmPrepare(pHDBUser);
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmPrepare(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                          OHA_DoPrmPrepare                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_DoPrmPrepare(
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16      NICId;

    OHA_HDB_GSY_PTR_TYPE    pHDBGsy;
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp;
    LSA_UINT16 PortId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoPrmPrepare(pHDBUser: 0x%X)",
                          pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    NICId    = pHDBUser->Params.pDetail->Params.User.NICId;
    pHDBGsy  = OHA_GSYGetHandleByNIC(NICId);
    pHDBDcp  = OHA_DCPGetHandleByNIC(NICId);

    OHA_ASSERT_NULL_PTR(pHDBGsy);
    OHA_ASSERT_NULL_PTR(pHDBDcp);
    OHA_ASSERT(pHDBUser->TopoState != OHA_TOPO_STATE_CLOSE);

    pHDBUser->PrmState  = OHA_PRM_STATE_PREPARE; /* a PrmPerpare is done  */

    /* delete the temporary Topo database */
    /* free buffered TOPO-data entries */
    for (PortId = 1; PortId <= OHA_CFG_MAX_PORTS; PortId++)  /* over all ports */
    {
        OHA_FreeTopoDB(pHDBUser->pPrmTopoBase[PortId-1]);
        pHDBUser->pPrmTopoBase[PortId-1] = LSA_NULL;
    }

    /* remove the temp sync data */
    OHA_MEMSET(&pHDBGsy->SyncDataWrite, 0, sizeof(OHA_SYNC_DATA_TYPE));

    /* remove the FSU data */
    OHA_MEMSET(&pHDBDcp->FSU_DataWrite, 0, sizeof(OHA_HELLO_DATA_TYPE));

#ifndef OHA_CFG_NO_REMA
    (LSA_UINT16)oha_rema_prm_prepare(NICId, LSA_FALSE);
#endif

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoPrmPrepare()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPrmEnd                           +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_END                 +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmPrepare                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to signal the end of PRM write                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmEnd(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp;

    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmEnd(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    /* Check if we have a EDD-Channel for the NIC-ID. */
    pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
    pHDBDcp = OHA_DCPGetHandleByNIC(NICId);

    if (OHA_IS_NULL(pHDBEdd) || OHA_IS_NULL(pHDBDcp))
    {
        Response = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_END failed. No EDD- or DCP-Channel present (NIC-ID: 0x%X)",NICId);

        OHA_UserRequestFinish(pHDBUser,pRQB,Response); /*an error occurred */
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmEnd(Response: 0x%X)", Response);
        return;
    }

	/* TFS 1028289: check, if PortIDs written and valid */
    if (!(oha_port_names_valid(NICId)))
    {
        Response = OHA_ERR_PRM_CONSISTENCY;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_END failed. No (valid) LlldpPortID set (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response); /*an error occurred */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmEnd(Response: 0x%X)", Response);
        return;
    }

    if (pHDBUser->TopoState == OHA_TOPO_STATE_CLOSE)   /* channel is not registered */
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_END failed. Channel not registered!");

        OHA_UserRequestFinish(pHDBUser,pRQB,Response); /*an error occurred */
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmEnd(Response: 0x%X)", Response);
        return;
    }
    if (pHDBUser->PrmState != OHA_PRM_STATE_PREPARE)   /* PrmCommit is not done */
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_PRM_STATE_PREPARE is not done before!");

        OHA_UserRequestFinish(pHDBUser,pRQB,Response); /*an error occurred */
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmEnd(Response: 0x%X)", Response);
        return;
    }

    if (pHDBUser->TopoReqPendingCnt != 0) /* queued request */
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* only one request ! */
    }

#ifndef OHA_CFG_NO_NARE
    /* Stop a possible running Hello.Req */
    /*  PrmEnd and HelloState == HelloSent -> do a Hello.Req(Stop) */
    if (pHDBDcp->HelloState == OHA_DCP_STATE_HELLO_SENT)
    {
        OHA_RQB_PUT_BLOCK_TOP(pHDBUser->TopoUserReq.pBottom,
                              pHDBUser->TopoUserReq.pTop,
                              pRQB);

        pHDBUser->TopoReqPendingCnt++;      /* Number of pend. User-PRM-requests */

        Response = OHA_NAREHelloStop(NICId, (LSA_UINT16)(OHA_RQB_GET_HANDLE(pRQB)));

        if (Response != OHA_OK)
        {
            OHA_TOPOUserCheckPending(pHDBUser, Response);   /* confirm an error */
        }

        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmEnd(Response: 0x%X)", Response);
        return;
    }
#endif

#ifndef OHA_CFG_NO_REMA
    (LSA_UINT16)oha_rema_prm_end(NICId);
#endif

    /* after PRM_END now */
    pHDBUser->PrmState = OHA_PRM_STATE_END; /* a PrmEnd is done */
    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

	/* patch for AP00916191: */
	/* OhaPrmEnd is done and the new IRDataUUID is different, send LLDP as {0} until OhaPrmCommit */
	oha_update_sync_status(NICId);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmEnd(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPrmCommit                        +*/
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
/*+     OHA_RQB_ARGS_TYPE      args.PrmPrepare                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check request to takeover the temporary written PRM data  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmCommit(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmCommit(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    if (pHDBUser->TopoState == OHA_TOPO_STATE_CLOSE)    /* channel is registered ? */
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_COMMIT failed. User-Channel not registered for Topo");
        Response = OHA_ERR_SEQUENCE;
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmCommit(Response: 0x%X)", Response);
        return;
    }
    if (pHDBUser->PrmState != OHA_PRM_STATE_END)    /* a PrmCommit without PrmEnd ? */
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_COMMIT without OHA_OPC_PRM_END failed.");
        Response = OHA_ERR_SEQUENCE;
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmCommit(Response: 0x%X)", Response);
        return;
    }
    if (pHDBUser->TopoReqPendingCnt) /* no queueing of user-requests in this version */
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_COMMIT failed. Already one PrmCommit-Request active(Response: 0x%X)", Response);
        Response = OHA_ERR_SEQUENCE;
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmCommit(Response: 0x%X)", Response);
        return;
    }

    if (pRQB->args.PrmControl.is_local != LSA_FALSE && pRQB->args.PrmControl.is_local != LSA_TRUE) /* sanity */
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_COMMIT failed. Invalid param. (is_local: 0x%X)", pRQB->args.PrmControl.is_local);
        Response = OHA_ERR_PARAM;
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmCommit(Response: 0x%X)", Response);
        return;
    }

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    /* Check if we have LLDP and a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);  /* EDD for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL))
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_COMMIT failed. No EDD-Channel present (NIC-ID: 0x%X)",NICId);
    }
	else
    {
#if 0
        OHA_HDB_GSY_PTR_TYPE    pHDBGsy = OHA_GSYGetHandleByNIC(NICId);

        OHA_ASSERT_NULL_PTR(pHDBGsy);

        /* we have to take a look if there's a PDSyncData record (RTC) written */
        if (pHDBGsy->SyncDataWrite.Mode == OHA_SYNC_ADD &&
            pHDBGsy->SyncDataWrite.RecordIndex == OHA_RECORD_INDEX_PD_SYNC_DATA_RTC)
        {
#endif

            /* get the actual red period - call EddGetPortParam(s) first   */
            /* or in case of changing the configuration ("Umprojektieren") */

            pHDBUser->TopoState = OHA_TOPO_STATE_STOP; /* no checking of topo data  */

            /* queue the PrmCommit request in the USER-HDB */

            if (pHDBUser->TopoReqPendingCnt != 0) /* queue the request */
            {
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* only one request ! */
            }

            OHA_RQB_PUT_BLOCK_TOP(pHDBUser->TopoUserReq.pBottom,
                                  pHDBUser->TopoUserReq.pTop,
                                  pRQB);

            pHDBUser->TopoReqPendingCnt++;      /* Number of pend. User-PRM-requests */

            /* AP01528425: First get LengthOfPeriod from EddGetParams */
            Response = OHA_EDDGetParams(pHDBEdd, (LSA_UINT16)(OHA_RQB_GET_HANDLE(pRQB)));
            if ( Response != OHA_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"PRM Issuing GetParamsRequest(Response: 0x%X) failed.", Response);

                /* call the request handler for EddGetPortParam */
                OHA_TOPORequestHandler(pHDBUser, NICId, OHA_OK); /* call the prm-handler */
            }

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmCommit(Response: 0x%X)", Response);
            return;
#if 0
        }
        else    /* takeover the temporary written PRM data */
        {
            OHA_PrmCommit(pRQB, pHDBUser);
        }
#endif
    }

    if (Response != OHA_OK)
    {
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmCommit(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmCommit                               +*/
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
/*+     OHA_RQB_ARGS_TYPE      args.PrmPrepare                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to takeover the temporary written PRM data        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmCommit(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser
)
{
    LSA_UINT16  NICId;
    LSA_UINT16 PortId;
    LSA_UINT16 Response = OHA_OK;
    OHA_HDB_GSY_PTR_TYPE pHDBGsy;
    OHA_HDB_LLDP_PTR_TYPE pHDBLldp;
    OHA_HDB_DCP_PTR_TYPE pHDBDcp;
    OHA_HDB_EDD_PTR_TYPE pHDBEdd;
    LSA_BOOL IsLocalCommit;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmCommit(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    IsLocalCommit = pRQB->args.PrmControl.is_local;
    NICId = pHDBUser->Params.pDetail->Params.User.NICId;
    pHDBGsy = OHA_GSYGetHandleByNIC(NICId);
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);
    pHDBDcp = OHA_DCPGetHandleByNIC(NICId);
    pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_ASSERT_NULL_PTR(pHDBGsy);
    OHA_ASSERT_NULL_PTR(pHDBLldp);
    OHA_ASSERT_NULL_PTR(pHDBDcp);
    OHA_ASSERT_NULL_PTR(pHDBEdd);

    /* copy temporary Topo-Data to Topo-DB */
    /* by atomic sequence: TopoStop / Delete (A) / Copy (B) to (A) / Delete (B) / TopoStart */

    /* (i) TopoStop for all ports */
    OHA_InitTopo(pHDBUser);

    pHDBUser->TopoState = OHA_TOPO_STATE_STOP; /* no checking of topo data  */
    pHDBUser->PrmState  = OHA_PRM_STATE_COMMIT; /* a PrmCommit is done */

    /* (ii) Remove All */
    /* free allocated TOPO-data entries */
    for (PortId = 1; PortId <= OHA_CFG_MAX_PORTS; PortId++)  /* over all ports */
    {
        OHA_FreeTopoDB(pHDBUser->pTopoDataBase[PortId-1]);
        pHDBUser->pTopoDataBase[PortId-1] = LSA_NULL;
    }

    /* (iii) copy Temp-Topo to Topo-DB */
    for (PortId = 1; PortId <= OHA_CFG_MAX_PORTS; PortId++)  /* over all ports */
    {
#if 0
        /* alloc the TopoDB if temp topo data or remote LLDP_PNIO_PORTSTATUS exists */
        if (OHA_IS_NOT_NULL(pHDBUser->pPrmTopoBase[PortId-1]))
#endif
            /* alloc the TopoDB at any rate */
        {
            LSA_UINT16 Ret;
            OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

            if (LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL))
            {
                OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;    /* variable in octets */

                /* alloc the db for topo */
                OHA_ALLOC_LOCAL_MEM(&pVarTemp, sizeof(OHA_DB_TOPO_TYPE));  /* alloc the topo DB */
                if ( LSA_HOST_PTR_ARE_EQUAL(pVarTemp, LSA_NULL) )
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_COMMIT: Allocating TOPO-DB failed.");
                    Response = LSA_RET_ERR_RESOURCE;
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmCommit(Response: 0x%X)", Response);
                }
                pTopoDB = (OHA_DB_TOPO_PTR_TYPE)pVarTemp;
                OHA_MEMSET(pTopoDB, 0, sizeof(OHA_DB_TOPO_TYPE));
                pHDBUser->pTopoDataBase[PortId-1] = pTopoDB;
            }

            /* ... and copy check data */
            if (OHA_IS_NOT_NULL(pHDBUser->pPrmTopoBase[PortId-1]))
            {
                Ret = OHA_CopyTopoData(pHDBUser->pTopoDataBase[PortId-1], pHDBUser->pPrmTopoBase[PortId-1]);
                if (Ret != LSA_RET_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_PrmCommit: OHA_CopyTopoData failed (Ret: 0x%X).", Ret);
                }
            }

            /* copy adjust data */
            Ret = OHA_CopyAdjustData(pHDBGsy, pHDBUser, PortId);
            if (Ret != LSA_RET_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_PrmCommit: OHA_CopyAdjustData failed (Ret: 0x%X).", Ret);
            }
        }
    }

    /* copy adjust interface data */
    OHA_ASSERT_NULL_PTR(pHDBLldp->pDB);
	pHDBLldp->pDB->MultipleIfModeValid = pHDBLldp->pDB->MultipleIfModeTemp;
	pHDBLldp->pDB->MultipleIfModeTemp  = 0;
	pHDBLldp->pDB->MultipleIfModeSetValid = pHDBLldp->pDB->MultipleIfModeSetTemp;
	pHDBLldp->pDB->MultipleIfModeSetTemp  = 0;
	pHDBLldp->pDB->MultipleIfModeExisting = LSA_TRUE;

    /* copy sync data if exists */
    if (OHA_OK != OHA_CopySyncData(pHDBGsy, pHDBUser))
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_PrmCommit: OHA_CopySyncData failed.");
    }

    /* copy FSU data (hello) if exists */
    if (LSA_RET_OK != OHA_DB_WRITE_DEVICE_INITIATIVE(NICId,
                                                     pHDBDcp->FSU_DataWrite.HelloMode,
                                                     pHDBDcp->FSU_DataWrite.HelloRetry,
                                                     pHDBDcp->FSU_DataWrite.HelloInterval,
                                                     pHDBDcp->FSU_DataWrite.HelloDelay ))
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_PrmCommit: OHA_DB_WRITE_DEVICE_INITIATIVE failed.");
    }

	if( IsLocalCommit )
	{
		/* copy PNIdent data */
		pHDBLldp->pDB->PNIdentValid = pHDBLldp->pDB->PNIdentTemp;
	}
	else
	{
		/* do nothing (usecase TIA 833256: keep values when parameterized remotely) */
	}

    /* delete the temp sync data */
    OHA_MEMSET(&pHDBGsy->SyncDataWrite, 0, sizeof(OHA_SYNC_DATA_TYPE));

    /* delete the temp FSU data */
    OHA_MEMSET(&pHDBDcp->FSU_DataWrite, 0, sizeof(OHA_HELLO_DATA_TYPE));
	
    /* delete the temp PNIdent data */
    OHA_MEMSET(&pHDBLldp->pDB->PNIdentTemp, 0, sizeof(OHA_PN_IDENT_TYPE));

    /* (iv) delete the temporary Topo database */
    /* free buffered TOPO-data entries */
    for (PortId = 1; PortId <= OHA_CFG_MAX_PORTS; PortId++)  /* over all ports */
    {
        OHA_FreeTopoDB(pHDBUser->pPrmTopoBase[PortId-1]);
        pHDBUser->pPrmTopoBase[PortId-1] = LSA_NULL;
    }

    /* (v) Topo Start */
    pHDBUser->TopoState = OHA_TOPO_STATE_START; /* checking of topo data  */
    pHDBUser->PrmState  = OHA_PRM_STATE_PREPARE; /* a PrmCommit is done */

    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"OHA_OPC_PRM_COMMIT successful");

    /* we handle the timer in case of OHA_OPC_PRM_COMMIT (and OHA_OPC_WRITE_PTCP_DATA) */
    /* STOP if running and START if there's a PDSyncData record written */
    for (PortId = 0; PortId < OHA_CFG_MAX_PORTS && PortId < pHDBLldp->pDB->PortCnt; PortId++)  /* over all ports */
    {
        /* stop the PTCP timer if running */
        if (pHDBUser->PtcpTimerInUse[PortId])
        {
            /* stop the PTCP timer */
            pHDBUser->PtcpTimerInUse[PortId] = LSA_FALSE;
            OHA_TimerStop(pHDBUser->PtcpTimerID[PortId]);
        }

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

    /* a PrmCommit shall start the LLdpSend, therfore we've to create a local RQB */
    {
        OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NICId);
        /* #ifdef OHA_LLDP_SEND_BY_PRM_COMMIT */
        OHA_UPPER_RQB_PTR_TYPE pRQBLocal = LSA_NULL;

        OHA_ASSERT_NULL_PTR(pHDBDcp);

        OHA_ALLOC_UPPER_RQB_LOCAL(&pRQBLocal,sizeof(OHA_RQB_TYPE));

        if ( LSA_HOST_PTR_ARE_EQUAL(pRQBLocal, LSA_NULL) )
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_PrmCommit(). Allocation RQB memory failed.");
            OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);
        }

        OHA_MEMCPY(pRQBLocal, pRQB, sizeof(OHA_RQB_TYPE));
        /* #endif */

        /* first the user-indication by demand */
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);

        /* OHA_InitTopo(pHDBUser, port_field); */

        /* the TOPO-indication for one user */
        /* duty: if a PrmCommit is done, the subcomponent has to indicate the new diag state */
        /* by an indication. If this is an empty one we have to indicate too by diag_cnt = 0 */
        (LSA_VOID)OHA_TopoIndicateInterface(pHDBUser, LSA_TRUE);

		{	/* AP01166830 */
			OHA_HDB_USER_PTR_TYPE pHDBIfUser;
			LSA_UINT16 i;
			for (pHDBIfUser = OHA_GET_HUSER_PTR(0),i=0; i < OHA_CFG_MAX_CHANNELS; pHDBIfUser++,i++)
			{
				if (OHA_IS_NOT_NULL(pHDBIfUser) && (pHDBIfUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
				{
					if (!(LSA_HOST_PTR_ARE_EQUAL(pHDBIfUser, pHDBUser)))
					{
						/* the TOPO-indication for other interfaces and user */
						(LSA_VOID)OHA_TopoIndicateInterface(pHDBIfUser, LSA_FALSE);
					}
				}
			}
		}

        for (PortId = 1; PortId <= OHA_CFG_MAX_PORTS && PortId <= pHDBLldp->pDB->PortCnt; PortId++)  /* over all ports */
        {
            (LSA_VOID)OHA_TopoIndicatePort(pHDBUser, PortId, LSA_TRUE, LSA_TRUE);
        }

        /* update, send the PTCP-status TLV and build the PTCP diagnosis (MasterSourceAddress := 0) */
        oha_update_sync_status(NICId);

        /* #ifdef OHA_LLDP_SEND_BY_PRM_COMMIT */
        /* call  LLDP-Start */
        OHA_RequestLldpStartSend (pRQBLocal, pHDBUser);
        /* #endif */

#ifndef OHA_CFG_NO_NARE
        /* Do a HELLO.req if Hello is written and at least one Port is in state LinkUp */
        if (IsLocalCommit)  /* is a hello to send ? */
        {
            if (pHDBDcp->HelloState == OHA_DCP_STATE_HELLO_WRITTEN)
            {
                LSA_UINT16 RetVal = OHA_DoHello(NICId, OHA_ADDITIONAL_HELLO_DELAY_BY_LINK_UP); /* AP01242778 */

                if (RetVal != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_PrmCommit(). Do HELLO.Req failed.");
                }
            }
        }
#endif

        /* store the (actual) PRM-User */
        pHDBEdd->pHDBUserPrm = pHDBUser;

        /* now we inform the EDD about the remote port states (EDD_SRV_SET_REMOTE_DELAY) */
        for (PortId = 1; PortId <= OHA_CFG_MAX_PORTS && PortId <= pHDBLldp->pDB->PortCnt; PortId++)  /* over all ports */
        {
            LSA_UINT16 RetVal = OHA_DoRemotePortState(pHDBUser, PortId);

            if (RetVal != OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_PrmCommit(). OHA_DoRemotePortState failed");
            }
        }

#ifndef OHA_CFG_NO_REMA
		{
	        LSA_BOOL do_rtf = LSA_TRUE;	/* AP00968621 */
	        (LSA_UINT16)oha_rema_prm_commit(NICId, pHDBUser->ThisHandle, do_rtf);
		}
#endif

		{
		    OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

			if(OHA_IS_NOT_NULL(pHDBEdd))
			{
				if (pHDBEdd->PendingPortId == 0)	/* interface */
				{
					(LSA_UINT16)OHA_SOCKGetMib2Status(NICId, pHDBEdd->PendingPortId, pHDBEdd->ThisHandle);
				}
			}
		}
    }
    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmCommit()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPrmWrite                         +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmPrepare                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write PRM data in a temporary database         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmWrite(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
	LSA_UINT16	NICId;
	LSA_UINT32  RecordIndex;
	LSA_UINT16 Response;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmWrite(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

	NICId = pHDBUser->Params.pDetail->Params.User.NICId;
	RecordIndex = pRQB->args.PrmData.record_index;

	if (!OHA_EDDCheckNICUsed(NICId))
    {
        /* Error: no such interface. Abort. */
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No EDD-Channel for this NicId (NIC-ID: 0x%X)",NICId);

		OHA_UserRequestFinish(pHDBUser,pRQB,Response);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmWrite(RecordIndex: 0x%X)", pRQB->args.PrmData.record_index);
	}

    /* we've three type of records: "PDEV records", "Station records"  */
	/* and REMA records.                                               */
    /* - PDEV records are depending on a registered topo channel       */
    /* - Station and SNMP records are independant of any registration  */
    /*   they do not have the A -> B buffering, they set NoS or IP immediately */

    switch (RecordIndex)
    {
        case OHA_RECORD_INDEX_MULTIPLE_STATION:
        case OHA_RECORD_INDEX_NAME_OF_STATION:
        case OHA_RECORD_INDEX_DCP_NAME_OF_STATION:
        case OHA_RECORD_INDEX_IP_SUITE:
        case OHA_RECORD_INDEX_DCP_IP_SUITE:
            OHA_PrmWriteStation(pRQB, pHDBUser);
            break;

        case OHA_RECORD_INDEX_SYS_NAME:
        case OHA_RECORD_INDEX_SYS_CONTACT:
        case OHA_RECORD_INDEX_SYS_LOCATION:
            OHA_PrmWriteSNMP(pRQB, pHDBUser);
            break;

        case OHA_RECORD_INDEX_MULTIPLE_SNMP:
        case OHA_RECORD_INDEX_LLDP_INTERFACE_DATA:
        case OHA_RECORD_INDEX_LLDP_PORT_DATA:
            OHA_PrmWriteLLDP(pRQB, pHDBUser);
            break;

        case OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA:
        case OHA_RECORD_INDEX_LLDP_PORT_DEFAULT_DATA:
			Response = OHA_ERR_PRM_INDEX;  /* not allowed */
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Request OHA_OPC_PRM_WRITE of LLDP default records not allowed. (Inrex: 0x%X)", RecordIndex);
			OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            break;

        case OHA_RECORD_INDEX_TCP_KEEP_ALIVE:
            OHA_PrmControlTCP(pRQB, pHDBUser);
            break;

		case OHA_RECORD_INDEX_DHCP_PARAMS:
            OHA_PrmWriteDHCP(pRQB, pHDBUser);
            break;

        case OHA_RECORD_INDEX_SNMP_CONTROL:
            OHA_PrmWriteSNMPControl(pRQB, pHDBUser);
            break;
			
        case OHA_RECORD_INDEX_BUS_COMMON:
        case OHA_RECORD_INDEX_BUS_ADDITION:
        case OHA_RECORD_INDEX_MULTIPLE_BUS:
            OHA_PrmWriteBUS(pRQB, pHDBUser);
            break;

        default:
            OHA_PrmWritePDEV(pRQB, pHDBUser);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmWrite(RecordIndex: 0x%X)", RecordIndex);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmWritePDEV                            +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmPrepare                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write PDEV data in a temporary database        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWritePDEV(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response;
    LSA_UINT16              NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmWritePDEV(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    if (pHDBUser->TopoState == OHA_TOPO_STATE_CLOSE)   /* channel is not registered */
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Channel not registered!");

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWritePDEV(Response: 0x%X)", Response);

        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }
    if (pHDBUser->PrmState != OHA_PRM_STATE_PREPARE)   /* a PrmPrepare is done ? */
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE only allowed after OHA_OPC_PRM_PREPARE (PrmState: 0x%X)!", pHDBUser->PrmState);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWritePDEV(Response: 0x%X)", Response);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }

	if (OHA_IS_NULL(pRQB->args.PrmData.record_data) || pRQB->args.PrmData.record_data_length < OHA_TOPO_BLOCK_HEADER_PDU_SIZE)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }
    else
    {
        OHA_UPPER_PRM_DATA_PTR_TYPE pPrm    = &pRQB->args.PrmData;

        if (pPrm->record_index == OHA_RECORD_INDEX_PD_PORT_DATA_CHECK)
        {
            if (!(pRQB->args.PrmData.edd_port_id != 0 && pRQB->args.PrmData.edd_port_id <= OHA_CFG_MAX_PORTS))  /* pHDB->PortCnt */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
            }
            else
            {
			    OHA_HDB_EDD_PTR_TYPE pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

                /* first check the record index and the interfaceID */
                Response = OHA_Check_PdPortData(pHDBUser, pRQB,
                                                pHDBEdd->LinkStatus[pRQB->args.PrmData.edd_port_id-1].IsWireless);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) PDPortData failed.", pPrm->record_index);
                }
                if (Response == OHA_OK)
                {
                    Response = OHA_RequestAddPrm(pPrm->record_data, pPrm->record_data_length,
                                                 pPrm->edd_port_id, pHDBUser);
                }
            }
        }
        else if (pPrm->record_index == OHA_RECORD_INDEX_PD_PORT_DATA_ADJUST)
        {
            if (!(pRQB->args.PrmData.edd_port_id != 0  && pRQB->args.PrmData.edd_port_id <= OHA_CFG_MAX_PORTS))  /* pHDB->PortCnt */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
            }
            else
            {
                /* first check the record index and the interfaceID */
                Response = OHA_Check_PdPortDataAdjust(pHDBUser, pRQB);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) PDPortDataAdjust failed.", pPrm->record_index);
                }
                if (Response == OHA_OK)
                {
                    Response = OHA_RequestAddPrmAdjust(pPrm->record_data, pPrm->record_data_length,
                                                       pPrm->edd_port_id, pHDBUser);
                }
            }
        }
        /* Expected PDSyncData for one subslot with SyncID value 0 */
        else if (pPrm->record_index == OHA_RECORD_INDEX_PD_SYNC_DATA)
        {
            if (pRQB->args.PrmData.edd_port_id != 0)    /* interface */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
            }
            else
            {
                /* first check the record index and the interfaceID */
                Response = OHA_Check_PdSyncData(pRQB);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) PDSyncData failed.", pPrm->record_index);
                }
                if (Response == OHA_OK)
                {
                    Response = OHA_RequestAddPrmSync(pPrm->record_data, pPrm->record_data_length,
                                                     pPrm->record_index, pHDBUser);
                }
            }
        }
        /* PDIRData for one subslot */
        else if (pPrm->record_index == OHA_RECORD_INDEX_PD_IR_DATA)
        {
            if (pRQB->args.PrmData.edd_port_id != 0)    /* interface */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
            }
            else
            {
                /* first check the record index and the interfaceID */
                Response = OHA_Check_PdIRData(pHDBUser, pRQB);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) PDIRData failed.", pPrm->record_index);
                }
                if (Response == OHA_OK)
                {
                    Response = OHA_RequestAddIRData(pPrm->record_data, pPrm->record_data_length,
                                                    pPrm->record_index, pHDBUser);
                }
            }
        }
        /* FastStartUp data for one AR */
        else if (pPrm->record_index == OHA_RECORD_INDEX_FAST_STARTUP)
        {
            if (pRQB->args.PrmData.edd_port_id != 0)    /* interface */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
            }
            else
            {
                /* first check the record index and the interfaceID */
                Response = OHA_Check_FastStartUp(pRQB);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) FastStartUp failed.", pPrm->record_index);
                }
                if (Response == OHA_OK)
                {
                    Response = OHA_RequestAddFastStartUp(pPrm->record_data, pPrm->record_data_length,
                                                         pPrm->record_index, pHDBUser);
                }
            }
        }
        /* PDInterfaceAdjust */
        else if (pPrm->record_index == OHA_RECORD_INDEX_PD_INTERFACE_ADJUST)
        {
            if (pRQB->args.PrmData.edd_port_id != 0)    /* interface */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
            }
            else
            {
                /* check the record and write */
                Response = OHA_RequestAddIfAdjustData(pPrm->record_data, pPrm->record_data_length,
                                                      pPrm->record_index, pHDBUser);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) PDInterfaceAdjust failed.", pPrm->record_index);
                }
            }
        }
        /* PNIdentification */
        else if (pPrm->record_index == OHA_RECORD_INDEX_PN_IDENTIFICATION)
        {
            if (pRQB->args.PrmData.edd_port_id != 0)    /* interface */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE RecordIndex (%u) failed. Portid is wrong (PORT-ID: 0x%X)",pPrm->record_index, pRQB->args.PrmData.edd_port_id);
            }
            else
            {
                /* check the record and write */
                Response = OHA_RequestAddPnIdentData(pRQB, pHDBUser);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) PNIdentification failed.", pPrm->record_index);
                }
            }
        }
        else
        {
#ifdef OHA_CFG_NO_REMA
            Response = OHA_OK;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
                                 , "REMA not configured, ignore RecordIndex: 0x%X", pRQB->args.PrmData.record_index
                                );
#else
            Response = OHA_OK;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OHA_PrmWritePDEV(REMA record_index: 0x%X)", pPrm->record_index);
#endif
        }

#ifndef OHA_CFG_NO_REMA
        if (pRQB->args.PrmData.mk_rema) /* only the "make" rema records */
        {
            (LSA_UINT16)oha_rema_prm_write(NICId, pRQB);
        }
#endif
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWritePDEV(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmWriteStation                         +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write station data in the database             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16              NICId;

	OHA_COMMON_MEM_U8_PTR_TYPE pStationName = LSA_NULL;
    LSA_UINT16                 Length       = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmWriteStation(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pPrm  = &pRQB->args.PrmData;
    NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
    if ((pPrm->subslot_number & 0xF0FF) != 0x8000)
    {
        Response = OHA_ERR_PRM_INDEX;
        /* Error: no ports. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid subslot_number (0x%X), format is 8ipp, pp must be 0 for an interface",pPrm->subslot_number);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }

	if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
          LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ) )
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if (OHA_IS_NULL(pPrm->record_data) || pPrm->record_data_length == 0)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_MULTIPLE_STATION)
    {
        if (pPrm->record_data_length < OHA_TOPO_BLOCK_HEADER_PDU_SIZE)
        {
            Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Multiple RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
        }
        else
        {
            LSA_UINT32 Options = 0;
            OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr = LSA_NULL;

            Response = OHA_Get_PrmStationData(pPrm->record_data,        /* check the record, get IP */
                                              pPrm->record_data_length,
                                              &Options,
                                              &pIpAddr,
											  &pStationName, &Length);
			
			if (Response == OHA_OK)
			{
				/* check, if it's a RFC-name */
				if (Length != 0)
				{
					if (!oha_is_valid_station_name(Length, pStationName, OHA_UPPER_CASES_ALLOWED(NICId), OHA_UPPER_CASES_ALLOWED(NICId)))
					{
						Response = OHA_ERR_PRM_DATA;
						OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_PrmWriteStation: NameOfStation is invalid.");
					}
				}
			}

            if (Response != OHA_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Getting StationData failed (Response=%u).", Response);
            }
            else
            {
                /******** CHECK **************/
                if (Options & OHA_CTRL_STATION_NAME)
                {
                    if (oha_db_read_user_lock(NICId, pHDBUser->ThisHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDNAME))  /* locked by any user ? */
                    {
                        Response = OHA_ERR_USER_LOCK;
                        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X), NoS", Response);
                        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation: 0x%X)", Response);
                        return;
                    }
                }
                if (Options & OHA_CTRL_IP_SUITE)
                {
                    if (oha_db_read_user_lock(NICId, pHDBUser->ThisHandle, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP))  /* locked by any user ? */
                    {
                        Response = OHA_ERR_USER_LOCK;
                        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X), IP", Response);
                        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation: 0x%X)", Response);
                        return;
                    }
                }

                /* we have an asynchronous call to SOCK before if the ip address is to set */
                if (Options & OHA_CTRL_IP_SUITE)
                {
                    OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NICId);

                    OHA_ASSERT_NULL_PTR(pHDBDcp);
                    /* SetIp from any other user running ? */
                    if (OHA_IsAnyLocalSetActive(NICId))
                    {
                        Response = OHA_ERR_SEQUENCE;  /* already one request active */
                        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
                        return;
                    }

                    /* an OHA controlled DCP set is active (no query-user existing) ? */
                    if (OHA_IS_NULL(pHDBDcp->pHDBUserCtrl) && OHA_IS_NOT_NULL(pHDBDcp->pLowerDcpSetRqb))
                    {
                        /* queue the user-rqb */
                        OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
                                              pHDBUser->SockUserReq.pTop,
                                              pRQB);

                        Response = OHA_OK_ACTIVE;
                        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"An OHA controlled Dcp-Set is active: DcpState (0x%X), NicID (0x%X)", pHDBDcp->QueryState, NICId);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
                        return;
                    }

                    /* the ip address is to set, queue the request and call SOCK_OPC_SET_IPADDR */
                    /* now we have an asynchronous call to SOCK */
                    Response = OHA_SOCKSetIpAddress(NICId, OHA_RQB_GET_HANDLE(pRQB),
                                                    pIpAddr,
                                                    pIpAddr+OHA_IP_ADDR_SIZE,
                                                    pIpAddr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,
                                                    LSA_FALSE);

                    if (Response != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Setting IP-Address failed (Response=%u).", Response);
                        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation: 0x%X)", Response);
                        return;
                    }
                    else
                    {
                        /* queue the user-rqb */
                        OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
                                              pHDBUser->SockUserReq.pTop,
                                              pRQB);

                        Response = OHA_OK_ACTIVE;
                        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
                        return;
                    }
                }

#ifndef OHA_CFG_NO_NARE
                if (Options & OHA_CTRL_STATION_NAME)
                {
                    /* cancel running check requests */
                    LSA_BOOL Finish = OHA_NARECheckAddressCancel(NICId, OHA_CTRL_STATION_NAME, OHA_RQB_GET_HANDLE(pRQB));
                    if (!Finish)
                    {
                        /* queue the user-rqb */
                        OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
                                              pHDBUser->SockUserReq.pTop,
                                              pRQB);

                        Response = OHA_OK_ACTIVE;
                        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
                        return;
                    }
                }
#endif
                Response = OHA_DoPrmWriteStation(pRQB, pHDBUser);
            }
        }
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_NAME_OF_STATION ||
             pPrm->record_index == OHA_RECORD_INDEX_DCP_NAME_OF_STATION)
    {
        if (oha_db_read_user_lock(NICId, pHDBUser->ThisHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDNAME))  /* locked by any user ? */
        {
            Response = OHA_ERR_USER_LOCK;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
            OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation: 0x%X)", Response);
            return;
        }

        /* SetNoS from any other user running ? */
        if (OHA_IsAnyLocalSetActive(NICId))
        {
            Response = OHA_ERR_SEQUENCE;  /* already one request active */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_OPC_PRM_WRITE-Write(NoS) failed. Already one User-Request active for this NicId (NIC-ID: 0x%X)",NICId);
            OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation: 0x%X)", Response);
            return;
        }

		if (pPrm->record_index == OHA_RECORD_INDEX_DCP_NAME_OF_STATION)	/* old record */
		{
			Response = OHA_Get_NameOfStationData(pPrm->record_data,     /* check the record */
												 pPrm->record_data_length,
												 &pStationName, &Length);
		}
		else
		{
			Response = OHA_Get_PrmNameOfStationData(pPrm->record_data,  /* check the record */
													pPrm->record_data_length,
													&pStationName, &Length);
		}

		if (Response == OHA_OK)
		{
			/* check, if it's a RFC-name */
			if (Length != 0)
			{
				if (!oha_is_valid_station_name(Length, pStationName, OHA_UPPER_CASES_ALLOWED(NICId), OHA_UPPER_CASES_ALLOWED(NICId)))
				{
					Response = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_PrmWriteStation: NameOfStation is invalid.");
				}
			}
		}

		if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) NameOfStation failed.", pPrm->record_index);
        }
        else
        {
#ifndef OHA_CFG_NO_NARE
            /* cancel running check requests */
            LSA_BOOL Finish = OHA_NARECheckAddressCancel(NICId, OHA_CTRL_STATION_NAME, OHA_RQB_GET_HANDLE(pRQB));
            if (!Finish)
            {
                /* queue the user-rqb */
                OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
                                      pHDBUser->SockUserReq.pTop,
                                      pRQB);
                Response = OHA_OK_ACTIVE;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
                return;
            }
#endif
            Response = OHA_DoPrmWriteStation(pRQB, pHDBUser);
        }
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_IP_SUITE ||
			 pPrm->record_index == OHA_RECORD_INDEX_DCP_IP_SUITE)
    {
        if (oha_db_read_user_lock(NICId, pHDBUser->ThisHandle, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP))  /* locked by any user ? */
        {
            Response = OHA_ERR_USER_LOCK;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
            OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            return;
        }

        /* SetIp from any other user running ? */
        if (OHA_IsAnyLocalSetActive(NICId))
        {
            Response = OHA_ERR_SEQUENCE;  /* already one request active */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_OPC_PRM_WRITE-Write(IP) failed. Already one User-Request active for this NicId (NIC-ID: 0x%X)",NICId);
        }
        else
        {
			LSA_UINT16              Length;
            OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr     = LSA_NULL;

			if (pPrm->record_index == OHA_RECORD_INDEX_DCP_IP_SUITE)	/* old record */
			{
				Response = OHA_Get_IpSuiteData(pPrm->record_data,       /* check the record */
											   pPrm->record_data_length,
											   &pIpAddr,
											   &Length);
			}
			else
			{
				Response = OHA_Get_PrmIpSuiteData(pPrm->record_data,    /* check the record */
												  pPrm->record_data_length,
												  &pIpAddr,
												  &Length);
			}
            if (Response == OHA_OK)
            {
                OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NICId); /* DCP channel necessary in case of a Query */

                /* an OHA controlled DCP set is active (no query-user existing) ? */
                if (OHA_IS_NULL(pHDBDcp->pHDBUserCtrl) && OHA_IS_NOT_NULL(pHDBDcp->pLowerDcpSetRqb))
                {
                    /* queue the user-rqb */
                    OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
                                          pHDBUser->SockUserReq.pTop,
                                          pRQB);

                    Response = OHA_OK_ACTIVE;
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"An OHA controlled Dcp-Set is active: DcpState (0x%X), NicID (0x%X)", pHDBDcp->QueryState, NICId);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation(Response: 0x%X)", Response);
                    return;
                }

                /* the ip address is to set, queue the request and call SOCK_OPC_SET_IPADDR */
                /* now we have an asynchronous call to SOCK */
                Response = OHA_SOCKSetIpAddress(NICId, OHA_RQB_GET_HANDLE(pRQB),
                                                pIpAddr,
                                                pIpAddr+OHA_IP_ADDR_SIZE,
                                                pIpAddr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,
                                                LSA_FALSE);
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: SockSetIpAddress (NICId 0x%X) failed.", NICId);
                }
                else
                {
                    /* queue the user-rqb */
                    OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
                                          pHDBUser->SockUserReq.pTop,
                                          pRQB);

                    Response = OHA_OK_ACTIVE;
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
                    return;
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) IpSuite failed.", pPrm->record_index);
            }
        }
    }
    else
    {
        Response = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "RecordIndex Error: OHA_PrmWriteStation(Index: 0x%X)", pPrm->record_index);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteStation: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPrmRead                          +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_READ                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmPrepare                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read the actual PRM data                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmRead(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
	LSA_UINT16 Response;
    LSA_UINT16 NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmRead(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

	NICId = pHDBUser->Params.pDetail->Params.User.NICId;

	if (!OHA_EDDCheckNICUsed(NICId))
    {
        /* Error: no such interface. Abort. */
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. No EDD-Channel for this NicId (NIC-ID: 0x%X)",NICId);

		OHA_UserRequestFinish(pHDBUser,pRQB,Response);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmRead(RecordIndex: 0x%X)", pRQB->args.PrmData.record_index);
	}

    /* we've three type of records: "PDEV records", "Station records"  */
	/* and SNMP records.                                               */
    /* - PDEV records are depending on a registered topo channel       */
    /* - Station and SNMP records are independant of any registration  */
    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_MULTIPLE_SNMP:
        case OHA_RECORD_INDEX_SYS_NAME:
        case OHA_RECORD_INDEX_SYS_CONTACT:
        case OHA_RECORD_INDEX_SYS_LOCATION:
			Response = OHA_RequestPrmReadSNMP(pRQB, NICId);
			if (Response == OHA_OK)
			{
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,"PRM-Read-SNMP(RecordIndex=0x%X) successful", pRQB->args.PrmData.record_index);
			}
		break;

        case OHA_RECORD_INDEX_LLDP_INTERFACE_DATA:
        case OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA:
            /* OHA_PrmReadLLDPIfData(pRQB, NICId); */
			Response = OHA_Put_PrmLLDPInterfaceData(NICId, pRQB->args.PrmData.record_data, 
													&pRQB->args.PrmData.record_data_length, 
													pRQB->args.PrmData.record_index,
													LSA_FALSE);
            break;

        case OHA_RECORD_INDEX_LLDP_PORT_DATA:
        case OHA_RECORD_INDEX_LLDP_PORT_DEFAULT_DATA:
			Response = OHA_Put_PrmLLDPPortData(NICId, pRQB->args.PrmData.edd_port_id,
												pRQB->args.PrmData.record_data, 
												&pRQB->args.PrmData.record_data_length, 
												pRQB->args.PrmData.record_index,
												LSA_FALSE);
            break;

        case OHA_RECORD_INDEX_MULTIPLE_STATION:
        case OHA_RECORD_INDEX_NAME_OF_STATION:
        case OHA_RECORD_INDEX_DCP_NAME_OF_STATION:
        case OHA_RECORD_INDEX_IP_SUITE:
        case OHA_RECORD_INDEX_DCP_IP_SUITE:
            /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
            if ((pRQB->args.PrmData.subslot_number & 0xF0FF) != 0x8000)
            {
                Response = OHA_ERR_PRM_INDEX;
                /* Error: no ports. Abort. */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. No valid subslot_number (0x%X), format is 8ipp, pp must be 0 for an interface",pRQB->args.PrmData.subslot_number);
                break;
            }
            Response = OHA_PrmReadPDEVStation(pRQB, NICId);
        break;

        case OHA_RECORD_INDEX_TCP_KEEP_ALIVE:
            OHA_PrmControlTCP(pRQB, pHDBUser);	/* async call to SOCK */
		    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmRead(RecordIndex: 0x%X)", pRQB->args.PrmData.record_index);
            return;

		case OHA_RECORD_INDEX_DHCP_PARAMS:
            Response = OHA_PrmReadDHCP(pRQB, NICId);
            break;

        case OHA_RECORD_INDEX_BUS_COMMON:
        case OHA_RECORD_INDEX_BUS_ADDITION:
            Response = OHA_RequestPrmReadBus(pRQB, NICId);
            break;

        default:
            if (pHDBUser->TopoState == OHA_TOPO_STATE_CLOSE) /* channel is not registered ? */
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. User-Channel not registered for Topo");
                Response = OHA_ERR_SEQUENCE;
                break;
            }
            Response = OHA_PrmReadPDEVStation(pRQB, NICId);
        break;
    }

	OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmRead(RecordIndex: 0x%X)", pRQB->args.PrmData.record_index);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmReadSNMP                             +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read PDEV- and station data records from OHA   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_PrmReadPDEVStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    LSA_UINT16              Response;
    OHA_UPPER_PRM_DATA_PTR_TYPE prm_data = &pRQB->args.PrmData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmReadPDEVStation(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    /* check the record index and portid */
    switch (prm_data->record_index)
    {
        case OHA_RECORD_INDEX_PD_PORT_DATA_REAL:    /* record PDPortDataReal */
        case OHA_RECORD_INDEX_PD_PORT_DATA_CHECK:   /* record PDPortDataCheck */
#if 0
        case OHA_RECORD_INDEX_PD_PORT_MRP_DATA_ADJUST:  /* record PDPortMrpDataAdjust */
#endif
        case OHA_RECORD_INDEX_PD_PORT_MRP_DATA_REAL:    /* record PDPortMrpDataReal */
            if (!(pRQB->args.PrmData.edd_port_id && (pRQB->args.PrmData.edd_port_id <= OHA_CFG_MAX_PORTS)))  /* pHDB->PortCnt */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
                return(Response);
            }
            break;
        case OHA_RECORD_INDEX_PD_INTERFACE_DATA_REAL:   /* record PDInterfaceDataReal */
            if (pRQB->args.PrmData.edd_port_id != 0)  /* pHDB->PortCnt */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
                return(Response);
            }
            break;
        case OHA_RECORD_INDEX_FAST_STARTUP: /* record FastStartUp */
            if (pRQB->args.PrmData.edd_port_id != 0)  /* pHDB->PortCnt */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
                return(Response);
            }
            break;
		case OHA_RECORD_INDEX_PD_INTERFACE_ADJUST:/* 0x8071 */
            if (pRQB->args.PrmData.edd_port_id != 0)    /* AP01517264: interface only */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
                return(Response);
            }
            break;
		case OHA_RECORD_INDEX_PN_IDENTIFICATION:
            if (pRQB->args.PrmData.edd_port_id != 0)    /* interface only */
            {
                Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
                /* Error on getting handle. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ (PNIdentification) failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
                return(Response);
            }
            break;
        case OHA_RECORD_INDEX_PD_IF_ADDITIONAL_INFO:
            Response = OHA_RequestReadAdditionalInfo(pRQB, NICId);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"TOPO-Read (AdditionalDiagnosis) successful");
            }
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
            return(Response);
        case OHA_RECORD_INDEX_MULTIPLE_STATION:
        case OHA_RECORD_INDEX_NAME_OF_STATION:
        case OHA_RECORD_INDEX_DCP_NAME_OF_STATION:
        case OHA_RECORD_INDEX_IP_SUITE:
        case OHA_RECORD_INDEX_DCP_IP_SUITE:
            Response = OHA_RequestPrmReadStation(pRQB, NICId);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"PRM-Read-Station(RecordIndex=0x%X) successful", prm_data->record_index);
            }
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
            return(Response);

        default:  /* no record PDPortDataCheck or PDPortMrpDataAdjust,... */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Invalid RecordIndex (0x%X)",pRQB->args.PrmData.record_index);
            Response = OHA_ERR_PRM_INDEX;  /* no record PDPortDataCheck or PDPortMrpDataAdjust */
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
            return(Response);
    }

    switch (prm_data->record_index)
    {
		case OHA_RECORD_INDEX_PN_IDENTIFICATION:
            Response = OHA_RequestReadPNIdentPrm(pRQB, NICId);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"PRM-Read (PNIdentification) successful");
            }
            break;
		case OHA_RECORD_INDEX_PD_INTERFACE_ADJUST:/* 0x8071 */
            Response = OHA_RequestReadPDInterfacePrm(pRQB, NICId);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"PRM-Read (PDInterfaceAdjust) successful");
            }
            break;
        case OHA_RECORD_INDEX_PD_PORT_DATA_REAL:    /* record PDPortDataReal */
            Response = OHA_RequestReadRealPrm(pRQB, NICId);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"TOPO-Read (RealData) successful");
            }
            break;
        case OHA_RECORD_INDEX_PD_PORT_DATA_CHECK:   /* record PDPortDataCheck */
			{
				OHA_HDB_USER_PTR_TYPE pHDB = OHA_GetUserHDBFromHandle(OHA_RQB_GET_HANDLE(pRQB));

				OHA_ASSERT_NULL_PTR(pHDB);
				Response = OHA_RequestReadExpectedPrm(pRQB, pHDB);
				if (Response == OHA_OK)
				{
					OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"TOPO-Read (ExpectedData) successful");
				}
			}
            break;
#if 0
        case OHA_RECORD_INDEX_PD_PORT_MRP_DATA_ADJUST:  /* record PDPortMrpDataAdjust */
#endif
        case OHA_RECORD_INDEX_PD_PORT_MRP_DATA_REAL:    /* record PDPortMrpDataReal */
            Response = OHA_RequestReadMrpPrm(pRQB, NICId, (LSA_UINT16)prm_data->record_index);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"TOPO-Read (MrpData) successful");
            }
            break;
        case OHA_RECORD_INDEX_PD_INTERFACE_DATA_REAL:   /* record PDInterfaceDataReal */
            Response = OHA_RequestReadRealInterfacePrm(pRQB, NICId);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"TOPO-Read (InterfaceDataReal) successful");
            }
            break;
        case OHA_RECORD_INDEX_FAST_STARTUP: /* record FastStartUp */
            Response = OHA_RequestReadFastStartUpPrm(pRQB, NICId);
            if (Response == OHA_OK)
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"PRM-Read (FastStartUp) successful");
            }
            break;
        default:  /* no record PDPortDataCheck or PDPortMrpDataAdjust,... */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Invalid RecordIndex (0x%X)",pRQB->args.PrmData.record_index);
            Response = OHA_ERR_PRM_INDEX;  /* no record PDPortDataCheck or PDPortMrpDataAdjust */
            break;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadPDEVStation(Response: 0x%X)", Response);
	return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetExt_PdPortDataExpected               +*/
/*+  Input/Output          :    PDPortDataCheck-PDU-fragment or a           +*/
/*+                             PDPortMrpDataAdjust-PDU                     +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_GetExt_PdPortDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pNumberOfPeers,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPeerRecord,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppFirstPeer,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUType,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pLineDelay,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pLinkStateExisting,
    OHA_COMMON_MEM_U16_PTR_TYPE pLinkState,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayDifferenceExisting,
    OHA_COMMON_MEM_U16_PTR_TYPE pDelayMode,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pMAUTypeDifferenceExisting,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUTypeMode)
{
    LSA_UINT16 BlockType;
    LSA_UINT16 BlockLength;
    LSA_UINT16 BlockVersion;

    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetExt_PdPortDataExpected(OHA_CheckPdPortData: 0x%X)", pRecData);

    if (!LSA_HOST_PTR_ARE_EQUAL(ppPeerRecord, LSA_NULL))
    {
        *ppPeerRecord  = 0;             /* init the peers */
    }
    if (!LSA_HOST_PTR_ARE_EQUAL(ppFirstPeer, LSA_NULL))
    {
        *ppFirstPeer   = 0;
    }

    RetVal = OHA_OK;

    *pNumberOfPeers    = 0;
    *pMAUType          = 0;
    * pDelayExisting   = LSA_FALSE;
    *pLineDelay        = 0;
    * pLinkStateExisting = LSA_FALSE;
    *pLinkState        = 0;

    * pDelayDifferenceExisting = LSA_FALSE;
    *pDelayMode        = 0;

    * pMAUTypeDifferenceExisting = LSA_FALSE;
    * pMAUTypeMode     = 0;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDPortDataCheck: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, */
    /*                  [CheckPeers], [CheckLineDelayFactor], [CheckMAUType], [CheckLinkState], */
    /*                  [CheckSyncDifference], [CheckMAUTypeDifference]           */

    /* PDPortMrpDataAdjust: BlockHeader, Padding, Padding, MRP_DomainUUID         */

    switch (BlockType)
    {
        case OHA_BLOCK_TYPE_PD_PORT_DATA_CHECK:         /* the Block PDPortDataCheck */
        {
            LSA_UINT16 SlotNumber;
            LSA_UINT16 SubSlotNumber;

            index += 2;                     /* 2 padding bytes        */
            if (index>RecordLen) goto RecLenFailed;
            if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataExpected() - wrong Padding !");
                break;
            }
            pRecord += 2;

            /* SlotNumber */
            index += 2;
            if (index>RecordLen) goto RecLenFailed;
            OHA_PDU_GET_U16 (SlotNumber, pRecord);

            /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
            index += 2;
            if (index>RecordLen) goto RecLenFailed;
            OHA_PDU_GET_U16 (SubSlotNumber, pRecord);

            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"OHA_GetExt_PdPortDataExpected(SlotNumber: 0x%X, SubSlotNumber: 0x%X)", SlotNumber, SubSlotNumber);
        }
        break;

        default:    /* not interesting for OHA */
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockType (%u) not interesting for OHA.", BlockType);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
            return (RetVal);
    }

    if (BlockType == OHA_BLOCK_TYPE_PD_PORT_DATA_CHECK)
    {
        LSA_UINT16      PeerBlockLength = 0;
        LSA_UINT16      MauTypeBlockLength = 0;
        LSA_UINT16      DelayBlockLength = 0;
        LSA_UINT16      LinkStatusBlockLength = 0;

        LSA_UINT16      DelayDifferenceBlockLength   = 0;
        LSA_UINT16      MAUTypeDifferenceBlockLength = 0;

        LSA_UINT16      mau_type;
        LSA_UINT16      number_of_peers;
        LSA_UINT32      line_delay;
        LSA_UINT16      link_state;

        LSA_UINT16      delay_mode;
        LSA_UINT16      mautype_mode;

        /* loop over [CheckPeers], [CheckLineDelayFactor], [CheckMAUType], [CheckDomainBoundary], [CheckMulticastBoundary] */
        while (index < RecordLen)
        {
            OHA_UPPER_MEM_U8_PTR_TYPE  pThisRecord = pRecord;
            LSA_UINT32      i;

            LSA_UINT32 sub_index = 0;   /* index of the SubBlock */

            /* check the BlockHeader: */
            /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
            index += 2;
            if (index>RecordLen) goto RecLenFailed;
            sub_index += 2;
            OHA_PDU_GET_U16 (BlockType, pRecord);
            index += 2;
            if (index>RecordLen) goto RecLenFailed;
            sub_index += 2;
            OHA_PDU_GET_U16 (BlockLength, pRecord);
            index += 2;
            if (index>RecordLen) goto RecLenFailed;
            sub_index += 2;
            OHA_PDU_GET_U16 (BlockVersion, pRecord);

            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader (BlockLength) failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader(BlockVersion) failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            switch (BlockType)
            {
                case OHA_TOPO_CHECK_PEERS:          /* the SubBlock CheckPeers from PDPortDataCheck */

                    /* SubBlock CheckPeers:                                                      */
                    /*     BlockHeader, NumberOfPeers (u8), (                                    */
                    /*                  LengthPeerPortName (u8), PeerPortName (octet[255]),          */
                    /*                  LengthPeerStationName (u8), PeerStationName (octet[1..255])  */
                    /*                                        )*, [Padding]                      */
                    /* Padding: The number of padding octets shall be adapted to make the block WORD aligned. */

                    /* already one sub block CheckPeers read */
                    if (PeerBlockLength)
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader CheckPeers.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    PeerBlockLength = BlockLength;

                    index += 1;
                    if (index>RecordLen) goto RecLenFailed;
                    sub_index += 1;
                    OHA_PDU_GET_U8 (number_of_peers, pRecord);      /* NumberOfPeers       */

                    if (number_of_peers != 1)   /* TIA 204272: more than 1 */ /* TIA 741012: not 0 (only 1) */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: NumberOfPeers (0x%X) failed.", number_of_peers);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    if (!LSA_HOST_PTR_ARE_EQUAL(ppPeerRecord, LSA_NULL))
                    {
                        *ppPeerRecord = pThisRecord;                /* here are the peers */
                    }
                    if (!LSA_HOST_PTR_ARE_EQUAL(ppFirstPeer, LSA_NULL))
                    {
                        *ppFirstPeer  = pRecord;
                    }

                    /* check TOPO-peer entries */
                    for (i = 0; i < number_of_peers && i < LLDP_MAX_PORT_SENDER; i++)  /* over all peers */
                    {
                        LSA_UINT8 PortLen, ChassisLen;

                        index += 1;
                        if (index>RecordLen) goto RecLenFailed;
                        sub_index += 1;
                        OHA_PDU_GET_U8 (PortLen, pRecord);  /* LengthPeerPortName  */
                        if (!PortLen)
                        {
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: LengthPeerPortName failed.");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                        }
                        pRecord += PortLen;                 /* PeerPortName        */

                        index += PortLen;
                        if (index>RecordLen) goto RecLenFailed;
                        sub_index += PortLen;
                        OHA_PDU_GET_U8 (ChassisLen, pRecord);/* LengthPeerStationName */
                        index += 1;
                        if (index>RecordLen) goto RecLenFailed;
                        sub_index += 1;
                        if (!ChassisLen)
                        {
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: LengthPeerStationName failed.");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                        }
                        pRecord += ChassisLen;              /* PeerStationName       */
                        index += ChassisLen;
                        if (index>RecordLen) goto RecLenFailed;
                        sub_index += ChassisLen;
                    }

                    /* padding bytes after reading all peers */
                    while ( (index & 3) != 0 )
                    {
                        index += 1;
                        if (index>RecordLen) goto RecLenFailed;
                        sub_index += 1;
                        if (*pRecord != 0) /* check the paddings */
                        {
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataExpected() - wrong Padding !");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                        }
                        pRecord += 1;                       /* padding byte        */
                    }

                    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                    if (!(PeerBlockLength == sub_index - 2*sizeof(LSA_UINT16)))
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Length of SubBlock (0x%X) CheckPeers failed.", PeerBlockLength);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    *pNumberOfPeers = number_of_peers;
                    break;

                case OHA_TOPO_CHECK_MAU_TYPE:       /* the SubBlock CheckMAUType from PDPortDataCheck */

                    /* SubBlock CheckMAUType:                                                      */
                    /*     BlockHeader, MAUType (u16)                                              */
                    /*                                                                             */

                    /* already one sub block CheckMAUType read */
                    if (MauTypeBlockLength)
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader CheckMAUType.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    MauTypeBlockLength = BlockLength;

                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    sub_index += 2;
                    OHA_PDU_GET_U16 (mau_type, pRecord);    /* MAUType    */

					/* AP00936138: check the subblock */
                    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                    if (!(MauTypeBlockLength == sub_index - 2*sizeof(LSA_UINT16)))
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Length of SubBlock (0x%X) CheckMAUType failed.", MauTypeBlockLength);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    *pMAUType = mau_type;
                    break;

                case OHA_TOPO_CHECK_LINK_STATE:     /* the SubBlock CheckLinkState from PDPortDataCheck */

                    /* SubBlock CheckLinkState:                                                      */
                    /*     BlockHeader, LinkState (u16)                                              */

                    /* already one sub block CheckLinkState read */
                    if (LinkStatusBlockLength)
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader CheckLinkState.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    LinkStatusBlockLength = BlockLength;

                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    sub_index += 2;
                    OHA_PDU_GET_U16 (link_state, pRecord);  /* LinkState    */

					/* AP00936138: check the subblock */
                    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                    if (!(LinkStatusBlockLength == sub_index - 2*sizeof(LSA_UINT16)))
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Length of SubBlock (0x%X) CheckLinkState failed.", LinkStatusBlockLength);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    * pLinkStateExisting = LSA_TRUE;
                    * pLinkState         = link_state;
                    break;

                case OHA_TOPO_CHECK_DELAY:      /* the SubBlock CheckLineDelay from PDPortDataCheck */

                    /* SubBlock CheckLineDelay:                                       */
                    /*     BlockHeader, Padding (u8), Padding (u8), LineDelay (u32)   */

                    /* already one sub block CheckLineDelay read */
                    if (DelayBlockLength)
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader CheckLineDelayFactor.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    DelayBlockLength = BlockLength;

                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    sub_index += 2;
                    if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataExpected() - wrong Padding !");
                        break;
                    }
                    pRecord += 2;                       /* 2 padding bytes        */

                    index += 4;
                    if (index>RecordLen) goto RecLenFailed;
                    sub_index += 4;
                    OHA_PDU_GET_U32 (line_delay, pRecord);  /* LineDelayFactor    */

					/* AP00936138: check the subblock */
                    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                    if (!(DelayBlockLength == sub_index - 2*sizeof(LSA_UINT16)))
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Length of SubBlock (0x%X) CheckLineDelay failed.", DelayBlockLength);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    * pDelayExisting   = LSA_TRUE;
                    *pLineDelay        = line_delay;

                    /* Bit 31: LineDelay.FormatIndicator */
                    /*  0   LineDelay.Value is coded as line delay - Shall be used in conjunction with PDPortDataCheck */
                    /*  1   LineDelay.Value is coded as cable delay */
                    if (line_delay & OHA_LINEDELAY_FORMAT_INDICATOR) /* a cable delay ? */
                    {
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"LineDelay (0x%X) is coded as cable delay - not in conjunction with PDPortDataCheck!", line_delay);
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    break;

                case OHA_TOPO_CHECK_DOMAIN_BOUNDARY:    /* the SubBlock CheckDomainBoundary from PDPortDataCheck */
                case OHA_TOPO_CHECK_MULTICAST_BOUNDARY: /* the SubBlock CheckMulticastBoundary from PDPortDataCheck */
                    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                    /* the BlockHeader is already read */
                    pRecord += (BlockLength + 2*sizeof(LSA_UINT16) - OHA_TOPO_BLOCK_HEADER_PDU_SIZE);
                    index   += (BlockLength + 2*sizeof(LSA_UINT16) - OHA_TOPO_BLOCK_HEADER_PDU_SIZE);
                    if (index>RecordLen) goto RecLenFailed;
                    break;

                case OHA_TOPO_CHECK_DELAY_DIFFERENCE:       /* Sub block for checking local and remote CableDelay */

                    /* SubBlock CheckSyncDifference:                                             */
                    /*     BlockHeader,  CheckSyncMode(u16)                                      */

                    /* already one sub block CheckSyncDifference read ? */
                    if (DelayDifferenceBlockLength)
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader CheckSyncDifference.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    DelayDifferenceBlockLength = BlockLength;

                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    sub_index += 2;
                    OHA_PDU_GET_U16 (delay_mode, pRecord);  /* CheckSyncMode    */

					/* AP00936138: check the subblock */
                    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                    if (!(DelayDifferenceBlockLength == sub_index - 2*sizeof(LSA_UINT16)))
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Length of SubBlock (0x%X) CheckSyncDifference failed.", DelayDifferenceBlockLength);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    * pDelayDifferenceExisting = LSA_TRUE;
                    * pDelayMode               = delay_mode;
                    break;

                case OHA_TOPO_CHECK_MAU_TYPE_DIFFERENCE:        /* Sub block for checking local and remote MAUTypes */

                    /* SubBlock CheckMAUTypeDifference:                                       */
                    /*     BlockHeader,  MAUTypeMode(u16)                                     */

                    /* already one sub block CheckMAUTypeDifference read ? */
                    if (MAUTypeDifferenceBlockLength)
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader CheckMAUTypeDifference.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    MAUTypeDifferenceBlockLength = BlockLength;

                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    sub_index += 2;
                    OHA_PDU_GET_U16 (mautype_mode, pRecord);    /* MAUTypeMode    */
					
					/* AP00936138: check the subblock */
                    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                    if (!(MAUTypeDifferenceBlockLength == sub_index - 2*sizeof(LSA_UINT16)))
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Length of SubBlock (0x%X) CheckMAUTypeDifference failed.", MAUTypeDifferenceBlockLength);
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }

                    * pMAUTypeDifferenceExisting = LSA_TRUE;
                    * pMAUTypeMode               = mautype_mode;
                    break;

                default:
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader(BlockType) unknown.");
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                    return(RetVal);
            }
        }
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetExt_PdPortDataAdjusted               +*/
/*+  Input/Output          :    PDPortDataAdjust-PDU                        +*/
/*+                                                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_GetExt_PdPortDataAdjusted(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDomainBoundaryExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pDomainBoundaryIngress,
    OHA_COMMON_MEM_U32_PTR_TYPE pDomainBoundaryEgress,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pMulticastBoundaryExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pMulticastBoundary,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pPeerToPeerBoundaryExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pPeerToPeerBoundary)
{
    LSA_UINT16 BlockType;
    LSA_UINT16 BlockLength;
    LSA_UINT16 BlockVersion;

    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetExt_PdPortDataAdjusted(OHA_CheckPdPortData: 0x%X)", pRecData);
    RetVal = OHA_OK;

    * pDomainBoundaryExisting = LSA_FALSE;
    * pDomainBoundaryIngress    = 0;
    * pDomainBoundaryEgress     = 0;
    * pMulticastBoundaryExisting = LSA_FALSE;
    * pMulticastBoundary        = 0;
    * pPeerToPeerBoundaryExisting = LSA_FALSE;
    * pPeerToPeerBoundary       = 0;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto AdjustRecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto AdjustRecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto AdjustRecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDPortDataAdjust: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber,       */
    /*                  [AdjustDomainBoundary], [AdjustMulticastBoundary], [AdjustMAUType ^ AdjustLinkState] */

    switch (BlockType)
    {
        case OHA_BLOCK_TYPE_PD_PORT_DATA_ADJUST:            /* the Block PDPortDataAdjust */
        {
            LSA_UINT16 SlotNumber;
            LSA_UINT16 SubSlotNumber;

            index += 2;                     /* 2 padding bytes        */
            if (index>RecordLen) goto AdjustRecLenFailed;
            if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataExpected() - wrong Padding !");
                break;
            }
            pRecord += 2;

            /* SlotNumber */
            index += 2;
            if (index>RecordLen) goto AdjustRecLenFailed;
            OHA_PDU_GET_U16 (SlotNumber, pRecord);

            /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
            index += 2;
            if (index>RecordLen) goto AdjustRecLenFailed;
            OHA_PDU_GET_U16 (SubSlotNumber, pRecord);

            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"OHA_GetExt_PdPortDataAdjusted(SlotNumber: 0x%X, SubSlotNumber: 0x%X)", SlotNumber, SubSlotNumber);
        }
        break;

        default:    /* not interesting for OHA */
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: BlockType (%u) not interesting for OHA.", BlockType);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
            return (RetVal);
    }

    /* the Block PDPortDataAdjust... */
    {
        LSA_UINT16      DomainBoundaryBlockLength = 0;
        LSA_UINT16      MulticastBoundaryBlockLength = 0;
        LSA_UINT16      MauTypeBlockLength = 0;
        LSA_UINT16      LinkStateBlockLength = 0;
        LSA_UINT16      PeerToPeerBlockLength = 0;
        LSA_UINT16      DcpBlockLength = 0;

        LSA_UINT32      domain_boundary;
        LSA_UINT32      multicast_boundary;
        LSA_UINT32      peer_to_peer_boundary;

        /* loop over [AdjustDomainBoundary], [AdjustMulticastBoundary], [AdjustMAUType ^ AdjustLinkState] */
        while (index < RecordLen)
        {
            /* check the BlockHeader: */
            /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
            index += 2;
            if (index>RecordLen) goto AdjustRecLenFailed;
            OHA_PDU_GET_U16 (BlockType, pRecord);
            index += 2;
            if (index>RecordLen) goto AdjustRecLenFailed;
            OHA_PDU_GET_U16 (BlockLength, pRecord);
            index += 2;
            if (index>RecordLen) goto AdjustRecLenFailed;
            OHA_PDU_GET_U16 (BlockVersion, pRecord);

            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: BlockHeader (BlockLength) failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            /* if (!((BlockVersion == OHA_TOPO_DATA_BLOCK_VERSION && BlockType != OHA_TOPO_ADJUST_DOMAIN_BOUNDARY) || */
            if (!(BlockVersion == OHA_TOPO_DATA_BLOCK_VERSION ||
                  (BlockVersion == OHA_TOPO_DATA_BLOCK_VERSION_V11 && BlockType == OHA_TOPO_ADJUST_DOMAIN_BOUNDARY)))
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: BlockHeader(BlockVersion) failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            switch (BlockType)
            {
                case  OHA_TOPO_ADJUST_DOMAIN_BOUNDARY:  /* Sub block for adjusting DomainBoundary  */

                    if (DomainBoundaryBlockLength)      /* already one sub block AdjustDomainBoundary read */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: More then one BlockHeader AdjustDomainBoundary.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    DomainBoundaryBlockLength = BlockLength;

                    /* SubBlock AdjustDomainBoundary (V1.0) not supported in V4.0i40  */
                    /* BlockHeader, Padding, Padding, DomainBoundary (u32), AdjustProperties (u16), [Padding] */

                    /* SubBlock AdjustDomainBoundary (V1.1):     */
                    /* BlockHeader, Padding, Padding, DomainBoundaryIngress (u32), DomainBoundaryEgress (u32), AdjustProperties (u16), [Padding] */

                    index += 2;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataAdjusted() - wrong Padding !");
                        break;
                    }
                    pRecord += 2;                       /* 2 padding bytes        */

                    if (BlockVersion == OHA_TOPO_DATA_BLOCK_VERSION)
                    {
                        index += 4;
                        if (index>RecordLen) goto AdjustRecLenFailed;
                        OHA_PDU_GET_U32 (domain_boundary, pRecord); /* DomainBoundary    */

                        * pDomainBoundaryExisting = LSA_TRUE;
                        *pDomainBoundaryIngress   = domain_boundary;
                        *pDomainBoundaryEgress    = domain_boundary;
                    }
                    else if (BlockVersion == OHA_TOPO_DATA_BLOCK_VERSION_V11)
                    {
                        index += 4;
                        if (index>RecordLen) goto AdjustRecLenFailed;
                        OHA_PDU_GET_U32 (domain_boundary, pRecord); /* DomainBoundaryIngress */

                        * pDomainBoundaryExisting   = LSA_TRUE;
                        *pDomainBoundaryIngress     = domain_boundary;

                        index += 4;
                        if (index>RecordLen) goto AdjustRecLenFailed;
                        OHA_PDU_GET_U32 (domain_boundary, pRecord); /* DomainBoundaryEgress  */

                        *pDomainBoundaryEgress     = domain_boundary;
                    }
                    else    /* alreay checked */
                    {
                        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    }

                    index += 2;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    pRecord += 2;                       /* AdjustProperties       */

                    /* padding bytes */
                    while ( (index & 3) != 0 )
                    {
                        index += 1;
                        if (index>RecordLen) goto AdjustRecLenFailed;
                        if (*pRecord != 0) /* check the paddings */
                        {
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataAdjusted() - wrong Padding !");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                        }
                        pRecord += 1;                       /* padding byte        */
                    }
                    break;

                case  OHA_TOPO_ADJUST_MULTICAST_BOUNDARY: /* Sub block for adjusting MulticastBoundary */

                    if (MulticastBoundaryBlockLength)       /* already one sub block AdjustMulticastBoundary read */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: More then one BlockHeader AdjustMulticastBoundary.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    MulticastBoundaryBlockLength = BlockLength;

                    /* SubBlock AdjustMulticastBoundary:     */
                    /* BlockHeader, Padding, Padding, MulticastBoundary (u32), AdjustProperties (u16), [Padding] */

                    index += 2;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataAdjusted() - wrong Padding !");
                        break;
                    }
                    pRecord += 2;                       /* 2 padding bytes        */

                    index += 4;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    OHA_PDU_GET_U32 (multicast_boundary, pRecord);  /* MulticastBoundary    */

                    * pMulticastBoundaryExisting   = LSA_TRUE;
                    *pMulticastBoundary        = multicast_boundary;

                    index += 2;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    pRecord += 2;                       /* AdjustProperties       */

                    /* padding bytes */
                    while ( (index & 3) != 0 )
                    {
                        index += 1;
                        if (index>RecordLen) goto AdjustRecLenFailed;
                        if (*pRecord != 0) /* check the paddings */
                        {
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataAdjusted() - wrong Padding !");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                        }
                        pRecord += 1;                       /* padding byte        */
                    }
                    break;

                case  OHA_TOPO_ADJUST_MAU_TYPE:         /* Sub block for adjusting MAUType */

                    if (MauTypeBlockLength || LinkStateBlockLength) /* already one sub block AdjustMAUType read */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: More then one BlockHeader CheckMAUType.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    MauTypeBlockLength = BlockLength;

                    /* SubBlock AdjustMAUType:     */
                    /* BlockHeader, Padding, Padding, MAUType (u16), AdjustProperties (u16) */
                    index += (BlockLength - sizeof(LSA_UINT16));
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    pRecord += (BlockLength - sizeof(LSA_UINT16));

                    break;

                case  OHA_TOPO_ADJUST_LINK_STATE:       /* Sub block for adjusting LinkState */

                    if (LinkStateBlockLength || MauTypeBlockLength) /* already one sub block AdjustLinkState read */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: More then one BlockHeader AdjustLinkState.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    LinkStateBlockLength = BlockLength;

                    /* SubBlock AdjustLinkState:     */
                    /* BlockHeader, Padding, Padding, LinkState (u16), AdjustProperties (u16) */
                    index += (BlockLength - sizeof(LSA_UINT16));
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    pRecord += (BlockLength - sizeof(LSA_UINT16));

                    break;

                case  OHA_TOPO_ADJUST_PEER_TO_PEER_BOUNDARY: /* Sub block for adjusting PeerToPeerBoundary */

                    if (PeerToPeerBlockLength)      /* already one sub block PeerToPeerBoundary read */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: More then one BlockHeader AdjustPeerToPeerBoundary.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    PeerToPeerBlockLength = BlockLength;

                    /* SubBlock AdjustPeerToPeerBoundary:     */
                    /* BlockHeader, Padding, Padding, PeerToPeerBoundary (u32), AdjustProperties (u16), [Padding*]  */
                    index += 2;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataAdjusted() - wrong Padding !");
                        break;
                    }
                    pRecord += 2;                       /* 2 padding bytes        */

                    index += 4;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    OHA_PDU_GET_U32 (peer_to_peer_boundary, pRecord);   /* MulticastBoundary    */

                    * pPeerToPeerBoundaryExisting   = LSA_TRUE;
                    *pPeerToPeerBoundary        = peer_to_peer_boundary;

                    index += 2;
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    pRecord += 2;                       /* AdjustProperties       */

                    /* padding bytes */
                    while ( (index & 3) != 0 )
                    {
                        index += 1;
                        if (index>RecordLen) goto AdjustRecLenFailed;
                        if (*pRecord != 0) /* check the paddings */
                        {
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_GetExt_PdPortDataAdjusted() - wrong Padding !");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                        }
                        pRecord += 1;                       /* padding byte        */
                    }

                    break;

                case  OHA_TOPO_ADJUST_DCP_BOUNDARY: /* Sub block for adjusting DcpBoundary */

                    if (DcpBlockLength)     /* already one sub block DcpBoundary read */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: More then one BlockHeader AdjustDcpBoundary.");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    DcpBlockLength = BlockLength;

                    /* SubBlock AdjustDcpBoundary:     */
                    /* BlockHeader, Padding, Padding, DCPBoundary (u32), AdjustProperties (u16), [Padding*]  */
                    index += (BlockLength - sizeof(LSA_UINT16));
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    pRecord += (BlockLength - sizeof(LSA_UINT16));

                    break;

                default:                /* future Sub block */

                    index += (BlockLength - sizeof(LSA_UINT16));
                    if (index>RecordLen) goto AdjustRecLenFailed;
                    pRecord += (BlockLength - sizeof(LSA_UINT16));

                    break;
            }
        }
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
AdjustRecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataAdjusted: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataAdjusted(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PdSyncData                          +*/
/*+  Input/Output          :    PDSyncData-PDU                              +*/
/*+                                                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get record parameters                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PdSyncData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSubDomainID,
    OHA_COMMON_MEM_U32_PTR_TYPE pReservedIntervalBegin,
    OHA_COMMON_MEM_U32_PTR_TYPE pReservedIntervalEnd,
    OHA_COMMON_MEM_U16_PTR_TYPE pSendClockFactor,
    OHA_COMMON_MEM_U16_PTR_TYPE pPTCPTimeoutFactor,
    OHA_COMMON_MEM_U32_PTR_TYPE pSyncSendFactor)
{
    LSA_UINT16 BlockType;
    LSA_UINT16 BlockLength;
    LSA_UINT16 BlockVersion;

    LSA_UINT32 Temp32;
    LSA_UINT16 Temp16;
    LSA_UINT8  Temp8;

    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PdSyncData(OHA_CheckPdPortData: 0x%X, RecordLen: 0x%X)", pRecData, RecordLen);
    RetVal = OHA_OK;

    * ppSubDomainID         = LSA_NULL;
    * pReservedIntervalBegin = 0;
    * pReservedIntervalEnd  = 0;
    * pSendClockFactor      = 0;
    * pPTCPTimeoutFactor    = 0;
    * pSyncSendFactor       = 0;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto SyncRecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto SyncRecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto SyncRecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdSyncData: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdSyncData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION_V12)
    {
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdSyncData: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdSyncData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDSyncData:  BlockHeader, Padding, Padding, SlotNumber, SubslotNumber,               */
    /* (V1.0)       PTCPSubdomainID, IRDataID, ReservedIntervalBegin, ReservedIntervalEnd,  */
    /*              PLLWindow, SyncSendFactor, SendClockFactor, SyncProperties,             */
    /*              SyncFrameAddress, PTCPTimeoutFactor                                     */

    /* PDSyncData:  BlockHeader, Padding, Padding, SlotNumber, SubslotNumber,               */
    /* (V1.1)       PTCPSubdomainID, IRDataID, ReservedIntervalBegin, ReservedIntervalEnd,  */
    /*              PLLWindow, SyncSendFactor, SendClockFactor, SyncProperties,             */
    /*              SyncFrameAddress, PTCPTimeoutFactor                                     */
    /*              PTCPLengthSubdomainName, PTCPSubdomainName, [Padding*]                  */

    /*  PDSyncData: BlockHeader, Padding, Padding, PTCPSubdomainID, ReservedIntervalBegin,  */
    /*  (V1.2):     ReservedIntervalEnd, PLLWindow, SyncSendFactor, SendClockFactor,        */
    /*              PTCPTimeoutFactor, PTCPTakeoverTimeoutFactor (u16),                     */
    /*              PTCPMasterStartupTime (u16), SyncProperties,                            */
    /*              PTCP_MasterPriority1 (u8), PTCP_MasterPriority2 (u8),                   */
    /*              PTCPLengthSubdomainName, PTCPSubdomainName, [Padding*]                  */

    switch (BlockType)
    {
        case OHA_BLOCK_TYPE_PD_SYNC_DATA:           /* the Block PDSyncData */
        {
            index += 2;                     /* 2 padding bytes        */
            if (index>RecordLen) goto SyncRecLenFailed;
            if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PdSyncData() - wrong Padding !");
                break;
            }
            pRecord += 2;

            /* PTCPSubdomainUUID */
            index += sizeof(OHA_UUID_TYPE);
            if (index>RecordLen) goto SyncRecLenFailed;
            *ppSubDomainID = pRecord;       /* PTCPSubdomainID    */
            pRecord += sizeof(OHA_UUID_TYPE);

            /* ReservedIntervalBegin */
            index += 4;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U32 (Temp32, pRecord);
            *pReservedIntervalBegin = Temp32;

            /* ReservedIntervalEnd */
            index += 4;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U32 (Temp32, pRecord);
            *pReservedIntervalEnd = Temp32;

            /* PLLWindow (u32) */
            index += 4;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U32 (Temp32, pRecord);
            LSA_UNUSED_ARG(Temp32);

            /* SyncSendFactor (u32) */
            index += 4;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U32 (Temp32, pRecord);
            *pSyncSendFactor = Temp32;

            /* SendClockFactor (u16) */
            index += 2;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U16 (Temp16, pRecord);
            *pSendClockFactor = Temp16;

            /* PTCPTimeoutFactor (u16) */
            index += 2;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U16 (Temp16, pRecord);
            *pPTCPTimeoutFactor = Temp16;

            /* PTCPTakeoverTimeoutFactor (u16) */
            index += 2;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U16 (Temp16, pRecord);

            /* PTCPMasterStartupTime (u16) */
            index += 2;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U16 (Temp16, pRecord);

            /* SyncProperties (u16) */
            index += 2;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U16 (Temp16, pRecord);
            LSA_UNUSED_ARG(Temp16);

            /* PTCP_MasterPriority1 (u8) */
            index += 1;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U8 (Temp8, pRecord);

            /* PTCP_MasterPriority2 (u8) */
            index += 1;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U8 (Temp8, pRecord);
            LSA_UNUSED_ARG(Temp8);

            /*  PTCPLengthSubdomainName (u8) */
            index += 1;
            if (index>RecordLen) goto SyncRecLenFailed;
            OHA_PDU_GET_U8 (Temp8, pRecord);

            if (Temp8 == 0 || Temp8 > 240) /* check the length */
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PdSyncData() - wrong PTCPLengthSubdomainName !");
                break;
            }

            /*  PTCPSubdomainName (1...240) */
            index += Temp8;
            if (index>RecordLen) goto SyncRecLenFailed;
            pRecord += Temp8;

            /* padding bytes after reading PTCPSubdomainName */
            while ( (index & 3) != 0 )
            {
                index += 1;
                if (index>RecordLen) goto SyncRecLenFailed;
                if (*pRecord != 0) /* check the paddings */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PdSyncData() - wrong Padding !");
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdSyncData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }
                pRecord += 1;                       /* padding byte        */
            }

            /* RecordLength should be the sum of all the existing BlockLengths */
            if (!(RecordLen == index))
            {
                goto SyncRecLenFailed;
            }
        }
        break;

        default:    /* not interesting for OHA */
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdSyncData: BlockType (%u) not interesting for OHA.", BlockType);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdSyncData(RetVal: 0x%X)", RetVal);
            return (RetVal);
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
SyncRecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdSyncData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdSyncData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdSyncData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PdIRData                            +*/
/*+  Input/Output          :    PDIRData-PDU                                +*/
/*+                                                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get record parameters                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PdIRData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIRDataUUID)
{
    LSA_UINT16 BlockType;
    LSA_UINT16 BlockLength;
    LSA_UINT16 BlockVersion;

    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PdIRData(OHA_CheckPdPortData: 0x%X, RecordLen: 0x%X)", pRecData, RecordLen);
    RetVal = OHA_OK;

    * ppIRDataUUID          = LSA_NULL;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto IRRecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto IRRecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto IRRecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdIRData: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION &&
        BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION_V11)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdIRData: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDIRData with BlockVersionLow = 0: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData, PDIRFrameData */
    /* PDIRData with BlockVersionLow = 1: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, [PDIRGlobalData], PDIRBeginEndData, [PDIRFrameData] */

    switch (BlockType)
    {
        case OHA_BLOCK_TYPE_PD_IR_DATA:         /* the Block PDIRData */
        {
            LSA_UINT16 SlotNumber;
            LSA_UINT16 SubSlotNumber;

            index += 2;                     /* 2 padding bytes        */
            if (index>RecordLen) goto IRRecLenFailed;
            if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PdIRData() - wrong Padding !");
                break;
            }
            pRecord += 2;

            /* SlotNumber */
            index += 2;
            if (index>RecordLen) goto IRRecLenFailed;
            OHA_PDU_GET_U16 (SlotNumber, pRecord);

            /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
            index += 2;
            if (index>RecordLen) goto IRRecLenFailed;
            OHA_PDU_GET_U16 (SubSlotNumber, pRecord);

            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"OHA_Get_PdIRData(SlotNumber: 0x%X, SubSlotNumber: 0x%X)", SlotNumber, SubSlotNumber);

            /* PDIRGlobalData with BlockVersionLow = 0 not supported in V4.0i40  */
            /* BlockHeader, Padding, Padding, IRDataUUID */

            /* PDIRGlobalData with BlockVersionLow = 1:  */
            /* BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts, */
            /* (MaxPortTxDelay, MaxPortRxDelay)*        */

            /* look at the the BlockHeader PDIRGlobalData: */
            /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
            index += 2;
            if (index>RecordLen) goto IRRecLenFailed;
            OHA_PDU_GET_U16 (BlockType, pRecord);
            index += 2;
            if (index>RecordLen) goto IRRecLenFailed;
            OHA_PDU_GET_U16 (BlockLength, pRecord);
            index += 2;
            if (index>RecordLen) goto IRRecLenFailed;
            OHA_PDU_GET_U16 (BlockVersion, pRecord);

            if (BlockType != OHA_BLOCK_TYPE_PDIR_GLOBAL_DATA)
            {
                RetVal = OHA_ERR_PRM_BLOCK;
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdIRData: BlockType (%u) not valid.", BlockType);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdIRData: BlockHeader (BlockLength) failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
            if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION &&
                BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION_V11 &&
				BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION_V12)
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdIRData: BlockHeader(BlockVersion) failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }

            index += 2;                     /* 2 padding bytes        */
            if (index>RecordLen) goto IRRecLenFailed;
            if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
            {
                RetVal = OHA_ERR_PRM_DATA;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PdIRData() - wrong Padding !");
                break;
            }
            pRecord += 2;

            /* IRDataUUID */
            index += sizeof(OHA_UUID_TYPE);
            if (index>RecordLen) goto IRRecLenFailed;
            *ppIRDataUUID = pRecord;        /* IRDataID    */

            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
        }
        return(RetVal);


        default:    /* not interesting for OHA */
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdIRData: BlockType (%u) not interesting for OHA.", BlockType);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
            return (RetVal);
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
IRRecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdIRData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdIRData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_FastStartUpData                     +*/
/*+  Input/Output          :    FsuData-PDU                                 +*/
/*+                                                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get record parameters                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_FastStartUpData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloMode,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloInterval,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloRetry,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloDelay)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;

    LSA_UINT16      BlockType;
    LSA_UINT16      BlockLength;
    LSA_UINT16      BlockVersion;

    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_FastStartUpData(OHA_Get_FastStartUpData: 0x%X, RecordLen: 0x%X)", pRecData, RecordLen);

    * pHelloMode     = 0;
    * pHelloInterval = 0;
    * pHelloRetry    = 0;
    * pHelloDelay    = 0;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 4;

    if (index>RecordLen) goto HelloRecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    OHA_PDU_GET_U16 (BlockLength, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockType != OHA_BLOCK_TYPE_PD_IF_FSU_DATA_ADJUST)
    {
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockType (%u) not valid.", BlockType);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(Response: 0x%X)", RetVal);
        return (RetVal);
    }

    /* BlockVersion */
    index   += 2;
    if (index>RecordLen) goto HelloRecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    index += 2;                     /* 2 padding bytes        */
    if (index>RecordLen) goto HelloRecLenFailed;
    if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong Padding !");
        return(RetVal);
    }
    pRecord += 2;

    /* PDInterface-FSUDataAdjust { BlockHeader, [Padding*], [FSHelloBlock], [FastStartUpBlock] * } */
    while (RetVal == OHA_OK && index < RecordLen)
    {
        /* look at the the BlockHeader: */
        /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
        index += 4;
        if (index>RecordLen) goto HelloRecLenFailed;
        OHA_PDU_GET_U16 (BlockType, pRecord);
        OHA_PDU_GET_U16 (BlockLength, pRecord);

        switch (BlockType)
        {
                /* FSHelloBlock:    BlockHeader, [Padding] *, FSHelloMode, FSHelloInterval, FSHelloRetry, FSHelloDelay, [Padding] * */
            case OHA_BLOCK_TYPE_FAST_STARTUP_HELLO:
            {
                LSA_UINT32 Temp32;

                /* BlockVersion */
                index   += 2;
                if (index>RecordLen) goto HelloRecLenFailed;
                pRecord += 2;

                index += 2;                     /* 2 padding bytes        */
                if (index>RecordLen) goto HelloRecLenFailed;
                if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_FastStartUpData() - wrong Padding !");
                    break;
                }
                pRecord += 2;

                /* FSHelloMode (u32), FSHelloInterval (u32), FSHelloRetry (u32), FSHelloDelay (u32) */
                index += 4;
                if (index>RecordLen) goto HelloRecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);
                *pHelloMode = Temp32;

                index += 4;
                if (index>RecordLen) goto HelloRecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);
                *pHelloInterval = Temp32;

                index += 4;
                if (index>RecordLen) goto HelloRecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);
                *pHelloRetry = Temp32;

                index += 4;
                if (index>RecordLen) goto HelloRecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);
                *pHelloDelay = Temp32;
            }
            break;

            default:
                /* FastStartUpBlock:BlockHeader, [Padding] *, Data*, [Padding] *    */
                if (BlockType >= OHA_BLOCK_TYPE_FAST_STARTUP_RESERVED_BEGIN &&
                    BlockType <= OHA_BLOCK_TYPE_FAST_STARTUP_RESERVED_END &&
                    BlockType != OHA_BLOCK_TYPE_PD_IF_FSU_DATA_ADJUST && /* not here */
                    BlockType != OHA_BLOCK_TYPE_PD_AR_FSU_DATA_ADJUST) /* not here */
                {
                    index += BlockLength;
                    if (index>RecordLen) goto HelloRecLenFailed;
                    pRecord += BlockLength;

                    /* The number of padding octets shall be adapted to make the block Unsigned32 aligned. */
                    while ( (index & 3) != 0 )
                    {
                        index += 1;
                        if (index>RecordLen) goto HelloRecLenFailed;
                        pRecord += 1;
                    }
                    break;
                }

                RetVal = OHA_ERR_PRM_BLOCK;
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_FastStartUpData: BlockType (%u) not valid.", BlockType);
                break;
        }
    }

    /* RecordLength should be the sum of all the existing Blocks */
    if ((RecordLen != index))
    {
HelloRecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_FastStartUpData: Length of Record failed.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_FastStartUpData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PdPortDataExpected                  +*/
/*+  Input/Output          :    PDPortDataCheck-PDU-fragment or a           +*/
/*+                             PDPortMrpDataAdjust-PDU                     +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PdPortDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pNumberOfPeers,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPeerRecord,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppFirstPeer,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUType,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pLineDelay)
{
    LSA_BOOL        LinkStateExisting;
    LSA_UINT16      LinkState;

    LSA_BOOL        DelayDifferenceExisting;
    LSA_UINT16      DelayMode;
    LSA_BOOL        MAUTypeDifferenceExisting;
    LSA_UINT16      MAUTypeMode;

    LSA_UINT16      RetVal;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PdPortDataExpected(OHA_CheckPdPortData: 0x%X)", pRecData);

    RetVal = OHA_GetExt_PdPortDataExpected(pRecData,
                                           RecordLen,
                                           pNumberOfPeers,
                                           ppPeerRecord,
                                           ppFirstPeer,
                                           pMAUType,
                                           pDelayExisting,
                                           pLineDelay,
                                           &LinkStateExisting,
                                           &LinkState,
                                           &DelayDifferenceExisting,
                                           &DelayMode,
                                           &MAUTypeDifferenceExisting,
                                           &MAUTypeMode);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataExpected(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PdPortMrpDataExpected               +*/
/*+  Input/Output          :    PDPortDataCheck-PDU-fragment or a           +*/
/*+                             PDPortMrpDataAdjust-PDU                     +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PdPortMrpDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppDomainUUID)
{
    LSA_UINT16 BlockType;
    LSA_UINT16 BlockLength;
    LSA_UINT16 BlockVersion;

    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PdPortDataExpected(OHA_CheckPdPortData: 0x%X)", pRecData);

    if (!LSA_HOST_PTR_ARE_EQUAL(ppDomainUUID, LSA_NULL))
    {
        *ppDomainUUID  = LSA_NULL;      /* init the UUID */
    }

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataExpected: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataExpected: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDPortDataCheck: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, */
    /*                  [CheckPeers], [CheckLineDelayFactor], [CheckMAUType], [CheckLinkState] */

    /* PDPortMrpDataAdjust: BlockHeader, Padding, Padding, MRP_DomainUUID         */

    switch (BlockType)
    {
        case OHA_BLOCK_TYPE_PD_PORT_MRP_DATA_REAL:  /* the Block PDPortMrpDataReal */
        {
            /*  PDPortMrpDataAdjust:    BlockHeader, Padding, Padding, MRP_DomainUUID   */
            /*                                                                          */
            /*        0x8053    PDPortMrpDataAdjust for one subslot                     */
            /*                                                                          */
            /*   MRP_DomainUUID:                                                        */
            /*   uuid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)                             */
            /*      0x00000000-0000-0000-0000-000000000000  - reserved                  */
            /*      0x00000000-0000-0000-0000-000000000001 to                           */
            /*      0xFFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFE  - unique                    */
            /*      0xFFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF  - default                   */

            index += 2;                     /* 2 padding bytes        */
            if (index>RecordLen) goto RecLenFailed;
            pRecord += 2;

            /* MRP_DomainUUID   */
            index += sizeof(OHA_UUID_TYPE);
            if (index>RecordLen) goto RecLenFailed;

            /* RecordLength should be the sum of all the existing BlockLengths */
            if (!(RecordLen == index))
            {
                goto RecLenFailed;
            }

            if (!LSA_HOST_PTR_ARE_EQUAL(ppDomainUUID, LSA_NULL))
            {
                *ppDomainUUID = pRecord;        /* MRP_DomainUUID    */
            }
        }
        break;

        default:    /* not interesting for OHA */
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataExpected: BlockType (%u) not interesting for OHA.", BlockType);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataExpected(RetVal: 0x%X)", RetVal);
            return (RetVal);
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataExpected: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record check is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataExpected(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddPrm                           +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Add (overwrite) the expected TOPO user neighbour-data     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPrm(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT16                  PortId,
    OHA_HDB_USER_PTR_TYPE       pHDBUser)
{
    LSA_UINT16           Response;
    OHA_DB_TOPO_PTR_TYPE pPrmTopoDB;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddPrm(pRecordData: 0x%X, RecordLen: 0x%X, pHDBUser: 0x%X)",
                          pRecordData, RecordLen, pHDBUser);

    if (LSA_HOST_PTR_ARE_EQUAL(pRecordData,  LSA_NULL) || RecordLen == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT16      MAUType             = 0;
        LSA_UINT16      LinkState           = 0;
        LSA_UINT16      NumberOfPeers       = 0;
        LSA_UINT32      LineDelay           = 0;
        LSA_BOOL        DelayExisting       = LSA_FALSE;
        OHA_UPPER_MEM_U8_PTR_TYPE pPeers    = LSA_NULL;
        LSA_BOOL        LinkStateExisting   = LSA_FALSE;

        LSA_BOOL        DelayDifferenceExisting   = LSA_FALSE;
        LSA_UINT16      DelayMode           = 0;
        LSA_BOOL        MAUTypeDifferenceExisting = LSA_FALSE;
        LSA_UINT16      MAUTypeMode         = 0;

        /***************************************************/
        /* first the record-check (peers, MAUType, ...)    */
        /***************************************************/

        Response = OHA_GetExt_PdPortDataExpected(pRecordData, RecordLen, &NumberOfPeers, &pPeers, 0, &MAUType, &DelayExisting, &LineDelay,
                                                 &LinkStateExisting, &LinkState,
                                                 &DelayDifferenceExisting, &DelayMode,
                                                 &MAUTypeDifferenceExisting, &MAUTypeMode);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Read Record PDPortDataCheck failed, PortId(0x%X).", PortId);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrm(Response: 0x%X)", Response);
            return (Response);
        }

        /* (i) the peers */
        if ((NumberOfPeers != 0) && (!LSA_HOST_PTR_ARE_EQUAL(pPeers, LSA_NULL)))  /* are there peers -> are they valid ? */
        {
            Response = OHA_CheckTopoPeers(pPeers, pHDBUser->Params.pDetail->Params.User.NICId);
            if (Response != OHA_OK)
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Read Record PDPortDataCheck (Peers) failed, PortId(0x%X).", PortId);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrm(Response: 0x%X)", Response);
                return (Response);
            }
        }

        /* (ii) the MAUType */
        if (MAUType) /* is there a MAUType -> is it valid ? */
        {
            if (!(OHA_CheckTopoMauType(MAUType, pHDBUser->Params.pDetail->Params.User.allow_non_pnio_mautypes)))
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Read Record PDPortDataCheck (MAUType) failed, PortId(0x%X).", PortId);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrm(Response: 0x%X)", Response);
                return (Response = OHA_ERR_PRM_DATA);
            }
        }

        /* (iii) the LineDelayFactor,... */
        if (!(NumberOfPeers || MAUType || DelayExisting || LinkStateExisting || DelayDifferenceExisting || MAUTypeDifferenceExisting))   /* no TOPO params for OHA ? */
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Record PDPortDataCheck with no Peer, MAUType or Delay, PortId(0x%X).", PortId);
            /* return (Response = OHA_ERR_PRM_DATA); */ /* AP01141787: an empty record is valid */
        }

        /********************************************************/
        /* it's O.K. alloc and write to the temporary database  */
        /********************************************************/
        /* check the PortId */
        OHA_ASSERT (!(PortId == 0 || PortId > OHA_CFG_MAX_PORTS));
        pPrmTopoDB = pHDBUser->pPrmTopoBase[PortId-1];

        if (LSA_HOST_PTR_ARE_EQUAL(pPrmTopoDB, LSA_NULL))
        {
            OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;    /* variable in octets */

            /* alloc the db for topo */
            OHA_ALLOC_LOCAL_MEM(&pVarTemp, sizeof(OHA_DB_TOPO_TYPE));  /* alloc the topo DB */
            if ( LSA_HOST_PTR_ARE_EQUAL(pVarTemp, LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Allocating TOPO-DB failed.");
                Response = LSA_RET_ERR_RESOURCE;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrm(Response: 0x%X)", Response);
                return (Response);
            }
            pPrmTopoDB = (OHA_DB_TOPO_PTR_TYPE)pVarTemp;
            OHA_MEMSET(pPrmTopoDB, 0, sizeof(OHA_DB_TOPO_TYPE));
            pHDBUser->pPrmTopoBase[PortId-1] = pPrmTopoDB;
        }

        /* (i) the peers */
        if (NumberOfPeers && (!LSA_HOST_PTR_ARE_EQUAL(pPeers, LSA_NULL)))   /* are there peers -> overwrite (all) the DB-peers */
        {
            OHA_FreeTopoPeers(pPrmTopoDB); /* overwrite, if necessary */

            Response = OHA_CopyTopoPeersToDB(pPrmTopoDB, pPeers);
            if (Response != OHA_OK)
            {
                OHA_FreeTopoPeers(pPrmTopoDB); /* delete, if some entries are made */
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Copying Peers to TOPO-DB failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrm(Response: 0x%X)", Response);
                return (Response);
            }
        }

        /* (ii) the MAUType */
        if (MAUType)
        {
            pPrmTopoDB->MAUType = MAUType;
        }

        /* (iii) the LineDelayFactor */
        if (DelayExisting) /* is there a LineDelay -> overwrite the DB-LineDelay */
        {
            pPrmTopoDB->DelayExisting   = DelayExisting;
            pPrmTopoDB->LineDelayFactor = LineDelay;
        }

        /* (iv) the LinkState */
        if (LinkStateExisting)
        {
            pPrmTopoDB->LinkState = LinkState;
            pPrmTopoDB->LinkStateExisting = LinkStateExisting;
        }

        /* (v) the DelayDifference */
        if (DelayDifferenceExisting)
        {
            pPrmTopoDB->DelayMode = DelayMode;
            pPrmTopoDB->DelayDifferenceExisting = DelayDifferenceExisting;
        }

        /* (vi) the MAUTypeDifference */
        if (MAUTypeDifferenceExisting)
        {
            pPrmTopoDB->MAUTypeMode = MAUTypeMode;
            pPrmTopoDB->MAUTypeDifferenceExisting = MAUTypeDifferenceExisting;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrm(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddPrmAdjust                     +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Add (overwrite) the expected TOPO user neighbour-data     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPrmAdjust(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT16                  PortId,
    OHA_HDB_USER_PTR_TYPE       pHDBUser)
{
    LSA_UINT16           Response;
    OHA_DB_TOPO_PTR_TYPE pPrmTopoDB;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddPrmAdjust(pRecordData: 0x%X, RecordLen: 0x%X, pHDBUser: 0x%X)",
                          pRecordData, RecordLen, pHDBUser);

    if (LSA_HOST_PTR_ARE_EQUAL(pRecordData,  LSA_NULL) || RecordLen == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_BOOL        DomainBoundaryExisting  = LSA_FALSE;
        LSA_UINT32      DomainBoundaryIngress   = 0;
        LSA_UINT32      DomainBoundaryEgress    = 0;
        LSA_BOOL        MulticastBoundaryExisting   = LSA_FALSE;
        LSA_UINT32      MulticastBoundary       = 0;
        LSA_BOOL        PeerToPeerBoundaryExisting  = LSA_FALSE;
        LSA_UINT32      PeerToPeerBoundary      = 0;

        /****************************************************/
        /* first the adjust record-check (DomainBoundary)   */
        /****************************************************/

        Response = OHA_GetExt_PdPortDataAdjusted(pRecordData, RecordLen,
                                                 &DomainBoundaryExisting, &DomainBoundaryIngress, &DomainBoundaryEgress,
                                                 &MulticastBoundaryExisting, &MulticastBoundary,
                                                 &PeerToPeerBoundaryExisting, &PeerToPeerBoundary);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Read Record PDPortDataAdjust failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrmAdjust(Response: 0x%X)", Response);
            return (Response);
        }

        /* (i) the DomainBoundary, the MulticastBoundary and the PeerToPeerBoundary */
        if (!(DomainBoundaryExisting || MulticastBoundaryExisting || PeerToPeerBoundaryExisting))   /* no TOPO params for OHA ? */
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Record PDPortDataAdjust with no Boundary.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrmAdjust(Response: 0x%X)", Response);
            return (Response);
        }

        /********************************************************/
        /* it's O.K. alloc and write to the temporary database  */
        /********************************************************/
        /* check the PortId */
        OHA_ASSERT (!(PortId == 0 || PortId > OHA_CFG_MAX_PORTS));
        pPrmTopoDB = pHDBUser->pPrmTopoBase[PortId-1];

        if (LSA_HOST_PTR_ARE_EQUAL(pPrmTopoDB, LSA_NULL))
        {
            OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;    /* variable in octets */

            /* alloc the db for topo */
            OHA_ALLOC_LOCAL_MEM(&pVarTemp, sizeof(OHA_DB_TOPO_TYPE));  /* alloc the topo DB */
            if ( LSA_HOST_PTR_ARE_EQUAL(pVarTemp, LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_TOPO_CONTROL-Write: Allocating TOPO-DB failed.");
                Response = LSA_RET_ERR_RESOURCE;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrmAdjust(Response: 0x%X)", Response);
                return (Response);
            }
            pPrmTopoDB = (OHA_DB_TOPO_PTR_TYPE)pVarTemp;
            OHA_MEMSET(pPrmTopoDB, 0, sizeof(OHA_DB_TOPO_TYPE));
            pHDBUser->pPrmTopoBase[PortId-1] = pPrmTopoDB;
        }

        /* (i) the DomainBoundary */
        if (DomainBoundaryExisting)
        {
            pPrmTopoDB->AdjustTopoData.DomainBoundaryIngress = DomainBoundaryIngress;
            pPrmTopoDB->AdjustTopoData.DomainBoundaryEgress  = DomainBoundaryEgress;
            pPrmTopoDB->AdjustTopoData.DomainBoundaryExisting = DomainBoundaryExisting;
        }

        /* (ii) the MulticastBoundary */
        if (MulticastBoundaryExisting)
        {
            pPrmTopoDB->AdjustTopoData.MulticastBoundary = MulticastBoundary;
            pPrmTopoDB->AdjustTopoData.MulticastBoundaryExisting = MulticastBoundaryExisting;
        }

        /* (iii) the PeerToPeerBoundary */
        if (PeerToPeerBoundaryExisting)
        {
            pPrmTopoDB->AdjustTopoData.PeerToPeerBoundary = PeerToPeerBoundary;
            pPrmTopoDB->AdjustTopoData.PeerToPeerBoundaryExisting = PeerToPeerBoundaryExisting;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrmAdjust(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddPrmSync                       +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Add (overwrite) the expected TOPO user neighbour-data     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPrmSync(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser)
{
    LSA_UINT16           Response;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddPrmSync(pRecordData: 0x%X, RecordLen: 0x%X, RecordIndex: 0x%X, pHDBUser: 0x%X)",
                          pRecordData, RecordLen, RecordIndex, pHDBUser);

    if (LSA_HOST_PTR_ARE_EQUAL(pRecordData,  LSA_NULL) || RecordLen == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else if (RecordIndex != OHA_RECORD_INDEX_PD_SYNC_DATA)
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        OHA_COMMON_MEM_U8_PTR_TYPE pSubDomainID = LSA_NULL;

        LSA_UINT32  ReservedIntervalBegin = 0;
        LSA_UINT32  ReservedIntervalEnd   = 0;
        LSA_UINT16  SendClockFactor     = 0;
        LSA_UINT16  PTCPTimeoutFactor   = 0;
        LSA_UINT32  SyncSendFactor      = 0;

        /****************************************************/
        /* first the record (PDSyncData)                    */
        /****************************************************/

        Response = OHA_Get_PdSyncData(pRecordData, RecordLen,
                                      &pSubDomainID,
                                      &ReservedIntervalBegin, &ReservedIntervalEnd,
                                      &SendClockFactor, &PTCPTimeoutFactor, &SyncSendFactor);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE: Write Record PDSyncData failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrmSync(Response: 0x%X)", Response);
            return (Response);
        }
        else /* write to temp sync data */
        {
            LSA_UINT16           NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
            OHA_HDB_GSY_PTR_TYPE pHDBGsy = OHA_GSYGetHandleByNIC(NICId);
            OHA_DB_IF_PTR_TYPE   pDbIf   = OHA_DB_GET_INTERFACE(NICId);

            if ( LSA_HOST_PTR_ARE_EQUAL(pHDBGsy,  LSA_NULL) || LSA_HOST_PTR_ARE_EQUAL(pDbIf,  LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PRM_DATA failed. No GSY-Channel present (NIC-ID: 0x%X)",NICId);
                Response = OHA_ERR_PARAM;
            }
            else
            {
                LSA_UINT16 i;

                OHA_ASSERT_NULL_PTR(pSubDomainID);

                /* delete buffers (A), but not IRDataUUID, it comes from the record PDIRData */
                pHDBGsy->SyncDataWrite.InterfaceID = 0;
                OHA_MEMSET (&pHDBGsy->SyncDataWrite.MasterSourceAddress, 0, sizeof(OHA_MAC_TYPE));

                /* => PTCPoverRTA (Index:=802D):                                */
                /*    => RedPeriodOffset := 0, invalid                          */
                /*    => OrangePeriodOffset := ReservedIntervalBegin, valid     */
                /*    => GreenPeriodOffset := ReservedIntervalEnd, valid        */
                /*    => PTCP_SubdomainID := PTCP_SubdomainID                   */
                /*    => IRDATAUUID := 0                                        */
                /*    => PTCP_SyncMaster := 0                                   */

                /* => PTCPoverRTC (Index:=802E):                                */
                /*    => RedPeriodOffset := edd_get_Port_Params.Receive.RedBegin, valid */
                /*    => OrangePeriodOffset := 0, invalid                       */
                /*    => GreenPeriodOffset := edd_get_Port_Params.Receive.RedEnd, valid */
                /*    => PTCP_SubdomainID := PTCP_SubdomainID                   */
                /*    => IRDATAUUID := IRDATAUUID                               */
                /*    => PTCP_SyncMaster := 0                                   */

                pHDBGsy->SyncDataWrite.Mode = OHA_SYNC_ADD; /* starts by PrmCommit */
                pHDBGsy->SyncDataWrite.RecordIndex = RecordIndex;
                OHA_MEMCPY (pHDBGsy->SyncDataWrite.SubdomainID, pSubDomainID, sizeof(OHA_UUID_TYPE));
                pHDBGsy->SyncDataWrite.PTCPTimeoutFactor = PTCPTimeoutFactor;
                pHDBGsy->SyncDataWrite.SyncSendFactor = SyncSendFactor;

#if 0
                for (i=0; i < OHA_CFG_MAX_PORTS; i++) /* AP01528425 */
                {
                    pHDBGsy->FrameOffsetTemp[i].LengthOfPeriod = 31250 * SendClockFactor; /* SendClockFactor x 31.25us */
                }

                for (i=0; i < OHA_CFG_MAX_PORTS; i++)   /* from the record PDSyncData */
                {
                    pHDBGsy->FrameOffsetTemp[i].OrangePeriodBegin = ReservedIntervalBegin;
                    pHDBGsy->FrameOffsetTemp[i].OrangePeriodBegin |= OHA_SYNC_PERIOD_VALID_MASK;
                    pHDBGsy->FrameOffsetTemp[i].GreenPeriodBegin  = ReservedIntervalEnd;
                    pHDBGsy->FrameOffsetTemp[i].GreenPeriodBegin  |= OHA_SYNC_PERIOD_VALID_MASK;
                }
#endif

                for (i=0; i < pDbIf->PortCnt && i < OHA_CFG_MAX_PORTS; i++)
                {
                    if (pHDBUser->PtcpTimerID[i] == OHA_TIMER_ID_INVALID)
                    {
                        /* now we need timer for delaying the PTCP diagnosis */
                        Response = OHA_TimerAlloc(LSA_TIMER_TYPE_ONE_SHOT,
                                                  LSA_TIME_BASE_100MS,
                                                  pHDBUser->ThisHandle,
                                                  OHA_TimerCB,
                                                  &pHDBUser->PtcpTimerID[i]);

                        if (Response != OHA_OK)
                        {
                            Response = OHA_ERR_RESOURCE;
                            /* Error on getting Timer. Abort. */
                            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_OPC_PRM_END failed. Got no Timer for PTCP diagnosis!");
                        }
                    }
                }
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPrmAdjust(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddIRData                        +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Add (overwrite) the expected TOPO user IR-data            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddIRData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser)
{
    LSA_UINT16           Response;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddIRData(pRecordData: 0x%X, RecordLen: 0x%X, RecordIndex: 0x%X, pHDBUser: 0x%X)",
                          pRecordData, RecordLen, RecordIndex, pHDBUser);

    if (LSA_HOST_PTR_ARE_EQUAL(pRecordData,  LSA_NULL) || RecordLen == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else if (RecordIndex != OHA_RECORD_INDEX_PD_IR_DATA)
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        OHA_COMMON_MEM_U8_PTR_TYPE pIRDataUUID  = LSA_NULL;

        /****************************************************/
        /* first the record (PDIRData)                      */
        /****************************************************/

        Response = OHA_Get_PdIRData(pRecordData, RecordLen, &pIRDataUUID);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE: Write Record PDIRData failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddIRData(Response: 0x%X)", Response);
            return (Response);
        }
        else /* write to temp sync data */
        {
            LSA_UINT16           NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
            OHA_HDB_GSY_PTR_TYPE pHDBGsy = OHA_GSYGetHandleByNIC(NICId);

            if ( LSA_HOST_PTR_ARE_EQUAL(pHDBGsy,  LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PRM_DATA failed. No GSY-Channel present (NIC-ID: 0x%X)",NICId);
                Response = OHA_ERR_PARAM;
            }
            else
            {
                OHA_ASSERT_NULL_PTR(pIRDataUUID);

                /* only copy the IRDataUUID to buffer (A) */
                OHA_MEMCPY (pHDBGsy->SyncDataWrite.IRDataID, pIRDataUUID, sizeof(OHA_UUID_TYPE));
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddIRData(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Check_PdPortData                        +*/
/*+  Input/Output          :    PDPortDataCheck-PDU-fragment or a           +*/
/*+                             PDPortMrpDataAdjust-PDU                     +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record to write                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdPortData(
    OHA_HDB_USER_PTR_TYPE    pHDBUser,
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_BOOL                 IsWireless)
{
    LSA_UINT16        RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord;
    LSA_UINT16      BlockType, BlockLen;
    LSA_UINT32      RecordLen;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Check_PdPortData(pHDBUser: 0x%X)", pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pRQB->args.PrmData.record_data);

    pRecord = pRQB->args.PrmData.record_data;
    RecordLen = pRQB->args.PrmData.record_data_length;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
	BlockType = OHA_GET16_NTOH (pRecord, 0);
	BlockLen  = OHA_GET16_NTOH (pRecord, 2);

	/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
	if (BlockLen != (pRQB->args.PrmData.record_data_length - 2*sizeof(LSA_UINT16))) /* AP01358661 */
	{
        RetVal = OHA_ERR_PRM_DATA;  /* BlockLength failed */
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen (0x%X) differs from BlockLength (0x%X)!",pRQB->args.PrmData.record_data_length, BlockLen);

		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdPortData(Response: 0x%X)", RetVal);
		return (RetVal);
	}

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_PD_PORT_DATA_CHECK:
            if (BlockType == OHA_BLOCK_TYPE_PD_PORT_DATA_CHECK)
            {
                /* no check of the InterfaceID in the SubslotNumber */
                /* PDPortDataCheck: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, */
                /*                  [CheckPeers], [CheckLineDelayFactor], [CheckMAUType], [CheckLinkState] */

                LSA_UINT16 SlotNumber, SubSlotNumber;
                LSA_UINT32 index = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

                pRecord += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

                index += 2;                     /* 2 padding bytes        */
                if (index>RecordLen) goto RecLenFailed;
                if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_PdPortData() - wrong Padding !");
                    break;
                }
                pRecord += 2;

                /* SlotNumber */
                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SlotNumber, pRecord);

                /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SubSlotNumber, pRecord);

                /* no check of the InterfaceID */
                LSA_UNUSED_ARG(pHDBUser);
#if 0
                LSA_UINT32 InterfaceID = (LSA_UINT32)(OHA_HIBYTE(SubSlotNumber & 0x0F00) + 1);

                if (InterfaceID == pHDBUser->InterfaceID)
                {
                    RetVal = OHA_OK;
                }
                else
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_PdPortData() - wrong InterfaceID (%u) !", (LSA_UINT16)InterfaceID);
                    break;
                }
#endif

                if (SlotNumber != pRQB->args.PrmData.slot_number) /* AP01358661 */
                {
                    RetVal = OHA_ERR_PRM_INDEX;
                    /* Error: no ports. Abort. */
                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid SlotNumber (0x%X) for this record (0x%X)",pRQB->args.PrmData.slot_number,pRQB->args.PrmData.record_index);
                    break;
                }
                /* SubSlotNumber 8001 (Port 1, Interface 1) Format: 8ipp */
                if (SubSlotNumber != pRQB->args.PrmData.subslot_number)
                {
                    RetVal = OHA_ERR_PRM_INDEX;
                    /* Error: no ports. Abort. */
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid SubSlotNumber for this interface (0x%X)",pRQB->args.PrmData.subslot_number);
                    break;
                }

                if (!IsWireless)
                {
                    break;
                }

                /* look for not allowed blocks in case of wireless: */
                /* CheckLineDelay, CheckMAUType, CheckMAUTypeDifference, CheckLinkState, CheckSyncDifference */
                while (index < RecordLen)
                {
                    LSA_UINT16 BlockLength;

                    /* BlockHeader: */
                    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    OHA_PDU_GET_U16 (BlockType, pRecord);
                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    OHA_PDU_GET_U16 (BlockLength, pRecord);
                    index += 2;
                    if (index>RecordLen) goto RecLenFailed;
                    pRecord += 2;

                    switch (BlockType)
                    {
                        case OHA_TOPO_CHECK_PEERS:              /* the SubBlock CheckPeers from PDPortDataCheck */
                        case OHA_TOPO_CHECK_DOMAIN_BOUNDARY:    /* the SubBlock CheckDomainBoundary from PDPortDataCheck */
                        case OHA_TOPO_CHECK_MULTICAST_BOUNDARY: /* the SubBlock CheckMulticastBoundary from PDPortDataCheck */
                            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
                            /* the BlockHeader is already read */
                            index   += (BlockLength + 2*sizeof(LSA_UINT16) - OHA_TOPO_BLOCK_HEADER_PDU_SIZE);
                            if (index>RecordLen) goto RecLenFailed;
                            pRecord += (BlockLength + 2*sizeof(LSA_UINT16) - OHA_TOPO_BLOCK_HEADER_PDU_SIZE);
                            break;

                        case OHA_TOPO_CHECK_MAU_TYPE:       /* the SubBlock CheckMAUType from PDPortDataCheck */
                        case OHA_TOPO_CHECK_LINK_STATE:     /* the SubBlock CheckLinkState from PDPortDataCheck */
                        case OHA_TOPO_CHECK_DELAY_DIFFERENCE:   /* Sub block for checking local and remote CableDelay */
                        case OHA_TOPO_CHECK_MAU_TYPE_DIFFERENCE:/* Sub block for checking local and remote MAUTypes */
                        case OHA_TOPO_CHECK_DELAY:      /* the SubBlock CheckLineDelayFactor from PDPortDataCheck */
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockType (0x%X) not allowed for wireless.", BlockType);
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                            return(RetVal);

                        default:
                            RetVal = OHA_ERR_PRM_BLOCK;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader(BlockType) unknown.");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                    }
                }
                break;

RecLenFailed:
                RetVal = OHA_ERR_PRM_BLOCK;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdPortData: Length of Record failed.");
            }
            break;
#if 0
        case OHA_RECORD_INDEX_PD_PORT_MRP_DATA_ADJUST:
            if (BlockType == OHA_BLOCK_TYPE_PD_PORT_MRP_DATA_ADJUST)
            {
                RetVal = OHA_OK;
            }
            break;
#endif
        default:
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdPortData: Index of Record (0x%X) failed.", pRQB->args.PrmData.record_index);
            RetVal = OHA_ERR_PRM_INDEX;
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdPortData(Response: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Check_PdPortDataAdjust                  +*/
/*+  Input/Output          :    PDPortDataCheck-PDU-fragment or a           +*/
/*+                             PDPortDataAdjust-PDU                        +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record to write                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdPortDataAdjust(
    OHA_HDB_USER_PTR_TYPE    pHDBUser,
    OHA_UPPER_RQB_PTR_TYPE   pRQB)
{
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord;
    LSA_UINT16      BlockType, BlockLen;
    LSA_UINT32      RecordLen;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Check_PdPortDataAdjust(pHDBUser: 0x%X)", pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pRQB->args.PrmData.record_data);

    pRecord = pRQB->args.PrmData.record_data;
    RecordLen = pRQB->args.PrmData.record_data_length;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
	BlockType = OHA_GET16_NTOH (pRecord, 0);
	BlockLen  = OHA_GET16_NTOH (pRecord, 2);

	/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
	if (BlockLen != (pRQB->args.PrmData.record_data_length - 2*sizeof(LSA_UINT16))) /* AP01358661 */
	{
        RetVal = OHA_ERR_PRM_DATA;  /* BlockLength failed */
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen (0x%X) differs from BlockLength (0x%X)!",pRQB->args.PrmData.record_data_length, BlockLen);

		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdPortDataAdjust(Response: 0x%X)", RetVal);
		return (RetVal);
	}

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_PD_PORT_DATA_ADJUST:
            if (BlockType == OHA_BLOCK_TYPE_PD_PORT_DATA_ADJUST)
            {
                /* no check of the InterfaceID in the SubslotNumber */
                /* PDPortDataAdjust: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber,       */
                /*                  [AdjustDomainBoundary], [AdjustMulticastBoundary], [AdjustMAUType ^ AdjustLinkState] */

                LSA_UINT16 SlotNumber, SubSlotNumber;
                LSA_UINT32 index = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

                pRecord += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

                index += 2;                     /* 2 padding bytes        */
                if (index>RecordLen) goto RecLenFailed;
                if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_PdPortDataAdjust() - wrong Padding !");
                    break;
                }
                pRecord += 2;

                /* SlotNumber */
                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SlotNumber, pRecord);

                /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SubSlotNumber, pRecord);
                LSA_UNUSED_ARG(pRecord);  /* not used anymore */

                /* no check of the InterfaceID */
                LSA_UNUSED_ARG(pHDBUser);
#if 0
                LSA_UINT32 InterfaceID = (LSA_UINT32)(OHA_HIBYTE(SubSlotNumber & 0x0F00) + 1);

                if (InterfaceID == pHDBUser->InterfaceID)
                {
                    RetVal = OHA_OK;
                }
                else
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_PdPortDataAdjust() - wrong InterfaceID (%u) !", (LSA_UINT16)InterfaceID);
                    break;
                }
#endif

                if (SlotNumber != pRQB->args.PrmData.slot_number) /* AP01358661 */
                {
                    RetVal = OHA_ERR_PRM_INDEX;
                    /* Error: no ports. Abort. */
                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid SlotNumber (0x%X) for this record (0x%X)",pRQB->args.PrmData.slot_number,pRQB->args.PrmData.record_index);
                    break;
                }
                /* SubSlotNumber 8001 (Port 1, Interface 1) Format: 8ipp */
                if (SubSlotNumber != pRQB->args.PrmData.subslot_number)
                {
                    RetVal = OHA_ERR_PRM_INDEX;
                    /* Error: no ports. Abort. */
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid  SubSlotNumber for this interface (0x%X)",pRQB->args.PrmData.subslot_number);
                    break;
                }
                break;

RecLenFailed:
                RetVal = OHA_ERR_PRM_BLOCK;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdPortDataAdjust: Length of Record failed.");
            }
            break;

        default:
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdPortDataAdjust: Index of Record (0x%X) failed.", pRQB->args.PrmData.record_index);
            RetVal = OHA_ERR_PRM_INDEX;
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdPortDataAdjust(Response: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Check_PdSyncData                        +*/
/*+  Input/Output          :    PDSyncData-PDU (RTA or RTC)                 +*/
/*+                                                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record to write                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdSyncData(
    OHA_UPPER_RQB_PTR_TYPE   pRQB)
{
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord;
    LSA_UINT16      BlockType, BlockLength, BlockVersion;
    LSA_UINT32      RecordLen;
    LSA_UINT32      index = 0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Check_PdSyncData(pRQB: 0x%X)", pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pRQB->args.PrmData.record_data);

    pRecord = pRQB->args.PrmData.record_data;
    RecordLen = pRQB->args.PrmData.record_data_length;

    if (pRQB->args.PrmData.record_index != OHA_RECORD_INDEX_PD_SYNC_DATA)       /* 0x802D */
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdSyncData: Index of Record (0x%X) failed.", pRQB->args.PrmData.record_index);
        RetVal = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdSyncData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);
    LSA_UNUSED_ARG(pRecord);  /* not used anymore */

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdSyncData: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdSyncData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	if (BlockLength != (pRQB->args.PrmData.record_data_length - 2*sizeof(LSA_UINT16))) /* AP01358661 */
	{
        RetVal = OHA_ERR_PRM_DATA;  /* BlockLength failed */
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen (0x%X) differs from BlockLength (0x%X)!",pRQB->args.PrmData.record_data_length, BlockLength);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdSyncData(Response: 0x%X)", RetVal);
		return (RetVal);
	}

    if (BlockType != OHA_BLOCK_TYPE_PD_SYNC_DATA)
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdSyncData: BlockHeader(BlockType) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdSyncData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION_V12)
    {
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdSyncData: BlockHeader(BlockVersion) failed.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdSyncData(Response: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Check_PdIRData                          +*/
/*+  Input/Output          :    PDIRData-PDU                                +*/
/*+                                                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record to write                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdIRData(
    OHA_HDB_USER_PTR_TYPE    pHDBUser,
    OHA_UPPER_RQB_PTR_TYPE   pRQB)
{
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord;
    LSA_UINT16      BlockType, BlockLen;
    LSA_UINT32      RecordLen;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Check_PdIRData(pHDBUser: 0x%X)", pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pRQB->args.PrmData.record_data);

    pRecord = pRQB->args.PrmData.record_data;
    RecordLen = pRQB->args.PrmData.record_data_length;

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    BlockType = OHA_GET16_NTOH(pRecord, 0);
	BlockLen  = OHA_GET16_NTOH (pRecord, 2);

	/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
	if (BlockLen != (pRQB->args.PrmData.record_data_length - 2*sizeof(LSA_UINT16))) /* AP01358661 */
	{
        RetVal = OHA_ERR_PRM_DATA;  /* BlockLength failed */
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen (0x%X) differs from BlockLength (0x%X)!",pRQB->args.PrmData.record_data_length, BlockLen);

		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdIRData(Response: 0x%X)", RetVal);
		return (RetVal);
	}

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_PD_IR_DATA:
            if (BlockType == OHA_BLOCK_TYPE_PD_IR_DATA)
            {
                /*  PDIRData:                                                                       */
                /*  BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData, PDIRFrameData */

                LSA_UINT16 SlotNumber, SubSlotNumber;
                LSA_UINT32 index = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

                pRecord += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

                index += 2;                     /* 2 padding bytes        */
                if (index>RecordLen) goto RecLenFailed;
                if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_PdIRData() - wrong Padding !");
                    break;
                }
                pRecord += 2;

                /* SlotNumber */
                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SlotNumber, pRecord);

                /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SubSlotNumber, pRecord);
                LSA_UNUSED_ARG(pRecord);  /* not used anymore */

                /* no check of the InterfaceID */
                LSA_UNUSED_ARG(pHDBUser);
#if 0
                LSA_UINT32 InterfaceID = (LSA_UINT32)(OHA_HIBYTE(SubSlotNumber & 0x0F00) + 1);

                if (InterfaceID == pHDBUser->InterfaceID)
                {
                    RetVal = OHA_OK;
                }
                else
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_PdIRData() - wrong InterfaceID (%u) !", (LSA_UINT16)InterfaceID);
                    break;
                }
#endif

                if (SlotNumber != pRQB->args.PrmData.slot_number) /* AP01358661 */
                {
                    RetVal = OHA_ERR_PRM_INDEX;
                    /* Error: no ports. Abort. */
                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid SlotNumber (0x%X) for this record (0x%X)",pRQB->args.PrmData.slot_number,pRQB->args.PrmData.record_index);
                    break;
                }
                /* SubSlotNumber 8001 (Port 1, Interface 1) Format: 8ipp */
                if (SubSlotNumber != pRQB->args.PrmData.subslot_number)
                {
                    RetVal = OHA_ERR_PRM_INDEX;
                    /* Error: no ports. Abort. */
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid  SubSlotNumber for this interface (0x%X)",pRQB->args.PrmData.subslot_number);
                    break;
                }
                break;

RecLenFailed:
                RetVal = OHA_ERR_PRM_BLOCK;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdIRData: Length of Record failed.");
            }
            else
            {
                RetVal = OHA_ERR_PRM_BLOCK;
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdIRData: BlockType (%u) not valid.", BlockType);
            }
            break;

        default:
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_PdIRData: Index of Record (0x%X) failed.", pRQB->args.PrmData.record_index);
            RetVal = OHA_ERR_PRM_INDEX;
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_PdIRData(Response: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadRealPrm                      +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_PRM                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the expected TOPO user neighbour-data               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadRealPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    LSA_UINT16      Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData = &pRQB->args.PrmData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadRealPrm(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    if (NICId == OHA_INVALID_NIC_ID || LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT16 RetVal;
        LSA_UINT32 CableDelay, MediumType;
        LSA_UINT16 MAUType;
        LSA_UINT16 LinkState;
		LSA_UINT16 RTClass3_PortStatus;
        LSA_UINT32 block_length;
        OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pRQB->args.PrmData.record_data;
        LSA_UINT16 PortId = pRQB->args.PrmData.edd_port_id;
        OHA_HDB_EDD_PTR_TYPE  pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);
        OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);
        OHA_HDB_GSY_PTR_TYPE  pHDBGsy  = OHA_GSYGetHandleByNIC(NICId);
        OHA_COMMON_MEM_U8_PTR_TYPE pVarPort = LSA_NULL;
        LSA_UINT16 LenPort = 0;
        LSA_UINT8  SubTypePort = 0xff;

        OHA_ASSERT_NULL_PTR(pHDBEdd);
        OHA_ASSERT_NULL_PTR(pHDBLldp);
        OHA_ASSERT_NULL_PTR(pHDBGsy);
        OHA_ASSERT_NULL_PTR(pHDBLldp->pDB);

        /* AP00422252: V4.0 supports the format indicator "cable" for the subblock LineDelay in record PDPortDataReal */
        CableDelay = pHDBEdd->LinkStatus[PortId-1].CableDelay;

        /* LineDelay.FormatIndicator: Default 0x00, if line delay or cable delay is unknown */
        if (CableDelay != 0)
        {
            CableDelay |= OHA_LINEDELAY_FORMAT_INDICATOR; /* cable delay */
        }

        /* the heading now */
        /* PDPortDataReal: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, SubBlock  */

        pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;  /* offset */
        block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;

        /* padding bytes after BlockHeader */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
        }

        /* the SlotNumber */
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, pPrmData->slot_number);   /* SlotNumber      */

        /* the SubslotNumber */
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, pPrmData->subslot_number);    /* SubSlotNumber 8001 (Port 1, Interface 1) Format: 8ipp */

        /* SubBlock RealPeers:                                                                 */
        /*     LengthOwnPortName (u8), OwnPortName (octet[8] or octet[14]), NumberOfPeers (u8),    */
        /*     [Padding]  [ LengthPeerPortName (u8), PeerPortName (octet[255]),                    */
        /*     LengthPeerStationName (u8), PeerStationName (octet[1..255]), [Padding]          */
        /*     LineDelayFactor (u32) ]*, MAUType(u16), [Padding],                              */
		/*     Reserved(u16), RTClass3_PortStatus (u16),    (see AP00992018)                   */            
        /*     MulticastBoundary, LinkState, [Padding*]                                        */
        /* Padding: The number of padding octets shall be adapted to make the block DWORD aligned. */

        RetVal = oha_db_read_loc_lldp_element(NICId, PortId, OHA_LLDP_TYPE_PORT_ID,
                                              &SubTypePort, 0, &pVarPort, &LenPort);
        if (!(RetVal == LSA_RET_OK && pVarPort && LenPort))         /* now the peers */
        {
            Response = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_RequestReadRealPrm: Read OwnPortName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestReadRealPrm(Response: 0x%X)", Response);
            return (Response);
        }
        else
        {
            LSA_UINT16 SenderId, CountOfRealPeers;
            LSA_UINT32 peer_length = 0;
            OHA_UPPER_MEM_U8_PTR_TYPE pNumberOfPeers;

            /* check, if it's a RFC-name                            */
            /* if (!(oha_is_valid_port_name(LenPort, pVarPort)))    */
            /* {                                                    */
            /*  Response = OHA_ERR_DATA_INVALID;                    */
            /* }                                                    */
            /* write the own PortID */
            block_length += 1;
            peer_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)LenPort);   /* LengthOwnPortName   */

            block_length += LenPort;
            peer_length += LenPort;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_MEM (pRecordData, pVarPort, LenPort);   /* OwnPortName         */

            pNumberOfPeers = pRecordData;
            block_length += 1;
            peer_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0);                    /* init. NumberOfPeers */

            /* padding bytes after writing OwnPortName */         /* DWORD aligned       */
            while ( (block_length & 3) != 0 )
            {
                block_length += 1;
                peer_length += 1;
                if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
            }

            for (CountOfRealPeers = 0, SenderId = 0; SenderId < LLDP_MAX_PORT_SENDER; SenderId++) /* over all real peers */
            {
                OHA_LLDP_SENDER_PTR_TYPE pRemoteSender = &pHDBLldp->pDB->LldpData[PortId-1].RemoteSender[SenderId];

                if ((!LSA_HOST_PTR_ARE_EQUAL(pRemoteSender, LSA_NULL)) &&
                    (pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID ||
                     pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID))
                {
                    OHA_COMMON_MEM_U8_PTR_TYPE pVarChassis = LSA_NULL;
                    LSA_UINT16 LenChassis = 0;
                    LSA_UINT8  SubTypeChassis = 0xff;
                    LSA_UINT8  SubTypePort    = 0xff;

                    if (LSA_RET_OK == oha_db_read_rem_chassis_name(NICId, PortId, (LSA_UINT16)(SenderId+1),
                                                                   &SubTypeChassis,
                                                                   &pVarChassis, &LenChassis))
                    {
                        if (LSA_RET_OK == oha_db_read_rem_port_name(NICId, PortId, (LSA_UINT16)(SenderId+1),
                                                                   &SubTypePort,
                                                                   &pVarPort, &LenPort))
                        {
                            if (!(LSA_HOST_PTR_ARE_EQUAL(pVarChassis, LSA_NULL) ||
                                  LSA_HOST_PTR_ARE_EQUAL(pVarPort, LSA_NULL)
                                  /* || LenChassis == 0 || */ /* LenPort == 0 */ )) /* TFS 908742, TFS 1025560 */
                            {
                                OHA_COMMON_MEM_U8_PTR_TYPE pTmpRecData;
                                OHA_COMMON_MEM_U8_PTR_TYPE pPeerMAC = LSA_NULL;

								/* This field shall contain the NameOfStation part of the received LLDP_PortID or LLDP_ChassisID, */
								/* if coded according to 4.3.1.4.15. Otherwise the LengthPeerStationName shall be zero.   */
								if(LenChassis > 255)
								{
									LenChassis = 0; /* cut off */
								}
								if(LenPort > 255)
								{
									LenPort = 0; /* cut off */
								}

								/* TIA 1025560: Adaption der PDPortDataReal (PeerPortName) */
                                switch (SubTypePort)
                                {
                                    case OHA_PORT_ID_INTERFACE_ALIAS: /* 1 */
                                    case OHA_PORT_ID_PORT_COMP:       /* 2 */
                                    case OHA_PORT_ID_INTERFACE_NAME:  /* 5 */
                                    case OHA_PORT_ID_AGENT_CIRCUIT_ID:/* 6 */
                                    case OHA_PORT_ID_LOCALLY_ASSIGNED:/* 7 */
                                        block_length += 1;
                                        peer_length += 1;
                                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                                        OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)LenPort);/* LengthPeerPortName */

										if(LenPort) /* PeerPortNane - Usage in conjunction with PDPortDataReal */
										{
                                            block_length += LenPort;
                                            peer_length += LenPort;
                                            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                                            OHA_PDU_SET_MEM (pRecordData, pVarPort, LenPort); /* PeerPortName     */
										}
                                        break;
                                    case OHA_PORT_ID_MAC_ADDR:        /* 3 */
                                        pTmpRecData = pRecordData; /* store the location for length */
                                        pRecordData++;
                                        block_length += 1;
                                        peer_length += 1; /* localtion for length */
                                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
										/* TFS 1025560: Non pnio neighbor (see SPH Physical Device: Multiple PN-Interfaces, V1.1i11) */
										if(LenPort == OHA_MAC_ADDR_SIZE)
										{
											if ((block_length + OHA_MAC2A_LEN) > pPrmData->record_data_length) goto RecLenFailed;
											LenPort = oha_mac2a (pVarPort, pRecordData);
											pRecordData += LenPort;
											block_length += LenPort;
											peer_length += LenPort;
	                                        OHA_PUT8_HTON (pTmpRecData, 0, (LSA_UINT8)LenPort);    /* LengthPeerPortID    */
										}
										else
										{
											OHA_PDU_SET_U8 (pRecordData, 0x0);      /* length byte  */
										}
                                        break;
                                    case OHA_PORT_ID_NETWORK_ADDR:    /* 4 */
                                        pTmpRecData = pRecordData; /* store the location for length */
                                        pRecordData++;
                                        block_length += 1;
                                        peer_length += 1; /* location for length */
                                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
										/* TFS 1025560: Non pnio neighbor (see SPH Physical Device: Multiple PN-Interfaces, V1.1i11) */
										if((LenPort == 1 + OHA_IP_ADDR_SIZE) && (*pVarPort == OHA_ADDR_FAMILY_IPV4)) /* the first octet contains the IANA Address Family Numbers enumeration value */
										{
											if ((block_length + OHA_IP2A_LEN) > pPrmData->record_data_length) goto RecLenFailed;
											LenPort = oha_ip2a (pVarPort + 1, pRecordData);
											pRecordData += LenPort;
											block_length += LenPort;
											peer_length += LenPort;
											OHA_PUT8_HTON (pTmpRecData, 0, (LSA_UINT8)LenPort);    /* LengthPeerChassisID    */
										}
										else
										{
											OHA_PUT8_HTON (pTmpRecData, 0, 0x0);      /* length byte (TIA 987843) */
										}
                                        break;
                                    default:
										block_length += 1;
										peer_length += 1; /* location for length */
										if (block_length > pPrmData->record_data_length) goto RecLenFailed;
										OHA_PDU_SET_U8 (pRecordData, 0x0);      /* length (=0) byte */
										break;
                                }

                                switch (SubTypeChassis)
                                {
                                    case OHA_CHASSIS_ID_COMP:
                                    case OHA_CHASSIS_ID_INTERFACE_ALIAS:
                                    case OHA_CHASSIS_ID_PORT:
                                    case OHA_CHASSIS_ID_INTERFACE_NAME:
                                    case OHA_CHASSIS_ID_LOCALLY_ASSIGNED:
                                        block_length += 1;
                                        peer_length += 1;
                                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                                        OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)LenChassis);    /* LengthPeerStationName  */

										if(LenChassis) /* TIA 908742: PeerStationNane - Usage in conjunction with PDPortDataReal */
										{
                                            block_length += LenChassis;
                                            peer_length += LenChassis;
                                            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                                            OHA_PDU_SET_MEM (pRecordData, pVarChassis, LenChassis); /* PeerStationName        */
										}
                                        break;
                                    case OHA_CHASSIS_ID_MAC_ADDR:
                                        pTmpRecData = pRecordData; /* store the location for length */
                                        pRecordData++;
                                        block_length += 1;
                                        peer_length += 1; /* localtion for length */
                                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
										/* TFS 832271: Non pnio neighbor (see SPH Physical Device: Multiple PN-Interfaces, V1.1i08) */
										if(LenChassis == OHA_MAC_ADDR_SIZE)
										{
											if ((block_length + OHA_MAC2A_LEN) > pPrmData->record_data_length) goto RecLenFailed;
											LenChassis = oha_mac2a (pVarChassis, pRecordData);
											pRecordData += LenChassis;
											block_length += LenChassis;
											peer_length += LenChassis;
	                                        OHA_PUT8_HTON (pTmpRecData, 0, (LSA_UINT8)LenChassis);    /* LengthPeerChassisID    */
										}
										else
										{
											OHA_PUT8_HTON (pTmpRecData, 0, 0x0);      /* length byte (TIA 987843) */
										}
                                        break;
                                    case OHA_CHASSIS_ID_NETWORK_ADDR:
                                        pTmpRecData = pRecordData; /* store the location for length */
                                        pRecordData++;
                                        block_length += 1;
                                        peer_length += 1; /* location for length */
                                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
										/* TFS 832271: Non pnio neighbor (see SPH Physical Device: Multiple PN-Interfaces, V1.1i08) */
										if((LenChassis == 1 + OHA_IP_ADDR_SIZE) && (*pVarChassis == OHA_ADDR_FAMILY_IPV4)) /* the first octet contains the IANA Address Family Numbers enumeration value */
										{
											if ((block_length + OHA_IP2A_LEN) > pPrmData->record_data_length) goto RecLenFailed;
											LenChassis = oha_ip2a (pVarChassis + 1, pRecordData);
											pRecordData += LenChassis;
											block_length += LenChassis;
											peer_length += LenChassis;
											OHA_PUT8_HTON (pTmpRecData, 0, (LSA_UINT8)LenChassis);    /* LengthPeerChassisID    */
										}
										else
										{
											OHA_PUT8_HTON (pTmpRecData, 0, 0x0);      /* length byte (TIA 987843) */
										}
                                        break;
                                    default:
										block_length += 1;
										peer_length += 1; /* location for length */
										if (block_length > pPrmData->record_data_length) goto RecLenFailed;
										OHA_PDU_SET_U8 (pRecordData, 0x0);      /* length byte     */
										break;
                                }

                                /* padding bytes after writing PeerStationName */
                                while ( (block_length & 3) != 0 )
                                {
                                    block_length += 1;
                                    peer_length += 1;
                                    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                                    OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte        */
                                }

                                /* LineDelay */ /* AP00422252 (>= V4.0i20) cable delay */
                                block_length += 4;
                                peer_length += 4;
                                if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                                OHA_PDU_SET_U32 (pRecordData, CableDelay);

                                /* PeerMACAddress */
                                block_length += OHA_MAC_ADDR_SIZE;
                                peer_length += OHA_MAC_ADDR_SIZE;
                                if (block_length > pPrmData->record_data_length) goto RecLenFailed;

                                {
                                    LSA_UINT16 RetVal;
                                    LSA_UINT16 Len   = 0;
                                    LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR;

                                    /* read the remote If-MAC */
                                    RetVal = oha_db_read_rem_lldp_element(NICId, PortId, (LSA_UINT16)(SenderId+1),
                                                                          OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                          &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                          &pPeerMAC, &Len);

                                    if (!(RetVal == LSA_RET_OK && OHA_IS_NOT_NULL(pPeerMAC) && Len == OHA_MAC_ADDR_SIZE))
                                    {
										pPeerMAC = LSA_NULL; /* TFS 832271: Non pnio neighbor (see SPH Physical Device: Multiple PN-Interfaces, V1.1i08) */
                                    }
                                }

                                if ( LSA_HOST_PTR_ARE_EQUAL(pPeerMAC,  LSA_NULL) )
                                {                                           /* no remote sender */
                                    LSA_UINT16 i;
                                    for (i = 0; i < OHA_MAC_ADDR_SIZE; i++)
                                    {
                                        OHA_PDU_SET_U8 (pRecordData, 0x0);  /* initial value    */
                                    }
                                }
                                else
                                {
                                    OHA_PDU_SET_MAC(pRecordData, pPeerMAC);
                                }

                                /* padding bytes after writing PeerMACAddress */
                                while ( (block_length & 3) != 0 )
                                {
                                    block_length += 1;
                                    peer_length += 1;
                                    LSA_UNUSED_ARG(peer_length);
                                    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                                    OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte        */
                                }

                                CountOfRealPeers++;
                            }
                        }
                    }
                }
            }

            OHA_PDU_SET_U8 (pNumberOfPeers, (LSA_UINT8)CountOfRealPeers);     /* now write NumberOfPeers */
			LSA_UNUSED_ARG(pNumberOfPeers);    /* not used anymore */
        }

        /* Speed for the MAUType */
        MAUType = pHDBEdd->LinkStatus[PortId-1].MAUType;
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, MAUType);

        /* padding bytes after writing MAUType */           /* DWORD aligned       */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
        }

		/* AP00992018: Reserved (4 octets) -> Reserved (2 octets), RTClass3_PortStatus (u16) */
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, 0);

        /* RTClass3_PortStatus  */
        {
            LSA_UINT16 Len   = 0;
            OHA_COMMON_MEM_U8_PTR_TYPE PortStatusPtr = LSA_NULL;
            LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR;

            SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;
            (LSA_VOID)oha_db_read_loc_lldp_element(NICId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO, &PortStatusPtr, &Len);
            if (OHA_IS_NOT_NULL(PortStatusPtr) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
			{
                PortStatusPtr += 2; /* 2nd word */
                RTClass3_PortStatus = OHA_GET16_NTOH (PortStatusPtr, 0); /* 2nd word: C3-Status */
			}
			else
			{
                RTClass3_PortStatus = OHA_PORT_STATUS_CLASS3_OFF;
			}
		}

        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, RTClass3_PortStatus);

        /* MulticastBoundary (from record PDPortDataAdjust) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBGsy->AdjustData[PortId-1].MulticastBoundary);

        /* LinkState from LinkStatus */
        LinkState = OHA_GetLinkStatus(pHDBEdd->LinkStatus[PortId-1].LinkStatus);

        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, LinkState);

        /* padding bytes after writing LinkState */         /* DWORD aligned       */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
        }

        MediumType = pHDBEdd->LinkStatus[PortId-1].MediaType;

        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, MediumType);

        if (block_length > pPrmData->record_data_length)
        {
RecLenFailed:
            Response = OHA_ERR_PRM_DATA;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestReadRealPrm(Response: 0x%X)", Response);
            return (Response);
        }
        LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

        /* the read length */
        pPrmData->record_data_length = block_length;

        /* finally write the BlockHeader PDPortDataReal */
		oha_prm_write_header(pRQB->args.PrmData.record_data, 
							 OHA_BLOCK_TYPE_PD_PORT_DATA_REAL, 
							 block_length, 
							 OHA_TOPO_DATA_BLOCK_VERSION,
							 LSA_FALSE);
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadRealPrm(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    RequestReadRealInterfacePrm                 +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_PRM                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the real interface TOPO data                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadRealInterfacePrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    LSA_UINT16      Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData = &pRQB->args.PrmData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : RequestReadRealInterfacePrm(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    if (NICId == OHA_INVALID_NIC_ID || LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT32 block_length;
        OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pRQB->args.PrmData.record_data;
        LSA_INT Len;
        OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE SubnetPtr = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE RouterPtr = LSA_NULL;

        /* PDInterfaceDataReal: BlockHeader, LengthOwnStationName, OwnStationName, [Padding*] a),  */
        /*                      MACAddressValue b), [Padding*] a), IPParameterValue            */
        /*  a) The number of padding octets shall be adapted to make the block Unsigned32 aligned. */
        /*  b) This field contains the interface MAC address            */

        /* BlockHeader */
        block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;  /* offset */

        Len = OHA_DB_READ_STATION_NAME(NICId, &NamePtr);

		/* AP00942580: No NameOfStation, no MAC-string */
        if (Len == 0 || OHA_IS_NULL(NamePtr)) /* if no chassisname present */
        {
            Len = 0;
			NamePtr = LSA_NULL;
        }

        /* LengthOwnStationName (u8) */
        block_length += 1;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)Len);

        /* OwnStationName */
        block_length += Len;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
		if (OHA_IS_NOT_NULL(NamePtr))
		{
            OHA_MEMCPY(pRecordData, NamePtr, Len);
		}
        pRecordData += Len;

        /* padding byte(s) after OwnStationName */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
        }

        /* MACAddressValue */
        block_length += OHA_MAC_ADDR_SIZE;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;

        Len = OHA_DB_READ_MAC_ADDR(NICId, &NamePtr);
        if (Len == OHA_MAC_ADDR_SIZE && OHA_IS_NOT_NULL(NamePtr)) /* MAC-Addr present */
        {
            OHA_MEMCPY(pRecordData, NamePtr, Len);
        }
        else
        {
            OHA_MEMSET(pRecordData, 0, OHA_MAC_ADDR_SIZE);
        }
        pRecordData += OHA_MAC_ADDR_SIZE;

        /* padding byte(s) after MACAddressValue */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
        }

        /* IPParameterValue: IPAddress, Subnetmask, StandardGateway */
        block_length += OHA_IP_SUITE_SIZE;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;

        Len = OHA_DB_READ_IP_SUITE(NICId, &NamePtr, &SubnetPtr, &RouterPtr);
        if (OHA_IS_NOT_NULL(NamePtr) && OHA_IS_NOT_NULL(SubnetPtr) && /* IP-Addr present */
            OHA_IS_NOT_NULL(RouterPtr) && Len == OHA_IP_ADDR_SIZE)
        {
            OHA_PDU_SET_MEM(pRecordData, NamePtr, Len);
            OHA_PDU_SET_MEM(pRecordData, SubnetPtr, Len);
            OHA_PDU_SET_MEM(pRecordData, RouterPtr, Len);
        }
        else
        {
            OHA_MEMSET(pRecordData, 0, OHA_IP_SUITE_SIZE);
			pRecordData += OHA_IP_SUITE_SIZE;
        }

        if (block_length > pPrmData->record_data_length)
        {
RecLenFailed:
            Response = OHA_ERR_PRM_DATA;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestReadRealInterfacePrm(Response: 0x%X)", Response);
            return (Response);
        }
		LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

        /* the read length */
        pPrmData->record_data_length = block_length;

        /* finally write the BlockHeader PDPortDataReal */
		oha_prm_write_header(pRQB->args.PrmData.record_data,
							 OHA_BLOCK_TYPE_PD_INTERFACE_DATA_REAL,
							 block_length,
							 OHA_TOPO_DATA_BLOCK_VERSION,
							 LSA_FALSE);
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: RequestReadRealInterfacePrm(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PdPortDataReal                      +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_PdPortDataReal(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pNumberOfPeers,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPeerRecord,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppFirstPeer,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUType,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pLineDelay,
    OHA_COMMON_MEM_U16_PTR_TYPE pLinkState,
    OHA_COMMON_MEM_U32_PTR_TYPE pMediumType,
    OHA_COMMON_MEM_U16_PTR_TYPE pRTClass3PortStatus
)
{
    LSA_UINT16      BlockType;
    LSA_UINT16      BlockLength;
    LSA_UINT16      BlockVersion;

    LSA_UINT16      mau_type;
    LSA_UINT16      number_of_peers;
    LSA_UINT32      line_delay;

    LSA_UINT32      index = 0;
    LSA_UINT32      i;
    LSA_BOOL        RetVal = LSA_FALSE;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PdPortDataReal(OHA_CheckPdPortData: 0x%X)", pRecData);

    if (!LSA_HOST_PTR_ARE_EQUAL(ppPeerRecord, LSA_NULL))
    {
        *ppPeerRecord  = 0;             /* init the peers */
    }
    if (!LSA_HOST_PTR_ARE_EQUAL(ppFirstPeer, LSA_NULL))
    {
        *ppFirstPeer   = 0;
    }
    *pNumberOfPeers    = 0;
    *pMAUType          = 0;
    * pDelayExisting   = LSA_FALSE;
    *pLineDelay        = 0;
    *pLinkState        = 0;
    *pMediumType       = 0;
	*pRTClass3PortStatus = 0;

    /* look at the the BlockHeader: */

    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockType != OHA_BLOCK_TYPE_PD_PORT_DATA_REAL)
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataReal: BlockHeader (BlockType) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataReal: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataReal: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDPortDataReal:  BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, SubBlock */
    {
        LSA_UINT16 SlotNumber;
        LSA_UINT16 SubSlotNumber;

        index += 2;                     /* 2 padding bytes        */
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;

        /* SlotNumber */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);

        /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SubSlotNumber, pRecord);

        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"OHA_Get_PdPortDataReal(SlotNumber: 0x%X, SubSlotNumber: 0x%X)", SlotNumber, SubSlotNumber);
    }

    /* SubBlock RealPeers:                                                                 */
    /*     LengthOwnPortName (u8), OwnPortName (octet[8] or octet[14]), NumberOfPeers (u8),    */
    /*     [Padding]  [ LengthPeerPortName (u8), PeerPortName (octet[255]),                    */
    /*     LengthPeerStationName (u8), PeerStationName (octet[1..255]), [Padding]          */
    /*     LineDelayFactor (u32) ]*, MAUType(u16), [Padding], Reserved(2 Octets),          */
    /*     RTClass3_PortStatus, MulticastBoundary, LinkState, [Padding*]                   */
    /* Padding: The number of padding octets shall be adapted to make the block DWORD aligned. */
    {
        OHA_UPPER_MEM_U8_PTR_TYPE  pThisRecord = pRecord;
        LSA_UINT8 PortLen;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (PortLen, pRecord);  /* LengthOwnPortName  */
        if (!PortLen)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataReal: LengthPeerPortName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataReal(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        pRecord += PortLen;                 /* OwnPortName          */
        index += PortLen;
        if (index>RecordLen) goto RecLenFailed;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (number_of_peers, pRecord);      /* NumberOfPeers       */

        if (number_of_peers > LLDP_MAX_PORT_SENDER)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataReal: NumberOfPeers failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataReal(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* padding byte after reading OwnPortName */
        while ( (index & 3) != 0 )
        {
            pRecord += 1;                       /* padding byte        */
            index += 1;
            if (index>RecordLen) goto RecLenFailed;
        }

        if (!LSA_HOST_PTR_ARE_EQUAL(ppPeerRecord, LSA_NULL))
        {
            *ppPeerRecord = pThisRecord;                /* here are the peers */
        }
        if (!LSA_HOST_PTR_ARE_EQUAL(ppFirstPeer, LSA_NULL))
        {
            *ppFirstPeer  = pRecord;
        }

        /* check TOPO-peer entries */

		/* Coding of the field PeerStationName: */
		/* This field shall be coded according to 4.3.1.4.15. It shall contain the NameOfStation */
		/* part of the received LLDP_PortID or LLDP_ChassisID, if coded according to 4.3.1.4.15. */
		/* Otherwise the LengthPeerStationName shall be zero.                                    */
		/* See also Coding of the field PeerPortName. */

        for (i = 0; i < number_of_peers && i < LLDP_MAX_PORT_SENDER; i++)  /* over all peers */
        {
            LSA_UINT8 ChassisLen;

            index += 1;
            if (index>RecordLen) goto RecLenFailed;
            OHA_PDU_GET_U8 (PortLen, pRecord);  /* LengthPeerPortName  */
            index += PortLen;
            if (index>RecordLen) goto RecLenFailed;
            pRecord += PortLen;                 /* PeerPortName        */

            index += 1;
            if (index>RecordLen) goto RecLenFailed;
            OHA_PDU_GET_U8 (ChassisLen, pRecord);/* LengthPeerStationName */
            index += ChassisLen;
            if (index>RecordLen) goto RecLenFailed;
            pRecord += ChassisLen;              /* PeerStationName     */

            /* padding byte after reading PeerStationName */
            while ( (index & 3) != 0 )
            {
                pRecord += 1;                       /* padding byte        */
                index += 1;
                if (index>RecordLen) goto RecLenFailed;
            }

            index += 4;
            if (index>RecordLen) goto RecLenFailed;
            OHA_PDU_GET_U32 (line_delay, pRecord);  /* LineDelayFactor    */

            * pDelayExisting   = LSA_TRUE;
            *pLineDelay = line_delay;

            /* PeerMACAddress */
            index += OHA_MAC_ADDR_SIZE;
            if (index>RecordLen) goto RecLenFailed;
            pRecord += OHA_MAC_ADDR_SIZE;

            /* padding bytes after reading PeerMACAddress */
            while ( (index & 3) != 0 )
            {
                pRecord += 1;                       /* padding byte        */
                index += 1;
                if (index>RecordLen) goto RecLenFailed;
            }
        }

        *pNumberOfPeers = number_of_peers;

        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (mau_type, pRecord);    /* MAUType    */

        *pMAUType = mau_type;

        /* padding byte after reading PeerStationName */
        while ( (index & 3) != 0 )
        {
            pRecord += 1;                       /* padding byte        */
            index += 1;
            if (index>RecordLen) goto RecLenFailed;
        }

        /* Reserved (2 octets)   */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;

		{
			LSA_UINT16 rtc3_status;
			index += 2;
			if (index>RecordLen) goto RecLenFailed;
			OHA_PDU_GET_U16 (rtc3_status, pRecord);  /* pRTClass3PortStatus  */

			*pRTClass3PortStatus = rtc3_status;
		}

        /* MulticastBoundary    */
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;

        /* LinkState */
        {
            LSA_UINT16 link_state;
            index += 2;
            if (index>RecordLen) goto RecLenFailed;
            OHA_PDU_GET_U16 (link_state, pRecord);  /* LinkState    */
            *pLinkState = link_state;
        }

        /* padding bytes after reading LinkState */
        while ( (index & 3) != 0 )
        {
            pRecord += 1;                       /* padding byte        */
            index += 1;
            if (index>RecordLen) goto RecLenFailed;
        }

        /* MediumType */
        {
            LSA_UINT32 medium_type;
            index += 4;
            if (index>RecordLen) goto RecLenFailed;
            OHA_PDU_GET_U32 (medium_type, pRecord); /* MediumType    */
            *pMediumType = medium_type;
        }
    }
    LSA_UNUSED_ARG(pRecord);    /* not used anymore */

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdPortDataReal: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = LSA_TRUE;  /* the record check is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdPortDataReal(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PdInterfaceDataReal                 +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks record                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_PdInterfaceDataReal(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE Chassis_Len,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppChassis,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppMacAddr,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr
)
{
    LSA_UINT32      index = 0;
    LSA_UINT8       Len;

    LSA_UINT16      BlockType;
    LSA_UINT16      BlockLength;
    LSA_UINT16      BlockVersion;

    LSA_BOOL        RetVal = LSA_FALSE;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PdInterfaceDataReal(pRecData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecord);

    *Chassis_Len = 0;
    *ppChassis   = LSA_NULL;
    *ppMacAddr   = LSA_NULL;
    *ppIpAddr    = LSA_NULL;

    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockType != OHA_BLOCK_TYPE_PD_INTERFACE_DATA_REAL)
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdInterfaceDataReal: BlockHeader (BlockType) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdInterfaceDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdInterfaceDataReal: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdInterfaceDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdInterfaceDataReal: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdInterfaceDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDInterfaceDataReal: BlockHeader, LengthOwnStationName, OwnStationName, [Padding*] a),   */
    /*                      MACAddressValue b), [Padding*] a), IPParameterValue             */
    /*  a) The number of padding octets shall be adapted to make the block Unsigned32 aligned. */
    /*  b) This field contains the interface MAC address                                    */

    /* LengthOwnStationName */
    index += 1;
    if (index>RecordLen) goto RecLenFailed;

    OHA_PDU_GET_U8 (Len, pRecord);  /* LengthOwnStationName  */
    *Chassis_Len = Len;

    /* OwnStationName */
    index += Len;
    if (index>RecordLen) goto RecLenFailed;
    if (Len != 0)
    {
        *ppChassis = pRecord;
    }
    pRecord += Len;

    /* padding byte after reading OwnStationName */
    while ( (index & 3) != 0 )
    {
        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 1;               /* padding byte        */
    }

    /* MACAddressValue */
    index += OHA_MAC_ADDR_SIZE;
    if (index>RecordLen) goto RecLenFailed;
    *ppMacAddr = pRecord;
    pRecord += OHA_MAC_ADDR_SIZE;

    /* padding byte after reading MACAddressValue */
    while ( (index & 3) != 0 )
    {
        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 1;               /* padding byte        */
    }

    index += OHA_IP_SUITE_SIZE;
    if (index>RecordLen) goto RecLenFailed;
    *ppIpAddr = pRecord;
    /* pRecord += OHA_IP_SUITE_SIZE; */

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PdInterfaceDataReal: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdInterfaceDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = LSA_TRUE;  /* the record check is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PdInterfaceDataReal(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadExpectedPrm                  +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_TOPO               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the expected TOPO user neighbour-data               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadExpectedPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16      Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData = &pRQB->args.PrmData;
    OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[pPrmData->edd_port_id-1];

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadExpectedPrm(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);


    /* Segment of PDPortDataCheck: [CheckPeers], [CheckLineDelayFactor], [CheckMAUType], [CheckLinkState] */

    if (LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT32  block_length;
        OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pRQB->args.PrmData.record_data;

        /* the heading now */
        /* PDPortDataReal: BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, Segment  */

        pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;  /* offset */
        block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;

        /* padding bytes after BlockHeader */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
        }

        /* the SlotNumber */
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, pPrmData->slot_number);   /* SlotNumber */

        /* the SubslotNumber */

        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, pPrmData->subslot_number);    /* SubSlotNumber 8001 (Port 1, Interface 1) Format: 8ipp */

        if ((!LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL)) && pTopoDB->NumberOfPeers) /* are there peers ? */
        {
            LSA_UINT16 i;
            LSA_UINT32 peer_length = 0;
            OHA_UPPER_MEM_U8_PTR_TYPE pPeerHeader = pRecordData;

            /* SubBlock CheckPeers:                                                      */
            /*     BlockHeader, NumberOfPeers (u8), (                                    */
            /*                  LengthPeerPortName (u8), PeerPortName (octet[255]),          */
            /*                  LengthPeerStationName (u8), PeerStationName (octet[1..255])  */
            /*                                        )*, [Padding]                      */
            /* Padding: The number of padding octets shall be adapted to make the block WORD aligned. */

            pRecordData  += OHA_TOPO_BLOCK_HEADER_PDU_SIZE; /* offset */
            block_length += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            peer_length  += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

            block_length += 1;
            peer_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)(pTopoDB->NumberOfPeers));  /* NumberOfPeers       */

            for (i = 0; i < pTopoDB->NumberOfPeers && i < LLDP_MAX_PORT_SENDER; i++)  /* over all expected peers */
            {
                OHA_UPPER_PEER_PTR_TYPE pRemotePeer = &pTopoDB->PeerSender[i];

                if (!(LSA_HOST_PTR_ARE_EQUAL(pRemotePeer, LSA_NULL)))
                {
                    LSA_UINT8 ChassisLen = (LSA_UINT8)(pRemotePeer->LengthPeerChassisID);
                    LSA_UINT8 PortLen    = (LSA_UINT8)(pRemotePeer->LengthPeerPortID);

                    if (!(LSA_HOST_PTR_ARE_EQUAL(pRemotePeer->pPeerChassisID, LSA_NULL) ||
                          LSA_HOST_PTR_ARE_EQUAL(pRemotePeer->pPeerPortID, LSA_NULL)    ||
                          ChassisLen == 0 || PortLen == 0))
                    {
                        /* check, if it's a RFC-name         */
                        /* if (!(oha_is_valid_port_name((LSA_UINT16)PortLen, pRemotePeer->pPeerPortID))) */
                        /* {                                 */
                        /*  Response = OHA_ERR_DATA_INVALID; */
                        /* }                                 */
                        /* if (!(oha_is_valid_station_name((LSA_UINT16)ChassisLen, pRemotePeer->pPeerChassisID, LSA_FALSE))) */
                        /* {                                 */
                        /*  Response = OHA_ERR_DATA_INVALID; */
                        /* }                                 */

                        block_length += 1;
                        peer_length += 1;
                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                        OHA_PDU_SET_U8 (pRecordData, PortLen);  /* LengthPeerPortName  */

                        block_length += PortLen;
                        peer_length += PortLen;
                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                        OHA_PDU_SET_MEM (pRecordData, pRemotePeer->pPeerPortID, PortLen); /* PeerPortName        */

                        block_length += 1;
                        peer_length += 1;
                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                        OHA_PDU_SET_U8 (pRecordData, ChassisLen);/* LengthPeerStationName */

                        block_length += ChassisLen;
                        peer_length += ChassisLen;
                        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                        OHA_PDU_SET_MEM (pRecordData, pRemotePeer->pPeerChassisID, ChassisLen); /* PeerStationName     */
                    }
                }
            }

            /* padding bytes after writing all peers */
            while ( (block_length & 3) != 0 )
            {
                block_length += 1;
                peer_length += 1;
                if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
            }

            /* finally write the BlockHeader for Peers */
			oha_prm_write_header(pPeerHeader, OHA_TOPO_CHECK_PEERS,
								 peer_length, OHA_TOPO_DATA_BLOCK_VERSION,
								 LSA_FALSE);
        }

        if ((!LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL)) && pTopoDB->DelayExisting) /* is there a LineDelayFactor ? */
        {
            /* SubBlock CheckLineDelayFactor:                                       */
            /*     BlockHeader, Padding, Padding, LineDelayFactor (u32)             */

            /* write the BlockHeader for LineDelayFactor */
            LSA_UINT16 delay_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 1+1 + sizeof(LSA_UINT32);
            block_length += delay_length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;

            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_CHECK_DELAY);
            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)(delay_length - 2*sizeof(LSA_UINT16)));
            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_DATA_BLOCK_VERSION);

            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
            OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */

            OHA_PDU_SET_U32 (pRecordData, pTopoDB->LineDelayFactor);    /* LineDelay * Resolution */
        }

        if ((!LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL)) && pTopoDB->MAUType)   /* is there a MAUType ? */
        {
            LSA_UINT16 mau_length;

            /* SubBlock CheckMAUType:                                                      */
            /*     BlockHeader, MAUType (u16)                                              */

            /* write the BlockHeader for MAUType */

            mau_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + sizeof(LSA_UINT16); /* sizeof (header + MAUType) */

            block_length += mau_length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;

            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_CHECK_MAU_TYPE);
            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)(mau_length - 2*sizeof(LSA_UINT16)));
            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_DATA_BLOCK_VERSION);

            OHA_PDU_SET_U16 (pRecordData, pTopoDB->MAUType);
        }

        if ((!LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL)) && pTopoDB->LinkStateExisting) /* is there a LinkState ? */
        {
            LSA_UINT16 port_length;

            /* SubBlock CheckLinkState:                                                      */
            /*     BlockHeader, LinkState (u16)                                              */

            /* write the BlockHeader for LinkState */

            port_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + sizeof(LSA_UINT16); /* sizeof (header + LinkState) */

            block_length += port_length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;

            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_CHECK_LINK_STATE);
            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)(port_length - 2*sizeof(LSA_UINT16)));
            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_DATA_BLOCK_VERSION);

            OHA_PDU_SET_U16 (pRecordData, pTopoDB->LinkState);
        }

        if ((!LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL)) && pTopoDB->DelayDifferenceExisting)   /* is there a CheckDelayDifference ? */
        {
            LSA_UINT16 check_delay_length;

            /* SubBlock CheckSyncDifference:                                             */
            /*     BlockHeader,  CheckSyncMode(u16)                                      */

            /* write the BlockHeader for CheckSyncDifference */

            check_delay_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + sizeof(LSA_UINT16); /* sizeof (header + CheckSyncMode) */

            block_length += check_delay_length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;

            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_CHECK_DELAY_DIFFERENCE);
            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)(check_delay_length - 2*sizeof(LSA_UINT16)));
            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_DATA_BLOCK_VERSION);

            OHA_PDU_SET_U16 (pRecordData, pTopoDB->DelayMode);
        }

        if ((!LSA_HOST_PTR_ARE_EQUAL(pTopoDB, LSA_NULL)) && pTopoDB->MAUTypeDifferenceExisting) /* is there a CheckMAUTypeDifference ? */
        {
            LSA_UINT16 check_mau_length;

            /* SubBlock CheckMAUTypeDifference:                                          */
            /*     BlockHeader,  MAUTypeMode(u16)                                        */

            /* write the BlockHeader for CheckMAUTypeDifference */

            check_mau_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + sizeof(LSA_UINT16); /* sizeof (header + MAUTypeMode) */

            block_length += check_mau_length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;

            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_CHECK_MAU_TYPE_DIFFERENCE);
            /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
            OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)(check_mau_length - 2*sizeof(LSA_UINT16)));
            OHA_PDU_SET_U16 (pRecordData, OHA_TOPO_DATA_BLOCK_VERSION);

            OHA_PDU_SET_U16 (pRecordData, pTopoDB->MAUTypeMode);
        }

        if (block_length > pPrmData->record_data_length)
        {
RecLenFailed:
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_RequestReadExpectedPrm: Length of Record failed.");
            Response = OHA_ERR_PRM_DATA;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadExpectedPrm(Response: 0x%X)", Response);
            return (Response);
        }
		LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

        /* if no netto data */
        if (block_length == (OHA_TOPO_BLOCK_HEADER_PDU_SIZE/*header*/ + 2/* paddings */
                             + 2/*Slotnumber*/ + 2/*SubSlotnumber*/))
        {
            pPrmData->record_data_length = 0;
        }
        else
        {                   /* the read length */
            pPrmData->record_data_length = block_length;
        }

        /* finally write the BlockHeader PDPortDataReal */
		oha_prm_write_header(pRQB->args.PrmData.record_data,
							 OHA_BLOCK_TYPE_PD_PORT_DATA_CHECK,
							 block_length,
							 OHA_TOPO_DATA_BLOCK_VERSION,
							 LSA_FALSE);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadExpectedPrm(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    OHA_RequestReadAdditionalInfo               +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_TOPO               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads AdditionalInfo record                               +*/
/*+                                                                         +*/
/*+  AdditionalInfo: BlockHeader, LocalIfInfo, [LocalPortNumber(u8),        +*/
/*+                  LocalInfo, LocalLinkInfo, RemoteInfo, TopoInfo]*       +*/
/*+                                                                         +*/
/*+  Blockversions:  V1.0    initial version                                +*/
/*+                  V1.1    LocalLinkInfo.EddIsWireless added              +*/
/*+                  V1.2    TopoInfo.RTClass3_PDEVTopoState added,         +*/
/*+                          "Local/RemoteInfos".Reserved(u32) added        +*/
/*+                                                                         +*/
/*+  LocalIfInfo:                                                           +*/
/*+     PTCP_SubdomainUUID ({0} if no sync record written)                  +*/
/*+     IRDataUUID ({0} if no sync record written)                          +*/
/*+     SyncMaster_MAC ({0} if not sync)                                    +*/
/*+     PTCPTimeoutFactor(u16) (0 if no sync record written)                +*/
/*+     SyncSendFactor(u32) (time base: 31,25 µs; 0 if no sync record written) +*/
/*+                                                                         +*/
/*+     (V1.2 or higher)   IfReserved(u32) (0...default)                    +*/
/*+                                                                         +*/
/*+  LocalPortNumber(u8) (1...n)                                            +*/
/*+                                                                         +*/
/*+  LocalInfo:                                                             +*/
/*+     CableDelay(u32) (in ns)                                             +*/
/*+     RxDelay(u32) (in ns)                                                +*/
/*+     TxDelay(u32) (in ns)                                                +*/
/*+     RTC2_PortStatus(u8) (values see oha_usr.h)                          +*/
/*+     RTC3_PortStatus(u8) (values see oha_usr.h)                          +*/
/*+     LengthOfPeriod(u32) (in ns)                                         +*/
/*+     RedOffset(u32) (in ns)                                              +*/
/*+     OrangeOffset(u32) (in ns)                                           +*/
/*+     GreenOffset(u32) (in ns)                                            +*/
/*+     OnlySyncForwarding(u8) (0...OFF, 1...ON)                            +*/
/*+                                                                         +*/
/*+     (V1.2 or higher)   LocReserved(u32) (0...default)                   +*/
/*+                                                                         +*/
/*+  LocalLinkInfo (values see edd_usr.h):                                  +*/
/*+     EddMAUType(u8) (EDD_MAUTYPE_UNKNOWN,...)                            +*/
/*+     EddLinkStatus(u8) (EDD_LINK_UP, EDD_LINK_DOWN,...)                  +*/
/*+     EddSpeed(u8) (EDD_LINK_SPEED_10, EDD_LINK_SPEED_100,...)            +*/
/*+     EddMode(u8) (EDD_LINK_MODE_HALF, EDD_LINK_MODE_FULL)                +*/
/*+     EddPortPresence(u8) (EDD_PORT_PRESENT, EDD_PORT_NOT_PRESENT)        +*/
/*+     EddClass2PortStatus(u8) (EDD_RTCLASS2_NOT_SUPPORTED, EDD_RTCLASS2_ACTIVE,... ) */
/*+     EddIRTPortStatus(u8) (EDD_IRT_NOT_SUPPORTED, EDD_IRT_PORT_INACTIVE,... ) */
/*+     EddLineDelay(u32) (in ticks)                                        +*/
/*+     EddCableDelay(u32) (in ns)                                          +*/
/*+     EddRxDelay(u32) (in ns)                                             +*/
/*+     EddTxDelay(u32) (in ns)                                             +*/
/*+     EddPortOnlyHasSyncTx(u8) (0...OFF, 1...ON)                          +*/
/*+     EddSyncTopoOk(u8) (0...Fault, 1...OK)                               +*/
/*+     EddLinkSpeedModeConfigured(u8) (EDD_LINK_AUTONEG, EDD_LINK_100MBIT_HALF,...) */
/*+     LinkSpeedModeCapability(u32) (EDD_LINK_AUTONEG, EDD_LINK_100MBIT_HALF,...) */
/*+     EddRxIntervalRedBegin(u32) (in ns)                                  +*/
/*+     EddRxIntervalRedEnd(u32) (in ns)                                    +*/
/*+     EddRxIntervalOrangeBegin(u32) (in ns)                               +*/
/*+     EddRxIntervalOrangeEnd(u32) (in ns)                                 +*/
/*+     EddPortState(u8) (EDD_PORT_STATE_DISABLE, EDD_PORT_STATE_BLOCKING,... ) */
/*+     EddMRPPortState(u8) (EDD_MRP_PORTSTATE_OFF, EDD_MRP_PORTSTATE_ACTIVATED) */
/*+     EddRRTPortState(u8) (Reserved)                                      +*/
/*+                                                                         +*/
/*+     (V1.1 or higher)   EddPortIsWireless(u8) (0...OFF, 1...ON)          +*/
/*+     (V1.2 or higher)   EddReserved(u32) (0...default)                   +*/
/*+                                                                         +*/
/*+  RemoteInfo:                                                            +*/
/*+     CableDelay(u32) (in ns)                                             +*/
/*+     RxDelay(u32) (in ns)                                                +*/
/*+     TxDelay(u32) (in ns)                                                +*/
/*+     RTC2_PortStatus(u8) (values see oha_usr.h)                          +*/
/*+     RTC3_PortStatus(u8) (values see oha_usr.h)                          +*/
/*+     LengthOfPeriod(u32) (in ns)                                         +*/
/*+     RedOffset(u32) (in ns)                                              +*/
/*+     OrangeOffset(u32) (in ns)                                           +*/
/*+     GreenOffset(u32) (in ns)                                            +*/
/*+                                                                         +*/
/*+     PTCP_SubdomainUUID ({0} if no sync record written)                  +*/
/*+     IRDataUUID ({0} if no sync record written)                          +*/
/*+     SyncMaster_MAC ({0} if not sync)                                    +*/
/*+                                                                         +*/
/*+     (V1.2 or higher)   RemReserved(u32) (0...default)                   +*/
/*+                                                                         +*/
/*+  TopoInfo:                                                              +*/
/*+     RTClass2_TopoState(u32) (0...Fault, 1...OK)                         +*/
/*+              Neighborhood (Remote_NAME, _PORT, IRDataUUID, _CableDelay)  */
/*+     RTClass3_TopoState(u32) (0...Fault, 1...OK)                         +*/
/*+              Neighborhood (Remote_ReservedInterval, _PTCP_SubdomainUUID) */
/*+     Sync_TopoState(u32)    (0...Fault, 1...OK)                          +*/
/*+              Neighborhood (Remote_PTCP_SubdomainUUID, _SyncMaster_MAC)   */
/*+                                                                         +*/
/*+     (V1.2 or higher):                                                   +*/
/*+                                                                         +*/
/*+     RTClass3_PDEVTopoState(u32) (0...Fault, 1...OK)                     +*/
/*+              PDevNeighborhood (Remote_IRDataUUID, _PTCP_SubdomainUUID)  +*/
/*+     TopoReserved(u32) (0...default)                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadAdditionalInfo(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData;
    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData;
    LSA_UINT16      Response = OHA_OK;

    OHA_HDB_EDD_PTR_TYPE   pHDBEdd;
    OHA_HDB_LLDP_PTR_TYPE  pHDBLldp;
    OHA_HDB_GSY_PTR_TYPE   pHDBGsy;

    OHA_DB_IF_PTR_TYPE pDB;
    LSA_UINT32      block_length;
    LSA_UINT16      i;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadAdditionalInfo(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    OHA_ASSERT_NULL_PTR(pRQB);

    pPrmData = &pRQB->args.PrmData;
    pDB      = OHA_DB_GET_INTERFACE(NICId);

    if (OHA_IS_NULL(pDB))
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"ROHA_RequestReadAdditionalInfo failed. No database present (NIC-ID: 0x%X)",NICId);
        return (Response);
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadAdditionalInfo(Response: 0x%X)", Response);
        return (Response);
    }

    pRecordData = pRQB->args.PrmData.record_data;
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);
    pHDBGsy  = OHA_GSYGetHandleByNIC(NICId);

    if (OHA_IS_NULL(pHDBEdd) || OHA_IS_NULL(pHDBLldp)|| OHA_IS_NULL(pHDBGsy))
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"ROHA_RequestReadAdditionalInfo failed. No EDD-, LLDP- or GSY-Channel present (NIC-ID: 0x%X)",NICId);
        return (Response);
    }

    /* the header */
    pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;  /* offset */
    block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;

    /* AdditionalInfo: BlockHeader, LocalIfInfo, [LocalPortNumber(u8), LocalLinkInfo, LocalInfo, RemoteInfo, TopoInfo]*  */

    /********** LocalIfInfo (Sync): **********/
    /* IRDataUUID, CableDelay, ReservedIntervalBegin, ReservedIntervalEnd, PTCP_SubdomainUUID, SyncMasterMAC */

    /* PTCP_SubdomainUUID ({0} if no sync record written) */
    block_length += sizeof(OHA_UUID_TYPE);
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    OHA_PDU_SET_MEM (pRecordData, pHDBGsy->SyncDataCommit.SubdomainID, sizeof(OHA_UUID_TYPE));/* PTCP_SubdomainUUID   */

    /* IRDataUUID ({0} if no sync record written) */
    block_length += sizeof(OHA_UUID_TYPE);
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    OHA_PDU_SET_MEM (pRecordData, pHDBGsy->SyncDataCommit.IRDataID, sizeof(OHA_UUID_TYPE));   /* IRDataUUID   */

    /* SyncMaster_MAC ({0} if not sync) */
    block_length += sizeof(OHA_MAC_TYPE);
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    OHA_PDU_SET_MEM (pRecordData, pHDBGsy->SyncDataCommit.MasterSourceAddress, sizeof(OHA_MAC_TYPE));/* SyncMaster_MAC   */

    /* PTCPTimeoutFactor(u16) (0 if no sync record written) */
    block_length += 2;
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    OHA_PDU_SET_U16 (pRecordData, pHDBGsy->SyncDataCommit.PTCPTimeoutFactor);/* PTCPTimeoutFactor */

    /* SyncSendFactor(u32) (time base: 31,25 µs; 0 if no sync record written) */
    block_length += 4;
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    OHA_PDU_SET_U32 (pRecordData, pHDBGsy->SyncDataCommit.SyncSendFactor);/* SyncSendFactor */
	
    /* IfReserved(u32) (0 ... default) */
    block_length += 4;
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    OHA_PDU_SET_U32 (pRecordData, 0);

    for (i = 0; i < OHA_CFG_MAX_PORTS && i < pDB->PortCnt; i++)  /* over all ports */
    {
        /* OHA_DB_TOPO_PTR_TYPE pTopoDB = pHDBUser->pTopoDataBase[i]; */

        /********** LocalPortNumber **********/
        /* LocalPortNumber(u8) (1...n) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)(i+1)); /* LocalPortNumber (1...n) */

        /********** LocalInfo (Lldp):       **********/
        {
            OHA_COMMON_MEM_U8_PTR_TYPE DelayPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE PortStatusPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE PtcpStatusPtr = LSA_NULL;
            LSA_UINT16 Len     = 0;
            LSA_UINT8  SubType;

            LSA_UINT32 CableDelay = 0;
            LSA_UINT32 RxDelay    = 0;
            LSA_UINT32 TxDelay    = 0;

            LSA_UINT16 RTC2_PortStatus = 0;
            LSA_UINT16 RTC3_PortStatus = 0;

            LSA_UINT32 LengthOfPeriod = 0;
            LSA_UINT32 RedOffset = 0;
            LSA_UINT32 OrangeOffset = 0;
            LSA_UINT32 GreenOffset = 0;

            /* read from LLDP-DB (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
            SubType = OHA_LLDP_SUB_TYPE_PNO_DELAY;
            (LSA_VOID)oha_db_read_loc_lldp_element(NICId, (LSA_UINT16)(i + 1), OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO, &DelayPtr, &Len);
            if (OHA_IS_NOT_NULL(DelayPtr) && (Len == OHA_LLDP_PNO_DELAY_SIZE))
            {
                CableDelay = OHA_LLDP_GET_U32(DelayPtr+(OHA_LLDP_IDX_CABLEDELAY_LOC*sizeof(LSA_UINT32)));
                TxDelay    = OHA_LLDP_GET_U32(DelayPtr+(OHA_LLDP_IDX_TXDELAY_LOC * sizeof(LSA_UINT32)));
                RxDelay    = OHA_LLDP_GET_U32(DelayPtr);
            }

            SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;
            (LSA_VOID)oha_db_read_loc_lldp_element(NICId, (LSA_UINT16)(i + 1), OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO, &PortStatusPtr, &Len);
            if (OHA_IS_NOT_NULL(PortStatusPtr) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
            {
                RTC2_PortStatus = OHA_GET16_NTOH (PortStatusPtr, 0); /* 1st word */
                RTC2_PortStatus &= OHA_PORT_STATUS_CLASS2_MASK;

                PortStatusPtr += 2; /* 2nd word */
                RTC3_PortStatus = OHA_GET16_NTOH (PortStatusPtr, 0); /* 2nd word */
                RTC3_PortStatus &= OHA_PORT_STATUS_CLASS3_MASK;
            }

            SubType = OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS;
            (LSA_VOID)oha_db_read_loc_lldp_element(NICId, (LSA_UINT16)(i + 1), OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO, &PtcpStatusPtr, &Len);
            if (OHA_IS_NOT_NULL(PtcpStatusPtr) && (Len == OHA_LLDP_PNO_PTCP_STATUS_SIZE))
            {
                PtcpStatusPtr += OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE+OHA_UUID_SIZE;
                LengthOfPeriod = OHA_GET32_NTOH (PtcpStatusPtr, 0);
                LengthOfPeriod &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
                RedOffset = OHA_GET32_NTOH (PtcpStatusPtr+sizeof(LSA_UINT32), 0);
                RedOffset &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
                OrangeOffset = OHA_GET32_NTOH (PtcpStatusPtr+2*sizeof(LSA_UINT32), 0);
                OrangeOffset &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
                GreenOffset = OHA_GET32_NTOH (PtcpStatusPtr+3*sizeof(LSA_UINT32), 0);
                GreenOffset &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
            }

            /* CableDelay(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, CableDelay);/* LineDelay in ns */

            /* RxDelay(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, RxDelay);/* RxDelay in ns */

            /* TxDelay(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, TxDelay);/* TxDelay in ns */

            /* RTC2_PortStatus(u8) */
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)RTC2_PortStatus);/* RTC2_PortStatus in ns */

            /* RTC3_PortStatus(u8) */
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)RTC3_PortStatus);/* RTC3_PortStatus in ns */

            /* LengthOfPeriod(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, LengthOfPeriod);/* LengthOfPeriod in ns */

            /* RedOffset(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, RedOffset);/* RedOffset in ns */

            /* OrangeOffset(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, OrangeOffset);/* OrangeOffset in ns */

            /* GreenOffset(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, GreenOffset);/* GreenOffset in ns */
        }

        /* OnlySyncForwarding(u8) (0...FALSE, 1...TRUE) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        {
            OHA_PDU_SET_U8 (pRecordData, 0);/* PortOnlyHasSyncTx */
        }
	
		/* LocReserved(u32) (0 ... default) */
		block_length += 4;
		if (block_length > pPrmData->record_data_length) goto RecLenFailed;
		OHA_PDU_SET_U32 (pRecordData, 0);

        /********** LocalLinkInfo (see edd_usr.h): **********/
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;

        /* EddMAUType(u8) (EDD_MAUTYPE_UNKNOWN,...) */
        OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)(pHDBEdd->LinkStatus[i].MAUType));  /* MAUType */

        /* EddLinkStatus(u8) (EDD_LINK_UP, EDD_LINK_DOWN,...) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)(pHDBEdd->LinkStatus[i].LinkStatus));/* LinkStatus */

        /* EddSpeed(u8) (EDD_LINK_SPEED_10, EDD_LINK_SPEED_100,...) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].Speed); /* Speed */

        /* EddMode(u8) (EDD_LINK_MODE_HALF, EDD_LINK_MODE_FULL) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].Mode);      /* (Duplex)Mode */

        /* EddPortPresence(u8) (EDD_PORT_PRESENT, EDD_PORT_NOT_PRESENT) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].PortStatus);/* PortStatus */

        /* EddClass2PortStatus(u8) (EDD_RTCLASS2_NOT_SUPPORTED, EDD_RTCLASS2_ACTIVE,... ) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].RTC2_PortStatus);/* Class2PortStatus */

        /* EddIRTPortStatus(u8) (EDD_IRT_NOT_SUPPORTED, EDD_IRT_PORT_INACTIVE,... ) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].RTC3_PortStatus);/* IRTPortStatus */

        /* EddLineDelay(u32) (in ticks) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].LineDelay);/* LineDelay in ticks */

        /* EddCableDelay(u32) (in ns) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].CableDelay);/* LineDelay in ns */

        /* EddRxDelay(u32) (in ns) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].RxDelay);/* RxDelay in ns */

        /* EddTxDelay(u32) (in ns) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].TxDelay);/* TxDelay in ns */

        /* EddPortOnlyHasSyncTx(u8) (0...FALSE, 1...TRUE) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        {
            OHA_PDU_SET_U8 (pRecordData, 0);/* PortOnlyHasSyncTx */
        }

        /* EddSyncTopoOk(u8) (0...FALSE, 1...TRUE) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        {
            LSA_UINT8 Temp = (pHDBEdd->LinkStatus[i].SyncTopoOk) ? (LSA_UINT8)1 : (LSA_UINT8)0;
            OHA_PDU_SET_U8 (pRecordData, Temp);/* EddSyncTopoOk */
        }
        /* EddLinkSpeedModeConfigured(u8) (EDD_LINK_AUTONEG, EDD_LINK_100MBIT_HALF,...) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].SpeedModeConfigured);/* EddLinkSpeedModeConfigured */

        /* LinkSpeedModeCapability(u32) (EDD_LINK_AUTONEG, EDD_LINK_100MBIT_HALF,...) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].SpeedModeCapability);/* LinkSpeedModeCapability */

        /* EddRxIntervalRedBegin(u32) (in ns) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].RxIntervalRedBegin);/* RsvIntervalRed.Rx.BeginNs */

        /* EddRxIntervalRedEnd(u32) (in ns) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].RxIntervalRedEnd);/* RsvIntervalRed.Rx.EndNs */

        /* EddRxIntervalOrangeBegin(u32) (in ns) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].RxIntervalOrangeBegin);/* RsvIntervalOrange.Tx.BeginNs */

        /* EddRxIntervalOrangeEnd(u32) (in ns) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LinkStatus[i].RxIntervalOrangeEnd);/* RsvIntervalOrange.Tx.EndNs */

        /* EddPortState(u8) (EDD_PORT_STATE_DISABLE, EDD_PORT_STATE_BLOCKING,... ) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].PortState);/* PortState */

        /* EddMRPPortState(u8) (EDD_MRP_PORTSTATE_OFF, EDD_MRP_PORTSTATE_ACTIVATED) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, pHDBEdd->LinkStatus[i].MRP_PortState);/* PortState */

        /* EddRRTPortState (Reserved) (u8) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, 0);/* RRTState */

        /* EddIsWireless(u8) (0...FALSE, 1...TRUE) */
        block_length++;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        {
            LSA_UINT8 Temp = (pHDBEdd->LinkStatus[i].IsWireless) ? (LSA_UINT8)1 : (LSA_UINT8)0;
            OHA_PDU_SET_U8 (pRecordData, Temp);/* PortIsWireless */
        }
	
		/* EddReserved(u32) (0 ... default) */
		block_length += 4;
		if (block_length > pPrmData->record_data_length) goto RecLenFailed;
		OHA_PDU_SET_U32 (pRecordData, 0);

        /********** RemoteInfo (Lldp):      **********/
        /* RTClass2_PortStatus, RTClass3_PortStatus, IRDataUUID, CableDelay, ReservedInterval, PTCP_SubdomainUUID, SyncMasterMAC */
        {
            OHA_COMMON_MEM_U8_PTR_TYPE DelayPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE PortStatusPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE PtcpStatusPtr = LSA_NULL;
            LSA_UINT16 Len     = 0;
            LSA_UINT16 SenderId;
            LSA_UINT8  SubType;

            LSA_UINT32 CableDelay = 0;
            LSA_UINT32 RxDelay    = 0;
            LSA_UINT32 TxDelay    = 0;

            LSA_UINT16 RTC2_PortStatus = 0;
            LSA_UINT16 RTC3_PortStatus = 0;

            LSA_UINT32 LengthOfPeriod = 0;
            LSA_UINT32 RedOffset = 0;
            LSA_UINT32 OrangeOffset = 0;
            LSA_UINT32 GreenOffset = 0;

            SenderId = pDB->LldpData[i].LldpSenderId;

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
            {
                /* read from LLDP-DB (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
                SubType = OHA_LLDP_SUB_TYPE_PNO_DELAY;
                (LSA_VOID)oha_db_read_rem_lldp_element(NICId, (LSA_UINT16)(i + 1), SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                       &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO, &DelayPtr, &Len);
                if (OHA_IS_NOT_NULL(DelayPtr) && (Len == OHA_LLDP_PNO_DELAY_SIZE))
                {
                    CableDelay = OHA_LLDP_GET_U32(DelayPtr+(OHA_LLDP_IDX_CABLEDELAY_LOC*sizeof(LSA_UINT32)));
                    TxDelay    = OHA_LLDP_GET_U32(DelayPtr+(OHA_LLDP_IDX_TXDELAY_LOC * sizeof(LSA_UINT32)));
                    RxDelay    = OHA_LLDP_GET_U32(DelayPtr);
                }

                SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;
                (LSA_VOID)oha_db_read_rem_lldp_element(NICId, (LSA_UINT16)(i + 1), SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                       &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO, &PortStatusPtr, &Len);
                if (OHA_IS_NOT_NULL(PortStatusPtr) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
                {
                    RTC2_PortStatus = OHA_GET16_NTOH (PortStatusPtr, 0); /* 1st word */
                    RTC2_PortStatus &= OHA_PORT_STATUS_CLASS2_MASK;

                    PortStatusPtr += 2; /* 2nd word */
                    RTC3_PortStatus = OHA_GET16_NTOH (PortStatusPtr, 0); /* 2nd word */
                    RTC3_PortStatus &= OHA_PORT_STATUS_CLASS3_MASK;
                }

                /* LLDP_PNIO_PTCPSTATUS-data: PTCP_MasterSourceAddress, PTCP_SubdomainUUID, IRDataUUID,        */
                /*     LLDP_LengthOfPeriod, LLDP_RedPeriodBegin, LLDP_OrangePeriodBegin, LLDP_GreenPeriodBegin */
                SubType = OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS;
                (LSA_VOID)oha_db_read_rem_lldp_element(NICId, (LSA_UINT16)(i + 1), SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                       &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO, &PtcpStatusPtr, &Len);
                if (OHA_IS_NOT_NULL(PtcpStatusPtr) && (Len == OHA_LLDP_PNO_PTCP_STATUS_SIZE))
                {
                    OHA_COMMON_MEM_U8_PTR_TYPE TmpPtcpPtr = PtcpStatusPtr;

                    TmpPtcpPtr += OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE+OHA_UUID_SIZE;
                    LengthOfPeriod = OHA_GET32_NTOH (TmpPtcpPtr, 0);
                    LengthOfPeriod &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
                    RedOffset = OHA_GET32_NTOH (TmpPtcpPtr+sizeof(LSA_UINT32), 0);
                    RedOffset &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
                    OrangeOffset = OHA_GET32_NTOH (TmpPtcpPtr+2*sizeof(LSA_UINT32), 0);
                    OrangeOffset &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
                    GreenOffset = OHA_GET32_NTOH (TmpPtcpPtr+3*sizeof(LSA_UINT32), 0);
                    GreenOffset &= ~OHA_SYNC_PERIOD_VALID_MASK; /* clear bit */
                }
            }

            /* CableDelay(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, CableDelay);/* LineDelay in ns */

            /* RxDelay(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, RxDelay);/* RxDelay in ns */

            /* TxDelay(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, TxDelay);/* TxDelay in ns */

            /* RTC2_PortStatus(u8) */
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)RTC2_PortStatus);/* RTC2_PortStatus in ns */

            /* RTC3_PortStatus(u8) */
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, (LSA_UINT8)RTC3_PortStatus);/* RTC3_PortStatus in ns */

            /* LengthOfPeriod(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, LengthOfPeriod);/* LengthOfPeriod in ns */

            /* RedOffset(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, RedOffset);/* RedOffset in ns */

            /* OrangeOffset(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, OrangeOffset);/* OrangeOffset in ns */

            /* GreenOffset(u32) (in ns) */
            block_length += 4;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U32 (pRecordData, GreenOffset);/* GreenOffset in ns */

            /* remote PTCP_SubdomainUUID ({0} if no sync record written) */
            block_length += sizeof(OHA_UUID_TYPE);
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            if (OHA_IS_NOT_NULL(PtcpStatusPtr) && (Len == OHA_LLDP_PNO_PTCP_STATUS_SIZE))
            {
                OHA_PDU_SET_MEM (pRecordData, PtcpStatusPtr+OHA_MAC_ADDR_SIZE, sizeof(OHA_UUID_TYPE));/* PTCP_SubdomainUUID   */
            }
            else
            {
                OHA_PDU_SET_MEM (pRecordData, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_UUID, sizeof(OHA_UUID_TYPE));/* {0} */
            }

            /* remote IRDataUUID ({0} if no sync record written) */
            block_length += sizeof(OHA_UUID_TYPE);
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            if (OHA_IS_NOT_NULL(PtcpStatusPtr) && (Len == OHA_LLDP_PNO_PTCP_STATUS_SIZE))
            {
                OHA_PDU_SET_MEM (pRecordData, PtcpStatusPtr+OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE, sizeof(OHA_UUID_TYPE));/* IRDataUUID   */
            }
            else
            {
                OHA_PDU_SET_MEM (pRecordData, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_UUID, sizeof(OHA_UUID_TYPE));/* {0} */
            }

            /* remote SyncMaster_MAC ({0} if not sync) */
            block_length += sizeof(OHA_MAC_TYPE);
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            if (OHA_IS_NOT_NULL(PtcpStatusPtr) && (Len == OHA_LLDP_PNO_PTCP_STATUS_SIZE))
            {
                OHA_PDU_SET_MEM (pRecordData, PtcpStatusPtr, sizeof(OHA_MAC_TYPE));/* SyncMaster_MAC   */
            }
            else
            {
                OHA_PDU_SET_MEM (pRecordData, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_MAC, sizeof(OHA_MAC_TYPE));/* {0} */
            }

			/* RemReserved(u32) (0 ... default) */
			block_length += 4;
			if (block_length > pPrmData->record_data_length) goto RecLenFailed;
			OHA_PDU_SET_U32 (pRecordData, 0);
        }

        /********** TopoInfo:       **********/
        /* RTClass2_TopoState(u32) */
        /* Neighborhood (Remote_NAME, _PORT, IRDataUUID, _CableDelay) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LastRemotePortState[i].RtClass2_TopoState);

        /* RTClass3_TopoState(u32) */
        /* Neighborhood (Remote_ReservedInterval, _PTCP_SubdomainUUID) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LastRemotePortState[i].RtClass3_TopoState);

        /* Sync_TopoState(u32) */
        /* Neighborhood (Remote_PTCP_SubdomainUUID, _SyncMaster_MAC) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LastRemotePortState[i].SyncId0_TopoState);

        /* RTClass3_PDEVTopoState(u32) */
		/* PDevNeighborhood (Remote_IRDataUUID, _PTCP_SubdomainUUID) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, pHDBEdd->LastRemotePortState[i].RtClass3_PDEVTopoState);
		
		/* TopoReserved(u32) (0 ... default) */
		block_length += 4;
		if (block_length > pPrmData->record_data_length) goto RecLenFailed;
		OHA_PDU_SET_U32 (pRecordData, 0);
    }

    if (block_length > pPrmData->record_data_length)
    {
RecLenFailed:
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_RequestReadAdditionalInfo: Length of Record failed.");
        Response = OHA_ERR_PRM_DATA;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadAdditionalInfo(Response: 0x%X)", Response);
        return (Response);
    }

    /* if no netto data */
    if (block_length == OHA_TOPO_BLOCK_HEADER_PDU_SIZE/*header*/)
    {
        pPrmData->record_data_length = 0;
    }
    else
    {                   /* the read length */
        pPrmData->record_data_length = block_length;
    }

    /* finally write the BlockHeader */
	oha_prm_write_header(pRQB->args.PrmData.record_data,
						 OHA_RECORD_INDEX_PD_IF_ADDITIONAL_INFO,
						 block_length,
						 OHA_TOPO_DATA_BLOCK_VERSION_V12,
						 LSA_FALSE);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadAdditionalInfo(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadMrpPrm                       +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_TOPO               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the MRP data and builds a PDPortMrpDataAdjust PDU   +*/
/*+                                                                         +*/
/*+  PDPortMrpDataAdjust:   BlockHeader, Padding, Padding, MRP_DomainUUID   +*/
/*+                                                                         +*/
/*+       0x8053    PDPortMrpDataAdjust for one subslot                     +*/
/*+                                                                         +*/
/*+  MRP_DomainUUID:                                                         */
/*   uuid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)                              */
/*      0x00000000-0000-0000-0000-000000000000  - reserved                   */
/*      0x00000000-0000-0000-0000-000000000001 to                            */
/*      0xFFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFE  - unique                     */
/*      0xFFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF  - default                    */
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadMrpPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId,
    LSA_UINT16               RecordIndex)
{
    LSA_UINT16      Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData = &pRQB->args.PrmData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadMrpPrm(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    if (NICId == OHA_INVALID_NIC_ID || LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT16 PortId = pRQB->args.PrmData.edd_port_id;
        OHA_COMMON_MEM_U8_PTR_TYPE VarPtr = LSA_NULL;
        LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP;
        LSA_UINT16 Len     = 0;
        LSA_UINT16 RetVal;

        RetVal = oha_db_read_loc_lldp_element(NICId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                              &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                              &VarPtr, &Len);
        if (!(RetVal == LSA_RET_OK && OHA_IS_NOT_NULL(VarPtr) &&
              (Len == OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE)))
        {
            Response = OHA_OK;  /* no data */
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadMrpPrm(Response: 0x%X)", Response);
            pPrmData->record_data_length = 0;
            return (Response);
        }
        else    /* build the PDU for PDPortMrpDataAdjust */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE  pRecordData = pPrmData->record_data;
            LSA_UINT32  block_length;

            block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;  /* offset */

            /* padding bytes after the block header */
            while ( (block_length & 3) != 0 )
            {
                block_length += 1;
                if (block_length > pPrmData->record_data_length) goto RecLenFailed;
                OHA_PDU_SET_U8 (pRecordData, 0x0);          /* padding bytes       */
            }

            block_length += sizeof(OHA_UUID_TYPE);
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_MEMCPY(pRecordData, VarPtr, sizeof(OHA_UUID_TYPE));/* MRP_DomainUUID      */

            if (RecordIndex != OHA_RECORD_INDEX_PD_PORT_MRP_DATA_REAL
#if 0
                && RecordIndex != OHA_RECORD_INDEX_PD_PORT_MRP_DATA_ADJUST
#endif
				)
            {
                Response = OHA_ERR_PRM_INDEX;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_RequestReadMrpPrm: Read MRP data failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadMrpPrm(Response: 0x%X)", Response);
                return (Response);
            }

            if (block_length > pPrmData->record_data_length)
            {
RecLenFailed:
                Response = OHA_ERR_PRM_DATA;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestReadMrpPrm(Response: 0x%X)", Response);
                return (Response);
            }

            /* write the read length */
            pPrmData->record_data_length = block_length;

            /* finally the BlockHeader for PDPortMrpDataAdjust */
			oha_prm_write_header(pRQB->args.PrmData.record_data,
								 RecordIndex,
								 block_length,
								 OHA_TOPO_DATA_BLOCK_VERSION,
								 LSA_FALSE);
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadMrpPrm(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPrmIndication                    +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_PRM_INDICATION         +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmInd                                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide prm Indication Resources               +*/
/*+                                                                         +*/
/*+               Stores the RQB wthin User-Handlemanagement                +*/
/*+               If this is the first RQB we check if some indications     +*/
/*+               pending to be indicated. If so we indicate them, if not   +*/
/*+               we queue the request.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16      PortId;
    LSA_UINT16      NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmIndication(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

    PortId = pRQB->args.PrmInd.edd_port_id;
	NICId  = pHDB->Params.pDetail->Params.User.NICId;

	if (!OHA_EDDCheckNICUsed(NICId))
    {
        /* Error: no such interface. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_INDICATION failed. No valid NicID for this interface (0x%X)",NICId);

		OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmIndication()");
		OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_RESOURCE);
        return;
    }

    /* check the port id (0 ... interface diag) */
    if (PortId > OHA_CFG_MAX_PORTS)  /* pHDB->PortCnt */
    {
        OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_PARAM);
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "ParamError: OHA_RequestPrmIndication()");
        return;
    }

    /* check the ind rqb for buffer pRecordData */
    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->args.PrmInd.diag_ptr, LSA_NULL))
    {
        OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_PARAM);
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "ParamError: OHA_RequestPrmIndication()");
        return;
    }

    /* init the diag data */
    pRQB->args.PrmInd.diag_cnt = 0;
    OHA_MEMSET(pRQB->args.PrmInd.diag_ptr, 0, OHA_TOPO_MAX_IND_DATA_SIZE);

    /* queue the request */
    OHA_RQB_PUT_BLOCK_TOP(pHDB->TopoIndReq[PortId].pBottom,
                          pHDB->TopoIndReq[PortId].pTop,
                          pRQB);

    pHDB->TopoIndReqPending++;

    /* if this is the first Indication Resource, we check if      */
    /* last indications are pending and the topocheck is started  */
    if (pHDB->TopoState == OHA_TOPO_STATE_START)
    {
        LSA_UINT16      i;

        /* look for collected Port-Indications */
        for (i = 0; i <= OHA_CFG_MAX_PORTS; i++)
        {
			OHA_LAST_PRM_IND_DATA_PTR_TYPE pThisInd = pHDB->pPortTopoInd[i];
			/* evaluate the PortField */
			/* is this port to indicate ? */			
			if (OHA_IS_NOT_NULL(pThisInd))
			{
				/* are there valid diag entries ? */
				if ((i != 0 && pThisInd->DiagValid[OHA_DIAG_MAU-1]) || /* port diag */
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_DELAY-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_CHASSIS-1]) || 
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_PORT-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_MRP_UUID-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_NONE-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_MAU-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_CABLEDELAY-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_LINK-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_RT_CLASS3-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_PTCP-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_PREAMBLE-1]) ||
					(i != 0 && pThisInd->DiagValid[OHA_DIAG_PEER_FRAGMENT-1]) ||

					(i == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_GATEWAY-1]) || /* If diag */
					(i == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_NOS-1]) ||
					(i == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_NOD-1]) ||
					(i == 0 && pThisInd->DiagValid[OHA_DIAG_INTERFACE_IP-1]))
				{
					(LSA_VOID)OHA_TopoIndicateEvent(pThisInd, pHDB);
				}
				else	/* an empty event */
				{
					(LSA_VOID)OHA_TopoIndicateEmptyEvent(i, pHDB);
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmIndication()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CompareTopoMAU                          +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCompare             : Pointer to value                                +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: compares the local MauType (real with expected)           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoMAU(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CompareTopoMAU(PortId: 0x%X, pHDBUser: 0x%X)",
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

                LSA_UINT16  LinkState = OHA_GetLinkStatus(pHDBEdd->LinkStatus[PortId-1].LinkStatus);

                /* if the Link is down -> return "no MAU can be checked" */
                if (LinkState != OHA_PORT_LINK_STATE_UP)
                {
                    /* an existing diagnosis object must DISAPPEAR -> return TRUE */
                    /* it'll be finally reduced to a LinkDown diagnosis */
                    *pCompare = LSA_TRUE;

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoMAU(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                /* real MAU existing ? */
                if (!LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL))
                {
                    /* get MAUType */
                    LSA_UINT16 MauType = pHDBEdd->LinkStatus[PortId-1].MAUType;
                    if (MauType == pTopoDB->MAUType)
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

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CompareTopoMAU(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

#if 0
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmCommitHandler                        +*/
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
/*+  Description: Statemachine for PRM-User-Request.                        +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_PrmCommitHandler(
    OHA_HDB_USER_PTR_TYPE           pHDBUser,
    LSA_UINT16                      Response)
{
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
    LSA_UINT16              NICId;
    OHA_UPPER_RQB_PTR_TYPE  pRQBUser;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmCommitHandler(pHDBUser: 0x%X, Response: 0x%X)",
                          pHDBUser, Response);

    pRQBUser = pHDBUser->TopoUserReq.pBottom;

    OHA_ASSERT_NULL_PTR(pRQBUser);
    OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PRM_COMMIT);

    NICId = OHA_TOPOGetNicByInterface(pHDBUser->InterfaceID);

    pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    OHA_ASSERT_NULL_PTR(pHDBEdd);
    OHA_ASSERT_NULL_PTR(pHDBLldp);

    while (1)
    {
        /* a EDD_SRV_GET_PORT_PARAMS is to do for all ports */
        /* counter is SetPendingCnt */
        /* o.k., let's continue the EDD_SRV_GET_PORT_PARAMS's */
        pHDBUser->PendingPortId++;

        /* are all EDD_SRV_GET_PORT_PARAMS's already done ? */
        if (pHDBUser->PendingPortId > pHDBLldp->pDB->PortCnt)
        {
            /* now EddGetPortParams for PrmCommit is done */

            OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->TopoUserReq.pBottom,
                                     pHDBUser->TopoUserReq.pTop,
                                     pRQBUser);

            /* because PendRCtrlCnt was > 0 we dont expect LSA_NULL here */
            OHA_ASSERT_NULL_PTR(pRQBUser);
            OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PRM_COMMIT);

            pHDBUser->PendingPortId--; /* Number of pend. Req's (PRM)*/
            OHA_ASSERT(pHDBUser->PendingPortId == 0); /* only one request */

            /* finally do the PrmCommit */
            OHA_LOCAL_FCT_ATTR OHA_PrmCommit(pRQBUser, pHDBUser);

            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Handle Prm-Commit successful");
        }
        else
        {
            LSA_UINT16 RetVal = OHA_EDDGetPortParams(pHDBEdd, pHDBUser->PendingPortId,
                                                     (LSA_UINT16)(OHA_RQB_GET_HANDLE(pRQBUser)));  /* next port (LLDP_OPC_SET) */
            if (RetVal != OHA_OK)                     /* an error occurred */
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"EDDGetPortParams failed during Prm-Commit!");
                continue;
            }
        }
        break;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmCommitHandler(), state %Xh", pHDBUser->TopoRequestState);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Check_FastStartUp                       +*/
/*+  Input/Output          :    FastStartUp-PDU                             +*/
/*+                                                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks FSU record to write                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_FastStartUp(
    OHA_UPPER_RQB_PTR_TYPE   pRQB)
{
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord;
    LSA_UINT16      BlockType;
    LSA_UINT16      BlockLength;
    LSA_UINT16      BlockVersion;

    LSA_UINT32      RecordLen;

    LSA_UINT16      RetVal  = OHA_OK;
    LSA_UINT32      index   = 0;

    LSA_BOOL        FSHelloBlockExisting     = LSA_FALSE;
    LSA_BOOL        FastStartUpBlockExisting = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Check_FastStartUp(pRQB: 0x%X)", pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pRQB->args.PrmData.record_data);

    pRecord = pRQB->args.PrmData.record_data;
    RecordLen = pRQB->args.PrmData.record_data_length;

    if (pRQB->args.PrmData.record_index != OHA_RECORD_INDEX_FAST_STARTUP)
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: Index of Record (0x%X) failed.", pRQB->args.PrmData.record_index);
        RetVal = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(Response: 0x%X)", RetVal);
        return (RetVal);
    }

    if ((LSA_UINT8)pRQB->args.PrmData.edd_port_id != 0)
    {
        RetVal = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pRQB->args.PrmData.edd_port_id);
    }

    /* PDInterface-FSUDataAdjust { BlockHeader, [Padding*], [FSHelloBlock], [FastStartUpBlock] * } */

    /* look at the the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    index += 4;

    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockType, pRecord);
    OHA_PDU_GET_U16 (BlockLength, pRecord);

    if (BlockType != OHA_BLOCK_TYPE_PD_IF_FSU_DATA_ADJUST)
    {
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockType (%u) not valid.", BlockType);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(Response: 0x%X)", RetVal);
        return (RetVal);
    }

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockHeader (BlockLength) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	if (BlockLength != (pRQB->args.PrmData.record_data_length - 2*sizeof(LSA_UINT16))) /* AP01358661 */
	{
        RetVal = OHA_ERR_PRM_DATA;  /* BlockLength failed */
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen (0x%X) differs from BlockLength (0x%X)!",pRQB->args.PrmData.record_data_length, BlockLength);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(Response: 0x%X)", RetVal);
		return (RetVal);
	}

    /* BlockVersion */
    index   += 2;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);

    if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockHeader(BlockVersion) failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    index += 2;                     /* 2 padding bytes        */
    if (index>RecordLen) goto RecLenFailed;
    if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong Padding !");
        return(RetVal);
    }
    pRecord += 2;

    /* FSHelloBlock:    BlockHeader, [Padding] *, FSHelloMode, FSHelloInterval, FSHelloRetry, FSHelloDelay, [Padding] * */
    /* FastStartUpBlock:BlockHeader, [Padding] *, Data*, [Padding] *                            */
    while (RetVal == OHA_OK && index < pRQB->args.PrmData.record_data_length)
    {
        LSA_UINT32 Temp32;
        /* look at the the BlockHeader: */
        /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (BlockType, pRecord);
        OHA_PDU_GET_U16 (BlockLength, pRecord);

        switch (BlockType)
        {
                /* FSHelloBlock:    BlockHeader, [Padding] *, FSHelloMode, FSHelloInterval, FSHelloRetry, FSHelloDelay, [Padding] * */
            case OHA_BLOCK_TYPE_FAST_STARTUP_HELLO:
            {
                /* BlockVersion */
                index   += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (BlockVersion, pRecord);

                if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockHeader(BlockVersion) failed.");
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                if (FSHelloBlockExisting)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - 2nd FSHelloBlock existing!");
                    break;
                }
                FSHelloBlockExisting     = LSA_TRUE;

                index += 2;                     /* 2 padding bytes        */
                if (index>RecordLen) goto RecLenFailed;
                if (*pRecord != 0 || *(pRecord+1) != 0) /* check the paddings */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong Padding !");
                    break;
                }
                pRecord += 2;

                /* FSHelloMode (u32), FSHelloInterval (u32), FSHelloRetry (u32), FSHelloDelay (u32) */
                index += 4;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);    /* FSHelloMode (u32) */
                if (Temp32 != OHA_DCP_HELLO_OFF && Temp32 != OHA_DCP_HELLO_ON && Temp32 != OHA_DCP_HELLO_ON_DELAY) /* check FSHelloMode */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong FSHelloMode (0x%X) !", Temp32);
                    break;
                }

                index += 4;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);    /* FSHelloInterval (u32) */
                if (Temp32 != 0x00000001E && Temp32 != 0x000000032 && Temp32 != 0x000000064 &&  /* check FSHelloInterval */
                    Temp32 != 0x00000012C && Temp32 != 0x0000001F4 && Temp32 != 0x0000003E8)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong FSHelloInterval (0x%X) !", Temp32);
                    break;
                }

                index += 4;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);    /* FSHelloRetry (u32) */
                if (Temp32 < 0x00000001 || Temp32 > 0x0000000F) /* check FSHelloRetry */
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong FSHelloRetry (0x%X) !", Temp32);
                    break;
                }

                index += 4;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U32 (Temp32, pRecord);    /* FSHelloDelay (u32) */
                if (Temp32 != 0 && Temp32 != 0x000000032 && Temp32 != 0x000000064 && /* check FSHelloDelay */
                    Temp32 != 0x0000001F4 && Temp32 != 0x0000003E8)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong FSHelloDelay (0x%X) !", Temp32);
                    break;
                }
            }
            break;

            default:
                FastStartUpBlockExisting = LSA_TRUE;
                /* FastStartUpBlock:BlockHeader, [Padding] *, Data*, [Padding] *    */
                if (BlockType >= OHA_BLOCK_TYPE_FAST_STARTUP_RESERVED_BEGIN &&
                    BlockType <= OHA_BLOCK_TYPE_FAST_STARTUP_RESERVED_END &&
                    BlockType != OHA_BLOCK_TYPE_PD_IF_FSU_DATA_ADJUST && /* not here */
                    BlockType != OHA_BLOCK_TYPE_PD_AR_FSU_DATA_ADJUST) /* not here */
                {
                    index += BlockLength;
                    if (index>RecordLen) goto RecLenFailed;
                    pRecord += BlockLength;

                    /* The number of padding octets shall be adapted to make the block Unsigned32 aligned. */
                    while ( (index & 3) != 0 )
                    {
                        index += 1;
                        if (index>RecordLen) goto RecLenFailed;
                        if (*pRecord != 0) /* check the paddings */
                        {
                            RetVal = OHA_ERR_PRM_DATA;
                            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Check_FastStartUp() - wrong Padding !");
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(RetVal: 0x%X)", RetVal);
                            return(RetVal);
                        }
                        pRecord += 1;                       /* padding byte        */
                    }
                    break;
                }

                RetVal = OHA_ERR_PRM_BLOCK;
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: BlockType (%u) not valid.", BlockType);
                break;
        }
    }

    /* RecordLength should be the sum of all the existing Blocks */
    if ((RecordLen != index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: Length of Record failed.");
    }

    /* At least one optional block shall be existing! */
    if (!FSHelloBlockExisting && !FastStartUpBlockExisting)
    {
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Check_FastStartUp: Empty record, no optional block existing.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Check_FastStartUp(Response: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddFastStartUp                   +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Add (overwrite) the expected FSU record data              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddFastStartUp(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser)
{
    LSA_UINT16           Response;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddFastStartUp(pRecordData: 0x%X, RecordLen: 0x%X, RecordIndex: 0x%X, pHDBUser: 0x%X)",
                          pRecordData, RecordLen, RecordIndex, pHDBUser);

    if (LSA_HOST_PTR_ARE_EQUAL(pRecordData,  LSA_NULL) || RecordLen == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else if (RecordIndex != OHA_RECORD_INDEX_FAST_STARTUP)
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT32  FSHelloMode     = 0;
        LSA_UINT32  FSHelloInterval = 0;
        LSA_UINT32  FSHelloRetry    = 0;
        LSA_UINT32  FSHelloDelay    = 0;

        /****************************************************/
        /* first the record (FastStartUp)                   */
        /****************************************************/
        Response = OHA_Get_FastStartUpData(pRecordData, RecordLen,
                                           &FSHelloMode, &FSHelloInterval,
                                           &FSHelloRetry, &FSHelloDelay);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE: Write Record FastStartUp failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddFastStartUp(Response: 0x%X)", Response);
            return (Response);
        }
        else /* write to temp FSU data */
        {
            LSA_UINT16           NICId   = pHDBUser->Params.pDetail->Params.User.NICId;
            OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NICId);
            OHA_DB_IF_PTR_TYPE   pDbIf   = OHA_DB_GET_INTERFACE(NICId);

            if ( LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL) || LSA_HOST_PTR_ARE_EQUAL(pDbIf,  LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PRM_DATA failed. No DCP-Channel present (NIC-ID: 0x%X)",NICId);
                Response = OHA_ERR_RESOURCE;
            }
            else
            {
                /* write buffers (A) */
                pHDBDcp->FSU_DataWrite.HelloMode =  FSHelloMode;
                pHDBDcp->FSU_DataWrite.HelloInterval =  FSHelloInterval;
                pHDBDcp->FSU_DataWrite.HelloRetry =  FSHelloRetry;
                pHDBDcp->FSU_DataWrite.HelloDelay =  FSHelloDelay;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddFastStartUp(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadFastStartUpPrm               +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_PRM                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Topo                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the FastStartUp record. If there aer no FSU data    +*/
/*+               at least one optional block shall be existing.            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadFastStartUpPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    LSA_UINT16      Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData = &pRQB->args.PrmData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadFastStartUpPrm(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    if (NICId == OHA_INVALID_NIC_ID || LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT32 block_length;
        OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pRQB->args.PrmData.record_data;
        LSA_UINT32 HelloMode   = 0;
        LSA_UINT32 HelloRetry  = 0;
        LSA_UINT32 HelloInterval = 0;
        LSA_UINT32 HelloDelay  = 0;
        LSA_UINT16 FSUBlockLen = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2;

        LSA_UINT16 RetVal = OHA_DB_READ_DEVICE_INITIATIVE(NICId, &HelloMode, &HelloRetry, &HelloInterval, &HelloDelay);

        if (RetVal != LSA_RET_OK)
        {
            pPrmData->record_data_length = 0;

            Response = OHA_ERR_PRM_DATA;

            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadFastStartUpPrm(Response: 0x%X)", Response);
            return (Response);
        }

        /* FSU data ? */
        if (HelloMode == 0 && HelloRetry == 0 && HelloInterval == 0 && HelloDelay == 0)
        {
            pPrmData->record_data_length = 0;

            Response = OHA_OK;

            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadFastStartUpPrm(Response: 0x%X)", Response);
            return (Response);
        }

        /* PDInterface-FSUDataAdjust { BlockHeader, [Padding*], [FSHelloBlock], [FastStartUpBlock] * } */
        /* BlockHeader */
        if (pPrmData->record_data_length < FSUBlockLen) goto RecLenFailed;
        pRecordData += FSUBlockLen; /* offset */

        /* FSHelloBlock:    BlockHeader, [Padding] *, FSHelloMode, FSHelloInterval, FSHelloRetry, FSHelloDelay, [Padding] * */
        /* BlockHeader */
        block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE;

        /* AP00757251: check length of both header */
        if ((block_length + FSUBlockLen) > pPrmData->record_data_length) goto RecLenFailed;
        pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;  /* offset */

        /* padding byte(s) after BlockHeader */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if ((block_length + FSUBlockLen) > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, 0x0);          /* padding bytes     */
        }

        /* FSHelloMode (u32) */
        block_length += 4;
        if ((block_length + FSUBlockLen) > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, HelloMode);

        /* FSHelloInterval (u32) */
        block_length += 4;
        if ((block_length + FSUBlockLen) > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, HelloInterval);

        /* FSHelloRetry (u32) */
        block_length += 4;
        if ((block_length + FSUBlockLen) > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, HelloRetry);

        /* FSHelloDelay (u32) */
        block_length += 4;
        if ((block_length + FSUBlockLen) > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, HelloDelay);
		LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

        if ((block_length + FSUBlockLen) > pPrmData->record_data_length)
        {
RecLenFailed:
            Response = OHA_ERR_PRM_DATA;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadFastStartUpPrm(Response: 0x%X)", Response);
            return (Response);
        }

        /* the read length */
        pPrmData->record_data_length = block_length + FSUBlockLen;

        /* now write the BlockHeader FSHelloBlock */
		oha_prm_write_header(pRQB->args.PrmData.record_data + FSUBlockLen,
							 OHA_BLOCK_TYPE_FAST_STARTUP_HELLO,
							 block_length,
							 OHA_TOPO_DATA_BLOCK_VERSION,
							 LSA_FALSE);

        /* finally write the block PDInterface-FSUDataAdjust */
		oha_prm_write_header(pRQB->args.PrmData.record_data,
							 OHA_BLOCK_TYPE_PD_IF_FSU_DATA_ADJUST,
							 block_length + FSUBlockLen,
							 OHA_TOPO_DATA_BLOCK_VERSION,
							 LSA_TRUE);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadFastStartUpPrm(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_NameOfStationData                   +*/
/*+  Input/Output          :    DcpNameOfStationRecord                      +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get NoS from a DcpNameOfStation-record                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_NameOfStationData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNameOfStation,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength
)
{
    LSA_UINT32    index  = 0;
    LSA_UINT16    RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_NameOfStationData(OHA_CheckPdPortData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);

	if (OHA_IS_NOT_NULL(pLength))
	{
		*pLength = 0;
	}
    if (!LSA_HOST_PTR_ARE_EQUAL(ppNameOfStation, LSA_NULL))
    {
        *ppNameOfStation = LSA_NULL;        /* name of station */
    }

    if (RecordLen < (1+1+2)) /* sizeof(NameOfStationBlock) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_NameOfStationData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* DcpNameOfStationRecord: */
    /* (i) BlockHeader (RemaWriteHeader) is part of the RQB:                                */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
#if 0
    {
        SA_UINT32  BlockIndex;
        LSA_UINT16 SlotNumber;
        LSA_UINT32 BlockLength;
        LSA_UINT16 Reserved;

        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockIndex, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;   /* API */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockLength, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;   /* CompID */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (Reserved, pRecord);

        /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
        /* shall only contain the length of the specific data octets without counting padding octets. */
        if (BlockLength < (RecordLen - OHA_REMA_RECORD_HEADER_LENGTH))
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_NameOfStationData: BlockHeader (RemaRecordDataLength = %u) failed.", BlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        if (BlockIndex != OHA_RECORD_INDEX_DCP_NAME_OF_STATION || Reserved != 0)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_NameOfStationData: RecordIndex (%u) or Reserved (%u) failed.", BlockIndex, Reserved);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        if (BlockLength == OHA_REMA_RECORD_HEADER_LENGTH)   /* empty record */
        {
            RetVal = OHA_OK;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
    }
#endif

    /* (ii) NameOfStationBlock: NameOfStationType (u8, u8), DCPBlocklength (u16), NameOfStationValue */
    /* NameOfStationType = DevicePropertiesOption (DcpOption), SuboptionNameOfStation (DcpSubOption) */
    {
        LSA_UINT8  DcpOption;
        LSA_UINT8  DcpSubOption;
        LSA_UINT16 DcpBlockLength;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (DcpOption, pRecord);

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (DcpSubOption, pRecord);

        if (DcpOption != DCP_DBID_IDENT || DcpSubOption != DCP_DSID_IDNAME)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_NameOfStationData: DcpOption (%u) or DcpSubOption (%u) failed.", DcpOption, DcpSubOption);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (DcpBlockLength, pRecord);

        /* DcpBlockLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
        /* data of the suboption without DCPBlocklength itself and without counting padding octets.           */
        index += DcpBlockLength;
        if (index>RecordLen) goto RecLenFailed;
        if (DcpBlockLength > DCP_MAX_STATION_NAME_LEN)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_NameOfStationData: DcpBlockLength (%u) too big.", DcpBlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		if (OHA_IS_NOT_NULL(pLength))
		{
			*pLength = DcpBlockLength;
		}
        if (!LSA_HOST_PTR_ARE_EQUAL(ppNameOfStation, LSA_NULL))
        {
            *ppNameOfStation = pRecord;     /* name of station */
        }

        /* check outside now, if it's a RFC-name */
#if 0
        pRecord += DcpBlockLength;
#endif
    }

#if 0
    /* (iii) Paddings */
    /* padding bytes after NameOfStationBlock */
    while ( (index & 3) != 0 )
    {
        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        if (*pRecord != 0) /* check the paddings */
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_NameOfStationData() - wrong Padding Data !");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        pRecord += 1;                       /* padding byte   */
    }
#endif

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_NameOfStationData: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_NameOfStationData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_IpSuiteData                         +*/
/*+  Input/Output          :    DcpIPSuiteRecord                            +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get IP-Suite from a DcpIPSuite-record                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_IpSuiteData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_IpSuiteData(OHA_CheckPdPortData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);

	if (OHA_IS_NOT_NULL(pLength))
	{
	    *pLength = 0;
	}
    if (!LSA_HOST_PTR_ARE_EQUAL(ppIpAddr, LSA_NULL))
    {
        *ppIpAddr = LSA_NULL;       /* IP suite */
    }

    if (RecordLen < (1+1+2)) /* sizeof(IPParameterBlock) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* DcpIPSuiteRecord: */

    /* (i) BlockHeader (RemaWriteHeader) is part of the RQB:                                */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
#if 0
    {
        LSA_UINT32 BlockIndex;
        LSA_UINT16 SlotNumber;
        LSA_UINT32 BlockLength;
        LSA_UINT16 Reserved;

        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockIndex, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;   /* API */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockLength, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;   /* CompID */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (Reserved, pRecord);

        /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
        /* shall only contain the length of the specific data octets without counting padding octets. */
        if (BlockLength < (RecordLen - OHA_REMA_RECORD_HEADER_LENGTH))
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: BlockHeader (RemaRecordDataLength = %u) failed.", BlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        if (BlockIndex != OHA_RECORD_INDEX_DCP_IP_SUITE || Reserved != 0)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: RecordIndex (%u) or Reserved (%u) failed.", BlockIndex, Reserved);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        if (BlockLength == OHA_REMA_RECORD_HEADER_LENGTH)   /* empty record */
        {
            RetVal = OHA_OK;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
    }
#endif

    /* (ii) IPParameterBlock: IPParameterType (u8, u8), DCPBlocklength (u16), IPParameterValue */

    /* IPParameterValue = IPOption (DcpOption), SuboptionIPParameter (DcpSubOption) */
    {
        LSA_UINT8  DcpOption;
        LSA_UINT8  DcpSubOption;
        LSA_UINT16 DcpBlockLength;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (DcpOption, pRecord);

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (DcpSubOption, pRecord);

        if (DcpOption != DCP_DBID_TCP || DcpSubOption != DCP_DSID_TCPIP)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: DcpOption (%u) or DcpSubOption (%u) failed.", DcpOption, DcpSubOption);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (DcpBlockLength, pRecord);

        /* DcpBlockLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
        /* data of the suboption without DCPBlocklength itself and without counting padding octets.           */
        index += DcpBlockLength;
        if (index>RecordLen) goto RecLenFailed;
        if (DcpBlockLength != OHA_IP_SUITE_SIZE)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: DcpBlockLength (%u) not equal IpSuiteSize.", DcpBlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* IPParameterValue: IPAddress, Subnetmask, StandardGateway */
		if (OHA_IS_NOT_NULL(pLength))
		{
	        *pLength = OHA_IP_SUITE_SIZE;
		}
        if (!LSA_HOST_PTR_ARE_EQUAL(ppIpAddr, LSA_NULL))
        {
            *ppIpAddr = pRecord;        /* ip suite */
        }

        /* we check the IP-address before */
        if (oha_is_valid_ip_address(OHA_GET32_NTOH(pRecord, 0),
                                    OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE, 0),
                                    OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE, 0),
                                    LSA_FALSE)) /* 0.0.0.0 is allowed by a local set */
        {
            RetVal = OHA_OK;    /* the record check is ok */
        }
        else
        {
            RetVal = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
            /* Error on getting IpAddress. Abort. */
            OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Invalid IP-Suite (IP: 0x%X, SubNet: 0x%X, GateWay: 0x%X)",
                                 OHA_GET32_NTOH(pRecord,0),
                                 OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE,0),
                                 OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,0));
        }

#if 0
        pRecord += DcpBlockLength;
#endif
    }

    /* (iii) we've no Paddings */

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmStationData                      +*/
/*+  Input/Output          :    MultipleStationRecord                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get NoS and IpSuite from a MultipleStation-record         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmStationData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pOptions,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNameOfStation,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength
)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmStationData: 0x%X", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pOptions);

    if (OHA_IS_NOT_NULL(pLength))
    {
        *pLength = 0;
    }

    *pOptions = 0;

    if (!LSA_HOST_PTR_ARE_EQUAL(ppNameOfStation, LSA_NULL))
    {
        *ppNameOfStation = LSA_NULL;        /* name of station */
    }

    if (RecordLen < OHA_REMA_RECORD_HEADER_LENGTH) /* sizeof(BlockHeader) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* (i) BlockHeader (RemaWriteHeader) is part of the RQB:                                */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
    while (RecordLen > index)
    {
        LSA_UINT32 BlockIndex;
        LSA_UINT16 SlotNumber;
        LSA_UINT32 BlockLength;
        LSA_UINT16 Reserved;

        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockIndex, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;   /* API */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockLength, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;   /* CompID */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (Reserved, pRecord);

        LSA_UNUSED_ARG(SlotNumber); /* no (more) check */

        /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
        /* shall only contain the length of the specific data octets without counting padding octets. */
        if ((index + BlockLength) > RecordLen)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: BlockHeader (RemaRecordDataLength = %u) failed.", BlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        if ((BlockIndex != OHA_RECORD_INDEX_DCP_NAME_OF_STATION &&
             BlockIndex != OHA_RECORD_INDEX_DCP_IP_SUITE) ||
            Reserved != 0)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: RecordIndex (%u) or Reserved (%u) failed.", BlockIndex, Reserved);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        switch (BlockIndex)
        {
            case OHA_RECORD_INDEX_DCP_NAME_OF_STATION:
            {
                LSA_UINT8  DcpOption;
                LSA_UINT8  DcpSubOption;
                LSA_UINT16 DcpBlockLength;
				OHA_UPPER_MEM_U8_PTR_TYPE pEmptyNoS;

                /* DcpNameOfStationRecord: */
                /* (ii) NameOfStationBlock: NameOfStationType (u8, u8), DCPBlocklength (u16), NameOfStationValue */
                /* NameOfStationType = DevicePropertiesOption (DcpOption), SuboptionNameOfStation (DcpSubOption) */

                if (*pOptions & OHA_CTRL_STATION_NAME)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: More than one DcpNameOfStationRecord (BlockIndex: 0x%X) existing!", BlockIndex);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 1;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U8 (DcpOption, pRecord);

                index += 1;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U8 (DcpSubOption, pRecord);

                if (DcpOption != DCP_DBID_IDENT || DcpSubOption != DCP_DSID_IDNAME)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: DcpOption (%u) or DcpSubOption (%u) failed.", DcpOption, DcpSubOption);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;

				pEmptyNoS = pRecord;  /* the length field */
                OHA_PDU_GET_U16 (DcpBlockLength, pRecord);

                /* DcpBlockLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
                /* data of the suboption without DCPBlocklength itself and without counting padding octets.           */
                index += DcpBlockLength;
                if (index>RecordLen) goto RecLenFailed;
                if (DcpBlockLength > DCP_MAX_STATION_NAME_LEN)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: DcpBlockLength (%u) too big.", DcpBlockLength);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                if (OHA_IS_NOT_NULL(pLength))
                {
                    *pLength = DcpBlockLength;
                }

				if(DcpBlockLength > 0) /* AP01052934 */
				{
					if (!LSA_HOST_PTR_ARE_EQUAL(ppNameOfStation, LSA_NULL))
					{
						*ppNameOfStation = pRecord;     /* name of station */
					}

	                *pOptions |= OHA_CTRL_STATION_NAME;
				}
				else
				{
					if (!LSA_HOST_PTR_ARE_EQUAL(ppNameOfStation, LSA_NULL))
					{
						*ppNameOfStation = pEmptyNoS;     /* empty string */
					}
				}

                pRecord += DcpBlockLength;

                /* (iii) Paddings */
                /* padding bytes after NameOfStationBlock */
                while ( (index & 3) != 0 )
                {
                    index += 1;
                    if (index>RecordLen) goto RecLenFailed;
                    if (*pRecord != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmStationData() - wrong Padding Data !");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    pRecord += 1;                       /* padding byte   */
                }
            }
            break;

            case OHA_RECORD_INDEX_DCP_IP_SUITE:
            {
                LSA_UINT8  DcpOption;
                LSA_UINT8  DcpSubOption;
                LSA_UINT16 DcpBlockLength;

                /* (ii) IPParameterBlock: IPParameterType (u8, u8), DCPBlocklength (u16), IPParameterValue */
                /* IPParameterValue = IPOption (DcpOption), SuboptionIPParameter (DcpSubOption) */

                if (*pOptions & OHA_CTRL_IP_SUITE)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: More than one DcpIPSuiteRecord (BlockIndex: 0x%X) existing!", BlockIndex);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 1;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U8 (DcpOption, pRecord);

                index += 1;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U8 (DcpSubOption, pRecord);

                if (DcpOption != DCP_DBID_TCP || DcpSubOption != DCP_DSID_TCPIP)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: DcpOption (%u) or DcpSubOption (%u) failed.", DcpOption, DcpSubOption);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (DcpBlockLength, pRecord);

                /* DcpBlockLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
                /* data of the suboption without DCPBlocklength itself and without counting padding octets.           */
                index += DcpBlockLength;
                if (index>RecordLen) goto RecLenFailed;
                if (DcpBlockLength != OHA_IP_SUITE_SIZE)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_IpSuiteData: DcpBlockLength (%u) not equal IpSuiteSize.", DcpBlockLength);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_IpSuiteData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                /* IPParameterValue: IPAddress, Subnetmask, StandardGateway */
                if (!LSA_HOST_PTR_ARE_EQUAL(ppIpAddr, LSA_NULL))
                {
                    *ppIpAddr = pRecord;        /* ip address */
                }

                /* we check the IP-address before */
                if (!oha_is_valid_ip_address(OHA_GET32_NTOH(pRecord, 0),
                                             OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE, 0),
                                             OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE, 0),
                                             LSA_FALSE)) /* 0.0.0.0 is allowed by a local set */
                {
                    /* Error on getting IpAddress. Abort. */
                    OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Invalid IP-Suite (IP: 0x%X, SubNet: 0x%X, GateWay: 0x%X)",
                                         OHA_GET32_NTOH(pRecord,0),
                                         OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE,0),
                                         OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,0));
                    goto RecLenFailed;
                }

                pRecord += OHA_IP_SUITE_SIZE;

                /* (iii) we've no Paddings */

                *pOptions |= OHA_CTRL_IP_SUITE;
            }
            break;

            default:
                /* only a NameOfStation- or a IPSuite Record ! */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid RecordIndex (0x%X)", BlockIndex);
                RetVal = OHA_ERR_PRM_INDEX;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
                return(RetVal);
        }
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmStationData: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmStationData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoPrmWriteStation                       +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_PRM_WRITE              +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.PrmData                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write parameters to the database               +*/
/*+                                                                         +*/
/*+               Writes station data to database and confirms request.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoPrmWriteStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16  RetVal;
    LSA_UINT32  Options = 0;
    LSA_UINT16  Length  = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr = LSA_NULL;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoPrmWriteStation(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    pPrm = &pRQB->args.PrmData;
    OHA_ASSERT_NULL_PTR(pPrm);

    NICId = pHDB->Params.pDetail->Params.User.NICId;
    OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_NAME_OF_STATION:
        case OHA_RECORD_INDEX_DCP_NAME_OF_STATION:
        {
            OHA_COMMON_MEM_U8_PTR_TYPE pStationName = LSA_NULL;

            /* AP01485967: Local PRM Write IP-Suite failed if Disabled (ReadOnly) */
            LSA_UINT8 AccessMode = 0;
			
			(LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NICId,
                                                 DCP_DBID_IDENT, DCP_DSID_IDNAME,
												 &AccessMode);

			if (pRQB->args.PrmData.record_index == OHA_RECORD_INDEX_DCP_NAME_OF_STATION)	/* old record */
			{
				Response = OHA_Get_NameOfStationData(pPrm->record_data,
													 pPrm->record_data_length,
													 &pStationName,
													 &Length);
			}
			else
			{
				Response = OHA_Get_PrmNameOfStationData(pPrm->record_data,
														pPrm->record_data_length,
														&pStationName,
														&Length);
			}
            if (Response == OHA_OK)
            {
                RetVal  = OHA_DB_WRITE_STATION_NAME(NICId,
                                                    pStationName,
                                                    Length,
                                                    AccessMode,
                                                    pPrm->mk_rema);
                if (RetVal != LSA_RET_OK)
                {
                    Response = RetVal;
                }
            }
            if (Response == OHA_OK)
            {
                Options = OHA_CTRL_STATION_NAME;
            }
        }
        break;
        case OHA_RECORD_INDEX_IP_SUITE:
        case OHA_RECORD_INDEX_DCP_IP_SUITE:
        {
            /* AP01485967: Local PRM Write IP-Suite failed if Disabled (ReadOnly) */
            LSA_UINT8 AccessMode = 0;
			
			(LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NICId,
                                                 DCP_DBID_TCP, DCP_DSID_TCPIP,
 												 &AccessMode);

			if(pRQB->args.PrmData.record_index == OHA_RECORD_INDEX_DCP_IP_SUITE) /* old record */
			{
				Response = OHA_Get_IpSuiteData(pPrm->record_data,          /* get the record data */
											   pPrm->record_data_length,
											   &pIpAddr,
											   &Length);
			}
			else
			{
				Response = OHA_Get_PrmIpSuiteData(pPrm->record_data,       /* get the record data */
												  pPrm->record_data_length,
												  &pIpAddr,
												  &Length);
			}

            if (Response == OHA_OK)
            {
                OHA_ASSERT_NULL_PTR(pIpAddr);

                RetVal = OHA_DB_WRITE_IP_SUITE(NICId,
                                               pIpAddr,
                                               pIpAddr+OHA_IP_ADDR_SIZE,
                                               pIpAddr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,
                                               OHA_IP_ADDR_SIZE,
                                               AccessMode,
                                               pPrm->mk_rema);

                if (RetVal != LSA_RET_OK)
                {
                    Response = RetVal;
					break;
                }

				if (pPrm->mk_rema)	/* AP00909635 */
				{
					RetVal = oha_db_store_rema_ip_address(NICId, pIpAddr);
					if (RetVal != LSA_RET_OK)
					{
		                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"OHA_DoPrmWriteStation: Write REMA IP failed (RetVal=%u).", RetVal);
					}
				}
            }
            if (Response == OHA_OK)
            {
                Options = OHA_CTRL_IP_SUITE;
            }
        }
        break;
        case OHA_RECORD_INDEX_MULTIPLE_STATION:
        {
            LSA_UINT16 Length  = 0;
            OHA_COMMON_MEM_U8_PTR_TYPE pName   = LSA_NULL;

            Response = OHA_Get_PrmStationData(pPrm->record_data,
                                              pPrm->record_data_length,
                                              &Options,
                                              &pIpAddr,
                                              &pName,
                                              &Length);
            if (Response != OHA_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_DoPrmWriteStation: Getting StationData failed (Response=%u).", Response);
                break;
            }

            if (Options & OHA_CTRL_STATION_NAME)
            {
                /* AP01485967: Local PRM Write IP-Suite failed if Disabled (ReadOnly) */
                LSA_UINT8 AccessMode = 0;
				
				(LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NICId,
                                                     DCP_DBID_IDENT, DCP_DSID_IDNAME,
 													 &AccessMode);

                RetVal = OHA_DB_WRITE_STATION_NAME(NICId,
                                                   pName,
                                                   Length,
                                                   AccessMode,
                                                   pPrm->mk_rema);
                if (RetVal != LSA_RET_OK)
                {
                    Response = OHA_ERR_USER_LOCK;
                    Options &= ~OHA_CTRL_STATION_NAME;
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_DoPrmWriteStation (NoS): Write NoS error (RetVal=%u)!", RetVal);
                }
            }
            if (Options & OHA_CTRL_IP_SUITE)
            {
                /* AP01485967: Local PRM Write IP-Suite failed if Disabled (ReadOnly) */
                LSA_UINT8 AccessMode = 0;

				(LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NICId,
                                                     DCP_DBID_TCP, DCP_DSID_TCPIP,
 													 &AccessMode);

                RetVal = OHA_DB_WRITE_IP_SUITE(NICId,
                                               pIpAddr,
                                               pIpAddr+OHA_IP_ADDR_SIZE,
                                               pIpAddr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,
                                               OHA_IP_ADDR_SIZE,
                                               AccessMode,
                                               pPrm->mk_rema);
                if (RetVal != LSA_RET_OK)
                {
                    Response = OHA_ERR_USER_LOCK;
                    Options &= ~OHA_CTRL_IP_SUITE;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_WARN,"OHA_DoPrmWriteStation (IP): Write IP error!, RetVal: 0x%X", RetVal);
                }
            }
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* wrong record ! */
            break;
    }

    /* update the DCP frame filter if filter is active */
    RetVal = OHA_DCPUpdateFilter(NICId, Options, OHA_HANDLE_TYPE_DCP);
    if (RetVal != OHA_OK)
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoPrmWriteStation - DCPUpdateFilter(RetVal: 0x%X)", RetVal);
        OHA_ASSERT(RetVal == OHA_OK);
    }

    /* update IP-Suites, the ARP-filter if IP-address changed */
    if (Options & OHA_CTRL_IP_SUITE)
    {
        OHA_ASSERT_NULL_PTR(pIpAddr);
        RetVal = oha_db_store_expected_ip_suite(NICId, pIpAddr, OHA_IP_SUITE_SIZE);
		if (RetVal != LSA_RET_OK)
		{
			 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_DoPrmWriteStation: Write Expected IP failed (RetVal=%u).", RetVal);
		}

        oha_db_delete_emergency_ip_suite(NICId);

        oha_db_write_ip_conflict_status(NICId, LSA_FALSE);

        RetVal = OHA_EDDSetArpFilter(NICId, OHA_HANDLE_TYPE_DCP);
        if (RetVal != OHA_OK)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoPrmWriteStation - EDDSetArpFilter(RetVal: 0x%X)", RetVal);
        }
        RetVal = OHA_EDDSetIp(NICId, OHA_HANDLE_TYPE_DCP);
        if (RetVal != OHA_OK)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoPrmWriteStation - EDDSetIp(RetVal: 0x%X)", RetVal);
        }
    }

    /* update the LLDP-send data if LLDP-Send is active */
    RetVal = OHA_LLDPUpdateSend(NICId, Options, OHA_HANDLE_TYPE_DCP);
    if (RetVal != OHA_OK)
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoPrmWriteStation - LLDPUpdateSend(RetVal: 0x%X)", RetVal);
    }

    if ((Options & OHA_CTRL_STATION_NAME) || /* AP01565375 */
        (Options & OHA_CTRL_IP_SUITE))
    {
        oha_db_write_factory_reset_status(NICId, LSA_FALSE);
    }

    if (Options)      /* are changes made ? */
    {
        OHA_AddressIndicateEventAllUser(NICId); /* address changed - indication */
        OHA_DcpIndicateEventAllOtherUsers(pRQB, NICId);  /* confirm other users */
    }

#ifndef OHA_CFG_NO_NARE
    /* AP00961349: is the IP-Suite to check (ethernet) */
    RetVal = OHA_NARECheckAddress(NICId, (pHDB->Params.pDetail->Params.User.CheckIpEnabled) ? OHA_CTRL_IP_SUITE : 0, 
								  Options, pHDB->Params.pDetail->Params.User.CheckInterval);
    if (RetVal != OHA_OK)
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoPrmWriteStation - OHA_NARECheckAddress failed (RetVal: 0x%X)", RetVal);
    }
#endif

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoPrmWriteStation(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    OHA_RequestPrmReadStation                   +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_READ                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads NameOfStation record and IPSuite record             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestPrmReadStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData;
    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData;
    LSA_UINT16      Response = OHA_OK;
    LSA_UINT32      RemaRecordDataLength = 0;
    OHA_DB_IF_PTR_TYPE pDB;
    LSA_UINT32      block_length;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmReadStation(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    OHA_ASSERT_NULL_PTR(pRQB);

    pPrmData = &pRQB->args.PrmData;

    pDB = OHA_DB_GET_INTERFACE(NICId);

    if (OHA_IS_NULL(pDB))
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestPrmReadStation failed. No database present (NIC-ID: 0x%X)",NICId);
        return (Response);
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data, LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadStation(Response: 0x%X)", Response);
        return (Response);
    }

    pRecordData = pRQB->args.PrmData.record_data;
    block_length = 0; /* no header */

#if 0
    /* the header */
    block_length = OHA_REMA_RECORD_HEADER_LENGTH;
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    pRecordData += OHA_REMA_RECORD_HEADER_LENGTH;   /* offset */
#endif

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_MULTIPLE_STATION:
            Response = OHA_GetMultipleStationData(NICId,
                                                    pPrmData->record_data,
                                                    &pPrmData->record_data_length,
                                                    LSA_FALSE);
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadStation(Response: 0x%X)", Response);
            return (Response);

        case OHA_RECORD_INDEX_NAME_OF_STATION:
            Response = OHA_GetNameOfStationData(NICId,
                                                pPrmData->record_data,
                                                &pPrmData->record_data_length,
                                                LSA_FALSE);

            pPrmData->mk_rema = oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, DCP_MASK_DS_REMANENT);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadStation(Response: 0x%X)", Response);
            return (Response);

        case OHA_RECORD_INDEX_DCP_NAME_OF_STATION:
        {
            OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
            LSA_INT Len = OHA_DB_READ_STATION_NAME(NICId, &NamePtr);

            if (Len && OHA_IS_NULL(NamePtr))    /* error */
            {
                Response = OHA_ERR_FAILED;
                break;
            }

            /* (ii) NameOfStationBlock: NameOfStationType (u8, u8), DCPBlocklength (u16), NameOfStationValue */
            /* NameOfStationType = DevicePropertiesOption (DcpOption), SuboptionNameOfStation (DcpSubOption) */
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, DCP_DBID_IDENT);
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, DCP_DSID_IDNAME);

            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Len);

            block_length += Len;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_MEM(pRecordData, NamePtr, Len);

            RemaRecordDataLength = 1 + 1 + 2 + Len; /* sizeof(NameOfStationBlock) */

            pPrmData->mk_rema = oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, DCP_MASK_DS_REMANENT);
        }
        break;
        case OHA_RECORD_INDEX_IP_SUITE:
            Response = OHA_GetIPSuiteData(NICId,
                                          pPrmData->record_data,
                                          &pPrmData->record_data_length,
                                          LSA_FALSE);

            pPrmData->mk_rema = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadStation(Response: 0x%X)", Response);
            return (Response);

        case OHA_RECORD_INDEX_DCP_IP_SUITE:
        {
            OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr      = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pSubnetMask  = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pGateway     = LSA_NULL;

            /* read local IP-Address, Subnet-mask and Default Router Address */
            (LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &pIpAddr, &pSubnetMask, &pGateway);
            if (OHA_IS_NULL(pIpAddr) || OHA_IS_NULL(pSubnetMask) || OHA_IS_NULL(pGateway))
            {
                Response = OHA_ERR_FAILED;
                break;
            }

            /* (ii) IPParameterBlock: IPParameterType (u8, u8), DCPBlocklength (u16), IPParameterValue */
            /* IPParameterValue = IPOption (DcpOption), SuboptionIPParameter (DcpSubOption) */
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, DCP_DBID_TCP);
            block_length += 1;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U8 (pRecordData, DCP_DSID_TCPIP);

            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, OHA_IP_SUITE_SIZE);

            block_length += OHA_IP_SUITE_SIZE;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_MEM(pRecordData, pIpAddr, OHA_IP_ADDR_SIZE);
            OHA_PDU_SET_MEM(pRecordData, pSubnetMask, OHA_IP_ADDR_SIZE);
            OHA_PDU_SET_MEM(pRecordData, pGateway, OHA_IP_ADDR_SIZE);

            RemaRecordDataLength = 1 + 1 + 2 + OHA_IP_SUITE_SIZE; /* sizeof(IPParameterBlock) */
            pPrmData->mk_rema = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT);
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* only a station record ! */
            break;
    }

	LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

    if (block_length > pPrmData->record_data_length)
    {
RecLenFailed:
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_RequestPrmReadStation: Length of Record failed.");
        Response = OHA_ERR_PRM_DATA;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadStation(Response: 0x%X)", Response);
        return (Response);
    }

    pPrmData->record_data_length = block_length;    /* the read length */

    LSA_UNUSED_ARG(RemaRecordDataLength);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadStation(Response: 0x%X)", Response);
    return (Response);
}

/*----------------------------------------------------------------------------*/

/* build a multiple Station record:

   RemaWriteMultiple    RemaWriteHeader a, (RemaWriteHeader, RemaRecordData, [Padding*] a)*

      a) The number of padding octets shall be adapted to make the block Unsigned32 aligned.

   RemaWriteHeader  RecordIndex (u32), API, SlotNumber, SubslotNumber, RemaRecordDataLength (u32)
                    RemaCompID(u16), Reserved(u16)

   RemaRecordData   RecordDataWrite ^ Data* ^ NameOfStationBlock b ^ IPParameterBlock b

      b) syntax see pnio draft

    only_rema       LSA_TRUE   build (only) the REMA objects
                    LSA_FALSE  build all (PRM) objects
*/
LSA_UINT16 OHA_GetMultipleStationData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
    LSA_UINT16 RetVal   = OHA_OK;

    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pData;
    LSA_UINT32 block_length = 0;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetMultipleStationData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);

    /* IPSuite record to write ? */
    {
        OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr      = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pSubnetMask  = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pGateway     = LSA_NULL;
        LSA_UINT32 record_data_length = 1 + 1 + 2 + OHA_IP_SUITE_SIZE; /* sizeof(IPParameterBlock) */

		if (only_rema)	/* build the IP object for REMA */
		{
			(LSA_VOID)oha_db_read_rema_ip_address(NICId, &pIpAddr);
			if (OHA_IS_NOT_NULL(pIpAddr))
			{
				pSubnetMask = pIpAddr     + OHA_IP_ADDR_SIZE;
				pGateway    = pSubnetMask + OHA_IP_ADDR_SIZE;
			}
		}
		else	/* build the (current) IP object for PRM */
		{
			/* read local IP-Address, Subnet-mask and Default Router Address */
			(LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &pIpAddr, &pSubnetMask, &pGateway);
			if (OHA_IS_NULL(pIpAddr) || OHA_IS_NULL(pSubnetMask) || OHA_IS_NULL(pGateway))
			{
				RetVal = OHA_ERR_FAILED;
				OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultipleStationData(RetVal: 0x%X)", RetVal);
				return(RetVal);
			}
		}

		if (OHA_IS_NOT_NULL(pIpAddr))
		{
	        /* RemaWriteHeader */
	        block_length += OHA_REMA_RECORD_HEADER_LENGTH;

			if (OHA_IS_NOT_NULL(pData)) /* write the record */
			{
				OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
				LSA_UINT16         subslot_nr;

	            OHA_ASSERT_NULL_PTR(pDB);

	            if (block_length > *pDataLen) goto RecLenFailed;

				/* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
				/* (P := 00 means interface itself) and P counting ports.                      */

	            subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));

				/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
				/*                  RemaRecordDataLength, RemaCompID(u16), Reserved(u16) */
				OHA_PDU_SET_U32 (pRecordData, OHA_RECORD_INDEX_DCP_IP_SUITE);
				OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* SlotNumber    */
				OHA_PDU_SET_U16 (pRecordData, subslot_nr);  /* SubslotNumber (0x8i00) */
				OHA_PDU_SET_U32 (pRecordData, record_data_length);  /* the record data length */
				OHA_PDU_SET_U16 (pRecordData, 0);			/* AP01092248: comp_id removed */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */
			}

			/* RemaRecordData (IPParameterBlock) */
			block_length += record_data_length;
			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* (ii) IPParameterBlock: IPParameterType (u8, u8), DCPBlocklength (u16), IPParameterValue */
				/* IPParameterValue = IPOption (DcpOption), SuboptionIPParameter (DcpSubOption) */
				OHA_PDU_SET_U8 (pRecordData, DCP_DBID_TCP);
				OHA_PDU_SET_U8 (pRecordData, DCP_DSID_TCPIP);
				OHA_PDU_SET_U16 (pRecordData, OHA_IP_SUITE_SIZE);
				OHA_PDU_SET_MEM(pRecordData, pIpAddr, OHA_IP_ADDR_SIZE);
				OHA_PDU_SET_MEM(pRecordData, pSubnetMask, OHA_IP_ADDR_SIZE);
				OHA_PDU_SET_MEM(pRecordData, pGateway, OHA_IP_ADDR_SIZE);
			}

	        OHA_ASSERT(!(block_length & 3));    /* IPParameterBlock needs no paddings */
		}
    }

    /* (remanent) NameOfStation to write ? */
    if ((!only_rema) || oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, DCP_MASK_DS_REMANENT))
    {
        OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
        LSA_INT Len = OHA_DB_READ_STATION_NAME(NICId, &NamePtr);
        LSA_UINT32 record_data_length = Len + 1 + 1 + 2; /* sizeof(NameOfStationBlock) */

        if (Len && OHA_IS_NULL(NamePtr))    /* error */
        {
            RetVal = OHA_ERR_FAILED;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetMultipleStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* RemaWriteHeader */
        block_length += OHA_REMA_RECORD_HEADER_LENGTH;

        if (OHA_IS_NOT_NULL(pData))
        {
            OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
            LSA_UINT16         subslot_nr;

            OHA_ASSERT_NULL_PTR(pDB);
            if (block_length > *pDataLen) goto RecLenFailed;

            /* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
            /* (P := 00 means interface itself) and P counting ports.                      */
            subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));

            /* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
            /*                  RemaRecordDataLength, RemaCompID(u16), Reserved(u16) */
            OHA_PDU_SET_U32 (pRecordData, OHA_RECORD_INDEX_DCP_NAME_OF_STATION);
            OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
            OHA_PDU_SET_U16 (pRecordData, 0);                       /* SlotNumber    */
            OHA_PDU_SET_U16 (pRecordData, subslot_nr);  /* SubslotNumber (0x8i00) */
            OHA_PDU_SET_U32 (pRecordData, record_data_length);  /* the record data length */
            OHA_PDU_SET_U16 (pRecordData, 0);			/* AP01092248: comp_id removed */
            OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */
        }

        /* RemaRecordData (IPParameterBlock) */
        block_length += record_data_length;
        if (OHA_IS_NOT_NULL(pData))
        {
            if (block_length > *pDataLen) goto RecLenFailed;

            /* write the block */
            /* (ii) NameOfStationBlock: NameOfStationType (u8, u8), DCPBlocklength (u16), NameOfStationValue */
            /* NameOfStationType = DevicePropertiesOption (DcpOption), SuboptionNameOfStation (DcpSubOption) */
            OHA_PDU_SET_U8 (pRecordData, DCP_DBID_IDENT);
            OHA_PDU_SET_U8 (pRecordData, DCP_DSID_IDNAME);
            OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Len);
            OHA_PDU_SET_MEM(pRecordData, NamePtr, Len);
        }

        /* padding byte(s) after RemaRecordData */
        while ( (block_length & 3) != 0 )
        {
            block_length += 1;
            if (OHA_IS_NOT_NULL(pData))
            {
                /* we determine the needed buffer length */
                if (!(block_length > *pDataLen))
                {
                    OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte    */
                }
            }
        }
    }

    if (OHA_IS_NOT_NULL(pData))
    {
        if (block_length > *pDataLen)
        {
RecLenFailed:
            RetVal = OHA_ERR_BUFFER_OVERFLOW;

            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
                                  , "OUT: OHA_GetMultipleStationData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
                                  , RetVal, *pDataLen, block_length
                                 );

            /* respond the needed buffer length */
            *pDataLen = block_length;

            return (RetVal);
        }
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultipleStationData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmWriteSNMP                            +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write SNMP data records to OHA                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteSNMP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16              NICId;
	LSA_UINT16 SysNameLen     = 0;
	LSA_UINT16 SysContactLen  = 0;
	LSA_UINT16 SysLocationLen = 0;
	OHA_COMMON_MEM_U8_PTR_TYPE pSysName     = LSA_NULL;
	OHA_COMMON_MEM_U8_PTR_TYPE pSysContact  = LSA_NULL;
	OHA_COMMON_MEM_U8_PTR_TYPE pSysLocation = LSA_NULL;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmWriteSNMP(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pPrm  = &pRQB->args.PrmData;
	NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    /* if the SOCK is to inform about a mib2 parameter (sysName, sysDescr or ifDescr) */
    if (OHA_IS_NOT_NULL(pHDBUser->Mib2UserReq.pBottom))
	{
		Response = OHA_ERR_SEQUENCE;  /* already one request active */
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Already one PRM(SetMib2)-Request active for this NicId (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
	}

    if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
               LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ) )
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
	else if (OHA_IsAnyLocalSetMib2Active(NICId))	/* SetMib2Params from any other user running ? */
	{
		Response = OHA_ERR_SEQUENCE;  /* already one user request active */
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_OPC_PRM_WRITE failed. Request from another user is active (NIC-ID: 0x%X)!",NICId);
	}
    else if (OHA_IS_NULL(pPrm->record_data) || pPrm->record_data_length == 0)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_MULTIPLE_SNMP)
    {
		LSA_BOOL SysNameRema      = pPrm->mk_rema;
		LSA_BOOL SysContactRema   = pPrm->mk_rema;
		LSA_BOOL SysLocationRema  = pPrm->mk_rema;

		Response = OHA_Get_PrmSNMPData(pPrm->record_data,        /* get the record data */
										pPrm->record_data_length,
										&pSysName, &SysNameLen,
										&pSysContact, &SysContactLen,
										&pSysLocation, &SysLocationLen);

        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Getting MultipleSNMPData failed (Response=%u).", Response);
			OHA_UserRequestFinish(pHDBUser,pRQB,Response);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
        }

		if (OHA_IS_NULL(pSysName))	/* read from the DB and rewrite */
		{
			(LSA_VOID)OHA_DB_READ_SYS_NAME(&pSysName, &SysNameLen, &SysNameRema);
		}
		if (OHA_IS_NULL(pSysLocation))	/* read from the DB and rewrite */
		{
			(LSA_VOID)OHA_DB_READ_SYS_LOCATION(&pSysLocation, &SysLocationLen, &SysLocationRema);
		}
		if (OHA_IS_NULL(pSysContact))	/* read from the DB and rewrite */
		{
			(LSA_VOID)OHA_DB_READ_SYS_CONTACT(&pSysContact, &SysContactLen, &SysContactRema);
		}

		/* the mib2 params are to set, queue the request and call SOCK_OPC_SET_MIBII_PARAMS */
		/* now we have an asynchronous call to SOCK */
		Response = OHA_SOCKUpdateMib2Params(NICId, OHA_RQB_GET_HANDLE(pRQB),
											pSysName, SysNameLen, SysNameRema,
											pSysContact, SysContactLen, SysContactRema,
											pSysLocation, SysLocationLen, SysLocationRema);
		if (Response != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Setting MIB2-Params failed (Response=%u).", Response);
		}
		else
		{
			/* queue the user-rqb */
			OHA_RQB_PUT_BLOCK_TOP(pHDBUser->Mib2UserReq.pBottom,
									pHDBUser->Mib2UserReq.pTop,
									pRQB);

			Response = OHA_OK_ACTIVE;
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
		}
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_SYS_NAME)
    {
		LSA_BOOL SysContactRema  = LSA_FALSE;
		LSA_BOOL SysLocationRema = LSA_FALSE;

		/* read from the DB and rewrite */
		(LSA_VOID)OHA_DB_READ_SYS_LOCATION(&pSysLocation, &SysLocationLen, &SysLocationRema);
		(LSA_VOID)OHA_DB_READ_SYS_CONTACT(&pSysContact, &SysContactLen, &SysContactRema);

        Response = OHA_Get_SNMPSysData(pPrm->record_data,     /* check the record */
                                       pPrm->record_data_length,
                                       &pSysName, &SysNameLen);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) Getting SysData failed.", pPrm->record_index);
			OHA_UserRequestFinish(pHDBUser,pRQB,Response);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
        }

		/* the mib2 params are to set, queue the request and call SOCK_OPC_SET_MIBII_PARAMS */
		/* now we have an asynchronous call to SOCK */
		Response = OHA_SOCKUpdateMib2Params(NICId, OHA_RQB_GET_HANDLE(pRQB),
											pSysName, SysNameLen, pPrm->mk_rema,
											pSysContact, SysContactLen, SysContactRema,
											pSysLocation, SysLocationLen, SysLocationRema);
		if (Response != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Setting MIB2-Params failed (Response=%u).", Response);
		}
		else
		{
			/* queue the user-rqb */
			OHA_RQB_PUT_BLOCK_TOP(pHDBUser->Mib2UserReq.pBottom,
									pHDBUser->Mib2UserReq.pTop,
									pRQB);

			Response = OHA_OK_ACTIVE;
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
		}
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_SYS_CONTACT)
    {
		LSA_BOOL SysNameRema     = LSA_FALSE;
		LSA_BOOL SysLocationRema = LSA_FALSE;

		/* read from the DB and rewrite */
		(LSA_VOID)OHA_DB_READ_SYS_NAME(&pSysName, &SysNameLen, &SysNameRema);
		(LSA_VOID)OHA_DB_READ_SYS_LOCATION(&pSysLocation, &SysLocationLen, &SysLocationRema);

        Response = OHA_Get_SNMPSysData(pPrm->record_data,     /* check the record */
                                       pPrm->record_data_length,
                                       &pSysContact, &SysContactLen);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) Getting SysData failed.", pPrm->record_index);
			OHA_UserRequestFinish(pHDBUser,pRQB,Response);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
        }

		/* the mib2 params are to set, queue the request and call SOCK_OPC_SET_MIBII_PARAMS */
		/* now we have an asynchronous call to SOCK */
		Response = OHA_SOCKUpdateMib2Params(NICId, OHA_RQB_GET_HANDLE(pRQB),
											pSysName, SysNameLen, SysNameRema,
											pSysContact, SysContactLen, pPrm->mk_rema,
											pSysLocation, SysLocationLen, SysLocationRema);
		if (Response != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Setting MIB2-Params failed (Response=%u).", Response);
		}
		else
		{
			/* queue the user-rqb */
			OHA_RQB_PUT_BLOCK_TOP(pHDBUser->Mib2UserReq.pBottom,
									pHDBUser->Mib2UserReq.pTop,
									pRQB);

			Response = OHA_OK_ACTIVE;
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
		}
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_SYS_LOCATION)
    {
		LSA_BOOL SysNameRema    = LSA_FALSE;
		LSA_BOOL SysContactRema = LSA_FALSE;

		/* read from the DB and rewrite */
		(LSA_VOID)OHA_DB_READ_SYS_NAME(&pSysName, &SysNameLen, &SysNameRema);
		(LSA_VOID)OHA_DB_READ_SYS_CONTACT(&pSysContact, &SysContactLen, &SysContactRema);

        Response = OHA_Get_SNMPSysData(pPrm->record_data,     /* check the record */
                                       pPrm->record_data_length,
                                       &pSysLocation, &SysLocationLen);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) Getting SysData failed.", pPrm->record_index);
			OHA_UserRequestFinish(pHDBUser,pRQB,Response);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
        }

		/* the mib2 params are to set, queue the request and call SOCK_OPC_SET_MIBII_PARAMS */
		/* now we have an asynchronous call to SOCK */
		Response = OHA_SOCKUpdateMib2Params(NICId, OHA_RQB_GET_HANDLE(pRQB),
											pSysName, SysNameLen, SysNameRema,
											pSysContact, SysContactLen, SysContactRema,
											pSysLocation, SysLocationLen, pPrm->mk_rema);

		if (Response != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Setting MIB2-Params failed (Response=%u).", Response);
		}
		else
		{
			/* queue the user-rqb */
			OHA_RQB_PUT_BLOCK_TOP(pHDBUser->Mib2UserReq.pBottom,
									pHDBUser->Mib2UserReq.pTop,
									pRQB);

			Response = OHA_OK_ACTIVE;
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
			return;
		}
    }
    else
    {
        Response = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "RecordIndex Error: OHA_PrmWriteSNMP(Index: 0x%X)", pPrm->record_index);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmSNMPData                         +*/
/*+  Input/Output          :    MultipleStationRecord                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get SysName, SysLocation and SysContact from a            +*/
/*+               MultipleSNMP-record.                                      +*/
/*+               If an object does not exist, a Null-Pointer is returned.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmSNMPData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysName,
    OHA_COMMON_MEM_U16_PTR_TYPE pSysNameLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysContact,
    OHA_COMMON_MEM_U16_PTR_TYPE pSysContactLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysLocation,
    OHA_COMMON_MEM_U16_PTR_TYPE pSysLocationLen)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmSNMPData(0x%X, Len: 0x%X):", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);

    if (OHA_IS_NOT_NULL(pSysNameLen))
	{
		*pSysNameLen = 0;
	}
    if (OHA_IS_NOT_NULL(pSysContactLen))
    {
        *pSysContactLen = 0;
    }
    if (OHA_IS_NOT_NULL(pSysLocationLen))
    {
        *pSysLocationLen = 0;
    }

    if (OHA_IS_NOT_NULL(ppSysName))
    {
        *ppSysName = LSA_NULL;
    }
    if (OHA_IS_NOT_NULL(ppSysContact))
    {
        *ppSysContact = LSA_NULL;
    }
    if (OHA_IS_NOT_NULL(ppSysLocation))
    {
        *ppSysLocation = LSA_NULL;
    }

    if (RecordLen < OHA_REMA_RECORD_HEADER_LENGTH) /* sizeof(BlockHeader) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* (i) BlockHeader (RemaWriteHeader) is part of the RQB:                                */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
    while (RecordLen > index)
    {
        LSA_UINT32 BlockIndex;
        LSA_UINT16 SlotNumber;
        LSA_UINT32 BlockLength;
        LSA_UINT16 Reserved;

        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockIndex, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;   /* API */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockLength, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;   /* CompID */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (Reserved, pRecord);

        LSA_UNUSED_ARG(SlotNumber); /* no (more) check */

        /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
        /* shall only contain the length of the specific data octets without counting padding octets. */
        if ((index + BlockLength) > RecordLen)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: BlockHeader (RemaRecordDataLength = %u) failed.", BlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        if ((BlockIndex != OHA_RECORD_INDEX_SYS_NAME &&
             BlockIndex != OHA_RECORD_INDEX_SYS_CONTACT &&
			 BlockIndex != OHA_RECORD_INDEX_SYS_LOCATION &&
			 BlockIndex != OHA_RECORD_INDEX_LLDP_INTERFACE_DATA &&
			 BlockIndex != OHA_RECORD_INDEX_LLDP_PORT_DATA) ||
            Reserved != 0)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: RecordIndex (%u) or Reserved (%u) failed.", BlockIndex, Reserved);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        switch (BlockIndex)
        {
            case OHA_RECORD_INDEX_SYS_NAME:
            {
                LSA_UINT16 SNMPDataType;
                LSA_UINT16 SNMPDataLength;

				/********************* SysNameBlock:     ********************/
				/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue a) */
				/* a) the value of the SNMPDataType shall be an ASN_STRING (value=4) with 0 to 255 octets. */
				/************************************************************/

                if(OHA_IS_NOT_NULL(ppSysName) && OHA_IS_NOT_NULL(*ppSysName))
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: More than one SysNameBlock (BlockIndex: 0x%X) existing!", BlockIndex);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SNMPDataType, pRecord);

                if (SNMPDataType != OHA_SNMP_STRING)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: SNMPDataType (%u) is not an octet string.", SNMPDataType);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SNMPDataLength, pRecord);

                /* SNMPDataLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
                /* data of the type without SNMPDataLength itself and without counting padding octets.           */
                index += SNMPDataLength;
                if (index>RecordLen) goto RecLenFailed;
                if (SNMPDataLength > OHA_MAX_CHASSIS_STRLEN)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: SNMPDataLength (%u) too big.", SNMPDataLength);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                if (OHA_IS_NOT_NULL(pSysNameLen))
                {
                    *pSysNameLen = SNMPDataLength;
                }
                if (OHA_IS_NOT_NULL(ppSysName))
                {
                    *ppSysName = pRecord;     /* SysName */
					if(SNMPDataLength == 0 && OHA_IS_NOT_NULL(pSysNameLen))
					{
						*ppSysName = (OHA_UPPER_MEM_U8_PTR_TYPE)pSysNameLen; /* return a Null string */
					}
                }

                pRecord += SNMPDataLength;

                /* (iii) Paddings */
                /* padding bytes after SysNameBlock */
                while ( (index & 3) != 0 )
                {
                    index += 1;
                    if (index>RecordLen) goto RecLenFailed;
                    if (*pRecord != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmSNMPData() - wrong Padding Data !");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    pRecord += 1;                       /* padding byte   */
                }
            }
            break;

            case OHA_RECORD_INDEX_SYS_CONTACT:
            {
                LSA_UINT16 SNMPDataType;
                LSA_UINT16 SNMPDataLength;

				/********************* SysContactBlock:  ********************/
				/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue a) */
				/* a) the value of the SNMPDataType shall be an ASN_STRING (value=4) with 0 to 255 octets. */
				/************************************************************/

                if(OHA_IS_NOT_NULL(ppSysContact) && OHA_IS_NOT_NULL(*ppSysContact))
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: More than one SysContactBlock (BlockIndex: 0x%X) existing!", BlockIndex);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SNMPDataType, pRecord);

                if (SNMPDataType != OHA_SNMP_STRING)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: SNMPDataType (%u) is not an octet string.", SNMPDataType);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SNMPDataLength, pRecord);

                /* SNMPDataLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
                /* data of the type without SNMPDataLength itself and without counting padding octets.           */
                index += SNMPDataLength;
                if (index>RecordLen) goto RecLenFailed;
                if (SNMPDataLength > OHA_MAX_CHASSIS_STRLEN)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: SNMPDataLength (%u) too big.", SNMPDataLength);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                if (OHA_IS_NOT_NULL(pSysContactLen))
                {
                    *pSysContactLen = SNMPDataLength;
                }
                if (OHA_IS_NOT_NULL(ppSysContact))
                {
                    *ppSysContact = pRecord;     /* SysContact */
					if(SNMPDataLength == 0 && OHA_IS_NOT_NULL(pSysContactLen))
					{
						*ppSysContact = (OHA_UPPER_MEM_U8_PTR_TYPE)pSysContactLen; /* return a Null string */
					}
                }

                pRecord += SNMPDataLength;

                /* (iii) Paddings */
                /* padding bytes after SysContactBlock */
                while ( (index & 3) != 0 )
                {
                    index += 1;
                    if (index>RecordLen) goto RecLenFailed;
                    if (*pRecord != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmSNMPData() - wrong Padding Data !");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    pRecord += 1;                       /* padding byte   */
                }
            }
            break;
            case OHA_RECORD_INDEX_SYS_LOCATION:
            {
                LSA_UINT16 SNMPDataType;
                LSA_UINT16 SNMPDataLength;

				/********************* SysLocationBlock: ********************/
				/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue a) */
				/* a) the value of the SNMPDataType shall be an ASN_STRING (value=4) with 0 to 255 octets. */
				/************************************************************/

                if(OHA_IS_NOT_NULL(ppSysLocation) && OHA_IS_NOT_NULL(*ppSysLocation))
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: More than one SysLocationBlock (BlockIndex: 0x%X) existing!", BlockIndex);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SNMPDataType, pRecord);

                if (SNMPDataType != OHA_SNMP_STRING)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: SNMPDataType (%u) is not an octet string.", SNMPDataType);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                index += 2;
                if (index>RecordLen) goto RecLenFailed;
                OHA_PDU_GET_U16 (SNMPDataLength, pRecord);

                /* SNMPDataLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
                /* data of the type without SNMPDataLength itself and without counting padding octets.           */
                index += SNMPDataLength;
                if (index>RecordLen) goto RecLenFailed;
                if (SNMPDataLength > OHA_MAX_CHASSIS_STRLEN)
                {
                    RetVal = OHA_ERR_PRM_DATA;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: SNMPDataLength (%u) too big.", SNMPDataLength);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

                if (OHA_IS_NOT_NULL(pSysLocationLen))
                {
                    *pSysLocationLen = SNMPDataLength;
                }
                if (OHA_IS_NOT_NULL(ppSysLocation))
                {
                    *ppSysLocation = pRecord;     /* SysLocation */
					if(SNMPDataLength == 0 && OHA_IS_NOT_NULL(pSysLocationLen))
					{
						*ppSysLocation = (OHA_UPPER_MEM_U8_PTR_TYPE)pSysLocationLen; /* return a Null string */
					}
                }

                pRecord += SNMPDataLength;

                /* (iii) Paddings */
                /* padding bytes after SysLocationBlock */
                while ( (index & 3) != 0 )
                {
                    index += 1;
                    if (index>RecordLen) goto RecLenFailed;
                    if (*pRecord != 0) /* check the paddings */
                    {
                        RetVal = OHA_ERR_PRM_DATA;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmSNMPData() - wrong Padding Data !");
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                        return(RetVal);
                    }
                    pRecord += 1;                       /* padding byte   */
                }
            }
            break;

            case OHA_RECORD_INDEX_LLDP_INTERFACE_DATA:
            case OHA_RECORD_INDEX_LLDP_PORT_DATA:
				index += BlockLength;
				pRecord += BlockLength;
			break;

            default:
                /* only SysName-, SysContact- or SysLocation Records ! */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid RecordIndex (0x%X)", BlockIndex);
                RetVal = OHA_ERR_PRM_INDEX;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
                return(RetVal);
        }
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPData: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_SNMPSysData                         +*/
/*+  Input/Output          :    SysNameRecord                               +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get SysName, SysContact or SysLocation from a SNMP-record +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_SNMPSysData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysData,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength
)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_SNMPSysData(pRecData: 0x%X, RecordLen: 0x%X)", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);

	if (OHA_IS_NOT_NULL(pLength))
	{
	    *pLength = 0;
	}
    if (!LSA_HOST_PTR_ARE_EQUAL(ppSysData, LSA_NULL))
    {
        *ppSysData = LSA_NULL; /* SysName, SysContact or SysLocation */
    }

    if (RecordLen < (2+2)) /* sizeof(SysXyzBlock) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_SNMPSysData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_SNMPSysData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/**** SysNameBlock, SysContactBlock or SysLocationBlock: ****/
	/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue a) */
	/* a) the value of the SNMPDataType shall be an ASN_STRING (value=4) with 0 to 255 octets. */
	/************************************************************/
    {
        LSA_UINT16 SNMPDataType;
        LSA_UINT16 SNMPDataLength;

		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (SNMPDataType, pRecord);

		if (SNMPDataType != OHA_SNMP_STRING)
		{
			RetVal = OHA_ERR_PRM_BLOCK;
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_SNMPSysData: SNMPDataType (%u) is not an octet string.", SNMPDataType);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_SNMPSysData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (SNMPDataLength, pRecord);

		/* SNMPDataLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
		/* data of the type without SNMPDataLength itself and without counting padding octets.           */
		index += SNMPDataLength;
		if (index>RecordLen) goto RecLenFailed;
		if (SNMPDataLength > OHA_MAX_CHASSIS_STRLEN)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_SNMPSysData: SNMPDataLength (%u) too big.", SNMPDataLength);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_SNMPSysData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (OHA_IS_NOT_NULL(pLength))
		{
			*pLength = SNMPDataLength;
		}
		if (OHA_IS_NOT_NULL(ppSysData))
		{
			*ppSysData = pRecord;     /* SysData */
		}

#if 0
        pRecord += DcpBlockLength;
#endif
    }

#if 0
    /* (iii) Paddings */
    /* padding bytes after NameOfStationBlock */
    while ( (index & 3) != 0 )
    {
        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        if (*pRecord != 0) /* check the paddings */
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_SNMPSysData() - wrong Padding Data !");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_SNMPSysData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        pRecord += 1;                       /* padding byte   */
    }
#endif

    /* RecordLength should be the sum of the existing BlockLength */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_SNMPSysData: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_SNMPSysData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_SNMPSysData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoPrmWriteSNMP                          +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_PRM_WRITE              +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.PrmData                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write SNMP parameters to the database          +*/
/*+                                                                         +*/
/*+               Writes SNMP data to database and confirms request.        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoPrmWriteSNMP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16                Response = OHA_OK;
	LSA_UINT16                NICId;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    
	LSA_BOOL   SysNameIsTheSame = LSA_TRUE;

	/* compare if different (strings, remanence)   */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoPrmWriteSNMP(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    NICId = pHDB->Params.pDetail->Params.User.NICId;
    pPrm = &pRQB->args.PrmData;
    OHA_ASSERT_NULL_PTR(pPrm);

    OHA_ASSERT(NICId != OHA_INVALID_NIC_ID);

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_SYS_NAME:
        {
			LSA_UINT16 Length  = 0;
			OHA_COMMON_MEM_U8_PTR_TYPE pSysData = LSA_NULL;

            Response = OHA_Get_SNMPSysData(pPrm->record_data,     /* get the data */
                                           pPrm->record_data_length,
                                           &pSysData,
                                           &Length);
			if (Response != OHA_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoPrmWriteSNMP(SysName): RecordIndex (%u) Getting SysData failed.", pPrm->record_index);
				break;
			}

			{
				OHA_COMMON_MEM_U8_PTR_TYPE pSysName = LSA_NULL;
				LSA_UINT16 SysNameLen = 0;
				(LSA_UINT16)OHA_DB_READ_SYS_NAME(&pSysName, &SysNameLen, LSA_NULL);

				SysNameIsTheSame = (Length == SysNameLen) ? LSA_TRUE : LSA_FALSE;
				if (SysNameIsTheSame && OHA_IS_NOT_NULL(pSysName))
				{
					SysNameIsTheSame = OHA_MEMCMP(pSysName, pSysData, Length);
				}
			}

			Response = OHA_DB_WRITE_SYS_NAME(pSysData, Length, pPrm->mk_rema);
            if (Response != OHA_OK)
            {
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoPrmWriteSNMP(SysName): RecordIndex (%u) Write to DB SysData failed.", pPrm->record_index);
            }
        }
        break;
        case OHA_RECORD_INDEX_SYS_LOCATION:
        {
		    LSA_UINT16 Length  = 0;
		    OHA_COMMON_MEM_U8_PTR_TYPE pSysData = LSA_NULL;

				Response = OHA_Get_SNMPSysData(pPrm->record_data,     /* get the data */
                                           pPrm->record_data_length,
                                           &pSysData,
                                           &Length);
			if (Response != OHA_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoPrmWriteSNMP(SysLocation): RecordIndex (%u) Getting SysData failed.", pPrm->record_index);
				break;
			}

			Response = OHA_DB_WRITE_SYS_LOCATION(pSysData, Length, pPrm->mk_rema);
            if (Response != OHA_OK)
            {
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoPrmWriteSNMP(SysLocation): RecordIndex (%u) Write to DB SysData failed.", pPrm->record_index);
            }
        }
        break;
        case OHA_RECORD_INDEX_SYS_CONTACT:
        {
		    LSA_UINT16 Length  = 0;
		    OHA_COMMON_MEM_U8_PTR_TYPE pSysData = LSA_NULL;

			Response = OHA_Get_SNMPSysData(pPrm->record_data,     /* get the data */
                                           pPrm->record_data_length,
                                           &pSysData,
                                           &Length);
			if (Response != OHA_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoPrmWriteSNMP(SysContact): RecordIndex (%u) Getting SysData failed.", pPrm->record_index);
				break;
			}

            Response = OHA_DB_WRITE_SYS_CONTACT(pSysData, Length, pPrm->mk_rema);
            if (Response != OHA_OK)
            {
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoPrmWriteSNMP(SysContact): RecordIndex (%u) Write to DB SysData failed.", pPrm->record_index);
            }
        }
        break;
        case OHA_RECORD_INDEX_MULTIPLE_SNMP:
        {
            LSA_UINT16 SysNameLen     = 0;
            LSA_UINT16 SysContactLen  = 0;
            LSA_UINT16 SysLocationLen = 0;
            OHA_COMMON_MEM_U8_PTR_TYPE pSysName     = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pSysContact  = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pSysLocation = LSA_NULL;

            Response = OHA_Get_PrmSNMPData(pPrm->record_data,        /* get the record data */
                                           pPrm->record_data_length,
                                           &pSysName,
                                           &SysNameLen,
                                           &pSysContact,
                                           &SysContactLen,
                                           &pSysLocation,
                                           &SysLocationLen);
            if (Response != OHA_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoPrmWriteSNMP(Multiple): Getting SNMPData failed (Response=%u).", Response);
				break;
            }

			if (OHA_IS_NOT_NULL(pSysName))
			{
				OHA_COMMON_MEM_U8_PTR_TYPE pName = LSA_NULL;
				LSA_UINT16 Length = 0;
				(LSA_UINT16)OHA_DB_READ_SYS_NAME(&pName, &Length, LSA_NULL);

				SysNameIsTheSame = (Length == SysNameLen) ? LSA_TRUE : LSA_FALSE;
				if (SysNameIsTheSame && OHA_IS_NOT_NULL(pName))
				{
					SysNameIsTheSame = OHA_MEMCMP(pSysName, pName, Length);
				}

				Response = OHA_DB_WRITE_SYS_NAME(pSysName, SysNameLen, pPrm->mk_rema);
            }
			if (OHA_IS_NOT_NULL(pSysContact))
			{
				Response = OHA_DB_WRITE_SYS_CONTACT(pSysContact, SysContactLen, pPrm->mk_rema);
            }
			if (OHA_IS_NOT_NULL(pSysLocation))
			{
				Response = OHA_DB_WRITE_SYS_LOCATION(pSysLocation, SysLocationLen, pPrm->mk_rema);
            }
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* wrong record ! */
        break;
    }

	if (!SysNameIsTheSame)	/* only SysName is to send and if different */
	{
		/* update the LLDP-send data if LLDP-Send is active */
		OHA_LLDPUpdateSendGlobal();
	}

#ifndef OHA_CFG_NO_REMA
	{
		OHA_HDB_SOCK_PTR_TYPE pSockHDB = OHA_SOCKGetHandleGlobal();
		if(OHA_IS_NOT_NULL(pSockHDB))
		{
			(LSA_VOID)oha_rema_snmp_update(pSockHDB->Params.pDetail->Params.Sock.NICId); /* Write to REMA (global NIC) */
		}
	}
#endif

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoPrmWriteSNMP(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    OHA_RequestPrmReadSNMP                      +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_READ                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads and SysName- SysLocation- and SysContact record     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestPrmReadSNMP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData;
    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData;

    LSA_UINT16      Response = OHA_OK;
    LSA_UINT32      RemaRecordDataLength = 0;
    LSA_UINT32      block_length;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmReadSNMP(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    OHA_ASSERT_NULL_PTR(pRQB);

    pPrmData = &pRQB->args.PrmData;

    if (OHA_IS_NULL(pPrmData->record_data) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadSNMP(Response: 0x%X)", Response);
        return (Response);
    }
	if (pRQB->args.PrmData.record_data_length < OHA_TOPO_BLOCK_HEADER_PDU_SIZE)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_READ failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }

    pRecordData = pRQB->args.PrmData.record_data;
    block_length = 0; /* no header */

#if 0
    /* the header */
    block_length = OHA_REMA_RECORD_HEADER_LENGTH;
    if (block_length > pPrmData->record_data_length) goto RecLenFailed;
    pRecordData += OHA_REMA_RECORD_HEADER_LENGTH;   /* offset */
#endif

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_MULTIPLE_SNMP:
            Response = OHA_GetMultipleSNMPData(NICId,
                                                 pPrmData->record_data,
                                                 &pPrmData->record_data_length,
                                                 LSA_FALSE);
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadSNMP(Response: 0x%X)", Response);
            return (Response);

        case OHA_RECORD_INDEX_SYS_NAME:
        {
            OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
            LSA_UINT16 Length = 0;
			LSA_BOOL   IsRema = LSA_FALSE;
			
			(LSA_VOID)OHA_DB_READ_SYS_NAME(&NamePtr, &Length, &IsRema);

            if (Length && OHA_IS_NULL(NamePtr))    /* error */
            {
                Response = OHA_ERR_FAILED;
                break;
            }

			/************************ SysNameBlock: *********************/
			/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue a) */
			/* a) the value of the SNMPDataType shall be an ASN_STRING (value=4) with 0 to 255 octets. */
			/************************************************************/
            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, OHA_SNMP_STRING);

            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, Length);

            block_length += Length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_MEM(pRecordData, NamePtr, Length);

            RemaRecordDataLength = 2 + 2 + Length; /* size of (SysNameBlock) */
            pPrmData->mk_rema = IsRema;
        }
        break;
        case OHA_RECORD_INDEX_SYS_CONTACT:
        {
            OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
            LSA_UINT16 Length = 0;
			LSA_BOOL   IsRema = LSA_FALSE;
			
			(LSA_VOID)OHA_DB_READ_SYS_CONTACT(&NamePtr, &Length, &IsRema);

            if (Length && OHA_IS_NULL(NamePtr))    /* error */
            {
                Response = OHA_ERR_FAILED;
                break;
            }

			/************************ SysContactBlock: ******************/
			/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue a) */
			/* a) the value of the SNMPDataType shall be an ASN_STRING (value=4) with 0 to 255 octets. */
			/************************************************************/
            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, OHA_SNMP_STRING);

            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, Length);

            block_length += Length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_MEM(pRecordData, NamePtr, Length);

            RemaRecordDataLength = 2 + 2 + Length; /* size of (SysNameBlock) */
            pPrmData->mk_rema = IsRema;
        }
        break;
        case OHA_RECORD_INDEX_SYS_LOCATION:
        {
            OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
            LSA_UINT16 Length = 0;
			LSA_BOOL   IsRema = LSA_FALSE;
			
			(LSA_VOID)OHA_DB_READ_SYS_LOCATION(&NamePtr, &Length, &IsRema);

            if (Length && OHA_IS_NULL(NamePtr))    /* error */
            {
                Response = OHA_ERR_FAILED;
                break;
            }

			/************************ SysLocationBlock: *****************/
			/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue a) */
			/* a) the value of the SNMPDataType shall be an ASN_STRING (value=4) with 0 to 255 octets. */
			/************************************************************/
            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, OHA_SNMP_STRING);

            block_length += 2;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_U16 (pRecordData, Length);

            block_length += Length;
            if (block_length > pPrmData->record_data_length) goto RecLenFailed;
            OHA_PDU_SET_MEM(pRecordData, NamePtr, Length);

            RemaRecordDataLength = 2 + 2 + Length; /* size of (SysNameBlock) */
            pPrmData->mk_rema = IsRema;
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* only a station record ! */
            break;
    }

#if 0
    /* padding bytes */
    while ( (block_length & 3) != 0 )
    {
        block_length += 1;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U8 (pRecordData, 0x0);          /* padding byte        */
    }
#endif
	LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

    if (block_length > pPrmData->record_data_length)
    {
RecLenFailed:
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_RequestPrmReadSNMP: Length of Record failed.");
        Response = OHA_ERR_PRM_DATA;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadSNMP(Response: 0x%X)", Response);
        return (Response);
    }

    pPrmData->record_data_length = block_length;    /* the read length */

#if 0
    /* (i) BlockHeader (RemaWriteHeader) fianlly:                                           */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
    pRecordData = pRQB->args.PrmData.record_data;
    OHA_PDU_SET_U32 (pRecordData, pRQB->args.PrmData.record_index);
    OHA_PDU_SET_U32 (pRecordData, 0);   /* API */
    OHA_PDU_SET_U16 (pRecordData, pRQB->args.PrmData.slot_number);
    OHA_PDU_SET_U16 (pRecordData, pRQB->args.PrmData.subslot_number);
    /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
    /* shall only contain the length of the specific data octets without counting padding octets. */
    OHA_PDU_SET_U32 (pRecordData, RemaRecordDataLength);
    OHA_PDU_SET_U16 (pRecordData, pRQB->args.PrmData.comp_id);
    OHA_PDU_SET_U16 (pRecordData, 0); /* Reserved */
#else
    LSA_UNUSED_ARG(RemaRecordDataLength);
#endif

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadSNMP(Response: 0x%X)", Response);
    return (Response);
}

/*----------------------------------------------------------------------------*/

/* build a single LLDP interface record:

  */
LSA_UINT16 OHA_Put_PrmLLDPInterfaceData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
	LSA_UINT32	record_index,
    LSA_BOOL only_rema)
{
	LSA_UINT32      Value = 0;

    LSA_UINT16      Response = OHA_OK;
    LSA_UINT32      block_length, record_data_length;
	LSA_BOOL        IsRema  = LSA_FALSE;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Put_PrmLLDPInterfaceData(pRecordData: 0x%X, NICId: 0x%X, record_index: 0x%X, only_rema: 0x%X)",
                          pRecordData, NICId, record_index, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);
	OHA_ASSERT(record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA ||
				record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA);
	record_data_length = *pDataLen;

	if(OHA_IS_NOT_NULL(pRecordData))
	{
		if (record_data_length < OHA_TOPO_BLOCK_HEADER_PDU_SIZE)
		{
			*pDataLen = 0;
			Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_Put_PrmLLDPInterfaceData failed. RecordDataLen too small (RecordDataLen: 0x%X)",record_data_length);
			return (Response);
		}
	}

    block_length = 0; /* no header */

	/* TxInterval, default for OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA */
    Value = LLDP_DEFAULT_MSG_DELAY;

	if(record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_INTERVAL, &Value, LSA_NULL, &IsRema);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_TX_INTERVAL,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}
	 /* TxHoldMultiplier, default for OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA  */
	Value = OHA_LLDP_TTL_FACTOR;

	if(record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_HOLD_MULTIPLIER, &Value, LSA_NULL, &IsRema);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_TX_HOLD_MULTIPLIER,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}
	 /* ReinitDelay, default for OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA  */
	Value = LLDP_DEFAULT_TX_REINIT_DELAY;

	if(record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_REINIT_DELAY, &Value, LSA_NULL, &IsRema);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_REINIT_DELAY,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}
	 /* TxDelay, default for OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA  */
	Value = LLDP_DEFAULT_TX_DELAY;

	if(record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_DELAY, &Value, LSA_NULL, &IsRema);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_TX_DELAY,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}
	 /* NotificationInterval, default for OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA  */
	Value = OHA_LLDP_NOTIFICATION_INTERVAL;

	if(record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_NOTIFICATION_INTERVAL, &Value, LSA_NULL, &IsRema);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_NOTIFICATION_INTERVAL,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

    if (OHA_IS_NOT_NULL(pRecordData))
    {
		if (block_length > record_data_length)
		{
RecLenFailed:
			*pDataLen = 0;
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Put_PrmLLDPInterfaceData: Length of Record failed.");
			Response = OHA_ERR_PRM_DATA;
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Put_PrmLLDPInterfaceData(Response: 0x%X)", Response);
			return (Response);
		}
	}

    *pDataLen = block_length;    /* the written length */

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Put_PrmLLDPInterfaceData(Response: 0x%X)", Response);
    return (Response);
}

/*----------------------------------------------------------------------------*/

/* build a multiple SNMP record:

   RemaWriteMultiple    RemaWriteHeader a), (RemaWriteHeader, RemaRecordData, [Padding*] a)*

      a) The number of padding octets shall be adapted to make the block Unsigned32 aligned.

   RemaWriteHeader  RecordIndex (u32), API, SlotNumber, SubslotNumber, RemaRecordDataLength (u32)
                    RemaCompID(u16), Reserved(u16)

   RemaRecordData   RecordDataWrite ^ Data* ^ NameOfStationBlock b) ^ IPParameterBlock b) ^ 
					SysNameBlock c) ^ SysContactBlock c) ^ SysLocationBlock c) ^
					LldpInterfaceRecord c) ^ LldpPortRecord c)

      b) syntax see PNIO draft
	  c) syntax see PPH

    only_rema       LSA_TRUE   build only the remanent objects
                    LSA_FALSE  build all objects
*/
LSA_UINT16 OHA_GetMultipleSNMPData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
	OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
	LSA_UINT16 subslot_nr;

    LSA_UINT16 RetVal;
	LSA_UINT16 PortId;
    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pData;
    LSA_UINT32 block_length = 0;

    OHA_COMMON_MEM_U8_PTR_TYPE NamePtr;
	LSA_UINT16 Length;
	LSA_BOOL IsRema;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetMultipleSNMPData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

	OHA_ASSERT_NULL_PTR(pDB);
    OHA_ASSERT_NULL_PTR(pDataLen);

    /* (remanent) SysName to write ? */
	NamePtr = LSA_NULL;
	Length = 0;
	IsRema = LSA_FALSE;

	(LSA_VOID)OHA_DB_READ_SYS_NAME(&NamePtr, &Length, &IsRema);
    if ((!only_rema) || (only_rema && IsRema))
    {
        LSA_UINT32 record_data_length = 2 + 2 + Length; /* sizeof(SysNameBlock) */

        if (Length && OHA_IS_NULL(NamePtr))    /* error */
        {
            RetVal = OHA_ERR_FAILED;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultipleSNMPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		/* if (Length) */	/* build an empty string too */
		{
			/* RemaWriteHeader */
			block_length += OHA_REMA_RECORD_HEADER_LENGTH;

			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
				/* (P := 00 means interface itself) and P counting ports.                      */
				subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));

				/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
				/*                  RemaRecordDataLength, RemaCompID(u16), Reserved(u16) */
				OHA_PDU_SET_U32 (pRecordData, OHA_RECORD_INDEX_SYS_NAME);
				OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* SlotNumber    */
				OHA_PDU_SET_U16 (pRecordData, subslot_nr);  /* SubslotNumber (0x8i00) */
				OHA_PDU_SET_U32 (pRecordData, record_data_length);  /* the record data length */
				OHA_PDU_SET_U16 (pRecordData, 0);			/* AP01092248: comp_id removed */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */
			}

			/* RemaRecordData (SysNameBlock) */
			/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue */
			block_length += record_data_length;
			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* write the block */
				OHA_PDU_SET_U16 (pRecordData, OHA_SNMP_STRING);
				OHA_PDU_SET_U16 (pRecordData, Length);
				OHA_PDU_SET_MEM(pRecordData, NamePtr, Length);
			}

			/* padding byte(s) after RemaRecordData */
			while ( (block_length & 3) != 0 )
			{
				block_length += 1;
				if (OHA_IS_NOT_NULL(pData))
				{
					/* we determine the needed buffer length */
					if (!(block_length > *pDataLen))
					{
						OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte    */
					}
				}
			}
		}
    }

    /* (remanent) SysLocation to write ? */
	NamePtr = LSA_NULL;
	Length = 0;
	IsRema = LSA_FALSE;
	(LSA_VOID)OHA_DB_READ_SYS_LOCATION(&NamePtr, &Length, &IsRema);

    if ((!only_rema) || (only_rema && IsRema))
    {
        LSA_UINT32 record_data_length = 2 + 2 + Length; /* sizeof(SysLocationBlock) */

        if (Length && OHA_IS_NULL(NamePtr))    /* error */
        {
            RetVal = OHA_ERR_FAILED;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultipleSNMPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
		/* if (Length) */	/* build an empty string too */
		{
			/* RemaWriteHeader */
			block_length += OHA_REMA_RECORD_HEADER_LENGTH;

			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
				/* (P := 00 means interface itself) and P counting ports.                      */
				subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));

				/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
				/*                  RemaRecordDataLength, Reserved(u16), Reserved(u16)   */
				OHA_PDU_SET_U32 (pRecordData, OHA_RECORD_INDEX_SYS_LOCATION);
				OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* SlotNumber    */
				OHA_PDU_SET_U16 (pRecordData, subslot_nr);  /* SubslotNumber (0x8i00) */
				OHA_PDU_SET_U32 (pRecordData, record_data_length);  /* the record data length */
				OHA_PDU_SET_U16 (pRecordData, 0);				/* AP01092248: comp_id removed */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */
			}

			/* RemaRecordData (SysNameBlock) */
			/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue */
			block_length += record_data_length;
			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* write the block */
				OHA_PDU_SET_U16 (pRecordData, OHA_SNMP_STRING);
				OHA_PDU_SET_U16 (pRecordData, Length);
				OHA_PDU_SET_MEM(pRecordData, NamePtr, Length);
			}

			/* padding byte(s) after RemaRecordData */
			while ( (block_length & 3) != 0 )
			{
				block_length += 1;
				if (OHA_IS_NOT_NULL(pData))
				{
					/* we determine the needed buffer length */
					if (!(block_length > *pDataLen))
					{
						OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte    */
					}
				}
			}
		}
	}

    /* (remanent) SysContact to write ? */
	NamePtr = LSA_NULL;
	Length = 0;
	IsRema = LSA_FALSE;
	(LSA_VOID)OHA_DB_READ_SYS_CONTACT(&NamePtr, &Length, &IsRema);
    if ((!only_rema) || (only_rema && IsRema))
    {
        LSA_UINT32 record_data_length = 2 + 2 + Length; /* sizeof(SysContactBlock) */

        if (Length && OHA_IS_NULL(NamePtr))    /* error */
        {
            RetVal = OHA_ERR_FAILED;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetMultipleSNMPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
		/* if (Length) */	/* build an empty string too */
		{
			/* RemaWriteHeader */
			block_length += OHA_REMA_RECORD_HEADER_LENGTH;

			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
				/* (P := 00 means interface itself) and P counting ports.                      */
				subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));

				/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
				/*                  RemaRecordDataLength, Reserved(u16), Reserved(u16)   */
				OHA_PDU_SET_U32 (pRecordData, OHA_RECORD_INDEX_SYS_CONTACT);
				OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* SlotNumber    */
				OHA_PDU_SET_U16 (pRecordData, subslot_nr);  /* SubslotNumber (0x8i00) */
				OHA_PDU_SET_U32 (pRecordData, record_data_length);  /* the record data length */
				OHA_PDU_SET_U16 (pRecordData, 0);			/* AP01092248: comp_id removed */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */
			}

			/* RemaRecordData (SysNameBlock) */
			/* SNMPDataType(u16), SNMPDataLength(u16), SNMPDataValue */
			block_length += record_data_length;
			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* write the block */
				OHA_PDU_SET_U16 (pRecordData, OHA_SNMP_STRING);
				OHA_PDU_SET_U16 (pRecordData, Length);
				OHA_PDU_SET_MEM(pRecordData, NamePtr, Length);
			}

			/* padding byte(s) after RemaRecordData */
			while ( (block_length & 3) != 0 )
			{
				block_length += 1;
				if (OHA_IS_NOT_NULL(pData))
				{
					/* we determine the needed buffer length */
					if (!(block_length > *pDataLen))
					{
						OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte    */
					}
				}
			}
		}
	}

	{
		/* (remanent) LldpInterfaceRecord to write ? */
		LSA_UINT32 record_data_length = 0;
		RetVal = OHA_Put_PrmLLDPInterfaceData(NICId, LSA_NULL, &record_data_length, 
										  OHA_RECORD_INDEX_LLDP_INTERFACE_DATA, only_rema);
		if (RetVal != OHA_OK) goto RecLenFailed;

		/* (rema) data to write */
		if (record_data_length)
		{
			/* RemaWriteHeader */
			block_length += OHA_REMA_RECORD_HEADER_LENGTH;

			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
				/* (P := 00 means interface itself) and P counting ports.                      */
				subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));

				/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
				/*                  RemaRecordDataLength, RemaCompID(u16), Reserved(u16) */
				OHA_PDU_SET_U32 (pRecordData, OHA_RECORD_INDEX_LLDP_INTERFACE_DATA);
				OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* SlotNumber    */
				OHA_PDU_SET_U16 (pRecordData, subslot_nr);     /* SubslotNumber (0x8i00) */
				OHA_PDU_SET_U32 (pRecordData, record_data_length);  /* the record data length */
				OHA_PDU_SET_U16 (pRecordData, 0);		  /* AP01092248: comp_id removed */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */
			}

			block_length += record_data_length;
			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* finally write (rema) data */
				RetVal = OHA_Put_PrmLLDPInterfaceData(NICId, pRecordData, &record_data_length, 
													  OHA_RECORD_INDEX_LLDP_INTERFACE_DATA, only_rema);
				if (RetVal != OHA_OK) goto RecLenFailed;
				pRecordData += record_data_length;
			}
		}
	}

    /* (remanent) LldpPortRecords to write ? */
	for(PortId = 1; PortId <= pDB->PortCnt; PortId++)
	{
		/* (remanent) LldpPortRecord to write ? */
		LSA_UINT32 record_data_length = 0;
		RetVal = OHA_Put_PrmLLDPPortData(NICId, PortId, LSA_NULL, &record_data_length, 
										 OHA_RECORD_INDEX_LLDP_PORT_DATA, only_rema);
		if (RetVal != OHA_OK) goto RecLenFailed;

		/* (rema) data to write */
		if (record_data_length)
		{
			/* RemaWriteHeader */
			block_length += OHA_REMA_RECORD_HEADER_LENGTH;

			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
				/* (P := 00 means interface itself) and P counting ports.                      */
				subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));
				subslot_nr += PortId;
				/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
				/*                  RemaRecordDataLength, RemaCompID(u16), Reserved(u16) */
				OHA_PDU_SET_U32 (pRecordData, OHA_RECORD_INDEX_LLDP_PORT_DATA);
				OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* SlotNumber    */
				OHA_PDU_SET_U16 (pRecordData, subslot_nr);     /* SubslotNumber (0x8i00) */
				OHA_PDU_SET_U32 (pRecordData, record_data_length);  /* the record data length */
				OHA_PDU_SET_U16 (pRecordData, 0);		  /* AP01092248: comp_id removed */
				OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */
			}

			block_length += record_data_length;
			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen) goto RecLenFailed;

				/* finally write (rema) data */
				RetVal = OHA_Put_PrmLLDPPortData(NICId, PortId, pRecordData, &record_data_length, 
												  OHA_RECORD_INDEX_LLDP_PORT_DATA, only_rema);
				if (RetVal != OHA_OK) goto RecLenFailed;
				pRecordData += record_data_length;
			}
		}
	}

    if (OHA_IS_NOT_NULL(pData))
    {
        if (block_length > *pDataLen)
        {
RecLenFailed:
            RetVal = OHA_ERR_BUFFER_OVERFLOW;

            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
                                  , "OUT: OHA_GetMultipleSNMPData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
                                  , RetVal, *pDataLen, block_length);

            /* respond the needed buffer length */
            *pDataLen = block_length;

            return (RetVal);
        }
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultipleSNMPData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmPDEVData                         +*/
/*+  Input                 :    MultiplePDEVRecord                          +*/
/*+  Output                :    SinglePDEVRecord                            +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: gets a single PDEVRecord from a MultiplePDEV-record       +*/
/*+               depending on the given RecordIndex                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Note:  Some single PDEV records can be further read with the           +*/
/*+         following OHA helper functions depending on the RecordIndex.    +*/
/*+                                                                         +*/
/*+  OHA_RECORD_INDEX_PD_PORT_DATA_CHECK     OHA_Get_PdPortDataExpected()   +*/	
/*+  OHA_RECORD_INDEX_PD_PORT_DATA_REAL      OHA_Get_PdPortDataReal()	    +*/	
/*+  OHA_RECORD_INDEX_PD_PORT_MRP_DATA_REAL  OHA_Get_PdPortMrpDataExpected()+*/
/*+  OHA_RECORD_INDEX_PD_INTERFACE_DATA_REAL OHA_Get_PdInterfaceDataReal()  +*/
/*+  OHA_RECORD_INDEX_FAST_STARTUP           OHA_Get_FastStartUpData()      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmPDEVData(
    OHA_UPPER_MEM_U8_PTR_TYPE					pRecordData,	/* IN */
    LSA_UINT32									RecordLen,		/* IN */
	LSA_UINT16									RecordIndex,	/* IN */
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPDevData,	/* OUT */
    OHA_COMMON_MEM_U32_PTR_TYPE					pPDevLen)		/* OUT */
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecordData;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmPDEVData(OHA_Get_PrmPDEVData: 0x%X, Len: 0x%X, RecordIndex: 0x%X)", 
							pRecordData, RecordLen, RecordIndex);

    OHA_ASSERT_NULL_PTR(pRecordData);

    if (OHA_IS_NOT_NULL(pPDevLen))
	{
		*pPDevLen = 0;
	}
    if (OHA_IS_NOT_NULL(ppPDevData))
    {
        *ppPDevData = LSA_NULL;
    }

    if (RecordLen < OHA_REMA_RECORD_HEADER_LENGTH) /* sizeof(BlockHeader) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPDEVData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPDEVData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* (i) BlockHeader (RemaWriteHeader) is part of the RQB:                                */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
    while (RecordLen > index)
    {
        LSA_UINT32 BlockIndex;
        LSA_UINT16 SlotNumber;
        LSA_UINT32 BlockLength;
        LSA_UINT16 Reserved;

        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockIndex, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;   /* API */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockLength, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;   /* CompID */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (Reserved, pRecord);

        LSA_UNUSED_ARG(SlotNumber); /* no (more) check */

        /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
        /* shall only contain the length of the specific data octets without counting padding octets. */
        if ((index + BlockLength) > RecordLen)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPDEVData: BlockHeader (RemaRecordDataLength = %u) failed.", BlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPDEVData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        if (Reserved != 0)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPDEVData: Reserved (%u) failed.", Reserved);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPDEVData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		index += BlockLength;
		if (index>RecordLen) goto RecLenFailed;

        if (BlockIndex == RecordIndex)
        {
			/* subrecord already found */
			if(OHA_IS_NOT_NULL(ppPDevData) && OHA_IS_NOT_NULL(*ppPDevData))
			{
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPDEVData: More than one RecordIndex (BlockIndex: 0x%X) existing!", BlockIndex);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPDEVData(RetVal: 0x%X)", RetVal);
				return(RetVal);
			}

			if (OHA_IS_NOT_NULL(ppPDevData))
			{
				*ppPDevData = pRecord;
			}
		}

		pRecord+= BlockLength;

		/* (iii) Paddings */
		/* padding bytes after SysNameBlock */
		while ( (index & 3) != 0 )
		{
			index += 1;
			if (index>RecordLen) goto RecLenFailed;
			if (*pRecord != 0) /* check the paddings */
			{
				RetVal = OHA_ERR_PRM_DATA;
				OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmPDEVData() - wrong Padding Data !");
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPDEVData(RetVal: 0x%X)", RetVal);
				return(RetVal);
			}
			pRecord += 1;                       /* padding byte   */
			BlockLength += 1;
		}

		/* DataLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
		/* data of the type without DataLength itself and without counting padding octets.           */
        if (BlockIndex == RecordIndex)
        {
			if (OHA_IS_NOT_NULL(pPDevLen))
			{
				*pPDevLen = BlockLength;
			}
		}
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPDEVData: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPDEVData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPDEVData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmWriteLLDP                            +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write LLDP interface data records to OHA       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteLLDP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
	OHA_HDB_USER_PTR_TYPE pHDBSnmpUser = OHA_USERIsAgentActive(OHA_HANDLE_TYPE_LLDP, LSA_NULL);
    LSA_UINT16              Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16              NICId;

    LSA_UINT16  CountOfQueries = 0; /* User queries (LLDP-Set)  */
    LSA_UINT16  CountOfStats   = 0;	/* LLDP statistics, MRP status */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmWriteLLDP(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pPrm  = &pRQB->args.PrmData;
	NICId = pHDBUser->Params.pDetail->Params.User.NICId;

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
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE of LLDP data (0x%X) failed. No queueing of user rqbs", OHA_RQB_GET_OPCODE(pRQB));
    }
	else if(OHA_IS_NOT_NULL(pHDBSnmpUser))	/* any other user active ? */
	{
        Response = OHA_ERR_SEQUENCE;       /* Error: no queueing of any other user snmp rqbs */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE of LLDP data (0x%X) failed. No queueing of any other user rqbs", OHA_RQB_GET_OPCODE(pRQB));
	}
    else if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
             LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ) )
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE of LLDP data failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if (OHA_IS_NULL(pPrm->record_data) && pPrm->record_data_length != 0)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }
#if 0
	else if (OHA_IsAnyLocalSetLldpIfDataActive(NICId))	/* Set LLDP data from any other user running ? */
	{
		Response = OHA_ERR_SEQUENCE;  /* already one user request active */
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_OPC_PRM_WRITE failed. Request from another user is active (NIC-ID: 0x%X)!",NICId);
	}
#endif
			/* a remote snmp_set, snmp_get or snmp_getnext is active */
	else if(OHA_IS_NOT_NULL(OHA_SOCKIsAgentActive(OHA_HANDLE_TYPE_LLDP)))
	{
		/* if a record is to write, we must queue a user PRM_WRITE LLDP record !   */
		/* happens, if a SOCK response is active during this PRM_WRITE LLDP record */
		/* for further actions, see OHA_SNMPSockCheckPending()                     */

        /* only queue the user-rqb */
		OHA_ASSERT(pHDBUser->UserReqPendCnt == 0);

        OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SnmpUserReq.pBottom,
                              pHDBUser->SnmpUserReq.pTop,
                              pRQB);

        Response = OHA_OK_ACTIVE;     /* No Error: a snmp telegram is active */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE /*LSA_TRACE_LEVEL_UNEXP*/,"Request OHA_PrmWriteLLDP (0x%X) queued. An LLDP agent is active", OHA_RQB_GET_OPCODE(pRQB));
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteLLDP: 0x%X)", Response);
		return;
	}

	if (Response != OHA_OK)
	{
		OHA_UserRequestFinish(pHDBUser,pRQB,Response);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteLLDP: 0x%X)", Response);
		return;
	}

	oha_snmp_delete_lldp_statistics();	/* init. */

    if (pPrm->record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA)
    {
	    LSA_UINT16              Options = 0;	 /* see OHA_OPC_LLDP_ACCESS_CONTROL */
		OHA_LLDP_IF_PARAM_TYPE  Config = {0,0,0,0,0};

		Response = OHA_Get_PrmLLDPInterfaceData(pPrm->record_data,        /* get the LLDP interface data */
												pPrm->record_data_length,
												&Options, &Config);
        if (Response == OHA_OK)
        {
			oha_snmp_set_lldp_config_options((LSA_UINT32)Options);
			(LSA_VOID)oha_db_write_temp_lldp_mib_values((LSA_UINT32)Options, &Config);	/* write to temp. MIB Database */
		}
	}
    else if (pPrm->record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
    {
		LSA_UINT32                PortOptions = 0;	 /* see OHA_OPC_LLDP_ACCESS_CONTROL */
		OHA_LLDP_PORT_PARAM_TYPE  PortConfig = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		if (pPrm->edd_port_id == 0 || pPrm->edd_port_id > OHA_CFG_MAX_PORTS)  /* pHDB->PortCnt */
		{
			Response = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
			/* Error on getting handle. Abort. */
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Portid is wrong (PORT-ID: 0x%X)",pPrm->edd_port_id);
		}
		else
		{
			Response = OHA_Get_PrmLLDPPortData(pPrm->record_data,        /* get the LLDP port data */
												pPrm->record_data_length,
												&PortOptions, &PortConfig);
		}
        if (Response == OHA_OK)
        {
			(LSA_VOID)oha_snmp_set_lldp_port_config_options(NICId, pPrm->edd_port_id, PortOptions);
			(LSA_VOID)oha_db_write_temp_lldp_mib_port_values(NICId, pPrm->edd_port_id, PortOptions, &PortConfig);	/* write to temp. MIB Database */
        }
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_MULTIPLE_SNMP)
    {
		if (OHA_IsAnyLocalSetMib2Active(NICId))	/* SetMib2Params from any other user running ? */
		{
			Response = OHA_ERR_SEQUENCE;  /* already one user request active */
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_OPC_PRM_WRITE failed. Request from another user is active (NIC-ID: 0x%X)!",NICId);
		}
		else
		{
			OHA_COMMON_MEM_U8_PTR_TYPE pSubRecData = LSA_NULL;
			LSA_UINT32 SubRecLen = 0;
			LSA_UINT16 PortID;

			/* get all LLDP data */
			if (OHA_OK == OHA_Get_PrmSNMPRecord(pPrm->record_data, /* get the record data */
												pPrm->record_data_length,
												OHA_RECORD_INDEX_LLDP_INTERFACE_DATA, 0, /* PortID */
												&pSubRecData, &SubRecLen))
			{
				if(OHA_IS_NOT_NULL(pSubRecData))
				{
					LSA_UINT16             Options = 0;	 /* see OHA_OPC_LLDP_ACCESS_CONTROL */
					OHA_LLDP_IF_PARAM_TYPE Config = {0,0,0,0,0};
					(LSA_VOID)OHA_Get_PrmLLDPInterfaceData(pSubRecData, SubRecLen, &Options, &Config);
					oha_snmp_set_lldp_config_options((LSA_UINT32)Options);
					(LSA_VOID)oha_db_write_temp_lldp_mib_values((LSA_UINT32)Options, &Config);	/* write to temp. MIB Database */
				}
			}
			for(PortID = 1; PortID <= OHA_CFG_MAX_PORTS; PortID++)
			{
				if (OHA_OK == OHA_Get_PrmSNMPRecord(pPrm->record_data, /* get the record data */
													pPrm->record_data_length,
													OHA_RECORD_INDEX_LLDP_PORT_DATA, PortID, /* PortID */
													&pSubRecData, &SubRecLen))
				{
					if(OHA_IS_NOT_NULL(pSubRecData))
					{
						LSA_UINT32               PortOptions = 0;
						OHA_LLDP_PORT_PARAM_TYPE PortConfig = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
						(LSA_VOID)OHA_Get_PrmLLDPPortData(pSubRecData, SubRecLen, &PortOptions, &PortConfig);
						(LSA_VOID)oha_snmp_set_lldp_port_config_options(NICId, PortID, PortOptions);
						(LSA_VOID)oha_db_write_temp_lldp_mib_port_values(NICId, PortID, PortOptions, &PortConfig);	/* write to temp. MIB Database */
					}
				}
			}
		}
	}
    else
    {
        Response = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "RecordIndex Error: OHA_PrmWriteLLDP(Index: 0x%X)", pPrm->record_index);
    }

	if (Response != OHA_OK)
	{
		OHA_UserRequestFinish(pHDBUser,pRQB,Response);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteLLDP: 0x%X)", Response);
		return;
	}

#if 0	/* AP01219785: no user query in case of a local PrmWrite.req (see also MultipleStationRecord) */
	Response = OHA_LLDPPrepareStats(pHDBUser->ThisHandle, &CountOfQueries, pPrm->mk_rema);
	if (Response != OHA_OK) /* no user ind resource(s) for query */
	{
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "OUT: OHA_PrmWriteLLDP(RetVal 0x%X): User query running or cancelled.", Response);
		Response = OHA_ERR_RESOURCE;

		OHA_UserRequestFinish(pHDBUser,pRQB,Response);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteLLDP: 0x%X)", Response);
		return;
	}
#endif

	if (CountOfQueries == 0) /* no user queries, continue */ 
	{
		/* statistic data (asynchronous) from LLDP_GET_STATS required ? */
		CountOfStats = OHA_LLDPDoStats(pHDBUser->ThisHandle, OHA_SNMP_SET_REQ, pPrm->mk_rema); /* OHA_HANDLE_TYPE_USER */
	}

	OHA_ASSERT(CountOfQueries == 0 || CountOfStats == 0);
	if (CountOfStats)
	{
	    if (pHDBUser->StatsPendCnt != 0) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* request is not queued */
		pHDBUser->StatsPendCnt = CountOfStats;  /* pending LLDP_OPC_STATS */

		/* lock the agent */
		/* queue the user-rqb */
		OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SnmpUserReq.pBottom,
							  pHDBUser->SnmpUserReq.pTop,
							  pRQB);

        pHDBUser->UserReqPendCnt++;  /* number of RQBs pending within LLDP */

		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteLLDP: 0x%X)", Response);
		return;
	}

	if (pPrm->record_index == OHA_RECORD_INDEX_MULTIPLE_SNMP)	/* SysName,... */
	{
		OHA_PrmWriteSNMP(pRQB, pHDBUser);
	}
	else
	{
	    OHA_UserRequestFinish(pHDBUser,pRQB,Response);
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteLLDP: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmLLDPInterfaceData                +*/
/*+  Input/Output          :    LLDP Interface record                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get TxInterval, LldpReinitDelay, LldpTxDelay,             +*/
/*+               TxHoldMultiplier and NotificationInterval                 +*/
/*+               from an LLDP Interface-record                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmLLDPInterfaceData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pOptions,
	OHA_LLDP_IF_PARAM_TYPE OHA_COMMON_MEM_ATTR * pConfig)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
	LSA_UINT16      Options = 0;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmLLDPInterfaceData (Data: 0x%X, Len: 0x%X)", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pOptions);

	*pOptions = 0;

    if (OHA_IS_NOT_NULL(pConfig))
    {
        (*pConfig).NotificationInterval = 0;
        (*pConfig).ReinitDelay = 0;
        (*pConfig).TxDelay = 0;
        (*pConfig).TxHoldMultiplier = 0;
        (*pConfig).TxInterval = 0;
    }

    if (RecordLen == 0)	/* get the default values */
	{
		*pOptions = OHA_CTRL_TX_INTERVAL | OHA_CTRL_TX_HOLD_MULTIPLIER | OHA_CTRL_REINIT_DELAY |
					OHA_CTRL_TX_DELAY | OHA_CTRL_NOTIFICATION_INTERVAL;

		if (OHA_IS_NOT_NULL(pConfig))
		{
			(*pConfig).TxInterval       = LLDP_DEFAULT_MSG_DELAY;
			(*pConfig).TxHoldMultiplier = OHA_LLDP_TTL_FACTOR;
			(*pConfig).ReinitDelay      = LLDP_DEFAULT_TX_REINIT_DELAY;
			(*pConfig).TxDelay          = LLDP_DEFAULT_TX_DELAY;
			(*pConfig).NotificationInterval = OHA_LLDP_NOTIFICATION_INTERVAL;
		}

        RetVal = OHA_OK;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OHA_Get_PrmLLDPInterfaceData: Get the default values by RecordLen (0x%X).", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPInterfaceData(RetVal: 0x%X)", RetVal);
        return(RetVal);
	}

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + sizeof(LSA_UINT16))) /* one entry at minimum */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPInterfaceData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPInterfaceData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* TxIntervalBlock:				BlockType = 0x7000	BlockHeader, LldpTxInterval (u16)			*/
	/* TxHoldMultiplierBlock:		BlockType = 0x7001	BlockHeader, LldpTxHoldMultiplier (u16)		*/
	/* ReinitDelayBlock:			BlockType = 0x7002	BlockHeader, LldpReinitDelay (u16)			*/
	/* TxDelayBlock:				BlockType = 0x7003	BlockHeader, LldpTxDelay (u16)				*/
	/* NotificationIntervalBlock:	BlockType = 0x7004	BlockHeader, LldpNotificationInterval (u16)	*/

	/* loop over [TxIntervalBlock], [TxHoldMultiplierBlock], [ReinitDelayBlock], [TxDelayBlock], [NotificationIntervalBlock] */
	while (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Value;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Value, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_LLDP_TX_INTERVAL:
			{
				/* SubBlock LldpTxInterval:                */
				/*     BlockHeader, TxInterval (u16)       */

				/* already one sub block LldpTxInterval read */
				if ((Options & OHA_CTRL_TX_INTERVAL) == OHA_CTRL_TX_INTERVAL)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_INTERVAL_MIN || Value > OHA_LLDP_TX_INTERVAL_MAX)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_TX_INTERVAL;
				if (OHA_IS_NOT_NULL(pConfig))
				{
					(*pConfig).TxInterval = Value;
				}
			}
			break;

			case OHA_BLOCK_TYPE_LLDP_TX_HOLD_MULTIPLIER:
			{
				/* SubBlock LldpTxHoldMultiplier:          */
				/*     BlockHeader, TxHoldMultiplier (u16) */

				/* already one sub block LldpTxHoldMultiplier read */
				if ((Options & OHA_CTRL_TX_HOLD_MULTIPLIER) == OHA_CTRL_TX_HOLD_MULTIPLIER)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TTL_FACTOR_MIN || Value > OHA_LLDP_TTL_FACTOR_MAX)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_TX_HOLD_MULTIPLIER;
				if (OHA_IS_NOT_NULL(pConfig))
				{
					(*pConfig).TxHoldMultiplier = Value;
				}
			}
			break;

			case OHA_BLOCK_TYPE_LLDP_REINIT_DELAY:
			{
				/* SubBlock LldpReinitDelay:          */
				/*     BlockHeader, ReinitDelay (u16) */

				/* already one sub block LldpReinitDelay read */
				if ((Options & OHA_CTRL_REINIT_DELAY) == OHA_CTRL_REINIT_DELAY)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value > 10)	/* (0,1..10 seconds) */
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_REINIT_DELAY;
				if (OHA_IS_NOT_NULL(pConfig))
				{
					(*pConfig).ReinitDelay = Value;
				}
			}
			break;

			case OHA_BLOCK_TYPE_LLDP_TX_DELAY:
			{
				/* SubBlock LldpTxDelay:          */
				/*     BlockHeader, TxDelay (u16) */

				/* already one sub block LldpTxDelay read */
				if ((Options & OHA_CTRL_TX_DELAY) == OHA_CTRL_TX_DELAY)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value > 8192)	/* (0,1..8192 seconds) */
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_TX_DELAY;
				if (OHA_IS_NOT_NULL(pConfig))
				{
					(*pConfig).TxDelay = Value;
				}
			}
			break;

			case OHA_BLOCK_TYPE_LLDP_NOTIFICATION_INTERVAL:
			{
				/* SubBlock LldpNotificationInterval:          */
				/*     BlockHeader, NotificationInterval (u16) */

				/* already one sub block LldpNotificationInterval read */
				if ((Options & OHA_CTRL_NOTIFICATION_INTERVAL) == OHA_CTRL_NOTIFICATION_INTERVAL)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: More than one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < 5 || Value > 3600)	/* (5..3600 seconds) */
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetExt_PdPortDataExpected: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetExt_PdPortDataExpected(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_NOTIFICATION_INTERVAL;
				if (OHA_IS_NOT_NULL(pConfig))
				{
					(*pConfig).NotificationInterval = Value;
				}
			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPInterfaceData: BlockHeader(BlockType: 0x%X) unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPInterfaceData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPInterfaceData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPInterfaceData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	*pOptions = Options;

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPInterfaceData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmLLDPPortData                     +*/
/*+  Input/Output          :    LLDP Interface record                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: retrieves parameters from an LLDP Port-record             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmLLDPPortData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pOptions,
	OHA_LLDP_PORT_PARAM_TYPE OHA_COMMON_MEM_ATTR * pPortConfig)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal = OHA_OK;
	LSA_UINT32      Options = 0;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmLLDPPortData(Data: 0x%X, Len: 0x%X)", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pOptions);
	
	*pOptions = 0;

    if (OHA_IS_NOT_NULL(pPortConfig))
    {
        (*pPortConfig).AdminStatus = 0;
        (*pPortConfig).Dot1PortProtoVlanTxEnable = 0;
        (*pPortConfig).Dot1ProtocolTxEnable      = 0;
        (*pPortConfig).Dot1VlanIdTxEnable        = 0;
        (*pPortConfig).Dot1VlanNameTxEnable      = 0;
        (*pPortConfig).Dot3TLVsTxEnable   = 0;
        (*pPortConfig).ManAddrTxEnable    = 0;
        (*pPortConfig).NotificationEnable = 0;
        (*pPortConfig).PnoAliasTxEnable = 0;
        (*pPortConfig).PnoDelayTxEnable = 0;
        (*pPortConfig).PnoMrpTxEnable   = 0;
        (*pPortConfig).PnoPortStatusTxEnable = 0;
        (*pPortConfig).PnoPtcpTxEnable = 0;
        (*pPortConfig).TLVsTxEnable    = 0;
    }

    if (RecordLen == 0)	/* get the default values */
	{
		*pOptions = OHA_CTRL_ADMIN_STATUS | OHA_CTRL_NOTIFICATION | OHA_CTRL_LLDP_MAN_ADDR |
					OHA_CTRL_PNO_DELAY | OHA_CTRL_PNO_PORT_STATUS | OHA_CTRL_PNO_ALIAS |
					OHA_CTRL_PNO_MRP | OHA_CTRL_PNO_PTCP | 
					OHA_CTRL_DOT1_VLAN_ID | OHA_CTRL_DOT1_VLAN_NAME |
					OHA_CTRL_DOT1_PROTO_VLAN | OHA_CTRL_DOT1_PROTOCOL |
					OHA_CTRL_LLDP_TLVS | OHA_CTRL_DOT3_TLVS;

		if (OHA_IS_NOT_NULL(pPortConfig))
		{
			(*pPortConfig).AdminStatus = OHA_LLDP_SEND_RECV;
			(*pPortConfig).Dot1PortProtoVlanTxEnable = OHA_LLDP_TX_DISABLE;
			(*pPortConfig).Dot1ProtocolTxEnable      = OHA_LLDP_TX_DISABLE;
			(*pPortConfig).Dot1VlanIdTxEnable        = OHA_LLDP_TX_DISABLE;
			(*pPortConfig).Dot1VlanNameTxEnable      = OHA_LLDP_TX_DISABLE;
			(*pPortConfig).Dot3TLVsTxEnable          = OHA_DOT3_TLVS_TX_ENABLE;
			(*pPortConfig).ManAddrTxEnable  = OHA_LLDP_TX_ENABLE;
			(*pPortConfig).NotificationEnable = OHA_LLDP_TX_DISABLE;
			(*pPortConfig).PnoAliasTxEnable = OHA_LLDP_TX_ENABLE;
			(*pPortConfig).PnoDelayTxEnable = OHA_LLDP_TX_ENABLE;
			(*pPortConfig).PnoMrpTxEnable   = OHA_LLDP_TX_ENABLE;
			(*pPortConfig).PnoPortStatusTxEnable = OHA_LLDP_TX_ENABLE;
			(*pPortConfig).PnoPtcpTxEnable  = OHA_LLDP_TX_ENABLE;
			(*pPortConfig).TLVsTxEnable     = OHA_LLDP_TLVS_TX_ENABLE;
		}

        RetVal = OHA_OK;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OHA_Get_PrmLLDPPortData: Get the default values by RecordLen (0x%X).", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
        return(RetVal);
	}

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + sizeof(LSA_UINT16))) /* one entry at minimum */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* AdminStatusBlock:				BlockType = 0x7005	BlockHeader,AdminStatus (u16)			*/
	/* NotificationEnableBlock:			BlockType = 0x7006	BlockHeader,NotificationEnable (u16)	*/
	/* TLVsTxEnableBlock:	    		BlockType = 0x7007	BlockHeader,TLVsTxEnable (u16)			*/
	/* ManAddrTxEnableBlock:			BlockType = 0x7008	BlockHeader,ManAddrTxEnable (u16)		*/
	/* Dot3TLVsTxEnableBlock:	    	BlockType = 0x7009	BlockHeader,Dot3TLVsTxEnable (u16)		*/

	/* Dot1VlanIdTxEnableBlock:			BlockType = 0x700A	BlockHeader,Dot1VlanIdTxEnable (u16)		*/
	/* Dot1VlanNameTxEnableBlock:		BlockType = 0x700B	BlockHeader,Dot1VlanNameTxEnable (u16)		*/
	/* Dot1PortProtoVlanTxEnableBlock:	BlockType = 0x700C	BlockHeader,Dot1PortProtoVlanTxEnable (u16)	*/
	/* Dot1ProtocolTxEnableBlock:		BlockType = 0x700D	BlockHeader,Dot1ProtocolTxEnable (u16)		*/

	/* PnoDelayTxEnableBlock:			BlockType = 0x700E	BlockHeader,PnoDelayTxEnable (u16)		*/
	/* PnoPortStatusTxEnableBlock:		BlockType = 0x700F	BlockHeader,PnoPortStatusTxEnable (u16)	*/
	/* PnoAliasTxEnableBlock:			BlockType = 0x7010	BlockHeader,PnoAliasTxEnable (u16)		*/
	/* PnoMrpTxEnableBlock:				BlockType = 0x7011	BlockHeader,PnoMrpTxEnable (u16)		*/
	/* PnoPtcpTxEnableBlock:			BlockType = 0x7012	BlockHeader,PnoPtcpTxEnable (u16)		*/

	/* loop over these block types */
	while (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Value;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Value, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_LLDP_ADMIN_STATUS:
			{
				/* SubBlock AdminStatus:                */
				/*     BlockHeader, AdminStatus (u16)       */

				/* already one sub block AdminStatus read */
				if ((Options & OHA_CTRL_ADMIN_STATUS) == OHA_CTRL_ADMIN_STATUS)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_SEND_ONLY || Value > OHA_LLDP_DISABLED)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_ADMIN_STATUS;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).AdminStatus = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_NOTIFICATION:
			{
				/* SubBlock NotificationEnable:                */
				/*     BlockHeader, NotificationEnable (u16)       */

				/* already one sub block NotificationEnable read */
				if ((Options & OHA_CTRL_NOTIFICATION) == OHA_CTRL_NOTIFICATION)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_NOTIFICATION;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).NotificationEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_TLVS_TX:
			{
				/* SubBlock TLVsTxEnable:                */
				/*     BlockHeader, TLVsTxEnable (u16)       */

				/* already one sub block TLVsTxEnable read */
				if ((Options & OHA_CTRL_LLDP_TLVS) == OHA_CTRL_LLDP_TLVS)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value > OHA_LLDP_TLVS_TX_ENABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_LLDP_TLVS;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).TLVsTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_MAN_ADDR_TX:
			{
				/* SubBlock ManAddrTxEnable:                */
				/*     BlockHeader, ManAddrTxEnable (u16)       */

				/* already one sub block ManAddrTxEnable read */
				if ((Options & OHA_CTRL_LLDP_MAN_ADDR) == OHA_CTRL_LLDP_MAN_ADDR)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_LLDP_MAN_ADDR;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).ManAddrTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_PNO_DELAY_TX:
			{
				/* SubBlock PnoDelayTxEnable:                */
				/*     BlockHeader, PnoDelayTxEnable (u16)       */

				/* already one sub block PnoDelayTxEnable read */
				if ((Options & OHA_CTRL_PNO_DELAY) == OHA_CTRL_PNO_DELAY)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_PNO_DELAY;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).PnoDelayTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_PNO_PORT_STATUS_TX:
			{
				/* SubBlock PnoPortStatusTxEnable:                */
				/*     BlockHeader, PnoPortStatusTxEnable (u16)       */

				/* already one sub block PnoPortStatusTxEnable read */
				if ((Options & OHA_CTRL_PNO_PORT_STATUS) == OHA_CTRL_PNO_PORT_STATUS)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_PNO_PORT_STATUS;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).PnoPortStatusTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_PNO_ALIAS_TX:
			{
				/* SubBlock PnoAliasTxEnable:                */
				/*     BlockHeader, PnoAliasTxEnable (u16)       */

				/* already one sub block PnoAliasTxEnable read */
				if ((Options & OHA_CTRL_PNO_ALIAS) == OHA_CTRL_PNO_ALIAS)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_PNO_ALIAS;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).PnoAliasTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_PNO_MRP_TX:
			{
				/* SubBlock PnoMrpTxEnable:                */
				/*     BlockHeader, PnoMrpTxEnable (u16)       */

				/* already one sub block PnoMrpTxEnable read */
				if ((Options & OHA_CTRL_PNO_MRP) == OHA_CTRL_PNO_MRP)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_PNO_MRP;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).PnoMrpTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_PNO_PTCP_TX:
			{
				/* SubBlock PnoPtcpTxEnable:                */
				/*     BlockHeader, PnoPtcpTxEnable (u16)       */

				/* already one sub block PnoPtcpTxEnable read */
				if ((Options & OHA_CTRL_PNO_PTCP) == OHA_CTRL_PNO_PTCP)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_PNO_PTCP;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).PnoPtcpTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_DOT1_VLAN_ID_TX:
			{
				/* SubBlock Dot1VlanIdTxEnable:                */
				/*     BlockHeader, Dot1VlanIdTxEnable (u16)       */

				/* already one sub block Dot1VlanIdTxEnable read */
				if ((Options & OHA_CTRL_DOT1_VLAN_ID) == OHA_CTRL_DOT1_VLAN_ID)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_DOT1_VLAN_ID;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).Dot1VlanIdTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_DOT1_VLAN_NAME_TX:
			{
				/* SubBlock Dot1VlanNameTxEnable:                */
				/*     BlockHeader, Dot1VlanNameTxEnable (u16)       */

				/* already one sub block Dot1VlanNameTxEnable read */
				if ((Options & OHA_CTRL_DOT1_VLAN_NAME) == OHA_CTRL_DOT1_VLAN_NAME)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_DOT1_VLAN_NAME;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).Dot1VlanNameTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_DOT1_PORT_PROTO_TX:
			{
				/* SubBlock Dot1PortProtoVlanTxEnable:                */
				/*     BlockHeader, Dot1PortProtoVlanTxEnable (u16)       */

				/* already one sub block Dot1PortProtoVlanTxEnable read */
				if ((Options & OHA_CTRL_DOT1_PROTO_VLAN) == OHA_CTRL_DOT1_PROTO_VLAN)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_DOT1_PROTO_VLAN;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).Dot1PortProtoVlanTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_LLDP_DOT1_PROTOCOL_TX:
			{
				/* SubBlock Dot1ProtocolTxEnable:                */
				/*     BlockHeader, Dot1ProtocolTxEnable (u16)       */

				/* already one sub block Dot1ProtocolTxEnable read */
				if ((Options & OHA_CTRL_DOT1_PROTOCOL) == OHA_CTRL_DOT1_PROTOCOL)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value < OHA_LLDP_TX_ENABLE || Value > OHA_LLDP_TX_DISABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_DOT1_PROTOCOL;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).Dot1ProtocolTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			case OHA_BLOCK_TYPE_DOT3_TLVS_TX:
			{
				/* SubBlock Dot3TLVsTxEnable:                */
				/*     BlockHeader, Dot3TLVsTxEnable (u16)       */

				/* already one sub block Dot3MacPhyConfigStatusTxEnable read */
				if ((Options & OHA_CTRL_DOT3_TLVS) == OHA_CTRL_DOT3_TLVS)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				/* check the range */
				if (Value > OHA_DOT3_TLVS_TX_ENABLE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Value (0x%X) of SubBlock (BlockType: 0x%X) out of range.", Value, BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				Options |= OHA_CTRL_DOT3_TLVS;
				if (OHA_IS_NOT_NULL(pPortConfig))
				{
					(*pPortConfig).Dot3TLVsTxEnable = (LSA_UINT8)Value;
				}
			}
			break;
			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: BlockHeader(BlockType: 0x%X) unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmLLDPPortData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	*pOptions = Options;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmLLDPPortData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Put_PrmLLDPPortData                     +*/
/*+  Input/Output          :    LLDP Interface record                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: writes parameters to an LLDP Port-record                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Put_PrmLLDPPortData(
    LSA_UINT16                  NICId,
	LSA_UINT16                  PortId,
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
	LSA_UINT32                  record_index,
    LSA_BOOL                    only_rema)
{
	LSA_UINT32                  Value;

    LSA_UINT16      Response = OHA_OK;
    LSA_UINT32      block_length, record_data_length;
	LSA_BOOL        IsRema  = LSA_FALSE;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Put_PrmLLDPPortData(NICId: 0x%X, PortId: 0x%X, RecIndex: 0x%X, Rema: 0x%X)",
                          NICId, PortId, record_index, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);
	OHA_ASSERT(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA ||
				record_index == OHA_RECORD_INDEX_LLDP_PORT_DEFAULT_DATA);
	record_data_length = *pDataLen;

    if (OHA_IS_NULL(pHDBLldp))
    {
		*pDataLen = 0;
		Response = OHA_ERR_PRM_DATA;
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_Put_PrmLLDPPortData failed. No interface (NICId: 0x%X)",NICId);
		return (Response);
    }
    if (OHA_IS_NOT_NULL(pRecordData))
    {
		if (record_data_length < OHA_TOPO_BLOCK_HEADER_PDU_SIZE)
		{
			*pDataLen = 0;
			Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_Put_PrmLLDPPortData failed. RecordDataLen too small (RecordDataLen: 0x%X)",record_data_length);
			return (Response);
		}
    }
	if (PortId == 0 || PortId > pHDBLldp->pDB->PortCnt)
	{
		Response = OHA_ERR_PARAM;  /* Portid is wrong */
		/* Error on getting handle. Abort. */
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_Put_PrmLLDPPortData failed. Portid is wrong (Port-ID: 0x%X)",PortId);
        return (Response);
	}

    block_length = 0; /* no header */

	/* AdminStatus, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_SEND_RECV;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_ADMIN_STATUS, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_ADMIN_STATUS,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* ManAddrTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_ENABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_LLDP_MAN_ADDR, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_MAN_ADDR_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* NotificationEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_DISABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_NOTIFICATION, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_NOTIFICATION,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* PnoAliasTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_ENABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_PNO_ALIAS, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_PNO_ALIAS_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* PnoDelayTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_ENABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_PNO_DELAY, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_PNO_DELAY_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* PnoMrpTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_ENABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_PNO_MRP, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_PNO_MRP_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* PnoPortStatusTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_ENABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_PNO_PORT_STATUS, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_PNO_PORT_STATUS_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* PnoPtcpTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_ENABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_PNO_PTCP, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_PNO_PTCP_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* TLVsTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TLVS_TX_ENABLE; /* PortDescTxEnable, SysCapTxEnable, SysDescTxEnable, SysNameTxEnable */
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_LLDP_TLVS, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_TLVS_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* Dot1PortProtoVlanTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_DISABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_DOT1_PROTO_VLAN, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
		{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_DOT1_PORT_PROTO_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* Dot1ProtocolTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_DISABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_DOT1_PROTOCOL, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_DOT1_PROTOCOL_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* Dot1VlanIdTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_DISABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_DOT1_VLAN_ID, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_DOT1_VLAN_ID_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* Dot1VlanNameTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_LLDP_TX_DISABLE;
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_DOT1_VLAN_NAME, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_LLDP_DOT1_VLAN_NAME_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

	/* Dot3TLVsTxEnable, default for OHA_RECORD_INDEX_LLDP_INTERFACE_PORT_DATA */
    Value = OHA_DOT3_TLVS_TX_ENABLE; /* Dot3LinkAggregationTxEnable, Dot3MacPhyConfigStatusTxEnable, */
									  /* Dot3MaxFrameSizeTxEnable, Dot3PowerViaMDITxEnable */
	if(record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_DOT3_TLVS, &Value, LSA_NULL, LSA_NULL);
	}
    if ((!only_rema) || (only_rema && IsRema))
	{
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2);
		if(OHA_IS_NOT_NULL(pRecordData))
		{
			if (block_length > record_data_length) goto RecLenFailed;
			oha_prm_write_header(pRecordData, OHA_BLOCK_TYPE_DOT3_TLVS_TX,
								 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);
			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
			OHA_PDU_SET_U16 (pRecordData, (LSA_UINT16)Value);
		}
	}

    if (OHA_IS_NOT_NULL(pRecordData))
    {
		if (block_length > record_data_length)
		{
RecLenFailed:
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Put_PrmLLDPPortData: Length of Record failed.");
			Response = OHA_ERR_PRM_DATA;
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Put_PrmLLDPPortData(Response: 0x%X)", Response);
			return (Response);
		}
	}

    *pDataLen = block_length;    /* the written length */

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Put_PrmLLDPPortData(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmSNMPRecord                       +*/
/*+  Input                 :    MultipleSNMPRecord                          +*/
/*+  Output                :    Single SNMPRecord                           +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: gets a single SNMP Record from a Multiple SNMP-record     +*/
/*+               depending on the given RecordIndex                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Note:  Some single SNMP records can be further read with the           +*/
/*+         following OHA helper functions depending on the RecordIndex.    +*/
/*+                                                                         +*/
/*+  OHA_RECORD_INDEX_LLDP_INTERFACE_DATA OHA_Get_PrmLLDPInterfaceData()    +*/	
/*+  OHA_RECORD_INDEX_LLDP_PORT_DATA      OHA_Get_PrmLLDPPortData()         +*/	
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmSNMPRecord(
    OHA_UPPER_MEM_U8_PTR_TYPE					pRecordData,	/* IN */
    LSA_UINT32									RecordLen,		/* IN */
	LSA_UINT32									RecordIndex,	/* IN */
	LSA_UINT16									PortID,	        /* IN */
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSnmpData,	/* OUT */
    OHA_COMMON_MEM_U32_PTR_TYPE					pSnmpLen)		/* OUT */
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecordData;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmPDEVData(OHA_Get_PrmSNMPRecord: 0x%X, Len: 0x%X, RecordIndex: 0x%X)", 
							pRecordData, RecordLen, RecordIndex);

    OHA_ASSERT_NULL_PTR(pRecordData);

    if (OHA_IS_NOT_NULL(pSnmpLen))
	{
		*pSnmpLen = 0;
	}
    if (OHA_IS_NOT_NULL(ppSnmpData))
    {
        *ppSnmpData = LSA_NULL;
    }

	if (RecordIndex == OHA_RECORD_INDEX_LLDP_PORT_DATA)
	{
		if (PortID == 0  || PortID > OHA_CFG_MAX_PORTS)
		{
			RetVal = OHA_ERR_PRM_PORTID;  /* Portid is wrong */
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPRecord: PortID (0x%X) is wrong.", PortID);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    if (RecordLen < OHA_REMA_RECORD_HEADER_LENGTH) /* sizeof(BlockHeader) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPRecord: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* (i) BlockHeader (RemaWriteHeader) is part of the RQB:                                */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
    while (RecordLen > index)
    {
        LSA_UINT32 BlockIndex;
        LSA_UINT16 SlotNumber;
        LSA_UINT16 SubSlotNumber;
        LSA_UINT32 BlockLength;
        LSA_UINT16 Reserved;

        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockIndex, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;   /* API */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SubSlotNumber, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockLength, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;   /* CompID */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (Reserved, pRecord);

        LSA_UNUSED_ARG(SlotNumber); /* no (more) check */

        /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
        /* shall only contain the length of the specific data octets without counting padding octets. */
        if ((index + BlockLength) > RecordLen)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPRecord: BlockHeader (RemaRecordDataLength = %u) failed.", BlockLength);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        if (Reserved != 0)
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPRecord: Reserved (%u) failed.", Reserved);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		index += BlockLength;
		if (index>RecordLen) goto RecLenFailed;

        if (BlockIndex == RecordIndex)
        {
			/* subrecord already found */
			if(OHA_IS_NOT_NULL(ppSnmpData) && OHA_IS_NOT_NULL(*ppSnmpData))
			{
				if (BlockIndex != OHA_RECORD_INDEX_LLDP_PORT_DATA)
				{		
					RetVal = OHA_ERR_PRM_BLOCK;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPRecord: More than one RecordIndex (BlockIndex: 0x%X) existing!", BlockIndex);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}
				else	/* port record */
				{
				    /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
					if(PortID == (SubSlotNumber & 0x00FF))
					{
						RetVal = OHA_ERR_PRM_BLOCK;
						OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPRecord: More than one RecordIndex (BlockIndex: 0x%X for PortID: 0x%X) existing!", BlockIndex, PortID);
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
				}
			}

			if (OHA_IS_NOT_NULL(ppSnmpData))
			{
				if (BlockIndex != OHA_RECORD_INDEX_LLDP_PORT_DATA)
				{		
					*ppSnmpData = pRecord;
				}
				else	/* port record */
				{
					if(PortID == (SubSlotNumber & 0x00FF))
					{
						*ppSnmpData = pRecord;
					}
				}
			}
		}

		pRecord+= BlockLength;

		/* (iii) Paddings */
		/* padding bytes after SysNameBlock */
		while ( (index & 3) != 0 )
		{
			index += 1;
			if (index>RecordLen) goto RecLenFailed;
			if (*pRecord != 0) /* check the paddings */
			{
				RetVal = OHA_ERR_PRM_DATA;
				OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmSNMPRecord() - wrong Padding Data !");
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
				return(RetVal);
			}
			pRecord += 1;                       /* padding byte   */
			BlockLength += 1;
		}

		/* DataLength shall be coded as data type Unsigned16 and shall contain the length of the specific */
		/* data of the type without DataLength itself and without counting padding octets.           */
        if (BlockIndex == RecordIndex)
        {
			if (OHA_IS_NOT_NULL(pSnmpLen))
			{
				if (BlockIndex != OHA_RECORD_INDEX_LLDP_PORT_DATA)
				{		
					*pSnmpLen = BlockLength;
				}
				else	/* port record */
				{
					if(PortID == (SubSlotNumber & 0x00FF))
					{
						*pSnmpLen = BlockLength;
					}
				}

			}
		}
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPRecord: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPRecord(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmControlTCP                           +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read/write TCP data records to/from OHA        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmControlTCP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16              NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmControlTCP(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pPrm  = &pRQB->args.PrmData;
	NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_WRITE || 
				OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_READ);

    if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
               LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ) )
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
	else if (OHA_IsAnyLocalCtrlTcIpActive(NICId)) /* ControlTimeParams from any (other) user running ? */
	{
		Response = OHA_ERR_SEQUENCE;  /* already one user request active */
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_OPC_PRM_WRITE failed. Request from another user is active (NIC-ID: 0x%X)!",NICId);
	}
    else if (OHA_IS_NULL(pPrm->record_data) || pPrm->record_data_length == 0)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_TCP_KEEP_ALIVE)
    {
		LSA_UINT16 KeepAliveTime = 0;

		if(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_WRITE)
		{
			Response = OHA_Get_PrmTCIPData(pPrm->record_data,     /* check the record and get the data */
										   pPrm->record_data_length,
										   &KeepAliveTime);
			if (Response != OHA_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) Getting TcpData failed.", pPrm->record_index);
				OHA_UserRequestFinish(pHDBUser,pRQB,Response);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmControlTCP: 0x%X)", Response);
				return;
			}
		}

		if(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_READ) /* TIA 991548 */
		{
			if (pPrm->record_data_length < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*KeepAliveTime*/ + 2/*Paddings*/)) /* one entry at minimum */
			{
				Response = OHA_ERR_PRM_DATA;
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_READ-Read: RecordLength (%u) too small.", pPrm->record_data_length);
				OHA_UserRequestFinish(pHDBUser,pRQB,Response);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmControlTCP: 0x%X)", Response);
				return;
			}
		}

		/* the tcip params are to set, queue the request and call SOCK_OPC_CNTRL_TIMEPARAMS */
		/* now we have an asynchronous call to SOCK */
		Response = OHA_SOCKControlTcipData(NICId, OHA_RQB_GET_HANDLE(pRQB), OHA_RQB_GET_OPCODE(pRQB), KeepAliveTime);
		if (Response != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Setting TCIP-Params failed (Response=%u).", Response);
		}
		else
		{
			/* queue the user-rqb */
			OHA_ASSERT(OHA_IS_NULL(pHDBUser->TcIpUserReq.pBottom));
			OHA_RQB_PUT_BLOCK_TOP(pHDBUser->TcIpUserReq.pBottom,
								  pHDBUser->TcIpUserReq.pTop,
								  pRQB);

			Response = OHA_OK_ACTIVE;
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmControlTCP: 0x%X)", Response);
			return;
		}
    }
    else
    {
        Response = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "RecordIndex Error: OHA_PrmWriteTCP(Index: 0x%X)", pPrm->record_index);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmControlTCP: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmTCIPData                         +*/
/*+  Input/Output          :    LLDP Interface record                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get KeepAliveTime from a TCPKeepAliveRecord               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmTCIPData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pKeepAliveTime)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;
	LSA_BOOL        KeepAliveTimeFound = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmLLDPInterfaceData (Data: 0x%X, Len: 0x%X)", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pKeepAliveTime);

	*pKeepAliveTime = 0;

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*KeepAliveTime*/ + 2/*Paddings*/)) /* one entry at minimum */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmTCIPData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* KeepAliveTimeBlock:	BlockType = 0x3012	BlockHeader, KeepAliveTime (u16) */

	/* loop over [KeepAliveTimeBlock] */
	while (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;
		LSA_UINT16 KeepAliveTime;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved(u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Reserved, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (KeepAliveTime, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*KeepAliveTime*/ + 2/*Paddings*/ - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmTCIPData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmTCIPData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (Reserved != 0)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmTCIPData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_TCP_KEEP_ALIVE_TIME:
			{
				/* SubBlock KeepAliveTime:                 */
				/*     BlockHeader, KeepAliveTime (u16)    */

				/* already one sub block KeepAliveTime read */
				if (KeepAliveTimeFound)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmTCIPData: More than one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				KeepAliveTimeFound = LSA_TRUE;
				*pKeepAliveTime = KeepAliveTime;

				/* padding bytes after KeepAliveTime */
				while ( (index & 3) != 0 )
				{
					index += 1;
					if (index>RecordLen) goto RecLenFailed;
					if (*pRecord != 0) /* check the paddings */
					{
						RetVal = OHA_ERR_PRM_DATA;
						OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmTCIPData() - wrong Padding Data !");
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
					pRecord += 1;         /* padding byte   */
				}
			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmTCIPData: BlockHeader(BlockType: 0x%X) unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmTCIPData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmTCIPData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmNameOfStationData                +*/
/*+  Input/Output          :    NameOfStationRecord                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get NoS from a NameOfStation-record                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmNameOfStationData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNameOfStation,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength
)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;
	LSA_BOOL        StationNameBlockFound = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmNameOfStationData(OHA_CheckPdPortData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);

	if (OHA_IS_NOT_NULL(pLength))
	{
		*pLength = 0;
	}
    if (!LSA_HOST_PTR_ARE_EQUAL(ppNameOfStation, LSA_NULL))
    {
        *ppNameOfStation = LSA_NULL;        /* name of station */
    }

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*NameOfStationLength*/ + 2/*Reserved*/ ))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* StationNameBlock:	BlockType = 0xA201	BlockHeader, NameOfStationLength (u16), Reserved (u16) */
	/*                                          NameOfStationValue, [Padding*]                         */

	/* loop over [StationNameBlock] */
	while (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;
		LSA_UINT16 NameOfStationLength;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved (u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Reserved, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*NameOfStationLength*/ + 2/*Reserved*/ - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (Reserved != 0)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_NAME_OF_STATION:
			{
				/* already one sub block StationNameBlock read */
				if (StationNameBlockFound)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}
				StationNameBlockFound = LSA_TRUE;

				index += 2;
				if (index>RecordLen) goto RecLenFailed;
				OHA_PDU_GET_U16 (NameOfStationLength, pRecord);

				if (NameOfStationLength > DCP_MAX_STATION_NAME_LEN)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: NameOfStationLength (%u) too big.", NameOfStationLength);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				index += 2;
				if (index>RecordLen) goto RecLenFailed;
				OHA_PDU_GET_U16 (Reserved, pRecord);

				if (Reserved != 0)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: Reserved failed.");
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				if (OHA_IS_NOT_NULL(pLength))
				{
					*pLength = NameOfStationLength;
				}

				if (NameOfStationLength != 0)
				{
					if (OHA_IS_NOT_NULL(ppNameOfStation))
					{
						*ppNameOfStation = pRecord;     /* name of station */
					}

					/* check outside now, if it's a RFC-name */
					pRecord += NameOfStationLength;
				}

				index += NameOfStationLength;

				/* padding bytes after NameOfStationValue */
				while ( (index & 3) != 0 )
				{
					index += 1;
					if (index>RecordLen) goto RecLenFailed;
					if (*pRecord != 0) /* check the paddings */
					{
						RetVal = OHA_ERR_PRM_DATA;
						OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmNameOfStationData() - wrong Padding Data !");
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
					pRecord += 1;         /* padding byte   */
				}
			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: BlockHeader(BlockType: 0x%X) invalid or unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmNameOfStationData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmNameOfStationData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmIpSuiteData                      +*/
/*+  Input/Output          :    IPSuiteRecord                               +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get IP-Suite from a IPSuite-record                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmIpSuiteData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;
	LSA_BOOL        IPSuiteBlockFound = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmIpSuiteData(pRecData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);

	if (OHA_IS_NOT_NULL(pLength))
	{
	    *pLength = 0;
	}
    if (!LSA_HOST_PTR_ARE_EQUAL(ppIpAddr, LSA_NULL))
    {
        *ppIpAddr = LSA_NULL;       /* IP suite */
    }

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + OHA_IP_SUITE_SIZE)) /* one entry at minimum */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIpSuiteData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* IPSuiteBlock:	BlockType = 0x3000	BlockHeader, Reserved, IPAddress */
	/*                                      SubnetMask, DefaultGateway       */

	/* loop over [IPSuiteBlock] */
	while (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved (u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Reserved, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + OHA_IP_SUITE_SIZE - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIpSuiteData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIpSuiteData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
		
		if (Reserved != 0)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIpSuiteData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_IP_SUITE:
			{
				/* already one sub block IPSuiteBlock read */
				if (IPSuiteBlockFound)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIpSuiteData: More then one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}
				IPSuiteBlockFound = LSA_TRUE;

				index += OHA_IP_SUITE_SIZE;
				if (index>RecordLen) goto RecLenFailed;

				/* IPParameterValue: IPAddress, Subnetmask, StandardGateway */
				if (OHA_IS_NOT_NULL(pLength))
				{
					*pLength = OHA_IP_SUITE_SIZE;
				}
				if (!LSA_HOST_PTR_ARE_EQUAL(ppIpAddr, LSA_NULL))
				{
					*ppIpAddr = pRecord;        /* ip suite */
				}

				/* we check the IP-address before */
				if (oha_is_valid_ip_address(OHA_GET32_NTOH(pRecord, 0),
											OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE, 0),
											OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE, 0),
											LSA_FALSE)) /* 0.0.0.0 is allowed by a local set */
				{
					RetVal = OHA_OK;    /* the record check is ok */
				}
				else
				{
					RetVal = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
					/* Error on getting IpAddress. Abort. */
					OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Invalid IP-Suite (IP: 0x%X, SubNet: 0x%X, GateWay: 0x%X)",
										 OHA_GET32_NTOH(pRecord,0),
										 OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE,0),
										 OHA_GET32_NTOH(pRecord+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,0));
				}
				pRecord += OHA_IP_SUITE_SIZE;
			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIpSuiteData: BlockHeader(BlockType: 0x%X) invalid or unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIpSuiteData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIpSuiteData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/

/* build NameOfStationRecord:

   BlockType = 0xA201 

   BlockHeader, Reserved(u16), NameOfStationLength(u16), Reserved(u16), NameOfStationValue, [Padding*] a)
      a) The number of padding octets shall be adapted to make the block Unsigned32 aligned.

    only_rema       LSA_TRUE   build (only) the REMA objects
                    LSA_FALSE  build all (PRM) objects
*/
LSA_UINT16 OHA_GetNameOfStationData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
    LSA_UINT16 RetVal   = OHA_OK;

    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pData;
    LSA_UINT32 block_length = 0;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetNameOfStationData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);

    /* (remanent) NameOfStation to write ? */
    if ((!only_rema) || oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, DCP_MASK_DS_REMANENT))
    {
        OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
        LSA_UINT16 Len = (LSA_UINT16)OHA_DB_READ_STATION_NAME(NICId, &NamePtr);

        if (Len && OHA_IS_NULL(NamePtr))    /* error */
        {
            RetVal = OHA_ERR_FAILED;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetNameOfStationData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		/* StationNameBlock: BlockHeader, NameOfStationLength, NameOfStationValue, [Padding*] */
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*NameOfStationLength*/ + 2/*Reserved*/ + Len);
        while ( (block_length & 3) != 0 ) /* padding bytes after NameOfStationValue (DWORD aligned) */
        {
            block_length += 1;
        }

        if (OHA_IS_NOT_NULL(pData))
        {
            if (block_length > *pDataLen)
			{
	            RetVal = OHA_ERR_BUFFER_OVERFLOW;

	            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		                              , "OUT: OHA_GetNameOfStationData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
			                          , RetVal, *pDataLen, block_length
				                     );

				/* respond the needed buffer length */
				*pDataLen = block_length;

			    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetNameOfStationData(RetVal: 0x%X)", RetVal);
				return (RetVal);
			}

            /* write the block */
			/* StationNameBlock: BlockHeader, NameOfStationLength, NameOfStationValue, [Padding*] */
			oha_prm_write_header(pData, OHA_BLOCK_TYPE_NAME_OF_STATION, block_length,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_TRUE);

			pRecordData += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/);
	        OHA_PDU_SET_U16 (pRecordData, Len);			 /* NameOfStationLength */
	        OHA_PDU_SET_U16 (pRecordData, 0);			 /* Reserved            */
		    OHA_PDU_SET_MEM (pRecordData, NamePtr, Len); /* NameOfStationValue  */

            while ( ((2/*NameOfStationLength*/ + 2/*Reserved*/ + Len) & 3) != 0 ) /* padding bytes after NameOfStationValue (DWORD aligned) */
            {
				Len++;
                OHA_PDU_SET_U8 (pRecordData, 0x0);              /* padding byte        */
            }
        }
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetNameOfStationData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/

/* build IPSuiteRecord:

   BlockType = 0x3000 

   BlockHeader, Reserved(u16), IPAddress, SubnetMask, DefaultGateway

    only_rema       LSA_TRUE   build (only) the REMA objects
                    LSA_FALSE  build all (PRM) objects
*/
LSA_UINT16 OHA_GetIPSuiteData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
    LSA_UINT16 RetVal   = OHA_OK;

    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pData;
    LSA_UINT32 block_length = 0;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetIPSuiteData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);

    /* IPSuite record to write ? */
    {
        OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr      = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pSubnetMask  = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pGateway     = LSA_NULL;

		if (only_rema)	/* build the IP object for REMA */
		{
			(LSA_VOID)oha_db_read_rema_ip_address(NICId, &pIpAddr);
			if (OHA_IS_NOT_NULL(pIpAddr))
			{
				pSubnetMask = pIpAddr     + OHA_IP_ADDR_SIZE;
				pGateway    = pSubnetMask + OHA_IP_ADDR_SIZE;
			}
		}
		else	/* build the (current) IP object for PRM */
		{
			/* read local IP-Address, Subnet-mask and Default Router Address */
			(LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &pIpAddr, &pSubnetMask, &pGateway);
		}

		/* (remanent) IP read ? */
		if (OHA_IS_NOT_NULL(pIpAddr) && OHA_IS_NOT_NULL(pSubnetMask) && OHA_IS_NOT_NULL(pGateway))
		{
			/* StationNameBlock: BlockHeader, NameOfStationLength, NameOfStationValue, [Padding*] */
			block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + OHA_IP_SUITE_SIZE);

			if (OHA_IS_NOT_NULL(pData))
			{
				if (block_length > *pDataLen)
				{
					RetVal = OHA_ERR_BUFFER_OVERFLOW;

					OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
										  , "OUT: OHA_GetIPSuiteData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
										  , RetVal, *pDataLen, block_length
										 );

					/* respond the needed buffer length */
					*pDataLen = block_length;

					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetIPSuiteData(RetVal: 0x%X)", RetVal);
					return (RetVal);
				}

				/* write the block */
				/* StationNameBlock: BlockHeader, NameOfStationLength, NameOfStationValue, [Padding*] */
				oha_prm_write_header(pData, OHA_BLOCK_TYPE_IP_SUITE, block_length,
									 OHA_TOPO_DATA_BLOCK_VERSION, LSA_TRUE);

				pRecordData += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/);
				OHA_PDU_SET_MEM(pRecordData, pIpAddr, OHA_IP_ADDR_SIZE);
				OHA_PDU_SET_MEM(pRecordData, pSubnetMask, OHA_IP_ADDR_SIZE);
				OHA_PDU_SET_MEM(pRecordData, pGateway, OHA_IP_ADDR_SIZE);
				LSA_UNUSED_ARG(pRecordData);
			}
		}
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetIPSuiteData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    RequestReadPDInterfacePrm                   +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_PRM                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the record PDInterfaceAdjust                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadPDInterfacePrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    LSA_UINT16      Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData = &pRQB->args.PrmData;
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadPDInterfacePrm(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    if (OHA_IS_NULL(pDB))
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestReadPDInterfacePrm failed. No database present (NIC-ID: 0x%X)",NICId);
        return (Response);
    }

    if (NICId == OHA_INVALID_NIC_ID || LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
	else if(!pDB->MultipleIfModeSetValid) /* 1095791: no record written ? */
	{
        Response = OHA_OK;  /* no data */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadPDInterfacePrm(Response: 0x%X)", Response);
        pPrmData->record_data_length = 0;
        return (Response);
	}
    else
    {
        LSA_UINT32 block_length;
        OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pRQB->args.PrmData.record_data;
        LSA_INT32 MultipleInterfaceMode = pDB->MultipleIfModeValid;

        /* PDInterfaceAdjust: BlockHeader, Padding, Padding, MultipleInterfaceMode, [Padding*] a)  */
        /*  a) The number of padding octets shall be adapted to make the block Unsigned32 aligned. */

        /* BlockHeader */
        block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 1/*Padding*/ + 1/*Padding*/;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        pRecordData += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 1 + 1);  /* offset */

        /* MultipleInterfaceMode (u32) */
        block_length += 4;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U32 (pRecordData, MultipleInterfaceMode);

        /* no padding byte(s) after MultipleInterfaceMode existing */

        if (block_length > pPrmData->record_data_length)
        {
RecLenFailed:
            Response = OHA_ERR_PRM_DATA;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestReadPDInterfacePrm(Response: 0x%X)", Response);
            return (Response);
        }
		LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

        /* the read length */
        pPrmData->record_data_length = block_length;

        /* finally write the BlockHeader PDPortDataReal */
		oha_prm_write_header(pRQB->args.PrmData.record_data,
							 OHA_BLOCK_TYPE_PD_INTERFACE_ADJUST,
							 block_length,
							 OHA_TOPO_DATA_BLOCK_VERSION,
							 LSA_TRUE);
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadPDInterfacePrm(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadPNIdentPrm                   +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ_PRM                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the record PNIdentification                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadPNIdentPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    LSA_UINT16      Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData = &pRQB->args.PrmData;
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);

	LSA_UINT16 VendorID = 0;
	LSA_UINT16 DeviceID = 0;
	LSA_UINT16 InstanceID = 0;
	LSA_BOOL   PNIdentExisting;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadPNIdentPrm(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    if (OHA_IS_NULL(pDB))
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestReadPNIdentPrm failed. No database present (NIC-ID: 0x%X)",NICId);
        return (Response);
    }

	PNIdentExisting = pDB->PNIdentValid.Existing; /* record written ? */
	if(PNIdentExisting)
	{
		VendorID   = pDB->PNIdentValid.VendorID;
		DeviceID   = pDB->PNIdentValid.DeviceID;
		InstanceID = pDB->PNIdentValid.InstanceID;
	}
	else	/* initial values written ? */
	{
        OHA_COMMON_MEM_U8_PTR_TYPE pName  = LSA_NULL;
        LSA_INT  len;

		len = OHA_DB_READ_STATION_ID(NICId, &pName);
		if ((len == (2*sizeof(LSA_UINT16))) && OHA_IS_NOT_NULL(pName))
		{
			VendorID = OHA_GET16_NTOH(pName, 0);
			DeviceID = OHA_GET16_NTOH(pName, sizeof(LSA_UINT16));

	        len = OHA_DB_READ_DEVICE_INSTANCE(NICId, &pName);
			if ((len == (sizeof(LSA_UINT16))) && pName)
			{
				InstanceID = OHA_GET16_NTOH(pName, 0); /* if existing, otherwise 0 */
			}
			PNIdentExisting = LSA_TRUE;
		}
	}

	if (LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data,  LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else if(PNIdentExisting) /* record written ? */
	{
        LSA_UINT32 block_length;
        OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pRQB->args.PrmData.record_data;

        /* PDInterfaceAdjust: BlockHeader, Padding, Padding, VendorID(u16), DeviceID(u16), InstanceID(u16), Reserved(6byte) */

        /* BlockHeader */
        block_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 1/*Padding*/ + 1/*Padding*/;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        pRecordData += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 1 + 1);  /* offset */

        /* VendorID (u16) */
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, VendorID);

        /* DeviceID (u16) */
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, DeviceID);

        /* InstanceID (u16) */
        block_length += 2;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, InstanceID);		/* read from the record */

        /* 6 padding bytes (see definition) */
        block_length += 6;
        if (block_length > pPrmData->record_data_length) goto RecLenFailed;
        OHA_PDU_SET_U16 (pRecordData, 0);
        OHA_PDU_SET_U16 (pRecordData, 0);
        OHA_PDU_SET_U16 (pRecordData, 0);

        if (block_length > pPrmData->record_data_length)
        {
RecLenFailed:
            Response = OHA_ERR_PRM_DATA;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestReadPNIdentPrm(Response: 0x%X)", Response);
            return (Response);
        }
		LSA_UNUSED_ARG(pRecordData);    /* not used anymore */

        /* the read length */
        pPrmData->record_data_length = block_length;

        /* finally write the BlockHeader PDPortDataReal */
		oha_prm_write_header(pRQB->args.PrmData.record_data,
							 OHA_BLOCK_TYPE_PN_IDENTIFICATION,
							 block_length,
							 OHA_TOPO_DATA_BLOCK_VERSION,
							 LSA_TRUE);
    }
	else
	{
        pPrmData->record_data_length = 0;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"OHA_RequestReadPNIdentPrm failed. No record written (NIC-ID: 0x%X)",NICId);
	}

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadPNIdentPrm(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmIfAdjustData                     +*/
/*+  Input/Output          :    LLDP Interface record                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get MultipleInterfaceMode from a PDInterfaceAdjustRecord  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmIfAdjustData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pMultipleInterfaceMode)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord  = pRecData;
	LSA_BOOL        MultipleIfModeFound = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmIfAdjustData (Data: 0x%X, Len: 0x%X)", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pMultipleInterfaceMode);

	*pMultipleInterfaceMode = 0;

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 4/*MultipleInterfaceMode*/ )) /* one entry at minimum */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIfAdjustData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDInterfaceAdjust: BlockType = 0x0250	BlockHeader, Padding, Padding, MultipleInterfaceMode, [Padding*] a)  */

	/* loop over [PDInterfaceAdjustBlock] */
	while (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;
		LSA_UINT32 MultipleIfMode;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved(u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Reserved, pRecord);
		index += 4;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U32 (MultipleIfMode, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 4/*KeepAliveTime*/ - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIfAdjustData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIfAdjustData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (Reserved != 0)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIfAdjustData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_PD_INTERFACE_ADJUST:
			{
				/* SubBlock PDInterfaceAdjust:                  */
				/*     BlockHeader, MultipleInterfaceMode (u32) */

				/* already one sub block PDInterfaceAdjust read */
				if (MultipleIfModeFound)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIfAdjustData: More than one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				MultipleIfModeFound = LSA_TRUE;
				*pMultipleInterfaceMode = MultipleIfMode;

				/* no padding bytes after MultipleInterfaceMode */
			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIfAdjustData: BlockHeader(BlockType: 0x%X) unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmIfAdjustData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmIfAdjustData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddIfAdjustData                  +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+ Description: Add (overwrite) the expected PDInterfaceAdjust record data +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddIfAdjustData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser)
{
    LSA_UINT16           Response;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddIfAdjustData(pRecordData: 0x%X, RecordLen: 0x%X, RecordIndex: 0x%X, pHDBUser: 0x%X)",
                          pRecordData, RecordLen, RecordIndex, pHDBUser);

    if (LSA_HOST_PTR_ARE_EQUAL(pRecordData,  LSA_NULL) || RecordLen == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else if (RecordIndex != OHA_RECORD_INDEX_PD_INTERFACE_ADJUST)
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT32  MultipleInterfaceMode = 0;
        LSA_UINT16  NICId = pHDBUser->Params.pDetail->Params.User.NICId;

        /****************************************************/
        /*       the record (PDInterfaceAdjust)             */
        /****************************************************/
        Response = OHA_Get_PrmIfAdjustData(pRecordData, RecordLen, &MultipleInterfaceMode);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE: Write Record PDInterfaceAdjust failed.");
        }

		/* Check: Only an existing NameOfDevice is conform to IEEE */
		if(MultipleInterfaceMode & OHA_NAME_OF_DEVICE_MASK)
		{
		    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
		    LSA_UINT16 Len = 0;

			(LSA_VOID)OHA_DB_READ_CHASSIS_NAME(&pVar, &Len);
			if(Len == 0 || OHA_IS_NULL(pVar))	/* no NameOfDevice existing */
			{
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"Request OHA_OPC_WRITE_PRM_DATA failed. No NameOfDevice present, see OHA_DB_WRITE_CHASSIS_NAME()!");
                Response = OHA_ERR_RESOURCE;
			}
		}

		if(Response == OHA_OK) /* write to temp FSU data */
        {
            OHA_DB_IF_PTR_TYPE   pDbIf = OHA_DB_GET_INTERFACE(NICId);

            if ( LSA_HOST_PTR_ARE_EQUAL(pDbIf,  LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PRM_DATA failed. No DataBase present (NIC-ID: 0x%X)",NICId);
                Response = OHA_ERR_RESOURCE;
            }
            else
            {
                /* write buffers (A) */
                pDbIf->MultipleIfModeTemp = MultipleInterfaceMode;
                pDbIf->MultipleIfModeSetTemp = LSA_TRUE;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddIfAdjustData(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddPnIdentData                  +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+ Description: Write the PNIdentification record data                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPnIdentData(
    OHA_UPPER_RQB_PTR_TYPE      pRQB,
    OHA_HDB_USER_PTR_TYPE       pHDBUser)
{
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16           Response;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddPnIdentData(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    pPrm = &pRQB->args.PrmData;
    OHA_ASSERT_NULL_PTR(pPrm);

    if (LSA_HOST_PTR_ARE_EQUAL(pPrm->record_data,  LSA_NULL) || pPrm->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
    }
    else if (pPrm->record_index != OHA_RECORD_INDEX_PN_IDENTIFICATION)
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        LSA_UINT16 NICId = pHDBUser->Params.pDetail->Params.User.NICId;

        LSA_UINT16 VendorID   = 0;
        LSA_UINT16 DeviceID   = 0;
        LSA_UINT16 InstanceID = 0;

        /****************************************************/
        /*       the record (PNIdentification)              */
        /****************************************************/
        Response = OHA_Get_PrmPnIdentData(pPrm->record_data, pPrm->record_data_length, &VendorID, &DeviceID, &InstanceID);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE: Write Record PNIdentification failed (Response: 0x%X).", Response);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPnIdentData(Response: 0x%X)", Response);
			return (Response);
        }

		if(VendorID == 0) /* reserved (see PNIO draft) */
		{
			Response = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE: Write Record PNIdentification failed (VendorID = 0).");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPnIdentData(Response: 0x%X)", Response);
			return (Response);
		}

		if(Response == OHA_OK) /* write to temp PNIdent data */
        {
            OHA_DB_IF_PTR_TYPE   pDbIf = OHA_DB_GET_INTERFACE(NICId);

            if ( LSA_HOST_PTR_ARE_EQUAL(pDbIf,  LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_WRITE_PRM_DATA failed. No DataBase present (NIC-ID: 0x%X)",NICId);
                Response = OHA_ERR_RESOURCE;
            }
            else
            {
                /* write buffers (A) */
				/* 936497 - PDIdentificationRecord has even to be accepted if VendorID and DeviceID differs from initialization */
				pDbIf->PNIdentTemp. VendorID  = VendorID;
				pDbIf->PNIdentTemp. DeviceID  = DeviceID;
                pDbIf->PNIdentTemp.InstanceID = InstanceID;
                pDbIf->PNIdentTemp.Remanent   = pPrm->mk_rema;
                pDbIf->PNIdentTemp.Existing   = LSA_TRUE;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddPnIdentData(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmPnIdentData                      +*/
/*+  Input/Output          :    PNIdentification data                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get PNIdent data from a PNIdentification Record           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmPnIdentData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pVendorID,
    OHA_COMMON_MEM_U16_PTR_TYPE pDeviceID,
    OHA_COMMON_MEM_U16_PTR_TYPE pInstanceID)
{
    LSA_UINT32      i, index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE pRecord = pRecData;
	LSA_BOOL        PNIdentBlockFound = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmPnIdentData (Data: 0x%X, Len: 0x%X)", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pVendorID);
    OHA_ASSERT_NULL_PTR(pDeviceID);
    OHA_ASSERT_NULL_PTR(pInstanceID);

	*pVendorID   = 0;
	*pDeviceID   = 0;
	*pInstanceID = 0;

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*VendorID*/ + 2/*DeviceID*/ + 2/*InstanceID*/) + 6/*padding up to 32 bit (see definition)*/)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPnIdentData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* PDInterfaceAdjust (BlockType = 0x3019): BlockHeader, Padding, Padding, VendorID(u16), DeviceID(u16), InstanceID(u16), Reserved(6byte) */

	/* loop over [PDIdentificationBlock] */
	while (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;
		LSA_UINT16 VendorID;
		LSA_UINT16 DeviceID;
		LSA_UINT16 InstanceID;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved(u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Reserved, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (VendorID, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (DeviceID, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (InstanceID, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2+2+2/*IDs*/ + 6/*paddings*/ - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPnIdentData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPnIdentData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (Reserved != 0)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPnIdentData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_PN_IDENTIFICATION:
			{
				/* SubBlock PNIdentification:                  */
				/*     BlockHeader, VendorID (u16), DeviceID (u16), InstanceID (u16), Reserved (6 bytes) */

				/* already one sub block PNIdentification read */
				if (PNIdentBlockFound)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPnIdentData: More than one BlockHeader (BlockType: 0x%X).", BlockType);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
					return(RetVal);
				}

				PNIdentBlockFound = LSA_TRUE;
				*pVendorID   = VendorID;
				*pDeviceID   = DeviceID;
				*pInstanceID = InstanceID;

				/* 6 padding bytes after PNIdentification data */
				for (i=0; i < 6; i++)
				{
					index += 1;
					if (index>RecordLen) goto RecLenFailed;
					if (*pRecord != 0) /* check the paddings */
					{
						RetVal = OHA_ERR_PRM_DATA;
						OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmPnIdentData() - wrong Padding Data !");
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
					pRecord += 1;         /* padding byte   */
				}

			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPnIdentData: BlockHeader(BlockType: 0x%X) unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmPnIdentData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmPnIdentData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmWriteDHCP                            +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDBUser   : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write DHCP data records to OHA                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteDHCP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16              NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmWriteDHCP(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pPrm  = &pRQB->args.PrmData;
	NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_WRITE);

    /* SubslotNumber e.g. 8001 - 8004 (Port 1 ... Port 4, IfID 1 -> i = 0 ...)  Format: 8ipp */
    if ((pPrm->subslot_number & 0xF0FF) != 0x8000)
    {
        Response = OHA_ERR_PRM_INDEX;
        /* Error: no ports. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid subslot_number (0x%X), format is 8ipp, pp must be 0 for an interface",pPrm->subslot_number);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteDHCP(Response: 0x%X)", Response);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }

    if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
               OHA_IS_NULL(pHDBUser->pOCReq)))
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE DHCP failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
	else if (OHA_IS_NULL(pPrm->record_data) && pPrm->record_data_length != 0) /* no buffer for read/write ? */
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE DHCP failed. No buffer for RecordData given (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
	}
    else if (pPrm->record_index == OHA_RECORD_INDEX_DHCP_PARAMS)
    {
		LSA_UINT16  RetVal;
		OHA_COMMON_MEM_U8_PTR_TYPE pDhcpData;
		OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;
		LSA_UINT16 DhcpDataLen  = 0;
		OHA_COMMON_MEM_U8_PTR_TYPE pClientID = LSA_NULL;
		LSA_UINT8   OptionCode  = 0;
		LSA_UINT8   ClientIDLen = 0;

		if(pPrm->record_data_length != 0) /* otherwise delete the object (by writing with length=0 to the database) */
		{
			Response = OHA_Get_PrmDHCPData(pPrm->record_data,     /* check the record and get the data */
										   pPrm->record_data_length,
										   &OptionCode,
										   &pClientID,
										   &ClientIDLen);
			if (Response != OHA_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE DHCP: RecordIndex (%u) Getting DhcpData failed.", pPrm->record_index);
				OHA_UserRequestFinish(pHDBUser,pRQB,Response);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteDHCP: 0x%X)", Response);
				return;
			}

			/* build the DHCP data (see RFC 2132): */
			/* The code for this option is 61, and its minimum length is 2. */

			/*    Code   Len   Type  Client-Identifier   */
			/*    +-----+-----+-----+-----+-----+---     */
			/*    |  61 |  n  |  t1 |  i1 |  i2 | ...    */
			/*    +-----+-----+-----+-----+-----+---     */

			/* use MAC (OptionCode=1):           */
			/* 0000   3d 01  01  cd              */
			/* use NoS (OptionCode=2):           */
			/* 0000   3d 01  00  cd              */
			/* use ClientID (OptionCode=3):      */
			/* 0000   3d 0b  00  6d 79 43 6c 69 65 6e 74 49 44 cd */

			DhcpDataLen = (LSA_UINT16)(3 + ClientIDLen);
			{
				OHA_ALLOC_LOCAL_MEM(&pVarTemp, DhcpDataLen);
				pDhcpData = (OHA_COMMON_MEM_U8_PTR_TYPE)pVarTemp;

				if (OHA_IS_NULL(pDhcpData))
				{
					Response = OHA_ERR_RESOURCE;
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE DHCP: RecordIndex (%u) Getting DhcpData memory failed.", pPrm->record_index);
					OHA_UserRequestFinish(pHDBUser,pRQB,Response);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteDHCP: 0x%X)", Response);
					return;
				}
			}

			*pDhcpData = 61;                          /* Code  */
			pDhcpData++;
			*pDhcpData = (LSA_UINT8)(ClientIDLen + 1)/*Type*/;     /* Len   */
			pDhcpData++;
			*pDhcpData = (OptionCode == 1) ? (LSA_UINT8)1 : (LSA_UINT8)0;   /* Type  */
			pDhcpData++;
			if(ClientIDLen)
			{                           /* Client-Identifier   */
				OHA_MEMCPY(pDhcpData, pClientID, ClientIDLen);
			}
		}

		pDhcpData = (OHA_COMMON_MEM_U8_PTR_TYPE)pVarTemp; /* rewind */

		/* finally write the DHCP params to the database */
		/* writing with length=0 means: DHCP object removed */
		{
            LSA_UINT8 AccessMode = OHA_READ_WRITE; /* default */
			
			(LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NICId, OHA_DCP_OPTION_DHCP, 61,
												 &AccessMode);
			RetVal = OHA_DB_WRITE_DCP_ELEMENT(NICId, OHA_DCP_OPTION_DHCP, 61, /* first byte = Id, suboption */
										      pDhcpData, DhcpDataLen, AccessMode, pPrm->mk_rema);
			if (RetVal != LSA_RET_OK)
			{
				Response = RetVal;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Setting DHCP-Params failed (Response=%u).", Response);
			}
		}

		if(OHA_IS_NOT_NULL(pDhcpData))
		{
			OHA_FREE_LOCAL_MEM(&RetVal, pDhcpData);
			OHA_MEM_FREE_FAIL_CHECK(RetVal);
		}

		OHA_DcpIndicateEventAllOtherUsers(pRQB, NICId);  /* confirm other users */

#ifndef OHA_CFG_NO_REMA
		(LSA_VOID)oha_rema_dhcp_update(NICId);
#endif
    }
    else
    {
        Response = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "RecordIndex Error: OHA_PrmWriteDHCP(Index: 0x%X)", pPrm->record_index);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteDHCP: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmDHCPData                         +*/
/*+  Input/Output          :    DHCPPararmetersRecord                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get data from a DHCPParameters-record                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmDHCPData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U8_PTR_TYPE  pOptionCode,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppClientID,
    OHA_COMMON_MEM_U8_PTR_TYPE  pClientIDLen
)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmDHCPData(OHA_CheckPdPortData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pOptionCode);
    OHA_ASSERT_NULL_PTR(pClientIDLen);

	*pClientIDLen = 0;
	*pOptionCode  = 0;
    if (!LSA_HOST_PTR_ARE_EQUAL(ppClientID, LSA_NULL))
    {
        *ppClientID = LSA_NULL;
    }

	/* DhcpBlock:	BlockType = 0x3008		BlockHeader, Reserved, OptionCode (u8), ClientIDLength (u8) */
	/*                                      ClientIDValue, [Padding*]                                   */
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;
		LSA_UINT8  OptionCode;
		LSA_UINT8  ClientIDLength;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved (u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Reserved, pRecord);

		if (BlockType != OHA_BLOCK_TYPE_DHCP_PARAMS)
		{
			RetVal = OHA_ERR_PRM_BLOCK;
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: BlockHeader(BlockType: 0x%X) invalid or unknown.", BlockType);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength != RecordLen - 2*sizeof(LSA_UINT16))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: BlockHeader (BlockLength: 0x%X) failed.", BlockLength);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (Reserved != 0) /* check paddings */
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		index += 1;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U8 (OptionCode, pRecord);

		/* OptionCode (u8):  */
		/*    0x01: use MAC Address    ClientIDLength = 0  */
		/*    0x02: use NameOfStation  ClientIDLength = 0  */
		/*    0x03: use ClientID       ClientIDLength <> 0 */
		if (OptionCode < 1 || OptionCode > 3)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: OptionCode (%u) out of range.", OptionCode);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		*pOptionCode = OptionCode;

		index += 1;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U8 (ClientIDLength, pRecord);

		if ((OptionCode == 1 && ClientIDLength != 0) || /* use MAC Address */
			(OptionCode == 2 && ClientIDLength != 0) || /* use NameOfStation */
			(OptionCode == 3 && ClientIDLength == 0))   /* use ClientID */
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: Not matching of OptionCode (%u) and ClientIDLength (%u)!", OptionCode, ClientIDLength);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		*pClientIDLen = ClientIDLength;

		index += ClientIDLength;
		if (index>RecordLen) goto RecLenFailed;

		if (ClientIDLength != 0)
		{
			if (OHA_IS_NOT_NULL(ppClientID))
			{
				*ppClientID = pRecord;     /* value of ClientID */
			}
			pRecord += ClientIDLength;
		}

		/* padding bytes after ClientID */
		while ( (index & 3) != 0 )
		{
			LSA_UINT8 pad;
			index += 1;
			if (index>RecordLen) goto RecLenFailed;
			OHA_PDU_GET_U8 (pad, pRecord);
			if (pad != 0) /* check the paddings */
			{
				RetVal = OHA_ERR_PRM_DATA;
				OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_Get_PrmDHCPData() - wrong Padding Data !");
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
				return(RetVal);
			}
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDHCPData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    OHA_PrmReadDHCP                             +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_READ                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads DHCPParameters record                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_PrmReadDHCP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData;
    LSA_UINT16      Response = OHA_OK;
    OHA_DB_IF_PTR_TYPE pDB;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmReadDHCP(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    OHA_ASSERT_NULL_PTR(pRQB);

    pPrmData = &pRQB->args.PrmData;

    pDB = OHA_DB_GET_INTERFACE(NICId);

    if (OHA_IS_NULL(pDB))
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_PrmReadDHCP failed. No database present (NIC-ID: 0x%X)",NICId);
        return (Response);
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data, LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadDHCP(Response: 0x%X)", Response);
        return (Response);
    }

    switch (pRQB->args.PrmData.record_index)
    {
        case OHA_RECORD_INDEX_DHCP_PARAMS:
            Response = OHA_GetDHCPData(NICId,
                                       pPrmData->record_data,
                                       &pPrmData->record_data_length,
                                       LSA_FALSE);

            pPrmData->mk_rema = oha_db_compare_remanence(NICId, OHA_DCP_OPTION_DHCP, 61, DCP_MASK_DS_REMANENT);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadDHCP(Response: 0x%X)", Response);
            return (Response);


        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* only a station record ! */
        break;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmReadDHCP(Response: 0x%X)", Response);
    return (Response);
}

/*----------------------------------------------------------------------------*/

/* build DHCPParametersRecord:

   BlockType = 0x3008 

   BlockHeader, Reserved, OptionCode (u8), ClientIDLength (u8), ClientIDValue, [Padding*] a)
      a) The number of padding octets shall be adapted to make the block Unsigned32 aligned.

    only_rema       LSA_TRUE   build (only) the REMA objects
                    LSA_FALSE  build all (PRM) objects
*/
LSA_UINT16 OHA_GetDHCPData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
    LSA_UINT16 RetVal   = OHA_OK;

    LSA_UINT32 data_length = 0;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetDHCPData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);

    /* (remanent) DHCPParameters to write ? */
    if((!only_rema) || oha_db_compare_remanence(NICId, OHA_DCP_OPTION_DHCP, 61, DCP_MASK_DS_REMANENT))
    {
        OHA_COMMON_MEM_U8_PTR_TYPE OptPtr = LSA_NULL;
		LSA_UINT8 Code;
		LSA_UINT8 Len;
		LSA_UINT8 OptionCode;
		LSA_UINT8 ClientIDLength;

	    OHA_DB_READ_DHCP_OPTION(NICId, &OptPtr, 61);
		if(OHA_IS_NULL(OptPtr))
        {
			*pDataLen = 0; /* element not existing */
			*pDataLen = 0;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetDHCPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		/* get the DHCP data (see RFC 2132): */
		/* The code for this option is 61, and its minimum length is 2. */

		/*    Code   Len   Type  Client-Identifier   */
		/*    +-----+-----+-----+-----+-----+---     */
		/*    |  61 |  n  |  t1 |  i1 |  i2 | ...    */
		/*    +-----+-----+-----+-----+-----+---     */
		Code = *OptPtr;
		Len  = *(OptPtr+1);

		if(Code != 61 || Len == 0)
        {
            RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: DHCP data failed(Code: 0x%X or Len 0x%X ).", Code, Len);
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetDHCPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

		/* use MAC (OptionCode=1):       */
		/* 0000   3d 01  01  cd          */
		/* use NoS (OptionCode=2):       */
		/* 0000   3d 01  00  cd          */
		/* use ClientID (OptionCode=3):  */
		/* 0000   3d 0b  00  6d 79 43 6c 69 65 6e 74 49 44 cd */
		{
			LSA_UINT8 Type = *(OptPtr+2);

			switch(Type)
			{
				case 0:
					OptionCode = (Len == 1) ? (LSA_UINT8)2 /* use NoS */ : (LSA_UINT8)3; /* use ClientID */
				break;
				case 1:
					if(Len == 1) /* otherwise error */
					{
						OptionCode = 1; /* use MAC */
						break;
					}
				default:
		            RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDHCPData: DHCP data invalid(Type: 0x%X or Len 0x%X ).", Type, Len);
					OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetDHCPData(RetVal: 0x%X)", RetVal);
				return(RetVal);
			}
		}

        /* and write it to the record */
		ClientIDLength = (LSA_UINT8)(Len - 1)/*Type*/;

		/* DhcpBlock: BlockHeader, Reserved, OptionCode (u8), ClientIDLength (u8),  ClientIDValue, [Padding*] */
		data_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*OptionCode, ClientIDLength*/ + ClientIDLength);
        while ( (data_length & 3) != 0 ) /* padding bytes after ClientIDValue (DWORD aligned) */
        {
            data_length += 1;
        }

        if (OHA_IS_NOT_NULL(pData))
        {
            if (data_length > *pDataLen)
			{
	            RetVal = OHA_ERR_BUFFER_OVERFLOW;
	            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,
		                              "OUT: OHA_GetDHCPData(RetVal: 0x%X)  got DataLen(%u) but need length(%u)",
			                          RetVal, *pDataLen, data_length);
				/* respond the needed buffer length */
				*pDataLen = data_length;
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			}

            /* write the block */
			/* DhcpBlock: BlockHeader, Reserved, OptionCode (u8), ClientIDLength (u8),  ClientIDValue, [Padding*] */
			oha_prm_write_header(pData, OHA_BLOCK_TYPE_DHCP_PARAMS, data_length,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_TRUE);
			pData += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/);
	        OHA_PDU_SET_U8 (pData, OptionCode);
	        OHA_PDU_SET_U8 (pData, ClientIDLength);
			if(ClientIDLength)
			{
			    OHA_PDU_SET_MEM (pData, (OptPtr+3), ClientIDLength); /* ClientIDValue  */
			}

            while ( ((2/*OptionCode, ClientIDLength*/ + ClientIDLength) & 3) != 0 ) /* padding bytes after ClientIDValue (DWORD aligned) */
            {
				ClientIDLength++;
                OHA_PDU_SET_U8 (pData, 0x0);       /* padding byte(s)   */
            }
        }
    }

    /* finally write the length ! */
    *pDataLen = data_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetDHCPData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmSNMPControlData                  +*/
/*+  Input/Output          :    SnmpControl record                          +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get SnmpControl from a SnmpControl Record                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmSNMPControlData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pSnmpControl)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmSNMPControlData (Data: 0x%X, Len: 0x%X)", pRecData, RecordLen);

    OHA_ASSERT_NULL_PTR(pRecData);
    OHA_ASSERT_NULL_PTR(pSnmpControl);

	*pSnmpControl = 0xffffffff;

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 4/*SnmpControl*/)) /* one entry */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPControlData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPControlData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* SnmpControlBlock:	BlockType = 0x7002	BlockHeader, SnmpControl (u32) */

	/* [SnmpControlBlock] */
	if (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;
		LSA_UINT32 SnmpControl;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved(u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (Reserved, pRecord);
		index += 4;
		if (index>RecordLen) goto RecLenFailed;
		SnmpControl  = OHA_GET32_NTOH (pRecord, 0);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 4/*SnmpControl*/ - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPControlData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPControlData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPControlData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPControlData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (Reserved != 0)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPControlData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPControlData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_SNMP_CONTROL:
			{
				/* SubBlock SnmpControl:                 */
				/*     BlockHeader, SnmpControl (u32)    */
				*pSnmpControl = SnmpControl;

			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPControlData: BlockHeader(BlockType: 0x%X) unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPControlData(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmSNMPControlData: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPControlData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmSNMPControlData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmWriteSNMPControl                     +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write SnmpControl record to OHA                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteSNMPControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16              NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmWriteSNMPControl(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pPrm  = &pRQB->args.PrmData;
    NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_WRITE);
    OHA_ASSERT(pPrm->record_index == OHA_RECORD_INDEX_SNMP_CONTROL);

    if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
               LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ) )
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if (OHA_IS_NULL(pPrm->record_data) || pPrm->record_data_length == 0)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }
	else if (pPrm->mk_rema)
	{
        Response = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. Remanence not allowed(mk_rema: 0x%X)",pPrm->mk_rema);
	}
	else
    {
		OHA_RequestSockControl(pRQB, pHDBUser);
	}

	if (Response != OHA_OK)
	{
		OHA_UserRequestFinish(pHDBUser,pRQB,Response);
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMPControl: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_PrmWriteBUS                             +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_WRITE               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write BUS data records to OHA                  +*/
/*+               - CommonBusParamsRecord, AdditionalBusParamsRecord,       +*/
/*+                 MultipleBusParams                                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteBUS(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response;
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrm;
    LSA_UINT16              NICId;
	LSA_UINT16 CommonBusLen   = 0;
	LSA_UINT16 AdditionBusLen = 0;
	OHA_COMMON_MEM_U8_PTR_TYPE pCommonBus   = LSA_NULL;
	OHA_COMMON_MEM_U8_PTR_TYPE pAdditionBus = LSA_NULL;
    LSA_UINT32 Options = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_PrmWriteBUS(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pPrm  = &pRQB->args.PrmData;
	NICId = pHDBUser->Params.pDetail->Params.User.NICId;

    if (pPrm->edd_port_id != 0) /* 1123469 - R1, no check of SubslotNumber */
    {
        Response = OHA_ERR_PRM_PORTID;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE failed. No valid edd_port_id (0x%X), must be 0 for an interface", pPrm->edd_port_id);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteBUS(Response: 0x%X)", Response);
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        return;
    }

    if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
               LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ) )
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE(BusParameters) failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if (OHA_IS_NULL(pPrm->record_data) || pPrm->record_data_length == 0)
    {
        Response = OHA_ERR_PRM_DATA;  /* record (sub block) too small */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_PRM_WRITE(BusParameters) failed. RecordDataLen too small (RecordDataLen: 0x%X)",pRQB->args.PrmData.record_data_length);
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_MULTIPLE_BUS)
    {
		Response = OHA_Get_PrmBUSData(pPrm->record_data,        /* get the multiple record data */
										pPrm->record_data_length,
                                        &Options,
										&pCommonBus, &CommonBusLen,
										&pAdditionBus, &AdditionBusLen);
        if (Response != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: Getting MultipleBusData failed (Response=%u).", Response);
        }
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_BUS_COMMON)
    {
		Response = OHA_Get_PrmBusDataCommon(pPrm->record_data,     /* get the record data */
											pPrm->record_data_length,
											&pCommonBus, &CommonBusLen);
		if (Response == OHA_OK)
		{
			Options |= OHA_CTRL_PROFIBUS_COMMON;
		}
		else
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) Getting CommonBusData failed.", pPrm->record_index);
        }
    }
    else if (pPrm->record_index == OHA_RECORD_INDEX_BUS_ADDITION)
    {
		Response = OHA_Get_PrmBusDataAddition(pPrm->record_data,     /* get the record data */
												pPrm->record_data_length,
												&pAdditionBus, &AdditionBusLen);
		if (Response == OHA_OK)
		{
			Options |= OHA_CTRL_PROFIBUS_ADDITION;
		}
		else
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_PRM_WRITE-Write: RecordIndex (%u) Getting CommonBusData failed.", pPrm->record_index);
        }
    }
    else
    {
        Response = OHA_ERR_PRM_INDEX;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "RecordIndex Error: OHA_PrmWriteBUS(Index: 0x%X)", pPrm->record_index);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

	if (Options & OHA_CTRL_PROFIBUS_COMMON)	/* write to the DB */
	{
		(LSA_VOID)OHA_DB_WRITE_PROFIBUS_COMMON(NICId, pCommonBus, pPrm->mk_rema);
		Options |= OHA_CTRL_PROFIBUS_COMMON;
	}
	if (Options & OHA_CTRL_PROFIBUS_ADDITION)
	{
		(LSA_VOID)OHA_DB_WRITE_PROFIBUS_ADDITION(NICId, pAdditionBus, pPrm->mk_rema);
		Options |= OHA_CTRL_PROFIBUS_ADDITION;
	}

    if (Options)      /* are changes made ? */
    {
#ifndef OHA_CFG_NO_REMA
        (LSA_VOID)oha_rema_bus_update(NICId);
#endif
        OHA_DcpIndicateEventAllOtherUsers(pRQB, NICId);  /* confirm other users */
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteBUS: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmBUSPData                         +*/
/*+  Input/Output          :    MultipleBusParamsRecord                     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get CommonBusParameters and AdditionalBusParameters       +*/
/*+               from a MultipleBusParams-record                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmBUSData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pOptions,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppCommonBus,
    OHA_COMMON_MEM_U16_PTR_TYPE pCommonBusLength,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppAdditionBus,
    OHA_COMMON_MEM_U16_PTR_TYPE pAdditionBusLength
)
{
    LSA_UINT32      index = 0;
    LSA_UINT16      RetVal;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;
    LSA_UINT32      Options = 0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmBUSPData: 0x%X", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);

    if (OHA_IS_NOT_NULL(pCommonBusLength))
    {
        *pCommonBusLength = 0;
    }
    if (OHA_IS_NOT_NULL(pAdditionBusLength))
    {
        *pAdditionBusLength = 0;
    }

    *pOptions = 0;

    if (!LSA_HOST_PTR_ARE_EQUAL(ppCommonBus, LSA_NULL))
    {
        *ppCommonBus = LSA_NULL;
    }
    if (!LSA_HOST_PTR_ARE_EQUAL(ppAdditionBus, LSA_NULL))
    {
        *ppAdditionBus = LSA_NULL;
    }

    if (RecordLen < OHA_REMA_RECORD_HEADER_LENGTH) /* sizeof(BlockHeader) */
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBUSPData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* (i) BlockHeader (RemaWriteHeader) is part of the RQB:                                */
    /*                 RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16), */
    /*                 RemaRecordDataLength (u32), RemaCompID(u16), Reserved(u16)           */
    while (RecordLen > index)
    {
        LSA_UINT32 BlockIndex;
        LSA_UINT16 SlotNumber;
        LSA_UINT16 SubSlotNumber;
        LSA_UINT32 BlockLength;
        LSA_UINT16 Reserved;

        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockIndex, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 4;   /* API */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SlotNumber, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (SubSlotNumber, pRecord);
        index += 4;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U32 (BlockLength, pRecord);
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        pRecord += 2;   /* CompID */
        index += 2;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U16 (Reserved, pRecord);

        LSA_UNUSED_ARG(SlotNumber); /* no (more) check */
        LSA_UNUSED_ARG(SubSlotNumber); /* no (more) check */

        /* The field RemaRecordDataLength shall be coded as data type Unsigned32. The field           */
        /* shall only contain the length of the specific data octets without counting padding octets. */
        if (((index + BlockLength) > RecordLen) ||  Reserved != 0)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBUSPData: BlockHeader (RemaRecordDataLength = %u or Reserved (%u)) failed.", BlockLength, Reserved);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        if ((BlockIndex != OHA_RECORD_INDEX_BUS_COMMON &&
             BlockIndex != OHA_RECORD_INDEX_BUS_ADDITION))
        {
            RetVal = OHA_ERR_PRM_BLOCK;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBUSPData: RecordIndex (%u)failed.", BlockIndex);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        switch (BlockIndex)
        {
            case OHA_RECORD_INDEX_BUS_COMMON:
            {
                if (Options & OHA_CTRL_PROFIBUS_COMMON)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBUSPData: More than one CommonBusParamsRecord (BlockIndex: 0x%X) existing!", BlockIndex);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

				/* CommonBusParametersRecord: */
				/* BlockHeader, BusParametersLength (28 octets, fixed), CommonBusParametersValue */
				RetVal = OHA_Get_PrmBusDataCommon(pRecord,  /* get the record data */
													BlockLength,
													ppCommonBus, pCommonBusLength);
				if (RetVal != OHA_OK)
				{
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X) failed!", RetVal);
                    return(RetVal);
				}
				OHA_ASSERT(*pCommonBusLength == DCP_DS_PBCOMMON_SIZE); /* fixed length */
				Options |= OHA_CTRL_PROFIBUS_COMMON;

				index += BlockLength;
                if (index>RecordLen) goto RecLenFailed;
				pRecord += BlockLength;
				OHA_ASSERT((index & 3) == 0);  /* the block must be Unsigned32 aligned */
            }
            break;

            case OHA_RECORD_INDEX_BUS_ADDITION:
            {
                if (Options & OHA_CTRL_PROFIBUS_ADDITION)
                {
                    RetVal = OHA_ERR_PRM_BLOCK;
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBUSPData: More than one AdditionalBusParamsRecord (BlockIndex: 0x%X) existing!", BlockIndex);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
                    return(RetVal);
                }

				/* AdditionalBusParametersRecord: */
				/* BlockHeader, BusParametersLength (26 octets, fixed), AdditionalParametersValue, Padding, Padding */
				RetVal = OHA_Get_PrmBusDataAddition(pRecord,  /* get the record data */
													BlockLength,
													ppAdditionBus, pAdditionBusLength);
				if (RetVal != OHA_OK)
				{
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X) failed!", RetVal);
                    return(RetVal);
				}
				OHA_ASSERT(*pAdditionBusLength == DCP_DS_PBADDITION_SIZE); /* fixed length */
				Options |= OHA_CTRL_PROFIBUS_ADDITION;

				index += BlockLength;
                if (index>RecordLen) goto RecLenFailed;
				pRecord += BlockLength;
				OHA_ASSERT((index & 3) == 0);  /* the block must be Unsigned32 aligned */
            }
            break;

            default:
                /* only a CommonBusParams- or a AdditionalBusParamsRecord ! */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid RecordIndex (0x%X)", BlockIndex);
                RetVal = OHA_ERR_PRM_INDEX;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
                return(RetVal);
        }
    }

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBUSPData: Length of Record failed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }
	 
	if(Options == OHA_CTRL_PROFIBUS_ADDITION) /* only additional params are not allowed */
	{
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBUSPData: Writing only additional params is not allowed (index = %u)!", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
	}

	*pOptions = Options;
    RetVal = OHA_OK;    /* the record is ok */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBUSPData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmBusDataCommon                    +*/
/*+  Input/Output          :    CommonBusParamsRecord                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get CommonBusParameters from a CommonBusParamsRecord      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmBusDataCommon(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppBusData,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength
)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmBusDataCommon(OHA_CheckPdPortData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);

	if (OHA_IS_NOT_NULL(pLength))
	{
		*pLength = 0;
	}
    if (!LSA_HOST_PTR_ARE_EQUAL(ppBusData, LSA_NULL))
    {
        *ppBusData = LSA_NULL;        /* BusParams */
    }

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*BusParametersLength*/))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataCommon: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataCommon(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* CommonBusParamsRecord (BlockType = 0x7020): BlockHeader, BusParametersLength (u16),     */
	/*                                             CommonBusParametersValue (28 octets, fixed) */
	if (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 BusDataLength;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved (u16) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength != RecordLen - 2*sizeof(LSA_UINT16))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataCommon: BlockHeader (BlockLength: 0x%X) failed.", BlockLength);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataCommon(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataCommon: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataCommon(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_BUS_COMMON:
			{
				index += 2;
				if (index>RecordLen) goto RecLenFailed;
				OHA_PDU_GET_U16 (BusDataLength, pRecord);

				if (OHA_IS_NOT_NULL(pLength))
				{
					*pLength = BusDataLength;
				}

				if (BusDataLength != 0)
				{
					if (OHA_IS_NOT_NULL(ppBusData))
					{
						*ppBusData = pRecord;     /* CommonBusParameters */
					}
					if (BusDataLength != DCP_DS_PBCOMMON_SIZE) /* CommonBusParameters have a fixed length of 28 octets */
					{
						RetVal = OHA_ERR_PRM_DATA;
					    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataCommon: Length of CommonBusParameters (fixed length of 28 bytes) failed (Length = %u)!", BusDataLength);
					}
					index += DCP_DS_PBCOMMON_SIZE;
				}
			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataCommon: BlockHeader(BlockType: 0x%X) invalid or unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataCommon(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataCommon: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataCommon(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataCommon(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmBusDataAddition                  +*/
/*+  Input/Output          :    CommonBusParamsRecord                       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get AdditionalBusParameters from an                       +*/
/*                AdditionalBusParamsRecord                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmBusDataAddition(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppBusData,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength
)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmBusDataAddition(OHA_CheckPdPortData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecData);

	if (OHA_IS_NOT_NULL(pLength))
	{
		*pLength = 0;
	}
    if (!LSA_HOST_PTR_ARE_EQUAL(ppBusData, LSA_NULL))
    {
        *ppBusData = LSA_NULL;        /* BusParams */
    }

    if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*BusParametersLength*/))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataAddition: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataAddition(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* AdditionalBusParamsRecord (BlockType = 0x7021): BlockHeader, BusParametersLength (u16),     */
	/*                           AdditionalBusParametersValue (26 octets, fixed), Padding, Padding */
	if (index < RecordLen)
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 BusDataLength;
		LSA_UINT16 Reserved;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) goto RecLenFailed;
		OHA_PDU_GET_U16 (BlockVersion, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength != RecordLen - 2*sizeof(LSA_UINT16))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataAddition: BlockHeader (BlockLength: 0x%X) failed.", BlockLength);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataAddition(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataAddition: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataAddition(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_BUS_ADDITION:
			{
				index += 2;
				if (index>RecordLen) goto RecLenFailed;
				OHA_PDU_GET_U16 (BusDataLength, pRecord);

				if (OHA_IS_NOT_NULL(pLength))
				{
					*pLength = BusDataLength;
				}

				if (BusDataLength != 0)
				{
					if (OHA_IS_NOT_NULL(ppBusData))
					{
						*ppBusData = pRecord;     /* AdditionalBusParameters */
					}
					if (BusDataLength != DCP_DS_PBADDITION_SIZE) /* AdditionalBusParameters have a fixed length of 28 octets */
					{
						RetVal = OHA_ERR_PRM_DATA;
					    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataAddition: Length of AdditionalBusParameters (fixed length of 26 bytes) failed (Length = %u)!", BusDataLength);
					}
					index += DCP_DS_PBADDITION_SIZE;

					/* the paddings */
					index += 2;
					if (index>RecordLen) goto RecLenFailed;
					pRecord += DCP_DS_PBADDITION_SIZE;
					OHA_PDU_GET_U16 (Reserved, pRecord);

					if (Reserved != 0)
					{
						RetVal = OHA_ERR_PRM_DATA;
						OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataAddition: Reserved failed.");
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataAddition(RetVal: 0x%X)", RetVal);
						return(RetVal);
					}
				}
			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataAddition: BlockHeader(BlockType: 0x%X) invalid or unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataAddition(RetVal: 0x%X)", RetVal);
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmBusDataAddition: Length of Record failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataAddition(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmBusDataAddition(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    OHA_RequestPrmReadBus                       +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_PRM_READ                +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.PrmData                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads CommonBusParams and AdditionalBusParams record      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestPrmReadBus(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NICId)
{
    OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData;
    LSA_UINT16      Response = OHA_OK;
    OHA_DB_IF_PTR_TYPE pDB;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPrmReadBus(pRQB: 0x%X, NICId: 0x%X)",
                          pRQB, NICId);

    OHA_ASSERT_NULL_PTR(pRQB);

    pPrmData = &pRQB->args.PrmData;

    pDB = OHA_DB_GET_INTERFACE(NICId);

    if (OHA_IS_NULL(pDB))
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestPrmReadBus failed. No database present (NIC-ID: 0x%X)",NICId);
        return (Response);
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pPrmData->record_data, LSA_NULL) || pPrmData->record_data_length == 0) /* no buffer for write ? */
    {
        Response = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadBus(Response: 0x%X)", Response);
        return (Response);
    }

    switch (pRQB->args.PrmData.record_index)
    {
#if 0
        case OHA_RECORD_INDEX_MULTIPLE_BUS:
            Response = OHA_GetMultipleBusData(NICId,
                                              pPrmData->record_data,
                                              &pPrmData->record_data_length,
                                              LSA_FALSE);
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadBus(Response: 0x%X)", Response);
            return (Response);
#endif
        case OHA_RECORD_INDEX_BUS_COMMON:
            Response = OHA_GetCommonBusData(NICId,
                                            pPrmData->record_data,
                                            &pPrmData->record_data_length,
                                            LSA_FALSE);

            pPrmData->mk_rema = oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, DCP_MASK_DS_REMANENT);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadBus(Response: 0x%X)", Response);
            return (Response);

        case OHA_RECORD_INDEX_BUS_ADDITION:
            Response = OHA_GetAdditionBusData(NICId,
                                              pPrmData->record_data,
                                              &pPrmData->record_data_length,
                                              LSA_FALSE);

            pPrmData->mk_rema = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadBus(Response: 0x%X)", Response);
            return (Response);

        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);  /* only a station record ! */
            break;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPrmReadBus(Response: 0x%X)", Response);
    return (Response);
}

/*----------------------------------------------------------------------------*/
/* build CommonBusParamsRecord:

   BlockType = 0x7020 

   CommonBusParametersRecord:
   BlockHeader, BusParametersLength (28 octets, fixed), CommonBusParametersValue
    only_rema       LSA_TRUE   build (only) the REMA objects
                    LSA_FALSE  build all (PRM) objects
*/
LSA_UINT16 OHA_GetCommonBusData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
    LSA_UINT16 RetVal   = OHA_OK;

    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pData;
    LSA_UINT32 block_length = 0;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetCommonBusData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);

    /* (remanent) CommonBusParameters to read ? */
    if ((!only_rema) || oha_db_compare_remanence(NICId, DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON, DCP_MASK_DS_REMANENT))
    {
        OHA_COMMON_MEM_U8_PTR_TYPE ParamPtr = LSA_NULL;
        (LSA_VOID)OHA_DB_READ_PROFIBUS_COMMON(NICId, &ParamPtr);

        if (OHA_IS_NULL(ParamPtr))    /* no data */
        {
            *pDataLen = 0;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetCommonBusData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* BlockHeader, BusParametersLength (28 octets, fixed), CommonBusParametersValue */
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*BusParametersLength*/ + DCP_DS_PBCOMMON_SIZE);

        if (OHA_IS_NOT_NULL(pData))
        {
            if (block_length > *pDataLen)
			{
	            RetVal = OHA_ERR_BUFFER_OVERFLOW;

	            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		                              , "OUT: OHA_GetCommonBusData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
			                          , RetVal, *pDataLen, block_length
				                     );

				/* respond the needed buffer length */
				*pDataLen = block_length;

			    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetCommonBusData(RetVal: 0x%X)", RetVal);
				return (RetVal);
			}

            /* write the block */
            /* BlockHeader, BusParametersLength (28 octets, fixed), CommonBusParametersValue */
			oha_prm_write_header(pData, OHA_BLOCK_TYPE_BUS_COMMON, block_length,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);

			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
	        OHA_PDU_SET_U16 (pRecordData, DCP_DS_PBCOMMON_SIZE);			/* BusParametersLength */
		    OHA_PDU_SET_MEM (pRecordData, ParamPtr, DCP_DS_PBCOMMON_SIZE);	/* BusParametersValue  */
        }
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetCommonBusData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/
/* build AdditionalBusParamsRecord:

   BlockType = 0x7021 

   CommonBusParametersRecord:
   BlockHeader, BusParametersLength (26 octets, fixed), AdditionalBusParametersValue
    only_rema       LSA_TRUE   build (only) the REMA objects
                    LSA_FALSE  build all (PRM) objects
*/
LSA_UINT16 OHA_GetAdditionBusData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
    LSA_UINT16 RetVal   = OHA_OK;

    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pData;
    LSA_UINT32 block_length = 0;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetAdditionBusData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

    OHA_ASSERT_NULL_PTR(pDataLen);

    /* (remanent) CommonBusParameters to read ? */
    if ((!only_rema) || oha_db_compare_remanence(NICId, DCP_DBID_PBPARAM, DCP_DSID_PBADDITION, DCP_MASK_DS_REMANENT))
    {
        OHA_COMMON_MEM_U8_PTR_TYPE ParamPtr = LSA_NULL;
        (LSA_VOID)OHA_DB_READ_PROFIBUS_ADDITION(NICId, &ParamPtr);

        if (OHA_IS_NULL(ParamPtr))    /* no data */
        {
            *pDataLen = 0;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetAdditionBusData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* BlockHeader, BusParametersLength (26 octets, fixed), AdditionalBusParametersValue */
		block_length += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*BusParametersLength*/ + DCP_DS_PBADDITION_SIZE + 2/*paddings*/);

        if (OHA_IS_NOT_NULL(pData))
        {
            if (block_length > *pDataLen)
			{
	            RetVal = OHA_ERR_BUFFER_OVERFLOW;

	            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
		                              , "OUT: OHA_GetAdditionBusData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
			                          , RetVal, *pDataLen, block_length
				                     );

				/* respond the needed buffer length */
				*pDataLen = block_length;

			    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetAdditionBusData(RetVal: 0x%X)", RetVal);
				return (RetVal);
			}

            /* write the block */
            /* BlockHeader, BusParametersLength (28 octets, fixed), CommonBusParametersValue */
			oha_prm_write_header(pData, OHA_BLOCK_TYPE_BUS_ADDITION, block_length,
								 OHA_TOPO_DATA_BLOCK_VERSION, LSA_FALSE);

			pRecordData += OHA_TOPO_BLOCK_HEADER_PDU_SIZE;
	        OHA_PDU_SET_U16 (pRecordData, DCP_DS_PBADDITION_SIZE);			/* BusParametersLength */
		    OHA_PDU_SET_MEM (pRecordData, ParamPtr, DCP_DS_PBADDITION_SIZE);/* BusParametersValue  */
            OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte        */
            OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte        */
        }
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetAdditionBusData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*----------------------------------------------------------------------------*/

/* build a MultipleBusParams record:

   RemaWriteMultiple    RemaWriteHeader a), (RemaWriteHeader, CommonBusParamsRecord ^ AdditionalBusParamsRecord, [Padding*] a)*

      a) The number of padding octets shall be adapted to make the block Unsigned32 aligned.

   RemaWriteHeader  RecordIndex (u32), API, SlotNumber, SubslotNumber, RemaRecordDataLength (u32)
                    RemaCompID(u16), Reserved(u16)

    only_rema       LSA_TRUE   build (only) the REMA objects
                    LSA_FALSE  build all (PRM) objects
*/
LSA_UINT16 OHA_GetMultipleBusData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema)
{
	OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);
    LSA_UINT16         subslot_nr;
    LSA_UINT16 RetVal  = OHA_OK;

    LSA_UINT32 block_length = 0;

    LSA_UINT32 record_data_length;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetMultipleBusData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X, only_rema: 0x%X)",
                          NICId, pData, pDataLen, only_rema);

    OHA_ASSERT_NULL_PTR(pDB);
	OHA_ASSERT_NULL_PTR(pDataLen);

    /* The subslotnumber shall be evaluated with 0x8IPP with I counting interfaces */
    /* (P := 00 means interface itself) and P counting ports.                      */
    subslot_nr = (LSA_UINT16)(0x8000 | (((LSA_UINT16)(pDB->InterfaceID - 1)) << 8));

    /* (remanent) CommonBusParameters to write ? */
	record_data_length = 0;
	(LSA_VOID)OHA_GetCommonBusData(NICId, LSA_NULL, &record_data_length, only_rema);
    if (record_data_length) /* CommonBusParameters(28 bytes) */
	{
        block_length += OHA_REMA_RECORD_HEADER_LENGTH;

		if (OHA_IS_NOT_NULL(pData)) /* write the record */
		{
			if (block_length > *pDataLen) goto RecLenFailed;

			/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
			/*                  RemaRecordDataLength, RemaCompID(u16), Reserved(u16) */
			OHA_PDU_SET_U32 (pData, OHA_RECORD_INDEX_BUS_COMMON);
			OHA_PDU_SET_U32 (pData, 0);                       /* API (default) */
			OHA_PDU_SET_U16 (pData, 0);                       /* SlotNumber    */
			OHA_PDU_SET_U16 (pData, subslot_nr);     /* SubslotNumber (0x8i00) */
			OHA_PDU_SET_U32 (pData, record_data_length);  /* the record data length */
			OHA_PDU_SET_U16 (pData, 0);			/* AP01092248: comp_id removed */
			OHA_PDU_SET_U16 (pData, 0);                            /* reserved */
		}

        block_length += record_data_length; /* CommonBusParameters(28 bytes) */
		if (OHA_IS_NOT_NULL(pData)) /* write the record */
		{
			if (block_length > *pDataLen) goto RecLenFailed;

			(LSA_VOID)OHA_GetCommonBusData(NICId, pData, &record_data_length, only_rema);

			pData += record_data_length;
		}
	}

    /* (remanent) AdditionalBusParameters to write ? */
	record_data_length = 0;
	(LSA_VOID)OHA_GetAdditionBusData(NICId, LSA_NULL, &record_data_length, only_rema);
    if (record_data_length) /* AdditionalBusParameters(26 bytes) */
	{
        block_length += OHA_REMA_RECORD_HEADER_LENGTH;

		if (OHA_IS_NOT_NULL(pData)) /* write the record */
		{
		    if (block_length > *pDataLen) goto RecLenFailed;

			/* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
			/*                  RemaRecordDataLength, RemaCompID(u16), Reserved(u16) */
			OHA_PDU_SET_U32 (pData, OHA_RECORD_INDEX_BUS_ADDITION);
			OHA_PDU_SET_U32 (pData, 0);                       /* API (default) */
			OHA_PDU_SET_U16 (pData, 0);                       /* SlotNumber    */
			OHA_PDU_SET_U16 (pData, subslot_nr);  /* SubslotNumber (0x8i00) */
			OHA_PDU_SET_U32 (pData, record_data_length);  /* the record data length */
			OHA_PDU_SET_U16 (pData, 0);			/* AP01092248: comp_id removed */
			OHA_PDU_SET_U16 (pData, 0);                       /* reserved */
		}

        block_length += record_data_length; /* CommonBusParameters(28 bytes) */

		if (OHA_IS_NOT_NULL(pData)) /* write the record */
		{
			if (block_length > *pDataLen) goto RecLenFailed;

			(LSA_VOID)OHA_GetAdditionBusData(NICId, pData, &record_data_length, only_rema);
		}
	}

    if (OHA_IS_NOT_NULL(pData))
    {
        if (block_length > *pDataLen)
        {
RecLenFailed:
            RetVal = OHA_ERR_BUFFER_OVERFLOW;

            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
                                  , "OUT: OHA_GetMultipleBusData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
                                  , RetVal, *pDataLen, block_length
                                 );
            /* respond the needed buffer length */
            *pDataLen = block_length;

            return (RetVal);
        }
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultipleBusData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*****************************************************************************/
/*  end of file OHA_PRM.C                                                    */
/*****************************************************************************/

