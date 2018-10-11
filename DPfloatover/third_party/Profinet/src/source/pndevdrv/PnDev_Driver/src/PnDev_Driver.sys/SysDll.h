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
/*  F i l e               &F: SysDll.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Common declarations for SYS/DLL
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __SysDll_h__
#define __SysDll_h__

	//########################################################################
	//  Compiler switches
	//########################################################################

	// Note:
	//	- compiler switch should be placed in a common file of SYS/DLL
	//	- they may affect both components!

//	#define FPGA1_ERTEC200P_ARM_NO_TOPLEVEL_ICU									// Host-IRQ created by PNIP-ICU2 IRQ0

	//------------------------------------------------------------------------
	#define	PNDEV_TRACE															// must always be set
	//#define	PNDEV_GET_RUNTIME_IOCTL
	//#define	PNDEV_GET_RUNTIME_COPY_BLOCK_PAGED
	//#define	PNDEV_GET_RUNTIME_THREAD
	//#define	PNDEV_GET_RUNTIME_ISR
	//#define	PNDEV_GET_RUNTIME_ISR_BY_GPIO
	//#define	PNDEV_TEST_GPIO
	//#define	PNDEV_DEVELOPMENT

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
		#define	PNDEV_TRACE_ISR_DPC
	#endif

	//########################################################################
	//  Version
	//########################################################################

	#define PNDEV_DRIVER_MAJOR	PNDEVDRV_VERSION
	#define PNDEV_DRIVER_MINOR	PNDEVDRV_DISTRIBUTION
	#define PNDEV_DRIVER_BUILD	PNDEVDRV_INCREMENT
	#define PNDEV_DRIVER_REV	PNDEVDRV_INTEGRATION_COUNTER

	//------------------------------------------------------------------------
	// conversion of numbers into string:										// see MSDN: #pragma message
	#define TO_STRING2(x)	#x
	#define TO_STRING(x)	TO_STRING2(x)

	//#define PNDEV_DRIVER_VERSION 	"V0.2.0.0000"
	#define PNDEV_DRIVER_VERSION	TO_STRING(PNDEV_DRIVER_MAJOR) "." TO_STRING(PNDEV_DRIVER_MINOR) "."	\
									TO_STRING(PNDEV_DRIVER_BUILD) "." TO_STRING(PNDEV_DRIVER_REV) 
	#define PNDEV_DRIVER_VERSION_UC	_TEXT(TO_STRING(PNDEV_DRIVER_MAJOR)) _TEXT(".") _TEXT(TO_STRING(PNDEV_DRIVER_MINOR)) _TEXT(".") 	\
									_TEXT(TO_STRING(PNDEV_DRIVER_BUILD)) _TEXT(".") _TEXT(TO_STRING(PNDEV_DRIVER_REV)) 

	//#pragma message(PNDEV_DRIVER_VERSION)

	//########################################################################
	//  Defines
	//########################################################################

	#define	PNDEV_SIZE_STRING_BUF			260									// same definition at PnDev_DriverU.h
	#define	PNDEV_SIZE_STRING_BUF_SHORT		64									// same definition at PnDev_DriverU.h
	#define	PNDEV_SIZE_STRING_TRACE			260

	//------------------------------------------------------------------------
	#define	PNDEV_ERROR_MASTER_APP			_TEXT("PNDEV_ERROR_MASTER_APP")
	#define	PNDEV_ERROR_SUPPORT				_TEXT("PNDEV_ERROR_SUPPORT")
	#define	PNDEV_ERROR_IN_USE				_TEXT("PNDEV_ERROR_IN_USE")
	#define	PNDEV_ERROR_DEVICE_NOT_FOUND	_TEXT("PNDEV_ERROR_DEVICE_NOT_FOUND")

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_IOCTL
	{
		ePNDEV_IOCTL_INVALID			= 0,
		ePNDEV_IOCTL_OK					= 1,
		ePNDEV_IOCTL_PENDING			= 2,
		ePNDEV_IOCTL_ERROR_BY_STRING	= 3,
		ePNDEV_IOCTL_ERROR_REQ			= 4,
		ePNDEV_IOCTL_ERROR_BUF_SIZE		= 5,
		ePNDEV_IOCTL_ERROR_BUF_IN		= 6,
		ePNDEV_IOCTL_ERROR_BUF_OUT		= 7
	}
	ePNDEV_IOCTL;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_DEVICE
	{
		ePNDEV_DEVICE_INVALID			= 0,
		ePNDEV_DEVICE_OK				= 1,
		ePNDEV_DEVICE_ERROR_RES			= 2,
		ePNDEV_DEVICE_ERROR_INTERNAL	= 3
	}
	ePNDEV_DEVICE;

	//------------------------------------------------------------------------
	#define	PNDEV_DIM_ARRAY_EVENT_THREAD_DLL	4

	#define	PNDEV_EVENT_THREAD_DLL_MIN	1
	#define	PNDEV_EVENT_THREAD_DLL_MAX	4

	typedef enum _ePNDEV_EVENT_THREAD_DLL
	{
		ePNDEV_EVENT_THREAD_DLL_INVALID			= 0,							// don't change this value, see fnWaitForEventThreadDll()
		ePNDEV_EVENT_THREAD_DLL_STOP			= 1,
		ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON	= 2,
		ePNDEV_EVENT_THREAD_DLL_UINFO			= 3,
		ePNDEV_EVENT_THREAD_DLL_UISR			= 4
	}
	ePNDEV_EVENT_THREAD_DLL;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_ICU
	{
		ePNDEV_ICU_INVALID			= 0,
		ePNDEV_ICU_IRTE				= 1,
		ePNDEV_ICU_PNIP				= 2,
		ePNDEV_ICU_TOP_LEVEL_PCI	= 3,
		ePNDEV_ICU_TOP_LEVEL_ARM	= 4,
		ePNDEV_ICU_TOP_LEVEL_PERIF	= 5,
		ePNDEV_ICU_INTEL			= 6,
		ePNDEV_ICU_MICREL			= 7
	}
	ePNDEV_ICU;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_FATAL_ASYNC
	{
		ePNDEV_FATAL_ASYNC__INVALID						= 0,
		ePNDEV_FATAL_ASYNC__INVALID_BOARD_DETAIL		= 1,
		ePNDEV_FATAL_ASYNC__INVALID_ICU_MODE			= 2,
		ePNDEV_FATAL_ASYNC__INVALID_INT_ACTION			= 3,
		ePNDEV_FATAL_ASYNC__INVALID_ASIC_INT			= 4,
		ePNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT			= 5,
		ePNDEV_FATAL_ASYNC__INVALID_DPC_CALL			= 6,
		ePNDEV_FATAL_ASYNC__ASIC_DEMO_INT_NOT_SET		= 7,
		ePNDEV_FATAL_ASYNC__BOARD_DEMO_INT_NOT_SET		= 8,
		ePNDEV_FATAL_ASYNC__DPC_NOT_QUEUED				= 9,
		ePNDEV_FATAL_ASYNC__UISR_NOT_QUEUED				= 10,
		ePNDEV_FATAL_ASYNC__ERROR_ISR_LOCK				= 11,
		ePNDEV_FATAL_ASYNC__ERROR_INT_MASK				= 12,
		ePNDEV_FATAL_ASYNC__PCIE_ACCESS_TIMEOUT			= 13,
		ePNDEV_FATAL_ASYNC__ADMIN_REG_NOT_EXIST			= 14,
		ePNDEV_FATAL_ASYNC__INVALID_DYN_DRIVER_PAGE		= 15,
		ePNDEV_FATAL_ASYNC__DYN_DRIVER_PAGE_SET_ERROR	= 16,
		ePNDEV_FATAL_ASYNC__SIGN_OF_LIFE				= 17,
		ePNDEV_FATAL_ASYNC__ERROR_SPIN_LOCK				= 18,
		ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC				= 19,
		ePNDEV_FATAL_ASYNC__ERROR_EVENT					= 20,

		ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_SYS			= 21,
		ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_SYS	= 22
	}
	ePNDEV_FATAL_ASYNC;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_WARNING
	{
		ePNDEV_WARNING__INVALID				= 0,
		ePNDEV_WARNING__PCIE_ACCESS_TIMEOUT	= 1
	}
	ePNDEV_WARNING;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_DEBUG
	{
		ePNDEV_DEBUG__INVALID					= 0,
		ePNDEV_DEBUG__SET_SEMAPHORE_BY_DEBUGGER	= 1
	}
	ePNDEV_DEBUG;

	//------------------------------------------------------------------------
	#define	PNDEV_FATAL_ASYNC__INVALID_BOARD_DETAIL			_TEXT("Invalid BoardDetail!")
	#define	PNDEV_FATAL_ASYNC__INVALID_ICU_MODE				_TEXT("Invalid IcuMode!")
	#define	PNDEV_FATAL_ASYNC__INVALID_INT_ACTION			_TEXT("Invalid IntAction!")
	#define	PNDEV_FATAL_ASYNC__INVALID_ASIC_INT				_TEXT("Invalid AsicInt!")
	#define	PNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT			_TEXT("Unexpected AsicInt!")
	#define	PNDEV_FATAL_ASYNC__INVALID_DPC_CALL				_TEXT("Invalid call of DPC (IntTest_ContIsr)!")
	#define	PNDEV_FATAL_ASYNC__ASIC_DEMO_INT_NOT_SET		_TEXT("AsicDemoInt not set (may happen at masking of interrupts)!")
	#define	PNDEV_FATAL_ASYNC__BOARD_DEMO_INT_NOT_SET		_TEXT("BoardDemoInt not set!")
	#define	PNDEV_FATAL_ASYNC__DPC_NOT_QUEUED				_TEXT("DPC not queued!")
	#define	PNDEV_FATAL_ASYNC__UISR_NOT_QUEUED				_TEXT("UISR not queued (no MasterApp)!")
	#define	PNDEV_FATAL_ASYNC__ERROR_ISR_LOCK				_TEXT("ISR lock error!")
	#define	PNDEV_FATAL_ASYNC__ERROR_INT_MASK				_TEXT("Interrupt mask error!")
	#define	PNDEV_FATAL_ASYNC__PCIE_ACCESS_TIMEOUT			_TEXT("PCIe access timeout!")
	#define	PNDEV_FATAL_ASYNC__ADMIN_REG_NOT_EXIST			_TEXT("Admin register not exist!")
	#define	PNDEV_FATAL_ASYNC__INVALID_DYN_DRIVER_PAGE		_TEXT("Invalid dynamic DriverPage!")
	#define	PNDEV_FATAL_ASYNC__DYN_DRIVER_PAGE_SET_ERROR	_TEXT("Error at setting dynamic DriverPage!")
	#define	PNDEV_FATAL_ASYNC__SIGN_OF_LIFE					_TEXT("No SignOfLife of UserFw!")
	#define	PNDEV_FATAL_ASYNC__ERROR_SPIN_LOCK				_TEXT("Error at SpinLock!")
	#define	PNDEV_FATAL_ASYNC__ERROR_CRIT_SEC				_TEXT("Error at CriticalSection!")
	#define	PNDEV_FATAL_ASYNC__ERROR_EVENT					_TEXT("Error at EventHandling!")

	#define	PNDEV_FATAL_ASYNC__SIMULATED_FATAL_SYS			_TEXT("Simulated FatalError at SYS!")
	#define	PNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_SYS	_TEXT("Simulated FatalError at SysThread!")
	#define	PNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_DLL	_TEXT("Simulated FatalError at DllThread!")

	#define	PNDEV_WARNING__PCIE_ACCESS_TIMEOUT				_TEXT("PCIe access timeout!")
	#define	PNDEV_REQ_CLOSE									_TEXT("Please close your device at once! An application wants to reset the board.")
	#define	PNDEV_DEBUG__SET_SEMAPHORE_BY_DEBUGGER			_TEXT("Please start your debugger script for setting semaphore...")

	//########################################################################
	//  Structures
	//########################################################################

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
	typedef struct _uPNDEV_EVENT
	{
		UINT32				lFatalFw;
		ePNDEV_FATAL_ASYNC	eFatalAsync;
		ePNDEV_WARNING		eWarning;
		BOOLEAN				bReqClose;
		ePNDEV_DEBUG		eDebug;
	}
	uPNDEV_EVENT;

	//========================================================================
	//	common for all IOCTLs
	//========================================================================

	typedef struct _uPNDEV_IOCTL_OUT
	{
		_TCHAR	sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_IOCTL_OUT;

	//========================================================================
	//	service ResetDevice
	//========================================================================

	typedef struct _uPNDEV_RESET_DEVICE_INTERNAL_IN
	{
		uPNDEV64_HANDLE uAppSys;
	}
	uPNDEV_RESET_DEVICE_INTERNAL_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_RESET_DEVICE_IN
	{
		uPNDEV64_HANDLE uDevice;

		uPNDEV_RESET_DEVICE_INTERNAL_IN uInternal;
	}
	uPNDEV_RESET_DEVICE_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_RESET_DEVICE_OUT
	{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_RESET_DEVICE_OUT;

	//========================================================================
	//	service GetEventList
	//========================================================================

	typedef struct _uPNDEV_GET_EVENT_LIST_IN
	{
		uPNDEV64_HANDLE uAppSys;
	}
	uPNDEV_GET_EVENT_LIST_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_GET_EVENT_LIST_OUT
	{
		_TCHAR			sError[PNDEV_SIZE_STRING_BUF];
		uPNDEV_EVENT	uEvent;
	}
	uPNDEV_GET_EVENT_LIST_OUT;

	//========================================================================
	//	service SetEventList
	//========================================================================

	typedef struct _uPNDEV_SET_EVENT_LIST_IN
	{
		uPNDEV64_HANDLE	uAppSys;
		uPNDEV_EVENT	uEvent;
	}
	uPNDEV_SET_EVENT_LIST_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_SET_EVENT_LIST_OUT
	{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_SET_EVENT_LIST_OUT;

	//========================================================================
	//	service StartBootFw
	//========================================================================

	typedef struct _uPNDEV_START_BOOT_FW_IN
	{
		UINT32 lDummy;
	}
	uPNDEV_START_BOOT_FW_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_START_BOOT_FW_OUT
	{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_START_BOOT_FW_OUT;

	//========================================================================
	//	service StartUserFw
	//========================================================================

	typedef struct _uPNDEV_START_USER_FW_IN
	{
		uPNDEV64_HANDLE			uAppSys;
		UINT32					lOffsetFwBuf;
		UINT32					lIndexFw;
		UINT32					lIndexFwMax;
		BOOLEAN					bDebugMode;
		BOOLEAN					bShmDebug;
		BOOLEAN					bShmHaltOnStartup;

		UINT32					lTimeoutFwLoad_10msec;
		UINT32					lTimeoutFwExe_10msec;
		UINT32					lTimeoutFwStartupStd_10msec;
		UINT32					lTimeoutFwStartupDebug_10msec;
		UINT32					lTimeoutDebugger_10msec;
		UINT32					lTimeWaitDebugger_10msec;
		UINT32					lTimeStartupSignOfLife_10msec;
		UINT32					lTimeUpdateSignOfLife_10msec;
		UINT32					lBatHostAddress;
		
		UINT32					lArrayUserData[PNCORE_DIM_ARRAY_USER_DATA];
		uPNCORE_PCI_HOST_PAR	uPciMaster[3];
		uPNDEV_HW_DESC_SDRAM	uAsicSdram_Org;
	}
	uPNDEV_START_USER_FW_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_START_USER_FW_OUT
	{
		_TCHAR					sError[PNDEV_SIZE_STRING_BUF];
		uPNDEV_HW_DESC_SDRAM	uAsicSdram_Updated;
	}
	uPNDEV_START_USER_FW_OUT;

	//========================================================================
	//	service IsUserFwReady
	//========================================================================

	typedef struct _uPNDEV_IS_USER_FW_READY_IN
	{
		uPNDEV64_HANDLE			uAppSys;
	}
	uPNDEV_IS_USER_FW_READY_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_IS_USER_FW_READY_OUT
	{
		_TCHAR					sError[PNDEV_SIZE_STRING_BUF];
		BOOLEAN					bIsUserFwReady;
	}
	uPNDEV_IS_USER_FW_READY_OUT;
	
	//========================================================================
	//	service ChangeAppLock
	//========================================================================

	typedef struct _uPNDEV_CHANGE_APP_LOCK_IN
	{
		uPNDEV64_HANDLE	uAppSys;
		BOOLEAN			bLockMultiApp;
	}
	uPNDEV_CHANGE_APP_LOCK_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_CHANGE_APP_LOCK_OUT
	{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_CHANGE_APP_LOCK_OUT;

	//========================================================================
	//	service NotifyDllState
	//========================================================================

	typedef struct _uPNDEV_NOTIFY_DLL_STATE_IN
	{
		BOOLEAN	bReady;
	}
	uPNDEV_NOTIFY_DLL_STATE_IN;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_NOTIFY_DLL_STATE_OUT
	{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_NOTIFY_DLL_STATE_OUT;

	//========================================================================
	//	service GetDeviceInfoSys
	//========================================================================

	typedef struct _uPNDEV_GET_DEVICE_INFO_SYS_IN
	{
		UINT32 lIndexDevice;
	}
	uPNDEV_GET_DEVICE_INFO_SYS_IN;
	//------------------------------------------------------------------------
	typedef struct _uPNDEV_GET_DEVICE_INFO_SYS_OUT
	{
		_TCHAR			sError[PNDEV_SIZE_STRING_BUF];
		UINT32			lCtrDevice;
		ePNDEV_BOARD	eBoard;
		PNDEV_CHAR		sPciLoc[PNDEV_SIZE_STRING_BUF_SHORT];					// e.g. "PCI bus 5, device 10, function 0"
	}
	uPNDEV_GET_DEVICE_INFO_SYS_OUT;

	//========================================================================
	//	service GetDeviceHandleSys
	//========================================================================

	typedef struct _uPNDEV_GET_DEVICE_HANDLE_SYS_IN
	{
		PNDEV_CHAR sPciLoc[PNDEV_SIZE_STRING_BUF_SHORT];						// e.g. "PCI bus 5, device 10, function 0"
	}
	uPNDEV_GET_DEVICE_HANDLE_SYS_IN;
	//------------------------------------------------------------------------
	typedef struct _uPNDEV_GET_DEVICE_HANDLE_SYS_OUT
	{
		_TCHAR			sError[PNDEV_SIZE_STRING_BUF];
		uPNDEV64_HANDLE	uDeviceSys;
	}
	uPNDEV_GET_DEVICE_HANDLE_SYS_OUT;

	//========================================================================
	//	service AllocEventId
	//========================================================================

	typedef struct _uPNDEV_ALLOC_EVENT_ID_IN
	{
		UINT32 lDummy;
	}
	uPNDEV_ALLOC_EVENT_ID_IN;
	//------------------------------------------------------------------------
	typedef struct _uPNDEV_ALLOC_EVENT_ID_OUT
	{
		_TCHAR	sError[PNDEV_SIZE_STRING_BUF];
		UINT32	lEventId;
	}
	uPNDEV_ALLOC_EVENT_ID_OUT;

	//========================================================================
	//	service FreeEventId
	//========================================================================

	typedef struct _uPNDEV_FREE_EVENT_ID_IN
	{
		UINT32 lEventId;
	}
	uPNDEV_FREE_EVENT_ID_IN;
	//------------------------------------------------------------------------
	typedef struct _uPNDEV_FREE_EVENT_ID_OUT
	{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_FREE_EVENT_ID_OUT;

	//------------------------------------------------------------------------
	#pragma pack(pop)															// restore packing alignment 
//	#pragma pack(show)

#endif
