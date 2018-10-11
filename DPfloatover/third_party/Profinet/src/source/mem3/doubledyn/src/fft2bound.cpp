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
/*  F i l e               &F: fft2bound.cpp                             :F&  */
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
#include "fft2bound.hpp"

int
cFft1Bnd:: Decode (ORD08 const *  CodeArr, int  Step, int  MStart, int  MLen)
  {
    int        NumOffset = 1;
    int        CodeLen   = 0;
    int        i         = 0;
    int        CurCode;
    int        j;

    for  (;;)
      {
        CurCode = GetCode (CodeArr [i], MStart, MLen);

        j = MLen - 1;
        for  (;;)
          {
            if  ((CurCode & (1 << j)) == 0)  break;

            CodeLen   ++;
            NumOffset <<= 1;
            j         --;
            if  (j <  0)                     break;
          }

        if  (j >= 0)  break;

        i += Step;
      }

    if  (j == 0) // load first code
      {
        i      += Step;
        CurCode = GetCode (CodeArr [i], MStart, MLen);
        j       = MLen;
      }
    else         // clear high bits of code and adjust to high digit
      {
        CurCode &= (1 << j) - 1;
      }

    int  Encode;

    if  (CodeLen >= j)
      {
        Encode   = CurCode;
        CodeLen -= j;

        while  (CodeLen >= MLen)
          {
            i       += Step;
            Encode <<= MLen;
            Encode  |= GetCode (CodeArr [i], MStart, MLen);
            CodeLen -= MLen;
          }

        if  (CodeLen >  0)
          {
            i       += Step;
            Encode <<= CodeLen;
            Encode  |= GetCode (CodeArr [i], MStart, MLen) >> (MLen - CodeLen);
            CodeLen  = 0;
          }
      }
    else
      { Encode = CurCode >> (j - CodeLen); }

    return (Encode + (NumOffset - 1)) * Step;
  }

void
cFft1Bnd:: Encode (ORD08 *  CodeArr, int  Num, int  MStart, int  MLen)
  {
    int const
    Step = MEM3_Arith::Sign (Num);
    Num *= Step;

    int  CodeLen   = 0;
    int  NumOffset = 1;

    for  (;;)
      {
        int  NumLimit  = (NumOffset << 1) - 1;
        if  (Num <  NumLimit)  break;

        NumOffset <<= 1;
        CodeLen   ++;
      }

    Num -= (NumOffset - 1);

    int const  Head1_1 = (1 << MLen) - 1;
    int        HeadLen = CodeLen;
    int        i       = 0;
    while  (HeadLen >= MLen)
      {
        PutCode (CodeArr [i], MStart, MLen, Head1_1);
        HeadLen -= MLen;
        i       += Step;
      }

    int  j         = MStart;
    int  NibbleLen = MLen;
    if  (HeadLen >  0)
      {
        PutCode (CodeArr [i], j, HeadLen, (1 << HeadLen) - 1);
        j         -= HeadLen;
        NibbleLen -= HeadLen;
      }

    PutCode (CodeArr [i], j, 1, 0);
    j         -= 1;
    NibbleLen -= 1;
    if  (NibbleLen == 0)
      {
        j         = MStart;
        NibbleLen = MLen;
        i        += Step;
      }

    if  (CodeLen >= NibbleLen)
      {
        int const  Shift = CodeLen - NibbleLen;

        PutCode (CodeArr [i], j, NibbleLen, Num >> Shift);
        CodeLen -= NibbleLen;
        i       += Step;
        Num     &= (1 << Shift) - 1;

        j         = MStart;
        NibbleLen = MLen;
      }

    while  (CodeLen >= MLen)
      {
        int const  Shift = CodeLen - MLen;

        PutCode (CodeArr [i], MStart, MLen, Num >> Shift);
        CodeLen -= MLen;
        i       += Step;
        Num     &= (1 << Shift) - 1;
      }

    if  (CodeLen >  0)
      {
        PutCode (CodeArr [i], j, CodeLen, Num);

        j         -= CodeLen;
        NibbleLen -= CodeLen;
      }

    if  ((NibbleLen % MLen) >  0)
      {
        PutCode (CodeArr [i], j, NibbleLen, 0);
      }
  }

void
cFft1Bnd:: Reset (ORD08 *  CodeArr, int  Step, int  MStart, int  MLen)
  {
    int        CodeLen   = 0;
    int        i         = 0;
    int        j;

    for  (;;)
      {
        int CurCode = GetCode (CodeArr [i], MStart, MLen);

        j = MLen - 1;
        for  (;;)
          {
            if  ((CurCode & (1 << j)) == 0)  break;

            CodeLen   ++;
            j         --;
            if  (j <  0)                     break;
          }

        if  (j >= 0)  break;

        i += Step;
      }

    int
    NumBytes = (CodeLen * 2 /* + 1 */ + MLen /* - 1 */) / MLen;
    i        = 0;

    do
      {
        PutCode (CodeArr [i], MStart, MLen, 0);
        i        += Step;
        NumBytes -= 1;
      }
    while  (NumBytes >  0);
  }

cFft1Bnd::  cFft1Bnd (ORD08 *  GranArray, int  Num, int const &  GranOut, bool  GenCluster)
  : mGranArray (GranArray),
    mDirSign   (MEM3_Arith::Sign (Num)),
    mGranStart (mDirSign >  0 ? 0 : -Num - 1),
    mGranOut   (GranOut),
    mGranBnd   (mGranStart),
    mGranEnd   (mGranStart - mDirSign),
    mGranUsed  (0)
  {
    if  (GenCluster)  InitEmptyCluster (Num);
  }

cFft1Bnd::  cFft1Bnd (ORD08 *  GranArray, int  Num, cFft1Bnd const &  AntiPool, bool  GenCluster)
  : mGranArray (GranArray),
    mDirSign   (MEM3_Arith::Sign (Num)),
    mGranStart (mDirSign >  0 ? 0 : -Num - 1),
    mGranOut   (AntiPool. mGranEnd),
    mGranBnd   (mGranStart),
    mGranEnd   (mGranStart - mDirSign),
    mGranUsed  (0)
  {
    if  (GenCluster)  InitEmptyCluster (Num);
  }

void
cFft1Bnd::  InitEmptyCluster (int  Num)
  {
    int const  Len = Num * mDirSign;

    MEM3_ADMIN_MEMSET (mGranArray, 0x00, Len);
    SetupCluster      (cEntry (0, Len - 1));
  }

void
cFft1Bnd:: ClearBlock (cEntry const &   Entry)
  {
    if  (((Entry. End - Entry. Pos) * mDirSign) <  0)  return;

    Reset   (mGranArray + Entry. Pos, MEM3_Arith::Sign (Entry. End - Entry. Pos), eShBlockLenStart, eShBlockLenBits);
    PutCode (mGranArray [Entry. Pos], eShBufStart, eShBufStartBits, false);

    Reset   (mGranArray + Entry. End, MEM3_Arith::Sign (Entry. Pos - Entry. End), eShBlockLenStart, eShBlockLenBits);
    PutCode (mGranArray [Entry. End], eShBufStart, eShBufStartBits, false);
  }

void
cFft1Bnd:: ClearCluster (cEntry const &   Entry)
  {
    if  (((Entry. End - Entry. Pos) * mDirSign) <  0)  return;

    Reset   (mGranArray + Entry. Pos, MEM3_Arith::Sign (Entry. End - Entry. Pos), eShClustLenStart, eShClustLenBits);
    PutCode (mGranArray [Entry. Pos], eShCluStart, eShCluStartBits, false);

    Reset   (mGranArray + Entry. End, MEM3_Arith::Sign (Entry. Pos - Entry. End), eShClustLenStart, eShClustLenBits);
    PutCode (mGranArray [Entry. End], eShCluStart, eShCluStartBits, false);
  }

void
cFft1Bnd:: SetupBlock (cEntry const &   Entry)
  {
    if  (((Entry. End - Entry. Pos) * mDirSign) <  0)  return;

    PutCode     (mGranArray [Entry. Pos], eShBufStart, eShBufStartBits, true);
    EncodeBlock (mGranArray + Entry. Pos, Entry. End - Entry. Pos);

    PutCode     (mGranArray [Entry. End], eShBufStart, eShBufStartBits, true);
    EncodeBlock (mGranArray + Entry. End, Entry. Pos - Entry. End);
  }

void
cFft1Bnd:: SetupCluster (cEntry const &   Entry)
  {
    if  (((Entry. End - Entry. Pos) * mDirSign) <  0)  return;

    PutCode     (mGranArray [Entry. Pos], eShCluStart, eShCluStartBits, true);
    EncodeClust (mGranArray + Entry. Pos, Entry. End - Entry. Pos);

    PutCode     (mGranArray [Entry. End], eShCluStart, eShCluStartBits, true);
    EncodeClust (mGranArray + Entry. End, Entry. Pos - Entry. End);
  }

int
cFft1Bnd:: Get1stGranAndFix (int  Num)
  {
    if  (mGranStart == mGranOut)  return mGranOut; // pool is compressed to be empty

    cEntry        OldLeftArea (mGranStart, mGranStart - mDirSign);
    cEntry        OldFreeArea;
    int           Next;

    MEM3_OPTIONAL_INNER_ASSERT (CluStart (mGranArray [mGranStart]));

    if  (BufStart (mGranArray [mGranStart]))
      {
        OldLeftArea. End =   DecodeClust (mGranArray + OldLeftArea. Pos, mDirSign)
                           + OldLeftArea. Pos;
      }

    for  (;;)
      {
        OldFreeArea. Pos = OldLeftArea. End + mDirSign;
        if  (OldFreeArea. Pos == mGranOut)  return OldFreeArea. Pos; // no more memory

        MEM3_OPTIONAL_INNER_ASSERT (  CluStart (mGranArray [OldFreeArea. Pos]));
        MEM3_OPTIONAL_INNER_ASSERT (! BufStart (mGranArray [OldFreeArea. Pos]));

        OldFreeArea. End =   DecodeClust (mGranArray + OldFreeArea. Pos, mDirSign)
                           + OldFreeArea. Pos;

        Next = OldFreeArea. End + mDirSign;

        if  (((Next - OldFreeArea. Pos - Num) * mDirSign) >= 0)  break;
        if  (Next == mGranOut)                                   return Next; // no more memory

        MEM3_OPTIONAL_INNER_ASSERT (CluStart (mGranArray [Next]));
        MEM3_OPTIONAL_INNER_ASSERT (BufStart (mGranArray [Next]));

        OldLeftArea. Pos = Next;
        OldLeftArea. End =   DecodeClust (mGranArray + OldLeftArea. Pos, mDirSign)
                           + OldLeftArea. Pos;
      }

    // fixup positions
    cEntry const  NewFreeArea  (OldFreeArea. Pos + Num, OldFreeArea. End);
    cEntry const  NewBuffer    (OldFreeArea. Pos, OldFreeArea. Pos + Num - mDirSign);
    cEntry const  OldRightArea (Next,
                                Next != mGranOut
                                  ? Next + DecodeClust (mGranArray + Next, mDirSign)
                                  : Next - mDirSign
                               );
    cEntry const  NewUsedArea  (OldLeftArea. Pos,
                                NewBuffer. End != OldFreeArea. End
                                  ? NewBuffer.    End
                                  : OldRightArea. End
                               );

    // clear old clusters and setup new ones and block
    ClearCluster (OldFreeArea);
    if  (NewUsedArea. Pos != NewBuffer. Pos)  ClearCluster (OldLeftArea);
    if  (NewUsedArea. End != NewBuffer. End)  ClearCluster (OldRightArea);
    SetupCluster (NewUsedArea);
    SetupCluster (NewFreeArea);
    SetupBlock   (NewBuffer);

    // adjust boundary: large, if new larger than end
    if  (((NewUsedArea. End - mGranEnd) * mDirSign) >  0)
      {
        mGranEnd = NewUsedArea. End;
        mGranBnd = mGranEnd + mDirSign;
      }

    mGranUsed += Num;
    return NewBuffer. Pos;
  }

int
cFft1Bnd:: GetEndGranAndFix (int  Num)
  {
    int  Pos = Get1stGranAndFix (-Num);

    if  (Pos != mGranOut)  Pos += DecodeBlock (mGranArray + Pos, mDirSign);
    return Pos;
  }

void
cFft1Bnd:: Put1st_Inner (int  Pos, int  End)
  {
    cEntry const  OldBuffer (Pos, End);
    cEntry        OldUsed   (OldBuffer);

    for  (;;)
      {
        if  (OldUsed. Pos == mGranStart)      break;

        int  Next    = OldUsed. Pos - mDirSign;
        if  (! BufStart (mGranArray [Next]))  break;

        OldUsed. Pos = Next + DecodeBlock (mGranArray + Next, -mDirSign);
      }

    for  (;;)
      {
        int  Next    = OldUsed. End + mDirSign;
        if  (Next         == mGranOut)        break;
        if  (! BufStart (mGranArray [Next]))  break;

        OldUsed. End = Next + DecodeBlock (mGranArray + Next,  mDirSign);
      }

    int
    Next = OldBuffer. End + mDirSign;
    cEntry const  OldRightFree (Next,
                                (Next           != mGranOut)   && (OldUsed. End == OldBuffer. End)
                                  ? Next + DecodeClust (mGranArray + Next, mDirSign)
                                  : OldBuffer. End
                               );

    Next = OldBuffer. Pos - mDirSign;
    cEntry const  OldLeftFree  ((OldBuffer. Pos != mGranStart) && (OldUsed. Pos == OldBuffer. Pos)
                                  ? Next + DecodeClust (mGranArray + Next, -mDirSign)
                                  : OldBuffer. Pos,
                                Next
                               );

    cEntry const  NewFree      (OldLeftFree. Pos, OldRightFree. End);
    cEntry const  NewLeftUsed  (OldUsed. Pos, NewFree. Pos - mDirSign);
    cEntry const  NewRightUsed (NewFree. End + mDirSign, OldUsed. End);

    // clear and setup of block and clusters
    ClearBlock   (OldBuffer);
    ClearCluster (OldLeftFree);
    ClearCluster (OldRightFree);
    ClearCluster (OldUsed);
    SetupCluster (NewLeftUsed);
    SetupCluster (NewRightUsed);
    SetupCluster (NewFree);

    // adjust boundary: short, if old matches end
    if  (OldBuffer. End == mGranEnd)
      {
        mGranEnd = NewLeftUsed. End;
        mGranBnd = mGranEnd + mDirSign;
      }
  }

void
cFft1Bnd:: Put1stGranAndFix (int  Pos)
  {
    MEM3_OPTIONAL_INNER_ASSERT (   (((mGranBnd - Pos)   * mDirSign) >  0)    // inside dynamic pool-area
                                && (((Pos - mGranStart) * mDirSign) >= 0)
                               );
    MEM3_OPTIONAL_INNER_ASSERT (BufStart (mGranArray [Pos]));                // start of buffer

    int const  End = Pos + DecodeBlock (mGranArray + Pos, mDirSign);

    MEM3_OPTIONAL_INNER_ASSERT (((mGranBnd - End) * mDirSign) >  0);  // inside dynamic pool-area
    MEM3_OPTIONAL_INNER_ASSERT (BufStart (mGranArray [End]));         // end of buffer

    Put1st_Inner (Pos, End);
    mGranUsed -= End - Pos;
  }

void
cFft1Bnd:: PutEndGranAndFix (int  End)
  {
    MEM3_OPTIONAL_INNER_ASSERT (   (((mGranBnd - End)   * mDirSign) >  0)    // inside dynamic pool-area
                                && (((End - mGranStart) * mDirSign) >= 0)
                               );
    MEM3_OPTIONAL_INNER_ASSERT (BufStart (mGranArray [End]));                // start of buffer

    int const  Pos = End + DecodeBlock (mGranArray + End, -mDirSign);

    MEM3_OPTIONAL_INNER_ASSERT (((mGranBnd - Pos) * mDirSign) >  0);  // inside dynamic pool-area
    MEM3_OPTIONAL_INNER_ASSERT (BufStart (mGranArray [Pos]));         // end of buffer

    Put1st_Inner (Pos, End);
    mGranUsed -= Pos - End;
  }

int
cFft1Bnd:: GetBufferBase (int  Pos)  const
  {
    if  (   (((mGranBnd - Pos)   * mDirSign) <= 0)    // inside dynamic pool-area
         || (((Pos - mGranStart) * mDirSign) <  0)
        )
      { return -1; }

    while  (! BufStart (mGranArray [Pos]))
      {
        if  (CluStart (mGranArray [Pos]))  return -1;
        if  (Pos == mGranStart)            return -1;
        Pos -= mDirSign;
      }

    return Pos;
  }

int
cFft1Bnd:: GetBufferEnd (int  Pos)  const
  {
    if  (   (((mGranBnd - Pos)   * mDirSign) <= 0)    // inside dynamic pool-area
         || (((Pos - mGranStart) * mDirSign) <  0)
        )
      { return -1; }

    while  (! BufStart (mGranArray [Pos]))
      {
        if  (CluStart (mGranArray [Pos]))  return -1;
        Pos += mDirSign;
        if  (Pos == mGranBnd)              return -1;
      }

    return Pos;
  }

int
cFft1Bnd:: GetBufferLenBase (int  Pos)  const
  {
    if  (   (((mGranBnd - Pos)   * mDirSign) <= 0)    // inside dynamic pool-area
         || (((Pos - mGranStart) * mDirSign) <  0)
        )
      { return 0; }

    if  (! BufStart (mGranArray [Pos]))  return 0;

    return DecodeBlock (mGranArray + Pos, mDirSign) + mDirSign;
  }

int
cFft1Bnd::  GetBufferLenEnd  (int  End)  const
  {
    if  (   (((mGranBnd - End)   * mDirSign) <= 0)    // inside dynamic pool-area
         || (((End - mGranStart) * mDirSign) <  0)
        )
      { return 0; }

    if  (! BufStart (mGranArray [End]))  return 0;

    return mDirSign - DecodeBlock (mGranArray + End, -mDirSign);
  }

int
cFft1Bnd::  GetFreeLenBase (int  Pos) const
  {
    MEM3_OPTIONAL_INNER_ASSERT (   (((mGranOut - Pos)   * mDirSign) >  0)
                                && (((Pos - mGranStart) * mDirSign) >= 0)
                               );

    MEM3_OPTIONAL_INNER_ASSERT (CluStart (mGranArray [Pos]));
    if  (BufStart (mGranArray [Pos]))  return 0;

    return DecodeClust (mGranArray + Pos, mDirSign) + mDirSign;
  }

int
cFft1Bnd::  GetFreeLenEnd  (int  End) const
  {
    MEM3_OPTIONAL_INNER_ASSERT (   (((mGranOut - End)   * mDirSign) >  0)
                                && (((End - mGranStart) * mDirSign) >= 0)
                               );

    MEM3_OPTIONAL_INNER_ASSERT (CluStart (mGranArray [End]));
    if  (BufStart (mGranArray [End]))  return 0;

    return mDirSign - DecodeClust (mGranArray + End, mDirSign);
  }

int
cFft1Bnd::  GetUsedLenBase (int  Pos) const
  {
    MEM3_OPTIONAL_INNER_ASSERT (   (((mGranBnd - Pos)   * mDirSign) >  0)
                                && (((Pos - mGranStart) * mDirSign) >= 0)
                               );

    MEM3_OPTIONAL_INNER_ASSERT (CluStart (mGranArray [Pos]));
    if  (! BufStart (mGranArray [Pos]))  return 0;

    return DecodeClust (mGranArray + Pos, mDirSign) + mDirSign;
  }

int
cFft1Bnd::  GetUsedLenEnd  (int  End) const
  {
    MEM3_OPTIONAL_INNER_ASSERT (   (((mGranBnd - End)   * mDirSign) >  0)
                                && (((End - mGranStart) * mDirSign) >= 0)
                               );

    MEM3_OPTIONAL_INNER_ASSERT (CluStart (mGranArray [End]));
    if  (! BufStart (mGranArray [End]))  return 0;

    return mDirSign - DecodeClust (mGranArray + End, mDirSign);
  }

int
cFft1Bnd::  GetNextBufStart (int  Pos) const
  {
    if  (Pos <  0)                              return -1;
    if  (((Pos - mGranStart) * mDirSign) <  0)  return -1;

    for  (;;)
      {
        if  (((mGranBnd - Pos) * mDirSign) >= 0)  return -1;
        if  (BufStart (mGranArray [Pos]))         return Pos;

        if  (CluStart (mGranArray [Pos]))
          { Pos += DecodeClust (mGranArray + Pos, mDirSign); }

        Pos += mDirSign;
      }
  }

int
cFft1Bnd::  GetNextBufEnd (int  End) const
  {
    if  (End <  0)                              return -1;
    if  (((mGranBnd - End) * mDirSign) >= 0)    return -1;

    for  (;;)
      {
        if  (((End - mGranStart) * mDirSign) <  0)  return -1;
        if  (BufStart (mGranArray [End]))           return End;

        if  (CluStart (mGranArray [End]))
          { End += DecodeClust (mGranArray + End, mDirSign); }

        End -= mDirSign;
      }
  }

cFft2Bnd:: cFft2Bnd (ORD08 *  GranArray, ORDHW  GranNum)
  : mGranNum     (GranNum),
    mGranArray   (GranArray),
    mPoolNormal  (GranArray,   int (GranNum), mPoolReverse),
    mPoolReverse (GranArray, - int (GranNum), mPoolNormal, false)
  {
  }

ORDHW
cFft2Bnd:: GetGranNormal  (ORDHW  Num)
  {
    ORDHW const  BufIdx = mPoolNormal. Get1stGranAndFix (Num);
    return int (BufIdx) == mPoolNormal. GranOut () ? mGranNum : BufIdx;
  }

ORDHW
cFft2Bnd:: GetGranReverse (ORDHW  Num)
  {
    ORDHW const  BufIdx = mPoolReverse. GetEndGranAndFix (Num);
    return int (BufIdx) == mPoolReverse. GranOut () ? mGranNum : BufIdx;
  }

void
cFft2Bnd::  PutGran (ORDHW  Pos)
  {
    if  (int (Pos) <  mPoolNormal. GranBnd ())
      {
        mPoolNormal.  Put1stGranAndFix (Pos);
      }
    else
      {
        mPoolReverse. PutEndGranAndFix (Pos);
      }
  }

int
cFft2Bnd::  GetBufferBase (int  Pos) const
  {
    return int (Pos) <  mPoolNormal. GranBnd ()
             ? mPoolNormal.  GetBufferBase (Pos)
             : mPoolReverse. GetBufferEnd  (Pos);
  }

ORDHW
cFft2Bnd::  GetBufferLen (int  Pos) const
  {
    int const  BufLen = int (Pos) <  mPoolNormal. GranBnd ()
                          ?  mPoolNormal.  GetBufferLenBase (Pos)
                          : -mPoolReverse. GetBufferLenEnd  (Pos);

    return BufLen >= 0 ? BufLen : 0;
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/