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
/*  F i l e               &F: nare_sck.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE SOCK-LowerLayer interface functions         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  6
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_SOCK */

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


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

#ifndef NARE_CFG_NO_SOCK

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKChannelDone                        +*/
/*+  Input                      NARE_HDB_SOCK_PTR_TYPE       pHDB           +*/
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
/*+               SOCK-Channels. This function is called after a lower      +*/
/*+               layer called back the NARE (e.g. after SOCK finished his  +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_SOCKChannelDone(
    NARE_HDB_SOCK_PTR_TYPE      pHDB,
    NARE_RSP                    Response)
{
    NARE_UPPER_RQB_PTR_TYPE     pRQB;
    LSA_UINT16                  PathStat;
    LSA_UINT32                  Finish;
    LSA_VOID                    LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR, Cbf) (NARE_UPPER_RQB_PTR_TYPE pRQB);
    LSA_SYS_PTR_TYPE            pSys;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKChannelDone(pHDB: 0x%X, Response: 0x%X)",pHDB,Response);
    Finish = NOT_FINISHED;      /* dont finish request by default               */
    pRQB         = pHDB->pOCReq;
    pHDB->pOCReq = LSA_NULL;    /* we finish all requests here. so reset pOCReq */
                                /* to signal no more request pending.           */
    Cbf    = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
    pSys   = pHDB->Params.pSys;
    NARE_ASSERT_NULL_PTR(pRQB);
    switch (pHDB->State)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Open Channel request.                        */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_OPEN_SOCK_RUNNING:
            if ( Response == NARE_RSP_OK)
            {
                /* no more actions needed. Channel is open now. */
                pHDB->State  = NARE_HANDLE_STATE_OPEN;
            }
            else
            {   /* opening of SOCK system channel failed stop working */
                NARE_FATAL(NARE_FATAL_ERR_OPENING_SYSCHANNEL);
            }
            Finish = FINISHED_OK;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Close-Channel request                        */
            /*-------------------------------------------------------------------*/
        case NARE_HANDLE_STATE_CLOSE_SOCK_RUNNING:
            if ( Response == NARE_RSP_OK)
            {
                /* we release the HDB                           */
                NARE_SOCKReleaseHDB(pHDB);
            }
            else
            {
                /* is this a fatal error ?                      */
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Other states not expected within this function.                   */
            /*-------------------------------------------------------------------*/
        default:
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */
    if ( Finish != NOT_FINISHED )  /* finish the request ? */
    {
        NARE_RQB_SET_RESPONSE(pRQB,Response);
        /* upper - handle already set */
        NARE_UPPER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),Response);
        NARECallCbf(Cbf,pRQB,pSys);
        if(Finish == FINISHED_REL_SYSPTR)
        {   /* release PATH-Info !                          */
            NARE_RELEASE_PATH_INFO(&PathStat,pHDB->Params.pSys,pHDB->Params.pDetail);
            if (PathStat != LSA_RET_OK )
            {
                NARE_FATAL(NARE_FATAL_ERR_RELEASE_PATH_INFO);
            }
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKGetHandleByNIC                     +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    NARE_HDB_SOCK_PTR_TYPE                      +*/
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

NARE_HDB_SOCK_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_SOCKGetHandleByNIC(
    LSA_UINT16 NICId)
{
    NARE_HDB_SOCK_PTR_TYPE pHDB;
    LSA_UINT32             i;
    LSA_BOOL               Found;

    i     = NARE_MGM_GET_HSOCK_PTR()->HandleCnt;
    pHDB  = NARE_MGM_GET_HSOCK_PTR()->HDB;
    Found = LSA_FALSE;
    while ( (! Found ) && ( i ) )
    {   /* SOCK-Handle independend of NICId! */
        if ( pHDB->State == NARE_HANDLE_STATE_OPEN )
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
    NICId = 0; /* prevent compiler warning for not accessing */
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKGetFreeHDB                         +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE NARE_LOCAL..pHDBSOCK +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : NARE SOCK  -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free SOCK -HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_SOCK_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBUser)
{

    NARE_RSP                  Status;
    LSA_BOOL                  Found;
    NARE_HDB_SOCK_PTR_TYPE    pHDB;
    LSA_UINT32                DSize;
    LSA_UINT32                i;
    NARE_SOCK_RCTRL_PTR_TYPE  pRCtrl;

    pHDB   = (NARE_MGM_GET_HSOCK_PTR()->HDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKGetFreeHDB(pHDB: 0x%X)",pHDBUser);
    i      = 0;
    Found  = LSA_FALSE;
    Status = NARE_RSP_OK;
    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    while ( (! Found)  &&  ( i < NARE_MGM_GET_HSOCK_PTR()->HandleCnt))
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
        DSize = sizeof(NARE_SOCK_RCTRL_TYPE) * NoOfParallelRQBs;
        /* allocate control-block memory */
        NARE_ALLOC_LOCAL_MEM((NARE_LOCAL_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *)&pHDB->SOCKRCtrl.pTable,DSize);

        if ( LSA_HOST_PTR_ARE_EQUAL(pHDB->SOCKRCtrl.pTable, LSA_NULL) )
        {
            Status = NARE_RSP_ERR_RESOURCE;
            NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"NARE_SOCKGetFreeHDB(). Allocation memory failed.");
        }
        if ( Status == NARE_RSP_OK)
        {   /* ------------------------------------------------------------------*/
            /* Setup RCTRL-Blocks for SOCK                                        */
            /* ------------------------------------------------------------------*/
            /* initialize memory with 0 */
            NARE_MEMSET_LOCAL(pHDB->SOCKRCtrl.pTable,0,DSize);
            /* initialize SOCK RCTRL-Block management  */
            pHDB->SOCKRCtrl.Cnt    = NoOfParallelRQBs;
            pHDB->SOCKRCtrl.Free   = NoOfParallelRQBs;
            pHDB->SOCKRCtrl.Used   = 0;
            pHDB->SOCKRCtrl.pFree.pBottom  = LSA_NULL;
            pHDB->SOCKRCtrl.pFree.pTop     = LSA_NULL;
            pRCtrl = &(* pHDB->SOCKRCtrl.pTable)[0]; /* pointer to first element */
            for ( i=0; i< NoOfParallelRQBs; i++)
            {
                NARE_PUT_BLOCK_TOP(pHDB->SOCKRCtrl.pFree.pBottom,
                                   pHDB->SOCKRCtrl.pFree.pTop,
                                   pRCtrl);
                pRCtrl->InUse   = LSA_FALSE;
                pRCtrl->ThisIdx = (LSA_UINT16)i;
                pRCtrl++;
            }
            NARE_MGM_GET_HSOCK_PTR()->UsedCnt++;
            pHDB->State = NARE_HANDLE_STATE_CLOSED;
            *pHDBUser = pHDB;
        }
    }
    else
    {
        Status = NARE_RSP_ERR_RESOURCE;
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"NARE_SOCKGetFreeHDB(). No more free handles.");
    }
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",Status,*pHDBUser);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKReleaseHDB                         +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE pHDB                 +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBSOCK    : Pointer to User Handle to release                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release SOCK -HDB                                         +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to NARE_HANDLE_STATE_FREE               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKReleaseHDB(
    NARE_HDB_SOCK_PTR_TYPE pHDB)
{
    LSA_UINT16   FreeStat;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKReleaseHDB(pHDB: 0x%X)",pHDB);
    NARE_ASSERT_NULL_PTR(pHDB->SOCKRCtrl.pTable);

    NARE_FREE_LOCAL_MEM(&FreeStat, pHDB->SOCKRCtrl.pTable);
    NARE_MEM_FREE_FAIL_CHECK(FreeStat);
    pHDB->SOCKRCtrl.pTable = LSA_NULL;

    pHDB->SOCKRCtrl.Cnt    = 0;
    pHDB->SOCKRCtrl.Free   = 0;
    pHDB->SOCKRCtrl.Used   = 0;
    pHDB->SOCKRCtrl.pFree.pBottom  = LSA_NULL;
    pHDB->SOCKRCtrl.pFree.pTop     = LSA_NULL;
    pHDB->State = NARE_HANDLE_STATE_FREE;
    NARE_MGM_GET_HSOCK_PTR()->UsedCnt--;
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKReleaseHDB()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKGetRCTRLByIdx                      +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE            pHDB      +*/
/*+                             LSA_UINT32                        Idx       +*/
/*+  Result                :    NARE_SOCK_RCTRL_PTR_TYPE                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK -HDB                                      +*/
/*+  Idx        : Index in RCTRl-block table.                               +*/
/*+                                                                         +*/
/*+  Result     : Pointer to RCTRL-Block                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a SOCK -RCTRL-Block by index              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_SOCK_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_SOCKGetRCTRLByIdx(
    NARE_HDB_SOCK_PTR_TYPE pHDB,
    LSA_UINT32             Idx)
{
    NARE_SOCK_RCTRL_PTR_TYPE  pRCtrl;

    NARE_ASSERT_NULL_PTR(pHDB);
    pRCtrl = LSA_NULL; /* prevent compiler warning with tasking */
    if ( Idx < pHDB->SOCKRCtrl.Cnt )
    {
        pRCtrl = &(*pHDB->SOCKRCtrl.pTable)[Idx];
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Invalid SOCK RCTRL-Index.");
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    return(pRCtrl);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKGetFreeRCTRL                       +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE             pHDB     +*/
/*+  Result                :    NARE_SOCK_RCTRL_PTR_TYPE                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK -HDB                                      +*/
/*+                                                                         +*/
/*+  Result     : Pointer to free RCTRL-Block or LSA_NULL if none free      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free SOCK -RCTRL-Block                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_SOCK_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_SOCKGetFreeRCTRL(
    NARE_HDB_SOCK_PTR_TYPE pHDB)
{
    NARE_SOCK_RCTRL_PTR_TYPE  pRCtrl;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_REM_BLOCK_BOTTOM(pHDB->SOCKRCtrl.pFree.pBottom,
                          pHDB->SOCKRCtrl.pFree.pTop,
                          pRCtrl);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl, LSA_NULL) )
    {
        pHDB->SOCKRCtrl.Used++;
        pHDB->SOCKRCtrl.Free--;
        pRCtrl->InUse = LSA_TRUE;
    }
    return(pRCtrl);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKReleaseRCTRL                       +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE              pHDB    +*/
/*+                             NARE_SOCK_RCTRL_PTR_TYPE            pRCtrl  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK-HDB                                       +*/
/*+  pRCtrl     : Pointer to SOCK-RCTRL-Block to release                    +*/
/*+                                                                         +*/
/*+  Result     : --                                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Releases an SOCK-RCTRL-Block                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKReleaseRCTRL(
    NARE_HDB_SOCK_PTR_TYPE      pHDB,
    NARE_SOCK_RCTRL_PTR_TYPE    pRCtrl)
{
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_ASSERT_NULL_PTR(pRCtrl);
    if ( pRCtrl->InUse )
    {  /* Consistenzcheck validates RCtrlblock. Check Poitner with "ThisIdx" */
        if ( ! LSA_HOST_PTR_ARE_EQUAL(&(*pHDB->SOCKRCtrl.pTable)[pRCtrl->ThisIdx], pRCtrl))
        {   /* unexpected. ThisIdx and Pointer not matching.*/
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
        }
        /* put into free-list */
        NARE_PUT_BLOCK_TOP(pHDB->SOCKRCtrl.pFree.pBottom,
                           pHDB->SOCKRCtrl.pFree.pTop,
                           pRCtrl);
        pHDB->SOCKRCtrl.Used--;
        pHDB->SOCKRCtrl.Free++;
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
/*+  Functionname          :    NARE_SOCKTrigger                            +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE     pHDB             +*/
/*+                             NARE_SOCK_RCTRL_PTR_TYPE   pRCtrl           +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK -HDB                                      +*/
/*+  pRCtrl     : Pointer to SOCK-RCTRL-Block containing User-RCTRL-Block   +*/
/*+               with valid Bstate.                                        +*/
/*+                                                                         +*/
/*+  Result:      NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Depending on the State of the User-RCTRL-Block the        +*/
/*+               neccessary SOCK-Request will be executed.                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKTrigger(
    NARE_HDB_SOCK_PTR_TYPE     pHDB,
    NARE_SOCK_RCTRL_PTR_TYPE   pRCtrl)
{

    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    NARE_RSP                  Status;

    NARE_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKTrigger(pHDB: 0x%X, PRCtrl: 0x%X (Idx: %d))",pHDB,pRCtrl,pRCtrl->ThisIdx);
    Status = NARE_RSP_OK;
    pRCtrlUser = pRCtrl->pRCtrl;
    /* Depending on RState we do the proper SOCK-Request  */
    switch (*pRCtrlUser->pRState)
    {
        case NARE_RCTRL_RSTATE_SOCK_GHBN:
            NARE_PROGRAM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK[%X:%d]-Calling NARE_SOCKGetHostByName().",pHDB->ThisHandle,pRCtrl->ThisIdx);
            NARE_ASSERT_NULL_PTR(pRCtrlUser->pRQB->args.ResConf.pStationName);
            Status = NARE_SOCKGetHostByName(pHDB,
                                            pRCtrl,
                                            (NARE_UPPER_MEM_U8_PTR_TYPE)pRCtrlUser->pRQB->args.ResConf.pStationName,
                                            pRCtrlUser->pRQB->args.ResConf.StationNameLen);
            break;

        default:
            /* unexpected state */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"SOCK[%X:%d]-Unexpected RState (0x%X).",pHDB->ThisHandle,pRCtrl->ThisIdx,*pRCtrlUser->pRState);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKTrigger(), Status: 0x%X",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKCheckAndStartPending               +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE              pHDB    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK -HDB                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RCTRL-Block pending for SOCK. If so check  +*/
/*+               for free SOCK-RCTRL-Blocks. As long as one is free and    +*/
/*+               User-RCTRL-Blocks present start handling this Blocks by   +*/
/*+               doing the SOCK-Request. The type of SOCK-Request depends  +*/
/*+               on the State of the User-RCTRL-Block.                     +*/
/*+                                                                         +*/
/*+               If starting of a SOCK-request fails we finish the request.+*/
/*+                                                                         +*/
/*+               This function should be called after a SOCK-Request was   +*/
/*+               finished, or a new User-RCTRL-Block was queued in SOCK    +*/
/*+               pending-queue.                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKCheckAndStartPending(
    NARE_HDB_SOCK_PTR_TYPE     pHDB)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    NARE_SOCK_RCTRL_PTR_TYPE   pRCtrlSOCK;
    LSA_BOOL                  Ready;
    NARE_RSP                  Status;
    NARE_HDB_USER_PTR_TYPE    pHDBUser;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKCheckAndStartPending(pHDB: 0x%X)",pHDB);
    Status = NARE_RSP_OK;
    Ready  = LSA_FALSE;
    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Checking for pending requests (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
    while (! Ready )
    {
        if ( pHDB->PendRCtrlCnt ) /* User-RCTRL-Blocks pending ? */
        {
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-requests pending (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
            if (pHDB->SOCKRCtrl.Free ) /* Free SOCK-RCTRL-Blocks ? */
            {
                NARE_REM_BLOCK_BOTTOM(pHDB->PendRCtrl.pBottom,
                                      pHDB->PendRCtrl.pTop,
                                      pRCtrlUser);
                /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
                NARE_ASSERT_NULL_PTR(pRCtrlUser);
                pRCtrlUser->QueueType = NARE_RCTRL_QTYPE_SOCK_RUNNING;
                pHDB->PendRCtrlCnt--;
                pRCtrlSOCK = NARE_SOCKGetFreeRCTRL(pHDB);
                /* because Free was >0 we dont expect LSA_NULL here         */
                NARE_ASSERT_NULL_PTR(pRCtrlSOCK);
                /* save pointer to user-RCTRL-Block within SOCK-RCTRL-block  */
                /* for later access. SOCK-requests getting the ThisIdx from  */
                /* the SOCK-RCTRL-Block as user-id                           */
                pRCtrlSOCK->pRCtrl = pRCtrlUser;
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Free RCTRL-Found [Hdl: 0x%X,Idx: %d]. Triggering SOCK Request",pHDB->ThisHandle,pRCtrlSOCK->ThisIdx);
                Status = NARE_SOCKTrigger(pHDB,pRCtrlSOCK);
                /* if failed we finish the user-request*/
                if ( Status != NARE_RSP_OK )
                {
                    /* release the SOCK-RCTRL-Block */
                    NARE_SOCKReleaseRCTRL(pHDB,pRCtrlSOCK);
                    pHDBUser = NAREGetUserHDBFromHandle(NARE_RQB_GET_HANDLE(pRCtrlUser->pRQB));
                    NARE_ASSERT_NULL_PTR(pHDBUser);
                    /* release RCtrl-Block and finish the request */
                    NARE_UserReady(pHDBUser,pRCtrlUser,Status);
                }
            }
            else
            {
                NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-No free SOCK-RCTRL (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
                Ready = LSA_TRUE;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-No requests pending  (pHDB: 0x%X,Handle: 0x%X)",pHDB,pHDB->ThisHandle);
            Ready = LSA_TRUE;
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKCheckAndStartPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKApplyRCtrl                         +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE     pHDB             +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK -HDB                                      +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block to handle.                    +*/
/*+               Must contain valid Bstate which selects the action to do. +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Give a User-RCTRL-Block to SOCK to handle the Request.    +*/
/*+               If no free SOCK-Resources present the request remains     +*/
/*+               queued until resources are feed. If resources are free    +*/
/*+               the request is handled (doing a SOCK-request)             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKApplyRCtrl(
    NARE_HDB_SOCK_PTR_TYPE    pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser)

{

    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Queuing block (pHDB: 0x%X,Handle: 0x%X,pRCtrlUser: 0x%X)",pHDB,pHDB->ThisHandle,pHDB->ThisHandle);
    NARE_PUT_BLOCK_TOP(pHDB->PendRCtrl.pBottom,
                       pHDB->PendRCtrl.pTop,
                       pRCtrlUser);
    pRCtrlUser->QueueType = NARE_RCTRL_QTYPE_SOCK_PENDING;
    pHDB->PendRCtrlCnt++;
    NARE_SOCKCheckAndStartPending(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKRemoveRCtrl                        +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE      pHDB            +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK -HDB                                      +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block to handle.                    +*/
/*+               Must contain valid Bstate which selects the action to do. +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removed RCTRL-Block from pending-queue.                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKRemoveRCtrl(
    NARE_HDB_SOCK_PTR_TYPE     pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlHelp;

    NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Removing block (pHDB: 0x%X, Handle: 0x%X,pRCtrlUser: 0x%X)",pHDB,pHDB->ThisHandle,pRCtrlUser);
    NARE_REM_BLOCK(pHDB->PendRCtrl.pBottom,
                   pHDB->PendRCtrl.pTop,
                   pRCtrlUser,
                   pRCtrlHelp);
    pHDB->PendRCtrlCnt--;
    NARE_ASSERT_NULL_PTR(pRCtrlHelp);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKOpenChannelDone                    +*/
/*+  Input                      NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                             NARE_HDB_SOCK_PTR_TYPE        pHDB          +*/
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
/*+               - save SOCK-Channel                                       +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKOpenChannelDone(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_SOCK_PTR_TYPE        pHDB)
{
    NARE_RSP                 Response;
    LSA_UINT16               RetVal;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_SOCK_RQB_GET_RESPONSE(pRQB));
    if ( NARE_SOCK_RQB_GET_RESPONSE(pRQB) == SOCK_RSP_OK )
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Open Channel successful");
        /* save SOCK-Channel in HDB */
        /* old: pHDB->SOCKHandle = pRQB->handle; */
        pHDB->SOCKHandle = pRQB->args.channel.handle;
    }
    else
    {
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"SOCK-Open Channel failed.[0x%X]",NARE_SOCK_RQB_GET_RESPONSE(pRQB));
        Response = NARE_RSP_ERR_LL;
    }
    NARE_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKOpenChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKCloseChannelDone                   +*/
/*+  Input                      NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                             NARE_HDB_SOCK_PTR_TYPE        pHDB          +*/
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

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKCloseChannelDone(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_SOCK_PTR_TYPE        pHDB)
{
    NARE_RSP                 Response;
    LSA_UINT16               RetVal;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKCloseChannelDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    Response = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_SOCK_RQB_GET_RESPONSE(pRQB));

    if ( NARE_SOCK_RQB_GET_RESPONSE(pRQB) == SOCK_RSP_OK )
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Close Channel successful");
    }
    else
    {
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"SOCK-Close Channel failed.[0x%X]",NARE_SOCK_RQB_GET_RESPONSE(pRQB));
        Response = NARE_RSP_ERR_LL;
    }
    NARE_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    NARE_MEM_FREE_FAIL_CHECK(RetVal);
    NARE_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKCloseChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKRQBCopyLLErrorLocal                +*/
/*+  Input                      LSA_RQB_LOCAL_ERROR_TYPE  NA...* pLLError   +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copys LL-Error to local structure.                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_SOCKRQBCopyLLErrorLocal(
    LSA_RQB_LOCAL_ERROR_TYPE  NARE_SOCK_LOWER_RQB_ATTR * pLLError)
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
/*+  Functionname          :    NARE_SOCKGetHostByNameDone                  +*/
/*+  Input                      NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                             NARE_HDB_SOCK_PTR_TYPE        pHDB          +*/
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

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKGetHostByNameDone(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_HDB_SOCK_PTR_TYPE        pHDB)
{
    NARE_RSP                       Response;
    LSA_BOOL                       CallStateMachine;
    NARE_SOCK_RCTRL_PTR_TYPE       pRCtrl;
    LSA_UINT16                     RetVal;
    NARE_USER_RCTRL_PTR_TYPE       pRCtrlUser;
    NARE_UNION_IP_TYPE             IPAddr;

    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKGetHostByNameDone(pRQB: 0x%X,pHDB: 0x%X)",pRQB,pHDB);
    CallStateMachine = LSA_TRUE;
    Response         = NARE_RSP_OK;
    NARE_LOWER_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_GETHOSTBYNAME done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,NARE_SOCK_RQB_GET_RESPONSE(pRQB));
    pRCtrl = NARE_SOCKGetRCTRLByIdx(pHDB,NARE_SOCK_RQB_GET_USERID_UVAR32(pRQB));
    NARE_ASSERT_NULL_PTR(pRCtrl);
    pRCtrlUser = pRCtrl->pRCtrl;
    NARE_ASSERT_NULL_PTR(pRCtrl);
    switch (NARE_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            IPAddr.w.IP1234 = *pRQB->args.get.ip_ptr;
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK[%X:%d]-GetHostByName successful (IP: %8.8X).",
                                  pHDB->ThisHandle,
                                  pRCtrl->ThisIdx,
                                  IPAddr.w.IP1234);
            /* Cfg-Address within User-RQB */
            pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[0] =  IPAddr.b.IPAddr[0];
            pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[1] =  IPAddr.b.IPAddr[1];
            pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[2] =  IPAddr.b.IPAddr[2];
            pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[3] =  IPAddr.b.IPAddr[3];
            NARE_PROGRAM_TRACE_06(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK[%X:%d]-GetHostByname. IP: %d.%d.%d.%d",
                                  pHDB->ThisHandle,
                                  pRCtrl->ThisIdx,
                                  pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[0],
                                  pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[1],
                                  pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[2],
                                  pRCtrlUser->pRQB->args.ResConf.Cfg.IPAddr[3]);
            break;

        case SOCK_RSP_ERR_RESOURCE:  /* DNS-Server not configured */
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"SOCK[%X:%d]-ERROR: No DNS Server",pHDB->ThisHandle,pRCtrl->ThisIdx);
            Response = NARE_RSP_ERR_DNS_FAILED;
            break;

        case SOCK_RSP_ERR_TIMEOUT:
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"SOCK[%X:%d]-ERROR: Timeout",pHDB->ThisHandle,pRCtrl->ThisIdx);
            Response = NARE_RSP_ERR_DNS_FAILED;
            break;

        default:
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"SOCK[%X:%d]-ERROR: Set LL (0x%X)",pHDB->ThisHandle,pRCtrl->ThisIdx,NARE_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = NARE_RSP_ERR_LL;
            break;
    }
    if (CallStateMachine)
    {   /* release the RQB and RCTRL-Block */
        NARE_SOCKReleaseRCTRL(pHDB,pRCtrl);
        NARE_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.get.ip_ptr,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        NARE_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.get.name_ptr,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        NARE_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        NARE_MEM_FREE_FAIL_CHECK(RetVal);
        /* call the main statemachine     */
        NARE_UserStateMachine(pRCtrlUser,Response,LSA_TRUE);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKGetHostByNameDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_sock_request_lower_done                +*/
/*+  Input                      NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for SOCK-requests.                       +*/
/*+               Fills Error-Structure in RQB with SOCKResponse            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_SOCK_LOWER_IN_FCT_ATTR nare_sock_request_lower_done(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB)
{
    NARE_HDB_SOCK_PTR_TYPE   pHDB;

    NARE_ENTER();
    NARE_ASSERT_NULL_PTR(pRQB);
    /* SOCK-request always done from an SOCK-Channel. !           */
    pHDB     = NAREGetSockHDBFromHandle(NARE_SOCK_RQB_GET_HANDLE(pRQB));
    /* save response from lower layer in global error-structure */
    /* SOCK has defined its own err-structure type. This is bad and should */
    /* be changed. All components shall use the same structure!            */
    NARE_SOCKRQBCopyLLErrorLocal((LSA_RQB_LOCAL_ERROR_TYPE  NARE_SOCK_LOWER_RQB_ATTR * )&pRQB->err);
    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        NARE_FATAL(NARE_FATAL_ERR_HANDLE_INVALID);
    }
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : nare_sock_request_lower_done(pRQB: 0x%X)",pRQB);
    switch (NARE_SOCK_RQB_GET_OPCODE(pRQB) )
    {
        case SOCK_OPC_OPEN_CHANNEL:
            NARE_SOCKOpenChannelDone(pRQB,pHDB);
            break;

        case SOCK_OPC_CLOSE_CHANNEL:
            NARE_SOCKCloseChannelDone(pRQB,pHDB);
            break;

        case SOCK_OPC_GETHOSTBYNAME:
            NARE_SOCKGetHostByNameDone(pRQB,pHDB);
            break;

        default:
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: nare_sock_request_lower_done()");
    NARE_EXIT();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKOpenChannel                        +*/
/*+  Input                      LSA_HANDLE_TYPE       NAREHandle            +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NAREHandle           : NARE SOCK-Channel handle                        +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to SOCK. The Request will be  +*/
/*+               finished by calling the SOCK-Callbackfunktion.            +*/
/*+               nare_SOCK_request_lower_done() if NARE_RSP_OK.            +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{

    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB;
    NARE_RSP                     Result;
    LSA_USER_ID_TYPE             UserId;

    NARE_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKOpenChannel(NAREHandle: 0x%X)",NAREHandle);
    Result = NARE_RSP_OK;
    UserId.uvar32 = 0;
    NARE_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_OPEN_CHANNEL);
        pRQB->err.lsa_component_id = LSA_COMP_ID_UNUSED;
        pRQB->args.channel.handle_upper  = NAREHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.sock_request_upper_done_ptr = nare_sock_request_lower_done;
        NARE_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",NAREHandle,pRQB);
        NARE_SOCK_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKOpenChannel. Rsp: 0x%X",Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKCloseChannel                       +*/
/*+  Input                      LSA_HANDLE_TYPE       SOCKHandle            +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+                             NARE_RSP_ERR_RESOURCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SOCKHandle            : SOCK handle to close                           +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to SOCK. The Request will be +*/
/*+               finished by calling the SOCK-Callbackfunktion.            +*/
/*+               nare_SOCK_request_lower_done()                            +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKCloseChannel(
    LSA_HANDLE_TYPE       SOCKHandle,
    LSA_SYS_PTR_TYPE      pSys)
{
    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB;
    NARE_RSP                     Result;
    LSA_USER_ID_TYPE             UserId;

    NARE_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKCloseChannel(SOCKHandle: 0x%X)",SOCKHandle);
    Result = NARE_RSP_OK;
    UserId.uvar32 = 0;
    NARE_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_CLOSE_CHANNEL);
        NARE_SOCK_RQB_SET_HANDLE(pRQB,SOCKHandle);
        pRQB->err.lsa_component_id = LSA_COMP_ID_UNUSED;
        NARE_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_CLOSE_CHANNEL (SOCKHandle: 0x%X,pRQB: 0x%X)",SOCKHandle,pRQB);
        NARE_SOCK_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        NARE_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKCloseChannel()");
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_SOCKGetHostByName                      +*/
/*+  Input/Output          :    NARE_HDB_SOCK_PTR_TYPE       pHDB           +*/
/*+                             NARE_SOCK_RCTRL_PTR_TYPE     pRCtrl         +*/
/*+                             NARE_UPPER_MEM_U8_PTR_TYPE   pStationName   +*/
/*+                             LSA_UINT32                   StationNameLen +*/
/*+                                                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to SOCK-HDB                              +*/
/*+  pRCtrl               : SOCK RCTRl-block pointer                         +*/
/*+  pStationName         : Pointer to StationName                          +*/
/*+  StationNameLen       : StationNameLen                                  +*/
/*+                                                                         +*/
/*+  Result               : NARE_RSP_OK                                     +*/
/*+                         NARE_RSP_ERR_RESOURCE                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues GetHostByName Request to SOCK                      +*/
/*+                                                                         +*/
/*+               The Request will be finished be calling the SOCK-Callback +*/
/*+               nare_sock_request_lower_done() if NARE_RSP_OK.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKGetHostByName(
    NARE_HDB_SOCK_PTR_TYPE       pHDB,
    NARE_SOCK_RCTRL_PTR_TYPE     pRCtrl,
    NARE_UPPER_MEM_U8_PTR_TYPE   pStationName,
    LSA_UINT32                   StationNameLen)
{
    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB;
    LSA_SYS_PTR_TYPE              pSys;
    NARE_RSP                      Result;
    LSA_USER_ID_TYPE              UserId;
    LSA_UINT16                    RetVal;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_ASSERT_NULL_PTR(pRCtrl);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARE_SOCKGetHostByName(pHandle: 0x%X)",pHDB);
    Result = NARE_RSP_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    NARE_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        NARE_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_GETHOSTBYNAME);
        NARE_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SOCKHandle);
        /* we user User-ID for RCtrl-identification */
        NARE_SOCK_RQB_SET_USERID_UVAR32(pRQB,pRCtrl->ThisIdx);
        /* allocate name_ptr memory (one byte more for 0-termination) */
        NARE_SOCK_ALLOC_LOWER_MEM(&pRQB->args.get.name_ptr,UserId,(LSA_UINT16)(StationNameLen+1),pSys);
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->args.get.name_ptr, LSA_NULL) )
        {
            NARE_SOCK_ALLOC_LOWER_MEM(&pRQB->args.get.ip_ptr,UserId,sizeof(pRQB->args.get.ip_ptr),pSys);
            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->args.get.name_ptr, LSA_NULL) )
            {   /* copy the station name */
                NARE_MEMCOPY_COMMON(
                    (NARE_COMMON_MEM_PTR_TYPE)pRQB->args.get.name_ptr,
                    (NARE_COMMON_MEM_PTR_TYPE)pStationName,
                    StationNameLen);
                /* append 0 */
                pRQB->args.get.name_ptr[StationNameLen] = 0;
                pRQB->args.get.retry    = LSA_FALSE;
                NARE_PROGRAM_TRACE_04(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Doing GHBN request (Handle: 0x%X,pRQB: %Xh) pStationName: %X, Len: %d",
                                      pHDB->ThisHandle,
                                      pRQB,
                                      pStationName,
                                      StationNameLen);
                NARE_LOWER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_GETHOSTBYNAME (Handle: 0x%X,SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->SOCKHandle,pRQB);
                NARE_SOCK_REQUEST_LOWER(pRQB,pSys);
            }
            else
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower-memory failed!");
                NARE_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.get.name_ptr,pSys);
                NARE_MEM_FREE_FAIL_CHECK(RetVal);
                NARE_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
                NARE_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower-RQB-memory failed!");
            NARE_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pSys);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower-memory failed!");
        Result = NARE_RSP_ERR_RESOURCE;
    }
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARE_SOCKGetHostByName() Result: 0x%x", Result);
    return(Result);
}
#endif

/*****************************************************************************/
/*  end of file NARE_SCK.C                                                   */
/*****************************************************************************/

