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
/*  F i l e               &F: memfix.cpp                                :F&  */
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
#include "memfix.hpp"

cMemFixPool::  cMemFixPool (ORD32        *&  pMemTarget,
                            ORD32        *&  pMemStack,
                            ORDHW            TargetSize,
                            ORDHW            BufGranularity,
                            cMem3PoolLoUl &  LockUlck,
                            int              PoolName,
                            cMemPool      *  pParent
                           ) EMPTY_THROW
  : cMemPoolInst (AddrFromPtr (pMemTarget), TargetSize, LockUlck, pParent),
    RasterSize   (GranSize (BufGranularity, sPoolAdmin. CheckOverwrite (PoolName), sPoolAdmin. GetAlignment (PoolName))),
    mPoolMem     ((ORD08 *) pMemTarget),
    MemList      (TargetSize / GranSize (BufGranularity, sPoolAdmin. CheckOverwrite (PoolName), sPoolAdmin. GetAlignment (PoolName)), pMemStack)
  {
    pMemTarget += TargetSize / sizeof (ORDHW);
  }

cMemFixPool:: ~cMemFixPool  ()
  {
  }

ORDHW
cMemFixPool::  HelperSize    (ORDHW  TargetSize, ORDHW  GranularitySize, bool  OvrWr, ORDHW  Alignment)
  {
    ORDHW  ListSize = sizeof (int) *  (TargetSize / GranSize (GranularitySize, OvrWr, Alignment));
    ORDHW  UsedSize = sizeof (cMemFixPool) + ListSize;

    return (UsedSize + sizeof (ORDHW) - 1) / sizeof (ORDHW) * sizeof (ORDHW);
  }

ORDHW
cMemFixPool::  GranSize  (ORDHW  GranularitySize, bool  OvrWr, ORDHW  Alignment)
  {
    return (  OvrWrBufHeadSize (OvrWr)
            + GranularitySize
            + OvrWrBufTailSize (OvrWr)
            + Alignment
            - 1
           ) / Alignment * Alignment;
  }

int
cMemFixPool::  PhysGranularity () const
  {
    return RasterSize;
  }

int
cMemFixPool::  PhysThreshold   () const
  {
    return RasterSize;
  }

cMemPool *
cMemFixPool::  Create (void         *&  pMemTarget,
                       void         *&  pMemHelper,
                       ORDHW            TargetSize,
                       ORDHW            GranularitySize,
                       ORDHW,        // FftThreshold
                       cMem3PoolLoUl &  LockUlck,
                       int              PoolName,
                       cMemPool      *  pParent
                      )
  {
    ORD32     *  pTmpMemStack  = (ORD32 *) pMemHelper;
    ORD32     *  pTmpMemTarget = (ORD32 *) pMemTarget;

    cMemFixPool *  pRetPool    = new (pTmpMemStack) cMemFixPool (pTmpMemTarget,
                                                                 pTmpMemStack,
                                                                 TargetSize,
                                                                 GranularitySize,
                                                                 LockUlck,
                                                                 PoolName,
                                                                 pParent
                                                                );

    pMemHelper = pTmpMemStack;
    pMemTarget = pTmpMemTarget;

    return pRetPool;
  }

void *
cMemFixPool::  InfoNextBufUsed (void *  pLook1st) const
  {
    int  Offset  = (ORD08 *) pLook1st - mPoolMem;
    if  (Offset <  0)  Offset = 0;

    int  Look1stIdx;
    do
      {   { cPoolLock  Lock (* this);
            SetClean ();
          }

        Look1stIdx = Offset / RasterSize;
        if  (Look1stIdx >  MemList .GetOutIdx ())  Look1stIdx = MemList .GetOutIdx ();

        Look1stIdx += MemList .GetFreeUnits (Look1stIdx);
        if  (Look1stIdx >= MemList .GetOutIdx ())  Look1stIdx = -1;
      }
    while  (GetDirty ());

    return Look1stIdx >= 0 ? & mPoolMem [RasterSize * Look1stIdx] : NULL;
  }

int
cMemFixPool::  MaxAvailBuf   () const
  {
    return MemList .BufAvailable () * RasterSize;
  }

int
cMemFixPool::  SumAvailMem   () const
  {
    return RasterSize * (MemList. GetOutIdx () - BufNumUsed ());
  }

void *
cMemFixPool::  GetBufLocked (size_t  Size, size_t &  TargetSize)
  {
    TargetSize = 0;
    if  ((Size <= 0) || (int (Size) >  RasterSize))  return NULL;

    int const  BufIdx = MemList .Pop ();
    if  (BufIdx <  0)  return NULL;

    TargetSize = RasterSize;
    return & mPoolMem [RasterSize * BufIdx];
  }

void
cMemFixPool::  PutBufLocked (void *  pAllocBuf)
  {
    int  Offset  = (ORD08 *) pAllocBuf - mPoolMem;
    int  BaseIdx = Offset / RasterSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == RasterSize * BaseIdx);

    ORDHW  ReturnedLen = MemList .Push (BaseIdx);
    MEM3_OPTIONAL_INNER_ASSERT (ReturnedLen >  0);
  }

void
cMemFixPool::  TryBuf    (void  *  pBuf, size_t  Size, size_t &  TargetSize)
  {
    int  Offset  = (ORD08 *) pBuf - mPoolMem;
    int  BaseIdx = Offset / RasterSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == RasterSize * BaseIdx);
    MEM3_OPTIONAL_INNER_ASSERT (MemList .GetBufferLen (BaseIdx) >  0);

    TargetSize = (int) Size <= RasterSize ? RasterSize : 0;
  }

void
cMemFixPool::  ShortBuf  (void  *  pBuf, size_t, size_t &  TargetSize)
  {
    int  Offset  = (ORD08 *) pBuf - mPoolMem;
    int  BaseIdx = Offset / RasterSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == RasterSize * BaseIdx);
    MEM3_OPTIONAL_INNER_ASSERT (MemList .GetBufferLen (BaseIdx) >  0);

    TargetSize = RasterSize;
  }

int
cMemFixPool::  GetBufSizeLocked (void *  pAllocBuf) const
  {
    int  Offset  = (ORD08 *) pAllocBuf - mPoolMem;
    int  BaseIdx = Offset / RasterSize;

    MEM3_OPTIONAL_INNER_ASSERT (Offset >= 0);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == RasterSize * BaseIdx);

    return  RasterSize * MemList .GetBufferLen (BaseIdx);
  }

void *
cMemFixPool::  GetBufBaseLocked (void *  pLook) const
  {
    int  Offset  = (ORD08 *) pLook - mPoolMem;
    int  BaseIdx = Offset / RasterSize;
    if  (Offset <  0)                           return NULL;
    if  (MemList .GetBufferLen (BaseIdx) == 0)  return NULL;

    return  & mPoolMem [RasterSize * BaseIdx];
  }


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
