#ifndef SOCK_SYS_H
#define SOCK_SYS_H
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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_sys.h                                :F&  */
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
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*------------------------------------------------------------------------------
//	CONSTANTS
//----------------------------------------------------------------------------*/

#define SOCK__LINE__       ((LSA_UINT16) __LINE__)  /* __LINE__ has type int */

/*------------------------------------------------------------------------------
//	TYPES
//----------------------------------------------------------------------------*/

#define SOCK_LOCAL_MEM_PTR_TYPE       SOCK_LOCAL_MEM_ATTR *

#ifndef SOCK_DETAIL_PTR_TYPE
	#define SOCK_DETAIL_PTR_TYPE        struct sock_details_tag SOCK_SYSTEM_MEM_ATTR *
#endif

#define SOCK_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
	struct lsa_fatal_error_tag  SOCK_LOCAL_MEM_ATTR *

typedef struct sock_socket_opt_tag
{
	LSA_BOOL                SO_REUSEADDR_;  /* allow local address reuse */
	LSA_BOOL                SO_DONTROUTE_;  /* just use interface addresses */
	LSA_BOOL                SO_SNDBUF_;     /* send buffer size */
	LSA_BOOL                SO_RCVBUF_;     /* receive buffer size */
	LSA_BOOL                SO_LINGER_;     /* socket lingers on close */
	LSA_BOOL                SO_TCPNODELAY_; /* delay sending data (Nagle algorithm) */
	LSA_BOOL                SO_TCPNOACKDELAY_;  /* delay sending ACKs */
} SOCKET_OPTION;


/*------------------------------------------------------------------------------
//	DETAIL POINTER
//----------------------------------------------------------------------------*/

typedef struct sock_details_tag
{
	SOCKET_OPTION           socket_option;
	LSA_INT32               send_buffer_max_len;
	LSA_INT32               rec_buffer_max_len;
	LSA_INT16               sock_close_at_once;
	LSA_INT16               linger_time;

} SOCK_DETAILS_TYPE;


/*------------------------------------------------------------------------------
//	[IN] functions
//----------------------------------------------------------------------------*/

/*=============================================================================
 * function name:  sock_init
 *
 * function:       basic initialization of socket
 *
 * parameters:     none
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              LSA_RET_ERR_RESOURCE    not enouth local memory
 *===========================================================================*/
LSA_UINT16 SOCK_SYSTEM_IN_FCT_ATTR
sock_init(
	LSA_VOID
);

/*=============================================================================
 * function name:  sock_undo_init
 *
 * function:       Undo initialization of global data
 *
 * parameters:     none
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  SOCK_SYSTEM_IN_FCT_ATTR
sock_undo_init(
	LSA_VOID
);

/*=============================================================================
 * function name:  sock_timeout
 *
 * function:       expiration of the running time of a timer
 *                 Signals the processing of a timer

 *
 * parameters:     LSA_TIMER_ID_TYPE timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of prefix
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID SOCK_SYSTEM_IN_FCT_ATTR
sock_timeout(
	LSA_TIMER_ID_TYPE   timer_id,           /* Timer-id of the system */
	LSA_USER_ID_TYPE    user_id             /* socket handle */
);

/*=============================================================================
 * function name:
 *
 * function:
 *
 * parameters:
 *
 * return value:
 *===========================================================================*/

LSA_VOID SOCK_SYSTEM_IN_FCT_ATTR
sock_mib2_sysgroup_written(
	char *pSysName,    /* != LSA_NULL if written */
	char *pSysContact, /* != LSA_NULL if written */
	char *pSysLocation /* != LSA_NULL if written */
);


/*------------------------------------------------------------------------------
//	[OUT] functions
//----------------------------------------------------------------------------*/

/*=============================================================================
 * function name:  SOCK_GET_PATH_INFO
 *
 * function:       Determin SYS- and DETAIL-pointer of a communication channel.
 *                 Both pointers are written directly in the local data area of SOCK.
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 PREFIX_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_GET_PATH_INFO
LSA_VOID  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_GET_PATH_INFO(
	LSA_UINT16              SOCK_LOCAL_MEM_ATTR     *ret_val_ptr,
	LSA_SYS_PTR_TYPE        SOCK_LOCAL_MEM_ATTR     *sys_ptr_ptr,
	SOCK_DETAIL_PTR_TYPE    SOCK_LOCAL_MEM_ATTR     *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       path
);
#endif

/*=============================================================================
 * function name:  SOCK_RELEASE_PATH_INFO
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *                 Release Path-Info again
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 PREFIX_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_RELEASE_PATH_INFO
LSA_VOID  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_RELEASE_PATH_INFO(
	LSA_UINT16              SOCK_LOCAL_MEM_ATTR *   ret_val_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr,
	SOCK_DETAIL_PTR_TYPE                            detail_ptr
);
#endif

/*=============================================================================
 * function name:  SOCK_MEMSET
 *
 * function:       fills memory with value
 *
 * parameters:      SOCK_LOCAL_MEM_PTR_TYPE        pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_MEMSET
LSA_VOID  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_MEMSET(
	LSA_VOID                        *   pMem,
	LSA_UINT8                           Value,
	LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  SOCK_STRLEN
 *
 * function:       Returns the length of str.
 *                 The length of a C string is determined by the terminating
 *                 null-character: A C string is as long as the amount of
 *                 characters between the beginning of the string and the
 *                 terminating null character.
 *
 * parameters:     const LSA_CHAR *                    str
 *
 * return value:   LSA_UINT32
 *============================================================================*/
#ifndef SOCK_STRLEN
LSA_UINT32  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_STRLEN(
	const LSA_CHAR *                    str
);
#endif

/*=============================================================================
 * function name:  SOCK_MEMCPY
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      SOCK_COMMON_MEM_PTR_TYPE        pDst
 *                  SOCK_COMMON_MEM_PTR_TYPE        pSrc
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_MEMCPY
LSA_VOID  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_MEMCPY(
	LSA_VOID                        *  pDst,
	LSA_VOID                        *  pSrc,
	LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  SOCK_MEMMOVE
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      SOCK_COMMON_MEM_PTR_TYPE        pDst
 *                  SOCK_COMMON_MEM_PTR_TYPE        pSrc
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_MEMMOVE
LSA_VOID  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_MEMMOVE(
	LSA_VOID                        *  pDst,
	LSA_VOID                        *  pSrc,
	LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  SOCK_MEMCMP
 *
 * function:       compares Length bytes of memory from pBuf1 with pBuf2
 *
 * parameters:      SOCK_COMMON_MEM_PTR_TYPE        pBuf1
 *                  SOCK_COMMON_MEM_PTR_TYPE        pBuf2
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_TRUE:  buffers differ
 *                 LSA_FALSE: buffers are the same
 *===========================================================================*/
#ifndef SOCK_MEMCMP
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_MEMCMP(
	LSA_VOID                        *  pBuf1,
	LSA_VOID                        *  pBuf2,
	LSA_UINT32                         Length
);
#endif


/*=============================================================================
 * function name:  SOCK_ALLOC_LOCAL_MEM
 *
 * Function:       Local-memory allocation for internal administration information
 *                 of SOCK.
 *
 * parameters:     SOCK_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT16                         length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_ALLOC_LOCAL_MEM
LSA_VOID  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_ALLOC_LOCAL_MEM(
	LSA_VOID SOCK_LOCAL_MEM_PTR_TYPE  SOCK_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
	LSA_UINT16                                          length
);
#endif

/*=============================================================================
 * function name:  SOCK_FREE_LOCAL_MEM
 *
 * function:       Deallocation of a local memory.
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 SOCK_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_FREE_LOCAL_MEM
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_FREE_LOCAL_MEM(
	LSA_UINT16  SOCK_LOCAL_MEM_ATTR     *  ret_val_ptr,
	LSA_VOID    SOCK_LOCAL_MEM_PTR_TYPE    local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  SOCK_ENTER
 *
 * function:       set reentrance lock
 *                 Begin of a uninterruptible part.
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_ENTER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_ENTER(
	LSA_VOID
);
#endif

/*=============================================================================
 * function name:  SOCK_EXIT
 *
 * function:       cancel reentrance lock
 *                 End of the uninterruptible part.
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_EXIT
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_EXIT(
	LSA_VOID
);
#endif


/*=============================================================================
 * function name:  SOCK_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 SOCK_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_FATAL_ERROR
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_FATAL_ERROR(
	LSA_UINT16                   length,
	SOCK_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif

/*=============================================================================
 * function name:  SOCK_REQUEST_LOCAL
 *
 * function:       send mail
 *
 * parameters:     SOCK_UPPER_RQB_PTR_TYPE    rqb_ptr: pointer to rqb
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_REQUEST_LOCAL
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_REQUEST_LOCAL(
	SOCK_UPPER_RQB_PTR_TYPE rqb_ptr
);
#endif

/*=============================================================================
 * function name:  SOCK_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
  * parameters:    sock_request_upper_done_ptr:
 *                     pointer to sock_request_upper_done_ptr, given by
 *                     sock_open_channel()
 *                 upper_rqb_ptr:	pointer to upper-RQB
 *                 sys_ptr:			system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_REQUEST_UPPER_DONE
LSA_VOID  SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_REQUEST_UPPER_DONE(
	SOCK_UPPER_CALLBACK_FCT_PTR_TYPE  sock_request_upper_done_ptr,
	SOCK_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  SOCK_START_SELECT
 *
 * function:       start select
 *
 * parameters:
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SOCK_START_SELECT
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_START_SELECT(LSA_VOID);
#endif

/*=============================================================================
 * function name:  SOCK_DNS_LOOKUP
 *
 *
 * parameters:     name
 *
 * return value:   retval, ip
 *===========================================================================*/
#ifndef SOCK_DNS_LOOKUP
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_DNS_LOOKUP(
	LSA_UINT16 * retval, LSA_UINT32 * ip, char * name
);
#endif

/*=============================================================================
 * function name:  SOCK_DNS_QUERY
 *
 *
 * parameters:     name
 *
 * return value:   retval, ip
 *===========================================================================*/
#ifndef SOCK_DNS_QUERY
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_DNS_QUERY(
	LSA_UINT16 * retval, char * name, LSA_UINT32 * ip
);
#endif

/*=============================================================================
 * function name:  SOCK_DNS_SERVERS
 *
 * parameters:
 *
 * return value:   retval = 1, if at least one server exists in the server
 *                 structure
 *===========================================================================*/
#ifndef SOCK_DNS_SERVERS
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_DNS_SERVERS(
	LSA_UINT16 * retval
);
#endif

/*=============================================================================
 * function name:  SOCK_ALLOC_TIMER
 *
 * parameters:      timer_type, time_base
 *
 * return value:    ret_val_ptr, timer_id_ptr,
 *===========================================================================*/
#ifndef SOCK_ALLOC_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_ALLOC_TIMER(
		LSA_UINT16 SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
		LSA_TIMER_ID_TYPE SOCK_LOCAL_MEM_ATTR * timer_id_ptr,
		LSA_UINT16 timer_type,
		LSA_UINT16 time_base
);
#endif

/*=============================================================================
 * function name:  SOCK_FREE_TIMER
 *
 * parameters:      timer_id
 *
 * return value:    ret_val_ptr
 *===========================================================================*/
#ifndef SOCK_FREE_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_FREE_TIMER(
		LSA_UINT16 SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
		LSA_TIMER_ID_TYPE timer_id
);
#endif

/*=============================================================================
 * function name:  SOCK_START_TIMER
 *
 * parameters:      timer_id, user_id, time
 *
 * return value:    ret_val_ptr
 *===========================================================================*/
#ifndef SOCK_START_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_START_TIMER(
		LSA_UINT16 SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
		LSA_TIMER_ID_TYPE timer_id,
		LSA_USER_ID_TYPE user_id,
		LSA_UINT16 time
);
#endif

/*=============================================================================
 * function name:  SOCK_STOP_TIMER
 *
 * parameters:      timer_id
 *
 * return value:    ret_val_ptr
 *===========================================================================*/
#ifndef SOCK_STOP_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_STOP_TIMER(
		LSA_UINT16 SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
		LSA_TIMER_ID_TYPE timer_id
);
#endif


/*=============================================================================
 * function name:  SOCK_GET_ANY_MAC_ADDR
 *
 * parameters:      ptr_interface_id, ptr_mac_addr, mac_addr_size
 *
 * return value:
 *===========================================================================*/
#ifndef SOCK_GET_ANY_MAC_ADDR
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_GET_ANY_MAC_ADDR(
		LSA_UINT32*     ptr_interface_id,
		LSA_UINT8       SOCK_SYS_DATA_ATTR  *ptr_mac_addr,
		LSA_UINT32      mac_addr_size
);
#endif




/*=============================================================================
 * function name:  SOCK_NTOHS
 *
 * function:       converts a UINT16 value from network to host
 *
 * parameters:     LSA_UINT16
 *
 * return value:   LSA_UINT16
 *===========================================================================*/
//TODO: ist auch in sock_usr.h definiert ?
#ifndef SOCK_NTOHS
LSA_UINT16 SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_NTOHS(
		LSA_UINT16   para
);
#endif

/*=============================================================================
 * function name:  SOCK_NTOHL
 *
 * function:       converts a UINT32 value from network to host
 *
 * parameters:     LSA_UINT32
 *
 * return value:   LSA_UINT32
 *===========================================================================*/
#ifndef SOCK_NTOHL
LSA_UINT32 SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_NTOHL(
		LSA_UINT32   para
);
#endif

/*=============================================================================
 * function name:  SOCK_HTONS
 *
 * function:       converts a UINT16 value from host to network
 *
 * parameters:     LSA_UINT16
 *
 * return value:   LSA_UINT16
 *===========================================================================*/
#ifndef SOCK_HTONS
LSA_UINT16 SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_HTONS(
		LSA_UINT16   para
);
#endif


/*=============================================================================
 * function name:  SOCK_HTONL
 *
 * function:       converts a UINT32 value from host to network
 *
 * parameters:     LSA_UINT32
 *
 * return value:   LSA_UINT32
 *===========================================================================*/
#ifndef SOCK_HTONL
LSA_UINT32 SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_HTONL(
		LSA_UINT32   para
);
#endif

LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_fatal(
	LSA_UINT16 sock_module_id,
	LSA_INT line
);

#ifndef SOCK_FATAL 
#define SOCK_FATAL()  sock_fatal(SOCK_MODULE_ID, __LINE__)
#endif

LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_fatal1(
	LSA_UINT16 sock_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
);

LSA_VOID  SOCK_LOCAL_FCT_ATTR
socks_fatal_error(
	LSA_UINT16 module,
	LSA_UINT16 line,
	LSA_UINT16 error_data_length,
	LSA_VOID_PTR_TYPE error_data_ptr,
	LSA_UINT32 error_code0,
	LSA_UINT32 error_code1,
	LSA_UINT32 error_code2,
	LSA_UINT32 error_code3
);


#define SOCK_ASSERT(cond_) { if (!(cond_)) { socks_fatal_error(LTRC_ACT_MODUL_ID, (LSA_UINT16)__LINE__,0,LSA_NULL,0,0,0,0); } }


#if !SOCK_CFG_INTERNICHE

#ifndef SOCK_SET_IPSUITE
LSA_BOOL SOCK_SET_IPSUITE(
	LSA_UINT32 interface_id,  /* id to requesting interface */
	SockIF_IPAddr ip_addr,    /* IP address to be set */
	SockIF_IPAddr net_mask,   /* associated net mask */
	SockIF_IPAddr gateway     /* associated gateway address */
);
#endif

#ifndef SOCK_GET_MIB2_IFINDEX
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_GET_MIB2_IFINDEX(
	LSA_UINT32 IfID,
	LSA_UINT16 PortID,
	LSA_UINT32* p_IfIndex
);
#endif

#ifndef SOCK_SET_MIB2_IFGROUP_DATA
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_SET_MIB2_IFGROUP_DATA(
	LSA_UINT32 interface_id,
	LSA_UINT16 PortId,
	SOCK_NAME_TYPE ifDescr
);
#endif

#ifndef SOCK_SET_MIB2_SYSGROUP_DATA
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_SET_MIB2_SYSGROUP_DATA(
	SOCK_NAME_TYPE sysName,
	SOCK_NAME_TYPE sysDescr,
	SOCK_NAME_TYPE sysContact,
	SOCK_NAME_TYPE sysLocation
);
#endif


#ifndef SOCK_TCPIP_GET_TIMEPARAMS
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_TCPIP_GET_TIMEPARAMS(
	LSA_UINT32 *ptrKEEPALIVETIME,
	LSA_UINT32 *ptrCONNECTIONTIMEOUT,
	LSA_UINT32 * ptrRETRANSTIMEOUT,
	LSA_UINT32 * ptrRETRANSCOUNT
);
#endif

#ifndef SOCK_TCPIP_SET_TIMEPARAMS
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_TCPIP_SET_TIMEPARAMS(
	LSA_UINT32 KEEPALIVETIME,
	LSA_UINT32 CONNECTIONTIMEOUT,
	LSA_UINT32 RETRANSTIMEOUT,
	LSA_UINT32 RETRANSCOUNT
);
#endif

#endif


/*
	This Macro must be implemented as a single, at least uniterruptable instuction
	that returns the value that places val the location ptr and returns the
	original value at location ptr
*/
#ifndef SOCK_INTERLOCKED_EXCHANGE
SOCK_EXCHANGE_TYPE SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_INTERLOCKED_EXCHANGE(
	SOCK_EXCHANGE_TYPE volatile *ptr,
	SOCK_EXCHANGE_TYPE val
);
#endif


/*=============================================================================
 * function name:  SOCK_SNMP_AGT_PARSE
 *
 * parameters:      precv_buffer, len, psnmp_buffer, snmp_len
 *
 * return value:    pret_val
 *===========================================================================*/

#ifndef SOCK_SNMP_AGT_PARSE
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR
SOCK_SNMP_AGT_PARSE(
		LSA_UINT8 * precv_buffer,
		LSA_UINT    len,
		LSA_UINT8 * psnmp_buffer,
		LSA_UINT    snmp_len,
		LSA_UINT16  SOCK_LOCAL_MEM_ATTR * pret_val
);
#endif


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif
