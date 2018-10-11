#ifndef DCP_PLS_H                     /* ----- reinclude-protection ----- */
#define DCP_PLS_H

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
/*  C o m p o n e n t     &C: DCP (Discovery & Configuration Protocol)  :C&  */
/*                                                                           */
/*  F i l e               &F: dcp_pls.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Check the configuration                                                  */
/*                                                                           */
/*****************************************************************************/
#ifdef DCP_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2003-04-15  P01.00.00.00_00.03.00.00  lrg  from LSA_GLOB_P02.01.00.00_00 */
/*                                             .01.01.01                     */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*  2003-12-15  P02.00.00.00_00.01.00.00  lrg  Version 2                     */
/*  2004-02-12  P02.00.00.00_00.02.00.00  lrg  Check:                        */
/*              #if !defined DCP_SERVER_MODE && !defined DCP_CLIENT_MODE     */
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg  Version 3                     */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  LSA_GLOB 2.1.0.0_0.4.2.1      */
/*  2006-06-30  P03.12.00.00_00.01.01.02  lrg  LSA_GLOB 2.1.0.0_0.7.2.1      */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2010-09-01  P05.00.00.00_00.01.01.02 lrg: AP00999104 DCP_CFG_TRACE_MODE  */
/*  2012-01-11  T05.01.00.00_00.05.13.01                                     */
/*              AP01297962: remove DCP_CLIENT_MODE and DCP_SERVER_MODE       */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined LSA_HOST_ENDIANESS_BIG && !defined LSA_HOST_ENDIANESS_LITTLE
#error "LSA_HOST_ENDIANESS_BIG or LSA_HOST_ENDIANESS_LITTLE is missing in file lsa_cfg.h!"
#endif

#if defined LSA_HOST_ENDIANESS_BIG && defined LSA_HOST_ENDIANESS_LITTLE
#error "Define LSA_HOST_ENDIANESS_BIG or LSA_HOST_ENDIANESS_LITTLE in file lsa_cfg.h!"
#endif

#if !defined DCP_CFG_TRACE_MODE
#error "DCP_CFG_TRACE_MODE not defined in file dcp_cfg.h"
#endif

#if !defined DCP_UPPER_IN_FCT_ATTR
#error "DCP_UPPER_IN_FCT_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_UPPER_OUT_FCT_ATTR
#error "DCP_UPPER_OUT_FCT_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_LOWER_IN_FCT_ATTR
#error "DCP_LOWER_IN_FCT_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_LOWER_OUT_FCT_ATTR
#error "DCP_LOWER_OUT_FCT_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_SYSTEM_IN_FCT_ATTR
#error "DCP_SYSTEM_IN_FCT_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_SYSTEM_OUT_FCT_ATTR
#error "DCP_SYSTEM_OUT_FCT_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_LOCAL_FCT_ATTR
#error "DCP_LOCAL_FCT_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_UPPER_RQB_ATTR
#error "DCP_UPPER_RQB_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_UPPER_MEM_ATTR
#error "DCP_UPPER_MEM_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_LOWER_RQB_ATTR
#error "DCP_LOWER_RQB_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_LOWER_MEM_ATTR
#error "DCP_LOWER_MEM_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_SYSTEM_MEM_ATTR
#error "DCP_SYSTEM_MEM_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined DCP_LOCAL_MEM_ATTR
#error "DCP_LOCAL_MEM_ATTR not defined in file dcp_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif

//#if !defined DCP_LOCAL_MEM_STATIC && !defined DCP_LOCAL_MEM_DYNAMIC
//#error "DCP_LOCAL_MEM_STATIC or DCP_LOCAL_MEM_DYNAMIC is missing in file dcp_cfg.h!"
#if !defined DCP_LOCAL_MEM_DYNAMIC || defined DCP_LOCAL_MEM_STATIC 
#error "Only DCP_LOCAL_MEM_DYNAMIC has to be defined in file dcp_cfg.h!"
#endif

//#if defined DCP_LOCAL_MEM_STATIC && defined DCP_LOCAL_MEM_DYNAMIC
//#error "Define DCP_LOCAL_MEM_STATIC or DCP_LOCAL_MEM_DYNAMIC in file dcp_cfg.h!"
//#endif

#if !defined DCP_CLEAR_GLOBAL_DATA && !defined DCP_NO_CLEAR_GLOBAL_DATA
#error "DCP_CLEAR_GLOBAL_DATA or DCP_NO_CLEAR_GLOBAL_DATA is missing in file dcp_cfg.h!"
#endif

#if defined DCP_CLEAR_GLOBAL_DATA && defined DCP_NO_CLEAR_GLOBAL_DATA
#error "Define DCP_CLEAR_GLOBAL_DATA or DCP_NO_CLEAR_GLOBAL_DATA in file dcp_cfg.h!"
#endif


/*****************************************************************************/
/*  end of file DCP_PLS.H                                                    */
/*****************************************************************************/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of DCP_PLS_H */
