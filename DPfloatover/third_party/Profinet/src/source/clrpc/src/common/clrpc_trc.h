#ifndef CLRPC_TRC_H
#define CLRPC_TRC_H

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
/*  C o m p o n e n t     &C: CLRPC (ConnectionLess RPC)                :C&  */
/*                                                                           */
/*  F i l e               &F: clrpc_trc.h                               :F&  */
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
 *	CLRPC uses the sub-systems:
 *		TRACE_SUBSYS_CLRPC_UPPER	(Common)
 *		TRACE_SUBSYS_CLRPC_CL		(Client)
 *		TRACE_SUBSYS_CLRPC_CL_PKT	(Client paket)
 *		TRACE_SUBSYS_CLRPC_SV		(Server)
 *		TRACE_SUBSYS_CLRPC_SV_PKT	(Server paket)
 *		TRACE_SUBSYS_CLRPC_EPM		(Endpoint-mapper)
 *		TRACE_SUBSYS_CLRPC_LOWER	(LOWER SOCK)
 *
 *	note: the subsystems of LTRC are defined per enum
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
 * macro names:   CLRPC_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_CLRPC_YYYYY (if LTRC is used in the file
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
#if CLRPC_CFG_TRACE_MODE == 0 /* no trace */


#define CLRPC_UPPER_TRACE_00(idx_, level_, msg_)
#define CLRPC_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define CLRPC_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CLRPC_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CLRPC_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CLRPC_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CLRPC_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CLRPC_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CLRPC_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CLRPC_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CLRPC_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CLRPC_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CLRPC_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CLRPC_CL_TRACE_00(idx_, level_, msg_)
#define CLRPC_CL_TRACE_01(idx_, level_, msg_, para1_)
#define CLRPC_CL_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CLRPC_CL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CLRPC_CL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CLRPC_CL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CLRPC_CL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CLRPC_CL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CLRPC_CL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CLRPC_CL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CLRPC_CL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CLRPC_CL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CLRPC_CL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CLRPC_CL_PKT_TRACE_00(idx_, level_, msg_)
#define CLRPC_CL_PKT_TRACE_01(idx_, level_, msg_, para1_)
#define CLRPC_CL_PKT_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CLRPC_CL_PKT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CLRPC_CL_PKT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CLRPC_CL_PKT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CLRPC_CL_PKT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CLRPC_CL_PKT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CLRPC_CL_PKT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CLRPC_CL_PKT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CLRPC_CL_PKT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CLRPC_CL_PKT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CLRPC_CL_PKT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CLRPC_SV_TRACE_00(idx_, level_, msg_)
#define CLRPC_SV_TRACE_01(idx_, level_, msg_, para1_)
#define CLRPC_SV_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CLRPC_SV_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CLRPC_SV_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CLRPC_SV_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CLRPC_SV_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CLRPC_SV_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CLRPC_SV_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CLRPC_SV_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CLRPC_SV_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CLRPC_SV_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CLRPC_SV_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CLRPC_SV_PKT_TRACE_00(idx_, level_, msg_)
#define CLRPC_SV_PKT_TRACE_01(idx_, level_, msg_, para1_)
#define CLRPC_SV_PKT_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CLRPC_SV_PKT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CLRPC_SV_PKT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CLRPC_SV_PKT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CLRPC_SV_PKT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CLRPC_SV_PKT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CLRPC_SV_PKT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CLRPC_SV_PKT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CLRPC_SV_PKT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CLRPC_SV_PKT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CLRPC_SV_PKT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CLRPC_EPM_TRACE_00(idx_, level_, msg_)
#define CLRPC_EPM_TRACE_01(idx_, level_, msg_, para1_)
#define CLRPC_EPM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CLRPC_EPM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CLRPC_EPM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CLRPC_EPM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CLRPC_EPM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CLRPC_EPM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CLRPC_EPM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CLRPC_EPM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CLRPC_EPM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CLRPC_EPM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CLRPC_EPM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

#define CLRPC_LOWER_TRACE_00(idx_, level_, msg_)
#define CLRPC_LOWER_TRACE_01(idx_, level_, msg_, para1_)
#define CLRPC_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define CLRPC_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define CLRPC_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define CLRPC_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define CLRPC_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define CLRPC_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define CLRPC_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define CLRPC_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define CLRPC_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define CLRPC_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define CLRPC_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)

/*----------------------------------------------------------------------------*/
#elif CLRPC_CFG_TRACE_MODE == 1 /* LSA trace */


#ifndef CLRPC_UPPER_TRACE_00
#define CLRPC_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_)
#endif

#ifndef CLRPC_UPPER_TRACE_01
#define CLRPC_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_)
#endif

#ifndef CLRPC_UPPER_TRACE_02
#define CLRPC_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_UPPER_TRACE_03
#define CLRPC_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_UPPER_TRACE_04
#define CLRPC_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_UPPER_TRACE_05
#define CLRPC_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_UPPER_TRACE_06
#define CLRPC_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_UPPER_TRACE_07
#define CLRPC_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_UPPER_TRACE_08
#define CLRPC_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_UPPER_TRACE_09
#define CLRPC_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_UPPER_TRACE_10
#define CLRPC_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_UPPER_TRACE_11
#define CLRPC_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_UPPER_TRACE_12
#define CLRPC_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CLRPC_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CLRPC_CL_TRACE_00
#define CLRPC_CL_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CLRPC_CL, level_, msg_)
#endif

#ifndef CLRPC_CL_TRACE_01
#define CLRPC_CL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_)
#endif

#ifndef CLRPC_CL_TRACE_02
#define CLRPC_CL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_CL_TRACE_03
#define CLRPC_CL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_CL_TRACE_04
#define CLRPC_CL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_CL_TRACE_05
#define CLRPC_CL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_CL_TRACE_06
#define CLRPC_CL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_CL_TRACE_07
#define CLRPC_CL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_CL_TRACE_08
#define CLRPC_CL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_CL_TRACE_09
#define CLRPC_CL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_CL_TRACE_10
#define CLRPC_CL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_CL_TRACE_11
#define CLRPC_CL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_CL_TRACE_12
#define CLRPC_CL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CLRPC_CL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

/*----------------------------------------------------------------------------*/

#ifndef CLRPC_CL_PKT_TRACE_00
#define CLRPC_CL_PKT_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_01
#define CLRPC_CL_PKT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_02
#define CLRPC_CL_PKT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_03
#define CLRPC_CL_PKT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_04
#define CLRPC_CL_PKT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_05
#define CLRPC_CL_PKT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_06
#define CLRPC_CL_PKT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_07
#define CLRPC_CL_PKT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_08
#define CLRPC_CL_PKT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_09
#define CLRPC_CL_PKT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_10
#define CLRPC_CL_PKT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_11
#define CLRPC_CL_PKT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_12
#define CLRPC_CL_PKT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CLRPC_CL_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CLRPC_SV_TRACE_00
#define CLRPC_SV_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CLRPC_SV, level_, msg_)
#endif

#ifndef CLRPC_SV_TRACE_01
#define CLRPC_SV_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_)
#endif

#ifndef CLRPC_SV_TRACE_02
#define CLRPC_SV_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_SV_TRACE_03
#define CLRPC_SV_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_SV_TRACE_04
#define CLRPC_SV_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_SV_TRACE_05
#define CLRPC_SV_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_SV_TRACE_06
#define CLRPC_SV_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_SV_TRACE_07
#define CLRPC_SV_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_SV_TRACE_08
#define CLRPC_SV_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_SV_TRACE_09
#define CLRPC_SV_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_SV_TRACE_10
#define CLRPC_SV_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_SV_TRACE_11
#define CLRPC_SV_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_SV_TRACE_12
#define CLRPC_SV_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CLRPC_SV, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CLRPC_SV_PKT_TRACE_00
#define CLRPC_SV_PKT_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_01
#define CLRPC_SV_PKT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_02
#define CLRPC_SV_PKT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_03
#define CLRPC_SV_PKT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_04
#define CLRPC_SV_PKT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_05
#define CLRPC_SV_PKT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_06
#define CLRPC_SV_PKT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_07
#define CLRPC_SV_PKT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_08
#define CLRPC_SV_PKT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_09
#define CLRPC_SV_PKT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_10
#define CLRPC_SV_PKT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_11
#define CLRPC_SV_PKT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_12
#define CLRPC_SV_PKT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CLRPC_SV_PKT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CLRPC_EPM_TRACE_00
#define CLRPC_EPM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CLRPC_EPM, level_, msg_)
#endif

#ifndef CLRPC_EPM_TRACE_01
#define CLRPC_EPM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_)
#endif

#ifndef CLRPC_EPM_TRACE_02
#define CLRPC_EPM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_EPM_TRACE_03
#define CLRPC_EPM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_EPM_TRACE_04
#define CLRPC_EPM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_EPM_TRACE_05
#define CLRPC_EPM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_EPM_TRACE_06
#define CLRPC_EPM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_EPM_TRACE_07
#define CLRPC_EPM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_EPM_TRACE_08
#define CLRPC_EPM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_EPM_TRACE_09
#define CLRPC_EPM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_EPM_TRACE_10
#define CLRPC_EPM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_EPM_TRACE_11
#define CLRPC_EPM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_EPM_TRACE_12
#define CLRPC_EPM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CLRPC_EPM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CLRPC_LOWER_TRACE_00
#define CLRPC_LOWER_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_)
#endif

#ifndef CLRPC_LOWER_TRACE_01
#define CLRPC_LOWER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_)
#endif

#ifndef CLRPC_LOWER_TRACE_02
#define CLRPC_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_LOWER_TRACE_03
#define CLRPC_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_LOWER_TRACE_04
#define CLRPC_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_LOWER_TRACE_05
#define CLRPC_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_LOWER_TRACE_06
#define CLRPC_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_LOWER_TRACE_07
#define CLRPC_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_LOWER_TRACE_08
#define CLRPC_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_LOWER_TRACE_09
#define CLRPC_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_LOWER_TRACE_10
#define CLRPC_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_LOWER_TRACE_11
#define CLRPC_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_LOWER_TRACE_12
#define CLRPC_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_CLRPC_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
#elif CLRPC_CFG_TRACE_MODE == 2 /* LSA index trace */

#ifndef CLRPC_UPPER_TRACE_00
#define CLRPC_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_)
#endif

#ifndef CLRPC_UPPER_TRACE_01
#define CLRPC_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_)
#endif

#ifndef CLRPC_UPPER_TRACE_02
#define CLRPC_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_UPPER_TRACE_03
#define CLRPC_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_UPPER_TRACE_04
#define CLRPC_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_UPPER_TRACE_05
#define CLRPC_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_UPPER_TRACE_06
#define CLRPC_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_UPPER_TRACE_07
#define CLRPC_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_UPPER_TRACE_08
#define CLRPC_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_UPPER_TRACE_09
#define CLRPC_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_UPPER_TRACE_10
#define CLRPC_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_UPPER_TRACE_11
#define CLRPC_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_UPPER_TRACE_12
#define CLRPC_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CLRPC_UPPER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

/*----------------------------------------------------------------------------*/

#ifndef CLRPC_CL_TRACE_00
#define CLRPC_CL_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_)
#endif

#ifndef CLRPC_CL_TRACE_01
#define CLRPC_CL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_)
#endif

#ifndef CLRPC_CL_TRACE_02
#define CLRPC_CL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_CL_TRACE_03
#define CLRPC_CL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_CL_TRACE_04
#define CLRPC_CL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_CL_TRACE_05
#define CLRPC_CL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_CL_TRACE_06
#define CLRPC_CL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_CL_TRACE_07
#define CLRPC_CL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_CL_TRACE_08
#define CLRPC_CL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_CL_TRACE_09
#define CLRPC_CL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_CL_TRACE_10
#define CLRPC_CL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_CL_TRACE_11
#define CLRPC_CL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_CL_TRACE_12
#define CLRPC_CL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CLRPC_CL_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

/*----------------------------------------------------------------------------*/

#ifndef CLRPC_CL_PKT_TRACE_00
#define CLRPC_CL_PKT_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_01
#define CLRPC_CL_PKT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_02
#define CLRPC_CL_PKT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_03
#define CLRPC_CL_PKT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_04
#define CLRPC_CL_PKT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_05
#define CLRPC_CL_PKT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_06
#define CLRPC_CL_PKT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_07
#define CLRPC_CL_PKT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_08
#define CLRPC_CL_PKT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_09
#define CLRPC_CL_PKT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_10
#define CLRPC_CL_PKT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_11
#define CLRPC_CL_PKT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_CL_PKT_TRACE_12
#define CLRPC_CL_PKT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CLRPC_CL_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

/*----------------------------------------------------------------------------*/

#ifndef CLRPC_SV_TRACE_00
#define CLRPC_SV_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_)
#endif

#ifndef CLRPC_SV_TRACE_01
#define CLRPC_SV_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_)
#endif

#ifndef CLRPC_SV_TRACE_02
#define CLRPC_SV_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_SV_TRACE_03
#define CLRPC_SV_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_SV_TRACE_04
#define CLRPC_SV_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_SV_TRACE_05
#define CLRPC_SV_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_SV_TRACE_06
#define CLRPC_SV_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_SV_TRACE_07
#define CLRPC_SV_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_SV_TRACE_08
#define CLRPC_SV_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_SV_TRACE_09
#define CLRPC_SV_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_SV_TRACE_10
#define CLRPC_SV_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_SV_TRACE_11
#define CLRPC_SV_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_SV_TRACE_12
#define CLRPC_SV_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CLRPC_SV_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

/*----------------------------------------------------------------------------*/

#ifndef CLRPC_SV_PKT_TRACE_00
#define CLRPC_SV_PKT_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_01
#define CLRPC_SV_PKT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_02
#define CLRPC_SV_PKT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_03
#define CLRPC_SV_PKT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_04
#define CLRPC_SV_PKT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_05
#define CLRPC_SV_PKT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_06
#define CLRPC_SV_PKT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_07
#define CLRPC_SV_PKT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_08
#define CLRPC_SV_PKT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_09
#define CLRPC_SV_PKT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_10
#define CLRPC_SV_PKT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_11
#define CLRPC_SV_PKT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_SV_PKT_TRACE_12
#define CLRPC_SV_PKT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CLRPC_SV_PKT_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CLRPC_EPM_TRACE_00
#define CLRPC_EPM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_)
#endif

#ifndef CLRPC_EPM_TRACE_01
#define CLRPC_EPM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_)
#endif

#ifndef CLRPC_EPM_TRACE_02
#define CLRPC_EPM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_EPM_TRACE_03
#define CLRPC_EPM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_EPM_TRACE_04
#define CLRPC_EPM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_EPM_TRACE_05
#define CLRPC_EPM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_EPM_TRACE_06
#define CLRPC_EPM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_EPM_TRACE_07
#define CLRPC_EPM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_EPM_TRACE_08
#define CLRPC_EPM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_EPM_TRACE_09
#define CLRPC_EPM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_EPM_TRACE_10
#define CLRPC_EPM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_EPM_TRACE_11
#define CLRPC_EPM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_EPM_TRACE_12
#define CLRPC_EPM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CLRPC_EPM_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/

#ifndef CLRPC_LOWER_TRACE_00
#define CLRPC_LOWER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_)
#endif

#ifndef CLRPC_LOWER_TRACE_01
#define CLRPC_LOWER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_)
#endif

#ifndef CLRPC_LOWER_TRACE_02
#define CLRPC_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_)
#endif

#ifndef CLRPC_LOWER_TRACE_03
#define CLRPC_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_)
#endif

#ifndef CLRPC_LOWER_TRACE_04
#define CLRPC_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef CLRPC_LOWER_TRACE_05
#define CLRPC_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef CLRPC_LOWER_TRACE_06
#define CLRPC_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef CLRPC_LOWER_TRACE_07
#define CLRPC_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef CLRPC_LOWER_TRACE_08
#define CLRPC_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef CLRPC_LOWER_TRACE_09
#define CLRPC_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef CLRPC_LOWER_TRACE_10
#define CLRPC_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef CLRPC_LOWER_TRACE_11
#define CLRPC_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef CLRPC_LOWER_TRACE_12
#define CLRPC_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_CLRPC_LOWER_IDX00, idx_ , level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif


/*----------------------------------------------------------------------------*/
#else

#error "CLRPC_CFG_TRACE_MODE has unknown value"

#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_TRC_H */
