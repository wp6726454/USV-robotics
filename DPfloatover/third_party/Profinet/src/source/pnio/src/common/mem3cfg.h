#ifndef MEM3CFG_H
#define MEM3CFG_H
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
/*  C o m p o n e n t     &C: PNIO                                      :C&  */
/*                                                                           */
/*  F i l e               &F: mem3cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration of MEM3 memory management                                  */
/*                                                                           */
/*****************************************************************************/

/* This file is reimported by 'mem3if.h' in the users modules. Therefor it
   should compile with C and C++ except the user knows about firmware already
   beeing C++.
   Some source lines only used for memory3 and not for reimport may be encapsulated
   with '#ifdef __cpluplus' clause.
*/


/* Activating is only usefull for debugging memory3 itself.
   That is not the user trace. For that reason see 'Mem3_Trace...' below.*/

#define MEM3TRACE
#define MEM3DEBUG

/* ------ adaption to PNIO Base components ----------- */
#include "lsa_cfg.h"
#include "lsa_sys.h"

/* Note: for MEM3 headerfiles only */
#define EXTERN LSA_EXTERN

LSA_EXTERN LSA_VOID   MEM3_ENTER(  LSA_VOID);
LSA_EXTERN LSA_VOID   MEM3_EXIT(   LSA_VOID);
LSA_EXTERN LSA_VOID mem3_assert_fatal(const LSA_CHAR *file, LSA_INT line, const LSA_CHAR *func);
LSA_EXTERN LSA_VOID*  mem3_memset( LSA_VOID * pMem, LSA_INT Value, LSA_UINT Length );
LSA_EXTERN LSA_VOID*  mem3_memcpy( LSA_VOID *pDst, LSA_VOID * pSrc, LSA_UINT Length );


#define MEM3_ASSERT(cond_)                                            \
  if(!(cond_))  mem3_assert_fatal(__FILE__, __LINE__, __FUNCTION__);  \
  
#define MEM3_USER_ASSERT(  BoolExp )      MEM3_ASSERT (BoolExp)
#define MEM3_MEMSET( dst, val, len)       mem3_memset( dst, val, len )
#define MEM3_ADMIN_MEMSET( dst, val, len) mem3_memset( dst, val, len )
#define MEM3_MEMCPY( pDst, pSrc, len )    mem3_memcpy( pDst, pSrc, len )

/* Note: to prevent define to memset in MEM3-Header file (mem3defaultmac.h) */
#define MEM3_POOL_MEMSET_FUNCTION         LSA_NULL

/* ------ end adaption to PNIO Base components ------ */

/* A lot of the following stuff should be done by the proper include-files.
   The explicitly filled typedefs and defines are to give you a documentation
   of what will be used.
   Have another look on the cfg-file in propriate subdirectories of 'doc'.
*/


/* Types and defines used by 'memory3'. Normally you will import them
   by some default header-files.
*/
// #include <string.h>

#define FLD                              /* 'far loadds' for "CadUl" without subsys else empty */

// Note: BYTE, INT32 defined as WIN32 types, do provide type mismatches
//       the types are set to WIN32 and not to LSA type 
typedef  LSA_UINT8  ORD08;               // 1-byte entity
typedef  LSA_UINT8  BYTE;                // 1-byte entity
typedef  LSA_UINT32 ORD32;               // 4-byte entity
typedef  ORD32      ORDHW;               // machine-entity
typedef  LSA_INT32  INT32;               // 4-byte entity
typedef  INT32      INTHW;               // machine-entity

#if !defined(__BOOL_DEFINED)
  #ifndef __cplusplus
    typedef LSA_UINT8 bool;  // C++ buit in type has 1 Byte size on WIN32
  #endif
#endif

#define MAX_INTHW       0x7FFFFFFF           /* the highest positive number of an INTHW            */

/*************************************************************
   You may define several macros depending of your platform.
   If you only uncomment the defines, nothing will change.
   The default is written at the comment.
*************************************************************/

/* General assertions in memory, not directly called. For direct calls see below.
#define MEM3_ASSERT(       BoolExp )  assert      (BoolExp)
*/

/* Such assertions will be made at the internal consistency of memory3.
#define MEM3_INNER_ASSERT( BoolExp )  MEM3_ASSERT (BoolExp)
*/

/* Such assertions will be made to check the actual parameters from the user.
   It might be an idea to expand it to 'throw domain_error (#BoolExp##__line)'
   if your platform handels throw, try and catch.
#define MEM3_USER_ASSERT(  BoolExp )  MEM3_ASSERT (BoolExp)
*/

/* You may define your own memory copy expression.
#define MEM3_MEMCPY(       pDst, pSrc, ByteNum )  memcpy (pDst, pSrc, ByteNum)
*/

/* You may define your own memory assignment expression.
#define MEM3_MEMSET(       pDst, Val8, ByteNum )  memset (pDst, Val8, ByteNum)
*/

/* You may define your own admin memory assignment expression.
#define MEM3_ADMIN_MEMSET( pDst, Val8, ByteNum )  memset (pDst, Val8, ByteNum)
*/

/* You may define your own pool  memory assignment function
#define MEM3_POOL_MEMSET_FUNCTION                 memset
*/

/* You have to do some auxiliary defines for configuration.
*/
#define BUDDY_MAX_LEN  20        /* the largest buffer you may get is 'Gran * 2**(BUDDY_MAX_LEN - 2)'  */
#define POOL_NUM      (5*15)     /* the maximum number of pools    */

/* */ #define LOCAL_NEWDEL       /* memory3 is used to manage submemory and may coexist with other     */
/*    #undef  LOCAL_NEWDEL   */  /* memory3 is used to replace a part of the default-library           */

// Note: if activated more memory for each buffer is allocated!!
/* */ #define MEM3_CHECK_OVERWRITE  /* */  /* define, if you wish overwrite-checking   */
/*    #undef  MEM3_CHECK_OVERWRITE     */  /* undef   else                             */

/* */ #define MEM3_BUFFER_STATISTIC       /* define, if you wish buffer-statistics    */
/*    #undef  MEM3_BUFFER_STATISTIC   */  /* undef   else                             */

/* */ #define POST_INVALIDATE             /* define, if you wish buffers invalidated by freeing    memory */
/*    #undef  POST_INVALIDATE         */  /* undef,  if you wish buffers invalidated by allocating memory */

/*    #define MEM3_SELECTIVE_LOCK            */
/* */ #undef  MEM3_SELECTIVE_LOCK         /* */

/* */ #define MEM3_SELECTIVE_OVERWRITE    /* */
/*   #undef  MEM3_SELECTIVE_OVERWRITE        */

/* */ #define MEM3_SELECTIVE_ALIGNMENT    /* */
/*   #undef  MEM3_SELECTIVE_ALIGNMENT        */

#define MEM3_BUF_PRESET_BYTE   0xCC      /* define with an 8-Bit value, if you wish other than 0xFF */

#define MEM3_ALIGN             4         /* the size of address-unit you wish to alloc    */

typedef ORDHW           LINADDR; /* a type with the usual arith-operators to hold an address-entity  */

#ifdef __cplusplus
LINADDR AddrFromPtr (void *);    /* A function that converts from a pointer to an address. With other
                                    than segmentated mode on an Intel core, it should be trivial.
                                 */

#include <new>  // StdLib++ 

#define EMPTY_THROW throw ()

/*    #define STD_ARRAY_NEW_HANDLING */  /* define, if ANSI-behaviour of compiler */
/* */ #undef  STD_ARRAY_NEW_HANDLING     /* undef , if not ...                    */


inline
LINADDR
AddrFromPtr (void *  pAll)
  {
    return LINADDR (pAll);
  }
#endif /* __cplusplus */

/* You may define some buffer trace. Buffer trace is on, if 'Mem3_TraceAlloc' and
   'Mem3_TraceFree' are defined.
   The buffer trace is thought as some post mortem dump.
   But effectively it is a hook there you can place any instrument.
*/
// #define Mem3_TraceAlloc( pBuf )  My_Mem3_TraceAlloc (pBuf)
// #define Mem3_TraceFree(  pBuf )  My_Mem3_TraceFree  (pBuf)

/* Non selective lock might block some high prior rt-threads, possibly only to get TIS-data.
   It looks to  be a better way using selective lock.
   If you don't use selective lock you've to define 'Mem3Lock' and 'Mem3Ulck'.

   If you use selective lock you have to fill another configuration file 'mem3lockcfg.hpp'.
*/
#ifndef MEM3_SELECTIVE_LOCK

// EXTERN pthread_mutex_t     MyMem3Mutex; // define it elsewhere, e.g. MGT

// #define Mem3Lock( )  pthread_mutex_lock   (& MyMem3Mutex)  /* you must tell memory3 how to enter    */
// #define Mem3Ulck( )  pthread_mutex_unlock (& MyMem3Mutex)  /* and leave a critical section          */

#define Mem3Lock( ) MEM3_ENTER()             /* you must tell memory3 how to enter    */
#define Mem3Ulck( ) MEM3_EXIT()              /* and leave a critical section          */

#endif /* ! MEM3_SELECTIVE_LOCK */

/* Possibly you have to declare some entities.
*/
// EXTERN void                My_Mem3_TraceAlloc (void const *);
// EXTERN void                My_Mem3_TraceFree  (void const *);

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of MEM3CFG_H */