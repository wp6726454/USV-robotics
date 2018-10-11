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
/*  F i l e               &F: Board.c                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Common board specific code
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
	#include "Board.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_InitVar)
//	#pragma alloc_text (PAGE, fnBoard_GetHwInfo)
//	#pragma alloc_text (PAGE, fnBoard_GetRemanentData)
//	#pragma alloc_text (PAGE, fnBoard_ConvertMacAdr)
//	#pragma alloc_text (PAGE, fnBoard_StartFlashPage)
//	#pragma alloc_text (PAGE, fnBoard_StopFlashPage)
//	#pragma alloc_text (PAGE, fnBoard_StartSdramPage)
//	#pragma alloc_text (PAGE, fnBoard_StopSdramPage)
//	#pragma alloc_text (PAGE, fnBoard_CopyDataDirect)
//	#pragma alloc_text (PAGE, fnBoard_CopyDataPaged_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_CopyBlockPaged_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_CheckVersionBootFw)
//	#pragma alloc_text (PAGE, fnBoard_CheckSignOfLifeBootFw)
//	#pragma alloc_text (PAGE, fnBoard_DoElfCmdLoadBootFw)
//	#pragma alloc_text (PAGE, fnBoard_DoElfCmdExecuteBootFw)
//	#pragma alloc_text (PAGE, fnBoard_DoBinCmdExecuteBootFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForElfCmdDoneBootFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForBinCmdDoneBootFw)
//	#pragma alloc_text (PAGE, fnBoard_CheckElfStatusBootFw)
//	#pragma alloc_text (PAGE, fnBoard_CheckBinStatusBootFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForStartupParUserFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForReadyUserFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForAckDebugger)
//	#pragma alloc_text (PAGE, fnBoard_InitLed)
//	#pragma alloc_text (PAGE, fnBoard_SetLed)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize variables
//************************************************************************

BOOLEAN fnBoard_InitVar(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		case ePNDEV_BOARD_EB200:			// EB200 board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= TRUE;
			pDevExtIn->bCopyDataSupport = TRUE;

			// set FctPtr
			//	- no dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIrte_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			//	- no TopLevel-ICU exist -> IRTE-ICU
			pDevExtIn->eIcu = ePNDEV_ICU_IRTE;
			
			if	(pDevExtIn->eBoard != ePNDEV_BOARD_EB200)
				// !Ertec200
			{
				pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_FIRST;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData and HostMem
			pDevExtIn->bFlashSupport 		= TRUE;
			pDevExtIn->bCopyDataSupport 	= TRUE;
			pDevExtIn->eValidHostMemPool 	= ePNDEV_VAL_HOST_MEM_FIRST;

			// set FctPtr
			//	- no dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIrte_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			//	- TopLevel-ICU = PCI-ICU
			pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_PCI;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			// success
			bResult = TRUE;

			// support Flash,CopyData and no HostMem
			pDevExtIn->bFlashSupport 	= TRUE;
			pDevExtIn->bCopyDataSupport = TRUE;

			// set FctPtr
			pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX) fnBoardIrtePnip_Reset;

			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					// set FctPtr
					//	- no dynamic UserPage exist
					pDevExtIn->pFnBoard_ChangePageXhif = NULL;

					#ifdef FPGA1_ERTEC200P_ARM_NO_TOPLEVEL_ICU

						// update IcuType
						//	- no TopLevel-ICU exist -> PNIP-ICU2
						pDevExtIn->eIcu = ePNDEV_ICU_PNIP;

					#else

						// update IcuType
						//	- TopLevel-ICU = ARM-ICU
						pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_ARM;

					#endif

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					// set FctPtr
					//	- dynamic UserPage exist
					pDevExtIn->pFnBoard_ChangePageXhif = (FN_BOARD_XX) fnBoardPnip_ChangePageXhif;

					// update IcuType
					//	- TopLevel-ICU = PERIF-ICU
					pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_PERIF;

					break;
				}
				default:
				{
					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= TRUE;
			pDevExtIn->bCopyDataSupport = TRUE;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIrtePnip_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= (FN_BOARD_XX) fnBoardPnip_ChangePageXhif;

			// update IcuType
			//	- TopLevel-ICU = PERIF-ICU
			pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_PERIF;

			break;
		}
		case ePNDEV_BOARD_I210:	// I210 board
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= FALSE;
			pDevExtIn->bCopyDataSupport = FALSE;
			
			// support HostMem
			pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIntel_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			pDevExtIn->eIcu = ePNDEV_ICU_INTEL;

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= TRUE;
			pDevExtIn->bCopyDataSupport = TRUE;
			
			// support HostMem
			pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIrtePnip_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			pDevExtIn->eIcu = ePNDEV_ICU_INVALID;

			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
		case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= FALSE;
			pDevExtIn->bCopyDataSupport = FALSE;
			
			// support HostMem
			pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardMicrel_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			pDevExtIn->eIcu = ePNDEV_ICU_MICREL;

			break;
		}
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= FALSE;
			pDevExtIn->bCopyDataSupport = TRUE;
			
			// support HostMem
			pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardOmap_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			pDevExtIn->eIcu = ePNDEV_ICU_INVALID;

			break;
		}
		default:
		{
		char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Invalid board!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,						// logging
						sTraceDetail);

			break;
		}
	}

	if	(bResult)
		// success
	{
		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_CP1616:			// CP1616 board
			case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_ERTEC400;

				// set AsicDetail
				{
				UINT32 lIrteVersion = 0;

					// get IrteVersion
					lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION);

					if	(lIrteVersion == IRTE_IP_VERSION__REV5_METAL_FIX)
						// MetalFix
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV5_METAL_FIX;
					}
					else
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV5;
					}
				}

				break;
			}
			case ePNDEV_BOARD_EB200:			// EB200 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_ERTEC200;

				// AsicDetail: see fnBoardIrte_SetStaticPagesLbu()

				break;
			}
			case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
			case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_SOC1;

				// set AsicDetail
				{
				UINT32 lIrteVersion = 0;

					// get IrteVersion
					lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION);

					if	(lIrteVersion == IRTE_IP_VERSION__REV7_METAL_FIX)
						// MetalFix
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV7_METAL_FIX;
					}
					else
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV7;
					}
				}

				break;
			}
			case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
			case ePNDEV_BOARD_EB200P:			// EB200P board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_ERTEC200P;

				// AsicDetail: see fnBoardPnip_SetStaticPagesXhif()

				break;
			}
			case ePNDEV_BOARD_I210:	// I210 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_I210;

				break;
			}
			case ePNDEV_BOARD_I82574:	// Intel I82574 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_I82574;

				break;
			}
			case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_HERA;

				break;
			}
			case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_KSZ8841;

				break;
			}
			case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_KSZ8842;

				break;
			}
			case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_AM5728;

				break;
			}
			default:
			{
				break;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get HwInfo
//************************************************************************

BOOLEAN fnBoard_GetHwInfo(	DEVICE_EXTENSION*	pDevExtIn,
							const UINT32		lDimErrorIn,
							uPNDEV_HW_INFO*		pHwInfoOut,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;

	// preset
	bResult = TRUE;

	pHwInfoOut->eBoard			= pDevExtIn->eBoard;
	pHwInfoOut->eBoardDetail	= pDevExtIn->eBoardDetail;
	pHwInfoOut->eAsic			= pDevExtIn->eAsic;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
		BOOLEAN						bResultPage			= FALSE;
		UINT32						lOffsetFlashData	= 0;
		uPNDEV_FLASH_PAR			uFlashPar;
		uPNDEV_FLASH_DATA_CP1616	uFlashData;
		_TCHAR						sVersionSbl[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

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

			if	(!bResultPage)
				// error
			{
				bResult = FALSE;
			}
			else
			{
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

				// set BoardInfo
				_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
									_countof(pHwInfoOut->sBoardInfo),
									_TEXT("CP1616 board, Version='%s'"),
									sVersionSbl);

				// AsicCore = Arm9
				pHwInfoOut->bAsicCoreExist = TRUE;
			}

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("DB-EB400-PCIe board"));

			// AsicCore = Arm9
			pHwInfoOut->bAsicCoreExist = TRUE;
			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
		UINT32 lVersionFpgaMajor = 0;
		UINT32 lVersionFpgaMinor = 0;

			// read version of PciBridge-FPGA
			lVersionFpgaMajor	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MAJOR);
			lVersionFpgaMinor	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MINOR);

			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("EB200 board (PCI-Bridge-FPGA 0x%02x.0x%02x)"),
								lVersionFpgaMajor,
								lVersionFpgaMinor);

			// AsicCore = Arm9
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("DB-Soc1-PCI board"));

			// AsicCore = MIPS
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("DB-Soc1-PCIe board"));

			// AsicCore = MIPS
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					// set BoardInfo
					_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
										_countof(pHwInfoOut->sBoardInfo),
										_TEXT("FPGA1-Ertec200P board (PCIe-AHB), Toplevel=0x%02x.0x%02x"),
										pDevExtIn->lBitStreamId,
										pDevExtIn->lBitStreamVersion);

					// no AsicCore
					pHwInfoOut->bAsicCoreExist = FALSE;

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					// set BoardInfo
					_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
										_countof(pHwInfoOut->sBoardInfo),
										_TEXT("FPGA1-Ertec200P board (PCIe-XHIF), Toplevel=0x%02x.0x%02x"),
										pDevExtIn->lBitStreamId,
										pDevExtIn->lBitStreamVersion);

					// AsicCore = Arm926
					pHwInfoOut->bAsicCoreExist = TRUE;

					break;
				}
				default:
				{
					// error
					bResult = FALSE;

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid BoardDetail!"),
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
        UINT32 lScrbId = 0;

            // get SCRB-ID
			lScrbId = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__ID);

            if	(lScrbId == ERTEC200P_SCRB_ID__REV2_HW2)
				// hw-release Step2
			{
                // set BoardInfo
			    _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								    _countof(pHwInfoOut->sBoardInfo),
								    _TEXT("EB200P-2 board, FPGA-Version=0x%08x"),
								    pDevExtIn->lBoardRevision);
            }
            else
            {
			    // set BoardInfo
			    _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								    _countof(pHwInfoOut->sBoardInfo),
								    _TEXT("EB200P board, FPGA-Version=0x%08x"),
								    pDevExtIn->lBoardRevision);
            }

			// AsicCore = Arm926
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_I210:	// I210 board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("I210 board, Version: 0x%04x (0x1=A1, 0x3=A2)"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;

			break;
		}
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
		{
			// set BoardInfo //TODO z002p64u print board revsion
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("I82574 board, Version: 0x%04x"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("FPGA1-Hera board, Inc: 0x%02x, Label: 0x%02x "),
								pDevExtIn->lBitStreamId,
								pDevExtIn->lBitStreamVersion);

			// AsicCore = Arm9
			pHwInfoOut->bAsicCoreExist = TRUE;
			
			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("Micrel KSZ8841 board, Version: 0x%04x"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;

			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("Micrel KSZ8842 board, Version: 0x%04x"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;

			break;
		}
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("TI Omap board, Version: 0x%08x "),
								pDevExtIn->lBitStreamVersion);

			// AsicCore = Cortex
			pHwInfoOut->bAsicCoreExist = TRUE;
			
			break;
		}
		default:
		{
			// error
			bResult = FALSE;

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid board!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	if	(bResult)
		// success
	{
		// store info about AsicCore
		pDevExtIn->bAsicCoreExist = pHwInfoOut->bAsicCoreExist;

		if	(fnGetInfoConfigSpace(	pDevExtIn,
									lDimErrorIn,
									&pHwInfoOut->uConfigSpace,
									sErrorOut))
			// getting ConfigSpaceInfo ok
		{
			// set AsicDetail
			pHwInfoOut->eAsicDetail = pDevExtIn->eAsicDetail;

			// set AsicInfo
			{
				switch	(pDevExtIn->eAsic)
						// AsicType
				{
					case ePNDEV_ASIC_ERTEC400:
					{
						if	(pDevExtIn->eAsicDetail == ePNDEV_ASIC_DETAIL_IRTE_REV5_METAL_FIX)
							// MetalFix
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}
						else
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (no MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}

						break;
					}
					case ePNDEV_ASIC_ERTEC200:
					{
						if	(pDevExtIn->eAsicDetail == ePNDEV_ASIC_DETAIL_IRTE_REV6_METAL_FIX)
							// MetalFix
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}
						else
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (no MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}

						break;
					}
					case ePNDEV_ASIC_SOC1:
					{
						if	(pDevExtIn->eAsicDetail == ePNDEV_ASIC_DETAIL_IRTE_REV7_METAL_FIX)
							// MetalFix
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}
						else
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (no MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}

						break;
					}
					case ePNDEV_ASIC_ERTEC200P:
					{
						switch	(pDevExtIn->eAsicDetail)
								// AsicDetail
						{
							case ePNDEV_ASIC_DETAIL_PNIP_REV1:
							{
								// set AsicInfo
								//	- IP_Version.IP_Development
								_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
													_countof(pHwInfoOut->sAsicInfo),
													_TEXT("PNIP Version=0x%04x Development=0x%04x (Rev.1)"),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_VERSION),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_DEVELOPMENT));

								break;
							}
							case ePNDEV_ASIC_DETAIL_PNIP_REV2:
							{
								// set AsicInfo
								//	- IP_Version.IP_Development
								_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
													_countof(pHwInfoOut->sAsicInfo),
													_TEXT("PNIP Version=0x%04x Development=0x%04x (Rev.2)"),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_VERSION),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_DEVELOPMENT));

								break;
							}
							default:
							{
								// error
								bResult = FALSE;

								// set ErrorString
								fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid AsicVersion!"),
												NULL,
												lDimErrorIn,
												sErrorOut);

								break;
							}
						}

						break;
					}
					case ePNDEV_ASIC_I210:
					{
					UINT32	lBootImageVersion 	= 0;
					UINT32	lTmpValue			= 0;

						// write address (BootImage Id is located at offset 0x5 at eeprom)
						WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__EERD, (0x5<<2));

						do
						{
							// get register value
							lTmpValue = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__EERD);
						}
								// read operation not done
						while	(((lTmpValue >> BOARD_I210__EERD_DONE) & 1) != BOARD_I210__EERD_DONE);

						// read data (bit 31:16)
						lBootImageVersion = (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__EERD)>>16);

						// set AsicInfo
						_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
											_countof(pHwInfoOut->sAsicInfo),
											_TEXT("RevisionId BootImageVersion=0x%04x"),
											lBootImageVersion);

						break;
					}
					case ePNDEV_ASIC_I82574:
					{
						// set AsicInfo - read of boot image version not supported on I82574 (possible position not in spec)
						break;
					}
					case ePNDEV_ASIC_HERA:
					{
						// set AsicInfo
						//	- IP_Version.IP_Development
						_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
											_countof(pHwInfoOut->sAsicInfo),
											_TEXT("PNIP Version=0x%04x Development=0x%04x (Rev.2)"),
											READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_VERSION),
											READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_DEVELOPMENT));

						break;
					}
					case ePNDEV_ASIC_KSZ8841:
					case ePNDEV_ASIC_KSZ8842:
					{
						// no AsicInformation
						break;
					}
					case ePNDEV_ASIC_AM5728:
					{
						// no AsicInformation
						// ToDo mk5656: AsicVersion herausfinden
						break;
					}
					default:
					{
						// error
						bResult = FALSE;

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid Asic!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

						break;
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
//  get RemanentData
//************************************************************************

BOOLEAN fnBoard_GetRemanentData(DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN				bResult				= FALSE;
BOOLEAN				bResultPage			= FALSE;
UINT32				lOffsetFlashData	= 0;
uPNDEV_FLASH_PAR	uFlashPar;

	// preset OutputParameter
	RtlZeroMemory(	&pDevExtIn->uRemanentData,
					sizeof(uPNCORE_REMANENT_DATA));

	if	(	(pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
		||	(pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8842))
		//	MicrelBoard
	{
		if	(fnBoardMicrel_GetMacAdress(pDevExtIn,
										lDimErrorIn,
										sErrorOut))
			// success
		{
			bResult = TRUE;			
		}
	}
	else
	{
		// Note:
		//	- if there are flash problems OpenDevice cannot solve them
		//		-> returned error is only used for debug purpose
	
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
			UINT32 lCtrMacAdrMin = 0;
	
				switch	(pDevExtIn->eAsic)
						// AsicType
				{
					case ePNDEV_ASIC_ERTEC400:	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC400;	break;}
					case ePNDEV_ASIC_ERTEC200:	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC200;	break;}
					case ePNDEV_ASIC_SOC1:		{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__SOC1;		break;}
					case ePNDEV_ASIC_ERTEC200P:	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC200P;	break;}
					case ePNDEV_ASIC_HERA:	    {lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__HERA;	    break;}
					default:
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid AsicType!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
	
						break;
					}
				}
	
				if	(lCtrMacAdrMin != 0)
					// success
				{
				UINT64 lMacAdr64_Tmp = 0;
	
					switch	(uFlashPar.eFlashTypePos[0])
							// FlashType
							//	- eFlashTypeReal not available, see above
					{
						case ePNDEV_FLASH_TYPE_NOR_AM29LV040B:
						case ePNDEV_FLASH_TYPE_NOR_AM29LV641:
						case ePNDEV_FLASH_TYPE_NOR_S29GL064N:
						case ePNDEV_FLASH_TYPE_NOR_S29GL512P:
						case ePNDEV_FLASH_TYPE_NOR_S29WS128P:
						{
							if	(pDevExtIn->eBoard == ePNDEV_BOARD_CP1616)
								// CP1616 board
							{
							uPNDEV_FLASH_DATA_CP1616 uFlashData;
	
								// success
								bResult = TRUE;
	
								// Note:
								//	- IA AS:	at least 5 MacAdr are necessary (4x PortMacAdr, 1x IfaMacAdr)
								//	- IA SC IC: at least 6 MacAdr are necessary (4x PortMacAdr, 2x IfaMacAdr)
								//	- lCtrMacAdr=0/0xff: 2 MacAdr are stored -> not useful -> treat it as an empty flash without FlashData
	
								// read FlashData
								*((uPNDEV_FLASH_DATA_CP1616*) &uFlashData) = *((uPNDEV_FLASH_DATA_CP1616*) (uFlashPar.pFlash + lOffsetFlashData));
	
								if	(	(uFlashData.lCtrMacAdr < PNDEV_MAC_ADR_MIN__ERTEC400)
									||	(uFlashData.lCtrMacAdr > PNDEV_MAC_ADR_MAX))
									// invalid number of MacAdr
								{
									// treat it as empty flash without FlashData
								}
								else
								{
									// return
									{
										pDevExtIn->uRemanentData.lCtrMacAdr = uFlashData.lCtrMacAdr;
	
										RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
														(void*) (uFlashData.lArrayPrivate),
														PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
									}
	
									// compute MacAdr64 = base of all other MacAdr
									{
									UINT8 i = 0;
	
										// example:
										//	- MacAdr = aa-bb-cc-dd-ee-ff = 0x0000aabbccddeeff
										//														  offset:  0  1  2  3  4  5
										//	- LittleEndian: 0x0000aabbccddeeff has following memory dump: ff ee dd cc bb aa 00 00
										//		-> offset 5 = MacAdr[0]
										//		-> offset 4 = MacAdr[1]
										//		-> offset 3 = MacAdr[2]
										//		-> offset 2 = MacAdr[3]
										//		-> offset 1 = MacAdr[4]
										//		-> offset 0 = MacAdr[5]
	
										for	(i = 0; i < 6; i++)
										{
											*(((volatile UINT8*) &lMacAdr64_Tmp) + (5-i)) = uFlashData.uMacAdr0[i];
										}
									}
								}
							}
							else
							{
							ePNDEV_FLASH_VERSION eVersion = ePNDEV_FLASH_VERSION_INVALID;
	
								// get version
								eVersion = *((ePNDEV_FLASH_VERSION*) (uFlashPar.pFlash + lOffsetFlashData));
	
								switch	(eVersion)
										// version
								{
									case ePNDEV_FLASH_VERSION_V0001:	// version V1
									{
									uPNDEV_FLASH_DATA_V1 uFlashData;
	
										// read FlashData
										*((uPNDEV_FLASH_DATA_V1*) &uFlashData) = *((uPNDEV_FLASH_DATA_V1*) (uFlashPar.pFlash + lOffsetFlashData));
	
										if	(	(uFlashData.lCtrMacAdr < lCtrMacAdrMin)
											||	(uFlashData.lCtrMacAdr > PNDEV_MAC_ADR_MAX))
											// invalid number of MacAdr
										{
											// set ErrorString
											fnBuildString(	_TEXT(__FUNCTION__),
															_TEXT("(): Invalid number of MacAdr!"),
															NULL,
															lDimErrorIn,
															sErrorOut);
										}
										else
										{
											// success
											bResult = TRUE;
	
											// return
											{
												pDevExtIn->uRemanentData.lCtrMacAdr = uFlashData.lCtrMacAdr;
	
												RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
																(void*) (uFlashData.lArrayPrivate),
																PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
											}
	
											// stored MacAdr = base of all other MacAdr
											lMacAdr64_Tmp = uFlashData.lMacAdr64;
										}
	
										break;
									}
									case ePNDEV_FLASH_VERSION_EMPTY:	// empty flash
									{
										// success
										bResult = TRUE;
	
										break;
									}
									default:
									{
										// Note:
										//	- don't return an error but treat it as an empty flash without FlashData
										//	- possible error conditions:
										//		flash not accessible
										//		invalid pattern at flash
	
										break;
									}
								}
							}
	
							break;
						}
						case ePNDEV_FLASH_TYPE_NAND_K91G08U0B:
						{
						ePNDEV_FLASH_VERSION eVersion = ePNDEV_FLASH_VERSION_INVALID;
	
							// get version
							bResult = fnFlashNand_Read(	pDevExtIn,
														&uFlashPar,
														lOffsetFlashData,
														sizeof(ePNDEV_FLASH_VERSION),
														TRUE,							// EncodedRsc
														lDimErrorIn,
														(UINT8*) &eVersion,
														sErrorOut);
	
							if	(bResult)
								// success
							{
								switch	(eVersion)
										// eVersion
								{
									case ePNDEV_FLASH_VERSION_V0001:	// version V1
									{
									uPNDEV_FLASH_DATA_V1 uFlashData;
	
										// read FlashData
										bResult = fnFlashNand_Read(	pDevExtIn,
																	&uFlashPar,
																	lOffsetFlashData,
																	sizeof(uFlashData),
																	TRUE,					// EncodedRsc
																	lDimErrorIn,
																	(UINT8*) &uFlashData,
																	sErrorOut);
	
										if	(	(uFlashData.lCtrMacAdr < lCtrMacAdrMin)
											||	(uFlashData.lCtrMacAdr > PNCORE_MAC_ADR_MAX))
											// invalid number of MacAdr
										{
											// set ErrorString
											fnBuildString(	_TEXT(__FUNCTION__),
															_TEXT("(): Invalid number of MacAdr!"),
															NULL,
															lDimErrorIn,
															sErrorOut);
										}
										else
										{
											// success
											bResult = TRUE;
	
											// return
											{
												pDevExtIn->uRemanentData.lCtrMacAdr = uFlashData.lCtrMacAdr;
	
												RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
																(void*) (uFlashData.lArrayPrivate),
																PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
											}
	
											// stored MacAdr = base of all other MacAdr
											lMacAdr64_Tmp = uFlashData.lMacAdr64;
										}
										break;
									}
									case ePNDEV_FLASH_VERSION_EMPTY:	// empty flash
									{
										// success
										bResult = TRUE;
	
										break;
									}
									default:
									{
										// Note:
										//	- don't return an error but treat it as an empty flash without FlashData
										//	- possible error conditions:
										//		flash not accessible
										//		invalid pattern at flash
										break;
									}
								}
								break;
							}
	
							break;
	
						}
						case ePNDEV_FLASH_TYPE_SPI_N25Q128A:
						{
						uPNDEV_FLASH_DATA_V1 uFlashData;
						
							bResult = fnFlashSPI_Read(	pDevExtIn,
														&uFlashPar,
														lOffsetFlashData,
														sizeof(uFlashData),
														FALSE,
														lDimErrorIn,
														(UINT8*) &uFlashData,
														sErrorOut);
	
							if	(bResult)
								// success
							{
								switch	(uFlashData.eVersion)
										// eVersion
								{
									case ePNDEV_FLASH_VERSION_V0001:	// version V1
									{
										if	(	(uFlashData.lCtrMacAdr < lCtrMacAdrMin)
											||	(uFlashData.lCtrMacAdr > PNCORE_MAC_ADR_MAX))
											// invalid number of MacAdr
										{
											// set ErrorString
											fnBuildString(	_TEXT(__FUNCTION__),
															_TEXT("(): Invalid number of MacAdr!"),
															NULL,
															lDimErrorIn,
															sErrorOut);
										}
										else
										{
											// success
											bResult = TRUE;
	
											// return
											{
												pDevExtIn->uRemanentData.lCtrMacAdr = uFlashData.lCtrMacAdr;
	
												RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
																(void*) (uFlashData.lArrayPrivate),
																PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
											}
	
											// stored MacAdr = base of all other MacAdr
											lMacAdr64_Tmp = uFlashData.lMacAdr64;
										}
										break;
									}
									case ePNDEV_FLASH_VERSION_EMPTY:	// empty flash
									{
										// success
										bResult = TRUE;
	
										break;
									}
									default:
									{
										// Note:
										//	- don't return an error but treat it as an empty flash without FlashData
										//	- possible error conditions:
										//		flash not accessible
										//		invalid pattern at flash
										break;
									}
								}
								break;
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
	
					if	(pDevExtIn->uRemanentData.lCtrMacAdr != 0)
						// MacAdr exist
					{
					UINT32 i = 0;
	
						for	(i = 0; i < pDevExtIn->uRemanentData.lCtrMacAdr; i++)
							// all MacAdr (based on stored MacAdr)
						{
							// convert MacAdr from UINT64 to array
							fnBoard_ConvertMacAdr(	(lMacAdr64_Tmp + i),
													&pDevExtIn->uRemanentData.lArrayMacAdr[i][0]);
						}
					}
				}
			}
		}
		//------------------------------------------------------------------------
		// stop using FlashPage
		fnBoard_StopFlashPage(pDevExtIn);
}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  convert MacAdr from UINT64 to array
//************************************************************************

void fnBoard_ConvertMacAdr(	const UINT64	lMacAdr64In,
							volatile UINT8*	pArrayMacAdr8Out)
{
UINT8 i = 0;

	// example:
	//	- MacAdr = aa-bb-cc-dd-ee-ff = 0x0000aabbccddeeff
	//                                                        offset:  0  1  2  3  4  5
	//	- LittleEndian: 0x0000aabbccddeeff has following memory dump: ff ee dd cc bb aa 00 00
	//		-> MacAdr[0] = offset 5
	//		-> MacAdr[1] = offset 4
	//		-> MacAdr[2] = offset 3
	//		-> MacAdr[3] = offset 2
	//		-> MacAdr[4] = offset 1
	//		-> MacAdr[5] = offset 0

	for	(i = 0; i < 6; i++)
	{
		*(pArrayMacAdr8Out + i) = *(((volatile UINT8*) &lMacAdr64In) + (5-i));
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start FlashPage
//************************************************************************

BOOLEAN fnBoard_StartFlashPage(	DEVICE_EXTENSION*	pDevExtIn,
								const BOOLEAN		bVerifyChipIn,
								const UINT32		lDimErrorIn,
								uPNDEV_FLASH_PAR*	pFlashParOut,
								UINT32*				pOffsetFlashDataOut,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset OutputParameter
	{
		RtlZeroMemory(	pFlashParOut,
						sizeof(uPNDEV_FLASH_PAR));

		*pOffsetFlashDataOut = 0;
	}

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
			pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.pPtr;
			pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X16;
			pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_AM29LV641;
			pFlashParOut->eFlashTypePos[1]	= ePNDEV_FLASH_TYPE_NOR_S29GL064N;
			pFlashParOut->eFlashInfo		= (ePNDEV_FLASH_INFO_SBL_EXIST + ePNDEV_FLASH_INFO_BOARD_DATA_EXIST);
			pFlashParOut->lSizeUsable		= BOARD_CP1616__FLASH_SIZE_USABLE;
			pFlashParOut->lSizeSector		= BOARD_CP1616__FLASH_SIZE_SECTOR;

			*pOffsetFlashDataOut = BOARD_CP1616__FLASH_OFFSET_DATA;

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.pPtr;
			pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X16;
			pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29GL064N;
			pFlashParOut->eFlashInfo		= ePNDEV_FLASH_INFO_SBL_EXIST;
			pFlashParOut->lSizeUsable		= BOARD_EB400__FLASH_SIZE_USABLE;
			pFlashParOut->lSizeSector		= BOARD_EB400__FLASH_SIZE_SECTOR;

			*pOffsetFlashDataOut = BOARD_EB400__FLASH_OFFSET_DATA;

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			//------------------------------------------------------------------------
			// [LBU: using dynamic DriverPage]
			//	- 8Bit-flash, page0
			fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
											eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK0,
											0,
											__FUNCTION__);
			{
				pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0.pPtr;
				pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X8_AS_X8;
				pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_AM29LV040B;
				pFlashParOut->eFlashInfo		= ePNDEV_FLASH_INFO_SBL_EXIST;
				pFlashParOut->lSizeUsable		= BOARD_EB200__FLASH_SIZE_USABLE;
				pFlashParOut->lSizeSector		= BOARD_EB200__FLASH_SIZE_SECTOR;

				*pOffsetFlashDataOut = BOARD_EB200__FLASH_OFFSET_DATA;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.pPtr;
			pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X8_AS_X16;
			pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NAND_K91G08U0B;
			pFlashParOut->eFlashInfo		= ePNDEV_FLASH_INFO_SBL_EXIST;
			pFlashParOut->lSizeUsable		= BOARD_SOC1_DEV__FLASH_SIZE_USABLE;
			pFlashParOut->lSizeSector		= BOARD_SOC1_DEV__FLASH_SIZE_SECTOR;

			pFlashParOut->uNand.lSizePage		= BOARD_SOC1_DEV__FLASH_SIZE_PAGE;
			pFlashParOut->uNand.lSizeRscData	= BOARD_SOC1_DEV__FLASH_SIZE_RSC_DATA;
			pFlashParOut->uNand.lSizeRscCheck	= BOARD_SOC1_DEV__FLASH_SIZE_RSC_CHECK;
			pFlashParOut->uNand.pPollReg		= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__IN5;
			pFlashParOut->uNand.lOffsetDataReg	= BOARD_SOC1_DEV__FLASH_OFFSET_DATA_REG;
			pFlashParOut->uNand.lOffsetCmdReg	= pFlashParOut->uNand.lOffsetDataReg + BOARD_SOC1_DEV__FLASH_OFFSET_CMD_REG;
			pFlashParOut->uNand.lOffsetAdrReg	= pFlashParOut->uNand.lOffsetDataReg + BOARD_SOC1_DEV__FLASH_OFFSET_ADR_REG;

			*pOffsetFlashDataOut = BOARD_SOC1_DEV__FLASH_OFFSET_DATA;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardId)
					// BoardId
			{
				case eBOARD_ID_FPGA1_ERTEC200P_XHIF_STD:
				case eBOARD_ID_FPGA1_ERTEC200P_AHB_STD:
				{
					//------------------------------------------------------------------------
					// [XHIF: using dynamic DriverPage]:
					//	- EMC AsyncBank1
					fnBoardPnip_StartDriverPageXhif(pDevExtIn,
													eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK1,
													0,
													__FUNCTION__);
					{
						pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.pPtr;
						pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X32;
						pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29GL512P;
						pFlashParOut->lSizeUsable		= BOARD_FPGA1__FLASH_SIZE_USABLE;
						pFlashParOut->lSizeSector		= BOARD_FPGA1__FLASH_SIZE_SECTOR;

						*pOffsetFlashDataOut = BOARD_FPGA1__FLASH_OFFSET_DATA;
					}

					break;
				}
				case eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200:
				case eBOARD_ID_FPGA1_ERTEC200P_XHIF_ET200:
				{
					//------------------------------------------------------------------------
					// [XHIF: using dynamic DriverPage]:
					//	- EMC AsyncBank0
					fnBoardPnip_StartDriverPageXhif(pDevExtIn,
													eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK0,
													0,
													__FUNCTION__);
					{
						pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr;
						pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X32;
						pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29GL512P;
						pFlashParOut->lSizeUsable		= BOARD_FPGA1__FLASH_SIZE_USABLE;
						pFlashParOut->lSizeSector		= BOARD_FPGA1__FLASH_SIZE_SECTOR;

						*pOffsetFlashDataOut = BOARD_FPGA1__FLASH_OFFSET_DATA;
					}

					break;
				}
				default:
				{
					// error
					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: using dynamic DriverPage]:
			//	- EMC AsyncBank1
			fnBoardPnip_StartDriverPageXhif(pDevExtIn,
//											eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK1,		// PageMode flash
											eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK0,		// BurstMode flash
											15,
											__FUNCTION__);
			{
					// 2x S29WS128P -> 2x 16MByte = 32MByte

				pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr;
				pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X32;
				pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29WS128P;
				pFlashParOut->lSizeUsable		= BOARD_EB200P__FLASH_SIZE_USABLE;
				pFlashParOut->lSizeSector		= BOARD_EB200P__FLASH_SIZE_SECTOR;

				*pOffsetFlashDataOut = BOARD_EB200P__FLASH_OFFSET_DATA;
			}

			break;
		}
        case ePNDEV_BOARD_FPGA1_HERA:
        {
            pFlashParOut->pFlash			        = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr;
            pFlashParOut->eFlashMode		        = ePNDEV_FLASH_MODE_SERIAL;
			pFlashParOut->eFlashTypePos[0]	        = ePNDEV_FLASH_TYPE_SPI_N25Q128A;
            pFlashParOut->eFlashInfo		        = ePNDEV_FLASH_INFO_INVALID;
            pFlashParOut->lSizeUsable		        = BOARD_FPGA1_HERA_SPI_FLASH__SIZE_USABLE;
            pFlashParOut->lSizeSector		        = BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_SIZE;

            pFlashParOut->uSPI.lSizePage	        = BOARD_FPGA1_HERA_SPI_FLASH__PAGE_SIZE;
            pFlashParOut->uSPI.lAddressMask         = BOARD_FPGA1_HERA_SPI_FLASH__ADDRESS_MASK;
            pFlashParOut->uSPI.bIsSSPInitialized    = FALSE;

            *pOffsetFlashDataOut = BOARD_FPGA1_HERA_SPI_FLASH__OFFSET_DATA;

            break;
        }
		default:
		{
			break;
		}
	}

	if	(pFlashParOut->pFlash != NULL)
		// success
	{
		// Note:
		//	- if a command is written then each chip must get the command -> datum must be duplicated if chips are interleaved
		//	- NOR  flash:
		//		8Bit offset of register X of chip 0 (shown at DataSheet) must be multiplied by DataWidth, this is independent of interleaving
		//		16Bit:	addressing Offset=0/1/2/.. of chip 0 -> MemOffset=0/2/4/..
		//		32Bit:	addressing Offset=0/1/2/.. of chip 0 -> MemOffset=0/4/8/..
		//	- NAND flash: offsets of DataReg/AdrReg/CmdReg are always constant independent of interleaving (ALE and CLE are set by fixed AddressBits)

		switch	(pFlashParOut->eFlashMode)
				// FlashMode
		{
			case ePNDEV_FLASH_MODE_X8_AS_X8:								// no-interleaving, a single x8 device in x8 mode
			{
				pFlashParOut->lDataWidth		= 1;						// 8Bit
				pFlashParOut->lDataMultiplier	= 0x00000001;				// multiplying with a UINT8 value X will result X (= chip1)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			case ePNDEV_FLASH_MODE_X8_AS_X16:								// two x8 devices interleaved to form x16
			{
				pFlashParOut->lDataWidth		= 2;						// 16Bit
				pFlashParOut->lDataMultiplier	= 0x00000101;				// multiplying with a UINT8 value X will show X at higher and lower byte (= chip1 and chip2)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				// For now we only use this mode on soc1_pci or soc1_pcie boards
				if	(	pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCI ||
						pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE)
				{
					// the whole address is calculated with adress of all nand flashes together -> multiply with datawidth
					// when accessing the nand flashes datawidth is used to calculate actual address of each flash -> divide through datawidth
					*pOffsetFlashDataOut *= pFlashParOut->lDataWidth;
				}
				break;
			}
			case ePNDEV_FLASH_MODE_X16_AS_X16:								// no-interleaving, a single x16 device in x16 mode
			{
				pFlashParOut->lDataWidth		= 2;						// 16Bit
				pFlashParOut->lDataMultiplier	= 0x00000001;				// multiplying with a UINT16 value X will result X (= chip1)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			case ePNDEV_FLASH_MODE_X16_AS_X32:								// two x16 devices interleaved to form x32
			{
				pFlashParOut->lDataWidth		= 4;						// 32Bit
				pFlashParOut->lDataMultiplier	= 0x00010001;				// multiplying with a UINT16 value X will show X at higher and lower word (= chip1 and chip2)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			case ePNDEV_FLASH_MODE_X32_AS_X32:								// no-interleaving, a single x32 device in x32 mode
			{
				pFlashParOut->lDataWidth		= 4;						// 32Bit
				pFlashParOut->lDataMultiplier	= 0x00000001;				// multiplying with a UINT32 value X will result X (= chip1)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			default:
			{
				break;
			}
		}

		if	(!bVerifyChipIn)
			// don't verify flash chip
		{
			// success
			bResult = TRUE;
		}
		else
		{
			if	(fnFlash_VerifyChip(pDevExtIn,
									lDimErrorIn,
									pFlashParOut,
									sErrorOut))
				// verifying flash chip ok
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
//  stop FlashPage
//************************************************************************

void fnBoard_StopFlashPage(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			//------------------------------------------------------------------------
			// [LBU: end of using dynamic DriverPage]
			fnBoardIrte_StopDriverPageLbu(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: end of using dynamic DriverPage]
			fnBoardPnip_StopDriverPageXhif(pDevExtIn);

			break;
		}
        case ePNDEV_BOARD_FPGA1_HERA:
        {
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
//  start SdramPage
//************************************************************************

void fnBoard_StartSdramPage(DEVICE_EXTENSION*	pDevExtIn,
							const UINT32		lSegmentAhbIn,
							BOOLEAN*			pPagedAccessOut,
							volatile UINT8**	pPtrCurSegmentOut)
{
	// preset OutputParameter
	*pPagedAccessOut	= FALSE;
	*pPtrCurSegmentOut	= NULL;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr;

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			//------------------------------------------------------------------------
			// [LBU: using dynamic DriverPage]
			//	- 8Bit-flash, page0
			fnBoardIrte_StartDriverPageLbu(	pDevExtIn,
											eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM,
											lSegmentAhbIn,
											__FUNCTION__);
			{
				*pPagedAccessOut	= TRUE;
				*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.pPtr;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					*pPagedAccessOut	= FALSE;
					*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.pPtr;

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					//------------------------------------------------------------------------
					// [XHIF: using dynamic DriverPage]:
					//	- EMC Sdram
					fnBoardPnip_StartDriverPageXhif(pDevExtIn,
													eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_SDRAM,
													lSegmentAhbIn,
													__FUNCTION__);
					{
						*pPagedAccessOut	= TRUE;
						*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram_DynPage.pPtr;
					}

					break;
				}
				default:
				{
					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: using dynamic DriverPage]:
			//	- EMC Sdram
			fnBoardPnip_StartDriverPageXhif(pDevExtIn,
											eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_SDRAM,
											lSegmentAhbIn,
											__FUNCTION__);
			{
				*pPagedAccessOut	= TRUE;
				*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram_DynPage.pPtr;
			}

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.pPtr;

			break;
		}
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.pPtr;

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
//  stop SdramPage
//************************************************************************

void fnBoard_StopSdramPage(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			//------------------------------------------------------------------------
			// [LBU: end of using dynamic DriverPage]
			fnBoardIrte_StopDriverPageLbu(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: end of using dynamic DriverPage]
			fnBoardPnip_StopDriverPageXhif(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			break;
		}
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
		{
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
//  copy data direct
//************************************************************************

void fnBoard_CopyDataDirect(uPNDEV_COPY_DATA_IN*	pIn,
							volatile UINT8*			pHostDataIn,
							volatile UINT8*			pAsicDestIn)
{
UINT32 i = 0;

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockWrite[i].lSize != 0)
			// current WriteBlock exist
		{
			RtlCopyMemory(	(void*) (pAsicDestIn + pIn->uArrayBlockWrite[i].lOffsetAsic),
							(void*) (pHostDataIn + pIn->uArrayBlockWrite[i].lOffsetHostDataBuf),
							pIn->uArrayBlockWrite[i].lSize);
		}
	}

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockRead[i].lSize != 0)
			// current ReadBlock exist
		{
			RtlCopyMemory(	(void*) (pHostDataIn + pIn->uArrayBlockRead[i].lOffsetHostDataBuf),
							(void*) (pAsicDestIn + pIn->uArrayBlockRead[i].lOffsetAsic),
							pIn->uArrayBlockRead[i].lSize);
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	copy data paged (AsicSdram)
//************************************************************************

void fnBoard_CopyDataPaged_Sdram(	DEVICE_EXTENSION*		pDevExtIn,
									uPNDEV_COPY_DATA_IN*	pIn,
									volatile UINT8*			pHostDataIn)
{
UINT32 i = 0;

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockWrite[i].lSize != 0)
			// current WriteBlock exist
		{
			// copy current block paged (AsicSdram)
			fnBoard_CopyBlockPaged_Sdram(	pDevExtIn,
											pHostDataIn + pIn->uArrayBlockWrite[i].lOffsetHostDataBuf,
											pIn->uArrayBlockWrite[i].lOffsetAsic,
											pIn->uArrayBlockWrite[i].lSize,
											ePNDEV_COPY_ACTION_WRITE);
		}
	}

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockRead[i].lSize != 0)
			// current ReadBlock exist
		{
			// copy current block paged (AsicSdram)
			fnBoard_CopyBlockPaged_Sdram(	pDevExtIn,
											pHostDataIn + pIn->uArrayBlockRead[i].lOffsetHostDataBuf,
											pIn->uArrayBlockRead[i].lOffsetAsic,
											pIn->uArrayBlockRead[i].lSize,
											ePNDEV_COPY_ACTION_READ);
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	copy current block paged (AsicSdram)
//************************************************************************

void fnBoard_CopyBlockPaged_Sdram(	DEVICE_EXTENSION*			pDevExtIn,
									volatile UINT8*				pHostDataIn,
									const UINT32				lOffsetAsicSdramIn,
									const UINT32				lSizeIn,
									const ePNDEV_COPY_ACTION	eActionIn)
{
volatile UINT8*	pHostData_Rest			= NULL;
UINT32			lOffsetAsicSdram_Rest	= 0;
UINT32			lSize_Rest				= 0;

	pHostData_Rest			= pHostDataIn;
	lOffsetAsicSdram_Rest	= lOffsetAsicSdramIn;
	lSize_Rest				= lSizeIn;

	while	(lSize_Rest != 0)
			// not all bytes copied
	{
	UINT32			lSegmentAhb				= 0;
	UINT32			lOffset_CurSegment		= 0;
	UINT32			lSize_CurSegment		= 0;
	volatile UINT8*	pAsicSdram_CurSegment	= NULL;
	BOOLEAN			bPagedAccess			= FALSE;

		// compute current segment (0,1,2,..)
		lSegmentAhb = lOffsetAsicSdram_Rest / (1024*1024*2);

		// compute offset to SegmentStartAdr
		lOffset_CurSegment = lOffsetAsicSdram_Rest % (1024*1024*2);

		if	(lOffset_CurSegment == 0)
			// block starts at SegmentStartAdr
		{
			if	(lSize_Rest <= (1024*1024*2))
				// final block
			{
				lSize_CurSegment = lSize_Rest;
			}
			else
			{
				lSize_CurSegment = (1024*1024*2);
			}
		}
		else
		{
		UINT32 lSizeMax_CurSegment = 0;

			lSizeMax_CurSegment = ((1024*1024*2) - lOffset_CurSegment);

			if	(lSize_Rest <= lSizeMax_CurSegment)
				// final block
			{
				lSize_CurSegment = lSize_Rest;
			}
			else
			{
				lSize_CurSegment = lSizeMax_CurSegment;
			}
		}

		// start SdramPage
		fnBoard_StartSdramPage(	pDevExtIn,
								lSegmentAhb,
								&bPagedAccess,
								&pAsicSdram_CurSegment);
		{
		#ifdef PNDEV_GET_RUNTIME_COPY_BLOCK_PAGED
		uPNDEV_CLOCK uClockStart;

				// get clock
				fnGetClock(&uClockStart);
		#endif

			switch	(eActionIn)
					// action
			{
				case ePNDEV_COPY_ACTION_READ:
				{
					RtlCopyMemory(	(void*) pHostData_Rest,
									(void*) (pAsicSdram_CurSegment + lOffset_CurSegment),
									lSize_CurSegment);

					break;
				}
				case ePNDEV_COPY_ACTION_WRITE:
				{
					RtlCopyMemory(	(void*) (pAsicSdram_CurSegment + lOffset_CurSegment),
									(void*) pHostData_Rest,
									lSize_CurSegment);

					break;
				}
				default:
				{
					break;
				}
			}

			pHostData_Rest			+= lSize_CurSegment;
			lOffsetAsicSdram_Rest	+= lSize_CurSegment;
			lSize_Rest				-= lSize_CurSegment;

			#ifdef PNDEV_GET_RUNTIME_COPY_BLOCK_PAGED
			{
			UINT32	lRuntime_Usec = 0;
			char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				// compute Runtime in usec
				lRuntime_Usec = fnComputeRunTimeUsec(&uClockStart);

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ++++ Copying page (max. 2MB): Runtime=%u usec (action=%s, size=%u)",
									pDevExtIn->sPciLocShortAscii,
									lRuntime_Usec,
									(eActionIn == ePNDEV_COPY_ACTION_READ) ? ("read") : ("write"),
									lSize_CurSegment);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							FALSE,						// logging
							sTraceDetail);
			}
			#endif
		}
		// stop SdramPage
		fnBoard_StopSdramPage(pDevExtIn);
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check version of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckVersionBootFw(	DEVICE_EXTENSION*	pDevExtIn,
									volatile UINT8*		pAsicSdramSeg0In,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult				= FALSE;
BOOLEAN	bResultAsic			= TRUE;
UINT32	lIdentRequired		= 0;
UINT32	lVersionRequired	= 0;

	switch	(pDevExtIn->eAsic)
			// AsicType
	{
		case ePNDEV_ASIC_ERTEC400:
		case ePNDEV_ASIC_ERTEC200:
		case ePNDEV_ASIC_ERTEC200P:
		{
			lIdentRequired		= 0xff550003;
			lVersionRequired	= 0x00020001;

			break;
		}
		case ePNDEV_ASIC_SOC1:
		{
			lIdentRequired		= 0xff550004;
			lVersionRequired	= 0x00010001;

			break;
		}
		default:
		{
			// error
			bResultAsic = FALSE;

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid AsicType!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	if	(bResultAsic)
		// success
	{
	UINT32	lIdent		= 0;
	UINT32	lVersion	= 0;

		// get version of BootFw
		lIdent		= READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_IDENT);
		lVersion	= READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_VERSION);

		if	(	(lIdent		!= lIdentRequired)
			||	(lVersion	!= lVersionRequired))
			// invalid BootFw
		{
		_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Invalid BootFw (real: 0x%08x.0x%08x, required: 0x%08x.0x%08x)!"),
								lIdent,
								lVersion,
								lIdentRequired,
								lVersionRequired);

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

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check SignOfLife of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckSignOfLifeBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		bTimeoutMsecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	lPatternPreset	= 0x11223344;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	// set AlivePattern
	WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
						lPatternPreset);

	for	(i = 0; i < bTimeoutMsecIn; i++)
		// MaxTime
	{
		if	(	(pConfigParIn != NULL)
			&&	(pConfigParIn->bCancel))
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
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
			UINT32 lAlive = 0;

				lAlive = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE);

				if	(lAlive == (~lPatternPreset))
					// success
				{
					// success
					bErrorTimeout = FALSE;

					// leave loop
					break;
				}
				else if	(lAlive == lPatternPreset)
						// AlivePattern unchanged
				{
					// wait further on
				}
				else
				{
					// AlivePattern changed but wrong value
					//	- if startup of BootFw not finished before checking SignOfLife then AlivePattern may be overwritten by startup code

					// rewrite AlivePattern
					WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
										lPatternPreset);
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
							_TEXT("(): Timeout, no SignOfLife of BootFw!"),
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

	// reset AlivePattern for ensuring that AsicCore sees a changed value if function is called next time
	WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
						0);

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  do ElfCmd 'Load' of BootFw
//************************************************************************

BOOLEAN fnBoard_DoElfCmdLoadBootFw(	DEVICE_EXTENSION*	pDevExtIn,
									volatile UINT8*		pAsicSdramSeg0In,
									uPNCORE_CONFIG_PAR*	pConfigParIn,
									const UINT32		lOffsetFwBufIn,
									const UINT32		lIndexFwIn,
									const UINT32		lIndexFwMaxIn,
									const UINT32		lTimeout_10msecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	// BootFw must be ready for commands

	if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
										pAsicSdramSeg0In,
										pConfigParIn,
										10,					// max. 10msec
										lDimErrorIn,
										sErrorOut))
		// checking SignOfLife of BootFw ok
	{
	UINT32 lAdrAsicSdram_AsicCore = 0;

		// Note:
		//	- BootFw needs addresses which need not to be converted (=view of AsicCore)!
		//	- UserFw is copied to start of SharedAsicSdram_Direct
		//	- use AsicSdram address which always exist independent of a previous loaded firmware (which may change virtual addresses!)

		switch	(pDevExtIn->eAsic)
				// AsicType
		{
			case ePNDEV_ASIC_ERTEC400:		{lAdrAsicSdram_AsicCore = ERTEC400_AHB_ADR__EMIF_SDRAM;					break;}
			case ePNDEV_ASIC_ERTEC200:		{lAdrAsicSdram_AsicCore = ERTEC200_AHB_ADR__EMIF_SDRAM;					break;}
			case ePNDEV_ASIC_SOC1:			{lAdrAsicSdram_AsicCore = SOC1_AHB_ADR__DDR2_SDRAM_BY_MIPS_UNCACHED;	break;}
			case ePNDEV_ASIC_ERTEC200P:		{lAdrAsicSdram_AsicCore = ERTEC200P_AHB_ADR__EMC_SDRAM;					break;}
			default:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid AsicType!"),
								NULL,
								lDimErrorIn,
								sErrorOut);

				break;
			}
		}

		if	(lAdrAsicSdram_AsicCore != 0)
			// success
		{
		_TCHAR	sAction[PNDEV_SIZE_STRING_BUF_SHORT] = {0};
		UINT32	lAdrFwBuf_AsicCore = 0;

			lAdrFwBuf_AsicCore = lAdrAsicSdram_AsicCore + PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT + lOffsetFwBufIn;

			// set BaseAdr of firmware, needed by ELF loader
			WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_BASE),
								lAdrFwBuf_AsicCore);

			// set command 'Load'
			WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_CMD),
								PNDEV_ASIC_SDRAM_ELF_CMD__LOAD);

			// set action
			_RtlStringCchPrintf(sAction,
								_countof(sAction),
								_TEXT(" (command Load, firmware %u of %u)"),
								lIndexFwIn,
								lIndexFwMaxIn);

			if	(fnBoard_WaitForElfCmdDoneBootFw(	pDevExtIn,
													pAsicSdramSeg0In,
													pConfigParIn,
													PNDEV_ASIC_SDRAM_ELF_CMD__LOAD,
													sAction,
													lTimeout_10msecIn,
													lDimErrorIn,
													sErrorOut))
				// waiting for ElfCmdDone of BootFw ok
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
//  do ElfCmd 'Execute' of BootFw
//************************************************************************

BOOLEAN fnBoard_DoElfCmdExecuteBootFw(	DEVICE_EXTENSION*			pDevExtIn,
										volatile UINT8*				pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*			pConfigParIn,
										const BOOLEAN				bDebugModeIn,
										const UINT32				lIndexFwIn,
										const UINT32				lIndexFwMaxIn,
										const UINT32				lTimeout_10msecIn,
										const UINT32				lTimeStartupSignOfLife_10msecIn,
										uPNCORE_STARTUP_PAR_DRIVER*	pStartupParDriverIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut)
{
BOOLEAN bResult = FALSE;

	// BootFw must be ready for next command

	if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
										pAsicSdramSeg0In,
										pConfigParIn,
										10,					// max. 10msec
										lDimErrorIn,
										sErrorOut))
		// checking SignOfLife of BootFw ok
	{
	_TCHAR sAction[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

		// update DriverConfigPar
		//	- must be done before command 'Execute' because Fw needs info at startup (e.g. DebugMode)
		{
			pConfigParIn->bDebugMode = bDebugModeIn;

			if	(pStartupParDriverIn != NULL)
				// valid ptr
			{
				*((uPNCORE_STARTUP_PAR_DRIVER*) &pConfigParIn->uDriver.uStartupPar) = *pStartupParDriverIn;
			}

			// set TimeStartupSignOfLife
			pConfigParIn->lTimeStartupSignOfLife_10msec = lTimeStartupSignOfLife_10msecIn;
		}

		// set command 'Execute'
		WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_CMD),
							PNDEV_ASIC_SDRAM_ELF_CMD__EXECUTE);

		// set action
		_RtlStringCchPrintf(sAction,
							_countof(sAction),
							_TEXT(" (command Execute, firmware %u of %u)"),
							lIndexFwIn,
							lIndexFwMaxIn);

		if	(fnBoard_WaitForElfCmdDoneBootFw(	pDevExtIn,
												pAsicSdramSeg0In,
												pConfigParIn,
												PNDEV_ASIC_SDRAM_ELF_CMD__EXECUTE,
												sAction,
												lTimeout_10msecIn,
												lDimErrorIn,
												sErrorOut))
			// waiting for ElfCmdDone of BootFw ok
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
//  do BinCmd 'Execute' of BootFw
//************************************************************************

BOOLEAN fnBoard_DoBinCmdExecuteBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	// BootFw must be ready for next command

	if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
										pAsicSdramSeg0In,
										pConfigParIn,
										10,					// max. 10msec
										lDimErrorIn,
										sErrorOut))
		// checking SignOfLife of BootFw ok
	{
		// set command 'Execute'
		WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_BIN_CMD),
							PNDEV_ASIC_SDRAM_BIN_CMD__EXECUTE);



		if	(fnBoard_WaitForBinCmdDoneBootFw(	pDevExtIn,
												pAsicSdramSeg0In,
												pConfigParIn,
												PNDEV_ASIC_SDRAM_BIN_CMD__EXECUTE,
												lTimeout_10msecIn,
												lDimErrorIn,
												sErrorOut))
			// waiting for BinCmdDone of BootFw ok
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
//  wait for ElfCmdDone of BootFw
//************************************************************************

BOOLEAN fnBoard_WaitForElfCmdDoneBootFw(DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lCmdIn,
										_TCHAR*				sActionIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_CMD);

			if	(lStatus == lCmdIn)
				// command running
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
				// command done anyway

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
							_TEXT("(): Timeout, no reaction of AsicCore"),
							sActionIn,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_STATUS);

			if	(fnBoard_CheckElfStatusBootFw(	pDevExtIn,
												lStatus,
												sActionIn,
												lDimErrorIn,
												sErrorOut))
				// success
			{
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  wait for BinCmdDone of BootFw
//************************************************************************

BOOLEAN fnBoard_WaitForBinCmdDoneBootFw(DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lCmdIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_BIN_CMD);

			if	(lStatus == lCmdIn)
				// command running
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
				// command done anyway

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
							_TEXT("(): Timeout, no reaction of AsicCore!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_BIN_STATUS);

			if	(fnBoard_CheckBinStatusBootFw(	pDevExtIn,
												lStatus,
												lDimErrorIn,
												sErrorOut))
				// success
			{
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check ELF status of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckElfStatusBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lStatusIn,
										_TCHAR*				sActionIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(lStatusIn == PNDEV_ASIC_SDRAM_ELF_STATUS__SUCCESS)
		// success
	{
		bResult = TRUE;
	}
	else
	{
		switch	(lStatusIn)
				// status
		{
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_NO_ELF:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Invalid format!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_BIT_WIDTH:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Invalid BitWidth!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_ENDIANESS:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Wrong endianess!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_NO_EXECUTABLE:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: No executable!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_LOAD:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Header correct but error at loading!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_BASE_KSEG0_1:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: BaseAddress for loading not in kseg0 or kseg1!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_ENTRY_KSEG0_1:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: EntryAddress for starting not in kseg0 or kseg1!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			default:
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Invalid confirmation (=0x%x)!"),
									lStatusIn);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								sActionIn,
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
//  check BIN status of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckBinStatusBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lStatusIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(lStatusIn == PNDEV_ASIC_SDRAM_BIN_STATUS__SUCCESS)
		// success
	{
		bResult = TRUE;
	}
	else
	{
		switch	(lStatusIn)
				// status
		{
			case PNDEV_ASIC_SDRAM_BIN_STATUS__ERROR_KSEG0_1:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): BIN firmware not in kseg0 or kseg1!"),
								NULL,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			default:
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Invalid confirmation (=0x%x)!"),
									lStatusIn);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
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
//  wait for StartupPar of UserFw
//************************************************************************

BOOLEAN fnBoard_WaitForStartupParUserFw(DEVICE_EXTENSION*	pDevExtIn,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
			if	(!pConfigParIn->uFw.bStartupParValid)
				// StartupPar of UserFw not yet valid
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
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout at waiting for valid StartupPar of UserFw!"),
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
//  wait for Ready of UserFw
//************************************************************************

BOOLEAN fnBoard_WaitForReadyUserFw(	DEVICE_EXTENSION*	pDevExtIn,
									uPNCORE_CONFIG_PAR*	pConfigParIn,
									const UINT32		lTimeout_10msecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
			if	(!pConfigParIn->uFw.bReady)
				// Fw not ready
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
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout at waiting for StartupDone of UserFw!"),
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
//  wait for ack of debugger
//************************************************************************

BOOLEAN fnBoard_WaitForAckDebugger(	DEVICE_EXTENSION*	pDevExtIn,
									uPNCORE_CONFIG_PAR*	pConfigParIn,
									const UINT32		lTimeout_10msecIn,
									const UINT32		lWaitTime_10msecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
			if	(!pConfigParIn->bAckDebugger)
				// semaphore not set
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
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout, no ack of debugger!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			// wait for start of debugger (go) after setting semaphore
			//	- AsicCore must be ready for commands of BootFw

			if	(fnDelayThread(	pDevExtIn,
								lWaitTime_10msecIn,
								lDimErrorIn,
								sErrorOut))
				// starting WaitTime of current thread ok
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
//  initialize LEDs
//************************************************************************

void fnBoard_InitLed(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// HW_Spec_CP1616_V12.Doc:
			//	- Page 17-19

			// LEDs:
			//	- GPIO2:  Activity LED Port0
			//	- GPIO3:  Link LED Port0
			//	- GPIO4:  Activity LED Port1
			//	- GPIO5:  Link LED Port1
			//	- GPIO6:  red SF-LED
			//	- GPIO7:  red BF-LED
			//	- GPIO8:  Activity LED Port2
			//	- GPIO9:  Link LED Port2
			//	- GPIO10: Activity LED Port3
			//	- GPIO11: Link LED Port3
			//	- low active

			// initialize values of GPIOs before setting as output
			{
/*
				// clear SF/BF-LED: GPIO7=GPIO6=1
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									| PNDEV_UINT32_SET_BIT_7
									| PNDEV_UINT32_SET_BIT_6));
*/
				// connect all Link/Activity LEDs to PHYs (GPIOx=1)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									| PNDEV_UINT32_SET_BIT_11
									| PNDEV_UINT32_SET_BIT_10
									| PNDEV_UINT32_SET_BIT_9
									| PNDEV_UINT32_SET_BIT_8
									| PNDEV_UINT32_SET_BIT_5
									| PNDEV_UINT32_SET_BIT_4
									| PNDEV_UINT32_SET_BIT_3
									| PNDEV_UINT32_SET_BIT_2));
			}

			// set as output: all used GPIOs
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__IOCTRL0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__IOCTRL0)
								& PNDEV_UINT32_CLR_BIT_11
								& PNDEV_UINT32_CLR_BIT_10
								& PNDEV_UINT32_CLR_BIT_9
								& PNDEV_UINT32_CLR_BIT_8
//								& PNDEV_UINT32_CLR_BIT_7
//								& PNDEV_UINT32_CLR_BIT_6
								& PNDEV_UINT32_CLR_BIT_5
								& PNDEV_UINT32_CLR_BIT_4
								& PNDEV_UINT32_CLR_BIT_3
								& PNDEV_UINT32_CLR_BIT_2));

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			// EB200_Handbuch_V110.pdf:
			//	- Page 15
			// PH_IEASCs_ERTEC200_V14.pdf:
			//	- Page 103-104

			// LEDs:
			//	- GPIO2:  red BF-LED
			//	- GPIO3:  green SF-LED
			//	- GPIO4:  Link LED Port0,		AltFct2
			//	- GPIO5:  Link LED Port1,		AltFct2
			//	- GPIO6:  Activity LED Port0,	AltFct4
			//	- GPIO7:  Activity LED Port1,	AltFct4
			//	- low active

			// initialize values of GPIOs before setting as output
			{
/*
				// clear SF/BF-LED: GPIO3=GPIO2=1
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
									| PNDEV_UINT32_SET_BIT_3
									| PNDEV_UINT32_SET_BIT_2));
*/
			}

			// set as output: all used GPIOs (independent which AltFct is used)
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__IOCTRL0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__IOCTRL0)
								& PNDEV_UINT32_CLR_BIT_7
								& PNDEV_UINT32_CLR_BIT_6
								& PNDEV_UINT32_CLR_BIT_5
								& PNDEV_UINT32_CLR_BIT_4));
//								& PNDEV_UINT32_CLR_BIT_3
//								& PNDEV_UINT32_CLR_BIT_2));

			// setting of AltFct may additionally be done by SystemEmbedding of EDDI: see EDDI_SysE200GpioSetActivityLinkLed()
			{
				// set AltFct2: GPIO5, GPIO4
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0)
									| (1 << 10)
									| (1 << 8)));

				// set AltFct4: GPIO7, GPIO6
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0)
									| (3 << 14)
									| (3 << 12)));
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
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
//  set state of green and red LED
//************************************************************************

void fnBoard_SetLed(DEVICE_EXTENSION*	pDevExtIn,
					BOOLEAN				bGreenOnIn,
					BOOLEAN				bRedOnIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// HW_Spec_CP1616_V12.Doc:
			//	- Page 17-19

			// LEDs:
			//	- GPIO2:  Activity LED Port0
			//	- GPIO3:  Link LED Port0
			//	- GPIO4:  Activity LED Port1
			//	- GPIO5:  Link LED Port1
			//	- GPIO6:  red SF-LED
			//	- GPIO7:  red BF-LED
			//	- GPIO8:  Activity LED Port2
			//	- GPIO9:  Link LED Port2
			//	- GPIO10: Activity LED Port3
			//	- GPIO11: Link LED Port3
			//	- low active

			// clear SF/BF-LED: GPIO7=GPIO6=1
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
								| PNDEV_UINT32_SET_BIT_7
								| PNDEV_UINT32_SET_BIT_6));

			if	(bGreenOnIn)
				// green LED ON
			{
				// set red SF-LED (GPIO6=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_6));
			}

			if	(bRedOnIn)
				// red LED ON
			{
				// set red BF-LED (GPIO7=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_7));
			}

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			// EB200_Handbuch_V110.pdf:
			//	- Page 15
			// PH_IEASCs_ERTEC200_V14.pdf:
			//	- Page 103-104

			// LEDs:
			//	- GPIO2:  red BF-LED
			//	- GPIO3:  green SF-LED
			//	- GPIO4:  Link LED Port0,		AltFct2
			//	- GPIO5:  Link LED Port1,		AltFct2
			//	- GPIO6:  Activity LED Port0,	AltFct4
			//	- GPIO7:  Activity LED Port1,	AltFct4
			//	- low active

			// clear SF/BF-LED: GPIO3=GPIO2=1
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
								| PNDEV_UINT32_SET_BIT_3
								| PNDEV_UINT32_SET_BIT_2));

			if	(bGreenOnIn)
				// green LED ON
			{
				// set green SF-LED (GPIO3=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_3));
			}

			if	(bRedOnIn)
				// red LED ON
			{
				// set red BF-LED (GPIO2=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_2));
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			break;
		}
		default:
		{
			break;
		}
	}
}
