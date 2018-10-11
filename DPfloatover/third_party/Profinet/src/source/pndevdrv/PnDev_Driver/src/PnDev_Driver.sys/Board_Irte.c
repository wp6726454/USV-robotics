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
/*  F i l e               &F: Board_Irte.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with IRTE (Ertec200/400/Soc1)
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
	#include "Board_Irte.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoardIrte_Reset)
//	#pragma alloc_text (PAGE, fnBoardIrte_ResetHw)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitHw1)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitHw2)
//	#pragma alloc_text (PAGE, fnBoardIrte_VerifyHwAfterReset)
//	#pragma alloc_text (PAGE, fnBoardIrte_CheckStartupPblSbl)
//	#pragma alloc_text (PAGE, fnBoardIrte_VersionOk)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitPciBridge)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitEmifController)
//	#pragma alloc_text (PAGE, fnBoardIrte_SetStaticPagesLbu)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitDriverPageLbu)
//	#pragma alloc_text (PAGE, fnBoardIrte_StartDriverPageLbu)
//	#pragma alloc_text (PAGE, fnBoardIrte_StopDriverPageLbu)
//	#pragma alloc_text (PAGE, fnBoardIrte_SetCmdStopArm9)
//	#pragma alloc_text (PAGE, fnBoardIrte_SetCmdDummyArm9)
//	#pragma alloc_text (PAGE, fnBoardIrte_SetCmdGoArm9)
//	#pragma alloc_text (PAGE, fnBoardIrte_SignalWaitingArm9Ok)
//	#pragma alloc_text (PAGE, fnBoardIrte_SignalRun2Arm9Ok)
//	#pragma alloc_text (PAGE, fnBoardIrte_EnableIrteModules)
//	#pragma alloc_text (PAGE, fnBoardIrte_PresetReqListPciBridge)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitIrteIcu)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitPciIcu)
//	#pragma alloc_text (PAGE, fnBoardIrte_InitFlashNand)
//	#pragma alloc_text (PAGE, fnBoardIrte_VerifyIrte)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoardIrte_Reset(	DEVICE_EXTENSION*			pDevExtIn,
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

			if	(pDevExtIn->eBoard == ePNDEV_BOARD_CP1616)
				// CP1616 board (running DefaultFw)
			{
				bStopDefaultFw	= TRUE;
				bResetHw		= TRUE;
			}

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

			if	(fnBoardIrte_InitHw1(	pDevExtIn,
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
					bResultHwReset = fnBoardIrte_ResetHw(	pDevExtIn,
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
						if	(fnBoardIrte_InitHw2(	pDevExtIn,
													bResetHw,
													lDimErrorIn,
													sErrorOut))
							// initializing hardware (phase 2) ok
						{
							if	(fnBoardIrte_VerifyHwAfterReset(pDevExtIn,
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

								// initialize LEDs
								fnBoard_InitLed(pDevExtIn);

								// show IRTE version
								{
								UINT32	lIrteVersion = 0;
								char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

									lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte	+ IRTE_REG__IP_VERSION);

									// set TraceDetail
									RtlStringCchPrintfA(sTraceDetail,
														_countof(sTraceDetail),
														"PnDev[%s]: RESET done, IRTE version=0x%08x",
														pDevExtIn->sPciLocShortAscii,
														lIrteVersion);
					
									// set trace
									fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
												ePNDEV_TRACE_CONTEXT_BASIC,
												TRUE,						// logging
												sTraceDetail);
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
//  initialize hardware (phase 1)
//************************************************************************

BOOLEAN fnBoardIrte_InitHw1(DEVICE_EXTENSION*	pDevExtIn,
							const BOOLEAN		bStopDefaultFwIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
			// success
			bResult = TRUE;

			if	(bStopDefaultFwIn)
				// stop DefaultFw
			{
				// set command 'STOP' for Arm9
				//	- command checked by Arm9 after HwReset
				fnBoardIrte_SetCmdStopArm9(pDevExtIn);
			}
			else	// release device, start DefaultFw
			{
				// set command 'DUMMY' for Arm9 -> program of flash is used
				//	- command checked by Arm9 after HwReset
				fnBoardIrte_SetCmdDummyArm9(pDevExtIn);
			}

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// success
			bResult = TRUE;

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			// success
			bResult = TRUE;

			// set static pages
			fnBoardIrte_SetStaticPagesLbu(pDevExtIn);

			// initialize PciBridge
			fnBoardIrte_InitPciBridge(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			// success
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
//  execute HwReset
//************************************************************************

BOOLEAN fnBoardIrte_ResetHw(DEVICE_EXTENSION*	pDevExtIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;
UINT32	lArrayConfigSpace[PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)];

    // preset
	RtlZeroMemory(	lArrayConfigSpace,
					PNDEV_PCI_CONFIG_SPACE__SIZE);

	// reset running
	pDevExtIn->bResetRunning = TRUE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
			// start/stop reset
			{
				// SystemCtrlReg: SoftReset=ON
				//	- Reset pulse (specified by pulse duration) will be set
				//	- setting Bit XRES_SOFT is not stored -> Reset need not to be deactivated after setting Bit XRES_SOFT
				//	- high active
				//	- don't reset AHB-side of PCI-AHB-Bridge
				//	- don't reset PCI-side of PCI-AHB-Bridge (PCI configuration would be lost!)
				//	- pulse duration = 0 -> 8x 20nsec = 160nsec
				//	- ResStatus register cannot be polled because it is not accessible during reset and it will not be changed if there is a SoftReset several times
				//	- DefaultValue of HwReset ok -> |= 0x..
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_SCRB + ERTEC400_SCRB_REG__RES_CTRL),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_SCRB + ERTEC400_SCRB_REG__RES_CTRL) | PNDEV_UINT32_SET_BIT_1));
			}

			// Note:
			//	- SBL must be ready before any further driver action
			//	- immediately after reset there is a time with undefined access results
			//	- here: handshake commands are at internal SRAM -> no EMIF controller must be initialized before accessing these cells
			//	  -> wait only a small fixed time

			// wait 1msec

			if	(fnDelayThread(	pDevExtIn,
								1,
								lDimErrorIn,
								sErrorOut))
				// starting WaitTime of current thread ok
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			if	(fnPrepareConfigSpaceHwReset(	pDevExtIn,
												lDimErrorIn,
												lArrayConfigSpace,
												sErrorOut))
				// preparing ConfigSpace for HwReset ok
			{
				// start/stop reset
				{
					// SystemCtrlReg: SoftReset=ON
					//	- Reset pulse (specified by pulse duration) will be set
					//	- setting Bit XRES_SOFT is not stored -> Reset need not to be deactivated after setting Bit XRES_SOFT
					//	- high active
					//	- don't reset AHB-side of PCI-AHB-Bridge
					//	- don't reset PCI-side of PCI-AHB-Bridge (PCI configuration would be lost!)
					//	- pulse duration = 0 -> 8x 20nsec = 160nsec
					//	- ResStatus register cannot be polled because it is not accessible during reset and it will not be changed if there is a SoftReset several times
					//	- DefaultValue of HwReset ok -> |= 0x..
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_SCRB + ERTEC400_SCRB_REG__RES_CTRL),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_SCRB + ERTEC400_SCRB_REG__RES_CTRL) | PNDEV_UINT32_SET_BIT_1));
				}
	
				// Note:
				//	- SBL must be ready before any further driver action
				//	- immediately after software reset bootloader resets PCI configuration, and runs SBL
				//	- SBL restores PCI configuration
				//	  -> wait a fixed time for reinitialisation
	
				// wait 100 msec
				if	(fnDelayThread(	pDevExtIn,
									100,
									lDimErrorIn,
									sErrorOut))
					// starting WaitTime of current thread ok
				{
					if	(fnRestoreConfigSpaceHwReset(	pDevExtIn,
														lArrayConfigSpace,
														lDimErrorIn,
														sErrorOut))
						// restoring ConfigSpace after HwReset ok
					{
						// success
						bResult = TRUE;
					}
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			// start/stop reset
			{
				// SystemCtrlReg: SoftReset=ON
				//	- Reset pulse (specified by pulse duration) will be set
				//	- setting Bit XRES_SOFT is not stored -> Reset need not be deactivated after setting Bit XRES_SOFT
				//	- high active
				//	- don't reset AHB-side of PCI-AHB-Bridge
				//	- don't reset PCI-side of PCI-AHB-Bridge (PCI configuration would be lost!)
				//	- pulse duration = 0 -> 8x 20nsec = 160nsec
				//	- ResStatus register cannot be polled because it is not accessible during reset and it will not be changed if there is a SoftReset several times
				//	- DefaultValue of HwReset ok -> |= 0x..
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_SCRB + ERTEC200_SCRB_REG__RES_CTRL),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_SCRB + ERTEC200_SCRB_REG__RES_CTRL) | PNDEV_UINT32_SET_BIT_1));
			}

			// Note:
			//	- SBL must be ready before any further driver action
			//	- immediately after reset there is a time with undefined access results
			//	- SBL must initialize EMIF controller before valid data can be read at SDRAM
			//	  -> wait a fixed time

			// wait 20msec

			if	(fnDelayThread(	pDevExtIn,
								20,
								lDimErrorIn,
								sErrorOut))
				// starting WaitTime of current thread ok
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			if	(fnPrepareConfigSpaceHwReset(	pDevExtIn,
												lDimErrorIn,
												lArrayConfigSpace,
												sErrorOut))
				// preparing ConfigSpace for HwReset ok
			{
				// start/stop reset
				{
					// SCRB-ResStatus register cannot be polled because it is not accessible during reset

					// set PulsDuration to 8*(n+1)*20nsec=320nsec
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__RES_PULS_DURATION),
										1);

					// stop and disable Watchdog
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_WATCHDOG + SOC1_WD_REG__CTRL_STATUS),
										(0x98760000 + 0));

					// set WD1-ReloadValue to 0x500*320nsec=409,6usec
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_WATCHDOG + SOC1_WD_REG__RELD1_H),
										(0x98760000 + 0x0000));
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_WATCHDOG + SOC1_WD_REG__RELD1_L),
										(0x98760000 + 0x500));

					// load WD0/1 with ReloadValue0/1
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_WATCHDOG + SOC1_WD_REG__CTRL_STATUS),
										(0x98760000 + SOC1_WD_CTRL_STATUS__LOAD_TRIGGER));

					// enable WD-reset
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__RES_CTRL),
										SOC1_RES_CTRL__ENABLE_WD_RESET);

					//  preset ReqList of PciBridge
					fnBoardIrte_PresetReqListPciBridge(pDevExtIn);

					//	enable WD1
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_WATCHDOG + SOC1_WD_REG__CTRL_STATUS),
										(0x98760000 + SOC1_WD_CTRL_STATUS__ENABLE1));
				}

				// Note:
				//	- SBL must be ready before any further driver action
				//	- immediately after reset there is a time with undefined access results
				//	- SBL must initialize DDR2 controller before valid data can be read at SDRAM
				//	  -> wait a fixed time

				// wait 100msec

				if	(fnDelayThread(	pDevExtIn,
									100,
									lDimErrorIn,
									sErrorOut))
					// starting WaitTime of current thread ok
				{
					if	(fnRestoreConfigSpaceHwReset(	pDevExtIn,
														lArrayConfigSpace,
														lDimErrorIn,
														sErrorOut))
						// restoring ConfigSpace after HwReset ok
					{
					UINT32 lResStatus = 0;

						// get ResetStatus
						lResStatus  = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SCRB + SOC1_SCRB_REG__RES_STATUS);
						lResStatus &= (PNDEV_UINT32_SET_BIT_1 + PNDEV_UINT32_SET_BIT_0);

						if	(lResStatus	!= SOC1_RES_STATUS_BIT1_0__WD_RESET)
							// not WdReset
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at watchdog reset!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
						}
						else
						{
							// success
							bResult = TRUE;
						}
					}
				}
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
//  initialize hardware (phase 2) 
//************************************************************************

BOOLEAN fnBoardIrte_InitHw2(DEVICE_EXTENSION*	pDevExtIn,
							const BOOLEAN		bResetHwIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN bResult			= FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// wait for PBL/SBL startup of Arm946
			//	- must be finished before any further driver action!

			if	(fnBoardIrte_CheckStartupPblSbl(pDevExtIn,
												bResetHwIn,
												lDimErrorIn,
												sErrorOut))
				// startup of PBL/SBL ok
			{
				if	(fnBoardIrte_VersionOk(	pDevExtIn,
											lDimErrorIn,
											sErrorOut))
					// version of board is ok
					//	- Note: startup of PBL/SBL must be finished for ensuring access to VersionPatterns which may be updated by SBL!
				{
					if	(fnBoardIrte_EnableIrteModules(	pDevExtIn,
														bResetHwIn,
														lDimErrorIn,
														sErrorOut))
						// enabling IRTE modules ok
					{
						// success
						bResult = TRUE;

						// initialize IRTE-ICU
						fnBoardIrte_InitIrteIcu(pDevExtIn);
					}
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			// wait for PBL/SBL startup of Arm946
			//	- must be finished before any further driver action!

			if	(fnBoardIrte_CheckStartupPblSbl(pDevExtIn,
												bResetHwIn,
												lDimErrorIn,
												sErrorOut))
				// startup of PBL/SBL ok
			{
				if	(fnBoardIrte_VersionOk(	pDevExtIn,
											lDimErrorIn,
											sErrorOut))
					// version of board is ok
					//	- Note: startup of PBL/SBL must be finished for ensuring access to VersionPatterns which may be updated by SBL!
				{
					// set static pages
					fnBoardIrte_SetStaticPagesLbu(pDevExtIn);

					// initialize PciBridge
					fnBoardIrte_InitPciBridge(pDevExtIn);

					if	(fnBoardIrte_EnableIrteModules(	pDevExtIn,
														bResetHwIn,
														lDimErrorIn,
														sErrorOut))
						// enabling IRTE modules ok
					{
					BOOLEAN bResultEmif = FALSE;

						// initialize IRTE-ICU
						fnBoardIrte_InitIrteIcu(pDevExtIn);

						// initialize EMIF controller
						{
							// Note:
							//	- initialization is already done by boot firmware
							//	- Arm9 is now polling semaphore at SDRAM
							//	- if driver reinitializes the EMIF controller there is the danger that the EMIF controller restarts
							//	  -> behavior of Arm9 may be undefined if access to SDRAM fails!
							bResultEmif = TRUE;
/*
							//------------------------------------------------------------------------
							// [LBU: using dynamic DriverPage]
							//	- EMIF Reg
							fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
															eBOARD_EB200__LBU_DYN_PAGE_EMIF_REG,
															0,
															__FUNCTION__);
							{
								// initialize EMC controller
								bResultEmif = fnBoardIrte_InitEmifController(	pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifReg.pPtr,
																				lDimErrorIn,
																				sErrorOut);
							}
							//------------------------------------------------------------------------
							// [LBU: end of using dynamic DriverPage]
							fnBoardIrte_StopDriverPageLbu(pDevExtIn);
*/
						}

						if	(bResultEmif)
							// success
						{
							// success
							bResult = TRUE;
						}
					}
				}
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			// wait for PBL/SBL startup of MIPS
			//	- must be finished before any further driver action!

			if	(fnBoardIrte_CheckStartupPblSbl(pDevExtIn,
												bResetHwIn,
												lDimErrorIn,
												sErrorOut))
				// startup of PBL/SBL ok
			{
				if	(fnBoardIrte_VersionOk(	pDevExtIn,
											lDimErrorIn,
											sErrorOut))
					// version of board is ok
					//	- Note: startup of PBL/SBL must be finished for ensuring access to VersionPatterns which may be updated by SBL!
				{
					if	(fnBoardIrte_EnableIrteModules(	pDevExtIn,
														bResetHwIn,
														lDimErrorIn,
														sErrorOut))
						// enabling IRTE modules ok
					{
						// initialize IRTE-ICU
						fnBoardIrte_InitIrteIcu(pDevExtIn);

						if	(fnBoardIrte_InitPciIcu(pDevExtIn,
													lDimErrorIn,
													sErrorOut))
							// initializing PCI-ICU ok
						{
							// success
							bResult = TRUE;

							if	(pDevExtIn->bPcBoard)
								// PcBoard
							{
								// initialize NAND flash
								fnBoardIrte_InitFlashNand(pDevExtIn);
							}
						}
					}
				}
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

BOOLEAN fnBoardIrte_VerifyHwAfterReset(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(fnBoardIrte_VerifyIrte(pDevExtIn,
								lDimErrorIn,
								sErrorOut))
		// verifying IRTE ok
	{
		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_CP1616:	// CP1616 board
			{
				// success
				bResult = TRUE;
				break;
			}
			case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
			{	
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};
				{
					// get SBL version
					pDevExtIn->uSblInfo.lSblDetail		= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr + BOARD_EB400__SDRAM_OFFSET_IDENT);
					pDevExtIn->uSblInfo.uVersion.lMinor	= READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr + BOARD_EB400__SDRAM_OFFSET_VERSION_MIN);
					pDevExtIn->uSblInfo.uVersion.lMajor	= READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr + BOARD_EB400__SDRAM_OFFSET_VERSION_MAJ);
				}

				if	(	(pDevExtIn->uSblInfo.lSblDetail			!= 0xFF550003)
					||	(pDevExtIn->uSblInfo.uVersion.lMajor	!= 0x0001)
					||	(pDevExtIn->uSblInfo.uVersion.lMinor	!= 0x0000))
					// invalid SBL
				{
					if	(pDevExtIn->uSblInfo.lSblDetail != 0xFF550003)
						//Invalid SBL Type
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid SBL Type (real: 0x%08x, required: 0xFF550003)!"), 
											pDevExtIn->uSblInfo.lSblDetail);
					}
					else	//Invalid SBL Version
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid SBL Version (real: 0x%04x.0x%04x, required: 0x0001.0000)!"), 
											pDevExtIn->uSblInfo.uVersion.lMajor,
											pDevExtIn->uSblInfo.uVersion.lMinor);
					}

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;
				}

				break;
			}
			case ePNDEV_BOARD_EB200:	// EB200 board
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				//------------------------------------------------------------------------
				// [LBU: using dynamic DriverPage]
				//	- 8Bit-flash, page0
				fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
												eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK0,
												0,
												__FUNCTION__);
				{
					// get SBL version at 8Bit-flash
					pDevExtIn->uSblInfo.lSblDetail		= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0.pPtr + BOARD_EB200__FLASH_OFFSET_FW_IDENT);
					pDevExtIn->uSblInfo.uVersion.lMajor	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0.pPtr + BOARD_EB200__FLASH_OFFSET_FW_VERSION);
				}
				//------------------------------------------------------------------------
				// [LBU: end of using dynamic DriverPage]
				fnBoardIrte_StopDriverPageLbu(pDevExtIn);

				if	(	(pDevExtIn->uSblInfo.lSblDetail			!= 0xFF5555FF)
					||	(pDevExtIn->uSblInfo.uVersion.lMajor	!= 0x00000002))
					// invalid SBL
				{
					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid SBL at plugged 8Bit flash (real: 0x%08x.0x%08x, required: 0xFF5555FF.0x00000002)!"), 
										pDevExtIn->uSblInfo.lSblDetail,
										pDevExtIn->uSblInfo.uVersion.lMajor);

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{			
				UINT32	lScrbId			= 0;
				UINT32	lBootModePin	= 0;
				UINT32	lConfigPin		= 0;
				UINT32	lEmifVersion	= 0;
				UINT32	lIrteVersion	= 0;

					// read SCRB-ID
					lScrbId = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_SCRB + ERTEC200_SCRB_REG__ID);

					// read BootModePin
					lBootModePin = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_SCRB + ERTEC200_SCRB_REG__BOOT);

					// read ConfigPin
					lConfigPin = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_SCRB + ERTEC200_SCRB_REG__CONFIG);

					//------------------------------------------------------------------------
					// [LBU: using dynamic DriverPage]
					//	- EMIF Reg
					fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
													eBOARD_EB200__LBU_DYN_PAGE_EMIF_REG,
													0,
													__FUNCTION__);
					{
						// read EMIF version
						lEmifVersion = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifReg.pPtr + ERTEC200_EMIF_REG__REVISION);
					}
					//------------------------------------------------------------------------
					// [LBU: end of using dynamic DriverPage]
					fnBoardIrte_StopDriverPageLbu(pDevExtIn);

					// read IRTE version
					lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION);

					if	(lScrbId != 0x40270100)
						// invalid SCRB-ID
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid SCRB-ID (real: 0x%x, expected: 0x40270100)!"),
											lScrbId);

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										sErrorDetail,
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else if	(lBootModePin != 0x00000008)
							// invalid BootModePin
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid BootMode settings (real: 0x%x, expected: 0x08=1000)! Please check the EB200 jumpers!"),
											lBootModePin);

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										sErrorDetail,
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else if	(lConfigPin != 0x00000012)
							// invalid lConfigPin
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid config settings (real: 0x%x, expected: 0x12=x001001x)! Please check the EB200 jumpers!"),
											lConfigPin);

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										sErrorDetail,
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else if	(lEmifVersion != 0x00000100)
							// invalid EMIF version
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid EMIF controller!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;
					}
				}

				break;
			}
			case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
			case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
			{
				// success
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
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check startup of PBL/SBL
//************************************************************************

BOOLEAN fnBoardIrte_CheckStartupPblSbl(	DEVICE_EXTENSION*	pDevExtIn,
										const BOOLEAN		bResetHwIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(!bResetHwIn)
		// no HwReset done
	{
		// Note:
		//	- AsicCore may be used by other Apps
		//	- then state of AsicCore must not be checked by driver!

		// success
		bResult = TRUE;
	}
	else
	{
	BOOLEAN	bErrorDelay		= FALSE;
	BOOLEAN	bErrorTimeout	= FALSE;

		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_CP1616:	// CP1616 board
			{
			UINT32 i = 0;

				// preset
				bErrorTimeout = TRUE;

				for	(i = 0; i < 100; i++)
					// MaxTime=100msec
				{
					if	(!fnBoardIrte_SignalWaitingArm9Ok(pDevExtIn))
						// signal 'WAITING' not set
					{
						// try again in 10msec

						if	(!fnDelayThread(pDevExtIn,
											10,
											lDimErrorIn,
											sErrorOut))
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
										_TEXT("(): Timeout, error at startup of SBL!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;
					}
				}

				break;
			}
			case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
			{
			UINT32 i = 0;
			volatile UINT32* pArm9BootAlive = NULL;

				// set ptr
				pArm9BootAlive = (volatile UINT32*) (pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr + BOARD_EB400__SDRAM_OFFSET_ALIVE);

				// preset
				bErrorTimeout = TRUE;

				for	(i = 0; i < 100; i++)
					// MaxTime=100msec
				{
					if	(*pArm9BootAlive != 0x00000000)
						// error
					{
						// try again in 10msec

						if	(!fnDelayThread(pDevExtIn,
											10,
											lDimErrorIn,
											sErrorOut))
							// error at starting WaitTime of current thread
						{
							bErrorDelay	= TRUE;

							// leave loop
							break;
						}
					}
					else
					{
					UINT32 lPattern = 0x12345678;

						// check SBL functionality
						//	- write a pattern
						//	- check if it is inverted after 1msec

						*pArm9BootAlive = lPattern;

						// wait 10msec

						if	(!fnDelayThread(pDevExtIn,
											10,
											lDimErrorIn,
											sErrorOut))
							// error at starting WaitTime of current thread
						{
							bErrorDelay	= TRUE;

							// leave loop
							break;
						}
						else
						{
							if	(*pArm9BootAlive == (~lPattern))
								// success
							{
								// success
								bErrorTimeout = FALSE;

								// leave loop
								break;
							}
						}
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
										_TEXT("(): Timeout, error at startup of SBL!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;
					}
				}

				break;
			}
			case ePNDEV_BOARD_EB200:	// EB200 board
			{
				//------------------------------------------------------------------------
				// [LBU: using dynamic DriverPage]
				//	- SDRAM (page Arm9-IF)
				fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
												eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM,
												BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED_TMP,
												__FUNCTION__);
				{
				volatile UINT32*	pArm9BootSemaphore	= NULL;
				volatile UINT32*	pArm9BootAlive		= NULL;
				UINT32 i = 0;

					// set ptr
					pArm9BootSemaphore	= (volatile UINT32*) (pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr + BOARD_EB200__SDRAM_OFFSET_SEMAPHORE);
					pArm9BootAlive		= (volatile UINT32*) (pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr + BOARD_EB200__SDRAM_OFFSET_ALIVE);

					// preset
					bErrorTimeout = TRUE;

					for	(i = 0; i < 100; i++)
						// MaxTime=100msec
					{
						if	(	(*pArm9BootSemaphore	!= 0x00000000)
							||	(*pArm9BootAlive		!= 0x00000000))
							// error
						{
							// try again in 10msec

							if	(!fnDelayThread(pDevExtIn,
												10,
												lDimErrorIn,
												sErrorOut))
								// error at starting WaitTime of current thread
							{
								bErrorDelay	= TRUE;

								// leave loop
								break;
							}
						}
						else
						{
						UINT32 lPattern = 0x12345678;

							// check Arm9 functionality
							//	- write a pattern
							//	- check if it is inverted after 1msec

							*pArm9BootAlive = lPattern;

							// wait 10msec

							if	(!fnDelayThread(pDevExtIn,
												10,
												lDimErrorIn,
												sErrorOut))
								// error at starting WaitTime of current thread
							{
								bErrorDelay	= TRUE;

								// leave loop
								break;
							}
							else
							{
								if	(*pArm9BootAlive == (~lPattern))
									// success
								{
									// success
									bErrorTimeout = FALSE;

									// leave loop
									break;
								}
							}
						}
					}
				}
				//------------------------------------------------------------------------
				// [LBU: end of using dynamic DriverPage]
				fnBoardIrte_StopDriverPageLbu(pDevExtIn);

				if	(!bErrorDelay)
					// no TimerError
				{
					if	(bErrorTimeout)
						// timeout
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Timeout, error at startup of SBL!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;
					}
				}

				break;
			}
			case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
			case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
			{
				// success
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
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if version of board is ok
//************************************************************************

BOOLEAN fnBoardIrte_VersionOk(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;
_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
		BOOLEAN						bResultPage			= FALSE;
		UINT32						lOffsetFlashData	= 0;
		uPNDEV_FLASH_PAR			uFlashPar;
		uPNDEV_FLASH_DATA_CP1616	uFlashData;

			//------------------------------------------------------------------------
			// start using FlashPage
			bResultPage = fnBoard_StartFlashPage(	pDevExtIn,
													FALSE,				// don't verify chip (AsicCore may read wrong opcode!)
													lDimErrorIn,
													&uFlashPar,
													&lOffsetFlashData,
													sErrorOut);
			{
				if	(bResultPage)
					// success
				{
					// read FlashData
					*((uPNDEV_FLASH_DATA_CP1616*) &uFlashData) = *((uPNDEV_FLASH_DATA_CP1616*) (uFlashPar.pFlash + lOffsetFlashData));
				}
			}
			//------------------------------------------------------------------------
			// stop using FlashPage
			fnBoard_StopFlashPage(pDevExtIn);

			if	(bResultPage)
				// success
			{
				// check version
				{
				_TCHAR	sVersionSbl[PNDEV_SIZE_STRING_BUF_SHORT] = {0};
				UINT8	lArrayVersion[4];
				UINT32	lVersionSbl = 0;

					// get version as string
					{
					UINT32 lLenTmp	= 0;
					UINT32 i		= 0;

						lLenTmp = sizeof(uAM_VERSION_BOOT);

						for	(i = 0; i < lLenTmp; i++)
							// complete array
						{
							sVersionSbl[i] = (_TCHAR) uFlashData.uBootVersion.uByte[i];
						}

						// add \0
						sVersionSbl[lLenTmp] = 0;
					}

					// get version as number
					{
						lArrayVersion[0]	= uFlashData.uBootVersion.uByte[9];
						lArrayVersion[1]	= uFlashData.uBootVersion.uByte[11];
						lArrayVersion[2]	= uFlashData.uBootVersion.uByte[13];
						lArrayVersion[3]	= uFlashData.uBootVersion.uByte[15];

						if	(	(lArrayVersion[0] >= '0')
							&&	(lArrayVersion[0] <= '9')
							&&	(lArrayVersion[1] >= '0')
							&&	(lArrayVersion[1] <= '9')
							&&	(lArrayVersion[2] >= '0')
							&&	(lArrayVersion[2] <= '9')
							&&	(lArrayVersion[3] >= '0')
							&&	(lArrayVersion[3] <= '9'))
							// valid number 0..9
						{
							// convert to UINT32
							//	- '0'..'9' = 0x30..0x39
							lVersionSbl	=	((lArrayVersion[0]-0x30) * 1000)
										+	((lArrayVersion[1]-0x30) * 100)
										+	((lArrayVersion[2]-0x30) * 10)
										+	((lArrayVersion[3]-0x30) * 1);
						}
					}

					if	(lVersionSbl < 1130)
						// version < V1.1.3.0
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid SBL, (real: '%s', required >= V1.1.3.0)!"),
											sVersionSbl);

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										sErrorDetail,
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;
					}
				}
			}

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// success
			bResult = TRUE;

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
		UINT32 lVersionFpgaMajor = 0;

			// Note:
			//	- ResetDevice may have failed last time
			//	  -> static pages of LBU are not set -> hardware referenced by default values is accessed
			//	  -> check register at ResetDevice

			// read version of PciBridge-FPGA
			lVersionFpgaMajor = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MAJOR);

			if	(lVersionFpgaMajor < 0x00000010)
				// invalid version
			{
				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Invalid version of PCI-Bridge-FPGA (real: 0x%x, required: >= 0x10)!"),
									lVersionFpgaMajor);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			// success
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
//	initialize PciBridge
//************************************************************************

void fnBoardIrte_InitPciBridge(DEVICE_EXTENSION* pDevExtIn)
{
	// set 32Bit consistency
	WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__BAR0_CONS),
						BOARD_EB200_PCI_BRIDGE_BARX_CONS__32BIT);
	WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__BAR1_CONS),
						BOARD_EB200_PCI_BRIDGE_BARX_CONS__32BIT);
	WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__BAR2_CONS),
						BOARD_EB200_PCI_BRIDGE_BARX_CONS__32BIT);
	WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__BAR3_CONS),
						BOARD_EB200_PCI_BRIDGE_BARX_CONS__32BIT);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	initialize EMIF controller
//************************************************************************

BOOLEAN fnBoardIrte_InitEmifController(	volatile UINT8*			pEmifRegIn,
										const UINT32			lDimErrorIn,
										_TCHAR*					sErrorOut)
{
BOOLEAN	bResult				= FALSE;
BOOLEAN	bResultEmifInitDone	= FALSE;
UINT32	lCtrReadPci			= 10000;
UINT32	i					= 0;

	// wait for startup of EMIF controller
	//	- very important!
	{
		for (i = 0; i < lCtrReadPci; i++)
		{
			if	((READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__SDRAM_REFRESH) & ERTEC200_EMIF_SDRAM_REFRESH__INIT_DONE) != 0)
				// SDRAM initialization done
			{
				// success
				bResultEmifInitDone = TRUE;

				// leave loop
				break;
			}
		}
	}

	if	(!bResultEmifInitDone)
		// error
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Error at startup of EMIF controller!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
	} 
	else
	{
	UINT32	lAsyncWaitCycle	= 0;
	UINT32	lSdram			= 0;
	UINT32	lSdramRefresh	= 0;
	UINT32	lAsyncBank0		= 0;
	UINT32	lAsyncBank1		= 0;
	UINT32	lAsyncBank2		= 0;
	UINT32	lAsyncBank3		= 0;
	UINT32	lExtended		= 0;

		// configure ASYNC-WaitCycle
		//	- default
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_WAIT_CYCLE),
							0x40000080);

		// configure SDRAM
		//	- 8 ColumnAdrLines
		//	- 1 Bank
		//	- 8 RowAdrLines
		//	- CAS-Latency=3clocks
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__SDRAM),
							0x00002000);

		// configure SDRAM-Refresh
		//	- required time between 2 SDRAM Refresh cycles=32 clocks
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__SDRAM_REFRESH),
							0x20000020);

		// configure ASYNC-Bank0
		//	- 8Bit device
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK0),
							0x04302200);

		// configure ASYNC-Bank1
		//	- 16Bit device
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK1),
							0x04302201);

		// configure ASYNC-Bank2
		//	- 32Bit device
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK2),
							0x44622312);

		// configure ASYNC-Bank3
		//	- 32Bit device
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK3),
							0xc4222112);

		// configure Extended
		//	- SDRAM as 32Bit device
		WRITE_REG_UINT32(	(pEmifRegIn + ERTEC200_EMIF_REG__EXTENDED),
							0x00020000);

		// read registers
		lAsyncWaitCycle	= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_WAIT_CYCLE);
		lSdram			= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__SDRAM);
		lSdramRefresh	= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__SDRAM_REFRESH);
		lAsyncBank0 	= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK0);
		lAsyncBank1 	= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK1);
		lAsyncBank2 	= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK2);
		lAsyncBank3 	= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__ASYNC_BANK3);
		lExtended		= READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__EXTENDED);

		if	(	(lAsyncWaitCycle	!= 0x40000080)
			||	(lSdram				!= 0x00002000)
			||	(lSdramRefresh		!= 0x20000020)
			||	(lAsyncBank0		!= 0x04302200)
			||	(lAsyncBank1		!= 0x04302201)
			||	(lAsyncBank2		!= 0x44622312)
			||	(lAsyncBank3		!= 0xc4222112)
			||	(lExtended			!= 0x00020000))
			// error
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid EMIF configuration!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			// preset
			bResultEmifInitDone = FALSE;

			// wait for confirmation of EMIF controller
			{
				for (i = 0; i < lCtrReadPci; i++)
				{
					if	((READ_REG_UINT32(pEmifRegIn + ERTEC200_EMIF_REG__SDRAM_REFRESH) & ERTEC200_EMIF_SDRAM_REFRESH__INIT_DONE) != 0)
						// SDRAM initialization done
					{
						// success
						bResultEmifInitDone = TRUE;

						// leave loop
						break;
					}
				}
			}

			if	(!bResultEmifInitDone)
				// error
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Missing confirmation of EMIF controller!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	LBU: set static pages (analog XHIF)
//************************************************************************

void fnBoardIrte_SetStaticPagesLbu(DEVICE_EXTENSION* pDevExtIn)
{
	// set page0 to IRTE
	{
		// 2MB aligned
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P0_RG_L),
							LOW16_OF_UINT32(BOARD_EB200__BAR0_SIZE_LBU_PAGE0));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P0_RG_H),
							HIGH16_OF_UINT32(BOARD_EB200__BAR0_SIZE_LBU_PAGE0));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P0_OF_L),
							LOW16_OF_UINT32(ERTEC200_AHB_ADR__IRTE));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P0_OF_H),
							HIGH16_OF_UINT32(ERTEC200_AHB_ADR__IRTE));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P0_CFG),
							ERTEC200_LBU_PX_CFG__32_BIT);

		// ptr = BAR0
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.pPtr				= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_EB200__BAR0_OFFSET_LBU_PAGE0);
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.lIndexBar			= 0;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.lOffsetBar		= BOARD_EB200__BAR0_OFFSET_LBU_PAGE0;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.lSize				= ERTEC200_AHB_SIZE__IRTE;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.lAdrAhb			= ERTEC200_AHB_ADR__IRTE;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.lOffsetPhyStart	= 0;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.pIrte_Debug				= (volatile IRTE_SW_MAKRO_T*) pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.pPtr;
		pDevExtIn->pIrte												= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte.pPtr;

		// set AsicDetail
		{
		UINT32 lIrteVersion = 0;

			// get IrteVersion
			lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION);

			if	(lIrteVersion == IRTE_IP_VERSION__REV6_METAL_FIX)
				// MetalFix
			{
				pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV6_METAL_FIX;
			}
			else
			{
				pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV6;
			}
		}
	}

	// set page1 to ApbPer
	{
		// 2MB aligned
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P1_RG_L),
							LOW16_OF_UINT32(BOARD_EB200__BAR1_SIZE_LBU_PAGE1));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P1_RG_H),
							HIGH16_OF_UINT32(BOARD_EB200__BAR1_SIZE_LBU_PAGE1));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P1_OF_L),
							LOW16_OF_UINT32(ERTEC200_AHB_ADR__APB_PER));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P1_OF_H),
							HIGH16_OF_UINT32(ERTEC200_AHB_ADR__APB_PER));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P1_CFG),
							ERTEC200_LBU_PX_CFG__32_BIT);

		// ptr = BAR1
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr			= (pDevExtIn->uHwResKm.uBar[1].pVirtualAdr + BOARD_EB200__BAR1_OFFSET_LBU_PAGE1);
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.lIndexBar		= 1;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.lOffsetBar		= BOARD_EB200__BAR1_OFFSET_LBU_PAGE1;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.lSize			= ERTEC200_AHB_SIZE__APB_PER;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.lAdrAhb			= ERTEC200_AHB_ADR__APB_PER;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.lOffsetPhyStart	= 0;
	}

	// set page2 to EMIF-SDRAM
	{
	UINT32 lAhbAdrSdram = 0;

		lAhbAdrSdram = (ERTEC200_AHB_ADR__EMIF_SDRAM + (BOARD_EB200__SDRAM_PAGE_SHARED_DIRECT * (1024*1024*2)));

		// 2MB aligned
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P2_RG_L),
							LOW16_OF_UINT32(BOARD_EB200__BAR2_SIZE_LBU_PAGE2));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P2_RG_H),
							HIGH16_OF_UINT32(BOARD_EB200__BAR2_SIZE_LBU_PAGE2));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P2_OF_L),
							LOW16_OF_UINT32(lAhbAdrSdram));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P2_OF_H),
							HIGH16_OF_UINT32(lAhbAdrSdram));
		WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P2_CFG),
							ERTEC200_LBU_PX_CFG__32_BIT);

		// ptr = BAR2
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_EB200__BAR2_OFFSET_LBU_PAGE2);
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.lIndexBar		= 2;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.lOffsetBar		= BOARD_EB200__BAR2_OFFSET_LBU_PAGE2;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.lSize			= BOARD_EB200__BAR2_SIZE_LBU_PAGE2;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.lAdrAhb			= lAhbAdrSdram;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.lOffsetPhyStart	= (BOARD_EB200__SDRAM_PAGE_SHARED_DIRECT * (1024*1024*2));
	}

	// set page3 (dynamic DriverPage) to ...
	{
		// initialize dynamic DriverPage
		fnBoardIrte_InitDriverPageLbu(pDevExtIn);
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	LBU: initialize dynamic DriverPage
//************************************************************************

void fnBoardIrte_InitDriverPageLbu(DEVICE_EXTENSION* pDevExtIn)
{
	pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifReg.pPtr			= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank1.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank2.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank3.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr	= NULL;	
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	LBU: start using dynamic DriverPage
//************************************************************************

void fnBoardIrte_StartDriverPageLbu(	DEVICE_EXTENSION*					pDevExtIn,
										const eBOARD_EB200__LBU_DYN_PAGE	ePageIn,
										const UINT32						lSegmentAhbIn,
										const char*							sTraceIn)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[%s]: Start dynamic DriverPage (%s)",
						pDevExtIn->sPciLocShortAscii,
						sTraceIn);
					
	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
				ePNDEV_TRACE_CONTEXT_BASIC,
				FALSE,						// logging
				sTraceDetail);

	//------------------------------------------------------------------------
	// enter CriticalSection
	fnEnterCritSec(	pDevExtIn,
					ePNDEV_CRIT_SEC_SYS_PAGE);
	{
	UINT32				lSize			= 0;
	UINT32				lAdrAhb			= MAXUINT32;
	UINT32				lOffsetPhyStart	= 0;
	uPNDEV_HW_DESC_KM*	pHwDescKm		= NULL;

		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifReg.pPtr			= NULL;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0.pPtr	= NULL;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank1.pPtr	= NULL;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank2.pPtr	= NULL;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank3.pPtr	= NULL;
		pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr	= NULL;	

		switch	(ePageIn)
				// dynamic DriverPage
		{
			case eBOARD_EB200__LBU_DYN_PAGE_EMIF_REG:
			{
				// 2MB aligned

				lSize			= ERTEC200_AHB_SIZE__EMIF_REG;
				lAdrAhb			= ERTEC200_AHB_ADR__EMIF_REG;
				lOffsetPhyStart	= 0;

				pHwDescKm		= &pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifReg;

				break;
			}		
			case eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM:
			{
				// 2MB aligned

				lSize			= BOARD_EB200__BAR2_SIZE_LBU_PAGE2;
				lAdrAhb			= (ERTEC200_AHB_ADR__EMIF_SDRAM + (lSegmentAhbIn * (1024*1024*2)));
				lOffsetPhyStart	= (lSegmentAhbIn * (1024*1024*2));

				pHwDescKm		= &pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage;

				break;
			}
			case eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK0:
			{
				// 2MB aligned

				lSize			= BOARD_EB200__BAR2_SIZE_LBU_PAGE2;
				lAdrAhb			= (ERTEC200_AHB_ADR__EMIF_ASYNC_BANK0 + (lSegmentAhbIn * (1024*1024*2)));
				lOffsetPhyStart	= 0;

				pHwDescKm		= &pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0;

				break;
			}
			case eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK1:
			{
				// 2MB aligned

				lSize			= BOARD_EB200__BAR2_SIZE_LBU_PAGE2;
				lAdrAhb			= (ERTEC200_AHB_ADR__EMIF_ASYNC_BANK1 + (lSegmentAhbIn * (1024*1024*2)));
				lOffsetPhyStart	= 0;

				pHwDescKm		= &pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank1;

				break;
			}
			case eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK2:
			{
				// 2MB aligned

				lSize			= BOARD_EB200__BAR2_SIZE_LBU_PAGE2;
				lAdrAhb			= (ERTEC200_AHB_ADR__EMIF_ASYNC_BANK2 + (lSegmentAhbIn * (1024*1024*2)));
				lOffsetPhyStart	= 0;

				pHwDescKm		= &pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank2;

				break;
			}
			case eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK3:
			{
				// 2MB aligned

				lSize			= BOARD_EB200__BAR2_SIZE_LBU_PAGE2;
				lAdrAhb			= (ERTEC200_AHB_ADR__EMIF_ASYNC_BANK3 + (lSegmentAhbIn * (1024*1024*2)));
				lOffsetPhyStart	= 0;

				pHwDescKm		= &pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank3;

				break;
			}
			default:
			{
				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Invalid dynamic DriverPage!",
									pDevExtIn->sPciLocShortAscii);
					
				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);

				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__INVALID_DYN_DRIVER_PAGE;

				break;
			}
		}

		if	(lAdrAhb != MAXUINT32)
			// success
		{
		UINT16 lCfgTmp			= 0;
		UINT16 lAdrAhbHighTmp	= 0;

			// 2MB aligned
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P3_RG_L),
								LOW16_OF_UINT32(BOARD_EB200__BAR3_SIZE_LBU_PAGE3));
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P3_RG_H),
								HIGH16_OF_UINT32(BOARD_EB200__BAR3_SIZE_LBU_PAGE3));
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P3_OF_L),
								LOW16_OF_UINT32(lAdrAhb));
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P3_OF_H),
								HIGH16_OF_UINT32(lAdrAhb));
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P3_CFG),
								ERTEC200_LBU_PX_CFG__32_BIT);

			// ptr = BAR3
			pHwDescKm->pPtr				= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_EB200__BAR3_OFFSET_LBU_PAGE3);
			pHwDescKm->lIndexBar		= 3;
			pHwDescKm->lOffsetBar		= BOARD_EB200__BAR3_OFFSET_LBU_PAGE3;
			pHwDescKm->lSize			= lSize;
			pHwDescKm->lAdrAhb			= lAdrAhb;
			pHwDescKm->lOffsetPhyStart	= lOffsetPhyStart;

			// ensure that new page is active before any access is done to it
			//	- PCIe:
			//		several write accesses (WR1..WRn) to IP1 may be stored before a read access to IP2 is started
			//		->	if last write access is a access to PageReg (IP1) then it is not ensured that new page (IP2, e.g. SDRAM) is active when IP2 is read
			//			(read access is done to a different IP -> IP2-read can be executed before IP1-write)
			//		->	PageReg (IP1) must be read back (-> IP1-write finished) before there is a read access to IP2
			//			(read back last access for ensuring that all previous write accesses are done)
			lCfgTmp = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P3_CFG);

			lAdrAhbHighTmp = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr + ERTEC200_LBU_REG__P3_OF_H);

			if	(lAdrAhbHighTmp != HIGH16_OF_UINT32(lAdrAhb))
				// error
			{
				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Dynamic DriverPage is not set correctly!",
									pDevExtIn->sPciLocShortAscii);
					
				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);

				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__DYN_DRIVER_PAGE_SET_ERROR;
			}
		}
	}
	//------------------------------------------------------------------------
	// leave CriticalSection: see fnBoardIrte_StopDriverPageLbu()
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	LBU: stop using dynamic DriverPage
//************************************************************************

void fnBoardIrte_StopDriverPageLbu(DEVICE_EXTENSION* pDevExtIn)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	//------------------------------------------------------------------------
	// enter CriticalSection: see fnBoardIrte_StartDriverPageLbu()
	{
		// initialize dynamic DriverPage
		fnBoardIrte_InitDriverPageLbu(pDevExtIn);
	}
	//------------------------------------------------------------------------
	// leave CriticalSection
	fnLeaveCritSec(	pDevExtIn,
					ePNDEV_CRIT_SEC_SYS_PAGE);

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[%s]: Stop dynamic DriverPage",
						pDevExtIn->sPciLocShortAscii);
					
	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
				ePNDEV_TRACE_CONTEXT_BASIC,
				FALSE,						// logging
				sTraceDetail);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set command 'STOP'
//************************************************************************

void fnBoardIrte_SetCmdStopArm9(DEVICE_EXTENSION* pDevExtIn)
{
	// write string without \0
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 0) = '!';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 1) = '!';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 2) = 'S';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 3) = 'T';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 4) = 'O';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 5) = 'P';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 6) = '!';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 7) = '!';
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set command 'DUMMY'
//************************************************************************

void fnBoardIrte_SetCmdDummyArm9(DEVICE_EXTENSION* pDevExtIn)
{
	// write string without \0
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 0) = 'D';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 1) = 'U';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 2) = 'M';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 3) = 'M';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 4) = 'Y';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 5) = '1';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 6) = '2';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 7) = '3';
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set command 'GO'
//************************************************************************

void fnBoardIrte_SetCmdGoArm9(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lFwAdrIn)
{
	// write StartAdr of UserFw
	//	- SDRAM OffsetX
	//	- address must be a Arm9 address -> 0x2000.0000 + OffsetX
	//	- format must be LittleEndian, Host-uP=Intel -> automatically LittleEndian
	{
	UINT32* pStartAdrCode = NULL;

		pStartAdrCode = (UINT32*) (pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 4);

		*pStartAdrCode = lFwAdrIn;
	}

	// write string without \0
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 0) = '!';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 1) = 'G';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 2) = 'O';
	*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 3) = '!';
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if signal 'WAITING' of Arm9 is set
//************************************************************************

BOOLEAN fnBoardIrte_SignalWaitingArm9Ok(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult = FALSE;

	if	(	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 0) == 'W')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 1) == 'A')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 2) == 'I')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 3) == 'T')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 4) == 'I')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 5) == 'N')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 6) == 'G')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 7) == '!'))
		// signal 'WAITING' set
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if signal 'RUN2' of Arm9 is set
//************************************************************************

BOOLEAN fnBoardIrte_SignalRun2Arm9Ok(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult = FALSE;

	if	(	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 0) == 'R')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 1) == 'U')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 2) == 'N')
		&&	(*(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr + BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL + 3) == '2'))
		// signal 'RUN2' set
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  enable IRTE modules
//************************************************************************

BOOLEAN fnBoardIrte_EnableIrteModules(	DEVICE_EXTENSION*	pDevExtIn,
										const BOOLEAN		bResetHwIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult				= FALSE;
UINT32	lRegSwitchControl	= 0;
UINT32	lSwitchStatusReset	= 0;
UINT32	lSwitchStatus		= 0;
UINT32	lValueSwitchControl	= 0;

	switch	(pDevExtIn->eAsic)
			// AsicType
	{
		case ePNDEV_ASIC_ERTEC400:
		{
			lRegSwitchControl = IRTE_REG_ERTECX00__SWITCH_CONTROL;

			// default value of SwitchStatus after reset (4 ports)
			lSwitchStatusReset = 0x0000ffff;

			break;
		}
		case ePNDEV_ASIC_ERTEC200:
		{
			lRegSwitchControl = IRTE_REG_ERTECX00__SWITCH_CONTROL;

			// default value of SwitchStatus after reset
			lSwitchStatusReset = 0x00003fff;

			break;
		}
		case ePNDEV_ASIC_SOC1:
		{
			lRegSwitchControl = IRTE_REG_SOC__SWITCH_CONTROL;

			// default value of SwitchStatus after reset (3 ports)
			lSwitchStatusReset = 0x00007fff;

			break;
		}
		default:
		{
			break;
		}
	}

	if	(lSwitchStatusReset != 0)
		// success
	{
	BOOLEAN bResultReset = TRUE;

		if	(bResetHwIn)
			// HwReset done
		{
			// read SwitchStatus
			lSwitchStatus = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__SWITCH_STATUS);

			if	(lSwitchStatus != lSwitchStatusReset)
				// any module enabled (Asic not reset)
			{
				// error
				bResultReset = FALSE;

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Ertec200/400/Soc1 is not reset!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
		}

		if	(bResultReset)
			// success
		{
			// enable desired modules
			//	- Bit10=1:	enable module InterruptController
			//	- Bit0=1:	enable Asic
			{
				lValueSwitchControl = (PNDEV_UINT32_SET_BIT_10 | PNDEV_UINT32_SET_BIT_0);

				WRITE_REG_UINT32(	(pDevExtIn->pIrte + lRegSwitchControl),
									lValueSwitchControl);
			}

			// read SwitchStatus
			lSwitchStatus = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__SWITCH_STATUS);

			if	(lSwitchStatus != (lSwitchStatusReset & (~lValueSwitchControl)))
				// desired modules not enabled
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid SwitchStatus!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  preset ReqList of PciBridge (Soc1)
//************************************************************************

void fnBoardIrte_PresetReqListPciBridge(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 i = 0;

	// Note:
	//	- dependent on previous state a WD-reset may cause an inconsistent state of PciBridge-ReqList
	//	  -> before starting WD fill ReqList with harmless write accesses
	//	- don't use write accesses to IPs which must be initialized after reset (e.g. SDRAM, initialized by SBL)

	for	(i = 0; i < 15; i++)
	{
		WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_MODE),
							pDevExtIn->lValueIrteIcuReg_IntMode);
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize IRTE-ICU
//************************************************************************

void fnBoardIrte_InitIrteIcu(DEVICE_EXTENSION* pDevExtIn)
{
	// default settings of IntModeReg:
	//	- don't clear interrupts by reading
	//	- AsicIntPolarity=LOW

	// Note:
	//	- PCI interrupts INTA#..INTD# are level-triggered and active-low
	//	- Ertec400/200:
	//		INTA# is connected with IRQ1_HP (by hardware)
	//		Default-AsicIntPolarity=LOW already ok (test: setting to HIGH causes cyclic interrupts with IR=0)
	//	- Soc1:
	//		don't change default settings, adapt PCI-ICU to IRTE-ICU

	// save value
	pDevExtIn->lValueIrteIcuReg_IntMode = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_MODE);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize PCI-ICU
//************************************************************************

BOOLEAN fnBoardIrte_InitPciIcu(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
UINT32	lPortMode5_L	= 0;

	// Note:
	//	- GPIO161-output = INTA# of PC -> can be read back by GPIO161-input, level by WiredOr of connected PciDevices
	//	- GpioFct: GPIO161-output can be set by GPIO-OutputReg
	//	- AltFct0: GPIO161-output is set by PCI-ICU

	lPortMode5_L =  READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE5_L);
	lPortMode5_L &= (PNDEV_UINT32_SET_BIT_3 + PNDEV_UINT32_SET_BIT_2);

	if	(lPortMode5_L)
		// GPIO161 not GpioFct
		//	- GpioFct: Bit3:2=00
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Asic is not reset!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
/*
		// test interrupt line by GPIO OutputReg
		//	- AltFct0 must be set (= default after reset)
		{
			// set as output: GPIO161
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__IOCTRL5),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__IOCTRL5) & PNDEV_UINT32_CLR_BIT_1));

			// GPIO161=1 (inactive)
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__OUT_SET5),
								PNDEV_UINT32_SET_BIT_1);

			// read INTA# of PC
			lTmp = (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__IN5) & PNDEV_UINT32_SET_BIT_1);

			// GPIO161=0 (active)
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__OUT_CLEAR5),
								PNDEV_UINT32_SET_BIT_1);

			// read INTA# of PC
			lTmp = (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__IN5) & PNDEV_UINT32_SET_BIT_1);

			// GPIO161=1 (inactive)
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__OUT_SET5),
								PNDEV_UINT32_SET_BIT_1);

			// read INTA# of PC
			lTmp = (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__IN5) & PNDEV_UINT32_SET_BIT_1);
		}
*/

		// set AltFct0 (= function 01) for GPIO161
		//	- AltFct0: Bit3:2=01
		WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE5_L),
							(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE5_L) | PNDEV_UINT32_SET_BIT_2));

		// connect IntSrc=IRTE
		//	- use PCI-ICU-MuxA-Input1
		{
			// Note:
			//	- IRQ1_HP of IRTE is connected to IR_INTA of Logic-IRTE
			//	- IR_INTA of Logic-IRTE is connected to Pin161 of PCI-ICU

			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A1),
								SOC1_PCI_ICU_PIN__IRTE_IRQ1);
		}

		// connect IntSrc=Timer0
		//	- use PCI-ICU-MuxA-Input2
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A2),
								SOC1_PCI_ICU_PIN__TIMER0);
		}

		// connect IntSrc=Timer1
		//	- use PCI-ICU-MuxA-Input3
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A3),
								SOC1_PCI_ICU_PIN__TIMER1);
		}

		// connect IntSrc=Timer2
		//	- use PCI-ICU-MuxA-Input4
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A4),
								SOC1_PCI_ICU_PIN__TIMER2);
		}

		// connect IntSrc=Timer3
		//	- use PCI-ICU-MuxA-Input5
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A5),
								SOC1_PCI_ICU_PIN__TIMER3);
		}

		// connect IntSrc=Timer4
		//	- use PCI-ICU-MuxA-Input6
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A6),
								SOC1_PCI_ICU_PIN__TIMER4);
		}

		// connect IntSrc=Timer5
		//	- use PCI-ICU-MuxA-Input7
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A7),
								SOC1_PCI_ICU_PIN__TIMER5);
		}

		// connect IntSrc=GDMA
		//	- use PCI-ICU-MuxA-Input8
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A8),
								SOC1_PCI_ICU_PIN__GDMA);
		}

		// connect IntSrc=GPIO180
		//	- use PCI-ICU-MuxA-Input9
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A9),
								SOC1_PCI_ICU_PIN__GPIO180);
		}

		// connect IntSrc=GPIO188
		//	- use PCI-ICU-MuxA-Input10
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A10),
								SOC1_PCI_ICU_PIN__GPIO188);
		}

		// connect IntSrc=PROFIBUS-PCI-F1
		//	- use PCI-ICU-MuxA-Input11
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A11),
								SOC1_PCI_ICU_PIN__PB_PCI_F1);
		}

		// connect IntSrc=PROFIBUS-PCI-F2
		//	- use PCI-ICU-MuxA-Input12
		{
			// connect correct Pin
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_A12),
								SOC1_PCI_ICU_PIN__PB_PCI_F2);
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize NAND flash
//************************************************************************

void fnBoardIrte_InitFlashNand(DEVICE_EXTENSION* pDevExtIn)
{
	// Note:
	//	- at NAND boot all flash settings (8Bit device) are already done by PBL
	//	- PBL needs access to NAND flash

	// connect NAND
	{
		// set AltFct0:
		//	- LB_A6:				GPIO102
		//	- LB_A7:				GPIO103
		//	- LB_CS0_N:				GPIO122
		//	- LB_NAND_RD_N:			GPIO130
		//	- LB_NAND_WR_N:			GPIO131
		//	- LB_D0..LB_D15:		GPIO136..GPIO151

		// no AltFct:
		//	- LB_NAND_WP_N:			GPIO129 (WriteProtection)
		//	- LB_NAND_RDY_BUSY_N:	GPIO187

		// set AltFct0 of GPIO103:102
		//	- PortMode3_L:	AltFct of GPIO111:96 can be set
		//					xxxx xxxx xxxx xxxx_0101 xxxx xxxx xxxx
		WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE3_L),
							(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE3_L) | 0x00005000));

		// set AltFct0 of GPIO122
		//	- PortMode3_H:	AltFct of GPIO127:112 can be set
		//					xxxx xxxx xx01 xxxx_xxxx xxxx xxxx xxxx
		WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE3_H),
							(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE3_H) | 0x00100000));

		// set AltFct0 of GPIO143:136 / GPIO131:130
		//	- PortMode4_L:	AltFct of GPIO143:128 can be set
		//					0101 0101 0101 0101_xxxx xxxx 0101 xxxx
		WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE4_L),
							(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE4_L) | 0x55550050));

		// set AltFct0 of GPIO151:144
		//	- PortMode4_H:	AltFct of GPIO159:144 can be set
		//					xxxx xxxx xxxx xxxx_0101 0101 0101 0101
		WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE4_H),
							(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__PORT_MODE4_H) | 0x00005555));
	}

	// initizialize LocalBus
	//	- NAND flash
	//	- 8Bit device
//	WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.pPtr + SOC1_LOCAL_BUS_REG__BANK0), 0x3ffffff8);
	WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.pPtr + SOC1_LOCAL_BUS_REG__BANK0), 0x3ffffff9);	// 16Bit device
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify IRTE
//************************************************************************

BOOLEAN fnBoardIrte_VerifyIrte(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;

	if	(pDevExtIn->pIrte != NULL)
		// Asic with IRTE
	{
	UINT32	lIpVersion	= 0;
	UINT32	lId1		= 0;
	UINT32	lId2		= 0;

		// get IpVersion
		lIpVersion = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION);

		switch	(pDevExtIn->eAsic)
				// AsicType
		{
			case ePNDEV_ASIC_ERTEC400:
			{
				lId1	= IRTE_IP_VERSION__REV5_NO_METAL_FIX;
				lId2	= IRTE_IP_VERSION__REV5_METAL_FIX;

				break;
			}
			case ePNDEV_ASIC_ERTEC200:
			{
				lId1	= IRTE_IP_VERSION__REV6_NO_METAL_FIX;
				lId2	= IRTE_IP_VERSION__REV6_METAL_FIX;

				break;
			}
			case ePNDEV_ASIC_SOC1:
			{
				lId1	= IRTE_IP_VERSION__REV7_NO_METAL_FIX;
				lId2	= IRTE_IP_VERSION__REV7_METAL_FIX;

				break;
			}
			default:
			{
				break;
			}
		}

		if	(lId1 != 0)
			// success
		{
			if	(	(lIpVersion != lId1)
				&&	(lIpVersion != lId2))
				// error
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Error at accessing IRTE (real: 0x%x, expected: 0x%x or 0x%x)!"),
									lIpVersion,
									lId1,
									lId2);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}
						//  - call not neccessary, it does nothing, reason is to implement the same reset procedure
