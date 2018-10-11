#ifndef EPS_INTELLL_H_
#define EPS_INTELLL_H_

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
/*  F i l e               &F: eps_hw_edds.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS EDDS Interface Adaption                                              */
/*                                                                           */
/*****************************************************************************/

#include "eps_plf.h"

#if (PSI_CFG_USE_EDDS == 1) 
#include <edd_inc.h>           /* EDD Types                                */
#include <edds_int.h>          /* EDDS Types                               */

#define eps_edds_basereg_reg32(basereg, addr)	(((volatile LSA_UINT32*)basereg)[(addr)>>2])
#define eps_edds_basereg_reg16(basereg, addr)	(((volatile LSA_UINT16*)basereg)[(addr)>>1])
	
#if defined (EDDS_CFG_HW_INTEL)
#include <intel_inc.h>         /* EDDS Lower Layer Intel Driver            */	

#define EPS_INTEL_PCI_VENDOR_ID     	0x8086
#define	EPS_INTEL_PCI_DEVICE_ID_I210	0x1533
#define EPS_INTEL_PCI_DEVICE_ID_I82574  0x10D3
	
/* - Intel Register Access Defines ---------------------------------------------------------------- */
#define INTEL_RAL_0_15(number)		(0x05400+(8*(number)))
#define INTEL_RAH_0_15(number)		(0x05404+(8*(number)))
#endif


#if defined (EDDS_CFG_HW_KSZ88XX)
#include <ksz88xx_inc.h>         /* EDDS Lower Layer KSZ88XX Driver            */	

#define EPS_KSZ88XX_PCI_VENDOR_ID     		0x16C6	
#define	EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8841	0x8841  
#define	EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8842	0x8842  

/* - KSZ88XX Register Access Defines ---------------------------------------------------------------- */
#define KSZ88XX_MARL_0_15(number)		(0x0200+(8*(number)))
#define KSZ88XX_MARM_0_15(number)		(0x0202+(8*(number)))
#define KSZ88XX_MARH_0_15(number)		(0x0204+(8*(number)))
#endif

#if defined (EDDS_CFG_HW_INTEL)
typedef struct eps_edds_intel_ll_tag
{
	INTEL_LL_HANDLE_TYPE hLL;
	EDDS_LL_TABLE_TYPE   tLLFuncs;
	INTEL_LL_PARAMETER_TYPE sIntelLLParameter;

} EPS_EDDS_INTEL_LL_TYPE, *EPS_EDDS_INTEL_LL_PTR_TYPE;
#endif	
	
#if defined (EDDS_CFG_HW_KSZ88XX)	
typedef struct eps_edds_ll_ksz88xx_tag
{
	KSZ88XX_LL_HANDLE_TYPE 		hLL;
	EDDS_LL_TABLE_TYPE   		tLLFuncs;	
} EPS_EDDS_KSZ88XX_LL_TYPE, *EPS_EDDS_KSZ88XX_LL_PTR_TYPE;	
#endif
	
/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

#if defined (EDDS_CFG_HW_INTEL)
LSA_BOOL 	eps_intelLL_get_mac_addr(	LSA_UINT16	uVendorId, 
											LSA_UINT8* 	pBase, 
											LSA_UINT8* 	pMac);

LSA_UINT16 eps_intelLL_open(EPS_EDDS_INTEL_LL_PTR_TYPE  pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE		pHwIn,
							EPS_SYS_PTR_TYPE			pSysDevIn,							
							LSA_UINT8* 					pRegBaseIn,
							LSA_UINT16 					hd_idIn,
							LSA_UINT16					uVendorIdIn,
							LSA_UINT16					uDeviceIdIn);
#endif
							
#if defined (EDDS_CFG_HW_KSZ88XX)

LSA_UINT16 eps_ksz88xx_open(EPS_EDDS_KSZ88XX_LL_PTR_TYPE  pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE		  pHwIn,
							EPS_SYS_PTR_TYPE			  pSysDevIn,							
							LSA_UINT8* 					  pRegBaseIn,
							LSA_UINT16 				      hd_idIn,
							LSA_UINT16					  uVendorIdIn,
							LSA_UINT16					  uDeviceIdIn);
#endif		


#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_INTELLL_H_ */
