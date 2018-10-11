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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_tim.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  LLDP-Timer functions                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  06.04.04    JS    initial version.                                       */
/*  17.06.04    JS    set opcode with macro.                                 */
/*  27.06.06    JS    added  LLDP_FILE_SYSTEM_EXTENSION                      */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  4
#define LLDP_MODULE_ID     LTRC_ACT_MODUL_ID /* LLDP_MODULE_ID_LLDP_TIM */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lldp_inc.h"            /* LLDP headerfiles */
#include "lldp_int.h"            /* internal header */

LLDP_FILE_SYSTEM_EXTENSION(LLDP_MODULE_ID)

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
/*+  Functionname          :    LLDP_TimerMgmSetup                          +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      LLDP_STS_OK                                               +*/
/*+               LLDP_STS_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the LLDP global Timer-Management              +*/
/*+               Note that the global management hast to be allocated.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_TimerMgmSetup(LSA_VOID)
{

    LSA_UINT32 i;

    LLDP_ASSERT_NULL_PTR(LLDP_MGM_GET_GLOB_PTR());

    for ( i= 0; i< LLDP_CFG_MAX_TIMER; i++ )
    {
        LLDP_MGM_GET_GLOB_PTR()->Timer[i].InUse = LSA_FALSE;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   LLDP_TimerSet                                +*/
/*+  Input/Output          :   LSA_TIMER_ID_TYPE                TimerID     +*/
/*+                            LLDP_UPPER_RQB_PTR_TYPE          pRQB        +*/
/*+                            LSA_FCT_PTR(LLDP_LOCAL_FCT_ATTR, Cbf) (..)   +*/
/*+                            LLDP_LOCAL_MEM_U16_PTR_TYPE      pLLDPTimerID+*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID put into timer-management                    +*/
/*+  pRQB         Pointer to intenal RQB put into timer-management          +*/
/*+  Cbf          Optional Callbackfunction called when timer elapses.      +*/
/*+               Gets pointer to IntRQB as parameter                       +*/
/*+  pLLDPTimerID LLDP internal Timer ID (= index ) returned                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Puts new Timer into Timer-management                      +*/
/*+               if a Cbf is present, it is called if the timer elapses.   +*/
/*+               if no Cbf is present, when the timer elapses the output   +*/
/*+               Macro LLDP_DO_TIMER_REQUEST is called with the rqb        +*/
/*+               spezified (only if rqb is not underway).                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_TimerSet(
    LSA_TIMER_ID_TYPE                 TimerID,
    LLDP_UPPER_RQB_PTR_TYPE           pRQB,
    LSA_VOID                          LSA_FCT_PTR(LLDP_LOCAL_FCT_ATTR, Cbf)
    (LLDP_INTERNAL_REQUEST_PTR_TYPE   pIntRQB),
    LLDP_LOCAL_MEM_U16_PTR_TYPE       pLLDPTimerID)

{
    LLDP_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_UINT16                      i;
    LSA_BOOL                        Found;

    pTimer  = LLDP_MGM_GET_GLOB_PTR()->Timer;
    i       = 0;
    Found   = LSA_FALSE;
    while (( i <LLDP_CFG_MAX_TIMER ) && (!Found ))
    {
        if (! pTimer[i].InUse )
        {
            Found = LSA_TRUE;
            pTimer[i].TimerID       = TimerID;
            pTimer[i].IntRQB.pRQB   = pRQB;
            pTimer[i].IntRQB.InUse  = LSA_FALSE;
            pTimer[i].Cbf           = Cbf;
            /* setup RQB */
            LLDP_RQB_SET_OPCODE(pRQB,LLDP_OPC_TIMER);
            pRQB->args.Timer.TimerID = i;  /* LLDP-Timer ID */
            /* set InUse last to be sure other entrys are */
            /* valid.                                     */
            pTimer[i].InUse         = LSA_TRUE;
            *pLLDPTimerID           = i;
        };
        i++;
    }
    if (!Found )
    {
        LLDP_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_FATAL,"LLDP_TimerSet(): No more Timer-Resources. LLDP_CFG_MAX_TIMER wrong?");
        LLDP_FATAL(LLDP_FATAL_ERR_TIMER_RESOURCE);
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   LLDP_TimerAlloc                              +*/
/*+  Input/Output              LSA_UINT16                       TimerType   +*/
/*+                            LSA_UINT16                       TimerBase   +*/
/*+                            LSA_UINT32                       UserID      +*/
/*+                            LSA_FCT_PTR(LLDP_LOCAL_FCT_ATTR, Cbf) (..)   +*/
/*+                            LLDP_LOCAL_MEM_U16_PTR_TYPE      pLLDPTimerID+*/
/*+                                                                         +*/
/*+  Result                :   LLDP_RSP_OK                                  +*/
/*+                            LLDP_RSP_ERR_RESOURCE                        +*/
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
/*+  pLLDPTimerID LLDP internal Timer ID (= index ) returned                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates LSA-Timer and setup LLDP-Timer management       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP  LLDP_LOCAL_FCT_ATTR LLDP_TimerAlloc(
    LSA_UINT16                        TimerType,
    LSA_UINT16                        TimerBase,
    LSA_UINT32                        UserID,
    lldpTimerCbf                      Cbf,          /* AP01539304 */
    LLDP_LOCAL_MEM_U16_PTR_TYPE       pLLDPTimerID)
{

    LSA_UINT16              RetVal;
    LLDP_RSP                Response;
    LSA_TIMER_ID_TYPE       TimerID;
    LLDP_UPPER_RQB_PTR_TYPE pRQB;

    LLDP_FUNCTION_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_TimerAlloc(Type: 0x%X, Base: 0x%X, UserID: 0x%X)",TimerType,TimerBase,UserID);

    Response = LLDP_RSP_OK;
    LLDP_ALLOC_UPPER_RQB_LOCAL(&pRQB,sizeof(LLDP_RQB_TYPE));
    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        Response = LLDP_RSP_ERR_RESOURCE;
        LLDP_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"LLDP_TimerAlloc(). Allocation RQB memory failed.");
    }
    else
    {
        LLDP_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"-->: Calling LLDP_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",TimerType,TimerBase);
        LLDP_ALLOC_TIMER(&RetVal,&TimerID,TimerType,TimerBase);
        if ( RetVal == LSA_RET_OK)
        {
            LLDP_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<--: Calling LLDP_ALLOC_TIMER success. TimerID: %d.",TimerID);
            /* setup RQB-user-ID */
            pRQB->args.Timer.UserID  = UserID;
            LLDP_TimerSet(TimerID,pRQB,Cbf,pLLDPTimerID);
        }
        else
        {
            Response = LLDP_RSP_ERR_RESOURCE;
            LLDP_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<--: Calling LLDP_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
            LLDP_FREE_UPPER_RQB_LOCAL(&RetVal,pRQB);
            LLDP_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }
    LLDP_FUNCTION_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_TimerAlloc(0x%X) LLDPTimerID: 0x%X",Response,*pLLDPTimerID);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_TimerFree                              +*/
/*+  Input/Output          :    LSA_UINT16                   LLDPTimerID    +*/
/*+  Result                :    LLDP_RSP_OK                                 +*/
/*+                             LLDP_RPS_ERR_SEQUENCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  LLDPTimerID   LLDP-Timer ID to free in management                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees timer in timermanagement and releases LSA-Timer     +*/
/*+               Fees internal RQB. If RQB still in use we abort with error+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP  LLDP_LOCAL_FCT_ATTR LLDP_TimerFree(LSA_UINT16 LLDPTimerID)
{
    LLDP_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_UINT16                      RetVal;
    LSA_TIMER_ID_TYPE               TimerID;
    LLDP_RSP                        Response;

    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_TimerFree(LLDPTimerID: 0x%X)",LLDPTimerID);
    Response = LLDP_RSP_OK;
    if ( LLDPTimerID < LLDP_CFG_MAX_TIMER )
    {
        pTimer = &LLDP_MGM_GET_GLOB_PTR()->Timer[LLDPTimerID];
        if ( pTimer->InUse )
        {
            if (!pTimer->IntRQB.InUse )
            {
                TimerID = pTimer->TimerID;
                LLDP_STOP_TIMER(&RetVal,TimerID); /* just for sure. dont matter if already*/
                /* stopped                              */
                LLDP_FREE_TIMER(&RetVal,TimerID);
                pTimer->InUse = LSA_FALSE;
                /* free internal RQB for Timer */
                LLDP_FREE_UPPER_RQB_LOCAL(&RetVal,pTimer->IntRQB.pRQB);
                LLDP_MEM_FREE_FAIL_CHECK(RetVal);
                pTimer->IntRQB.pRQB  = LSA_NULL;
                pTimer->IntRQB.InUse = LSA_FALSE;
            }
            else
            {
                Response = LLDP_RSP_ERR_SEQUENCE;
                LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"LLDP_TimerFree(): RQB still in use! (0x%X).",LLDPTimerID);
            }
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_FATAL,"LLDP_TimerFree(): Invalid Timer-ID(0x%X)",LLDPTimerID);
        LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
    }
    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_TimerFree(0x%X)",Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_TimerStart                             +*/
/*+  Input/Output          :    LSA_UINT16                   LLDPTimerID    +*/
/*+                             LSA_UINT16                   Time           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  LLDPTimerID   LLDP-Timer ID to free in management                      +*/
/*+  Time          running time                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_TimerStart(
    LLDP_HDB_PTR_TYPE   pHDB,
    LSA_UINT16          LLDPTimerID,
    LSA_UINT16          Time)
{
    LSA_UINT16                      RetVal;
    LLDP_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_USER_ID_TYPE                Dummy;

    LSA_UNUSED_ARG(pHDB);   /* may be unused depending on the trace compilelevel */
    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_TimerStart(LLDPTimerID: 0x%X, Time: 0x%X)",LLDPTimerID,Time);
    if ( LLDPTimerID < LLDP_CFG_MAX_TIMER )
    {
        pTimer = &LLDP_MGM_GET_GLOB_PTR()->Timer[LLDPTimerID];
        if ( pTimer->InUse )
        {
            Dummy.uvar32 = LLDPTimerID; /* we use user-id for LLDP-Timer ID */
            LLDP_START_TIMER(&RetVal,
                             pTimer->TimerID,
                             Dummy,
                             Time);
            if ( RetVal == LSA_RET_ERR_PARAM )
            {
                LLDP_SYSTEM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"<--: Calling LLDP_START_TIMER failed (Response: 0x%X).",RetVal);
                LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
            }
        }
        else
        {
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"LLDP_TimerStart(): Timer not in use (0x%X).",LLDPTimerID);
            LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"LLDP_TimerStart(): Wrong Timer ID (0x%X).",LLDPTimerID);
        LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_TimerStart()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_TimerStop                              +*/
/*+  Input/Output          :    LSA_UINT16                   LLDPTimerID    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  LLDPTimerID   LLDP-Timer ID to free in management                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_TimerStop(
    LLDP_HDB_PTR_TYPE       pHDB,
    LSA_UINT16              LLDPTimerID)
{
    LSA_UINT16                      RetVal;
    LLDP_TIMER_INFO_PTR_TYPE        pTimer;

    LSA_UNUSED_ARG(pHDB);   /* may be unused depending on the trace compilelevel */
    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_TimerStop(LLDPTimerID: 0x%X)",LLDPTimerID);

    if ( LLDPTimerID < LLDP_CFG_MAX_TIMER )
    {
        pTimer = &LLDP_MGM_GET_GLOB_PTR()->Timer[LLDPTimerID];
        if ( pTimer->InUse )
        {
            LLDP_STOP_TIMER(&RetVal,pTimer->TimerID);
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"LLDP_TimerStart(): Wrong Timer ID (0x%X).",LLDPTimerID);
        LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_TimerStop()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_TimerRQBFree                           +*/
/*+  Input/Output          :    LSA_UINT16                   LLDPTimerID    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  LLDPTimerID   LLDP-Timer ID to free in management                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after RQB was received and is free for further use +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_TimerRQBFree(
    LSA_UINT16 LLDPTimerID)
{
    LLDP_TIMER_INFO_PTR_TYPE   pTimer;

    pTimer = &LLDP_MGM_GET_GLOB_PTR()->Timer[LLDPTimerID];
    pTimer->IntRQB.InUse = LSA_FALSE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_TimerRQBCheckFree                      +*/
/*+  Input/Output          :    LSA_UINT16                   LLDPTimerID    +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  LLDPTimerID   LLDP-Timer ID to free in management                      +*/
/*+                                                                         +*/
/*+  Result : LSA_TRUE: Timer RQB is not in use                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check if Timer RQB is free.                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP  LLDP_LOCAL_FCT_ATTR LLDP_TimerRQBCheckFree(
    LSA_UINT16 LLDPTimerID)
{
    LLDP_TIMER_INFO_PTR_TYPE   pTimer;
    LLDP_RSP                   Status;

    pTimer = &LLDP_MGM_GET_GLOB_PTR()->Timer[LLDPTimerID];
    if (pTimer->IntRQB.InUse )
    {
        Status = LSA_FALSE;
    }
    else
    {
        Status = LSA_TRUE;
    }
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    lldp_timeout                                +*/
/*+  Input/Output          :    LSA_TIMER_ID_TYPE timer_id                  +*/
/*+                             LSA_USER_ID_TYPE  user_id                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  timer_id     : ID of Timer                                             +*/
/*+  user_id      : Used for LLDPTimerID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called from systemadaption on timerevent. May be called   +*/
/*+               within ISR-Handler.                                       +*/
/*+                                                                         +*/
/*+               LSA-Systemfunction                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_SYSTEM_IN_FCT_ATTR  lldp_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id)
{
    LLDP_TIMER_INFO_PTR_TYPE   pTimer;
    LSA_UINT32                 LLDPTimerID;

    LLDPTimerID = (LSA_UINT16) user_id.uvar32;
    #ifdef LLDP_CFG_USE_ENTER_EXIT_TIMEOUT
    LLDP_ENTER();
    #endif

    LLDP_FUNCTION_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,"IN : lldp_timeout(timer_id: 0x%X, user_id: 0x%X)",timer_id,user_id.uvar32);
    if ( LLDPTimerID < LLDP_CFG_MAX_TIMER )
    {
        pTimer = &LLDP_MGM_GET_GLOB_PTR()->Timer[LLDPTimerID];
        if (( pTimer->InUse ) &&
            ( pTimer->TimerID == timer_id ))
        {
            /* if a callbackfunction is present, call it */
            if (!LSA_HOST_PTR_ARE_EQUAL(pTimer->Cbf, LSA_NULL) )
            {
                pTimer->Cbf(&pTimer->IntRQB);
            }
            else /* do standard handling..*/
            {
                if (!pTimer->IntRQB.InUse )
                {
                    /* LLDP_DBG_ACT("Calling LLDP_DO_TIMER_REQUEST(pRQB: %Xh)\r\n",(LSA_UINT32)pTimer->IntRQB.pRQB,0,0); */
                    /* Note that the RQB is already filled up in startup! */
                    pTimer->IntRQB.InUse = LSA_TRUE;
                    LLDP_DO_TIMER_REQUEST(pTimer->IntRQB.pRQB);
                }
            }
        }
        else
        {
            LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"lldp_timeout(): Invalid Timer-ID (0x%X).",LLDPTimerID);
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_FATAL,"lldp_timeout(): Invalid Timer-ID (0x%X).",LLDPTimerID);
        LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
    }
    LLDP_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: lldp_timeout()");
    #ifdef LLDP_CFG_USE_ENTER_EXIT_TIMEOUT
    LLDP_EXIT();
    #endif
}

/*****************************************************************************/
/*  end of file LLDP_TIM.C                                                   */
/*****************************************************************************/


