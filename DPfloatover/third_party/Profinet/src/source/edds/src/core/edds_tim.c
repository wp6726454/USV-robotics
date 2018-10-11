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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_tim.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Timer functions                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  10.08.04    JS    use of EDDS_FREE_CHECK macro                           */
/*  28.01.05    JS    LTRC support                                           */
/*  09.02.05    JS    added optional reentrance check                        */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  5
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_TIM */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

#ifdef EDDS_CFG_LOCAL_MEM_STATIC
EDDS_STATIC EDDS_TIMER_INFO_TYPE  g_EDDSTimer[EDDS_CFG_MAX_TIMER];  //lint !e835 JB 11/11/2014 on purpose (#def 0)
#endif


/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*===========================================================================*/
/*=====  Timer -functions                                                ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TimerInfoIni                           +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the EDDS global Timer-Management              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_TimerInfoIni(LSA_VOID)
{

    EDDS_LOCAL_TIMER_INFO_PTR_TYPE  pTimer;
    LSA_RESULT                      Status;

    g_EDDSInfo.pTIMInfo = LSA_NULL;

    #ifdef EDDS_CFG_LOCAL_MEM_STATIC
    pTimer = g_EDDSTimer;
    #else
    EDDS_ALLOC_LOCAL_MEM((EDDS_LOCAL_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *)&pTimer, (EDDS_CFG_MAX_TIMER) * sizeof(EDDS_TIMER_INFO_TYPE));
    #endif

    if ( LSA_HOST_PTR_ARE_EQUAL(pTimer, LSA_NULL) )  //lint !e774 JB 11/11/2014 intended (but g_EDDSTimer could be set different)
        Status = EDD_STS_ERR_RESOURCE;
    else
    {

        g_EDDSInfo.pTIMInfo = pTimer;
        EDDS_MEMSET_LOCAL((EDDS_LOCAL_MEM_PTR_TYPE)pTimer,0,(EDDS_CFG_MAX_TIMER) * sizeof(EDDS_TIMER_INFO_TYPE));
        Status = EDD_STS_OK;
    }


    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TimerInfoFree                          +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees Timermanagement                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_TimerInfoFree(LSA_VOID)
{

    LSA_RESULT              Status;

    #ifndef EDDS_CFG_LOCAL_MEM_STATIC
    LSA_UINT16              FreeStat;
    #endif


    Status = EDD_STS_OK;

    #ifndef EDDS_CFG_LOCAL_MEM_STATIC

    if ( ! LSA_HOST_PTR_ARE_EQUAL(g_EDDSInfo.pTIMInfo, LSA_NULL) )
    {

        EDDS_FREE_LOCAL_MEM(&FreeStat, (EDDS_LOCAL_MEM_PTR_TYPE) g_EDDSInfo.pTIMInfo);
        EDDS_FREE_CHECK(FreeStat);

    }
    #endif


    g_EDDSInfo.pTIMInfo = LSA_NULL;

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TimerSet                               +*/
/*+  Input/Output          :    LSA_TIMER_ID_TYPE               TimerID     +*/
/*+                             EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB     +*/
/*+                             LSA_FCT_PTR(EDDS_LOCAL_FCT_ATTR, Cbf) (..)  +*/
/*+                             EDDS_LOCAL_MEM_U16_PTR_TYPE    EDDSTimerID  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID put into timer-management                    +*/
/*+  pIntRQB      Pointer to intenal RQB put into timer-management          +*/
/*+  Cbf          Optional Callbackfunction called when timer elapses.      +*/
/*+               Gets pointer to IntRQB as parameter                       +*/
/*+  EDDSTimerID EDDS internal Timer ID (= index )                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Puts new Timer into Timer-management                      +*/
/*+               if a Cbf is present, it is called if the timer elapses.   +*/
/*+               if no Cbf is present, when the timer elapses the output   +*/
/*+               Macro EDDS_DO_TIMEOUT_REQUEST is called with the rqb      +*/
/*+               spezified (only if rqb is not underway).                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,EDDSTimerID)
//JB 11/11/2014 EDDSTimer ID is getting set within this function
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TimerSet(
    LSA_TIMER_ID_TYPE      TimerID,
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB,
    LSA_VOID                        LSA_FCT_PTR(EDDS_LOCAL_FCT_ATTR, Cbf)
    (EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB),
    volatile EDDS_LOCAL_MEM_U16_PTR_TYPE EDDSTimerID)

{
    LSA_RESULT                      Status;
    EDDS_LOCAL_TIMER_INFO_PTR_TYPE  pTimer;
    LSA_UINT16                      i;
    LSA_BOOL                        Found;


    EDDS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TimerSet(pIntRQB: 0x%X, Cbf: 0x%X, EDDSTimerID: 0x%X)",
                           pIntRQB,
                           Cbf,
                           EDDSTimerID);

    Status = EDD_STS_OK;

    pTimer = g_EDDSInfo.pTIMInfo;

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pTimer, LSA_NULL) )
    {
        i       = 0;
        Found   = LSA_FALSE;

        while (( i <EDDS_CFG_MAX_TIMER ) && ( ! Found ))
        {

            if (! pTimer[i].InUse )
            {
                Found = LSA_TRUE;
                pTimer[i].TimerID   = TimerID;
                pTimer[i].pIntRQB   = pIntRQB;
                pTimer[i].Cbf       = Cbf;

                /* set InUse last to be sure other entrys are */
                /* valid.                                     */

                pTimer[i].InUse     = LSA_TRUE;
                *EDDSTimerID        = i;
            };

            i++;
        }

        if ( ! Found ) Status = EDD_STS_ERR_RESOURCE;

    }
    else Status = EDD_STS_ERR_RESOURCE;

    if ( Status != EDD_STS_OK )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_TIMER_RESOURCE,
                        EDDS_MODULE_ID,
                        __LINE__);
    }


    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TimerSet()");

}
//lint +esym(613,EDDSTimerID)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TimerFree                              +*/
/*+  Input/Output          :    LSA_UINT16                   EDDSTimerID    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  EDDSTimerID EDDS-Timer ID to free in management                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees timer in timermanagement and releases LSA-Timer     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_TimerFree(LSA_UINT16 EDDSTimerID)

{
    EDDS_LOCAL_TIMER_INFO_PTR_TYPE  pTimer;
    LSA_UINT16                      RetVal;
    LSA_TIMER_ID_TYPE               TimerID;

    EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TimerFree(EDDSTimerID: 0x%X)",
                           EDDSTimerID);

    pTimer = &g_EDDSInfo.pTIMInfo[EDDSTimerID]; /* EDDSTimerID is index */

    if ( pTimer->InUse )
    {
        TimerID = pTimer->TimerID;

        EDDS_STOP_TIMER(&RetVal,TimerID); /* just for sure. dont matter if already*/
        /* stopped                              */
        EDDS_FREE_TIMER(&RetVal,TimerID);

        pTimer->InUse = LSA_FALSE;
    }

    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TimerFree()");

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_timeout                                +*/
/*+  Input/Output          :    LSA_TIMER_ID_TYPE timer_id                  +*/
/*+                             LSA_USER_ID_TYPE  user_id                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  timer_id     : ID of Timer                                             +*/
/*+  user_id      : Used for EDDSTimerID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called from systemadaption on timerevent. May be called   +*/
/*+               within ISR-Handler.                                       +*/
/*+                                                                         +*/
/*+               LSA-Systemfunction                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_SYSTEM_IN_FCT_ATTR edds_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id)
{

    EDDS_LOCAL_TIMER_INFO_PTR_TYPE  pTimer;
    LSA_UINT32                      EDDSTimerID;

    EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_timeout(timer_id: 0x%X, user_id: 0x%X)",
                           timer_id,
                           user_id.uvar32);

    EDDS_ENTER_CHECK_TIMER();

    EDDSTimerID = (LSA_UINT16) user_id.uvar32;

    if ( EDDSTimerID < EDDS_CFG_MAX_TIMER )
    {
        pTimer = &g_EDDSInfo.pTIMInfo[EDDSTimerID];

        if (( pTimer->InUse ) &&
            ( pTimer->TimerID == timer_id ))
        {
            /* if a callbackfunction is present, call it */

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pTimer->Cbf, LSA_NULL) )
            {
                pTimer->Cbf(pTimer->pIntRQB);
            }
            else /* do standard handling..*/
            {
                if ( ! pTimer->pIntRQB->InUse )
                {
                    /* Note that the RQB is already filled up in startup! */
                    pTimer->pIntRQB->InUse = LSA_TRUE;
                    EDDS_DO_TIMEOUT_REQUEST(pTimer->pIntRQB->pRQB);
                }
            }
        }
    }

    EDDS_EXIT_CHECK_TIMER();

    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_timeout()");

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_ll_timeout                             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called when a LL timeout occurred.       +*/
/*+               This function calls the deposited EDDS_LL_TIMEOUT function+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_LLTimeoutCallback(EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_LLTimeout(pDDB: 0x%X)",
                           pDDB);

    pGlob =  pDDB->pGlob;

    pGlob->LLTimer.InProgress = LSA_FALSE;
    pGlob->RQBInt_LL_Tout.InUse = LSA_FALSE;

    /* --------------------------------------------------------------------------*/
    /* Call LL timeout function.                                                 */
    /* --------------------------------------------------------------------------*/

    EDDS_SYSTEM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_TIMEOUT(pDDB: 0x%X)",pDDB);

    if(LSA_FALSE == EDDS_LL_AVAILABLE(pDDB,timeout))  //lint !e731 JB 11/11/2014 on purpose
    {
    	EDDS_SYSTEM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,"*** EDDS_LL_TIMEOUT(pDDB: 0x%X) LLIF timeout function not available!",pDDB);

        EDDS_FatalError(EDDS_FATAL_ERR_TIMER_RESOURCE,
                        EDDS_MODULE_ID,
                        __LINE__);
    }else
    {
    	if(EDD_STS_OK != EDDS_LL_TIMEOUT(pDDB))      /* call standard LL timeout function */
    	{
            EDDS_FatalError(EDDS_FATAL_ERR_LL,
                            EDDS_MODULE_ID,
                            __LINE__);
    	}
    }

    EDDS_SYSTEM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_TIMEOUT()");

}
//lint +esym(613,pDDB)

//lint -esym(613,pDDB)
//JB 11/11/2014 checked where called (LLIF)
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR edds_ll_start_timer(
		EDDS_HANDLE const hDDB,
		LSA_UINT16 StandbyTime
		)

{
	LSA_RESULT RetVal;
	LSA_USER_ID_TYPE dummyUserID;
	EDDS_LOCAL_DDB_PTR_TYPE const pDDB =  (EDDS_LOCAL_DDB_PTR_TYPE const) hDDB;

	if(LSA_FALSE != pDDB->pGlob->LLTimer.InUse)
	{
		dummyUserID.uvar32 = pDDB->pGlob->LLTimer.EDDSTimerID;


		EDDS_START_TIMER(&RetVal,
				pDDB->pGlob->LLTimer.TimerID,
				dummyUserID,
				(StandbyTime+9)/10);		/* Standby Time in 10ms ticks */

		if(LSA_RET_OK == RetVal)
		{
			pDDB->pGlob->LLTimer.InProgress  = LSA_TRUE;
		}
        else
        {
            EDDS_SYSTEM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"*** EDDS_LL_TIMEOUT(pDDB: 0x%X) LLIF timer cannot be started (RetVal==0x%x)).",pDDB,RetVal);
        }
	}else
	{
		EDDS_SYSTEM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"*** EDDS_LL_TIMEOUT(pDDB: 0x%X) LLIF timeout function not available!",pDDB);
	}
}
//lint +esym(613,pDDB)



/*****************************************************************************/
/*  end of file EDDS_SYS.C                                                   */
/*****************************************************************************/

