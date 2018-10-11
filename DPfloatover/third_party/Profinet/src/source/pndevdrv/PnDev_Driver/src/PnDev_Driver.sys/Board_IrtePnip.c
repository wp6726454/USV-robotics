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
/*  F i l e               &F: Board_IrtePnip.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with IRTE or PNIP
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
	#include "Board_IrtePnip.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_Reset)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_ResetHw)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_InitHw1)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_InitHw2)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_VerifyHwAfterReset)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_VersionOk)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_CheckIcuMode)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_GetBoardId)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_GetBoardRevision)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_GetBitStreamId)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_GetBitStreamVersion)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_ChangeReset)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_CheckResetActive)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_CheckPllLocked)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_CheckIntAdminReg)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_CheckAccessTimeout)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_ChangeIntMaskGlobal)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_ChangeBoardDemoInt)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_ChangeXhifConfig)
//	#pragma alloc_text (PAGE, fnBoardIrtePnip_CheckXhifA20)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoardIrtePnip_Reset(	DEVICE_EXTENSION*			pDevExtIn,
								const ePNDEV_RESET_ACTION	eActionIn,
								const UINT32				lDimErrorIn,
								_TCHAR*						sErrorOut)
{
BOOLEAN bResult			= FALSE;
BOOLEAN bResultAction	= FALSE;
BOOLEAN	bStopDefaultFw	= FALSE;
BOOLEAN	bResetHw		= FALSE;
BOOLEAN	bInitHw			= FALSE;

	// preset
	bResultAction = TRUE;

	switch	(eActionIn)
			// action
	{
		case ePNDEV_RESET_ACTION_ALLOC_HW:
		{
			// Note:
			//	- EntryBoardState = PowerOnState
			//	- ExitBoardState  = ReadyForUse by driver
			//	- an existing DefaultFw must be stopped

			bInitHw = TRUE;

			break;
		}
		case ePNDEV_RESET_ACTION_RELEASE_HW:
		{
			// Note:
			//	- ExitBoardState = PowerOnState
			//	- an existing DefaultFw must be started

			bStopDefaultFw	= FALSE;
			bResetHw		= TRUE;

			break;
		}
		case ePNDEV_RESET_ACTION_REINIT_HW:
		{
			// Note:
			//	- ExitBoardState = ReadyForUse by driver
			//	- an existing DefaultFw must be stopped

			bStopDefaultFw	= TRUE;
			bResetHw		= TRUE;
			bInitHw			= TRUE;

			break;
		}
		case ePNDEV_RESET_ACTION_USE_HW:
		{
			// Note:
			//	- HwSettings must not be changed
			//	- HwInit is done by another DriverInstance

			break;
		}
		default:
		{
			// error
			bResultAction = FALSE;

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid ResetAction!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	if	(bResultAction)
		// success
	{
		if	(	(!bStopDefaultFw)
			&&	(!bResetHw)
			&&	(!bInitHw))
			// nothing to do
		{
			// success
			bResult = TRUE;
		} 
		else
		{
			// disable interrupts at board globally
			fnBoard_ChangeIntMaskGlobal(pDevExtIn,
										FALSE);

			if	(fnBoardIrtePnip_InitHw1(	pDevExtIn,
											bStopDefaultFw,
											lDimErrorIn,
											sErrorOut))
				// initializing hardware (phase 1) ok
			{
			BOOLEAN bResultHwReset = FALSE;

				if	(!bResetHw)
					// don't reset Hw
				{
					// success
					bResultHwReset = TRUE;
				}
				else
				{
					// execute HwReset (bResetRunning=TRUE)
					bResultHwReset = fnBoardIrtePnip_ResetHw(	pDevExtIn,
																lDimErrorIn,
																sErrorOut);
				}

				if	(bResultHwReset)
					// success
				{
					if	(!bInitHw)
						// don't initialize Hw
					{
						// success
						bResult = TRUE;
					}
					else
					{
						if	(fnBoardIrtePnip_InitHw2(	pDevExtIn,
														bResetHw,
														lDimErrorIn,
														sErrorOut))
							// initializing hardware (phase 2) ok
						{
							if	(fnBoardIrtePnip_VerifyHwAfterReset(pDevExtIn,
																	lDimErrorIn,
																	sErrorOut))
								// verifying hardware after HwReset ok
							{
							BOOLEAN			bPagedAccess	= FALSE;
							volatile UINT8*	pAsicSdramSeg0	= NULL;

								// success
								bResult = TRUE;

								//------------------------------------------------------------------------
								// start using SdramPage 0
								fnBoard_StartSdramPage(	pDevExtIn,
														0,
														&bPagedAccess,
														&pAsicSdramSeg0);
								{
									// preset SharedAsicSdram_Config
									RtlZeroMemory(	(void*) (pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG),
													PNCORE_SHARED_ASIC_SDRAM__SIZE_CONFIG);
								}
								//------------------------------------------------------------------------
								// stop using SdramPage
								fnBoard_StopSdramPage(pDevExtIn);

								// show BitStream version and IRTE/PNIP version
								//	- if using PCIe-XHIF: must be here because pPnip is not valid before
								{
								char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

									if	(pDevExtIn->pIrte != NULL)
										// board with IRTE
									{
									UINT32 lIrteVersion = 0;

										lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte	+ IRTE_REG__IP_VERSION);

										// set TraceDetail
										RtlStringCchPrintfA(sTraceDetail,
															_countof(sTraceDetail),
															"PnDev[%s]: RESET done, BitStream=0x%x.0x%x, IRTE=0x%08x, Board=0x%08x",
															pDevExtIn->sPciLocShortAscii,
															pDevExtIn->lBitStreamId,
															pDevExtIn->lBitStreamVersion,
															lIrteVersion,
															pDevExtIn->lBoardRevision);
					
										// set trace
										fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
													ePNDEV_TRACE_CONTEXT_BASIC,
													TRUE,						// logging
													sTraceDetail);
									}
									else if	(pDevExtIn->pPnIp != NULL)
											// board with PNIP
									{
									UINT32	lPnIpVersion		= 0;
									UINT32	lPnIpDevelopment	= 0;

										lPnIpVersion		= READ_REG_UINT32(pDevExtIn->pPnIp	+ PNIP_REG__IP_VERSION);
										lPnIpDevelopment	= READ_REG_UINT32(pDevExtIn->pPnIp	+ PNIP_REG__IP_DEVELOPMENT);

										// set TraceDetail
										RtlStringCchPrintfA(sTraceDetail,
															_countof(sTraceDetail),
															"PnDev[%s]: RESET done, BitStream=0x%x.0x%x, PNIP=0x%08x, PNIP=0x%08x (RB%u), Board=0x%08x",
															pDevExtIn->sPciLocShortAscii,
															pDevExtIn->lBitStreamId,
															pDevExtIn->lBitStreamVersion,
															lPnIpVersion,
															lPnIpDevelopment,
															(lPnIpDevelopment & 0x000007ff),
															pDevExtIn->lBoardRevision);
					
										// set trace
										fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
													ePNDEV_TRACE_CONTEXT_BASIC,
													TRUE,						// logging
													sTraceDetail);
									}
								}
							}
						}

						// enable interrupts at board globally
						//	- do it only if Hw is initialized, otherwise don't change ResetState
						fnBoard_ChangeIntMaskGlobal(pDevExtIn,
													TRUE);
					}

					// reset finished
					pDevExtIn->bResetRunning = FALSE;
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  execute HwReset
//************************************************************************

BOOLEAN fnBoardIrtePnip_ResetHw(DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult				= FALSE;
BOOLEAN	bErrorBoardStatus	= FALSE;
BOOLEAN	bErrorTimeLoop		= FALSE;

	// reset running
	pDevExtIn->bResetRunning = TRUE;
	
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			//------------------------------------------------------------------------
			{
				// reset running
				pDevExtIn->bResetRunning = TRUE;
		
				// start reset
				fnBoardIrtePnip_ChangeReset(pDevExtIn,
											TRUE);
		
				if	(!fnBoardIrtePnip_CheckResetActive(pDevExtIn))
					// reset not active
				{
					bErrorBoardStatus = TRUE;
				}
				else
				{
					if	(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)
						// FPGA1-Ertec200P board
					{
					UINT32 lValueConfigPins = 0;
		
						lValueConfigPins = fnBoardPnip_GetValueConfigPins(pDevExtIn);
		
						// set Asic-ConfigPins
						//	- important for informing Asic about desired interface (e.g. XHIF)
						//	- latched at stop of reset
						WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONFIG),
											lValueConfigPins);
					}
		
					// hardware synchronization < 1usec
		
					if	(!fnTimeLoop(	pDevExtIn,
										100,
										FALSE))		// don't show details
						// error at waiting 100usec
						//	- DIRQL!
					{
						bErrorTimeLoop = TRUE;
					}
		
					// stop reset
					fnBoardIrtePnip_ChangeReset(pDevExtIn,
												FALSE);
		
					if	(!fnTimeLoop(	pDevExtIn,
										100,
										FALSE))		// don't show details
						// error at waiting 100usec
						//	- DIRQL!
					{
						bErrorTimeLoop = TRUE;
					}
		
					if	(fnBoardIrtePnip_CheckResetActive(pDevExtIn))
						// reset active
					{
						bErrorBoardStatus = TRUE;
					}
				}
			}
			//------------------------------------------------------------------------
		
			if	(bErrorBoardStatus)
				// error
			{
				// debug purpose
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
									0xaffeaffe);
		
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid state of ResetStatus!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else if	(bErrorTimeLoop)
					// error
			{
				// debug purpose
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
									0xaffeaffe);
		
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Error at fnTimeLoop()!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
		
				// wait 10msec
				//	- Ertec200P needs 1,268msec for startup (initialization of RAMs,...)
				//	- if there are accesses before this startup then EB200P FPGA creates an AccessTimeout -> reading of 0xffffffff until board is reset
		
				if	(fnDelayThread(	pDevExtIn,
									10,
									lDimErrorIn,
									sErrorOut))
					// starting WaitTime of current thread ok
				{
				}
			}

            break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:		// FPGA1-Hera board
		{
		UINT32	lArrayConfigSpace[PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)];	
			
			if	(pDevExtIn->bFlrSupport)
				// FLR is supported
			{
				if	(fnPrepareConfigSpaceHwReset(	pDevExtIn,
													lDimErrorIn,
													lArrayConfigSpace,
													sErrorOut))
					// preparing ConfigSpace for HwReset ok
				{
					if	(fnCheckTransactionPending(	pDevExtIn,
													lDimErrorIn,
													sErrorOut))
						// no TransactionPending
					{
						
						if	(fnSetFlrAtConfigSpace(	pDevExtIn,
													lDimErrorIn,
													sErrorOut))
							// setting of FLR successful
						{
							// wait 100msec				
							if	(fnDelayThread(	pDevExtIn,
												100,
												lDimErrorIn,
												sErrorOut))
								// starting WaitTime of current thread ok
							{
								
								if	(fnCheckVendorDeviceIdAtConfigSpace(pDevExtIn,
																		0x110A,
																		0x4090,
																		lDimErrorIn,
																		sErrorOut))
									// read Vendor/-DeviceId correct after reset
								{
									// restore Configspace
									if	(fnRestoreConfigSpaceHwReset(	pDevExtIn,
																		lArrayConfigSpace,
																		lDimErrorIn,
																		sErrorOut))
										// restoring ConfigSpace after HwReset ok
									{
										// set GPIO_OUT_CLEAR_1 : GPIO63 auf 0
										WRITE_REG_UINT32((	pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_GPIO + 0x2C),
															0x80000000);
										
										// set GPIO_IOCTRL_1 : GPIO63 to output
										WRITE_REG_UINT32((	pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_GPIO + 0x20),
															0x7FFFFFFF);
										
										// set GPIO_OUT_SET_1 : GPIO63 auf 0
										WRITE_REG_UINT32((	pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_GPIO + 0x28),
															0x00000000);
										
										// set GPIO_OUT_SET_1 : GPIO63 auf 1
										WRITE_REG_UINT32((	pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_GPIO + 0x28),
															0x80000000);
										
										// wait 100msec
										if	(fnDelayThread(	pDevExtIn,
															100,
															lDimErrorIn,
															sErrorOut))
											// starting WaitTime of current thread ok
										{
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
			else
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
							    _TEXT("(): FLR isn't supported by this Hera-FPGA1 board!"),
							    NULL,
							    lDimErrorIn,
							    sErrorOut);
			}
						
			break;
		}
		default:
		{
		    // set ErrorString
		    fnBuildString(	_TEXT(__FUNCTION__),
						    _TEXT("(): Invalid board!"),
						    NULL,
						    lDimErrorIn,
						    sErrorOut);

			break;
		}
	}
	
	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize hardware (phase 1)
//************************************************************************

BOOLEAN fnBoardIrtePnip_InitHw1(DEVICE_EXTENSION*	pDevExtIn,
								const BOOLEAN		bStopDefaultFwIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			if	(pDevExtIn->pAdminReg == NULL)
				// invalid ptr
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid ptr to AdminReg!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			} 
			else
			{
				if	(!fnBoardIrtePnip_CheckPllLocked(pDevExtIn))
					// PLL not locked
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Clock for TopLevel and PNIP/IRTE not present!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;
		
					// clear BoardDemoInt
					//	- AdminReg are not changed by reset
					fnBoardIrtePnip_ChangeBoardDemoInt(	pDevExtIn,
														FALSE);
		
					if	(pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
						// board with Ertec200P/PCIe-XHIF
					{
						// XHIF DataBusWidth=32Bit required
						//	- FPGA1-Ertec200P board:	can be set by register BOARD_FPGA1_REG__CONTROL
						//	- EB200P board:				hardwired
		
						switch	(pDevExtIn->eBoard)
								// BoardType
						{
							case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
							{
								// initialize AdapterHw between PCIe and XHIF
								//	- XHIF DataBusWidth = 32Bit
								WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
													(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) | BOARD_FPGA1_CONTROL_XHIF_BUS_SIZE32));
		
								break;
							}
							case ePNDEV_BOARD_EB200P:	// EB200P board
							{
								// Note:
								//	- if a PCIe-AccessTimeout occurred then there must be a BoardReset and AccessTimeoutInt must be acked
								//	- otherwise accesses to Asic are terminated by 0xffffffff
		
								// ack all possible interrupts
								WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_STATUS_ACK),
													MAXUINT32);
		
								break;
							}
							default:
							{
								// set ErrorString
								fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid board!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
		
								break;
							}
						}
		
						// activate 8*1MB
						{
							// signal XHIF_XCS_R_A20:
							//	- external multiplexer must be set to CS-R before internal multiplexer is set to CS-R by AsicReset
							//	  (otherwise accesses to upper 1MB of a page would address CS-R of Asic -> overwriting of PageCfgReg!
							//	   new concept: no user accesses during reset phase -> no problem any longer)
							//	- internal multiplexer:	set by SCRB.XHIF_MODE or AsicReset
							//	- external multiplexer:	set by BAR0.BoardControl
		
							fnBoardIrtePnip_ChangeXhifConfig(	pDevExtIn,
																FALSE);		// CS-R
						}
					}
				}
			}

            break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:		// FPGA1-Hera board
		{
			// nothing to do 
			bResult = TRUE;
			
			break;
		}
		default:
		{
		    // set ErrorString
		    fnBuildString(	_TEXT(__FUNCTION__),
						    _TEXT("(): Invalid board!"),
						    NULL,
						    lDimErrorIn,
						    sErrorOut);

			break;
		}
	}
	
	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize hardware (phase 2)
//************************************************************************

BOOLEAN fnBoardIrtePnip_InitHw2(DEVICE_EXTENSION*	pDevExtIn,
								const BOOLEAN		bResetHwIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
		BOOLEAN bResultTmp = FALSE;

			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					// no Arm926 -> no PBL/SBL -> no wait for startup of Arm926 PBL/SBL necessary

					if	(fnBoardIrtePnip_VersionOk(	pDevExtIn,
													lDimErrorIn,
													sErrorOut))
						// version of board is ok
						//	- Note: startup of PBL/SBL must be finished for ensuring access to VersionPatterns which may be updated by SBL!
					{
						// success
						bResultTmp = TRUE;

						// initialize IrqLogic
						fnBoardPnip_InitIrqLogic(pDevExtIn);
					}

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					// after reset:
					//	- 8*1MB
					//	- PageCfgReg only accessible by XHIF_XCS_R 
					//		-> BAR1 (XHIF_XCS_R_A20 = XCS_R)
					{
						pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg = pDevExtIn->uHwResKm.uBar[1].pVirtualAdr;
					}

					// wait for startup of Arm926 PBL
					//	- must be finished before any further driver action!

					if	(fnBoardPnip_CheckStartupPbl(	pDevExtIn,
														bResetHwIn,
														lDimErrorIn,
														sErrorOut))
						// startup of Arm926 PBL ok
					{
						if	(fnBoardIrtePnip_VersionOk(	pDevExtIn,
														lDimErrorIn,
														sErrorOut))
							// version of board is ok
							//	- Note: startup of PBL/SBL must be finished for ensuring access to VersionPatterns which may be updated by SBL!
						{
							if	(fnBoardPnip_SetStaticPagesXhif(pDevExtIn,
																lDimErrorIn,
																sErrorOut))
								// setting static pages ok
							{
								// success
								bResultTmp = TRUE;

								// restore dynamic UserPage
								fnBoardPnip_AllocUserPageXhif(	pDevExtIn,
																NULL,
																pDevExtIn->uUserPageXhif.ePage,
																pDevExtIn->uUserPageXhif.lSegmentAhb,
																NULL);

								// initialize IrqLogic
								fnBoardPnip_InitIrqLogic(pDevExtIn);

								// activate AHB BurstBreaker
								//	- limit BurstLen of ARM926/KRISC32 to 8 for ensuring that a XHIF read access from the host will be executed
								//	- if the number of consecutive address phases of ARM926/KRISC32 without an IDLE cycle is too high then the Burst Breaker will insert an IDLE cycle
								WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__AHB_BURST_BREAKER),
													0x00080808);
							}
						}
					}

					break;
				}
				default:
				{
					break;
				}
			}

			if	(bResultTmp)
				// success
			{
				// no SBL which initializes EMC controller

				if	(fnBoardPnip_InitEmcController(	pDevExtIn,
													lDimErrorIn,
													sErrorOut))
					// initializing EMC controller ok
				{
					// success
					bResult = TRUE;

					if	(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)
						// FPGA1-Ertec200P board
					{
						// reset PNIP-RAMs which are not initialized by hardware
						fnBoardPnip_ResetRam(pDevExtIn);
					}
				}
			}

			break;
		}		
		case ePNDEV_BOARD_FPGA1_HERA:		// FPGA1-Hera board
		{
			if	(fnBoardIrtePnip_VersionOk(	pDevExtIn,
											lDimErrorIn,
											sErrorOut))
				// version of board is ok
			{
				// success
				bResult = TRUE;
				
				// reset PNIP-RAMs which are not initialized by hardware
				fnBoardPnip_ResetRam(pDevExtIn);
			}

			break;
		}
		default:
		{
		    // set ErrorString
		    fnBuildString(	_TEXT(__FUNCTION__),
						    _TEXT("(): Invalid board!"),
						    NULL,
						    lDimErrorIn,
						    sErrorOut);

			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoardIrtePnip_VerifyHwAfterReset(	DEVICE_EXTENSION*	pDevExtIn,
											const UINT32		lDimErrorIn,
											_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			if	(fnBoardPnip_VerifyPnip(pDevExtIn,
										lDimErrorIn,
										sErrorOut))
				// verifying PNIP ok
			{
			volatile UINT8*	pApbPer				= NULL;
			UINT32			lConfigPins6_3		= 0;
			UINT32			lValueConfigPins6_3	= 0;

				// Note:
				//	- check only Bit6:3 of ConfigPins because Bit2:0 may be user specific (e.g. ARM9 clock)

				lValueConfigPins6_3 = fnBoardPnip_GetValueConfigPins(pDevExtIn) & 0x78;

				pApbPer = pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr;

				// read state of ConfigPins latched by Asic
				lConfigPins6_3 = READ_REG_UINT32(pApbPer + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__CONFIG) & 0x78;
			    
				if	(lConfigPins6_3 != lValueConfigPins6_3)
					// error at latching ConfigPins
				{
				_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

					// debug purpose
					WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
										0xaffeaffe);

					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Error at latching ConfigPins (real: 0x%x, expected (Bit2:0=0): 0x%x)!"),
										lValueConfigPins6_3,
										lConfigPins6_3);

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					#ifdef PNDEV_TEST_GPIO

						if	(fnBoardPnip_TestGpio(	pDevExtIn,
													pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr,
													lDimErrorIn,
													sErrorOut))
							// testing GPIO ok
						{
							// success
							bResult = TRUE;
						}

					#else

						// success
						bResult = TRUE;

					#endif
				}
			}

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:			// FPGA1-Hera board
		{
			if	(fnBoardPnip_VerifyPnip(pDevExtIn,
										lDimErrorIn,
										sErrorOut))
				// verifying PNIP ok
			{
				// success
				bResult = TRUE;
			}
			
			break;
		}
		default:
		{
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if version of board is ok
//************************************************************************

BOOLEAN fnBoardIrtePnip_VersionOk(	DEVICE_EXTENSION*	pDevExtIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult						= FALSE;
BOOLEAN	bResultHw					= FALSE;
UINT32	lBitStreamIdRequired1		= 0;
UINT32	lBitStreamIdRequired2		= 0;
UINT32	lBitStreamVersionRequired1	= 0;
UINT32	lBitStreamVersionRequired2	= 0;
_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	// preset
	bResultHw = TRUE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardId)
					// BoardId
			{
				case eBOARD_ID_FPGA1_ERTEC200P_AHB_STD:
				{
					lBitStreamIdRequired1		= 0x4b;
					lBitStreamVersionRequired1	= 0x56;

					break;
				}
				case eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200:
				{
					// error
					bResultHw = FALSE;

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): No support of Ertec200P with PCIe-AHB (ET200 version)!"),
									NULL,
									lDimErrorIn,
									sErrorOut);

					break;
				}
				case eBOARD_ID_FPGA1_ERTEC200P_XHIF_STD:
				{
					lBitStreamIdRequired1		= 0x5b;
					lBitStreamVersionRequired1	= 0x3c;

					lBitStreamIdRequired2		= 0x5d;
					lBitStreamVersionRequired2	= 0x01;

					break;
				}
				case eBOARD_ID_FPGA1_ERTEC200P_XHIF_ET200:
				{
					lBitStreamIdRequired1		= 0x5c;
					lBitStreamVersionRequired1	= 0x3d;

					break;
				}
				default:
				{
					// error
					bResultHw = FALSE;

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid board ID!"),
									NULL,
									lDimErrorIn,
									sErrorOut);

					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
		UINT32 lBoardRevisionRequired = 0x00010001;		// V1.1

			lBitStreamIdRequired1		= 0;
			lBitStreamVersionRequired1	= 0;

			if	(pDevExtIn->eBoardId != eBOARD_ID_EB200P)
				// invalid BoardId
			{
				// error
				bResultHw = FALSE;

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid board ID!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else if	(pDevExtIn->lBoardRevision < lBoardRevisionRequired)
					// invalid version
			{
				// error
				bResultHw = FALSE;

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("FPGA-Version 0x%08x required (current: 0x%08x)"),
									lBoardRevisionRequired,
									pDevExtIn->lBoardRevision);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
			UINT32 lTmp = 0;

				// get FPGA jumper settings for XHIF interface
				//	- all set jumper do not have the same value 0 or 1
				//	- a set jumper inverts only the signal level which is predefined by a PullUp or PullDown
				lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__XHIF_CONFIG);

				if	(lTmp != 0x0007)
					// invalid settings
				{
					// error
					bResultHw = FALSE;

					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid FPGA jumper settings (real: 0x%04x, required: 0x0007)!"), 
										lTmp);

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__STATUS);

					if	(lTmp & BOARD_EB200P_STATUS_MSI)
						// message signaled interrupts can be used
					{
						// error
						bResultHw = FALSE;

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): MSI support not expected!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
				}
			}

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			lBitStreamIdRequired1		= 0x02;
			lBitStreamVersionRequired1	= 0x08;

			break;
		}
		default:
		{
			// error
			bResultHw = FALSE;

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid board!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	if	(bResultHw)
		// success
	{
		// preset
		bResult = TRUE;

		if	(	(pDevExtIn->lBitStreamId		!=	lBitStreamIdRequired1)
			||	(pDevExtIn->lBitStreamVersion	<	lBitStreamVersionRequired1))
			// invalid BitStream1
		{
			if	(	(pDevExtIn->lBitStreamId		!=	lBitStreamIdRequired2)
				||	(pDevExtIn->lBitStreamVersion	<	lBitStreamVersionRequired2))
				// invalid BitStream2
			{
				// error
				bResult = FALSE;

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("At least BitStream version 0x%x.0x%x or 0x%x.0x%x required (current: 0x%x.0x%x)"), 
									lBitStreamIdRequired1,
									lBitStreamVersionRequired1,
									lBitStreamIdRequired2,
									lBitStreamVersionRequired2,
									pDevExtIn->lBitStreamId,
									pDevExtIn->lBitStreamVersion);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if IcuMode is ok (not changed by user)
//************************************************************************

BOOLEAN fnBoardIrtePnip_CheckIcuMode(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN	bResult	= FALSE;
UINT32	lTmp	= 0;
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	if	(pDevExtIn->pIrte != NULL)
		// board with IRTE
	{
		lTmp = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_MODE);

		if	(lTmp != pDevExtIn->lValueIrteIcuReg_IntMode)
			// error at IcuModeRegister
		{
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Invalid IcuMode (real: 0x%x, expected: 0x%x)!",
								pDevExtIn->sPciLocShortAscii,
								lTmp,
								pDevExtIn->lValueIrteIcuReg_IntMode);
					
			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,						// logging
						sTraceDetail);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}
	else if	(pDevExtIn->pPnIp != NULL)
			// board with PNIP
	{
	UINT32 lPnIpReg_IcuControl = 0;

		switch	(pDevExtIn->eBoardDetail)
				// BoardDetail
		{
			case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
			{
				#ifdef FPGA1_ERTEC200P_ARM_NO_TOPLEVEL_ICU
				#else

					// TopLevel-ICU = ARM-ICU

					// PNIP-ICU2 must be used!
					lPnIpReg_IcuControl = PNIP_REG__ICU2_CONTROL;

				#endif

				break;
			}
			case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
			{
				// TopLevel-ICU = PERIF-ICU

				// PNIP-ICU3 must be used!
				lPnIpReg_IcuControl = PNIP_REG__ICU3_CONTROL;

				break;
			}
			default:
			{
				break;
			}
		}

		if	(lPnIpReg_IcuControl)
			// valid ptr
		{
			lTmp = (READ_REG_UINT32(pDevExtIn->pPnIp + lPnIpReg_IcuControl) & 0x0000ffff);

			if	(lTmp != pDevExtIn->lValuePnIpIcuReg_ControlLow)
				// error at ControlRegister
			{
				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Invalid IcuMode (real: 0x%x, expected: 0x%x)!",
									pDevExtIn->sPciLocShortAscii,
									lTmp,
									pDevExtIn->lValuePnIpIcuReg_ControlLow);
					
				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
		else if	(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_HERA)	// mk5656: muss wieder zurückgebaut werden, da eigneltich Pnip zutrifft
				// FPGA1-Hera board
		{
			bResult = TRUE;	
		}
	}
	else if	(pDevExtIn->eBoard == ePNDEV_BOARD_I210)
			// Intel I210 board
	{
		// success
		bResult = TRUE;		
	}
	else if	(pDevExtIn->eBoard == ePNDEV_BOARD_I82574)
			// Intel I82574 board
	{
		// success
		bResult = TRUE;	
	}
	else if	(pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
			// Micrel KSZ8842 board
	{
		// success
		bResult = TRUE;		
	}
	else if	(pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
			// Micrel KSZ8842 board
	{
		// success
		bResult = TRUE;		
	}
	else if	(pDevExtIn->eBoard == ePNDEV_BOARD_TI_OMAP)
			// TI Omap board
	{
		// success
		bResult = TRUE;		
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BoardId
//************************************************************************

UINT32 fnBoardIrtePnip_GetBoardId(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 lResult = 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lResult = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__ID);

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			lResult = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__ID);

			break;
		}
		default:
		{
			break;
		}
	}

	return(lResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BoardRevision
//************************************************************************

UINT32 fnBoardIrtePnip_GetBoardRevision(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 lResult = 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lResult = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__REVISION);

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			lResult = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__REVISION);

			break;
		}
		default:
		{
			break;
		}
	}

	return(lResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BitStreamId
//************************************************************************

UINT32 fnBoardIrtePnip_GetBitStreamId(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 lResult = 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lResult = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__STREAM_ID);

			break;
		}
		default:
		{
			break;
		}
	}

	return(lResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BitStreamVersion
//************************************************************************

UINT32 fnBoardIrtePnip_GetBitStreamVersion(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 lResult = 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lResult = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__STREAM_VERSION);

			break;
		}
		default:
		{
			break;
		}
	}

	return(lResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start/stop reset
//************************************************************************

void fnBoardIrtePnip_ChangeReset(	DEVICE_EXTENSION*	pDevExtIn,
									const BOOLEAN		bStartIn)
{
	if	(bStartIn)
		// start reset
	{
		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
			{
				// start reset
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__RESET),
									0x55aa55aa);

				break;
			}
			case ePNDEV_BOARD_EB200P:	// EB200P board
			{
				// start reset
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__RESET),
									0x55aa55aa);

				break;
			}
			default:
			{
				break;
			}
		}
	}
	else	// stop reset
	{
		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
			{
				// stop reset
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__RESET),
									0);

				break;
			}
			case ePNDEV_BOARD_EB200P:	// EB200P board
			{
				// stop reset
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__RESET),
									0);

				break;
			}
			default:
			{
				break;
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if reset is active
//************************************************************************

BOOLEAN fnBoardIrtePnip_CheckResetActive(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN	bResult	= FALSE;
UINT32	lTmp	= 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__STATUS);

			(lTmp & BOARD_FPGA1_STATUS_RESET) ? (bResult = TRUE) : (bResult = FALSE);

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__STATUS);

			(lTmp & BOARD_EB200P_STATUS_RESET) ? (bResult = TRUE) : (bResult = FALSE);

			break;
		}
		default:
		{
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if PLL is locked
//************************************************************************

BOOLEAN fnBoardIrtePnip_CheckPllLocked(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN	bResult	= FALSE;
UINT32	lTmp	= 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__STATUS);

			(lTmp & BOARD_FPGA1_STATUS_PLL_LOC) ? (bResult = TRUE) : (bResult = FALSE);

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__STATUS);

			(lTmp & BOARD_EB200P_STATUS_PLL_LOC) ? (bResult = TRUE) : (bResult = FALSE);

			break;
		}
		default:
		{
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if any interrupt set at AdminReg and get details
//************************************************************************

BOOLEAN fnBoardIrtePnip_CheckIntAdminReg(	DEVICE_EXTENSION*	pDevExtIn,
											uPNDEV_INT_DETAIL*	pIntDetailOut)
{
BOOLEAN	bResult	= FALSE;
UINT32	lTmp	= 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__STATUS);

			if	(lTmp & BOARD_FPGA1_STATUS_INT)
				// any interrupt exist
			{
				// success
				bResult = TRUE;

				lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL);

				if	(lTmp & BOARD_FPGA1_CONTROL_SET_DEMO_INT)
					// BoardDemoInt set
				{
					pIntDetailOut->uCommon.bBoardDemoInt = TRUE;
				}
				else
				{
					// any AsicInt occurred
					pIntDetailOut->uCommon.bAsicInt = TRUE;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__STATUS);

			if	(lTmp & BOARD_EB200P_STATUS_INT)
				// any interrupt exist
			{
			UINT32 lIrr	= 0;
			UINT32 lAck	= 0;
			UINT32 lImr	= 0;

				// success
				bResult = TRUE;

				// get IRR
				lIrr = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_STATUS_ACK);

				// get IntMask
				lImr = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK);

				if	(	(lIrr & BOARD_EB200P_INT_STATUS_ACK_ACCESS_TIMEOUT)
					&&	(lImr & BOARD_EB200P_INT_STATUS_ACK_ACCESS_TIMEOUT))
					// AccessTimeoutInt set
				{
					pIntDetailOut->uCommon.bAccessTimeoutInt = TRUE;

					lAck |= BOARD_EB200P_INT_STATUS_ACK_ACCESS_TIMEOUT;
				}

				if	(	(lIrr & BOARD_EB200P_INT_STATUS_ACK_GPIO)
					&&	(lImr & BOARD_EB200P_INT_STATUS_ACK_GPIO))
					// GpioInt set
				{
					pIntDetailOut->uCommon.bGpioInt = TRUE;

					lAck |= BOARD_EB200P_INT_STATUS_ACK_GPIO;
				}

				if	(	(lIrr & BOARD_EB200P_INT_STATUS_ACK_DMA)
					&&	(lImr & BOARD_EB200P_INT_STATUS_ACK_DMA))
					// DmaInt set
				{
					pIntDetailOut->uCommon.bDmaInt = TRUE;

					lAck |= BOARD_EB200P_INT_STATUS_ACK_DMA;
				}

				if	(	(lIrr & BOARD_EB200P_INT_STATUS_ACK_DMA_ERROR)
					&&	(lImr & BOARD_EB200P_INT_STATUS_ACK_DMA_ERROR))
					// DmaErrorInt set
				{
					pIntDetailOut->uCommon.bDmaErrorInt = TRUE;

					lAck |= BOARD_EB200P_INT_STATUS_ACK_DMA_ERROR;
				}

				if	(	(lIrr & BOARD_EB200P_INT_STATUS_ACK_DEMO_INT)
					&&	(lImr & BOARD_EB200P_INT_STATUS_ACK_DEMO_INT))
					// BoardDemoInt set
				{
					// Note:
					//	- before acking this interrupt source must be disabled (see fnBoardIrtePnip_ChangeBoardDemoInt())

					pIntDetailOut->uCommon.bBoardDemoInt = TRUE;

					lAck |= BOARD_EB200P_INT_STATUS_ACK_DEMO_INT;
				}

				if	(	(lIrr & BOARD_EB200P_INT_STATUS_ACK_XHIF_HOST)
					&&	(lImr & BOARD_EB200P_INT_STATUS_ACK_XHIF_HOST))
					// AsicInt set
				{
					// any AsicInt occurred
					pIntDetailOut->uCommon.bAsicInt = TRUE;

					lAck |= BOARD_EB200P_INT_STATUS_ACK_XHIF_HOST;
				}

				// ack detected interrupts
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_STATUS_ACK),
									lAck);

				// set EOI
				//	- send deassert message
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL) | BOARD_EB200P_CONTROL_EOI));

				// ensure that EOI is written before state of PERIF-ICU is changed (PostedWrite)
				//	- FPGA must be ready for new falling edge of PERIF-ICU
				lIrr = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_STATUS_ACK);
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check PCIe-AccessTimeout
//************************************************************************

BOOLEAN fnBoardIrtePnip_CheckAccessTimeout(	DEVICE_EXTENSION*	pDevExtIn,
											BOOLEAN*			pWarningOut)
{
BOOLEAN	bResult	= FALSE;
UINT32	lTmp	= 0;

	// preset OutputParameter
	*pWarningOut = FALSE;

	// Note:
	//	- AccessTimeout occurs 
	//		if there is a PCIe access during reset or before initialization of EMC/..
	//		if an accessed IP does not respond in a fixed time interval

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__STATUS);

			if	(lTmp & BOARD_FPGA1_STATUS_ACCESS_TIMEOUT)
				// PCIe-AccessTimeout occurred
			{
				// declare as warning
				*pWarningOut = TRUE;

				// reset and disable 'PCIe-AccessTimeout'
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) | BOARD_FPGA1_CONTROL_RESET_ACCESS_TIMEOUT));

				// enable 'PCIe-AccessTimeout'
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) & (~BOARD_FPGA1_CONTROL_RESET_ACCESS_TIMEOUT)));

				// debug purpose
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
									0xaffeaffe);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_STATUS_ACK);

			if	(lTmp & BOARD_EB200P_INT_STATUS_ACK_ACCESS_TIMEOUT)
				// PCIe-AccessTimeout occurred
			{
				// Note:
				//	- if a PCIe-AccessTimeout occurred then there must be a BoardReset and AccessTimeoutInt must be acked
				//	- otherwise accesses to Asic are terminated by 0xffffffff

				// ack AccessTimeoutInt
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_STATUS_ACK),
									BOARD_EB200P_INT_STATUS_ACK_ACCESS_TIMEOUT);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		default:
		{
			// success
			bResult = TRUE;

			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  enable/disable interrupts at board globally
//************************************************************************

void fnBoardIrtePnip_ChangeIntMaskGlobal(	DEVICE_EXTENSION*	pDevExtIn,
											const BOOLEAN		bEnableIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			if	(bEnableIn)
				// enable interrupt
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) | BOARD_FPGA1_CONTROL_ENABLE_INT));
			}
			else	// disable interrupt
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) & (~BOARD_FPGA1_CONTROL_ENABLE_INT)));
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			if	(bEnableIn)
				// enable interrupt
			{
				// global
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL) | BOARD_EB200P_CONTROL_INT_ENABLE));

				// specific
				//	- Ertec200P XHIF interrupt
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK) | BOARD_EB200P_INT_MASK_ENABLE_XHIF_HOST));

			}
			else	// disable interrupt
			{
				// global
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL) & (~BOARD_EB200P_CONTROL_INT_ENABLE)));

				// specific
				//	- mask all
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK),
									0);
			}

			break;
		}
		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set/clear BoardDemoInt
//************************************************************************

void fnBoardIrtePnip_ChangeBoardDemoInt(DEVICE_EXTENSION*	pDevExtIn,
										const BOOLEAN		bSetIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			if	(bSetIn)
				// set BoardDemoInt
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) | BOARD_FPGA1_CONTROL_SET_DEMO_INT));
			}
			else	// clear BoardDemoInt
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) & (~BOARD_FPGA1_CONTROL_SET_DEMO_INT)));
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			if	(bSetIn)
				// set BoardDemoInt
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK) | BOARD_EB200P_INT_MASK_SET_DEMO_INT));
			}
			else	// clear BoardDemoInt
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__INT_MASK) & (~BOARD_EB200P_INT_MASK_SET_DEMO_INT)));
			}

			break;
		}
		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  change signal XHIF_CS_R_A20
//************************************************************************

void fnBoardIrtePnip_ChangeXhifConfig(	DEVICE_EXTENSION*	pDevExtIn,
										BOOLEAN				bSetA20In)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			if	(bSetA20In)
				// set signal XHIF_CS_R_A20 by external multiplexer to A20 (8x 2MB)
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) & (~BOARD_FPGA1_CONTROL_PAGE_CS_R)));
			}
			else	// set signal XHIF_CS_R_A20 by external multiplexer to CS-R (PageCfgReg)
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL) | BOARD_FPGA1_CONTROL_PAGE_CS_R));
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			if	(bSetA20In)
				// set signal XHIF_CS_R_A20 by external multiplexer to A20 (8x 2MB)
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL) & (~BOARD_EB200P_CONTROL_PAGE_CS_R)));
			}
			else	// set signal XHIF_CS_R_A20 by external multiplexer to CS-R (PageCfgReg)
			{
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL),
									(READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL) | BOARD_EB200P_CONTROL_PAGE_CS_R));
			}

			break;
		}
		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if signal XHIF_CS_R_A20 is set to A20 (8x 2MB)
//************************************************************************

BOOLEAN fnBoardIrtePnip_CheckXhifA20(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN	bResult	= FALSE;
UINT32	lTmp	= 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__CONTROL);

			((lTmp & BOARD_FPGA1_CONTROL_PAGE_CS_R) == 0) ? (bResult = TRUE) : (bResult = FALSE);

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			lTmp = READ_REG_UINT32(pDevExtIn->pAdminReg + BOARD_EB200P_REG__CONTROL);

			((lTmp & BOARD_EB200P_CONTROL_PAGE_CS_R) == 0) ? (bResult = TRUE) : (bResult = FALSE);

			break;
		}
		default:
		{
			break;
		}
	}

	return(bResult);
}
	
