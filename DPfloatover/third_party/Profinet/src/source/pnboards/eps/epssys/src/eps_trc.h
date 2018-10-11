#ifndef EPS_TRC_H_
#define EPS_TRC_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_trc.h                                 :F&  */
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
 *	EPS uses the sub-systems:
 *    TRACE_SUBSYS_EPS_SYSTEM
 *
 *	EPS usage of lsa-log-level:
 *
 *	LSA_TRACE_LEVEL_FATAL			.. calling EPS_FATAL_ERROR
 *	LSA_TRACE_LEVEL_ERROR			.. wrong para from upper, eps-opcode.cnf(-)
 *	LSA_TRACE_LEVEL_UNEXP			.. wrong para from lower / inside eps-library functions
 *	LSA_TRACE_LEVEL_WARN			.. changing state
 *
 *	LSA_TRACE_LEVEL_NOTE_HIGH	    .. interrupts, receive, send, timeout
 *	LSA_TRACE_LEVEL_NOTE			.. eps-opcode.req, eps-opcode.cnf(+)
 *	LSA_TRACE_LEVEL_NOTE_LOW        .. program flow
 *
 *	LSA_TRACE_LEVEL_CHAT			.. program flow
 *
 *
 *	TODO: EPS usage of references:
 *	...
 *
 */

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef EPS_FILE_SYSTEM_EXTENSION
# define EPS_FILE_SYSTEM_EXTENSION(module_id_) /* nothing */
#endif

/*=============================================================================
 * macro names:   EPS_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *  or
 * macro names:   EPS_YYYYY_IDX_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_EPS_YYYYY (if LTRC is used in the file
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

#if PSI_CFG_TRACE_MODE == 0 /* no trace */

#define EPS_SYSTEM_TRACE_00(idx_, level_, msg_)
#define EPS_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define EPS_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EPS_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EPS_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EPS_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EPS_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EPS_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EPS_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EPS_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define EPS_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define EPS_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define EPS_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

/*----------------------------------------------------------------------------*/
#elif PSI_CFG_TRACE_MODE == 1 /* LSA trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_EPS_SYSTEM */

#ifndef EPS_SYSTEM_TRACE_00
#define EPS_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_)
#endif

#ifndef EPS_SYSTEM_TRACE_01
#define EPS_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_)
#endif

#ifndef EPS_SYSTEM_TRACE_02
#define EPS_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_)
#endif

#ifndef EPS_SYSTEM_TRACE_03
#define EPS_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EPS_SYSTEM_TRACE_04
#define EPS_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EPS_SYSTEM_TRACE_05
#define EPS_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EPS_SYSTEM_TRACE_06
#define EPS_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EPS_SYSTEM_TRACE_07
#define EPS_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EPS_SYSTEM_TRACE_08
#define EPS_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EPS_SYSTEM_TRACE_09
#define EPS_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EPS_SYSTEM_TRACE_10
#define EPS_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EPS_SYSTEM_TRACE_11
#define EPS_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef EPS_SYSTEM_TRACE_12
#define EPS_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

#ifndef EPS_SYSTEM_TRACE_STRING
#define EPS_SYSTEM_TRACE_STRING (idx_, level_, msg_, st) \
    LSA_TRACE_STRING(TRACE_SUBSYS_EPS_SYSTEM, level_, msg_, st)
#endif

/*----------------------------------------------------------------------------*/
#elif PSI_CFG_TRACE_MODE == 2 /* LSA index trace */

/*----------------------------------------------------------------------------*/
/* TRACE_SUBSYS_EPS_SYSTEM */

#ifndef EPS_SYSTEM_TRACE_00
#define EPS_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_)
#endif

#ifndef EPS_SYSTEM_TRACE_01
#define EPS_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EPS_SYSTEM_TRACE_02
#define EPS_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EPS_SYSTEM_TRACE_03
#define EPS_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EPS_SYSTEM_TRACE_04
#define EPS_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EPS_SYSTEM_TRACE_05
#define EPS_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EPS_SYSTEM_TRACE_06
#define EPS_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EPS_SYSTEM_TRACE_07
#define EPS_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EPS_SYSTEM_TRACE_08
#define EPS_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EPS_SYSTEM_TRACE_09
#define EPS_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EPS_SYSTEM_TRACE_10
#define EPS_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EPS_SYSTEM_TRACE_11
#define EPS_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef EPS_SYSTEM_TRACE_12
#define EPS_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

#ifndef EPS_SYSTEM_TRACE_STRING
#define EPS_SYSTEM_TRACE_STRING(idx_, level_, msg_, st) \
    LSA_TRACE_STRING(TRACE_SUBSYS_EPS_SYSTEM_IDX00, idx_, level_, msg_, st)
#endif

#ifndef EPS_SYSTEM_TRACE_SYNC_TIME
#define EPS_SYSTEM_TRACE_SYNC_TIME(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_SYNC_TIME(TRACE_SUBSYS_PNTRC_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

/*----------------------------------------------------------------------------*/
#else

#error "PSI_CFG_TRACE_MODE has unknown value"

#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_TRC_H */
