#ifndef NARE_OUT_H                     /* ----- reinclude-protection ----- */
#define NARE_OUT_H

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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_out.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  NARE output macros framework. Have to be filled by systemadaption.       */
/*  This file declares prototypes with no function if macro is not defined   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Output-functions for System                                               */
/*                                                                           */
/*****************************************************************************/

/*=============================================================================
 * function name:  NARE_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 NARE_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_GET_PATH_INFO
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_GET_PATH_INFO(
    LSA_UINT16              NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        NARE_LOCAL_MEM_ATTR *  sys_ptr_ptr,
    NARE_DETAIL_PTR_TYPE    NARE_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                              path
);
#endif

/*=============================================================================
 * function name:  NARE_RELEASE_PATH_INFO
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
 *                 NARE_DETAIL_PTR_TYPE  detail_ptr:  detail-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_RELEASE_PATH_INFO
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_RELEASE_PATH_INFO(
    LSA_UINT16              NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr,
    NARE_DETAIL_PTR_TYPE  detail_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_ALLOC_TIMER
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
#ifndef NARE_ALLOC_TIMER
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_ALLOC_TIMER(
    LSA_UINT16         NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE  NARE_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                                timer_type,
    LSA_UINT16                                time_base
);
#endif

/*=============================================================================
 * function name:  NARE_START_TIMER
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
 *                                 system will call NARE_timeout().
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of prefix
 *                 LSA_UINT16          time:      running time
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_START_TIMER
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_START_TIMER(
    LSA_UINT16        NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                        timer_id,
    LSA_USER_ID_TYPE                         user_id,
    LSA_UINT16                               time
);
#endif

/*=============================================================================
 * function name:  NARE_STOP_TIMER
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
 *                 LSA_TIMER_ID_TYPE  timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_STOP_TIMER
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_STOP_TIMER(
    LSA_UINT16  NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                  timer_id
);
#endif

/*=============================================================================
 * function name:  NARE_FREE_TIMER
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
 *                 LSA_TIMER_ID_TYPE  timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_FREE_TIMER
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_FREE_TIMER(
    LSA_UINT16  NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                  timer_id
);
#endif


/*=============================================================================
 * function name:  NARE_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 NARE_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_FATAL_ERROR
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_FATAL_ERROR(
    LSA_UINT16                 length,
    NARE_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_ENTER
 *
 * function:       Protect sequence within NARE from beeing interrupted.
 *                 (reentrance protection)
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_ENTER
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_ENTER(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  NARE_EXIT
 *
 * function:       Finishes NARE_ENTER-Sequence.
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_EXIT
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_EXIT(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  NARE_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * parameters:     NARE_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_ALLOC_LOCAL_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_ALLOC_LOCAL_MEM(
    NARE_LOCAL_MEM_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
    LSA_UINT32                                      length
);
#endif

/*=============================================================================
 * function name:  NARE_FREE_LOCAL_MEM
 *
 * function:       frees local -memory
 *
 * parameters:              ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 NARE_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_FREE_LOCAL_MEM
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_FREE_LOCAL_MEM(
    LSA_UINT16                 NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_LOCAL_MEM_PTR_TYPE                           local_mem_ptr
);
#endif

/*=============================================================================
 * function name:   NARE_DBG_INIT
 *
 * function:        initialize the Debug-Interface
 *
 * parameters:      LSA_VOID
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef NARE_DBG_INIT
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DBG_INIT(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:   NARE_DBG_WRITE_BYTE
 *
 * function:        Writes byte to debug-Interface
 *
 * parameters:      LSA_UINT8 Value
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef NARE_DBG_WRITE_BYTE
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DBG_WRITE_BYTE(
    LSA_UINT8       Value
);
#endif


/*=============================================================================
 * function name:  NARE_RQB_ERROR
 *
 * function:       notify a RQB-error in a user/system-request.
 *                 this function is called if there is a error (i.e. param)
 *                 within RQB which prevents notification of the caller
 *                 (i.e. missing call-back-function). the error-code is set
 *                 in "Status" in RQB. this error is typically a implementation
 *                 error.
 *
 * parameters:     NARE_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef NARE_RQB_ERROR
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_RQB_ERROR(
    NARE_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  NARE_MEMSET_LOCAL
 *
 * function:       fills memory with value
 *
 * parameters:      NARE_LOCAL_MEM_PTR_TYPE        pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_MEMSET_LOCAL
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_MEMSET_LOCAL(
    NARE_LOCAL_MEM_PTR_TYPE             pMem,
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  NARE_MEMCOPY_UPPER
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      NARE_UPPER_MEM_PTR_TYPE         pDst
 *                  NARE_UPPER_MEM_PTR_TYPE         pSrc
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_MEMCOPY_UPPER
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_MEMCOPY_UPPER(
    NARE_UPPER_MEM_PTR_TYPE              pDst,
    NARE_UPPER_MEM_PTR_TYPE              pSrc,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  NARE_MEMCOPY_COMMON
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      NARE_COMMON_MEM_PTR_TYPE        pDst
 *                  NARE_COMMON_MEM_PTR_TYPE        pSrc
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_MEMCOPY_COMMON
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_MEMCOPY_COMMON(
    NARE_COMMON_MEM_PTR_TYPE            pDst,
    NARE_COMMON_MEM_PTR_TYPE            pSrc,
    LSA_UINT32                          Length
);
#endif


/*=============================================================================
 * function name:  NARE_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper-RQB for local use
 *
 * parameters:     NARE_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT16                      length:   length of RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_ALLOC_UPPER_RQB_LOCAL
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_ALLOC_UPPER_RQB_LOCAL(
    NARE_UPPER_RQB_PTR_TYPE  NARE_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
    LSA_UINT16                                    length
);
#endif


/*=============================================================================
 * function name:  NARE_FREE_UPPER_RQB_LOCAL
 *
 * function:       free an upper-RQB allocated with NARE_ALLOC_UPPER_RQB_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-RQB or
 *                 NARE_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_FREE_UPPER_RQB_LOCAL
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16                 NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
    NARE_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr
);
#endif

/*=============================================================================
 * function name:  NARE_DO_TIMER_REQUEST
 *
 * function:       Does a nare_system() request with the spezified RQB. This
 *                 function is called within NARE timer handler to signal
 *                 a Timer event to the main context of NARE.
 *                 Systemadaption has to call nare_system() with the specified
 *                 RQB. Before calling, the systemadaption should change to
 *                 taskstate.
 *
 * parameters:     NARE_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_DO_TIMER_REQUEST
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_DO_TIMER_REQUEST(
    NARE_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  NARE_ARP_RESPONSE_FRAME_CONTROL
 *
 * function:       Controls ARP-Sniffing
 *                 If turned on, systemadaption has to call
 *                 nare_ARPResponseFrameIn() with incoming frames for this
 *                 channel, till sniffer turned off.
 *
 * parameters:     LSA_UINT16                   NICId of EDD-Systemchannel
 *                 LSA_BOOL                     On
 *                                              LSA_TRUE: Turn on sniffer
 *                                              LSA_FALSE:Turn off sniffer
 *
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_ARP_RESPONSE_FRAME_CONTROL
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_ARP_RESPONSE_FRAME_CONTROL(
    LSA_UINT16                      NICId,
    LSA_BOOL                        On
);
#endif

/*=============================================================================
 * function name:  NARE_ARP_RESPONSE_FRAME_OUT
 *
 * function:       Returns a frame given to NARE via nare_ARPResponseFrameIn()
 *                 back to systemadaption. Called within nare_ARPResponseFrameIn()
 *
 * parameters:     LSA_UINT16                   NICId
 *                 NARE_EDD_LOWER_MEM_PTR_TYPE  pFrame
 *                 LSA_UINT16                   FrameLen
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef NARE_ARP_RESPONSE_FRAME_OUT
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_ARP_RESPONSE_FRAME_OUT(
    LSA_UINT16                      NICId,
    NARE_EDD_LOWER_MEM_PTR_TYPE     pFrame,
    LSA_UINT16                      FrameLen
);
#endif

/*=============================================================================
 * function name:  NARE_NTOHS
 *
 * function:       converts a UINT16 value from network to host
 *
 * parameters:     LSA_UINT16
 *
 * return value:   LSA_UINT16
 *===========================================================================*/

/* Note: will be implemented within NARE with general handling if not defined */
/*       by systemadaption.                                                   */

/*=============================================================================
 * function name:  NARE_NTOHL
 *
 * function:       converts a UINT32 value from network to host
 *
 * parameters:     LSA_UINT32
 *
 * return value:   LSA_UINT32
 *===========================================================================*/

/* Note: will be implemented within NARE with general handling if not defined */
/*       by systemadaption.                                                   */

/*=============================================================================
 * function name:  NARE_HTONS
 *
 * function:       converts a UINT16 value from host to network
 *
 * parameters:     LSA_UINT16
 *
 * return value:   LSA_UINT16
 *===========================================================================*/

/* Note: will be implemented within NARE with general handling if not defined */
/*       by systemadaption.                                                   */

/*=============================================================================
 * function name:  NARE_HTONL
 *
 * function:       converts a UINT32 value from host to network
 *
 * parameters:     LSA_UINT32
 *
 * return value:   LSA_UINT32
 *===========================================================================*/

/* Note: will be implemented within NARE with general handling if not defined */
/*       by systemadaption.                                                   */


/*****************************************************************************/
/*  end of file NARE_OUT.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_OUT_H */
