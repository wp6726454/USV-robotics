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
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: General.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for general functions
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "General.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnCheckEventList)
//	#pragma alloc_text (PAGE, fnDoThreadSys)
//	#pragma alloc_text (PAGE, fnTimeLoop)
//	#pragma alloc_text (PAGE, fnSetTraceFctError)
#endif

//########################################################################
//  Variables
//########################################################################

static UINT32			g_lValueTmp	= 0;
static volatile UINT32*	g_pValueTmp	= &g_lValueTmp;

//************************************************************************
//  D e s c r i p t i o n :
//
//  check EventList
//************************************************************************

void fnCheckEventList(	DEVICE_EXTENSION*	pDevExtIn,
						uPNDEV_APP*			pAppExcludedIn,
						BOOLEAN				bThreadSysIn)
{
KIRQL lIrqlCur = PASSIVE_LEVEL;

	if	(!fnCheckIrql(	DISPATCH_LEVEL,
						&lIrqlCur))
		// invalid IRQL
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: ########## Invalid Irql (=%u) [%s()]!",
							pDevExtIn->sPciLocShortAscii,
							lIrqlCur,
							__FUNCTION__);
					
		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else
	{
		if	(	(pDevExtIn->uEvent.lFatalFw		!= 0)
			||	(pDevExtIn->uEvent.eFatalAsync	!= ePNDEV_FATAL_ASYNC__INVALID)
			||	(pDevExtIn->uEvent.eWarning		!= ePNDEV_WARNING__INVALID)
			||	(pDevExtIn->uEvent.bReqClose)
			||	(pDevExtIn->uEvent.eDebug		!= ePNDEV_DEBUG__INVALID))
			// event exist
		{
			// Note:
			//	- don't mask board interrupt here because it is not unmasked again (only at fnEvtIntEnable())

			if	(pDevExtIn->lCtrApp == 0)
				// no App exist
			{
				// don't clear events
			}
			else
			{
				// inform all AppObjects
				//	- AppList not used at ISR
				{
				uLIST_HEADER*	pList		= NULL;
				uLIST_HEADER*	pBlockTmp	= NULL;
				uPNDEV_EVENT	uEventCopy;

					// copy EventList
					//	- otherwise different Apps may get different events because of possible new occurring events
					*((uPNDEV_EVENT*) &uEventCopy)	= *((uPNDEV_EVENT*) &pDevExtIn->uEvent);

					// clear events
					pDevExtIn->uEvent.lFatalFw		= 0;
					pDevExtIn->uEvent.eFatalAsync	= ePNDEV_FATAL_ASYNC__INVALID;
					pDevExtIn->uEvent.eWarning		= ePNDEV_WARNING__INVALID;
					pDevExtIn->uEvent.bReqClose		= FALSE;
					pDevExtIn->uEvent.eDebug		= ePNDEV_DEBUG__INVALID;

					pList = &pDevExtIn->uListApp;

					//------------------------------------------------------------------------
					// enter CriticalSection
					fnEnterCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
					{
						// get first block of AppList
						pBlockTmp = (uLIST_HEADER*) pList->pNext;

						while	(pBlockTmp != pList)
								// end of list not reached
						{
						uPNDEV_APP* pApp = (uPNDEV_APP*) pBlockTmp;

							if	(pApp != pAppExcludedIn)
								// not excluded App
							{
								// copy events to AppObject
								//	- necessary because each App will read it by service GetEventList
								//	- otherwise there would be no real trigger for clearing events
								pApp->uEvent.lFatalFw		= uEventCopy.lFatalFw;
								pApp->uEvent.eFatalAsync	= uEventCopy.eFatalAsync;
								pApp->uEvent.eWarning 		= uEventCopy.eWarning;
								pApp->uEvent.bReqClose		= uEventCopy.bReqClose;
								pApp->uEvent.eDebug 		= uEventCopy.eDebug;

								// set EventUinfo
								fnSetEventThreadDll(pDevExtIn,
													pApp,
													ePNDEV_EVENT_THREAD_DLL_UINFO,
													bThreadSysIn);
							}

							// get next block of AppList
							pBlockTmp = (uLIST_HEADER*) pBlockTmp->pNext;
						}
					}
					//------------------------------------------------------------------------
					// leave CriticalSection
					fnLeaveCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
				}
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  do actions of SysThread
//************************************************************************

void fnDoThreadSys(	DEVICE_EXTENSION*	pDevExtIn,
					UINT32				lTimeCycle_10msecIn)
{
	// Note:
	//	- if there is an event then IOCTL_GET_EVENT_LIST must be called by DLL which needs pApp
	//	- pApp is returned by OpenDevice
	//		-> EventUinfo must not be set before DLL of first OpenDevice is ready
        
	if	(pDevExtIn->bDllReadyFirstOpenDevice)
		// DLL ready of first OpenDevice
	{
		pDevExtIn->lCtrThreadSys++;

		if	(pDevExtIn->lCtrThreadSys == 1)
			// ThreadStart
		{
			// simulate FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_SYS;
		}

		if	(pDevExtIn->bResetRunning)
			// reset running
		{
			// don't access hardware during reset (wrong values would be read)
		}
		else
		{
		BOOLEAN	bWarning = FALSE;
		char	sTraceDetail[PNDEV_SIZE_STRING_TRACE]	= {0};

			if	(!fnBoardIrtePnip_CheckAccessTimeout(	pDevExtIn,
														&bWarning))
				// error at checking PCIe-AccessTimeout
			{
				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## PCIe access timeout!",
									pDevExtIn->sPciLocShortAscii);
					
				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);


				if	(bWarning)
					// warning
				{
					// set warning
					pDevExtIn->uEvent.eWarning = ePNDEV_WARNING__PCIE_ACCESS_TIMEOUT;
				}
				else
				{
					// set FatalAsync
					pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__PCIE_ACCESS_TIMEOUT;
				}
			}

			if	(!fnBoardIrtePnip_CheckIcuMode(pDevExtIn))
				// error at checking IcuMode
			{
				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__INVALID_ICU_MODE;
			}

			if	(pDevExtIn->bUserFwRunning)
				// UserFw running
			{
			BOOLEAN			bPagedAccess	= FALSE;
			volatile UINT8*	pAsicSdramSeg0	= NULL;

				//------------------------------------------------------------------------
				// start using SdramPage 0
				fnBoard_StartSdramPage(	pDevExtIn,
										0,
										&bPagedAccess,
										&pAsicSdramSeg0);
				{
				uPNCORE_CONFIG_PAR*	pConfigPar	= NULL;
				UINT32				lFatalFw	= 0;

					pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);

					// set SignOfLife of driver
					fnPnCore_SetSignOfLife(	pAsicSdramSeg0,
											LSA_FALSE);			// called by driver

					// check SignOfLife of UserFw periodically
					{
						if	(!fnPnCore_CheckSignOfLife(	pAsicSdramSeg0,
														lTimeCycle_10msecIn,
														LSA_FALSE))			// called by driver
							// SignOfLife of partner not set
						{
							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[%s]: ########## Fatal, no SignOfLife!",
												pDevExtIn->sPciLocShortAscii);
					
							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
										ePNDEV_TRACE_CONTEXT_BASIC,
										TRUE,						// logging
										sTraceDetail);

							// set FatalAsync
							pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__SIGN_OF_LIFE;
						}
					}

					#ifdef PNDEV_DEVELOPMENT
						// debug SignOfLife of driver
						{
							if	(pConfigPar->uSignOfLifeDriver.lCtrCycleActivePhase == 0)
								// StartupPhase
							{
								// set TraceDetail
								RtlStringCchPrintfA(sTraceDetail,
													_countof(sTraceDetail),
													"PnDev[%s]: -- SignOfLife of driver: StartupPhase",
													pDevExtIn->sPciLocShortAscii);
							}
							else	// ActivePhase
							{
								// set TraceDetail
								RtlStringCchPrintfA(sTraceDetail,
													_countof(sTraceDetail),
													"PnDev[%s]: -- SignOfLife of driver: ActivePhase (now checking by UserFw)",
													pDevExtIn->sPciLocShortAscii);
							}

							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
										ePNDEV_TRACE_CONTEXT_BASIC,
										FALSE,						// logging
										sTraceDetail);
						}

						// debug SignOfLife of UserFw
						{
							if	(pConfigPar->uSignOfLifeFw.lCtrCycleActivePhase == 0)
								// StartupPhase
							{
								// set TraceDetail
								RtlStringCchPrintfA(sTraceDetail,
													_countof(sTraceDetail),
													"PnDev[%s]: -- SignOfLife of UserFw: StartupPhase",
													pDevExtIn->sPciLocShortAscii);
							}
							else	// ActivePhase
							{
								// set TraceDetail
								RtlStringCchPrintfA(sTraceDetail,
													_countof(sTraceDetail),
													"PnDev[%s]: -- SignOfLife of UserFw: ActivePhase (now checking by driver)",
													pDevExtIn->sPciLocShortAscii);
							}

							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
										ePNDEV_TRACE_CONTEXT_BASIC,
										FALSE,						// logging
										sTraceDetail);
						}
					#endif

					// check FwFatal periodically
					{
						lFatalFw = pConfigPar->uFw.lFatal;

						if	(lFatalFw != 0)
							// FwFatal exist
						{
							// set event
							pDevExtIn->uEvent.lFatalFw = lFatalFw;

							// reset
							pConfigPar->uFw.lFatal = 0;
						}
					}
				}
				//------------------------------------------------------------------------
				// stop using SdramPage
				fnBoard_StopSdramPage(pDevExtIn);
			}
		}

		// check EventList periodically
		fnCheckEventList(	pDevExtIn,
							NULL,		// no App excluded
							TRUE);		// called by SysThread
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	TimeLoop
//
//	Note for Windows:
//	- KeDelayExecutionThread' only allowed if callers are running at IRQL = PASSIVE_LEVEL)
//************************************************************************

BOOLEAN fnTimeLoop(	DEVICE_EXTENSION*	pDevExtIn,
					const UINT32		lTimeUsecIn,
					const BOOLEAN		bShowDetailsIn)
{
BOOLEAN	bResult				= FALSE;
UINT32	lTimeDesiredUsec	= 0;
UINT32	lTimeCurUsec		= 0;		// placed here for DebugPurpose
UINT32	lCtrLoop			= 0;		// placed here for DebugPurpose

	// update desired WaitTime
	lTimeDesiredUsec = lTimeUsecIn * pDevExtIn->lTimeScaleFactor;

	if	(lTimeDesiredUsec != 0)
		// valid WaitTime
	{
	uPNDEV_CLOCK	uClockStart;
	UINT32			i = 0;

		// success
		bResult = TRUE;

		// get clock
		fnGetClock(&uClockStart);

		do
		{
			// loop by instructions (it's not optimal to call fnGetClock() in a loop)
			for	(i = 0; i < 500; i++)
			{
				*g_pValueTmp = (((*g_pValueTmp / 3) * 7) / 17);
			}

			// compute Runtime in usec
			lTimeCurUsec = fnComputeRunTimeUsec(&uClockStart);

			if	(	(bShowDetailsIn)
				&&	((lCtrLoop % 10000) == 0))
				// details should be shown
			{
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[x,x,x]: Current WaitTime=%u usec",
									lTimeCurUsec);
					
				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}

			lCtrLoop++;
		}
		while	(lTimeCurUsec < lTimeDesiredUsec);
				// current WaitTime < desired WaitTime (usec)
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set trace by returned ErrorString
//************************************************************************

void fnSetTraceFctError(DEVICE_EXTENSION*	pDevExtIn,
						_TCHAR*				sErrorIn)
{
char	sErrorAscii [PNDEV_SIZE_STRING_BUF]		= {0};
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE]	= {0};

	// convert _TCHAR to ASCII string
	fnConvertStringTcharToChar(	sErrorIn,
								sErrorAscii);

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[%s]: #### %s",
						(pDevExtIn == NULL) ? "x,x,x" : pDevExtIn->sPciLocShortAscii,
						sErrorAscii);
					
	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
				ePNDEV_TRACE_CONTEXT_BASIC,
				TRUE,						// logging
				sTraceDetail);
}
