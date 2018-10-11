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
/*  F i l e               &F: IsrDpc.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for handling of ISR and DPC
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
	#include "IsrDpc.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnIsr)
//	#pragma alloc_text (PAGE, fnDpc)
//	#pragma alloc_text (PAGE, fnEvtIsrSharedInt)
//	#pragma alloc_text (PAGE, fnUpdateStatisticIsr)
//	#pragma alloc_text (PAGE, fnUpdateStatisticDpc)
//	#pragma alloc_text (PAGE, fnStartRuntimeIsr)
//	#pragma alloc_text (PAGE, fnStopRuntimeIsr)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  ISR
//************************************************************************

BOOLEAN fnIsr(	DEVICE_EXTENSION*	pDevExtIn,
				HANDLE				hOsParIn,
				UINT32				lMsgIdMsiIn)
{
BOOLEAN				bResult = FALSE;
uPNDEV_INT_DETAIL	uIntDetail;

	UNREFERENCED_PARAMETER(lMsgIdMsiIn);

	// Note:
	//	- don't access hardware during reset (possible hangup if using Soc1)
	//	- if a reset is running then values of AsicRegisters may be undefined
	//	- if an existing DefaultFw is not stopped then Asic may be used by this firmware -> no access allowed

	if	(	(pDevExtIn->bResetRunning)
		||	(!fnBoard_CheckInt(	pDevExtIn,
								&uIntDetail)))
		//		reset running
		// OR	no interrupt exist
	{
		// return FALSE to indicate that this device did not cause the interrupt

		// SharedInt occurred
		fnEvtIsrSharedInt(pDevExtIn);
	}
	else
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		if	(pDevExtIn->bIsrRunning)
			// ISR interrupted itself
		{
			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_ISR_LOCK;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## ISR lock error!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_ISR,
						TRUE,						// logging
						sTraceDetail);
		}
		else
		{
			// interrupt caused by this device
			bResult = TRUE;

			// set state IsrRunning
			pDevExtIn->bIsrRunning = TRUE;
			{
			BOOLEAN bReqDpc = FALSE;

				#ifdef PNDEV_GET_RUNTIME_ISR
					// start RuntimeIsr
					fnStartRuntimeIsr(pDevExtIn);
				#endif

				// update IsrStatistic
				fnUpdateStatisticIsr(pDevExtIn);

				if	(pDevExtIn->eIntAction <= PNDEV_INT_ACTION_STD_MAX)
					// IntAction=Standard
				{
					// standard ISR
					fnBoard_IsrStandard(pDevExtIn,
										&uIntDetail,
										&bReqDpc);

                    //z00301au
                    // If the NetPROFI Mode is active, a few Interrupts are handled by the netPROFI kernel, a few interrupts are handled by the UserSpace implementation. Overwrite value in netPROFI Mode
                    if(pDevExtIn->bNetProfi)
                    {
                        bReqDpc = fnNetProfiIntHandler (pDevExtIn);
                    }

				}
				else	// IntAction=Test
				{
					// ISR for IntTest
					fnBoard_IsrIntTest(	pDevExtIn,
										&uIntDetail,
										&bReqDpc);
				}

				if	(bReqDpc)
					// DPC should be requested
				{
					if	(pDevExtIn->bOsDpcSupport)
						// OS supports DPC
					{
						if	(pDevExtIn->lCtrDpcQueued == 0)
							// DPC not queued up to now
						{
							if	(!fnQueueDpc(	pDevExtIn,
												hOsParIn))
								// error at queuing DPC
							{
								// set FatalAsync
								pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__DPC_NOT_QUEUED;

								// set TraceDetail
								RtlStringCchPrintfA(sTraceDetail,
													_countof(sTraceDetail),
													"PnDev[%s]: ########## DPC not queued!",
													pDevExtIn->sPciLocShortAscii);

								// set trace
								fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
											ePNDEV_TRACE_CONTEXT_ISR,
											TRUE,						// logging
											sTraceDetail);
							}
							else
							{
								pDevExtIn->lCtrDpcQueued++;

								if	(pDevExtIn->uIntStatistic.lCtrIsr == 1)
									// first call of ISR
								{
									#ifdef PNDEV_TRACE_ISR_DPC
										// set TraceDetail
										RtlStringCchPrintfA(sTraceDetail,
															_countof(sTraceDetail),
															"PnDev[%s]: ++++ DPC queued",
															pDevExtIn->sPciLocShortAscii);

										// set trace
										fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
													ePNDEV_TRACE_CONTEXT_ISR,
													FALSE,						// logging
													sTraceDetail);
									#endif
								}
							}
						}
					}
					else
					{
						pDevExtIn->lCtrDpcQueued++;

						// do DPC
						fnDpc(pDevExtIn);
					}
				}

				#ifdef PNDEV_GET_RUNTIME_ISR
					// stop RuntimeIsr
					fnStopRuntimeIsr(pDevExtIn);
				#endif
			}
			// reset state IsrRunning
			pDevExtIn->bIsrRunning = FALSE;
		}

		// set AsicEoi
		//	- a running EOI-Timer is restarted if EOI is written again!
		fnBoard_IsrSetEoi(pDevExtIn);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  DPC
//************************************************************************

void fnDpc(DEVICE_EXTENSION* pDevExtIn)
{
	//------------------------------------------------------------------------
	// acquire DpcSpinLock
	//	- protect against re-entrance at MultiCore system!
	fnAcquireSpinLock(	pDevExtIn,
						ePNDEV_SPIN_LOCK_DPC);
	{
		if	(pDevExtIn->bResetRunning)
			// reset running
		{
			// don't access hardware during reset (possible hangup if using Soc1)
			// ignore stored DPCs
		}
		else
		{
		BOOLEAN bReqUisr = FALSE;

			pDevExtIn->lCtrDpcQueued--;

			// update DpcStatistic
			fnUpdateStatisticDpc(pDevExtIn);

			if	(pDevExtIn->eIntAction <= PNDEV_INT_ACTION_STD_MAX)
				// IntAction=Standard
			{
				// standard DPC
				fnBoard_DpcStandard(pDevExtIn,
									&bReqUisr);
			}
			else	// IntAction=Test
			{
				// DPC for IntTest
				fnBoard_DpcIntTest(	pDevExtIn,
									&bReqUisr);
			}

			if	(bReqUisr)
				// UISR should be requested
			{
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				if	(pDevExtIn->pMasterApp == NULL)
					// no MasterApp exist
				{
					// set FatalAsync
					pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__UISR_NOT_QUEUED;

					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: ########## No MasterApp, UISR not queued!",
										pDevExtIn->sPciLocShortAscii);

					// set trace
					fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
								ePNDEV_TRACE_CONTEXT_DPC,
								TRUE,						// logging
								sTraceDetail);
				}
				else
				{
					if	(pDevExtIn->pMasterApp->hEventUisr != NULL)
						// valid event
					{
						// set EventUisr of AppObject (MasterApp)
						fnSetEventThreadDll(pDevExtIn,
											pDevExtIn->pMasterApp,
											ePNDEV_EVENT_THREAD_DLL_UISR,
											FALSE);
					}

					if	(pDevExtIn->uIntStatistic.lCtrDpc == 1)
						// first call of DPC
					{
						#ifdef PNDEV_TRACE_ISR_DPC
							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[%s]: ++++ UISR queued",
												pDevExtIn->sPciLocShortAscii);

							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
										ePNDEV_TRACE_CONTEXT_DPC,
										FALSE,						// logging
										sTraceDetail);
						#endif
					}
				}
			}

			//------------------------------------------------------------------------
			// acquire SpinLockIrq of this device
			//fnAcquireSpinLockIrq(pDevExtIn);
			//	- Stringfunctions with Unicode format codes can only be used at PASSIVE_LEVEL
			{
				//...
			}
			//------------------------------------------------------------------------
			// release SpinLockIrq
			//fnReleaseSpinLockIrq(pDevExtIn);
		}
	}
	//------------------------------------------------------------------------
	// release DpcSpinLock
	fnReleaseSpinLock(	pDevExtIn,
						ePNDEV_SPIN_LOCK_DPC);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  SharedInt occurred
//************************************************************************

void fnEvtIsrSharedInt(DEVICE_EXTENSION* pDevExtIn)
{
	pDevExtIn->uIntStatistic.lCtrSharedInt++;

	if	((pDevExtIn->uIntStatistic.lCtrSharedInt % PNDEV_DEBUG_CTR_THRESHOLD) == 0)
	{
		#ifdef PNDEV_DEVELOPMENT
		{
		char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: SharedInt=%u",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->uIntStatistic.lCtrSharedInt);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						FALSE,						// logging
						sTraceDetail);
		}
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  update IsrStatistic
//************************************************************************

void fnUpdateStatisticIsr(DEVICE_EXTENSION*	pDevExtIn)
{
#ifdef PNDEV_TRACE_ISR_DPC
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
#endif

	// increment CtrIntTest
	pDevExtIn->uIntStatistic.lCtrIsr++;

	if	(pDevExtIn->uIntStatistic.lCtrIsr == 1)
		// first call of ISR
	{
		pDevExtIn->uIntStatistic.bIsrEntered = TRUE;

		#ifdef PNDEV_TRACE_ISR_DPC
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ++++ ISR entered",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						TRUE,						// logging
						sTraceDetail);
		#endif
	}
	else if	((pDevExtIn->uIntStatistic.lCtrIsr % PNDEV_DEBUG_CTR_THRESHOLD) == 0)
	{
		#ifdef PNDEV_DEVELOPMENT
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ISRs=%u",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->uIntStatistic.lCtrIsr);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						FALSE,						// logging
						sTraceDetail);
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  update DpcStatistic
//************************************************************************

void fnUpdateStatisticDpc(DEVICE_EXTENSION*	pDevExtIn)
{
#ifdef PNDEV_TRACE_ISR_DPC
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
#endif

	// increment CtrIntTest
	pDevExtIn->uIntStatistic.lCtrDpc++;

	if	(pDevExtIn->uIntStatistic.lCtrDpc == 1)
		// first call of DPC
	{
		pDevExtIn->uIntStatistic.bDpcEntered = TRUE;

		#ifdef PNDEV_TRACE_ISR_DPC
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ++++ DPC entered",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_DPC,
						TRUE,						// logging
						sTraceDetail);
		#endif
	}
	else if	((pDevExtIn->uIntStatistic.lCtrDpc % PNDEV_DEBUG_CTR_THRESHOLD) == 0)
	{
		#ifdef PNDEV_DEVELOPMENT
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: DPCs=%u",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->uIntStatistic.lCtrDpc);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_DPC,
						FALSE,						// logging
						sTraceDetail);
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start RuntimeIsr
//************************************************************************

void fnStartRuntimeIsr(DEVICE_EXTENSION* pDevExtIn)
{
	pDevExtIn->lTimeIrqEdge_Nsec	= 0;
	pDevExtIn->lTimeIsrStart_Nsec	= 0;

	// get clock
	fnGetClock(&pDevExtIn->uClockStart);

	if	(pDevExtIn->pPnIp != NULL)
		// board with PNIP
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		switch	(pDevExtIn->eBoardDetail)
				// BoardDetail
		{
			case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
			{
				#ifdef FPGA1_ERTEC200P_ARM_NO_TOPLEVEL_ICU
				#else

					// TopLevel-ICU = ARM-ICU

					// get time of IRQ-edge and current time
					pDevExtIn->lTimeIrqEdge_Nsec	= READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_TIME_STAMP);
					pDevExtIn->lTimeIsrStart_Nsec	= READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU_TIME_VALUE);

					// clear timestamp of IRQ-edge
					WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_TIME_STAMP),
										0);
				#endif

				break;
			}
			case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
			{
				// TopLevel-ICU = PERIF-ICU

				// get time of IRQ-edge and current time
				pDevExtIn->lTimeIrqEdge_Nsec	= READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU3_TIME_STAMP);
				pDevExtIn->lTimeIsrStart_Nsec	= READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU_TIME_VALUE);

				// clear timestamp of IRQ-edge
				WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU3_TIME_STAMP),
									0);

				break;
			}
			default:
			{
				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__INVALID_BOARD_DETAIL;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Invalid BoardDetail!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);

				break;
			}
		}

		#ifdef PNDEV_TRACE_ISR_DPC
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: t_Irq=0x%x, t_Isr=0x%x",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->lTimeIrqEdge_Nsec,
								pDevExtIn->lTimeIsrStart_Nsec);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						FALSE,						// logging
						sTraceDetail);
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  stop RuntimeIsr
//************************************************************************

void fnStopRuntimeIsr(DEVICE_EXTENSION* pDevExtIn)
{
UINT32	lIsrRuntimeWin_Usec	= 0;
UINT32	IsrRuntimeAsic_Nsec	= 0;
UINT32	lIsrDelayAsic_Nsec	= 0;

	// compute runtime of ISR by PC timer
	{
		// compute Runtime in usec
		lIsrRuntimeWin_Usec = fnComputeRunTimeUsec(&pDevExtIn->uClockStart);

		// update statistic
		{
			if	(	(pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec == 0)
				&&	(pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec == 0))
				// first time after Startup/UserRead
			{
				// initialize Min/Max
				pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec	= lIsrRuntimeWin_Usec;
				pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec	= lIsrRuntimeWin_Usec;
			}
			else if	(lIsrRuntimeWin_Usec < pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec)
				// Cur < Min
			{
				// update Min
				pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec = lIsrRuntimeWin_Usec;
			}
			else if	(lIsrRuntimeWin_Usec > pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec)
				// Cur > Max
			{
				// update Max
				pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec = lIsrRuntimeWin_Usec;
			}
		}
	}

	if	(pDevExtIn->pPnIp != NULL)
		// board with PNIP
	{
	UINT32 lTimeIsrStop_Nsec = 0;

	#ifdef PNDEV_TRACE_ISR_DPC
		char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
	#endif

		// compute runtime of ISR by Asic timer
		{
			// get current time
			lTimeIsrStop_Nsec = READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU_TIME_VALUE);

			if	(lTimeIsrStop_Nsec >= pDevExtIn->lTimeIsrStart_Nsec)
			{
				IsrRuntimeAsic_Nsec = (lTimeIsrStop_Nsec - pDevExtIn->lTimeIsrStart_Nsec);
			}
			else
			{
				// Runtime = delta(BeforeWrap) + delta(AfterWrap)
				IsrRuntimeAsic_Nsec = (0xFFFFffff - pDevExtIn->lTimeIsrStart_Nsec + 1) + lTimeIsrStop_Nsec;
			}

			// update statistic
			{
				// update statistic
				{
					if		(IsrRuntimeAsic_Nsec < 10000)	{pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT10_Usec++;}
					else if	(IsrRuntimeAsic_Nsec < 20000)	{pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT20_Usec++;}
					else if	(IsrRuntimeAsic_Nsec < 30000)	{pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT30_Usec++;}
					else if	(IsrRuntimeAsic_Nsec < 40000)	{pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT40_Usec++;}
					else if	(IsrRuntimeAsic_Nsec < 50000)	{pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT50_Usec++;}
					else									{pDevExtIn->uIntStatistic.lCtrIsrRuntime_GE50_Usec++;}
				}
			}

			pDevExtIn->uIntStatistic.lCtrIsrByPnIp++;

			if	((pDevExtIn->uIntStatistic.lCtrIsrByPnIp % 500) == 0)
				// n interrupts
			{
				#ifdef PNDEV_TRACE_ISR_DPC
					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: (%06dx) ISR-Runtime[usec]: |min: %03d|max: %03d|<10: %03dx|<20: %03dx|<30: %03dx|<40: %03dx|<50: %03dx|>50: %03dx|",
										pDevExtIn->sPciLocShortAscii,
										pDevExtIn->uIntStatistic.lCtrIsrByPnIp,
										pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec,
										pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT10_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT20_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT30_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT40_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT50_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrRuntime_GE50_Usec);

					// set trace
					fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
								ePNDEV_TRACE_CONTEXT_ISR,
								FALSE,						// logging
								sTraceDetail);
				#endif

				pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec	= 0;
				pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec	= 0;

				pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT10_Usec	= 0;
				pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT20_Usec	= 0;
				pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT30_Usec	= 0;
				pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT40_Usec	= 0;
				pDevExtIn->uIntStatistic.lCtrIsrRuntime_LT50_Usec	= 0;
				pDevExtIn->uIntStatistic.lCtrIsrRuntime_GE50_Usec	= 0;
			}
		}

		if	(pDevExtIn->lTimeIrqEdge_Nsec != 0)
			// edge of special ICU/MUX-IRQ detected by Asic (otherwise Time is set to 0 by Asic)
		{
			// compute delay IRQ-ISR
			{
				if	(pDevExtIn->lTimeIsrStart_Nsec >= pDevExtIn->lTimeIrqEdge_Nsec)
				{
					lIsrDelayAsic_Nsec = (pDevExtIn->lTimeIsrStart_Nsec - pDevExtIn->lTimeIrqEdge_Nsec);
				}
				else
				{
					// Runtime = delta(BeforeWrap) + delta(AfterWrap)
					lIsrDelayAsic_Nsec = (0xFFFFffff - pDevExtIn->lTimeIrqEdge_Nsec + 1) + pDevExtIn->lTimeIsrStart_Nsec;
				}

				// update statistic
				{
					if		(lIsrDelayAsic_Nsec < 10000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT10_Usec++;}
					else if	(lIsrDelayAsic_Nsec < 50000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT50_Usec++;}
					else if	(lIsrDelayAsic_Nsec < 100000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT100_Usec++;}
					else if	(lIsrDelayAsic_Nsec < 200000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT200_Usec++;}
					else if	(lIsrDelayAsic_Nsec < 300000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT300_Usec++;}
					else if	(lIsrDelayAsic_Nsec < 400000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT400_Usec++;}
					else if	(lIsrDelayAsic_Nsec < 500000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT500_Usec++;}
					else if	(lIsrDelayAsic_Nsec < 1000000)	{pDevExtIn->uIntStatistic.lCtrIsrDelay_LT1000_Usec++;}
					else
					{
						pDevExtIn->uIntStatistic.lCtrIsrDelay_GE1000_Usec++;

						#ifdef PNDEV_TRACE_ISR_DPC
							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[%s]: lIsrDelayAsic_Nsec=%u, t_IRQ=0x%x, t_ISR=0x%x",
												pDevExtIn->sPciLocShortAscii,
												lIsrDelayAsic_Nsec,
												pDevExtIn->lTimeIrqEdge_Nsec,
												pDevExtIn->lTimeIsrStart_Nsec);

							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
										ePNDEV_TRACE_CONTEXT_ISR,
										FALSE,						// logging
										sTraceDetail);
						#endif
					}

					if	(	(pDevExtIn->uIntStatistic.lIsrDelayAsicMin_Nsec == 0)
						&&	(pDevExtIn->uIntStatistic.lIsrDelayAsicMax_Nsec == 0))
						// first time after Startup/UserRead
					{
						// initialize Min/Max
						pDevExtIn->uIntStatistic.lIsrDelayAsicMin_Nsec	= lIsrDelayAsic_Nsec;
						pDevExtIn->uIntStatistic.lIsrDelayAsicMax_Nsec	= lIsrDelayAsic_Nsec;
					}
					else if	(lIsrDelayAsic_Nsec < pDevExtIn->uIntStatistic.lIsrDelayAsicMin_Nsec)
						// Cur < Min
					{
						// update Min
						pDevExtIn->uIntStatistic.lIsrDelayAsicMin_Nsec = lIsrDelayAsic_Nsec;
					}
					else if	(lIsrDelayAsic_Nsec > pDevExtIn->uIntStatistic.lIsrDelayAsicMax_Nsec)
						// Cur > Max
					{
						// update Max
						pDevExtIn->uIntStatistic.lIsrDelayAsicMax_Nsec = lIsrDelayAsic_Nsec;
					}
				}
			}

			pDevExtIn->uIntStatistic.lCtrIsrByPnIpSpecialIrq++;

			if	(	(pDevExtIn->uIntStatistic.lCtrIsrByPnIpSpecialIrq			<= 5)
				||	((pDevExtIn->uIntStatistic.lCtrIsrByPnIpSpecialIrq % 500)	== 0))
				// n interrupts (max. 999 because of %03dx)
			{
				#ifdef PNDEV_TRACE_ISR_DPC
					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: (%06dx) ISR-Delay[usec]: |min: %03d|max: %03d|<10: %03dx|<50: %03dx|<100: %03dx|<200: %03dx|<300: %03dx|<400: %03dx|<500: %03dx|<1000: %03dx|>1000: %03dx|",
										pDevExtIn->sPciLocShortAscii,
										pDevExtIn->uIntStatistic.lCtrIsrByPnIpSpecialIrq,
										(pDevExtIn->uIntStatistic.lIsrDelayAsicMin_Nsec/1000),
										(pDevExtIn->uIntStatistic.lIsrDelayAsicMax_Nsec/1000),
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT10_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT50_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT100_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT200_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT300_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT400_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT500_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_LT1000_Usec,
										pDevExtIn->uIntStatistic.lCtrIsrDelay_GE1000_Usec);

					// set trace
					fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
								ePNDEV_TRACE_CONTEXT_ISR,
								FALSE,						// logging
								sTraceDetail);
				#endif

				if	(pDevExtIn->uIntStatistic.lCtrIsrByPnIpSpecialIrq > 5)
				{
					pDevExtIn->uIntStatistic.lIsrDelayAsicMin_Nsec		= 0;
					pDevExtIn->uIntStatistic.lIsrDelayAsicMax_Nsec		= 0;

					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT10_Usec		= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT50_Usec		= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT100_Usec	= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT200_Usec	= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT300_Usec	= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT400_Usec	= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT500_Usec	= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_LT1000_Usec	= 0;
					pDevExtIn->uIntStatistic.lCtrIsrDelay_GE1000_Usec	= 0;
				}
			}
		}
	}
/*
	#ifdef PNDEV_TRACE_ISR_DPC
		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: IsrDelay=%u, RuntimeNsec=%u, RuntimeUsec=%u",
							pDevExtIn->sPciLocShortAscii,
							lIsrDelayAsic_Nsec,
							IsrRuntimeAsic_Nsec,
							lIsrRuntimeWin_Usec);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_ISR,
					FALSE,						// logging
					sTraceDetail);
	#endif
*/
}
