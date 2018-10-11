
#ifndef PARS_VER_H                     /* ----- reinclude-protection ----- */
#define PARS_VER_H

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: PARS_Version.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Version information for the xml parser                                   */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*            version according to "KM-Prozessbeschreibung"                  */
/*===========================================================================*/

/* "&K","&V","&D" and so on may be used by configuration management
 */

#define PARS_COMPONENT_ID          COMP_ID_PARS
                                   /* see sys.h: component_id */
#define PARS_PREFIX                {'M','W','E','B','_','C','O','R','E','\0'}
/* example:
#define S7L4_PREFIX                {'L','S','A','_','S','7','L','4','\0'}
*/
#define PARS_KIND                  /* &K */ 'P'  /* K& */
                                   /* preliminary: 'R': release       */
                                   /*              'C': correction    */
                                   /*              'S': spezial       */
                                   /*              'T': test          */
                                   /*              'B': labor         */
                                   /* prereleased: 'P': pilot         */
                                   /* released:    'V': version       */
                                   /*              'K': correction    */
                                   /*              'D': demonstration */
#define PARS_VERSION               /* &V */ 4    /* V& */ /* [1 - 99] */
#define PARS_DISTRIBUTION          /* &D */ 3    /* D& */ /* [0 - 99] */
#define PARS_FIX                   /* &F */ 3    /* F& */ /* [0 - 99] */
#define PARS_HOTFIX                /* &H */ 0    /* H& */ /* [0]      */
#define PARS_PROJECT_NUMBER        /* &P */ 0    /* P& */ /* [0 - 99] */
                                                 /* At LSA always 0!  */
#define PARS_INCREMENT             /* &I */ 1    /* I& */ /* [1 - 99] */
#define PARS_INTEGRATION_COUNTER   /* &C */ 1    /* C& */ /* [1 - 99] */
#define PARS_GEN_COUNTER           /* &G */ 2    /* G& */ /* [1]      */

#define PARS_FULL_VERSION          (((PARS_VERSION&0xff)<<24) |          \
                                   ((PARS_DISTRIBUTION&0xff)<<16) |     \
                                   ((PARS_FIX&0xff)          << 8 ) |   \
                                   ((PARS_HOTFIX&0xff )))

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
#endif  /* of PARS_VER_H */

