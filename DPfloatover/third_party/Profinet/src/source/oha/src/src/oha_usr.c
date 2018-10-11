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
/*  F i l e               &F: oha_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA user-interface functions with RQBs           */
/*                                                                           */
/*                          - oha_open_channel()                             */
/*                            Release DCP/EDD HDB if error on open channel.  */
/*                            If wrong handle use OHA_RQB_ERROR.             */
/*                          - oha_request()                                  */
/*                          - oha_close_channel()                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  1
#define OHA_MODULE_ID      LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_USR */

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

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UserCancelReceive(
    OHA_HDB_USER_PTR_TYPE       pHDB);

/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_open_channel                            +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:        OHA_OPC_OPEN_CHANNEL         +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LSA_UINT16              Response      -                             +*/
/*+     OHA_RQB_ERROR_TYPE     Error          -                             +*/
/*+     OHA_RQB_ARGS_TYPE      Args.Channel:  OHA_RQB_OPEN_CHANNEL_TYPE     +*/
/*+                                                                         +*/
/*+     Args.Channel:                                                       +*/
/*+                                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:       channel-handle of OHA (ret)   +*/
/*+     LSA_HANDLE_TYPE         handle_upper  channel-handle of user        +*/
/*+     LSA_SYS_PATH_TYPE       sys_path      system-path of channel        +*/
/*+     LSA_VOID                LSA_FCT_PTR (OHA_UPPER_OUT_FCT_ATTR,        +*/
/*+                                          oha_request_upper_done_ptr)    +*/
/*+                                         (OHA_UPPER_RQB_PTR_TYPE pRQB)   +*/
/*+                                             callback-function           +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user. This  +*/
/*+                                         is HandleUpper from Args.Channel+*/
/*+     LSA_UINT16              Response    OHA_OK                          +*/
/*+                                         OHA_ERR_RESOURCE                +*/
/*+                                         OHA_ERR_SYS_PATH                +*/
/*+                                         OHA_ERR_PARAM                   +*/
/*+                                         ...                             +*/
/*+     OHA_RQB_ERROR_TYPE     Error       additional errorcodes from LL    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a channel                                 +*/
/*+                                                                         +*/
/*+               Within this request the output - macro OHA_GET_PATH_INFO  +*/
/*+               is used to get further channel-descriptons and parameters +*/
/*+                                                                         +*/
/*+               After a successful open a channel-handle is returned      +*/
/*+               in Args.Channel.Handle for use with further requests.     +*/
/*+                                                                         +*/
/*+  HandleUpper: This handle will be stored within OHA and return as       +*/
/*+               handle on request-confirmation (in RQB-header).           +*/
/*+                                                                         +*/
/*+  cbf:         The request is always finished by calling the spezified   +*/
/*+               callback-function. This callback-function will also be    +*/
/*+               used for all other other future requests for this channel.+*/
/*+                                                                         +*/
/*+ SysPath:      The SysPath variable will not be used inside but given to +*/
/*+               systemadaption via OHA_GET_PATH_INFO.                     +*/
/*+                                                                         +*/
/*+               This function is most likely done asynchroneous because   +*/
/*+               of calling lower layer open channel (DCP or SOCK)         +*/
/*+               depending on channel type spezified in CDP (detail-ptr)   +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro OHA_RQB_ERROR with the RQB  +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               OHA_ERR_PARAM                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_UPPER_IN_FCT_ATTR  oha_open_channel(OHA_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16                Status    = OHA_OK;
    LSA_BOOL                  Finish;
    OHA_DETAIL_PTR_TYPE       pDetail   = LSA_NULL;
    LSA_UINT16                PathStat  = LSA_RET_OK;
    /* OHA_HDB_USER_PTR_TYPE  pHDBUser; */
    /* OHA_HDB_DCP_PTR_TYPE   pHDBDcp;  */
    /* OHA_HDB_EDD_PTR_TYPE   pHDBEdd;  */
    /* OHA_HDB_LLDP_PTR_TYPE  pHDBLldp; */
    /* OHA_HDB_SOCK_PTR_TYPE  pHDBSock; */
#ifndef OHA_CFG_NO_NARE
    /* OHA_HDB_NARE_PTR_TYPE  pHDBNare; */
#endif
    LSA_BOOL                  PathOpen  = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_open_channel(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /*----------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                   */
    /*----------------------------------------------------------------------------*/

    OHA_RQB_SET_HANDLE(pRQB,pRQB->args.channel.handle_upper); /* set upper handle */

    /*----------------------------------------------------------------------------*/
    /* Checks some parameters                                                     */
    /*----------------------------------------------------------------------------*/

    if ( OHA_RQB_GET_OPCODE(pRQB) != OHA_OPC_OPEN_CHANNEL )
    {
        OHA_RQB_SET_RESPONSE(pRQB, OHA_ERR_PARAM);
        Status = OHA_ERR_PARAM;
        OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,">>>: oha_open_channel(): Invalid RQB-Opcode (0x%X)",OHA_RQB_GET_OPCODE(pRQB));
        OHA_RQB_ERROR(pRQB);
    }
    else
    {
        OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_OPEN_CHANNEL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));

        if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->args.channel.oha_request_upper_done_ptr, LSA_NULL) )
        {
            OHA_RQB_SET_RESPONSE(pRQB, OHA_ERR_PARAM);
            Status = OHA_ERR_PARAM;
            OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"oha_open_channel(): Cbf is NULL");
            OHA_RQB_ERROR(pRQB);
        }
    }

    /*----------------------------------------------------------------------------*/
    /* if all ok handle request                                                   */
    /*----------------------------------------------------------------------------*/
    if (Status == OHA_OK)
    {
        LSA_SYS_PTR_TYPE  pSys;

        Finish = LSA_TRUE;

        /*------------------------------------------------------------------------*/
        /* Call LSA Output-macro..                                                */
        /*------------------------------------------------------------------------*/

        OHA_GET_PATH_INFO(&PathStat, &pSys, &pDetail, pRQB->args.channel.sys_path);

        OHA_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling OHA_GET_PATH_INFO");

        if ( PathStat != LSA_RET_OK)
        {
            OHA_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"<--: OHA_GET_PATH_INFO failed (Rsp: 0x%X).",PathStat);
            Status = OHA_ERR_SYS_PATH;
        }
        else
        {
            OHA_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"<--: OHA_GET_PATH_INFO success (Detail-Ptr: 0x%X).",pDetail);

            if (LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL))
            {
                Status = OHA_ERR_PARAM;
            }
            else
            {
                PathOpen = LSA_TRUE;
            }
        }

        /*------------------------------------------------------------------------*/
        /* Get Infos from Detail-Ptr (CDB) and start handling..                   */
        /*------------------------------------------------------------------------*/

        if ( Status == OHA_OK )
        {
            OHA_ASSERT_NULL_PTR(pDetail);
            switch (pDetail->PathType)
            {
                    /* ---------------------------------------------------------------*/
                    /* User-Channel.                                                  */
                    /* ---------------------------------------------------------------*/
                case OHA_PATH_TYPE_USER:
                {
                    OHA_HDB_EDD_PTR_TYPE  pHDBEdd = OHA_GET_HEDD_PTR(0);   /* any channel must be opened */

                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening USER-Channel");

                    /* true: 10Mbps and half-duplex (for all speeds) are allowed */
                    OHA_ASSERT (pDetail->Params.User.allow_non_pnio_mautypes == LSA_FALSE ||
                                pDetail->Params.User.allow_non_pnio_mautypes == LSA_TRUE); /* sanity */
                    OHA_ASSERT (pDetail->Params.User.CheckTopology == LSA_FALSE ||
                                pDetail->Params.User.CheckTopology == LSA_TRUE); /* sanity */

					/* 0 ... no REMA user channel */
					/* <>0 ..for OHA_OPC_REMA_INDICATION(data_tag=OHA_STATION_DATA) confirmation: */
					/* OHA_REMA_STATION_FORMAT_STANDARD (1...Classic):  index=OHA_RECORD_INDEX_MULTIPLE_STATION will be indicated  */
					/* OHA_REMA_STATION_FORMAT_ADVANCED (2...Advanced): index=OHA_RECORD_INDEX_NAME_OF_STATION, OHA_RECORD_INDEX_IP_SUITE */
                    OHA_ASSERT (pDetail->Params.User.RemaStationFormat <= OHA_REMA_STATION_FORMAT_ADVANCED);

                    if (!(pHDBEdd && (pHDBEdd->State != OHA_HANDLE_STATE_FREE) && pHDBEdd->Params.pDetail))
                    {
                        Status = OHA_ERR_SEQUENCE;
                        /* Error on getting EDD-Channel. Abort. */
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA USER-channel failed. No EDD-Channel present");
                    }
                    /* 0 ... no cyclic check (NameOfStation or IP),  1 ... check every 5sec, 2...10sec, 6...30sec (max.) */
                    else if (pDetail->Params.User.CheckInterval > OHA_MAX_CHECK_INTERVAL)
                    {
                        Status = OHA_ERR_PARAM;
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA USER-channel failed. No EDD-Channel present");
                    }
                    else
                    {
                        OHA_HDB_USER_PTR_TYPE pHDBUser = LSA_NULL;
                        /* Get a free, initialized HDB */
                        Status = OHA_UserGetFreeHDB(&pHDBUser);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBUser);

                            pHDBUser->Params.pSys          = pSys;
                            pHDBUser->Params.pDetail       = pDetail;
                            pHDBUser->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBUser->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBUser->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle      = pHDBUser->ThisHandle;

                            /* pHDBUser->LLDP = pRQB; */ /* save pointer to request for later use */
                            pHDBUser->State                = OHA_HANDLE_STATE_OPEN;
                            pHDBUser->LldpReqPendingCnt    = 0; /* Number of pend. User-LLDP-requests */

							if(pDetail->Params.User.CheckTopology) /* AP01132314: OHA_OPC_TOPO_CONTROL removed */
							{
			                    OHA_InitTopo(pHDBUser);
			                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"channel is registered for TOPO");

								pHDBUser->TopoState    = OHA_TOPO_STATE_OPEN;    /* channel is registered */
								pHDBUser->PrmState     = OHA_PRM_STATE_PREPARE;
							}
							else
							{
								pHDBUser->TopoState     = OHA_TOPO_STATE_CLOSE;
								pHDBUser->PrmState      = OHA_PRM_STATE_CLOSE;
							}

#ifndef OHA_CFG_NO_NARE
                            /* alloc and start the cyclic timer by the first Check-User */
                            if (pDetail->Params.User.CheckInterval != 0)
                            {
                                /* Start the cyclic Timer if not already started */
                                if ( ! OHA_GET_GLOB_PTR()->CyclicTimerUserCnt )
                                {
                                    LSA_UINT16 RetVal;

                                    if (OHA_GET_GLOB_PTR()->CyclicTimerID == OHA_TIMER_ID_INVALID)
                                    {
                                        RetVal = OHA_TimerAlloc(LSA_TIMER_TYPE_CYCLIC,
                                                                LSA_TIME_BASE_1S,
                                                                OHA_TIMER_USER_ID_CYLIC,
                                                                OHA_TimerCB,
                                                                &OHA_GET_GLOB_PTR()->CyclicTimerID);

                                        if ( RetVal != OHA_OK )
                                        {
                                            Status = OHA_ERR_RESOURCE;
                                            /* Error on getting timer. Abort. */
                                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA USER-channel failed. Alloc Timer failed! (RetVal: 0x%X).",RetVal);
                                            break;
                                        }
                                    }

                                    /* Start the 1sec timer */
                                    OHA_TimerStart(OHA_GET_GLOB_PTR()->CyclicTimerID,1);
                                }

                                OHA_GET_GLOB_PTR()->CyclicTimerUserCnt++;
                            }
#endif
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA USER-channel failed. Cannot get USER handle! (Rsp: 0x%X).",Status);
                        }
                    }
                }
                break;
                /* --------------------------------------------------------------*/
                /* DCP -Channel.                                                 */
                /* --------------------------------------------------------------*/
                case OHA_PATH_TYPE_DCP:
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening DCP- SystemChannel");

                    /* Check if NIC-ID already used by a DCP-Channel. If so abort */

                    if ( OHA_DCPCheckNICUsed(pDetail->Params.Dcp.NICId))
                    {
                        Status = OHA_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA DCP-channel failed. NIC-ID already used (ID: 0x%X)",pDetail->Params.Dcp.NICId);
                    }
                    else
                    {
                        OHA_HDB_DCP_PTR_TYPE pHDBDcp = LSA_NULL;

                        /* Get a free, initialized HDB */
                        Status = OHA_DCPGetFreeHDB(&pHDBDcp);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBDcp);

                            pHDBDcp->Params.pSys          = pSys;
                            pHDBDcp->Params.pDetail       = pDetail;
                            pHDBDcp->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBDcp->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBDcp->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBDcp->ThisHandle;

                            /* Do a DCP- open channel request */
                            Status = OHA_DCPOpenChannel(pHDBDcp->ThisHandle,pHDBDcp->Params.SysPath,pHDBDcp->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBDcp->DcpState  = OHA_HANDLE_STATE_OPEN_DCP_RUNNING;
                                pHDBDcp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish          = LSA_FALSE;
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA-DCP channel failed. (Rsp: %Xh)",Status);
                                OHA_DCPReleaseHDB(pHDBDcp);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA-DCP channel failed. Cannot get DCP handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* EDD -Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case OHA_PATH_TYPE_EDD:
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening EDD- SystemChannel");

                    OHA_ASSERT (pDetail->Params.Edd.NoS_allow_upper_cases == LSA_FALSE ||
                                pDetail->Params.Edd.NoS_allow_upper_cases == LSA_TRUE); /* sanity */

                    /* Check if NIC-ID already used by a EDD-Channel. If so abort */

                    if ( OHA_EDDCheckNICUsed(pDetail->Params.Edd.NICId))
                    {
                        Status = OHA_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA EDD-channel failed. NIC-ID already used (ID: %Xh)",pDetail->Params.Edd.NICId);
                    }
                    else
                    {
                        OHA_HDB_EDD_PTR_TYPE pHDBEdd = LSA_NULL;
                        /* Get a free, initialized HDB */
                        Status = OHA_EDDGetFreeHDB(&pHDBEdd);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBEdd);

                            pHDBEdd->Params.pSys          = pSys;
                            pHDBEdd->Params.pDetail       = pDetail;
                            pHDBEdd->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBEdd->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBEdd->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBEdd->ThisHandle;

                            /* Do a EDD- open channel request */
                            Status = OHA_EDDOpenChannel(pHDBEdd->ThisHandle,pHDBEdd->Params.SysPath,pHDBEdd->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBEdd->State  = OHA_HANDLE_STATE_OPEN_EDD_RUNNING;
                                pHDBEdd->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish          = LSA_FALSE;
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA-EDD channel failed. (Rsp: %Xh)",Status);
                                OHA_EDDReleaseHDB(pHDBEdd);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA-EDD channel failed. Cannot get EDD handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* LLDP-Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case OHA_PATH_TYPE_LLDP:
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening LLDP- SystemChannel");

                    /* Check if NIC-ID already used by a LLDP-Channel. If so abort */
                    if ( OHA_LLDPCheckNICUsed(pDetail->Params.Lldp.NICId))
                    {
                        Status = OHA_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA LLDP-channel failed. NIC-ID already used (ID: 0x%X)",pDetail->Params.Lldp.NICId);
                    }
                    else
                    {
                        OHA_HDB_LLDP_PTR_TYPE pHDBLldp = LSA_NULL;
                        /* Get a free, initialized HDB */
                        Status = OHA_LLDPGetFreeHDB(&pHDBLldp);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBLldp);

                            pHDBLldp->Params.pSys          = pSys;
                            pHDBLldp->Params.pDetail       = pDetail;
                            pHDBLldp->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBLldp->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBLldp->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBLldp->ThisHandle;

                            /* Do a LLDP- open channel request */
                            Status = OHA_LLDPOpenChannel(pHDBLldp->ThisHandle,pHDBLldp->Params.SysPath,pHDBLldp->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBLldp->State  = OHA_HANDLE_STATE_OPEN_LLDP_RUNNING;
                                pHDBLldp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish           = LSA_FALSE;
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA-LLDP channel failed. (Rsp: %Xh)",Status);
                                OHA_LLDPReleaseHDB(pHDBLldp);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA-LLDP channel failed. Cannot get LLDP handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* SOCK-Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case OHA_PATH_TYPE_SOCK:
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening SOCK- SystemChannel");

                    /* Check if NIC-ID already used by a SOCK-Channel. If so abort */
                    if ( OHA_SOCKCheckNICUsed(pDetail->Params.Sock.NICId) )
                    {
                        Status = OHA_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA SOCK-channel already used");
                    }
					else if (OHA_IS_NULL(OHA_DB_GET_INTERFACE(pDetail->Params.Sock.NICId)))
					{
                        Status = OHA_ERR_RESOURCE;
                        /* Error on getting interface. abort */
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA SOCK-channel, no (global) interface for this Nic-ID (%Xh) initalized", pDetail->Params.Sock.NICId);
					}
					else
                    {
						LSA_UINT16 IfCount = OHA_GetCountOfInterfaces(); /* count the opened interface channels */
                        OHA_HDB_SOCK_PTR_TYPE pHDBSock = LSA_NULL;

						/* AP01017785: Are EDD System channel(s) opened first ? */
						/* Only allowed if one interface is used and if the NICId's are the same! */
						if ((IfCount > 1) ||
							(IfCount == 1 && (!OHA_EDDCheckNICUsed(pDetail->Params.Sock.NICId))))
						{
							OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,
								 "WRONG SYSTEM ADAPTION(0x%X interfaces): Open this global SOCK channel first!", IfCount);
							Status = OHA_ERR_SEQUENCE;
							OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
							break;
						}

                        /* Get a free, initialized HDB */
                        Status = OHA_SOCKGetFreeHDB(&pHDBSock);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBSock);

                            pHDBSock->Params.pSys          = pSys;
                            pHDBSock->Params.pDetail       = pDetail;
                            pHDBSock->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBSock->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBSock->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBSock->ThisHandle;

                            /* Do a SOCK- open channel request */
                            Status = OHA_SOCKOpenChannel(pHDBSock->ThisHandle,pHDBSock->Params.SysPath,pHDBSock->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBSock->State  = OHA_HANDLE_STATE_OPEN_SOCK_RUNNING;
                                pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish           = LSA_FALSE;
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA-SOCK channel failed. (Rsp: %Xh)",Status);
                                OHA_SOCKReleaseHDB(pHDBSock);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA-SOCK channel failed. Cannot get SOCK handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* SNMPX-Channel.                                                */
                    /* --------------------------------------------------------------*/
                case OHA_PATH_TYPE_SNMPX:
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening SNMPX-SystemChannel");
#ifndef OHA_CFG_USE_SNMPX
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open channel failed. SNMPX not supported yet!");
                    Status = OHA_ERR_PARAM;
#else
                    /* Check if NIC-ID already used by a SNMPX-Channel. If so abort */

                    if ( OHA_SNMPXCheckNICUsed(0) )
                    {
                        Status = OHA_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA SNMPX-channel already used");
                    }
                    else
                    {
                        OHA_HDB_SNMPX_PTR_TYPE pHDBSnmpx;
                        /* Get a free, initialized HDB */
                        Status = OHA_SNMPXGetFreeHDB(&pHDBSnmpx);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBSnmpx);

                            pHDBSnmpx->Params.pSys          = pSys;
                            pHDBSnmpx->Params.pDetail       = pDetail;
                            pHDBSnmpx->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBSnmpx->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBSnmpx->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBSnmpx->ThisHandle;

                            /* Do a SNMPX-open channel request */
                            Status = OHA_SNMPXOpenChannel(pHDBSnmpx->ThisHandle,pHDBSnmpx->Params.SysPath,pHDBSnmpx->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBSnmpx->SnmpxState  = OHA_HANDLE_STATE_OPEN_SNMPX_RUNNING;
                                pHDBSnmpx->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish           = LSA_FALSE;
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA-SNMPX channel failed. (Rsp: %Xh)",Status);
                                OHA_SNMPXReleaseHDB(pHDBSnmpx);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA-SNMPX channel failed. Cannot get SNMPX handle! (Rsp: %Xh).",Status);
                        }
                    }
#endif
                    break;

                    /* --------------------------------------------------------------*/
                    /* NARE-Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case OHA_PATH_TYPE_NARE:
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening NARE- SystemChannel");

#ifdef OHA_CFG_NO_NARE
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open channel failed. NARE not supported yet!");
                    Status = OHA_ERR_PARAM;
#else
                    /* Check if NIC-ID already used by a NARE-Channel. If so abort */
                    if ( OHA_NARECheckNICUsed(pDetail->Params.Nare.NICId))
                    {
                        Status = OHA_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA NARE-channel failed. NIC-ID already used (ID: 0x%X)",pDetail->Params.Nare.NICId);
                    }
                    else
                    {
                        OHA_HDB_NARE_PTR_TYPE pHDBNare = LSA_NULL;
                        /* Get a free, initialized HDB */
                        Status = OHA_NAREGetFreeHDB(&pHDBNare);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBNare);

                            pHDBNare->Params.pSys          = pSys;
                            pHDBNare->Params.pDetail       = pDetail;
                            pHDBNare->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBNare->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBNare->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBNare->ThisHandle;

                            /* Do a NARE- open channel request */
                            Status = OHA_NAREOpenChannel(pHDBNare->ThisHandle,pHDBNare->Params.SysPath,pHDBNare->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBNare->State  = OHA_HANDLE_STATE_OPEN_NARE_RUNNING;
                                pHDBNare->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish           = LSA_FALSE;
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA-NARE channel failed. (Rsp: %Xh)",Status);
                                OHA_NAREReleaseHDB(pHDBNare);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA-NARE channel failed. Cannot get NARE handle! (Rsp: %Xh).",Status);
                        }
                    }
#endif
                    break;

                    /* --------------------------------------------------------------*/
                    /* MRP-Channel.                                                  */
                    /* --------------------------------------------------------------*/
                case OHA_PATH_TYPE_MRP:
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening MRP- SystemChannel");

#ifdef OHA_CFG_NO_MRP
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Open channel failed. MRP not supported yet!");
                    Status = OHA_ERR_PARAM;
#else
                    /* Check if NIC-ID already used by a MRP-Channel. If so abort */
                    if ( OHA_MRPCheckNICUsed(pDetail->Params.Mrp.NICId))
                    {
						Status = OHA_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA MRP-channel failed. NIC-ID already used (ID: 0x%X)",pDetail->Params.Mrp.NICId);
                    }
                    else
                    {
                        OHA_HDB_MRP_PTR_TYPE pHDBMrp = LSA_NULL;
                        /* Get a free, initialized HDB */
                        Status = OHA_MRPGetFreeHDB(&pHDBMrp);

                        if ( Status == OHA_OK )
                        {
                            OHA_ASSERT_NULL_PTR(pHDBMrp);

                            pHDBMrp->Params.pSys          = pSys;
                            pHDBMrp->Params.pDetail       = pDetail;
                            pHDBMrp->Params.Cbf           = pRQB->args.channel.oha_request_upper_done_ptr;
                            pHDBMrp->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBMrp->Params.HandleUpper   = pRQB->args.channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBMrp->ThisHandle;

                            /* Do a MRP- open channel request */
                            Status = OHA_MRPOpenChannel(pHDBMrp->ThisHandle,pHDBMrp->Params.SysPath,pHDBMrp->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBMrp->State  = OHA_HANDLE_STATE_OPEN_MRP_RUNNING;
                                pHDBMrp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish          = LSA_FALSE;
                            }
                            else
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open OHA-MRP channel failed. (Rsp: %Xh)",Status);
                                OHA_MRPReleaseHDB(pHDBMrp);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open OHA-MRP channel failed. Cannot get MRP handle! (Rsp: %Xh).",Status);
                        }
                    }
#endif
                    break;

                default:
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Unknown PathType in CDB (Type: %Xh)",pDetail->PathType);
                    Status = OHA_ERR_PARAM;
                    break;
            }/* switch */
        }

		/*----------------------------------------------------------------------------*/
		/* Should we finish the request ?                                             */
		/*----------------------------------------------------------------------------*/
		if ( Finish )  /* finish the request ? */
		{
			/* upper - handler already set */
			OHA_RQBSetResponse(pRQB,Status);
			OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
			OHA_CallCbf(pRQB->args.channel.oha_request_upper_done_ptr,pRQB,pSys);
		}

        /*----------------------------------------------------------------------------*/
        /* On error, if Syspath was opened we close the path                          */
        /*----------------------------------------------------------------------------*/
        if ( ( Status != OHA_OK ) &&  ( PathOpen)  )
        {
	        OHA_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling OHA_RELEASE_PATH_INFO");

	        OHA_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);

			if ( PathStat != LSA_RET_OK)
			{
				OHA_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"<--: OHA_RELEASE_PATH_INFO failed (Rsp: 0x%X).",PathStat);
				OHA_FATAL(OHA_FATAL_ERR_RELEASE_PATH_INFO);
			}
        }
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_open_channel()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_close_channel                           +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       OHA_OPC_CLOSE_CHANNEL         +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LSA_UINT16              Response      -                             +*/
/*+     OHA_RQB_ERROR_TYPE      Error         -                             +*/
/*+     OHA_RQB_ARGS_TYPE       Args          -                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user.       +*/
/*+     LSA_UINT16              Response    OHA_OK                          +*/
/*+                                         OHA_ERR_RESOURCE                +*/
/*+                                         OHA_ERR_PARAM                   +*/
/*+                                         ...                             +*/
/*+     OHA_RQB_ERROR_TYPE     Error       additional errorcodes from LL    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes a channel.                                         +*/
/*+               Close lower-layer channel if neccessary. (depends on      +*/
/*+               channel type). Releases Path-info. Note that this         +*/
/*+               function is done asynchron because of calling lower layer +*/
/*+               functions with callback. On error we may finish synchron. +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro OHA_RQB_ERROR with the RQB  +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               OHA_ERR_PARAM                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_close_channel(OHA_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16                Status;
    LSA_HANDLE_TYPE           Handle;
    LSA_BOOL                  Finish;
    LSA_HANDLE_TYPE           HandleUpper;
    OHA_HDB_USER_PTR_TYPE     pHDBUser;
    OHA_HDB_DCP_PTR_TYPE      pHDBDcp;
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    OHA_HDB_LLDP_PTR_TYPE     pHDBLldp;
    OHA_HDB_SOCK_PTR_TYPE     pHDBSock;
#ifdef OHA_CFG_USE_SNMPX
    OHA_HDB_SNMPX_PTR_TYPE     pHDBSnmpx;
#endif
#ifndef OHA_CFG_NO_NARE
    OHA_HDB_NARE_PTR_TYPE     pHDBNare;
#endif
#ifndef OHA_CFG_NO_MRP
    OHA_HDB_MRP_PTR_TYPE      pHDBMrp;
#endif

    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE          pSys;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_close_channel(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /*---------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                  */
    /*---------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    if ( OHA_RQB_GET_OPCODE(pRQB) != OHA_OPC_CLOSE_CHANNEL )
    {
        Status = OHA_ERR_PARAM;
        OHA_RQB_SET_RESPONSE(pRQB, Status);
        OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,">>>: oha_close_channel(): Invalid RQB-Opcode (0x%X)",OHA_RQB_GET_OPCODE(pRQB));
        OHA_RQB_ERROR(pRQB);
    }
    else
    {
        OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_CLOSE_CHANNEL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));

        /*-----------------------------------------------------------------------*/
        /* Check if it is allowed to close this channel.                         */
        /*-----------------------------------------------------------------------*/

        Handle = OHA_RQB_GET_HANDLE(pRQB);

        switch ( OHA_HANDLE_GET_TYPE(Handle))
        {
                /* ------------------------------------------------------------------*/
                /* USER Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_USER:
                pHDBUser = OHA_GetUserHDBFromHandle(Handle);
                /* pHDBDcp = OHAGetDcpHDBFromHandle(Handle); */

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser, LSA_NULL) )
                {
                    Finish      = LSA_TRUE;
                    Cbf         = pHDBUser->Params.Cbf;
                    HandleUpper = pHDBUser->Params.HandleUpper;
					pSys        = pHDBUser->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    /* only if no open/close request in progress */
                    if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL))
                    {
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Open or Close already running");
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else if (/*( pHDBUser->UserRCtrl.Used ) ||*/ pHDBUser->UserReqPendCnt)
                    {
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. SNMP-User requests pending");
                        Status = OHA_ERR_SEQUENCE;
                    }
#if 0
                    else if (OHA_IS_NOT_NULL(pHDBUser->SockUserReq.pBottom))
                    {
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. SOCK-User requests pending");
                        Status = OHA_ERR_SEQUENCE;
                    }
#endif
                    else if (OHA_IS_NOT_NULL(pHDBUser->TcIpUserReq.pBottom))
                    {
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. SOCK-TCIP-User requests pending");
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else if (OHA_CheckDcpControlUser(pHDBUser))
                    {
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): User request OHA_OPC_DCP_RESPONSE pending");
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else if (OHA_CheckSockUser(pHDBUser))
                    {
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): User request Agent (De)Activate pending");
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else
                    {
                        LSA_BOOL   Finish_Lldp = LSA_TRUE;  /* LldpControl (internal or user) */
                        LSA_BOOL   Finish_Nare = LSA_TRUE;  /* cyclic nare requests */
                        LSA_BOOL   Finish_Cyc  = LSA_TRUE;  /* cyclic timer */
                        LSA_BOOL   Finish_Ptcp = LSA_TRUE;  /* ptcp timer */
                        LSA_BOOL   Finish_Sock = LSA_TRUE;  /* SetAddress (IP) */

                        pHDBUser->pOCReq = pRQB; /* save RQB for later use */

                        /* cancel lldp-requests (OhaOpcLldpControl or local PrmCommit) */
                        (LSA_VOID)OHA_CancelLldpUser(pHDBUser, &Finish_Lldp);

                        /* stop PTCP timer if they are active */
                        (LSA_VOID)OHA_CancelPtcpTimer(pHDBUser, &Finish_Ptcp);

                        /* cancel SOCK-requests (OhaOpcSetAddress) */
                        (LSA_VOID)OHA_CancelSockUser(pHDBUser, &Finish_Sock);

#ifndef OHA_CFG_NO_NARE
                        /* stop the cyclic timer if there's the CheckInterval in use */
                        if (pHDBUser->Params.pDetail->Params.User.CheckInterval > 0)
                        {
                            /* cancel cyclic requests if there's the CheckInterval in use */
                            (LSA_VOID)OHA_CancelCyclicNare(pHDBUser, &Finish_Nare);

                            /* cancel cyclic timer if active */
                            (LSA_VOID)OHA_CancelCyclicTimer(pHDBUser, &Finish_Cyc);
                        }
#endif
                        Status = OHA_OK;

                        if ( Finish && Finish_Cyc && Finish_Nare && Finish_Lldp && Finish_Ptcp && Finish_Sock)
                        {
                            OHA_USERChannelDone(pHDBUser, OHA_OK);  /* we can close the channel and confirm the user */
                        }

                        Finish = LSA_FALSE;     /* to do underway or already done */
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-User Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }

                break;

                /* ------------------------------------------------------------------*/
                /* DCP -Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_DCP:
                pHDBDcp = OHAGetDcpHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBDcp, LSA_NULL) )
                {
					LSA_UINT16 UsedUserCnt = OHA_GetUsedUserCnt(pHDBDcp->Params.pDetail->Params.Dcp.NICId);

                    Finish      = LSA_TRUE;
                    Cbf         = pHDBDcp->Params.Cbf;
                    HandleUpper = pHDBDcp->Params.HandleUpper;
					pSys        = pHDBDcp->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    /* HDB-entry of the UserCnt */
                    if ( UsedUserCnt || pHDBDcp->UserCnt ) /* some user-channel uses this DCP-channel ?*/
                    {
                        OHA_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel() failed. Channel still in use by user-channels, UsedUserCnt (0x%X), UserCnt (0x%X)", UsedUserCnt, pHDBDcp->UserCnt);
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBDcp->pOCReq, LSA_NULL))
                        {
                            OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Close already running.");
                            Status = OHA_ERR_SEQUENCE;
                        }
                        else
                        {
                            /* first Cancel the ReceiveRQB's the close the channel ! */
                            /* Status = OHA_DCPCloseChannel(pHDBDcp->DcpHandle,pHDBDcp->Params.pSys); */
                            Status = OHA_DCPCancelReceive(pHDBDcp->DcpHandle,pHDBDcp->Params.pSys);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBDcp->DcpState  = OHA_HANDLE_STATE_CLOSE_DCP_RUNNING;
                                pHDBDcp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-DCP Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* EDD -Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_EDD:
                pHDBEdd = OHAGetEddHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL) )
                {
					LSA_UINT16 UsedUserCnt = OHA_GetUsedUserCnt(pHDBEdd->Params.pDetail->Params.Edd.NICId);

                    Finish      = LSA_TRUE;
                    Cbf         = pHDBEdd->Params.Cbf;
                    HandleUpper = pHDBEdd->Params.HandleUpper;
					pSys        = pHDBEdd->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    if ( UsedUserCnt || pHDBEdd->UserCnt ) /* some user-channel uses this EDD-channel ?*/
                    {
                        OHA_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel() failed. Channel still in use by user-channels, UsedUserCnt (0x%X), UserCnt (0x%X)", UsedUserCnt, pHDBEdd->UserCnt);
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd->pOCReq, LSA_NULL))
                        {
                            OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Close already running");
                            Status = OHA_ERR_SEQUENCE;
                        }
                        else
                        {
                            Status = OHA_EDDCloseChannel(pHDBEdd->EDDHandle,pHDBEdd->Params.pSys);

                            if ( Status == OHA_OK )
                            {
#ifndef OHA_CFG_NO_REMA
								/* the last sytem channel clears the list, otherwise we have no NicID */
								oha_PDEV_undo_init(pHDBEdd->Params.pDetail->Params.Edd.NICId);
                                oha_station_undo_init(pHDBEdd->Params.pDetail->Params.Edd.NICId);
                                oha_bus_undo_init(pHDBEdd->Params.pDetail->Params.Edd.NICId);
#endif
                                /* Request is underway now.. */
                                pHDBEdd->State  = OHA_HANDLE_STATE_CLOSE_EDD_RUNNING;
                                pHDBEdd->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }

                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-EDD Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* LLDP-Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_LLDP:
                pHDBLldp = OHAGetLldpHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL) )
                {
					LSA_UINT16 UsedUserCnt = OHA_GetUsedUserCnt(pHDBLldp->Params.pDetail->Params.Lldp.NICId);

                    Finish      = LSA_TRUE;
                    Cbf         = pHDBLldp->Params.Cbf;
                    HandleUpper = pHDBLldp->Params.HandleUpper;
					pSys        = pHDBLldp->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    if ( UsedUserCnt || pHDBLldp->UserCnt ) /* some user-channel uses this LLDP-channel ?*/
                    {
                        OHA_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel() failed. Channel still in use by user-channels, UsedUserCnt (0x%X), UserCnt (0x%X)", UsedUserCnt, pHDBLldp->UserCnt);
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBLldp->pOCReq, LSA_NULL))
                        {
                            OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Close already running.");
                            Status = OHA_ERR_SEQUENCE;
                        }
                        else
                        {
                            /* Status = OHA_LLDPCloseChannel(pHDBLldp->LldpHandle,pHDBLldp->Params.pSys); */
                            /* an internal stop (all) is made at any rate */
                            Status = OHA_LLDPStop(pHDBLldp, LLDP_PORT_ID_ALL, OHA_LLDP_SEND_RECV, OHA_HANDLE_TYPE_SYS);

                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBLldp->RequestState = OHA_LLDP_STATE_WF_STOP; /* LLDP-Stop is running (TFS 828801) */
                                /* pHDBLldp->State  = OHA_HANDLE_STATE_CLOSE_LLDP_RUNNING; */
                                pHDBLldp->State  = OHA_HANDLE_STATE_STOP_LLDP_RUNNING;
                                pHDBLldp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                            else
                            {
                                /* is this a fatal error ?                      */
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"Cannot Stop LLDP. Status: 0x%X", Status);
                            }
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-LLDP Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* SOCK-Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_SOCK: /* SOCK not supported yet */

                pHDBSock = OHAGetSockHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSock, LSA_NULL) )
                {
                    Finish      = LSA_TRUE;
                    Cbf         = pHDBSock->Params.Cbf;
                    HandleUpper = pHDBSock->Params.HandleUpper;
					pSys        = pHDBSock->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    if ( OHA_GET_GLOB_PTR()->UsedUserCnt || pHDBSock->UserCnt ) /* some user-channel uses this SOCK-channel ? */
                    {
                        OHA_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel() failed. Channel still in use by user-channels, UsedUserCnt (0x%X), UserCnt (0x%X)", OHA_GET_GLOB_PTR()->UsedUserCnt, pHDBSock->UserCnt);
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSock->pOCReq, LSA_NULL))
                    {
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Close already running.");
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else if (OHA_IS_NOT_NULL(pHDBSock->SetIpRQBs.pBottom))
                    {
                        /* SetIpAddress-Request is underway.. */
                        pHDBSock->State  = OHA_HANDLE_STATE_SETIP_SOCK_RUNNING;
                        pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                        OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"oha_close_channel(): Cancel SetIpAddress before Close");
                        Status = OHA_OK;
                        Finish = LSA_FALSE;
                    }
                    else
                    {
						if (pHDBSock->LldpAgent.SnmpHandle != SOCK_NO_HANDLE)	/* is the subagent to deactivate ? */
						{
							Status = OHA_SOCKClosePort(pHDBSock, OHA_LLDP_MIB_NODE);
						}
						else
						{
							/* Status = OHA_SOCKCloseChannel(pHDBSock->SockHandle,pHDBSock->Params.pSys); */
							Status = OHA_SOCKShutDown(pHDBSock);
						}
                        if ( Status == OHA_OK )
                        {
                            /* Request is underway now.. */
                            pHDBSock->State  = OHA_HANDLE_STATE_CLOSE_SOCK_RUNNING;
                            pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                            /* pHDBSock->State  = OHA_HANDLE_STATE_CLOSE_PORT_RUNNING; */
                            Finish = LSA_FALSE;
                        }
                        else
                        {
                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCKET(). Status: 0x%X", Status);
                            /* is this a fatal error ?                      */
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-SOCK Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* SNMPX-Channel.                                                    */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_SNMPX: /* SNMPX not supported yet */

#ifndef OHA_CFG_USE_SNMPX
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Close channel failed. SNMPX not supported yet!");
#else
                pHDBSnmpx = OHAGetSnmpxHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSnmpx, LSA_NULL) )
                {
                    Finish      = LSA_TRUE;
                    Cbf         = pHDBSnmpx->Params.Cbf;
                    HandleUpper = pHDBSnmpx->Params.HandleUpper;
					pSys        = pHDBSnmpx->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    if ( OHA_GET_GLOB_PTR()->UsedUserCnt || pHDBSnmpx->UserCnt ) /* some user-channel uses this SNMPX-channel ?*/
                    {
                        OHA_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel() failed. Channel still in use by user-channels, UsedUserCnt (0x%X), UserCnt (0x%X)", OHA_GET_GLOB_PTR()->UsedUserCnt, pHDBSnmpx->UserCnt);
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSnmpx->pOCReq, LSA_NULL))
                        {
                            OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Close already running.");
                            Status = OHA_ERR_SEQUENCE;
                        }
                        else
                        {
#if 0
                            Status = OHA_SNMPXCloseChannel(pHDBSnmpx->SnmpxHandle,pHDBSnmpx->Params.pSys);
                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBSnmpx->SnmpxState  = OHA_HANDLE_STATE_CLOSE_SNMPX_RUNNING;
                                pHDBSnmpx->pOCReq = pRQB; /* save pointer to request for later use */
                                /* pHDBSnmpx->State  = OHA_HANDLE_STATE_STOP_SNMPX_RUNNING; */
                                Finish = LSA_FALSE;
                            }
                            else
                            {
                                /* is this a fatal error ?                      */
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"Cannot Close SNMPX. Status: 0x%X", Status);
                            }
#endif
                            Status = OHA_SNMPXUnregisterAgent(pHDBSnmpx);
                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBSnmpx->SnmpxState  = OHA_HANDLE_STATE_UNREGISTER_SNMPX_RUNNING;
                                pHDBSnmpx->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                            else
                            {
                                /* is this a fatal error ?                      */
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"Cannot Close SNMPX. Status: 0x%X", Status);
                            }
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
#endif
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-SNMPX Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* NARE-Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_NARE:
#ifdef OHA_CFG_NO_NARE
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Close channel failed. NARE not supported yet!");
#else
                pHDBNare = OHAGetNareHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBNare, LSA_NULL) )
                {
					LSA_UINT16 UsedUserCnt = OHA_GetUsedUserCnt(pHDBNare->Params.pDetail->Params.Nare.NICId);

                    Finish      = LSA_TRUE;
                    Cbf         = pHDBNare->Params.Cbf;
                    HandleUpper = pHDBNare->Params.HandleUpper;
					pSys        = pHDBNare->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    if ( UsedUserCnt || pHDBNare->UserCnt ) /* some user-channel uses this EDD-channel ?*/
                    {
                        OHA_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Channel still in use by user-channels, UsedUserCnt (0x%X), UserCnt (0x%X)", UsedUserCnt, pHDBNare->UserCnt);
                        Status = OHA_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBNare->pOCReq, LSA_NULL))
                        {
                            OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Close already running");
                            Status = OHA_ERR_SEQUENCE;
                        }
                        else
                        {
                            if (pHDBNare->NareRQBCnt)   /* e.g. a Hello.Req is active */
                            {
                                OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"oha_close_channel(): (0x%X) Hello Request(s) active!", pHDBNare->NareRQBCnt);
                                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                            }

                            /* local CheckAddress-Data (OhaOpcSetAddress, OhaOpcDcpResponse) */
                            if (pHDBNare->IpCheck.Underway) /* is active ? */
                            {
                                /* we can cancel the running user request ResolveAddress */
                                if (OHA_IS_NULL(pHDBNare->IpCheck.pCancelRQB))
                                {
                                    pHDBNare->IpCheck.pCancelRQB = OHA_NARETestIpCancel(pHDBNare, pHDBNare->ThisHandle, (LSA_UINT32)OHA_HANDLE_TYPE_SYS);
                                }
                            }
                            if (pHDBNare->IpCheck.Underway)
                            {
                                /* Request is underway now.. */
                                pHDBNare->State  = OHA_HANDLE_STATE_CLOSE_NARE_RUNNING;
                                pHDBNare->pOCReq = pRQB; /* save pointer to request for later use */
                                break;
                            }

                            Status = OHA_NARECloseChannel(pHDBNare->NareHandle,pHDBNare->Params.pSys);
                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBNare->State  = OHA_HANDLE_STATE_CLOSE_NARE_RUNNING;
                                pHDBNare->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                            else
                            {
                                OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): OHA_NARECloseChannel() failed! (Status: 0x%X)",Status);
                            }
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
#endif
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-NARE Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* MRP-Channel.                                                      */
                /* ------------------------------------------------------------------*/
            case OHA_HANDLE_TYPE_MRP:
#ifdef OHA_CFG_NO_MRP
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"Close channel failed. MRP not supported yet!");
#else
                pHDBMrp = OHAGetMrpHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBMrp, LSA_NULL) )
                {
					LSA_UINT16 UsedUserCnt = OHA_GetUsedUserCnt(pHDBMrp->Params.pDetail->Params.Mrp.NICId);

                    Finish      = LSA_TRUE;
                    Cbf         = pHDBMrp->Params.Cbf;
                    HandleUpper = pHDBMrp->Params.HandleUpper;
					pSys        = pHDBMrp->Params.pSys;

                    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );

                    if ( UsedUserCnt || pHDBMrp->UserCnt ) /* some user-channel uses this MRP-channel ?*/
                    {
                        OHA_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Channel still in use by user-channels, UsedUserCnt (0x%X), UserCnt (0x%X)", UsedUserCnt, pHDBMrp->UserCnt);
						Status = OHA_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBMrp->pOCReq, LSA_NULL))
                        {
                            OHA_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Close channel failed. Close already running");
                            Status = OHA_ERR_SEQUENCE;
                        }
                        else
                        {
                            Status = OHA_MRPCloseChannel(pHDBMrp->MrpHandle,pHDBMrp->Params.pSys);
                            if ( Status == OHA_OK )
                            {
                                /* Request is underway now.. */
                                pHDBMrp->State  = OHA_HANDLE_STATE_CLOSE_MRP_RUNNING;
                                pHDBMrp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = LSA_FALSE;
                            }
                            else
                            {
                                OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): OHA_MRPCloseChannel() failed! (Status: 0x%X)",Status);
                            }
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						OHA_RQBSetResponse(pRQB,Status);
						OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
						OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
						OHA_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
#endif
                {
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid OHA-MRP Handle! (0x%X)",Handle);
                    Status = OHA_ERR_PARAM;
                    OHA_RQB_SET_RESPONSE(pRQB, Status);
                    OHA_RQB_ERROR(pRQB);
                }
                break;

            default:
                OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"oha_close_channel(): Invalid Handle (0x%X)",Handle);
                Status = OHA_ERR_PARAM;
                OHA_RQB_SET_RESPONSE(pRQB, Status);
                OHA_RQB_ERROR(pRQB);
                break;
        }
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_close_channel()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_request                                 +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       varies:                       +*/
/*+                                           OHA_OPC_GET_CONFIG            +*/
/*+                                           OHA_OPC_GET_NEIGHBOUR         +*/
/*+                                           OHA_OPC_GET_ADDRESS           +*/
/*+                                           OHA_OPC_DCP_CONTROL           +*/
/*+                                           OHA_OPC_DCP_INDICATION        +*/
/*+                                           OHA_OPC_LLDP_INDICATION       +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LSA_UINT16              Response      -                             +*/
/*+     OHA_RQB_ERROR_TYPE      Error         -                             +*/
/*+     OHA_RQB_ARGS_TYPE       Args          varies depending on opcode    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     upper-handle from open_channel  +*/
/*+     LSA_UINT16              Response    OHA_OK                          +*/
/*+                                         OHA_ERR_RESOURCE                +*/
/*+                                         OHA_ERR_PARAM                   +*/
/*+                                         ...                             +*/
/*+     OHA_RQB_ERROR_TYPE     Error       additional errorcodes from LL    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given on open_channel-request.             +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro OHA_RQB_ERROR with the RQB  +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               OHA_ERR_PARAM                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_request(OHA_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_HANDLE_TYPE          Handle;
    OHA_HDB_USER_PTR_TYPE    pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_request(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    Handle   = OHA_RQB_GET_HANDLE(pRQB);

    /* TIMER-Request needs special handling  */
    if ( OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_TIMER )
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,">>>: Request: OHA_OPC_TIMER (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
        OHA_RequestTimer(pRQB);

        OHA_EXIT();
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_request()");
        return;
    }

    pHDB = OHA_GetUserHDBFromHandle(Handle);

    if (! LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        /* no open/close runnning ? and channel open ?                    */
        /* Note: If a close-channel failed within some closing state. The */
        /*       channel can not be used anymore for this user - requests */
        /*       The channel is in some inoperable state and it can only  */
        /*       be tried to do another close-request..                   */

        if ( (pHDB->State == OHA_HANDLE_STATE_OPEN) &&
             LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) )
        {
            switch ( OHA_RQB_GET_OPCODE(pRQB))
            {
                case OHA_OPC_GET_CONFIG:      /* EDD, (LLDP) */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_GET_CONFIG (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestGetConfig(pRQB, pHDB);
                    break;
                case OHA_OPC_GET_ADDRESS:       /* EDD */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_GET_ADDRESS (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestGetAddress(pRQB, pHDB);
                    break;
                case OHA_OPC_SET_ADDRESS:       /* EDD */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_SET_ADDRESS (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestSetAddress(pRQB, pHDB);
                    break;
                case OHA_OPC_SET_EMERGENCY:       /* EDD */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_SET_EMERGENCY (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestSetAddress(pRQB, pHDB);
                    break;
                case OHA_OPC_SET_IDENT:         /* DCP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_SET_IDENT (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestSetIdent(pRQB, pHDB);
                    break;
                case OHA_OPC_HELLO_INDICATION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_HELLO_INDICATION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestHelloIndication(pRQB, pHDB);   /* EDD, DCP */
                    break;
                case OHA_OPC_DCP_CONTROL:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_DCP_CONTROL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestDcpControl(pRQB, pHDB);   /* DCP */
                    break;
                case OHA_OPC_DCP_INDICATION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_DCP_INDICATION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestDcpIndication(pRQB, pHDB);   /* DCP */
                    break;
                case OHA_OPC_DCP_RESPONSE:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_DCP_RESPONSE (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestDcpResponse(pRQB, pHDB);   /* DCP */
                    break;
                case OHA_OPC_TEST_LLDP_CONTROL:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_TEST_LLDP_CONTROL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestLldpControl(pRQB, pHDB);   /* LLDP */
                    break;
#if 0
                case OHA_OPC_PORT_CONTROL:  /* deactivated in V4.0i30 */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_PORT_CONTROL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPortControl(pRQB, pHDB);   /* LLDP */
                    break;
#endif
                case OHA_OPC_WRITE_PTCP_DATA:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_WRITE_PTCP_DATA (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPtcpWriteData(pRQB, pHDB);   /* LLDP */
                    break;
                case OHA_OPC_LLDP_INDICATION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_INDICATION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestLldpIndication(pRQB, pHDB);   /* LLDP */
                    break;
                case OHA_OPC_ADDR_INFO_IND:       /* EDD */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_ADDR_INFO_IND (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestAddressIndication(pRQB, pHDB);
                    break;
                case OHA_OPC_GET_NEIGHBOUR:        /* LLDP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_GET_NEIGHBOUR (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestGetNeighbour(pRQB, pHDB);
                    break;
                case OHA_OPC_LLDP_CONTROL_OID:          /* LLDP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_CONTROL_OID (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestControlSnmpOid(pRQB, pHDB);
                    break;
                case OHA_OPC_MRP_CONTROL_OID:          /* MRP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_MRP_CONTROL_OID (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestControlSnmpOid(pRQB, pHDB);
                    break;
                case OHA_OPC_DCP_READ_ELEMENT:          /* EDD + DCP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_DCP_READ_ELEMENT (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestReadDcpElement(pRQB, pHDB);
                    break;
                case OHA_OPC_DCP_WRITE_ELEMENT:         /* EDD + DCP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_DCP_WRITE_ELEMENT (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestWriteDcpElement(pRQB, pHDB);
                    break;
                case OHA_OPC_LLDP_READ_ELEMENT:         /* EDD + LLDP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_READ_ELEMENT (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestReadLldpElement(pRQB, pHDB);
                    break;
                case OHA_OPC_READ_QUERY_ELEMENTS:           /* EDD + DCP */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_READ_QUERY_ELEMENTS (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestReadQueryElements(pRQB, pHDB);
                    break;
                case OHA_OPC_TCIP_CONTROL:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_TCIP_CONTROL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestControlTcipData(pRQB, pHDB);   /* SOCK */
                    break;
#ifdef OHA_CFG_USE_SNMPX
                case OHA_OPC_TEST_SNMPX_CONTROL:         /* EDD + SNMPX */
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_TEST_SNMPX_CONTROL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestSnmpxControl(pRQB, pHDB);
                    break;
#endif
                case OHA_OPC_PRM_PREPARE:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_PRM_PREPARE (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPrmPrepare(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_PRM_WRITE:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_PRM_WRITE (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPrmWrite(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_PRM_END:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_PRM_END (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPrmEnd(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_PRM_COMMIT:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_PRM_COMMIT (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPrmCommit(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_PRM_READ:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_PRM_READ (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPrmRead(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_PRM_INDICATION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_PRM_INDICATION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestPrmIndication(pRQB, pHDB);   /* EDD, LLDP */
                    break;
#ifndef OHA_CFG_NO_REMA
                case OHA_OPC_REMA_INDICATION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_REMA_INDICATION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestRemaIndication(pRQB, pHDB);   /* EDD */
                    break;
                case OHA_OPC_REMA_READ:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_REMA_READ (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestRemaRead(pRQB, pHDB);   /* EDD */
                    break;
                case OHA_OPC_REMA_CONTROL:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_REMA_CONTROL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestRemaControl(pRQB, pHDB);   /* EDD */
                    break;
#endif
                case OHA_OPC_LLDP_READ_OPTIONS:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_READ_OPTIONS (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestReadLldpOptions(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_LLDP_WRITE_OPTION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_WRITE_OPTION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestWriteLldpOption(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_LLDP_ACCESS_CONTROL:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_ACCESS_CONTROL (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestLldpAccess(pRQB, pHDB);   /* EDD, LLDP */
                    break;
                case OHA_OPC_LLDP_CONFIG_INDICATION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_CONFIG_INDICATION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestLldpConfigIndication(pRQB, pHDB);   /* LLDP */
                    break;
                case OHA_OPC_LLDP_CONFIG_RESPONSE:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_CONFIG_RESPONSE (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestLldpConfigResponse(pRQB, pHDB);   /* LLDP */
                    break;
                case OHA_OPC_LLDP_PORT_CONFIG_INDICATION:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_PORT_CONFIG_INDICATION (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestLldpPortConfigIndication(pRQB, pHDB);   /* LLDP */
                    break;
                case OHA_OPC_LLDP_PORT_CONFIG_RESPONSE:
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: OHA_OPC_LLDP_PORT_CONFIG_RESPONSE (Handle: 0x%X)",OHA_RQB_GET_HANDLE(pRQB));
                    OHA_RequestLldpPortConfigResponse(pRQB, pHDB);   /* LLDP */
                    break;
                default:
                    OHA_RQB_SET_RESPONSE(pRQB, OHA_ERR_PARAM);
                    OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,">>>: oha_request(): Invalid RQB-Opcode (0x%X)",OHA_RQB_GET_OPCODE(pRQB));
                    OHA_RQB_ERROR(pRQB);
                    break;
            } /* switch */
        }
        else
        {
            OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,">>>: oha_request(): Channel not open or Open/Close running (0x%X)",OHA_RQB_GET_OPCODE(pRQB));
            OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_SEQUENCE);
        }
    }
    else
    {
        OHA_RQB_SET_RESPONSE(pRQB, OHA_ERR_PARAM);
        OHA_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,">>>: oha_request(): Invalid handle (0x%X)",Handle);
        OHA_RQB_ERROR(pRQB);
    }

    OHA_EXIT();
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_request()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UserCancelReceive                       +*/
/*+  Input                      LSA_HANDLE_TYPE       DCPHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : User HDB                                        +*/
/*+  pRQB                 : User RQB (CloseChannel)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Cancel Receive Request to User. The Request will be+*/
/*+               finished by calling the DCP-Callbackfunction.             +*/
/*+               oha_dcp_request_lower_done()                              +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple close request at a time.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UserCancelReceive(
    OHA_HDB_USER_PTR_TYPE       pHDB)
{
    LSA_UINT16                  Result  = OHA_OK;
    OHA_UPPER_RQB_PTR_TYPE      pRQBHelp;
    LSA_UINT16                  i;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UserCancelReceive(pHDB: 0x%X)",
                          pHDB);

    /* cancel all pending user-indication resources for DCP */
    while (pHDB->DcpIndReq.pBottom)
    {
        OHA_RQB_REM_BLOCK_BOTTOM(
            pHDB->DcpIndReq.pBottom,
            pHDB->DcpIndReq.pTop,
            pRQBHelp);

        pHDB->DcpIndReqPending--; /* not here, in the callback */

        OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
    }

    /* cancel all pending user-indication resources for LLDP */
    while (pHDB->LldpIndReq.pBottom)
    {
        OHA_RQB_REM_BLOCK_BOTTOM(
            pHDB->LldpIndReq.pBottom,
            pHDB->LldpIndReq.pTop,
            pRQBHelp);

        pHDB->LldpIndReqPending--; /* not here, in the callback */

        OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
    }

    /* cancel all pending user-indication resources for LLDP-MIB */
    while (pHDB->LldpPortConfigInd.pBottom)
    {
        OHA_RQB_REM_BLOCK_BOTTOM(
            pHDB->LldpPortConfigInd.pBottom,
            pHDB->LldpPortConfigInd.pTop,
            pRQBHelp);

        pHDB->LldpPortConfigIndPending--; /* not here, in the callback */

        OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
    }

    /* cancel all pending user-indication resources for LLDP-MIB */
    while (pHDB->LldpConfigInd.pBottom)
    {
        OHA_RQB_REM_BLOCK_BOTTOM(
            pHDB->LldpConfigInd.pBottom,
            pHDB->LldpConfigInd.pTop,
            pRQBHelp);

        pHDB->LldpConfigIndPending--; /* not here, in the callback */

        OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
    }

    /* cancel all pending user-indication resources for TOPO */
    for (i = 0; i <= OHA_CFG_MAX_PORTS; i++) /* port 0 too */
    {
        while (pHDB->TopoIndReq[i].pBottom)
        {
            OHA_RQB_REM_BLOCK_BOTTOM(
                pHDB->TopoIndReq[i].pBottom,
                pHDB->TopoIndReq[i].pTop,
                pRQBHelp);

            pHDB->TopoIndReqPending--; /* not here, in the callback */

            OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
        }
    }

    /* cancel all pending user-indication resources for ADDRESS */
    for (i = 0; i < OHA_CFG_MAX_NICS; i++)
    {
        OHA_UPPER_RQB_PTR_TYPE pRQBHelp = pHDB->pAddressInd[i];
        if (OHA_IS_NOT_NULL(pRQBHelp))
        {
            OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);

            pHDB->pAddressInd[i] = LSA_NULL;
        }
    }

#ifndef OHA_CFG_NO_NARE
    /* cancel all pending user-indication resources for NARE (cyclic CheckName, CheckIp) */
    while (pHDB->CheckIndReq.pBottom)
    {
        OHA_RQB_REM_BLOCK_BOTTOM(
            pHDB->CheckIndReq.pBottom,
            pHDB->CheckIndReq.pTop,
            pRQBHelp);

        pHDB->CheckIndReqPending--; /* not here, in the callback */

        OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
    }
#endif

    /* cancel all pending user-indication resources for DCP (Device Initiative) */
    while (pHDB->HelloIndReq.pBottom)
    {
        OHA_RQB_REM_BLOCK_BOTTOM(
            pHDB->HelloIndReq.pBottom,
            pHDB->HelloIndReq.pTop,
            pRQBHelp);

        pHDB->HelloIndReqPending--; /* not here, in the callback */

        OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
    }

    /* cancel all pending user-indication resources for REMA */
    for (i = 0; i < OHA_CFG_MAX_NICS; i++)
    {
		while (pHDB->RemaInd[i].pBottom)
		{
			OHA_RQB_REM_BLOCK_BOTTOM(
				pHDB->RemaInd[i].pBottom,
				pHDB->RemaInd[i].pTop,
				pRQBHelp);

	        pHDB->RemaIndPending[i]--;

			OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_UserCancelReceive(Result: 0x%X)", Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_USERChannelDone                         +*/
/*+  Input                      OHA_HDB_USER_PTR_TYPE         pHDB          +*/
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
/*+               USER-Channels. This function is called after a lower      +*/
/*+               layer called back the OHA (e.g. after USER finished his   +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_USERChannelDone(
    OHA_HDB_USER_PTR_TYPE       pHDB,
    LSA_UINT16                 Response)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB = pHDB->pOCReq;
    LSA_UINT16                  Status;
    LSA_HANDLE_TYPE             HandleUpper;
	LSA_SYS_PTR_TYPE            pSys;
	OHA_DETAIL_PTR_TYPE         pDetail;

    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_USERChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB, Response);

    if (OHA_IS_NULL(pRQB)) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

    Cbf         = pHDB->Params.Cbf;
    HandleUpper = pHDB->Params.HandleUpper;
	pSys        = pHDB->Params.pSys;
	pDetail     = pHDB->Params.pDetail;

    /* first Cancel the ReceiveRQB's, then close the channel ! */
    Status = OHA_UserCancelReceive(pHDB);
    if (Status != OHA_OK ) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

    /* reset DCP-control channel to this user if there is one */
    OHA_CancelDcpControlUser(pHDB);

#ifndef OHA_CFG_NO_NARE
    /* if there was a cyclic check running we've to free the rqb's */
    if (OHA_IS_NOT_NULL(pHDB->Params.pDetail) &&
        pHDB->Params.pDetail->Params.User.CheckInterval != 0)
    {
        (LSA_VOID) OHA_FreeCyclicNareRqbs(pHDB);
    }
#endif

    /* we release the HDB. Handle close now  */
    OHA_UserReleaseHDB(pHDB);

    /* confirm the user */
    OHA_RQBSetResponse(pRQB,Response);
    OHA_RQB_SET_HANDLE(pRQB, HandleUpper );
    OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Status);
    OHA_CallCbf(Cbf,pRQB,pSys);

    /* release PATH-Info ! */
    OHA_RELEASE_PATH_INFO(&Status,pSys,pDetail);
    if (Status != LSA_RET_OK ) OHA_FATAL(OHA_FATAL_ERR_RELEASE_PATH_INFO);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_USERChannelDone");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_USERCloseChannelDone                    +*/
/*+  Input                      OHA_HDB_USER_PTR_TYPE         pHDB          +*/
/*+                             LSA_UINT16                   Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to close Channel request                   +*/
/*+  Response   : Responsecode of actual action taken.                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handler for Close-Channel requests to OHA for USER-Chann. +*/
/*+               This function is called after a lower layer called back   +*/
/*+               the OHA (e.g. after USER finished his close request)      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_USERCloseChannelDone(
    OHA_HDB_USER_PTR_TYPE       pHDB,
    LSA_UINT16                 Response)
{
    OHA_UPPER_RQB_PTR_TYPE      pRQB = pHDB->pOCReq;
    LSA_BOOL                    Finish_Ptcp = LSA_TRUE; /* ptcp timer */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_USERCloseChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB, Response);

    if (OHA_IS_NULL(pRQB)) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    if (OHA_RQB_GET_OPCODE(pRQB)!=OHA_OPC_CLOSE_CHANNEL) OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

    /* is the timer to stop completely (timer rqb is outstanding) ? */
    if (pHDB->State == OHA_HANDLE_STATE_CLOSE_WAIT_TIMER)
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "The cyclic timer rqb is outstanding - no close.");
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_USERCloseChannelDone");
        return;
    }

    /* LLDP-requests (OhaOpcLldpControl or local PrmCommit) queued ? */
    if (pHDB->LldpReqPendingCnt)
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "lldp-requests (OhaOpcLldpControl and/or local PrmCommit) outstanding - no close.");
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_USERCloseChannelDone");
        return;
    }

    /* NARE-requests (TestIP or ResolveAddress) queued ? */


    /* SOCK-requests (OhaOpcSetAddress) queued ? */
    if (OHA_IS_NOT_NULL(pHDB->SockUserReq.pBottom))
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "sock-requests (OhaOpcSetAddress) outstanding - no close.");
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_USERCloseChannelDone");
        return;
    }

    /* no more ptcp timer underway ? */
    (LSA_VOID)OHA_CancelPtcpTimer(pHDB, &Finish_Ptcp);
    if (! Finish_Ptcp )
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "A further ptcp timer rqb is outstanding - no close.");
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_USERCloseChannelDone");
        return;
    }

    /* finally we can close the channel */
    OHA_USERChannelDone(pHDB, Response);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_USERCloseChannelDone");
    return;
}

/*****************************************************************************/
/*  end of file OHA_USR.C                                                    */
/*****************************************************************************/

