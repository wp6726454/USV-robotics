#ifndef ACP_PLS_H
#define ACP_PLS_H

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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_pls.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  check consistency with EDD                                               */
/*                                                                           */
/*  check the configuration                                                  */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#ifndef ACP_CFG_MAX_CHANNELS
#error "ACP_CFG_MAX_CHANNELS not defined in acp_cfg.h"
#endif

#ifndef ACP_CFG_CAST_WARN
#error "ACP_CFG_CAST_WARN not defined in acp_cfg.h"
#endif

#ifndef ACP_CFG_RT_CLASS_3_SUPPORT_ON
#error "ACP_CFG_RT_CLASS_3_SUPPORT_ON not defined in acp_cfg.h"
#endif

#ifdef ACP_ASSERT_OR_FATAL
#error "ACP_ASSERT_OR_FATAL is no longer necessary, remove if from acp_cfg.h"
#endif

#ifndef ACP_FILE_SYSTEM_EXTENSION
#error "ACP_FILE_SYSTEM_EXTENSION not defined in acp_cfg.h"
#endif

#ifndef ACP_ASSERT
#error "ACP_ASSERT not defined in acp_cfg.h"
#endif

/*----------------------------------------------------------------------------*/

#if 1 <= ACP_INVALID_HANDLE  && ACP_INVALID_HANDLE <= ACP_CFG_MAX_CHANNELS
#error "ACP_CFG_MAX_CHANNELS wrong"
#endif

/*----------------------------------------------------------------------------*/

#if ACP_HW_TYPE_USED_MSK != EDD_HW_TYPE_USED_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_TYPE_USED_STANDARD_MAC != EDD_HW_TYPE_USED_STANDARD_MAC
# error "ACP != EDD"
#endif

#if ACP_HW_TYPE_USED_ERTEC_400 != EDD_HW_TYPE_USED_ERTEC_400
# error "ACP != EDD"
#endif

#if ACP_HW_TYPE_USED_ERTEC_200 != EDD_HW_TYPE_USED_ERTEC_200
# error "ACP != EDD"
#endif

#if ACP_HW_TYPE_USED_SOC != EDD_HW_TYPE_USED_SOC
# error "ACP != EDD"
#endif

#if ACP_HW_TYPE_USED_PNIP_200P != EDD_HW_TYPE_USED_ERTEC_200P
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_TIMESTAMP_MSK != EDD_HW_TYPE_FEATURE_TIMESTAMP_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_DELAY_MSK != EDD_HW_TYPE_FEATURE_DELAY_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_PARTIAL_DATA_CLASS12_MSK != EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_CT_MSK != EDD_HW_TYPE_FEATURE_CT_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_PARTIAL_DATA_CLASS3_MSK != EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_PARTIAL_DATA_CLASS_UDP_MSK != EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_AUTOPADDING_CLASS_UDP_MSK != EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_AUTOPADDING_CLASS12_MSK != EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_AUTOPADDING_CLASS3_MSK != EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_MSK
# error "ACP != EDD"
#endif

#if ACP_HW_FEATURE_APDUSTATUS_SEPERATE_MSK != EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE_MSK
# error "ACP != EDD"
#endif

/*----------------------------------------------------------------------------*/

#if ACP_DATASTATUS_PRIMARY != EDD_CSRT_DSTAT_BIT_STATE
# error "ACP != EDD"
#endif

#if ACP_DATASTATUS_REDUNDANCY != EDD_CSRT_DSTAT_BIT_REDUNDANCY
# error "ACP != EDD"
#endif

#if ACP_DATASTATUS_DATA_VALID != EDD_CSRT_DSTAT_BIT_DATA_VALID
# error "ACP != EDD"
#endif

#if ACP_DATASTATUS_BIT_3 != EDD_CSRT_DSTAT_BIT_3
# error "ACP != EDD"
#endif

#if ACP_DATASTATUS_RUN != EDD_CSRT_DSTAT_BIT_STOP_RUN
# error "ACP != EDD"
#endif

#if ACP_DATASTATUS_STATION_OK != EDD_CSRT_DSTAT_BIT_STATION_FAILURE
# error "ACP != EDD"
#endif

#if ACP_DATASTATUS_BIT_6 != EDD_CSRT_DSTAT_BIT_6
# error "ACP != EDD"
#endif

#if ACP_DATASTATUS_BIT_7 != EDD_CSRT_DSTAT_BIT_IGNORE
# error "ACP != EDD"
#endif

/*----------------------------------------------------------------------------*/

#if ACP_PORT_ID_AUTO != EDD_PORT_ID_AUTO
# error "ACP != EDD"
#endif

#if ACP_LINK_STATUS_UNKNOWN != EDD_LINK_UNKNOWN
# error "ACP != EDD"
#endif

#if ACP_LINK_STATUS_UP != EDD_LINK_UP
# error "ACP != EDD"
#endif

#if ACP_LINK_STATUS_DOWN != EDD_LINK_DOWN
# error "ACP != EDD"
#endif

#if ACP_LINK_STATUS_UP_CLOSED != EDD_LINK_UP_CLOSED
# error "ACP != EDD"
#endif

#if ACP_LINK_STATUS_UP_DISABLED != EDD_LINK_UP_DISABLED
# error "ACP != EDD"
#endif

#if ACP_LINK_SPEED_UNKNOWN != EDD_LINK_UNKNOWN
# error "ACP != EDD"
#endif

#if ACP_LINK_SPEED_10_M != EDD_LINK_SPEED_10
# error "ACP != EDD"
#endif

#if ACP_LINK_SPEED_100_M != EDD_LINK_SPEED_100
# error "ACP != EDD"
#endif

#if ACP_LINK_SPEED_1_G != EDD_LINK_SPEED_1000
# error "ACP != EDD"
#endif

#if ACP_LINK_SPEED_10_G != EDD_LINK_SPEED_10000
# error "ACP != EDD"
#endif

#if ACP_LINK_MODE_UNKNOWN != EDD_LINK_UNKNOWN
# error "ACP != EDD"
#endif

#if ACP_LINK_MODE_HALF_DUPLEX != EDD_LINK_MODE_HALF
# error "ACP != EDD"
#endif

#if ACP_LINK_MODE_FULL_DUPLEX != EDD_LINK_MODE_FULL
# error "ACP != EDD"
#endif


/*----------------------------------------------------------------------------*/

#if ACP_MULTICAST_ENABLE != EDD_MULTICAST_ENABLE
# error "ACP != EDD"
#endif

#if ACP_MULTICAST_DISABLE != EDD_MULTICAST_DISABLE
# error "ACP != EDD"
#endif


/*----------------------------------------------------------------------------*/
/* LSA */

#if !defined ACP_UPPER_IN_FCT_ATTR
#error "ACP_UPPER_IN_FCT_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_UPPER_OUT_FCT_ATTR
#error "ACP_UPPER_OUT_FCT_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_LOWER_IN_FCT_ATTR
#error "ACP_LOWER_IN_FCT_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_LOWER_OUT_FCT_ATTR
#error "ACP_LOWER_OUT_FCT_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_SYSTEM_IN_FCT_ATTR
#error "ACP_SYSTEM_IN_FCT_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_SYSTEM_OUT_FCT_ATTR
#error "ACP_SYSTEM_OUT_FCT_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_LOCAL_FCT_ATTR
#error "ACP_LOCAL_FCT_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_UPPER_RQB_ATTR
#error "ACP_UPPER_RQB_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_UPPER_MEM_ATTR
#error "ACP_UPPER_MEM_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_LOWER_RQB_ATTR
#error "ACP_LOWER_RQB_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_LOWER_MEM_ATTR
#error "ACP_LOWER_MEM_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_SYSTEM_MEM_ATTR
#error "ACP_SYSTEM_MEM_ATTR not defined in file acp_cfg.h"
#endif

#if !defined ACP_LOCAL_MEM_ATTR
#error "ACP_LOCAL_MEM_ATTR not defined in file acp_cfg.h"
#endif

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
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_PLS_H */
