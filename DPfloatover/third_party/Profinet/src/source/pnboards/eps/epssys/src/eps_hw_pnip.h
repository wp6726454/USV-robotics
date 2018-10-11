#ifndef EPS_HW_PNIP_H_
#define EPS_HW_PNIP_H_

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
/*  F i l e               &F: eps_hw_pnip.h                             :F&  */
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

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

#if (PSI_CFG_USE_EDDP == 1)

typedef enum
{
	EPS_HW_E200P_DEBUG_GPIO_1 = 1,
	EPS_HW_E200P_DEBUG_GPIO_2 = 2
} EPS_HW_E200P_GPIO_DESC_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID eps_hw_e200p_init                          ( LSA_UINT8 * pApbPerif );
LSA_VOID eps_hw_e200p_enable_isochronous_interrupt  ( LSA_UINT8 * pPnip, LSA_UINT8 * pApbPerif, LSA_BOOL bPnipIsIntern );
LSA_VOID eps_hw_e200p_disable_isochronous_interrupt ( LSA_UINT8 * pPnip, LSA_BOOL bPnipIsIntern );
LSA_VOID eps_hw_e200p_ack_isochronous_interrupt     ( LSA_UINT8 * pPnip, LSA_BOOL bPnipIsIntern );
LSA_VOID eps_hw_e200p_calculateSizeInSDRAM          ( LSA_UINT32  size, LSA_UINT32 * sizeNRT, LSA_UINT32 * sizeCRT );
LSA_VOID eps_hw_e200p_set_gpio                      ( LSA_UINT8 * pApbPerif, EPS_HW_E200P_GPIO_DESC_TYPE eGpio );
LSA_VOID eps_hw_e200p_clear_gpio                    ( LSA_UINT8 * pApbPerif, EPS_HW_E200P_GPIO_DESC_TYPE eGpio );

#endif  // PSI_CFG_USE_EDDP

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_HW_PNIP_H_ */