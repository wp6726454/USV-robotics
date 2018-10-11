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
/*  F i l e               &F: memfix.hpp                                :F&  */
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

#ifndef MEMFIX_HPP
#define MEMFIX_HPP

#include "classmem.hpp"
#include "mem3pool.hpp"

class  cMemFixPool: public cMemPoolInst
  {
  private:
            int        const RasterSize;
            ORD08 *          mPoolMem;
            cClassMem        MemList;

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
                                                           int              PoolName,
                                                           cMemPool      *  pParent
                                                          );

  protected:
                             cMemFixPool                  (ORD32        *&  pMemTarget,
                                                           ORD32        *&  pMemStack,
                                                           ORDHW            TargetSize,
                                                           ORDHW            BufGranularity,
                                                           cMem3PoolLoUl &  LockUlck,
                                                           int              PoolName,
                                                           cMemPool      *  pParent
                                                          ) EMPTY_THROW;
    virtual                 ~cMemFixPool                  ();
  };

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
