
#ifndef LLDP_CFG_H                       /* ----- reinclude-protection ----- */
#define LLDP_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for LLDP:                                                  */
/*  Defines constants, types and macros for LLDP.                            */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  05.04.04    JS    initial version.                                       */
/*  13.10.04    JS    added support of external LSA- RQB HEADER - defines.   */
/*  01.06.05    JS    EDD_RQB_SET/GET_STATUS replaced by SET/GET_RESPONSE.   */
/*  27.09.05    JS    changed default of LLDP_CFG_TX_DELAY to 0s.            */
/*                    changed default of LLDP_CFG_USE_STOP_DELAY to #undef   */
/*                    changed default of LLDP_CFG_TX_REINIT_DELAY to 0s      */
/*  27.06.06    JS    added  LLDP_FILE_SYSTEM_EXTENSION                      */
/*  12.10.06    JS    using LSA RQB Header definitions                       */
/*  09.01.07    JS    added LLDP_CFG_NRT_SEND_PRIO                           */
/*  19.02.09    JS    changed default of LLDP_CFG_NRT_SEND_PRIO              */
/*  16.06.11    TB    EDD_NRT_SEND_PRIO_ORG -> EDD_NRT_SEND_PRIO_MGMT_LOW    */
#endif
/*****************************************************************************/

void lldp_psi_startstop( int start );

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Maximum number of user-channels                                           */
/*---------------------------------------------------------------------------*/

#ifndef LLDP_CFG_MAX_CHANNELS
# error "LLDP_CFG_MAX_CHANNELS not defined" /* max number of user-channels */
#endif

/*---------------------------------------------------------------------------*/
/* Set some local_mem static or dynamic:                                     */
/* If LLDP_CFG_LOCAL_MEM_STATIC is not defined, all local-memory will be     */
/* allocated dynamically. If defined, some local memory is static.           */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_LOCAL_MEM_STATIC

/*---------------------------------------------------------------------------*/
/* If defined, all frame-buffers needed will be allocated on opening the     */
/* EDD systemchannel. This frame-buffers will stay till closing the channel. */
/* If not defined the frame-buffers will be allocated on need and freed      */
/* if not needed anymore (e.g. on every send)                                */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN

/*---------------------------------------------------------------------------*/
/* Number of Receive Resources used per Port. This defines the number of     */
/* EDD- Receive-Requests used per EDD-Channel. The number of Requests is     */
/* LLDP_CFG_RECV_RESOURCE_PER_PORT * PortCnt.                                */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_RECV_RESOURCE_PER_PORT   2

/*---------------------------------------------------------------------------*/
/* Defines if we should send a Shutdown-frame if stopping a started port.    */
/* If defined we send a shutdown, if not we dont. After the port is stopped  */
/* we may delay (reinitdelay) the stop-request                               */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_SEND_SHUTDOWN_ON_STOP

/*---------------------------------------------------------------------------*/
/* Advanced protection and function handling settings.                       */
/*                                                                           */
/* LLDP is designed to use the input functions lldp_timeout() without        */
/* LLDP_ENTER() and LLDP_EXIT() even if called from a priority above standard*/
/* handling 1). The requirement for this is, that lldp_timeout() can not be  */
/* intercepted by standard handling.                                         */
/* If this requirement is not met, additional protection with LLDP_ENTER()   */
/* and LLDP_EXIT() is neccessary. If so the following defines may have to    */
/* be used to include protection within this function.                       */
/* LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN is a little special, because it         */
/* simplyfies some handing if no context-change is neccessary.               */
/*                                                                           */
/* LLDP_CFG_USE_ENTER_EXIT_TIMEOUT                                           */
/*      defined: Uses the output macros LLDP_ENTER() and LLDP_EXIT() even    */
/*               with the timeout-handler lldp_timeout(). This must be       */
/*               used if it is neccessary to protect this function if it can */
/*               be interrupted by standard-handling 1)                      */
/*               Note that the output macro LLDP_DO_TIMER_REQUEST() is called*/
/*               within LLDP_ENTER() protected code sequence.                */
/*                                                                           */
/*      undef:   No LLDP_ENTER() and LLDP_EXIT() present within function     */
/*               lldp_timeout(). Can be used if function is not              */
/*               intercepted by standard handling 1)                         */
/*                                                                           */
/* LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN:                                        */
/*      defined: LLDP does not call the output macro LLDP_DO_TIMER_REQUEST   */
/*               within lldp_timeout() but does the neccessary standard      */
/*               handling within lldp_timeout(). This can be set if no       */
/*               context change is neccessary or the function is protected   */
/*               with LLDP_ENTER() and LLDP_EXIT().                          */
/*      undef:   LLDP uses LLDP_DO_TIMER_REQUEST within lldp_timeout() to    */
/*               trigger a context change. Within lldp_timeout() very little */
/*               is done, so no protection with LLDP_ENTER() and LLDP_EXIT() */
/*               is neccessary if lldp_timeout() is called within a priority */
/*               above standard-input functions.                             */
/*                                                                           */
/* Defaults:                                                                 */
/*                                                                           */
/*  #undef LLDP_CFG_USE_ENTER_EXIT_TIMEOUT                                   */
/*  #undef LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN                                 */
/*                                                                           */
/*                                                                           */
/* 1) Standard handling is the following:                                    */
/*    * lldp_init                                                            */
/*    * lldp_undo_init                                                       */
/*    * lldp_edd_request_lower_done                                          */
/*    * lldp_open_channel                                                    */
/*    * lldp_close_channel                                                   */
/*    * lldp_request                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#undef LLDP_CFG_USE_ENTER_EXIT_TIMEOUT
#undef LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/

#define LLDP_ATTR_NEAR
#define LLDP_ATTR_FAR
#define LLDP_ATTR_SHUGE
#define LLDP_ATTR_HUGE

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/

#define LLDP_UPPER_IN_FCT_ATTR         LLDP_ATTR_HUGE  /* LLDP-user to LLDP  */
#define LLDP_UPPER_OUT_FCT_ATTR        LLDP_ATTR_HUGE  /* LLDP to LLDP -user */
#define LLDP_EDD_LOWER_IN_FCT_ATTR     LLDP_ATTR_HUGE  /* EDD  lower component to LLDP */
#define LLDP_EDD_LOWER_OUT_FCT_ATTR    LLDP_ATTR_HUGE  /* LLDP to EDD  lower component */
#define LLDP_SYSTEM_IN_FCT_ATTR        LLDP_ATTR_FAR   /* system to LLDP */
#define LLDP_SYSTEM_OUT_FCT_ATTR       LLDP_ATTR_FAR   /* LLDP to system */
#define LLDP_LOCAL_FCT_ATTR            LLDP_ATTR_NEAR  /* local function */

/*=====  data attributes  =====*/

#define LLDP_UPPER_RQB_ATTR            LLDP_ATTR_HUGE  /* upper-rqb data         */
#define LLDP_UPPER_MEM_ATTR            LLDP_ATTR_HUGE  /* upper-memory data      */
#define LLDP_EDD_LOWER_RQB_ATTR        LLDP_ATTR_HUGE  /* DCP  lower-rqb data    */
#define LLDP_EDD_LOWER_MEM_ATTR        LLDP_ATTR_HUGE  /* DCP  lower memory data */
#define LLDP_SYSTEM_MEM_ATTR           LLDP_ATTR_HUGE  /* system-memory data     */
#define LLDP_LOCAL_MEM_ATTR            LLDP_ATTR_HUGE  /* local data             */
#define LLDP_COMMON_MEM_ATTR           LLDP_ATTR_HUGE  /* greatest common data   */
                                                       /* attribute (matches all)*/

/*---------------------------------------------------------------------------*/
/* Definition of RQB-HEADER:                                                 */
/*---------------------------------------------------------------------------*/

#define LLDP_RQB_HEADER LSA_RQB_HEADER(LLDP_UPPER_RQB_PTR_TYPE)

/*---------------------------------------------------------------------------*/
/* Definition of RQB-TRAILER                                                 */
/*                                                                           */
/* If necessary during system integration, additional parameters can be      */
/* added!                                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define LLDP_RQB_TRAILER  LSA_RQB_TRAILER

/*---------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_RQB_SET_NEXT_RQB_PTR(_pRQB, _Value)    LSA_RQB_SET_NEXT_RQB_PTR(_pRQB, _Value)
#define LLDP_RQB_GET_NEXT_RQB_PTR(_pRQB)            LSA_RQB_GET_NEXT_RQB_PTR(_pRQB)
#define LLDP_RQB_SET_PREV_RQB_PTR(_pRQB, _Value)    LSA_RQB_SET_PREV_RQB_PTR(_pRQB, _Value)
#define LLDP_RQB_GET_PREV_RQB_PTR(_pRQB)            LSA_RQB_GET_PREV_RQB_PTR(_pRQB)
#define LLDP_RQB_SET_OPCODE(_pRQB, _Value)          LSA_RQB_SET_OPCODE(_pRQB, _Value)
#define LLDP_RQB_GET_OPCODE(_pRQB)                  LSA_RQB_GET_OPCODE(_pRQB)
#define LLDP_RQB_SET_HANDLE(_pRQB, _Value)          LSA_RQB_SET_HANDLE(_pRQB, _Value)
#define LLDP_RQB_GET_HANDLE(_pRQB)                  LSA_RQB_GET_HANDLE(_pRQB)
#define LLDP_RQB_SET_USERID_UVAR32(_pRQB, _Value)   LSA_RQB_SET_USER_ID_UVAR32(_pRQB, _Value)
#define LLDP_RQB_GET_USERID_UVAR32(_pRQB)           LSA_RQB_GET_USER_ID_UVAR32(_pRQB)
#define LLDP_RQB_SET_USERID_UVAR16_1(_pRQB, _Value) LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(_pRQB, _Value)
#define LLDP_RQB_GET_USERID_UVAR16_1(_pRQB)         LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(_pRQB)
#define LLDP_RQB_SET_USERID_UVAR16_2(_pRQB, _Value) LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(_pRQB, _Value)
#define LLDP_RQB_GET_USERID_UVAR16_2(_pRQB)         LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(_pRQB)
#define LLDP_RQB_SET_RESPONSE(_pRQB, _Value)        LSA_RQB_SET_RESPONSE(_pRQB, _Value)
#define LLDP_RQB_GET_RESPONSE(_pRQB)                LSA_RQB_GET_RESPONSE(_pRQB)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to EDD                                             */
/*===========================================================================*/

#define LLDP_EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)   EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define LLDP_EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)   EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define LLDP_EDD_RQB_SET_OPCODE(rb_, val_)         EDD_RQB_SET_OPCODE(rb_, val_)
#define LLDP_EDD_RQB_SET_SERVICE(rb_, val_)        EDD_RQB_SET_SERVICE(rb_, val_)
#define LLDP_EDD_RQB_SET_HANDLE(rb_, val_)         EDD_RQB_SET_HANDLE(rb_, val_)
#define LLDP_EDD_RQB_SET_USERID_UVAR32(rb_, val_)  EDD_RQB_SET_USERID_UVAR32(rb_, val_)
#define LLDP_EDD_RQB_SET_RESPONSE(rb_, val_)       EDD_RQB_SET_RESPONSE(rb_, val_)

#define LLDP_EDD_RQB_GET_NEXT_RQB_PTR(rb_)         EDD_RQB_GET_NEXT_RQB_PTR(rb_)
#define LLDP_EDD_RQB_GET_PREV_RQB_PTR(rb_)         EDD_RQB_GET_PREV_RQB_PTR(rb_)
#define LLDP_EDD_RQB_GET_OPCODE(rb_)               EDD_RQB_GET_OPCODE(rb_)
#define LLDP_EDD_RQB_GET_SERVICE(rb_)              EDD_RQB_GET_SERVICE(rb_)
#define LLDP_EDD_RQB_GET_HANDLE(rb_)               EDD_RQB_GET_HANDLE(rb_)
#define LLDP_EDD_RQB_GET_USERID_UVAR32(rb_)        EDD_RQB_GET_USERID_UVAR32(rb_)
#define LLDP_EDD_RQB_GET_RESPONSE(rb_)             EDD_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* macro name:    LLDP_FILE_SYSTEM_EXTENSION(module_id_)                     */
/*                                                                           */
/* macro:         System integrators can use the macro to smuggle something  */
/*                into the component source files. By default it's empty.    */
/*                                                                           */
/*                The system integrators are responsible for the contents of */
/*                that macro.                                                */
/*                                                                           */
/*                Internal data of LSA components may not be changed!        */
/*                                                                           */
/* parameter:     Module-id                                                  */
/*                                                                           */
/*                There's no prototype for that macro!                       */
/*===========================================================================*/

#define LLDP_FILE_SYSTEM_EXTENSION(module_id_)

/*===========================================================================*/
/* LTRC - Support (LSA-Trace)                                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define the Trace mode for LLDP                                            */
/* 0: no traces or external traces (default)                                 */
/* 1: enable Traces and use LTRC (LSA-Trace module) see LLDP_ltrc.h          */
/* 2: enables full index trace  (LSA-index Trace module)                     */
/*---------------------------------------------------------------------------*/

#ifndef LLDP_CFG_TRACE_MODE
# error "LLDP_CFG_TRACE_MODE not defined" 
#endif


/*===========================================================================*/
/* Debugging (for developer use)                                             */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define to enable spezial additonal assertion-checks (e.g. NULL-Ptr)       */
/* This assertion will cause an fatal-error.                                 */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_DEBUG_ASSERT /* default: enabled */

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*****************************************************************************/
/*  end of file LLDP_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of LLDP_CFG_H */
