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
/*  F i l e               &F: nare_sys.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE-system input-functions                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  1
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_SYS */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "nare_inc.h"            /* NARE headerfiles */
#include "nare_int.h"            /* internal header */

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

struct NARE_STATIC_TYPE_
{
    /* main-managment of NARE */
    NARE_LOCAL_MEM_ATTR NARE_MGM_TYPE        Mgm;
    
    /* Requestcontrolblocks for handling user-requests  */
    NARE_LOCAL_MEM_ATTR NARE_USER_RCTRL_ARRAY_TYPE  RUser[NARE_CFG_MAX_USER_CHANNELS];
    /* Requestcontrolblocks for handling DCP-requests   */
    NARE_LOCAL_MEM_ATTR NARE_DCP_RCTRL_ARRAY_TYPE   RDcp[NARE_CFG_MAX_DCP_CHANNELS];
    /* Requestcontrolblocks for handling EDD-requests   */
    NARE_LOCAL_MEM_ATTR NARE_EDD_RCTRL_ARRAY_TYPE   REdd[NARE_CFG_MAX_EDD_CHANNELS];
 #ifndef NARE_CFG_NO_SOCK
    /* Requestcontrolblocks for handling SOCK-requests  */
    NARE_LOCAL_MEM_ATTR NARE_SOCK_RCTRL_ARRAY_TYPE  RSock[1];
 #endif
} g_NARE_Static;

NARE_LOCAL_MEM_ATTR NARE_MGM_PTR_TYPE   g_pNAREInfo;

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserGetFreeRCTRL                       +*/
/*+  Input/Output          :    NARE_HDB_USER_PTR_TYPE             pHDB     +*/
/*+  Result                :    NARE_USER_RCTRL_PTR_TYPE                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to User-HDB                                       +*/
/*+                                                                         +*/
/*+  Result     : Pointer to free RCTRL-Block or LSA_NULL if none free      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free USER-RCTRL-Block                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_USER_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_UserGetFreeRCTRL(
    NARE_HDB_USER_PTR_TYPE pHDB)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrl;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_REM_BLOCK_BOTTOM(pHDB->UserRCtrl.pFree.pBottom,
                          pHDB->UserRCtrl.pFree.pTop,
                          pRCtrl);
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRCtrl, LSA_NULL) )
    {
        pHDB->UserRCtrl.Used++;
        pHDB->UserRCtrl.Free--;

        pRCtrl->BState     = NARE_RCTRL_BSTATE_USED;
        pRCtrl->DoCancel   = LSA_FALSE;
        pRCtrl->DCPSetMode = NARE_RCTRL_DCP_SET_NONE;
        pRCtrl->QueueType  = NARE_RCTRL_QTYPE_NONE;
        pRCtrl->pRState    = LSA_NULL;
    }
    return(pRCtrl);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserReleaseRCTRL                       +*/
/*+  Input/Output          :    NARE_HDB_USER_PTR_TYPE             pHDB     +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE           pRCtrl   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to User-HDB                                       +*/
/*+  pRCtrl     : Pointer to User-RCTRL-Block to release                    +*/
/*+                                                                         +*/
/*+  Result     : --                                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Releases an User-RCTRL-Block                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserReleaseRCTRL(
    NARE_HDB_USER_PTR_TYPE      pHDB,
    NARE_USER_RCTRL_PTR_TYPE    pRCtrl)
{
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_ASSERT_NULL_PTR(pRCtrl);

    if ( pRCtrl->BState == NARE_RCTRL_BSTATE_USED )
    {
        if ( ! LSA_HOST_PTR_ARE_EQUAL(&(*pHDB->UserRCtrl.pTable)[pRCtrl->ThisIdx],pRCtrl))
        {
            /* unexpected. ThisIdx and Pointer not matching.*/
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
        }
        /* put into free-list */
        NARE_PUT_BLOCK_TOP(pHDB->UserRCtrl.pFree.pBottom,
                           pHDB->UserRCtrl.pFree.pTop,
                           pRCtrl);
        pHDB->UserRCtrl.Used--;
        pHDB->UserRCtrl.Free++;
        pRCtrl->BState    = NARE_RCTRL_BSTATE_FREE;
        pRCtrl->DoCancel  = LSA_FALSE;
        pRCtrl->QueueType = NARE_RCTRL_QTYPE_NONE;
    }
    else
    {   /* unexpected. Pointer not found in list */
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserGetFreeHDB                         +*/
/*+  Input/Output          :    NARE_HDB_USER_PTR_TYPE NARE_LOCAL..pHDBUser +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : NARE user -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free User-HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NARE_UserGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_USER_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBUser)
{
    NARE_RSP                  Status;
    LSA_BOOL                  Found;
    NARE_HDB_USER_PTR_TYPE    pHDB;
    LSA_UINT32                RSize;
    LSA_UINT32                i;
    NARE_USER_RCTRL_PTR_TYPE  pRCtrl;

    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : NAREGetFreeUserHDB(pHDB: %Xh)",pHDBUser);
    pHDB   = NARE_MGM_GET_HUSER_PTR()->HDB;
    i      = 0;
    Found  = LSA_FALSE;
    Status = NARE_RSP_OK;
    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    while ( (! Found)  &&  ( i < NARE_MGM_GET_HUSER_PTR()->HandleCnt))
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
        RSize = sizeof(NARE_USER_RCTRL_TYPE) * NoOfParallelRQBs;
        /* allocate control-block memory */
        NARE_ALLOC_LOCAL_MEM((NARE_LOCAL_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *)&pHDB->UserRCtrl.pTable, RSize);
        if ( LSA_HOST_PTR_ARE_EQUAL(pHDB->UserRCtrl.pTable, LSA_NULL) )
        {
            Status = NARE_RSP_ERR_RESOURCE;
            NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"NAREGetFreeUserHDB(). Allocation memory failed.");
        }

        if ( Status == NARE_RSP_OK)
        {
            /* ------------------------------------------------------------------*/
            /* Setup RCTRL-Blocks for User                                       */
            /* ------------------------------------------------------------------*/
            /* initialize memory */
            NARE_MEMSET_LOCAL(pHDB->UserRCtrl.pTable,0,RSize);
            /* initialize User RCTRL-Block management */
            pHDB->UserRCtrl.Cnt   = NoOfParallelRQBs;
            pHDB->UserRCtrl.Free  = NoOfParallelRQBs;
            pHDB->UserRCtrl.Used  = 0;
            pHDB->UserRCtrl.pFree.pBottom = LSA_NULL;
            pHDB->UserRCtrl.pFree.pTop   = LSA_NULL;
            pRCtrl = &(* pHDB->UserRCtrl.pTable)[0]; /* pointer to first element */
            for ( i=0; i< NoOfParallelRQBs; i++)
            {
                NARE_PUT_BLOCK_TOP(pHDB->UserRCtrl.pFree.pBottom,
                                   pHDB->UserRCtrl.pFree.pTop,
                                   pRCtrl);
                pRCtrl->ThisIdx = (LSA_UINT16)i;
                /* set state of control block */
                pRCtrl->BState    = NARE_RCTRL_BSTATE_FREE;
                pRCtrl->pRState   = LSA_NULL;
                pRCtrl->DoCancel  = LSA_FALSE;
                pRCtrl->QueueType = NARE_RCTRL_QTYPE_NONE;
                pRCtrl++;
            }
            NARE_MGM_GET_HUSER_PTR()->UsedCnt++;
            pHDB->State = NARE_HANDLE_STATE_CLOSED;
            pHDB->TraceIdx = 0;
            *pHDBUser = pHDB;
        }
    }
    else
    {
        Status = NARE_RSP_ERR_RESOURCE;
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"NAREGetFreeUserHDB(). No more free handles.");
    }
    NARE_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"OUT: NAREGetFreeUserHDB(Status: 0x%X, pHDB: 0x%X )",Status,*pHDBUser);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserReleaseHDB                         +*/
/*+  Input/Output          :    NARE_HDB_USER_PTR_TYPE pHDBUser             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser   : Pointer to User Handle to release                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release User-HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to NARE_HANDLE_STATE_FREE               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserReleaseHDB(
    NARE_HDB_USER_PTR_TYPE pHDB)
{
    LSA_UINT16   FreeStat;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_UserReleaseHDB(pHDB: 0x%X)",pHDB);
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FREE_LOCAL_MEM(&FreeStat, pHDB->UserRCtrl.pTable);
    NARE_MEM_FREE_FAIL_CHECK(FreeStat);

    pHDB->UserRCtrl.pTable = LSA_NULL;
    pHDB->UserRCtrl.Cnt   = 0;
    pHDB->UserRCtrl.Free  = 0;
    pHDB->UserRCtrl.Used  = 0;
    pHDB->UserRCtrl.pFree.pBottom = LSA_NULL;
    pHDB->UserRCtrl.pFree.pTop   = LSA_NULL;
    pHDB->State           = NARE_HANDLE_STATE_FREE;
    pHDB->TraceIdx        = 0xffffffff;

    NARE_MGM_GET_HUSER_PTR()->UsedCnt--;
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_UserReleaseHDB()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREFreeMgm                                 +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    NARE_RSP_OK                                 +*/
/*+                             NARE_RSP_ERR_SEQUENCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees NARE Managmenent if allocated.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_LOCAL_FCT_ATTR NAREFreeMgm(LSA_VOID)
{
    NARE_RSP Response;

    Response = NARE_RSP_OK;
    if ( ! LSA_HOST_PTR_ARE_EQUAL(g_pNAREInfo, LSA_NULL) )
    {
        /* ----------------------------------------------------------------------*/
        /* free ARP-Timer if present                                             */
        /* ----------------------------------------------------------------------*/
        if ( g_pNAREInfo->HEdd.ARPTimer.ID != NARE_TIMER_ID_INVALID )
        {
            Response = NARETimerFree(g_pNAREInfo->HEdd.ARPTimer.ID);
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_init                                   +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    NARE_RESP                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the NARE.                                     +*/
/*+                                                                         +*/
/*+               Note: Must only be called if not already initialized, not +*/
/*+                     checked!                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_SYSTEM_IN_FCT_ATTR nare_init(LSA_VOID)
{
    NARE_RSP                 Status;
    LSA_UINT32               i;

    Status = NARE_RSP_OK;
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_init()");

    /* --------------------------------------------------------------------------*/
    /* the Request-Control-blocks will be                                        */
    /* allocated on opening the channel!                                         */
    /* --------------------------------------------------------------------------*/
    g_pNAREInfo = &g_NARE_Static.Mgm;
    NARE_MEMSET_LOCAL(g_pNAREInfo,0,sizeof(NARE_MGM_TYPE));
    /* --------------------------------------------------------------------------*/
    /* Setup management                                                          */
    /* --------------------------------------------------------------------------*/
    if (Status == NARE_RSP_OK )
    {
        g_pNAREInfo->NDB.LLError.lsa_component_id = LSA_COMP_ID_UNUSED;

        g_pNAREInfo->HUser.HandleCnt = NARE_CFG_MAX_USER_CHANNELS;
        g_pNAREInfo->HUser.UsedCnt  = 0;
        for ( i=0; i<NARE_CFG_MAX_USER_CHANNELS; i++)
        {
            g_pNAREInfo->HUser.HDB[i].State      = NARE_HANDLE_STATE_FREE;
            g_pNAREInfo->HUser.HDB[i].ThisHandle = (LSA_HANDLE_TYPE) (i | NARE_HANDLE_TYPE_USER);
            g_pNAREInfo->HUser.HDB[i].TraceIdx   = 0;
        }
        g_pNAREInfo->HDcp.HandleCnt = NARE_CFG_MAX_DCP_CHANNELS;
        g_pNAREInfo->HDcp.UsedCnt   = 0;
        for ( i=0; i<NARE_CFG_MAX_DCP_CHANNELS; i++)
        {
            g_pNAREInfo->HDcp.HDB[i].State      = NARE_HANDLE_STATE_FREE;
            g_pNAREInfo->HDcp.HDB[i].ThisHandle = (LSA_HANDLE_TYPE) (i | NARE_HANDLE_TYPE_DCP);
            g_pNAREInfo->HDcp.HDB[i].TraceIdx  = 0;
        }
        g_pNAREInfo->HEdd.HandleCnt = NARE_CFG_MAX_EDD_CHANNELS;
        g_pNAREInfo->HEdd.UsedCnt   = 0;
        for ( i=0; i<NARE_CFG_MAX_EDD_CHANNELS; i++)
        {
            g_pNAREInfo->HEdd.HDB[i].State        = NARE_HANDLE_STATE_FREE;
            g_pNAREInfo->HEdd.HDB[i].ThisHandle   = (LSA_HANDLE_TYPE) (i | NARE_HANDLE_TYPE_EDD);
            g_pNAREInfo->HEdd.HDB[i].TraceIdx     = 0;
        }
#ifndef NARE_CFG_NO_SOCK
        g_pNAREInfo->HSock.HandleCnt = 1; /* only one SOCK-channel supported! */
        g_pNAREInfo->HSock.UsedCnt   = 0;
        g_pNAREInfo->HSock.HDB[0].State = NARE_HANDLE_STATE_FREE;
        g_pNAREInfo->HSock.HDB[0].ThisHandle   = (LSA_HANDLE_TYPE) (0 | NARE_HANDLE_TYPE_SOCK);
#endif
        /* ----------------------------------------------------------------------*/
        /* Setup Timer-Management                                                */
        /* ----------------------------------------------------------------------*/
        NARETimerMgmSetup();
        /* ----------------------------------------------------------------------*/
        /* Allocate ARP-Timer                                                    */
        /* ----------------------------------------------------------------------*/

        g_pNAREInfo->HEdd.ARPTimer.ID = NARE_TIMER_ID_INVALID;

        Status = NARETimerAlloc( LSA_TIMER_TYPE_CYCLIC,
                                 LSA_TIME_BASE_100MS,
                                 NARE_TIMER_USER_ID_ARP,
                                 NARE_EDDTimerCB,
                                 &g_pNAREInfo->HEdd.ARPTimer.ID);
        if ( Status == NARE_RSP_OK )
        {
            g_pNAREInfo->HEdd.ARPTimer.UserCnt = 0;
        }
        else
        {
            NAREFreeMgm();
        }
    }
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_init(), Status: 0x%X",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_undo_init                              +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    NARE_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      NARE_RSP_OK                                               +*/
/*+               NARE_RSP_ERR_SEQUENCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Uninizializes NARE.                                       +*/
/*+               This is only possible if no device is open                +*/
/*+               NARE_RSP_ERR_SEQUENCE if device is open.                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP NARE_SYSTEM_IN_FCT_ATTR  nare_undo_init(LSA_VOID)
{
    NARE_RSP  Status;

    Status = NARE_RSP_OK;

    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_undo_init()");

#ifdef NARE_CFG_NO_SOCK
    if ( NARE_MGM_GET_HUSER_PTR()->UsedCnt  ||
         NARE_MGM_GET_HEDD_PTR()->UsedCnt   ||
         NARE_MGM_GET_HDCP_PTR()->UsedCnt )
#else
        if ( NARE_MGM_GET_HSOCK_PTR()->UsedCnt ||
             NARE_MGM_GET_HEDD_PTR()->UsedCnt  ||
             NARE_MGM_GET_HDCP_PTR()->UsedCnt  ||
             NARE_MGM_GET_HUSER_PTR()->UsedCnt  )
#endif
    {
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"nare_undo_init(). Not all channels closed yet.");
        Status = NARE_RSP_ERR_SEQUENCE;
    }
    else
    {
        Status = NAREFreeMgm();
    }
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_undo_init(), Status: 0x%X",Status);
    return(Status);
}

/*****************************************************************************/
/*  end of file NARE_SYS.C                                                    */
/*****************************************************************************/

