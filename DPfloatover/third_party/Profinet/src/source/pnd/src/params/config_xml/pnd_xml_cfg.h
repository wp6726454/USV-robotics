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
/*  F i l e               &F: pnd_xml_cfg.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration for XML:                                                   */
/*  Defines constants, types and macros for XML interpreter                  */
/*                                                                           */
/*  This file has to be overwritten during system integration                */
/*                                                                           */
/*****************************************************************************/



#ifndef PND_XML_CFG_INCLUDED
#define PND_XML_CFG_INCLUDED


#include "psi_inc.h"
#include "pnd_sys.h"

#define XML_MAX_RECORD_LENGTH 10000



#define         xml_strlen      pnd_strlen
#define         xml_strcpy      pnd_strcpy
#define         xml_strtok      pnd_strtok
#define         xml_strcmp      pnd_strcmp
#define         xml_strncmp     pnd_strncmp
#define         xml_memset      pnd_memset
LSA_VOID_PTR_TYPE       xml_malloc( /*  in */ LSA_UINT32     a_dwSize );
LSA_VOID                xml_free(/*  in */ LSA_VOID_PTR_TYPE a_ptrPointer );
LSA_VOID                xml_memcpy (LSA_VOID_PTR_TYPE dst, LSA_VOID_PTR_TYPE src, LSA_UINT len);
LSA_UINT32              xml_htonl(LSA_UINT32 hostlong);


///////////////////////////////////////////////////////////////////////////////
///  xml_printf shall stand for target platform supported printf.
///////////////////////////////////////////////////////////////////////////////

#define xml_printf pnd_printf
///////////////////////////////////////////////////////////////////////////////
///  XML_ASSERT shall stand for target platform suppoerted assert functionality.
///////////////////////////////////////////////////////////////////////////////
#define XML_ASSERT(cond)    { \
    if (!(cond)) { pnd_fatal(__FILE__, __LINE__, __FUNCTION__, "IOX_ASSERT: " #cond); } \
    }


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_XML_CFG_H */