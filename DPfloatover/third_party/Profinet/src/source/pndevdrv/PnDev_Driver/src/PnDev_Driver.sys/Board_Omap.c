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
/*  F i l e               &F: Board_Omap.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with Omap
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
	#include "Board_Omap.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoardOmap_Reset)
//	#pragma alloc_text (PAGE, fnBoardOmap_ResetHw)
//	#pragma alloc_text (PAGE, fnBoardOmap_InitHw1)
//	#pragma alloc_text (PAGE, fnBoardOmap_InitHw2)
//	#pragma alloc_text (PAGE, fnBoardOmap_VerifyHwAfterReset)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoardOmap_Reset(	DEVICE_EXTENSION*			pDevExtIn,
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

			if	(fnBoardOmap_InitHw1(	pDevExtIn,
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
					bResultHwReset = fnBoardOmap_ResetHw(	pDevExtIn,
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
						if	(fnBoardOmap_InitHw2(	pDevExtIn,
													bResetHw,
													lDimErrorIn,
													sErrorOut))
							// initializing hardware (phase 2) ok
						{
							if	(fnBoardOmap_VerifyHwAfterReset(	pDevExtIn,
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

BOOLEAN fnBoardOmap_InitHw1(DEVICE_EXTENSION*	pDevExtIn,
							const BOOLEAN		bStopDefaultFwIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
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

BOOLEAN fnBoardOmap_ResetHw(DEVICE_EXTENSION*	pDevExtIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;

	// reset running
	pDevExtIn->bResetRunning = TRUE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
		{
		/*UINT32 lValueTmp = 0;
		
			// clear ResetStatus
			WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__PRM_RSTST, 0x0);
	
				
			lValueTmp = READ_REG_UINT32(pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__CM_SHADOW_FREQ_CONFIG1);
			
			if	(!(READ_REG_UINT32(pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__CM_SHADOW_FREQ_CONFIG1)&0x1))
				// Frequency update is requested 
			{
				// initiate warm reset 
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__PRM_RSTCTRL),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__PRM_RSTCTRL)) | 0x1);
	
				// wait 100msec
				if	(fnDelayThread(	pDevExtIn,
									100,
									lDimErrorIn,
									sErrorOut))
					// starting WaitTime of current thread ok
				{
				UINT32 	lResStatus = 0;
	
					// read ResetStatus
					lResStatus 	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__PRM_RSTST);
					lResStatus 	= (lResStatus >> 1) & 1;
	
					if	(lResStatus	== 0x1)
						// warm reset occured
					{
						// clear ResetStatus
						WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__PRM_RSTST, 0x0);
						
						bResult = TRUE;
					}
				}
			}*/
			
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

BOOLEAN fnBoardOmap_InitHw2(DEVICE_EXTENSION*	pDevExtIn,
							const BOOLEAN		bResetHwIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
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
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoardOmap_VerifyHwAfterReset(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
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
//  configure OutboundRegion
//************************************************************************

BOOLEAN fnBoardOmap_ConfigPciMaster(DEVICE_EXTENSION*	pDevExtIn,
									const UINT32		lPhyAdrIn,
									const UINT32		lSizeIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
		{
		volatile UINT8*	pPtrTmp 	= NULL;
		UINT32			lRegionSize = 0;
		
			// set ptr for Configuration
			pPtrTmp = pDevExtIn->uHwResKm.uBoard.as.uOmap.uPcieSs1.pPtr;
			
			// get lower target address
			lRegionSize = READ_REG_UINT32(pPtrTmp + BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET);
			
		    // update ATU region index to region 0
			WRITE_REG_UINT32(pPtrTmp + BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_INDEX, 0x0);
			
			// get max Address
			lRegionSize = 	(	READ_REG_UINT32(pPtrTmp + BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LIMIT) 
							- 	READ_REG_UINT32(pPtrTmp + BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_BASE) + 1);
			
			if 	(lRegionSize < lSizeIn)
				// valid Size
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid RegionSize!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
			UINT32 lTargetAddr = 0;
				
				// get lower target address
				lTargetAddr = READ_REG_UINT32(pPtrTmp + BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET);
				
				if	(lTargetAddr != 0xAABBC000)
					// valid lTargetAddr
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid TargetAddr set by SBL!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					if 	((lPhyAdrIn % 0x1000) != 0)
						// not 4 kByte alligned 
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid PhyAddr (is not 4 kByte alligned)!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{				
						// set physical host address
						WRITE_REG_UINT32(pPtrTmp + BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET, lPhyAdrIn);
									
						bResult = TRUE;
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
