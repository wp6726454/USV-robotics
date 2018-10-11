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
/*  F i l e               &F: memcombi.cpp                              :F&  */
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

#include "mem3if.hpp"
#include "memcombi.hpp"

ORDHW
cMemCmbFftPoolRef::  GranSize  (ORDHW  GranularitySize, bool  OvrWr, ORDHW  Alignment)
  {
    return (  OvrWrBufHeadSize (OvrWr)
            + GranularitySize
            + OvrWrBufTailSize (OvrWr)
            + Alignment
            - 1
           ) / Alignment * Alignment;
  }

cMemCmbFftPoolRef::  cMemCmbFftPoolRef (ORD32         *  pMemTarget,
                                        cFft2Bnd      &  MemAdmin,
                                        ORDHW            TargetSize,
                                        ORDHW            GranularitySize,
                                        cMem3PoolLoUl &  LockUlck,
                                        int              PoolName,
                                        cTryLock      &  DirtyBusy,
                                        cMemPool      *  pParent
                                       ) EMPTY_THROW
  : cMemPool   (AddrFromPtr (pMemTarget), TargetSize, LockUlck, DirtyBusy, pParent),
    mGranSize  (GranSize (GranularitySize, sPoolAdmin. CheckOverwrite (PoolName), sPoolAdmin. GetAlignment (PoolName))),
    mPoolMem   (reinterpret_cast <ORD08 *> (pMemTarget)),
    mMemAdmin  (MemAdmin)
  {
  }

void
cMemCmbFftPoolRef::  PutBufLocked (void  *   pAllocBuf)
  {
    int  Offset  = (ORD08 *) pAllocBuf - mPoolMem;
    int  BaseIdx = Offset / mGranSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == mGranSize * BaseIdx);

    mMemAdmin. PutGran (BaseIdx);
  }

int
cMemCmbFftPoolRef::  GetBufSizeLocked (void  *   pAllocBuf) const
  {
    int  Offset  = (ORD08 *) pAllocBuf - mPoolMem;
    int  BaseIdx = Offset / mGranSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == mGranSize * BaseIdx);

    return  mGranSize * mMemAdmin. GetBufferLen (BaseIdx);
  }

void *
cMemCmbFftPoolRef::  GetBufBaseLocked (void  *   pLook) const
  {
    int const  Offset  = (ORD08 *) pLook - mPoolMem;
    int const  Pos     = Offset / mGranSize;
    int const  BaseIdx = mMemAdmin .GetBufferBase (Pos);

    if  (BaseIdx <  0)  return NULL;

    return  BaseIdx >= 0 ? & mPoolMem [mGranSize * BaseIdx] : NULL;
  }

cMem1stFftPoolRef::  cMem1stFftPoolRef (ORD32         *  pMemTarget,
                                        cFft2Bnd      &  MemAdmin,
                                        ORDHW            TargetSize,
                                        ORDHW            GranularitySize,
                                        cMem3PoolLoUl &  LockUlck,
                                        int              PoolName,
                                        cTryLock      &  DirtyBusy,
                                        cMemPool      *  pParent
                                       ) EMPTY_THROW
  : cMemCmbFftPoolRef (pMemTarget,
                       MemAdmin,
                       TargetSize,
                       GranularitySize,
                       LockUlck,
                       PoolName,
                       DirtyBusy,
                       pParent
                      )
  {
  }

void  *
cMem1stFftPoolRef::  GetBufLocked (size_t  Size, size_t &  TargetSize)
  {
    TargetSize = 0;
    if  (Size <= 0)  return NULL;

    ORDHW const  RoundUp = Size + mGranSize - 1;
    if  (RoundUp <  Size)  return NULL; // wrap around

    ORDHW const  GranNum = RoundUp / mGranSize;
    ORDHW const  BufIdx  = mMemAdmin. GetGranNormal (GranNum);
    if  (BufIdx == mMemAdmin. GranNum ())  return NULL;

    TargetSize = mGranSize * GranNum;
    return & mPoolMem [mGranSize * BufIdx];
  }

void
cMem1stFftPoolRef::  TryBuf (void *  pBuf, size_t  Size, size_t &  TargetSize)
  {
    int const  Offset  = (ORD08 *) pBuf - mPoolMem;
    int const  BaseIdx = Offset / mGranSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == mGranSize * BaseIdx);

    int const  BufLen = mMemAdmin. GetBufferLen (BaseIdx) * mGranSize;
    MEM3_OPTIONAL_INNER_ASSERT (BufLen >  0);

    TargetSize = int (Size) <= BufLen ? size_t (BufLen) : 0;
  }

void
cMem1stFftPoolRef::  ShortBuf (void *  pBuf, size_t  Size, size_t &  TargetSize)
  {
    int const  Offset  = (ORD08 *) pBuf - mPoolMem;
    int const  BaseIdx = Offset / mGranSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == mGranSize * BaseIdx);

    int const  BufLen = mMemAdmin. GetBufferLen (BaseIdx) * mGranSize;
    MEM3_OPTIONAL_INNER_ASSERT (BufLen >= int (Size));

    TargetSize = BufLen;
  }

int
cMem1stFftPoolRef::  MaxAvailBuf () const
  {
    int  BaseIdx = 0;
    int  MaxLen  = 0;

    for  (;;)
      {
        int  CurrLen;
          { cPoolLock  Lock (* this);
            if  (BaseIdx == mMemAdmin. GranOutNormal ())  break;
            CurrLen = mMemAdmin. GetFreeLenNormal (BaseIdx);
          }

        if  (CurrLen > MaxLen)  MaxLen = CurrLen;
        BaseIdx += CurrLen;

          { cPoolLock  Lock (* this);
            if  (BaseIdx == mMemAdmin. GranOutNormal ())  break;
            BaseIdx += mMemAdmin. GetUsedLenNormal (BaseIdx);
          }
      }

    return MaxLen * mGranSize;
  }

int
cMem1stFftPoolRef::  SumAvailMem () const
  {
    return mMemAdmin. FreeGranNormal () * mGranSize;
  }

void *
cMem1stFftPoolRef::  InfoNextBufUsed (void *  pLook1st) const
  {
    int  Offset  = (ORD08 *) pLook1st - mPoolMem;
    if  (Offset <  0)  Offset = 0;

    int  Look1stIdx;

      { cPoolLock  Lock (* this);

        Look1stIdx = mMemAdmin. GetBufStrtNormal (Offset / mGranSize);
      }

    return Look1stIdx >= 0 ? & mPoolMem [mGranSize * Look1stIdx] : NULL;
  }

int
cMem1stFftPoolRef::  PhysThreshold () const
  {
    return mMemAdmin. GranBndNormal ();
  }

bool
cMem1stFftPoolRef::  LogicalInside (void *  pBuf) const
  {
    int const  Idx = ((ORD08 *) pBuf - mPoolMem) / mGranSize;

    return (Idx >= 0) && (Idx <  mMemAdmin. GranBndNormal ());
  }

cMemEndFftPoolRef::  cMemEndFftPoolRef (ORD32         *  pMemTarget,
                                        cFft2Bnd      &  MemAdmin,
                                        ORDHW            TargetSize,
                                        ORDHW            GranularitySize,
                                        cMem3PoolLoUl &  LockUlck,
                                        int              PoolName,
                                        cTryLock      &  DirtyBusy,
                                        cMemPool      *  pParent
                                       ) EMPTY_THROW
  : cMemCmbFftPoolRef (pMemTarget,
                       MemAdmin,
                       TargetSize,
                       GranularitySize,
                       LockUlck,
                       PoolName,
                       DirtyBusy,
                       pParent
                      )
  {
  }


void  *
cMemEndFftPoolRef::  GetBufLocked (size_t  Size, size_t &  TargetSize)
  {
    TargetSize = 0;
    if  (Size <= 0)  return NULL;

    ORDHW const  RoundUp = Size + mGranSize - 1;
    if  (RoundUp <  Size)  return NULL; // wrap around

    ORDHW const  GranNum = RoundUp / mGranSize;
    ORDHW const  BufIdx  = mMemAdmin. GetGranReverse (GranNum);
    if  (BufIdx == mMemAdmin. GranNum ())  return NULL;

    TargetSize = mGranSize * GranNum;
    return & mPoolMem [mGranSize * BufIdx];
  }

void
cMemEndFftPoolRef::  TryBuf (void *  pBuf, size_t  Size, size_t &  TargetSize)
  {
    int const  Offset  = (ORD08 *) pBuf - mPoolMem;
    int const  BaseIdx = Offset / mGranSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == mGranSize * BaseIdx);

    int const  BufLen = mMemAdmin. GetBufferLen (BaseIdx) * mGranSize;
    MEM3_OPTIONAL_INNER_ASSERT (BufLen >  0);

    TargetSize = int (Size) <= BufLen ? size_t (BufLen) : 0;
  }

void
cMemEndFftPoolRef::  ShortBuf (void *  pBuf, size_t  Size, size_t &  TargetSize)
  {
    int const  Offset  = (ORD08 *) pBuf - mPoolMem;
    int const  BaseIdx = Offset / mGranSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == mGranSize * BaseIdx);

    int const  BufLen = mMemAdmin. GetBufferLen (BaseIdx) * mGranSize;
    MEM3_OPTIONAL_INNER_ASSERT (BufLen >= int (Size));

    TargetSize = BufLen;
  }

int
cMemEndFftPoolRef::  MaxAvailBuf () const
  {
    int  BaseIdx = mMemAdmin. GranNum () - 1;
    int  MaxLen  = 0;

    for  (;;)
      {
        int  CurrLen;
          { cPoolLock  Lock (* this);
            if  (BaseIdx == mMemAdmin. GranOutReverse ())  break;
            CurrLen = -mMemAdmin. GetFreeLenReverse (BaseIdx);
          }

        if  (CurrLen > MaxLen)  MaxLen = CurrLen;
        BaseIdx -= CurrLen;

          { cPoolLock  Lock (* this);
            if  (BaseIdx == mMemAdmin. GranOutReverse ())  break;
            BaseIdx += mMemAdmin. GetUsedLenReverse (BaseIdx);
          }
      }

    return MaxLen * mGranSize;
  }

int
cMemEndFftPoolRef::  SumAvailMem () const
  {
    return mMemAdmin. FreeGranReverse () * mGranSize;
  }

void *
cMemEndFftPoolRef::  InfoNextBufUsed (void *  pLook1st) const
  {
    int  Offset  = (ORD08 *) pLook1st - mPoolMem;

    int  Look1stIdx;

      { cPoolLock  Lock (* this);

        if  (Offset <  0)  Offset = mMemAdmin. GranBndReverse () + 1;

        Look1stIdx = mMemAdmin. GetBufStrtReverse (Offset / mGranSize);
      }

    return Look1stIdx >= 0 ? & mPoolMem [mGranSize * Look1stIdx] : NULL;
  }

int
cMemEndFftPoolRef::  PhysThreshold () const
  {
    return mMemAdmin. GranBndReverse ();
  }

bool
cMemEndFftPoolRef::  LogicalInside (void *  pBuf) const
  {
    int const  Idx = ((ORD08 *) pBuf - mPoolMem) / mGranSize;

    return (Idx >  mMemAdmin. GranBndReverse ()) && (Idx <  int (mMemAdmin. GranNum ()));
  }

ORDHW
cMem1stFftPool::  HelperSize (ORDHW  TargetSize, ORDHW  GranularitySize, bool  OvrWr, ORDHW  Alignment)
  {
    ORDHW const  GranArrSize = TargetSize / GranSize (GranularitySize, OvrWr, Alignment);

    return   (sizeof (cMem1stFftPool) + sizeof (ORDHW) - 1) / sizeof (ORDHW) * sizeof (ORDHW)
           + (GranArrSize + sizeof (ORDHW) - 1) / sizeof (ORDHW) * sizeof (ORDHW);
  }

cMemPool *
cMem1stFftPool::  Create (void         *&  pMemTarget,
                          void         *&  pMemHelper,
                          ORDHW            TargetSize,
                          ORDHW            GranularitySize,
                          ORDHW,        // FftThreshold
                          cMem3PoolLoUl &  LockUlck,
                          int              PoolName,
                          cMemPool      *  pParent
                         )
  {
    ORD32     *  pTmpMemAdmin  = (ORD32 *) pMemHelper;
    ORD32     *  pTmpMemTarget = (ORD32 *) pMemTarget;

    cMem1stFftPool *  pRetPool = new (pTmpMemAdmin) cMem1stFftPool (pTmpMemTarget,
                                                                    pTmpMemAdmin,
                                                                    TargetSize,
                                                                    GranularitySize,
                                                                    LockUlck,
                                                                    PoolName,
                                                                    pParent
                                                                   );

    pMemHelper = pTmpMemAdmin;
    pMemTarget = pTmpMemTarget;

    return pRetPool;
  }

cFftAdmin::  cFftAdmin (ORD32 *&  pMemAdmin, ORDHW  GranNum)
  : mGranArray   (reinterpret_cast <ORD08 *> (pMemAdmin)),
    mMemAdminDir (mGranArray, GranNum)
  {
    pMemAdmin  += (GranNum + sizeof (ORDHW) - 1) / sizeof (ORDHW);
  }

cMem1stFftPool::  cMem1stFftPool (ORD32        *&  pMemTarget,
                                  ORD32        *&  pMemAdmin,
                                  ORDHW            TargetSize,
                                  ORDHW            GranularitySize,
                                  cMem3PoolLoUl &  LockUlck,
                                  int              PoolName,
                                  cMemPool      *  pParent
                                 ) EMPTY_THROW
  : cFftAdmin         (pMemAdmin, TargetSize / GranSize (GranularitySize, sPoolAdmin. CheckOverwrite (PoolName), sPoolAdmin. GetAlignment (PoolName))),
    cMem1stFftPoolRef (pMemTarget,
                       mMemAdminDir,
                       TargetSize,
                       GranularitySize,
                       LockUlck,
                       PoolName,
                       mDirtyBusy,
                       pParent
                      ),
    mReversePool      (pMemTarget,
                       mMemAdminDir,
                       TargetSize,
                       GranularitySize,
                       LockUlck,
                       PoolName,
                       mDirtyBusy,
                       pParent
                      )
  {
    pMemTarget += TargetSize / sizeof (ORDHW);
  }

cMemPool *
cMem1stFftPool::  GetAliasPool ()
  {
    return & mReversePool;
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/


