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
/*  F i l e               &F: oha_socu.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA SOCK functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  15
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_SOCU */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles */
#include "oha_int.h"            /* internal header */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

#ifndef OHA_CFG_NO_SOCK

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
/*+  Functionname          :    OHA_SOCKGetHandleByIf                       +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_SOCK_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceId      : InterfaceId from Table   1...n                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a SOCK HDB with IfId. If none is found    +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SOCKGetHandleByIf(
    LSA_UINT32 IfId)
{
    OHA_HDB_SOCK_PTR_TYPE pHDB = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKGetHandleByIf(IfId: 0x%X)",
                          IfId);

    if (IfId <= EDD_INTERFACE_ID_MAX) /* 0 is valid (global channel) */
    {
		LSA_UINT16 NicID = OHA_DB_GET_NIC(IfId);

		if(NicID != OHA_INVALID_NIC_ID)
		{
		    pHDB = OHA_SOCKGetHandleByNIC(NicID);
			if(OHA_IS_NULL(pHDB))
			{
				OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetHandleByIf(pHDB: 0x%X, NicID: 0x%X)", pHDB, NicID);
			}
		}
	}

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetHandleByIf(pHDB:  0x%X)", pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKReleaseHDB                          +*/
/*+  Input/Output          :    OHA_HDB_SOCK_PTR_TYPE pHDB                  +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBSock    : Pointer to User Handle to release                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release SOCK -HDB                                         +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKReleaseHDB(
    OHA_HDB_SOCK_PTR_TYPE pHDB)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    pHDB->State = OHA_HANDLE_STATE_FREE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKReleaseHDB()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKGetFreeHDB                          +*/
/*+  Input/Output          :    OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL..pHDBSock   +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA SOCK  -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free SOCK -HDB                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKGetFreeHDB(
    OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBSock)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found;
    OHA_HDB_SOCK_PTR_TYPE     pHDB  = OHA_GET_HSOCK_PTR();

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKGetFreeHDB(pHDBSock: 0x%X)",
                          pHDBSock);

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    {
        if ( pHDB->State == OHA_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            Found = LSA_FALSE;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..                  */
    /* --------------------------------------------------------------------------*/
    if ( Found )
    {
        pHDB->State = OHA_HANDLE_STATE_CLOSED;
        *pHDBSock = pHDB;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_SOCKGetFreeHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBSock);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestSockControl                      +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+  ControlMode: Activate (0) / Deactivate (1) the SNMP-Agent              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to activate/deactivate the SNMP-Agent by the user +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestSockControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
#ifndef OHA_SOCK_SNMP_OFF
    LSA_UINT16                Response = OHA_OK;
#else
    LSA_UINT16                Response = OHA_ERR_PRM_INDEX;  /* not allowed */
#endif
    OHA_HDB_SOCK_PTR_TYPE     pHDBSock;
    LSA_UINT16                NicId  = pHDBUser->Params.pDetail->Params.User.NICId;
    LSA_UINT32                ControlMode;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestSockControl(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    /* Check if we have the SOCK-Channel, only one over all NIC's. */
    pHDBSock  = OHA_SOCKGetHandleGlobal();  /* only EDD and SOCK for this request necessary */
	LSA_UNUSED_ARG(NicId);

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBSock,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request Activate/Deactivate SNMP-Agent failed. No Lower Channel for SOCK");
    }
    else
    {
		OHA_UPPER_PRM_DATA_PTR_TYPE pPrm = &pRQB->args.PrmData;

		(LSA_VOID)OHA_Get_PrmSNMPControlData(pPrm->record_data,     /* check the record and get the data */
											pPrm->record_data_length,
											&ControlMode);

        switch ( ControlMode )
        {
            case OHA_CTRL_SNMP_ACTIVATE:
                if (pHDBSock->UserRQBCnt) /* no queueing of user-requests in this version */
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request Activate SNMP-Agent failed. Already one User-Request active");
                    Response = OHA_ERR_SEQUENCE;
                }
                break;
            case OHA_CTRL_SNMP_DEACTIVATE:
                if (pHDBSock->UserRQBCnt) /* no queueing of user-requests in this version */
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request Deactivate SNMP-Agent failed. Already one User-Request active");
                    Response = OHA_ERR_SEQUENCE;
                }
                break;
            default:
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"SNMP-Control-Mode(0x%X) failed.[0x%X]",ControlMode, OHA_ERR_SYNTAX);
                Response = OHA_ERR_SYNTAX;
                break;
        }
    }

    if ( Response != OHA_OK )  /* finish the request ? */
    {
        OHA_UserRequestFinish(pHDBUser,pRQB,Response);
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestSockControl(Request finished)");
    }
    else
    {
        /* queue the request in the SOCK-HDB */
        OHA_RQB_PUT_BLOCK_TOP(pHDBSock->UserRQBs.pBottom,
                              pHDBSock->UserRQBs.pTop,
                              pRQB);
        pHDBSock->UserRQBCnt++;
        OHA_SOCKRequestHandler(pHDBSock, OHA_LLDP_MIB_NODE, OHA_OK);  /* call the sock-handler */
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_RequestSockControl(Request queued)");
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKGetHandleByNIC                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_SOCK_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a SOCK HDB with NICId. If none is found   +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SOCKGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_SOCK_PTR_TYPE  pHDB   = OHA_GET_HSOCK_PTR();
    LSA_BOOL               Found;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKGetHandleByNIC(NICId: 0x%X)",
                          NICId);

    {
        if ((pHDB->State == OHA_HANDLE_STATE_OPEN || 
			 pHDB->State == OHA_HANDLE_STATE_OPEN_PORT_RUNNING || 
			 pHDB->State == OHA_HANDLE_STATE_SETMIB2_SOCK_RUNNING || 
			 pHDB->State == OHA_HANDLE_STATE_OPEN_SOCK_RUNNING) &&
            (pHDB->Params.pDetail->Params.Sock.NICId == NICId))
        {
            Found = LSA_TRUE;
        }
        else
        {
            Found = LSA_FALSE;
        }
    }

    if ( ! Found ) pHDB = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetHandleByNIC(pHDB:  0x%X)", pHDB);
    return(pHDB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKCheckNICUsed                        +*/
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
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_SOCKCheckNICUsed(
    LSA_UINT16 NICId)
{
    OHA_HDB_SOCK_PTR_TYPE  pHDB  =  OHA_GET_HSOCK_PTR();
    LSA_BOOL              Found =   LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKCheckNICUsed(NICId: 0x%X)",
                          NICId);

    if (NICId != OHA_INVALID_NIC_ID)
    {
        {
            if (( pHDB->State != OHA_HANDLE_STATE_FREE ) &&
                ( pHDB->Params.pDetail->Params.Sock.NICId == NICId  ))

            {
                Found = LSA_TRUE;
            }
            else
            {
                Found = LSA_FALSE;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKCheckNICUsed(Found:  0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKGetHandleGlobal                     +*/
/*+  Input/Output          :                                                +*/
/*+  Result                :    OHA_HDB_SOCK_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to the globalSOCK HDB if opened.             +*/
/*+               If none is found or not opened LSA_NULL is returned       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SOCKGetHandleGlobal(
    LSA_VOID)
{
    OHA_HDB_SOCK_PTR_TYPE pHDBSock = OHAGetSockHDBFromHandle(OHA_HANDLE_TYPE_SOCK + 0); /* the global channel */
    OHA_HDB_SOCK_PTR_TYPE  pHDB    = LSA_NULL;

	OHA_ASSERT_NULL_PTR(pHDBSock);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKGetHandleGlobal()");

    if (pHDBSock->State == OHA_HANDLE_STATE_OPEN || /* same as in OHA_SOCKGetHandleByNIC() */
		 pHDBSock->State == OHA_HANDLE_STATE_OPEN_PORT_RUNNING || 
		 pHDBSock->State == OHA_HANDLE_STATE_SETMIB2_SOCK_RUNNING || 
		 pHDBSock->State == OHA_HANDLE_STATE_OPEN_SOCK_RUNNING)
	{
		OHA_ASSERT_NULL_PTR(pHDBSock->Params.pDetail);

		pHDB = pHDBSock;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKGetHandleGlobal(pHDB:  0x%X)", pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKUserCheckPending                    +*/
/*+  Input/Output          :    OHA_HDB_SOCK_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKUserCheckPending(
    OHA_HDB_SOCK_PTR_TYPE   pHDBSock,
    LSA_UINT16              Response)
{
    OHA_HDB_USER_PTR_TYPE     pHDBUser;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKUserCheckPending(pHDBSock: 0x%X, Response: 0x%X)",
                          pHDBSock, Response);

    if ( pHDBSock->UserRQBCnt ) /* User-RCTRL-Blocks pending ? */
    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBSock->UserRQBs.pBottom,
                                 pHDBSock->UserRQBs.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);
        pHDBSock->UserRQBCnt--;

        /* control = &pRQBUser->args.SnmpControl; */
        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);
        OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKUserCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKUserSetMib2ParamsPending            +*/
/*+  Input/Output          :    OHA_HDB_SOCK_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKUserSetMib2ParamsPending(
	OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response)
{
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKUserSetMib2ParamsPending(pHDBSock: 0x%X, Response: 0x%X)",
                          pHDBUser, Response);

    {
        OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

	    OHA_ASSERT_NULL_PTR(pHDBUser);

	    OHA_ASSERT_NULL_PTR(pHDBUser->Mib2UserReq.pBottom); /* User-RCTRL-Blocks pending ? */

        OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->Mib2UserReq.pBottom,
                                 pHDBUser->Mib2UserReq.pTop,
                                 pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        OHA_ASSERT_NULL_PTR(pRQBUser);

		switch (OHA_RQB_GET_OPCODE(pRQBUser))
		{
			case OHA_OPC_PRM_WRITE:
			{
				if (Response == OHA_OK)
				{
					Response = OHA_DoPrmWriteSNMP(pRQBUser, pHDBUser);
				}
			}
			break;
			case OHA_OPC_LLDP_WRITE_OPTION:	/* already written to DB */
			break;
			default:
				Response = OHA_ERR_PRM_INDEX;
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "Opcode Error: OHA_SOCKUserSetMib2ParamsPending(Opcode: 0x%X)", OHA_RQB_GET_OPCODE(pRQBUser));
				OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
			break;
		}

        Handle = OHA_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = OHA_GetUserHDBFromHandle(Handle);
        OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKUserSetMib2ParamsPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKRequestHandler                      +*/
/*+                        :    OHA_HDB_SOCK_PTR_TYPE      pHDB             +*/
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
/*+  Description: Statemaching for SOCK-User-Requests.                      +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKRequestHandler(
    OHA_HDB_SOCK_PTR_TYPE           pHDBSock,
    OHA_OID                         MibNode,
    LSA_UINT16                      Response)
{
    LSA_BOOL                  Ready    = LSA_FALSE;
    OHA_UPPER_RQB_PTR_TYPE    pRQBUser;
    OHA_HDB_AGNT_PTR_TYPE     pSubAgnt;
    LSA_UINT32                SnmpControlMode = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SockRequestHandler(pHDBSock: 0x%X, MibNode: 0x%X, Response: 0x%X)",
                          pHDBSock, MibNode, Response);

    OHA_ASSERT_NULL_PTR(pHDBSock);
	pRQBUser = pHDBSock->UserRQBs.pBottom;

	OHA_ASSERT(MibNode == OHA_LLDP_MIB_NODE || MibNode == OHA_MRP_MIB_NODE);
	pSubAgnt = (MibNode == OHA_LLDP_MIB_NODE) ? &pHDBSock->LldpAgent : &pHDBSock->MrpAgent;

	if(OHA_IS_NOT_NULL(pRQBUser))
	{
		OHA_UPPER_PRM_DATA_PTR_TYPE pPrm = &pRQBUser->args.PrmData;

		OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQBUser)==OHA_OPC_PRM_WRITE);
		OHA_ASSERT(pPrm->record_index == OHA_RECORD_INDEX_SNMP_CONTROL);
		(LSA_VOID)OHA_Get_PrmSNMPControlData(pPrm->record_data,     /* check the record and get the data */
											 pPrm->record_data_length,
											 &SnmpControlMode);
		OHA_ASSERT(SnmpControlMode == OHA_CTRL_SNMP_ACTIVATE || SnmpControlMode == OHA_CTRL_SNMP_DEACTIVATE);
	}

    /* -----------------------------------------------------------------------*/
    /* forward in statemachine till an asynchroneous request is underway which*/
    /* we have to wait for.                                                   */
    /* -----------------------------------------------------------------------*/
    while (! Ready)
    {
        Ready = LSA_TRUE;      /* Default: break loop because request underway */

        switch (pSubAgnt->RequestState)
        {
            case OHA_SNMP_STATE_STOP:   /* initial, after OpenChannel: Agent is in Deact */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_STOP (Resp: %Xh)",pHDBSock->ThisHandle,Response);

                /* comes from the user ? */
                /* stopped state and a User-STOP_SNMP => no action */
                if (pRQBUser && (SnmpControlMode == OHA_CTRL_SNMP_DEACTIVATE))
                {
                    /* check for pending user-requests and confirm  */
                    OHA_SOCKUserCheckPending(pHDBSock, OHA_OK);
                    break;
                }

                /* User-OHA_OPC_START_SNMP => activate the SNMP-agent */
                if (pRQBUser && (SnmpControlMode == OHA_CTRL_SNMP_ACTIVATE))
                {
                    /* Activate the SNMP-Agent   */
                    LSA_UINT16 RetVal = OHA_SOCKOpenPort(pHDBSock, MibNode);
                    if ( RetVal != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing SockAgentActivate-Request failed.");
                        /* check for pending user-requests and confirm  */
                        OHA_SOCKUserCheckPending(pHDBSock, RetVal);
                    }
                    else
                    {
                        pSubAgnt->RequestState = OHA_SNMP_STATE_WF_START;
                    }
                    break;
                }

                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            case OHA_SNMP_STATE_WF_START:   /* OHA_OPC_START_SNMP.Req is active    */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_WF_START (Resp: %Xh)",pHDBSock->ThisHandle,Response);

                /* is there an error during OHA_OPC_START_SNMP ? */
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SOCK SockAgentActivate request failed. Response: 0x%X", Response);

                    pSubAgnt->RequestState = OHA_SNMP_STATE_STOP;
                }
                else
                {
                    /* NOTE: Providing Recv-Indication Request must be          */
                    /*       the last action before going into State "open".    */
                    /*       So only Indciations while in Open-state must be    */
                    /*       handled and reprovided!                            */

                    if (!pSubAgnt->RxPendingCnt) /* no Recv-Requests pending within SOCK (first Activate-Call ?) */
                    {
                        /* provide SOCK-Recv-Requests */
                        Response = OHA_SOCKRecvProvide(pHDBSock, MibNode, OHA_SNMP_NUM_OF_RECV);

                        if ( Response != OHA_OK )
                        {
                            /* Any Recv-Request failed. Cancel Recv-Requests (by DeactAgent) */
                            OHA_PROGRAM_TRACE_01(pHDBSock->Params.pDetail->Params.Sock.NICId, LSA_TRACE_LEVEL_UNEXP,"Any SOCK Receive Provide request failed. Response: 0x%X", Response);
                        }
                    }

                    /* no more actions needed. Agent is started now. */
                    pSubAgnt->RequestState = OHA_SNMP_STATE_START; /* initialize */
                }
                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_SOCKUserCheckPending(pHDBSock, Response);
                break;

            case OHA_SNMP_STATE_WF_STOP:     /* OHA_OPC_STOP_SNMP.Req is active   */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_WF_STOP (Resp: %Xh)",pHDBSock->ThisHandle,Response);

                /* is there an error during DEACTIVATE_AGENT ? */
                if (Response != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"SOCK SockAgentDeactivate request failed. Response: 0x%X", Response);

                    pSubAgnt->RequestState = OHA_SNMP_STATE_START;
                }
                else
                {
                    /* is there a stored receive-rqb and response-rqb */
                    OHA_SOCKFreeAgent(pSubAgnt, pHDBSock);
                    pSubAgnt->RequestState = OHA_SNMP_STATE_STOP;
                }
                /* confirm to the user, if exist */
                /* check for pending user-requests   */
                OHA_SOCKUserCheckPending(pHDBSock, Response);
                break;

            case OHA_SNMP_STATE_WF_STATISTICS:
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_WF_STAT (Resp: %Xh)",pSubAgnt->RequestState,Response);
            case OHA_SNMP_STATE_WF_USER:
                /* no break, intentionally left ! */

            case OHA_SNMP_STATE_START:      /* SNMP-agent is active     */
                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_START (Resp: %Xh)",pSubAgnt->RequestState,Response);

                if (pRQBUser && (SnmpControlMode == OHA_CTRL_SNMP_DEACTIVATE))
                {
                    LSA_UINT16 RetVal = OHA_SOCKClosePort(pHDBSock, MibNode);
                    if (RetVal != OHA_OK)
                    {                           /* confirm to the user, if exist */
                        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing SockAgentDeactivate-Request failed.");
                        /* check for pending user-requests   */
                        OHA_SOCKUserCheckPending(pHDBSock, Response);
                    }
                    else
                    {
                        pSubAgnt->RequestState = OHA_SNMP_STATE_WF_STOP;
                    }
                    break;
                }
                /* started state and a User-START => no action */
                if (pRQBUser && (SnmpControlMode == OHA_CTRL_SNMP_ACTIVATE))
                {
                    /* check for pending user-requests and confirm  */
                    OHA_SOCKUserCheckPending(pHDBSock, OHA_OK);
                    break;
                }

                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;

            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDBSock->Params.pDetail->Params.Sock.NICId), LSA_TRACE_LEVEL_FATAL,"OHA_SockRequestHandler(): Invalid RState (%Xh)",pSubAgnt->RequestState);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SockRequestHandler(), state %Xh", pSubAgnt->RequestState);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKUserSetIpPending                    +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE      pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to USER-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-Request SetIp is pending.                  +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKUserSetIpPending(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response)
{
    OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
#ifndef OHA_CFG_NO_NARE
    LSA_BOOL                DoFinish;
#endif
    LSA_UINT32              CancelOptions = 0;
    LSA_UINT16              NicId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_SOCKUserSetIpPending(pHDBUser: 0x%X, Response: 0x%X)",
                          pHDBUser, Response);

    OHA_ASSERT_NULL_PTR(pHDBUser);

    OHA_ASSERT_NULL_PTR(OHA_IS_NOT_NULL(pHDBUser->SockUserReq.pBottom)); /* User-RCTRL-Blocks pending ? */

    pRQBUser     = pHDBUser->SockUserReq.pBottom;
    NicId        = pHDBUser->Params.pDetail->Params.User.NICId;

    switch (OHA_RQB_GET_OPCODE(pRQBUser))
    {
        case OHA_OPC_SET_ADDRESS:
        case OHA_OPC_SET_EMERGENCY:
        {
		    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SOCKUserSetIpPending done (NICId: 0x%X,Resp: 0x%X,Opcode: 0x%X)",NicId,Response,OHA_RQB_GET_OPCODE(pRQBUser));

            if (Response != OHA_OK) /* e.g. Cancel or SockSetIpAddress() failed */
            {
                pRQBUser->args.Address.Options &= ~OHA_CTRL_IP_SUITE;   /* IP-Address not set */
                pRQBUser->args.Address.Options &= ~OHA_CTRL_RESET_TO_FACTORY; /* or RTF not set */
	            pRQBUser->args.Address.Options &= ~OHA_CTRL_RESET_COMMUNICATION; /* or Reset not set */
            }
            CancelOptions = pRQBUser->args.Address.Options;
			if(pRQBUser->args.Address.CheckIpEnabled) /* AP01522573: ip only */
			{
                CancelOptions |= OHA_CTRL_IP_SUITE;
			}
            if ( (CancelOptions & OHA_CTRL_RESET_TO_FACTORY) ||
                 (CancelOptions & OHA_CTRL_RESET_COMMUNICATION) )
            {
                CancelOptions = OHA_CTRL_IP_SUITE | OHA_CTRL_STATION_NAME;
            }
        }
        break;

        case OHA_OPC_DCP_RESPONSE:
        {
            OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd = &pRQBUser->args.DcpInd;
            OHA_ASSERT_NULL_PTR(pInd);

		    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SOCKUserSetIpPending done (NICId: 0x%X,Resp: 0x%X,Opcode: 0x%X)",NicId,Response,OHA_RQB_GET_OPCODE(pRQBUser));

            if (Response != OHA_OK) /* e.g. Cancel SockSetIpAddress() failed */
            {
                (LSA_VOID)oha_db_delete_temp_ip_address(NicId);
            }
            CancelOptions = pInd->Options;
			if(pInd->CheckIpEnabled) /* IP to check ? */
			{
                CancelOptions |= OHA_CTRL_IP_SUITE;
			}
			if ( (CancelOptions & OHA_CTRL_RESET_TO_FACTORY) ||
                 (CancelOptions & OHA_CTRL_RESET_COMMUNICATION) )
			{
				CancelOptions = OHA_CTRL_IP_SUITE | OHA_CTRL_STATION_NAME;
			}
        }
        break;

        case OHA_OPC_DCP_WRITE_ELEMENT:

		    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SOCKUserSetIpPending done (NICId: 0x%X,Resp: 0x%X,Opcode: 0x%X)",NicId,Response,OHA_RQB_GET_OPCODE(pRQBUser));

            if (Response != OHA_OK) /* e.g. Cancel or SockSetIpAddress() failed */
            {
                /* no action here */
            }
            else
            {
                /* the ip address was set (changed), a check-IP (ethernet) is to cancel */
                CancelOptions = OHA_CTRL_IP_SUITE;
            }
            break;

        case OHA_OPC_PRM_COMMIT:

		    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SOCKUserSetIpPending done (NICId: 0x%X,Resp: 0x%X,Opcode: 0x%X)",NicId,Response,OHA_RQB_GET_OPCODE(pRQBUser));

            if (Response != OHA_OK) /* e.g. Cancel or PrmCommit(ResetToFactory) failed */
            {
                /* no action here */
            }
            else
            {
                /* the ip address was set (changed), a check-IP (ethernet) is to cancel */
                CancelOptions = OHA_CTRL_IP_SUITE | OHA_CTRL_STATION_NAME;
            }
            break;

        case OHA_OPC_PRM_WRITE:
        {
		    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SOCKUserSetIpPending done (NICId: 0x%X,Resp: 0x%X,Opcode: 0x%X)",NicId,Response,OHA_RQB_GET_OPCODE(pRQBUser));

            if (Response != OHA_OK) /* e.g. Cancel or PrmCommit(ResetToFactory) failed */
            {
                /* no action here */
            }
            else
            {
                switch (pRQBUser->args.PrmData.record_index)
                {
                    case OHA_RECORD_INDEX_IP_SUITE:
                    case OHA_RECORD_INDEX_DCP_IP_SUITE:
                        /* AP00961349: the ip address was set (changed), a check-IP (ethernet) is to cancel */
                        CancelOptions = OHA_CTRL_IP_SUITE;
                        break;
                    case OHA_RECORD_INDEX_MULTIPLE_STATION:
                        (LSA_VOID)OHA_Get_PrmStationData(pRQBUser->args.PrmData.record_data,
                                                         pRQBUser->args.PrmData.record_data_length,
                                                         &CancelOptions,
                                                         LSA_NULL,
                                                         LSA_NULL,
                                                         LSA_NULL);
                        break;
                    default:
                        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                        break;
                }
            }
        }
        break;

        default:
            NicId = OHA_INVALID_NIC_ID;

		    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_SOCKUserSetIpPending done (NICId: 0x%X,Resp: 0x%X,Opcode: 0x%X)",NicId,Response,OHA_RQB_GET_OPCODE(pRQBUser));

            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

#ifndef OHA_CFG_NO_NARE
    /* cancel running check requests */
    DoFinish = OHA_NARECheckAddressCancel(NicId, CancelOptions, OHA_RQB_GET_HANDLE(pRQBUser));
    if (DoFinish)
#endif
    {
        OHA_UserSetAddrPending (pHDBUser, Response);
    }

    /* is an open/close user-channel runnning ? */
    if (!( pHDBUser->State == OHA_HANDLE_STATE_OPEN &&
           LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ))
    {
        if (OHA_RQB_GET_OPCODE(pHDBUser->pOCReq)==OHA_OPC_CLOSE_CHANNEL)
        {
            LSA_BOOL Finish_Sock = LSA_TRUE;

            (LSA_VOID)OHA_CancelSockUser(pHDBUser, &Finish_Sock);
            if (Finish_Sock)
            {
                OHA_USERCloseChannelDone(pHDBUser,OHA_OK);
            }
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_SOCKUserSetIpPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CancelSockUser                          +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel handles a SOCK-Request set Flag TRUE +*/
/*+               SOCK-Requests are:                                        +*/
/*+                      local SetIpAddress ... OHA_OPC_SET_ADDRESS         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelSockUser(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish
)
{
    LSA_BOOL                    Finish   = LSA_TRUE;
    LSA_UINT16                  Status   = OHA_OK;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CancelSockUser(pHDBUser: 0x%X)",
                          pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pFinish);

    if (OHA_IS_NOT_NULL(pHDBUser->SockUserReq.pBottom))
    {
        Finish = LSA_FALSE;
    }
    if (OHA_IS_NOT_NULL(pHDBUser->Mib2UserReq.pBottom))
    {
        Finish = LSA_FALSE;
    }
    if (OHA_IS_NOT_NULL(pHDBUser->TcIpUserReq.pBottom))
    {
        Finish = LSA_FALSE;
    }

    *pFinish = Finish;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_CancelSockUser(Status=0x%X)", Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CheckSockUser                           +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel controls a Dcp-Server and a          +*/
/*+               OHA_OPC_DCP_RESPONSE is active -> returns LSA_TRUE        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckSockUser(
    OHA_HDB_USER_PTR_TYPE         pHDBUser
)
{
    OHA_HDB_SOCK_PTR_TYPE       pHDBSock = OHA_SOCKGetHandleGlobal();
    LSA_BOOL                    RetVal   = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CheckSockUser(pHDBUser: 0x%X)",
                          pHDBUser);

    /* Agent activate / deactivate running by this user ? */
    if (OHA_IS_NOT_NULL(pHDBSock) && pHDBSock->UserRQBs.pBottom)
    {
        LSA_HANDLE_TYPE ActiveHandle = OHA_RQB_GET_HANDLE(pHDBSock->UserRQBs.pBottom);

        if(ActiveHandle == pHDBUser->ThisHandle)
        {
            RetVal = LSA_TRUE;
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_CheckSockUser()");
    return RetVal;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestControlTcipData                  +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_READ/WRITE_TCIP_DATA    +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.TcipData                                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to set or get keep alive parameters of an IP stack */
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestControlTcipData(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16                Response;
    OHA_HDB_SOCK_PTR_TYPE     pHDBSock;
    LSA_UINT16                NicId;
	
    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser->Params.pDetail);
	NicId = pHDBUser->Params.pDetail->Params.User.NICId;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestControlTcipData(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_TCIP_CONTROL);

    /* Check if we have the SOCK-Channel, only one over all NIC's. */
    pHDBSock  = OHA_SOCKGetHandleGlobal();  /* only SOCK for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBSock,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_TCIP_CONTROL failed. No such Lower Channel for SOCK");
    }
    else if (!(pHDBUser->State == OHA_HANDLE_STATE_OPEN &&  /* is no open/close user-channel runnning ? */
               LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL) ) )
    {
        Response = OHA_ERR_SEQUENCE;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_TCIP_CONTROL failed. User Channel is closing or not open for this NicId (NIC-ID: 0x%X)",NicId);
    }
	else if (OHA_IsAnyLocalCtrlTcIpActive(NicId)) /* ControlTimeParams from any other user running ? */
	{
		Response = OHA_ERR_SEQUENCE;  /* already one user request active */
		OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH, "OHA_OPC_TCIP_CONTROL failed. Request from another user is active (NIC-ID: 0x%X)!",NicId);
	}
	else if (pRQB->args.TcipData.TimeOption != OHA_GET_TIME &&
			 pRQB->args.TcipData.TimeOption != OHA_SET_TIME &&
			 pRQB->args.TcipData.TimeOption != OHA_RESET_TIME)
	{
        Response = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_TCIP_CONTROL failed. Invalid parameter TimeOption (0x%X)",pRQB->args.TcipData.TimeOption);
	}
    else
    {
	    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQBSock  = LSA_NULL;
        OHA_UPPER_TCIP_DATA_PTR_TYPE pTcipData = &pRQB->args.TcipData;
	    LSA_USER_ID_TYPE            UserId;

		UserId.uvar32 = 0;
		OHA_SOCK_ALLOC_LOWER_RQB(&pRQBSock,UserId,sizeof(SOCK_RQB_TYPE),pHDBSock->Params.pSys);
		if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBSock, LSA_NULL) )
		{
			LSA_UINT16 Orginator = OHA_RQB_GET_HANDLE(pRQB);

			OHA_SOCK_RQB_SET_OPCODE(pRQBSock,SOCK_OPC_CNTRL_TIMEPARAMS);
			OHA_SOCK_RQB_SET_HANDLE(pRQBSock,pHDBSock->SockHandle);
			OHA_SOCK_RQB_SET_USERID_UVAR16(pRQBSock,Orginator);

			if (pRQB->args.TcipData.TimeOption == OHA_SET_TIME)
			{
				pRQBSock->args.timeparam_rq.timeopt = SOCK_SET_TIMEPARAMS;
				pRQBSock->args.timeparam_rq.connectiontimeout = pTcipData->ConnectionTimeout;
				pRQBSock->args.timeparam_rq.keepalivetime     = pTcipData->KeepAliveTime;
				pRQBSock->args.timeparam_rq.retransmitcount   = pTcipData->RetransmitCount;
				pRQBSock->args.timeparam_rq.retranstimeout    = pTcipData->RetransmitTimeout;
			}
			else if (pRQB->args.TcipData.TimeOption == OHA_RESET_TIME)
			{
				pRQBSock->args.timeparam_rq.timeopt = SOCK_SET_TIMEPARAMS_CLOSE_TCP_CONN;
				pRQBSock->args.timeparam_rq.connectiontimeout = pTcipData->ConnectionTimeout;
				pRQBSock->args.timeparam_rq.keepalivetime     = pTcipData->KeepAliveTime;
				pRQBSock->args.timeparam_rq.retransmitcount   = pTcipData->RetransmitCount;
				pRQBSock->args.timeparam_rq.retranstimeout    = pTcipData->RetransmitTimeout;
			}
			else if (pRQB->args.TcipData.TimeOption == OHA_GET_TIME)
			{
				pRQBSock->args.timeparam_rq.timeopt = SOCK_GET_TIMEPARAMS;
				pRQBSock->args.timeparam_rq.connectiontimeout = 0;
				pRQBSock->args.timeparam_rq.keepalivetime     = 0;
				pRQBSock->args.timeparam_rq.retransmitcount   = 0;
				pRQBSock->args.timeparam_rq.retranstimeout    = 0;
			}
			
			/* queue the user-rqb */
			OHA_ASSERT(OHA_IS_NULL(pHDBUser->TcIpUserReq.pBottom));
			OHA_RQB_PUT_BLOCK_TOP(pHDBUser->TcIpUserReq.pBottom,
								  pHDBUser->TcIpUserReq.pTop,
								  pRQB);

			OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_CNTRL_TIMEPARAMS (SOCKHandle: 0x%X,pRQBSock: 0x%X)",pHDBSock->SockHandle,pRQBSock);
			OHA_SOCK_REQUEST_LOWER(pRQBSock,pHDBSock->Params.pSys);

			Response = OHA_OK_ACTIVE;
			OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestControlTcipData: 0x%X)", Response);
			return;
		}
		else
		{
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory for OHA_OPC_TCIP_CONTROL failed!");
			Response = OHA_ERR_RESOURCE;
		}
	}

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestControlTcipData(Request finished: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestControlTcipDataDone              +*/
/*+  Input                      OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB           +*/
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
/*+  Description: Control TcIP data done handling (Get, Set)                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_RequestControlTcipDataDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB)
{
	OHA_HDB_USER_PTR_TYPE   pHDBUser;
    OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
    LSA_UINT16              Orginator;
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestControlTcipDataDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT(OHA_SOCK_RQB_GET_OPCODE(pRQB) == SOCK_OPC_CNTRL_TIMEPARAMS);
    OHA_ASSERT(pRQB->args.timeparam_rq.timeopt == SOCK_GET_TIMEPARAMS ||
			   pRQB->args.timeparam_rq.timeopt == SOCK_SET_TIMEPARAMS ||
			   pRQB->args.timeparam_rq.timeopt == SOCK_SET_TIMEPARAMS_CLOSE_TCP_CONN);

    Orginator = OHA_SOCK_RQB_GET_USERID_UVAR16(pRQB);
	OHA_ASSERT(OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_USER);

	/* check the handle */
	if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
	{
		OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
	}

	pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(Orginator)));
    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBUser->TcIpUserReq.pBottom); /* User-RCTRL-Blocks pending ? */

	OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->TcIpUserReq.pBottom,
							 pHDBUser->TcIpUserReq.pTop,
							 pRQBUser);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDBUser->Params.pDetail->Params.User.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_CNTRL_TIMEPARAMS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_SOCK_RQB_GET_RESPONSE(pRQB));

    switch (OHA_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
			if(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_TCIP_CONTROL)
			{
				OHA_UPPER_TCIP_DATA_PTR_TYPE pTcipData = &pRQBUser->args.TcipData;
				pTcipData->ConnectionTimeout = pRQB->args.timeparam_rq.connectiontimeout;
				pTcipData->KeepAliveTime     = pRQB->args.timeparam_rq.keepalivetime;
				pTcipData->RetransmitCount   = pRQB->args.timeparam_rq.retransmitcount;
				pTcipData->RetransmitTimeout = pRQB->args.timeparam_rq.retranstimeout;
			}
			else if (OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_PRM_READ) /* write the record */
			{
				OHA_UPPER_MEM_U8_PTR_TYPE record_data = pRQBUser->args.PrmData.record_data;
				LSA_UINT16  record_data_length;

				oha_prm_write_header(record_data, OHA_BLOCK_TYPE_TCP_KEEP_ALIVE_TIME,
									 OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*KeepAliveTime*/ + 2/*Paddings*/,
									 OHA_TOPO_DATA_BLOCK_VERSION, LSA_TRUE);
				record_data += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/);
				OHA_PDU_SET_U16 (record_data, (LSA_UINT16)(pRQB->args.timeparam_rq.keepalivetime/1000)); /* KeepAliveTime in ms */
				OHA_PUT16_HTON (record_data, 0, 0); /* 2 paddings */
				record_data_length = OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ + 2/*KeepAliveTime*/ + 2/*Paddings*/;

				pRQBUser->args.PrmData.record_data_length = record_data_length;
			}
            Response = OHA_OK;
            break;

        case SOCK_RSP_ERR_PARAM:
			if(OHA_RQB_GET_OPCODE(pRQBUser) == OHA_OPC_TCIP_CONTROL)
			{
				/* Set: At least one of value is not within the range allowed for this request. */
				/* If this error code is returned, the return values are set to 0 for the  */
				/* "in range" parameters, -1 is returned for the "out of range" parameter(s) */
				OHA_UPPER_TCIP_DATA_PTR_TYPE pTcipData = &pRQBUser->args.TcipData;
				pTcipData->ConnectionTimeout = pRQB->args.timeparam_rq.connectiontimeout;
				pTcipData->KeepAliveTime     = pRQB->args.timeparam_rq.keepalivetime;
				pTcipData->RetransmitCount   = pRQB->args.timeparam_rq.retransmitcount;
				pTcipData->RetransmitTimeout = pRQB->args.timeparam_rq.retranstimeout;
			}
            Response = OHA_ERR_PARAM;
            break;

        default:
            Response = OHA_ERR_LL;
            break;
    }

    if (Response != OHA_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDBUser->Params.pDetail->Params.User.NICId), LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_CNTRL_TIMEPARAMS failed.(0x%X).",OHA_SOCK_RQB_GET_RESPONSE(pRQB));
    }

    OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestControlTcipDataDone(Response: 0x%X)", Response);
}

#endif    /* OHA_CFG_NO_SOCK */

/*****************************************************************************/
/*  end of file OHA_SOCU.C                                                   */
/*****************************************************************************/

