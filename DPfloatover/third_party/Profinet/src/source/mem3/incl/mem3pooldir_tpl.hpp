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
/*  F i l e               &F: mem3pooldir_tpl.hpp                       :F&  */
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

#ifndef MEM3POOLDIR_TPL_HPP
#define MEM3POOLDIR_TPL_HPP

/************** inlining and templating ********************/

template <int  Len>
cPoolSort <Len>::  cPoolSort ()
  : mSortNum (0)
  {
    for  (int i = 0; i <  Len; ++ i)
      {
        mSortArr [i]. PoolAddr = LINADDR (-1);
        mSortArr [i]. PoolIdx  = -1;
      }
  }

/* Finds the lowest index with 'PoolAddr >  Key'-condition.
*/
template <int  Len>
int
cPoolSort <Len>::  SortIdxGr (LINADDR  Key) const
  {
    int  ClosedLow = 0;
    int  OpenUpper = mSortNum;

    while  (ClosedLow < OpenUpper)
      {
        int  NewBound  = (ClosedLow + OpenUpper) / 2;

        if      (mSortArr [NewBound]. PoolAddr >  Key)     OpenUpper = NewBound;
        else /* (mSortArr [NewBound]. PoolAddr <= Key) */  ClosedLow = NewBound + 1;
      }

    return OpenUpper;
  }

/* Finds the lowest index with 'PoolAddr >= Key'-condition.
*/
template <int  Len>
int
cPoolSort <Len>::  SortIdxGe (LINADDR  Key) const
  {
    int  ClosedLow = 0;
    int  OpenUpper = mSortNum;

    /* We terminate on an empty subarray with 'ClosedLow' == 'OpenUpper'.
       Having (1') and (2') in mind, we just have having to be saved, we
       know that 'SortedArr [OpenUpper] >= Key'.
       The proposals (1) and (2) are the invariants of our loop. An
        additional invariant is a proposal for its termination:
       'ClosedLow' <= 'NewBound' <= 'OpenUpper'.
    */
    while  (ClosedLow < OpenUpper)
      {
        int  NewBound  = (ClosedLow + OpenUpper) / 2;

        /* Because 'SortedArr' is already sorted, we save our proposals
           with the following conditional assignment.
           Because either 'OpenUpper' will become 'NewBound' or 'ClosedLow'
           will become 'NewBound' + 1 the array-range will short by every
           loop; so it will terminate.
        */
        if      (mSortArr [NewBound]. PoolAddr >= Key)     OpenUpper = NewBound;     // save (1)
        else /* (mSortArr [NewBound]. PoolAddr <  Key) */  ClosedLow = NewBound + 1; // save (2)
      }

    /* Whenever 'ClosedLow' == 'OpenUpper' then 'NewBound' equals, too.
       By a now empty sub-array and the fullfilled proposals (1) and (2)
       we return the predicted result (q.e.d.).
    */
    return OpenUpper;
  }

/* Finds the highest index with 'PoolAddr <= Key'-condition.
*/
template <int  Len>
int
cPoolSort <Len>::  SortIdxLe (LINADDR  Key) const
  {
    int  ClosedHgh = mSortNum - 1;
    int  OpenLower = - 1;

    /* We terminate on an empty subarray with 'ClosedHgh' == 'OpenLower'.
       Having (1') and (2') in mind, we just have having to be saved, we
       know that 'SortedArr [OpenLower] <= Key'.
       The proposals (1) and (2) are the invariants of our loop. An
        additional invariant is a proposal for its termination:
       'ClosedHgh' >= 'NewBound' >= 'OpenLower'.
    */
    while  (ClosedHgh > OpenLower)
      {
        int  NewBound  = (ClosedHgh + OpenLower + 1) / 2;

        /* Because 'SortedArr' is already sorted, we save our proposals
           with the following conditional assignment.
           Because either 'OpenLower' will become 'NewBound' or 'ClosedHgh'
           will become 'NewBound' + 1 the array-range will short by every
           loop; so it will terminate.
        */
        if      (mSortArr [NewBound]. PoolAddr <= Key)     OpenLower = NewBound;     // save (1)
        else /* (mSortArr [NewBound]. PoolAddr >  Key) */  ClosedHgh = NewBound - 1; // save (2)
      }

    /* Whenever 'ClosedHgh' == 'OpenLower' then 'NewBound' equals, too.
       By a now empty sub-array and the fullfilled proposals (1) and (2)
       we return the predicted result (q.e.d.).
    */
    return OpenLower;
  }

template <int  Len>
void
cPoolSort <Len>::  Delete (int  SortIdx)
  {
    mSortNum --;
    for  (; SortIdx <  mSortNum; ++ SortIdx)
      {
        mSortArr [SortIdx] = mSortArr [SortIdx + 1];
      }

    mSortArr [SortIdx]. PoolAddr = LINADDR (-1);
    mSortArr [SortIdx]. PoolIdx  = -1;
  }

template <int  Len>
int
cPoolSort <Len>::  Insert (cPoolXTag const &  Pool)
  {
    MEM3_INNER_ASSERT (mSortNum <  Len);

    int  SortIdx = SortIdxGr (Pool. PoolAddr);

    for  (int i = mSortNum; i >  SortIdx; -- i)
      {
        mSortArr [i] = mSortArr [i - 1];
      }

    mSortArr [SortIdx] = Pool;
    mSortNum ++;

    return SortIdx;
  }

template <int  Len>
cPoolAdmin <Len>::  cPoolAdmin ()
  : mRegActive (0)
  {
    mReaders [0] =
    mReaders [1] = 0;

    #ifdef  MEM3_CHECK_OVERWRITE
    bool const  CheckOverwrite = true;
    #else
    bool const  CheckOverwrite = false;
    #endif

    ORDHW const  Alignment = MEMORY3_INTERN::MakeAlignment (MEM3_ALIGN);

    for  (int i = 0; i <  Len; ++ i)
      {
        mPoolList                 [i] [0] =
        mPoolList                 [i] [1] = NULL;
        mCheckOverwrite           [i]     = CheckOverwrite;
        mAlignment                [i]     = Alignment;
        mPoolInit                 [i]     = MEM3_BUF_PRESET_BYTE;
        mpPoolMemSet_f            [i]     = MEM3_POOL_MEMSET_FUNCTION;
        mpPoolAssignUint_f        [i]     = MEM3_POOL_ASSIGN_UINT_FUNCTION;
        mpPoolGetUint_f           [i]     = MEM3_POOL_GET_UINT_FUNCTION;
        mpPoolAssignUint32_LeMa_f [i]     = MEM3_POOL_ASSIGN_UINT32_LE_MA_FUNCTION;
        mpPoolGetUint32_LeMa_f    [i]     = MEM3_POOL_GET_UINT32_LE_MA_FUNCTION;
        mInternallyCreatedLock    [i]     = false;
        mpaPoolLoUl               [i]     = & mMem3PoolDfLoUl;
      }
  }

/* Within the registry pools are ordered by increasing start address.
   Since there are possibly child pools, equal start addresses are ordered
   by parent before child.
*/
template <int  Len>
cMemPool *
cPoolAdmin <Len>::  Create (void         *&  pMemTarget,
                            void         *&  pMemHelper,
                            ORDHW            TargetSize,
                            ORDHW            HelperSize,
                            ORDHW            GranularitySize,
                            ORDHW            FftThreshold,
                            eMemType         MType,
                            int              PoolName
                           )
  {
    if  (   (ORDHW (MType)    >= MEM3_ARR_LEN (Mem3_Strategy_Hook))
         || (ORDHW (PoolName) >= Len)
        )  return NULL;

    cLockChange  Lock (* this);

    if  (mPoolList [PoolName] [0]                     != NULL)        return NULL;
    if  (Mem3_Strategy_Hook [MType].
           fHelperSize (TargetSize,
                        GranularitySize,
                        mCheckOverwrite [PoolName],
                        mAlignment      [PoolName]
                       )                              >  HelperSize)  return NULL;

    cPoolXTag const  Pool = { AddrFromPtr (pMemTarget), PoolName };

    int
    NewActive             = mRegActive ^ 1;
    mRegistry [NewActive] = mRegistry [mRegActive];

    int  SortIdx = mRegistry [NewActive]. Insert (Pool);
    MEM3_INNER_ASSERT (SortIdx >= 0);

    cMemPool *  pParent;
    if  (SortIdx >  0)  // has left neighbour or parent
      {
        int const   LeftIdx       = mRegistry [NewActive]. PoolIdx (SortIdx - 1);

        cMemPool *  pLeftPool     = mPoolList [LeftIdx] [0];
        LINADDR     LeftOpenUpper;
        for  (;;)
          {
            LeftOpenUpper = pLeftPool-> PoolMemAddr + pLeftPool-> PoolMemSize;
            if  ((pLeftPool-> mpParent == NULL) || (LeftOpenUpper >  Pool. PoolAddr))  break;

            pLeftPool = pLeftPool-> mpParent;
          }

        if  (LeftOpenUpper >  Pool. PoolAddr)
          { /* be inside some buffer of left neighbour or die */
            void *  pIfChild  = pLeftPool-> GetBufBase (pMemTarget);
            if  (pIfChild == NULL)  return NULL;

            bool const          LeftOvrWr = mCheckOverwrite [LeftIdx];
            int  const          FrameSize =   pLeftPool-> GetBufSize (pIfChild)
                                            - MEMORY3_INTERN::AddBufSize [LeftOvrWr];
            char const * const  OpenUpBuf =   static_cast <char *> (pIfChild)
                                            + FrameSize
                                            + MEMORY3_INTERN::BufHeadSize [LeftOvrWr];
            char const * const  OpenUp    = static_cast <char *> (pMemTarget) + TargetSize;
            if  (OpenUp >  OpenUpBuf)  return NULL;

            pParent = pLeftPool;
          }
        else
          {
            pParent = NULL;
          }
      }
    else
      {
        pParent = NULL;
      }

    // also in case of being child it may have a sibling. Therefor check!
    if  (SortIdx <  (mRegistry [NewActive]. SortNum () - 1))  // has right neighbour
      {
        int const  RightIdx = mRegistry [NewActive]. PoolIdx (SortIdx + 1);

        if  ((Pool. PoolAddr + TargetSize) >  mPoolList [RightIdx] [0]-> PoolMemAddr)  return NULL;
      }

    cMemPool *  pTmp         =
    mPoolList [PoolName] [0] = Mem3_Strategy_Hook [MType]. fCreate (pMemTarget,
                                                                    pMemHelper,
                                                                    TargetSize,
                                                                    GranularitySize,
                                                                    FftThreshold,
                                                                    * mpaPoolLoUl [PoolName],
                                                                    PoolName,
                                                                    pParent
                                                                   );
    if  (pTmp != NULL)  mPoolList [PoolName] [1] = pTmp-> GetAliasPool();

      { cMem3MicroLock  MicroLock;

        Lock. mUlckPending = mReaders [mRegActive] == 0;
        mRegActive         = NewActive;
      }

    return pTmp;
  }

template <int  Len>
int
cPoolAdmin <Len>::  Delete (int  PoolName)
  {
    if  (ORDHW (PoolName) >= Len)  return -2;

    LockChange ();

    cMemPool * const pPool  = mPoolList [PoolName] [0];
    cMemPool * const pAlias = mPoolList [PoolName] [1];

    if  (pPool == NULL)
      {
        UlckChange ();
        return -1;
      }

    if  (! pPool-> SetDelete ())
      {
        UlckChange ();
        return -4;
      }

      { int  BufNum  = pPool-> BufNumUsed ();

        if  (pAlias != NULL)  BufNum += pAlias-> BufNumUsed ();

        if  (BufNum >  0)
          {
            pPool-> FreeDelete ();
            UlckChange ();
            return BufNum;
          }
      }

    int
    NewActive             = mRegActive ^ 1;
    mRegistry [NewActive] = mRegistry [mRegActive];

    int  SortIdx = mRegistry [NewActive]. SortIdxGe (pPool-> PoolMemAddr);
    MEM3_INNER_ASSERT (   (SortIdx <  mRegistry [NewActive]. SortNum ())
                       && (mRegistry [NewActive]. PoolAddr (SortIdx) == pPool-> PoolMemAddr)
                       && (mRegistry [NewActive]. PoolIdx  (SortIdx) == PoolName)
                      );

    mRegistry [NewActive]. Delete (SortIdx);

    mPoolInit                 [PoolName]     = MEM3_BUF_PRESET_BYTE;
    mpPoolMemSet_f            [PoolName]     = MEM3_POOL_MEMSET_FUNCTION;
    mpPoolAssignUint_f        [PoolName]     = MEM3_POOL_ASSIGN_UINT_FUNCTION;
    mpPoolGetUint_f           [PoolName]     = MEM3_POOL_GET_UINT_FUNCTION;
    mpPoolAssignUint32_LeMa_f [PoolName]     = MEM3_POOL_ASSIGN_UINT32_LE_MA_FUNCTION;
    mpPoolGetUint32_LeMa_f    [PoolName]     = MEM3_POOL_GET_UINT32_LE_MA_FUNCTION;
    mPoolList                 [PoolName] [0] =
    mPoolList                 [PoolName] [1] = NULL;
    pPool-> ~cMemPool ();

    bool UlckPending;

      { cMem3MicroLock  Lock;

        UlckPending = mReaders [mRegActive] == 0;
        mRegActive  = NewActive;
      }

    if  (UlckPending)  UlckChange ();

    return 0;
  }

template <int  Len>
bool
cPoolAdmin <Len>::  PoolValid (int  PoolName) const
  {
    return ORDHW (PoolName & MEM3_AliasMask) >= Len
             ? false
             : mPoolList [PoolName & MEM3_AliasMask] [PoolName>> MEM3_AliasShift] != NULL;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetPresetVal (int  PoolName, BYTE  PresetVal)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mPoolInit [PoolName] = PresetVal;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetMemSetFunc (int  PoolName, pMemSet_ft  pMemSet_f)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mpPoolMemSet_f [PoolName] = pMemSet_f;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetAssignUintFunc (int  PoolName, pPoolAssignUint_ft  pPoolAssignUint_f)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mpPoolAssignUint_f [PoolName] = pPoolAssignUint_f;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetGetUintFunc (int  PoolName, pPoolGetUint_ft  pPoolGetUint_f)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mpPoolGetUint_f [PoolName] = pPoolGetUint_f;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetAssignUint32_LeMaFunc (int  PoolName, pPoolAssignUint32_LeMa_ft  pPoolAssignUint32_LeMa_f)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mpPoolAssignUint32_LeMa_f [PoolName] = pPoolAssignUint32_LeMa_f;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetGetUint32_LeMaFunc (int  PoolName, pPoolGetUint32_LeMa_ft  pPoolGetUint32_LeMa_f)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mpPoolGetUint32_LeMa_f [PoolName] = pPoolGetUint32_LeMa_f;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetOverwrite (int  PoolName, bool  OvrWr)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mCheckOverwrite [PoolName] = OvrWr;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetAlignment (int  PoolName, ORDHW  Alignment)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    mAlignment [PoolName] = MEMORY3_INTERN::MakeAlignment (Alignment);
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetPoolLock  (int  PoolName)
  {
    SetPoolLock  (PoolName, mMem3PoolDfLoUl);
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetPoolLock  (int  PoolName, cMem3PoolLoUl &  LockUlck)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    if  (mInternallyCreatedLock [PoolName])
      {
        mpaPoolLoUl [PoolName]-> ~cMem3PoolLoUl ();
        mInternallyCreatedLock [PoolName] = false;
      }

    mpaPoolLoUl [PoolName] = & LockUlck;
  }

template <int  Len>
void
cPoolAdmin <Len>::  SetPoolLock  (int  PoolName, fpLockUlck  Lock, fpLockUlck  Ulck, void *  pUser)
  {
    MEM3_USER_ASSERT (ORDHW (PoolName) < Len);
    MEM3_USER_ASSERT (mPoolList [PoolName] [0] == NULL);

    if  (mInternallyCreatedLock [PoolName])
      {
        mpaPoolLoUl [PoolName]-> ~cMem3PoolLoUl ();
        mInternallyCreatedLock [PoolName] = false;
      }

    void * const  pPlace   = & mArray_cMem3PoolLoUlFct [PoolName * sizeof (cMem3PoolLoUlFct)];
    mpaPoolLoUl [PoolName] = new (pPlace) cMem3PoolLoUlFct (Lock, Ulck, pUser);

    mInternallyCreatedLock [PoolName] = true;
  }

template <int  Len>
int
cPoolAdmin <Len>::  GetName (void *  pBuf) const
  {
    LINADDR  BufAddr = AddrFromPtr (pBuf);

    int  RegActive;
      { cMem3MicroLock  MicroLock;

        RegActive = mRegActive;
        ++ mReaders [RegActive];
      }

    int  PoolIdx;
    int  SortIdx = mRegistry [RegActive]. SortIdxLe (BufAddr);
    if  (SortIdx >= 0)
      {
        PoolIdx                   = mRegistry [RegActive]. PoolIdx (SortIdx);
        cMemPool const *  pPool   = mPoolList [PoolIdx] [0];
        LINADDR           HiBound = pPool-> PoolMemAddr + pPool-> PoolMemSize;

        while  ((BufAddr >= HiBound) && (pPool-> mpParent != NULL))
          {
            pPool   = pPool-> mpParent;
            HiBound = pPool-> PoolMemAddr + pPool-> PoolMemSize;
            do
              {
                -- SortIdx; MEM3_OPTIONAL_INNER_ASSERT (SortIdx >= 0);
                PoolIdx = mRegistry [RegActive]. PoolIdx (SortIdx);
              }
            while  (mPoolList [PoolIdx] [0] != pPool);
          }

        if  (BufAddr <  HiBound)
          {
            if  (! pPool-> LogicalInside (pBuf))
              {
                PoolIdx = mPoolList [PoolIdx] [1]-> LogicalInside (pBuf)
                            ? PoolIdx + MEM3_AliasIdx
                            : -1;
              }
          }
        else
          {
            PoolIdx = -1;
          }
      }
    else
      {
        PoolIdx = -1;
      }

    bool UlckPending;
      { cMem3MicroLock  MicroLock;

        UlckPending = ((-- mReaders [RegActive]) == 0) & (RegActive != mRegActive);
      }

    if  (UlckPending)  UlckChange ();

    return PoolIdx;
  }

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
