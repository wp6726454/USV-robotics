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
/*  F i l e               &F: Board_Res.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for handling board resources
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
	#include "Board_Res.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_GetHwResKm)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResKm)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm
//	#pragma alloc_text (PAGE, fnBoard_SetPtrMemMapUM)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_Single)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_UpdatePtrHwResUm_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPer400)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPer200)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPerSoc1)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPer200P)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get HwRes of board
//	- get the HW resources assigned by the bus driver from the start-IRP
//	- map it to system address space
//************************************************************************

BOOLEAN fnBoard_GetHwResKm(	DEVICE_EXTENSION*	pDevExtIn,
							HANDLE				hOsParIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
UINT32	lFirstBar	= 0;

	if	(	(pDevExtIn->eBoard == ePNDEV_BOARD_CP1616)
		|| 	(pDevExtIn->eBoard == ePNDEV_BOARD_DB_EB400_PCIE))
		// CP1616 board or DB-EB400-PCIe board
	{
		// start with BAR1
		lFirstBar = 1;
	}

	if	(fnGetResListConfigSpace(	pDevExtIn,
									hOsParIn,
									lFirstBar,
									lDimErrorIn,
									sErrorOut))
		// getting ResList of ConfigSpace ok
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify BarConfig
//************************************************************************

BOOLEAN fnBoard_VerifyBarConfig(DEVICE_EXTENSION*	pDevExtIn,
								volatile UINT32*	pArrayBarSizeIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;
UINT32	i = 0;

	// preset
	bResult = TRUE;

	for	(i = 0; (i < PNDEV_DIM_ARRAY_BAR); i++)
		// all BARs
	{
	UINT32 lBarSizeTmp = pArrayBarSizeIn[i];

		if	(pDevExtIn->uHwResKm.uBar[i].lSize != lBarSizeTmp)
			// invalid BarSize
		{
		_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// error
			bResult = FALSE;

			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Invalid BAR%u, (expected size: 0x%x, real size: 0x%x)!"),
								i,
								lBarSizeTmp,
								pDevExtIn->uHwResKm.uBar[i].lSize);

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

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set KernelModePtr to HwRes
//************************************************************************

BOOLEAN fnBoard_SetPtrHwResKm(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult								= FALSE;
BOOLEAN	bResultTmp							= FALSE;
UINT32	lArrayBarSize[PNDEV_DIM_ARRAY_BAR]	= {0,0,0,0,0,0};

	// preset
	pDevExtIn->bPcBoard = TRUE;

	// set common MemoryMapping
	pDevExtIn->uAsicSdram.uIndirect[0].lOffsetPhyStart	= PNCORE_SHARED_ASIC_SDRAM__OFFSET_INDIRECT0;
	pDevExtIn->uAsicSdram.uIndirect[0].lSize			= PNCORE_SHARED_ASIC_SDRAM__SIZE_INDIRECT0;
	pDevExtIn->uAsicSdram.uDirect.lOffsetPhyStart		= PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// set BarSize
			lArrayBarSize[0]	= BOARD_CP1616__BAR0_SIZE;
			lArrayBarSize[1]	= BOARD_CP1616__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_CP1616__BAR2_SIZE;
			lArrayBarSize[3]	= BOARD_CP1616__BAR3_SIZE;
			lArrayBarSize[4]	= BOARD_CP1616__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_CP1616__BAR5_SIZE;

			if	(fnBoard_VerifyBarConfig(	pDevExtIn,
											lArrayBarSize,
											lDimErrorIn,
											sErrorOut))
				// verifying BarConfig ok
			{
				// success
				bResultTmp = TRUE;

				// BAR0
				{
				}

				// BAR1
				{
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr			= (pDevExtIn->uHwResKm.uBar[1].pVirtualAdr + BOARD_CP1616__BAR1_OFFSET_APB_PER);
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.lIndexBar		= 1;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.lOffsetBar		= BOARD_CP1616__BAR1_OFFSET_APB_PER;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.lSize			= ERTEC400_AHB_SIZE__APB_PER;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.lAdrAhb			= ERTEC400_AHB_ADR__APB_PER;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.lOffsetPhyStart	= 0;
				}

				// BAR2
				{
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_CP1616__BAR2_OFFSET_SRAM);
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.lIndexBar			= 2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.lOffsetBar		= BOARD_CP1616__BAR2_OFFSET_SRAM;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.lSize				= ERTEC400_AHB_SIZE__SRAM;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.lAdrAhb			= ERTEC400_AHB_ADR__SRAM;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uSram.lOffsetPhyStart	= 0;
				}

				// BAR3
				{
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.pPtr				= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_CP1616__BAR3_OFFSET_IRTE);
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.lIndexBar			= 3;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.lOffsetBar		= BOARD_CP1616__BAR3_OFFSET_IRTE;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.lSize				= ERTEC400_AHB_SIZE__IRTE;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.lAdrAhb			= ERTEC400_AHB_ADR__IRTE;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.lOffsetPhyStart	= 0;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.pIrte_Debug				= (volatile IRTE_SW_MAKRO_T*) pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.pPtr;
					pDevExtIn->pIrte												= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte.pPtr;
				}

				// BAR4
				{
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.pPtr			= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_CP1616__BAR4_OFFSET_EMIF_ASYNC_BANK0);
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.lIndexBar		= 4;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.lOffsetBar		= BOARD_CP1616__BAR4_OFFSET_EMIF_ASYNC_BANK0;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.lSize			= BOARD_CP1616__BAR4_SIZE_EMIF_ASYNC_BANK0;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.lAdrAhb			= ERTEC400_AHB_ADR__EMIF_ASYNC_BANK0;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.lOffsetPhyStart	= 0;
				}

				// BAR5
				{
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr				= (pDevExtIn->uHwResKm.uBar[5].pVirtualAdr + BOARD_CP1616__BAR5_OFFSET_EMIF_SDRAM);
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.lIndexBar		= 5;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.lOffsetBar		= BOARD_CP1616__BAR5_OFFSET_EMIF_SDRAM;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.lSize			= BOARD_CP1616__BAR5_SIZE_EMIF_SDRAM;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.lAdrAhb			= ERTEC400_AHB_ADR__EMIF_SDRAM;
					pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.lOffsetPhyStart	= 0;
				}

				// set MemoryMapping
				{
					// PrivateMemAsic_Fw:	32MB
					// Driver:				32MB
					//	- SharedMemAsic_Indirect1:	none
					//	- SharedMemAsic_Direct:		30MB
					//	- SharedMemAsic_Reserved:	2MB

					pDevExtIn->uAsicSdram.lSizePhy						= (1024*1024*64);
					pDevExtIn->uAsicSdram.lSizeDriver					= (1024*1024*32);
					pDevExtIn->uAsicSdram.uDirect.lSize					= (1024*1024*30);
					pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart	= 0;
					pDevExtIn->uAsicSdram.uIndirect[1].lSize			= 0;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			// set BarSize
			lArrayBarSize[0]	= BOARD_EB200__BAR0_SIZE;
			lArrayBarSize[1]	= BOARD_EB200__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_EB200__BAR2_SIZE;
			lArrayBarSize[3]	= BOARD_EB200__BAR3_SIZE;
			lArrayBarSize[4]	= BOARD_EB200__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_EB200__BAR5_SIZE;

			if	(fnBoard_VerifyBarConfig(	pDevExtIn,
											lArrayBarSize,
											lDimErrorIn,
											sErrorOut))
				// verifying BarConfig ok
			{
				// success
				bResultTmp = TRUE;

				// BAR4
				{
					// LBU registers:
					//	- not accessible by AHB bus
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.pPtr			= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_EB200__BAR4_OFFSET_LBU_REG);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.lIndexBar		= 4;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.lOffsetBar		= BOARD_EB200__BAR4_OFFSET_LBU_REG;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.lSize			= ERTEC200_LBU_REG__SIZE;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uLbuReg.lOffsetPhyStart	= 0;
				}

				// BAR5
				{
					// PciBridge registers:
					//	- not accessible by AHB bus
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr				= (pDevExtIn->uHwResKm.uBar[5].pVirtualAdr + BOARD_EB200__BAR5_OFFSET_PCI_BRIDGE_REG);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.lIndexBar			= 5;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.lOffsetBar		= BOARD_EB200__BAR5_OFFSET_PCI_BRIDGE_REG;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.lSize				= BOARD_EB200_PCI_BRIDGE_REG__SIZE;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.lOffsetPhyStart	= 0;
				}

				// set MemoryMapping
				{
					// PrivateMemAsic_Fw:	none
					// Driver:				64MB
					//	- SharedMemAsic_Indirect1:	60MB
					//	- SharedMemAsic_Direct:		2MB
					//	- SharedMemAsic_Reserved:	2MB

					pDevExtIn->uAsicSdram.lSizePhy						= (1024*1024*64);
					pDevExtIn->uAsicSdram.lSizeDriver					= (1024*1024*64);
					pDevExtIn->uAsicSdram.uDirect.lSize					= (1024*1024*2);
					pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart	= (1024*1024*4);
					pDevExtIn->uAsicSdram.uIndirect[1].lSize			= (1024*1024*60);
				}
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
		BOOLEAN bResultSbl = FALSE;
		UINT32 lSblDetail = 0;

			// preset
			pDevExtIn->eBoardDetail = ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_UNKNOWN;

			// Note:
			//	- for getting BAR configuration the SBL version is required
			//		-> access to TPA and DDR2_SDRAM necessary

			// BAR0
			{
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr			= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_SOC1_DEV__BAR0_OFFSET_TPA_RAM);
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.lIndexBar		= 0;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.lOffsetBar		= BOARD_SOC1_DEV__BAR0_OFFSET_TPA_RAM;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.lSize			= SOC1_AHB_SIZE__TPA_RAM;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.lAdrAhb			= SOC1_AHB_ADR__TPA_RAM;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.lOffsetPhyStart	= 0;
			}

			// BAR5
			{
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.pPtr				= (pDevExtIn->uHwResKm.uBar[5].pVirtualAdr + BOARD_SOC1_DEV__BAR5_OFFSET_DDR2_SDRAM);
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.lIndexBar		= 5;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.lOffsetBar		= BOARD_SOC1_DEV__BAR5_OFFSET_DDR2_SDRAM;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.lSize			= pDevExtIn->uHwResKm.uBar[5].lSize;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.lAdrAhb			= SOC1_AHB_ADR__DDR2_SDRAM;
				pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.lOffsetPhyStart	= 0;
			}

			// check if SBL-S7p

			// get BarConfig
			lSblDetail = READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_BAR_CFG);

			// Note:
			//	- V1: LocalBus0, DDR2=64MB, no PCI-Master
			//	- V2: LocalBus0, DDR2=64MB, PCI-Master

			switch	(lSblDetail)
					// BarConfig
			{
				case 0x0002:	{pDevExtIn->eSblSoc = ePNDEV_SBL_S7P_V2;	break;}
				default:
				{
					break;
				}
			}

			// preset
			bResultSbl = TRUE;

			switch	(pDevExtIn->eSblSoc)
					// SBL
			{
				case ePNDEV_SBL_S7P_V2:
				{
					// set BarSize
					lArrayBarSize[0]	= BOARD_SOC1_DEV__BAR0_SIZE_V2;
					lArrayBarSize[1]	= BOARD_SOC1_DEV__BAR1_SIZE_V2;
					lArrayBarSize[2]	= BOARD_SOC1_DEV__BAR2_SIZE_V2;
					lArrayBarSize[3]	= BOARD_SOC1_DEV__BAR3_SIZE_V2;
					lArrayBarSize[4]	= BOARD_SOC1_DEV__BAR4_SIZE_V2;
					lArrayBarSize[5]	= BOARD_SOC1_DEV__BAR5_SIZE_V2;

					break;
				}
				default:
				{
				_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

					// error
					bResultSbl = FALSE;

					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid SblVersion at flash (SBL-S7p: 1.0.0 ,real: 0x%04x . 0x%04x . 0x%04x)!"),
										pDevExtIn->uSblInfo.uVersion.lMajor,
										pDevExtIn->uSblInfo.uVersion.lMinor,
										pDevExtIn->uSblInfo.uVersion.lBuild);

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);

					break;
				}
			}

			if	(bResultSbl)
				// success
			{
				// get SBL version
				pDevExtIn->uSblInfo.lProduct		= READ_REG_UINT8(	pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_PRODUCT);
				pDevExtIn->uSblInfo.lHw				= READ_REG_UINT16(	pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_HW);
				pDevExtIn->uSblInfo.lSblDetail		= READ_REG_UINT16(	pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_BAR_CFG);
				pDevExtIn->uSblInfo.uVersion.lMajor	= READ_REG_UINT8(	pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_MAJOR);
				pDevExtIn->uSblInfo.uVersion.lMinor	= READ_REG_UINT8(	pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_MINOR);
				pDevExtIn->uSblInfo.uVersion.lBuild	= READ_REG_UINT8(	pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam.pPtr + BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_BUILD);


				if	(	(pDevExtIn->uSblInfo.uVersion.lMajor == 0x1)
					&&	(pDevExtIn->uSblInfo.uVersion.lMinor == 0x0)
					&& 	(pDevExtIn->uSblInfo.uVersion.lBuild == 0x0))
					// valid SblVersion
				{
					if	(pDevExtIn->uSblInfo.lProduct != SPB_PROD_PC_BOARD)
						// not PC board
					{
						// no PcBoard
						pDevExtIn->bPcBoard = FALSE;
					}

					if	(pDevExtIn->bPcBoard)
						// PcBoard
					{
						// Note:
						//	- PcBoard must be checked because DeviceId of DB-Soc1-xx boards may be used by embedded hardware

						switch	(pDevExtIn->eBoard)
								// BoardType
						{
							case ePNDEV_BOARD_DB_SOC1_PCI:	{pDevExtIn->eBoardDetail = ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_EXT;	break;}
							case ePNDEV_BOARD_DB_SOC1_PCIE:	{pDevExtIn->eBoardDetail = ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_INT;	break;}
							default:
							{
								break;
							}
						}
					}

					if	(fnBoard_VerifyBarConfig(	pDevExtIn,
													lArrayBarSize,
													lDimErrorIn,
													sErrorOut))
						// verifying BarConfig ok
					{
						// success
						bResultTmp = TRUE;

						// BAR1
						{
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTtu.pPtr				= (pDevExtIn->uHwResKm.uBar[1].pVirtualAdr + BOARD_SOC1_DEV__BAR1_OFFSET_TRACE_TIMER_UNIT);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTtu.lIndexBar			= 1;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTtu.lOffsetBar			= BOARD_SOC1_DEV__BAR1_OFFSET_TRACE_TIMER_UNIT;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTtu.lSize				= SOC1_AHB_SIZE__TRACE_TIMER_UNIT;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTtu.lAdrAhb			= SOC1_AHB_ADR__TRACE_TIMER_UNIT;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTtu.lOffsetPhyStart	= 0;
						}

						// BAR2
						{
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.lIndexBar		= 2;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.lOffsetBar		= BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.lSize			= BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.lAdrAhb			= SOC1_AHB_ADR__LOCAL_BUS0;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.lOffsetPhyStart	= 0;

							if	(lArrayBarSize[2] == (1024*1024*64))
								// BarSize=64MB
							{
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus1.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS1);
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus1.lIndexBar		= 2;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus1.lOffsetBar		= BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS1;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus1.lSize			= BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS1;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus1.lAdrAhb			= SOC1_AHB_ADR__LOCAL_BUS1;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus1.lOffsetPhyStart	= 0;

								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus2.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS2);
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus2.lIndexBar		= 2;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus2.lOffsetBar		= BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS2;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus2.lSize			= BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS2;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus2.lAdrAhb			= SOC1_AHB_ADR__LOCAL_BUS2;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus2.lOffsetPhyStart	= 0;

								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus3.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS3);
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus3.lIndexBar		= 2;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus3.lOffsetBar		= BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS3;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus3.lSize			= BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS3;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus3.lAdrAhb			= SOC1_AHB_ADR__LOCAL_BUS3;
								pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus3.lOffsetPhyStart	= 0;
							}
						}

						// BAR3
						{
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.pPtr				= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_SOC1_DEV__BAR3_OFFSET_LOCAL_BUS_REG);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.lIndexBar			= 3;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.lOffsetBar			= BOARD_SOC1_DEV__BAR3_OFFSET_LOCAL_BUS_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.lSize				= SOC1_AHB_SIZE__LOCAL_BUS_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.lAdrAhb			= SOC1_AHB_ADR__LOCAL_BUS_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg.lOffsetPhyStart	= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.pPtr						= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_SOC1_DEV__BAR3_OFFSET_IRTE);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.lIndexBar					= 3;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.lOffsetBar				= BOARD_SOC1_DEV__BAR3_OFFSET_IRTE;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.lSize						= SOC1_AHB_SIZE__IRTE;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.lAdrAhb					= SOC1_AHB_ADR__IRTE;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.lOffsetPhyStart			= 0;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.pIrte_Debug						= (volatile IRTE_SW_MAKRO_T*) pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.pPtr;
							pDevExtIn->pIrte													= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte.pPtr;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPbComRam.pPtr					= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_SOC1_DEV__BAR3_OFFSET_PB_COM_RAM);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPbComRam.lIndexBar				= 3;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPbComRam.lOffsetBar			= BOARD_SOC1_DEV__BAR3_OFFSET_PB_COM_RAM;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPbComRam.lSize					= SOC1_AHB_SIZE__PB_COM_RAM;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPbComRam.lAdrAhb				= SOC1_AHB_ADR__PB_COM_RAM;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPbComRam.lOffsetPhyStart		= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciBridgeReg.pPtr				= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_SOC1_DEV__BAR3_OFFSET_PCI_BRIDGE_REG);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciBridgeReg.lIndexBar			= 3;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciBridgeReg.lOffsetBar		= BOARD_SOC1_DEV__BAR3_OFFSET_PCI_BRIDGE_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciBridgeReg.lSize				= SOC1_AHB_SIZE__PCI_BRIDGE_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciBridgeReg.lAdrAhb			= SOC1_AHB_ADR__PCI_BRIDGE_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciBridgeReg.lOffsetPhyStart	= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIocc.pPtr						= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_SOC1_DEV__BAR3_OFFSET_IOCC);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIocc.lIndexBar					= 3;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIocc.lOffsetBar				= BOARD_SOC1_DEV__BAR3_OFFSET_IOCC;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIocc.lSize						= SOC1_AHB_SIZE__IOCC;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIocc.lAdrAhb					= SOC1_AHB_ADR__IOCC;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIocc.lOffsetPhyStart			= 0;
						}

						// BAR4
						{
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uMmcSd.pPtr					= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_MMC_SD);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uMmcSd.lIndexBar			= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uMmcSd.lOffsetBar			= BOARD_SOC1_DEV__BAR4_OFFSET_MMC_SD;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uMmcSd.lSize				= SOC1_AHB_SIZE__MMC_SD;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uMmcSd.lAdrAhb				= SOC1_AHB_ADR__MMC_SD;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uMmcSd.lOffsetPhyStart		= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGdma.pPtr					= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_GDMA);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGdma.lIndexBar				= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGdma.lOffsetBar			= BOARD_SOC1_DEV__BAR4_OFFSET_GDMA;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGdma.lSize					= SOC1_AHB_SIZE__GDMA;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGdma.lAdrAhb				= SOC1_AHB_ADR__GDMA;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGdma.lOffsetPhyStart		= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGicu.pPtr					= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_GICU);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGicu.lIndexBar				= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGicu.lOffsetBar			= BOARD_SOC1_DEV__BAR4_OFFSET_GICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGicu.lSize					= SOC1_AHB_SIZE__GICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGicu.lAdrAhb				= SOC1_AHB_ADR__GICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGicu.lOffsetPhyStart		= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr				= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_PCI_ICU);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.lIndexBar			= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.lOffsetBar			= BOARD_SOC1_DEV__BAR4_OFFSET_PCI_ICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.lSize				= SOC1_AHB_SIZE__PCI_ICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.lAdrAhb				= SOC1_AHB_ADR__PCI_ICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.lOffsetPhyStart		= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uEccIcu.pPtr				= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_ECC_ICU);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uEccIcu.lIndexBar			= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uEccIcu.lOffsetBar			= BOARD_SOC1_DEV__BAR4_OFFSET_ECC_ICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uEccIcu.lSize				= SOC1_AHB_SIZE__ECC_ICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uEccIcu.lAdrAhb				= SOC1_AHB_ADR__ECC_ICU;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uEccIcu.lOffsetPhyStart		= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2cReg.pPtr				= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_DDR2C_REG);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2cReg.lIndexBar			= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2cReg.lOffsetBar		= BOARD_SOC1_DEV__BAR4_OFFSET_DDR2C_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2cReg.lSize				= SOC1_AHB_SIZE__DDR2C_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2cReg.lAdrAhb			= SOC1_AHB_ADR__DDR2C_REG;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2cReg.lOffsetPhyStart	= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDsaSoftPll.pPtr			= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_DSA_SOFT_PLL);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDsaSoftPll.lIndexBar		= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDsaSoftPll.lOffsetBar		= BOARD_SOC1_DEV__BAR4_OFFSET_DSA_SOFT_PLL;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDsaSoftPll.lSize			= SOC1_AHB_SIZE__DSA_SOFT_PLL;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDsaSoftPll.lAdrAhb			= SOC1_AHB_ADR__DSA_SOFT_PLL;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDsaSoftPll.lOffsetPhyStart	= 0;

							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr				= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_SOC1_DEV__BAR4_OFFSET_APB_PER);
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.lIndexBar			= 4;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.lOffsetBar			= BOARD_SOC1_DEV__BAR4_OFFSET_APB_PER;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.lSize				= SOC1_AHB_SIZE__APB_PER;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.lAdrAhb				= SOC1_AHB_ADR__APB_PER;
							pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.lOffsetPhyStart		= 0;
						}

						// set MemoryMapping
						{
							// PrivateMemAsic_Fw:	192MB
							// Driver:				64MB
							//	- SharedMemAsic_Indirect1:	none
							//	- SharedMemAsic_Direct:		62MB
							//	- SharedMemAsic_Reserved:	2MB

							pDevExtIn->uAsicSdram.lSizePhy						= (1024*1024*256);
							pDevExtIn->uAsicSdram.lSizeDriver					= (1024*1024*64);
							pDevExtIn->uAsicSdram.uDirect.lSize					= (1024*1024*62);
							pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart	= 0;
							pDevExtIn->uAsicSdram.uIndirect[1].lSize			= 0;
						}
					}
				}
				else
				{
				_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid SblVersion at flash (SBL-S7p: 1.0.0 ,real: 0x%04x . 0x%04x . 0x%04x)!"),
										pDevExtIn->uSblInfo.uVersion.lMajor,
										pDevExtIn->uSblInfo.uVersion.lMinor,
										pDevExtIn->uSblInfo.uVersion.lBuild);

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
			}

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			// Note:
			//	- for getting BoardDetails the AdminReg is required

			// BAR0
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pAdminReg	= pDevExtIn->uHwResKm.uBar[0].pVirtualAdr;
			pDevExtIn->pAdminReg								= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pAdminReg;

			// get BoardDetails
			{
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				// get BoardId
				pDevExtIn->eBoardId = fnBoardIrtePnip_GetBoardId(pDevExtIn);

				// get BoardRevision
				pDevExtIn->lBoardRevision = fnBoardIrtePnip_GetBoardRevision(pDevExtIn);

				// get BitStream if exist
				pDevExtIn->lBitStreamId			= fnBoardIrtePnip_GetBitStreamId(pDevExtIn);
				pDevExtIn->lBitStreamVersion	= fnBoardIrtePnip_GetBitStreamVersion(pDevExtIn);

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: Board-ID=0x%x",
									pDevExtIn->sPciLocShortAscii,
									pDevExtIn->eBoardId);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}

			switch	(pDevExtIn->eBoard)
					// BoardType
			{
				case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
				{
					switch	(pDevExtIn->eBoardId)
							// BoardId
					{
						case eBOARD_ID_FPGA1_ERTEC200P_AHB_STD:
						case eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200:
						{
							// board with Ertec200P/PCIe-AHB
							pDevExtIn->eBoardDetail = ePNDEV_BOARD_DETAIL_ERTEC200P_AHB;

							// set BarSize
							lArrayBarSize[0]	= BOARD_FPGA1_ERTEC200P__BAR0_SIZE;
							lArrayBarSize[1]	= BOARD_ERTEC200P_AHB__BAR1_SIZE;
							lArrayBarSize[2]	= BOARD_ERTEC200P_AHB__BAR2_SIZE;
							lArrayBarSize[3]	= BOARD_ERTEC200P_AHB__BAR3_SIZE;
							lArrayBarSize[4]	= BOARD_ERTEC200P_AHB__BAR4_SIZE;
							lArrayBarSize[5]	= BOARD_ERTEC200P_AHB__BAR5_SIZE;

							if	(pDevExtIn->eBoardId == eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200)
								// ET200 version
							{
								lArrayBarSize[1]	= BOARD_ERTEC200P_AHB__BAR1_SIZE_ET200;
								lArrayBarSize[4]	= BOARD_ERTEC200P_AHB__BAR4_SIZE_ET200;
							}

							break;
						}
						case eBOARD_ID_FPGA1_ERTEC200P_XHIF_STD:
						case eBOARD_ID_FPGA1_ERTEC200P_XHIF_ET200:
						{
							// board with Ertec200P/PCIe-XHIF
							pDevExtIn->eBoardDetail = ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF;

							// set BarSize
							lArrayBarSize[0]	= BOARD_FPGA1_ERTEC200P__BAR0_SIZE;
							lArrayBarSize[1]	= BOARD_ERTEC200P_XHIF__BAR1_SIZE;
							lArrayBarSize[2]	= BOARD_ERTEC200P_XHIF__BAR2_SIZE;
							lArrayBarSize[3]	= BOARD_ERTEC200P_XHIF__BAR3_SIZE;
							lArrayBarSize[4]	= BOARD_ERTEC200P_XHIF__BAR4_SIZE;
							lArrayBarSize[5]	= BOARD_ERTEC200P_XHIF__BAR5_SIZE;

							break;
						}
						default:
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid BoardId!"),
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
					// board with Ertec200P/PCIe-XHIF
					pDevExtIn->eBoardDetail = ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF;

					// set BarSize
					lArrayBarSize[0]	= BOARD_FPGA1_ERTEC200P__BAR0_SIZE;
					lArrayBarSize[1]	= BOARD_ERTEC200P_XHIF__BAR1_SIZE;
					lArrayBarSize[2]	= BOARD_ERTEC200P_XHIF__BAR2_SIZE;
					lArrayBarSize[3]	= BOARD_ERTEC200P_XHIF__BAR3_SIZE;
					lArrayBarSize[4]	= BOARD_ERTEC200P_XHIF__BAR4_SIZE;
					lArrayBarSize[5]	= BOARD_ERTEC200P_XHIF__BAR5_SIZE;

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

			if	(pDevExtIn->eBoardDetail != ePNDEV_BOARD_DETAIL_INVALID)
				// success
			{
				if	(fnBoard_VerifyBarConfig(	pDevExtIn,
												lArrayBarSize,
												lDimErrorIn,
												sErrorOut))
					// verifying BarConfig ok
				{
					switch	(pDevExtIn->eBoardDetail)
							// BoardDetail
					{
						case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
						{
							// success
							bResultTmp = TRUE;

							// BAR2
							{
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_ARM_ICU);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu.lIndexBar			= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_ARM_ICU;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu.lSize				= ERTEC200P_AHB_SIZE__ARM_ICU;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu.lAdrAhb			= ERTEC200P_AHB_ADR__ARM_ICU;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu.lOffsetPhyStart	= 0;
								pDevExtIn->pArmIcu													= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu.pPtr;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.pPtr					= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_PNIP);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lIndexBar			= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_PNIP;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lSize				= ERTEC200P_AHB_SIZE__PNIP;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lAdrAhb				= ERTEC200P_AHB_ADR__PNIP;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lOffsetPhyStart		= 0;
//								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lPhyAdr				= (pDevExtIn->uHwResKm.uBar[2].uPhyAdr.u.LowPart + BOARD_ERTEC200P_AHB__BAR2_OFFSET_PNIP);
								pDevExtIn->pPnIp													= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.pPtr;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_PERIF);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lIndexBar			= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_PERIF;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lSize				= ERTEC200P_AHB_SIZE__PERIF;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lAdrAhb				= ERTEC200P_AHB_ADR__PERIF;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lOffsetPhyStart		= 0;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.pPtr					= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_GDMA);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lIndexBar			= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_GDMA;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lSize				= ERTEC200P_AHB_SIZE__GDMA;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lAdrAhb				= ERTEC200P_AHB_ADR__GDMA;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lOffsetPhyStart		= 0;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.pPtr					= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_TTU);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lIndexBar				= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_TTU;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lSize					= ERTEC200P_AHB_SIZE__TTU;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lAdrAhb				= ERTEC200P_AHB_ADR__TTU;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lOffsetPhyStart		= 0;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_SD_MMC);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.lIndexBar			= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_SD_MMC;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.lSize				= ERTEC200P_AHB_SIZE__SD_MMC;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.lAdrAhb				= ERTEC200P_AHB_ADR__SD_MMC;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.lOffsetPhyStart		= 0;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_EMC_REG);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.lIndexBar			= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_EMC_REG;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.lSize				= ERTEC200P_AHB_SIZE__EMC_REG;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.lAdrAhb			= ERTEC200P_AHB_ADR__EMC_REG;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.lOffsetPhyStart	= 0;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR2_OFFSET_APB_PER);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lIndexBar			= 2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR2_OFFSET_APB_PER;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lSize				= ERTEC200P_AHB_SIZE__APB_PER;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lAdrAhb			= ERTEC200P_AHB_ADR__APB_PER;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lOffsetPhyStart	= 0;
							}

							// BAR1
							//	- must be after setting pointer of ApbPer (see AsicVersion)!
							{
							UINT32 lScrbId = 0;

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.pPtr			= (pDevExtIn->uHwResKm.uBar[1].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR1_OFFSET_EMC_ASYNC_BANK2);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.lIndexBar		= 1;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.lOffsetBar		= BOARD_ERTEC200P_AHB__BAR1_OFFSET_EMC_ASYNC_BANK2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.lSize			= BOARD_ERTEC200P_AHB__BAR1_SIZE_EMC_ASYNC_BANK2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK2;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.lOffsetPhyStart	= 0;

								if	(pDevExtIn->eBoardId == eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200)
									// ET200 version
								{
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.lSize = BOARD_ERTEC200P_AHB__BAR1_SIZE_EMC_ASYNC_BANK2_ET200;
								}

								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.pPtr				= (pDevExtIn->uHwResKm.uBar[1].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lIndexBar			= 1;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lOffsetBar			= BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lAdrAhb			= ERTEC200P_AHB_ADR__KRISC32_TCM;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lOffsetPhyStart	= 0;

								if	(pDevExtIn->eBoardId == eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200)
									// ET200 version
								{
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.pPtr		= (pDevExtIn->uHwResKm.uBar[1].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM_ET200);
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lOffsetBar	= BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM_ET200;
								}

								// get SCRB-ID
								lScrbId = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__ID);

								// Note:
								//	- FPGA1-Ertec200P board: KRISC32-TCM at TopLevel-FGPA -> size depends on TopLevel version (SCRB-ID)

					            if	(lScrbId == ERTEC200P_SCRB_ID__REV2 || lScrbId == ERTEC200P_SCRB_ID__REV2_HW2)
						            // revision 2 or hw-release Step2
								{
									// set AsicDetail
									pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_PNIP_REV2;

									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lSize = ERTEC200P_AHB_SIZE__KRISC32_TCM_REV2;
								}
								else
								{
									// set AsicDetail
									pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_PNIP_REV1;

									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lSize = ERTEC200P_AHB_SIZE__KRISC32_TCM_REV1;
								}
							}

							// BAR3
							{
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.pPtr				= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR3_OFFSET_EMC_SDRAM);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lIndexBar		= 3;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lOffsetBar		= BOARD_ERTEC200P_AHB__BAR3_OFFSET_EMC_SDRAM;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lSize			= BOARD_ERTEC200P_AHB__BAR3_SIZE_EMC_SDRAM;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lAdrAhb			= ERTEC200P_AHB_ADR__EMC_SDRAM;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lOffsetPhyStart	= 0;
							}

							// BAR4
							{
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr			= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK0);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.lIndexBar		= 4;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.lOffsetBar		= BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK0;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.lSize			= BOARD_ERTEC200P_AHB__BAR4_SIZE_EMC_ASYNC_BANK0;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK0;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.lOffsetPhyStart	= 0;

								if	(pDevExtIn->eBoardId == eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200)
									// ET200 version
								{
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.pPtr			= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK3_ET200);
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.lIndexBar		= 4;
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.lOffsetBar		= BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK3_ET200;
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.lSize			= BOARD_ERTEC200P_AHB__BAR4_SIZE_EMC_ASYNC_BANK3_ET200;
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK3;
									pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.lOffsetPhyStart	= 0;
								}
							}

							// BAR5
							{
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.pPtr			= (pDevExtIn->uHwResKm.uBar[5].pVirtualAdr + BOARD_ERTEC200P_AHB__BAR5_OFFSET_EMC_ASYNC_BANK1);
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.lIndexBar		= 5;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.lOffsetBar		= BOARD_ERTEC200P_AHB__BAR5_OFFSET_EMC_ASYNC_BANK1;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.lSize			= BOARD_ERTEC200P_AHB__BAR5_SIZE_EMC_ASYNC_BANK1;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK1;
								pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.lOffsetPhyStart	= 0;
							}

							// set MemoryMapping
							{
								// PrivateMemAsic_Fw:	64MB
								// Driver:				64MB
								//	- SharedMemAsic_Indirect1:	none
								//	- SharedMemAsic_Direct:		62MB
								//	- SharedMemAsic_Reserved:	2MB

								pDevExtIn->uAsicSdram.lSizePhy						= (1024*1024*128);
								pDevExtIn->uAsicSdram.lSizeDriver					= (1024*1024*64);
								pDevExtIn->uAsicSdram.uDirect.lSize					= (1024*1024*62);
								pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart	= 0;
								pDevExtIn->uAsicSdram.uIndirect[1].lSize			= 0;
							}

							break;
						}
						case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
						{
							// success
							bResultTmp = TRUE;

							// set MemoryMapping
							{
								// PrivateMemAsic_Fw:	none
								// Driver:				128MB
								//	- SharedMemAsic_Indirect1:	124MB
								//	- SharedMemAsic_Direct:		2MB
								//	- SharedMemAsic_Reserved:	2MB

								pDevExtIn->uAsicSdram.lSizePhy						= (1024*1024*128);
								pDevExtIn->uAsicSdram.lSizeDriver					= (1024*1024*128);
								pDevExtIn->uAsicSdram.uDirect.lSize					= (1024*1024*2);
								pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart	= (1024*1024*4);
								pDevExtIn->uAsicSdram.uIndirect[1].lSize			= (1024*1024*124);
							}

							break;
						}
						default:
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid BoardDetail!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

							break;
						}
					}
				}
			}

			break;
		}
		case ePNDEV_BOARD_I210:		// Intel I210 board
		{
			// set BarSize
			// get BarSize dynamically, because the sizes are different
			// it depends on the corresponding nvm image
			
			lArrayBarSize[0]	= pDevExtIn->uHwResKm.uBar[0].lSize;
			lArrayBarSize[1]	= BOARD_I210__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_I210__BAR2_SIZE;
			lArrayBarSize[3]	= pDevExtIn->uHwResKm.uBar[3].lSize;
			lArrayBarSize[4]	= BOARD_I210__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_I210__BAR5_SIZE;

			if	(fnBoard_VerifyBarConfig(	pDevExtIn,
											lArrayBarSize,
											lDimErrorIn,
											sErrorOut))
				// verifying BarConfig ok
			{
				// success
				bResultTmp = TRUE;

				// BAR0
				{
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr				= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.lIndexBar			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.lOffsetBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.lSize				= 0x20000;												// standard register set of first 128kb will be used
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.lOffsetPhyStart	= 0;
				}

				// BAR3
				{
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar3.pPtr				= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar3.lIndexBar			= 3;
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar3.lOffsetBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar3.lSize				= pDevExtIn->uHwResKm.uBar[3].lSize;
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar3.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI210.uBar3.lOffsetPhyStart	= 0;
				}

				// set IcuPtr
				pDevExtIn->pIntelIcu = pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr;

				// RevisionId is bit8:15 of MirrowedId
				pDevExtIn->lBoardRevision = (	(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__MREVID)>>8)
											&	0xFFFFffff);
			}

			break;
		}
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
		{
			// set BarSize
			lArrayBarSize[0]	= pDevExtIn->uHwResKm.uBar[0].lSize;
			lArrayBarSize[1]	= pDevExtIn->uHwResKm.uBar[1].lSize;
			lArrayBarSize[2]	= BOARD_I82574__BAR2_SIZE;
			lArrayBarSize[3]	= pDevExtIn->uHwResKm.uBar[3].lSize;
			lArrayBarSize[4]	= BOARD_I82574__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_I82574__BAR5_SIZE;

			if	(fnBoard_VerifyBarConfig(	pDevExtIn,
											lArrayBarSize,
											lDimErrorIn,
											sErrorOut))
				// verifying BarConfig ok
			{
			UINT32	lTmpValue = 0;

				// success
				bResultTmp = TRUE;

				// BAR0
				{
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr			= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.lIndexBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.lOffsetBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.lSize			= 0x20000;												// standard register set of first 128kb will be used
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.lOffsetPhyStart	= 0;
				}

				// BAR2
				{
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar1.pPtr			= (pDevExtIn->uHwResKm.uBar[1].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar1.lIndexBar		= 1;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar1.lOffsetBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar1.lSize			= pDevExtIn->uHwResKm.uBar[1].lSize;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar1.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar1.lOffsetPhyStart	= 0;
				}

				// BAR3
				{
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar3.pPtr			= (pDevExtIn->uHwResKm.uBar[3].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar3.lIndexBar		= 3;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar3.lOffsetBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar3.lSize			= pDevExtIn->uHwResKm.uBar[3].lSize;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar3.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar3.lOffsetPhyStart	= 0;
				}

				// set IcuPtr
				pDevExtIn->pIntelIcu = pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr;

				// Device Rev Id is located at offset 0x1E at eeprom (15:2 at EERD), bit 0 EERD = start read --> 0x79 (0111 1001)
				// write address
				WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__EERD, (0x79));

				do
				{
					// get register value
					lTmpValue = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__EERD);
				}
				// read operation not done
				while	(((lTmpValue >> BOARD_I82574__EERD_DONE) & 1) != BOARD_I82574__EERD_DONE);

				// read data (bit 31:16)
				pDevExtIn->lBoardRevision = (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__EERD)>>16);
			}

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			// set BarSize
			lArrayBarSize[0]	= BOARD_FPGA1_HERA__BAR0_SIZE;
			lArrayBarSize[1]	= BOARD_FPGA1_HERA__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_FPGA1_HERA__BAR2_SIZE;
			lArrayBarSize[3]	= BOARD_FPGA1_HERA__BAR3_SIZE;
			lArrayBarSize[4]	= BOARD_FPGA1_HERA__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_FPGA1_HERA__BAR5_SIZE;

			// get BoardDetails
			{
			UINT32	lTmpBitStream 	= 0;
			UINT32	lTargetPlatform = 0; 
			UINT32	lIdentification = 0;
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};	

				// get BitStream if exist
				lTmpBitStream = fnReadConfigSpaceBitStreamVer(	pDevExtIn,
                                								0xC10,
                                								lDimErrorIn,
                                								sErrorOut);
				
				// get label Bit10-0
				pDevExtIn->lBitStreamVersion 	= (lTmpBitStream & 0x7FF);
				// get increment Bit15-11
				pDevExtIn->lBitStreamId 		= ((lTmpBitStream >> 11) & 0x1F);
				
				// get target platform (asic=00, fpga=01) Bit 20-19
				lTargetPlatform 		= ((lTmpBitStream >> 19) & 0x3);
				// get identification (Hera=0x14) Bit 31-21
				lIdentification 		= ((lTmpBitStream >> 21) & 0x7FF);				
				
				if	(	(pDevExtIn->lBitStreamVersion < 0x08)
					||	(pDevExtIn->lBitStreamId < 0x02)
					||	(lTargetPlatform != 0x01)
					||	(lIdentification != 0x14))
				{
					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: Invalid BitStream: (real: Inc=0x%x, Label=0x%x, TargetPlat=0x%x, Ident=0x%x, expected: Inc>=0x02 and Label>=0x08, TargetPlat=0x1, Ident=0x14) \n",
										pDevExtIn->sPciLocShortAscii,
										pDevExtIn->lBitStreamId,
										pDevExtIn->lBitStreamVersion,
										lTargetPlatform,
										lIdentification);
					
				}
				else
				{
					// set BitStreamId == 0, because ther is only a BitStramVersion
					pDevExtIn->lBitStreamId = 0x0;
					
					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: BitStreamInc = 0x%x, BitStreamLabel = 0x%x, TargetPlat = 0x%x, Ident = 0x%x \n",
										pDevExtIn->sPciLocShortAscii,
										pDevExtIn->lBitStreamId,
										pDevExtIn->lBitStreamVersion,
										lTargetPlatform,
										lIdentification);
					
					if	(fnBoard_VerifyBarConfig(	pDevExtIn,
													lArrayBarSize,
													lDimErrorIn,
													sErrorOut))
						// verifying BarConfig ok
					{
						// success
						bResultTmp = TRUE;

						// BAR4
						{                                      
							pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.pPtr						= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_DDR3SDRAM);
							pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.lIndexBar				= 4;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.lOffsetBar				= BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_DDR3SDRAM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.lSize					= HERA_AHB_SIZE__DDR3SDRAM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.lAdrAhb					= HERA_AHB_ADR__DDR3SDRAM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.lOffsetPhyStart			= 0;
							
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc_Ddr3Sdram.pPtr				= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_KRISC_DDR3SDRAM);
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc_Ddr3Sdram.lIndexBar			= 4;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc_Ddr3Sdram.lOffsetBar			= BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_KRISC_DDR3SDRAM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc_Ddr3Sdram.lSize				= HERA_AHB_SIZE__KRISC_DDR3SDRAM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc_Ddr3Sdram.lAdrAhb			= HERA_AHB_ADR__KRISC_DDR3SDRAM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc_Ddr3Sdram.lOffsetPhyStart	= 0;
						}
						
						// BAR2
						{                                      
							pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp.pPtr					= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_PNIP);
							pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp.lIndexBar				= 2;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp.lOffsetBar			= BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_PNIP;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp.lSize					= HERA_AHB_SIZE__PNIP;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp.lAdrAhb				= HERA_AHB_ADR__PNIP;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp.lOffsetPhyStart		= 0;
							pDevExtIn->pPnIp												= pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp.pPtr;
							                                                            	
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Atcm.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R5_ATCM);
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Atcm.lIndexBar			= 2;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Atcm.lOffsetBar		= BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R5_ATCM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Atcm.lSize				= HERA_AHB_SIZE__R5_ATCM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Atcm.lAdrAhb			= HERA_AHB_ADR__R5_ATCM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Atcm.lOffsetPhyStart	= 0;
							
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Btcm.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R5_BTCM);
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Btcm.lIndexBar			= 2;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Btcm.lOffsetBar		= BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R5_BTCM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Btcm.lSize				= HERA_AHB_SIZE__R5_BTCM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Btcm.lAdrAhb			= HERA_AHB_ADR__R5_BTCM;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Btcm.lOffsetPhyStart	= 0;
						}
						
						// BAR0
						{                                      
							pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr				= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER);
							pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.lIndexBar			= 0;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.lOffsetBar			= BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.lSize				= HERA_AHB_SIZE__APB_PER;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.lAdrAhb				= HERA_AHB_ADR__APB_PER;
							pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.lOffsetPhyStart		= 0;
						}

						// set MemoryMapping
						{
							// PrivateMemAsic_Fw:	385MB
							// Driver:				127MB
							//	- SharedMemAsic_Indirect1:	none
							//	- SharedMemAsic_Direct:		125MB
							//	- SharedMemAsic_Reserved:	2MB

							pDevExtIn->uAsicSdram.lSizePhy						= (1024*1024*512);
							pDevExtIn->uAsicSdram.lSizeDriver					= (1024*1024*127);
							pDevExtIn->uAsicSdram.uDirect.lSize					= (1024*1024*125);
							pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart	= 0;
							pDevExtIn->uAsicSdram.uIndirect[1].lSize			= 0;
						}
					}
				}

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}

			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
		{
		
			lArrayBarSize[0]	= BOARD_MICREL_KSZ884X__BAR0_SIZE;
			lArrayBarSize[1]	= BOARD_MICREL_KSZ884X__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_MICREL_KSZ884X__BAR2_SIZE;
			lArrayBarSize[3]	= BOARD_MICREL_KSZ884X__BAR3_SIZE;
			lArrayBarSize[4]	= BOARD_MICREL_KSZ884X__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_MICREL_KSZ884X__BAR5_SIZE;

			if	(fnBoard_VerifyBarConfig(	pDevExtIn,
											lArrayBarSize,
											lDimErrorIn,
											sErrorOut))
				// verifying BarConfig ok
			{
				// success
				bResultTmp = TRUE;

				// BAR0
				{
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr				= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.lIndexBar			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.lOffsetBar			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.lSize				= pDevExtIn->uHwResKm.uBar[0].lSize;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.lOffsetPhyStart	= 0;
				}

				// set IcuPtr
				pDevExtIn->pIntelIcu = pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr;

				// RevisionId is bit8:15 of MirrowedId
				pDevExtIn->lBoardRevision = (UINT32)(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0.pPtr + BOARD_MICREL_KSZ88XX__SCIDER));
			}

			break;
		}
		case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
		{
		
			lArrayBarSize[0]	= BOARD_MICREL_KSZ884X__BAR0_SIZE;
			lArrayBarSize[1]	= BOARD_MICREL_KSZ884X__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_MICREL_KSZ884X__BAR2_SIZE;
			lArrayBarSize[3]	= BOARD_MICREL_KSZ884X__BAR3_SIZE;
			lArrayBarSize[4]	= BOARD_MICREL_KSZ884X__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_MICREL_KSZ884X__BAR5_SIZE;

			if	(fnBoard_VerifyBarConfig(	pDevExtIn,
											lArrayBarSize,
											lDimErrorIn,
											sErrorOut))
				// verifying BarConfig ok
			{
				// success
				bResultTmp = TRUE;

				// BAR0
				{
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr				= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.lIndexBar			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.lOffsetBar			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.lSize				= pDevExtIn->uHwResKm.uBar[0].lSize;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.lOffsetPhyStart	= 0;
				}

				// set IcuPtr
				pDevExtIn->pIntelIcu = pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr;

				// RevisionId is bit8:15 of MirrowedId
				pDevExtIn->lBoardRevision = (UINT32)(READ_REG_UINT16(pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0.pPtr + BOARD_MICREL_KSZ88XX__SCIDER));
			}

			break;
		}
		case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
		{
			// set BarSize
			lArrayBarSize[0]	= BOARD_TI_OMAP__BAR0_SIZE;
			lArrayBarSize[1]	= BOARD_TI_OMAP__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_TI_OMAP__BAR2_SIZE;
			lArrayBarSize[3]	= BOARD_TI_OMAP__BAR3_SIZE;
			lArrayBarSize[4]	= BOARD_TI_OMAP__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_TI_OMAP__BAR5_SIZE;

			// get BoardDetails
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};
			
				// BAR2
				{                                      
					pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.pPtr					= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_TI_OMAP__BAR2_OFFSET_EMIF_SDRAM);
					pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.lIndexBar			= 2;
					pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.lOffsetBar			= BOARD_TI_OMAP__BAR2_OFFSET_EMIF_SDRAM;
					pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.lSize				= OMAP_AHB_SIZE__EMIF_SDRAM;
					pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.lAdrAhb				= OMAP_AHB_ADR__EMIF_SDRAM;
					pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.lOffsetPhyStart		= 0;
				}

				pDevExtIn->uSblInfo.lSblDetail		= READ_REG_UINT32(	pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.pPtr + BOARD_TI_OMAP__SDRAM_OFFSET_SBL_VERSION_TYPE);
				pDevExtIn->uSblInfo.uVersion.lMajor	= READ_REG_UINT16(	pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.pPtr	+ BOARD_TI_OMAP__SDRAM_OFFSET_SBL_VERSION_MAJOR);
				pDevExtIn->uSblInfo.uVersion.lMinor	= READ_REG_UINT16(	pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.pPtr	+ BOARD_TI_OMAP__SDRAM_OFFSET_SBL_VERSION_MINOR);
				
				if	(	(pDevExtIn->uSblInfo.uVersion.lMajor == 0x2)
					&&	(pDevExtIn->uSblInfo.uVersion.lMinor == 0x1)
					&& 	(pDevExtIn->uSblInfo.uVersion.lBuild == 0x0))
					// valid SblVersion
				{
					if	(pDevExtIn->uSblInfo.lSblDetail != 0xff550000)
						// false BootLoaderType
					{
						// set ErrorDetail
						_RtlStringCchPrintf(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT("(): Invalid SecondaryBootloader type (SBL-Type: 0xff550000 ,real: 0x%x)!"),
											pDevExtIn->uSblInfo.lSblDetail);
					}
					else
					{
						if	(fnBoard_VerifyBarConfig(	pDevExtIn,
														lArrayBarSize,
														lDimErrorIn,
														sErrorOut))
							// verifying BarConfig ok
						{	
							
							// success
							bResultTmp = TRUE;
											
							// BAR0
							{                                      
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uL4Per1.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_L4PER1);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uL4Per1.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uL4Per1.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_L4PER1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uL4Per1.lSize					= OMAP_AHB_SIZE__L4PER1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uL4Per1.lAdrAhb					= OMAP_AHB_ADR__L4PER1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uL4Per1.lOffsetPhyStart			= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uUart3.pPtr						= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_UART3);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uUart3.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uUart3.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_UART3;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uUart3.lSize					= OMAP_AHB_SIZE__UART3;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uUart3.lAdrAhb					= OMAP_AHB_ADR__UART3;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uUart3.lOffsetPhyStart			= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uTimer.pPtr						= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_TIMER);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uTimer.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uTimer.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_TIMER;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uTimer.lSize					= OMAP_AHB_SIZE__TIMER;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uTimer.lAdrAhb					= OMAP_AHB_ADR__TIMER;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uTimer.lOffsetPhyStart			= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uApbPer.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_APB);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uApbPer.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uApbPer.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_APB;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uApbPer.lSize					= OMAP_AHB_SIZE__APB;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uApbPer.lAdrAhb					= OMAP_AHB_ADR__APB;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uApbPer.lOffsetPhyStart			= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_GMAC_SW);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_GMAC_SW;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw.lSize					= OMAP_AHB_SIZE__GMAC_SW;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw.lAdrAhb					= OMAP_AHB_ADR__GMAC_SW;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw.lOffsetPhyStart			= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCtrlModuleCore.pPtr			= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_CTRL_MODULE_CORE);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCtrlModuleCore.lIndexBar		= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCtrlModuleCore.lOffsetBar		= BOARD_TI_OMAP__BAR0_OFFSET_CTRL_MODULE_CORE;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCtrlModuleCore.lSize			= OMAP_AHB_SIZE__CTRL_MODULE_CORE;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCtrlModuleCore.lAdrAhb			= OMAP_AHB_ADR__CTRL_MODULE_CORE;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCtrlModuleCore.lOffsetPhyStart	= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCoreAon.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_CM_CORE_AON);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCoreAon.lIndexBar			= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCoreAon.lOffsetBar			= BOARD_TI_OMAP__BAR0_OFFSET_CM_CORE_AON;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCoreAon.lSize				= OMAP_AHB_SIZE__CM_CORE_AON;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCoreAon.lAdrAhb				= OMAP_AHB_ADR__CM_CORE_AON;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCoreAon.lOffsetPhyStart		= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCore.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_CM_CORE);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCore.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCore.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_CM_CORE;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCore.lSize					= OMAP_AHB_SIZE__CM_CORE;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCore.lAdrAhb					= OMAP_AHB_ADR__CM_CORE;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCore.lOffsetPhyStart			= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uDmaSystem.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_DMA_SYSTEM);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uDmaSystem.lIndexBar			= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uDmaSystem.lOffsetBar			= BOARD_TI_OMAP__BAR0_OFFSET_DMA_SYSTEM;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uDmaSystem.lSize				= OMAP_AHB_SIZE__DMA_SYSTEM;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uDmaSystem.lAdrAhb				= OMAP_AHB_ADR__DMA_SYSTEM;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uDmaSystem.lOffsetPhyStart		= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uEdma.pPtr						= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_EDMA);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uEdma.lIndexBar					= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uEdma.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_EDMA;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uEdma.lSize						= OMAP_AHB_SIZE__EDMA;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uEdma.lAdrAhb					= OMAP_AHB_ADR__EDMA;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uEdma.lOffsetPhyStart			= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1Config.pPtr				= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_PRUSS1Config);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1Config.lIndexBar			= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1Config.lOffsetBar		= BOARD_TI_OMAP__BAR0_OFFSET_PRUSS1Config;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1Config.lSize				= OMAP_AHB_SIZE__PRUSS1Config;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1Config.lAdrAhb			= OMAP_AHB_ADR__PRUSS1Config;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1Config.lOffsetPhyStart	= 0;
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_PRUSS1);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_PRUSS1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1.lSize					= OMAP_AHB_SIZE__PRUSS1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1.lAdrAhb					= OMAP_AHB_ADR__PRUSS1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1.lOffsetPhyStart			= 0;	
								
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPcieSs1.pPtr					= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + BOARD_TI_OMAP__BAR0_OFFSET_PCIE_SS1);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPcieSs1.lIndexBar				= 0;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPcieSs1.lOffsetBar				= BOARD_TI_OMAP__BAR0_OFFSET_PCIE_SS1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPcieSs1.lSize					= OMAP_AHB_SIZE__PCIE_SS1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPcieSs1.lAdrAhb				= OMAP_AHB_ADR__PCIE_SS1;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uPcieSs1.lOffsetPhyStart		= 0;
							}
							
							// BAR4
							{                                      
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uQSpi.pPtr						= (pDevExtIn->uHwResKm.uBar[4].pVirtualAdr + BOARD_TI_OMAP__BAR4_OFFSET_QSPI);
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uQSpi.lIndexBar					= 4;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uQSpi.lOffsetBar				= BOARD_TI_OMAP__BAR4_OFFSET_QSPI;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uQSpi.lSize						= OMAP_AHB_SIZE__QSPI;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uQSpi.lAdrAhb					= OMAP_AHB_ADR__QSPI;
								pDevExtIn->uHwResKm.uBoard.as.uOmap.uQSpi.lOffsetPhyStart			= 0;
							}
							
							// set MemoryMapping
							{
								// PrivateMemAsic_Fw:	32MB
								// Driver:				32MB
								//	- SharedMemAsic_Indirect1:	none
								//	- SharedMemAsic_Direct:		30MB
								//	- SharedMemAsic_Reserved:	2MB

								pDevExtIn->uAsicSdram.lSizePhy						= (1024*1024*64);
								pDevExtIn->uAsicSdram.lSizeDriver					= (1024*1024*32);
								pDevExtIn->uAsicSdram.uDirect.lSize					= (1024*1024*30);
								pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart	= 0;
								pDevExtIn->uAsicSdram.uIndirect[1].lSize			= 0;
							}
						}
					}
				}
				else
				{
					// set ErrorDetail
					_RtlStringCchPrintf(sErrorDetail,
										_countof(sErrorDetail),
										_TEXT("(): Invalid SblVersion at flash (SBL: 2.1.0 ,real: 0x%04x . 0x%04x . 0x%04x)!"),
										pDevExtIn->uSblInfo.uVersion.lMajor,
										pDevExtIn->uSblInfo.uVersion.lMinor,
										pDevExtIn->uSblInfo.uVersion.lBuild);

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									sErrorDetail,
									NULL,
									lDimErrorIn,
									sErrorOut);
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

	if	(bResultTmp)
		// success
	{
		if	(fnBoard_InitVar(pDevExtIn))
			// initializing variables ok
			//	- cannot be done before pAdminReg is known
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
//  set UserModePtr to HwRes
//************************************************************************

BOOLEAN fnBoard_SetPtrMemMapUM(	DEVICE_EXTENSION*					pDevExtIn,
								const UINT32						lDimErrorIn,
								uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalInOut,
								_TCHAR*								sErrorOut)
{
BOOLEAN	bResult	= FALSE;
UINT32	j		= 0;

	// preset
	bResult = TRUE;

	// set Bar ptr
	for	(j = 0; j < PNDEV_DIM_ARRAY_BAR; j++)
	{
		pInternalInOut->uBarKm[j].uVirtualAdr.pPtr 	= pDevExtIn->uHwResKm.uBar[j].pVirtualAdr;
		pInternalInOut->uBarKm[j].lPhyAdr 			= pDevExtIn->uHwResKm.uBar[j].uPhyAdr.QuadPart;
		pInternalInOut->uBarKm[j].lSize 			= pDevExtIn->uHwResKm.uBar[j].lSize;
	}

	// set Sdram ptr
	{
	UINT32 			lOffsetDirect 		= 0;
	UINT32 			lOffsetPhyStartTmp 	= 0;
	volatile UINT8* pVirtualPtrTmp 		= 0;
	INT64 			lPhysAddrTmp		= 0;

		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_CP1616:			// CP1616 board
			case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
			{
				lOffsetPhyStartTmp 	= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.lOffsetPhyStart;
				pVirtualPtrTmp 		= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr;
				lPhysAddrTmp		= (pDevExtIn->uHwResKm.uBar[5].uPhyAdr.QuadPart + BOARD_CP1616__BAR5_OFFSET_EMIF_SDRAM);

				break;
			}
			case ePNDEV_BOARD_EB200:	// EB200 board
			{
				lOffsetPhyStartTmp 	= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.lOffsetPhyStart;
				pVirtualPtrTmp 		= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram.pPtr;
				lPhysAddrTmp		= (pDevExtIn->uHwResKm.uBar[2].uPhyAdr.QuadPart + BOARD_EB200__BAR2_OFFSET_LBU_PAGE2);

				break;
			}
			case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
			case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
			{
				lOffsetPhyStartTmp 	= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.lOffsetPhyStart;
				pVirtualPtrTmp 		= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.pPtr;
				lPhysAddrTmp		= (pDevExtIn->uHwResKm.uBar[5].uPhyAdr.QuadPart + BOARD_SOC1_DEV__BAR5_OFFSET_DDR2_SDRAM);

				break;
			}
			case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
			case ePNDEV_BOARD_EB200P:			// EB200P board
			{
				lOffsetPhyStartTmp 	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lOffsetPhyStart;
				pVirtualPtrTmp 		= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.pPtr;
				
				if	(pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
					// board with Ertec200P/PCIe-XHIF
				{
					lPhysAddrTmp = (pDevExtIn->uHwResKm.uBar[2].uPhyAdr.QuadPart + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE6);
				}
				else if (pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_AHB)
				{
					lPhysAddrTmp = (pDevExtIn->uHwResKm.uBar[3].uPhyAdr.QuadPart + BOARD_ERTEC200P_AHB__BAR3_OFFSET_EMC_SDRAM);
				}

				break;
			}
			case ePNDEV_BOARD_I210:		// Intel I210 board
			case ePNDEV_BOARD_I82574:	// Intel I82574 board
			{
				// success
				bResult = TRUE;
				break;
			}
			case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
			{
				lOffsetPhyStartTmp 	= pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.lOffsetPhyStart;
				pVirtualPtrTmp 		= pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.pPtr;
				lPhysAddrTmp		= (pDevExtIn->uHwResKm.uBar[4].uPhyAdr.QuadPart + BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_DDR3SDRAM);

				break;
			}
			case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
			case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
			{
				// success
				bResult = TRUE;
				
				break;
			}
			case ePNDEV_BOARD_TI_OMAP:			// TI Omap board
			{
				lOffsetPhyStartTmp 	= pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.lOffsetPhyStart;
				pVirtualPtrTmp 		= pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram.pPtr;
				lPhysAddrTmp		= (pDevExtIn->uHwResKm.uBar[2].uPhyAdr.QuadPart + BOARD_TI_OMAP__BAR2_OFFSET_EMIF_SDRAM);
				
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

		if	(pVirtualPtrTmp != NULL)
			// valid ptr
		{
			if	(lOffsetPhyStartTmp == 0)
				// KernelModePtr references start of physical AsicSdram
			{
				// offset of SharedAsicSdram_Direct to HwRes = 2MB
				lOffsetDirect = PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;
			}
			else	// KernelModePtr references SharedAsicSdram_Direct (2MB page)
			{
				// no offset of SharedAsicSdram_Direct to HwRes
				lOffsetDirect = 0;
			}

			pInternalInOut->uSdramKm.uVirtualAdr.pPtr	= (volatile UINT8*) (pVirtualPtrTmp + lOffsetDirect);
			pInternalInOut->uSdramKm.lSize				= pDevExtIn->uAsicSdram.uDirect.lSize;
			pInternalInOut->uSdramKm.lPhyAdr			= (lPhysAddrTmp + lOffsetDirect);
		}
	}

	// set HostMem ptr
	{
	    if	(pDevExtIn->bUsesHostMem)
	    	// HostMem available
	    {
	    INT32 i = 0;

	    	// preset
	    	bResult = FALSE;

	    	for	(i=3; i>=0; i--)
	    		// valid HostMemIndex
	    	{
	    		if (!g_uMemSys.uHostMem[i].bInUse)
	    			// not InUse
	    		{
		            if	(	((pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_FIRST) && (i == 0))	// only HostMemory0 valid
		            	||	(pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_ALL))					// all HostMemories valid
		            	// valid HostMemory
		            {
						pInternalInOut->uHostMemKm.uVirtualAdr.pPtr = g_uMemSys.uHostMem[i].pAdrVirt;
						pInternalInOut->uHostMemKm.lPhyAdr	 		= g_uMemSys.uHostMem[i].lPhyAdr;
						pInternalInOut->uHostMemKm.lSize			= g_uMemSys.uHostMem[i].lSize;
						pDevExtIn->lUsesHostMemIndex				= i;

						// success
		            	bResult = TRUE;

						break;
		            }
	    		}
	    		else
	    		{
	    			// error
	    			bResult = FALSE;
	    		}
	    	}

	    	if  (!bResult)
	    	    // error
	    	{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Error at setting ptr uHostMemKm for MappingVirtualToUM!"),
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
//  set UserModePtr to HwRes
//************************************************************************

BOOLEAN fnBoard_SetPtrHwResUm(	DEVICE_EXTENSION*					pDevExtIn,
								const UINT32						lDimErrorIn,
								uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
								uPNDEV_HW_RES*						pHwResUmOpenOut,
								volatile UINT8**					pIcuOut,
								uPNDEV_APP*							pAppOut,
								_TCHAR*								sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN	bResultMap	= TRUE;
UINT32	i			= 0;

	for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
	{
		pAppOut->uHwResUm.uBar[i] = pInternalIn->uBarUm[i];
	}

	if	(bResultMap)
		// success
	{
		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_CP1616:			// CP1616 board
			case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
			{
				if	(fnBoard_SetPtrHwResUm_Sdram(	pDevExtIn,
													pAppOut,
													pInternalIn,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram,
													lDimErrorIn,
													&pHwResUmOpenOut->as.uErtec400.uEmifSdram,
													sErrorOut))
					// setting UserModePtr to AsicSdram ok
				{
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec400.uIrte,
													&pHwResUmOpenOut->as.uErtec400.uIrte);

					fnBoard_SetPtrHwResUm_ApbPer400(	pAppOut,
														&pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer,
														&pHwResUmOpenOut->as.uErtec400.uApbPer);

					// no TopLevel-ICU exist

					// DLL: return ptr to IRTE
					*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uErtec400.uIrte.uBase.pPtr;

					if	(fnMapSharedHostMemUM(	pDevExtIn,
												lDimErrorIn,
												pInternalIn,
												&pHwResUmOpenOut->as.uErtec400.uSharedHostSdram,
												sErrorOut))
						// map memory to userspace ok
					{
						// success
						bResult = TRUE;
					}
				}

				break;
			}
			case ePNDEV_BOARD_EB200:	// EB200 board
			{
				if	(fnBoard_SetPtrHwResUm_Sdram(	pDevExtIn,
													pAppOut,
													pInternalIn,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram,
													lDimErrorIn,
													&pHwResUmOpenOut->as.uErtec200.uEmifSdram,
													sErrorOut))
					// setting UserModePtr to AsicSdram ok
				{
					// success
					bResult = TRUE;

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200.uIrte,
													&pHwResUmOpenOut->as.uErtec200.uIrte);

					fnBoard_SetPtrHwResUm_ApbPer200(pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer,
													&pHwResUmOpenOut->as.uErtec200.uApbPer);

					// no TopLevel-ICU exist

					// DLL: return ptr to IRTE
					*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uErtec200.uIrte.uBase.pPtr;
				}

				break;
			}
			case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
			case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
			{
				if	(fnBoard_SetPtrHwResUm_Sdram(	pDevExtIn,
													pAppOut,
													pInternalIn,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram,
													lDimErrorIn,
													&pHwResUmOpenOut->as.uSoc1.uDdr2Sdram,
													sErrorOut))
					// setting UserModePtr to AsicSdram ok
				{
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTpaRam,
													&pHwResUmOpenOut->as.uSoc1.uTpaRam);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uTtu,
													&pHwResUmOpenOut->as.uSoc1.uTtu);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0,
													&pHwResUmOpenOut->as.uSoc1.uLocalBus0);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus1,
													&pHwResUmOpenOut->as.uSoc1.uLocalBus1);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus2,
													&pHwResUmOpenOut->as.uSoc1.uLocalBus2);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus3,
													&pHwResUmOpenOut->as.uSoc1.uLocalBus3);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus4,
													&pHwResUmOpenOut->as.uSoc1.uLocalBus4);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBusReg,
													&pHwResUmOpenOut->as.uSoc1.uLocalBusReg);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIrte,
													&pHwResUmOpenOut->as.uSoc1.uIrte);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPbComRam,
													&pHwResUmOpenOut->as.uSoc1.uPbComRam);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uIocc,
													&pHwResUmOpenOut->as.uSoc1.uIocc);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uMmcSd,
													&pHwResUmOpenOut->as.uSoc1.uMmcSd);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGdma,
													&pHwResUmOpenOut->as.uSoc1.uGdma);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uGicu,
													&pHwResUmOpenOut->as.uSoc1.uGicu);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDsaSoftPll,
													&pHwResUmOpenOut->as.uSoc1.uDsaSoftPll);

					fnBoard_SetPtrHwResUm_ApbPerSoc1(	pAppOut,
														&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer,
														&pHwResUmOpenOut->as.uSoc1.uApbPer);

					// TopLevel-ICU = PCI-ICU

					// DLL: return ptr to PCI-ICU
					{
					uPNDEV_HW_DESC uPciIcuTmp;

						fnBoard_SetPtrHwResUm_Single(	pAppOut,
														&pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu,
														&uPciIcuTmp);

						*pIcuOut = (volatile UINT8*) uPciIcuTmp.uBase.pPtr;
					}

					if	(fnMapSharedHostMemUM(	pDevExtIn,
												lDimErrorIn,
												pInternalIn,
												&pHwResUmOpenOut->as.uSoc1.uSharedHostSdram,
												sErrorOut))
						// map memory to userspace ok
					{
						// success
						bResult = TRUE;

					}
				}

				break;
			}
			case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
			case ePNDEV_BOARD_EB200P:			// EB200P board
			{
				if	(fnBoard_SetPtrHwResUm_Sdram(	pDevExtIn,
													pAppOut,
													pInternalIn,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram,
													lDimErrorIn,
													&pHwResUmOpenOut->as.uErtec200P.uEmcSdram,
													sErrorOut))
					// setting UserModePtr to AsicSdram ok
				{
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm,
													&pHwResUmOpenOut->as.uErtec200P.uKrisc32Tcm);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArm926Tcm,
													&pHwResUmOpenOut->as.uErtec200P.uArm926Tcm);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp,
													&pHwResUmOpenOut->as.uErtec200P.uPnIp);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf,
													&pHwResUmOpenOut->as.uErtec200P.uPerIf);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma,
													&pHwResUmOpenOut->as.uErtec200P.uGdma);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu,
													&pHwResUmOpenOut->as.uErtec200P.uTtu);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc,
													&pHwResUmOpenOut->as.uErtec200P.uSdMmc);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0,
													&pHwResUmOpenOut->as.uErtec200P.uEmcAsyncBank0);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1,
													&pHwResUmOpenOut->as.uErtec200P.uEmcAsyncBank1);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2,
													&pHwResUmOpenOut->as.uErtec200P.uEmcAsyncBank2);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3,
													&pHwResUmOpenOut->as.uErtec200P.uEmcAsyncBank3);

					fnBoard_SetPtrHwResUm_ApbPer200P(	pAppOut,
														&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer,
														&pHwResUmOpenOut->as.uErtec200P.uApbPer);

					switch	(pDevExtIn->eBoardDetail)
							// BoardDetail
					{
						case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
						{
							// success
							bResult = TRUE;

							#ifdef FPGA1_ERTEC200P_ARM_NO_TOPLEVEL_ICU

								// no TopLevel-ICU exist

								// DLL: return ptr to PNIP
								*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uErtec200P.uPnIp.uBase.pPtr;

							#else

								// TopLevel-ICU = ARM-ICU

								// DLL: return ptr to ARM-ICU
								{
								uPNDEV_HW_DESC uArmIcuTmp;

									fnBoard_SetPtrHwResUm_Single(	pAppOut,
																	&pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArmIcu,
																	&uArmIcuTmp);

									*pIcuOut = (volatile UINT8*) uArmIcuTmp.uBase.pPtr;
								}

							#endif

							break;
						}
						case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
						{
							// success
							bResult = TRUE;

							// TopLevel-ICU = PERIF-ICU

							// DLL: return ptr to PERIF-ICU
							*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uErtec200P.uPerIf.uBase.pPtr;

							break;
						}
						default:
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid BoardDetail!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

							break;
						}
					}
				}

				break;
			}
			case ePNDEV_BOARD_I210:	// Intel I210 board
			{
				fnBoard_SetPtrHwResUm_Single(	pAppOut,
												&pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0,
												&pHwResUmOpenOut->as.uI210.uBar0);

				fnBoard_SetPtrHwResUm_Single(	pAppOut,
												&pDevExtIn->uHwResKm.uBoard.as.uI210.uBar3,
												&pHwResUmOpenOut->as.uI210.uBar3);

				// DLL: return ptr to Icu
				*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uI210.uBar0.uBase.pPtr;

				if	(fnMapSharedHostMemUM(	pDevExtIn,
											lDimErrorIn,
											pInternalIn,
											&pHwResUmOpenOut->as.uI210.uSharedHostSdram,
											sErrorOut))
					// map memory to userspace ok
				{
					// success
					bResult = TRUE;
				}

				break;
			}
			case ePNDEV_BOARD_I82574:	// Intel I82574 board
			{
				fnBoard_SetPtrHwResUm_Single(	pAppOut,
												&pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0,
												&pHwResUmOpenOut->as.uI82574.uBar0);

				fnBoard_SetPtrHwResUm_Single(	pAppOut,
												&pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar1,
												&pHwResUmOpenOut->as.uI82574.uBar1);

				fnBoard_SetPtrHwResUm_Single(	pAppOut,
												&pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar3,
												&pHwResUmOpenOut->as.uI82574.uBar3);

				// DLL: return ptr to Icu
				*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uI82574.uBar0.uBase.pPtr;

				if	(fnMapSharedHostMemUM(	pDevExtIn,
											lDimErrorIn,
											pInternalIn,
											&pHwResUmOpenOut->as.uI82574.uSharedHostSdram,
											sErrorOut))
					// map memory to userspace ok
				{
					// success
					bResult = TRUE;
				}

				break;
			}
			case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
			{
				if	(fnBoard_SetPtrHwResUm_Sdram(	pDevExtIn,
													pAppOut,
													pInternalIn,
													&pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram,
													lDimErrorIn,
													&pHwResUmOpenOut->as.uHera.uDdr3Sdram,
													sErrorOut))
					// setting UserModePtr to AsicSdram ok
				{		
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc_Ddr3Sdram,
													&pHwResUmOpenOut->as.uHera.uKriscDdr3Sdram);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Atcm,
													&pHwResUmOpenOut->as.uHera.uR5Atcm);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uHera.uKrisc32_Btcm,
													&pHwResUmOpenOut->as.uHera.uR5Btcm);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uHera.uPnIp,
													&pHwResUmOpenOut->as.uHera.uPnIp);

					fnBoard_SetPtrHwResUm_ApbPerHera(	pAppOut,
														&pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer,
														&pHwResUmOpenOut->as.uHera.uApbPer);

					if	(fnMapSharedHostMemUM(	pDevExtIn,
												lDimErrorIn,
												pInternalIn,
												&pHwResUmOpenOut->as.uHera.uSharedHostSdram,
												sErrorOut))
						// map memory to userspace ok
					{
						// success
						bResult = TRUE;
					}
				}
				
				break;
			}
			case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8842 board
			{
				fnBoard_SetPtrHwResUm_Single(	pAppOut,
												&pDevExtIn->uHwResKm.uBoard.as.uKSZ8841.uBar0,
												&pHwResUmOpenOut->as.uKSZ8841.uBar0);

				// DLL: return ptr to Icu
				*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uKSZ8841.uBar0.uBase.pPtr;

				if	(fnMapSharedHostMemUM(	pDevExtIn,
											lDimErrorIn,
											pInternalIn,
											&pHwResUmOpenOut->as.uKSZ8841.uSharedHostSdram,
											sErrorOut))
					// map memory to userspace ok
				{
					// success
					bResult = TRUE;
				}

				break;
			}
			case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
			{
				fnBoard_SetPtrHwResUm_Single(	pAppOut,
												&pDevExtIn->uHwResKm.uBoard.as.uKSZ8842.uBar0,
												&pHwResUmOpenOut->as.uKSZ8842.uBar0);

				// DLL: return ptr to Icu
				*pIcuOut = (volatile UINT8*) pHwResUmOpenOut->as.uKSZ8842.uBar0.uBase.pPtr;

				if	(fnMapSharedHostMemUM(	pDevExtIn,
											lDimErrorIn,
											pInternalIn,
											&pHwResUmOpenOut->as.uKSZ8842.uSharedHostSdram,
											sErrorOut))
					// map memory to userspace ok
				{
					// success
					bResult = TRUE;
				}

				break;
			}
			case ePNDEV_BOARD_TI_OMAP:	// TI Omap board
			{
				if	(fnBoard_SetPtrHwResUm_Sdram(	pDevExtIn,
													pAppOut,
													pInternalIn,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uEmifSdram,
													lDimErrorIn,
													&pHwResUmOpenOut->as.uOmap.uEmifSdram,
													sErrorOut))
					// setting UserModePtr to AsicSdram ok
				{
	
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uL4Per1,
													&pHwResUmOpenOut->as.uOmap.uL4Per1);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uUart3,
													&pHwResUmOpenOut->as.uOmap.uUart3);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uTimer,
													&pHwResUmOpenOut->as.uOmap.uTimer);

					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw,
													&pHwResUmOpenOut->as.uOmap.uGmacSw);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uCtrlModuleCore,
													&pHwResUmOpenOut->as.uOmap.uCtrlModuleCore);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCoreAon,
													&pHwResUmOpenOut->as.uOmap.uCmCoreAon);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uCmCore,
													&pHwResUmOpenOut->as.uOmap.uCmCore);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uDmaSystem,
													&pHwResUmOpenOut->as.uOmap.uDmaSystem);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uEdma,
													&pHwResUmOpenOut->as.uOmap.uEdma);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1Config,
													&pHwResUmOpenOut->as.uOmap.uPruss1Config);
					
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uPruss1,
													&pHwResUmOpenOut->as.uOmap.uPruss1);
								
					fnBoard_SetPtrHwResUm_Single(	pAppOut,
													&pDevExtIn->uHwResKm.uBoard.as.uOmap.uQSpi,
													&pHwResUmOpenOut->as.uOmap.uQSpi);


					fnBoard_SetPtrHwResUm_ApbPerOmap(	pAppOut,
														&pDevExtIn->uHwResKm.uBoard.as.uOmap.uApbPer,
														&pHwResUmOpenOut->as.uOmap.uApbPer);

					if	(fnMapSharedHostMemUM(	pDevExtIn,
												lDimErrorIn,
												pInternalIn,
												&pHwResUmOpenOut->as.uOmap.uSharedHostSdram,
												sErrorOut))
						// map memory to userspace ok
					{
						// success
						bResult = TRUE;
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
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set UserModePtr to a single HwRes
//************************************************************************

void fnBoard_SetPtrHwResUm_Single(	uPNDEV_APP*			pAppIn,
									uPNDEV_HW_DESC_KM*	pHwDescKmIn,
									uPNDEV_HW_DESC*		pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 lSizeHwRes	= 0;
	UINT32 lSizeBar		= 0;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		pHwDescUmOut->uBase.pPtr	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr + pHwDescKmIn->lOffsetBar;
		pHwDescUmOut->lOffset		= 0;
		pHwDescUmOut->lSize			= lSizeHwRes;
		pHwDescUmOut->lAdrAhb		= pHwDescKmIn->lAdrAhb;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to AsicSdram
//************************************************************************

BOOLEAN fnBoard_SetPtrHwResUm_Sdram(DEVICE_EXTENSION*					pDevExtIn,
									uPNDEV_APP*							pAppIn,
									uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
									uPNDEV_HW_DESC_KM*					pHwDescKmIn,
									const UINT32						lDimErrorIn,
									uPNDEV_HW_DESC_SDRAM*				pHwDescUmOut,
									_TCHAR*								sErrorOut)
{
BOOLEAN	bResult			= FALSE;
UINT32	lOffsetDirect	= 0;

	// Note:
	//	- board with SdramPage:	KernelModePtr references start of physical AsicSdram
	//		else:				KernelModePtr references SharedAsicSdram_Direct (2MB page)
	//	- only SharedAsicSdram_Direct will be mapped to UserMode
	//	- mapping of complete BAR would cause some inconvenience:
	//		no protection of SharedAsicSdram_Reserved
	//		WinXP:	if BarSize=64MB:	reducing of SharedAsicSdram_Direct to 61,94MB (PNDEV_MAX_SIZE_MEM_UM-2MB) instead of 62MB,
	//									further exceptions if SharedAsicSdram_Direct is reduced by Fw

	if	(pHwDescKmIn->lOffsetPhyStart == 0)
		// KernelModePtr references start of physical AsicSdram
	{
		// offset of SharedAsicSdram_Direct to HwRes = 2MB
		lOffsetDirect = PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;
	}
	else	// KernelModePtr references SharedAsicSdram_Direct (2MB page)
	{
		// no offset of SharedAsicSdram_Direct to HwRes
		lOffsetDirect = 0;
	}

	if	(pInternalIn->uSdramUm.uAdrUser.pPtr != NULL)
		// valid ptr
	{
		// success
		bResult = TRUE;

		// set UserModePtr
		pAppIn->uHwResUm.uAsicSdramDirect.uAdrUser.pPtr	= pInternalIn->uSdramUm.uAdrUser.pPtr;
		pAppIn->uHwResUm.uAsicSdramDirect.lSize			= pInternalIn->uSdramUm.lSize;
		pAppIn->uHwResUm.uAsicSdramDirect.uMdl.pPtr		= pInternalIn->uSdramUm.uMdl.pPtr;

		pHwDescUmOut->uDirect.uBase.pPtr	= pAppIn->uHwResUm.uAsicSdramDirect.uAdrUser.pPtr;
		pHwDescUmOut->uDirect.lOffset		= pDevExtIn->uAsicSdram.uDirect.lOffsetPhyStart;
		pHwDescUmOut->uDirect.lSize			= pAppIn->uHwResUm.uAsicSdramDirect.lSize;
		pHwDescUmOut->uDirect.lAdrAhb		= pHwDescKmIn->lAdrAhb + lOffsetDirect;

		pHwDescUmOut->lSizePhy				= pDevExtIn->uAsicSdram.lSizePhy;
		pHwDescUmOut->lSizeDriver			= pDevExtIn->uAsicSdram.lSizeDriver;
		pHwDescUmOut->uIndirect[0].lOffset	= pDevExtIn->uAsicSdram.uIndirect[0].lOffsetPhyStart;
		pHwDescUmOut->uIndirect[0].lSize	= pDevExtIn->uAsicSdram.uIndirect[0].lSize;
		pHwDescUmOut->uIndirect[0].lAdrAhb	= (pHwDescKmIn->lAdrAhb - pHwDescKmIn->lOffsetPhyStart) + pDevExtIn->uAsicSdram.uIndirect[0].lOffsetPhyStart;

		pHwDescUmOut->uIndirect[1].lOffset	= pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart;
		pHwDescUmOut->uIndirect[1].lSize	= pDevExtIn->uAsicSdram.uIndirect[1].lSize;

		if	(pDevExtIn->uAsicSdram.uIndirect[1].lSize)
			// SharedAsicSdram_Indirect1 exist
		{
			pHwDescUmOut->uIndirect[1].lAdrAhb = (pHwDescKmIn->lAdrAhb - pHwDescKmIn->lOffsetPhyStart) + pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart;
		}
	}
	else
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): No valid ptr from MappingVirtualToUM!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  update UserModePtr to AsicSdram
//************************************************************************

BOOLEAN fnBoard_UpdatePtrHwResUm_Sdram(	uPNCORE_STARTUP_PAR_FW*	pStartupParFwIn,
										const UINT32			lDimErrorIn,
										uPNDEV_HW_DESC_SDRAM*	pHwDescUmInOut,
										_TCHAR*					sErrorOut)
{
BOOLEAN bResult				= FALSE;
BOOLEAN bResultIndirect1	= TRUE;

	if	(pStartupParFwIn->lSizeIndirect1 != 0)
		// setup SharedAsicSdram_Indirect1
	{
		// preset
		bResultIndirect1 = FALSE;

		if	(pHwDescUmInOut->uIndirect[1].lSize != 0)
			// SharedAsicSdram_Indirect1 already exist
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Firmware: SharedAsicSdram_Indirect1 already exist!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else if	((pStartupParFwIn->lSizeIndirect1 % 4) != 0)
				// size of SharedAsicSdram_Indirect1 not multiple of 4
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Firmware: Size of new SharedAsicSdram_Indirect1 is not a multiple of 4!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else if	(pStartupParFwIn->lSizeIndirect1 > (pHwDescUmInOut->uDirect.lSize - (1024*1024*2)))
				// reduced SharedAsicSdram_Direct < 2MB
				//	- SharedAsicSdram_Direct always >= 2MB -> difference >= 0
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Firmware: Reduced SharedAsicSdram_Direct must have a minimal size of 2MB!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			// success
			bResultIndirect1 = TRUE;

			// reduce SharedAsicSdram_Direct
			pHwDescUmInOut->uDirect.lSize -= pStartupParFwIn->lSizeIndirect1;

			// set SharedAsicSdram_Indirect1
			pHwDescUmInOut->uIndirect[1].lOffset	= pHwDescUmInOut->uDirect.lOffset + pHwDescUmInOut->uDirect.lSize;
			pHwDescUmInOut->uIndirect[1].lSize		= pStartupParFwIn->lSizeIndirect1;
			pHwDescUmInOut->uIndirect[1].lAdrAhb	= pHwDescUmInOut->uDirect.lAdrAhb + pHwDescUmInOut->uDirect.lSize;
		}
	}

	if	(bResultIndirect1)
		// success
	{
	UINT32	lAdrAhbPhyStart	= 0;
	BOOLEAN	bResultHif		= TRUE;

		// compute AhbAdr of physical start of AsicSdram
		lAdrAhbPhyStart = pHwDescUmInOut->uDirect.lAdrAhb - pHwDescUmInOut->uDirect.lOffset;

		if	(pStartupParFwIn->uHif.lSize != 0)
			// HIF exist
		{
			// preset
			bResultHif = FALSE;

			if	(	(pStartupParFwIn->uHif.lOffset									< pHwDescUmInOut->uDirect.lOffset)
				||	((pStartupParFwIn->uHif.lOffset + pStartupParFwIn->uHif.lSize)	> (pHwDescUmInOut->uDirect.lOffset + pHwDescUmInOut->uDirect.lSize)))
				// FwHif offset outside of SharedAsicSdram_Direct
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Firmware: HIF outside of SharedAsicSdram_Direct!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResultHif = TRUE;

				// set FwHif
				pHwDescUmInOut->uFwHif.uBase.pPtr	= (pHwDescUmInOut->uDirect.uBase.pPtr - pHwDescUmInOut->uDirect.lOffset) + pStartupParFwIn->uHif.lOffset;
				pHwDescUmInOut->uFwHif.lOffset		= pStartupParFwIn->uHif.lOffset;
				pHwDescUmInOut->uFwHif.lSize		= pStartupParFwIn->uHif.lSize;
				pHwDescUmInOut->uFwHif.lAdrAhb		= lAdrAhbPhyStart + pStartupParFwIn->uHif.lOffset;
			}
		}

		if	(bResultHif)
			// success
		{
			// success
			bResult = TRUE;

			if	(pStartupParFwIn->uFatal.lSize != 0)
				// FatalBuffer exist
			{
				// set FwFatal
				pHwDescUmInOut->uFwFatal.lOffset	= pStartupParFwIn->uFatal.lOffset;
				pHwDescUmInOut->uFwFatal.lSize		= pStartupParFwIn->uFatal.lSize;
				pHwDescUmInOut->uFwFatal.lAdrAhb	= lAdrAhbPhyStart + pStartupParFwIn->uFatal.lOffset;
			}

			if	(pStartupParFwIn->uTrace[0].lSize != 0)
				// TraceBuffer0 exist
			{
				pHwDescUmInOut->uFwTrace[0].lOffset	= pStartupParFwIn->uTrace[0].lOffset;
				pHwDescUmInOut->uFwTrace[0].lSize	= pStartupParFwIn->uTrace[0].lSize;
				pHwDescUmInOut->uFwTrace[0].lAdrAhb	= lAdrAhbPhyStart + pStartupParFwIn->uTrace[0].lOffset;
			}

			if	(pStartupParFwIn->uTrace[1].lSize != 0)
				// TraceBuffer1 exist
			{
				pHwDescUmInOut->uFwTrace[1].lOffset	= pStartupParFwIn->uTrace[1].lOffset;
				pHwDescUmInOut->uFwTrace[1].lSize	= pStartupParFwIn->uTrace[1].lSize;
				pHwDescUmInOut->uFwTrace[1].lAdrAhb	= lAdrAhbPhyStart + pStartupParFwIn->uTrace[1].lOffset;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec400)
//************************************************************************

void fnBoard_SetPtrHwResUm_ApbPer400(	uPNDEV_APP*					pAppIn,
										uPNDEV_HW_DESC_KM*			pHwDescKmIn,
										uPNDEV_HW_DESC_APB_PER_400*	pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 			lSizeHwRes	= 0;
	UINT32 			lSizeBar	= 0;
	volatile UINT8*	pBaseApbPer	= NULL;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		// get BasePtr to ApbPer
		pBaseApbPer = (((volatile UINT8*) pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr) + pHwDescKmIn->lOffsetBar);

		pHwDescUmOut->uTimer0_1.uBase.pPtr	= pBaseApbPer + ERTEC400_APB_PER__OFFSET_TIMER0_1;
		pHwDescUmOut->uTimer0_1.lOffset		= ERTEC400_APB_PER__OFFSET_TIMER0_1;
		pHwDescUmOut->uTimer0_1.lSize		= ERTEC400_APB_PER__SIZE_TIMER0_1;
		pHwDescUmOut->uTimer0_1.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC400_APB_PER__OFFSET_TIMER0_1;

		pHwDescUmOut->uWatchdog.uBase.pPtr	= pBaseApbPer + ERTEC400_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lOffset		= ERTEC400_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lSize		= ERTEC400_APB_PER__SIZE_WATCHDOG;
		pHwDescUmOut->uWatchdog.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC400_APB_PER__OFFSET_WATCHDOG;

		pHwDescUmOut->uSpi.uBase.pPtr		= pBaseApbPer + ERTEC400_APB_PER__OFFSET_SPI;
		pHwDescUmOut->uSpi.lOffset			= ERTEC400_APB_PER__OFFSET_SPI;
		pHwDescUmOut->uSpi.lSize			= ERTEC400_APB_PER__SIZE_SPI;
		pHwDescUmOut->uSpi.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC400_APB_PER__OFFSET_SPI;

		pHwDescUmOut->uUart1.uBase.pPtr		= pBaseApbPer + ERTEC400_APB_PER__OFFSET_UART1;
		pHwDescUmOut->uUart1.lOffset		= ERTEC400_APB_PER__OFFSET_UART1;
		pHwDescUmOut->uUart1.lSize			= ERTEC400_APB_PER__SIZE_UART1;
		pHwDescUmOut->uUart1.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC400_APB_PER__OFFSET_UART1;

		pHwDescUmOut->uUart2.uBase.pPtr		= pBaseApbPer + ERTEC400_APB_PER__OFFSET_UART2;
		pHwDescUmOut->uUart2.lOffset		= ERTEC400_APB_PER__OFFSET_UART2;
		pHwDescUmOut->uUart2.lSize			= ERTEC400_APB_PER__SIZE_UART2;
		pHwDescUmOut->uUart2.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC400_APB_PER__OFFSET_UART2;

		pHwDescUmOut->uGpio.uBase.pPtr		= pBaseApbPer + ERTEC400_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lOffset			= ERTEC400_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lSize			= ERTEC400_APB_PER__SIZE_GPIO;
		pHwDescUmOut->uGpio.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC400_APB_PER__OFFSET_GPIO;

		pHwDescUmOut->uScrb.uBase.pPtr		= pBaseApbPer + ERTEC400_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lOffset			= ERTEC400_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lSize			= ERTEC400_APB_PER__SIZE_SCRB;
		pHwDescUmOut->uScrb.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC400_APB_PER__OFFSET_SCRB;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec200)
//************************************************************************

void fnBoard_SetPtrHwResUm_ApbPer200(	uPNDEV_APP*					pAppIn,
										uPNDEV_HW_DESC_KM*			pHwDescKmIn,
										uPNDEV_HW_DESC_APB_PER_200*	pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 			lSizeHwRes	= 0;
	UINT32 			lSizeBar	= 0;
	volatile UINT8*	pBaseApbPer	= NULL;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		// get BasePtr to ApbPer
		pBaseApbPer = (((volatile UINT8*) pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr) + pHwDescKmIn->lOffsetBar);

		pHwDescUmOut->uTimer0_2.uBase.pPtr	= pBaseApbPer + ERTEC200_APB_PER__OFFSET_TIMER0_2;
		pHwDescUmOut->uTimer0_2.lOffset		= ERTEC200_APB_PER__OFFSET_TIMER0_2;
		pHwDescUmOut->uTimer0_2.lSize		= ERTEC200_APB_PER__SIZE_TIMER0_2;
		pHwDescUmOut->uTimer0_2.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200_APB_PER__OFFSET_TIMER0_2;

		pHwDescUmOut->uWatchdog.uBase.pPtr	= pBaseApbPer + ERTEC200_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lOffset		= ERTEC200_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lSize		= ERTEC200_APB_PER__SIZE_WATCHDOG;
		pHwDescUmOut->uWatchdog.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200_APB_PER__OFFSET_WATCHDOG;

		pHwDescUmOut->uSpi.uBase.pPtr		= pBaseApbPer + ERTEC200_APB_PER__OFFSET_SPI;
		pHwDescUmOut->uSpi.lOffset			= ERTEC200_APB_PER__OFFSET_SPI;
		pHwDescUmOut->uSpi.lSize			= ERTEC200_APB_PER__SIZE_SPI;
		pHwDescUmOut->uSpi.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200_APB_PER__OFFSET_SPI;

		pHwDescUmOut->uUart.uBase.pPtr		= pBaseApbPer + ERTEC200_APB_PER__OFFSET_UART;
		pHwDescUmOut->uUart.lOffset			= ERTEC200_APB_PER__OFFSET_UART;
		pHwDescUmOut->uUart.lSize			= ERTEC200_APB_PER__SIZE_UART;
		pHwDescUmOut->uUart.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200_APB_PER__OFFSET_UART;

		pHwDescUmOut->uGpio.uBase.pPtr		= pBaseApbPer + ERTEC200_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lOffset			= ERTEC200_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lSize			= ERTEC200_APB_PER__SIZE_GPIO;
		pHwDescUmOut->uGpio.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200_APB_PER__OFFSET_GPIO;

		pHwDescUmOut->uScrb.uBase.pPtr		= pBaseApbPer + ERTEC200_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lOffset			= ERTEC200_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lSize			= ERTEC200_APB_PER__SIZE_SCRB;
		pHwDescUmOut->uScrb.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200_APB_PER__OFFSET_SCRB;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec400)
//************************************************************************

void fnBoard_SetPtrHwResUm_ApbPerSoc1(	uPNDEV_APP*						pAppIn,
										uPNDEV_HW_DESC_KM*				pHwDescKmIn,
										uPNDEV_HW_DESC_APB_PER_SOC1*	pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 			lSizeHwRes	= 0;
	UINT32 			lSizeBar	= 0;
	volatile UINT8*	pBaseApbPer	= NULL;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		// get BasePtr to ApbPer
		pBaseApbPer = (((volatile UINT8*) pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr) + pHwDescKmIn->lOffsetBar);

		pHwDescUmOut->uI2c.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_I2C;
		pHwDescUmOut->uI2c.lOffset				= SOC1_APB_PER__OFFSET_I2C;
		pHwDescUmOut->uI2c.lSize				= SOC1_APB_PER__SIZE_I2C;
		pHwDescUmOut->uI2c.lAdrAhb				= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_I2C;

		pHwDescUmOut->uPbKriscReg.uBase.pPtr	= pBaseApbPer + SOC1_APB_PER__OFFSET_PB_KRISC_REG;
		pHwDescUmOut->uPbKriscReg.lOffset		= SOC1_APB_PER__OFFSET_PB_KRISC_REG;
		pHwDescUmOut->uPbKriscReg.lSize			= SOC1_APB_PER__SIZE_PB_KRISC_REG;
		pHwDescUmOut->uPbKriscReg.lAdrAhb		= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_PB_KRISC_REG;

		pHwDescUmOut->uUart1.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_UART1;
		pHwDescUmOut->uUart1.lOffset			= SOC1_APB_PER__OFFSET_UART1;
		pHwDescUmOut->uUart1.lSize				= SOC1_APB_PER__SIZE_UART1;
		pHwDescUmOut->uUart1.lAdrAhb			= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_UART1;

		pHwDescUmOut->uUart2.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_UART2;
		pHwDescUmOut->uUart2.lOffset			= SOC1_APB_PER__OFFSET_UART2;
		pHwDescUmOut->uUart2.lSize				= SOC1_APB_PER__SIZE_UART2;
		pHwDescUmOut->uUart2.lAdrAhb			= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_UART2;

		pHwDescUmOut->uUart3.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_UART3;
		pHwDescUmOut->uUart3.lOffset			= SOC1_APB_PER__OFFSET_UART3;
		pHwDescUmOut->uUart3.lSize				= SOC1_APB_PER__SIZE_UART3;
		pHwDescUmOut->uUart3.lAdrAhb			= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_UART3;

		pHwDescUmOut->uSpi.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_SPI;
		pHwDescUmOut->uSpi.lOffset				= SOC1_APB_PER__OFFSET_SPI;
		pHwDescUmOut->uSpi.lSize				= SOC1_APB_PER__SIZE_SPI;
		pHwDescUmOut->uSpi.lAdrAhb				= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_SPI;

		pHwDescUmOut->uWatchdog.uBase.pPtr		= pBaseApbPer + SOC1_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lOffset			= SOC1_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lSize			= SOC1_APB_PER__SIZE_WATCHDOG;
		pHwDescUmOut->uWatchdog.lAdrAhb			= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_WATCHDOG;

		pHwDescUmOut->uSemaphore.uBase.pPtr		= pBaseApbPer + SOC1_APB_PER__OFFSET_SEMAPHORE;
		pHwDescUmOut->uSemaphore.lOffset		= SOC1_APB_PER__OFFSET_SEMAPHORE;
		pHwDescUmOut->uSemaphore.lSize			= SOC1_APB_PER__SIZE_SEMAPHORE;
		pHwDescUmOut->uSemaphore.lAdrAhb		= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_SEMAPHORE;

		pHwDescUmOut->uTimer.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_TIMER;
		pHwDescUmOut->uTimer.lOffset			= SOC1_APB_PER__OFFSET_TIMER;
		pHwDescUmOut->uTimer.lSize				= SOC1_APB_PER__SIZE_TIMER;
		pHwDescUmOut->uTimer.lAdrAhb			= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_TIMER;

		pHwDescUmOut->uGpio.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lOffset				= SOC1_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lSize				= SOC1_APB_PER__SIZE_GPIO;
		pHwDescUmOut->uGpio.lAdrAhb				= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_GPIO;

		pHwDescUmOut->uScrb.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lOffset				= SOC1_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lSize				= SOC1_APB_PER__SIZE_SCRB;
		pHwDescUmOut->uScrb.lAdrAhb				= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_SCRB;

		pHwDescUmOut->uBootRom.uBase.pPtr		= pBaseApbPer + SOC1_APB_PER__OFFSET_BOOT_ROM;
		pHwDescUmOut->uBootRom.lOffset			= SOC1_APB_PER__OFFSET_BOOT_ROM;
		pHwDescUmOut->uBootRom.lSize			= SOC1_APB_PER__SIZE_BOOT_ROM;
		pHwDescUmOut->uBootRom.lAdrAhb			= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_BOOT_ROM;

		pHwDescUmOut->uSps.uBase.pPtr			= pBaseApbPer + SOC1_APB_PER__OFFSET_SPS;
		pHwDescUmOut->uSps.lOffset				= SOC1_APB_PER__OFFSET_SPS;
		pHwDescUmOut->uSps.lSize				= SOC1_APB_PER__SIZE_SPS;
		pHwDescUmOut->uSps.lAdrAhb				= pHwDescKmIn->lAdrAhb + SOC1_APB_PER__OFFSET_SPS;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec200P)
//************************************************************************

void fnBoard_SetPtrHwResUm_ApbPer200P(	uPNDEV_APP*						pAppIn,
										uPNDEV_HW_DESC_KM*				pHwDescKmIn,
										uPNDEV_HW_DESC_APB_PER_200P*	pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 			lSizeHwRes	= 0;
	UINT32 			lSizeBar	= 0;
	volatile UINT8*	pBaseApbPer	= NULL;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		// get BasePtr to ApbPer
		pBaseApbPer = (((volatile UINT8*) pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr) + pHwDescKmIn->lOffsetBar);

		pHwDescUmOut->uPerIf.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_PERIF;
		pHwDescUmOut->uPerIf.lOffset		= ERTEC200P_APB_PER__OFFSET_PERIF;
		pHwDescUmOut->uPerIf.lSize			= ERTEC200P_APB_PER__SIZE_PERIF;
		pHwDescUmOut->uPerIf.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_PERIF;

		pHwDescUmOut->uUart1.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_UART1;
		pHwDescUmOut->uUart1.lOffset		= ERTEC200P_APB_PER__OFFSET_UART1;
		pHwDescUmOut->uUart1.lSize			= ERTEC200P_APB_PER__SIZE_UART1;
		pHwDescUmOut->uUart1.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_UART1;

		pHwDescUmOut->uUart2.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_UART2;
		pHwDescUmOut->uUart2.lOffset		= ERTEC200P_APB_PER__OFFSET_UART2;
		pHwDescUmOut->uUart2.lSize			= ERTEC200P_APB_PER__SIZE_UART2;
		pHwDescUmOut->uUart2.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_UART2;

		pHwDescUmOut->uUart3.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_UART3;
		pHwDescUmOut->uUart3.lOffset		= ERTEC200P_APB_PER__OFFSET_UART3;
		pHwDescUmOut->uUart3.lSize			= ERTEC200P_APB_PER__SIZE_UART3;
		pHwDescUmOut->uUart3.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_UART3;

		pHwDescUmOut->uUart4.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_UART4;
		pHwDescUmOut->uUart4.lOffset		= ERTEC200P_APB_PER__OFFSET_UART4;
		pHwDescUmOut->uUart4.lSize			= ERTEC200P_APB_PER__SIZE_UART4;
		pHwDescUmOut->uUart4.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_UART4;

		pHwDescUmOut->uI2c.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_I2C;
		pHwDescUmOut->uI2c.lOffset			= ERTEC200P_APB_PER__OFFSET_I2C;
		pHwDescUmOut->uI2c.lSize			= ERTEC200P_APB_PER__SIZE_I2C;
		pHwDescUmOut->uI2c.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_I2C;

		pHwDescUmOut->uScrb.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lOffset			= ERTEC200P_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lSize			= ERTEC200P_APB_PER__SIZE_SCRB;
		pHwDescUmOut->uScrb.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_SCRB;

		pHwDescUmOut->uSpi1.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_SPI1;
		pHwDescUmOut->uSpi1.lOffset			= ERTEC200P_APB_PER__OFFSET_SPI1;
		pHwDescUmOut->uSpi1.lSize			= ERTEC200P_APB_PER__SIZE_SPI1;
		pHwDescUmOut->uSpi1.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_SPI1;

		pHwDescUmOut->uSpi2.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_SPI2;
		pHwDescUmOut->uSpi2.lOffset			= ERTEC200P_APB_PER__OFFSET_SPI2;
		pHwDescUmOut->uSpi2.lSize			= ERTEC200P_APB_PER__SIZE_SPI2;
		pHwDescUmOut->uSpi2.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_SPI2;

		pHwDescUmOut->uTimer0_5.uBase.pPtr	= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_TIMER0_5;
		pHwDescUmOut->uTimer0_5.lOffset		= ERTEC200P_APB_PER__OFFSET_TIMER0_5;
		pHwDescUmOut->uTimer0_5.lSize		= ERTEC200P_APB_PER__SIZE_TIMER0_5;
		pHwDescUmOut->uTimer0_5.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_TIMER0_5;

		pHwDescUmOut->uWatchdog.uBase.pPtr	= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lOffset		= ERTEC200P_APB_PER__OFFSET_WATCHDOG;
		pHwDescUmOut->uWatchdog.lSize		= ERTEC200P_APB_PER__SIZE_WATCHDOG;
		pHwDescUmOut->uWatchdog.lAdrAhb		= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_WATCHDOG;

		pHwDescUmOut->uGpio.uBase.pPtr		= pBaseApbPer + ERTEC200P_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lOffset			= ERTEC200P_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lSize			= ERTEC200P_APB_PER__SIZE_GPIO;
		pHwDescUmOut->uGpio.lAdrAhb			= pHwDescKmIn->lAdrAhb + ERTEC200P_APB_PER__OFFSET_GPIO;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Hera)
//************************************************************************

void fnBoard_SetPtrHwResUm_ApbPerHera(	uPNDEV_APP*						pAppIn,
										uPNDEV_HW_DESC_KM*				pHwDescKmIn,
										uPNDEV_HW_DESC_APB_PER_HERA*	pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 			lSizeHwRes	= 0;
	UINT32 			lSizeBar	= 0;
	volatile UINT8*	pBaseApbPer	= NULL;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		// get BasePtr to ApbPer
		pBaseApbPer = (((volatile UINT8*) pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr) + pHwDescKmIn->lOffsetBar);

		pHwDescUmOut->uScrb.uBase.pPtr		= pBaseApbPer + HERA_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lOffset			= HERA_APB_PER__OFFSET_SCRB;
		pHwDescUmOut->uScrb.lSize			= HERA_APB_PER__SIZE_SCRB;
		pHwDescUmOut->uScrb.lAdrAhb			= pHwDescKmIn->lAdrAhb + HERA_APB_PER__OFFSET_SCRB;

		pHwDescUmOut->uSpi1.uBase.pPtr		= pBaseApbPer + HERA_APB_PER__OFFSET_SPI1;
		pHwDescUmOut->uSpi1.lOffset			= HERA_APB_PER__OFFSET_SPI1;
		pHwDescUmOut->uSpi1.lSize			= HERA_APB_PER__SIZE_SPI1;
		pHwDescUmOut->uSpi1.lAdrAhb			= pHwDescKmIn->lAdrAhb + HERA_APB_PER__OFFSET_SPI1;

		pHwDescUmOut->uGpio.uBase.pPtr		= pBaseApbPer + HERA_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lOffset			= HERA_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lSize			= HERA_APB_PER__SIZE_GPIO;
		pHwDescUmOut->uGpio.lAdrAhb			= pHwDescKmIn->lAdrAhb + HERA_APB_PER__OFFSET_GPIO;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Omap)
//************************************************************************

void fnBoard_SetPtrHwResUm_ApbPerOmap(	uPNDEV_APP*						pAppIn,
										uPNDEV_HW_DESC_KM*				pHwDescKmIn,
										uPNDEV_HW_DESC_APB_PER_OMAP*	pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 			lSizeHwRes	= 0;
	UINT32 			lSizeBar	= 0;
	volatile UINT8*	pBaseApbPer	= NULL;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		// get BasePtr to ApbPer
		pBaseApbPer = (((volatile UINT8*) pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr) + pHwDescKmIn->lOffsetBar);

		pHwDescUmOut->uGpio.uBase.pPtr		= pBaseApbPer + OMAP_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lOffset			= OMAP_APB_PER__OFFSET_GPIO;
		pHwDescUmOut->uGpio.lSize			= OMAP_APB_PER__SIZE_GPIO;
		pHwDescUmOut->uGpio.lAdrAhb			= pHwDescKmIn->lAdrAhb + OMAP_APB_PER__OFFSET_GPIO;
	
		pHwDescUmOut->uI2C1.uBase.pPtr		= pBaseApbPer + OMAP_APB_PER__OFFSET_I2C1;
		pHwDescUmOut->uI2C1.lOffset			= OMAP_APB_PER__OFFSET_I2C1;
		pHwDescUmOut->uI2C1.lSize			= OMAP_APB_PER__SIZE_I2C1;
		pHwDescUmOut->uI2C1.lAdrAhb			= pHwDescKmIn->lAdrAhb + OMAP_APB_PER__OFFSET_I2C1;

		pHwDescUmOut->uI2C2.uBase.pPtr		= pBaseApbPer + OMAP_APB_PER__OFFSET_I2C2;
		pHwDescUmOut->uI2C2.lOffset			= OMAP_APB_PER__OFFSET_I2C2;
		pHwDescUmOut->uI2C2.lSize			= OMAP_APB_PER__SIZE_I2C2;
		pHwDescUmOut->uI2C2.lAdrAhb			= pHwDescKmIn->lAdrAhb + OMAP_APB_PER__OFFSET_I2C2;

		pHwDescUmOut->uI2C3.uBase.pPtr		= pBaseApbPer + OMAP_APB_PER__OFFSET_I2C3;
		pHwDescUmOut->uI2C3.lOffset			= OMAP_APB_PER__OFFSET_I2C3;
		pHwDescUmOut->uI2C3.lSize			= OMAP_APB_PER__SIZE_I2C3;
		pHwDescUmOut->uI2C3.lAdrAhb			= pHwDescKmIn->lAdrAhb + OMAP_APB_PER__OFFSET_I2C3;

		pHwDescUmOut->uUart.uBase.pPtr		= pBaseApbPer + OMAP_APB_PER__OFFSET_UART;
		pHwDescUmOut->uUart.lOffset			= OMAP_APB_PER__OFFSET_UART;
		pHwDescUmOut->uUart.lSize			= OMAP_APB_PER__SIZE_UART;
		pHwDescUmOut->uUart.lAdrAhb			= pHwDescKmIn->lAdrAhb + OMAP_APB_PER__OFFSET_UART;
	}
}

