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
/*  F i l e               &F: oha_db.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA USER-database interface functions            */
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

#define LTRC_ACT_MODUL_ID  12
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_DB */

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

OHA_DB_TYPE oha_database;        /* common database */

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
/*+  Functionname          :    OHA_RequestGetConfig                        +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_GET_CONFIG             +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.GetConfig                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to get config parameters from OHA                 +*/
/*+                                                                         +*/
/*+               Fills RQB with config data and confirms request.          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestGetConfig(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16            Response;
    LSA_UINT16            NICId = pHDB->Params.pDetail->Params.User.NICId;
	OHA_DB_IF_PTR_TYPE    pDB   = OHA_DB_GET_INTERFACE(NICId);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestGetConfig(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    Response = OHA_OK;

    if (OHA_IS_NOT_NULL(pDB) && (pDB->NicId != OHA_INVALID_NIC_ID))
    {
		pRQB->args.Config.InterfaceID = pDB->InterfaceID;
		pRQB->args.Config.NicId       = pDB->NicId;
		pRQB->args.Config.PortCnt     = pDB->PortCnt;
        pRQB->args.Config.MaxSender = pDB->PortMaxSender;
    }
    else
    {
        pRQB->args.Config.PortCnt   = 0;
        pRQB->args.Config.InterfaceID = 0;
        pRQB->args.Config.NicId     = OHA_INVALID_NIC_ID;  /* invalid */
        pRQB->args.Config.MaxSender = 0;
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestGetConfig(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestGetAddress                       +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_GET_ADDRESS            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.DbControl                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read parameters from the database              +*/
/*+                                                                         +*/
/*+               Fills RQB with station data and confirms request.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestGetAddress(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId;
    OHA_UPPER_ADDRESS_PTR_TYPE address;
    OHA_DB_IF_PTR_TYPE        pDB;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestGetAddress(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB->Params.pDetail);

    address = &pRQB->args.Address;
    NICId = pHDB->Params.pDetail->Params.User.NICId;
	pDB = OHA_DB_GET_INTERFACE(NICId);

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId); /* only EDD channel for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_GET_ADDRESS failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else if ( (!(address->Options)) || (OHA_IS_NULL(address->pStationData)) )
    {
        Response = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
        /* Error on getting Options. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_GET_ADDRESS failed. No Parameter to write or read (Options: 0x%X)",pRQB->args.Address.Options);
    }
    else if (!(pHDB->State == OHA_HANDLE_STATE_OPEN &&
               LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) ) )
    {           /* is no open/close user-channel runnning ? */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_GET_ADDRESS failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else
    {
        OHA_UPPER_STATION_PTR_TYPE pStation = address->pStationData;
        OHA_COMMON_MEM_U8_PTR_TYPE pName   = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pSubnet = LSA_NULL;
        OHA_COMMON_MEM_U8_PTR_TYPE pRouter = LSA_NULL;
        LSA_INT  len;

        /* LSA_UINT16   SetStatus = address->Options;  */ /* save the Controls */

        address->Options     = 0;
        address->Remanent    = 0;    /* irrelevant */

        len = OHA_DB_READ_MAC_ADDR(NICId, &pName);
        if ((len == OHA_MAC_ADDR_SIZE) && OHA_IS_NOT_NULL(pName))
        {
            address->Options |= OHA_CTRL_MAC_ADDR;
            OHA_MEMCPY(&pStation->LocalMacAddr, pName, len);
        }
        /* IP-Suite */
        len = OHA_DB_READ_IP_SUITE(NICId, &pName, &pSubnet, &pRouter);
        if ((len == OHA_IP_ADDR_SIZE) && OHA_IS_NOT_NULL(pName))
        {
            address->Options |= OHA_CTRL_IP_SUITE;
            OHA_MEMCPY(&pStation->LocalIpAddr, pName, OHA_IP_ADDR_SIZE);
            OHA_MEMCPY(&pStation->LocalSubnetMask, pSubnet, OHA_IP_ADDR_SIZE);
            OHA_MEMCPY(&pStation->DefaultRouterAddr, pRouter, OHA_IP_ADDR_SIZE);

        }

        if (pStation->pStationType && pStation->StationTypeLen <= OHA_MAX_STATION_TYPE_LEN)
        {
            len = OHA_DB_READ_STATION_TYPE(NICId, &pName);

            if (len && OHA_IS_NOT_NULL(pName))
            {
                address->Options |= OHA_CTRL_STATION_TYPE;
                OHA_MEMCPY(pStation->pStationType, pName, len);
                pStation->StationTypeLen = (LSA_UINT16)len;
            }
            else
            {
                pStation->StationTypeLen = 0;
            }
        }
        else
        {
            pStation->StationTypeLen = 0;
        }
        if (OHA_IS_NOT_NULL(pStation->pStationName) && pStation->StationNameLen <= OHA_MAX_STATION_NAME_LEN)
        {
            len = OHA_DB_READ_STATION_NAME(NICId, &pName);
            if (len && pName)
            {
                address->Options |= OHA_CTRL_STATION_NAME;
                OHA_MEMCPY(pStation->pStationName, pName, len);
                pStation->StationNameLen = (LSA_UINT16)len;
            }
            else
            {
                pStation->StationNameLen = 0;
            }
        }
        else
        {
            pStation->StationNameLen = 0;
        }

		if(OHA_IS_NOT_NULL(pDB) &&
		   pDB->PNIdentValid.Existing) /* record written ? */
		{
            address->Options  |= OHA_CTRL_STATION_ID;
            pStation->VendorId = pDB->PNIdentValid.VendorID;
            pStation->DeviceId = pDB->PNIdentValid.DeviceID;
		}
		else
		{
        len = OHA_DB_READ_STATION_ID(NICId, &pName);
        if ((len == (2*sizeof(LSA_UINT16))) && pName)
        {
            address->Options  |= OHA_CTRL_STATION_ID;
            pStation->VendorId = OHA_GET16_NTOH(pName, 0);
            pStation->DeviceId = OHA_GET16_NTOH(pName, sizeof(LSA_UINT16));
        }
        else
        {
            pStation->VendorId = 0;
            pStation->DeviceId = 0;
        }
		}

        {
            pStation->ProfinetRole = OHA_DB_READ_PROFINET_ROLE(NICId);
            if (pStation->ProfinetRole)
            {
                address->Options |= OHA_CTRL_PNIO_ROLE;
            }
        }
        {
            pStation->TcpIpStatus = OHA_DB_READ_TCPIP_STATUS(NICId);
            if (pStation->TcpIpStatus)
            {
                address->Options |= OHA_CTRL_TCIP_STATUS;
            }
        }

		if(OHA_IS_NOT_NULL(pDB) &&
		   pDB->PNIdentValid.Existing) /* record written ? */
		{
            address->Options  |= OHA_CTRL_DEVICE_INSTANCE;
            pStation->DeviceInstance = pDB->PNIdentValid.InstanceID;
		}
		else
		{
        len = OHA_DB_READ_DEVICE_INSTANCE(NICId, &pName);
        if ((len == (sizeof(LSA_UINT16))) && pName)
        {
            address->Options  |= OHA_CTRL_DEVICE_INSTANCE;
            pStation->DeviceInstance = OHA_GET16_NTOH(pName, 0);
        }
        else
        {
            pStation->DeviceInstance = 0;
        }
		}
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestGetAddress(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestSetAddress                       +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_SET_ADDRESS            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.DbControl                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write parameters to the database               +*/
/*+               If IP-Address is to set, SOCK_OPC_SET_IPADDR is called    +*/
/*+               Writes station data to database and confirms request.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestSetAddress(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp;
    LSA_UINT16              Response;
    LSA_UINT16              NICId;
    OHA_UPPER_ADDRESS_PTR_TYPE pAddress;
    OHA_UPPER_STATION_PTR_TYPE pStation;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestSetAddress(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->Params.pDetail);

    pAddress = &pRQB->args.Address;
    OHA_ASSERT_NULL_PTR(pAddress);
    pStation = pAddress->pStationData;
    NICId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId); /* only EDD channel for this request necessary */
    pHDBDcp  = OHA_DCPGetHandleByNIC(NICId); /* DCP channel necessary in case of a Query    */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) ||
         LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL ) )
    {
        Response = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. No Lower Channel for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if (!(pHDB->State == OHA_HANDLE_STATE_OPEN &&
               LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) ) )
    {           /* is no open/close user-channel runnning ? */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if ( (!(pAddress->Options)) || (OHA_IS_NULL(pAddress->pStationData)) )
    {
        pAddress->Options = 0;      /* 'nothing set' */
        Response = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
        /* Error on getting Options. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. No Parameter to write or read (Options: 0x%X)",pRQB->args.Address.Options);
    }
    else if (!(pAddress->AccessMode == OHA_READ_ONLY ||
               pAddress->AccessMode == OHA_READ_WRITE ||
               pAddress->AccessMode == OHA_QUERY_WRITE))
    {
        Response = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
        /* Error on getting AccessMode. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. Invalid AccessMode (0x%X)", pRQB->args.Address.AccessMode);
    }
    else
    {
        LSA_UINT32 ErrorOptions;

#if 0
        static int i = 0;

        if (i == 1)
        {
            OHA_HDB_SOCK_PTR_TYPE pHDBSock = OHA_SOCKGetHandleByNIC(pAddress->NicId);
            if (pHDBSock->SetIpRQBCnt == 0)
            {
                Response = OHA_ERR_SEQUENCE;  /* ParameterStatus is wrong */
                OHA_UserRequestFinish(pHDB,pRQB,Response);
                /* Error on getting AccessMode. Abort. */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. Invalid AccessMode (0x%X)", pRQB->args.Address.AccessMode);
                return;
            }
        }
        i++;
#endif

        ErrorOptions = 0;				/* init the Options with 'no error' */
        pAddress->UserLockStatus = 0;	/* init the UserLockStatus with 'nothing locked' */

		/*******************************************/
		/* check all options for OHA_ERR_PARAM     */
		/*******************************************/
        if (pAddress->Options & OHA_CTRL_MAC_ADDR)
        {
            ErrorOptions |= OHA_CTRL_MAC_ADDR; /* MacAddr is not allowed to set */
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. Set MacAddress in not supported!");
        }

        if (pAddress->Options & OHA_CTRL_RESET_TO_FACTORY)
        {
            /* Rtf: The NameOfStation and/or the IP-Address is to delete => no OHA_CTRL_IP_SUITE */
            /* or OHA_CTRL_STATION_NAME is allowed. */
            if (pAddress->Options & OHA_CTRL_STATION_NAME)
            {
                ErrorOptions |= OHA_CTRL_RESET_TO_FACTORY;
                ErrorOptions |= OHA_CTRL_STATION_NAME;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. ResetToFactory allows no additional setting of NoS!");
            }
            if (pAddress->Options & OHA_CTRL_IP_SUITE)
            {
                ErrorOptions |= OHA_CTRL_RESET_TO_FACTORY;
                ErrorOptions |= OHA_CTRL_IP_SUITE;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. ResetToFactory allows no additional setting of IP!");
            }
		}

        if (pAddress->Options & OHA_CTRL_RESET_COMMUNICATION)
        {
            /* Rtf: The NameOfStation and/or the IP-Address is to delete => no OHA_CTRL_IP_SUITE */
            /* or OHA_CTRL_STATION_NAME is allowed. */
            if (pAddress->Options & OHA_CTRL_STATION_NAME)
            {
                ErrorOptions |= OHA_CTRL_RESET_COMMUNICATION;
                ErrorOptions |= OHA_CTRL_STATION_NAME;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. ResetCommunication allows no additional setting of NoS!");
            }
            if (pAddress->Options & OHA_CTRL_IP_SUITE)
            {
                ErrorOptions |= OHA_CTRL_RESET_COMMUNICATION;
                ErrorOptions |= OHA_CTRL_IP_SUITE;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. ResetCommunication allows no additional setting of IP!");
            }
		}

        if (pAddress->Options & OHA_CTRL_IP_SUITE)
        {
            /* we check the IP-address before set */
            if (!oha_is_valid_ip_address(OHA_GET32_NTOH(&pAddress->pStationData->LocalIpAddr, 0),
                                         OHA_GET32_NTOH(&pAddress->pStationData->LocalSubnetMask, 0),
                                         OHA_GET32_NTOH(&pAddress->pStationData->DefaultRouterAddr, 0),
                                         LSA_FALSE)) /* 0.0.0.0 is allowed by a local set */
            {
                /* Error on getting IpAddress. Abort. */
                ErrorOptions |= OHA_CTRL_IP_SUITE;
                OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_ADDRESS failed. Invalid IP-Suite (IP: 0x%X, SubNet: 0x%X, GateWay: 0x%X)",
                                     OHA_GET32_NTOH(&pAddress->pStationData->LocalIpAddr,0),
                                     OHA_GET32_NTOH(&pAddress->pStationData->LocalSubnetMask,0),
                                     OHA_GET32_NTOH(&pAddress->pStationData->DefaultRouterAddr,0));
            }
        }

        if (pAddress->Options & OHA_CTRL_STATION_NAME)
        {
			if(pStation->StationNameLen > OHA_MAX_STATION_NAME_LEN)
			{
	            ErrorOptions |= OHA_CTRL_STATION_NAME;
			}
			else if(pStation->StationNameLen != 0)
			{
				if (!oha_is_valid_station_name(pStation->StationNameLen, 
												(OHA_COMMON_MEM_U8_PTR_TYPE)(pStation->pStationName), 
												OHA_UPPER_CASES_ALLOWED(NICId), 
												OHA_UPPER_CASES_ALLOWED(NICId)))
				{
		            ErrorOptions |= OHA_CTRL_STATION_NAME;
				}
			}
		}
		if (pAddress->Options & OHA_CTRL_STATION_TYPE)
		{
		    if (!(pStation->StationTypeLen == 0 || 
				(OHA_IS_NOT_NULL(pStation->pStationType) && (pStation->StationTypeLen <= OHA_MAX_STATION_TYPE_LEN))))
			{
                ErrorOptions |= OHA_CTRL_STATION_TYPE;
			}
		}

		 /* 936497: initial value must be set via OHA_DB_WRITE_STATION_ID(), a runtime value is set via PDIdentificationRecord */
		if (pAddress->Options & OHA_CTRL_STATION_ID)
		{
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"The initial value of VendorID/DeviceID must be set via OHA_DB_WRITE_STATION_ID()!");
                ErrorOptions |= OHA_CTRL_STATION_ID;
		}
		 /* 936497: initial value must be set via OHA_DB_WRITE_DEVICE_INSTANCE(), a runtime value is set via PDIdentificationRecord */
		if (pAddress->Options & OHA_CTRL_DEVICE_INSTANCE)
		{
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"The initial value of InstanceID must be set via OHA_DB_WRITE_DEVICE_INSTANCE()!");
                ErrorOptions |= OHA_CTRL_DEVICE_INSTANCE;
		}

		if(ErrorOptions) /* 'error' options for param error existing ? */
		{
            Response = OHA_ERR_PARAM;
			pAddress->Options = ErrorOptions; 
            OHA_UserRequestFinish(pHDB,pRQB,Response);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
            return;
		}

		/*******************************************/
		/* check all options for OHA_ERR_SEQUENCE  */
		/*******************************************/
        if (pAddress->Options & OHA_CTRL_RESET_TO_FACTORY)
        {
            /* SetIp from any other user running ? */
            if (OHA_IsAnyLocalSetActive(NICId))
            {
                ErrorOptions |= OHA_CTRL_RESET_TO_FACTORY;
            }
		}

        if (pAddress->Options & OHA_CTRL_RESET_COMMUNICATION)
        {
            /* SetIp from any other user running ? */
            if (OHA_IsAnyLocalSetActive(NICId))
            {
                ErrorOptions |= OHA_CTRL_RESET_COMMUNICATION;
            }
		}

		/* AP01294301: problem with parallel OHA_OPC_SET_ADDRESS.Req */
        if ((pAddress->Options & OHA_CTRL_IP_SUITE) || (pAddress->Options & OHA_CTRL_STATION_NAME))
        {
            /* SetIp from any other user running ? */
            if (OHA_IsAnyLocalSetActive(NICId))
            {
                ErrorOptions |= OHA_CTRL_IP_SUITE;
            }
		}

		if(ErrorOptions) /* 'error' options for sequence error existing ? */
		{
            Response = OHA_ERR_SEQUENCE;  /* already one request active */
			pAddress->Options = ErrorOptions; 
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"No queueing, Request (Opcode: 0x%X) is already active", OHA_RQB_GET_OPCODE(pHDB->SockUserReq.pBottom));

            OHA_UserRequestFinish(pHDB,pRQB,Response);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
            return;
		}

		/*******************************************/
		/* check all options for OHA_ERR_USER_LOCK */
		/*******************************************/
        if (pAddress->Options & OHA_CTRL_MAC_ADDR)    /* if the user want to set */
        {
            if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, OHA_DCP_OPTION_TCP, DCP_DSID_MACADR))  /* locked by any user ? */
            {
                pAddress->UserLockStatus |= OHA_CTRL_MAC_ADDR;
            }
		}
        if (pAddress->Options & OHA_CTRL_IP_SUITE) /* IpSuite is to set */
        {
            if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP))  /* locked by any user ? */
            {
                pAddress->UserLockStatus |= OHA_CTRL_IP_SUITE;
            }
        }
        if (pAddress->Options & OHA_CTRL_STATION_TYPE) /* StationType is to set */
        {
            if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDTYPE))  /* locked by any user ? */
            {
                pAddress->UserLockStatus |= OHA_CTRL_STATION_TYPE;
            }
        }
        if (pAddress->Options & OHA_CTRL_STATION_NAME) /* StationName is to set */
        {
            if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDNAME))  /* locked by any user ? */
            {
                pAddress->UserLockStatus |= OHA_CTRL_STATION_NAME;
            }
        }
        if (pAddress->Options & OHA_CTRL_STATION_ID) /* StationId is to set */
        {
            if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDID))  /* locked by any user ? */
            {
                pAddress->UserLockStatus |= OHA_CTRL_STATION_ID;
            }
        }
        if (pAddress->Options & OHA_CTRL_PNIO_ROLE) /* PnioRole is to set */
        {
            if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDROLE))  /* locked by any user ? */
            {
                pAddress->UserLockStatus |= OHA_CTRL_PNIO_ROLE;
            }
        }
        if (pAddress->Options & OHA_CTRL_DEVICE_INSTANCE) /* DeviceInstance is to set */
        {
            if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, OHA_DCP_OPTION_IDENT, DCP_DSID_IDDEVINST))  /* locked by any user ? */
            {
                pAddress->UserLockStatus |= OHA_CTRL_DEVICE_INSTANCE;
            }
        }

		/* special response (OHA_ERR_USER_LOCK) e.g. if an AR is established (AP00907729) */
		if (pAddress->UserLockStatus)
		{
			pAddress->Options = pAddress->UserLockStatus; /* error option */

			Response = OHA_ERR_USER_LOCK;
            OHA_UserRequestFinish(pHDB,pRQB,Response);

            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
            return;
		}

        if ( (pAddress->Options & OHA_CTRL_RESET_TO_FACTORY) ||
			 (pAddress->Options & OHA_CTRL_RESET_COMMUNICATION) )
        {
            /* now we have an asynchronous call to SOCK */
            Response = OHA_SOCKSetIpAddress(NICId, OHA_RQB_GET_HANDLE(pRQB),
                                            (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
                                            (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
                                            (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP,
                                            LSA_FALSE);
            if (Response == OHA_OK)
            {

                pAddress->CheckIpEnabled = LSA_FALSE;        /* no check of '0.0.0.0' necessary */
                /* queue the user-rqb */
                OHA_RQB_PUT_BLOCK_TOP(pHDB->SockUserReq.pBottom,
                                      pHDB->SockUserReq.pTop,
                                      pRQB);

                Response = OHA_OK_ACTIVE;
            }
			else
			{
                if (pAddress->Options & OHA_CTRL_RESET_TO_FACTORY)
				{
                    ErrorOptions |= OHA_CTRL_RESET_TO_FACTORY; /* ResetToFactory failed */
				}
                if (pAddress->Options & OHA_CTRL_RESET_COMMUNICATION)
				{
                    ErrorOptions |= OHA_CTRL_RESET_COMMUNICATION; /* Reset Data failed */
				}
				pAddress->Options = ErrorOptions; 
				OHA_UserRequestFinish(pHDB,pRQB,Response);
			}

			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
			return;
        }

        /* we have an asynchronous call to SOCK before if the ip address is to set */
        if (pAddress->Options & OHA_CTRL_IP_SUITE)
        {
            /* an OHA controlled DCP set is active (no query-user existing) ? */
            if (OHA_IS_NULL(pHDBDcp->pHDBUserCtrl) && OHA_IS_NOT_NULL(pHDBDcp->pLowerDcpSetRqb))
            {
                /* queue the user-rqb */
                OHA_RQB_PUT_BLOCK_TOP(pHDB->SockUserReq.pBottom,
                                      pHDB->SockUserReq.pTop,
                                      pRQB);

                Response = OHA_OK_ACTIVE;
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"An OHA controlled Dcp-Set is active: DcpState (0x%X), NicID (0x%X)", pHDBDcp->QueryState, NICId);
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
                return;
            }

            /* the ip address is to set, queue the request and call SOCK_OPC_SET_IPADDR */
            /* now we have an asynchronous call to SOCK */
            Response = OHA_SOCKSetIpAddress(NICId, OHA_RQB_GET_HANDLE(pRQB),
                                            pAddress->pStationData->LocalIpAddr,
                                            pAddress->pStationData->LocalSubnetMask,
                                            pAddress->pStationData->DefaultRouterAddr,
											/* AP01270055: OHA_ADDR_INFO_TYPE: Missing information by AddressConflict of an EmergencyIP */
                                            (LSA_BOOL)((OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_SET_EMERGENCY) ? LSA_TRUE : LSA_FALSE));
            if (Response != OHA_OK)
            {
                /* continue with SetAddress */
                ErrorOptions |= OHA_CTRL_IP_SUITE;    /* IPAddress not set */
				pAddress->Options = ErrorOptions; 
				OHA_UserRequestFinish(pHDB,pRQB,Response);
            }
            else
            {
                /* queue the user-rqb */
                OHA_RQB_PUT_BLOCK_TOP(pHDB->SockUserReq.pBottom,
                                      pHDB->SockUserReq.pTop,
                                      pRQB);

                Response = OHA_OK_ACTIVE;
            }

			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
			return;
        }

#ifndef OHA_CFG_NO_NARE
        if (pAddress->Options & OHA_CTRL_STATION_NAME)
        {
            /* cancel running check requests */
			LSA_UINT32 CheckOptions = (pAddress->CheckIpEnabled) ? OHA_CTRL_IP_SUITE : 0;
            LSA_BOOL Finish = OHA_NARECheckAddressCancel(NICId, pAddress->Options | CheckOptions, OHA_RQB_GET_HANDLE(pRQB));
            if (!Finish)
            {
                /* queue the user-rqb */
                OHA_RQB_PUT_BLOCK_TOP(pHDB->SockUserReq.pBottom,
                                      pHDB->SockUserReq.pTop,
                                      pRQB);

                Response = OHA_OK_ACTIVE;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
                return;
            }
        }
#endif

        Response = OHA_DoSetAddressRequest(pRQB, pHDB);
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestSetAddress(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoSetAddressRequest                     +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_SET_ADDRESS            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.DbControl                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write parameters to the database               +*/
/*+                                                                         +*/
/*+               Writes station data to database and confirms request.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoSetAddressRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_DCP_PTR_TYPE      pHDBDcp;
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId;
    OHA_UPPER_STATION_PTR_TYPE pStation;
    OHA_UPPER_ADDRESS_PTR_TYPE pAddress;
    LSA_UINT16  RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoSetAddressRequest(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->Params.pDetail);

    pAddress = &pRQB->args.Address;
    OHA_ASSERT_NULL_PTR(pAddress);
    NICId    = pHDB->Params.pDetail->Params.User.NICId;
    pHDBDcp  = OHA_DCPGetHandleByNIC(NICId); /* DCP channel necessary in case of a Query    */
    pStation = pAddress->pStationData;
    OHA_ASSERT_NULL_PTR(pStation);

    if (pAddress->Options & OHA_CTRL_MAC_ADDR)    /* what the user wants to set */
    {
        LSA_BOOL Remanent = (pAddress->Remanent & OHA_CTRL_MAC_ADDR) ? LSA_TRUE : LSA_FALSE;
        RetVal = OHA_DB_WRITE_MAC_ADDR(NICId, pStation->LocalMacAddr, OHA_MAC_ADDR_SIZE, pAddress->AccessMode, Remanent);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    if (pAddress->Options & OHA_CTRL_IP_SUITE)
    {
        LSA_BOOL Remanent = (pAddress->Remanent & OHA_CTRL_IP_SUITE) ? LSA_TRUE : LSA_FALSE;

        RetVal = OHA_DB_WRITE_IP_SUITE(NICId, pStation->LocalIpAddr, pStation->LocalSubnetMask, pStation->DefaultRouterAddr, OHA_IP_ADDR_SIZE, pAddress->AccessMode, Remanent);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    if (pAddress->Options & OHA_CTRL_STATION_TYPE)
    {
        LSA_BOOL Remanent = (pAddress->Remanent & OHA_CTRL_STATION_TYPE) ? LSA_TRUE : LSA_FALSE;
        RetVal = OHA_DB_WRITE_STATION_TYPE(NICId, (OHA_COMMON_MEM_U8_PTR_TYPE)(pStation->pStationType), pStation->StationTypeLen, pAddress->AccessMode, Remanent);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    if (pAddress->Options & OHA_CTRL_STATION_NAME)
    {
        LSA_BOOL Remanent = (pAddress->Remanent & OHA_CTRL_STATION_NAME) ? LSA_TRUE : LSA_FALSE;
        RetVal = OHA_DB_WRITE_STATION_NAME(NICId, (OHA_COMMON_MEM_U8_PTR_TYPE)(pStation->pStationName), pStation->StationNameLen, pAddress->AccessMode, Remanent);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    if (pAddress->Options & OHA_CTRL_STATION_ID)
    {
        LSA_BOOL Remanent = (pAddress->Remanent & OHA_CTRL_STATION_ID) ? LSA_TRUE : LSA_FALSE;

        LSA_UINT8 StationID[2*sizeof(LSA_UINT16)];
        OHA_COMMON_MEM_U8_PTR_TYPE pStationID = StationID;

        OHA_PUT16_HTON(pStationID, 0, pStation->VendorId);
        OHA_PUT16_HTON(pStationID, 2, pStation->DeviceId);

        RetVal = OHA_DB_WRITE_STATION_ID(NICId, pStationID, 2*sizeof(LSA_UINT16), pAddress->AccessMode, Remanent);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    if (pAddress->Options & OHA_CTRL_PNIO_ROLE)
    {
        RetVal = OHA_DB_WRITE_PROFINET_ROLE(NICId, pStation->ProfinetRole);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    if (pAddress->Options & OHA_CTRL_TCIP_STATUS)
    {
        RetVal = OHA_DB_WRITE_TCPIP_STATUS(NICId, pStation->TcpIpStatus);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
    if (pAddress->Options & OHA_CTRL_DEVICE_INSTANCE)
    {
        LSA_BOOL Remanent = (pAddress->Remanent & OHA_CTRL_DEVICE_INSTANCE) ? LSA_TRUE : LSA_FALSE;
        RetVal = OHA_DB_WRITE_DEVICE_INSTANCE(NICId, pStation->DeviceInstance, pAddress->AccessMode, Remanent);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    if (pAddress->Options & OHA_CTRL_RESET_TO_FACTORY)
    {
        /* a ResetToFactory is done, now SetIp (0.0.0.0), SetName (""),... */
		(LSA_VOID)OHA_DoRtfRequest(NICId, pHDB->Params.pDetail->Params.User.CheckInterval);
    }
    if (pAddress->Options & OHA_CTRL_RESET_COMMUNICATION)
    {
        /* a ResetToFactory is done, now SetIp (0.0.0.0), SetName (""),... */
		(LSA_VOID)OHA_DoRtfRequest(NICId, pHDB->Params.pDetail->Params.User.CheckInterval);
    }

    /* changes are made and are queries ? */
    if (pAddress->Options && pAddress->AccessMode == OHA_QUERY_WRITE)
    {
        /* is there already a query-user ? */
        if (!pHDBDcp->pHDBUserCtrl) /* store the control-user */
        {
            pHDBDcp->pHDBUserCtrl = pHDB;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Control User-Set successful");
        }
    }

    /* update the DCP frame filter if filter is active */
    RetVal = OHA_DCPUpdateFilter(NICId,
                                 pAddress->Options,
                                 OHA_HANDLE_TYPE_DCP);
    if (RetVal != OHA_OK)
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoSetAddressRequest - DCPUpdateFilter(RetVal: 0x%X)", RetVal);
        OHA_ASSERT(RetVal == OHA_OK);
    }

    /* update IP-Suites, the ARP-filter if IP-address changed */
    if (pAddress->Options & OHA_CTRL_IP_SUITE)
    {
        LSA_UINT8 IpSuite[OHA_IP_SUITE_SIZE];
		LSA_BOOL  Remanent = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT);

        OHA_MEMCPY(&IpSuite, pStation->LocalIpAddr, OHA_IP_ADDR_SIZE);
        OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE], pStation->LocalSubnetMask, OHA_IP_ADDR_SIZE);
        OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE], pStation->DefaultRouterAddr, OHA_IP_ADDR_SIZE);

		switch(OHA_RQB_GET_OPCODE(pRQB))
		{
			case OHA_OPC_SET_ADDRESS:   /* ExpectedIp := CurrentIp, EmergencyIp := 0.0.0.0 */
				RetVal = oha_db_store_expected_ip_suite(NICId, IpSuite, OHA_IP_SUITE_SIZE);
				if (RetVal != LSA_RET_OK)
				{
					 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_DoSetAddressRequest: Write Expected IP failed (RetVal=%u).", RetVal);
				}
		        oha_db_delete_emergency_ip_suite(NICId);

				if (Remanent)	/* AP00909635 */
				{
					RetVal = oha_db_store_rema_ip_address(NICId, IpSuite);
					if (RetVal != LSA_RET_OK)
					{
						 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_DoSetAddressRequest: Write REMA IP failed (RetVal=%u).", RetVal);
					}
				}
				else
				{
					(LSA_VOID)oha_db_delete_rema_ip_address(NICId);
				}
			break;
			case OHA_OPC_SET_EMERGENCY: /* EmergencyIp := CurrentIp */
                RetVal = oha_db_store_emergency_ip_suite(NICId, IpSuite, OHA_IP_SUITE_SIZE);
					if (RetVal != LSA_RET_OK)
					{
						 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_DoSetAddressRequest: Write Emergency IP failed (RetVal=%u).", RetVal);
					}

				/* AP00909635 - in case of an emergency IP the rema IP remains ! */
				/* AP01265748 - a remanent emergency IP is not to store in REMA ! */
			break;
			default:
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			break;
		}

        oha_db_write_ip_conflict_status(NICId, LSA_FALSE);

        RetVal = OHA_EDDSetArpFilter(NICId, OHA_HANDLE_TYPE_DCP);
        if (RetVal != OHA_OK)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoSetAddressRequest - EDDSetArpFilter(RetVal: 0x%X)", RetVal);
        }
        RetVal = OHA_EDDSetIp(NICId, OHA_HANDLE_TYPE_DCP);
        if (RetVal != OHA_OK)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoSetAddressRequest - EDDSetIp(RetVal: 0x%X)", RetVal);
        }
    }

    if ((pAddress->Options & OHA_CTRL_STATION_NAME) || /* AP01565375 */
        (pAddress->Options & OHA_CTRL_IP_SUITE))
    {
        oha_db_write_factory_reset_status(NICId, LSA_FALSE);
    }

    /* update the LLDP-send data if LLDP-Send is active */
    RetVal = OHA_LLDPUpdateSend(NICId,
                                pAddress->Options,
                                OHA_HANDLE_TYPE_DCP);
    if (RetVal != OHA_OK)
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoSetAddressRequest - LLDPUpdateSend(RetVal: 0x%X)", RetVal);
    }

    if (pAddress->Options)      /* are changes made ? */
    {
        OHA_AddressIndicateEventAllUser(NICId); /* address changed - indication */
        OHA_DcpIndicateEventAllOtherUsers(pRQB, NICId);  /* confirm other users */
    }

#ifndef OHA_CFG_NO_NARE
    /* is the IP-Suite to check (ethernet) */
    /* is the NameOfStation to check (ethernet) */
    RetVal = OHA_NARECheckAddress(NICId, (pAddress->CheckIpEnabled) ? OHA_CTRL_IP_SUITE : 0, 
                                  pAddress->Options, pHDB->Params.pDetail->Params.User.CheckInterval);
    if (RetVal != OHA_OK)
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoSetAddressRequest - OHA_NARECheckAddress failed (RetVal: 0x%X)", RetVal);
    }
#endif

	pAddress->Options = 0;	/* no error, all params written */

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoSetAddressRequest(Response: 0x%X)", Response);
    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadDcpElement                   +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_DCP_READ_ELEMENT       +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.DcpElem                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read parameters from the database              +*/
/*+                                                                         +*/
/*+               Fills RQB with station data and confirms request.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadDcpElement(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_UINT16                Response;
    LSA_UINT16                NICId;
    OHA_DCP_ELEM_PTR_TYPE     pDcpElement;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadDcpElement(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

    pDcpElement = &pRQB->args.DcpElem;
    OHA_ASSERT_NULL_PTR(pDcpElement);
    NICId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId); /* only EDD channel for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_READ_ELEMENT failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else
    {
        OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
        LSA_UINT16 Length = 0;

        Response = OHA_DB_READ_DCP_ELEMENT(NICId,
                                           pDcpElement->Type,      /* type of variable, option */
                                           pDcpElement->SubType,   /* subtype of variable, suboption */
                                           &pVar,                 /* variable in octets */
                                           &Length,                /* length of variable in octets */
                                           &pDcpElement->AccCtrl,  /* access control for variable */
                                           &pDcpElement->Remanent);/* storage of variable in memory */

        if (Response == LSA_RET_OK)
        {
            Response = OHA_OK;
            if (OHA_IS_NOT_NULL(pDcpElement->pVariable) && (Length > pDcpElement->Length))
            {
                Response = OHA_ERR_BUFFER_OVERFLOW;
            }
            else
            {
                pDcpElement->Length = Length;
            }
            if (OHA_IS_NOT_NULL(pVar) && OHA_IS_NOT_NULL(pDcpElement->pVariable))
            {
                OHA_MEMCPY(pDcpElement->pVariable, pVar, pDcpElement->Length);
            }
        }
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadDcpElement(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoWriteAddressRequest                   +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_SET_ADDRESS            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.DcpElem                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write parameters to the database               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoWriteAddressRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16                Response;
    OHA_DCP_ELEM_PTR_TYPE     pDcpElement;
    LSA_UINT32                Options = 0;
    LSA_UINT16                NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoWriteAddressRequest(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    pDcpElement = &pRQB->args.DcpElem;
    OHA_ASSERT_NULL_PTR(pDcpElement);
    NICId = pHDB->Params.pDetail->Params.User.NICId;

    Response = OHA_DB_WRITE_DCP_ELEMENT(NICId,
                                        pDcpElement->Type,      /* type of variable, option */
                                        pDcpElement->SubType,   /* subtype of variable, suboption */
                                        pDcpElement->pVariable, /* variable in octets */
                                        pDcpElement->Length,   /* length of variable in octets */
                                        pDcpElement->AccCtrl,  /* access control for variable */
                                        pDcpElement->Remanent);/* storage of variable in memory */

	if (Response != OHA_OK)
	{
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_DoWriteAddressRequest: Write DCP ELEMENT failed (RetVal=%u).", Response);
	    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoWriteAddressRequest(Response: 0x%X)", Response);
	    return Response;
	}

    /* Set IpAddress ? */
    if (pDcpElement->Type == OHA_DCP_OPTION_TCP && pDcpElement->SubType == DCP_DSID_TCPIP)
    {
        Options = OHA_CTRL_IP_SUITE;

		if (pDcpElement->Remanent)	/* AP00909635 */
		{
			LSA_UINT16 RetVal;
			OHA_ASSERT(pDcpElement->Length == OHA_IP_SUITE_SIZE && OHA_IS_NOT_NULL(pDcpElement->pVariable));

			RetVal = oha_db_store_rema_ip_address(NICId, pDcpElement->pVariable);
			if (RetVal != LSA_RET_OK)
			{
				 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_DoWriteAddressRequest: Write REMA IP failed (RetVal=%u).", RetVal);
			}
		}
		else
		{
			(LSA_VOID)oha_db_delete_rema_ip_address(NICId);
		}
    }

    /* Set NameOfStation ? */
    if (pDcpElement->Type == OHA_DCP_OPTION_IDENT && pDcpElement->SubType == DCP_DSID_IDNAME)
    {
        Options = OHA_CTRL_STATION_NAME;
    }

#ifndef OHA_CFG_NO_NARE
    if (Options)
    {
        OHA_HDB_NARE_PTR_TYPE pHDBNare = OHA_NAREGetHandleByNIC(NICId);

        OHA_ASSERT_NULL_PTR(pHDBNare);

        /* initialize the ethernet check parameters */
        if (Options & OHA_CTRL_IP_SUITE)
        {
            pHDBNare->IpCheck.State    = OHA_CHECK_STATE_STOP;
            pHDBNare->IpCheck.Interval = pHDB->Params.pDetail->Params.User.CheckInterval;

            oha_db_write_ip_conflict_status(NICId, LSA_FALSE);
        }
    }
#endif

    /* ResetToFactory ? */
    if (pDcpElement->Type == OHA_DCP_OPTION_CTRL && pDcpElement->SubType == OHA_DCP_RESET_TO_FACTORY)
	{
		Options = OHA_CTRL_RESET_TO_FACTORY;

        /* a ResetToFactory is done, now SetIp (0.0.0.0), SetName (""),... */
		(LSA_VOID)OHA_DoRtfRequest(NICId, pHDB->Params.pDetail->Params.User.CheckInterval);
	}

    /* Reset Data ? */
    if (pDcpElement->Type == OHA_DCP_OPTION_CTRL && pDcpElement->SubType == OHA_DCP_RESET)
	{
		Options = OHA_CTRL_RESET_COMMUNICATION;

        /* a ResetToFactory is done, now SetIp (0.0.0.0), SetName (""),... */
		(LSA_VOID)OHA_DoRtfRequest(NICId, pHDB->Params.pDetail->Params.User.CheckInterval);
	}

    if ((Options & OHA_CTRL_STATION_NAME) ||
        (Options & OHA_CTRL_IP_SUITE)) /* AP01565375 */
    {
        oha_db_write_factory_reset_status(NICId, LSA_FALSE);
    }

	if (Options)
    {
       /* update the LLDP-send data if LLDP-Send is active */
        LSA_UINT16 RetVal = OHA_LLDPUpdateSend(NICId, Options, OHA_HANDLE_TYPE_DCP);
        if (RetVal != OHA_OK)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoWriteAddressRequest - LLDPUpdateSend(RetVal: 0x%X)", RetVal);
        }

		/* confirm other users */
        OHA_AddressIndicateEventAllUser(NICId);    /* address changed - indication */
        OHA_DcpIndicateEventAllOtherUsers(pRQB, NICId);
    }

#ifndef OHA_CFG_NO_REMA
    /* Set DHCP ? */
    if (pDcpElement->Type == OHA_DCP_OPTION_DHCP && pDcpElement->SubType == 61)
    {
		(LSA_VOID)oha_rema_dhcp_update(NICId);
    }
#endif

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoWriteAddressRequest(Response: 0x%X)", Response);
    return Response;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoRtfRequest                            +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_PRM_COMMIT             +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.DcpElem                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: ResetToFactory-Request to write NameOfStation := "" and   +*/
/*+               IP-Suite := (0.0.0.0, 0.0.0.0, 0.0.0.0) to the database.  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoRtfRequest(
   LSA_UINT16               NICId,
   LSA_UINT16               CheckInterval)
{
	LSA_UINT16 RetVal;
	LSA_UINT16 Response = OHA_OK;
	OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NICId);

	OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
    	                   "IN : OHA_DoRtfRequest(NICId: 0x%X, CheckInterval: 0x%X)", NICId, CheckInterval);

    /* Set IP-Suite := (0.0.0.0, 0.0.0.0, 0.0.0.0) */
    (LSA_VOID)oha_db_delete_real_ip_suite(NICId);
    (LSA_VOID)oha_db_write_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, LSA_FALSE);

    /* Set NameOfStation := "" */
    (LSA_VOID)oha_db_delete_station_name(NICId);
    (LSA_VOID)oha_db_write_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, LSA_FALSE);

    oha_db_write_ip_conflict_status(NICId, LSA_FALSE);

	oha_db_write_factory_reset_status(NICId, LSA_TRUE); /* AP01565375 */

    oha_db_delete_emergency_ip_suite(NICId);
    oha_db_delete_expected_ip_suite(NICId);

	(LSA_VOID)oha_db_delete_rema_ip_address(NICId);	/* AP00909635 */

	oha_db_delete_profibus_params(NICId);

    /* update the DCP frame filter if filter is active */
    RetVal = OHA_DCPUpdateFilter(NICId,
                                 OHA_CTRL_STATION_NAME,
                                 OHA_HANDLE_TYPE_DCP);
    if (RetVal != OHA_OK)
    {
        Response = OHA_ERR_FAILED;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoRtfRequest - DCPUpdateFilter(RetVal: 0x%X)", RetVal);
    }

    RetVal = OHA_EDDSetArpFilter(NICId, OHA_HANDLE_TYPE_DCP);
    if (RetVal != OHA_OK)
    {
        Response = OHA_ERR_FAILED;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoRtfRequest - EDDSetArpFilter(RetVal: 0x%X)", RetVal);
    }
    RetVal = OHA_EDDSetIp(NICId, OHA_HANDLE_TYPE_DCP);
    if (RetVal != OHA_OK)
    {
        Response = OHA_ERR_FAILED;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoRtfRequest - EDDSetIp(RetVal: 0x%X)", RetVal);
    }

#ifndef OHA_CFG_NO_NARE
    /* is the IP-Suite to uncheck (ethernet) */
    /* is the NameOfStation to uncheck (ethernet) */
    RetVal = OHA_NARECheckAddress(NICId, 0, OHA_CTRL_STATION_NAME | OHA_CTRL_IP_SUITE, CheckInterval);
    if (RetVal != OHA_OK)
    {
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DoRtfRequest - OHA_NARECheckAddress failed (RetVal: 0x%X)", RetVal);
    }
#endif

	/* AP01319671 (writeable LLDP-MIB): set MIB values and MIB access to default by ResetToFactory */
	OHA_ASSERT_NULL_PTR(pDB);
	(LSA_VOID)oha_init_defvals(pDB);	/* this if */

	/* delete (mib objects) SysName, SysContact and SysLocation */
    oha_delete_lldp_DbElement(&oha_database.StationData.pSysName, LSA_NULL);  /* system - name */
    oha_delete_snmp_DbElement(&oha_database.StationData.pSysContact, LSA_NULL); /* system - contact */
    oha_delete_snmp_DbElement(&oha_database.StationData.pSysLocation, LSA_NULL); /* system - location */

	RetVal = OHA_SOCKUpdateMib2Params(NICId, OHA_HANDLE_TYPE_LLDP,
										LSA_NULL, 0, LSA_FALSE,
										LSA_NULL, 0, LSA_FALSE,
										LSA_NULL, 0, LSA_FALSE);
	if (RetVal != OHA_OK)
	{
		OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_DoRtfRequest: Setting MIB2-Params failed (RetVal=%u).", RetVal);
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

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoRtfRequest(Response: 0x%X)", Response);
    return Response;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestWriteDcpElement                  +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_DCP_WRITE_ELEMENT      +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.DcpElem                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read parameters from the database              +*/
/*+                                                                         +*/
/*+               Fills RQB with station data and confirms request.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestWriteDcpElement(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId;
    OHA_DCP_ELEM_PTR_TYPE     pDcpElement;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestWriteDcpElement(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

    pDcpElement = &pRQB->args.DcpElem;
    OHA_ASSERT_NULL_PTR(pDcpElement);
    NICId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId); /* only EDD channel for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_RequestWriteDcpElement failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else if (!(pHDB->State == OHA_HANDLE_STATE_OPEN &&
               LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) ) )
    {           /* is no open/close user-channel runnning ? */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_RequestWriteDcpElement failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else if (oha_db_read_user_lock(NICId, pHDB->ThisHandle, pDcpElement->Type, pDcpElement->SubType))  /* locked by any user ? */
    {
        Response = OHA_ERR_PARAM;  /* any user has locked */
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_RequestWriteDcpElement failed. Any user has locked (Option: 0x%X, SubOption: 0x%X)", pDcpElement->Type, pDcpElement->SubType);
    }
    else if (pDcpElement->Type == OHA_DCP_OPTION_TCP && pDcpElement->SubType == DCP_DSID_TCPIP) /* Set IpAddress */
    {
        if (!(pDcpElement->Length == OHA_IP_SUITE_SIZE && OHA_IS_NOT_NULL(pDcpElement->pVariable)))
        {
            Response = OHA_ERR_PARAM;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_RequestWriteDcpElement failed. No buffer or length of IpSuite is wrong (NIC-ID: 0x%X)", NICId);
        }
        /* we check the IP-address too */
        else if (!oha_is_valid_ip_address(OHA_GET32_NTOH(pDcpElement->pVariable, 0),
                                          OHA_GET32_NTOH((pDcpElement->pVariable + OHA_IP_ADDR_SIZE), 0),
                                          OHA_GET32_NTOH((pDcpElement->pVariable + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE), 0),
                                          LSA_FALSE)) /* 0.0.0.0 is allowed by a local set */
        {
            Response = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
            /* Error on getting IpAddress. Abort. */
            OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_RequestWriteDcpElement failed. Invalid IP-Suite (IP: 0x%X, SubNet: 0x%X, GateWay: 0x%X)",
                                 OHA_GET32_NTOH(pDcpElement->pVariable,0),
                                 OHA_GET32_NTOH(pDcpElement->pVariable + OHA_IP_ADDR_SIZE,0),
                                 OHA_GET32_NTOH(pDcpElement->pVariable + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE,0));
        }
        if (Response == OHA_OK) /* ip address is o.k. */
        {
            /* the ip address is to set, queue the request and call SOCK_OPC_SET_IPADDR */
            if (OHA_IS_NOT_NULL(pHDB->SockUserReq.pBottom))
            {
                Response = OHA_ERR_SEQUENCE;  /* already one request active */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_DCP_WRITE_ELEMENT(IpAddr) failed. Already one User-Request active for this NicId (NIC-ID: 0x%X)",NICId);
            }
            else
            {
                /* SetIp from any other user running ? */
                if (OHA_IsAnyLocalSetActive(NICId))
                {
                    Response = OHA_ERR_SEQUENCE;  /* already one request active */
                    OHA_UserRequestFinish(pHDB,pRQB,Response);
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestWriteDcpElement(Response: 0x%X)", Response);
                    return;
                }

                Response = OHA_SOCKSetIpAddress(NICId, OHA_RQB_GET_HANDLE(pRQB),
                                                pDcpElement->pVariable,
                                                pDcpElement->pVariable + OHA_IP_ADDR_SIZE,
                                                pDcpElement->pVariable + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE,
                                                LSA_FALSE);
                if (Response == OHA_OK)
                {
                    /* queue the user-rqb */
                    OHA_RQB_PUT_BLOCK_TOP(pHDB->SockUserReq.pBottom,
                                          pHDB->SockUserReq.pTop,
                                          pRQB);

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestWriteDcpElement(Response: 0x%X)", Response);
                    return;
                }
            }
        }
    }
    else if (pDcpElement->Type == OHA_DCP_OPTION_IDENT && pDcpElement->SubType == DCP_DSID_IDNAME)  /* NameOfStation */
    {
        if (pDcpElement->Length != 0 && pDcpElement->Length <= OHA_MAX_STATION_NAME_LEN)
        {
            /* check, if it's a RFC-name */
            if (!oha_is_valid_station_name(pDcpElement->Length, pDcpElement->pVariable, OHA_UPPER_CASES_ALLOWED(NICId), OHA_UPPER_CASES_ALLOWED(NICId)))
            {
                Response = OHA_ERR_PARAM;
            }
        }
    }
    else if (pDcpElement->Type == OHA_DCP_OPTION_IDENT && pDcpElement->SubType == DCP_DSID_IDID)  /* StationID */
	{
		 /* 936497: initial value must be set via OHA_DB_WRITE_STATION_ID(), a runtime value is set via PDIdentificationRecord */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"The initial value of VendorID/DeviceID is to set via OHA_DB_WRITE_STATION_ID()!");
        Response = OHA_ERR_PARAM;
    }
    else if (pDcpElement->Type == OHA_DCP_OPTION_IDENT && pDcpElement->SubType == DCP_DSID_IDDEVINST)  /* DeviceInstance */
	{
		 /* 936497: initial value must be set via OHA_DB_WRITE_DEVICE_INSTANCE(), a runtime value is set via PDIdentificationRecord */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"The initial value of InstanceID is to set via OHA_DB_WRITE_DEVICE_INSTANCE()!");
        Response = OHA_ERR_PARAM;
	}
    else if (pDcpElement->Type == OHA_DCP_OPTION_IDENT && pDcpElement->SubType == DCP_DSID_IDOEMID)  /* OEMDeviceID */
	{
		 /* initial value must be set via OHA_OPC_SET_IDENT, setting a runtime value is not allowed */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"The initial value of OEMDeviceID is to set via OHA_OPC_SET_IDENT!");
        Response = OHA_ERR_PARAM;
	}

    if (Response == OHA_OK) /* everything o.k. */
    {
        Response = OHA_DoWriteAddressRequest(pRQB, pHDB);
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestWriteDcpElement(Response: 0x%X)", Response);
}


/*=============================================================================
 * function name:  OHA_CONTROL_DCP_OPTIONS
 * function:       Lock or Unlock  Dcp-Options
 * parameters:     AccessControl :
                   OHA_READ_ONLY, OHA_READ_WRITE, OHA_NO_ACCESS
 *                 Options (bit-coded):
                   OHA_CHANGE_OF_MAC_ADDR,  OHA_CHANGE_OF_IP_SUITE,
                   OHA_CHANGE_OF_DHCP_OPTION, OHA_CHANGE_OF_STATION_ID,
                   OHA_CHANGE_OF_STATION_NAME, OHA_CHANGE_OF_STATION_TYPE,
                   OHA_CHANGE_OF_PROFIBUS_PARAM, OHA_CHANGE_OF_PROFIBUS_PROFIL,
                   OHA_CHANGE_OF_PBC_SWITCH, OHA_CHANGE_OF_PBC_RESPONSE,

 * return value:   LSA_UINT16:      LSA_RET_OK or Error
 *===========================================================================*/
LSA_UINT16 OHA_CONTROL_DCP_OPTIONS (
    LSA_UINT16      NicId,
    LSA_UINT8       AccessControl,
    LSA_UINT32      Options
)
{
    LSA_UINT16          Ret;
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CONTROL_DCP_OPTIONS(NicId: 0x%X, AccessControl: 0x%X, Options: 0x%X)", NicId, AccessControl, Options);
    if (OHA_IS_NOT_NULL(pDB))
    {
        switch (AccessControl)
        {
            case OHA_READ_ONLY:   /* write the LockStatus for DcpSet to the database */
            case OHA_READ_WRITE:  /* unlock a DcpSet to the database */
            case OHA_NO_ACCESS:   /* no Dcp-GET and no Dcp-SET */
            case OHA_QUERY_WRITE: /* query the user for a Dcp-SET */
                Ret = OHA_DcpWriteAccessControl(NicId, Options, AccessControl);
                break;
            default:
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Control failed, no such option supported");
                Ret = LSA_RET_ERR_PARAM;
                break;
        }
    }
    else
    {
        Ret = LSA_RET_ERR_PARAM;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CONTROL_DCP_OPTIONS(Ret: 0x%X)", Ret);
    return(Ret);
}


/*=============================================================================
 * function name:  oha_check_access_control
 * function:       check the parameter
 * parameters:     AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 oha_check_access_control (
    LSA_UINT8       AccessControl )
{
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_check_access_control(AccessControl: 0x%X)", AccessControl);
    switch (AccessControl)
    {
        case OHA_READ_ONLY:
        case OHA_WRITE_ONLY:
        case OHA_NO_ACCESS:
        case OHA_QUERY_WRITE:
        case OHA_READ_WRITE:
            RetVal = LSA_RET_OK;
            break;
        default:
            RetVal = LSA_RET_ERR_PARAM;
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_check_access_control(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*****************************************************************************/
/* Internal input function: oha_create_DbElement()                           */
/* Allocates local database-element                                          */
/*****************************************************************************/
OHA_DB_ELEM_PTR_TYPE oha_create_DbElement(
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length,     /* length of variable in octets */
    LSA_UINT8                   AccCtrl,    /* access control for variable */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
)
{
    OHA_DB_ELEM_PTR_TYPE pElement = LSA_NULL;
    OHA_LOCAL_MEM_PTR_TYPE MemPtr = LSA_NULL;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_create_DbElement(Type: 0x%X, SubType: 0x%X, Length: 0x%X, AccCtrl: 0x%X, Remanent: 0x%X)",
                          Type, SubType, Length, AccCtrl, Remanent);
    OHA_ALLOC_LOCAL_MEM(&MemPtr, sizeof(OHA_DB_ELEM_TYPE));
    if (! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
    {
        OHA_MEMSET(MemPtr,0,sizeof(OHA_DB_ELEM_TYPE));
        pElement = (OHA_DB_ELEM_PTR_TYPE)MemPtr;

        pElement->Type     = Type;
        pElement->SubType  = SubType;
        pElement->AccCtrl  = AccCtrl;
        pElement->Remanent = Remanent;
        OhaListInitialize(& pElement->ListElem);

        if (Length && OHA_IS_NOT_NULL(pVariable))
        {
            OHA_ALLOC_LOCAL_MEM(&MemPtr, Length);
            if (! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
            {
                pElement->pVariable = (OHA_COMMON_MEM_U8_PTR_TYPE)MemPtr;
                OHA_MEMCPY(pElement->pVariable, pVariable, Length);
                pElement->Length = Length;
            }
            else
            {
                LSA_UINT16   FreeStat = LSA_RET_OK;
                OHA_FREE_LOCAL_MEM(&FreeStat, pElement);
                OHA_MEM_FREE_FAIL_CHECK(FreeStat);
                pElement = LSA_NULL;
            }
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_create_DbElement(). Allocating memory failed.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_create_DbElement(pElement: 0x%X)", pElement);
    return pElement;
}


/*****************************************************************************/
/* Internal input function: oha_find_DbElement()                             */
/* Search for a database-element                                             */
/*****************************************************************************/
OHA_DB_ELEM_PTR_TYPE oha_find_DbElement(
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor,   /* start of a list  */
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType     /* subtype of variable */
)
{
    OHA_DB_ELEM_PTR_TYPE pElement = LSA_NULL;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_find_DbElement(Type: 0x%X, SubType: 0x%X)",
                          Type, SubType);

    if (!(LSA_HOST_PTR_ARE_EQUAL(pAnchor, LSA_NULL) || OhaListIsEmpty(pAnchor)) )
    {
        switch (Type)
        {
            case OHA_DCP_OPTION_LLDP:
            case OHA_DCP_OPTION_DHCP:
            case OHA_DCP_OPTION_CTRL:
            {
                OHA_DB_ELEM_PTR_TYPE pTempElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE);    /* starts with the first element */

                while (OHA_IS_NOT_NULL(pTempElement))
                {
                    if (pTempElement->Type == Type &&
                        pTempElement->SubType == SubType)
                    {
                        pElement = pTempElement;
                        break;  /* element found */
                    }
                    pTempElement = OhaListNext(pAnchor, &pTempElement->ListElem, OHA_DB_ELEM_PTR_TYPE);
                }
            }
            break;
            default:
                pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE);         /* only one element possible */
                break;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"oha_find_DbElement(). List is empty or Anchor is NULL.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_find_DbElement(pElement: 0x%X)", pElement);
    return pElement;
}


/*****************************************************************************/
/* Internal input function: oha_update_DbElement()                           */
/* updates an existing database-element                                      */
/*****************************************************************************/
LSA_UINT16 oha_update_DbElement(
    OHA_DB_ELEM_PTR_TYPE        pElement,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length,     /* length of variable in octets */
    LSA_UINT8                   AccCtrl,    /* access control for variable */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
)
{
    LSA_UINT16 RetVal = LSA_RET_OK;
    OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;    /* variable in octets */

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_update_DbElement(Type: 0x%X, SubType: 0x%X, Length: 0x%X, AccCtrl: 0x%X, Remanent: 0x%X)",
                          Type, SubType, Length, AccCtrl, Remanent);
    if (! LSA_HOST_PTR_ARE_EQUAL(pElement, LSA_NULL) )
    {
        LSA_UINT16   FreeStat = LSA_RET_OK;  /* free the actual buffer for var. */

        if (Length && OHA_IS_NOT_NULL(pVariable))   /* is there a buffer to allocate ? */
        {
            OHA_ALLOC_LOCAL_MEM(&pVarTemp, Length); /* alloc the new buffer for the variable */
            if ( LSA_HOST_PTR_ARE_EQUAL(pVarTemp, LSA_NULL) )   /* failed the allocation */
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_update_DbElement(). Allocating memory failed.");
                RetVal = LSA_RET_ERR_RESOURCE;
                return RetVal;
            }
        }

        /* is there a buffer to free ? */
        if (pElement->pVariable)
        {
            OHA_FREE_LOCAL_MEM(&FreeStat, pElement->pVariable);
            OHA_MEM_FREE_FAIL_CHECK(FreeStat);
        }

        pElement->pVariable = (OHA_COMMON_MEM_U8_PTR_TYPE)pVarTemp;   /* now supply the new var. */

        pElement->Type = Type;
        pElement->SubType = SubType;
        if (Length && pVariable)
        {
            OHA_MEMCPY(pElement->pVariable, pVariable, Length);
            pElement->Length = Length;
        }
        else
        {
            pElement->Length = 0;
        }

        pElement->AccCtrl = AccCtrl;
        pElement->Remanent = Remanent;
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_update_DbElement(). No such element.");
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_update_DbElement(RetVal: 0x%X, pElement: 0x%X)", RetVal, pElement);
    return RetVal;
}

/*****************************************************************************/
/* Internal input function: oha_remove_DbElement()                           */
/* Frees the database-element                                                */
/*****************************************************************************/
LSA_VOID oha_remove_DbElement  (OHA_DB_ELEM_PTR_TYPE pElement)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_remove_DbElement(pElement: 0x%X)", pElement);
    if (pElement)       /* get the element */
    {
        LSA_UINT16 RetVal = LSA_RET_OK;

        if (! LSA_HOST_PTR_ARE_EQUAL(pElement->pVariable, LSA_NULL) )
        {
            LSA_UINT16   FreeStat = LSA_RET_OK;
            OHA_FREE_LOCAL_MEM(&FreeStat, pElement->pVariable);
            OHA_MEM_FREE_FAIL_CHECK(FreeStat);
        }
        OHA_FREE_LOCAL_MEM(&RetVal,pElement);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_remove_DbElement()");
}


/*****************************************************************************/
/* Internal input function: oha_delete_DbElement()                           */
/* Frees the database-element                                                */
/*****************************************************************************/
LSA_VOID oha_delete_DbElement (OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_delete_DbElement(pAnchor: 0x%X)", pAnchor);

    if (OHA_IS_NOT_NULL(pAnchor))       /* get the element */
    {
        OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE);    /* starts with the first element */

        if (OHA_IS_NOT_NULL(pElement))
        {
#if 0       /* AP01500867: OHA_DB_UNDO_INIT_INTERFACE() */
            switch (pElement->Type)
            {
                case OHA_DCP_OPTION_LLDP:
                case OHA_DCP_OPTION_DHCP:
                case OHA_DCP_OPTION_CTRL:
                {
#endif
                    while (OHA_IS_NOT_NULL(pElement))
                    {
                        /* remove from list and free */
                        OhaListRemoveEntry(&pElement->ListElem);
                        oha_remove_DbElement (pElement);
                        pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); /* reinit. */
                        /* pElement = OhaListNext(pAnchor, &Element->ListElem, OHA_DB_ELEM_PTR_TYPE); */
                    }
#if 0
                }
                break;
                default:
                    oha_remove_DbElement (pElement);
                    break;
            }
#endif
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_delete_DbElement()");
}


/*****************************************************************************/
/* Internal input function: oha_unlock_DbElement()                           */
/* Unlock the database-element for an user                                   */
/*****************************************************************************/
LSA_VOID oha_unlock_DbElement (OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor, LSA_UINT16 UserHandle)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_unlock_DbElement(pAnchor: 0x%X)", pAnchor);

    if (UserHandle < OHA_CFG_MAX_CHANNELS && /* valid user handle */
        OHA_IS_NOT_NULL(pAnchor))            /* get the element   */
    {
        OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE);    /* starts with the first element */

        if (OHA_IS_NOT_NULL(pElement))
        {
            switch (pElement->Type)
            {
                default:
					if(pElement->LockCnt[UserHandle])
					{
						OHA_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_UNEXP,"oha_unlock_DbElement(): UserHandle(0x%X), Type(0x%X), SubType(0x%X) is locked (LockCnt: 0x%X).", 
												UserHandle, pElement->Type, pElement->SubType, pElement->LockCnt[UserHandle]);
					}
                    pElement->LockCnt[UserHandle] = 0;     /* finally unlock the user-control */
                    break;
                case OHA_DCP_OPTION_LLDP:
                case OHA_DCP_OPTION_DHCP:
                case OHA_DCP_OPTION_CTRL:
                {
                    while (OHA_IS_NOT_NULL(pElement))
                    {
						if(pElement->LockCnt[UserHandle])
						{
							OHA_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_UNEXP,"oha_unlock_DbElement(): UserHandle(0x%X), Type(0x%X), SubType(0x%X) is locked (LockCnt: 0x%X).", 
													UserHandle, pElement->Type, pElement->SubType, pElement->LockCnt[UserHandle]);
						}
                        pElement->LockCnt[UserHandle] = 0;     /* finally unlock the user-control */
                        pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_ELEM_PTR_TYPE);
                    }
                }
                break;
            }
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_unlock_DbElement()");
}


/*=============================================================================
 * function name:  oha_get_db_element
 * function:       writes the parameters to the database
 * parameters:     Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
 * return value:   pElement:        NULL or <> NULL
 *===========================================================================*/
OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * oha_get_db_element(
    LSA_UINT16  NicId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pElement = LSA_NULL;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_get_db_element(NicId: 0x%X, Type: 0x%X, SubType: 0x%X)",
                          NicId, Type, SubType);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_get_db_element(pElement: 0x%X)", pElement);
        return (pElement);           /* NULL */
    }

    /* ++ check the params ++ check the params ++ */
    if ((Type >= 12) && (Type <= 127)) /* reserved options */
    {
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_get_db_element(pElement: 0x%X)", pElement);
        return (pElement);           /* NULL */
    }
    else
    {
        OHA_DB_IF_PTR_TYPE  pDbIf = OHA_DB_GET_INTERFACE(NicId);
        if (OHA_IS_NULL(pDbIf))            /* no interface */
        {
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_get_db_element(pElement: 0x%X)", pElement);
            return (pElement);       /* NULL */
        }

        /* ++ get the db-entry ++ gwt the db-entry ++ */
        switch (Type)
        {                       /* DCP Database IDs (wellknown Options) */
            case OHA_DCP_OPTION_TCP:            /* TCP-IP parameter (1)     */
            {
                switch (SubType)
                {
                    case DCP_DSID_MACADR:       /* ISO/TCP: MAC address (1) */
                        pElement = &pDbIf->DcpData.pLocalMacAddr;
                        break;
                    case DCP_DSID_TCPIP:        /* IP address... (2)        */
                        pElement = &pDbIf->DcpData.pLocalIpSuite;
                        break;
                    default:                   /* manufactoring options */
                        break;
                }
            }
            break;
            case OHA_DCP_OPTION_IDENT:          /* Station identification (2)   */
            {
                switch (SubType)
                {
                    case DCP_DSID_IDNAME:       /* IDENT: station name (2)      */
                        pElement = &pDbIf->pStationName;
                        break;
                    case DCP_DSID_IDOPTS:       /* IDENT: Option list (5)      */
                        pElement = &pDbIf->DcpData.pDbaseList;
                        break;
                    case DCP_DSID_IDTYPE:       /* IDENT: station type (1)    */
		                pElement = &pDbIf->DcpData.pStationType;
						break;
                    case DCP_DSID_IDID:         /* IDENT: DetailedType (3)    */
		                pElement = &pDbIf->DcpData.pStationId;
						break;
                    case DCP_DSID_IDROLE:       /* IDENT: ProfinetIO role (4) */
		                pElement = &pDbIf->DcpData.pProfinetRole;
						break;
                    case DCP_DSID_IDDEVINST:  /* IDENT: Device instance (7) */
		                pElement = &pDbIf->DcpData.pDevInstance;
						break;
                    case DCP_DSID_IDOEMID:  /* IDENT: OEMDeviceID (8) */
		                pElement = &pDbIf->DcpData.pOEMDeviceID;
						break;
                    default:                    /* manufactoring options */
                        break;
                }
            }
            break;
            case OHA_DCP_OPTION_DHCP:           /* DHCP-Parameter (3)           */
            {
                pElement = &pDbIf->DcpData.pDhcpOptions;
            }
            break;
            case OHA_DCP_OPTION_LLDP:           /* DHCP-Parameter (4)           */
            {
            }
            break;
            case OHA_DCP_OPTION_CTRL:           /* Control options (5)          */
            {
                switch (SubType)
                {
                    case OHA_DCP_TRANS_START:       /* (1) Start transact.          */
                    case OHA_DCP_TRANS_END:         /* (2) EndOf transact.          */
                    case OHA_DCP_SIGNAL:            /* (3) Signal                   */
                    case OHA_DCP_RESPONSE:          /* (4) Response result          */
                    case OHA_DCP_RESET_TO_FACTORY:  /* (5) Reset to factory settings */
                    case OHA_DCP_RESET:             /* (6) Reset param and/or data  */
                        pElement = &pDbIf->DcpData.pControlOptions;
                        break;
                    default:                    /* other options        */
                        break;                      /* return NULL: are not stored */
                }
            }
            break;
            case DCP_DBID_PBPARAM:        /* Profibus parameter (129)     */
            {
                switch (SubType)
                {
                    case DCP_DSID_PBCOMMON:     /* Profibus: Common Parameters (3) */
                        pElement = &pDbIf->DcpData.pPB_Common;
                        break;
                    case DCP_DSID_PBADDITION:      /* Profibus: Additional Parameters (4) */
                        pElement = &pDbIf->DcpData.pPB_Addition;
                        break;
                    default:                   /* manufactoring options */
                        pElement = &pDbIf->DcpData.pPB_Others;
                        break;
                }
            }
            break;
            case OHA_DCP_OPTION_NOFILTER:       /* IDENTIFY No filter (255)     */
            {
                switch (SubType)
                {
                    case DCP_DSID_NOFILTER: /* IDENTIFY: No filter (255)     */
                        break;
                    default:                   /* manufactoring options */
                        break;
                }
            }
            break;
            case DCP_DBID_DEVINIT:              /* DeviceInitiative Option  */
            {
                switch (SubType)
                {
                    case DCP_DSID_DEVINIT:      /* DeviceInitiative SubOpt. */
                        pElement = &pDbIf->DcpData.pDeviceInitiative;
                        break;
                    default:
                        break;
                }
            }
            break;
            default:                   /* manufactoring options */
                break;
        }
    }

    /* only initialized with NULL */
    if (OHA_IS_NOT_NULL(pElement) && OhaListIsNotInitialized(pElement))
    {
        OhaListInitialize(pElement);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_get_db_element(pElement: 0x%X)", pElement);
    return pElement;
}

/*****************************************************************************/
/* Internal input function: OHA_GetDbElements()                              */
/* Counts the database-suboptions and writes them in the DbaseList if given  */
/*****************************************************************************/
LSA_UINT16 OHA_GetDbElements(OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor,
							 OHA_COMMON_MEM_U16_PTR_TYPE pDbaseList,
							 LSA_UINT32 Offset,
							 LSA_UINT32 Length)
{
	LSA_UINT16 Count = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetDbElements(pAnchor: 0x%X, Length: 0x%X)", pAnchor, Length);

    if (OHA_IS_NOT_NULL(pAnchor))       /* get the element */
    {
        OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE);    /* starts with the first element */

        while (OHA_IS_NOT_NULL(pElement))
        {
			Count++;
			if(OHA_IS_NOT_NULL(pDbaseList))
			{
				OHA_ASSERT((Count+Offset) <= Length);	/* Option List not too small */
				*(pDbaseList+Offset) = DCP_WORD(pElement->SubType, pElement->Type);
				pDbaseList++;
			}
            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_ELEM_PTR_TYPE);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetDbElements(Count: 0x%X)", Count);
	return (Count);
}

/*=============================================================================
 * function name:  oha_update_supported_database
 * function:       writes the optional and manufacturer specific option/suboptions only

  Option IPOption (0x01)
      0x03          SuboptionFullIPSuite (not implemented)
   Option DevicePropertiesOption (0x02)
      0x07          SuboptionDeviceInstance
      0x08          SuboptionOEMDeviceID
   Option ManufacturerSpecificOption (0x80 - 0xFE)
      0x00 - 0xFF   SuboptionManufacturerSpecific

 * parameters:     NicId
                   OnlyOptionals 
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 oha_update_supported_database(
    LSA_UINT16                  NicId
)
{
    OHA_DB_IF_PTR_TYPE  pDbIf = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_OK;
	LSA_UINT32 Count;
	LSA_UINT32 Length = 0;
	OHA_COMMON_MEM_U16_PTR_TYPE pOptionList = LSA_NULL;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_update_supported_database(NicId: 0x%X)", NicId);

	while (1)
	{
		/*  (i) count option entries */
		/* (ii) write to option list */
		Count = 0;

		if(pDbIf->PNIdentValid.Existing) /* 832350: record written ? */
		{
			Count += 1;
			if(OHA_IS_NOT_NULL(pOptionList))
			{
				OHA_ASSERT(Count <= Length);	/* Option List not too small */
				*pOptionList = DCP_WORD(DCP_DSID_IDDEVINST, OHA_DCP_OPTION_IDENT);
			}
		}
		else
		{
			Count += OHA_GetDbElements (&pDbIf->DcpData.pDevInstance, pOptionList, Count, Length);
		}

		Count += OHA_GetDbElements (&pDbIf->DcpData.pOEMDeviceID, pOptionList, Count, Length);

		Count += OHA_GetDbElements (&pDbIf->DcpData.pPB_Common, pOptionList, Count, Length);
		Count += OHA_GetDbElements (&pDbIf->DcpData.pPB_Addition, pOptionList, Count, Length);
		Count += OHA_GetDbElements (&pDbIf->DcpData.pPB_Others, pOptionList, Count, Length);

		Length = Count;

		if(Length == 0)	/* no options -> delete option list */
		{
			oha_delete_DbElement (&pDbIf->DcpData.pDbaseList); /* 832350 */

			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_update_supported_database(). No option present.");
			return RetVal;
		}

		if(OHA_IS_NULL(pOptionList)) 		/* alloc option list */
		{
			OHA_LOCAL_MEM_PTR_TYPE  pMem = LSA_NULL;
			OHA_ALLOC_LOCAL_MEM(&pMem, 2*Length); /* alloc the option list (in words) */
			if ( LSA_HOST_PTR_ARE_EQUAL(pMem, LSA_NULL) )   /* failed the allocation */
			{
				OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_update_supported_database(). Allocating option list failed!");
				RetVal = LSA_RET_ERR_RESOURCE;
				return RetVal;
			}
			pOptionList = (OHA_COMMON_MEM_U16_PTR_TYPE)pMem;
		}
		else
		{
			break;
		}
	}

	if(OHA_IS_NOT_NULL(pOptionList))
	{
		LSA_UINT16   FreeStat = LSA_RET_OK;

		RetVal = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_IDENT, /* Length in WORDS */
										  DCP_DSID_IDOPTS, (OHA_COMMON_MEM_U8_PTR_TYPE)pOptionList,
										  (LSA_UINT16)(Length * 2), OHA_READ_ONLY, LSA_FALSE);

		OHA_FREE_LOCAL_MEM(&FreeStat, pOptionList);
		OHA_MEM_FREE_FAIL_CHECK(FreeStat);
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_update_supported_database(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_DCP_ELEMENT
 * function:       writes the parameters to the database
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
                   AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
                   Remanent (storage of variable in memory)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_DCP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT8                   SubType,    /* subtype of variable, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length,     /* length of variable in octets */
    LSA_UINT8                   AccCtrl,    /* access control for variable */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_DCP_ELEMENT(Type: 0x%X, SubType: 0x%X, Length: 0x%X, AccCtrl: 0x%X, Remanent: 0x%X)",
                          Type, SubType, Length, AccCtrl, Remanent);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType) ) /* invalid (sub)option */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_DCP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    if (Length > EDD_NRT_MAX_SND_LEN) /* EDD_IEEE_FRAME_LENGTH */  /* absolute maximum */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_DCP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }


    if (LSA_RET_OK != oha_check_access_control(AccCtrl)) /* no valid AcCtrl */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_DCP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    /* TIA 857716: Only DHCP suboption 61 supported */
    if (Type == OHA_DCP_OPTION_DHCP)
	{
		if (SubType != 61)
		{
			RetVal = LSA_RET_ERR_PARAM;
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,"DHCP suboption (0x%X) not supported!", SubType);
			OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_DCP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
			return RetVal;
		}
	}

    pAnchor = oha_get_db_element (NicId, Type, SubType);   /* get the anchor */
    if (OHA_IS_NOT_NULL(pAnchor))
    {                       /* find the element */
        /* OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);
        if (OHA_IS_NULL(pElement))     /* element existing ? */
        {                      /* create it ! */
            pElement = oha_create_DbElement(Type, SubType, pVariable, Length, AccCtrl, Remanent);
            if (OHA_IS_NOT_NULL(pElement))
            {
                OhaListInsertTail(pAnchor, &pElement->ListElem);        /* queue the element */
            }
            else
            {
                RetVal = LSA_RET_ERR_PARAM;
            }
        }
        else
        {
            RetVal = oha_update_DbElement(pElement, Type, SubType, pVariable, Length, AccCtrl, Remanent);
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    if (Type == OHA_DCP_OPTION_TCP && SubType == DCP_DSID_TCPIP)    /* OHA_DB_WRITE_IP_SUITE() */
    {
        OHA_ASSERT(RetVal == LSA_RET_OK && OHA_IS_NOT_NULL(pVariable) && Length == OHA_IP_SUITE_SIZE);
        /* write the (local) management address (IP) to the lldp database */
        (LSA_VOID)OHA_DbStoreMgmAddress(NicId, pVariable);
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_WRITE_DCP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}


/*=============================================================================
 * function name:  OHA_DB_WRITE_ACCESS_CONTROL
 * function:       writes the parameter AccessControl to the database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_DB_WRITE_ACCESS_CONTROL(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    LSA_UINT8                   AccCtrl    /* access control for variable */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_ACCESS_CONTROL(Type: 0x%X, SubType: 0x%X, AccCtrl: 0x%X)",
                          Type, SubType, AccCtrl);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_ACCESS_CONTROL(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    if (LSA_RET_OK != oha_check_access_control(AccCtrl)) /* no valid AcCtrl */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_ACCESS_CONTROL(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {                   /* find the element */
        /* OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NULL(pElement))     /* element existing ? */
        {                      /* create it ! */
            pElement = oha_create_DbElement(Type, SubType, 0, 0, AccCtrl, LSA_FALSE);
            if (OHA_IS_NOT_NULL(pElement))
            {
                OhaListInsertTail(pAnchor, &pElement->ListElem);        /* queue the element */
            }
            else
            {
                RetVal = LSA_RET_ERR_PARAM;
            }
        }
        if (OHA_IS_NOT_NULL(pElement))     /* element existing already ? */
        {
            (pElement)->AccCtrl = AccCtrl;
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_WRITE_ACCESS_CONTROL(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_write_remanence
 * function:       writes the parameter AccessControl to the database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   RemaStatus (LSA_TRUE, LSA_FALSE)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 oha_db_write_remanence(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    LSA_BOOL /* AP01430038 */   RemaStatus  /* rema for variable */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;
    LSA_BOOL Remanence = (RemaStatus) ? LSA_TRUE : LSA_FALSE;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_remanence(Type: 0x%X, SubType: 0x%X, RemaStatus: 0x%X)",
                          Type, SubType, RemaStatus);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_write_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {                   /* find the element */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NOT_NULL(pElement))     /* element existing already ? */
        {
            (pElement)->Remanent = Remanence;
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_write_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DbWriteUserControl
 * function:       writes the parameter AccessControl to the database-element
 * parameters:     NicId
                   UserHandle
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   UserControl (OHA_LOCK_SET, OHA_UNLOCK_SET)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_DbWriteUserControl(
    LSA_UINT16                  NicId,
    LSA_UINT16                  UserHandle,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    LSA_UINT8                   UserControl /* user control for variable */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DbWriteUserControl(UsrHandle: 0x%X, Type: 0x%X, SubType: 0x%X, UserControl: 0x%X)",
                          UserHandle, Type, SubType, UserControl);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DbWriteUserControl(NicId: 0x%X, Type: 0x%X, SubType: 0x%X)", NicId, Type, SubType);
        return RetVal;
    }

    if (UserHandle >= OHA_CFG_MAX_CHANNELS) /* invalid user handle */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DbWriteUserControl(NicId: 0x%X, UserHandle: 0x%X)", NicId, UserHandle);
        return RetVal;
    }

    if (!(UserControl == OHA_LOCK_SET || UserControl == OHA_UNLOCK_SET)) /* no valid UserControl */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DbWriteUserControl(NicId: 0x%X, UserControl: 0x%X)", NicId, UserControl);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {                           /* find the element */
        /* OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NULL(pElement))     /* element existing ? */
        {                      /* create it ! */
            pElement = oha_create_DbElement(Type, SubType, 0, 0, OHA_NO_ACCESS, LSA_FALSE);
            if (OHA_IS_NOT_NULL(pElement))
            {
                OhaListInsertTail(pAnchor, &pElement->ListElem);        /* queue the element */
            }
            else
            {
                RetVal = LSA_RET_ERR_RESOURCE;  /* no memory */
            }
        }
        if (OHA_IS_NOT_NULL(pElement))     /* element existing now ? */
        {
			switch (UserControl)
			{
				case OHA_LOCK_SET:
					if((pElement)->LockCnt[UserHandle] == 0xffff)
					{
						OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP, "OUT: OHA_DbWriteUserControl(NicId: 0x%X, LockCnt: 0x%X)", NicId, (pElement)->LockCnt[UserHandle]);
						RetVal = OHA_ERR_FAILED;
					}
					else
					{
						(pElement)->LockCnt[UserHandle]++;     /* finally write the user-control */
					}
				break;
				case OHA_UNLOCK_SET:
					if((pElement)->LockCnt[UserHandle] == 0)
					{
						OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP, "OUT: OHA_DbWriteUserControl(NicId: 0x%X, LockCnt: 0x%X)", NicId, (pElement)->LockCnt[UserHandle]);
						RetVal = OHA_ERR_FAILED;
					}
					else
					{
						(pElement)->LockCnt[UserHandle]--;     /* finally write the user-control */
					}
				break;
				default:
					OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				break;
			}
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DbWriteUserControl(NicId: 0x%X, no such element in the base)", NicId);
    }
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DbWriteUserControl(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_set_access
 * function:       reads the parameter AccessControl from the database
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   AccessControl (OHA_WRITE_ONLY, OHA_QUERY_WRITE,
                                  OHA_READ_WRITE, OHA_LOCK_SET)
 * return value:   NULL:    no remote DcpSet allowed
                   <>NULL:  AccessControl(OHA_WRITE_ONLY, OHA_QUERY_WRITE,
                                    OHA_READ_WRITE), a remote DcpSet is allowed
 *===========================================================================*/
LSA_UINT8 oha_db_read_set_access(LSA_UINT16 NicId,
                                 LSA_UINT8   Type,
                                 LSA_UINT8   SubType)
{
    OHA_DB_ELEM_PTR_TYPE pElement;
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT8 RetVal = 0;    /* no remote DcpSet allowed    */
    LSA_UINT8 AccCtrl;       /* access control for variable */

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_set_access(Type: 0x%X, SubType: 0x%X)",
                          Type, SubType);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_set_access(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        /* pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        pElement = oha_find_DbElement(pAnchor, Type, SubType);

        /* in case of no control-option allow the access */
        if (OHA_IS_NULL(pElement))              /* element existing ? */
        {
            if (Type == OHA_DCP_OPTION_CTRL)
            {
                RetVal = OHA_READ_WRITE;
            }
        }
        else
        {
            LSA_UINT16 i;
            LSA_UINT16 lock_cnt = 0;
            LSA_UINT16 usr_handle = 0;
            OHA_HDB_DCP_PTR_TYPE  pHDBDcp;

            pHDBDcp = OHA_DCPGetHandleByNIC(NicId); /* DCP channel necessary in case of a Query    */
            OHA_ASSERT_NULL_PTR(pHDBDcp);

            AccCtrl = pElement->AccCtrl;    /* read it ! */
            switch (AccCtrl)
            {
                case OHA_WRITE_ONLY:
                case OHA_QUERY_WRITE:
                case OHA_READ_WRITE:
                    RetVal = AccCtrl;
                    break;
                default:        /* not supported sevice - ignore    */
                    break;
            }

            for (i=0; i < OHA_CFG_MAX_CHANNELS; i++)    /* locked by any user ? */
            {
                if (pElement->LockCnt[i])     /* read it ! */
                {                       /* one of the users has locked */
                    usr_handle = i;
                    lock_cnt++;
                }
            }

            if (lock_cnt == 0)                  /* no user has locked       */
            {
            }
            else if (lock_cnt == 1 &&           /* only one user has locked */
                     OHA_IS_NOT_NULL(pHDBDcp->pHDBUserCtrl) &&  /* there is a control-user */
                     usr_handle == pHDBDcp->pHDBUserCtrl->ThisHandle)   /* and it's the query user */
            {
                OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "Only the query user has locked (NicId: 0x%X, UserHandle: 0x%X)", NicId, usr_handle);
            }
            else                /* one of the users has locked */
            {
                RetVal = 0;       /* no remote DcpSet allowed */
            }
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_set_access(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_user_lock
 * function:       reads the parameter UsrCtrl from the database
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   UsrCtrl (OHA_LOCK_SET delivers TRUE)
 * return value:   LSA_FALSE:   a User Set is allowed
                   LSA_TRUE:    no User Set is allowed (locked)
 *===========================================================================*/
LSA_BOOL oha_db_read_user_lock(
    LSA_UINT16 NicId,
    LSA_UINT16 UserHandle,
    LSA_UINT8 Type,
    LSA_UINT8 SubType)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_BOOL RetVal = LSA_FALSE;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_user_lock(UserHandle: 0x%X, Type: 0x%X, SubType: 0x%X)",
                          UserHandle, Type, SubType);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_user_lock(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        /* pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NOT_NULL(pElement))  /* element existing ? */
        {
            LSA_UINT16 i;
            LSA_UINT16 lock_cnt = 0;
            LSA_UINT16 usr_handle = 0;

            for (i=0; i < OHA_CFG_MAX_CHANNELS; i++)
            {
                if (pElement->LockCnt[i])     /* read it ! */
                {
                    usr_handle = i;
                    lock_cnt++;
                }
            }
            if (lock_cnt == 0)                 /* no user has locked       */
            {
				OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW, "No user has locked (NicId: 0x%X, UserHandle: 0x%X)", NicId, UserHandle);
				OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_user_lock(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
				return RetVal;
            }
            if (lock_cnt == 1)                 /* only one user has locked... */
			{
				OHA_HDB_DCP_PTR_TYPE  pHDBDcp = OHA_DCPGetHandleByNIC(NicId);

				if (usr_handle == UserHandle)  /* and it's the actual user, a set is allowed */
				{
					OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW, "Only the actual user has locked (NicId: 0x%X, UserHandle: 0x%X)", NicId, UserHandle);
					OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_user_lock(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
					return RetVal;
				}

				if (OHA_IS_NOT_NULL(pHDBDcp) &&  /* AP01446880: or it's the query user, a set is allowed */
					OHA_IS_NOT_NULL(pHDBDcp->pHDBUserCtrl) &&
					pHDBDcp->pHDBUserCtrl->ThisHandle == UserHandle)
				{
					OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW, "Only the query user has locked (NicId: 0x%X, UserHandle: 0x%X)", NicId, UserHandle);
					OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_user_lock(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
					return RetVal;
				}
			}

			RetVal = LSA_TRUE;       /* one or more than one of the users has locked */
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_user_lock(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_query_set_access
 * function:       writes the parameter AccessControl to the database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   AccessControl (OHA_QUERY_WRITE or OHA_LOCK_SET delivers TRUE)
 * return value:   LSA_FALSE:   no remote DcpSet allowed
                   LSA_TRUE:    a remote DcpSet is allowed
 *===========================================================================*/
LSA_BOOL oha_db_read_query_set_access(
    LSA_UINT16    NicId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_BOOL RetVal = LSA_FALSE;
    LSA_UINT8 AccCtrl;    /* access control for variable */

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_query_set_access(Type: 0x%X, SubType: 0x%X)",
                          Type, SubType);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_query_set_access(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        /* pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NOT_NULL(pElement))            /* element existing ? */
        {
            LSA_UINT16 i;
            LSA_UINT16 lock_cnt = 0;
            LSA_UINT16 usr_handle = 0;
            OHA_HDB_DCP_PTR_TYPE  pHDBDcp;

            pHDBDcp = OHA_DCPGetHandleByNIC(NicId); /* DCP channel necessary in case of a Query    */
            OHA_ASSERT_NULL_PTR(pHDBDcp);

            AccCtrl = pElement->AccCtrl;    /* read it ! */
            if (AccCtrl == OHA_QUERY_WRITE)
            {
                RetVal = LSA_TRUE;
            }
            for (i=0; i < OHA_CFG_MAX_CHANNELS; i++)
            {
                if (pElement->LockCnt[i])     /* read it ! */
                {
                    usr_handle = i;
                    lock_cnt++;
                }
            }

            if (lock_cnt == 0)                  /* no user has locked       */
            {
            }
            else if (lock_cnt == 1 &&           /* only one user has locked */
                     OHA_IS_NOT_NULL(pHDBDcp->pHDBUserCtrl) &&  /* there is a control-user */
                     usr_handle == pHDBDcp->pHDBUserCtrl->ThisHandle)   /* and it's the query user */
            {
                OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "Only the query user has locked (NicId: 0x%X, UserHandle: 0x%X)", NicId, usr_handle);
            }
            else
            {
                RetVal = LSA_FALSE;       /* one of the users has locked */
            }
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_query_set_access(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DB_READ_ACCESS_CONTROL
 * function:       reads the parameter AccessControl of the database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   pAccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_DB_READ_ACCESS_CONTROL(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    LSA_UINT8 *                 pAccCtrl    /* access control for variable */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_ACCESS_CONTROL(Type: 0x%X, SubType: 0x%X, pAccCtrl: 0x%X)",
                          Type, SubType, pAccCtrl);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_ACCESS_CONTROL(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        /* OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NULL(pElement))     /* element existing ? */
        {                      /* read it ! */
            RetVal = LSA_RET_ERR_PARAM;
        }
        else
        {
            *pAccCtrl = pElement->AccCtrl;
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_ACCESS_CONTROL(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DB_READ_LOCK_STATUS
 * function:       reads the lock status of a database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
* return value:    LSA_FALSE: no user lock (a Dcp-Set is allowed)
                   LSA_TRUE:  locked by any user (a Dcp-Set is not allowed)
*===========================================================================*/
LSA_BOOL OHA_DB_READ_LOCK_STATUS(
    LSA_UINT16      NicId,
    LSA_UINT8       Type,
    LSA_UINT8       SubType
)
{
    LSA_BOOL LockStatus;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "IN: OHA_DB_READ_LOCK_STATUS(NicId: 0x%X, Type: 0x%X, SubType: 0x%X)", NicId, Type, SubType);
    LockStatus = oha_db_read_user_lock(NicId, OHA_INVALID_HANDLE, Type, SubType);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_LOCK_STATUS(LockStatus: 0x%X)", LockStatus);
    return (LockStatus);
}

/*=============================================================================
 * function name:  OHA_DB_RESET_LOCK_STATUS
 * function:       deletes all user locks of a database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
* return value:    LSA_RET_OK, LSA_RET_ERR_PARAM or LSA_RET_ERR_RESOURCE
*===========================================================================*/
LSA_UINT16 OHA_DB_RESET_LOCK_STATUS(
    LSA_UINT16      NicId,
    LSA_UINT8       Type,
    LSA_UINT8       SubType
)
{
    LSA_UINT16 RetVal = LSA_RET_OK;
    LSA_UINT16 i;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "IN: OHA_DB_RESET_LOCK_STATUS(NicId: 0x%X, Type: 0x%X, SubType: 0x%X)", NicId, Type, SubType);
    for (i = 0; i < OHA_CFG_MAX_CHANNELS; i++)
    {
        LSA_UINT16 Ret = OHA_DbWriteUserControl(NicId, i, Type, SubType, OHA_UNLOCK_SET);
        if (Ret != LSA_RET_OK)
        {
            RetVal = Ret;
        }
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_RESET_LOCK_STATUS(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_READ_DCP_ELEMENT
 * function:       reades the parameters of the database element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
                   AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
                   Remanent (storage of variable in memory)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_DCP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,        /* type of variable, option */
    LSA_UINT8                   SubType,     /* subtype of variable, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppVariable, /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength,     /* length of variable in octets */
    OHA_COMMON_MEM_U8_PTR_TYPE  pAccCtrl,    /* access control for variable */
    LSA_BOOL                  * pRemanent    /* storage of variable in memory */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_DCP_ELEMENT(Type: 0x%X, SubType: 0x%X, pLength: 0x%X, pAccCtrl: 0x%X, pRemanent: 0x%X)",
                          Type, SubType, pLength, pAccCtrl, pRemanent);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_DCP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        /* OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NULL(pElement))     /* element does not exist */
        {
            RetVal = LSA_RET_ERR_PARAM;
        }
        else
        {
            *ppVariable = pElement->pVariable;
            *pLength    = pElement->Length;
            if (OHA_IS_NOT_NULL(pAccCtrl))	/* 205580 */
            {
                *pAccCtrl   = pElement->AccCtrl;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent  = pElement->Remanent;
            }
        }
    }
    else    /* no such element in DB */
    {
        RetVal = LSA_RET_ERR_PARAM;
    }
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_DCP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeDBinterface                         +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Database elements if allocated.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeDbInterface(OHA_DB_IF_PTR_TYPE   pDbIf)
{
    LSA_UINT16           i, j;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FreeDbInterface(pDbIf: 0x%X)", pDbIf);

    /* free allocated DCP-data entries */
    oha_delete_DbElement (&pDbIf->pStationName);
    oha_remove_DbElement (pDbIf->pTempStationName);
    pDbIf->pTempStationName = LSA_NULL;
    oha_delete_DbElement (&pDbIf->DcpData.pDbaseList);
    oha_delete_DbElement (&pDbIf->DcpData.pLocalMacAddr);
    oha_delete_DbElement (&pDbIf->DcpData.pPB_Common);
    oha_delete_DbElement (&pDbIf->DcpData.pPB_Addition);
    oha_delete_DbElement (&pDbIf->DcpData.pPB_Others);
    oha_delete_DbElement (&pDbIf->DcpData.pControlOptions);
    oha_delete_DbElement (&pDbIf->DcpData.pDhcpOptions);

    oha_delete_DbElement (&pDbIf->DcpData.pStationType);
    oha_delete_DbElement (&pDbIf->DcpData.pStationId);
    oha_delete_DbElement (&pDbIf->DcpData.pProfinetRole);
    oha_delete_DbElement (&pDbIf->DcpData.pDevInstance);
    oha_delete_DbElement (&pDbIf->DcpData.pOEMDeviceID);

    oha_delete_DbElement (&pDbIf->DcpData.pLocalIpSuite);
    oha_remove_DbElement (pDbIf->DcpData.pTempIpSuite);
    pDbIf->DcpData.pTempIpSuite = LSA_NULL;
    oha_remove_DbElement (pDbIf->DcpData.pRemaIpSuite);
    pDbIf->DcpData.pRemaIpSuite = LSA_NULL;
    oha_remove_DbElement (pDbIf->DcpData.pExpectedIpSuite);
    pDbIf->DcpData.pExpectedIpSuite = LSA_NULL;
    oha_remove_DbElement (pDbIf->DcpData.pEmergencyIpSuite);
    pDbIf->DcpData.pEmergencyIpSuite = LSA_NULL;
    oha_delete_DbElement (&pDbIf->DcpData.pDeviceInitiative);

    /* free allocated LLDP-data entries */
    oha_delete_lldp_DbElement(&pDbIf->pTTL, LSA_NULL);
    oha_delete_lldp_DbElement(&pDbIf->pMgmAddr, LSA_NULL);
    oha_delete_lldp_DbElement(&pDbIf->pIfDescr_0, LSA_NULL);

    for (i = 0; i < OHA_CFG_MAX_PORTS; i++)  /* over all ports */
    {
        oha_delete_lldp_DbElement(&pDbIf->LldpData[i].pPortId, LSA_NULL);
        oha_delete_lldp_DbElement(&pDbIf->LldpData[i].pIfDescr, LSA_NULL);

        oha_delete_lldp_DbElement(&pDbIf->LldpData[i].pOrgSpec_Others, LSA_NULL);
        oha_delete_lldp_DbElement(&pDbIf->LldpData[i].pUnknown_Optionals, LSA_NULL);
        oha_delete_lldp_DbElement(&pDbIf->LldpData[i].pOrgSpec_802_1, LSA_NULL);
        oha_delete_lldp_DbElement(&pDbIf->LldpData[i].pOrgSpec_802_3, LSA_NULL);
        oha_delete_lldp_DbElement(&pDbIf->LldpData[i].pOrgSpec_Pno, LSA_NULL);

		if(OHA_IS_NOT_NULL(pDbIf->LldpData[i].pLldpPortID))
		{
	        LSA_UINT16   FreeStat = LSA_RET_OK;
            OHA_FREE_LOCAL_MEM(&FreeStat, pDbIf->LldpData[i].pLldpPortID);
            OHA_MEM_FREE_FAIL_CHECK(FreeStat);
            pDbIf->LldpData[i].pLldpPortID = LSA_NULL;
		}

        for (j = 0; j < LLDP_MAX_PORT_SENDER; j++)  /* over all ports */
        {
            LSA_UINT16  CountOfDeletedElem = 0; /* count statistics */
            OHA_LLDP_SENDER_PTR_TYPE pRemoteSender = &pDbIf->LldpData[i].RemoteSender[j];

            OHA_FreeRemoteSender(pRemoteSender, &CountOfDeletedElem);

            /* count statistics */
            pDbIf->LldpData[i].PortStats.RxPortTLVsDiscardedTotal += CountOfDeletedElem;
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_FreeDbInterface()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeDatabase                            +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Database elements if allocated.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeDataBase(LSA_VOID)
{
    OHA_DB_IF_PTR_TYPE   pDB  = &oha_database.IfData[0];
    LSA_UINT32           i    = 0;

    /* +++ first the device-global elements */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "IN : OHA_FreeDataBase()");

    oha_delete_lldp_DbElement(&oha_database.StationData.pChassisId, LSA_NULL); /* NameOfDevice */
    oha_delete_lldp_DbElement(&oha_database.StationData.pSysName, LSA_NULL);  /* system - name */
    oha_delete_lldp_DbElement(&oha_database.StationData.pSysDescr, LSA_NULL); /* system - description */
    oha_delete_lldp_DbElement(&oha_database.StationData.pSysCap, LSA_NULL);   /* system - capabilities */

    oha_delete_snmp_DbElement(&oha_database.StationData.pSysLocation, LSA_NULL); /* system - location */
    oha_delete_snmp_DbElement(&oha_database.StationData.pSysContact, LSA_NULL); /* system - contact */

    while ( i < OHA_CFG_MAX_NICS )
    {
        if (pDB->NicId != OHA_INVALID_NIC_ID)   /* used interface */
        {
            OHA_FreeDbInterface(pDB);
        }
        pDB++;
        i++;
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_FreeDataBase()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UnlockDbUser                            +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Unlocks this User for all Database elements               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UnlockDbUser(OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UnlockDbUser(pHDBUser: 0x%X)", pHDBUser);

    if (OHA_IS_NOT_NULL(pHDBUser))
    {
        LSA_UINT32           i;
        OHA_DB_IF_PTR_TYPE pDbIf = &oha_database.IfData[0];

        for (i=0; i < OHA_CFG_MAX_NICS; pDbIf++, i++)
        {
            if (pDbIf->NicId != OHA_INVALID_NIC_ID)   /* used interface */
            {
                /* unlock DCP-data entries */
                oha_unlock_DbElement(&pDbIf->pStationName, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pDbaseList, pHDBUser->ThisHandle);

				/* AP01270147: OHA doesn't release all if a channel is closed */
		        oha_unlock_DbElement(&pDbIf->DcpData.pStationType, pHDBUser->ThisHandle);
		        oha_unlock_DbElement(&pDbIf->DcpData.pStationId, pHDBUser->ThisHandle);
		        oha_unlock_DbElement(&pDbIf->DcpData.pProfinetRole, pHDBUser->ThisHandle);
		        oha_unlock_DbElement(&pDbIf->DcpData.pDevInstance, pHDBUser->ThisHandle);
		        oha_unlock_DbElement(&pDbIf->DcpData.pOEMDeviceID, pHDBUser->ThisHandle);

                oha_unlock_DbElement(&pDbIf->DcpData.pLocalMacAddr, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pPB_Common, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pPB_Addition, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pPB_Others, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pControlOptions, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pDhcpOptions, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pLocalIpSuite, pHDBUser->ThisHandle);
                oha_unlock_DbElement(&pDbIf->DcpData.pDeviceInitiative, pHDBUser->ThisHandle);
            }
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UnlockDbUser()");
    return;
}

/*=============================================================================
 * function name:  oha_db_read_get_access
 * function:       reads the parameter AccessControl from the database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY, OHA_QUERY_WRITE,
                                  OHA_READ_WRITE delivers TRUE)
 * return value:   NULL:    no remote DcpGet allowed
                   <>NULL:  AccessControl(OHA_WRITE_ONLY, OHA_QUERY_WRITE,
                                    OHA_READ_WRITE), a remote DcpGet is allowed
 *===========================================================================*/
LSA_UINT8 oha_db_read_get_access(
    LSA_UINT16  NicId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT8 RetVal = 0;
    LSA_UINT8 AccCtrl;    /* access control for variable */

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_get_access(Type: 0x%X, SubType: 0x%X)",
                          Type, SubType);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || (!SubType)) /* invalid (sub)option or no element */
    {
        RetVal = 0;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_get_access(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_db_element (NicId, Type, SubType);
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        /* OHA_DB_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE); */
        OHA_DB_ELEM_PTR_TYPE pElement = oha_find_DbElement(pAnchor, Type, SubType);

        if (OHA_IS_NOT_NULL(pElement))     /* element existing ? */
        {                      /* read it ! */
            AccCtrl = pElement->AccCtrl;
            switch (AccCtrl)
            {
                case OHA_READ_ONLY:
                case OHA_WRITE_ONLY:
                case OHA_QUERY_WRITE:
                case OHA_READ_WRITE:
                    RetVal = AccCtrl;
                    break;
                default:        /* not supported service - ignore   */
                    break;
            }
        }
    }
    else
    {
        RetVal = 0;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_get_access(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_store_temp_ip_address
 * function:       temporary store of an IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 *                 OHA_IP_ADDR IpAddrPtr:               Pointer to IP address
 *                 LSA_INT    AddrLen:                  Length of address
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_temp_ip_address(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr,
    LSA_INT                       AddrLen)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_store_ip_address(NicId: 0x%X, IpAddrPtr: 0x%X, AddrLen: 0x%X)",
                          NicId, IpSuitePtr, AddrLen);

    if (OHA_IS_NOT_NULL(pDB) && (AddrLen == OHA_IP_SUITE_SIZE) &&
        OHA_IS_NOT_NULL(IpSuitePtr) )
    {
        /* IpAddr, SubnetMask, RouterAddr */
        if (oha_is_valid_ip_address(OHA_GET32_NTOH(IpSuitePtr,0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE),0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE),0),
                                    LSA_FALSE))
        {
            OHA_DB_ELEM_PTR_TYPE pElement = pDB->DcpData.pTempIpSuite;

            if (OHA_IS_NULL(pElement))      /* element not existing */
            {                      /* create it ! */
                pElement = oha_create_DbElement(OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, OHA_READ_WRITE, LSA_FALSE);
                if (OHA_IS_NOT_NULL(pElement))
                {
                    pDB->DcpData.pTempIpSuite = pElement;       /* store the element */
                    RetVal = LSA_RET_OK;
                }
            }
            else
            {
                RetVal = oha_update_DbElement(pElement, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, pElement->AccCtrl, pElement->Remanent);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_store_ip_address(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_temp_ip_address
 * function:       read the temporary stored IP address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              Length
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_ip_address(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pAccCtrl,    /* access control for variable */
    LSA_BOOL                      * pRemanent )   /* storage of variable in memory */
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = 0;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_temp_ip_address(NicId: 0x%X, IpAddrPtrPtr: 0x%X)",
                          NicId, IpSuitePtrPtr);

    /* element exists */
    if (OHA_IS_NOT_NULL(pDB) && OHA_IS_NOT_NULL(pDB->DcpData.pTempIpSuite))
    {
        OHA_DB_ELEM_PTR_TYPE pReal = LSA_NULL;
        OHA_DB_ELEM_PTR_TYPE pTemp;

        pAnchor = oha_get_db_element (NicId, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP);    /* get the anchor */
        if (OHA_IS_NOT_NULL(pAnchor))
        {                       /* find the element */
            pReal = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE);
        }

        pTemp = pDB->DcpData.pTempIpSuite;

        if (OHA_IS_NOT_NULL(pTemp->pVariable) && pTemp->Length == OHA_IP_SUITE_SIZE)
        {
            /* IpAddr, SubnetMask, RouterAddr */
            *IpSuitePtrPtr = pTemp->pVariable;
            RetVal = OHA_IP_SUITE_SIZE;
        }
        else
        {
            *IpSuitePtrPtr = LSA_NULL;
        }

        /* write actual access control and remanent from existing database entry */
        if (OHA_IS_NOT_NULL(pReal))     /* element exists */
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl = pReal->AccCtrl;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent  = pReal->Remanent;
            }
        }
        else
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl   = OHA_NO_ACCESS;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent  = LSA_FALSE;
            }
        }
    }
    else
    {
        *IpSuitePtrPtr = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_read_temp_ip_address(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_read_temp_station_name
 * function:       Get pointer to and length of station name string
 * parameters:     OHA_SYS_BYTE_PTR_TYPE * NamePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  oha_db_read_temp_station_name(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE        * NamePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE          pAccCtrl,    /* access control for variable */
    LSA_BOOL                          * pRemanent )   /* storage of variable in memory */
{
    LSA_INT length;
    OHA_DB_IF_PTR_TYPE pDbIf = OHA_DB_GET_INTERFACE(NicId);

    OHA_DB_ELEM_PTR_TYPE pTemp;
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag) * pAnchor;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_temp_station_name(NicId: 0x%X, NamePtrPtr: 0x%X)",
                          NicId, NamePtrPtr);

    OHA_ASSERT_NULL_PTR(pDbIf);

    pTemp = pDbIf->pTempStationName;

    if (OHA_IS_NOT_NULL(pTemp))     /* element exists */
    {
        OHA_DB_ELEM_PTR_TYPE pReal = LSA_NULL;

        pAnchor = oha_get_db_element (NicId, DCP_DBID_IDENT, DCP_DSID_IDNAME);    /* get the anchor */
        if (OHA_IS_NOT_NULL(pAnchor))
        {                       /* find the element */
            pReal = OhaListFirst(pAnchor, OHA_DB_ELEM_PTR_TYPE);
        }

        *NamePtrPtr = pTemp->pVariable;
        length      = pTemp->Length;

        /* write actual access control and remanent from existing database entry */
        if (OHA_IS_NOT_NULL(pReal))     /* element exists */
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl   = pReal->AccCtrl;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent  = pReal->Remanent;
            }
        }
        else
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl   = OHA_NO_ACCESS;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent  = LSA_FALSE;
            }
        }
    }
    else    /* no such element in DB */
    {
        *NamePtrPtr = LSA_NULL;
        length = 0;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_read_temp_station_name(Length: 0x%X)", length);
    return (length);
}

/*=============================================================================
 * function name:  oha_db_delete_temp_ip_address
 * function:       read the temporary stored IP address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              LSA_RET_OK, LSA_RET_ERR_RESOURCE
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_temp_ip_address(
    LSA_UINT16                      NicId)
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_temp_ip_address(NicId: 0x%X)", NicId);

    /* element exists */
    if (OHA_IS_NULL(pDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                              "OUT: oha_db_delete_temp_ip_address(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    if (OHA_IS_NOT_NULL(pDB->DcpData.pTempIpSuite))
    {
        /* delete temporary ip object */
        oha_remove_DbElement (pDB->DcpData.pTempIpSuite);
        pDB->DcpData.pTempIpSuite = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_temp_ip_address(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_delete_temp_station_name
 * function:       read the temporary stored NameOfStation
 * parameters:     LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              LSA_RET_OK, LSA_RET_ERR_RESOURCE
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_temp_station_name(
    LSA_UINT16                      NicId)
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_temp_station_name(NicId: 0x%X)", NicId);

    /* element exists */
    if (OHA_IS_NULL(pDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                              "OUT: oha_db_delete_temp_station_name(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    if (OHA_IS_NOT_NULL(pDB->pTempStationName))
    {
        /* delete temporary ip object */
        oha_remove_DbElement (pDB->pTempStationName);
        pDB->pTempStationName = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_temp_station_name(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_store_temp_station_name
 * function:       Set station name (database 8, dataset 2)
 * parameters:     LSA_UINT8    *   NamePtr:    Ptr to name string
 *                 LSA_UINT16       NameLen:    Length of name string
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station name
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_temp_station_name(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_LOCAL_MEM_ATTR* NamePtr,
    LSA_UINT16                    NameLen)
{
    LSA_UINT16  RetVal = LSA_RET_OK;
    OHA_DB_IF_PTR_TYPE pDbIf = OHA_DB_GET_INTERFACE(NicId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_store_temp_station_name(NicId: 0x%X, NamePtr: 0x%X, NameLen: 0x%X)",
                          NicId, NamePtr, NameLen);

    OHA_ASSERT_NULL_PTR(pDbIf);

    if (NameLen > OHA_MAX_STATION_NAME_LEN)
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                              "OUT: oha_db_store_temp_station_name(RetVal: 0x%X)", RetVal);
        return RetVal;
    }

    if (NameLen > 0)
    {
        /* check, if it's a RFC-name */
        if (!oha_is_valid_station_name((LSA_UINT16)NameLen, NamePtr, OHA_UPPER_CASES_ALLOWED(NicId), LSA_FALSE))
        {
            RetVal = LSA_RET_ERR_PARAM;
        }
    }

    /* NameLen = 0 shall delete the NameOfStation */
    if (RetVal == LSA_RET_OK)
    {
        OHA_DB_ELEM_PTR_TYPE pElement = pDbIf->pTempStationName;
        if (OHA_IS_NULL(pElement))     /* element existing ? */
        {                      /* create it ! */
            pElement = oha_create_DbElement(DCP_DBID_IDENT, DCP_DSID_IDNAME, NamePtr, NameLen, OHA_READ_WRITE, LSA_FALSE);
            if (OHA_IS_NOT_NULL(pElement))
            {
                pDbIf->pTempStationName = pElement;     /* store the element */
            }
            else
            {
                RetVal = LSA_RET_ERR_PARAM;
            }
        }
        else
        {
            RetVal = oha_update_DbElement(pElement, DCP_DBID_IDENT, DCP_DSID_IDNAME, NamePtr, NameLen, pElement->AccCtrl, pElement->Remanent);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_store_temp_station_name(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_compare_remanence
 * function:       reads the parameter Remanence from the database
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
 * return value:   LSA_FALSE:   remanence is different
                   LSA_TRUE:    remanence is identical
 *===========================================================================*/
LSA_BOOL oha_db_compare_remanence(
    LSA_UINT16 NicId,
    LSA_UINT8  Type,
    LSA_UINT8  SubType,
    LSA_UINT16 DcpStatus)
{
    LSA_UINT16           Response;
    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
    LSA_UINT16           Length = 0;
    LSA_BOOL             RetVal = LSA_FALSE;    /* remanence is different */
    LSA_BOOL             Remanence = LSA_FALSE;
    LSA_UINT8            AccCtrl;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_compare_remanence(Type: 0x%X, SubType: 0x%X, DcpStatus: 0x%X)",
                          Type, SubType, DcpStatus);

    Response = OHA_DB_READ_DCP_ELEMENT(NicId,
                                       Type,       /* type of variable, option */
                                       SubType,    /* subtype of variable, suboption */
                                       &pVar,      /* variable in octets */
                                       &Length,    /* length of variable in octets */
                                       &AccCtrl,   /* access control for variable */
                                       &Remanence);/* storage of variable in memory */

    /* element does not exist or an error occurred */
    if (Response != LSA_RET_OK)
    {
        /* remanence is different */
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_compare_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    /* Is remanence of the DCP-parameter in the Status demanded? */
    if ((DcpStatus & DCP_MASK_DS_REMANENT) != 0)
    {
        if (Remanence)
        {
            /* remanence is identical */
            RetVal = LSA_TRUE;
            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_compare_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
            return RetVal;
        }
    }

    /* Is no remanence of the DCP-parameter in the Status demanded? */
    if ((DcpStatus & DCP_MASK_DS_REMANENT) == 0)
    {
        if (!Remanence)
        {
            /* remanence is identical */
            RetVal = LSA_TRUE;
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_compare_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestAddressIndication                +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_ADDR_INFO_IND          +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.Address                                 +*/
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
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestAddressIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestAddressIndication(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

    NICId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId); /* only EDD channel for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_ADDR_INFO_IND failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else if (!(pHDB->State == OHA_HANDLE_STATE_OPEN &&
               LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) ) )
    {           /* is no open/close user-channel runnning ? */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_ADDR_INFO_IND failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)", NICId);
    }
    else
    {   /* only one RQB for one NICId the same time */
        OHA_ASSERT(OHA_HANDLE_IDX(pHDBEdd->ThisHandle) < OHA_CFG_MAX_NICS);
        if (OHA_IS_NOT_NULL(pHDB->pAddressInd[OHA_HANDLE_IDX(pHDBEdd->ThisHandle)]))
        {
            Response = OHA_ERR_SEQUENCE;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP, "SequenceError: only one RQB for one NICId (0x%X) allowed!", NICId);
        }
        else
        {
            /* store the IndRQB */
            pHDB->pAddressInd[OHA_HANDLE_IDX(pHDBEdd->ThisHandle)] = pRQB;

            /* if this is the first Indication Resource, we check if an indication is pending */
            (LSA_VOID)OHA_AddressIndicateEvent(NICId, pHDB);
        }
    }

    if (Response != OHA_OK)
    {
        OHA_UserRequestFinish(pHDB,pRQB,Response);
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestAddressIndication(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_AddressIndicateEvent                    +*/
/*+  Input/Output          :    LSA_UINT32                   IfId           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DIAG-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_AddressIndicateEventAllUser(
    LSA_UINT16                              NICId
)
{
    OHA_HDB_USER_PTR_TYPE pHDBUser;
    LSA_UINT16 i;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_AddressIndicateEventAllUser(NICId: 0x%X)",
                          NICId);

    (LSA_VOID)OHA_SOCKSetNameAddress(NICId);

    for (pHDBUser = OHA_GET_HUSER_PTR(0),i=0; i < OHA_CFG_MAX_CHANNELS; pHDBUser++,i++)
    {
        if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
        {
			/* AP01166830: the TOPO-indication for all Nics */
			(LSA_VOID)OHA_TopoIndicateInterface(pHDBUser, LSA_FALSE);

			/* AP01022057: confirm only the user with the same Nic */
			if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
	            (LSA_VOID)OHA_AddressIndicateEvent(NICId, pHDBUser);/* address changed - indication */
			}
		}
    }

#ifndef OHA_CFG_NO_REMA
    (LSA_VOID)oha_rema_station_update(NICId);
#endif

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_AddressIndicateEventAllUser()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_AddressIndicateEvent                    +*/
/*+  Input/Output          :    LSA_UINT32                   IfId           +*/
/*+                             OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DIAG-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AddressIndicateEvent(
    LSA_UINT16                              NICId,
    OHA_HDB_USER_PTR_TYPE                   pHDB
)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                  RetVal = LSA_RET_OK;
    OHA_DB_IF_PTR_TYPE          pDB    = OHA_DB_GET_INTERFACE(NICId);
    OHA_HDB_EDD_PTR_TYPE        pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_AddressIndicateEvent(NICId: 0x%X, pHDB: 0x%X)",
                          NICId, pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDBEdd);

    /* only one RQB for one NICId the same time */
    OHA_ASSERT(OHA_HANDLE_IDX(pHDBEdd->ThisHandle) < OHA_CFG_MAX_NICS);
    pRQB = pHDB->pAddressInd[OHA_HANDLE_IDX(pHDBEdd->ThisHandle)];

    if (OHA_IS_NULL(pRQB) || OHA_IS_NULL(pDB))  /* resource provided ? */
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_AddressIndicateEvent(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    /* is no open/close user-channel runnning ? */
    if (!(pHDB->State == OHA_HANDLE_STATE_OPEN &&
          LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) ) )
    {
        RetVal = LSA_RET_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_AddressIndicateEvent failed. User Channel is closing or not open for this Interface (If-ID: 0x%X)", pDB->InterfaceID);
    }
    else
    {
        OHA_UPPER_ADDR_INFO_PTR_TYPE pAddress = &pRQB->args.AddrInfo;
        LSA_BOOL    IpAddrChanged        = LSA_FALSE;
        LSA_BOOL    NameOfStationChanged = LSA_FALSE;
        LSA_BOOL    FactoryResetChanged  = LSA_FALSE;

        OHA_ASSERT_NULL_PTR(pAddress);

        {
            OHA_COMMON_MEM_U8_PTR_TYPE IpAddrPtr         = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE IpMaskPtr         = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE GatewayPtr        = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pExpectedIp       = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pExpectedIpMask   = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pExpectedGateway  = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pEmergencyIp      = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pEmergencyIpMask  = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE pEmergencyGateway = LSA_NULL;

            LSA_BOOL IpIsTheSame;
            LSA_BOOL CurrentIPSuiteIsEmergency;

            (LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &IpAddrPtr, &IpMaskPtr, &GatewayPtr); /* local IP-Address (RealIP) */
            OHA_ASSERT_NULL_PTR(IpAddrPtr);
            OHA_ASSERT_NULL_PTR(IpMaskPtr);
            OHA_ASSERT_NULL_PTR(GatewayPtr);

            (LSA_VOID)oha_db_read_expected_ip_suite(NICId, &pExpectedIp, LSA_NULL, LSA_NULL);
            if (OHA_IS_NOT_NULL(pExpectedIp))
            {
                pExpectedIpMask  = pExpectedIp + OHA_IP_ADDR_SIZE;
                pExpectedGateway = pExpectedIp + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE;
            }

            (LSA_VOID)oha_db_read_emergency_ip_suite(NICId, &pEmergencyIp, LSA_NULL, LSA_NULL);
            if (OHA_IS_NOT_NULL(pEmergencyIp))
            {
                pEmergencyIpMask  = pEmergencyIp + OHA_IP_ADDR_SIZE;
                pEmergencyGateway = pEmergencyIp + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE;
            }

            /* compare RealIP */
            IpIsTheSame = oha_ipcmp(pAddress->CurrentIPSuite.IpAddr,
                                    pAddress->CurrentIPSuite.NetMask,
                                    pAddress->CurrentIPSuite.Gateway,
                                    IpAddrPtr,
                                    IpMaskPtr,
                                    GatewayPtr);

            if (IpIsTheSame)
            {
                if ((!pAddress->CurrentIPSuiteConflictDetected && pDB->DcpData.IpConflictDetected) ||
                    (pAddress->CurrentIPSuiteConflictDetected && !pDB->DcpData.IpConflictDetected))
                {
                    IpIsTheSame = LSA_FALSE;
                }
            }

            /* compare ExpectedIp */
            if (IpIsTheSame)
            {
                IpIsTheSame = oha_ipcmp(pAddress->ExpectedIPSuite.IpAddr,
                                        pAddress->ExpectedIPSuite.NetMask,
                                        pAddress->ExpectedIPSuite.Gateway,
                                        pExpectedIp,
                                        pExpectedIpMask,
                                        pExpectedGateway);
            }

            /* compare EmergencyIp */
            if (IpIsTheSame)
            {
                IpIsTheSame = oha_ipcmp(pAddress->EmergencyIPSuite.IpAddr,
                                        pAddress->EmergencyIPSuite.NetMask,
                                        pAddress->EmergencyIPSuite.Gateway,
                                        pEmergencyIp,
                                        pEmergencyIpMask,
                                        pEmergencyGateway);
            }

            /* compare flag for CurrentIp */
            
			/* AP01270055: OHA_ADDR_INFO_TYPE: Missing information by AddressConflict of an EmergencyIP */
			CurrentIPSuiteIsEmergency = pHDBEdd->SetAddrInfo.CurrentIPSuiteIsEmergency;

            if (IpIsTheSame)
            {
                if ((!pAddress->CurrentIPSuiteIsEmergency && CurrentIPSuiteIsEmergency) ||
                    (pAddress->CurrentIPSuiteIsEmergency && !CurrentIPSuiteIsEmergency))
                {
                    IpIsTheSame = LSA_FALSE;
                }
            }

            if (IpIsTheSame) /* AP01462749: rema status added */
            {
				IpIsTheSame = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, ( pAddress->IpSuiteRemanent ) ? DCP_MASK_DS_REMANENT : 0);
            }

            /* ... if not the same, copy and mark */
            if (!IpIsTheSame)
            {
				LSA_BOOL IsTheSameForTrace;

				/* AP00850530 tracing in case of changes */
				IsTheSameForTrace = oha_ipcmp(pAddress->CurrentIPSuite.IpAddr,
												pAddress->CurrentIPSuite.NetMask,
												pAddress->CurrentIPSuite.Gateway,
												IpAddrPtr, IpMaskPtr, GatewayPtr);

				if(!IsTheSameForTrace) /* AP00850530 */
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,
										 "OHA-AddrInfoInd(NICId=0x%X): CurrentIpSuite is different.",NICId);
				}

				/* current IP */
                OHA_MEMCPY(pAddress->CurrentIPSuite.IpAddr,  IpAddrPtr, OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pAddress->CurrentIPSuite.NetMask, IpMaskPtr, OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pAddress->CurrentIPSuite.Gateway, GatewayPtr, OHA_IP_ADDR_SIZE);


				/* AP00850530 tracing in case of changes */
				if ((!pAddress->CurrentIPSuiteConflictDetected && pDB->DcpData.IpConflictDetected) ||
					 (pAddress->CurrentIPSuiteConflictDetected && !pDB->DcpData.IpConflictDetected))
				{
					OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,
										 "OHA-AddrInfoInd(NICId=0x%X): IpConflictDetected(0x%X) differs (0x%X).",
										 NICId, pDB->DcpData.IpConflictDetected, pAddress->CurrentIPSuiteConflictDetected);
				}

                pAddress->CurrentIPSuiteConflictDetected = pDB->DcpData.IpConflictDetected;


				/* AP00850530 tracing in case of changes */
				IsTheSameForTrace = oha_ipcmp(pAddress->ExpectedIPSuite.IpAddr,
												pAddress->ExpectedIPSuite.NetMask,
												pAddress->ExpectedIPSuite.Gateway,
												pExpectedIp, pExpectedIpMask, pExpectedGateway);

				if(!IsTheSameForTrace) /* AP00850530 */
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,
										 "OHA-AddrInfoInd(NICId=0x%X): ExpectedIPSuite is different.",NICId);
				}

                /* expected IP */
                OHA_MEMCPY(pAddress->ExpectedIPSuite.IpAddr,
                           OHA_IS_NULL(pExpectedIp) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pExpectedIp,
                           OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pAddress->ExpectedIPSuite.NetMask,
                           OHA_IS_NULL(pExpectedIpMask) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pExpectedIpMask,
                           OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pAddress->ExpectedIPSuite.Gateway,
                           OHA_IS_NULL(pExpectedGateway) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pExpectedGateway,
                           OHA_IP_ADDR_SIZE);


				/* AP00850530 tracing in case of changes */
				IsTheSameForTrace = oha_ipcmp(pAddress->EmergencyIPSuite.IpAddr,
												pAddress->EmergencyIPSuite.NetMask,
												pAddress->EmergencyIPSuite.Gateway,
												pEmergencyIp, pEmergencyIpMask, pEmergencyGateway);

				if(!IsTheSameForTrace) /* AP00850530 */
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,
										 "OHA-AddrInfoInd(NICId=0x%X): EmergencyIPSuite is different.",NICId);
				}

                /* emergency IP */
                OHA_MEMCPY(pAddress->EmergencyIPSuite.IpAddr,
                           OHA_IS_NULL(pEmergencyIp) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pEmergencyIp,
                           OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pAddress->EmergencyIPSuite.NetMask,
                           OHA_IS_NULL(pEmergencyIpMask) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pEmergencyIpMask,
                           OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pAddress->EmergencyIPSuite.Gateway,
                           OHA_IS_NULL(pEmergencyGateway) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pEmergencyGateway,
                           OHA_IP_ADDR_SIZE);

				/* AP00850530 tracing in case of changes */
				if ((!pAddress->CurrentIPSuiteIsEmergency && CurrentIPSuiteIsEmergency) ||
					 (pAddress->CurrentIPSuiteIsEmergency && !CurrentIPSuiteIsEmergency))
				{
					OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,
										 "OHA-AddrInfoInd(NICId=0x%X): CurrentIPSuiteIsEmergency(0x%X) is different (0x%X).",
										 NICId, CurrentIPSuiteIsEmergency, pAddress->CurrentIPSuiteIsEmergency);
				}
                pAddress->CurrentIPSuiteIsEmergency = CurrentIPSuiteIsEmergency;

				IsTheSameForTrace = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, ( pAddress->IpSuiteRemanent ) ? DCP_MASK_DS_REMANENT : 0);

				if(!IsTheSameForTrace) /* AP00850530 */
				{
					OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,
										 "OHA-AddrInfoInd(NICId=0x%X): Remanence of CurrentIPSuite differs from %u.",
										 NICId, pAddress->IpSuiteRemanent);
				}

                pAddress->IpSuiteRemanent = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT); /* AP01462749 */

                IpAddrChanged = LSA_TRUE;
            }
        }

        if (OHA_IS_NOT_NULL(pAddress->NoS.pName))
        {
            OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
            LSA_INT     str_len;
            LSA_BOOL    NoSIsTheSame;

            /* read local NameOfStation     */
            str_len = OHA_DB_READ_STATION_NAME(NICId, &NamePtr);

            /* ... and compare */
            NoSIsTheSame = (str_len == pAddress->NoS.Length) ? LSA_TRUE : LSA_FALSE;

            if (NoSIsTheSame)
            {
                if (OHA_IS_NOT_NULL(pAddress->NoS.pName))
                {
                    NoSIsTheSame = OHA_MEMCMP(pAddress->NoS.pName, NamePtr, str_len);
                }
            }

			/* AP00850530 tracing in case of changes */
			if (!NoSIsTheSame)
			{
				OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH,
									 "OHA-AddrInfoInd(NICId=0x%X): NameOfStation is different (Length are %u and %u).",
									 NICId, str_len, pAddress->NoS.Length);
			}

            if (NoSIsTheSame) /* AP01462749: rema status added */
            {
				NoSIsTheSame = oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, ( pAddress->NoS.Remanent ) ? DCP_MASK_DS_REMANENT : 0);
            }

			/* AP00850530 tracing in case of changes */
			if (!NoSIsTheSame)
			{
				OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,
									 "OHA-AddrInfoInd(NICId=0x%X): Remanence of NameOfStation differs from %u.",
									 NICId, pAddress->NoS.Remanent);
			}

            /* ... if not the same, copy and mark */
            if (!NoSIsTheSame)
            {
                if (OHA_IS_NOT_NULL(pAddress->NoS.pName))
                {
                    OHA_MEMCPY(pAddress->NoS.pName, NamePtr, str_len);
                }
                pAddress->NoS.Length  = (LSA_UINT16)str_len;
                pAddress->NoS.Remanent = oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, DCP_MASK_DS_REMANENT); /* AP01462749 */

                NameOfStationChanged  = LSA_TRUE;
            }
        }

        if ((pAddress->FactoryReset && !pDB->DcpData.FactoryReset) || /* AP01565375 */
            (!pAddress->FactoryReset && pDB->DcpData.FactoryReset))
        {
			OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,
								 "OHA-AddrInfoInd(NICId=0x%X): FactoryReset(0x%X) is different (0x%X).",
								 NICId, pDB->DcpData.FactoryReset, pAddress->FactoryReset);
        	
            pAddress->FactoryReset = pDB->DcpData.FactoryReset;

            FactoryResetChanged = LSA_TRUE;
        }

        if (IpAddrChanged || NameOfStationChanged || FactoryResetChanged)
        {
            /* delete the entry in the UserHDB */
            pHDB->pAddressInd[OHA_HANDLE_IDX(pHDBEdd->ThisHandle)] = LSA_NULL;

            OHA_UserRequestFinish(pHDB,pRQB,OHA_OK);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_AddressIndicateEvent(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_WriteAddrInfo                           +*/
/*+  Input/Output          :    LSA_UINT16                   NICId          +*/
/*+                             OHA_UPPER_ADDR_INFO_PTR_TYPE pAddress       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pAddress             : AddressInfo block                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Writes AddressInfo to the given block                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_WriteAddrInfo(
    LSA_UINT16                              NICId,
    OHA_UPPER_ADDR_INFO_PTR_TYPE            pAddress
)
{
    LSA_UINT16                  RetVal  = LSA_RET_OK;
    OHA_DB_IF_PTR_TYPE          pDB     = OHA_DB_GET_INTERFACE(NICId);
    OHA_HDB_EDD_PTR_TYPE        pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr  = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE pIpMask  = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE pGateway = LSA_NULL;

    OHA_COMMON_MEM_U8_PTR_TYPE pName    = LSA_NULL;
    LSA_INT     str_len;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_WriteAddrInfo(NICId: 0x%X, pAddress: 0x%X)",
                          NICId, pAddress);

    OHA_ASSERT_NULL_PTR(pAddress);
    OHA_ASSERT_NULL_PTR(pHDBEdd);
    OHA_ASSERT_NULL_PTR(pDB);

	/* Current Ip */
    (LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &pIpAddr, &pIpMask, &pGateway); /* local IP-Address (RealIP) */
    OHA_ASSERT_NULL_PTR(pIpAddr);
    OHA_ASSERT_NULL_PTR(pIpMask);
    OHA_ASSERT_NULL_PTR(pGateway);

    OHA_MEMCPY(pAddress->CurrentIPSuite.IpAddr,  pIpAddr, OHA_IP_ADDR_SIZE);
    OHA_MEMCPY(pAddress->CurrentIPSuite.NetMask, pIpMask, OHA_IP_ADDR_SIZE);
    OHA_MEMCPY(pAddress->CurrentIPSuite.Gateway, pGateway, OHA_IP_ADDR_SIZE);

    /* Expected Ip */
    (LSA_VOID)oha_db_read_expected_ip_suite(NICId, &pIpAddr, LSA_NULL, LSA_NULL);
    if (OHA_IS_NOT_NULL(pIpAddr))
    {
		pIpMask  = pIpAddr + OHA_IP_ADDR_SIZE;
		pGateway = pIpAddr + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE;
    }

    OHA_MEMCPY(pAddress->ExpectedIPSuite.IpAddr,
               OHA_IS_NULL(pIpAddr) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pIpAddr,
               OHA_IP_ADDR_SIZE);
    OHA_MEMCPY(pAddress->ExpectedIPSuite.NetMask,
               OHA_IS_NULL(pIpAddr) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pIpMask,
               OHA_IP_ADDR_SIZE);
    OHA_MEMCPY(pAddress->ExpectedIPSuite.Gateway,
               OHA_IS_NULL(pIpAddr) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pGateway,
               OHA_IP_ADDR_SIZE);

    /* Emergency Ip */
    (LSA_VOID)oha_db_read_emergency_ip_suite(NICId, &pIpAddr, LSA_NULL, LSA_NULL);
    if (OHA_IS_NOT_NULL(pIpAddr))
    {
        pIpMask  = pIpAddr + OHA_IP_ADDR_SIZE;
        pGateway = pIpAddr + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE;
    }

    OHA_MEMCPY(pAddress->EmergencyIPSuite.IpAddr,
               OHA_IS_NULL(pIpAddr) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pIpAddr,
               OHA_IP_ADDR_SIZE);
    OHA_MEMCPY(pAddress->EmergencyIPSuite.NetMask,
               OHA_IS_NULL(pIpAddr) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pIpMask,
               OHA_IP_ADDR_SIZE);
    OHA_MEMCPY(pAddress->EmergencyIPSuite.Gateway,
               OHA_IS_NULL(pIpAddr) ? (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP : pGateway,
               OHA_IP_ADDR_SIZE);

    pAddress->CurrentIPSuiteConflictDetected = pDB->DcpData.IpConflictDetected;
	pAddress->CurrentIPSuiteIsEmergency = pHDBEdd->SetAddrInfo.CurrentIPSuiteIsEmergency;
    pAddress->IpSuiteRemanent = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT); /* AP01462749 */

    /* read local NameOfStation */
    str_len = OHA_DB_READ_STATION_NAME(NICId, &pName);

	if (OHA_IS_NOT_NULL(pAddress->NoS.pName))
    {
		if(pAddress->NoS.Length >= str_len)
		{
			OHA_MEMCPY(pAddress->NoS.pName, pName, str_len);
		}
		else
		{
			OHA_MEMCPY(pAddress->NoS.pName, pName, pAddress->NoS.Length);
			RetVal = OHA_ERR_BUFFER_OVERFLOW;
		}
    }

    pAddress->NoS.Length = (LSA_UINT16)str_len;
    pAddress->NoS.Remanent = oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, DCP_MASK_DS_REMANENT); /* AP01462749 */
    pAddress->FactoryReset = pDB->DcpData.FactoryReset;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_WriteAddrInfo(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_store_expected_ip_suite
 * function:       store of the expected IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 *                 OHA_IP_ADDR IpAddrPtr:               Pointer to IP address
 *                 LSA_INT    AddrLen:                  Length of address
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_expected_ip_suite(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr,
    LSA_INT                       AddrLen)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_store_expected_ip_suite(NicId: 0x%X, IpAddrPtr: 0x%X, AddrLen: 0x%X)",
                          NicId, IpSuitePtr, AddrLen);

    if (OHA_IS_NOT_NULL(pDB) && (AddrLen == OHA_IP_SUITE_SIZE) &&
        OHA_IS_NOT_NULL(IpSuitePtr) )
    {
        /* IpAddr, SubnetMask, RouterAddr */
        if (oha_is_valid_ip_address(OHA_GET32_NTOH(IpSuitePtr,0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE),0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE),0),
                                    LSA_FALSE))
        {
            OHA_DB_ELEM_PTR_TYPE pElement = pDB->DcpData.pExpectedIpSuite;

            if (OHA_IS_NULL(pElement))      /* element not existing */
            {                      /* create it ! */
                pElement = oha_create_DbElement(OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, OHA_READ_WRITE, LSA_FALSE);
                if (OHA_IS_NOT_NULL(pElement))
                {
                    pDB->DcpData.pExpectedIpSuite = pElement;       /* store the element */
                    RetVal = LSA_RET_OK;
                }
            }
            else
            {
                RetVal = oha_update_DbElement(pElement, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, pElement->AccCtrl, pElement->Remanent);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_store_expected_ip_suite(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_expected_ip_suite
 * function:       read the expected IP address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              Length
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_expected_ip_suite(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pAccCtrl,    /* access control for variable */
    LSA_BOOL                      * pRemanent )   /* storage of variable in memory */
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = 0;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_expected_ip_suite(NicId: 0x%X, IpAddrPtrPtr: 0x%X)",
                          NicId, IpSuitePtrPtr);

    /* element exists */
    if (OHA_IS_NOT_NULL(pDB) && OHA_IS_NOT_NULL(pDB->DcpData.pExpectedIpSuite))
    {
        OHA_DB_ELEM_PTR_TYPE pTemp = pDB->DcpData.pExpectedIpSuite;

        if (OHA_IS_NOT_NULL(pTemp->pVariable) && pTemp->Length == OHA_IP_SUITE_SIZE)
        {
            /* IpAddr, SubnetMask, RouterAddr */
            *IpSuitePtrPtr = pTemp->pVariable;
            RetVal = OHA_IP_SUITE_SIZE;
        }
        else
        {
            *IpSuitePtrPtr = LSA_NULL;
        }

        /* write actual access control and remanent from existing database entry */
        if (OHA_IS_NOT_NULL(pTemp))     /* element exists */
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl = pTemp->AccCtrl;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent = pTemp->Remanent;
            }
        }
        else
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl   = OHA_NO_ACCESS;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent  = LSA_FALSE;
            }
        }
    }
    else
    {
        *IpSuitePtrPtr = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_read_expected_ip_suite(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_delete_expected_ip_suite
 * function:       delete the emergency IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_expected_ip_suite(
    LSA_UINT16                    NicId)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_RESOURCE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_expected_ip_suite(NicId: 0x%X)", NicId);

    if (OHA_IS_NOT_NULL(pDB))
    {
        if (OHA_IS_NOT_NULL(pDB->DcpData.pExpectedIpSuite))
        {
            oha_remove_DbElement (pDB->DcpData.pExpectedIpSuite);
            pDB->DcpData.pExpectedIpSuite = LSA_NULL;
			RetVal = LSA_RET_OK;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_expected_ip_suite(RetVal: 0x%X)", RetVal);
    return;
}

/*=============================================================================
 * function name:  oha_db_store_emergency_ip_suite
 * function:       store of the emergency IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 *                 OHA_IP_ADDR IpAddrPtr:               Pointer to IP address
 *                 LSA_INT    AddrLen:                  Length of address
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_emergency_ip_suite(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr,
    LSA_INT                       AddrLen)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_store_emergency_ip_suite(NicId: 0x%X, IpAddrPtr: 0x%X, AddrLen: 0x%X)",
                          NicId, IpSuitePtr, AddrLen);

    if (OHA_IS_NOT_NULL(pDB) && (AddrLen == OHA_IP_SUITE_SIZE) &&
        OHA_IS_NOT_NULL(IpSuitePtr) )
    {
        /* IpAddr, SubnetMask, RouterAddr */
        if (oha_is_valid_ip_address(OHA_GET32_NTOH(IpSuitePtr,0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE),0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE),0),
                                    LSA_FALSE))
        {
            OHA_DB_ELEM_PTR_TYPE pElement = pDB->DcpData.pEmergencyIpSuite;

            if (OHA_IS_NULL(pElement))      /* element not existing */
            {                      /* create it ! */
                pElement = oha_create_DbElement(OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, OHA_READ_WRITE, LSA_FALSE);
                if (OHA_IS_NOT_NULL(pElement))
                {
                    pDB->DcpData.pEmergencyIpSuite = pElement;      /* store the element */
                    RetVal = LSA_RET_OK;
                }
            }
            else
            {
                RetVal = oha_update_DbElement(pElement, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, pElement->AccCtrl, pElement->Remanent);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_store_emergency_ip_suite(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_emergency_ip_suite
 * function:       read the emergency IP address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              Length
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_emergency_ip_suite(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pAccCtrl,    /* access control for variable */
    LSA_BOOL                      * pRemanent )   /* storage of variable in memory */
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = 0;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_emergency_ip_suite(NicId: 0x%X, IpAddrPtrPtr: 0x%X)",
                          NicId, IpSuitePtrPtr);

    /* element exists */
    if (OHA_IS_NOT_NULL(pDB) && OHA_IS_NOT_NULL(pDB->DcpData.pEmergencyIpSuite))
    {
        OHA_DB_ELEM_PTR_TYPE pTemp = pDB->DcpData.pEmergencyIpSuite;

        if (OHA_IS_NOT_NULL(pTemp->pVariable) && pTemp->Length == OHA_IP_SUITE_SIZE)
        {
            /* IpAddr, SubnetMask, RouterAddr */
            *IpSuitePtrPtr = pTemp->pVariable;
            RetVal = OHA_IP_SUITE_SIZE;
        }
        else
        {
            *IpSuitePtrPtr = LSA_NULL;
        }

        /* write actual access control and remanent from existing database entry */
        if (OHA_IS_NOT_NULL(pTemp))     /* element exists */
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl = pTemp->AccCtrl;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent = pTemp->Remanent;
            }
        }
        else
        {
            if (OHA_IS_NOT_NULL(pAccCtrl))
            {
                *pAccCtrl   = OHA_NO_ACCESS;
            }
            if (OHA_IS_NOT_NULL(pRemanent))
            {
                *pRemanent  = LSA_FALSE;
            }
        }
    }
    else
    {
        *IpSuitePtrPtr = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_read_emergency_ip_suite(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_delete_emergency_ip_suite
 * function:       delete the emergency IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_emergency_ip_suite(
    LSA_UINT16                    NicId)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_RESOURCE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_emergency_ip_suite(NicId: 0x%X)", NicId);

    if (OHA_IS_NOT_NULL(pDB))
    {
        if (OHA_IS_NOT_NULL(pDB->DcpData.pEmergencyIpSuite))
        {
            oha_remove_DbElement (pDB->DcpData.pEmergencyIpSuite);
            pDB->DcpData.pEmergencyIpSuite = LSA_NULL;
			RetVal = LSA_RET_OK;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_emergency_ip_suite(RetVal: 0x%X)", RetVal);
    return;
}

/*=============================================================================
 * function name:  oha_db_delete_profibus_params
 * function:       delete the emergency IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_profibus_params(
    LSA_UINT16                    NicId)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_RESOURCE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_profibus_params(NicId: 0x%X)", NicId);

    if (OHA_IS_NOT_NULL(pDB))
    {
        oha_delete_DbElement (&pDB->DcpData.pPB_Common);
        oha_delete_DbElement (&pDB->DcpData.pPB_Addition);
        oha_delete_DbElement (&pDB->DcpData.pPB_Others);
		RetVal = LSA_RET_OK;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_profibus_params(RetVal: 0x%X)", RetVal);
    return;
}

/*=============================================================================
 * function name:  oha_db_write_ip_conflict_status
 * function:       write the IPConflict status
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_write_ip_conflict_status(
    LSA_UINT16                  NicId,
    LSA_BOOL                    ConflictStatus)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_RESOURCE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_ip_conflict_status(NicId: 0x%X)", NicId);

    if (OHA_IS_NOT_NULL(pDB))
    {
        pDB->DcpData.IpConflictDetected = ConflictStatus;
		RetVal = LSA_RET_OK;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_write_ip_conflict_status(RetVal: 0x%X)", RetVal);
    return;
}


/*=============================================================================
 * function name:  oha_db_read_ip_conflict_status
 * function:       read the IPConflict status
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_read_ip_conflict_status(
    LSA_UINT16                  NicId,
    LSA_BOOL *                  ConflictStatus)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_ip_conflict_status(NicId: 0x%X)", NicId);

    if (OHA_IS_NOT_NULL(pDB))
    {
        *ConflictStatus = pDB->DcpData.IpConflictDetected;
        RetVal = LSA_RET_OK;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_read_ip_conflict_status(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_write_factory_reset_status
 * function:       write the factory reset status
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_write_factory_reset_status(
    LSA_UINT16                  NicId,
    LSA_BOOL                    FactoryReset)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_RESOURCE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_factory_reset_status(NicId: 0x%X)", NicId);

    if (OHA_IS_NOT_NULL(pDB))
    {
        pDB->DcpData.FactoryReset = FactoryReset;
		RetVal = LSA_RET_OK;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_write_factory_reset_status(RetVal: 0x%X)", RetVal);
    return;
}

/*=============================================================================
 * function name:  oha_db_delete_real_ip_suite
 * function:       delete the real IP address (set to 0.0.0.0)
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_real_ip_suite(
    LSA_UINT16                    NicId)
{
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;
    OHA_COMMON_MEM_U8_PTR_TYPE  IpAddrPtr     = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE  SubnetAddrPtr = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE  RouterAddrPtr = LSA_NULL;
    LSA_INT size = OHA_DB_READ_IP_SUITE(NicId, &IpAddrPtr, &SubnetAddrPtr, &RouterAddrPtr);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_real_ip_suite(NicId: 0x%X)", NicId);

    if (size && OHA_IS_NOT_NULL(IpAddrPtr) &&
        OHA_IS_NOT_NULL(SubnetAddrPtr) && OHA_IS_NOT_NULL(RouterAddrPtr))
    {
        OHA_MEMSET(IpAddrPtr, 0, OHA_IP_ADDR_SIZE);
        OHA_MEMSET(SubnetAddrPtr, 0, OHA_IP_ADDR_SIZE);
        OHA_MEMSET(RouterAddrPtr, 0, OHA_IP_ADDR_SIZE);

        RetVal = LSA_RET_OK;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_real_ip_suite(RetVal: 0x%X)", RetVal);
    return RetVal;
}


/*=============================================================================
 * function name:  oha_db_set_real_ip_suite
 * function:       set the real IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_set_real_ip_suite(
    LSA_UINT16                  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE  pIpAddr,
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubnetAddr,
    OHA_COMMON_MEM_U8_PTR_TYPE  pRouterAddr)
{
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;
    OHA_COMMON_MEM_U8_PTR_TYPE  IpAddrPtr     = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE  SubnetAddrPtr = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE  RouterAddrPtr = LSA_NULL;
    LSA_INT size = OHA_DB_READ_IP_SUITE(NicId, &IpAddrPtr, &SubnetAddrPtr, &RouterAddrPtr);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_set_real_ip_suite(NicId: 0x%X)", NicId);

    OHA_ASSERT_NULL_PTR(pIpAddr);
    OHA_ASSERT_NULL_PTR(pSubnetAddr);
    OHA_ASSERT_NULL_PTR(pRouterAddr);

    if (size && OHA_IS_NOT_NULL(IpAddrPtr) &&
        OHA_IS_NOT_NULL(SubnetAddrPtr) && OHA_IS_NOT_NULL(RouterAddrPtr))
    {
        OHA_MEMCPY(IpAddrPtr,     pIpAddr,      OHA_IP_ADDR_SIZE);
        OHA_MEMCPY(SubnetAddrPtr, pSubnetAddr, OHA_IP_ADDR_SIZE);
        OHA_MEMCPY(RouterAddrPtr, pRouterAddr,  OHA_IP_ADDR_SIZE);

        RetVal = LSA_RET_OK;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_set_real_ip_suite(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_delete_station_name
 * function:       delete the NameOfStation (set to "")
 * parameters:     LSA_UINT16  NicId:       NIC
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_station_name(
    LSA_UINT16                    NicId)
{
    LSA_BOOL   Remanent = LSA_FALSE;
    LSA_UINT8  AccessControl = 0;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_station_name(NicId: 0x%X)", NicId);

    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDNAME,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK)
    {
        RetVal = OHA_DB_WRITE_STATION_NAME (NicId, (OHA_COMMON_MEM_U8_PTR_TYPE)"", 0, AccessControl, Remanent);
        OHA_ASSERT(RetVal == LSA_RET_OK);
    }
    else    /* not existing */
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,
                              "oha_db_delete_station_name: NameOfStation not existing (RetVal: 0x%X)", RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_station_name(RetVal: 0x%X)", RetVal);
    return RetVal;
}


/*=============================================================================
 * function name:   oha_db_write_snmp_variable
 * function:        Writes snmp object to the database (e.g. for MIB II)
 * parameters:      OHA_LIST_TYPED_ENTRY_TYPE * pAnchor: Ptr to database
 *                  OHA_COMMON_MEM_U8_PTR_TYPE  VarPtr:  Ptr to snmp object value
 *                  LSA_UINT16      VarLen:     Length of snmp object
 *                  LSA_UINT16      VarType:    Type of snmp object
 *                  LSA_BOOL        IsRema:     Store value permanently
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if location cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_snmp_variable (
	OHA_LIST_TYPED_ENTRY_TYPE(oha_db_snmp_elem_tag) * pAnchor,
    OHA_COMMON_MEM_U8_PTR_TYPE    VarPtr,
    LSA_UINT16                    VarLen,
    LSA_UINT8                     VarType,
	LSA_BOOL                      IsRema)
{
    LSA_UINT16 Ret = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_snmp_variable(pAnchor: 0x%X, VarPtr: 0x%X, VarLen: 0x%X, IsRema: 0x%X)",
                          pAnchor, VarPtr, VarLen, IsRema);

	if (OHA_IS_NOT_NULL(pAnchor))
	{
		OHA_DB_SNMP_ELEM_PTR_TYPE pElement;

		if (OhaListIsNotInitialized(pAnchor)) /* only initialized with NULL */
		{
			OhaListInitialize(pAnchor);
		}
			
		/* find the element, we've only one element */
		pElement = OhaListFirst(pAnchor, OHA_DB_SNMP_ELEM_PTR_TYPE);

		if (OHA_IS_NULL(pElement))     /* element existing ? */
		{                      /* create it ! */
			pElement = oha_create_snmp_DbElement(VarType, 0, 0, VarPtr, VarLen);
			if (OHA_IS_NOT_NULL(pElement))
			{
				pElement->Remanent = IsRema;
				OhaListInsertTail(pAnchor, &pElement->ListElem);        /* queue the element */
			}
			else
			{
				Ret = LSA_RET_ERR_PARAM;
			}
		}
		else
		{
			pElement->Remanent = IsRema;
			Ret = oha_update_snmp_DbElement(pElement, VarType, 0, 0, VarPtr, VarLen);
		}
	}
	else
	{
		Ret = LSA_RET_ERR_PARAM;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_write_snmp_variable(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:   oha_db_read_snmp_variable
 * function:        Reads snmp object from the database (e.g. for MIB II)
 * parameters:      OHA_LIST_TYPED_ENTRY_TYPE * pAnchor: Ptr to database
 *                  OHA_COMMON_MEM_U8_PTR_TYPE * pVarPtr:  Ptr to snmp object value
 *                  OHA_COMMON_MEM_U16_PTR_TYPE pVarLen:   pLength of snmp object
 *                  OHA_COMMON_MEM_U8_PTR_TYPE  pVarType:  pType of snmp object
 *                  LSA_BOOL     pIsRema:                  Storage of value 
 * return value:    LSA_UINT16   Error: != LSA_RET_OK if location cannot be written
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_snmp_variable (
	OHA_LIST_TYPED_ENTRY_TYPE(oha_db_snmp_elem_tag) * pAnchor,
    OHA_COMMON_MEM_U8_PTR_TYPE  * pVarPtr,
    OHA_COMMON_MEM_U16_PTR_TYPE   pVarLen,
    OHA_COMMON_MEM_U8_PTR_TYPE    pVarType,
	LSA_BOOL                    * pIsRema)
{
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_snmp_variable(pAnchor: 0x%X, pVarPtr: 0x%X, pVarLen: 0x%X, pIsRema: 0x%X)",
                          pAnchor, pVarPtr, pVarLen, pIsRema);

    if (OHA_IS_NOT_NULL(pAnchor))
    {   		/* find the element, we've only one element */
        OHA_DB_SNMP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_SNMP_ELEM_PTR_TYPE);
        if (OHA_IS_NULL(pElement))     /* element does not exist */
        {
            if (OHA_IS_NOT_NULL(pVarPtr))  *pVarPtr  = LSA_NULL;
            if (OHA_IS_NOT_NULL(pVarLen))  *pVarLen  = 0;
            if (OHA_IS_NOT_NULL(pVarType)) *pVarType = 0;
            if (OHA_IS_NOT_NULL(pIsRema))  *pIsRema  = LSA_FALSE;
	        RetVal = LSA_RET_ERR_RESOURCE;
        }
        else
        {
            if (OHA_IS_NOT_NULL(pVarPtr))  *pVarPtr  = pElement->pVariable;
            if (OHA_IS_NOT_NULL(pVarLen))  *pVarLen  = pElement->Length;
            if (OHA_IS_NOT_NULL(pVarType)) *pVarType = pElement->Type;
            if (OHA_IS_NOT_NULL(pIsRema))  *pIsRema  = pElement->Remanent;
        }
    }
    else    /* no such element in DB */
    {
        if (OHA_IS_NOT_NULL(pVarPtr)) *pVarPtr = LSA_NULL;
        if (OHA_IS_NOT_NULL(pVarLen)) *pVarLen = 0;
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_snmp_variable(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_store_rema_ip_address
 * function:       remanent store of an IP address
 * parameters:     LSA_UINT16  NicId:                   NIC
 *                 OHA_IP_ADDR IpAddrPtr:               Pointer to IP address
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_rema_ip_address(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr)
{
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_store_rema_ip_address(NicId: 0x%X, IpAddrPtr: 0x%X)",
                          NicId, IpSuitePtr);

    if (OHA_IS_NOT_NULL(pDB) && OHA_IS_NOT_NULL(IpSuitePtr) )
    {
        /* IpAddr, SubnetMask, RouterAddr */
        if (oha_is_valid_ip_address(OHA_GET32_NTOH(IpSuitePtr,0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE),0),
                                    OHA_GET32_NTOH((IpSuitePtr+OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE),0),
                                    LSA_FALSE))
        {
            OHA_DB_ELEM_PTR_TYPE pElement = pDB->DcpData.pRemaIpSuite;

            if (OHA_IS_NULL(pElement))      /* element not existing */
            {                      /* create it ! */
                pElement = oha_create_DbElement(OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, OHA_READ_WRITE, LSA_FALSE);
                if (OHA_IS_NOT_NULL(pElement))
                {
                    pDB->DcpData.pRemaIpSuite = pElement;       /* store the element */
                    RetVal = LSA_RET_OK;
                }
            }
            else
            {
                RetVal = oha_update_DbElement(pElement, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuitePtr, OHA_IP_SUITE_SIZE, pElement->AccCtrl, pElement->Remanent);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_store_rema_ip_address(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_rema_ip_address
 * function:       read the remanent stored IP address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              Length
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rema_ip_address(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr)   /* storage of variable in memory */
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = 0;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_rema_ip_address(NicId: 0x%X, IpAddrPtrPtr: 0x%X)",
                          NicId, IpSuitePtrPtr);

    /* element exists */
    if (OHA_IS_NOT_NULL(pDB) && OHA_IS_NOT_NULL(pDB->DcpData.pRemaIpSuite))
    {
        OHA_DB_ELEM_PTR_TYPE pRema = pDB->DcpData.pRemaIpSuite;

        if (OHA_IS_NOT_NULL(pRema->pVariable) && pRema->Length == OHA_IP_SUITE_SIZE)
        {
            /* IpAddr, SubnetMask, RouterAddr */
            *IpSuitePtrPtr = pRema->pVariable;
            RetVal = OHA_IP_SUITE_SIZE;
        }
        else
        {
            *IpSuitePtrPtr = LSA_NULL;
        }
    }
    else
    {
        *IpSuitePtrPtr = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_read_rema_ip_address(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_delete_rema_ip_address
 * function:       delete the remanent stored IP address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              LSA_RET_OK, LSA_RET_ERR_RESOURCE
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rema_ip_address(
    LSA_UINT16                      NicId)
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_rema_ip_address(NicId: 0x%X)", NicId);

    /* element exists */
    if (OHA_IS_NULL(pDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                              "OUT: oha_db_delete_rema_ip_address(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    if (OHA_IS_NOT_NULL(pDB->DcpData.pRemaIpSuite))
    {
        /* delete remanent ip object */
        oha_remove_DbElement (pDB->DcpData.pRemaIpSuite);
        pDB->DcpData.pRemaIpSuite = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_delete_rema_ip_address(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  oha_db_update_rema_ip_address
 * function:       update the remanent stored IP address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   LSA_UINT16  NicId:       NIC
 * return value:   LSA_UINT16:              LSA_RET_OK, LSA_RET_ERR_RESOURCE
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_update_rema_ip_address(
    LSA_UINT16                      NICId)
{
    LSA_UINT16 RetVal = LSA_RET_OK;
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NICId);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_update_rema_ip_address(NicId: 0x%X)", NICId);

    if (OHA_IS_NULL(pDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,
                              "OUT: oha_db_update_rema_ip_address(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

	if (oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT))
	{
		OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr  = LSA_NULL;
		OHA_COMMON_MEM_U8_PTR_TYPE pNetMask = LSA_NULL;
		OHA_COMMON_MEM_U8_PTR_TYPE pGateWay = LSA_NULL;

        (LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &pIpAddr, &pNetMask, &pGateWay);

		if (OHA_IS_NOT_NULL(pIpAddr))
		{
			LSA_UINT16 RetVal = oha_db_store_rema_ip_address(NICId, pIpAddr);
			if (RetVal != LSA_RET_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "oha_db_update_rema_ip_address: Write REMA IP failed (RetVal=%u).", RetVal);
			}
		}
	}
	else
	{
		(LSA_VOID)oha_db_delete_rema_ip_address(NICId);
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_db_update_rema_ip_address(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DbGetTrcIdx                             +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_UINT16       TraceIdx                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets TraceIdx from the Database-Interface-Entry by NICId  +*/
/*+  for the interface trace. If none is found 0 is returned.               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DbGetTrcIdx(
    LSA_UINT16 NICId)
{
    OHA_DB_IF_PTR_TYPE    pDB;
    LSA_UINT32            i;

	/* no trace for the trace ! */

    if (NICId == OHA_INVALID_NIC_ID)
    {
		return 0;
	}

	for(i=0, pDB=&oha_database.IfData[0]; i<OHA_CFG_MAX_NICS; i++, pDB++)
	{
		if (pDB->NicId == NICId)
		{
			return((LSA_UINT16)pDB->TraceIdx);
		}
    }

    return(0);
}

#if 0
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DbGetTrcIdxByIf                         +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_UINT16       TraceIdx                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  IfID      : InterfaceID from database (from EDD_SRV_GET_PARAMS)        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets TraceIdx from the Database-Interface-Entry by IfId   +*/
/*+  for the interface trace. If none is found 0 is returned.               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DbGetTrcIdxByIf(
    LSA_UINT32 IfId)
{
    OHA_DB_IF_PTR_TYPE    pDB  = &oha_database.IfData[0];
    LSA_UINT32            i     = 0;

	/* no trace */

    if (IfId > EDD_INTERFACE_ID_MAX) /* 0 is valid (global channel) */
    {
		return 0;
	}

	for(i=0, pDB=&oha_database.IfData[0]; i<OHA_CFG_MAX_NICS; i++, pDB++)
	{
		if(pDB->NicId != OHA_INVALID_NIC_ID)
		{
			if (pDB->InterfaceID == IfId)
			{
				return((LSA_UINT16)pDB->TraceIdx);
            }
        }
    }

    return(0);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_undo_init_interface                     +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes a Database-Interface (the NICId too!)             +*/
/*+  If no interface is found LSA_RET_ERR_RESOURCE is returned              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_undo_init_interface(
    LSA_UINT16 NICId)
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NICId);
    LSA_UINT16          Status = LSA_RET_ERR_RESOURCE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_undo_init_interface(NICId: 0x%X)",
                          NICId);

    if (OHA_IS_NOT_NULL(pDB))
	{
		if (pDB->NicId != OHA_INVALID_NIC_ID)   /* used interface */
		{
			OHA_FreeDbInterface(pDB);
			pDB->NicId = OHA_INVALID_NIC_ID;	/* set IF invalid */
			Status = LSA_RET_OK;
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_undo_init_interface(Status: 0x%X)", Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestSetIdent                         +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_SET_IDENT              +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.Ident                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write identification parameters to the         +*/
/*+               database and confirms request.                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestSetIdent(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd;
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp;
    LSA_UINT16              Response;
    LSA_UINT16              NICId;
    OHA_UPPER_IDENT_PTR_TYPE pIdent = &pRQB->args.Ident;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestSetIdent(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->Params.pDetail);

    OHA_ASSERT_NULL_PTR(pIdent);
    NICId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId); /* EDD channel necessary */
    pHDBDcp  = OHA_DCPGetHandleByNIC(NICId); /* DCP channel necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) ||
         LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL ) )
    {
        Response = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_IDENT failed. No Lower Channel for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if (!(pHDB->State == OHA_HANDLE_STATE_OPEN &&
               LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) ) )
    {           /* is no open/close user-channel runnning ? */
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_IDENT failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NICId);
    }
    else if (pIdent->Options == 0)   /* 'nothing to set ? */
    {
        Response = OHA_ERR_PARAM;  /* ParameterStatus is wrong */
        /* Error on getting Options. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_SET_IDENT failed. No Parameter to write (Options: 0x%X)",pRQB->args.Ident.Options);
    }
    else
    {
        Response = OHA_DoSetIdentRequest(pRQB, pHDB);
	}

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestSetIdent(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoSetIdentRequest                       +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_SET_IDENT              +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.Ident                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Write identificatiopn parameters to the database          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoSetIdentRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16                Response = OHA_OK;
    LSA_UINT16                NICId    = pHDB->Params.pDetail->Params.User.NICId;
    OHA_UPPER_IDENT_PTR_TYPE  pIdent   = &pRQB->args.Ident;
    LSA_UINT16                RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoSetIdentRequest(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pIdent);

    if (pIdent->Options & OHA_CTRL_OEM_DEVICE_ID)
    {
        LSA_UINT8 DeviceID[2*sizeof(LSA_UINT16)];
        OHA_COMMON_MEM_U8_PTR_TYPE pDeviceID = DeviceID;

        OHA_PUT16_HTON(pDeviceID, 0, pIdent->OEMVendorID);
        OHA_PUT16_HTON(pDeviceID, 2, pIdent->OEMDeviceID);

        RetVal = OHA_DB_WRITE_OEM_DEVICE_ID(NICId, pDeviceID, 2*sizeof(LSA_UINT16), OHA_READ_ONLY, LSA_FALSE);
        if(RetVal != LSA_RET_OK) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    pIdent->Options = 0;    /* no error, all params written */

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoSetIdentRequest(Response: 0x%X)", Response);
    return (Response);
}

/*****************************************************************************/
/*  end of file OHA_DB.C                                                     */
/*****************************************************************************/
