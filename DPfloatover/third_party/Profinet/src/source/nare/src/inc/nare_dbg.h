
#ifndef NARE_DBG_H                       /* ----- reinclude-protection ----- */
#define NARE_DBG_H

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
/*  F i l e               &F: nare_dbg.h                                :F&  */
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

#ifdef NARE_CFG_DEBUG_ASSERT
#define NARE_ASSERT_NULL_PTR(pPtr) \
{                                                         \
    if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )          \
    {                                                     \
        NAREFatalError( NARE_FATAL_ERR_NULL_PTR,          \
                        NARE_MODULE_ID,                   \
                        __LINE__);                        \
    }                                                     \
}
#else
#define NARE_ASSERT_NULL_PTR(pPtr) {}
#endif

#ifdef NARE_CFG_DEBUG_ASSERT
#define NARE_ASSERT_FALSE(Cond_)  \
{                                                         \
    if( ! (Cond_) )                                       \
    {                                                     \
        NAREFatalError( NARE_FATAL_ERR_INCONSISTENZ,      \
                        NARE_MODULE_ID,                   \
                        __LINE__);                        \
    }                                                     \
}
#else
#define NARE_ASSERT_FALSE(Condition) {}
#endif

#ifdef NARE_CFG_DEBUG_ASSERT
#define NARE_ASSERT_TRUE(Cond_)  \
{                                                         \
    if( (Cond_) )                                         \
    {                                                     \
        NAREFatalError( NARE_FATAL_ERR_INCONSISTENZ,      \
                        NARE_MODULE_ID,                   \
                        __LINE__);                        \
    }                                                     \
}
#else
#define NARE_ASSERT_TRUE(Condition) {}
#endif


/*****************************************************************************/
/*  end of file NARE_DBG.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_DBG_H */
