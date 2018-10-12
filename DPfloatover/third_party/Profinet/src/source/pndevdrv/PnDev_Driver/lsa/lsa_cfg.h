
#ifndef LSA_CFG_H /* ----- reinclude-protection ----- */
#define LSA_CFG_H

#ifdef __cplusplus /* If C++ - compiler: Use C linkage */
extern "C" {
#endif

/*****************************************************************************/
/*  Copyright (C) 2000 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: Layer Stack Architecture (LSA)    :P&          */
/*                                                                           */
/*  P a c k a g e         &W: LSA-Global                        :W&          */
/*                                                                           */
/*  C o m p o n e n t     &C: Global Definitions                :C&          */
/*                                                                           */
/*  F i l e               &F: lsa_cfg.h/txt                     :F&          */
/*                                                                           */
/*  V e r s i o n         &V: LSA_GLOB_P02.01.00.00_00.11.02.01 :V&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2008-12-02                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                        Frame for file "lsa_cfg.h".                        */
/*                        ===========================                        */
/*                                                                           */
/*  Configuration for LSA:                                                   */
/*  Defines constants, types and macros for LSA.                             */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*  2001-07-26  P02.00.01.001  ws   Basis-Typ LSA_INT neu definiert.         */
/*  2001-07-26  P02.00.01.001  ws   Einige Kommentare ergaenzt.              */
/*  2001-07-26  P02.00.01.001  ws   *_USER_ID_TYPE von lsa_cfg.h nach        */
/*                                  lsa_sys.h verlagert.                     */
/*  2001-07-26  P02.00.01.001  ws   VOID-Cast bei LSA_NULL weggelassen.      */
/*  2001-07-27  P02.00.01.001  ws   __LSA_CFG_H geaendert nach LSA_CFG_H.    */
/*  2001-07-27  P02.00.01.001  ws   LSA_HOST_PTR_COMPARE_TYPE neu definiert. */
/*  2001-07-27  P02.00.01.001  ws   LSA_VOID_PTR_TYPE neu definiert.         */
/*  2001-07-30  P02.00.01.001  ws   Schutz vor erneutem Includieren an den   */
/*                                  Dateianfang verlagert.                   */
/*  2001-09-27  P02.00.01.001  ws   lsa_cfg.h umbenannt nach lsa_cfg.txt     */
/*  2001-10-08  P02.00.01.001  ws   Kommentare ergaenzt.                     */
/*  2001-12-04  P02.00.01.001  ws   Weil user-id eine union ist ergaenzt:    */
/*                                  Attribut LSA_COMMON_MEM_ATTR,            */
/*                                  Konstante LSA_USER_ID_UINT8_UNUSED,      */
/*                                  Makros LSA_USER_ID_INIT() und            */
/*                                  LSA_USER_ID_ARE_EQUAL().                 */
/*  2002-01-29  P02.00.01.002  ws   Kommentare ergaenzt.                     */
/*  2002-01-29  P02.00.01.002  ws   Lokale Parameter in Makros mit "_"       */
/*                                  gekennzeichnet.                          */
/*  2002-01-30  P02.00.01.002  ws   LSA_USER_ID_ARE_EQUAL geaendert: Rueck-  */
/*                                  gabewert per Zeiger zurueckgeben.        */
/*  2002-01-30  P02.00.01.002  ws   Makro LSA_USER_ID_HANDLE() neu.          */
/*  2002-05-17  P2.0.0.0_0.1.4.1 ws LSA_USER_ID_HANDLE() geaendert: Auf die  */
/*                                  union nicht mehr per Pointer zugreifen,  */
/*                                  sondern ueber den Strukturoperator.      */
/*  2002-07-26  V2.0.0.0_0.1.6.1 ws ARTS 51557: LSA_HOST_PTR_ARE_EQUAL() von */
/*                                  lsa_sys.h nach lsa_cfg.h verlagert.      */
/*  2002-10-28  P2.1.0.0_0.1.1.1 ws ARTS 141840: LSA_CONST_MEM_ATTR neu.     */
/*  2002-10-28  P2.1.0.0_0.1.1.1 ws ARTS 141708: LSA_UINT neu.               */
/*  2003-06-05  P2.1.0.0_0.2.1.1 ws ARTS+ AP00035493: Compiler-Warnings      */
/*                                  verhindern: Makro LSA_UNUSED_ARG neu.    */
/*  2003-11-24  P2.1.0.0_0.3.1.1 ws Trace neu: Konstanten LSA_TRACE_LEVEL_*  */
/*                                  und Makros LSA_(INST_)TRACE_* neu.       */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00132068: Fuer C++-Compiler      */
/*                                  Datei mit C-Bindung gekennzeichnet;      */
/*                                  ausserdem LSA_EXTERN angepasst.          */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00167458: Copyright abgeaendert. */
/*  2004-11-30  P2.1.0.0_0.4.2.1 ws Wegen Tasking-Compiler im Kommentar      */
/*                                  einige Umlaute durch andere Zeichen      */
/*                                  ersetzt.                                 */
/*  2005-01-04  P2.1.0.0_0.4.3.1 ws ARTS+ AP00197761: timer_id:              */
/*                                  LSA_TIMER_ID_TYPE neu.                   */
/*  2005-07-22  P2.1.0.0_0.5.1.1 ws Trace: Neu: Pseudo-Makros als Scanner-   */
/*                                  Kennung fuer die symbolische Anzeige von */
/*                                  Konstanten hier fuer den Compiler als    */
/*                                  Leer-Makros definiert.                   */
/*  2006-03-27  P2.1.0.0_0.7.1.1 ws ARTS+ AP00200674: Endianess: Compiler-   */
/*                                  schalter LSA_HOST_ENDIANESS_BIG und      */
/*                                  LSA_HOST_ENDIANESS_LITTLE neu.           */
/*  2006-03-28  P2.1.0.0_0.7.1.1 ws ARTS+ AP00268519: LSA_RQB_HEADER-        */
/*                                  Definition neu: RQB-Header-Definition    */
/*                                  von prefix_cfg.txt und l4_cfg.txt nach   */
/*                                  lsa_cfg.txt verlagert.                   */
/*  2006-04-05  P2.1.0.0_0.7.1.1 ws ARTS+ AP00277026: 64 Bit-Datentypen neu. */
/*  2007-03-22  P2.1.0.0_0.8.1.1 ws ARTS+ AP00424201: Variable priority im   */
/*                                  LSA_RQB_HEADER neu.                      */
/*  2007-07-05  P2.1.0.0_0.9.1.1 ws Namensumbenennung: Instanziierter Trace  */
/*                                  -> Indizierter Trace: Neue Makros        */
/*                                  definiert und alte Makros auf die neuen  */
/*                                  Makros abgebildet. Kommentare angepasst. */
/*  2007-07-05  P2.1.0.0_0.9.1.1 ws Wegen Trace-Scanner-Problem bei der      */
/*                                  Definition von LSA_RQB_HEADER einige     */
/*                                  Backslashes geloescht.                   */
/*  2007-08-08 P2.1.0.0_0.10.1.1 ws ARTS+ AP00498913: LSA_CHAR neu:          */
/*                                  Kommentar ergaenzt.                      */
/*  2007-08-08 P2.1.0.0_0.10.1.1 ws ARTS+ AP00501849: LSA_VOID_PTR_TYPE      */
/*                                  ergaenzt um LSA_COMMON_MEM_ATTR.         */
/*  2008-01-25 P2.1.0.0_0.11.1.1 ws ARTS+ AP00565054: Wegen 64 Bit-Plattform */
/*                                  Konstante LSA_RQB_ERROR_CODE_SIZE        */
/*                                  angepasst: Hier Makros LSA_MIN() und     */
/*                                  LSA_MAX() neu.                           */
/*  2008-12-01 P2.1.0.0_0.11.2.1 ws Im Kommentar Schreibfehler korrigiert.   */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                         compiler-switches for lsa                         */
/*===========================================================================*/

/*=============================================================================
 *       Endianess:
 * If you have a computer system whitch stors most significant byte at the
 * lowest address of the word/doubleword: Define this
#define LSA_HOST_ENDIANESS_BIG
 * else define this
#define LSA_HOST_ENDIANESS_LITTLE
 *===========================================================================*/

/*===========================================================================*/
/*                          data-attribute for lsa                           */
/*===========================================================================*/

//#define LSA_COMMON_MEM_ATTR   <attribute>  /* greatest common data attribute
//*/
//#define LSA_CONST_MEM_ATTR    <attribute>  /* const data attribute */

#define LSA_COMMON_MEM_ATTR /* greatest common data attribute */
#define LSA_CONST_MEM_ATTR  /* const data attribute */

/*===========================================================================*/
/* basic types for lsa according ANSI C1999 Standard (ISO/IEC 9899:1999)     */
/*                                                                           */
/* ANSI C1999: "The header <stdint.h> declares sets of integer types having  */
/* specified widths."                                                        */
/* For example: uint8_t, uint16_t, uint32_t, int8_t, int16_t, int32_t        */
/*                                                                           */
/* See also file lsa_sys.h (definition of LSA_CHAR).                         */
/*===========================================================================*/

#define LSA_UINT8 uint8_t /* or unsigned char      */
/* UNSIGNED-8-Bit:                   0 ...                        +255 */
#define LSA_UINT16 uint16_t /* or unsigned short     */
/* UNSIGNED-16-Bit:                  0 ...                     +65 535 */
#define LSA_UINT32 uint32_t /* or unsigned long      */
/* UNSIGNED-32-Bit:                  0 ...              +4 294 967 295 */
#define LSA_UINT64 uint64_t /* or unsigned long long */
                            /* or unsigned __int64   */
/* UNSIGNED-64-Bit:                  0 ... +18 446 744 073 709 551 615 */

#define LSA_UINT unsigned int
/* 16-, 32-, etc. bit according to compiler                            */
/* minimal range:                    0 ...                     +65 535 */

#define LSA_INT8 int8_t /* or signed char        */
/* SIGNED-8-Bit:            -128 ... 0 ...                        +127 */
#define LSA_INT16 int16_t /* or signed short       */
/* SIGNED-16-Bit:        -32 768 ... 0 ...                     +32 767 */
#define LSA_INT32 int32_t /* or signed long        */
/* SIGNED-32-Bit: -2 147 483 648 ... 0 ...              +2 147 483 647 */
#define LSA_INT64 int64_t /* or signed long long   */
                          /* or signed __int64     */
/* SIGNED-64-Bit:                                                      */
/*    -9 223 372 036 854 775 808 ... 0 ...  +9 223 372 036 854 775 807 */

#define LSA_INT int
/* 16-, 32-, etc. bit according to compiler                            */
/* minimal range:        -32 768 ... 0 ...                     +32 767 */

/*===========================================================================*/
/*                            other types for lsa                            */
/*===========================================================================*/
#define LSA_VOID void
#define LSA_VOID_PTR_TYPE void LSA_COMMON_MEM_ATTR *
#define LSA_BOOL LSA_UINT8 /* LSA_FALSE, LSA_TRUE */

/*=====  types for system integration  =====*/
/*=============================================================================
 *       system-path for the channels
 * examples:
#define LSA_SYS_PATH_TYPE           LSA_UINT8
#define LSA_SYS_PATH_TYPE           LSA_UINT16
#define LSA_SYS_PATH_TYPE           LSA_UINT32
#define LSA_SYS_PATH_TYPE           LSA_VOID_PTR_TYPE
 *===========================================================================*/
#define LSA_SYS_PATH_TYPE LSA_UINT8

/*=============================================================================
 *      system-pointer for the channels
 * examples:
#define LSA_SYS_PTR_TYPE            LSA_UINT16
#define LSA_SYS_PTR_TYPE            LSA_UINT32
#define LSA_SYS_PTR_TYPE            LSA_VOID_PTR_TYPE
 *===========================================================================*/
#define LSA_SYS_PTR_TYPE LSA_VOID_PTR_TYPE

/*=============================================================================
 *      lsa common request block type
 *===========================================================================*/

/*=====  definition of parameter types  =====*/
#define LSA_OPCODE_TYPE LSA_UINT8 /* for opcode */
#define LSA_HANDLE_TYPE LSA_UINT8 /* for channel-handle */
/* Use only the types LSA_UINT8, LSA_UINT16 and        */
/* LSA_UINT32, otherwise it can't be used as an index. */
#define LSA_COMP_ID_TYPE LSA_UINT16 /* for lsa-component-id, */
                                    /* see lsa_sys.h */
/* Hint: LSA_RESPONSE_TYPE, LSA_USER_ID_TYPE and LSA_PRIORITY_TYPE
 *       definitions see lsa_sys.h
 */

/*====  Definition of LSA-global RQB-HEADER:  =====*/
/* The order of the parameters can be changed!
 * All parameters are mandatory!
 * Do not change names of the types and names of variables!
 * The length of the LSA-RQB-HEADER must be 2/4-byte-aligned according to
 * the system! If necessary, dummy-bytes must be added!
 * If necessary during system integration, additional parameters can be added!
 * For example:
    LSA_COMP_ID_TYPE                comp_id;
 */
#define LSA_RQB_HEADER(QUEUE_PTR_TYPE_)   \
  QUEUE_PTR_TYPE_ next_rqb_ptr;           \
  /* pointer to next RQB */               \
  QUEUE_PTR_TYPE_ prev_rqb_ptr;           \
  /* pointer to previous RQB */           \
  LSA_OPCODE_TYPE opcode;                 \
  /* req: opcode, see *_usr.h */          \
  /* cnf: unchanged           */          \
  LSA_HANDLE_TYPE handle;                 \
  /* req: if *_OPC_OPEN_CHANNEL: -     */ \
  /*      else: channel-handle of      */ \
  /*            component              */ \
  /* cnf:       channel-handle of user */ \
  LSA_RESPONSE_TYPE response;             \
  /* req: -                     */        \
  /* cnf: response, see *_usr.h */        \
  LSA_USER_ID_TYPE user_id;               \
  /* req: id of user */                   \
  /* cnf: unchanged  */                   \
  LSA_PRIORITY_TYPE priority;
/* Priority set by a component or    */
/* a user of a component,            */
/* evaluated by the system           */
/* integration or another component, */
/* see lsa_sys.h */

/*====  Definition of LSA-RQB-TRAILER:  =====*/
/* If necessary during system integration, additional parameters can be
 * added!
 */
#define LSA_RQB_TRAILER
/* nothing by default */

/*=============================================================================
 *      for pointer-compare, see LSA_HOST_PTR_ARE_EQUAL()
 * Type must correspond to the memory need of a pointer on the respective
 * target system.
 * examples:
#define LSA_HOST_PTR_COMPARE_TYPE   LSA_UINT8
#define LSA_HOST_PTR_COMPARE_TYPE   LSA_UINT16
#define LSA_HOST_PTR_COMPARE_TYPE   LSA_UINT32
 *===========================================================================*/
#define LSA_HOST_PTR_COMPARE_TYPE LSA_UINT32

/*=============================================================================
 *      timer_id type
 * See parameter timer_id at *_ALLOC_TIMER, *_START_TIMER, *_STOP_TIMER,
 * *_FREE_TIMER and *_timeout().
 * examples:
#define LSA_TIMER_ID_TYPE   LSA_UINT16
#define LSA_TIMER_ID_TYPE   LSA_UINT32
 *===========================================================================*/
#define LSA_TIMER_ID_TYPE LSA_UINT16

/*===========================================================================*/
/*                         basic constants for lsa                           */
/*===========================================================================*/

#define LSA_FALSE ((LSA_BOOL)0)
/* for variables of type LSA_BOOL: must be 0! */
#define LSA_TRUE (!LSA_FALSE)
/* for variables of type LSA_BOOL: must be (!FALSE)! */
#define LSA_NULL 0
#define LSA_OK 1 /* only allowed: 0 or 1 */

#ifdef __cplusplus /* If C++ - compiler: Use C linkage */
#define LSA_EXTERN extern "C"
#else
#define LSA_EXTERN extern
#endif

#define LSA_USER_ID_UINT8_UNUSED 0xff

/*=============================================================================
 *      LSA-trace: trace levels
 * If the LSA component LTRC is used for trace in the target system, nothing
 * needs here to be changed. If the LSA component LTRC isn't used for trace in
 * the target system, then the LSA trace levels can be redefined here.
 *===========================================================================*/
#define LSA_TRACE_LEVEL_OFF LTRC_TRACE_LEVEL_OFF
/* This trace entry is never created! */
#define LSA_TRACE_LEVEL_FATAL LTRC_TRACE_LEVEL_FATAL
/* Serious not tradable error         */
/* occurred!                          */
#define LSA_TRACE_LEVEL_ERROR LTRC_TRACE_LEVEL_ERROR
/* Serious tradable error occurred!   */
#define LSA_TRACE_LEVEL_UNEXP LTRC_TRACE_LEVEL_UNEXP
/* Unexpected situation occurred!     */
#define LSA_TRACE_LEVEL_WARN LTRC_TRACE_LEVEL_WARN
/* Warning: Program exits productive  */
/* branch!                            */
#define LSA_TRACE_LEVEL_NOTE_HIGH LTRC_TRACE_LEVEL_NOTE_HIGH
/* Important information productive   */
/* case: High level!                  */
#define LSA_TRACE_LEVEL_NOTE LTRC_TRACE_LEVEL_NOTE
/* Important information productive   */
/* case: Normal level!                */
#define LSA_TRACE_LEVEL_NOTE_LOW LTRC_TRACE_LEVEL_NOTE_LOW
/* Important information productive   */
/* case: Low level!                   */
#define LSA_TRACE_LEVEL_CHAT LTRC_TRACE_LEVEL_CHAT
/* Important code places within the   */
/* software!                          */

/*===========================================================================*/
/*                           basic macros for lsa                            */
/*===========================================================================*/

/*=============================================================================
 *       function pointer for system integration
 * examples:
 *
 * for BSO-Tasking-compiler:
#define LSA_FCT_PTR(code_attr_, fct_ptr_) \
    code_attr_ (* fct_ptr_)
 *
 * or for Visual C++-compiler:
#define LSA_FCT_PTR(code_attr_, fct_ptr_) \
    (* fct_ptr_)
 *
 * or for other compilers:
#define LSA_FCT_PTR(code_attr_, fct_ptr_) \
    (code_attr_ * fct_ptr_)
 *===========================================================================*/
#define LSA_FCT_PTR(code_attr_, fct_ptr_) (code_attr_ * fct_ptr_)

/*====  for user_id:  =====*/

/*=============================================================================
 *       writes a handle into the user-id and reads a handle from the user-id
 *
 * for    #define LSA_HANDLE_TYPE  LSA_UINT8:
#define LSA_USER_ID_HANDLE(lsa_user_id_) \
    ( (lsa_user_id_).uvar8 )
 *
 * or for #define LSA_HANDLE_TYPE  LSA_UINT16:
#define LSA_USER_ID_HANDLE(lsa_user_id_) \
    ( (lsa_user_id_).uvar16 )
 *
 * or for #define LSA_HANDLE_TYPE  LSA_UINT32:
#define LSA_USER_ID_HANDLE(lsa_user_id_) \
    ( (lsa_user_id_).uvar32 )
 *===========================================================================*/
#define LSA_USER_ID_HANDLE(lsa_user_id_) ((lsa_user_id_).uvar8)

/* initialize user-id as unused
 */
#define LSA_USER_ID_INIT(lsa_user_id_ptr_)                      \
  {                                                             \
    LSA_INT i_;                                                 \
    LSA_UINT8 LSA_COMMON_MEM_ATTR *ptr_;                        \
                                                                \
    ptr_ = (LSA_UINT8 LSA_COMMON_MEM_ATTR *)(lsa_user_id_ptr_); \
                                                                \
    for (i_ = sizeof(LSA_USER_ID_TYPE); i_ > 0; i_--) {         \
      *ptr_++ = LSA_USER_ID_UINT8_UNUSED;                       \
    }                                                           \
  }

/* compare two user-ids
 */
#define LSA_USER_ID_ARE_EQUAL(ret_val_ptr_, lsa_user_id_ptr1_,    \
                              lsa_user_id_ptr2_)                  \
  {                                                               \
    LSA_INT i_;                                                   \
    LSA_UINT8 LSA_COMMON_MEM_ATTR *ptr1_;                         \
    LSA_UINT8 LSA_COMMON_MEM_ATTR *ptr2_;                         \
                                                                  \
    ptr1_ = (LSA_UINT8 LSA_COMMON_MEM_ATTR *)(lsa_user_id_ptr1_); \
    ptr2_ = (LSA_UINT8 LSA_COMMON_MEM_ATTR *)(lsa_user_id_ptr2_); \
    *(ret_val_ptr_) = LSA_TRUE;                                   \
                                                                  \
    for (i_ = sizeof(LSA_USER_ID_TYPE); i_ > 0; i_--) {           \
      if (*ptr1_++ != *ptr2_++) {                                 \
        *(ret_val_ptr_) = LSA_FALSE;                              \
        break;                                                    \
      }                                                           \
    }                                                             \
  }

/*====  for pointer compare:  =====*/

/* So that pointer comparisons are executed correctly also at the segmented
 * memory model (this means page and offset of the pointers are compared with
 * each other), all LSA-components use the following macro.
 * The macro usually doesn't have to be changed. It must be adjusted in certain
 * cases only at the BSO-Tasking/80C166-compiler.
 */
#define LSA_HOST_PTR_ARE_EQUAL(ptr1_, ptr2_) \
  ((LSA_HOST_PTR_COMPARE_TYPE)(ptr1_) == (LSA_HOST_PTR_COMPARE_TYPE)(ptr2_))

/*====  for unused function parameters:  =====*/

/* If in a LSA component a input parameter of a function is unused, it comes to
 * a compiler warning. That may happen, because of compiler switches in that
 * function. The following macro invoked in that function prevents the compiler
 * warning, if the macro uses the input parameter formally.
 */
#define LSA_UNUSED_ARG(arg_)

/*====  for comparison:  =====*/
#define LSA_MIN(x_, y_) (((x_) <= (y_)) ? (x_) : (y_))
#define LSA_MAX(x_, y_) (((x_) >= (y_)) ? (x_) : (y_))

/*====  for LSA-trace:  =====*/

/*=============================================================================
 * If the LSA component LTRC is used for trace in the target system, nothing
 * needs here to be changed. If the LSA component LTRC isn't used for trace in
 * the target system, then the LSA trace macros can be redefined here.
 *
 * Note: There aren't any prototypes for these macros so that the target system
 * defines the pointer size and not LSA.
 * A function can be nevertheless invoked within the macro.
 * Attention: The LSA components mustn't cast the parameters at the call.
 *
 * macro names:   LSA_TRACE_XX           (XX = 00, 01, 02, 03, ..., 16)
 *                LSA_IDX_TRACE_XX       (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters
 *
 * macro names:   LSA_TRACE_BYTE_ARRAY
 *                LSA_IDX_TRACE_BYTE_ARRAY
 *
 * macros:        Trace entry of a byte array
 *
 * parameters:    subsys_:  subsystem of a component: TRACE_SUBSYS_PREFIX_*
 *                idx_:     index
 *                level_:   LSA_TRACE_LEVEL_OFF, LSA_TRACE_LEVEL_FATAL,
 *                          LSA_TRACE_LEVEL_ERROR, LSA_TRACE_LEVEL_UNEXP,
 *                          LSA_TRACE_LEVEL_WARN, LSA_TRACE_LEVEL_NOTE_HIGH,
 *                          LSA_TRACE_LEVEL_NOTE, LSA_TRACE_LEVEL_NOTE_LOW
 *                          or LSA_TRACE_LEVEL_CHAT
 *                msg_:     string similar to "printf"
 *                para1_:   parameter 1
 *                para2_:   parameter 2
 *                 ...        ...
 *                para16_:  parameter 16
 *                ptr_:     pointer to byte array
 *                len_:     number of bytes of the array
 *
 * return value:  LSA_VOID
 *===========================================================================*/

#define LSA_TRACE_00(subsys_, level_, msg_) LTRC_TRACE_00(subsys_, level_, msg_)
#define LSA_TRACE_01(subsys_, level_, msg_, para1_) \
  LTRC_TRACE_01(subsys_, level_, msg_, para1_)
#define LSA_TRACE_02(subsys_, level_, msg_, para1_, para2_) \
  LTRC_TRACE_02(subsys_, level_, msg_, para1_, para2_)
#define LSA_TRACE_03(subsys_, level_, msg_, para1_, para2_, para3_) \
  LTRC_TRACE_03(subsys_, level_, msg_, para1_, para2_, para3_)
#define LSA_TRACE_04(subsys_, level_, msg_, para1_, para2_, para3_, para4_) \
  LTRC_TRACE_04(subsys_, level_, msg_, para1_, para2_, para3_, para4_)
#define LSA_TRACE_05(subsys_, level_, msg_, para1_, para2_, para3_, para4_, \
                     para5_)                                                \
  LTRC_TRACE_05(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define LSA_TRACE_06(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_)                                           \
  LTRC_TRACE_06(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_)
#define LSA_TRACE_07(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_)                                   \
  LTRC_TRACE_07(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_)
#define LSA_TRACE_08(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_)                           \
  LTRC_TRACE_08(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_)
#define LSA_TRACE_09(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_)                   \
  LTRC_TRACE_09(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_)
#define LSA_TRACE_10(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_, para10_)          \
  LTRC_TRACE_10(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_, para10_)
#define LSA_TRACE_11(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_, para10_, para11_) \
  LTRC_TRACE_11(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_, para10_, para11_)
#define LSA_TRACE_12(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_, para10_, para11_, \
                     para12_)                                                  \
  LTRC_TRACE_12(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_, para10_, para11_, para12_)
#define LSA_TRACE_13(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_, para10_, para11_, \
                     para12_, para13_)                                         \
  LTRC_TRACE_13(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_, para10_, para11_, para12_,     \
                para13_)
#define LSA_TRACE_14(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_, para10_, para11_, \
                     para12_, para13_, para14_)                                \
  LTRC_TRACE_14(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_, para10_, para11_, para12_,     \
                para13_, para14_)
#define LSA_TRACE_15(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_, para10_, para11_, \
                     para12_, para13_, para14_, para15_)                       \
  LTRC_TRACE_15(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_, para10_, para11_, para12_,     \
                para13_, para14_, para15_)
#define LSA_TRACE_16(subsys_, level_, msg_, para1_, para2_, para3_, para4_,    \
                     para5_, para6_, para7_, para8_, para9_, para10_, para11_, \
                     para12_, para13_, para14_, para15_, para16_)              \
  LTRC_TRACE_16(subsys_, level_, msg_, para1_, para2_, para3_, para4_, para5_, \
                para6_, para7_, para8_, para9_, para10_, para11_, para12_,     \
                para13_, para14_, para15_, para16_)

#define LSA_TRACE_BYTE_ARRAY(subsys_, level_, msg_, ptr_, len_) \
  LTRC_TRACE_BYTE_ARRAY(subsys_, level_, msg_, ptr_, len_)

#define LSA_IDX_TRACE_00(subsys_, idx_, level_, msg_) \
  LTRC_INST_TRACE_00(subsys_, idx_, level_, msg_)
#define LSA_IDX_TRACE_01(subsys_, idx_, level_, msg_, para1_) \
  LTRC_INST_TRACE_01(subsys_, idx_, level_, msg_, para1_)
#define LSA_IDX_TRACE_02(subsys_, idx_, level_, msg_, para1_, para2_) \
  LTRC_INST_TRACE_02(subsys_, idx_, level_, msg_, para1_, para2_)
#define LSA_IDX_TRACE_03(subsys_, idx_, level_, msg_, para1_, para2_, para3_) \
  LTRC_INST_TRACE_03(subsys_, idx_, level_, msg_, para1_, para2_, para3_)
#define LSA_IDX_TRACE_04(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_)                                              \
  LTRC_INST_TRACE_04(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_)
#define LSA_IDX_TRACE_05(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_)                                      \
  LTRC_INST_TRACE_05(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_)
#define LSA_IDX_TRACE_06(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_)                              \
  LTRC_INST_TRACE_06(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_)
#define LSA_IDX_TRACE_07(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_)                      \
  LTRC_INST_TRACE_07(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_)
#define LSA_IDX_TRACE_08(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_, para8_)              \
  LTRC_INST_TRACE_08(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_, para8_)
#define LSA_IDX_TRACE_09(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_, para8_, para9_)      \
  LTRC_INST_TRACE_09(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_, para8_, para9_)
#define LSA_IDX_TRACE_10(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_, para8_, para9_,      \
                         para10_)                                             \
  LTRC_INST_TRACE_10(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define LSA_IDX_TRACE_11(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_, para8_, para9_,      \
                         para10_, para11_)                                    \
  LTRC_INST_TRACE_11(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_, para8_, para9_, para10_, \
                     para11_)
#define LSA_IDX_TRACE_12(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_, para8_, para9_,      \
                         para10_, para11_, para12_)                           \
  LTRC_INST_TRACE_12(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_, para8_, para9_, para10_, \
                     para11_, para12_)
#define LSA_IDX_TRACE_13(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_, para8_, para9_,      \
                         para10_, para11_, para12_, para13_)                  \
  LTRC_INST_TRACE_13(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_, para8_, para9_, para10_, \
                     para11_, para12_, para13_)
#define LSA_IDX_TRACE_14(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                         para4_, para5_, para6_, para7_, para8_, para9_,      \
                         para10_, para11_, para12_, para13_, para14_)         \
  LTRC_INST_TRACE_14(subsys_, idx_, level_, msg_, para1_, para2_, para3_,     \
                     para4_, para5_, para6_, para7_, para8_, para9_, para10_, \
                     para11_, para12_, para13_, para14_)
#define LSA_IDX_TRACE_15(subsys_, idx_, level_, msg_, para1_, para2_, para3_,  \
                         para4_, para5_, para6_, para7_, para8_, para9_,       \
                         para10_, para11_, para12_, para13_, para14_, para15_) \
  LTRC_INST_TRACE_15(subsys_, idx_, level_, msg_, para1_, para2_, para3_,      \
                     para4_, para5_, para6_, para7_, para8_, para9_, para10_,  \
                     para11_, para12_, para13_, para14_, para15_)
#define LSA_IDX_TRACE_16(subsys_, idx_, level_, msg_, para1_, para2_, para3_,  \
                         para4_, para5_, para6_, para7_, para8_, para9_,       \
                         para10_, para11_, para12_, para13_, para14_, para15_, \
                         para16_)                                              \
  LTRC_INST_TRACE_16(subsys_, idx_, level_, msg_, para1_, para2_, para3_,      \
                     para4_, para5_, para6_, para7_, para8_, para9_, para10_,  \
                     para11_, para12_, para13_, para14_, para15_, para16_)

#define LSA_IDX_TRACE_BYTE_ARRAY(subsys_, idx_, level_, msg_, ptr_, len_) \
  LTRC_INST_TRACE_BYTE_ARRAY(subsys_, idx_, level_, msg_, ptr_, len_)

/*=============================================================================
 * Old macros. Some LSA components still use these macros perhaps.
 *===========================================================================*/
#define LSA_INST_TRACE_00(subsys_, idx_, level_, msg_) \
  LSA_IDX_TRACE_00(subsys_, idx_, level_, msg_)
#define LSA_INST_TRACE_01(subsys_, idx_, level_, msg_, para1_) \
  LSA_IDX_TRACE_01(subsys_, idx_, level_, msg_, para1_)
#define LSA_INST_TRACE_02(subsys_, idx_, level_, msg_, para1_, para2_) \
  LSA_IDX_TRACE_02(subsys_, idx_, level_, msg_, para1_, para2_)
#define LSA_INST_TRACE_03(subsys_, idx_, level_, msg_, para1_, para2_, para3_) \
  LSA_IDX_TRACE_03(subsys_, idx_, level_, msg_, para1_, para2_, para3_)
#define LSA_INST_TRACE_04(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_)                                              \
  LSA_IDX_TRACE_04(subsys_, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define LSA_INST_TRACE_05(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_)                                      \
  LSA_IDX_TRACE_05(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_)
#define LSA_INST_TRACE_06(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_)                              \
  LSA_IDX_TRACE_06(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_)
#define LSA_INST_TRACE_07(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_)                      \
  LSA_IDX_TRACE_07(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_)
#define LSA_INST_TRACE_08(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_)              \
  LSA_IDX_TRACE_08(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_)
#define LSA_INST_TRACE_09(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_)      \
  LSA_IDX_TRACE_09(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_)
#define LSA_INST_TRACE_10(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_,      \
                          para10_)                                             \
  LSA_IDX_TRACE_10(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define LSA_INST_TRACE_11(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_,      \
                          para10_, para11_)                                    \
  LSA_IDX_TRACE_11(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_, para10_,    \
                   para11_)
#define LSA_INST_TRACE_12(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_,      \
                          para10_, para11_, para12_)                           \
  LSA_IDX_TRACE_12(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_, para10_,    \
                   para11_, para12_)
#define LSA_INST_TRACE_13(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_,      \
                          para10_, para11_, para12_, para13_)                  \
  LSA_IDX_TRACE_13(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_, para10_,    \
                   para11_, para12_, para13_)
#define LSA_INST_TRACE_14(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_,      \
                          para10_, para11_, para12_, para13_, para14_)         \
  LSA_IDX_TRACE_14(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_, para10_,    \
                   para11_, para12_, para13_, para14_)
#define LSA_INST_TRACE_15(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_,      \
                          para10_, para11_, para12_, para13_, para14_,         \
                          para15_)                                             \
  LSA_IDX_TRACE_15(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_, para10_,    \
                   para11_, para12_, para13_, para14_, para15_)
#define LSA_INST_TRACE_16(subsys_, idx_, level_, msg_, para1_, para2_, para3_, \
                          para4_, para5_, para6_, para7_, para8_, para9_,      \
                          para10_, para11_, para12_, para13_, para14_,         \
                          para15_, para16_)                                    \
  LSA_IDX_TRACE_16(subsys_, idx_, level_, msg_, para1_, para2_, para3_,        \
                   para4_, para5_, para6_, para7_, para8_, para9_, para10_,    \
                   para11_, para12_, para13_, para14_, para15_, para16_)

#define LSA_INST_TRACE_BYTE_ARRAY(subsys_, idx_, level_, msg_, ptr_, len_) \
  LSA_IDX_TRACE_BYTE_ARRAY(subsys_, idx_, level_, msg_, ptr_, len_)

/*****************************************************************************/
/*  The following part doesn't belong to LSA. But the following pseudo trace */
/*  macros must be once defined in a system, because otherwise LSA software  */
/*  can't be compiled. The component LTRC (or the LTRC successor component   */
/*  PTRC = platform trace) will contain these declarations in future.        */
/*****************************************************************************/

/*=============================================================================
 * First, two macros that can be used to define regions of code that
 * are invisible for the trace scanner.  Their foremost application is
 * to hide the definitions of all pseudo macros defined in this file.
 *===========================================================================*/
#define PTRC_IGNORE_OPEN()
#define PTRC_IGNORE_CLOSE()

/*===========================================================================*/
/* Definition of pseudo macros for trace definitions */
/*===========================================================================*/
PTRC_IGNORE_OPEN()

/* Defines a descriptive string for a symbolic constant */
#define PTRC_EXPLAIN(p)

/* Two macros to parenthesize lists of symbolic constants */
#define PTRC_SUBLABEL_LIST_OPEN(p)
#define PTRC_SUBLABEL_LIST_CLOSE(p)

/* Connect symbolic constants with its user data */
#define PTRC_META_01(s1)
#define PTRC_META_02(s1, s2)
#define PTRC_META_03(s1, s2, s3)
#define PTRC_META_04(s1, s2, s3, s4)
#define PTRC_META_05(s1, s2, s3, s4, s5)
#define PTRC_META_06(s1, s2, s3, s4, s5, s6)
#define PTRC_META_07(s1, s2, s3, s4, s5, s6, s7)
#define PTRC_META_08(s1, s2, s3, s4, s5, s6, s7, s8)
#define PTRC_META_09(s1, s2, s3, s4, s5, s6, s7, s8, s9)
#define PTRC_META_10(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10)
#define PTRC_META_11(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11)
#define PTRC_META_12(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12)
#define PTRC_META_13(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13)
#define PTRC_META_14(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, \
                     s14)
#define PTRC_META_15(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, \
                     s14, s15)
#define PTRC_META_16(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, \
                     s14, s15, s16)

PTRC_IGNORE_CLOSE()
/*****************************************************************************/
/*  end of pseudo trace makro definitions.                                   */
/*****************************************************************************/

/*****************************************************************************/
/*  end of file LSA_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif /* of LSA_CFG_H */
