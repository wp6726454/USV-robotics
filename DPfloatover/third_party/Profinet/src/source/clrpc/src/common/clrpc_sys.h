#ifndef CLRPC_SYS_H
#define CLRPC_SYS_H

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
/*  F i l e               &F: clrpc_sys.h                               :F&  */
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

#if 1 /* why the preprocessor when we can use typedef... */

typedef LSA_VOID  CLRPC_UPPER_MEM_ATTR *CLRPC_UPPER_MEM_PTR_TYPE; /* pointer to upper-memory */

typedef LSA_VOID  CLRPC_LOWER_MEM_ATTR *CLRPC_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID  CLRPC_LOCAL_MEM_ATTR *CLRPC_LOCAL_MEM_PTR_TYPE; /* pointer to local-memory */

typedef struct clrpc_detail_tag    CLRPC_SYSTEM_MEM_ATTR *CLRPC_DETAIL_PTR_TYPE; /* pointer to CLRPC_DETAIL */

typedef struct lsa_fatal_error_tag CLRPC_LOCAL_MEM_ATTR *CLRPC_FATAL_ERROR_PTR_TYPE; /* pointer to LSA_FATAL_ERROR */

#else

#define CLRPC_UPPER_MEM_PTR_TYPE               /* pointer to upper-memory */ \
    LSA_VOID                    CLRPC_UPPER_MEM_ATTR *

#define CLRPC_LOWER_MEM_PTR_TYPE               /* pointer to lower-memory */ \
    LSA_VOID                    CLRPC_LOWER_MEM_ATTR *

#define CLRPC_LOCAL_MEM_PTR_TYPE               /* pointer to local-memory */ \
    LSA_VOID                    CLRPC_LOCAL_MEM_ATTR *

#define CLRPC_DETAIL_PTR_TYPE                 /* pointer to CLRPC_DETAIL */ \
    struct clrpc_detail_tag    CLRPC_SYSTEM_MEM_ATTR *

#define CLRPC_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
    struct lsa_fatal_error_tag  CLRPC_LOCAL_MEM_ATTR *
#endif

/*------------------------------------------------------------------------------
// clrpc details
//----------------------------------------------------------------------------*/

#define CLRPC_PATH_TYPE_SYSTEM		1
#define CLRPC_PATH_TYPE_USER		2

typedef struct clrpc_detail_tag 
{

	LSA_UINT16  path_type; /* see CLRPC_PATH_TYPE_xxx */

	/* note: the parameter ep_mapper_port was removed with V4.0i80.2 (AP00619205), value is now always CLRPC_EPM_DEFAULT_PORT */

	union 
    {
#if CLRPC_CFG_MAX_CLIENTS
		struct 
        { /* client details */
			LSA_UINT8  nothing_so_far; /* details were deleted, AP01309324 */
            /* values send_data_size, rtxTimeout and rtxMax were deleted, 
               used values are CLRPC_MUST_RECV_BODY_SIZE, CLRPC_RTX_TIMEOUT_MSEC and CLRPC_MAX_RTX_RETRY */
        } cl;
#endif

#if CLRPC_CFG_MAX_SERVERS /* EPM is a special server */
		struct 
        { /* endpoint-mapper details */

			LSA_UINT8  nothing_so_far; /* details were moved to CM-PD with V4.1i3 (AP00706078) */

		} epm;
#endif
	} u;

} CLRPC_DETAIL_TYPE;

/*------------------------------------------------------------------------------
// clrpc init-parameters
//----------------------------------------------------------------------------*/

typedef struct clrpc_init_tag {

	LSA_UINT16 oneshot_lsa_time_base;   /* oneshot-timer, see clrpc_oneshot_sys_init() */
	LSA_UINT16 oneshot_lsa_time_factor; /* oneshot-timer, see clrpc_oneshot_sys_init() */

} CLRPC_INIT_TYPE, CLRPC_SYSTEM_MEM_ATTR * CLRPC_INIT_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/



/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  clrpc_init
 *
 * function:       initialize
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
LSA_UINT16  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_init(
	CLRPC_INIT_PTR_TYPE init
);

/*=============================================================================
 * function name:  clrpc_undo_init
 *
 * function:       undo the initialization of clrpc
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
LSA_UINT16  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_undo_init(
    LSA_VOID
);

/*=============================================================================
 * function name:  clrpc_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_timeout(
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE   user_id
);


/*=============================================================================
 * function name:  clrpc_get_sock_handle
 *
 * function:       undocumented, for testing only
 *===========================================================================*/
LSA_HANDLE_TYPE  CLRPC_SYSTEM_IN_FCT_ATTR
clrpc_get_sock_handle (
    LSA_HANDLE_TYPE  clrpc_handle
);


/*====  out functions  =====*/

/*=============================================================================
 * function name:  CLRPC_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_GET_PATH_INFO
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_GET_PATH_INFO(
    LSA_UINT16              CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        CLRPC_LOCAL_MEM_ATTR *  sys_ptr_ptr,
    CLRPC_DETAIL_PTR_TYPE   CLRPC_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                               path
);
#endif

/*=============================================================================
 * function name:  CLRPC_RELEASE_PATH_INFO
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_RELEASE_PATH_INFO
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_RELEASE_PATH_INFO(
    LSA_UINT16             CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                               sys_ptr,
    CLRPC_DETAIL_PTR_TYPE                          detail_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_ALLOC_LOCAL_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_ALLOC_LOCAL_MEM(
    CLRPC_LOCAL_MEM_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
    LSA_UINT16                                        length
);
#endif

/*=============================================================================
 * function name:  CLRPC_FREE_LOCAL_MEM
 *
 * function:       free a local-memory
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_FREE_LOCAL_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_FREE_LOCAL_MEM(
    LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    CLRPC_LOCAL_MEM_PTR_TYPE                           local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower-memory to upper-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_LOWER_TO_UPPER_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_LOWER_TO_UPPER_MEM(
    CLRPC_UPPER_MEM_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
    CLRPC_LOWER_MEM_PTR_TYPE                          lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper-memory to lower-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_UPPER_TO_LOWER_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_UPPER_TO_LOWER_MEM(
    CLRPC_LOWER_MEM_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    CLRPC_UPPER_MEM_PTR_TYPE                          upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_LOWER_TO_LOWER_MEM
 *
 * function:       convert lower-memory to lower-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_LOWER_TO_LOWER_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_LOWER_TO_LOWER_MEM(
    CLRPC_LOWER_MEM_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    CLRPC_LOWER_MEM_PTR_TYPE                          lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                  src_sys_ptr,
    LSA_SYS_PTR_TYPE                                  dst_sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_COPY_COMMON_MEM
 *
 * function:       copy common memory
 *                 The memory areas of source and destination mustn't overlap.
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_COPY_COMMON_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_COPY_COMMON_MEM(
    LSA_VOID  CLRPC_COMMON_MEM_ATTR *  src_common_mem_ptr,
    LSA_VOID  CLRPC_COMMON_MEM_ATTR *  dst_common_mem_ptr,
    LSA_UINT16                         length
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_ALLOC_TIMER
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_ALLOC_TIMER(
    LSA_UINT16         CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE  CLRPC_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                                 timer_type,
    LSA_UINT16                                 time_base
);
#endif

/*=============================================================================
 * function name:  CLRPC_START_TIMER
 *
 * function:       start a timer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_START_TIMER
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_START_TIMER(
    LSA_UINT16        CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                         timer_id,
    LSA_USER_ID_TYPE                          user_id,
    LSA_UINT16                                time
);
#endif

/*=============================================================================
 * function name:  CLRPC_STOP_TIMER
 *
 * function:       stop a timer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_STOP_TIMER
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_STOP_TIMER(
    LSA_UINT16  CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                   timer_id
);
#endif

/*=============================================================================
 * function name:  CLRPC_FREE_TIMER
 *
 * function:       free a timer
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_FREE_TIMER
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_FREE_TIMER(
    LSA_UINT16  CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                   timer_id
);
#endif

/*=============================================================================
 * function name:  CLRPC_ENTER
 *
 * function:       set reentrance lock
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0
#ifndef CLRPC_ENTER
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_ENTER(
    LSA_VOID
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_EXIT
 *
 * function:       cancel reentrance lock
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0
#ifndef CLRPC_EXIT
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_EXIT(
    LSA_VOID
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_FATAL_ERROR
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_FATAL_ERROR(
    LSA_UINT16                  length,
    CLRPC_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif

/*=============================================================================
 * function name:  clrpc_fatal1
 *
 * function:       undocumented, not in clrpc_int.h because of clrpc_cfg.c
 *===========================================================================*/
LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR
clrpc_fatal1(
	LSA_UINT16 clrpc_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
);

#ifndef CLRPC_FATAL1 /* AP00289268, note: returning from fatal is not allowed */
#define CLRPC_FATAL1(ec_0)  clrpc_fatal1 (CLRPC_MODULE_ID, __LINE__, ec_0)
#endif



/*=============================================================================*/
/** \brief reads the value of a 100ns-counter

    This value will be used for creating a uuid or server-boot-time. 
    The counter starts with 0 an counts the 100ns from the last power-on until now.
*//*===========================================================================*/
#ifndef CLRPC_GET_100NS_COUNTER
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_GET_100NS_COUNTER(
    LSA_UINT16  CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,         /**< pointer for return value 
                                                            - LSA_RET_OK    nor error
                                                            - LSA_RET_RESOURCE    counter could not be read */
    LSA_UINT64  CLRPC_LOCAL_MEM_ATTR * counter_value_ptr   /**< pointer for value of 100ns-counter */
);
#endif


/*=============================================================================
 * function name:  CLRPC_PUT8,16,32,UUID and CLRPC_GET8,16,32,UUID
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifdef  CLRPC_PUT8_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT8_ALIGNED(ptr, val) { \
	*(LSA_UINT8 CLRPC_COMMON_MEM_ATTR *)(ptr) = (LSA_UINT8)(val); \
	}
#endif

#ifdef  CLRPC_GET8_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET8_ALIGNED(ptr) ( \
	*(LSA_UINT8 CLRPC_COMMON_MEM_ATTR *)(ptr) \
	)
#endif

#ifdef  CLRPC_PUT16_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT16_ALIGNED(ptr, val) { \
	*(LSA_UINT16 CLRPC_COMMON_MEM_ATTR *)(CLRPC_TEST_ALIGN2(ptr)) = (LSA_UINT16)(val); \
	}
#endif

#ifdef  CLRPC_GET16_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET16_ALIGNED(ptr) ( \
	*(LSA_UINT16 CLRPC_COMMON_MEM_ATTR *)(CLRPC_TEST_ALIGN2(ptr)) \
	)
#endif

#ifdef  CLRPC_PUT32_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT32_ALIGNED(ptr, val) { \
	*(LSA_UINT32 CLRPC_COMMON_MEM_ATTR *)(CLRPC_TEST_ALIGN4(ptr)) = (LSA_UINT32)(val); \
	}
#endif

#ifdef  CLRPC_GET32_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET32_ALIGNED(ptr) ( \
	*(LSA_UINT32 CLRPC_COMMON_MEM_ATTR *)(CLRPC_TEST_ALIGN4(ptr)) \
	)
#endif

#ifdef  CLRPC_PUT_UUID_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT_UUID_ALIGNED(ptr, uuid_ptr) { \
	*(CLRPC_UUID_TYPE CLRPC_COMMON_MEM_ATTR *)(CLRPC_TEST_ALIGN4(ptr)) = *(/*no-cast*/uuid_ptr); \
	}
#endif

#ifdef  CLRPC_GET_UUID_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET_UUID_ALIGNED(uuid_ptr, ptr) { \
	*(uuid_ptr) = *(CLRPC_UUID_TYPE CLRPC_COMMON_MEM_ATTR *)(CLRPC_TEST_ALIGN4(ptr)); \
	}
#endif

/*=============================================================================
 * function name:  CLRPC_SWAP_U16, CLRPC_SWAP_U32 and CLRPC_SWAP_UUID
 *
 * function:       swap "in place"
 *                 between "little" and "big" endian or vice versa
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_SWAP_U16
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_SWAP_U16(
	LSA_UINT16 CLRPC_COMMON_MEM_ATTR * short_ptr /* correct alignment! */
);
#endif

#ifndef CLRPC_SWAP_U32
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_SWAP_U32(
	LSA_UINT32 CLRPC_COMMON_MEM_ATTR * long_ptr /* correct alignment! */
);
#endif

#ifndef CLRPC_SWAP_UUID
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_SWAP_UUID(
	CLRPC_UUID_PTR_TYPE uuid_ptr /* correct alignment! */
);
#endif

/*=============================================================================
 * function name:  CLRPC_HTONS ... host to network byte order, short integer
 *                 CLRPC_HTONL ... host to network byte order, long  integer
 *                 CLRPC_NTOHS ... network to host byte order, short integer
 *                 CLRPC_NTOHL ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_CLRPC_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CLRPC_HTONS
LSA_UINT16 CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_HTONS(
	LSA_UINT16 host_short
);
#endif

#ifndef CLRPC_HTONL
LSA_UINT32 CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_HTONL(
	LSA_UINT32 host_long
);
#endif

#ifndef CLRPC_NTOHS
LSA_UINT16 CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_NTOHS(
	LSA_UINT16 network_short
);
#endif

#ifndef CLRPC_NTOHL
LSA_UINT32 CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_NTOHL(
	LSA_UINT32 network_long
);
#endif

/*=============================================================================
 * function name:  CLRPC_ PUT/GET 16_/UUID_ LITTLE
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifndef CLRPC_PUT16_LITTLE
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_PUT16_LITTLE( /* put, little endian byte order */
	LSA_VOID_PTR_TYPE ptr,
	LSA_UINT16 val
);
#endif

#ifndef CLRPC_GET16_LITTLE
LSA_UINT16  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_GET16_LITTLE( /* get, little endian byte order */
	LSA_VOID_PTR_TYPE ptr
);
#endif

#ifndef CLRPC_PUT_UUID_LITTLE
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_PUT_UUID_LITTLE( /* put, convert into little endian byte order */
	LSA_VOID_PTR_TYPE ptr,
	CLRPC_UUID_PTR_TYPE uuid_ptr /* correct alignment! */
);
#endif
#ifndef CLRPC_GET_UUID_LITTLE
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_GET_UUID_LITTLE( /* get, convert from little endian byte order */
	CLRPC_UUID_PTR_TYPE uuid_ptr, /* correct alignment! */
	LSA_VOID_PTR_TYPE ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_MEMSET, CLRPC_MEMCPY,
 *
 * function:       macros for standard C-lib functions
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat"
 *
 *===========================================================================*/

#ifndef CLRPC_MEMSET
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT val,
	LSA_UINT len
);
#endif

#ifndef CLRPC_MEMCPY
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR
CLRPC_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_SYS_H */
