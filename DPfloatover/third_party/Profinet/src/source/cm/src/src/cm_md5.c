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
/*  F i l e               &F: cm_md5.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MD5                                things                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  691
#define CM_MODULE_ID       691

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

#define CM_MD5_PUT_64BIT_LE(cp, value) {				\
	(cp)[7] = 0xff & ((value) >> 56);					\
	(cp)[6] = 0xff & ((value) >> 48);					\
	(cp)[5] = 0xff & ((value) >> 40);					\
	(cp)[4] = 0xff & ((value) >> 32);					\
	(cp)[3] = 0xff & ((value) >> 24);					\
	(cp)[2] = 0xff & ((value) >> 16);					\
	(cp)[1] = 0xff & ((value) >> 8);					\
	(cp)[0] = 0xff & ((value)); }

#define CM_MD5_PUT_32BIT_LE(cp, value) {				\
	(cp)[3] = 0xff & ((value) >> 24);					\
	(cp)[2] = 0xff & ((value) >> 16);					\
	(cp)[1] = 0xff & ((value) >> 8);					\
	(cp)[0] = 0xff & ((value)); }

static LSA_UINT8 CM_MD5_PADDING[CM_MD5_BLOCK_LENGTH] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define CM_MD5_F1(x, y, z) (z ^ (x & (y ^ z)))
#define CM_MD5_F2(x, y, z) CM_MD5_F1(z, x, y)
#define CM_MD5_F3(x, y, z) (x ^ y ^ z)
#define CM_MD5_F4(x, y, z) (y ^ (x | ~z))

#define CM_MD5_MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_init (
	CM_MD5_CTX_PTR_TYPE ctx
	)
{
	ctx->count = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
}

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_transform (
	LSA_UINT32 state[4],
	LSA_UINT8 block[CM_MD5_BLOCK_LENGTH]
	)
{
	LSA_UINT32 a, b, c, d, in[CM_MD5_BLOCK_LENGTH / 4];

	CM_MEMCPY(in, block, sizeof(in));

	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];

	CM_MD5_MD5STEP(CM_MD5_F1, a, b, c, d, (in[ 0] + 0xd76aa478),  7);
	CM_MD5_MD5STEP(CM_MD5_F1, d, a, b, c, (in[ 1] + 0xe8c7b756), 12);
	CM_MD5_MD5STEP(CM_MD5_F1, c, d, a, b, (in[ 2] + 0x242070db), 17);
	CM_MD5_MD5STEP(CM_MD5_F1, b, c, d, a, (in[ 3] + 0xc1bdceee), 22);
	CM_MD5_MD5STEP(CM_MD5_F1, a, b, c, d, (in[ 4] + 0xf57c0faf),  7);
	CM_MD5_MD5STEP(CM_MD5_F1, d, a, b, c, (in[ 5] + 0x4787c62a), 12);
	CM_MD5_MD5STEP(CM_MD5_F1, c, d, a, b, (in[ 6] + 0xa8304613), 17);
	CM_MD5_MD5STEP(CM_MD5_F1, b, c, d, a, (in[ 7] + 0xfd469501), 22);
	CM_MD5_MD5STEP(CM_MD5_F1, a, b, c, d, (in[ 8] + 0x698098d8),  7);
	CM_MD5_MD5STEP(CM_MD5_F1, d, a, b, c, (in[ 9] + 0x8b44f7af), 12);
	CM_MD5_MD5STEP(CM_MD5_F1, c, d, a, b, (in[10] + 0xffff5bb1), 17);
	CM_MD5_MD5STEP(CM_MD5_F1, b, c, d, a, (in[11] + 0x895cd7be), 22);
	CM_MD5_MD5STEP(CM_MD5_F1, a, b, c, d, (in[12] + 0x6b901122),  7);
	CM_MD5_MD5STEP(CM_MD5_F1, d, a, b, c, (in[13] + 0xfd987193), 12);
	CM_MD5_MD5STEP(CM_MD5_F1, c, d, a, b, (in[14] + 0xa679438e), 17);
	CM_MD5_MD5STEP(CM_MD5_F1, b, c, d, a, (in[15] + 0x49b40821), 22);

	CM_MD5_MD5STEP(CM_MD5_F2, a, b, c, d, (in[ 1] + 0xf61e2562),  5);
	CM_MD5_MD5STEP(CM_MD5_F2, d, a, b, c, (in[ 6] + 0xc040b340),  9);
	CM_MD5_MD5STEP(CM_MD5_F2, c, d, a, b, (in[11] + 0x265e5a51), 14);
	CM_MD5_MD5STEP(CM_MD5_F2, b, c, d, a, (in[ 0] + 0xe9b6c7aa), 20);
	CM_MD5_MD5STEP(CM_MD5_F2, a, b, c, d, (in[ 5] + 0xd62f105d),  5);
	CM_MD5_MD5STEP(CM_MD5_F2, d, a, b, c, (in[10] + 0x02441453),  9);
	CM_MD5_MD5STEP(CM_MD5_F2, c, d, a, b, (in[15] + 0xd8a1e681), 14);
	CM_MD5_MD5STEP(CM_MD5_F2, b, c, d, a, (in[ 4] + 0xe7d3fbc8), 20);
	CM_MD5_MD5STEP(CM_MD5_F2, a, b, c, d, (in[ 9] + 0x21e1cde6),  5);
	CM_MD5_MD5STEP(CM_MD5_F2, d, a, b, c, (in[14] + 0xc33707d6),  9);
	CM_MD5_MD5STEP(CM_MD5_F2, c, d, a, b, (in[ 3] + 0xf4d50d87), 14);
	CM_MD5_MD5STEP(CM_MD5_F2, b, c, d, a, (in[ 8] + 0x455a14ed), 20);
	CM_MD5_MD5STEP(CM_MD5_F2, a, b, c, d, (in[13] + 0xa9e3e905),  5);
	CM_MD5_MD5STEP(CM_MD5_F2, d, a, b, c, (in[ 2] + 0xfcefa3f8),  9);
	CM_MD5_MD5STEP(CM_MD5_F2, c, d, a, b, (in[ 7] + 0x676f02d9), 14);
	CM_MD5_MD5STEP(CM_MD5_F2, b, c, d, a, (in[12] + 0x8d2a4c8a), 20);

	CM_MD5_MD5STEP(CM_MD5_F3, a, b, c, d, (in[ 5] + 0xfffa3942),  4);
	CM_MD5_MD5STEP(CM_MD5_F3, d, a, b, c, (in[ 8] + 0x8771f681), 11);
	CM_MD5_MD5STEP(CM_MD5_F3, c, d, a, b, (in[11] + 0x6d9d6122), 16);
	CM_MD5_MD5STEP(CM_MD5_F3, b, c, d, a, (in[14] + 0xfde5380c), 23);
	CM_MD5_MD5STEP(CM_MD5_F3, a, b, c, d, (in[ 1] + 0xa4beea44),  4);
	CM_MD5_MD5STEP(CM_MD5_F3, d, a, b, c, (in[ 4] + 0x4bdecfa9), 11);
	CM_MD5_MD5STEP(CM_MD5_F3, c, d, a, b, (in[ 7] + 0xf6bb4b60), 16);
	CM_MD5_MD5STEP(CM_MD5_F3, b, c, d, a, (in[10] + 0xbebfbc70), 23);
	CM_MD5_MD5STEP(CM_MD5_F3, a, b, c, d, (in[13] + 0x289b7ec6),  4);
	CM_MD5_MD5STEP(CM_MD5_F3, d, a, b, c, (in[ 0] + 0xeaa127fa), 11);
	CM_MD5_MD5STEP(CM_MD5_F3, c, d, a, b, (in[ 3] + 0xd4ef3085), 16);
	CM_MD5_MD5STEP(CM_MD5_F3, b, c, d, a, (in[ 6] + 0x04881d05), 23);
	CM_MD5_MD5STEP(CM_MD5_F3, a, b, c, d, (in[ 9] + 0xd9d4d039),  4);
	CM_MD5_MD5STEP(CM_MD5_F3, d, a, b, c, (in[12] + 0xe6db99e5), 11);
	CM_MD5_MD5STEP(CM_MD5_F3, c, d, a, b, (in[15] + 0x1fa27cf8), 16);
	CM_MD5_MD5STEP(CM_MD5_F3, b, c, d, a, (in[2 ] + 0xc4ac5665), 23);

	CM_MD5_MD5STEP(CM_MD5_F4, a, b, c, d, (in[ 0] + 0xf4292244),  6);
	CM_MD5_MD5STEP(CM_MD5_F4, d, a, b, c, (in[7 ] + 0x432aff97), 10);
	CM_MD5_MD5STEP(CM_MD5_F4, c, d, a, b, (in[14] + 0xab9423a7), 15);
	CM_MD5_MD5STEP(CM_MD5_F4, b, c, d, a, (in[5 ] + 0xfc93a039), 21);
	CM_MD5_MD5STEP(CM_MD5_F4, a, b, c, d, (in[12] + 0x655b59c3),  6);
	CM_MD5_MD5STEP(CM_MD5_F4, d, a, b, c, (in[3 ] + 0x8f0ccc92), 10);
	CM_MD5_MD5STEP(CM_MD5_F4, c, d, a, b, (in[10] + 0xffeff47d), 15);
	CM_MD5_MD5STEP(CM_MD5_F4, b, c, d, a, (in[1 ] + 0x85845dd1), 21);
	CM_MD5_MD5STEP(CM_MD5_F4, a, b, c, d, (in[8 ] + 0x6fa87e4f),  6);
	CM_MD5_MD5STEP(CM_MD5_F4, d, a, b, c, (in[15] + 0xfe2ce6e0), 10);
	CM_MD5_MD5STEP(CM_MD5_F4, c, d, a, b, (in[6 ] + 0xa3014314), 15);
	CM_MD5_MD5STEP(CM_MD5_F4, b, c, d, a, (in[13] + 0x4e0811a1), 21);
	CM_MD5_MD5STEP(CM_MD5_F4, a, b, c, d, (in[4 ] + 0xf7537e82),  6);
	CM_MD5_MD5STEP(CM_MD5_F4, d, a, b, c, (in[11] + 0xbd3af235), 10);
	CM_MD5_MD5STEP(CM_MD5_F4, c, d, a, b, (in[2 ] + 0x2ad7d2bb), 15);
	CM_MD5_MD5STEP(CM_MD5_F4, b, c, d, a, (in[9 ] + 0xeb86d391), 21);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_update (
	CM_MD5_CTX_PTR_TYPE ctx,
	LSA_UINT8 *input, LSA_UINT16 len
	)
{
	LSA_UINT16 have, need;

	/* Check how many bytes we already have and how many more we need. */
	have = (LSA_UINT16)((ctx->count >> 3) & (CM_MD5_BLOCK_LENGTH - 1));
	need = CM_MD5_BLOCK_LENGTH - have;

	/* Update bitcount */
	ctx->count += (LSA_UINT64)len << 3;

	if (len >= need) {
		if (have != 0) {
			CM_MEMCPY(ctx->buffer + have, input, need);
			cm_md5_transform(ctx->state, ctx->buffer);
			input += need;
			len -= need;
			have = 0;
		}

		/* Process data in MD5_BLOCK_LENGTH-byte chunks. */
		while (len >= CM_MD5_BLOCK_LENGTH) {
			cm_md5_transform(ctx->state, input);
			input += CM_MD5_BLOCK_LENGTH;
			len -= CM_MD5_BLOCK_LENGTH;
		}
	}

	/* Handle any remaining bytes of data. */
	if (len != 0) {
		CM_MEMCPY(ctx->buffer + have, input, len);
	}
}

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_md5_final (
	LSA_UINT8 digest[CM_MD5_DIGEST_LENGTH],
	CM_MD5_CTX_PTR_TYPE ctx
	)
{
	LSA_UINT8 count[8];
	LSA_UINT16 padlen;
	int i;

	/* Convert count to 8 bytes in little endian order. */
	CM_MD5_PUT_64BIT_LE(count, ctx->count);

	/* Pad out to 56 mod 64. */
	padlen = CM_MD5_BLOCK_LENGTH -
	    ((ctx->count >> 3) & (CM_MD5_BLOCK_LENGTH - 1));
	if (padlen < 1 + 8) {
		padlen += CM_MD5_BLOCK_LENGTH;
	}
	cm_md5_update(ctx, CM_MD5_PADDING, padlen - 8);		/* padlen - 8 <= 64 */
	cm_md5_update(ctx, count, 8);

	if (digest != LSA_NULL) {
		for (i = 0; i < 4; i++) {
			CM_MD5_PUT_32BIT_LE(digest + i * 4, ctx->state[i]);
		}
	}
	CM_MEMSET(ctx, 0x0, sizeof(CM_MD5_CTX_TYPE));
}
