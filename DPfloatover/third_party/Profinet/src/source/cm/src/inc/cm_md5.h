#ifndef CM_MD5_H
#define CM_MD5_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_md5.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MD5 things                                                               */
/*                                                                           */
/*****************************************************************************/

#define	CM_MD5_BLOCK_LENGTH		64
#define	CM_MD5_DIGEST_LENGTH	16

typedef struct CM_MD5Context {
	LSA_UINT32 state[4];                   /* state */
	LSA_UINT64 count;                      /* number of bits, mod 2^64 */
	LSA_UINT8 buffer[CM_MD5_BLOCK_LENGTH]; /* input buffer */
} CM_MD5_CTX_TYPE;

typedef CM_MD5_CTX_TYPE CM_COMMON_MEM_ATTR  * CM_MD5_CTX_PTR_TYPE;

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_init (
	CM_MD5_CTX_PTR_TYPE ctx
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_update (
	CM_MD5_CTX_PTR_TYPE ctx,
	LSA_UINT8 *input, LSA_UINT16 len
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_transform (
	LSA_UINT32 state[4],
	LSA_UINT8 block[CM_MD5_BLOCK_LENGTH]
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_final (
	LSA_UINT8 digest[CM_MD5_DIGEST_LENGTH],
	CM_MD5_CTX_PTR_TYPE ctx
	);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_MD5_H */
