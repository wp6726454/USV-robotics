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
/*  F i l e               &F: os.h                                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for adaption of SYS to used OS
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __os_h__
#define __os_h__

	//########################################################################
	//  Structures - required for Defines
	//########################################################################

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#else
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#endif

	//########################################################################
	//  Defines
	//########################################################################

	#define	HIGH16_OF_UINT32(lValueIn)	((lValueIn) >> 16)
	#define	LOW16_OF_UINT32( lValueIn)	((lValueIn) & 0x0000ffff)

	//------------------------------------------------------------------------
	// Note:
	//	- gcc creates warnings for multi-character character constants
	//	- it would be possible to suppress it by -Wno-multichar
	//	- better: convert it to HEX by a macro
	//		e.g. tag 'Fred' is passed as 'derF' to ExAllocatePoolWithTag() for getting 0x64657246 (LittleEndian, 'd'=0x64, 'e'=0x65, 'r'=0x72, 'F'=0x46)
	//		-> macro TAG('F', 'r', 'e', 'd') which regards LittleEndian automatically
	#define	TAG(a, b, c ,d)		((a) + ((b)<<8) + ((c)<<16) + ((d)<<24))

	//------------------------------------------------------------------------
	#if defined(PNDEV_UNICODE)
		#define	_RtlStringCchCopy	RtlStringCchCopyW
		#define	_RtlStringCchCatN	RtlStringCchCatNW
		#define	_RtlStringCchPrintf	RtlStringCchPrintfW
	#else
		#define	_RtlStringCchCopy	RtlStringCchCopyA
		#define	_RtlStringCchCatN	RtlStringCchCatNA
		#define	_RtlStringCchPrintf	RtlStringCchPrintfA
	#endif

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	typedef	LARGE_INTEGER uPNDEV_CLOCK;

	//------------------------------------------------------------------------
	// Microsoft macros use e.g. fence() instruction which is important for volatile stores (cashing problems)

	#define READ_REG_UINT32(	pRegIn)				READ_REGISTER_ULONG(	((UINT32*) (pRegIn)))
	#define WRITE_REG_UINT32(	pRegIn, lValueIn)	WRITE_REGISTER_ULONG(	((UINT32*) (pRegIn)), lValueIn)

	#define READ_REG_UINT16(	pRegIn)				READ_REGISTER_USHORT(	((UINT16*) (pRegIn)))
	#define WRITE_REG_UINT16(	pRegIn, lValueIn)	WRITE_REGISTER_USHORT(	((UINT16*) (pRegIn)), lValueIn)

	#define READ_REG_UINT8(		pRegIn)				READ_REGISTER_UCHAR(	((UINT8*)  (pRegIn)))
	#define WRITE_REG_UINT8(	pRegIn, lValueIn)	WRITE_REGISTER_UCHAR(	((UINT8*)  (pRegIn)), lValueIn)

	//------------------------------------------------------------------------
	#else
	//------------------------------------------------------------------------
	typedef	struct timespec uPNDEV_CLOCK;
	typedef INT64			KAFFINITY;
	typedef UINT8			KIRQL;

	//------------------------------------------------------------------------
	#define	PASSIVE_LEVEL	0
	#define	APC_LEVEL		1
	#define	DISPATCH_LEVEL	2

	//------------------------------------------------------------------------
	#define PAGED_CODE()

	//------------------------------------------------------------------------
	// following functions don't have a ReturnValue -> implementation as macro possible
	#define RtlZeroMemory(	pDestOut, lSizeIn)				__builtin_memset(	(pDestOut), 0, (lSizeIn))
	#define RtlFillMemory(	pDestOut, lSizeIn, lValueIn)	__builtin_memset(	(pDestOut), (lValueIn), (lSizeIn))
	#define RtlCopyMemory(	pDestOut, pSrcIn, lSizeIn)		__builtin_memcpy(	(pDestOut), (pSrcIn),(lSizeIn))

	//------------------------------------------------------------------------
	// ReturnValue of following functions is never checked -> implementation as macro possible
	#define	RtlStringCchCopyW		wcscpy_s
	#define	RtlStringCchCopyA		strcpy_s

	#define	RtlStringCchCatNW		wcsncat_s
	#define	RtlStringCchCatNA		strncat_s

	#define	RtlStringCchPrintfW		swprintf_s
	#define	RtlStringCchPrintfA		sprintf_s

	//------------------------------------------------------------------------
	#define READ_REG_UINT32(	pRegIn)				( *(volatile UINT32* const) (pRegIn))
	#define WRITE_REG_UINT32(	pRegIn, lValueIn)	((*(volatile UINT32* const) (pRegIn)) = lValueIn)

	#define READ_REG_UINT16(	pRegIn)				( *(volatile UINT16* const) (pRegIn))
	#define WRITE_REG_UINT16(	pRegIn, lValueIn)	((*(volatile UINT16* const) (pRegIn)) = lValueIn)

	#define READ_REG_UINT8(		pRegIn)				( *(volatile UINT8*  const) (pRegIn))
	#define WRITE_REG_UINT8(	pRegIn, lValueIn)	((*(volatile UINT8*  const) (pRegIn)) = lValueIn)

	//------------------------------------------------------------------------
	#define PNDEV_DIM_ARRAY_EVENT_HANDLE_TABLE	256

	//------------------------------------------------------------------------
	#define PNDEV_PCI_STATUS_CAPABILITIES_LIST	0x10
	#define PNDEV_PCI_MULTIFUNCTION				0x80
	#define PNDEV_PCI_CAPABILITY_PTR_TYPE_0_1	0x34
	#define PNDEV_PCI_CAPABILITY_PTR_TYPE_2		0x14
	#define PNDEV_PCI_BRIDGE_TYPE				0x01
	#define PNDEV_PCI_CARDBUS_BRIDGE_TYPE		0x02

	//------------------------------------------------------------------------
	#define PNDEV_PCIE_CAPABILITY_ID	0x10

	//------------------------------------------------------------------------
	#define PNDEV_PCIE_CAPABILITY_LINK_STATUS_OFFSET 	0x12
	#define PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET 0x08
	#define PNDEV_PCIE_CAPABILITY_DEVICE_STATUS_OFFSET 	0x0a

	//------------------------------------------------------------------------
	#define PNDEV_PCIE_CAPABILITY_MASK_LINK_WIDTH		0x3F0					// bits 9:4
	#define PNDEV_PCIE_CAPABILITY_MASK_PAY_LOAD_SIZE	0xE0					// bits	7:5

	#endif

	//########################################################################
	//  Structures
	//########################################################################

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------

	typedef struct _uPNDEV_EVENT_HANDLE_TABLE
	{
		UINT32 lDummy;
	}
	uPNDEV_EVENT_HANDLE_TABLE;

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

#endif
