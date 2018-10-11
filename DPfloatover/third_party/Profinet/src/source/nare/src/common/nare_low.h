
#ifndef NARE_LOW_H                       /* ----- reinclude-protection ----- */
#define NARE_LOW_H

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
/*  F i l e               &F: nare_low.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Lower component interface                                                */
/*  Defines constants, types, macros and prototyping for NARE                */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/* LOWER LAYER MACROS to DCP                                                 */
/*===========================================================================*/

/* RQB */
typedef DCP_RQB_TYPE    NARE_DCP_LOWER_RQB_TYPE;

typedef NARE_DCP_LOWER_RQB_TYPE   NARE_DCP_LOWER_RQB_ATTR *  NARE_DCP_LOWER_RQB_PTR_TYPE;

/*===========================================================================*/
/* LOWER LAYER MACROS to EDD                                                 */
/*===========================================================================*/

/* RQB */
typedef EDD_RQB_TYPE    NARE_EDD_LOWER_RQB_TYPE;

typedef NARE_EDD_LOWER_RQB_TYPE   NARE_EDD_LOWER_RQB_ATTR *  NARE_EDD_LOWER_RQB_PTR_TYPE;

typedef EDD_RQB_OPEN_CHANNEL_TYPE    NARE_EDD_LOWER_MEM_ATTR * NARE_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE;
typedef EDD_RQB_NRT_SEND_TYPE        NARE_EDD_LOWER_MEM_ATTR * NARE_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE;
typedef EDD_RQB_GET_PARAMS_TYPE      NARE_EDD_LOWER_MEM_ATTR * NARE_EDD_RQB_GET_PARAMS_PTR_TYPE;

/*===========================================================================*/
/* LOWER LAYER MACROS to SOCK                                                */
/*===========================================================================*/

/* RQB */
#ifndef NARE_CFG_NO_SOCK
typedef SOCK_RQB_TYPE    NARE_SOCK_LOWER_RQB_TYPE;

typedef NARE_SOCK_LOWER_RQB_TYPE   NARE_SOCK_LOWER_RQB_ATTR *  NARE_SOCK_LOWER_RQB_PTR_TYPE;

#endif

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*===========================================================================*/
/*====                     in functions from DCP                         ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  nare_dcp_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of NARE-lower request to DCP:
 *
 * parameters:     NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB   pointer to lower-
 *                                                     RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see dcp_open_channel(), dcp_close_channel()
 *                    respectively dcp_request()
 *===========================================================================*/
LSA_VOID  NARE_DCP_LOWER_IN_FCT_ATTR  nare_dcp_request_lower_done(
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB
);

/*===========================================================================*/
/*====                    out functions to DCP                           ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  NARE_DCP_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to DCP lower layer
 *
 * parameters:     NARE_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     LSA_USER_ID_TYPE   user-id:       id of prefix
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of prefix
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (DCP_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
 *
 * RQB-return values via callback-function:
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of lower layer
 *                        response:      *_RSP_OK
 *                                       *_RSP_ERR_SYNTAX
 *                                       *_RSP_ERR_RESOURCE
 *                                       *_RSP_ERR_SYS_PATH
 *                                       or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef NARE_DCP_OPEN_CHANNEL_LOWER
LSA_VOID  NARE_DCP_LOWER_OUT_FCT_ATTR  NARE_DCP_OPEN_CHANNEL_LOWER(
    NARE_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to DCP lower layer
 *
 * parameters:     NARE_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:  id of prefix
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef NARE_DCP_CLOSE_CHANNEL_LOWER
LSA_VOID  NARE_DCP_LOWER_OUT_FCT_ATTR  NARE_DCP_CLOSE_CHANNEL_LOWER(
    NARE_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to DCP lower layer
 *
 * parameters:     NARE_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  *_RSP_ERR_REF
 *                                  *_RSP_ERR_SEQUENCE
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
#ifndef NARE_DCP_REQUEST_LOWER
LSA_VOID  NARE_DCP_LOWER_OUT_FCT_ATTR  NARE_DCP_REQUEST_LOWER(
    NARE_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB for DCP synchronously
 *
 * parameters:     NARE_DCP_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DCP_ALLOC_LOWER_RQB
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DCP_ALLOC_LOWER_RQB(
    NARE_DCP_LOWER_RQB_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB from DCP
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *                 NARE_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DCP_FREE_LOWER_RQB
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DCP_FREE_LOWER_RQB(
    LSA_UINT16                     NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_DCP_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory for DCP synchronously
 *
 * parameters:     NARE_DCP_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DCP_ALLOC_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DCP_ALLOC_LOWER_MEM(
    NARE_DCP_LOWER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_FREE_LOWER_MEM
 *
 * function:       free a lower-memory for DCP
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 NARE_DCP_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DCP_FREE_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DCP_FREE_LOWER_MEM(
    LSA_UINT16                     NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_DCP_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower-memory to upper-memory
 *
 * parameters:     NARE_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 NARE_DCP_LOWER_MEM_PTR_TYPE     lower_mem_ptr:
 *                                                 pointer to lower-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DCP_LOWER_TO_UPPER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DCP_LOWER_TO_UPPER_MEM(
    NARE_UPPER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *      upper_mem_ptr_ptr,
    NARE_DCP_LOWER_MEM_PTR_TYPE                         lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper-memory to lower-memory
 *
 * parameters:     NARE_DCP_LOWER_MEM_PTR_TYPE  ..*  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 NARE_UPPER_MEM_PTR_TYPE         upper_mem_ptr:
 *                                                 pointer to upper-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DCP_UPPER_TO_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DCP_UPPER_TO_LOWER_MEM(
    NARE_DCP_LOWER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR   *   lower_mem_ptr_ptr,
    NARE_UPPER_MEM_PTR_TYPE                                upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                       sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DCP_UPPER_RQB_TO_LOWER_MEM
 *
 * function:       convert upper-RQB memory to lower-memory
 *
 * parameters:     NARE_DCP_LOWER_MEM_PTR_TYPE  ..*  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 NARE_UPPER_RQB_MEM_PTR_TYPE     upper_rqb_mem_ptr:
 *                                                 pointer to upper-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DCP_UPPER_RQB_TO_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DCP_UPPER_RQB_TO_LOWER_MEM(
    NARE_DCP_LOWER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR   *   lower_mem_ptr_ptr,
    NARE_UPPER_RQB_MEM_PTR_TYPE                            upper_rqb_mem_ptr,
    LSA_SYS_PTR_TYPE                                       sys_ptr
);
#endif

#ifndef NARE_CFG_NO_SOCK

/*===========================================================================*/
/*====                     in functions from SOCK                        ====*/
/*===========================================================================*/


/*=============================================================================
 * function name:  nare_sock_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of NARE-lower request to SOCK:
 *
 * parameters:     NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB:  pointer to lower-
 *                                                      RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see sock_open_channel(), sock_close_channel()
 *                    respectively sock_request()
 *===========================================================================*/

LSA_VOID  NARE_SOCK_LOWER_IN_FCT_ATTR  nare_sock_request_lower_done(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  pRQB
);

/*===========================================================================*/
/*====                    out functions to SOCK                          ====*/
/*===========================================================================*/


/*=============================================================================
 * function name:  NARE_SOCK_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to DCP lower layer
 *
 * parameters:     NARE_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                               RQB
 *                 LSA_SYS_PTR_TYPE              sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     LSA_USER_ID_TYPE   user-id:       id of prefix
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of prefix
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (SOCK_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
 *
 * RQB-return values via callback-function:
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of lower layer
 *                        response:      *_RSP_OK
 *                                       *_RSP_ERR_SYNTAX
 *                                       *_RSP_ERR_RESOURCE
 *                                       *_RSP_ERR_SYS_PATH
 *                                       or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef NARE_SOCK_OPEN_CHANNEL_LOWER
LSA_VOID  NARE_SOCK_LOWER_OUT_FCT_ATTR  NARE_SOCK_OPEN_CHANNEL_LOWER(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE              sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to DCP lower layer
 *
 * parameters:     NARE_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                               RQB
 *                 LSA_SYS_PTR_TYPE              sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:  id of prefix
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef NARE_SOCK_CLOSE_CHANNEL_LOWER
LSA_VOID  NARE_SOCK_LOWER_OUT_FCT_ATTR  NARE_SOCK_CLOSE_CHANNEL_LOWER(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE              sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to DCP lower layer
 *
 * parameters:     NARE_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                               RQB
 *                 LSA_SYS_PTR_TYPE              sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  *_RSP_ERR_REF
 *                                  *_RSP_ERR_SEQUENCE
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
#ifndef NARE_SOCK_REQUEST_LOWER
LSA_VOID  NARE_SOCK_LOWER_OUT_FCT_ATTR  NARE_SOCK_REQUEST_LOWER(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE              sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB for DCP synchronously
 *
 * parameters:     NARE_SOCK_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_SOCK_ALLOC_LOWER_RQB
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_SOCK_ALLOC_LOWER_RQB(
    NARE_SOCK_LOWER_RQB_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                     user_id,
    LSA_UINT16                                           length,
    LSA_SYS_PTR_TYPE                                     sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB from DCP
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *                 NARE_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                               RQB
 *                 LSA_SYS_PTR_TYPE              sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_SOCK_FREE_LOWER_RQB
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_SOCK_FREE_LOWER_RQB(
    LSA_UINT16                     NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_SOCK_LOWER_RQB_PTR_TYPE                          lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory for DCP synchronously
 *
 * parameters:     NARE_SOCK_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_SOCK_ALLOC_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_SOCK_ALLOC_LOWER_MEM(
    NARE_SOCK_LOWER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                     user_id,
    LSA_UINT16                                           length,
    LSA_SYS_PTR_TYPE                                     sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_FREE_LOWER_MEM
 *
 * function:       free a lower-memory for DCP
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 NARE_SOCK_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                               memory
 *                 LSA_SYS_PTR_TYPE              sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_SOCK_FREE_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_SOCK_FREE_LOWER_MEM(
    LSA_UINT16                     NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_SOCK_LOWER_MEM_PTR_TYPE                          lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower-memory to upper-memory
 *
 * parameters:     NARE_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 NARE_SOCK_LOWER_MEM_PTR_TYPE    lower_mem_ptr:
 *                                                 pointer to lower-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_SOCK_LOWER_TO_UPPER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_SOCK_LOWER_TO_UPPER_MEM(
    NARE_UPPER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *      upper_mem_ptr_ptr,
    NARE_SOCK_LOWER_MEM_PTR_TYPE                        lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_SOCK_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper-memory to lower-memory
 *
 * parameters:     NARE_SOCK_LOWER_MEM_PTR_TYPE  ..*  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 NARE_UPPER_MEM_PTR_TYPE         upper_mem_ptr:
 *                                                 pointer to upper-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_SOCK_UPPER_TO_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_SOCK_UPPER_TO_LOWER_MEM(
    NARE_SOCK_LOWER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    NARE_UPPER_MEM_PTR_TYPE                                upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                       sys_ptr
);
#endif

#endif

/*===========================================================================*/
/*====                     in functions from EDD                         ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  nare_edd_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of NARE-lower request to EDD:
 *
 * parameters:     NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB   pointer to lower-
 *                                                     RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see edd_open_channel(), edd_close_channel()
 *                    respectively edd_request()
 *===========================================================================*/

LSA_VOID  NARE_EDD_LOWER_IN_FCT_ATTR  nare_edd_request_lower_done(
    NARE_EDD_LOWER_RQB_PTR_TYPE  pRQB
);

/*===========================================================================*/
/*====                    out functions to EDD                           ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  NARE_EDD_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to EDD lower layer
 *
 * parameters:     NARE_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     LSA_USER_ID_TYPE   user-id:       id of prefix
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of prefix
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (DCP_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
 *
 * RQB-return values via callback-function:
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of lower layer
 *                        response:      *_RSP_OK
 *                                       *_RSP_ERR_SYNTAX
 *                                       *_RSP_ERR_RESOURCE
 *                                       *_RSP_ERR_SYS_PATH
 *                                       or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef NARE_EDD_OPEN_CHANNEL_LOWER
LSA_VOID  NARE_EDD_LOWER_OUT_FCT_ATTR  NARE_EDD_OPEN_CHANNEL_LOWER(
    NARE_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_EDD_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to EDD lower layer
 *
 * parameters:     NARE_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:  id of prefix
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef NARE_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID  NARE_EDD_LOWER_OUT_FCT_ATTR  NARE_EDD_CLOSE_CHANNEL_LOWER(
    NARE_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_EDD_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to EDD lower layer
 *
 * parameters:     NARE_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  *_RSP_ERR_REF
 *                                  *_RSP_ERR_SEQUENCE
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
#ifndef NARE_EDD_REQUEST_LOWER
LSA_VOID  NARE_EDD_LOWER_OUT_FCT_ATTR  NARE_EDD_REQUEST_LOWER(
    NARE_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_EDD_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB for EDD synchronously
 *
 * parameters:     NARE_EDD_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_EDD_ALLOC_LOWER_RQB
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_EDD_ALLOC_LOWER_RQB(
    NARE_EDD_LOWER_RQB_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_EDD_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB from EDD
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *                 NARE_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_EDD_FREE_LOWER_RQB
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_EDD_FREE_LOWER_RQB(
    LSA_UINT16                     NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_EDD_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_EDD_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory for EDD synchronously
 *
 * parameters:     NARE_EDD_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_EDD_ALLOC_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_EDD_ALLOC_LOWER_MEM(
    NARE_EDD_LOWER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_EDD_FREE_LOWER_MEM
 *
 * function:       free a lower-memory for EDD
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 NARE_EDD_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_EDD_FREE_LOWER_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_EDD_FREE_LOWER_MEM(
    LSA_UINT16                     NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_EDD_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif


/*=============================================================================
 * function name:  NARE_EDD_ALLOC_LOWER_TXMEM
 *
 * function:       allocate a lower memory for send PDUs
 *
 *                 This memory has to be DWORD-aligned!!
 *
 * parameters:     NARE_EDD_LOWER_MEM_PTR_TYPE  ...  * lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: failed.
 *                 LSA_USER_ID_TYPE                   user_id:  id of edd
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_EDD_ALLOC_LOWER_TXMEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_EDD_ALLOC_LOWER_TXMEM(
    NARE_EDD_LOWER_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_EDD_FREE_LOWER_TXMEM
 *
 * function:       free a lower memory for send PDUs
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 NARE_EDD_LOWER_MEM_PTR_TYPE  lower_mem_ptr:pointer to lower-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:      system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_EDD_FREE_LOWER_TXMEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_EDD_FREE_LOWER_TXMEM(
    LSA_UINT16                 NARE_LOCAL_MEM_ATTR *    ret_val_ptr,
    NARE_EDD_LOWER_MEM_PTR_TYPE                         lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*****************************************************************************/
/*  end of file NARE_LOW.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_LOW_H */
