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
/*  F i l e               &F: PnDev_DriverU.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for user interface of PnDev_Driver
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __PnDev_DriverU_h__
#define __PnDev_DriverU_h__

	#ifdef __cplusplus		// if C++ compiler: Use C linkage
	extern "C"
	{
	#endif

	/*===========================================================================*/
	/*            Version according to "KM-Prozessbeschreibung"                  */
	/*===========================================================================*/

	/* "&T","&V","&D" and so on may be used by configuration management */

	#define PNDEVDRV_LSA_COMPONENT_ID		LSA_COMP_ID_PNDEVDRV
											/* see lsa_sys.h: lsa_component_id */

	#define PNDEVDRV_LSA_PREFIX				{'L','S','A','_','P','N','D','E','V','D','R','V','\0','\0','\0'}

	#define PNDEVDRV_KIND               	/* &K */ 'P'  /* K& */
											/* preliminary: 'R': release       */
											/*              'C': correction    */
											/*              'S': spezial       */
											/*              'T': test          */
											/*              'B': labor         */
											/* prereleased: 'P': pilot         */
											/* released:    'V': version       */
											/*              'K': correction    */
											/*              'D': demonstration */
	#define PNDEVDRV_VERSION            	/* &V */ 3    /* V& */ /* [1 - 99] */
	#define PNDEVDRV_DISTRIBUTION       	/* &D */ 0	  /* D& */ /* [0 - 99] */
	#define PNDEVDRV_FIX                	/* &F */ 0    /* F& */ /* [0 - 99] */
	#define PNDEVDRV_HOTFIX             	/* &H */ 0    /* H& */ /* [0]      */
	#define PNDEVDRV_PROJECT_NUMBER     	/* &P */ 0    /* P& */ /* [0 - 99] */
														  /* At LSA always 0!  */
	#define PNDEVDRV_INCREMENT				/* &I */ 1    /* I& */ /* [1 - 99] */
	#define PNDEVDRV_INTEGRATION_COUNTER 	/* &C */ 1    /* C& */ /* [1 - 99] */
	#define PNDEVDRV_GEN_COUNTER         	/* &G */ 1    /* G& */ /* [1]      */

	//########################################################################
	//  Defines
	//########################################################################

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	// The following ifdef block is the standard way of creating macros which make exporting
	// from a DLL simpler. All files within this DLL are compiled with the PNDEV_DRIVER_EXPORTS
	// symbol defined on the command line. this symbol should not be defined on any project
	// that uses this DLL. This way any other project whose source files include this file see
	// PNDEV_DRIVER_API functions as being imported from a DLL, whereas this DLL sees symbols
	// defined with this macro as being exported.
	#ifdef PNDEV_DRIVER_EXPORTS
		#define PNDEV_DRIVER_API __declspec(dllexport)
	#else
		#define PNDEV_DRIVER_API __declspec(dllimport)
	#endif

	//------------------------------------------------------------------------
	#else
	//------------------------------------------------------------------------
	#define PNDEV_DRIVER_API

	#endif

	//------------------------------------------------------------------------
	// const in "C" not available
	#define	PNDEV_SIZE_STRING_BUF			260									// size of buffer for Unicode characters inclusive terminal NULL (corresponding to MAX_PATH at WinDef.h)
																				// internal: same definition at SysDll.h
	#define	PNDEV_SIZE_STRING_BUF_SHORT		64									// internal: same definition at SysDll.h

	#define	PNDEV_DIM_ARRAY_DRIVER			16

	#define	PNDEV_DIM_ARRAY_COPY_DATA		4

	#define	PNDEV_DIM_ARRAY_SBL				4096

	#define	PNDEV_MAC_ADR_MAX				PNCORE_MAC_ADR_MAX
	#define	PNDEV_MAC_ADR_MIN__ERTEC400		(4+1)								// 4x Port + 1x Interface
	#define	PNDEV_MAC_ADR_MIN__ERTEC200		(2+1)								// 2x Port + 1x Interface
	#define	PNDEV_MAC_ADR_MIN__SOC1			(3+1)								// 3x Port + 1x Interface
	#define	PNDEV_MAC_ADR_MIN__ERTEC200P	(2+1)								// 2x Port + 1x Interface
    #define	PNDEV_MAC_ADR_MIN__HERA	        (4+2)								// 4x Port + 2x Interface

	#define	PNDEV_THREAD_PRIO_NORMAL		10
	#define	PNDEV_THREAD_PRIO_HIGH			20
	#define	PNDEV_THREAD_PRIO_TIME_CRITICAL	30
	#define	PNDEV_THREAD_PRIO_UNCHANGED		MAXUINT32

#if defined (PNDEV_OS_LINUX)
	#define PNDEV_THREAD_POLICY				SCHED_RR	// Scheduling policy for real time linux threads (default policy in adonis)
#else
	#define PNDEV_THREAD_POLICY				0			// PNDEV_THREAD_POLICY parameter is currently ignored in other operating systems
#endif

	#define	PNDEV_DIM_ARRAY_BAR			6

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_RESULT
	{
		ePNDEV_FATAL				= 0,
		ePNDEV_FATAL_BUF			= 1,
		ePNDEV_OK					= 2,
		ePNDEV_ERROR_NOT_FOUND		= 3,
		ePNDEV_ERROR_IN_USE			= 4,
		ePNDEV_ERROR_MASTER_APP		= 5,
		ePNDEV_ERROR_CBF			= 6,
		ePNDEV_ERROR_NO_SUPPORT		= 7
	}
	ePNDEV_RESULT;

	//------------------------------------------------------------------------
	// naming convention: <Board>__<Asic>										// Note: if TopLevel changed then AsicName must be changed!
	typedef enum _ePNDEV_BOARD
	{
		ePNDEV_BOARD_INVALID			= 0,
		ePNDEV_BOARD_CP1616				= 1,									// CP1616 board
		ePNDEV_BOARD_EB200				= 2,									// EB200 board
		ePNDEV_BOARD_DB_SOC1_PCI		= 3,									// DB-Soc1-PCI board
		ePNDEV_BOARD_DB_SOC1_PCIE		= 4,									// DB-Soc1-PCIe board
		ePNDEV_BOARD_FPGA1_ERTEC200P	= 5,									// FPGA1-Ertec200P board
		ePNDEV_BOARD_EB200P				= 6,									// EB200P board
		ePNDEV_BOARD_I210				= 7,									// Intel I210 board (formerly Springville)
		ePNDEV_BOARD_DB_EB400_PCIE		= 8,									// DB-EB400-PCIe board
		ePNDEV_BOARD_I82574				= 9,									// Intel I82574 board (formerly Hartwell)
		ePNDEV_BOARD_FPGA1_HERA			= 10,									// FPGA1-Hera board
		ePNDEV_BOARD_MICREL_KSZ8841		= 11,									// Micrel KSZ8841 board
		ePNDEV_BOARD_MICREL_KSZ8842		= 12,									// Micrel KSZ8842 board
		ePNDEV_BOARD_TI_OMAP			= 13									// TI Omap board
	}
	ePNDEV_BOARD;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_BOARD_DETAIL
	{
		ePNDEV_BOARD_DETAIL_INVALID					= 0,
		ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_EXT		= 1,
		ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_INT		= 2,
		ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_UNKNOWN	= 3,
		ePNDEV_BOARD_DETAIL_ERTEC200P_AHB			= 4,
		ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF			= 5
	}
	ePNDEV_BOARD_DETAIL;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_ASIC
	{
		ePNDEV_ASIC_INVALID		= 0,
		ePNDEV_ASIC_ERTEC400	= 1,
		ePNDEV_ASIC_ERTEC200	= 2,
		ePNDEV_ASIC_SOC1		= 3,
		ePNDEV_ASIC_ERTEC200P	= 4,
		ePNDEV_ASIC_I210		= 5,
		ePNDEV_ASIC_I82574		= 6,
		ePNDEV_ASIC_HERA		= 7,
		ePNDEV_ASIC_KSZ8841		= 8,
		ePNDEV_ASIC_KSZ8842		= 9,
		ePNDEV_ASIC_AM5728		= 10
	}
	ePNDEV_ASIC;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_ASIC_DETAIL
	{
		ePNDEV_ASIC_DETAIL_INVALID				= 0,
		ePNDEV_ASIC_DETAIL_IRTE_REV5			= 1,
		ePNDEV_ASIC_DETAIL_IRTE_REV5_METAL_FIX	= 2,
		ePNDEV_ASIC_DETAIL_IRTE_REV6			= 3,
		ePNDEV_ASIC_DETAIL_IRTE_REV6_METAL_FIX	= 4,
		ePNDEV_ASIC_DETAIL_IRTE_REV7			= 5,
		ePNDEV_ASIC_DETAIL_IRTE_REV7_METAL_FIX	= 6,
		ePNDEV_ASIC_DETAIL_PNIP_REV1			= 7,
		ePNDEV_ASIC_DETAIL_PNIP_REV2			= 8
	}
	ePNDEV_ASIC_DETAIL;
	
	//------------------------------------------------------------------------
	typedef enum _ePNDEV_EVENT_CLASS
	{
		ePNDEV_EVENT_CLASS_INVALID		= 0,
		ePNDEV_EVENT_CLASS_TEST			= 1,
		ePNDEV_EVENT_CLASS_FATAL_FW		= 2,
		ePNDEV_EVENT_CLASS_FATAL_ASYNC	= 3,
		ePNDEV_EVENT_CLASS_WARNING		= 4,
		ePNDEV_EVENT_CLASS_REQ_CLOSE	= 5,
		ePNDEV_EVENT_CLASS_DEBUG		= 6
	}
	ePNDEV_EVENT_CLASS;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_XHIF_PAGE
	{
		ePNDEV_XHIF_PAGE_INVALID					= 0,
		ePNDEV_XHIF_PAGE_ERTEC200P__SD_MMC			= 1,
		ePNDEV_XHIF_PAGE_ERTEC200P__ARM926_TCM		= 2,
		ePNDEV_XHIF_PAGE_ERTEC200P__EMC_SDRAM		= 3,
		ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK0	= 4,
		ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK1	= 5,
		ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK2	= 6,
		ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK3	= 7
	}
	ePNDEV_XHIF_PAGE;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_INT_PIN
	{
		ePNDEV_INT_PIN_INVALID	= 0,
		ePNDEV_INT_PIN_INTA		= 1,
		ePNDEV_INT_PIN_INTB		= 2,
		ePNDEV_INT_PIN_INTC		= 3,
		ePNDEV_INT_PIN_INTD		= 4
	}
	ePNDEV_INT_PIN;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_INT_ACTION
	{
		ePNDEV_INT_ACTION_INVALID							= 0,
		ePNDEV_INT_ACTION_UNMASK							= 1,
		ePNDEV_INT_ACTION_MASK_ALL							= 2,				// internal: must be last of IntStandard

		// for internal use:
		ePNDEV_INT_ACTION_TEST__ONCE_UISR					= 100,				// internal: must be first of IntTest
		ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_SYS			= 101,
		ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_THREAD_DLL	= 102,
		ePNDEV_INT_ACTION_TEST__CONT_UISR					= 103,
		ePNDEV_INT_ACTION_TEST__CONT_DPC					= 104,
		ePNDEV_INT_ACTION_TEST__CONT_ISR					= 105
	}
	ePNDEV_INT_ACTION;

	// for internal use:
	#define	PNDEV_INT_ACTION_STD_MAX	ePNDEV_INT_ACTION_MASK_ALL
	#define	PNDEV_INT_ACTION_TEST_MIN	ePNDEV_INT_ACTION_TEST__ONCE_UISR

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_INT_SRC
	{
		ePNDEV_INT_SRC_INVALID	= 0,

		//--------------------------------------------------------------------
		// Ertec200/400
		//--------------------------------------------------------------------
		ePNDEV_INT_SRC_ERTECX00__IRTE_IRQ1	= 1,

		//--------------------------------------------------------------------
		// Soc1/2
		//--------------------------------------------------------------------
		ePNDEV_INT_SRC_SOC__IRTE_IRQ1		= 2,
		ePNDEV_INT_SRC_SOC__TIMER0			= 3,
		ePNDEV_INT_SRC_SOC__TIMER1			= 4,
		ePNDEV_INT_SRC_SOC__TIMER2			= 5,
		ePNDEV_INT_SRC_SOC__TIMER3			= 6,
		ePNDEV_INT_SRC_SOC__TIMER4			= 7,
		ePNDEV_INT_SRC_SOC__TIMER5			= 8,
		ePNDEV_INT_SRC_SOC__GDMA			= 9,
		ePNDEV_INT_SRC_SOC__GPIO180			= 10,
		ePNDEV_INT_SRC_SOC__GPIO188			= 11,
		ePNDEV_INT_SRC_SOC__PB_PCI_F1		= 12,
		ePNDEV_INT_SRC_SOC__PB_PCI_F2		= 13,

		//--------------------------------------------------------------------
		// Ertec200P common
		//--------------------------------------------------------------------
		ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0		= 20,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ1		= 21,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2		= 22,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3		= 23,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4		= 24,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5		= 25,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6		= 26,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7		= 27,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8		= 28,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9		= 29,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10	= 30,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11	= 31,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12	= 32,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13	= 33,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14	= 34,
		ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15	= 35,

		//--------------------------------------------------------------------
		// Ertec200P PCIe-XHIF
		//--------------------------------------------------------------------
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA1		= 40,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA2		= 41,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA3		= 42,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA4		= 43,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA5		= 44,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA6		= 45,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA7		= 46,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA8		= 47,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA9		= 48,

		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT0		= 49,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT1		= 50,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT2		= 51,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT3		= 52,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT4		= 53,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT5		= 54,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT6		= 55,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT7		= 56,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT8		= 57,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT9		= 58,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT10		= 59,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT11		= 60,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT12		= 61,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT13		= 62,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT14		= 63,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT15		= 64,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT16		= 65,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT17		= 66,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT18		= 67,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT19		= 68,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT20		= 69,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT21		= 70,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT22		= 71,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT23		= 72,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT24		= 73,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT25		= 74,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT26		= 75,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT27		= 76,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT28		= 77,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT29		= 78,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT30		= 79,
		ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT31		= 80,

		ePNDEV_INT_SRC_ERTEC200P__ERR_AHB_HOSTIF		= 81,
		ePNDEV_INT_SRC_ERTEC200P__ERR_XHIF_HOSTIF		= 82,
		ePNDEV_INT_SRC_ERTEC200P__ERR_SPI_HOSTIF		= 83,
		ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_ADR_APP		= 84,
		ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_ADR_PNIP		= 85,
		ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_STATE_APP		= 86,
		ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_STATE_PNIP	= 87,

		//--------------------------------------------------------------------
		// Ertec200P PCIe-AHB
		//--------------------------------------------------------------------
		ePNDEV_INT_SRC_ERTEC200P__SPI1_COMBINED				= 90,
		ePNDEV_INT_SRC_ERTEC200P__SPI1_OVERRUN_ERROR		= 91,
		ePNDEV_INT_SRC_ERTEC200P__SPI2_COMBINED				= 92,
		ePNDEV_INT_SRC_ERTEC200P__SPI2_OVERRUN_ERROR		= 93,
		ePNDEV_INT_SRC_ERTEC200P__SPI12_PARITY_ERROR		= 94,
		ePNDEV_INT_SRC_ERTEC200P__SPI1_SEND_FIFO_EMPTY		= 95,
		ePNDEV_INT_SRC_ERTEC200P__SPI1_RCVE_FIFO_NOT_EMPTY	= 96,
		ePNDEV_INT_SRC_ERTEC200P__SPI2_SEND_FIFO_EMPTY		= 97,
		ePNDEV_INT_SRC_ERTEC200P__SPI2_RCVE_FIFO_NOT_EMPTY	= 98,

		ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO1					= 99,
		ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO2					= 100,

		ePNDEV_INT_SRC_ERTEC200P__PERIF						= 101,

		ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT9				= 102,
		ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT10				= 103,
		ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT11				= 104,
		ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT12				= 105,
		ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT13				= 106,
		ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT14				= 107,

		//--------------------------------------------------------------------
		// I210
		//--------------------------------------------------------------------
		/*ePNDEV_INT_SRC_I210__TXDW				= 108,
		ePNDEV_INT_SRC_I210__LSC 				= 109,
		ePNDEV_INT_SRC_I210__RXDMT0				= 110,
		ePNDEV_INT_SRC_I210__RX_MISS			= 111,
		ePNDEV_INT_SRC_I210__RXDW				= 112,
		ePNDEV_INT_SRC_I210__GPHY				= 113,
		ePNDEV_INT_SRC_I210__GPI_SDP0			= 114,
		ePNDEV_INT_SRC_I210__GPI_SDP1			= 115,
		ePNDEV_INT_SRC_I210__GPI_SDP2 			= 116,
		ePNDEV_INT_SRC_I210__GPI_SDP3			= 117,
		ePNDEV_INT_SRC_I210__MNG 				= 118,
		ePNDEV_INT_SRC_I210__TIME_SYNC			= 119,
		ePNDEV_INT_SRC_I210__FER				= 120,
		ePNDEV_INT_SRC_I210__PCI_EXEPTION		= 121,
		ePNDEV_INT_SRC_I210__SCE				= 122,
		ePNDEV_INT_SRC_I210__SOFTWARE_WD		= 123,
		ePNDEV_INT_SRC_I210__TCP_TIMER			= 124,
		ePNDEV_INT_SRC_I210__DRSTA				= 125,*/
		ePNDEV_INT_SRC_I210						= 108,
		ePNDEV_INT_SRC_I82574					= 109,
		ePNDEV_INT_SRC_MICREL_KSZ8841			= 110,
		ePNDEV_INT_SRC_MICREL_KSZ8842			= 111,

		//--------------------------------------------------------------------
		// IntTest
		//--------------------------------------------------------------------
		ePNDEV_INT_SRC_TEST	= 112												// must be last element!
	}
	ePNDEV_INT_SRC;

	#define	PNDEV_DIM_ARRAY_INT_SRC	(ePNDEV_INT_SRC_TEST + 1)

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_FW_SRC
	{
		ePNDEV_FW_SRC_INVALID	= 0,
		ePNDEV_FW_SRC_FILE		= 1,
		ePNDEV_FW_SRC_BUF		= 2
	}
	ePNDEV_FW_SRC;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_COPY_HW_RES
	{
		ePNDEV_COPY_HW_RES_INVALID		= 0,
		ePNDEV_COPY_HW_RES_SDRAM		= 1,
		ePNDEV_COPY_HW_RES_PNIP_IRTE	= 2
	}
	ePNDEV_COPY_HW_RES;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_FLASH_ACTION
	{
		ePNDEV_FLASH_ACTION_INVALID				= 0,
		ePNDEV_FLASH_ACTION_ERASE_CHIP			= 1,
		ePNDEV_FLASH_ACTION_ERASE_SECTOR_DATA	= 2,
		ePNDEV_FLASH_ACTION_PROGRAM_DATA		= 3,
		ePNDEV_FLASH_ACTION_PROGRAM_SBL			= 4
	}
	ePNDEV_FLASH_ACTION;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_EDGE
	{
		ePNDEV_EDGE_INVALID	= 0,
		ePNDEV_EDGE_RISING	= 1,
		ePNDEV_EDGE_FALLING	= 2
	}
	ePNDEV_EDGE;

	//------------------------------------------------------------------------
	// __stdcall:
	//	- the default calling convention in C# is __stdcall whereas in C or C++ it is __cdecl
	//	- the Microsoft .NET IDE does not provide a way to change the calling convention -> adaption of DLL
	typedef void	(__stdcall * PNDEV_CBF_EVENT)(		const HANDLE				hDeviceIn,
														const ePNDEV_EVENT_CLASS	eEventClassIn,
														PNDEV_CHAR*					sEventIn,
														const void*					pCbfParUserIn);
	typedef void	(__stdcall * PNDEV_CBF_ISR)(		const HANDLE				hDeviceIn,
														const BOOLEAN				bDeviceTestIn,
														const ePNDEV_INT_SRC		eIntSrcIn,
														const void*					pCbfParUserIn);
	typedef void	(__stdcall * PNDEV_CBF_NRT_SEND)(	const HANDLE				hDeviceIn,
														const void*					pNrtRqbIn,
														const void*					pCbfParUserIn);
	typedef void	(__stdcall * PNDEV_CBF_NRT_RCVE)(	const HANDLE				hDeviceIn,
														const void*					pNrtRqbIn,
														const void*					pCbfParUserIn,
														BOOLEAN*					pReturnRqbOut);

	//------------------------------------------------------------------------
	// example of an exported variable
	//extern PNDEV_DRIVER_API int lTest;

	//########################################################################
	//  Structures
	//########################################################################

	// Note:
	//	- ensure that driver and DLL use the same alignment!
//	#pragma pack(show)
	#pragma pack(push)															    // save current packing alignment
    //------------------------------------------------------------------------
    #if defined PNDEV_OS_WINDOWS || defined PNDEV_OS_ADONIS
    //------------------------------------------------------------------------
    #pragma pack(8)                                                           // set packing alignment 8 Byte
    //------------------------------------------------------------------------
    #elif defined PNDEV_OS_LINUX
    //------------------------------------------------------------------------
    #pragma pack(4)                                                           // set packing alignment 4 Byte
	//------------------------------------------------------------------------
    #else 
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    #endif

//	#pragma pack(show)

	//========================================================================
	//	common
	//========================================================================

	typedef	union _uPNDEV64_HANDLE
	{
		HANDLE	hHandle;
		UINT64	lUint64;
	}
	uPNDEV64_HANDLE;

	//------------------------------------------------------------------------
	typedef	union _uPNDEV64_PTR_VOID
	{
		void*	pPtr;
		UINT64	lUint64;
	}
	uPNDEV64_PTR_VOID;

	//------------------------------------------------------------------------
	typedef	union _uPNDEV64_PTR_UINT8
	{
		volatile UINT8*	pPtr;
		UINT64			lUint64;
	}
	uPNDEV64_PTR_UINT8;

	//------------------------------------------------------------------------
	typedef	union _uPNDEV64_CBF_EVENT
	{
		PNDEV_CBF_EVENT	pPtr;
		UINT64			lUint64;
	}
	uPNDEV64_CBF_EVENT;

	//------------------------------------------------------------------------
	typedef	union _uPNDEV64_CBF_ISR
	{
		PNDEV_CBF_ISR	pPtr;
		UINT64			lUint64;
	}
	uPNDEV64_CBF_ISR;

	//------------------------------------------------------------------------
	typedef	union _uPNDEV64_CBF_NRT_SEND
	{
		PNDEV_CBF_NRT_SEND	pPtr;
		UINT64				lUint64;
	}
	uPNDEV64_CBF_NRT_SEND;

	//------------------------------------------------------------------------
	typedef	union _uPNDEV64_CBF_NRT_RCVE
	{
		PNDEV_CBF_NRT_RCVE	pPtr;
		UINT64				lUint64;
	}
	uPNDEV64_CBF_NRT_RCVE;

	//========================================================================
	//	service GetDeviceInfo
	//========================================================================

	typedef struct _uPNDEV_DRIVER_INFO
	{
		UINT32		lCtrDriver;
		UINT32		lIndexCurDriver;
		PNDEV_CHAR	sArrayDriver[PNDEV_DIM_ARRAY_DRIVER][PNDEV_SIZE_STRING_BUF_SHORT];
	}
	uPNDEV_DRIVER_INFO;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_GET_DEVICE_INFO_IN
	{
		uPNDEV64_HANDLE uListBoxCurAction;
	}
	uPNDEV_GET_DEVICE_INFO_IN;
	//------------------------------------------------------------------------
	typedef struct _uPNDEV_GET_DEVICE_INFO_OUT
	{
		ePNDEV_BOARD		eBoard;
		PNDEV_CHAR			sPciLoc   [PNDEV_SIZE_STRING_BUF_SHORT];			// e.g. "PCI bus 5, device 10, function 0"
		PNDEV_CHAR			sClassName[PNDEV_SIZE_STRING_BUF_SHORT];			// e.g. "SIMATIC NET"
		BOOLEAN				bInUse;
		uPNDEV_DRIVER_INFO	uDriverInfo;
	}
	uPNDEV_GET_DEVICE_INFO_OUT;

	//========================================================================
	//	service OpenDevice
	//========================================================================

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_RES_SINGLE_UM
	{
		uPNDEV64_PTR_UINT8	uAdrUser;
		UINT32				lSize;
		uPNDEV64_PTR_VOID	uMdl;
	}
	uPNDEV_HW_RES_SINGLE_UM;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_RES_SINGLE_KM_TMP
	{
		UINT32              lSize;
		uPNDEV64_PTR_UINT8	uVirtualAdr;
		INT64				lPhyAdr;
	}
	uPNDEV_HW_RES_SINGLE_KM_TMP;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_THREAD_DLL
	{
		UINT32		lPrio;
		UINT32		lStackSize;
		PNDEV_CHAR	sName[PNDEV_SIZE_STRING_BUF_SHORT];
	}
	uPNDEV_THREAD_DLL;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_THREAD_SYS
	{
		UINT32		lPrio;
		UINT32		lStackSize;
		PNDEV_CHAR	sName[PNDEV_SIZE_STRING_BUF_SHORT];
		UINT32		lTimeCycle_10msec;
	}
	uPNDEV_THREAD_SYS;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_FW_DESC
	{
		ePNDEV_FW_SRC eSrc;

		union
		{
			struct
			{
				PNDEV_CHAR sPath[PNDEV_SIZE_STRING_BUF];
			}
			uFile;

			struct
			{
				uPNDEV64_PTR_VOID	uBase;
				UINT32				lSize;
			}
			uBuf;
		}
		as;
	}
	uPNDEV_FW_DESC;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_USER_FW
	{
		UINT32	lCtrFw;
		BOOLEAN	bDebugMode;
		BOOLEAN bShmDebug;
		BOOLEAN bShmHaltOnStartup;

		UINT32	lTimeoutFwLoad_10msec;
		UINT32	lTimeoutFwExe_10msec;
		UINT32	lTimeoutFwStartupStd_10msec;
		UINT32	lTimeoutFwStartupDebug_10msec;
		UINT32	lTimeoutDebugger_10msec;
		UINT32	lTimeWaitDebugger_10msec;
		UINT32	lTimeStartupSignOfLife_10msec;
		UINT32	lTimeUpdateSignOfLife_10msec;
		UINT32	lBatHostAddress;

		UINT32	lArrayUserData[PNCORE_DIM_ARRAY_USER_DATA];
		uPNDEV_FW_DESC uFw[2];
	}
	uPNDEV_USER_FW;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_THREAD_IOCTL
	{
		uPNDEV64_HANDLE	uEventUinfo;
		uPNDEV64_HANDLE	uEventUinfoUm;
		uPNDEV64_HANDLE	uEventUisr;
		uPNDEV64_HANDLE	uThreadDll;
	}
	uPNDEV_THREAD_IOCTL;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_CONFIG_SPACE_EXTENDED
	{
		UINT32	lLinkWidth;
		UINT32	lMaxPayloadSize;
		BOOLEAN	bCorrectableErrorDetected;
		BOOLEAN bNonFatalErrorDetected;
		BOOLEAN bFatalErrorDetected;
		BOOLEAN bUnsupportedRequestDetected;
		BOOLEAN bAuxPowerDetected;
		BOOLEAN bTransactionsPending;
	}
	uPNDEV_CONFIG_SPACE_EXTENDED;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_CONFIG_SPACE
	{
		UINT32							lIntLine;								// 0->IRQ0, 1->IRQ1, 2->IRQ2,...
		ePNDEV_INT_PIN					lIntPin;
		BOOLEAN							bExtendedExist;
		uPNDEV_CONFIG_SPACE_EXTENDED	uExtended;
	}
	uPNDEV_CONFIG_SPACE;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_THREAD_INFO
	{
		UINT32	lIdDll;
		UINT32	lIdSys;
	}
	uPNDEV_THREAD_INFO;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_SBL_INFO
	{
		UINT32	lProduct;
		UINT32	lHw;
		UINT32	lSblDetail;

		struct
		{
			UINT32	lMajor;
			UINT32	lMinor;
			UINT32	lBuild;
		}
		uVersion;
	}
	uPNDEV_SBL_INFO;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_INFO
	{
		ePNDEV_BOARD			eBoard;
		PNDEV_CHAR				sBoardInfo[PNDEV_SIZE_STRING_BUF_SHORT];
		ePNDEV_BOARD_DETAIL		eBoardDetail;
		ePNDEV_ASIC				eAsic;
		PNDEV_CHAR				sAsicInfo[PNDEV_SIZE_STRING_BUF_SHORT];
		ePNDEV_ASIC_DETAIL		eAsicDetail;
		BOOLEAN					bAsicCoreExist;
		uPNDEV_CONFIG_SPACE		uConfigSpace;
	}
	uPNDEV_HW_INFO;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC
	{
		uPNDEV64_PTR_UINT8	uBase;
		UINT32				lOffset;
		UINT32				lSize;
		UINT32				lAdrAhb;
	}
	uPNDEV_HW_DESC;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_INDIRECT
	{
		UINT32	lOffset;
		UINT32	lSize;
		UINT32	lAdrAhb;
	}
	uPNDEV_HW_DESC_INDIRECT;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_SDRAM
	{
		UINT32					lSizePhy;
		UINT32					lSizeDriver;
		uPNDEV_HW_DESC			uDirect;
		uPNDEV_HW_DESC_INDIRECT	uIndirect[2];
		uPNDEV_HW_DESC			uFwHif;
		uPNDEV_HW_DESC_INDIRECT	uFwFatal;
		uPNDEV_HW_DESC_INDIRECT	uFwTrace[2];
	}
	uPNDEV_HW_DESC_SDRAM;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_APB_PER_400
	{
		uPNDEV_HW_DESC	uTimer0_1;
		uPNDEV_HW_DESC	uWatchdog;
		uPNDEV_HW_DESC	uSpi;
		uPNDEV_HW_DESC	uUart1;
		uPNDEV_HW_DESC	uUart2;
		uPNDEV_HW_DESC	uGpio;
		uPNDEV_HW_DESC	uScrb;
	}
	uPNDEV_HW_DESC_APB_PER_400;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_APB_PER_200
	{
		uPNDEV_HW_DESC	uTimer0_2;
		uPNDEV_HW_DESC	uWatchdog;
		uPNDEV_HW_DESC	uSpi;
		uPNDEV_HW_DESC	uUart;
		uPNDEV_HW_DESC	uGpio;
		uPNDEV_HW_DESC	uScrb;
	}
	uPNDEV_HW_DESC_APB_PER_200;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_APB_PER_SOC1
	{
		uPNDEV_HW_DESC	uI2c;
		uPNDEV_HW_DESC	uPbKriscReg;
		uPNDEV_HW_DESC	uUart1;
		uPNDEV_HW_DESC	uUart2;
		uPNDEV_HW_DESC	uUart3;
		uPNDEV_HW_DESC	uSpi;
		uPNDEV_HW_DESC	uWatchdog;
		uPNDEV_HW_DESC	uSemaphore;
		uPNDEV_HW_DESC	uTimer;
		uPNDEV_HW_DESC	uGpio;
		uPNDEV_HW_DESC	uScrb;
		uPNDEV_HW_DESC	uBootRom;
		uPNDEV_HW_DESC	uSps;
	}
	uPNDEV_HW_DESC_APB_PER_SOC1;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_APB_PER_200P
	{
		uPNDEV_HW_DESC	uPerIf;
		uPNDEV_HW_DESC	uUart1;
		uPNDEV_HW_DESC	uUart2;
		uPNDEV_HW_DESC	uUart3;
		uPNDEV_HW_DESC	uUart4;
		uPNDEV_HW_DESC	uI2c;
		uPNDEV_HW_DESC	uScrb;
		uPNDEV_HW_DESC	uSpi1;
		uPNDEV_HW_DESC	uSpi2;
		uPNDEV_HW_DESC	uTimer0_5;
		uPNDEV_HW_DESC	uWatchdog;
		uPNDEV_HW_DESC	uGpio;
	}
	uPNDEV_HW_DESC_APB_PER_200P;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_APB_PER_HERA
	{
		uPNDEV_HW_DESC	uGpio;
		uPNDEV_HW_DESC	uScrb;
		uPNDEV_HW_DESC	uSpi1;
	}
	uPNDEV_HW_DESC_APB_PER_HERA;
	//------------------------------------------------------------------------
	
	typedef	struct _uPNDEV_HW_DESC_APB_PER_OMAP
	{
		uPNDEV_HW_DESC	uGpio;
		uPNDEV_HW_DESC	uI2C1;
		uPNDEV_HW_DESC	uI2C2;
		uPNDEV_HW_DESC	uI2C3;
		uPNDEV_HW_DESC	uUart;
	}
	uPNDEV_HW_DESC_APB_PER_OMAP;
	//------------------------------------------------------------------------

	typedef	struct _uPNDEV_SHARED_HOST_SDRAM
	{
		uPNDEV64_PTR_UINT8	uBase;
		UINT32				lSize;
		UINT32				lPhyAdr;
	}
	uPNDEV_SHARED_HOST_SDRAM;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_RES
	{
		struct
		{
			uPNCORE_REMANENT_DATA uRemanentData;
		}
		uCommon;

		union
		{
			struct
			{
				uPNDEV_HW_DESC				uIrte;
				uPNDEV_HW_DESC_SDRAM		uEmifSdram;
				uPNDEV_HW_DESC				uEmifAsyncBank0;
				uPNDEV_HW_DESC				uEmifAsyncBank1;
				uPNDEV_HW_DESC				uEmifAsyncBank2;
				uPNDEV_HW_DESC				uEmifAsyncBank3;
				uPNDEV_HW_DESC_APB_PER_400	uApbPer;
				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
				uPNCORE_PCI_HOST_PAR		uPciMaster[3];
			}
			uErtec400;

			struct
			{
				uPNDEV_HW_DESC				uIrte;
				uPNDEV_HW_DESC_SDRAM		uEmifSdram;
				uPNDEV_HW_DESC				uEmifAsyncBank0;
				uPNDEV_HW_DESC				uEmifAsyncBank1;
				uPNDEV_HW_DESC				uEmifAsyncBank2;
				uPNDEV_HW_DESC				uEmifAsyncBank3;
				uPNDEV_HW_DESC_APB_PER_200	uApbPer;
			}
			uErtec200;

			struct
			{
				uPNDEV_HW_DESC				uTpaRam;
				uPNDEV_HW_DESC				uTtu;
				uPNDEV_HW_DESC				uLocalBus0;
				uPNDEV_HW_DESC				uLocalBus1;
				uPNDEV_HW_DESC				uLocalBus2;
				uPNDEV_HW_DESC				uLocalBus3;
				uPNDEV_HW_DESC				uLocalBus4;
				uPNDEV_HW_DESC				uLocalBusReg;
				uPNDEV_HW_DESC				uIrte;
				uPNDEV_HW_DESC				uPbComRam;
				uPNDEV_HW_DESC				uIocc;
				uPNDEV_HW_DESC				uMmcSd;
				uPNDEV_HW_DESC				uGdma;
				// RQ
				uPNDEV_HW_DESC				uGicu;
				uPNDEV_HW_DESC				uDsaSoftPll;
				uPNDEV_HW_DESC_APB_PER_SOC1	uApbPer;
				uPNDEV_HW_DESC_SDRAM		uDdr2Sdram;
				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
				uPNCORE_PCI_HOST_PAR		uPciMaster[3];
			}
			uSoc1;

			struct
			{
				uPNDEV_HW_DESC				uKrisc32Tcm;
				uPNDEV_HW_DESC				uArm926Tcm;
				uPNDEV_HW_DESC				uPnIp;
				uPNDEV_HW_DESC				uPerIf;
				uPNDEV_HW_DESC				uGdma;
				uPNDEV_HW_DESC				uTtu;
				uPNDEV_HW_DESC				uSdMmc;
				uPNDEV_HW_DESC_SDRAM		uEmcSdram;
				uPNDEV_HW_DESC				uEmcAsyncBank0;
				uPNDEV_HW_DESC				uEmcAsyncBank1;
				uPNDEV_HW_DESC				uEmcAsyncBank2;
				uPNDEV_HW_DESC				uEmcAsyncBank3;
				uPNDEV_HW_DESC_APB_PER_200P	uApbPer;
			}
			uErtec200P;

			struct
			{
				uPNDEV_HW_DESC	uBar0;
				uPNDEV_HW_DESC	uBar3;

				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
			}
			uI210;

			struct
			{
				uPNDEV_HW_DESC	uBar0;
				uPNDEV_HW_DESC	uBar1;
				uPNDEV_HW_DESC	uBar3;

				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
			}
			uI82574;
			
			struct
			{
				uPNDEV_HW_DESC_SDRAM		uDdr3Sdram;
				uPNDEV_HW_DESC				uKriscDdr3Sdram;
				uPNDEV_HW_DESC				uR5Atcm;
				uPNDEV_HW_DESC				uR5Btcm;
				uPNDEV_HW_DESC				uPnIp;
				uPNDEV_HW_DESC_APB_PER_HERA	uApbPer;
				
				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
			}
			uHera;
			
			struct
			{
				uPNDEV_HW_DESC	uBar0;

				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
			}
			uKSZ8841;
			
			struct
			{
				uPNDEV_HW_DESC	uBar0;

				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
			}
			uKSZ8842;
			
			struct
			{
				uPNDEV_HW_DESC			uL4Per1;
				uPNDEV_HW_DESC			uUart3;
				uPNDEV_HW_DESC			uTimer;
				uPNDEV_HW_DESC			uGmacSw;
				uPNDEV_HW_DESC			uCtrlModuleCore;
				uPNDEV_HW_DESC			uCmCoreAon;
				uPNDEV_HW_DESC			uCmCore;
				uPNDEV_HW_DESC			uDmaSystem;
				uPNDEV_HW_DESC			uEdma;
				uPNDEV_HW_DESC			uPruss1Config;
				uPNDEV_HW_DESC			uPruss1;
				uPNDEV_HW_DESC_SDRAM	uEmifSdram;
				uPNDEV_HW_DESC			uQSpi;
				
				uPNDEV_HW_DESC_APB_PER_OMAP	uApbPer;			// Gpio, Uart, I2C
				
				uPNDEV_SHARED_HOST_SDRAM	uSharedHostSdram;
			}
			uOmap;
		}
		as;
	}
	uPNDEV_HW_RES;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CORE_IF_OUT
	{
		uPNCORE_INT_PAR	uIntPar_IcuHost;
		uPNCORE_INT_PAR	uIntPar_IcuAsic;
	}
	uPNDEV_CORE_IF_OUT;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_OPEN_DEVICE_INTERNAL_IN
	{
		uPNDEV_THREAD_IOCTL uThreadIoctl;
	}
	uPNDEV_OPEN_DEVICE_INTERNAL_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_OPEN_DEVICE_INTERNAL_OUT
	{
		uPNDEV64_HANDLE					uAppSys;
		BOOLEAN							bFirstOpenDevice;
		UINT32							eIcu;
		uPNDEV64_PTR_UINT8				uIcu;

		uPNDEV_HW_RES_SINGLE_KM_TMP		uBarKm[6];
		uPNDEV_HW_RES_SINGLE_KM_TMP		uSdramKm;
		uPNDEV_HW_RES_SINGLE_KM_TMP		uHostMemKm;
		uPNDEV_HW_RES_SINGLE_UM			uBarUm[6];
		uPNDEV_HW_RES_SINGLE_UM			uSdramUm;
		uPNDEV_HW_RES_SINGLE_UM     	uHostMemUm;
	}
	uPNDEV_OPEN_DEVICE_INTERNAL_OUT;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_OPEN_DEVICE_IN
	{
		PNDEV_CHAR			sPciLoc[PNDEV_SIZE_STRING_BUF_SHORT];
		BOOLEAN				bMasterApp;
		BOOLEAN				bInitPhy;
		uPNDEV64_CBF_EVENT	uCbfEvent;
		uPNDEV64_CBF_ISR	uCbfIsr;
		uPNDEV64_PTR_VOID	uCbfParUser;
		uPNDEV_THREAD_DLL	uThreadDll;
		uPNDEV_THREAD_SYS	uThreadSys;
		uPNDEV_FW_DESC		uBootFw;
		uPNDEV_USER_FW		uUserFw;
		UINT32				lTimeScaleFactor;
		BOOLEAN				bVerifyEnvironment;
        BOOLEAN				bReqHostMem;
		uPNDEV64_HANDLE		uListBoxCurAction;

		uPNDEV_OPEN_DEVICE_INTERNAL_IN uInternal;
	}
	uPNDEV_OPEN_DEVICE_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_OPEN_DEVICE_OUT
	{
		PNDEV_CHAR				sError[PNDEV_SIZE_STRING_BUF];
		uPNDEV64_HANDLE			uDevice;
		PNDEV_CHAR				sPciLoc     [PNDEV_SIZE_STRING_BUF_SHORT];		// returned InputParameter
		PNDEV_CHAR				sPciLocShort[PNDEV_SIZE_STRING_BUF_SHORT];
		BOOLEAN					bMasterApp;										// returned InputParameter
		UINT32					lCtrFw;											// returned InputParameter
		uPNDEV_THREAD_INFO		uThreadInfo;
		uPNDEV_SBL_INFO			uSblInfo;
		uPNDEV_HW_INFO			uHwInfo;
		uPNDEV_HW_RES			uHwRes;
		uPNDEV_CORE_IF_OUT		uCoreIf;

		uPNDEV_OPEN_DEVICE_INTERNAL_OUT	uInternal;
	}
	uPNDEV_OPEN_DEVICE_OUT;

	//========================================================================
	//	service ChangeInt
	//========================================================================

	typedef struct _uPNDEV_INT_POLARITY
	{
		union
		{
			struct
			{
				ePNDEV_EDGE	eGpio180;
				ePNDEV_EDGE	eGpio188;
				BOOLEAN		bEnablePullGpio180;
				BOOLEAN		bEnablePullGpio188;
			}
			uSoc1;

			struct
			{
				UINT32 lDummy;
			}
			uErtec200P;
		}
		as;
	}
	uPNDEV_INT_POLARITY;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CHANGE_INT_INTERNAL_IN
	{
		uPNDEV64_HANDLE uAppSys;
	}
	uPNDEV_CHANGE_INT_INTERNAL_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CHANGE_INT_IN
	{
		uPNDEV64_HANDLE		uDevice;
		ePNDEV_INT_ACTION	eIntAction;
		BOOLEAN				bArrayIntUnmask[PNDEV_DIM_ARRAY_INT_SRC];
		BOOLEAN				bArrayIntAck   [PNDEV_DIM_ARRAY_INT_SRC];
		UINT32				lPrioThread;
		uPNDEV_INT_POLARITY	uIntPolarity;

		uPNDEV_CHANGE_INT_INTERNAL_IN uInternal;
	}
	uPNDEV_CHANGE_INT_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CHANGE_INT_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_CHANGE_INT_OUT;

	//========================================================================
	//	service GetDeviceState
	//========================================================================

	typedef struct _uPNDEV_GET_DEVICE_STATE_INTERNAL_IN
	{
		uPNDEV64_HANDLE uAppSys;
	}
	uPNDEV_GET_DEVICE_STATE_INTERNAL_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_GET_DEVICE_STATE_IN
	{
		uPNDEV64_HANDLE uDevice;

		uPNDEV_GET_DEVICE_STATE_INTERNAL_IN uInternal;
	}
	uPNDEV_GET_DEVICE_STATE_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_GET_DEVICE_STATE_OUT
	{
		PNDEV_CHAR	sError[PNDEV_SIZE_STRING_BUF];
		BOOLEAN		bMasterApp;
		PNDEV_CHAR	sPciLoc[PNDEV_SIZE_STRING_BUF_SHORT];
		UINT32		lCtrApp;
		UINT32		lCtrRef_XhifPage;
		BOOLEAN		bUserFwRunning;

		struct
		{
			UINT32	lCtrUisr;
			UINT32	lCtrDpc;
			UINT32	lCtrIsr;
		}
		uIntStatistic;
	}
	uPNDEV_GET_DEVICE_STATE_OUT;

	//========================================================================
	//	service ProgramFlash
	//========================================================================

	typedef	struct _uPNDEV_REMANENT_DATA_WRITE
	{
		UINT32	lCtrMacAdr;
		UINT64	lMacAdr64;
		UINT8	lArrayPrivate[PNCORE_DIM_ARRAY_REMANENT_PRIVATE];
	}
	uPNDEV_REMANENT_DATA_WRITE;

	//------------------------------------------------------------------------
	typedef union _uPNDEV_PROGRAM_DATA_SRC
	{
		uPNDEV_REMANENT_DATA_WRITE uRemanentData;

		struct
		{
			ePNDEV_FW_SRC eSrc;

			struct
			{
				PNDEV_CHAR sPath[PNDEV_SIZE_STRING_BUF];
			}
			uFile;

			UINT8 lArrayFw[PNDEV_DIM_ARRAY_SBL];
		}
		uSbl;
	}
	uPNDEV_PROGRAM_DATA_SRC;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_PROGRAM_FLASH_INTERNAL_IN
	{
		uPNDEV64_HANDLE uAppSys;
	}
	uPNDEV_PROGRAM_FLASH_INTERNAL_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_PROGRAM_FLASH_IN
	{
		uPNDEV64_HANDLE			uDevice;
		ePNDEV_FLASH_ACTION		eFlashAction;
		uPNDEV_PROGRAM_DATA_SRC	as;

		uPNDEV_PROGRAM_FLASH_INTERNAL_IN uInternal;
	}
	uPNDEV_PROGRAM_FLASH_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_PROGRAM_FLASH_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_PROGRAM_FLASH_OUT;

	//========================================================================
	//	service ChangeXhifPage
	//========================================================================

	typedef struct _uPNDEV_CHANGE_XHIF_PAGE_INTERNAL_IN
	{
		uPNDEV64_HANDLE uAppSys;
	}
	uPNDEV_CHANGE_XHIF_PAGE_INTERNAL_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CHANGE_XHIF_PAGE_IN
	{
		uPNDEV64_HANDLE		uDevice;
		ePNDEV_XHIF_PAGE	ePage;
		UINT32				lSegmentAhb;

		uPNDEV_CHANGE_XHIF_PAGE_INTERNAL_IN uInternal;
	}
	uPNDEV_CHANGE_XHIF_PAGE_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CHANGE_XHIF_PAGE_OUT
	{
		PNDEV_CHAR		sError[PNDEV_SIZE_STRING_BUF];
		uPNDEV_HW_DESC	uHwDesc;
		UINT32			lCtrRef;
	}
	uPNDEV_CHANGE_XHIF_PAGE_OUT;

	//========================================================================
	//	service ComparePciLoc
	//========================================================================

	typedef struct _uPNDEV_COMPARE_PCI_LOC_IN
	{
		UINT32		lPciBusNo;
		UINT32		lPciDeviceNo;
		UINT32		lPciFunctionNo;
		PNDEV_CHAR	sPciLoc[PNDEV_SIZE_STRING_BUF_SHORT];
	}
	uPNDEV_COMPARE_PCI_LOC_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_COMPARE_PCI_LOC_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_COMPARE_PCI_LOC_OUT;

	//========================================================================
	//	service CopyData
	//========================================================================

	typedef	struct _uPNDEV_COPY_BLOCK_DESC
	{
		UINT32	lOffsetHostDataBuf;
		UINT32	lOffsetAsic;
		UINT32	lSize;
	}
	uPNDEV_COPY_BLOCK_DESC;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_COPY_DATA_IN
	{
		uPNDEV64_HANDLE			uDevice;
		ePNDEV_COPY_HW_RES		eHwRes;
		uPNDEV_COPY_BLOCK_DESC	uArrayBlockWrite[PNDEV_DIM_ARRAY_COPY_DATA];
		uPNDEV_COPY_BLOCK_DESC	uArrayBlockRead[ PNDEV_DIM_ARRAY_COPY_DATA];
	}
	uPNDEV_COPY_DATA_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_COPY_DATA_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_COPY_DATA_OUT;

	//========================================================================
	//	service CloseDevice
	//========================================================================

	typedef struct _uPNDEV_CLOSE_DEVICE_INTERNAL_IN
	{
		uPNDEV64_HANDLE uAppSys;
	}
	uPNDEV_CLOSE_DEVICE_INTERNAL_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CLOSE_DEVICE_INTERNAL_OUT
	{
		uPNDEV_HW_RES_SINGLE_UM			uBarUm[6];
		uPNDEV_HW_RES_SINGLE_UM			uSdramUm;
		uPNDEV_HW_RES_SINGLE_UM     	uHostMemUm;
	}
	uPNDEV_CLOSE_DEVICE_INTERNAL_OUT;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CLOSE_DEVICE_IN
	{
		uPNDEV64_HANDLE	uDevice;
		BOOLEAN			bDoAnyway;
		PNDEV_CHAR		sDriverTarget[PNDEV_SIZE_STRING_BUF_SHORT];

		uPNDEV_CLOSE_DEVICE_INTERNAL_IN uInternal;
	}
	uPNDEV_CLOSE_DEVICE_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CLOSE_DEVICE_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];

		uPNDEV_CLOSE_DEVICE_INTERNAL_OUT uInternal;
	}
	uPNDEV_CLOSE_DEVICE_OUT;

	//------------------------------------------------------------------------
	//	service DoNetProfi
	//------------------------------------------------------------------------

	struct uPNDEV_NP_KERNEL_TMP_COMMON_REQ_DRIVER_IN
	{
        uPNDEV64_HANDLE	    uDevice;
        eNP_CODE_SERVICE	eService;
	};
//------------------------------------------------------------------------
	struct uPNDEV_NP_KERNEL_TMP_COMMON_REQ_DRIVER_OUT
	{
        UINT32 dummy;
    };
//------------------------------------------------------------------------
	struct uPNDEV_NP_KERNEL_TMP_REQ_STARTUP_DRIVER_IN
	{
		UINT32	lSizeSharedMemPc;											// number of Bytes at PC-Ram, max. 4096 MByte
		UINT32	lSizeFifoIntern;											// number of Bytes of Fifo for copying data from ISR to SystemThread
	};
//------------------------------------------------------------------------
	struct uPNDEV_NP_KERNEL_TMP_REQ_STARTUP_DRIVER_OUT
	{
		UINT32  dummy;
	};
//------------------------------------------------------------------------
	struct uPNDEV_NP_KERNEL_TMP_REQ_SHUT_DOWN_DRIVER_IN
	{
        UINT32 dummy;
	};

    struct uPNDEV_NP_KERNEL_TMP_REQ_SHUT_DOWN_DRIVER_OUT
	{
        UINT32 dummy;
	};
//------------------------------------------------------------------------
	struct uPNDEV_NP_KERNEL_TMP_REQ_CAPTURE_DRIVER_IN
	{
		eNP_MODE_CAPTURE	eMode;
		uNP_SW_FILTER_PAR	uFilterSw;
		eNP_BOOL			eDisableProtocol;
		UINT32				lByteMsecHarddisk;								// <>0:		test of Harddisk (bytes/msec)
		UINT32				lNoFileMax;
		UINT32				lSizeFile;
		eNP_BOOL			eModeRingBuffer;
		eNP_BOOL			eDmacwSdram;
		UINT32				lIndexFile;
		UINT64				lIndexFileAbsolute;
		eNP_BOOL			eCreateAsicDmpFile;
		UINT32				lThresholdEvent;
		PNDEV_CHAR			sPathFileData[kNP_LEN_MAX_STRING + 1];			// Attention:	At transition ring3-ring0 the complete buffer for all parameters is mapped (hint: IN/OUT use the same buffer)
																			//				-> that IN/OUT-buffer must include the whole StringBuffer not only the pointer (if a pointer is used driver must allocate buffer by ExAllocatePoolWithTag())
	};
//------------------------------------------------------------------------
	struct uPNDEV_NP_KERNEL_TMP_REQ_CAPTURE_DRIVER_OUT
	{
		uNP_STATISTIC		uStatistic[4];
		UINT32				lTimeStamp_10nsec;
		uNP_DRIVER_INFO		uDriverInfo;
		uNP_PROTOCOL_INFO	uProtocolInfo;
	};
//------------------------------------------------------------------------
	union uPNDEV_NP_KERNEL_TMP_SPEC_REQ_DRIVER_IN
	{
		struct	uPNDEV_NP_KERNEL_TMP_REQ_STARTUP_DRIVER_IN	uStartup;
		struct 	uPNDEV_NP_KERNEL_TMP_REQ_SHUT_DOWN_DRIVER_IN	uShutDown;
		struct 	uPNDEV_NP_KERNEL_TMP_REQ_CAPTURE_DRIVER_IN	uCapture;
	};
//------------------------------------------------------------------------
	union uPNDEV_NP_KERNEL_TMP_SPEC_REQ_DRIVER_OUT
	{
		struct 	uPNDEV_NP_KERNEL_TMP_REQ_STARTUP_DRIVER_OUT	uStartup;
		struct 	uPNDEV_NP_KERNEL_TMP_REQ_SHUT_DOWN_DRIVER_OUT	uShutDown;
		struct 	uPNDEV_NP_KERNEL_TMP_REQ_CAPTURE_DRIVER_OUT	uCapture;
	};
//------------------------------------------------------------------------
	typedef struct _uPNDEV_NP_DO_NETPROFI_REQ_IN
	{
		struct	uPNDEV_NP_KERNEL_TMP_COMMON_REQ_DRIVER_IN	    uCommon;
		union	uPNDEV_NP_KERNEL_TMP_SPEC_REQ_DRIVER_IN		uAs;
	}
	uPNDEV_NP_DO_NETPROFI_REQ_IN;
//------------------------------------------------------------------------
	typedef struct _uPNDEV_NP_DO_NETPROFI_REQ_OUT
	{
        PNDEV_CHAR 	                                            sError[PNDEV_SIZE_STRING_BUF];
		struct	    uPNDEV_NP_KERNEL_TMP_COMMON_REQ_DRIVER_OUT	uCommon;
		union	    uPNDEV_NP_KERNEL_TMP_SPEC_REQ_DRIVER_OUT	uAs;

	}
	uPNDEV_NP_DO_NETPROFI_REQ_OUT;

//------------------------------------------------------------------------
	typedef struct _uPNDEV_MAP_MEMORY_IN
	{
		uPNDEV_HW_RES_SINGLE_KM_TMP	uBarKm[6];
		uPNDEV_HW_RES_SINGLE_KM_TMP	uSdramKm;
		uPNDEV_HW_RES_SINGLE_KM_TMP	uHostMemKm;
	}
	uPNDEV_MAP_MEMORY_IN;
//------------------------------------------------------------------------
	typedef struct _uPNDEV_MAP_MEMORY_OUT
	{
        PNDEV_CHAR				sError[PNDEV_SIZE_STRING_BUF];

		uPNDEV_HW_RES_SINGLE_UM	uBarUm[6];
		uPNDEV_HW_RES_SINGLE_UM	uSdramUm;
		uPNDEV_HW_RES_SINGLE_UM	uHostMemUm;
	}
	uPNDEV_MAP_MEMORY_OUT;

	//========================================================================
	//	common for NRT
	//========================================================================

	//------------------------------------------------------------------------
	// LongEnable=0:
	//		without VLAN:	FrameLen <= 1518 (incl. CRC): 6+6+0+2+2+Data+CRC -> Data <= 1498
	//		with VLAN:		FrameLen <= 1522 (incl. CRC): 6+6+4+2+2+Data+CRC -> Data <= 1498
	// LongEnable=1:		FrameLen <= 1536 (incl. CRC): 6+6+4+2+2+Data+CRC -> Data <= 1512

	// Note:
	//		There is no fixed gap for VLAN. The buffer is filled like the stream on the bus

	struct uPNDEV_AS_FRAME_HEADER
	{
		UINT8	lArrayMacAdrDa[6];
		UINT8	lArrayMacAdrSa[6];
		UINT8	lArrayVlanTag[4];
	};

	//------------------------------------------------------------------------
	struct uPNDEV_AS_FRAME_NO_VLAN												// FrameLen <= 1536
	{
		UINT8	lArrayMacAdrDa[6];
		UINT8	lArrayMacAdrSa[6];
		UINT8	lArrayEtherType[2];
		UINT8	lArrayFrameId[2];
		UINT8	lArrayData[1520];
	};

	//------------------------------------------------------------------------
	struct uPNDEV_AS_FRAME_VLAN													// FrameLen <= 1536
	{
		UINT8	lArrayMacAdrDa[6];
		UINT8	lArrayMacAdrSa[6];
		UINT8	lArrayVlanType[2];
		UINT8	lArrayVlanTag[2];
		UINT8	lArrayEtherType[2];
		UINT8	lArrayFrameId[2];
		UINT8	lArrayData[1516];
	};

	//------------------------------------------------------------------------
	typedef union _uPNDEV_FRAME_BUF
	{
		struct	uPNDEV_AS_FRAME_HEADER	uAs_Header;
		struct	uPNDEV_AS_FRAME_NO_VLAN	uAs_FrameNoVlan;
		struct	uPNDEV_AS_FRAME_VLAN	uAs_FrameVlan;
	}
	uPNDEV_FRAME_BUF;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_DMACW
	{
		union
		{
			struct
			{
				union
				{
					struct
					{
						UINT32 lTest;
					}
					uErtec400;

					struct
					{
						UINT32 lTest;
					}
					uErtec200;

					struct
					{
						UINT32 lTest;
					}
					uSoc1;

					struct
					{
						UINT32	lLenFrameNoCrc:			11;						// Bit 0..10 (FrameLen without CRC, CRC=4 Bytes, 24..1532 Bytes)
						UINT32	lDestPort:				4;						// Bit11..14
						UINT32	lDestRawMode:			1;						// Bit15
						UINT32	lPhasePrio:				3;						// Bit16..18
						UINT32	lSelectPhasePrio:		2;						// Bit19..20
						UINT32	lReserved1:				2;						// Bit21..22
						UINT32	lRawMode:				1;						// Bit23
						UINT32	lSendResponse:			1;						// Bit24
						UINT32	lLearningDisable:		1;						// Bit25
						UINT32	lRtHandling:			2;						// Bit26..27
						UINT32	lResult:				3;						// Bit28..30
						UINT32	lOwner:					1;						// Bit31
						UINT32	pNext;
						UINT32	lTime;
						UINT32	pFrameBuffer;
						UINT32	lFrameHandling:			2;						// Bit 0..1
						UINT32	lSelectFrameHandling:	1;						// Bit 2
						UINT32	lReserved2:				1;						// Bit 3
						UINT32	lUseType:				3;						// Bit 4..6
						UINT32	lSelectUseType:			1;						// Bit 7
						UINT32	lTrafficClass:			3;						// Bit 8..10
						UINT32	lSelectTrafficClass:	1;						// Bit11
						UINT32	lRedundancy:			1;						// Bit12
						UINT32	lSelectRedundancy:		1;						// Bit13
						UINT32	lFragmentation:			1;						// Bit14
						UINT32	lSelectFragmentation:	1;						// Bit15
						UINT32	lResourceType:			4;						// Bit16..19
						UINT32	lSelectResourceType:	1;						// Bit20
						UINT32	lReserved3:				3;						// Bit21..23
						UINT32	lPrioMapping:			1;						// Bit24
						UINT32	lSelectPrioMapping:		1;						// Bit25
						UINT32	lReserved4:				6;						// Bit26..31
					}
					uErtec200P;
				}
				as;
			}
			uSend;

			struct
			{
				union
				{
					struct
					{
						UINT32 lTest;
					}
					uErtec400;

					struct
					{
						UINT32 lTest;
					}
					uErtec200;

					struct
					{
						UINT32	lLenFrameNoCrc:			11;						// Bit 0..10 (FrameLen without CRC, CRC=4 Bytes)
						UINT32	lSelector:				1;						// Bit11 (>=Rev.6)
						UINT32	lPort:					2;						// Bit12..13
						UINT32	lReserved1:				1;						// Bit14
						UINT32	lReserved2:				1;						// Bit15
						UINT32	lLenFrameBuffer:		9;						// Bit16..24
						UINT32	lReserved3:				1;						// Bit25
						UINT32	lTrigger:				1;						// Bit26
						UINT32	lStatus:				2;						// Bit27..28
						UINT32	lOwner:					1;						// Bit 29
						UINT32	lSrtPrioBackToBack:		1;						// Bit 30 (only relevant at mode BackToBack)
						UINT32	lTagged:				1;						// Bit 31
						UINT32	pNext;
						UINT32	lTime;
						UINT32	pFrameBuffer;
					}
					uSoc1;

					struct
					{
						UINT32	lLenFrameNoCrc:			11;						// Bit 0..10 (FrameLen without CRC, CRC=4 Bytes)
						UINT32	lSrcPort:				4;						// Bit11..14
						UINT32	lReserved1:				1;						// Bit15
						UINT32	lLenFrameBuffer:		9;						// Bit16..24
						UINT32	lReserved2:				2;						// Bit25..26
						UINT32	lResult:				2;						// Bit27..28
						UINT32	lSnycFrame:				1;						// Bit29
						UINT32	lReserved3:				1;						// Bit30
						UINT32	lOwner:					1;						// Bit31
						UINT32	pNext;
						UINT32	lTime;
						UINT32	pFrameBuffer;
						UINT32	lCodeFilterDecision:	16;						// Bit 0..15
						UINT32	lUseType:				3;						// Bit16..18
						UINT32	lReserved4:				13;						// Bit19..31
					}
					uErtec200P;
				}
				as;
			}
			uRcve;
		}
		as;
	}
	uPNDEV_DMACW;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_NRT_RQB
	{
		uPNDEV_DMACW		uDmacw;
		uPNDEV_FRAME_BUF	uFrameBuf;
	}
	uPNDEV_NRT_RQB;

	//------------------------------------------------------------------------
	typedef	union _uPNDEV64_PTR_NRT_RQB
	{
		volatile uPNDEV_NRT_RQB*	pPtr;
		UINT64						lUint64;
	}
	uPNDEV64_PTR_NRT_RQB;

	//========================================================================
	//	service InitNrt
	//========================================================================

	typedef struct _uPNDEV_INIT_NRT_IN
	{
		uPNDEV64_HANDLE			uDevice;
		uPNDEV_HW_DESC			uMemNrt;
		uPNDEV_HW_DESC			uMemDescTable;
		uPNDEV64_CBF_NRT_SEND	uCbfNrtSend;
		uPNDEV64_CBF_NRT_RCVE	uCbfNrtRcve;
		uPNDEV64_PTR_VOID		uCbfParUser;
	}
	uPNDEV_INIT_NRT_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_INIT_NRT_OUT
	{
		PNDEV_CHAR	sError[PNDEV_SIZE_STRING_BUF];
		UINT32		lCtrNrtRqb;
	}
	uPNDEV_INIT_NRT_OUT;

	//========================================================================
	//	service AllocNrtRqb
	//========================================================================

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_ALLOC_NRT_RQB_IN
	{
		uPNDEV64_HANDLE uDevice;
	}
	uPNDEV_ALLOC_NRT_RQB_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_ALLOC_NRT_RQB_OUT
	{
		PNDEV_CHAR				sError[PNDEV_SIZE_STRING_BUF];
		uPNDEV64_PTR_NRT_RQB	uNrtRqb;
	}
	uPNDEV_ALLOC_NRT_RQB_OUT;

	//========================================================================
	//	service FreeNrtRqb
	//========================================================================

	typedef struct _uPNDEV_FREE_NRT_RQB_IN
	{
		uPNDEV64_HANDLE			uDevice;
		uPNDEV64_PTR_NRT_RQB	uNrtRqb;
	}
	uPNDEV_FREE_NRT_RQB_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_FREE_NRT_RQB_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_FREE_NRT_RQB_OUT;

	//========================================================================
	//	service SendNrt
	//========================================================================

	typedef struct _uPNDEV_SEND_NRT_IN
	{
		uPNDEV64_HANDLE			uDevice;
		uPNDEV64_PTR_NRT_RQB	uNrtRqb;
	}
	uPNDEV_SEND_NRT_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_SEND_NRT_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_SEND_NRT_OUT;

	//========================================================================
	//	service RcveNrt
	//========================================================================

	typedef struct _uPNDEV_RCVE_NRT_IN
	{
		uPNDEV64_HANDLE			uDevice;
		uPNDEV64_PTR_NRT_RQB	uNrtRqb;
	}
	uPNDEV_RCVE_NRT_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_RCVE_NRT_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_RCVE_NRT_OUT;

	//========================================================================
	//	service ChangeNrt
	//========================================================================

	typedef struct _uPNDEV_CHANGE_NRT_IN
	{
		uPNDEV64_HANDLE	uDevice;
		BOOLEAN			bEnableRcve;
		BOOLEAN			bFreeNrtMem;
	}
	uPNDEV_CHANGE_NRT_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CHANGE_NRT_OUT
	{
		PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_CHANGE_NRT_OUT;

	//------------------------------------------------------------------------
	#pragma pack(pop)															// restore packing alignment
//	#pragma pack(show)

	//########################################################################
	//  Function prototypes
	//########################################################################

	// example of an exported class
/*
	class PNDEV_DRIVER_API clsPnDev_Driver
	{
	public:
		clsPnDev_Driver(void);
	};
*/
	//------------------------------------------------------------------------
	// test function
	typedef				int (*PNDEV_FN_TEST)(	void);
	PNDEV_DRIVER_API	int fnPnDev_Test(		void);

	//------------------------------------------------------------------------
	typedef	ePNDEV_RESULT	(*PNDEV_FN_OPEN_DRIVER)(		const UINT32					lDimErrorIn,
															void**							pHandleDriverOut,
															PNDEV_CHAR*						sErrorOut);
	typedef	ePNDEV_RESULT	(*PNDEV_FN_CLOSE_DRIVER)(		HANDLE							hDriverIn,
															const UINT32					lDimErrorIn,
															PNDEV_CHAR*						sErrorOut);
	typedef	LSA_UINT16		(*PNDEV_FN_GET_VERSION)(		LSA_UINT16						length,
															LSA_VERSION_TYPE*				ptr_version);
	typedef	ePNDEV_RESULT	(*PNDEV_FN_CHECK_VERSION)(		HANDLE							hDriverIn,
															const UINT32					lVersionIn,
															const UINT32					lDistributionIn,
															const UINT32					lIncrementIn,
															const UINT32					lIntegrationCtrIn,
															const UINT32					lDimErrorIn,
															PNDEV_CHAR*						sErrorOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_GET_DEVICE_INFO)(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															const UINT32					lDimOutputArrayIn,
															const UINT32					lDimErrorIn,
															uPNDEV_GET_DEVICE_INFO_IN*		pIn,
															uPNDEV_GET_DEVICE_INFO_OUT		uArrayOut[],
															UINT32*							pCtrDeviceOut,
															PNDEV_CHAR*						sErrorOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_OPEN_DEVICE)(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_OPEN_DEVICE_IN*			pIn,
															uPNDEV_OPEN_DEVICE_OUT*			pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_CHANGE_INT)(			HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CHANGE_INT_IN*			pIn,
															uPNDEV_CHANGE_INT_OUT*			pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_GET_DEVICE_STATE)(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_GET_DEVICE_STATE_IN*		pIn,
															uPNDEV_GET_DEVICE_STATE_OUT*	pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_PROGRAM_FLASH)(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_PROGRAM_FLASH_IN*		pIn,
															uPNDEV_PROGRAM_FLASH_OUT*		pOut);
	typedef	ePNDEV_RESULT	(*PNDEV_FN_CHANGE_XHIF_PAGE)(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CHANGE_XHIF_PAGE_IN*		pIn,
															uPNDEV_CHANGE_XHIF_PAGE_OUT*	pOut);
	typedef	ePNDEV_RESULT	(*PNDEV_FN_COMPARE_PCI_LOC)(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_COMPARE_PCI_LOC_IN*		pIn,
															uPNDEV_COMPARE_PCI_LOC_OUT*		pOut);
	typedef	ePNDEV_RESULT	(*PNDEV_FN_COPY_DATA)(			HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_COPY_DATA_IN*			pIn,
															uPNDEV_COPY_DATA_OUT*			pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_CLOSE_DEVICE)(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CLOSE_DEVICE_IN*			pIn,
															uPNDEV_CLOSE_DEVICE_OUT*		pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_INIT_NRT)(			HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_INIT_NRT_IN*				pIn,
															uPNDEV_INIT_NRT_OUT*			pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_ALLOC_NRT_RQB)(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_ALLOC_NRT_RQB_IN*		pIn,
															uPNDEV_ALLOC_NRT_RQB_OUT*		pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_FREE_NRT_RQB)(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_FREE_NRT_RQB_IN*			pIn,
															uPNDEV_FREE_NRT_RQB_OUT*		pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_SEND_NRT)(			HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_SEND_NRT_IN*				pIn,
															uPNDEV_SEND_NRT_OUT*			pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_RCVE_NRT)(			HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_RCVE_NRT_IN*				pIn,
															uPNDEV_RCVE_NRT_OUT*			pOut);
	typedef ePNDEV_RESULT	(*PNDEV_FN_CHANGE_NRT)(			HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CHANGE_NRT_IN*			pIn,
															uPNDEV_CHANGE_NRT_OUT*			pOut);
   typedef ePNDEV_RESULT	(*PNDEV_FN_DO_NETPROFI)   (	HANDLE							hDriverIn,
													        const UINT32					lSizeInputBufIn,
													        const UINT32					lSizeOutputBufIn,
															uPNDEV_NP_DO_NETPROFI_REQ_IN*	    pIn,
													        uPNDEV_NP_DO_NETPROFI_REQ_OUT*		pOut);

	//========================================================================
	// Note:
	//	- ensure that driver and DLL use the same alignment!
//	#pragma pack(show)
    #pragma pack(push)															// save current packing alignment
    //------------------------------------------------------------------------
    #if defined PNDEV_OS_WINDOWS || defined PNDEV_OS_ADONIS
    //------------------------------------------------------------------------
    #pragma pack(8)                                                           // set packing alignment 8 Byte
    //------------------------------------------------------------------------
    #elif defined PNDEV_OS_LINUX
    //------------------------------------------------------------------------
    #pragma pack(4)                                                           // set packing alignment 4 Byte
    //------------------------------------------------------------------------
    #else 
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    #endif
//	#pragma pack(show)

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_FCT_PTR
	{
		PNDEV_FN_TEST				pFnTest;
		PNDEV_FN_OPEN_DRIVER		pFnOpenDriver;
		PNDEV_FN_CLOSE_DRIVER		pFnCloseDriver;
		PNDEV_FN_GET_VERSION		pFnGetVersion;
		PNDEV_FN_CHECK_VERSION		pFnCheckVersion;
		PNDEV_FN_GET_DEVICE_INFO	pFnGetDeviceInfo;
		PNDEV_FN_OPEN_DEVICE		pFnOpenDevice;
		PNDEV_FN_CHANGE_INT			pFnChangeInt;
		PNDEV_FN_GET_DEVICE_STATE	pFnGetDeviceState;
		PNDEV_FN_PROGRAM_FLASH		pFnProgramFlash;
		PNDEV_FN_CHANGE_XHIF_PAGE	pFnChangeXhifPage;
		PNDEV_FN_COMPARE_PCI_LOC	pFnComparePciLoc;
		PNDEV_FN_COPY_DATA			pFnCopyData;
		PNDEV_FN_CLOSE_DEVICE		pFnCloseDevice;
		PNDEV_FN_INIT_NRT			pFnInitNrt;
		PNDEV_FN_ALLOC_NRT_RQB		pFnAllocNrtRqb;
		PNDEV_FN_FREE_NRT_RQB		pFnFreeNrtRqb;
		PNDEV_FN_SEND_NRT			pFnSendNrt;
		PNDEV_FN_RCVE_NRT			pFnRcveNrt;
		PNDEV_FN_CHANGE_NRT			pFnChangeNrt;
        PNDEV_FN_DO_NETPROFI       	pFnDoNetProfi;
	}
	uPNDEV_FCT_PTR;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_DRIVER_DESC
	{
		LSA_VERSION_TYPE	uVersion;
		void*				pDataSet;
		HANDLE				hDll;
		uPNDEV_FCT_PTR		uIf;
	}
	uPNDEV_DRIVER_DESC;

	//------------------------------------------------------------------------
	#pragma pack(pop)															// restore packing alignment
//	#pragma pack(show)

	//========================================================================
	// located at common\PnDev_DriverU.c

	ePNDEV_RESULT	fnPnDev_StartUpDriver(	PNDEV_CHAR*				sPathDllIn,
											const UINT32			lDimErrorIn,
											uPNDEV_DRIVER_DESC**	pHandleDriverOut,
											PNDEV_CHAR*				sErrorOut);
	ePNDEV_RESULT	fnPnDev_ShutDownDriver(	uPNDEV_DRIVER_DESC*		hDriverIn,
											const UINT32			lDimErrorIn,
											PNDEV_CHAR*				sErrorOut);

	//------------------------------------------------------------------------
	// located at DLL
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_OpenDriver(		const UINT32					lDimErrorIn,
															HANDLE*							pHandleDriverOut,
															PNDEV_CHAR*						sErrorOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_CloseDriver(	HANDLE							hDriverIn,
															const UINT32					lDimErrorIn,
															PNDEV_CHAR*						sErrorOut);
	PNDEV_DRIVER_API LSA_UINT16		fnPnDev_GetVersion(		LSA_UINT16						length,
															LSA_VERSION_TYPE*				ptr_version);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_CheckVersion(	HANDLE							hDriverIn,
															const UINT32					lVersionIn,
															const UINT32					lDistributionIn,
															const UINT32					lIncrementIn,
															const UINT32					lIntegrationCtrIn,
															const UINT32					lDimErrorIn,
															PNDEV_CHAR*						sErrorOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_GetDeviceInfo(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															const UINT32					lDimOutputArrayIn,
															const UINT32					lDimErrorIn,
															uPNDEV_GET_DEVICE_INFO_IN*		pIn,
															uPNDEV_GET_DEVICE_INFO_OUT		uArrayOut[],
															UINT32*							pCtrDeviceOut,
															PNDEV_CHAR*						sErrorOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_OpenDevice(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_OPEN_DEVICE_IN*			pIn,
															uPNDEV_OPEN_DEVICE_OUT*			pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_ChangeInt(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CHANGE_INT_IN*			pIn,
															uPNDEV_CHANGE_INT_OUT*			pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_GetDeviceState(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_GET_DEVICE_STATE_IN*		pIn,
															uPNDEV_GET_DEVICE_STATE_OUT*	pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_ProgramFlash(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_PROGRAM_FLASH_IN*		pIn,
															uPNDEV_PROGRAM_FLASH_OUT*		pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_ChangeXhifPage(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CHANGE_XHIF_PAGE_IN*		pIn,
															uPNDEV_CHANGE_XHIF_PAGE_OUT*	pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_ComparePciLoc(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_COMPARE_PCI_LOC_IN*		pIn,
															uPNDEV_COMPARE_PCI_LOC_OUT*		pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_CopyData(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_COPY_DATA_IN*			pIn,
															uPNDEV_COPY_DATA_OUT*			pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_CloseDevice(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CLOSE_DEVICE_IN*			pIn,
															uPNDEV_CLOSE_DEVICE_OUT*		pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_InitNrt(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_INIT_NRT_IN*				pIn,
															uPNDEV_INIT_NRT_OUT*			pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_AllocNrtRqb(	HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_ALLOC_NRT_RQB_IN*		pIn,
															uPNDEV_ALLOC_NRT_RQB_OUT*		pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_FreeNrtRqb(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_FREE_NRT_RQB_IN*			pIn,
															uPNDEV_FREE_NRT_RQB_OUT*		pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_SendNrt(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_SEND_NRT_IN*				pIn,
															uPNDEV_SEND_NRT_OUT*			pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_RcveNrt(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_RCVE_NRT_IN*				pIn,
															uPNDEV_RCVE_NRT_OUT*			pOut);
	PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_ChangeNrt(		HANDLE							hDriverIn,
															const UINT32					lSizeInputBufIn,
															const UINT32					lSizeOutputBufIn,
															uPNDEV_CHANGE_NRT_IN*			pIn,
															uPNDEV_CHANGE_NRT_OUT*			pOut);

	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ compiler: End of C linkage
	}
	#endif

#endif
