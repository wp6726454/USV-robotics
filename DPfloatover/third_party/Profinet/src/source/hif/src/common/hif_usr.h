#ifndef HIF_USR_H
#define HIF_USR_H

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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*------------------------------------------------------------------------------
//	OPCODES
//	HIF_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/

/* System */
#define HIF_OPC_LD_UPPER_OPEN       1
#define HIF_OPC_LD_UPPER_CLOSE      2
#define HIF_OPC_LD_LOWER_OPEN       3
#define HIF_OPC_LD_LOWER_CLOSE      4
#define HIF_OPC_HD_UPPER_OPEN       5
#define HIF_OPC_HD_UPPER_CLOSE      6
#define HIF_OPC_HD_LOWER_OPEN       7
#define HIF_OPC_HD_LOWER_CLOSE      8

/* Internal */
#define HIF_OPC_POLL               16
#define HIF_OPC_TIMEOUT_POLL       17           
#define HIF_OPC_ISR                18
#define HIF_OPC_RECEIVE            19

/*------------------------------------------------------------------------------
//	ERROR_CODES
//	HIF_RQB_TYPE::response
//----------------------------------------------------------------------------*/

#define HIF_OK                     LSA_RSP_OK                  /* 0x01       */
#define HIF_ERR_RESOURCE           LSA_RSP_ERR_RESOURCE        /* 0x84 / 132 */
#define HIF_ERR_PARAM              LSA_RSP_ERR_PARAM           /* 0x86 / 134 */
#define HIF_ERR_SEQUENCE           LSA_RSP_ERR_SEQUENCE        /* 0x87 / 135 */
#define HIF_ERR_SYS_PATH           LSA_RSP_ERR_SYS_PATH        /* 0x91 / 145 */
#define HIF_ERR_ALLOC_LOCAL_MEM    (LSA_RSP_ERR_OFFSET + 0x01) /* 0xc1 / 193 */
#define HIF_ERR_OPCODE             (LSA_RSP_ERR_OFFSET + 0x02) /* 0xc2 / 194 */

/*-----------------------------------------------------------------------------
// Pipes
//---------------------------------------------------------------------------*/

/* HIF Internal Pipe  for LD and HD */
#define HIF_GLOB_ADMIN                            0

/* HIF LD Pipes */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_1           1 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_2           2 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_3           3 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_4           4 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_5           5 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_6           6 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_7           7 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SOCK_USER_CH_8           8 /* User Application Channel */
#define HIF_CPLD_GLO_APP_SNMPX_USER_CH           10 /* User Application Channel */
#define HIF_CPLD_GLO_APP_OHA_USER_CH             11 /* User Application Channel */
#define HIF_CPLD_GLO_APP_CLRPC_USER_CH           12 /* User Application Channel */
#define HIF_CPLD_IF_APP_CMPD_USER_CH             20 /* User Application Channel */
#define HIF_CPLD_IF_APP_IOC_CMCL_ACP_USER_CH     21 /* User Application Channel */
#define HIF_CPLD_IF_APP_IOM_CMMC_ACP_USER_CH     22 /* User Application Channel */
#define HIF_CPLD_IF_APP_IOD_CMSV_ACP_USER_CH     23 /* User Application Channel */
#define HIF_CPLD_IF_APP_IOH_USER_CH              24 /* User Application Channel */
#define HIF_CPLD_IF_APP_OHA_USER_CH              25 /* User Application Channel */
#define HIF_CPLD_IF_APP_EDD_SIMATIC_TIME_SYNC    26 /* User Application Channel */
#define HIF_CPLD_IF_APP_NARE_IP_TEST             27 /* User Application Channel */

/* HIF HD Pipes*/
#define HIF_CPHD_SYS_LTRC                         1
#define HIF_CPHD_IF_APP_CMPD_USER_CH              2 /* User Application Channel */
#define HIF_CPHD_IF_APP_IOC_CMCL_ACP_USER_CH      3 /* User Application Channel */
#define HIF_CPHD_IF_APP_IOM_CMMC_ACP_USER_CH      4 /* User Application Channel */
#define HIF_CPHD_IF_APP_IOD_CMSV_ACP_USER_CH      5 /* User Application Channel */
#define HIF_CPHD_IF_APP_IOH_USER_CH               6 /* User Application Channel */
#define HIF_CPHD_IF_SYS_CM_CLRPC                  7 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_TCIP_EDD_ARP              8
#define HIF_CPHD_IF_SYS_TCIP_EDD_ICMP             9
#define HIF_CPHD_IF_SYS_TCIP_EDD_UDP             10
#define HIF_CPHD_IF_SYS_TCIP_EDD_TCP             11
#define HIF_CPHD_IF_SYS_OHA_LLDP_EDD             12
#define HIF_CPHD_IF_SYS_OHA_MRP                  13
#define HIF_CPHD_IF_SYS_OHA_EDD                  14
#define HIF_CPHD_IF_SYS_OHA_DCP_EDD              15
#define HIF_CPHD_IF_SYS_OHA_NARE                 16
#define HIF_CPHD_IF_SYS_CM_OHA                   17 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_MRP_EDD                  18 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_DCP_EDD                  19 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_NARE_EDD                 20 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_NARE_DCP_EDD             21 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_GSY_EDD_SYNC             22 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_GSY_EDD_ANNO             23 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_ACP_EDD                  24 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_CM_EDD                   25 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_CM_MRP                   26 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_CM_POF_EDD               27 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_CM_GSY_CLOCK             28 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_IF_SYS_CM_NARE                  30 /* Pipe used by PSI LD Channel State Machine */
#define HIF_CPHD_L_IF_SYS_CM_OHA                 31
#define HIF_CPHD_L_IF_SYS_CM_CLRPC               32
#define HIF_CPHD_IF_APP_EDD_SIMATIC_TIME_SYNC    33 /* User Application Channel */
#define HIF_CPHD_IF_APP_NARE_IP_TEST             34 /* User Application Channel */

#if (HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS == 1)
#define HIF_CPHD_GLO_APP_SOCK_USER_CH_1          40  /* User Application Channel */
#define HIF_CPHD_GLO_APP_SOCK_USER_CH_2          41  /* User Application Channel */
#endif

/* User xD Pipe Offset */
#define HIF_CPXD_USR_PIPE_OFFSET                100

/*-----------------------------------------------------------------------------
// forward declarations / common types
//---------------------------------------------------------------------------*/

typedef struct hif_rqb_tag  *HIF_RQB_PTR_TYPE;

typedef LSA_VOID  * HIF_UPPER_MEM_PTR_TYPE;
typedef LSA_UINT8 * HIF_UPPER_MEM_U8_PTR_TYPE;
typedef LSA_UINT8 * HIF_LOWER_MEM_U8_PTR_TYPE;

#define HIF_HANDLE        LSA_UINT16
#define HIF_PIPE_ID_TYPE  LSA_UINT16

typedef  LSA_VOID  LSA_FCT_PTR( , HIF_UPPER_CALLBACK_FCT_PTR_TYPE)(
	LSA_VOID*  rb
);

typedef  LSA_VOID  LSA_FCT_PTR( , HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)(
	LSA_VOID*  rb
);

/*-----------------------------------------------------------------------------
// HIF PN Stack Config Params
//---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
typedef struct hif_hd_if_input_tag // All PNIO IF spezifc inputs for one HD IF
{
	HIF_IF_ID_TYPE        if_id;              // identifier for this interface
} HIF_HD_IF_INPUT_TYPE;
/*----------------------------------------------------------------------------*/
typedef struct hif_hd_input_tag // HD input configuration for one HD
{
	HIF_HD_ID_TYPE        hd_id;                  // HD ID
	LSA_UINT16            nr_of_if;               // nr of interfaces for this HD
	HIF_HD_IF_INPUT_TYPE  pnio_if[HIF_HD_MAX_IF];           // PNIO Interface setting

} HIF_HD_INPUT_TYPE;

/*------------------------------------------------------------------------------
// HIF_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

typedef struct hif_rqb_tag
{
    HIF_RQB_HEADER
    union
    {
        struct
        {
            HIF_SYS_HANDLE      hSysDev;                 //In:  upper device handle
            HIF_HANDLE          hH;                      //Out: reference to this HIF instance

            LSA_UINT32          hd_count;                //In:  used element count of hd_args[]
            HIF_HD_INPUT_TYPE   hd_args[HIF_LD_MAX_HD];  //In:  configuration of all HD

            HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE Cbf;        //In:  Callback function
        } dev_ld_open;
        struct
        {
            HIF_SYS_HANDLE      hSysDev;                 //In:  upper device handle        
            HIF_HANDLE          hH;                      //Out: reference to this HIF instance
            
            HIF_HD_INPUT_TYPE   hd_args;                 //In:  configuration of HD
            HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE Cbf;        //In:  Callback function
        } dev_hd_open;        
        struct
        {
            HIF_SYS_HANDLE      hSysDev;                 //In:  upper device handle        
            HIF_HANDLE          hH;                      //Out: reference to this HIF instance
            HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE Cbf;        //In:  Callback function       
        } dev_lower_open;        
        struct
        {
            HIF_HANDLE          hH;                      //In:  reference to the HIF instance to be closed
			HIF_HD_ID_TYPE      hd_id;                   //Out: hd_id of the device (HIF_OPC_HD_UPPER_CLOSE only)
            HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE Cbf;        //In:  Callback function         
        } dev_close;
        struct
        {
            struct
            {
                HIF_HANDLE      hHDest;
            } receive;
        } internal_req;
    } args;  
    HIF_RQB_TRAILER
} HIF_RQB_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/



/*====  in functions  =====*/

/*=============================================================================
 * function name:  hif_ld_u_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of user
 *     LSA_USER_ID_TYPE   user-id:       id of user
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (HIF_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
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
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_ld_u_open_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_ld_l_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of user
 *     LSA_USER_ID_TYPE   user-id:       id of user
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (HIF_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
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
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_ld_l_open_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_ld_u_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:  id of user
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_ld_u_close_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_ld_l_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:  id of user
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_ld_l_close_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_ld_u_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  hif_ld_u_request(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_ld_l_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  hif_ld_l_request(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_hd_u_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of user
 *     LSA_USER_ID_TYPE   user-id:       id of user
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (HIF_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
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
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_hd_u_open_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_hd_l_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of user
 *     LSA_USER_ID_TYPE   user-id:       id of user
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (HIF_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
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
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_hd_l_open_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_hd_u_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:  id of user
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_hd_u_close_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_hd_l_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:  id of user
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  hif_hd_l_close_channel(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_hd_u_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  hif_hd_u_request(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  hif_hd_l_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     HIF_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of hif
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  hif_hd_l_request(
    HIF_RQB_PTR_TYPE  upper_rqb_ptr
);

/*====  out functions  =====*/

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*        */
/*****************************************************************************/
#endif  /* of HIF_USR_H */
