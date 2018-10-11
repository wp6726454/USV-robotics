#ifndef ACP_CFG_H
#define ACP_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: acp_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System integration of LSA-component ACP                                  */
/*  Using the PSI framework.                                              */
/*                                                                           */
/*****************************************************************************/

/*
 * included by "acp_inc.h"
 */

LSA_VOID acp_psi_startstop (LSA_INT start);

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#define ACP_DEBUG  PSI_DEBUG

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/
#define ACP_ATTR_NEAR
#define ACP_ATTR_FAR
#define ACP_ATTR_SHUGE
#define ACP_ATTR_HUGE


/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/
#define ACP_UPPER_IN_FCT_ATTR    ACP_ATTR_HUGE  /* acp-user to acp */
#define ACP_UPPER_OUT_FCT_ATTR   ACP_ATTR_HUGE  /* acp to acp-user */
#define ACP_LOWER_IN_FCT_ATTR    ACP_ATTR_HUGE  /* lower component to acp */
#define ACP_LOWER_OUT_FCT_ATTR   ACP_ATTR_HUGE  /* acp to lower component */
#define ACP_SYSTEM_IN_FCT_ATTR   ACP_ATTR_FAR   /* system to acp */
#define ACP_SYSTEM_OUT_FCT_ATTR  ACP_ATTR_FAR   /* acp to system */
#define ACP_LOCAL_FCT_ATTR       ACP_ATTR_NEAR  /* local function */


/*=====  data attributes  =====*/
#define ACP_UPPER_RQB_ATTR       ACP_ATTR_HUGE  /* upper-rqb data */
#define ACP_UPPER_MEM_ATTR       ACP_ATTR_NEAR  /* upper-memory data */
#define ACP_LOWER_RQB_ATTR       ACP_ATTR_HUGE  /* lower-rqb data */
#define ACP_LOWER_MEM_ATTR       ACP_ATTR_NEAR  /* lower-memory data */
#define ACP_SYSTEM_MEM_ATTR      ACP_ATTR_HUGE  /* system-memory data */
#define ACP_LOCAL_MEM_ATTR       ACP_ATTR_SHUGE /* local data */

#define ACP_COMMON_MEM_ATTR      ACP_ATTR_SHUGE /* greatest common */
                                                /* data attribute  */


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*------------------------------------------------------------------------------
// the lower layer
//----------------------------------------------------------------------------*/

#define ACP_LOWER_RQB_TYPE						EDD_RQB_TYPE

#define ACP_LL_GET_NEXT_RQB_PTR(pRQB)			EDD_RQB_GET_NEXT_RQB_PTR (pRQB)
#define ACP_LL_GET_PREV_RQB_PTR(pRQB)			EDD_RQB_GET_PREV_RQB_PTR (pRQB)

#define ACP_LL_SET_OPCODE(pRQB, value)			EDD_RQB_SET_OPCODE (pRQB, value)
#define ACP_LL_GET_OPCODE(pRQB)					EDD_RQB_GET_OPCODE (pRQB)

#define ACP_LL_SET_SERVICE(pRQB, value)			EDD_RQB_SET_SERVICE (pRQB, value)
#define ACP_LL_GET_SERVICE(pRQB)				EDD_RQB_GET_SERVICE (pRQB)

#define ACP_LL_SET_HANDLE(pRQB, value)			EDD_RQB_SET_HANDLE (pRQB, value)
#define ACP_LL_GET_HANDLE(pRQB)					EDD_RQB_GET_HANDLE (pRQB)

#define ACP_LL_SET_RESPONSE(pRQB, value)		EDD_RQB_SET_RESPONSE (pRQB, value)
#define ACP_LL_GET_RESPONSE(pRQB)				EDD_RQB_GET_RESPONSE (pRQB)

#define ACP_LL_SET_USERID_PTR(pRQB, value)		EDD_RQB_SET_USERID_PTR (pRQB, value)
#define ACP_LL_GET_USERID_PTR(pRQB)				EDD_RQB_GET_USERID_PTR (pRQB)

#define ACP_LL_SET_PPARAM(pRQB, value)			EDD_RQB_SET_PPARAM (pRQB, value)
#define ACP_LL_GET_PPARAM(pRQB, type)			((type)(EDD_RQB_GET_PPARAM (pRQB)))


/*===========================================================================*/
/* LSA-HEADER and LSA-TRAILER                                                */
/*===========================================================================*/

#define ACP_RQB_HEADER						LSA_RQB_HEADER(ACP_UPPER_RQB_PTR_TYPE)

#define ACP_RQB_TRAILER						LSA_RQB_TRAILER

#define ACP_RQB_SET_NEXT_RQB_PTR(rb,v)		LSA_RQB_SET_NEXT_RQB_PTR(rb,v)
#define ACP_RQB_SET_PREV_RQB_PTR(rb,v)		LSA_RQB_SET_PREV_RQB_PTR(rb,v)
#define ACP_RQB_SET_OPCODE(rb,v)			LSA_RQB_SET_OPCODE(rb,v)
#define ACP_RQB_SET_HANDLE(rb,v)			LSA_RQB_SET_HANDLE(rb,v)
#define ACP_RQB_SET_RESPONSE(rb,v)			LSA_RQB_SET_RESPONSE(rb,v)
#define ACP_RQB_SET_USER_ID_PTR(rb,v)		LSA_RQB_SET_USER_ID_PTR(rb,v)

#define ACP_RQB_GET_NEXT_RQB_PTR(rb)		LSA_RQB_GET_NEXT_RQB_PTR(rb)
#define ACP_RQB_GET_PREV_RQB_PTR(rb)		LSA_RQB_GET_PREV_RQB_PTR(rb)
#define ACP_RQB_GET_OPCODE(rb)				LSA_RQB_GET_OPCODE(rb)
#define ACP_RQB_GET_HANDLE(rb)				LSA_RQB_GET_HANDLE(rb)
#define ACP_RQB_GET_RESPONSE(rb)			LSA_RQB_GET_RESPONSE(rb)
#define ACP_RQB_GET_USER_ID_PTR(rb)			LSA_RQB_GET_USER_ID_PTR(rb)


/*------------------------------------------------------------------------------
// maximum number of open LSA channels
//----------------------------------------------------------------------------*/

#ifndef ACP_CFG_MAX_CHANNELS
# error "ACP_CFG_MAX_CHANNELS not defined"   /* 4 .. System / UserIOC / UserIOD / UserIOM */
#endif


/*------------------------------------------------------------------------------
// enable / disable cast-warnings
//----------------------------------------------------------------------------*/

#ifndef ACP_CFG_CAST_WARN
# define ACP_CFG_CAST_WARN  0
#endif


/*------------------------------------------------------------------------------
// enable / disable RT class 3 support
//----------------------------------------------------------------------------*/

#ifndef ACP_CFG_RT_CLASS_3_SUPPORT_ON
# error "ACP_CFG_RT_CLASS_3_SUPPORT_ON not defined" /* 0 = no rt-class 3, 1 = rt-class 3 supported */
#endif


/*------------------------------------------------------------------------------
// enable / disable ACP_ASSERT
//----------------------------------------------------------------------------*/

#if ACP_DEBUG

extern int acp_semicolon_missing; /* see acp_cfg.c */

	/* check for condition: internal programming error */
# define ACP_ASSERT(cond) \
	if (! (cond)) { ACP_FATAL (0); } else acp_semicolon_missing = 1

#else

	/* no more programming error exists :-) */
# define ACP_ASSERT(cond)

#endif


/*------------------------------------------------------------------------------
// ACP_STATIC_ASSERT
//  Does an ASSERT at compile time.
//  With Visual Studio 2010 you can use:   static_assert(expr_, msg_)
//  with C11 you can use:                  _Static_assert(expr_, msg_)
//  with cpp magic you can use:            { extern char _compile_time_assert[expr_]; }
//  else use runtime assert:               ACP_ASSERT(expr_)
//----------------------------------------------------------------------------*/

#ifndef ACP_STATIC_ASSERT
/* define ACP_STATIC_ASSERT(expr_, msg_)    static_assert(expr_, msg_) */
#  define ACP_STATIC_ASSERT(expr_, msg_)    ACP_ASSERT(expr_) /* use runtime assert */
#endif


/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef ACP_FILE_SYSTEM_EXTENSION
# define ACP_FILE_SYSTEM_EXTENSION(module_id_)
#endif


/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 .. no LSA trace
//	1 .. LSA trace [default]
//	2 .. LSA index trace
//----------------------------------------------------------------------------*/

#ifndef ACP_CFG_TRACE_MODE
# error "ACP_CFG_TRACE_MODE not defined" 
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_CFG_H */
