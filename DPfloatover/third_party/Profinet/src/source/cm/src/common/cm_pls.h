#ifndef CM_PLS_H
#define CM_PLS_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_pls.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Check plausibility                                                       */
/*                                                                           */
/*****************************************************************************/

#ifndef PNIO_VERSION
# error "PNIO_VERSION not defined, you must include pnio_version.h in file cm_inc.h"
#endif

#ifndef PNIO_DISTRIBUTION
# error "PNIO_DISTRIBUTION not defined, you must include pnio_version.h in file cm_inc.h"
#endif

#ifndef PNIO_PROJECT_NUMBER
# error "PNIO_PROJECT_NUMBER not defined, you must include pnio_version.h in file cm_inc.h"
#endif

#ifndef PNIO_INCREMENT
# error "PNIO_INCREMENT not defined, you must include pnio_version.h in file cm_inc.h"
#endif

#ifndef PNIO_INTEGRATION_COUNTER
# error "PNIO_INTEGRATION_COUNTER not defined, you must include pnio_version.h in file cm_inc.h"
#endif

#ifndef PNIO_GEN_COUNTER
# error "PNIO_GEN_COUNTER not defined, you must include pnio_version.h in file cm_inc.h"
#endif

/*===========================================================================*/

#ifndef CM_FILE_SYSTEM_EXTENSION
# error "CM_FILE_SYSTEM_EXTENSION not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#ifndef CM_CFG_TRACE_MODE
# error "CM_CFG_TRACE_MODE not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#ifndef CM_DEBUG
# error "CM_DEBUG not defined in cm_cfg.h!"
#endif

/*===========================================================================*/

#ifndef CM_ASSERT
# error "CM_ASSERT() not defined in cm_cfg.h!"
#endif

/*===========================================================================*/

#ifndef CM_CFG_USE_CASTS
# error "CM_CFG_USE_CASTS not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#ifndef CM_CFG_USE_MRP
# error "CM_CFG_USE_MRP not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#ifndef CM_CFG_USE_GSY
# error "CM_CFG_USE_GSY not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#ifndef CM_CFG_USE_POF
# error "CM_CFG_USE_POF not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#ifndef CM_CFG_USE_NARE
# error "CM_CFG_USE_NARE not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#ifdef CM_CFG_USE_REMA
# error "CM_CFG_USE_REMA is defined. functionality of REMA was moved to OHA. please remove the define from cm_cfg.h"
#endif

/*===========================================================================*/

#ifdef CM_CFG_USE_ACP
# error "CM_CFG_USE_ACP must not be defined in cm_cfg.h. see definition some lines below!"
#endif

/*===========================================================================*/

#ifdef CM_CFG_HAS_BUFFERS
# error "CM_CFG_HAS_BUFFERS is defined. functionality was moved to EDDS (EDDS_IOBuffer_xxx, AP01403254). please remove the define from cm_cfg.h"
#endif

/*===========================================================================*/
/*===========================================================================*/

#ifndef CM_CFG_MAX_NICS
# error "CM_CFG_MAX_NICS not defined in file cm_cfg.h"
#elif CM_CFG_MAX_NICS < 1
# error "CM_CFG_MAX_NICS too small"
#endif

/*===========================================================================*/

#if CM_CFG_USE_GSY
#ifndef CM_CFG_MAX_PTCPSUBDOMAINS
# error "CM_CFG_MAX_PTCPSUBDOMAINS not defined in file cm_cfg.h"
#elif CM_CFG_MAX_PTCPSUBDOMAINS < 1
# error "CM_CFG_MAX_PTCPSUBDOMAINS too small"
#endif
#endif

/*===========================================================================*/

#ifndef CM_CFG_SIMATIC_B000
# error "CM_CFG_SIMATIC_B000 not defined in file cm_cfg.h"
#endif

#ifdef CM_CFG_SIMATIC_7F00
# error "CM_CFG_SIMATIC_7F00 was replaced by CM_CFG_SIMATIC_B000"
#endif

/*===========================================================================*/

#ifndef CM_CFG_RT_CLASS_2_SUPPORT_ON
# error "CM_CFG_RT_CLASS_2_SUPPORT_ON not defined in file cm_cfg.h"
#endif

#ifndef CM_CFG_RT_CLASS_3_SUPPORT_ON
# error "CM_CFG_RT_CLASS_3_SUPPORT_ON not defined in file cm_cfg.h"
#endif

#if CM_CFG_RT_CLASS_3_SUPPORT_ON != 0
# if CM_CFG_RT_CLASS_2_SUPPORT_ON == 0 /* don't know how to disable it, there is only one sync-record */
#  error "CM_CFG_RT_CLASS_2_SUPPORT_ON == 0 cannot be configured if CM_CFG_RT_CLASS_3_SUPPORT_ON != 0"
# endif
#endif

#if CM_CFG_USE_GSY == 0
# if CM_CFG_RT_CLASS_3_SUPPORT_ON != 0
#  error "CM_CFG_RT_CLASS_3_SUPPORT_ON != 0 cannot be configured if CM_CFG_USE_GSY == 0"
# endif
# if CM_CFG_RT_CLASS_2_SUPPORT_ON != 0
#  error "CM_CFG_RT_CLASS_2_SUPPORT_ON != 0 cannot be configured if CM_CFG_USE_GSY == 0"
# endif
#endif

/*===========================================================================*/

#ifdef CM_CFG_ALLOW_ALL_ALARMTYPES
# error "CM_CFG_ALLOW_ALL_ALARMTYPES is defined. functionality was removed with V5.3i1.10 (AP01476188). please remove the define from cm_cfg.h"
#endif

/*===========================================================================*/

#ifdef CM_CFG_ALLOW_PULL_MODULE_ALARM
# error "CM_CFG_ALLOW_PULL_MODULE_ALARM was a temporary define for V4.0 and AP00517543. Please remove it."
#endif

/*===========================================================================*/

#ifndef CM_CFG_MAX_CLIENTS
# error "CM_CFG_MAX_CLIENTS not defined in file cm_cfg.h"
#elif CM_CFG_MAX_CLIENTS < 0
# error "CM_CFG_MAX_CLIENTS must not be negative"
#elif CM_CFG_MAX_CLIENTS > CM_CFG_MAX_NICS
# error "CM_CFG_MAX_CLIENTS must be 0..CM_CFG_MAX_NICS"
#endif

#if CM_CFG_MAX_CLIENTS

#ifndef CM_CFG_MAX_CL_DEVICES
# error "CM_CFG_MAX_CL_DEVICES not defined in file cm_cfg.h"
#endif

#if CM_CFG_MAX_CL_DEVICES > 2048
# error "CM_CFG_MAX_CL_DEVICES wrong size"
#endif

#ifndef CM_CFG_MAX_CL_LOG_ENTRIES
# error "CM_CFG_MAX_CL_LOG_ENTRIES not defined in file cm_cfg.h"
#endif

#ifndef CM_CFG_MAX_CL_IMPLICIT_READ
# error "CM_CFG_MAX_CL_IMPLICIT_READ not defined in file cm_cfg.h"
#endif

#ifndef CM_CFG_MAX_CL_FSU_DEVICES
# error "CM_CFG_MAX_CL_FSU_DEVICES not defined in file cm_cfg.h"
#endif

#ifndef CM_CFG_CL_ALLOW_DIAG_AND_PDEV
# define CM_CFG_CL_ALLOW_DIAG_AND_PDEV 0
#endif

#endif

/*===========================================================================*/

#ifndef CM_CFG_MAX_SERVERS
# error "CM_CFG_MAX_SERVERS not defined in file cm_cfg.h"
#elif CM_CFG_MAX_SERVERS < 0
# error "CM_CFG_MAX_SERVERS must not be negative"
#elif CM_CFG_MAX_SERVERS > CM_CFG_MAX_NICS
# error "CM_CFG_MAX_SERVERS must be 0..CM_CFG_MAX_NICS"
#endif

#if CM_CFG_MAX_SERVERS

#ifndef CM_CFG_MAX_SV_DEVICES
# error "CM_CFG_MAX_SV_DEVICES not defined in file cm_cfg.h"
#elif CM_CFG_MAX_SV_DEVICES < 1
# error "CM_CFG_MAX_SV_DEVICES must not be less than 1"
#elif CM_CFG_MAX_SV_DEVICES > 128
# error "CM_CFG_MAX_SV_DEVICES must not be greater than 128"
#endif

#ifndef CM_CFG_MAX_SV_LOG_ENTRIES
# error "CM_CFG_MAX_SV_LOG_ENTRIES not defined in file cm_cfg.h"
#elif CM_CFG_MAX_SV_LOG_ENTRIES < 1
# error "CM_CFG_MAX_SV_LOG_ENTRIES too small"
#elif CM_CFG_MAX_SV_LOG_ENTRIES > ((0xFFFF - (1+1+8+2)) / (8+16+4+4))
# error "CM_CFG_MAX_SV_LOG_ENTRIES too big - limited due to BlockLength of LogBookData-Record"
#endif

#ifndef CM_CFG_MAX_SV_ARS
# error "CM_CFG_MAX_SV_ARS not defined in file cm_cfg.h"
#elif CM_CFG_MAX_SV_ARS < 1
# error "CM_CFG_MAX_SV_ARS must not be less than 1"
#elif CM_CFG_MAX_SV_ARS > 0x7F /* ar_nr = (0),1..127 */
# error "CM_CFG_MAX_SV_ARS must not be greater than 127"
#endif

#ifndef CM_CFG_SV_AR_HAS_MCONS
# error "CM_CFG_SV_AR_HAS_MCONS not defined in file cm_cfg.h"
#elif CM_CFG_SV_AR_HAS_MCONS != 0
# error "CM_CFG_SV_AR_HAS_MCONS must be 0 due to AP00774501"
#elif CM_CFG_SV_AR_HAS_MCONS != 0 && CM_CFG_USE_NARE == 0
# error "CM_CFG_SV_AR_HAS_MCONS needs CM_CFG_USE_NARE"
#endif

#ifdef CM_CFG_SV_AR_CHECK_ALL
# error "CM_CFG_SV_AR_CHECK_ALL is obsolete (AP00826555, V4.2). please remove the define from cm_cfg.h"
#endif

#ifndef CM_CFG_SV_WEAK_PDEV_CHECK /* AP01409358, note: don't define it if you don't know the exact usescase */
#define CM_CFG_SV_WEAK_PDEV_CHECK 0 /* silently set to "normal" */
#endif

#endif

/*===========================================================================*/

#ifndef CM_CFG_MAX_MULTICAST_CHANNELS
# error "CM_CFG_MAX_MULTICAST_CHANNELS not defined in file cm_cfg.h"
#elif CM_CFG_MAX_MULTICAST_CHANNELS < 0
# error "CM_CFG_MAX_MULTICAST_CHANNELS must not be negative"
#elif CM_CFG_MAX_MULTICAST_CHANNELS > CM_CFG_MAX_NICS
# error "CM_CFG_MAX_MULTICAST_CHANNELS must be 0..CM_CFG_MAX_NICS"
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS

#ifndef CM_CFG_MAX_MC_DEVICES
#error "CM_CFG_MAX_MC_DEVICES not defined in file cm_cfg.h"
#endif

#if CM_CFG_MAX_MC_DEVICES > 2048
# error "CM_CFG_MAX_MC_DEVICES wrong size"
#endif

#ifndef CM_CFG_MC_HAS_AUTOSTOP
#error "CM_CFG_MC_HAS_AUTOSTOP not defined in file cm_cfg.h"
#endif

#if CM_CFG_USE_NARE == 0
# error "CM_CFG_MAX_MULTICAST_CHANNELS needs CM_CFG_USE_NARE"
#endif

#endif

/*===========================================================================*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_MAX_SERVERS
# define CM_CFG_USE_ACP 1
#else
# define CM_CFG_USE_ACP 0 /* "minimal PDEV" (AP00565264) */
#endif

/*===========================================================================*/

#if !defined CM_UPPER_IN_FCT_ATTR
#error "CM_UPPER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_UPPER_OUT_FCT_ATTR
#error "CM_UPPER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_SYSTEM_IN_FCT_ATTR
#error "CM_SYSTEM_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_SYSTEM_OUT_FCT_ATTR
#error "CM_SYSTEM_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_LOCAL_FCT_ATTR
#error "CM_LOCAL_FCT_ATTR not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#if !defined CM_ACP_LOWER_IN_FCT_ATTR
#error "CM_ACP_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_ACP_LOWER_OUT_FCT_ATTR
#error "CM_ACP_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_EDD_LOWER_IN_FCT_ATTR
#error "CM_EDD_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_EDD_LOWER_OUT_FCT_ATTR
#error "CM_EDD_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_MRP_LOWER_IN_FCT_ATTR
#error "CM_MRP_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_MRP_LOWER_OUT_FCT_ATTR
#error "CM_MRP_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_GSY_LOWER_IN_FCT_ATTR
#error "CM_GSY_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_GSY_LOWER_OUT_FCT_ATTR
#error "CM_GSY_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_POF_LOWER_IN_FCT_ATTR
#error "CM_POF_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_POF_LOWER_OUT_FCT_ATTR
#error "CM_POF_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_OHA_LOWER_IN_FCT_ATTR
#error "CM_OHA_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_OHA_LOWER_OUT_FCT_ATTR
#error "CM_OHA_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_NARE_LOWER_IN_FCT_ATTR
#error "CM_NARE_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_NARE_LOWER_OUT_FCT_ATTR
#error "CM_NARE_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_RPC_LOWER_IN_FCT_ATTR
#error "CM_RPC_LOWER_IN_FCT_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_RPC_LOWER_OUT_FCT_ATTR
#error "CM_RPC_LOWER_OUT_FCT_ATTR not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#if !defined CM_COMMON_MEM_ATTR
#error "CM_COMMON_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_UPPER_RQB_ATTR
#error "CM_UPPER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_UPPER_MEM_ATTR
#error "CM_UPPER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_SYSTEM_MEM_ATTR
#error "CM_SYSTEM_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_LOCAL_MEM_ATTR
#error "CM_LOCAL_MEM_ATTR not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#if !defined CM_ACP_LOWER_RQB_ATTR
#error "CM_ACP_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_ACP_LOWER_MEM_ATTR
#error "CM_ACP_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_EDD_LOWER_RQB_ATTR
#error "CM_EDD_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_EDD_LOWER_MEM_ATTR
#error "CM_EDD_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_MRP_LOWER_RQB_ATTR
#error "CM_MRP_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_MRP_LOWER_MEM_ATTR
#error "CM_MRP_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_GSY_LOWER_RQB_ATTR
#error "CM_GSY_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_GSY_LOWER_MEM_ATTR
#error "CM_GSY_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_POF_LOWER_RQB_ATTR
#error "CM_POF_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_POF_LOWER_MEM_ATTR
#error "CM_POF_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_OHA_LOWER_RQB_ATTR
#error "CM_OHA_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_OHA_LOWER_MEM_ATTR
#error "CM_OHA_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_NARE_LOWER_RQB_ATTR
#error "CM_NARE_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_NARE_LOWER_MEM_ATTR
#error "CM_NARE_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_RPC_LOWER_RQB_ATTR
#error "CM_RPC_LOWER_RQB_ATTR not defined in file cm_cfg.h"
#endif

#if !defined CM_RPC_LOWER_MEM_ATTR
#error "CM_RPC_LOWER_MEM_ATTR not defined in file cm_cfg.h"
#endif

/*===========================================================================*/

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif

/*===========================================================================*/

#ifndef CM_LOCAL_MEM_DYNAMIC
# error "inconsistency: CM_LOCAL_MEM_DYNAMIC not defined, but cm uses dynamic allocation!"
#endif

#ifndef CM_NO_CLEAR_GLOBAL_DATA
# error "inconsistency: CM_NO_CLEAR_GLOBAL_DATA not defined, but cm clears global data internally!"
#endif

#if !defined CM_LOCAL_MEM_STATIC && !defined CM_LOCAL_MEM_DYNAMIC
#error "CM_LOCAL_MEM_STATIC or CM_LOCAL_MEM_DYNAMIC is missing in file cm_cfg.h!"
#endif

#if defined CM_LOCAL_MEM_STATIC && defined CM_LOCAL_MEM_DYNAMIC
#error "Define CM_LOCAL_MEM_STATIC or CM_LOCAL_MEM_DYNAMIC in file cm_cfg.h!"
#endif

#if !defined CM_CLEAR_GLOBAL_DATA && !defined CM_NO_CLEAR_GLOBAL_DATA
#error "CM_CLEAR_GLOBAL_DATA or CM_NO_CLEAR_GLOBAL_DATA is missing in file cm_cfg.h!"
#endif

#if defined CM_CLEAR_GLOBAL_DATA && defined CM_NO_CLEAR_GLOBAL_DATA
#error "Define CM_CLEAR_GLOBAL_DATA or CM_NO_CLEAR_GLOBAL_DATA in file cm_cfg.h!"
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_PLS_H */
