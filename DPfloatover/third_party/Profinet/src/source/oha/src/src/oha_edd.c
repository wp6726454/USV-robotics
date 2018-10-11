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
/*  F i l e               &F: oha_edd.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA EDD functions                                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  5
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_EDD */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#define OHA_CFG_EDD_RQB_NO_ERR	1 /* Hallo Ernst, bitte sauber machen */


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

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SetMacPhyConfigurationStatus(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * PHY_Difference);

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DeleteMacPhyConfigurationStatus(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * PHY_Difference);

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SetMaxFrameSize(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * FrameSize_Difference);

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DeleteMaxFrameSize(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * FrameSize_Difference);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_edd_request_lower_done                  +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for EDD-requests.                        +*/
/*+               Fills Error-Structure in NDB with EDD-Response            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_EDD_LOWER_IN_FCT_ATTR oha_edd_request_lower_done(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_EDD_PTR_TYPE   pHDB;

    OHA_ENTER();

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_edd_request_lower_done(pRQB: 0x%X)",
                          pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);

    /* EDD-request always done from an EDD-Channel. !           */

    pHDB = OHAGetEddHDBFromHandle(OHA_EDD_RQB_GET_HANDLE(pRQB));

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
    }

    switch (OHA_EDD_RQB_GET_OPCODE(pRQB) )
    {
        case EDD_OPC_OPEN_CHANNEL:
            OHA_EDDOpenChannelDone(pRQB,pHDB);
            break;
        case EDD_OPC_CLOSE_CHANNEL:
            OHA_EDDCloseChannelDone(pRQB,pHDB);
            break;
        case EDD_OPC_REQUEST:
            switch ( OHA_EDD_RQB_GET_SERVICE(pRQB))
            {
                case EDD_SRV_GET_PARAMS:
                    OHA_EDDGetParamsDone(pRQB,pHDB);
                    break;
                case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
                    OHA_EDDLinkIndDone(pRQB,pHDB);
                    break;
                case EDD_SRV_GET_PORT_PARAMS:
                    OHA_EDDGetPortParamsDone(pRQB,pHDB);
                    break;
                case EDD_SRV_LED_BLINK:
                    OHA_EDDControlSignalDone(pRQB,pHDB);
                    break;
                case EDD_SRV_SET_REMOTE_PORT_STATE:
                    OHA_EDDSetRemotePortStateDone(pRQB,pHDB);
                    break;
                case EDD_SRV_NRT_SET_ARP_FILTER:
                    OHA_EDDSetArpFilterDone(pRQB,pHDB);
                    break;
                case EDD_SRV_SET_IP:
                    OHA_EDDSetIpDone(pRQB,pHDB);
                    break;
				case EDD_SRV_NRT_SET_DCP_FILTER:
					OHA_EDD_SetFilterDone(pRQB,pHDB);
					break;
                default:
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    break;
            }
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_edd_request_lower_done()");
    OHA_EXIT();
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDChannelDone                          +*/
/*+  Input                      OHA_HDB_EDD_PTR_TYPE         pHDB           +*/
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
/*+               EDD-Channels. This function is called after a lower       +*/
/*+               layer called back the OHA (e.g. after EDD finished his    +*/
/*+               open/close request)                                       +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_EDDChannelDone(
    OHA_HDB_EDD_PTR_TYPE       pHDB,
    LSA_UINT16                 Response)
{
    OHA_UPPER_RQB_PTR_TYPE       pRQB;
    LSA_UINT16                   PathStat  = LSA_RET_OK;
    LSA_BOOL                     Finish;
    LSA_BOOL                     Release;
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE             pSys;
	OHA_DETAIL_PTR_TYPE          pDetail;
	LSA_UINT16                   NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB,Response);

    Finish = LSA_FALSE; /* dont finish request by default */
    Release = LSA_FALSE; /* dont release PATH-Info by default */

    OHA_ASSERT_NULL_PTR(pHDB);

    pRQB    = pHDB->pOCReq;
    Cbf     = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe freed!*/
	pSys    = pHDB->Params.pSys;
	pDetail = pHDB->Params.pDetail;
	NICId   = pHDB->Params.pDetail->Params.Edd.NICId;

    OHA_ASSERT_NULL_PTR(pRQB);

    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_EDDChannelDone done (NICId: 0x%X,Rsp: 0x%X,State: 0x%X)",NICId,Response,pHDB->State);

    switch (pHDB->State)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Open Channel request.                         */
            /* Next Get MAC from EDD                                             */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_EDD_RUNNING:
            if ( Response == OHA_OK)
            {
                /* Get MAC-Address from EDD                    */
                pHDB->State  = OHA_HANDLE_STATE_GET_MAC_EDD_RUNNING;

                Response = OHA_EDDGetParams(pHDB, OHA_HANDLE_TYPE_SYS);
                if ( Response != OHA_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD Issuing GetParamsRequest failed.");

                    pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error.");

                    if ( OHA_EDDCloseChannel(pHDB->EDDHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        OHA_FATAL(OHA_FATAL_ERR_EDD);
                    }
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open EDD. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a GET-MAC EDD-request.                              */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_GET_MAC_EDD_RUNNING:
            if ( Response == OHA_OK)
            {
                /* Get PortParams from EDD                    */
                pHDB->State  = OHA_HANDLE_STATE_GET_LINK_STATUS_EDD_RUNNING;

                Response = OHA_EDDGetPortParams(NICId, 1, pHDB->ThisHandle);
                if (Response != OHA_OK)                     /* an error occurred */
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD Issuing GetPortParamsRequest failed.");

                    pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error.");

                    if ( OHA_EDDCloseChannel(pHDB->EDDHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        OHA_FATAL(OHA_FATAL_ERR_EDD);
                    }
                }
			}
            else
            {
                /* GetParams or Ind-Request failed. Cancel Recv-Requests */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD GetParams request failed. Response: 0x%X", Response);

                pHDB->OpenResponse = Response; /* save for later */

                pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;

                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");

                if ( OHA_EDDCloseChannel(pHDB->EDDHandle,pHDB->Params.pSys) != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                    OHA_FATAL(OHA_FATAL_ERR_EDD);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a GET-PortParams EDD-request.                       */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_GET_LINK_STATUS_EDD_RUNNING:
            if ( Response == OHA_OK)
            {
				/* AP01017785: init interface */
				OHA_COMMON_MEM_U8_PTR_TYPE IpAdrPtr         = LSA_NULL;
				OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr    = LSA_NULL;
				OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtrPtr = LSA_NULL;
				LSA_UINT16 RetVal;

				/* init. the emergency ip suite in the database */
				oha_db_delete_emergency_ip_suite(NICId);

				/* read IP-Address, Subnet-mask and Default Router Address ...  */
				(LSA_VOID) OHA_DB_READ_IP_SUITE(NICId, &IpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr);

		        (LSA_UINT16)OHA_InitAddrInfo(NICId);

				/* no ip address initialized in the database, set to (0,0,0,0) */
				if (OHA_IS_NULL(IpAdrPtr))
				{
					RetVal  = OHA_DB_WRITE_IP_SUITE(NICId,
													LSA_NULL, LSA_NULL, LSA_NULL,
													OHA_IP_ADDR_SIZE, OHA_READ_WRITE, LSA_FALSE);
					if (RetVal != LSA_RET_OK)
					{
						OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-OHA_DB_WRITE_IP_SUITE failed.[0x%X]",RetVal);
					}

					/* init. the expected ip suite in the database */
					oha_db_delete_expected_ip_suite(NICId);
				}
				else
				{
					/* init. the expected ip suite (default) in the database */
					RetVal = oha_db_store_expected_ip_suite (NICId,
															 IpAdrPtr,
															 OHA_IP_SUITE_SIZE);
					if (RetVal != LSA_RET_OK)
					{
						OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-oha_db_store_expected_ip_suite failed.[0x%X]",IpAdrPtr);
					}
				}

				if (OHA_IS_NOT_NULL(OHA_SOCKGetHandleGlobal())) /* a (global) SOCK channel is open? */
				{
					if (OHA_IS_NOT_NULL(IpAdrPtr))
					{
						/* initial set of the (local) ip address */
						Response = OHA_SOCKSetIpAddress(NICId,
														pHDB->ThisHandle,	/* EDD */
														IpAdrPtr, SubnetMaskPtr, RouterAddrPtrPtr,
														LSA_FALSE);
						if (Response == OHA_OK)
						{
							pHDB->State = OHA_HANDLE_STATE_SET_ADDRESS_EDD_RUNNING;
							break;
						}
						else
						{
							/* initial set of the (local) ip address fails -> delete the real ip address (0,0,0,0) */
							(LSA_VOID)oha_db_delete_real_ip_suite(NICId);

							OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-SetIpAddress failed.[0x%X]",Response);
							Response = OHA_ERR_LL;
						}
					}
				}
			}
            if ( Response == OHA_OK)
            {
                OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NICId);
                if (pDB)
                {   /* NicId must be already initialized by oha_init_database() */
                    /* provide Link-Indication Requests */
                    Response = OHA_EDDLinkIndProvide(pHDB,pDB->PortCnt);
                }
                else
                {
                    Response = OHA_ERR_RESOURCE;
                }
            }

            if ( Response == OHA_OK)
            {
                /* no more actions needed. Channel is open now. */
                pHDB->State  = OHA_HANDLE_STATE_OPEN;
                Finish = LSA_TRUE;
            }
            else
            {
                /* GetParams or Ind-Request failed. Cancel Recv-Requests */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD GetParams, EDD LinkIndProvide request failed. Response: 0x%X", Response);

                pHDB->OpenResponse = Response; /* save for later */

                pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;

                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");

                if ( OHA_EDDCloseChannel(pHDB->EDDHandle,pHDB->Params.pSys) != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                    OHA_FATAL(OHA_FATAL_ERR_EDD);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SET-IP SOCK-request.                              */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_SET_ADDRESS_EDD_RUNNING:
            if ( Response == OHA_OK)
            {
                OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NICId);
                if (pDB)
                {   /* NicId must be already initialized by oha_init_database() */
                    /* provide Link-Indication Requests */
                    Response = OHA_EDDLinkIndProvide(pHDB,pDB->PortCnt);
                }
                else
                {
                    Response = OHA_ERR_RESOURCE;
                }
            }
			if ( Response == OHA_OK)
			{
				LSA_UINT16 RetVal;

                /* no more actions needed. Channel is open now. */
                pHDB->State  = OHA_HANDLE_STATE_OPEN;
                Finish = LSA_TRUE;
				
				/* AP00909635 */
				(LSA_UINT16)oha_db_update_rema_ip_address(NICId);

#ifndef OHA_CFG_NO_REMA
				(LSA_VOID)oha_rema_station_update(NICId);
				(LSA_VOID)oha_rema_dhcp_update(NICId);
#endif

                RetVal = OHA_EDDSetArpFilter(NICId, OHA_HANDLE_TYPE_DCP);
                if (RetVal != OHA_OK)
                {
                    Response = OHA_ERR_FAILED;
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_EDDChannelDone - EDDSetArpFilter(RetVal: 0x%X)", RetVal);
                }
                RetVal = OHA_EDDSetIp(NICId, OHA_HANDLE_TYPE_DCP);
                if (RetVal != OHA_OK)
                {
                    Response = OHA_ERR_FAILED;
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OHA_EDDChannelDone - EDDSetIp(RetVal: 0x%X)", RetVal);
                }

                /* update the LLDP-send data if LLDP-Send is active */
                RetVal = OHA_LLDPUpdateSend(NICId,
                                            OHA_CTRL_IP_SUITE,
                                            OHA_HANDLE_TYPE_SYS);
                if (RetVal != OHA_OK)
                {
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OHA_EDDChannelDone - LLDPUpdateSend(RetVal: 0x%X)", RetVal);
                }

                OHA_AddressIndicateEventAllUser(NICId);/* address changed - indication */
				OHA_DcpIndicateCheckIpEvent(NICId); /* confirm DcpInd users */
            }
            else
            {
                /* SetIp failed. Cancel Recv-Requests */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK SetIp request failed. Response: 0x%X", Response);

                pHDB->OpenResponse = Response; /* save for later */

                pHDB->State  = OHA_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;

                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");

                if ( OHA_EDDCloseChannel(pHDB->EDDHandle,pHDB->Params.pSys) != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                    OHA_FATAL(OHA_FATAL_ERR_EDD);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Close-Channel because of error within Open    */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING:

            if ( Response == OHA_OK)
            {
                Response = pHDB->OpenResponse;  /* get Error-Response  */

                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                OHA_FATAL(OHA_FATAL_ERR_EDD);
            }

            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_EDD_RUNNING:

            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot close EDD. Response: 0x%X", Response);
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
        OHA_EDDReleaseHDB(pHDB);
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

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDGetParams                            +*/
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
/*+  Description: Get Params from EDD (e.g. MAC address)                    +*/
/*+               (Sends EDD_SRV_GET_MAC_ADDR request)                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDGetParams(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  Orginator)
{
    OHA_EDD_LOWER_RQB_PTR_TYPE                  pRQB = LSA_NULL;
    OHA_EDD_RQB_GET_PARAMS_PTR_TYPE             pGetParams;
    LSA_SYS_PTR_TYPE                            pSys;
    LSA_UINT16                                  Status = OHA_OK;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDGetParams(pHandle: 0x%X, Orginator: 0x%X)",
                          pHDB, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(*pGetParams),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            pGetParams = (OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)MemPtr;
            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_GET_PARAMS);
            OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            OHA_EDD_RQB_SET_USERID_UVAR16(pRQB,Orginator);

            pRQB->pParam       = pGetParams;

            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PARAMS (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            OHA_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            Status = OHA_ERR_RESOURCE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDGetParams(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDGetParamsDone                        +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Get Params done handling                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDGetParamsDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{

    LSA_UINT16               Response = OHA_OK;
    LSA_UINT16               RetVal = LSA_RET_OK;
    LSA_UINT16               PortCnt, CycleBaseFactor;
    LSA_UINT8                MaxMRP_Instances;
    LSA_UINT8                MRPDefaultRole, MRPRoleCapability;
    LSA_UINT32               InterfaceID, Resolution, HardwareType, TraceIdx;
	LSA_UINT16               NICId  = pHDB->Params.pDetail->Params.Edd.NICId;
    LSA_UINT16               Orginator = OHA_EDD_RQB_GET_USERID_UVAR16(pRQB);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDGetParamsDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_GET_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/

            PortCnt = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MaxPortCnt;
            InterfaceID = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->InterfaceID;
            Resolution = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->TimeResolutionInNs;
            HardwareType = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->HardwareType;
			TraceIdx = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->TraceIdx;
            MaxMRP_Instances = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MaxMRP_Instances;
            MRPDefaultRole = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MRPDefaultRoleInstance0;
			MRPRoleCapability  = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MRPSupportedRole;
			CycleBaseFactor = ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->CycleBaseFactor;

            /* are there more ports as defined ? */
            if (PortCnt > OHA_CFG_MAX_PORTS)
            {
                Response = OHA_ERR_RESOURCE;
            }
            else if (!(InterfaceID >= EDD_INTERFACE_ID_MIN && InterfaceID <= EDD_INTERFACE_ID_MAX)) /* 1 ... 16, see EDD */
            {
                Response = OHA_ERR_LL;
            }
            else
            {   /* write NicId to database */
                OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NICId);
                if (pDB)
                {
					/* NicId must be already initialized by oha_init_database() */
                    /* pDB->NicId = pHDB->Params.pDetail->Params.Edd.NICId;     */
                    pDB->PortCnt      = PortCnt;
                    pDB->InterfaceID  = InterfaceID;
                    pDB->Resolution   = Resolution;
                    pDB->HardwareType = HardwareType;
					pDB->TraceIdx     = TraceIdx;
                    pDB->MaxMRP_Instances = MaxMRP_Instances;
                    pDB->MRPDefaultRole    = MRPDefaultRole;
					pDB->MRPRoleCapability = MRPRoleCapability;
					pDB->LengthOfPeriod = 31250 * CycleBaseFactor; /* CycleBaseFactor x 31.25us */

                    /* RetVal = OHA_DB_WRITE_DCP_ELEMENT(pHDB->Params.pDetail->Params.Edd.NICId, OHA_DCP_OPTION_TCP, */
                    /*              DCP_DSID_MACADR,                                                                 */
                    /*              ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr,                 */
                    /*              OHA_MAC_ADDR_SIZE, OHA_READ_ONLY, LSA_FALSE);                                    */

                    RetVal = OHA_DB_WRITE_MAC_ADDR(NICId,
                                                   ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr,
                                                   OHA_MAC_ADDR_SIZE, OHA_READ_ONLY, LSA_FALSE);
                    if (RetVal != LSA_RET_OK)
                    {
                        Response = OHA_ERR_RESOURCE;
                    }

                    /* update the (local) management address (only InterfaceID) in the lldp database */
                    (LSA_VOID)OHA_DbStoreMgmAddress(pHDB->Params.pDetail->Params.Edd.NICId, LSA_NULL);
                }
                else     /* no Database for this Nic */
                {
                    Response = OHA_ERR_RESOURCE;
                }
            }

            OHA_PROGRAM_TRACE_06(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PARAMS MAC: %2X-%2X-%2X-%2X-%2X-%2X",
                                 ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr[0],
                                 ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr[1],
                                 ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr[2],
                                 ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr[3],
                                 ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr[4],
                                 ((OHA_EDD_RQB_GET_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr[5]);

            OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-GET PARAMS PortCnt: %d, InterfaceID: %d",PortCnt, InterfaceID);
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: GET PARAMS failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

	switch(OHA_HANDLE_GET_TYPE(Orginator))
	{
		case OHA_HANDLE_TYPE_SYS:
			OHA_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */
		break;
        case OHA_HANDLE_TYPE_USER: /* PrmCommit from the User */
		{
			/* orginator is the user handle */
			OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)Orginator);

			OHA_ASSERT_NULL_PTR(pHDBUser);

            /* call the request handler for EddGetPortParam */
            OHA_TOPORequestHandler(pHDBUser, NICId, OHA_OK); /* call the prm-handler */
		}
        break;
		default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_EDDGetParamsDone: Invalid HandleType (0x%X).", Orginator);
            OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
		break;
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDGetParamsDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDLinkIndProvide                       +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE         pHDB           +*/
/*+                             LSA_UINT16                   PortCnt        +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to EDD-HDB                              +*/
/*+  PortCnt              : Number of Ports                                 +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends NRT-Link Indication Provide Requests to EDD         +*/
/*+               (EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT) for PortCnt ports.  +*/
/*+                                                                         +*/
/*+               Allocates RQB and does EDD-Requests                       +*/
/*+                                                                         +*/
/*+               Note: we have on Ind-Resource per Port.                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDLinkIndProvide(
    OHA_HDB_EDD_PTR_TYPE     pHDB,
    LSA_UINT16               PortCnt)
{

    OHA_EDD_LOWER_RQB_PTR_TYPE                    pRQB = LSA_NULL;
    OHA_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pLink;
    LSA_SYS_PTR_TYPE                              pSys;
    LSA_UINT16                                    Status;
    LSA_USER_ID_TYPE                              UserId;
    LSA_UINT16                                    RetVal = LSA_RET_OK;
    LSA_UINT16                                    PortID;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDLinkIndProvide(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    PortID = 1;  /* first port */

    /*---------------------------------------------------------------------------*/
    /* allocate and send Request to EDD                                          */
    /*---------------------------------------------------------------------------*/

    pHDB->LinkIndPendingCnt = 0;  /* number of Link-Ind-Request pending within EDD */

    while ((Status == OHA_OK) && (PortCnt))
    {
        /* allocate RQB */
        OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
        {
            OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
            OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE),pSys);

            if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
            {
                pLink = (OHA_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE)MemPtr;

                OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
                OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT);
                OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);

                pRQB->pParam       = pLink;
                pLink->PortID      = PortID;
                pLink->Autoneg     = EDD_AUTONEG_UNKNOWN;   /* Current negotiation configuration */
                pLink->Status      = EDD_LINK_UNKNOWN;
                pLink->Speed       = EDD_LINK_UNKNOWN;
                pLink->Mode        = EDD_LINK_UNKNOWN;
                pLink->MAUType     = EDD_MAUTYPE_UNKNOWN;
                pLink->MediaType   = EDD_MEDIATYPE_UNKNOWN;
                pLink->PortStatus  = EDD_LINK_UNKNOWN;
                pLink->IRTPortStatus = EDD_LINK_UNKNOWN;
                pLink->RTClass2_PortStatus = EDD_LINK_UNKNOWN;
                pLink->PortState   = EDD_PORT_STATE_DISABLE;    /* Forwarding PortState */
                pLink->LineDelayInTicksMeasured = EDD_DELAY_UNKNOWN;
                pLink->CableDelayInNsMeasured   = EDD_DELAY_UNKNOWN;
                pLink->PortTxDelay    = EDD_DELAY_UNKNOWN;
                pLink->PortRxDelay    = EDD_DELAY_UNKNOWN;
                pLink->SyncId0_TopoOk = LSA_FALSE;
                pLink->PhyStatus      = EDD_PHY_STATUS_UNKNOWN;
                pLink->ShortPreamble  = LSA_FALSE;
                pLink->TxFragmentation = LSA_FALSE;

                OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);

                OHA_EDD_REQUEST_LOWER(pRQB,pSys);

                pHDB->LinkIndPendingCnt++;  /* number of Link-Ind-Request pending within EDD */
            }
            else
            {
                Status = OHA_ERR_RESOURCE;
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower EDD memory failed!");
                OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        else
        {
            Status = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower EDD RQB-memory failed!");
        }

        PortCnt--;
        PortID++;

    } /* while */


    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDLinkIndProvide(Status: 0x%X)",Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDLinkIndDone                          +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
/*+                             OHA_EDD_HDB_PTR_TYPE         pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: EDD LinkStatus Indication Request done (Link changed)     +*/
/*+                                                                         +*/
/*+               Copy link-status to port-managment                        +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides request to EDD                                 +*/
/*+                                                                         +*/
/*+               If Request was canceled we free the RQB and               +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDLinkIndDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE      pRQB,
    OHA_HDB_EDD_PTR_TYPE            pHDB)
{
    LSA_UINT16                                      NicID;
    LSA_UINT16                                      Status;
    LSA_BOOL                                        Reprovide;
    OHA_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE    pLink;
    OHA_DB_IF_PTR_TYPE                              pDB;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDLinkIndDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    Reprovide        = LSA_TRUE;
    NicID            = pHDB->Params.pDetail->Params.Edd.NICId;

    pDB = OHA_DB_GET_INTERFACE(NicID);

    OHA_ASSERT_NULL_PTR(pDB);
	
    OHA_LOWER_TRACE_03(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT done (Handle: 0x%X,Rsp: 0x%X,State: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB),pHDB->State);

    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the request and dont reprovide it. This is the case if we are  */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid rqb while closing, so CANCEL     */
    /*       may not be enough to catch the rqb. We have to check the State too  */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Ind-Request will be provided in the last  */
    /*       State while opening the channel.                                    */

    if ( pHDB->State == OHA_HANDLE_STATE_OPEN )
    {
        switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
        {
            case EDD_STS_OK:
                /* ---------------------------------------------*/
                /* Successfully done the request.               */
                /* ---------------------------------------------*/

                pLink = (OHA_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

                OHA_PROGRAM_TRACE_05(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD Link (Ch: %d,Port: %d): St/Sp/M: %d/%d/%d",pHDB->ThisHandle,pLink->PortID,pLink->Status,pLink->Speed,pLink->Mode);

                if (( pLink->PortID > 0 ) && (pLink->PortID <= pDB->PortCnt ))
                {
                    LSA_UINT8 LocalDelay[OHA_LLDP_PNO_DELAY_SIZE+1];
                    OHA_UPPER_MEM_U8_PTR_TYPE pLocalDelay = LocalDelay;

                    LSA_BOOL DelayDifference;
                    LSA_BOOL PortDifference;
                    LSA_BOOL PHY_Difference = LSA_FALSE;  /* the MAC/PHY Configuration/Status */
                    LSA_UINT16 LastLinkStatus;
					LSA_BOOL LinkStatusDifference = LSA_FALSE;
					LSA_BOOL PtcpStatusDifference = LSA_FALSE;

                    OHA_ASSERT (pLink->ShortPreamble == LSA_FALSE || pLink->ShortPreamble == LSA_TRUE); /* sanity */
                    OHA_ASSERT (pLink->TxFragmentation == LSA_FALSE || pLink->TxFragmentation == LSA_TRUE); /* sanity */

					/* for AP01001236: minimize the changes */
					/* Link-Status (Status, Speed, Mode, Autoneg) changed, update LLDP PortDescr */ 
					if ((OHA_GetLinkStatusChange(pHDB->LinkStatus[pLink->PortID-1].LinkStatus, pLink->Status) != EDD_LINK_UNKNOWN) ||
						(pLink->Speed   != pHDB->LinkStatus[pLink->PortID-1].Speed)      ||
						(pLink->Mode    != pHDB->LinkStatus[pLink->PortID-1].Mode)       ||
						(pLink->Autoneg != pHDB->LinkStatus[pLink->PortID-1].Autoneg))
					{
						LinkStatusDifference = LSA_TRUE;
					}

                    OHA_PROGRAM_TRACE_02(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE, "~~~EDDLinkIndDone: PortID: 0x%X, MAUType: %d", pLink->PortID, pLink->MAUType);

                    pHDB->LinkStatus[pLink->PortID-1].MAUType   = pLink->MAUType;
                    pHDB->LinkStatus[pLink->PortID-1].MediaType = pLink->MediaType;
                    pHDB->LinkStatus[pLink->PortID-1].Autoneg   = pLink->Autoneg;

                    pHDB->LinkStatus[pLink->PortID-1].PortStatus = pLink->PortStatus;
                    pHDB->LinkStatus[pLink->PortID-1].PhyStatus  = pLink->PhyStatus;

                    pHDB->LinkStatus[pLink->PortID-1].ShortPreamble   = pLink->ShortPreamble;
                    pHDB->LinkStatus[pLink->PortID-1].TxFragmentation = pLink->TxFragmentation;

                    pHDB->LinkStatus[pLink->PortID-1].RTC2_PortStatus = pLink->RTClass2_PortStatus; /* for info record */
                    pHDB->LinkStatus[pLink->PortID-1].RTC3_PortStatus = pLink->IRTPortStatus;   /* for info record */
                    pHDB->LinkStatus[pLink->PortID-1].RxDelay = pLink->PortRxDelay;                 /* for info record */
                    pHDB->LinkStatus[pLink->PortID-1].TxDelay = pLink->PortTxDelay;                 /* for info record */
                    pHDB->LinkStatus[pLink->PortID-1].PortState = (LSA_UINT8)pLink->PortState;          /* for info record */
                    pHDB->LinkStatus[pLink->PortID-1].SyncTopoOk = (LSA_UINT8)pLink->SyncId0_TopoOk;/* for info record */

                    LastLinkStatus = pHDB->LinkStatus[pLink->PortID-1].LinkStatus;

                    OHA_MEMSET(&LocalDelay, 0, sizeof(LocalDelay));

                    pHDB->LinkStatus[pLink->PortID-1].LinkStatus = pLink->Status;

                    switch (pLink->Status)
                    {
                            /* all EDD-Link-Up-States will be mapped to OHA_LINK_OK */
                        case EDD_LINK_UP:
                        case EDD_LINK_UP_CLOSED:
                        case EDD_LINK_UP_DISABLED:	/* AP01001281: UP_CLOSED and UP_DISABLED are mapped as LinkUp (for LldpSend) */
                            pHDB->LinkStatus[pLink->PortID-1].Speed      = pLink->Speed;
                            pHDB->LinkStatus[pLink->PortID-1].Mode       = pLink->Mode;
                            pHDB->LinkStatus[pLink->PortID-1].LineDelay  = pLink->LineDelayInTicksMeasured;
                            pHDB->LinkStatus[pLink->PortID-1].DelayExisting = LSA_TRUE;
                            pHDB->LinkStatus[pLink->PortID-1].CableDelay = pLink->CableDelayInNsMeasured;

                            /* delay info exists if supported (EDD_GET_PARAM-HwType)? */
                            if OHA_EDD_TIMESTAMP_SUPPORTED(pDB->HardwareType)
                            {
                                OHA_LLDP_SET_U32(pLocalDelay, pLink->PortRxDelay);
                                OHA_LLDP_SET_U32(pLocalDelay, 0);
                                OHA_LLDP_SET_U32(pLocalDelay, pLink->PortTxDelay);
                                OHA_LLDP_SET_U32(pLocalDelay, 0);
                                OHA_LLDP_SET_U32(pLocalDelay, pLink->CableDelayInNsMeasured);
                                LSA_UNUSED_ARG(pLocalDelay);    /* no more used */
                            }
                            break;

                        case EDD_LINK_DOWN:
                        {
                            /* reset the LldpSenderStatus for creating a Topo.Ind */
                            /* with RemotePortStatus=NotSupported */
                            OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicID);
                            if (!(LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL) || LSA_HOST_PTR_ARE_EQUAL(pHDBLldp->pDB, LSA_NULL)))
                            {
                                pHDBLldp->pDB->LldpData[pLink->PortID-1].LldpSenderStatus = OHA_STATUS_NOT_PRESENT;
                            }

                            pHDB->LinkStatus[pLink->PortID-1].Speed      = 0;
                            pHDB->LinkStatus[pLink->PortID-1].Mode       = 0;
                            pHDB->LinkStatus[pLink->PortID-1].LineDelay  = 0;
                            pHDB->LinkStatus[pLink->PortID-1].DelayExisting = LSA_FALSE;
                            pHDB->LinkStatus[pLink->PortID-1].CableDelay = 0;
                        }
                        break;

                        default:
                            OHA_FATAL(OHA_FATAL_ERR_EDD);
                            break;
                    }

                    /* the local SRT status (Class2) */
                    switch (pLink->RTClass2_PortStatus)
                    {
                        case EDD_RTCLASS2_NOT_SUPPORTED:
                            pHDB->LinkStatus[pLink->PortID-1].SRTPortStatus = OHA_RTCLASS2_OFF; /* RTClass2 mode not supported */
                            break;
                        case EDD_RTCLASS2_INACTIVE:     /* Not used or configured */
                            pHDB->LinkStatus[pLink->PortID-1].SRTPortStatus = OHA_RTCLASS2_OFF; /* RTClass2 mode enabled */
                            break;
                        case EDD_RTCLASS2_ACTIVE:
                            pHDB->LinkStatus[pLink->PortID-1].SRTPortStatus = OHA_RTCLASS2_UP;      /* RTClass2 mode not used */
                            break;
                        default:
                            OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDD LinkInd - invalid PortStatus (0x%X).",pLink->PortStatus);
                            OHA_FATAL(OHA_FATAL_ERR_EDD);
                            break;
                    }

                    /* write the IRT port status (Class3) */
                    {
                        LSA_UINT16 StatusClass3; /* for LLDP Tlv */

                        OHA_LOWER_TRACE_07(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH
                                           , "EDD Link PortID(%u): Status(%u)  C3_Status(%u) CableDelay(%u)  SyncTopo(%u) Preamble(%u) Fragmentation(%u)"
                                           , pLink->PortID, pLink->Status, pLink->IRTPortStatus
                                           , pLink->CableDelayInNsMeasured, pLink->SyncId0_TopoOk
						                   , pLink->ShortPreamble, pLink->TxFragmentation
                                          );

                        switch (pLink->IRTPortStatus)
                        {
                            case EDD_IRT_NOT_SUPPORTED:
                                pHDB->LinkStatus[pLink->PortID-1].IRTPortStatus = OHA_PORT_STATUS_CLASS3_OFF;
                                break;
                            case EDD_IRT_PORT_INACTIVE:     /* Not used or configured */
                                pHDB->LinkStatus[pLink->PortID-1].IRTPortStatus = OHA_PORT_STATUS_CLASS3_CONFIGURED;
                                break;
                            case EDD_IRT_PORT_ACTIVE_TX_UP:
                                pHDB->LinkStatus[pLink->PortID-1].IRTPortStatus = OHA_PORT_STATUS_CLASS3_UP;
                                break;
                            case EDD_IRT_PORT_ACTIVE_TX_RX:
                                pHDB->LinkStatus[pLink->PortID-1].IRTPortStatus = OHA_PORT_STATUS_CLASS3_RUNNING;
                                break;
                            default:
                                OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDD LinkInd - invalid IRTPortStatus (0x%X).",pLink->IRTPortStatus);
                                pHDB->LinkStatus[pLink->PortID-1].IRTPortStatus = OHA_PORT_STATUS_CLASS3_OFF;
                                OHA_FATAL(OHA_FATAL_ERR_EDD);
                                break;
                        }

                        switch (pLink->IRTPortStatus)
                        {
                            case EDD_IRT_NOT_SUPPORTED:
                                StatusClass3 = OHA_PORT_STATUS_CLASS3_OFF;
                                break;
                            case EDD_IRT_PORT_INACTIVE:     /* Not used or configured */
                                StatusClass3 = OHA_PORT_STATUS_CLASS3_OFF;
                                break;
                            case EDD_IRT_PORT_ACTIVE_TX_UP:
                                StatusClass3 = OHA_PORT_STATUS_CLASS3_UP;
                                break;
                            case EDD_IRT_PORT_ACTIVE_TX_RX:
                                StatusClass3 = OHA_PORT_STATUS_CLASS3_RUNNING;
                                break;
                            default:
                                OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDD LinkInd - invalid IRTPortStatus (0x%X).",pLink->IRTPortStatus);
                                StatusClass3 = OHA_PORT_STATUS_CLASS3_OFF;
                                OHA_FATAL(OHA_FATAL_ERR_EDD);
                                break;
                        }

						if(pHDB->LinkStatus[pLink->PortID-1].TxFragmentation)
						{
							StatusClass3 |= OHA_PORT_STATUS_CLASS3_FRAGMENTATION_MASK;
						}
						if(pHDB->LinkStatus[pLink->PortID-1].ShortPreamble)
						{
							StatusClass3 |= OHA_PORT_STATUS_CLASS3_PREAMBLE_MASK;
						}

                        {
                            OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
                            LSA_UINT16 Len     = 0;
                            LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;

                            LSA_UINT8  LldpPortStatus[OHA_LLDP_PNO_PORT_STATUS_SIZE];
                            OHA_UPPER_MEM_U8_PTR_TYPE pPortStatus = LldpPortStatus;
                            LSA_UINT16 StatusClass2 = pHDB->LinkStatus[pLink->PortID-1].SRTPortStatus;

                            OHA_MEMSET(&LldpPortStatus, 0, sizeof(LldpPortStatus));

                            OHA_PUT16_HTON (pPortStatus, 0, StatusClass2);
                            OHA_PUT16_HTON (pPortStatus, 2, StatusClass3);

                            /* read from LLDP-DB (PortStatus) */
                            (LSA_VOID)oha_db_read_loc_lldp_element(NicID, pLink->PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                                   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                                   &NamePtr, &Len);

                            /* is the port status different from the last indication ? */
                            if ((!LSA_HOST_PTR_ARE_EQUAL(NamePtr, LSA_NULL)) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
                            {
                                if (OHA_MEMCMP(NamePtr, LldpPortStatus, OHA_LLDP_PNO_PORT_STATUS_SIZE))
                                {
                                    PortDifference = LSA_FALSE;
                                }
                                else
                                {
                                    PortDifference = LSA_TRUE;
                                }
                            }
                            else
                            {
                                PortDifference = LSA_TRUE;
                            }

                            /* write to LLDP-DB (PortStatus) */
                            Status = OHA_DB_WRITE_LLDP_ELEMENT(NicID, pLink->PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                               OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS,/*SubType*/ OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                               LldpPortStatus,/* pVariable */ OHA_LLDP_PNO_PORT_STATUS_SIZE /* Length */);
                            if (Status != LSA_RET_OK)
                            {
                                OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_DB_WRITE_LLDP_ELEMENT(PortStatus) failed (Status: 0x%X).", Status);
                            }
                        }
                    }

                    /* delays supported ? */
                    if (OHA_EDD_TIMESTAMP_SUPPORTED(pDB->HardwareType))
                    {
                        OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
                        LSA_UINT16 Len     = 0;
                        LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_DELAY;

                        DelayDifference = LSA_TRUE;

                        /* read from LLDP-DB (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
                        (LSA_VOID)oha_db_read_loc_lldp_element(NicID, pLink->PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                               &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                               &NamePtr, &Len);

                        /* are the delays different from the last one ? */
                        if ((!LSA_HOST_PTR_ARE_EQUAL(NamePtr, LSA_NULL)) && (Len == OHA_LLDP_PNO_DELAY_SIZE))
                        {
                            if (OHA_MEMCMP(NamePtr, &LocalDelay, OHA_LLDP_PNO_DELAY_SIZE))
                            {
                                DelayDifference = LSA_FALSE;
                            }
                        }

                        /* write to LLDP-DB (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
                        Status = OHA_DB_WRITE_LLDP_ELEMENT(NicID, pLink->PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           OHA_LLDP_SUB_TYPE_PNO_DELAY,/*SubType*/ OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           LocalDelay,/* pVariable */ OHA_LLDP_PNO_DELAY_SIZE /* Length */);
                        if (Status != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_DB_WRITE_LLDP_ELEMENT(PortDelays) failed (Status: 0x%X).", Status);
                        }
                    }
                    else
                    {
                        DelayDifference    = LSA_FALSE;
                    }

                    /* delete MAC/PHY Configuration/Status in database in case of wireless */
                    if (pHDB->LinkStatus[pLink->PortID-1].IsWireless)
                    {
                        Status = OHA_DeleteMacPhyConfigurationStatus(pHDB, pLink->PortID, &PHY_Difference);
                        if (Status != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_DeleteMacPhyConfigurationStatus failed (RetVal: 0x%X).", Status);
                        }
                    }
                    else
                    {
                        /* MAC/PHY Configuration/Status */
                        Status = OHA_SetMacPhyConfigurationStatus(pHDB, pLink->PortID, &PHY_Difference);
                        if (Status != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_SetMacPhyConfigurationStatus failed (Status: 0x%X).", Status);
                        }
                    }

                    /* if IRTStatus is not supported we have to send the IRDATAUUID = 0 */
                    /* that means => update (local) LLDP-Tlv LLDP_PNIO_PTCPSTATUS and if changed, LLDP is to send */
                    if (oha_update_ptcp_status(NicID, pLink->PortID, LSA_FALSE))
                    {
                        PtcpStatusDifference = LSA_TRUE;
                    }

                    {
                        LSA_UINT16 LinkState, LinkStatusChange;
						LSA_UINT32 AdminStatus = 0;

                        LinkState = OHA_GetLinkStatus(pHDB->LinkStatus[pLink->PortID-1].LinkStatus);

                        LinkStatusChange = OHA_GetLinkStatusChange(LastLinkStatus,
                                                                   pHDB->LinkStatus[pLink->PortID-1].LinkStatus);

                        (LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicID, pLink->PortID, OHA_CTRL_ADMIN_STATUS, 
																 &AdminStatus, LSA_NULL, LSA_NULL);

                        /* link gose down ? */
                        if (LinkStatusChange == EDD_LINK_DOWN)
                        {
                            OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicID);

                            if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL)) &&
                                pHDBLldp->RequestState == OHA_LLDP_STATE_START) /* LLDP-Stop is possible ? */
                            {
                                Status = OHA_LLDPStop(pHDBLldp, pLink->PortID, OHA_LLDP_SEND_RECV, OHA_HANDLE_TYPE_EDD);
                                if (Status != LSA_RET_OK)
                                {
                                    OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_LLDPStop failed (Status: 0x%X).", Status);
                                }
                            }

                            /* Handle the Link Status Change for this port */
                            OHA_EddIndicateEventAllUsers(pHDB, pLink->PortID);

                            break;  /* no update of the lldp frame ! */
                        }

                        OHA_LOWER_TRACE_06(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH
                                           , "EDD Link PortID(%u): LinkState(%u) DelayDifference(%u) PortDifference(%u) PHY_Difference(%u) PtcpStatus_Difference(%u)"
                                           , pLink->PortID, LinkState, DelayDifference, PortDifference, PHY_Difference , PtcpStatusDifference);

                        /* update LLDP frame only if the link is up */
                        if ((DelayDifference || PortDifference || PHY_Difference || LinkStatusDifference || PtcpStatusDifference) &&
                            (LinkState == OHA_PORT_LINK_STATE_UP)) /* update LLDP data by LLDPSet() */
                        {
                            OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicID);

                            OHA_LOWER_TRACE_02(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH
                                               , "EDD Link PortID(%u): LldpReqState(%u)"
                                               , pLink->PortID, pHDBLldp ? pHDBLldp->RequestState : 0 );

                            if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL)) &&
                                (pHDBLldp->RequestState == OHA_LLDP_STATE_START ||
                                 pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START ||
                                 pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START)) /* LLDP-Set is possible ? */
                            {
                                Status = OHA_LLDPSet(pHDBLldp, pLink->PortID, OHA_HANDLE_TYPE_SYS); /* send them */
                                if (Status != LSA_RET_OK)
                                {
                                    OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_LLDPSet failed (Status: 0x%X).", Status);
                                }
                            }
                        }

                        /* link goes up ? */
                        if (LinkStatusChange == EDD_LINK_UP)
                        {
                            OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicID);

                            if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL)) &&
                                (pHDBLldp->RequestState == OHA_LLDP_STATE_START ||
                                 pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START || /* AP00976789 */
                                 pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START)) /* LLDP-Start is possible ? */
                            {
                                /* only activate send if the AdminStatus is txOnly(1) or txAndRx(3) ! */
								LSA_UINT16 SendMode;
                                SendMode = OHA_AdminStatusToSendMode(AdminStatus);
                                if (SendMode == OHA_LLDP_SEND_ON)
                                {
                                    /* update sending of the lldp frame */
                                    Status = OHA_LLDPStart(pHDBLldp, pLink->PortID, (LSA_UINT16)AdminStatus, OHA_HANDLE_TYPE_EDD);
                                    if (Status != LSA_RET_OK)
                                    {
                                        OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_LLDPSart failed (Status: 0x%X).", Status);
                                    }
                                }
                            }

#ifndef OHA_CFG_NO_NARE
                            /* Do a HELLO.req if Hello is written and at least one Port is in state LinkUp */
                            if (LinkState == OHA_PORT_LINK_STATE_UP)
                            {
                                OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NicID);

                                /* AP00494470: send a hello at every LinkUp */
                                if (OHA_IS_NOT_NULL(pHDBDcp) /* && pHDBDcp->HelloState == OHA_DCP_STATE_HELLO_IS_TO_SEND */)
                                {
                                    LSA_UINT16 RetVal = OHA_DoHello(NicID, OHA_ADDITIONAL_HELLO_DELAY_BY_LINK_UP);
                                    if (RetVal != OHA_OK)
                                    {
                                        OHA_PROGRAM_TRACE_00(pDB->TraceIdx, LSA_TRACE_LEVEL_WARN,"OHA_EDDLinkIndDone(). Do HELLO.Req failed.");
                                    }
                                }
                            }
                            else
                            {
                                OHA_FATAL(OHA_FATAL_ERR_EDD);   /* why, the change went to up */
                            }
#endif
                        }
                    }

                    /* Handle the Link Status Change for this port */
                    OHA_EddIndicateEventAllUsers(pHDB, pLink->PortID);
                }

                break;

            case EDD_STS_OK_CANCEL:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> OHA_HANDLE_STATE_OPEN */
                /*       while closing!                             */

                OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD LinkInd canceled (0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            default:
                OHA_PROGRAM_TRACE_01(pDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDD-ERROR: EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
                OHA_FATAL(OHA_FATAL_ERR_EDD);
                break;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDD LinkInd while not in open-state. discard and dont reprovide.");
        Reprovide = LSA_FALSE;
    }

    if ( Reprovide )
    {
        OHA_LOWER_TRACE_03(pDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
        OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle); /* set handle */
        OHA_EDD_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        LSA_UINT16 RetVal = LSA_RET_OK;

        pHDB->LinkIndPendingCnt--;  /* number of LinkStatus-Request pending within EDD */

        OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);

        OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDLinkIndDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDGetPortParams                        +*/
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
/*+  Description: Get Port-Params from EDD (phaysical type)                 +*/
/*+               (Sends EDD_SRV_GET_PORT_PARAMS request)                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDGetPortParams(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  Orginator)
{
    OHA_EDD_LOWER_RQB_PTR_TYPE                  pRQB = LSA_NULL;
    OHA_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE        pGetPortParams;
    LSA_SYS_PTR_TYPE                            pSys;
    LSA_UINT16                                  Status = OHA_OK;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal = LSA_RET_OK;
    OHA_HDB_EDD_PTR_TYPE                        pHDB   = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDGetPortParams(NICId: 0x%X, PortId: 0x%X, Orginator: 0x%X)",
                          NICId, PortId, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    if ((!PortId) || (PortId > OHA_CFG_MAX_PORTS))
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(*pGetPortParams),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            pGetPortParams = (OHA_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE)MemPtr;
            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_GET_PORT_PARAMS);
            OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            OHA_EDD_RQB_SET_USERID_UVAR16(pRQB,Orginator);

            pGetPortParams->PortID       = PortId;

            pRQB->pParam       = pGetPortParams;

            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PORT_PARAMS (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            OHA_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            Status = OHA_ERR_RESOURCE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDGetPortParams(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDGetPortParamsDone                    +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Get PortParams done handling                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDGetPortParamsDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response  = OHA_OK;
    LSA_UINT16               RetVal = LSA_RET_OK;
    LSA_UINT16               PortId;
    LSA_UINT16               Orginator;
    OHA_HDB_USER_PTR_TYPE    pHDBUser;
    LSA_UINT8                LinkSpeedModeConfigured;
    LSA_UINT32               LinkSpeedModeCapability;
    OHA_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE    pGetPortParams;
	LSA_UINT16               NICId = pHDB->Params.pDetail->Params.Edd.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDGetPortParamsDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_GET_PORT_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    pGetPortParams = (OHA_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE)pRQB->pParam;
    OHA_ASSERT_NULL_PTR(pGetPortParams);

    PortId = pGetPortParams->PortID;

    Orginator  = OHA_EDD_RQB_GET_USERID_UVAR16(pRQB);

    switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            LinkSpeedModeConfigured = pGetPortParams->LinkSpeedModeConfigured;

            LinkSpeedModeCapability = pGetPortParams->AutonegCapAdvertised;

            /* are there more ports as defined ? */
            if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
            {
                Response = OHA_ERR_RESOURCE;
            }
            else if (pGetPortParams->PortStatus == EDD_PORT_NOT_PRESENT)
            {
				(LSA_VOID)OHA_DeleteMaxFrameSize(pHDB, PortId, LSA_NULL);
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"OHA_EDDGetPortParamsDone: Port (0x%X) not present!", PortId);
            }
            else
            {
                LSA_BOOL    PHY_Difference = LSA_FALSE;
                LSA_UINT16  LinkState;

                /* the port MAC Addr */
                OHA_MEMCPY(pHDB->LinkStatus[PortId-1].PortMACAddr,
                           ((OHA_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE)pRQB->pParam)->MACAddr.MacAdr,
                           OHA_MAC_ADDR_SIZE);

                /* write the Configured LinkSpeedMode */
                switch (LinkSpeedModeConfigured)
                {
                    case EDD_LINK_AUTONEG:
                    case EDD_LINK_100MBIT_HALF:
                    case EDD_LINK_100MBIT_FULL:
                    case EDD_LINK_10MBIT_HALF:
                    case EDD_LINK_10MBIT_FULL:
                    case EDD_LINK_1GBIT_HALF:
                    case EDD_LINK_1GBIT_FULL:
                    case EDD_LINK_10GBIT_HALF:
                    case EDD_LINK_10GBIT_FULL:
                        pHDB->LinkStatus[PortId-1].SpeedModeConfigured = LinkSpeedModeConfigured;
                        break;
                    default:
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"EDD GetPortParams - invalid LinkSpeedMode configured (0x%X).", LinkSpeedModeConfigured);
                        OHA_FATAL(OHA_FATAL_ERR_EDD);
                        break;
                }

                pHDB->LinkStatus[PortId-1].SpeedModeCapability  = LinkSpeedModeCapability;
                /* pHDB->LinkStatus[PortId-1].PortStatus        = pGetPortParams->PortStatus; */

                /* Red Part of Reserved Interval (contains IRT_Frames only) */
                pHDB->LinkStatus[PortId-1].RxIntervalRedBegin   = pGetPortParams->RsvIntervalRed.Tx.BeginNs;
                pHDB->LinkStatus[PortId-1].RxIntervalRedEnd     = pGetPortParams->RsvIntervalRed.Tx.EndNs;
                pHDB->LinkStatus[PortId-1].RxIntervalOrangeBegin= pGetPortParams->RsvIntervalOrange.Tx.BeginNs;
                pHDB->LinkStatus[PortId-1].RxIntervalOrangeEnd  = pGetPortParams->RsvIntervalOrange.Tx.EndNs;

                OHA_ASSERT (pGetPortParams->IsWireless == LSA_FALSE ||
                            pGetPortParams->IsWireless == LSA_TRUE); /* sanity */
                pHDB->LinkStatus[PortId-1].IsWireless = pGetPortParams->IsWireless;

                OHA_LOWER_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH
                                   , "EDD Link PortID(%u): PortStatus(%u)  C3_Status(%u) CableDelay(%u)"
                                   , pGetPortParams->PortID, pGetPortParams->PortStatus, pGetPortParams->IRTPortStatus
                                   , pGetPortParams->CableDelayInNsMeasured
                                  );

                /* delete MAC/PHY Configuration/Status in database in case of wireless */
                if (pHDB->LinkStatus[PortId-1].IsWireless)
                {
                    RetVal = OHA_DeleteMacPhyConfigurationStatus(pHDB, PortId, &PHY_Difference);
                    if (RetVal != LSA_RET_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_EDDGetPortParamsDone: OHA_DeleteMacPhyConfigurationStatus failed (RetVal: 0x%X).", RetVal);
                    }
                }
                else
                {
                    /* write MAC/PHY Configuration/Status to database for reading the MIB */
                    RetVal = OHA_SetMacPhyConfigurationStatus(pHDB, PortId, &PHY_Difference);
                    if (RetVal != LSA_RET_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_EDDGetPortParamsDone: OHA_SetMacPhyConfigurationStatus failed (RetVal: 0x%X).", RetVal);
                    }
                }

                /* update LLDP frame only  if the link is up */
                LinkState = OHA_GetLinkStatus(pHDB->LinkStatus[PortId-1].LinkStatus);
                if ((PHY_Difference) && LinkState == OHA_PORT_LINK_STATE_UP) /* update LLDP data by LLDPSet() */
                {
                    OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

                    if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL)) &&
                        (pHDBLldp->RequestState == OHA_LLDP_STATE_START || pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START)) /* LLDP-Set is possible ? */
                    {
                        RetVal = OHA_LLDPSet(pHDBLldp, PortId, OHA_HANDLE_TYPE_SYS); /* send them */
                        if (RetVal != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_EDDGetPortParamsDone: OHA_LLDPSet failed (RetVal: 0x%X).", RetVal);
                        }
                    }
                }

				(LSA_VOID)OHA_SetMaxFrameSize(pHDB, PortId, LSA_NULL);
            }
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: GET PARAMS failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_USER:
            break;
        case OHA_HANDLE_TYPE_EDD:
			{
                OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NICId);
                
				OHA_ASSERT_NULL_PTR(pDB);

				if(Response == OHA_OK && PortId < pDB->PortCnt)
				{
					Response = OHA_EDDGetPortParams(NICId, (LSA_UINT16)(PortId + 1), pHDB->ThisHandle);
					if(Response == OHA_OK)
					{
						OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDGetPortParamsDone()");
						return;
					}
				}
				OHA_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */
				OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDGetPortParamsDone()");
	            return;
			}
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    /* orginator is the user handle */
    pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)Orginator);

    OHA_ASSERT_NULL_PTR(pHDBUser);

    /* further actions taken in request handler */
    OHA_TOPORequestHandler(pHDBUser,
                           NICId,
                           Response);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDGetPortParamsDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDOpenChannel                          +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle            : OHA EDD-Channel handle                          +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to EDD. The Request will be   +*/
/*+               finished by calling the EDD-Callbackfunction.             +*/
/*+               oha_edd_request_lower_done() if OHA_OK.                   +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_EDD_LOWER_RQB_PTR_TYPE                 pRQB   = LSA_NULL;
    OHA_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE    pOpen;
    LSA_UINT16                                 Result = OHA_OK;
    LSA_USER_ID_TYPE                           UserId;
    LSA_UINT16                                 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);

    UserId.uvar32 = 0;
    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;

        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(EDD_RQB_OPEN_CHANNEL_TYPE),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            pOpen = (OHA_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE)MemPtr;
            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_OPEN_CHANNEL);
            OHA_EDD_RQB_SET_SERVICE(pRQB,0);

#ifndef OHA_CFG_EDD_RQB_NO_ERR
            pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;
#endif
            pRQB->pParam       = pOpen;
            pOpen->HandleUpper = OHAHandle;
            pOpen->SysPath     = SysPath;
            pOpen->Cbf         = oha_edd_request_lower_done;

            OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
            OHA_EDD_OPEN_CHANNEL_LOWER(pRQB,pSys);
        }
        else
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            Result = OHA_ERR_RESOURCE;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDOpenChannel. Rsp: 0x%X",
                          Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDOpenChannelDone                      +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
/*+                             OHA_HDB_EDD_PTR_TYPE         pHDB           +*/
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
/*+               - save EDD-Channel                                        +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDOpenChannelDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16               RetVal   = LSA_RET_OK;
    LSA_UINT16               Response = OHA_OK;
    LSA_UINT16               Orginator;
	LSA_UINT16               NICId    = pHDB->Params.pDetail->Params.Edd.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDOpenChannelDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"<<<: EDD_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_EDD_RQB_GET_HANDLE(pRQB);

    if (OHA_EDD_RQB_GET_RESPONSE(pRQB) == EDD_STS_OK)
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Open Channel successful.");
        /* save channel */
        pHDB->EDDHandle = ((OHA_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE )pRQB->pParam)->Handle;

    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Open Channel failed.[0x%X].",OHA_EDD_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    if (Response == OHA_OK) /* initialize ARP filter */
    {
        (LSA_VOID)OHA_EDDSetIp(NICId, Orginator);
        (LSA_VOID)OHA_EDDSetArpFilter(NICId, Orginator);
    }
    else    /* LL-error */
    {
        OHA_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDOpenChannelDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDCloseChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       EDDHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UNIT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  EDDHandle            : EDD handle to close                             +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to EDD. The Request will be  +*/
/*+               finished by calling the EDD-Callbackfunction.             +*/
/*+               oha_edd_request_lower_done()                              +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDCloseChannel(
    LSA_HANDLE_TYPE       EDDHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_EDD_LOWER_RQB_PTR_TYPE                  pRQB   = LSA_NULL;
    LSA_UINT16                                  Result = OHA_OK;
    LSA_USER_ID_TYPE                            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDCloseChannel(EDDHandle: 0x%X)",
                          EDDHandle);

    UserId.uvar32 = 0;
    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_CLOSE_CHANNEL);
        OHA_EDD_RQB_SET_SERVICE(pRQB,0);

        OHA_EDD_RQB_SET_HANDLE(pRQB,EDDHandle);

#ifndef OHA_CFG_EDD_RQB_NO_ERR
        pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;
#endif

        pRQB->pParam = LSA_NULL;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_OPC_CLOSE_CHANNEL (EDDHandle: 0x%X,pRQB: 0x%X)",EDDHandle,pRQB);
        OHA_EDD_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDCloseChannel(Result:  0x%X)", Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDCloseChannelDone                     +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
/*+                             OHA_HDB_EDD_PTR_TYPE         pHDB           +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDCloseChannelDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16           RetVal   = LSA_RET_OK;
    LSA_UINT16           Response = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: EDD_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    if ( OHA_EDD_RQB_GET_RESPONSE(pRQB) == EDD_STS_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Close Channel successful.");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Close Channel failed.[0x%X].",OHA_EDD_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDCloseChannelDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDReleaseHDB                           +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE pHDB                   +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to User Handle to release                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release EDD -HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDReleaseHDB(
    OHA_HDB_EDD_PTR_TYPE pHDB)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    if (OHA_IS_NOT_NULL(pHDB->SetAddrInfo.NoS.pName))
    {
		LSA_UINT16 RetVal = LSA_RET_OK;
        OHA_SOCK_FREE_LOWER_MEM(&RetVal,pHDB->SetAddrInfo.NoS.pName,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);

        pHDB->SetAddrInfo.NoS.pName = LSA_NULL;
    }

    pHDB->State = OHA_HANDLE_STATE_FREE;
    OHA_GET_GLOB_PTR()->UsedLowerCnt--;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDReleaseHDB()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDGetHandleByNIC                       +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_EDD_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a EDD HDB with NICId. If none is found    +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_EDD_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_EDDGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_EDD_PTR_TYPE  pHDB  = OHA_GET_HEDD_PTR(0);
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDGetHandleByNIC(NICId: 0x%X)",
                          NICId);

    while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
    {
        if (( pHDB->State == OHA_HANDLE_STATE_OPEN || 
			  pHDB->State == OHA_HANDLE_STATE_OPEN_EDD_RUNNING ||
			  pHDB->State == OHA_HANDLE_STATE_GET_MAC_EDD_RUNNING ||
			  pHDB->State == OHA_HANDLE_STATE_GET_LINK_STATUS_EDD_RUNNING || 
			  pHDB->State == OHA_HANDLE_STATE_SET_ADDRESS_EDD_RUNNING) &&
            ( pHDB->Params.pDetail->Params.Edd.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDB++;
            i++;
        }
    }

    if ( ! Found )
    {
        pHDB = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDGetHandleByNIC(pHDB:  0x%X)", pHDB);
    return(pHDB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDCheckNICUsed                         +*/
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
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_EDDCheckNICUsed(
    LSA_UINT16 NICId)
{
    OHA_HDB_EDD_PTR_TYPE  pHDB  = OHA_GET_HEDD_PTR(0);
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDCheckNICUsed(NICId: 0x%X)",
                          NICId);
    if (NICId != OHA_INVALID_NIC_ID)
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS ) )
        {
            if (( pHDB->State != OHA_HANDLE_STATE_FREE ) &&
                ( pHDB->Params.pDetail->Params.Edd.NICId == NICId  ))
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

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDCheckNICUsed(Found:  0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDGetFreeHDB                           +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE OHA_LOCAL..pHDBEdd     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA EDD  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free EDD -HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDGetFreeHDB(
    OHA_HDB_EDD_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBEdd)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found = LSA_FALSE;
    OHA_HDB_EDD_PTR_TYPE      pHDB  = OHA_GET_HEDD_PTR(0);
    LSA_UINT32                i     = 0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDGetFreeHDB(pHDBEdd: 0x%X)",
                          pHDBEdd);

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
        *pHDBEdd = pHDB;
        OHA_GET_GLOB_PTR()->UsedLowerCnt++;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_EDDGetFreeHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBEdd);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EddIndicateEventAllUsers                +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EddIndicateEventAllUsers(
    OHA_HDB_EDD_PTR_TYPE        pHDBEdd,
    LSA_UINT16                  PortId
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_USER_PTR_TYPE       pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_UINT16                  NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EddIndicateEventAllUsers(pHDBEdd: 0x%X, PortId:  0x%X)",
                          pHDBEdd, PortId);

    OHA_ASSERT_NULL_PTR(pHDBEdd);
	NICId = pHDBEdd->Params.pDetail->Params.Edd.NICId;

    /* control the port state machine */
    if (OHA_IS_NOT_NULL(pHDBEdd->pHDBUserPrm))
    {
        LSA_UINT16 RetVal = OHA_DoRemotePortState(pHDBEdd->pHDBUserPrm, PortId);

        if (RetVal != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN,"OHA_EddIndicateEventAllUsers(RetVal: 0x%X). OHA_DoRemotePortState() failed", RetVal);
        }
    }

    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_CHANNELS)
    {
        if ( OHA_IS_NOT_NULL(pHDBUser) && pHDBUser->State == OHA_HANDLE_STATE_OPEN) /* user channel is opened ? */
		{
			/* AP01022057: confirm only the user with the same Nic */
			if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
				/* check the TOPO and store the indication in the USER-HDB as LastTopoWriteInd */
				/* indicate MAU-Type, delay, link */
				(LSA_VOID)OHA_TopoIndicatePort(pHDBUser, PortId, LSA_FALSE, LSA_TRUE);
			}
		}
        i++;       /* indicate the next user-channel */
        pHDBUser++;
    }
    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EddIndicateEventAllUsers()");
    return;
}

/*=============================================================================
 * function name:  OHA_EDDControlSignal
 * function:       Start Show_Location, Mode FLASH_ONCE
 * parameters:     Frequency        Flash frequency in Hz
 * return value:   LSA_UINT16:      LSA_RET_OK or Error
 *
 *  Notice: The signalization will be started and after 3 sec automatically
 *          stopped. The signalization will flash with 0,5 Hz.
 *          For signalization the Link LED (or a similar element
 *          element e.g. LCD Display) shal be used.
 *===========================================================================*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDControlSignal(
    LSA_UINT16  NicId,
    LSA_UINT8  Frequency
)
{
    OHA_EDD_LOWER_RQB_PTR_TYPE              pRQB = LSA_NULL;
    OHA_HDB_EDD_PTR_TYPE                    pHDB;
    EDD_UPPER_LED_BLINK_PTR_TYPE            pLedBlinkParams;
    LSA_UINT16                              Status = OHA_OK;
    LSA_USER_ID_TYPE                        UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDControlSignal(NicId: 0x%X, Frequency: 0x%X)",
                          NicId, Frequency);

    if (Frequency!=DCP_FLASH_ONCE) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

    pHDB = OHA_EDDGetHandleByNIC(NicId);

    OHA_ASSERT_NULL_PTR(pHDB);
    UserId.uvar32 = 0;

    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pHDB->Params.pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(*pLedBlinkParams),pHDB->Params.pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            pLedBlinkParams = (EDD_UPPER_LED_BLINK_PTR_TYPE)MemPtr;
            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_LED_BLINK);
            OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);

            /* FlashOnce means to flash the Ethernet LINK LED (or an alternative signalization for the */
            /* Ethernet link status) with a duration of 3 s with a frequency of 2 Hz (500 ms on, 500 ms off).*/

			/* AP01373749: params not necessary and are deleted */
			OHA_MEMSET(MemPtr, 0, sizeof(*pLedBlinkParams));
            /* pLedBlinkParams->OnOffDurationIn500msTicks   = 1; */ /* 2 Hz */
            /* pLedBlinkParams->TotalBlinkDurationInSeconds = DCP_FLASH_ONCE_TIME; */ /* 3sec */

            pRQB->pParam    = pLedBlinkParams;

            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_LED_BLINK (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            OHA_EDD_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
        }
        else
        {
            LSA_UINT16   RetVal = LSA_RET_OK;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            Status = OHA_ERR_RESOURCE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDControlSignal(Status: 0x%X)", Status);

    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDControlSignalDone                    +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Get ControlSignal done handling                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDControlSignalDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16         RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDControlSignalDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_LED_BLINK done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-LED BLINK o.k.");
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: LED BLINK failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
            break;
    }

    OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDControlSignalDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SetMacPhyConfigurationStatus            +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE OHA_LOCAL..pHDBEdd     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA EDD  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               LSA_RET_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets MAC/PHY Configuration/Status in the DataBase         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SetMacPhyConfigurationStatus(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * PHY_Difference)
{
    OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
    LSA_UINT16 Len     = 0;
    LSA_UINT32 Length  = 0;
    LSA_UINT8  SubType = LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS;
    LSA_UINT8  MacPhyCfgStatus[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_SIZE];
    LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = MacPhyCfgStatus;
    LLDP_PARAMS_MAC_PHY_CONFIG_STATUS_TYPE TLVParams;

    LSA_UINT16 RetVal;

    if (OHA_IS_NULL(pHDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_SetMacPhyConfigurationStatus(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SetMacPhyConfigurationStatus(pHDB: 0x%X, PortId: 0x%X, PHY_Difference: 0x%X)",
                          pHDB, PortId, PHY_Difference);

    /* are there more ports as defined ? */
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_SetMacPhyConfigurationStatus(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }


    OHA_MEMSET(MacPhyCfgStatus, 0, sizeof(MacPhyCfgStatus));

    /* turn EDD params to lldp element */
    /* auto-negotiation support-status */
    if (pHDB->LinkStatus[PortId-1].SpeedModeCapability)
    {
        TLVParams.AutonegSupportStatus = LLDP_TLV_MAC_PHY_AUTONEG_SUPPORTED;
    }
    else
    {
        TLVParams.AutonegSupportStatus = LLDP_TLV_MAC_PHY_AUTONEG_NOT_SUPPORTED;
    }

    if (pHDB->LinkStatus[PortId-1].SpeedModeConfigured == EDD_LINK_AUTONEG)
    {
        TLVParams.AutonegSupportStatus |= LLDP_TLV_MAC_PHY_AUTONEG_ENABLED;

        TLVParams.OperationalMAUType    = pHDB->LinkStatus[PortId-1].MAUType;
    }
    else    /* no AutoNeg configured - fixed MauType */
    {
        TLVParams.OperationalMAUType  = pHDB->LinkStatus[PortId-1].MAUType;
    }

	TLVParams.PMDAutonegAdvertisedCap = OHA_GetAdvertisedCapFromParams(pHDB->LinkStatus[PortId-1].SpeedModeCapability);

    RetVal = lldp_TLVSetMACPHYConfigStatus(&pVar, &Length, &TLVParams);
    if (RetVal != LLDP_RSP_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_FATAL,"lldp_TLVSetMACPHYConfigStatus: Invalid Response (0x%X)", RetVal);
        OHA_FATAL(OHA_FATAL_ERR_LLDP);
    }

    *PHY_Difference = LSA_TRUE;

    /* read from LLDP-DB */
    (LSA_VOID)oha_db_read_loc_lldp_element(pHDB->Params.pDetail->Params.Edd.NICId,
                                           PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
                                           &NamePtr, &Len);

    if ((!LSA_HOST_PTR_ARE_EQUAL(NamePtr, LSA_NULL)) &&
        (Len == (LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE +
                 LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE +
                 LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_MAUT_TYPE_SIZE)) )
    {
        if (OHA_MEMCMP(NamePtr, &MacPhyCfgStatus[LLDP_TL_SIZE+LLDP_TLV_ORG_SPECIFIC_MIN_SIZE], Len))
        {
            *PHY_Difference = LSA_FALSE;
        }
    }

    /* write updated element to LLDP-DB */
    RetVal = OHA_DB_WRITE_LLDP_ELEMENT(pHDB->Params.pDetail->Params.Edd.NICId,
                                       PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                       LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS,/*SubType*/ OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
                                       &MacPhyCfgStatus[LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE],
                                       LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE +
                                       LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE +
                                       LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_MAUT_TYPE_SIZE);
    if (RetVal != LSA_RET_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SetMacPhyConfigurationStatus: OHA_DB_WRITE_LLDP_ELEMENT(MAC/PHY Configuration/Status) failed (RetVal: 0x%X).", RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SetMacPhyConfigurationStatus(RetVal: 0x%X)", RetVal);

    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DeleteMacPhyConfigurationStatus         +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE OHA_LOCAL..pHDBEdd     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA EDD  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               LSA_RET_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Deletes MAC/PHY Configuration/Status in the DataBase      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DeleteMacPhyConfigurationStatus(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * PHY_Difference)
{
    OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
    LSA_UINT16 Len     = 0;
    LSA_UINT8  SubType = LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS;
    LSA_UINT16 RetVal  = LSA_RET_OK;

    if (OHA_IS_NULL(pHDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        *PHY_Difference = LSA_FALSE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DeleteMacPhyConfigurationStatus(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DeleteMacPhyConfigurationStatus(pHDB: 0x%X, PortId: 0x%X, PHY_Difference: 0x%X)",
                          pHDB, PortId, PHY_Difference);

    /* are there more ports as defined ? */
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        *PHY_Difference = LSA_FALSE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DeleteMacPhyConfigurationStatus(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    /* read from LLDP-DB */
    (LSA_VOID)oha_db_read_loc_lldp_element(pHDB->Params.pDetail->Params.Edd.NICId,
                                           PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
                                           &NamePtr, &Len);
    if (OHA_IS_NOT_NULL(NamePtr))
    {
        *PHY_Difference = LSA_TRUE;

        /* delete the lldp object in the database */
        (LSA_VOID)oha_db_delete_loc_lldp_element(pHDB->Params.pDetail->Params.Edd.NICId,
                                                 PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                 SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3);
    }
    else
    {
        *PHY_Difference = LSA_FALSE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DeleteMacPhyConfigurationStatus(RetVal: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SetMaxFrameSize                         +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE OHA_LOCAL..pHDBEdd     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA EDD  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               LSA_RET_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets MaxFrameSize Tlv in the DataBase                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SetMaxFrameSize(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * Size_Difference)
{
    OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
    LSA_UINT16 Len     = 0;

    LSA_UINT32 Length  = 0;
    LSA_UINT8  SubType = LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE;
    LSA_UINT8  MaxFrameSize[LLDP_TL_SIZE + LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_SIZE];
    LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR pVar = MaxFrameSize;
    LLDP_PARAMS_MAX_FRAME_SIZE_TYPE TLVParams;
    LSA_UINT16 NICId;

    LSA_UINT16 RetVal;

    if (OHA_IS_NULL(pHDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_SetMaxFrameSize(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

	NICId = pHDB->Params.pDetail->Params.Edd.NICId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SetMaxFrameSize(pHDB: 0x%X, PortId: 0x%X, Size_Difference: 0x%X)",
                          pHDB, PortId, Size_Difference);

    /* are there more ports as defined ? */
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_SetMaxFrameSize(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }


    OHA_MEMSET(MaxFrameSize, 0, sizeof(MaxFrameSize));

    /* turn EDD params to lldp element */
    TLVParams.MaxFrameSize = EDD_IEEE_FRAME_LENGTH_WITH_CRC; /* EDD_IEEE_FRAME_LENGTH+4 */

    RetVal = lldp_TLVSetMaxFrameSize(&pVar, &Length, &TLVParams);
    if (RetVal != LLDP_RSP_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"lldp_TLVSetMaxFrameSize: Invalid Response (0x%X)", RetVal);
        OHA_FATAL(OHA_FATAL_ERR_LLDP);
    }

	if(OHA_IS_NOT_NULL(Size_Difference))
	{
		*Size_Difference = LSA_TRUE;
	}

    /* read from LLDP-DB */
    (LSA_VOID)oha_db_read_loc_lldp_element(NICId,
                                           PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
                                           &NamePtr, &Len);

    if ((!LSA_HOST_PTR_ARE_EQUAL(NamePtr, LSA_NULL)) &&
        (Len == LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_FRAME_SIZE_SIZE) )
    {
        if (OHA_MEMCMP(NamePtr, &MaxFrameSize[LLDP_TL_SIZE+LLDP_TLV_ORG_SPECIFIC_MIN_SIZE], Len))
        {
			if(OHA_IS_NOT_NULL(Size_Difference))
			{
	            *Size_Difference = LSA_FALSE;
			}
        }
    }

    /* write updated element to LLDP-DB */
    RetVal = OHA_DB_WRITE_LLDP_ELEMENT(NICId,
                                       PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                       LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE, /*SubType*/
									   OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
                                       &MaxFrameSize[LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE],
                                       LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_FRAME_SIZE_SIZE);
    if (RetVal != LSA_RET_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_SetMaxFrameSize: OHA_DB_WRITE_LLDP_ELEMENT(MaxFrameSize) failed (RetVal: 0x%X).", RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SetMaxFrameSize(RetVal: 0x%X)", RetVal);

    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DeleteMaxFrameSize                      +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE OHA_LOCAL..pHDBEdd     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA EDD  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               LSA_RET_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Deletes MaxFrameSize in the DataBase                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DeleteMaxFrameSize(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
    LSA_UINT16                  PortId,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * FrameSize_Difference)
{
    OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
    LSA_UINT16 Len     = 0;
    LSA_UINT8  SubType = LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE;
    LSA_UINT16 RetVal  = LSA_RET_OK;
    LSA_UINT16 NICId;

	if(OHA_IS_NOT_NULL(FrameSize_Difference))
	{
		*FrameSize_Difference = LSA_FALSE;
	}

    if (OHA_IS_NULL(pHDB))
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DeleteMaxFrameSize(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

	NICId = pHDB->Params.pDetail->Params.Edd.NICId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DeleteMaxFrameSize(pHDB: 0x%X, PortId: 0x%X, FrameSize_Difference: 0x%X)",
                          pHDB, PortId, FrameSize_Difference);

    /* are there more ports as defined ? */
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DeleteMaxFrameSize(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    /* read from LLDP-DB */
    (LSA_VOID)oha_db_read_loc_lldp_element(NICId,
                                           PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                           &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
                                           &NamePtr, &Len);
    if (OHA_IS_NOT_NULL(NamePtr))
    {
		if(OHA_IS_NOT_NULL(FrameSize_Difference))
		{
			*FrameSize_Difference = LSA_TRUE;
		}
        /* delete the lldp object in the database */
        (LSA_VOID)oha_db_delete_loc_lldp_element(NICId,
                                                 PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                 SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DeleteMaxFrameSize(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsAtLeastOnePortUp                      +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks the global link status                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_EDDIsAtLeastOnePortUp(
    LSA_UINT16              NICId)
{
    LSA_BOOL RetVal = LSA_FALSE;
    OHA_HDB_EDD_PTR_TYPE    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsAtLeastOnePortUp(NICId: 0x%X)", NICId);

    if ((! LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL)) &&
        (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL)) &&
        (! LSA_HOST_PTR_ARE_EQUAL(pHDBLldp->pDB,  LSA_NULL)))
    {
        LSA_UINT16 PortId;

        for (PortId = 0; PortId < OHA_CFG_MAX_PORTS && PortId < pHDBLldp->pDB->PortCnt; PortId++)
        {
            LSA_UINT16  LinkState = OHA_GetLinkStatus(pHDBEdd->LinkStatus[PortId].LinkStatus);

            /* if the Link is up -> return o.k. */
            if (LinkState == OHA_PORT_LINK_STATE_UP)
            {
                RetVal = LSA_TRUE;
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsAtLeastOnePortUp(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IsAtLeastOnePortUp(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*=============================================================================
 * function name:  OHA_EDDSetRemotePortState
 * function:       Set remote port state
 * parameters:     NicId
 * return value:   LSA_UINT16:      OHA_OK or Error
 *
 *  Notice:        does a EDD_SRV_SET_REMOTE_PORT_STATE if something has changed
 *===========================================================================*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDSetRemotePortState(
    LSA_UINT16  NicId,
    EDD_RQB_SET_REMOTE_PORT_STATE_TYPE RemotePortState
)
{
    OHA_EDD_LOWER_RQB_PTR_TYPE              pRQB   = LSA_NULL;
    OHA_HDB_EDD_PTR_TYPE                    pHDB;
    LSA_UINT16                              RetVal = OHA_OK;
    LSA_USER_ID_TYPE                        UserId;
    EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE pRemotePortState;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDSetRemotePortState(NicId: 0x%X, RemotePortState.PortID: 0x%X)",
                          NicId, RemotePortState.PortID);

    pHDB = OHA_EDDGetHandleByNIC(NicId);

    OHA_ASSERT_NULL_PTR(pHDB);
    UserId.uvar32 = 0;

    /* check the remote port-id  */
    if (RemotePortState.PortID == 0 || RemotePortState.PortID > OHA_CFG_MAX_PORTS)
    {
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "ParamError (PortId): RemotePortState.PortID()");
        RetVal = OHA_ERR_PARAM; /* no valid PortId */
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDSetRemotePortState(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* AP01001236 */
	if(pHDB->Set_PendingCnt[RemotePortState.PortID-1] == 0)
	{
		pHDB->Set_IsActual[RemotePortState.PortID-1] = LSA_TRUE;
	}
	else /* Set(s) already active, do not update */
	{
		pHDB->Set_IsActual[RemotePortState.PortID-1] = LSA_FALSE;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,"OHA_EDDSetRemotePortState already pending (PortId: 0x%X, PendingCnt: 0x%X).", RemotePortState.PortID, pHDB->Set_PendingCnt[RemotePortState.PortID-1]);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDSetRemotePortState(RetVal: 0x%X)", RetVal);
		return(RetVal);
	}

    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pHDB->Params.pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(*pRemotePortState),pHDB->Params.pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            pRemotePortState = (EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE)MemPtr;
            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_SET_REMOTE_PORT_STATE);
            OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);

            pRemotePortState->PortID               = RemotePortState.PortID;
            pRemotePortState->RtClass2_TopoState   = RemotePortState.RtClass2_TopoState;
            pRemotePortState->RtClass3_RemoteState = RemotePortState.RtClass3_RemoteState;
            pRemotePortState->RtClass3_TopoState   = RemotePortState.RtClass3_TopoState;
            pRemotePortState->SyncId0_TopoState    = RemotePortState.SyncId0_TopoState;
            pRemotePortState->RtClass3_PDEVTopoState = RemotePortState.RtClass3_PDEVTopoState;
            pRemotePortState->Neighbour_TopoState  = RemotePortState.Neighbour_TopoState;

            OHA_LOWER_TRACE_07(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,
                               ">>>: EDD_SRV_SET_REMOTE_PORT_STATE PortID(%u) C2_Topo(%u) C3_Topo(%u) C3_PDEVTopo(%u) C3_RemPort(%u) Sync(%u) Neigh_Topo(%u)"
                               , RemotePortState.PortID
                               , RemotePortState.RtClass2_TopoState
                               , RemotePortState.RtClass3_TopoState
                               , RemotePortState.RtClass3_PDEVTopoState
                               , RemotePortState.RtClass3_RemoteState
                               , RemotePortState.SyncId0_TopoState
                               , RemotePortState.Neighbour_TopoState
                              );

			pHDB->Set_PendingCnt[RemotePortState.PortID-1]++; /* AP01001236 */
            pRQB->pParam    = pRemotePortState;

            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_SET_REMOTE_PORT_STATE (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            OHA_EDD_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
        }
        else
        {
            LSA_UINT16   RetVal = LSA_RET_OK;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            RetVal = OHA_ERR_RESOURCE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDSetRemotePortState(RetVal: 0x%X)", RetVal);

    return RetVal;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDSetRemotePortStateDone               +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Set RemotePortState done handling                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDSetRemotePortStateDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16         RetVal = LSA_RET_OK;
    EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE pRemotePortState = (EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE)pRQB->pParam;
    LSA_UINT16         PortID = pRemotePortState->PortID;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDSetRemotePortStateDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

	OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_SET_REMOTE_PORT_STATE done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    OHA_ASSERT(!(PortID == 0 || PortID > OHA_CFG_MAX_PORTS));
	OHA_ASSERT(pHDB->Set_PendingCnt[PortID-1]);

	pHDB->Set_PendingCnt[PortID-1]--; /* AP01001236 */

    switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"EDD-SET REMOTE_PORT_STATE o.k.");
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: SET REMOTE_PORT_STATE failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
            break;
    }

	if(!pHDB->Set_IsActual[PortID-1]) /* AP01001236 */
	{
		if(pHDB->Set_PendingCnt[PortID-1] == 0) /* update now */
		{
			RetVal = OHA_EDDSetRemotePortState(pHDB->Params.pDetail->Params.Edd.NICId, pHDB->LastRemotePortState[PortID-1]);
			if (RetVal != OHA_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_FATAL,"OHA_EDDSetRemotePortStateDone(): SetRemotePortState on Port %u failed.", PortID);
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			}
		}
	}

    OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_EDDSetRemotePortStateDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_update_port_status                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LSA_TRUE  - the TLV LLDP_PNIO_PORTSTATUS is to update.    +*/
/*+               LSA_FALSE - the TLV LLDP_PNIO_PORTSTATUS is not to update.+*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_update_port_status(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortID,
    LSA_BOOL                    OnlySyncForwarding)
{
    LSA_BOOL UpdateSend = LSA_FALSE;

    OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
    LSA_UINT16 Len     = 0;
    LSA_UINT8  SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;

    OHA_HDB_EDD_PTR_TYPE  pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    LSA_UINT16 StatusClass3; /* for LLDP Tlv */
    LSA_UINT16 StatusClass2;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_update_port_status(NicID: 0x%X, PortID: 0x%X, OnlySyncForwarding: 0x%X)",
                          NICId, PortID, OnlySyncForwarding);

    /* PortID 1 ... n */
    OHA_ASSERT (PortID && (PortID <= OHA_CFG_MAX_PORTS));
    OHA_ASSERT_NULL_PTR(pHDBEdd);

    /* write the Flex port status (Class2) */
    StatusClass2 = pHDBEdd->LinkStatus[PortID-1].SRTPortStatus;

    /* write the IRT port status (Class3) */

    /* Is only SyncForwarding specified for an (IRT)Port, an lLDP send */
    /* with (RDATAUUID == 0 and) RTClass3-PortState==OFF will be done. */
    if (OnlySyncForwarding)
    {
        StatusClass3 = OHA_PORT_STATUS_CLASS3_OFF;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "oha_update_port_status (PortId: 0x%X): OnlySyncForwarding port, RClass3_PortState is OFF (=OK)!", StatusClass3);
    }
    else
    {
        switch (pHDBEdd->LinkStatus[PortID-1].IRTPortStatus)
        {
            case OHA_PORT_STATUS_CLASS3_OFF:
            case OHA_PORT_STATUS_CLASS3_CONFIGURED:     /* Not used or configured */
                StatusClass3 = OHA_PORT_STATUS_CLASS3_OFF;
                break;
            case OHA_PORT_STATUS_CLASS3_UP:
                StatusClass3 = OHA_PORT_STATUS_CLASS3_UP;
                break;
            case OHA_PORT_STATUS_CLASS3_RUNNING:
                StatusClass3 = OHA_PORT_STATUS_CLASS3_RUNNING;
                break;
            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"oha_update_port_status() - invalid IRTPortStatus (0x%X).",pHDBEdd->LinkStatus[PortID-1].IRTPortStatus);
                StatusClass3 = OHA_PORT_STATUS_CLASS3_OFF;
                OHA_FATAL(OHA_FATAL_ERR_EDD);
                break;
        }

		if(pHDBEdd->LinkStatus[PortID-1].TxFragmentation)
		{
            StatusClass3 |= OHA_PORT_STATUS_CLASS3_FRAGMENTATION_MASK;
		}
		if(pHDBEdd->LinkStatus[PortID-1].ShortPreamble)
		{
            StatusClass3 |= OHA_PORT_STATUS_CLASS3_PREAMBLE_MASK;
		}
    }

    {
        LSA_UINT8  LldpPortStatus[OHA_LLDP_PNO_PORT_STATUS_SIZE];
        OHA_UPPER_MEM_U8_PTR_TYPE pPortStatus = LldpPortStatus;

        OHA_MEMSET(&LldpPortStatus, 0, sizeof(LldpPortStatus));

        OHA_PUT16_HTON (pPortStatus, 0, StatusClass2);
        OHA_PUT16_HTON (pPortStatus, 2, StatusClass3);

        /* read from LLDP-DB (PortStatus) */
        (LSA_VOID)oha_db_read_loc_lldp_element(NICId, PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                               &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                               &NamePtr, &Len);

        /* is the (existing) port status different from the last indication ? */
        if ((!LSA_HOST_PTR_ARE_EQUAL(NamePtr, LSA_NULL)) && (Len == OHA_LLDP_PNO_PORT_STATUS_SIZE))
        {
            if (!(OHA_MEMCMP(NamePtr, LldpPortStatus, OHA_LLDP_PNO_PORT_STATUS_SIZE)))
            {
                LSA_UINT16 Status;

                /* write to LLDP-DB (PortStatus) */
                Status = OHA_DB_WRITE_LLDP_ELEMENT(NICId, PortID, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                   OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS,/*SubType*/ OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                   LldpPortStatus,/* pVariable */ OHA_LLDP_PNO_PORT_STATUS_SIZE /* Length */);
                if (Status != LSA_RET_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"oha_update_port_status(): OHA_DB_WRITE_LLDP_ELEMENT(PortStatus) failed (Status: 0x%X).", Status);
                }

                UpdateSend = LSA_TRUE;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_update_port_status(UpdateSend: 0x%X)", UpdateSend);
    return(UpdateSend);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDSetArpFilter                         +*/
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
/*+  Description: Set Arp-Filter by EDD                                     +*/
/*+               (Sends EDD_SRV_SET_ARP_FILTER request)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDSetArpFilter(
    LSA_UINT16                  NicID,
    LSA_UINT16                  Orginator)
{
    OHA_HDB_EDD_PTR_TYPE                    pHDB = OHA_EDDGetHandleByNIC(NicID);
    OHA_EDD_LOWER_RQB_PTR_TYPE              pRQB = LSA_NULL;
    OHA_EDD_RQB_SET_ARP_FILTER_PTR_TYPE     pSetArpFilter;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status = OHA_OK;
    LSA_USER_ID_TYPE                        UserId;
    LSA_UINT16                              RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDSetArpFilter(NicID: 0x%X, Orginator: 0x%X)",
                          NicID, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(*pSetArpFilter),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            OHA_COMMON_MEM_U8_PTR_TYPE IpAddrPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtr = LSA_NULL;

            /* read IP-Address, Subnet-mask and Default Router Address ...  */
            (LSA_VOID) OHA_DB_READ_IP_SUITE(NicID,
                                            &IpAddrPtr, &SubnetMaskPtr, &RouterAddrPtr);

            pSetArpFilter = (OHA_EDD_RQB_SET_ARP_FILTER_PTR_TYPE)MemPtr;

            pSetArpFilter->FilterCnt = 1; /* we have one IP address */

            if (OHA_IS_NOT_NULL(IpAddrPtr)) /* ip address existing */
            {
                OHA_MEMCPY(pSetArpFilter->IPAddr, IpAddrPtr, OHA_IP_ADDR_SIZE);
                if (OHA_MEMCMP(pSetArpFilter->IPAddr,(OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP,OHA_IP_ADDR_SIZE))
                {
                    pSetArpFilter->Mode = EDD_ARP_FILTER_OFF;
                }
                else
                {
                    pSetArpFilter->Mode = EDD_ARP_FILTER_ON;
                }
            }
            else
            {
                pSetArpFilter->Mode = EDD_ARP_FILTER_OFF;
                OHA_MEMSET(pSetArpFilter->IPAddr, 0, OHA_IP_ADDR_SIZE);
            }

            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_NRT_SET_ARP_FILTER);
            OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            OHA_EDD_RQB_SET_USERID_UVAR16(pRQB,Orginator);

            pRQB->pParam       = pSetArpFilter;

            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_SET_ARP_FILTER (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            OHA_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            Status = OHA_ERR_RESOURCE;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-rqb failed!");
        Status = OHA_ERR_RESOURCE;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDSetArpFilter(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDSetArpFilterDone                     +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Set Arp-Filter done handling                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDSetArpFilterDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response  = OHA_OK;
    LSA_UINT16               RetVal = LSA_RET_OK;
    LSA_UINT16               Orginator;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDSetArpFilterDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

	OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_SET_ARP_FILTER done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_EDD_RQB_GET_USERID_UVAR16(pRQB);

    switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: SET ARP-FILTER failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
            /* Response = OHA_ERR_LL; */
            break;
    }

    OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_EDD:
            OHA_EDDChannelDone(pHDB,Response); /* further actions taken in channel handler */
            break;
        case OHA_HANDLE_TYPE_DCP:
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDSetArpFilterDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDSetIp                                +*/
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
/*+  Description: Set Ip by EDD                                             +*/
/*+               (Sends EDD_SRV_SET_IP request)                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDSetIp(
    LSA_UINT16                  NicID,
    LSA_UINT16                  Orginator)
{
    OHA_HDB_EDD_PTR_TYPE                    pHDB = OHA_EDDGetHandleByNIC(NicID);
    OHA_EDD_LOWER_RQB_PTR_TYPE              pRQB = LSA_NULL;
    EDD_UPPER_SET_IP_PTR_TYPE               pSetIp;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status = OHA_OK;
    LSA_USER_ID_TYPE                        UserId;
    LSA_UINT16                              RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDSetIp(NicID: 0x%X, Orginator: 0x%X)",
                          NicID, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_EDD_LOWER_MEM_PTR_TYPE MemPtr = LSA_NULL;
        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(*pSetIp),pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            OHA_COMMON_MEM_U8_PTR_TYPE IpAddrPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr = LSA_NULL;
            OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtr = LSA_NULL;

            /* read IP-Address, Subnet-mask and Default Router Address ...  */
            (LSA_VOID) OHA_DB_READ_IP_SUITE(NicID,
                                            &IpAddrPtr, &SubnetMaskPtr, &RouterAddrPtr);

            pSetIp = (EDD_UPPER_SET_IP_PTR_TYPE)MemPtr;

            if (OHA_IS_NOT_NULL(IpAddrPtr))
            {
                pSetIp->LocalIP.IPAdr[0] = *IpAddrPtr;
                pSetIp->LocalIP.IPAdr[1] = *(IpAddrPtr+1);
                pSetIp->LocalIP.IPAdr[2] = *(IpAddrPtr+2);
                pSetIp->LocalIP.IPAdr[3] = *(IpAddrPtr+3);
            }
            else
            {
                pSetIp->LocalIP.IPAdr[0] = 0;
                pSetIp->LocalIP.IPAdr[1] = 0;
                pSetIp->LocalIP.IPAdr[2] = 0;
                pSetIp->LocalIP.IPAdr[3] = 0;
            }

            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_SET_IP);
            OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            OHA_EDD_RQB_SET_USERID_UVAR16(pRQB,Orginator);

            pRQB->pParam       = pSetIp;

            OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_SET_IP (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
            OHA_LOWER_TRACE_04(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_NOTE,
                               "REQ: EDD_SRV_SET_IP, IP(%d.%d.%d.%d)",
                               pSetIp->LocalIP.IPAdr[0],
                               pSetIp->LocalIP.IPAdr[1],
                               pSetIp->LocalIP.IPAdr[2],
                               pSetIp->LocalIP.IPAdr[3] );
            OHA_EDD_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-memory failed!");
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
            Status = OHA_ERR_RESOURCE;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDSetIp(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDDSetIpDone                            +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Set Ip done handling                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDSetIpDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16               RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDDSetIpDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_SET_IP done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_EDD_RQB_GET_RESPONSE(pRQB));

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT( EDD_SRV_SET_IP == EDD_RQB_GET_SERVICE( pRQB ) );

    switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Edd.NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: SET IP failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
            break;
    }

    OHA_EDD_FREE_LOWER_MEM(&RetVal,pRQB->pParam,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDDSetIpDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDD_SetFilter                           +*/
/*+  Input/Output          :    OHA_HDB_EDD_PTR_TYPE         pHDB           +*/
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
/*+  Description: Starts the sending of the local neighbour-data for all    +*/
/*+               ports                                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDD_SetFilter(
    LSA_UINT16	                NICId,
    LSA_UINT8                   Mode,
    LSA_UINT16                  Orginator)
{
    OHA_HDB_EDD_PTR_TYPE       pHDB = OHA_EDDGetHandleByNIC(NICId);
    OHA_EDD_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE  pSetDcpFilter;
    LSA_SYS_PTR_TYPE           pSys;
    LSA_UINT16                 Status = OHA_OK;
    LSA_USER_ID_TYPE           UserId;
    LSA_UINT16                 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDD_SetFilter(NICId: 0x%X, Mode: 0x%X, Orginator: 0x%X)", NICId, Mode, Orginator);

    OHA_ASSERT_NULL_PTR(pHDB);

    pSys = pHDB->Params.pSys;
    UserId.uvar32 = 0;

	OHA_ASSERT(pHDB->SetFilter_PendingCnt == 0 || pHDB->SetFilter_PendingCnt == 1);
    OHA_EDD_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(EDD_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        switch (Mode)
        {
            case OHA_SET_FILTER:
            case OHA_RESET_FILTER:
            {
                LSA_UINT16  Count = 0;

                pSetDcpFilter = (OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE)oha_dcp_alloc_FilterTable(NICId, Mode, &Count); /* need a filter-table for SetDcpFilter-request */

				if(OHA_IS_NOT_NULL(pSetDcpFilter))
				{
		            pRQB->pParam  = pSetDcpFilter; /* store the table */

					if(Count != 0)
					{
						pSetDcpFilter->Mode = (Mode == OHA_SET_FILTER) ? (LSA_UINT8)EDD_DCP_FILTER_ON : (LSA_UINT8)EDD_DCP_FILTER_OFF;
					}
					else
					{
						pSetDcpFilter->Mode = EDD_DCP_FILTER_OFF;
					}
					pSetDcpFilter->FilterTLVCnt = Count;
				}
				else
				{
					OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating lower-DcpSetMemory failed!");
					OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
					OHA_MEM_FREE_FAIL_CHECK(RetVal);
					Status = OHA_ERR_RESOURCE;
				}
            }
            break;
            default:
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			break;
        }

        if (Status == OHA_OK)
        {
            OHA_EDD_RQB_SET_OPCODE(pRQB,EDD_OPC_REQUEST);
            OHA_EDD_RQB_SET_SERVICE(pRQB,EDD_SRV_NRT_SET_DCP_FILTER);
            OHA_EDD_RQB_SET_HANDLE(pRQB,pHDB->EDDHandle);
            OHA_EDD_RQB_SET_USERID_UVAR16(pRQB,Orginator);

			if (pHDB->SetFilter_PendingCnt == 0) /* send the request */
			{
				pHDB->SetFilter_PendingCnt++;
				OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_NRT_SET_DCP_FILTER (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
				OHA_EDD_REQUEST_LOWER(pRQB,pSys);
			}
			else /* queue the request */
			{
				LSA_UINT32 BlockCount;
				OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Queue: EDD_SRV_NRT_SET_DCP_FILTER (Handle: 0x%X, EDDHandle: 0x%X, pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pRQB);
				OHA_RQB_PUT_BLOCK_TOP(pHDB->LowerDcpFilterRQBs.pBottom,
									  pHDB->LowerDcpFilterRQBs.pTop,
									  pRQB);
				{	/* see AP01001236: we count the queued requests for info */
					OHA_EDD_LOWER_RQB_PTR_TYPE pBlock = pHDB->LowerDcpFilterRQBs.pBottom;
					for (BlockCount = 0; OHA_IS_NOT_NULL(pBlock); BlockCount++)
					{
						pBlock = OHA_RQB_GET_NEXT_RQB_PTR(pBlock);
					}
				}
				OHA_LOWER_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: EDD-Queue-Details (pHDB: 0x%X, Mode: 0x%X, Orginator: 0x%X, QueuekCount: 0x%X)",pHDB,Mode,Orginator,BlockCount);
			}
        }
        else
        {
            OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-Allocating RQB-memory failed!");
        Status = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDD_SetFilter(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_EDD_SetFilterDone                       +*/
/*+  Input                      OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                             OHA_HDB_EDD_PTR_TYPE        pHDB            +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDD_SetFilterDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB)
{
    LSA_UINT16      Response;
    LSA_UINT16      RetVal  = LSA_RET_OK;
    LSA_UINT16      Orginator;
    LSA_UINT16      NICId   = pHDB->Params.pDetail->Params.Edd.NICId;
	OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_EDD_SetFilterDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    Orginator  = OHA_EDD_RQB_GET_USERID_UVAR16(pRQB);

    OHA_ASSERT(OHA_EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_NRT_SET_DCP_FILTER);
	OHA_ASSERT(pHDB->SetFilter_PendingCnt == 1);

    switch (OHA_EDD_RQB_GET_RESPONSE(pRQB))
    {
        case EDD_STS_OK:
        {
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
        }
        break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"EDD-ERROR: SET FILTER failed.(0x%X).",OHA_EDD_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
        break;
    }

	oha_dcp_free_FilterTable(NICId, (OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE)(pRQB->pParam)); /* AP01017666: free the filter-table */

    OHA_EDD_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    switch (Orginator)
    {
        case OHA_HANDLE_TYPE_SYS:
			OHA_ASSERT_NULL_PTR(pHDBDcp); /* AP01571755 */
			OHA_ASSERT_NULL_PTR(pHDBDcp->pOCReq);
			switch(OHA_DCP_RQB_GET_OPCODE(pHDBDcp->pOCReq))
			{
				case DCP_OPC_OPEN_CHANNEL:
				if ( Response != OHA_OK)
				{
					pHDBDcp->DcpState = OHA_HANDLE_STATE_CLOSE_DCP_RUNNING;
                    pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_STOP;
					if ( OHA_DCPCloseChannel(pHDBDcp->DcpHandle,pHDB->Params.pSys) != OHA_OK)
					{
						OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close DCP Channel().");
						OHA_FATAL(OHA_FATAL_ERR_DCP);
					}
					break;
				}
				else
				{
                    pHDBDcp->RequestState = OHA_DCP_STATE_FILTER_START; /* filter now is set by open channel */
					OHA_DCPChannelDone(pHDBDcp,Response);
				}
				break;
				case DCP_OPC_CLOSE_CHANNEL: /* EDDx: automatically deactivation of a DCP-Filter at LSA-Close-Channel */
				default:
					OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
				break;
			}
            break;
        case OHA_HANDLE_TYPE_DCP:     /* NameOfStation changed      */
        case OHA_HANDLE_TYPE_LLDP:    /* AliasNameOfStation changed */
            break;
        case OHA_HANDLE_TYPE_USER:
			OHA_ASSERT_NULL_PTR(pHDBDcp); /* AP01571755 */
			OHA_DCPRequestHandler(pHDBDcp, LSA_NULL, Response); /* further actions taken in request handler */
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

	pHDB->SetFilter_PendingCnt--;

    while (pHDB->SetFilter_PendingCnt == 0 && 
			OHA_IS_NOT_NULL(pHDB->LowerDcpFilterRQBs.pBottom))	/* queued rqb's */
    {
		OHA_EDD_LOWER_RQB_PTR_TYPE pNextRQB;

        OHA_RQB_REM_BLOCK_BOTTOM(pHDB->LowerDcpFilterRQBs.pBottom,
								 pHDB->LowerDcpFilterRQBs.pTop,
								 pNextRQB);

		switch (OHA_EDD_RQB_GET_USERID_UVAR16(pNextRQB))
		{
			case OHA_HANDLE_TYPE_SYS:	/* Open/CloseChannel */
			case OHA_HANDLE_TYPE_USER:	/* DCP-Control */
				pHDB->SetFilter_PendingCnt++;
				OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: DCP-NextRequest(user): EDD_SRV_NRT_SET_ARP_FILTER (Handle: 0x%X, EDDPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,pNextRQB);
				OHA_EDD_REQUEST_LOWER(pNextRQB,pHDB->Params.pSys);
			break;
			case OHA_HANDLE_TYPE_DCP:   /* NameOfStation changed      */
			case OHA_HANDLE_TYPE_LLDP:  /* AliasNameOfStation changed */
				if (OHA_IS_NOT_NULL(pHDBDcp) && pHDBDcp->RequestState == OHA_DCP_STATE_FILTER_START)	/* execute the request */
				{
					Orginator = OHA_EDD_RQB_GET_USERID_UVAR16(pNextRQB);
		            Response = OHA_EDD_SetFilter(NICId, OHA_SET_FILTER, Orginator);	/* AP01256689, AP01265574: update filter and execute the request */
					OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,">>>: DCP-NextRequest(local): EDD_SRV_NRT_SET_ARP_FILTER (Handle: 0x%X, DCPHandle: 0x%X,Response: 0x%X)",pHDB->ThisHandle,pHDB->EDDHandle,Response);
				}
				/* free the queued RQB */
				oha_dcp_free_FilterTable(NICId, (OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE)(pNextRQB->pParam));
				OHA_EDD_FREE_LOWER_RQB(&RetVal,pNextRQB,pHDB->Params.pSys);
				OHA_MEM_FREE_FAIL_CHECK(RetVal);
			break;
			default:
	            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
		}	
	}

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_EDD_SetFilterDone(Response: 0x%X)",Response);
}

/*****************************************************************************/
/*  end of file OHA_EDD.C                                                    */
/*****************************************************************************/

