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
/*  F i l e               &F: oha_dcp1.c                                :F&  */
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
/*  07.09.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  10
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_DCP1 */

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

/* DCP-GET */
LSA_INT oha_dcp_GetOptPbus  (LSA_UINT16 NICId, LSA_UINT16 Type, DCP_USR_DATASET_PTR DsPtr, LSA_UINT32 *DcpGetStatus);
LSA_INT oha_dcp_GetOptTcp   (LSA_UINT16 NICId, LSA_UINT16 Type, DCP_USR_DATASET_PTR DsPtr, LSA_UINT32 *DcpGetStatus);
LSA_INT oha_dcp_GetOptDhcp  (LSA_UINT16 NICId, LSA_UINT16 Type, DCP_USR_DATASET_PTR DsPtr, LSA_UINT32 *DcpGetStatus);
LSA_INT oha_dcp_GetOptIdent (LSA_UINT16 NICId, LSA_UINT16 Type, DCP_USR_DATASET_PTR DsPtr, LSA_UINT32 *DcpGetStatus);
LSA_INT oha_dcp_GetOptInitiative(LSA_UINT16 NICId, LSA_UINT16 Type, DCP_USR_DATASET_PTR DsPtr, LSA_UINT32 *DcpGetStatus);

/* DCP-IDENTIFY */
LSA_INT oha_dcp_IdentifyFilterCheck(LSA_UINT16 NICId, DCP_USR_REQ_DATA_PTR ReqPtr, LSA_INT OptCnt,
                                    DCP_USR_RSP_DATA_PTR RspPtr, LSA_INT *OptCntPtr);
LSA_INT oha_dcp_IdentifyRspSet (LSA_UINT16 NICId, DCP_USR_RSP_DATA_PTR DataPtr, LSA_INT DataLen,
                                LSA_INT *OptCntPtr);

LSA_BOOL oha_dcp_FindType (DCP_USR_RSP_DATA_PTR DataPtr, LSA_INT DataLen, LSA_UINT16 Type);
DCP_USR_DATASET_PTR oha_dcp_FindTypePtr(DCP_USR_RSP_DATA_PTR    DataPtr, LSA_INT DataLen, LSA_UINT16 Type);

/* DCP-SET */
LSA_UINT8 oha_dcp_SetOptControl(LSA_UINT16 NICId, DCP_USR_DATASET_PTR  DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT8 oha_dcp_SetOptTcp (LSA_UINT16 NICId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT8 oha_dcp_SetOptDhcp(LSA_UINT16 NICId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT8 oha_dcp_SetOptIdent(LSA_UINT16 NICId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT8 oha_dcp_SetOptPbus(LSA_UINT16 NICId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);

OHA_DCP_LOWER_RQB_PTR_TYPE  oha_dcp_alloc_rsp_rqb(OHA_HDB_DCP_PTR_TYPE pHDB);


/*****************************************************************************/
/* Local function: oha_dcp_confirm_remote_set_ip()                           */
/* If the IP address is changed by DcpSet, an AddressInfo.Ind is done soon.  */
/*****************************************************************************/
LSA_VOID oha_dcp_confirm_remote_set_ip(
	LSA_UINT16 NICId,
	LSA_UINT32 Options, 
	LSA_BOOL CurrentIPIsEmergency)
{
	OHA_COMMON_MEM_U8_PTR_TYPE  IpAddrPtr = LSA_NULL;
    LSA_UINT16  RetVal;

	OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
						  "IN : oha_dcp_confirm_remote_set_ip(NICId: 0x%X, Options: 0x%X, CurrentIPIsEmergency: 0x%X)",
						  NICId, Options, CurrentIPIsEmergency);

	OHA_ASSERT(Options & OHA_CTRL_IP_SUITE);

	(LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &IpAddrPtr, LSA_NULL, LSA_NULL);
	OHA_ASSERT_NULL_PTR(IpAddrPtr);

	if(OHA_MEMCMP(IpAddrPtr, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE))
	{
		OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_confirm_remote_set_ip() - IP address is 0.0.0.0");
		return;
	}

	if (CurrentIPIsEmergency) /* EmergencyIp := CurrentIp */
	{
		RetVal = oha_db_store_emergency_ip_suite(NICId, IpAddrPtr, OHA_IP_SUITE_SIZE);
		if (RetVal != LSA_RET_OK)
		{
			 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "oha_dcp_confirm_remote_set_ip: Write Emergency IP failed (RetVal=%u).", RetVal);
		}
	}
	else    /* ExpectedIp := CurrentIp, EmergencyIp := 0.0.0.0 */
	{
		RetVal = oha_db_store_expected_ip_suite(NICId, IpAddrPtr, OHA_IP_SUITE_SIZE);
		if (RetVal != LSA_RET_OK)
		{
			 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "oha_dcp_confirm_remote_set_ip: Write Expected IP failed (RetVal=%u).", RetVal);
		}

		oha_db_delete_emergency_ip_suite(NICId);
	}

	/* reset conflict flags */
	oha_db_write_ip_conflict_status(NICId, LSA_FALSE);

    oha_db_write_factory_reset_status(NICId, LSA_FALSE); /* AP01565375 */

	/* AddressInfo changed - indication (to all user) only */
	{
		OHA_HDB_USER_PTR_TYPE pHDBUser;
		LSA_UINT16 i;

		for (pHDBUser = OHA_GET_HUSER_PTR(0),i=0; i < OHA_CFG_MAX_CHANNELS; pHDBUser++,i++)
		{
			if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
			{
				/* AP01022057: confirm only the user with the same Nic */
				if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
				{
					(LSA_VOID)OHA_AddressIndicateEvent(NICId, pHDBUser);/* address changed - indication */
				}
			}
		}
	}

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_confirm_remote_set_ip()");
}

/*****************************************************************************/
/* Local function: oha_dcp_confirm_remote_set()                              */
/* SET Request will be confirmed.                                            */
/*****************************************************************************/
LSA_VOID oha_dcp_confirm_remote_set(
    OHA_HDB_DCP_PTR_TYPE                pHDB,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pSetInd)
{
    LSA_UINT16  RetVal;
    LSA_UINT16  NICId;
    LSA_UINT32  Options = pSetInd->Options;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_confirm_remote_set(pHDB: 0x%X, pSetInd: 0x%X)",
                          pHDB, pSetInd);

    OHA_ASSERT_NULL_PTR(pHDB);

    NICId = pHDB->Params.pDetail->Params.Dcp.NICId;

    /* a Reset to Factory was in the Set-Request, the users are already confirmed */
    if (pSetInd->Options & OHA_CTRL_RESET_TO_FACTORY)
    {
		OHA_DCP_INDICATION_TYPE  ResetToFactory;  /* indication for ResetToFactory */

        ResetToFactory.Event    = OHA_EVENT_CONTROL;

        ResetToFactory.Options  = OHA_CTRL_RESET_TO_FACTORY;
        ResetToFactory.RemaOptions = 0;
        ResetToFactory.Count    = 1;
        ResetToFactory.Type     = OHA_DCP_OPTION_CTRL;
        ResetToFactory.SubType  = OHA_DCP_RESET_TO_FACTORY;
        OHA_MEMCPY(&ResetToFactory.SenderMACAddr, &pSetInd->SenderMACAddr, OHA_MAC_ADDR_SIZE);

        OHA_DcpIndicateEventAllUsers(pHDB, &ResetToFactory);/* event indicate */

        pSetInd->Options &= ~OHA_CTRL_RESET_TO_FACTORY; /* delete mark */

		/* a ResetToFactory is done, now SetIp (0.0.0.0), SetName (""),... */
		(LSA_VOID)OHA_DoRtfRequest(NICId, 0);
    }

    if (pSetInd->Options & OHA_CTRL_RESET_COMMUNICATION)
    {
		OHA_DCP_INDICATION_TYPE  ResetCommunication;  /* indication for comm. params */

        ResetCommunication.Event    = OHA_EVENT_CONTROL;

        ResetCommunication.Options  = OHA_CTRL_RESET_COMMUNICATION;
        ResetCommunication.RemaOptions = 0;
        ResetCommunication.Count    = 1;
        ResetCommunication.Type     = OHA_DCP_OPTION_CTRL;
        ResetCommunication.SubType  = OHA_DCP_RESET;
        OHA_MEMCPY(&ResetCommunication.SenderMACAddr, &pSetInd->SenderMACAddr, OHA_MAC_ADDR_SIZE);

        OHA_DcpIndicateEventAllUsers(pHDB, &ResetCommunication);/* event indicate */

        pSetInd->Options &= ~OHA_CTRL_RESET_COMMUNICATION; /* delete mark */

		/* a ResetToFactory is done, now SetIp (0.0.0.0), SetName (""),... */
		(LSA_VOID)OHA_DoRtfRequest(NICId, 0);
    }

    if (pSetInd->Options)           /* changes made ? */
    {
        pHDB->LastSet.Event    = OHA_EVENT_REMOTE_SET;
        pHDB->LastSet.Options  = pSetInd->Options;
        pHDB->LastSet.RemaOptions = pSetInd->RemaOptions;
        pHDB->LastSet.Type     = pSetInd->Type;
        pHDB->LastSet.SubType  = pSetInd->SubType;
        pHDB->LastSet.Count    = pSetInd->Count;
        OHA_MEMCPY(&pHDB->LastSet.SenderMACAddr, &pSetInd->SenderMACAddr, OHA_MAC_ADDR_SIZE);

        /* update the DCP frame filter if filter is active */
        (LSA_VOID)OHA_DCPUpdateFilter(NICId,
                                      pHDB->LastSet.Options,
                                      OHA_HANDLE_TYPE_DCP);

        /* update the ARP-filter if IP-address changed */
        if (pSetInd->Options & OHA_CTRL_IP_SUITE)
        {
            /* update IP-Suites */
            LSA_UINT8 IpSuite[OHA_IP_SUITE_SIZE];
			LSA_BOOL  Remanent = oha_db_compare_remanence(NICId, DCP_DBID_TCP, DCP_DSID_TCPIP, DCP_MASK_DS_REMANENT);

            OHA_COMMON_MEM_U8_PTR_TYPE  IpAddrPtr     = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE  SubnetAddrPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE  RouterAddrPtr = LSA_NULL;

            (LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &IpAddrPtr, &SubnetAddrPtr, &RouterAddrPtr);
            OHA_ASSERT_NULL_PTR(IpAddrPtr);
            OHA_ASSERT_NULL_PTR(SubnetAddrPtr);
            OHA_ASSERT_NULL_PTR(RouterAddrPtr);

            OHA_MEMCPY(&IpSuite, IpAddrPtr, OHA_IP_ADDR_SIZE);
            OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE], SubnetAddrPtr, OHA_IP_ADDR_SIZE);
            OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE], RouterAddrPtr, OHA_IP_ADDR_SIZE);

            if (pSetInd->Event == OHA_MODE_EMERGENCY_SET) /* EmergencyIp := CurrentIp */
            {
                RetVal = oha_db_store_emergency_ip_suite(NICId, IpSuite, OHA_IP_SUITE_SIZE);
				if (RetVal != LSA_RET_OK)
				{
					 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_RequestSetAddress: Write Emergency IP failed (RetVal=%u).", RetVal);
				}

				/* AP00909635 - in case of an emergency IP the rema IP remains ! */
				/* AP01265748 - a remanent emergency IP is not to store in REMA ! */
            }
            else    /* ExpectedIp := CurrentIp, EmergencyIp := 0.0.0.0 */
            {
                RetVal = oha_db_store_expected_ip_suite(NICId, IpSuite, OHA_IP_SUITE_SIZE);
				if (RetVal != LSA_RET_OK)
				{
					 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_RequestSetAddress: Write Expected IP failed (RetVal=%u).", RetVal);
				}

                oha_db_delete_emergency_ip_suite(NICId);

				if (Remanent)	/* AP00909635 */
				{
					RetVal = oha_db_store_rema_ip_address(NICId, IpSuite);
					if (RetVal != LSA_RET_OK)
					{
						 OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "OHA_RequestSetAddress: Write REMA IP failed (RetVal=%u).", RetVal);
					}
				}
				else
				{
					(LSA_VOID)oha_db_delete_rema_ip_address(NICId);
				}
            }

            oha_db_write_ip_conflict_status(NICId, LSA_FALSE);

            RetVal = OHA_EDDSetArpFilter(NICId, OHA_HANDLE_TYPE_DCP);
            if (RetVal != OHA_OK)
            {
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_dcp_confirm_remote_set - EDDSetArpFilter(RetVal: 0x%X)", RetVal);
            }
            RetVal = OHA_EDDSetIp(NICId, OHA_HANDLE_TYPE_DCP);
            if (RetVal != OHA_OK)
            {
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_dcp_confirm_remote_set - EDDSetIp(RetVal: 0x%X)", RetVal);
            }
        }

        OHA_DcpIndicateEventAllUsers(pHDB, &pHDB->LastSet);  /* event only when set is successful */
    }

    if ((Options & OHA_CTRL_STATION_NAME) || /* AP01565375 */
        (Options & OHA_CTRL_IP_SUITE))
    {
        oha_db_write_factory_reset_status(NICId, LSA_FALSE);
    }

    /* do the address changed - indication, if provided for this NIC */
    if (Options & OHA_CTRL_IP_SUITE || Options & OHA_CTRL_STATION_NAME ||
		Options & OHA_CTRL_RESET_TO_FACTORY || Options & OHA_CTRL_RESET_COMMUNICATION) /* AP00968621 */
    {
        OHA_AddressIndicateEventAllUser(NICId);/* address changed - indication */
    }

	/* AP00846402: In case of a Reset to Factory via DCP the OHA-database (NameOfStation, IP address) is not deleted in the lldp frame */
	if(Options)
	{
		/* update the LLDP-send data if LLDP-Send is active */
		RetVal = OHA_LLDPUpdateSend(NICId, 
									Options,
									OHA_HANDLE_TYPE_DCP);

		if (RetVal != OHA_OK)
		{
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_confirm_remote_set - LLDPUpdateSend(RetVal: 0x%X)", RetVal);
		}
	}

#ifndef OHA_CFG_NO_REMA
	 if (Options & OHA_CTRL_DHCP_OPTION)
	 {
		(LSA_VOID)oha_rema_dhcp_update(NICId);
	 }
	 if ((Options & OHA_CTRL_PROFIBUS_COMMON) ||
		 (Options & OHA_CTRL_PROFIBUS_ADDITION))
	 {
		(LSA_VOID)oha_rema_bus_update(NICId);
	 }
#endif

#ifndef OHA_CFG_NO_NARE
    /* is the IP-Suite to check (ethernet) */
    {
        LSA_UINT16 CheckInterval;
        LSA_UINT32 CheckOptions = (pSetInd->CheckIpEnabled) ? OHA_CTRL_IP_SUITE : 0;

        if (OHA_IS_NOT_NULL(pHDB->pUpperRespRqb))  /* user query ? */
        {
            LSA_HANDLE_TYPE Handle = OHA_RQB_GET_HANDLE(pHDB->pUpperRespRqb);
            OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GetUserHDBFromHandle(Handle);

            OHA_ASSERT_NULL_PTR(pHDBUser);
            CheckInterval = pHDBUser->Params.pDetail->Params.User.CheckInterval;
        }
        else
        {
            CheckInterval = 0; /* no user query, no check */
        }

        RetVal = OHA_NARECheckAddress(NICId, CheckOptions, pSetInd->Options, CheckInterval);
        if (RetVal != OHA_OK)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_dcp_confirm_remote_set - OHA_NARECheckAddress failed (RetVal: 0x%X)", RetVal);
        }
    }
#endif

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_confirm_remote_set(Options: 0x%X)",pHDB->LastSet.Options);
}


/*****************************************************************************/
/* Local function: oha_dcp_Set()                                             */
/* Does the SET Request.                                                     */
/*****************************************************************************/
LSA_UINT16      oha_dcp_Set(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pSetInd)
{
    LSA_UINT16              Type;
    LSA_INT                 ReqLen;
    LSA_INT                 ReadLen;
    DCP_USR_DATASET_PTR     RespoPtr;
    LSA_INT                 WriteLen;
    DCP_USR_BYTE_PTR        ReadPtr;
    DCP_USR_DATASET_PTR     DsPtr;
    LSA_INT                 SendLen = 0;
    LSA_UINT16              RetVal = LSA_RET_OK;
    DCP_RQB_ARGS_RECV_IND DCP_UPPER_MEM_ATTR * pRecv = &pRQB->args.RecvInd;
    DCP_USR_REQ_DATA_PTR    ReqPtr = (DCP_USR_REQ_DATA_PTR)(pRecv->DataPtr);
    LSA_UINT16              NicId;

	OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pSetInd);

	NicId = pHDB->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_Set(pRQB: 0x%X, NIC-ID: 0x%X, pSetInd: 0x%X)",
                          pRQB, NicId, pSetInd);

    /* the SET-actions are depending on pSetInd->Event:                                                            */
    /*                                                                                                             */
    /* OHA_EVENT_REMOTE_SET: an OHA controlled (remote) SET, set all the options, which are not user-locked or write protected */
    /* OHA_MODE_SET or     an USER controlled SET, set all the options, which are not user-locked or write protected */
    /* OHA_MODE_EMERGENCY_SET: If there are CheckOptions (CheckIp or CheckNoS), check the ethernet (double Name/IP)  */
    /*                    respond the remote Server positive and send the Nare-Requests                            */
    /*                    respond the User by OhaOpcResponse.Con and do the entry in the database if O.K.          */
    /* OHA_MODE_NO_SET:   only respond the remote DCP-server negative (User- or OHA controlled)                    */

    /* init the options         */
    pSetInd->Options  = pSetInd->RemaOptions = 0;

    /* do not initialize, there might have been generated control-options in OHA_REPORT_DCP_SET_IND() */
    /* pSetInd->AdditionalOptions  = 0; */
    pSetInd->Type     = pSetInd->SubType  = 0;
    pSetInd->Count    = 0;

    /* check the Request: one dataset at least one dataset existing */
    if (pRecv->OptCnt > 0)
    {
        DCP_UPPER_RQB_PTR_TYPE  RbPtr;
        LSA_INT                 i;

        DsPtr = &ReqPtr->Set.Dataset;
        ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;

        /* calculate laenge of the TLV-chain */
        for (i = 0, ReqLen = 0; i < pRecv->OptCnt; i++)
        {
            ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);
            if ((ReadLen % 2) != 0)
            {
                /* jump over fillbyte */
                ReadLen++;
            }

            ReqLen += ReadLen;
            ReadPtr += ReadLen;
            DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;
        }

        /* allocate Response-RQB */
        RbPtr = oha_dcp_alloc_rsp_rqb(pHDB);
        if ( ! LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL) )
        {
            DCP_USR_BYTE_PTR    WritePtr  = DCP_UPPER_RQB_RESP_REQ_DATA_PTR_GET(RbPtr);
            DCP_USR_DATASET_PTR ResultPtr = (DCP_USR_DATASET_PTR)WritePtr;
            LSA_INT             RestLen   = ReqLen;
            DCP_USR_BYTE_PTR    DstMacPtr = DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_GET(RbPtr);

            DsPtr = &ReqPtr->Set.Dataset;
            ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;
            RespoPtr  = ResultPtr;

            while (RestLen > 0)
            {
                LSA_UINT8   Result;
                LSA_UINT8   DbaseID;

                ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);
                WriteLen = DCP_DS_RESULT_MIN_SIZE;

                if (RestLen >= ReadLen)
                {
                    DbaseID = DsPtr->Hdr.Opt;

					if (SendLen + WriteLen > DCP_MAX_RSP_DATA_SIZE)
					{
						/* TFS 743037: The (further) requested data do not fit in the DCP-Set-ResPDU ! */
				        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_FATAL,"All requested data OptCnt: %d) do not fit in the DCP-Set-ResPDU!", pRecv->OptCnt);
						OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
						break;
					}


                    if (pSetInd->Event == OHA_MODE_NO_SET)  /* No set of parameters, only respond the remote dcp-server */
                    {                            /* with an error */
                        Result = (pSetInd->Reason == OHA_DCP_REASON_OK) ? (LSA_UINT8)DCP_RESULT_DS_NOT_SET : pSetInd->Reason;
                    }
                    else
                    {
                        switch (DbaseID)
                        {
                            case DCP_DBID_TCP:          /* TCP/IP-Parameter 1 */
                                Result = oha_dcp_SetOptTcp(NicId, DsPtr, pSetInd); /* MacAddr, IpAddr, SubnetMask, DefRouterAddr */
                                break;
                            case DCP_DBID_DHCP:         /* DHCP-Parameter 3 */
                                Result = oha_dcp_SetOptDhcp(NicId, DsPtr, pSetInd);  /* DhcpOption */
                                break;
                            case DCP_DBID_IDENT:        /* Allgem. Geraeteeigenschaften 2 */
                                Result = oha_dcp_SetOptIdent(NicId, DsPtr, pSetInd); /* StationType, StationName, StationId */
                                break;
                            case DCP_DBID_CTRL:         /* Control Option 5 */
                                Result = oha_dcp_SetOptControl(NicId, DsPtr, pSetInd);
                                break;                                      /* LockBegin, FlashStart */
                            case DCP_DBID_PBPARAM:      /* Profibusparameter 129 */
                                Result = oha_dcp_SetOptPbus(NicId, DsPtr, pSetInd); /* ProfibusProfil, ProfibusParam */
                                break;
                            default:                    /* unknown database */
                                Result = DCP_RESULT_NO_DATABASE;
                                break;
                        }
                    }
                }
                else
                {               /* length too short for a further dataset */
                    Result = DCP_RESULT_NO_DATASET;
                }

                /* fill Result Dataset: Request-Option/Suboption in Status */
                Type = OHA_WORD(DsPtr->Hdr.Id, DsPtr->Hdr.Opt);
                ResultPtr->Status = OHA_SWAP16(Type);
                ResultPtr->Hdr.Opt = DCP_DBID_CTRL;
                ResultPtr->Hdr.Id = DCP_DSID_RESPONSE;
                ResultPtr->Hdr.Len = OHA_SWAP16(1+2);
                ResultPtr->Data.Result.Code = Result;
                ResultPtr->Data.Result.AddOn = DCP_RESULT_RESERVED;
                SendLen += WriteLen;

                /* Further dataset in the Request? */
                if ((ReadLen % 2) != 0)
                {
                    /* jump over Fillbyte */
                    ReadLen++;
                }
                RestLen -= ReadLen;
                if (RestLen != 0)
                {
                    ReadPtr += ReadLen;
                    WritePtr += WriteLen;
                    DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;
                    ResultPtr = (DCP_USR_DATASET_PTR)WritePtr;
                }
            }

            /* No set of parameters, only respond the remote dcp-server */
            /* In case of a negative returnvalue of OHA_DCP_TRANS_END()
             * signalize the error in the first Result in the Response */
            if ((pSetInd->Event == OHA_MODE_NO_SET) &&
                (DCP_RESULT_OK == RespoPtr->Data.Result.Code))
            {
                RespoPtr->Data.Result.Code = (pSetInd->Reason == OHA_DCP_REASON_OK) ? (LSA_UINT8)DCP_RESULT_DS_NOT_SET : pSetInd->Reason;
            }

            /* take over the Source-MAC-Address as Destination-MAC-Address
             * and set the rest of the RQB-parameters */
            OHA_MEMCPY(DstMacPtr, pRecv->AddrPtr, OHA_MAC_ADDR_SIZE);
            DCP_UPPER_RQB_RESP_REQ_OPT_CNT_SET(RbPtr, pRecv->OptCnt);
            DCP_UPPER_RQB_RESP_REQ_TRANS_ID_SET(RbPtr, pRecv->TransactId);
            DCP_UPPER_RQB_RESP_REQ_SERVICE_ID_SET(RbPtr, DCP_SET);
            pHDB->TxPendingCnt++;
            /* send Response */
            OHA_DCP_REQUEST_LOWER(RbPtr,pHDB->Params.pSys);

            /* AP00xxxxxx: Waiting for DCP-Set.Con for calling OHA_REPORT_DCP_SET_RES() */

			/* AP01271668 earlier OHA-AddrInfo.IND */
			if(pSetInd->Options & OHA_CTRL_IP_SUITE)
			{
				switch(pSetInd->Event)
				{
					case OHA_MODE_NO_SET:
					case OHA_MODE_EXTERNAL_SET:
					case OHA_EVENT_CONTROL:
					break;
					case OHA_MODE_SET:
					case OHA_EVENT_LOCAL_SET:
					case OHA_EVENT_REMOTE_SET:
						oha_dcp_confirm_remote_set_ip(NicId, pSetInd->Options, LSA_FALSE);
					break;
					case OHA_MODE_EMERGENCY_SET:
						oha_dcp_confirm_remote_set_ip(NicId, pSetInd->Options, LSA_TRUE);
					break;
					case OHA_EVENT_QUERY_SET:
					default:
			            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_FATAL,"DCP-Set failed.(Set-Event: 0x%X).",pSetInd->Event);
						OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
					break;
				}
			}
        }
        else
        {
            pSetInd->Reason = OHA_DCP_REASON_NO_RESOURCE;
            RetVal = LSA_RET_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower RQB-memory failed!");
        }
    }
    else
    {
        pSetInd->Reason = OHA_DCP_REASON_NO_DATASET;
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"No dataset demanded from the ethenet!");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_Set(RetVal: 0x%X, SendLen: %d)", RetVal, SendLen);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_Get()                                             */
/* Does the GET Request: Send the demanded datasets of a database            */
/* or an error to the client.                                                */
/*****************************************************************************/
LSA_UINT16                       oha_dcp_Get(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB)
{
    DCP_TYPE                        Type;
    DCP_TYPE DCP_UPPER_MEM_ATTR *   TypePtr;
    DCP_USR_BYTE_PTR                DstMacPtr;
    DCP_USR_BYTE_PTR                WritePtr;
    DCP_USR_DATASET_PTR             ResultPtr;
    LSA_INT                         SendLen = 0;
    LSA_INT                         DsetCnt;
    LSA_UINT16                      RetVal = LSA_RET_OK;
	LSA_UINT16                      NICId;

    DCP_RQB_ARGS_RECV_IND DCP_UPPER_MEM_ATTR * pRecv = &pRQB->args.RecvInd;
    /* OHA_DCP_LOWER_MEM_PTR_TYPE   pTLV = (OHA_DCP_LOWER_MEM_PTR_TYPE) pRQB->args.RecvInd.DataPtr; */

    DCP_USR_REQ_DATA_PTR            ReqPtr   = (DCP_USR_REQ_DATA_PTR)(pRecv->DataPtr);
    /* OHA_MAC_ADDR     DCP_UPPER_MEM_ATTR  AddrPtr  = pRecv->AddrPtr;  */
    /* LSA_INT                          OptCnt   = pRecv->OptCnt;       */
    /* LSA_UINT32                       TxId     = pRecv->TransactId;   */
    LSA_UINT32                      DcpGetStatus = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_get(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRecv);
    OHA_ASSERT_NULL_PTR(pRecv->DataPtr);

    DsetCnt = 0;
	NICId   = pHDB->Params.pDetail->Params.Dcp.NICId;

    /* Check the request (at least one dataset demanded) */
    if (pRecv->OptCnt > 0)
    {
        /* allocate Response-RQB */
        OHA_DCP_LOWER_RQB_PTR_TYPE RbPtr = oha_dcp_alloc_rsp_rqb(pHDB);
        if ( ! LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL) )
        {
            LSA_UINT8               DbaseID;

            DstMacPtr = DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_GET(RbPtr);
            WritePtr  = DCP_UPPER_RQB_RESP_REQ_DATA_PTR_GET(RbPtr);
            ResultPtr = (DCP_USR_DATASET_PTR)WritePtr;
            TypePtr   = ReqPtr->Get.Type;

            while (DsetCnt < pRecv->OptCnt)
            {
                LSA_UINT8   Result;
                LSA_INT     DsetLen;

                Type = OHA_SWAP16(*TypePtr);
                DbaseID = OHA_HIBYTE(Type);

				/* The requested data shall be responded as long as they fit in the DCP-Get-ResPDU */
				/* AP01509627: Do the (further) requested data fit in the DCP-Get-ResPDU ? */
                switch (DbaseID)            /* option */
                {
                    case DCP_DBID_TCP:          /* TCP/IP-Parameter */
                        DsetLen = oha_dcp_GetOptTcp(NICId, Type, LSA_NULL, &DcpGetStatus);
                        break;
                    case DCP_DBID_DHCP:         /* DHCP-Parameter */
                        DsetLen = oha_dcp_GetOptDhcp(NICId, Type, LSA_NULL, &DcpGetStatus);
                        break;
                    case DCP_DBID_IDENT:        /* Device Identification */
                        DsetLen = oha_dcp_GetOptIdent(NICId, Type, LSA_NULL, &DcpGetStatus);
                        break;
                    case DCP_DBID_PBPARAM:      /* Profibusparameter */
                        DsetLen = oha_dcp_GetOptPbus(NICId, Type, LSA_NULL, &DcpGetStatus);
                        break;
                    case DCP_DBID_CTRL:         /* read the Control option */
                         DsetLen = 0;
                         break;
                    default:                    /* Unknown database */
                         DsetLen = 0;
                         break;
                }

				if (DsetLen == 0)
				{
	               DsetLen = DCP_DS_RESULT_MIN_SIZE;
				}
                if ((DsetLen % 2) != 0)
                {
					DsetLen++;
				}
				if (SendLen + DsetLen > DCP_MAX_RSP_DATA_SIZE)
				{
					/* AP01509627: The (further) requested data do not fit in the DCP-Get-ResPDU ! */
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"All requested data (DsetCnt: %d) do not fit in the DCP-Get-ResPDU!", DsetCnt);
					break;
				}

                /* local database exists: get the demanded datasets */
                switch (DbaseID)            /* option */
                {
                    case DCP_DBID_TCP:          /* TCP/IP-Parameter */
                        DsetLen = oha_dcp_GetOptTcp(NICId, Type, ResultPtr, &DcpGetStatus);
						Result = (DsetLen == 0) ? (LSA_UINT8)DCP_RESULT_NO_DATASET : (LSA_UINT8)DCP_RESULT_OK;
                        break;

                    case DCP_DBID_DHCP:         /* DHCP-Parameter */
                        DsetLen = oha_dcp_GetOptDhcp(NICId, Type, ResultPtr, &DcpGetStatus);
						Result = (DsetLen == 0) ? (LSA_UINT8)DCP_RESULT_NO_DATASET : (LSA_UINT8)DCP_RESULT_OK;
                        break;

                    case DCP_DBID_IDENT:        /* Device Identification */
                        DsetLen = oha_dcp_GetOptIdent(NICId, Type, ResultPtr, &DcpGetStatus);
						Result = (DsetLen == 0) ? (LSA_UINT8)DCP_RESULT_NO_DATASET : (LSA_UINT8)DCP_RESULT_OK;
                        break;

                    case DCP_DBID_PBPARAM:      /* Profibusparameter */
                        DsetLen = oha_dcp_GetOptPbus(NICId, Type, ResultPtr, &DcpGetStatus);
						Result = (DsetLen == 0) ? (LSA_UINT8)DCP_RESULT_NO_DATASET : (LSA_UINT8)DCP_RESULT_OK;
                        break;

                    case DCP_DBID_CTRL:         /* read the Control option */
                        DsetLen = 0;
                        Result = DCP_RESULT_NO_DATASET;
                        break;

                    default:                    /* Unknown database */
                        DsetLen = 0;
                        Result = DCP_RESULT_NO_DATABASE;
                        break;
                }

                if (Result != DCP_RESULT_OK) /* a Dcp-Get is not allowed */
                {
                    /* Fill Result Dataset: Request-Type (Option/Suboption) in Status */
                    ResultPtr->Status = OHA_SWAP16(Type);
                    ResultPtr->Hdr.Opt = DCP_DBID_CTRL;
                    ResultPtr->Hdr.Id = DCP_DSID_RESPONSE;
                    ResultPtr->Hdr.Len = OHA_SWAP16(1+2);
                    ResultPtr->Data.Result.Code = Result;
                    ResultPtr->Data.Result.AddOn = DCP_RESULT_RESERVED;
                    DsetLen = DCP_DS_RESULT_MIN_SIZE;
                }

                if ((DsetLen % 2) != 0)
                {
                    /* fillbyte if length ist odd   */
                    WritePtr[DsetLen++] = 0;
                }

                /* Further dataset in the request existing? */
                SendLen += DsetLen;
                DsetCnt++;
                if (DsetCnt < pRecv->OptCnt)
                {
                    TypePtr++;
                    WritePtr += DsetLen;
                    ResultPtr = (DCP_USR_DATASET_PTR)WritePtr;
                }
            }

            /* Take over Source-MAC-Address as Destination-MAC-Address */
            /* and set the rest of the RQB-parameters */
            OHA_MEMCPY(DstMacPtr, pRecv->AddrPtr, OHA_MAC_ADDR_SIZE);
            DCP_UPPER_RQB_RESP_REQ_OPT_CNT_SET(RbPtr, DsetCnt);
            DCP_UPPER_RQB_RESP_REQ_TRANS_ID_SET(RbPtr, pRecv->TransactId);
            DCP_UPPER_RQB_RESP_REQ_SERVICE_ID_SET(RbPtr, DCP_GET);
            pHDB->TxPendingCnt++;
            /* send response */
            OHA_DCP_REQUEST_LOWER(RbPtr,pHDB->Params.pSys);
        }
        else
        {
            RetVal = LSA_RET_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower RQB-memory failed!");
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"No Dataset demanded from the Ethenet!");
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_get(RetVal: 0x%X, SendLen: %d)", RetVal, SendLen);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_Identify()                                        */
/* Identify Request will be treated.                                         */
/*****************************************************************************/
LSA_UINT16 OHA_LOCAL_FCT_ATTR oha_dcp_Identify (OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
                                                OHA_HDB_DCP_PTR_TYPE         pHDB)
{
    OHA_DCP_LOWER_RQB_PTR_TYPE RbPtr;
    DCP_USR_BYTE_PTR        DstMacPtr;
    DCP_USR_RSP_DATA_PTR    ResultPtr;
    LSA_INT                 RespLen = 0;
    LSA_INT                 RespOptCnt = 0;
    LSA_UINT16              RetVal  = LSA_RET_OK;
    DCP_RQB_ARGS_RECV_IND DCP_UPPER_MEM_ATTR * pRecv = &pRQB->args.RecvInd;
    DCP_USR_REQ_DATA_PTR    ReqPtr  = (DCP_USR_REQ_DATA_PTR)(pRecv->DataPtr);
	LSA_UINT16              NICId   = pHDB->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_Identify(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    /* allocate Response-RQB */
    RbPtr = oha_dcp_alloc_rsp_rqb(pHDB);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(RbPtr, LSA_NULL) )
    {
        DstMacPtr =  DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_GET(RbPtr);
        ResultPtr = (DCP_USR_RSP_DATA_PTR)DCP_UPPER_RQB_RESP_REQ_DATA_PTR_GET(RbPtr);

        /* if the local station shall respond... */
        RespLen = oha_dcp_IdentifyFilterCheck(NICId, ReqPtr, pRecv->OptCnt, ResultPtr, &RespOptCnt);
        if (RespLen >= 0)
        {
            /* fill buffer for sending */
            RespLen = oha_dcp_IdentifyRspSet(NICId, ResultPtr, RespLen, &RespOptCnt);

            /* Take over the Source-MAC-Address as Destination-MAC-Address
             * and write further RQB-Parameters */
            OHA_MEMCPY(DstMacPtr, pRecv->AddrPtr, OHA_MAC_ADDR_SIZE);
            DCP_UPPER_RQB_RESP_REQ_OPT_CNT_SET(RbPtr, RespOptCnt);
            DCP_UPPER_RQB_RESP_REQ_TRANS_ID_SET(RbPtr, pRecv->TransactId);
            DCP_UPPER_RQB_RESP_REQ_SERVICE_ID_SET(RbPtr, DCP_IDENTIFY);
            pHDB->TxPendingCnt++;

            /* send Response    */
            OHA_DCP_REQUEST_LOWER(RbPtr,pHDB->Params.pSys);
        }
        else
        {
            oha_dcp_free_rsp_rqb(pHDB, RbPtr);
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_Identify(RetVal: 0x%X, RespLen: %d)", RetVal, RespLen);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_CheckHello()                                      */
/*                                                                           */
/* Check of the DCP-Hello-ReqPDU (see coding PNIO draft):                    */
/*  DCP-UC-Header, NameOfStationBlockRes, { IPParameterBlockRes,             */
/*  DeviceIDBlockRes, [DeviceOptionsBlockRes v DeviceVendorBlockRes], DeviceRoleBlockRes, */
/*  DeviceInitiativeBlockRes, [DeviceInstanceBlockRes] } f                   */
/*                                                                           */
/* f Additional blocks may be appended by the sender.                        */
/*                                                                           */
/*****************************************************************************/
LSA_UINT16 oha_dcp_CheckHello(DCP_USR_BYTE_PTR DataPtr, 
							LSA_INT	OptCnt)
{
    LSA_UINT16              RetVal;
    LSA_INT                 i;

	LSA_UINT32              Options = 0;
	LSA_UINT32              NeededOptions = OHA_CTRL_STATION_NAME | OHA_CTRL_STATION_ID | OHA_CTRL_PNIO_ROLE | OHA_CTRL_IP_SUITE;
	LSA_BOOL                DevInitOption = LSA_FALSE;
    LSA_UINT16              OptionListCount = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_CheckHello(DataPtr: 0x%X, OptCnt: 0x%X)",
                          DataPtr, OptCnt);

    /* check TLV-chain */
    for (i = 1, RetVal = OHA_OK; i <= OptCnt && RetVal == OHA_OK; i++)
    {
		DCP_USR_REQ_DATA_PTR ReqPtr = (DCP_USR_REQ_DATA_PTR)DataPtr;
        DCP_USR_DATASET_PTR  DsPtr  = &ReqPtr->Hello.Dataset;

        LSA_INT ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);

        if ((ReadLen % 2) != 0)
        {
            ReadLen++;   /* jump over fillbyte */
        }

        switch (DsPtr->Hdr.Opt)
		{
			case DCP_DBID_IDENT:
				switch (DsPtr->Hdr.Id)
				{
					case DCP_DSID_IDNAME: /* NameOfStationBlockRes */
						Options |= OHA_CTRL_STATION_NAME;
						RetVal = (i == 1) ? (LSA_UINT16)OHA_OK : (LSA_UINT16)OHA_ERR_PROTOCOL; /* NameOfStationBlockRes must be the first block */
						break;
					case DCP_DSID_IDID: /* DeviceIDBlockRes */
						Options |= OHA_CTRL_STATION_ID;
						RetVal = (i == 3) ? (LSA_UINT16)OHA_OK : (LSA_UINT16)OHA_ERR_PROTOCOL; /* DeviceIDBlockRes must be the 3rd block */
						break;
					case DCP_DSID_IDOPTS: /* DeviceOptionsBlockRes */
					case DCP_DSID_IDTYPE: /* TFS 962641: DeviceVendorBlockRes */
						OptionListCount++;
						RetVal = (i == 4) ? (LSA_UINT16)OHA_OK : (LSA_UINT16)OHA_ERR_PROTOCOL; /* DeviceOptionsBlockRes or DeviceVendorBlockRes must be the 4th block */
						break;
					case DCP_DSID_IDROLE: /* DeviceRoleBlockRes */
						Options |= OHA_CTRL_PNIO_ROLE;
						RetVal = (i == (4 + OptionListCount)) ? (LSA_UINT16)OHA_OK : (LSA_UINT16)OHA_ERR_PROTOCOL; /* must follow the optional DeviceOptionsBlockRes */
						break;
					default: /* additional block */
						RetVal = OHA_ERR_PROTOCOL;
						break;
				}
				break;
			case DCP_DBID_TCP: /* IPParameterBlockRes */
				if(DsPtr->Hdr.Id == DCP_DSID_TCPIP)
				{
					Options |= OHA_CTRL_IP_SUITE;
					RetVal = (i == 2) ? (LSA_UINT16)OHA_OK : (LSA_UINT16)OHA_ERR_PROTOCOL; /* IPParameterBlockRes must be the second block */
				}
				else
				{
					RetVal = OHA_ERR_PROTOCOL;
				}
				break;
			case DCP_DBID_DEVINIT: /* DeviceInitiativeBlockRes */
				if(DsPtr->Hdr.Id == DCP_DSID_DEVINIT)
				{
					DevInitOption = LSA_TRUE;
					RetVal = (i == (5 + OptionListCount)) ? (LSA_UINT16)OHA_OK : (LSA_UINT16)OHA_ERR_PROTOCOL; /* must follow the DeviceRoleBlockRes */
				}
				else
				{
					RetVal = OHA_ERR_PROTOCOL;
				}
				break;
			default: /* Additional block */
				RetVal = OHA_ERR_PROTOCOL;
				break;
		}

		if(RetVal != OHA_OK)
		{
			OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN,"Wrong order (0x%X) or ivalid additional block (Option 0x%X, Id 0x%X)!", i, DsPtr->Hdr.Opt, DsPtr->Hdr.Id);
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_CheckHello(RetVal: 0x%X)",RetVal);
			return(RetVal);
		}

		DataPtr += ReadLen;
    }

	if ((Options & NeededOptions) != NeededOptions)
	{
		OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_WARN,"Wrong coding - Block(s) missing (Options: 0x%X)!", Options);
		RetVal = OHA_ERR_PROTOCOL;
	}
	if (!DevInitOption)
	{
		OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_WARN,"Wrong coding - DeviceInitiativeBlockRes missing!");
		RetVal = OHA_ERR_PROTOCOL;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_CheckHello(RetVal: 0x%X)",RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_Hello()                                           */
/* HELLO Request will be done.                                               */
/*****************************************************************************/
LSA_UINT16      oha_dcp_Hello(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB
)
{
    LSA_INT                 ReqLen;
    LSA_INT                 ReadLen;
    LSA_UINT16              NICId = pHDB->Params.pDetail->Params.Dcp.NICId;

    LSA_UINT16              RetVal;
    DCP_RQB_ARGS_RECV_IND DCP_UPPER_MEM_ATTR * pRecv = &pRQB->args.RecvInd;
    DCP_USR_REQ_DATA_PTR    ReqPtr  = (DCP_USR_REQ_DATA_PTR)(pRecv->DataPtr);
    OHA_HELLO_INDICATION_TYPE   HelloInd;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_Hello(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_MEMSET(&HelloInd, 0, sizeof(HelloInd));

    HelloInd.FilterID = pRecv->FilterId;

    /* take over the MAC-Address */
    OHA_MEMCPY(HelloInd.MacAddr, pRecv->AddrPtr, OHA_MAC_ADDR_SIZE);

	RetVal = oha_dcp_CheckHello(pRecv->DataPtr, pRecv->OptCnt);
	if (RetVal != OHA_OK)
	{
		/* TFS 909130: Wrong coding of the DCP-Set-ReqPDU, see PNIO draft */
		RetVal = OHA_ERR_PROTOCOL;
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"Wrong coding of the DCP-Hello-ReqPDU (RetVal: 0x%X)!", RetVal);

        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: oha_dcp_Hello(RetVal: 0x%X)", RetVal);
		return(RetVal);
	}

    /* Check Request: At minimum one dataset existing */
    if (pRecv->OptCnt > 0)
    {
        DCP_USR_DATASET_PTR DsPtr   = &ReqPtr->Hello.Dataset;
        DCP_USR_BYTE_PTR    ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;
        LSA_UINT8           Result;
        LSA_UINT16          DevInit = OHA_DCP_HELLO_OFF;
        LSA_INT     i;

        /* get length of TLV-chain */
        for (i = 0, ReqLen = 0; i < pRecv->OptCnt; i++)
        {
            ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);
            if ((ReadLen % 2) != 0)
            {
                /* jump over fillbyte */
                ReadLen++;
            }

            ReqLen += ReadLen;
            ReadPtr += ReadLen;
            DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;
        }

        DsPtr = &ReqPtr->Set.Dataset;
        ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;
		Result = DCP_RESULT_OK;

        while (ReqLen > 0)
        {
            ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);

            if (ReqLen >= ReadLen)
            {
                LSA_UINT8   DbaseID = DsPtr->Hdr.Opt;

                switch (DbaseID)
                {
                    case DCP_DBID_TCP:          /* TCP/IP-Parameter 1 */
                        Result |= oha_dcp_HelloOptTcp(DsPtr, &HelloInd);  /* MacAddr, IpAddr, SubnetMask, DefRouterAddr */
                        break;
                    case DCP_DBID_IDENT:        /* Allgem. Geraeteeigenschaften 2 */
                        Result |= oha_dcp_HelloOptIdent(DsPtr, &HelloInd); /* StationType, StationName, StationId */
                        break;
                    case DCP_DBID_DEVINIT:      /* Device Initiative  */
                        Result |= oha_dcp_HelloOptDevInit(DsPtr, &DevInit);  /* Device Initiative */
                        break;
                    default:                    /* Unknown Dataset */
						Result |= DCP_RESULT_OK;
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"Indication HELLO: An unknown option occurred (DbaseID: 0x%X)",DbaseID);
                        break;
                }
            }

            /* Further Dataset in the request? */
            if ((ReadLen % 2) != 0)
            {
                /* jump over fillbyte */
                ReadLen++;
            }
            ReqLen -= ReadLen;
            if (ReqLen != 0)
            {
                ReadPtr += ReadLen;
                DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;
            }
        }

        /* a set of parameters */
        if (Result == DCP_RESULT_OK)
        {
            /* send HELLO indication only if "DeviceInitiativeValue.Hello = ON" */
            if (DevInit & OHA_DCP_HELLO_ON)
            {
                OHA_DcpHelloEventAllUsers(&HelloInd, NICId);
            }
        }
        else
        {
            RetVal = LSA_RET_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Got no Dataset from the Ethenet!");
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Got no Dataset from the Ethenet!");
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_Hello(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_GetOptPbus()                                      */
/* Requested dataset of the Profibus-Database is written to the GET-Response */
/*****************************************************************************/
LSA_INT                 oha_dcp_GetOptPbus(
    LSA_UINT16              NICId,
    LSA_UINT16              Type,
    DCP_USR_DATASET_PTR     DsPtr,
    LSA_UINT32          *   DcpGetStatus )
{
    LSA_UINT16      RetVal;
    LSA_INT         DsLen = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE ParamPtr = LSA_NULL;
    LSA_UINT8       DsID   = OHA_LOBYTE(Type);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_GetOptPbus(NICId: 0x%X, DsPtr: 0x%X, DsID:0x%X)",
                          NICId, DsPtr, DsID);

    switch (DsID)
    {
            /* get dataset fron the system and write it to the response
            */
        case DCP_DSID_PBPROFIL:         /* suboption Busprofil (obsolete) */
            break;
        case DCP_DSID_PBPARAM:          /* suboption, further parameter (obsolete) */
            break;

        case DCP_DSID_PBCOMMON:         /* suboption common parameters */
            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON)) /* is'nt it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_PROFIBUS_COMMON;
            }
            RetVal = OHA_DB_READ_PROFIBUS_COMMON(NICId, &ParamPtr);
            if (RetVal == LSA_RET_OK && OHA_IS_NOT_NULL(ParamPtr))
            {
                if(OHA_IS_NOT_NULL(DsPtr))
                {
					DsPtr->Status = DCP_RESERVED;
					DsPtr->Hdr.Opt = DCP_DBID_PBPARAM;
					DsPtr->Hdr.Id = DCP_DSID_PBCOMMON;
					DsPtr->Hdr.Len = OHA_SWAP16(DCP_DS_PBCOMMON_SIZE+2+3/*OUI*/);

					DsPtr->Data.PbManufact.OUI[0] = 0x08;
					DsPtr->Data.PbManufact.OUI[1] = 0x00;
					DsPtr->Data.PbManufact.OUI[2] = 0x06;
					OHA_MEMCPY(DsPtr->Data.PbManufact.String, ParamPtr, DCP_DS_PBCOMMON_SIZE);
				}
                DsLen = DCP_DS_HDR_SIZE+2+3/*OUI*/ + DCP_DS_PBCOMMON_SIZE;
            }
            break;

        case DCP_DSID_PBADDITION:          /* suboption, additional parameters */
            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_PBPARAM, DCP_DSID_PBADDITION)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_PROFIBUS_ADDITION;
            }
            RetVal = OHA_DB_READ_PROFIBUS_ADDITION(NICId, &ParamPtr);
            if (RetVal == LSA_RET_OK && OHA_IS_NOT_NULL(ParamPtr))
            {
                if(OHA_IS_NOT_NULL(DsPtr))
                {
					DsPtr->Status = DCP_RESERVED;
					DsPtr->Hdr.Opt = DCP_DBID_PBPARAM;
					DsPtr->Hdr.Id = DCP_DSID_PBADDITION;
					DsPtr->Hdr.Len = OHA_SWAP16(DCP_DS_PBADDITION_SIZE+2+3/*OUI*/);

					DsPtr->Data.PbManufact.OUI[0] = 0x08;
					DsPtr->Data.PbManufact.OUI[1] = 0x00;
					DsPtr->Data.PbManufact.OUI[2] = 0x06;
					OHA_MEMCPY(DsPtr->Data.PbManufact.String, ParamPtr, DCP_DS_PBADDITION_SIZE);
				}
                DsLen = DCP_DS_HDR_SIZE+2+3/*OUI*/ + DCP_DS_PBADDITION_SIZE;
            }
            break;

        default:                    /* unknown dataset */
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_GetOptPbus(), DsPtr: 0x%X, DsLen:0x%X", DsPtr, DsLen);
    return(DsLen);
}

/*****************************************************************************/
/* Local function: oha_dcp_GetTcpIpAddr()                                    */
/* Get the address-information for TCP/IP-Dataset from the System and store  */
/*****************************************************************************/
LSA_INT                 oha_dcp_GetTcpIpAddr(
    LSA_UINT16              NICId,
    DCP_USR_DATASET_PTR     DsPtr)
{
    LSA_UINT16      Status;
    OHA_COMMON_MEM_U8_PTR_TYPE IpAdrPtr         = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr    = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtrPtr = LSA_NULL;
    LSA_INT         DataLen =  0;
    OHA_IP_TYPE     DefaultAdr = {0,0,0,0};
    OHA_DB_IF_PTR_TYPE pDB;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_GetTcpIpAddr(NICId: 0x%X, DsPtr: 0x%X), DataLen:0x%X",
                          NICId, DsPtr, DataLen);

    pDB = OHA_DB_GET_INTERFACE(NICId);
    OHA_ASSERT_NULL_PTR(pDB);

    /* read the ip address */
    (LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr); /* local IP-Address */

    /* AP00827191, AP00818951 */
    /* In case of no IP address (RtF or set to 0,0,0,0) the DCP_BlockInfoIPstatus must be set to "No_IP_Parameter" */
    if (OHA_IS_NULL(IpAdrPtr) ||
        OHA_MEMCMP(IpAdrPtr, (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP, OHA_IP_ADDR_SIZE))    /* IP set to 0.0.0.0 */
    {
        Status = DCP_STATUS_TCPIP_NONE;
    }
    else
    {
        Status = OHA_DB_READ_TCPIP_STATUS(NICId);
    }

    if (Status == DCP_STATUS_TCPIP_NONE)
    {
		/* AP00862725: address conflict bit not set */
        if (pDB->DcpData.IpConflictDetected)
        {
            OhaGetConflictIp(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr, LSA_NULL); /* RealIP (invalid) */
            Status |= DCP_STATUS_TCPIP_CONFLICT;
        }
        else
		{
			/* take Default Address */
			IpAdrPtr = SubnetMaskPtr = RouterAddrPtrPtr = DefaultAdr;
		}
    }
    else
    {
        if (pDB->DcpData.IpConflictDetected)
        {
            OhaGetConflictIp(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr, LSA_NULL); /* RealIP (invalid) */
            Status |= DCP_STATUS_TCPIP_CONFLICT;
        }
        else
        {
            (LSA_VOID)OHA_DB_READ_IP_SUITE(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr); /* local IP-Address */
        }
    }

    OHA_ASSERT_NULL_PTR(IpAdrPtr);
    OHA_ASSERT_NULL_PTR(SubnetMaskPtr);
    OHA_ASSERT_NULL_PTR(RouterAddrPtrPtr);

    if (OHA_IS_NOT_NULL(DsPtr))
    {
        /* ...write to dataset and set the status   */
        OHA_MEMCPY(DsPtr->Data.TcpIp.IpAddr, IpAdrPtr, OHA_IP_ADDR_SIZE);

        /* PNO relationship between station-ip and next gateway-ip (AP00462068): */
        /* If NG-IP == Station-IP the subnet mask is don't care. */
        /* That means - NO NextGateway! DCP_Get delivers the subnet mask with '0.0.0.0'! */

        /* AP00521755 */
        /* if (OHA_MEMCMP(IpAdrPtr, RouterAddrPtrPtr, OHA_IP_ADDR_SIZE)) */
        /* { */
        /*  OHA_MEMCPY(DsPtr->Data.TcpIp.SubnetMask, DefaultAdr, OHA_IP_ADDR_SIZE); */
        /* } */
        /* else */
        {
            OHA_MEMCPY(DsPtr->Data.TcpIp.SubnetMask, SubnetMaskPtr, OHA_IP_ADDR_SIZE);
        }
        OHA_MEMCPY(DsPtr->Data.TcpIp.DefaultRouter, RouterAddrPtrPtr, OHA_IP_ADDR_SIZE);

        DsPtr->Status = OHA_SWAP16(Status);
        DsPtr->Hdr.Id = DCP_DSID_TCPIP;
        DsPtr->Hdr.Opt = DCP_DBID_TCP;
        DsPtr->Hdr.Len = OHA_SWAP16(DCP_DS_TCPIP_SIZE+2);
    }

    DataLen = DCP_DS_HDR_SIZE+2 + DCP_DS_TCPIP_SIZE;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_GetTcpIpAddr(), DsPtr: 0x%X, DataLen:0x%X", DsPtr, DataLen);
    return(DataLen);
}

/*****************************************************************************/
/* Local function: oha_dcp_GetOptTcp()                                       */
/* requested dataset of the TCP-database will be written to the GET-Response */
/*****************************************************************************/
LSA_INT                 oha_dcp_GetOptTcp(
    LSA_UINT16              NICId,
    LSA_UINT16              Type,
    DCP_USR_DATASET_PTR     DsPtr,
    LSA_UINT32          *   DcpGetStatus )
{
    LSA_INT     DsLen = 0;
    LSA_UINT8   DsID  = OHA_LOBYTE(Type);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_GetOptTcp(NICId: 0x%X, DsPtr: 0x%X), DsID:0x%X",
                          NICId, DsPtr, DsID);

    switch (DsID)
    {
            /* get values of the dataset from the system and write it to the response */
        case DCP_DSID_MACADR:       /* MAC-Address */
        {
            OHA_COMMON_MEM_U8_PTR_TYPE  AddrPtr = LSA_NULL;
            LSA_INT Len = OHA_DB_READ_MAC_ADDR(NICId, &AddrPtr);
            if (Len == DCP_DS_MACADDR_SIZE && AddrPtr)
            {
                if(OHA_IS_NOT_NULL(DsPtr))
                {
                    OHA_MEMCPY(&DsPtr->Data.MacAddr, AddrPtr, DCP_DS_MACADDR_SIZE);
                    /* DCP_DS_SET_ADDR(&DsPtr->Data.MacAddr, dcp_SrvDetailPtr->LocalMacAdr, DCP_DS_MACADDR_SIZE); */

                    DsPtr->Status = DCP_RESERVED;
                    DsPtr->Hdr.Opt = OHA_HIBYTE(Type);
                    DsPtr->Hdr.Id = DCP_DSID_MACADR;
                    DsPtr->Hdr.Len = OHA_SWAP16(DCP_DS_MACADDR_SIZE+2);
                }
                DsLen = DCP_DS_HDR_SIZE+2 + DCP_DS_MACADDR_SIZE;
            }
            if (oha_db_read_get_access(NICId,
                                       OHA_DCP_OPTION_TCP, DCP_DSID_MACADR))  /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_MAC_ADDR;
            }
        }
        break;

        case DCP_DSID_TCPIP:        /* IP-Parameter */

            DsLen = oha_dcp_GetTcpIpAddr(NICId, DsPtr);
            if (DsLen != 0)
            {
                if(OHA_IS_NOT_NULL(DsPtr))
                {
                    DsPtr->Hdr.Opt = OHA_HIBYTE(Type);
                }
            }
            if (oha_db_read_get_access(NICId,
                                       OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_IP_SUITE;
            }
            break;

        default:                    /* unknown dataset */
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_GetOptTcp(), DsPtr: 0x%X, DsLen:0x%X", DsPtr, DsLen);
    return(DsLen);
}

/*****************************************************************************/
/* Local function: oha_dcp_GetOptDhcp()                                      */
/* requested DHCP-Option will be written to the GET-Response                 */
/*****************************************************************************/
LSA_INT                 oha_dcp_GetOptDhcp(
    LSA_UINT16              NICId,
    LSA_UINT16              Type,
    DCP_USR_DATASET_PTR     DsPtr,
    LSA_UINT32          *   DcpGetStatus )
{
    LSA_UINT16      RetVal;
    LSA_UINT8     * OptPtr = LSA_NULL;
    /* LSA_BOOL     NextOpt = LSA_FALSE; */
    LSA_INT         DsLen  = 0;
    LSA_UINT8       DsID   = OHA_LOBYTE(Type);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_GetOptDhcp(NICId: 0x%X, DsPtr: 0x%X), DsID:0x%X",
                          NICId, DsPtr, DsID);

    /* get DHCP-option from the system */
    if (oha_db_read_get_access(NICId,
                               DCP_DBID_DHCP, DsID)) /* is it locked ? */
    {
        (*DcpGetStatus) |= OHA_CTRL_DHCP_OPTION;
    }
    RetVal = OHA_DB_READ_DHCP_OPTION(NICId, &OptPtr, DsID);
    if (RetVal == LSA_RET_OK)
    {
        if (!LSA_HOST_PTR_ARE_EQUAL(OptPtr, LSA_NULL)
            &&  (*OptPtr == DsID))
        {
            /* and write it to the GET-Response */
            if(OHA_IS_NOT_NULL(DsPtr))
            {
                DsPtr->Data.Dhcp.OptionCode = *(OptPtr);
                DsPtr->Data.Dhcp.OptionLen  = *(OptPtr+1);
                /* DCP_DS_SET_ADDR(DsPtr->Data.Dhcp.OptionVal , OptPtr+2, *(OptPtr+1)); */
                OHA_MEMCPY(DsPtr->Data.Dhcp.OptionVal , OptPtr+2, *(OptPtr+1));
            }
            DsLen += *(OptPtr+1) + 2;
            if(OHA_IS_NOT_NULL(DsPtr))
            {
                DsPtr->Status = DCP_RESERVED;
                DsPtr->Hdr.Opt = DCP_DBID_DHCP;
                DsPtr->Hdr.Id = DsID;
                DsPtr->Hdr.Len = OHA_SWAP16(DsLen+2);
            }
            DsLen += DCP_DS_HDR_SIZE+2;
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_GetOptDhcp(), DsPtr: 0x%X, DsLen:0x%X", DsPtr, DsLen);
    return(DsLen);
}


/*****************************************************************************/
/* Local function: dcp_GetOptIdent()                                         */
/* requested dataset of the database "General device-properties" (8)         */
/* are written to the GET-Response                                           */
/*****************************************************************************/
LSA_INT                 oha_dcp_GetOptIdent(
    LSA_UINT16              NICId,
    LSA_UINT16              Type,
    DCP_USR_DATASET_PTR     DsPtr,
    LSA_UINT32          *   DcpGetStatus )
{
    DCP_SYS_BYTE_PTR_TYPE RetPtr   = LSA_NULL;
    DCP_SYS_WORD_PTR_TYPE DbasePtr = LSA_NULL;

    LSA_INT         DbaseCnt;
    LSA_UINT8       Role;
    /* LSA_UINT8    IdType; */
    LSA_INT         i;
    LSA_INT         RetLen;
    LSA_INT         DsLen = 0;
    LSA_UINT8       DsID  = OHA_LOBYTE(Type);
    OHA_DB_IF_PTR_TYPE pDB;

    pDB = OHA_DB_GET_INTERFACE(NICId);
	OHA_ASSERT_NULL_PTR(pDB);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_GetOptIdent(NICId: 0x%X, DsPtr: 0x%X), DsID:0x%X",
                          NICId, DsPtr, DsID);

    switch (DsID)
    {
        case DCP_DSID_IDTYPE:       /* Stationstyp */

            /* get the type of station and write it to the response dataset */
            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_IDENT, DCP_DSID_IDTYPE)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_STATION_TYPE;
            }
            RetLen = OHA_DB_READ_STATION_TYPE(NICId, &RetPtr);
            if ((RetLen >= 0) && (!LSA_HOST_PTR_ARE_EQUAL(RetPtr, LSA_NULL)))
            {
                if(OHA_IS_NOT_NULL(DsPtr))
                {
                    for (i = 0; i < RetLen; i++)
                        DsPtr->Data.IdentType.String[i] = RetPtr[i];
                    DsPtr->Status = DCP_RESERVED;
                    DsPtr->Hdr.Opt = DCP_DBID_IDENT;
                    DsPtr->Hdr.Id = DCP_DSID_IDTYPE;
                    DsPtr->Hdr.Len = OHA_SWAP16(RetLen+2);
                }               
                DsLen = DCP_DS_HDR_SIZE+2 + RetLen;
            }
            break;

        case DCP_DSID_IDNAME:       /* Stationsname */

            /* get the name of station and write it to the response dataset */
            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_IDENT, DCP_DSID_IDNAME)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_STATION_NAME;
            }
            RetLen = OHA_DB_READ_STATION_NAME(NICId, &RetPtr);

			if(OHA_IS_NOT_NULL(DsPtr))
			{
				if ((RetLen >= 0) && OHA_IS_NOT_NULL(RetPtr))
				{
					/* RetLen == 0 means: station has no name */
					for (i = 0; i < RetLen; i++)
						DsPtr->Data.IdentName.String[i] = RetPtr[i];
				}

				DsPtr->Hdr.Opt = DCP_DBID_IDENT;
				DsPtr->Hdr.Id = DCP_DSID_IDNAME;
				DsPtr->Status = DCP_RESERVED;
				DsPtr->Hdr.Len = OHA_SWAP16(RetLen+2);
			}

            DsLen = DCP_DS_HDR_SIZE+2 + RetLen;
            break;

        case DCP_DSID_IDID:         /* Stationsidentification */
			{
            /* get the Identification of the station and
             * if existing, write it to the response dataset */
			LSA_UINT8 DevIdent[2*sizeof(LSA_UINT16)];

            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_IDENT, DCP_DSID_IDID)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_STATION_ID;
            }

			if(pDB->PNIdentValid.Existing) /* record written ? */
			{
			    OHA_PUT16_HTON(DevIdent, 0, pDB->PNIdentValid.VendorID);
			    OHA_PUT16_HTON(DevIdent, 2, pDB->PNIdentValid.DeviceID);

				RetPtr = DevIdent;
				RetLen = sizeof(DevIdent);
			}
			else
			{
	            RetLen = OHA_DB_READ_STATION_ID(NICId, &RetPtr/*, &IdType*/);
			}

            if ((RetLen > 0) && (!LSA_HOST_PTR_ARE_EQUAL(RetPtr, LSA_NULL)))
            {
				if(OHA_IS_NOT_NULL(DsPtr))
				{
					/* DeviceIDValue has length of 4 Bytes in PNIO */
					for (i = 0; i < RetLen; i++)
					{
						DsPtr->Data.IdentId.Value[i] = RetPtr[i];
					}

					DsPtr->Status = DCP_RESERVED;
					DsPtr->Hdr.Opt = DCP_DBID_IDENT;
					DsPtr->Hdr.Id = DCP_DSID_IDID;
					DsPtr->Hdr.Len = OHA_SWAP16(RetLen+2);
				}

                DsLen = DCP_DS_HDR_SIZE+2 + RetLen;
            }
			}
            break;

        case DCP_DSID_IDDEVINST:         /* DeviceInstance */
			{
			/* TIA 855557: Local variable used out of scope of validity */
			LSA_UINT8 DevInst[sizeof(pDB->PNIdentValid.InstanceID)];

			/* get the Instance of the station and
             * if existing, write it to the response dataset */
            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_IDENT, DCP_DSID_IDDEVINST)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_DEVICE_INSTANCE;
            }

			if(pDB->PNIdentValid.Existing) /* record written ? */
			{
			    OHA_PUT16_HTON(DevInst, 0, pDB->PNIdentValid.InstanceID);

				RetPtr = DevInst;
				RetLen = sizeof(DevInst);
			}
			else
			{
				RetLen = OHA_DB_READ_DEVICE_INSTANCE(NICId, &RetPtr);
			}

            if ((RetLen > 0) && (!LSA_HOST_PTR_ARE_EQUAL(RetPtr, LSA_NULL)))
            {
				if(OHA_IS_NOT_NULL(DsPtr))
				{
					/* DeviceInstanceValue has length of 2 Bytes in PNIO */
					for (i = 0; i < RetLen; i++)
					{
						DsPtr->Data.IdentId.Value[i] = RetPtr[i];
					}

					DsPtr->Status = DCP_RESERVED;
					DsPtr->Hdr.Opt = DCP_DBID_IDENT;
					DsPtr->Hdr.Id = DCP_DSID_IDDEVINST;
					DsPtr->Hdr.Len = OHA_SWAP16(RetLen+2);
				}

                DsLen = DCP_DS_HDR_SIZE+2 + RetLen;
            }
			}
            break;

        case DCP_DSID_IDOEMID:         /* SuboptionOEMDeviceID */
			{
			/* get the OEMDeviceID of the station and
             * if existing, write it to the response dataset */
            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_IDENT, DCP_DSID_IDOEMID)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_OEM_DEVICE_ID;
            }

			RetLen = OHA_DB_READ_OEM_DEVICE_ID(NICId, &RetPtr);

            if ((RetLen > 0) && (!LSA_HOST_PTR_ARE_EQUAL(RetPtr, LSA_NULL)))
            {
				if(OHA_IS_NOT_NULL(DsPtr))
				{
					/* SuboptionOEMDeviceID has length of 4 Bytes in PNIO */
					for (i = 0; i < RetLen; i++)
					{
						DsPtr->Data.IdentId.Value[i] = RetPtr[i];
					}

					DsPtr->Status = DCP_RESERVED;
					DsPtr->Hdr.Opt = DCP_DBID_IDENT;
					DsPtr->Hdr.Id = DCP_DSID_IDOEMID;
					DsPtr->Hdr.Len = OHA_SWAP16(RetLen+2);
				}

                DsLen = DCP_DS_HDR_SIZE+2 + RetLen;
            }
			}
            break;

        case DCP_DSID_IDROLE:       /* ProfinetIO Role */
			{
				if (oha_db_read_get_access(NICId,
										   DCP_DBID_IDENT, DCP_DSID_IDROLE)) /* is it locked ? */
				{
					(*DcpGetStatus) |= OHA_CTRL_ANY_OTHER_OPTION;
				}

				Role = OHA_DB_READ_PROFINET_ROLE(NICId);			
				if(OHA_IS_NOT_NULL(DsPtr))
				{
					DsPtr->Data.IdentRole.Role = Role;
					DsPtr->Data.IdentRole.Reserved = DCP_RESERVED;
					DsPtr->Status = DCP_RESERVED;
					DsPtr->Hdr.Opt = DCP_DBID_IDENT;
					DsPtr->Hdr.Id = DCP_DSID_IDROLE;
					DsPtr->Hdr.Len = OHA_SWAP16(DCP_DS_IDROLE_SIZE+2);
				}
				DsLen = DCP_DS_HDR_SIZE+2 + DCP_DS_IDROLE_SIZE;
			}
            break;

        case DCP_DSID_IDOPTS:       /* Device supported Types (Options/Subopt.) */

            /* read directory of the database/datasets */
            if (oha_db_read_get_access(NICId,
                                       DCP_DBID_IDENT, DCP_DSID_IDOPTS)) /* is it locked ? */
            {
                (*DcpGetStatus) |= OHA_CTRL_ANY_OTHER_OPTION;
            }
            DbaseCnt = OHA_DB_READ_SUPPORTED_DATABASE(NICId, &DbasePtr);
			/* AP01501878(Get.rsp): only optional and manufaturer specifif options -> empty block if none */
			if(OHA_IS_NOT_NULL(DsPtr))
			{
				for (i = 0; i < DbaseCnt; i++)
					DsPtr->Data.IdentOpts.Type[i] = OHA_SWAP16(DbasePtr[i]);

				DsPtr->Status = DCP_RESERVED;
				DsPtr->Hdr.Opt = DCP_DBID_IDENT;
				DsPtr->Hdr.Id = DCP_DSID_IDOPTS;
				DsPtr->Hdr.Len = OHA_SWAP16((DbaseCnt*2)+2);        /* count of bytes */
			}
            DsLen = DCP_DS_HDR_SIZE+2 + (DbaseCnt * 2);
            break;

        default:                    /* unknown dataset */
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_GetOptIdent(), DsPtr: 0x%X, DsLen:0x%X", DsPtr, DsLen);
    return(DsLen);
}


/*****************************************************************************/
/* Local function: oha_dcp_alloc_rsp_rqb()                                   */
/* allocate a Response-RQB for DCP                                           */
/*****************************************************************************/
OHA_DCP_LOWER_RQB_PTR_TYPE oha_dcp_alloc_rsp_rqb(OHA_HDB_DCP_PTR_TYPE pHDB)
{
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_USER_ID_TYPE            UserId;
    OHA_DCP_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL; /* Pointer to TLV-Frame memory. */
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_alloc_rsp_rqb(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);
    pSys   =  pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate Response-RQB...     */
    OHA_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        /* ...set opcode of DCP_OPC_SEND_RSP,... */
        OHA_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_SEND_RSP);
        OHA_DCP_RQB_SET_HANDLE(pRQB,pHDB->DcpHandle);
        DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_SET(pRQB, LSA_NULL);
        DCP_UPPER_RQB_RESP_REQ_DATA_PTR_SET(pRQB, LSA_NULL);

        OHA_DCP_ALLOC_LOWER_MEM(&MemPtr, UserId, OHA_MAC_ADDR_SIZE, pSys);
        if (!LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
        {
            /* ...set address of the buffer and allocate the datablock */
            DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_SET(pRQB, (DCP_USR_BYTE_PTR)MemPtr);
            OHA_DCP_ALLOC_LOWER_MEM(&MemPtr, UserId, DCP_MAX_RSP_DATA_SIZE, pSys);

            if (!LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
            {
                /* ...set allocated datablock to the send-RB */
                DCP_UPPER_RQB_RESP_REQ_DATA_PTR_SET(pRQB, (DCP_USR_BYTE_PTR)MemPtr);
            }
            else
            {
                MemPtr = DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_GET(pRQB);
                OHA_DCP_FREE_LOWER_MEM(&RetVal, MemPtr, pSys);
                OHA_DCP_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
                pRQB = LSA_NULL;    /* AP00627762 */
            }
        }
        else
        {
            OHA_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            pRQB = LSA_NULL;    /* AP00627762 */
        }
    }
    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_alloc_rsp_rqb(pRQB: 0x%X)", pRQB);
    return (pRQB);
}

/*****************************************************************************/
/* Local function: oha_dcp_free_rsp_rqb()                                    */
/* free a lower Response-RQB for DCP                                         */
/*****************************************************************************/
LSA_VOID OHA_LOCAL_FCT_ATTR oha_dcp_free_rsp_rqb(OHA_HDB_DCP_PTR_TYPE pHDB,
                                                 OHA_DCP_LOWER_RQB_PTR_TYPE pRQB)
{
    OHA_DCP_LOWER_MEM_PTR_TYPE  MemPtr;
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_free_rsp_rqb(pHDB: 0x%X, pRQB: 0x%X)",
                          pHDB, pRQB);
    /* free allocated buffer */
    MemPtr = DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_GET(pRQB);
    if (!LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
	{
        OHA_DCP_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
	}
    MemPtr = DCP_UPPER_RQB_RESP_REQ_DATA_PTR_GET(pRQB);
    if (!LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
	{
        OHA_DCP_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
	}
    OHA_DCP_FREE_LOWER_RQB(&RetVal, pRQB, pHDB->Params.pSys);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_free_rsp_rqb(RetVal: 0x%X)", RetVal);
    return;
}


/*****************************************************************************/
/* Local function: oha_dcp_IdentifyFilterCheck()                             */
/* Identify-Filter NameOfStation, ID, Stationtype will be checkedand         */
/* pointer of stationtype (RetVal = length) will be written.   */
/*****************************************************************************/
LSA_INT                 oha_dcp_IdentifyFilterCheck(
    LSA_UINT16              NICId,
    DCP_USR_REQ_DATA_PTR    ReqPtr,
    LSA_INT                 OptCnt,
    DCP_USR_RSP_DATA_PTR    RspPtr,
    LSA_INT             *   OptCntPtr)
{
    DCP_USR_DSIDENT_PTR     DsPtr;
    LSA_UINT16              Type;
    LSA_UINT16              DsLen;
    LSA_INT                 i, j;
    LSA_UINT16              CheckLen;
    LSA_INT                 RestLen;
    LSA_INT                 DsetLen;
    LSA_INT                 RetVal = -1;
    LSA_INT                 RetLen = 0;
    LSA_BOOL                FilterMatch = LSA_TRUE;
    /* DCP_USR_BYTE_PTR     CheckPtr = LSA_NULL; */
    DCP_USR_BYTE_PTR        BytePtr;
    DCP_USR_BYTE_PTR        WritePtr = &RspPtr->Identify.IdentDs.Hdr.Opt;
    LSA_UINT32              DcpSetStatus = 0;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : dcp_IdentifyFilterCheck(NICId: 0x%X, ReqPtr: 0x%X, RspPtr: 0x%X, OptCnt: 0x%X)",
                          NICId, ReqPtr, RspPtr, OptCnt);

    /* get length of filter chain */
    BytePtr = &ReqPtr->Identify.Filter.Opt;
    DsPtr = (DCP_USR_DSIDENT_PTR)BytePtr;
    for (i = 0, RestLen = 0; i < OptCnt; i++)
    {
        DsLen = (LSA_UINT16)(DCP_DS_IDENT_HDR_SIZE + OHA_SWAP16(DsPtr->Len));
        if ((DsLen % 2) != 0)
        {
            /* jump over fillbyte */
            DsLen++;
        }

        RestLen += DsLen;
        BytePtr += DsLen;
        DsPtr = (DCP_USR_DSIDENT_PTR)BytePtr;
    }

    if (RestLen > 0)
    {
        BytePtr = &ReqPtr->Identify.Filter.Opt;
    }
    else
        FilterMatch = LSA_FALSE;


    /* as long as filters in the request are existing... */
    while (FilterMatch && (RestLen > 0))
    {
        DCP_USR_DATASET_PTR     RdPtr;

        RdPtr = (DCP_USR_DATASET_PTR)WritePtr;
        DsPtr = (DCP_USR_DSIDENT_PTR)BytePtr;
        DsLen = OHA_SWAP16(DsPtr->Len);         /* here is no status in the dataset */
        FilterMatch = LSA_FALSE;
        DsetLen = 0;

        if ((DCP_DBID_NOFILTER == DsPtr->Opt) && (DCP_DSID_NOFILTER == DsPtr->Id)
            &&  (0 == DsLen))
        {
            /* NoFilter - Dummyfilter   */
            FilterMatch = LSA_TRUE;
        }
        else if ((DCP_DBID_IDENT == DsPtr->Opt) && (DCP_DSID_IDTYPE == DsPtr->Id))
        {
            /* a stationtype-filter (e.g. PBC)
             * ask the system, if it it is this type */
            if (DsLen <= 256)
                FilterMatch = OHA_DB_STATION_TYPE_CHECK(NICId, DsPtr->Data.IdentType.String,
                                                        DsLen);
            if (FilterMatch)
            {
                /* StationType - copy the dataset from the request to the response */
                DsetLen = OHA_SWAP16(DsPtr->Len);
                RdPtr->Hdr.Opt = DsPtr->Opt;
                RdPtr->Hdr.Id = DsPtr->Id;
                RdPtr->Hdr.Len = OHA_SWAP16(DsetLen+2);
                RdPtr->Status = DCP_RESERVED;
                for (i = 0; i < DsetLen; i++)
                    RdPtr->Data.IdentType.String[i] = DsPtr->Data.IdentType.String[i];
                DsetLen += DCP_DS_HDR_SIZE+2;
            }
        }
        else if ((DCP_DBID_IDENT == DsPtr->Opt) && (DCP_DSID_IDALIAS == DsPtr->Id))
        {
            /* if it is a AliasName-filter
             * ask DB, if such a AliasName is existing */
            if (DsLen <= 256)
            {
                FilterMatch = OHA_DB_ALIAS_NAME_CHECK(NICId, DsPtr->Data.IdentType.String, DsLen);
            }
            if (FilterMatch)
            {
                /* AliasName - copy dataset from request to the response */
                DsetLen = OHA_SWAP16(DsPtr->Len);
                RdPtr->Hdr.Opt = DsPtr->Opt;
                RdPtr->Hdr.Id = DsPtr->Id;
                RdPtr->Hdr.Len = OHA_SWAP16(DsetLen+2);
                RdPtr->Status = DCP_RESERVED;
                for (i = 0; i < DsetLen; i++)
                    RdPtr->Data.IdentType.String[i] = DsPtr->Data.IdentType.String[i];
                DsetLen += DCP_DS_HDR_SIZE+2;
            }
        }
        else
        {
            /* check any dataset */
            FilterMatch = LSA_FALSE;
            Type = OHA_WORD(DsPtr->Id, DsPtr->Opt);

            {
                /* if type in the directory is existing: get the dataset */
                switch (DsPtr->Opt)
                {
                    case DCP_DBID_TCP:          /* TCP/IP-Parameter */
                        DsetLen = oha_dcp_GetOptTcp(NICId, Type, RdPtr, &DcpSetStatus);
                        break;

                    case DCP_DBID_DHCP:         /* DHCP-Parameter */
                        DsetLen = oha_dcp_GetOptDhcp(NICId, Type, RdPtr, &DcpSetStatus);
                        break;

                    case DCP_DBID_IDENT:        /* general Device properties */
                        DsetLen = oha_dcp_GetOptIdent(NICId, Type, RdPtr, &DcpSetStatus);
                        break;

                    case DCP_DBID_PBPARAM:      /* profibus parameters */
                        DsetLen = oha_dcp_GetOptPbus(NICId, Type, RdPtr, &DcpSetStatus);
                        break;

                    default:                    /* unknown database */
                        DsetLen = 0;
                        break;
                }

                if (DsetLen != 0)
                {
                    /* the length of the dataset is the same... */
                    CheckLen = OHA_SWAP16(DsPtr->Len);
                    if (DsetLen == CheckLen + DCP_DS_HDR_SIZE+2)
                    {
                        /* compare the data with filter */
                        FilterMatch = LSA_TRUE;
                        if ((DCP_DSID_IDNAME == DsPtr->Id)
                            && (DCP_DBID_IDENT == DsPtr->Opt))
                        {
                            LSA_UINT8 CheckName[DCP_MAX_STATION_NAME_LEN];

                            CheckName[0] = 0;
                            /* if filter == NameOfStation: for comparism convert
                               upper to lower cases */
                            OHA_MEMCPY(CheckName, &RdPtr->Data.IdentName.String, CheckLen);
                            if (OHA_UPPER_CASES_ALLOWED(NICId))
                            {
                                OHA_STRNTOLOWER(CheckName, CheckLen);
                            }
                            for (i = 0, j = 4; i < CheckLen; i++, j++)
                            {
                                if (CheckName[i] != BytePtr[j])
                                {
                                    FilterMatch = LSA_FALSE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            for (i = DCP_DS_HDR_SIZE+2, j = 4; j < 4+CheckLen; i++, j++)
                            {
                                if (WritePtr[i] != BytePtr[j])
                                {
                                    FilterMatch = LSA_FALSE;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (FilterMatch)
        {
            /* get length of the rest of the filter
             * and set pointer to the next request-filter */
            RestLen -= 4 + DsLen;
            BytePtr += 4 + DsLen;
            if ((DsLen != 0) && ((DsLen % 2) != 0))
            {
                /* jump over fillbyte */
                RestLen--;
                BytePtr++;
            }

            /* continue with the datensatz in the response */
            if (DsetLen != 0)
                (*OptCntPtr)++;
            WritePtr += DsetLen;
            RetLen += DsetLen;
            if ((DsetLen != 0) && ((DsetLen % 2) != 0))
            {
                /* clear fillbyte */
                *WritePtr = 0;
                RetLen++;
                WritePtr++;
            }
        }
    }

    if (FilterMatch)
    {
        /* if all the fitler criterions are fulfilled
         * return the length of the response */
        RetVal = RetLen;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_IdentifyFilterCheck(), ReqPtr: 0x%X, RetVal:0x%X", ReqPtr, RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_IdentifyRspSet()                                  */
/* establish the DCP-Identify Response-PDU                                   */
/*****************************************************************************/
LSA_INT                 oha_dcp_IdentifyRspSet(
    LSA_UINT16              NICId,
    DCP_USR_RSP_DATA_PTR    DataPtr,
    LSA_INT                 DataLen,
    LSA_INT             *   OptCntPtr)
{
    DCP_USR_BYTE_PTR        BytePtr;
    LSA_UINT16              Type;
    LSA_UINT16              Status;
    LSA_INT                 Len;
    LSA_INT                 DataSize = DataLen;
    LSA_UINT8               DefaultIpParam[] = {DCP_DBID_TCP,DCP_DSID_TCPIP,0,14,
                                                0,0,0,0,0,0,255,255,255,255,0,0,0,0
                                               };
    LSA_UINT32              DcpGetStatus = 0;
    LSA_BOOL                BuildNameOfStationTlv = LSA_TRUE;   /* default */
    LSA_INT                 DbaseCnt;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_IdentifyRspSet(NICId: 0x%X, DataPtr: 0x%X, *OptCntPtr: 0x%X)",
                          NICId, DataPtr, *OptCntPtr);

    DbaseCnt = OHA_DB_READ_SUPPORTED_DATABASE(NICId, LSA_NULL);

    BytePtr = &(DataPtr->Byte[DataLen]);

    /* write default dataset to the response,
     * if not already existing because of request-filters */

    Type = OHA_WORD(DCP_DSID_IDOPTS, DCP_DBID_IDENT);
	/* AP01501878 (Identify.rsp): only optional and manufaturer specifif options -> no block if none */
    if ((!oha_dcp_FindType(DataPtr, DataLen, Type)) && (DbaseCnt != 0))
    {
        /* send OptionList of the station */
        Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
        if ((Len % 2) != 0)
        {
            /* fillbyte has odd length */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
    }

    Type = OHA_WORD(DCP_DSID_IDTYPE, DCP_DBID_IDENT);
    if (!oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        /*  120304lrg002: send TypeOfStation, if existing */
        Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
        if ((Len % 2) != 0)
        {
            /* fillbyte in case of odd length */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
    }

    /* if AliasNameValue requested (already in response buffer) */
    /* and no NameOfStation existing -> do not build the Tlv    */
    Type = OHA_WORD(DCP_DSID_IDALIAS, DCP_DBID_IDENT);
    if (oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        OHA_COMMON_MEM_U8_PTR_TYPE  NamePtr;

        if (0 == OHA_DB_READ_STATION_NAME(NICId, &NamePtr))
        {
            BuildNameOfStationTlv = LSA_FALSE;  /* no NameOfStation -> no build */
        }
    }

    if (BuildNameOfStationTlv)
    {
        Type = OHA_WORD(DCP_DSID_IDNAME, DCP_DBID_IDENT);
        if (!oha_dcp_FindType(DataPtr, DataLen, Type))
        {
            /* write namen of station, if existing */
            Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
            if ((Len % 2) != 0)
            {
                /* fillbyte in case of odd length */
                BytePtr[Len++] = 0;
            }
            BytePtr += Len;
            DataSize += Len;
            if (Len != 0)
                (*OptCntPtr)++;
        }
    }

    Type = OHA_WORD(DCP_DSID_IDID, DCP_DBID_IDENT);
    if (!oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        /* write DetailedType (Identificator) of the station if existing */
        Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
        if ((Len % 2) != 0)
        {
            /* fillbyte in case of odd length */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
    }

    Type = OHA_WORD(DCP_DSID_IDROLE, DCP_DBID_IDENT);
    if (!oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        /* write ProfinetIO Role of the station if existing */
        Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
        if ((Len % 2) != 0)
        {
            /* fillbyte in case of odd length */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
    }

    Type = OHA_WORD(DCP_DSID_IDDEVINST, DCP_DBID_IDENT);
    if (!oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        /* write DetailedType (Instance) of the station if existing */
        Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
        if ((Len % 2) != 0)
        {
            /* fillbyte in case of odd length */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
    }

    Type = OHA_WORD(DCP_DSID_IDOEMID /* SuboptionOEMDeviceID */, DCP_DBID_IDENT);
    if (!oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        /* write DetailedType (OEMDeviceID) of the station if existing */
        Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
        if ((Len % 2) != 0)
        {
            /* fillbyte in case of odd length */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
    }

    Type = OHA_WORD(DCP_DSID_TCPIP, DCP_DBID_TCP);
    if (!oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        /* write TCP/IP-Dataset as Ident-Block if existing */
        Len = oha_dcp_GetOptTcp(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
        if (Len == 0)
        {
            /* 030604lrg001: send Default-IP-Parameter,
             * if the dataset could not be read */
            Len = DCP_DS_HDR_SIZE + 2 + DCP_DS_TCPIP_SIZE;
            Status = OHA_DB_READ_TCPIP_STATUS(NICId);
            OHA_SWAPW(&Status, &DefaultIpParam[DCP_DS_HDR_SIZE]);
            OHA_MEMCPY(BytePtr, DefaultIpParam, Len);

        }
        if ((Len % 2) != 0)
        {
            /* fillbyte in case of odd length */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
    }

    /* TCP/IP-Dataset Device-Initiative */
    /* DeviceInitiativeBlockRes: DeviceInitiativeType, DCPBlocklength, BlockInfo, DeviceInitiativeValue */
    Type = OHA_WORD(DCP_DSID_DEVINIT, DCP_DBID_DEVINIT);

    if (!oha_dcp_FindType(DataPtr, DataLen, Type))
    {
        Len = oha_dcp_GetOptInitiative(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);

        /* responding the DevInit in case of OFF ist optional and should not be done. */
        if (Len == DCP_DS_HDR_SIZE + 2 + DCP_DS_DEVINIT_SIZE)
        {
            LSA_UINT16 DeviceInitiative = OHA_GET16_NTOH(BytePtr, DCP_DS_HDR_SIZE + 2);

            if ((DeviceInitiative & OHA_DCP_HELLO_MODE_MASK) != OHA_DCP_HELLO_OFF)
            {
                if ((Len % 2) != 0)
                {
                    /* fillbyte in case of odd length */
                    BytePtr[Len++] = 0;
                }
                BytePtr += Len;
                DataSize += Len;
                (*OptCntPtr)++;
            }
        }
    }

    LSA_UNUSED_ARG(BytePtr);    /* not used anymore */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_IdentifyRspSet(), DataPtr: 0x%X, *OptCntPtr:0x%X", DataPtr, *OptCntPtr);
    return(DataSize);
}

/*****************************************************************************/
/* Local function: oha_dcp_FindType()                                        */
/* look for Option/Suboption in the IDENTIFY-Response                        */
/*****************************************************************************/
LSA_BOOL oha_dcp_FindType(
    DCP_USR_RSP_DATA_PTR    DataPtr,
    LSA_INT                 DataLen,
    LSA_UINT16              Type)
{
    LSA_UINT16          DsLen;
    DCP_USR_DATASET_PTR DsPtr;
    LSA_BOOL            RetVal = LSA_FALSE;
    DCP_USR_BYTE_PTR    BytePtr = &DataPtr->Identify.IdentDs.Hdr.Opt;
    LSA_UINT16          Count = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_FindType(DataPtr: 0x%X,Type: 0x%X)",
                          DataPtr, Type);

    while (Count < DataLen)
    {
        DsPtr = (DCP_DATASET*)BytePtr;
        if ((DsPtr->Hdr.Opt == OHA_HIBYTE(Type))
            &&  (DsPtr->Hdr.Id == OHA_LOBYTE(Type)))
        {
            RetVal = LSA_TRUE;
            break;
        }

        DsLen = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) + DCP_DS_HDR_SIZE);
        if ((DsLen % 2) != 0)
        {
            /* fillbyte in case of odd length */
            DsLen++;
        }
        Count += DsLen;
        BytePtr += DsLen;
    }
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_FindType(), DataPtr: 0x%X, RetVal:0x%X", DataPtr, RetVal);

    return(RetVal);
}


/*****************************************************************************/
/* Local function: oha_dcp_SetOptControl()                                   */
/* write Set-Request of a Control-Suboption                                  */
/*****************************************************************************/
LSA_UINT8                               oha_dcp_SetOptControl(
    LSA_UINT16                              NICId,
    DCP_USR_DATASET_PTR                     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE       pInd)
{
    LSA_UINT16  DsLen;
    LSA_UINT8   Result = DCP_RESULT_OK;
	LSA_UINT16  Status = OHA_SWAP16(DsPtr->Status);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_SetOptControl(NICId: 0x%X, DsPtr: 0x%X, DsPtr: 0x%X)",
                          NICId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    switch (DsPtr->Hdr.Id)
    {
        case DCP_DSID_START:    /* AP01534406:DCP Control Command Start-Transaction is obsolete */
            if (DsLen != 0)
            {
                Result = DCP_RESULT_NO_DATASET;
            }
            break;

        case DCP_DSID_END:      /* AP01534406:DCP Control Command Stop-Transaction is obsolete */
            if (DsLen != 0)
            {
                Result = DCP_RESULT_NO_DATASET;
            }
            break;

        case DCP_DSID_SIGNAL:       /* handle Signal Request */
            if (DsLen != DCP_DS_SIGNAL_SIZE)
                Result = DCP_RESULT_NO_DATASET;
            else if (DsPtr->Data.Signal.ModifierHigh != DCP_FLASH_ONCE)
                Result = DCP_RESULT_DS_NOT_SET;
            else
            {
                OHA_DCP_INDICATION_TYPE  Response;  /* indication for response */

                /* start SHOW_LOCATION - FLASH_ONCE   */
                /* NEW: an end of SHOW_LOCATION is not indicated! */
                if (OHA_OK != OHA_EDDControlSignal(NICId, DCP_FLASH_ONCE))
                {
                    Result = DCP_RESULT_DS_NOT_SET;
                }

                Response.Event    = OHA_EVENT_CONTROL;
                Response.Options  = OHA_CTRL_ANY_OTHER_OPTION;
                Response.RemaOptions = 0;
                Response.Count    = 1;
                Response.Type     = OHA_DCP_OPTION_CTRL;
                Response.SubType  = OHA_DCP_SIGNAL;
                OHA_MEMCPY(&Response.SenderMACAddr, pInd->SenderMACAddr, OHA_MAC_ADDR_SIZE);

                OHA_DcpIndicateEventAllUsers(OHA_DCPGetHandleByNIC(NICId), &Response); /* LED: only this user */
            }
            break;

        case  DCP_DSID_RESPONSE:  /* TFS 852764: Suboption 凴esponse?only for coding of a DCP-ResPDU, see PNIO draft */
            Result = DCP_RESULT_NO_DATASET;
            break;

        case  OHA_DCP_RESET_TO_FACTORY:             /* get ResetToFactory */
            if (oha_db_read_set_access(NICId,
                                       DCP_DBID_CTRL, DsPtr->Hdr.Id) != 0) /* is it locked ? */
            {
                if (pInd->Event != OHA_MODE_NO_SET)
                {
					pInd->Options |= OHA_CTRL_RESET_TO_FACTORY; /* set flag! */
                }
                else
                {
                    Result = (pInd->Reason == OHA_DCP_REASON_OK) ? (LSA_UINT8)DCP_RESULT_DS_NOT_SET : pInd->Reason;
                }
            }
            else    /* user lock, but not a query user */
            {
                Result = OHA_DCP_REASON_NOT_APPLICABLE;
            }
            break;

        case  OHA_DCP_RESET:             /* get Reset (param and/or data) */
			/* BlockQualifier with option ControlOption and suboption SuboptionResetToFactory */
			/* Bit 0		reserved */
			/* Bit 1 - 15	behavior */
			/*       1 ... Reset application data */
			/*       2 ... Reset communication parameter (only supported by PNIO) */
			/*       3 ... Reset engineering parameter */
			/*  5 -  7 ... Reserved                    */
			/*       8 ... Reset all stored data ...   */
			/*       9 ... Reset and restore data      */
			/* 10 - 15 ... Reserved                    */
            if (oha_db_read_set_access(NICId,
                                       DCP_DBID_CTRL, DsPtr->Hdr.Id) != 0) /* is it locked ? */
            {
                if (pInd->Event != OHA_MODE_NO_SET)
                {
					Status >>= 1;      /* Bit 0 reserved */
					if(Status == DCP_STATUS_RESET_COMMUNICATION)   /* only supported by PNIO */
					{
						pInd->Options |= OHA_CTRL_RESET_COMMUNICATION; /* set flag! */
					}
					else
					{
						Result = OHA_DCP_REASON_NO_DATASET; /* not supported by PNIO */
					}
                }
                else
                {
                    Result = (pInd->Reason == OHA_DCP_REASON_OK) ? (LSA_UINT8)DCP_RESULT_DS_NOT_SET : pInd->Reason;
                }
            }
            else    /* user lock, but not a query user */
            {
                Result = OHA_DCP_REASON_NOT_APPLICABLE;
            }
            break;

        default:
            /* unknown suboption */
            Result = DCP_RESULT_NO_DATASET;
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_SetOptControl(), DsPtr:0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}

/*****************************************************************************/
/* Local function: oha_dcp_SetOptPbus()                                      */
/* local storeage of the datasets of the Profibus-Database in the SET-Req.   */
/*****************************************************************************/
LSA_UINT8               oha_dcp_SetOptPbus(
    LSA_UINT16              NICId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16      DsLen;
    LSA_UINT8       Result = DCP_RESULT_DS_NOT_SET;
    OHA_COMMON_MEM_U8_PTR_TYPE ParamPtr;
    LSA_BOOL        Permanent;
    LSA_UINT16      Status;
	LSA_UINT8       AccessMode = 0;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_SetOptPbus(NICId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NICId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

	Status = OHA_SWAP16(DsPtr->Status);   /* remanence?   */
    if ((Status & DCP_MASK_DS_REMANENT) == 0)
        Permanent = LSA_FALSE;
    else
        Permanent = LSA_TRUE;

	/* ManufacturerSpecificParameterValue: ManufacturerOUI (0x08 0x00 0x06), ManufacturerSpecificString */
    switch (DsPtr->Hdr.Id)
    {
            /* get the values of the dataset from the request and write it to the system */
        case DCP_DSID_PBCOMMON:         /* suboption common profibus parameters */
            (LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NICId, DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON, &AccessMode);
			if(!AccessMode)
			{
                Result = DCP_RESULT_NO_DATASET;
                break;
			}
            if (DsLen == DCP_DS_PBCOMMON_SIZE + 3/*OUI*/)
            {
				if(DsPtr->Data.PbManufact.OUI[0] == 0x08 &&
					DsPtr->Data.PbManufact.OUI[1] == 0x00 &&
					DsPtr->Data.PbManufact.OUI[2] == 0x06)
				{
					ParamPtr = (OHA_COMMON_MEM_U8_PTR_TYPE)DsPtr->Data.PbManufact.String;

					if (oha_db_read_set_access(NICId,
											   DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON)) /* is'nt it locked ? */
					{
						LSA_UINT16 RetVal = OHA_DB_WRITE_PROFIBUS_COMMON(NICId, ParamPtr, Permanent);

						if (RetVal == LSA_RET_OK)
						{
							pInd->Options |= OHA_CTRL_PROFIBUS_COMMON;
							if (Permanent != LSA_FALSE)
							{
								pInd->RemaOptions |= OHA_CTRL_PROFIBUS_COMMON;
							}
							pInd->Count++;
							Result = DCP_RESULT_OK;
						}
					}
					else    /* user lock, but not a query user */
					{
						Result = OHA_DCP_REASON_NOT_APPLICABLE;
					}
				}
            }
            break;

        case DCP_DSID_PBADDITION:          /* additional parameters */
            (LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NICId, DCP_DBID_PBPARAM, DCP_DSID_PBADDITION, &AccessMode);
			if(!AccessMode)
			{
                Result = DCP_RESULT_NO_DATASET;
                break;
			}
            if (DsLen == DCP_DS_PBADDITION_SIZE + 3/*OUI*/)
            {
				if(DsPtr->Data.PbManufact.OUI[0] == 0x08 &&
					DsPtr->Data.PbManufact.OUI[1] == 0x00 &&
					DsPtr->Data.PbManufact.OUI[2] == 0x06)
				{
					ParamPtr = (OHA_COMMON_MEM_U8_PTR_TYPE)DsPtr->Data.PbManufact.String;

					if (oha_db_read_set_access(NICId,
											   DCP_DBID_PBPARAM, DCP_DSID_PBADDITION)) /* is it locked ? */
					{
						LSA_UINT16 RetVal = OHA_DB_WRITE_PROFIBUS_ADDITION(NICId, ParamPtr, Permanent);

						if (RetVal == LSA_RET_OK)
						{
							pInd->Options |= OHA_CTRL_PROFIBUS_ADDITION;
							if (Permanent != LSA_FALSE)
							{
								pInd->RemaOptions |= OHA_CTRL_PROFIBUS_ADDITION;
							}
							pInd->Count++;
							Result = DCP_RESULT_OK;
						}
					}
					else    /* user lock, but not a query user */
					{
						Result = OHA_DCP_REASON_NOT_APPLICABLE;
					}
				}
            }
            break;

        default:                    /* unknown dataset */
            /* one option at minimum, with code and length... */
            if (DsLen > 1)
            {
                pInd->Type = DsPtr->Hdr.Opt;
                pInd->SubType = DsPtr->Hdr.Id;

                /* write the option to the system */
                if (oha_db_read_set_access(NICId,
                                           DCP_DBID_PBPARAM, DsPtr->Hdr.Id)) /* is it locked ? */
                {
                    LSA_UINT16 RetVal = OHA_DB_WRITE_DCP_ELEMENT(NICId, DCP_DBID_PBPARAM, DsPtr->Hdr.Id,
                                                                 (OHA_COMMON_MEM_U8_PTR_TYPE)&DsPtr->Data, DsLen, OHA_READ_WRITE, LSA_FALSE);
                    if (RetVal == LSA_RET_OK)
                    {
                        Result = DCP_RESULT_OK;
                        pInd->Options |= OHA_CTRL_ANY_OTHER_OPTION;
                        pInd->Count++;
                    }
                    else
                    {
                        Result = DCP_RESULT_DS_NOT_SET;
                    }
                }
                else    /* user lock, but not a query user */
                {
                    Result = OHA_DCP_REASON_NOT_APPLICABLE;
                }
            }
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_SetOptControl(), DsPtr: 0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}

/*****************************************************************************/
/* Local function: oha_dcp_SetOptIdent()                                     */
/* local storage of the datasets "General. Device properties" (8)            */
/* of a SET-Request                                                          */
/*****************************************************************************/
LSA_UINT8               oha_dcp_SetOptIdent(
    LSA_UINT16              NICId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  Status;
    LSA_UINT16  DsLen;
    LSA_INT     DataLen = 0;
    LSA_UINT8   Result = DCP_RESULT_NO_DATASET;
    LSA_BOOL    Permanent;
    LSA_UINT8   AccessControl;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_SetOptIdent(NICId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NICId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    /* no check necessary, if output function will be changed to LSA_UINT16 */
    if (DsLen < 256)
        DataLen = DsLen;

    /* remanence?   */
    Status = OHA_SWAP16(DsPtr->Status);
    if ((Status & DCP_MASK_DS_REMANENT) == 0)
        Permanent = LSA_FALSE;
    else
        Permanent = LSA_TRUE;

    switch (DsPtr->Hdr.Id)
    {
            /* write the values of the dataset to the system  */
        case DCP_DSID_IDTYPE:       /* Stationtype = ReadOnly */
           Result =  DCP_RESULT_DS_NOT_SET;
           break;

        case DCP_DSID_IDNAME:       /* NameOfStation */
            AccessControl = oha_db_read_set_access(NICId,
                                                   DCP_DBID_IDENT, DCP_DSID_IDNAME); /* is it locked ? */
            if (AccessControl)
            {
                LSA_BOOL Found = LSA_FALSE;
                OHA_COMMON_MEM_U8_PTR_TYPE pName = LSA_NULL;

                LSA_INT len = OHA_DB_READ_STATION_NAME(NICId, &pName);

                /* check the "incoming" NameOfStation strictly */
                if (DataLen && (!oha_is_valid_station_name((LSA_UINT16)DataLen, DsPtr->Data.IdentName.String, OHA_UPPER_CASES_ALLOWED(NICId), LSA_FALSE)))
                {
                    Result =  DCP_RESULT_DS_NOT_SET;
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"oha_dcp_SetOptIdent: Invalid NameOfStation.");
                    break;  /* invalid name of station */
                }

                if (len == DataLen)
                {
                    Found = oha_namecmp(DsPtr->Data.IdentName.String, pName, len, OHA_UPPER_CASES_ALLOWED(NICId));

                    if (Found)  /* compare the remanence status too ! */
                        Found = oha_db_compare_remanence(NICId, DCP_DBID_IDENT, DCP_DSID_IDNAME, Status);
                }

                if (Found)          /* NameOfStation is the same, only set the result */
                {
                    Result = DCP_RESULT_OK;
                }
                else
                {
                    LSA_UINT16 RetVal;

                    switch (pInd->Event)
                    {
                        case OHA_MODE_SET:          /* no ether-check */
                        case OHA_MODE_EMERGENCY_SET:
                        case OHA_EVENT_REMOTE_SET:  /* no ether-check */
                            RetVal = OHA_DB_WRITE_STATION_NAME(NICId,
                                                               DsPtr->Data.IdentName.String, DataLen, AccessControl, Permanent);
                            break;
                        case OHA_MODE_EXTERNAL_SET: /* do not write to the Database */
                            RetVal = LSA_RET_OK;    /* but respond the remote DCP-Client positive ! */
                            break;
                        default:                    /* no action */
                            RetVal = LSA_RET_ERR_PARAM;
                            break;
                    }
                    if (RetVal == LSA_RET_OK)
                    {
                        Result =  DCP_RESULT_OK;
                        pInd->Options |= OHA_CTRL_STATION_NAME;
                        pInd->Count++;
                        if (Permanent != LSA_FALSE)
                        {
                            pInd->RemaOptions |= OHA_CTRL_STATION_NAME;
                        }
                    }
                    else
                    {
                        Result =  DCP_RESULT_DS_NOT_SET;
                    }
                }
            }
            else    /* user lock, but not a query user */
            {
                Result = OHA_DCP_REASON_NOT_APPLICABLE;
            }
            break;

        case DCP_DSID_IDID:     /* Stationsidentificator = ReadOnly */
            Result =  DCP_RESULT_DS_NOT_SET;
			break;

        case DCP_DSID_IDDEVINST:   /* DeviceInstance = ReadOnly */
            Result =  DCP_RESULT_DS_NOT_SET;
            break;

        case DCP_DSID_IDROLE:   /* ProfinetIO Role = ReadOnly */
            Result =  DCP_RESULT_DS_NOT_SET;
            break;

        default:                    /* unknown dataset */
            pInd->Type = DsPtr->Hdr.Opt;
            pInd->SubType = DsPtr->Hdr.Id;
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_SetOptIdent(), DsPtr: 0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}


/*****************************************************************************/
/* Local function: oha_dcp_SetOptDhcp()                                      */
/* local storage of DHCP-Options of a SET-Request                            */
/*****************************************************************************/
LSA_UINT8               oha_dcp_SetOptDhcp(
    LSA_UINT16              NICId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  DsLen;
    LSA_UINT8   Result = DCP_RESULT_NO_DATASET;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_SetOptIdent(NICId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NICId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);  /* length of Hdr.Data */
    }

    /* At minimum DHCP-Option code and length existing... */
    if (DsLen >= 2)
    {
	    LSA_UINT16  Status = OHA_SWAP16(DsPtr->Status);
		/* TFS 857716: check the DHCP-Option */
		if (DsPtr->Hdr.Id == 61)
		{
			if(oha_is_valid_dhcp_option(DsLen, (OHA_COMMON_MEM_U8_PTR_TYPE)&DsPtr->Data.Dhcp))
			{
				if (oha_db_read_set_access(NICId, DCP_DBID_DHCP, DsPtr->Hdr.Id/*DsPtr->Data.Dhcp.OptionCode*/)) /* is it locked ? */
				{
					LSA_UINT16 RetVal = OHA_DB_WRITE_DHCP_OPTION(NICId, &DsPtr->Data.Dhcp.OptionCode);

					if (RetVal == LSA_RET_OK)
					{
						Result = DCP_RESULT_OK;
						pInd->Options |= OHA_CTRL_DHCP_OPTION;
						if ((Status & DCP_MASK_DS_REMANENT) != 0)
						{
							pInd->RemaOptions |= OHA_CTRL_DHCP_OPTION;
							(LSA_VOID)oha_db_write_remanence(NICId, OHA_DCP_OPTION_DHCP, DsPtr->Hdr.Id, LSA_TRUE);
						}
						pInd->Count++;
					}
					else
					{
						Result = DCP_RESULT_DS_NOT_SET;
					}
				}
				else    /* user lock, but not a query user */
				{
					Result = OHA_DCP_REASON_NOT_APPLICABLE;
				}
			}
			else   /* option checked */
			{
				Result = DCP_RESULT_DS_NOT_SET;
			}
		}
		else   /* length checked */
		{
			Result = DCP_RESULT_DS_NOT_SET;
		}
    }

    pInd->Type = DsPtr->Hdr.Opt;
    pInd->SubType = DsPtr->Hdr.Id;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_SetOptDhcp(), DsPtr: 0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}

/*****************************************************************************/
/* Local function: oha_dcp_SetOptTcp()                                       */
/* local storage of the TCP-Databases of a SET-Request                       */
/*****************************************************************************/
LSA_UINT8               oha_dcp_SetOptTcp(
    LSA_UINT16              NICId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  Status;
    LSA_UINT16  DsLen;
    LSA_BOOL    Permanent  = LSA_TRUE;
    LSA_UINT8   Result = DCP_RESULT_NO_DATASET;
    LSA_UINT8   AccessControl;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_SetOptTcp(NICId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NICId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    switch (DsPtr->Hdr.Id)
    {
            /* put the values of the dataset to the sytem */
        case DCP_DSID_MACADR:       /* MAC-Address */

            if (DsLen == OHA_MAC_ADDR_SIZE)
            {
                AccessControl = oha_db_read_set_access(NICId,
                                                       OHA_DCP_OPTION_TCP, DCP_DSID_MACADR);  /* is it locked ? */
                if (AccessControl)
                {
#if 0   /* Set MacAddress is not supported */
                    LSA_UINT16 RetVal = OHA_DB_WRITE_MAC_ADDR(NICId,
                                                              &DsPtr->Data.MacAddr.Addr[0], OHA_MAC_ADDR_SIZE, AccessControl, Permanent);
                    if (RetVal == LSA_RET_OK)
                    {
                        Result =  DCP_RESULT_OK;
                        pInd->Options |= OHA_CTRL_MAC_ADDR;
                        pInd->Count++;
                    }
                    else
#endif
                    {
                        Result =  DCP_RESULT_DS_NOT_SET;
                    }
                }
                else    /* user lock, but not a query user */
                {
                    Result = OHA_DCP_REASON_NOT_APPLICABLE;
                }
            }
            break;

        case DCP_DSID_TCPIP:        /* IP-Parameter */

            if (DsLen == DCP_DS_TCPIP_SIZE)
            {
                LSA_BOOL IpIsValid;
                OHA_COMMON_MEM_U8_PTR_TYPE TempIpSuitePtr = LSA_NULL;

                /* Is remanence of the TCP/IP-parameters in the Status demanded? */
                Status = OHA_SWAP16(DsPtr->Status);
                if ((Status & DCP_MASK_DS_REMANENT) == 0)
                {
                    Permanent = LSA_FALSE;
                }

                /* IP-Adresse, Subnetzmaske, Default Router Adresse schreiben... */
                AccessControl = oha_db_read_set_access(NICId,
                                                       OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP); /* is it locked ? */

                /* IP-suite 0.0.0.0 / 0.0.0.0 / 0.0.0.0 is allowed (see PNIO Spec V2.2). */
                IpIsValid = oha_is_valid_ip_address(OHA_GET32_NTOH(&DsPtr->Data.TcpIp.IpAddr,0),
                                                    OHA_GET32_NTOH(&DsPtr->Data.TcpIp.SubnetMask,0),
                                                    OHA_GET32_NTOH(&DsPtr->Data.TcpIp.DefaultRouter,0),
                                                    LSA_FALSE); /* 0.0.0.0 is also allowed by a remote dcp set (AP00521755) */

                (LSA_VOID) oha_db_read_temp_ip_address(NICId,
                                                       &TempIpSuitePtr, LSA_NULL, LSA_NULL);

                /* if the ip address is to set or already set, the access control status could have been changed */
                /* if SockSetIpAddress fails, the temporary IpAddress will be deleted ! */
                if (/* AccessControl && */ OHA_IS_NOT_NULL(TempIpSuitePtr) && IpIsValid)
                {
                    LSA_UINT16      TcpIpStatus;

                    /* is the IP-address already the same ? */
                    OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr = LSA_NULL;
                    OHA_COMMON_MEM_U8_PTR_TYPE pSubnet = LSA_NULL;
                    OHA_COMMON_MEM_U8_PTR_TYPE pRouter = LSA_NULL;
                    LSA_BOOL Found = LSA_FALSE;

                    /* read the IP-suite and compare */
                    LSA_INT  len = OHA_DB_READ_IP_SUITE(NICId, &pIpAddr, &pSubnet, &pRouter);

                    /* !!! read the TCPIP_STATUS too !!! */
                    TcpIpStatus = OHA_DB_READ_TCPIP_STATUS(NICId);

                    /* do not compare IP if it's got from DHCP - even the same address must be set again */
                    if (TcpIpStatus != DCP_STATUS_TCPIP_DHCP)    /* TCP/IP Parameter not got from DHCP   */
                    {
                        /* compare IP, if it is the same -> ignore */
                        if ((len == OHA_IP_ADDR_SIZE) && OHA_IS_NOT_NULL(pIpAddr) && OHA_IS_NOT_NULL(pSubnet) && OHA_IS_NOT_NULL(pRouter))
                        {
                            Found = OHA_MEMCMP(&DsPtr->Data.TcpIp.IpAddr, pIpAddr, OHA_IP_ADDR_SIZE);
                            if (Found)
                                Found = OHA_MEMCMP(&DsPtr->Data.TcpIp.SubnetMask, pSubnet, OHA_IP_ADDR_SIZE);
                            if (Found)
                                Found = OHA_MEMCMP(&DsPtr->Data.TcpIp.DefaultRouter, pRouter, OHA_IP_ADDR_SIZE);

                            if (Found)  /* compare the remanence status too ! */
                                Found = oha_db_compare_remanence(NICId, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, Status);
                        }
                    }

                    if (Found)  /* IP is the same and is not compared, only set the result */
                    {
                        Result = DCP_RESULT_OK;
                    }
                    else
                    {
                        LSA_UINT16 RetVal;

                        switch (pInd->Event)
                        {
                            case OHA_MODE_SET:          /* no ether check ! */
                            case OHA_MODE_EMERGENCY_SET:
                            case OHA_EVENT_REMOTE_SET:  /* no ether-check   */
                                RetVal = OHA_DB_READ_ACCESS_CONTROL (NICId,
                                                                     OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, &AccessControl);
                                if (RetVal != LSA_RET_OK)
                                {
                                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                                };

                                RetVal = OHA_DB_WRITE_IP_SUITE(NICId,
                                                               DsPtr->Data.TcpIp.IpAddr, DsPtr->Data.TcpIp.SubnetMask,
                                                               DsPtr->Data.TcpIp.DefaultRouter,
                                                               OHA_IP_ADDR_SIZE, AccessControl, Permanent);
                                break;
                            case OHA_MODE_EXTERNAL_SET: /* do not write to the Database */
                                RetVal = LSA_RET_OK;    /* but respond the remote DCP-Client positive ! */
                                break;
                            default:                    /* no action        */
                                RetVal = LSA_RET_ERR_PARAM;
                                break;
                        }
                        if (RetVal == LSA_RET_OK)
                        {
                            Result = DCP_RESULT_OK;
                            pInd->Options |= OHA_CTRL_IP_SUITE;
                            pInd->Count++;
                            if (Permanent != LSA_FALSE)
                            {
                                pInd->RemaOptions |= OHA_CTRL_IP_SUITE;
                            }
                        }
                        else
                        {
                            Result =  DCP_RESULT_DS_NOT_SET;
                        }
                    }
                }
                else    /* user lock, but not a query user ? */
                {
                    Result = (AccessControl) ? (LSA_UINT8)DCP_RESULT_DS_NOT_SET : (LSA_UINT8)OHA_DCP_REASON_NOT_APPLICABLE;
                }
            }
            break;

        default:                    /* unknown dataset */
            pInd->Type = DsPtr->Hdr.Opt;
            pInd->SubType = DsPtr->Hdr.Id;
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_SetOptTcp(), DsPtr: 0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}

/*****************************************************************************/
/* Local function: oha_dcp_GetOptInitiative()                                */
/* requested dataset of the Initiative-Database is written to the Requuest   */
/*****************************************************************************/
LSA_INT                 oha_dcp_GetOptInitiative(
    LSA_UINT16              NICId,
    LSA_UINT16              Type,
    DCP_USR_DATASET_PTR     DsPtr,
    LSA_UINT32          *   DcpGetStatus )
{
    LSA_INT     DsLen  = 0;
    LSA_UINT8   DsID   = OHA_LOBYTE(Type);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_GetOptInitiative(NICId: 0x%X, DsPtr: 0x%X), DsID:0x%X",
                          NICId, DsPtr, DsID);

    (*DcpGetStatus) |= 0;

    switch (DsID)
    {
        case DCP_DSID_DEVINIT:      /* Device-Initiative */
        {
            LSA_BOOL   Remanent;
            LSA_UINT8  AccessControl;
            LSA_UINT16 Length = 0;
            LSA_UINT16 RetVal;
            OHA_COMMON_MEM_U8_PTR_TYPE  ValuePtr = LSA_NULL;

            RetVal = OHA_DB_READ_DCP_ELEMENT(NICId,
                                             DCP_DBID_DEVINIT,
                                             DCP_DSID_DEVINIT,
                                             &ValuePtr,
                                             &Length,
                                             &AccessControl,
                                             &Remanent);
            if (RetVal == LSA_RET_OK &&
                Length == DCP_DS_DEVINIT_SIZE &&
                OHA_IS_NOT_NULL(ValuePtr))
            {
                OHA_MEMCPY(&DsPtr->Data.Result, ValuePtr, DCP_DS_DEVINIT_SIZE);

                DsPtr->Hdr.Opt = DCP_DBID_DEVINIT;
                DsPtr->Hdr.Id  = DCP_DSID_DEVINIT;
                DsPtr->Status = DCP_RESERVED;
                DsPtr->Hdr.Len = OHA_SWAP16(DCP_DS_DEVINIT_SIZE+2);
                DsLen = DCP_DS_HDR_SIZE + 2 + DCP_DS_DEVINIT_SIZE;
            }
        }
        break;

        default:                    /* unknown dataset */
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_GetOptInitiative(), DsPtr: 0x%X, DsLen:0x%X", DsPtr, DsLen);
    return(DsLen);
}

/*****************************************************************************/
/* Local function: oha_dcp_HelloReqSet()                                     */
/* DCP-Hello Request-PDU is established                                      */
/*****************************************************************************/
LSA_INT                 oha_dcp_HelloReqSet(
    OHA_HDB_DCP_PTR_TYPE    pHDB,
    DCP_USR_RSP_DATA_PTR    DataPtr,
    LSA_INT                 DataLen,
    LSA_INT             *   OptCntPtr)
{
    DCP_USR_BYTE_PTR        BytePtr;
    LSA_UINT16              Type;
    LSA_UINT16              Status;
    LSA_INT                 Len;
    LSA_INT                 DataSize = DataLen;
    LSA_UINT8               DefaultIpParam[] = {DCP_DBID_TCP,DCP_DSID_TCPIP,0,14,
                                                0,0,0,0,0,0,255,255,255,255,0,0,0,0
                                               };
    LSA_UINT32              DcpGetStatus = 0;
    LSA_UINT16              NICId;

    if (OHA_IS_NULL(pHDB))
    {
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                              "IN/OUT: oha_dcp_HelloReqSet(), DataPtr: 0x%X, *OptCntPtr:0x%X", DataPtr, *OptCntPtr);
        return(DataSize);
    }

	NICId = pHDB->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_HelloReqSet(pHDB: 0x%X, DataPtr: 0x%X, *OptCntPtr: 0x%X)",
                          pHDB, DataPtr, *OptCntPtr);

    /* NameOfStationBlockRes, IPParameterBlockRes, DeviceIDBlockRes,    */
    /* DeviceOptionsBlockRes, DeviceRoleBlockRes, DeviceInitiativeBlockRes */

    BytePtr = &(DataPtr->Byte[DataLen]);

    /* send the Name Of Station if existing (NameOfStationBlockRes) */
    Type = OHA_WORD(DCP_DSID_IDNAME, DCP_DBID_IDENT);

    Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
    if ((Len % 2) != 0)
    {
        /* fillbyte if length is odd */
        BytePtr[Len++] = 0;
    }
    BytePtr += Len;
    DataSize += Len;
    if (Len != 0)
        (*OptCntPtr)++;


    /* send TCP/IP-Dataset as Ident-Block if existing (IPParameterBlockRes) */
    Type = OHA_WORD(DCP_DSID_TCPIP, DCP_DBID_TCP);

    Len = oha_dcp_GetOptTcp(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
    if (Len == 0)
    {
        /* 030604lrg001: send Default-IP-Parameters,
         * if the dataset could not be read */
        Len = DCP_DS_HDR_SIZE + 2 + DCP_DS_TCPIP_SIZE;
        Status = OHA_DB_READ_TCPIP_STATUS(pHDB->Params.pDetail->Params.Dcp.NICId);
        OHA_SWAPW(&Status, &DefaultIpParam[DCP_DS_HDR_SIZE]);
        OHA_MEMCPY(BytePtr, DefaultIpParam, Len);
    }
    if ((Len % 2) != 0)
    {
        /* fillbyte if length is odd */
        BytePtr[Len++] = 0;
    }
    BytePtr += Len;
    DataSize += Len;
    if (Len != 0)
        (*OptCntPtr)++;


    /* send DetailedType (Identificator) of the Station if existing (DeviceIDBlockRes) */
    Type = OHA_WORD(DCP_DSID_IDID, DCP_DBID_IDENT);

    Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
    if ((Len % 2) != 0)
    {
        /* fillbyte if length is odd */
        BytePtr[Len++] = 0;
    }
    BytePtr += Len;
    DataSize += Len;
    if (Len != 0)
        (*OptCntPtr)++;


    /* AP01027615: send (DeviceOptionsBlockRes), not TypeOfStation */
	/* DeviceOptionsBlockRes: DeviceOptionsType, DCPBlocklength, BlockInfo, DeviceOptionsValue */
    Type = OHA_WORD(DCP_DSID_IDOPTS, DCP_DBID_IDENT);

    Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
	if (Len != 0)   /* 946117: DeviceOptionsBlockRes is optional */
	{
        if ((Len % 2) != 0)
        {
            /* fillbyte if length is odd */
            BytePtr[Len++] = 0;
        }
        BytePtr += Len;
        DataSize += Len;
        if (Len != 0)
            (*OptCntPtr)++;
	}

    /* send ProfinetIO Role of the station if existing */
    Type = OHA_WORD(DCP_DSID_IDROLE, DCP_DBID_IDENT);

    Len = oha_dcp_GetOptIdent(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
    if ((Len % 2) != 0)
    {
        /* fillbyte if length is odd */
        BytePtr[Len++] = 0;
    }
    BytePtr += Len;
    DataSize += Len;
    if (Len != 0)
        (*OptCntPtr)++;


    /* TCP/IP-Dataset Device-Initiative (DeviceInitiativeBlockRes) */
    /* DeviceInitiativeBlockRes: DeviceInitiativeType, DCPBlocklength, BlockInfo, DeviceInitiativeValue */
    Type = OHA_WORD(DCP_DSID_DEVINIT, DCP_DBID_DEVINIT);

    Len = oha_dcp_GetOptInitiative(NICId, Type, (DCP_USR_DATASET_PTR)BytePtr, &DcpGetStatus);
    if (Len == 0)
    {
        LSA_UINT16 DeviceInitiativeValue;
        LSA_UINT8 DefaultInitiative[] = {DCP_DBID_DEVINIT,DCP_DSID_DEVINIT,0,4,0,0,0,0};

        /* send Default-Device-Initiative
         * if the dataset could not be read */
        Len = DCP_DS_HDR_SIZE + 2 + DCP_DS_DEVINIT_SIZE;
        DeviceInitiativeValue = OHA_DCP_HELLO_OFF;
        OHA_SWAPW(&DeviceInitiativeValue, &DefaultInitiative[DCP_DS_HDR_SIZE + 2]);
        OHA_MEMCPY(BytePtr, DefaultInitiative, Len);
    }
    if ((Len % 2) != 0)
    {
        /* fillbyte if length is odd */
        BytePtr[Len++] = 0;
    }
    BytePtr += Len;
    DataSize += Len;
    if (Len != 0)
        (*OptCntPtr)++;

    LSA_UNUSED_ARG(BytePtr);    /* avoid warning */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_HelloReqSet(), DataPtr: 0x%X, *OptCntPtr:0x%X", DataPtr, *OptCntPtr);
    return(DataSize);
}

/*****************************************************************************/
/*  end of file OHA_DCP1.C                                                   */
/*****************************************************************************/

