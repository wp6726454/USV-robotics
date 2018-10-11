#ifndef CM_SYS_H
#define CM_SYS_H

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
/*  F i l e               &F: cm_sys.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef LSA_FATAL_ERROR_TYPE  CM_LOCAL_MEM_ATTR  *CM_FATAL_ERROR_PTR_TYPE; /* pointer to LSA_FATAL_ERROR */


typedef LSA_VOID  CM_UPPER_MEM_ATTR  *CM_UPPER_MEM_PTR_TYPE; /* pointer to upper-memory */

typedef LSA_VOID  CM_LOCAL_MEM_ATTR  *CM_LOCAL_MEM_PTR_TYPE; /* pointer to local-memory */


typedef LSA_VOID  CM_ACP_LOWER_MEM_ATTR  *CM_ACP_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CM_EDD_LOWER_MEM_ATTR  *CM_EDD_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CM_GSY_LOWER_MEM_ATTR  *CM_GSY_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CM_NARE_LOWER_MEM_ATTR  *CM_NARE_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CM_OHA_LOWER_MEM_ATTR  *CM_OHA_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CM_RPC_LOWER_MEM_ATTR  *CM_RPC_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CM_MRP_LOWER_MEM_ATTR  *CM_MRP_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CM_POF_LOWER_MEM_ATTR  *CM_POF_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */


/*------------------------------------------------------------------------------
// cm details
//----------------------------------------------------------------------------*/

enum cm_path_type_enum {

	CM_PATH_TYPE_EDD       =  0,  /* the system path to EDD */

#if CM_CFG_USE_MRP
	CM_PATH_TYPE_MRP       =  1,  /* the system path to MRP */
#endif

#if CM_CFG_USE_GSY
	CM_PATH_TYPE_GSY       =  2,  /* the system path to GSY */
#endif

#if CM_CFG_USE_POF
	CM_PATH_TYPE_POF       =  3,  /* the system path to POF */
#endif

	CM_PATH_TYPE_OHA       =  4,  /* the system path to OHA */

	CM_PATH_TYPE_5         =  5,  /* defunct, was system path to REMA */

#if CM_CFG_USE_NARE
	CM_PATH_TYPE_NARE      =  6,  /* the system path to NARE */
#endif

	CM_PATH_TYPE_RPC       =  7,  /* the system path to CLRPC */

	CM_PATH_TYPE_PD        =  8,  /* a user path to a CM-PD */

	CM_PATH_TYPE_CLIENT    =  9,  /* a user path to a CM client */
	CM_PATH_TYPE_MULTICAST = 10,  /* a user path to a MC-channel */
	CM_PATH_TYPE_SERVER    = 11,  /* a user path to a CM server */

	CM_PATH_TYPE_ANY_MAX          /* for array-size */
};

#define CM_PATH_TYPE_SYS_MAX  (CM_PATH_TYPE_RPC)  /* ref to last system-channel, internally used */


#define CM_MAX_EDD  (CM_CFG_MAX_NICS)

#if CM_CFG_USE_MRP
#define CM_MAX_MRP  (CM_CFG_MAX_NICS)
#else
#define CM_MAX_MRP  (0)
#endif

#if CM_CFG_USE_GSY
#define CM_MAX_GSY  (CM_CFG_MAX_PTCPSUBDOMAINS * CM_CFG_MAX_NICS)
#else
#define CM_MAX_GSY  (0)
#endif

#if CM_CFG_USE_POF
#define CM_MAX_POF  (CM_CFG_MAX_NICS)
#else
#define CM_MAX_POF  (0)
#endif

#define CM_MAX_OHA  (CM_CFG_MAX_NICS)

#if CM_CFG_USE_NARE
#define CM_MAX_NARE (CM_CFG_MAX_NICS)
#else
#define CM_MAX_NARE (0)
#endif

#define CM_MAX_RPC  (CM_CFG_MAX_NICS)

#define CM_MAX_PD   (CM_CFG_MAX_NICS)

#define CM_MAX_CHANNELS \
	( CM_MAX_EDD \
	+ CM_MAX_MRP \
	+ CM_MAX_GSY \
	+ CM_MAX_POF \
	+ CM_MAX_OHA \
	+ CM_MAX_NARE \
	+ CM_MAX_RPC \
	+ CM_MAX_PD \
	+ CM_CFG_MAX_CLIENTS \
	+ CM_CFG_MAX_MULTICAST_CHANNELS \
	+ CM_CFG_MAX_SERVERS \
	)

typedef struct cm_detail_tag {

	LSA_UINT8  path_type; /* see cm_path_type_enum */
	LSA_UINT8  nic_id;    /* an ID that defines the NIC (network connector) at the lower end of the path */

	/***/

	union cm_detail_path_type_tag {

		struct cm_detail_edd_tag { /* for CM_PATH_TYPE_EDD */

			LSA_UINT8 nothing_so_far;
		} edd;

		struct cm_detail_mrp_tag { /* for CM_PATH_TYPE_MRP */

			LSA_UINT8 nothing_so_far;
		} mrp;

		struct cm_detail_gsy_tag { /* for CM_PATH_TYPE_GSY */

			LSA_UINT8 nothing_so_far;
		} gsy;

		struct cm_detail_pof_tag { /* for CM_PATH_TYPE_POF */

			LSA_UINT8 nothing_so_far;
		} pof;

		struct cm_detail_oha_tag { /* for CM_PATH_TYPE_OHA */

			LSA_UINT16  hello_resource_count;
				/*
				 * number of hello-indication resources (OHA_OPC_HELLO_INDICATION)
				 * - IO Controller with 'fast startup' (see PNIO-Spec)
				 *     value := same as OHA_CDB_PATH_PARAMS_TYPE::Dcp.NumOfRecvFSU
				 *     note that if 'fast startup' is enabled, 'accelerated mode' (see u.nare) should be enabled too!
				 * - IO Controller without 'fast startup'
				 *     value := 0
				 * - IO Device:
				 *     value := 0
				 */
		} oha;

		struct cm_detail_nare_tag { /* for CM_PATH_TYPE_NARE */

			LSA_UINT16 resolve_notify_resource_count;
				/*
				 * number of resolve-notify-indication resources (NARE_OPC_RESOLVE_CONFIG_NOTIFY)
				 * - IO Controller with 'accelerated mode' (see PNIO-Spec)
				 *     value := same as CM_DETAIL_TYPE::u.cl.max_connects_per_second
				 * - IO Controller without 'accelerated mode'
				 *     value := 0
				 * - IO Device:
				 *     value := 0
				 */
		} nare;

		struct cm_detail_clrpc_tag { /* for CM_PATH_TYPE_RPC */

			LSA_UINT16  recv_resource_count;
				/*
				 * resources for the clrpc-client (see the clrpc-doc)
				 */

		} rpc;

		struct cm_detail_pd_tag { /* for CM_PATH_TYPE_PD */

			struct {
				/*
				 * the cm-pd creates a clrpc-server for the endpoint-mapper interface (AP00706078)
				 */

				LSA_UINT16  recv_resource_count;
				LSA_UINT16  send_resource_count;
					/*
					 * resources for the clrpc-server (see the clrpc-doc, registering interfaces)
					 *
					 * note: alloc_len and call_resource_count are determined internally
					 */
			} epm;
		} pd;



		struct cm_detail_cl_tag { /* for CM_PATH_TYPE_CLIENT */

			struct {
				/*
				 * each cm-client creates...
				 * - a clrpc-server for the IPNIO_Controller interface
				 * - and a clrpc-client for each AR (Connect, Release, ...)
				 */

				LSA_UINT32  alloc_len;
				LSA_UINT16  call_resource_count;
				LSA_UINT16  recv_resource_count;
				LSA_UINT16  send_resource_count;
					/*
					 * resources for the clrpc-server (see the clrpc-doc, registering interfaces)
					 *
					 * note: the parameter server_udp_port was removed with V4.0i80.5 (AP00619205)
					 *       the port is now always selected by the ip-stack (server_udp_port = 0)
					 */
			} rpc;

			struct {

				LSA_UINT16 state_ind_resource_count;
				LSA_UINT16 alarm_high_prio_ind_resource_count;
				LSA_UINT16 alarm_low_prio_ind_resource_count;
				LSA_UINT16 max_alarm_data_length; /* must match with CM_OPC_CL_CREATE::max_alarm_data_length (see AP00342171) */
					/*
					 * indication resources for acp (see the acp-doc)
					 */
			} acp;

			LSA_UINT16 max_connects_per_second;
			LSA_UINT16 min_reconnect_timeout; /* in seconds */
			LSA_UINT16 schedule_timeout; /* in milli-seconds (~200 msec) */
				/*
				 * "max_connects_per_second" simultaneously running IPNIO-Connects.
				 * a failed connect waits "min_reconnect_timeout" before reconnecting.
				 * a schedule decision is taken every "schedule_timeout".
				 */
		} cl;

		struct cm_detail_mc_tag { /* for CM_PATH_TYPE_MULTICAST */

			LSA_UINT16 state_ind_resource_count;
				/*
				 * indication resources for acp (see the acp-doc)
				 */
		} mc;

		struct cm_detail_sv_tag { /* for CM_PATH_TYPE_SERVER */

			struct {
				/*
				 * each cm-server creates...
				 * - a clrpc-server for the IPNIO_Device interface
				 * - and a clrpc-client for each AR (Appl-Ready callback)
				 */

				LSA_UINT32  alloc_len;
				LSA_UINT16  call_resource_count;
				LSA_UINT16  recv_resource_count;
				LSA_UINT16  send_resource_count;
					/*
					 * resources for the clrpc-server (see the clrpc-doc, registering interfaces)
					 *
					 * note: the parameter server_udp_port was removed with V4.0i80.5 (AP00619205)
					 *       the port is now always selected by the ip-stack (server_udp_port = 0)
					 */
			} rpc;

			struct {

				LSA_UINT16 state_ind_resource_count;
				LSA_UINT16 alarm_high_prio_ind_resource_count;
				LSA_UINT16 alarm_low_prio_ind_resource_count;
				LSA_UINT16 max_alarm_data_length;
					/*
					 * indication resources for acp (see the acp-doc)
					 */
			} acp;

			/* note: the parameter multi_api_device was removed with V5.3i1.10 (AP01499644) */

		} sv;
	} u;

} CM_DETAIL_TYPE;

typedef CM_DETAIL_TYPE  CM_SYSTEM_MEM_ATTR  *CM_DETAIL_PTR_TYPE;

/*------------------------------------------------------------------------------
// cm init-parameters
//----------------------------------------------------------------------------*/

typedef struct cm_init_tag {

	LSA_UINT16 oneshot_lsa_time_base;   /* oneshot-timer, see cm_oneshot_sys_init() */
	LSA_UINT16 oneshot_lsa_time_factor; /* oneshot-timer, see cm_oneshot_sys_init() */

} CM_INIT_TYPE;

typedef CM_INIT_TYPE CM_SYSTEM_MEM_ATTR * CM_INIT_PTR_TYPE;


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/


/*====  in functions  =====*/


/*=============================================================================
 * function name:  cm_init
 *
 * function:       initialization
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
LSA_UINT16  CM_SYSTEM_IN_FCT_ATTR  cm_init(
	CM_INIT_PTR_TYPE  init
);


/*=============================================================================
 * function name:  cm_undo_init
 *
 * function:       undo the initialization
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
LSA_UINT16  CM_SYSTEM_IN_FCT_ATTR  cm_undo_init(
    LSA_VOID
);


/*=============================================================================
 * function name:  cm_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR  cm_timeout(
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE  user_id
);


/*=============================================================================
 * function name:  cm_fatal, cm_fatal1
 *
 * function:       undocumented, not in cm_int.h because of cm_cfg.c
 *===========================================================================*/
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR
cm_fatal(
	LSA_UINT16 cm_module_id,
	LSA_INT line
);

#ifndef CM_FATAL /* AP00289268, note: returning from fatal is not allowed */
#define CM_FATAL()  cm_fatal(CM_MODULE_ID, __LINE__)
#endif


LSA_VOID  CM_SYSTEM_IN_FCT_ATTR
cm_fatal1(
	LSA_UINT16 cm_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
);

#ifndef CM_FATAL1 /* AP00289268, note: returning from fatal is not allowed */
#define CM_FATAL1(ec_0)       cm_fatal1 (CM_MODULE_ID, __LINE__, ec_0)
#endif


/*====  out functions  =====*/

/*=============================================================================
 * function name:  CM_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_GET_PATH_INFO
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GET_PATH_INFO(
    LSA_UINT16          CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE    CM_LOCAL_MEM_ATTR *  sys_ptr_ptr,
    CM_DETAIL_PTR_TYPE  CM_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                        path
);
#endif


/*=============================================================================
 * function name:  CM_RELEASE_PATH_INFO
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_RELEASE_PATH_INFO
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_RELEASE_PATH_INFO(
    LSA_UINT16             CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                            sys_ptr,
    CM_DETAIL_PTR_TYPE                          detail_ptr
);
#endif


/*=============================================================================
 * function name:  CM_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_ALLOC_LOCAL_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ALLOC_LOCAL_MEM(
    CM_LOCAL_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
    LSA_UINT16                                  length
);
#endif


/*=============================================================================
 * function name:  CM_FREE_LOCAL_MEM
 *
 * function:       free a local-memory
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_FREE_LOCAL_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_FREE_LOCAL_MEM(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    CM_LOCAL_MEM_PTR_TYPE                           local_mem_ptr
);
#endif


/*=============================================================================
 * function name:  CM_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower-memory to upper-memory
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used, #ifndef CM_LOWER_TO_UPPER_MEM */
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR  CM_LOWER_TO_UPPER_MEM(
    CM_UPPER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
    CM_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                            sys_ptr
);
#endif


/*=============================================================================
 * function name:  CM_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper-memory to lower-memory
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used, #ifndef CM_UPPER_TO_LOWER_MEM */
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR  CM_UPPER_TO_LOWER_MEM(
    CM_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    CM_UPPER_MEM_PTR_TYPE                       upper_mem_ptr,
    LSA_SYS_PTR_TYPE                            sys_ptr
);
#endif


/*=============================================================================
 * function name:  CM_LOWER_TO_LOWER_MEM
 *
 * function:       convert lower-memory to lower-memory
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used, #ifndef CM_LOWER_TO_LOWER_MEM */
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR  CM_LOWER_TO_LOWER_MEM(
    CM_LOWER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    CM_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
    LSA_SYS_PTR_TYPE                            src_sys_ptr,
    LSA_SYS_PTR_TYPE                            dst_sys_ptr
);
#endif


/*=============================================================================
 * function name:  CM_COPY_COMMON_MEM
 *
 * function:       copy common memory
 *                 The memory areas of source and destination mustn't overlap.
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used, #ifndef CM_COPY_COMMON_MEM */
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR  CM_COPY_COMMON_MEM(
    LSA_VOID  CM_COMMON_MEM_ATTR *  src_common_mem_ptr,
    LSA_VOID  CM_COMMON_MEM_ATTR *  dst_common_mem_ptr,
    LSA_UINT16                      length
);
#endif


/*=============================================================================
 * function name:  CM_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_ALLOC_TIMER
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ALLOC_TIMER(
    LSA_UINT16  CM_LOCAL_MEM_ATTR        *  ret_val_ptr,
    LSA_TIMER_ID_TYPE  CM_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                              timer_type,
    LSA_UINT16                              time_base
);
#endif


/*=============================================================================
 * function name:  CM_START_TIMER
 *
 * function:       start a timer
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_START_TIMER
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_START_TIMER(
    LSA_UINT16        CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                      timer_id,
    LSA_USER_ID_TYPE                       user_id,
    LSA_UINT16                             time
);
#endif


/*=============================================================================
 * function name:  CM_STOP_TIMER
 *
 * function:       stop a timer
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_STOP_TIMER
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_STOP_TIMER(
    LSA_UINT16  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                timer_id
);
#endif


/*=============================================================================
 * function name:  CM_FREE_TIMER
 *
 * function:       free a timer
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_FREE_TIMER
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_FREE_TIMER(
    LSA_UINT16  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                timer_id
);
#endif


/*=============================================================================
 * function name:  CM_ENTER
 *
 * function:       set reentrance lock
 *
 * documentation:  ---
 *
 *===========================================================================*/
#if 0 /* not used, #ifndef CM_ENTER */
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR  CM_ENTER(
    LSA_VOID
);
#endif


/*=============================================================================
 * function name:  CM_EXIT
 *
 * function:       cancel reentrance lock
 *
 * documentation:  ---
 *
 *===========================================================================*/
#if 0 /* not used, #ifndef CM_EXIT */
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR  CM_EXIT(
    LSA_VOID
);
#endif


/*=============================================================================
 * function name:  CM_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_FATAL_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_FATAL_ERROR(
    LSA_UINT16               length,
    CM_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif


/*=============================================================================
 * function name:  CM_GET_CYCLE_COUNTER
 *
 * function:       read the 64 bit cycle counter
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_GET_CYCLE_COUNTER
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GET_CYCLE_COUNTER(
    LSA_UINT16  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr,
    LSA_UINT64  CM_LOCAL_MEM_ATTR *  cycle_counter_ptr
);
#endif


/*=============================================================================
 * function name:  CM_GET_PRIMARY_AR
 *
 * function:       get the primary AR of an AR-set
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_GET_PRIMARY_AR
LSA_UINT16  CM_SYSTEM_OUT_FCT_ATTR
CM_GET_PRIMARY_AR(
	LSA_UINT16        arset_nr,
	LSA_SYS_PTR_TYPE  sys_ptr
);
#endif


/*=============================================================================
 * UUID functions, prototypes here because used in cm_arcb_xxx()
 *===========================================================================*/

LSA_BOOL  CM_SYSTEM_IN_FCT_ATTR
cm_uuid_equal (
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const uuid1,
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const uuid2
);

LSA_BOOL  CM_SYSTEM_IN_FCT_ATTR
cm_uuid_is_nil (
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const uuid
);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_SYS_H */
