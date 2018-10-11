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
/*  F i l e               &F: mem3if.h                                  :F&  */
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

#ifndef MEM3USER_H
#define MEM3USER_H

#include "mem3cfg.h"
#include "mem3defaultmac.h"

#define MEMORY3_VERSION "V04.03.01.00"
#define MEMORY3_DATE    "2013/08/21"

extern
#ifdef  __cplusplus
 "C"
#endif
char const memory3_version [39];

#ifdef  __cplusplus
  #ifdef MEM3_SELECTIVE_LOCK

  class  cMem3PoolLoUl
    {
    public:
      virtual void    Lock           () = 0;
      virtual void    Ulck           () = 0;
      virtual        ~cMem3PoolLoUl  () {}
    };

  #else /* ! MEM3_SELECTIVE_LOCK */

  class  cMem3PoolLoUl
    {
    public:
              void    Lock           () {}
              void    Ulck           () {}
      virtual        ~cMem3PoolLoUl  () {}
    };

  #endif /* ! MEM3_SELECTIVE_LOCK */
#endif /* __cplusplus */

#ifndef MEM3_ALIGN
  #define MEM3_ALIGN 8
#endif

#ifndef MEM3_BUF_PRESET_BYTE
  #define MEM3_BUF_PRESET_BYTE -1
#endif

#define     STOP_LABEL               0x504F5453  // == 'STOP'

#ifdef LOCAL_NEWDEL
  #ifndef STD_POOL_NUM
    #define  STD_POOL_NUM( ) 0
  #endif
#else
  #define  Mem3free                  free

#ifdef  __cplusplus
  extern "C" {
#endif /* __cplusplus */

  void *   realloc (void *  pBuf, size_t  BufSize);
  void *   malloc                (size_t  BufSize);

#ifdef  __cplusplus
  } // extern "C"
#endif /* __cplusplus */

#endif

/* Signatures of some function, that the may overwrite by preprocessor defines or dynamically
   for the use in a distinct pool.
   If the user defines his own (partitial) set of functions, they shall exactly confirm with the
   type denoted below.
   Otherwise it's ill formed.
*/

typedef void *  (* pMemSet_ft)                (void *, int, size_t); /* MEM3_POOL_MEMSET_FUNCTION              */
typedef void    (* pPoolAssignUint_ft)        (ORDHW *, ORDHW);      /* MEM3_POOL_ASSIGN_UINT_FUNCTION         */
typedef ORDHW   (* pPoolGetUint_ft)           (ORDHW const *);       /* MEM3_POOL_GET_UINT_FUNCTION            */
typedef void    (* pPoolAssignUint32_LeMa_ft) (ORD08 *, ORDHW);      /* MEM3_POOL_ASSIGN_UINT32_LE_MA_FUNCTION */
typedef ORDHW   (* pPoolGetUint32_LeMa_ft)    (ORD08 const *);       /* MEM3_POOL_GET_UINT32_LE_MA_FUNCTION    */

/* possible pool types
*/
enum  eMemType { Universal3, FixedRaster, Fft2Bound, OutRangeMemType };

#ifndef __cplusplus
  typedef  enum  eMemType  eMemType;
#endif

struct MEM3_STATISTIC
  {
    int     BufClassSize;
    int     BufAllocationNum;
  };

#ifndef __cplusplus
  typedef  struct MEM3_STATISTIC  MEM3_STATISTIC;
#endif

#ifdef  __cplusplus
  extern "C" {
#endif /* __cplusplus */

void *   Mem3pralloc           (void *    pBuf, size_t  BufSize, int  PoolName);
void *   Mem3palloc                            (size_t  BufSize, int  PoolName);
void     Mem3free              (void *    pBuf);

int      GetPoolName           (void *    pBuf);
int      PoolAliasId           (int       PoolConstructId);

#ifdef  MEM3_SELECTIVE_OVERWRITE

ORDHW    PoolHelperSize2       (ORDHW     TargetSize, ORDHW  GranularitySize, eMemType  MType, bool  OvrWr);
ORDHW    PoolGranSize2                               (ORDHW  GranularitySize, eMemType  MType, bool  OvrWr);
void     SetCheckOverwrite     (int       PoolName,   bool   OvrWr);

#ifdef  MEM3_SELECTIVE_ALIGNMENT

ORDHW    PoolHelperSize3       (ORDHW     TargetSize, ORDHW  GranularitySize, eMemType  MType, bool  OvrWr, ORDHW  Alignment);
ORDHW    PoolGranSize3                               (ORDHW  GranularitySize, eMemType  MType, bool  OvrWr, ORDHW  Alignment);
void     SetPoolAlignment      (int       PoolName,   ORDHW  Alignment);

#endif /* ! MEM3_SELECTIVE_ALIGNMENT */

#endif /* ! MEM3_SELECTIVE_OVERWRITE */

ORDHW    PoolHelperSize        (ORDHW     TargetSize, ORDHW  GranularitySize, eMemType  MType);
ORDHW    PoolGranSize                                (ORDHW  GranularitySize, eMemType  MType);

int      CreatePool            (void **   ppMemTarget,
                                void **   ppMemHelper,
                                ORDHW     TargetSize,
                                ORDHW     HelperSize,
                                ORDHW     GranularitySize,
                                ORDHW     FftThreshold,
                                eMemType  MType,
                                int       PoolName
                               );

bool     VerifyMem3Buf             (void *  pBuf);
void *   Mem3_UserBuf              (void *  pSysBuf);
void *   Mem3_CondionalUserBuf     (void *  pIfaSysBuf);
int      DeletePool                (int     PoolName);
bool     PoolValid                 (int     PoolName);
int      BufNumUsedInPool          (int     PoolName);
int      MaxAvailBufInPool         (int     PoolName);
int      SumAvailMemInPool         (int     PoolName);
ORDHW    GetPoolPhysSize           (int     PoolName);
int      PhysGranularity           (int     PoolName);
int      PhysThreshold             (int     PoolName);
void *   GetBufBase                (void *  pLook);
ORDHW    GetBufSize                (void *  pBuf);
void     SetPresetVal              (int     PoolName, BYTE                       PresetVal);
void     SetMemSetFunc             (int     PoolName, pMemSet_ft                 pMemSet_f);
void     SetAssignUintFunc         (int     PoolName, pPoolAssignUint_ft         pPoolAssignUint_f);
void     SetGetUintFunc            (int     PoolName, pPoolGetUint_ft            pPoolGetUint_f);
void     SetAssignUint32_LeMaFunc  (int     PoolName, pPoolAssignUint32_LeMa_ft  pPoolAssignUint32_LeMa_f);
void     SetGetUint32_LeMaFunc     (int     PoolName, pPoolGetUint32_LeMa_ft     pPoolGetUint32_LeMa_f);

#ifdef MEM3_SELECTIVE_LOCK

typedef void  (* fpLockUlck) (void *);

void     SetPoolLockFct        (int  PoolName, fpLockUlck  Lock, fpLockUlck  Ulck, void *  pUser);
void     ResPoolLock           (int  PoolName);

#endif /* MEM3_SELECTIVE_LOCK */

/* Die naechsten 3 Funktionen arbeiten mit physikalischen statt logischen
   Puffern. Letztere sind inbesondere im 'CHECK_OVERWRITE'-Mode von ersteren
   verschieden.
   Soll ein physikalischer Puffer bearbeitet, z.B. verifiziert werden, so muss
   er zuvor mit 'Mem3_UserBuf' in einen logischen gewandelt werden.
*/
void *   InfoNextBufUsedInPool (int  PoolName, void *  pLook1st);
int      InfoBufferSize        (int  PoolName, void *  pBuf);
void     SetMem3StatisticBuf   (MEM3_STATISTIC *  pStatBuf, int  StatBufLen, int  PoolName);

#ifdef  __cplusplus
  } // extern "C"
#endif /* __cplusplus */

#ifdef  __cplusplus
void     SetPoolLock           (int  PoolName, cMem3PoolLoUl &  LockUlck);
#endif /* __cplusplus */

#endif


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
