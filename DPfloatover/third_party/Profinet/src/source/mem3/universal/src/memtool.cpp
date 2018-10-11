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
/*  F i l e               &F: memtool.cpp                               :F&  */
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
#include "memtool.hpp"

ORDHW  const   BitSetARR [32]
  = {
      (1u <<  0), (1u <<  1), (1u <<  2), (1u <<  3),
      (1u <<  4), (1u <<  5), (1u <<  6), (1u <<  7),
      (1u <<  8), (1u <<  9), (1u << 10), (1u << 11),
      (1u << 12), (1u << 13), (1u << 14), (1u << 15),
      (1u << 16), (1u << 17), (1u << 18), (1u << 19),
      (1u << 20), (1u << 21), (1u << 22), (1u << 23),
      (1u << 24), (1u << 25), (1u << 26), (1u << 27),
      (1u << 28), (1u << 29), (1u << 30), (1u << 31)
    };

ORDHW  const   BitResARR [32]
  = {
      ~(1u <<  0), ~(1u <<  1), ~(1u <<  2), ~(1u <<  3),
      ~(1u <<  4), ~(1u <<  5), ~(1u <<  6), ~(1u <<  7),
      ~(1u <<  8), ~(1u <<  9), ~(1u << 10), ~(1u << 11),
      ~(1u << 12), ~(1u << 13), ~(1u << 14), ~(1u << 15),
      ~(1u << 16), ~(1u << 17), ~(1u << 18), ~(1u << 19),
      ~(1u << 20), ~(1u << 21), ~(1u << 22), ~(1u << 23),
      ~(1u << 24), ~(1u << 25), ~(1u << 26), ~(1u << 27),
      ~(1u << 28), ~(1u << 29), ~(1u << 30), ~(1u << 31)
    };

ORDHW
LdDown (ORDHW  Num)
  {
    ORDHW  Ld = 0;
    while  (Num > 1)
      {
        Ld  ++;
        Num >>= 1;
      }
    return Ld;
  }

ORDHW
LdUp (ORDHW  Num)
  {
    Num += Num - 1;

    ORDHW  Ld = 0;
    while  (Num > 1)
      {
        Ld  ++;
        Num >>= 1;
      }
    return Ld;
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

