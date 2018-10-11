
#ifndef LSA_USR_H                        /* ----- reinclude-protection ----- */
#define LSA_USR_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*  Copyright (C) 2010 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: Layer Stack Architecture (LSA)    :P&          */
/*                                                                           */
/*  P a c k a g e         &W: LSA-Global                        :W&          */ 
/*                                                                           */
/*  C o m p o n e n t     &C: Global Definitions                :C&          */
/*                                                                           */
/*  F i l e               &F: lsa_usr.h                         :F&          */
/*                                                                           */
/*  V e r s i o n         &V: LSA_GLOB_P02.01.00.00_00.13.02.01 :V&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2010-09-16                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface for LSA                                                   */
/*  Defines constants, types and macros for LSA.                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*  2003-05-06  P2.1.0.0_0.2.1.1 ws ARTS+ AP00007688 und AP00007692: RQB-    */
/*                                  Error-Struktur geaendert: Datei          */
/*                                  lsa_usr.h neu; LSA_RQB_LOCAL_ERROR_TYPE  */
/*                                  neu.                                     */
/*  2003-12-08  P2.1.0.0_0.3.1.1 ws ARTS+ AP00007692: RQB-Error-Struktur ge- */
/*                                  aendert: Kommentar ergaenzt.             */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00132068: Fuer C++-Compiler      */
/*                                  Datei mit C-Bindung gekennzeichnet.      */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00167479: LSA_RSP_* neu.         */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00007692: Zur besseren Handhabung*/
/*                                  des LSA_RQB_LOCAL_ERROR_TYPE die         */
/*                                  Konstante LSA_RQB_ERROR_CODE_SIZE neu    */
/*                                  definiert; ausserdem Kommentar ergaenzt. */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00167458: Copyright abgeaendert. */
/*  2005-07-26  P2.1.0.0_0.5.1.1 ws ARTS+ AP00240160: Neu: LSA_RSP_OK_OFFSET */
/*                                  und LSA_RSP_ERR_OFFSET.                  */
/*  2008-01-25 P2.1.0.0_0.11.1.1 ws ARTS+ AP00565054: Wegen 64 Bit-Plattform */
/*                                  Konstante LSA_RQB_ERROR_CODE_SIZE        */
/*                                  angepasst.                               */
/*  2008-12-01 P2.1.0.0_0.11.2.1 ws Im Kommentar Schreibfehler korrigiert.   */
/*                                                                           */
/*****************************************************************************/
                              


/*===========================================================================*/
/*                            constants for lsa                              */
/*===========================================================================*/

/*=====  response codes within request block  =====*/
/* If possible, a component should define its *_RSP_* constants in *_usr.h */
/* on these LSA constants:                                                 */
#define LSA_RSP_OK                    LSA_OK   /* without errors */

#define LSA_RSP_ERR_RESOURCE          0x84     /* no local resources       */
                                               /* available (memory, LSAP, */
                                               /* timer, ...)              */
#define LSA_RSP_ERR_PARAM             0x86     /* any parameters in the RQB */
                                               /* are faulty, incl. opcode  */
#define LSA_RSP_ERR_SEQUENCE          0x87     /* wrong state */
#define LSA_RSP_ERR_SYS_PATH          0x91     /* error at *_GET_PATH_INFO() */
                                               /* or if the contents of the  */
                                               /* detail-pointer isn't       */
                                               /* correct or inconsistent    */
#define LSA_RSP_UNUSED                0xff     /* unused */

#define LSA_RSP_OK_OFFSET             0x40
          /* lsa-components may use 0x40 - 0x7F for own response ok-codes */
#define LSA_RSP_ERR_OFFSET            0xc0
          /* lsa-components may use 0xc0 - 0xfe for own response error-codes */
          /* 0xff is reserved for *_RSP_UNUSED */


/*====  for LSA_RQB_LOCAL_ERROR_TYPE  =====*/
#define LSA_RQB_ERROR_CODE_SIZE       LSA_MAX(12, 3*sizeof(LSA_VOID_PTR_TYPE))


/*===========================================================================*/
/*                              types for lsa                                */
/*===========================================================================*/

/*=====  lsa request block definition  =====*/

/* error from lower layer
 */
typedef struct lsa_rqb_local_error_tag
{
    LSA_UINT16    lsa_component_id;   /* lsa-component where the error has   */
                                      /* been detected, see lsa_sys.h        */ 
    LSA_UINT16    error_code_id;      /* how to interpret the following      */
                                      /* error_code[LSA_RQB_ERROR_CODE_SIZE] */
    LSA_UINT8     error_code[LSA_RQB_ERROR_CODE_SIZE];
} LSA_RQB_LOCAL_ERROR_TYPE;
/* (LSA_RQB_LOCAL_ERROR_TYPE itself will always be 4-byte-aligned.) 
 */
/* Hints: 
 * - If a component uses an own structure for filling error_code, it should 
 *   copy the values of its structure with the help of *_COPY_COMMON_MEM() 
 *   into error_code. Otherwise perhaps alignment problems can occur.
 * - If the component developer wants to store pointers into error_code, he
 *   must take into account the size of a pointer. The component developer can 
 *   therefore store at most 3 pointers into this structure (see definition of 
 *   LSA_RQB_ERROR_CODE_SIZE above).
 */

/* Note: Other request block definitions see the files *_cfg.h and *_usr.h 
 * at the respective LSA-component.
 */



/*===========================================================================*/
/*                             macros for lsa                                */
/*===========================================================================*/



/*****************************************************************************/
/*  end of file LSA_USR.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of LSA_USR_H */
