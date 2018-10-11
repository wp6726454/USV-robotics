#ifndef CLRPC_PLS_H
#define CLRPC_PLS_H

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
/*  F i l e               &F: clrpc_pls.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Check the configuration                                                  */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#ifndef CLRPC_DEBUG
# error "CLRPC_DEBUG not defined in clrpc_cfg.h!"
#endif

/*===========================================================================*/

#ifndef CLRPC_FILE_SYSTEM_EXTENSION
# error "CLRPC_FILE_SYSTEM_EXTENSION not defined in file clrpc_cfg.h"
#endif

/*===========================================================================*/

#ifndef CLRPC_CFG_USE_CASTS
# error "CLRPC_CFG_USE_CASTS not defined in file clrpc_cfg.h"
#endif

/*===========================================================================*/

#ifndef CLRPC_CFG_MAX_CHANNELS
# error "CLRPC_CFG_MAX_CHANNELS not defined in file clrpc_cfg.h"
#endif

#ifndef CLRPC_CFG_MAX_CLIENTS
# error "CLRPC_CFG_MAX_CLIENTS not defined in file clrpc_cfg.h"
#endif

#ifndef CLRPC_CFG_MAX_SERVERS
# error "CLRPC_CFG_MAX_SERVERS not defined in file clrpc_cfg.h"
#endif

#ifndef CLRPC_CFG_MAX_SCALLS
# error "CLRPC_CFG_MAX_SCALLS not defined in file clrpc_cfg.h"
#endif

#ifndef CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID
# error "CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID not defined in file clrpc_cfg.h"
#endif

#ifndef CLRPC_CFG_DREP_INT
# error "CLRPC_CFG_DREP_INT not defined in file clrpc_cfg.h"
#endif

#ifndef CLRPC_CFG_DREP_CHAR
# error "CLRPC_CFG_DREP_CHAR not defined in file clrpc_cfg.h"
#endif

#ifndef CLRPC_CFG_DREP_FLOAT
# error "CLRPC_CFG_DREP_FLOAT not defined in file clrpc_cfg.h"
#endif

/*===========================================================================*/

#ifdef CLRPC_ASSERT_OR_FATAL
# error "CLRPC_ASSERT_OR_FATAL is no longer necessary (removed with V3.12i30.2)"
#endif

#ifndef CLRPC_ASSERT
# error "CLRPC_ASSERT(condition) not defined in clrpc_cfg.h!"
#endif

/*===========================================================================*/

#ifndef CLRPC_LOCAL_MEM_DYNAMIC
# error "inconsistency: CLRPC_LOCAL_MEM_DYNAMIC not defined, but clrpc uses dynamic allocation!"
#endif

#ifndef CLRPC_NO_CLEAR_GLOBAL_DATA
# error "inconsistency: CLRPC_NO_CLEAR_GLOBAL_DATA not defined, but clrpc does not clear global data!"
#endif

/*===========================================================================*/

#if !defined CLRPC_UPPER_IN_FCT_ATTR
#error "CLRPC_UPPER_IN_FCT_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_UPPER_OUT_FCT_ATTR
#error "CLRPC_UPPER_OUT_FCT_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_LOWER_IN_FCT_ATTR
#error "CLRPC_LOWER_IN_FCT_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_LOWER_OUT_FCT_ATTR
#error "CLRPC_LOWER_OUT_FCT_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_SYSTEM_IN_FCT_ATTR
#error "CLRPC_SYSTEM_IN_FCT_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_SYSTEM_OUT_FCT_ATTR
#error "CLRPC_SYSTEM_OUT_FCT_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_LOCAL_FCT_ATTR
#error "CLRPC_LOCAL_FCT_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_UPPER_RQB_ATTR
#error "CLRPC_UPPER_RQB_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_UPPER_MEM_ATTR
#error "CLRPC_UPPER_MEM_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_LOWER_RQB_ATTR
#error "CLRPC_LOWER_RQB_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_LOWER_MEM_ATTR
#error "CLRPC_LOWER_MEM_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_SYSTEM_MEM_ATTR
#error "CLRPC_SYSTEM_MEM_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined CLRPC_LOCAL_MEM_ATTR
#error "CLRPC_LOCAL_MEM_ATTR not defined in file clrpc_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined CLRPC_LOCAL_MEM_STATIC && !defined CLRPC_LOCAL_MEM_DYNAMIC
#error "CLRPC_LOCAL_MEM_STATIC or CLRPC_LOCAL_MEM_DYNAMIC is missing in file clrpc_cfg.h!"
#endif

#if defined CLRPC_LOCAL_MEM_STATIC && defined CLRPC_LOCAL_MEM_DYNAMIC
#error "Define CLRPC_LOCAL_MEM_STATIC or CLRPC_LOCAL_MEM_DYNAMIC in file clrpc_cfg.h!"
#endif

#if !defined CLRPC_CLEAR_GLOBAL_DATA && !defined CLRPC_NO_CLEAR_GLOBAL_DATA
#error "CLRPC_CLEAR_GLOBAL_DATA or CLRPC_NO_CLEAR_GLOBAL_DATA is missing in file clrpc_cfg.h!"
#endif

#if defined CLRPC_CLEAR_GLOBAL_DATA && defined CLRPC_NO_CLEAR_GLOBAL_DATA
#error "Define CLRPC_CLEAR_GLOBAL_DATA or CLRPC_NO_CLEAR_GLOBAL_DATA in file clrpc_cfg.h!"
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_PLS_H */
