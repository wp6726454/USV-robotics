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
/*  F i l e               &F: Board_Micrel.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with Micrel
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Kerl			xx
/*****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Board_Micrel.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoardMicrel_Reset)
//	#pragma alloc_text (PAGE, fnBoardMicrel_ResetHw)
//	#pragma alloc_text (PAGE, fnBoardMicrel_InitHw1)
//	#pragma alloc_text (PAGE, fnBoardMicrel_InitHw2)
//	#pragma alloc_text (PAGE, fnBoardMicrel_VerifyHwAfterReset)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoardMicrel_Reset(	DEVICE_EXTENSION*			pDevExtIn,
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

			if	(fnBoardMicrel_InitHw1(	pDevExtIn,
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
					bResultHwReset = fnBoardMicrel_ResetHw(	pDevExtIn,
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
						if	(fnBoardMicrel_InitHw2(	pDevExtIn,
													bResetHw,
													lDimErrorIn,
													sErrorOut))
							// initializing hardware (phase 2) ok
						{
							if	(fnBoardMicrel_VerifyHwAfterReset(	pDevExtIn,
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

BOOLEAN fnBoardMicrel_InitHw1(	DEVICE_EXTENSION*	pDevExtIn,
								const BOOLEAN		bStopDefaultFwIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_MICREL_KSZ8841:		// Micrel KSZ8841 board
		{
			// success
			bResult = TRUE;

			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8842:		// Micrel KSZ8842 board
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

BOOLEAN fnBoardMicrel_ResetHw(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;

	// reset running
	pDevExtIn->bResetRunning = TRUE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
		{
		UINT16 lTmpValue = 0;
		
			// set Reset to active
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR),
								(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR)) | PNDEV_UINT32_SET_BIT_0);
			
			// PciRead for PostedWrite
			lTmpValue = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR);
			  
			// wait 100msec
			if	(fnDelayThread(	pDevExtIn,
								100,
								lDimErrorIn,
								sErrorOut))
				// starting WaitTime of current thread ok
			{
				// set Reset to inactive
				WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR),
									(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR)) & PNDEV_UINT32_CLR_BIT_0);
			  
				// PciRead for PostedWrite
				lTmpValue = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR);

				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
		{
		UINT16 lTmpValue = 0;
		
			// set Reset to active
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR),
								(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR)) | PNDEV_UINT32_SET_BIT_0);
			
			// PciRead for PostedWrite
			lTmpValue = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR);
			  
			// wait 100msec
			if	(fnDelayThread(	pDevExtIn,
								100,
								lDimErrorIn,
								sErrorOut))
				// starting WaitTime of current thread ok
			{
				// set Reset to inactive
				WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR),
									(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR)) & PNDEV_UINT32_CLR_BIT_0);
			  
				// PciRead for PostedWrite
				lTmpValue = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__GRR);

				bResult = TRUE;
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

BOOLEAN fnBoardMicrel_InitHw2(	DEVICE_EXTENSION*	pDevExtIn,
								const BOOLEAN		bResetHwIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
		{
		UINT16 lTmpValue = 0;
		
			// set phy power off Port1
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P1CR4),
								(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P1CR4)) | PNDEV_UINT32_SET_BIT_11);
			
			// read phy power
			lTmpValue = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P1CR4);

			// success
			bResult = TRUE;

			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
		{
		UINT16 lTmpValue = 0;
		
			// set phy power off Port1
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P1CR4),
								(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P1CR4)) | PNDEV_UINT32_SET_BIT_11);
			
			// read phy power
			lTmpValue = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P1CR4);
			

			// set phy power off Port2
			WRITE_REG_UINT16(	(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P2CR4),
								(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P2CR4)) | PNDEV_UINT32_SET_BIT_11);
			
			// read phy power
			lTmpValue = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__P2CR4);

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
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoardMicrel_VerifyHwAfterReset(	DEVICE_EXTENSION*	pDevExtIn,
											const UINT32		lDimErrorIn,
											_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8842 board
		case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
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
//  get MacAddress
//************************************************************************

BOOLEAN fnBoardMicrel_GetMacAdress(	DEVICE_EXTENSION*	pDevExtIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN 		bResult = FALSE;
volatile UINT8* pBase 	= NULL;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
		{
			pBase = pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr;
			pDevExtIn->uRemanentData.lCtrMacAdr = 2;
					
			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
		{
			pBase = pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr;
			pDevExtIn->uRemanentData.lCtrMacAdr = 3;

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
		
	if (	(pDevExtIn->uRemanentData.lCtrMacAdr != 0) 
		&&	(pBase != NULL))
		// valid ptr and CtrMac
	{
	UINT16 	lTmpValue0	= 0;
	UINT16 	lTmpValue1	= 0;
	UINT16 	lTmpValue2	= 0;
	UINT32	i 			= 0;
	
	// Read mac address low
	lTmpValue0 = READ_REG_UINT16(pBase + BOARD_MICREL_KSZ88XX__MARL);
	
	// Read mac address middle
	lTmpValue1 = READ_REG_UINT16(pBase + BOARD_MICREL_KSZ88XX__MARM);
	
	// Read mac address high
	lTmpValue2 = READ_REG_UINT16(pBase + BOARD_MICREL_KSZ88XX__MARH);
	
	
		for	(i = 0; i < pDevExtIn->uRemanentData.lCtrMacAdr; i++)
			// generate port addresses
		{		
			pDevExtIn->uRemanentData.lArrayMacAdr[i][4] = (lTmpValue0>>8)&0xFF; 
			pDevExtIn->uRemanentData.lArrayMacAdr[i][5] = lTmpValue0 & 0xFF;
				
			pDevExtIn->uRemanentData.lArrayMacAdr[i][2] = (lTmpValue1>>8)&0xFF; 
			pDevExtIn->uRemanentData.lArrayMacAdr[i][3] = lTmpValue1 & 0xFF;
				
			pDevExtIn->uRemanentData.lArrayMacAdr[i][0] = (lTmpValue2>>8)&0xFF; 
			pDevExtIn->uRemanentData.lArrayMacAdr[i][1] = lTmpValue2 & 0xFF;
		}
		
		for	(i = 1; i < pDevExtIn->uRemanentData.lCtrMacAdr; i++)
			// generate port addresses
		{
			pDevExtIn->uRemanentData.lArrayMacAdr[i][5] = (pDevExtIn->uRemanentData.lArrayMacAdr[i-1][5] + 1);
		}
		
		// success
		bResult = TRUE;
	}
	else
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Invalid lCtrMacAdr or BasePointer!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
		
	}

	return(bResult);
}
