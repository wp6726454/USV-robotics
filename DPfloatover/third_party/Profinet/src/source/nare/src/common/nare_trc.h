
#ifndef NARE_TRC_H                       /* ----- reinclude-protection ----- */
#define NARE_TRC_H

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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_trc.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface for lsa-trace                                           */
/*  Defines lsa-trace macros for NARE.                                       */
/*                                                                           */
/*      Here the macros are adapted to the lsa component LTRC.               */
/*                                                                           */
/*      If LTRC shall not be used, then either the LSA_TRACE macros used     */
/*      here must be defined by the system in the file lsa_cfg.h/txt or      */
/*      directly the NARE-trace macro must be redefined on the desired       */
/*      function/macro in the nare_cfg.h/txt file.                           */
/*      If the system isn't able to store trace data then an "empty          */
/*      definition" must be implemented in file nare_cfg.h/txt (or the       */
/*      LSA_TRACE macros must be defined "empty" in lsa_cfg.h/txt).          */
/*                                                                           */
/*      See also NARE_CFG_TRACE_MODE in nare_cfg.h/txt                       */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/* Usage of traces within NARE: (have to be defines by LTRC or System!)      */
/*                                                                           */
/* Trace-Subsystems used                                                     */
/*                                                                           */
/* TRACE_SUBSYS_NARE_UPPER    :    Upper-Interface calls/response            */
/* TRACE_SUBSYS_NARE_LOWER    :    Lower-Interface calls/response            */
/* TRACE_SUBSYS_NARE_SYSTEM   :    System-Interface calls/response           */
/* TRACE_SUBSYS_NARE_FUNCTION :    Function In/Out-Calls                     */
/* TRACE_SUBSYS_NARE_PROGRAM  :    Programm flow and errors                  */
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


/*===========================================================================*/
/* TRACE-Referenzes have to be defined by Systemadaption.                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* system interface output macros
 *
 * Note: There aren't any prototypes for these macros so that the target system
 * defines the pointer size and not the NARE. NARE doesn't cast the
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
 */


/*=============================================================================
 * macro names:   NARE_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_NARE_YYYYY (if LTRC is used in the file
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
#if NARE_CFG_TRACE_MODE == 0 /* no trace */

#define NARE_UPPER_TRACE_00(idx_, level_, msg_) 
#define NARE_UPPER_TRACE_01(idx_, level_, msg_, para1_) 
#define NARE_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) 
#define NARE_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) 
#define NARE_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) 

#define NARE_LOWER_TRACE_00(idx_, level_, msg_) 
#define NARE_LOWER_TRACE_01(idx_, level_, msg_, para1_) 
#define NARE_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_) 
#define NARE_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) 
#define NARE_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) 

#define NARE_SYSTEM_TRACE_00(idx_, level_, msg_) 
#define NARE_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) 
#define NARE_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) 
#define NARE_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) 
#define NARE_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) 

#define NARE_FUNCTION_TRACE_00(idx_, level_, msg_) 
#define NARE_FUNCTION_TRACE_01(idx_, level_, msg_, para1_) 
#define NARE_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_) 
#define NARE_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) 
#define NARE_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) 

#define NARE_PROGRAM_TRACE_00(idx_, level_, msg_) 
#define NARE_PROGRAM_TRACE_01(idx_, level_, msg_, para1_) 
#define NARE_PROGRAM_TRACE_02(idx_, level_, msg_, para1_, para2_) 
#define NARE_PROGRAM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) 
#define NARE_PROGRAM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) 
#define NARE_PROGRAM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) 
#define NARE_PROGRAM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) 
#define NARE_PROGRAM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) 

#elif NARE_CFG_TRACE_MODE == 1

#ifndef NARE_UPPER_TRACE_00
#define NARE_UPPER_TRACE_00(idx_, level_, msg_) \
LSA_TRACE_00(TRACE_SUBSYS_NARE_UPPER, level_, msg_)
#endif

#ifndef NARE_UPPER_TRACE_01
#define NARE_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
LSA_TRACE_01(TRACE_SUBSYS_NARE_UPPER, level_, msg_, para1_)
#endif

#ifndef NARE_UPPER_TRACE_02
#define NARE_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_TRACE_02(TRACE_SUBSYS_NARE_UPPER, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_UPPER_TRACE_03
#define NARE_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_TRACE_03(TRACE_SUBSYS_NARE_UPPER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_UPPER_TRACE_04
#define NARE_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
LSA_TRACE_04(TRACE_SUBSYS_NARE_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef NARE_LOWER_TRACE_00
#define NARE_LOWER_TRACE_00(idx_, level_, msg_) \
LSA_TRACE_00(TRACE_SUBSYS_NARE_LOWER, level_, msg_)
#endif

#ifndef NARE_LOWER_TRACE_01
#define NARE_LOWER_TRACE_01(idx_, level_, msg_, para1_) \
LSA_TRACE_01(TRACE_SUBSYS_NARE_LOWER, level_, msg_, para1_)
#endif

#ifndef NARE_LOWER_TRACE_02
#define NARE_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_TRACE_02(TRACE_SUBSYS_NARE_LOWER, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_LOWER_TRACE_03
#define NARE_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_TRACE_03(TRACE_SUBSYS_NARE_LOWER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_SYSTEM_TRACE_00
#define NARE_SYSTEM_TRACE_00(idx_, level_, msg_) \
LSA_TRACE_00(TRACE_SUBSYS_NARE_SYSTEM, level_, msg_)
#endif

#ifndef NARE_SYSTEM_TRACE_01
#define NARE_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
LSA_TRACE_01(TRACE_SUBSYS_NARE_SYSTEM, level_, msg_, para1_)
#endif

#ifndef NARE_SYSTEM_TRACE_02
#define NARE_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_TRACE_02(TRACE_SUBSYS_NARE_SYSTEM, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_SYSTEM_TRACE_03
#define NARE_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_TRACE_03(TRACE_SUBSYS_NARE_SYSTEM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_FUNCTION_TRACE_00
#define NARE_FUNCTION_TRACE_00(idx_, level_, msg_) \
LSA_TRACE_00(TRACE_SUBSYS_NARE_FUNCTION, level_, msg_)
#endif

#ifndef NARE_FUNCTION_TRACE_01
#define NARE_FUNCTION_TRACE_01(idx_, level_, msg_, para1_) \
LSA_TRACE_01(TRACE_SUBSYS_NARE_FUNCTION, level_, msg_, para1_)
#endif

#ifndef NARE_FUNCTION_TRACE_02
#define NARE_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_TRACE_02(TRACE_SUBSYS_NARE_FUNCTION, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_FUNCTION_TRACE_03
#define NARE_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_TRACE_03(TRACE_SUBSYS_NARE_FUNCTION, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_PROGRAM_TRACE_00
#define NARE_PROGRAM_TRACE_00(idx_, level_, msg_) \
LSA_TRACE_00(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_)
#endif

#ifndef NARE_PROGRAM_TRACE_01
#define NARE_PROGRAM_TRACE_01(idx_, level_, msg_, para1_) \
LSA_TRACE_01(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_, para1_)
#endif

#ifndef NARE_PROGRAM_TRACE_02
#define NARE_PROGRAM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_TRACE_02(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_PROGRAM_TRACE_03
#define NARE_PROGRAM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_TRACE_03(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_PROGRAM_TRACE_04
#define NARE_PROGRAM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
LSA_TRACE_04(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef NARE_PROGRAM_TRACE_05
#define NARE_PROGRAM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
LSA_TRACE_05(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef NARE_PROGRAM_TRACE_06
#define NARE_PROGRAM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
LSA_TRACE_06(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef NARE_PROGRAM_TRACE_08
#define NARE_PROGRAM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
LSA_TRACE_08(TRACE_SUBSYS_NARE_PROGRAM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
/* end: NARE_CFG_TRACE_MODE == 1 */

#elif NARE_CFG_TRACE_MODE == 2

#ifndef NARE_UPPER_TRACE_00
#define NARE_UPPER_TRACE_00(idx_, level_, msg_) \
LSA_IDX_TRACE_00(TRACE_SUBSYS_NARE_UPPER_IDX00, idx_,level_, msg_)
#endif

#ifndef NARE_UPPER_TRACE_01
#define NARE_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
LSA_IDX_TRACE_01(TRACE_SUBSYS_NARE_UPPER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef NARE_UPPER_TRACE_02
#define NARE_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_IDX_TRACE_02(TRACE_SUBSYS_NARE_UPPER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_UPPER_TRACE_03
#define NARE_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_IDX_TRACE_03(TRACE_SUBSYS_NARE_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_UPPER_TRACE_04
#define NARE_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
LSA_IDX_TRACE_04(TRACE_SUBSYS_NARE_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef NARE_LOWER_TRACE_00
#define NARE_LOWER_TRACE_00(idx_, level_, msg_) \
LSA_IDX_TRACE_00(TRACE_SUBSYS_NARE_LOWER_IDX00, idx_, level_, msg_)
#endif

#ifndef NARE_LOWER_TRACE_01
#define NARE_LOWER_TRACE_01(idx_, level_, msg_, para1_) \
LSA_IDX_TRACE_01(TRACE_SUBSYS_NARE_LOWER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef NARE_LOWER_TRACE_02
#define NARE_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_IDX_TRACE_02(TRACE_SUBSYS_NARE_LOWER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_LOWER_TRACE_03
#define NARE_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_IDX_TRACE_03(TRACE_SUBSYS_NARE_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_SYSTEM_TRACE_00
#define NARE_SYSTEM_TRACE_00(idx_, level_, msg_) \
LSA_IDX_TRACE_00(TRACE_SUBSYS_NARE_SYSTEM_IDX00, idx_, level_, msg_)
#endif

#ifndef NARE_SYSTEM_TRACE_01
#define NARE_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
LSA_IDX_TRACE_01(TRACE_SUBSYS_NARE_SYSTEM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef NARE_SYSTEM_TRACE_02
#define NARE_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_IDX_TRACE_02(TRACE_SUBSYS_NARE_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_SYSTEM_TRACE_03
#define NARE_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_IDX_TRACE_03(TRACE_SUBSYS_NARE_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_FUNCTION_TRACE_00
#define NARE_FUNCTION_TRACE_00(idx_, level_, msg_) \
LSA_IDX_TRACE_00(TRACE_SUBSYS_NARE_FUNCTION_IDX00, idx_, level_, msg_)
#endif

#ifndef NARE_FUNCTION_TRACE_01
#define NARE_FUNCTION_TRACE_01(idx_, level_, msg_, para1_) \
LSA_IDX_TRACE_01(TRACE_SUBSYS_NARE_FUNCTION_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef NARE_FUNCTION_TRACE_02
#define NARE_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_IDX_TRACE_02(TRACE_SUBSYS_NARE_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_FUNCTION_TRACE_03
#define NARE_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_IDX_TRACE_03(TRACE_SUBSYS_NARE_FUNCTION_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_PROGRAM_TRACE_00
#define NARE_PROGRAM_TRACE_00(idx_, level_, msg_) \
LSA_IDX_TRACE_00(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_)
#endif

#ifndef NARE_PROGRAM_TRACE_01
#define NARE_PROGRAM_TRACE_01(idx_, level_, msg_, para1_) \
LSA_IDX_TRACE_01(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef NARE_PROGRAM_TRACE_02
#define NARE_PROGRAM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
LSA_IDX_TRACE_02(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef NARE_PROGRAM_TRACE_03
#define NARE_PROGRAM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
LSA_IDX_TRACE_03(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef NARE_PROGRAM_TRACE_04
#define NARE_PROGRAM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
LSA_IDX_TRACE_04(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef NARE_PROGRAM_TRACE_05
#define NARE_PROGRAM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
LSA_IDX_TRACE_05(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef NARE_PROGRAM_TRACE_06
#define NARE_PROGRAM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
LSA_IDX_TRACE_06(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef NARE_PROGRAM_TRACE_08
#define NARE_PROGRAM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
LSA_IDX_TRACE_08(TRACE_SUBSYS_NARE_PROGRAM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

/* if NARE_CFG_TRACE_MODE == 2 */
#endif 

/*****************************************************************************/
/*  end of file nare_trc.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_TRC_H */
