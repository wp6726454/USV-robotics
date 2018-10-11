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
/*  F i l e               &F: mergemem.hpp                              :F&  */
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

#ifndef MERGEMEM_HPP
#define MERGEMEM_HPP

#include "mem3if.hpp"
#include "memtool.hpp"

struct cMemEntry
  {
            ORDHW            mLdMaxBufNum;
            ORDHW            mShareNum;
            ORDHW         *  mpFreeSet;
            ORDHW         *  mpUsedSet;

    static  ORDHW            MaxBufNum                    (INTHW     TabIdx,
                                                           ORDHW     LdBaseUnitNum);
    static  ORDHW            UsedSetLen                   (INTHW     TabIdx,
                                                           ORDHW     LdBaseUnitNum);
    static  ORDHW            FreeSetLen                   (INTHW     TabIdx,
                                                           ORDHW     LdBaseUnitNum);
    static  ORDHW            SetLen                       (INTHW     TabIdx,
                                                           ORDHW     LdBaseUnitNum);

            void             SetupInitValues              (ORDHW *&  pSets,
                                                           INTHW     TabIdx,
                                                           ORDHW     BaseUnitNum);
            void             Invalidate                   ();
            bool             GetFlagFree                  (INTHW     TabIdx,
                                                           INTHW     BufIdx) const;
            void             SetFlagFree                  (INTHW     TabIdx,
                                                           INTHW     BufIdx);
            void             ResFlagFree                  (INTHW     TabIdx,
                                                           INTHW     BufIdx);
            bool             GetFlagUsed                  (INTHW     BufIdx) const;
            void             SetFlagUsed                  (INTHW     BufIdx);
            void             ResFlagUsed                  (INTHW     BufIdx);
  };

class  cBuddyMem
  {
            cMemEntry        mEntryARR [BUDDY_MAX_LEN];
            int              mBuddyTabLen;
            int        const mOutIdx;

            int              SelectClosestPullAndPosition (int       TabIdx,
                                                           int       BufIdx);
            void             ForceBreakDownToClosest      (int       MinIdx,
                                                           int       TabIdx,
                                                           int       BaseIdx);
            void             ForceFetchBuffer             (int       MinIdx,
                                                           int       BaseIdx);
            void             FindBufAndPullPosition       (int       MinIdx,
                                                           int   &   OutPullIdx,
                                                           int   &   OutBufIdx) const;
            void             SetToUsed                    (int       UpIdx,
                                                           int       BufIdx);
            void             ClearSearchPath              (int       UpIdx,
                                                           int       BufIdx);
            void             SetToUnused                  (int       UpIdx,
                                                           int       BufIdx);
            void             SetSearchPath                (int       UpIdx,
                                                           int       BufIdx);
            int              FetchBuffer                  (int       MinIdx);

  public:
    static  ORDHW            SetMemLen                    (ORDHW     BaseUnitNum);
            int              FreeUnits                    (int       BaseIdx,
                                                           int       MaxUnits) const;
            int              UsedUnitsAtomic              (int       BaseIdx)  const;
            int              UsedUnits                    (int       BaseIdx)  const;
            int              GetOutIdx                    ()                   const;

  protected:
            void             PullFreeArea                 (int       BaseIdx,
                                                           int       BaseLen);
            ORDHW            GetBddLen                    (ORDHW     BaseLen) const;
            int              GetBuffer                    (ORDHW     BaseLen);
            ORDHW            PutBuffer                    (int       BaseIdx);
            ORDHW            GetBufferLen                 (int       BaseIdx) const;
            int              GetBufferBase                (int       BaseIdx) const;

                             cBuddyMem                    (ORD32 *&  pSetMem,
                                                           ORDHW     BaseUnitNum);
  };

class  cMergedMem: public cBuddyMem
  {
  private:
            int              mFirstLookIdx;
            int              mUsedUnits;

  public:
            ORDHW      const mFftThreshold2Power;

  private:
            ORDHW *          mpEntrySet;

    static  ORDHW            EntrySetMemLen               (ORDHW     BaseUnitNum);

            int              GetBddBuffer                 (ORDHW     BaseLen, ORDHW &  TargetLen);
            int              GetFftBuffer                 (ORDHW     BaseLen, ORDHW &  TargetLen);

  public:
    static  ORDHW            SetMemLen                    (ORDHW     BaseUnitNum);
            int              GetAnyBuffer                 (ORDHW     BaseLen, ORDHW &  TargetLen);
            ORDHW            PutAnyBuffer                 (int       BaseIdx);
            void             TryAnyBuffer                 (int       BaseIdx, ORDHW  BaseLen, ORDHW &  TargetLen);
            void             CutAnyBuffer                 (int       BaseIdx, ORDHW  BaseLen, ORDHW &  TargetLen);
            ORDHW            GetAnyBufferLen              (int       BaseIdx) const;
            int              GetAnyBufferBase             (int       BaseIdx) const;
            int              MaxFreeBuf                   (int       BaseIdx, int  UnitNum) const;
            int              GetFreeUnits                 () const;

                             cMergedMem                   (ORD32 *&  pSetMem,
                                                           ORDHW     BaseUnitNum,
                                                           ORDHW     FftThreshold);
  };

/******************* inlining and templating ***************************/

inline
bool
cMemEntry::  GetFlagFree (INTHW  TabIdx, INTHW  BufIdx) const
  {
    return GetFlag (mpFreeSet, (TabIdx << mLdMaxBufNum) + BufIdx);
  }

inline
void
cMemEntry::  SetFlagFree (INTHW  TabIdx, INTHW  BufIdx)
  {
    SetFlag (mpFreeSet, (TabIdx << mLdMaxBufNum) + BufIdx);
  }

inline
void
cMemEntry::  ResFlagFree (INTHW  TabIdx, INTHW  BufIdx)
  {
    ResFlag (mpFreeSet, (TabIdx << mLdMaxBufNum) + BufIdx);
  }

inline
bool
cMemEntry::  GetFlagUsed (INTHW  BufIdx) const
  {
    return GetFlag (mpUsedSet, BufIdx);
  }

inline
void
cMemEntry::  SetFlagUsed (INTHW  BufIdx)
  {
    SetFlag (mpUsedSet, BufIdx);
  }

inline
void
cMemEntry::  ResFlagUsed (INTHW  BufIdx)
  {
    ResFlag (mpUsedSet, BufIdx);
  }

inline
int
cBuddyMem::  GetOutIdx () const
  {
    return mOutIdx;
  }

inline
ORDHW
cBuddyMem::  GetBddLen (ORDHW    BaseLen) const
  {
    return 1 << LdUp (BaseLen);
  }

inline
int
cMergedMem::  GetFreeUnits () const
  {
    return GetOutIdx () - mUsedUnits;
  }

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
