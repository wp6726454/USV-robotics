#ifndef CM_LIB_H
#define CM_LIB_H

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
/*  F i l e               &F: cm_lib.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Conversion functions                                                     */
/*  C-stdlib mapping functions                                               */
/*                                                                           */
/*****************************************************************************/

/*
 * included in cm_usr.h
 */

/*=============================================================================
 * function name:  CM_HTON16 ... host to network byte order, short integer
 *                 CM_HTON32 ... host to network byte order, long  integer
 *                 CM_NTOH16 ... network to host byte order, short integer
 *                 CM_NTOH32 ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CM_HTON16
LSA_UINT16 CM_SYSTEM_OUT_FCT_ATTR
CM_HTON16(
	LSA_UINT16 host_short
);
#endif

#ifndef CM_HTON32
LSA_UINT32 CM_SYSTEM_OUT_FCT_ATTR
CM_HTON32(
	LSA_UINT32 host_long
);
#endif

#ifndef CM_NTOH16
LSA_UINT16 CM_SYSTEM_OUT_FCT_ATTR
CM_NTOH16(
	LSA_UINT16 network_short
);
#endif

#ifndef CM_NTOH32
LSA_UINT32 CM_SYSTEM_OUT_FCT_ATTR
CM_NTOH32(
	LSA_UINT32 network_long
);
#endif


/*=============================================================================
 * function name:  CM_PUT8,16,32,UUID and CM_GET8,16,32,UUID
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifndef CM_PUT8_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_PUT8_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset,
	LSA_UINT8 val
);
#endif

#ifndef CM_GET8_NTOH
LSA_UINT8  CM_SYSTEM_OUT_FCT_ATTR
CM_GET8_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset
);
#endif

#ifndef CM_PUT16_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_PUT16_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset,
	LSA_UINT16 val
);
#endif

#ifndef CM_GET16_NTOH
LSA_UINT16  CM_SYSTEM_OUT_FCT_ATTR
CM_GET16_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset
);
#endif

#ifndef CM_PUT32_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_PUT32_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset,
	LSA_UINT32 val
);
#endif

#ifndef CM_GET32_NTOH
LSA_UINT32  CM_SYSTEM_OUT_FCT_ATTR
CM_GET32_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset
);
#endif

#ifndef CM_PUT_UUID_HTON
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_PUT_UUID_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset,
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const uuid
);
#endif

#ifndef CM_GET_UUID_NTOH
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_GET_UUID_NTOH( /* get and convert from network byte order to host byte order */
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset
);
#endif


/*=============================================================================
 * function name:  CM_MEMSET, CM_MEMCPY, CM_MEMMOVE
 *
 * function:       macros for standard C-lib functions
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat"
 *
 *===========================================================================*/

#ifndef CM_MEMSET
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MEMSET(
	LSA_VOID_PTR_TYPE  ptr,
	LSA_INT            val,
	LSA_UINT           len
);
#endif

#ifndef CM_MEMCPY
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MEMCPY(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len
);
#endif

#ifndef CM_MEMMOVE
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_MEMMOVE(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len
);
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_LIB_H */
