#ifndef TCIP_SYS_H
#define TCIP_SYS_H

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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_sys.h                                :F&  */
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

typedef LSA_FATAL_ERROR_TYPE  TCIP_LOCAL_MEM_ATTR  *TCIP_FATAL_ERROR_PTR_TYPE;	/* pointer to LSA_FATAL_ERROR */

typedef LSA_VOID  TCIP_UPPER_MEM_ATTR  *TCIP_UPPER_MEM_PTR_TYPE;				/* pointer to upper-memory */

typedef LSA_VOID  TCIP_LOCAL_MEM_ATTR  *TCIP_LOCAL_MEM_PTR_TYPE;				/* pointer to local-memory */

typedef LSA_VOID  TCIP_EDD_LOWER_MEM_ATTR  *TCIP_EDD_LOWER_MEM_PTR_TYPE;		/* pointer to lower-memory */


/*------------------------------------------------------------------------------
// tcip details
//----------------------------------------------------------------------------*/

#define	TCIP_PATH_TYPE_ARP       0  /* the ARP channel  */
#define	TCIP_PATH_TYPE_ICMP      1  /* the ICMP channel */
#define	TCIP_PATH_TYPE_UDP       2  /* the UDP channel  */
#define	TCIP_PATH_TYPE_TCP       3  /* the TCP channel  */


#define TCIP_MAX_CHANNELS (4* EDD_CFG_MAX_INTERFACE_CNT)


typedef struct tcip_detail_tag {

	LSA_UINT8  path_type; /* TCIP_PATH_TYPE_ARP, etc. */
	
	LSA_UINT8  group_id;  /* identifies all channels belonging to the same network interface */
						  /* must use the same value as NARE_CDB_PATH_PARAMS_TYPE::NICId, see TCIP_ARP_RESPONSE_RECEIVED() */

	/***/

	LSA_UINT32 number_of_recv_resources;   /* number of receive recources */
	
	LSA_UINT32 number_of_send_resources;   /* number of send resources, 0 if not TCIP_PATH_TYPE_ARP */

} TCIP_DETAIL_TYPE;

typedef TCIP_DETAIL_TYPE  TCIP_SYSTEM_MEM_ATTR  *TCIP_DETAIL_PTR_TYPE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/


/*====  in functions  =====*/


/*=============================================================================
 * function name:  tcip_init
 *
 * function:       initializes tcip component
 *
 * parameters:     TCIP_INIT_PTR_TYPE   init_ptr:  pointer init structure
 *                 
 * return value:   LSA_UINT16:			LSA_RET_OK if successful
 *===========================================================================*/
LSA_UINT16  TCIP_SYSTEM_IN_FCT_ATTR
tcip_init(
	LSA_VOID
);


/*=============================================================================
 * function name:  tcip_undo_init
 *
 * function:       undo the initialization
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_UINT16  TCIP_SYSTEM_IN_FCT_ATTR
tcip_undo_init(
	LSA_VOID
);



/*=============================================================================
 * function name:  tcip_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  TCIP_SYSTEM_IN_FCT_ATTR
tcip_timeout(
	LSA_TIMER_ID_TYPE  timer_id,
	LSA_USER_ID_TYPE  user_id
);


/*=============================================================================
 * function name:  tcip_fatal
 *
 * function:       undocumented, not in tcip_int.h because of tcip_cfg.c
 *===========================================================================*/
LSA_VOID  TCIP_SYSTEM_IN_FCT_ATTR
tcip_fatal(
	LSA_UINT16 tcip_module_id,
	LSA_INT line 
);

#ifndef TCIP_FATAL 
#define TCIP_FATAL()  tcip_fatal(TCIP_MODULE_ID, __LINE__)
#endif


LSA_VOID  TCIP_SYSTEM_IN_FCT_ATTR
tcip_fatal1(
	LSA_UINT16 tcip_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
);

#ifndef TCIP_FATAL1 
#define TCIP_FATAL1(ec_0)       tcip_fatal1 (TCIP_MODULE_ID, __LINE__, ec_0)
#endif



/*====  out functions  =====*/

/*=============================================================================
 * function name:  TCIP_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_GET_PATH_INFO
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_GET_PATH_INFO(
	LSA_UINT16				TCIP_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE		TCIP_LOCAL_MEM_ATTR *  sys_ptr_ptr,
	TCIP_DETAIL_PTR_TYPE	TCIP_LOCAL_MEM_ATTR *  detail_ptr_ptr,
	LSA_SYS_PATH_TYPE		path
);
#endif


/*=============================================================================
 * function name:  TCIP_RELEASE_PATH_INFO
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_RELEASE_PATH_INFO
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_RELEASE_PATH_INFO(
	LSA_UINT16 TCIP_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE                  sys_ptr,
	TCIP_DETAIL_PTR_TYPE              detail_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_ALLOC_LOCAL_MEM
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_ALLOC_LOCAL_MEM(
	TCIP_LOCAL_MEM_PTR_TYPE  TCIP_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
	LSA_UINT16										length
);
#endif


/*=============================================================================
 * function name:  TCIP_FREE_LOCAL_MEM
 *
 * function:       free a local-memory
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_FREE_LOCAL_MEM
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_FREE_LOCAL_MEM(
	LSA_UINT16                 TCIP_LOCAL_MEM_ATTR *  ret_val_ptr,
	TCIP_LOCAL_MEM_PTR_TYPE                           local_mem_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_STRLEN
 *
 * function:       Returns the length of str. 
 *                 The length of a C string is determined by the terminating 
 *                 null-character: A C string is as long as the amount of 
 *                 characters between the beginning of the string and the 
 *                 terminating null character.
 *============================================================================*/
#ifndef TCIP_STRLEN
LSA_UINT32  TCIP_SYSTEM_OUT_FCT_ATTR  
TCIP_STRLEN(
	const LSA_CHAR LSA_COMMON_MEM_ATTR * p_str
);
#endif


/*=============================================================================
 * function name:  TCIP_MEMSET, TCIP_MEMCPY, TCIP_MEMCMP
 *
 * function:       macro for stdlib function memset
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat"
 *
 *===========================================================================*/

#ifndef TCIP_MEMSET
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT val,
	LSA_UINT len
);
#endif

#ifndef TCIP_MEMCPY
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len
);
#endif

#ifndef TCIP_MEMMOVE
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_MEMMOVE(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len
);
#endif

#ifndef TCIP_MEMCMP
LSA_INT TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_MEMCMP(
	LSA_VOID_PTR_TYPE buf1,
	LSA_VOID_PTR_TYPE buf2,
	LSA_UINT len
);
#endif


/*=============================================================================
 * function name:  TCIP_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_ALLOC_TIMER
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_ALLOC_TIMER(
	LSA_UINT16  TCIP_LOCAL_MEM_ATTR        *  ret_val_ptr,
	LSA_TIMER_ID_TYPE  TCIP_LOCAL_MEM_ATTR *  timer_id_ptr,
	LSA_UINT16								timer_type,
	LSA_UINT16								time_base
);
#endif


/*=============================================================================
 * function name:  TCIP_START_TIMER
 *
 * function:       start a timer
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_START_TIMER
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_START_TIMER(
	LSA_UINT16        TCIP_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                      timer_id,
	LSA_USER_ID_TYPE                       user_id,
	LSA_UINT16                             time
);
#endif


/*=============================================================================
 * function name:  TCIP_STOP_TIMER
 *
 * function:       stop a timer
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_STOP_TIMER
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_STOP_TIMER(
	LSA_UINT16  TCIP_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                timer_id
);
#endif


/*=============================================================================
 * function name:  TCIP_FREE_TIMER
 *
 * function:       free a timer
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_FREE_TIMER
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_FREE_TIMER(
	LSA_UINT16  TCIP_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                timer_id
);
#endif


/*=============================================================================
 * function name:  TCIP_ARP_RESPONSE_RECEIVED
 *
 * function:       is called when an ARP frame with opcode 2 is received
 *                 intended useage: ARP sniffer, see nare_ARPResponseFrameIn()
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_ARP_RESPONSE_RECEIVED
LSA_VOID TCIP_ARP_RESPONSE_RECEIVED(
	LSA_UINT16                 group_id, /* must have the same value as NARE_CDB_PATH_PARAMS_TYPE::NICId */
	EDD_UPPER_MEM_U8_PTR_TYPE  frame_buf,
	LSA_UINT16                 frame_len
);
#endif

/*=============================================================================
 * function name:  TCIP_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_FATAL_ERROR
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_FATAL_ERROR(
	LSA_UINT16					length,
	TCIP_FATAL_ERROR_PTR_TYPE	error_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_EXCHANGE_LONG
 *
 * function:       thread-save exchange of a "long", returns the previous value
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef TCIP_EXCHANGE_LONG
long  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_EXCHANGE_LONG(
	long volatile *ptr,
	long val
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_SYS_H */
