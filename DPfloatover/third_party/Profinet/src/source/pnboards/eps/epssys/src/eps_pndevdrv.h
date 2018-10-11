#ifndef EPS_PNDEVDRV_H_
#define EPS_PNDEVDRV_H_

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
/*  F i l e               &F: eps_yyy.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PnDevDrv Driver                                                      */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
// PAEA settings (light)
#define EPS_IOCC_BASE                                           0
#define EPS_U_IOCC_PA_EA_DIRECT_start                           (0x100000 + EPS_IOCC_BASE)
#define EPS_U_IOCC_PA_EA_DIRECT_end                             (0x18FFFC + EPS_IOCC_BASE)
#define EPS_IOCC_PAEA_START_SOC1                                EPS_U_IOCC_PA_EA_DIRECT_start

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID   eps_pndevdrv_install (LSA_VOID);
LSA_UINT16 eps_pndevdrv_open    (EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndevdrv_close   (EPS_PNDEV_HW_PTR_TYPE pHwInstIn);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PNDEVDRV_H_ */