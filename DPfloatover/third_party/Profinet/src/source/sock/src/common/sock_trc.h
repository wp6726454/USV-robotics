#ifndef SOCK_TRC_H
#define SOCK_TRC_H
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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_trc.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Macros for LSA-Traces                                                    */
/*      Adaptation to the LSA-Module LTRC is taken place.                    */
/*      Should this module not be used, either the used LSA_TRACE macros     */
/*      must be defined from the system, or directly the SOCK-Trace-Macro    */
/*      located in the sock_cfg data, must be redefined to desired           */
/*      function/macro.                                                      */
/*      If the system has no posibility to store Trace data, then in         */
/*      sock_cfg.h has to be an "empty definition"                           */
/*      (or the LSA_TRACE macro has to be defined as "empty").               */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

#if SOCK_CFG_TRACE_MODE == 0 /* no trace */

#define SOCK_UPPER_TRACE_00(idx_, level_, msg_)
#define SOCK_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define SOCK_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define SOCK_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define SOCK_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define SOCK_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define SOCK_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define SOCK_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define SOCK_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define SOCK_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define SOCK_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define SOCK_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define SOCK_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define SOCK_LOWER_TRACE_00(idx_, level_, msg_)
#define SOCK_LOWER_TRACE_01(idx_, level_, msg_, para1_)
#define SOCK_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define SOCK_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define SOCK_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define SOCK_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define SOCK_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define SOCK_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define SOCK_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define SOCK_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define SOCK_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define SOCK_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define SOCK_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define SOCK_SYSTEM_TRACE_00(idx_, level_, msg_)
#define SOCK_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define SOCK_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define SOCK_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define SOCK_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define SOCK_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define SOCK_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define SOCK_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define SOCK_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define SOCK_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define SOCK_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define SOCK_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define SOCK_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define SOCK_PROTOCOL_TRACE_00(idx_, level_, msg_)
#define SOCK_PROTOCOL_TRACE_01(idx_, level_, msg_, para1_)
#define SOCK_PROTOCOL_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define SOCK_PROTOCOL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define SOCK_PROTOCOL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define SOCK_PROTOCOL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define SOCK_PROTOCOL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define SOCK_PROTOCOL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define SOCK_PROTOCOL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define SOCK_PROTOCOL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define SOCK_PROTOCOL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define SOCK_PROTOCOL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define SOCK_PROTOCOL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)


/*****************************************************************************/
#elif SOCK_CFG_TRACE_MODE == 1 /* LSA trace */

#ifndef SOCK_UPPER_TRACE_00
#define SOCK_UPPER_TRACE_00(idx, level, msg)\
	LSA_TRACE_00(TRACE_SUBSYS_SOCK_UPPER, level, msg)
#endif

#ifndef SOCK_UPPER_TRACE_01
#define SOCK_UPPER_TRACE_01(idx, level, msg, para1)\
	LSA_TRACE_01(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1)
#endif

#ifndef SOCK_UPPER_TRACE_02
#define SOCK_UPPER_TRACE_02(idx, level, msg, para1, para2)\
	LSA_TRACE_02(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2)
#endif

#ifndef SOCK_UPPER_TRACE_03
#define SOCK_UPPER_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_TRACE_03(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_UPPER_TRACE_04
#define SOCK_UPPER_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_TRACE_04(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_UPPER_TRACE_05
#define SOCK_UPPER_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_TRACE_05(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_UPPER_TRACE_06
#define SOCK_UPPER_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_TRACE_06(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_UPPER_TRACE_07
#define SOCK_UPPER_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_TRACE_07(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_UPPER_TRACE_08
#define SOCK_UPPER_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_TRACE_08(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_UPPER_TRACE_09
#define SOCK_UPPER_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_TRACE_09(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_UPPER_TRACE_10
#define SOCK_UPPER_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_TRACE_10(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_UPPER_TRACE_11
#define SOCK_UPPER_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_TRACE_11(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_UPPER_TRACE_12
#define SOCK_UPPER_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_TRACE_12(TRACE_SUBSYS_SOCK_UPPER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif


#ifndef SOCK_LOWER_TRACE_00
#define SOCK_LOWER_TRACE_00(idx, level, msg)\
	LSA_TRACE_00(TRACE_SUBSYS_SOCK_LOWER, level, msg)
#endif

#ifndef SOCK_LOWER_TRACE_01
#define SOCK_LOWER_TRACE_01(idx, level, msg, para1)\
	LSA_TRACE_01(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1)
#endif

#ifndef SOCK_LOWER_TRACE_02
#define SOCK_LOWER_TRACE_02(idx, level, msg, para1, para2)\
	LSA_TRACE_02(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2)
#endif

#ifndef SOCK_LOWER_TRACE_03
#define SOCK_LOWER_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_TRACE_03(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_LOWER_TRACE_04
#define SOCK_LOWER_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_TRACE_04(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_LOWER_TRACE_05
#define SOCK_LOWER_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_TRACE_05(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_LOWER_TRACE_06
#define SOCK_LOWER_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_TRACE_06(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_LOWER_TRACE_07
#define SOCK_LOWER_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_TRACE_07(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_LOWER_TRACE_08
#define SOCK_LOWER_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_TRACE_08(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_LOWER_TRACE_09
#define SOCK_LOWER_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_TRACE_09(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_LOWER_TRACE_10
#define SOCK_LOWER_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_TRACE_10(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_LOWER_TRACE_11
#define SOCK_LOWER_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_TRACE_11(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_LOWER_TRACE_12
#define SOCK_LOWER_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_TRACE_12(TRACE_SUBSYS_SOCK_LOWER, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif


#ifndef SOCK_SYSTEM_TRACE_00
#define SOCK_SYSTEM_TRACE_00(idx, level, msg)\
	LSA_TRACE_00(TRACE_SUBSYS_SOCK_SYSTEM, level, msg)
#endif

#ifndef SOCK_SYSTEM_TRACE_01
#define SOCK_SYSTEM_TRACE_01(idx, level, msg, para1)\
	LSA_TRACE_01(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1)
#endif

#ifndef SOCK_SYSTEM_TRACE_02
#define SOCK_SYSTEM_TRACE_02(idx, level, msg, para1, para2)\
	LSA_TRACE_02(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2)
#endif

#ifndef SOCK_SYSTEM_TRACE_03
#define SOCK_SYSTEM_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_TRACE_03(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_SYSTEM_TRACE_04
#define SOCK_SYSTEM_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_TRACE_04(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_SYSTEM_TRACE_05
#define SOCK_SYSTEM_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_TRACE_05(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_SYSTEM_TRACE_06
#define SOCK_SYSTEM_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_TRACE_06(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_SYSTEM_TRACE_07
#define SOCK_SYSTEM_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_TRACE_07(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_SYSTEM_TRACE_08
#define SOCK_SYSTEM_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_TRACE_08(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_SYSTEM_TRACE_09
#define SOCK_SYSTEM_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_TRACE_09(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_SYSTEM_TRACE_10
#define SOCK_SYSTEM_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_TRACE_10(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_SYSTEM_TRACE_11
#define SOCK_SYSTEM_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_TRACE_11(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_SYSTEM_TRACE_12
#define SOCK_SYSTEM_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_TRACE_12(TRACE_SUBSYS_SOCK_SYSTEM, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif


#ifndef SOCK_PROTOCOL_TRACE_00
#define SOCK_PROTOCOL_TRACE_00(idx, level, msg)\
	LSA_TRACE_00(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg)
#endif

#ifndef SOCK_PROTOCOL_TRACE_01
#define SOCK_PROTOCOL_TRACE_01(idx, level, msg, para1)\
	LSA_TRACE_01(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1)
#endif

#ifndef SOCK_PROTOCOL_TRACE_02
#define SOCK_PROTOCOL_TRACE_02(idx, level, msg, para1, para2)\
	LSA_TRACE_02(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2)
#endif

#ifndef SOCK_PROTOCOL_TRACE_03
#define SOCK_PROTOCOL_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_TRACE_03(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_PROTOCOL_TRACE_04
#define SOCK_PROTOCOL_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_TRACE_04(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_PROTOCOL_TRACE_05
#define SOCK_PROTOCOL_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_TRACE_05(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_PROTOCOL_TRACE_06
#define SOCK_PROTOCOL_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_TRACE_06(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_PROTOCOL_TRACE_07
#define SOCK_PROTOCOL_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_TRACE_07(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_PROTOCOL_TRACE_08
#define SOCK_PROTOCOL_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_TRACE_08(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_PROTOCOL_TRACE_09
#define SOCK_PROTOCOL_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_TRACE_09(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_PROTOCOL_TRACE_10
#define SOCK_PROTOCOL_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_TRACE_10(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_PROTOCOL_TRACE_11
#define SOCK_PROTOCOL_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_TRACE_11(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_PROTOCOL_TRACE_12
#define SOCK_PROTOCOL_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_TRACE_12(TRACE_SUBSYS_SOCK_PROTOCOL, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif

/*****************************************************************************/

#elif SOCK_CFG_TRACE_MODE == 2 /* LSA indexed trace */

#ifndef SOCK_UPPER_TRACE_00
#define SOCK_UPPER_TRACE_00(idx, level, msg)\
	LSA_IDX_TRACE_00(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg)
#endif

#ifndef SOCK_UPPER_TRACE_01
#define SOCK_UPPER_TRACE_01(idx, level, msg, para1)\
	LSA_IDX_TRACE_01(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1)
#endif

#ifndef SOCK_UPPER_TRACE_02
#define SOCK_UPPER_TRACE_02(idx, level, msg, para1, para2)\
	LSA_IDX_TRACE_02(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2)
#endif

#ifndef SOCK_UPPER_TRACE_03
#define SOCK_UPPER_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_IDX_TRACE_03(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_UPPER_TRACE_04
#define SOCK_UPPER_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_IDX_TRACE_04(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_UPPER_TRACE_05
#define SOCK_UPPER_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_IDX_TRACE_05(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_UPPER_TRACE_06
#define SOCK_UPPER_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_IDX_TRACE_06(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_UPPER_TRACE_07
#define SOCK_UPPER_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_IDX_TRACE_07(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_UPPER_TRACE_08
#define SOCK_UPPER_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_IDX_TRACE_08(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_UPPER_TRACE_09
#define SOCK_UPPER_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_IDX_TRACE_09(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_UPPER_TRACE_10
#define SOCK_UPPER_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_IDX_TRACE_10(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_UPPER_TRACE_11
#define SOCK_UPPER_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_IDX_TRACE_11(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_UPPER_TRACE_12
#define SOCK_UPPER_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_IDX_TRACE_12(TRACE_SUBSYS_SOCK_UPPER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif


#ifndef SOCK_LOWER_TRACE_00
#define SOCK_LOWER_TRACE_00(idx, level, msg)\
	LSA_IDX_TRACE_00(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg)
#endif

#ifndef SOCK_LOWER_TRACE_01
#define SOCK_LOWER_TRACE_01(idx, level, msg, para1)\
	LSA_IDX_TRACE_01(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1)
#endif

#ifndef SOCK_LOWER_TRACE_02
#define SOCK_LOWER_TRACE_02(idx, level, msg, para1, para2)\
	LSA_IDX_TRACE_02(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2)
#endif

#ifndef SOCK_LOWER_TRACE_03
#define SOCK_LOWER_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_IDX_TRACE_03(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_LOWER_TRACE_04
#define SOCK_LOWER_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_IDX_TRACE_04(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_LOWER_TRACE_05
#define SOCK_LOWER_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_IDX_TRACE_05(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_LOWER_TRACE_06
#define SOCK_LOWER_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_IDX_TRACE_06(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_LOWER_TRACE_07
#define SOCK_LOWER_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_IDX_TRACE_07(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_LOWER_TRACE_08
#define SOCK_LOWER_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_IDX_TRACE_08(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_LOWER_TRACE_09
#define SOCK_LOWER_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_IDX_TRACE_09(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_LOWER_TRACE_10
#define SOCK_LOWER_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_IDX_TRACE_10(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_LOWER_TRACE_11
#define SOCK_LOWER_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_IDX_TRACE_11(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_LOWER_TRACE_12
#define SOCK_LOWER_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_IDX_TRACE_12(TRACE_SUBSYS_SOCK_LOWER_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif


#ifndef SOCK_SYSTEM_TRACE_00
#define SOCK_SYSTEM_TRACE_00(idx, level, msg)\
	LSA_IDX_TRACE_00(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg)
#endif

#ifndef SOCK_SYSTEM_TRACE_01
#define SOCK_SYSTEM_TRACE_01(idx, level, msg, para1)\
	LSA_IDX_TRACE_01(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1)
#endif

#ifndef SOCK_SYSTEM_TRACE_02
#define SOCK_SYSTEM_TRACE_02(idx, level, msg, para1, para2)\
	LSA_IDX_TRACE_02(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2)
#endif

#ifndef SOCK_SYSTEM_TRACE_03
#define SOCK_SYSTEM_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_IDX_TRACE_03(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_SYSTEM_TRACE_04
#define SOCK_SYSTEM_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_IDX_TRACE_04(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_SYSTEM_TRACE_05
#define SOCK_SYSTEM_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_IDX_TRACE_05(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_SYSTEM_TRACE_06
#define SOCK_SYSTEM_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_IDX_TRACE_06(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_SYSTEM_TRACE_07
#define SOCK_SYSTEM_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_IDX_TRACE_07(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_SYSTEM_TRACE_08
#define SOCK_SYSTEM_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_IDX_TRACE_08(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_SYSTEM_TRACE_09
#define SOCK_SYSTEM_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_IDX_TRACE_09(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_SYSTEM_TRACE_10
#define SOCK_SYSTEM_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_IDX_TRACE_10(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_SYSTEM_TRACE_11
#define SOCK_SYSTEM_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_IDX_TRACE_11(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_SYSTEM_TRACE_12
#define SOCK_SYSTEM_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_IDX_TRACE_12(TRACE_SUBSYS_SOCK_SYSTEM_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif


#ifndef SOCK_PROTOCOL_TRACE_00
#define SOCK_PROTOCOL_TRACE_00(idx, level, msg)\
	LSA_IDX_TRACE_00(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg)
#endif

#ifndef SOCK_PROTOCOL_TRACE_01
#define SOCK_PROTOCOL_TRACE_01(idx, level, msg, para1)\
	LSA_IDX_TRACE_01(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1)
#endif

#ifndef SOCK_PROTOCOL_TRACE_02
#define SOCK_PROTOCOL_TRACE_02(idx, level, msg, para1, para2)\
	LSA_IDX_TRACE_02(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2)
#endif

#ifndef SOCK_PROTOCOL_TRACE_03
#define SOCK_PROTOCOL_TRACE_03(idx, level, msg, para1, para2, para3)\
	LSA_IDX_TRACE_03(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3)
#endif

#ifndef SOCK_PROTOCOL_TRACE_04
#define SOCK_PROTOCOL_TRACE_04(idx, level, msg, para1, para2, para3, para4)\
	LSA_IDX_TRACE_04(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4)
#endif

#ifndef SOCK_PROTOCOL_TRACE_05
#define SOCK_PROTOCOL_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)\
	LSA_IDX_TRACE_05(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5)
#endif

#ifndef SOCK_PROTOCOL_TRACE_06
#define SOCK_PROTOCOL_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)\
	LSA_IDX_TRACE_06(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6)
#endif

#ifndef SOCK_PROTOCOL_TRACE_07
#define SOCK_PROTOCOL_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)\
	LSA_IDX_TRACE_07(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7)
#endif

#ifndef SOCK_PROTOCOL_TRACE_08
#define SOCK_PROTOCOL_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)\
	LSA_IDX_TRACE_08(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#endif

#ifndef SOCK_PROTOCOL_TRACE_09
#define SOCK_PROTOCOL_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)\
	LSA_IDX_TRACE_09(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#endif

#ifndef SOCK_PROTOCOL_TRACE_10
#define SOCK_PROTOCOL_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)\
	LSA_IDX_TRACE_10(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#endif

#ifndef SOCK_PROTOCOL_TRACE_11
#define SOCK_PROTOCOL_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)\
	LSA_IDX_TRACE_11(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#endif

#ifndef SOCK_PROTOCOL_TRACE_12
#define SOCK_PROTOCOL_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)\
	LSA_IDX_TRACE_12(TRACE_SUBSYS_SOCK_PROTOCOL_IDX00, idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#endif

/*****************************************************************************/

#else

#error "SOCK_CFG_TRACE_MODE has unknown value"

#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of SOCK_TRC_H */
