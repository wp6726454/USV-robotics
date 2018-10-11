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
/*  F i l e               &F: oha_tim.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA-Timer functions                              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  12.06.06    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_TIM */

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
/*                             external functions                            */
/*===========================================================================*/

/*===========================================================================*/
/*=====  Timer -functions                                                ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TimerCB                                 +*/
/*+  Input                      OHA_INTERNAL_REQUEST_PTR_TYPE   pIntRQB     +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to internal RQB-mgm.                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles Timerevent for Cylcic Timer.                      +*/
/*+               Called by OHA-Timerhandler                                +*/
/*+                                                                         +*/
/*+               It can be configured if we should call the output macro   +*/
/*+               OHA_DO_TIMER_REQUEST for doing a context change or if     +*/
/*+               we should do the handling inside the function.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerCB(
    OHA_INTERNAL_REQUEST_PTR_TYPE   pIntRQB)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerCB(pIntRQB: 0x%X)",
                          pIntRQB);

    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup! */
        pIntRQB->InUse = LSA_TRUE;

#ifndef OHA_CFG_HANDLE_DO_TIMEOUT_INTERN
        OHA_DO_TIMER_REQUEST(pIntRQB->pRQB); /* at least calls OHA_RequestTimer() */
#else
        OHA_RequestTimer(pIntRQB->pRQB);    /* in another thread, ENTER and EXIT */
#endif
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TimerCB");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TimerMgmSetup                           +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the OHA global Timer-Management               +*/
/*+               Note that the global management hast to be allocated.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_TimerMgmSetup(LSA_VOID)
{
    LSA_UINT32 i;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerMgmSetup()");

    OHA_ASSERT_NULL_PTR(OHA_GET_GLOB_PTR());

    for ( i= 0; i< OHA_CFG_MAX_TIMER; i++ )
    {
        OHA_GET_GLOB_PTR()->Timer[i].InUse = LSA_FALSE;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TimerMgmSetup");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   OHA_TimerSet                                 +*/
/*+  Input/Output          :   LSA_UINT16                      TimerID      +*/
/*+                            OHA_UPPER_RQB_PTR_TYPE          pRQB         +*/
/*+                            LSA_FCT_PTR(OHA_LOCAL_FCT_ATTR, Cbf) (..)    +*/
/*+                            OHA_LOCAL_MEM_U16_PTR_TYPE      pOHATimerID  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID put into timer-management                    +*/
/*+  pRQB         Pointer to intenal RQB put into timer-management          +*/
/*+  Cbf          Optional Callbackfunction called when timer elapses.      +*/
/*+               Gets pointer to IntRQB as parameter                       +*/
/*+  pOHATimerID  OHA internal Timer ID (= index ) returned                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Puts new Timer into Timer-management                      +*/
/*+               if a Cbf is present, it is called if the timer elapses.   +*/
/*+               if no Cbf is present, when the timer elapses the output   +*/
/*+               Macro OHA_DO_TIMER_REQUEST is called with the rqb         +*/
/*+               spezified (only if rqb is not underway).                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_TimerSet(
    LSA_UINT16                      TimerID,
    OHA_UPPER_RQB_PTR_TYPE         pRQB,
    LSA_VOID                        LSA_FCT_PTR(OHA_LOCAL_FCT_ATTR, Cbf)
    (OHA_INTERNAL_REQUEST_PTR_TYPE   pIntRQB),
    OHA_LOCAL_MEM_U16_PTR_TYPE     pOHATimerID)
{
    OHA_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_UINT16                      i;
    LSA_BOOL                        Found;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerSet(TimerID: 0x%X, pRQB: 0x%X, Cbf: 0x%X)",
                          TimerID,pRQB,Cbf);

    pTimer  = OHA_GET_GLOB_PTR()->Timer;

    i       = 0;
    Found   = LSA_FALSE;

    while (( i <OHA_CFG_MAX_TIMER ) && ( ! Found ))
    {

        if (! pTimer[i].InUse )
        {
            Found = LSA_TRUE;
            pTimer[i].TimerID       = TimerID;
            pTimer[i].IntRQB.pRQB   = pRQB;
            pTimer[i].IntRQB.InUse  = LSA_FALSE;
            pTimer[i].Cbf           = Cbf;

            /* setup RQB */
            OHA_RQB_SET_OPCODE(pRQB,OHA_OPC_TIMER);
            pRQB->args.Timer.TimerID = i;  /* OHA-Timer ID */

            /* set InUse last to be sure other entrys are */
            /* valid.                                     */
            pTimer[i].InUse         = LSA_TRUE;
            *pOHATimerID            = i;
        };
        i++;
    }

    if ( ! Found )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"OHA_TimerSet(): No more Timer-Resources. OHA_CFG_MAX_TIMER wrong?");
        OHA_FATAL(OHA_FATAL_ERR_TIMER_RESOURCE);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TimerSet");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   OHA_TimerAlloc                               +*/
/*+  Input/Output              LSA_UINT16                      TimerType    +*/
/*+                            LSA_UINT16                      TimerBase    +*/
/*+                            LSA_UINT32                      UserID       +*/
/*+                            LSA_FCT_PTR(OHA_LOCAL_FCT_ATTR, Cbf) (..)    +*/
/*+                            OHA_LOCAL_MEM_U16_PTR_TYPE      pOHATimerID  +*/
/*+                                                                         +*/
/*+  Result                :   OHA_OK                                       +*/
/*+                            OHA_ERR_RESOURCE                             +*/
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
/*+  pOHATimerID  OHA internal Timer ID (= index ) returned                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates LSA-Timer and setup OHA-Timer management        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_TimerAlloc(
    LSA_UINT16                      TimerType,
    LSA_UINT16                      TimerBase,
    LSA_UINT32                      UserID,
    OHA_LOCAL_CALLBACK_FCT_PTR_TYPE Cbf,   /* AP01539318 */
    OHA_LOCAL_MEM_U16_PTR_TYPE     pOHATimerID)
{
    LSA_UINT16             Response;

    OHA_UPPER_RQB_PTR_TYPE pRQB = LSA_NULL;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerAlloc(Type: 0x%X, Base: 0x%X, UserID: 0x%X)",
                          TimerType,TimerBase,UserID);

    Response = OHA_OK;

    OHA_ALLOC_UPPER_RQB_LOCAL(&pRQB,sizeof(OHA_RQB_TYPE));

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        Response = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"OHA_TimerAlloc(). Allocation RQB memory failed.");
    }
    else
    {
        LSA_UINT16             RetVal  = LSA_RET_OK;
        LSA_UINT16             TimerID = 0;

        OHA_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling OHA_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",TimerType,TimerBase);

        OHA_ALLOC_TIMER(&RetVal, &TimerID, TimerType, TimerBase);

        if ( RetVal == LSA_RET_OK)
        {
            OHA_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"<--: Calling OHA_ALLOC_TIMER success. TimerID: %d.",TimerID);

            /* setup RQB-user-ID */
            pRQB->args.Timer.UserID  = UserID;

            OHA_TimerSet(TimerID,pRQB,Cbf,pOHATimerID);
        }
        else
        {
            Response = OHA_ERR_RESOURCE;
            OHA_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"<--: Calling OHA_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
            OHA_FREE_UPPER_RQB_LOCAL(&RetVal,pRQB);
            OHA_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_TimerAlloc(0x%X) OHATimerID: 0x%X",
                          Response,*pOHATimerID);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TimerFree                               +*/
/*+  Input/Output          :    LSA_UINT16                   OHATimerID     +*/
/*+  Result                :    OHA_OK                                      +*/
/*+                             OHA_ERR_SEQUENCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHATimerID   OHA-Timer ID to free in management                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees timer in timermanagement and releases LSA-Timer     +*/
/*+               Fees internal RQB. If RQB still in use we abort with error+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_TimerFree(LSA_UINT16 OHATimerID)

{
    OHA_TIMER_INFO_PTR_TYPE        pTimer;
    LSA_UINT16                     TimerID;
    LSA_UINT16                     Response;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerFree(OHATimerID: 0x%X)",
                          OHATimerID);

    Response = OHA_OK;

    if ( OHATimerID < OHA_CFG_MAX_TIMER )
    {
        pTimer = &OHA_GET_GLOB_PTR()->Timer[OHATimerID];

        if ( pTimer->InUse )
        {
            if ( ! pTimer->IntRQB.InUse )
            {
                LSA_UINT16  RetVal = LSA_RET_OK;

                TimerID = pTimer->TimerID;

                OHA_STOP_TIMER(&RetVal,TimerID); /* just for sure. dont matter if */
                /* already stopped               */
                OHA_FREE_TIMER(&RetVal,TimerID);

                pTimer->InUse = LSA_FALSE;

                /* free internal RQB for Timer */
                OHA_FREE_UPPER_RQB_LOCAL(&RetVal,pTimer->IntRQB.pRQB);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);

                pTimer->IntRQB.pRQB  = LSA_NULL;
                pTimer->IntRQB.InUse = LSA_FALSE;
            }
            else
            {
                Response = OHA_ERR_SEQUENCE;
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_TimerFree(): Timer-ID(0x%X), RQB still in use!.",OHATimerID);
            }
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_TimerFree(): Invalid Timer-ID(0x%X)",OHATimerID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_TimerFree(0x%X)",
                          Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TimerStart                              +*/
/*+  Input/Output          :    LSA_UINT16                   OHATimerID     +*/
/*+                             LSA_UINT16                   Time           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHATimerID    OHA-Timer ID to free in management                       +*/
/*+  Time          running time                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerStart(
    LSA_UINT16 OHATimerID,
    LSA_UINT16 Time)
{
    OHA_TIMER_INFO_PTR_TYPE         pTimer;
    LSA_USER_ID_TYPE                Dummy;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerStart(OHATimerID: 0x%X, Time: 0x%X)",
                          OHATimerID,Time);

    if ( OHATimerID < OHA_CFG_MAX_TIMER )
    {
        pTimer = &OHA_GET_GLOB_PTR()->Timer[OHATimerID];

        if ( pTimer->InUse )
        {
            LSA_UINT16 RetVal = LSA_RET_OK;

            Dummy.uvar32 = OHATimerID; /* we use user-id for OHA-Timer ID */

            OHA_START_TIMER(&RetVal,
                            pTimer->TimerID,
                            Dummy,
                            Time);

            if ( RetVal == LSA_RET_ERR_PARAM )
            {
                OHA_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"<--: Calling OHA_START_TIMER failed (Response: 0x%X).",RetVal);
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            }
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_TimerStart(): Timer not in use (0x%X).",OHATimerID);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_TimerStart(): Wrong Timer ID (0x%X).",OHATimerID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TimerStart()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TimerStop                               +*/
/*+  Input/Output          :    LSA_UINT16                    OHATimerID    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHATimerID   OHA-Timer ID to free in management                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerStop(
    LSA_UINT16 OHATimerID)

{
    LSA_UINT16                     RetVal;
    OHA_TIMER_INFO_PTR_TYPE        pTimer;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerStop(OHATimerID: 0x%X)",
                          OHATimerID);

    if ( OHATimerID < OHA_CFG_MAX_TIMER )
    {
        pTimer = &OHA_GET_GLOB_PTR()->Timer[OHATimerID];

        if ( pTimer->InUse )
        {
            OHA_STOP_TIMER(&RetVal, pTimer->TimerID);
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_TimerStart(): Wrong Timer ID (0x%X).",OHATimerID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TimerStop()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TimerRQBFree                            +*/
/*+  Input/Output          :    LSA_UINT16                   OHATimerID     +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHATimerID   OHA-Timer ID to free in management                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after RQB was received and is free for further use +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerRQBFree(
    LSA_UINT16 OHATimerID)

{
    OHA_TIMER_INFO_PTR_TYPE   pTimer;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerRQBFree(OHATimerID: 0x%X)",
                          OHATimerID);

    pTimer = &OHA_GET_GLOB_PTR()->Timer[OHATimerID];
    pTimer->IntRQB.InUse = LSA_FALSE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TimerRQBFree()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TimerRQBCheckFree                       +*/
/*+  Input/Output          :    LSA_UINT16                    OHATimerID    +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHATimerID   OHA-Timer ID to free in management                        +*/
/*+                                                                         +*/
/*+  Result : LSA_TRUE: Timer RQB is not in use                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check if Timer RQB is free.                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_TimerRQBCheckFree(
    LSA_UINT16 OHATimerID)

{
    OHA_TIMER_INFO_PTR_TYPE   pTimer;
    LSA_UINT16                Status;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TimerRQBCheckFree(OHATimerID: 0x%X)",
                          OHATimerID);

    pTimer = &OHA_GET_GLOB_PTR()->Timer[OHATimerID];
    if (pTimer->IntRQB.InUse ) Status = LSA_FALSE;
    else  Status = LSA_TRUE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_TimerRQBCheckFree(0x%X)",
                          Status);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_timeout                                 +*/
/*+  Input/Output          :    LSA_UINT16        timer_id                  +*/
/*+                             LSA_USER_ID_TYPE  user_id                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  timer_id     : ID of Timer                                             +*/
/*+  user_id      : Used for OHATimerID                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called from systemadaption on timerevent. May be called   +*/
/*+               within ISR-Handler.                                       +*/
/*+                                                                         +*/
/*+               LSA-Systemfunction                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_SYSTEM_IN_FCT_ATTR  oha_timeout(
    LSA_UINT16        timer_id,
    LSA_USER_ID_TYPE  user_id)
{

    OHA_TIMER_INFO_PTR_TYPE   pTimer;
    LSA_UINT32                OHATimerID;

    OHATimerID = (LSA_UINT16) user_id.uvar32;

#ifdef OHA_CFG_USE_ENTER_EXIT_TIMEOUT
    OHA_ENTER();
#endif

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_timeout(timer_id: 0x%X, user_id: 0x%X)",
                          timer_id,user_id.uvar32);


    if ( OHATimerID < OHA_CFG_MAX_TIMER )
    {
        pTimer = &OHA_GET_GLOB_PTR()->Timer[OHATimerID];

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
                    /* OHA_DBG_ACT("Calling OHA_DO_TIMER_REQUEST(pRQB: %Xh)\r\n",(LSA_UINT32)pTimer->IntRQB.pRQB,0,0); */
                    /* Note that the RQB is already filled up in startup! */
                    pTimer->IntRQB.InUse = LSA_TRUE;
                    OHA_DO_TIMER_REQUEST(pTimer->IntRQB.pRQB);
                }
            }
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_timeout(): Invalid Timer-ID (0x%X).",OHATimerID);
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"oha_timeout(): Invalid Timer-ID (0x%X).",OHATimerID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_timeout()");

#ifdef OHA_CFG_USE_ENTER_EXIT_TIMEOUT
    OHA_EXIT();
#endif

    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestTimer                            +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     OHA_OPC_TIMER                   +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     OHA_RQB_ERROR_TYPE      Error       Addition errorcodes from LowerL.+*/
/*+     OHA_RQB_ARGS_TYPE       args.Timer: OHA_RQB_TIMER_TYPE              +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : OHA-Timer ID                                             +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from OHA within oha_timeout().          +*/
/*+               Dispatches Timer event.                                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_RequestTimer(OHA_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT32    i;
    LSA_UINT16    j;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestTimer(pRQB: 0x%X)",
                          pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);

    if (pRQB->args.Timer.TimerID == OHA_TIMER_ID_INVALID)
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestTimer. Invalid TimerID (0x%X)",pRQB->args.Timer.TimerID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    /* a timer for delaying the PTCP diagnosis ? */
    if (OHA_GET_GLOB_PTR()->CyclicTimerID != pRQB->args.Timer.TimerID)
    {
        OHA_HDB_USER_PTR_TYPE   pHDBUser = OHA_GET_HUSER_PTR(0);

        for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
        {
            for (j=0; j < OHA_CFG_MAX_PORTS; j++)
            {
                if ( pHDBUser->PtcpTimerID[j] != OHA_TIMER_ID_INVALID )
                {
                    if (pHDBUser->PtcpTimerID[j] == pRQB->args.Timer.TimerID)
                    {
                        /* we've found the timer - now check if the user-channel is to close ! */
                        if (OHA_IS_NOT_NULL(pHDBUser->pOCReq) &&
                            OHA_RQB_GET_OPCODE(pHDBUser->pOCReq)==OHA_OPC_CLOSE_CHANNEL)
                        {
                            /* timeout, the timer must have been stopped by closing the channel */
                            if (pHDBUser->PtcpTimerInUse[j])
                            {
                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_Request(Ptcp)Timer. Timer not stopped, TimerID (0x%X)",pRQB->args.Timer.TimerID);
                                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                            }

                            /* free timer */
                            OHA_TimerRQBFree(pRQB->args.Timer.TimerID);

                            /* we can call a close channel */
                            OHA_USERCloseChannelDone(pHDBUser, OHA_OK);

                            OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestTimer()");
                            return;
                        }

                        OHA_RequestPtcpTimer(pRQB, pHDBUser, (LSA_UINT16)(j+1));
                        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestTimer(TimerID (0x%X))", pRQB->args.Timer.TimerID);
                        return;
                    }
                }
            }
        }
    }
#ifndef OHA_CFG_NO_NARE
    else
    {
        OHA_RequestCyclicCheck(pRQB);   /* the cyclic timer for check address   */
        OHA_RequestCyclicTimer(pRQB);   /* the cyclic timer for resolve address */

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestTimer(TimerID (0x%X))", pRQB->args.Timer.TimerID);
        return;
    }
#endif

    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestTimer. TimerID (0x%X) not found",pRQB->args.Timer.TimerID);
    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestTimer()");
}


#ifndef OHA_CFG_NO_NARE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestCyclicTimer                      +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     OHA_OPC_TIMER                   +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     OHA_RQB_ERROR_TYPE      Error       Addition errorcodes from LowerL.+*/
/*+     OHA_RQB_ARGS_TYPE       args.Timer: OHA_RQB_TIMER_TYPE              +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : OHA-Timer ID                                             +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from OHA within oha_timeout().          +*/
/*+               Handles Cyclic Timer event.                               +*/
/*+                                                                         +*/
/*+               Cyclic checks for NameOfStation and/or IP address.        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_RequestCyclicTimer(OHA_UPPER_RQB_PTR_TYPE pRQB)
{
    OHA_HDB_USER_PTR_TYPE        pHDB;
    LSA_UINT32                   i;
    LSA_UINT32                   TimerCnt;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestTimer(pRQB: 0x%X)",
                          pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);

#if 0  /* check does not work, Timer.UserID=pHDBUser->ThisHandle and Timer.UserID=OHA_TIMER_USER_ID_CYLIC overlaps! */ 
    if (pRQB->args.Timer.UserID != OHA_TIMER_USER_ID_CYLIC )
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestCyclicTimer. Invalid Timer-UserID (0x%X)",pRQB->args.Timer.UserID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
#endif

    OHA_TimerRQBFree(pRQB->args.Timer.TimerID);

    pHDB = OHA_GET_HUSER_PTR(0);
    OHA_ASSERT_NULL_PTR(pHDB);

    TimerCnt = ++OHA_GET_GLOB_PTR()->CyclicTimerCnt;

    /* For all User-Channels we do the timer handling. */

    for (i=0; i<OHA_CFG_MAX_CHANNELS; i++, pHDB++)
    {
        LSA_UINT32            j;

        /* check if we are in waiting for Timer to close the channel. If so we */
        /* finish the close-handling                                           */
        /* Note: Currently we only have one User-Handle!!                      */

        /* only if the CheckInterval is in use */
        if (OHA_IS_NULL(pHDB->Params.pDetail) || pHDB->Params.pDetail->Params.User.CheckInterval == 0)
        {
            continue;
        }

        switch (pHDB->State)
        {
            case OHA_HANDLE_STATE_CLOSE_WAIT_TIMER:
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Timer event while closing. continue.");

                if ( ! LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) )
                {
                    /* timer RQB is free now. */
                    /* OHA_UserChannelStateMachine(pHDB,OHA_OK); */
                    pHDB->State = OHA_HANDLE_STATE_OPEN; /* OHA_HANDLE_STATE_CLOSE_USER_RUNNING */

                    OHA_USERCloseChannelDone(pHDB,OHA_OK); /* further actions taken in channel handler */
                }
                else
                {
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"pOCReq is NULL. Cannot Close channel");
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }
                break;

            case OHA_HANDLE_STATE_OPEN:
                for ( j = 0; j < OHA_CFG_MAX_NICS; j++) /* for all Nic's */
                {
                    OHA_CHECK_INFO_PTR_TYPE pInfo;
                    LSA_UINT32              k;

                    /* both checks: NameOfStation and IP */
                    for (k=0, pInfo = &pHDB->NameCheckInfo[j]; k < 2; k++, pInfo = &pHDB->IpCheckInfo[j])
                    {
                        /* ----------------------------------------------- */
                        /* Send-handling first                             */
                        /* check for requests to be send and start sending */
                        /* ----------------------------------------------- */
                        if ( pInfo->State == OHA_CHECK_STATE_START )
                        {
                            /* send timer reached ? */
                            if ( pInfo->NextSend == TimerCnt )
                            {
                                /* set timer counter for next send */
                                pInfo->NextSend += pInfo->Interval;

                                /* if there is another cyclic request look at the grid */
                                /* and correct the distance to 5 seconds               */

                                if ( ! pInfo->Underway )
                                {
                                    OHA_NARE_RQB_PTR_TYPE   pRQB = pInfo->pCheckRQB;

                                    /* only send if not active */
                                    if ( OHA_IS_NOT_NULL(pRQB) )
                                    {
                                        OHA_HDB_NARE_PTR_TYPE   pHDBNare;
                                        LSA_UINT16              Orginator;
                                        OHA_COMMON_MEM_U8_PTR_TYPE LocIpAdrPtr      = LSA_NULL;
                                        OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr    = LSA_NULL;
                                        OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtrPtr = LSA_NULL;

                                        /* orginator must be NARE */
                                        Orginator = OHA_NARE_RQB_GET_USERID_UVAR16(pRQB);
                                        OHA_ASSERT (OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_NARE);

                                        pHDBNare = OHA_GET_HNARE_PTR(OHA_HANDLE_IDX(Orginator));
                                        OHA_ASSERT_NULL_PTR(pHDBNare);

                                        /* read local IP-Address, Subnet-mask and Default Router Address */
                                        (LSA_VOID)OHA_DB_READ_IP_SUITE(pHDBNare->Params.pDetail->Params.Nare.NICId, &LocIpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr);
                                        OHA_ASSERT_NULL_PTR(LocIpAdrPtr);

                                        OHA_NARE_RQB_SET_HANDLE(pRQB,pHDBNare->NareHandle);

                                        switch OHA_NARE_RQB_GET_OPCODE(pRQB)
                                        {
                                            case NARE_OPC_TEST_IP:
												/* AP00842368 Checking the IP-address by NARE (Gratuitous ARP): Sender IP must be 0.0.0.0 */
												OHA_MEMCPY (pRQB->args.TestIP.LocalIPAddr, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP, NARE_IP_ADDR_SIZE);
                                                break;
                                            case NARE_OPC_RESOLVE_CONFIG:
                                                OHA_MEMCPY (pRQB->args.ResConf.LocalIPAddr, LocIpAdrPtr, NARE_IP_ADDR_SIZE);
                                                break;
                                            default:
                                                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestCyclicTimer(): Wrong NARE-Opcode (%u)", OHA_NARE_RQB_GET_OPCODE(pRQB));
                                                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                                                break;
                                        }

                                        pInfo->Underway = LSA_TRUE;

                                        OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW
                                                             , "Check Request for channel %u, interface %u, opcode %u", i+1, j+1, OHA_NARE_RQB_GET_OPCODE(pRQB));
                                        OHA_NARE_REQUEST_LOWER(pRQB,pHDBNare->Params.pSys);
                                    }
                                }
                                else
                                {
                                    pInfo->StillUnderway++; /* for Info */
                                }
                            }
                        }
                    }
                }
                break;

            default:    /* ignore timer */
                break;
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestCyclicTimer()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestCyclicCheck                      +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     OHA_OPC_TIMER                   +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     OHA_RQB_ERROR_TYPE      Error       Addition errorcodes from LowerL.+*/
/*+     OHA_RQB_ARGS_TYPE       args.Timer: OHA_RQB_TIMER_TYPE              +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : OHA-Timer ID                                             +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from OHA within oha_timeout().          +*/
/*+               Handles Cyclic Timer event.                               +*/
/*+                                                                         +*/
/*+               Cyclic checks for NameOfStation and/or IP address if      +*/
/*+               a conflict is detected                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_RequestCyclicCheck(OHA_UPPER_RQB_PTR_TYPE pRQB)
{
    OHA_HDB_NARE_PTR_TYPE        pHDBNare;
    LSA_UINT32                   i;
    LSA_UINT32                   TimerCnt;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestCyclicCheck(pRQB: 0x%X)",
                          pRQB);

    OHA_ASSERT_NULL_PTR(pRQB);

#if 0  /* check does not work, Timer.UserID=pHDBUser->ThisHandle and Timer.UserID=OHA_TIMER_USER_ID_CYLIC overlaps! */
    if (pRQB->args.Timer.UserID != OHA_TIMER_USER_ID_CYLIC )
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestCyclicCheck. Invalid Timer-UserID (0x%X)",pRQB->args.Timer.UserID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }
#endif

    pHDBNare = OHA_GET_HNARE_PTR(0);
    OHA_ASSERT_NULL_PTR(pHDBNare);

    TimerCnt = OHA_GET_GLOB_PTR()->CyclicTimerCnt;

    /* For all Nare-Channels we do the timer handling. */

    for (i=0; i < OHA_CFG_MAX_NICS; i++, pHDBNare++) /* for all Nic's */
    {
        /* only if the Channel is in use */
        if (OHA_IS_NULL(pHDBNare->Params.pDetail))
        {
            continue;
        }

        switch (pHDBNare->State)
        {
            case OHA_HANDLE_STATE_OPEN:
            {
                OHA_CHECK_INFO_PTR_TYPE pInfo = &pHDBNare->IpCheck;

                /* AP01522573: still one check: IP */
                {
                    /* ----------------------------------------------- */
                    /* Send-handling first                             */
                    /* check for requests to be send and start sending */
                    /* ----------------------------------------------- */
                    if ( pInfo->State == OHA_CHECK_STATE_START )
                    {
                        /* send timer reached ? */
                        if ( pInfo->NextSend == TimerCnt )
                        {
                            /* set timer counter for next send */
                            pInfo->NextSend += (pInfo->Interval * OHA_CHECK_GRID);

                            if ( ! pInfo->Underway )
                            {
                                OHA_NARE_RQB_PTR_TYPE   pRQB = pInfo->pCheckRQB;

                                /* only send if not active */
                                if ( OHA_IS_NOT_NULL(pRQB) )
                                {
                                    LSA_UINT16              Orginator;
                                    OHA_COMMON_MEM_U8_PTR_TYPE LocIpAdrPtr      = LSA_NULL;
                                    OHA_COMMON_MEM_U8_PTR_TYPE SubnetMaskPtr    = LSA_NULL;
                                    OHA_COMMON_MEM_U8_PTR_TYPE RouterAddrPtrPtr = LSA_NULL;

                                    /* orginator must be SYS */
                                    Orginator = OHA_NARE_RQB_GET_USERID_UVAR16(pRQB);
                                    if (OHA_HANDLE_GET_TYPE(Orginator) != OHA_HANDLE_TYPE_SYS)
									{
										OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestCyclicCheck. Invalid Orginator (0x%X)",Orginator);
										OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
									}

                                    /* read local IP-Address, Subnet-mask and Default Router Address */
                                    (LSA_VOID)OHA_DB_READ_IP_SUITE(pHDBNare->Params.pDetail->Params.Nare.NICId, &LocIpAdrPtr, &SubnetMaskPtr, &RouterAddrPtrPtr);
                                    OHA_ASSERT_NULL_PTR(LocIpAdrPtr);

                                    OHA_NARE_RQB_SET_HANDLE(pRQB,pHDBNare->NareHandle);

                                    switch OHA_NARE_RQB_GET_OPCODE(pRQB)
                                    {
                                        case NARE_OPC_TEST_IP:
											/* AP00842368 Checking the IP-address by NARE (Gratuitous ARP): Sender IP must be 0.0.0.0 */
											OHA_MEMCPY (pRQB->args.TestIP.LocalIPAddr, (OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_IP, NARE_IP_ADDR_SIZE);
                                            break;
                                        case NARE_OPC_RESOLVE_CONFIG:
                                            OHA_MEMCPY (pRQB->args.ResConf.LocalIPAddr, LocIpAdrPtr, NARE_IP_ADDR_SIZE);
                                            break;
                                        default:
                                            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestCyclicCheck(): Wrong NARE-Opcode (%u)", OHA_NARE_RQB_GET_OPCODE(pRQB));
                                            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                                            break;
                                    }

                                    pInfo->Underway = LSA_TRUE;

                                    OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW
                                                         , "Check Request for channel %u, opcode %u", i+1, OHA_NARE_RQB_GET_OPCODE(pRQB));
                                    OHA_NARE_REQUEST_LOWER(pRQB,pHDBNare->Params.pSys);
                                }
                            }
                            else
                            {
                                pInfo->StillUnderway++; /* for Info */
                            }
                        }
                    }
                }
            }
            break;

            default:    /* ignore timer */
                break;
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestCyclicCheck()");
}

#endif /* OHA_CFG_NO_NARE */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestPtcpTimer                        +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     OHA_OPC_TIMER                   +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     OHA_RQB_ERROR_TYPE      Error       Addition errorcodes from LowerL.+*/
/*+     OHA_RQB_ARGS_TYPE       args.Timer: OHA_RQB_TIMER_TYPE              +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : OHA-Timer ID                                             +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from OHA within oha_timeout().          +*/
/*+               Handles Timer delay for PTCP diagnosis.                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_RequestPtcpTimer(
    OHA_UPPER_RQB_PTR_TYPE pRQB,
    OHA_HDB_USER_PTR_TYPE  pHDBUser,
    LSA_UINT16             PortId)
{
    LSA_BOOL                RemoteMatch = LSA_FALSE;
    LSA_UINT16              RetVal;
    OHA_DB_TOPO_PTR_TYPE    pTopoDB;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestPtcpTimer(pRQB: 0x%X, pHDBUser: 0x%X, PortId: 0x%X)",
                          pRQB, pHDBUser, PortId);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

#if 0 /* check does not work, Timer.UserID=pHDBUser->ThisHandle and Timer.UserID=OHA_TIMER_USER_ID_CYLIC overlaps! */ 
    if (pRQB->args.Timer.UserID == OHA_TIMER_USER_ID_CYLIC )
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestPtcpTimer. Invalid Timer-UserID (0x%X)",pRQB->args.Timer.UserID);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        return;
    }
#endif

    if (!(PortId && PortId <= OHA_CFG_MAX_PORTS))
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_RequestPtcpTimer. Invalid PortId (0x%X)", PortId);
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        return;
    }

    pTopoDB = pHDBUser->pTopoDataBase[PortId-1];

    /* timeout, check if the timer is running, if not - ignore */
    if (!pHDBUser->PtcpTimerInUse[PortId-1])
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_WARN,"OHA_RequestPtcpTimer. Timer already stopped, UserID (0x%X)",pRQB->args.Timer.UserID);
        OHA_TimerRQBFree(pRQB->args.Timer.TimerID);
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPtcpTimer()");
        return;
    }

    /* timer is inactive */
    pHDBUser->PtcpTimerInUse[PortId-1] = LSA_FALSE;

    /* (viii) compare the Peer PTCP status if existing */
    if (OHA_IS_NOT_NULL(pTopoDB))
    {
        /* Peer PTCP-Status mismatch: Voraussetzung ist: Sync/IRDATA-Record (SubdomainID, IRDATAUUID) */
        RetVal = OHA_CompareTopoPeer_PTCP_Status(pHDBUser, PortId, &RemoteMatch);

        if (RetVal == OHA_OK)
        {
            LSA_UINT16  ExtCHannelErrorType = OHA_PEER_PTCP_MISMATCH;
            LSA_UINT16  ChannelProperties = (RemoteMatch) ? (LSA_UINT16)OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY : (LSA_UINT16)OHA_TOPO_ERROR_APPEARS;

            /* indicate only the changes */
            if (pHDBUser->TopoLastIndStati[PortId].LastSpecifierPeer_PTCP_status != ChannelProperties)
            {
                (LSA_VOID)OHA_PrepareTopoIndication(pHDBUser, PortId,
                                                    OHA_REMOTE_MISMATCH,
                                                    ExtCHannelErrorType,
                                                    ChannelProperties,
                                                    OHA_TOPO_EXT_CHANNEL_ADD_VALUE,
                                                    OHA_DIAG_PEER_PTCP);
            }

            if (OHA_IS_NOT_NULL(pHDBUser->pPortTopoInd[PortId]))
            {
                (LSA_VOID)OHA_TopoIndicateEvent(pHDBUser->pPortTopoInd[PortId], pHDBUser);
            }
        }
    }

    OHA_TimerRQBFree(pRQB->args.Timer.TimerID);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestPtcpTimer()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CancelPtcpTimer                         +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel handles a delay for PTCP diagnosis   +*/
/*+               and the timer is started -> stop it.                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelPtcpTimer(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish
)
{
    LSA_UINT16      i;
    LSA_UINT16      RetVal = OHA_OK;
    LSA_BOOL        Finish = LSA_TRUE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CancelPtcpTimer(pHDBUser: 0x%X)",
                          pHDBUser);

    for (i=0; i<OHA_CFG_MAX_PORTS; i++)
    {
        if ( pHDBUser->PtcpTimerID[i] != OHA_TIMER_ID_INVALID )
        {
            if (!(pHDBUser->PtcpTimerID[i] < OHA_CFG_MAX_TIMER))
            {
                OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,"OHA_CancelPtcpTimer(): Timer-ID (0x%X) for Port (0x%X) wrong, see OHA_CFG_MAX_TIMER ?", pHDBUser->PtcpTimerID[i], i+1);
                OHA_FATAL(OHA_FATAL_ERR_TIMER_RESOURCE);
            }

            if (pHDBUser->PtcpTimerInUse[i])
            {
                pHDBUser->PtcpTimerInUse[i] = LSA_FALSE;
                OHA_TimerStop(pHDBUser->PtcpTimerID[i]);
            }

            /* check if timer RQB is free. if not we have to wait if we are    */
            /* the last one using the timer                                    */
            /* (the timer RQB maybe pending because a timer event occured just */
            /* before we stopped the timer)                                    */
            if (!OHA_TimerRQBCheckFree(pHDBUser->PtcpTimerID[i]))
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Have to wait for PtcpTimer before closing channel.");

                /* Request is underway now. check in timer-hanling again and  */
                /* finish the request.                                        */
                Finish = LSA_FALSE; /* a timer request is active */
            }
        }
    }

    *pFinish = Finish;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CancelPtcpTimer(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CancelCyclicTimer                       +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel handles a delay for PTCP diagnosis   +*/
/*+               and the timer is started -> stop it.                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelCyclicTimer(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish
)
{
    LSA_BOOL    Finish   = LSA_TRUE;
    LSA_BOOL    WaitTimer = LSA_FALSE;
    LSA_UINT16  RetVal = OHA_OK;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CancelCyclicTimer(pHDBUser: 0x%X)",
                          pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pFinish);

    /* if we are not in state OPEN, a previos attempt to close */
    /* this handle failed and we must not stop the timer again*/
    if ( pHDBUser->State == OHA_HANDLE_STATE_OPEN )
    {
        /* if this is the last user, we stop the timer */
        if ( OHA_GET_GLOB_PTR()->CyclicTimerUserCnt == 1 )
        {
            OHA_TimerStop(OHA_GET_GLOB_PTR()->CyclicTimerID);
            WaitTimer = LSA_TRUE;
        }

        OHA_GET_GLOB_PTR()->CyclicTimerUserCnt--;
    }

    /* check if timer RQB is free. if not we have to wait if we are    */
    /* the last one using the timer                                    */
    /* (the timer RQB maybe pending because a timer event occured just */
    /* before we stopped the timer)                                    */
    if ( ! WaitTimer ||
         ( OHA_TimerRQBCheckFree(OHA_GET_GLOB_PTR()->CyclicTimerID)))
    {
        /* dont need to wait for timer. continue... */
        /* OHA_UserChannelStateMachine(pHDB,OHA_OK); */
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Have to wait for Timer before closing channel.");

        /* Request is underway now. check in timer-hanling again and  */
        /* finish the request.                                        */
        Finish = LSA_FALSE; /* a timer request is active */
        pHDBUser->State  = OHA_HANDLE_STATE_CLOSE_WAIT_TIMER;
    }

    *pFinish = Finish;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CancelCyclicTimer(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*****************************************************************************/
/*  end of file OHA_TIM.C                                                   */
/*****************************************************************************/


