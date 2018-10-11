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
/*  F i l e               &F: PnCore.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for Core-IF (HostCore-AsicCore)
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __PnCore_h__
#define __PnCore_h__

	#ifdef __cplusplus		// if C++ compiler: Use C linkage
	extern "C"
	{
	#endif

	//########################################################################
	//  Defines
	//########################################################################

	#define PNCORE_REG32_READ( pBaseIn, lOffsetIn)				*((volatile LSA_UINT32*) (((volatile LSA_UINT8*) pBaseIn) + lOffsetIn))
	#define	PNCORE_REG32_WRITE(pBaseIn, lOffsetIn, lValueIn)	*((volatile LSA_UINT32*) (((volatile LSA_UINT8*) pBaseIn) + lOffsetIn)) = lValueIn

	//------------------------------------------------------------------------
	#define	PNCORE_SIZE_STRING_BUF	260											// size of buffer for Unicode characters inclusive terminal NULL (corresponding to PNDEV_SIZE_STRING_BUF)

	//------------------------------------------------------------------------
	#define	PNCORE_MAC_ADR_MAX					32
	#define	PNCORE_DIM_ARRAY_REMANENT_PRIVATE	1024
	#define PNCORE_DIM_ARRAY_USER_DATA			128 

	//------------------------------------------------------------------------
	#define	PNCORE_SHARED_ASIC_SDRAM__SIZE_RESERVED		(1024*1024*2)

	#define	PNCORE_SHARED_ASIC_SDRAM__SIZE_VECTOR		(1024*1)
	#define	PNCORE_SHARED_ASIC_SDRAM__SIZE_BOOT			(1024*1)
	#define	PNCORE_SHARED_ASIC_SDRAM__SIZE_CONFIG		(1024*2)
	#define	PNCORE_SHARED_ASIC_SDRAM__SIZE_INDIRECT0	(PNCORE_SHARED_ASIC_SDRAM__SIZE_RESERVED - (PNCORE_SHARED_ASIC_SDRAM__SIZE_VECTOR + PNCORE_SHARED_ASIC_SDRAM__SIZE_BOOT + PNCORE_SHARED_ASIC_SDRAM__SIZE_CONFIG))

	#define	PNCORE_SHARED_ASIC_SDRAM__OFFSET_VECTOR		0
	#define	PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT		(PNCORE_SHARED_ASIC_SDRAM__SIZE_VECTOR)
	#define	PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG		(PNCORE_SHARED_ASIC_SDRAM__SIZE_VECTOR + PNCORE_SHARED_ASIC_SDRAM__SIZE_BOOT)
	#define	PNCORE_SHARED_ASIC_SDRAM__OFFSET_INDIRECT0	(PNCORE_SHARED_ASIC_SDRAM__SIZE_VECTOR + PNCORE_SHARED_ASIC_SDRAM__SIZE_BOOT + PNCORE_SHARED_ASIC_SDRAM__SIZE_CONFIG)
	#define	PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT		PNCORE_SHARED_ASIC_SDRAM__SIZE_RESERVED

	//------------------------------------------------------------------------
	#define PNCORE_SOC1_PCI_BRIDGE_REG__ADR 					0x1D500000
	#define	PNCORE_ERTEC400_PCI_BRIDGE_REG__ADR					0x80000000
		
	#define PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_EN_CNF_PCI_SIDE	0xFC
	#define PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_AHB_STAT_FUNC		0xCC
		
	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAR2				0x98
	#define PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAM2				0xAC
	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAT2				0xC0

	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAR3				0x9C
	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAM3				0xB0
	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAT3				0xC4

	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAR4				0xA0
	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAM4				0xB4
	#define	PNCORE_AHB_PCI_BRIDGE_REG__OFFSET_BAT4				0xC8
		
	#define PNCORE_AHB_PCI_BRIDGE_REG__BAT3_DEFAULT				0x30000000
	#define PNCORE_AHB_PCI_BRIDGE_REG__BAT3_ALIGNMENT			(256*1024*1024)	

	#define	PNCORE_AHB_PCI_MASTER1_SOC1_BAR						0xC0000000
	#define	PNCORE_AHB_PCI_MASTER1_SOC1_BAT						0x0
	#define	PNCORE_AHB_PCI_MASTER1_SOC1_SIZE					(512*1024*1024)		
	#define	PNCORE_AHB_PCI_MASTER2_SOC1_BAR						0xE0000000
	#define	PNCORE_AHB_PCI_MASTER2_SOC1_BAT						0x20000000
	#define	PNCORE_AHB_PCI_MASTER2_SOC1_SIZE					(256*1024*1024)		
	#define	PNCORE_AHB_PCI_MASTER3_SOC1_BAR						0xF0000000
	#define	PNCORE_AHB_PCI_MASTER3_SOC1_BAT						0x30000000
	#define	PNCORE_AHB_PCI_MASTER3_SOC1_SIZE					(256*1024*1024)
		
	#define	PNCORE_AHB_PCI_MASTER1_ERTEC400_BAR					0xC0000000
	#define	PNCORE_AHB_PCI_MASTER1_ERTEC400_BAT					0x0
	#define	PNCORE_AHB_PCI_MASTER1_ERTEC400_SIZE				(512*1024*1024)		
	#define	PNCORE_AHB_PCI_MASTER2_ERTEC400_BAR					0xE0000000
	#define	PNCORE_AHB_PCI_MASTER2_ERTEC400_BAT					0x20000000
	#define	PNCORE_AHB_PCI_MASTER2_ERTEC400_SIZE				(256*1024*1024)		
	#define	PNCORE_AHB_PCI_MASTER3_ERTEC400_BAR					0xF0000000
	#define	PNCORE_AHB_PCI_MASTER3_ERTEC400_BAT					0x30000000
	#define	PNCORE_AHB_PCI_MASTER3_ERTEC400_SIZE				(256*1024*1024)
		
	//------------------------------------------------------------------------
	typedef enum _ePNCORE_ASIC
	{
		ePNCORE_ASIC_INVALID	= 0,
		ePNCORE_ASIC_ERTEC400	= 1,
		ePNCORE_ASIC_ERTEC200	= 2,
		ePNCORE_ASIC_SOC1		= 3,
		ePNCORE_ASIC_ERTEC200P	= 4,
		ePNCORE_ASIC_I210		= 5,
		ePNCORE_ASIC_I82574		= 6,
		ePNCORE_ASIC_HERA		= 7,
		ePNCORE_ASIC_KSZ8841	= 8,
		ePNCORE_ASIC_KSZ8842	= 9,
		ePNCORE_ASIC_AM5728		= 10
	}
	ePNCORE_ASIC;

	//------------------------------------------------------------------------
	typedef enum _ePNCORE_ICU
	{
		ePNCORE_ICU_INVALID	= 0,
		ePNCORE_ICU_HOST	= 1,
		ePNCORE_ICU_ASIC	= 2
	}
	ePNCORE_ICU;
	
	//------------------------------------------------------------------------
	typedef enum _ePNCORE_MASK_STATE
	{
		ePNCORE_MASK_STATE_INVALID	= 0,
		ePNCORE_MASK_STATE_0		= 1,
		ePNCORE_MASK_STATE_1		= 2
	}
	ePNCORE_MASK_STATE;

	//########################################################################
	//  Structures
	//########################################################################

	typedef	struct _uPNCORE_REMANENT_DATA
	{
		LSA_UINT32	lCtrMacAdr;
		LSA_UINT8	lArrayMacAdr [PNCORE_MAC_ADR_MAX][6];
		LSA_UINT8	lArrayPrivate[PNCORE_DIM_ARRAY_REMANENT_PRIVATE];
	}
	uPNCORE_REMANENT_DATA;
	
	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_SHARED_HOST_SDRAM
	{
		UINT32	lSize;
		UINT32	lPhyAdr;
	}
	uPNCORE_SHARED_HOST_SDRAM;	
	
	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_PCI_HOST_PAR 
	{
		LSA_UINT32				lBarHost;
		LSA_UINT32				lBatPciHost;
		LSA_UINT32				lSizePciHost;
	}
	uPNCORE_PCI_HOST_PAR;

	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_HW_BLOCK_DESC 
	{
		LSA_UINT32	lOffset;
		LSA_UINT32	lSize;
		LSA_UINT32	lDummy[2];
	}
	uPNCORE_HW_BLOCK_DESC;

	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_SIGN_OF_LIFE 
	{
		LSA_UINT32	bAlive;														// own SignOfLife
		LSA_UINT32	lCtrCycleCur;												// current number of cycles
		LSA_UINT32	lCtrCycleActivePhase;										// after this number of cycles own SignOfLife is checked by partner
		LSA_UINT32	lDummy;
	}
	uPNCORE_SIGN_OF_LIFE;

	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_STARTUP_PAR_FW 
	{
		LSA_UINT32				lTimeUpdateSignOfLife_10msec;					// TimeInterval for a guaranteed update of SignOfLife of UserFw
		LSA_UINT32				lSizeIndirect1;
		LSA_UINT32				lDummy[2];

		uPNCORE_HW_BLOCK_DESC	uHif;
		uPNCORE_HW_BLOCK_DESC	uFatal;
		uPNCORE_HW_BLOCK_DESC	uTrace[2];
	}
	uPNCORE_STARTUP_PAR_FW;

	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_STARTUP_PAR_DRIVER 
	{
		LSA_UINT32					lTimeUpdateSignOfLife_10msec;					// TimeInterval for a guaranteed update of SignOfLife of driver
		LSA_UINT32					bShmHaltOnStartup;
		LSA_UINT32					lDummy[2];
		LSA_UINT32					lArrayUserData[PNCORE_DIM_ARRAY_USER_DATA];
		
		uPNCORE_PCI_HOST_PAR		uPciMaster[3];
		uPNCORE_SHARED_HOST_SDRAM	uSharedHostSdram;
		uPNCORE_REMANENT_DATA		uRemanentData;
	}
	uPNCORE_STARTUP_PAR_DRIVER;

	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_CONFIG_PAR_FW 
	{
		LSA_UINT32				bStartupParValid;
		LSA_UINT32				bReady;
		LSA_UINT32				lFatal;											// 0: no error
		LSA_UINT32				lDummy;
		
		uPNCORE_PCI_HOST_PAR	uPciMaster[3];
		uPNCORE_STARTUP_PAR_FW	uStartupPar;
	}
	uPNCORE_CONFIG_PAR_FW;

	//------------------------------------------------------------------------
	typedef	struct _uPNCORE_CONFIG_PAR_DRIVER 
	{
		LSA_UINT32					bReady;
		LSA_UINT32					lDummy[3];

		uPNCORE_STARTUP_PAR_DRIVER	uStartupPar;
	}
	uPNCORE_CONFIG_PAR_DRIVER;

	//------------------------------------------------------------------------
	typedef struct _uPNCORE_CONFIG_PAR
	{
		LSA_UINT32					bAckDebugger;
		LSA_UINT32					bDebugMode;
		LSA_UINT32					bCancel;
		LSA_UINT32					lTimeStartupSignOfLife_10msec;
		LSA_UINT32					lOffsetBridgeConfigReg;
		LSA_UINT32					lBatHostAddress;

		uPNCORE_SIGN_OF_LIFE		uSignOfLifeFw;
		uPNCORE_SIGN_OF_LIFE		uSignOfLifeDriver;

		uPNCORE_CONFIG_PAR_FW		uFw;
		uPNCORE_CONFIG_PAR_DRIVER	uDriver;
	}
	uPNCORE_CONFIG_PAR;

	//------------------------------------------------------------------------
	typedef struct _uPNCORE_INT_PAR
	{
		LSA_UINT32			lOffsetIrrReg;
		LSA_UINT32			lOffsetAckReg;
		LSA_UINT32			lOffsetMaskReg;
		LSA_UINT32			lOffsetIrReg;
		LSA_UINT32			lOffsetEoiReg;
		LSA_UINT32			lBitShift;
		ePNCORE_MASK_STATE	eMaskState;
	}
	uPNCORE_INT_PAR;	

	//########################################################################
	//  Function prototypes
	//########################################################################

	LSA_BOOL	fnPnCore_CheckDebugMode(		volatile LSA_UINT8*			pAsicSdramIn);
	LSA_BOOL 	fnPnCore_ExchangeStartupPar(	volatile LSA_UINT8*			pAsicSdramIn,
												uPNCORE_STARTUP_PAR_FW*		pStartupParFwIn,
												uPNCORE_STARTUP_PAR_DRIVER*	pStartupParDriverOut);
	LSA_BOOL	fnPnCore_ExchangeStartupState(	volatile LSA_UINT8*			pAsicSdramIn);
	void		fnPnCore_SetSignOfLife(			volatile LSA_UINT8*			pAsicSdramIn,
												volatile LSA_BOOL			bCallByUserFwIn);
	LSA_BOOL	fnPnCore_CheckSignOfLife(		volatile LSA_UINT8*			pAsicSdramIn,
												volatile LSA_UINT32			lTimeCycle_10msecIn,
												volatile LSA_BOOL			bCallByUserFwIn);
	void		fnPnCore_SetFatal(				volatile LSA_UINT8*			pAsicSdramIn,
												LSA_UINT32					lFatalIn);
	LSA_BOOL	fnPnCore_GetIntPar(				ePNCORE_ASIC				eAsicIn,
												ePNCORE_ICU					eIcuIn,
												uPNCORE_INT_PAR*			pIntParOut);
	void		fnPnCore_MaskInt(				volatile LSA_UINT8*			pIrtePnipIn,	
												uPNCORE_INT_PAR*			pIntParIn);
	void		fnPnCore_UnmaskInt(				volatile LSA_UINT8*			pIrtePnipIn,	
												uPNCORE_INT_PAR*			pIntParIn);
	void		fnPnCore_AckInt(				volatile LSA_UINT8*			pIrtePnipIn,	
												uPNCORE_INT_PAR*			pIntParIn);
	void		fnPnCore_SetInt(				volatile LSA_UINT8*			pIrtePnipIn,	
												uPNCORE_INT_PAR*			pIntParIn);
	void		fnPnCore_SetEoi(				volatile LSA_UINT8*			pIrtePnipIn,	
												uPNCORE_INT_PAR*			pIntParIn,
												LSA_UINT32					lInactiveTime);
	LSA_BOOL	fnPnCore_CheckIntIr(			volatile LSA_UINT8*			pIrtePnipIn,	
												uPNCORE_INT_PAR*			pIntParIn);
	LSA_BOOL	fnPnCore_CheckIntIrr(			volatile LSA_UINT8*			pIrtePnipIn,	
												uPNCORE_INT_PAR*			pIntParIn);

	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ compiler: End of C linkage
	}
	#endif

#endif
