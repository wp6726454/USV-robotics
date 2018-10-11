#ifndef PND_TRC_H                     /* ----- reinclude-protection ----- */
#define PND_TRC_H

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_trc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface for lsa-trace                                           */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/* Usage of traces within PnDriver:                                          */
/*                                                                           */
/* Trace-Subsystems used                                                     */
/*                                                                           */
/* TRACE_SUBSYS_PND_SYSTEM : Used in PnDriver common traces                  */
/* TRACE_SUBSYS_PND_PD     : Used for PnDriver PD user traces                */
/* TRACE_SUBSYS_PND_IOC    : Used for PnDriver IOC user traces               */
/* TRACE_SUBSYS_PND_IOH    : Used for PnDriver IODU user traces              */
/* TRACE_SUBSYS_PND_OHA    : Used for PnDriver OHA user traces               */
/* TRACE_SUBSYS_PND_PNTRC  : Used for PnDriver PNTRC user traces             */
/* TRACE_SUBSYS_PND_IODU   : Used for PnDriver IODU traces                   */
/* TRACE_SUBSYS_PND_PARAM  : Used for PnDriver PARAMS traces                 */
/* TRACE_SUBSYS_PND_AGENT  : Used for PnDriver Agent traces                  */
/* TRACE_SUBSYS_PND_IOBC   : Used for PnDriver IO-Base core traces           */
/* TRACE_SUBSYS_PND_PNUC   : Used for PnDriver PNIO user core traces         */
/*                                                                           */
/* Trace-Level usage:                                                        */
/*                                                                           */
/* LSA_TRACE_LEVEL_FATAL      :    Fatal errors                              */
/* LSA_TRACE_LEVEL_ERROR      :    Errors                                    */
/* LSA_TRACE_LEVEL_UNEXP      :    Unexpected errors                         */
/* LSA_TRACE_LEVEL_NOTE_HIGH  :    More important situations                 */
/* LSA_TRACE_LEVEL_NOTE       :    Upper/Lower/System calls/responses        */
/*                                 More important notes                      */
/* LSA_TRACE_LEVEL_NOTE_LOW   :    Normal program flow                       */
/* LSA_TRACE_LEVEL_CHAT       :    Function in/out                           */
/*                                                                           */
/*===========================================================================*/
/* Usage of idx_                                                             */
/* 0             Traces of global PNIO IF                                    */
/* 1-16          IF specific Tracing for PNIO-IF 1..16                       */
/*===========================================================================*/

#if ( PSI_CFG_TRACE_MODE == 0 ) /* no trace */

#define PND_SYSTEM_TRACE_00(idx_, level_, msg_)
#define PND_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define PND_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_PD_TRACE_00(idx_, level_, msg_)
#define PND_PD_TRACE_01(idx_, level_, msg_, para1_)
#define PND_PD_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_PD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_PD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_PD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_PD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_PD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_PD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_PD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_PD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_PD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_PD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_IOC_TRACE_00(idx_, level_, msg_)
#define PND_IOC_TRACE_01(idx_, level_, msg_, para1_)
#define PND_IOC_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_IOC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_IOC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_IOC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_IOC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_IOC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_IOC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_IOC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_IOC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_IOC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_IOC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_IOH_TRACE_00(idx_, level_, msg_)
#define PND_IOH_TRACE_01(idx_, level_, msg_, para1_)
#define PND_IOH_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_IOH_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_IOH_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_IOH_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_IOH_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_IOH_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_IOH_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_IOH_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_IOH_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_IOH_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_IOH_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_OHA_TRACE_00(idx_, level_, msg_)
#define PND_OHA_TRACE_01(idx_, level_, msg_, para1_)
#define PND_OHA_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_OHA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_OHA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_OHA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_OHA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_OHA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_OHA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_OHA_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_OHA_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_OHA_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_OHA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_PNTRC_TRACE_00(idx_, level_, msg_)
#define PND_PNTRC_TRACE_01(idx_, level_, msg_, para1_)
#define PND_PNTRC_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_PNTRC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_PNTRC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_PNTRC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_PNTRC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_PNTRC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_PNTRC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_PNTRC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_PNTRC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_PNTRC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_PNTRC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_IODU_TRACE_00(idx_, level_, msg_)
#define PND_IODU_TRACE_01(idx_, level_, msg_, para1_)
#define PND_IODU_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_IODU_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_IODU_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_IODU_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_IODU_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_IODU_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_IODU_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_IODU_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_IODU_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_IODU_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_IODU_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_PARAMS_TRACE_00(idx_, level_, msg_)
#define PND_PARAMS_TRACE_01(idx_, level_, msg_, para1_)
#define PND_PARAMS_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_PARAMS_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_PARAMS_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_PARAMS_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_PARAMS_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_PARAMS_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_PARAMS_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_PARAMS_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_PARAMS_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_PARAMS_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_PARAMS_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_AGENT_TRACE_00(idx_, level_, msg_)
#define PND_AGENT_TRACE_01(idx_, level_, msg_, para1_)
#define PND_AGENT_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_AGENT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_AGENT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_AGENT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_AGENT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_AGENT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_AGENT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_AGENT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_AGENT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_AGENT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_AGENT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_IOBC_TRACE_00(idx_, level_, msg_)
#define PND_IOBC_TRACE_01(idx_, level_, msg_, para1_)
#define PND_IOBC_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_IOBC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_IOBC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_IOBC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_IOBC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_IOBC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_IOBC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_IOBC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_IOBC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_IOBC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_IOBC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PND_PNUC_TRACE_00(idx_, level_, msg_)
#define PND_PNUC_TRACE_01(idx_, level_, msg_, para1_)
#define PND_PNUC_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PND_PNUC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PND_PNUC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PND_PNUC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PND_PNUC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PND_PNUC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PND_PNUC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PND_PNUC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PND_PNUC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PND_PNUC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PND_PNUC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

/*----------------------------------------------------------------------------*/
#elif ( PSI_CFG_TRACE_MODE == 1 ) /* LSA trace */
/*----------------------------------------------------------------------------*/

#error "config error, PND only supports indexed tracing"

/*----------------------------------------------------------------------------*/
#elif ( PSI_CFG_TRACE_MODE == 2 ) /* LSA index trace */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_SYSTEM */

#ifndef PND_SYSTEM_TRACE_00
#define PND_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_SYSTEM_TRACE_01
#define PND_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_SYSTEM_TRACE_02
#define PND_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_SYSTEM_TRACE_03
#define PND_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_SYSTEM_TRACE_04
#define PND_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_SYSTEM_TRACE_05
#define PND_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_SYSTEM_TRACE_06
#define PND_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_SYSTEM_TRACE_07
#define PND_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_SYSTEM_TRACE_08
#define PND_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_SYSTEM_TRACE_09
#define PND_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_SYSTEM_TRACE_10
#define PND_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_SYSTEM_TRACE_11
#define PND_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_SYSTEM_TRACE_12
#define PND_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_PD */

#ifndef PND_PD_TRACE_00
#define PND_PD_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_PD_TRACE_01
#define PND_PD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_PD_TRACE_02
#define PND_PD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_PD_TRACE_03
#define PND_PD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_PD_TRACE_04
#define PND_PD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_PD_TRACE_05
#define PND_PD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_PD_TRACE_06
#define PND_PD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_PD_TRACE_07
#define PND_PD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_PD_TRACE_08
#define PND_PD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_PD_TRACE_09
#define PND_PD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_PD_TRACE_10
#define PND_PD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_PD_TRACE_11
#define PND_PD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_PD_TRACE_12
#define PND_PD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_IOC */

#ifndef PND_IOC_TRACE_00
#define PND_IOC_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_IOC_TRACE_01
#define PND_IOC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_IOC_TRACE_02
#define PND_IOC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_IOC_TRACE_03
#define PND_IOC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_IOC_TRACE_04
#define PND_IOC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_IOC_TRACE_05
#define PND_IOC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_IOC_TRACE_06
#define PND_IOC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_IOC_TRACE_07
#define PND_IOC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_IOC_TRACE_08
#define PND_IOC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_IOC_TRACE_09
#define PND_IOC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_IOC_TRACE_10
#define PND_IOC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_IOC_TRACE_11
#define PND_IOC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_IOC_TRACE_12
#define PND_IOC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_IOC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_IOH */

#ifndef PND_IOH_TRACE_00
#define PND_IOH_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_IOH_TRACE_01
#define PND_IOH_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_IOH_TRACE_02
#define PND_IOH_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_IOH_TRACE_03
#define PND_IOH_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_IOH_TRACE_04
#define PND_IOH_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_IOH_TRACE_05
#define PND_IOH_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_IOH_TRACE_06
#define PND_IOH_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_IOH_TRACE_07
#define PND_IOH_TRACE_07( idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_IOH_TRACE_08
#define PND_IOH_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_IOH_TRACE_09
#define PND_IOH_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_IOH_TRACE_10
#define PND_IOH_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_IOH_TRACE_11
#define PND_IOH_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_IOH_TRACE_12
#define PND_IOH_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_IOH_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_OHA */

#ifndef PND_OHA_TRACE_00
#define PND_OHA_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_OHA_TRACE_01
#define PND_OHA_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_OHA_TRACE_02
#define PND_OHA_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_OHA_TRACE_03
#define PND_OHA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_OHA_TRACE_04
#define PND_OHA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_OHA_TRACE_05
#define PND_OHA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_OHA_TRACE_06
#define PND_OHA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_OHA_TRACE_07
#define PND_OHA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_OHA_TRACE_08
#define PND_OHA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_OHA_TRACE_09
#define PND_OHA_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_OHA_TRACE_10
#define PND_OHA_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_OHA_TRACE_11
#define PND_OHA_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_OHA_TRACE_12
#define PND_OHA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_PNTRC */

#ifndef PND_PNTRC_TRACE_00
#define PND_PNTRC_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_PNTRC_TRACE_01
#define PND_PNTRC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_PNTRC_TRACE_02
#define PND_PNTRC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_PNTRC_TRACE_03
#define PND_PNTRC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_PNTRC_TRACE_04
#define PND_PNTRC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_PNTRC_TRACE_05
#define PND_PNTRC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_PNTRC_TRACE_06
#define PND_PNTRC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_PNTRC_TRACE_07
#define PND_PNTRC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_PNTRC_TRACE_08
#define PND_PNTRC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_PNTRC_TRACE_09
#define PND_PNTRC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_PNTRC_TRACE_10
#define PND_PNTRC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_PNTRC_TRACE_11
#define PND_PNTRC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_PNTRC_TRACE_12
#define PND_PNTRC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_PNTRC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_IODU */

#ifndef PND_IODU_TRACE_00
#define PND_IODU_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_IODU_TRACE_01
#define PND_IODU_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_IODU_TRACE_02
#define PND_IODU_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_IODU_TRACE_03
#define PND_IODU_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_IODU_TRACE_04
#define PND_IODU_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_IODU_TRACE_05
#define PND_IODU_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_IODU_TRACE_06
#define PND_IODU_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_IODU_TRACE_07
#define PND_IODU_TRACE_07( idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_IODU_TRACE_08
#define PND_IODU_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_IODU_TRACE_09
#define PND_IODU_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_IODU_TRACE_10
#define PND_IODU_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_IODU_TRACE_11
#define PND_IODU_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_IODU_TRACE_12
#define PND_IODU_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_IODU_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_PARAMS */

#ifndef PND_PARAMS_TRACE_00
#define PND_PARAMS_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_PARAMS_TRACE_01
#define PND_PARAMS_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_PARAMS_TRACE_02
#define PND_PARAMS_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_PARAMS_TRACE_03
#define PND_PARAMS_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_PARAMS_TRACE_04
#define PND_PARAMS_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_PARAMS_TRACE_05
#define PND_PARAMS_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_PARAMS_TRACE_06
#define PND_PARAMS_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_PARAMS_TRACE_07
#define PND_PARAMS_TRACE_07( idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_PARAMS_TRACE_08
#define PND_PARAMS_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_PARAMS_TRACE_09
#define PND_PARAMS_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_PARAMS_TRACE_10
#define PND_PARAMS_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_PARAMS_TRACE_11
#define PND_PARAMS_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_PARAMS_TRACE_12
#define PND_PARAMS_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_PARAMS_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_AGENT */

#ifndef PND_AGENT_TRACE_00
#define PND_AGENT_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_AGENT_TRACE_01
#define PND_AGENT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_AGENT_TRACE_02
#define PND_AGENT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_AGENT_TRACE_03
#define PND_AGENT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_AGENT_TRACE_04
#define PND_AGENT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_AGENT_TRACE_05
#define PND_AGENT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_AGENT_TRACE_06
#define PND_AGENT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_AGENT_TRACE_07
#define PND_AGENT_TRACE_07( idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_AGENT_TRACE_08
#define PND_AGENT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_AGENT_TRACE_09
#define PND_AGENT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_AGENT_TRACE_10
#define PND_AGENT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_AGENT_TRACE_11
#define PND_AGENT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_AGENT_TRACE_12
#define PND_AGENT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_AGENT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_IOBC */

#ifndef PND_IOBC_TRACE_00
#define PND_IOBC_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_IOBC_TRACE_01
#define PND_IOBC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_IOBC_TRACE_02
#define PND_IOBC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_IOBC_TRACE_03
#define PND_IOBC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_IOBC_TRACE_04
#define PND_IOBC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_IOBC_TRACE_05
#define PND_IOBC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_IOBC_TRACE_06
#define PND_IOBC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_IOBC_TRACE_07
#define PND_IOBC_TRACE_07( idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_IOBC_TRACE_08
#define PND_IOBC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_IOBC_TRACE_09
#define PND_IOBC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_IOBC_TRACE_10
#define PND_IOBC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_IOBC_TRACE_11
#define PND_IOBC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_IOBC_TRACE_12
#define PND_IOBC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_IOBC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PND_PNUC */

#ifndef PND_PNUC_TRACE_00
#define PND_PNUC_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00( TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_);
#endif

#ifndef PND_PNUC_TRACE_01
#define PND_PNUC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_);
#endif

#ifndef PND_PNUC_TRACE_02
#define PND_PNUC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_);
#endif

#ifndef PND_PNUC_TRACE_03
#define PND_PNUC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_);
#endif

#ifndef PND_PNUC_TRACE_04
#define PND_PNUC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_);
#endif

#ifndef PND_PNUC_TRACE_05
#define PND_PNUC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_);
#endif

#ifndef PND_PNUC_TRACE_06
#define PND_PNUC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_);
#endif

#ifndef PND_PNUC_TRACE_07
#define PND_PNUC_TRACE_07( idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
     LSA_IDX_TRACE_07(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_);
#endif

#ifndef PND_PNUC_TRACE_08
#define PND_PNUC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_);
#endif

#ifndef PND_PNUC_TRACE_09
#define PND_PNUC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_);
#endif

#ifndef PND_PNUC_TRACE_10
#define PND_PNUC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_);
#endif

#ifndef PND_PNUC_TRACE_11
#define PND_PNUC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_);
#endif

#ifndef PND_PNUC_TRACE_12
#define PND_PNUC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PND_PNUC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_);
#endif

#else

#error "PSI_CFG_TRACE_MODE has unknown value"

#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  end of file pnd_trc.h                                                   */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
#endif  /* of PND_TRC_H */
