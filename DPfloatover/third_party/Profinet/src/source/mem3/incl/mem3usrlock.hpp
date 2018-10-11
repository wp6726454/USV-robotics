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
/*  F i l e               &F: mem3usrlock.hpp                           :F&  */
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

#ifndef MEM3USRLOCK_HPP
#define MEM3USRLOCK_HPP

#ifndef MEM3_SELECTIVE_LOCK

class  cMem3MicroLock
  {
  public:
                    cMem3MicroLock () {}
                   ~cMem3MicroLock () {}
  };

typedef cMem3PoolLoUl  cMem3PoolDfLoUl;

class  cMem3AdminLoUl
  {
  public:
            void    Lock           () {}
            void    Ulck           () {}
  };

class  cTryLock
  {
  public:
            bool    TryRd          () { return true; }
            bool    TryWr          () { return true; }
            void    GiveRd         () {}
            void    GiveWr         () {}
            void    SetDirty       () {}
            void    SetClean       () {}
            bool    GetDirty       () { return false; }
  };

class  cOldGeneralLock
  {
  public:
            cOldGeneralLock () { Mem3Lock (); }
           ~cOldGeneralLock () { Mem3Ulck (); }
  };

#else

class  cTryLock
  {
            int     mUnits;
            bool    mDirty;

  private:
            bool    TryN           (int);
            void    GiveN          (int);

  public:
                    cTryLock       () : mUnits (0), mDirty (false) {}
            bool    TryRd          () { return TryN (1);       }
            bool    TryWr          () { return TryN (MAX_INT); }
            void    GiveRd         () { GiveN (1);       }
            void    GiveWr         () { GiveN (MAX_INT); }
            void    SetDirty       () { mDirty = true;  }
            void    SetClean       () { mDirty = false; }
            bool    GetDirty       () { return mDirty; }
  };

class  cOldGeneralLock
  {
  public:
            cOldGeneralLock () {}
           ~cOldGeneralLock () {}
  };

#ifdef MEM3_INITIAL_SOURCE_MODUL

bool
cTryLock::  TryN (int  nUnits)
  {
    cMem3MicroLock  Lock;

    mUnits    -= nUnits;
    int
    WrapAround = mUnits >> (sizeof (int) * 8 - 1); // -1 or 0
    mUnits    += WrapAround & nUnits;

    return bool (WrapAround + 1);
  }

void
cTryLock::  GiveN (int  nUnits)
  {
    cMem3MicroLock  Lock;

    mUnits    += nUnits;
  }

#endif

#endif

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
