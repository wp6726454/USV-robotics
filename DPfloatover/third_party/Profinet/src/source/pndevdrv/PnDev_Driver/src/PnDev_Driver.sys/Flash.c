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
/*  F i l e               &F: Flash.c                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for basic functions of Flash
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
	#include "Flash.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnFlash_VerifyAction)
//	#pragma alloc_text (PAGE, fnFlash_VerifyChip)
//	#pragma alloc_text (PAGE, fnFlash_Write)
//	#pragma alloc_text (PAGE, fnFlash_Read)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify FlashAction
//************************************************************************

BOOLEAN fnFlash_VerifyAction(	DEVICE_EXTENSION*			pDevExtIn,
								const ePNDEV_FLASH_ACTION	eFlashActionIn,
								const ePNDEV_FLASH_INFO		eFlashInfoIn,
								const UINT32				lDimErrorIn,
								_TCHAR*						sErrorOut)
{
BOOLEAN bResult	= FALSE;

	switch	(eFlashActionIn)
			// action
	{
		case ePNDEV_FLASH_ACTION_ERASE_CHIP:
		{
			if	(eFlashInfoIn & ePNDEV_FLASH_INFO_SBL_EXIST)
				// SBL exist
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Erasing of flash chip not allowed because of existing SBL!"),
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
		case ePNDEV_FLASH_ACTION_ERASE_SECTOR_DATA:
		{
			if	(eFlashInfoIn & ePNDEV_FLASH_INFO_BOARD_DATA_EXIST)
				// BoardData exist
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Erasing of flash sector not allowed because of existing BoardData!"),
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
		case ePNDEV_FLASH_ACTION_PROGRAM_DATA:
		{
			// success
			bResult = TRUE;

			break;
		}
		case ePNDEV_FLASH_ACTION_PROGRAM_SBL:
		{
			switch	(pDevExtIn->eBoard)
					// BoardType
			{
				case ePNDEV_BOARD_CP1616:	// CP1616 board
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Please contact product owner of CP1616!"),
									NULL,
									lDimErrorIn,
									sErrorOut);

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

					break;
				}
				case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
				case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
				{
					// success
					bResult = TRUE;

					break;
				}
				case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
                case ePNDEV_BOARD_FPGA1_HERA:       // FPGA1-HERA board
				case ePNDEV_BOARD_EB200P:			// EB200P board
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): No SBL required!"),
									NULL,
									lDimErrorIn,
									sErrorOut);

					break;
				}
				default:
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid BoardType!"),
									NULL,
									lDimErrorIn,
									sErrorOut);

					break;
				}
			}

			break;
		}
		default:
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid FlashAction!"),
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
//  verify flash chip
//************************************************************************

BOOLEAN fnFlash_VerifyChip(	DEVICE_EXTENSION*	pDevExtIn,
							const UINT32		lDimErrorIn,
							uPNDEV_FLASH_PAR*	pFlashParInOut,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;
_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};
UINT32	i = 0;

	for	(i = 0; i < PNDEV_DIM_ARRAY_FLASH_TYPE; i++)
		// all possible FlashTypes
	{
		switch	(pFlashParInOut->eFlashTypePos[i])
				// FlashType
		{
			case ePNDEV_FLASH_TYPE_NOR_AM29LV040B:
			{
			UINT32	lManufactureId	= 0;
			UINT32	lDeviceId		= 0;

				// set command reset
				fnFlashNor_SetCmdReset(pFlashParInOut);

				// set command AutoselectEntry
				fnFlashNor_SetCmdEnterAutoselect(pFlashParInOut);

				// get data
				lManufactureId	= fnFlash_Read(pFlashParInOut->pFlash, (FLASH_NOR_OFFSET__MANUFACTURE_ID *	pFlashParInOut->lAdrMultiplierNor), pFlashParInOut->eFlashMode);
				lDeviceId		= fnFlash_Read(pFlashParInOut->pFlash, (FLASH_NOR_OFFSET__DEVICE_ID *		pFlashParInOut->lAdrMultiplierNor), pFlashParInOut->eFlashMode);

				// set command AutoselectExit
				fnFlashNor_SetCmdExitAutoselect(pFlashParInOut);

				if	(	(lManufactureId	!= (0x01 * pFlashParInOut->lDataMultiplier))
					||	(lDeviceId		!= (0x4f * pFlashParInOut->lDataMultiplier)))
					// error
				{
					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid flash identification of AM29LV040B (ManufactureId: 0x%x, DeviceId: 0x%x)!"),
										lManufactureId,
										lDeviceId);

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
			case ePNDEV_FLASH_TYPE_NOR_AM29LV641:
			{
			UINT32	lManufactureId	= 0;
			UINT32	lDeviceId		= 0;

				// set command reset
				fnFlashNor_SetCmdReset(pFlashParInOut);

				// set command AutoselectEntry
				fnFlashNor_SetCmdEnterAutoselect(pFlashParInOut);

				// get data
				lManufactureId	= fnFlash_Read(pFlashParInOut->pFlash, (FLASH_NOR_OFFSET__MANUFACTURE_ID *	pFlashParInOut->lAdrMultiplierNor), pFlashParInOut->eFlashMode);
				lDeviceId		= fnFlash_Read(pFlashParInOut->pFlash, (FLASH_NOR_OFFSET__DEVICE_ID *		pFlashParInOut->lAdrMultiplierNor), pFlashParInOut->eFlashMode);

				// set command AutoselectExit
				fnFlashNor_SetCmdExitAutoselect(pFlashParInOut);

				if	(	(lManufactureId	!= (0x01   * pFlashParInOut->lDataMultiplier))
					||	(lDeviceId		!= (0x22d7 * pFlashParInOut->lDataMultiplier)))
					// error
				{
					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid flash identification of AM29LV641 (ManufactureId: 0x%x, DeviceId: 0x%x)!"),
										lManufactureId,
										lDeviceId);

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
			case ePNDEV_FLASH_TYPE_NOR_S29GL064N:
			case ePNDEV_FLASH_TYPE_NOR_S29GL512P:
			case ePNDEV_FLASH_TYPE_NOR_S29WS128P:
			{
			UINT32	lManufactureId	= 0;
			UINT32	lDeviceId_Word1	= 0;

				// set command reset
				fnFlashNor_SetCmdReset(pFlashParInOut);

				// set command AutoselectEntry
				fnFlashNor_SetCmdEnterAutoselect(pFlashParInOut);

				// get data
				lManufactureId	= fnFlash_Read(pFlashParInOut->pFlash, (FLASH_NOR_OFFSET__MANUFACTURE_ID *	pFlashParInOut->lAdrMultiplierNor), pFlashParInOut->eFlashMode);
				lDeviceId_Word1	= fnFlash_Read(pFlashParInOut->pFlash, (FLASH_NOR_OFFSET__DEVICE_ID *		pFlashParInOut->lAdrMultiplierNor), pFlashParInOut->eFlashMode);

				// set command AutoselectExit
				fnFlashNor_SetCmdExitAutoselect(pFlashParInOut);

				if	(	(lManufactureId		!= (0x0001 * pFlashParInOut->lDataMultiplier))
					||	(lDeviceId_Word1	!= (0x227e * pFlashParInOut->lDataMultiplier)))
					// error
				{
					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid flash identification of S29GL064N/S29GL512P/S29WS128P (ManufactureId: 0x%x, DeviceId_Word1: 0x%x)!"),
										lManufactureId,
										lDeviceId_Word1);

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
			case ePNDEV_FLASH_TYPE_NAND_K91G08U0B:
			{
			UINT32	lManufactureId	= 0;
			UINT32	lDeviceId		= 0;
			BOOLEAN	bResultPoll		= FALSE;
		
				// set commands
				bResultPoll = fnFlashNand_SetCmdReset(	pDevExtIn,
														pFlashParInOut,
														lDimErrorIn, 
														sErrorOut);

				if	(bResultPoll)
					// success
				{
					fnFlashNand_SetCmdGetId(pFlashParInOut);
	
					// write address
					fnFlash_Write(	pFlashParInOut->pFlash, 
									pFlashParInOut->uNand.lOffsetAdrReg, 
									(0 * pFlashParInOut->lDataMultiplier), 
									pFlashParInOut->eFlashMode);
				
					// get data
					lManufactureId	= fnFlash_Read(pFlashParInOut->pFlash, 0 , pFlashParInOut->eFlashMode);
					lDeviceId		= fnFlash_Read(pFlashParInOut->pFlash, 0 , pFlashParInOut->eFlashMode);				
	
					if	(	(lManufactureId	!= (0xec * pFlashParInOut->lDataMultiplier))
						||	(lDeviceId		!= (0xf1 * pFlashParInOut->lDataMultiplier)))
						// error
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid flash identification of K91G08U0B (ManufactureId: 0x%x, lDeviceId: 0x%x)!"),
											lManufactureId,
											lDeviceId);
	
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
				
				break;
			}
            case ePNDEV_FLASH_TYPE_SPI_N25Q128A:
            {
            UINT8	lDeviceId[20]	= { 0,0,0,0,0,
                                        0,0,0,0,0,
                                        0,0,0,0,0,
                                        0,0,0,0,0 };

				bResult = fnFlashSPI_ReadID(pDevExtIn,
											pFlashParInOut,
											0,
											sizeof(lDeviceId),
											FALSE,
											lDimErrorIn,
											(UINT8*) &lDeviceId,
											sErrorOut);

				if	(   (lDeviceId[0] != BOARD_FPGA1_HERA_SPI_FLASH__MEM_TYPE_N25Q128A_MANUFAC_ID)
                    ||  (lDeviceId[1] != BOARD_FPGA1_HERA_SPI_FLASH__MEM_TYPE_N25Q128A_MEMTYPE_ID)
                    ||  (lDeviceId[2] != BOARD_FPGA1_HERA_SPI_FLASH__MEM_TYPE_N25Q128A_MEMSIZE_ID))
					// error
				{
					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid flash identification of N25Q128A (ManufactureId: 0x%x, FlashType: 0x%x, FlashSize: 0x%x)!"),
										lDeviceId[0],
                                        lDeviceId[1],
                                        lDeviceId[2]);

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
			default:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid FlashType!"),
								NULL,
								lDimErrorIn,
								sErrorOut);

				break;
			}
		}

		if	(bResult)
			// success
		{
			// return
			pFlashParInOut->eFlashTypeReal = pFlashParInOut->eFlashTypePos[i];

			// clear ErrorString
			//	- may be set if several FlashTypes are tested!
			RtlZeroMemory(	sErrorOut,
							lDimErrorIn);

			// leave loop
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
// write to flash
//************************************************************************

void fnFlash_Write(	volatile UINT8*			pFlashIn,
					const UINT32			lOffsetIn,
					const UINT32			lValueIn,
					const ePNDEV_FLASH_MODE	eFlashModeIn)
{
	switch	(eFlashModeIn)
			// FlashMode
	{
		case ePNDEV_FLASH_MODE_X8_AS_X8:	{WRITE_REG_UINT8(	(pFlashIn + lOffsetIn), ((UINT8)  lValueIn));	break;}
		case ePNDEV_FLASH_MODE_X8_AS_X16:	{WRITE_REG_UINT16(	(pFlashIn + lOffsetIn), ((UINT16) lValueIn));	break;}
		case ePNDEV_FLASH_MODE_X16_AS_X16:	{WRITE_REG_UINT16(	(pFlashIn + lOffsetIn), ((UINT16) lValueIn));	break;}
		case ePNDEV_FLASH_MODE_X16_AS_X32:	{WRITE_REG_UINT32(	(pFlashIn + lOffsetIn), ((UINT32) lValueIn));	break;}
		case ePNDEV_FLASH_MODE_X32_AS_X32:	{WRITE_REG_UINT32(	(pFlashIn + lOffsetIn), ((UINT32) lValueIn));	break;}
        case ePNDEV_FLASH_MODE_SERIAL:      { break; } // call of this function with case serial is not allowed !
		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
// read from flash
//************************************************************************

UINT32 fnFlash_Read(volatile UINT8*			pFlashIn,
					const UINT32			lOffsetIn,
					const ePNDEV_FLASH_MODE	eFlashModeIn)
{
UINT32 lResult = 0;

	switch	(eFlashModeIn)
			// FlashMode
	{
		case ePNDEV_FLASH_MODE_X8_AS_X8:	{lResult = READ_REG_UINT8((	pFlashIn + lOffsetIn));	break;}
		case ePNDEV_FLASH_MODE_X8_AS_X16:	{lResult = READ_REG_UINT16((pFlashIn + lOffsetIn));	break;}
		case ePNDEV_FLASH_MODE_X16_AS_X16:	{lResult = READ_REG_UINT16((pFlashIn + lOffsetIn));	break;}
		case ePNDEV_FLASH_MODE_X16_AS_X32:	{lResult = READ_REG_UINT32((pFlashIn + lOffsetIn));	break;}
		case ePNDEV_FLASH_MODE_X32_AS_X32:	{lResult = READ_REG_UINT32((pFlashIn + lOffsetIn));	break;}
        case ePNDEV_FLASH_MODE_SERIAL:      { break; } // call of this function with case serial is not allowed !
		default:
		{
			break;
		}
	}

	return(lResult);
}
