
#ifndef OHA_LOW_H                        /* ----- reinclude-protection ----- */
#define OHA_LOW_H

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
/*  C o m p o n e n t     &C: OHA (Option HAndler)                      :C&  */
/*                                                                           */
/*  F i l e               &F: oha_low.h                                 :F&  */
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
/*  Defines constants, types, macros and prototyping for OHA                 */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/



/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/* LOWER LAYER MACROS to XXX, XXX = EDD, LLDP, DCP, NARE, SOCK, MRP          */
/*===========================================================================*/

/* EDD-RQB */
typedef EDD_RQB_TYPE    OHA_EDD_LOWER_RQB_TYPE;

typedef OHA_EDD_LOWER_RQB_TYPE   OHA_EDD_LOWER_RQB_ATTR *  OHA_EDD_LOWER_RQB_PTR_TYPE;

typedef EDD_RQB_OPEN_CHANNEL_TYPE    OHA_EDD_LOWER_MEM_ATTR * OHA_EDD_LOWER_RQB_OPEN_CHANNEL_PTR_TYPE;
typedef EDD_RQB_GET_PARAMS_TYPE      OHA_EDD_LOWER_MEM_ATTR * OHA_EDD_RQB_GET_PARAMS_PTR_TYPE;
typedef EDD_RQB_GET_PORT_PARAMS_TYPE OHA_EDD_LOWER_MEM_ATTR * OHA_EDD_RQB_GET_PORT_PARAMS_PTR_TYPE;
typedef EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE   OHA_EDD_LOWER_MEM_ATTR * OHA_EDD_RQB_LINK_STATUS_IND_PROVIDE_PTR_TYPE;
typedef EDD_RQB_NRT_SET_ARP_FILTER_TYPE OHA_EDD_LOWER_MEM_ATTR * OHA_EDD_RQB_SET_ARP_FILTER_PTR_TYPE;
typedef EDD_RQB_NRT_SET_DCP_FILTER_TYPE OHA_EDD_LOWER_MEM_ATTR * OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE;

/* NARE-RQB */
#ifndef OHA_CFG_NO_NARE
typedef NARE_RQB_TYPE    OHA_NARE_LOWER_RQB_TYPE;

typedef OHA_NARE_LOWER_RQB_TYPE   OHA_NARE_LOWER_RQB_ATTR *  OHA_NARE_LOWER_RQB_PTR_TYPE;
#endif

/* MRP-RQB */
#ifndef OHA_CFG_NO_MRP
typedef MRP_RQB_TYPE    OHA_MRP_LOWER_RQB_TYPE;

typedef OHA_MRP_LOWER_RQB_TYPE   OHA_MRP_LOWER_RQB_ATTR *  OHA_MRP_LOWER_RQB_PTR_TYPE;
#endif

/* SOCK-RQB */
typedef SOCK_RQB_TYPE    OHA_SOCK_LOWER_RQB_TYPE;

typedef OHA_SOCK_LOWER_RQB_TYPE   OHA_SOCK_LOWER_RQB_ATTR *  OHA_SOCK_LOWER_RQB_PTR_TYPE;

/* sock doesn't define the pointers */

typedef SOCK_RQB_ARGS_CHANNEL_TYPE      OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_OPEN_CHANNEL_PTR_TYPE;
typedef SOCK_RQB_ARGS_UDP_OPEN_TYPE     OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_UDP_OPEN_PTR_TYPE;
typedef SOCK_RQB_ARGS_CLOSE_TYPE        OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_UDP_CLOSE_PTR_TYPE;
typedef SOCK_RQB_ARGS_DATA_TYPE         OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_UDP_DATA_PTR_TYPE;

/* typedef SOCK_RQB_ARGS_GETHOSTBYNAME_TYPE   OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_GETHOSTBYNAME_PTR_TYPE; */
typedef SOCK_RQB_ARGS_SNMP_ACTIVATE_TYPE   OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_SNMP_ACTIVATE_PTR_TYPE;
typedef SOCK_RQB_ARGS_SNMP_DEACTIVATE_TYPE OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_SNMP_DEACTIVATE_PTR_TYPE;
typedef SOCK_RQB_ARGS_SNMP_RECEIVE_TYPE    OHA_SOCK_LOWER_MEM_ATTR *OHA_SOCK_SNMP_RECEIVE_PTR_TYPE;


/* SNMPX-RQB */
#ifdef OHA_CFG_USE_SNMPX
typedef SNMPX_RQB_TYPE    OHA_SNMPX_LOWER_RQB_TYPE;

typedef OHA_SNMPX_LOWER_RQB_TYPE   OHA_SNMPX_LOWER_RQB_ATTR *  OHA_SNMPX_LOWER_RQB_PTR_TYPE;
#endif

/* DCP-RQB */
typedef DCP_RQB_TYPE    OHA_DCP_LOWER_RQB_TYPE;

typedef OHA_DCP_LOWER_RQB_TYPE   OHA_DCP_LOWER_RQB_ATTR *  OHA_DCP_LOWER_RQB_PTR_TYPE;

/* LLDP-RQB */
typedef LLDP_RQB_TYPE    OHA_LLDP_LOWER_RQB_TYPE;

typedef OHA_LLDP_LOWER_RQB_TYPE   OHA_LLDP_LOWER_RQB_ATTR *  OHA_LLDP_LOWER_RQB_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/



/*===========================================================================*/
/*====   in functions from XXX, XXX = EDD, LLDP, DCP, NARE, SOCK, MRP    ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  oha_xxx_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of OHA-lower request to XXX:
 *
 * parameters:     OHA_XXX_LOWER_RQB_PTR_TYPE  pRQB   pointer to lower-
 *                                                     RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see xxx_open_channel(), xxx_close_channel()
 *                    respectively xxx_request()
 *===========================================================================*/

LSA_VOID  OHA_EDD_LOWER_IN_FCT_ATTR  oha_edd_request_lower_done(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB
);

#ifndef OHA_CFG_NO_NARE
LSA_VOID  OHA_NARE_LOWER_IN_FCT_ATTR  oha_nare_request_lower_done(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB
);
#endif

#ifndef OHA_CFG_NO_MRP
LSA_VOID  OHA_MRP_LOWER_IN_FCT_ATTR  oha_mrp_request_lower_done(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB
);
#endif

LSA_VOID  OHA_SOCK_LOWER_IN_FCT_ATTR  oha_sock_request_lower_done(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB
);

#ifdef OHA_CFG_USE_SNMPX
LSA_VOID  OHA_SNMPX_LOWER_IN_FCT_ATTR  oha_snmpx_request_lower_done(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB
);
#endif

LSA_VOID  OHA_DCP_LOWER_IN_FCT_ATTR  oha_dcp_request_lower_done(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB
);

LSA_VOID  OHA_LLDP_LOWER_IN_FCT_ATTR  oha_lldp_request_lower_done(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB
);

/*===========================================================================*/
/*====   out functions to XXX, XXX = EDD, LLDP, DCP, NARE, SOCK, MRP     ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  OHA_XXX_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to XXX lower layer
 *
 * parameters:     OHA_XXX_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
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
 *                                    (OHA_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
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
#ifndef OHA_EDD_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_EDD_LOWER_OUT_FCT_ATTR  OHA_EDD_OPEN_CHANNEL_LOWER(
    OHA_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef OHA_CFG_NO_NARE
#ifndef OHA_NARE_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_NARE_LOWER_OUT_FCT_ATTR  OHA_NARE_OPEN_CHANNEL_LOWER(
    OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_MRP
#ifndef OHA_MRP_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_MRP_LOWER_OUT_FCT_ATTR  OHA_MRP_OPEN_CHANNEL_LOWER(
    OHA_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif
#endif

#ifndef OHA_SOCK_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_SOCK_LOWER_OUT_FCT_ATTR  OHA_SOCK_OPEN_CHANNEL_LOWER(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifdef OHA_CFG_USE_SNMPX
#ifndef OHA_SNMPX_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_SNMPX_LOWER_OUT_FCT_ATTR  OHA_SNMPX_OPEN_CHANNEL_LOWER(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE              sys_ptr
);
#endif
#endif

#ifndef OHA_DCP_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_DCP_LOWER_OUT_FCT_ATTR  OHA_DCP_OPEN_CHANNEL_LOWER(
    OHA_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef OHA_LLDP_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_LLDP_LOWER_OUT_FCT_ATTR  OHA_LLDP_OPEN_CHANNEL_LOWER(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_XXX_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to XXX lower layer
 *
 * parameters:     OHA_XXX_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
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
#ifndef OHA_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_EDD_LOWER_OUT_FCT_ATTR  OHA_EDD_CLOSE_CHANNEL_LOWER(
    OHA_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef OHA_CFG_NO_NARE
#ifndef OHA_NARE_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_NARE_LOWER_OUT_FCT_ATTR  OHA_NARE_CLOSE_CHANNEL_LOWER(
    OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_MRP
#ifndef OHA_MRP_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_MRP_LOWER_OUT_FCT_ATTR  OHA_MRP_CLOSE_CHANNEL_LOWER(
    OHA_MRP_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif
#endif

#ifndef OHA_SOCK_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_SOCK_LOWER_OUT_FCT_ATTR  OHA_SOCK_CLOSE_CHANNEL_LOWER(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifdef OHA_CFG_USE_SNMPX
#ifndef OHA_SNMPX_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_SNMPX_LOWER_OUT_FCT_ATTR  OHA_SNMPX_CLOSE_CHANNEL_LOWER(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE              sys_ptr
);
#endif
#endif

#ifndef OHA_DCP_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_DCP_LOWER_OUT_FCT_ATTR  OHA_DCP_CLOSE_CHANNEL_LOWER(
    OHA_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef OHA_LLDP_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_LLDP_LOWER_OUT_FCT_ATTR  OHA_LLDP_CLOSE_CHANNEL_LOWER(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_XXX_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to XXX lower layer
 *
 * parameters:     OHA_XXX_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
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
#ifndef OHA_EDD_REQUEST_LOWER
LSA_VOID  OHA_EDD_LOWER_OUT_FCT_ATTR  OHA_EDD_REQUEST_LOWER(
    OHA_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef OHA_CFG_NO_NARE
#ifndef OHA_NARE_REQUEST_LOWER
LSA_VOID  OHA_NARE_LOWER_OUT_FCT_ATTR  OHA_NARE_REQUEST_LOWER(
    OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_MRP
#ifndef OHA_MRP_REQUEST_LOWER
LSA_VOID  OHA_MRP_LOWER_OUT_FCT_ATTR  OHA_MRP_REQUEST_LOWER(
    OHA_MRP_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif
#endif

#ifndef OHA_SOCK_REQUEST_LOWER
LSA_VOID  OHA_SOCK_LOWER_OUT_FCT_ATTR  OHA_SOCK_REQUEST_LOWER(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifdef OHA_CFG_USE_SNMPX
#ifndef OHA_SNMPX_REQUEST_LOWER
LSA_VOID  OHA_SNMPX_LOWER_OUT_FCT_ATTR  OHA_SNMPX_REQUEST_LOWER(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif
#endif

#ifndef OHA_DCP_REQUEST_LOWER
LSA_VOID  OHA_DCP_LOWER_OUT_FCT_ATTR  OHA_DCP_REQUEST_LOWER(
    OHA_DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef OHA_LLDP_REQUEST_LOWER
LSA_VOID  OHA_LLDP_LOWER_OUT_FCT_ATTR  OHA_LLDP_REQUEST_LOWER(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_XXX_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB for XXX synchronously
 *
 * parameters:     OHA_XXX_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_EDD_ALLOC_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_EDD_ALLOC_LOWER_RQB(
    OHA_EDD_LOWER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

#ifndef OHA_CFG_NO_NARE
#ifndef OHA_NARE_ALLOC_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_NARE_ALLOC_LOWER_RQB(
    OHA_NARE_LOWER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_MRP
#ifndef OHA_MRP_ALLOC_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_MRP_ALLOC_LOWER_RQB(
    OHA_MRP_LOWER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif
#endif

#ifndef OHA_DCP_ALLOC_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DCP_ALLOC_LOWER_RQB(
    OHA_DCP_LOWER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *   lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

#ifndef OHA_SOCK_ALLOC_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SOCK_ALLOC_LOWER_RQB(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

#ifdef OHA_CFG_USE_SNMPX
#ifndef OHA_SNMPX_ALLOC_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SNMPX_ALLOC_LOWER_RQB(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif
#endif

#ifndef OHA_LLDP_ALLOC_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_LLDP_ALLOC_LOWER_RQB(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_XXX_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB from XXX
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *                 OHA_XXX_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_EDD_FREE_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_EDD_FREE_LOWER_RQB(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_EDD_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

#ifndef OHA_CFG_NO_NARE
#ifndef OHA_NARE_FREE_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_NARE_FREE_LOWER_RQB(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_NARE_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_MRP
#ifndef OHA_MRP_FREE_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_MRP_FREE_LOWER_RQB(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_MRP_LOWER_RQB_PTR_TYPE                            lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif
#endif

#ifndef OHA_DCP_FREE_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DCP_FREE_LOWER_RQB(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_DCP_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

#ifndef OHA_SOCK_FREE_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SOCK_FREE_LOWER_RQB(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_SOCK_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

#ifdef OHA_CFG_USE_SNMPX
#ifndef OHA_SNMPX_FREE_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SNMPX_FREE_LOWER_RQB(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_SNMPX_LOWER_RQB_PTR_TYPE                          lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif
#endif

#ifndef OHA_LLDP_FREE_LOWER_RQB
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_LLDP_FREE_LOWER_RQB(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_LLDP_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_XXX_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory for EDD synchronously
 *
 * parameters:     OHA_XXX_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
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
#ifndef OHA_EDD_ALLOC_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_EDD_ALLOC_LOWER_MEM(
    OHA_EDD_LOWER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

#ifndef OHA_DCP_ALLOC_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DCP_ALLOC_LOWER_MEM(
    OHA_DCP_LOWER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

#ifndef OHA_SOCK_ALLOC_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SOCK_ALLOC_LOWER_MEM(
    OHA_SOCK_LOWER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

#ifdef OHA_CFG_USE_SNMPX
#ifndef OHA_SNMPX_ALLOC_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SNMPX_ALLOC_LOWER_MEM(
    OHA_SNMPX_LOWER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_NARE
#ifndef OHA_NARE_ALLOC_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_NARE_ALLOC_LOWER_MEM(
    OHA_NARE_LOWER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_MRP
#ifndef OHA_MRP_ALLOC_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_MRP_ALLOC_LOWER_MEM(
    OHA_MRP_LOWER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif
#endif

#ifndef OHA_LLDP_ALLOC_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_LLDP_ALLOC_LOWER_MEM(
    OHA_LLDP_LOWER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_XXX_FREE_LOWER_MEM
 *
 * function:       free a lower-memory for XXX
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 OHA_EDD_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_EDD_FREE_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_EDD_FREE_LOWER_MEM(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_EDD_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

#ifndef OHA_CFG_NO_NARE
#ifndef OHA_NARE_FREE_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_NARE_FREE_LOWER_MEM(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_NARE_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif
#endif

#ifndef OHA_CFG_NO_MRP
#ifndef OHA_MRP_FREE_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_MRP_FREE_LOWER_MEM(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_MRP_LOWER_MEM_PTR_TYPE                            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif
#endif

#ifndef OHA_DCP_FREE_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DCP_FREE_LOWER_MEM(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_DCP_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

#ifndef OHA_SOCK_FREE_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SOCK_FREE_LOWER_MEM(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_SOCK_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

#ifdef OHA_CFG_USE_SNMPX
#ifndef OHA_SNMPX_FREE_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_SNMPX_FREE_LOWER_MEM(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_SNMPX_LOWER_MEM_PTR_TYPE                          lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif
#endif

#ifndef OHA_LLDP_FREE_LOWER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_LLDP_FREE_LOWER_MEM(
    LSA_UINT16                     OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_LLDP_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*****************************************************************************/
/*  end of file OHA_LOW.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_LOW_H */
