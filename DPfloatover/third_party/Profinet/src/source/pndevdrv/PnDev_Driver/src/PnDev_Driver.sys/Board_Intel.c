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
/*  F i l e               &F: Board_Intel.c                             :F&  */
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
	#include "Board_Intel.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoardIntel_Reset)
//	#pragma alloc_text (PAGE, fnBoardIntel_ResetHw)
//	#pragma alloc_text (PAGE, fnBoardIntel_InitHw1)
//	#pragma alloc_text (PAGE, fnBoardIntel_InitHw2)
//	#pragma alloc_text (PAGE, fnBoardIntel_VerifyHwAfterReset)
//	#pragma alloc_text (PAGE, fnBoardIntel_CheckStartupPblSbl)
//	#pragma alloc_text (PAGE, fnBoardIntel_VersionOk)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoardIntel_Reset(	DEVICE_EXTENSION*			pDevExtIn,
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

			if	(fnBoardIntel_InitHw1(	pDevExtIn,
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
					bResultHwReset = fnBoardIntel_ResetHw(	pDevExtIn,
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
						if	(fnBoardIntel_InitHw2(	pDevExtIn,
													bResetHw,
													lDimErrorIn,
													sErrorOut))
							// initializing hardware (phase 2) ok
						{
							if	(fnBoardIntel_VerifyHwAfterReset(pDevExtIn,
																lDimErrorIn,
																sErrorOut))
								// verifying hardware after HwReset ok
							{
								// success
								bResult = TRUE;
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

BOOLEAN fnBoardIntel_InitHw1(DEVICE_EXTENSION*	pDevExtIn,
							const BOOLEAN		bStopDefaultFwIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_I210:		// I210 board
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
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

BOOLEAN fnBoardIntel_ResetHw(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;

	// reset running
	pDevExtIn->bResetRunning = TRUE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_I210:	// I210 board
		{
			// reset I210
			{
			UINT32 lValueTmp = 0;

				lValueTmp = (1 << BOARD_I210__CTRL_RST)|(1 << BOARD_I210__CTRL_LRST)|(1 << BOARD_I210__CTRL_DEV_RST);

				// do SoftwareReset, DeviceReset and LinkReset
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__CTRL),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__CTRL) & BOARD_I210__CTRL_RESERVED_MASK) | lValueTmp);
			}

			// wait 100msec
			if	(fnDelayThread(	pDevExtIn,
								100,
								lDimErrorIn,
								sErrorOut))
				// starting WaitTime of current thread ok
			{
			UINT32 	lResStatus = 0;
			BOOLEAN bResetError = TRUE;

				// read ResetStatus
				lResStatus 	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__CTRL);
				lResStatus 	= (lResStatus >> BOARD_I210__CTRL_RST) & 1;

				if	(lResStatus	== 0x0)
					// success
				{
					// read ResetStatus
					lResStatus  = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__STATUS);
					lResStatus 	= (lResStatus >> BOARD_I210__STATUS_PF_RST_DONE) & 1;

					if	(lResStatus	== 0x1)
						// success
					{
						// read ResetStatus
						lResStatus  = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__EECD);
						lResStatus 	= (lResStatus >> BOARD_I210__EDCD_AUTO_RD) & 1;

						if	(lResStatus	== 0x1)
							// success
						{
							// no Error at SoftwareReset
							bResetError = FALSE;
						}
					}
				}

				if	(!bResetError)
					// success
				{
					bResult = TRUE;
				}
			}

			break;
		}
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
		{
			// reset I82574

			// Step1: set GIO Master Disable
			{
			UINT32 lValueTmp = 0;

				lValueTmp = (1 << BOARD_I82574__CTRL_GIO_MASTER_DISABLE) | BOARD_I82574__CTRL_RESERVED_ONES_MASK;

				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL) & BOARD_I82574__CTRL_RESERVED_ZEROS_MASK) | lValueTmp);
			}

			// Step2: wait 100ms for Status GIO Master Enable == 0
			{
			UINT32 	lValueTmp = 1;
			BOOLEAN bResetError = TRUE;

				if	(fnDelayThread(	pDevExtIn,
									100,
									lDimErrorIn,
									sErrorOut))
					// starting WaitTime of current thread ok
				{
					// read GIO Status
					lValueTmp 	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__STATUS);
					lValueTmp 	= (lValueTmp >> BOARD_I82574__STATUS_GIO_MASTER_ENABLE) & 1;

					if	(lValueTmp	== 0x0)
						// success
					{
						// Step3: initiate reset
						{
						UINT32 lValueTmp = 0;

							lValueTmp = (1 << BOARD_I82574__CTRL_RST) | (1 << BOARD_I82574__CTRL_PHY_RST) | BOARD_I82574__CTRL_RESERVED_ONES_MASK;

							// do SoftwareReset
							WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL),
												(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL) & BOARD_I82574__CTRL_RESERVED_ZEROS_MASK) | lValueTmp);
						}

						// Step4: wait 100msec for self clearing CTRL_RST and check for it as well as for GIO_Master_Disable to ensure board access and NVM read done?
						if	(fnDelayThread(	pDevExtIn,
											100,
											lDimErrorIn,
											sErrorOut))
							// starting WaitTime of current thread ok
						{
						UINT32 	lResStatus = 0;
						UINT32 	lResStatusAutoRD = 0;
						UINT32 	lGIOStatus = 1;

							// read ResetStatus
							lResStatus 	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL);
							lResStatus 	= (lResStatus >> BOARD_I82574__CTRL_RST) & 1;

							// read GIOStatus
							lGIOStatus 	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL);
							lGIOStatus 	= (lGIOStatus >> BOARD_I82574__CTRL_GIO_MASTER_DISABLE) & 1;

							// read ResetStatus2 - NVM Auto Read done?
							lResStatusAutoRD  = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__EEC);
							lResStatusAutoRD  = (lResStatusAutoRD >> BOARD_I82574__EEC_AUTO_RD) & 1;

							if	(	lResStatus			== 0x0 &&
									lGIOStatus			== 0x0 &&
									lResStatusAutoRD	== 0x1)
								// success
							{
								// no Error at SoftwareReset
								bResetError = FALSE;
							}

							if	(!bResetError)
								// success
							{
								bResult = TRUE;
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
//  initialize hardware (phase 2)
//************************************************************************

BOOLEAN fnBoardIntel_InitHw2(DEVICE_EXTENSION*	pDevExtIn,
							const BOOLEAN		bResetHwIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN bResult			= FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_I210:		// Intel I210 board
		{
			// set status DriverLoaded
			WRITE_REG_UINT32(	pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__CTRL_EXT,
								((READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__CTRL_EXT) & BOARD_I210__CTRL_EXT_RESERVED_MASK) | BOARD_I210__CTRL_EXT_DRV_LOAD));

			// success
			bResult = TRUE;

			break;
		}
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
		{
			// set status DriverLoaded
			WRITE_REG_UINT32(	pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL_EXT,
								((READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL_EXT) & BOARD_I82574__CTRL_EXT_RESERVED_MASK) | BOARD_I82574__CTRL_EXT_DRV_LOAD));

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
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoardIntel_VerifyHwAfterReset(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_I210:		// Intel I210 board
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
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
