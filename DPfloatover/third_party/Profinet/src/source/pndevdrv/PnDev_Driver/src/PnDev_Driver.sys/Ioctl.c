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
/*  F i l e               &F: Ioctl.c                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for handling of Ioctl requests
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
	#include "Ioctl.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnDoIoctl)
//	#pragma alloc_text (PAGE, fnIoctlOpenDevice)
//	#pragma alloc_text (PAGE, fnIoctlCloseDevice)
//	#pragma alloc_text (PAGE, fnIoctlResetDevice)
//	#pragma alloc_text (PAGE, fnIoctlGetEventList)
//	#pragma alloc_text (PAGE, fnIoctlSetEventList)
//	#pragma alloc_text (PAGE, fnIoctlChangeInt)
//	#pragma alloc_text (PAGE, fnIoctlGetDeviceState)
//	#pragma alloc_text (PAGE, fnIoctlProgramFlash)
//	#pragma alloc_text (PAGE, fnIoctlChangeXhifPage)
//	#pragma alloc_text (PAGE, fnIoctlCopyData)
//	#pragma alloc_text (PAGE, fnIoctlStartBootFw)
//	#pragma alloc_text (PAGE, fnIoctlStartUserFw)
//	#pragma alloc_text (PAGE, fnIoctlChangeAppLock)
//	#pragma alloc_text (PAGE, fnIoctlNotifyDllState)
//	#pragma alloc_text (PAGE, fnCheckIoctlPar)
//	#pragma alloc_text (PAGE, fnCloseDevice)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  do IoDeviceControl
//************************************************************************

ePNDEV_IOCTL fnDoIoctl(	DEVICE_EXTENSION*	pDevExtIn,						// Attention: pDevExtIn may be NULL if DriverService!
						HANDLE				hOsParIn,
						const UINT32		lIoctlIn,
						const UINT32		lSizeInputBufIn,
						const UINT32		lSizeOutputBufIn,
						void*				pInputBufIn,
						void*				pOutputBufIn)
{
ePNDEV_IOCTL	eResult = ePNDEV_IOCTL_INVALID;

	#ifdef PNDEV_DEVELOPMENT
	char			sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ++++ IOCTL 0x%x",	// pDevExtIn may be NULL
							lIoctlIn);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					FALSE,						// logging
					sTraceDetail);
	#endif

	// check IoctlPar
	eResult = fnCheckIoctlPar(	lIoctlIn,
								lSizeInputBufIn,
								lSizeOutputBufIn);

	if	(eResult == ePNDEV_IOCTL_OK)
		// success
	{
	BOOLEAN	bResultBuf	= FALSE;
	void*	pIn			= NULL;
	void*	pOut		= NULL;

		if	(	(pInputBufIn	!= NULL)
			&&	(pOutputBufIn	!= NULL))
			// valid IoctlBuffer (e.g. Adonis)
		{
			// success
			bResultBuf = TRUE;

			// use passed IoctlBuffer
			pIn		= pInputBufIn;
			pOut	= pOutputBufIn;
		}
		else	// e.g. Windows
		{
			// preset
			eResult = ePNDEV_IOCTL_INVALID;

			// get IoctlBuffer
			eResult = fnGetIoctlBuf(hOsParIn,
									lSizeInputBufIn,
									lSizeOutputBufIn,
									&pIn,
									&pOut);

			if	(eResult == ePNDEV_IOCTL_OK)
				// success
			{
				bResultBuf = TRUE;
			}
		}

		if	(bResultBuf)
			// success
		{
		KIRQL	lIrqlCur = PASSIVE_LEVEL;
		_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// Note:
			//	- now errors can be returned by pOut->sError

			// preset
			eResult = ePNDEV_IOCTL_ERROR_BY_STRING;

			// Note:
			//	- IOCTLs must not be called at DISPATCH_LEVEL because of some system calls

			if	(!fnCheckIrql(	PASSIVE_LEVEL,
								&lIrqlCur))
				// invalid IRQL
			{
			uPNDEV_IOCTL_OUT* pOutTmp = pOut;

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): IOCTL 0x%x: Invalid Irql (=%u) [%s()]!"),
									lIoctlIn,
									lIrqlCur,
									__FUNCTION__);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								_countof(pOutTmp->sError),
								pOutTmp->sError);
			}
			else
			{
			BOOLEAN bResultService = FALSE;

				if	(pDevExtIn == NULL)
					// DriverService (not a DeviceService) -> no DeviceHandle!
				{
					switch	(lIoctlIn)
							// IoControlCode
					{
						case IOCTL_GET_DEVICE_INFO_SYS:
						{
							// service GetDeviceInfoSys
							bResultService = fnIoctlGetDeviceInfoSys(	(uPNDEV_GET_DEVICE_INFO_SYS_IN*)	pIn,
																		(uPNDEV_GET_DEVICE_INFO_SYS_OUT*)	pOut);

							break;
						}
						case IOCTL_GET_DEVICE_HANDLE_SYS:
						{
							// service GetDeviceHandleSys
							bResultService = fnIoctlGetDeviceHandleSys(	(uPNDEV_GET_DEVICE_HANDLE_SYS_IN*)	pIn,
																		(uPNDEV_GET_DEVICE_HANDLE_SYS_OUT*)	pOut);

							break;
						}
						case IOCTL_ALLOC_EVENT_ID:
						{
							// service AllocEventId
							bResultService = fnIoctlAllocEventId(	(uPNDEV_ALLOC_EVENT_ID_IN*)		pIn,
																	(uPNDEV_ALLOC_EVENT_ID_OUT*)	pOut);

							break;
						}
						case IOCTL_FREE_EVENT_ID:
						{
							// service FreeEventId
							bResultService = fnIoctlFreeEventId((uPNDEV_FREE_EVENT_ID_IN*)	pIn,
																(uPNDEV_FREE_EVENT_ID_OUT*)	pOut);

							break;
						}
						default:
						{
							break;
						}
					}

					if	(bResultService)
						// success
					{
						eResult = ePNDEV_IOCTL_OK;
					}
				}
				else
				{
					//------------------------------------------------------------------------
					// enter CriticalSection (serialize IOCTLs for this device)
					fnEnterCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_IOCTL);
					{
					    
						if	(pDevExtIn->bIoctlRunning)
							// IOCTL interrupted itself
						{
						uPNDEV_IOCTL_OUT* pOutTmp = pOut;

							// set ErrorDetail
							_RtlStringCchPrintf(sErrorDetail,
												_countof(sErrorDetail),
												_TEXT("(): IOCTL 0x%x: IOCTL interrupted itself!"),
												lIoctlIn);

							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											sErrorDetail,
											NULL,
											_countof(pOutTmp->sError),
											pOutTmp->sError);
						}
						else
						{
                            // set variable
							pDevExtIn->bIoctlRunning = TRUE;
							{
							#ifdef PNDEV_GET_RUNTIME_IOCTL
							uPNDEV_CLOCK uClockStart;

									// get clock
									fnGetClock(&uClockStart);
							#endif

								switch	(lIoctlIn)
										// IoControlCode
								{
									case IOCTL_OPEN_DEVICE:
									{
										// service OpenDevice
										bResultService = fnIoctlOpenDevice(	pDevExtIn,
																			(uPNDEV_OPEN_DEVICE_IN*)	pIn,
																			(uPNDEV_OPEN_DEVICE_OUT*)	pOut);

										break;
									}
									case IOCTL_CLOSE_DEVICE:
									{
										// service CloseDevice
										bResultService = fnIoctlCloseDevice(pDevExtIn,
																			(uPNDEV_CLOSE_DEVICE_IN*)	pIn,
																			(uPNDEV_CLOSE_DEVICE_OUT*)	pOut);

										break;
									}
									case IOCTL_RESET_DEVICE:
									{
										// service ResetDevice
										bResultService = fnIoctlResetDevice(pDevExtIn,
																			(uPNDEV_RESET_DEVICE_IN*)	pIn,
																			(uPNDEV_RESET_DEVICE_OUT*)	pOut);

										break;
									}
									case IOCTL_GET_EVENT_LIST:
									{
										// service GetEventList
										bResultService = fnIoctlGetEventList(	pDevExtIn,
																				(uPNDEV_GET_EVENT_LIST_IN*)		pIn,
																				(uPNDEV_GET_EVENT_LIST_OUT*)	pOut);

										break;
									}
									case IOCTL_SET_EVENT_LIST:
									{
										// service SetEventList
										bResultService = fnIoctlSetEventList(	pDevExtIn,
																				(uPNDEV_SET_EVENT_LIST_IN*)		pIn,
																				(uPNDEV_SET_EVENT_LIST_OUT*)	pOut);

										break;
									}
									case IOCTL_CHANGE_INT:
									{
										// service ChangeInt
										bResultService = fnIoctlChangeInt(	pDevExtIn,
																			(uPNDEV_CHANGE_INT_IN*)		pIn,
																			(uPNDEV_CHANGE_INT_OUT*)	pOut);

										break;
									}
									case IOCTL_GET_DEVICE_STATE:
									{
										// service GetDeviceState
										bResultService = fnIoctlGetDeviceState(	pDevExtIn,
																				(uPNDEV_GET_DEVICE_STATE_IN*)	pIn,
																				(uPNDEV_GET_DEVICE_STATE_OUT*)	pOut);

										break;
									}
									case IOCTL_PROGRAM_FLASH:
									{
										// service ProgramFlash
										bResultService = fnIoctlProgramFlash(	pDevExtIn,
																				(uPNDEV_PROGRAM_FLASH_IN*)	pIn,
																				(uPNDEV_PROGRAM_FLASH_OUT*)	pOut);

										break;
									}
									case IOCTL_CHANGE_XHIF_PAGE:
									{
										// service ChangeXhifPage
										bResultService = fnIoctlChangeXhifPage(	pDevExtIn,
																				(uPNDEV_CHANGE_XHIF_PAGE_IN*)	pIn,
																				(uPNDEV_CHANGE_XHIF_PAGE_OUT*)	pOut);

										break;
									}
									case IOCTL_COPY_DATA:
									{
										// service CopyData
										bResultService = fnIoctlCopyData(	pDevExtIn,
																			(uPNDEV_COPY_DATA_IN*)	pIn,
																			(uPNDEV_COPY_DATA_OUT*)	pOut);

										break;
									}
									case IOCTL_START_BOOT_FW:
									{
										// service StartBootFw
										bResultService = fnIoctlStartBootFw(pDevExtIn,
																			(uPNDEV_START_BOOT_FW_IN*)	pIn,
																			(uPNDEV_START_BOOT_FW_OUT*)	pOut);

										break;
									}
									case IOCTL_START_USER_FW:
									{
										// service StartUserFw
										bResultService = fnIoctlStartUserFw(pDevExtIn,
																			(uPNDEV_START_USER_FW_IN*)	pIn,
																			(uPNDEV_START_USER_FW_OUT*)	pOut);

										break;
									}
									case IOCTL_CHANGE_APP_LOCK:
									{
										// service ChangeAppLock
										bResultService = fnIoctlChangeAppLock(	pDevExtIn,
																				(uPNDEV_CHANGE_APP_LOCK_IN*)	pIn,
																				(uPNDEV_CHANGE_APP_LOCK_OUT*)	pOut);

										break;
									}
									case IOCTL_NOTIFY_DLL_STATE:
									{
										// service NotifyDllState
										bResultService = fnIoctlNotifyDllState(	pDevExtIn,
																				(uPNDEV_NOTIFY_DLL_STATE_IN*)	pIn,
																				(uPNDEV_NOTIFY_DLL_STATE_OUT*)	pOut);

										break;
									}
                                    case IOCTL_DO_NETPROFI:
		                            {
		                            	// execute service at PASSIVE_LEVEL (BufferedMode)
                                        bResultService =  fnIoctlDoNetProfi(pDevExtIn,
                                        									(uPNDEV_NP_DO_NETPROFI_REQ_IN*) 	pIn,
					                    						            (uPNDEV_NP_DO_NETPROFI_REQ_OUT*)	pOut);

                                        break;
		                            }
    								case IOCTL_MAP_MEMORY:
    								{
    									// service MapMemory
    									bResultService = fnIoctlMapMemory(	pDevExtIn,
    																		(uPNDEV_MAP_MEMORY_IN*)		pIn,
    																		(uPNDEV_MAP_MEMORY_OUT*)	pOut);

    									break;
    								}
    								case IOCTL_SET_HW_RES_UM:
    								{
    									// service MapMemory
    									bResultService = fnIoctlSetHwResUm(	pDevExtIn,
    																		(uPNDEV_OPEN_DEVICE_OUT*)	pIn,
    																		(uPNDEV_OPEN_DEVICE_OUT*)	pOut);

    									break;
    								}
									case IOCTL_IS_USER_FW_READY:
									{
										// service StartUserFw
										bResultService = fnIoctlIsUserFwReady(	pDevExtIn,
																				(uPNDEV_IS_USER_FW_READY_IN*)	pIn,
																				(uPNDEV_IS_USER_FW_READY_OUT*)	pOut);

										break;
									}
									default:
									{
										break;
									}
								}

								if	(bResultService)
									// success
								{
									eResult = ePNDEV_IOCTL_OK;
								}

								#ifdef PNDEV_GET_RUNTIME_IOCTL
								{
								UINT32 lRuntime_Usec = 0;

									// compute Runtime in usec
									lRuntime_Usec = fnComputeRunTimeUsec(&uClockStart);

									// set TraceDetail
									RtlStringCchPrintfA(sTraceDetail,
														_countof(sTraceDetail),
														"PnDev[%s]: ++++ IOCTL 0x%x Runtime=%u usec",
														pDevExtIn->sPciLocShortAscii,
														lIoctlIn,
														lRuntime_Usec);

									// set trace
									fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
												ePNDEV_TRACE_CONTEXT_BASIC,
												FALSE,						// logging
												sTraceDetail);
								}
								#endif
							}
							// reset variable
							pDevExtIn->bIoctlRunning = FALSE;
						}
					}
					//------------------------------------------------------------------------
					// leave CriticalSection
					fnLeaveCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_IOCTL);
				}
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service OpenDevice
//************************************************************************

BOOLEAN fnIoctlOpenDevice(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_OPEN_DEVICE_IN*	pIn,
							uPNDEV_OPEN_DEVICE_OUT*	pOut)
{
BOOLEAN				bResult				= FALSE;
BOOLEAN				bMasterApp			= FALSE;
UINT32				lCtrFw				= 0;
BOOLEAN				bDebugModeFw		= FALSE;
BOOLEAN				bShmDebugFw			= FALSE;
UINT32				lTimeScaleFactor	= 0;
BOOLEAN				bReqHostMem			= FALSE;
uPNDEV_APP*			pApp				= NULL;
uPNDEV_THREAD_SYS   uThreadSys;
uPNDEV_THREAD_IOCTL	uThreadIoctl;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		RtlZeroMemory(	&uThreadSys,
						sizeof(uPNDEV_THREAD_SYS));

		RtlZeroMemory(	&uThreadIoctl,
						sizeof(uPNDEV_THREAD_IOCTL));

		// copy input data
		{
			bMasterApp			= pIn->bMasterApp;
			lCtrFw				= pIn->uUserFw.lCtrFw;
			bDebugModeFw		= pIn->uUserFw.bDebugMode;
			bShmDebugFw			= pIn->uUserFw.bShmDebug;
			lTimeScaleFactor	= pIn->lTimeScaleFactor;
            bReqHostMem         = pIn->bReqHostMem;


			*((uPNDEV_THREAD_SYS*)   &uThreadSys)   = *((uPNDEV_THREAD_SYS*)   &pIn->uThreadSys);
			*((uPNDEV_THREAD_IOCTL*) &uThreadIoctl) = *((uPNDEV_THREAD_IOCTL*) &pIn->uInternal.uThreadIoctl);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_OPEN_DEVICE_OUT));
	}

	// set ScaleFactor for WaitTimes and TimeoutTimes
	{
		if	(lTimeScaleFactor == 0)
			// default value desired
		{
			// use original TimeValues
			pDevExtIn->lTimeScaleFactor = 1;
		}
		else
		{
			// stretch TimeValues by this factor
			pDevExtIn->lTimeScaleFactor = lTimeScaleFactor;
		}
	}

	// allocate AppObject
	pApp = fnAllocMemNonPaged(	sizeof(uPNDEV_APP),
								TAG('O','p','e','n'));

	if	(pApp == NULL)
		// error
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Insufficient memory for AppObject!"),
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// preset AppObject
		RtlZeroMemory(	pApp,
						sizeof(uPNDEV_APP));

		if 	(fnStartUpThreadResDll(	pDevExtIn,
									pApp,
									&uThreadIoctl,
									_countof(pOut->sError),
									pOut->sError))
			// starting up resources of DllThread ok
		{
		BOOLEAN bResultLock = TRUE;
		BOOLEAN bResulTmp 	= TRUE;

			// update AppList
			{
				//------------------------------------------------------------------------
				// enter CriticalSection
				fnEnterCritSec(	pDevExtIn,
								ePNDEV_CRIT_SEC_SYS_APP_LIST);
				{
					// put AppObject to AppList
					fnPutBlkToList(	&pDevExtIn->uListApp,
									((uLIST_HEADER*) pApp));

					pDevExtIn->lCtrApp++;
				}
				//------------------------------------------------------------------------
				// leave CriticalSection
				fnLeaveCritSec(	pDevExtIn,
								ePNDEV_CRIT_SEC_SYS_APP_LIST);
			}

			if	(pDevExtIn->lCtrApp == 1)
				// first OpenDevice
			{
				// enable BusMaster here
				bResulTmp = fnEnableBusMasterConfigSpace(	pDevExtIn,
															_countof(pOut->sError),
															pOut->sError);
				
				// preset DllState of first OpenDevice
				//	- lock OpenDevice for verifying environment and starting Fw at DLL afterwards
				//	- during this phase there must not be an OpenDevice of any other App
				pDevExtIn->bDllReadyFirstOpenDevice = FALSE;
			}
			else
			{
				if	(	!pDevExtIn->bDllReadyFirstOpenDevice
					&& 	!bShmDebugFw)
					// first OpenDevice not completed at DLL and not opened for shared memory debugging!
				{
					// error
					bResultLock = FALSE;

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): First OpenDevice not completed at DLL!"),
									NULL,
									_countof(pOut->sError),
									pOut->sError);
				}
			}

			if	(	bResultLock 
				&& 	bResulTmp)
				// success
			{
			BOOLEAN bResultMasterApp = TRUE;

				if	(bMasterApp)
					// MasterApp
				{
					if	(pDevExtIn->pMasterApp != NULL)
						// MasterApp already exist
					{
						// error
						bResultMasterApp = FALSE;

						// set ErrorString
						//	- changed by DLL to ReturnValue=ePNDEV_ERROR_MASTER_APP
						fnBuildString(	PNDEV_ERROR_MASTER_APP,
										NULL,
										NULL,
										_countof(pOut->sError),
										pOut->sError);
					}
					else if (!pDevExtIn->bLegIntConnected)
							// no LegIntConnected
					{
						// error
						bResultMasterApp = FALSE;

						// set ErrorString
						//	- changed by DLL to ReturnValue=ePNDEV_ERROR_MASTER_APP
						fnBuildString(	NULL,
										_TEXT("Connected LegacyInt for MasterApplicaton required (start Non-MasterApplication)!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

					}
					else if (	(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_HERA)
							||	(pDevExtIn->eBoard == ePNDEV_BOARD_TI_OMAP))
							// Fpga-Hera Board
							// TI Omap board
					{
						// error
						bResultMasterApp = FALSE;

						// set ErrorString
						fnBuildString(	NULL,
										_TEXT("MasterApp for Fpga-Hera board not valid, because there is no interrupt handling supported (start Non-MasterApplication)!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

					}
					else
					{
						// book IntHandling
						pDevExtIn->pMasterApp = pApp;
					}
				}

				if	(bResultMasterApp)
					// success
				{
				BOOLEAN bResultCleanHw = TRUE;

					if	(	(bMasterApp)
						||	(lCtrFw != 0))
						//		MasterApp required (EDDx needs clean hardware)
						// OR	starting of UserFw
					{
						if	(pDevExtIn->lCtrApp > 1)
							// not first OpenDevice
							//	- decision cannot be done at DLL because AppCtr is only consistent in driver context (service GetDeviceState returns only a copy!)
						{
							// Hw may not be in ResetState

							// error
							bResultCleanHw = FALSE;

							// set ErrorString
							fnBuildString(	NULL,
											_TEXT("CloseDevice of all other applications required (start of UserFw / MasterApp need a clean hardware by ResetDevice)!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);

							// set event 'ReqClose'
							{
								pDevExtIn->uEvent.bReqClose = TRUE;

								// check EventList
								//	- must be done here:
								//		SysThread is only started if service is ok!
								//		SysThread does not know the application which should be excluded!
								fnCheckEventList(	pDevExtIn,
													pApp,		// App excluded
													FALSE);		// not called by SysThread
							}
						}
					}

					if	(bResultCleanHw)
						// success
					{
						if	(pDevExtIn->lCtrApp == 1)
							// first OpenDevice
						{
							// Note:
							//	- don't try to read RemanentData if current board is not a known PcBoard!

							if	(pDevExtIn->bPcBoard)
								// PcBoard
							{
							_TCHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

								if	(!fnBoard_GetRemanentData(	pDevExtIn,
																_countof(sError),
																sError))
									// error at getting RemanentData
								{
									// Note:
									//	- if there are flash problems OpenDevice cannot solve them
									//		-> returned error is only used for debug purpose

									// set trace by returned ErrorString
									fnSetTraceFctError(	pDevExtIn,
														sError);
								}
							}
						}

						// return RemanentData
						*((uPNCORE_REMANENT_DATA*) &pOut->uHwRes.uCommon.uRemanentData) = *((uPNCORE_REMANENT_DATA*) &pDevExtIn->uRemanentData);

						if	(!fnPnCore_GetIntPar(	pDevExtIn->eAsic,
													ePNCORE_ICU_HOST,
													&pOut->uCoreIf.uIntPar_IcuHost))
							// error at getting parameter of INT_CoreIf (HostCore-ICU)
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at fnPnCore_GetIntPar() [HostCore ICU]!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
						}
						else if	(!fnPnCore_GetIntPar(	pDevExtIn->eAsic,
														ePNCORE_ICU_ASIC,
														&pOut->uCoreIf.uIntPar_IcuAsic))
							// error at getting parameter of INT_CoreIf (AsicCore-ICU)
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at fnPnCore_GetIntPar() [AsicCore ICU]!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
						}
						else
						{
							if	(fnBoard_GetHwInfo(	pDevExtIn,
													_countof(pOut->sError),
													&pOut->uHwInfo,
													pOut->sError))
								// getting HwInfo ok
							{
							BOOLEAN	bResultThreadSys	= FALSE;
							UINT32	lThreadId			= 0;

								if	(pDevExtIn->lCtrApp != 1)
									// not first OpenDevice
								{
									// success
									bResultThreadSys = TRUE;
								}
								else
								{
					            UINT32 lStackSize = 0;

									// no UserFw running
									pDevExtIn->bUserFwRunning = FALSE;

									// reset ThreadCtr
									pDevExtIn->lCtrThreadSys = 0;

									// set parameter of SysThread
									{
										if	(uThreadSys.lStackSize == 0)
											// default value desired
										{
											// 8kByte
											lStackSize = (1024*8);
										}
										else
										{
											lStackSize = uThreadSys.lStackSize;
										}

										if	(uThreadSys.lTimeCycle_10msec == 0)
											// default value desired
										{
											// 100msec
											pDevExtIn->lTimeCycleThreadSys_10msec = 10;
										}
										else
										{
											pDevExtIn->lTimeCycleThreadSys_10msec = uThreadSys.lTimeCycle_10msec;
										}
									}

									if	(fnStartUpThreadResSys(	pDevExtIn,
																uThreadSys.lPrio,
                                                                lStackSize,
                                                                uThreadSys.sName,
																_countof(pOut->sError),
																&lThreadId,
																pOut->sError))
										// starting up resources of periodic SysThread ok
									{
										// success
										bResultThreadSys = TRUE;
									}
								}

								if	(bResultThreadSys)
									// success
								{

                                    if(fnCheckSharedHostMemUM(	pDevExtIn,
                                    							bReqHostMem,
                                    							bMasterApp,
                                    							_countof(pOut->sError),
                                    							pOut->sError))
                                        // success
                                    {
                                    	if	(fnBoard_SetPtrMemMapUM(	pDevExtIn,
                                    									_countof(pOut->sError),
                                    									&pOut->uInternal,
                                    									pOut->sError))
											// success
                                    	{
                                    		// success
                                    		bResult = TRUE;

                                    		// return
											{
												_RtlStringCchCopy(	pOut->sPciLocShort,
																	_countof(pOut->sPciLocShort),
																	pDevExtIn->sPciLocShort);

												// SblInfo
												*((uPNDEV_SBL_INFO*) &pOut->uSblInfo) = *((uPNDEV_SBL_INFO*) &pDevExtIn->uSblInfo);

												pOut->uThreadInfo.lIdSys		= lThreadId;
												pOut->uInternal.eIcu			= (UINT32) pDevExtIn->eIcu;
												pOut->uInternal.uAppSys.hHandle	= pApp;							// necessary for some services

												if	(pDevExtIn->lCtrApp == 1)
													// first OpenDevice
												{
													// return
													pOut->uInternal.bFirstOpenDevice = TRUE;
												}
											}
											/*
											if	(pDevExtIn->lCtrApp == 1)
												// first OpenDevice
											{
												// green LED=ON, red LED=OFF
												fnBoard_SetLed(	pDevExtIn,
																TRUE,
																FALSE);
											}
											 */
                                    	}
                                    }
								}
							}
						}
					}
				}
			}
		}

		if	(!bResult)
			// error
		{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

			if	(!fnCloseDevice(pDevExtIn,
								pApp,
								_countof(sError),
								sError))
				// error at closing device
			{
				// set trace by returned ErrorString
				fnSetTraceFctError(	pDevExtIn,
									sError);
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service CloseDevice
//************************************************************************

BOOLEAN fnIoctlCloseDevice(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_CLOSE_DEVICE_IN*		pIn,
							uPNDEV_CLOSE_DEVICE_OUT*	pOut)
{
BOOLEAN		bResult		= FALSE;
uPNDEV_APP*	pApp		= NULL;
BOOLEAN		bDoAnyway	= FALSE;
BOOLEAN		bDoClose	= FALSE;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp		= (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;
		bDoAnyway	= pIn->bDoAnyway;

		// preset OutputParameter
		//	- necessary because of METHOD_BUFFERED
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CLOSE_DEVICE_OUT));
	}

	// preset
	bDoClose = TRUE;

	if	(pApp->bStartedUserFw)
		// UserFw started by this App
	{
		// code cannot be done at DLL because AppCtr is only consistent in driver context (service GetDeviceState returns only a copy!)

		if	(pDevExtIn->lCtrApp > 1)
			// other App exist
		{
			if	(!bDoAnyway)
				// standard CloseDevice
			{
				bDoClose = FALSE;

				// Note:
				//	- internal ResetDevice is done at last CloseDevice

				// set ErrorString
				fnBuildString(	NULL,
								_TEXT("CloseDevice of all other applications required (stop of UserFw needs a ResetDevice)!"),
								NULL,
								_countof(pOut->sError),
								pOut->sError);

				// set event 'ReqClose'
				{
					pDevExtIn->uEvent.bReqClose = TRUE;

					// check EventList
					//	- must be done here:
					//		SysThread does not know the application which should be excluded!
					fnCheckEventList(	pDevExtIn,
										pApp,		// App excluded
										FALSE);		// not called by SysThread
				}
			}
		}
	}

	if	(bDoClose)
		// success
	{
		// set ptr to InternalOut
		{
		UINT32 i = 0;

			for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
			{
				// set ptr to InternalOut
				pOut->uInternal.uBarUm[i] = pApp->uHwResUm.uBar[i];
			}

			pOut->uInternal.uSdramUm = pApp->uHwResUm.uAsicSdramDirect;

			pOut->uInternal.uHostMemUm = g_uMemSys.uHostMem[pDevExtIn->lUsesHostMemIndex].uHwResUserSpace;
		}

		// close device
		bResult = fnCloseDevice(pDevExtIn,
								pApp,
								_countof(pOut->sError),
								pOut->sError);
/*
		if	(pDevExtIn->lCtrApp == 0)
			// last CloseDevice
		{
			// green LED=OFF, red LED=OFF
			fnBoard_SetLed(	pDevExtIn,
							FALSE,
							FALSE);
		}
*/
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ResetDevice
//************************************************************************

BOOLEAN fnIoctlResetDevice(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_RESET_DEVICE_IN*		pIn,
							uPNDEV_RESET_DEVICE_OUT*	pOut)
{
BOOLEAN		bResult		= FALSE;
BOOLEAN		bResultTmp	= FALSE;
uPNDEV_APP*	pApp		= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp = (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_RESET_DEVICE_OUT));
	}

	if	(pDevExtIn->lCtrApp == 1)
		// only one App exist
	{
		// success
		bResultTmp = TRUE;
	}
	else
	{
		if	(pDevExtIn->pMasterApp == pApp)
			// MasterApp
		{
			// success
			bResultTmp = TRUE;
		}
		else
		{
			// set ErrorString
			//	- changed by DLL to ReturnValue=ePNDEV_ERROR_MASTER_APP
			fnBuildString(	PNDEV_ERROR_MASTER_APP,
							NULL,
							NULL,
							_countof(pOut->sError),
							pOut->sError);
		}
	}

	if	(bResultTmp)
		// success
	{
		// reset board
		bResult = (*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
																	ePNDEV_RESET_ACTION_REINIT_HW,
																	_countof(pOut->sError),
																	pOut->sError);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetEventList (get events by DLL)
//************************************************************************

BOOLEAN fnIoctlGetEventList(DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_GET_EVENT_LIST_IN*	pIn,
							uPNDEV_GET_EVENT_LIST_OUT*	pOut)
{
BOOLEAN		bResult	= FALSE;
uPNDEV_APP* pApp	= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp = (uPNDEV_APP*) pIn->uAppSys.hHandle;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_GET_EVENT_LIST_OUT));
	}

	if	(pApp == NULL)
		// invalid AppObject
	{
	_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Invalid AppObject!"));

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// success
		bResult = TRUE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		fnEnterCritSec(	pDevExtIn,
						ePNDEV_CRIT_SEC_SYS_APP_LIST);
		{
			// copy events
			pOut->uEvent.lFatalFw		= pApp->uEvent.lFatalFw;
			pOut->uEvent.eFatalAsync 	= pApp->uEvent.eFatalAsync;
			pOut->uEvent.eWarning 		= pApp->uEvent.eWarning;
			pOut->uEvent.bReqClose 		= pApp->uEvent.bReqClose;
			pOut->uEvent.eDebug 		= pApp->uEvent.eDebug;

			// clear events at AppObject
			pApp->uEvent.lFatalFw		= 0;
			pApp->uEvent.eFatalAsync	= ePNDEV_FATAL_ASYNC__INVALID;
			pApp->uEvent.eWarning		= ePNDEV_WARNING__INVALID;
			pApp->uEvent.bReqClose		= FALSE;
			pApp->uEvent.eDebug			= ePNDEV_DEBUG__INVALID;
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		fnLeaveCritSec(	pDevExtIn,
						ePNDEV_CRIT_SEC_SYS_APP_LIST);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service SetEventList (set events by DLL)
//************************************************************************

BOOLEAN fnIoctlSetEventList(DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_SET_EVENT_LIST_IN*	pIn,
							uPNDEV_SET_EVENT_LIST_OUT*	pOut)
{
BOOLEAN			bResult	= FALSE;
uPNDEV_APP*		pApp	= NULL;
uPNDEV_EVENT	uEvent;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		RtlZeroMemory(	&uEvent,
						sizeof(uPNDEV_EVENT));

		// copy input data
		{
			pApp = (uPNDEV_APP*) pIn->uAppSys.hHandle;

			*((uPNDEV_EVENT*) &uEvent) = *((uPNDEV_EVENT*) &pIn->uEvent);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_SET_EVENT_LIST_OUT));
	}

	if	(pApp == NULL)
		// invalid AppObject
	{
	_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Invalid AppObject!"));

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// success
		bResult = TRUE;

		if	(uEvent.lFatalFw != 0)
			// event set
		{
			pDevExtIn->uEvent.lFatalFw = uEvent.lFatalFw;
		}

		if	(uEvent.eFatalAsync != ePNDEV_FATAL_ASYNC__INVALID)
			// event set
		{
			pDevExtIn->uEvent.eFatalAsync = uEvent.eFatalAsync;
		}

		if	(uEvent.eWarning != ePNDEV_WARNING__INVALID)
			// event set
		{
			pDevExtIn->uEvent.eWarning = uEvent.eWarning;
		}

		if	(uEvent.bReqClose)
			// event set
		{
			pDevExtIn->uEvent.bReqClose = uEvent.bReqClose;
		}

		if	(uEvent.eDebug != ePNDEV_DEBUG__INVALID)
			// event set
		{
			pDevExtIn->uEvent.eDebug = uEvent.eDebug;
		}

		// check EventList
		//	- do it here at once:
		//		currently only one event of each EventClass can be stored
		fnCheckEventList(	pDevExtIn,
							NULL,		// no App excluded
							FALSE);		// not called by SysThread
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ChangeInt
//************************************************************************

BOOLEAN fnIoctlChangeInt(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_CHANGE_INT_IN*	pIn,
							uPNDEV_CHANGE_INT_OUT*	pOut)
{
BOOLEAN				bResult		= FALSE;
ePNDEV_INT_ACTION	eIntAction	= ePNDEV_INT_ACTION_INVALID;
BOOLEAN				bArrayIntUnmask[PNDEV_DIM_ARRAY_INT_SRC];
uPNDEV_INT_POLARITY	uIntPolarity;
uPNDEV_APP*			pApp = NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		RtlZeroMemory(	bArrayIntUnmask,
						(sizeof(BOOLEAN) * PNDEV_DIM_ARRAY_INT_SRC));

		RtlZeroMemory(	&uIntPolarity,
						sizeof(uPNDEV_INT_POLARITY));

		// copy input data
		{
			eIntAction	= pIn->eIntAction;
			pApp		= (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;

			RtlCopyMemory(	bArrayIntUnmask,
							pIn->bArrayIntUnmask,
							(sizeof(BOOLEAN) * PNDEV_DIM_ARRAY_INT_SRC));

			*((uPNDEV_INT_POLARITY*) &uIntPolarity) = *((uPNDEV_INT_POLARITY*) &pIn->uIntPolarity);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CHANGE_INT_OUT));
	}

	// update IntAction
	pDevExtIn->eIntAction = eIntAction;

	// reset IntStatistic
	RtlZeroMemory(	&pDevExtIn->uIntStatistic,
					sizeof(uPNDEV_INT_STATISTIC));

	switch	(pDevExtIn->eIcu)
			// IcuType
	{
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
		UINT32 lRegEdge	= 0;
		UINT32 lRegPull	= 0;
		UINT32 lTmp		= 0;

			// Note:
			//	- interrupts must be edge triggered, never level triggered!
			//	- same edge must be used for output of IntSrc and input of PCI-ICU
			//	- if an edge is changed it may happen that an interrupt will be stored at PCI-ICU-IRR, this must be cleared before interrupts are unmasked!

			// debug purpose
			lRegEdge	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A);
			lRegPull	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6);

			if	(bArrayIntUnmask[ePNDEV_INT_SRC_SOC__IRTE_IRQ1])
				// unmask IrteIrq1
			{
				// Note:
				//	- default output edge after reset: IRQ1_HP of IRTE is low-active (don't change IRTE)
				//	- default input  edge after reset: PCI-ICU is high-active

				lTmp = 1<<SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ1;

				// change PCI-ICU interrupt detection of PCI-ICU-MuxA-Input1 to falling edge
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A) | lTmp));

				// ack PCI-ICU-MuxA-EventX
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A),
									SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ1);
			}

			if	(bArrayIntUnmask[ePNDEV_INT_SRC_SOC__GPIO180])
				// unmask Gpio180
			{
				lTmp = 1<<SOC1_PCI_ICU_VECTOR_MUX_A__GPIO180;

				if	(uIntPolarity.as.uSoc1.eGpio180 == ePNDEV_EDGE_FALLING)
					// falling edge required
				{
					// change PCI-ICU interrupt detection of PCI-ICU-MuxA-InputX to falling edge
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A) | lTmp));
				}
				else
				{
					// change PCI-ICU interrupt detection of PCI-ICU-MuxA-InputX to rising edge
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A) & (~lTmp)));
				}

				// ack PCI-ICU-MuxA-EventX
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A),
									SOC1_PCI_ICU_VECTOR_MUX_A__GPIO180);

				lTmp = 1<<(180-160);

				if	(uIntPolarity.as.uSoc1.bEnablePullGpio180)
					// enable PullUp of GPIO180
				{
					// enable internal pull resistor
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6) | lTmp));
				}
				else
				{
					// disable internal pull resistor
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6) & (~lTmp)));
				}
			}

			if	(bArrayIntUnmask[ePNDEV_INT_SRC_SOC__GPIO188])
				// unmask Gpio188
			{
				lTmp = 1<<SOC1_PCI_ICU_VECTOR_MUX_A__GPIO188;

				if	(uIntPolarity.as.uSoc1.eGpio188 == ePNDEV_EDGE_FALLING)
					// falling edge required
				{
					// change PCI-ICU interrupt detection of PCI-ICU-MuxA-InputX to falling edge
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A) | lTmp));
				}
				else
				{
					// change PCI-ICU interrupt detection of PCI-ICU-MuxA-InputX to rising edge
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A) & (~lTmp)));
				}

				// ack PCI-ICU-MuxA-EventX
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A),
									SOC1_PCI_ICU_VECTOR_MUX_A__GPIO188);

				lTmp = 1<<(188-160);

				if	(uIntPolarity.as.uSoc1.bEnablePullGpio188)
					// enable PullUp of GPIO188
				{
					// enable internal pull resistor
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6) | lTmp));
				}
				else
				{
					// disable internal pull resistor
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6) & (~lTmp)));
				}
			}

			// debug purpose
			lRegEdge	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_EDGEREG_A);
			lRegPull	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__GPIO_PULL_CONTROL6);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			break;
		}
		case ePNDEV_ICU_INTEL:	// Intel
		{
			break;
		}
		case ePNDEV_ICU_MICREL:	// Micrel
		{
			break;
		}
		default:
		{
			break;
		}
	}

	if	(pDevExtIn->eIntAction <= PNDEV_INT_ACTION_STD_MAX)
		// IntAction=Standard
	{
		// ePNDEV_INT_ACTION_UNMASK:
		//	- Asic with TopLevel-ICU:    interrupts will be unmasked by DLL afterwards
		//	- Asic without TopLevel-ICU: interrupts will be forwarded/filtered by DLL dependent on bArrayIntUnmask[]

		// success
		bResult = TRUE;
	}
	else	// IntAction=Test
	{
		if	(fnBoard_StartIntTest(	pDevExtIn,
									_countof(pOut->sError),
									pOut->sError))
			// starting IntTest ok
		{
			// success
			bResult = TRUE;

			// Note:
			//	- if BoardInt:	always unmasked -> ISR should be already called
			//	- else:			interrupt will be unmasked by DLL afterwards -> ISR not called up to now
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetDeviceState
//************************************************************************

BOOLEAN fnIoctlGetDeviceState(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_GET_DEVICE_STATE_IN*		pIn,
								uPNDEV_GET_DEVICE_STATE_OUT*	pOut)
{
BOOLEAN		bResult	= FALSE;
uPNDEV_APP* pApp	= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp = (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_GET_DEVICE_STATE_OUT));
	}

	// success
	bResult = TRUE;

	// update OutputParameter of driver
	{
		if	(pDevExtIn->pMasterApp == pApp)
			// MasterApp
		{
			pOut->bMasterApp = TRUE;
		}

		pOut->lCtrApp				= pDevExtIn->lCtrApp;
		pOut->lCtrRef_XhifPage		= pDevExtIn->uUserPageXhif.lCtrRef;
		pOut->bUserFwRunning		= pDevExtIn->bUserFwRunning;
		pOut->uIntStatistic.lCtrDpc	= pDevExtIn->uIntStatistic.lCtrDpc;		// do it before lCtrIsr for ensuring that lCtrIsr>=lCtrDpc
		pOut->uIntStatistic.lCtrIsr	= pDevExtIn->uIntStatistic.lCtrIsr;
	}

	// to do at DLL
	//	- pOut->uIntStatistic.lCtrUisr

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ProgramFlash
//************************************************************************

BOOLEAN fnIoctlProgramFlash(DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_PROGRAM_FLASH_IN*	pIn,
							uPNDEV_PROGRAM_FLASH_OUT*	pOut)
{
BOOLEAN						bResult	= FALSE;
uPNDEV_APP*					pApp	= NULL;
ePNDEV_FLASH_ACTION			eFlashAction;
uPNDEV_PROGRAM_DATA_SRC*	pDataSrc;

	// Note:
	// - Linux Kernel Threads Stack Size is limited to 8k, use kernel heap for big data structures!
	pDataSrc= (uPNDEV_PROGRAM_DATA_SRC*) fnAllocMemNonPaged(	sizeof(uPNDEV_PROGRAM_DATA_SRC),
																TAG('D','a','t','a'));


	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		{
			pApp			= (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;
			eFlashAction	= pIn->eFlashAction;


			*((uPNDEV_PROGRAM_DATA_SRC*) pDataSrc) = *((uPNDEV_PROGRAM_DATA_SRC*) &pIn->as);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_PROGRAM_FLASH_OUT));
	}

	if	(!pDevExtIn->bFlashSupport)
		// Board don't support Flash
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Service ProgramFlash not supported!"),
						NULL,
						_countof(pOut->sError),
						pOut->sError);

	}
	else	// success
	{
        // TODO: UT5507 - Hack to enable Hera because no BootFw is running
        if(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_HERA)
        {
            // enable param 
            pDevExtIn->bBootFwRunning = TRUE;
        }

		if	(	(pDevExtIn->bAsicCoreExist)
			&&	(!pDevExtIn->bBootFwRunning))
			//		AsicCore exist
			// AND	BootFw not running
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): BootFw not running. Please check parameters of OpenDevice!"),
							NULL,
							_countof(pOut->sError),
							pOut->sError);
		}
		else
		{
		BOOLEAN				bResultPage			= FALSE;
		UINT32				lOffsetFlashData	= 0;
		uPNDEV_FLASH_PAR	uFlashPar;

            // TODO: UT5507 - Hack to enable Hera because no BootFw is running
            if(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_HERA)
            {
                // disable param after the critical if
                pDevExtIn->bBootFwRunning = FALSE;
            }

			// Note:
			//	- programming sequence must not be interrupted by other flash accesses!
			//	- Arm9/MIPS firmware must be stopped if it accesses flash by code fetches
			//	- problem can be solved by starting BootFw, running at SDRAM/DDR2RAM (-> no flash access)

			//------------------------------------------------------------------------
			// start using FlashPage
			bResultPage = fnBoard_StartFlashPage(	pDevExtIn,
													TRUE,					// chip can be verified (see above)
													_countof(pOut->sError),
													&uFlashPar,
													&lOffsetFlashData,
													pOut->sError);
			{
				if	(bResultPage)
					// success
				{
				FN_FLASH_PROGRAM pFnFlash_Program = NULL;

					switch	(uFlashPar.eFlashTypeReal)
							// FlashType
					{
						case ePNDEV_FLASH_TYPE_NOR_AM29LV040B:
						case ePNDEV_FLASH_TYPE_NOR_AM29LV641:
						case ePNDEV_FLASH_TYPE_NOR_S29GL064N:
						case ePNDEV_FLASH_TYPE_NOR_S29GL512P:
						case ePNDEV_FLASH_TYPE_NOR_S29WS128P:
						{
							pFnFlash_Program = fnFlashNor_Program;

							break;
						}
						case ePNDEV_FLASH_TYPE_NAND_K91G08U0B:
						{
							pFnFlash_Program = fnFlashNand_Program;

							break;
						}
                        case ePNDEV_FLASH_TYPE_SPI_N25Q128A:
						{
                            pFnFlash_Program = fnFlashSPI_Program;

							break;
						}
						default:
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid FlashType!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);

							break;
						}
					}

					if	(pFnFlash_Program != NULL)
						// success
					{
						if	(fnFlash_VerifyAction(	pDevExtIn,
													eFlashAction,
													uFlashPar.eFlashInfo,
													_countof(pOut->sError),
													pOut->sError))
							// verifying FlashAction ok
						{
							switch	(eFlashAction)
									// action
							{
								case ePNDEV_FLASH_ACTION_ERASE_CHIP:
								{
									// program flash
									bResult = (*pFnFlash_Program)(	pDevExtIn,
																	&uFlashPar,
																	0,						// OffsetDest
																	NULL,					// DataSrc
																	0,						// SizeSrc
																	TRUE,					// EraseChip
																	_countof(pOut->sError),
																	pOut->sError);

									break;
								}
								case ePNDEV_FLASH_ACTION_ERASE_SECTOR_DATA:
								{
									// program flash
									bResult = (*pFnFlash_Program)(	pDevExtIn,
																	&uFlashPar,
																	lOffsetFlashData,		// OffsetDest
																	NULL,					// DataSrc
																	0,						// SizeSrc
																	FALSE,					// EraseChip
																	_countof(pOut->sError),
																	pOut->sError);

									break;
								}
								case ePNDEV_FLASH_ACTION_PROGRAM_DATA:
								{
									if	(pDataSrc->uRemanentData.lCtrMacAdr > PNDEV_MAC_ADR_MAX)
										// invalid number of MacAdr
									{
										// set ErrorString
										fnBuildString(	_TEXT(__FUNCTION__),
														_TEXT("(): Invalid number of MacAdr (must be <= PNDEV_MAC_ADR_MAX)!"),
														NULL,
														_countof(pOut->sError),
														pOut->sError);
									}
									else
									{
									UINT32 lCtrMacAdrMin = 0;

										switch	(pDevExtIn->eAsic)
												// AsicType
										{
											case ePNDEV_ASIC_ERTEC400:		{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC400;	break;}
											case ePNDEV_ASIC_ERTEC200:		{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC200;	break;}
											case ePNDEV_ASIC_SOC1:			{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__SOC1;		break;}
											case ePNDEV_ASIC_ERTEC200P:		{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC200P;	break;}
                                            case ePNDEV_ASIC_HERA:	    	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__HERA;   	break;}
											default:
											{
												// set ErrorString
												fnBuildString(	_TEXT(__FUNCTION__),
																_TEXT("(): Invalid AsicType!"),
																NULL,
																_countof(pOut->sError),
																pOut->sError);

												break;
											}
										}

										if	(lCtrMacAdrMin != 0)
											// success
										{
											if	(pDataSrc->uRemanentData.lCtrMacAdr < lCtrMacAdrMin)
												// invalid number of MacAdr
											{
												// set ErrorString
												fnBuildString(	_TEXT(__FUNCTION__),
																_TEXT("(): Invalid number of MacAdr (must be >= PNDEV_MAC_ADR_MIN__xx)!"),
																NULL,
																_countof(pOut->sError),
																pOut->sError);
											}
											else
											{
												// CP1616:	lCtrMacAdr: type=UINT8, 0x00/0xff means 2 MacAdr
												// else:	lCtrMacAdr: type=UINT32

												if	(pDevExtIn->eBoard == ePNDEV_BOARD_CP1616)
													// CP1616 board
												{
												uPNDEV_FLASH_DATA_CP1616* pFlashData;

												// Note:
												// - Linux Kernel Threads Stack Size is limited to 8k, use kernel heap for big data structures!
												pFlashData = (uPNDEV_FLASH_DATA_CP1616*) fnAllocMemNonPaged(	sizeof(uPNDEV_FLASH_DATA_CP1616),
																												TAG('D','a','t','a'));

													// HwInfo must be completely rewritten

													// read FlashData
													*((uPNDEV_FLASH_DATA_CP1616*) pFlashData) = *((uPNDEV_FLASH_DATA_CP1616*) (uFlashPar.pFlash + lOffsetFlashData));

													// change FlashData
													{
														pFlashData->lCtrMacAdr = (UINT8) pDataSrc->uRemanentData.lCtrMacAdr;

														// convert MacAdr from UINT64 to array
														fnBoard_ConvertMacAdr(	pDataSrc->uRemanentData.lMacAdr64,
																				(volatile UINT8*) &pFlashData->uMacAdr0[0]);

														RtlCopyMemory(	(void*) (pFlashData->lArrayPrivate),
																		(void*) (pDataSrc->uRemanentData.lArrayPrivate),
																		PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
													}

													// program flash
													bResult = (*pFnFlash_Program)(	pDevExtIn,
																					&uFlashPar,
																					lOffsetFlashData,					// OffsetDest
																					(volatile UINT8*) pFlashData,		// DataSrc
																					sizeof(uPNDEV_FLASH_DATA_CP1616),	// SizeSrc
																					FALSE,								// EraseChip
																					_countof(pOut->sError),
																					pOut->sError);
													// delete memory of big data structure
													fnFreeMemNonPaged(	pFlashData,
																		TAG('D','a','t','a'));
												}
												else
												{
												uPNDEV_FLASH_DATA* pFlashData;

												// Note:
												// - Linux Kernel Threads Stack Size is limited to 8k, use kernel heap for big data structures!
												pFlashData = (uPNDEV_FLASH_DATA*) fnAllocMemNonPaged(	sizeof(uPNDEV_FLASH_DATA),
																										TAG('D','a','t','a'));

													RtlZeroMemory(	pFlashData,
																	sizeof(uPNDEV_FLASH_DATA));

													// set FlashData
													{
														pFlashData->eVersion		= ePNDEV_FLASH_VERSION_V0001;
														pFlashData->lCtrMacAdr	= pDataSrc->uRemanentData.lCtrMacAdr;
														pFlashData->lMacAdr64	= pDataSrc->uRemanentData.lMacAdr64;

														RtlCopyMemory(	(void*) (pFlashData->lArrayPrivate),
																		(void*) (pDataSrc->uRemanentData.lArrayPrivate),
																		PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
													}

													// program flash
													bResult = (*pFnFlash_Program)(	pDevExtIn,
																					&uFlashPar,
																					lOffsetFlashData,				// OffsetDest
																					(volatile UINT8*) pFlashData,	// DataSrc
																					sizeof(uPNDEV_FLASH_DATA),		// SizeSrc
																					FALSE,							// EraseChip
																					_countof(pOut->sError),
																					pOut->sError);
													// delete memory of big data structure
													fnFreeMemNonPaged(	pFlashData,
																		TAG('D','a','t','a'));
												}
											}
										}
									}

									break;
								}
								case ePNDEV_FLASH_ACTION_PROGRAM_SBL:
								{
									// program flash
									bResult = (*pFnFlash_Program)(	pDevExtIn,
																	&uFlashPar,
																	0,							// OffsetDest: Sector0/Block0
																	pDataSrc->uSbl.lArrayFw,	// DataSrc
																	PNDEV_DIM_ARRAY_SBL,		// SizeSrc
																	FALSE,						// EraseChip
																	_countof(pOut->sError),
																	pOut->sError);

									break;
								}
								default:
								{
									// set ErrorString
									fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid FlashAction!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);

									break;
								}
							}
						}
					}
				}
			}
		}
	}
	//------------------------------------------------------------------------
	// stop using FlashPage
	fnBoard_StopFlashPage(pDevExtIn);

	// delete memory of big data structure
	fnFreeMemNonPaged(	pDataSrc,
						TAG('D','a','t','a'));

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ChangeXhifPage
//************************************************************************

BOOLEAN fnIoctlChangeXhifPage(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_CHANGE_XHIF_PAGE_IN*		pIn,
								uPNDEV_CHANGE_XHIF_PAGE_OUT*	pOut)
{
BOOLEAN				bResult		= FALSE;
uPNDEV_APP*			pApp		= NULL;
ePNDEV_XHIF_PAGE	ePage		= ePNDEV_XHIF_PAGE_INVALID;
UINT32				lSegmentAhb	= 0;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp		= (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;
		ePage		= pIn->ePage;
		lSegmentAhb	= pIn->lSegmentAhb;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CHANGE_XHIF_PAGE_OUT));
	}

	if	(pDevExtIn->pFnBoard_ChangePageXhif == NULL)
		// no support
	{
		// set ErrorString
		//	- changed by DLL to ReturnValue=ePNDEV_ERROR_SUPPORT
		fnBuildString(	PNDEV_ERROR_SUPPORT,
						NULL,
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// change dynamic UserPage
		bResult = (*((FN_BOARD_CHANGE_PAGE_XHIF) pDevExtIn->pFnBoard_ChangePageXhif))(	pDevExtIn,
																						pApp,
																						ePage,
																						lSegmentAhb,
																						_countof(pOut->sError),
																						&pOut->uHwDesc,
																						&pOut->lCtrRef,
																						pOut->sError);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service CopyData
//************************************************************************

BOOLEAN fnIoctlCopyData(DEVICE_EXTENSION*		pDevExtIn,
						uPNDEV_COPY_DATA_IN*	pIn,
						uPNDEV_COPY_DATA_OUT*	pOut)
{
BOOLEAN			bResult			= FALSE;
volatile UINT8* pHostDataBuf	= NULL;

	// preset OutputParameter
	//	- not necessary because of METHOD_DIRECT (driver OutputParameter (=uPNDEV_COPY_DATA_OUT) are already preset by DLL function fnDoEntryActions() )
	//	- user-specific data behind must not be overwritten!

	pHostDataBuf = ((UINT8*) pOut) + sizeof(uPNDEV_COPY_DATA_OUT);

	if	(!pDevExtIn->bCopyDataSupport)
		// Board don't support CopyData
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Service CopyData not supported!"),
						NULL,
						_countof(pOut->sError),
						pOut->sError);

	}
	else	// success
	{
		switch	(pIn->eHwRes)
				// HwRes
		{
			case ePNDEV_COPY_HW_RES_SDRAM:
			{
			BOOLEAN			bPagedAccess	= FALSE;
			volatile UINT8*	pAsicSdramSeg0	= NULL;

				// success
				bResult = TRUE;

				// first check if a paged access to AsicSdram required

				//------------------------------------------------------------------------
				// start using SdramPage 0
				fnBoard_StartSdramPage(	pDevExtIn,
										0,
										&bPagedAccess,
										&pAsicSdramSeg0);
				{
					if	(!bPagedAccess)
						// non-paged access to AsicSdram
					{
						// AsicSdramPtr = pAsicSdramSeg0

						// copy data direct
						fnBoard_CopyDataDirect(	pIn,
												pHostDataBuf,
												pAsicSdramSeg0);
					}
				}
				//------------------------------------------------------------------------
				// stop using SdramPage
				fnBoard_StopSdramPage(pDevExtIn);

				if	(bPagedAccess)
					// paged access to AsicSdram
				{
					// copy data paged (AsicSdram)
					fnBoard_CopyDataPaged_Sdram(pDevExtIn,
												pIn,
												pHostDataBuf);
				}

				break;
			}
			case ePNDEV_COPY_HW_RES_PNIP_IRTE:
			{
			volatile UINT8* pPnipIrte = NULL;

				switch	(pDevExtIn->eAsic)
						// AsicType
				{
					case ePNDEV_ASIC_ERTEC400:		{pPnipIrte = pDevExtIn->pIrte;	break;}
					case ePNDEV_ASIC_ERTEC200:		{pPnipIrte = pDevExtIn->pIrte;	break;}
					case ePNDEV_ASIC_SOC1:			{pPnipIrte = pDevExtIn->pIrte;	break;}
					case ePNDEV_ASIC_ERTEC200P:		{pPnipIrte = pDevExtIn->pPnIp;	break;}
					default:
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid AsicType!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

						break;
					}
				}

				if	(pPnipIrte != NULL)
					// success
				{
					// success
					bResult = TRUE;

					// copy data direct
					fnBoard_CopyDataDirect(	pIn,
											pHostDataBuf,
											pPnipIrte);
				}

				break;
			}
			default:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid HwRes!"),
								NULL,
								_countof(pOut->sError),
								pOut->sError);

				break;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service StartBootFw
//************************************************************************

BOOLEAN fnIoctlStartBootFw(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_START_BOOT_FW_IN*	pIn,
							uPNDEV_START_BOOT_FW_OUT*	pOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bResultBoard	= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_START_BOOT_FW_OUT));
	}

	// preset
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
			WRITE_REG_UINT32(	(pAsicSdramSeg0 + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
								0xaaffffee);
			WRITE_REG_UINT32(	(pAsicSdramSeg0 + PNDEV_ASIC_SDRAM__OFFSET_IDENT),
								0xaaffffee);
			WRITE_REG_UINT32(	(pAsicSdramSeg0 + PNDEV_ASIC_SDRAM__OFFSET_VERSION),
								0xaaffffee);
		}
		//------------------------------------------------------------------------
		// stop using SdramPage
		fnBoard_StopSdramPage(pDevExtIn);
	}

	// Note:
	//	- BootFw copied to start of SharedAsicSdram_Indirect0 by DLL

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
		UINT32 i = 0;

			if	(!fnBoardIrte_SignalWaitingArm9Ok(pDevExtIn))
				// signal 'WAITING' not set
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): SBL not ready for loading BootFw!"),
								NULL,
								_countof(pOut->sError),
								pOut->sError);
			}
			else
			{
				// set command 'GO' for Arm9 -> jump to BootFw
				fnBoardIrte_SetCmdGoArm9(	pDevExtIn,
											(ERTEC400_AHB_ADR__EMIF_SDRAM + PNCORE_SHARED_ASIC_SDRAM__OFFSET_INDIRECT0));

				// preset
				bErrorTimeout = TRUE;

				for	(i = 0; i < 100; i++)
					// MaxTime=100msec
				{
					if	(!fnBoardIrte_SignalRun2Arm9Ok(pDevExtIn))
						// signal 'RUN2' not set
					{
						// try again in 10msec

						if	(!fnDelayThread(pDevExtIn,
											10,
											_countof(pOut->sError),
											pOut->sError))
							// error at starting WaitTime of current thread
						{
							bErrorDelay	= TRUE;

							// leave loop
							break;
						}
					}
					else
					{
						// success
						bErrorTimeout = FALSE;

						// leave loop
						break;
					}
				}

				if	(!bErrorDelay)
					// no TimerError
				{
					if	(bErrorTimeout)
						// timeout
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Timeout, error at starting of BootFw!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);
					}
					else
					{
						// success
						bResultBoard = TRUE;
					}
				}
			}

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// success
			bResultBoard = TRUE;

			//Note:	Write BOOT_FW address and BIN_EXECUTE command to secified addresses in SDRAM
			// - SBL checks for BIN_EXECUTE command to be written to SDRAM_BIN_CMD adress
			// - when BIN_EXECUTE command is written, SBL jumps to address written to SDRAM_BIN_FW


			// write BOOT_FW address to SDRAM_BIN_FW in SDRAM
			WRITE_REG_UINT32(	(volatile UINT32*) (pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr + BOARD_EB400__SDRAM_BIN_FW),
								BOARD_EB400__SDRAM_BOOT_FW);

			// write BIN_EXECUTE command to SDRAM_BIN_EXECUTE -> jump to BOOT_FW
			WRITE_REG_UINT32(	(volatile UINT32*) (pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr + BOARD_EB400__SDRAM_BIN_CMD),
								BOARD_EB400__SDRAM_BIN_EXECUTE);

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
		UINT32 lTmp = 0;

			// Arm946 PBL/SBL

			// success
			bResultBoard = TRUE;

			// Note:
			//	- JmpBootFw was copied to start of SharedAsicSdram_Direct temporary
			//	- JmpBootFw is a section of 1kB with a jmp at offset 0

			//------------------------------------------------------------------------
			// [LBU: using dynamic DriverPage]
			//	- SDRAM (page UserFw)
			fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
											eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM,
											BOARD_EB200__SDRAM_PAGE_USER_FW,
											__FUNCTION__);
			{
			volatile UINT8* pEntryAfterPbl = NULL;

				pEntryAfterPbl = (volatile UINT8*) (pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr + BOARD_EB200__SDRAM_OFFSET_FW);

				// copy JmpBootFw to requested location at SDRAM (offset 0x48 at start of SDRAM)
				//	- JumpBootFw is very small although there is a specification of 1kB
				//	- don't use size=1kB because then first part of SharedAsicSdram_Boot is overwritten
				//		-> preset variables Alive/Ident/Version would be overwritten again
				RtlCopyMemory(	(void*) pEntryAfterPbl,
								(void*) pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.pPtr,
								512);
//								1024);

				lTmp = READ_REG_UINT32(pEntryAfterPbl);
			}
			//------------------------------------------------------------------------
			// [LBU: end of using dynamic DriverPage]
			fnBoardIrte_StopDriverPageLbu(pDevExtIn);

			//------------------------------------------------------------------------
			// [LBU: using dynamic DriverPage]
			//	- SDRAM (page Arm9-IF)
			fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
											eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM,
											BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED_TMP,
											__FUNCTION__);
			{
			volatile UINT32* pArm9BootSemaphore = NULL;

				pArm9BootSemaphore = (volatile UINT32*) (pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr + BOARD_EB200__SDRAM_OFFSET_SEMAPHORE);

				// start JmpBootFw -> jump to BootFw
				WRITE_REG_UINT32(	pArm9BootSemaphore,
									0x68799786);

				lTmp = READ_REG_UINT32(pArm9BootSemaphore);
			}
			//------------------------------------------------------------------------
			// [LBU: end of using dynamic DriverPage]
			fnBoardIrte_StopDriverPageLbu(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
		volatile UINT8* pSdram 					= NULL;
		volatile UINT8*	pSharedAsicSdram_Direct	= NULL;

			// MIPS PBL/SBL

			// Note:
			//	- JmpBootFw was copied to start of SharedAsicSdram_Direct temporary
			//	- JmpBootFw is a section of 1kB with a jmp \EEnstruction at offset 0

			pSdram = pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.pPtr;

			// success
			bResultBoard = TRUE;

			pSharedAsicSdram_Direct = pSdram + PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;

			// copy JmpBootFw to requested location at MIPS-TPA
			RtlCopyMemory(	(void*) (pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_CODE),
							(void*) pSharedAsicSdram_Direct,
							1024);

			// start JmpBootFw -> jump to BootFw
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_SEMAPHORE),
								0x68799786);

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					// no Arm926 -> no PBL/SBL

					// success
					bResultBoard = TRUE;

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					// Arm926 PBL

					//------------------------------------------------------------------------
					// [XHIF: using dynamic DriverPage]
					//	- ARM926-DTCM
					fnBoardPnip_StartDriverPageXhif(pDevExtIn,
													eBOARD_ERTEC200P_XHIF__DYN_PAGE_ARM926_TCM,
													0,
													__FUNCTION__);
					{
					volatile UINT8*	pTcmArm9				= NULL;
					volatile UINT8*	pSharedAsicSdram_Direct	= NULL;

						switch	(pDevExtIn->eBoard)
								// BoardType
						{
							case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
							{
								pTcmArm9				= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArm926Tcm.pPtr;
								pSharedAsicSdram_Direct	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.pPtr;

								break;
							}
							case ePNDEV_BOARD_EB200P:	// EB200P board
							{
								pTcmArm9				= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArm926Tcm.pPtr;
								pSharedAsicSdram_Direct = pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.pPtr;

								break;
							}
							default:
							{
								// set ErrorString
								fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid board!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);

								break;
							}
						}

						if	(	(pSharedAsicSdram_Direct	!= NULL)
							&&	(pTcmArm9					!= NULL))
							// success
						{
							// success
							bResultBoard = TRUE;

							// Note:
							//	- JmpBootFw was copied to start of SharedAsicSdram_Direct temporary
							//	- JmpBootFw is a section of 1kB with a jmp \EEnstruction at offset 0

							// PBL:
							//	- uses ARM926-DTCM block3 for destination address of user code and semaphore
							//	- converts ARM926-DTCM block3 to ITCM before jumping
							//		(but does not swap VectorTable -> VectorTable of BootRom is used further on, swap instruction would change it)
							//	- DTCM/ITCM must be reset if user firmware wants to rewrite ARM926-DTCM block3 (no write access to ITCM!)
							//		-> done by BootFw (register ERTEC200P_SCRB_REG__TCM926_MAP)

							// copy JmpBootFw to requested location at ARM926-DTCM
							RtlCopyMemory(	(void*) (pTcmArm9 + ERTEC200P_DTCM_OFFSET__CODE),
											(void*) pSharedAsicSdram_Direct,
											1024);

							// start JmpBootFw -> jump to BootFw
							WRITE_REG_UINT32(	(pTcmArm9 + ERTEC200P_DTCM_OFFSET__SEMAPHORE),
												0x68799786);
						}
					}
					//------------------------------------------------------------------------
					// [XHIF: end of using dynamic DriverPage]
					fnBoardPnip_StopDriverPageXhif(pDevExtIn);

					break;
				}
				default:
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid BoardDetail!"),
									NULL,
									_countof(pOut->sError),
									pOut->sError);

					break;
				}
			}

			break;
		}
		default:
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid BoardType!"),
							NULL,
							_countof(pOut->sError),
							pOut->sError);

			break;
		}
	}

	if	(bResultBoard)
		// success
	{
		// wait 50msec to ensure that startup of BootFw is really finished
		//	- PBL/SBL can only confirm request before jumping to BootFw
		//		-> wait necessary because startup of BootFw may initialized variables, no command before this is done!

		if	(fnDelayThread(	pDevExtIn,
							50,
							_countof(pOut->sError),
							pOut->sError))
			// starting WaitTime of current thread ok
		{
			// verify BootFw
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
					if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
														pAsicSdramSeg0,
														NULL,
														100,						// max. 100msec
														_countof(pOut->sError),
														pOut->sError))
						// checking SignOfLife of BootFw ok
					{
						if	(fnBoard_CheckVersionBootFw(pDevExtIn,
														pAsicSdramSeg0,
														_countof(pOut->sError),
														pOut->sError))
							// checking version of BootFw ok
						{
							// success
							bResult = TRUE;

							// BootFw running
							pDevExtIn->bBootFwRunning = TRUE;
						}
					}
				}
				//------------------------------------------------------------------------
				// stop using SdramPage
				fnBoard_StopSdramPage(pDevExtIn);
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service StartUserFw
//************************************************************************

BOOLEAN fnIoctlStartUserFw(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_START_USER_FW_IN*	pIn,
							uPNDEV_START_USER_FW_OUT*	pOut)
{
BOOLEAN						bResult							= FALSE;
uPNDEV_APP*					pApp							= NULL;
UINT32						lOffsetFwBuf					= 0;
UINT32						lIndexFw						= 0;
UINT32						lIndexFwMax						= 0;
BOOLEAN						bDebugMode						= FALSE;
BOOLEAN						bShmDebug						= FALSE;
UINT32						lTimeoutFwLoad_10msec			= 0;
UINT32						lTimeoutFwExe_10msec			= 0;
UINT32						lTimeoutFwStartupStd_10msec		= 0;
UINT32						lTimeoutFwStartupDebug_10msec	= 0;
UINT32						lTimeoutDebugger_10msec			= 0;
UINT32						lTimeWaitDebugger_10msec		= 0;
UINT32						lTimeStartupSignOfLife_10msec	= 0;
UINT32						lBatHostAddress					= 0;
UINT32						i								= 0;
BOOLEAN						bPagedAccess					= FALSE;
volatile UINT8*				pAsicSdramSeg0					= NULL;
uPNCORE_STARTUP_PAR_DRIVER	uStartupParDriver;
uPNDEV_HW_DESC_SDRAM		uAsicSdram_Org;

	RtlZeroMemory(	&uStartupParDriver,
					sizeof(uPNCORE_STARTUP_PAR_DRIVER));

	RtlZeroMemory(	&uAsicSdram_Org,
					sizeof(uPNDEV_HW_DESC_SDRAM));

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		{
			pApp											= (uPNDEV_APP*) pIn->uAppSys.hHandle;
			lOffsetFwBuf									= pIn->lOffsetFwBuf;
			lIndexFw										= pIn->lIndexFw;
			lIndexFwMax										= pIn->lIndexFwMax;
			bDebugMode										= pIn->bDebugMode;
			bShmDebug										= pIn->bShmDebug;
			lTimeoutFwLoad_10msec							= pIn->lTimeoutFwLoad_10msec;
			lTimeoutFwExe_10msec							= pIn->lTimeoutFwExe_10msec;
			lTimeoutFwStartupStd_10msec						= pIn->lTimeoutFwStartupStd_10msec;
			lTimeoutFwStartupDebug_10msec					= pIn->lTimeoutFwStartupDebug_10msec;
			lTimeoutDebugger_10msec							= pIn->lTimeoutDebugger_10msec;
			lTimeWaitDebugger_10msec						= pIn->lTimeWaitDebugger_10msec;
			lTimeStartupSignOfLife_10msec					= pIn->lTimeStartupSignOfLife_10msec;
			uStartupParDriver.lTimeUpdateSignOfLife_10msec	= pIn->lTimeUpdateSignOfLife_10msec;
			uStartupParDriver.bShmHaltOnStartup 			= pIn->bShmHaltOnStartup;
			lBatHostAddress									= pIn->lBatHostAddress;

			
			// copy PciMasterPar
			*(&uStartupParDriver.uPciMaster[0]) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[0]);
			*(&uStartupParDriver.uPciMaster[1]) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[1]);
			*(&uStartupParDriver.uPciMaster[2]) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[2]);

			// copy uSharedHostSdram
			uStartupParDriver.uSharedHostSdram.lPhyAdr 		= g_uMemSys.uHostMem[pDevExtIn->lUsesHostMemIndex].lPhyAdr;
			uStartupParDriver.uSharedHostSdram.lSize	 	= g_uMemSys.uHostMem[pDevExtIn->lUsesHostMemIndex].uHwResUserSpace.lSize;

			for	(i=0; i < PNCORE_DIM_ARRAY_USER_DATA; i++)
				// dim array
			{
				// copy FwUserData
				uStartupParDriver.lArrayUserData[i] = pIn->lArrayUserData[i];
			}

			*((uPNDEV_HW_DESC_SDRAM*) &uAsicSdram_Org) = *((uPNDEV_HW_DESC_SDRAM*) &pIn->uAsicSdram_Org);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_START_USER_FW_OUT));
	}

	if	(lTimeoutFwLoad_10msec == 0)
		// default value desired
	{
		// 60sec (=60.000msec=6000*10msec)
		lTimeoutFwLoad_10msec = 6000;
	}

	if	(lTimeoutFwExe_10msec == 0)
		// default value desired
	{
		// 10sec (=10.000msec=1000*10msec)
		lTimeoutFwExe_10msec = 1000;
	}

	if	(lTimeoutFwStartupStd_10msec == 0)
		// default value desired
	{
		// 30sec (=30.000msec=3000*10msec)
		lTimeoutFwStartupStd_10msec = 3000;
	}

	if	(lTimeoutFwStartupDebug_10msec == 0)
		// default value desired
	{
		// 300sec (=300.000msec=30000*10msec)
		lTimeoutFwStartupDebug_10msec = 30000;
	}

	if	(lTimeoutDebugger_10msec == 0)
		// default value desired
	{
		// 300sec (=300.000msec=30000*10msec)
		lTimeoutDebugger_10msec = 30000;
	}

	if	(lTimeWaitDebugger_10msec == 0)
		// default value desired
	{
		// 10sec (=10000msec=1000*10msec)
		lTimeWaitDebugger_10msec = 1000;
	}

	if	(lTimeStartupSignOfLife_10msec == 0)
		// default value desired
	{
		// 1sec
		lTimeStartupSignOfLife_10msec = 100;
	}

	if	(uStartupParDriver.lTimeUpdateSignOfLife_10msec == 0)
		// default value desired
	{
		// 5sec
		uStartupParDriver.lTimeUpdateSignOfLife_10msec = 500;
	}

	//------------------------------------------------------------------------
	// start using SdramPage 0
	fnBoard_StartSdramPage(	pDevExtIn,
							0,
							&bPagedAccess,
							&pAsicSdramSeg0);
	{
	BOOLEAN				bResultAckDebugger	= FALSE;
	uPNCORE_CONFIG_PAR*	pConfigPar			= NULL;

		pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);

		// SharedAsicSdram_Config is already preset by ResetDevice

		// preset
		bResultAckDebugger = TRUE;

		if	(bDebugMode)
			// DebugMode
		{
			if	(lIndexFw == 0)
				// first downloading of UserFw after reset
			{
				if	(!fnBoard_WaitForAckDebugger(	pDevExtIn,
													pConfigPar,
													lTimeoutDebugger_10msec,
													lTimeWaitDebugger_10msec,
													_countof(pOut->sError),
													pOut->sError))
					// error at waiting for ack of debugger
				{
					bResultAckDebugger = FALSE;
				}
			}
		}

		if	(bResultAckDebugger)
			// success
		{
			if	(fnBoard_DoElfCmdLoadBootFw(pDevExtIn,
											pAsicSdramSeg0,
											pConfigPar,
											lOffsetFwBuf,
											lIndexFw,
											lIndexFwMax,
											lTimeoutFwLoad_10msec,
											_countof(pOut->sError),
											pOut->sError))
				// doing ElfCmd 'Load' of BootFw ok
			{
			uPNCORE_STARTUP_PAR_DRIVER* pStartupParDriver = NULL;

				if	(lIndexFw == lIndexFwMax)
					// MainFw
				{
					pStartupParDriver = &uStartupParDriver;

					// copy BAT for 3.PciMaster
					pConfigPar->lBatHostAddress = lBatHostAddress;

					if	(pDevExtIn->eAsic == ePNDEV_ASIC_ERTEC400)
						// Ertec400
					{
						// set BridgeConfigReg
						pConfigPar->lOffsetBridgeConfigReg = PNCORE_ERTEC400_PCI_BRIDGE_REG__ADR;
					}
					else if	(pDevExtIn->eAsic == ePNDEV_ASIC_SOC1)
							// Soc1
					{
						// set BridgeConfigReg
						pConfigPar->lOffsetBridgeConfigReg = PNCORE_SOC1_PCI_BRIDGE_REG__ADR;
					}

					// copy RemanentData
					*((uPNCORE_REMANENT_DATA*) &uStartupParDriver.uRemanentData) = *((uPNCORE_REMANENT_DATA*) &pDevExtIn->uRemanentData);

					// set Boardtype


				}

				if	(fnBoard_DoElfCmdExecuteBootFw(	pDevExtIn,
													pAsicSdramSeg0,
													pConfigPar,
													(	bDebugMode 
													||	bShmDebug),
													lIndexFw,
													lIndexFwMax,
													lTimeoutFwExe_10msec,
													lTimeStartupSignOfLife_10msec,
													pStartupParDriver,
													_countof(pOut->sError),
													pOut->sError))
					// doing ElfCmd 'Execute' of BootFw ok
				{
				UINT32 lTimeoutFwStartup = 0;

					if	(	bDebugMode
						||	bShmDebug)
						// DebugMode or ShmDebug
					{
						lTimeoutFwStartup = lTimeoutFwStartupDebug_10msec;
					}
					else
					{
						lTimeoutFwStartup = lTimeoutFwStartupStd_10msec;
					}

					if	(lIndexFw < lIndexFwMax)
						// SetupFw
					{
						if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
															pAsicSdramSeg0,
															pConfigPar,
															(lTimeoutFwStartup * 10),
															_countof(pOut->sError),
															pOut->sError))
							// checking SignOfLife of BootFw ok
						{
							// success
							bResult = TRUE;
						}
					}
					else	// MainFw
					{
						if	(fnBoard_WaitForStartupParUserFw(	pDevExtIn,
																pConfigPar,
																lTimeoutFwStartup,
																_countof(pOut->sError),
																pOut->sError))
							// waiting for StartupPar of UserFw ok
						{
							// preset SdramPar with current values
							*((uPNDEV_HW_DESC_SDRAM*) &pOut->uAsicSdram_Updated) = *((uPNDEV_HW_DESC_SDRAM*) &uAsicSdram_Org);

							if	(fnBoard_UpdatePtrHwResUm_Sdram(&pConfigPar->uFw.uStartupPar,
																_countof(pOut->sError),
																&pOut->uAsicSdram_Updated,
																pOut->sError))
								// updating SdramPar ok
							{

								if	(!bShmDebug)
								{
								
									if	(fnBoard_WaitForReadyUserFw(pDevExtIn,
																	pConfigPar,
																	lTimeoutFwStartup,
																	_countof(pOut->sError),
																	pOut->sError))
										// waiting for Ready of UserFw ok
									{
										if	(!bDebugMode)
											// not DebugMode
										{
											// start WD
										}
	
										// UserFw running
										//	- this variable activates some specific code of SysThread
										pDevExtIn->bUserFwRunning = TRUE;
	
										// activate watchdog
										//	- Note: partner may check it immediately after setting Ready
										{
											// set SignOfLife of driver
											fnPnCore_SetSignOfLife(	pAsicSdramSeg0,
																	LSA_FALSE);			// called by driver
	
											// signal Ready of driver to Fw
											pConfigPar->uDriver.bReady = TRUE;
										}
	
										// UserFw started by this App
										pApp->bStartedUserFw = TRUE;
	
										// success
										bResult = TRUE;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//------------------------------------------------------------------------
	// stop using SdramPage
	fnBoard_StopSdramPage(pDevExtIn);

	if	(!bResult)
		// error
	{
		// Note:
		//	- if there is an error at downloading of UserFw then service CloseDevice is automatically called in context of service OpenDevice
		//		-> automatic BoardReset at last CloseDevice
		//		-> loaded UserFw will always be stopped
	}

	return(bResult);
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service StartUserFw
//************************************************************************

BOOLEAN fnIoctlIsUserFwReady(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_IS_USER_FW_READY_IN*		pIn,
								uPNDEV_IS_USER_FW_READY_OUT*	pOut)
{
BOOLEAN			bResult			= FALSE;
uPNDEV_APP*		pApp			= NULL;
BOOLEAN			bPagedAccess	= FALSE;
volatile UINT8*	pAsicSdramSeg0	= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		{
			pApp = (uPNDEV_APP*) pIn->uAppSys.hHandle;
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_IS_USER_FW_READY_OUT));
	}
	
	
	//------------------------------------------------------------------------
	// start using SdramPage 0
	fnBoard_StartSdramPage(	pDevExtIn,
							0,
							&bPagedAccess,
							&pAsicSdramSeg0);
	{
	uPNCORE_CONFIG_PAR*	pConfigPar = NULL;

		pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);
	
		if	(!pConfigPar->bCancel)
		{
			if	(pConfigPar->uFw.bReady)
				// waiting for Ready of UserFw ok
			{
				// UserFw running
				//	- this variable activates some specific code of SysThread
				pDevExtIn->bUserFwRunning = TRUE;
			
				// activate watchdog
				//	- Note: partner may check it immediately after setting Ready
				{
					// set SignOfLife of driver
					fnPnCore_SetSignOfLife(	pAsicSdramSeg0,
											LSA_FALSE);			// called by driver
			
					// signal Ready of driver to Fw
					pConfigPar->uDriver.bReady = TRUE;
				}
			
				// UserFw started by this App
				pApp->bStartedUserFw = TRUE;
				// Stop waiting for ready UserFw in service
				pOut->bIsUserFwReady = TRUE;
			}
			// success
			bResult = TRUE;
		}
	}
	//------------------------------------------------------------------------
	// stop using SdramPage
	fnBoard_StopSdramPage(pDevExtIn);

	return bResult;
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ChangeAppLock
//************************************************************************

BOOLEAN fnIoctlChangeAppLock(	DEVICE_EXTENSION*			pDevExtIn,
								uPNDEV_CHANGE_APP_LOCK_IN*	pIn,
								uPNDEV_CHANGE_APP_LOCK_OUT*	pOut)
{
BOOLEAN		bResult			= FALSE;
uPNDEV_APP*	pApp			= NULL;
BOOLEAN		bLockMultiApp	= FALSE;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp			= (uPNDEV_APP*) pIn->uAppSys.hHandle;
		bLockMultiApp	= pIn->bLockMultiApp;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CHANGE_APP_LOCK_OUT));
	}

	if	(bLockMultiApp)
		// lock MultiApp
	{
		if	(pDevExtIn->lCtrApp > 1)
			// other App exist
		{
			// set ErrorString
			fnBuildString(	NULL,
							_TEXT("Internal service ChangeAppLock requires a CloseDevice of all other applications!"),
							NULL,
							_countof(pOut->sError),
							pOut->sError);

			// set event 'ReqClose'
			{
				pDevExtIn->uEvent.bReqClose = TRUE;

				// check EventList
				//	- must be done here:
				//		SysThread does not know the application which should be excluded!
				fnCheckEventList(	pDevExtIn,
									pApp,		// App excluded
									FALSE);		// not called by SysThread
			}
		}
		else
		{
			// success
			bResult = TRUE;

			// lock MultiApp
			pDevExtIn->bMultiAppLocked = TRUE;
		}
	}
	else
	{
		// success
		bResult = TRUE;

		// unlock MultiApp
		pDevExtIn->bMultiAppLocked = FALSE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service NotifyDllState
//************************************************************************

BOOLEAN fnIoctlNotifyDllState(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_NOTIFY_DLL_STATE_IN*		pIn,
								uPNDEV_NOTIFY_DLL_STATE_OUT*	pOut)
{
BOOLEAN bResult	= FALSE;
BOOLEAN bReady	= FALSE;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		bReady = pIn->bReady;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_NOTIFY_DLL_STATE_OUT));
	}

	// store DllState of first OpenDevice
	pDevExtIn->bDllReadyFirstOpenDevice = bReady;

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  MapMemory to corresponding Application
//************************************************************************

BOOLEAN fnIoctlMapMemory(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_MAP_MEMORY_IN*	pIn,
							uPNDEV_MAP_MEMORY_OUT*	pOut)
{
BOOLEAN bResult		= FALSE;
BOOLEAN	bResultMap	= TRUE;
UINT32	i			= 0;
uPNDEV_HW_RES_SINGLE_KM_TMP	uBarKm[6];
uPNDEV_HW_RES_SINGLE_KM_TMP	uSdramKm;
uPNDEV_HW_RES_SINGLE_KM_TMP	uHostMemKm;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// save input parameter
		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			uBarKm[i] = pIn->uBarKm[i];
		}

		uSdramKm 	= pIn->uSdramKm;
		uHostMemKm 	= pIn->uHostMemKm;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_MAP_MEMORY_OUT));
	}

	// map Bars
	{
		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
		UINT32 lSizeBar = uBarKm[i].lSize;

			if	(!fnMapMemVirtualToUser(pDevExtIn,
										((const void*) uBarKm[i].uVirtualAdr.pPtr),
										lSizeBar,
										_countof(pOut->sError),
										&pOut->uBarUm[i],
										pOut->sError))
				// error at mapping BARx to UserMode
			{
				bResultMap = FALSE;

				// leave loop
				break;
			}
		}
	}

	if	(bResultMap)
		// success
	{
		// map Sdram
		{
			if	(!fnMapMemVirtualToUser(pDevExtIn,
										(const void*) (uSdramKm.uVirtualAdr.pPtr),
										uSdramKm.lSize,
										_countof(pOut->sError),
										&pOut->uSdramUm,
										pOut->sError))
				// error at mapping SharedAsicSdram_Direct to UserMode
			{
				// error
				bResultMap = FALSE;
			}
		}

		if	(bResultMap)
			// success
		{
			// map SharedHostMemory
			{
				if	(uHostMemKm.uVirtualAdr.pPtr != NULL)
					// HostMem required
				{
					// map memory to userspace
					if	(!fnMapMemVirtualToUser(pDevExtIn,
												((const void*) (uHostMemKm.uVirtualAdr.pPtr)),
												uHostMemKm.lSize,
												_countof(pOut->sError),
												&pOut->uHostMemUm,
												pOut->sError))
						// mapping failed
					{
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Can not provide contiguous shared host memory for device: Failed mapping to UserSpace"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

					}
					else
					{
						// success
						bResult = TRUE;
					}
				}
				else
				{
					// success
					bResult = TRUE;
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  MapMemory to corresponding Application
//************************************************************************
BOOLEAN	fnIoctlSetHwResUm(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_OPEN_DEVICE_OUT*	pIn,
							uPNDEV_OPEN_DEVICE_OUT*	pOut)
{
BOOLEAN 	bResult = FALSE;
uPNDEV_APP*	pApp	= NULL;

	// copy InputBuffer to OutputBuffer
	memcpy_s(	pOut,
				sizeof(uPNDEV_OPEN_DEVICE_OUT),
				pIn,
				sizeof(uPNDEV_OPEN_DEVICE_OUT));

	pApp = (uPNDEV_APP*) pOut->uInternal.uAppSys.hHandle;

    if	(fnBoard_SetPtrHwResUm(	pDevExtIn,
    							_countof(pOut->sError),
    							&pOut->uInternal,
    							&pOut->uHwRes,
    							&pOut->uInternal.uIcu.pPtr,		// ptr to ICU
    							pApp,
    							pOut->sError))
    	// setting UserModePtr to HwRes ok
    {
    	// success
    	bResult = TRUE;
    }

	if	(!bResult)
		// error
	{
	_TCHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

		if	(!fnCloseDevice(pDevExtIn,
							pApp,
							_countof(sError),
							sError))
			// error at closing device
		{
			// set trace by returned ErrorString
			fnSetTraceFctError(	pDevExtIn,
								sError);
		}
	}

	return(bResult);

}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check IoctlPar
//************************************************************************
ePNDEV_IOCTL fnCheckIoctlPar(	const UINT32		lIoctlIn,
								const UINT32		lSizeInputBufIn,
								const UINT32		lSizeOutputBufIn)
{
ePNDEV_IOCTL	eResult				= ePNDEV_IOCTL_INVALID;
BOOLEAN			bCompareEqual		= FALSE;
UINT32			lMinSizeInputBuf	= 0;
UINT32			lMinSizeOutputBuf	= 0;
char			sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
	// preset
	bCompareEqual = TRUE;

	switch	(lIoctlIn)
			// IoControlCode
	{
		case IOCTL_OPEN_DEVICE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_OPEN_DEVICE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_OPEN_DEVICE_OUT);
			break;
		}
		case IOCTL_CLOSE_DEVICE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CLOSE_DEVICE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CLOSE_DEVICE_OUT);
			break;
		}
		case IOCTL_RESET_DEVICE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_RESET_DEVICE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_RESET_DEVICE_OUT);
			break;
		}
		case IOCTL_GET_EVENT_LIST:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_EVENT_LIST_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_EVENT_LIST_OUT);
			break;
		}
		case IOCTL_SET_EVENT_LIST:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_SET_EVENT_LIST_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_SET_EVENT_LIST_OUT);
			break;
		}
		case IOCTL_CHANGE_INT:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CHANGE_INT_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CHANGE_INT_OUT);
			break;
		}
		case IOCTL_GET_DEVICE_STATE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_DEVICE_STATE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_DEVICE_STATE_OUT);
			break;
		}
		case IOCTL_PROGRAM_FLASH:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_PROGRAM_FLASH_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_PROGRAM_FLASH_OUT);
			break;
		}
		case IOCTL_CHANGE_XHIF_PAGE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CHANGE_XHIF_PAGE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CHANGE_XHIF_PAGE_OUT);
			break;
		}
		case IOCTL_COPY_DATA:
		{
			// size of output buffer may be larger
			bCompareEqual = FALSE;

			lMinSizeInputBuf	= sizeof(uPNDEV_COPY_DATA_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_COPY_DATA_OUT);
			break;
		}
		case IOCTL_START_BOOT_FW:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_START_BOOT_FW_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_START_BOOT_FW_OUT);
			break;
		}
		case IOCTL_START_USER_FW:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_START_USER_FW_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_START_USER_FW_OUT);
			break;
		}
		case IOCTL_CHANGE_APP_LOCK:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CHANGE_APP_LOCK_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CHANGE_APP_LOCK_OUT);
			break;
		}
		case IOCTL_NOTIFY_DLL_STATE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_NOTIFY_DLL_STATE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_NOTIFY_DLL_STATE_OUT);
			break;
		}
		case IOCTL_GET_DEVICE_INFO_SYS:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_DEVICE_INFO_SYS_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_DEVICE_INFO_SYS_OUT);
			break;
		}
		case IOCTL_GET_DEVICE_HANDLE_SYS:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_OUT);
			break;
		}
		case IOCTL_ALLOC_EVENT_ID:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_ALLOC_EVENT_ID_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_ALLOC_EVENT_ID_OUT);
			break;
		}
		case IOCTL_FREE_EVENT_ID:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_FREE_EVENT_ID_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_FREE_EVENT_ID_OUT);
			break;
		}
        case IOCTL_DO_NETPROFI:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_NP_DO_NETPROFI_REQ_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_NP_DO_NETPROFI_REQ_OUT);
			break;
		}
        case IOCTL_MAP_MEMORY:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_MAP_MEMORY_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_MAP_MEMORY_OUT);
			break;
		}
        case IOCTL_SET_HW_RES_UM:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_OPEN_DEVICE_OUT);
			lMinSizeOutputBuf	= sizeof(uPNDEV_OPEN_DEVICE_OUT);
			break;
		}
        case IOCTL_IS_USER_FW_READY:
        {
			lMinSizeInputBuf	= sizeof(uPNDEV_IS_USER_FW_READY_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_IS_USER_FW_READY_OUT);
			break;
        }
		default:
		{
			break;
		}
	}

	if	(lMinSizeInputBuf == 0)
		// error
	{
		// return
		eResult = ePNDEV_IOCTL_ERROR_REQ;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ########## Invalid IOCTL request!");

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else
	{
	BOOLEAN bResultSize = FALSE;

		if	(bCompareEqual)
			// compare for equal size
		{
			if	(	(lSizeInputBufIn	== lMinSizeInputBuf)
				&&	(lSizeOutputBufIn	== lMinSizeOutputBuf))
			{
				// success
				bResultSize = TRUE;
			}
		}
		else
		{
			if	(	(lSizeInputBufIn	== lMinSizeInputBuf)
				&&	(lSizeOutputBufIn	>= lMinSizeOutputBuf))
			{
				// success
				bResultSize = TRUE;
			}
		}
	
		if	(!bResultSize)
			// error
		{
		    // return
			eResult = ePNDEV_IOCTL_ERROR_BUF_SIZE;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[x,x,x]: ########## Invalid size of IOCTL input or output buffer!");

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,						// logging
						sTraceDetail);
			
            

		}
		else
		{
			// success
			eResult = ePNDEV_IOCTL_OK;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  close device
//************************************************************************

BOOLEAN fnCloseDevice(				DEVICE_EXTENSION*	pDevExtIn,
					  PFD_MEM_ATTR	uPNDEV_APP*			pAppIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN	bResultXhif	= FALSE;

	if	(pDevExtIn->pMasterApp == pAppIn)
		// MasterApp
	{
		// Note:
		//	- interrupts are only unmasked at MasterApp

		// disable interrupts at board globally
		fnBoard_ChangeIntMaskGlobal(pDevExtIn,
									FALSE);
	}

	// preset
	bResultXhif = TRUE;

	// release XHIF page
	{
		if	(pDevExtIn->pFnBoard_ChangePageXhif != NULL)
			// dynamic UserPage exist
		{
			if	(!(*((FN_BOARD_CHANGE_PAGE_XHIF) pDevExtIn->pFnBoard_ChangePageXhif))(	pDevExtIn,
																						pAppIn,
																						ePNDEV_XHIF_PAGE_INVALID,	// release
																						0,
																						lDimErrorIn,
																						NULL,
																						NULL,
																						sErrorOut))
				// error at releasing dynamic UserPage
				//	- page can only be released by an App which allocated it
			{
				bResultXhif = FALSE;
			}
		}
	}

	if	(bResultXhif)
		// success
	{
	BOOLEAN bResultRes = TRUE;

		if	(pDevExtIn->lCtrApp == 1)
			// last CloseDevice
		{
			if (fnDisableBusMasterConfigSpace(	pDevExtIn,
												lDimErrorIn,
												sErrorOut))
				// success
			{
				if	(!fnShutDownThreadResSys(	pDevExtIn,
												lDimErrorIn,
												sErrorOut))
					// error at shutting down resources of periodic SysThread
				{
					bResultRes = FALSE;
				}
			}
			else
			{
				bResultRes = FALSE;
			}
		}

		if	(bResultRes)
			// success
		{
			if	(pAppIn == NULL)
				// AppObject not exist
			{
				// success
				bResult = TRUE;
			}
			else
			{
			BOOLEAN bResultBlk = TRUE;

				// update AppList
				{
					//------------------------------------------------------------------------
					// enter CriticalSection
					fnEnterCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
					{
						if	(!fnBlkAtList(	&pDevExtIn->uListApp,
											((uLIST_HEADER*) pAppIn)))
							// AppObject not stored at AppList
						{
							// error
							bResultBlk = FALSE;

							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): AppObject not stored at AppList!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
						}
						else
						{
							// remove AppObject from AppList
							fnRemoveBlkFromList(&pDevExtIn->uListApp,
												((uLIST_HEADER*) pAppIn));

							pDevExtIn->lCtrApp--;
						}
					}
					//------------------------------------------------------------------------
					// leave CriticalSection
					fnLeaveCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
				}

				if	(bResultBlk)
					// success
				{
					// Note:
					//	- before releasing any resources of AppObject it must be signed as invalid

					// release UserModePtr to HwRes
					{
					UINT32 i = 0;

						// Note:
						//	- caller must be in the context of the original process before calling MmUnmapLockedPages
						//	- this is because the unmapping operation occurs in the context of the calling process, and if the context is incorrect
						//	  the unmapping operation could delete the address range of a random process
						//	- solution:
						//		DLL provides handle to AppObject

						for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
						{
							// release memory mapped to UserMode
							fnReleaseMemUser(	pDevExtIn,
												&pAppIn->uHwResUm.uBar[i]);
						}

						// release memory mapped to UserMode
						fnReleaseMemUser(	pDevExtIn,
											&pAppIn->uHwResUm.uAsicSdramDirect);

                        // undo shared memory mapping
						fnReleaseSharedHostMemUM (pDevExtIn);
					}

					if	(fnShutDownThreadResDll(pDevExtIn,
												pAppIn,
												lDimErrorIn,
												sErrorOut))
						// shutting down resources of DllThread ok
					{
						// free AppObject
						fnFreeMemNonPaged(	pAppIn,
											TAG('O','p','e','n'));

						if	(pDevExtIn->lCtrApp != 0)
							// not last CloseDevice
						{
							// success
							bResult = TRUE;
						}
						else
						{
							if	((*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
																					ePNDEV_RESET_ACTION_REINIT_HW,
																					lDimErrorIn,
																					sErrorOut))
								// resetting board ok
							{
								// success
								bResult = TRUE;

								// BootFw/UserFw stopped if running
								pDevExtIn->bBootFwRunning	= FALSE;
								pDevExtIn->bUserFwRunning	= FALSE;
							}
						}

						if	(bResult)
							// success
						{
							if	(pDevExtIn->pMasterApp == pAppIn)
								// MasterApp
							{
								// release MasterApp
								pDevExtIn->pMasterApp = NULL;
							}
						}
					}
				}
			}
		}
	}

	return(bResult);
}
