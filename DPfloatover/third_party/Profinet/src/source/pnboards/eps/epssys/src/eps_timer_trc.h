#ifndef EPS_TIMER_TRC_H                   /* ----- reinclude-protection ----- */
#define EPS_TIMER_TRC_H

/*****************************************************************************/
/*  Copyright (C) 2015ENS AG, 2010.                    All rights reserved.  */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: EPS_TIMER                          :P&          */
/*                                                                           */
/*  P a c k a g e         &W: EPS_TIMER                          :W&          */
/*                                                                           */
/*  C o m p o n e n t     &C: EPS_TIMER                          :C&          */
/*                                                                           */
/*  F i l e               &F: EPS_timer_trc.h                    :F&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*  For every component using ltrc you must create such a file.              */
/*  Each component has at least one subsystem.                               */
/*                                                                           */
/*  Here: component EPS_TIMER with its subsystems                             */
/*****************************************************************************/

/**********************************************************************
 * Alternatives for selection of trace systems                        *
 *********************************************************************/
#define EPS_TIMER_TRC_LTRC       1      ///< LTRC is selected
#define EPS_TIMER_TRC_PTRC       2      ///< PTRC is selected
#define EPS_TIMER_TRC_NO_TRCSYS  3      ///< No trace system is selected

/**********************************************************************
 * Use / don't use                                                    *
 *********************************************************************/
#define NO                    1      ///< Switch is turned off
#define YES                   2      ///< Switch is turned on


#define EPS_TIMER_SYS_TRACE_USAGE NO
#define EPS_TIMER_TRC_SYS EPS_TIMER_TRC_NO_TRCSYS

#if EPS_TIMER_TRC_SYS == EPS_TIMER_TRC_LTRC
  #include <ltrc_if.h>
#endif


/*============================================================================*/
/*          defines, if traces aren't activated in EPS_TIMER                   */
/*============================================================================*/
#if EPS_TIMER_SYS_TRACE_USAGE == NO

/*----------------------------------------------------------------------------*/
/*------------------ EPS_TIMER tracepoint macros ------------------------------*/
/*----------------------------------------------------------------------------*/
  #define EPSTIMER_TRACE_00(level, msg)
  #define EPSTIMER_TRACE_01(level, msg, p1)
  #define EPSTIMER_TRACE_02(level, msg, p1, p2)
  #define EPSTIMER_TRACE_03(level, msg, p1, p2, p3)
  #define EPSTIMER_TRACE_04(level, msg, p1, p2, p3, p4)
  #define EPSTIMER_TRACE_05(level, msg, p1, p2, p3, p4, p5)
  #define EPSTIMER_TRACE_06(level, msg, p1, p2, p3, p4, p5, p6)
  #define EPSTIMER_TRACE_07(level, msg, p1, p2, p3, p4, p5, p6, p7)
  #define EPSTIMER_TRACE_08(level, msg, p1, p2, p3, p4, p5, p6, p7, p8)
  #define EPSTIMER_TRACE_09(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9)
  #define EPSTIMER_TRACE_10(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)
  #define EPSTIMER_TRACE_11(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)
  #define EPSTIMER_TRACE_12(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)
  #define EPSTIMER_TRACE_13(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)
  #define EPSTIMER_TRACE_14(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)
  #define EPSTIMER_TRACE_15(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15)
  #define EPSTIMER_TRACE_16(level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16)

/*----------------------------------------------------------------------------*/
/*------------------ General EPS_TIMER tracelevel macros ----------------------*/
/*----------------------------------------------------------------------------*/
  #define EPSTIMER_TRACE_LEVEL_OFF             0
  #define EPSTIMER_TRACE_LEVEL_FATAL           1
  #define EPSTIMER_TRACE_LEVEL_ERROR           2
  #define EPSTIMER_TRACE_LEVEL_UNEXP           3
  #define EPSTIMER_TRACE_LEVEL_WARN            4
  #define EPSTIMER_TRACE_LEVEL_NOTE_HIGH       5
  #define EPSTIMER_TRACE_LEVEL_NOTE            6
  #define EPSTIMER_TRACE_LEVEL_NOTE_LOW        7
  #define EPSTIMER_TRACE_LEVEL_CHAT            8

/*----------------------------------------------------------------------------*/
/*------------------ EPS_TIMER subsystem macros -------------------------------*/
/*----------------------------------------------------------------------------*/
//ToDo by lln: check whether this define is necessary
//  #define TRACE_SUBSYS_EPSTIMER

#endif  //EPS_TIMER_SYS_TRACE_USAGE == NO

/*============================================================================*/
/*          Redefined EPS_TIMER trace marcos to LSA trace macros               */
/*============================================================================*/
#if EPS_TIMER_SYS_TRACE_USAGE == YES && EPS_TIMER_TRC_SYS == EPS_TIMER_TRC_LTRC

/*==============================================================================
 * trace level macros
 *============================================================================*/
#define EPSTIMER_TRACE_LEVEL_OFF         LSA_TRACE_LEVEL_OFF
                                      /* This trace entry is never created!   */
#define EPSTIMER_TRACE_LEVEL_FATAL       LSA_TRACE_LEVEL_FATAL
                                      /* Serious not tradable error occurred! */
#define EPSTIMER_TRACE_LEVEL_ERROR       LSA_TRACE_LEVEL_ERROR
                                      /* Serious tradable error occurred!     */
#define EPSTIMER_TRACE_LEVEL_UNEXP       LSA_TRACE_LEVEL_UNEXP
                                      /* Unexpected situation occurred!       */
#define EPSTIMER_TRACE_LEVEL_WARN        LSA_TRACE_LEVEL_WARN
                                      /* Warning: Program exits productive    */
                                      /* branch!                              */
#define EPSTIMER_TRACE_LEVEL_NOTE_HIGH   LSA_TRACE_LEVEL_NOTE_HIGH
                                      /* Important information productive     */
                                      /* case: High level!                    */
#define EPSTIMER_TRACE_LEVEL_NOTE        LSA_TRACE_LEVEL_NOTE
                                      /* Important information productive     */
                                      /* case: Normal level!                  */
#define EPSTIMER_TRACE_LEVEL_NOTE_LOW    LSA_TRACE_LEVEL_NOTE_LOW
                                      /* Important information productive     */
                                      /* case: Low level!                     */
#define EPSTIMER_TRACE_LEVEL_CHAT        LSA_TRACE_LEVEL_CHAT
                                      /* Important code places within the     */
                                      /* software!                            */

/*------------------ EPS_TIMER ------------------------------------------------*/
#ifndef EPSTIMER_TRACE_00
    #define EPSTIMER_TRACE_00(level, msg) \
               LSA_TRACE_00(TRACE_SUBSYS_EPSTIMER, level, msg)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_01
    #define EPSTIMER_TRACE_01(level, msg, p1) \
               LSA_TRACE_01(TRACE_SUBSYS_EPSTIMER, level, msg, p1)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_02
    #define EPSTIMER_TRACE_02(level, msg, p1,p2) \
               LSA_TRACE_02(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_03
    #define EPSTIMER_TRACE_03(level, msg, p1,p2,p3) \
               LSA_TRACE_03(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_04
    #define EPSTIMER_TRACE_04(level, msg, p1,p2,p3,p4) \
               LSA_TRACE_04(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_05
    #define EPSTIMER_TRACE_05(level, msg, p1,p2,p3,p4,p5) \
               LSA_TRACE_05(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_06
    #define EPSTIMER_TRACE_06(level, msg, p1,p2,p3,p4,p5,p6) \
               LSA_TRACE_06(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_07
    #define EPSTIMER_TRACE_07(level, msg, p1,p2,p3,p4,p5,p6,p7) \
               LSA_TRACE_07(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_08
    #define EPSTIMER_TRACE_08(level, msg, p1,p2,p3,p4,p5,p6,p7,p8) \
               LSA_TRACE_08(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_09
    #define EPSTIMER_TRACE_09(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9) \
               LSA_TRACE_09(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_10
    #define EPSTIMER_TRACE_10(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) \
               LSA_TRACE_10(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_11
    #define EPSTIMER_TRACE_11(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) \
               LSA_TRACE_11(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_12
    #define EPSTIMER_TRACE_12(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12) \
               LSA_TRACE_12(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_13
    #define EPSTIMER_TRACE_13(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13) \
               LSA_TRACE_13(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_14
    #define EPSTIMER_TRACE_14(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14) \
               LSA_TRACE_14(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_15
    #define EPSTIMER_TRACE_15(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) \
               LSA_TRACE_15(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_16
    #define EPSTIMER_TRACE_16(level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16) \
               LSA_TRACE_16(TRACE_SUBSYS_EPSTIMER, level, msg, p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16)
#endif
/*----------------------------------------------------------------------------*/
#ifndef EPSTIMER_TRACE_BYTE_ARRAY
    #define EPSTIMER_TRACE_BYTE_ARRAY(level, msg, ptr, len) \
               LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_EPSTIMER, level, msg, ptr, len)
#endif

#endif  //EPS_TIMER_SYS_TRACE_USAGE == YES && EPS_TIMER_TRC_SYS == EPS_TIMER_TRC_LTRC

/******************************************************************************/
/*  end of file EPS_TIMER_TRC.H                                                */
/******************************************************************************/
#endif  /* of EPS_TIMER_TRC_H */

