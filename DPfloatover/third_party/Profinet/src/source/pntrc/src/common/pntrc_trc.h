#ifndef PNTRC_TRC_H
#define PNTRC_TRC_H

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
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_trc.h                               :F&  */
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

/*
 *	PNTRC uses the sub-systems:
 *		TRACE_SUBSYS_PNTRC_UPPER		.. _UPPER_	.. PNTRC user
 *		TRACE_SUBSYS_PNTRC_LOWER		.. _LL_		.. EDD
 *		TRACE_SUBSYS_PNTRC_MEMORY		.. _MEM_	.. ALLOC/FREE; only used in pntrc_cfg.txt
 *		TRACE_SUBSYS_PNTRC_SYSTEM		.. _SYSTEM_	.. SYSTEM-protocol
 *
 *
 *	PNTRC usage of lsa-log-level:
 *
 *	LSA_TRACE_LEVEL_FATAL			.. calling PNTRC_FATAL_ERROR
 *	LSA_TRACE_LEVEL_ERROR			.. wrong para from upper, pntrc-opcode.cnf(-)
 *	LSA_TRACE_LEVEL_UNEXP			.. wrong para from lower / inside pntrc-library functions
 *	LSA_TRACE_LEVEL_WARN			.. ? changing state
 *
 *	LSA_TRACE_LEVEL_NOTE_HIGH		.. indications
 *	LSA_TRACE_LEVEL_NOTE			.. pntrc-opcode.req, pntrc-opcode.cnf(+)
 *	LSA_TRACE_LEVEL_NOTE_LOW		.. ? program flow
 *
 *	LSA_TRACE_LEVEL_CHAT			.. program flow
 *
 *
 *	TODO: PNTRC usage of references:
 *	...
 *	aref(%u/%u)		..	local.alarm_ref, remote.alarm_ref
 *	aref(%u/%u/%u)	..	local.alarm_ref, remote.alarm_ref, .priority
 *
 */


/*=============================================================================
 * macro names:   PNTRC_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *  or
 * macro names:   PNTRC_YYYYY_IDX_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_PNTRC_YYYYY (if LTRC is used in the file
 *                ltrc_sub.h/txt or if LTRC isn't used in a system file)!
 *
 * parameters:    idx_:     index
 *                level_:   LSA_TRACE_LEVEL_OFF, LSA_TRACE_LEVEL_FATAL,
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

#if PNTRC_CFG_TRACE_MODE == 0 /* no trace */

#define PNTRC_UPPER_TRACE_00(idx_, level_, msg_)
#define PNTRC_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define PNTRC_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PNTRC_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PNTRC_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PNTRC_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PNTRC_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PNTRC_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PNTRC_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PNTRC_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PNTRC_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PNTRC_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PNTRC_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PNTRC_LOWER_TRACE_00(idx_, level_, msg_)
#define PNTRC_LOWER_TRACE_01(idx_, level_, msg_, para1_)
#define PNTRC_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PNTRC_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PNTRC_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PNTRC_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PNTRC_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PNTRC_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PNTRC_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PNTRC_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PNTRC_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PNTRC_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PNTRC_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PNTRC_MEMORY_TRACE_00(idx_, level_, msg_)
#define PNTRC_MEMORY_TRACE_01(idx_, level_, msg_, para1_)
#define PNTRC_MEMORY_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PNTRC_MEMORY_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PNTRC_MEMORY_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PNTRC_MEMORY_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PNTRC_MEMORY_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PNTRC_MEMORY_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PNTRC_MEMORY_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PNTRC_MEMORY_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PNTRC_MEMORY_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PNTRC_MEMORY_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PNTRC_MEMORY_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define PNTRC_SYSTEM_TRACE_00(idx_, level_, msg_)
#define PNTRC_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define PNTRC_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define PNTRC_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define PNTRC_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define PNTRC_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define PNTRC_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define PNTRC_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define PNTRC_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define PNTRC_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define PNTRC_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define PNTRC_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define PNTRC_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)


/*----------------------------------------------------------------------------*/
#elif PNTRC_CFG_TRACE_MODE == 1 /* LSA trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_UPPER */

#ifndef PNTRC_UPPER_TRACE_00
#define PNTRC_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_)
#endif

#ifndef PNTRC_UPPER_TRACE_01
#define PNTRC_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_)
#endif

#ifndef PNTRC_UPPER_TRACE_02
#define PNTRC_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_UPPER_TRACE_03
#define PNTRC_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_UPPER_TRACE_04
#define PNTRC_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PNTRC_UPPER_TRACE_05
#define PNTRC_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PNTRC_UPPER_TRACE_06
#define PNTRC_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PNTRC_UPPER_TRACE_07
#define PNTRC_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PNTRC_UPPER_TRACE_08
#define PNTRC_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PNTRC_UPPER_TRACE_09
#define PNTRC_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PNTRC_UPPER_TRACE_10
#define PNTRC_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PNTRC_UPPER_TRACE_11
#define PNTRC_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PNTRC_UPPER_TRACE_12
#define PNTRC_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_PNTRC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_LOWER */

#ifndef PNTRC_LL_TRACE_00
#define PNTRC_LL_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_)
#endif

#ifndef PNTRC_LL_TRACE_01
#define PNTRC_LL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_)
#endif

#ifndef PNTRC_LL_TRACE_02
#define PNTRC_LL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_LL_TRACE_03
#define PNTRC_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_LL_TRACE_04
#define PNTRC_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PNTRC_LL_TRACE_05
#define PNTRC_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PNTRC_LL_TRACE_06
#define PNTRC_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PNTRC_LL_TRACE_07
#define PNTRC_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PNTRC_LL_TRACE_08
#define PNTRC_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PNTRC_LL_TRACE_09
#define PNTRC_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PNTRC_LL_TRACE_10
#define PNTRC_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PNTRC_LL_TRACE_11
#define PNTRC_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PNTRC_LL_TRACE_12
#define PNTRC_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_PNTRC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_MEMORY */

#ifndef PNTRC_MEMORY_TRACE_00
#define PNTRC_MEMORY_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_PNTRC_MEMORY, level_, msg_)
#endif

#ifndef PNTRC_MEMORY_TRACE_01
#define PNTRC_MEMORY_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_PNTRC_MEMORY, level_, msg_, para1_)
#endif

#ifndef PNTRC_MEMORY_TRACE_02
#define PNTRC_MEMORY_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_PNTRC_MEMORY, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_MEMORY_TRACE_03
#define PNTRC_MEMORY_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_PNTRC_MEMORY, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_MEMORY_TRACE_04
#define PNTRC_MEMORY_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_PNTRC_MEMORY, level_, msg_, para1_, para2_, para3_, para4_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_SYSTEM */

#ifndef PNTRC_SYSTEM_TRACE_00
#define PNTRC_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_01
#define PNTRC_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_02
#define PNTRC_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_03
#define PNTRC_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_04
#define PNTRC_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_05
#define PNTRC_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_06
#define PNTRC_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_07
#define PNTRC_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_08
#define PNTRC_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_09
#define PNTRC_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_10
#define PNTRC_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_11
#define PNTRC_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_12
#define PNTRC_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_PNTRC_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
#elif PNTRC_CFG_TRACE_MODE == 2 /* LSA index trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_UPPER */

#ifndef PNTRC_UPPER_TRACE_00
#define PNTRC_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_)
#endif

#ifndef PNTRC_UPPER_TRACE_01
#define PNTRC_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef PNTRC_UPPER_TRACE_02
#define PNTRC_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_UPPER_TRACE_03
#define PNTRC_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_UPPER_TRACE_04
#define PNTRC_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PNTRC_UPPER_TRACE_05
#define PNTRC_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PNTRC_UPPER_TRACE_06
#define PNTRC_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PNTRC_UPPER_TRACE_07
#define PNTRC_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PNTRC_UPPER_TRACE_08
#define PNTRC_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PNTRC_UPPER_TRACE_09
#define PNTRC_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PNTRC_UPPER_TRACE_10
#define PNTRC_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PNTRC_UPPER_TRACE_11
#define PNTRC_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PNTRC_UPPER_TRACE_12
#define PNTRC_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PNTRC_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_LOWER */

#ifndef PNTRC_LL_TRACE_00
#define PNTRC_LL_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_)
#endif

#ifndef PNTRC_LL_TRACE_01
#define PNTRC_LL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef PNTRC_LL_TRACE_02
#define PNTRC_LL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_LL_TRACE_03
#define PNTRC_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_LL_TRACE_04
#define PNTRC_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PNTRC_LL_TRACE_05
#define PNTRC_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PNTRC_LL_TRACE_06
#define PNTRC_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PNTRC_LL_TRACE_07
#define PNTRC_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PNTRC_LL_TRACE_08
#define PNTRC_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PNTRC_LL_TRACE_09
#define PNTRC_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PNTRC_LL_TRACE_10
#define PNTRC_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PNTRC_LL_TRACE_11
#define PNTRC_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PNTRC_LL_TRACE_12
#define PNTRC_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PNTRC_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_MEMORY */

#ifndef PNTRC_MEMORY_TRACE_00
#define PNTRC_MEMORY_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PNTRC_MEMORY_IDX00, idx_, level_, msg_)
#endif

#ifndef PNTRC_MEMORY_TRACE_01
#define PNTRC_MEMORY_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PNTRC_MEMORY_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef PNTRC_MEMORY_TRACE_02
#define PNTRC_MEMORY_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PNTRC_MEMORY_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_MEMORY_TRACE_03
#define PNTRC_MEMORY_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PNTRC_MEMORY_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_MEMORY_TRACE_04
#define PNTRC_MEMORY_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PNTRC_MEMORY_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_PNTRC_SYSTEM */

#ifndef PNTRC_SYSTEM_TRACE_00
#define PNTRC_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_01
#define PNTRC_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_02
#define PNTRC_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_03
#define PNTRC_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_04
#define PNTRC_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_05
#define PNTRC_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_06
#define PNTRC_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_07
#define PNTRC_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_08
#define PNTRC_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_09
#define PNTRC_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_10
#define PNTRC_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_11
#define PNTRC_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_12
#define PNTRC_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_13
#define PNTRC_SYSTEM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_) \
    LSA_IDX_TRACE_13(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_14
#define PNTRC_SYSTEM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_) \
    LSA_IDX_TRACE_13(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_15
#define PNTRC_SYSTEM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_) \
    LSA_IDX_TRACE_13(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_16
#define PNTRC_SYSTEM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_) \
    LSA_IDX_TRACE_13(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_BYTE_ARRAY
#define PNTRC_SYSTEM_TRACE_BYTE_ARRAY(idx_, level_, msg_,  ptr_, len_) \
    LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_,  ptr_, len_)
#endif

#ifndef PNTRC_SYSTEM_TRACE_STRING
#define PNTRC_SYSTEM_TRACE_STRING(idx_, level_, msg_,  st) \
    LSA_IDX_TRACE_STRING(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_,  st)
#endif

#ifndef PNTRC_SYSTEM_TRACE_SYNC_TIME
#define PNTRC_SYSTEM_TRACE_SYNC_TIME(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_SYNC_TIME(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

/*----------------------------------------------------------------------------*/
#else

#error "PNTRC_CFG_TRACE_MODE has unknown value"

#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNTRC_TRC_H */
