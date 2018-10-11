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
/*  F i l e               &F: mem3pool.hpp                              :F&  */
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

#ifndef MEM3POOL_HPP
#define MEM3POOL_HPP

#include "mem3if.hpp"

#ifdef  MEM3_SELECTIVE_LOCK
  #include "mem3lockcfg.hpp"
#else
  typedef void  (* fpLockUlck) (void *);

  #ifdef  __cplusplus
    extern "C" {
  #endif /* __cplusplus */

  void     SetPoolLockFct        (int  PoolName, fpLockUlck  Lock, fpLockUlck  Ulck, void *  pUser);
  void     ResPoolLock           (int  PoolName);

  #ifdef  __cplusplus
    } // extern "C"
  #endif /* __cplusplus */
#endif

#include "mem3pooldir.hpp"
#include "pooltype.hpp"

#ifndef MEM3_SELECTIVE_LOCK
#endif /* MEM3_SELECTIVE_LOCK */

struct MEMORY3_INTERN  // 'struct' because 2.7.2 supports no namespaces
  {
    struct CHECK_OVERWRITE  // 'struct' because 2.7.2 supports no namespaces
      {
        static  bool    VerifyMem3Buf    (int  PoolName, void *  pBuf);
        static  void    Mem3_InitBuf     (int  PoolName, void *  pBuf,    size_t  LogSize, ORDHW  PhySize);
        static  void    Mem3_InvalOvwr   (void *  pSysBuf, BYTE    Preset, pMemSet_ft  pMemSet_f);
        static  void    Mem3_InitBufTail (int  PoolName, void *  pBuf,    size_t  LogSize, ORDHW  PhySize, ORDHW  Offset);
        static  void *  SysBuf           (int  PoolName, void *  pUserBuf);
        static  ORDHW   GetBufSize       (int  PoolName, void *  pBuf);
        static  void *  Mem3_UserBuf     (void *  pSysBuf);
      }; // namespace CHECK_OVERWRITE

    struct NOCHK_OVERWRITE  // 'struct' because 2.7.2 supports no namespaces
      {
        static  bool    VerifyMem3Buf    (int  PoolName, void *  pBuf);
        static  void    Mem3_InitBuf     (int  PoolName, void *  pBuf, size_t  LogSize, ORDHW  PhySize);
        static  void    Mem3_InvalOvwr   (void *  pSysBuf, BYTE  Preset, pMemSet_ft  pMemSet_f);
        static  void    Mem3_InitBufTail (int  PoolName, void *  pBuf, size_t  LogSize, ORDHW  PhySize, ORDHW  Offset);
        static  void *  SysBuf           (int  PoolName, void *  pUserBuf);
        static  ORDHW   GetBufSize       (int  PoolName, void *  pBuf);
        static  void *  Mem3_UserBuf     (void *  pSysBuf);
      }; // namespace NOCHK_OVERWRITE

    static  size_t const  BufHeadSize [2];
    static  size_t const  BufTailSize [2];
    static  size_t const  AddBufSize  [2];

    static  bool    (* pfVerifyMem3Buf    [2]) (int, void *);
    static  void    (* pfMem3_InitBuf     [2]) (int, void *, size_t, ORDHW);
    static  void    (* pfMem3_InvalOvwr   [2]) (void *, BYTE, pMemSet_ft);
    static  void    (* pfMem3_InitBufTail [2]) (int, void *, size_t, ORDHW, ORDHW);
    static  void *  (* pfSysBuf           [2]) (int, void *);
    static  ORDHW   (* pfGetBufSize       [2]) (int, void *);
    static  void *  (* pfMem3_UserBuf     [2]) (void *);
    static  void       PoolAssignUint          (ORDHW *  pLval, ORDHW  Rval);
    static  ORDHW      PoolGetUint             (ORDHW const *  pRval);
    static  void       PoolAssignUint32_LeMa   (ORD08 *  pLval, ORDHW  Rval);
    static  ORDHW      PoolGetUint32_LeMa      (ORD08 const *  pRval);
    static  ORDHW      Ggt                     (ORDHW  a, ORDHW  b);
    static  ORDHW      MakeAlignment           (ORDHW  Alignment);
  }; // namespace MEMORY3_INTERN

class  cMemPoolPtrBusyLock;

class  cMemPool
  {
  friend void *  Mem3pralloc           (void *, size_t, int);
  friend void *  Mem3palloc                    (size_t, int);
  friend void    Mem3free              (void *);
  friend int     BufNumUsedInPool      (int);
  friend int     MaxAvailBufInPool     (int);
  friend int     SumAvailMemInPool     (int);
  friend void *  GetBufBase            (void *);
  friend ORDHW   GetBufSize            (void *);
  friend int     PhysGranularity       (int);
  friend int     PhysThreshold         (int);
  friend void *  InfoNextBufUsedInPool (int, void *);
  friend ORDHW   GetPoolPhysSize       (int);
  friend int     InfoBufferSize        (int, void *);
  friend void    SetMem3StatisticBuf   (MEM3_STATISTIC *, int, int);
  friend int     CreatePool            (void  **,
                                        void  **,
                                        ORDHW,
                                        ORDHW,
                                        ORDHW,
                                        ORDHW,
                                        eMemType,
                                        int
                                       );
  friend void    SetPoolLockFct        (int, fpLockUlck, fpLockUlck, void *);
  friend void    ResPoolLock           (int);
  friend void    SetPoolLock           (int, cMem3PoolLoUl &);
  friend void    SetCheckOverwrite     (int, bool);
  friend void    SetPoolAlignment      (int, ORDHW);

  friend ORDHW   MEMORY3_INTERN::NOCHK_OVERWRITE::GetBufSize (int, void *);
  friend size_t  InnerBufHeadSize (int);
  friend size_t  InnerBufTailSize (int);
  friend size_t  InnerAddBufSize     (int);
  friend bool    Inner_VerifyMem3Buf (int, void *);
  friend void *  Inner_Mem3_UserBuf  (int, void *);
  friend void    Inner_Mem3_InitBuf     (int, void *, size_t, ORDHW);
  friend void    Inner_Mem3_InitBufTail (int, void *, size_t, ORDHW, ORDHW);
  friend void    Inner_Mem3_InvalOvwr   (int, void *, BYTE);
  friend void *  Inner_SysBuf           (int, void *);

  friend class   cPoolAdmin <POOL_NUM>;

  protected:
    class  cPoolLock
      {
                cMem3PoolLoUl &   mLockUlck;

      public:
                                  cPoolLock (cMemPool const &  MemPool) : mLockUlck (MemPool. mPlLockUlck) { mLockUlck. Lock (); }
                                 ~cPoolLock ()  { mLockUlck. Ulck (); }
      };

  friend class  cPoolLock;
  friend class  cMemPoolPtrBusyLock;

  protected:
    static  cPoolAdmin <POOL_NUM>   sPoolAdmin;

  public:
            LINADDR           const PoolMemAddr;
            ORDHW             const PoolMemSize;

  private:
            cTryLock             &  mDirtyBusy;
            int                     mBufNumUsed;
            cMem3PoolLoUl        &  mPlLockUlck;
            cMemPool             *  mpParent;

  protected:
            bool                    SetDelete          () const { return mDirtyBusy. TryWr (); }
            void                    FreeDelete         () const { mDirtyBusy. GiveWr (); }
            bool                    SetBusy            () const { return mDirtyBusy. TryRd (); }
            void                    FreeBusy           () const { mDirtyBusy. GiveRd (); }
            void                    SetDirty           () const { mDirtyBusy. SetDirty (); }
            void                    SetClean           () const { mDirtyBusy. SetClean (); }
            bool                    GetDirty           () const { return mDirtyBusy. GetDirty (); }

  public:
    static  void  *                 operator new       (size_t    Size,
                                                        ORD32 *&  pPool
                                                       );
    static  void                    operator delete    (void  *,
                                                        ORD32 *&
                                                       );
    static  void                    operator delete    (void  *);


    static  int                     GetName            (void         *   pBuf);
    static  cMemPool *              Create             (void         *&  pMemTarget,
                                                        void         *&  pMemHelper,
                                                        ORDHW            TargetSize,
                                                        ORDHW            HelperSize,
                                                        ORDHW            GranularitySize,
                                                        ORDHW            FftThreshold,
                                                        eMemType         MType,
                                                        int              PoolName
                                                       );
    static  int                     Delete             (int          PoolName);
    static  void                    SetPresetVal       (int          PoolName, BYTE        PresetVal);
    static  void                    SetMemSetFunc             (int  PoolName, pMemSet_ft                 pMemSet_f);
    static  void                    SetAssignUintFunc         (int  PoolName, pPoolAssignUint_ft         pPoolAssignUint_f);
    static  void                    SetGetUintFunc            (int  PoolName, pPoolGetUint_ft            pPoolGetUint_f);
    static  void                    SetAssignUint32_LeMaFunc  (int  PoolName, pPoolAssignUint32_LeMa_ft  pPoolAssignUint32_LeMa_f);
    static  void                    SetGetUint32_LeMaFunc     (int  PoolName, pPoolGetUint32_LeMa_ft     pPoolGetUint32_LeMa_f);
    static  cMemPool *              GetPool            (int          PoolName);
    static  cMemPool *              GetPoolBusy        (int          PoolName);
    static  bool                    PoolValid          (int          PoolName);
    static  BYTE                    GetInitVal         (int          PoolName);
    static  pMemSet_ft                  GetMemSetVal        (int  PoolName);
    static  pPoolAssignUint_ft          GetAssignUintVal    (int  PoolName);
    static  pPoolGetUint_ft             GetGetUintVal       (int  PoolName);
    static  pPoolAssignUint32_LeMa_ft   GetAssignUint32Val  (int  PoolName);
    static  pPoolGetUint32_LeMa_ft      GetGetUint32Val     (int  PoolName);
    static  ORDHW                   GetPoolNum         ();

    // Use PoolLock independendly on pool type.
            void *                  GetBuf             (size_t, size_t &);
            void                    PutBuf             (void *);
            int                     GetBufSize         (void *) const;
            void *                  GetBufBase         (void *) const;

    // No lock.
            int                     BufNumUsed         () const { return mBufNumUsed; }

    // May use PoolLock dependendly on pool type.
    virtual void                    TryBuf             (void *, size_t, size_t &)     = 0;
    virtual void                    ShortBuf           (void *, size_t, size_t &)     = 0;
    virtual int                     MaxAvailBuf        ()                       const = 0;
    virtual int                     SumAvailMem        ()                       const = 0;
    virtual void *                  InfoNextBufUsed    (void  *      pLook1st)  const = 0;

    // No lock.
    virtual int                     PhysGranularity    ()                       const = 0;
    virtual int                     PhysThreshold      ()                       const = 0;
    virtual cMemPool *              GetAliasPool       ();
    virtual bool                    LogicalInside      (void *) const;

            ORDHW                   GetPoolPhysSize    ();

  protected:
    virtual void *                  GetBufLocked       (size_t, size_t &)             = 0;
    virtual void                    PutBufLocked       (void *)                       = 0;
    virtual int                     GetBufSizeLocked   (void *) const                 = 0;
    virtual void *                  GetBufBaseLocked   (void *) const                 = 0;

                                    cMemPool           (LINADDR, ORDHW, cMem3PoolLoUl &, cTryLock &, cMemPool *) EMPTY_THROW;
    virtual                        ~cMemPool           ();
  };

class  cMemPoolPtrBusyLock
  {
    cMemPool *  mpPool;

  public:
                cMemPoolPtrBusyLock (int  PoolIdx)
                  : mpPool (ORDHW (PoolIdx & MEM3_AliasMask) < cMemPool::GetPoolNum ()
                              ? cMemPool::GetPoolBusy (PoolIdx)
                              : NULL
                           )
                  {
                  }
               ~cMemPoolPtrBusyLock ()
                  {
                    if  (mpPool != NULL)  mpPool-> FreeBusy ();
                  }
                operator cMemPool * () { return mpPool; }
    cMemPool &  operator *  () { return * mpPool; }
    cMemPool *  operator -> () { return   mpPool; }
    bool        operator == (cMemPool *  pP) const { return mpPool == pP; }
    bool        operator != (cMemPool *  pP) const { return mpPool != pP; }
  };

class  cMemPoolInst: public cMemPool
  {
            cTryLock mutable        mDirtyBusy;

  protected:

                                    cMemPoolInst       (LINADDR, ORDHW, cMem3PoolLoUl &, cMemPool *) EMPTY_THROW;
  };

/********************************* start of inling *********************************/

#include "mem3pooldir_tpl.hpp"

inline
ORDHW
MEMORY3_INTERN::Ggt (ORDHW  a, ORDHW  b)
  {
    ORDHW  D = b;
    ORDHW  N = a;

    while  (D >  0)
      {
        ORDHW  R;

        R = N % D;
        N = D;
        D = R;
      }

    return N;
  }

inline
ORDHW
MEMORY3_INTERN::MakeAlignment (ORDHW  Alignment)
  {
    /* we       adjust to our internal alignment size (ORDHW)
       the user adjusts to the user's  alignment MEM3_ALIGN
    */
    return sizeof (ORDHW) * Alignment / Ggt (sizeof (ORDHW), Alignment);
  }

inline
ORDHW
cMemPool::  GetPoolPhysSize ()
  {
    return PoolMemSize;
  }

inline
void
cMemPool::  operator delete    (void  *, ORD32 *&)
  {
    /* do nothing */
  }

inline
void
cMemPool::  operator delete    (void  *  pRet)
  {
    ::operator delete (pRet);
  }

inline
int
cMemPool::  GetName (void *  pBuf)
  {
    return sPoolAdmin. GetName (pBuf);
  }

inline
cMemPool *
cMemPool::  Create (void         *&  pMemTarget,
                    void         *&  pMemHelper,
                    ORDHW            TargetSize,
                    ORDHW            HelperSize,
                    ORDHW            GranularitySize,
                    ORDHW            FftThreshold,
                    eMemType         MType,
                    int              PoolName
                   )
  {
    return sPoolAdmin. Create (pMemTarget,
                               pMemHelper,
                               TargetSize,
                               HelperSize,
                               GranularitySize,
                               FftThreshold,
                               MType,
                               PoolName
                              );
  }

inline
int
cMemPool::  Delete (int  PoolName)
  {
    return sPoolAdmin. Delete (PoolName);
  }

inline
void
cMemPool::  SetPresetVal (int  PoolName, BYTE  PresetVal)
  {
    sPoolAdmin. SetPresetVal (PoolName, PresetVal);
  }

inline
void
cMemPool::  SetMemSetFunc (int  PoolName, pMemSet_ft  pMemSet_f)
  {
    sPoolAdmin. SetMemSetFunc (PoolName, pMemSet_f);
  }

inline
void
cMemPool::  SetAssignUintFunc (int  PoolName, pPoolAssignUint_ft  pPoolAssignUint_f)
  {
    sPoolAdmin. SetAssignUintFunc (PoolName, pPoolAssignUint_f);
  }

inline
void
cMemPool::  SetGetUintFunc (int  PoolName, pPoolGetUint_ft  pPoolGetUint_f)
  {
    sPoolAdmin. SetGetUintFunc (PoolName, pPoolGetUint_f);
  }

inline
void
cMemPool::  SetAssignUint32_LeMaFunc (int  PoolName, pPoolAssignUint32_LeMa_ft  pPoolAssignUint32_LeMa_f)
  {
    sPoolAdmin. SetAssignUint32_LeMaFunc (PoolName, pPoolAssignUint32_LeMa_f);
  }

inline
void
cMemPool::  SetGetUint32_LeMaFunc (int  PoolName, pPoolGetUint32_LeMa_ft  pPoolGetUint32_LeMa_f)
  {
    sPoolAdmin. SetGetUint32_LeMaFunc (PoolName, pPoolGetUint32_LeMa_f);
  }

inline
cMemPool *
cMemPool::  GetPool (int  PoolName)
  {
    return sPoolAdmin. GetPool (PoolName);
  }

inline
bool
cMemPool::  PoolValid (int  PoolName)
  {
    return sPoolAdmin. PoolValid (PoolName);
  }

inline
BYTE
cMemPool::  GetInitVal (int  PoolName)
  {
    return sPoolAdmin. GetInitVal (PoolName);
  }

inline
pMemSet_ft
cMemPool::  GetMemSetVal (int  PoolName)
  {
    return sPoolAdmin. GetMemSetVal (PoolName);
  }

inline
pPoolAssignUint_ft
cMemPool::  GetAssignUintVal (int  PoolName)
  {
    return sPoolAdmin. GetAssignUintVal (PoolName);
  }

inline
pPoolGetUint_ft
cMemPool::  GetGetUintVal (int  PoolName)
  {
    return sPoolAdmin. GetGetUintVal (PoolName);
  }

inline
pPoolAssignUint32_LeMa_ft
cMemPool::  GetAssignUint32Val (int  PoolName)
  {
    return sPoolAdmin. GetAssignUint32Val (PoolName);
  }

inline
pPoolGetUint32_LeMa_ft
cMemPool::  GetGetUint32Val (int  PoolName)
  {
    return sPoolAdmin. GetGetUint32Val (PoolName);
  }

inline
ORDHW
cMemPool::  GetPoolNum ()
  {
    return POOL_NUM;
  }

inline
size_t
OvrWrBufHeadSize (bool  OvrWr)
  {
    return MEMORY3_INTERN::BufHeadSize [OvrWr];
  }

inline
size_t
OvrWrBufTailSize (bool  OvrWr)
  {
    return MEMORY3_INTERN::BufTailSize [OvrWr];
  }

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
