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
/*  F i l e               &F: memuni.hpp                                :F&  */
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

#ifndef MEM3_HPP
#define MEM3_HPP

#include "mergemem.hpp"
#include "mem3pool.hpp"

class  cMem3Pool: public cMemPoolInst
  {
  private:
            ORDHW      const mLdBufGranularity;
            ORD08 *          mPoolMem;
            cMergedMem       MemSearch;

            void  *          GetBufLocked                 (size_t    Size, size_t &  TargetSize);
            void             PutBufLocked                 (void  *   pAllocBuf);
            void             TryBuf                       (void  *   pBuf,
                                                           size_t    Size,
                                                           size_t &  TargetSize);
            void             ShortBuf                     (void  *   pBuf,
                                                           size_t    Size,
                                                           size_t &  TargetSize);

  public:
    static  ORDHW            HelperSize                   (ORDHW     TargetSize,
                                                           ORDHW     GranularitySize,
                                                           bool      OvrWr,
                                                           ORDHW     Alignment);
    static  ORDHW            GranSize                     (ORDHW     GranularitySize,
                                                           bool      OvrWr,
                                                           ORDHW     Alignment);

            ORDHW            BytesToUnits                 (ORDHW     ByteNum)   const;
            ORDHW            UnitsToBytes                 (ORDHW     UnitNum)   const;

            int              MaxAvailBuf                  ()                    const;
            int              SumAvailMem                  ()                    const;
            int              GetBufSizeLocked             (void  *   pAllocBuf) const;
            void *           GetBufBaseLocked             (void  *   pLook)     const;
            void *           InfoNextBufUsed              (void  *   pLook1st)  const;
            int              PhysGranularity              ()                    const;
            int              PhysThreshold                ()                    const;

    static  cMemPool *       Create                       (void         *&  pMemTarget,
                                                           void         *&  pMemHelper,
                                                           ORDHW            TargetSize,
                                                           ORDHW            GranularitySize,
                                                           ORDHW            FftThreshold,
                                                           cMem3PoolLoUl &  LockUlck,
                                                           int,
                                                           cMemPool      *  pParent
                                                          );

  protected:
                             cMem3Pool                    (ORD32        *&  pMemTarget,
                                                           ORD32        *&  pMemSet,
                                                           ORDHW            TargetSize,
                                                           ORDHW            LdBufGranularity,
                                                           ORDHW            FftThreshold,
                                                           cMem3PoolLoUl &  LockUlck,
                                                           cMemPool      *  pParent
                                                          ) EMPTY_THROW;
    virtual                 ~cMem3Pool                    ();
  };

/******************************* inlining and templating ****************************************/

inline
int
cMem3Pool::  PhysGranularity () const
  {
    return 1 << mLdBufGranularity;
  }

inline
int
cMem3Pool::  PhysThreshold   () const
  {
    return MemSearch.mFftThreshold2Power << mLdBufGranularity;
  }

inline
ORDHW
cMem3Pool::  BytesToUnits (ORDHW  ByteNum) const
  {
    return (ByteNum + (1 << mLdBufGranularity) - 1) >> mLdBufGranularity;
  }

inline
ORDHW
cMem3Pool::  UnitsToBytes (ORDHW  UnitNum) const
  {
    return UnitNum << mLdBufGranularity;
  }


#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
