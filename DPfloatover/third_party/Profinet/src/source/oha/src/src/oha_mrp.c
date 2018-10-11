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
/*  F i l e               &F: oha_mrp.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA MRP functions                                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  48
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_MRP */

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

#ifndef OHA_CFG_NO_MRP

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_mrp_request_lower_done                  +*/
/*+  Input                      OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for MRP-requests.                        +*/
/*+               Fills Error-Structure in NDB with MRP-Response            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_MRP_LOWER_IN_FCT_ATTR oha_mrp_request_lower_done(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_MRP_PTR_TYPE    pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN :oha_mrp_request_lower_done(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /* MRP-request always done from an MRP-Channel. !    */

    pHDB = OHAGetMrpHDBFromHandle(OHA_MRP_RQB_GET_HANDLE(pRQB));

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
    }

    switch (OHA_MRP_RQB_GET_OPCODE(pRQB) )
    {
        case MRP_OPC_OPEN_CHANNEL:
            OHA_MRPOpenChannelDone(pRQB,pHDB);
            break;
        case MRP_OPC_CLOSE_CHANNEL:
            OHA_MRPCloseChannelDone(pRQB,pHDB);
            break;
        case MRP_OPC_RINGSTATE_INDICATION:
            OHA_MRPRecvDone(pRQB,pHDB);
            break;
        case MRP_OPC_GET_STATUS:
            OHA_MRPGetStatusDone(pRQB,pHDB);
            break;
        case MRP_OPC_SET_STATUS:
            OHA_MRPSetStatusDone(pRQB,pHDB);
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_mrp_request_lower_done()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHAGetMrpHDBFromHandle                      +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_MRP_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA MRP-channel handle to get pointer to HDB for.         +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to MRP-HDB for Handle.                       +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_MRP_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetMrpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_MRP_PTR_TYPE pHDB;
    LSA_UINT16           Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAGetMrpHDBFromHandle(Handle: 0x%X)",
                          Handle);
    if ( OHA_HANDLE_IS_MRP(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get MRP-Idx */

        if ( Idx < OHA_CFG_MAX_NICS/*OHA_GET_GLOB_PTR()->LowerCnt*/ )
        {
            pHDB = OHA_GET_HMRP_PTR(Idx);
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid MRP handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid MRP handle (0x%X). No MRP handle.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAGetMrpHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPGetHandleByIndex                     +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_MRP_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceId      : Index from Table   1...n                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a MRP HDB with Index. If none is found    +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_MRPGetHandleByIndex(
    LSA_UINT32 Index)
{
    OHA_HDB_MRP_PTR_TYPE pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPGetHandleByIndex(Index: 0x%X)",
                          Index);
    if (Index && (Index <= OHA_CFG_MAX_NICS))    /* 0 is not valid */
    {
        pHDB = OHA_GET_HMRP_PTR(Index-1);
        if ( OHA_IS_NOT_NULL(pHDB) && (pHDB->State == OHA_HANDLE_STATE_OPEN) )
        {
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetHandleByIndex(pHDB:  0x%X)", pHDB);
            return(pHDB);
        }
        else
        {
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetHandleByIndex(pHDB:  0x%X)", pHDB);
            return(LSA_NULL);
        }
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetHandleByIndex()");
    return(LSA_NULL);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPGetHandleByIf                        +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_MRP_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceId      : InterfaceId from Table   1...n                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a MRP HDB with IfId. If none is found     +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_MRPGetHandleByIf(
    LSA_UINT32 IfId)
{
    OHA_HDB_MRP_PTR_TYPE pHDB = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPGetHandleByIf(IfId: 0x%X)",
                          IfId);

    if (IfId >= EDD_INTERFACE_ID_MIN && IfId <= EDD_INTERFACE_ID_MAX) /* 0 is not valid */
    {
		LSA_UINT16 NicID = OHA_DB_GET_NIC(IfId);

		if(NicID != OHA_INVALID_NIC_ID)
		{
		    pHDB = OHA_MRPGetHandleByNIC(NicID);
			if(OHA_IS_NULL(pHDB))
			{
				OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetHandleByIf(pHDB: 0x%X, NicID: 0x%X)", pHDB, NicID);
			}
		}
	}

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetHandleByIf(pHDB:  0x%X)", pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPCheckNICUsed                         +*/
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
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_MRPCheckNICUsed(
    LSA_UINT16 NICId)
{
    OHA_HDB_MRP_PTR_TYPE  pHDB  =  OHA_GET_HMRP_PTR(0);
    LSA_UINT32            i     =   0;
    LSA_BOOL              Found =   LSA_FALSE;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPCheckNICUsed(NICId: 0x%X)",
                          NICId);
    if (NICId != OHA_INVALID_NIC_ID)
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS ) )
        {
            if (( pHDB->State != OHA_HANDLE_STATE_FREE ) &&
                ( pHDB->Params.pDetail->Params.Mrp.NICId == NICId  ))
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

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPCheckNICUsed(Found:  0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPGetRingPortState                     +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+  State      : mrpDomainRingPortState,  SYNTAX INTEGER {                 +*/
/*+                             	       disabled (1),                    +*/
/*+                             		   blocked (2),                     +*/
/*+                             		   forwarding (3),                  +*/
/*+                             		   not-connected (4)                +*/
 /*+                             		   }                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Returns mrpDomainRingPortState                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPGetRingPortState(
    LSA_UINT16 NICId, 
	LSA_UINT16 RingPortId)
{
	OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
    LSA_UINT16 RingPortState = 0;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPCheckNICUsed(NICId: 0x%X, RingPortId: 0x%X)",
                          NICId, RingPortId);

	if(OHA_IS_NULL(pHDBEdd))
	{
		/* Error on getting handle. Abort. */
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"Request OHA_MRPGetRingPortState failed. No EDD Channel for this NicId (NIC-ID: 0x%X)", NICId);
	    return(RingPortState);
	}

    if (RingPortId && (RingPortId <= OHA_CFG_MAX_PORTS))
	{
		if(pHDBEdd->LinkStatus[RingPortId-1].LinkStatus == EDD_LINK_DOWN)
		{
            RingPortState = MRP_MIB_RPORT_STATE_NOT_CONNECTED;	/* not-connected */
		}
		else
		{
			switch(pHDBEdd->LinkStatus[RingPortId-1].MRP_PortState)
			{
			case EDD_MRP_PORTSTATE_OFF:
	            RingPortState = 1;	/* disabled */
				break;
			case EDD_MRP_PORTSTATE_ACTIVATED:
	            RingPortState = 3;	/* forwarding */
				break;
			default:
	            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_MRPGetRingPortState: MRP_PortStatus 0x%x failed.", pHDBEdd->LinkStatus[RingPortId-1].MRP_PortState);
				break;
			}
		}
	}
     
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetRingPortState(RingPortState:  0x%X)", RingPortState);
    return(RingPortState);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPGetFreeHDB                           +*/
/*+  Input/Output          :    OHA_HDB_MRP_PTR_TYPE OHA_LOCAL..pHDBMrp    +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA MRP  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free MRP -HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPGetFreeHDB(
    OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBMrp)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found = LSA_FALSE;
    OHA_HDB_MRP_PTR_TYPE      pHDB  = OHA_GET_HMRP_PTR(0);
    LSA_UINT32                i     = 0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPGetFreeHDB(pHDB: 0x%X)",
                          pHDBMrp);

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
        *pHDBMrp    = pHDB;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_MRPGetFreeHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBMrp);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPReleaseHDB                           +*/
/*+  Input/Output          :    OHA_HDB_MRP_PTR_TYPE pHDB                   +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBMrp     : Pointer to User Handle to release                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release MRP -HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPReleaseHDB(
    OHA_HDB_MRP_PTR_TYPE pHDB)
{
    LSA_UINT16 RetVal = LSA_RET_OK;
	LSA_UINT16 Instance;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

	for(Instance = 0; Instance < EDD_CFG_MAX_MRP_INSTANCE_CNT; Instance++)
	{
		if(OHA_IS_NOT_NULL(pHDB->MrpGetStatus[Instance].pDomainName))
		{
			OHA_FREE_LOCAL_MEM(&RetVal,pHDB->MrpGetStatus[Instance].pDomainName);
			OHA_MEM_FREE_FAIL_CHECK(RetVal);

			pHDB->MrpGetStatus[Instance].pDomainName = LSA_NULL;
		}
	}

    pHDB->State = OHA_HANDLE_STATE_FREE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPReleaseHDB()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPChannelDone                          +*/
/*+  Input                      OHA_HDB_MRP_PTR_TYPE         pHDB           +*/
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
/*+               MRP-Channels. This function is called after a lower       +*/
/*+               layer called back the OHA (e.g. after MRP finished his    +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_MRPChannelDone(
    OHA_HDB_MRP_PTR_TYPE       pHDB,
    LSA_UINT16                 Response)
{

    OHA_UPPER_RQB_PTR_TYPE       pRQB;
    LSA_UINT16                   PathStat = LSA_RET_OK;
    LSA_BOOL                     Finish;
    LSA_BOOL                     Release;
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE             pSys;
	OHA_DETAIL_PTR_TYPE          pDetail;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB,Response);

    Finish = LSA_FALSE; /* dont finish request by default */
    Release = LSA_FALSE; /* dont release PATH-Info by default */

    OHA_ASSERT_NULL_PTR(pHDB);

    pRQB         = pHDB->pOCReq;
    pHDB->pOCReq = LSA_NULL;    /* we finish all requests here. so reset pOCReq */
    /* to signal no more request pending.           */

    Cbf     = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
	pSys    = pHDB->Params.pSys;
	pDetail = pHDB->Params.pDetail;

    OHA_ASSERT_NULL_PTR(pRQB);

    OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_MRPChannelDone done (NICId: 0x%X,Rsp: 0x%X,State: 0x%X)",pHDB->Params.pDetail->Params.Mrp.NICId,Response,pHDB->State);

    switch (pHDB->State)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a MRP-Open Channel request.                         */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_MRP_RUNNING:
            if ( Response == OHA_OK)
            {
                /* NOTE: Providing Recv-Indication Request must be          */
                /*       the last action before going into State "open".    */
                /*       So only Indciation while in Open-state must be     */
                /*       handled and reprovided!                            */

                Response = OHA_MRPRecvProvide(pHDB);

                if ( Response == OHA_OK )
                {
                    /* Channel is open now, no more actions needed. */
	                pHDB->State  = OHA_HANDLE_STATE_OPEN;
				}
				else
				{
                    /* Any Recv-Request failed. Cancel Recv-Requests (by CloseChannel) */
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP Receive Provide request failed. Response: 0x%X", Response);

                    pHDB->OpenResponse = Response; /* save for later */

                    pHDB->State = OHA_HANDLE_STATE_CLOSE_ERROR_MRP_RUNNING;

                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"MRP-Closing Channel because of error");

                    if ( OHA_MRPCloseChannel(pHDB->MrpHandle,pHDB->Params.pSys) != OHA_OK)
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_FATAL,"Cannot Close MRP Channel().");
                        OHA_FATAL(OHA_FATAL_ERR_MRP);
                    }

				}
            }
            else
            {
                /* on error, we copy the LL-Error to the RQB    */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open MRP. Response: 0x%X", Response);

                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }

            Finish = LSA_TRUE;

            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a MRP-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_MRP_RUNNING:

            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot close MRP. Response: 0x%X", Response);
                /* is this a fatal error ?                      */
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
         OHA_MRPReleaseHDB(pHDB);
	}

    if ( Finish )  /* finish the request ? */
    {
        OHA_RQBSetResponse(pRQB,Response);
        /* upper - handler already set */
        OHA_UPPER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Response);
        OHA_CallCbf(Cbf,pRQB,pSys);
    }

    if ( Release )  /* release the PATH-Info ? */
    {
         OHA_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);

         if (PathStat != LSA_RET_OK ) OHA_FATAL(OHA_FATAL_ERR_RELEASE_PATH_INFO);
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPOpenChannel                          +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA MRP-Channel handle                           +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to MRP. The Request will be   +*/
/*+               finished by calling the MRP-Callbackfunction.             +*/
/*+               oha_mrp_request_lower_done() if OHA_OK.                   +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB  = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);

    UserId.uvar32 = 0;
    OHA_MRP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(MRP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_MRP_RQB_SET_OPCODE(pRQB,MRP_OPC_OPEN_CHANNEL);
		OHA_MRP_RQB_SET_HANDLE(pRQB,OHAHandle);

        pRQB->args.channel.open.handle_upper = OHAHandle;
        pRQB->args.channel.open.sys_path     = SysPath;
        pRQB->args.channel.open.pCbf         = oha_mrp_request_lower_done;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: MRP-Request: MRP_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
        OHA_MRP_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"MRP-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPOpenChannel. Result: 0x%X",
                          Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPOpenChannelDone                      +*/
/*+  Input                      OHA_MRP_LOWER_RQB_PTR_TYPE   pRQB           +*/
/*+                             OHA_HDB_MRP_PTR_TYPE         pHDB           +*/
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
/*+               - save MRP-Channel                                        +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPOpenChannelDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB)
{
    LSA_UINT16          Response = OHA_OK;
    LSA_UINT16          RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: MRP_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_MRP_RQB_GET_RESPONSE(pRQB));

    if ( OHA_MRP_RQB_GET_RESPONSE(pRQB) == MRP_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE_LOW,"MRP-Open Channel successful");
        /* save MRP-Channel in HDB */
        pHDB->MrpHandle = pRQB->args.channel.open.handle;
    }
    else
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-Open Channel failed.[0x%X]",OHA_MRP_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_MRP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_MRPChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPOpenChannelDone(Response: 0x%X)", Response);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPCloseChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       MRPHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  MRPHandle            : MRP handle to close                             +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to MRP. The Request will be  +*/
/*+               finished by calling the MRP-Callbackfunction.             +*/
/*+               oha_mrp_request_lower_done()                              +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPCloseChannel(
    LSA_HANDLE_TYPE       MRPHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB  = LSA_NULL;
    LSA_UINT16                  Result = OHA_OK;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPCloseChannel(MRPHandle: 0x%X)",
                          MRPHandle);

    UserId.uvar32 = 0;
    OHA_MRP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(MRP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {

        OHA_MRP_RQB_SET_OPCODE(pRQB,MRP_OPC_CLOSE_CHANNEL);
        OHA_MRP_RQB_SET_HANDLE(pRQB,MRPHandle);

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: MRP-Request: MRP_OPC_CLOSE_CHANNEL (MRPHandle: 0x%X,pRQB: 0x%X)",MRPHandle,pRQB);
        OHA_MRP_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"MRP-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPCloseChannel(Result: 0x%X)", Result);
    return(Result);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPCloseChannelDone                     +*/
/*+  Input                      OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                             OHA_HDB_MRP_PTR_TYPE        pHDB            +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPCloseChannelDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response = OHA_OK;
    LSA_UINT16               RetVal   = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: MRP_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_MRP_RQB_GET_RESPONSE(pRQB));

    if ( OHA_MRP_RQB_GET_RESPONSE(pRQB) == MRP_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"MRP-Close Channel successful");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"MRP-Close Channel failed.[0x%X]",OHA_MRP_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    /* Could all allocated Receive-RQBs be freed ? */
    OHA_ASSERT(pHDB->RxPendingCnt == 0);

    OHA_MRP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_MRPChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPCloseChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPRecvProvide                          +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                             LSA_UINT32                   Cnt            +*/
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
/*+  Description: Sends Indication-Resources to MRP                         +*/
/*+               (MRP_OPC_RINGSTATE_INDICATION)                            +*/
/*+                                                                         +*/
/*+               Allocates RQB and does MRP-Requests                       +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe RECV-Request pending     +*/
/*+                     within MRP!                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPRecvProvide(
    OHA_HDB_MRP_PTR_TYPE    pHDB)
{
    OHA_MRP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE            pSys;
    LSA_UINT16                  Status, Instance;
    LSA_USER_ID_TYPE            UserId;
    OHA_DB_IF_PTR_TYPE          pDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPRecvProvide(pHDB: 0x%X)", pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

	pDB = OHA_DB_GET_INTERFACE(pHDB->Params.pDetail->Params.Mrp.NICId);
    OHA_ASSERT_NULL_PTR(pDB);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Receive-Request to MRP                                  */
    /*---------------------------------------------------------------------------*/

    pHDB->RxPendingCnt = 0; /* no Rx pending within MRP */

	for(Instance = 0; (Instance < pDB->MaxMRP_Instances) && (Instance < EDD_CFG_MAX_MRP_INSTANCE_CNT); Instance++)
	{
		/* allocate RQB */
		OHA_MRP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(MRP_RQB_TYPE),pSys);

		if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
		{
			MRP_USR_STATE_INDICATION_PTR_TYPE pMrpInd = &pRQB->args.state.state_ind;

			OHA_MRP_RQB_SET_OPCODE(pRQB,MRP_OPC_RINGSTATE_INDICATION);
			OHA_MRP_RQB_SET_HANDLE(pRQB,pHDB->MrpHandle);

			OHA_MEMSET(pMrpInd, 0, sizeof(MRP_STATE_INDICATION_TYPE));
			pMrpInd->ohaStateInd.mrpInstance = (LSA_UINT16)(Instance+1);	/* 1 ... EDD_CFG_MAX_MRP_INSTANCE_CNT */

			OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: MRP-Request: MRP_OPC_RINGSTATE_INDICATION (Handle: 0x%X,MRPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->MrpHandle,pRQB);

			OHA_MRP_REQUEST_LOWER(pRQB,pSys);
			pHDB->RxPendingCnt++;  /* number of Rx-Request pending within MRP */
		}
		else
		{
			Status = OHA_ERR_RESOURCE;
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-Allocating lower RQB-memory failed!");
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPRecvProvide(Status: 0x%X)",Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPRecvDone                             +*/
/*+  Input                      OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                             OHA_MRP_HDB_PTR_TYPE        pHDB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: MRP Recv-Request done (Indication received)               +*/
/*+                                                                         +*/
/*+               Copy received infos and content to the DataBase           +*/
/*+               within global management for further handling.            +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides Receveive-Request to MRP                       +*/
/*+                                                                         +*/
/*+               If Recv-Request was canceled we free the RQB/Frame and    +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPRecvDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB)
{
    LSA_UINT16                        RetVal = LSA_RET_OK;
    LSA_BOOL                          Reprovide = LSA_TRUE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPRecvDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: MRP_OPC_RINGSTATE_INDICATION done (Handle: 0x%X,Rsp: 0x%X,State: 0x%X)",
                       pHDB->ThisHandle,OHA_MRP_RQB_GET_RESPONSE(pRQB),pHDB->State);

    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the status and dont reprovide it. This is the case if we are   */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid frame while closing, so CANCEL   */
    /*       may not be enough to catch all rqbs. We have to check the State too */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Recv-Request will be provided in the last */
    /*       State while opening the channel.                                    */

    if ( pHDB->State == OHA_HANDLE_STATE_OPEN )
    {
        switch (OHA_MRP_RQB_GET_RESPONSE(pRQB))
        {
            case MRP_RSP_OK:
            {
                /* ---------------------------------------------*/
                /* Successfully done the request.               */
                /* ---------------------------------------------*/
				OHA_MrpWriteData(pRQB, pHDB);
            }
            break;

            case MRP_RSP_OK_CANCEL:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> OHA_HANDLE_STATE_OPEN  */
                /*       while closing!                             */

                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"MRP Receive canceled (0x%X).",OHA_MRP_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            default:
				Reprovide = LSA_FALSE;
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_FATAL,"MRP-ERROR: MRP_OPC_RINGSTATE_INDICATION failed.(0x%X).",OHA_MRP_RQB_GET_RESPONSE(pRQB));
                OHA_FATAL(OHA_FATAL_ERR_MRP);
                break;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"MRP Receive while not in open-state. discard and dont reprovide.");
        Reprovide = LSA_FALSE;
    }

    if ( Reprovide )
    {
        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: MRP-Request: MRP_OPC_RINGSTATE_INDICATION (Handle: 0x%X,MRPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->MrpHandle,pRQB);
        OHA_MRP_RQB_SET_HANDLE(pRQB,pHDB->MrpHandle); /* set handle */
        OHA_MRP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
    }
    else
    {
        pHDB->RxPendingCnt--;  /* number of Recv-Request pending within MRP */

        OHA_MRP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPRecvDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MrpWriteData                            +*/
/*+  Input/Output          :    MRP_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    MRP_HDB_PTR_TYPE           pHDB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to MrpInd-RQB with RingPortStatus-parameters      +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE        opcode:      MRP_OPC_RINGSTATE_INDICATION    +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response                                     +*/
/*+     MRP_RQB_ARGS_TYPE      args.state.state_ind                         +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write the expected MRP neighbour-data          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_MrpWriteData(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
	MRP_USR_STATE_INDICATION_PTR_TYPE pMrpInd;
	LSA_UINT16              Instance;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MrpWriteData(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

	OHA_ASSERT_NULL_PTR(pRQB);
	OHA_ASSERT_NULL_PTR(pHDB);
    pMrpInd = &pRQB->args.state.state_ind;
    OHA_ASSERT_NULL_PTR(pMrpInd);
	Instance = pMrpInd->ohaStateInd.mrpInstance;
	OHA_ASSERT(Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT);

    /* Check if we have LLDP and a EDD-Channel for NIC-ID. */
	NICId    = pHDB->Params.pDetail->Params.Mrp.NICId;
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);  /* only EDD and LLDP for this request necessary */

	if ( OHA_IS_NULL(pHDBLldp) || OHA_IS_NULL(pHDBLldp->pDB))
    {
        Response = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request MRP_OPC_RINGSTATE_INDICATION failed. No LLDP-Channel or DataBase present (NIC-ID: 0x%X)",NICId);
    }
    else if (pMrpInd->ohaStateInd.rPort1 > OHA_CFG_MAX_PORTS)  /* pHDB->PortCnt */
    {
        Response = OHA_ERR_PARAM;  /* Portid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request MRP_OPC_RINGSTATE_INDICATION failed. rPortId1 is wrong (PORT-ID1: 0x%X)",pMrpInd->ohaStateInd.rPort1);
    }
    else if (pMrpInd->ohaStateInd.rPort2 > OHA_CFG_MAX_PORTS)  /* pHDB->PortCnt */
    {
        Response = OHA_ERR_PARAM;  /* Portid is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request MRP_OPC_RINGSTATE_INDICATION failed. rPortId2 is wrong (PORT-ID2: 0x%X)",pMrpInd->ohaStateInd.rPort2);
    }
    else
    {
        LSA_UINT16 RingPortID_1 = pMrpInd->ohaStateInd.rPort1;
        LSA_UINT16 RingPortID_2 = pMrpInd->ohaStateInd.rPort2;
        LSA_BOOL MRP_RingPortDifference_1;
        LSA_BOOL MRP_RingPortDifference_2;

		if(RingPortID_1 == 0 || RingPortID_2 == 0)	/* both must be 0 */
		{
			if(RingPortID_1 != RingPortID_2)
			{
		        Response = OHA_ERR_PARAM;  /* Portid is wrong */
                OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"MRP-Write (Add,Check) failed (CtrlMode: 0x%X, RingPort1: 0x%X, RingPort2: 0x%X)", 
									 pMrpInd->ohaStateInd.topoCtrlMode, RingPortID_1, RingPortID_2);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MrpWriteData(Response: 0x%X)", Response);
				return;
			}
		}

        /* check the MRP_PortStatus */
        if (!OHA_CheckMRP_PortStatus(pMrpInd->ohaStateInd.mrpStatus))  /* is there a valid MRP_RingPortStatus ? */
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"MRP_OPC_RINGSTATE_INDICATION(Add): MRP_RingPortStatus(0x%X) failed.", pMrpInd->ohaStateInd.mrpStatus);
            Response = LSA_RET_ERR_PARAM;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MrpWriteData(Response: 0x%X)", Response);
            return;
        }

        switch (pMrpInd->ohaStateInd.topoCtrlMode)
        {
			case MRP_USR_ENUM_TOPO_CTRL_ADD:       /* former OHA_MRP_ADD: add (expected) */
			case MRP_USR_ENUM_TOPO_CTRL_ADD_CHECK:  /* former OHA_MRP_ADD_CHECK: add (expected) and build diagnosis */
            {
                /**********************************************************/
                /* first the check (MRT_PortStatus, DomainUUID, ...)      */
                /**********************************************************/

                /* check the MRP_DomainUUID */
                if (!OHA_CheckDomainUUID(pMrpInd->ohaStateInd.domainUUID))  /* is there a valid UUID ? */
                {
                    /* is there a port status <> Null ? */
                    if (pMrpInd->ohaStateInd.mrpStatus == MRP_OPERATION_OFF)
                    {
                        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"MRP_OPC_RINGSTATE_INDICATION-Write: DomainUUID failed.");
                        Response = LSA_RET_ERR_PARAM;
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MrpWriteData(Response: 0x%X)", Response);
                        return;
                    }
                }

				/* write the (last) MRP status (topoCtrlMode, DomainUUID, MRP_PortStatus,...) to the TopoDB */
				pHDB->MrpStateInd[Instance-1] = pMrpInd->ohaStateInd;

                OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"MRP-Write (Add,Check) successful (CtrlMode: 0x%X, RingPort1: 0x%X, RingPort2: 0x%X)", 
									 pMrpInd->ohaStateInd.topoCtrlMode, RingPortID_1, RingPortID_2);

				if(RingPortID_1 == 0 || RingPortID_2 == 0)
				{
					break;
				}

				pHDBLldp->pDB->LldpData[RingPortID_1-1].MrpInstance = Instance;
				pHDBLldp->pDB->LldpData[RingPortID_2-1].MrpInstance = Instance;

                /* determines, if is the LLDP-TLV LLDP_PNIO_MRPPORTSTATUS is to send */
                MRP_RingPortDifference_1 = oha_update_mrp_port_status(pHDB, NICId, RingPortID_1);
                MRP_RingPortDifference_2 = oha_update_mrp_port_status(pHDB, NICId, RingPortID_2);

                if (MRP_RingPortDifference_1) /* update LLDP data by LLDPSet() */
                {
                    if (pHDBLldp->RequestState == OHA_LLDP_STATE_START || 
						pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START ||  /* AP01070629, AP01137963 */
						pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START) /* LLDP-Set is possible ? */
                    {
                        LSA_UINT16 Status = OHA_LLDPSet(pHDBLldp, RingPortID_1, OHA_HANDLE_TYPE_SYS); /* send them */
                        if (Status != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_MrpWriteData: OHA_LLDPSet(RingPortID_1) failed (Status: 0x%X).", Status);
                        }
                    }
                }
                if (MRP_RingPortDifference_2) /* update LLDP data by LLDPSet() */
                {
                    if (pHDBLldp->RequestState == OHA_LLDP_STATE_START || 
						pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START ||  /* AP01070629, AP01137963 */
						pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START) /* LLDP-Set is possible ? */
                    {
                        LSA_UINT16 Status = OHA_LLDPSet(pHDBLldp, RingPortID_2, OHA_HANDLE_TYPE_SYS); /* send them */
                        if (Status != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_MrpWriteData: OHA_LLDPSet(RingPortID_2) failed (Status: 0x%X).", Status);
                        }
                    }
                }

                /* check the TOPO and store the indication in the USER-HDB as LastTopoWriteInd */
                /* Handle the MRP Status Change for these ports */
                OHA_IndicateEventAllUsers(NICId, RingPortID_1);
                OHA_IndicateEventAllUsers(NICId, RingPortID_2);
            }
            break;

            case MRP_USR_ENUM_TOPO_CTRL_NONE: /* former OHA_MRP_REMOVE: remove all peers of this port */
            {
				/* write the (last) MRP status to the TopoDB */
				pHDB->MrpStateInd[Instance-1] = pMrpInd->ohaStateInd;

                OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"MRP-Write (Delete) successful (RingPort1: 0x%X, RingPort2: 0x%X)", RingPortID_1, RingPortID_2);

				if(RingPortID_1 == 0 || RingPortID_2 == 0)
				{
					break;
				}

				pHDBLldp->pDB->LldpData[RingPortID_1-1].MrpInstance = Instance;
				pHDBLldp->pDB->LldpData[RingPortID_2-1].MrpInstance = Instance;

                /* determines, if is the LLDP-TLV LLDP_PNIO_MRPPORTSTATUS is to send */
                MRP_RingPortDifference_1 = oha_update_mrp_port_status(pHDB, NICId, RingPortID_1);
                MRP_RingPortDifference_2 = oha_update_mrp_port_status(pHDB, NICId, RingPortID_2);

                if (MRP_RingPortDifference_1) /* update LLDP data by LLDPSet() */
                {
                    if (pHDBLldp->RequestState == OHA_LLDP_STATE_START || pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START) /* LLDP-Set is possible ? */
                    {
                        LSA_UINT16 Status = OHA_LLDPSet(pHDBLldp, RingPortID_1, OHA_HANDLE_TYPE_SYS); /* send them */
                        if (Status != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_MrpWriteData: OHA_LLDPSet(RingPortID_1) failed (Status: 0x%X).", Status);
                        }
                    }
                }
                if (MRP_RingPortDifference_2) /* update LLDP data by LLDPSet() */
                {
                    if (pHDBLldp->RequestState == OHA_LLDP_STATE_START || pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START) /* LLDP-Set is possible ? */
                    {
                        LSA_UINT16 Status = OHA_LLDPSet(pHDBLldp, RingPortID_2, OHA_HANDLE_TYPE_SYS); /* send them */
                        if (Status != LSA_RET_OK)
                        {
                            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_MrpWriteData: OHA_LLDPSet(RingPortID_2) failed (Status: 0x%X).", Status);
                        }
                    }
                }

                /* check the TOPO and store the indication in the USER-HDB as LastTopoWriteInd */
                /* Handle the MRP Status Change for this port */
                OHA_IndicateEventAllUsers(NICId, RingPortID_1);
                OHA_IndicateEventAllUsers(NICId, RingPortID_2);
            }
            break;
            default:
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-Write failed.[topoCtrlMode: 0x%X]",pMrpInd->ohaStateInd.topoCtrlMode);
                Response = OHA_ERR_SYNTAX;
            break;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MrpWriteData(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPGetStatus                            +*/
/*+  Input/Output          :    LSA_UINT16                  NicID           +*/
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
/*+  Description: Get status of MRP                                         +*/
/*+               (MRP_OPC_GET_STATUS)                                      +*/
/*+                                                                         +*/
/*+               Allocates RQB and does the MRP-Request                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPGetStatus(
    OHA_HDB_MRP_PTR_TYPE        pHDB,
    LSA_UINT16                  Orginator,
    LSA_UINT16                  Instance)
{
    OHA_MRP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE           pSys;
    LSA_UINT16                 Status;
    LSA_USER_ID_TYPE           UserId;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPGetStatus(pHDB: 0x%X, Orginator: 0x%X, Instance: 0x%X)", pHDB, Orginator, Instance);

    OHA_ASSERT_NULL_PTR(pHDB);
	OHA_ASSERT(Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT);

    /* Check if we have a MRP-Channel for this NIC-ID.*/
    if (OHA_IS_NULL(pHDB))
    {
        Status = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"Request MRP_OPC_GET_STATUS failed. No MRP Channel!");
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetStatus(Status: 0x%X)",Status);
		return(Status);
    }

	/* is an open/close MRP-channel runnning ? */
    if (!(pHDB->State == OHA_HANDLE_STATE_OPEN) )
    {
        Status = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"Request MRP_OPC_GET_STATUS failed. MRP Channel is closing or not open (State: 0x%X)", pHDB->State);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetStatus(Status: 0x%X)",Status);
		return(Status);
    }

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Request to MRP                                          */
    /*---------------------------------------------------------------------------*/

    OHA_MRP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(MRP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
		OHA_MRP_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL;

		MRP_GET_STATUS_PTR_TYPE pMrpGet = &pRQB->args.ctrl.get_status;

		OHA_MRP_RQB_SET_OPCODE(pRQB,MRP_OPC_GET_STATUS);
        OHA_MRP_RQB_SET_HANDLE(pRQB,pHDB->MrpHandle);
        OHA_MRP_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_MEMSET(pMrpGet, 0, sizeof(MRP_GET_STATUS_TYPE));
		pMrpGet->mrpInstance = Instance;

        /* alloc the buffer and attach them to the RQB */
		OHA_MRP_ALLOC_LOWER_MEM(&MemPtr, UserId, MRP_MIB_DEFAULT_DOMAIN_NAME_SIZE_MAX, pSys);

		if(!LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
		{
			pMrpGet->pDomainName = (MRP_COMMON_MEM_U8_PTR_TYPE)MemPtr;

	        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: MRP-Request: MRP_OPC_GET_STATUS (Handle: 0x%X,MRPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->MrpHandle,pRQB);

			OHA_MRP_REQUEST_LOWER(pRQB,pSys);
			pHDB->MrpRQBCnt++;  /* number of Requests pending within MRP */
		}
		else
		{
			LSA_UINT16 RetVal = LSA_RET_OK;
			OHA_MRP_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
			OHA_MEM_FREE_FAIL_CHECK(RetVal);
			Status = OHA_ERR_RESOURCE;
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-Allocating lower memory failed!");
		}
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPGetStatus(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPGetStatusDone                        +*/
/*+  Input                      OHA_MRP_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Get MRP status confirmation                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPGetStatusDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              Orginator, Instance;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPGetStatusDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

	OHA_ASSERT_NULL_PTR(pRQB);
	OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"<<<: MRP_OPC_GET_STATUS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_MRP_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_LLDP_RQB_GET_USERID_UVAR16(pRQB);
	Instance  = pRQB->args.ctrl.get_status.mrpInstance;
	OHA_ASSERT(Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT);

    switch (OHA_MRP_RQB_GET_RESPONSE(pRQB))
    {
        case MRP_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
			OHA_ASSERT(pRQB->args.ctrl.get_status.domainNameLength <= MRP_MIB_DEFAULT_DOMAIN_NAME_SIZE_MAX);
			OHA_ASSERT_NULL_PTR(pRQB->args.ctrl.get_status.pDomainName);

			/* need a new buffer for the DomainName */
			if(OHA_IS_NOT_NULL(pHDB->MrpGetStatus[Instance-1].pDomainName))
			{
				OHA_FREE_LOCAL_MEM(&RetVal,pHDB->MrpGetStatus[Instance-1].pDomainName);
				OHA_MEM_FREE_FAIL_CHECK(RetVal);

				pHDB->MrpGetStatus[Instance-1].pDomainName = LSA_NULL;
			}

			{
			    OHA_LOCAL_MEM_PTR_TYPE MemPtr = LSA_NULL;

				/* Size+1 for null termination (the MIB has no object for the length) */
			    OHA_ALLOC_LOCAL_MEM(&MemPtr, pRQB->args.ctrl.get_status.domainNameLength+1);
			    if (! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
				{
			        pHDB->MrpGetStatus[Instance-1].pDomainName = (MRP_COMMON_MEM_U8_PTR_TYPE)MemPtr;
				}
				else
				{
			        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"Allocating local memory for MrpGet failed!");
					Response = OHA_ERR_RESOURCE;
		            break;
				}
			}

			pHDB->MrpGetStatus[Instance-1].domainNameLength = pRQB->args.ctrl.get_status.domainNameLength;
			OHA_MEMCPY(pHDB->MrpGetStatus[Instance-1].pDomainName,
						pRQB->args.ctrl.get_status.pDomainName,
						pRQB->args.ctrl.get_status.domainNameLength);
			/* null termination (the MIB has no object for the length) */
			pHDB->MrpGetStatus[Instance-1].pDomainName[pHDB->MrpGetStatus[Instance-1].domainNameLength] = 0;
			pHDB->MrpGetStatus[Instance-1].operRole         = pRQB->args.ctrl.get_status.operRole;
			pHDB->MrpGetStatus[Instance-1].rPort1           = pRQB->args.ctrl.get_status.rPort1;
			pHDB->MrpGetStatus[Instance-1].rPort2           = pRQB->args.ctrl.get_status.rPort2;
			pHDB->MrpGetStatus[Instance-1].rPort1State      = pRQB->args.ctrl.get_status.rPort1State;
			pHDB->MrpGetStatus[Instance-1].rPort2State      = pRQB->args.ctrl.get_status.rPort2State;
			pHDB->MrpGetStatus[Instance-1].domainState      = pRQB->args.ctrl.get_status.domainState;
			pHDB->MrpGetStatus[Instance-1].domainError      = pRQB->args.ctrl.get_status.domainError;
			pHDB->MrpGetStatus[Instance-1].domainRingOpenCount = pRQB->args.ctrl.get_status.domainRingOpenCount;
			pHDB->MrpGetStatus[Instance-1].domainLastRingOpenChange = pRQB->args.ctrl.get_status.domainLastRingOpenChange;
			pHDB->MrpGetStatus[Instance-1].domainRoundTripDelayMax = pRQB->args.ctrl.get_status.domainRoundTripDelayMax;
			pHDB->MrpGetStatus[Instance-1].domainRoundTripDelayMin = pRQB->args.ctrl.get_status.domainRoundTripDelayMin;

			Response = OHA_OK;
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-ERROR: MRP_OPC_GET_STATUS failed.(0x%X).",OHA_MRP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

	OHA_MRP_FREE_LOWER_MEM(&RetVal, pRQB->args.ctrl.get_status.pDomainName, pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_MRP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    pHDB->MrpRQBCnt--;

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_USER:  /* from OpcReadOid */
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
        case OHA_HANDLE_TYPE_SOCK:  /* from cable */
        {
			LSA_HANDLE_TYPE			Handle	= (LSA_HANDLE_TYPE)Orginator;
			OHA_HDB_SOCK_PTR_TYPE	pHDBSock = OHAGetSockHDBFromHandle(Handle); /* not OHA_SOCKGetHandleByNIC()! */

            if (OHA_IS_NOT_NULL(pHDBSock))  /* not closed */
            {
				OHA_HDB_AGNT_PTR_TYPE pSubAgnt = &pHDBSock->MrpAgent;

                if (pSubAgnt->StatsPendCnt == 0)    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

                pSubAgnt->StatsPendCnt--;   /* decrement the active requests */

                if (pSubAgnt->StatsPendCnt == 0)    /* got all necessary statistics ? */
                {
					if (pSubAgnt->RequestState == OHA_SNMP_STATE_WF_STATISTICS)
					{
						OHA_SNMPSockCheckPending(pHDBSock, OHA_MRP_MIB_NODE, Response); /* further actions taken in request handler */
						pSubAgnt->RequestState = OHA_SNMP_STATE_START;
					}
					else
					{
						OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE, "No active state anymore, late LldpGetStats.Con, state %Xh", pSubAgnt->RequestState);
					}
                }
            }
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetStatusDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPSetStatus                            +*/
/*+  Input/Output          :    LSA_UINT16                  NicID           +*/
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
/*+  Description: Set status of MRP                                         +*/
/*+               (MRP_OPC_SET_STATUS)                                      +*/
/*+                                                                         +*/
/*+               Allocates RQB and does the MRP-Request                    +*/
/*+                                                                         +*/
/*+  Note:        "ResetRoundTripDelays" only                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPSetStatus(
    OHA_HDB_MRP_PTR_TYPE        pHDB,
    LSA_UINT16                  Orginator,
    LSA_UINT16                  Instance
)
{
    OHA_MRP_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE           pSys;
    LSA_UINT16                 Status;
    LSA_USER_ID_TYPE           UserId;
	LSA_UINT32                 ResetDelays;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPSetStatus(pHDB: 0x%X, Orginator: 0x%X, Instance: 0x%X)", pHDB, Orginator, Instance);

    OHA_ASSERT_NULL_PTR(pHDB);
	OHA_ASSERT(Instance >= 1 && Instance <= EDD_CFG_MAX_MRP_INSTANCE_CNT);

	/* is an open/close MRP-channel runnning ? */
    if (!(pHDB->State == OHA_HANDLE_STATE_OPEN) )
    {
        Status = OHA_ERR_SEQUENCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,"Request MRP_OPC_SET_STATUS failed. MRP Channel is closing or not open (pHDB: 0x%X)", pHDB);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPSetStatus(Status: 0x%X)",Status);
		return(Status);
    }

	/* check the param(s) */
	ResetDelays = pHDB->MrpSetStatus[Instance-1].domainResetRoundTripDelays;

    if (ResetDelays != 1)
    {
        Status = OHA_ERR_PARAM;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"Request MRP_OPC_SET_STATUS failed. Invalid params (ResetDelays: 0x%X)", ResetDelays);
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPSetStatus(Status: 0x%X)",Status);
		return(Status);
    }

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Request to MRP                                          */
    /*---------------------------------------------------------------------------*/

    OHA_MRP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(MRP_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
		MRP_SET_STATUS_PTR_TYPE pMrpSet = &pRQB->args.ctrl.set_status;

		OHA_MRP_RQB_SET_OPCODE(pRQB,MRP_OPC_SET_STATUS);
        OHA_MRP_RQB_SET_HANDLE(pRQB,pHDB->MrpHandle);
        OHA_MRP_RQB_SET_USERID_UVAR16(pRQB,Orginator);

		pMrpSet->mrpInstance = Instance;
        pMrpSet->domainResetRoundTripDelays = ResetDelays;	/* to be done */

        OHA_LOWER_TRACE_03(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE,">>>: MRP-Request: MRP_OPC_SET_STATUS (Handle: 0x%X,MRPHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->MrpHandle,pRQB);

        OHA_MRP_REQUEST_LOWER(pRQB,pSys);
        pHDB->MrpRQBCnt++;  /* number of Requests pending within MRP */
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_MRPSetStatus(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPSetStatusDone                        +*/
/*+  Input                      OHA_MRP_LOWER_RQB_PTR_TYPE   pRQB           +*/
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
/*+  Description: Set MRP status confirmation                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPSetStatusDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal = LSA_RET_OK;
    LSA_UINT16              Orginator;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPSetStatusDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: MRP_OPC_SET_STATUS done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_MRP_RQB_GET_RESPONSE(pRQB));

	OHA_ASSERT_NULL_PTR(pRQB);
	OHA_ASSERT_NULL_PTR(pHDB);

    Orginator = OHA_LLDP_RQB_GET_USERID_UVAR16(pRQB);
	OHA_ASSERT(pRQB->args.ctrl.set_status.mrpInstance >= 1 && 
				pRQB->args.ctrl.set_status.mrpInstance <= EDD_CFG_MAX_MRP_INSTANCE_CNT);

    switch (OHA_MRP_RQB_GET_RESPONSE(pRQB))
    {
        case MRP_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = OHA_OK;
            break;

        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_UNEXP,"MRP-ERROR: MRP_OPC_SET_STATUS failed.(0x%X).",OHA_MRP_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_MRP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    pHDB->MrpRQBCnt--;

    /* further actions to be done */
    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
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
        case OHA_HANDLE_TYPE_SOCK:  /* from cable */
        {
			LSA_HANDLE_TYPE			Handle	= (LSA_HANDLE_TYPE)Orginator;
			OHA_HDB_SOCK_PTR_TYPE	pHDBSock = OHAGetSockHDBFromHandle(Handle); /* not OHA_SOCKGetHandleByNIC()! */

            if (OHA_IS_NOT_NULL(pHDBSock))  /* not closed */
            {
				OHA_HDB_AGNT_PTR_TYPE pSubAgnt = &pHDBSock->MrpAgent;

                if (pSubAgnt->StatsPendCnt == 0)    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

                pSubAgnt->StatsPendCnt--;   /* decrement the active requests */

                if (pSubAgnt->StatsPendCnt == 0)    /* got all necessary statistics ? */
                {
					if (pSubAgnt->RequestState == OHA_SNMP_STATE_WF_STATISTICS)
					{
						OHA_SNMPSockCheckPending(pHDBSock, OHA_MRP_MIB_NODE, Response); /* further actions taken in request handler */
						pSubAgnt->RequestState = OHA_SNMP_STATE_START;
					}
					else
					{
						OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_NOTE, "No active state anymore, late LldpGetStats.Con, state %Xh", pSubAgnt->RequestState);
					}
                }
            }
        }
        break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Mrp.NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPSetStatusDone(Response: 0x%X)", Response);
}

#endif /* OHA_CFG_NO_MRP */

/*****************************************************************************/
/*  end of file OHA_MRP.C                                                    */
/*****************************************************************************/

