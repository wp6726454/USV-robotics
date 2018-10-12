#ifndef PSI_CFG_H /* ----- reinclude-protection ----- */
#define PSI_CFG_H

#ifdef __cplusplus /* If C++ - compiler: Use C linkage */
extern "C" {
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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System integration of LSA-component PSI.                                 */
/*  Using the PNIO framework.                                                */
/*  Note: In case of EPS common used keys are included from EPS.             */
/*                                                                           */
/*****************************************************************************/

/*
* included by "psi_inc.h"
*/

#define _EPSPSICFG_QUOTEME(x) #x
#define EPSPSICFG_QUOTEME(x) _EPSPSICFG_QUOTEME(x)

#ifndef EPS_PSI_CFG_PLATFORM_H
#error "EPS_PSI_CFG_PLATFORM_H not defined in makefiles"
#endif
#include EPSPSICFG_QUOTEME(EPS_PSI_CFG_PLATFORM_H) /* EPS specific PSI compile keys which need to be defined by compiler args (e.g. -DEPS_PSI_CFG_PLATFORM_H=pnboards_psi_cfg_plf_eps.h) */

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#ifndef EPS_PLF
#error "config error, EPS_PLF is not defined"
#endif

#ifndef PSI_DEBUG
#error "config error, PSI_DEBUG is not defined"
#endif

/*---------------------------------------------------------------------------*/
/* definition for invalid LOCK- and Event-Handles                            */
/*---------------------------------------------------------------------------*/

#define PSI_LOCK_ID_INVALID 0
#define PSI_EVENT_ID_INVALID 0
#define PSI_EVENT_NO_TIMEOUT 0

/*---------------------------------------------------------------------------*/
/* Type of systemhandles used by system for device and interface             */
/*---------------------------------------------------------------------------*/

#define PSI_SYS_HANDLE LSA_VOID_PTR_TYPE
#define PSI_EDD_HDDB LSA_VOID_PTR_TYPE
#define PSI_EDD_HIDB LSA_VOID_PTR_TYPE
#define PSI_BLOCKING_WAIT_HANDLE LSA_VOID_PTR_TYPE

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/
/* none PSI only supports flat memory modell */

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/
/* none PSI only supports flat memory modell */

/*===========================================================================*/
/* LSA-HEADER and LSA-TRAILER                                                */
/*===========================================================================*/

#define PSI_RQB_HEADER LSA_RQB_HEADER(PSI_UPPER_RQB_PTR_TYPE)

#define PSI_RQB_TRAILER LSA_RQB_TRAILER

#define PSI_RQB_SET_NEXT_RQB_PTR(rb, v) LSA_RQB_SET_NEXT_RQB_PTR(rb, v)
#define PSI_RQB_SET_PREV_RQB_PTR(rb, v) LSA_RQB_SET_PREV_RQB_PTR(rb, v)
#define PSI_RQB_SET_OPCODE(rb, v) LSA_RQB_SET_OPCODE(rb, v)
#define PSI_RQB_SET_HANDLE(rb, v) LSA_RQB_SET_HANDLE(rb, v)
#define PSI_RQB_SET_RESPONSE(rb, v) LSA_RQB_SET_RESPONSE(rb, v)
#define PSI_RQB_SET_USER_ID_PTR(rb, v) LSA_RQB_SET_USER_ID_PTR(rb, v)
#define PSI_RQB_SET_COMP_ID(rb, v) LSA_RQB_SET_COMP_ID(rb, v)

#define PSI_RQB_GET_NEXT_RQB_PTR(rb) LSA_RQB_GET_NEXT_RQB_PTR(rb)
#define PSI_RQB_GET_PREV_RQB_PTR(rb) LSA_RQB_GET_PREV_RQB_PTR(rb)
#define PSI_RQB_GET_OPCODE(rb) LSA_RQB_GET_OPCODE(rb)
#define PSI_RQB_GET_HANDLE(rb) LSA_RQB_GET_HANDLE(rb)
#define PSI_RQB_GET_RESPONSE(rb) LSA_RQB_GET_RESPONSE(rb)
#define PSI_RQB_GET_USER_ID_PTR(rb) LSA_RQB_GET_USER_ID_PTR(rb)
#define PSI_RQB_GET_COMP_ID(rb) LSA_RQB_GET_COMP_ID(rb)

/*------------------------------------------------------------------------------
// enable / disable PSI_ASSERT
//----------------------------------------------------------------------------*/

#if PSI_DEBUG

extern int psi_semicolon_missing; /* see psi_cfg.c */

/* check for condition: internal programming error */
#define PSI_ASSERT(cond) \
  if (!(cond)) {         \
    PSI_FATAL(0);        \
  } else                 \
  psi_semicolon_missing = 1

#else

/* no more programming error exists :-) */
#define PSI_ASSERT(cond)

#endif

/*------------------------------------------------------------------------------
// pointer test
//----------------------------------------------------------------------------*/

void* PSI_TEST_POINTER(void* ptr);
void* PSI_TEST_ALIGN2(void* ptr);
void* PSI_TEST_ALIGN4(void* ptr);

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef PSI_FILE_SYSTEM_EXTENSION
#define PSI_FILE_SYSTEM_EXTENSION(module_id_)
#endif

/*---------------------------------------------------------------------------*/
/* Configure PNIO packages based on PSI base configuration                   */
/*---------------------------------------------------------------------------*/

#if ((PSI_CFG_MAX_PNIO_IF_CNT < 1) || (PSI_CFG_MAX_PNIO_IF_CNT > 4))
#error "config of MAX PNIO IF invalid, check value ([1..4])"
#endif

#if (PSI_CFG_USE_IOD_MC != 0)
#error "don't activate, feature IOD MC not released"
#endif

#if (PSI_CFG_USE_SNMPX == 1) && (PSI_CFG_USE_SNMPX_AGT == 1)
#error "don't activate, feature SNMPX AGENT not released"
#endif

/*---------------------------------------------------------------------------*/
/* Configure Tracing                                                         */
/*---------------------------------------------------------------------------*/

#ifndef PSI_CFG_TRACE_MODE
#error "config error, PSI_CFG_TRACE_MODE not defined"
#else
#if (PSI_CFG_TRACE_MODE == 1)
#error "config error, PSI_CFG_TRACE_MODE not set to OFF, or indexed trace"
#endif
#endif

/*---------------------------------------------------------------------------*/
/* Configure Common                                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure ACP                                                             */
/*---------------------------------------------------------------------------*/

#define ACP_CFG_MAX_CHANNELS \
  (4 * PSI_CFG_MAX_PNIO_IF_CNT)  // System , UserIOC, UserIOD, UserIOM per NIC
#define ACP_CFG_RT_CLASS_3_SUPPORT_ON PSI_CFG_RT_CLASS_3_SUPPORT_ON
#define ACP_CFG_TRACE_MODE PSI_CFG_TRACE_MODE

/*---------------------------------------------------------------------------*/
/* Configure CLRPC                                                           */
/*---------------------------------------------------------------------------*/

#define CLRPC_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define CLRPC_CFG_USE_CASTS 1
#define CLRPC_CFG_MAX_CHANNELS \
  (3 * PSI_CFG_MAX_PNIO_IF_CNT)  // per NIC: 1(PATH_TYPE_EPM) +
                                 // 2(PATH_TYPE_USER, CM and Easy-Supervisor)
#define CLRPC_CFG_MAX_CLIENTS                                  \
  ((PSI_MAX_CL_DEVICES + PSI_MAX_SV_DEVICES * PSI_MAX_SV_AR) * \
   PSI_CFG_MAX_PNIO_IF_CNT)
#define CLRPC_CFG_MAX_SERVERS                         \
  (1 /*EPM*/ +                                        \
   (1 /*CM_CL*/ + 1 * PSI_MAX_SV_DEVICES /*CM-SV*/) * \
       PSI_CFG_MAX_PNIO_IF_CNT) /* 0 = no server-part */
#define CLRPC_CFG_MAX_SCALLS                                       \
  ((PSI_MAX_CL_DEVICES +                                           \
    PSI_MAX_SV_DEVICES * (PSI_MAX_SV_AR + PSI_MAX_SV_READ_IMPL)) * \
   PSI_CFG_MAX_PNIO_IF_CNT)
#define CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID \
  (PSI_MAX_SV_READ_IMPL * PSI_CFG_MAX_PNIO_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure CM                                                              */
/*---------------------------------------------------------------------------*/

#define PSI_MAX_CONNECTS_PER_SECOND \
  min(50,                           \
      PSI_MAX_CL_DEVICES) /* Count of devices CM can handle in parallel */ /*(LaM)Changed*/

#if (PSI_CFG_USE_MRP == 1)  // included
#define CM_CFG_USE_MRP 1
#else
#define CM_CFG_USE_MRP 0
#endif

#if (PSI_CFG_USE_GSY == 1)  // included
#define CM_CFG_USE_GSY 1
#else
#define CM_CFG_USE_GSY 0
#endif

#if (PSI_CFG_USE_POF == 1)  // included
#define CM_CFG_USE_POF 1
#else
#define CM_CFG_USE_POF 0
#endif

#if (PSI_CFG_USE_NARE == 1)  // included
#define CM_CFG_USE_NARE 1
#else
#define CM_CFG_USE_NARE 0
#endif

#define CM_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define CM_CFG_USE_CASTS 1
#define CM_CFG_RT_CLASS_2_SUPPORT_ON PSI_CFG_RT_CLASS_2_SUPPORT_ON
#define CM_CFG_RT_CLASS_3_SUPPORT_ON PSI_CFG_RT_CLASS_3_SUPPORT_ON
#define CM_CFG_MAX_NICS PSI_CFG_MAX_PNIO_IF_CNT  // at least 1 NIC (PNIO IF)
#define CM_CFG_SIMATIC_B000 1  // enable simatic indices always for pcIOX

// IOC
#define CM_CFG_MAX_CLIENTS (PSI_CFG_USE_IOC * PSI_CFG_MAX_PNIO_IF_CNT)
#define CM_CFG_MAX_CL_DEVICES PSI_MAX_CL_DEVICE_NR
#define CM_CFG_MAX_CL_IMPLICIT_READ PSI_MAX_CL_READ_IMPL
#define CM_CFG_MAX_CL_FSU_DEVICES PSI_MAX_CL_OSU_DEVICES
#define CM_CFG_CL_ALLOW_DIAG_AND_PDEV 1  // testing! for pcIOX only!
#define CM_CFG_MAX_CL_LOG_ENTRIES \
  12  // current session + IN_DATA/OFFLINE for old session
      // (see cm_cl_logbook_lbentry_type_enum (per client-device)

// IOM
#define CM_CFG_MAX_MULTICAST_CHANNELS \
  (PSI_CFG_USE_IOM * PSI_CFG_MAX_PNIO_IF_CNT)
#define CM_CFG_MAX_MC_DEVICES PSI_MAX_MC_DEVICE_NR
#define CM_CFG_MC_HAS_AUTOSTOP 0  // 0 = restart after offline.rsp,
// 1 = restart needs device_control(activate).req

// IOD
#define CM_CFG_MAX_SERVERS (PSI_CFG_USE_IOD * PSI_CFG_MAX_PNIO_IF_CNT)
#define CM_CFG_MAX_SV_DEVICES PSI_MAX_SV_DEVICES
#define CM_CFG_MAX_SV_ARS PSI_MAX_SV_AR
#define CM_CFG_SV_AR_HAS_MCONS PSI_CFG_USE_IOD_MC
#define CM_CFG_MAX_SV_LOG_ENTRIES 16  // per device (min=16, see part 6)

/*---------------------------------------------------------------------------*/
/* Configure DCP                                                             */
/*---------------------------------------------------------------------------*/

#define DCP_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define DCP_MAX_LOW_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)
#define DCP_MAX_CLIENT_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure EDDx (for all)                                                  */
/*---------------------------------------------------------------------------*/

#define EDD_CFG_MAX_PORT_CNT PSI_CFG_MAX_PORT_CNT
#define EDD_CFG_MAX_INTERFACE_CNT PSI_CFG_MAX_PNIO_IF_CNT
#define EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE \
  (PSI_CFG_MAX_PORT_CNT * PSI_CFG_MAX_PNIO_IF_CNT)
#define EDD_CFG_MAX_MRP_INSTANCE_CNT 2

/*---------------------------------------------------------------------------*/
/* Configure EDDI                                                            */
/*---------------------------------------------------------------------------*/

#define EDDI_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define EDDI_CFG_MAX_DEVICES PSI_CFG_MAX_HD_CNT
#define EDDI_CFG_MAX_CHANNELS (16 * PSI_CFG_MAX_PNIO_IF_CNT)
#define EDDI_CFG_MAX_SWITCH_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)

// pcIOX NIF uses:  EDDI SII standard mode (see eddi_cfg.h.txt for variants
#define EDDI_CFG_SII_ADD_PRIO1_AUX  // active at all product variants -
                                    // 2015-02-26

#if (EPS_PLF == \
     EPS_PLF_WINDOWS_X86)  // Light -> Polling Mode - Polling-Cycle ~1ms
#define EDDI_CFG_SII_POLLING_MODE
#elif (EPS_PLF == \
       EPS_PLF_ADONIS_X86)  // Advanced -> Polling Mode - Polling-Cycle ~1ms
#define EDDI_CFG_SII_POLLING_MODE
#elif (EPS_PLF == \
       EPS_PLF_LINUX_X86)  // Linux -> Polling Mode - Polling-Cycle ~1ms
#define EDDI_CFG_SII_POLLING_MODE
#elif (EPS_PLF == EPS_PLF_SOC_MIPS)  // BASIC MIPS (Soc) - ExtTimer + Interrupt
#define EDDI_CFG_SII_EXTTIMER_MODE_ON  // this settings are identical to PLC
// 15xx - 2015-02-26
//#define EDDI_CFG_SII_REMOVE_ISR_LOCKS     // currently not allowed, because
// eps calls eddi_interrupt in thread context (not in interrupt context)
#define EDDI_CFG_SII_INT_RESTART_MODE_LOOP
#elif (EPS_PLF == EPS_PLF_PNIP_ARM9)  // BASIC PNIP (EB200P) - EDDP!
// -> EDDP
#elif (EPS_PLF == EPS_PLF_ERTEC200_ARM9)  // BASIC ERTEC200 - Polling
#define EDDI_CFG_SII_POLLING_MODE
#elif (EPS_PLF == EPS_PLF_ERTEC400_ARM9)  // BASIC ERTEC400 - Polling
#define EDDI_CFG_SII_POLLING_MODE
#endif

/* Enable the automatic filling up to 3 bytes with 0 in NRT-Send-Buffer by EDDI
 */
/* Switching on is only allowed, if PSI_TCIP_CFG_COPY_ON_SEND is defined! */
#define PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT

/*---------------------------------------------------------------------------*/
/* Configure EDDP                                                            */
/*---------------------------------------------------------------------------*/

#define EDDP_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define EDDP_CFG_MAX_DEVICES PSI_CFG_MAX_HD_CNT
#define EDDP_CFG_MAX_CHANNELS (16 * PSI_CFG_MAX_PNIO_IF_CNT)
#define EDDP_CFG_MAX_USER_CMD_HANDLES (EDDP_CFG_MAX_DEVICES)

/*---------------------------------------------------------------------------*/
/* Configure EDDS                                                            */
/*---------------------------------------------------------------------------*/

#define EDDS_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define EDDS_CFG_MAX_DEVICES PSI_CFG_MAX_HD_CNT
#define EDDS_CFG_MAX_CHANNELS (16 * PSI_CFG_MAX_PNIO_IF_CNT)

#if defined(EDDS_CFG_HW_INTEL) || defined(EDDS_CFG_HW_KSZ88XX)
#define PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT 0
#else
#define PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT 1
#endif

/*---------------------------------------------------------------------------*/
/* Configure GSY                                                             */
/*---------------------------------------------------------------------------*/

#define GSY_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define GSY_CH_MAX_PORTS PSI_CFG_MAX_PORT_CNT
#define GSY_MAX_SYS_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)
#define GSY_MAX_USR_CHANNELS (1 /* Clock */ * PSI_CFG_MAX_PNIO_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure HIF                                                             */
/*---------------------------------------------------------------------------*/

#define HIF_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define HIF_LD_MAX_HD PSI_CFG_MAX_HD_CNT
#define HIF_HD_MAX_IF PSI_CFG_MAX_HD_IF_CNT
#define HIF_LD_MAX_IF PSI_CFG_MAX_PNIO_IF_CNT

#ifdef EPS_HD_ONLY
#define HIF_CFG_COMPILE_SERIALIZATION 1  // Use Serialization
#define HIF_CFG_MAX_LD_INSTANCES 0       // HD Variant => No LD
#define HIF_CFG_MAX_HD_INSTANCES 1       // anzahl der Hw Devices
#elif defined EPS_LOCAL_PN_USER
#define HIF_CFG_COMPILE_SERIALIZATION 0  // Don't use Serialization
#define HIF_CFG_MAX_LD_INSTANCES 2       // anzahl der USER LDs  *2
#define HIF_CFG_MAX_HD_INSTANCES \
  (PSI_CFG_MAX_HD_CNT * 2)  // ableiten aus MAX HD *2 fuer Upper/lower
#elif (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86)
#define HIF_CFG_COMPILE_SERIALIZATION 1  // Don't use Serialization
#define HIF_CFG_MAX_LD_INSTANCES 2       // anzahl der USER LDs  *2
#define HIF_CFG_MAX_HD_INSTANCES \
  (PSI_CFG_MAX_HD_CNT * 2)  // ableiten aus MAX HD *2 fuer Upper/lower
#else
#define HIF_CFG_COMPILE_SERIALIZATION 1  // Use Serialization
#define HIF_CFG_MAX_LD_INSTANCES 1       // anzahl der USER LDs
#ifndef HIF_CFG_MAX_HD_INSTANCES
#define HIF_CFG_MAX_HD_INSTANCES \
  (PSI_CFG_MAX_HD_CNT * 2)  // anzahl der Hw Devices * 2
#endif
#endif

#define HIF_CFG_MAX_LD_CH_HANDLES 100  // globale User + MX IF* IF Pipes
#define HIF_CFG_MAX_HD_CH_HANDLES 155  // Max IF * HD IF Pipes + HD Pipe

#define HIF_CFG_POLL_INTERVAL 100  // In PSI auf 100 setzen

#define HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS \
  0  // Don't use app specific HD Sock channels

#define HIF_CFG_USE_CLRPC 1  // PSI_CFG_USE_CLRPC always part of HIF! (LD + HD)!
#define HIF_CFG_USE_OHA 1    // PSI_CFG_USE_OHA   always part of HIF! (LD + HD)!
#define HIF_CFG_USE_PSI 1

#define HIF_CFG_USE_ACP PSI_CFG_USE_ACP
#define HIF_CFG_USE_CM PSI_CFG_USE_CM
#define HIF_CFG_USE_DCP PSI_CFG_USE_DCP
#define HIF_CFG_USE_EDD (PSI_CFG_USE_EDDI | PSI_CFG_USE_EDDP | PSI_CFG_USE_EDDS)
#define HIF_CFG_USE_GSY PSI_CFG_USE_GSY
#define HIF_CFG_USE_IOH PSI_CFG_USE_IOH
#define HIF_CFG_USE_LLDP PSI_CFG_USE_LLDP
#define HIF_CFG_USE_MRP PSI_CFG_USE_MRP
#define HIF_CFG_USE_NARE PSI_CFG_USE_NARE
#define HIF_CFG_USE_PNTRC 0
#define HIF_CFG_USE_SOCK PSI_CFG_USE_SOCK
#define HIF_CFG_USE_SNMPX PSI_CFG_USE_SNMPX
#define HIF_CFG_USE_HIF PSI_CFG_USE_HIF

#define HIF_CFG_USE_SND_WAIT_ACK  // Use Ack for Send/Receive
//#define HIF_ALLOC_LOCAL_MEM_WITH_NO_MEMSET

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
#define HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC
//#define HIF_ALLOC_LOCAL_MEM_WITH_NO_MEMSET //TODO still fixes in hif necassary
// to remove memset of local allocated memory
#define HIF_DEBUG_MEASURE_PERFORMANCE
#else
#define HIF_CFG_ALIGNMENT_SAVE_COPY_FUNCTION  // attention: for arm platforms
// (ertec200p, ertec200, ertec400)
// this define has to be set! 32
// bit copy does not work for
// misaligned 8/16 bit enums!
#endif

/**
  * Enables the HIF Special Mode (HSM) to get serialization in Windows
  * when running on a single core.
  **/
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
//#define HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
//#define HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT
#endif

/*---------------------------------------------------------------------------*/
/* Configure LLDP                                                            */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define LLDP_CFG_MAX_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure MRP                                                             */
/*---------------------------------------------------------------------------*/

#define MRP_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define MRP_NO_OF_PORTS PSI_CFG_MAX_PORT_CNT
#define MRP_MAX_NO_OF_INTERFACES PSI_CFG_MAX_PNIO_IF_CNT

/*---------------------------------------------------------------------------*/
/* Configure NARE                                                            */
/*---------------------------------------------------------------------------*/

#define NARE_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define NARE_CFG_TRACE_RQB_ERRORS 0 /* Off */
#define NARE_CFG_MAX_USER_CHANNELS \
  (3 * PSI_CFG_MAX_PNIO_IF_CNT) /* CM, OHA, NareAppChannel */
#define NARE_CFG_MAX_DCP_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)
#define NARE_CFG_MAX_EDD_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure OHA                                                             */
/*---------------------------------------------------------------------------*/

#define OHA_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define OHA_CFG_TRACE_RQB_ERRRORS 0 /* Off */
#define OHA_CFG_MAX_CHANNELS \
  (1 /* global */ + (4 * PSI_CFG_MAX_PNIO_IF_CNT /* IF User */))
#define OHA_CFG_MAX_NICS (1 + PSI_CFG_MAX_PNIO_IF_CNT)
#define OHA_CFG_MAX_PORTS PSI_CFG_MAX_PORT_CNT

#if (PSI_CFG_USE_MRP == 0)  // not used
#define OHA_CFG_NO_MRP      // Turn off MRP
#endif

#if (PSI_CFG_USE_NARE == 0)  // not used
#define OHA_CFG_NO_NARE      // Turn off NARE
#endif

/* The system capabilities contains the primary functions of the system (IEEE
 * 802.1AB) */
#define PSI_CFG_OHA_LLDP_SYSTEM_CAPABILITIES \
  (LLDP_TLV_SYSCAB_STATION_ONLY + LLDP_TLV_SYSCAB_BRIDGE)

/* The enabled capabilities contains the currently enabled functions of the
 * system capabilities (IEEE 802.1AB) */
#define PSI_CFG_OHA_LLDP_ENABLED_CAPABILITIES \
  (LLDP_TLV_SYSCAB_STATION_ONLY + LLDP_TLV_SYSCAB_BRIDGE)

/*---------------------------------------------------------------------------*/
/* Configure PNTRACE                                                         */
/*---------------------------------------------------------------------------*/

#define PNTRC_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define PNTRC_CFG_COMPILE_TEXT 1
#define PNTRC_CFG_COMPILE_MODULE_ID 1
#define PNTRC_CFG_COMPILE_FILE 1
#define PNTRC_CFG_COMPILE_LINE 1
#define PNTRC_CFG_LEVEL_DEFAULT 4
#define PNTRC_CFG_LOCAL_SYSTEM_ID 1 /* PNTRC_SYSTEM_ID_APP */
#define PNTRC_CFG_SYNC_INTERVAL \
  500 /* 500 ms Time Sync Interval to lower systems */
#define PNTRC_CFG_MAX_LOWER_CHANNELS PSI_CFG_MAX_HD_CNT

/*---------------------------------------------------------------------------*/
/* Configure POF                                                             */
/*---------------------------------------------------------------------------*/

#define POF_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define POF_CFG_MAX_CHANNELS (1 * PSI_CFG_MAX_PNIO_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure SNMPX                                                           */
/*---------------------------------------------------------------------------*/

#define SNMPX_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define SNMPX_CFG_TRACE_RQB_ERRORS 0 /* Off */
#define SNMPX_CFG_MAX_CHANNELS (1 /* pcIOX user */ + 1 /* SNMPX test dialog*/)
#define SNMPX_CFG_MAX_MANAGER_SESSIONS 32
#define SNMPX_CFG_MAX_NICS 1 /* only one global user */

#if (PSI_CFG_USE_SNMPX == 1) && (PSI_CFG_USE_SNMPX_MGR == 1)
#define SNMPX_CFG_SNMP_MANAGER
#endif

#if (PSI_CFG_USE_SNMPX == 1) && (PSI_CFG_USE_SNMPX_AGT == 1)
#define SNMPX_CFG_SNMP_AGENT
#endif

/*---------------------------------------------------------------------------*/
/* Configure SOCK                                                            */
/*---------------------------------------------------------------------------*/

#if (!defined(PSI_CFG_MAX_SOCK_APP_CHANNELS)) || \
    (PSI_CFG_MAX_SOCK_APP_CHANNELS < 1) || (PSI_CFG_MAX_SOCK_APP_CHANNELS > 8)
#error "PSI_CFG_MAX_SOCK_APP_CHANNELS not defined or out of range (1..8)"
#endif

#define SOCK_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define SOCK_CFG_MAX_CHANNELS \
  (3 + PSI_CFG_MAX_SOCK_APP_CHANNELS)  // global SOCK App Users + CLRPC + SNMPX
                                       // + OHA /*(LaM)Changed*/
#define SOCK_CFG_MAX_INTERFACE_CNT \
  PSI_CFG_MAX_PNIO_IF_CNT  // same as EDD_CFG_MAX_INTERFACE_CNT

/*----------------------------------------------------------*/
/* calculation of SOCK_CFG_MAX_CONNECTIONS                  */
/* same as TCIP_CFG_MAX_SOCKETS                             */
/*----------------------------------------------------------*/
#define PSI_MAX_SOCKETS_PCIOX_STATIC                                          \
  (1 /*EPM*/ + 1 /*SNMP (UDP)*/ + 1 /*WebSrv Listening*/ +                    \
   1 /*WebSrv Worker*/ + 1 /*ScriptSrv Listening*/ + 1 /*ScriptSrv Worker*/ + \
   1 /*ScriptSrv Client*/)
#ifdef PSI_CFG_USE_IOC
#define PSI_MAX_SOCKETS_IOC 1
#else
#define PSI_MAX_SOCKETS_IOC 0
#endif
#define PSI_CFG_MAX_SOCKETS                       \
  (PSI_MAX_SOCKETS_PCIOX_STATIC +                 \
   PSI_MAX_SV_DEVICES * PSI_CFG_MAX_PNIO_IF_CNT + \
   PSI_MAX_SOCKETS_IOC * PSI_CFG_MAX_PNIO_IF_CNT)
#define SOCK_CFG_MAX_CONNECTIONS PSI_CFG_MAX_SOCKETS
/*----------------------------------------------------------*/

#define SOCK_CFG_ENABLE_MULTICASTS PSI_CFG_SOCK_MULTICAST_SUPPORT

#if (PSI_CFG_USE_TCIP == 1)
#define SOCK_CFG_INTERNICHE 1  // Activate Interniche IP-Stack
#define SOCK_CFG_SUN 0
#define SOCK_CFG_LINUX386 0
#define SOCK_CFG_WINSOCK 0
#define SOCK_CFG_FUSION 0
#define SOCK_CFG_VXWORKS 0
#define SOCK_CFG_BSDVXW 0
#define SOCK_CFG_TINDK 0
#define SOCK_CFG_MSPS 0
#else
#define SOCK_CFG_WINSOCK 1
#define SOCK_CFG_INTERNICHE 0
#define SOCK_CFG_SUN 0
#define SOCK_CFG_LINUX386 0
#define SOCK_CFG_FUSION 0
#define SOCK_CFG_VXWORKS 0
#define SOCK_CFG_BSDVXW 0
#define SOCK_CFG_TINDK 0
#define SOCK_CFG_MSPS 0
#endif

/*---------------------------------------------------------------------------*/
/* Configure TCIP                                                            */
/*---------------------------------------------------------------------------*/

#include <eps_lib.h> /* Std Libs (target / OS specific) */

#define TCIP_CFG_INTERNICHE 1
#define TCIP_CFG_OPEN_BSD 0

// #ifndef min
// #define min(x,y)            LSA_MIN(x,y)
// #endif
// #ifndef max
// #define max(x,y)            LSA_MAX(x,y)
// #endif

#define TCIP_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define TCIP_CFG_MAX_PORT_COUNT PSI_CFG_MAX_PORT_CNT
#define TCIP_CFG_MAX_INTERFACES_TOTAL PSI_CFG_MAX_PNIO_IF_CNT
#define TCIP_CFG_MAX_SOCKETS SOCK_CFG_MAX_CONNECTIONS
#define TCIP_CFG_SOMAXCONN \
  10 /* maximum length of the TCP backlog (1-127, 0 defaults to 5) */
#define TCIP_CFG_ENABLE_MULTICAST PSI_CFG_SOCK_MULTICAST_SUPPORT

// #ifndef min
// #define min(x, y) LSA_MIN(x, y)
// #endif
// #ifndef max
// #define max(x, y) LSA_MAX(x, y)
// #endif

/*---------------------------------------------------------------------------*/
/* Configure IOH                                                             */
/*---------------------------------------------------------------------------*/

#define IOH_CFG_TRACE_MODE PSI_CFG_TRACE_MODE
#define IOH_CFG_MAX_CHANNELS PSI_CFG_MAX_PNIO_IF_CNT  // one userchannel per IF
#define IOH_CFG_MAX_DEVICES PSI_CFG_MAX_PNIO_IF_CNT   //!!!UL TODO APICHANGE?
#define IOH_CFG_MAX_RUNNING_JOBS 256     // max number of jobs per IF
#define IOH_CFG_MAX_RUNNING_ISO_JOBS 16  // max number of isochronous jobs per
                                         // IF

#define IOH_CFG_MAX_PI_BUFFERS_IOC (PSI_MAX_CL_DEVICES)  // max number for IOC
#define IOH_CFG_MAX_PI_BUFFERS_IOD \
  (PSI_MAX_SV_DEVICES * PSI_MAX_SV_IO_ARS)  // max number for IOD

#define IOH_CFG_MAX_PI_BUFFERS        \
  LSA_MAX(IOH_CFG_MAX_PI_BUFFERS_IOC, \
          IOH_CFG_MAX_PI_BUFFERS_IOD)  // either IOD or IOC

#define IOH_CFG_MAX_CONSUMER_PI_BUFFERS IOH_CFG_MAX_PI_BUFFERS
#define IOH_CFG_MAX_PROVIDER_PI_BUFFERS IOH_CFG_MAX_PI_BUFFERS

/*---------------------------------------------------------------------------*/
/* stdLib                                                                    */
/*---------------------------------------------------------------------------*/
#define PSI_MEMCPY eps_memcpy
#define PSI_MEMCMP eps_memcmp
#define PSI_MEMSET eps_memset
#define PSI_STRLEN eps_strlen

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* of PSI_CFG_H */
