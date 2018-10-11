#ifndef CM_LOW_H
#define CM_LOW_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_low.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Lower-Layer interface                                                    */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/

typedef CM_ACP_LOWER_RQB_TYPE  CM_ACP_LOWER_RQB_ATTR *CM_ACP_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

typedef CM_EDD_LOWER_RQB_TYPE  CM_EDD_LOWER_RQB_ATTR *CM_EDD_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

#if CM_CFG_USE_GSY
typedef CM_GSY_LOWER_RQB_TYPE  CM_GSY_LOWER_RQB_ATTR *CM_GSY_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
#endif

#if CM_CFG_USE_NARE
typedef CM_NARE_LOWER_RQB_TYPE  CM_NARE_LOWER_RQB_ATTR *CM_NARE_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
#endif

typedef CM_OHA_LOWER_RQB_TYPE  CM_OHA_LOWER_RQB_ATTR *CM_OHA_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

typedef CM_RPC_LOWER_RQB_TYPE  CM_RPC_LOWER_RQB_ATTR *CM_RPC_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

#if CM_CFG_USE_MRP
typedef CM_MRP_LOWER_RQB_TYPE  CM_MRP_LOWER_RQB_ATTR *CM_MRP_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
#endif

#if CM_CFG_USE_POF
typedef CM_POF_LOWER_RQB_TYPE  CM_POF_LOWER_RQB_ATTR *CM_POF_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
#endif

/*----------------------------------------------------------------------------*/

#define CM_EDD_GET_PPARAM(edd_rqb_ptr, edd_type)	((edd_type)EDD_RQB_GET_PPARAM(edd_rqb_ptr))

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  cm_request_lower_done
 *
 * function:       lower done callback-function
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID  CM_ACP_LOWER_IN_FCT_ATTR
cm_acp_request_lower_done (
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

LSA_VOID  CM_EDD_LOWER_IN_FCT_ATTR
cm_edd_request_lower_done (
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

#if CM_CFG_USE_GSY
LSA_VOID  CM_GSY_LOWER_IN_FCT_ATTR
cm_gsy_request_lower_done (
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

#if CM_CFG_USE_NARE
LSA_VOID  CM_NARE_LOWER_IN_FCT_ATTR
cm_nare_request_lower_done (
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

LSA_VOID  CM_OHA_LOWER_IN_FCT_ATTR
cm_oha_request_lower_done (
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

LSA_VOID  CM_RPC_LOWER_IN_FCT_ATTR
cm_rpc_request_lower_done (
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

#if CM_CFG_USE_MRP
LSA_VOID  CM_RPC_LOWER_IN_FCT_ATTR
cm_mrp_request_lower_done (
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

#if CM_CFG_USE_POF
LSA_VOID  CM_RPC_LOWER_IN_FCT_ATTR
cm_pof_request_lower_done (
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  cm_lower_rqb
 *
 * function:       asynchronous allocation of a lower-RQB
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used */
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR  cm_lower_rqb(
    LSA_USER_ID_TYPE        user_id,
    LSA_UINT16              length,
    CM_LOWER_RQB_PTR_TYPE   lower_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  cm_lower_mem
 *
 * function:       asynchronous allocation of a lower-memory
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used */
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR  cm_lower_mem(
    LSA_USER_ID_TYPE        user_id,
    LSA_UINT16              length,
    CM_LOWER_MEM_PTR_TYPE   lower_mem_ptr
);
#endif


/*====  out functions  =====*/

/*=============================================================================
 * function name:  CM_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to lower layer
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_OPEN_CHANNEL_LOWER
LSA_VOID  CM_ACP_LOWER_OUT_FCT_ATTR
CM_ACP_OPEN_CHANNEL_LOWER(
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_EDD_OPEN_CHANNEL_LOWER
LSA_VOID  CM_EDD_LOWER_OUT_FCT_ATTR
CM_EDD_OPEN_CHANNEL_LOWER(
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_OPEN_CHANNEL_LOWER
LSA_VOID  CM_GSY_LOWER_OUT_FCT_ATTR
CM_GSY_OPEN_CHANNEL_LOWER(
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_OPEN_CHANNEL_LOWER
LSA_VOID  CM_NARE_LOWER_OUT_FCT_ATTR
CM_NARE_OPEN_CHANNEL_LOWER(
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#ifndef CM_OHA_OPEN_CHANNEL_LOWER
LSA_VOID  CM_OHA_LOWER_OUT_FCT_ATTR
CM_OHA_OPEN_CHANNEL_LOWER(
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_RPC_OPEN_CHANNEL_LOWER
LSA_VOID  CM_RPC_LOWER_OUT_FCT_ATTR
CM_RPC_OPEN_CHANNEL_LOWER(
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_OPEN_CHANNEL_LOWER
LSA_VOID  CM_MRP_LOWER_OUT_FCT_ATTR
CM_MRP_OPEN_CHANNEL_LOWER(
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_OPEN_CHANNEL_LOWER
LSA_VOID  CM_POF_LOWER_OUT_FCT_ATTR
CM_POF_OPEN_CHANNEL_LOWER(
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif


/*=============================================================================
 * function name:  CM_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to lower layer
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_ACP_LOWER_OUT_FCT_ATTR
CM_ACP_CLOSE_CHANNEL_LOWER (
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_EDD_LOWER_OUT_FCT_ATTR
CM_EDD_CLOSE_CHANNEL_LOWER (
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_GSY_LOWER_OUT_FCT_ATTR
CM_GSY_CLOSE_CHANNEL_LOWER (
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_NARE_LOWER_OUT_FCT_ATTR
CM_NARE_CLOSE_CHANNEL_LOWER (
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#ifndef CM_OHA_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_OHA_LOWER_OUT_FCT_ATTR
CM_OHA_CLOSE_CHANNEL_LOWER (
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_RPC_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_RPC_LOWER_OUT_FCT_ATTR
CM_RPC_CLOSE_CHANNEL_LOWER(
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_MRP_LOWER_OUT_FCT_ATTR
CM_MRP_CLOSE_CHANNEL_LOWER (
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_CLOSE_CHANNEL_LOWER
LSA_VOID  CM_POF_LOWER_OUT_FCT_ATTR
CM_POF_CLOSE_CHANNEL_LOWER (
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif


/*=============================================================================
 * function name:  CM_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to lower layer
 *
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_REQUEST_LOWER
LSA_VOID  CM_ACP_LOWER_OUT_FCT_ATTR
CM_ACP_REQUEST_LOWER (
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_EDD_REQUEST_LOWER
LSA_VOID  CM_EDD_LOWER_OUT_FCT_ATTR
CM_EDD_REQUEST_LOWER (
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_REQUEST_LOWER
LSA_VOID  CM_GSY_LOWER_OUT_FCT_ATTR
CM_GSY_REQUEST_LOWER (
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_REQUEST_LOWER
LSA_VOID  CM_NARE_LOWER_OUT_FCT_ATTR
CM_NARE_REQUEST_LOWER (
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#ifndef CM_OHA_REQUEST_LOWER
LSA_VOID  CM_OHA_LOWER_OUT_FCT_ATTR
CM_OHA_REQUEST_LOWER (
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_RPC_REQUEST_LOWER
LSA_VOID  CM_RPC_LOWER_OUT_FCT_ATTR
CM_RPC_REQUEST_LOWER(
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_REQUEST_LOWER
LSA_VOID  CM_MRP_LOWER_OUT_FCT_ATTR
CM_MRP_REQUEST_LOWER (
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_REQUEST_LOWER
LSA_VOID  CM_POF_LOWER_OUT_FCT_ATTR
CM_POF_REQUEST_LOWER (
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif


/*=============================================================================
 * function name:  CM_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ACP_ALLOC_LOWER_RQB(
    CM_ACP_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#ifndef CM_EDD_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_EDD_ALLOC_LOWER_RQB(
    CM_EDD_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GSY_ALLOC_LOWER_RQB(
    CM_GSY_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_NARE_ALLOC_LOWER_RQB(
    CM_NARE_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                   user_id,
    LSA_UINT16                                         length,
    LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif
#endif

#ifndef CM_OHA_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_OHA_ALLOC_LOWER_RQB(
    CM_OHA_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#ifndef CM_RPC_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_RPC_ALLOC_LOWER_RQB(
    CM_RPC_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MRP_ALLOC_LOWER_RQB(
    CM_MRP_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_ALLOC_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_POF_ALLOC_LOWER_RQB(
    CM_POF_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif


/*=============================================================================
 * function name:  CM_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ACP_FREE_LOWER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_ACP_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#ifndef CM_EDD_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_EDD_FREE_LOWER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_EDD_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GSY_FREE_LOWER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_GSY_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_NARE_FREE_LOWER_RQB(
    LSA_UINT16                  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_NARE_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                 sys_ptr
);
#endif
#endif

#ifndef CM_OHA_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_OHA_FREE_LOWER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_OHA_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#ifndef CM_RPC_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_RPC_FREE_LOWER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_RPC_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MRP_FREE_LOWER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_MRP_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_FREE_LOWER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_POF_FREE_LOWER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_POF_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif
#endif


/*=============================================================================
 * function name:  CM_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ACP_ALLOC_LOWER_MEM(
    CM_ACP_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#ifndef CM_ACP_ALLOC_ALARM_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ACP_ALLOC_ALARM_MEM(
    CM_ACP_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_alarm_ptr_ptr,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#ifndef CM_EDD_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_EDD_ALLOC_LOWER_MEM(
    CM_EDD_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GSY_ALLOC_LOWER_MEM(
    CM_GSY_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_NARE_ALLOC_LOWER_MEM(
    CM_NARE_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                   user_id,
    LSA_UINT16                                         length,
    LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif
#endif

#ifndef CM_OHA_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_OHA_ALLOC_LOWER_MEM(
    CM_OHA_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#ifndef CM_RPC_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_RPC_ALLOC_LOWER_MEM(
    CM_RPC_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MRP_ALLOC_LOWER_MEM(
    CM_MRP_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_ALLOC_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_POF_ALLOC_LOWER_MEM(
    CM_POF_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif


/*=============================================================================
 * function name:  CM_FREE_LOWER_MEM
 *
 * function:       free a lower-memory
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ACP_FREE_LOWER_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_ACP_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#ifndef CM_ACP_FREE_ALARM_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ACP_FREE_ALARM_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_ACP_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#ifndef CM_EDD_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_EDD_FREE_LOWER_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GSY_FREE_LOWER_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_GSY_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_NARE_FREE_LOWER_MEM(
    LSA_UINT16                  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_NARE_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                 sys_ptr
);
#endif
#endif

#ifndef CM_OHA_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_OHA_FREE_LOWER_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_OHA_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#ifndef CM_RPC_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_RPC_FREE_LOWER_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_RPC_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MRP_FREE_LOWER_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_MRP_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_FREE_LOWER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_POF_FREE_LOWER_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_POF_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                sys_ptr
);
#endif
#endif


/*=============================================================================
 * function name:  CM_LOWER_RQB_ERROR
 *
 * function:       notify a fatal-error within the lower rqb
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_ACP_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ACP_LOWER_RQB_ERROR(
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

#ifndef CM_EDD_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_EDD_LOWER_RQB_ERROR(
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GSY_LOWER_RQB_ERROR(
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif
#endif

#if CM_CFG_USE_NARE
#ifndef CM_NARE_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_NARE_LOWER_RQB_ERROR(
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif
#endif

#ifndef CM_OHA_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_OHA_LOWER_RQB_ERROR(
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

#ifndef CM_RPC_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_RPC_LOWER_RQB_ERROR(
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MRP_LOWER_RQB_ERROR(
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_LOWER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_POF_LOWER_RQB_ERROR(
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_LOW_H */
