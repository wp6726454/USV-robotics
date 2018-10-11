#ifndef HIF_PLS_H
#define HIF_PLS_H

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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_pls.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  check consistency                                                        */
/*                                                                           */
/*  check the configuration                                                  */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#ifndef HIF_CFG_MAX_LD_INSTANCES
#error "HIF_CFG_MAX_LD_INSTANCES not defined in hif_cfg.h"
#endif

#ifndef HIF_CFG_MAX_HD_INSTANCES
#error "HIF_CFG_MAX_HD_INSTANCES not defined in hif_cfg.h"
#endif

#ifndef HIF_CFG_MAX_LD_CH_HANDLES
#error "HIF_CFG_MAX_LD_CH_HANDLES not defined in hif_cfg.h"
#endif

#ifndef HIF_CFG_MAX_HD_CH_HANDLES
#error "HIF_CFG_MAX_HD_CH_HANDLES not defined in hif_cfg.h"
#endif

#ifndef HIF_LD_MAX_HD
#error "HIF_LD_MAX_HD not defined in hif_cfg.h"
#endif

#ifndef HIF_LD_MAX_IF
#error "HIF_LD_MAX_IF not defined in hif_cfg.h"
#endif

#ifndef HIF_HD_MAX_IF
#error "HIF_HD_MAX_IF not defined in hif_cfg.h"
#endif

#ifndef HIF_MAX_COMP_ID
#error "HIF_MAX_COMP_ID not defined in hif_cfg.h"
#endif

#ifndef HIF_ASSERT
#error "HIF_ASSERT not defined in hif_cfg.h"
#endif
    
#ifndef HIF_CFG_MEM_MALLOC_ALIGN
#error "HIF_CFG_MEM_MALLOC_ALIGN not defined in hif_cfg.h"
#endif
    
#ifndef HIF_CFG_MEM_MALLOC_POOL_MAX
#error "HIF_CFG_MEM_MALLOC_POOL_MAX not defined in hif_cfg.h"
#endif

#ifndef HIF_CFG_MEM_RING_POOL_ALIGN
#error "HIF_CFG_MEM_RING_POOL_ALIGN not defined in hif_cfg.h"
#endif

#ifndef HIF_CFG_MEM_RING_POOL_MAX
#error "HIF_CFG_MEM_RING_POOL_MAX not defined in hif_cfg.h"
#endif

#if (defined (HIF_CFG_MEM_RING_CHECK_OVERWRITE)) && \
    (HIF_CFG_MEM_RING_CHECK_OVERWRITE != HIF_MEM_RING_CHECK_START_STOP_1) && \
    (HIF_CFG_MEM_RING_CHECK_OVERWRITE != HIF_MEM_RING_CHECK_START_STOP_4)
#error "HIF_CFG_MEM_RING_CHECK_OVERWRITE not correctly defined in file hif_cfg.h"
#endif
    
/*----------------------------------------------------------------------------*/
/* LSA */

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif
    
    
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*        */
/*****************************************************************************/
#endif  /* of HIF_PLS_H */
