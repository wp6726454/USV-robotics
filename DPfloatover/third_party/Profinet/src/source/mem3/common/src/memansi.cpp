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
/*  F i l e               &F: memansi.cpp                               :F&  */
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

#include "mem3pool.hpp"

inline ORDHW   Ggt          (ORDHW  a, ORDHW b);
static void    AdjustToPool (void **  ppMemTarget, ORDHW *  pTargetSize, size_t  HeadSize, ORDHW  Alignment);

void
MEMORY3_INTERN::  PoolAssignUint (ORDHW *  pLval, ORDHW  Rval)
  {
    * pLval = Rval;
  }

ORDHW
MEMORY3_INTERN::  PoolGetUint (ORDHW const *  pRval)
  {
    return * pRval;
  }

void
MEMORY3_INTERN::  PoolAssignUint32_LeMa (ORD08 *  pLval, ORDHW  Rval)
  {
    pLval [0] = ORD08 (Rval >>  0);
    pLval [1] = ORD08 (Rval >>  8);
    pLval [2] = ORD08 (Rval >> 16);
    pLval [3] = ORD08 (Rval >> 24);
  }

ORDHW
MEMORY3_INTERN::  PoolGetUint32_LeMa (ORD08 const *  pRval)
  {
    return   (ORDHW (pRval [0]) <<  0)
           + (ORDHW (pRval [1]) <<  8)
           + (ORDHW (pRval [2]) << 16)
           + (ORDHW (pRval [3]) << 24);
  }

#ifdef  POST_INVALIDATE

inline
void
PrologToPool (void *  pPoolMem, ORDHW  PoolMemSize, BYTE  Preset, pMemSet_ft  pMemSet_f)
  {
    pMemSet_f (pPoolMem, Preset, PoolMemSize);
  }

inline
void
PrologToBuf (void *, ORDHW, BYTE, pMemSet_ft)
  {
  }

inline
void
PrologToBufTail (void *, ORDHW, ORDHW, BYTE, pMemSet_ft)
  {
  }

inline
void
EpilogToBuf (cMemPool *  pMem, void *  pBuf, BYTE  Preset, pMemSet_ft  pMemSet_f)
  {
    pMemSet_f (pBuf, Preset, pMem-> GetBufSize (pBuf));
  }

inline
void
EpilogToBufTail (void *  pBuf, size_t  NewSize, size_t  OldSize, BYTE  Preset, pMemSet_ft  pMemSet_f)
  {
    pMemSet_f (static_cast <ORD08 *> (pBuf) + NewSize, Preset, OldSize - NewSize);
  }

#else  // end  POST_INVALIDATE, begin PRE_INVALIDATE

inline
void
PrologToPool (void *, ORDHW, BYTE, pMemSet_ft)
  {
  }

inline
void
PrologToBuf (void *  pBuf, ORDHW  BufSize, BYTE  Preset, pMemSet_ft  pMemSet_f)
  {
    pMemSet_f (pBuf, Preset, BufSize);
  }

inline
void
PrologToBufTail (void *  pBuf, ORDHW  BufSize, ORDHW  Offset, BYTE  Preset, pMemSet_ft  pMemSet_f)
  {
    pMemSet_f (static_cast <ORD08 *> (pBuf) + Offset, Preset, BufSize - Offset);
  }

inline
void
EpilogToBuf (cMemPool *, void *, BYTE, pMemSet_ft)
  {
  }

inline
void
EpilogToBufTail (void *, size_t, size_t, BYTE, pMemSet_ft)
  {
  }

#endif  // end   PRE_INVALIDATE

bool
MEMORY3_INTERN::CHECK_OVERWRITE::  VerifyMem3Buf (int  PoolName, void *  pBuf)
  {
    pPoolGetUint_ft        const  GetOff    = cMemPool::GetGetUintVal   (PoolName);
    pPoolGetUint32_LeMa_ft const  GetLabel  = cMemPool::GetGetUint32Val (PoolName);
    ORDHW                  const  CheckSize = GetOff   (static_cast <ORDHW *> (pBuf) - 1);
    ORDHW                  const  Magic     = GetLabel (static_cast <ORD08 *> (pBuf) + CheckSize - sizeof (ORDHW));
    return Magic == STOP_LABEL;
  }

void
MEMORY3_INTERN::CHECK_OVERWRITE::  Mem3_InitBuf (int  PoolName, void *  pBuf, size_t  LogSize, ORDHW  PhySize)
  {
    PrologToBuf (pBuf, PhySize, cMemPool::GetInitVal (PoolName), cMemPool::GetMemSetVal (PoolName));

    pPoolAssignUint_ft        const  AssignOff   = cMemPool::GetAssignUintVal   (PoolName);
    pPoolAssignUint32_LeMa_ft const  AssignLabel = cMemPool::GetAssignUint32Val (PoolName);

    AssignOff   (static_cast <ORDHW *> (pBuf), LogSize + sizeof (ORDHW));
    AssignLabel (static_cast <ORD08 *> (pBuf) + LogSize + sizeof (ORDHW), STOP_LABEL);
  }

void
MEMORY3_INTERN::CHECK_OVERWRITE::  Mem3_InvalOvwr (void *  pSysBuf, BYTE  Preset, pMemSet_ft  pMemSet_f)
  {
    ORDHW  CheckSize = *(static_cast <ORDHW *> (pSysBuf));

    pMemSet_f (static_cast <ORD08 *> (pSysBuf) + CheckSize, Preset, sizeof (ORDHW));
  }

void
MEMORY3_INTERN::CHECK_OVERWRITE::  Mem3_InitBufTail (int  PoolName, void *  pBuf, size_t  LogSize, ORDHW  PhySize, ORDHW  Offset)
  {
    PrologToBufTail (pBuf, PhySize, Offset, cMemPool::GetInitVal (PoolName), cMemPool::GetMemSetVal (PoolName));

    pPoolAssignUint_ft        const  AssignOff   = cMemPool::GetAssignUintVal   (PoolName);
    pPoolAssignUint32_LeMa_ft const  AssignLabel = cMemPool::GetAssignUint32Val (PoolName);

    AssignOff   (static_cast <ORDHW *> (pBuf), LogSize + sizeof (ORDHW));
    AssignLabel (static_cast <ORD08 *> (pBuf) + LogSize + sizeof (ORDHW), STOP_LABEL);
  }

void *
MEMORY3_INTERN::CHECK_OVERWRITE::  SysBuf (int  PoolName, void *  pUserBuf)
  {
    void * const  pSysBuf = static_cast <ORDHW *> (pUserBuf) - 1;

    pPoolGetUint_ft        const  GetOff   = cMemPool::GetGetUintVal   (PoolName);
    pPoolGetUint32_LeMa_ft const  GetLabel = cMemPool::GetGetUint32Val (PoolName);

    ORDHW const  CheckSize = GetOff   (static_cast <ORDHW *> (pSysBuf));
    ORDHW const  LabelVal  = GetLabel (static_cast <ORD08 *> (pSysBuf) + CheckSize);

    MEM3_USER_ASSERT (LabelVal == STOP_LABEL);
    return pSysBuf;
  }

ORDHW
MEMORY3_INTERN::CHECK_OVERWRITE::  GetBufSize (int  PoolName, void *  pBuf)
  {
    pPoolGetUint_ft        const  GetOff   = cMemPool::GetGetUintVal   (PoolName);
    pPoolGetUint32_LeMa_ft const  GetLabel = cMemPool::GetGetUint32Val (PoolName);

    ORDHW const  CheckSize = GetOff   (static_cast <ORDHW *> (pBuf) - 1) - sizeof (ORDHW);
    ORDHW const  LabelVal  = GetLabel (static_cast <ORD08 *> (pBuf) + CheckSize);

    MEM3_USER_ASSERT (LabelVal == STOP_LABEL);
    return CheckSize;
  }

void *
MEMORY3_INTERN::CHECK_OVERWRITE::  Mem3_UserBuf (void *  pSysBuf)
  {
    return static_cast <ORDHW *> (pSysBuf) + 1;
  }

bool
MEMORY3_INTERN::NOCHK_OVERWRITE::  VerifyMem3Buf (int, void *)
  {
    return true;
  }

void
MEMORY3_INTERN::NOCHK_OVERWRITE::  Mem3_InitBuf (int  PoolName, void *  pBuf, size_t, ORDHW  PhySize)
  {
    PrologToBuf (pBuf, PhySize,  cMemPool::GetInitVal (PoolName), cMemPool::GetMemSetVal (PoolName));
  }

void
MEMORY3_INTERN::NOCHK_OVERWRITE::  Mem3_InvalOvwr (void *, BYTE, pMemSet_ft)
  {
  }

void
MEMORY3_INTERN::NOCHK_OVERWRITE::  Mem3_InitBufTail (int  PoolName, void *  pBuf, size_t, ORDHW  PhySize, ORDHW  Offset)
  {
    PrologToBufTail (pBuf, PhySize, Offset,  cMemPool::GetInitVal (PoolName), cMemPool::GetMemSetVal (PoolName));
  }

void *
MEMORY3_INTERN::NOCHK_OVERWRITE::  SysBuf  (int, void *  pUserBuf)
  {
    return pUserBuf;
  }

ORDHW
MEMORY3_INTERN::NOCHK_OVERWRITE::  GetBufSize (int  PoolName, void *  pBuf)
  {
    cMemPoolPtrBusyLock  pPool (PoolName);

    int  BufSize = -1;
    if  (pPool != NULL)
      {
        BufSize = pPool-> GetBufSize (pBuf);
      }
    MEM3_USER_ASSERT (BufSize >= 0);

    return BufSize;
  }

void *
MEMORY3_INTERN::NOCHK_OVERWRITE::  Mem3_UserBuf (void *  pSysBuf)
  {
    return pSysBuf;
  }

size_t const  MEMORY3_INTERN::BufHeadSize [2] = { 0, sizeof (ORDHW) };
size_t const  MEMORY3_INTERN::BufTailSize [2] = { 0, sizeof (ORDHW) };
size_t const  MEMORY3_INTERN::AddBufSize  [2] = { 0, 2 * sizeof (ORDHW) };

bool    (* MEMORY3_INTERN::pfVerifyMem3Buf    [2]) (int, void *) =
  {
    NOCHK_OVERWRITE::VerifyMem3Buf,
    CHECK_OVERWRITE::VerifyMem3Buf
  };

void    (* MEMORY3_INTERN::pfMem3_InitBuf     [2]) (int, void *, size_t, ORDHW) =
  {
    NOCHK_OVERWRITE::Mem3_InitBuf,
    CHECK_OVERWRITE::Mem3_InitBuf
  };

void    (* MEMORY3_INTERN::pfMem3_InvalOvwr   [2]) (void *, BYTE, pMemSet_ft) =
  {
    NOCHK_OVERWRITE::Mem3_InvalOvwr,
    CHECK_OVERWRITE::Mem3_InvalOvwr
  };

void    (* MEMORY3_INTERN::pfMem3_InitBufTail [2]) (int, void *, size_t, ORDHW, ORDHW) =
  {
    NOCHK_OVERWRITE::Mem3_InitBufTail,
    CHECK_OVERWRITE::Mem3_InitBufTail
  };

void *  (* MEMORY3_INTERN::pfSysBuf           [2]) (int, void *) =
  {
    NOCHK_OVERWRITE::SysBuf,
    CHECK_OVERWRITE::SysBuf
  };

ORDHW   (* MEMORY3_INTERN::pfGetBufSize       [2]) (int, void *) =
  {
    NOCHK_OVERWRITE::GetBufSize,
    CHECK_OVERWRITE::GetBufSize
  };

void *  (* MEMORY3_INTERN::pfMem3_UserBuf     [2]) (void *) =
  {
    NOCHK_OVERWRITE::Mem3_UserBuf,
    CHECK_OVERWRITE::Mem3_UserBuf
  };

inline
size_t
InnerBufHeadSize (int  PoolIdx)
  {
    return MEMORY3_INTERN::BufHeadSize [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)];
  }

inline
size_t
InnerBufTailSize (int  PoolIdx)
  {
    return MEMORY3_INTERN::BufTailSize [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)];
  }

inline
size_t
InnerAddBufSize (int  PoolIdx)
  {
    return MEMORY3_INTERN::AddBufSize [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)];
  }

inline
bool
Inner_VerifyMem3Buf (int  PoolIdx, void *  pBuf)
  {
    return MEMORY3_INTERN::pfVerifyMem3Buf [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)] (PoolIdx, pBuf);
  }

inline
void *
Inner_Mem3_UserBuf (int  PoolIdx, void *  pSysBuf)
  {
    return MEMORY3_INTERN::pfMem3_UserBuf [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)] (pSysBuf);
  }

bool
VerifyMem3Buf (void *  pBuf)
  {
    cOldGeneralLock  Lock; // lock clean

    int   PoolIdx = cMemPool::GetName (pBuf);
    bool  RetVal  = PoolIdx >= 0;

    if  (RetVal)
      { RetVal = Inner_VerifyMem3Buf (PoolIdx, pBuf); }

    return RetVal;
  }

void *
Mem3_UserBuf (void *  pSysBuf)
  {
    cOldGeneralLock  Lock; // lock clean

    int   PoolIdx = cMemPool::GetName (pSysBuf);
    MEM3_USER_ASSERT (PoolIdx >= 0);

    return Inner_Mem3_UserBuf (PoolIdx, pSysBuf);
  }

void *
Mem3_CondionalUserBuf (void *  pIfaSysBuf)
  {
    return pIfaSysBuf != NULL ? Mem3_UserBuf (pIfaSysBuf) : NULL;
  }

inline
void
Inner_Mem3_InitBuf (int  PoolIdx, void *  pBuf, size_t  LogSize, ORDHW  PhySize)
  {
    MEMORY3_INTERN::pfMem3_InitBuf [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)] (PoolIdx, pBuf, LogSize, PhySize);
  }

inline
void
Inner_Mem3_InitBufTail (int  PoolIdx, void *  pBuf, size_t  LogSize, ORDHW  PhySize, ORDHW  Offset)
  {
    MEMORY3_INTERN::pfMem3_InitBufTail [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)] (PoolIdx, pBuf, LogSize, PhySize, Offset);
  }

inline
void
Inner_Mem3_InvalOvwr (int  PoolIdx, void *  pSysBuf, BYTE  Preset)
  {
    MEMORY3_INTERN::pfMem3_InvalOvwr [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)] (pSysBuf, Preset, cMemPool::GetMemSetVal (PoolIdx));
  }

inline
void *
Inner_SysBuf  (int  PoolIdx, void *  pUserBuf)
  {
    return MEMORY3_INTERN::pfSysBuf [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)] (PoolIdx, pUserBuf);
  }

ORDHW
GetBufSize (void *  pBuf)
  {
    cOldGeneralLock  Lock; // lock clean

    int  PoolIdx = cMemPool::GetName (pBuf);
    MEM3_USER_ASSERT (PoolIdx >= 0);

    int  BufSize = -1;

    if  (PoolIdx >= 0)
      {
        BufSize = MEMORY3_INTERN::pfGetBufSize [cMemPool::sPoolAdmin. CheckOverwrite (PoolIdx)] (PoolIdx, pBuf);
      }

    return BufSize;
  }

#ifdef  MEM3_BUFFER_STATISTIC

struct cMem3Statistic
  {
    MEM3_STATISTIC *   pStatPoolBuf;
    int                StatPoolBufLen;
    int                StatPoolBddLen;

    void               RecognizeBuf (int);
  };

static cMem3Statistic     BufStatistic   [POOL_NUM] [2];

void
cMem3Statistic::  RecognizeBuf (int  BufSize)
  {
    if  (StatPoolBufLen <= 0)  return;

    int  Idx       = StatPoolBddLen;
    int  Threshold = pStatPoolBuf [Idx].BufClassSize;

    if  (BufSize >  Threshold)
      {
        Idx += (BufSize - Threshold) / Threshold;
        if  (Idx >= StatPoolBufLen)  Idx = StatPoolBufLen - 1;
      }
    else
      {
        while  ((Idx >  0) && (BufSize <= pStatPoolBuf [Idx - 1].BufClassSize))  Idx --;
      }

    pStatPoolBuf [Idx].BufAllocationNum ++;
  }

inline
void
RecognizeBuf (int  BufSize, int  PoolName)
  {
    BufStatistic [PoolName & MEM3_AliasMask] [PoolName >> MEM3_AliasShift].RecognizeBuf (BufSize);
  }

extern "C"
void
SetMem3StatisticBuf (MEM3_STATISTIC *  pStatBuf, int  StatBufLen, int  PoolName)
  {
    if  (pStatBuf == NULL)  StatBufLen = 0;

    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pPool (PoolName);

    if  (pPool != NULL)
      {
        cMem3Statistic &  TmpStatistik = BufStatistic [PoolName & MEM3_AliasMask] [PoolName >> MEM3_AliasShift];

        if  (StatBufLen > 0)
          {
            int  Granularity = pPool-> PhysGranularity ();
            int  Threshold   = pPool-> PhysThreshold   ();
            int  StatClasses = StatBufLen - 1;
            int  i           = 0;
            int  Limit       = Granularity << (BUDDY_MAX_LEN - 2);

            if  (Threshold >  Limit)  Threshold = Limit;

            while  ((i <  StatClasses) && (Granularity <  Threshold))
              {
                pStatBuf [i].BufClassSize     = Granularity;
                pStatBuf [i].BufAllocationNum = 0;
                i           ++;
                Granularity <<= 1;
              }

            TmpStatistik. StatPoolBddLen = i;

            while  ((i <  StatClasses) && (Granularity <  Limit))
              {
                pStatBuf [i].BufClassSize     = Granularity;
                pStatBuf [i].BufAllocationNum = 0;
                i           ++;
                Granularity += Threshold;
              }

            pStatBuf [i].BufClassSize     = MAX_INTHW;
            pStatBuf [i].BufAllocationNum = 0;
          }
        else
          { TmpStatistik. StatPoolBddLen = 0; }

        TmpStatistik. pStatPoolBuf   = pStatBuf;
        TmpStatistik. StatPoolBufLen = StatBufLen;
      }
  }

#else  //  end  MEM3_BUFFER_STATISTIC, begin  NO_BUFFER_STATISTIC

inline
void
RecognizeBuf (int, int)
  {
  }

extern "C"
void
SetMem3StatisticBuf (MEM3_STATISTIC *, int, int)
  {
  }

#endif  // end  NO_BUFFER_STATISTIC

void
AdjustToPool (void **  ppMemTarget, ORDHW *  pTargetSize, size_t  HeadSize, ORDHW  Alignment)
  {
    ORDHW const  Mem3Offset = AddrFromPtr (* ppMemTarget);
    ORDHW const  UserOffset = Mem3Offset + HeadSize;
    ORDHW const  PoolShift  = (UserOffset + Alignment - 1) / Alignment * Alignment - UserOffset;

    * (           pTargetSize)  -= PoolShift;
    * ((ORD08 **) ppMemTarget)  += PoolShift;
  }

#ifndef Mem3_TraceAlloc
  #define Mem3_TraceAlloc( p )  do {} while (0)
#endif

#ifndef Mem3_TraceFree
  #define Mem3_TraceFree(  p )  do {} while (0)
#endif

extern "C" {

void *
Mem3palloc (size_t  BufSize, int  PoolName)
  {
    void *  pBuf = NULL;

    if  ((BufSize >  0) && (ORDHW (PoolName & MEM3_AliasMask) <  cMemPool::GetPoolNum ()))
      {
        size_t  TgtSize = 0;

          { cOldGeneralLock      Lock; // lock clean
            cMemPoolPtrBusyLock  pMem (PoolName);

            if  (pMem != NULL)
              {
                size_t const  FrameSize = BufSize + InnerAddBufSize (PoolName);
                if  (FrameSize >= BufSize) // else a wrap around
                  {
                    pBuf = pMem-> GetBuf (FrameSize, TgtSize);
                  }
              }

          }

        if  (pBuf != NULL)
          {
            Inner_Mem3_InitBuf (PoolName, pBuf, BufSize, TgtSize);
            pBuf = Inner_Mem3_UserBuf (PoolName, pBuf);
            RecognizeBuf (TgtSize, PoolName);
          }
      }

    Mem3_TraceAlloc (pBuf);
    return pBuf;
  }

void *
Mem3pralloc (void *  pBuf, size_t  BufSize, int  PoolName)
  {
    if  (BufSize <= 0) // another style of Mem3free
      {
        Mem3free (pBuf);
        return NULL;
      }

    if  (pBuf == NULL) // another style of Mem3palloc
      {
        return Mem3palloc (BufSize, PoolName);
      }

    cOldGeneralLock  Lock; // lock clean

    int    const  OldPoolName = cMemPool::GetName (pBuf);
    MEM3_USER_ASSERT (OldPoolName >= 0);

    if  (PoolName <  0) // if PoolName is less than 0, the new pool matches the original one
      {
        PoolName = OldPoolName;
      }

    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ()) // index out of range
      {
        Mem3free (pBuf);
        return NULL;
      }

    size_t const  FrameSize = BufSize + InnerAddBufSize (PoolName);

    if  (FrameSize <  BufSize) // wrap around of internal address space
      {
        Mem3free (pBuf);
        return NULL;
      }

    Mem3_TraceFree (pBuf);

    pBuf = Inner_SysBuf (OldPoolName, pBuf);

    size_t      TgtSize  = 0;
    void     *  pNewBuf  = NULL;
    cMemPool *  pOldMem  = cMemPool::GetPool (OldPoolName);
    size_t      CopySize = pOldMem-> GetBufSize (pBuf);

    cMemPoolPtrBusyLock  pNewMem (PoolName);

    BYTE       const  PresetOld  = cMemPool::GetInitVal   (OldPoolName);
    pMemSet_ft const  pMemSetOld = cMemPool::GetMemSetVal (OldPoolName);

    if  (pNewMem == pOldMem)
      {
        if  (FrameSize >  CopySize)
          {
            pNewMem-> TryBuf (pBuf, FrameSize, TgtSize);

            if  (TgtSize >= FrameSize)
              {
                Inner_Mem3_InvalOvwr   (OldPoolName, pBuf, PresetOld);
                pNewBuf = pBuf;
                Inner_Mem3_InitBufTail (PoolName, pNewBuf, BufSize, TgtSize, CopySize);
                pNewBuf = Inner_Mem3_UserBuf (PoolName, pNewBuf);
                RecognizeBuf (TgtSize, PoolName);
              }
          }
        else
          {
            Inner_Mem3_InvalOvwr (OldPoolName, pBuf, PresetOld);
            EpilogToBufTail      (pBuf, FrameSize, CopySize, PresetOld, pMemSetOld);
            pNewMem-> ShortBuf   (pBuf, FrameSize, TgtSize);
            pNewBuf = pBuf;
            Inner_Mem3_InitBufTail (PoolName, pNewBuf, BufSize, TgtSize, BufSize);
            pNewBuf = Inner_Mem3_UserBuf (PoolName, pNewBuf);
            RecognizeBuf (TgtSize, PoolName);
          }
      }

    if  (pNewBuf == NULL)
      {
        if  (pNewMem != NULL)
          {
            pNewBuf = pNewMem-> GetBuf (FrameSize, TgtSize);
          }

        if  (TgtSize <  CopySize)  CopySize = TgtSize;

        if  (pNewBuf != NULL)
          {
            Inner_Mem3_InvalOvwr   (OldPoolName, pBuf, PresetOld);
            MEM3_MEMCPY            (pNewBuf, pBuf, CopySize);
            EpilogToBuf            (pOldMem, pBuf, PresetOld, pMemSetOld);
            pOldMem-> PutBuf       (pBuf);
            Inner_Mem3_InitBufTail (PoolName, pNewBuf, BufSize, TgtSize, CopySize);
            pNewBuf = Inner_Mem3_UserBuf (PoolName, pNewBuf);
            RecognizeBuf (TgtSize, PoolName);
          }
      }

    Mem3_TraceAlloc (pNewBuf);

    return pNewBuf;
  }

void
Mem3free  (void *  pBuf)
  {
    Mem3_TraceFree (pBuf);
    if  (pBuf == NULL)  return;

    cOldGeneralLock  Lock; // lock clean

    int  PoolName = cMemPool::GetName (pBuf);
    MEM3_USER_ASSERT (PoolName >= 0);

    cMemPoolPtrBusyLock  pPool (PoolName);
    MEM3_USER_ASSERT (pPool != NULL);

    pBuf = Inner_SysBuf (PoolName, pBuf);

    if  (pPool != NULL)
      {
        EpilogToBuf (pPool, pBuf, cMemPool::GetInitVal (PoolName), cMemPool::GetMemSetVal (PoolName));
        pPool-> PutBuf (pBuf);
      }
  }

int
GetPoolName (void *  pBuf)
  {
    return cMemPool::GetName (pBuf);
  }

void *
GetBufBase (void *  pLook)
  {
    cOldGeneralLock  Lock; // lock clean

    int     PoolIdx = cMemPool::GetName (pLook);
    if  (PoolIdx <  0)  return NULL;

    cMemPoolPtrBusyLock  pPool (PoolIdx);
    MEM3_USER_ASSERT (pPool != NULL);

    void * const  pBase = pPool-> GetBufBase (pLook);

    return pBase != NULL ? Inner_Mem3_UserBuf (PoolIdx, pBase) : NULL;
  }

#ifdef  MEM3_SELECTIVE_OVERWRITE

ORDHW
PoolHelperSize2 (ORDHW  TargetSize, ORDHW  GranularitySize, eMemType  MType, bool  OvrWr)
  {
    if  (MType <        0)                                  return 0;
    if  (MType >= (int) MEM3_ARR_LEN (Mem3_Strategy_Hook))  return 0;

    return Mem3_Strategy_Hook [MType] .fHelperSize (TargetSize, GranularitySize, OvrWr, MEM3_ALIGN);
  }

ORDHW
PoolGranSize2 (ORDHW  GranularitySize, eMemType  MType, bool  OvrWr)
  {
    if  (MType <        0)                                  return 0;
    if  (MType >= (int) MEM3_ARR_LEN (Mem3_Strategy_Hook))  return 0;

    return Mem3_Strategy_Hook [MType] .fGranSize (GranularitySize, OvrWr, MEM3_ALIGN);
  }

void
SetCheckOverwrite (int  PoolName, bool  OvrWr)
  {
    cMemPool::sPoolAdmin. SetOverwrite (PoolName, OvrWr);
  }

#ifdef  MEM3_SELECTIVE_ALIGNMENT

ORDHW
PoolHelperSize3 (ORDHW  TargetSize, ORDHW  GranularitySize, eMemType  MType, bool  OvrWr, ORDHW  Alignment)
  {
    if  (MType <        0)                                  return 0;
    if  (MType >= (int) MEM3_ARR_LEN (Mem3_Strategy_Hook))  return 0;

    return Mem3_Strategy_Hook [MType] .fHelperSize (TargetSize, GranularitySize, OvrWr, Alignment);
  }

ORDHW
PoolGranSize3 (ORDHW  GranularitySize, eMemType  MType, bool  OvrWr, ORDHW  Alignment)
  {
    if  (MType <        0)                                  return 0;
    if  (MType >= (int) MEM3_ARR_LEN (Mem3_Strategy_Hook))  return 0;

    return Mem3_Strategy_Hook [MType] .fGranSize (GranularitySize, OvrWr, Alignment);
  }

void
SetPoolAlignment (int  PoolName, ORDHW  Alignment)
  {
    cMemPool::sPoolAdmin. SetAlignment (PoolName, Alignment);
  }

#endif /* ! MEM3_SELECTIVE_ALIGNMENT */
#endif /* ! MEM3_SELECTIVE_OVERWRITE */

ORDHW
PoolHelperSize    (ORDHW  TargetSize, ORDHW  GranularitySize, eMemType  MType)
  {
    if  (MType <        0)                                  return 0;
    if  (MType >= (int) MEM3_ARR_LEN (Mem3_Strategy_Hook))  return 0;

    bool const  OvrWr =
    #ifdef  MEM3_CHECK_OVERWRITE
      true
    #else
      false
    #endif
    ;

    return Mem3_Strategy_Hook [MType] .fHelperSize (TargetSize, GranularitySize, OvrWr, MEM3_ALIGN);
  }

ORDHW
PoolGranSize  (ORDHW  GranularitySize, eMemType  MType)
  {
    if  (MType <        0)                                  return 0;
    if  (MType >= (int) MEM3_ARR_LEN (Mem3_Strategy_Hook))  return 0;

    bool const  OvrWr =
    #ifdef  MEM3_CHECK_OVERWRITE
      true
    #else
      false
    #endif
    ;

    return Mem3_Strategy_Hook [MType] .fGranSize (GranularitySize, OvrWr, MEM3_ALIGN);
  }

int
CreatePool (void  **    ppMemTarget,
            void  **    ppMemHelper,
            ORDHW       TargetSize,
            ORDHW       HelperSize,
            ORDHW       GranularitySize,
            ORDHW       FftThreshold,
            eMemType    MType,
            int         PoolName
           )
  {
    cMemPool *  pPool;
    void     *  pMemTargetTmp = * ppMemTarget;
    ORDHW       TargetSizeTmp = TargetSize;

    cOldGeneralLock  Lock; // lock clean

    AdjustToPool (& pMemTargetTmp,
                  & TargetSizeTmp,
                  InnerBufHeadSize (PoolName),
                  cMemPool::sPoolAdmin. GetAlignment (PoolName)
                 );

    pPool = cMemPool::Create (pMemTargetTmp,
                              * ppMemHelper,
                              TargetSizeTmp,
                              HelperSize,
                              GranularitySize,
                              FftThreshold,
                              MType,
                              PoolName
                             );

    if  (pPool != NULL)
      {
        PrologToPool (* ppMemTarget, TargetSize, cMemPool::GetInitVal (PoolName), cMemPool::GetMemSetVal (PoolName));
        * ppMemTarget = pMemTargetTmp;
        pPool-> FreeDelete ();
      }
    else
      { PoolName = -1; }

    return PoolName;
  }

int
DeletePool (int  PoolName)
  {
    cOldGeneralLock  Lock; // lock clean

    SetMem3StatisticBuf (NULL, 0, PoolName);
    return cMemPool::Delete (PoolName);
  }

void
SetPresetVal (int  PoolName, BYTE  PresetVal)
  {
    cMemPool::  SetPresetVal (PoolName, PresetVal);
  }

void
SetMemSetFunc (int  PoolName, pMemSet_ft  pMemSet_f)
  {
    cMemPool::  SetMemSetFunc (PoolName, pMemSet_f);
  }

void
SetAssignUintFunc (int  PoolName, pPoolAssignUint_ft  pPoolAssignUint_f)
  {
    cMemPool::  SetAssignUintFunc (PoolName, pPoolAssignUint_f);
  }

void
SetGetUintFunc (int  PoolName, pPoolGetUint_ft  pPoolGetUint_f)
  {
    cMemPool::  SetGetUintFunc (PoolName, pPoolGetUint_f);
  }

void
SetAssignUint32_LeMaFunc (int  PoolName, pPoolAssignUint32_LeMa_ft  pPoolAssignUint32_LeMa_f)
  {
    cMemPool::  SetAssignUint32_LeMaFunc (PoolName, pPoolAssignUint32_LeMa_f);
  }

void
SetGetUint32_LeMaFunc (int  PoolName, pPoolGetUint32_LeMa_ft  pPoolGetUint32_LeMa_f)
  {
    cMemPool::  SetGetUint32_LeMaFunc (PoolName, pPoolGetUint32_LeMa_f);
  }

bool
PoolValid (int  PoolName)
  {
    return cMemPool::  PoolValid (PoolName);
  }

int
BufNumUsedInPool  (int  PoolName)
  {
    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ())  return 0;

    int  RetVal = 0;
    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pMem (PoolName);
    if  (pMem != NULL)
      {
        RetVal = pMem-> BufNumUsed ();
      }

    return RetVal;
  }

int
MaxAvailBufInPool (int  PoolName)
  {
    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ())  return 0;

    int  RetVal = 0;
    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pMem (PoolName);
    if  (pMem != NULL)
      {
        RetVal = pMem-> MaxAvailBuf ();
      }

    return RetVal;
  }

int
SumAvailMemInPool (int  PoolName)
  {
    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ())  return 0;

    int  RetVal = 0;
    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pMem (PoolName);
    if  (pMem != NULL)
      {
        RetVal = pMem-> SumAvailMem ();
      }

    return RetVal;
  }

void *
InfoNextBufUsedInPool (int  PoolName, void *  pLook1st)
  {
    void *  pRetVal = NULL;

    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ())  return pRetVal;

    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pMem (PoolName);
    if  (pMem != NULL)
      {
        pRetVal = pMem-> InfoNextBufUsed (pLook1st);
      }

    return pRetVal;
  }

ORDHW
GetPoolPhysSize (int  PoolName)
  {
    ORDHW  pRetVal = 0;

    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ())  return pRetVal;

    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pMem (PoolName);
    if  (pMem != NULL)
      {
        pRetVal = pMem-> GetPoolPhysSize ();
      }

    return pRetVal;
  }

int
PhysGranularity   (int  PoolName)
  {
    int  RetVal = -1;

    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ())  return RetVal;

    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pMem (PoolName);
    if  (pMem != NULL)
      {
        RetVal = pMem-> PhysGranularity ();
      }

    return RetVal;
  }

int
PhysThreshold     (int  PoolName)
  {
    int  RetVal = -1;

    if  (ORDHW (PoolName & MEM3_AliasMask) >= cMemPool::GetPoolNum ())  return RetVal;

    cOldGeneralLock  Lock; // lock clean

    cMemPoolPtrBusyLock  pMem (PoolName);
    if  (pMem != NULL)
      {
        RetVal = pMem-> PhysThreshold ();
      }

    return RetVal;
  }

int
InfoBufferSize (int  PoolName, void *  pBuf)
  {
    int  RetVal = -1;

    if  (ORDHW (PoolName & MEM3_AliasMask) <  cMemPool::GetPoolNum ())
      {
        cOldGeneralLock  Lock; // lock clean

        cMemPoolPtrBusyLock  pMem (PoolName);
        if  (pMem != NULL)
          {
            RetVal = pMem-> GetBufSize (pBuf);
          }
      }
    MEM3_USER_ASSERT (RetVal >= 0);
    return RetVal;
  }

void
SetPoolLockFct (int  PoolName, fpLockUlck  Lock, fpLockUlck  Ulck, void *  pUser)
  {
    cMemPool::sPoolAdmin. SetPoolLock (PoolName, Lock, Ulck, pUser);
  }

void
ResPoolLock    (int  PoolName)
  {
    cMemPool::sPoolAdmin. SetPoolLock (PoolName);
  }

} // extern "C"

void
SetPoolLock    (int  PoolName, cMem3PoolLoUl &  LockUlck)
  {
    cMemPool::sPoolAdmin. SetPoolLock (PoolName, LockUlck);
  }

int
PoolAliasId (int  PoolConstructId)
  {
    return PoolConstructId + MEM3_AliasIdx;
  }

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/