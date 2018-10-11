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
/*  F i l e               &F: mergemem.cpp                              :F&  */
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
#include "mergemem.hpp"
#include "mem3trc.hpp"

#define LdBufTraceLen 8

ORDHW
cMemEntry::  MaxBufNum (INTHW  TabIdx, ORDHW  LdBaseUnitNum)
  {
    return (1 << (LdBaseUnitNum - TabIdx)) / 2 * 2;
  }

ORDHW
cMemEntry::  UsedSetLen (INTHW  TabIdx, ORDHW  LdBaseUnitNum)
  {
    return (MaxBufNum (TabIdx, LdBaseUnitNum) + 31) / 32;
  }

ORDHW
cMemEntry::  FreeSetLen (INTHW  TabIdx, ORDHW  LdBaseUnitNum)
  {
    return ((MaxBufNum (TabIdx, LdBaseUnitNum) * TabIdx) + 31) / 32;
  }

ORDHW
cMemEntry::  SetLen (INTHW  TabIdx, ORDHW  LdBaseUnitNum)
  {
    return UsedSetLen (TabIdx, LdBaseUnitNum) + FreeSetLen (TabIdx, LdBaseUnitNum);
  }

void
cMemEntry::  SetupInitValues (ORDHW *&  pSets, INTHW  TabIdx, ORDHW  LdBaseUnitNum)
  {
    mLdMaxBufNum = LdBaseUnitNum - TabIdx;
    mShareNum    = 0;

    ORDHW  SetLen;

    mpFreeSet    = pSets;
    SetLen       = FreeSetLen (TabIdx, LdBaseUnitNum);
    MEM3_ADMIN_MEMSET (mpFreeSet, 0x00u, SetLen * sizeof (ORDHW));
    pSets       += SetLen;

    mpUsedSet    = pSets;
    SetLen       = UsedSetLen (TabIdx, LdBaseUnitNum);
    MEM3_ADMIN_MEMSET (mpUsedSet, 0x00u, SetLen * sizeof (ORDHW));
    pSets       += SetLen;
  }

void
cMemEntry::  Invalidate ()
  {
    mLdMaxBufNum = (ORDHW) -1;
    mShareNum    = 0;
    mpFreeSet    = NULL;
    mpUsedSet    = NULL;
  }

ORDHW
cBuddyMem::  SetMemLen (ORDHW  BaseUnitNum)
  {
    ORDHW  LdBaseUnitNum = LdDown (BaseUnitNum) + 1;
    ORDHW  MemSum        = 0;

    for  (int i = 0; i < (int) LdBaseUnitNum; i ++)
      {
        MemSum += cMemEntry:: SetLen (i, LdBaseUnitNum);
      }
    return MemSum;
  }

cBuddyMem::  cBuddyMem  (ORD32 *&  pSetMem, ORDHW  BaseUnitNum)
  : mOutIdx (BaseUnitNum)
  {
    ORDHW  LdBaseUnitNum = LdDown (BaseUnitNum) + 1;

    int  i;
    for  (i = 0;; i ++)
      {
        mEntryARR [i]. SetupInitValues (pSetMem, i, LdBaseUnitNum);
        if  (mEntryARR [i].mLdMaxBufNum <= 1)  break;
      }
    MEM3_INNER_ASSERT (i < BUDDY_MAX_LEN);

    mEntryARR [i].mShareNum += 2;

    mBuddyTabLen = i + 1;
    for  (i = mBuddyTabLen; i < BUDDY_MAX_LEN; i ++)
      {
        mEntryARR [i]. Invalidate ();
      }

    /* Initially the high class is free and all search-paths are locked.
       That is the full address-space designating our virtual memory
       free at this time.
       We now pull that vitual memory not being real.
    */
    PullFreeArea (BaseUnitNum, (1 << LdBaseUnitNum) - BaseUnitNum);
  }

int
cBuddyMem::  SelectClosestPullAndPosition (int  TabIdx, int  BufIdx)
  {
    int  TestIdx = TabIdx + 1;

    for  (;;)
      {
        if  (TestIdx >= mBuddyTabLen)                           break;

        BufIdx >>= 1;
        if  (mEntryARR [TestIdx].GetFlagFree (TabIdx, BufIdx))  break;

        TabIdx  = TestIdx;
        TestIdx ++;
      }

    return TabIdx;
  }

void
cBuddyMem::  ForceBreakDownToClosest (int  MinIdx, int  TabIdx, int  BaseIdx)
  {
    while  (TabIdx > MinIdx)
      {
        int           BufIdx;

        TabIdx    --;
        BufIdx    = BaseIdx >> (TabIdx - MinIdx);

        MEM3_OPTIONAL_INNER_ASSERT (! mEntryARR [TabIdx]. GetFlagUsed (BufIdx));
        mEntryARR [TabIdx]. SetFlagUsed (BufIdx);

        int  BddIdx = BufIdx ^ 1;
        SetSearchPath (TabIdx, BddIdx);
        SetToUnused   (TabIdx, BddIdx);
      }
  }

void
cBuddyMem::  ForceFetchBuffer (int  TabIdx, int  BufIdx)
  {
    int           PullIdx = SelectClosestPullAndPosition (TabIdx, BufIdx);
    int           UseIdx  = BufIdx >> (PullIdx - TabIdx);

    SetToUsed                       (PullIdx, UseIdx);
    ClearSearchPath                 (PullIdx, UseIdx);
    ForceBreakDownToClosest (TabIdx, PullIdx, BufIdx);
  }

void
cBuddyMem::  PullFreeArea (int  BaseIdx, int  BaseLen)
  {
    if  (BaseLen <= 0)  return;

    int   MinIdx     = 0;
    int   BaseBufNum = 1;

    for  (;;)
      {
        int   TwoBufNum  = BaseBufNum << 1;

        while  (((BaseIdx & 1) == 0) && (TwoBufNum <= BaseLen))
          {
            BaseIdx   >>= 1;
            BaseBufNum  = TwoBufNum;
            TwoBufNum <<= 1;
            MinIdx     += 1;
          }

        ForceFetchBuffer (MinIdx, BaseIdx);
        BaseIdx += 1;
        BaseLen -= BaseBufNum;

        if  (BaseLen <= 0)  break;

        while  (BaseBufNum > BaseLen)
          {
            BaseIdx    <<= 1;
            BaseBufNum >>= 1;
            MinIdx      -= 1;
          }
      }
  }

void
cBuddyMem::  FindBufAndPullPosition (int  MinIdx, int &  OutPullIdx, int &  OutBufIdx) const
  {
    int           TabIdx;
    int           PullIdx;
    int           BufIdx;

    /* Find the closest entry for best fit in the sets of the top entry.
    */
    PullIdx = MinIdx;
    while  (mEntryARR [PullIdx].mShareNum <= 0)
      {
        PullIdx ++;

        if  (PullIdx >= mBuddyTabLen)
          {
            OutPullIdx = mBuddyTabLen;  // outrange
            OutBufIdx  = -1;            // outrange
            return;
          }
      }

    /* Break down to a possibly found entry by binary decision.
    */
    TabIdx = mBuddyTabLen - 1;
    BufIdx = 0;
    while  (TabIdx >  PullIdx)
      {
        BufIdx  += ORDHW (mEntryARR [TabIdx]. GetFlagFree (PullIdx, BufIdx)) ^ 1;
        BufIdx <<= 1;
        TabIdx   --;
      }

    BufIdx += ORDHW (mEntryARR [TabIdx]. GetFlagUsed (BufIdx));

    OutPullIdx = PullIdx;
    OutBufIdx  = BufIdx;
  }

void
cBuddyMem::  SetToUsed (int  UpIdx, int  BufIdx)
  {
    cMemEntry  *  pMemEntry = & mEntryARR [UpIdx];

    MEM3_OPTIONAL_INNER_ASSERT (! pMemEntry-> GetFlagUsed (BufIdx));
    pMemEntry-> SetFlagUsed (BufIdx);
    pMemEntry-> mShareNum --;
  }

void
cBuddyMem::  ClearSearchPath (int  UpIdx, int  BufIdx)
  {
    int           TabIdx    = UpIdx;
    cMemEntry  *  pMemEntry;

    for  (;;)
      {
        TabIdx ++;
        if  (TabIdx >= mBuddyTabLen)                         break;

        BufIdx   >>= 1;
        pMemEntry  = & mEntryARR [TabIdx];
        MEM3_OPTIONAL_INNER_ASSERT (pMemEntry-> GetFlagFree (UpIdx, BufIdx));
        pMemEntry-> ResFlagFree (UpIdx, BufIdx);

        if  (pMemEntry-> GetFlagFree (UpIdx, BufIdx ^ 1))    break;
      }
  }

void
cBuddyMem::  SetToUnused (int  UpIdx, int  BufIdx)
  {
    cMemEntry  *  pMemEntry = & mEntryARR [UpIdx];

    MEM3_OPTIONAL_INNER_ASSERT (! pMemEntry-> GetFlagUsed (BufIdx));
    pMemEntry-> mShareNum ++;
  }

void
cBuddyMem::  SetSearchPath (int  UpIdx, int  BufIdx)
  {
    int           TabIdx    = UpIdx;
    cMemEntry  *  pMemEntry;

    for  (;;)
      {
        TabIdx ++;
        if  (TabIdx >= mBuddyTabLen)                         break;

        BufIdx >>= 1;

        pMemEntry = & mEntryARR [TabIdx];
        MEM3_OPTIONAL_INNER_ASSERT (! pMemEntry-> GetFlagFree (UpIdx, BufIdx));
        pMemEntry-> SetFlagFree (UpIdx, BufIdx);

        if  (pMemEntry-> GetFlagFree (UpIdx, BufIdx ^ 1))    break;
      }
  }

int
cBuddyMem::  FreeUnits (int  BaseIdx, int  MaxUnits) const
  {
      { /* Do it that way, because 'BaseIdx + MaxUnits' may swap
           around and become negative.
        */
        int   MaxMaxUnits = mOutIdx - BaseIdx;

        if  (MaxUnits >  MaxMaxUnits)  MaxUnits = MaxMaxUnits;
      }

    int   BufIdx = BaseIdx;
    int   Units  = 0;
    int   OutIdx = BaseIdx + MaxUnits;

    for  (;;)
      {
        if  (BaseIdx >= OutIdx)                         break;

        int          TabIdx  = 0;
        int          TestIdx = 1;

        for  (;;)
          {
            if  (TestIdx >= mBuddyTabLen)                                  break;

            int  BufTimes2Idx = BufIdx >> 1;
            if  (mEntryARR [TestIdx]. GetFlagFree (TabIdx, BufTimes2Idx))  break;

            TabIdx  = TestIdx;
            TestIdx ++;
            BufIdx  = BufTimes2Idx;
          }

        /* TestIdx points to the closest class or beyond, where 1! of 2 members of
           class TabIdx=TestIdx-1 is free. The Member BufIdx of class TabIdx contains
           BaseIdx, the member BufIdx^1 does not.
        */
        if  (mEntryARR [TabIdx]. GetFlagUsed (BufIdx))  break; // the neighbour was free

        /* We increment the lookup-boundary by 1 element of class TabIdx.
           At the next we make it base-boundary by times 2 power TabIdx. Then
           we increment Units by the difference to the last BaseIdx and let
           become BaseIdx our new lookup-boundary.
           That is repeated until there is no more free memory or we've found
           enough by reaching MaxUnits.
        */
        BufIdx  ++;
        BufIdx <<= TabIdx;
        Units   += BufIdx - BaseIdx;
        BaseIdx  = BufIdx;
      }

    return Units;
  }

int
cBuddyMem::  UsedUnitsAtomic (int  BaseIdx) const
  {
    if  (BaseIdx >= mOutIdx)  return 0;

    int   BufIdx = BaseIdx;
    int   TabIdx = 0;

    if  (! mEntryARR [TabIdx]. GetFlagUsed (BufIdx))
      {
        for  (;;)
          {
            TabIdx ++;
            if  (TabIdx >= mBuddyTabLen)                    break;

            BufIdx >>= 1;

            if  (mEntryARR [TabIdx]. GetFlagUsed (BufIdx))  break;
          }

        if  (TabIdx >= mBuddyTabLen)                                return 0;
        if  (mEntryARR [TabIdx]. GetFlagFree (TabIdx - 1, BufIdx))  return 0;
      }

    return ((BufIdx + 1) << TabIdx) - BaseIdx;
  }

int
cBuddyMem::  UsedUnits (int  BaseIdx) const
  {
    int   SumUnits  = 0;

    for  (;;)
      {
        int  CurUnits  = UsedUnitsAtomic (BaseIdx + SumUnits);
        if  (CurUnits == 0)  break;

        SumUnits += CurUnits;
      }

    return SumUnits;
  }

int
cBuddyMem::  FetchBuffer (int    MinIdx)
  {
    int           PullIdx;
    int           BufIdx;

    FindBufAndPullPosition  (MinIdx, PullIdx, BufIdx);
    if  (BufIdx <  0)  return -1;

    SetToUsed                       (PullIdx, BufIdx);
    ClearSearchPath                 (PullIdx, BufIdx);

    BufIdx <<= PullIdx - MinIdx;
    ForceBreakDownToClosest (MinIdx, PullIdx, BufIdx);

    return BufIdx << MinIdx;
  }

int
cBuddyMem::  GetBuffer (ORDHW    BaseLen)
  {
    int  MinIdx = LdUp (BaseLen);

    return MinIdx < mBuddyTabLen ? FetchBuffer (MinIdx) : -1;
  }

ORDHW
cBuddyMem::  PutBuffer (int  BaseIdx)
  {
    int          BufIdx  = BaseIdx;
    int          TabIdx  = 0;
    cMemEntry  * pMemEntry;

    if  (BaseIdx <  0)        return 0;
    if  (BaseIdx >= mOutIdx)  return 0;

    for (;;)
      {
        pMemEntry = & mEntryARR [TabIdx];
        if  (pMemEntry-> GetFlagUsed (BufIdx))  break;

        TabIdx ++;
        if  (TabIdx >= mBuddyTabLen)  return 0;

        BufIdx = BufIdx >> 1;
      }

    ORDHW  BaseLen = 1 << TabIdx;

    MEM3_OPTIONAL_INNER_ASSERT (pMemEntry-> GetFlagUsed (BufIdx));
    pMemEntry-> ResFlagUsed (BufIdx);
    int BddIdx = BufIdx ^ 0x0001;

    while  (! pMemEntry-> GetFlagUsed (BddIdx))
      {
        pMemEntry-> mShareNum --;
        ClearSearchPath (TabIdx, BddIdx);

        TabIdx ++;
        pMemEntry = & mEntryARR [TabIdx];

        BufIdx = BufIdx >> 1;
        MEM3_OPTIONAL_INNER_ASSERT (pMemEntry-> GetFlagUsed (BufIdx));
        pMemEntry-> ResFlagUsed (BufIdx);
        BddIdx = BufIdx ^ 0x0001;
      }

    SetSearchPath (TabIdx, BufIdx);
    pMemEntry-> mShareNum ++;

    return BaseLen;
  }

ORDHW
cBuddyMem::  GetBufferLen (int  BaseIdx) const
  {
    int          BufIdx  = BaseIdx;
    int          TabIdx  = 0;

    if  (BaseIdx <  0)        return 0;
    if  (BaseIdx >= mOutIdx)  return 0;

    for (;;)
      {
        if  (mEntryARR [TabIdx]. GetFlagUsed (BufIdx))  break;

        TabIdx ++;
        if  (TabIdx >= mBuddyTabLen)  return 0;

        BufIdx = BufIdx >> 1;
      }

    return 1 << TabIdx;
  }

int
cBuddyMem::  GetBufferBase (int  BaseIdx) const
  {
    int          BufIdx  = BaseIdx;
    int          TabIdx  = 0;

    if  (BaseIdx <  0)        return -1;
    if  (BaseIdx >= mOutIdx)  return -1;

    for (;;)
      {
        if  (mEntryARR [TabIdx]. GetFlagUsed (BufIdx))  break;

        TabIdx ++;
        if  (TabIdx >= mBuddyTabLen)  return -1;

        BufIdx = BufIdx >> 1;
      }

    return BufIdx << TabIdx;
  }

ORDHW
cMergedMem::  EntrySetMemLen (ORDHW  BaseUnitNum)
  {
    return (BaseUnitNum + 1 + 31) >> 5;
  }

ORDHW
cMergedMem::  SetMemLen (ORDHW  BaseUnitNum)
  {
    return cBuddyMem:: SetMemLen (BaseUnitNum) + EntrySetMemLen (BaseUnitNum);
  }

cMergedMem::  cMergedMem (ORD32 *&  pSetMem, ORDHW  BaseUnitNum, ORDHW  FftThreshold)
  : cBuddyMem           (pSetMem, BaseUnitNum),
    mFirstLookIdx       (0),
    mUsedUnits          (0),
    mFftThreshold2Power (1u << LdDown (FftThreshold)),
    mpEntrySet          (pSetMem)
  {
    ORDHW const  FftSetLen = EntrySetMemLen (BaseUnitNum);
    MEM3_ADMIN_MEMSET (mpEntrySet, 0x00u, FftSetLen * 4);
    SetFlag           (mpEntrySet, BaseUnitNum);
    pSetMem += FftSetLen;
  }

int
cMergedMem::  GetBddBuffer (ORDHW  BaseLen, ORDHW &  TargetLen)
  {
    int    BufIdx = GetBuffer (BaseLen);
    ORDHW  TgtLen = 0;

    if  (BufIdx >= 0)
      {
        TgtLen = GetBddLen (BaseLen);
        SetFlag (mpEntrySet, BufIdx);
        mFirstLookIdx = BufIdx + TgtLen;
        TraceFill (cBuf (BufIdx, BaseLen, 'B'));
      }

    TargetLen = TgtLen;
    return BufIdx;
  }

int
cMergedMem::  GetFftBuffer (ORDHW  BaseLen, ORDHW &  TargetLen)
  {
    int      BufIdx  = -1;
    ORDHW    FftMask = mFftThreshold2Power - 1;
    ORDHW    TestLen = (        BaseLen       + FftMask) & ~FftMask;
    int      TestIdx = ((ORDHW) mFirstLookIdx + FftMask) & ~FftMask;
    ORDHW    Delta   = 0;
    ORDHW    TgtLen  = 0;

    for  (;;)
      {
        if  (TestIdx >= GetOutIdx ())  break;

        Delta = FreeUnits (TestIdx, TestLen);
        if  (Delta   >= BaseLen)       break;

        TestIdx += Delta;
        if  (TestIdx >= GetOutIdx ())  break;

        TestIdx +=  FftMask + UsedUnits (TestIdx);
        TestIdx &= ~FftMask;
      }

    if  (Delta <  BaseLen)
      {
        TestIdx = 0;

        for  (;;)
          {
            Delta = FreeUnits (TestIdx, TestLen);
            if  (Delta   >= BaseLen)        break;

            TestIdx += Delta;
            if  (TestIdx >= mFirstLookIdx)  break;

            TestIdx +=  FftMask + UsedUnits (TestIdx);
            TestIdx &= ~FftMask;
            if  (TestIdx >= mFirstLookIdx)  break;
          }
      }

    if  (Delta >= BaseLen)
      {
        if  (Delta >= TestLen)
          { Delta = TestLen; }
        else
          {
            ORDHW  Mask = ~1;
            ORDHW  Next = Delta & Mask;

            while  (Next >= BaseLen)
              {
                Delta   = Next;
                Mask  <<= 1;
                Next    = Delta & Mask;
              }
          }

        TgtLen = Delta;
        PullFreeArea (TestIdx,    Delta);
        SetFlag      (mpEntrySet, TestIdx);

        mFirstLookIdx = TestIdx + Delta;
        BufIdx        = TestIdx;
        TraceFill (cBuf (BufIdx, Delta, 'F'));
      }

    TargetLen = TgtLen;
    return BufIdx;
  }

int
cMergedMem::  GetAnyBuffer (ORDHW  BaseLen, ORDHW &  TargetLen)
  {
    int const
    RetIdx      = BaseLen >= mFftThreshold2Power
                    ? GetFftBuffer (BaseLen, TargetLen)
                    : GetBddBuffer (BaseLen, TargetLen);
    mUsedUnits += TargetLen;
    return RetIdx;
  }

ORDHW
cMergedMem::  PutAnyBuffer (int  BaseIdx)
  {
    TraceDel (BaseIdx);

    if  (BaseIdx <  0)                     return 0;
    if  (BaseIdx >= GetOutIdx ())          return 0;
    if  (! GetFlag (mpEntrySet, BaseIdx))  return 0;

    ORDHW  BufBaseLen = 0;

    mFirstLookIdx = BaseIdx;
    ResFlag (mpEntrySet, mFirstLookIdx);

    do
      {
        ORDHW  BddBaseLen = PutBuffer (BaseIdx);

        BufBaseLen += BddBaseLen;
        BaseIdx    += BddBaseLen;
      }
    while  (   (! GetFlag (mpEntrySet, BaseIdx))
            && (FreeUnits (BaseIdx, 1) == 0)
           );

    mUsedUnits -= BufBaseLen;
    return BufBaseLen;
  }

void
cMergedMem::  TryAnyBuffer (int  BaseIdx, ORDHW  BaseLen, ORDHW &  TargetLen)
  {
    TargetLen = 0;
    if  (BaseIdx <  0)                     return;
    if  (BaseIdx >= GetOutIdx ())          return;
    if  (! GetFlag (mpEntrySet, BaseIdx))  return;

    ORDHW const  FftMask    = mFftThreshold2Power - 1;
    ORDHW const  TestLen    = BaseLen >  mFftThreshold2Power
                            ? (BaseLen + FftMask) & ~FftMask
                            : 1 << LdUp (BaseLen);
    ORDHW const  TestMask   = ~(TestLen - 1) | ~FftMask;
    ORDHW const  BufBaseLen = GetAnyBufferLen (BaseIdx);

    TargetLen = BufBaseLen;
    if  ((BaseIdx & TestMask) != 0)  return;  // buddy missaligned for expansion

    int   const  TestIdx = BaseIdx + BufBaseLen;
    ORDHW        Delta   = FreeUnits (TestIdx, TestLen - BufBaseLen);
    ORDHW const  MinLen  = BaseLen - BufBaseLen;

    if  (Delta >= MinLen)
      {
        ORDHW const  WishLen = TestLen - BufBaseLen;

        if  (Delta >= WishLen)
          { Delta = WishLen; }
        else
          {
            ORDHW  Mask = ~1;
            ORDHW  Next = Delta & Mask;

            while  (Next >= MinLen)
              {
                Delta   = Next;
                Mask  <<= 1;
                Next    = Delta & Mask;
              }
          }

        PullFreeArea (TestIdx, Delta);
        mFirstLookIdx = TestIdx    + Delta;
        TargetLen     = BufBaseLen + Delta;
      }

    mUsedUnits += TargetLen - BufBaseLen;
  }

void
cMergedMem::  CutAnyBuffer (int  BaseIdx, ORDHW  BaseLen, ORDHW &  TargetLen)
  {
    TargetLen = 0;
    if  (BaseIdx <  0)                     return;
    if  (BaseIdx >= GetOutIdx ())          return;
    if  (! GetFlag (mpEntrySet, BaseIdx))  return;

    ORDHW  BddBaseLen;
    ORDHW  BufBaseLen = 0;

    do
      {
        BddBaseLen  = GetBufferLen (BaseIdx);
        BufBaseLen += BddBaseLen;
        BaseIdx    += BddBaseLen;
      }
    while  (   (! GetFlag   (mpEntrySet, BaseIdx)           )
            && (  FreeUnits (BaseIdx, 1)          == 0      )
            && (  BufBaseLen                      <  BaseLen) );

    int    LastIdx = BaseIdx    -  BddBaseLen;
    ORDHW  PartLen = BddBaseLen - (BufBaseLen - BaseLen);

    if  (BufBaseLen >  BddBaseLen) // composed
      {
        ORDHW  Mask = mFftThreshold2Power - 1;

        PartLen +=  Mask;
        PartLen &= ~Mask;
      }
    else
      { PartLen  = 1 << LdUp (PartLen); }

    if  (PartLen <  BddBaseLen)
      {
        PutBuffer    (LastIdx);
        PullFreeArea (LastIdx, PartLen);
      }
    else
      { PartLen = BddBaseLen; }

    TargetLen     = BufBaseLen - BddBaseLen + PartLen;
    mFirstLookIdx = LastIdx + PartLen;

    while  (   (! GetFlag   (mpEntrySet, BaseIdx)     )
            && (  FreeUnits (BaseIdx, 1)          == 0) )
      {
        BaseIdx += PutBuffer (BaseIdx);
      }

    mUsedUnits -= BufBaseLen - TargetLen;
  }

ORDHW
cMergedMem::  GetAnyBufferLen (int  BaseIdx) const
  {
    if  (BaseIdx <  0)                     return 0;
    if  (BaseIdx >= GetOutIdx ())          return 0;
    if  (! GetFlag (mpEntrySet, BaseIdx))  return 0;

    int    CurrIdx    = BaseIdx;
    ORDHW  BufBaseLen = 0;

    do
      {
        ORDHW  BddBaseLen = GetBufferLen (BaseIdx);

        BufBaseLen += BddBaseLen;
        BaseIdx    += BddBaseLen;
        CurrIdx     = BaseIdx;
      }
    while  (   (! GetFlag   (mpEntrySet, CurrIdx)     )
            && (  FreeUnits (CurrIdx, 1)          == 0) );

    return BufBaseLen;
  }

int
cMergedMem::  GetAnyBufferBase (int  BaseIdx) const
  {
    for  (;;)
      {
        BaseIdx = GetBufferBase (BaseIdx);
        if  (BaseIdx <  0)                   break;
        if  (GetFlag (mpEntrySet, BaseIdx))  break;

        BaseIdx --;
      }

    return BaseIdx;
  }

int
cMergedMem::  MaxFreeBuf (int  BaseIdx, int  UnitNum) const
  {
    int  CurMax = 0;

    while  (UnitNum >  CurMax)
      {
        int  CurAdd = 1;

        for  (;;)
          {
            int  NewAdd;
            int  Mask;

            if  (CurAdd           >= (int) mFftThreshold2Power)  break;

            NewAdd = CurAdd << 1;
            Mask   = (-NewAdd);

            if  (NewAdd           >        UnitNum)              break;
            if  ((BaseIdx & Mask) !=       BaseIdx)              break;

            CurAdd = NewAdd;
          }

        UnitNum -= CurAdd;

        int  CurSum = CurAdd;
        if  (CurAdd >= (int) mFftThreshold2Power)
          {
            while  (CurAdd <= UnitNum)
              {
                BaseIdx += CurAdd;
                CurSum  += CurAdd;
                UnitNum -= CurAdd;
              }
          }

        if  (CurSum >  CurMax)  CurMax = CurSum;
      }

    return CurMax;
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
