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
/*  F i l e               &F: mem3if.hpp                                :F&  */
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

#ifndef MEM3IF_HPP
#define MEM3IF_HPP

#include "mem3if.h"

#ifndef EMPTY_THROW
  #define EMPTY_THROW
  #define MEMORY3_NOTHROW
#endif

#ifndef MEMORY3_NOTHROW
  #define  MEMORY3_NOTHROW_VAL         std::nothrow
  #define  MEMORY3_NOTHROW_ADD_PARM    , MEMORY3_NOTHROW_VAL
  #define  MEMORY3_NOTHROW_PLACEMENT   (MEMORY3_NOTHROW_VAL)
  #define  MEMORY3_NOTHROW_PARM_LIST   , std::nothrow_t const&
#else
  #define  MEMORY3_NOTHROW_VAL
  #define  MEMORY3_NOTHROW_ADD_PARM
  #define  MEMORY3_NOTHROW_PLACEMENT
  #define  MEMORY3_NOTHROW_PARM_LIST
#endif

void * FLD operator new       (size_t, int) EMPTY_THROW;
void   FLD operator delete    (void *, int);

#ifdef STD_ARRAY_NEW_HANDLING
  void * FLD operator new    [] (size_t, int) EMPTY_THROW;
  void   FLD operator delete [] (void *, int);
#endif

#ifdef LOCAL_NEWDEL

    class  nArrayBase
      {
      protected:
        static  void *  CtorN   (int      PoolName,
                                 size_t   TgtSize,
                                 int      TgtNum,
                                 void (*  TgtCtor) (void *)
                                );
        static  void    DtorN   (void  *  pTgt,
                                 void (*  TgtDtor) (void *)
                                );
        static  void    Dtor1   (void  *  pTgt,
                                 void (*  TgtDtor) (void *)
                                );
      };

    template <class T>
    class  nArray: public nArrayBase
      {
        static  void    Ctor    (void *  pT);
        static  void    Dtor    (void *  pT);

      public:
        static  T *     ArrNew  (int  Num, int  PoolName);
        static  void    ArrDel  (T *  pTgt);
        static  void    Delete  (T *  pTgt);
      };

    template <class T>
    T *
    nArray <T>::  ArrNew (int  Num, int  PoolName)
      {
        return (T *) CtorN (PoolName, sizeof (T), Num, Ctor);
      }

    template <class T>
    void
    nArray <T>::  ArrDel  (T *  pTgt)
      {
        DtorN (pTgt, Dtor);
      }

    template <class T>
    void
    nArray <T>::  Delete  (T *  pTgt)
      {
        Dtor1 (pTgt, Dtor);
      }

    template <class T>
    void
    nArray <T>::  Ctor (void *  pT)
      {
        new (pT) T;
      }

    template <class T>
    void
    nArray <T>::  Dtor (void *  pT)
      {
        ((T *) pT)-> ~T ();
      }

    template <class T>
    inline
    void
    Mem3ArrDel  (T *  pTgt)
      {
        nArray <T>::  ArrDel  (pTgt);
      }

    template <class T>
    inline
    void
    Mem3Delete  (T *  pTgt)
      {
        nArray <T>::  Delete  (pTgt);
      }

#else
  #ifndef __NEW__
    #error File "new" must be included within "mem3cfg.h"
  #endif
#endif

#ifdef LOCAL_NEWDEL
  #define  NEW                           new                  (STD_POOL_NUM ())
  #define  MEM3DELETE( pTgt )            Mem3Delete           (pTgt)
  #define  NEWARR( PoolName, Type, Num ) nArray<Type>::ArrNew (Num, PoolName)
  #define  DELARR( pTgt )                Mem3ArrDel           (pTgt)
#else
  #define  NEW                           new    MEMORY3_NOTHROW_PLACEMENT
  #define  MEM3DELETE( pTgt )            delete            pTgt
  #define  NEWARR( PoolName, Type, Num ) new    (PoolName) Type [Num]
  #define  DELARR( pTgt )                delete []         pTgt
#endif

template <class T>  T *  Mem3Complete (T *);

/******************* inlining starts here ***********************/

template <class T>
T *
Mem3Complete (T *  pT)
  {
    if  (pT != NULL)
      {
        if  (! pT-> Complete ())
          {
            MEM3DELETE (pT);
            pT = NULL;
          }
      }

    return pT;
  }

#endif


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
