#ifndef PND_VER_H
#define PND_VER_H

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
/*  F i l e               &F: PND_version.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Provides the version of the actual used PND stack.                      */
/*                                                                           */
/*****************************************************************************/
#ifdef PND_MESSAGE
/*-----------------------------------------------------------------------------
// History:
//		new: upd: ren: del:
//---------------------------------------------------------------------------*/
#endif /* PND_MESSAGE */


/*===========================================================================*/
/*            version according to "KM-Prozessbeschreibung"                  */
/*===========================================================================*/

/* "&T","&V","&D" and so on may be used by configuration management */


#define PND_LSA_COMPONENT_ID    LSA_COMP_ID_PND
                                   /* see lsa_sys.h: lsa_component_id */
#define PND_LSA_PREFIX          {'L','S','A','_','P','N','D','\0','\0','\0'}

#define PND_KIND                /* &PK */ 'V'  /* PK& */
                                   /* preliminary: 'R': release       */
                                   /*              'C': correction    */
                                   /*              'S': spezial       */
                                   /*              'T': test          */
                                   /*              'B': labor         */
                                   /* prereleased: 'P': pilot         */
                                   /* released:    'V': version       */
                                   /*              'K': correction    */
                                   /*              'D': demonstration */
#define PND_VERSION             /* &PV */ 1     /* PV& */ /* [1 - 99] */
#define PND_DISTRIBUTION        /* &PD */ 1     /* PD& */ /* [0 - 99] */
#define PND_FIX                 /* &PF */ 0     /* PF& */ /* [0 - 99] */
#define PND_HOTFIX              /* &PH */ 0     /* PH& */ /* [0 - 99] */
#define PND_PROJECT_NUMBER      /* &PP */ 0     /* PP& */ /* [0 - 99] */
                                                 /* At LSA always 0!   */
#define PND_INCREMENT           /* &PI */ 1     /* PI& */ /* [1 - 99] */
#define PND_INTEGRATION_COUNTER /* &PC */ 0     /* PC& */ /* [1 - 99] */
#define PND_GEN_COUNTER         /* &PG */ 16    /* PG& */ /* [1 - 99] */


LSA_UINT16 PND_version( LSA_UINT16 length, LSA_VERSION_TYPE *version);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PND_VER_H */
