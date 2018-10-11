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
/*  C o m p o n e n t     &C: pcIOX (PNIO Controler, Device and more)   :C&  */
/*                                                                           */
/*  F i l e               &F: mem3defaultmac.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MEM3 memory management                                                   */
/*                                                                           */
/*****************************************************************************/

#ifndef MEM3DEFAULTMAC_H
#define MEM3DEFAULTMAC_H

#ifndef MEM3_ASSERT
  #define MEM3_ASSERT(       BoolExp )  assert (BoolExp)
#endif

#ifndef MEM3_USER_ASSERT
  #define MEM3_USER_ASSERT(  BoolExp )  MEM3_ASSERT (BoolExp)
#endif

#ifndef MEM3_INNER_ASSERT
  #define MEM3_INNER_ASSERT( BoolExp )  MEM3_ASSERT (BoolExp)
#endif

#ifndef MEM3_MEMCPY
  #define MEM3_MEMCPY(       pDst, pSrc, ByteNum )  memcpy (pDst, pSrc, ByteNum)
#endif

#ifndef MEM3_MEMSET
  #define MEM3_MEMSET(       pDst, Val8, ByteNum )  memset (pDst, Val8, ByteNum)
#endif

#ifndef MEM3_ADMIN_MEMSET
  #define MEM3_ADMIN_MEMSET( pDst, Val8, ByteNum )  MEM3_MEMSET (pDst, Val8, ByteNum)
#endif

#ifndef MEM3_POOL_MEMSET_FUNCTION
  #define MEM3_POOL_MEMSET_FUNCTION                 memset
#endif

#ifndef MEM3_POOL_ASSIGN_UINT_FUNCTION
  #define MEM3_POOL_ASSIGN_UINT_FUNCTION            MEMORY3_INTERN::PoolAssignUint
#endif

#ifndef MEM3_POOL_GET_UINT_FUNCTION
  #define MEM3_POOL_GET_UINT_FUNCTION               MEMORY3_INTERN::PoolGetUint
#endif

#ifndef MEM3_POOL_ASSIGN_UINT32_LE_MA_FUNCTION
  #define MEM3_POOL_ASSIGN_UINT32_LE_MA_FUNCTION    MEMORY3_INTERN::PoolAssignUint32_LeMa
#endif

#ifndef MEM3_POOL_GET_UINT32_LE_MA_FUNCTION
  #define MEM3_POOL_GET_UINT32_LE_MA_FUNCTION       MEMORY3_INTERN::PoolGetUint32_LeMa
#endif

#ifdef  MEM3DEBUG
  #define  MEM3_OPTIONAL_INNER_ASSERT( BoolExp )  MEM3_INNER_ASSERT (BoolExp)
#else
  #define  MEM3_OPTIONAL_INNER_ASSERT( BoolExp )
#endif

#define MEM3_ARR_LEN( Arr ) ((int) ((sizeof (Arr)) / (sizeof ((Arr) [0]))))

#if (   (  defined (Mem3_TraceAlloc) && ! defined (Mem3_TraceFree)) \
     || (! defined (Mem3_TraceAlloc) &&   defined (Mem3_TraceFree)))
  #error inconsistent trace definition
#endif

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/