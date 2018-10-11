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
/*  F i l e               &F: nare_tim.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE-Timer functions                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_TIM */

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
/*                             external functions                            */
/*===========================================================================*/

/*===========================================================================*/
/*=====  Timer -functions                                                ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARETimerMgmSetup                           +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      NARE_STS_OK                                               +*/
/*+               NARE_STS_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the NARE global Timer-Management              +*/
/*+               Note that the global management hast to be allocated.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARETimerMgmSetup(LSA_VOID)
{
    LSA_UINT32 i;

    NARE_ASSERT_NULL_PTR(NARE_MGM_GET_NDB_PTR());
    for ( i= 0; i< NARE_CFG_MAX_TIMER; i++ )
    {
        NARE_MGM_GET_NDB_PTR()->Timer[i].InUse = LSA_FALSE;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   NARETimerSet                                 +*/
/*+  Input/Output          :   LSA_TIMER_ID_TYPE                TimerID     +*/
/*+                            NARE_UPPER_RQB_PTR_TYPE          pRQB        +*/
/*+                            LSA_FCT_PTR(NARE_LOCAL_FCT_ATTR, Cbf) (..)   +*/
/*+                            NARE_LOCAL_MEM_U16_PTR_TYPE      pNARETimerID+*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID put into timer-management                    +*/
/*+  pRQB         Pointer to intenal RQB put into timer-management          +*/
/*+  Cbf          Optional Callbackfunction called when timer elapses.      +*/
/*+               Gets pointer to IntRQB as parameter                       +*/
/*+  pNARETimerID NARE internal Timer ID (= index ) returned                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Puts new Timer into Timer-management                      +*/
/*+               if a Cbf is present, it is called if the timer elapses.   +*/
/*+               if no Cbf is present, when the timer elapses the output   +*/
/*+               Macro NARE_DO_TIMER_REQUEST is called with the rqb        +*/
/*+               spezified (only if rqb is not underway).                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARETimerSet(
    LSA_TIMER_ID_TYPE               TimerID,
    NARE_UPPER_RQB_PTR_TYPE         pRQB,
    LSA_VOID                        LSA_FCT_PTR(NARE_LOCAL_FCT_ATTR, Cbf)
    (NARE_INTERNAL_REQUEST_PTR_TYPE pIntRQB),
    NARE_LOCAL_MEM_U16_PTR_TYPE     pNARETimerID)
{
    NARE_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_UINT16                      i;
    LSA_BOOL                        Found;

    pTimer  = NARE_MGM_GET_NDB_PTR()->Timer;
    i       = 0;
    Found   = LSA_FALSE;
    while (( i <NARE_CFG_MAX_TIMER ) && ( ! Found ))
    {
        if (! pTimer[i].InUse )
        {
            Found = LSA_TRUE;
            pTimer[i].TimerID       = TimerID;
            pTimer[i].IntRQB.pRQB   = pRQB;
            pTimer[i].IntRQB.InUse  = LSA_FALSE;
            pTimer[i].Cbf           = Cbf;
            /* setup RQB */
            NARE_RQB_SET_OPCODE(pRQB,NARE_OPC_TIMER);
            pRQB->args.Timer.TimerID = i;  /* NARE-Timer ID */
            /* set InUse last to be sure other entrys are   */
            /* valid.                                       */
            pTimer[i].InUse         = LSA_TRUE;
            *pNARETimerID           = i;
        };
        i++;
    }
    if ( ! Found )
    {
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"NARETimerSet(): No more Timer-Resources. NARE_CFG_MAX_TIMER wrong?");
        NARE_FATAL(NARE_FATAL_ERR_TIMER_RESOURCE);
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   NARETimerAlloc                               +*/
/*+  Input/Output              LSA_UINT16                       TimerType   +*/
/*+                            LSA_UINT16                       TimerBase   +*/
/*+                            LSA_UINT32                       UserID      +*/
/*+                            LSA_FCT_PTR(NARE_LOCAL_FCT_ATTR, Cbf) (..)   +*/
/*+                            NARE_LOCAL_MEM_U16_PTR_TYPE      pNARETimerID+*/
/*+                                                                         +*/
/*+  Result                :   NARE_RSP_OK                                  +*/
/*+                            NARE_RSP_ERR_RESOURCE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerType    LSA-TimerType                                             +*/
/*+               LSA_TIMER_TYPE_ONE_SHOT                                   +*/
/*+               LSA_TIMER_TYPE_CYCLIC                                     +*/
/*+  TimerBase    LSA-TimerBase                                             +*/
/*+               LSA_TIME_BASE_1MS etc                                     +*/
/*+  UserID       UserID put into RQB send on timer-event.                  +*/
/*+  Cbf          Optional Callbackfunction called when timer elapses.      +*/
/*+               Gets pointer to IntRQB as parameter                       +*/
/*+               LSA_NULL for standardhandling                             +*/
/*+  pNARETimerID NARE internal Timer ID (= index ) returned                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates LSA-Timer and setup NARE-Timer management       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP  NARE_LOCAL_FCT_ATTR NARETimerAlloc(
    LSA_UINT16                      TimerType,
    LSA_UINT16                      TimerBase,
    LSA_UINT32                      UserID,
    NareTimerCbf                    Cbf,
    NARE_LOCAL_MEM_U16_PTR_TYPE     pNARETimerID)
{
    LSA_UINT16              RetVal;
    NARE_RSP                Response;
    LSA_TIMER_ID_TYPE       TimerID;
    NARE_UPPER_RQB_PTR_TYPE pRQB;

    NARE_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,"IN : NARETimerAlloc(Type: 0x%X, Base: 0x%X, UserID: 0x%X)",TimerType,TimerBase,UserID);
    Response = NARE_RSP_OK;
    NARE_ALLOC_UPPER_RQB_LOCAL(&pRQB,sizeof(NARE_RQB_TYPE));
    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        Response = NARE_RSP_ERR_RESOURCE;
        NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"NARETimerAlloc(). Allocation RQB memory failed.");
    }
    else
    {
        NARE_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling NARE_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",TimerType,TimerBase);
        NARE_ALLOC_TIMER(&RetVal,
                         &TimerID,
                         TimerType,
                         TimerBase);
        if ( RetVal == LSA_RET_OK)
        {
            NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"<--: Calling NARE_ALLOC_TIMER success. TimerID: %d.",TimerID);
            /* setup RQB-user-ID */
            pRQB->args.Timer.UserID  = UserID;
            NARETimerSet(TimerID,pRQB,Cbf,pNARETimerID);
        }
        else
        {
            Response = NARE_RSP_ERR_RESOURCE;
            NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"<--: Calling NARE_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
            NARE_FREE_UPPER_RQB_LOCAL(&RetVal,pRQB);
            NARE_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }
    NARE_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARETimerAlloc(0x%X) NARETimerID: 0x%X",Response,*pNARETimerID);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARETimerFree                               +*/
/*+  Input/Output          :    LSA_UINT16                   NARETimerID    +*/
/*+  Result                :    NARE_RSP_OK                                 +*/
/*+                             NARE_RPS_ERR_SEQUENCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NARETimerID   NARE-Timer ID to free in management                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees timer in timermanagement and releases LSA-Timer     +*/
/*+               Fees internal RQB. If RQB still in use we abort with error+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_RSP  NARE_LOCAL_FCT_ATTR NARETimerFree(LSA_UINT16 NARETimerID)
{
    NARE_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_UINT16                      RetVal;
    LSA_TIMER_ID_TYPE               TimerID;
    NARE_RSP                        Response;

    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : NARETimerFree(NARETimerID: 0x%X)",NARETimerID);
    Response = NARE_RSP_OK;
    if ( NARETimerID < NARE_CFG_MAX_TIMER )
    {
        pTimer = &NARE_MGM_GET_NDB_PTR()->Timer[NARETimerID];
        if ( pTimer->InUse )
        {
            if ( ! pTimer->IntRQB.InUse )
            {
                TimerID = pTimer->TimerID;
                NARE_STOP_TIMER(&RetVal,TimerID); /* just for sure. dont matter if already*/
                /* stopped                              */
                NARE_FREE_TIMER(&RetVal,TimerID);
                pTimer->InUse = LSA_FALSE;
                /* free internal RQB for Timer */
                NARE_FREE_UPPER_RQB_LOCAL(&RetVal,pTimer->IntRQB.pRQB);
                NARE_MEM_FREE_FAIL_CHECK(RetVal);
                pTimer->IntRQB.pRQB  = LSA_NULL;
                pTimer->IntRQB.InUse = LSA_FALSE;
            }
            else
            {
                Response = NARE_RSP_ERR_SEQUENCE;
                NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"NARETimerFree(): RQB still in use! (0x%X).",NARETimerID);
            }
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"NARETimerFree(): Invalid Timer-ID(0x%X)",NARETimerID);
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARETimerFree(0x%X)",Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARETimerStart                              +*/
/*+  Input/Output          :    LSA_UINT16                   NARETimerID    +*/
/*+                             LSA_UINT16                   Time           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NARETimerID   NARE-Timer ID to free in management                      +*/
/*+  Time          running time                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARETimerStart(
    LSA_UINT16 NARETimerID,
    LSA_UINT16 Time)
{
    LSA_UINT16                      RetVal;
    NARE_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_USER_ID_TYPE                Dummy;

    NARE_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"IN : NARETimerStart(NARETimerID: 0x%X, Time: 0x%X)",NARETimerID,Time);
    if ( NARETimerID < NARE_CFG_MAX_TIMER )
    {
        pTimer = &NARE_MGM_GET_NDB_PTR()->Timer[NARETimerID];
        if ( pTimer->InUse )
        {
            Dummy.uvar32 = NARETimerID; /* we use user-id for NARE-Timer ID */
            NARE_START_TIMER(&RetVal,
                             pTimer->TimerID,
                             Dummy,
                             Time);
            if ( RetVal == LSA_RET_ERR_PARAM )
            {
                NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"<--: Calling NARE_START_TIMER failed (Response: 0x%X).",RetVal);
                NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"NARETimerStart(): Timer not in use (0x%X).",NARETimerID);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"NARETimerStart(): Wrong Timer ID (0x%X).",NARETimerID);
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARETimerStart()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARETimerStop                               +*/
/*+  Input/Output          :    LSA_UINT16                   NARETimerID    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NARETimerID   NARE-Timer ID to free in management                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARETimerStop(
    LSA_UINT16 NARETimerID)

{

    LSA_UINT16                      RetVal;
    NARE_TIMER_INFO_PTR_TYPE        pTimer;

    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : NARETimerStop(NARETimerID: 0x%X)",NARETimerID);
    if ( NARETimerID < NARE_CFG_MAX_TIMER )
    {
        pTimer = &NARE_MGM_GET_NDB_PTR()->Timer[NARETimerID];
        if ( pTimer->InUse )
        {
            NARE_STOP_TIMER(&RetVal,pTimer->TimerID);
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"NARETimerStart(): Wrong Timer ID (0x%X).",NARETimerID);
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARETimerStop()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARETimerRQBFree                            +*/
/*+  Input/Output          :    LSA_UINT16                   NARETimerID    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NARETimerID   NARE-Timer ID to free in management                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after RQB was received and is free for further use +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_LOCAL_FCT_ATTR NARETimerRQBFree(
    LSA_UINT16 NARETimerID)
{
    NARE_TIMER_INFO_PTR_TYPE   pTimer;

    pTimer = &NARE_MGM_GET_NDB_PTR()->Timer[NARETimerID];
    pTimer->IntRQB.InUse = LSA_FALSE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_timeout                                +*/
/*+  Input/Output          :    LSA_TIMER_ID_TYPE timer_id                  +*/
/*+                             LSA_USER_ID_TYPE  user_id                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  timer_id     : ID of Timer                                             +*/
/*+  user_id      : Used for NARETimerID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called from systemadaption on timerevent. May be called   +*/
/*+               within ISR-Handler.                                       +*/
/*+                                                                         +*/
/*+               LSA-Systemfunction                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  NARE_SYSTEM_IN_FCT_ATTR  nare_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id)
{
    NARE_TIMER_INFO_PTR_TYPE   pTimer;
    LSA_UINT32                 NARETimerID;

    NARETimerID = (LSA_UINT16) user_id.uvar32;
#ifdef NARE_CFG_USE_ENTER_EXIT_TIMEOUT
    NARE_ENTER();
#endif
    NARE_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_timeout(timer_id: 0x%X, user_id: 0x%X)",timer_id,user_id.uvar32);
    if ( NARETimerID < NARE_CFG_MAX_TIMER )
    {
        pTimer = &NARE_MGM_GET_NDB_PTR()->Timer[NARETimerID];
        if (( pTimer->InUse ) &&
            ( pTimer->TimerID == timer_id ))
        {
            /* if a callbackfunction is present, call it */
            if ( ! LSA_HOST_PTR_ARE_EQUAL(pTimer->Cbf, LSA_NULL) )
            {
                pTimer->Cbf(&pTimer->IntRQB);
            }
            else /* do standard handling..*/
            {
                if ( ! pTimer->IntRQB.InUse )
                {
                    /* NARE_DBG_ACT("Calling NARE_DO_TIMER_REQUEST(pRQB: %Xh)\r\n",(LSA_UINT32)pTimer->IntRQB.pRQB,0,0); */
                    /* Note that the RQB is already filled up in startup! */
                    pTimer->IntRQB.InUse = LSA_TRUE;
                    NARE_DO_TIMER_REQUEST(pTimer->IntRQB.pRQB);
                }
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"nare_timeout(): Invalid Timer-ID (0x%X).",NARETimerID);
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"nare_timeout(): Invalid Timer-ID (0x%X).",NARETimerID);
        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_timeout()");
#ifdef NARE_CFG_USE_ENTER_EXIT_TIMEOUT
    NARE_EXIT();
#endif
}

/*****************************************************************************/
/*  end of file NARE_TIM.C                                                   */
/*****************************************************************************/


