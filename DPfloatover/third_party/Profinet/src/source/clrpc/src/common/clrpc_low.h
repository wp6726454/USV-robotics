#ifndef CLRPC_LOW_H
#define CLRPC_LOW_H

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
/*  F i l e               &F: clrpc_low.h                               :F&  */
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
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef CLRPC_LOWER_RQB_TYPE  CLRPC_LOWER_RQB_ATTR *CLRPC_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

/*===========================================================================*/

/* sock doesn't define the pointers */

typedef SOCK_RQB_ARGS_CHANNEL_TYPE		CLRPC_LOWER_MEM_ATTR *CLRPC_LL_OPEN_CHANNEL_PTR_TYPE;
typedef SOCK_RQB_ARGS_UDP_OPEN_TYPE		CLRPC_LOWER_MEM_ATTR *CLRPC_LL_UDP_OPEN_PTR_TYPE;
typedef SOCK_RQB_ARGS_CLOSE_TYPE		CLRPC_LOWER_MEM_ATTR *CLRPC_LL_UDP_CLOSE_PTR_TYPE;
typedef SOCK_RQB_ARGS_DATA_TYPE			CLRPC_LOWER_MEM_ATTR *CLRPC_LL_UDP_DATA_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  clrpc_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of clrpc_open_channel(),
 *                 clrpc_close_channel() and clrpc_request()
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

LSA_VOID  CLRPC_LOWER_IN_FCT_ATTR  clrpc_request_lower_done(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


/*=============================================================================
 * function name:  clrpc_lower_rqb
 *
 * function:       asynchronous allocation of a lower-RQB
 *
 * NOTE: not used
 *
 *===========================================================================*/

#if 0
LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_lower_rqb(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    CLRPC_LOWER_RQB_PTR_TYPE   lower_rqb_ptr
);
#endif

/*=============================================================================
 * function name:  clrpc_lower_mem
 *
 * function:       asynchronous allocation of a lower-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/

#if 0
LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_lower_mem(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    CLRPC_LOWER_MEM_PTR_TYPE   lower_mem_ptr
);
#endif

/*====  out functions  =====*/

/*=============================================================================
 * function name:  CLRPC_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to lower layer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_OPEN_CHANNEL_LOWER
LSA_VOID  CLRPC_LOWER_OUT_FCT_ATTR  CLRPC_OPEN_CHANNEL_LOWER(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to lower layer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_CLOSE_CHANNEL_LOWER
LSA_VOID  CLRPC_LOWER_OUT_FCT_ATTR  CLRPC_CLOSE_CHANNEL_LOWER(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to lower layer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_REQUEST_LOWER
LSA_VOID  CLRPC_LOWER_OUT_FCT_ATTR  CLRPC_REQUEST_LOWER(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_ALLOC_LOWER_RQB
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_ALLOC_LOWER_RQB(
    CLRPC_LOWER_RQB_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_FREE_LOWER_RQB
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_FREE_LOWER_RQB(
    LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_ALLOC_LOWER_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_ALLOC_LOWER_MEM(
    CLRPC_LOWER_MEM_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_FREE_LOWER_MEM
 *
 * function:       free a lower-memory
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_FREE_LOWER_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_FREE_LOWER_MEM(
    LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    CLRPC_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_LOWER_RQB_ERROR
 *
 * function:       notify a fatal-error
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_LOWER_RQB_ERROR
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_LOWER_RQB_ERROR(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_LOW_H */
