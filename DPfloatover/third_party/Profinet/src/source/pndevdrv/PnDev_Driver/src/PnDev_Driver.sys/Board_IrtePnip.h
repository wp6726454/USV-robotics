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
/*  F i l e               &F: Board_IrtePnip.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Common declarations for boards with IRTE or PNIP
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __Board_IrtePnip_h__
#define __Board_IrtePnip_h__

	//########################################################################
	//  Defines
	//########################################################################

	//========================================================================
	//	FPGA1-Xx board
	//========================================================================

	//------------------------------------------------------------------------
	//	Board Register
	//------------------------------------------------------------------------
	#define BOARD_FPGA1_REG__ID					0x00000000
	#define BOARD_FPGA1_REG__REVISION			0x00000004
	#define BOARD_FPGA1_REG__STATUS				0x00000008
	#define BOARD_FPGA1_REG__STREAM_ID			0x0000000c
	#define BOARD_FPGA1_REG__STREAM_VERSION		0x00000010
	#define BOARD_FPGA1_REG__CONTROL			0x00000020
	#define BOARD_FPGA1_REG__SCRATCH			0x00000024
	#define BOARD_FPGA1_REG__RESET				0x00000028
	#define BOARD_FPGA1_REG__BOOT_MODE			0x0000002c
	#define BOARD_FPGA1_REG__CONFIG				0x00000030

	//------------------------------------------------------------------------
	typedef enum _eBOARD_ID
	{
		eBOARD_ID_INVALID						= 0,
		eBOARD_ID_FPGA1_ERTEC200P_AHB_STD		= 0x00000001,
		eBOARD_ID_FPGA1_ERTEC200P_XHIF_STD		= 0x00000002,
		eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200		= 0x00000003,
		eBOARD_ID_FPGA1_ERTEC200P_XHIF_ET200	= 0x00000004,
		eBOARD_ID_FPGA1_ERTEC400				= 0x00001000,
		eBOARD_ID_FPGA1_ERTEC200				= 0x00001001,
		eBOARD_ID_FPGA1_SOC1					= 0x00001002,
		eBOARD_ID_EB200P						= 0x00002000
	}
	eBOARD_ID;

	//------------------------------------------------------------------------
	// Bits of BoardStatus register
	#define	BOARD_FPGA1_STATUS_RESET			PNDEV_UINT32_SET_BIT_0
	#define	BOARD_FPGA1_STATUS_PLL_LOC			PNDEV_UINT32_SET_BIT_1
	#define	BOARD_FPGA1_STATUS_INT				PNDEV_UINT32_SET_BIT_2
	#define	BOARD_FPGA1_STATUS_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_3			// PCIe access not finished by Asic -> Timeout

	//------------------------------------------------------------------------
	// Bits of BoardControl register
	#define	BOARD_FPGA1_CONTROL_SET_DEMO_INT			PNDEV_UINT32_SET_BIT_0
	#define	BOARD_FPGA1_CONTROL_ENABLE_INT				PNDEV_UINT32_SET_BIT_1
	#define	BOARD_FPGA1_CONTROL_XHIF_BUS_SIZE32			PNDEV_UINT32_SET_BIT_2
	#define	BOARD_FPGA1_CONTROL_PAGE_CS_R				PNDEV_UINT32_SET_BIT_3
	#define	BOARD_FPGA1_CONTROL_RESET_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_4

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 2x 16Bit NOR flash S29GL512P (2x 64MB):
	//	- used: 1x chip, 1MB (PCIe-XHIF: page0 -> 2MB would be possible
	//						  PCIe-AHB:  no pages, but only 1MB (BAR5))
	//			UsableSize:	1MB
	//			SectorSize:	128kB
	//			MacAdr at last 4kB of last sector
	#define	BOARD_FPGA1__FLASH_SIZE_USABLE	(1024*1024*1)
	#define	BOARD_FPGA1__FLASH_SIZE_SECTOR	(1024*128)
	#define	BOARD_FPGA1__FLASH_OFFSET_DATA	(0x00100000 - 0x00001000)			// offset=1MB-4k=0x000f_f000

	//========================================================================
	//	EB200P board
	//========================================================================

	//------------------------------------------------------------------------
	//	Board Register
	//------------------------------------------------------------------------
	#define BOARD_EB200P_REG__ID					0x00000000
	#define BOARD_EB200P_REG__REVISION				0x00000004
	#define BOARD_EB200P_REG__STATUS				0x00000008
	#define BOARD_EB200P_REG__CONTROL				0x00000020
	#define BOARD_EB200P_REG__SCRATCH				0x00000024
	#define BOARD_EB200P_REG__RESET					0x00000028
	#define BOARD_EB200P_REG__XHIF_CONFIG			0x00000030
	#define BOARD_EB200P_REG__INT_MASK				0x00000034
	#define BOARD_EB200P_REG__INT_STATUS_ACK		0x00000038
	#define BOARD_EB200P_REG__EMC_SDRAM_REFRESH		0x00000100
	#define BOARD_EB200P_REG__EMC_ASYNC_WAIT_CYCLE	0x00000104
	#define BOARD_EB200P_REG__EMC_PM				0x00000108
	#define BOARD_EB200P_REG__EMC_BF				0x0000010c
	#define BOARD_EB200P_REG__EMC_ASYNC_BANK0		0x00000110
	#define BOARD_EB200P_REG__EMC_ASYNC_BANK1		0x00000114
	#define BOARD_EB200P_REG__EMC_ASYNC_BANK2		0x00000118
	#define BOARD_EB200P_REG__EMC_ASYNC_BANK3		0x0000011c
	#define BOARD_EB200P_REG__EMC_LPEMR				0x00000120
	#define BOARD_EB200P_REG__EMC_EXTENDED			0x00000124
	#define BOARD_EB200P_REG__EMC_SDRAM_CONFIG		0x00000128
	#define BOARD_EB200P_REG__EMC_RECOVERY			0x0000012c

	//------------------------------------------------------------------------
	// Bits of BoardStatus register
	#define	BOARD_EB200P_STATUS_RESET		PNDEV_UINT32_SET_BIT_0
	#define	BOARD_EB200P_STATUS_PLL_LOC		PNDEV_UINT32_SET_BIT_1
	#define	BOARD_EB200P_STATUS_INT			PNDEV_UINT32_SET_BIT_2
	#define	BOARD_EB200P_STATUS_DMA_TIMEOUT	PNDEV_UINT32_SET_BIT_3				// PCIe access not finished by Asic -> Timeout
	#define	BOARD_EB200P_STATUS_MSI			PNDEV_UINT32_SET_BIT_4

	//------------------------------------------------------------------------
	// Bits of BoardControl register
	#define	BOARD_EB200P_CONTROL_EOI		PNDEV_UINT32_SET_BIT_1
	#define	BOARD_EB200P_CONTROL_INT_ENABLE	PNDEV_UINT32_SET_BIT_2
	#define	BOARD_EB200P_CONTROL_PAGE_CS_R	PNDEV_UINT32_SET_BIT_3

	//------------------------------------------------------------------------
	// Bits of IntMask register
	#define	BOARD_EB200P_INT_MASK_SET_DEMO_INT			PNDEV_UINT32_SET_BIT_0
	#define	BOARD_EB200P_INT_MASK_ENABLE_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_1
	#define	BOARD_EB200P_INT_MASK_ENABLE_GPIO_ERTEC200P	PNDEV_UINT32_SET_BIT_2
	#define	BOARD_EB200P_INT_MASK_ENABLE_DMA			PNDEV_UINT32_SET_BIT_3
	#define	BOARD_EB200P_INT_MASK_ENABLE_DMA_ERROR		PNDEV_UINT32_SET_BIT_4
	#define	BOARD_EB200P_INT_MASK_ENABLE_XHIF_HOST		PNDEV_UINT32_SET_BIT_5

	//------------------------------------------------------------------------
	// Bits of IntStatus/Ack register
	#define	BOARD_EB200P_INT_STATUS_ACK_DEMO_INT		PNDEV_UINT32_SET_BIT_0
	#define	BOARD_EB200P_INT_STATUS_ACK_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_1
	#define	BOARD_EB200P_INT_STATUS_ACK_GPIO			PNDEV_UINT32_SET_BIT_2
	#define	BOARD_EB200P_INT_STATUS_ACK_DMA				PNDEV_UINT32_SET_BIT_3
	#define	BOARD_EB200P_INT_STATUS_ACK_DMA_ERROR		PNDEV_UINT32_SET_BIT_4
	#define	BOARD_EB200P_INT_STATUS_ACK_XHIF_HOST		PNDEV_UINT32_SET_BIT_5

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 2x 16Bit NOR flash S29WS128P (2x 16MB):
	//	- used: 2x chip, 2MB
	//			UsableSize:	2MB
	//			SectorSize:	128kB
	//			MacAdr at last 4kB of last sector
	#define	BOARD_EB200P__FLASH_SIZE_USABLE	(1024*1024*2)
	#define	BOARD_EB200P__FLASH_SIZE_SECTOR	(1024*128)
	#define	BOARD_EB200P__FLASH_OFFSET_DATA	(0x00200000 - 0x00001000)			// page15, offset=2MB-4k=0x001f_f000

	//========================================================================
	//	FPGA1-Ertec400 board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//========================================================================
	//	FPGA1-Ertec200 board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR0_SIZE		(1024*4)
	#define	BOARD_FPGA1_ERTEC200__BAR1_SIZE		(1024*1024*8)
	#define	BOARD_FPGA1_ERTEC200__BAR2_SIZE		(1024*1024*16)
	#define	BOARD_FPGA1_ERTEC200__BAR3_SIZE		(1024*1024*64)
	#define	BOARD_FPGA1_ERTEC200__BAR4_SIZE		(1024*1024*1)
	#define	BOARD_FPGA1_ERTEC200__BAR5_SIZE		(1024*1024*1)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_ARM_ICU	0x00000000
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_IRTE		0x00500000
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_EMC_REG	0x00b00000
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_APB_PER	0x00c00000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR3_OFFSET_EMC_SDRAM	0x00000000

	#define	BOARD_FPGA1_ERTEC200__BAR3_SIZE_EMC_SDRAM	(1024*1024*64)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR4_OFFSET_EMC_ASYNC_BANK0	0x00000000

	#define	BOARD_FPGA1_ERTEC200__BAR4_SIZE_EMC_ASYNC_BANK0		(1024*1024*1)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR5_OFFSET_EMC_ASYNC_BANK1	0x00000000

	#define	BOARD_FPGA1_ERTEC200__BAR5_SIZE_EMC_ASYNC_BANK1		(1024*1024*1)

	//========================================================================
	//	FPGA1-Soc1 board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//========================================================================
	//	FPGA1-Ertec200P / EB200P board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_FPGA1_ERTEC200P__BAR0_SIZE				(1024*4)

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	#define	BOARD_ERTEC200P_XHIF__BAR1_SIZE			(1024*4)
	#define	BOARD_ERTEC200P_XHIF__BAR2_SIZE			(1024*1024*16)
	#define	BOARD_ERTEC200P_XHIF__BAR3_SIZE			0
	#define	BOARD_ERTEC200P_XHIF__BAR4_SIZE			0
	#define	BOARD_ERTEC200P_XHIF__BAR5_SIZE			0

	#define	BOARD_ERTEC200P_XHIF__PAGE_SIZE_1MB		(1024*1024*1)
	#define	BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB		(1024*1024*2)
	#define	BOARD_ERTEC200P_XHIF__PAGE_OFFSET_TTU	(1024*1024*1)

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE			(1024*1024*8)
	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE_ET200	(1024*1024*16)
	#define	BOARD_ERTEC200P_AHB__BAR2_SIZE			(1024*1024*16)
	#define	BOARD_ERTEC200P_AHB__BAR3_SIZE			(1024*1024*64)
	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE			(1024*1024*1)
	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE_ET200	(1024*1024*2)
	#define	BOARD_ERTEC200P_AHB__BAR5_SIZE			(1024*1024*1)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR1_OFFSET_EMC_ASYNC_BANK2		0x00000000
	#define	BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM			0x00400000
	#define	BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM_ET200		0x00800000

	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE_EMC_ASYNC_BANK2			(1024*1024*4)
	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE_EMC_ASYNC_BANK2_ET200	(1024*1024*8)

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	// each page can address a maximal window of 2MB
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE0	0
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE1	(1024*1024*2)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE2	(1024*1024*4)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE3	(1024*1024*6)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE4	(1024*1024*8)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE5	(1024*1024*10)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE6	(1024*1024*12)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE7	(1024*1024*14)

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_ARM_ICU	0x00000000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_PNIP		0x00500000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_PERIF		0x00700000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_GDMA		0x00800000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_TTU		0x00900000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_SD_MMC		0x00a00000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_EMC_REG	0x00b00000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_APB_PER	0x00c00000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR3_OFFSET_EMC_SDRAM	0x00000000

	#define	BOARD_ERTEC200P_AHB__BAR3_SIZE_EMC_SDRAM	(1024*1024*64)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK0		0x00000000
	#define	BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK3_ET200	0x00100000

	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE_EMC_ASYNC_BANK0			(1024*1024*1)
	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE_EMC_ASYNC_BANK3_ET200	(1024*1)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR5_OFFSET_EMC_ASYNC_BANK1	0x00000000

	#define	BOARD_ERTEC200P_AHB__BAR5_SIZE_EMC_ASYNC_BANK1		(1024*1024*1)

	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	#define BOARD_ERTEC200P_XHIF__SDRAM_PAGE_SHARED_RESERVED	0				// page0
	#define BOARD_ERTEC200P_XHIF__SDRAM_PAGE_SHARED_DIRECT		1				// page1

	//#define BOARD_ERTEC200P_XHIF__SDRAM_OFFSET_FW			(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0)	// jmp to this offset if semaphore=0x6879_9786
	//#define BOARD_ERTEC200P_XHIF__SDRAM_OFFSET_SEMAPHORE	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 4)
	//#define BOARD_ERTEC200P_XHIF__SDRAM_OFFSET_HANDSHAKE	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 8)	// value will be inverted if changed by user
	
	//========================================================================
	//	FPGA1-HERA board
	//========================================================================
	
	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	
	// Note:
	//	- only the bars with even number are use, because the bars have a width of 64bit	
	
	#define	BOARD_FPGA1_HERA__BAR0_SIZE				(1024*64)
	#define	BOARD_FPGA1_HERA__BAR1_SIZE				(1024*0)
	#define	BOARD_FPGA1_HERA__BAR2_SIZE				(1024*1024*16)
	#define	BOARD_FPGA1_HERA__BAR3_SIZE				(1024*0)
	#define	BOARD_FPGA1_HERA__BAR4_SIZE				(1024*1024*128)
	#define	BOARD_FPGA1_HERA__BAR5_SIZE				(1024*0)
	
	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_DDR3SDRAM			0x00000000
	#define	BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_KRISC_DDR3SDRAM	0x07F00000 + BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_DDR3SDRAM  // 127MB + 0KB

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_PNIP			0x00200000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R5_ATCM		0x00A00000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R5_BTCM		0x00A10000

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER		0x00000000
	
	//------------------------------------------------------------------------
	//	SSP (Serializer)  / GPIOs
	//------------------------------------------------------------------------

    #define BOARD_FPGA1_HERA__GPIO_IOCTRL_0          BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x4400      // Configuration register for General Purpose IOs (31:0) --> 0: GPIOx = output, 1: GPIOx = input
    #define BOARD_FPGA1_HERA__GPIO_OUT_0             BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x4404      // Output register for General Purpose IOs (31:0) --> 0: GPIO outputx = 0, 1: GPIO outputx = 1
    #define BOARD_FPGA1_HERA__GPIO_OUT_SET_0         BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x4408      // Bit-selective setting of the output register for General Purpose IOs (31:0) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 1 / Read always returns 0
    #define BOARD_FPGA1_HERA__GPIO_OUT_CLEAR_0       BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x440C      // Bit-selective reset of the output register for General Purpose IOs (31:0) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 0 / Read always returns 0
    #define BOARD_FPGA1_HERA__GPIO_RES_DIS_0         BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x4410      // Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0)
    #define BOARD_FPGA1_HERA__GPIO_IN_0              BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x4414      // Input register for General Purpose IOs (31:0)
    #define BOARD_FPGA1_HERA__GPIO_PORT_MODE_0_L     BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x4418      // Configuration register for GPIO port (15:0) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"
    #define BOARD_FPGA1_HERA__GPIO_PORT_MODE_0_H     BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0x441C      // Configuration register for GPIO port (31:16) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"

    #define BOARD_FPGA1_HERA__SSP_CR0                BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0xC000      // Control Register 0 [15:0]
    #define BOARD_FPGA1_HERA__SSP_CR1                BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0xC004      // Control Register 1 [15:0]
    #define BOARD_FPGA1_HERA__SSP_DR                 BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0xC008      // Data Register r/w  [15:0]
    #define BOARD_FPGA1_HERA__SSP_SR                 BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0xC00C      // Status Register    [4:0]
    #define BOARD_FPGA1_HERA__SSP_CPSR               BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER + 0xC010      // Clock prescale register r/w  [7:0]

    /* USER settings */
    #define BOARD_FPGA1_HERA__GPIO_PORT_MODE_0_L_SETTINGS       0x0054      // Reg GPIO_PORT_MODE_0_L -> GPIOs[3:1] alternate function A (SPI_1); GPIO[0] gpio function for CS

    #define BOARD_FPGA1_HERA__SSP_CR0_SETTINGS                  0x00003107  // serial clock rate 0x31 (=49), Motorola frame format, SPH/SPO = 0, data width 8 Bit
    #define BOARD_FPGA1_HERA__SSP_CPSR_SETTINGS                 0x0002      // Clock Prescale Devisor 0x2; results in a 500 kHz clock
    #define BOARD_FPGA1_HERA__SSP_TIMEOUT_MAX_COUNT             50          // wait max 50 milliseconds
    #define BOARD_FPGA1_HERA__SSP_DUMMY_SEND                    0x00        // dummy byte

    /* Serial stream structure */
    typedef struct _SSP_CHARSTREAM
    {
        UINT32 lLength;
        UINT8* pBuffer;

    }   SSP_CHARSTREAM;

    /* Acceptable values for SPI master side configuration */
    typedef enum _SSP_CONFIG_OPTION
    {
        SSP_OptionNull,  			    // do nothing
        SSP_OptionEnableChipSelect,	    // enable transmit
        SSP_OptionKeepUpChipSelect,     // another transmit is following (e.g. send command -> send data)
        SSP_OptionDisableChipSelect,    // no transmit is following

    }SSP_CONFIG_OPTION;

	//------------------------------------------------------------------------
	//	SPI FLASH
	//------------------------------------------------------------------------

    /* manufacturer id + mem type + mem capacity  */
    #define BOARD_FPGA1_HERA_SPI_FLASH__MEM_TYPE_N25Q128A	            0x20BB18// ID for N25Q128A device
    #define BOARD_FPGA1_HERA_SPI_FLASH__MEM_TYPE_N25Q128A_MANUFAC_ID	0x20    // ID for N25Q128A device
    #define BOARD_FPGA1_HERA_SPI_FLASH__MEM_TYPE_N25Q128A_MEMTYPE_ID	0xBB    // ID for N25Q128A device
    #define BOARD_FPGA1_HERA_SPI_FLASH__MEM_TYPE_N25Q128A_MEMSIZE_ID	0x18    // ID for N25Q128A device

    /* Flash shape */
    #define BOARD_FPGA1_HERA_SPI_FLASH__SIZE           	        0x1000000   // 128 MB
    #define BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_COUNT           	0x100       // 256 Sectors
    #define BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_SIZE           	0x10000     // 64 kB
    #define BOARD_FPGA1_HERA_SPI_FLASH__SUBSECTOR_COUNT         0x1000      // 4096 SubSectors
    #define BOARD_FPGA1_HERA_SPI_FLASH__SUBSECTOR_SIZE          0x1000      // 4 kB
    #define BOARD_FPGA1_HERA_SPI_FLASH__PAGE_COUNT              0x10000     // 65536 Pages
    #define BOARD_FPGA1_HERA_SPI_FLASH__PAGE_SIZE               0x100       // 256 Bytes
    #define BOARD_FPGA1_HERA_SPI_FLASH__OTP_SIZE                0x40        // 64 Bytes
    #define BOARD_FPGA1_HERA_SPI_FLASH__ADDRESS_MASK            0xFF
    
    /* USER settings */
    #define BOARD_FPGA1_HERA_SPI_FLASH__SIZE_USABLE             BOARD_FPGA1_HERA_SPI_FLASH__SIZE - BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_SIZE // 128 MB - 64kB using one sector for remanent data
    #define BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_DATA             255 // using last sector for remanent data
    #define BOARD_FPGA1_HERA_SPI_FLASH__OFFSET_DATA             BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_DATA * BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_SIZE // adress of sector for remanent data

    /************************************************************************************************************************/
    /* Command definitions (please see datasheet for more details) */	
    /* These are not all possible defines because a full implementation of the flash was not necessary */

	/* RESET Operations */
    #define BOARD_FPGA1_HERA_SPI_FLASH__INS_REN		            0x66	// reset enable
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RMEM                0x99	// reset memory

	/* IDENTIFICATION Operations */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDID                0x9F	// read Identification
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDID_ALT            0x9E	// read Identification
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_MULT_IO_RDID        0xAF    // read multiple i/o read id
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_DISCOVER_PARAMETER  0x5A    // read serial flash discovery parameter

	/* READ operations */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_READ                0x03	// read data bytes
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_FAST_READ           0x0B	// read data bytes at higher speed

	/* WRITE operations */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_WREN                0x06    // write enable
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_WRDI                0x04    // write disable

	/* REGISTER operations */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDSR                0x05    // read status register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_WRSR                0x01    // write status register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDLR                0xE8    // read lock register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_CMD_WRLR            0xE5    // write lock register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RFSR                0x70    // read flag status register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_CLFSR               0x50    // clear flag status register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDNVCR              0xB5    // read non volatile configuration register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_WRNVCR              0xB1    // write non volatile configuration register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDVCR               0x85    // read volatile configuration register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_WRVCR               0x81    // write volatile configuration register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDVECR              0x65    // read volatile enhanced configuration register
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_WRVECR              0x61    // write volatile enhanced configuration register

	/* PROGRAM operations */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_PP                  0x02    // PAGE PROGRAM

	/* ERASE operations */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_SSE                 0x20    // sub sector erase
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_SE                  0xD8    // sector erase
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_DE                  0xC4    // die erase
    #define BOARD_FPGA1_HERA_SPI_FLASH__INS_BE                  0xC7    // bulk erase
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_PER                 0x7A    // program Erase Resume
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_PES                 0x75    // program Erase Suspend

	/* OTP operations */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RDOTP               0x4B    // read OTP array
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_PROTP               0x42    // program OTP array

	/* DEEP POWER-DOWN operation */
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_ENTERDPD            0xB9    // enter deep power-down
	#define BOARD_FPGA1_HERA_SPI_FLASH__INS_RELEASEDPD          0xA8    // release deep power-down

    /************************************************************************************************************************/
    /* Register Definitions */
    
    /* Status Register */
    #define BOARD_FPGA1_HERA_SPI_FLASH__SR_SRWD                 0x80    // status Register Write Protect
	#define BOARD_FPGA1_HERA_SPI_FLASH__SR_BP3                  0x40    // block Protect Bit3
	#define BOARD_FPGA1_HERA_SPI_FLASH__SR_TB                   0x20    // top/Bottom bit
	#define BOARD_FPGA1_HERA_SPI_FLASH__SR_BP2                  0x10    // block Protect Bit2
	#define BOARD_FPGA1_HERA_SPI_FLASH__SR_BP1                  0x08    // block Protect Bit1
	#define BOARD_FPGA1_HERA_SPI_FLASH__SR_BP0                  0x04    // block Protect Bit0
	#define BOARD_FPGA1_HERA_SPI_FLASH__SR_WEL                  0x02    // write Enable Latch
	#define BOARD_FPGA1_HERA_SPI_FLASH__SR_WIP                  0x01    // write/Program/Erase in progress bit
    
    /* Flag Status Register */
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_PROG_ERASE_CTL      0x80    // StatusBit - 0=Busy,1=Ready - Indicates whether a PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NONVOLATILE CONFIGURATION command cycle is in progress
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_ERASE_SUSP          0x40    // StatusBit - 0=NotInEffect,1=InEffect - Indicates whether an ERASE operation has been or is going to be suspended
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_ERASE               0x20    // ErrorBit -  0=Clear,1=FailureOrProtectionError - Indicates whether an ERASE operation has succeeded or failed
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_PROGRAM             0x10    // ErrorBit -  0=Clear,1=FailureOrProtectionError - Indicates whether a PROGRAM operation has succeeded or failed
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_VPP                 0x08    // ErrorBit -  0=Enabled,1=Disabled(Default) - Indicates an invalid voltage on VPP during a PROGRAM or ERASE operation
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_PROG_SUSP           0x04    // StatusBit - 0=NotInEffect,1=InEffect - Indicates whether a PROGRAM operation has been or is going to be suspended
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_PROT                0x02    // ErrorBit -  0=Clear,1=FailureOrProtectionError - Indicates whether an ERASE or a PROGRAM operation has attempted to modify the protected array sector, or whether a PROGRAM operation has attempted to access the locked OTP space
	#define BOARD_FPGA1_HERA_SPI_FLASH__FSR_ADDR_MODE           0x01    // Reserved

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

#endif
