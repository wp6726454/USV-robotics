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
/*  F i l e               &F: Flash.h                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for basic functions of flash
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __Flash_h__
#define __Flash_h__

	//########################################################################
	//  Defines
	//########################################################################

	#define	PNDEV_DIM_ARRAY_FLASH_TYPE	2

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_FLASH_MODE
	{
		ePNDEV_FLASH_MODE_INVALID		= 0,
		ePNDEV_FLASH_MODE_X8_AS_X8		= 1,
		ePNDEV_FLASH_MODE_X8_AS_X16		= 2,
		ePNDEV_FLASH_MODE_X16_AS_X16	= 3,
		ePNDEV_FLASH_MODE_X16_AS_X32	= 4,
		ePNDEV_FLASH_MODE_X32_AS_X32	= 5,	
        ePNDEV_FLASH_MODE_SERIAL    	= 6
	}
	ePNDEV_FLASH_MODE;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_FLASH_TYPE
	{
		ePNDEV_FLASH_TYPE_INVALID			= 0,
		ePNDEV_FLASH_TYPE_NOR_AM29LV040B	= 1,
		ePNDEV_FLASH_TYPE_NOR_AM29LV641		= 2,
		ePNDEV_FLASH_TYPE_NOR_S29GL064N		= 3,
		ePNDEV_FLASH_TYPE_NOR_S29GL512P		= 4,
		ePNDEV_FLASH_TYPE_NOR_S29WS128P		= 5,
		ePNDEV_FLASH_TYPE_NAND_K91G08U0B	= 6,
        ePNDEV_FLASH_TYPE_SPI_N25Q128A  	= 7
	}
	ePNDEV_FLASH_TYPE;

	//------------------------------------------------------------------------
	// Note:
	//	- There must be unique bits which can be combined!

	typedef enum _ePNDEV_FLASH_INFO
	{
		ePNDEV_FLASH_INFO_INVALID			= 0,
		ePNDEV_FLASH_INFO_SBL_EXIST			= 1,								// Bit0 signals: there is a SBL which must not be erased
		ePNDEV_FLASH_INFO_BOARD_DATA_EXIST	= 2									// Bit1 signals: there are BoardData which must not be erased
	}
	ePNDEV_FLASH_INFO;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_FLASH_VERSION
	{
		ePNDEV_FLASH_VERSION_INVALID	= 0,
		ePNDEV_FLASH_VERSION_V0001		= 0xabcd0001,
		ePNDEV_FLASH_VERSION_EMPTY		= 0xFFFFffff							// value if flash is erased
	}
	ePNDEV_FLASH_VERSION;

	//========================================================================
	//	NOR flash
	//========================================================================

	// Spansion flash: see lld.h
	typedef enum _ePNDEV_FLASH_NOR_POLL
	{
		ePNDEV_FLASH_NOR_POLL_NONE			= 0,
		ePNDEV_FLASH_NOR_POLL_PGM			= 1,								// poll program status
		ePNDEV_FLASH_NOR_POLL_WRT_BUF_PGM	= 2,								// poll program status at WriteToBuffer
		ePNDEV_FLASH_NOR_POLL_SEC_ERS		= 3,								// poll sector erase
		ePNDEV_FLASH_NOR_POLL_CHIP_ERS		= 4,								// poll chip erase
		ePNDEV_FLASH_NOR_POLL_RESUME		= 5,
		ePNDEV_FLASH_NOR_POLL_BLANK			= 6									// poll device sector blank check
	}
	ePNDEV_FLASH_NOR_POLL;

	//------------------------------------------------------------------------
	// Spansion flash: see lld.h
	typedef enum _ePNDEV_FLASH_NOR_STATUS
	{
		 ePNDEV_FLASH_NOR_STATUS_UNKNOWN				= 0,
		 ePNDEV_FLASH_NOR_STATUS_NOT_BUSY				= 1,
		 ePNDEV_FLASH_NOR_STATUS_BUSY					= 2,
		 ePNDEV_FLASH_NOR_STATUS_EXCEEDED_TIME_LIMITS	= 3,
		 ePNDEV_FLASH_NOR_STATUS_SUSPEND				= 4,
		 ePNDEV_FLASH_NOR_STATUS_WRITE_BUFFER_ABORT		= 5,
		 ePNDEV_FLASH_NOR_STATUS_STATUS_GET_PROBLEM		= 6,
		 ePNDEV_FLASH_NOR_STATUS_VERIFY_ERROR			= 7,
		 ePNDEV_FLASH_NOR_STATUS_BYTES_PER_OP_WRONG		= 8,
		 ePNDEV_FLASH_NOR_STATUS_ERASE_ERROR			= 9,
		 ePNDEV_FLASH_NOR_STATUS_PROGRAM_ERROR			= 10,
		 ePNDEV_FLASH_NOR_STATUS_SECTOR_LOCK			= 11,
		 ePNDEV_FLASH_NOR_STATUS_PROGRAM_SUSPEND		= 12,					// device is in program suspend mode
		 ePNDEV_FLASH_NOR_STATUS_PROGRAM_SUSPEND_ERROR	= 13,					// device program suspend error
		 ePNDEV_FLASH_NOR_STATUS_ERASE_SUSPEND			= 14,					// device is in erase suspend mode
		 ePNDEV_FLASH_NOR_STATUS_ERASE_SUSPEND_ERROR	= 15,					// device erase suspend error
		 ePNDEV_FLASH_NOR_STATUS_BUSY_IN_OTHER_BANK		= 16					// busy operation in other bank
	} 
	ePNDEV_FLASH_NOR_STATUS;

	//------------------------------------------------------------------------
	// Spansion flash: see lld.h
	//	- offset at chip
	#define FLASH_NOR_OFFSET__UNLOCK1			0x00000555
	#define FLASH_NOR_OFFSET__UNLOCK2			0x000002AA

	#define FLASH_NOR_OFFSET__RESET				0x00000000

	#define FLASH_NOR_OFFSET__MANUFACTURE_ID	0x00000000
	#define FLASH_NOR_OFFSET__DEVICE_ID			0x00000001

	//------------------------------------------------------------------------
	// Spansion flash: see lld.h
	#define FLASH_NOR_CMD_X8__CFI_QUERY					0x98
	#define FLASH_NOR_CMD_X8__CHIP_ERASE				0x10
	#define FLASH_NOR_CMD_X8__ERASE_SETUP				0x80
	#define FLASH_NOR_CMD_X8__RESET						0xF0
	#define FLASH_NOR_CMD_X8__SECSI_SECTOR_ENTRY		0x88
	#define FLASH_NOR_CMD_X8__SECTOR_ERASE				0x30
	#define FLASH_NOR_CMD_X8__WRITE_BUFFER_LOAD			0x25
	#define FLASH_NOR_CMD_X8__WRITE_BUFFER_PGM_CONFIRM	0x29 
	#define FLASH_NOR_CMD_X8__SET_CONFIG				0xD0
	#define FLASH_NOR_CMD_X8__BIT_FIELD					0xBF

	#define FLASH_NOR_CMD_X8__ERASE_SUSPEND				0xB0
	#define FLASH_NOR_CMD_X8__ERASE_RESUME				0x30
	#define FLASH_NOR_CMD_X8__PROGRAM_SUSPEND			0x51
	#define FLASH_NOR_CMD_X8__PROGRAM_RESUME			0x50
	#define FLASH_NOR_CMD_X8__STATUS_REG_READ			0x70
	#define FLASH_NOR_CMD_X8__STATUS_REG_CLEAR			0x71
	#define FLASH_NOR_CMD_X8__BLANK_CHECK				0x33

	#define FLASH_NOR_CMD_X8__AUTOSELECT				0x90
	#define FLASH_NOR_CMD_X8__PROGRAM					0xA0
	#define FLASH_NOR_CMD_X8__SECSI_SECTOR_EXIT_SETUP	0x90
	#define FLASH_NOR_CMD_X8__SECSI_SECTOR_EXIT			0x00
	#define FLASH_NOR_CMD_X8__UNLOCK_BYPASS_ENTRY		0x20
	#define FLASH_NOR_CMD_X8__UNLOCK_BYPASS_PROGRAM		0xA0
	#define FLASH_NOR_CMD_X8__UNLOCK_BYPASS_RESET1		0x90
	#define FLASH_NOR_CMD_X8__UNLOCK_BYPASS_RESET2		0x00
	#define FLASH_NOR_CMD_X8__UNLOCK_DATA1				0xAA
	#define FLASH_NOR_CMD_X8__UNLOCK_DATA2				0x55
	#define FLASH_NOR_CMD_X8__SUSPEND					0xB0
	#define FLASH_NOR_CMD_X8__RESUME					0x30
	#define FLASH_NOR_CMD_X8__READ_CONFIG				0xC6
	#define FLASH_NOR_CMD_X8__WRITE_BUFFER_ABORT_RESET	0xF0

	//------------------------------------------------------------------------
	// Spansion flash: see lld.h
	#define FLASH_NOR_MASK_X8__DQ1	0x02
	#define FLASH_NOR_MASK_X8__DQ2	0x04
	#define FLASH_NOR_MASK_X8__DQ5	0x20
	#define FLASH_NOR_MASK_X8__DQ6	0x40
	#define FLASH_NOR_MASK_X8__DQ7	0x80

	//========================================================================
	//	NAND flash
	//========================================================================

	#define FLASH_NAND_CMD_X8__RESET		0xff
	#define FLASH_NAND_CMD_X8__READ			0x00
	#define FLASH_NAND_CMD_X8__SYNC			0x30
	#define FLASH_NAND_CMD_X8__GET_ID		0x90
	#define FLASH_NAND_CMD_X8__ERASE1		0x60
	#define FLASH_NAND_CMD_X8__ERASE2		0xD0
	#define FLASH_NAND_CMD_X8__STATUS_READ	0x70
	#define FLASH_NAND_CMD_X8__PROGRAM1		0x80
	#define FLASH_NAND_CMD_X8__PROGRAM2		0x10

	//------------------------------------------------------------------------
	#define FLASH_NAND_STATUS__PROGRAM_ERASE_FINISHED	0x01
	#define FLASH_NAND_STATUS__PROGRAM_ERASE_PASSED		0x00

	//------------------------------------------------------------------------

	#define FLASH_NAND_STATUS__ERASE_FINISHED	0x01
	#define FLASH_NAND_STATUS__ERASE_PASSED		0x00

	//------------------------------------------------------------------------
	// ReedSolomonCode 
	#define FLASH_NAND_RSC__NO_ERROR	0x0000						
	#define FLASH_NAND_RSC__ONE_ERROR	0x0001
	#define FLASH_NAND_RSC__TWO_ERROR	0x0002
	#define FLASH_NAND_RSC__THREE_ERROR	0x0003
	#define FLASH_NAND_RSC__FOUR_ERROR	0x0004
	#define FLASH_NAND_RSC__OTHER_ERROR	0xffff

	#define FLASH_NAND_RSC__GEN_POL_0	0x15
	#define FLASH_NAND_RSC__GEN_POL_1	0xb5
	#define FLASH_NAND_RSC__GEN_POL_2	0x09
	#define FLASH_NAND_RSC__GEN_POL_3	0x89
	#define FLASH_NAND_RSC__GEN_POL_4	0x02
	#define FLASH_NAND_RSC__GEN_POL_5	0xa7

	//########################################################################
	//  Structures
	//########################################################################

	typedef struct _uPNDEV_FLASH_NAND_PAR
	{
		UINT32			lSizePage;
		UINT32			lSizeRscData;
		UINT32			lSizeRscCheck;
		UINT8			lOffsetDataReg;
		UINT8			lOffsetCmdReg;
		UINT8			lOffsetAdrReg;
		volatile UINT8*	pPollReg;
	}
	uPNDEV_FLASH_NAND_PAR;

    typedef struct _uPNDEV_FLASH_SPI_PAR
	{
        UINT32			lSizePage;
        UINT32          lAddressMask;
        BOOLEAN         bIsSSPInitialized;
	}
	uPNDEV_FLASH_SPI_PAR;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_FLASH_PAR
	{
		volatile UINT8*			pFlash;
		ePNDEV_FLASH_MODE		eFlashMode;
		ePNDEV_FLASH_TYPE		eFlashTypePos[PNDEV_DIM_ARRAY_FLASH_TYPE];		// possible FlashTypes
		ePNDEV_FLASH_TYPE		eFlashTypeReal;
		ePNDEV_FLASH_INFO		eFlashInfo;
		UINT32					lDataMultiplier;
		UINT32					lDataWidth;
		UINT32					lAdrMultiplierNor;
		UINT32					lSizeUsable;									// usable size of chip
		UINT32					lSizeSector;									// NOR:  SectorSize
																				// NAND: BlockSize
		uPNDEV_FLASH_NAND_PAR	uNand;
        uPNDEV_FLASH_SPI_PAR	uSPI;
	}
	uPNDEV_FLASH_PAR;

	//========================================================================
	//	non CP1616 boards
	//========================================================================

	typedef struct _uPNDEV_FLASH_DATA_V1										// physical structure of RemanentData stored at flash
	{
		ePNDEV_FLASH_VERSION	eVersion;										// must be first parameter, type must not be changed!
		UINT32					lCtrMacAdr;
		UINT64					lMacAdr64;
		UINT8					lArrayPrivate[PNCORE_DIM_ARRAY_REMANENT_PRIVATE];
	}
	uPNDEV_FLASH_DATA_V1;

	//------------------------------------------------------------------------
	//typedef synonym;
	typedef	uPNDEV_FLASH_DATA_V1 uPNDEV_FLASH_DATA;								// current structure for writing

	//========================================================================
	//	CP1616 board
	//========================================================================

	typedef struct _uAM_VERSION_BOOT
	{
		UINT8	uByte[24];
	}
	uAM_VERSION_BOOT;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_FLASH_DATA_CP1616									// physical structure of RemanentData stored at flash
	{
		UINT8				uBootLine[255];
		UINT8				uReserved1[4];
		UINT8				uMacAdr0[6];
		UINT8				uMlfb[32];
		UINT8				uSerialNumber[16];
		uAM_VERSION_BOOT	uBootVersion;
		UINT8				lCtrMacAdr;											// 0 or 0xff: 2 MacAdr
		UINT8				bAllMacAdrStored;									// always FALSE: only first MacAdr is stored
		UINT8				uMacAdr1[6];
		UINT8				uMacAdr2[6];
		UINT8				uMacAdr3[6];
		UINT8				uMacAdr4[6];
		UINT8				uMacAdr5[6];
		UINT8				lHwVersionMajor;
		UINT8				lHwVersionMinor;									// not used
		UINT8				uReserved2[70];
		UINT8				uReserved3;											// added because of alignment 
																				//	- if size is not a multiple of UINT16 then a undefined value will be programmed!
		UINT8				lArrayPrivate[PNCORE_DIM_ARRAY_REMANENT_PRIVATE];
	}
	uPNDEV_FLASH_DATA_CP1616;

#endif
