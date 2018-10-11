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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_dcp.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE DCP functions                               */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  5
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_DCP */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "nare_inc.h"            /* NARE headerfiles */
#include "nare_int.h"            /* internal header */

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

#define NARE_IS_UPPER(c)    ((c) >= 'A' && (c) <= 'Z')
#define NARE_TO_LOWER(c)    (NARE_IS_UPPER(c) ? (c) + ('a' - 'A') : (c))
/* some defines that reflect the state of the received TLVs when identifying */

#define DEV_AND_INSTANCE_NOT_SET    0
#define DEV_VENDOR_SET              1
#define INSTANCE_SET                2
#define DEV_AND_INSTANCE_SET (INSTANCE_SET + DEV_VENDOR_SET)

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPNotifyRQBProvide                    +*/
/*+  Input                      NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  Result     : NARE_RSP_OK                                               +*/
/*+               NARE_RPS_ERR_RESOURCE                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates DCP-RQB for NOTIFY and provides them to DCP     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP  NARE_LOCAL_FCT_ATTR NARE_DCPNotifyRQBProvide(
    NARE_HDB_DCP_PTR_TYPE         pHDB)
{
    NARE_DCP_LOWER_RQB_PTR_TYPE   pRQB;
    LSA_USER_ID_TYPE              UserId;
    LSA_UINT16                    RetVal;
    NARE_RSP                      Status;
    LSA_UINT32                    i;
    NARE_DCP_LOWER_MEM_PTR_TYPE   pAlloc;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN : NARE_DCPNotifyRQBProvide(pHDB: 0x%X)",pHDB);
    Status = NARE_RSP_OK;

    pHDB->NotifyDCPCnt = 0;
    UserId.uvar32      = 0; /* not used */

    i = 0;

    while ((i< pHDB->NoOfRCtrlCnt) && (Status == NARE_RSP_OK))
    {
        NARE_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pHDB->Params.pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
        {
            NARE_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_NOTIFY);
            NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
            NARE_DCP_RQB_SET_USERID_UVAR32(pRQB,0);

            /* allocate Node memory     */
            NARE_DCP_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(DCP_NODE_IDENTIFY),pHDB->Params.pSys);
            pRQB->args.Notify.NodePtr = (DCP_USR_NODE_PTR)pAlloc;

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Notify.NodePtr,LSA_NULL) )
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-allocated lower NOTIFY RQB-memory (0x%X,0x%X)",pRQB->args.Notify.NodePtr,pRQB);
                NARE_LOWER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_NOTIFY (Handle: 0x%X,DCP-Handle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DCPHandle,pRQB);
                NARE_DCP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
                pHDB->NotifyDCPCnt++;
            }
            else
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-memory failed!");
                NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
                NARE_MEM_FREE_FAIL_CHECK(RetVal);
                Status = NARE_RSP_ERR_RESOURCE;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-rqb memory failed!");
            Status = NARE_RSP_ERR_RESOURCE;
        }
        i++;
    }
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPNotifyRQBProvide(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPChannelDone                         +*/
/*+  Input                      NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                             NARE_RSP                     Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  Response   : Responsecode of actual action taken (depends on state)    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handler for Open and Close-Channel requests to NARE for   +*/
/*+               DCP-Channels. This function is called after a lower       +*/
/*+               layer called back the NARE (e.g. after DCP finished his   +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_DCPChannelDone(
    NARE_HDB_DCP_PTR_TYPE       pHDB,
    NARE_RSP                    Response)
{
    NARE_UPPER_RQB_PTR_TYPE      pRQB;
    LSA_UINT16                   PathStat;
    LSA_UINT32                   Finish;
    LSA_VOID                     LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR, Cbf) (NARE_UPPER_RQB_PTR_TYPE pRQB);
	LSA_UINT32                   TraceIdx;

	TraceIdx = pHDB->TraceIdx;	/* save it, for the HDB may get freed */
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPChannelDone(pHDB: 0x%X, Response: 0x%X)",pHDB,Response);
    Finish = NOT_FINISHED; /* dont finish request by default */
    NARE_ASSERT_NULL_PTR(pHDB);
    pRQB   = pHDB->pOCReq;
    Cbf    = pHDB->Params.Cbf;
    NARE_ASSERT_NULL_PTR(pRQB);
    switch (pHDB->State)
    {       /*-------------------------------------------------------------------*/
            /* Just finished a DCP-Open Channel request.                         */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_OPEN_DCP_RUNNING:
            if ( Response == NARE_RSP_OK)
            {   /* allocate DCP-NOTIY RQBs and provide them     */
                Response = NARE_DCPNotifyRQBProvide(pHDB);
                if ( Response == NARE_RSP_OK)
                {   /* no more actions needed. Channel is open now. */
                    pHDB->State  = NARE_HANDLE_STATE_OPEN;
                    Finish = FINISHED_OK;
                }
                else
                {
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"NARE_DCPNotifyRQBProvide failed(0x%X). Aborting!",Response);
                    pHDB->State  = NARE_HANDLE_STATE_CLOSE_ERROR_DCP_RUNNING;
                    pHDB->OpenResponse = Response;  /* save response */
                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Closing Channel because of error.");
                    if ( NARE_DCPCloseChannel(pHDB->DCPHandle,pHDB->Params.pSys) != NARE_RSP_OK)
                    {
                        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close DCP Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        NARE_FATAL(NARE_FATAL_ERR_DCP);
                    }
                }
            }
            else
            {
                Finish = FINISHED_REL_SYSPTR;
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a DCP-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_CLOSE_DCP_RUNNING:
            if ( Response == NARE_RSP_OK)
            {   /* release PATH-Info !                          */
                Finish = FINISHED_REL_SYSPTR;
            }
            else
            {
                Finish = FINISHED_OK;
                /* is this a fatal error ?                      */
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a DCP-Close-Channel request because of error        */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_CLOSE_ERROR_DCP_RUNNING:
            if ( Response == NARE_RSP_OK)
            {  
                Response = pHDB->OpenResponse;  /* get Error-Response  */
            }
            else
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close DCP Channel().");
                NARE_FATAL(NARE_FATAL_ERR_DCP);
            }
            Finish = FINISHED_OK;
            break;

            /*-------------------------------------------------------------------*/
            /* Other states not expected within this function.                   */
            /*-------------------------------------------------------------------*/
        default:
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */
    if ( Finish != NOT_FINISHED)  /* finish the request ? */
    {
        pHDB->pOCReq = LSA_NULL;  /* no request running */
        NARE_RQB_SET_RESPONSE(pRQB,Response);
        /* upper - handler already set */
        NARE_UPPER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),Response);
        NARECallCbf(Cbf,pRQB,pHDB->Params.pSys);
        if (Finish == FINISHED_REL_SYSPTR)
        {	/* we release the HDB first                     */
            NARE_DCPReleaseHDB(pHDB);
			/* release PATH-Info !                          */
            NARE_RELEASE_PATH_INFO(&PathStat,pHDB->Params.pSys,pHDB->Params.pDetail);
            if (PathStat != LSA_RET_OK )
            {
                NARE_FATAL(NARE_FATAL_ERR_RELEASE_PATH_INFO);
            }
        }
    }
    NARE_FUNCTION_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPGetHandleByNIC                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    NARE_HDB_DCP_PTR_TYPE                       +*/
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
NARE_HDB_DCP_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_DCPGetHandleByNIC(
    LSA_UINT16 NICId)
{
    NARE_HDB_DCP_PTR_TYPE pHDB;
    LSA_UINT32            i;
    LSA_BOOL              Found;

    i     = NARE_MGM_GET_HDCP_PTR()->HandleCnt;
    pHDB  = NARE_MGM_GET_HDCP_PTR()->HDB;
    Found = LSA_FALSE;
    while ( (! Found ) && ( i ) )
    {
        if (( pHDB->State == NARE_HANDLE_STATE_OPEN ) &&
            ( pHDB->Params.pDetail->Params.Dcp.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDB++;
            i--;
        }
    }
    if ( ! Found )
    {
        pHDB = LSA_NULL;
    }
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPCheckNICUsed                        +*/
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
LSA_BOOL NARE_LOCAL_FCT_ATTR NARE_DCPCheckNICUsed(
    LSA_UINT16 NICId)
{

    NARE_HDB_DCP_PTR_TYPE pHDB;
    LSA_UINT32            i;
    LSA_BOOL              Found;

    i     = NARE_MGM_GET_HDCP_PTR()->HandleCnt;
    pHDB  = NARE_MGM_GET_HDCP_PTR()->HDB;
    Found = LSA_FALSE;

    while ( (! Found ) && ( i ) )
    {
        if (( pHDB->State != NARE_HANDLE_STATE_FREE ) &&
            ( pHDB->Params.pDetail->Params.Dcp.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDB++;
            i--;
        }
    }
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPGetFreeHDB                          +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE NARE_LOCAL..pHDBUser  +*/
/*+                             LSA_SYS_PTR_TYPE                  pSys      +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser   : NARE DCP  -channel handle to get pointer to HDB for.      +*/
/*+  pSys       : Systempointer                                             +*/
/*+                                                                         +*/
/*+  Result     : NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free DCP -HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_DCP_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBUser,
    LSA_SYS_PTR_TYPE                           pSys)
{
    NARE_RSP                  Status;
    LSA_BOOL                  Found;
    NARE_HDB_DCP_PTR_TYPE     pHDB;
    LSA_UINT32                DSize;
    LSA_UINT32                i;
    NARE_DCP_RCTRL_PTR_TYPE   pRCtrl;
    LSA_USER_ID_TYPE          UserId;


    pHDB   = (NARE_MGM_GET_HDCP_PTR()->HDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT, "IN : NARE_DCPGetFreeHDB(pHDB: 0x%X)",pHDBUser);
    i      = 0;
    Found  = LSA_FALSE;
    Status = NARE_RSP_OK;

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/

    while ( (! Found)  &&  ( i < NARE_MGM_GET_HDCP_PTR()->HandleCnt))
    {
        if ( pHDB->State == NARE_HANDLE_STATE_FREE )
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

        DSize = sizeof(NARE_DCP_RCTRL_TYPE) * NoOfParallelRQBs;

        /* allocate control-block memory */
        NARE_ALLOC_LOCAL_MEM((NARE_LOCAL_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *)&pHDB->DCPRCtrl.pTable,DSize);

        if ( LSA_HOST_PTR_ARE_EQUAL(pHDB->DCPRCtrl.pTable, LSA_NULL) )
        {
            Status = NARE_RSP_ERR_RESOURCE;
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"NARE_DCPGetFreeHDB(). Allocating LOCAL memory failed.");
        }
        if ( Status == NARE_RSP_OK)
        {
            UserId.uvar32 = 0;
            /* preallocate RQB used with HELLO-requets */
            NARE_DCP_ALLOC_LOWER_RQB(&pHDB->Hello.pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);
            if ( LSA_HOST_PTR_ARE_EQUAL(pHDB->Hello.pRQB, LSA_NULL) )
            {
                Status = NARE_RSP_ERR_RESOURCE;
                NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"NARE_DCPGetFreeHDB(). Allocating DCP RQB memory failed.");
            }
        }
        if ( Status == NARE_RSP_OK)
        {
            pHDB->Hello.Status = NARE_DCP_HELLO_IDLE; /* no HELLO active */
            /* ------------------------------------------------------------------*/
            /* Setup RCTRL-Blocks for DCP                                        */
            /* ------------------------------------------------------------------*/
            	/* initialize memory with 0 */
            NARE_MEMSET_LOCAL(pHDB->DCPRCtrl.pTable,0,DSize);
            /* initialize DCP RCTRL-Block management  */
            pHDB->DCPRCtrl.Cnt    = NoOfParallelRQBs;
            pHDB->DCPRCtrl.Free   = NoOfParallelRQBs;
            pHDB->DCPRCtrl.Used   = 0;
            pHDB->DCPRCtrl.pFree.pBottom  = LSA_NULL;
            pHDB->DCPRCtrl.pFree.pTop     = LSA_NULL;
            pRCtrl = &(* pHDB->DCPRCtrl.pTable)[0]; /* pointer to first element */
            for ( i=0; i< NoOfParallelRQBs; i++)
            {
                NARE_PUT_BLOCK_TOP(pHDB->DCPRCtrl.pFree.pBottom,
                                   pHDB->DCPRCtrl.pFree.pTop,
                                   pRCtrl);
                pRCtrl->InUse   = LSA_FALSE;
                pRCtrl->ThisIdx = (LSA_UINT16)i;
                pRCtrl++;
            }
            NARE_MGM_GET_HDCP_PTR()->UsedCnt++;
            pHDB->TraceIdx = 0;
            pHDB->State = NARE_HANDLE_STATE_CLOSED;
            *pHDBUser = pHDB;
        }
    }
    else
    {
        Status = NARE_RSP_ERR_RESOURCE;
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"NARE_DCPGetFreeHDB(). No more free handles.");
    }
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",Status,*pHDBUser);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPReleaseHDB                          +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE pHDB                  +*/
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
/*+               Sets Handle-state to NARE_HANDLE_STATE_FREE               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPReleaseHDB(
    NARE_HDB_DCP_PTR_TYPE pHDB)
{
    LSA_UINT16   FreeStat;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPReleaseHDB(pHDB: 0x%X)",pHDB);

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_ASSERT_NULL_PTR(pHDB->DCPRCtrl.pTable);

    NARE_FREE_LOCAL_MEM(&FreeStat, pHDB->DCPRCtrl.pTable);
    NARE_MEM_FREE_FAIL_CHECK(FreeStat);
    pHDB->DCPRCtrl.pTable = LSA_NULL;

    /* free HELLO - RQB */
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pHDB->Hello.pRQB, LSA_NULL) )
    {
        LSA_UINT16               RetVal;
        NARE_DCP_FREE_LOWER_RQB(&RetVal,pHDB->Hello.pRQB,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
    }
    pHDB->DCPRCtrl.Cnt    = 0;
    pHDB->DCPRCtrl.Free   = 0;
    pHDB->DCPRCtrl.Used   = 0;
    pHDB->DCPRCtrl.pFree.pBottom  = LSA_NULL;
    pHDB->DCPRCtrl.pFree.pTop     = LSA_NULL;
    pHDB->State           = NARE_HANDLE_STATE_FREE;
    NARE_MGM_GET_HDCP_PTR()->UsedCnt--;
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPReleaseHDB()");
    pHDB->TraceIdx        = 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPGetRCTRLByIdx                       +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE             pHDB      +*/
/*+                             LSA_UINT32                        Idx       +*/
/*+  Result                :    NARE_DCP_RCTRL_PTR_TYPE                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+  Idx        : Index in RCTRl-block table.                               +*/
/*+                                                                         +*/
/*+  Result     : Pointer to RCTRL-Block                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a DCP -RCTRL-Block by index               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_DCP_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_DCPGetRCTRLByIdx(
    NARE_HDB_DCP_PTR_TYPE pHDB,
    LSA_UINT32            Idx)
{
    NARE_DCP_RCTRL_PTR_TYPE  pRCtrl;

    NARE_ASSERT_NULL_PTR(pHDB);
    pRCtrl = LSA_NULL; /* prevent compiler warning with tasking */
    if ( Idx < pHDB->DCPRCtrl.Cnt )
    {
        pRCtrl = &(*pHDB->DCPRCtrl.pTable)[Idx];
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Invalid DCP RCTRL-Index.");
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    return(pRCtrl);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPGetFreeRCTRL                        +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE             pHDB      +*/
/*+  Result                :    NARE_DCP_RCTRL_PTR_TYPE                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+                                                                         +*/
/*+  Result     : Pointer to free RCTRL-Block or LSA_NULL if none free      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free DCP -RCTRL-Block                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_DCP_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_DCPGetFreeRCTRL(
    NARE_HDB_DCP_PTR_TYPE pHDB)
{
    NARE_DCP_RCTRL_PTR_TYPE  pRCtrl;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_REM_BLOCK_BOTTOM(pHDB->DCPRCtrl.pFree.pBottom,
                          pHDB->DCPRCtrl.pFree.pTop,
                          pRCtrl);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl, LSA_NULL) )
    {
        pHDB->DCPRCtrl.Used++;
        pHDB->DCPRCtrl.Free--;
        pRCtrl->InUse = LSA_TRUE;
    }
    return(pRCtrl);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPReleaseRCTRL                        +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE              pHDB     +*/
/*+                             NARE_DCP_RCTRL_PTR_TYPE            pRCtrl   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP-HDB                                        +*/
/*+  pRCtrl     : Pointer to DCP-RCTRL-Block to release                     +*/
/*+                                                                         +*/
/*+  Result     : --                                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Releases an DCP-RCTRL-Block                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPReleaseRCTRL(
    NARE_HDB_DCP_PTR_TYPE      pHDB,
    NARE_DCP_RCTRL_PTR_TYPE    pRCtrl)
{
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_ASSERT_NULL_PTR(pRCtrl);

    if ( pRCtrl->InUse )
    {
        /* Consistenzcheck validates RCtrlblock. Check Poitner with "ThisIdx" */
        if ( ! LSA_HOST_PTR_ARE_EQUAL(&(*pHDB->DCPRCtrl.pTable)[pRCtrl->ThisIdx],
                                      pRCtrl))
        {
            /* unexpected. ThisIdx and Pointer not matching.*/
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
        }
        /* put into free-list */
        NARE_PUT_BLOCK_TOP(pHDB->DCPRCtrl.pFree.pBottom,
                           pHDB->DCPRCtrl.pFree.pTop,
                           pRCtrl);
        pHDB->DCPRCtrl.Used--;
        pHDB->DCPRCtrl.Free++;
        pRCtrl->InUse = LSA_FALSE;
    }
    else
    {
        /* unexpected. Pointer not found in list */
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPTrigger                             +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE     pHDB              +*/
/*+                             NARE_DCP_RCTRL_PTR_TYPE   pRCtrl            +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+  pRCtrl     : Pointer to DCP-RCTRL-Block containing User-RCTRL-Block    +*/
/*+               with valid Bstate.                                        +*/
/*+                                                                         +*/
/*+  Result:      NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Depending on the State of the User-RCTRL-Block the        +*/
/*+               neccessary DCP-Request will be executed.                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPTrigger(
    NARE_HDB_DCP_PTR_TYPE     pHDB,
    NARE_DCP_RCTRL_PTR_TYPE   pRCtrl)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    NARE_RSP                  Status;

    NARE_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPTrigger(pHDB: 0x%X, PRCtrl: 0x%X (Idx: %d))",pHDB,pRCtrl,pRCtrl->ThisIdx);

    Status = NARE_RSP_OK;
    pRCtrlUser = pRCtrl->pRCtrl;
        /* Depending on RState we do the proper DCP-Request  */
    switch (*pRCtrlUser->pRState)
    {
        case NARE_RCTRL_RSTATE_DCP_IDENT:
            pRCtrl->RetryCnt  = pHDB->Params.pDetail->Params.Dcp.DCPIdentifyRetryCnt;
            pRCtrl->AliasUsed = 0; /* no ALIAS-Name used yet */
            pRCtrl->NotifyCnt = 0; /* no Notifys yet */
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Calling NARE_DCPIdentify().",pHDB->ThisHandle,pRCtrl->ThisIdx);
            NARE_ASSERT_NULL_PTR(pRCtrlUser->pRQB->args.ResConf.pStationName);
            Status = NARE_DCPIdentify(pHDB,
                                      pRCtrl,
                                      pRCtrlUser->pRQB->args.ResConf.pStationName,
                                      pRCtrlUser->pRQB->args.ResConf.StationNameLen);
            break;

        case NARE_RCTRL_RSTATE_DCP_SET:
            pRCtrl->RetryCnt = pHDB->Params.pDetail->Params.Dcp.DCPSetRetryCnt;
            /* Note: All parameters have to be filled within User-RQB ! */
            /*       If we use DNS the IP has to be set into the RQB    */
            /*       after DNS-Resolve.                                 */
            /*       So the Cfg.IPAddr is our desired address.          */
            /*              Res.IPAddr is our actual address from Ident.*/
            /*       If the actual address is equal to the desired one  */
            /*       we dont need a DCP-Set. this was checked outside.  */
            switch (pRCtrlUser->DCPSetMode)
            {
                    /* set only IP-Params */
                case NARE_RCTRL_DCP_SET_IP:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Calling NARE_DCPSet() IP-only.",pHDB->ThisHandle,pRCtrl->ThisIdx);

                    Status = NARE_DCPSet(pHDB,
                                         pRCtrl,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Res.MACAddr,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Cfg.IPAddr,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Cfg.SubMask,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Cfg.DefRouter,
                                         LSA_NULL,
                                         0,
                                         pRCtrl->pRCtrl->pRQB->args.ResConf.SetIPRemanent);
                    break;

                    /* set only StationName */
                case NARE_RCTRL_DCP_SET_STATIONNAME:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Calling NARE_DCPSet() StationName-only.",pHDB->ThisHandle,pRCtrl->ThisIdx);

                    Status = NARE_DCPSet(pHDB,
                                         pRCtrl,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Res.MACAddr,
                                         LSA_NULL,
                                         LSA_NULL,
                                         LSA_NULL,
                                         (NARE_UPPER_MEM_U8_PTR_TYPE)pRCtrl->pRCtrl->pRQB->args.ResConf.pStationName,
                                         pRCtrl->pRCtrl->pRQB->args.ResConf.StationNameLen,
                                         pRCtrl->pRCtrl->pRQB->args.ResConf.SetIPRemanent);
                    break;

                    /* set IP-Params and StationName */
                case NARE_RCTRL_DCP_SET_ALL:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Calling NARE_DCPSet() IP + StationName.",pHDB->ThisHandle,pRCtrl->ThisIdx);

                    Status = NARE_DCPSet(pHDB,
                                         pRCtrl,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Res.MACAddr,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Cfg.IPAddr,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Cfg.SubMask,
                                         &pRCtrl->pRCtrl->pRQB->args.ResConf.Cfg.DefRouter,
                                         (NARE_UPPER_MEM_U8_PTR_TYPE)pRCtrl->pRCtrl->pRQB->args.ResConf.pStationName,
                                         pRCtrl->pRCtrl->pRQB->args.ResConf.StationNameLen,
                                         pRCtrl->pRCtrl->pRQB->args.ResConf.SetIPRemanent);
                    break;
                default:
                    /* nothing to set. Not expected  */
                    NARE_PROGRAM_TRACE_03(0,LSA_TRACE_LEVEL_FATAL,"DCP[%X:%d]-Unexpected DCPSetMode (0x%X).",pHDB->ThisHandle,pRCtrl->ThisIdx,pRCtrlUser->DCPSetMode);
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;
            } /* switch */
            break;

            default:
            /* unexpected state */
            NARE_PROGRAM_TRACE_03(0,LSA_TRACE_LEVEL_FATAL,"DCP[%X:%d]-Unexpected RState (0x%X).",pHDB->ThisHandle,pRCtrl->ThisIdx,*pRCtrlUser->pRState);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    NARE_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT: NARE_DCPTrigger(), Status: 0x%X",
                           Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPCheckAndStartPending                +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE              pHDB     +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RCTRL-Block pending for DCP. If so check   +*/
/*+               for free DCP-RCTRL-Blocks. As long as one is free and     +*/
/*+               User-RCTRL-Blocks present start handling this Blocks by   +*/
/*+               doing the DCP-Request. The type of DCP-Request depends on +*/
/*+               the State of the User-RCTRL-Block.                        +*/
/*+                                                                         +*/
/*+               If starting of a DCP-request fails we finish the request. +*/
/*+                                                                         +*/
/*+               This function should be called after a DCP-Request was    +*/
/*+               finished, or a new User-RCTRL-Block was queued in DCP     +*/
/*+               pending-queue.                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPCheckAndStartPending(
    NARE_HDB_DCP_PTR_TYPE     pHDB)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    NARE_DCP_RCTRL_PTR_TYPE   pRCtrlDcp;
    LSA_BOOL                  Ready;
    NARE_RSP                  Status;
    NARE_HDB_USER_PTR_TYPE    pHDBUser;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPCheckAndStartPending(pHDB: 0x%X)",pHDB);
    Status = NARE_RSP_OK;
    Ready  = LSA_FALSE;

    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Checking for pending requests (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
    while (! Ready )
    {
        if ( pHDB->PendRCtrlCnt ) /* User-RCTRL-Blocks pending ? */
        {
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-requests pending (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
            if (pHDB->DCPRCtrl.Free ) /* Free DCP-RCTRL-Blocks ? */
            {
                NARE_REM_BLOCK_BOTTOM(pHDB->PendRCtrl.pBottom,
                                      pHDB->PendRCtrl.pTop,
                                      pRCtrlUser);
                /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
                NARE_ASSERT_NULL_PTR(pRCtrlUser);
                pRCtrlUser->QueueType = NARE_RCTRL_QTYPE_DCP_RUNNING;
                pHDB->PendRCtrlCnt--;
                pRCtrlDcp = NARE_DCPGetFreeRCTRL(pHDB);
                /* because Free was >0 we dont expect LSA_NULL here         */
                NARE_ASSERT_NULL_PTR(pRCtrlDcp);
                /* save pointer to user-RCTRL-Block within DCP-RCTRL-block  */
                /* for later access. DCP-requests getting the ThisIdx from  */
                /* the DCP-RCTRL-Block as user-id                           */
                pRCtrlDcp->pRCtrl = pRCtrlUser;
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Free RCTRL-Found [Hdl: 0x%X,Idx: %d]. Triggering EDD Request",pHDB->ThisHandle,pRCtrlDcp->ThisIdx);
                Status = NARE_DCPTrigger(pHDB,pRCtrlDcp);
                /* if failed we finish the user-request*/
                if ( Status != NARE_RSP_OK )
                {
                    /* release the DCP-RCTRL-Block */
                    NARE_DCPReleaseRCTRL(pHDB,pRCtrlDcp);
                    pHDBUser = NAREGetUserHDBFromHandle(NARE_RQB_GET_HANDLE(pRCtrlUser->pRQB));
                    NARE_ASSERT_NULL_PTR(pHDBUser);
                    /* release RCtrl-Block and finish the request */
                    NARE_UserReady(pHDBUser,pRCtrlUser,Status);
                }
            }
            else
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-No free DCP-RCTRL (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
                Ready = LSA_TRUE;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-No requests pending  (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
            Ready = LSA_TRUE;
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPCheckAndStartPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPApplyRCtrl                          +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE      pHDB             +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block to handle.                    +*/
/*+               Must contain valid Bstate which selects the action to do. +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Give a User-RCTRL-Block to DCP to handle the Request.     +*/
/*+               If no free DCP-Resources present the request remains      +*/
/*+               queued until resources are feed. If resources are free    +*/
/*+               the request is handled (doing a DCP-request)              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPApplyRCtrl(
    NARE_HDB_DCP_PTR_TYPE     pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser)
{
    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Queuing block (pHDB: 0x%X,Handle: 0x%X,pRCtrlUser: 0x%X)",pHDB,pHDB->ThisHandle,pHDB->ThisHandle);

    NARE_PUT_BLOCK_TOP(pHDB->PendRCtrl.pBottom,
                       pHDB->PendRCtrl.pTop,
                       pRCtrlUser);
    pRCtrlUser->QueueType = NARE_RCTRL_QTYPE_DCP_PENDING;
    pHDB->PendRCtrlCnt++;
    NARE_DCPCheckAndStartPending(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPRemoveRCtrl                         +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE      pHDB             +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block to handle.                    +*/
/*+               Must contain valid Bstate which selects the action to do. +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removed RCTRL-Block from pending-queue.                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPRemoveRCtrl(
    NARE_HDB_DCP_PTR_TYPE     pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlHelp;

    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Removing block (pHDB: 0x%X, Handle: 0x%X,pRCtrlUser: 0x%X)",pHDB,pHDB->ThisHandle,pRCtrlUser);
    NARE_REM_BLOCK(pHDB->PendRCtrl.pBottom,
                   pHDB->PendRCtrl.pTop,
                   pRCtrlUser,
                   pRCtrlHelp);
    pHDB->PendRCtrlCnt--;
    NARE_ASSERT_NULL_PTR(pRCtrlHelp);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPOpenChannelDone                     +*/
/*+  Input                      NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
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
/*+               - save DCP-Channel                                        +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPOpenChannelDone(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_DCP_PTR_TYPE        pHDB)
{
    NARE_RSP                 Response;
    LSA_UINT16               RetVal;

    NARE_FUNCTION_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_DCP_RQB_GET_RESPONSE(pRQB));
    if ( NARE_DCP_RQB_GET_RESPONSE(pRQB) == DCP_RSP_OK )
    {
        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Open Channel successful");
        /* save DCP-Channel in HDB */
        /* old: pHDB->DCPHandle = pRQB->handle; */
        pHDB->DCPHandle = pRQB->args.channel.handle;
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Open Channel failed.[0x%X]",NARE_DCP_RQB_GET_RESPONSE(pRQB));
        Response = NARE_RSP_ERR_LL;
    }
    NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_DCPChannelDone(pHDB,Response); /* further actions taken in channel handler */
    NARE_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPOpenChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPCloseChannelDone                    +*/
/*+  Input                      NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
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

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPCloseChannelDone(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_DCP_PTR_TYPE        pHDB)
{
    NARE_RSP                 Response;
    LSA_UINT16               RetVal;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);

    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_DCP_RQB_GET_RESPONSE(pRQB));
    if ( NARE_DCP_RQB_GET_RESPONSE(pRQB) == DCP_RSP_OK )
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Close Channel successful");
        /* there should be no more notify rqbs within DCP ! */
        /* all should be canceled                           */
        if ( pHDB->NotifyDCPCnt )
        {
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"DCP-Close Channel. Still NOTIFY RQBs in use (%d)",pHDB->NotifyDCPCnt);
            NARE_FATAL(NARE_FATAL_ERR_DCP);
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-Close Channel failed.[0x%X]",NARE_DCP_RQB_GET_RESPONSE(pRQB));
        Response = NARE_RSP_ERR_LL;
    }
    NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_DCPChannelDone(pHDB,Response); /* further actions taken in channel handler */
    NARE_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPCloseChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPAnalyseIdentifyRspTLV               +*/
/*+  Input                      DCP_USR_NODE_PTR                pNode       +*/
/*+                             LSA_UINT8 NARE_UPPER_RQB_ATTR * pMAC        +*/
/*+                             LSA_UINT8 NARE_UPPER_RQB_ATTR * pIP         +*/
/*+                             LSA_UINT8 NARE_UPPER_RQB_ATTR * pSubMask    +*/
/*+                             LSA_UINT8 NARE_UPPER_RQB_ATTR * pDefRouter  +*/
/*+                             NARE_UPPER_MEM_U8_PTR_TYPE      pStationName+*/
/*+                             LSA_UINT32                      StationName.+*/
/*+                             LSA_UINT32 NARE_LOCAL_MEM_ATTR * pStation...+*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pNode                 :    Pointer to Nodes returned from Identify     +*/
/*+  pMac                  :    pointer to memory for MAC (output) 6 Byte   +*/
/*+  pIP                   :    Pointer to memory for IP  (output) 4 Byte   +*/
/*+  pSubMask              :    Pointer to memory for SM  (output) 4 Byte   +*/
/*+  pDefRouter            :    Pointer to memory for DR  (output) 4 Byte   +*/
/*+  pStationName          :    Pointer to StationName to check (input)     +*/
/*+  StationNameLen        :    Length of StationName           (input)     +*/
/*+  pStationNameState     :    StationName state (output)                  +*/
/*+                             NARE_RCTRL_DCP_STATION_NAME_NOT_SET         +*/
/*+                             NARE_RCTRL_DCP_STATION_NAME_SET_EQUAL       +*/
/*+                             NARE_RCTRL_DCP_STATION_NAME_SET_DIFFERENT   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_TRUE: MAC and IP found and set.         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get MAC and IP from Node returned from Identify request.  +*/
/*+               Check if StationName within Node is equal to pStationName +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL NARE_LOCAL_FCT_ATTR NARE_DCPAnalyseIdentifyRspTLV(
    DCP_USR_NODE_PTR                pNode,
    LSA_UINT8 NARE_UPPER_RQB_ATTR  *pMAC,
    LSA_UINT8 NARE_UPPER_RQB_ATTR  *pIP,
    LSA_UINT8 NARE_UPPER_RQB_ATTR  *pSubMask,
    LSA_UINT8 NARE_UPPER_RQB_ATTR  *pDefRouter,
    NARE_UPPER_MEM_U8_PTR_TYPE      pStationName,
    LSA_UINT32                      StationNameLen,
    LSA_UINT32 NARE_LOCAL_MEM_ATTR *pStationNameState,
    NARE_ID_STORAGE_TYPE           *pIDS)
{
    LSA_UINT32                      Found;
    LSA_BOOL                        Result;
    LSA_UINT32                      Len;
    LSA_UINT32                      i;
    NARE_DCP_IDENT_RSP_TLV_PTR_TYPE pTLV;


    NARE_COPY_MAC_ADDR_UR_LDM(pMAC,pNode->NodeAddr);
    Result = LSA_FALSE;
    Found  = 0;
    *pStationNameState = NARE_RCTRL_DCP_STATION_NAME_NOT_SET;
    pTLV  = (NARE_DCP_IDENT_RSP_TLV_PTR_TYPE) pNode->TlvChain;
    pIDS->Foundstate = DEV_AND_INSTANCE_NOT_SET;
    while ((pNode->TlvCnt) && ( Found != 0xF ))
    {
        Len = NARE_NTOHS(pTLV->Len);
        /* check if this is a TCP-IP Dataset and size is valid */
        switch (NARE_NTOHS(pTLV->Type))
        {       /*--------------------------------------------------------------------*/
            case DCP_TYPE_TCP_IP:
                /*--------------------------------------------------------------------*/
                if ( ! (Found & 1))
                {
                    Found  |= 1;
                    if ( Len >= (sizeof(NARE_DCP_DS_TCPIP_TYPE) + sizeof(pTLV->Status)) )
                    {
                        Result  = LSA_TRUE;
                        /* if IP Addr is not active we ignore it and continue like no */
                        /* IP Addr is set at all                                      */
                        if ( NARE_NTOHS(pTLV->Status) & NARE_DCP_IDENTIFY_STATUS_IP_ADDR_CONFLICT )
                        {
                            LSA_UINT32 j;
                            NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"DCP-Identfiy. IP conflict bit set. Ignoring IP-Suite!");
                            for (j=0; j<NARE_IP_ADDR_SIZE; j++)
                            {
                                pIP[j]       =0;
                                pSubMask[j]  =0;
                                pDefRouter[j]=0;
                            }
                        }
                        else
                        {
                            NARE_COPY_IP_ADDR_UR_LDM(pIP,pTLV->Data.TcpIp.IpAddr);
                            NARE_COPY_IP_ADDR_UR_LDM(pSubMask,pTLV->Data.TcpIp.SubnetMask);
                            NARE_COPY_IP_ADDR_UR_LDM(pDefRouter,pTLV->Data.TcpIp.DefaultRouter);
                        }
                    }
                    else
                    {
                        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Identfiy response invalid TCPIP parameter TLV!");
                    }
                }
                else
                {
                    NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Identfiy response multiple TCPIP parameters!");
                }
                break;

                /*--------------------------------------------------------------------*/
            case DCP_TYPE_DEVICE_NAME:
                /*--------------------------------------------------------------------*/
                if ( ! (Found & 2))
                {
                    Found |= 2;
                    if (( Len == (StationNameLen + sizeof(pTLV->Status)) ) &&
                        (! LSA_HOST_PTR_ARE_EQUAL(pStationName, LSA_NULL)))  /* same length */
                    {
                        LSA_BOOL StationOK;
                        StationOK = LSA_TRUE;
                        i = 0;
                        while ((i < StationNameLen ) && ( StationOK ))
                        {
                            if (NARE_TO_LOWER(pTLV->Data.NameOfStation.Name[i]) != NARE_TO_LOWER(pStationName[i]))
                            {
                                StationOK = LSA_FALSE;
                            }
                            i++;
                        }
                        if ( StationOK )
                        {
                            *pStationNameState = NARE_RCTRL_DCP_STATION_NAME_SET_EQUAL;
                        }
                        else
                        {
                            NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"DCP-Identfiy: Station name set different!");
                            /* Station name set but different */
                            *pStationNameState = NARE_RCTRL_DCP_STATION_NAME_SET_DIFFERENT;
                        }
                    }
                    else
                    {
                        if ( Len == sizeof(pTLV->Status) )
                        {
                            /* a Len of 0 is not valid but we accept it and ignore it          */
                            /* (to be compatible with older OHA version setting this by fault) */
                            NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"DCP-Identfiy: Station name Len is 0! -> ignore");
                            *pStationNameState = NARE_RCTRL_DCP_STATION_NAME_NOT_SET;
                        }
                        else
                        {
                            NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"DCP-Identfiy: Station name set different!");
                            /* Station name set but different */
                            *pStationNameState = NARE_RCTRL_DCP_STATION_NAME_SET_DIFFERENT;
                        }
                    }
                }
                else
                {
                    NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Identfiy response multiple NameOfStation!");
                }
                break;

            case DCP_TYPE_DEVICE_ID:
                {
                    if ( ! (Found & 4))
                    {
                        Found |= 4;
                        pIDS->DeviceID = NARE_NTOHS(pTLV->Data.DevAndVendorID.DeviceID);
                        pIDS->VendorID = NARE_NTOHS(pTLV->Data.DevAndVendorID.VendorID);
                        pIDS->Foundstate |= DEV_VENDOR_SET;
                        NARE_PROGRAM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"DCP-Identfiy Device-IDs 0x%x VendorID 0x%x",pIDS->DeviceID, pIDS->VendorID);
                    }
                    else
                    {
                        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Identfiy response multiple Device-IDs!");
                    }
                }
                break;

            case DCP_TYPE_DEVICE_INST:
                if ( ! (Found & 8))
                {
                    Found |= 8;
                    pIDS->InstanceID =  NARE_NTOHS(pTLV->Data.InstanceId.InstanceID);
                    pIDS->Foundstate |= INSTANCE_SET;
                }
                else
                {
                    NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Identfiy response multiple Device-instances!");
                }
                break;

            default:
                break;
        }
        /* switch to next TLV */
        if ( Len & 1 ) Len++; /* next TLV on even boundary! */
        pTLV = (NARE_DCP_IDENT_RSP_TLV_PTR_TYPE) (((NARE_DCP_LOWER_MEM_U8_PTR_TYPE) pTLV) + sizeof(NARE_DCP_TL_TYPE) + Len);
        pNode->TlvCnt--;
    }
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPIdentifyDone                        +*/
/*+  Input                      NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Identify done handling.                                   +*/
/*+                                                                         +*/
/*+               * check if retry neccessary                               +*/
/*+               * use ALIAS-Names if present.                             +*/
/*+               * on success fill identify-data into user-rqb for later   +*/
/*+                 use (IP/MAC) (in Res.)                                  +*/
/*+               * if no retry, release RCTRL-Block and RQB-memory         +*/
/*+               * Call User-Statemachine.                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPIdentifyDone(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_DCP_PTR_TYPE        pHDB)
{
    NARE_RSP                 Response;
    LSA_BOOL                 CallStateMachine;
    LSA_BOOL                 FreeAll;
    NARE_DCP_RCTRL_PTR_TYPE  pRCtrl;
    LSA_UINT16               RetVal;
    NARE_USER_RCTRL_PTR_TYPE pRCtrlUser;
    NARE_ID_STORAGE_TYPE     remIDs;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPIdentifyDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    CallStateMachine = LSA_TRUE;
    Response         = NARE_RSP_OK;
    FreeAll          = LSA_TRUE;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_IDENTIFY done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_DCP_RQB_GET_RESPONSE(pRQB));
    pRCtrl = NARE_DCPGetRCTRLByIdx(pHDB,NARE_DCP_RQB_GET_USERID_UVAR32(pRQB));
    NARE_ASSERT_NULL_PTR(pRCtrl);
    pRCtrlUser = pRCtrl->pRCtrl;
    NARE_ASSERT_NULL_PTR(pRCtrlUser);
    switch (NARE_DCP_RQB_GET_RESPONSE(pRQB))
    {
        case DCP_RSP_OK:
        case DCP_RSP_OK_MORE:
            /* ---------------------------------------------*/
            /* if NodeCnt is 0 we found no station and      */
            /* we may retry.                                */
            /* ---------------------------------------------*/
            if ( pRQB->args.Identify.NodeCnt == 0 )
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify no response.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                if (( pRCtrl->RetryCnt) && ( ! pRCtrl->pRCtrl->DoCancel ))
                {
                    pRCtrl->RetryCnt--;
                    /* set the right handle. and NodeCnt back to 1  */
                    /* all other params still ok.                   */
                    NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
                    pRQB->args.Identify.NodeCnt  = 1;
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify retry request.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                    NARE_DCP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
                    CallStateMachine = LSA_FALSE;
                }
                else
                {
#ifndef NARE_CFG_NO_ALIAS_SUPPORT
                    /* try ALIAS-Names if present */
                    if (( pRCtrl->AliasUsed < pRCtrlUser->pRQB->args.ResConf.AliasCnt ) && ( ! pRCtrl->pRCtrl->DoCancel ))
                    {
                        NARE_ASSERT_NULL_PTR(pRCtrlUser->pRQB->args.ResConf.pStationNameAlias);

                        NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Identify.NodePtr,pHDB->Params.pSys);
                        NARE_MEM_FREE_FAIL_CHECK(RetVal);
                        NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Identify.FilterPtr,pHDB->Params.pSys);
                        NARE_MEM_FREE_FAIL_CHECK(RetVal);
                        FreeAll = LSA_FALSE; /* Flag that we already freed parts of RQB */
                        /* fillup RQB with ALIAS-Name */
                        Response = NARE_DCPIdentifyRQBSetup(pHDB,
                                                            pRQB,
                                                            (NARE_UPPER_MEM_U8_PTR_TYPE)pRCtrlUser->pRQB->args.ResConf.pStationNameAlias[pRCtrl->AliasUsed].pName,
                                                            pRCtrlUser->pRQB->args.ResConf.pStationNameAlias[pRCtrl->AliasUsed].NameLen,
                                                            LSA_TRUE,
                                                            pRCtrlUser->pRQB->args.ResConf.Notify,
                                                            pRCtrl->ThisIdx,
                                                            pRCtrlUser->pRQB->args.ResConf.Tolerance);
                        pRCtrl->AliasUsed++;
                        if ( Response == NARE_RSP_OK )
                        {   /* set the right handle. and NodeCnt back to 1  */
                            /* all other params still ok.                   */
                            NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
                            pRQB->args.Identify.NodeCnt  = 1;
                            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify retry request with ALIAS %d.",pHDB->ThisHandle,pRCtrl->ThisIdx,pRCtrl->AliasUsed);
                            NARE_DCP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
                            CallStateMachine = LSA_FALSE;
                        }
                    }
                    else /* no more ALIAS present */
#endif
                    {
                        NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,"DCP[%X:%d]-Identify Station not found.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                        Response = NARE_RSP_ERR_DCP_STATION_NOT_FOUND;
                    }
                }
            }
            else /* no timeout */
            {   /* ---------------------------------------------*/
                /* At least one station response.               */
                /* ---------------------------------------------*/
                if ( pRQB->args.Identify.NodeCnt != 1 )
                {
                    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"DCP[%X:%d]-Identify invalid NodeCnt (%d)",pHDB->ThisHandle,pRCtrl->ThisIdx,pRQB->args.Identify.NodeCnt);
                    NARE_FATAL(NARE_FATAL_ERR_DCP);
                }
                /* copy ALIAS used into user-RQB                            */
                /* With this we see we have to SET the StationName later on */
                pRCtrlUser->pRQB->args.ResConf.AliasUsed = pRCtrl->AliasUsed;
                pRCtrlUser->DCPStationNameState = NARE_RCTRL_DCP_STATION_NAME_NOT_SET;
                if ( NARE_DCPAnalyseIdentifyRspTLV(pRQB->args.Identify.NodePtr,
                                                   pRCtrlUser->pRQB->args.ResConf.Res.MACAddr,
                                                   pRCtrlUser->pRQB->args.ResConf.Res.IPAddr,
                                                   pRCtrlUser->pRQB->args.ResConf.Res.SubMask,
                                                   pRCtrlUser->pRQB->args.ResConf.Res.DefRouter,
                                                   (NARE_UPPER_MEM_U8_PTR_TYPE)pRCtrlUser->pRQB->args.ResConf.pStationName,
                                                   pRCtrlUser->pRQB->args.ResConf.StationNameLen,
                                                   &pRCtrlUser->DCPStationNameState,
                                                   &remIDs ))
                {
                    NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify successful1. IP: %d.%d.%d.%d",
                                          pHDB->ThisHandle,
                                          pRCtrl->ThisIdx,
                                          pRCtrlUser->pRQB->args.ResConf.Res.IPAddr[0],
                                          pRCtrlUser->pRQB->args.ResConf.Res.IPAddr[1],
                                          pRCtrlUser->pRQB->args.ResConf.Res.IPAddr[2],
                                          pRCtrlUser->pRQB->args.ResConf.Res.IPAddr[3]);
                    NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify successful2. SN: %d.%d.%d.%d",
                                          pHDB->ThisHandle,
                                          pRCtrl->ThisIdx,
                                          pRCtrlUser->pRQB->args.ResConf.Res.SubMask[0],
                                          pRCtrlUser->pRQB->args.ResConf.Res.SubMask[1],
                                          pRCtrlUser->pRQB->args.ResConf.Res.SubMask[2],
                                          pRCtrlUser->pRQB->args.ResConf.Res.SubMask[3]);
                    NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify successful3. DR: %d.%d.%d.%d",
                                          pHDB->ThisHandle,
                                          pRCtrl->ThisIdx,
                                          pRCtrlUser->pRQB->args.ResConf.Res.DefRouter[0],
                                          pRCtrlUser->pRQB->args.ResConf.Res.DefRouter[1],
                                          pRCtrlUser->pRQB->args.ResConf.Res.DefRouter[2],
                                          pRCtrlUser->pRQB->args.ResConf.Res.DefRouter[3]);
                    NARE_PROGRAM_TRACE_08(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify successful4. MAC: %02X-%02X-%02X-%02X-%02X-%02X",
                                          pHDB->ThisHandle,
                                          pRCtrl->ThisIdx,
                                          pRCtrlUser->pRQB->args.ResConf.Res.MACAddr[0],
                                          pRCtrlUser->pRQB->args.ResConf.Res.MACAddr[1],
                                          pRCtrlUser->pRQB->args.ResConf.Res.MACAddr[2],
                                          pRCtrlUser->pRQB->args.ResConf.Res.MACAddr[3],
                                          pRCtrlUser->pRQB->args.ResConf.Res.MACAddr[4],
                                          pRCtrlUser->pRQB->args.ResConf.Res.MACAddr[5]);
                    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify successful5. AliasUsed: %d",
                                          pHDB->ThisHandle,
                                          pRCtrl->ThisIdx,
                                          pRCtrl->AliasUsed);
                    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Identify successful6. StationState: %d",
                                          pHDB->ThisHandle,
                                          pRCtrl->ThisIdx,
                                          pRCtrlUser->DCPStationNameState);
                    /* if more then one station response we have a multiple station response */
                    /* and we abort (but still filled the MAC from one station for user)     */
                    if ( NARE_DCP_RQB_GET_RESPONSE(pRQB) == DCP_RSP_OK_MORE )
                    {   /* ---------------------------------------------*/
                        /* more then one Station with same station name */
                        /* ---------------------------------------------*/
                        NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP[%X:%d]-ERROR: Identify multiple stations.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                        Response = NARE_RSP_ERR_DCP_MULTIPLE_STATIONS;
                    }
                    else
                    {
                        /* AP01486134 und AP01480905:introduction of VendorID and DeviceID Check            */
                        /* first check if extended mode is active and the Dev-/ vendor IDs are really found */
                        if(pRCtrlUser->pRQB->args.ResConf.ExtendedMode != NARE_EXT_MODE_LEGACY)
                        {  
                            if (!(remIDs.Foundstate & DEV_VENDOR_SET ))    /* no device vendor ids found */
                             {
                                   Response = NARE_RSP_ERR_DCP_ID_MISMATCH;
                                   break;
                             }
                             /* check vendorID and DeviceID first */
                            if((pRCtrlUser->pRQB->args.ResConf.VendorID == remIDs.VendorID)  &&     /* VendorID match */
                                (pRCtrlUser->pRQB->args.ResConf.DeviceID == remIDs.DeviceID) &&     /* deviceID match */
                                (remIDs.Foundstate != DEV_AND_INSTANCE_NOT_SET))  /* VendorID and DeviceID were found */
                            {
                                if( remIDs.Foundstate & INSTANCE_SET)             /* instance present?                */
                                {
                                    if(pRCtrlUser->pRQB->args.ResConf.InstanceID != remIDs.InstanceID)
                                    {
                                        Response = NARE_RSP_ERR_DCP_ID_MISMATCH;
                                    }
                                }
                                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                    "Vendor and DeciceID match deviceId = 0x%x Rem InstanceId = 0x%x ==? args_InstanceID 0x%x",
                                    remIDs.DeviceID,remIDs.InstanceID,pRCtrlUser->pRQB->args.ResConf.InstanceID);
                            }
                            else
                            {
                                Response = NARE_RSP_ERR_DCP_ID_MISMATCH;
                            }
                            if  (Response == NARE_RSP_OK) /* the ID's are matching */
                            {   /* if AllowStationNameOverwrite is set we overwite if different! */
                                if (pRCtrlUser->DCPStationNameState == NARE_RCTRL_DCP_STATION_NAME_SET_DIFFERENT)
                                {
                                    if (pRCtrlUser->pRQB->args.ResConf.AllowStationNameOverwrite)
                                    {
                                        pRCtrlUser->DCPStationNameState = NARE_RCTRL_DCP_STATION_NAME_NOT_SET;
                                        pRCtrlUser->pRQB->args.ResConf.StationNameSetEqual = LSA_TRUE;
                                    }
                                    else
                                    {
                                        Response = NARE_RSP_ERR_STATION_NAME_DIFFERENT;
                                    }
                                }
                                else
                                    pRCtrlUser->pRQB->args.ResConf.StationNameSetEqual = LSA_TRUE;
                            }
                        } /* end of ID CHECK */
                        else 
                        { /* no ID check: */
                            if ( pRCtrlUser->DCPStationNameState == NARE_RCTRL_DCP_STATION_NAME_SET_DIFFERENT )
                            {
                                if (pRCtrlUser->pRQB->args.ResConf.AllowStationNameOverwrite)
                                {
                                    pRCtrlUser->DCPStationNameState = NARE_RCTRL_DCP_STATION_NAME_NOT_SET;
                                }
                                else
                                {
                                    Response = NARE_RSP_ERR_STATION_NAME_DIFFERENT;
                                }
                            }
                            else
                            {   /* If the StationName was already set we set StationNameSetEqual to TRUE */
                                if ( pRCtrlUser->DCPStationNameState == NARE_RCTRL_DCP_STATION_NAME_SET_EQUAL )
                                {
                                    pRCtrlUser->pRQB->args.ResConf.StationNameSetEqual = LSA_TRUE;
                                }
                            }
                        }
                    }
                }
                else
                {
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP[%X:%d]-ERROR: No (valid) IP-Dataset within Identify response",
                        pHDB->ThisHandle,pRCtrl->ThisIdx);
                    Response = NARE_RSP_ERR_DCP_IDENT_TLV_ERROR; /* not expected to find no IP */
                }
            }
            break;

        default:
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,"DCP[%X:%d]-ERROR: Identify LL (0x%X)",pHDB->ThisHandle,pRCtrl->ThisIdx,NARE_DCP_RQB_GET_RESPONSE(pRQB));
            Response = NARE_RSP_ERR_LL;
            break;
    }
    if (CallStateMachine)
    {
        /* release the RQB and RCTRL-Block */
        NARE_DCPReleaseRCTRL(pHDB,pRCtrl);
        if ( FreeAll )
        {
            NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Identify.NodePtr,pHDB->Params.pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
            NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Identify.FilterPtr,pHDB->Params.pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
        }
        NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        /* call the main statemachine     */
        NARE_UserStateMachine(pRCtrlUser,Response,LSA_TRUE);
    }
    NARE_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPIdentifyDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPSetDone                             +*/
/*+  Input                      NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: SET done handling.                                        +*/
/*+                                                                         +*/
/*+               On Timeout we retry if configured.                        +*/
/*+               On Success we test the result within request.             +*/
/*+               if resullt ok we copy the cfg-ip in rqb to res because    +*/
/*+               this is the new actual ip of the device                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPSetDone(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_DCP_PTR_TYPE        pHDB)
{
    NARE_RSP                       Response;
    LSA_BOOL                       CallStateMachine;
    NARE_DCP_RCTRL_PTR_TYPE        pRCtrl;
    LSA_UINT16                     RetVal;
    NARE_USER_RCTRL_PTR_TYPE       pRCtrlUser;
    NARE_DCP_SET_RSP_TLV_PTR_TYPE  pTLV;
    LSA_UINT32                     i;
    LSA_UINT32                     Len;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPSetDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);

    CallStateMachine = LSA_TRUE;
    Response         = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_SET done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_DCP_RQB_GET_RESPONSE(pRQB));
    pRCtrl = NARE_DCPGetRCTRLByIdx(pHDB,NARE_DCP_RQB_GET_USERID_UVAR32(pRQB));
    NARE_ASSERT_NULL_PTR(pRCtrl);
    pRCtrlUser = pRCtrl->pRCtrl;
    NARE_ASSERT_NULL_PTR(pRCtrl);
    switch (NARE_DCP_RQB_GET_RESPONSE(pRQB))
    {
        case DCP_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* Check if SET was a success                   */
            /* ---------------------------------------------*/
            if (pRQB->args.Set.OptCnt == pRCtrl->SetOptions.OptionCnt ) /* we only correct option cnt */
            {
                pTLV = (NARE_DCP_SET_RSP_TLV_PTR_TYPE) pRQB->args.Set.RspPtr;
                i = 0;
                /* check if this are the right TLVs and status is OK */
                while (( i < pRCtrl->SetOptions.OptionCnt) && ( Response == NARE_RSP_OK))
                {
                    Len = NARE_NTOHS(pTLV->Len); /* Len of Value-field of TLV */
                    if ( ( pTLV->Type    == NARE_HTONS(DCP_TYPE_CTRL_RESULT) ) &&
                         ( pTLV->Status  == pRCtrl->SetOptions.OptionType[i] ) &&
                         ( pTLV->Result  == DCP_RESULT_OK                    ))
                    {
                        /* switch to next TLV */
                        if ( Len & 1 ) Len++; /* each TLV is even in size ! */
                        pTLV = (NARE_DCP_SET_RSP_TLV_PTR_TYPE) (((NARE_DCP_LOWER_MEM_U8_PTR_TYPE) pTLV) + sizeof(NARE_DCP_TL_TYPE) + Len);
                    }
                    else
                    {
                        NARE_PROGRAM_TRACE_05(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Set failed (T:0x%X,S:0x%X,R:0x%X)",
                                              pHDB->ThisHandle,
                                              pRCtrl->ThisIdx,
                                              NARE_NTOHS(pTLV->Type),
                                              NARE_NTOHS(pTLV->Status),
                                              pTLV->Result);
                        Response = NARE_RSP_ERR_DCP_SET_FAILED;
                    }
                    i++;
                }
                if ( Response == NARE_RSP_OK )
                {
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Set successful.",pHDB->ThisHandle,pRCtrl->ThisIdx);

                    /* copy cfg IP from RQB to res. now it is (or it already was if we dont needed */
                    /* to set the ip. So we dont need to check if we just set the ip               */

                    NARE_COPY_IP_ADDR_UR_UR(&pRCtrlUser->pRQB->args.ResConf.Res.IPAddr,
                                            &pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr);
                    NARE_COPY_IP_ADDR_UR_UR(&pRCtrlUser->pRQB->args.ResConf.Res.SubMask,
                                            &pRCtrlUser->pRQB->args.ResConf.Cfg.SubMask);
                    NARE_COPY_IP_ADDR_UR_UR(&pRCtrlUser->pRQB->args.ResConf.Res.DefRouter,
                                            &pRCtrlUser->pRQB->args.ResConf.Cfg.DefRouter);
                }
            }
            else
            {
                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP[%X:%d]-Set failed. Invalid OptCnt(0x%X))",pHDB->ThisHandle,pRCtrl->ThisIdx,pRQB->args.Set.OptCnt);
                Response = NARE_RSP_ERR_DCP_SET_FAILED;
            }
            break;

        case DCP_RSP_ERR_NO_SERVICE:
        case DCP_RSP_ERR_PROTOCOL:
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Set failed (DCP-Resp: 0x%X)",pHDB->ThisHandle,pRCtrl->ThisIdx,NARE_DCP_RQB_GET_RESPONSE(pRQB));
            Response = NARE_RSP_ERR_DCP_SET_FAILED;
            break;

        case DCP_RSP_ERR_TIMEOUT:
            /* ---------------------------------------------*/
            /* Timeout.We retry if configured.              */
            /* ---------------------------------------------*/
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"DCP[%X:%d]-ERROR: Set timed out",pHDB->ThisHandle,pRCtrl->ThisIdx);
            if (( pRCtrl->RetryCnt) && ( ! pRCtrl->pRCtrl->DoCancel ))
            {
                pRCtrl->RetryCnt--;
                /* set the right handle and refill parameters   */
                /* changed.                                     */
                NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
                pRQB->args.Set.OptCnt    = pRCtrl->SetOptions.OptionCnt;
/*AP01565411    pRQB->args.Set.TrStart   = LSA_FALSE;
                pRQB->args.Set.TrEnd     = LSA_TRUE;
removed */
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"DCP[%X:%d]-Set retry request.",pHDB->ThisHandle,pRCtrl->ThisIdx);
                NARE_DCP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
                CallStateMachine = LSA_FALSE;
            }
            else
            {
                Response = NARE_RSP_ERR_DCP_SET_TIMEOUT;
            }
            break;

        default:
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP[%X:%d]-ERROR: Set LL (0x%X)",pHDB->ThisHandle,pRCtrl->ThisIdx,NARE_DCP_RQB_GET_RESPONSE(pRQB));
            Response = NARE_RSP_ERR_LL;
            break;
    }
    if (CallStateMachine)
    {
        /* release the RQB and RCTRL-Block */
        NARE_DCPReleaseRCTRL(pHDB,pRCtrl);
        NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Set.ReqPtr,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Set.RspPtr,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        /* call the main statemachine     */
        NARE_UserStateMachine(pRCtrlUser,Response,LSA_TRUE);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPSetDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPHelloDone                           +*/
/*+  Input                      NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: HELLO done handling.                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPHelloDone(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_DCP_PTR_TYPE        pHDB)
{
    NARE_RSP                       Response;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPHelloDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);

    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_HELLO done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_DCP_RQB_GET_RESPONSE(pRQB));
    NARE_ASSERT_NULL_PTR(pHDB->Hello.pHDB);
    NARE_ASSERT_NULL_PTR(pHDB->Hello.pHDB->Hello.pRQB);
    switch (NARE_DCP_RQB_GET_RESPONSE(pRQB))
    {
        case DCP_RSP_OK:
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"DCP HELLO successful!");
            break;

        default:
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-ERROR: Set LL (0x%X)",NARE_DCP_RQB_GET_RESPONSE(pRQB));
            Response = NARE_RSP_ERR_LL;
            break;
    }
    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Finishing HELLO-request.");
    NAREUserRequestFinish(pHDB->Hello.pHDB,pHDB->Hello.pHDB->Hello.pRQB,Response);
    pHDB->Hello.Status             = NARE_DCP_HELLO_IDLE;
    pHDB->Hello.pHDB->Hello.Status = NARE_USR_HELLO_IDLE;
    pHDB->Hello.pHDB->Hello.pRQB   = LSA_NULL;
    pHDB->Hello.pHDB               = LSA_NULL;

    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPHelloDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPNotifyDone                          +*/
/*+  Input                      NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: NOTIFY done handling.                                     +*/
/*+               Check if we should NOTIFY to the user. If so we NOTIFY if +*/
/*+               a NOTIFY resource is present, the TLV is valid and this   +*/
/*+               is the first notification for this identify request.      +*/
/*+               (We only notify once per identify!)                       +*/
/*+                                                                         +*/
/*+               We only notify if the Device is already setup!            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPNotifyDone(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_DCP_PTR_TYPE        pHDB)
{
    NARE_DCP_RCTRL_PTR_TYPE       pRCtrl;
    NARE_USER_RCTRL_PTR_TYPE      pRCtrlUser;
    NARE_RSP                      Response;
    LSA_UINT16                    RetVal;
    NARE_HDB_USER_PTR_TYPE        pHDBUser;
    NARE_UPPER_RQB_PTR_TYPE       pRQBUser;
    LSA_BOOL                      Notify;
    NARE_ID_STORAGE_TYPE          remIDs;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPNotifyDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: DCP_OPC_NOTIFY done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_DCP_RQB_GET_RESPONSE(pRQB));

    switch (pHDB->State)
    {
        case NARE_HANDLE_STATE_OPEN:
            switch (NARE_DCP_RQB_GET_RESPONSE(pRQB))
            {
                case DCP_RSP_OK:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"DCP NOTIFY occured (ID: %d)!",pRQB->args.Notify.NotifyId);
                    /* NotifyID is used as RCtrl Idx */
                    pRCtrl = NARE_DCPGetRCTRLByIdx(pHDB,pRQB->args.Notify.NotifyId);
                    NARE_ASSERT_NULL_PTR(pRCtrl);
                    pRCtrlUser = pRCtrl->pRCtrl;
                    NARE_ASSERT_NULL_PTR(pRCtrlUser);
                    /* Pointer to UserHDB */
                    pHDBUser = NAREGetUserHDBFromHandle(NARE_RQB_GET_HANDLE(pRCtrlUser->pRQB));
                    NARE_ASSERT_NULL_PTR(pHDBUser);
                    /* If we should notify (should be!) and we dont used an ALIAS and   */
                    /* a notify RQB is present and this is ther first indication.       */
                    if (( pRCtrlUser->pRQB->args.ResConf.Notify ) && /* should be */
                        ( ! pRCtrl->NotifyCnt ) &&
                        ( ! LSA_HOST_PTR_ARE_EQUAL(pHDBUser->NotifyReq.pBottom, LSA_NULL)))
                    {
                        LSA_UINT32 DCPStatioNameState;

                        /* Get pointer to NOTIFY RQB */
                        pRQBUser = pHDBUser->NotifyReq.pBottom;
                        if ( NARE_DCPAnalyseIdentifyRspTLV(pRQB->args.Notify.NodePtr,
                                                           pRQBUser->args.Notify.Res.MACAddr,
                                                           pRQBUser->args.Notify.Res.IPAddr,
                                                           pRQBUser->args.Notify.Res.SubMask,
                                                           pRQBUser->args.Notify.Res.DefRouter,
                                                           (NARE_UPPER_MEM_U8_PTR_TYPE)pRCtrlUser->pRQB->args.ResConf.pStationName,
                                                           pRCtrlUser->pRQB->args.ResConf.StationNameLen,
                                                           &DCPStatioNameState,
                                                           &remIDs) )
                        {
                            NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify1. IP: %d.%d.%d.%d",
                                                  pHDB->ThisHandle,
                                                  pRCtrl->ThisIdx,
                                                  pRQBUser->args.Notify.Res.IPAddr[0],
                                                  pRQBUser->args.Notify.Res.IPAddr[1],
                                                  pRQBUser->args.Notify.Res.IPAddr[2],
                                                  pRQBUser->args.Notify.Res.IPAddr[3]);

                            NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify2. SN: %d.%d.%d.%d",
                                                  pHDB->ThisHandle,
                                                  pRCtrl->ThisIdx,
                                                  pRQBUser->args.Notify.Res.SubMask[0],
                                                  pRQBUser->args.Notify.Res.SubMask[1],
                                                  pRQBUser->args.Notify.Res.SubMask[2],
                                                  pRQBUser->args.Notify.Res.SubMask[3]);

                            NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify3. DR: %d.%d.%d.%d",
                                                  pHDB->ThisHandle,
                                                  pRCtrl->ThisIdx,
                                                  pRQBUser->args.Notify.Res.DefRouter[0],
                                                  pRQBUser->args.Notify.Res.DefRouter[1],
                                                  pRQBUser->args.Notify.Res.DefRouter[2],
                                                  pRQBUser->args.Notify.Res.DefRouter[3]);

                            NARE_PROGRAM_TRACE_08(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify4. MAC: %02X-%02X-%02X-%02X-%02X-%02X",
                                                  pHDB->ThisHandle,
                                                  pRCtrl->ThisIdx,
                                                  pRQBUser->args.Notify.Res.MACAddr[0],
                                                  pRQBUser->args.Notify.Res.MACAddr[1],
                                                  pRQBUser->args.Notify.Res.MACAddr[2],
                                                  pRQBUser->args.Notify.Res.MACAddr[3],
                                                  pRQBUser->args.Notify.Res.MACAddr[4],
                                                  pRQBUser->args.Notify.Res.MACAddr[5]);

                            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify4. AliasUsed: %d",
                                                  pHDB->ThisHandle,
                                                  pRCtrl->ThisIdx,
                                                  pRCtrl->AliasUsed);

                            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify5. StationOK: %d",
                                                  pHDB->ThisHandle,
                                                  pRCtrl->ThisIdx,
                                                  DCPStatioNameState);
                            /* Now check if Parameters equal to configured ones. Only possible with  */
                            /* Mode =  NARE_MODE_CONFIG_IP_FIXED                                     */
                            if ( DCPStatioNameState == NARE_RCTRL_DCP_STATION_NAME_SET_EQUAL )
                            {
                                Notify = LSA_TRUE;
                                if ( pRCtrlUser->pRQB->args.ResConf.Mode == NARE_MODE_CONFIG_IP_FIXED )
                                {
                                    if ( NARE_CheckIpSuiteIdentically(&pRCtrlUser->pRQB->args.ResConf.Cfg,&pRQBUser->args.Notify.Res))
                                    {
                                        NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify params equal to resolve_config!",
                                                                                                pHDB->ThisHandle,pRCtrl->ThisIdx);
                                    }
                                    else
                                    {
                                        NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify params NOT equal to resolve_config!",
                                                                                                pHDB->ThisHandle,pRCtrl->ThisIdx);
                                        Notify = LSA_FALSE;
                                    }
                                }
                                /* RQ 856337: check whether the dev, vendor or instance must be checked as well */
                                /* RQ 951843: check this only if previous check was ok                          */
                                if((pRCtrlUser->pRQB->args.ResConf.ExtendedMode  != NARE_EXT_MODE_LEGACY) && Notify)
                                {
                                    Notify = LSA_FALSE;
                                    if(remIDs.Foundstate & DEV_VENDOR_SET )
                                    {   /* device and vendor found */
                                        if((pRCtrlUser->pRQB->args.ResConf.VendorID == remIDs.VendorID)  &&     /* VendorID match */
                                           (pRCtrlUser->pRQB->args.ResConf.DeviceID == remIDs.DeviceID))        /* deviceID match */
                                        {
                                            if( remIDs.Foundstate & INSTANCE_SET)             /* instance present?                */
                                            {
                                                if(pRCtrlUser->pRQB->args.ResConf.InstanceID == remIDs.InstanceID)
                                                {
                                                     Notify = LSA_TRUE; /* vendor and dev and inst -ID match */
                                                }
                                            }
                                            else
                                            {
                                                 Notify = LSA_TRUE;    /* vendor and dev match  no inst-ID */
                                            }
                                        }
                                    }
                                }
                                if ( Notify )
                                {
                                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify to user!",
                                                                                                pHDB->ThisHandle,pRCtrl->ThisIdx);
                                    NARE_RQB_REM_BLOCK_BOTTOM(pHDBUser->NotifyReq.pBottom,pHDBUser->NotifyReq.pTop,pRQBUser);
                                    NARE_ASSERT_NULL_PTR(pRQBUser);
                                    /* Copy NotifyID from RESOLVE_CONFIG-Request */
                                    pRQBUser->args.Notify.NotifyID = pRCtrlUser->pRQB->args.ResConf.NotifyID;
                                    NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Finishing NOTIFY-request.");
                                    NAREUserRequestFinish(pHDBUser,pRQBUser,NARE_RSP_OK);
                                }
                                else
                                {
                                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify discarded!",pHDB->ThisHandle,pRCtrl->ThisIdx);
                                }
                            }
                            else
                            {
                                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP[%X:%d]-Notify response but StationName not set!",pHDB->ThisHandle,pRCtrl->ThisIdx);
                            }
                        }
                        else
                        {
                            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP[%X:%d]-ERROR: No (valid) IP-Dataset within NOTIFY Identify response",pHDB->ThisHandle,pRCtrl->ThisIdx);
                        }
                    }
                    else
                    {
                        NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"DCP[%X:%d]-NOTE: Notify response but can/shall not notify!",pHDB->ThisHandle,pRCtrl->ThisIdx);
                    }
                    pRCtrl->NotifyCnt++; /* we got a notify */
                    break;

                default:
                    NARE_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-NOTIFY ERROR (0x%X)  ",NARE_DCP_RQB_GET_RESPONSE(pRQB));
                    break;
            }
            /* reprovide RQB */
            NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
            NARE_LOWER_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_NOTIFY (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DCPHandle,pRQB);
            NARE_DCP_REQUEST_LOWER(pRQB,pHDB->Params.pSys);
            break;

        default:
            /* channel not open            */
            /* release RQB we are closing  */
            NARE_PROGRAM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-releasing NOTIFY RQB-memory (0x%X,0x%X)!",pRQB->args.Notify.NodePtr,pRQB);
            NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Notify.NodePtr,pHDB->Params.pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
            NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
            pHDB->NotifyDCPCnt--;
            break;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPNotifyDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPRQBCopyLLErrorLocal                 +*/
/*+  Input                      LSA_RQB_LOCAL_ERROR_TYPE  NA...* pLLError   +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copys LL-Error to local structure.                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_DCPRQBCopyLLErrorLocal(
    LSA_RQB_LOCAL_ERROR_TYPE   NARE_DCP_LOWER_RQB_ATTR * pLLError)
{
    /* copy only if something filled in.. */
    if ( pLLError->lsa_component_id != LSA_COMP_ID_UNUSED)
    {
        NARE_MGM_GET_NDB_PTR()->LLError = * pLLError;
    }
    else
    {
        NARE_MGM_GET_NDB_PTR()->LLError.lsa_component_id = LSA_COMP_ID_UNUSED;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_dcp_request_lower_done                 +*/
/*+  Input                      NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for DCP-requests.                        +*/
/*+               Fills Error-Structure in NDB with DPC-Response            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_DCP_LOWER_IN_FCT_ATTR nare_dcp_request_lower_done(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB)
{
    NARE_HDB_DCP_PTR_TYPE   pHDB;

    NARE_ENTER();
    NARE_ASSERT_NULL_PTR(pRQB);
    /* DCP-request always done from an DCP-Channel. !           */
    pHDB     = NAREGetDcpHDBFromHandle(NARE_DCP_RQB_GET_HANDLE(pRQB));
    /* save response from lower layer in global error-structure */
    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        NARE_FATAL(NARE_FATAL_ERR_HANDLE_INVALID);
    }
    /* pHDB is now != LSA_NULL !! */
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : nare_dcp_request_lower_done(pRQB: 0x%X)",pRQB);
    switch (NARE_DCP_RQB_GET_OPCODE(pRQB) )
    {
        case DCP_OPC_OPEN_CHANNEL:
            NARE_DCPOpenChannelDone(pRQB,pHDB);
            break;

        case DCP_OPC_CLOSE_CHANNEL:
            NARE_DCPCloseChannelDone(pRQB,pHDB);
            break;

        case DCP_OPC_IDENTIFY_REQ:
            NARE_DCPIdentifyDone(pRQB,pHDB);
            break;

        case DCP_OPC_SET_REQ:
            NARE_DCPSetDone(pRQB,pHDB);
            break;

        case DCP_OPC_HELLO_REQ:
            NARE_DCPHelloDone(pRQB,pHDB);
            break;

        case DCP_OPC_NOTIFY:
            NARE_DCPNotifyDone(pRQB,pHDB);
            break;

        default:
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: nare_dcp_request_lower_done()");
    NARE_EXIT();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPOpenChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       NAREHandle            +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NAREHandle           : NARE DCP-Channel handle                         +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to DCP. The Request will be   +*/
/*+               finished by calling the DCP-Callbackfunktion.             +*/
/*+               nare_dcp_request_lower_done() if NARE_RSP_OK.             +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB;
    NARE_RSP                     Result;
    LSA_USER_ID_TYPE             UserId;

    NARE_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPOpenChannel(NAREHandle: 0x%X)",NAREHandle);
    Result = NARE_RSP_OK;
    UserId.uvar32 = 0;
    NARE_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_OPEN_CHANNEL);
        pRQB->args.channel.handle_upper  = NAREHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.dcp_request_upper_done_ptr = nare_dcp_request_lower_done;
        NARE_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",NAREHandle,pRQB);
        NARE_DCP_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating RQB memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPOpenChannel. Rsp: 0x%X",Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPCloseChannel                        +*/
/*+  Input                      LSA_HANDLE_TYPE       DCPHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+                             NARE_RSP_ERR_RESOURCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  DCPHandle            : DCP handle to close                             +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to DCP. The Request will be  +*/
/*+               finished by calling the DCP-Callbackfunktion.             +*/
/*+               nare_dcp_request_lower_done()                             +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPCloseChannel(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys)
{
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB;
    NARE_RSP                     Result;
    LSA_USER_ID_TYPE             UserId;

    NARE_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPCloseChannel(DCPHandle: 0x%X)",DCPHandle);
    Result        = NARE_RSP_OK;
    UserId.uvar32 = 0;
    NARE_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_CLOSE_CHANNEL);
        NARE_DCP_RQB_SET_HANDLE(pRQB,DCPHandle);
        NARE_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_CLOSE_CHANNEL (DCPHandle: 0x%X,pRQB: 0x%X)",DCPHandle,pRQB);
        NARE_DCP_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating RQB memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPCloseChannel()");
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPIdentifyRQBSetup                    +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                             NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_UPPER_MEM_U8_PTR_TYPE   pStationName   +*/
/*+                             LSA_UINT8                    StationNameLen +*/
/*+                             LSA_BOOL                     Alias          +*/
/*+                             LSA_BOOL                     Notify         +*/
/*+                             LSA_UINT32                   NotifyID       +*/
/*+                             LSA_UINT16                   Tolerance      +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pRQB                 : DCP Identiy-RQB filled with FilterPtr,NodePtr   +*/
/*+  pStatioName          : Pointer to StationName to Identify              +*/
/*+  StationNameLen       : Len of StationName in Bytes                     +*/
/*+  Alias                : LSA_TRUE :Alias name                            +*/
/*+                         LSA_FALSE:Station name                          +*/
/*+  Notify               : LSA_TRUE :Notify identfiy responses             +*/
/*+                         LSA_FALSE:dont Notify                           +*/
/*+  NotifyID             : NotifyID (only needed if Notify=TRUE)           +*/
/*+  Tolerance            : Timeout of DCP-Identify in ms. if 0, 1000 is    +*/
/*+                         used (does DCP internal)                        +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup Identify RQB with FilterPtr and NodePtr             +*/
/*+               - assemble FilterTLV from StationName                     +*/
/*+               - convert StationName to lower case                       +*/
/*+               - allocates memory for FilterPtr and NodePtr.             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPIdentifyRQBSetup(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_UPPER_MEM_U8_PTR_TYPE   pStationName,
    LSA_UINT16                   StationNameLen,
    LSA_BOOL                     Alias,
    LSA_BOOL                     Notify,
    LSA_UINT32                   NotifyID,
    LSA_UINT16                   Tolerance)
{
    NARE_RSP                         Result;
    LSA_SYS_PTR_TYPE                 pSys;
    LSA_USER_ID_TYPE                 UserId;
    LSA_UINT16                       RetVal;
    LSA_UINT32                       i;
    NARE_DCP_IDENT_REQ_TLV_PTR_TYPE  pTLV;
    LSA_UINT16                       Size;
    NARE_DCP_LOWER_MEM_PTR_TYPE      pAlloc;

    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    NARE_ASSERT_TRUE(StationNameLen == 0);
    NARE_ASSERT_NULL_PTR(pStationName);

    pRQB->args.Identify.AddrPtr   = LSA_NULL;
    pRQB->args.Identify.FilterCnt = 1;
    pRQB->args.Identify.NodeCnt   = 1;
    pRQB->args.Identify.Faltung   = 1;
    pRQB->args.Identify.Notify    = Notify;
    pRQB->args.Identify.NotifyId  = NotifyID;
    pRQB->args.Identify.Tolerance = Tolerance;
    /* calculate size of Filter TLV. */
    Size = sizeof(NARE_DCP_TL_TYPE) + StationNameLen;
    /* add one byte if odd size      */
    if ( StationNameLen & 1 )
    {
        Size += 1;
    }
    /* allocate FilterPtr memory     */
    NARE_DCP_ALLOC_LOWER_MEM(&pAlloc,UserId,Size,pSys);
    pTLV = (NARE_DCP_IDENT_REQ_TLV_PTR_TYPE) pAlloc;
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pTLV,LSA_NULL) )
    {
        pRQB->args.Identify.FilterPtr = (DCP_USR_BYTE_PTR) pTLV;
        /* allocate NodePtr memory */
        NARE_DCP_ALLOC_LOWER_MEM(&pAlloc,UserId,sizeof(DCP_NODE_IDENTIFY),pSys);
        pRQB->args.Identify.NodePtr = (DCP_USR_NODE_PTR)pAlloc;
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Identify.NodePtr, LSA_NULL) )
        {
            /* Setup FilterPtr-memory */
            if ( Alias )
            {
                pTLV->Type = NARE_HTONS(DCP_TYPE_DEVICE_ALIAS); /* use AliasName as filter */
            }
            else
            {
                pTLV->Type = NARE_HTONS(DCP_TYPE_DEVICE_NAME); /* use StationName as filter */
            }
            pTLV->Len  = NARE_HTONS(StationNameLen);
            for ( i=0; i<StationNameLen; i++)
            {   /* convert to lowercase */
                pTLV->Data.NameOfStation.Name[i] = NARE_TO_LOWER(pStationName[i]);
            }
            /* if odd len, we have to fill a padding byte at end */
            if ( StationNameLen & 1 )
            {
                pTLV->Data.NameOfStation.Name[StationNameLen] = 0;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-memory failed!");
            NARE_DCP_FREE_LOWER_MEM(&RetVal,pTLV,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
            Result = NARE_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPIdentify                            +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                             NARE_DCP_RCTRL_PTR_TYPE      pRCtrl         +*/
/*+                             NARE_UPPER_MEM_PTR_TYPE      pStationName   +*/
/*+                             LSA_UINT8                    StationNameLen +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pRCtrl               : DCP RCTRl-block pointer                         +*/
/*+  pStatioName          : Pointer to StationName to Identify              +*/
/*+  StationNameLen       : Len of StationName in Bytes                     +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Identify Request to DCP.                           +*/
/*+               The Request will be finished be calling the DCP-Callback  +*/
/*+               nare_dcp_request_lower_done() if NARE_RSP_OK.             +*/
/*+                                                                         +*/
/*+               Note: pStationName is given to DCP so dont free this      +*/
/*+                     before the request is finished!!                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPIdentify(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_RCTRL_PTR_TYPE      pRCtrl,
    NARE_UPPER_MEM_PTR_TYPE      pStationName,
    LSA_UINT8                    StationNameLen)
{
    NARE_DCP_LOWER_RQB_PTR_TYPE   pRQB;
    LSA_SYS_PTR_TYPE              pSys;
    NARE_RSP                      Result;
    LSA_USER_ID_TYPE              UserId;
    LSA_UINT16                    RetVal;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPIdentify(pHandle: 0x%X,pName: 0x%X, Len: 0x%X)",pHDB,pStationName,StationNameLen);
    NARE_ASSERT_NULL_PTR(pRCtrl);
    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    NARE_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {   /* we user User-ID as NotifyID */
        Result = NARE_DCPIdentifyRQBSetup(pHDB,
                                          pRQB,
                                          (NARE_UPPER_MEM_U8_PTR_TYPE)pStationName,
                                          StationNameLen,
                                          LSA_FALSE,
                                          pRCtrl->pRCtrl->pRQB->args.ResConf.Notify,
                                          pRCtrl->ThisIdx,
                                          pRCtrl->pRCtrl->pRQB->args.ResConf.Tolerance);
        if ( Result == NARE_RSP_OK)
        {
            NARE_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_IDENTIFY_REQ);
            NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
            /* we user User-ID for RCtrl-identification */
            NARE_DCP_RQB_SET_USERID_UVAR32(pRQB,pRCtrl->ThisIdx);
            NARE_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_IDENTIFY (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DCPHandle,pRQB);
            NARE_DCP_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPIdentify()");
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPSetRQBSetup                         +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                             NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB           +*/
/*+                             NARE_DCP_RCTRL_PTR_TYPE      pRCtrl         +*/
/*+                             NARE_UPPER_RQB_MAC_PTR_TYPE  pMAC           +*/
/*+                             NARE_UPPER_RQB_IP_PTR_TYPE   pIPAddr        +*/
/*+                             NARE_UPPER_RQB_IP_PTR_TYPE   pSubnetMask    +*/
/*+                             NARE_UPPER_RQB_IP_PTR_TYPE   pDefRouter     +*/
/*+                             NARE_UPPER_MEM_U8_PTR_TYPE   pStationName   +*/
/*+                             LSA_UINT32                   StationNameLen +*/
/*+                             LSA_UINT16                   SetIPRemanent  +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pRQB                 : DCP Identiy-RQB filled with FilterPtr,NodePtr   +*/
/*+  pRCtrl               : DCP RCTRl-block pointer                         +*/
/*+  pMAC                 : MAC - Address of IO-Device                      +*/
/*+  pIPAddr              : Pointer to IP address                           +*/
/*+                         LSA_NULL if not used                            +*/
/*+  pSubnetMask          : Pointer to SubnetMask IP address                +*/
/*+  pDefRouter           : Pointer to DefaultRouter IP address             +*/
/*+  pStationName         : Pointer to optional StationName                 +*/
/*+                         LSA_NULL if not used                            +*/
/*+  StationNameLen       : optional StationNameLen                         +*/
/*+  SetIPRemanent        : Set IP-Parameters remanent?                     +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup SET RQB for setting IP-Parameters and/or StationName+*/
/*+               - Fill ReqPtr with FilterTLV                              +*/
/*+               - Set  RspPtr                                             +*/
/*+               - Set all other Set-parameters                            +*/
/*+                                                                         +*/
/*+           If IP-Parameters should not be set, set pIPAddr to LSA_NULL   +*/
/*+           If Stationname should not be set, set pStationName to LSA_NULL+*/
/*+           and StationNameLen to 0                                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPSetRQBSetup(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_DCP_RCTRL_PTR_TYPE      pRCtrl,
    NARE_UPPER_RQB_MAC_PTR_TYPE  pMAC,
    NARE_UPPER_RQB_IP_PTR_TYPE   pIPAddr,
    NARE_UPPER_RQB_IP_PTR_TYPE   pSubnetMask,
    NARE_UPPER_RQB_IP_PTR_TYPE   pDefRouter,
    NARE_UPPER_MEM_U8_PTR_TYPE   pStationName,
    LSA_UINT32                   StationNameLen,
    LSA_BOOL                     SetIPRemanent)
{
    NARE_RSP                         Result;
    LSA_SYS_PTR_TYPE                 pSys;
    LSA_USER_ID_TYPE                 UserId;
    LSA_UINT16                       RetVal;
    LSA_UINT32                       i;
    NARE_DCP_LOWER_MEM_PTR_TYPE      pLower;
    NARE_DCP_SET_REQ_TLV_PTR_TYPE    pTLV;
    LSA_UINT16                       Size;
    NARE_DCP_LOWER_MEM_PTR_TYPE      pAlloc;

    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    /* we convert the pointer and dont copy the content! */
    NARE_DCP_UPPER_RQB_TO_LOWER_MEM(&pLower,pMAC,pSys);
    if ( LSA_HOST_PTR_ARE_EQUAL(pLower, LSA_NULL) )
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"NARE_DCP_UPPER_RQB_TO_LOWER_MEM returned NULL-Ptr.");
        NARE_FATAL(NARE_FATAL_ERR_NULL_PTR);
    }
    pRQB->args.Set.AddrPtr   = (DCP_USR_BYTE_PTR) pLower;  /* pointer to MAC */
    pRQB->args.Set.OptCnt    = 0;     /* number of Options */
/*  pRQB->args.Set.TrStart   = LSA_FALSE;	//AP01565411    
    pRQB->args.Set.TrEnd     = LSA_TRUE;	//AP01565411  removed */  
    pRCtrl->SetOptions.OptionCnt  = 0;
    /* allocate RspPtr memory */
    NARE_DCP_ALLOC_LOWER_MEM(&pAlloc,UserId,DCP_MAX_RSP_DATA_SIZE,pSys);
    pRQB->args.Set.RspPtr = (DCP_USR_BYTE_PTR)pAlloc;
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Set.RspPtr, LSA_NULL) )
    {
        Size = 0;
        /*------------------------------------------------------------------------*/
        /* Calculate size of TLV and allocate memory                              */
        /*------------------------------------------------------------------------*/
        /* Setup TCP/IP-TLV if present */
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pIPAddr,LSA_NULL) )
        {
            Size += sizeof(NARE_DCP_TLS_TYPE) + sizeof(NARE_DCP_DS_TCPIP_TYPE);  /* even ! */
        }
        /* Setup StationNameTLV if present */
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pStationName,LSA_NULL) && StationNameLen )
        {
            Size += (LSA_UINT16)(sizeof(NARE_DCP_TLS_TYPE) + StationNameLen);
            /* add one byte if odd size       */
            if ( StationNameLen & 1 ) Size += 1;
        }
        NARE_DCP_ALLOC_LOWER_MEM(&pAlloc,UserId,Size,pSys);
        pTLV = (NARE_DCP_SET_REQ_TLV_PTR_TYPE)pAlloc;
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pTLV, LSA_NULL) )
        {   /*--------------------------------------------------------------------*/
            /* Fill TLVs                                                          */
            /*--------------------------------------------------------------------*/
            pRQB->args.Set.ReqPtr = (DCP_USR_BYTE_PTR) pTLV; /* start of TLV chain*/
            /* Setup TCP/IP-TLV if present */
            if ( ! LSA_HOST_PTR_ARE_EQUAL(pIPAddr,LSA_NULL) )
            {
                /* Setup TLV-memory */
                pTLV->Type   = NARE_HTONS(DCP_TYPE_TCP_IP);
                pTLV->Len    = NARE_HTONS((sizeof(NARE_DCP_DS_TCPIP_TYPE) + sizeof (pTLV->Status)));
                if ( SetIPRemanent )
                {
                    pTLV->Status = NARE_HTONS(NARE_DCP_STATUS_REMANENT);     /* remanent */
                }
                else
                {
                    pTLV->Status = NARE_HTONS(NARE_DCP_STATUS_NOT_REMANENT); /* not remanent */
                }
                /* setup IP-Params */
                for ( i= 0; i<NARE_IP_ADDR_SIZE; i++)
                {
                    pTLV->Data.TcpIp.IpAddr[i]        =  (*pIPAddr)[i];
                    pTLV->Data.TcpIp.SubnetMask[i]    =  (*pSubnetMask)[i];
                    pTLV->Data.TcpIp.DefaultRouter[i] =  (*pDefRouter)[i];
                }
                pRCtrl->SetOptions.OptionType[pRQB->args.Set.OptCnt] = pTLV->Type;
                pRQB->args.Set.OptCnt++;
                /* advance pTLV-Pointer behind this TLV */
                pTLV = (NARE_DCP_SET_REQ_TLV_PTR_TYPE) (((NARE_DCP_LOWER_MEM_U8_PTR_TYPE) pTLV)
                                                        + sizeof(NARE_DCP_TLS_TYPE)
                                                        + sizeof(NARE_DCP_DS_TCPIP_TYPE));
            }
            /* Setup StationName-TLV if present */
            if ( (! LSA_HOST_PTR_ARE_EQUAL(pStationName,LSA_NULL)) && StationNameLen )
            {   /* Setup TLV-memory */
                pTLV->Type   = NARE_HTONS(DCP_TYPE_DEVICE_NAME);
                pTLV->Len    = NARE_HTONS(StationNameLen + sizeof (pTLV->Status));
                pTLV->Status = NARE_HTONS(NARE_DCP_STATUS_REMANENT); /* remanent */
                for ( i=0; i<StationNameLen; i++)
                {
                    /* dont convert to lower case */
                    pTLV->Data.NameOfStation.Name[i] = pStationName[i];
                }
                /* if odd len, we have to fill a padding byte at end */
                if ( StationNameLen & 1 ) pTLV->Data.NameOfStation.Name[StationNameLen] = 0;
                pRCtrl->SetOptions.OptionType[pRQB->args.Set.OptCnt] = pTLV->Type;
                pRQB->args.Set.OptCnt++;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-memory failed!");
            NARE_DCP_FREE_LOWER_MEM(&RetVal,pRQB->args.Set.RspPtr,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
            Result = NARE_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    pRCtrl->SetOptions.OptionCnt = (LSA_UINT16) pRQB->args.Set.OptCnt;
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPSet                                 +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                             NARE_DCP_RCTRL_PTR_TYPE      pRCtrl         +*/
/*+                             NARE_UPPER_RQB_MAC_PTR_TYPE  pMAC           +*/
/*+                             NARE_UPPER_RQB_IP_PTR_TYPE   pIPAddr        +*/
/*+                             NARE_UPPER_RQB_IP_PTR_TYPE   pSubnetMask    +*/
/*+                             NARE_UPPER_RQB_IP_PTR_TYPE   pDefRouter     +*/
/*+                             NARE_UPPER_MEM_U8_PTR_TYPE   pStationName   +*/
/*+                             LSA_UINT32                   StationNameLen +*/
/*+                             LSA_BOOL                     SetIPRemanent  +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pRCtrl               : DCP RCTRl-block pointer                         +*/
/*+  pMAC                 : MAC - Address of IO-Device                      +*/
/*+  pIPAddr              : Pointer to IP address                           +*/
/*+                         LSA_NULL if not used                            +*/
/*+  pSubnetMask          : Pointer to SubnetMask IP address                +*/
/*+  pDefRouter           : Pointer to DefaultRouter IP address             +*/
/*+  pStationName         : Pointer to optional StationName                 +*/
/*+                         LSA_NULL if not used                            +*/
/*+  StationNameLen       : optional StationNameLen                         +*/
/*+  SetIPRemanent        : Set IP-Parameters remanent?                     +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Set Request to DCP                                 +*/
/*+                                                                         +*/
/*+               The Request will be finished be calling the DCP-Callback  +*/
/*+               nare_dcp_request_lower_done() if NARE_RSP_OK.             +*/
/*+                                                                         +*/
/*+               We set the IP-Parameters for Database 7 / Dataset 2       +*/
/*+               The parameters will be set to "not remanent"              +*/
/*+                                                                         +*/
/*+               Note: pStationName is given to DCP so dont free this      +*/
/*+                     before the request is finished!!                    +*/
/*+                                                                         +*/
/*+           If IP-Parameters should not be set, set pIPAddr to LSA_NULL   +*/
/*+           If Stationname should not be set, set pStationName to LSA_NULL+*/
/*+           and StationNameLen to 0                                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPSet(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_RCTRL_PTR_TYPE      pRCtrl,
    NARE_UPPER_RQB_MAC_PTR_TYPE  pMAC,
    NARE_UPPER_RQB_IP_PTR_TYPE   pIPAddr,
    NARE_UPPER_RQB_IP_PTR_TYPE   pSubnetMask,
    NARE_UPPER_RQB_IP_PTR_TYPE   pDefRouter,
    NARE_UPPER_MEM_U8_PTR_TYPE   pStationName,
    LSA_UINT32                   StationNameLen,
    LSA_BOOL                     SetIPRemanent)
{
    NARE_DCP_LOWER_RQB_PTR_TYPE   pRQB;
    LSA_SYS_PTR_TYPE              pSys;
    NARE_RSP                      Result;
    LSA_USER_ID_TYPE              UserId;
    LSA_UINT16                    RetVal;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPSet(pHandle: 0x%X)",pHDB);
    NARE_ASSERT_NULL_PTR(pRCtrl);
    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    NARE_DCP_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DCP_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_SET_REQ);
        NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
        /* we user User-ID for RCtrl-identification */
        NARE_DCP_RQB_SET_USERID_UVAR32(pRQB,pRCtrl->ThisIdx);
        Result = NARE_DCPSetRQBSetup(pHDB,pRQB,pRCtrl,pMAC,pIPAddr,pSubnetMask,pDefRouter,pStationName,StationNameLen,SetIPRemanent);
        if ( Result == NARE_RSP_OK)
        {
            if (! LSA_HOST_PTR_ARE_EQUAL(pIPAddr, LSA_NULL))
            {
                NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Doing SET request1 (Handle: 0x%X,pRQB: %Xh) IP: %d.%d.%d.%d",
                                      pHDB->ThisHandle,
                                      pRQB,
                                      (*pIPAddr)[0],
                                      (*pIPAddr)[1],
                                      (*pIPAddr)[2],
                                      (*pIPAddr)[3]);

                NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Doing SET request2 (Handle: 0x%X,pRQB: %Xh) SN: %d.%d.%d.%d",
                                      pHDB->ThisHandle,
                                      pRQB,
                                      (*pSubnetMask)[0],
                                      (*pSubnetMask)[1],
                                      (*pSubnetMask)[2],
                                      (*pSubnetMask)[3]);

                NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Doing SET request3 (Handle: 0x%X,pRQB: %Xh) RT: %d.%d.%d.%d",
                                      pHDB->ThisHandle,
                                      pRQB,
                                      (*pDefRouter)[0],
                                      (*pDefRouter)[1],
                                      (*pDefRouter)[2],
                                      (*pDefRouter)[3]);
            }
            if (! LSA_HOST_PTR_ARE_EQUAL(pStationName, LSA_NULL))
            {
                NARE_PROGRAM_TRACE_04(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"DCP-Doing SET request4 (Handle: 0x%X,pRQB: %Xh) pStationName: %X, Len: %d",
                                      pHDB->ThisHandle,
                                      pRQB,
                                      pStationName,
                                      StationNameLen);
            }
            NARE_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_SET (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DCPHandle,pRQB);
            NARE_DCP_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            NARE_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating lower-memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPSet()");
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPHello                               +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                             NARE_HDB_USER_PTR_TYPE       pHDBUser       +*/
/*+                             LSA_UINT32                   SendInterval   +*/
/*+                             LSA_UINT32                   SendCnt        +*/
/*+                             LSA_UINT32                   Delay          +*/
/*+                             NARE_UPPER_MEM_PTR_TYPE      pTLV           +*/
/*+                             LSA_UINT32                   OptCnt         +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pHDBUser             : Pointer to USER-HDB                             +*/
/*+  SendInterval         : Sendinterval in 10ms ticks                      +*/
/*+  SendCnt              : Number of sends                                 +*/
/*+  Delay                : Delay of first send in 10ms ticks               +*/
/*+  pTLV                 : Pointer to HELLO TLV chain                      +*/
/*+  OptCnt               : OptionCount                                     +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_SEQUENCE                           +*/
/*+                         NARE_RSP_ERR_PARAM                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues HELLO Request to DCP                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPHello(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_HDB_USER_PTR_TYPE       pHDBUser,
    LSA_UINT32                   SendInterval,
    LSA_UINT32                   SendCnt,
    LSA_UINT32                   Delay,
    NARE_UPPER_MEM_PTR_TYPE      pTLV,
    LSA_UINT32                   OptCnt)
{
    NARE_DCP_LOWER_RQB_PTR_TYPE   pRQB;
    LSA_SYS_PTR_TYPE              pSys;
    NARE_RSP                      Result;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPHello(pHandle: 0x%X)",pHDB);
    NARE_ASSERT_NULL_PTR(pHDBUser);
    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    /* only if no HELLO in progress */
    if ( pHDB->Hello.Status == NARE_DCP_HELLO_IDLE )
    {
        pRQB   = pHDB->Hello.pRQB;  /* preallocated! */
        NARE_ASSERT_NULL_PTR(pRQB);
        NARE_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_HELLO_REQ);
        NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
        NARE_DCP_UPPER_TO_LOWER_MEM(&pHDB->Hello.pTLV,pTLV,pSys);
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pTLV, LSA_NULL) )
        {
            pRQB->args.Hello.AddrPtr = LSA_NULL;
            pRQB->args.Hello.ReqPtr  = (DCP_USR_BYTE_PTR)pHDB->Hello.pTLV;
            pRQB->args.Hello.OptCnt  = (LSA_INT) OptCnt;
            pRQB->args.Hello.Time    = (LSA_INT) SendInterval;
            pRQB->args.Hello.Delay   = (LSA_INT) Delay;
            pRQB->args.Hello.SendCnt = (LSA_INT) SendCnt;
            pHDB->Hello.pHDB         = pHDBUser;
            pHDB->Hello.Status       = NARE_DCP_HELLO_IN_PROGRESS;
            NARE_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_HELLO (start) (Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DCPHandle,pRQB);
            NARE_DCP_REQUEST_LOWER(pRQB,pSys);
        }
        else
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"TLVptr conversion failed!");
            Result = NARE_RSP_ERR_RESOURCE;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"HELLO request already in progress!");
        Result = NARE_RSP_ERR_SEQUENCE;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPHello()");
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_DCPHelloStop                           +*/
/*+  Input/Output          :    NARE_HDB_DCP_PTR_TYPE        pHDB           +*/
/*+                             NARE_HDB_USER_PTR_TYPE       pHDBUser       +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to DCP-HDB                              +*/
/*+  pHDBUser             : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_SEQUENCE                           +*/
/*+                         NARE_RSP_ERR_PARAM                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues HELLO Request to DCP to Stop sending HELLO         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPHelloStop(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_HDB_USER_PTR_TYPE       pHDBUser)
{
    NARE_DCP_LOWER_RQB_PTR_TYPE   pRQB;
    LSA_SYS_PTR_TYPE              pSys;
    NARE_RSP                      Result;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_DCPHelloStop(pHandle: 0x%X)",pHDB);
    NARE_ASSERT_NULL_PTR(pHDBUser);
    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    /* only if no HELLO in progress */
    if ( pHDB->Hello.Status == NARE_DCP_HELLO_IDLE )
    {
        pRQB   = pHDB->Hello.pRQB; /* preallocated! */
        NARE_ASSERT_NULL_PTR(pRQB);
        NARE_DCP_RQB_SET_OPCODE(pRQB,DCP_OPC_HELLO_REQ);
        NARE_DCP_RQB_SET_HANDLE(pRQB,pHDB->DCPHandle);
        pRQB->args.Hello.AddrPtr = LSA_NULL;
        pRQB->args.Hello.ReqPtr  = LSA_NULL;
        pRQB->args.Hello.OptCnt  = 0;
        pRQB->args.Hello.Time    = 0;
        pRQB->args.Hello.SendCnt = 0;
        pHDB->Hello.pHDB         = pHDBUser; /* save User channel who issued this request */
        pHDB->Hello.Status       = NARE_DCP_HELLO_IN_PROGRESS;
        NARE_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: DCP-Request: DCP_OPC_HELLO (stop)(Handle: 0x%X,EDDHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DCPHandle,pRQB);
        NARE_DCP_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"HELLO request already in progress!");
        Result = NARE_RSP_ERR_SEQUENCE;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_DCPHelloStop()");
    return(Result);
}

/*****************************************************************************/
/*  end of file NARE_DCP.C                                                    */
/*****************************************************************************/

