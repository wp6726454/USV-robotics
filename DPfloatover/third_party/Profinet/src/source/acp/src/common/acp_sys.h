#ifndef ACP_SYS_H
#define ACP_SYS_H

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
/*  F i l e               &F: acp_sys.h                                 :F&  */
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
/*                       constants for detail-pointer                        */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define ACP_LOWER_MEM_PTR_TYPE               /* pointer to lower-memory */ \
    LSA_VOID                    ACP_LOWER_MEM_ATTR *

#define ACP_LOCAL_MEM_PTR_TYPE               /* pointer to local-memory */ \
    LSA_VOID                    ACP_LOCAL_MEM_ATTR *

#define ACP_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
    struct lsa_fatal_error_tag  ACP_LOCAL_MEM_ATTR *


/*------------------------------------------------------------------------------
// acp_init_tag
//----------------------------------------------------------------------------*/

typedef struct acp_init_tag
{
	LSA_UINT16	rta_time_base;		/* see LSA_TIME_BASE_xxx */
	LSA_UINT16  rta_time_factor;	/* */
} ACP_INIT_TYPE;

typedef struct acp_init_tag ACP_SYSTEM_MEM_ATTR * ACP_INIT_PTR_TYPE;


/*------------------------------------------------------------------------------
// acp_detail_tag
//----------------------------------------------------------------------------*/

enum acp_channel_type_enum {
	ACP_CHANNEL_TYPE_SYSTEM		= 1,
	ACP_CHANNEL_TYPE_USER		= 2
};

typedef struct acp_detail_tag
{
	LSA_UINT16	nic_id;					/* reference to the network device which must be used  */
	LSA_UINT16	channel_type;			/* see acp_channel_type_enum */

	union acp_detail_union {

	struct acp_detail_sys_tag {

		LSA_UINT16  max_edd_indication_res;		/* count of ind resources, used by edd.state.indications */
												/* earlier: ACP_CFG_MAX_EDD_INDICATION_RES */

		LSA_UINT16  max_edd_nrt_send_res;		/* count of send resouces, used by edd.nrt.send */
		LSA_UINT16  max_edd_nrt_recv_res;		/* count of recv resouces, used by edd.nrt.recv */

	}  system;

	struct acp_detail_usr_tag {

			/* class 1 RT */
		LSA_UINT16	frame_id_base_class1_unicast;	/* the first available frame_id of the consumer range of the class 1 */
		LSA_UINT16	nr_of_consumers_class1_unicast;	/* number of available consumer of the class 1  */
		LSA_UINT16	frame_id_base_class1_multicast;
		LSA_UINT16	nr_of_consumers_class1_multicast;

			/* class 2 */
		LSA_UINT16	frame_id_base_class2_unicast;	/* the first available frame_id of the consumer range of the class 2 */
		LSA_UINT16	nr_of_consumers_class2_unicast;	/* number of available consumer of the class 2  */
		LSA_UINT16	frame_id_base_class2_multicast;
		LSA_UINT16	nr_of_consumers_class2_multicast;

		LSA_UINT16	nr_of_providers_class1_2;		/* number of available provider of the class 1 and class 2  */

			/* class 3 */
		LSA_UINT16	nr_of_consumers_class3;	/* number of available consumer of the class 3 */
		LSA_UINT16	nr_of_providers_class3;	/* number of available provider of the class 3 */

			/* alarm */
		LSA_UINT16	alarm_ref_base;			/* first available alarm-reference number */
		LSA_UINT16	alarm_nr_of_ref;		/* number of available reference numbers */

			/* for all provider */
		LSA_UINT16	group_id;				/* group_id for Provider: 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP */

	}  user;
	} type;
} ACP_DETAIL_TYPE;

typedef struct acp_detail_tag ACP_SYSTEM_MEM_ATTR * ACP_DETAIL_PTR_TYPE;


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  acp_init
 *
 * function:       initialize acp
 *
 * parameters:     ACP_INIT_PTR_TYPE  init
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  ACP_SYSTEM_IN_FCT_ATTR  acp_init (
    ACP_INIT_PTR_TYPE  init
);

/*=============================================================================
 * function name:  acp_undo_init
 *
 * function:       undo the initialization of acp
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  ACP_SYSTEM_IN_FCT_ATTR  acp_undo_init (
    LSA_VOID
);

/*=============================================================================
 * function name:  acp_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_UINT16        timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of acp
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_timeout (
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE   user_id
);


/*====  out functions  =====*/

/*=============================================================================
 * function name:  ACP_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 ACP_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_GET_PATH_INFO
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_GET_PATH_INFO (
    LSA_UINT16           ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE     ACP_LOCAL_MEM_ATTR *  sys_ptr_ptr,
    ACP_DETAIL_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                          path
);
#endif

/*=============================================================================
 * function name:  ACP_RELEASE_PATH_INFO
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16      ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no release because of
 *                                                      wrong system-pointer or
 *                                                      wrong detail-pointer
 *                 LSA_SYS_PTR_TYPE        sys_ptr:     system-pointer
 *                 ACP_DETAIL_PTR_TYPE  detail_ptr:  detail-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_RELEASE_PATH_INFO
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_RELEASE_PATH_INFO (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr,
    ACP_DETAIL_PTR_TYPE               detail_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * parameters:     ACP_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_ALLOC_LOCAL_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_ALLOC_LOCAL_MEM (
    ACP_LOCAL_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
    LSA_UINT32                                    length
);
#endif

/*=============================================================================
 * function name:  ACP_FREE_LOCAL_MEM
 *
 * function:       free a local-memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 ACP_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_FREE_LOCAL_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_FREE_LOCAL_MEM (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_LOCAL_MEM_PTR_TYPE            local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower-memory to upper-memory
 *
 * parameters:     PREFIX_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 PREFIX_LOWER_MEM_PTR_TYPE       lower_mem_ptr:
 *                                                 pointer to lower-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if 0 /* not used */
#ifndef ACP_LOWER_TO_UPPER_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_LOWER_TO_UPPER_MEM(
    ACP_UPPER_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
    ACP_LOWER_MEM_PTR_TYPE                        lower_mem_ptr,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  ACP_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper-memory to lower-memory
 *
 * parameters:     PREFIX_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 PREFIX_UPPER_MEM_PTR_TYPE       upper_mem_ptr:
 *                                                 pointer to upper-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if 0 /* not used */
#ifndef ACP_UPPER_TO_LOWER_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_UPPER_TO_LOWER_MEM(
    ACP_LOWER_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    ACP_UPPER_MEM_PTR_TYPE                        upper_mem_ptr,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  ACP_LOWER_TO_LOWER_MEM
 *
 * function:       convert lower-memory to lower-memory
 *
 * parameters:     PREFIX_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible
 *                 PREFIX_LOWER_MEM_PTR_TYPE       lower_mem_ptr:
 *                                                 pointer to lower-memory
 *                 LSA_SYS_PTR_TYPE                src_sys_ptr:
 *                                                 system-pointer of
 *                                                 source-memory
 *                 LSA_SYS_PTR_TYPE                dst_sys_ptr:
 *                                                 system-pointer of
 *                                                 destination-memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if 0 /* not used */
#ifndef ACP_LOWER_TO_LOWER_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_LOWER_TO_LOWER_MEM(
    ACP_LOWER_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    ACP_LOWER_MEM_PTR_TYPE                        lower_mem_ptr,
    LSA_SYS_PTR_TYPE                              src_sys_ptr,
    LSA_SYS_PTR_TYPE                              dst_sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  ACP_COPY_COMMON_MEM
 *
 * function:       copy common memory
 *                 The memory areas of source and destination mustn't overlap.
 *
 * parameters:     LSA_VOID  ACP_COMMON_MEM_ATTR * src_common_mem_ptr:
 *                                                 void-pointer to source-
 *                                                 common-memory
 *                 LSA_VOID  ACP_COMMON_MEM_ATTR * dst_common_mem_ptr:
 *                                                 void-pointer to destination-
 *                                                 common-memory
 *                 LSA_UINT32                      length: number of bytes
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if 0 /* not used, see ACP_MEMCPY() */
#ifndef ACP_COPY_COMMON_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_COPY_COMMON_MEM(
    LSA_VOID  ACP_COMMON_MEM_ATTR *  src_common_mem_ptr,
    LSA_VOID  ACP_COMMON_MEM_ATTR *  dst_common_mem_ptr,
    LSA_UINT32                       length
);
#endif
#endif

/*=============================================================================
 * function name:  ACP_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                           return value: LSA_RET_OK            timer has been
 *                                                               allocated
 *                                         LSA_RET_ERR_NO_TIMER  no timer has
 *                                                               been allocated
 *                 LSA_TIMER_ID_TYPE  ...  *  timer_id_ptr:
 *                           return value: pointer to id of timer
 *                 LSA_UINT16          timer_type:  LSA_TIMER_TYPE_ONE_SHOT or
 *                                                  LSA_TIMER_TYPE_CYCLIC
 *                 LSA_UINT16          time_base:   LSA_TIME_BASE_1MS,
 *                                                  LSA_TIME_BASE_10MS,
 *                                                  LSA_TIME_BASE_100MS,
 *                                                  LSA_TIME_BASE_1S,
 *                                                  LSA_TIME_BASE_10S or
 *                                                  LSA_TIME_BASE_100S
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_ALLOC_TIMER
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_ALLOC_TIMER (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *         ret_val_ptr,
    LSA_TIMER_ID_TYPE  ACP_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                               timer_type,
    LSA_UINT16                               time_base
);
#endif

/*=============================================================================
 * function name:  ACP_START_TIMER
 *
 * function:       start a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                  timer has been
 *                                                             started
 *                                 LSA_RET_OK_TIMER_RESTARTED  timer has been
 *                                                             restarted
 *                                 LSA_RET_ERR_PARAM           timer has not
 *                                                             been started
 *                                                             because of wrong
 *                                                             timer-id
 *                                 After the expiration of the running time
 *                                 system will call acp_timeout().
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of acp
 *                 LSA_UINT16          time:      running time
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_START_TIMER
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_START_TIMER (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                 timer_id,
    LSA_USER_ID_TYPE                  user_id,
    LSA_UINT16                        time
);
#endif

/*=============================================================================
 * function name:  ACP_STOP_TIMER
 *
 * function:       stop a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                    timer has been
 *                                                               stopped
 *                                 LSA_RET_OK_TIMER_NOT_RUNNING  timer was not
 *                                                               running; timer
 *                                                               is stopped
 *                                 LSA_RET_ERR_PARAM             timer has not
 *                                                               stopped
 *                                                               because
 *                                                               of wrong
 *                                                               timer-id
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_STOP_TIMER
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_STOP_TIMER (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                 timer_id
);
#endif

/*=============================================================================
 * function name:  ACP_FREE_TIMER
 *
 * function:       free a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                return value: LSA_RET_OK                    timer has been
 *                                                            deallocated
 *                              LSA_RET_ERR_TIMER_IS_RUNNING  because timer is
 *                                                            running timer has
 *                                                            not been
 *                                                            deallocated
 *                              LSA_RET_ERR_PARAM             no deallocation
 *                                                            because of wrong
 *                                                            timer-id
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_FREE_TIMER
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_FREE_TIMER (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                 timer_id
);
#endif

/*=============================================================================
 * function name:  ACP_ENTER
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_ENTER
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_ENTER (
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  ACP_EXIT
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_EXIT
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_EXIT (
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  ACP_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                length:     number of bytes
 *                 ACP_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                       LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_FATAL_ERROR
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_FATAL_ERROR (
    LSA_UINT16                length,
    ACP_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif


/*=============================================================================
 * function name:  ACP_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     ACP_UPPER_CALLBACK_FCT_PTR_TYPE
 *                                         acp_request_upper_done_ptr:
 *                     pointer to acp_request_upper_done_ptr, given by
 *                     acp_open_channel()
 *
 *                 ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-RQB
 *                 LSA_SYS_PTR_TYPE        sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef ACP_REQUEST_UPPER_DONE
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_REQUEST_UPPER_DONE (
    ACP_UPPER_CALLBACK_FCT_PTR_TYPE  acp_request_upper_done_ptr,
    ACP_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr
);
#endif


/*===========================================================================*/
/* non-LSA-predefined             prototyping                                */
/*===========================================================================*/


/*=============================================================================
 * function name:  acp_fatal_error
 *
 * function:       helper to fills
 *                    LSA_FATAL_ERROR_TYPE  acp_glob_fatal_error
 *                 and then calls ACP_FATAL_ERROR
 *
 *===========================================================================*/

LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR
acp_fatal_error (
	LSA_UINT16         module_id,
	LSA_UINT16         line,
	LSA_UINT32         error_code_0,
	LSA_UINT32         error_code_1,
	LSA_UINT32         error_code_2,
	LSA_UINT32         error_code_3,
	LSA_UINT16         error_data_len,
	LSA_VOID_PTR_TYPE  error_data
	);


/*------------------------------------------------------------------------------
// ASRT within ACP
// need NRT
//----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_NRT_SEND_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_ALLOC_NRT_SEND_MEM (
    ACP_LOWER_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_UINT32                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

#ifndef ACP_FREE_NRT_SEND_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_FREE_NRT_SEND_MEM (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr
);
#endif

#ifndef ACP_ALLOC_NRT_RECV_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_ALLOC_NRT_RECV_MEM (
    ACP_LOWER_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    LSA_UINT32                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

#ifndef ACP_FREE_NRT_RECV_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_FREE_NRT_RECV_MEM (
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr
);
#endif


/*=============================================================================
 * function name:  ACP_MEMCPY
 *
 * function:       macros for standard C-lib functions
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat"
 *
 *===========================================================================*/

#ifndef ACP_MEMCPY
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_MEMCPY (
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           length
);
#endif



/*=============================================================================
 * function name:  ACP_PUT 8,16,32 and ACP_GET 8,16,32
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifndef ACP_PUT8_HTON
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_PUT8_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT8          val
);
#endif

#ifndef ACP_GET8_NTOH
LSA_UINT8  ACP_SYSTEM_OUT_FCT_ATTR
ACP_GET8_NTOH ( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset
);
#endif

#ifndef ACP_PUT16_HTON
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_PUT16_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT16         val
);
#endif

#ifndef ACP_GET16_NTOH
LSA_UINT16  ACP_SYSTEM_OUT_FCT_ATTR
ACP_GET16_NTOH ( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset
);
#endif

#ifndef ACP_PUT32_HTON
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_PUT32_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT32         val
);
#endif

#ifndef ACP_GET32_NTOH
LSA_UINT32  ACP_SYSTEM_OUT_FCT_ATTR
ACP_GET32_NTOH ( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_SYS_H */
