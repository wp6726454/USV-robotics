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
/*  F i l e               &F: mem3pooldir.hpp                           :F&  */
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

#ifndef MEM3POOLDIR_HPP
#define MEM3POOLDIR_HPP

class  cMemPool;

#include "mem3usrlock.hpp"

enum  eMemAlias
  {
    MEM3_AliasShift = 30,
    MEM3_AliasIdx   = (1 << MEM3_AliasShift),
    MEM3_AliasMask  = ~MEM3_AliasIdx
  };

class  cMem3PoolLoUlFct: public cMem3PoolLoUl
  {
            fpLockUlck   mLock;
            fpLockUlck   mUlck;
            void *       mpUser;

  public:
    virtual void         Lock             () { mLock (mpUser); }
    virtual void         Ulck             () { mUlck (mpUser); }
                         cMem3PoolLoUlFct (fpLockUlck  Lock, fpLockUlck  Ulck, void *  pUser)
                           : mLock (Lock), mUlck (Ulck), mpUser( pUser)
                           {
                           }
  };

struct cPoolXTag
  {
    LINADDR  PoolAddr;
    int      PoolIdx;
  };

template <int  Len>
class  cPoolSort
  {
            cPoolXTag   mSortArr [Len];
            int         mSortNum;

  public:
                        cPoolSort ();

            int         SortIdxGr (LINADDR) const;
            int         SortIdxGe (LINADDR) const;
            int         SortIdxLe (LINADDR) const;
            int         PoolIdx   (int  si)    const { return mSortArr [si]. PoolIdx; }
            LINADDR     PoolAddr  (int  si)    const { return mSortArr [si]. PoolAddr; }
            void        Delete    (int);
            int         Insert    (cPoolXTag const &);
            int         SortNum   () const { return mSortNum; }
  };

template <int  Len>
class  cPoolAdmin
  {
    struct cLockChange
      {
        cPoolAdmin &  mPoolAdmin;
        bool          mUlckPending;
                      cLockChange (cPoolAdmin &);
                     ~cLockChange ();
      };

  friend struct cLockChange;

  private:
            cPoolSort <Len>             mRegistry                 [2];
            int                         mRegActive;
            int            mutable      mReaders                  [2];
            BYTE                        mPoolInit                 [Len];
            pMemSet_ft                  mpPoolMemSet_f            [Len];
            pPoolAssignUint_ft          mpPoolAssignUint_f        [Len];
            pPoolGetUint_ft             mpPoolGetUint_f           [Len];
            pPoolAssignUint32_LeMa_ft   mpPoolAssignUint32_LeMa_f [Len];
            pPoolGetUint32_LeMa_ft      mpPoolGetUint32_LeMa_f    [Len];
            cMemPool *                  mPoolList                 [Len] [2];
            bool                        mCheckOverwrite           [Len];
            ORDHW                       mAlignment                [Len];
            cMem3AdminLoUl mutable      mAdmLockUlck;
            cMem3PoolDfLoUl             mMem3PoolDfLoUl;
            BYTE                        mArray_cMem3PoolLoUlFct   [Len * sizeof (cMem3PoolLoUlFct)];
            bool                        mInternallyCreatedLock    [Len];
            cMem3PoolLoUl *             mpaPoolLoUl               [Len];

            void                        LockChange          () const { mAdmLockUlck. Lock (); }
            void                        UlckChange          () const { mAdmLockUlck. Ulck (); }

  public:
                                        cPoolAdmin          ();
            cMemPool *                  Create              (void         *&  pMemTarget,
                                                             void         *&  pMemHelper,
                                                             ORDHW            TargetSize,
                                                             ORDHW            HelperSize,
                                                             ORDHW            GranularitySize,
                                                             ORDHW            FftThreshold,
                                                             eMemType         MType,
                                                             int              PoolName
                                                            );
            int                         Delete              (int  PoolName);
            cMemPool * const &          GetPool             (int  PoolName) const { return mPoolList       [PoolName & MEM3_AliasMask] [PoolName>> MEM3_AliasShift]; }
            bool                        PoolValid           (int  PoolName) const;
            BYTE                        GetInitVal          (int  PoolName) const { return mPoolInit       [PoolName & MEM3_AliasMask]; }
            pMemSet_ft                  GetMemSetVal        (int  PoolName) const { return mpPoolMemSet_f            [PoolName & MEM3_AliasMask]; }
            pPoolAssignUint_ft          GetAssignUintVal    (int  PoolName) const { return mpPoolAssignUint_f        [PoolName & MEM3_AliasMask]; }
            pPoolGetUint_ft             GetGetUintVal       (int  PoolName) const { return mpPoolGetUint_f           [PoolName & MEM3_AliasMask]; }
            pPoolAssignUint32_LeMa_ft   GetAssignUint32Val  (int  PoolName) const { return mpPoolAssignUint32_LeMa_f [PoolName & MEM3_AliasMask]; }
            pPoolGetUint32_LeMa_ft      GetGetUint32Val     (int  PoolName) const { return mpPoolGetUint32_LeMa_f    [PoolName & MEM3_AliasMask]; }
            bool                        CheckOverwrite      (int  PoolName) const { return mCheckOverwrite           [PoolName & MEM3_AliasMask]; }
            ORDHW                       GetAlignment        (int  PoolName) const { return mAlignment                [PoolName & MEM3_AliasMask]; }
            void                        SetOverwrite        (int  PoolName, bool   OvrWr);
            void                        SetAlignment        (int  PoolName, ORDHW  Alignment);
            void                        SetPresetVal        (int  PoolName, BYTE   PresetVal);
            void                        SetMemSetFunc             (int  PoolName, pMemSet_ft                 pMemSet_f);
            void                        SetAssignUintFunc         (int  PoolName, pPoolAssignUint_ft         pPoolAssignUint_f);
            void                        SetGetUintFunc            (int  PoolName, pPoolGetUint_ft            pPoolGetUint_f);
            void                        SetAssignUint32_LeMaFunc  (int  PoolName, pPoolAssignUint32_LeMa_ft  pPoolAssignUint32_LeMa_f);
            void                        SetGetUint32_LeMaFunc     (int  PoolName, pPoolGetUint32_LeMa_ft     pPoolGetUint32_LeMa_f);
            void                        SetPoolLock    (int  PoolName);
            void                        SetPoolLock    (int  PoolName, cMem3PoolLoUl &  LockUlck);
            void                        SetPoolLock    (int  PoolName, fpLockUlck  Lock, fpLockUlck  Ulck, void *  pUser);
            int                         GetName        (void *  pBuf) const;
  };

/*********************** inlining ********************************/
template <int  Len>
inline
cPoolAdmin <Len>::cLockChange::  cLockChange (cPoolAdmin &  PoolAdmin)
  : mPoolAdmin   (PoolAdmin),
    mUlckPending (true)
  {
    mPoolAdmin. LockChange ();
  }

template <int  Len>
inline
cPoolAdmin <Len>::cLockChange:: ~cLockChange ()
  {
    if  (mUlckPending)  mPoolAdmin. UlckChange ();
  }

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
