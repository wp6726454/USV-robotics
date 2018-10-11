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
/*  F i l e               &F: oha_dcpu.c                                :F&  */
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

#define LTRC_ACT_MODUL_ID  6
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_DCPU */

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
/*+  Functionname          :    OHA_DCPReleaseHDB                           +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE pHDB                   +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBDcp    : Pointer to User Handle to release                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release DCP -HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPReleaseHDB(
    OHA_HDB_DCP_PTR_TYPE pHDB)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    if (pHDB->pLowerDcpSetRqb)  /* stored LowerRqb for a DcpSet response */
    {                        /* the user has'nt responded a DcpSet    */
        oha_dcp_free_rcv_rqb(pHDB, pHDB->pLowerDcpSetRqb);
        pHDB->pLowerDcpSetRqb = 0;
    }
    pHDB->DcpState = OHA_HANDLE_STATE_FREE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPReleaseHDB()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPGetFreeHDB                           +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE OHA_LOCAL..pHDBDcp     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA DCP  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free DCP -HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPGetFreeHDB(
    OHA_HDB_DCP_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBDcp)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found = LSA_FALSE;
    OHA_HDB_DCP_PTR_TYPE      pHDB  = OHA_GET_HDCP_PTR(0);
    LSA_UINT32                i     = 0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPGetFreeHDB(pHDB: 0x%X)",
                          pHDBDcp);

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    while ( (! Found)  &&  ( i < OHA_CFG_MAX_NICS))
    {
        if ( pHDB->DcpState == OHA_HANDLE_STATE_FREE )
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
        pHDB->DcpState = OHA_HANDLE_STATE_CLOSED;
        *pHDBDcp = pHDB;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_DCPGetFreeHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DCPGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBDcp);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestDcpIndication                    +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_DCP_INDICATION         +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.DcpInd                                  +*/
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
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestDcpIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestDcpIndication(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);

    pRQB->args.DcpInd.Count  = 0;  /* init. */
    pRQB->args.DcpInd.Reason = OHA_DCP_REASON_OK;

    /* queue the request */
    OHA_RQB_PUT_BLOCK_TOP(pHDBUser->DcpIndReq.pBottom,
                          pHDBUser->DcpIndReq.pTop,
                          pRQB);

    pHDBUser->DcpIndReqPending++;

    /* if this is the first Indication Resource, we check if some */
    /* indications outstanding */

    if ( pHDBUser->pLastDcpInd )
    {
        LSA_UINT16 NicId = pHDBUser->Params.pDetail->Params.User.NICId; /* get the DCP-LDB for this NIC-ID */
        OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NicId);
        if (OHA_IS_NOT_NULL(pHDBDcp))
        {
            (LSA_VOID)OHA_DcpIndicateEvent(pHDBUser, pHDBUser->pLastDcpInd);
        }
		else
		{
            OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP,
                                  "No (more) valid DCP-Handle for the buffered Dcp-Indication (pHDBUser->ThisHandle: 0x%X, pHDBUser->DCPHandle: 0x%X, pLastDcpInd->NicId: 0x%X)", 
								  pHDBUser->ThisHandle, pHDBUser->DCPHandle, NicId);
		}
    }

    /* if (Response != OHA_OK)                         */
    /* {                                               */
    /*  OHA_UserRequestFinish(pHDBUser,pRQB,Response); */
    /* }                                               */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestDcpIndication(DcpIndReqPending: 0x%X)", pHDBUser->DcpIndReqPending);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpIndicateEvent                        +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE              pHDBUser +*/
/*+                             OHA_UPPER_DCP_INDICATION_PTR_TYPE  pEvent   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pEvent               : Pointer to EventTag                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DCP-Set-Event to one user                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpIndicateEvent(
    OHA_HDB_USER_PTR_TYPE               pHDBUser,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE   pEvent
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal  = LSA_RET_OK;
    LSA_UINT16                  NicId; 
    OHA_HDB_DCP_PTR_TYPE        pHDBDcp;

    OHA_ASSERT_NULL_PTR(pHDBUser);

	NicId   = pHDBUser->Params.pDetail->Params.User.NICId; 
    pHDBDcp = OHA_DCPGetHandleByNIC(NicId);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpIndicateEvent(pHDBUser: 0x%X, pEvent: 0x%X)",
                          pHDBUser, pEvent);

    OHA_ASSERT_NULL_PTR(pEvent);

    /* look for a user-indication-filter:      */
    /* OHA_OPC_FILTER_INDICATIONS was called ! */

    if (pHDBUser->Params.pDetail->Params.User.DcpIndicationFilter != 0)	/* filter set ? */
    {
        if (pEvent->Event == OHA_EVENT_REMOTE_SET ||
            pEvent->Event == OHA_EVENT_LOCAL_SET  ||
            pEvent->Event == OHA_EVENT_CONTROL) /* only a DCP-Set, not a Query-Set ! */
        {
            if (pHDBUser->Params.pDetail->Params.User.DcpIndicationFilter & pEvent->Options) /* are there filter options set ? */
            {
            }
            else        /* !!! nothing to indicate !!! */
            {
                OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                                      "OUT: OHA_DcpIndicateEvent(EventOptions: 0x%X, DcpIndFilterOptions: 0x%X)", 
									  pEvent->Options, pHDBUser->Params.pDetail->Params.User.DcpIndicationFilter);
                return (RetVal);
            }
        }
    }

    /* get indication rqb */
    OHA_RQB_REM_BLOCK_BOTTOM(
        pHDBUser->DcpIndReq.pBottom,
        pHDBUser->DcpIndReq.pTop,
        pRQB);

    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb's exists ? */
    {
        OHA_UPPER_STATION_PTR_TYPE  pStationData = pRQB->args.DcpInd.pStationData;

        pRQB->args.DcpInd.Event         = pEvent->Event;

        pRQB->args.DcpInd.Options       = pEvent->Options;

        /* if there's an user-indication filter set, write only the filter-options to the Ind-CB */
        /* if it's a query-user, the filter is not considered ! */
        if (pHDBUser->Params.pDetail->Params.User.DcpIndicationFilter != 0)
        {
            if (pEvent->Event == OHA_EVENT_REMOTE_SET ||
                pEvent->Event == OHA_EVENT_LOCAL_SET  ||
                pEvent->Event == OHA_EVENT_CONTROL) /* only a DCP-Set, not a Query-Set ! */
            {
                pRQB->args.DcpInd.Options &= pHDBUser->Params.pDetail->Params.User.DcpIndicationFilter;
            }
        }
        pRQB->args.DcpInd.RemaOptions   = pEvent->RemaOptions;

        pRQB->args.DcpInd.Type          = pEvent->Type;
        pRQB->args.DcpInd.SubType       = pEvent->SubType;

        OHA_MEMCPY(&pRQB->args.DcpInd.SenderMACAddr,
                   &pEvent->SenderMACAddr, OHA_MAC_ADDR_SIZE);

        pRQB->args.DcpInd.CheckIpEnabled  = pEvent->CheckIpEnabled;
        pRQB->args.DcpInd.Count         = pEvent->Count;

        /* a station struct is added and it's a QueryUser ? */
        /* copy the temporary IP-Address and/or NameOfStation to the user-structs */
        if (OHA_IS_NOT_NULL(pStationData))
        {
            OHA_COMMON_MEM_U8_PTR_TYPE  NamePtr       = LSA_NULL;

            if (pEvent->Event == OHA_EVENT_QUERY_SET)
            {
                LSA_BOOL    Remanent  = LSA_TRUE;
                LSA_UINT8   AccCtrl = 0;

                if (pEvent->Options & OHA_CTRL_IP_SUITE)
                {
                    OHA_COMMON_MEM_U8_PTR_TYPE  IpSuitePtr     = LSA_NULL;
                    LSA_UINT16 size = oha_db_read_temp_ip_address(NicId,
                                                                  &IpSuitePtr, &AccCtrl, &Remanent);

                    if (size && OHA_IS_NOT_NULL(IpSuitePtr))
                    {
                        OHA_MEMCPY(&pStationData->LocalIpAddr, IpSuitePtr, OHA_IP_ADDR_SIZE);
                        OHA_MEMCPY(&pStationData->LocalSubnetMask, IpSuitePtr+OHA_IP_ADDR_SIZE, OHA_IP_ADDR_SIZE);
                        OHA_MEMCPY(&pStationData->DefaultRouterAddr, IpSuitePtr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE, OHA_IP_ADDR_SIZE);
                    }
                    else    /* no valid IP */
                    {
                        OHA_MEMSET(pStationData->LocalIpAddr, 0, OHA_IP_ADDR_SIZE);
                        OHA_MEMSET(pStationData->LocalSubnetMask, 0, OHA_IP_ADDR_SIZE);
                        OHA_MEMSET(pStationData->DefaultRouterAddr, 0, OHA_IP_ADDR_SIZE);
                    }
                }
                if (pEvent->Options & OHA_CTRL_STATION_NAME)
                {
                    LSA_UINT16 length = (LSA_UINT16)oha_db_read_temp_station_name(NicId, &NamePtr, &AccCtrl, &Remanent);

                    if (OHA_IS_NOT_NULL(NamePtr) && length && (length <= OHA_MAX_STATION_NAME_LEN))
                    {
                        pStationData->StationNameLen = length;
                        if (OHA_IS_NOT_NULL(pStationData->pStationName))
                        {
                            OHA_MEMCPY(pStationData->pStationName, NamePtr, length);
                        }
                    }
                    else    /* no valid name */
                    {
                        pStationData->StationNameLen = 0;
                    }
                }
            }
            /* a station struct is added and it's a local User or remote DCP server ? */
            /* copy the changed IP-Address and/or NameOfStation to the "other" user-structs */
            else if (pEvent->Event == OHA_EVENT_LOCAL_SET ||
                     pEvent->Event == OHA_EVENT_REMOTE_SET)
            {
                if (pEvent->Options & OHA_CTRL_IP_SUITE)
                {
                    OHA_COMMON_MEM_U8_PTR_TYPE  IpAddrPtr     = LSA_NULL;
                    OHA_COMMON_MEM_U8_PTR_TYPE  SubnetAddrPtr = LSA_NULL;
                    OHA_COMMON_MEM_U8_PTR_TYPE  RouterAddrPtr = LSA_NULL;

                    LSA_INT size = OHA_DB_READ_IP_SUITE(NicId, &IpAddrPtr, &SubnetAddrPtr, &RouterAddrPtr);

                    if (size && OHA_IS_NOT_NULL(IpAddrPtr) &&
                        OHA_IS_NOT_NULL(SubnetAddrPtr) && OHA_IS_NOT_NULL(RouterAddrPtr))
                    {
                        OHA_MEMCPY(pStationData->LocalIpAddr, IpAddrPtr, OHA_IP_ADDR_SIZE);
                        OHA_MEMCPY(pStationData->LocalSubnetMask, SubnetAddrPtr, OHA_IP_ADDR_SIZE);
                        OHA_MEMCPY(pStationData->DefaultRouterAddr, RouterAddrPtr, OHA_IP_ADDR_SIZE);
                    }
                    else    /* no valid IP */
                    {
                        OHA_MEMSET(pStationData->LocalIpAddr, 0, OHA_IP_ADDR_SIZE);
                        OHA_MEMSET(pStationData->LocalSubnetMask, 0, OHA_IP_ADDR_SIZE);
                        OHA_MEMSET(pStationData->DefaultRouterAddr, 0, OHA_IP_ADDR_SIZE);
                    }
                }
                if (pEvent->Options & OHA_CTRL_STATION_NAME)
                {
                    LSA_INT length = OHA_DB_READ_STATION_NAME(NicId, &NamePtr);

                    if (OHA_IS_NOT_NULL(NamePtr) && length && (length <= OHA_MAX_STATION_NAME_LEN))
                    {
                        pStationData->StationNameLen = (LSA_UINT16)length;
                        if (OHA_IS_NOT_NULL(pStationData->pStationName))
                        {
                            OHA_MEMCPY(pStationData->pStationName, NamePtr, length);
                        }
                    }
                    else    /* no valid name */
                    {
                        pStationData->StationNameLen = 0;
                    }
                }
            }
        }

        /* if there are no indication-rqb's: indicated by pLastDcpInd   */
        /* the data of the last indication is stored in the Dcp-Channel */
        if (pHDBDcp && LSA_HOST_PTR_ARE_EQUAL(pEvent, &pHDBDcp->LastSet))
        {
            pHDBUser->pLastDcpInd = 0;
        }
        pHDBUser->DcpIndReqPending--;

        OHA_UserRequestFinish(pHDBUser,pRQB,OHA_OK);
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        /* if there are no indication-rqb's: indicated by pLastDcpInd   */
        /* the data of the last indication is stored in the Dcp-Channel */
        if (pHDBDcp && LSA_HOST_PTR_ARE_EQUAL(pEvent, &pHDBDcp->LastSet))
        {
            pHDBUser->pLastDcpInd = pEvent;  /* store the last indication */
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpIndicateEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpWriteAccessControl                   +*/
/*+  Input/Output          :    LSA_UINT16         NicId                    +*/
/*+                             LSA_UINT16         control                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pPCtrl               : Pointer to PortManagement                       +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK: control-options written to DataBase     +*/
/*+                         <>OHA_OK: could not write                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets the access control options in the database           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpWriteAccessControl(
    LSA_UINT16 NicId,
    LSA_UINT32 options,
    LSA_UINT8 control)
{
    LSA_UINT16   Response = OHA_ERR_PARAM;
    LSA_UINT16   RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpWriteAccessControl(options: 0x%X, control: 0x%X)",
                          options, control);

    /* Station-Type and Station-Id are NIC-global - must be written to all interfaces (NICs) ! */
    if (options & OHA_CTRL_STATION_TYPE)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDTYPE, control);
        /* if (RetVal == LSA_RET_OK)    */
        /* {                            */
        /*  OHA_DcpSetAllNics(control, OHA_CTRL_STATION_TYPE); */
        /* }                            */
    }
    if (options & OHA_CTRL_STATION_ID)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDID, control);
        /* if (RetVal == LSA_RET_OK)    */
        /* {                            */
        /*  OHA_DcpSetAllNics(control, OHA_CTRL_STATION_ID); */
        /* }                            */
    }
    if (options & OHA_CTRL_DEVICE_INSTANCE)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDDEVINST, control);
    }

    if (options & OHA_CTRL_STATION_NAME)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDNAME, control);
    }

    if (options & OHA_CTRL_MAC_ADDR)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_MACADR, control);
    }
    if (options & OHA_CTRL_IP_SUITE)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, control);
    }

    if (options & OHA_CTRL_DHCP_OPTION)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_DHCP, 61, control);
    }
    if (options & OHA_CTRL_PROFIBUS_COMMON)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON, control);
    }
    if (options & OHA_CTRL_PROFIBUS_ADDITION)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBADDITION, control);
    }
    if (options & OHA_CTRL_RESET_TO_FACTORY)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_CTRL, OHA_DCP_RESET_TO_FACTORY, control);
    }
    if (options & OHA_CTRL_RESET_COMMUNICATION)
    {
        RetVal = OHA_DB_WRITE_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_CTRL, OHA_DCP_RESET, control);
    }

    if (RetVal == LSA_RET_OK)
    {
        Response = OHA_OK;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpWriteAccessControl(Response: 0x%X)", Response);

    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpWriteUserControl                     +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE                 UserHandle  +*/
/*+                             OHA_UPPER_DCP_CONTROL_PTR_TYPE  pControl    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  UserHandle           : User handle                                     +*/
/*+  pControl             : Pointer to RQB with DCP_CONTROL                 +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK: user-locks written to DataBase          +*/
/*+                         <>OHA_OK: could not write "UsrLock"             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets the access control options in the database           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpWriteUserControl(
	LSA_UINT16      NicId,
    LSA_HANDLE_TYPE UserHandle,
    OHA_UPPER_DCP_CONTROL_PTR_TYPE pControl)
{
    LSA_UINT16   Response;
    LSA_UINT16   RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpWriteUserControl(NicId: 0x%X, UserHandle: 0x%X, pControl: 0x%X)",
                          NicId, UserHandle, pControl);

    if (pControl->Options & OHA_CTRL_IP_SUITE)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_STATION_TYPE)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDTYPE, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_STATION_ID)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDID, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_DEVICE_INSTANCE)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDDEVINST, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_STATION_NAME)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDNAME, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_MAC_ADDR)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, OHA_DCP_OPTION_TCP, DCP_DSID_MACADR, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_DHCP_OPTION)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, OHA_DCP_OPTION_DHCP, 61, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_PROFIBUS_COMMON)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON, pControl->Mode);
    }
    if (pControl->Options & OHA_CTRL_PROFIBUS_ADDITION)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, DCP_DBID_PBPARAM, DCP_DSID_PBADDITION, pControl->Mode);
    }

    /* look at the type-entry */
    if (pControl->Type && pControl->SubType)
    {
        RetVal |= OHA_DbWriteUserControl(NicId, UserHandle, pControl->Type, pControl->SubType, pControl->Mode);
    }

	switch(RetVal)
	{
		case LSA_RET_OK:
		case LSA_RET_ERR_PARAM:
		case LSA_RET_ERR_RESOURCE:
			Response = RetVal;
		break;
		default:
			Response = OHA_ERR_FAILED;
		break;
	}

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpWriteUserControl(Response: 0x%X)", Response);

    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpIndicateEventAllUsers                +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE              pHDB      +*/
/*+                             OHA_UPPER_DCP_INDICATION_PTR_TYPE pEvent    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pEvent               : Pointer to eventcb                              +*/
/*+                                                                         +*/
/*+  Result               :                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DCP-Set-Event to all users,                   +*/
/*+               but not to a query-user.                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpIndicateEventAllUsers(
    OHA_HDB_DCP_PTR_TYPE                pHDBDcp,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE   pEvent
)
{
    LSA_UINT16                  i        = 0;
    /* LSA_BOOL                 Status   = LSA_TRUE;  */
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_UINT16                  NicId;

    OHA_ASSERT_NULL_PTR(pHDBDcp);
	NicId = pHDBDcp->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpIndicateEventAllUsers(pHDBDcp: 0x%X, pEvent: 0x%X)",
                          pHDBDcp, pEvent);

    OHA_ASSERT_NULL_PTR(pEvent);

    if (!(pEvent->Event == OHA_EVENT_REMOTE_SET ||
          pEvent->Event == OHA_EVENT_LOCAL_SET  ||
          pEvent->Event == OHA_EVENT_CONTROL))
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_DcpIndicateEventAllUsers failed.(Event: 0x%X).",pEvent->Event);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        /* is a DcpSet controlled by the user ? */
        if ( pHDBUser && pHDBUser->State == OHA_HANDLE_STATE_OPEN ) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NicId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
#if 0
				if (!(LSA_HOST_PTR_ARE_EQUAL(pHDBUser,pHDBDcp->pHDBUserCtrl) && /* no Query-User for a Set ? */
					  (pEvent->Event == OHA_EVENT_REMOTE_SET || pEvent->Event == OHA_EVENT_LOCAL_SET)))
				{
					(LSA_VOID)OHA_DcpIndicateEvent(pHDBUser, pEvent);
				}
#else
				if (LSA_HOST_PTR_ARE_EQUAL(pHDBUser,pHDBDcp->pHDBUserCtrl)) /* a Query-User for a Set ? */
				{
					/* local and remote set events are already indicated */
					if (pEvent->Event == OHA_EVENT_CONTROL)
					{
						LSA_BOOL AlreadyIndicatedAsQuery = LSA_FALSE;

						/* was the control event already a query event ? */
						if (oha_db_read_query_set_access(NicId, DCP_DBID_CTRL, OHA_DCP_RESET_TO_FACTORY) && /* TFS 657239 */
							(pEvent->Options & OHA_CTRL_RESET_TO_FACTORY) )
						{
							AlreadyIndicatedAsQuery = LSA_TRUE;
						}
						if (oha_db_read_query_set_access(NicId, DCP_DBID_CTRL, OHA_DCP_RESET) && /* TFS 657239 */
							(pEvent->Options & OHA_CTRL_RESET_COMMUNICATION) )
						{
							AlreadyIndicatedAsQuery = LSA_TRUE;
						}

						if (!AlreadyIndicatedAsQuery)
						{
							(LSA_VOID)OHA_DcpIndicateEvent(pHDBUser, pEvent);
						}
					}
					else /* TIA 903768 */
					{
						/* non of the query options -> an OHA controlled DCP-Set */
						if(pHDBDcp->QueryState == OHA_DCP_STATE_QUERY_SCAN)
						{
							(LSA_VOID)OHA_DcpIndicateEvent(pHDBUser, pEvent);
						}
					}
				}
				else    /* indicate every event */
				{
					(LSA_VOID)OHA_DcpIndicateEvent(pHDBUser, pEvent);
				}
#endif
			}
        }
        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpIndicateEventAllUsers()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpImmediateEventAllUsers               +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE              pHDB      +*/
/*+                             OHA_UPPER_DCP_INDICATION_PTR_TYPE pEvent    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pEvent               : Pointer to eventcb                              +*/
/*+                                                                         +*/
/*+  Result               :                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DCP-Set-Event to all users,                   +*/
/*+               but not to a control-user.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpImmediateEventAllUsers(
    OHA_HDB_DCP_PTR_TYPE                pHDBDcp,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE   pEvent
)
{
    LSA_UINT16                  i        = 0;
    /* LSA_BOOL                 Status   = LSA_TRUE;  */
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_UINT16                  NICId;

	OHA_ASSERT_NULL_PTR(pHDBDcp);
	NICId = pHDBDcp->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpImmediateEventAllUsers(pHDBDcp: 0x%X, pEvent: 0x%X)",
                          pHDBDcp, pEvent);

    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        /* is a DcpSet controlled by the user ? */
        if ( pHDBUser && pHDBUser->State == OHA_HANDLE_STATE_OPEN ) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
				if (pEvent->Event == OHA_EVENT_REMOTE_SET ||
					pEvent->Event == OHA_EVENT_LOCAL_SET  ||
					pEvent->Event == OHA_EVENT_CONTROL)
				{
					OHA_DcpIndicateEvent(pHDBUser, pEvent);
				}
			}
        }
        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpImmediateEventAllUsers()");
    return;
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpIndicateEventAllOtherUsers           +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE            pHDB      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  Result               :                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a User-Set-Event to all other users,            +*/
/*+               but not to the caused user.                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpIndicateEventAllOtherUsers(
    OHA_UPPER_RQB_PTR_TYPE              pRQB,
    LSA_UINT16                          NicId
)
{
    LSA_UINT16                  i;
    OHA_DCP_INDICATION_TYPE     Event;
    OHA_COMMON_MEM_U8_PTR_TYPE  AddrPtr  = LSA_NULL;
    LSA_INT                     Len;
    LSA_UINT32                  Options;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpIndicateEventAllOtherUsers(pRQB: 0x%X)", pRQB);

    /* opcode OHA_OPC_SET_ADDRESS or OHA_OPC_DCP_WRITE_ELEMENT */
    switch ( OHA_RQB_GET_OPCODE(pRQB))
    {
        case OHA_OPC_SET_ADDRESS:               /* fill the event-tag */
        case OHA_OPC_SET_EMERGENCY:
            Event.Event    = OHA_EVENT_LOCAL_SET;
            Event.Options  = pRQB->args.Address.Options;
            Event.Type     = 0;
            Event.SubType  = 0;
            Event.RemaOptions = pRQB->args.Address.Remanent;
            Event.Count    = 0;
            Options = Event.Options;
            for (i=0; i < 32; i++)
            {
                if (Options & 1)
                {
                    Event.Count++;
                }
                Options >>= 1;
            }
            break;
        case OHA_OPC_DCP_WRITE_ELEMENT:         /* fill the event-tag */
            Event.Event    = OHA_EVENT_LOCAL_SET;
            Event.Options  = OHA_CTRL_ANY_OTHER_OPTION;
            Event.Type     = pRQB->args.DcpElem.Type;
            Event.SubType  = pRQB->args.DcpElem.SubType;
            Event.RemaOptions = (pRQB->args.DcpElem.Remanent) ? OHA_CTRL_ANY_OTHER_OPTION : 0;
            Event.Count    = 1;
            break;
        case OHA_OPC_PRM_WRITE:
        {
            Event.Event    = OHA_EVENT_LOCAL_SET;

            switch (pRQB->args.PrmData.record_index)
            {
                case OHA_RECORD_INDEX_NAME_OF_STATION:
                case OHA_RECORD_INDEX_DCP_NAME_OF_STATION:
                    Event.Options  = OHA_CTRL_STATION_NAME;
                    Event.Type     = DCP_DBID_IDENT;
                    Event.SubType  = DCP_DSID_IDNAME;
                    Event.RemaOptions = (pRQB->args.PrmData.mk_rema) ? OHA_CTRL_STATION_NAME : (LSA_UINT16)0;
                    break;
                case OHA_RECORD_INDEX_IP_SUITE:
                case OHA_RECORD_INDEX_DCP_IP_SUITE:
                    Event.Options  = OHA_CTRL_IP_SUITE;
                    Event.Type     = DCP_DBID_TCP;
                    Event.SubType  = DCP_DSID_TCPIP;
                    Event.RemaOptions = (pRQB->args.PrmData.mk_rema) ? OHA_CTRL_IP_SUITE : (LSA_UINT16)0;
                    break;
                case OHA_RECORD_INDEX_MULTIPLE_STATION:
                {
                    LSA_UINT32 Options = 0;

                    (LSA_VOID)OHA_Get_PrmStationData(pRQB->args.PrmData.record_data,
                                                     pRQB->args.PrmData.record_data_length,
                                                     &Options,
                                                     LSA_NULL,
                                                     LSA_NULL,
                                                     LSA_NULL);
                    Event.Options  = Options;
                    Event.Type     = 0;
                    Event.SubType  = 0;
                    Event.RemaOptions = (pRQB->args.PrmData.mk_rema) ? Options : (LSA_UINT16)0;
                }
                break;
                case OHA_RECORD_INDEX_DHCP_PARAMS:
                    Event.Options  = OHA_CTRL_DHCP_OPTION;
                    Event.Type     = DCP_DBID_DHCP;
                    Event.SubType  = 61;
                    Event.RemaOptions = (pRQB->args.PrmData.mk_rema) ? OHA_CTRL_DHCP_OPTION : (LSA_UINT16)0;
                    break;
                case OHA_RECORD_INDEX_MULTIPLE_BUS:
                {
                    LSA_UINT16 CommonBusLen   = 0;
                    LSA_UINT16 AdditionBusLen = 0;

				    Event.Options  = 0;
                    (LSA_VOID)OHA_Get_PrmBUSData(pRQB->args.PrmData.record_data,
                                                 pRQB->args.PrmData.record_data_length,
												 &Event.Options,
                                                 LSA_NULL, &CommonBusLen,
                                                 LSA_NULL, &AdditionBusLen);
                    Event.Type     = 0;
                    Event.SubType  = 0;
                    Event.RemaOptions = (pRQB->args.PrmData.mk_rema) ? Event.Options : (LSA_UINT16)0;
                }
                break;
                case OHA_RECORD_INDEX_BUS_COMMON:
                    Event.Options  = OHA_CTRL_PROFIBUS_COMMON;
                    Event.Type     = DCP_DBID_PBPARAM;
                    Event.SubType  = DCP_DSID_PBCOMMON;
                    Event.RemaOptions = (pRQB->args.PrmData.mk_rema) ? OHA_CTRL_PROFIBUS_COMMON : (LSA_UINT16)0;
                    break;
                case OHA_RECORD_INDEX_BUS_ADDITION:
                    Event.Options  = OHA_CTRL_PROFIBUS_ADDITION;
                    Event.Type     = DCP_DBID_PBPARAM;
                    Event.SubType  = DCP_DSID_PBADDITION;
                    Event.RemaOptions = (pRQB->args.PrmData.mk_rema) ? OHA_CTRL_PROFIBUS_ADDITION : (LSA_UINT16)0;
                    break;
                default:
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    break;
            }

            Event.Count    = 1;
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            return;
    }

    Len = OHA_DB_READ_MAC_ADDR(NicId, &AddrPtr);
    if (Len == OHA_MAC_ADDR_SIZE)
    {
        OHA_MEMCPY(&Event.SenderMACAddr, AddrPtr, Len);
    }
    else
    {
        OHA_MEMSET(&Event.SenderMACAddr, 0, OHA_MAC_ADDR_SIZE);
    }

    /* look for other User-Channels */
	{
	    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);

		for (i = 0; i < OHA_CFG_MAX_CHANNELS; i++)
		{
			if ( OHA_IS_NOT_NULL(pHDBUser) && pHDBUser->State == OHA_HANDLE_STATE_OPEN ) /* user channel is opened ? */
			{
				if (OHA_RQB_GET_HANDLE(pRQB) != pHDBUser->ThisHandle)    /* do not confirm to the same user ? */
				{
					OHA_ASSERT_NULL_PTR(pHDBUser->Params.pDetail);

					/* AP01022057: confirm only the user with the same Nic */
					if(NicId == pHDBUser->Params.pDetail->Params.User.NICId)
					{				
						(LSA_VOID)OHA_DcpIndicateEvent(pHDBUser, &Event);
					}
				}
			}
			pHDBUser++; /* indicate the next user-channel */
		}
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpIndicateEventAllOtherUsers()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestDcpResponse                      +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_DCP_INDICATION          +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.DcpInd                                  +*/
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
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestDcpResponse(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId    = pHDBUser->Params.pDetail->Params.User.NICId;
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
    OHA_UPPER_DCP_INDICATION_PTR_TYPE indication = &pRQB->args.DcpInd;
    LSA_BOOL                Finish = LSA_TRUE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestDcpResponse(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    /* Check if we have DCP and a EDD-Channel for NIC-ID. */
    pHDBDcp  = OHA_DCPGetHandleByNIC(NICId);  /* only EDD and DCP for this request necessary */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_RESPONSE failed. No Lower Channel for this NicId (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestDcpResponse(Response: 0x%X)", Response);
        return;
    }

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL) ||
         (pHDBDcp->QueryState != OHA_DCP_STATE_QUERY_USER) ||
         (pHDBDcp->DcpState != OHA_HANDLE_STATE_OPEN) )
    {
        /* no DCP-Channel opened or no lower RQB to reprovide */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_RESPONSE failed. No DCP-Channel opened or DCP-Indication present (NIC-ID: 0x%X)",NICId);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestDcpResponse(Response: 0x%X)", Response);
        return;
    }

    /* Queue for OHA_OPC_SET_ADDRESS, OHA_OPC_DCP_RESPONSE */
    if (OHA_IS_NOT_NULL(pHDBUser->SockUserReq.pBottom))
    {
        Response = OHA_ERR_SEQUENCE;  /* already one request active */
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_RESPONSE failed. Already one User-Request (Opcode=0x%X) active for this NicId (NIC-ID: 0x%X)",OHA_RQB_GET_OPCODE(pRQB),NICId);
        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestDcpResponse(Response: 0x%X)", Response);
        return;
    }

    /* user special: a negative result is to send - check the INPUT parameter 'reason' */
    if (indication->Event == OHA_MODE_NO_SET)
    {
        LSA_UINT16 RetVal = OHA_DcpGetReason(indication->Reason);
        if (RetVal == OHA_OK)
        {
            /* response must be negative, we will take the negative default reason code OHA_DCP_REASON_DS_NOT_SET */
            indication->Reason = OHA_DCP_REASON_DS_NOT_SET;  /* we take the negative default value, not OK ! */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestDcpResponse. Negative default value for Reason code (0x%X) is set!", indication->Reason);
        }
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,
                          "OHA_RequestDcpResponse(NICId: 0x%X, indication->Event: 0x%X)", NICId, indication->Event);

    switch (indication->Event)
    {
        case OHA_MODE_NO_SET:
        case OHA_MODE_EXTERNAL_SET:
            break;

        case OHA_MODE_SET:
        case OHA_MODE_EMERGENCY_SET:
        {
            OHA_COMMON_MEM_U8_PTR_TYPE TempIpSuitePtr = LSA_NULL;

			/* AP00846402: In case of a Reset to Factory via DCP the OHA-database (NameOfStation, IP address) is not deleted */
			if ( (pHDBDcp->CheckSetInd.Options & OHA_CTRL_RESET_TO_FACTORY) ||
				 (pHDBDcp->CheckSetInd.Options & OHA_CTRL_RESET_COMMUNICATION) )
			{
				/* AP01337555, AP01348107: No further checks */
				/* now we have an asynchronous call to SOCK */
				Response = OHA_SOCKSetIpAddress(NICId, OHA_RQB_GET_HANDLE(pRQB), 
												(OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
												(OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
												(OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
												LSA_FALSE);
				if (Response == OHA_OK)
				{
					if (pHDBDcp->CheckSetInd.Options & OHA_CTRL_RESET_TO_FACTORY)
					{
						indication->Options |= OHA_CTRL_RESET_TO_FACTORY; /* ResetToFactory is (to) set */
					}
					if (pHDBDcp->CheckSetInd.Options & OHA_CTRL_RESET_COMMUNICATION)
					{
						indication->Options |= OHA_CTRL_RESET_COMMUNICATION; /* Reset Data is (to) set */
					}
					indication->CheckIpEnabled = LSA_FALSE;						/* no check of '0.0.0.0' and 'noname' necessary */
					
					Finish = LSA_FALSE;	/* queue the user-rqb, see below */
				}
				else
				{
					indication->Options = 0;		/* 'nothing set' */
				   	Response = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
					OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"Request OHA_OPC_DCP_RESPONSE(ResetToFactory) setting of IP to 0.0.0.0 failed!");

					OHA_UserRequestFinish(pHDBUser, pRQB, Response);

					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestDcpResponse(Response: 0x%X)", Response);
					return;
				}

				break;
			}

            /* if the ip address is to set, it's temporary buffered */
            (LSA_VOID) oha_db_read_temp_ip_address(pHDBDcp->Params.pDetail->Params.Dcp.NICId,
                                                   &TempIpSuitePtr, LSA_NULL, LSA_NULL);
            /* if the option IP-Suite is to set there must have been stored a temporary IpAddress in oha_dcp_ParseSet() */
            if ((pHDBDcp->CheckSetInd.Options & OHA_CTRL_IP_SUITE) &&
                OHA_IS_NOT_NULL(TempIpSuitePtr))
            {
                LSA_UINT16 RetVal;

                LSA_UINT8 AccessControl = oha_db_read_set_access(pHDBDcp->Params.pDetail->Params.Dcp.NICId,
                                                                 OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP); /* is it locked meanwhile ? */
                if (!AccessControl)  /* AP01335332 (AP01271744): for AR-Connect, lock IP-Suite */ 
                {
                    (LSA_VOID)oha_db_delete_temp_ip_address(pHDBDcp->Params.pDetail->Params.Dcp.NICId);
					break;
                }

                /* SetIp from any other user running ? */
                if (OHA_IsAnyLocalSetActive(pHDBDcp->Params.pDetail->Params.Dcp.NICId))
                {
                    Response = OHA_ERR_SEQUENCE;  /* already one request active */
                    OHA_UserRequestFinish(pHDBUser, pRQB, Response);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestDcpResponse(Response: 0x%X)", Response);
                    return;
                }

                /* the ip address is to set, queue the request and call SOCK_OPC_SET_IPADDR */
                /* call SOCK_OPC_SET_IPADDR */
                RetVal = OHA_SOCKSetIpAddress(pHDBDcp->Params.pDetail->Params.Dcp.NICId, OHA_RQB_GET_HANDLE(pRQB),
                                              TempIpSuitePtr, TempIpSuitePtr+OHA_IP_ADDR_SIZE, TempIpSuitePtr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE,
                                              (LSA_BOOL)((indication->Event == OHA_MODE_EMERGENCY_SET) ? LSA_TRUE : LSA_FALSE));
                if (RetVal == OHA_OK)
                {
                    Finish = LSA_FALSE;
                }
                else        /* local SockSetIpAddress fails */
                {
                    (LSA_VOID)oha_db_delete_temp_ip_address(pHDBDcp->Params.pDetail->Params.Dcp.NICId);
                }
            }
            else    /* iP address or NoS is (only) to check ? */
            {
#ifndef OHA_CFG_NO_NARE
                /* cancel running check requests */
                LSA_UINT32 CheckOptions = (indication->CheckIpEnabled) ? OHA_CTRL_IP_SUITE : 0;
                Finish = OHA_NARECheckAddressCancel(pHDBDcp->Params.pDetail->Params.Dcp.NICId,
                                                    indication->Options | CheckOptions, OHA_RQB_GET_HANDLE(pRQB));
#endif
            }
        }
        break;

        default:
            Response = OHA_ERR_PARAM;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_RESPONSE failed. Option (0x%X) is invalid !", indication->Options);
            OHA_UserRequestFinish(pHDBUser, pRQB, Response);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                                  "OUT: OHA_RequestDcpResponse(Response: 0x%X)", Response);
            return;
    }

    if (Finish) /* it's not a SetIpAddress.Req */
    {
        Response = OHA_DoDcpResponseRequest(pRQB, pHDBUser);
    }
    else
    {
        /* queue the user-rqb */
        OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
                              pHDBUser->SockUserReq.pTop,
                              pRQB);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestDcpResponse(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FinishDcpResponse                       +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_FinishDcpResponse(
    OHA_HDB_DCP_PTR_TYPE     pHDBDcp,
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               Response)
{
    OHA_UPPER_DCP_INDICATION_PTR_TYPE indication;

    OHA_ASSERT_NULL_PTR(pHDBDcp);
    OHA_ASSERT_NULL_PTR(pRQB);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDBDcp->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FinishDcpResponse(pRQB: 0x%X, Response: 0x%X)",
                          pRQB, Response);

    indication = &pRQB->args.DcpInd;
    OHA_ASSERT_NULL_PTR(indication);

    /* write the values of the DCP-SET to the User-RQB's and confirm */
    switch (indication->Event)
    {
        case OHA_MODE_NO_SET:
            OHA_MEMSET(indication->SenderMACAddr, 0, OHA_MAC_ADDR_SIZE);
            indication->Options    = 0;
            indication->RemaOptions = 0;
            indication->Type       = 0;
            indication->SubType    = 0;
            indication->Count      = 0;
            break;

        case OHA_MODE_SET:
        case OHA_MODE_EXTERNAL_SET:
        case OHA_MODE_EMERGENCY_SET:
            OHA_MEMCPY(indication->SenderMACAddr, &pHDBDcp->CheckSetInd.SenderMACAddr, OHA_MAC_ADDR_SIZE);
            indication->Options    = pHDBDcp->CheckSetInd.Options;
            indication->RemaOptions = pHDBDcp->CheckSetInd.RemaOptions;
            indication->Type       = pHDBDcp->CheckSetInd.Type;
            indication->SubType    = pHDBDcp->CheckSetInd.SubType;
            indication->Count      = pHDBDcp->CheckSetInd.Count;
            break;

        default:
            break;
    }

    oha_dcp_confirm_remote_set(pHDBDcp, &pHDBDcp->CheckSetInd); /* indicate the user(s) */

    pHDBDcp->QueryState = OHA_DCP_STATE_QUERY_STOP;  /* a user query is done */

    if (OHA_IS_NOT_NULL(pHDBDcp->pUpperRespRqb))
    {
        OHA_ASSERT(LSA_HOST_PTR_ARE_EQUAL(pRQB, pHDBDcp->pUpperRespRqb));
        pHDBDcp->pUpperRespRqb = LSA_NULL;
    }

    {
        LSA_HANDLE_TYPE       Handle   = OHA_RQB_GET_HANDLE(pRQB);
        OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GetUserHDBFromHandle(Handle);

        OHA_UserRequestFinish(pHDBUser, pRQB, Response);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_FinishDcpResponse(Response: 0x%X)", Response);
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoDcpResponseRequest                    +*/
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
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request for doing DCP-Indications (QuerySet)              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DoDcpResponseRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              RetVal;
    LSA_UINT16              Response;
    OHA_UPPER_DCP_INDICATION_PTR_TYPE indication = &pRQB->args.DcpInd;
    LSA_UINT16              NICId;
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp;

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pRQB);

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;

	OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoDcpResponseRequest(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    pHDBDcp = OHA_DCPGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBDcp);
    OHA_ASSERT_NULL_PTR(pHDBDcp->pLowerDcpSetRqb);

    /* get the input params of OHA_OPC_DCP_RESPONSE */
    OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_DCP_RESPONSE);

    pHDBDcp->CheckSetInd.Event        = indication->Event;
    /* entries from the user for checking the ethernet */
    pHDBDcp->CheckSetInd.CheckIpEnabled = indication->CheckIpEnabled;
    pHDBDcp->CheckSetInd.Reason       = indication->Reason;

    /* write the params to the database and respond the remote Dcp-Server  */
    RetVal = oha_dcp_Set(pHDBDcp->pLowerDcpSetRqb, pHDBDcp, &pHDBDcp->CheckSetInd);

    Response = (RetVal == LSA_RET_OK) ? (LSA_UINT16)OHA_OK : (LSA_UINT16)OHA_ERR_FAILED;

    /* finally reprovide the lower RQB to Dcp  */
    pHDBDcp->RxPendingCnt++;  /* number of Recv-Request pending within DCP */
    OHA_DCP_RQB_SET_HANDLE(pHDBDcp->pLowerDcpSetRqb, pHDBDcp->DcpHandle); /* set handle */
    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: OHA_OPC_RECV_IND (Handle: 0x%X,DCPHandle: 0x%X,pRQB: 0x%X)",pHDBDcp->ThisHandle,pHDBDcp->DcpHandle,pHDBDcp->pLowerDcpSetRqb);
    OHA_DCP_REQUEST_LOWER(pHDBDcp->pLowerDcpSetRqb, pHDBDcp->Params.pSys);
    pHDBDcp->pLowerDcpSetRqb = 0;  /* delete this LowerRqb in the handle-table now */

    /* AP00xxxxxx: Wait for DCP-Set.Con for calling oha_dcp_confirm_remote_set() */
    if (RetVal != LSA_RET_OK) /* could not respond, no resource */
    {
        OHA_FinishDcpResponse(pHDBDcp, pRQB, Response);
    }
    else    /* store the Response RQB */
    {
        OHA_ASSERT_NULL_PTR(pHDBDcp);
        pHDBDcp->pUpperRespRqb = pRQB;  /* store this UpperRqb in the handle-table now */
    }

#if 0
    /* write the values of the DCP-SET to the User-RQB's and confirm */
    switch (indication->Event)
    {
        case OHA_MODE_NO_SET:
            OHA_MEMSET(indication->SenderMACAddr, 0, OHA_MAC_ADDR_SIZE);
            indication->Options    = 0;
            indication->AdditionalOptions = 0;
            indication->Remanent   = 0;
            indication->Type       = 0;
            indication->SubType    = 0;
            indication->Count      = 0;
            break;

        case OHA_MODE_SET:
        case OHA_MODE_EXTERNAL_SET:
        case OHA_MODE_EMERGENCY_SET:
            OHA_MEMCPY(indication->SenderMACAddr, &pHDBDcp->CheckSetInd.SenderMACAddr, OHA_MAC_ADDR_SIZE);
            indication->Options    = pHDBDcp->CheckSetInd.Options;
            indication->AdditionalOptions = pHDBDcp->CheckSetInd.AdditionalOptions;
            indication->Remanent   = pHDBDcp->CheckSetInd.Remanent;
            indication->Type       = pHDBDcp->CheckSetInd.Type;
            indication->SubType    = pHDBDcp->CheckSetInd.SubType;
            indication->Count      = pHDBDcp->CheckSetInd.Count;
            break;

        default:
            break;
    }

    oha_dcp_confirm_remote_set(pHDBDcp, &pHDBDcp->CheckSetInd); /* indicate the user(s) */

    pHDBDcp->QueryState = OHA_DCP_STATE_QUERY_STOP;  /* a user query is done */

    OHA_UserRequestFinish(pHDBUser, pRQB, Response);
#endif

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DoDcpResponseRequest(Response: 0x%X)", Response);
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoDcpSetAddr                            +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE           pHDB         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : DCP HDB                                                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request for doing DCP-Indications (an OHA controlled-Set) +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DoDcpSetAddr(
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp)
{
    LSA_UINT16                  RetVal = OHA_OK;
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQBDcp;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDBDcp->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoDcpSetAddr(pHDBDcp: 0x%X)", pHDBDcp);

    pRQBDcp = pHDBDcp->pLowerDcpSetRqb;  /* get this LowerRqb, respond a DcpSet*/
    OHA_ASSERT_NULL_PTR(pRQBDcp);

    switch (pHDBDcp->CheckSetInd.Event)
    {
        case OHA_EVENT_REMOTE_SET:          /* an OHA controlled Dcp-Set */
        case OHA_MODE_EMERGENCY_SET:        /* an OHA controlled Dcp-Set for emergency set */

            /* AP00xxxxxx: Wait for DCP-Set.Con for calling oha_dcp_confirm_remote_set() */
            RetVal = oha_dcp_Set(pRQBDcp, pHDBDcp, &pHDBDcp->CheckSetInd);
            if (RetVal != LSA_RET_OK) /* could not respond, no resource */
            {
                pHDBDcp->QueryState = OHA_DCP_STATE_QUERY_STOP;   /* reset state */
            }
            break;
        default:
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"invalid DCP_SET option");
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    if ( pHDBDcp->DcpState == OHA_HANDLE_STATE_OPEN )
    {
        /* finally reprovide the lower RQB to Dcp  */
        pHDBDcp->RxPendingCnt++;  /* number of Recv-Request pending within DCP */
        OHA_DCP_RQB_SET_HANDLE(pRQBDcp, pHDBDcp->DcpHandle); /* set handle */
        OHA_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: OHA_OPC_RECV_IND (Handle: 0x%X,DCPHandle: 0x%X,pRQB: 0x%X)",pHDBDcp->ThisHandle,pHDBDcp->DcpHandle,pRQBDcp);
        OHA_DCP_REQUEST_LOWER(pRQBDcp, pHDBDcp->Params.pSys);
    }
    else
    {
        oha_dcp_free_rcv_rqb(pHDBDcp, pRQBDcp);
    }
    pHDBDcp->pLowerDcpSetRqb = 0;  /* delete this LowerRqb in the handle-table now */

    OHA_DoLocalSetAddress(pHDBDcp->Params.pDetail->Params.Dcp.NICId);   /* local set address request in any queue ? */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DoDcpSetAddr(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestDcpControl                       +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_DCP_CONTROL             +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.DcpControl                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to control the DCP-Server by the user             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestDcpControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId;
    OHA_HDB_DCP_PTR_TYPE      pHDBDcp;
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_BOOL                  Finish = LSA_TRUE; /* finish request by default */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestDcpControl(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;
    /* Check if we have DCP and a EDD-Channel for NIC-ID. */
    pHDBDcp  = OHA_DCPGetHandleByNIC(NICId);  /* only EDD and DCP for this request necessary */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* NICid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_CONTROL failed. No Lower Channel for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if ( LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL))
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_CONTROL failed. No EDD-Channel present (NIC-ID: 0x%X)",NICId);
    }
    else
    {
        OHA_UPPER_DCP_CONTROL_PTR_TYPE control = &pRQB->args.DcpControl;
        OHA_ASSERT_NULL_PTR(pHDBDcp->pDB);

        switch (control->Mode)
        {
            case OHA_QUERY_WRITE:  /* only this user controls a DCP-Set ? */
                if (pHDBDcp->pHDBUserCtrl && (!(LSA_HOST_PTR_ARE_EQUAL(pHDBDcp->pHDBUserCtrl, pHDBUser))))  /* ther's no DcpSet to control for this Dcp-Channel */
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"This Server is already controlled by another User!");
                    Response = OHA_ERR_PARAM;
                    break;
                }
                else if (!pHDBDcp->pHDBUserCtrl)    /* store the control-user */
                {
                    pHDBDcp->pHDBUserCtrl = pHDBUser;
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Control User-Set successful");
                }
                /* intentionally no break */
            case OHA_READ_ONLY:   /* write the LockStatus for DcpSet to the database */
            case OHA_READ_WRITE:  /* unlock a DcpSet to the database */
            case OHA_NO_ACCESS:   /* no Dcp-GET and no Dcp-SET */
                Response = OHA_DcpWriteAccessControl(NICId, control->Options, control->Mode);
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Control Auto-Set successful");

                /* look at the type-entry */
                if (control->Type && control->SubType)
                {
                    Response = OHA_DB_WRITE_ACCESS_CONTROL(NICId, control->Type, control->SubType, control->Mode);
                }
                break;
            case OHA_LOCK_SET:    /* this user locks a DCP-Set */
            case OHA_UNLOCK_SET:  /* this user unlocks a DCP-Set */
				if (control->Options & OHA_CTRL_IP_SUITE) /* AP01335332. Lock IpAddress ? */
				{
					/* SetIp from any user running ? */
					if (OHA_IsAnyLocalSetActive(NICId))
					{
						/* queue the user-rqb */
						OHA_RQB_PUT_BLOCK_TOP(pHDBUser->SockUserReq.pBottom,
											  pHDBUser->SockUserReq.pTop,
											  pRQB);

						OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
											  "OUT: OHA_RequestDcpControlRequest queued (Mode: 0x%X)", control->Mode);
						return;
					}
				}

                Response = OHA_DcpWriteUserControl(NICId, (LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(pHDBUser->ThisHandle)), control);
				if(OHA_IS_NOT_NULL(control->pAddrInfo))
				{
					(LSA_UINT16)OHA_WriteAddrInfo(NICId, control->pAddrInfo);
				}
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Control Auto-Set successful");
                break;

            case OHA_SET_FILTER:
            case OHA_RESET_FILTER:
                if (pHDBDcp->UserRQBCnt) /* no queueing of user-requests in this version */
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_CONTROL failed. Already one User-Request active for this NicId (NIC-ID: 0x%X)",NICId);
                    Response = OHA_ERR_SEQUENCE;
                }
                else
                {
                    Finish = LSA_FALSE;  /* further actions */
                }
                break;
			case OHA_SHOW_LOCATION:	/* LED flashing */
				Response = OHA_EDDControlSignal(NICId, DCP_FLASH_ONCE );
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Control Show-Location");
				break;
            case OHA_WRITE_ONLY:  /* the user controls a DCP-Get */
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Control OHA_NO_ACCESS or OHA_WRITE_ONLY not implemented");
                Response = OHA_ERR_NOT_IMPLEMENTED;
                break;
            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Control failed.[0x%X]",OHA_DCP_RQB_GET_RESPONSE(pRQB));
                Response = OHA_ERR_SYNTAX;
                break;
        }
    }

    if ( Finish )  /* finish the request ? */
    {
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestDcpControl(Request finished)");
    }
    else
    {
        OHA_ASSERT_NULL_PTR(pHDBDcp);
        /* queue the request in the DCP-HDB */
        OHA_RQB_PUT_BLOCK_TOP(pHDBDcp->UserRQBs.pBottom,
                              pHDBDcp->UserRQBs.pTop,
                              pRQB);
        pHDBDcp->UserRQBCnt++;
        OHA_DCPRequestHandler(pHDBDcp, pRQB, OHA_OK);  /* call the dcp-handler */
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestDcpControl(Request queued)");
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPGetHandleByNIC                       +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_DCP_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a DCP HDB with NICId. If none is found    +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

OHA_HDB_DCP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_DCPGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_DCP_PTR_TYPE  pHDB  =   OHA_GET_HDCP_PTR(0);
    LSA_UINT32            i     =   0;
    LSA_BOOL              Found =   LSA_FALSE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPGetHandleByNIC(NICId: 0x%X)",
                          NICId);

    while ( (! Found ) && ( i < OHA_CFG_MAX_NICS ) )
    {
        if (( pHDB->DcpState == OHA_HANDLE_STATE_OPEN || pHDB->DcpState == OHA_HANDLE_STATE_OPEN_DCP_RUNNING) &&
            ( pHDB->Params.pDetail->Params.Dcp.NICId == NICId  ))
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

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPGetHandleByNIC(pHDB:  0x%X)", pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPCheckNICUsed                         +*/
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
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_DCPCheckNICUsed(
    LSA_UINT16 NICId)
{
    OHA_HDB_DCP_PTR_TYPE  pHDB  =   OHA_GET_HDCP_PTR(0);
    LSA_UINT32            i     =   0;
    LSA_BOOL              Found =   LSA_FALSE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPCheckNICUsed(NICId: 0x%X)",
                          NICId);
    if (NICId != OHA_INVALID_NIC_ID)
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS ) )
        {
            if (( pHDB->DcpState != OHA_HANDLE_STATE_FREE ) &&
                ( pHDB->Params.pDetail->Params.Dcp.NICId == NICId  ))
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

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPCheckNICUsed(Found:  0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DCPUserCheckPending                     +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE       pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP-HDB                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPUserCheckPending(
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp,
    LSA_UINT16              Response)
{
    OHA_HDB_USER_PTR_TYPE     pHDBUser;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DCPUserCheckPending(pHDBDcp: 0x%X, Response: 0x%X)",
                          pHDBDcp, Response);

    if ( pHDBDcp->UserRQBCnt ) /* User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

	    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDBDcp->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,">>>: DCPUserCheckPending (NICId: 0x%X,UserRQBCnt: 0x%X)",
												pHDBDcp->Params.pDetail->Params.Dcp.NICId,pHDBDcp->UserRQBCnt);

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBDcp->UserRQBs.pBottom,
                                 pHDBDcp->UserRQBs.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
        pHDBDcp->UserRQBCnt--;

        /* control = &pRQBUser->args.DcpControl; */
        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);
        OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPUserCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpRequestHandler                       +*/
/*+                        :    OHA_HDB_DCP_PTR_TYPE       pHDB             +*/
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
/*+  Description: Statemaching for DCP-User-Requests.                       +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPRequestHandler(
    OHA_HDB_DCP_PTR_TYPE            pHDBDcp,
    OHA_UPPER_RQB_PTR_TYPE          pRQBUser,
    LSA_UINT16                      Response)
{
    LSA_BOOL                  Ready = LSA_FALSE;
    LSA_UINT16                NICId = pHDBDcp->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpRequestHandler(pHDBDcp: 0x%X, pRQBUser: 0x%X, Response: 0x%X)",
                          pHDBDcp, pRQBUser, Response);

    /* -----------------------------------------------------------------------*/
    /* forward in statemachine till an asynchroneous request is underway which*/
    /* we have to wait for.                                                   */
    /* -----------------------------------------------------------------------*/
    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,
						"<<<: OHA_DCPRequestHandler (NICId: 0x%X,Rsp: 0x%X,RequestState: 0x%X)",NICId,Response,pHDBDcp->RequestState);
    while (! Ready)
    {
        Ready = LSA_TRUE;      /* Default: break loop because request underway */

        switch (pHDBDcp->RequestState)
        {
            case OHA_DCP_STATE_FILTER_STOP:       /* no filtering of frames     */
                OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,"DCP-SM [%X]: STATE_STOPPED (Resp: %Xh)",pHDBDcp->ThisHandle,Response);

                /* comes from the user ? */
                /* stopped state and a User-RESET_FILTER => no action */
                if (pRQBUser && (pRQBUser->args.DcpControl.Mode == OHA_RESET_FILTER))
                {
                    /* check for pending user-requests and confirm  */
                    OHA_DCPUserCheckPending(pHDBDcp, OHA_OK);
                    break;
                }
                /* User-SetFilter => starts with OHA_MODE_SET_FILTER */
                if (pRQBUser && (pRQBUser->args.DcpControl.Mode == OHA_SET_FILTER))
                {
                    LSA_UINT16 RetVal = OHA_EDD_SetFilter(NICId, pRQBUser->args.DcpControl.Mode, OHA_HANDLE_TYPE_USER);  /* action is SET_FILTER */
                    if (RetVal != OHA_OK)
                    {
                        /* check for pending user-requests and confirm  */
                        OHA_DCPUserCheckPending(pHDBDcp, RetVal);
                    }
                    else
                    {
                        pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_WF_START;
                    }
                    break;
                }
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            case OHA_DCP_STATE_FILTER_WF_START:   /* SET_FILTER.Req is active    */
                OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,"DCP-SM [%X]: STATE_WF_START (Resp: %Xh)",pHDBDcp->ThisHandle,Response);

                /* is there an error during DCP_START ? */
                if (Response != OHA_OK)
                {
                    pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_STOP;
                }
                else
                {
                    pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_START;
                }
                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_DCPUserCheckPending(pHDBDcp, Response);
                break;

            case OHA_DCP_STATE_FILTER_WF_STOP:     /* (RE)SET_FILTER.Req is active   */
                OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,"DCP-SM [%X]: STATE_WF_STOP (Resp: %Xh)",pHDBDcp->ThisHandle,Response);

                /* is there an error during DCP_STOP ? */
                if (Response != OHA_OK)
                {
                    pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_START;
                }
                else
                {
                    pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_STOP;
                }
                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_DCPUserCheckPending(pHDBDcp, Response);
                break;

            case OHA_DCP_STATE_FILTER_START:      /* frame-filter is active     */
                OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,"DCP-SM [%X]: STATE_START (Resp: %Xh)",pHDBDcp->ThisHandle,Response);

                if (pRQBUser && (pRQBUser->args.DcpControl.Mode == OHA_RESET_FILTER))
                {
                    LSA_UINT16 RetVal = OHA_EDD_SetFilter(NICId, pRQBUser->args.DcpControl.Mode, OHA_HANDLE_TYPE_USER);
                    if (RetVal != OHA_OK)
                    {                           /* confirm to the user, if exist */
                        /* check for pending user-requests   */
                        OHA_DCPUserCheckPending(pHDBDcp, Response);
                    }
                    else
                    {
                        pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_WF_STOP;
                    }
                    break;
                }
                /* started state and a User-START => update the frame filter */
                if (pRQBUser && (pRQBUser->args.DcpControl.Mode == OHA_SET_FILTER))
                {
                    LSA_UINT16 RetVal = OHA_EDD_SetFilter(NICId, pRQBUser->args.DcpControl.Mode, OHA_HANDLE_TYPE_USER);
                    if (RetVal != OHA_OK)
                    {                           /* confirm to the user, if exist */
                        /* check for pending user-requests   */
                        OHA_DCPUserCheckPending(pHDBDcp, Response);
                    }
                    else
                    {
                        pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_WF_START;
                    }
                    break;
                }
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            default:
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_DcpRequestHandler(): Invalid RState (%Xh)",pHDBDcp->RequestState);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DcpRequestHandler(), state %Xh", pHDBDcp->RequestState);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpIndicateEventExtUsers                +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE              pHDB      +*/
/*+                             OHA_UPPER_DCP_INDICATION_PTR_TYPE pEvent    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pEvent               : Pointer to eventcb                              +*/
/*+                                                                         +*/
/*+  Result               :                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DCP-Set-Event to all users,                   +*/
/*+               but not to a control-user.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpIndicateEventExtUsers(
    LSA_UINT16                          NicId,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE   pEvent
)
{
    OHA_HDB_DCP_PTR_TYPE     pHDB;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpIndicateEventExtUsers(NicId: 0x%X, pEvent: 0x%X)",
                          NicId, pEvent);

    /* get the DCP-Channel for the NIC-ID */
    pHDB = OHA_DCPGetHandleByNIC(NicId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDB,  LSA_NULL))
    {
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_DcpIndicateEventExtUsers failed. No DCP-Channel present (NIC-ID: 0x%X)",NicId);
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DcpIndicateEventExtUsers failed.");
        return;
    }

    OHA_DcpIndicateEventAllUsers(pHDB, pEvent);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpIndicateEventExtUsers()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpHelloEvent                           +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+                             OHA_UPPER_DCP_HELLOPTR_TYPE  pEvent         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pEvent               : Pointer to EventTag                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DCP-Hello-Event to one user                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpHelloEvent(
    OHA_HDB_USER_PTR_TYPE           pHDBUser,
    OHA_UPPER_HELLO_PTR_TYPE        pEvent
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  Response = OHA_OK;

    OHA_ASSERT_NULL_PTR(pHDBUser);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDBUser->Params.pDetail->Params.User.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpHelloEvent(pHDBUser: 0x%X, pEvent: 0x%X)",
                          pHDBUser, pEvent);

    OHA_ASSERT_NULL_PTR(pEvent);

    /* get hello indication rqb */
    OHA_RQB_REM_BLOCK_BOTTOM(
        pHDBUser->HelloIndReq.pBottom,
        pHDBUser->HelloIndReq.pTop,
        pRQB);

    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* user-IndRqb's exists ? */
    {
        pRQB->args.HelloInd.FilterID = pEvent->FilterID;

        OHA_MEMCPY(&pRQB->args.HelloInd.MacAddr, pEvent->MacAddr, OHA_MAC_ADDR_SIZE);
        OHA_MEMCPY(&pRQB->args.HelloInd.IPAddr, pEvent->IPAddr, OHA_IP_ADDR_SIZE);
        OHA_MEMCPY(&pRQB->args.HelloInd.SubnetMask, pEvent->SubnetMask, OHA_IP_ADDR_SIZE);
        OHA_MEMCPY(&pRQB->args.HelloInd.DefaultRouterAddr, pEvent->DefaultRouterAddr, OHA_IP_ADDR_SIZE);

        if (OHA_IS_NOT_NULL(pEvent->pStationName) && pEvent->StationNameLen)
        {
            if (OHA_IS_NOT_NULL(pRQB->args.HelloInd.pStationName))
            {
                if (pEvent->StationNameLen <= pRQB->args.HelloInd.StationNameLen)
                {
                    OHA_MEMCPY(pRQB->args.HelloInd.pStationName, pEvent->pStationName, pEvent->StationNameLen);
                }
                else
                {
                    OHA_MEMCPY(pRQB->args.HelloInd.pStationName, pEvent->pStationName, pRQB->args.HelloInd.StationNameLen);
                    Response = OHA_ERR_BUFFER_OVERFLOW;
                }
            }
        }

        pRQB->args.HelloInd.StationNameLen = pEvent->StationNameLen;

        pHDBUser->HelloIndReqPending--;

        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
    }
    else
    {
        Response = LSA_RET_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpHelloEvent(Response: 0x%X)", Response);
    return (Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpHelloEventAllUsers                   +*/
/*+  Input/Output          :    OHA_HDB_DCP_PTR_TYPE              pHDB      +*/
/*+                             OHA_UPPER_DCP_INDICATION_PTR_TYPE pEvent    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pEvent               : Pointer to eventcb                              +*/
/*+                                                                         +*/
/*+  Result               :                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DCP-Set-Event to all users,                   +*/
/*+               but not to a control-user.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpHelloEventAllUsers(
    OHA_UPPER_HELLO_PTR_TYPE        pEvent,
	LSA_UINT16                      NicId
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpHelloEventAllUsers(NicId: 0x%X, pEvent: 0x%X)",
                          NicId, pEvent);

    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        if ( pHDBUser && pHDBUser->State == OHA_HANDLE_STATE_OPEN ) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NicId == pHDBUser->Params.pDetail->Params.User.NICId)
			{				
	            (LSA_VOID)OHA_DcpHelloEvent(pHDBUser, pEvent);
			}
        }
        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpHelloEventAllUsers()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpIndicateCheckIpEvent                 +*/
/*+  Input/Output          :    LSA_UINT16                        NicId     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NicId                : NICId from SystemPath                           +*/
/*+                                                                         +*/
/*+  Result               :                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a local DCP-Set(Ip)-Event to all users.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DcpIndicateCheckIpEvent(
	LSA_UINT16                  NicId
)
{
    OHA_HDB_DCP_PTR_TYPE        pHDB;
    OHA_COMMON_MEM_U8_PTR_TYPE  AddrPtr = LSA_NULL;
	LSA_BOOL					IsRema;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpIndicateCheckIpEvent(NicId: 0x%X)", NicId);

    /* get the DCP-Channel for the NIC-ID */
    pHDB = OHA_DCPGetHandleByNIC(NicId);

	if(OHA_IS_NULL(pHDB))
	{
	    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
	                          "OUT: OHA_DcpIndicateCheckIpEvent()");
	    return;
	}
	
	IsRema = oha_db_compare_remanence(NicId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT);

    pHDB->LastSet.Event    = OHA_EVENT_LOCAL_SET;

    pHDB->LastSet.Options  = OHA_CTRL_IP_SUITE;
    pHDB->LastSet.RemaOptions = IsRema ? OHA_CTRL_IP_SUITE : (LSA_UINT16)0;
    pHDB->LastSet.Type     = DCP_DBID_TCP;
    pHDB->LastSet.SubType  = DCP_DSID_TCPIP;
    pHDB->LastSet.Count    = 1;

    if (DCP_DS_MACADDR_SIZE == OHA_DB_READ_MAC_ADDR(NicId, &AddrPtr) &&
        OHA_IS_NOT_NULL(AddrPtr))
    {
        OHA_MEMCPY(&pHDB->LastSet.SenderMACAddr, AddrPtr, OHA_MAC_ADDR_SIZE);
    }
    else
    {
        OHA_MEMSET(&pHDB->LastSet.SenderMACAddr, 0, OHA_MAC_ADDR_SIZE);
    }

	/* we've to indicate a query user too, it's a local (re)set IP */
	{
		LSA_UINT16                  i        = 0;
		OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);

		while ( i < OHA_CFG_MAX_CHANNELS)
		{
			if ( pHDBUser && pHDBUser->State == OHA_HANDLE_STATE_OPEN ) /* user channel is opened ? */
			{
				/* AP01022057: confirm only the user with the same Nic */
				if(NicId == pHDBUser->Params.pDetail->Params.User.NICId)
				{
	                (LSA_VOID)OHA_DcpIndicateEvent(pHDBUser, &pHDB->LastSet);
				}
            }
			i++;       /* indicate the next user-channel */
			pHDBUser++;
		}
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpIndicateCheckIpEvent()");
    return;
}

/*****************************************************************************/
/*  end of file OHA_DCPU.C                                                   */
/*****************************************************************************/

