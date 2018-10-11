#ifndef EPS_LOCKS_H_
#define EPS_LOCKS_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_locks.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_LOCK_ID_INVALID   0

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID eps_locks_init ( LSA_VOID );
LSA_VOID eps_locks_undo_init ( LSA_VOID );

LSA_RESPONSE_TYPE eps_alloc_critical_section ( LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive);
LSA_RESPONSE_TYPE eps_free_critical_section ( LSA_UINT16 lock_id );

LSA_VOID eps_enter_critical_section ( LSA_UINT16 lock_id );
LSA_VOID eps_exit_critical_section ( LSA_UINT16 lock_id );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_LOCKS_H_ */