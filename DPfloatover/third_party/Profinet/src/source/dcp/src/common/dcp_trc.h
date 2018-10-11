#ifndef DCP_TRC_H                     /* ----- reinclude-protection ----- */
#define DCP_TRC_H

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
/*  C o m p o n e n t     &C: DCP (Discovery & Configuration Protocol)  :C&  */
/*                                                                           */
/*  F i l e               &F: dcp_trc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface for LSA-trace                                           */
/*  Defines LSA-trace macros for DCP.                                        */
/*                                                                           */
/*      Here the macros are adapted to the LSA component LTRC.               */
/*                                                                           */
/*      If LTRC shall not be used, then either the LSA_TRACE macros used     */
/*      here must be defined by the system in the file lsa_cfg.h/txt or      */
/*      directly the DCP-trace macro must be redefined on the desired        */
/*      function/macro in the gsy_cfg.h/txt file.                            */
/*      If the system isn't able to store trace data then an "empty          */
/*      definition" must be implemented in file gsy_cfg.h/txt (or the        */
/*      LSA_TRACE macros must be defined "empty" in lsa_cfg.h/txt).          */
/*                                                                           */
/*****************************************************************************/
#ifdef DCP_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version         Who What                                     */
/*  2005-02-21  P03.04.00.00_00.04.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2010-08-24  P05.00.00.00_00.01.01.02 lrg: AP00999104 DCP_CFG_TRACE_MODE  */
/*  2011-06-30  S05.01.00.00_00.03.03.02 lrg: DCP_FUNCTION_TRACE_00          */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* system interface output macros
 *
 * Note: There aren't any prototypes for these macros so that the target system
 * defines the pointer size and not the DCP. DCP doesn't cast the 
 * parameters at the call.
 * A function can be nevertheless invoked within the macro.
 *
 * The subsytem YYYYY (see below) must be defined by the LSA component 
 * developer. It can be =
 *    [empty]:   If this component uses only one subsystem.
 *    UPPER:     All requirements of the upper user are written into this
 *               subsystem.
 *               Calls of the user are marked with                   >>>
 *               Confirmations to the user are marked with           <<<
 *    LOWER:     All requirements to the lower layer are written into this 
 *               subsystem.
 *               Requests to the lower layer are marked with         >>>
 *               Confirmations from the lower layer are marked with  <<<
 *    MEMORY:    All memory requests for RQBs, data buffer, TA-Buffer, ...
 *               are written into this subsystem. Conversions of UPPER_TO_LOWER
 *               and reversed also belong to this subsystem.
 *               For functions which the system invokes:
 *               Calls of the system are marked with                 -->
 *               The end of the functions are marked with            <--
 *    SYSTEM:    All other requirements to / from the system are written into
 *               this subsystem. For example: Timers, path information, copying
 *               processes of COMMON memory, ....
 *               For functions which the system invokes:
 *               Calls of the system are marked with                 -->
 *               The end of the functions are marked with            <--
 *    PROTOCOL:  The protocol is written down here.
 *    FUNCTION:  (Internal) function call trace.
 *
 *    If the component developer wants to make trace entries of  other things 
 *    than above mentioned, he can define other subsystems.
 *
 * Note: The system integrator must define the following identifiers (if LTRC
 * is used in the file ltrc_sub.h/txt or if LTRC isn't used in a system file):
 *    TRACE_SUBSYS_DCP_YYYYY
 *    TRACE_SUBSYS_DCP_YYYYY_INST
 *    etc.
 */


/*=============================================================================
 * macro names:   DCP_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the 
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier 
 *                TRACE_SUBSYS_DCP_YYYYY (if LTRC is used in the file
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
 *                para16_:  parameter 16
 *
 * return value:  LSA_VOID
 *===========================================================================*/

/*****************************************************************************/
#if DCP_CFG_TRACE_MODE == 1                                     /* LSA trace */
/*****************************************************************************/

#ifndef DCP_UPPER_TRACE_04
#define DCP_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_DCP_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_UPPER_TRACE_03
#define DCP_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_DCP_UPPER, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_UPPER_TRACE_02
#define DCP_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_DCP_UPPER, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_UPPER_TRACE_01
#define DCP_UPPER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_DCP_UPPER, level_, msg_, para1_)
#endif
#ifndef DCP_LOWER_TRACE_04
#define DCP_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_DCP_LOWER, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_LOWER_TRACE_03
#define DCP_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_DCP_LOWER, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_LOWER_TRACE_02
#define DCP_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_DCP_LOWER, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_LOWER_TRACE_01
#define DCP_LOWER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_DCP_LOWER, level_, msg_, para1_)
#endif
#ifndef DCP_SYSTEM_TRACE_04
#define DCP_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_DCP_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_SYSTEM_TRACE_03
#define DCP_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_DCP_SYSTEM, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_SYSTEM_TRACE_02
#define DCP_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_DCP_SYSTEM, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_SYSTEM_TRACE_01
#define DCP_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_DCP_SYSTEM, level_, msg_, para1_)
#endif
#ifndef DCP_ERROR_TRACE_05
#define DCP_ERROR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_DCP_ERROR, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef DCP_ERROR_TRACE_04
#define DCP_ERROR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_DCP_ERROR, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_ERROR_TRACE_03
#define DCP_ERROR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_DCP_ERROR, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_ERROR_TRACE_02
#define DCP_ERROR_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_DCP_ERROR, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_ERROR_TRACE_01
#define DCP_ERROR_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_DCP_ERROR, level_, msg_, para1_)
#endif
#ifndef DCP_ERROR_TRACE_00
#define DCP_ERROR_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_DCP_ERROR, level_, msg_)
#endif
#ifndef DCP_FUNCTION_TRACE_07
#define DCP_FUNCTION_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
		LSA_TRACE_07(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef DCP_FUNCTION_TRACE_06
#define DCP_FUNCTION_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
		LSA_TRACE_06(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef DCP_FUNCTION_TRACE_05
#define DCP_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef DCP_FUNCTION_TRACE_04
#define DCP_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_FUNCTION_TRACE_03
#define DCP_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_FUNCTION_TRACE_02
#define DCP_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_FUNCTION_TRACE_01
#define DCP_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_, para1_)
#endif
#ifndef DCP_FUNCTION_TRACE_00
#define DCP_FUNCTION_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_DCP_FUNCTION, level_, msg_)
#endif
#ifndef DCP_SERVER_TRACE_04
#define DCP_SERVER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_DCP_SERVER, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_SERVER_TRACE_03
#define DCP_SERVER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_DCP_SERVER, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_SERVER_TRACE_02
#define DCP_SERVER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_DCP_SERVER, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_SERVER_TRACE_01
#define DCP_SERVER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_DCP_SERVER, level_, msg_, para1_)
#endif
#ifndef DCP_SRVERR_TRACE_04
#define DCP_SRVERR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_DCP_SRVERR, level_, msg_, para1_, para2_, para3_, para4_)
#endif		   
#ifndef DCP_SRVERR_TRACE_03
#define DCP_SRVERR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_DCP_SRVERR, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_SRVERR_TRACE_02
#define DCP_SRVERR_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_DCP_SRVERR, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_SRVERR_TRACE_01
#define DCP_SRVERR_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_DCP_SRVERR, level_, msg_, para1_)
#endif

/*=============================================================================
 * macro names:   DCP_YYYYY_TRACE_BYTE_ARRAY
 *
 * macro:         Trace entry of a byte array for the subsystem "YYYYY".
 *
 *                The system integrator must define the identifier 
 *                TRACE_SUBSYS_DCP_YYYYY (if LTRC is used in the file
 *                ltrc_sub.h/txt or if LTRC isn't used in a system file)!
 *
 *                Note: DCP_YYYYY_TRACE_BYTE_ARRAY isn't supported by
 *                LSA_LTRC_P01.00.00.00_00.02.02.01 yet.
 *
 * parameters:    level_:   LSA_TRACE_LEVEL_OFF, LSA_TRACE_LEVEL_FATAL,
 *                          LSA_TRACE_LEVEL_ERROR, LSA_TRACE_LEVEL_UNEXP,
 *                          LSA_TRACE_LEVEL_WARN, LSA_TRACE_LEVEL_NOTE_HIGH,
 *                          LSA_TRACE_LEVEL_NOTE, LSA_TRACE_LEVEL_NOTE_LOW or
 *                          LSA_TRACE_LEVEL_CHAT
 *                msg_:     string similar to "printf"
 *                ptr_:     pointer to byte array
 *                len_:     number of bytes of the array; len <= 1024
 *
 * return value:  LSA_VOID
 *===========================================================================*/
#ifndef DCP_SNDRCV_TRACE_BYTE_ARRAY
#define DCP_SNDRCV_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
		LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_DCP_SNDRCV, level_, msg_, ptr_, len_)
#endif
#ifndef DCP_ERROR_TRACE_BYTE_ARRAY
#define DCP_ERROR_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
		LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_DCP_ERROR, level_, msg_, ptr_, len_)
#endif

/*****************************************************************************/
#elif DCP_CFG_TRACE_MODE == 2                           /* LSA indexed trace */
/*****************************************************************************/

#ifndef DCP_UPPER_TRACE_04
#define DCP_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_IDX_TRACE_04(TRACE_SUBSYS_DCP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_UPPER_TRACE_03
#define DCP_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_IDX_TRACE_03(TRACE_SUBSYS_DCP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_UPPER_TRACE_02
#define DCP_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_IDX_TRACE_02(TRACE_SUBSYS_DCP_UPPER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_UPPER_TRACE_01
#define DCP_UPPER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_IDX_TRACE_01(TRACE_SUBSYS_DCP_UPPER_IDX00, idx_, level_, msg_, para1_)
#endif
#ifndef DCP_LOWER_TRACE_04
#define DCP_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_IDX_TRACE_04(TRACE_SUBSYS_DCP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_LOWER_TRACE_03
#define DCP_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_IDX_TRACE_03(TRACE_SUBSYS_DCP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_LOWER_TRACE_02
#define DCP_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_IDX_TRACE_02(TRACE_SUBSYS_DCP_LOWER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_LOWER_TRACE_01
#define DCP_LOWER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_IDX_TRACE_01(TRACE_SUBSYS_DCP_LOWER_IDX00, idx_, level_, msg_, para1_)
#endif
#ifndef DCP_SYSTEM_TRACE_04
#define DCP_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_IDX_TRACE_04(TRACE_SUBSYS_DCP_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_SYSTEM_TRACE_03
#define DCP_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_IDX_TRACE_03(TRACE_SUBSYS_DCP_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_SYSTEM_TRACE_02
#define DCP_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_IDX_TRACE_02(TRACE_SUBSYS_DCP_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_SYSTEM_TRACE_01
#define DCP_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_IDX_TRACE_01(TRACE_SUBSYS_DCP_SYSTEM_IDX00, idx_, level_, msg_, para1_)
#endif
#ifndef DCP_SYSTEM_TRACE_00
#define DCP_SYSTEM_TRACE_00(idx_, level_, msg_)	\
		LSA_IDX_TRACE_00(TRACE_SUBSYS_DCP_SYSTEM_IDX00, idx_, level_, msg_)
#endif
#ifndef DCP_ERROR_TRACE_05
#define DCP_ERROR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_IDX_TRACE_05(TRACE_SUBSYS_DCP_ERROR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef DCP_ERROR_TRACE_04
#define DCP_ERROR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_IDX_TRACE_04(TRACE_SUBSYS_DCP_ERROR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_ERROR_TRACE_03
#define DCP_ERROR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_IDX_TRACE_03(TRACE_SUBSYS_DCP_ERROR_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_ERROR_TRACE_02
#define DCP_ERROR_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_IDX_TRACE_02(TRACE_SUBSYS_DCP_ERROR_IDX00, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_ERROR_TRACE_01
#define DCP_ERROR_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_IDX_TRACE_01(TRACE_SUBSYS_DCP_ERROR_IDX00, idx_, level_, msg_, para1_)
#endif
#ifndef DCP_ERROR_TRACE_00
#define DCP_ERROR_TRACE_00(idx_, level_, msg_)	\
		LSA_IDX_TRACE_00(TRACE_SUBSYS_DCP_ERROR_IDX00, idx_, level_, msg_)
#endif
#ifndef DCP_FUNCTION_TRACE_07
#define DCP_FUNCTION_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
		LSA_IDX_TRACE_07(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef DCP_FUNCTION_TRACE_06
#define DCP_FUNCTION_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
		LSA_IDX_TRACE_06(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef DCP_FUNCTION_TRACE_05
#define DCP_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_IDX_TRACE_05(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef DCP_FUNCTION_TRACE_04
#define DCP_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_IDX_TRACE_04(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_FUNCTION_TRACE_03
#define DCP_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_IDX_TRACE_03(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_FUNCTION_TRACE_02
#define DCP_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_IDX_TRACE_02(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_FUNCTION_TRACE_01
#define DCP_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_IDX_TRACE_01(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_, para1_)
#endif
#ifndef DCP_FUNCTION_TRACE_00
#define DCP_FUNCTION_TRACE_00(idx_, level_, msg_)	\
		LSA_IDX_TRACE_00(TRACE_SUBSYS_DCP_FUNCTION_IDX00, idx_, level_, msg_)
#endif
#ifndef DCP_SERVER_TRACE_04
#define DCP_SERVER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_IDX_TRACE_04(TRACE_SUBSYS_DCP_SERVER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef DCP_SERVER_TRACE_03
#define DCP_SERVER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_IDX_TRACE_03(TRACE_SUBSYS_DCP_SERVER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_SERVER_TRACE_02
#define DCP_SERVER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_IDX_TRACE_02(TRACE_SUBSYS_DCP_SERVER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_SERVER_TRACE_01
#define DCP_SERVER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_IDX_TRACE_01(TRACE_SUBSYS_DCP_SERVER_IDX00, idx_, level_, msg_, para1_)
#endif
#ifndef DCP_SRVERR_TRACE_04
#define DCP_SRVERR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_IDX_TRACE_04(TRACE_SUBSYS_DCP_SRVERR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif		   
#ifndef DCP_SRVERR_TRACE_03
#define DCP_SRVERR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_IDX_TRACE_03(TRACE_SUBSYS_DCP_SRVERR_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef DCP_SRVERR_TRACE_02
#define DCP_SRVERR_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_IDX_TRACE_02(TRACE_SUBSYS_DCP_SRVERR_IDX00, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef DCP_SRVERR_TRACE_01
#define DCP_SRVERR_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_IDX_TRACE_01(TRACE_SUBSYS_DCP_SRVERR_IDX00, idx_, level_, msg_, para1_)
#endif

/*=============================================================================
 * macro names:   DCP_YYYYY_TRACE_BYTE_ARRAY
 *
 * macro:         Trace entry of a byte array for the subsystem "YYYYY".
 *
 *                The system integrator must define the identifier 
 *                TRACE_SUBSYS_DCP_YYYYY (if LTRC is used in the file
 *                ltrc_sub.h/txt or if LTRC isn't used in a system file)!
 *
 *                Note: DCP_YYYYY_TRACE_BYTE_ARRAY isn't supported by
 *                LSA_LTRC_P01.00.00.00_00.02.02.01 yet.
 *
 * parameters:    level_:   LSA_TRACE_LEVEL_OFF, LSA_TRACE_LEVEL_FATAL,
 *                          LSA_TRACE_LEVEL_ERROR, LSA_TRACE_LEVEL_UNEXP,
 *                          LSA_TRACE_LEVEL_WARN, LSA_TRACE_LEVEL_NOTE_HIGH,
 *                          LSA_TRACE_LEVEL_NOTE, LSA_TRACE_LEVEL_NOTE_LOW or
 *                          LSA_TRACE_LEVEL_CHAT
 *                msg_:     string similar to "printf"
 *                ptr_:     pointer to byte array
 *                len_:     number of bytes of the array; len <= 1024
 *
 * return value:  LSA_VOID
 *===========================================================================*/
#ifndef DCP_SNDRCV_TRACE_BYTE_ARRAY
#define DCP_SNDRCV_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
		LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_DCP_SNDRCV_IDX00, idx_, level_, msg_, ptr_, len_)
#endif
#ifndef DCP_ERROR_TRACE_BYTE_ARRAY
#define DCP_ERROR_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
		LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_DCP_ERROR_IDX00, idx_, level_, msg_, ptr_, len_)
#endif

/*****************************************************************************/
#elif DCP_CFG_TRACE_MODE == 0 /* no trace */
/*****************************************************************************/

#define DCP_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define DCP_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define DCP_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define DCP_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define DCP_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define DCP_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define DCP_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define DCP_LOWER_TRACE_01(idx_, level_, msg_, para1_)
#define DCP_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define DCP_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define DCP_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define DCP_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define DCP_ERROR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define DCP_ERROR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define DCP_ERROR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define DCP_ERROR_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define DCP_ERROR_TRACE_01(idx_, level_, msg_, para1_)
#define DCP_ERROR_TRACE_00(idx_, level_, msg_)
#define DCP_FUNCTION_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define DCP_FUNCTION_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define DCP_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define DCP_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define DCP_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define DCP_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define DCP_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)
#define DCP_FUNCTION_TRACE_00(idx_, level_, msg_)
#define DCP_SERVER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define DCP_SERVER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define DCP_SERVER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define DCP_SERVER_TRACE_01(idx_, level_, msg_, para1_)
#define DCP_SRVERR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define DCP_SRVERR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define DCP_SRVERR_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define DCP_SRVERR_TRACE_01(idx_, level_, msg_, para1_)

#define DCP_SNDRCV_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)
#define DCP_ERROR_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)

/*****************************************************************************/
#else
#error "DCP_CFG_TRACE_MODE has unknown value"
#endif

/*****************************************************************************/
/*  end of file dcp_trc.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of DCP_TRC_H */
