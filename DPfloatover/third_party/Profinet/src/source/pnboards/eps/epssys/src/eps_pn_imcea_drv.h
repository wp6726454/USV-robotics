#ifndef EPS_PN_IMCEA_DRV_H_
#define EPS_PN_IMCEA_DRV_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_pn_imcea_drv.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#include "eps_sys.h"
#include "eps_rtos.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include "imcdrv.h"             // Defines for the NT-Driver
#include "imcealib.h"           // Defines for our LIB
#endif

#include <eps_plf.h>       

#include <psi_cfg.h>      
#include "eps_sys.h"

#if defined (EPS_CFG_USE_PNDEVDRV)
#include <precomp.h>
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#endif
#include "eps_hif_drv_if.h"
#include "eps_shm_if_config.h"
#include "eps_shm_file.h"
#include "eps_shm_if.h"

#include <pntrc_cfg.h>
#include <pntrc_usr.h>
#include "pntrc_sys.h"

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
#ifndef PROCESSOR_ARCHITECTURE_AMD64
#define PROCESSOR_ARCHITECTURE_AMD64            9
#endif

#define EPS_CFG_PN_IMCEA_MAX_BOARDS        5

#define EPS_PN_IMCEA_WD_ALIVE_POLL_TIME_MS              1000    // time in ms
#define EPS_PN_IMCEA_WD_MAX_TIMEOUTCOUNTER              20      // EPS_CFG_PN_IMCEA_WD_ALIVE_POLL_TIME_MS * EPS_CFG_PN_IMCEA_WD_MAX_TIMEOUTCOUNTER = TIMEOUT (e.g. 1000 ms * 20 = 20 sec) 
#define EPS_PN_IMCEA_ADN_REBOOT_MAX_TIMEOUTCOUNTER      20      // wait max. 20 trys for the reboot of adonis (each try waits 500 ms -> 20 * 500 ms = 10 sec)
    
#define EPS_PN_IMCEA_SELF_STATE_NINIT  0x00000000ul
#define EPS_PN_IMCEA_SELF_STATE_READY  0x12345678ul
#define EPS_PN_IMCEA_SELF_STATE_FATAL  0xaffeaffeul
#define EPS_PN_IMCEA_REMO_STATE_NINIT  0x00000000ul
#define EPS_PN_IMCEA_REMO_STATE_READY  0x5512AA34ul
#define EPS_PN_IMCEA_REMO_STATE_CLOSED 0xcafecafeul

typedef enum eps_imcea_thread_msg
{
	IMCEA_THREAD_START,
	IMCEA_THREAD_STOP
}EPS_IMCEA_THREAD_MSG;

typedef struct eps_imcea_thread_msg_type
{
	EPS_IMCEA_THREAD_MSG  msgType;
	HANDLE                eventDone;
} EPS_IMCEA_THREAD_MSG_TYPE, *EPS_IMCEA_THREAD_MSG_PTR_TYPE;



//------------------------------------------------------------------------
typedef struct _uIMCEA_OPEN_DEVICE_OUT  // device parama after open
{
	LSA_UINT16 uEddType;
	LSA_UINT16 uPciBusNr;
	LSA_UINT16 uPciDeviceNr;
	LSA_UINT16 uPciFuncNr;
}
uIMCEA_OPEN_DEVICE_OUT;

typedef struct eps_imcea_driver_open_info_tag 
	{
	LSA_BOOL               is_open;    // Board opened yes/no
	LSA_UINT16             board_nr;   // logical board number (from INI)
	LSA_UINT16             hif_handle; // HIF handle for HIF ISR
	uIMCEA_OPEN_DEVICE_OUT openInfo;   // Device params after open
} EPS_IMCEA_DRIVER_OPEN_INFO_TYPE;

typedef struct eps_pn_imcea_wd_tag
{
    LSA_UINT32          hWdThread;
    LSA_UINT32          bWdEnabled;
    LSA_TIMER_ID_TYPE   hWdTimer;
    LSA_UINT32          bAlive;
    LSA_UINT32          uTimeOutCounter;
}EPS_PN_IMCEA_WD_TYPE, *EPS_PN_IMCEA_WD_PTR_TYPE;

typedef struct eps_imcea_check_remote_state_tag
{
    LSA_UINT32          bCheckRemoteStateEnabled;
    LSA_TIMER_ID_TYPE   hCheckRemoteStateTimer;
}EPS_IMCEA_CHECK_REMOTE_STATE_TYPE, *EPS_IMCEA_CHECK_REMOTE_STATE_PTR_TYPE;

typedef struct {  // structure of Shared MEM( must match mit EPS, see also EPS)
	LSA_UINT32 uSyncSelf;
	LSA_UINT32 uSyncRemote;
	LSA_UINT32 uSizeSharedMemBlock;
	LSA_UINT32 uSharedMemBlockAddr;
	LSA_UINT32 uHifOffset;
	LSA_UINT32 uHifSize;
	LSA_UINT32 uFatalOffset;
	LSA_UINT32 uFatalSize;
	LSA_UINT32 uTrace1Offset;
	LSA_UINT32 uTrace1Size;
	LSA_UINT32 uTrace2Offset;
	LSA_UINT32 uTrace2Size;
	LSA_UINT32 bDebugOn;
	LSA_UINT32 bFatal;
	LSA_UINT32 uOsIdent;
	struct
	{
		LSA_UINT32 uEddType;
		LSA_UINT32 uPciBusNr;
		LSA_UINT32 uPciDeviceNr;
		LSA_UINT32 uPciFuncNr;
	} BoardInfoStruct[10];
	EPS_SHM_FILE_TRANSFER_TYPE HostFileTransfer;
	EPS_PN_IMCEA_WD_TYPE Watchdog;
	EPS_IMCEA_CHECK_REMOTE_STATE_TYPE   CheckRemoteState;
} EPS_IMCEA_SHM_MEM_HEADER_TYPE, *EPS_IMCEA_SHM_MEM_HEADER_PTR_TYPE;

typedef struct eps_pn_imcea_board_tag
{
    LSA_BOOL bUsed;
	EPS_PNDEV_HW_TYPE sHw;
	LSA_UINT32 uCountIsrEnabled;	
	EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;
	EPS_SYS_TYPE sysDev;
	//EPS_STDMACDRV_DEVICE_INFO_TYPE sDeviceInfo;
	struct eps_imceadrv_store_tag* pBackRef;
    LSA_UINT16 hd_id;
	LSA_BOOL bBoardFwUsed;
	EPS_SHM_UPPER_CONFIG_TYPE SharedMemConfig;

	struct 
	{
		LSA_BOOL bOpened;
		PNTRC_CPU_HANDLE_PTR_TYPE hPntrcCPU;
	} Pntrc;

} EPS_PN_IMCEA_BOARD_TYPE, *EPS_PN_IMCEA_BOARD_PTR_TYPE;

typedef struct eps_imceadrv_poll_tag
{
	LSA_BOOL   bRunning;
	LSA_UINT32 uCntUsers;
	LSA_UINT32 hThread;
	EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc[EPS_CFG_PN_IMCEA_MAX_BOARDS];
	EPS_PN_IMCEA_BOARD_PTR_TYPE pBoard[EPS_CFG_PN_IMCEA_MAX_BOARDS];
} EPS_IMCEADRV_POLL_TYPE, *EPS_IMCEADRV_POLL_PTR_TYPE;


typedef struct eps_imceadrv_store_tag
{
	LSA_BOOL bInit;
	LSA_UINT16 hEnterExit;
	EPS_PN_IMCEA_BOARD_TYPE pBoard[EPS_CFG_PN_IMCEA_MAX_BOARDS];
	EPS_SHM_HW_TYPE sHw;
	EPS_HIF_DRV_HW_PTR_TYPE pHifDrvHwInstOut;
	LSA_UINT32 uCntFoundDevices;
	struct
	{
		EPS_IMCEADRV_POLL_TYPE PnGatheredIsr;
	} poll;

} EPS_IMCEADRV_STORE_TYPE, *EPS_IMCEADRV_STORE_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID eps_pn_imceadrv_install(LSA_VOID);
LSA_VOID eps_pn_imceadrv_uninstall(LSA_VOID);
LSA_UINT16 eps_pn_imceadrv_open(EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
LSA_UINT16 eps_pn_imceadrv_close(EPS_PNDEV_HW_PTR_TYPE pHwInstIn);
LSA_BOOL ReadHwParam( LSA_UINT16 boardNr, void* pParam);
LSA_BOOL eps_pn_imceadrv_connect_board( LSA_UINT16 boardNr, EPS_PN_IMCEA_BOARD_PTR_TYPE pBoard); 
LSA_BOOL eps_pn_imceadrv_disconnect_board( LSA_UINT16 boardNr);
errno_t shmFileOpen(LSA_UINT8* sFile, FILE** phFile);
LSA_BOOL HandleFileTransfer( LSA_VOID );
LSA_BOOL ReadTraceData( LSA_UINT16 boardNr, LSA_UINT32	offset, LSA_UINT8* Ptr, LSA_UINT32 Size);
LSA_BOOL WriteTraceData( LSA_UINT16 boardNr, LSA_UINT8* Ptr, LSA_UINT32 offset, LSA_UINT32 Size);
LSA_BOOL  IsConnected( LSA_UINT16 boardNr );
LSA_VOID eps_pn_imceadrv_get_board( LSA_UINT16 boardNr, EPS_PNDEV_LOCATION_PTR_TYPE location_id );
LSA_BOOL eps_pn_imceadrv_connect_driver( LSA_VOID );

typedef struct eps_imcea_global_vals 
{
	LSA_UINT32  uThreadFileTransferUserCnt;
	LSA_BOOL    bImceaStarted;
	LSA_BOOL    bFWDebugActivated;

	EPS_IMCEA_DRIVER_OPEN_INFO_TYPE   DeviceOpenList[PSI_CFG_MAX_HD_CNT+1];      // Device list
	EPS_IMCEA_SHM_MEM_HEADER_PTR_TYPE pShmHeaderShared;

	HANDLE hDevice;
	char szDriverName[50];

} EPS_IMCEA_GLOBAL_VALS_TYPE;

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/



#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PN_IMCEA_DRV_H_ */
