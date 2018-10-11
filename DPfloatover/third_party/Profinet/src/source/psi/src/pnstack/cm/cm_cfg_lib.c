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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: cm_cfg_lib.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CM using PSI.                           */
/*                                                                           */
/*****************************************************************************/

#define CM_MODULE_ID 32006 /* see CM_ASSERT */

#include "psi_int.h"

#include "cm_inc.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*------------------------------------------------------------------------------
//	Library Functions (prototypes in cm_lib.h)
//----------------------------------------------------------------------------*/

#ifndef CM_HTON16
LSA_UINT16 CM_SYSTEM_OUT_FCT_ATTR CM_HTON16(
	LSA_UINT16 host_short )
{
	/* return ( CLRPC_HTONS(host_short) ); */ /* map to lower layer */
	return ( PSI_HTON16(host_short) );
}
#else
#error "by design a function"
#endif

#ifndef CM_HTON32 
LSA_UINT32 CM_SYSTEM_OUT_FCT_ATTR CM_HTON32(
	LSA_UINT32 host_long )
{
	/* return( CLRPC_HTONL(host_long)); */ /* map to lower layer */
	return ( PSI_HTON32(host_long) );
}
#else
#error "by design a function"
#endif

#ifndef CM_NTOH16
LSA_UINT16 CM_SYSTEM_OUT_FCT_ATTR CM_NTOH16(
	LSA_UINT16 network_short )
{
	/* return( CLRPC_NTOHS(network_short) ); */ /* map to lower layer */
	return ( PSI_NTOH16(network_short) );
}
#else
#error "by design a function"
#endif

#ifndef CM_NTOH32
LSA_UINT32 CM_SYSTEM_OUT_FCT_ATTR CM_NTOH32(
	LSA_UINT32 network_long )
{
	/* return( CLRPC_NTOHL(network_long)); */ /* map to lower layer */
	return ( PSI_NTOH32(network_long) );
}
#else
#error "by design a function"
#endif

/*------------------------------------------------------------------------------
// Put / Get
//----------------------------------------------------------------------------*/

#ifndef CM_PUT8_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_PUT8_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT8         val )
{
	CM_COMMON_MEM_U8_PTR_TYPE ptr = (CM_COMMON_MEM_U8_PTR_TYPE) base;
	ptr[offset] = val;
}
#else
#error "by design a function"
#endif

#ifndef CM_GET8_NTOH
LSA_UINT8  CM_SYSTEM_OUT_FCT_ATTR CM_GET8_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	CM_COMMON_MEM_U8_PTR_TYPE ptr = (CM_COMMON_MEM_U8_PTR_TYPE) base;
	return ptr[offset];
}
#else
#error "by design a function"
#endif

#ifndef CM_PUT16_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_PUT16_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT16         val )
{
	CM_COMMON_MEM_U8_PTR_TYPE ptr = (CM_COMMON_MEM_U8_PTR_TYPE) base;
	ptr += offset;
	val = CM_HTON16(val);
	CM_MEMCPY(ptr, &val, sizeof(val));
}
#else
#error "by design a function"
#endif

#ifndef CM_GET16_NTOH
LSA_UINT16  CM_SYSTEM_OUT_FCT_ATTR CM_GET16_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE   base,
	LSA_INT             offset )
{
	LSA_UINT16 val;
	CM_COMMON_MEM_U8_PTR_TYPE ptr = (CM_COMMON_MEM_U8_PTR_TYPE) base;
	ptr += offset;
	CM_MEMCPY(&val, ptr, sizeof(val));

	return( CM_NTOH16(val) );
}
#else
#error "by design a function"
#endif

#ifndef CM_PUT32_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_PUT32_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT32         val )
{
	CM_COMMON_MEM_U8_PTR_TYPE ptr = (CM_COMMON_MEM_U8_PTR_TYPE) base;
	ptr += offset;
	val = CM_HTON32(val);
	CM_MEMCPY(ptr, &val, sizeof(val));
}
#else
#error "by design a function"
#endif

#ifndef CM_GET32_NTOH
LSA_UINT32  CM_SYSTEM_OUT_FCT_ATTR CM_GET32_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE   base,
	LSA_INT             offset )
{
	LSA_UINT32 val;
	CM_COMMON_MEM_U8_PTR_TYPE ptr = (CM_COMMON_MEM_U8_PTR_TYPE) base;
	ptr += offset;
	CM_MEMCPY(&val, ptr, sizeof(val));

	return( CM_NTOH32(val));
}
#else
#error "by design a function"
#endif

#ifndef CM_PUT_UUID_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_PUT_UUID_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE                           base,
	LSA_INT                                     offset,
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR   * const uuid )
{
	CM_PUT32_HTON(base, offset +  0, uuid->time_low);
	CM_PUT16_HTON(base, offset +  4, uuid->time_mid);
	CM_PUT16_HTON(base, offset +  6, uuid->time_hi_and_version);
	CM_PUT8_HTON (base, offset +  8, uuid->clock_seq_hi_and_reserved);
	CM_PUT8_HTON (base, offset +  9, uuid->clock_seq_low);
	CM_PUT8_HTON (base, offset + 10, uuid->node[0]);
	CM_PUT8_HTON (base, offset + 11, uuid->node[1]);
	CM_PUT8_HTON (base, offset + 12, uuid->node[2]);
	CM_PUT8_HTON (base, offset + 13, uuid->node[3]);
	CM_PUT8_HTON (base, offset + 14, uuid->node[4]);
	CM_PUT8_HTON (base, offset + 15, uuid->node[5]);
}
#else
#error "by design a function"
#endif

#ifndef CM_GET_UUID_NTOH
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_GET_UUID_NTOH( /* get and convert from network byte order to host byte order */
	CLRPC_UUID_PTR_TYPE   uuid,
	LSA_VOID_PTR_TYPE     base,
	LSA_INT               offset )
{
	uuid->time_low                  = CM_GET32_NTOH(base, offset +  0);
	uuid->time_mid                  = CM_GET16_NTOH(base, offset +  4);
	uuid->time_hi_and_version       = CM_GET16_NTOH(base, offset +  6);
	uuid->clock_seq_hi_and_reserved = CM_GET8_NTOH (base, offset +  8);
	uuid->clock_seq_low             = CM_GET8_NTOH (base, offset +  9);
	uuid->node[0]                   = CM_GET8_NTOH (base, offset + 10);
	uuid->node[1]                   = CM_GET8_NTOH (base, offset + 11);
	uuid->node[2]                   = CM_GET8_NTOH (base, offset + 12);
	uuid->node[3]                   = CM_GET8_NTOH (base, offset + 13);
	uuid->node[4]                   = CM_GET8_NTOH (base, offset + 14);
	uuid->node[5]                   = CM_GET8_NTOH (base, offset + 15);
}
#else
#error "by design a function"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CM_MEMSET
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_MEMSET(
	LSA_VOID_PTR_TYPE  ptr,
	LSA_INT            val,
	LSA_UINT           len )
{
	PSI_MEMSET( ptr, val, len );
}
#endif

#ifndef CM_MEMCPY
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_MEMCPY(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len )
{
	PSI_MEMCPY( dst, src, len );
}
#endif

#ifndef CM_MEMMOVE
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_MEMMOVE(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len )
{
	PSI_MEMMOVE( dst, src, len );
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
