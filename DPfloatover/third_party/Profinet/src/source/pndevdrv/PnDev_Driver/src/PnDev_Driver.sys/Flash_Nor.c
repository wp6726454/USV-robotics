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
/*  F i l e               &F: Flash_Nor.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for basic functions of NOR flash
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
	#include "Flash_Nor.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnFlashNor_Program)
//	#pragma alloc_text (PAGE, fnFlashNor_ProgramValue)
//	#pragma alloc_text (PAGE, fnFlashNor_EraseChip)
//	#pragma alloc_text (PAGE, fnFlashNor_EraseSector)
//	#pragma alloc_text (PAGE, fnFlashNor_GetStatus)
//	#pragma alloc_text (PAGE, fnFlashNor_Poll)
//	#pragma alloc_text (PAGE, fnFlashNor_SetCmdReset)
//	#pragma alloc_text (PAGE, fnFlashNor_SetCmdAbortWriteToBuffer)
//	#pragma alloc_text (PAGE, fnFlashNor_SetCmdEnterAutoselect)
//	#pragma alloc_text (PAGE, fnFlashNor_SetCmdExitAutoselect)
//	#pragma alloc_text (PAGE, fnFlashNor_SetCmdEraseChip)
//	#pragma alloc_text (PAGE, fnFlashNor_SetCmdEraseSector)
//	#pragma alloc_text (PAGE, fnFlashNor_SetCmdProgram)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  program NOR flash
//************************************************************************

BOOLEAN fnFlashNor_Program(	DEVICE_EXTENSION*	pDevExtIn,
							uPNDEV_FLASH_PAR*	pFlashParIn,
							const UINT32		lOffsetDestIn,
							volatile UINT8*		pDataSrcIn,
							const UINT32		lSizeBytesSrcIn,
							const BOOLEAN		bEraseChipIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN					bResult			= FALSE;
ePNDEV_FLASH_NOR_STATUS eResultErase	= ePNDEV_FLASH_NOR_STATUS_UNKNOWN;
_TCHAR					sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	if	(bEraseChipIn)
		// EraseChip
	{
		// erase chip
		eResultErase = fnFlashNor_EraseChip(pFlashParIn,
											0);
	}
	else
	{
		// erase used flash sector before programming
		//	- must be done if flash is not empty

		// set command SectorErase
		eResultErase = fnFlashNor_EraseSector(	pFlashParIn,
												lOffsetDestIn);
	}

	if	(eResultErase != ePNDEV_FLASH_NOR_STATUS_NOT_BUSY)
		// error
	{
		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Error at erasing flash chip or sector (ErrorCode=%u)!"),
							eResultErase);

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		if	(lSizeBytesSrcIn == 0)
			// no programming of flash
		{
			// success
			bResult = TRUE;
		}
		else
		{
		UINT32 i = 0;

			// preset
			bResult = TRUE;

			while	(i < lSizeBytesSrcIn)
					// not all data programmed
			{
			ePNDEV_FLASH_NOR_STATUS	eStatusFlash	= ePNDEV_FLASH_NOR_STATUS_UNKNOWN;
			UINT32					lCurValue		= 0;

				// get current datum
				//	- use fnFlash_Read() because of DataWidth
				lCurValue = fnFlash_Read(pDataSrcIn, i, pFlashParIn->eFlashMode);

				// program value to flash
				eStatusFlash = fnFlashNor_ProgramValue(	pFlashParIn,
														(lOffsetDestIn + i),
														lCurValue);

				if	(eStatusFlash != ePNDEV_FLASH_NOR_STATUS_NOT_BUSY)
					// error
				{
					bResult = FALSE;

					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Error at programming flash (ErrorCode=%u)!"),
										eResultErase);

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);

					// leave loop
					break;
				}

				// update offset to next datum
				i+= pFlashParIn->lDataWidth;
			}

			if	(bResult)
				// success
			{
				// verify FlashData per byte access
				{
					for	(i = 0; i < lSizeBytesSrcIn; i++)
						// all bytes
					{
					UINT8	lFlashTmp	= 0;
					UINT8	lDataTmp	= 0;

						lFlashTmp	= *(pFlashParIn->pFlash + lOffsetDestIn + i);
						lDataTmp	= *(pDataSrcIn + i);

						if	(lFlashTmp != lDataTmp)
							// error
						{
							bResult = FALSE;

							// set ErrorDetail
							_RtlStringCchPrintf(sErrorDetail,
												_countof(sErrorDetail),
												_TEXT("(): Error at verifying FlashData (i=%u, Flash[i]=0x%x, Data[i]=0x%x)!"),
												i,
												lFlashTmp,
												lDataTmp);

							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											sErrorDetail,
											NULL,
											lDimErrorIn,
											sErrorOut);

							// leave loop
							break;
						}
					}
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// program value to flash (analog lld_ProgramOp)
//************************************************************************

ePNDEV_FLASH_NOR_STATUS fnFlashNor_ProgramValue(uPNDEV_FLASH_PAR*	pFlashParIn,
												const UINT32		lOffsetIn,
												const UINT32		lValueIn)
{
ePNDEV_FLASH_NOR_STATUS	eResult = ePNDEV_FLASH_NOR_STATUS_UNKNOWN;

	// set command program
	fnFlashNor_SetCmdProgram(pFlashParIn);

	// write data
	fnFlash_Write(	pFlashParIn->pFlash,
					lOffsetIn, 
					lValueIn, 
					pFlashParIn->eFlashMode);

	// poll for completion
	eResult = fnFlashNor_Poll(	pFlashParIn,
								lOffsetIn,
								lValueIn,					// expected value read after programming (dependent on DataWidth!)
								ePNDEV_FLASH_NOR_POLL_PGM);
	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// erase chip (analog lld_ChipEraseOp)
//************************************************************************

ePNDEV_FLASH_NOR_STATUS fnFlashNor_EraseChip(	uPNDEV_FLASH_PAR*	pFlashParIn,
												const UINT32		lOffsetIn)
{
ePNDEV_FLASH_NOR_STATUS	eResult				= ePNDEV_FLASH_NOR_STATUS_UNKNOWN;
UINT32					lValueAfterErase	= 0;
UINT32					lTmp				= MAXUINT32;

	// get expected value after erasing
	//	- use fnFlash_Read() because of DataWidth
	lValueAfterErase = fnFlash_Read((volatile UINT8*) &lTmp, 
									0, 
									pFlashParIn->eFlashMode);

	// set command EraseChip
	fnFlashNor_SetCmdEraseChip(pFlashParIn);

	// poll for completion
	eResult = fnFlashNor_Poll(	pFlashParIn,
								lOffsetIn,
								lValueAfterErase,					// expected value read after erasing (dependent on DataWidth!)
								ePNDEV_FLASH_NOR_POLL_CHIP_ERS);

	if	(eResult != ePNDEV_FLASH_NOR_STATUS_NOT_BUSY)
		// error
	{
		// if an error during polling, write command Reset to device

		// set command reset
		fnFlashNor_SetCmdReset(pFlashParIn);
	}
	else
	{
		// verify FlashData per byte access
		{
		UINT32 i = 0;

			for	(i = 0; i < 8; i++)
				// first 8 bytes
			{
			UINT8 lFlashTmp = 0;

				lFlashTmp = *(pFlashParIn->pFlash + lOffsetIn + i);

				if	(lFlashTmp != 0xff)
					// error
				{
					eResult = ePNDEV_FLASH_NOR_STATUS_VERIFY_ERROR;

					// leave loop
					break;
				}
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command SectorErase (analog lld_SectorEraseOp)
//************************************************************************

ePNDEV_FLASH_NOR_STATUS fnFlashNor_EraseSector(	uPNDEV_FLASH_PAR*	pFlashParIn,
												const UINT32		lOffsetIn)
{
ePNDEV_FLASH_NOR_STATUS	eResult				= ePNDEV_FLASH_NOR_STATUS_UNKNOWN;
UINT32					lValueAfterErase	= 0;
UINT32					lTmp				= MAXUINT32;

	// get expected value after erasing
	//	- use fnFlash_Read() because of DataWidth
	lValueAfterErase = fnFlash_Read((volatile UINT8*) &lTmp, 
									0, 
									pFlashParIn->eFlashMode);

	// set command EraseSector
	fnFlashNor_SetCmdEraseSector(	pFlashParIn,
									lOffsetIn);

	// poll for completion
	eResult = fnFlashNor_Poll(	pFlashParIn,
								lOffsetIn,
								lValueAfterErase,				// expected value read after erasing (dependent on DataWidth!)
								ePNDEV_FLASH_NOR_POLL_SEC_ERS);

	if	(eResult == ePNDEV_FLASH_NOR_STATUS_NOT_BUSY)
		// success
	{
		// verify FlashData per byte access
		{
		UINT32 i = 0;

			for	(i = 0; i < 8; i++)
				// first 8 bytes
			{
			UINT8 lFlashTmp = 0;

				lFlashTmp = *(pFlashParIn->pFlash + lOffsetIn + i);

				if	(lFlashTmp != 0xff)
					// error
				{
					eResult = ePNDEV_FLASH_NOR_STATUS_VERIFY_ERROR;

					// leave loop
					break;
				}
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command GetStatus (analog lld_StatusGet)
//************************************************************************

ePNDEV_FLASH_NOR_STATUS fnFlashNor_GetStatus(	uPNDEV_FLASH_PAR*			pFlashParIn,
												const UINT32				lOffsetIn,
												const UINT32				lValueIn,
												const ePNDEV_FLASH_NOR_POLL	ePollTypeIn)
{
ePNDEV_FLASH_NOR_STATUS	eResult = ePNDEV_FLASH_NOR_STATUS_UNKNOWN;

	switch	(pFlashParIn->eFlashTypeReal)
			// FlashType
	{
		case ePNDEV_FLASH_TYPE_NOR_AM29LV040B:
		case ePNDEV_FLASH_TYPE_NOR_AM29LV641:
		{
		volatile UINT32 lRead1 = 0;		// read is done 2x -> volatile!

			// Note:
			//	- for each chip there is a DataBitX -> more than 1 Bit may be set (e.g. DataBit6: 0x0040_0040)!
			//	- multiplication with lDataMultiplier

			// Note:
			//	- during an embedded program algorithm, the device outputs on DQ7 the complement of the datum programmed to DQ7;
			//	  when the algorithm is complete, the device outputs the datum programmed to DQ7
			//	- during an embedded erase algorithm, the device outputs on DQ7 the value 0 (desired lValueIn = 0xff)
			//	- algorithm see DataSheet S.16

			// read 1x
			lRead1 = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);

			if	((lRead1 & (FLASH_NOR_MASK_X8__DQ7 * pFlashParIn->lDataMultiplier)) == (lValueIn & (FLASH_NOR_MASK_X8__DQ7 * pFlashParIn->lDataMultiplier)))
				// DQ7 set to desired value (-> embedded program/erase algorithm completed)
			{
				// success
				eResult = ePNDEV_FLASH_NOR_STATUS_NOT_BUSY;
			}
			else
			{
				if	((lRead1 & (FLASH_NOR_MASK_X8__DQ5 * pFlashParIn->lDataMultiplier)) == 0)
					// timeout DQ5 not set
				{
					// device busy
					eResult = ePNDEV_FLASH_NOR_STATUS_BUSY;
				}
				else	// timeout
				{
					// DQ7 must be rechecked because DQ7 may change simultaneously with DQ5

					// read 1x
					lRead1 = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);

					if	((lRead1 & (FLASH_NOR_MASK_X8__DQ7 * pFlashParIn->lDataMultiplier)) == (lValueIn & (FLASH_NOR_MASK_X8__DQ7 * pFlashParIn->lDataMultiplier)))
						// DQ7 set to desired value (-> embedded program/erase algorithm completed)
					{
						// success
						eResult = ePNDEV_FLASH_NOR_STATUS_NOT_BUSY;
					}
					else
					{
						// timeout condition
						eResult = ePNDEV_FLASH_NOR_STATUS_EXCEEDED_TIME_LIMITS;
					}
				}
			}

			break;
		}
		case ePNDEV_FLASH_TYPE_NOR_S29GL064N:
		case ePNDEV_FLASH_TYPE_NOR_S29GL512P:
		case ePNDEV_FLASH_TYPE_NOR_S29WS128P:
		{
		volatile UINT32	lRead1		= 0;	// read is done 2x -> volatile!
		volatile UINT32	lRead2		= 0;	// read is done 2x -> volatile!
		UINT32			lRead3		= 0;
		UINT32			lToggleDq6	= 0;

			// Note:
			//	- for each chip there is a DataBitX -> more than 1 Bit may be set (e.g. DataBit6: 0x0040_0040)!
			//	- multiplication with lDataMultiplier

			// Note:
			//	- during an embedded program or erase algorithm operation, successive read cycles to any address cause DQ6 to toggle
			//	- algorithm see DataSheet S.37

			// read 3x
			lRead1 = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);
			lRead2 = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);
			lRead3 = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);

			// check for toggling of DataBit6
			//	- toggling between read1 and read2
			//	- toggling between read2 and read3
			//	- check for DataBit6 (DQ6) only
			lToggleDq6 = ((lRead1 ^ lRead2) & (lRead2 ^ lRead3) & (FLASH_NOR_MASK_X8__DQ6 * pFlashParIn->lDataMultiplier));

			if	(lToggleDq6)
				// any DQ6 toggling -> program or erase operation running
			{
				if	(	(ePollTypeIn == ePNDEV_FLASH_NOR_POLL_WRT_BUF_PGM)
					&&	(lRead1 & (lToggleDq6 >> 5)))
					//		(programming by WriteToBuffer operation)
					// AND	(all devices that have DQ6 toggling also have WriteBufferAbort DQ1 set)
					//		(right shifting of a set DQ6 by 5 checks if DQ1 is set too)
				{
					// WriteBuffer Abort condition
					//	- programming by WriteBuffer was aborted (not used by driver)
					eResult = ePNDEV_FLASH_NOR_STATUS_WRITE_BUFFER_ABORT;
				}
				else if	(lRead1 & (lToggleDq6 >> 1))
						// all devices that have DQ6 toggling also have Timeout DQ5 set
						// (right shifting of a set DQ6 by 1 checks if DQ5 is set too)
				{
					// timeout condition
					eResult = ePNDEV_FLASH_NOR_STATUS_EXCEEDED_TIME_LIMITS;
				}
				else
				{
					// device busy
					eResult = ePNDEV_FLASH_NOR_STATUS_BUSY;
				}
			}
			else	// device has completed the program or erase operation
			{
				// DQ2 toggles when the system reads at addresses within those sectors that have been selected for erasure

				lRead1 = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);
				lRead2 = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);

				if	(((lRead1 ^ lRead2) & (FLASH_NOR_MASK_X8__DQ2 * pFlashParIn->lDataMultiplier)) == 0)
					// all devices DQ2 not toggling
				{
					// success
					eResult = ePNDEV_FLASH_NOR_STATUS_NOT_BUSY;
				}
				else if	((lRead1 ^ lRead2) & (FLASH_NOR_MASK_X8__DQ2 * pFlashParIn->lDataMultiplier))
						// all devices DQ2 toggling
				{
					// EraseSuspend condition
					eResult = ePNDEV_FLASH_NOR_STATUS_SUSPEND;
				}
				else
				{
					// device busy -> wait for all devices DQ2 not toggling
					eResult = ePNDEV_FLASH_NOR_STATUS_BUSY;
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// poll flash device for embedded operation completion
//************************************************************************

ePNDEV_FLASH_NOR_STATUS fnFlashNor_Poll(uPNDEV_FLASH_PAR*			pFlashParIn,
										const UINT32				lOffsetIn,
										const UINT32				lValueIn,
										const ePNDEV_FLASH_NOR_POLL	ePollTypeIn)
{
ePNDEV_FLASH_NOR_STATUS	eResult			= ePNDEV_FLASH_NOR_STATUS_UNKNOWN;
ePNDEV_FLASH_NOR_STATUS	eFlashStatus	= ePNDEV_FLASH_NOR_STATUS_UNKNOWN;
UINT32					lCtrPoll		= MAXUINT32;

	do
	{
		// get FlashStatus
		eFlashStatus = fnFlashNor_GetStatus(pFlashParIn,
											lOffsetIn,
											lValueIn,
											ePollTypeIn);
		lCtrPoll--;
	}
	while	(	(eFlashStatus != ePNDEV_FLASH_NOR_STATUS_NOT_BUSY) 
			&&	(lCtrPoll > 0));
			//		(FlashStatus <> NotBusy)
			// AND	(PollCtr > 0)

	if	(eFlashStatus != ePNDEV_FLASH_NOR_STATUS_NOT_BUSY)
		// error
	{
		if	(eFlashStatus == ePNDEV_FLASH_NOR_STATUS_WRITE_BUFFER_ABORT)
			// FlashStatus = WriteBufferAbort
		{
			// set command AbortWriteToBuffer
			fnFlashNor_SetCmdAbortWriteToBuffer(pFlashParIn);
		}
		else 
		{
			// set command reset
			fnFlashNor_SetCmdReset(pFlashParIn);
		}

		// return
		eResult = eFlashStatus;
	}
	else
	{
	UINT32 lRead = 0;

		// Note:
		//	- lValueIn = 32/16/8 Bit dependent on DataWith
		//	- fnFlash_Read() reads 32/16/8 Bit dependent on DataWith
		//	-> no mask necessary

		// read actual data
		lRead = fnFlash_Read(pFlashParIn->pFlash, lOffsetIn, pFlashParIn->eFlashMode);

		if	(lValueIn == lRead)
			// expected value found
		{
			// return
			eResult = ePNDEV_FLASH_NOR_STATUS_NOT_BUSY;
		}
		else
		{
			// return
			eResult = ePNDEV_FLASH_NOR_STATUS_VERIFY_ERROR;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command reset (analog lld_ResetCmd())
//************************************************************************

void fnFlashNor_SetCmdReset(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					(FLASH_NOR_OFFSET__RESET * pFlashParIn->lAdrMultiplierNor), 
					(FLASH_NOR_CMD_X8__RESET * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command WriteToBufferAbortReset (analog lld_WriteBufferAbortResetCmd())
//
// Note:
//	- some flashes provide a programming per WriteToBuffer operation (faster, 16 words)
//************************************************************************

void fnFlashNor_SetCmdAbortWriteToBuffer(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA1 * pFlashParIn->lDataMultiplier),				pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK2 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA2 * pFlashParIn->lDataMultiplier),				pFlashParIn->eFlashMode);

	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__WRITE_BUFFER_ABORT_RESET * pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command EnterAutoselect (analog lld_AutoselectEntryCmd())
//************************************************************************

void fnFlashNor_SetCmdEnterAutoselect(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA1 *	pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK2 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA2 *	pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__AUTOSELECT *	pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command ExitAutoselect (analog lld_AutoselectExitCmd())
//************************************************************************

void fnFlashNor_SetCmdExitAutoselect(uPNDEV_FLASH_PAR* pFlashParIn)
{
	// set command reset
	fnFlashNor_SetCmdReset(pFlashParIn);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command EraseChip (analog lld_ChipEraseCmd())
//************************************************************************

void fnFlashNor_SetCmdEraseChip(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA1 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK2 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA2 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);

	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__ERASE_SETUP *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);

	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA1 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK2 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA2 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);

	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__CHIP_ERASE *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command EraseSector (analog lld_SectorEraseCmd())
//************************************************************************

void fnFlashNor_SetCmdEraseSector(	uPNDEV_FLASH_PAR*	pFlashParIn,
									const UINT32		lOffsetIn)
{
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor),	(FLASH_NOR_CMD_X8__UNLOCK_DATA1 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK2 * pFlashParIn->lAdrMultiplierNor),	(FLASH_NOR_CMD_X8__UNLOCK_DATA2 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);

	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor),	(FLASH_NOR_CMD_X8__ERASE_SETUP *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);

	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor),	(FLASH_NOR_CMD_X8__UNLOCK_DATA1 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK2 * pFlashParIn->lAdrMultiplierNor),	(FLASH_NOR_CMD_X8__UNLOCK_DATA2 *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);

	fnFlash_Write(pFlashParIn->pFlash, lOffsetIn,														(FLASH_NOR_CMD_X8__SECTOR_ERASE *	pFlashParIn->lDataMultiplier),	pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command program (analog lld_ProgramCmd())
//************************************************************************

void fnFlashNor_SetCmdProgram(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA1 * pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK2 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__UNLOCK_DATA2 * pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);

	// write ProgramCommand
	fnFlash_Write(pFlashParIn->pFlash, (FLASH_NOR_OFFSET__UNLOCK1 * pFlashParIn->lAdrMultiplierNor), (FLASH_NOR_CMD_X8__PROGRAM * pFlashParIn->lDataMultiplier),  pFlashParIn->eFlashMode);
}
