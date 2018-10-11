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
/*  F i l e               &F: classmem.cpp                              :F&  */
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
#include "classmem.hpp"

ORDHW
cClassMem::  StackLen (int  BufNum)
  {
    return (BufNum * sizeof (int) + sizeof (ORDHW) - 1) / sizeof (ORDHW);
  }

cClassMem::  cClassMem (int  aMaxBufNum, ORD32 * &  aIdxStackArr)
  : MaxBufNum (aMaxBufNum), IdxStackArr ((int *&) aIdxStackArr), TopOfStack (aMaxBufNum - 1)
  {
    for  (int i = TopOfStack; i >= 0; i --)  IdxStackArr [i] = i - 1;
    aIdxStackArr += ORD32 (aMaxBufNum);
  }

int
cClassMem::  Pop ()
  {
    int  FreeIdx = TopOfStack;

    if  (FreeIdx >= 0) // < 0 means empty
      {
        int &  Cur = IdxStackArr [FreeIdx];

        TopOfStack = Cur;     // from stack down
        Cur        = FreeIdx; // to   itself
      }

    return FreeIdx;
  }

ORDHW
cClassMem::  Push (int  FreeIdx)
  {
    int &  Cur = IdxStackArr [FreeIdx];
    if  (Cur != FreeIdx)  return 0; // 0 elements filled in

    Cur        = TopOfStack;
    TopOfStack = FreeIdx;
    return 1; // 1 element filled in
  }

ORDHW
cClassMem::  GetBufferLen (int  Idx)  const
  {
    return Idx == IdxStackArr [Idx];
  }

int
cClassMem::  GetFreeUnits (int  StartIdx)  const
  {
    int  i;
    for  (i = StartIdx; i < MaxBufNum; i ++)
      {
        if  (i == IdxStackArr [i])  break;
      }

    return i - StartIdx;
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

