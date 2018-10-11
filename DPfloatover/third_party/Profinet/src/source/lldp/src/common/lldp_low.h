
#ifndef LLDP_LOW_H                   /* ----- reinclude-protection ----- */
#define LLDP_LOW_H

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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_low.h                                :F&  */
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
/*  Defines constants, types, macros and prototyping for LLDP                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  06.04.04    JS    initial version.                                       */
#endif
/*****************************************************************************/



/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/* LOWER LAYER MACROS to EDD                                                 */
/*===========================================================================*/

/* RQB */
typedef EDD_RQB_TYPE    LLDP_EDD_LOWER_RQB_TYPE;

typedef LLDP_EDD_LOWER_RQB_TYPE   LLDP_EDD_LOWER_RQB_ATTR *  LLDP_EDD_LOWER_RQB_PTR_TYPE;

typedef EDD_RQB_OPEN_CHANNEL_TYPE              LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE;
typedef EDD_RQB_NRT_SEND_TYPE                  LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_LOWER_RQB_NRT_SEND_PTR_TYPE;
typedef EDD_RQB_NRT_RECV_TYPE                  LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_LOWER_RQB_NRT_RECV_PTR_TYPE;
typedef EDD_RQB_NRT_CANCEL_TYPE                LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_LOWER_RQB_NRT_CANCEL_PTR_TYPE;
typedef EDD_RQB_GET_PARAMS_TYPE                LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_RQB_GET_PARAMS_PTR_TYPE;
typedef EDD_RQB_GET_PORT_PARAMS_TYPE           LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE;
typedef EDD_RQB_MULTICAST_TYPE                 LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_RQB_MULTICAST_PTR_TYPE;

typedef EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE   LLDP_EDD_LOWER_MEM_ATTR * LLDP_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/



/*===========================================================================*/
/*====                     in functions from EDD                         ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  lldp_edd_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of LLDP-lower request to EDD:
 *
 * parameters:     LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB   pointer to lower-
 *                                                     RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see edd_open_channel(), edd_close_channel()
 *                    respectively edd_request()
 *===========================================================================*/

LSA_VOID  LLDP_EDD_LOWER_IN_FCT_ATTR  lldp_edd_request_lower_done(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  pRQB
);


/*===========================================================================*/
/*====                    out functions to EDD                           ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  LLDP_EDD_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to EDD lower layer
 *
 * parameters:     LLDP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
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
#ifndef LLDP_EDD_OPEN_CHANNEL_LOWER
LSA_VOID  LLDP_EDD_LOWER_OUT_FCT_ATTR  LLDP_EDD_OPEN_CHANNEL_LOWER(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to EDD lower layer
 *
 * parameters:     LLDP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
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
#ifndef LLDP_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID  LLDP_EDD_LOWER_OUT_FCT_ATTR  LLDP_EDD_CLOSE_CHANNEL_LOWER(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to EDD lower layer
 *
 * parameters:     LLDP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
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
#ifndef LLDP_EDD_REQUEST_LOWER
LSA_VOID  LLDP_EDD_LOWER_OUT_FCT_ATTR  LLDP_EDD_REQUEST_LOWER(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB for EDD synchronously
 *
 * parameters:     LLDP_EDD_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_EDD_ALLOC_LOWER_RQB
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_ALLOC_LOWER_RQB(
    LLDP_EDD_LOWER_RQB_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB from EDD
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *                 LLDP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_EDD_FREE_LOWER_RQB
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_FREE_LOWER_RQB(
    LSA_UINT16                     LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LLDP_EDD_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory for EDD synchronously
 *
 * parameters:     LLDP_EDD_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
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
#ifndef LLDP_EDD_ALLOC_LOWER_MEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_ALLOC_LOWER_MEM(
    LLDP_EDD_LOWER_MEM_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_FREE_LOWER_MEM
 *
 * function:       free a lower-memory for EDD
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 LLDP_EDD_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_EDD_FREE_LOWER_MEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_FREE_LOWER_MEM(
    LSA_UINT16                     LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LLDP_EDD_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif


/*=============================================================================
 * function name:  LLDP_EDD_ALLOC_LOWER_TXMEM
 *
 * function:       allocate a lower memory for send PDUs
 *
 *                 This memory has to be DWORD-aligned!!
 *
 * parameters:     LLDP_EDD_LOWER_MEM_PTR_TYPE  ...  * lower_mem_ptr_ptr:
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
#ifndef LLDP_EDD_ALLOC_LOWER_TXMEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_ALLOC_LOWER_TXMEM(
    LLDP_EDD_LOWER_MEM_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_FREE_LOWER_TXMEM
 *
 * function:       free a lower memory for send PDUs
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 LLDP_EDD_LOWER_MEM_PTR_TYPE  lower_mem_ptr:pointer to lower-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:      system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_EDD_FREE_LOWER_TXMEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_FREE_LOWER_TXMEM(
    LSA_UINT16                 LLDP_LOCAL_MEM_ATTR *    ret_val_ptr,
    LLDP_EDD_LOWER_MEM_PTR_TYPE                         lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif


/*=============================================================================
 * function name:  LLDP_EDD_ALLOC_LOWER_RXMEM
 *
 * function:       allocate a lower memory for recv PDUs
 *
 *                 This memory has to be DWORD-aligned!!
 *
 * parameters:     LLDP_EDD_LOWER_MEM_PTR_TYPE  ...  * lower_mem_ptr_ptr:
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
#ifndef LLDP_EDD_ALLOC_LOWER_RXMEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_ALLOC_LOWER_RXMEM(
    LLDP_EDD_LOWER_MEM_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                  user_id,
    LSA_UINT16                                        length,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_EDD_FREE_LOWER_RXMEM
 *
 * function:       free a lower memory for recv PDUs
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 LLDP_EDD_LOWER_MEM_PTR_TYPE  lower_mem_ptr:pointer to lower-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:      system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_EDD_FREE_LOWER_RXMEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EDD_FREE_LOWER_RXMEM(
    LSA_UINT16                 LLDP_LOCAL_MEM_ATTR *    ret_val_ptr,
    LLDP_EDD_LOWER_MEM_PTR_TYPE                         lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif


/*****************************************************************************/
/*  end of file LLDP_LOW.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of LLDP_LOW_H */
