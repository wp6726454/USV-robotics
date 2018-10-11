#ifndef EPS_MEM_H_
#define EPS_MEM_H_

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
/*  F i l e               &F: eps_mem.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Memory                                                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_RESPONSE_TYPE eps_mem_init ( LSA_UINT32* pStdMemBase, LSA_UINT32 uStdMemSize, LSA_UINT32* pFastMemBase, LSA_UINT32 uFastMemSize );
LSA_VOID          eps_mem_undo_init ( LSA_VOID );    

LSA_VOID_PTR_TYPE eps_mem_alloc_from_pool ( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool );
LSA_UINT16        eps_mem_free_from_pool ( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool );

LSA_VOID_PTR_TYPE eps_mem_alloc ( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type );
LSA_UINT16        eps_mem_free ( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type );

LSA_VOID_PTR_TYPE eps_fast_mem_alloc ( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type );
LSA_UINT16        eps_fast_mem_free ( LSA_VOID_PTR_TYPE ptr );

LSA_VOID          eps_mem_lock  ( LSA_VOID );
LSA_VOID          eps_mem_unlock( LSA_VOID );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_MEM_H_ */