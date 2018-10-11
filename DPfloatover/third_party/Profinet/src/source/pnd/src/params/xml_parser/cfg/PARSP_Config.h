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
/*  F i l e               &F: PARSP_Config.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  source level config parameters are set here                              */
/*                                                                           */
/*****************************************************************************/

/* ---------------------------------------------------------------------------------------------------------------------- */
/* --- !!! PORTLAYER !! -- !! PORTLAYER !! -- !! PORTLAYER !! - !! PORTLAYER !! -- !! PORTLAYER !! -- !! PORTLAYER !! --- */
/* ---------------------------------------------------------------------------------------------------------------------- */

#ifndef __CONFIG_DEFINED__
#define __CONFIG_DEFINED__

/*----------------------------------------------------------------------------------------------------*\
|   Compiler DLL Binding                                                        Compiler DLL Binding   |
\*----------------------------------------------------------------------------------------------------*/
#define     DLLCALL     /*__stdcall*/

#define PARS_HAVE_A_DLL_SYSTEM              PARS_CF_NO                /**< Do we use DLL system?                          */
#define PARS_LOADER_SUPP_WEBSERVICES        PARS_CF_NO                /**< Must be set to support WebServices             */
#define PARS_SOCKET_USE_MULTICAST           PARS_CF_NO                /**< Should Multi cast Sockets be used              */
#define PARS_SOCKET_USE_CLIENT_SOCKET       PARS_CF_NO                /**< Should Client Socket be supported              */
#define PARS_SOCKET_USES_EXTENDED           PARS_CF_NO                /**< Support extended Socket functions              */
#define PARS_LIC_MANAGEMENT                 PARS_CF_NO                /**< Activate License Management                    */
#define PARS_USE_SSL                        PARS_CF_NO                /**< Enable SSL Support                             */
#define PARS_HTTP_TRANSFER_BUFFER           (8  PARS_CF_KILO_BYTES)   /**< Amount of Bytes sent per Block                 */
#define PARS_DEFAP_DIR_BROWSING             PARS_CF_YES               /**< Include Directory Browsing Feature             */
#define PARS_ENABLE_EXTERNAL_TOOLS          PARS_CF_NO                /**< Enable EXEC for CGI Application                */
#define PARS_CGI_MWSL_EXTENSION             PARS_CF_NO                /**< Enable MWSL CGI Gateway                        */
#define PARS_AUTH_USE_HASH                  PARS_CF_YES
#define PARS_XUDB_SUPP_CERT                 PARS_CF_NO
#define PARS_MWSL2_SUPPORT_DATAIF           PARS_CF_NO
#define PARS_MWSL2_DENY_TEMPL_RECURSION     PARS_CF_NO               /**< Prohibit Template within Templates             */

#define PARS_MEM_MEMPOOL_VARPROVIDER        PARS_CF_NO
#define PARS_IMPLEMENT_EXTENSION_IF         PARS_CF_YES
#define PARS_EXTENSION_ASYNC_READ_EVENT     PARS_CF_NO          /**< Uncouples the simulated read event from call stack   */

#define PARS_MWSL2_USE_COMPILATION          PARS_CF_YES                                 /**< only look for compiled files */
#define PARS_IPV6_SUPPORT                   PARS_CF_NO
#define PARS_PROACTOR_USE_EXPLICIT_READ     PARS_CF_NO
#define PARS_IMPL_URL_PARAM_HANDLING        PARS_CF_YES
#define PARS_MWSL2_PUSHED_OUTPUT            PARS_CF_NO
#define PARS_HTTP_VERBOSE_TRACE_OUTPUT      PARS_CF_NO                /**< Produces lots of trace about every sent buffer */
#define PARS_PROACTOR_CALLTRACE             PARS_CF_YES
#define PARS_DEBUG_OUT                      PARS_CF_YES               /**< Global Switch for Debug infos                  */

#define PARS_LINKAGE_HTTP_SECURITY          PARS_CF_STATIC
#define PARS_LINKAGE_MWSL2                  PARS_CF_STATIC
#define PARS_LINKAGE_AUTH_FORMBASED         PARS_CF_STATIC
#define PARS_LINKAGE_XMLUSERDATABASE        PARS_CF_STATIC
#define PARS_LINKAGE_CRYPT_SERVICE          PARS_CF_STATIC
#define PARS_LINKAGE_CONVERSIONSERVICE      PARS_CF_STATIC
#define PARS_LINKAGE_CRYPT_MD5              PARS_CF_STATIC               /* Change later when we have an openssl free md5 */
#define PARS_LINKAGE_AJAXAPPLICATION        PARS_CF_NONE
#define PARS_LINKAGE_SOCKIF                 PARS_CF_NONE

#define PARS_PROACTOR_PRINT_THROUGHPUT      PARS_CF_NO
#define PARS_MEM_MALLOC_USE_POOLS           PARS_CF_NO

/* Directories will be defined in PARSP_Config_S7P.h */
#if 1 /* For Frank's PC */
#define PARS_WWWRootDefault                 ".." PARS_DIR_SEPERATOR ".." PARS_DIR_SEPERATOR ".." PARS_DIR_SEPERATOR "Test" PARS_DIR_SEPERATOR "WWWRoot" PARS_DIR_SEPERATOR
#define PARS_SystemRootDefault              ".." PARS_DIR_SEPERATOR ".." PARS_DIR_SEPERATOR ".." PARS_DIR_SEPERATOR "Test" PARS_DIR_SEPERATOR "Win32" PARS_DIR_SEPERATOR "System" PARS_DIR_SEPERATOR
#else /* 1 */
#include "PARSP_Config_S7P.h"
#endif /* 1 */
#include "PARSP_Config_default.h"

#endif /* __CONFIG_DEFINED__ */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/