#ifndef EPS_PN_DRV_IF_H_
#define EPS_PN_DRV_IF_H_

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
/*  F i l e               &F: eps_pn_drv_if.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN Hardware Manager Interface                                        */
/*                                                                           */
/*****************************************************************************/

#if defined (EPS_CFG_USE_PNDEVDRV)
#include <precomp.h>
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#endif

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_PNDEV_RET_ERR             0
#define EPS_PNDEV_RET_OK              1
#define EPS_PNDEV_RET_DEV_NOT_FOUND   2
#define EPS_PNDEV_RET_DEV_OPEN_FAILED 3
#define EPS_PNDEV_RET_UNSUPPORTED     4

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

struct eps_pndev_hw_tag;
struct eps_pndev_callback_tag;
struct eps_pndev_location_tag;
struct eps_pndev_open_option_tag;
struct eps_pndev_hw_tag;

typedef enum
{
	EPS_PNDEV_ISR_UNDEFINED     = 0,
	EPS_PNDEV_ISR_ISOCHRONOUS   = 1,
	EPS_PNDEV_ISR_PN_GATHERED   = 2,
	EPS_PNDEV_ISR_PN_NRT        = 3
} EPS_PNDEV_INTERRUPT_DESC_TYPE, *EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE;

typedef enum
{
	EPS_PNDEV_ASIC_ERTEC200  = 0,
	EPS_PNDEV_ASIC_ERTEC400  = 1,
	EPS_PNDEV_ASIC_IRTEREV7  = 2,
	EPS_PNDEV_ASIC_ERTEC200P = 3,
	EPS_PNDEV_ASIC_INTEL_XX  = 4,
    EPS_PNDEV_ASIC_KSZ88XX   = 5
} EPS_PNDEV_ASIC_TYPE, *EPS_PNDEV_ASIC_PTR_TYPE;

typedef enum
{
	EPS_PNDEV_LOCATION_INVALID = 0,
	EPS_PNDEV_LOCATION_PCI     = 1,
	EPS_PNDEV_LOCATION_MAC     = 2
} EPS_PNDEV_LOCATION_SLECTOR_TYPE, *EPS_PNDEV_LOCATION_SLECTOR_PTR_TYPE;

typedef enum
{
	EPS_PNDEV_DEBUG_GPIO_1 = 1,
	EPS_PNDEV_DEBUG_GPIO_2 = 2
} EPS_PNDEV_GPIO_DESC_TYPE;

typedef void (*EPS_PNDEV_ISR_CBF)(LSA_UINT32 uParam, LSA_VOID* pArgs);

typedef struct eps_pndev_callback_tag
{
	EPS_PNDEV_ISR_CBF pCbf;
	LSA_UINT32 uParam;
	LSA_VOID*  pArgs;
} EPS_PNDEV_CALLBACK_TYPE, *EPS_PNDEV_CALLBACK_PTR_TYPE;

typedef LSA_UINT8 EPS_PNDEV_MAC_TYPE[6];  /* MAC Type */

typedef struct eps_pndev_location_tag
{
	EPS_PNDEV_LOCATION_SLECTOR_TYPE eLocationSelector;
	LSA_UINT32 uBusNr;
	LSA_UINT32 uDeviceNr;
	LSA_UINT32 uFctNr;
	EPS_PNDEV_MAC_TYPE Mac;
} EPS_PNDEV_LOCATION_TYPE, *EPS_PNDEV_LOCATION_PTR_TYPE;

typedef struct eps_pndev_open_option_tag
{
	LSA_BOOL bLoadFw;
	LSA_BOOL bDebug;
	LSA_BOOL bShmDebug;
	LSA_BOOL bShmHaltOnStartup;
} EPS_PNDEV_OPEN_OPTION_TYPE, *EPS_PNDEV_OPEN_OPTION_PTR_TYPE;

typedef struct eps_pndev_timer_ctrl_tag
{
	LSA_UINT32 uTimeoutUs;
} EPS_PNDEV_TIMER_CTRL_TYPE, *EPS_PNDEV_TIMER_CTRL_PTR_TYPE;

typedef LSA_VOID   (*EPS_PNDEV_UNINSTALL_FCT)   (LSA_VOID);
typedef LSA_UINT16 (*EPS_PNDEV_OPEN_FCT)        (EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, struct eps_pndev_hw_tag** ppHwInstOut, LSA_UINT16 hd_id);
typedef LSA_UINT16 (*EPS_PNDEV_CLOSE_FCT)       (struct eps_pndev_hw_tag* pHwInstIn);
typedef LSA_UINT16 (*EPS_PNDEV_ENABLE_ISR_FCT)  (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
typedef LSA_UINT16 (*EPS_PNDEV_DISABLE_ISR_FCT) (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt);
typedef LSA_UINT16 (*EPS_PNDEV_SET_GPIO)        (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
typedef LSA_UINT16 (*EPS_PNDEV_CLEAR_GPIO)      (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
typedef LSA_UINT16 (*EPS_PNDEV_TIMER_CTRL_START)(struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
typedef LSA_UINT16 (*EPS_PNDEV_TIMER_CTRL_STOP) (struct eps_pndev_hw_tag* pHwInstIn);
typedef LSA_UINT16 (*EPS_PNDEV_READ_TRACE_DATA) (struct eps_pndev_hw_tag* pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
typedef LSA_UINT16 (*EPS_PNDEV_WRITE_TRACE_DATA)(struct eps_pndev_hw_tag* pHwInstIn, LSA_UINT8* ptr, LSA_UINT32 offset, LSA_UINT32 size);
typedef LSA_UINT16 (*EPS_PNDEV_SAVE_DUMP)       (struct eps_pndev_hw_tag* pHwInstIn);

typedef struct eps_pndev_hw_tag
{	
	LSA_VOID* hDevice;	
	EPS_BOARD_INFO_TYPE  EpsBoardInfo;
	EPS_PNDEV_ENABLE_ISR_FCT  EnableIsr;
	EPS_PNDEV_DISABLE_ISR_FCT DisableIsr;
	EPS_PNDEV_SET_GPIO SetGpio;
	EPS_PNDEV_CLEAR_GPIO ClearGpio;
	EPS_PNDEV_TIMER_CTRL_START TimerCtrlStart;
	EPS_PNDEV_TIMER_CTRL_STOP TimerCtrlStop;
	EPS_PNDEV_READ_TRACE_DATA ReadTraceData;
	EPS_PNDEV_WRITE_TRACE_DATA WriteTraceData;
    EPS_PNDEV_SAVE_DUMP SaveDump;
	EPS_PNDEV_ASIC_TYPE  asic_type;
} EPS_PNDEV_HW_TYPE, *EPS_PNDEV_HW_PTR_TYPE, **EPS_PNDEV_HW_PTR_PTR_TYPE;

typedef struct
{
	EPS_PNDEV_OPEN_FCT open;
	EPS_PNDEV_CLOSE_FCT close;
	EPS_PNDEV_UNINSTALL_FCT uninstall;
}  EPS_PNDEV_IF_TYPE, *EPS_PNDEV_IF_PTR_TYPE;

typedef struct
{
	LSA_UINT16 uPciAddrValid;
	LSA_UINT16 uMacAddrValid;
	LSA_UINT16 uPciBusNr;
	LSA_UINT16 uPciDeviceNr;
	LSA_UINT16 uPciFuncNr;
	LSA_UINT16 uEddType;
	LSA_UINT8  uMacAddr[6];
    #if defined (EPS_CFG_USE_PNDEVDRV)
    ePNDEV_BOARD eBoard;
    #endif

} EPS_PNDEV_IF_DEV_INFO_TYPE, *EPS_PNDEV_IF_DEV_INFO_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_pndev_if_init(LSA_VOID);
LSA_VOID   eps_pndev_if_undo_init(LSA_VOID);
LSA_VOID   eps_pndev_if_fatal_close(LSA_VOID);
LSA_VOID   eps_pndev_if_register(EPS_PNDEV_IF_PTR_TYPE pPnDevIf);
LSA_UINT16 eps_pndev_if_open(EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndev_if_close(LSA_UINT16 hd_id);
LSA_UINT16 eps_pndev_if_get(LSA_UINT16 hd_id, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut);

LSA_VOID   eps_pndev_if_register_device(EPS_PNDEV_IF_DEV_INFO_PTR_TYPE pDev);
LSA_VOID   eps_pndev_if_get_registered_devices(EPS_PNDEV_IF_DEV_INFO_PTR_TYPE pDev, LSA_UINT16 uMaxDevices, LSA_UINT16* pCntDevices);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PN_DRV_IF_H_ */
