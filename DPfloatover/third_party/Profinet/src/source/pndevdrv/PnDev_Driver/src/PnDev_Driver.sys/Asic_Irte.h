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
/*  F i l e               &F: Asic_Irte.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for Asics with IRTE (Ertec200/400/Soc1)
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __Asic_Irte_h__
#define __Asic_Irte_h__

	//########################################################################
	//  Defines
	//########################################################################

	//========================================================================
	//	AHB addresses/sizes
	//========================================================================

	//------------------------------------------------------------------------
	//	Ertec200
	//------------------------------------------------------------------------
	#define ERTEC200_AHB_ADR__IRTE				0x10000000
	#define ERTEC200_AHB_ADR__EMIF_SDRAM		0x20000000
	#define ERTEC200_AHB_ADR__EMIF_ASYNC_BANK0	0x30000000
	#define ERTEC200_AHB_ADR__EMIF_ASYNC_BANK1	0x31000000
	#define ERTEC200_AHB_ADR__EMIF_ASYNC_BANK2	0x32000000
	#define ERTEC200_AHB_ADR__EMIF_ASYNC_BANK3	0x33000000
	#define ERTEC200_AHB_ADR__APB_PER			0x40000000
	#define ERTEC200_AHB_ADR__ARM_ICU			0x50000000
	#define ERTEC200_AHB_ADR__EMIF_REG			0x70000000

	#define ERTEC200_AHB_SIZE__IRTE				(1024*1024*2)
	#define ERTEC200_AHB_SIZE__APB_PER			(1024*10)						// 0x0000_2800
	#define ERTEC200_AHB_SIZE__EMIF_REG			(1024*1024*1)

	// sizes of EMIF_SDRAM and EMIF_ASYNC_BANKx do not depend on Asic but on board

	//------------------------------------------------------------------------
	//	Ertec400
	//------------------------------------------------------------------------
	#define ERTEC400_AHB_ADR__IRTE				0x10000000
	#define ERTEC400_AHB_ADR__EMIF_SDRAM		0x20000000
	#define ERTEC400_AHB_ADR__EMIF_ASYNC_BANK0	0x30000000
	#define ERTEC400_AHB_ADR__EMIF_ASYNC_BANK1	0x31000000
	#define ERTEC400_AHB_ADR__EMIF_ASYNC_BANK2	0x32000000
	#define ERTEC400_AHB_ADR__EMIF_ASYNC_BANK3	0x33000000
	#define ERTEC400_AHB_ADR__APB_PER			0x40000000
	#define ERTEC400_AHB_ADR__SRAM				0x60000000
	#define ERTEC400_AHB_ADR__EMIF_REG			0x70000000

	#define ERTEC400_AHB_SIZE__IRTE				(1024*1024*2)
	#define ERTEC400_AHB_SIZE__APB_PER			(1024*10)						// 0x0000_2800
	#define ERTEC400_AHB_SIZE__SRAM				(1024*8)
	#define ERTEC400_AHB_SIZE__EMIF_REG			(1024*1024*1)

	// sizes of EMIF_SDRAM and EMIF_ASYNC_BANKx do not depend on Asic but on board

	//------------------------------------------------------------------------
	//	Soc1
	//------------------------------------------------------------------------
	#define SOC1_AHB_ADR__TPA_RAM						0x15e00000
	#define SOC1_AHB_ADR__TRACE_TIMER_UNIT				0x16000000
	#define SOC1_AHB_ADR__LOCAL_BUS0					0x18000000
	#define SOC1_AHB_ADR__LOCAL_BUS1					0x19000000
	#define SOC1_AHB_ADR__LOCAL_BUS2					0x1a000000
	#define SOC1_AHB_ADR__LOCAL_BUS3					0x1b000000
	#define SOC1_AHB_ADR__LOCAL_BUS4					0x1c000000
	#define SOC1_AHB_ADR__LOCAL_BUS_REG					0x1d000000
	#define SOC1_AHB_ADR__IRTE							0x1d200000
	#define SOC1_AHB_ADR__PB_COM_RAM					0x1d400000
	#define SOC1_AHB_ADR__PCI_BRIDGE_REG				0x1d500000
	#define SOC1_AHB_ADR__IOCC							0x1d600000
	#define SOC1_AHB_ADR__MMC_SD						0x1e000000
	#define SOC1_AHB_ADR__GDMA							0x1e100000
	#define SOC1_AHB_ADR__GICU							0x1e200000
	#define SOC1_AHB_ADR__PCI_ICU						0x1e300000
	#define SOC1_AHB_ADR__ECC_ICU						0x1e400000
	#define SOC1_AHB_ADR__DDR2C_REG						0x1e700000
	#define SOC1_AHB_ADR__DSA_SOFT_PLL					0x1e800000
	#define SOC1_AHB_ADR__APB_PER						0x1ea00000
	#define SOC1_AHB_ADR__DDR2_SDRAM					0x40000000
	#define SOC1_AHB_ADR__DDR2_SDRAM_BY_MIPS_CACHEABLE	0x80000000
	#define SOC1_AHB_ADR__DDR2_SDRAM_BY_MIPS_UNCACHED	0xa0000000

	#define SOC1_AHB_SIZE__TPA_RAM						(1024*8)
	#define SOC1_AHB_SIZE__TRACE_TIMER_UNIT				(1024*1)
	#define SOC1_AHB_SIZE__LOCAL_BUS_REG				(1024*1024*1)
	#define SOC1_AHB_SIZE__IRTE							(1024*1024*2)
	#define SOC1_AHB_SIZE__PB_COM_RAM					(1024*1024*1)
	#define SOC1_AHB_SIZE__PCI_BRIDGE_REG				(1024*1024*1)
	#define SOC1_AHB_SIZE__IOCC							(1024*1024*2)
	#define SOC1_AHB_SIZE__MMC_SD						(1024*1024*1)
	#define SOC1_AHB_SIZE__GDMA							(1024*1024*1)
	#define SOC1_AHB_SIZE__GICU							(1024*1024*1)
	#define SOC1_AHB_SIZE__PCI_ICU						(1024*1024*1)
	#define SOC1_AHB_SIZE__ECC_ICU						(1024*1024*1)
	#define SOC1_AHB_SIZE__DDR2C_REG					(1024*1024*1)
	#define SOC1_AHB_SIZE__DSA_SOFT_PLL					(1024*1024*1)
	#define SOC1_AHB_SIZE__APB_PER						(1024*1024*22)			// 0x1ea0_0000...(0x2000_0000 - 1)

	// sizes of DDR2_SDRAM and LOCAL_BUSx do not depend on Asic but on board

	//========================================================================
	//	IRTE
	//========================================================================

	#define IRTE_REG_ERTECX00__SWITCH_CONTROL	0x00019038
	#define IRTE_REG_SOC__SWITCH_CONTROL		0x0001905c

	#define IRTE_REG__IP_VERSION				0x00019400
	#define IRTE_REG__SWITCH_STATUS				0x00019404

	//------------------------------------------------------------------------
	// IP Version Register:
	#define IRTE_IP_VERSION__REV5_NO_METAL_FIX		0x20051600					// Ertec400
	#define IRTE_IP_VERSION__REV5_METAL_FIX			0x20051601					// Ertec400-MetalFix

	#define IRTE_IP_VERSION__REV6_NO_METAL_FIX		0x3b060801					// Ertec200
	#define IRTE_IP_VERSION__REV6_METAL_FIX			0x3b060802					// Ertec200-MetalFix

	#define IRTE_IP_VERSION__REV7_NO_METAL_FIX		0x47070901					// Soc1
	#define IRTE_IP_VERSION__REV7_METAL_FIX			0x47070902					// Soc1-MetalFixA/B

	#define IRTE_IP_VERSION_SOC2__REV7_NO_METAL_FIX	0x5F070901					// Soc2
	#define IRTE_IP_VERSION_SOC2__REV7_METAL_FIX	0x5F070902					// Soc2-MetalFix

	//------------------------------------------------------------------------
	// IRTE-ICU
	//------------------------------------------------------------------------
	#define IRTE_REG__ICU_MODE				0x00017000
	#define IRTE_REG__ICU_IRQ0_MASK_IRT		0x00017004
	#define IRTE_REG__ICU_IRQ0_MASK_NRT		0x00017008
	#define IRTE_REG__ICU_IRQ1_MASK_IRT		0x0001700c
	#define IRTE_REG__ICU_IRQ1_MASK_NRT		0x00017010
	#define IRTE_REG__ICU_IRQ0_IR_IRT		0x00017400
	#define IRTE_REG__ICU_IRQ0_IR_NRT		0x00017404
	#define IRTE_REG__ICU_IRQ1_IR_IRT		0x00017408
	#define IRTE_REG__ICU_IRQ1_IR_NRT		0x0001740c
	#define IRTE_REG__ICU_ACK_IRT			0x00017410
	#define IRTE_REG__ICU_ACK_NRT			0x00017414
	#define IRTE_REG__ICU_IRR_IRT			0x00017418
	#define IRTE_REG__ICU_IRR_NRT			0x0001741c
	#define IRTE_REG__ICU_IRQ0_EOI			0x00017420
	#define IRTE_REG__ICU_IRQ1_EOI			0x00017424

	//------------------------------------------------------------------------
	// IRQs:
	#define IRTE_IRQ_NRT__HP	0x00001000

	//========================================================================
	//	APB-Peripherals
	//========================================================================

	//------------------------------------------------------------------------
	//	Ertec200
	//------------------------------------------------------------------------
	#define ERTEC200_APB_PER__OFFSET_TIMER0_2	0x00002000
	#define ERTEC200_APB_PER__OFFSET_WATCHDOG	0x00002100
	#define ERTEC200_APB_PER__OFFSET_SPI		0x00002200
	#define ERTEC200_APB_PER__OFFSET_UART		0x00002300
	#define ERTEC200_APB_PER__OFFSET_GPIO		0x00002500
	#define ERTEC200_APB_PER__OFFSET_SCRB		0x00002600

	#define ERTEC200_APB_PER__SIZE_TIMER0_2		(256)
	#define ERTEC200_APB_PER__SIZE_WATCHDOG		(256)
	#define ERTEC200_APB_PER__SIZE_SPI			(256)
	#define ERTEC200_APB_PER__SIZE_UART			(256)
	#define ERTEC200_APB_PER__SIZE_GPIO			(256)
	#define ERTEC200_APB_PER__SIZE_SCRB			(256)

	//------------------------------------------------------------------------
	// SCRB Register:
	#define ERTEC200_SCRB_REG__ID			0x00
	#define ERTEC200_SCRB_REG__BOOT			0x04
	#define ERTEC200_SCRB_REG__CONFIG		0x08
	#define ERTEC200_SCRB_REG__RES_CTRL		0x0c
	#define ERTEC200_SCRB_REG__RES_STATUS	0x10
	#define ERTEC200_SCRB_REG__ARM9_CTRL	0x50

	//------------------------------------------------------------------------
	// GPIO Register:
	#define ERTEC200_GPIO_REG__IOCTRL0		0x00
	#define ERTEC200_GPIO_REG__OUT0			0x04
	#define ERTEC200_GPIO_REG__IN0			0x08
	#define ERTEC200_GPIO_REG__PORT_MODE_L0	0x0c

	//------------------------------------------------------------------------
	//	Ertec400
	//------------------------------------------------------------------------
	#define ERTEC400_APB_PER__OFFSET_TIMER0_1	0x00002000
	#define ERTEC400_APB_PER__OFFSET_WATCHDOG	0x00002100
	#define ERTEC400_APB_PER__OFFSET_SPI		0x00002200
	#define ERTEC400_APB_PER__OFFSET_UART1		0x00002300
	#define ERTEC400_APB_PER__OFFSET_UART2		0x00002400
	#define ERTEC400_APB_PER__OFFSET_GPIO		0x00002500
	#define ERTEC400_APB_PER__OFFSET_SCRB		0x00002600

	#define ERTEC400_APB_PER__SIZE_TIMER0_1		(256)
	#define ERTEC400_APB_PER__SIZE_WATCHDOG		(256)
	#define ERTEC400_APB_PER__SIZE_SPI			(256)
	#define ERTEC400_APB_PER__SIZE_UART1		(256)
	#define ERTEC400_APB_PER__SIZE_UART2		(256)
	#define ERTEC400_APB_PER__SIZE_GPIO			(256)
	#define ERTEC400_APB_PER__SIZE_SCRB			(256)

	//------------------------------------------------------------------------
	// SCRB Register:
	#define ERTEC400_SCRB_REG__ID			0x00
	#define ERTEC400_SCRB_REG__BOOT			0x04
	#define ERTEC400_SCRB_REG__CONFIG		0x08
	#define ERTEC400_SCRB_REG__RES_CTRL		0x0c
	#define ERTEC400_SCRB_REG__RES_STATUS	0x10
	#define ERTEC400_SCRB_REG__ARM9_CTRL	0x50

	//------------------------------------------------------------------------
	// GPIO Register:
	#define ERTEC400_GPIO_REG__IOCTRL0		0x00
	#define ERTEC400_GPIO_REG__OUT0			0x04
	#define ERTEC400_GPIO_REG__OUT_SET0		0x08
	#define ERTEC400_GPIO_REG__OUT_CLEAR0	0x0c

	//------------------------------------------------------------------------
	//	Soc1
	//------------------------------------------------------------------------
	#define SOC1_APB_PER__OFFSET_I2C			(0x1eb00000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_PB_KRISC_REG	(0x1ef00000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_UART1			(0x1f400000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_UART2			(0x1f480000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_UART3			(0x1f500000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_SPI			(0x1f600000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_WATCHDOG		(0x1f700000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_SEMAPHORE		(0x1f800000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_TIMER			(0x1f900000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_GPIO			(0x1fa00000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_SCRB			(0x1fb00000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_BOOT_ROM		(0x1fc00000 - SOC1_AHB_ADR__APB_PER)
	#define SOC1_APB_PER__OFFSET_SPS			(0x1fe00000 - SOC1_AHB_ADR__APB_PER)

	#define SOC1_APB_PER__SIZE_I2C				(1024*512)
	#define SOC1_APB_PER__SIZE_PB_KRISC_REG		(1024*1024*1)
	#define SOC1_APB_PER__SIZE_UART1			(1024*512)
	#define SOC1_APB_PER__SIZE_UART2			(1024*512)
	#define SOC1_APB_PER__SIZE_UART3			(1024*512)
	#define SOC1_APB_PER__SIZE_SPI				(1024*1024*1)
	#define SOC1_APB_PER__SIZE_WATCHDOG			(1024*1024*1)
	#define SOC1_APB_PER__SIZE_SEMAPHORE		(1024*1024*1)
	#define SOC1_APB_PER__SIZE_TIMER			(1024*1024*1)
	#define SOC1_APB_PER__SIZE_GPIO				(1024*1024*1)
	#define SOC1_APB_PER__SIZE_SCRB				(1024*1024*1)
	#define SOC1_APB_PER__SIZE_BOOT_ROM			(1024*1024*1)
	#define SOC1_APB_PER__SIZE_SPS				(1024*1024*1)

	//------------------------------------------------------------------------
	// SCRB Register:
	#define SOC1_SCRB_REG__IRTE_CTRL			0x150

	#define SOC1_SCRB_REG__CONFIG				0x300
	#define SOC1_SCRB_REG__BOOT					0x304
	#define SOC1_SCRB_REG__RES_STATUS			0x310
	#define SOC1_SCRB_REG__RES_CTRL				0x314
	#define SOC1_SCRB_REG__RES_PULS_DURATION	0x318
	#define SOC1_SCRB_REG__RES_ENABLE			0x31c
	#define SOC1_SCRB_REG__RES_IOC_DISABLE		0x320

	#define SOC1_SCRB_REG__SOC1_ID				0x800							// 0x50c1
	
	#define SOC1_SCRB_REG__GPIO_PULL_CONTROL6	0xa34

	// Register RES_CTRL
	#define SOC1_RES_CTRL__ENABLE_WD_RESET		PNDEV_UINT32_SET_BIT_0

	// Register RES_STATUS
	#define SOC1_RES_STATUS_BIT1_0__HW_RESET	0
	#define SOC1_RES_STATUS_BIT1_0__WD_RESET	1
	#define SOC1_RES_STATUS_BIT1_0__SOFT_RESET	2

	//------------------------------------------------------------------------
	// Watchdog Register:
	#define SOC1_WD_REG__CTRL_STATUS			0x00
	#define SOC1_WD_REG__RELD0_L				0x04
	#define SOC1_WD_REG__RELD0_H				0x08
	#define SOC1_WD_REG__RELD1_L				0x0c
	#define SOC1_WD_REG__RELD1_H				0x10
	#define SOC1_WD_REG__WD0					0x14
	#define SOC1_WD_REG__WD1					0x18

	// Register BARx_CONS
	#define SOC1_WD_CTRL_STATUS__ENABLE0		PNDEV_UINT32_SET_BIT_0
	#define SOC1_WD_CTRL_STATUS__ENABLE1		PNDEV_UINT32_SET_BIT_1
	#define SOC1_WD_CTRL_STATUS__LOAD_TRIGGER	PNDEV_UINT32_SET_BIT_2
	#define SOC1_WD_CTRL_STATUS__EXPIRED0		PNDEV_UINT32_SET_BIT_3
	#define SOC1_WD_CTRL_STATUS__EXPIRED1		PNDEV_UINT32_SET_BIT_4

	//------------------------------------------------------------------------
	// GPIO Register:
	#define SOC1_GPIO_REG__IOCTRL0			0x00
	#define SOC1_GPIO_REG__OUT0				0x04
	#define SOC1_GPIO_REG__OUT_SET0			0x08
	#define SOC1_GPIO_REG__OUT_CLEAR0		0x0c
	#define SOC1_GPIO_REG__PORT_MODE0_L		0x18
	#define SOC1_GPIO_REG__PORT_MODE0_H		0x1c

	#define SOC1_GPIO_REG__IOCTRL1			0x20
	#define SOC1_GPIO_REG__OUT1				0x24
	#define SOC1_GPIO_REG__OUT_SET1			0x28
	#define SOC1_GPIO_REG__OUT_CLEAR1		0x2c
	#define SOC1_GPIO_REG__PORT_MODE1_L		0x38
	#define SOC1_GPIO_REG__PORT_MODE1_H		0x3c

	#define SOC1_GPIO_REG__IOCTRL2			0x40
	#define SOC1_GPIO_REG__OUT2				0x44
	#define SOC1_GPIO_REG__OUT_SET2			0x48
	#define SOC1_GPIO_REG__OUT_CLEAR2		0x4c
	#define SOC1_GPIO_REG__PORT_MODE2_L		0x58
	#define SOC1_GPIO_REG__PORT_MODE2_H		0x5c

	#define SOC1_GPIO_REG__IOCTRL3			0x60
	#define SOC1_GPIO_REG__OUT3				0x64
	#define SOC1_GPIO_REG__OUT_SET3			0x68
	#define SOC1_GPIO_REG__OUT_CLEAR3		0x6c
	#define SOC1_GPIO_REG__PORT_MODE3_L		0x78
	#define SOC1_GPIO_REG__PORT_MODE3_H		0x7c

	#define SOC1_GPIO_REG__IOCTRL4			0x80
	#define SOC1_GPIO_REG__OUT4				0x84
	#define SOC1_GPIO_REG__OUT_SET4			0x88
	#define SOC1_GPIO_REG__OUT_CLEAR4		0x8c
	#define SOC1_GPIO_REG__PORT_MODE4_L		0x98
	#define SOC1_GPIO_REG__PORT_MODE4_H		0x9c

	#define SOC1_GPIO_REG__IOCTRL5			0xa0
	#define SOC1_GPIO_REG__OUT5				0xa4
	#define SOC1_GPIO_REG__OUT_SET5			0xa8
	#define SOC1_GPIO_REG__OUT_CLEAR5		0xac
	#define SOC1_GPIO_REG__IN5				0xb4
	#define SOC1_GPIO_REG__PORT_MODE5_L		0xb8
	#define SOC1_GPIO_REG__PORT_MODE5_H		0xbc

	//========================================================================
	//	EMIF
	//========================================================================

	//------------------------------------------------------------------------
	//	Ertec200
	//------------------------------------------------------------------------
	#define ERTEC200_EMIF_REG__REVISION			0x00000000
	#define ERTEC200_EMIF_REG__ASYNC_WAIT_CYCLE	0x00000004
	#define ERTEC200_EMIF_REG__SDRAM			0x00000008
	#define ERTEC200_EMIF_REG__SDRAM_REFRESH	0x0000000c
	#define ERTEC200_EMIF_REG__ASYNC_BANK0		0x00000010
	#define ERTEC200_EMIF_REG__ASYNC_BANK1		0x00000014
	#define ERTEC200_EMIF_REG__ASYNC_BANK2		0x00000018
	#define ERTEC200_EMIF_REG__ASYNC_BANK3		0x0000001c
	#define ERTEC200_EMIF_REG__EXTENDED			0x00000020

	// Register SDRAM-Refresh
	#define ERTEC200_EMIF_SDRAM_REFRESH__INIT_DONE	(1<<29)

	//========================================================================
	//	LBU
	//========================================================================

	//------------------------------------------------------------------------
	//	Ertec200
	//------------------------------------------------------------------------
	#define ERTEC200_LBU_REG__P0_RG_L	0x00000000
	#define ERTEC200_LBU_REG__P0_RG_H	0x00000002
	#define ERTEC200_LBU_REG__P0_OF_L	0x00000004
	#define ERTEC200_LBU_REG__P0_OF_H	0x00000006
	#define ERTEC200_LBU_REG__P0_CFG	0x00000008

	#define ERTEC200_LBU_REG__P1_RG_L	0x00000010
	#define ERTEC200_LBU_REG__P1_RG_H	0x00000012
	#define ERTEC200_LBU_REG__P1_OF_L	0x00000014
	#define ERTEC200_LBU_REG__P1_OF_H	0x00000016
	#define ERTEC200_LBU_REG__P1_CFG	0x00000018

	#define ERTEC200_LBU_REG__P2_RG_L	0x00000020
	#define ERTEC200_LBU_REG__P2_RG_H	0x00000022
	#define ERTEC200_LBU_REG__P2_OF_L	0x00000024
	#define ERTEC200_LBU_REG__P2_OF_H	0x00000026
	#define ERTEC200_LBU_REG__P2_CFG	0x00000028

	#define ERTEC200_LBU_REG__P3_RG_L	0x00000030
	#define ERTEC200_LBU_REG__P3_RG_H	0x00000032
	#define ERTEC200_LBU_REG__P3_OF_L	0x00000034
	#define ERTEC200_LBU_REG__P3_OF_H	0x00000036
	#define ERTEC200_LBU_REG__P3_CFG	0x00000038

	#define	ERTEC200_LBU_REG__SIZE		0x40

	// Register BARx_CONS
	#define ERTEC200_LBU_PX_CFG__32_BIT	0x00000001

	//========================================================================
	//	LOCAL_BUS
	//========================================================================

	//------------------------------------------------------------------------
	//	Soc1
	//------------------------------------------------------------------------
	#define SOC1_LOCAL_BUS_REG__REVISION_CODE	0x00000000
	#define SOC1_LOCAL_BUS_REG__WAIT_CYCLE		0x00000004
	#define SOC1_LOCAL_BUS_REG__BANK0			0x00000008
	#define SOC1_LOCAL_BUS_REG__BANK1			0x0000000c
	#define SOC1_LOCAL_BUS_REG__BANK2			0x00000010
	#define SOC1_LOCAL_BUS_REG__BANK3			0x00000014
	#define SOC1_LOCAL_BUS_REG__BANK4			0x00000018
	#define SOC1_LOCAL_BUS_REG__EXTENDED		0x0000001c

	//========================================================================
	//	PCI-ICU
	//========================================================================

	//------------------------------------------------------------------------
	//	Soc1
	//------------------------------------------------------------------------
	#define SOC1_PCI_ICU_REG__LOCK_REQ_A		0x00000000
	#define SOC1_PCI_ICU_REG__IRQ_IRVEC_A		0x00000004
	#define SOC1_PCI_ICU_REG__IRQ_ACK_A			0x00000008
	#define SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A		0x0000000c
	#define SOC1_PCI_ICU_REG__IRQ_MASKALL_A		0x00000010
	#define SOC1_PCI_ICU_REG__IRQ_MASKREG_A		0x00000014
	#define SOC1_PCI_ICU_REG__IRQ_END_A			0x00000018
	#define SOC1_PCI_ICU_REG__IRQ_IRREG_A		0x0000001c
	#define SOC1_PCI_ICU_REG__IRQ_ISREG_A		0x00000020
	#define SOC1_PCI_ICU_REG__IRQ_TRIGREG_A		0x00000024
	#define SOC1_PCI_ICU_REG__IRQ_EDGEREG_A		0x00000028
	#define SOC1_PCI_ICU_REG__IRQ_SWIRREG_A		0x0000002c
	#define SOC1_PCI_ICU_REG__IRQ_PRIOREG_A1	0x00000034

	#define SOC1_PCI_ICU_REG__IRQ_LOCKREG_B		0x000000b4
	#define SOC1_PCI_ICU_REG__IRQ_IRVEC_B		0x000000b8
	#define SOC1_PCI_ICU_REG__IRQ_ACK_B			0x000000bc
	#define SOC1_PCI_ICU_REG__IRQ_IRCLVEC_B		0x000000c0
	#define SOC1_PCI_ICU_REG__IRQ_MASKALL_B		0x000000c4
	#define SOC1_PCI_ICU_REG__IRQ_MASKREG_B		0x000000c8
	#define SOC1_PCI_ICU_REG__IRQ_END_B			0x000000cc
	#define SOC1_PCI_ICU_REG__IRQ_IRREG_B		0x000000d0
	#define SOC1_PCI_ICU_REG__IRQ_ISREG_B		0x000000d4
	#define SOC1_PCI_ICU_REG__IRQ_TRIGREG_B		0x000000d8
	#define SOC1_PCI_ICU_REG__IRQ_EDGEREG_B		0x000000dc
	#define SOC1_PCI_ICU_REG__IRQ_SWIRREG_B		0x000000e0

	#define SOC1_PCI_ICU_REG__ID_REGISTER		0x00000168

	#define SOC1_PCI_ICU_REG__IRQ_SEL_A1		0x0000016c
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A2		0x00000170
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A3		0x00000174
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A4		0x00000178
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A5		0x0000017c
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A6		0x00000180
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A7		0x00000184
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A8		0x00000188
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A9		0x0000018c
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A10		0x00000190
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A11		0x00000194
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A12		0x00000198
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A13		0x0000019c
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A14		0x000001a0
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A15		0x000001a4
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A16		0x000001a8
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A17		0x000001ac
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A18		0x000001b0
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A19		0x000001b4
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A20		0x000001b8
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A21		0x000001bc
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A22		0x000001c0
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A23		0x000001c4
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A24		0x000001c8
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A25		0x000001cc
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A26		0x000001d0
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A27		0x000001d4
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A28		0x000001d8
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A29		0x000001dc
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A30		0x000001e0
	#define SOC1_PCI_ICU_REG__IRQ_SEL_A31		0x000001e4

	#define SOC1_PCI_ICU_REG__IRQ_SEL_B1		0x000001e8
	//------------------------------------------------------------------------
	// pin of IntSrc
	#define SOC1_PCI_ICU_PIN__GDMA			7
	#define SOC1_PCI_ICU_PIN__TIMER0		22
	#define SOC1_PCI_ICU_PIN__TIMER1		23
	#define SOC1_PCI_ICU_PIN__TIMER2		24
	#define SOC1_PCI_ICU_PIN__TIMER3		25
	#define SOC1_PCI_ICU_PIN__TIMER4		26
	#define SOC1_PCI_ICU_PIN__TIMER5		27
	#define SOC1_PCI_ICU_PIN__GPIO180		126
	#define SOC1_PCI_ICU_PIN__GPIO188		127
	#define SOC1_PCI_ICU_PIN__IRTE_IRQ1		161
	#define SOC1_PCI_ICU_PIN__PB_PCI_F1		162
	#define SOC1_PCI_ICU_PIN__PB_PCI_F2		163
	//------------------------------------------------------------------------
	// MuxA-Vector
	//	- private mapping of IntSrc (pin) to vector (MuxInput) by IRQ_SEL_A1..31
	#define SOC1_PCI_ICU_VECTOR_MUX_A__DEFAULT		0							// DefaultVector
	#define SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ1	1							// IRTE-IRQ1 mapped to Vector1
	#define SOC1_PCI_ICU_VECTOR_MUX_A__TIMER0		2
	#define SOC1_PCI_ICU_VECTOR_MUX_A__TIMER1		3
	#define SOC1_PCI_ICU_VECTOR_MUX_A__TIMER2		4
	#define SOC1_PCI_ICU_VECTOR_MUX_A__TIMER3		5
	#define SOC1_PCI_ICU_VECTOR_MUX_A__TIMER4		6
	#define SOC1_PCI_ICU_VECTOR_MUX_A__TIMER5		7
	#define SOC1_PCI_ICU_VECTOR_MUX_A__GDMA			8
	#define SOC1_PCI_ICU_VECTOR_MUX_A__GPIO180		9
	#define SOC1_PCI_ICU_VECTOR_MUX_A__GPIO188		10
	#define SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F1	11
	#define SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F2	12
	#define SOC1_PCI_ICU_VECTOR_MUX_A__13			13
	#define SOC1_PCI_ICU_VECTOR_MUX_A__14			14
	#define SOC1_PCI_ICU_VECTOR_MUX_A__15			15
	#define SOC1_PCI_ICU_VECTOR_MUX_A__16			16
	#define SOC1_PCI_ICU_VECTOR_MUX_A__17			17
	#define SOC1_PCI_ICU_VECTOR_MUX_A__18			18
	#define SOC1_PCI_ICU_VECTOR_MUX_A__19			19
	#define SOC1_PCI_ICU_VECTOR_MUX_A__20			20
	#define SOC1_PCI_ICU_VECTOR_MUX_A__21			21
	#define SOC1_PCI_ICU_VECTOR_MUX_A__22			22
	#define SOC1_PCI_ICU_VECTOR_MUX_A__23			23
	#define SOC1_PCI_ICU_VECTOR_MUX_A__24			24
	#define SOC1_PCI_ICU_VECTOR_MUX_A__25			25
	#define SOC1_PCI_ICU_VECTOR_MUX_A__26			26
	#define SOC1_PCI_ICU_VECTOR_MUX_A__27			27
	#define SOC1_PCI_ICU_VECTOR_MUX_A__28			28
	#define SOC1_PCI_ICU_VECTOR_MUX_A__29			29
	#define SOC1_PCI_ICU_VECTOR_MUX_A__30			30
	#define SOC1_PCI_ICU_VECTOR_MUX_A__31			31

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

#endif
