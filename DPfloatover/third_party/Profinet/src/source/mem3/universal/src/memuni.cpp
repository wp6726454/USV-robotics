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
/*  F i l e               &F: memuni.cpp                                :F&  */
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
#include "memuni.hpp"

cMem3Pool::  cMem3Pool  (ORD32        *&  pMemTarget,
                         ORD32        *&  pMemSet,
                         ORDHW            TargetSize,
                         ORDHW            LdBufGranularity,
                         ORDHW            FftThreshold,
                         cMem3PoolLoUl &  LockUlck,
                         cMemPool      *  pParent
                        ) EMPTY_THROW
  : cMemPoolInst      (AddrFromPtr (pMemTarget), TargetSize, LockUlck, pParent),
    mLdBufGranularity (LdBufGranularity),
    mPoolMem          ((ORD08 *) pMemTarget),
    MemSearch         (pMemSet,
                       TargetSize   / (1 << LdBufGranularity),
                       FftThreshold / (1 << LdBufGranularity)
                      )
  {
    pMemTarget += TargetSize / sizeof (ORDHW);
  }

cMem3Pool:: ~cMem3Pool  ()
  {
  }

ORDHW
cMem3Pool::  HelperSize    (ORDHW  TargetSize, ORDHW  GranularitySize, bool  OvrWr, ORDHW  Alignment)
  {
    ORDHW  SetSize  = sizeof (ORDHW) * cMergedMem::SetMemLen (TargetSize / GranSize (GranularitySize, OvrWr, Alignment));
    ORDHW  UsedSize = sizeof (cMem3Pool) + SetSize;

    return (UsedSize + sizeof (ORDHW) - 1) / sizeof (ORDHW) * sizeof (ORDHW);
  }

ORDHW
cMem3Pool::  GranSize  (ORDHW  GranularitySize, bool  OvrWr, ORDHW  Alignment)
  {
    ORDHW const  Tara =   (OvrWrBufHeadSize (OvrWr) + OvrWrBufTailSize (OvrWr) + sizeof (ORDHW) + Alignment - 1)
                        / Alignment
                        * Alignment;
    return 1 << LdUp (Tara > GranularitySize ? Tara : GranularitySize);
  }

cMemPool *
cMem3Pool::  Create (void         *&  pMemTarget,
                     void         *&  pMemHelper,
                     ORDHW            TargetSize,
                     ORDHW            GranularitySize,
                     ORDHW            FftThreshold,
                     cMem3PoolLoUl &  LockUlck,
                     int,
                     cMemPool      *  pParent
                    )
  {
    ORD32     *  pTmpMemSet    = (ORD32     *) pMemHelper;
    ORD32     *  pTmpMemTarget = (ORD32     *) pMemTarget;

    cMem3Pool *  pRetPool      = new (pTmpMemSet) cMem3Pool (pTmpMemTarget,
                                                             pTmpMemSet,
                                                             TargetSize,
                                                             LdUp (GranularitySize),
                                                             FftThreshold,
                                                             LockUlck,
                                                             pParent
                                                            );

    pMemHelper = pTmpMemSet;
    pMemTarget = pTmpMemTarget;

    return pRetPool;
  }

void *
cMem3Pool::  InfoNextBufUsed (void *  pLook1st) const
  {
    int  Offset  = (ORD08 *) pLook1st - mPoolMem;
    if  (Offset <  0)  Offset = 0;

    int  Look1stIdx = BytesToUnits (Offset);

      { cPoolLock  Lock (* this);

        if  (Look1stIdx >  MemSearch. GetOutIdx ())  Look1stIdx = MemSearch. GetOutIdx ();

        Look1stIdx += MemSearch. FreeUnits (Look1stIdx, MAX_INTHW);
        if  (Look1stIdx >= MemSearch. GetOutIdx ())  Look1stIdx = -1;
      }

    return Look1stIdx >= 0 ? & mPoolMem [UnitsToBytes (Look1stIdx)] : NULL;
  }

int
cMem3Pool::  MaxAvailBuf   () const
  {
    int  BaseIdx = 0;
    int  MaxLen  = 0;

    for  (;;)
      {
        int  CurrLen;
          { cPoolLock  Lock (* this);
            if  (BaseIdx >= MemSearch. GetOutIdx ())  break;
            CurrLen = MemSearch. FreeUnits  (BaseIdx, MAX_INTHW);
          }

        int  ContLen;
          { cPoolLock  Lock (* this);
            ContLen = MemSearch. MaxFreeBuf (BaseIdx, CurrLen);
          }

        if  (ContLen > MaxLen)  MaxLen = ContLen;
        BaseIdx += CurrLen;

          { cPoolLock  Lock (* this);
            if  (BaseIdx >= MemSearch. GetOutIdx ())  break;
            BaseIdx += MemSearch. UsedUnits (BaseIdx);
          }
      }

    return UnitsToBytes (MaxLen);
  }

int
cMem3Pool::  SumAvailMem   () const
  {
    return UnitsToBytes (MemSearch. GetFreeUnits ());
  }

void *
cMem3Pool::  GetBufLocked (size_t  Size, size_t &  TargetSize)
  {
    ORDHW const  UnitNum = BytesToUnits (Size);
    if  (UnitsToBytes (UnitNum) <  Size)  return NULL;  // wrap around

    ORDHW  TargetLen;
    int
    BufIdx     = MemSearch. GetAnyBuffer (UnitNum, TargetLen);
    TargetSize = UnitsToBytes (TargetLen);
    if  (BufIdx <  0)  return NULL;

    return & mPoolMem [UnitsToBytes (BufIdx)];
  }

void
cMem3Pool::  PutBufLocked (void *  pAllocBuf)
  {
    ORDHW  Offset  = static_cast <ORD08 *> (pAllocBuf) - mPoolMem;
    ORDHW  BaseIdx = BytesToUnits (Offset);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == UnitsToBytes (BaseIdx));

    int const  Len = MemSearch. PutAnyBuffer (BaseIdx);
    MEM3_OPTIONAL_INNER_ASSERT (Len >  0);
  }

void
cMem3Pool::  TryBuf    (void  *  pBuf, size_t  Size, size_t &  TargetSize)
  {
    cPoolLock  Lock (* this);

    SetDirty ();

    ORDHW  Offset  = static_cast <ORD08 *> (pBuf) - mPoolMem;
    ORDHW  BaseIdx = BytesToUnits (Offset);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == UnitsToBytes (BaseIdx));

    ORDHW  TargetLen;
    MemSearch. TryAnyBuffer (BaseIdx, BytesToUnits (Size), TargetLen);
    TargetSize = UnitsToBytes (TargetLen);
  }

void
cMem3Pool::  ShortBuf  (void  *  pBuf, size_t  Size, size_t &  TargetSize)
  {
    cPoolLock  Lock (* this);

    SetDirty ();

    ORDHW  Offset  = static_cast <ORD08 *> (pBuf) - mPoolMem;
    ORDHW  BaseIdx = BytesToUnits (Offset);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == UnitsToBytes (BaseIdx));

    ORDHW  TargetLen;
    MemSearch. CutAnyBuffer (BaseIdx, BytesToUnits (Size), TargetLen);
    TargetSize = UnitsToBytes (TargetLen);
  }

int
cMem3Pool::  GetBufSizeLocked (void *  pAllocBuf) const
  {
    ORDHW  Offset  = static_cast <ORD08 *> (pAllocBuf) - mPoolMem;
    ORDHW  BaseIdx = BytesToUnits (Offset);
    MEM3_OPTIONAL_INNER_ASSERT (Offset == UnitsToBytes (BaseIdx));

    return  UnitsToBytes (MemSearch. GetAnyBufferLen (BaseIdx));
  }

void *
cMem3Pool::  GetBufBaseLocked (void *  pLook) const
  {
    int  Offset  = (ORD08 *) pLook - mPoolMem;
    int  BaseIdx = MemSearch. GetAnyBufferBase (Offset >> mLdBufGranularity);
    if  (BaseIdx <  0)  return NULL;

    return  & mPoolMem [UnitsToBytes (BaseIdx)];
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
