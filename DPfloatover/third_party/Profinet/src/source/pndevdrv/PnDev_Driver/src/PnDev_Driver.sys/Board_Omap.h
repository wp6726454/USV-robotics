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
/*  F i l e               &F: Board_Omap.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for boards with Omap
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Kerl			xx
/*****************************************************************************/

#ifndef __Board_Omap_h__
#define __Board_Omap_h__

	//########################################################################
	//  Defines
	//########################################################################

	#define OMAP_AHB_ADR__L4PER1						0x48000000
    #define OMAP_AHB_ADR__UART3							0x48020000
	#define OMAP_AHB_ADR__TIMER							0x48032000
	#define OMAP_AHB_ADR__APB							0x48051000
	#define OMAP_AHB_ADR__GMAC_SW						0x48484000
	#define OMAP_AHB_ADR__CTRL_MODULE_CORE				0x4A002000
	#define OMAP_AHB_ADR__CM_CORE_AON					0x4A005000 
	#define OMAP_AHB_ADR__CM_CORE						0x4A008000
	#define OMAP_AHB_ADR__DMA_SYSTEM					0x4A056000
	#define OMAP_AHB_ADR__EDMA							0x4A161000
	#define OMAP_AHB_ADR__PRUSS1Config					0x4A175000
	#define OMAP_AHB_ADR__PRUSS1						0x4B200000
	#define OMAP_AHB_ADR__PCIE_SS1						0x51000000
	#define OMAP_AHB_ADR__PCIE_SS2						0x51800000

	#define OMAP_AHB_ADR__EMIF_SDRAM					0x80000000
	#define OMAP_AHB_ADR__QSPI							0x5C000000

	#define OMAP_AHB_SIZE__L4PER1						(1024*8)
	#define OMAP_AHB_SIZE__UART3						(1024*8)
	#define OMAP_AHB_SIZE__TIMER						(1024*56)
	#define OMAP_AHB_SIZE__APB							(1024*140)
	#define OMAP_AHB_SIZE__GMAC_SW						(1024*20)
 	#define OMAP_AHB_SIZE__CTRL_MODULE_CORE				(1024*8)
    #define OMAP_AHB_SIZE__CM_CORE_AON					(1024*8)
    #define OMAP_AHB_SIZE__CM_CORE						(1024*12)
    #define OMAP_AHB_SIZE__DMA_SYSTEM					(1024*8)
    #define OMAP_AHB_SIZE__EDMA							(1024*16)
    #define OMAP_AHB_SIZE__PRUSS1Config					(1024*8)
    #define OMAP_AHB_SIZE__PRUSS1						(1024*512)
	#define OMAP_AHB_SIZE__PCIE_SS1						(1024*1024*8)
	#define OMAP_AHB_SIZE__PCIE_SS2						(1024*1024*8)
   	#define OMAP_AHB_SIZE__EMIF_SDRAM					(1024*1024*64)
    #define OMAP_AHB_SIZE__QSPI							(1024*1024*64)

	//------------------------------------------------------------------------
	//	APB-Peripherals
	//------------------------------------------------------------------------

 	#define OMAP_APB_PER__OFFSET_GPIO		0x00000000
 	#define OMAP_APB_PER__OFFSET_I2C1		0x0001F000
 	#define OMAP_APB_PER__OFFSET_I2C2		0x00021000
	#define OMAP_APB_PER__OFFSET_I2C3		0x0000F000
	#define OMAP_APB_PER__OFFSET_UART		0x00015000

 	#define OMAP_APB_PER__SIZE_GPIO			(1024*7*8)
 	#define OMAP_APB_PER__SIZE_I2C1			(1024*8)
	#define OMAP_APB_PER__SIZE_I2C2			(1024*8)
	#define OMAP_APB_PER__SIZE_I2C3			(1024*8)
    #define OMAP_APB_PER__SIZE_UART			(1024*5*8)

	#define BOARD_TI_OMAP__PRM_RSTCTRL				0x2E07D00
	#define BOARD_TI_OMAP__PRM_RSTST				0x2E07D04

	#define BOARD_TI_OMAP__CM_SHADOW_FREQ_CONFIG1	0x2005260
	
	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_TI_OMAP__BAR0_SIZE	(1024*1024*512)
	#define	BOARD_TI_OMAP__BAR1_SIZE	(1024*0)
	#define	BOARD_TI_OMAP__BAR2_SIZE	(1024*1024*64)
	#define	BOARD_TI_OMAP__BAR3_SIZE	(1024*0)
	#define	BOARD_TI_OMAP__BAR4_SIZE	(1024*1024*64)
	#define	BOARD_TI_OMAP__BAR5_SIZE	(1024*0)

	#define	BOARD_TI_OMAP__OFFSET_L4PER1				0x08000000

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define	BOARD_TI_OMAP__BAR0_OFFSET_L4PER1				0x00000000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_UART3				0x00020000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_TIMER				0x00032000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_APB					0x00051000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_GMAC_SW				0x00484000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_CTRL_MODULE_CORE		0x02002000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_CM_CORE_AON			0x02005000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_CM_CORE				0x02008000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_DMA_SYSTEM			0x02056000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_EDMA					0x02161000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_PRUSS1Config			0x02175000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_PRUSS1				0x03200000 + BOARD_TI_OMAP__OFFSET_L4PER1
	#define	BOARD_TI_OMAP__BAR0_OFFSET_PCIE_SS1				0x09000000 + BOARD_TI_OMAP__OFFSET_L4PER1
	
	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_TI_OMAP__BAR2_OFFSET_EMIF_SDRAM			0x00000000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_TI_OMAP__BAR4_OFFSET_QSPI					0x00000000

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	PCI-Bridge Register
	//------------------------------------------------------------------------
	#define BOARD_TI_OMAP__PCIE_SS1_PL_CONF								(0x700)
	#define BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_INDEX				(0x200 + BOARD_TI_OMAP__PCIE_SS1_PL_CONF)
	#define BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LIMIT			(0x214 + BOARD_TI_OMAP__PCIE_SS1_PL_CONF)
	#define BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_BASE		(0x20C + BOARD_TI_OMAP__PCIE_SS1_PL_CONF)
	#define BOARD_TI_OMAP__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET	(0x218 + BOARD_TI_OMAP__PCIE_SS1_PL_CONF)

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------


	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
	#define BOARD_TI_OMAP__SDRAM_OFFSET_SBL_VERSION_TYPE	(0x0404 + 0x00)		// 32Bit
	#define BOARD_TI_OMAP__SDRAM_OFFSET_SBL_VERSION_MAJOR	(0x0408 + 0x00)		// 16Bit
	#define BOARD_TI_OMAP__SDRAM_OFFSET_SBL_VERSION_MINOR	(0x0408 + 0x02)		// 16Bit
		

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

#endif
