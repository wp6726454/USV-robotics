#ifndef ACP_LOW_H
#define ACP_LOW_H

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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_low.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Lower component interface                                                */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define ACP_LOWER_RQB_PTR_TYPE                  /* pointer to lower-RQB */ \
    ACP_LOWER_RQB_TYPE    ACP_LOWER_RQB_ATTR *


typedef EDD_RQB_OPEN_CHANNEL_TYPE				ACP_LOWER_MEM_ATTR	* ACP_LL_OPEN_CHANNEL_PTR_TYPE;
typedef EDD_RQB_GET_PARAMS_TYPE					ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_PARAM_PTR_TYPE;

typedef EDD_RQB_CSRT_IND_PROVIDE_TYPE			ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_IND_PROVIDE_PTR_TYPE;

typedef EDD_RQB_CSRT_PROVIDER_ADD_TYPE			ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_PROVIDER_ADD_PTR_TYPE;
typedef EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE		ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_PROVIDER_CONTROL_PTR_TYPE;
typedef EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE		ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_PROVIDER_REMOVE_PTR_TYPE;
typedef EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE	ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_PROVIDER_SET_STATE_PTR_TYPE;
typedef EDD_RQB_CSRT_PROVIDER_CHANGE_PHASE_TYPE	ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_PROVIDER_CHANGE_PHASE_PTR_TYPE;
typedef EDD_RQB_CSRT_PROVIDER_XCHANGE_BUF_TYPE	ACP_LOWER_MEM_ATTR  * ACP_LL_SRT_PROVIDER_XCHANGE_BUF_PTR_TYPE;

typedef EDD_RQB_CSRT_CONSUMER_ADD_TYPE			ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_CONSUMER_ADD_PTR_TYPE;
typedef EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE		ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_CONSUMER_CONTROL_PTR_TYPE;
typedef EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE		ACP_LOWER_MEM_ATTR	* ACP_LL_SRT_CONSUMER_REMOVE_PTR_TYPE;
typedef EDD_RQB_CSRT_CONSUMER_XCHANGE_BUF_TYPE	ACP_LOWER_MEM_ATTR  * ACP_LL_SRT_CONSUMER_XCHANGE_BUF_PTR_TYPE;

typedef EDD_RQB_CSRT_GET_APDU_STATUS_TYPE		ACP_LOWER_MEM_ATTR  * ACP_LL_GET_APDU_STATUS_PTR_TYPE;

typedef EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE	ACP_LOWER_MEM_ATTR  * ACP_LL_LINK_STATUS_PROVIDE_PTR_TYPE;

typedef EDD_RQB_RX_OVERLOAD_IND_PROVIDE_TYPE	ACP_LOWER_MEM_ATTR  * ACP_LL_RX_OVERLOAD_PROVIDE_PTR_TYPE;

typedef EDD_RQB_MULTICAST_TYPE					ACP_LOWER_MEM_ATTR  * ACP_LL_MULTICAST_PTR_TYPE;

typedef EDD_RQB_NRT_SEND_TYPE					ACP_LOWER_MEM_ATTR  * ACP_LL_NRT_SEND_PTR_TYPE;
typedef EDD_RQB_NRT_RECV_TYPE					ACP_LOWER_MEM_ATTR  * ACP_LL_NRT_RECV_PTR_TYPE;
typedef EDD_RQB_NRT_CANCEL_TYPE					ACP_LOWER_MEM_ATTR  * ACP_LL_NRT_CANCEL_PTR_TYPE;


typedef union {
	EDD_RQB_OPEN_CHANNEL_TYPE  ch_op;
	EDD_RQB_GET_PARAMS_TYPE  ch_para;

	EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE  link_ind;

	EDD_RQB_CSRT_IND_PROVIDE_TYPE  prov_ind;
	EDD_RQB_CSRT_PROVIDER_ADD_TYPE  prov_add;
	EDD_RQB_CSRT_CONSUMER_ADD_TYPE  cons_add;
	EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE  prov_ctrl;
	EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE  cons_ctrl;
	EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE  prov_rm;
	EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE  cons_rm;
	EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE  glob_prov_ctrl;
	EDD_RQB_CSRT_PROVIDER_CHANGE_PHASE_TYPE  prov_change_phase;
	EDD_RQB_CSRT_PROVIDER_XCHANGE_BUF_TYPE  prov_xbuf;
	EDD_RQB_CSRT_CONSUMER_XCHANGE_BUF_TYPE  cons_xbuf;

	EDD_RQB_CSRT_GET_APDU_STATUS_TYPE  apdu_status;

	EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE  link_status;

	EDD_RQB_MULTICAST_TYPE  multicast;

	EDD_RQB_NRT_SEND_TYPE  nrt_send;
	EDD_RQB_NRT_RECV_TYPE  nrt_recv;
	EDD_RQB_NRT_CANCEL_TYPE  nrt_cancel;

} ACP_LL_USED_DATA_TYPE;


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  acp_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of acp_open_channel(),
 *                 acp_close_channel() and acp_request()
 *
 * parameters:     ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see acp_open_channel(), acp_close_channel()
 *                        respectively acp_request()
 *===========================================================================*/
LSA_VOID  ACP_LOWER_IN_FCT_ATTR  acp_request_lower_done (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

#if 0 /* LSA-function, not used */
/*=============================================================================
 * function name:  acp_lower_rqb
 *
 * function:       asynchronous allocation of a lower-RQB
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of acp
 *                 LSA_UINT16                 length:         length of RQB
 *                 ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_lower_rqb (
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr
);

/*=============================================================================
 * function name:  acp_lower_mem
 *
 * function:       asynchronous allocation of a lower-memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of acp
 *                 LSA_UINT16                 length:         length of memory
 *                 ACP_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_lower_mem (
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_MEM_PTR_TYPE     lower_mem_ptr
);
#endif

/*====  out functions  =====*/

/*=============================================================================
 * function name:  ACP_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to lower layer
 *
 * parameters:     ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
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
 *     LSA_VOID  LSA_FCT_PTR(PREFIX_LOWER_IN_FCT_ATTR, prefix_request_lower_done_ptr)
 *                                    (PREFIX_LOWER_RQB_PTR_TYPE lower_rqb_ptr)
 *                                       pointer to callback-function
 *
 * RQB-return values via callback-function:
 *     LSA_UINT16         response:      *_RSP_OK
 *                                       *_RSP_ERR_SYNTAX
 *                                       *_RSP_ERR_RESOURCE
 *                                       *_RSP_ERR_SYS_PATH
 *                                       possibly:
 *                                       *_RSP_ERR_PARAM
 *                                       *_RSP_ERR_LOWER_LAYER
 *                                       or others
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of lower layer
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef ACP_OPEN_CHANNEL_LOWER
LSA_VOID  ACP_LOWER_OUT_FCT_ATTR  ACP_OPEN_CHANNEL_LOWER (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to lower layer
 *
 * parameters:     ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:  id of acp
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of acp
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef ACP_CLOSE_CHANNEL_LOWER
LSA_VOID  ACP_LOWER_OUT_FCT_ATTR  ACP_CLOSE_CHANNEL_LOWER (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to lower layer
 *
 * parameters:     ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:   id of acp
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of acp
 *     LSA_USER_ID_TYPE  user-id:   id of acp
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  *_RSP_ERR_REF
 *                                  *_RSP_ERR_SEQUENCE
 *                                  possibly:
 *                                  *_RSP_OK_EOM
 *                                  *_RSP_ERR_RESOURCE
 *                                  *_RSP_ERR_PARAM
 *                                  *_RSP_ERR_LOC_ABORT
 *                                  *_RSP_ERR_REM_ABORT
 *                                  *_RSP_ERR_LOC_NET
 *                                  *_RSP_ERR_REM_NET
 *                                  *_RSP_ERR_PROTOCOL
 *                                  *_RSP_ERR_TIMEOUT
 *                                  *_RSP_ERR_DEV
 *                                  *_RSP_ERR_ROUTE
 *                                  *_RSP_ERR_OWN_ADDR
 *                                  *_RSP_ERR_LOWER_LAYER
 *                                  *_RSP_ERR_WITHDRAW
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
#ifndef ACP_REQUEST_LOWER
LSA_VOID  ACP_LOWER_OUT_FCT_ATTR  ACP_REQUEST_LOWER (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB
 *
 * parameters:     ACP_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 acp_lower_rqb().
 *                 LSA_USER_ID_TYPE                   user_id:  id of acp
 *                 LSA_UINT32                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_ALLOC_LOWER_RQB
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_ALLOC_LOWER_RQB (
    ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT32                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *                 ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_FREE_LOWER_RQB
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_FREE_LOWER_RQB (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_LOWER_RQB_PTR_TYPE            lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory
 *
 * parameters:     ACP_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 acp_lower_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of acp
 *                 LSA_UINT32                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_ALLOC_LOWER_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_ALLOC_LOWER_MEM (
    ACP_LOWER_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT32                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_FREE_LOWER_MEM
 *
 * function:       free a lower-memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 ACP_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_FREE_LOWER_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_FREE_LOWER_MEM (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_LOW_H */
