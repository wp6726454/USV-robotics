
#ifndef EDDS_DBG_H                       /* ----- reinclude-protection ----- */
#define EDDS_DBG_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_dbg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile for debugging                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  05.04.04    JS    some defines changed to prevent compiler warnings.     */
/*  10.08.04    JS    added some macros                                      */
/*  27.01.05    JS    ported to LTRC                                         */
/*  06.07.05    JS    Tracelevel warning added.                              */
/*  19.10.05    JS    LSA_TRACE_LEVEL_WARNING to LSA_TRACE_LEVEL_WARN        */
#endif
/*****************************************************************************/



/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

#ifdef EDDS_CFG_DEBUG_ASSERT
#define EDDS_ASSERT_NULL_PTR(pPtr) \
{                                                         \
if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )          \
{                                                     \
EDDS_FatalError( EDDS_FATAL_ERR_NULL_PTR,         \
EDDS_MODULE_ID,                    \
__LINE__);                        \
	while(1) {}	\
}                                                     \
}
#else
#define EDDS_ASSERT_NULL_PTR(pPtr) {}
#endif

#ifdef EDDS_CFG_DEBUG_ASSERT
#define EDDS_ASSERT(Cond_) \
{                                                         \
if( (Cond_) )                                         \
{                                                     \
EDDS_FatalError( EDDS_FATAL_ERR_INCONSISTENCE,     \
EDDS_MODULE_ID,                    \
__LINE__);                        \
}                                                     \
}
#else
#define EDDS_ASSERT(Condition) {}
#endif

#ifdef EDDS_CFG_FREE_ERROR_IS_FATAL
#define EDDS_FREE_CHECK(RetVal) \
{                                                         \
if ( (RetVal) != LSA_RET_OK )                     \
EDDS_FatalError(EDDS_FATAL_ERR_MEM_FREE,  \
EDDS_MODULE_ID,             \
__LINE__);                  \
}
#else
#define EDDS_FREE_CHECK(RetVal) {}
#endif


/*---------------------------------------------------------------------------*/
/* Reentrance check macros                                                  */
/*---------------------------------------------------------------------------*/

#ifdef EDDS_CFG_REENTRANCE_CHECK
#define EDDS_ENTER_CHECK_REQUEST(pRQB,pRentrPrio) EDDS_EnterCheckRequest(pRQB,pRentrPrio)
#define EDDS_EXIT_CHECK_REQUEST(RentrPrio)      EDDS_ExitCheckRequest(RentrPrio)
#define EDDS_ENTER_CHECK_INTERRUPT() EDDS_EnterCheckISR()
#define EDDS_EXIT_CHECK_INTERRUPT()  EDDS_ExitCheckISR()
#define EDDS_ENTER_CHECK_TIMER()     EDDS_EnterCheckTimer()
#define EDDS_EXIT_CHECK_TIMER()      EDDS_ExitCheckTimer()

#define EDDS_RENTR_PRIO_NONE      0
#define EDDS_RENTR_PRIO_LOW       1
#define EDDS_RENTR_PRIO_MIDDLE    2
#define EDDS_RENTR_PRIO_HIGH      3
#define EDDS_RENTR_PRIO_TIMER     4
#define EDDS_RENTR_PRIO_INT       5

#else
#define EDDS_ENTER_CHECK_REQUEST(pRQB,pRentrPrio) {}
#define EDDS_EXIT_CHECK_REQUEST(RentrPrio)       {}
#define EDDS_ENTER_CHECK_INTERRUPT()  {}
#define EDDS_EXIT_CHECK_INTERRUPT()   {}
#define EDDS_ENTER_CHECK_TIMER()      {}
#define EDDS_EXIT_CHECK_TIMER()       {}
#endif

/*---------------------------------------------------------------------------*/
/* Debugging (enables traces)                                                */
/*                                                                           */
/* meaning see edd_cfg.h                                                    */
/*---------------------------------------------------------------------------*/

#ifdef EDDS_CFG_DEBUG_ENABLE

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DbgDumpFrame(
    LSA_INT8  EDDS_LOCAL_MEM_ATTR    * pString,
    LSA_UINT8 EDDS_LOWER_MEM_ATTR    * pFrame,
    LSA_UINT32                         Len);

#if EDDS_CFG_DEBUG_DUMP_FRAME
#define EDDS_DBG_DUMP(pString,pFrame,Len)              EDDS_DbgDumpFrame(pString,pFrame,Len)
#else
#define EDDS_DBG_DUMP(pString,pFrame,Len)                    {}
#endif
#define EDDS_IS_VALID_PTR_HANDLER                            {}
#else
#define EDDS_DBG_DUMP(pString,pFrame,Len)                    {}
#define EDDS_IS_VALID_PTR_HANDLER                            {}
#endif




#if defined(_lint)
#define EDDS_IS_VALID_PTR_HANDLER(ptr)  if(LSA_HOST_PTR_ARE_EQUAL(LSA_NULL,ptr))\
                                        {\
                                            EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,EDDS_MODULE_ID,__LINE__);\
                                        }
#endif

#define EDDS_IS_VALID_PTR(ptr) EDDS_IS_VALID_PTR_HANDLER(ptr)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/

#endif  /* of EDDS_DBG_H */
/*****************************************************************************/
/*  end of file EDDS_DBG.H                                                */
/*****************************************************************************/