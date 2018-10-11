#ifndef PNIO_VER_H
#define PNIO_VER_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: PROFINET IO Runtime Software              :C&  */
/*                                                                           */
/*  F i l e               &F: pnio_version.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Provides the version of the actual used PNIO stack.                      */
/*                                                                           */
/*****************************************************************************/
#ifdef PNIO_MESSAGE
/*-----------------------------------------------------------------------------
// History:
//		new: upd: ren: del:
//   2009-07-02: Conversion to Visual Studio 2008, correct LSA_COMP_ID set
//   2009-11-10: Correction of wrong PNIO_VERSION treatment,
//               change default values of defines
//---------------------------------------------------------------------------*/
#endif /* PNIO_MESSAGE */


/*===========================================================================*/
/*            version according to "KM-Prozessbeschreibung"                  */
/*===========================================================================*/

/* "&T","&V","&D" and so on may be used by configuration management */


#define PNIO_LSA_COMPONENT_ID    LSA_COMP_ID_PNIO
                                   /* see lsa_sys.h: lsa_component_id */
#define PNIO_LSA_PREFIX          {'B','C','_','P','N','R','U','N','\0','\0'}

#define PNIO_KIND                /* &PK */ 'P'  /* PK& */
                                   /* preliminary: 'R': release       */
                                   /*              'C': correction    */
                                   /*              'S': spezial       */
                                   /*              'T': test          */
                                   /*              'B': labor         */
                                   /* prereleased: 'P': pilot         */
                                   /* released:    'V': version       */
                                   /*              'K': correction    */
                                   /*              'D': demonstration */
#define PNIO_VERSION             /* &PV */ 5     /* PV& */ /* [1 - 99] */
#define PNIO_DISTRIBUTION        /* &PD */ 4     /* PD& */ /* [0 - 99] */
#define PNIO_FIX                 /* &PF */ 0     /* PF& */ /* [0 - 99] */
#define PNIO_HOTFIX              /* &PH */ 0     /* PH& */ /* [0 - 99] */
#define PNIO_PROJECT_NUMBER      /* &PP */ 0     /* PP& */ /* [0 - 99] */
                                                 /* At LSA always 0!   */
#define PNIO_INCREMENT           /* &PI */ 2     /* PI& */ /* [1 - 99] */
#define PNIO_INTEGRATION_COUNTER /* &PC */ 0     /* PC& */ /* [1 - 99] */
#define PNIO_GEN_COUNTER         /* &PG */ 40    /* PG& */ /* [1 - 99] */


LSA_UINT16 pnio_version( LSA_UINT16 length, LSA_VERSION_TYPE *version);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNIO_VER_H */
