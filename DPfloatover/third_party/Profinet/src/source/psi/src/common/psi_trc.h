#ifndef PSI_TRC_H
#define PSI_TRC_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_trc.h                                 :F&  */
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
/*****************************************************************************/

/*===========================================================================*/
/* Usage of traces within PSI:                                               */
/*                                                                           */
/* Trace-Subsystems used                                                     */
/*                                                                           */
/* TRACE_SUBSYS_PSI_SYSTEM   : Used for PSI common traces                    */
/* TRACE_SUBSYS_PSI_LD       : Used for PSI LD (LogicalDevice) traces        */
/* TRACE_SUBSYS_PSI_HD       : Used for PSI HD (HardwareDevice) traces       */
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

#if ( PSI_CFG_TRACE_MODE == 0 ) /* no trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_SYSTEM */

#define PSI_SYSTEM_TRACE_00(idx_, level_, msg_)
#define PSI_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define PSI_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PSI_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PSI_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PSI_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PSI_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PSI_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PSI_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PSI_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PSI_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PSI_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PSI_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_LD */

#define PSI_LD_TRACE_00(idx_, level_, msg_)
#define PSI_LD_TRACE_01(idx_, level_, msg_, para1_)
#define PSI_LD_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PSI_LD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PSI_LD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PSI_LD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PSI_LD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PSI_LD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PSI_LD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PSI_LD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PSI_LD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PSI_LD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PSI_LD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_HD */

#define PSI_HD_TRACE_00(idx_, level_, msg_)
#define PSI_HD_TRACE_01(idx_, level_, msg_, para1_)
#define PSI_HD_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PSI_HD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PSI_HD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PSI_HD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PSI_HD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PSI_HD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PSI_HD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PSI_HD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PSI_HD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PSI_HD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PSI_HD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

/*----------------------------------------------------------------------------*/
#elif ( PSI_CFG_TRACE_MODE == 1 ) /* LSA trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_SYSTEM */

#ifndef PSI_SYSTEM_TRACE_00
#define PSI_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_)
#endif

#ifndef PSI_SYSTEM_TRACE_01
#define PSI_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_)
#endif

#ifndef PSI_SYSTEM_TRACE_02
#define PSI_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_)
#endif

#ifndef PSI_SYSTEM_TRACE_03
#define PSI_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PSI_SYSTEM_TRACE_04
#define PSI_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PSI_SYSTEM_TRACE_05
#define PSI_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PSI_SYSTEM_TRACE_06
#define PSI_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PSI_SYSTEM_TRACE_07
#define PSI_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PSI_SYSTEM_TRACE_08
#define PSI_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PSI_SYSTEM_TRACE_09
#define PSI_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PSI_SYSTEM_TRACE_10
#define PSI_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PSI_SYSTEM_TRACE_11
#define PSI_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PSI_SYSTEM_TRACE_12
#define PSI_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_PSI_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_LD */

#ifndef PSI_LD_TRACE_00
#define PSI_LD_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_PSI_LD, level_, msg_)
#endif

#ifndef PSI_LD_TRACE_01
#define PSI_LD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_)
#endif

#ifndef PSI_LD_TRACE_02
#define PSI_LD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_)
#endif

#ifndef PSI_LD_TRACE_03
#define PSI_LD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PSI_LD_TRACE_04
#define PSI_LD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PSI_LD_TRACE_05
#define PSI_LD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PSI_LD_TRACE_06
#define PSI_LD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PSI_LD_TRACE_07
#define PSI_LD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PSI_LD_TRACE_08
#define PSI_LD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PSI_LD_TRACE_09
#define PSI_LD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PSI_LD_TRACE_10
#define PSI_LD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PSI_LD_TRACE_11
#define PSI_LD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PSI_LD_TRACE_12
#define PSI_LD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_PSI_LD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_HD */

#ifndef PSI_HD_TRACE_00
#define PSI_HD_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_PSI_HD, level_, msg_)
#endif

#ifndef PSI_HD_TRACE_01
#define PSI_HD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_)
#endif

#ifndef PSI_HD_TRACE_02
#define PSI_HD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_)
#endif

#ifndef PSI_HD_TRACE_03
#define PSI_HD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PSI_HD_TRACE_04
#define PSI_HD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PSI_HD_TRACE_05
#define PSI_HD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PSI_HD_TRACE_06
#define PSI_HD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PSI_HD_TRACE_07
#define PSI_HD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PSI_HD_TRACE_08
#define PSI_HD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PSI_HD_TRACE_09
#define PSI_HD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PSI_HD_TRACE_10
#define PSI_HD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PSI_HD_TRACE_11
#define PSI_HD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PSI_HD_TRACE_12
#define PSI_HD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_PSI_HD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
#elif ( PSI_CFG_TRACE_MODE == 2 ) /* LSA index trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_SYSTEM */

#ifndef PSI_SYSTEM_TRACE_00
#define PSI_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_)
#endif

#ifndef PSI_SYSTEM_TRACE_01
#define PSI_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef PSI_SYSTEM_TRACE_02
#define PSI_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef PSI_SYSTEM_TRACE_03
#define PSI_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PSI_SYSTEM_TRACE_04
#define PSI_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PSI_SYSTEM_TRACE_05
#define PSI_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PSI_SYSTEM_TRACE_06
#define PSI_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PSI_SYSTEM_TRACE_07
#define PSI_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PSI_SYSTEM_TRACE_08
#define PSI_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PSI_SYSTEM_TRACE_09
#define PSI_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PSI_SYSTEM_IDX00, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PSI_SYSTEM_TRACE_10
#define PSI_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PSI_SYSTEM_TRACE_11
#define PSI_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PSI_SYSTEM_TRACE_12
#define PSI_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PSI_SYSTEM_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_LD */

#ifndef PSI_LD_TRACE_00
#define PSI_LD_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_)
#endif

#ifndef PSI_LD_TRACE_01
#define PSI_LD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_)
#endif

#ifndef PSI_LD_TRACE_02
#define PSI_LD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_)
#endif

#ifndef PSI_LD_TRACE_03
#define PSI_LD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PSI_LD_TRACE_04
#define PSI_LD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PSI_LD_TRACE_05
#define PSI_LD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PSI_LD_TRACE_06
#define PSI_LD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PSI_LD_TRACE_07
#define PSI_LD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PSI_LD_TRACE_08
#define PSI_LD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PSI_LD_TRACE_09
#define PSI_LD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PSI_LD_TRACE_10
#define PSI_LD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PSI_LD_TRACE_11
#define PSI_LD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PSI_LD_TRACE_12
#define PSI_LD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PSI_LD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PSI_HD */

#ifndef PSI_HD_TRACE_00
#define PSI_HD_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_)
#endif

#ifndef PSI_HD_TRACE_01
#define PSI_HD_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_)
#endif

#ifndef PSI_HD_TRACE_02
#define PSI_HD_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_)
#endif

#ifndef PSI_HD_TRACE_03
#define PSI_HD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PSI_HD_TRACE_04
#define PSI_HD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PSI_HD_TRACE_05
#define PSI_HD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PSI_HD_TRACE_06
#define PSI_HD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PSI_HD_TRACE_07
#define PSI_HD_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PSI_HD_TRACE_08
#define PSI_HD_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PSI_HD_TRACE_09
#define PSI_HD_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PSI_HD_TRACE_10
#define PSI_HD_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PSI_HD_TRACE_11
#define PSI_HD_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PSI_HD_TRACE_12
#define PSI_HD_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PSI_HD_IDX00, idx_,level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


#else

#error "PSI_CFG_TRACE_MODE has unknown value"

#endif // PSI_CFG_TRACE_MODE

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PSI_TRC_H */
