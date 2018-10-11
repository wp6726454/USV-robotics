
#ifndef EDDS_TRC_H                       /* ----- reinclude-protection ----- */
#define EDDS_TRC_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_trc.h                                :F&  */
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
/*  Defines lsa-trace macros for EDDS                                        */
/*                                                                           */
/*      Here the macros are adapted to the lsa component LTRC.               */
/*                                                                           */
/*      If LTRC shall not be used, then either the LSA_TRACE macros used     */
/*      here must be defined by the system in the file lsa_cfg.h/txt or      */
/*      directly the EDDS-trace macro must be redefined on the desired       */
/*      function/macro in the edd_cfg.h/txt file.                            */
/*      If the system isn't able to store trace data then an "empty          */
/*      definition" must be implemented in file edd_cfg.h/txt (or the        */
/*      LSA_TRACE macros must be defined "empty" in lsa_cfg.h/txt).          */
/*                                                                           */
/*      See also EDDS_CFG_TRACE_MODE in edd_cfg.h/txt                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  27.01.05    JS    initial version.                                       */
/*  15.06.11    MA    TraceIdx support, new EDDP-like trace.                                      */
#endif
/*****************************************************************************/

/*===========================================================================*/
/* Usage of traces within EDD : (have to be defines by LTRC or System!)      */
/*                                                                           */
/* Trace-Subsystems used                                                     */
/*                                                                           */
/*                                                                           */
/* TRACE_SUBSYS_EDDS_UPPER  Upper interface calls/response (Requests)        */
/* TRACE_SUBSYS_EDDS_LOWER  Lower interface calls/response (Requests)        */
/* TRACE_SUBSYS_EDDS_SYSTEM System interface calls/response                  */
/*                                                                           */
/* TRACE_SUBSYS_EDDS_CORE   Core   function in/out, flow, errors ..          */
/* TRACE_SUBSYS_EDDS_NRT    NRT    function in/out, flow, errors ..          */
/* TRACE_SUBSYS_EDDS_CRT    CRT    function in/out, flow, errors ..          */
/* TRACE_SUBSYS_EDDS_PHY    PHY    function in/out, flow, errors ..          */
/* TRACE_SUBSYS_EDDS_SWI    SWITCH function in/out, flow, errors ..          */
/* TRACE_SUBSYS_EDDS_PRM    PRM    function in/out, flow, errors ..          */
/* TRACE_SUBSYS_EDDS_PRM    LLIF   function in/out, flow, errors ..          */
/*                                                                           */
/* Trace-Level usage:                                                        */
/*                                                                           */
/* LSA_TRACE_LEVEL_FATAL      : Fatal errors       : Fatal error (halt)      */
/* LSA_TRACE_LEVEL_ERROR      : Errors             : flow, error             */
/* LSA_TRACE_LEVEL_UNEXP      : Unexpected errors  : flow, unexp             */
/* LSA_TRACE_LEVEL_WARNING    : Warning            : flow, warning           */
/* LSA_TRACE_LEVEL_NOTE_HIGH  : Higher important   : flow                    */
/* LSA_TRACE_LEVEL_NOTE       : Normal             : flow                    */
/* LSA_TRACE_LEVEL_NOTE_LOW   : Less important     : function in/out, flow   */
/* LSA_TRACE_LEVEL_CHAT       : Very less important: function in/out         */
/*                                                                           */
/* flow = Program flow                                                       */
/*                                                                           */
/*===========================================================================*/

/*===========================================================================*/
/* TRACE-References have to be defined by Systemadaption.                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* system interface output macros
 *
 * Note: There aren't any prototypes for these macros so that the target system
 * defines the pointer size and not the EDDS. EDDS doesn't cast the
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
 * macro names:   EDDS_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_EDDS_YYYYY (if LTRC is used in the file
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

   

// used if pIDB->TraceIdx not present
#define EDDS_UNDEF_TRACE_IDX 0



#if EDDS_CFG_TRACE_MODE == 2

/*****************************************************************************/
/* Standard LSA LTRC (with idx trace).                                       */
/*****************************************************************************/

#ifndef EDDS_UPPER_TRACE_00
#define EDDS_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_UPPER_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_UPPER_TRACE_01
#define EDDS_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_UPPER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_UPPER_TRACE_02
#define EDDS_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_UPPER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_UPPER_TRACE_03
#define EDDS_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_UPPER_TRACE_04
#define EDDS_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_UPPER_TRACE_05
#define EDDS_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_UPPER_TRACE_06
#define EDDS_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_UPPER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_LOWER_TRACE_00
#define EDDS_LOWER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_LOWER_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_LOWER_TRACE_01
#define EDDS_LOWER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_LOWER_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_LOWER_TRACE_02
#define EDDS_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_LOWER_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_LOWER_TRACE_03
#define EDDS_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_LOWER_TRACE_04
#define EDDS_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_LOWER_TRACE_05
#define EDDS_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_LOWER_TRACE_06
#define EDDS_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_LOWER_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif


#ifndef EDDS_SYSTEM_TRACE_00
#define EDDS_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_SYSTEM_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_SYSTEM_TRACE_01
#define EDDS_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_SYSTEM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_SYSTEM_TRACE_02
#define EDDS_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_SYSTEM_TRACE_03
#define EDDS_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_SYSTEM_TRACE_04
#define EDDS_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_SYSTEM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_CORE_TRACE_00
#define EDDS_CORE_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_CORE_TRACE_01
#define EDDS_CORE_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_CORE_TRACE_02
#define EDDS_CORE_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_CORE_TRACE_03
#define EDDS_CORE_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_CORE_TRACE_04
#define EDDS_CORE_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_CORE_TRACE_05
#define EDDS_CORE_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_CORE_TRACE_06
#define EDDS_CORE_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_CORE_TRACE_07
#define EDDS_CORE_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_CORE_TRACE_08
#define EDDS_CORE_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_CORE_TRACE_BYTE_ARRAY
#define EDDS_CORE_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_CORE_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_NRT_TRACE_00
#define EDDS_NRT_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_NRT_TRACE_01
#define EDDS_NRT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_NRT_TRACE_02
#define EDDS_NRT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_NRT_TRACE_03
#define EDDS_NRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_NRT_TRACE_04
#define EDDS_NRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_NRT_TRACE_05
#define EDDS_NRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_NRT_TRACE_06
#define EDDS_NRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_NRT_TRACE_07
#define EDDS_NRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_NRT_TRACE_08
#define EDDS_NRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_NRT_TRACE_BYTE_ARRAY
#define EDDS_NRT_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_NRT_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_CRT_TRACE_00
#define EDDS_CRT_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_CRT_TRACE_01
#define EDDS_CRT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_CRT_TRACE_02
#define EDDS_CRT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_CRT_TRACE_03
#define EDDS_CRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_CRT_TRACE_04
#define EDDS_CRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_CRT_TRACE_05
#define EDDS_CRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_CRT_TRACE_06
#define EDDS_CRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_CRT_TRACE_07
#define EDDS_CRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_CRT_TRACE_08
#define EDDS_CRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_CRT_TRACE_09
#define EDDS_CRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EDDS_CRT_TRACE_10
#define EDDS_CRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EDDS_CRT_TRACE_BYTE_ARRAY
#define EDDS_CRT_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_CRT_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_LL_TRACE_00
#define EDDS_LL_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_LL_TRACE_01
#define EDDS_LL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_LL_TRACE_02
#define EDDS_LL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_LL_TRACE_03
#define EDDS_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_LL_TRACE_04
#define EDDS_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_LL_TRACE_05
#define EDDS_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_LL_TRACE_06
#define EDDS_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_LL_TRACE_07
#define EDDS_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_LL_TRACE_08
#define EDDS_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
  
#ifndef EDDS_LL_TRACE_09
#define EDDS_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EDDS_LL_TRACE_10
#define EDDS_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EDDS_LL_TRACE_11
#define EDDS_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) //(file reference: edds_gen.c - why?) (e1795)
#endif

#ifndef EDDS_LL_TRACE_12
#define EDDS_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

#ifndef EDDS_LL_TRACE_13
#define EDDS_LL_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_) \
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)
#endif

#ifndef EDDS_LL_TRACE_14
#define EDDS_LL_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_) \
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)
#endif

#ifndef EDDS_LL_TRACE_15
#define EDDS_LL_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_) \
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)
#endif  

#ifndef EDDS_LL_TRACE_BYTE_ARRAY
#define EDDS_LL_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_LL_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_SWI_TRACE_00
#define EDDS_SWI_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_SWI_TRACE_01
#define EDDS_SWI_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_SWI_TRACE_02
#define EDDS_SWI_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_SWI_TRACE_03
#define EDDS_SWI_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_SWI_TRACE_04
#define EDDS_SWI_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_SWI_TRACE_05
#define EDDS_SWI_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_SWI_TRACE_06
#define EDDS_SWI_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_SWI_TRACE_07
#define EDDS_SWI_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_SWI_TRACE_08
#define EDDS_SWI_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_SWI_TRACE_BYTE_ARRAY
#define EDDS_SWI_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_SWI_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_PRM_TRACE_00
#define EDDS_PRM_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_PRM_TRACE_01
#define EDDS_PRM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_PRM_TRACE_02
#define EDDS_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_PRM_TRACE_03
#define EDDS_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_PRM_TRACE_04
#define EDDS_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_PRM_TRACE_05
#define EDDS_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_PRM_TRACE_06
#define EDDS_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_PRM_TRACE_07
#define EDDS_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_PRM_TRACE_08
#define EDDS_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_PRM_TRACE_BYTE_ARRAY
#define EDDS_PRM_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_PRM_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_ISR_TRACE_00
#define EDDS_ISR_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_)
#endif

#ifndef EDDS_ISR_TRACE_01
#define EDDS_ISR_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_)
#endif

#ifndef EDDS_ISR_TRACE_02
#define EDDS_ISR_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_ISR_TRACE_03
#define EDDS_ISR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_ISR_TRACE_04
#define EDDS_ISR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_ISR_TRACE_05
#define EDDS_ISR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_ISR_TRACE_06
#define EDDS_ISR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_ISR_TRACE_07
#define EDDS_ISR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_ISR_TRACE_08
#define EDDS_ISR_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_ISR_TRACE_09
#define EDDS_ISR_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EDDS_ISR_TRACE_10
#define EDDS_ISR_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EDDS_ISR_TRACE_BYTE_ARRAY
#define EDDS_ISR_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
    LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_ISR_IDX00, idx_, level_, msg_, para1_, para2_)
#endif


#elif EDDS_CFG_TRACE_MODE == 1

/*****************************************************************************/
/* Standard LSA LTRC. We dont use additional EDDS compile levels..           */
/*****************************************************************************/

#ifndef EDDS_UPPER_TRACE_00
#define EDDS_UPPER_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_UPPER, level_, msg_)
#endif

#ifndef EDDS_UPPER_TRACE_01
#define EDDS_UPPER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_UPPER, level_, msg_, para1_)
#endif

#ifndef EDDS_UPPER_TRACE_02
#define EDDS_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_UPPER, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_UPPER_TRACE_03
#define EDDS_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_UPPER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_UPPER_TRACE_04
#define EDDS_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_UPPER_TRACE_05
#define EDDS_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_UPPER_TRACE_06
#define EDDS_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_UPPER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_LOWER_TRACE_00
#define EDDS_LOWER_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_LOWER, level_, msg_)
#endif

#ifndef EDDS_LOWER_TRACE_01
#define EDDS_LOWER_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_LOWER, level_, msg_, para1_)
#endif

#ifndef EDDS_LOWER_TRACE_02
#define EDDS_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_LOWER, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_LOWER_TRACE_03
#define EDDS_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_LOWER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_LOWER_TRACE_04
#define EDDS_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_LOWER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_LOWER_TRACE_05
#define EDDS_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_LOWER_TRACE_06
#define EDDS_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_LOWER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif


#ifndef EDDS_SYSTEM_TRACE_00
#define EDDS_SYSTEM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_SYSTEM, level_, msg_)
#endif

#ifndef EDDS_SYSTEM_TRACE_01
#define EDDS_SYSTEM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_SYSTEM, level_, msg_, para1_)
#endif

#ifndef EDDS_SYSTEM_TRACE_02
#define EDDS_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_SYSTEM, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_SYSTEM_TRACE_03
#define EDDS_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_SYSTEM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_SYSTEM_TRACE_04
#define EDDS_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_CORE_TRACE_00
#define EDDS_CORE_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_CORE, level_, msg_)
#endif

#ifndef EDDS_CORE_TRACE_01
#define EDDS_CORE_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_)
#endif

#ifndef EDDS_CORE_TRACE_02
#define EDDS_CORE_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_CORE_TRACE_03
#define EDDS_CORE_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_CORE_TRACE_04
#define EDDS_CORE_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_CORE_TRACE_05
#define EDDS_CORE_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_CORE_TRACE_06
#define EDDS_CORE_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_CORE_TRACE_07
#define EDDS_CORE_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_CORE_TRACE_08
#define EDDS_CORE_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_CORE_TRACE_BYTE_ARRAY
#define EDDS_CORE_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_CORE, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_NRT_TRACE_00
#define EDDS_NRT_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_NRT, level_, msg_)
#endif

#ifndef EDDS_NRT_TRACE_01
#define EDDS_NRT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_)
#endif

#ifndef EDDS_NRT_TRACE_02
#define EDDS_NRT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_NRT_TRACE_03
#define EDDS_NRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_NRT_TRACE_04
#define EDDS_NRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_NRT_TRACE_05
#define EDDS_NRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_NRT_TRACE_06
#define EDDS_NRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_NRT_TRACE_07
#define EDDS_NRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_NRT_TRACE_08
#define EDDS_NRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_NRT_TRACE_BYTE_ARRAY
#define EDDS_NRT_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_NRT, level_, msg_, para1_, para2_)
#endif


#ifndef EDDS_CRT_TRACE_00
#define EDDS_CRT_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_CRT, level_, msg_)
#endif

#ifndef EDDS_CRT_TRACE_01
#define EDDS_CRT_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_)
#endif

#ifndef EDDS_CRT_TRACE_02
#define EDDS_CRT_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_CRT_TRACE_03
#define EDDS_CRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_CRT_TRACE_04
#define EDDS_CRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_CRT_TRACE_05
#define EDDS_CRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_CRT_TRACE_06
#define EDDS_CRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_CRT_TRACE_07
#define EDDS_CRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_CRT_TRACE_08
#define EDDS_CRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_CRT_TRACE_09
#define EDDS_CRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EDDS_CRT_TRACE_10
#define EDDS_CRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EDDS_CRT_TRACE_BYTE_ARRAY
#define EDDS_CRT_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_CRT, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_LL_TRACE_00
#define EDDS_LL_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_LL, level_, msg_)
#endif

#ifndef EDDS_LL_TRACE_01
#define EDDS_LL_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_)
#endif

#ifndef EDDS_LL_TRACE_02
#define EDDS_LL_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_LL_TRACE_03
#define EDDS_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_LL_TRACE_04
#define EDDS_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_LL_TRACE_05
#define EDDS_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_LL_TRACE_06
#define EDDS_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_LL_TRACE_07
#define EDDS_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_LL_TRACE_08
#define EDDS_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
  
#ifndef EDDS_LL_TRACE_09
#define EDDS_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EDDS_LL_TRACE_10
#define EDDS_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EDDS_LL_TRACE_11
#define EDDS_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
    LSA_TRACE_11(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef EDDS_LL_TRACE_12
#define EDDS_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_) \
    LSA_TRACE_12(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#endif

#ifndef EDDS_LL_TRACE_13
#define EDDS_LL_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_) \
    LSA_TRACE_13(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)
#endif

#ifndef EDDS_LL_TRACE_14
#define EDDS_LL_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_) \
    LSA_TRACE_14(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)
#endif

#ifndef EDDS_LL_TRACE_15
#define EDDS_LL_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_) \
    LSA_TRACE_15(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)
#endif  

#ifndef EDDS_LL_TRACE_BYTE_ARRAY
#define EDDS_LL_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_LL, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_SWI_TRACE_00
#define EDDS_SWI_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_SWI, level_, msg_)
#endif

#ifndef EDDS_SWI_TRACE_01
#define EDDS_SWI_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_)
#endif

#ifndef EDDS_SWI_TRACE_02
#define EDDS_SWI_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_SWI_TRACE_03
#define EDDS_SWI_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_SWI_TRACE_04
#define EDDS_SWI_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_SWI_TRACE_05
#define EDDS_SWI_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_SWI_TRACE_06
#define EDDS_SWI_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_SWI_TRACE_07
#define EDDS_SWI_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_SWI_TRACE_08
#define EDDS_SWI_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_SWI_TRACE_BYTE_ARRAY
#define EDDS_SWI_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_SWI, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_PRM_TRACE_00
#define EDDS_PRM_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_PRM, level_, msg_)
#endif

#ifndef EDDS_PRM_TRACE_01
#define EDDS_PRM_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_)
#endif

#ifndef EDDS_PRM_TRACE_02
#define EDDS_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_PRM_TRACE_03
#define EDDS_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_PRM_TRACE_04
#define EDDS_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_PRM_TRACE_05
#define EDDS_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_PRM_TRACE_06
#define EDDS_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_PRM_TRACE_07
#define EDDS_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_PRM_TRACE_08
#define EDDS_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_PRM_TRACE_BYTE_ARRAY
#define EDDS_PRM_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
	LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_PRM, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_ISR_TRACE_00
#define EDDS_ISR_TRACE_00(idx_, level_, msg_) \
    LSA_TRACE_00(TRACE_SUBSYS_EDDS_ISR, level_, msg_)
#endif

#ifndef EDDS_ISR_TRACE_01
#define EDDS_ISR_TRACE_01(idx_, level_, msg_, para1_) \
    LSA_TRACE_01(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_)
#endif

#ifndef EDDS_ISR_TRACE_02
#define EDDS_ISR_TRACE_02(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_02(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_)
#endif

#ifndef EDDS_ISR_TRACE_03
#define EDDS_ISR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_TRACE_03(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef EDDS_ISR_TRACE_04
#define EDDS_ISR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_TRACE_04(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef EDDS_ISR_TRACE_05
#define EDDS_ISR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_) \
    LSA_TRACE_05(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef EDDS_ISR_TRACE_06
#define EDDS_ISR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_) \
    LSA_TRACE_06(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef EDDS_ISR_TRACE_07
#define EDDS_ISR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_) \
    LSA_TRACE_07(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef EDDS_ISR_TRACE_08
#define EDDS_ISR_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_) \
    LSA_TRACE_08(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif

#ifndef EDDS_ISR_TRACE_09
#define EDDS_ISR_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_) \
    LSA_TRACE_09(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef EDDS_ISR_TRACE_10
#define EDDS_ISR_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_) \
    LSA_TRACE_10(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef EDDS_ISR_TRACE_BYTE_ARRAY
#define EDDS_ISR_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_) \
    LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EDDS_ISR, level_, msg_, para1_, para2_)
#endif

#elif EDDS_CFG_TRACE_MODE == 0

/*****************************************************************************/
/* No  Trace                                                                 */
/*****************************************************************************/

#define EDDS_UPPER_TRACE_00(idx_, level_, msg_)
#define EDDS_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)

#define EDDS_LOWER_TRACE_00(idx_, level_, msg_)
#define EDDS_LOWER_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)

#define EDDS_SYSTEM_TRACE_00(idx_, level_, msg_)
#define EDDS_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)

#define EDDS_CORE_TRACE_00(idx_, level_, msg_)
#define EDDS_CORE_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_CORE_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_CORE_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_CORE_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_CORE_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_CORE_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EDDS_CORE_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EDDS_CORE_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EDDS_CORE_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_)

#define EDDS_NRT_TRACE_00(idx_, level_, msg_)
#define EDDS_NRT_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_NRT_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_NRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_NRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_NRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_NRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EDDS_NRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EDDS_NRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EDDS_NRT_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_)

#define EDDS_CRT_TRACE_00(idx_, level_, msg_)
#define EDDS_CRT_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_CRT_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_CRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_CRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_CRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_CRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EDDS_CRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EDDS_CRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EDDS_CRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define EDDS_CRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define EDDS_CRT_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_)

#define EDDS_LL_TRACE_00(idx_, level_, msg_)
#define EDDS_LL_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_LL_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_LL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_LL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_LL_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_LL_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EDDS_LL_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EDDS_LL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EDDS_LL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define EDDS_LL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define EDDS_LL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#define EDDS_LL_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#define EDDS_LL_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)
#define EDDS_LL_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)
#define EDDS_LL_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)
#define EDDS_LL_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_)

#define EDDS_SWI_TRACE_00(idx_, level_, msg_)
#define EDDS_SWI_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_SWI_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_SWI_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_SWI_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_SWI_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_SWI_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EDDS_SWI_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EDDS_SWI_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EDDS_SWI_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_)

#define EDDS_PRM_TRACE_00(idx_, level_, msg_)
#define EDDS_PRM_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EDDS_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EDDS_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EDDS_PRM_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_)

#define EDDS_ISR_TRACE_00(idx_, level_, msg_)
#define EDDS_ISR_TRACE_01(idx_, level_, msg_, para1_)
#define EDDS_ISR_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define EDDS_ISR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define EDDS_ISR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define EDDS_ISR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define EDDS_ISR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define EDDS_ISR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define EDDS_ISR_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define EDDS_ISR_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define EDDS_ISR_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define EDDS_ISR_TRACE_BYTE_ARRAY(idx_, level_, msg_, para1_, para2_)

#else
#error "EDDS_CFG_TRACE_MODE not supported"
#endif



/*****************************************************************************/
/*  end of file edds_trc.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_TRC_H */
