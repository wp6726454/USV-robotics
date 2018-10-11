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
/*  F i l e               &F: Board_Irte.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for boards with IRTE (Ertec200/400/Soc1)
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __Board_Irte_h__
#define __Board_Irte_h__

	//########################################################################
	//  Defines
	//########################################################################

	//========================================================================
	//	CP1616 board / CP1616-Eval board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR0_SIZE			(0)
	#define	BOARD_CP1616__BAR1_SIZE			(1024*64)
	#define	BOARD_CP1616__BAR2_SIZE			(1024*64)
	#define	BOARD_CP1616__BAR3_SIZE			(1024*1024*8)
	#define	BOARD_CP1616__BAR4_SIZE			(1024*1024*16)
	#define	BOARD_CP1616__BAR5_SIZE			(1024*1024*32)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR1_OFFSET_APB_PER	0x00000000

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR2_OFFSET_SRAM	0x00000000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR3_OFFSET_IRTE	0x00000000

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR4_OFFSET_EMIF_ASYNC_BANK0	0x00000000

	#define	BOARD_CP1616__BAR4_SIZE_EMIF_ASYNC_BANK0	(1024*1024*8)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR5_OFFSET_EMIF_SDRAM	0x00000000

	#define	BOARD_CP1616__BAR5_SIZE_EMIF_SDRAM		(1024*1024*32)

	//------------------------------------------------------------------------
	//	SRAM
	//------------------------------------------------------------------------
	#define BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL	((1024*8) - 8)			// command/signal of SBL: last 8 bytes of SRAM (0x1ff8)

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 1x 16Bit NOR flash AM29LV641/S29GL064N (1x 8MB):
	//	- used: 1x chip
	//			UsableSize:	8MB
	//			SectorSize:	64kB
	//			MacAdr at start of last sector
	#define	BOARD_CP1616__FLASH_SIZE_USABLE		(1024*1024*8)
	#define	BOARD_CP1616__FLASH_SIZE_SECTOR		(1024*64)
	#define BOARD_CP1616__FLASH_OFFSET_DATA		(0x00800000 - 0x00010000)		// offset=8MB-64k=0x007f_0000

	//========================================================================
	//	DB-EB400-PCIE board
	//========================================================================

	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
	#define		BOARD_EB400__SDRAM_BASE					(0x20000000) //EB400 integration z002p64u
	#define		BOARD_EB400__SDRAM_BOOT_FW				(0x20001000)

	#define		BOARD_EB400__SDRAM_OFFSET_ALIVE			(0x400)
	#define		BOARD_EB400__SDRAM_OFFSET_IDENT			(0x404)
	#define		BOARD_EB400__SDRAM_OFFSET_VERSION_MIN	(0x408)
	#define		BOARD_EB400__SDRAM_OFFSET_VERSION_MAJ	(0x40A)
	#define		BOARD_EB400__SDRAM_OFFSET_RES1			(0x40C)
	
	#define		BOARD_EB400__SDRAM_BIN_CMD				(0x420)
	#define		BOARD_EB400__SDRAM_BIN_FW				(0x424)
	#define		BOARD_EB400__SDRAM_BIN_STATUS			(0x428)
	#define		BOARD_EB400__SDRAM_BIN_EXECUTE	 		(0x00000002)


	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 1x 16Bit NOR flash S29GL064N (1x 8MB):
	//	- used: 1x chip
	//			UsableSize:	8MB
	//			SectorSize:	64kB
	//			MacAdr at start of last sector
	#define	BOARD_EB400__FLASH_SIZE_USABLE		(1024*1024*8)
	#define	BOARD_EB400__FLASH_SIZE_SECTOR		(1024*64)
	#define BOARD_EB400__FLASH_OFFSET_DATA		(0x00800000 - 0x00010000)		// offset=8MB-64k=0x007f_0000

	//========================================================================
	//	EB200 board
	//========================================================================

	typedef enum _eBOARD_EB200__LBU_DYN_PAGE
	{
		eBOARD_EB200__LBU_DYN_PAGE_INVALID			= 0,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_REG			= 1,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM		= 2,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK0	= 3,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK1	= 4,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK2	= 5,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK3	= 6
	}
	eBOARD_EB200__LBU_DYN_PAGE;	

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_EB200__BAR0_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR1_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR2_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR3_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR4_SIZE	(1024*64)
	#define	BOARD_EB200__BAR5_SIZE	(1024*64)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR0_OFFSET_LBU_PAGE0	0x00000000

	#define	BOARD_EB200__BAR0_SIZE_LBU_PAGE0	(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR1_OFFSET_LBU_PAGE1	0x00000000

	#define	BOARD_EB200__BAR1_SIZE_LBU_PAGE1	(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR2_OFFSET_LBU_PAGE2	0x00000000

	#define	BOARD_EB200__BAR2_SIZE_LBU_PAGE2	(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR3_OFFSET_LBU_PAGE3	0x00000000

	#define	BOARD_EB200__BAR3_SIZE_LBU_PAGE3	(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR4_OFFSET_LBU_REG	0x00000000

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR5_OFFSET_PCI_BRIDGE_REG	0x00000000

	//------------------------------------------------------------------------
	//	PCI-Bridge Register
	//------------------------------------------------------------------------
	#define BOARD_EB200_PCI_BRIDGE_REG__MASTER_OFFSET		0x00000000
	#define BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MAJOR	0x00000004
	#define BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MINOR	0x00000008			// introduced May 2013
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR0_CONS			0x00000010
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR1_CONS			0x00000014
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR2_CONS			0x00000018
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR3_CONS			0x0000001c

	#define BOARD_EB200_PCI_BRIDGE_REG__SIZE				0x20

	// Register BARx_CONS
	#define BOARD_EB200_PCI_BRIDGE_BARX_CONS__32BIT	0x00000001

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 1x 8Bit NOR flash AM29LV040B (1x 512kB):
	//	- used: 1x chip
	//			UsableSize:	512kB
	//			SectorSize:	64kB
	//			MacAdr at last 4kB of last sector
	#define	BOARD_EB200__FLASH_SIZE_USABLE			(1024*512)
	#define	BOARD_EB200__FLASH_SIZE_SECTOR			(1024*64)
	#define BOARD_EB200__FLASH_OFFSET_DATA			(0x00080000 - 0x00001000)	// page0, offset=512kB-4k=0x0007_f000
	
	#define BOARD_EB200__FLASH_OFFSET_FW_IDENT		0x00000024					// page0
	#define BOARD_EB200__FLASH_OFFSET_FW_VERSION	0x00000020					// page0

	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
#ifdef EB200_NEW_BOOTLOADER
	#define BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED		0						// page0
	#define BOARD_EB200__SDRAM_PAGE_SHARED_DIRECT		1						// page1

	#define BOARD_EB200__SDRAM_OFFSET_FW				(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0)	// jmp to this offset if semaphore=0x6879_9786
	#define BOARD_EB200__SDRAM_OFFSET_SEMAPHORE			(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 4)
	#define BOARD_EB200__SDRAM_OFFSET_ALIVE				(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 8)	// value will be inverted if changed by user
#else
	#define BOARD_EB200__SDRAM_PAGE_USER_FW				0						// page0, SWAP instruction can select SDRAM (CS) -> Arm9-Adr0 = Sdram-Adr0
	#define BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED		0						// page0
	#define BOARD_EB200__SDRAM_PAGE_SHARED_DIRECT		1						// page1

	#define BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED_TMP	31						// page31
	#define BOARD_EB200__SDRAM_OFFSET_SEMAPHORE			0x001ffff8
	#define BOARD_EB200__SDRAM_OFFSET_ALIVE				0x001ffffc				// value will be inverted if changed by user
	#define BOARD_EB200__SDRAM_OFFSET_FW				0x00000048				// jmp to this offset if semaphore=0x6879_9786
#endif

	//========================================================================
	//	DB-Soc1-PCI board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR0_SIZE_V2	(1024*8)
	#define	BOARD_SOC1_DEV__BAR1_SIZE_V2	(1024*1)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_V2	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR3_SIZE_V2	(1024*1024*8)
	#define	BOARD_SOC1_DEV__BAR4_SIZE_V2	(1024*1024*32)
	#define	BOARD_SOC1_DEV__BAR5_SIZE_V2	(1024*1024*64)

	#define	BOARD_SOC1_DEV__BAR0_SIZE_V3	(1024*8)
	#define	BOARD_SOC1_DEV__BAR1_SIZE_V3	(1024*1)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_V3	(1024*1024*64)
	#define	BOARD_SOC1_DEV__BAR3_SIZE_V3	(1024*1024*8)
	#define	BOARD_SOC1_DEV__BAR4_SIZE_V3	(1024*1024*32)
	#define	BOARD_SOC1_DEV__BAR5_SIZE_V3	(1024*1024*64)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR0_OFFSET_TPA_RAM	0x00000000

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR1_OFFSET_TRACE_TIMER_UNIT	0x00000000

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0	0x00000000
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS1	(BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0)
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS2	(BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS1)
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS3	(BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS1 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS2)

	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS1	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS2	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS3	(1024*1024*16)

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_LOCAL_BUS_REG	(SOC1_AHB_ADR__LOCAL_BUS_REG	- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_IRTE			(SOC1_AHB_ADR__IRTE				- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_PB_COM_RAM		(SOC1_AHB_ADR__PB_COM_RAM		- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_PCI_BRIDGE_REG	(SOC1_AHB_ADR__PCI_BRIDGE_REG	- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_IOCC			(SOC1_AHB_ADR__IOCC				- 0x1d000000)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_MMC_SD			(SOC1_AHB_ADR__MMC_SD		- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_GDMA			(SOC1_AHB_ADR__GDMA			- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_GICU			(SOC1_AHB_ADR__GICU			- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_PCI_ICU			(SOC1_AHB_ADR__PCI_ICU		- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_ECC_ICU			(SOC1_AHB_ADR__ECC_ICU		- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_DDR2C_REG		(SOC1_AHB_ADR__DDR2C_REG	- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_DSA_SOFT_PLL	(SOC1_AHB_ADR__DSA_SOFT_PLL	- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_APB_PER			(SOC1_AHB_ADR__APB_PER		- 0x1e000000)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR5_OFFSET_DDR2_SDRAM	0x00000000

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 2x 8Bit NAND flash K91G08U0B (2x 128MB):
	//	- used:	1x chip	
	//			UsableSize:		128MB
	//			BlockSize:		128kB
	//			NandPageSize:	2kB
	//			MacAdr at first good of last 8 reserved blocks (1MB)
	#define	BOARD_SOC1_DEV__FLASH_SIZE_USABLE		(1024*1024*128)
	#define	BOARD_SOC1_DEV__FLASH_SIZE_SECTOR		(1024*128)
	#define	BOARD_SOC1_DEV__FLASH_SIZE_PAGE			(1024*2)
	#define BOARD_SOC1_DEV__FLASH_OFFSET_DATA		(0x08000000 - 0x00100000)	// offset=128MB-(8*128k)=0x07f0_0000

	//------------------------------------------------------------------------
	#define BOARD_SOC1_DEV__FLASH_OFFSET_DATA_REG	0x00						// data
	#define BOARD_SOC1_DEV__FLASH_OFFSET_CMD_REG	0x40						// Command Register
	#define BOARD_SOC1_DEV__FLASH_OFFSET_ADR_REG	0x80						// Address Register

	//------------------------------------------------------------------------
	#define BOARD_SOC1_DEV__FLASH_SIZE_RSC_DATA		249		
	#define BOARD_SOC1_DEV__FLASH_SIZE_RSC_CHECK	6	

	//------------------------------------------------------------------------
	//	TPA-RAM
	//------------------------------------------------------------------------
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_SEMAPHORE		0x00000000
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_CODE				0x00000004

	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_TYPE		(0x17f0 + 0x00)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_MAJOR	(0x17f0 + 0x01)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_MINOR	(0x17f0 + 0x02)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_BUILD	(0x17f0 + 0x03)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_HW				(0x17f0 + 0x04)		// 16Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_BAR_CFG			(0x17f0 + 0x06)		// 16Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_PRODUCT			(0x17f0 + 0x08)		// 8Bit

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

#endif
