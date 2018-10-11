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
/*  F i l e               &F: pooltype.hpp                              :F&  */
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

#ifndef POOLTYPE_HPP
#define POOLTYPE_HPP

class  cMemPool;

typedef  cMemPool *  (* pfCreate)     (void  *&, void  *&, ORDHW, ORDHW, ORDHW, cMem3PoolLoUl &, int, cMemPool *);
typedef  ORDHW       (* pfHelperSize) (ORDHW,    ORDHW,    bool,  ORDHW);
typedef  ORDHW       (* pfGranSize)   (ORDHW,    bool,     ORDHW);

struct cHook
  {
    pfCreate      fCreate;
    pfHelperSize  fHelperSize;
    pfGranSize    fGranSize;
  };

extern cHook  Mem3_Strategy_Hook [OutRangeMemType];

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
