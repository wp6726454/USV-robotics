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
/*  F i l e               &F: mem3if.cpp                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of MEM3 memory management                                 */
/*                                                                           */
/*****************************************************************************/

#define MEM3_INITIAL_SOURCE_MODUL
#include "mem3pool.hpp"
#undef  MEM3_INITIAL_SOURCE_MODUL

cPoolAdmin <POOL_NUM>   cMemPool::sPoolAdmin;

void  *
cMemPool::  operator new  (size_t  Size, ORD32 *&  pPool)
  {
    ORD32 *  pTmpPool  = pPool;
    pPool              = pTmpPool + (Size + sizeof (ORDHW) - 1) / sizeof (ORDHW);

    return pTmpPool;
  }

cMemPool *
cMemPool::  GetPoolBusy (int  PoolName)
  {
    cMemPool * const &  pPoolPos = sPoolAdmin. GetPool (PoolName);

    cMem3MicroLock  Lock;

    cMemPool *  pPool = pPoolPos;
    if  (pPool != NULL)
      {
        if  (! pPool-> SetBusy ())
          { pPool = NULL; }
      }

    return pPool;
  }

cMemPool::  cMemPool (LINADDR          aPoolMemAddr,
                      ORDHW            aPoolMemSize,
                      cMem3PoolLoUl &  LockUlck,
                      cTryLock      &  DirtyBusy,
                      cMemPool      *  pParent
                     ) EMPTY_THROW
  : PoolMemAddr (aPoolMemAddr),
    PoolMemSize (aPoolMemSize),
    mDirtyBusy  (DirtyBusy),
    mBufNumUsed (0),
    mPlLockUlck (LockUlck),
    mpParent    (pParent)
  {
  }

cMemPool:: ~cMemPool ()
  {
  }

void *
cMemPool::  GetBuf (size_t  Size, size_t &  TargetSize)
  {
    cPoolLock  Lock (* this);

    SetDirty ();
    mBufNumUsed ++;

    void *  pTmp = GetBufLocked (Size, TargetSize);

    if  (pTmp == NULL)  mBufNumUsed --;

    return pTmp;
  }

void
cMemPool::  PutBuf (void *  pAllocBuf)
  {
    cPoolLock  Lock (* this);

    SetDirty ();
    PutBufLocked (pAllocBuf);
    mBufNumUsed --;
  }

int
cMemPool::  GetBufSize (void *  pAllocBuf) const
  {
    cPoolLock  Lock (* this);

    return GetBufSizeLocked (pAllocBuf);
  }

void *
cMemPool::  GetBufBase (void *  pLook) const
  {
    cPoolLock  Lock (* this);

    return GetBufBaseLocked (pLook);
  }

cMemPool *
cMemPool::  GetAliasPool ()
  {
    return NULL;
  }

bool
cMemPool::  LogicalInside (void *) const
  {
    return true;
  }

cMemPoolInst::  cMemPoolInst (LINADDR          aPoolMemAddr,
                              ORDHW            aPoolMemSize,
                              cMem3PoolLoUl &  LockUlck,
                              cMemPool      *  pParent
                             ) EMPTY_THROW
  : cMemPool (aPoolMemAddr, aPoolMemSize, LockUlck, mDirtyBusy, pParent)
  {
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
