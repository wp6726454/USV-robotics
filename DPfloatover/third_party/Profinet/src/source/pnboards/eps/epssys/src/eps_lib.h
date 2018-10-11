#ifndef EPS_LIB_H_
#define EPS_LIB_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_lib.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Standard Lib Include                                                 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#include <stdio.h>
#include <string.h>
#if (EPS_PLF == EPS_PLF_LINUX_X86)
#include <stdint.h>
#else
#include <stdlib.h>
#endif

/*****************************************************************************/
/*  stdio                                                                    */
/*****************************************************************************/
#define eps_memset      memset
#define eps_memcpy      memcpy
#define eps_memcmp      memcmp
#define eps_memmove     memmove
#define eps_strlen(_Str)           strlen((const char*) _Str)
#define eps_strcmp(_Str1, _Str2)   strcmp((const char*) _Str1, (const char*) _Str2)
#define eps_strcpy(_Dest, _Source) strcpy((char*) _Dest, (const char*) _Source)


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_LIB_H_ */