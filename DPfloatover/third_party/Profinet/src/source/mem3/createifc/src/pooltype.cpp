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
/*  F i l e               &F: pooltype.cpp                              :F&  */
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

/* strategy modules: fill in
*/
#include "memuni.hpp"
#include "memfix.hpp"
#include "memcombi.hpp"

cHook  Mem3_Strategy_Hook [OutRangeMemType] =
  {
      { cMem3Pool::      Create, cMem3Pool::      HelperSize, cMem3Pool::      GranSize},
      { cMemFixPool::    Create, cMemFixPool::    HelperSize, cMemFixPool::    GranSize},
      { cMem1stFftPool:: Create, cMem1stFftPool:: HelperSize, cMem1stFftPool:: GranSize}
  };

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/