#ifndef ACP_TRC_H
#define ACP_TRC_H

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
/*  F i l e               &F: acp_trc.h                                 :F&  */
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
 *	ACP uses the sub-systems:
 *		TRACE_SUBSYS_ACP_UPPER		.. _UPPER_	.. ACP user
 *		TRACE_SUBSYS_ACP_LOWER		.. _LL_		.. EDD
 *		TRACE_SUBSYS_ACP_MEMORY		.. _MEM_	.. ALLOC/FREE; only used in acp_cfg.txt
 *		TRACE_SUBSYS_ACP_RTA		.. _RTA_	.. RTA-protocol
 *
 *
 *	ACP usage of lsa-log-level:
 *
 *	LSA_TRACE_LEVEL_FATAL			.. calling ACP_FATAL_ERROR
 *	LSA_TRACE_LEVEL_ERROR			.. wrong para from upper, acp-opcode.cnf(-)
 *	LSA_TRACE_LEVEL_UNEXP			.. wrong para from lower / inside acp-library functions
 *	LSA_TRACE_LEVEL_WARN			.. ? changing state
 *
 *	LSA_TRACE_LEVEL_NOTE_HIGH		.. indications
 *	LSA_TRACE_LEVEL_NOTE			.. acp-opcode.req, acp-opcode.cnf(+)
 *	LSA_TRACE_LEVEL_NOTE_LOW		.. ? program flow
 *
 *	LSA_TRACE_LEVEL_CHAT			.. program flow
 *
 *
 *	TODO: ACP usage of references:
 *	...
 *	aref(%u/%u)		..	local.alarm_ref, remote.alarm_ref
 *	aref(%u/%u/%u)	..	local.alarm_ref, remote.alarm_ref, .priority
 *
 */


/*=============================================================================
 * macro names:   ACP_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *  or
 * macro names:   ACP_YYYYY_IDX_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_ACP_YYYYY (if LTRC is used in the file
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

#if ACP_CFG_TRACE_MODE == 0 /* no trace */

#define ACP_UPPER_TRACE_00(idx_, level_, msg_)
#define ACP_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define ACP_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define ACP_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define ACP_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define ACP_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define ACP_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define ACP_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define ACP_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define ACP_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define ACP_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define ACP_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define ACP_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define ACP_LL_TRACE_00(idx_, level_, msg_)
#define ACP_LL_TRACE_01(idx_, level_, msg_, para1_)
#define ACP_LL_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define ACP_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define ACP_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define ACP_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define ACP_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define ACP_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define ACP_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define ACP_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define ACP_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define ACP_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define ACP_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define ACP_MEM_TRACE_00(idx_, level_, msg_)
#define ACP_MEM_TRACE_01(idx_, level_, msg_, para1_)
#define ACP_MEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define ACP_MEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define ACP_MEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define ACP_MEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define ACP_MEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define ACP_MEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define ACP_MEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define ACP_MEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define ACP_MEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define ACP_MEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define ACP_MEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define ACP_RTA_TRACE_00(idx_, level_, msg_)
#define ACP_RTA_TRACE_01(idx_, level_, msg_, para1_)
#define ACP_RTA_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define ACP_RTA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define ACP_RTA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define ACP_RTA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define ACP_RTA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define ACP_RTA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define ACP_RTA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define ACP_RTA_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define ACP_RTA_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define ACP_RTA_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define ACP_RTA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)


/*----------------------------------------------------------------------------*/
#elif ACP_CFG_TRACE_MODE == 1 /* LSA trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_UPPER */

#ifndef ACP_UPPER_TRACE_00
#define ACP_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_ACP_UPPER, level_, msg_)
#endif

#ifndef ACP_UPPER_TRACE_01
#define ACP_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_)
#endif

#ifndef ACP_UPPER_TRACE_02
#define ACP_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_UPPER_TRACE_03
#define ACP_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_UPPER_TRACE_04
#define ACP_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef ACP_UPPER_TRACE_05
#define ACP_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef ACP_UPPER_TRACE_06
#define ACP_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef ACP_UPPER_TRACE_07
#define ACP_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef ACP_UPPER_TRACE_08
#define ACP_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef ACP_UPPER_TRACE_09
#define ACP_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef ACP_UPPER_TRACE_10
#define ACP_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef ACP_UPPER_TRACE_11
#define ACP_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef ACP_UPPER_TRACE_12
#define ACP_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_ACP_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_LOWER */

#ifndef ACP_LL_TRACE_00
#define ACP_LL_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_ACP_LOWER, level_, msg_)
#endif

#ifndef ACP_LL_TRACE_01
#define ACP_LL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_)
#endif

#ifndef ACP_LL_TRACE_02
#define ACP_LL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_LL_TRACE_03
#define ACP_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_LL_TRACE_04
#define ACP_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef ACP_LL_TRACE_05
#define ACP_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef ACP_LL_TRACE_06
#define ACP_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef ACP_LL_TRACE_07
#define ACP_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef ACP_LL_TRACE_08
#define ACP_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef ACP_LL_TRACE_09
#define ACP_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef ACP_LL_TRACE_10
#define ACP_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef ACP_LL_TRACE_11
#define ACP_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef ACP_LL_TRACE_12
#define ACP_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_ACP_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_MEMORY */

#ifndef ACP_MEM_TRACE_00
#define ACP_MEM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_ACP_MEMORY, level_, msg_)
#endif

#ifndef ACP_MEM_TRACE_01
#define ACP_MEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_ACP_MEMORY, level_, msg_, para1_)
#endif

#ifndef ACP_MEM_TRACE_02
#define ACP_MEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_ACP_MEMORY, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_MEM_TRACE_03
#define ACP_MEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_ACP_MEMORY, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_MEM_TRACE_04
#define ACP_MEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_ACP_MEMORY, level_, msg_, para1_, para2_, para3_, para4_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_RTA */

#ifndef ACP_RTA_TRACE_00
#define ACP_RTA_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_ACP_RTA, level_, msg_)
#endif

#ifndef ACP_RTA_TRACE_01
#define ACP_RTA_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_)
#endif

#ifndef ACP_RTA_TRACE_02
#define ACP_RTA_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_RTA_TRACE_03
#define ACP_RTA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_RTA_TRACE_04
#define ACP_RTA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef ACP_RTA_TRACE_05
#define ACP_RTA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef ACP_RTA_TRACE_06
#define ACP_RTA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef ACP_RTA_TRACE_07
#define ACP_RTA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef ACP_RTA_TRACE_08
#define ACP_RTA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef ACP_RTA_TRACE_09
#define ACP_RTA_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef ACP_RTA_TRACE_10
#define ACP_RTA_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef ACP_RTA_TRACE_11
#define ACP_RTA_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef ACP_RTA_TRACE_12
#define ACP_RTA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_ACP_RTA, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
#elif ACP_CFG_TRACE_MODE == 2 /* LSA index trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_UPPER */

#ifndef ACP_UPPER_TRACE_00
#define ACP_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_)
#endif

#ifndef ACP_UPPER_TRACE_01
#define ACP_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef ACP_UPPER_TRACE_02
#define ACP_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_UPPER_TRACE_03
#define ACP_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_UPPER_TRACE_04
#define ACP_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef ACP_UPPER_TRACE_05
#define ACP_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef ACP_UPPER_TRACE_06
#define ACP_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef ACP_UPPER_TRACE_07
#define ACP_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef ACP_UPPER_TRACE_08
#define ACP_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef ACP_UPPER_TRACE_09
#define ACP_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef ACP_UPPER_TRACE_10
#define ACP_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef ACP_UPPER_TRACE_11
#define ACP_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef ACP_UPPER_TRACE_12
#define ACP_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_ACP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_LOWER */

#ifndef ACP_LL_TRACE_00
#define ACP_LL_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_)
#endif

#ifndef ACP_LL_TRACE_01
#define ACP_LL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef ACP_LL_TRACE_02
#define ACP_LL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_LL_TRACE_03
#define ACP_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_LL_TRACE_04
#define ACP_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef ACP_LL_TRACE_05
#define ACP_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef ACP_LL_TRACE_06
#define ACP_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef ACP_LL_TRACE_07
#define ACP_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef ACP_LL_TRACE_08
#define ACP_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef ACP_LL_TRACE_09
#define ACP_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef ACP_LL_TRACE_10
#define ACP_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef ACP_LL_TRACE_11
#define ACP_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef ACP_LL_TRACE_12
#define ACP_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_ACP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_MEMORY */

#ifndef ACP_MEM_TRACE_00
#define ACP_MEM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_ACP_MEMORY_IDX00, idx_, level_, msg_)
#endif

#ifndef ACP_MEM_TRACE_01
#define ACP_MEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_ACP_MEMORY_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef ACP_MEM_TRACE_02
#define ACP_MEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_ACP_MEMORY_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_MEM_TRACE_03
#define ACP_MEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_ACP_MEMORY_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_MEM_TRACE_04
#define ACP_MEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_ACP_MEMORY_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif


/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_ACP_RTA */

#ifndef ACP_RTA_TRACE_00
#define ACP_RTA_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_)
#endif

#ifndef ACP_RTA_TRACE_01
#define ACP_RTA_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef ACP_RTA_TRACE_02
#define ACP_RTA_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef ACP_RTA_TRACE_03
#define ACP_RTA_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef ACP_RTA_TRACE_04
#define ACP_RTA_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef ACP_RTA_TRACE_05
#define ACP_RTA_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef ACP_RTA_TRACE_06
#define ACP_RTA_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef ACP_RTA_TRACE_07
#define ACP_RTA_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef ACP_RTA_TRACE_08
#define ACP_RTA_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef ACP_RTA_TRACE_09
#define ACP_RTA_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef ACP_RTA_TRACE_10
#define ACP_RTA_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef ACP_RTA_TRACE_11
#define ACP_RTA_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef ACP_RTA_TRACE_12
#define ACP_RTA_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_ACP_RTA_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
#else

#error "ACP_CFG_TRACE_MODE has unknown value"

#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_TRC_H */
