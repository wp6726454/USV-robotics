#ifndef EPS_PLF_H_
#define EPS_PLF_H_

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
/*  F i l e               &F: eps_plf.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS HW/RTOS adapttion interface                                          */
/*                                                                           */
/*****************************************************************************/

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#include "psi_inc.h"
#include "eps_rtos.h"

#if defined (EPS_CFG_USE_PNDEVDRV)
#include <precomp.h>
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#endif

#define EPS_APP_FILE_CHUNK_SIZE   1024*128 /*128 KB*/

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef void (*EPS_PLF_ISR_CBF)(LSA_UINT32 uParam, LSA_VOID* pArgs);

typedef struct
{
	EPS_PLF_ISR_CBF pCbf;
	LSA_UINT32      uParam;
	LSA_VOID*       pArgs;
} EPS_PLF_ISR_CBF_TYPE, *EPS_PLF_ISR_CBF_PTR_TYPE;


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID EPS_PLF_REGISTER_SHM_IF(LSA_VOID* pShmIf);
LSA_VOID EPS_PLF_DISABLE_INTERRUPTS(LSA_VOID);
LSA_VOID EPS_PLF_ENABLE_INTERRUPTS(LSA_VOID);
long  EPS_PLF_EXCHANGE_LONG(long volatile *pAddr, long lVal);

/*****************************************************************************/
/*  HW Support Interface                                                     */
/*****************************************************************************/

LSA_UINT8* EPS_PLF_VIRTUAL_TO_PHYSICAL_ADDRESS(LSA_UINT8* pVirtAddress);

/*****************************************************************************/
/*  PCI Interface                                                            */
/*****************************************************************************/

typedef void (*EPS_PLF_PCI_ISR_CBF)(LSA_UINT32 uParam, LSA_VOID* pArgs);

typedef struct
{
	EPS_PLF_PCI_ISR_CBF pCbf;
	LSA_UINT32      uParam;
	LSA_VOID*       pArgs;
} EPS_PLF_PCI_ISR_CBF_TYPE, *EPS_PLF_PCI_ISR_CBF_PTR_TYPE;

typedef struct
{
	LSA_UINT16 uVendorId;
	LSA_UINT16 uDeviceId;
	LSA_UINT16 uBusNr;
	LSA_UINT16 uDeviceNr;
	LSA_UINT16 uFunctionNr;
	LSA_UINT8* pBase0;                 /* Bar 0 physical address            */
	LSA_UINT32 uSize0;                 /* Bar 0 size (max 32 Bit supported) */
	struct
	{
		LSA_BOOL bValid;               /* info struct is only used if bValid is set to LSA_TRUE */
		LSA_BOOL bMSISupport;
		LSA_BOOL bMSIXSupport;
		LSA_UINT32 Status;
		LSA_UINT32 Command;
		LSA_UINT32 IOApicLine;
		struct
		{
			LSA_UINT32 Bar;
			LSA_UINT32 BarSize;
			LSA_UINT32 BarMaskedVal;
		} Bar[6];
	} info;
} EPS_PLF_PCI_CONFIG_SPACE_TYPE, *EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE;

#ifdef EPS_USE_RTOS_ADONIS
typedef struct
{
	int fdEpsAdonisIsr;
	#if (EPS_PLF == EPS_PLF_ADONIS_X86)
	int fdStdOut;
	#endif
} EPS_PLF_ADONIS_TYPE, *EPS_PLF_ADONIS_PTR_TYPE;
#endif

LSA_BOOL   EPS_PLF_PCI_MAP_MEMORY(LSA_UINT8** pBase, LSA_UINT32 uBase, LSA_UINT32 uSize);
LSA_VOID   EPS_PLF_PCI_GET_DEVICES(LSA_UINT16 uVendorId, LSA_UINT16 uDeviceId, EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpaceOut, LSA_UINT16 uMaxDevice, LSA_UINT16 *pFoundDevices);
LSA_VOID   EPS_PLF_PCI_ENA_INTERRUPT(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpace, EPS_PLF_PCI_ISR_CBF_PTR_TYPE pCbf);
LSA_VOID   EPS_PLF_PCI_DIA_INTERRUPT(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpace);
LSA_VOID   EPS_PLF_PCI_READ_BYTE(LSA_UINT32 uBusNum, LSA_UINT32 uDeviceNum, LSA_UINT32 uFuncNum, LSA_UINT32 uOffset, LSA_UINT8* uVal);
LSA_VOID   EPS_PLF_PCI_READ_DOUBLE_WORD(LSA_UINT32 uBusNum, LSA_UINT32 uDeviceNum, LSA_UINT32 uFuncNum, LSA_UINT32 uOffset, LSA_UINT32* uVal);
LSA_UINT8* EPS_PLF_PCI_TRANSLATE_PCI_TO_LOCAL_ADDR(LSA_UINT8* uPciAddress);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PLF_H_ */
