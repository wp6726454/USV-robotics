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
/*  F i l e               &F: memifdef.cpp                              :F&  */
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

void *  FLD
operator new (size_t  BufSize, int  PoolName) EMPTY_THROW
  {
    return Mem3palloc (BufSize, PoolName);
  }

void   FLD
operator delete (void *  pBuf, int)
  {
    if  (pBuf != NULL)  Mem3free (pBuf);
  }

#ifdef  STD_ARRAY_NEW_HANDLING
    void * FLD
    operator new    [] (size_t  BufSize, int  PoolName) EMPTY_THROW
      {
        return operator new (BufSize, PoolName);
      }

    void   FLD
    operator delete [] (void *  pBuf, int)
      {
        operator delete [] (pBuf);
      }

#endif

#ifdef  LOCAL_NEWDEL
#ifndef STD_ARRAY_NEW_HANDLING
    /* Another new and delete may coexist. We do inlining (MEM3NEW.INL). At this point we reprogram
       the array-new and array-delete.
    */

    struct cDynArray
      {
        int  TgtSize;
        int  TgtNum;
      };

    void *
    nArrayBase::  CtorN (int      PoolName,
                         size_t   TgtSize,
                         int      TgtNum,
                         void (*  TgtCtor) (void *)
                        )
      {
        union
          {
            cDynArray *  Arr;
            ORD08     *  Byte;
            void      *  Param;
          }  p;

        p.Param = (cDynArray *) Mem3palloc (sizeof (cDynArray) + TgtSize * TgtNum, PoolName);

        if  (p.Param == NULL)  return p.Param;

        p.Arr-> TgtSize = TgtSize;
        p.Arr-> TgtNum  = TgtNum;
        p.Arr ++;

        int i = 0;
        int j = 0;
        while  (i < TgtNum)
          {
            TgtCtor (& p.Byte [j]);
            i ++;
            j += TgtSize;
          }

        return p.Param;
      }

    void
    nArrayBase::  DtorN (void  *  pTgt,
                         void (*  TgtDtor) (void *)
                        )
      {
        if  (pTgt == NULL)  return;

        size_t   TgtSize;
        int      TgtNum;
        union
          {
            cDynArray *  Arr;
            ORD08     *  Byte;
            void      *  Param;
          }  p;

        p.Param = pTgt;
        p.Arr --;

        TgtSize = p.Arr-> TgtSize;
        TgtNum  = p.Arr-> TgtNum;

        int i = TgtNum  - 1;
        int j = TgtSize * i + sizeof (cDynArray);

        while  (i >= 0)
          {
            TgtDtor (& p.Byte [j]);
            i --;
            j -= TgtSize;
          }

        Mem3free (p.Param);
      }

    void
    nArrayBase::  Dtor1 (void  *  pTgt,
                         void (*  TgtDtor) (void *)
                        )
      {
        if  (pTgt == NULL)  return;

        TgtDtor  (pTgt);
        Mem3free (pTgt);
      }
#endif
#else
    /* We realize new and delete used from the system.
    */

    void *  FLD
    operator new  (size_t  BufSize MEMORY3_NOTHROW_PARM_LIST) EMPTY_THROW
      {
        return operator new (BufSize, STD_POOL_NUM ());
      }

    void    FLD
    operator delete (void *  pBuf MEMORY3_NOTHROW_PARM_LIST)
      {
        if  (pBuf != NULL)  Mem3free (pBuf);
      }

    #ifndef MEMORY3_NOTHROW
        void *  FLD
        operator new  (size_t  BufSize)
          {
            void *  pTmp =  operator new (BufSize);
            if  (pTmp == NULL)  throw bad_alloc;

            return pTmp;
          }

        void    FLD
        operator delete (void *  pBuf)
          {
            operator delete (pBuf);
          }
    #endif

    #ifdef  STD_ARRAY_NEW_HANDLING
        void *  FLD
        operator new [] (size_t  BufSize MEMORY3_NOTHROW_PARM_LIST) EMPTY_THROW
          {
            return operator new (BufSize MEMORY3_NOTHROW_ADD_PARM);
          }

        void    FLD
        operator delete [] (void *  pBuf MEMORY3_NOTHROW_PARM_LIST)
          {
            operator delete (pBuf);
          }

        #ifndef MEMORY3_NOTHROW

            void *  FLD
            operator new [] (size_t  BufSize)
              {
                void *  pTmp =  operator new [] (BufSize MEMORY3_NOTHROW_ADD_PARM);
                if  (pTmp == NULL)  throw bad_alloc;

                return pTmp;
              }

            void    FLD
            operator delete [] (void *  pBuf)
              {
                operator delete [] (pBuf);
              }

        #endif
    #endif

    extern "C" {

    void *
    malloc (size_t  BufSize)
      {
        return Mem3palloc (BufSize, 0);
      }

    void *
    realloc (void *  pBuf, size_t  BufSize)
      {
        return Mem3pralloc (pBuf, BufSize, 0);
      }

    } // extern "C"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/