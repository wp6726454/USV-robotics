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
/*  F i l e               &F: os.c                                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//  Code for adaption of SYS to used OS
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date        Who             What
//  xxxx-xx-xx  Spichtinger     xx
/*****************************************************************************/

#ifndef __pndev_np_common_h__
#define __pndev_np_common_h__

//########################################################################
//  Literallies
//########################################################################
#define kNP_LEN_MAX_STRING		 260						// -> buffer length must be (kNP_ONL_STR_LEN_MAX + 1) because of 0-termination
															// (= value of Microsoft-Define "MAX_PATH")
#define	kNP_MAX_ENTRIES_FDB		 16							// maximal number of MacAdr for FilterDa
#define	kNP_MAX_NO_SA			 16							// maximal number of MacAdr for FilterSa
#define	kNP_MAX_NO_PORTS		 4						    // maximal number of Ethernet ports of Asic
#define	kNP_FILTER_SW_BYPASS	 0xffff						// bypass current SwFilter

#define kNP_MAX_ENTRIES_PROTOCOL 55							// maximal number of entries of ProtocolArray (MaxIndex+1)

	// Attention:
	//	- Sometimes a Boolean value was not copied correctly at interface Managed-/Unmanaged code (e.g. OutputParameter of IntTest).
	//	- This happened only if the complete structure was copied at DLL (pOnlParOut->IntTest = uDriverParOut.uAs.uIntTest).
	//	- It doesn't happen if single elements were copied (pOnlParOut->IntTest.bOverflow = uDriverParOut.uAs.uIntTest.bOverflow).
	//	- At DLL 'bOverflow' was always correct.
	//	- 'bOverflow' was only wrong at C#
	//	=>	size of Boolean of C# is incompatible to size of Boolean of C++
	//	=>	definition of a private eNP_BOOL

	// Attention:
	//	- at C++ FALSE is 0, this ReturnValue was interpreted as true by C#
	//	=>	definition of a private eNP_BOOL

	typedef enum _eNP_BOOL
	{
		eNP_FALSE	= 0,
		eNP_TRUE	= 1
	}
	eNP_BOOL;

//------------------------------------------------------------------------
//	common
//------------------------------------------------------------------------

	struct uAS_UINT8
	{
		UINT8	lLow;
		UINT8	lHigh;
	};
//------------------------------------------------------------------------
	struct uAS_UINT16
	{
		UINT16	lLow;
		UINT16	lHigh;
	};
//------------------------------------------------------------------------
	struct uAS_UINT32
	{
		UINT32	lLow;
		UINT32	lHigh;
	};
//------------------------------------------------------------------------
	union uACCESS_UINT16
	{
		UINT16				uAs_UInt16;
		struct	uAS_UINT8	uAs_UInt8;
	};
//------------------------------------------------------------------------
	union uACCESS_UINT32
	{
		UINT32				uAs_UInt32;
		struct	uAS_UINT16	uAs_UInt16;
	};
//------------------------------------------------------------------------
	typedef union _uACCESS_UINT64
	{
		UINT64				uAs_UInt64;
		struct	uAS_UINT32	uAs_UInt32;
	}uACCESS_UINT64;

    typedef	union _uNP64_PTR_VOID
	{
		void*	pPtr;
		UINT64	lUint64;
	}
	uNP64_PTR_VOID;

    typedef	union _uNP64_HANDLE
	{
		HANDLE	hHandle;
		UINT64	lUint64;
	}
	uNP64_HANDLE;

    typedef enum _eNP_FRAME_NOTE
	{
		// Attention: The following values must not use ErrorBits 31..21 (see below)

		eNP_NOTE_INVALID					= 0,

		eNP_NOTE_PnPtcpSync_CycleWithoutFu	= 1,
		eNP_NOTE_PnPtcpSync_CycleWithFu		= 2,
		eNP_NOTE_PnPtcpSync_CycleFu			= 3,
		eNP_NOTE_PnPtcpSync_CycleAnnounce	= 4,
		eNP_NOTE_PnPtcpSync_TimeWithoutFu	= 5,
		eNP_NOTE_PnPtcpSync_TimeWithFu		= 6,
		eNP_NOTE_PnPtcpSync_TimeFu			= 7,
		eNP_NOTE_PnPtcpSync_TimeAnnounce	= 8,
		eNP_NOTE_PnPtcpDelay_Req			= 9,
		eNP_NOTE_PnPtcpDelay_ResWithoutFu	= 10,
		eNP_NOTE_PnPtcpDelay_ResWithFu		= 11,
		eNP_NOTE_PnPtcpDelay_ResFu			= 12,
		eNP_NOTE_PnRtc3_Red_UcMc			= 13,
		eNP_NOTE_PnRtc2_Red_UcMc			= 14,
		eNP_NOTE_PnRtc2_Orange_UcMc			= 15,
		eNP_NOTE_PnRtc1_Uc					= 16,
		eNP_NOTE_PnRtc1_Mc					= 17,
		eNP_NOTE_PnAlarmHigh				= 18,
		eNP_NOTE_PnAlarmLow					= 19,
		eNP_NOTE_PnDcp_Hello				= 20,
		eNP_NOTE_PnDcp_GetSet				= 21,
		eNP_NOTE_PnDcp_IdentifyReq			= 22,
		eNP_NOTE_PnDcp_IdentifyRes			= 23,
		eNP_NOTE_PnFragmentation			= 24,
		eNP_NOTE_PnInvalidFrameId			= 25,

		eNP_NOTE_Arp						= 26,
		eNP_NOTE_Lldp						= 27,
		eNP_NOTE_Mrp						= 28,
		eNP_NOTE_IpIcmp						= 29,
		eNP_NOTE_IpTcp						= 30,
		eNP_NOTE_IpUdp_Standard				= 31,							// IP-UDP Standard (not PN)
		eNP_NOTE_IpUdp_PnAlarmHigh			= 32,
		eNP_NOTE_IpUdp_PnAlarmLow			= 33,
		eNP_NOTE_IpUdp_PnRtc1Uc				= 34,
		eNP_NOTE_IpUdp_PnRtc1Mc				= 35,
		eNP_NOTE_IpOtherThanIcmpTcpUdp		= 36,							// IP but not ICMP/TCP/UDP

		eNP_NOTE_OtherThanArpIpLldpMrpPn	= 37,							// not ARP/IP/LLDP/MRP/PN
//------------------------------------------------------------------------
		// Bits which are set additionally to above values
		eNP_NOTE_BIT31_WithVlan								= (1<<31),		// Bit 31
		eNP_NOTE_BIT30_PnVlanMissing						= (1<<30),		// Bit 30
		eNP_NOTE_BIT29_PnTransferStatus_ErrorAlignChecksum	= (1<<29),		// Bit 29
		eNP_NOTE_BIT28_PnTransferStatus_ErrorLen			= (1<<28),		// Bit 28
		eNP_NOTE_BIT27_PnTransferStatus_OverflowMacRcveBuf	= (1<<27),		// Bit 27
		eNP_NOTE_BIT26_PnTransferStatus_ErrorRtc3_Red		= (1<<26),		// Bit 26
		eNP_NOTE_BIT25_PnDataStatus_IocrStateBackup			= (1<<25),		// Bit 25
		eNP_NOTE_BIT24_PnDataStatus_InvalidDataItem			= (1<<24),		// Bit 24
		eNP_NOTE_BIT23_PnDataStatus_ProviderStateStop		= (1<<23),		// Bit 23
		eNP_NOTE_BIT22_PnDataStatus_StationProblem			= (1<<22)		// Bit 22
	}
	eNP_FRAME_NOTE;

#define kNP_MAX_ENTRIES_PROTOCOL 55									// maximal number of entries of ProtocolArray (MaxIndex+1)

	// Attention:
	//	- Enums are used for XML Profile too
	//		FilterProtocol stored at array (look to clsCaptureParAdvanced)
	//		-> don't change values!
	//		-> reserve a value if an item is removed!
	//	- kNP_MAX_ENTRIES_PROTOCOL must not be smaller than at former versions
	//		otherwise cloning causes an exception because a larger array of a former version is copied to a smaller array of current version
	//		(search for m_bArrayFilterProtocol.Length)

	typedef enum _eNP_PROTOCOL_ID
	{
		eNP_PCOL_Invalid								= 0,

		eNP_PCOL_Pn										= 1,					// if used as CounterIndex:
		eNP_PCOL_Arp									= 2,					//	- only 1 of the following ArrayItem can be set by a frame (-> exclusive set)
		eNP_PCOL_Lldp									= 3,
		eNP_PCOL_Mrp									= 4,
		eNP_PCOL_IpIcmp									= 5,
		eNP_PCOL_IpTcp									= 6,
		eNP_PCOL_IpUdp_Standard							= 7,
		eNP_PCOL_IpUdp_PnAlarmHigh						= 8,
		eNP_PCOL_IpUdp_PnAlarmLow						= 9,
		eNP_PCOL_IpUdp_PnRtc1Uc							= 10,
		eNP_PCOL_IpUdp_PnRtc1Mc							= 11,
		eNP_PCOL_IpOtherThanIcmpTcpUdp					= 12,
		eNP_PCOL_OtherThanArpIpLldpMrpPn				= 13,
		eNP_PCOL_PnPtcpSync_CycleWithoutFu				= 14,
		eNP_PCOL_PnPtcpSync_CycleWithFu					= 15,
		eNP_PCOL_PnPtcpSync_CycleFu						= 16,
		eNP_PCOL_PnPtcpSync_CycleAnnounce				= 17,
		eNP_PCOL_PnPtcpSync_TimeWithoutFu				= 18,
		eNP_PCOL_PnPtcpSync_TimeWithFu					= 19,
		eNP_PCOL_PnPtcpSync_TimeFu						= 20,
		eNP_PCOL_PnPtcpSync_TimeAnnounce				= 21,
		eNP_PCOL_PnPtcpDelay_Req						= 22,
		eNP_PCOL_PnPtcpDelay_ResWithoutFu				= 23,
		eNP_PCOL_PnPtcpDelay_ResWithFu					= 24,
		eNP_PCOL_PnPtcpDelay_ResFu						= 25,
		eNP_PCOL_PnFragmentation						= 26,
		eNP_PCOL_PnAlarmHigh							= 27,
		eNP_PCOL_PnAlarmLow								= 28,
		eNP_PCOL_PnDcp_Hello							= 29,
		eNP_PCOL_PnDcp_GetSet							= 30,
		eNP_PCOL_PnDcp_IdentifyReq						= 31,
		eNP_PCOL_PnDcp_IdentifyRes						= 32,
		eNP_PCOL_PnRtc3_Red_UcMc						= 33,
		eNP_PCOL_PnRtc2_Red_UcMc						= 34,
		eNP_PCOL_PnRtc2_Orange_UcMc						= 35,
		eNP_PCOL_PnRtc1_Uc								= 36,
		eNP_PCOL_PnRtc1_Mc								= 37,
		eNP_PCOL_PnInvalidFrameId						= 38,

		eNP_PCOL_Broadcast								= 39,				// if used as ProtocolCounter:
		eNP_PCOL_WithVlan								= 40,				//	- several of the following ArrayItems can be set additionally by the same frame
		eNP_PCOL_WithoutVlan							= 41,
		eNP_PCOL_PnVlanMissing							= 42,
		eNP_PCOL_PnReserved1							= 43,
		eNP_PCOL_PnTransferStatus_ErrorAlignChecksum	= 44,
		eNP_PCOL_PnTransferStatus_ErrorLen				= 45,
		eNP_PCOL_PnTransferStatus_OverflowMacRcveBuf	= 46,
		eNP_PCOL_PnTransferStatus_ErrorRtc3_Red			= 47,
		eNP_PCOL_PnDataStatus_IocrStateBackup			= 48,
		eNP_PCOL_PnDataStatus_InvalidDataItem			= 49,
		eNP_PCOL_PnDataStatus_ProviderStateStop			= 50,
		eNP_PCOL_PnDataStatus_StationProblem			= 51,
		eNP_PCOL_BeforeFilterProtocol					= 52,				// ArrayItem irrelevant if used as FilterProtocol
		eNP_PCOL_AfterFilterProtocol					= 53,				// ArrayItem irrelevant if used as FilterProtocol
		eNP_PCOL_DataFile								= 54				// ArrayItem irrelevant if used as FilterProtocol
	}
	eNP_PROTOCOL_ID;

    typedef enum _eNP_CODE_SERVICE
	{
		eNP_SERVICE_INVALID		= 0,
		eNP_SERVICE_STARTUP		= 1,
		eNP_SERVICE_SHUT_DOWN	= 2,
		//eAM_SERVICE_INT_TEST	= 3,
		eNP_SERVICE_CAPTURE		= 4
		//eAM_SERVICE_GET_DATA	= 5
	}
	eNP_CODE_SERVICE;

    typedef enum _eNP_MODE_CAPTURE
	{
		eNP_MODE_CAPTURE_INVALID	= 0,
		eNP_MODE_CAPTURE_START		= 1,
		eNP_MODE_CAPTURE_STOP		= 2,
		eNP_MODE_CAPTURE_GET_INFO	= 3
	}
	eNP_MODE_CAPTURE;

	// LongEnable=0:
	//		without VLAN:	FrameLen <= 1518 (incl. CRC): 6+6+0+2+2+Data+CRC -> Data <= 1498
	//		with VLAN:		FrameLen <= 1522 (incl. CRC): 6+6+4+2+2+Data+CRC -> Data <= 1498
	// LongEnable=1:		FrameLen <= 1536 (incl. CRC): 6+6+4+2+2+Data+CRC -> Data <= 1512

	// Attention:
	//		There is no fixed gap for VLAN. The buffer is filled like the stream on the bus

	struct uAS_FRAME_HEADER
	{
		UINT8	lArrayMacAdrDa[6];
		UINT8	lArrayMacAdrSa[6];
		UINT8	lArrayVlanTag[4];
	};

    struct uAS_FRAME_NO_VLAN												// FrameLen <= 1536
	{
		UINT8	lArrayMacAdrDa[6];
		UINT8	lArrayMacAdrSa[6];
		UINT8	lArrayEtherType[2];
		UINT8	lArrayFrameId[2];
		UINT8	lArrayData[1520];
	};

    struct uAS_FRAME_VLAN													// FrameLen <= 1536
	{
		UINT8	lArrayMacAdrDa[6];
		UINT8	lArrayMacAdrSa[6];
		UINT8	lArrayVlanType[2];
		UINT8	lArrayVlanTag[2];
		UINT8	lArrayEtherType[2];
		UINT8	lArrayFrameId[2];
		UINT8	lArrayData[1516];
	};

    typedef struct _uNP_FRAME_INFO
	{
		uACCESS_UINT64	lMacAdrDa;                                  /**< Destination MAC address */
		uACCESS_UINT64	lMacAdrSa;                                  /**< Source MAC address*/
		UINT32			lVlan;                                      /**< VLAN Tag*/
		UINT16			lEtherType;                                 /**< Ethertype*/
		UINT16			lFrameId;                                   /**< Frame ID*/
		UINT16			lPnCycleCtr;                                /**< Cycle Count from APDU status*/
		UINT8			lPnDataStatus;                              /**< DataStatus from APDU status*/
		UINT8			lPnTransferStatus;                          /**< Transferstatus from APDU status*/
		UINT16			lPortRcve;                                  /**< Receive Port*/
		UINT16			lOffsetData;                                /**< */
		UINT16			lSizeData;                                  /**< */
		eNP_FRAME_NOTE	eNote;
	}
    uNP_FRAME_INFO;

    typedef union _uNP_FRAME_BUFFER
	{
		struct	uAS_FRAME_HEADER	uAs_Header;
		struct	uAS_FRAME_NO_VLAN	uAs_FrameNoVlan;
		struct	uAS_FRAME_VLAN		uAs_FrameVlan;
	}uNP_FRAME_BUFFER;

    // ERTECReg.h
    typedef struct _uNP_STATISTIC
	{
		UINT32	x00_Frames_RcveGood_Bytes;									// (*1)
		UINT32	x04_Frames_RcveTotal_Bytes;
		UINT32	x08_Frames_SendTotal_Bytes;
		UINT32	x0c_Frames_SendTotal;
		UINT32	x10_Frames_RcveTotal_Unicast;
		UINT32	x14_Frames_RcveGood_UnicastUnknown;							// (*1), DestMacAdr not found at FDB table
		UINT32	x18_Frames_RcveTotal_Multicast;
		UINT32	x1c_Frames_RcveGood_MulticastBroadcastUnknown;				// (*1), DestMacAdr not found at FDB table
		UINT32	x20_Frames_RcveTotal_Broadcast;
		UINT32	x24_Frames_SendTotal_Unicast;
		UINT32	x28_Frames_SendTotal_Multicast;
		UINT32	x2c_Frames_SendTotal_Broadcast;
		UINT32	x30_Frames_RcveGoodSendTotal_Lt64;
		UINT32	x34_Frames_RcveGoodSendTotal_64;
		UINT32	x38_Frames_RcveGoodSendTotal_65_127;
		UINT32	x3c_Frames_RcveGoodSendTotal_128_255;
		UINT32	x40_Frames_RcveGoodSendTotal_256_511;
		UINT32	x44_Frames_RcveGoodSendTotal_512_1023;
		UINT32	x48_Frames_RcveGoodSendTotal_1024_1536;
		UINT32	x4c_Frames_RcveGood;										// (*1)
		UINT32	x50_Frames_RcveTotal;
		UINT32	x54_Frames_RcveTotal_Lt64BadCrc;
		UINT32	x58_Frames_RcveTotal_Overflow;								// overflow at MacRcveFifo or PortRcveBuffer_2kByte
		UINT32	x5c_Frames_RcveTotal_Gt1536;
		UINT32	x60_Frames_RcveTotal_ErrorCrcAlignment;
		UINT32	x64_Frames_RcveTotal_Dropped;								// dropped if LowWaterMark or lower limit of FCWs/DBs reached
		UINT32	x68_Frames_SendTotal_Collision;
		UINT32	x6c_HolLimitCrossed;										// HolLimit crossed
		UINT32	x70_CcDiffSummary;
		UINT32	x74_CcDelaySummary;
		UINT32	x78_CcCount;
	}
	uNP_STATISTIC;

    typedef struct _uNP_PROTOCOL_INFO
	{
		UINT32			lArrayFrame[kNP_MAX_ENTRIES_PROTOCOL];				// if an array is embedded in a struct then no for-loop is necessary for copying!
		UINT32			lArrayBytes[kNP_MAX_ENTRIES_PROTOCOL];				// if an array is embedded in a struct then no for-loop is necessary for copying!
	}
	uNP_PROTOCOL_INFO;

    typedef struct _uNP_DRIVER_INFO
	{
		UINT32		lCtrDmacw_UsedMin;
		UINT32		lCtrDmacw_UsedMax;
		UINT32		lCtrDmacw_Total;
		UINT32		lCtrDbHol_UsedMin;
		UINT32		lCtrDbHol_UsedMax;
		UINT32		lCtrDbHol_Total;
		UINT32		lSizeFifoIntern_UsedMin;
		UINT32		lSizeFifoIntern_UsedMax;
		UINT32		lSizeFifoIntern_Total;
		UINT32		lSizeFifoExtern_UsedMin;
		UINT32		lSizeFifoExtern_UsedMax;
		UINT32		lSizeFifoExtern_Total;
		UINT32		lCtrByteCopiedToHarddisk_Min;
		UINT32		lCtrByteCopiedToHarddisk_Max;
		UINT32		lRuntimeIntHandlerUsec_Min;
		UINT32		lRuntimeIntHandlerUsec_Max;
		UINT32		lCtrIdleThread;
		UINT32		lCtrFileFull;
		UINT32		lIndexFileCur;
		UINT32		lCtrByteFileCur;
		UINT32		lIndexFileAbsoluteCaptureStart;
		eNP_BOOL	eSpaceHarddiskFull;
		eNP_BOOL	eIrqNoFreeDb;
		eNP_BOOL	eIrqNoFreeFcw;
		eNP_BOOL	eIrqLowWater;
		eNP_BOOL	eHolLimit;
	}
	uNP_DRIVER_INFO;

    typedef struct _uNP_SW_FILTER_SA
	{
		UINT64	lArrayMacAdr[kNP_MAX_NO_SA];
	}
	uNP_SW_FILTER_SA;

    typedef struct _uNP_SW_FILTER_PROTOCOL
	{
		eNP_BOOL eArrayEnabled[kNP_MAX_ENTRIES_PROTOCOL];
	}
	uNP_SW_FILTER_PROTOCOL;

    typedef struct _uNP_SW_FILTER_PAR
	{
		UINT32					lCtrSa;
		UINT32					lCtrProtocol;
		uNP_SW_FILTER_SA		uSa;
		uNP_SW_FILTER_PROTOCOL	uProtocol;
	}
	uNP_SW_FILTER_PAR;

    //------------------------------------------------------------------------
	typedef struct _uNP_FILE_INFO_V10
	{
		UINT32	lCtrBytesValid;
	}
	uNP_FILE_INFO_V10;
//------------------------------------------------------------------------
	typedef struct _uNP_FILE_INFO_V11
	{
		UINT64	lTimeStartPc_100nsec1601;									// 100nsec since 01.01.1601
		UINT32	lTimeStartAsic_10nsec;
		UINT32	lNoFile;
		UINT32	lRevisionAsic;												// important for getting tick of ERTEC timer (10nsec, 1nsec)
		UINT32	lVersion;													// 0x11 = V1.1 (future: always at this position)
		UINT32	lCtrBytesValid;												// must be last value -> compatible to V1.0
//		UINT32	lDwordFill;													// Dword created by Compiler: sizeof(uNP_FILE_INFO_V11)=0x20  (future: use it for length of struct)
	}
	uNP_FILE_INFO_V11;
//------------------------------------------------------------------------
	typedef struct _uNP_FILE_INFO_V12
	{
		// Attention: compiler may create FillBytes if size!=n*32Byte and alignment not ok

		// first block of 32Byte
		UINT32	lReserved1;
		UINT32	lReserved2;
		UINT32	lReserved3;
		UINT32	lReserved4;
		UINT64	lTimeStartPc_100nsec1601;									// 100nsec since 01.01.1601
		UINT32	lTimeStartAsic_10nsec;
		UINT32	lReserved5;

		// second block of 32Byte
		UINT32	lReserved6;
		UINT32	lRevisionAsic;												// important for getting tick of ERTEC timer (10nsec, 1nsec)
		UINT64	lCtrBytesValid;
		UINT64	lNoFile;
		UINT32	lVersion;													// fixed position, 0x12 = V1.2
		UINT32	lLenStruct;													// fixed position, sizeof(uNP_FILE_INFO_V12)
	}
	uNP_FILE_INFO_V12;
//------------------------------------------------------------------------
	typedef struct _uNP_FILE_INFO_V13
	{
		// Attention: compiler may create FillBytes if size!=n*32Byte and alignment not ok

		// first block of 32Byte
		UINT32	lReserved;
		UINT32	lTimeCaptureStartAsic_10nsec;
		UINT64	lTimeCaptureStartPc_100nsec1601;							// 100nsec since 01.01.1601
		UINT64	lTimeFileStartPc_100nsec1601;								// 100nsec since 01.01.1601: used to find the oldest file
		UINT64	lTimeFileStopPc_100nsec1601;								// 100nsec since 01.01.1601

		// second block of 32Byte
		UINT32	lNoFilesMax;												// maximal number of DataFiles
		UINT32	lSizeFile;													// fixed size of a DataFile
		UINT32	lRevisionAsic;												// important for getting tick of ERTEC timer (10nsec, 1nsec)
		UINT32	lCtrByteValid;
		UINT64	lIndexFileAbsolute;											// absolute file index (incremented since creation of files at this directory)
		UINT32	lVersion;													// fixed position, 0x13 = V1.3
		UINT32	lLenStruct;													// fixed position, sizeof(uNP_FILE_INFO_V13)
	}
	uNP_FILE_INFO_V13;
//------------------------------------------------------------------------
	typedef struct _uNP_FILE_INFO_V14
	{
		// Attention: compiler may create FillBytes if size!=n*32Byte and alignment not ok

		// first block of 32Byte
		UINT32	lLenStruct;													// fixed position, sizeof(uNP_FILE_INFO_V14)
		UINT32	lVersion;													// fixed position, 0x14 = V1.4
		UINT64	lIndexFileAbsolute;											// absolute file index (incremented since creation of files at this directory)
		UINT32	lCtrByteValid;
		UINT32	lRevisionAsic;												// important for getting tick of ERTEC timer (10nsec, 1nsec)
		UINT32	lSizeFile;													// fixed size of a DataFile
		UINT32	lNoFileMax;													// maximal number of DataFiles

		// second block of 32Byte
		UINT64	lTimeFileStartPc_100nsec1601;								// 100nsec since 01.01.1601: used to find the oldest file
		UINT64	lTimeFileStopPc_100nsec1601;								// 100nsec since 01.01.1601
		UINT64	lTimeCaptureStartPc_100nsec1601;							// 100nsec since 01.01.1601
		UINT32	lTimeCaptureStartAsic_10nsec;
		UINT32	lReserved;
	}
	uNP_FILE_INFO_V14;

    typedef struct _uNP_DMACW_RCVE_NETPROFI
	{
		UINT32			lLenFrameWithoutCrc:	11;			// Bit0..10 (FrameLen without CRC, CRC=4 Bytes, Asic removes CRC)
		UINT32			lSelector:				1;			// Bit11 (>=Rev.6)
		UINT32			lPort:					2;			// Bit12..13
		UINT32			lReserved_FrameDelayed:	1;			// Bit14
		UINT32			lReserved2:				1;			// Bit15
		UINT32			lLenFrameBuffer:		9;			// Bit16..24
		UINT32			lReserved3:				1;			// Bit25
		UINT32			lTrigger:				1;			// Bit26
		UINT32			lStatus:				2;			// Bit27..28
		UINT32			lOwner:					1;			// Bit 29
		UINT32			lSrtPrioBackToBack:		1;			// Bit 30 (only relevant at mode BackToBack)
		UINT32			lTagged:				1;			// Bit 31
		UINT32			pNotUsed;
		uACCESS_UINT64	lTime;								// 32Bit-Low:	written by Asic
															// 32Bit-High:	written by driver
	}
	uNP_DMACW_RCVE_NETPROFI;

    // UINT16 at bus:
	//	- at first bytes must be swapped before Bits can be evaluated (e.g. lFlags_OffsetFragment(bus)=0x7abc, lFlags_OffsetFragment(uP)=0xbc7a)
	//	  -> StructureDefinition by Bitfields (e.g.var:3) cannot be used!
	// Bits at UINT8
	//	- at structure the lowest bit must be defined at first!
#ifdef PNDEV_OS_WIN
#pragma warning (disable: 4214)
#endif
	typedef struct _uNP_IP_HEADER
	{
		UINT8	lLenHeaderIpDiv4:	4;										// real HeaderLength = lLenHeaderIpDiv4 x 4Byte (StandardValue=5)
		UINT8	lVersion:			4;
		UINT8	lTos;
		UINT16	lLenIpTotal;												// IP-header + IP-data
		UINT16	lIdentification;
		UINT16	lFlags_OffsetFragment;
		UINT8	lTimeToLive;
		UINT8	lProtocolId;
		UINT16	lCrcHeader;
		UINT32	lAdrSrc;
		UINT32	lAdrDst;
	}
	uNP_IP_HEADER;

    typedef struct _uNP_UDP_HEADER
	{
		UINT16	lPortSrc;
		UINT16	lPortDst;
		UINT16	lLenUdpTotal;												// UDP-header + UDP-data
		UINT16	lCrcUdp;													// 0: not used
	}
	uNP_UDP_HEADER;
#endif
