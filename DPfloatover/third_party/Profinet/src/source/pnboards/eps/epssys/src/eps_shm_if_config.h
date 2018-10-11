#ifndef EPS_SHM_IF_CONFIG_H_
#define EPS_SHM_IF_CONFIG_H_

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
/*  F i l e               &F: eps_shm_if_config.h                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Shared Memory Interface Configuration Record                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef struct
{
	LSA_UINT32 bValid;     /* Configuration Record Valid = 1 */
	LSA_UINT32 uCfgLength; /* Length of configuration record = sizeof(EPS_SHM_UPPER_CONFIG_TYPE) */
	EPS_HIF_DRV_SHM_UPPER_CONFIG_TYPE HIF;
} EPS_SHM_UPPER_CONFIG_TYPE, *EPS_SHM_UPPER_CONFIG_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_SHM_IF_CONFIG_H_ */