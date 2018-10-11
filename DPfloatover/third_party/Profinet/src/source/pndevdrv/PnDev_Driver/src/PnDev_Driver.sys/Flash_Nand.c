/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the licepolynomial nse          */
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
/*  F i l e               &F: Flash_Nand.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//   
//	Code for basic functions of NAND flash
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
	#include "Flash_Nand.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdReset)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdRead1)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdRead2)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdGetId)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdErase1)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdErase2)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdProgram1)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdProgram2)
//	#pragma alloc_text (PAGE, fnFlashNand_SetCmdStatusRead)
//	#pragma alloc_text (PAGE, fnFlashNand_SetAdrReg)
//	#pragma alloc_text (PAGE, fnFlashNand_Poll)
//	#pragma alloc_text (PAGE, fnFlashNand_StateReady)
//	#pragma alloc_text (PAGE, fnFlashNand_GetFirstGoodBlock)
//	#pragma alloc_text (PAGE, fnFlashNand_CheckBlock)
//	#pragma alloc_text (PAGE, fnFlashNand_EraseBlock)
//	#pragma alloc_text (PAGE, fnFlashNand_EraseChip)
//	#pragma alloc_text (PAGE, fnFlashNand_MarkBadBlock)
//	#pragma alloc_text (PAGE, fnFlashNand_Read)
//	#pragma alloc_text (PAGE, fnFlashNand_ReadBlock)
//	#pragma alloc_text (PAGE, fnFlashNand_ReadPage)
//	#pragma alloc_text (PAGE, fnFlashNand_Program)
//	#pragma alloc_text (PAGE, fnFlashNand_ProgramBlock)
//	#pragma alloc_text (PAGE, fnFlashNand_ProgramPage)
//	#pragma alloc_text (PAGE, fnFlashNand_ExponentiateRscTerm)
//	#pragma alloc_text (PAGE, fnFlashNand_MultplicateRscValues)
//	#pragma alloc_text (PAGE, fnFlashNand_AddRscValues)
//	#pragma alloc_text (PAGE, fnFlashNand_LogarithmiseRscValue)
//	#pragma alloc_text (PAGE, fnFlashNand_GetRscPolynomialValue)
//	#pragma alloc_text (PAGE, fnFlashNand_GetRscSyndrome)
//	#pragma alloc_text (PAGE, fnFlashNand_GetRscErrorVector)
//	#pragma alloc_text (PAGE, fnFlashNand_GetRscErrorPolynomialValue)
//	#pragma alloc_text (PAGE, fnFlashNand_CorrectRscErrors)
//	#pragma alloc_text (PAGE, fnFlashNand_EncodeRscBlock)
//	#pragma alloc_text (PAGE, fnFlashNand_DecodeRscBlock)
#endif

//########################################################################
//  Variables
//########################################################################
//------------------------------------------------------------------------
	// table for calculations in Galois field, F256
	// delogarithm to the basis 2 in F256 (2^index = value), e.g. 2^1 = 2 
	static UINT8 g_lArrayDelog[512] =
	{
		1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38,
		76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192,
		157, 39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35,
		70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222, 161,
		95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 120, 240,
		253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 91, 182, 113, 226,
		217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52, 104, 208, 189, 103, 206,
		129, 31, 62, 124, 248, 237, 199, 147, 59, 118, 236, 197, 151, 51, 102, 204,
		133, 23, 46, 92, 184, 109, 218, 169, 79, 158, 33, 66, 132, 21, 42, 84,
		168, 77, 154, 41, 82, 164, 85, 170, 73, 146, 57, 114, 228, 213, 183, 115,
		230, 209, 191, 99, 198, 145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
		227, 219, 171, 75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65,
		130, 25, 50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166,
		81, 162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9,
		18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 22,
		44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142, 1, // table has double size, otherwise byte arithmetic would be necessary (no %255 oder 0xff scaling)
		2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38,
		76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192,
		157, 39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35,
		70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222, 161,
		95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 120, 240,
		253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 91, 182, 113, 226,
		217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52, 104, 208, 189, 103, 206,
		129, 31, 62, 124, 248, 237, 199, 147, 59, 118, 236, 197, 151, 51, 102, 204,
		133, 23, 46, 92, 184, 109, 218, 169, 79, 158, 33, 66, 132, 21, 42, 84,
		168, 77, 154, 41, 82, 164, 85, 170, 73, 146, 57, 114, 228, 213, 183, 115,
		230, 209, 191, 99, 198, 145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
		227, 219, 171, 75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65,
		130, 25, 50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166,
		81, 162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9,
		18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 22,
		44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142, 1, 2
	};

//------------------------------------------------------------------------
	// logarithm to the basis 2 in F256 (log(index) = value), e.g. log(2) = 1
	static UINT8 g_lArrayLog[256] =
	{
		255, 0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75,
		4, 100, 224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113,
		5, 138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69,
		29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 166,
		6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145, 34, 136,
		54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92, 131, 56, 70, 64,
		30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40, 84, 250, 133, 186, 61,
		202, 94, 155, 159, 10, 21, 121, 43, 78, 212, 229, 172, 115, 243, 167, 87,
		7, 112, 192, 247, 140, 128, 99, 13, 103, 74, 222, 237, 49, 197, 254, 24,
		227, 165, 153, 119, 38, 184, 180, 124, 17, 68, 146, 217, 35, 32, 137, 46,
		55, 63, 209, 91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190, 97,
		242, 86, 211, 171, 20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162,
		31, 45, 67, 216, 183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246,
		108, 161, 59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90,
		203, 89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215,
		79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 88, 175
	};

/************************************************************************
//  D e s c r i p t i o n :
//
// set command reset to CmdReg
*************************************************************************/

BOOLEAN fnFlashNand_SetCmdReset(DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__RESET * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
	
	// poll for completion
	bResult = fnFlashNand_Poll(	pDevExtIn,
								pFlashParIn,
								lDimErrorIn, 
								sErrorOut);
	
	return (bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command read to CmdReg
//************************************************************************

void fnFlashNand_SetCmdRead1(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__READ * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command read to CmdReg  
//************************************************************************

void fnFlashNand_SetCmdRead2(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__SYNC * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command getId to CmdReg  
//************************************************************************

void fnFlashNand_SetCmdGetId(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__GET_ID * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command erase to CmdReg 
//************************************************************************

void fnFlashNand_SetCmdErase1(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__ERASE1 * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command erase to CmdReg 
//************************************************************************

void fnFlashNand_SetCmdErase2(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__ERASE2 * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command program to CmdReg
//************************************************************************

void fnFlashNand_SetCmdProgram1(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__PROGRAM1 * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command program to CmdReg
//************************************************************************

void fnFlashNand_SetCmdProgram2(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__PROGRAM2 * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set command StatusRead to CmdReg 
//************************************************************************

void fnFlashNand_SetCmdStatusRead(uPNDEV_FLASH_PAR* pFlashParIn)
{
	fnFlash_Write(	pFlashParIn->pFlash, 
					pFlashParIn->uNand.lOffsetCmdReg, 
					(FLASH_NAND_CMD_X8__STATUS_READ * pFlashParIn->lDataMultiplier), 
					pFlashParIn->eFlashMode);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// set column address to address pointer
//************************************************************************

BOOLEAN fnFlashNand_SetAdrReg(	const UINT32		lOffsetDestIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const BOOLEAN		bEraseBlockIn,
								const BOOLEAN		bCheckSpareAreaIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult	= FALSE;
UINT16	lPage	= 0; 
UINT16	lColumn	= 0;

	// calculate number of required pages
	lPage = lOffsetDestIn / (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
	
	// calculate number of column in page
	{
		if	(bCheckSpareAreaIn)
			// check SpareArea
		{
			// column=PageSize reaches the first byte in SpareArea
			lColumn = (UINT16) pFlashParIn->uNand.lSizePage; //location of spare area

			bResult = TRUE;
		}
		else
		{
			// is in our case always 0, because we start always on a page start address
			lColumn = (lOffsetDestIn - (lPage * (pFlashParIn->uNand.lSizePage *  pFlashParIn->lDataWidth)));

			if	(lColumn == 0)
				// success
			{
				bResult = TRUE;
			}
			else	// offset not page aligned
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Offset not page aligned (offset=%u)!"),
									lOffsetDestIn);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
		}
	}

	if (bResult)
	{
		// Note:
		//	- at erasing of block ColumnAdr must not be set (see DataSheet)

		if	(!bEraseBlockIn)
			// not erasing of block
		{
			// set bit 0:7 at AdrReg
			fnFlash_Write(pFlashParIn->pFlash, pFlashParIn->uNand.lOffsetAdrReg, 
					((*((UINT8*) &lColumn)) * pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);

			// set bit 8:15 at AdrReg
			fnFlash_Write(pFlashParIn->pFlash, pFlashParIn->uNand.lOffsetAdrReg,
					((*(((UINT8*) &lColumn) + 1)) * pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);
		}

		// set bit 16:23 at AdrReg
		fnFlash_Write(pFlashParIn->pFlash, pFlashParIn->uNand.lOffsetAdrReg,
				((*((UINT8*) &lPage)) * pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);

		// set bit 24:31 at AdrReg
		fnFlash_Write(pFlashParIn->pFlash, pFlashParIn->uNand.lOffsetAdrReg,
				((*(((UINT8*) &lPage) + 1)) * pFlashParIn->lDataMultiplier), pFlashParIn->eFlashMode);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// poll flash device for embedded operation completion
//************************************************************************

BOOLEAN fnFlashNand_Poll(	DEVICE_EXTENSION*	pDevExtIn,
							uPNDEV_FLASH_PAR*	pFlashParIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	lTmp			= 0;
UINT32	i = 0;

	// ensure that command is written before ReadyBusySignal (GPIO) is checked (PostedWrite)
	lTmp = READ_REG_UINT32(pFlashParIn->uNand.pPollReg); 

	// Note:
	//	- after a posted write (command) a read access (GPIO) may follow immediately
	//		-> wait time
	//	- it may take 100nsec until ReadyBusySignal starts to get busy (see DataSheet)
	//		-> ReadyBusySignal must not be checked before this time
	//		-> wait time

	// wait 10msec

	if	(!fnDelayThread(pDevExtIn,
						10,
						lDimErrorIn,
						sErrorOut))
		// error at starting WaitTime
	{
		bErrorDelay	= TRUE;
	}

	if	(!bErrorDelay)
		// success
	{
		// preset
		bErrorTimeout = TRUE;

		for	(i = 0; i < 100; i++)
			// MaxTime=100msec
		{
			if	(!fnFlashNand_StateReady(pFlashParIn))
				// flash not ready
			{
				// try again in 10msec

				if	(!fnDelayThread(pDevExtIn,
									10,
									lDimErrorIn,
									sErrorOut))
					// error at starting WaitTime
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
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout, error at waiting for ReadyState!"),
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
	
	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if flash is ready
//************************************************************************

BOOLEAN fnFlashNand_StateReady(uPNDEV_FLASH_PAR* pFlashParIn)
{
BOOLEAN	bResult	= FALSE;
UINT32	lValue	= 0;

	// Note:
	//	- ReadyBusySignal=0: busy
	//	- ReadyBusySignal=1: ready
	//	- ReadyBusySignal is connected to GPIO187
	//	- GPIO_IN_5 reads GPIO191:160 -> GPIO187=Bit27

	// get GPIO_IN_5
	lValue = READ_REG_UINT32(pFlashParIn->uNand.pPollReg); 

	if	((lValue & PNDEV_UINT32_SET_BIT_27) != 0)
		// flash ready (GPIO187=1)
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// get first GoodBlock in ReservedArea (last 8 blocks)
//************************************************************************

BOOLEAN fnFlashNand_GetFirstGoodBlock(	DEVICE_EXTENSION*	pDevExtIn,
										uPNDEV_FLASH_PAR*	pFlashParIn,
										const UINT32		lOffsetDestIn,
										volatile UINT32*	pOffsetGoodBlockOut,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult	= FALSE;
UINT32	lTmp	= 0;
	
	for	(lTmp = lOffsetDestIn; lTmp < (pFlashParIn->lSizeUsable * pFlashParIn->lDataWidth); lTmp += (pFlashParIn->lSizeSector * pFlashParIn->lDataWidth))
		// lTmp within the ReservedArea
	{
		if	(fnFlashNand_CheckBlock(pDevExtIn,
									pFlashParIn,
									lTmp,
									lDimErrorIn,
									sErrorOut))
			// current block is a GoodBlock
		{
			// success
			bResult = TRUE;
			
			// save address for GoodBlock
			*pOffsetGoodBlockOut = lTmp;
			
			// leave loop
			break;
		}	
	}

	if	(!bResult)
		// error
	{		
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): No GoodBlock available in ReservedArea."),
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	
	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// check if GoodBlock
//************************************************************************

BOOLEAN fnFlashNand_CheckBlock(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lOffsetDestIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN bResultPoll	= FALSE;
UINT32	lValue		= 0;
UINT32	i			= 0;

	if	(lOffsetDestIn % (pFlashParIn->lSizeSector * pFlashParIn->lDataWidth) != 0)
		// offset not block aligned
	{
	_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Offset not block aligned (offset=%u)!"),
							lOffsetDestIn);

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
	UINT32 lOffsetTmp = 0;

		// preset
		bResult = TRUE;

		// start with first page
		lOffsetTmp = lOffsetDestIn;

		for	(i = 0; i < 2; i++)          
			// each 1st and 2nd page per block
		{
			
			if	(!fnFlashNand_SetCmdReset(	pDevExtIn,
											pFlashParIn,
											lDimErrorIn, 
											sErrorOut))
				// error at set command reset to CmdReg
			{
				// error
				bResult = FALSE;
			}
			else
			{
				// set 1st command read to CmdReg
				fnFlashNand_SetCmdRead1(pFlashParIn);
	
				// write the page address to the AdrReg
				if	(fnFlashNand_SetAdrReg(	lOffsetTmp, 
											pFlashParIn,
											FALSE,			// EraseBlock
											TRUE,			// SpareArea
											lDimErrorIn, 
											sErrorOut))
					// success
				{
					// set 2nd command read to CmdReg
					fnFlashNand_SetCmdRead2(pFlashParIn);
	
					// poll for completion
					bResultPoll = fnFlashNand_Poll(	pDevExtIn,
													pFlashParIn,
													lDimErrorIn, 
													sErrorOut);
	
					if	(!bResultPoll)
						// error
					{
						bResult = FALSE;
	
						// leave loop
						break;
					}
					else
					{
						// read flash value
						lValue = fnFlash_Read(	pFlashParIn->pFlash, 
												0x0, 
												pFlashParIn->eFlashMode);
	
						if	(lValue != (0xFF * pFlashParIn->lDataMultiplier))
							// BadBlock
						{
							// error
							bResult = FALSE;
	
							// leave loop
							break;
						}
					}
	
					// increase pointer
					lOffsetTmp += (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
				}
				else	// error
				{
					bResult = FALSE;
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// erase block
//************************************************************************

BOOLEAN fnFlashNand_EraseBlock(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lOffsetDestIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult				= FALSE;
BOOLEAN bResultPoll			= FALSE;
UINT32	lStatusAfterErase	= 1;
_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	if	(fnFlashNand_CheckBlock(pDevExtIn,
								pFlashParIn,
								lOffsetDestIn,
								lDimErrorIn,
								sErrorOut)) 
		// block is a GoodBlock, continue		
	{
		if	(lOffsetDestIn % (pFlashParIn->lSizeSector * pFlashParIn->lDataWidth) != 0)
			// offset not block aligned
		{
			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Offset not block aligned (offset=%u)!"),
								lOffsetDestIn);

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							sErrorDetail,
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			if	(!fnFlashNand_SetCmdReset(	pDevExtIn,
											pFlashParIn,
											lDimErrorIn, 
											sErrorOut))
				// error at set command reset to CmdReg
			{
				bResult = FALSE;
			}
			else
			{
				// set 1st command erase to CmdReg
				fnFlashNand_SetCmdErase1(pFlashParIn);
	
				// write the page address to the AdrReg
				if	(fnFlashNand_SetAdrReg(	lOffsetDestIn, 
											pFlashParIn,
											TRUE,			// EraseBlock
											FALSE,			// SpareArea
											lDimErrorIn, 
											sErrorOut))
					// success
				{
					// set 2cd command erase to CmdReg
					fnFlashNand_SetCmdErase2(pFlashParIn);
	
					// poll for completion
					bResultPoll = fnFlashNand_Poll(	pDevExtIn,
													pFlashParIn,
													lDimErrorIn, 
													sErrorOut);
		
					if	(bResultPoll)
						// success
					{
						// set status read command to CmdReg
						fnFlashNand_SetCmdStatusRead(pFlashParIn);
	
						if	(fnDelayThread(	pDevExtIn,
											1,
											lDimErrorIn,
											sErrorOut))
							// error at starting WaitTime of current thread
						{
							// read StatusRegister from DatReg
							lStatusAfterErase = (0x01* pFlashParIn->lDataMultiplier) & fnFlash_Read(pFlashParIn->pFlash, 
																									0x00, 
																									pFlashParIn->eFlashMode);
						}			
	
						if	(lStatusAfterErase != (FLASH_NAND_STATUS__PROGRAM_ERASE_PASSED * pFlashParIn->lDataMultiplier))
							// erase operation wasn't successful, the block must be highlighted as bad block
						{
							if (fnFlashNand_MarkBadBlock(	pDevExtIn,
															pFlashParIn,
															lOffsetDestIn,
															lDimErrorIn, 
															sErrorOut))
								// BadBlock was successfully marked
							{
								// set ErrorDetail
								_RtlStringCchPrintf(sErrorDetail,
													_countof(sErrorDetail),
													_TEXT("(): Error at erasing NAND flash block (OffsetDest=%u, status=%u), block was marked as BadBlock!"),
													lOffsetDestIn,
													lStatusAfterErase);
	
								// set ErrorString
								fnBuildString(	_TEXT(__FUNCTION__),
												sErrorDetail,
												NULL,
												lDimErrorIn,
												sErrorOut);
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
		}
	}
	else
	{
		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Can't erase a BadBlock: (OffsetDest=%u)!"),
							lOffsetDestIn);

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// erase chip
//************************************************************************

BOOLEAN fnFlashNand_EraseChip(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lOffsetDestIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult	= FALSE;
UINT32	i		= 0;

	for	(i = 0; i <= (pFlashParIn->lSizeUsable * pFlashParIn->lDataWidth); i = i + (pFlashParIn->lSizeSector * pFlashParIn->lDataWidth))
		// each sector
	{
		// erase current block
		bResult = fnFlashNand_EraseBlock(	pDevExtIn,
											pFlashParIn, 
											i, 
											lDimErrorIn, 
											sErrorOut);
		
		if (!bResult)
			// error
		{
			// leave loop
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// program SpareArea when BadBlock occurred 
//************************************************************************

BOOLEAN fnFlashNand_MarkBadBlock(	DEVICE_EXTENSION*	pDevExtIn,
									uPNDEV_FLASH_PAR*	pFlashParIn,
									const UINT32		lOffsetDestIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN bResultPoll = FALSE;
UINT32	lTmp		= 1;
_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	if	(lOffsetDestIn % (pFlashParIn->lSizeSector * pFlashParIn->lDataWidth) != 0)
		// offset not block aligned
	{
		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Offset not block aligned (offset=%u)!"),
							lOffsetDestIn);

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		if	(!fnFlashNand_SetCmdReset(	pDevExtIn,
										pFlashParIn,
										lDimErrorIn, 
										sErrorOut))
			// error at set command reset to CmdReg
		{
			bResult = FALSE;
		}
		else
		{
			// set 1st command program  to CmdReg
			fnFlashNand_SetCmdProgram1(pFlashParIn);
	
			// write the page address to the AdrReg
			if	(fnFlashNand_SetAdrReg(	lOffsetDestIn, 
									pFlashParIn,
									FALSE,			// EraseBlock
									TRUE,			// SpareArea
									lDimErrorIn, 
									sErrorOut))
				// success
			{
				// write flash value, should be not equal 0xFF 
				fnFlash_Write(	pFlashParIn->pFlash, 
								0 , 
								(0x00 * pFlashParIn->lDataMultiplier), 
								pFlashParIn->eFlashMode);
	
				// set 2nd command program to CmdReg
				fnFlashNand_SetCmdProgram2(pFlashParIn);
	
				// poll for completion
				bResultPoll = fnFlashNand_Poll(	pDevExtIn,
												pFlashParIn,
												lDimErrorIn, 
												sErrorOut);
	
				// set statur read cmd  to CmdReg
				fnFlashNand_SetCmdStatusRead(pFlashParIn);
	
				if	(fnDelayThread(	pDevExtIn,
									1,
									lDimErrorIn,
									sErrorOut))
				// error at starting WaitTime of current thread
				{
					// read StatusRegiser from DatReg
					lTmp = (0x01* pFlashParIn->lDataMultiplier) & fnFlash_Read(	pFlashParIn->pFlash, 
																				0x00, 
																				pFlashParIn->eFlashMode);
				}
	
				if	(bResultPoll)
				// success
				{
					if	(lTmp != (FLASH_NAND_STATUS__PROGRAM_ERASE_PASSED * pFlashParIn->lDataMultiplier))      
						// program flash wasn't successful
					{		
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Error at marking BadBlock (OffsetDest=%u, status=%u)!"),
											lOffsetDestIn,
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
						// success
	
						if	(!fnFlashNand_SetCmdReset(	pDevExtIn,
														pFlashParIn,
														lDimErrorIn, 
														sErrorOut))
							// error at set command reset to CmdReg
						{
							bResult = FALSE;
						}
						else
						{
							if	(fnFlashNand_CheckBlock(pDevExtIn,
														pFlashParIn, 
														lOffsetDestIn,
														lDimErrorIn,
														sErrorOut))
								// GoodBlock
							{
								// set ErrorString
								fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Block was successfully marked as BadBlock, but thereafter it wasn't recognized as BadBlock!"),
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
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// read flash 
//************************************************************************

BOOLEAN	fnFlashNand_Read(	DEVICE_EXTENSION*	pDevExtIn,
							uPNDEV_FLASH_PAR*	pFlashParIn,
							const UINT32		lOffsetDestIn,
							const UINT32		lSizeBytesIn,
							const BOOLEAN		bEncodedRscIn,
							const UINT32		lDimErrorIn,
							UINT8*				pDataOut,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult	= FALSE;
	
	if	(lOffsetDestIn == 0)
		// SBL
	{
		// read first flash block
		bResult = fnFlashNand_ReadBlock(pDevExtIn,
										pFlashParIn,
										0,					// OffsetDest
										lSizeBytesIn,
										bEncodedRscIn,
										lDimErrorIn,
										pDataOut, 
										sErrorOut);
	}
	else	// RemanentData
	{
	UINT32 lOffsetGoodBlock = 0;

		// get first GoodBlock in ReservedArea
		bResult = fnFlashNand_GetFirstGoodBlock(pDevExtIn,
												pFlashParIn,
												lOffsetDestIn,
												(UINT32*) &lOffsetGoodBlock,
												lDimErrorIn,
												sErrorOut);

		if	(bResult)
			// GoodBlock was found
		{
			// read block
			bResult = fnFlashNand_ReadBlock(pDevExtIn,
											pFlashParIn,
											lOffsetGoodBlock,
											lSizeBytesIn,
											bEncodedRscIn,
											lDimErrorIn,
											pDataOut, 
											sErrorOut);
		}
	}
	
	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// read block 
//************************************************************************

BOOLEAN	fnFlashNand_ReadBlock(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lOffsetDestIn,
								const UINT32		lSizeBytesIn,
								const BOOLEAN		bEncodedRscIn,
								const UINT32		lDimErrorIn,
								UINT8*				pDataOut,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN bResultTmp		= FALSE;
UINT8*	pDataTmp		= 0;
UINT32	lSizeBytesTmp	= 0;
UINT32	lOffsetDestTmp	= lOffsetDestIn;
UINT32	i				= 0;
UINT32	j				= 0;
UINT32	lPages			= 0;
UINT32	lSize			= 0;
UINT32	lRscBlocks		= 0;
UINT32	lRscErrors		= 0;
UINT8*  pArrayRscData	= NULL;
UINT8*	pArrayTmpEnc	= NULL;
_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	if	(lOffsetDestIn % (pFlashParIn->lSizeSector * pFlashParIn->lDataWidth) != 0)
		// offset not block aligned
	{
		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Offset not block aligned (offset=%u)!"),
							lOffsetDestIn);

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		// Note:
		//	- if ReedSolomonCode was added to data then size at flash is higher than UserSize
		//		-> allocate a temporary buffer to store flash content

		// save pointer 
		pDataTmp = pDataOut;
		
		if	(bEncodedRscIn)
			// ReedSolomonCode was added
		{
			// calculate number of RscBlocks, how many blocks of 249 data bytes should be decoded
			lRscBlocks = lSizeBytesIn /	pFlashParIn->uNand.lSizeRscData;

			if	((lSizeBytesIn % pFlashParIn->uNand.lSizeRscData) != 0)
				// rest exist
			{
				lRscBlocks++;
			}

			// calculate new lSizeBytesIn, for the data + check bytes
			lSizeBytesTmp = lRscBlocks * (pFlashParIn->uNand.lSizeRscData + pFlashParIn->uNand.lSizeRscCheck);
								
			if	(	(pFlashParIn->uNand.lSizeRscData	> 0)
				||	(lRscBlocks							> 0))
				// values are making sense for allocation
			{
				// allocate the memory 
				pArrayRscData = (UINT8*) fnAllocMemNonPaged(((lRscBlocks * (pFlashParIn->uNand.lSizeRscData + pFlashParIn->uNand.lSizeRscCheck)) + 1) * sizeof(UINT8),
															TAG('N','a','n','d')); 
																
				// allocate the memory 
				pArrayTmpEnc = (UINT8*) fnAllocMemNonPaged(	(pFlashParIn->uNand.lSizeRscData * sizeof(UINT8)),
															TAG('N','a','n','d')); 
			}

			if	(	(pArrayRscData	== NULL)
				||	(pArrayTmpEnc	== NULL))
				// error
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Insufficient memory!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				// save start array address for the read operation
				pDataTmp = &pArrayRscData[0];	
			}
		}
		else
		{
			lSizeBytesTmp = lSizeBytesIn;
		}

		// calculate number of page
		lPages = lSizeBytesTmp / (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
		
		if	(lSizeBytesTmp % (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth) != 0)
			// isn't a division without a remainder
		{
			lPages++;
		}

		if	(lSizeBytesTmp <= pFlashParIn->lSizeSector * pFlashParIn->lDataWidth)
			// Length equal or less than block size
		{
			for	(j = 0; j < lPages; j++)
				// each separate page
			{
				if	(j < (lPages - 1))
					// current page is last page
				{
					// complete PageSize (e. g. 2048 bytes)
					lSize = (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
				}
				else
				{
					// rest of buffer in last page (e. g. 255bytes)
					lSize = lSizeBytesTmp - (j * (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth));
				}

				// read Page
				bResultTmp = fnFlashNand_ReadPage(	pDevExtIn,
													pFlashParIn, 
													lOffsetDestTmp, 
													lSize, 
													lDimErrorIn,  
													pDataTmp,
													sErrorOut);
												
				// set next page address
				lOffsetDestTmp = lOffsetDestTmp + (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);

				// increase DataPointer
				pDataTmp = pDataTmp + ((pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth));

				if	(!bResultTmp)
				{
					// error
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): ReadPage wasn't successful!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
					
					// leave loop
					break;
				}
			}

			if	(	bEncodedRscIn 
				&&	bResultTmp)
				// if ReedSolomonCode was added and read operation successful
			{
				for	(i = 0; i < lRscBlocks; i++)
					// each 
				{	
					// decode current RscBlock
					lRscErrors = fnFlashNand_DecodeRscBlock(pFlashParIn, 
															(void*) ((&pArrayRscData[i*(pFlashParIn->uNand.lSizeRscData + pFlashParIn->uNand.lSizeRscCheck)])), 
															(void*) &pArrayTmpEnc[0]);
					
					if	( lRscErrors > (pFlashParIn->uNand.lSizeRscCheck / 2)) 
						// error
					{
						// block includes more than 3 error bytes
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): More than 3 errors were found in RscBlock!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
							
						// leave loop
						break;
					}
					else
					{
						// calculate required length
						{
							if	(i < (lRscBlocks - 1))
							{
								// complete RscBlock (e. g. 249 bytes)
								lSize = pFlashParIn->uNand.lSizeRscData;
							}
							else
							{
								// read rest of data buffer in last RscBlock (e. g. 44 bytes)
								lSize = lSizeBytesIn - (i * pFlashParIn->uNand.lSizeRscData);
							}
						}

						for	(j = 0; j < lSize; j++)
							// lSize of decoded Data
						{
							// copy to output pointer
							*pDataOut++ = pArrayTmpEnc[j];
						}

						bResult = TRUE;
					}
				}

				// deallocate memory of data array
				fnFreeMemNonPaged(	pArrayTmpEnc,
									TAG('N','a','n','d'));
				fnFreeMemNonPaged(	pArrayRscData,
									TAG('N','a','n','d'));		
			}
			else
			{
				// set pointer of output parameter
				pDataOut = pDataTmp;

				bResult = TRUE;
			}
		}
		else
		{
			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Desired size exceeds BlockSizeLength (size=%u)!"),
								lSizeBytesTmp);

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							sErrorDetail,
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// read page
//************************************************************************

BOOLEAN	fnFlashNand_ReadPage(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lOffsetDestIn,
								const UINT32		lSizeBytesIn,
								const UINT32		lDimErrorIn,
								volatile UINT8*		pDataOut,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN bResultPoll = FALSE;
UINT32	i			= 0;
UINT32	lCurValue	= 0;

	if	(!fnFlashNand_SetCmdReset(	pDevExtIn,
									pFlashParIn,
									lDimErrorIn, 
									sErrorOut))
		// error at set command reset to CmdReg
	{
		bResult = FALSE;
	}
	else
	{
		// set 1st command read to CmdReg
		fnFlashNand_SetCmdRead1(pFlashParIn);
	
		// write address to the AdrReg
		if	(fnFlashNand_SetAdrReg(	lOffsetDestIn, 
									pFlashParIn,
									FALSE,			// EraseBlock
									FALSE,			// SpareArea
									lDimErrorIn, 
									sErrorOut))
			// success
		{
			// set 2nd command read to CmdReg
			fnFlashNand_SetCmdRead2(pFlashParIn);
	
			// poll for completion
			bResultPoll = fnFlashNand_Poll(	pDevExtIn,
											pFlashParIn,
											lDimErrorIn, 
											sErrorOut);
								
			if	(bResultPoll)
				// success
			{
				for	(i = 0; i < (lSizeBytesIn / pFlashParIn->lDataWidth) ; i++)
					// read cycles, in the 8bit mode the cycles are equal the lSizeBytesIn, but in the 16bit mode there are only half of the cycles necessary 
				{
					// get current value from Flash
					lCurValue = fnFlash_Read(	pFlashParIn->pFlash, 
												0, 
												pFlashParIn->eFlashMode);
	
					// write current value to pDataOut
					fnFlash_Write(	pDataOut, 
									i * pFlashParIn->lDataWidth, 
									lCurValue, 
									pFlashParIn->eFlashMode);
				}
	
				bResult = TRUE;
			}
			else
			{
				// error string poll
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  program flash
//************************************************************************

BOOLEAN	fnFlashNand_Program(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lOffsetDestIn,
								volatile UINT8*		pDataSrcIn,
								const UINT32		lSizeBytesSrcIn,
								const BOOLEAN		bEraseChipIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult		= FALSE;
BOOLEAN	bAddRsc		= FALSE;

	if	(bEraseChipIn)
		// erase chip
	{
		// call erase chip
		bResult = fnFlashNand_EraseChip(pDevExtIn,
										pFlashParIn,
										0,
										lDimErrorIn,
										sErrorOut);
	}
	
	if	(lOffsetDestIn == 0)
		// program SBL
	{
		// erase used flash block before programming
		//	- must be done if flash is not empty
		bResult = fnFlashNand_EraseBlock(	pDevExtIn,
											pFlashParIn,
											0,
											lDimErrorIn,
											sErrorOut);

		if	(bResult)
			// success
		{
			if	(lSizeBytesSrcIn != 0)
				// SrcData exists
			{
				// program block
				bResult = fnFlashNand_ProgramBlock(	pDevExtIn,
													pFlashParIn,
													0,
													pDataSrcIn,
													lSizeBytesSrcIn,
													bAddRsc,
													lDimErrorIn, 
													sErrorOut);
			}
		}
	}
	else	// flash RemanentData
	{
	UINT32 lOffsetGoodBlock = 0;

		// add ReedSolomonCode					 
		bAddRsc	= TRUE;

		// get first GoodBlock in ReservedArea
		bResult = fnFlashNand_GetFirstGoodBlock(pDevExtIn,
												pFlashParIn,
												lOffsetDestIn,
												(UINT32*) &lOffsetGoodBlock,
												lDimErrorIn,
												sErrorOut);

		if	(bResult)
			// success, usable block was found
		{
			// erase used flash sector before programming
			//	- must be done if flash is not empty
			bResult = fnFlashNand_EraseBlock(	pDevExtIn,
												pFlashParIn,
												lOffsetGoodBlock,
												lDimErrorIn,
												sErrorOut);

			if	(bResult)
				// success
			{
				if	(lSizeBytesSrcIn != 0)
					// SrcData exists
				{
					bResult = fnFlashNand_ProgramBlock(	pDevExtIn,
														pFlashParIn,
														lOffsetGoodBlock,
														pDataSrcIn,
														lSizeBytesSrcIn,
														bAddRsc,
														lDimErrorIn, 
														sErrorOut);
				}
			}
		}
		else
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Not enough space at ReservedBlockArea!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// read block 
//************************************************************************

BOOLEAN	fnFlashNand_ProgramBlock(	DEVICE_EXTENSION*	pDevExtIn,
									uPNDEV_FLASH_PAR*	pFlashParIn,
									const UINT32		lOffsetDestIn,
									volatile UINT8*		pDataIn,
									const UINT32		lSizeBytesIn,
									const BOOLEAN		bAddRscIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult				= FALSE;
UINT32	lSizeBytesTmp		= 0;
UINT32	lOffestDestTmp		= lOffsetDestIn;
UINT32	i 					= 0;
UINT32	lPages				= 0;
UINT32	lSize				= 0;
UINT32	lRscBlocks			= 0;
UINT32	lRscBlockPerPage	= (pFlashParIn->uNand.lSizePage / (pFlashParIn->uNand.lSizeRscData + pFlashParIn->uNand.lSizeRscCheck + 1));
UINT8*	pArrayRscData		= NULL;
_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	if	(lOffsetDestIn % (pFlashParIn->lSizeSector * pFlashParIn->lDataWidth) != 0)
		// offset not block aligned
	{
		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Offset not block aligned (offset=%u)!"),
							lOffsetDestIn);

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		if	(bAddRscIn)
			// do RSC
		{
				// calculate number of RscBlocks
				lRscBlocks = lSizeBytesIn / pFlashParIn->uNand.lSizeRscData;
				
				if	(lSizeBytesIn % pFlashParIn->uNand.lSizeRscData != 0)
					// isn't a division without a remainder
				{
					// add one more RscBlock for rest
					lRscBlocks++;
				}
			
		
			if	(lRscBlocks > 0)
				// greater than 0 for allocation
			{
				// allocate the memory for ArrayRscData
				pArrayRscData = (UINT8*) fnAllocMemNonPaged(((lRscBlocks * (pFlashParIn->uNand.lSizeRscData + pFlashParIn->uNand.lSizeRscCheck))+1) * sizeof(UINT8),
															TAG('N','a','n','d')); 
			}
			
			if	(pArrayRscData == NULL)
				// error
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Insufficient memory!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success

				for	(i = 0; i < lRscBlocks; i++)
					// each RscBlock
				{
					// encode current RscBlock
					fnFlashNand_EncodeRscBlock(	pFlashParIn,
												(void*) pDataIn, 
												(void*) &pArrayRscData[i*(pFlashParIn->uNand.lSizeRscData + pFlashParIn->uNand.lSizeRscCheck)]);

					// increase pointer
					pDataIn = pDataIn + pFlashParIn->uNand.lSizeRscData; 
				}
			
				// save start array address for the program operation
				pDataIn = &pArrayRscData[0];

				// calculate number of flash pages
				lPages = lRscBlocks / (lRscBlockPerPage * pFlashParIn->lDataWidth);
				
				if	(lRscBlocks % (lRscBlockPerPage * pFlashParIn->lDataWidth) != 0)
					//isn't a division without a remainder
				{
					// add one more flash page for the rest
					lPages++;
				}
			
				// calculate required size
				lSizeBytesTmp = lRscBlocks * (pFlashParIn->uNand.lSizeRscData + pFlashParIn->uNand.lSizeRscCheck);
			}
		}
		else
		{
			// calculate number of pages
			lPages = lSizeBytesIn / (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
			
			if	(lSizeBytesIn % (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth) != 0)
				//isn't a division without a remainder
			{
				lPages++;
			}

			// set required size
			lSizeBytesTmp = lSizeBytesIn;
		}

		if	(lSizeBytesTmp <= pFlashParIn->lSizeSector * pFlashParIn->lDataWidth)
			// length equal or less than block size
		{
			for	(i = 0; i < lPages; i++)
				// each separate page
			{
				if (i < (lPages - 1))
					// current page is last page
				{
					// take complete page (e. g. 2048 bytes)
					lSize = (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
				}
				else
				{
					// take rest of buffer in last page (e. g. 255 bytes)
					lSize = lSizeBytesTmp - (i * pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
				}

				// program page
				bResult = fnFlashNand_ProgramPage(	pDevExtIn,
													pFlashParIn, 
													lOffestDestTmp, 
													pDataIn, 
													lSize, 
													lDimErrorIn, 
													sErrorOut);

				// set next page address
				lOffestDestTmp = lOffestDestTmp + (pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth);
				
				// increase data pointer
				pDataIn = pDataIn + ((pFlashParIn->uNand.lSizePage * pFlashParIn->lDataWidth));

				if	(!bResult)
					// error
				{
					// leave loop
					break;
				}
			}
				// finished programing
		}
		else	// error lSizeBytesIn exceeds block size length
		{
			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Desired size exceeds BlockSizeLength (size=%u)!"),
								lSizeBytesIn);

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							sErrorDetail,
							NULL,
							lDimErrorIn,
							sErrorOut);
		}

		if	(bAddRscIn)
			// deallocate memory
		{
			fnFreeMemNonPaged(	pArrayRscData,
								TAG('N','a','n','d'));
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// program page
//************************************************************************

BOOLEAN	fnFlashNand_ProgramPage(DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_FLASH_PAR*	pFlashParIn,
								const UINT32		lOffsetDestIn,
								volatile UINT8*		pDataIn,
								const UINT32		lSizeBytesIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN bResultPoll	= FALSE;
UINT32	i			= 0;
UINT32	lCurValue	= 0;
UINT32	lStatusTmp	= 1;

	if	(!fnFlashNand_SetCmdReset(	pDevExtIn,
									pFlashParIn,
									lDimErrorIn, 
									sErrorOut))
		// error at set command reset to CmdReg
	{
		bResult = FALSE;
	}
	else
	{
		
		// set 1st command program to CmdReg
		fnFlashNand_SetCmdProgram1(pFlashParIn);
	
		// write address to the AdrReg
		if	(fnFlashNand_SetAdrReg(	lOffsetDestIn, 
									pFlashParIn,
									FALSE,			// EraseBlock
									FALSE,			// SpareArea
									lDimErrorIn, 
									sErrorOut))
		{
	
			for	(i = 0; i < (lSizeBytesIn / pFlashParIn->lDataWidth) ; i++)
				//read cycles, in the 8bit mode the cycles are equal the lSizeBytesIn, but in the 16bit mode there are only half of the cycles necessary
			{
				// get current value from buffer
				lCurValue = fnFlash_Read(	pDataIn, 
											(i * pFlashParIn->lDataWidth), 
											pFlashParIn->eFlashMode);
				
				// write datum to flash
				fnFlash_Write(	pFlashParIn->pFlash, 
								0, 
								lCurValue, 
								pFlashParIn->eFlashMode);
			}
	
			// set 2nd command program to CmdReg
			fnFlashNand_SetCmdProgram2(pFlashParIn);
	
			// poll for completion
			bResultPoll = fnFlashNand_Poll(	pDevExtIn,
											pFlashParIn,
											lDimErrorIn, 
											sErrorOut);
	
			if	(bResultPoll)
				// success
			{
				// set status read  to CmdReg
				fnFlashNand_SetCmdStatusRead(pFlashParIn);
	
				if	(fnDelayThread(	pDevExtIn,
									1,
									lDimErrorIn,
									sErrorOut))
				// error at starting WaitTime of current thread
				{
					// read StatusRegister from DatReg
					lStatusTmp = (0x01* pFlashParIn->lDataMultiplier) & fnFlash_Read(	pFlashParIn->pFlash, 
																						0x00, 
																						pFlashParIn->eFlashMode);
				}	
	
				if	(lStatusTmp != (FLASH_NAND_STATUS__PROGRAM_ERASE_PASSED * pFlashParIn->lDataMultiplier))      
					// program operation wasn't successful, the block must be highlighted as bad block
				{
					// error
					
					if (fnFlashNand_MarkBadBlock(	pDevExtIn,
													pFlashParIn,
													lOffsetDestIn,
													lDimErrorIn, 
													sErrorOut))
						// mark bad block was successful
					{
					_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};
	
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Error at programming NAND flash page (status=%u)!"),
											lStatusTmp);
	
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										sErrorDetail,
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
				}
				else
				{
					if	(fnFlashNand_SetCmdReset(	pDevExtIn,
													pFlashParIn,
													lDimErrorIn, 
													sErrorOut))
						// set command reset to CmdReg
					{
						bResult = TRUE;
					}
				}
			}
			else
			{
				// error string poll
			}
		}
	}
	
	return(bResult);
}


/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  these functions are necessary to decode the data and find errors
************************************************************************/

UINT8 fnFlashNand_ExponentiateRscTerm(	const UINT8	lMantIn, 
										const UINT8	lPowerIn)
{
UINT8 lExpt = 0;

	if	(lMantIn == 0)
		// base is null
	{
		// lMantIn ^ 0 is null
		lExpt = 0;
	}
	else
	{
		// get lMantIn ^ lPowerIn in Finite Field
		lExpt = g_lArrayDelog[(g_lArrayLog[lMantIn] * lPowerIn) % 255];
	}

	return (lExpt);
}

/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  multiplication in Finite Field
************************************************************************/

UINT8 fnFlashNand_MultplicateRscValues(	const UINT8	lMul1In, 
										const UINT8 lMul2In)
{
UINT8 lMul3 = 0;

	if	(	(lMul1In == 0) 
		||	(lMul2In == 0))
		// value null
	{
		// multiplikation is null
		lMul3 = 0;
	}
	else
	{
		// get lMul1In * lMul2In in Finite Field
		lMul3 = g_lArrayDelog[(g_lArrayLog[lMul1In] + g_lArrayLog[lMul2In]) % 255];
	}

	return (lMul3);
}

/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  addition in Finite Field
************************************************************************/

UINT8 fnFlashNand_AddRscValues(	const UINT8	lAdd1In, 
								const UINT8	lAdd2In)
{
UINT8 lAdd3 = 0; 

	// lAdd1In + lAdd2In in Finite Field
	lAdd3 = lAdd1In ^ lAdd2In;

	return (lAdd3);
}

/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  logarithm in Finite Field
***********************************************************************/

UINT8 fnFlashNand_LogarithmiseRscValue(const UINT8 lIvtIn)
{
UINT8 lIvtd = 0;
  
	if	(lIvtIn == 0)
	{
		//error
		return (0);  
	}
	
	//  get Log(lIvtIn) in finite field
	lIvtd = g_lArrayDelog[255 - g_lArrayLog[lIvtIn]];

	return (lIvtd);
}

/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  calculate function value of polynomial  in Finite Field
************************************************************************/

UINT8 fnFlashNand_GetRscPolynomialValue(const UINT8	pPIn[], 
										const UINT8 lXIn)
{
UINT8 y	= 0;
UINT8 i	= 0;

	// calcuate polynomial  in Finite Field
	{
		for	(i = 0; i < 255; i++)
			// each term in polynomial 
		{
			// calculate term i and add to y
			y = fnFlashNand_AddRscValues (	y, 
											fnFlashNand_MultplicateRscValues (pPIn[i], fnFlashNand_ExponentiateRscTerm (lXIn, i)));
		}
	}

	return (y);
}

/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  build syndrome polynomial 
************************************************************************/

void fnFlashNand_GetRscSyndrome(const UINT8	pCIn[], 
								UINT8		pSOut[])
{
UINT8 i = 0;
	
	// build syndrome polynomial  for error correction
	{
		pSOut[0] = 0;

		for	(i = 1; i < 7; i++)
			// order 7
		{
			// calculate coefficient i
			pSOut[i] = fnFlashNand_GetRscPolynomialValue(	pCIn, 					// ai
															g_lArrayDelog[i]);		// 2^i

			pSOut[0] = pSOut[0] | pSOut[i];
		}
	}
}

/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  get error vector of RscDataBlock
//  Error matrix, look at equation 9) 
//	
//	calculate the determinate of the error matrix for S1, S2, or S3 errors
//  The number of errors is found, if the result of the corresponding determinate is not equal 0
//
************************************************************************/

void fnFlashNand_GetRscErrorVector(	UINT8	pSIn[], 
									UINT8*	pDetIn, 
									UINT32*	pErrsOut)
{
	// get error vector and detect places
	{
		// calculate det S3
		*pDetIn = fnFlashNand_MultplicateRscValues (pSIn[2], fnFlashNand_MultplicateRscValues (pSIn[4], pSIn[6]));
		*pDetIn = fnFlashNand_AddRscValues (*pDetIn, fnFlashNand_MultplicateRscValues (pSIn[2], fnFlashNand_MultplicateRscValues (pSIn[5], pSIn[5])));
		*pDetIn = fnFlashNand_AddRscValues (*pDetIn, fnFlashNand_MultplicateRscValues (pSIn[6], fnFlashNand_MultplicateRscValues (pSIn[3], pSIn[3])));
		*pDetIn = fnFlashNand_AddRscValues (*pDetIn, fnFlashNand_MultplicateRscValues (pSIn[4], fnFlashNand_MultplicateRscValues (pSIn[4], pSIn[4])));

		*pErrsOut = 3;

		if	(*pDetIn != 0)
			// is determinate of S3 not equal 0
		{
			// detected there 3 errors
			return;
		}

		// calculate det S2
		*pDetIn = fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (pSIn[2], pSIn[4]), 
											fnFlashNand_ExponentiateRscTerm (pSIn[3], 2));
		*pErrsOut = 2;

		if	(*pDetIn != 0)
			// is determinate of S2 not equal 0
		{
			// detected there 2 errors
			return;
		}
		
		// calculate det S1
		*pDetIn = pSIn[1];

		*pErrsOut = 1;

		if	(*pDetIn != 0)
			// is determinate of S1 not equal 0
		{
			// detected there 1 error
			return;
		}
		
		*pErrsOut = 4;
	}
}

/************************************************************************
//  D e s c r i p t i o n :
//	RSC
//  calculate function value at error position
************************************************************************/

void fnFlashNand_GetRscErrorPolynomialValue(UINT8	pPolynomIn[], 
											UINT8	pRootsOut[], 
											UINT32*	pNumsolOut)
{
UINT8 i = 0;
UINT8 j = 0;
UINT8 y = 0;

	*pNumsolOut = 0;
	
	// Note:
	// try each Element of 2^i of Finite Field if it is a root of the error polynomal sigma(x)
	// if it is, save the root and the number of found roots

	for	(i = 0; i < 255; i++)
		// RscBlock
	{
		y = 0;

		for	(j = 0; j < 4; j++)
			// polynomial  order 4
		{
			// calculate function value at j and i
			y = fnFlashNand_AddRscValues (	y, 
											fnFlashNand_MultplicateRscValues (pPolynomIn[j], fnFlashNand_ExponentiateRscTerm (g_lArrayDelog[i], j)));
		}

		if (y == 0)
		{
			pRootsOut[*pNumsolOut] = g_lArrayDelog[i];

			*pNumsolOut = *pNumsolOut + 1;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//  Rsc
//  correct occurred error bytes in RscBlock
//
// Note - Example:
//
//	- Original Message:			c	= (c0, c1, ..., c(n-1))
//	- Read Message:				c~	 = (c~0, c~1, ..., c~(n-1))
//	- Error vector:				f	= (f0, f1, ..., f(n-1))
// 
//	- Error correction:			c~ = c + f						5)
//
//	-							Data bytes	m = 3	
//	-							Check bytes	n = 5
//	-							r		= (102, 88, 5, 25, 114)
//	-							c	 	= (142, 164, 186, 114, 25, 5, 88, 102)
//	- Message with two Errors:	c~		= (255, 164, 186, 114, 25, 7, 88, 102)
//************************************************************************/

UINT32 fnFlashNand_CorrectRscErrors(uPNDEV_FLASH_PAR*	pFlashParIn,
									const UINT8			pEnc_ArrayIn[], 
									UINT8				pSrc_ArrayOut[])
{
UINT8	lArraySyn[7]		= {0}; 
UINT8	lDeter				= 0; 
UINT8	lArrayErrorPoly[4]	= {0}; 
UINT8	lE0					= 0; 
UINT8	lE1					= 0; 
UINT8	lE2					= 0; 
UINT8	lN0					= 0; 
UINT8	lN1					= 0; 
UINT8	lN2					= 0; 
UINT8	lW0					= 0; 
UINT8	lW1					= 0; 
UINT8	lW2					= 0;
UINT8	lX0					= 0; 
UINT8	lArrayRoots[3]		= {0};
UINT32	i					= 0;
UINT32	lSols				= 0;
UINT32	lHyerror			= 0;
UINT32*	lErrcode			= 0;
UINT32	lResult				= 0;

	lErrcode  = (UINT32*) &lHyerror;

	*lErrcode = 0x0000;

	for	(i = 0; i < pFlashParIn->uNand.lSizeRscData; i++)
		// RscBlock
	{
		// copy source array to array for error correction
		pSrc_ArrayOut[i] = pEnc_ArrayIn[254 - i];
	}

	// Note - Example:
	//	- syndrome of Example
	//	- si = c~(s^i)	 	= 255\B7(2^i)^7 + 164\B7(2^i)^6 + 186\B7(2^i)^5 + 114\B7(2^i)^4 + 25\B7(2^i)^3 + 7\B7(2^i)^2 + 88\B7(2^i) + 102
	//	- s1 = c~(2^1) 	= 255\B72^7 + 164\B72^6 + 186\B72^5 + 114\B72^4 + 25\B72^3 + 7\B72^2 + 88\B72 + 102
	//					= 2^(150+7) + 2^(80+6) + 2^(33+5) + 2^(14+4) + 2^(140+3) + 2^(53+2) + 2^(136+1) + 2^181 = 126
	//	- s2 = c~(2^2) 	= 2^(150+14) + 2^(80+12) + 2^(33+10) + 2^(14+8) + 2^(140+6) + 2^(53+4) + 2^(136+2) + 2181 = 161
	//	- s3 = c~(2^3) 	= 2^(150+21) + 2^(80+18) + 2^(33+15) + 2^(14+12) + 2^(140+9) + 2^(53+6) + 2^(136+3) + 2181 = 114
	//	- s4 = c~(2^4) 	= 2^(150+28) + 2^(80+24) + 2^(33+20) + 2^(14+16) + 2^(140+12) + 2^(53+8) + 2^(136+4) + 2181 = 243
	//	- s5 = c~(2^5) 	= 2^(150+35) + 2^(80+30) + 2^(33+25) + 2^(14+20) + 2^(140+15) + 2^(53+10) + 2^(136+5) + 2181 = 137
	// build snydrom polynomial 
	fnFlashNand_GetRscSyndrome(	pEnc_ArrayIn, 
								lArraySyn);

	if	(lArraySyn[0] == 0)
		// no error
	{ 
		lResult = FLASH_NAND_RSC__NO_ERROR;
	}

	// If there are no errors detected,there is something strange going on, so return with errcode 4,
	// else pass the number of errors back via errcode.

	
	// Note - Example:
	//	- determinant of Error Matrix S shows the number of errors
	//	- in our case we have 2 errors, go to case 2
	//	- Polynomial, look at docu equation 9)
	
	// get number of errors
	fnFlashNand_GetRscErrorVector(	lArraySyn, 
									&lDeter, 
									lErrcode);

	if	(*lErrcode == 4)
	{
		lResult = FLASH_NAND_RSC__FOUR_ERROR;
	}

	// at least one error occured, if the number of errors is greater than 3, 
	// there is no possibilty to execute the error correction

	switch (*lErrcode)
	{

		case 1:
		{
			// calculate index in RscBlock
			lX0 = fnFlashNand_MultplicateRscValues(	lArraySyn[2], 
													fnFlashNand_LogarithmiseRscValue (lArraySyn[1]));

			// calculate byte new, which was incorrect
			lW0 = fnFlashNand_MultplicateRscValues(	fnFlashNand_ExponentiateRscTerm (lArraySyn[1], 2), 
													fnFlashNand_LogarithmiseRscValue (lArraySyn[2]));

			// write byte to the array
			if	(g_lArrayLog[lX0] > 5)
			{
				pSrc_ArrayOut[254 - g_lArrayLog[lX0]] = fnFlashNand_AddRscValues(	pSrc_ArrayOut[254 - g_lArrayLog[lX0]], 
																					lW0);
			}

			lResult = FLASH_NAND_RSC__ONE_ERROR;
			break;
		}
		case 2:
		{
			// Note - Example:
			//	- sigma (x) = sigma2 x\B2 + sigma1 x + 1 = 90 x\B2 + 132 x + 1
			//	- calculate coefficients of error polynomial sigma(x)
			{
				lArrayErrorPoly[0] = fnFlashNand_MultplicateRscValues (fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lArraySyn[1], lArraySyn[3]), fnFlashNand_ExponentiateRscTerm (lArraySyn[2], 2)), fnFlashNand_LogarithmiseRscValue (lDeter));
				lArrayErrorPoly[1] = fnFlashNand_MultplicateRscValues (fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lArraySyn[2], lArraySyn[3]), fnFlashNand_MultplicateRscValues (lArraySyn[1], lArraySyn[4])), fnFlashNand_LogarithmiseRscValue (lDeter));
				lArrayErrorPoly[2] = 1;
				lArrayErrorPoly[3] = 0;
			}

			// calculate error index in message with error polynomial sigma(x)
			fnFlashNand_GetRscErrorPolynomialValue(	lArrayErrorPoly, 
													lArrayRoots, 
													&lSols);
			if	(lSols != 2)
			{
				*lErrcode = 4;
				lResult = FLASH_NAND_RSC__OTHER_ERROR;
			}

			// calculate indexes in Arraylog
			{
				lW0 = fnFlashNand_MultplicateRscValues (lArrayErrorPoly[0], lArraySyn[1]);
				lW1 = fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lArrayErrorPoly[0], lArraySyn[2]), fnFlashNand_MultplicateRscValues (lArrayErrorPoly[1], lArraySyn[1]));
				
				// error index in pSrc_ArrayOut
				lN0 = 254 - g_lArrayLog[fnFlashNand_LogarithmiseRscValue (lArrayRoots[0])];
				lN1 = 254 - g_lArrayLog[fnFlashNand_LogarithmiseRscValue (lArrayRoots[1])];
			}

			// calculate right byte for array
			{
				// Note - Example:
				//	- take roots 2 and 7  and calculate error vector F
				//	- s1 = f2\B72^(1\B72) + f7\B72^(1\B77)		f2 = 2
				//	- s2 = f2\B72^(2\B72) + f7\B72^(2\B77)		f7 = 113
				
				lE0 = fnFlashNand_MultplicateRscValues (fnFlashNand_AddRscValues (lW0, fnFlashNand_MultplicateRscValues (lW1, lArrayRoots[0])), fnFlashNand_LogarithmiseRscValue (lArrayErrorPoly[1]));
				lE1 = fnFlashNand_MultplicateRscValues (fnFlashNand_AddRscValues (lW0, fnFlashNand_MultplicateRscValues (lW1, lArrayRoots[1])), fnFlashNand_LogarithmiseRscValue (lArrayErrorPoly[1]));
			}

			// write bytes to the array
			{
				// Note - Example:
				//	- c = c~ + f
				//	- c2 = c~2  + f2 = 7 + 2 = 5
				//	- c7 = c~7  + f7 = 255 + 113 = 142
				//	- right message is c = (142, 164, 186, 114, 25, 5, 88, 102)
				//	-> bytes 2 and 7 are corrected
			
				// Add error fector to read byte and store it at out buffer
				if	(lN0 < pFlashParIn->uNand.lSizeRscData)
				{
					pSrc_ArrayOut[lN0] = fnFlashNand_AddRscValues (pSrc_ArrayOut[lN0], lE0);
				}
				if	(lN1 < pFlashParIn->uNand.lSizeRscData)
				{
					pSrc_ArrayOut[lN1] = fnFlashNand_AddRscValues (pSrc_ArrayOut[lN1], lE1);
				}
			} 
		
			lResult = FLASH_NAND_RSC__TWO_ERROR;
			break;
		}
		case 3:
		{
			// get polynomial 
			{
				lArrayErrorPoly[3] = 1;
				lArrayErrorPoly[2] =									   fnFlashNand_MultplicateRscValues (lArraySyn[1], fnFlashNand_MultplicateRscValues (lArraySyn[4], lArraySyn[6]));
				lArrayErrorPoly[2] = fnFlashNand_AddRscValues (lArrayErrorPoly[2], fnFlashNand_MultplicateRscValues (lArraySyn[1], fnFlashNand_MultplicateRscValues (lArraySyn[5], lArraySyn[5])));
				lArrayErrorPoly[2] = fnFlashNand_AddRscValues (lArrayErrorPoly[2], fnFlashNand_MultplicateRscValues (lArraySyn[5], fnFlashNand_MultplicateRscValues (lArraySyn[3], lArraySyn[3])));
				lArrayErrorPoly[2] = fnFlashNand_AddRscValues (lArrayErrorPoly[2], fnFlashNand_MultplicateRscValues (lArraySyn[3], fnFlashNand_MultplicateRscValues (lArraySyn[4], lArraySyn[4])));
				lArrayErrorPoly[2] = fnFlashNand_AddRscValues (lArrayErrorPoly[2], fnFlashNand_MultplicateRscValues (lArraySyn[2], fnFlashNand_MultplicateRscValues (lArraySyn[5], lArraySyn[4])));
				lArrayErrorPoly[2] = fnFlashNand_AddRscValues (lArrayErrorPoly[2], fnFlashNand_MultplicateRscValues (lArraySyn[2], fnFlashNand_MultplicateRscValues (lArraySyn[3], lArraySyn[6])));
				lArrayErrorPoly[2] =									   fnFlashNand_MultplicateRscValues (lArrayErrorPoly[2], fnFlashNand_LogarithmiseRscValue (lDeter));

				lArrayErrorPoly[1] =									   fnFlashNand_MultplicateRscValues (lArraySyn[1], fnFlashNand_MultplicateRscValues (lArraySyn[3], lArraySyn[6]));
				lArrayErrorPoly[1] = fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_MultplicateRscValues (lArraySyn[1], fnFlashNand_MultplicateRscValues (lArraySyn[5], lArraySyn[4])));
				lArrayErrorPoly[1] = fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_MultplicateRscValues (lArraySyn[4], fnFlashNand_MultplicateRscValues (lArraySyn[3], lArraySyn[3])));
				lArrayErrorPoly[1] = fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_MultplicateRscValues (lArraySyn[2], fnFlashNand_MultplicateRscValues (lArraySyn[4], lArraySyn[4])));
				lArrayErrorPoly[1] = fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_MultplicateRscValues (lArraySyn[2], fnFlashNand_MultplicateRscValues (lArraySyn[3], lArraySyn[5])));
				lArrayErrorPoly[1] = fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_MultplicateRscValues (lArraySyn[2], fnFlashNand_MultplicateRscValues (lArraySyn[2], lArraySyn[6])));
				lArrayErrorPoly[1] =									   fnFlashNand_MultplicateRscValues (lArrayErrorPoly[1], fnFlashNand_LogarithmiseRscValue (lDeter));

				lArrayErrorPoly[0] =									   fnFlashNand_MultplicateRscValues (lArraySyn[2], fnFlashNand_MultplicateRscValues (lArraySyn[3], lArraySyn[4]));
				lArrayErrorPoly[0] = fnFlashNand_AddRscValues (lArrayErrorPoly[0], fnFlashNand_MultplicateRscValues (lArraySyn[3], fnFlashNand_MultplicateRscValues (lArraySyn[2], lArraySyn[4])));
				lArrayErrorPoly[0] = fnFlashNand_AddRscValues (lArrayErrorPoly[0], fnFlashNand_MultplicateRscValues (lArraySyn[3], fnFlashNand_MultplicateRscValues (lArraySyn[5], lArraySyn[1])));
				lArrayErrorPoly[0] = fnFlashNand_AddRscValues (lArrayErrorPoly[0], fnFlashNand_MultplicateRscValues (lArraySyn[4], fnFlashNand_MultplicateRscValues (lArraySyn[4], lArraySyn[1])));
				lArrayErrorPoly[0] = fnFlashNand_AddRscValues (lArrayErrorPoly[0], fnFlashNand_MultplicateRscValues (lArraySyn[3], fnFlashNand_MultplicateRscValues (lArraySyn[3], lArraySyn[3])));
				lArrayErrorPoly[0] = fnFlashNand_AddRscValues (lArrayErrorPoly[0], fnFlashNand_MultplicateRscValues (lArraySyn[2], fnFlashNand_MultplicateRscValues (lArraySyn[2], lArraySyn[5])));
				lArrayErrorPoly[0] =									   fnFlashNand_MultplicateRscValues (lArrayErrorPoly[0], fnFlashNand_LogarithmiseRscValue (lDeter));
			}

			// get roots and errors of polynomial 
			fnFlashNand_GetRscErrorPolynomialValue(	lArrayErrorPoly, 
													lArrayRoots, 
													&lSols);

			if (lSols != 3)
			{
				*lErrcode = 4;

				lResult = FLASH_NAND_RSC__OTHER_ERROR;
			}

			// calculate indexes in Arraylog
			{
				lW0 =							fnFlashNand_MultplicateRscValues (lArrayErrorPoly[0], lArraySyn[1]);
				lW1 = fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lArrayErrorPoly[0], lArraySyn[2]), fnFlashNand_MultplicateRscValues (lArrayErrorPoly[1], lArraySyn[1]));
				lW2 = fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lArrayErrorPoly[0], lArraySyn[3]), fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lArrayErrorPoly[1], lArraySyn[2]), fnFlashNand_MultplicateRscValues (lArrayErrorPoly[2], lArraySyn[1])));

				lN0 = 254 - g_lArrayLog[fnFlashNand_LogarithmiseRscValue (lArrayRoots[0])];
				lN1 = 254 - g_lArrayLog[fnFlashNand_LogarithmiseRscValue (lArrayRoots[1])];
				lN2 = 254 - g_lArrayLog[fnFlashNand_LogarithmiseRscValue (lArrayRoots[2])];
			}

			// calculate right byte for array
			{
				lE0 = fnFlashNand_AddRscValues (lW0, fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lW1, lArrayRoots[0]), fnFlashNand_MultplicateRscValues (lW2, fnFlashNand_ExponentiateRscTerm (lArrayRoots[0], 2))));
				lE0 = fnFlashNand_MultplicateRscValues (lE0, fnFlashNand_LogarithmiseRscValue (fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_ExponentiateRscTerm (lArrayRoots[0], 2))));
				lE1 = fnFlashNand_AddRscValues (lW0, fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lW1, lArrayRoots[1]), fnFlashNand_MultplicateRscValues (lW2, fnFlashNand_ExponentiateRscTerm (lArrayRoots[1], 2))));
				lE1 = fnFlashNand_MultplicateRscValues (lE1, fnFlashNand_LogarithmiseRscValue (fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_ExponentiateRscTerm (lArrayRoots[1], 2))));
				lE2 = fnFlashNand_AddRscValues (lW0, fnFlashNand_AddRscValues (fnFlashNand_MultplicateRscValues (lW1, lArrayRoots[2]), fnFlashNand_MultplicateRscValues (lW2, fnFlashNand_ExponentiateRscTerm (lArrayRoots[2], 2))));
				lE2 = fnFlashNand_MultplicateRscValues (lE2, fnFlashNand_LogarithmiseRscValue (fnFlashNand_AddRscValues (lArrayErrorPoly[1], fnFlashNand_ExponentiateRscTerm (lArrayRoots[2], 2))));
			}

			// write bytes to the array
			{
				if	(lN0 < pFlashParIn->uNand.lSizeRscData)
				{
					pSrc_ArrayOut[lN0] = fnFlashNand_AddRscValues (pSrc_ArrayOut[lN0], lE0);
				}
				if	(lN1 < pFlashParIn->uNand.lSizeRscData)
				{
					pSrc_ArrayOut[lN1] = fnFlashNand_AddRscValues (pSrc_ArrayOut[lN1], lE1);
				}
				if	(lN2 < pFlashParIn->uNand.lSizeRscData)
				{
					pSrc_ArrayOut[lN2] = fnFlashNand_AddRscValues (pSrc_ArrayOut[lN2], lE2);
				}
			}

			lResult = FLASH_NAND_RSC__THREE_ERROR;
			break;
		}
		default:
		{
			lResult = FLASH_NAND_RSC__OTHER_ERROR;
		}
	}

	return(lResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// encode RscBlock
// Example from Doku
// Message with Checkbytes:	c(x) = a(x) * x^k + r(x)		(1)
// Generator polynomial:	g(x) = Generatorpolynomial		(2)
// Check byte polynomial:	r(x) = a(x) * x^k mod g(x)		(3)
//
// Note - Example:
//	- Data bytes	m = 3	
//	- Check bytes	n = 5
//	- Message		a(x) = 186 + 164x + 142 x\B2
//	- g(x)	= x5 + 62x4 + 111x3 + 15 x2 + 48x + 228
//	- r(x) 	= (186 + 164x + 142 x\B2) x5 mod (x^5 + 62x^4 + 111x^3 + 15 x^2 + 48x + 228)
//			= 114x^4 + 25x^3 + 5x^2 + 88x + 102
//	- r		= (102, 88, 5, 25, 114)
//	- c	 	= (142, 164, 186, 114, 25, 5, 88, 102)
//************************************************************************/

void fnFlashNand_EncodeRscBlock(uPNDEV_FLASH_PAR*	pFlashParIn,
								volatile UINT8*		pDataIn, 
								volatile UINT8*		pDataOut)
{
UINT8	lRtmp		= 0;
UINT8	lTtmp		= 0;
UINT8	lCalcChk0	= 0;
UINT8	lCalcChk1	= 0;
UINT8	lCalcChk2	= 0;
UINT8	lCalcChk3	= 0;
UINT8	lCalcChk4	= 0;
UINT8	lCalcChk5	= 0;
UINT32	i			= 0; 

	for	(i = 0; i < pFlashParIn->uNand.lSizeRscData; i++)
		// each RscBlock
	{
		if	(i < pFlashParIn->uNand.lSizeRscData)
			// within RscBlock
		{
			// get coefficient ai of polynomial
			// lRtmp = ai + r5
			lRtmp = (*pDataIn ^ lCalcChk5);

			// read data bytes to out buffer
			*pDataOut++ = *pDataIn++;
		}
		else
		{
			// lRtmp = 0xff + r5
			lRtmp = (0xff ^ lCalcChk5);
		}

		if	(lRtmp == 0x00)
			// all coefficient zero
		{
			// add null to the checksum
			lCalcChk5 = lCalcChk4 ^ 0;
			lCalcChk4 = lCalcChk3 ^ 0;
			lCalcChk3 = lCalcChk2 ^ 0;
			lCalcChk2 = lCalcChk1 ^ 0;
			lCalcChk1 = lCalcChk0 ^ 0;
			lCalcChk0 =  0;
		}
		else
		{
			// calculate and set checksum
			// coeffizients of generator polynomial are used with defiens FLASH_NAND_RSC__GEN_POL
			// r5 = r4 + (ai * g5)  
			// r4 = r3 + (ai * g4)
			// ...

						// get offset for generator polynomial 
			lTtmp = g_lArrayLog[lRtmp];
			
			// calculate and set checksum
			lCalcChk5 = lCalcChk4 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_5 + lTtmp)]);
			lCalcChk4 = lCalcChk3 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_4 + lTtmp)]);
			lCalcChk3 = lCalcChk2 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_3 + lTtmp)]);
			lCalcChk2 = lCalcChk1 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_2 + lTtmp)]);
			lCalcChk1 = lCalcChk0 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_1 + lTtmp)]);
			lCalcChk0 =				g_lArrayDelog[( FLASH_NAND_RSC__GEN_POL_0 + lTtmp)];
		}
	}
	
	// write check sum to the array
	*pDataOut++ = lCalcChk0;
	*pDataOut++ = lCalcChk1;
	*pDataOut++ = lCalcChk2;
	*pDataOut++ = lCalcChk3;
	*pDataOut++ = lCalcChk4;
	*pDataOut++ = lCalcChk5;
}

/************************************************************************
//  D e s c r i p t i o n :
//
// encode RscBlock
************************************************************************/

UINT8 fnFlashNand_DecodeRscBlock(	uPNDEV_FLASH_PAR*	pFlashParIn, 
									UINT8*				pDataIn, 
									UINT8*				pDataOut)
{
UINT8	lRtmp			= 0;
UINT8	lTtmp			= 0;
UINT8	lCalcChk0		= 0;
UINT8	lCalcChk1		= 0;
UINT8	lCalcChk2		= 0;
UINT8	lCalcChk3		= 0;
UINT8	lCalcChk4		= 0;
UINT8	lCalcChk5		= 0;
UINT8	lReadChk0		= 0;
UINT8	lReadChk1		= 0;
UINT8	lReadChk2		= 0;
UINT8	lReadChk3		= 0;
UINT8	lReadChk4		= 0;
UINT8	lReadChk5		= 0;
UINT32	i				= 0;
UINT8	lErrors			= 0;
UINT8*	pDataTmpIn		= pDataIn;
UINT8*	pDataTmpOut		= pDataOut;
UINT8*	pError			= NULL;
UINT8	lArrayEnc [256]	= {0};
UINT8	lArraySrc [256]	= {0};

	for	(i = 0; i < pFlashParIn->uNand.lSizeRscData; i++)
		// decode each RscBlock
	{
		// read data bytes and compute check sum
		{
			*pDataOut = *pDataIn++;
			lRtmp = (*pDataOut ^ lCalcChk5);
			pDataOut++;
		}

		if	(lRtmp == 0x00)
			// all coefficient zero
		{
			// add null to the checksum
			lCalcChk5 = lCalcChk4 ^ 0;
			lCalcChk4 = lCalcChk3 ^ 0;
			lCalcChk3 = lCalcChk2 ^ 0;
			lCalcChk2 = lCalcChk1 ^ 0;
			lCalcChk1 = lCalcChk0 ^ 0;
			lCalcChk0 =  0;
		}
		else
		{
			// calculate and set checksum
			// r5 = r4 ^ (ai * g5)  
			// r4 = r3 ^ (ai * g5)
			// ...

			// get offset for generator polynomial 
			lTtmp = g_lArrayLog[lRtmp];

			// calculate and set checksum
			lCalcChk5 = lCalcChk4 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_5 + lTtmp)%255]);
			lCalcChk4 = lCalcChk3 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_4 + lTtmp)%255]);
			lCalcChk3 = lCalcChk2 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_3 + lTtmp)%255]);
			lCalcChk2 = lCalcChk1 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_2 + lTtmp)%255]);
			lCalcChk1 = lCalcChk0 ^ (g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_1 + lTtmp)%255]);
			lCalcChk0 =				 g_lArrayDelog[(FLASH_NAND_RSC__GEN_POL_0 + lTtmp)%255];
		}
	}
	
	// read check sum from flash
	lReadChk0 = *pDataIn++;
	lReadChk1 = *pDataIn++;
	lReadChk2 = *pDataIn++;
	lReadChk3 = *pDataIn++;
	lReadChk4 = *pDataIn++;
	lReadChk5 = *pDataIn;
	
	if	(	(lReadChk0 == lCalcChk0) 
		&&	(lReadChk1 == lCalcChk1) 
		&&	(lReadChk2 == lCalcChk2) 
		&&	(lReadChk3 == lCalcChk3) 
		&&	(lReadChk4 == lCalcChk4) 
		&&	(lReadChk5 == lCalcChk5))
		// compare check sum from flash with new calculated check sum
	{
		// success
	}
	else
	{
		// error occurred
		// save pointer for correction
		pError = pDataTmpIn;

		for	(i = 0; i < pFlashParIn->uNand.lSizeRscData; i++)
			// RscBlock
		{
			// prepare decoded array and fill up till error byte
			lArrayEnc[254 - i] = *pError++;
		}

		// copy right check sum to decoded array
		lArrayEnc[0] = lReadChk0;
		lArrayEnc[1] = lReadChk1;
		lArrayEnc[2] = lReadChk2;
		lArrayEnc[3] = lReadChk3;
		lArrayEnc[4] = lReadChk4;
		lArrayEnc[5] = lReadChk5;

		// find errors and correct the failure bytes if it's possible
		lErrors &= fnFlashNand_CorrectRscErrors(pFlashParIn,
												lArrayEnc, 
												lArraySrc );

		for	(i = 0;i < pFlashParIn->uNand.lSizeRscData; i++)
			// RscBlock
		{
			// copy corrected data to output pointer
			*pDataTmpOut++ = lArraySrc[i];
		}
	}
	
	// return number of errors
	return (lErrors); 
}

