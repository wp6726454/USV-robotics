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
/*  F i l e               &F: Board_Pnip.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with PNIP (Ertec200P)
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
	#include "Board_Pnip.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoardPnip_ChangePageXhif)
//	#pragma alloc_text (PAGE, fnBoardPnip_InitIrqLogic)
//	#pragma alloc_text (PAGE, fnBoardPnip_CheckStartupPbl)
//	#pragma alloc_text (PAGE, fnBoardPnip_SetStaticPagesXhif)
//	#pragma alloc_text (PAGE, fnBoardPnip_InitDriverPageXhif)
//	#pragma alloc_text (PAGE, fnBoardPnip_StartDriverPageXhif)
//	#pragma alloc_text (PAGE, fnBoardPnip_StopDriverPageXhif)
//	#pragma alloc_text (PAGE, fnBoardPnip_AllocUserPageXhif)
//	#pragma alloc_text (PAGE, fnBoardPnip_ReleaseUserPageXhif)
//	#pragma alloc_text (PAGE, fnBoardPnip_VerifyPnip)
//	#pragma alloc_text (PAGE, fnBoardPnip_InitEmcController)
//	#pragma alloc_text (PAGE, fnBoardPnip_ResetRam)
//	#pragma alloc_text (PAGE, fnBoardPnip_GetValueConfigPins)
//	#pragma alloc_text (PAGE, fnBoardPnip_TestGpio)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	XHIF: change dynamic UserPage
//************************************************************************

BOOLEAN fnBoardPnip_ChangePageXhif(	DEVICE_EXTENSION*		pDevExtIn,
									uPNDEV_APP*				pAppIn,
									const ePNDEV_XHIF_PAGE	ePageIn,
									const UINT32			lSegmentAhbIn,
									const UINT32			lDimErrorIn,
									uPNDEV_HW_DESC*			pHwDescUmOut,
									UINT32*					pCtrRefOut,
									_TCHAR*					sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(pDevExtIn->eBoardDetail != ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
		// not board with Ertec200P/PCIe-XHIF
	{
		// set ErrorString
		//	- changed by DLL to ReturnValue=ePNDEV_ERROR_SUPPORT
		fnBuildString(	PNDEV_ERROR_SUPPORT,
						NULL,
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		if	(ePageIn == ePNDEV_XHIF_PAGE_INVALID)
			// release page
		{
			// success
			bResult = TRUE;

			// release page
			fnBoardPnip_ReleaseUserPageXhif(pDevExtIn,
											pAppIn);
		}
		else	// allocate page
		{
			if	(pDevExtIn->uUserPageXhif.ePage	== ePNDEV_XHIF_PAGE_INVALID)
				// page unused
			{
				if	(pDevExtIn->uUserPageXhif.lCtrRef != 0)
					// error
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid internal page state!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;

					// allocate page
					fnBoardPnip_AllocUserPageXhif(	pDevExtIn,
													pAppIn,
													ePageIn,
													lSegmentAhbIn,
													pHwDescUmOut);
				}
			}
			else if	(pDevExtIn->uUserPageXhif.ePage	== ePageIn)
					// page in use - already set to desired HwRes
			{
				// success
				bResult = TRUE;

				if	(pAppIn->bAllocPageXhif)
					// page already allocated by this App
				{
					// release page
					fnBoardPnip_ReleaseUserPageXhif(pDevExtIn,
													pAppIn);
				}

				// allocate page
				fnBoardPnip_AllocUserPageXhif(	pDevExtIn,
												pAppIn,
												ePageIn,
												lSegmentAhbIn,
												pHwDescUmOut);
			}
			else	// page in use - set to a different HwRes
			{
				if	(!pAppIn->bAllocPageXhif)
					// page not allocated by this App
				{
					// set ErrorString
					//	- changed by DLL to ReturnValue=ePNDEV_ERROR_IN_USE
					fnBuildString(	PNDEV_ERROR_IN_USE,
									NULL,
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else if	(pDevExtIn->uUserPageXhif.lCtrRef > 1)
					// page used by this and other App
				{
					// set ErrorString
					//	- changed by DLL to ReturnValue=ePNDEV_ERROR_IN_USE
					fnBuildString(	PNDEV_ERROR_IN_USE,
									NULL,
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else	// page only used by this App
				{
					// release page
					fnBoardPnip_ReleaseUserPageXhif(pDevExtIn,
													pAppIn);

					if	(	(pDevExtIn->uUserPageXhif.lCtrRef	!= 0)
						||	(pDevExtIn->uUserPageXhif.ePage		!= ePNDEV_XHIF_PAGE_INVALID))
						// error
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid state of internal variables!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;

						// alloc page
						fnBoardPnip_AllocUserPageXhif(	pDevExtIn,
														pAppIn,
														ePageIn,
														lSegmentAhbIn,
														pHwDescUmOut);
					}
				}
			}
		}

		if	(pCtrRefOut != NULL)
			// valid ptr
		{
			// return
			*pCtrRefOut = pDevExtIn->uUserPageXhif.lCtrRef;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	initialize IRQ logic
//************************************************************************

void fnBoardPnip_InitIrqLogic(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 lPnIpReg_IcuControl = 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
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

			// set IrqLength of PN-MUXx, activate IRQ-measurement
			//	- high-active
			//	- default: IrqLength=8nsec
			//	  ARM-ICU/PERIF-ICU (31,25MHz: 32nsec) require 2 clocks because of different clocks (PNIP=125MHz -> (7+1)*8nsec=64nsec)
			//	- time at occurring of ICUx-IRQ0 will be stored
			//	- other values=default
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + lPnIpReg_IcuControl),
								((1<<20)+(0<<16)+(7<<1)+(1<<0)));

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			// TopLevel-ICU = PERIF-ICU

			// PNIP-ICU3 must be used!
			lPnIpReg_IcuControl = PNIP_REG__ICU3_CONTROL;

			// set IrqLength of PN-MUXx, activate IRQ-measurement
			//	- high-active
			//	- default: IrqLength=8nsec
			//	  PERIF-ICU (125MHz: 8nsec) requires 1 clock because of same clocks (PNIP=125MHz -> (0+1)*8nsec=8nsec)
			//	- time at occurring of ICUx-IRQ0 will be stored
			//	- other values=default
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + lPnIpReg_IcuControl),
								((1<<20)+(0<<16)+(0<<1)+(1<<0)));

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
		// save value
		pDevExtIn->lValuePnIpIcuReg_ControlLow = (READ_REG_UINT32(pDevExtIn->pPnIp + lPnIpReg_IcuControl) & 0x0000ffff);
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check startup of Arm926 PBL
//************************************************************************

BOOLEAN fnBoardPnip_CheckStartupPbl(DEVICE_EXTENSION*	pDevExtIn,
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
	UINT32	lOffsetPage		= 0;
	UINT32	lRangePage		= 0;
	UINT32	i = 0;

		//	- PBL initializes XHIF interface -> don't initialize PageReg before!
		//	- last action of PBL: set XHIF page0 to Arm926-DTCM, don't access memory before!

		// preset
		bErrorTimeout = TRUE;

		for	(i = 0; i < 100; i++)
			// MaxTime=100msec
		{
			lOffsetPage	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_CSR__PAGE0_OFFSET);
			lRangePage	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_CSR__PAGE0_RANGE);

			if	(	(lOffsetPage	!= ERTEC200P_AHB_ADR__ARM926_TCM)
				||	(lRangePage		!= BOARD_ERTEC200P_XHIF__PAGE_SIZE_1MB))
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
				// debug purpose
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
									0xaffeaffe);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Timeout, XHIF page0 not set to Arm926-DTCM by PBL!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
			volatile UINT8*	pTcmArm9 = NULL;

				// preset
				bErrorTimeout = TRUE;

				// after reset: page0 = Arm926-DTCM
				pTcmArm9 = (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE0);

				for	(i = 0; i < 100; i++)
					// MaxTime=100msec
				{
					if	(READ_REG_UINT32(pTcmArm9 + ERTEC200P_DTCM_OFFSET__SEMAPHORE) != 0x00000000)
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
						// test hardware by write access
						WRITE_REG_UINT32(	(pTcmArm9 + ERTEC200P_DTCM_OFFSET__CODE),
											0x11223344);

						if	(READ_REG_UINT32(pTcmArm9 + ERTEC200P_DTCM_OFFSET__CODE) == 0x11223344)
							// success
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
						// debug purpose
						WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
											0xaffeaffe);

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Timeout, semaphore not set by PBL or Arm926-DTCM not writable!"),
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

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	XHIF: set static pages
//************************************************************************

BOOLEAN fnBoardPnip_SetStaticPagesXhif(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
		// board with Ertec200P/PCIe-XHIF
	{
	UINT32	lValueXhifModeAsic	= 0;
	BOOLEAN	bErrorDelay			= FALSE;

		// set page0 (dynamic DriverPage) to ...
		{
			// use page0 for driver because page0 is the only page which is configured by PBL (Arm926-DTCM)
			//	- if this page would be used for user accessible resources then user could be write to Arm926-DTCM during reset!

			// initialize dynamic DriverPage
			fnBoardPnip_InitDriverPageXhif(pDevExtIn);
		}

		// set page1 to ApbPer
		{
			// 2MB aligned
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_CSR__PAGE1_OFFSET),
								ERTEC200P_AHB_ADR__APB_PER);
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_CSR__PAGE1_RANGE),
								BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB);

			// ptr=BAR2.Page1
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE1);
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lIndexBar			= 2;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lOffsetBar			= BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE1;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lSize				= ERTEC200P_AHB_SIZE__APB_PER;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lAdrAhb			= ERTEC200P_AHB_ADR__APB_PER;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.lOffsetPhyStart	= 0;
		}

		// activate 8*2MB
		{
			// signal XHIF_XCS_R_A20:
			//	- internal multiplexer must be set to A20 before external multiplexer is set to A20
			//	  (otherwise accesses to upper 1MB of a page would address CS-R of Asic -> overwriting of PageCfgReg!
			//	   new concept: no user accesses during reset phase -> no problem any longer)
			//	- internal multiplexer:	set by SCRB.XHIF_MODE or AsicReset
			//	- external multiplexer:	set by BAR0.BoardControl

			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__XHIF_MODE),
								1);

			fnBoardIrtePnip_ChangeXhifConfig(	pDevExtIn,
												TRUE);		// A20

			// now PageCfgReg only accessible by XHIF_XCS_M
			//	- hardwired by accesses to BAR2 (XHIF_XCS_R_A20 = A20)
			//	- only by ApbPer
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg = (pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_HOSTIF);

			// AP01168675:
			//	- for switching from CS-R to CS-M hardware needs about 200ns
			//	- after a posted write (~70ns) a read access (~1,3usec) may follow immediately
			//		-> wait time
			//	- otherwise a read access may be immediately after the posted write (XHIF write accesses are always posted writes)

			// wait 10msec

			if	(!fnDelayThread(pDevExtIn,
								10,
								lDimErrorIn,
								sErrorOut))
				// error at starting WaitTime of current thread
			{
				bErrorDelay	= TRUE;
			}
		}

		if	(!bErrorDelay)
			// success
		{
			// verify settings

			lValueXhifModeAsic = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__XHIF_MODE);

			if	(lValueXhifModeAsic != 1)
				// error
			{
				// debug purpose
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
									0xaffeaffe);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): XHIF settings at Asic not correct (SCRB.XHIF_MODE<>1)!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else if	(!fnBoardIrtePnip_CheckXhifA20(pDevExtIn))
					// signal XHIF_CS_R_A20 not set to A20
			{
				// debug purpose
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
									0xaffeaffe);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): XHIF settings at BAR0.BoardControl not correct (XHIF_CS_R_A20 not set to A20)!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;

				// set page2 to PERIF by AHB
				{
					// 2MB aligned
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE2_OFFSET),
										ERTEC200P_AHB_ADR__PERIF);
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE2_RANGE),
										BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB);

					// ptr=Bar2.Page2
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.pPtr			= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE2);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lIndexBar		= 2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lOffsetBar		= BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lSize			= ERTEC200P_AHB_SIZE__PERIF;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lAdrAhb			= ERTEC200P_AHB_ADR__PERIF;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.lOffsetPhyStart	= 0;
					pDevExtIn->pPerIfIcu											= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPerIf.pPtr;
				}

				// set page3 to PNIP
				{
					// 2MB aligned
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE3_OFFSET),
										ERTEC200P_AHB_ADR__PNIP);
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE3_RANGE),
										BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB);

					// ptr=BAR2.Page3
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE3);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lIndexBar		= 2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lOffsetBar		= BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE3;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lSize			= ERTEC200P_AHB_SIZE__PNIP;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lAdrAhb			= ERTEC200P_AHB_ADR__PNIP;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.lOffsetPhyStart	= 0;
					pDevExtIn->pPnIp												= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uPnIp.pPtr;
				}

				// set page4 to KRISC32-TCM
				//	- must be after setting page of ApbPer (see AsicVersion)!
				{
				UINT32 lScrbId = 0;

					// 2MB aligned
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE4_OFFSET),
										ERTEC200P_AHB_ADR__KRISC32_TCM);
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE4_RANGE),
										BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB);

					// ptr=BAR2.Page4
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE4);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lIndexBar			= 2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lOffsetBar			= BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE4;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lAdrAhb			= ERTEC200P_AHB_ADR__KRISC32_TCM;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uKrisc32Tcm.lOffsetPhyStart	= 0;

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

				// set page5 to GDMA/TTU
				{
					// 2MB aligned
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE5_OFFSET),
										ERTEC200P_AHB_ADR__GDMA);
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE5_RANGE),
										BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB);

					// ptr=BAR2.Page5
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE5);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lIndexBar		= 2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lOffsetBar		= BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE5;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lSize			= ERTEC200P_AHB_SIZE__GDMA;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lAdrAhb			= ERTEC200P_AHB_ADR__GDMA;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uGdma.lOffsetPhyStart	= 0;

					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE5 + BOARD_ERTEC200P_XHIF__PAGE_OFFSET_TTU);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lIndexBar			= 2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lOffsetBar		= BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE5 + BOARD_ERTEC200P_XHIF__PAGE_OFFSET_TTU;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lSize				= ERTEC200P_AHB_SIZE__TTU;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lAdrAhb			= ERTEC200P_AHB_ADR__TTU;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uTtu.lOffsetPhyStart	= 0;
				}

				// set page6 to SharedAsicSdram_Direct
				{
				UINT32 lAhbAdrSdram = 0;

					lAhbAdrSdram = ERTEC200P_AHB_ADR__EMC_SDRAM + (BOARD_ERTEC200P_XHIF__SDRAM_PAGE_SHARED_DIRECT * (1024*1024*2));

					// 2MB aligned
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE6_OFFSET),
										lAhbAdrSdram);
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE6_RANGE),
										BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB);

					// ptr=BAR2.Page6
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.pPtr				= (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE6);
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lIndexBar		= 2;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lOffsetBar		= BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE6;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lSize			= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lAdrAhb			= lAhbAdrSdram;
					pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.lOffsetPhyStart	= (BOARD_ERTEC200P_XHIF__SDRAM_PAGE_SHARED_DIRECT * (1024*1024*2));
				}

				// set page7 (dynamic UserPage) to ...
				{
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	XHIF: initialize dynamic DriverPage
//************************************************************************

void fnBoardPnip_InitDriverPageXhif(DEVICE_EXTENSION* pDevExtIn)
{
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArm926Tcm.pPtr		= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.pPtr			= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.pPtr			= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.pPtr	= NULL;
	pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram_DynPage.pPtr	= NULL;

	WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE0_OFFSET),
						0);
	WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE0_RANGE),
						0);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	XHIF: start using dynamic DriverPage
//************************************************************************

void fnBoardPnip_StartDriverPageXhif(	DEVICE_EXTENSION*						pDevExtIn,
										const eBOARD_ERTEC200P_XHIF__DYN_PAGE	ePageIn,
										const UINT32							lSegmentAhbIn,
										const char*								sTraceIn)
{
	if	(pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
		// board with Ertec200P/PCIe-XHIF
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
		BOOLEAN	bResultPage	= FALSE;
		UINT32	lAdrAhb		= 0;
		UINT32	lSizeAhb	= 0;
		void**	pPtrTmp		= NULL;

			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArm926Tcm.pPtr		= NULL;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.pPtr			= NULL;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.pPtr			= NULL;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr	= NULL;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.pPtr	= NULL;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.pPtr	= NULL;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.pPtr	= NULL;
			pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram_DynPage.pPtr	= NULL;

			switch	(ePageIn)
					// dynamic DriverPage
			{
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_ARM926_TCM:
				{
					// set page to ARM926-DTCM
					//	- 2MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__ARM926_TCM;
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uArm926Tcm.pPtr;

					break;
				}
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_SD_MMC:
				{
					// set page to SD-MMC
					//	- 2MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__SD_MMC + (lSegmentAhbIn * (1024*1024*2));
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uSdMmc.pPtr;

					break;
				}
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_REG:
				{
					// set page to EMC Reg
					//	- only 1MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__EMC_REG;
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_1MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.pPtr;

					break;
				}
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_SDRAM:
				{
					// set page to EMC SDRAM
					//	- 2MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__EMC_SDRAM + (lSegmentAhbIn * (1024*1024*2));
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram_DynPage.pPtr;

					break;
				}
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK0:
				{
					// set page to EMC AsyncBank0
					//	- 2MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK0 + (lSegmentAhbIn * (1024*1024*2));
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr;

					break;
				}
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK1:
				{
					// set page to EMC AsyncBank1
					//	- 2MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK1 + (lSegmentAhbIn * (1024*1024*2));
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.pPtr;

					break;
				}
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK2:
				{
					// set page to EMC AsyncBank2
					//	- 2MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK2 + (lSegmentAhbIn * (1024*1024*2));
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank2.pPtr;

					break;
				}
				case eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK3:
				{
					// set page to EMC AsyncBank3
					//	- 2MB aligned

					// success
					bResultPage = TRUE;

					lAdrAhb		= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK3 + (lSegmentAhbIn * (1024*1024*2));
					lSizeAhb	= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
					pPtrTmp		= (void**) &pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank3.pPtr;

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

			if	(bResultPage)
				// success
			{
			UINT32 lAdrAhbTmp = 0;

				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE0_OFFSET),
									lAdrAhb);
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE0_RANGE),
									lSizeAhb);

				// ptr=BAR2.Page0
				//	- dynamic DriverPage not accessible at UserMode -> set only pPtr
				*pPtrTmp = (void*) (pDevExtIn->uHwResKm.uBar[2].pVirtualAdr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE0);

				// ensure that new page is active before any access is done to it
				//	- PCIe:
				//		several write accesses (WR1..WRn) to IP1 may be stored before a read access to IP2 is started (PostedWrite)
				//		->	if last write access is a access to PageReg (IP1) then it is not ensured that new page (IP2, e.g. SDRAM) is active when IP2 is read
				//			(read access is done to a different IP -> IP2-read can be executed before IP1-write)
				//		->	PageReg (IP1) must be read back (-> IP1-write finished) before there is a read access to IP2
				lAdrAhbTmp = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE0_OFFSET);

				if	(lAdrAhbTmp != lAdrAhb)
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
		// leave CriticalSection: see fnBoardPnip_StopDriverPageXhif()
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	XHIF: stop using dynamic DriverPage
//************************************************************************

void fnBoardPnip_StopDriverPageXhif(DEVICE_EXTENSION* pDevExtIn)
{
	if	(pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
		// board with Ertec200P/PCIe-XHIF
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		//------------------------------------------------------------------------
		// enter CriticalSection: see fnBoardPnip_StartDriverPageXhif()
		{
			// initialize dynamic DriverPage
			fnBoardPnip_InitDriverPageXhif(pDevExtIn);
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
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	XHIF: allocate dynamic UserPage
//************************************************************************

void fnBoardPnip_AllocUserPageXhif(	DEVICE_EXTENSION*		pDevExtIn,
									uPNDEV_APP*				pAppIn,
									const ePNDEV_XHIF_PAGE	ePageIn,
									const UINT32			lSegmentAhbIn,
									uPNDEV_HW_DESC*			pHwDescUmOut)
{
	if	(pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
		// board with Ertec200P/PCIe-XHIF
	{
	BOOLEAN bResultPage		= FALSE;
	UINT32	lAdrAhb			= 0;
	UINT32	lSizeAhb		= 0;
	UINT32	lSizeUser		= 0;
	UINT32	lOffsetPhyStart	= 0;

		switch	(ePageIn)
				// page
		{
			case ePNDEV_XHIF_PAGE_ERTEC200P__SD_MMC:
			{
				// set page to SD-MMC
				//	- 2MB aligned

				// success
				bResultPage = TRUE;

				lAdrAhb			= ERTEC200P_AHB_ADR__SD_MMC + (lSegmentAhbIn * (1024*1024*2));
				lSizeAhb		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lSizeUser		= ERTEC200P_AHB_SIZE__SD_MMC;
				lOffsetPhyStart	= 0;

				break;
			}
			case ePNDEV_XHIF_PAGE_ERTEC200P__ARM926_TCM:
			{
				// set page to ARM926-TCM
				//	- 2MB aligned

				// success
				bResultPage = TRUE;

				lAdrAhb			= ERTEC200P_AHB_ADR__ARM926_TCM + (lSegmentAhbIn * (1024*1024*2));
				lSizeAhb		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lSizeUser		= ERTEC200P_AHB_SIZE__ARM926_TCM;
				lOffsetPhyStart	= 0;

				break;
			}
			case ePNDEV_XHIF_PAGE_ERTEC200P__EMC_SDRAM:
			{
				// set page to EMC SDRAM
				//	- 2MB aligned

				// success
				bResultPage = TRUE;

				lAdrAhb			= ERTEC200P_AHB_ADR__EMC_SDRAM + (lSegmentAhbIn * (1024*1024*2));
				lSizeAhb		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lSizeUser		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lOffsetPhyStart	= lSegmentAhbIn * (1024*1024*2);

				break;
			}
			case ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK0:
			{
				// set page to EMC AsyncBank0
				//	- 2MB aligned

				// success
				bResultPage = TRUE;

				lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK0 + (lSegmentAhbIn * (1024*1024*2));
				lSizeAhb		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lSizeUser		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lOffsetPhyStart	= 0;

				break;
			}
			case ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK1:
			{
				// set page to EMC AsyncBank1
				//	- 2MB aligned

				// success
				bResultPage = TRUE;

				lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK1 + (lSegmentAhbIn * (1024*1024*2));
				lSizeAhb		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lSizeUser		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lOffsetPhyStart	= 0;

				break;
			}
			case ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK2:
			{
				// set page to EMC AsyncBank2
				//	- 2MB aligned

				// success
				bResultPage = TRUE;

				lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK2 + (lSegmentAhbIn * (1024*1024*2));
				lSizeAhb		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lSizeUser		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lOffsetPhyStart	= 0;

				break;
			}
			case ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK3:
			{
				// set page to EMC AsyncBank3
				//	- 2MB aligned

				// success
				bResultPage = TRUE;

				lAdrAhb			= ERTEC200P_AHB_ADR__EMC_ASYNC_BANK3 + (lSegmentAhbIn * (1024*1024*2));
				lSizeAhb		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lSizeUser		= BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB;
				lOffsetPhyStart	= 0;

				break;
			}
			default:
			{
				break;
			}
		}

		if	(bResultPage)
			// success
		{
			// configure page independent if first use or reuse
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE7_OFFSET),
								lAdrAhb);
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE7_RANGE),
								lSizeAhb);

			if	(pAppIn != NULL)
				// call by service ChangeXhifPage
			{
				// current App uses page
				pAppIn->bAllocPageXhif = TRUE;

				// update RefCtr for page
				pDevExtIn->uUserPageXhif.lCtrRef++;

				// update HwRes referenced by page
				pDevExtIn->uUserPageXhif.ePage			= ePageIn;
				pDevExtIn->uUserPageXhif.lSegmentAhb	= lSegmentAhbIn;

				// ptr=BAR2.Page7
				//	- UserMode!
				pHwDescUmOut->uBase.pPtr	= (pAppIn->uHwResUm.uBar[2].uAdrUser.pPtr + BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE7);
				pHwDescUmOut->lOffset		= lOffsetPhyStart;
				pHwDescUmOut->lSize			= lSizeUser;
				pHwDescUmOut->lAdrAhb		= lAdrAhb;
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	XHIF: release dynamic UserPage
//************************************************************************

void fnBoardPnip_ReleaseUserPageXhif(	DEVICE_EXTENSION*	pDevExtIn,
										uPNDEV_APP*			pAppIn)
{
	if	(pDevExtIn->eBoardDetail == ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF)
		// board with Ertec200P/PCIe-XHIF
	{
		// page can only be released by an App which allocated it!

		if	(pAppIn->bAllocPageXhif)
			// page allocated by this App
		{
			pAppIn->bAllocPageXhif = FALSE;

			// update RefCtr for page
			pDevExtIn->uUserPageXhif.lCtrRef--;

			if	(pDevExtIn->uUserPageXhif.lCtrRef == 0)
				// page released by all App
			{
				// update HwRes referenced by page
				pDevExtIn->uUserPageXhif.ePage = ePNDEV_XHIF_PAGE_INVALID;

				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE7_OFFSET),
									0);
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.pXhifPageCfgReg + ERTEC200P_XHIF_REG_MEM__PAGE7_RANGE),
									0);
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify PNIP
//************************************************************************

BOOLEAN fnBoardPnip_VerifyPnip(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(pDevExtIn->pPnIp != NULL)
		// Asic with PNIP
	{
	UINT32	lTmp	= 0;
	UINT32	lIdPnIp	= 0;

		// verify access to PNIP
		lTmp = READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_DEVELOPMENT);

		// get PNIP-ID
		lIdPnIp = (lTmp >> 21);

		if	(lIdPnIp != PNIP_IP_DEVELOPMENT_BIT31_21__ID)
			// error
		{
		_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// debug purpose
			//WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
			//					0xaffeaffe);

			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Error at accessing PNIP (real: 0x%x, expected: 0x%x)!"),
								lIdPnIp,
								PNIP_IP_DEVELOPMENT_BIT31_21__ID);

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
//	initialize EMC controller
//************************************************************************

BOOLEAN fnBoardPnip_InitEmcController(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	//------------------------------------------------------------------------
	// [XHIF: using dynamic DriverPage]
	//	- EMC Reg
	fnBoardPnip_StartDriverPageXhif(pDevExtIn,
									eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_REG,
									0,
									__FUNCTION__);
	{
	volatile UINT8*	pEmcReg				= NULL;
	BOOLEAN			bEnableClockSdram2	= FALSE;

		// Note:
		//	- all boards have two SDRAM chips
		//	- FPGA1-xx boards:	1 clock for both chips
		//	- EB200P board:		SDRAM chip2 needs own clock, default: clock=off (-> 32Bit access is done by two 16Bit accesses, slow)
		//						-> clock for SDRAM chip2 must be enabled before doing settings of EMC controller

		switch	(pDevExtIn->eBoard)
				// BoardType
		{
			case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
			{
				pEmcReg = pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.pPtr;

				break;
			}
			case ePNDEV_BOARD_EB200P:	// EB200P board
			{
				pEmcReg = pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcReg.pPtr;

				// see above
				bEnableClockSdram2 = TRUE;

				break;
			}
			default:
			{
				break;
			}
		}

		if	(pEmcReg != NULL)
			// success
		{
		UINT32 lTmp		= 0;
		UINT32 lIdEmc	= 0;

			if	(bEnableClockSdram2)
				// clock of second SDRAM chip should be enabled
			{
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__DRIVE_EMC),
									0x00ffffff);
			}

			// verify access to EMC
			lTmp = READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__REV);

			// get EMC-ID
			lIdEmc = (lTmp >> 21);

			if	(lIdEmc != ERTEC200P_EMC_REV_BIT31_21__ID)
				// error at accessing EmcReg
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// debug purpose
				WRITE_REG_UINT32(	(pDevExtIn->pAdminReg + BOARD_FPGA1_REG__SCRATCH),
									0xaffeaffe);

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Error at accessing EMC register (real: 0x%x, expected: 0x%x)!"),
									lIdEmc,
									ERTEC200P_EMC_REV_BIT31_21__ID);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
			BOOLEAN	bResultEmcInitDone = FALSE;

				// wait for startup of EMC controller
				//	- very important!
				{
				BOOLEAN	bErrorDelay		= FALSE;
				BOOLEAN	bErrorTimeout	= FALSE;
				UINT32	i = 0;

					// preset
					bErrorTimeout = TRUE;

					// Bit29 must be set by 480usec at the latest

					for	(i = 0; i < 100; i++)
						// MaxTime=100msec
					{
						if	((READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__SDRAM_REFRESH) & ERTEC200P_EMC_SDRAM_REFRESH__INIT_DONE) == 0)
							// SDRAM initialization not done
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
											_TEXT("(): Timeout, error at startup of EMC controller!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
						}
						else
						{
							// success
							bResultEmcInitDone = TRUE;
						}
					}
				}

				if	(bResultEmcInitDone)
					// success
				{
				UINT32	lAsyncWaitCycle			= 0;
				UINT32	lSdramConfig			= 0;
				UINT32	lSdramRefresh			= 0;
				UINT32	lAsyncBank0				= 0;
				UINT32	lAsyncBank1				= 0;
				UINT32	lAsyncBank2				= 0;
				UINT32	lAsyncBank3				= 0;
				UINT32	lExtended				= 0;
				UINT32	lLowPowerExtendedMode	= 0;
				UINT32	lBurstMode				= 0;
				UINT32	lPageMode				= 0;
				UINT32	lRecovery				= 0;
                UINT32  lScrbId                 = 0;

					if	(pDevExtIn->eBoard == ePNDEV_BOARD_EB200P)
						// EB200P board
					{
                    char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

						// set values
						lAsyncWaitCycle			= 0x40000080;
						lSdramConfig			= 0x00002522;
						lSdramRefresh			= 0x000003d0;
						lAsyncBank0 			= 0x3ffe2502;
						lAsyncBank1 			= 0x0421fff2;
						lAsyncBank2 			= 0x3ffffff2;
						lAsyncBank3 			= 0x3ffffff2;
						lLowPowerExtendedMode	= 0x00000000;
						lBurstMode				= 0x00000000;
						lPageMode				= 0x00000042;
						lRecovery				= 0x00000000;

                        // get SCRB-ID
			            lScrbId = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__ID);

                        // set TraceDetail
		                RtlStringCchPrintfA(sTraceDetail,
							                _countof(sTraceDetail),
							                "PnDev[%s]: SCRB-Id=0x%08x",
							                pDevExtIn->sPciLocShortAscii,
                                            lScrbId);

				        // set trace
				        fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							        ePNDEV_TRACE_CONTEXT_BASIC,
							        TRUE,						// logging
							        sTraceDetail);

                        if	(lScrbId == ERTEC200P_SCRB_ID__REV2_HW2)
				            // hw-release Step2
			            {
                            lExtended		    = 0x05B74600;
                        }
                        else
                        {
                            lExtended		    = 0x0DB74600;
                        }
					}
					else
					{
						// set values
						lAsyncWaitCycle			= 0x40000080;
						lSdramConfig			= 0x00000522;
						lSdramRefresh			= 0x000000f4;
						lAsyncBank0 			= 0x3ffe4102;
						lAsyncBank1 			= 0x3ffffff2;
						lAsyncBank2 			= 0x3ffffff2;
						lAsyncBank3 			= 0x3ffffff2;
						lExtended				= 0x01070400;
						lLowPowerExtendedMode	= 0;
						lBurstMode				= 0;
						lPageMode				= 0x00000042;
						lRecovery				= 0x000000;

						if	(	(pDevExtIn->eBoardId == eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200)
							||	(pDevExtIn->eBoardId == eBOARD_ID_FPGA1_ERTEC200P_XHIF_ET200))
							// ET200 version
						{
							lAsyncWaitCycle	= 0x0f000080;
							lAsyncBank0 	= 0x00100101;
							lAsyncBank2 	= 0x40200101;
							lAsyncBank3 	= 0x40200101;
						}
					}

					// configure EMC
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__ASYNC_WAIT_CYCLE),			lAsyncWaitCycle);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__SDRAM_CONFIG),				lSdramConfig);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__SDRAM_REFRESH),				lSdramRefresh);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK0),				lAsyncBank0);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK1),				lAsyncBank1);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK2),				lAsyncBank2);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK3),				lAsyncBank3);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__EXTENDED),					lExtended);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__LOW_POWER_EXTENDED_MODE),	lLowPowerExtendedMode);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__BURST_MODE),					lBurstMode);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__PAGE_MODE),					lPageMode);
					WRITE_REG_UINT32((pEmcReg + ERTEC200P_EMC_REG__RECOV),						lRecovery);

					// read registers
					lAsyncWaitCycle			= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__ASYNC_WAIT_CYCLE);
					lSdramConfig			= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__SDRAM_CONFIG);
					lSdramRefresh			= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__SDRAM_REFRESH);
					lAsyncBank0 			= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK0);
					lAsyncBank1 			= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK1);
					lAsyncBank2 			= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK2);
					lAsyncBank3 			= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__ASYNC_BANK3);
					lExtended				= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__EXTENDED);
					lLowPowerExtendedMode	= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__LOW_POWER_EXTENDED_MODE);
					lBurstMode				= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__BURST_MODE);
					lPageMode				= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__PAGE_MODE);
					lRecovery				= READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__RECOV);

					// wait for confirmation of EMC controller
					//	- should be done after 3 clocks
					{
					BOOLEAN	bErrorDelay		= FALSE;
					BOOLEAN	bErrorTimeout	= FALSE;
					UINT32	i = 0;

						// preset
						bErrorTimeout = TRUE;

						// wait for confirmation of EMC controller
						//	- should be done after 3 clocks

						for	(i = 0; i < 100; i++)
							// MaxTime=100msec
						{
							if	((READ_REG_UINT32(pEmcReg + ERTEC200P_EMC_REG__SDRAM_REFRESH) & ERTEC200P_EMC_SDRAM_REFRESH__INIT_DONE) == 0)
								// SDRAM initialization not done
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
												_TEXT("(): Timeout, error at confirmation of EMC controller!"),
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
	//------------------------------------------------------------------------
	// [XHIF: end of using dynamic DriverPage]
	fnBoardPnip_StopDriverPageXhif(pDevExtIn);

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	reset PNIP-RAMs which are not initialized by hardware
//************************************************************************

void fnBoardPnip_ResetRam(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 i = 0;

	// reset StatisticRam
	//	- not all cells will be cleared, only cells with real counters
	//WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__STATISTIC_CTRL),
	//					0x0000000F);

	// reset StatisticRam
	for (i = 0; i < PNIP_RAM__SIZE_STATISTIC; i+=4)
	{
		WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_RAM__OFFSET_STATISTIC + i),
							0x00000000);
	}

	// reset SyncRam
	for (i = 0; i < PNIP_RAM__SIZE_SYNC; i+=4)
	{
		WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_RAM__OFFSET_SYNC + i),
							0x00000000);
	}
/*
	// reset PackCtrlRam
	for (i = 0; i < PNIP_RAM__SIZE_PACK_CTRL; i+=4)
	{
		WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_RAM__OFFSET_PACK_CTRL + i),
							0x00000000);
	}

	// reset PackDataRam
	for (i = 0; i < PNIP_RAM__SIZE_PACK_DATA; i+=4)
	{
		WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_RAM__OFFSET_PACK_DATA + i),
							0x00000000);
	}
*/
	// reset CmdRam
	for (i = 0; i < PNIP_RAM__SIZE_CMD; i+=4)
	{
		WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_RAM__OFFSET_CMD + i),
							0x00000000);
	}

	// reset ApiCtrlRam
	for (i = 0; i < PNIP_RAM__SIZE_API_CTRL; i+=4)
	{
		WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_RAM__OFFSET_API_CTRL + i),
							0x00000000);
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get value of ConfigPins
//************************************************************************

UINT32 fnBoardPnip_GetValueConfigPins(DEVICE_EXTENSION* pDevExtIn)
{
UINT32 lResult = 0;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					lResult = BOARD_ERTEC200P__CONFIG_PINS_AHB;

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					lResult = BOARD_ERTEC200P__CONFIG_PINS_XHIF;

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
			lResult = BOARD_ERTEC200P__CONFIG_PINS_XHIF;

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
//  test GPIO
//************************************************************************

BOOLEAN fnBoardPnip_TestGpio(	DEVICE_EXTENSION*	pDevExtIn,
								volatile UINT8*		pApbPerIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult	= FALSE;

	// set HwSignals for debugging
	{
		// set as output: GPIO31:0
		WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__IOCTRL_0),
							0);

		// set GPIO2
		WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_SET_0),
							PNDEV_UINT32_SET_BIT_2);
		// clear GPIO2
		WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_CLEAR_0),
							PNDEV_UINT32_SET_BIT_2);

		// set GPIO3
		WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_SET_0),
							PNDEV_UINT32_SET_BIT_3);
		// clear GPIO3
		WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_CLEAR_0),
							PNDEV_UINT32_SET_BIT_3);

		// set GPIO4
		WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_SET_0),
							PNDEV_UINT32_SET_BIT_4);
		// clear GPIO4
		WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_CLEAR_0),
							PNDEV_UINT32_SET_BIT_4);
	}

	// test IntHandling by GPIO4
	{
	BOOLEAN	bResultIcu					= FALSE;
	UINT32	lGpioTmp					= 0;
	UINT32	lIrrTmp						= 0;
	UINT32	lPnIpReg_IcuIrq0_AckMid		= 0;
	UINT32	lPnIpReg_IcuIrq0_EventMid	= 0;
	BOOLEAN	bDebug						= FALSE;

		switch	(pDevExtIn->eBoardDetail)
				// BoardDetail
		{
			case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
			{
				// success
				bResultIcu = TRUE;

				#ifdef FPGA1_ERTEC200P_ARM_NO_TOPLEVEL_ICU

					// PNIP-ICU2 is used
					lPnIpReg_IcuIrq0_AckMid		= PNIP_REG__ICU2_IRQ0_ACK_MID;
					lPnIpReg_IcuIrq0_EventMid	= PNIP_REG__ICU2_IRQ0_EVENT_MID;

				#else

					// TopLevel-ICU = ARM-ICU

					// PNIP-ICU2 is used
					lPnIpReg_IcuIrq0_AckMid		= PNIP_REG__ICU2_IRQ0_ACK_MID;
					lPnIpReg_IcuIrq0_EventMid	= PNIP_REG__ICU2_IRQ0_EVENT_MID;

				#endif

				break;
			}
			case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
			{
				// success
				bResultIcu = TRUE;

				// TopLevel-ICU = PERIF-ICU

				// PNIP-ICU3 is used
				lPnIpReg_IcuIrq0_AckMid		= PNIP_REG__ICU3_IRQ0_ACK_MID;
				lPnIpReg_IcuIrq0_EventMid	= PNIP_REG__ICU3_IRQ0_EVENT_MID;

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

		if	(bResultIcu)
			// success
		{
			// Ertec200P: GPIO7:4 are connected to PN-ICU

			// initialize GPIO
			//	- GPIO4 = input
			WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__IOCTRL_0),
								PNDEV_UINT32_SET_BIT_4);

			// connect GPIO4 and GPIO3 by cable

			// ack interrupt PN_ext_IRQ0 (connected to GPIO4)
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + lPnIpReg_IcuIrq0_AckMid),
								PNDEV_UINT32_SET_BIT_24);

			lGpioTmp	= READ_REG_UINT32(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__IN_0);
			lIrrTmp		= READ_REG_UINT32(pDevExtIn->pPnIp + lPnIpReg_IcuIrq0_EventMid);

			// set GPIO3
			WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_SET_0),
								PNDEV_UINT32_SET_BIT_3);

			lGpioTmp = READ_REG_UINT32(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__IN_0);

			// clear GPIO3
			WRITE_REG_UINT32(	(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_CLEAR_0),
								PNDEV_UINT32_SET_BIT_3);

			lIrrTmp = READ_REG_UINT32(pDevExtIn->pPnIp + lPnIpReg_IcuIrq0_EventMid);

			if	(	(lGpioTmp	!= 0x18)
				||	(lIrrTmp)	!= PNDEV_UINT32_SET_BIT_24)
				// error
			{
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				// don't return ErrorString because GPIO pins are not available at all boards and are not always connected!

				// success
				bResult = TRUE;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Error at testing of GPIO (Test Pin3/4 connected?)!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);

/*
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Interrupt by GPIO does not work!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
*/
			}
			else
			{
				// success
				bResult = TRUE;
			}

			// ack interrupt PN_ext_IRQ0 (connected to GPIO4)
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + lPnIpReg_IcuIrq0_AckMid),
								PNDEV_UINT32_SET_BIT_24);

			lGpioTmp	= READ_REG_UINT32(pApbPerIn + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__IN_0);
			lIrrTmp		= READ_REG_UINT32(pDevExtIn->pPnIp + lPnIpReg_IcuIrq0_EventMid);

			// debug purpose
			bDebug = FALSE;
		}
	}

	return(bResult);
}
