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
/*  F i l e               &F: pnd_xml_cfg.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of XML Parser helper access functions                     */
/*                                                                           */
/*****************************************************************************/


#include "pnd_xml_cfg.h"


///////////////////////////////////////////////////////////////////////////////
///
///  Memory allocation system function. 
///
///  @param [in]       a_dwSize (LSA_UINT16): size
///
///  @return uuid (LSA_VOID_PTR_TYPE):Pointer to the allocated memory
///
///////////////////////////////////////////////////////////////////////////////

LSA_VOID_PTR_TYPE  xml_malloc( /*  in */ LSA_UINT32     a_dwSize )
{
    return pnd_mem_alloc ( a_dwSize);
}

///////////////////////////////////////////////////////////////////////////////
///
///  Memory free system function. 
///
///  @param [in]       a_ptrPointer (LSA_VOID_PTR_TYPE): memory pointer
///
///  @return none
///
///////////////////////////////////////////////////////////////////////////////

LSA_VOID xml_free(/*  in */ LSA_VOID_PTR_TYPE a_ptrPointer )
{
    if (a_ptrPointer!=LSA_NULL)
    {
        pnd_mem_free(a_ptrPointer);
    }
    return;
}

///////////////////////////////////////////////////////////////////////////////
///
///  memcpy system function. 
///
///  @param [in]       dst (LSA_VOID_PTR_TYPE): destination
///  @param [in]       src (LSA_VOID_PTR_TYPE): source
///  @param [in]       len (LSA_UINT)         : length
///
///  @return (LSA_VOID)
///
///////////////////////////////////////////////////////////////////////////////

LSA_VOID xml_memcpy (LSA_VOID_PTR_TYPE dst, LSA_VOID_PTR_TYPE src, LSA_UINT len)
{
    PND_ASSERT(dst);
    PND_ASSERT(src);
    pnd_memcpy (dst, src, len);
}

///////////////////////////////////////////////////////////////////////////////
///
///  xml_htonl function converts a u_long from host to TCP/IP network byte order (which is big-endian).
///
///  @param [in]       hostlong (LSA_VOID_PTR_TYPE): A 32-bit number in host byte order.
///
///  @return (LSA_UINT32) value in TCP/IP's network byte order.
///
///////////////////////////////////////////////////////////////////////////////
LSA_UINT32 xml_htonl(LSA_UINT32 hostlong)
{
    return pnd_hton32 (hostlong);
}
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
