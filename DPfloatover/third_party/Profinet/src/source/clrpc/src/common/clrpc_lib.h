#ifndef CLRPC_LIB_H
#define CLRPC_LIB_H

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
/*  C o m p o n e n t     &C: CLRPC (ConnectionLess RPC)                :C&  */
/*                                                                           */
/*  F i l e               &F: clrpc_lib.h                               :F&  */
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

/*=============================================================================*/
/** \brief converts a uuid into a string

	 uuid string: tttttttt-tttt-vttt-rccc-nnnnnnnnnnnn 
*//*===========================================================================*/
#define CLRPC_MAX_UUID_STRING  (36+1) /* including the '\0' */

LSA_UINT16  CLRPC_SYSTEM_OUT_FCT_ATTR
clrpc_uuid_to_string (
	CLRPC_UUID_PTR_TYPE	uuid,
	CLRPC_COMMON_MEM_U8_PTR_TYPE uuid_string
);

/*=============================================================================*/
/** \brief converts a uuid from string to uuid-format

	 uuid string: tttttttt-tttt-vttt-rccc-nnnnnnnnnnnn 
*//*===========================================================================*/
LSA_UINT16  CLRPC_SYSTEM_OUT_FCT_ATTR
clrpc_uuid_from_string (
	CLRPC_UUID_PTR_TYPE	uuid,
	CLRPC_COMMON_MEM_U8_PTR_TYPE uuid_string
);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_LIB_H */
