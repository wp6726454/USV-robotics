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
/*  F i l e               &F: Board_Intel.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for Intel boards
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Kerl			xx
/*****************************************************************************/

#ifndef __Board_Intel_h__
#define __Board_Intel_h__

	//########################################################################
	//  Defines
	//########################################################################
	//========================================================================
	//	Intel
	//========================================================================

	// Interrupt registers
	#define BOARD_INTEL__ICR						0x00C0
	#define BOARD_INTEL__ITR						0x00C4
	#define BOARD_INTEL__ICS						0x00C8
	#define BOARD_INTEL__IMS						0x00D0
	#define BOARD_INTEL__IMC						0x00D8
	//========================================================================
	//	Intel I210 (formerly Springville)
	//========================================================================

	// CTRL Register
	#define BOARD_I210__CTRL					0x0
	#define BOARD_I210__CTRL_RESERVED_MASK		0xFCFF1BC5
	#define BOARD_I210__CTRL_FD					0
	// reserved 1  (set to 0b)
	#define BOARD_I210__CTRL_GIO_MASTER_DISABLE	2		
	#define BOARD_I210__CTRL_LRST				3
	// reserved 4  (set to 0b)
	#define BOARD_I210__CTRL_ASDE				5
	#define BOARD_I210__CTRL_SLU				6
	#define BOARD_I210__CTRL_ILOS				7
	#define BOARD_I210__CTRL_SPEED_s			8
	#define BOARD_I210__CTRL_SPEED_e			9
	// reserved 10  (set to 0b)
	#define BOARD_I210__CTRL_FRCSPD				11
	#define BOARD_I210__CTRL_FRCDPLX			12
	// reserved 17:13  (set to 0x0)
	#define BOARD_I210__CTRL_SDP0_DATA			18
	#define BOARD_I210__CTRL_SDP1_DATA			19
	#define BOARD_I210__CTRL_ADVD3WUC			20
	#define BOARD_I210__CTRL_EN_PHY_PWR_MGMT	21
	#define BOARD_I210__CTRL_SDP0_IODIR			22
	#define BOARD_I210__CTRL_SDP1_IODIR			23
	// reserved 25:24  (set to 0x0)
	#define BOARD_I210__CTRL_RST				26
	#define BOARD_I210__CTRL_RFCE				27
	#define BOARD_I210__CTRL_TFCE				28
	#define BOARD_I210__CTRL_DEV_RST			29		
	#define BOARD_I210__CTRL_VME				30
	#define BOARD_I210__CTRL_PHY_RST			31

	// Status Register
	#define BOARD_I210__STATUS					0x8
	#define BOARD_I210__STATUS_FD				0
	#define BOARD_I210__STATUS_LU				1
	#define BOARD_I210__STATUS_FunctionID_s		2
	#define BOARD_I210__STATUS_FunctionID_e		3
	#define BOARD_I210__STATUS_TXOFF			4
	#define BOARD_I210__STATUS_TBIMODE			5
	#define BOARD_I210__STATUS_SPEED_s			6
	#define BOARD_I210__STATUS_SPEED_e			7
	#define BOARD_I210__STATUS_ASDV_s			8
	#define BOARD_I210__STATUS_ASDV_e			9
	// reserved 10
	#define BOARD_I210__STATUS_PCI66			11
	#define BOARD_I210__STATUS_BUS64			12
	#define BOARD_I210__STATUS_PCIX_MODE		13
	#define BOARD_I210__STATUS_PCIXSPD_s		14
	#define BOARD_I210__STATUS_PCIXSPD_e		15
	#define BOARD_I210__STATUS_PF_RST_DONE		21		
	// reserved 31:16

	#define BOARD_I210__EECD					0x00010
	#define BOARD_I210__EDCD_AUTO_RD			9

	// Interrupts on I210 board
	#define BOARD_I210__ICU_RESERVED_MASK			0x67CC7CD5

	#define BOARD_I210__ICU_VECTOR__TXDW			(1<<0)
	// reserved 1	
	#define BOARD_I210__ICU_VECTOR__LSC        	 	(1<<2)
	// reserved 3
	#define BOARD_I210__ICU_VECTOR__RXDMT0     	 	(1<<4)
	// reserved 5
	#define BOARD_I210__ICU_VECTOR__RX_MISS     	(1<<6)
	#define BOARD_I210__ICU_VECTOR__RXDW			(1<<7)
	// reserved 9:8
	#define BOARD_I210__ICU_VECTOR__GPHY			(1<<10)
	#define BOARD_I210__ICU_VECTOR__GPI_SDP0  		(1<<11)
	#define BOARD_I210__ICU_VECTOR__GPI_SDP1  		(1<<12)
	#define BOARD_I210__ICU_VECTOR__GPI_SDP2  		(1<<13)
	#define BOARD_I210__ICU_VECTOR__GPI_SDP3  		(1<<14)
	// reserved 15 - kein Wert
	// reserved 16:17
	#define BOARD_I210__ICU_VECTOR__MNG       		(1<<18)
	#define BOARD_I210__ICU_VECTOR__TIME_SYNC 		(1<<19)
	// reserved 21:20 
	#define BOARD_I210__ICU_VECTOR__FER       		(1<<22)
	// reserved 23
	#define BOARD_I210__ICU_VECTOR__PCI_EXEPTION 	(1<<24)
	#define BOARD_I210__ICU_VECTOR__SCE           	(1<<25)
	#define BOARD_I210__ICU_VECTOR__SOFTWARE_WD   	(1<<26)
	// reserved 27
	// reserved 28 - kein Wert
	#define BOARD_I210__ICU_VECTOR__TCP_TIMER     	(1<<29)
	#define BOARD_I210__ICU_VECTOR__DRSTA         	(1<<30)
	// reserved 31

	#define BOARD_I210__CTRL_EXT					0x00018
	#define BOARD_I210__CTRL_EXT_RESERVED_MASK		0x16DFBCCE
	#define BOARD_I210__CTRL_EXT_DRV_LOAD			(1<<28)

	#define BOARD_I210__MREVID						0x5B64
	#define BOARD_I210__EERD						0x12014
	#define BOARD_I210__EERD_DONE					1

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_I210__BAR0_SIZE	(1024*512)
	#define	BOARD_I210__BAR1_SIZE	(1024*0)
	#define	BOARD_I210__BAR2_SIZE	(1024*0)
	#define	BOARD_I210__BAR3_SIZE	(1024*16)
	#define	BOARD_I210__BAR4_SIZE	(1024*0)
	#define	BOARD_I210__BAR5_SIZE	(1024*0)

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

	//------------------------------------------------------------------------
	//	PCI-Bridge Register
	//------------------------------------------------------------------------


	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------


	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------

	//========================================================================
	//	Intel I82574 (formerly Hartwell)
	//========================================================================

	// CTRL Register
	#define BOARD_I82574__CTRL						0x0
	#define BOARD_I82574__CTRL_RESERVED_ZEROS_MASK	0xDC101B6D
	#define BOARD_I82574__CTRL_RESERVED_ONES_MASK	0x00000008
	#define BOARD_I82574__CTRL_FD					0
	// reserved 1 (set to 0b)
	#define BOARD_I82574__CTRL_GIO_MASTER_DISABLE	2
	// reserved 3 (set to 1b)
	// reserved 4 (set to 0b)
	#define BOARD_I82574__CTRL_ASDE					5
	#define BOARD_I82574__CTRL_SLU					6
	// reserved 7 (set to 0b)
	#define BOARD_I82574__CTRL_SPEED_s				8
	#define BOARD_I82574__CTRL_SPEED_e				9
	// reserved 10 (set to 0b)
	#define BOARD_I82574__CTRL_FRCSPD				11
	#define BOARD_I82574__CTRL_FRCDPLX				12
	// reserved 19:13 (set to 0x0)
	#define BOARD_I82574__CTRL_ADVD3WUC				20
	// reserved 25:21 (set to 0x0)
	#define BOARD_I82574__CTRL_RST					26
	#define BOARD_I82574__CTRL_RFCE					27
	#define BOARD_I82574__CTRL_TFCE					28
	// reserved 29 (set to 0b)
	#define BOARD_I82574__CTRL_VME					30
	#define BOARD_I82574__CTRL_PHY_RST				31

	// Status Register
	#define BOARD_I82574__STATUS						0x8
	#define BOARD_I82574__STATUS_FD					0
	#define BOARD_I82574__STATUS_LU					1
	// reserved 3:2
	#define BOARD_I82574__STATUS_TXOFF				4
	// reserved 5
	#define BOARD_I82574__STATUS_SPEED_s				6
	#define BOARD_I82574__STATUS_SPEED_e				7
	#define BOARD_I82574__STATUS_ASDV_s				8
	#define BOARD_I82574__STATUS_ASDV_e				9
	#define BOARD_I82574__STATUS_PHYRA				10
	// reserved 18:11
	#define BOARD_I82574__STATUS_GIO_MASTER_ENABLE 	19
	// reserved 31:20

	//EEPROM/FLASH Control Register EEC
	#define BOARD_I82574__EEC						0x00010
	#define BOARD_I82574__EEC_AUTO_RD				9

	#define BOARD_I82574__EERD						0x00014
	#define BOARD_I82574__EERD_DONE					1

	// Interrupts on I82574 board
	#define BOARD_I82574__ICU_RESERVED_MASK			0x01F782D7

	#define BOARD_I82574__ICU_VECTOR__TXDW			(1<<0)
	#define BOARD_I82574__ICU_VECTOR__TXQE			(1<<1)
	#define BOARD_I82574__ICU_VECTOR__LSC        	(1<<2)
	// reserved 3
	#define BOARD_I82574__ICU_VECTOR__RXDMT0     	(1<<4)
	// reserved 5
	#define BOARD_I82574__ICU_VECTOR__RXO		    (1<<6)
	#define BOARD_I82574__ICU_VECTOR__RXT0			(1<<7)
	// reserved 8
	#define BOARD_I82574__ICU_VECTOR__MDAC			(1<<9)
	// reserved 14:10
	#define BOARD_I82574__ICU_VECTOR__TXD_LOW		(1<<15)
	#define BOARD_I82574__ICU_VECTOR__SRPD			(1<<16)
	#define BOARD_I82574__ICU_VECTOR__ACK			(1<<17)
	#define BOARD_I82574__ICU_VECTOR__MNG       		(1<<18)
	// reserved 19 
	#define BOARD_I82574__ICU_VECTOR__RXQ0      		(1<<20)
	#define BOARD_I82574__ICU_VECTOR__RXQ1      		(1<<21)
	#define BOARD_I82574__ICU_VECTOR__TXQ0      		(1<<22)
	#define BOARD_I82574__ICU_VECTOR__TXQ1      		(1<<23)
	#define BOARD_I82574__ICU_VECTOR__OTHER 			(1<<24)
	// reserved 30:25
	// INT_ASSERTED is a non-maskable interrupt 
	// it can be enabled in PCI configuration space
	#define BOARD_I82574__ICU_VECTOR__INT_ASSERTED  	(1<<31)


	#define BOARD_I82574__CTRL_EXT					0x00018
	#define BOARD_I82574__CTRL_EXT_RESERVED_MASK		0xB9DAB000
	#define BOARD_I82574__CTRL_EXT_DRV_LOAD			(1<<28)


	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_I82574__BAR0_SIZE	(1024*128)
	#define	BOARD_I82574__BAR1_SIZE	(1024*512)
	#define	BOARD_I82574__BAR2_SIZE	(1024*0)
	#define	BOARD_I82574__BAR3_SIZE	(1024*16)
	#define	BOARD_I82574__BAR4_SIZE	(1024*0)
	#define	BOARD_I82574__BAR5_SIZE	(1024*0)

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

	//------------------------------------------------------------------------
	//	PCI-Bridge Register
	//------------------------------------------------------------------------


	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------


	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
	

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

#endif
