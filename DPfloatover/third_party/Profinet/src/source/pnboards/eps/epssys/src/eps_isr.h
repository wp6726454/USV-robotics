#ifndef EPS_ISR_H_
#define EPS_ISR_H_

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
/*  F i l e               &F: eps_isr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Interrupt handling                                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID eps_isr_init ( LSA_VOID* pShmHw );
LSA_VOID eps_eddi_exttimer_start ( PSI_SYS_HANDLE hSysDev );
LSA_VOID eps_eddi_exttimer_stop ( PSI_SYS_HANDLE hSysDev );
LSA_VOID eps_enable_pnio_ir ( PSI_SYS_HANDLE hSysDev );
LSA_VOID eps_disable_pnio_ir ( PSI_SYS_HANDLE hSysDev );
LSA_VOID eps_enable_iso_interrupt ( PSI_SYS_HANDLE hSysDev, void (*func_ptr)(int param, void* args), int param, void* args );
LSA_VOID eps_disable_iso_interrupt ( PSI_SYS_HANDLE hSysDev );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_ISR_H_ */