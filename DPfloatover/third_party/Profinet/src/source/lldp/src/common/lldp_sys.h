
#ifndef LLDP_SYS_H                       /* ----- reinclude-protection ----- */
#define LLDP_SYS_H

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
/*  F i l e               &F: lldp_sys.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for lldp                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  06.04.04    JS    initial version.                                       */
/*  30.06.04    JS    comment about initializing of memory with 0 removed.   */
/*  15.07.04    JS    PortCnt removed from CDB. Got it from EDD now.         */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* fatal-errorcodes (used for LLDPFatalError())                              */
/*---------------------------------------------------------------------------*/

#define LLDP_FATAL_ERROR_TYPE  LSA_UINT32

#define LLDP_FATAL_ERR_NULL_PTR            (LLDP_FATAL_ERROR_TYPE)  0x01
#define LLDP_FATAL_ERR_MEM_FREE            (LLDP_FATAL_ERROR_TYPE)  0x02
#define LLDP_FATAL_ERR_RELEASE_PATH_INFO   (LLDP_FATAL_ERROR_TYPE)  0x03
#define LLDP_FATAL_ERR_INCONSISTENZ        (LLDP_FATAL_ERROR_TYPE)  0x04
#define LLDP_FATAL_ERR_TIMER_RESOURCE      (LLDP_FATAL_ERROR_TYPE)  0x05
#define LLDP_FATAL_ERR_HANDLE_INVALID      (LLDP_FATAL_ERROR_TYPE)  0x06
#define LLDP_FATAL_ERR_EDD                 (LLDP_FATAL_ERROR_TYPE)  0x07

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/
#define LLDP_DETAIL_PTR_TYPE                 /* pointer to LLDP_DETAIL */ \
struct lldp_detail_tag      LLDP_SYSTEM_MEM_ATTR *

#define LLDP_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
struct lsa_fatal_error_tag  LLDP_LOCAL_MEM_ATTR *



/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                   CDB (Channel Description Block)                         */
/*                     (used by LLDP_GET_PATH_INFO)                          */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* CDB- Channel Description Block                                            */
/*      (used as detail-ptr in LLDP_GET_PATH_INFO)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct lldp_detail_tag
{
    LSA_UINT8   dummy;
} LLDP_CDB_TYPE;

#define LLDP_PORT_CNT_MAX  0xFF   /* max port cnt supported (255) = PortID 1..255*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                              external functions                           */
/*===========================================================================*/


/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  lldp_init
 *
 * function:       initialize LLDP
 *
 * parameters:     ....
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/

LSA_UINT16  LLDP_SYSTEM_IN_FCT_ATTR lldp_init(LSA_VOID);

/*=============================================================================
 * function name:  lldp_undo_init
 *
 * function:       undo the initialization of LLDP
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/

LSA_UINT16  LLDP_SYSTEM_IN_FCT_ATTR  lldp_undo_init(LSA_VOID);

/*=============================================================================
 * function name:  lldp_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_TIMER_ID_TYPE timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of prefix
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  LLDP_SYSTEM_IN_FCT_ATTR  lldp_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id);


/*****************************************************************************/
/*                                                                           */
/* Output-functions/macros                                                   */
/*                                                                           */
/*****************************************************************************/

/*=============================================================================
 * function name:  LLDP_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 LLDP_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_GET_PATH_INFO
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_GET_PATH_INFO(
    LSA_UINT16              LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        LLDP_LOCAL_MEM_ATTR *  sys_ptr_ptr,
    LLDP_DETAIL_PTR_TYPE    LLDP_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                              path
);
#endif


/*=============================================================================
 * function name:  LLDP_RELEASE_PATH_INFO
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
 *                 LLDP_DETAIL_PTR_TYPE  detail_ptr:  detail-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_RELEASE_PATH_INFO
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_RELEASE_PATH_INFO(
    LSA_UINT16              LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr,
    LLDP_DETAIL_PTR_TYPE  detail_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_ALLOC_TIMER
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
#ifndef LLDP_ALLOC_TIMER
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_ALLOC_TIMER(
    LSA_UINT16         LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE  LLDP_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                                timer_type,
    LSA_UINT16                                time_base
);
#endif

/*=============================================================================
 * function name:  LLDP_START_TIMER
 *
 * function:       start a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                  timer has been
 *                                                             started
 *                                 LSA_RET_OK_TIMER_RESTARTED  timer has been
 *                                                             restarted
 *                                 LSA_RET_ERR_PARAM           timer hasn´t
 *                                                             been started
 *                                                             because of wrong
 *                                                             timer-id
 *                                 After the expiration of the running time
 *                                 system will call LLDP_timeout().
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of prefix
 *                 LSA_UINT16          time:      running time
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_START_TIMER
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_START_TIMER(
    LSA_UINT16        LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                        timer_id,
    LSA_USER_ID_TYPE                         user_id,
    LSA_UINT16                               time
);
#endif

/*=============================================================================
 * function name:  LLDP_STOP_TIMER
 *
 * function:       stop a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                    timer has been
 *                                                               stopped
 *                                 LSA_RET_OK_TIMER_NOT_RUNNING  timer was not
 *                                                               running; timer
 *                                                               is stopped
 *                                 LSA_RET_ERR_PARAM             timer hasn´t
 *                                                               stopped
 *                                                               because
 *                                                               of wrong
 *                                                               timer-id
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_STOP_TIMER
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_STOP_TIMER(
    LSA_UINT16  LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                  timer_id
);
#endif

/*=============================================================================
 * function name:  LLDP_FREE_TIMER
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
#ifndef LLDP_FREE_TIMER
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_FREE_TIMER(
    LSA_UINT16  LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                  timer_id
);
#endif


/*=============================================================================
 * function name:  LLDP_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 LLDP_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_FATAL_ERROR
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_FATAL_ERROR(
    LSA_UINT16                 length,
    LLDP_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif

/*=============================================================================
 * function name:  LLDP_ENTER
 *
 * function:       Protect sequence within LLDP from beeing interrupted.
 *                 (reentrance protection)
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_ENTER
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_ENTER(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  LLDP_EXIT
 *
 * function:       Finishes LLDP_ENTER-Sequence.
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_EXIT
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_EXIT(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  LLDP_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * parameters:     LLDP_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_ALLOC_LOCAL_MEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_ALLOC_LOCAL_MEM(
    LLDP_LOCAL_MEM_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
    LSA_UINT32                                      length
);
#endif

/*=============================================================================
 * function name:  LLDP_FREE_LOCAL_MEM
 *
 * function:       frees local -memory
 *
 * parameters:              ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 LLDP_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_FREE_LOCAL_MEM
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_FREE_LOCAL_MEM(
    LSA_UINT16                 LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LLDP_LOCAL_MEM_PTR_TYPE                           local_mem_ptr
);
#endif



/*=============================================================================
 * function name:  LLDP_RQB_ERROR
 *
 * function:       notify a RQB-error in a user/system-request.
 *                 this function is called if there is a error (i.e. param)
 *                 within RQB which prevents notification of the caller
 *                 (i.e. missing call-back-function). the error-code is set
 *                 in "Status" in RQB. this error is typically a implementation
 *                 error.
 *
 * parameters:     LLDP_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_RQB_ERROR
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_RQB_ERROR(
    LLDP_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  LLDP_MEMSET_LOCAL
 *
 * function:       fills memory with value
 *
 * parameters:      LLDP_LOCAL_MEM_PTR_TYPE        pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_MEMSET_LOCAL
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_MEMSET_LOCAL(
    LLDP_LOCAL_MEM_PTR_TYPE             pMem,
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  LLDP_MEMCOPY_COMMON
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      LLDP_COMMON_MEM_PTR_TYPE        pDst
 *                  LLDP_COMMON_MEM_PTR_TYPE        pSrc
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_MEMCOPY_COMMON
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_MEMCOPY_COMMON(
    LLDP_COMMON_MEM_PTR_TYPE            pDst,
    LLDP_COMMON_MEM_PTR_TYPE            pSrc,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  LLDP_MEMCMP_COMMON
 *
 * function:       compares Length bytes of memory from pBuf1 with pBuf2
 *
 * parameters:      LLDP_COMMON_MEM_PTR_TYPE        pBuf1
 *                  LLDP_COMMON_MEM_PTR_TYPE        pBuf2
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_TRUE:  buffers are the same
 *                 LSA_FALSE: buffers differ
 *===========================================================================*/

#ifndef LLDP_MEMCMP_COMMON
LSA_BOOL LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_MEMCMP_COMMON(
    LLDP_COMMON_MEM_PTR_TYPE            pBuf1,
    LLDP_COMMON_MEM_PTR_TYPE            pBuf2,
    LSA_UINT32                          Length
);
#endif



/*=============================================================================
 * function name:  LLDP_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper-RQB for local use
 *
 * parameters:     LLDP_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT16                      length:   length of RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_ALLOC_UPPER_RQB_LOCAL
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_ALLOC_UPPER_RQB_LOCAL(
    LLDP_UPPER_RQB_PTR_TYPE  LLDP_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
    LSA_UINT16                                    length
);
#endif


/*=============================================================================
 * function name:  LLDP_FREE_UPPER_RQB_LOCAL
 *
 * function:       free an upper-RQB allocated with LLDP_ALLOC_UPPER_RQB_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-RQB or
 *                 LLDP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef LLDP_FREE_UPPER_RQB_LOCAL
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16                 LLDP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LLDP_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  LLDP_DO_TIMER_REQUEST
 *
 * function:       Does a LLDP_system() request with the spezified RQB. This
 *                 function is called within LLDP timer handler to signal
 *                 a Timer event to the main context of LLDP.
 *                 Systemadaption has to call LLDP_system() with the specified
 *                 RQB. Before calling, the systemadaption should change to
 *                 taskstate.
 *
 * parameters:     LLDP_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef LLDP_DO_TIMER_REQUEST
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_DO_TIMER_REQUEST(
    LLDP_UPPER_RQB_PTR_TYPE  pRQB
);
#endif


/*============================================================================= 
 * function name:  LLDP_REQUEST_UPPER_DONE 
 * 
 * function:       returns a finished request to the upper layer 
 * 
 * parameters:     LLDP_UPPER_CALLBACK_FCT_PTR_TYPE  
 *                                            prefix_request_upper_done_ptr: 
 *                     pointer to prefix_request_upper_done_ptr, given by 
 *                     prefix_open_channel() 
 * 
 *                 LLDP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper- 
 *                                                            RQB 
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 * 
 * return value:   LSA_VOID 
 * 
 *===========================================================================*/
typedef  LSA_VOID  LSA_FCT_PTR( LLDP_UPPER_OUT_FCT_ATTR,
                               LLDP_UPPER_CALLBACK_FCT_PTR_TYPE)(    
                               LLDP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr);

/* #define LLDP_REQUEST_UPPER_DONE(cbf,rqb,sys_ptr) cbf(rqb); for use in old environments */

#ifndef LLDP_REQUEST_UPPER_DONE
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_REQUEST_UPPER_DONE(    
       LLDP_UPPER_CALLBACK_FCT_PTR_TYPE  lldp_request_upper_done_ptr,    
       LLDP_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,    
       LSA_SYS_PTR_TYPE                  sys_ptr
);
#endif

/*****************************************************************************/
/*  end of file LLDP_SYS.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of LLDP_SYS_H */
