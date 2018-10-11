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
/*  F i l e               &F: memcombi.hpp                              :F&  */
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

#ifndef MEMCOMBI_HPP
#define MEMCOMBI_HPP

#include "fft2bound.hpp"
#include "mem3pool.hpp"

class  cMemCmbFftPoolRef: public cMemPool
  {
  protected:
            int       const  mGranSize;
            ORD08         *  mPoolMem;
            cFft2Bnd      &  mMemAdmin;

            void             PutBufLocked                 (void  *   pAllocBuf);

  public:
    static  ORDHW            GranSize                     (ORDHW     GranularitySize,
                                                           bool      OvrWr,
                                                           ORDHW     Alignment);

            int              GapSize                      ()                    const;
            int              GetBufSizeLocked             (void  *   pAllocBuf) const;
            void *           GetBufBaseLocked             (void  *   pLook)     const;
            int              PhysGranularity              ()                    const;

  protected:
                             cMemCmbFftPoolRef            (ORD32         *  pMemTarget,
                                                           cFft2Bnd      &  MemAdmin,
                                                           ORDHW            TargetSize,
                                                           ORDHW            GranularitySize,
                                                           cMem3PoolLoUl &  LockUlck,
                                                           int              PoolName,
                                                           cTryLock      &  DirtyBusy,
                                                           cMemPool      *  pParent
                                                          )  EMPTY_THROW;
  };

class  cMem1stFftPoolRef: public cMemCmbFftPoolRef
  {
  private:
            void  *          GetBufLocked                 (size_t    Size, size_t &  TargetSize);
            void             TryBuf                       (void  *   pBuf,
                                                           size_t    Size,
                                                           size_t &  TargetSize);
            void             ShortBuf                     (void  *   pBuf,
                                                           size_t    Size,
                                                           size_t &  TargetSize);

  public:
                             cMem1stFftPoolRef            (ORD32         *  pMemTarget,
                                                           cFft2Bnd      &  MemAdmin,
                                                           ORDHW            TargetSize,
                                                           ORDHW            GranularitySize,
                                                           cMem3PoolLoUl &  LockUlck,
                                                           int              PoolName,
                                                           cTryLock      &  DirtyBusy,
                                                           cMemPool      *  pParent
                                                          )  EMPTY_THROW;

            int              MaxAvailBuf                  ()                    const;
            int              SumAvailMem                  ()                    const;
            void *           InfoNextBufUsed              (void  *   pLook1st)  const;
            int              PhysThreshold                ()                    const;
            bool             LogicalInside                (void  *   pBuf)      const;
  };

class  cMemEndFftPoolRef: public cMemCmbFftPoolRef
  {
  private:
            void  *          GetBufLocked                 (size_t    Size, size_t &  TargetSize);
            void             TryBuf                       (void  *   pBuf,
                                                           size_t    Size,
                                                           size_t &  TargetSize);
            void             ShortBuf                     (void  *   pBuf,
                                                           size_t    Size,
                                                           size_t &  TargetSize);

  public:
                             cMemEndFftPoolRef            (ORD32         *  pMemTarget,
                                                           cFft2Bnd      &  MemAdmin,
                                                           ORDHW            TargetSize,
                                                           ORDHW            GranularitySize,
                                                           cMem3PoolLoUl &  LockUlck,
                                                           int              PoolName,
                                                           cTryLock      &  DirtyBusy,
                                                           cMemPool      *  pParent
                                                          )  EMPTY_THROW;

            int              MaxAvailBuf                  ()                    const;
            int              SumAvailMem                  ()                    const;
            void *           InfoNextBufUsed              (void  *   pLook1st)  const;
            int              PhysThreshold                ()                    const;
            bool             LogicalInside                (void  *   pBuf)      const;
  };

struct cFftAdmin
  {
            ORD08           *  mGranArray;
            cFft2Bnd           mMemAdminDir;
                               cFftAdmin (ORD32 *&  pMemAdmin, ORDHW  GranNum);
  };

class  cMem1stFftPool: public cFftAdmin, public cMem1stFftPoolRef
  {
            cMemEndFftPoolRef    mReversePool;
            cTryLock mutable     mDirtyBusy;

  public:
    static  ORDHW                HelperSize                 (ORDHW            TargetSize,
                                                             ORDHW            GranularitySize,
                                                             bool             OvrWr,
                                                             ORDHW            Alignment);

    static  cMemPool          *  Create                     (void         *&  pMemTarget,
                                                             void         *&  pMemHelper,
                                                             ORDHW            TargetSize,
                                                             ORDHW            GranularitySize,
                                                             ORDHW            FftThreshold,
                                                             cMem3PoolLoUl &  LockUlck,
                                                             int              PoolName,
                                                             cMemPool      *  pParent
                                                            );

            cMemPool          *  GetAliasPool               ();

  protected:
                                 cMem1stFftPool             (ORD32        *&  pMemTarget,
                                                             ORD32        *&  pMemAdmin,
                                                             ORDHW            TargetSize,
                                                             ORDHW            GranularitySize,
                                                             cMem3PoolLoUl &  LockUlck,
                                                             int              PoolName,
                                                             cMemPool      *  pParent
                                                            ) EMPTY_THROW;
  };

/************* inlining and templating *******************/

inline
int
cMemCmbFftPoolRef::  GapSize () const
  {
    return mMemAdmin. GapNum () * mGranSize;
  }

inline
int
cMemCmbFftPoolRef::  PhysGranularity () const
  {
    return mGranSize;
  }


#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
