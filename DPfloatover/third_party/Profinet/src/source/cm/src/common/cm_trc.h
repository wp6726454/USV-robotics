#ifndef CM_TRC_H                     /* ----- reinclude-protection ----- */
#define CM_TRC_H

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
/*  F i l e               &F: cm_trc.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface for lsa-trace                                           */
/*                                                                           */
/*  See "Trace_fuer_LSA-Entwickler_akt.doc" in LSA_LTRC for more information */
/*                                                                           */
/*****************************************************************************/


/*
 *  CM uses the sub-systems:
 *    TRACE_SUBSYS_CM_UPPER      (UPPER Common)
 *    TRACE_SUBSYS_CM_PD         (UPPER Physical Device)
 *    TRACE_SUBSYS_CM_CL         (UPPER Client)
 *    TRACE_SUBSYS_CM_MC         (UPPER Multicast)
 *    TRACE_SUBSYS_CM_SV         (UPPER Server)
 *    TRACE_SUBSYS_CM_AR         (AR related things)
 *    TRACE_SUBSYS_CM_ACP        (LOWER ACP)
 *    TRACE_SUBSYS_CM_EDD        (LOWER EDD)
 *    TRACE_SUBSYS_CM_GSY        (LOWER GSY)
 *    TRACE_SUBSYS_CM_NARE       (LOWER NARE)
 *    TRACE_SUBSYS_CM_OHA        (LOWER OHA)
 *    TRACE_SUBSYS_CM_RPC        (LOWER CLRPC)
 *    TRACE_SUBSYS_CM_MRP        (LOWER MRP)
 *    TRACE_SUBSYS_CM_POF        (LOWER POF)
 *    TRACE_SUBSYS_CM_MEM        (MEMORY)
 *
 *    note: the subsystems of LTRC are defined per enum
 */

/*
 *  LSA_TRACE_LEVEL_FATAL        Serious not tradable error occurred.
 *  LSA_TRACE_LEVEL_ERROR        Serious tradable error occurred.
 *  LSA_TRACE_LEVEL_UNEXP        Unexpected situation occurred.
 *  LSA_TRACE_LEVEL_WARN         Warning: Program exits productive branch.
 *
 *  LSA_TRACE_LEVEL_NOTE_HIGH    Important information productive case: High level.
 *  LSA_TRACE_LEVEL_NOTE         Important information productive case: Normal level.
 *  LSA_TRACE_LEVEL_NOTE_LOW     Important information productive case: Low level.
 *  LSA_TRACE_LEVEL_CHAT         Important code places within the software.
 */

/*=============================================================================
 * macro names:   CM_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_CM_YYYYY (if LTRC is used in the file
 *                ltrc_sub.h/txt or if LTRC isn't used in a system file)!
 *
 * parameters:    level_:   LSA_TRACE_LEVEL_OFF, LSA_TRACE_LEVEL_FATAL,
 *                          LSA_TRACE_LEVEL_ERROR, LSA_TRACE_LEVEL_UNEXP,
 *                          LSA_TRACE_LEVEL_WARN, LSA_TRACE_LEVEL_NOTE_HIGH,
 *                          LSA_TRACE_LEVEL_NOTE, LSA_TRACE_LEVEL_NOTE_LOW or
 *                          LSA_TRACE_LEVEL_CHAT
 *                msg_:     string similar to "printf"
 *                para1_:   parameter 1
 *                para2_:   parameter 2
 *                 ...        ...
 *                para16_:  paramater 16
 *
 * return value:  LSA_VOID
 *===========================================================================*/


/*----------------------------------------------------------------------------*/
#if CM_CFG_TRACE_MODE == 0 /* no trace */


#define CM_UPPER_TRACE_00(idx_, level_, msg_)
#define CM_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define CM_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)

#define CM_PD_TRACE_00(idx_, level_, msg_)
#define CM_PD_TRACE_01(idx_, level_, msg_, para1_)
#define CM_PD_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_PD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_PD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_PD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_PD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_PD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_PD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_PD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CM_PD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)

#define CM_CL_TRACE_00(idx_, level_, msg_)
#define CM_CL_TRACE_01(idx_, level_, msg_, para1_)
#define CM_CL_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_CL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_CL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_CL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_CL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_CL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_CL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_CL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CM_CL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CM_CL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CM_CL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CM_MC_TRACE_00(idx_, level_, msg_)
#define CM_MC_TRACE_01(idx_, level_, msg_, para1_)
#define CM_MC_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_MC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_MC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_MC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_MC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_MC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_MC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)

#define CM_SV_TRACE_00(idx_, level_, msg_)
#define CM_SV_TRACE_01(idx_, level_, msg_, para1_)
#define CM_SV_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_SV_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_SV_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_SV_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_SV_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_SV_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_SV_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_SV_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CM_SV_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CM_SV_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CM_SV_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CM_AR_TRACE_00(idx_, level_, msg_)
#define CM_AR_TRACE_01(idx_, level_, msg_, para1_)
#define CM_AR_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_AR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_AR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_AR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_AR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_AR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_AR_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_AR_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CM_AR_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CM_AR_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CM_AR_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CM_ACP_TRACE_00(idx_, level_, msg_)
#define CM_ACP_TRACE_01(idx_, level_, msg_, para1_)
#define CM_ACP_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_ACP_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_ACP_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_ACP_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_ACP_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_ACP_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_ACP_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_ACP_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CM_ACP_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)

#define CM_EDD_TRACE_00(idx_, level_, msg_)
#define CM_EDD_TRACE_01(idx_, level_, msg_, para1_)
#define CM_EDD_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_EDD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_EDD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_EDD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_EDD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_EDD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_EDD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_EDD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)

#define CM_GSY_TRACE_00(idx_, level_, msg_)
#define CM_GSY_TRACE_01(idx_, level_, msg_, para1_)
#define CM_GSY_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_GSY_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_GSY_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_GSY_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_GSY_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_GSY_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_GSY_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_GSY_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CM_NARE_TRACE_00(idx_, level_, msg_)
#define CM_NARE_TRACE_01(idx_, level_, msg_, para1_)
#define CM_NARE_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_NARE_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_NARE_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_NARE_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_NARE_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_NARE_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_NARE_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)

#define CM_OHA_TRACE_00(idx_, level_, msg_)
#define CM_OHA_TRACE_01(idx_, level_, msg_, para1_)
#define CM_OHA_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_OHA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_OHA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_OHA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_OHA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_OHA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_OHA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CM_OHA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CM_RPC_TRACE_00(idx_, level_, msg_)
#define CM_RPC_TRACE_01(idx_, level_, msg_, para1_)
#define CM_RPC_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_RPC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_RPC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_RPC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_RPC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_RPC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_RPC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)

#define CM_MRP_TRACE_00(idx_, level_, msg_)
#define CM_MRP_TRACE_01(idx_, level_, msg_, para1_)
#define CM_MRP_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_MRP_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_MRP_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_MRP_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_MRP_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_MRP_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_MRP_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)

#define CM_POF_TRACE_00(idx_, level_, msg_)
#define CM_POF_TRACE_01(idx_, level_, msg_, para1_)
#define CM_POF_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_POF_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_POF_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_POF_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_POF_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_POF_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_POF_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)

#define CM_MEM_TRACE_00(idx_, level_, msg_)
#define CM_MEM_TRACE_01(idx_, level_, msg_, para1_)
#define CM_MEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CM_MEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CM_MEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CM_MEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CM_MEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CM_MEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CM_MEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)


/*----------------------------------------------------------------------------*/
#elif CM_CFG_TRACE_MODE == 1 /* LSA trace */


#ifndef CM_UPPER_TRACE_00
#define CM_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_UPPER, level_, msg_)
#endif

#ifndef CM_UPPER_TRACE_01
#define CM_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_)
#endif

#ifndef CM_UPPER_TRACE_02
#define CM_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_)
#endif

#ifndef CM_UPPER_TRACE_03
#define CM_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_UPPER_TRACE_04
#define CM_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_UPPER_TRACE_05
#define CM_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_UPPER_TRACE_06
#define CM_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_UPPER_TRACE_07
#define CM_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_UPPER_TRACE_08
#define CM_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_UPPER_TRACE_09
#define CM_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CM_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_PD_TRACE_00
#define CM_PD_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_PD, level_, msg_)
#endif

#ifndef CM_PD_TRACE_01
#define CM_PD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_PD, level_, msg_, para1_)
#endif

#ifndef CM_PD_TRACE_02
#define CM_PD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_)
#endif

#ifndef CM_PD_TRACE_03
#define CM_PD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_PD_TRACE_04
#define CM_PD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_PD_TRACE_05
#define CM_PD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_PD_TRACE_06
#define CM_PD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_PD_TRACE_07
#define CM_PD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_PD_TRACE_08
#define CM_PD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_PD_TRACE_09
#define CM_PD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_PD_TRACE_10
#define CM_PD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CM_PD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_CL_TRACE_00
#define CM_CL_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_CL, level_, msg_)
#endif

#ifndef CM_CL_TRACE_01
#define CM_CL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_CL, level_, msg_, para1_)
#endif

#ifndef CM_CL_TRACE_02
#define CM_CL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_)
#endif

#ifndef CM_CL_TRACE_03
#define CM_CL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_CL_TRACE_04
#define CM_CL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_CL_TRACE_05
#define CM_CL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_CL_TRACE_06
#define CM_CL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_CL_TRACE_07
#define CM_CL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_CL_TRACE_08
#define CM_CL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_CL_TRACE_09
#define CM_CL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_CL_TRACE_10
#define CM_CL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CM_CL_TRACE_11
#define CM_CL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CM_CL_TRACE_12
#define CM_CL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CM_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_MC_TRACE_00
#define CM_MC_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_MC, level_, msg_)
#endif

#ifndef CM_MC_TRACE_01
#define CM_MC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_MC, level_, msg_, para1_)
#endif

#ifndef CM_MC_TRACE_02
#define CM_MC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_MC, level_, msg_, para1_, para2_)
#endif

#ifndef CM_MC_TRACE_03
#define CM_MC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_MC, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_MC_TRACE_04
#define CM_MC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_MC, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_MC_TRACE_05
#define CM_MC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_MC, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_MC_TRACE_06
#define CM_MC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_MC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_MC_TRACE_07
#define CM_MC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_MC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_MC_TRACE_08
#define CM_MC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_MC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_SV_TRACE_00
#define CM_SV_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_SV, level_, msg_)
#endif

#ifndef CM_SV_TRACE_01
#define CM_SV_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_SV, level_, msg_, para1_)
#endif

#ifndef CM_SV_TRACE_02
#define CM_SV_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_)
#endif

#ifndef CM_SV_TRACE_03
#define CM_SV_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_SV_TRACE_04
#define CM_SV_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_SV_TRACE_05
#define CM_SV_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_SV_TRACE_06
#define CM_SV_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_SV_TRACE_07
#define CM_SV_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_SV_TRACE_08
#define CM_SV_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_SV_TRACE_09
#define CM_SV_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_SV_TRACE_10
#define CM_SV_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CM_SV_TRACE_11
#define CM_SV_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CM_SV_TRACE_12
#define CM_SV_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CM_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_AR_TRACE_00
#define CM_AR_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_AR, level_, msg_)
#endif

#ifndef CM_AR_TRACE_01
#define CM_AR_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_AR, level_, msg_, para1_)
#endif

#ifndef CM_AR_TRACE_02
#define CM_AR_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_)
#endif

#ifndef CM_AR_TRACE_03
#define CM_AR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_AR_TRACE_04
#define CM_AR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_AR_TRACE_05
#define CM_AR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_AR_TRACE_06
#define CM_AR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_AR_TRACE_07
#define CM_AR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_AR_TRACE_08
#define CM_AR_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_AR_TRACE_09
#define CM_AR_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_AR_TRACE_10
#define CM_AR_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CM_AR_TRACE_11
#define CM_AR_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CM_AR_TRACE_12
#define CM_AR_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CM_AR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_ACP_TRACE_00
#define CM_ACP_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_ACP, level_, msg_)
#endif

#ifndef CM_ACP_TRACE_01
#define CM_ACP_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_)
#endif

#ifndef CM_ACP_TRACE_02
#define CM_ACP_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_)
#endif

#ifndef CM_ACP_TRACE_03
#define CM_ACP_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_ACP_TRACE_04
#define CM_ACP_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_ACP_TRACE_05
#define CM_ACP_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_ACP_TRACE_06
#define CM_ACP_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_ACP_TRACE_07
#define CM_ACP_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_ACP_TRACE_08
#define CM_ACP_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_ACP_TRACE_09
#define CM_ACP_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_ACP_TRACE_10
#define CM_ACP_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CM_ACP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_EDD_TRACE_00
#define CM_EDD_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_EDD, level_, msg_)
#endif

#ifndef CM_EDD_TRACE_01
#define CM_EDD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_)
#endif

#ifndef CM_EDD_TRACE_02
#define CM_EDD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_)
#endif

#ifndef CM_EDD_TRACE_03
#define CM_EDD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_EDD_TRACE_04
#define CM_EDD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_EDD_TRACE_05
#define CM_EDD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_EDD_TRACE_06
#define CM_EDD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_EDD_TRACE_07
#define CM_EDD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_EDD_TRACE_08
#define CM_EDD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_EDD_TRACE_09
#define CM_EDD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CM_EDD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_GSY_TRACE_00
#define CM_GSY_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_GSY, level_, msg_)
#endif

#ifndef CM_GSY_TRACE_01
#define CM_GSY_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_)
#endif

#ifndef CM_GSY_TRACE_02
#define CM_GSY_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_, para2_)
#endif

#ifndef CM_GSY_TRACE_03
#define CM_GSY_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_GSY_TRACE_04
#define CM_GSY_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_GSY_TRACE_05
#define CM_GSY_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_GSY_TRACE_06
#define CM_GSY_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_GSY_TRACE_07
#define CM_GSY_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_GSY_TRACE_12
#define CM_GSY_TRACE_12(idx, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CM_GSY, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_NARE_TRACE_00
#define CM_NARE_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_NARE, level_, msg_)
#endif

#ifndef CM_NARE_TRACE_01
#define CM_NARE_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_)
#endif

#ifndef CM_NARE_TRACE_02
#define CM_NARE_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_, para2_)
#endif

#ifndef CM_NARE_TRACE_03
#define CM_NARE_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_NARE_TRACE_04
#define CM_NARE_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_NARE_TRACE_05
#define CM_NARE_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_NARE_TRACE_06
#define CM_NARE_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_NARE_TRACE_07
#define CM_NARE_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_NARE_TRACE_08
#define CM_NARE_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_NARE, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_TRACE_00
#define CM_OHA_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_OHA, level_, msg_)
#endif

#ifndef CM_OHA_TRACE_01
#define CM_OHA_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_)
#endif

#ifndef CM_OHA_TRACE_02
#define CM_OHA_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_)
#endif

#ifndef CM_OHA_TRACE_03
#define CM_OHA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_OHA_TRACE_04
#define CM_OHA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_OHA_TRACE_05
#define CM_OHA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_OHA_TRACE_06
#define CM_OHA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_OHA_TRACE_07
#define CM_OHA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_OHA_TRACE_08
#define CM_OHA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_OHA_TRACE_12
#define CM_OHA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CM_OHA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_TRACE_00
#define CM_RPC_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_RPC, level_, msg_)
#endif

#ifndef CM_RPC_TRACE_01
#define CM_RPC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_)
#endif

#ifndef CM_RPC_TRACE_02
#define CM_RPC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_, para2_)
#endif

#ifndef CM_RPC_TRACE_03
#define CM_RPC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_RPC_TRACE_04
#define CM_RPC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_RPC_TRACE_05
#define CM_RPC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_RPC_TRACE_06
#define CM_RPC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_RPC_TRACE_07
#define CM_RPC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_RPC_TRACE_08
#define CM_RPC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_RPC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_MRP_TRACE_00
#define CM_MRP_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_MRP, level_, msg_)
#endif

#ifndef CM_MRP_TRACE_01
#define CM_MRP_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_)
#endif

#ifndef CM_MRP_TRACE_02
#define CM_MRP_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_, para2_)
#endif

#ifndef CM_MRP_TRACE_03
#define CM_MRP_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_MRP_TRACE_04
#define CM_MRP_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_MRP_TRACE_05
#define CM_MRP_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_MRP_TRACE_06
#define CM_MRP_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_MRP_TRACE_07
#define CM_MRP_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_MRP_TRACE_08
#define CM_MRP_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_MRP, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_POF_TRACE_00
#define CM_POF_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_POF, level_, msg_)
#endif

#ifndef CM_POF_TRACE_01
#define CM_POF_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_POF, level_, msg_, para1_)
#endif

#ifndef CM_POF_TRACE_02
#define CM_POF_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_POF, level_, msg_, para1_, para2_)
#endif

#ifndef CM_POF_TRACE_03
#define CM_POF_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_POF, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_POF_TRACE_04
#define CM_POF_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_POF, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_POF_TRACE_05
#define CM_POF_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_POF, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_POF_TRACE_06
#define CM_POF_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_POF, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_POF_TRACE_07
#define CM_POF_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_POF, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_POF_TRACE_08
#define CM_POF_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_POF, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_MEM_TRACE_00
#define CM_MEM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CM_MEM, level_, msg_)
#endif

#ifndef CM_MEM_TRACE_01
#define CM_MEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_)
#endif

#ifndef CM_MEM_TRACE_02
#define CM_MEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_, para2_)
#endif

#ifndef CM_MEM_TRACE_03
#define CM_MEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_MEM_TRACE_04
#define CM_MEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_MEM_TRACE_05
#define CM_MEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_MEM_TRACE_06
#define CM_MEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_MEM_TRACE_07
#define CM_MEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_MEM_TRACE_08
#define CM_MEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CM_MEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/
#elif CM_CFG_TRACE_MODE == 2 /* LSA index trace */

#ifndef CM_UPPER_TRACE_00
#define CM_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_UPPER_TRACE_01
#define CM_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_UPPER_TRACE_02
#define CM_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_UPPER_TRACE_03
#define CM_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_UPPER_TRACE_04
#define CM_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_UPPER_TRACE_05
#define CM_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_UPPER_TRACE_06
#define CM_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_UPPER_TRACE_07
#define CM_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_UPPER_TRACE_08
#define CM_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_UPPER_TRACE_09
#define CM_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CM_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_PD_TRACE_00
#define CM_PD_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_PD_TRACE_01
#define CM_PD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_PD_TRACE_02
#define CM_PD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_PD_TRACE_03
#define CM_PD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_PD_TRACE_04
#define CM_PD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_PD_TRACE_05
#define CM_PD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_PD_TRACE_06
#define CM_PD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_PD_TRACE_07
#define CM_PD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_PD_TRACE_08
#define CM_PD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_PD_TRACE_09
#define CM_PD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_PD_TRACE_10
#define CM_PD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CM_PD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_CL_TRACE_00
#define CM_CL_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_CL_TRACE_01
#define CM_CL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_CL_TRACE_02
#define CM_CL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_CL_TRACE_03
#define CM_CL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_CL_TRACE_04
#define CM_CL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_CL_TRACE_05
#define CM_CL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_CL_TRACE_06
#define CM_CL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_CL_TRACE_07
#define CM_CL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_CL_TRACE_08
#define CM_CL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_CL_TRACE_09
#define CM_CL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_CL_TRACE_10
#define CM_CL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CM_CL_TRACE_11
#define CM_CL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CM_CL_TRACE_12
#define CM_CL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CM_CL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_MC_TRACE_00
#define CM_MC_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_MC_TRACE_01
#define CM_MC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_MC_TRACE_02
#define CM_MC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_MC_TRACE_03
#define CM_MC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_MC_TRACE_04
#define CM_MC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_MC_TRACE_05
#define CM_MC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_MC_TRACE_06
#define CM_MC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_MC_TRACE_07
#define CM_MC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_MC_TRACE_08
#define CM_MC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_MC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_SV_TRACE_00
#define CM_SV_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_SV_TRACE_01
#define CM_SV_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_SV_TRACE_02
#define CM_SV_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_SV_TRACE_03
#define CM_SV_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_SV_TRACE_04
#define CM_SV_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_SV_TRACE_05
#define CM_SV_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_SV_TRACE_06
#define CM_SV_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_SV_TRACE_07
#define CM_SV_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_SV_TRACE_08
#define CM_SV_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_SV_TRACE_09
#define CM_SV_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_SV_TRACE_10
#define CM_SV_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CM_SV_TRACE_11
#define CM_SV_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CM_SV_TRACE_12
#define CM_SV_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CM_SV_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_AR_TRACE_00
#define CM_AR_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_AR_TRACE_01
#define CM_AR_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_AR_TRACE_02
#define CM_AR_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_AR_TRACE_03
#define CM_AR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_AR_TRACE_04
#define CM_AR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_AR_TRACE_05
#define CM_AR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_AR_TRACE_06
#define CM_AR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_AR_TRACE_07
#define CM_AR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_AR_TRACE_08
#define CM_AR_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_AR_TRACE_09
#define CM_AR_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_AR_TRACE_10
#define CM_AR_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CM_AR_TRACE_11
#define CM_AR_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CM_AR_TRACE_12
#define CM_AR_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CM_AR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_ACP_TRACE_00
#define CM_ACP_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_ACP_TRACE_01
#define CM_ACP_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_ACP_TRACE_02
#define CM_ACP_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_ACP_TRACE_03
#define CM_ACP_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_ACP_TRACE_04
#define CM_ACP_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_ACP_TRACE_05
#define CM_ACP_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_ACP_TRACE_06
#define CM_ACP_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_ACP_TRACE_07
#define CM_ACP_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_ACP_TRACE_08
#define CM_ACP_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_ACP_TRACE_09
#define CM_ACP_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CM_ACP_TRACE_10
#define CM_ACP_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CM_ACP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_EDD_TRACE_00
#define CM_EDD_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_EDD_TRACE_01
#define CM_EDD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_EDD_TRACE_02
#define CM_EDD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_EDD_TRACE_03
#define CM_EDD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_EDD_TRACE_04
#define CM_EDD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_EDD_TRACE_05
#define CM_EDD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_EDD_TRACE_06
#define CM_EDD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_EDD_TRACE_07
#define CM_EDD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_EDD_TRACE_08
#define CM_EDD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_EDD_TRACE_09
#define CM_EDD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CM_EDD_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_GSY_TRACE_00
#define CM_GSY_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_GSY_TRACE_01
#define CM_GSY_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_GSY_TRACE_02
#define CM_GSY_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_GSY_TRACE_03
#define CM_GSY_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_GSY_TRACE_04
#define CM_GSY_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_GSY_TRACE_05
#define CM_GSY_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_GSY_TRACE_06
#define CM_GSY_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_GSY_TRACE_07
#define CM_GSY_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_GSY_TRACE_12
#define CM_GSY_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CM_GSY_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_NARE_TRACE_00
#define CM_NARE_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_NARE_TRACE_01
#define CM_NARE_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_NARE_TRACE_02
#define CM_NARE_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_NARE_TRACE_03
#define CM_NARE_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_NARE_TRACE_04
#define CM_NARE_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_NARE_TRACE_05
#define CM_NARE_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_NARE_TRACE_06
#define CM_NARE_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_NARE_TRACE_07
#define CM_NARE_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_NARE_TRACE_08
#define CM_NARE_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_NARE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_TRACE_00
#define CM_OHA_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_OHA_TRACE_01
#define CM_OHA_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_OHA_TRACE_02
#define CM_OHA_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_OHA_TRACE_03
#define CM_OHA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_OHA_TRACE_04
#define CM_OHA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_OHA_TRACE_05
#define CM_OHA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_OHA_TRACE_06
#define CM_OHA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_OHA_TRACE_07
#define CM_OHA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_OHA_TRACE_08
#define CM_OHA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CM_OHA_TRACE_12
#define CM_OHA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CM_OHA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_TRACE_00
#define CM_RPC_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_RPC_TRACE_01
#define CM_RPC_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_RPC_TRACE_02
#define CM_RPC_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_RPC_TRACE_03
#define CM_RPC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_RPC_TRACE_04
#define CM_RPC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_RPC_TRACE_05
#define CM_RPC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_RPC_TRACE_06
#define CM_RPC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_RPC_TRACE_07
#define CM_RPC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_RPC_TRACE_08
#define CM_RPC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_RPC_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_MRP_TRACE_00
#define CM_MRP_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_MRP_TRACE_01
#define CM_MRP_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_MRP_TRACE_02
#define CM_MRP_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_MRP_TRACE_03
#define CM_MRP_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_MRP_TRACE_04
#define CM_MRP_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_MRP_TRACE_05
#define CM_MRP_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_MRP_TRACE_06
#define CM_MRP_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_MRP_TRACE_07
#define CM_MRP_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_MRP_TRACE_08
#define CM_MRP_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_MRP_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_POF_TRACE_00
#define CM_POF_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_POF_TRACE_01
#define CM_POF_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_POF_TRACE_02
#define CM_POF_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_POF_TRACE_03
#define CM_POF_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_POF_TRACE_04
#define CM_POF_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_POF_TRACE_05
#define CM_POF_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_POF_TRACE_06
#define CM_POF_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_POF_TRACE_07
#define CM_POF_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_POF_TRACE_08
#define CM_POF_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_POF_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CM_MEM_TRACE_00
#define CM_MEM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_)
#endif

#ifndef CM_MEM_TRACE_01
#define CM_MEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef CM_MEM_TRACE_02
#define CM_MEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef CM_MEM_TRACE_03
#define CM_MEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CM_MEM_TRACE_04
#define CM_MEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CM_MEM_TRACE_05
#define CM_MEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CM_MEM_TRACE_06
#define CM_MEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CM_MEM_TRACE_07
#define CM_MEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CM_MEM_TRACE_08
#define CM_MEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CM_MEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif


/*----------------------------------------------------------------------------*/
#else

#error "CM_CFG_TRACE_MODE has unknown value"

#endif


/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_TRC_H */
