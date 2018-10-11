#ifndef EPS_STDMACDRV_H_
#define EPS_STDMACDRV_H_

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
/*  F i l e               &F: eps_stdmacdrv.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.30         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-05-13                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS EDDS Intel NIC Driver                                                */
/*                                                                           */
/*****************************************************************************/

#if ( (PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_INTEL))
#include <edd_inc.h>           /* EDD Types                                */
#include <edds_int.h>          /* EDDS Types                               */
#include <intel_inc.h>         /* EDDS Lower Layer Intel Driver            */
#include <eps_plf.h>           /* EPS PCI Interface functions              */
#include <eps_hw_edds.h>       /* Own Header                               */
	

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif
/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_ISR_MODE_ISR    1   /* Always use ISR (Legacy + MSI)                                            */
#define EPS_ISR_MODE_POLL   2   /* Never use Interrupts, a polling thread is created                        */
#define EPS_ISR_MODE_MIXED  3   /* Only use interrupts if MSI is supported, create a poll thread for others */

#ifndef EPS_STDMACDRV_ISR_MODE
	#ifdef EPS_NO_MSI_USAGE
		#define EPS_STDMACDRV_ISR_MODE EPS_ISR_MODE_POLL
	#else
        #if (EPS_PLF == EPS_PLF_LINUX_X86)
		#define EPS_STDMACDRV_ISR_MODE EPS_ISR_MODE_ISR
        #else
		#define EPS_STDMACDRV_ISR_MODE EPS_ISR_MODE_MIXED
	#endif
#endif
#endif

#define EPS_CFG_STDMACDRV_MAX_BOARDS              10

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_STDMACDRV_PRINTF_OPEN_CLOSE_INFO
#define EPS_STDMACDRV_PRINTF_FOUND_BOARDS
#endif

/* - Typedefs ------------------------------------------------------------------------------------- */

typedef enum
{
	//DevId_Intel_82540em = 0x100E,
	DevId_Intel_IG82541PI = 0x1076,
	DevId_Intel_IG82540EM = 0x100F,
	DevId_Intel_GBEPCI_GT_Adapter = 0x107C,
	DevId_Intel_PCIe_CT_Adapter_82574L_Gigabit_Network = 0x10D3,
	DevId_Intel_Onboard_82567LM_3_Gigabit_Network = 0x10DE,
	DevId_Intel_Springville = 0x1533
} EPS_STDMACDRV_INTEL_PCI_DEVICE_IDS;

struct eps_stdmacdrv_store_tag;

typedef struct eps_stdmacdrv_device_info_tag
{
	EPS_PNDEV_LOCATION_TYPE sLoc;
	LSA_UINT8* pBase;
	LSA_UINT32 uSize;
	EPS_STDMACDRV_INTEL_PCI_DEVICE_IDS eDeviceId;
	EPS_PLF_PCI_CONFIG_SPACE_TYPE sConfigSpace;
} EPS_STDMACDRV_DEVICE_INFO_TYPE, *EPS_STDMACDRV_DEVICE_INFO_PTR_TYPE;

typedef struct eps_stdmacdrv_board_tag
{	
	LSA_BOOL bUsed;
	EPS_PNDEV_HW_TYPE sHw;
	LSA_UINT32 uCountIsrEnabled;	
	EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;
	EPS_SYS_TYPE sysDev;
	EPS_STDMACDRV_DEVICE_INFO_TYPE sDeviceInfo;
	EPS_EDDS_INTEL_LL_TYPE uEddsIntel;
	struct eps_stdmacdrv_store_tag* pBackRef;
} EPS_STDMACDRV_BOARD_TYPE, *EPS_STDMACDRV_BOARD_PTR_TYPE;

typedef struct eps_stdmacdrv_poll_tag
{
	LSA_BOOL   bRunning;
	LSA_UINT32 uCntUsers;
	LSA_UINT32 hThread;
	EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc[EPS_CFG_STDMACDRV_MAX_BOARDS];
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard[EPS_CFG_STDMACDRV_MAX_BOARDS];
} EPS_STDMACDRV_POLL_TYPE, *EPS_STDMACDRV_POLL_PTR_TYPE;

typedef struct eps_stdmacdrv_store_tag
{
	LSA_BOOL bInit;
	LSA_UINT16 hEnterExit;
	EPS_STDMACDRV_BOARD_TYPE board[EPS_CFG_STDMACDRV_MAX_BOARDS];
	LSA_UINT32 uCntFoundDevices;
	struct
	{
		EPS_STDMACDRV_POLL_TYPE PnGatheredIsr;
		EPS_STDMACDRV_POLL_TYPE PnNrtIsr;
	} poll;
} EPS_STDMACDRV_STORE_TYPE, *EPS_STDMACDRV_STORE_PTR_TYPE;

#endif  // #if ((PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_INTEL))

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID   eps_stdmacdrv_install (LSA_VOID);
LSA_UINT16 eps_stdmacdrv_open    (EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
LSA_UINT16 eps_stdmacdrv_close   (EPS_PNDEV_HW_PTR_TYPE pHwInstIn);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_STDMACDRV_H_ */
