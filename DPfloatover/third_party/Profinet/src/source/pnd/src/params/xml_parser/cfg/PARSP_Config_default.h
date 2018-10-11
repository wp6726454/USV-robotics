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
/*  F i l e               &F: PARSP_Config_default.h                    :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The main source configuration file of the xml parser - default settings  */
/*  Within the config.h file all source level config parameters are set.     */
/*                                                                           */
/*****************************************************************************/

#ifndef __CONFIG_DEFAULT_DEFINED__
#define __CONFIG_DEFAULT_DEFINED__

/* ---  ------------------------------------------------------------------------------------------------------------- --- *\
|   Includes                                                                                                    Includes   |
\* --- -------------------------------------------------------------------------------------------------------------- --- */
#include "PARS_Version.h"                                                                /* KM Version configuration file */

/* ---------------------------------------------------------------------------------------------------------------------- */
/**
 * @name Version information
 *
 * These defines set up various version related information
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_PLATTFORM_NAME
#  define     PARS_PLATTFORM_NAME             "PC Based"
#endif /* PARS_PLATTFORM_NAME */
#ifndef PARS_DIR_SEPARATOR
#  define     PARS_DIR_SEPARATOR              "\\"
#endif /* PARS_DIR_SEPARATOR */
#ifndef PARS_WWWRootDefault
#  define     PARS_WWWRootDefault             ".." PARS_DIR_SEPARATOR ".." PARS_DIR_SEPARATOR "Test" PARS_DIR_SEPARATOR "WWWRoot" PARS_DIR_SEPARATOR
#endif /* PARS_WWWRootDefault */
#ifndef PARS_SystemRootDefault
#  define     PARS_SystemRootDefault          ".." PARS_DIR_SEPARATOR ".." PARS_DIR_SEPARATOR "Test" PARS_DIR_SEPARATOR "Win32" PARS_DIR_SEPARATOR "System" PARS_DIR_SEPARATOR
#endif /* PARS_SystemRootDefault */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Compiler DLL Binding
 *
 * These defines are responsible for binding dynamic libs
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef DLLCALL
#  define     DLLCALL         __stdcall
#endif /* DLLCALL */
#ifndef DLL_EXPORT
#  define     DLL_EXPORT      __declspec( dllexport )
#endif /* DLL_EXPORT */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Configuration option values
 *
 * normally, you shouldn't have to change anything here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#define PARS_CF_YES                           1                       /**< used to enable a configuration flag            */
#define PARS_CF_NO                            0                       /**< used to disable a configuration flag           */

#define PARS_CF_NONE                          0
#define PARS_CF_STATIC                        1
#define PARS_CF_DYNAMIC                       2

#ifndef PARS_CF_BYTES
#  define PARS_CF_BYTES                       *1                      /**< multiplication factor for bytes                */
#endif /* PARS_CF_BYTES */
#ifndef PARS_CF_KILO_BYTES
#  define PARS_CF_KILO_BYTES                  *1024                   /**< multiplication factor for kilobytes            */
#endif /* PARS_CF_KILO_BYTES */
#ifndef PARS_CF_MEGA_BYTES
#  define PARS_CF_MEGA_BYTES                  *1048576                /**< multiplication factor for megabytes            */
#endif /* PARS_CF_MEGA_BYTES */

#ifndef PARS_CF_SECOND
#  define PARS_CF_SECOND                      *1000                   /**< multiplication factor for one second           */
#endif /* PARS_CF_SECOND */
#ifndef PARS_CF_SECONDS
#  define PARS_CF_SECONDS                     PARS_CF_SECOND          /**< multiplication factor for seconds              */
#endif /* PARS_CF_SECONDS */
#ifndef PARS_CF_MINUTE
#  define PARS_CF_MINUTE                      *60 PARS_CF_SECONDS     /**< multiplication factor for one minute           */
#endif /* PARS_CF_MINUTE */
#ifndef PARS_CF_MINUTES
#  define PARS_CF_MINUTES                     PARS_CF_MINUTE          /**< multiplication factor for minutes              */
#endif /* PARS_CF_MINUTES */
#ifndef PARS_CF_HOUR
#  define PARS_CF_HOUR                        *60 PARS_CF_MINUTES     /**< multiplication factor for one hour             */
#endif /* PARS_CF_HOUR */
#ifndef PARS_CF_HOURS
#  define PARS_CF_HOURS                       PARS_CF_HOUR            /**< multiplication factor for hours                */
#endif /* PARS_CF_HOURS */
#ifndef PARS_CF_DAY
#  define PARS_CF_DAY                         *24 PARS_CF_HOURS       /**< multiplication factor for one day              */
#endif /* PARS_CF_DAY */
#ifndef PARS_CF_DAYS
#  define PARS_CF_DAYS                        PARS_CF_DAY             /**< multiplication factor for days                 */
#endif /* PARS_CF_DAYS */

/**
 * In Visual C++ 2005, gmtime_s is an inline function which evaluates to _gmtime64_s and time_t
 * is equivalent to __time64_t. If you need to force the compiler to interpret time_t as the old
 * 32-bit time_t, you can define _USE_32BIT_TIME_T. Doing this will cause gmtime_s to be in-lined to
 * _gmtime32_s. This is not recommended because your application may fail after January 18, 2038,
 * and it is not allowed on 64-bit platforms.
 * */
#ifndef _USE_32BIT_TIME_T
#  define     _USE_32BIT_TIME_T
#endif /* _USE_32BIT_TIME_T */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name C++ handling
 *
 * Here you can configure if the code shall be built as C++ modules and how it shall behave then
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_BUILD_CPP
#  define     PARS_BUILD_CPP                  PARS_CF_NO                              /**< Compile Modules as C++ */
#endif /* PARS_BUILD_CPP */

#ifndef PARS_LITTLE_ENDIAN
#  define PARS_LITTLE_ENDIAN                  PARS_CF_YES                 /**< Compiles for little or Big endian machines */
#endif /* PARS_LITTLE_ENDIAN */

#ifndef PARS_DOUBLE_MIXED_ENDIAN
#  define PARS_DOUBLE_MIXED_ENDIAN            PARS_CF_NO     /**< on some weird platforms, a DOUBLE has a mixed endianess */
#endif /* PARS_DOUBLE_MIXED_ENDIAN */

/**
 * @def     PARS_BEGIN_EXTERN_C
 * @brief   Opens up an "extern C" block if we are not compiling C++
 * */

/**
 * @def     PARS_END_EXTERN_C
 * @brief   Closes an "extern C" block if we are not compiling C++
 * */

#if ( PARS_BUILD_CPP == PARS_CF_YES )
#  ifndef PARS_BEGIN_EXTERN_C
#    define PARS_BEGIN_EXTERN_C
#  endif /* PARS_BEGIN_EXTERN_C */
#  ifndef PARS_END_EXTERN_C
#    define PARS_END_EXTERN_C
#  endif /* PARS_END_EXTERN_C */
#else /* PARS_BUILD_CPP */
#  if defined(__cplusplus)
#    ifndef PARS_BEGIN_EXTERN_C
#      define PARS_BEGIN_EXTERN_C extern "C" {
#    endif /* PARS_BEGIN_EXTERN_C */
#    ifndef PARS_END_EXTERN_C
#      define PARS_END_EXTERN_C }
#    endif /* PARS_END_EXTERN_C */
#  else /* __cplusplus */
#    define PARS_BEGIN_EXTERN_C
#    define PARS_END_EXTERN_C
#  endif /* __cplusplus */
#endif /* PARS_BUILD_CPP */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Multi threading behavior
 *
 * Enable or disable multi threading here as well as the usage of synchronization measures and thread pools
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/**
 * Enable for Multi threaded version. Defaults are
 * - PARS_CF_YES under AuDLinux and
 * - PARS_CF_NO otherwise
 */
#ifndef PARS_MTHREAD
#  define PARS_MTHREAD                        PARS_CF_NO              /**< Use multiple Threads to process requests       */
#endif /* PARS_MTHREAD */

#ifndef PARS_USE_SYNCHRONISATION
#  define PARS_USE_SYNCHRONISATION            PARS_CF_YES             /**< Use Critical Sects and Semaphores?             */
#endif /* PARS_USE_SYNCHRONISATION */

/** Use ThreadPooling for Multi thread. Check PARSP_ConfigThreadPool.h                   */
#ifndef PARS_USE_THREADPOOL
#  define PARS_USE_THREADPOOL                 PARS_CF_NO
#endif /* PARS_USE_THREADPOOL */

/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Tracing and Error Logging
 *
 * Configure here if you want to have trace at all
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_DEBUG_OUT
#  define PARS_DEBUG_OUT                      PARS_CF_YES             /**< Global Switch for Debug infos                  */
#endif /* PARS_DEBUG_OUT */
#ifndef PARS_MEMORY_TRACE
#  define PARS_MEMORY_TRACE                   PARS_CF_NO              /**< Enable Tracing of Memory Usage                 */
#endif /* PARS_MEMORY_TRACE */
#ifndef PARS_DEBUG_MODE_CALL_TRACE
#  define PARS_DEBUG_MODE_CALL_TRACE          PARS_CF_NO              /**< Switches on Function call tracing              */
#endif /* PARS_DEBUG_MODE_CALL_TRACE */
#ifndef PARS_MEMORY_TRACE_WINDOWS
#  define PARS_MEMORY_TRACE_WINDOWS           PARS_CF_NO              /**< Uses Windows Memory trace Hooks                */
#endif /* PARS_MEMORY_TRACE_WINDOWS */
#ifndef PARS_MEMORY_POOL_TRACE
#  define PARS_MEMORY_POOL_TRACE              PARS_CF_NO              /**< Debug infos of the memory pools                */
#endif /* PARS_MEMORY_POOL_TRACE */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Trace Application
 *
 * Detailed settings for the trace application
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#if PARS_DEBUG_OUT
#  ifndef PARS_TRACE_TO_FILE
#    define PARS_TRACE_TO_FILE                PARS_CF_NO              /**< Trace to File (debugout.txt)                   */
#  endif /* PARS_TRACE_TO_FILE */
#  ifndef PARS_TRACE_TO_CONSOLE
#    define PARS_TRACE_TO_CONSOLE             PARS_CF_YES             /**< Trace to Console if set                        */
#  endif /* PARS_TRACE_TO_CONSOLE */
#  ifndef PARS_TRACE_TO_TELNET
#    define PARS_TRACE_TO_TELNET              PARS_CF_YES             /**< Trace to Telnet Console if set                 */
#  endif /* PARS_TRACE_TO_TELNET */
#  ifndef PARS_TRACE_THREADID
#    define PARS_TRACE_THREADID               PARS_CF_YES             /**< Print ThreadId in console traces               */
#  endif /* PARS_TRACE_THREADID */
#  ifndef PARS_TRACE_WITH_TIME
#    define PARS_TRACE_WITH_TIME              PARS_CF_YES             /**< Add time info to the debug output              */
#  endif /* PARS_TRACE_WITH_TIME */
#  ifndef PARS_MAX_DEBUG_FILE_SIZE
#    define PARS_MAX_DEBUG_FILE_SIZE          10  PARS_CF_MEGA_BYTES  /**< Size of the log files                          */
#  endif /* PARS_MAX_DEBUG_FILE_SIZE */
#  ifndef PARS_TRACE_BUFFER_SIZE
#    define PARS_TRACE_BUFFER_SIZE            32  PARS_CF_KILO_BYTES  /**< 32 K Buffer for Traces                         */
#  endif /* PARS_TRACE_BUFFER_SIZE */
#  ifndef PARS_TRACEAPP_TELNET_PORT
#    define PARS_TRACEAPP_TELNET_PORT         5023                    /**< On which port should the telnet server listen  */
#  endif /* PARS_TRACEAPP_TELNET_PORT */
#  ifndef PARS_TRACE_TRACEUSER
#    define PARS_TRACE_TRACEUSER              "TraceUser"             /**< User for WWWRoot                               */
#  endif /* PARS_TRACE_TRACEUSER */
#  ifndef PARS_TRACE_APPNAME
#    define PARS_TRACE_APPNAME                "Trace"                 /**< Name of the application, used to save file log */
#  endif /* PARS_TRACE_APPNAME */
#  ifndef PARS_TRACE_FILE_PATH
#    define PARS_TRACE_FILE_PATH              "/Traces/XMLParser"     /**< Path and Base Name of the trace files          */
#  endif /* PARS_TRACE_FILE_PATH */
#  ifndef PARS_TRACE_FILE_EXTENSION
#    define PARS_TRACE_FILE_EXTENSION         ".log"                  /**< Extension of the trace files.                  */
#  endif /* PARS_TRACE_FILE_EXTENSION */

/** The path to the file containing all the facilities for the TraceApp */
#  ifndef PARS_TRACE_FACILITIES_PATH
#    define PARS_TRACE_FACILITIES_PATH        "/de/Trace/FacilityList.txt"
#  endif /* PARS_TRACE_FACILITIES_PATH */

/** The path to the file containing all the settings for the TraceApp */
#  ifndef PARS_TRACE_FACILITY_STORAGE_FILE
#    define PARS_TRACE_FACILITY_STORAGE_FILE  "TraceConfig.bin"
#  endif /* PARS_TRACE_FACILITY_STORAGE_FILE */

/** The path to the configuration template */
#  ifndef PARS_TRACE_CONFIG_TEMPLATE_PATH
#    define PARS_TRACE_CONFIG_TEMPLATE_PATH   "/de/Trace/ConfigurationTemplate.tmpl"
#  endif /* PARS_TRACE_CONFIG_TEMPLATE_PATH */

/** When using TELNET, this is the hello message */
#  ifndef PARS_TRACE_TELNET_WELCOME
#    define PARS_TRACE_TELNET_WELCOME         "== XMLParser Telnet Trace ==\r\n"
#  endif /* PARS_TRACE_TELNET_WELCOME */
#endif /* PARS_DEBUG_OUT */

#ifdef _DEBUG
#ifndef PARS_BREAK_ON_BAILOUT
#  define PARS_BREAK_ON_BAILOUT(Y)   PARS_DebugBreak(Y);
#endif /* PARS_BREAK_ON_BAILOUT */
#else /* _DEBUG */
#  define PARS_BREAK_ON_BAILOUT(Y)
#endif /* _DEBUG */

#ifndef PARS_CHECK_STACK_SIZE
#  define PARS_CHECK_STACK_SIZE               PARS_CF_NO              /**< Trace stack usage                              */
#endif /* PARS_CHECK_STACK_SIZE */

#ifndef PARS_INIT_GLOBALS_MANUALLY
#  define PARS_INIT_GLOBALS_MANUALLY          PARS_CF_YES             /**< initialize global vars, needed for restart!    */
#endif /* PARS_INIT_GLOBALS_MANUALLY */

/* Include Error Trace Macros ------------------------------------------------------------------------------------------- */
/* Attention, the Defines above modify the behavior of the PARS_ErrorCodes.h macros                                       */
#include "PARS_ErrorCodes.h"
/* End Include Error Trace Macros --------------------------------------------------------------------------------------- */

#ifndef PARS_TRACE_INITIAL_TRACELEVEL
#  define PARS_TRACE_INITIAL_TRACELEVEL       PARS_DEBUG_LEVEL_HINT   /**< Initial trace level                            */
#endif /* PARS_TRACE_INITIAL_TRACELEVEL */
/** Debug purpose: Check for illegal sockets. Lacks performance if set.     */
#ifndef PARS_DEBUG_FIND_ILLEGAL_SOCKETS
#  define PARS_DEBUG_FIND_ILLEGAL_SOCKETS     PARS_CF_NO
#endif /* PARS_DEBUG_FIND_ILLEGAL_SOCKETS */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Memory Management
 *
 * Memory related stuff goes here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_USE_MEM_POOL
#  define PARS_USE_MEM_POOL                   PARS_CF_NO             /**< Enable Mempool extension.                      */
#endif /* PARS_USE_MEM_POOL */
/* PARS_MEM_MALLOC_USE_POOLS is no longer existent, The SMemXXX Functions use always Pools.                               */
/* The MemXXX functions use direct Malloc                                                                                 */

#ifndef PARS_MEMORY_POOL_TEST
#  define PARS_MEMORY_POOL_TEST               PARS_CF_NO              /**< Test validity of Mempool, slow!                */
#endif /* PARS_MEMORY_POOL_TEST */

#if PARS_USE_MEM_POOL
/** shall (S)Malloc use Malloc if no pool space is available?           */
#  ifndef PARS_MEM_FALLBACK_TO_MALLOC
#    define PARS_MEM_FALLBACK_TO_MALLOC      PARS_CF_YES
#  endif /* PARS_MEM_FALLBACK_TO_MALLOC */
#endif /* PARS_USE_MEM_POOL */

/** Minimize dynamic Memory access, use more static memory instead.     */
#ifndef PARS_PREFER_STATIC_MEMORY
#  define PARS_PREFER_STATIC_MEMORY           PARS_CF_YES
#endif /* PARS_PREFER_STATIC_MEMORY */

#if PARS_USE_MEM_POOL
#  ifndef PARS_MEM_MEMPOOL_VARPROVIDER
#    define PARS_MEM_MEMPOOL_VARPROVIDER     PARS_CF_YES             /**< Should Memory pool offer a variable provider    */
#  endif /* PARS_MEM_MEMPOOL_VARPROVIDER */
#endif /* PARS_USE_MEM_POOL */

#ifndef PARS_MEM_MALLOC_POOLS_NUMBER
#  define PARS_MEM_MALLOC_POOLS_NUMBER        12                      /**< initialization of pools see PARSP_Memory.c     */
#endif /* PARS_MEM_MALLOC_POOLS_NUMBER */
#ifndef PARS_MEM_ALIGNMENT
#  define PARS_MEM_ALIGNMENT                  8                       /**< Alignment for struct members etc.              */
#endif /* PARS_MEM_ALIGNMENT */

/** Maximum Memory pool Name len incl. '\\0'. Must be 4 Bytes aligned */
#ifndef PARS_MEM_MEMPOOL_NAME_SIZE
#  define PARS_MEM_MEMPOOL_NAME_SIZE          64
#endif /* PARS_MEM_MEMPOOL_NAME_SIZE */

#ifndef PARS_MEM_MAX_MEMPOOLS
#  define PARS_MEM_MAX_MEMPOOLS               70                      /**< Maximum Memory pools in system                 */
#endif /* PARS_MEM_MAX_MEMPOOLS */
#ifndef PARS_MEM_MEMPOOL_ALIGNMENT
#  define PARS_MEM_MEMPOOL_ALIGNMENT          8                       /**< alignment of elements of MemPool               */
#endif /* PARS_MEM_MEMPOOL_ALIGNMENT */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Memory trace argument Handling
 *
 * Set up the format of the memory trace arguments here. You shouldn't have to change this!
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/**
 * @def     PARS_MEM_TRACE_ARGS_DEC
 * @brief   Additional parameters for trace messages when doing a memory trace
 * */
#if PARS_MEMORY_TRACE
#  ifndef PARS_MEM_TRACE_ARGS_DEC
#    define PARS_MEM_TRACE_ARGS_DEC        , PARS_INT a_nLineNumber, PARS_LPSTR   a_strFileName
#  endif /* PARS_MEM_TRACE_ARGS_DEC */
#else /* PARS_MEMORY_TRACE */
#  ifndef PARS_MEM_TRACE_ARGS_DEC
#    define PARS_MEM_TRACE_ARGS_DEC
#  endif /* PARS_MEM_TRACE_ARGS_DEC */
#endif /* PARS_MEMORY_TRACE */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Different core functionality.
 *
 * Configure a vast amount of details here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/

#ifndef PARS_IMPLEMENT_EXTENSION_IF
#  define PARS_IMPLEMENT_EXTENSION_IF         PARS_CF_NO              /**< use the ExtIf to couple with IIS or Proactor   */
#endif /* PARS_IMPLEMENT_EXTENSION_IF */

#ifndef PARS_EXTENSION_ASYNC_READ_EVENT
#  define PARS_EXTENSION_ASYNC_READ_EVENT     PARS_CF_NO        /**< Uncouples the simulated read event from call stack   */
#endif /* PARS_EXTENSION_ASYNC_READ_EVENT */

#ifndef PARS_IS_STANDALONE
#  define PARS_IS_STANDALONE                  PARS_CF_NO              /**< only set if xml parser is used as an extension    */
#endif /* PARS_IS_STANDALONE */

#ifndef PARS_IMPLEMENT_PREFILTER
#  define PARS_IMPLEMENT_PREFILTER            PARS_CF_NO              /**< enable Pre filter functionality                */
#endif /* PARS_IMPLEMENT_PREFILTER */
#ifndef PARS_IMPLEMENT_POSTFILTER
#  define PARS_IMPLEMENT_POSTFILTER           PARS_CF_NO              /**< enable Post filter functionality               */
#endif /* PARS_IMPLEMENT_POSTFILTER */

#ifndef PARS_USES_EXTERNAL_SYSTEM_ROOT
#  define PARS_USES_EXTERNAL_SYSTEM_ROOT      PARS_CF_YES             /**< Is System root in the File system              */
#endif /* PARS_USES_EXTERNAL_SYSTEM_ROOT */

#ifndef PARS_STRING_TESTSUITE
#  define PARS_STRING_TESTSUITE               PARS_CF_NO              /**< Compile and run testSuite                      */
#endif /* PARS_STRING_TESTSUITE */

#ifndef PARS_LOADER_SUPP_WEBSERVICES
#  define PARS_LOADER_SUPP_WEBSERVICES        PARS_CF_YES             /**< Must be set to support WebServices             */
#endif /* PARS_LOADER_SUPP_WEBSERVICES */

#ifndef PARS_IMPL_URL_PARAM_HANDLING
#  define PARS_IMPL_URL_PARAM_HANDLING        PARS_CF_NO              /**< add URL Handling to the ECB                    */
#endif /* PARS_IMPL_URL_PARAM_HANDLING */

#ifndef PARS_USE_A_FILESYSTEM
#  define PARS_USE_A_FILESYSTEM               PARS_CF_YES             /**< Should at least one file sys used?             */
#endif /* PARS_USE_A_FILESYSTEM */

#ifndef PARS_CORE_USES_CONVERSION
#  define PARS_CORE_USES_CONVERSION           PARS_CF_YES             /**< Core uses conversion for URL decoding          */
#endif /* PARS_CORE_USES_CONVERSION */

#ifndef PARS_HAVE_A_DLL_SYSTEM
#  define PARS_HAVE_A_DLL_SYSTEM              PARS_CF_YES             /**< Do we use DLL system?                          */
#endif /* PARS_HAVE_A_DLL_SYSTEM */
#ifndef PARS_CORE_USES_AUTHENTICATION
#  define PARS_CORE_USES_AUTHENTICATION       PARS_CF_YES             /**< Core and DefApp use Authentication             */
#endif /* PARS_CORE_USES_AUTHENTICATION */

#ifndef PARS_HTTP_STRICT_VERB_CHECK
#  define PARS_HTTP_STRICT_VERB_CHECK         PARS_CF_YES             /**< If set,only known Verbs are allowed            */
#endif /* PARS_HTTP_STRICT_VERB_CHECK */
#ifndef PARS_CORE_USES_DEFAPPSERVICE
#  define PARS_CORE_USES_DEFAPPSERVICE        PARS_CF_YES             /**< DefappService used for Error Codes             */
#endif /* PARS_CORE_USES_DEFAPPSERVICE */
/** should Core automatically reply to "Expect: 100-continue" for all applications, when not set, applications can do (and
 * have to do) 100 continue processing of their own. */
#ifndef PARS_CORE_AUTOMATIC_100_CONTINUE
#  define PARS_CORE_AUTOMATIC_100_CONTINUE    PARS_CF_YES
#endif /* PARS_CORE_AUTOMATIC_100_CONTINUE */
#ifndef PARS_CORE_ENABLE_CHUNKED
#  define PARS_CORE_ENABLE_CHUNKED            PARS_CF_YES             /**< enable chunked data mechanism                  */
#endif /* PARS_CORE_ENABLE_CHUNKED */
#ifndef PARS_CORE_AUTOMATIC_CHUNKED
#  define PARS_CORE_AUTOMATIC_CHUNKED         PARS_CF_YES             /**< Send chunked when no Content Length            */
#endif /* PARS_CORE_AUTOMATIC_CHUNKED */

#ifndef PARS_SUPPORT_WEBCFG
#  define PARS_SUPPORT_WEBCFG                 PARS_CF_YES             /**< Enable WebCfg.xml as configuration base        */
#endif /* PARS_SUPPORT_WEBCFG */

#ifndef PARS_USE_SHORT_ERRORCODES
#  define PARS_USE_SHORT_ERRORCODES           PARS_CF_YES             /**< no error page:use Status line instead          */
#endif /* PARS_USE_SHORT_ERRORCODES */

#ifndef PARS_DEFINES_64_BIT_DATATYPES
#  define PARS_DEFINES_64_BIT_DATATYPES      PARS_CF_NO               /**< not supported yet                              */
#endif /* PARS_DEFINES_64_BIT_DATATYPES */

/** automatic map Request URL from UTF8 to ASCII with the default CodePage          */
#ifndef PARS_AUTOMATIC_UTF8_URL_DECODING
#  define PARS_AUTOMATIC_UTF8_URL_DECODING    PARS_CF_NO
#endif /* PARS_AUTOMATIC_UTF8_URL_DECODING */

#ifndef PARS_DEFINES_64_BIT_DATETIME
#  define PARS_DEFINES_64_BIT_DATETIME        PARS_CF_NO           /**< just the definition, no handling code implemented */
#endif /* PARS_DEFINES_64_BIT_DATETIME */

#ifndef PARS_IPV6_SUPPORT
#  define PARS_IPV6_SUPPORT                   PARS_CF_NO              /**< ENABLE/DISABLE IPV6                            */
#endif /* PARS_IPV6_SUPPORT */

#ifndef PARS_USE_WIDE_CHAR
#  define PARS_USE_WIDE_CHAR                  PARS_CF_NO              /**< can xml parser handle wide chars?                 */
#endif /* PARS_USE_WIDE_CHAR */

/** enable write access, when no authorization is used.                             */
#ifndef PARS_CORE_ALWAYS_WRITEACCESS
#  define PARS_CORE_ALWAYS_WRITEACCESS        PARS_CF_NO
#endif /* PARS_CORE_ALWAYS_WRITEACCESS */

#ifndef PARS_USE_SETSYSTEMTIME
#  define PARS_USE_SETSYSTEMTIME              PARS_CF_NO              /**< Can xml parser set the system time (PORTLAYER!)   */
#endif /* PARS_USE_SETSYSTEMTIME */

#ifndef PARS_RAND_MAX
#  define PARS_RAND_MAX                       0x7fff                  /**< RAND_MAX for WINDOWS is 0x7fff                 */
#endif /* PARS_RAND_MAX */

/* --- Core Memory Sizes                                                                                              --- */
#ifndef PARS_MCB_MAX_MODULE_COUNT
#  define PARS_MCB_MAX_MODULE_COUNT           100                     /**< How many Modules xml parser supports              */
#endif /* PARS_MCB_MAX_MODULE_COUNT */

#ifndef PARS_MCB_MAX_NESTED_LOOKUPS
#  define PARS_MCB_MAX_NESTED_LOOKUPS         20                      /**< Avoid cyclic HTTP_Lookups                      */
#endif /* PARS_MCB_MAX_NESTED_LOOKUPS */
#ifndef PARS_MCB_MAX_LOOKUP_CALLBACKS
#  define PARS_MCB_MAX_LOOKUP_CALLBACKS       10                      /**< Amount of outstanding inform callbacks         */
#endif /* PARS_MCB_MAX_LOOKUP_CALLBACKS */

#ifndef PARS_CORE_MAX_PREAMBLE_LENGTH
#  define PARS_CORE_MAX_PREAMBLE_LENGTH       16                     /**< Maximum length of a preamble, don't change this */
#endif /* PARS_CORE_MAX_PREAMBLE_LENGTH */

#ifndef PARS_ADMIN_USER
#  define PARS_ADMIN_USER                     "Administrator"         /**< User name of the Administrator User            */
#endif /* PARS_ADMIN_USER */

/**
 * @def PARS_MAX_HEADER_LEN
 * @brief  Max header length allowed
 * @note   If you use .NET clients with WebServices pay attention to the header length!
 *         It will exceed the PARS_MAX_HEADER_LEN of 1500 because .NET includes its debug info (3kb)
 *         in header of the request if its a debug build!!!
 */
#ifndef PARS_MAX_HEADER_LEN
#  define PARS_MAX_HEADER_LEN                 8   PARS_CF_KILO_BYTES
#endif /* PARS_MAX_HEADER_LEN */

#ifndef PARS_MAX_URL_LEN
#  define PARS_MAX_URL_LEN                    512 PARS_CF_BYTES       /**< Max len of URL: 512 Bytes suggested by RFC 2616. */
#endif /* PARS_MAX_URL_LEN */
#ifndef PARS_MAX_NAME_LEN
#  define PARS_MAX_NAME_LEN                   60  PARS_CF_BYTES       /**< Username, Password, Realm size, 4 Byte aligned.  */
#endif /* PARS_MAX_NAME_LEN */

/** Size of the Scratch buffer in ECB must <= PARS_FILES_MAX_PATH_LENGTH            */
#ifndef PARS_MAX_SCRATCH_BUFFER
#  define PARS_MAX_SCRATCH_BUFFER             PARS_MAX_URL_LEN
#endif /* PARS_MAX_SCRATCH_BUFFER */

/* --- Several other communication settings                                                                           --- */

#ifndef PARS_HTTP_TIMEOUT
#  define PARS_HTTP_TIMEOUT                   20  PARS_CF_SECONDS     /**< Timeout for Wait on read etc. (s)              */
#endif /* PARS_HTTP_TIMEOUT */
#ifndef PARS_CONNECTION_TIMEOUT
#  define PARS_CONNECTION_TIMEOUT             50  PARS_CF_SECONDS     /**< Max Time to receive New Request(s)             */
#endif /* PARS_CONNECTION_TIMEOUT */
#ifndef PARS_SOCKIF_SELECT_TIMEOUT
#  define PARS_SOCKIF_SELECT_TIMEOUT          1   PARS_CF_SECOND      /**< Max wait time for sending (ms)                 */
#endif /* PARS_SOCKIF_SELECT_TIMEOUT */

#ifndef PARS_DEFAULT_STACK_SIZE
#  define PARS_DEFAULT_STACK_SIZE             50  PARS_CF_KILO_BYTES  /**< Stack Size for new Threads                     */
#endif /* PARS_DEFAULT_STACK_SIZE */

#if PARS_MTHREAD
  /**
   * max Socket lists (correspondents to max Communication threads) for multithreaded environment.
   * Defaults are
   * - 64 for Windows and
   * - 16 for Linux
   */
#  ifndef PARS_MAX_SOCKET_LISTS
#    define PARS_MAX_SOCKET_LISTS             64
#  endif /* PARS_MAX_SOCKET_LISTS */
#else  /* PARS_MTHREAD */
#  ifndef PARS_MAX_SOCKET_LISTS
#    define PARS_MAX_SOCKET_LISTS             1                       /**< 1 socket list in single thread                 */
#  endif /* PARS_MAX_SOCKET_LISTS */
#endif /* PARS_MTHREAD */

/**
 * Max web server connections. Defaults are
 * - 32 for Windows and
 * - 16 for Linux
 **/
#ifndef PARS_MAX_HTTP_CONNECTIONS
#  define PARS_MAX_HTTP_CONNECTIONS           32
#endif /* PARS_MAX_HTTP_CONNECTIONS */

#ifndef PARS_MAX_HTTPC_CONNECTIONS
#  define PARS_MAX_HTTPC_CONNECTIONS          10                      /**< Max HTTP client connections                    */
#endif /* PARS_MAX_HTTPC_CONNECTIONS */

#ifndef PARS_MCB_MAX_SHUTDOWNCBS
#  define PARS_MCB_MAX_SHUTDOWNCBS            3                       /**< How many modules can sign an early inform cb   */
#endif /* PARS_MCB_MAX_SHUTDOWNCBS */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Socket IF
 *
 * Detailed setup for the socket Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_SOCKET_USE_MULTICAST
#  define PARS_SOCKET_USE_MULTICAST           PARS_CF_YES             /**< Should Multi cast Sockets be used              */
#endif /* PARS_SOCKET_USE_MULTICAST */
#ifndef PARS_SOCKET_USE_CLIENT_SOCKET
#  define PARS_SOCKET_USE_CLIENT_SOCKET       PARS_CF_YES             /**< Should Client Socket be supported              */
#endif /* PARS_SOCKET_USE_CLIENT_SOCKET */
#ifndef PARS_SOCKET_USES_EXTENDED
#  define PARS_SOCKET_USES_EXTENDED           PARS_CF_YES             /**< Support extended Socket functions              */
#endif /* PARS_SOCKET_USES_EXTENDED */

/**
 * @brief   Max socket lists. Defaults are
 *          - 64 for Windows and
 *          - 16 for Linux
 *
 * @note    In multi threaded environment every connection use a socket list.
 *
 * @note    The number entered here must not be greater than PARS_SOCK_SOCKET_ARR_SIZE!
 */
#ifndef PARS_MAX_SOCKET_CONNECTIONS
#  define PARS_MAX_SOCKET_CONNECTIONS         64                      /**< Max Value is: PARS_SOCK_SOCKET_ARR_SIZE        */
#endif /* PARS_MAX_SOCKET_CONNECTIONS */

#ifndef PARS_MAXIMUM_LOOP_TIME
#  define PARS_MAXIMUM_LOOP_TIME              2 PARS_CF_SECONDS       /**< Maximum wait time for Sockets or other events. */
#endif /* PARS_MAXIMUM_LOOP_TIME */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name ListMgmt Defines
 *
 * Configuration settings for the LIST Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_LISTMGMT_MAX_LIST_ENTRIES
#  define PARS_LISTMGMT_MAX_LIST_ENTRIES      2000                    /**< used if PARS_USE_MEM_POOL is set               */
#endif /* PARS_LISTMGMT_MAX_LIST_ENTRIES */
#ifndef PARS_LISTMGMT_MAX_LISTS
#  define PARS_LISTMGMT_MAX_LISTS             100                     /**< used if PARS_USE_MEM_POOL is set               */
#endif /* PARS_LISTMGMT_MAX_LISTS */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Timer Defines
 *
 * Configuration settings of the TIMER Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_TIMER_MAX_TIMER_PER_SLOT
#  define PARS_TIMER_MAX_TIMER_PER_SLOT       500                     /**< Maximum timer which arise at the same time     */
#endif /* PARS_TIMER_MAX_TIMER_PER_SLOT */

/** used if PARS_USE_MEM_POOL is set. OPC-XML needs 250 Timers! */
#ifndef PARS_TIMER_MAX_TIMER
#  define PARS_TIMER_MAX_TIMER                5000
#endif /* PARS_TIMER_MAX_TIMER */

#ifndef PARS_TIMER_TICKCOUNT_SIZE
#  define PARS_TIMER_TICKCOUNT_SIZE           0xffffffffl             /**< Maximum value for tick count                   */
#endif /* PARS_TIMER_TICKCOUNT_SIZE */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Event Service
 *
 * Configuration settings fo the EVENT Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_EVENT_MAX_ENTRIES
#  define PARS_EVENT_MAX_ENTRIES              50                      /* Maximum Events and Subscribers                   */
#endif /* PARS_EVENT_MAX_ENTRIES */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name License Service
 *
 * Configuration settings fo the LICENSE Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_LIC_MANAGEMENT
#  define PARS_LIC_MANAGEMENT                 PARS_CF_YES             /**< Activate License Management                    */
#endif /* PARS_LIC_MANAGEMENT */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Secure Socket Layer (SSL)
 *
 * Configuration settings fo the SSL Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_USE_SSL
#  define PARS_USE_SSL                            PARS_CF_YES         /**< Enable SSL Support                             */
#endif /* PARS_USE_SSL */
#ifndef PARS_SSL_SUPPORT_TLS_1_1
#  define PARS_SSL_SUPPORT_TLS_1_1                PARS_CF_NO          /**< TLS1.1 SHOULD work, TLS1.0 DOES work           */
#endif /* PARS_SSL_SUPPORT_TLS_1_1 */
#ifndef PARS_MAX_CA_STORE_SIZE
#  define PARS_MAX_CA_STORE_SIZE                  8
#endif /* PARS_MAX_CA_STORE_SIZE */
#ifndef PARS_MAX_CLIENTCERT_STORE_SIZE
#  define PARS_MAX_CLIENTCERT_STORE_SIZE          8
#endif /* PARS_MAX_CLIENTCERT_STORE_SIZE */
#ifndef PARS_SSL_USE_GENERIC_CERTIFICATES
#  define PARS_SSL_USE_GENERIC_CERTIFICATES       PARS_CF_YES         /**< use Generic Certs if the are none              */
#endif /* PARS_SSL_USE_GENERIC_CERTIFICATES */
#ifndef PARS_SSL_USE_CERT_CHAIN
#  define PARS_SSL_USE_CERT_CHAIN                 PARS_CF_YES         /**< Enable CERT CHAIN Support                      */
#endif /* PARS_SSL_USE_CERT_CHAIN */
#ifndef PARS_SSL_ACCEPT_EMPTY_SSL_RECORDS
#  define PARS_SSL_ACCEPT_EMPTY_SSL_RECORDS       PARS_CF_YES         /**< Accept empty SSL User data Records             */
#endif /* PARS_SSL_ACCEPT_EMPTY_SSL_RECORDS */
#ifndef PARS_SSL_MAX_CERT_CHAIN_DEPTH
#  define PARS_SSL_MAX_CERT_CHAIN_DEPTH           5                   /**< Max depth of the cert chain                    */
#endif /* PARS_SSL_MAX_CERT_CHAIN_DEPTH */

/** must be PARS_NULL or "1", PARS_NULL=ca derived cert, 1=selfsigned  */
#ifndef PARS_SSL_GEN_CERT_B_SELF
#  define PARS_SSL_GEN_CERT_B_SELF                PARS_NULL
#endif /* PARS_SSL_GEN_CERT_B_SELF */
#ifndef PARS_CERT_NOTSELFSIGNED_DURATION_YEARS
#  define PARS_CERT_NOTSELFSIGNED_DURATION_YEARS  1
#endif /* PARS_CERT_NOTSELFSIGNED_DURATION_YEARS */
#ifndef PARS_CERT_SELFSIGNED_DURATION_YEARS
#  define PARS_CERT_SELFSIGNED_DURATION_YEARS     10
#endif /* PARS_CERT_SELFSIGNED_DURATION_YEARS */

#ifndef PARS_SSL_GEN_CERT_TXT_C
#  define PARS_SSL_GEN_CERT_TXT_C                 "country"           /**< country for auto generated certificate         */
#endif /* PARS_SSL_GEN_CERT_TXT_C */
#ifndef PARS_SSL_GEN_CERT_TXT_P
#  define PARS_SSL_GEN_CERT_TXT_P                 "province"          /**< province for auto generated certificate        */
#endif /* PARS_SSL_GEN_CERT_TXT_P */
#ifndef PARS_SSL_GEN_CERT_TXT_L
#  define PARS_SSL_GEN_CERT_TXT_L                 "locality"          /**< locality for auto generated certificate        */
#endif /* PARS_SSL_GEN_CERT_TXT_L */
#ifndef PARS_SSL_GEN_CERT_TXT_O
#  define PARS_SSL_GEN_CERT_TXT_O                 "Siemens AG"        /**< organization for auto generated certificate    */
#endif /* PARS_SSL_GEN_CERT_TXT_O */
#ifndef PARS_SSL_GEN_CERT_TXT_OU
#  define PARS_SSL_GEN_CERT_TXT_OU                "Industry"          /**< org. unit for auto generated certificate       */
#endif /* PARS_SSL_GEN_CERT_TXT_OU */
#ifndef PARS_SSL_GEN_CERT_TXT_E
#  define PARS_SSL_GEN_CERT_TXT_E                 "email"             /**< email for auto generated certificate           */
#endif /* PARS_SSL_GEN_CERT_TXT_E */
#ifndef PARS_SSL_GEN_CERT_TXT_CN
#  define PARS_SSL_GEN_CERT_TXT_CN                "common_name"       /**< common_name for auto generated certificate     */
#endif /* PARS_SSL_GEN_CERT_TXT_CN */
#ifndef PARS_SSL_GEN_CERT_TXT_PWD
#  define PARS_SSL_GEN_CERT_TXT_PWD               "password"          /**< password for auto generated certificate        */
#endif /* PARS_SSL_GEN_CERT_TXT_PWD */

/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name HTTP Core Defines
 *
 * Configuration of the HTTP Core
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/** The max number of bytes the IP-stack can handle in one write. */
#ifndef PARS_HTTP_TRANSFER_BUFFER
#  define PARS_HTTP_TRANSFER_BUFFER           32  PARS_CF_KILO_BYTES  /**< Amount of Bytes sent per Block                 */
#endif /* PARS_HTTP_TRANSFER_BUFFER */
/**
 * Amount of Bytes that can be buffered by WriteClientEx.
 * This doesn't mean that this buffer always get's allocated.
 * This is just the max size of content that could be buffered.
 * Since this buffer does the decoupling of consumer and the sockets, this buffer should be reasonably large.
 * */
#ifndef PARS_HTTP_MAX_TEMP_SEND_BUFFER
#  define PARS_HTTP_MAX_TEMP_SEND_BUFFER      1   PARS_CF_MEGA_BYTES
#endif /* PARS_HTTP_MAX_TEMP_SEND_BUFFER */
#ifndef PARS_HTTP_VERBOSE_TRACE_OUTPUT
#  define PARS_HTTP_VERBOSE_TRACE_OUTPUT      PARS_CF_NO              /**< Produces lots of trace about every sent buffer */
#endif /* PARS_HTTP_VERBOSE_TRACE_OUTPUT */
#ifndef PARS_MAX_TRACE_LEN
#  define PARS_MAX_TRACE_LEN                  1 PARS_CF_KILO_BYTES    /**< Produces lots of trace about every sent buffer */
#endif /* PARS_HTTP_VERBOSE_TRACE_OUTPUT */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name DefApp Defines
 *
 * Configuration of the Default Application. Be very careful with changing these values because these switches can really
 * blow up the whole thing!
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/

#ifndef PARS_USE_LAST_MODIFIED
#  define PARS_USE_LAST_MODIFIED              PARS_CF_YES             /**< Add last modified header.                      */
#endif /* PARS_USE_LAST_MODIFIED */
#ifndef PARS_DEFAP_DIR_BROWSING
#  define PARS_DEFAP_DIR_BROWSING             PARS_CF_YES             /**< Include Directory Browsing Feature             */
#endif /* PARS_DEFAP_DIR_BROWSING */
#ifndef PARS_DEFAP_EARLY_WEB_CFG_COMMIT
#  define PARS_DEFAP_EARLY_WEB_CFG_COMMIT     PARS_CF_YES             /**< Immediate persist WebCfg changes               */
#endif /* PARS_DEFAP_EARLY_WEB_CFG_COMMIT */
#ifndef PARS_DEFAP_VERBOSE_UPLOADS
#  define PARS_DEFAP_VERBOSE_UPLOADS          PARS_CF_NO              /**< Verbose Log information on uploads             */
#endif /* PARS_DEFAP_VERBOSE_UPLOADS */
#ifndef PARS_USES_LANGFILTER
#  define PARS_USES_LANGFILTER                PARS_CF_YES             /**< File browsing with language filter             */
#endif /* PARS_USES_LANGFILTER */
#ifndef PARS_DEFAPP_TINY
#  define PARS_DEFAPP_TINY                    PARS_CF_NO              /**< Don't use WebCfg for file access               */
#endif /* PARS_DEFAPP_TINY */
#ifndef PARS_DEFAPP_EXTERNAL_TAG
#  define PARS_DEFAPP_EXTERNAL_TAG            "EXTERNAL"              /**< External reference for Files in WebCfg.xml     */
#endif /* PARS_DEFAPP_EXTERNAL_TAG */

/**
 * Which pattern is used by file sys to get all entries from a directory. Set this to
 * - "*.*" under Windows and to
 * - "" under Linux
 */
#ifndef PARS_FILES_ALLFILE_PATTERN
#  define PARS_FILES_ALLFILE_PATTERN          "*.*"
#endif /* PARS_FILES_ALLFILE_PATTERN */
#ifndef PARS_DEFAP_DEFAULT_URL
#  define PARS_DEFAP_DEFAULT_URL              "/DefApp"               /**< URL of Default application                     */
#endif /* PARS_DEFAP_DEFAULT_URL */

/* --- DEFAPP Service parameters                                                                                      --- */
/* Parameters for the filter table mechanism, corresponds to AddTextUsingFilter()                                         */
#ifndef PARS_DEFAP_FLTRTBL_MAX_ENTRIES
#  define PARS_DEFAP_FLTRTBL_MAX_ENTRIES      15                      /**< Maximum entries in a filter table              */
#endif /* PARS_DEFAP_FLTRTBL_MAX_ENTRIES */
#ifndef PARS_DEFAP_FLTRTBL_MAX_KEY_LEN
#  define PARS_DEFAP_FLTRTBL_MAX_KEY_LEN      PARS_MAX_NAME_LEN       /**< Maximum Length of Filter Key                   */
#endif /* PARS_DEFAP_FLTRTBL_MAX_KEY_LEN */
#ifndef PARS_DEFAP_FLTRTBL_MAX_VALUE_LEN
#  define PARS_DEFAP_FLTRTBL_MAX_VALUE_LEN    PARS_MAX_URL_LEN        /**< Max Length of Filter Value                     */
#endif /* PARS_DEFAP_FLTRTBL_MAX_VALUE_LEN */

/* Parameters for Web Config File handling:                                                           */
#ifndef PARS_DEFAP_WEBCFG_COMMIT_TIME
#  define PARS_DEFAP_WEBCFG_COMMIT_TIME       10  PARS_CF_SECONDS     /**< Time to commit changes of WebCfg, 0 for off.   */
#endif /* PARS_DEFAP_WEBCFG_COMMIT_TIME */
#ifndef PARS_DEFAP_MAXIMUM_WEBCFG_SIZE
#  define PARS_DEFAP_MAXIMUM_WEBCFG_SIZE      100 PARS_CF_KILO_BYTES  /**< Max size of a new WebCfg uploaded by a client. */
#endif /* PARS_DEFAP_MAXIMUM_WEBCFG_SIZE */

/* Parameters for DefApps memory Pools, corresponds with simultaneous requests:                       */
#ifndef PARS_DEFAP_MAX_SIMUL_UPLOADS
#  define PARS_DEFAP_MAX_SIMUL_UPLOADS        3                       /**< Maximum of simultaneous Uploads                */
#endif /* PARS_DEFAP_MAX_SIMUL_UPLOADS */
#ifndef PARS_DEFAP_MAX_SIMUL_DOWNLOADS
#  define PARS_DEFAP_MAX_SIMUL_DOWNLOADS      10                      /**< Maximum of simultaneous Uploads                */
#endif /* PARS_DEFAP_MAX_SIMUL_DOWNLOADS */
#ifndef PARS_DEFAP_MAX_SIMUL_DIRBROWSES
#  define PARS_DEFAP_MAX_SIMUL_DIRBROWSES     3                       /**< Maximum of simultaneous Dir Browses            */
#endif /* PARS_DEFAP_MAX_SIMUL_DIRBROWSES */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name FileSys defines
 *
 * Configure the FILE service here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_FILES_FILESYS_USE_WWWROOT
#  define PARS_FILES_FILESYS_USE_WWWROOT      PARS_CF_YES             /**< Should WWWRoot be used                         */
#endif /* PARS_FILES_FILESYS_USE_WWWROOT */

/** First occurrence of local link terminates the search in XML File base                       */
#ifndef PARS_FILESYS_FIRST_LOCALLINK_HITS
#  define PARS_FILESYS_FIRST_LOCALLINK_HITS   PARS_CF_NO
#endif /* PARS_FILESYS_FIRST_LOCALLINK_HITS */

/** Support of *, ? in xml-Findfirst a must for WebDav. Forces file sys to use VarIF!           */
#ifndef PARS_XMLFILESYS_PATTERN_MATCHING
#  define PARS_XMLFILESYS_PATTERN_MATCHING    PARS_CF_YES
#endif /* PARS_XMLFILESYS_PATTERN_MATCHING */

/** Use XML file sys case insensitive. Use this, if your extern file sys is case insensitive    */
#ifndef PARS_FILESYS_XML_IGNORE_CASE
#  define PARS_FILESYS_XML_IGNORE_CASE        PARS_CF_NO
#endif /* PARS_FILESYS_XML_IGNORE_CASE */

#ifndef PARS_PROTECT_HIDDEN_FILES
#  define PARS_PROTECT_HIDDEN_FILES           PARS_CF_YES             /**< No access to (extern) hidden files             */
#endif /* PARS_PROTECT_HIDDEN_FILES */
#ifndef PARS_FILESYS_HAVE_XML_FILESYS
#  define PARS_FILESYS_HAVE_XML_FILESYS       PARS_CF_YES             /**< Use the transparent XML file sys.              */
#endif /* PARS_FILESYS_HAVE_XML_FILESYS */
#ifndef PARS_FILESYS_WRITEBABLE_WEBCFG
#  define PARS_FILESYS_WRITEBABLE_WEBCFG      PARS_CF_YES             /**< Write access to XML file sys                   */
#endif /* PARS_FILESYS_WRITEBABLE_WEBCFG */

#ifndef PARS_FILES_MAX_PATH_LENGTH
#  define PARS_FILES_MAX_PATH_LENGTH          260 PARS_CF_BYTES       /**< Max length of local file path                  */
#endif /* PARS_FILES_MAX_PATH_LENGTH */
#ifndef PARS_FILES_MAX_FILESYSTEM_SIZE
#  define PARS_FILES_MAX_FILESYSTEM_SIZE      0xffffffffL             /**< Maximum Bytes free on Filesystem               */
#endif /* PARS_FILES_MAX_FILESYSTEM_SIZE */

/* --- the following is only used when PARS_USE_MEM_POOL is used                                  --- */
#ifndef PARS_FILES_MAX_FILE_HANDLES
#  define PARS_FILES_MAX_FILE_HANDLES         20                      /**< Maximum simultaneous open files                */
#endif /* PARS_FILES_MAX_FILE_HANDLES */
#ifndef PARS_FILES_MAX_FIND_HANDLES
#  define PARS_FILES_MAX_FIND_HANDLES         10                      /**< Maximum simultaneous file browses              */
#endif /* PARS_FILES_MAX_FIND_HANDLES */

/** Realm a user must have to access hidden files in external file sys.                         */
#ifndef PARS_FILES_ALLOWED_REALM
#  define PARS_FILES_ALLOWED_REALM            "ATS_KUNDE"
#endif /* PARS_FILES_ALLOWED_REALM */

#ifndef PARS_FILES_USE_ASSERTS
#  define PARS_FILES_USE_ASSERTS              PARS_CF_NO              /**< Assert of file not found etc.                  */
#endif /* PARS_FILES_USE_ASSERTS */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name External Sys Call
 *
 * The CGI relevant stuff gets configured here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_ENABLE_EXTERNAL_TOOLS
#  define PARS_ENABLE_EXTERNAL_TOOLS          PARS_CF_YES             /**< Enable EXEC for CGI Application                */
#endif /* PARS_ENABLE_EXTERNAL_TOOLS */
#ifndef PARS_CGI_MWSL_EXTENSION
#  define PARS_CGI_MWSL_EXTENSION             PARS_CF_YES             /**< Enable MWSL CGI Gateway                        */
#endif /* PARS_CGI_MWSL_EXTENSION */

#ifndef PARS_CGI_MWSL_FUNCTION_NAME
#  define PARS_CGI_MWSL_FUNCTION_NAME         "ExecCGI"               /**< Name of the CGI Function                       */
#endif /* PARS_CGI_MWSL_FUNCTION_NAME */
#ifndef PARS_CGI_SUSPEND_TIME
#  define PARS_CGI_SUSPEND_TIME               250                     /**< Timeout for CGI Calls                          */
#endif /* PARS_CGI_SUSPEND_TIME */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name XML Parser
 *
 * Configuration of the XML service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_XML_VERBOSE_PARSING
#  define PARS_XML_VERBOSE_PARSING            PARS_CF_NO              /**< Output Parsing process information             */
#endif /* PARS_XML_VERBOSE_PARSING */

#ifndef PARS_XML_ENABLE_XPATH
#  define PARS_XML_ENABLE_XPATH               PARS_CF_NO              /**< Enable XPath interpreter for xml variables     */
#endif /* PARS_XML_ENABLE_XPATH */

/* --- SAX is an event triggered stream able XML parser extension, not used by the core                               --- */
#ifndef PARS_XML_ENABLE_SXPARSER
#  define PARS_XML_ENABLE_SXPARSER            PARS_CF_NO              /**< Should the sax parser included                 */
#endif /* PARS_XML_ENABLE_SXPARSER */

/* --- The writer is used to write an xml document with simple calls to fwrite, so it saves your memory               --- */
#ifndef PARS_XML_ENABLE_XMLWRITER
#  define PARS_XML_ENABLE_XMLWRITER           PARS_CF_YES              /**< Should the xml writer be included              */
#endif /* PARS_XML_ENABLE_XMLWRITER */

/* --- the following is only used when PARS_USE_MEM_POOL is used                                                      --- */
#ifndef PARS_XML_MAX_NODES_IN_NODESET
#  define PARS_XML_MAX_NODES_IN_NODESET       1000                    /**< 1 pointer per entry!                           */
#endif /* PARS_XML_MAX_NODES_IN_NODESET */

#if PARS_USE_MEM_POOL
#ifndef PARS_XML_MAX_ATTRIBUTES
  #  define PARS_XML_MAX_ATTRIBUTES           60000                   /**< max number of attributes                       */
#endif /* PARS_XML_MAX_ATTRIBUTES */
#ifndef PARS_XML_MAX_NODES
  #  define PARS_XML_MAX_NODES                40000                   /**< max number of nodes                            */
#endif /* PARS_XML_MAX_NODES */
#ifndef PARS_XML_MAX_SHEETS
  #  define PARS_XML_MAX_SHEETS               32                      /**< max number of sheets                           */
#endif /* PARS_XML_MAX_SHEETS */
#endif /* PARS_USE_MEM_POOL */

#ifndef PARS_XML_MAX_TEMP_BUFFER_SIZE
#  define PARS_XML_MAX_TEMP_BUFFER_SIZE       32 PARS_CF_KILO_BYTES   /**< Page size for XML document gen.                */
#endif /* PARS_XML_MAX_TEMP_BUFFER_SIZE */

#ifndef PARS_XML_DEFAULT_INDENT
#  define PARS_XML_DEFAULT_INDENT             2                       /**< Default Indent for XML Text formatting         */
#endif /* PARS_XML_DEFAULT_INDENT */
#ifndef PARS_XML_TABSIZE
#  define PARS_XML_TABSIZE                    2                       /**< Number of spaces a TAB is expanded to.         */
#endif /* PARS_XML_TABSIZE */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Stream Service
 *
 * Configuration of the STREAM Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_STRM_EXPANDFAKTOR
#  define PARS_STRM_EXPANDFAKTOR              100                     /**< Factor x/100,of expanding a stream             */
#endif /* PARS_STRM_EXPANDFAKTOR */
#ifndef PARS_STRM_MINSIZE
#  define PARS_STRM_MINSIZE                   (1 PARS_CF_KILO_BYTES)  /**< Minimum size for first expand.                 */
#endif /* PARS_STRM_MINSIZE */
#ifndef PARS_STREAM_MAX_PRINTF_BUFFER
#  define PARS_STREAM_MAX_PRINTF_BUFFER       512                     /**< Maximum Length for the printf buffer           */
#endif /* PARS_STREAM_MAX_PRINTF_BUFFER */
#ifndef PARS_STRM_TRACE_USAGE
#  define PARS_STRM_TRACE_USAGE               PARS_CF_NO              /**< print information about stream memory usage    */
#endif /* PARS_STRM_TRACE_USAGE */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Conversion Service
 *
 * Configuration of the CONVERSION Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_CONV_TEST_SERVICE
#  define PARS_CONV_TEST_SERVICE              PARS_CF_NO              /**< Call self test of the service                  */
#endif /* PARS_CONV_TEST_SERVICE */
#ifndef PARS_CONV_USE_CODEPAGE
#  define PARS_CONV_USE_CODEPAGE              PARS_CF_NO              /**< Code page Usage (needs XML Parser)             */
#endif /* PARS_CONV_USE_CODEPAGE */
#ifndef PARS_CONV_DECODE_PLUS
#  define PARS_CONV_DECODE_PLUS               PARS_CF_YES             /**< Convert '+' to space in URL decoding           */
#endif /* PARS_CONV_DECODE_PLUS */
#ifndef PARS_CONVS_MAX_CODEPAGE_COUNT
#  define PARS_CONVS_MAX_CODEPAGE_COUNT       2                       /**< Maximum count of code pages used               */
#endif /* PARS_CONVS_MAX_CODEPAGE_COUNT */
#ifndef PARS_CONVS_MAX_CP_NAME_LENGTH
#  define PARS_CONVS_MAX_CP_NAME_LENGTH       16                      /**< 15 Chars + '\\0', must be >=16                 */
#endif /* PARS_CONVS_MAX_CP_NAME_LENGTH */
#ifndef PARS_CONV_UTC_STR_LEN
#  define PARS_CONV_UTC_STR_LEN               30                      /**< Maximum length of an UTC time string           */
#endif /* PARS_CONV_UTC_STR_LEN */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Variable Service
 *
 * Settings for the (deprecated) VARIF Service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_VARIF_USES_EVENTIF
#  define PARS_VARIF_USES_EVENTIF              PARS_CF_NO             /**< Variable Service use Event Service             */
#endif /* PARS_VARIF_USES_EVENTIF */
#ifndef PARS_VARIF_AUTOMATIC_PROVIDER_LOOKUP
#  define PARS_VARIF_AUTOMATIC_PROVIDER_LOOKUP PARS_CF_YES            /**< Should the VarIF lookup the provider handles?  */
#endif /* PARS_VARIF_AUTOMATIC_PROVIDER_LOOKUP */

/* --- the following is only used when PARS_USE_MEM_POOL is used                                                      --- */
/** Maximum count of Providers e.g.the Compliance Test uses by default 10 Connections * 10 Subscriptions = 100 + 10
 *  parallel writes         */
#ifndef PARS_VARIF_MAX_PROVIDER_ENTRIES
#  define PARS_VARIF_MAX_PROVIDER_ENTRIES     10
#endif /* PARS_VARIF_MAX_PROVIDER_ENTRIES */

#ifndef PARS_VARIF_MACHINE_SIZE
#  define PARS_VARIF_MACHINE_SIZE             4                       /**< How many bytes has a machine word              */
#endif /* PARS_VARIF_MACHINE_SIZE */
#ifndef PARS_VARIF_FLOAT_MANTISSA_DIGITS
#  define PARS_VARIF_FLOAT_MANTISSA_DIGITS    7                       /**< IEEE754: 32Bit Real = 7 Precision              */
#endif /* PARS_VARIF_FLOAT_MANTISSA_DIGITS */
#ifndef PARS_VARIF_DOUBLE_MANTISSA_DIGITS
#  define PARS_VARIF_DOUBLE_MANTISSA_DIGITS   15                      /**<          64Bit Real = 15 Precision             */
#endif /* PARS_VARIF_DOUBLE_MANTISSA_DIGITS */
#ifndef PARS_VARIF_DECIMAL_PRECISION
#  define PARS_VARIF_DECIMAL_PRECISION        4                       /**< OPC has 4 Digits decimal precision             */
#endif /* PARS_VARIF_DECIMAL_PRECISION */
#ifndef PARS_VARIF_MAX_TRANSPORT_ELEMENTS
#  define PARS_VARIF_MAX_TRANSPORT_ELEMENTS   1000                    /**< Maximum variables transported                  */
#endif /* PARS_VARIF_MAX_TRANSPORT_ELEMENTS */
#ifndef PARS_VARIF_MAX_TRANSPORTS
#  define PARS_VARIF_MAX_TRANSPORTS           500                     /**< Maximum Transport structs                      */
#endif /* PARS_VARIF_MAX_TRANSPORTS */
#ifndef PARS_VARIF_MAX_STRING_LEN
#  define PARS_VARIF_MAX_STRING_LEN           255                     /**< Maximum len of a string                        */
#endif /* PARS_VARIF_MAX_STRING_LEN */

/* Provider Name length is define due PARS_MAX_NAME_LEN, see above.                                                       */

/* special chars for pattern matching */
#ifndef PARS_VARIF_PATTERN_DIGIT
#  define PARS_VARIF_PATTERN_DIGIT               '#'
#endif /* PARS_VARIF_PATTERN_DIGIT */
#ifndef PARS_VARIF_PATTERN_ANY_CHAR
#  define PARS_VARIF_PATTERN_ANY_CHAR            '?'
#endif /* PARS_VARIF_PATTERN_ANY_CHAR */
#ifndef PARS_VARIF_PATTERN_CHAR_SET_SEQUENCE
#  define PARS_VARIF_PATTERN_CHAR_SET_SEQUENCE   '*'
#endif /* PARS_VARIF_PATTERN_CHAR_SET_SEQUENCE */
#ifndef PARS_VARIF_PATTERN_CHAR_SET_START
#  define PARS_VARIF_PATTERN_CHAR_SET_START      '['
#endif /* PARS_VARIF_PATTERN_CHAR_SET_START */
#ifndef PARS_VARIF_PATTERN_CHAR_SET_END
#  define PARS_VARIF_PATTERN_CHAR_SET_END        ']'
#endif /* PARS_VARIF_PATTERN_CHAR_SET_END */
#ifndef PARS_VARIF_PATTERN_CHAR_SET_NOT
#  define PARS_VARIF_PATTERN_CHAR_SET_NOT        '!'
#endif /* PARS_VARIF_PATTERN_CHAR_SET_NOT */
#ifndef PARS_VARIF_PATTERN_CHAR_SET_RANGE
#  define PARS_VARIF_PATTERN_CHAR_SET_RANGE      '-'
#endif /* PARS_VARIF_PATTERN_CHAR_SET_RANGE */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Sample provider
 *
 * Settings for the SAMPLE PROVIDER of OPC UA
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_SMPL_PRVDR_MAX_VARIABLES
#  define PARS_SMPL_PRVDR_MAX_VARIABLES       30
#endif /* PARS_SMPL_PRVDR_MAX_VARIABLES */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name HTTP Service
 *
 * The module that handles the HTTP protocol
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_HTSRVC_IGNORE_UNEXPECTED_SP
#  define PARS_HTSRVC_IGNORE_UNEXPECTED_SP    PARS_CF_NO              /**< ignore unexpected subparts during upload       */
#endif /* PARS_HTSRVC_IGNORE_UNEXPECTED_SP */
#ifndef PARS_HTSRVC_USE_CONVERSION
#  define PARS_HTSRVC_USE_CONVERSION          PARS_CF_YES             /**< Use Conv. Service to decode URLs               */
#endif /* PARS_HTSRVC_USE_CONVERSION */

#ifndef PARS_HTSRV_MAX_GLOBAL_IDS
#  define PARS_HTSRV_MAX_GLOBAL_IDS           300                     /**< Maximum count of global ids.                   */
#endif /* PARS_HTSRV_MAX_GLOBAL_IDS */
#ifndef PARS_GUID_STRING_SIZE
#  define PARS_GUID_STRING_SIZE               37                      /**< Buffer size of a global id.                    */
#endif /* PARS_GUID_STRING_SIZE */
#ifndef PARS_HTSRV_MAX_UPLOADS
#  define PARS_HTSRV_MAX_UPLOADS              10                      /**< Maximum parallel uploads.                      */
#endif /* PARS_HTSRV_MAX_UPLOADS */
#ifndef PARS_LANGUAGE_COOKIE_NAME
#  define PARS_LANGUAGE_COOKIE_NAME           "siemens_automation_language"
#endif /* PARS_LANGUAGE_COOKIE_NAME */
#ifndef PARS_LANGFLTR_LONGEST_PREFIX_MATCH
#  define PARS_LANGFLTR_LONGEST_PREFIX_MATCH  PARS_CF_YES
#endif /* PARS_LANGFLTR_LONGEST_PREFIX_MATCH */

/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name MWSL
 *
 * Configuration for the legacy MWSL(1) service
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_MWSL_IMPLEMENT_FILTER
#  define PARS_MWSL_IMPLEMENT_FILTER          PARS_CF_YES             /**< Use MWSL as Filter                             */
#endif /* PARS_MWSL_IMPLEMENT_FILTER */
#ifndef PARS_MWSL_USE_TEMPLATES
#  define PARS_MWSL_USE_TEMPLATES             PARS_CF_YES             /**< Enable Template Processing                     */
#endif /* PARS_MWSL_USE_TEMPLATES */
#ifndef PARS_MWSL_ERROR_OUTPUT
#  define PARS_MWSL_ERROR_OUTPUT              PARS_CF_YES             /**< Write Script errors to output                  */
#endif /* PARS_MWSL_ERROR_OUTPUT */
#ifndef MWSL_APP_SUPPORT_POST
#  define MWSL_APP_SUPPORT_POST               PARS_CF_YES             /**< Process MWSL App Post requests                 */
#endif /* MWSL_APP_SUPPORT_POST */
#ifndef PARS_MWSL_OWN_EXTENSIONS
#  define PARS_MWSL_OWN_EXTENSIONS            PARS_CF_YES             /**< enable non ecmascript extensions               */
#endif /* PARS_MWSL_OWN_EXTENSIONS */

#ifndef PARS_MWSL_MAX_CONTEXTS
#  define PARS_MWSL_MAX_CONTEXTS              10                      /**< Max parallel MWSL Contexts                     */
#endif /* PARS_MWSL_MAX_CONTEXTS */
#ifndef PARS_MWSL_MAX_VAR_CACHE_ENTRIES
#  define PARS_MWSL_MAX_VAR_CACHE_ENTRIES     20                      /**< Max Varcache entries per request               */
#endif /* PARS_MWSL_MAX_VAR_CACHE_ENTRIES */
#ifndef PARS_MWSL_MAX_VAR_CACHE_ENTRY_SIZE
#  define PARS_MWSL_MAX_VAR_CACHE_ENTRY_SIZE  50                      /**< Max Varname / Value Len (+1)                   */
#endif /* PARS_MWSL_MAX_VAR_CACHE_ENTRY_SIZE */

#ifndef PARS_MWSL_SCRIPT_MAX_FUNCTIONS
#  define PARS_MWSL_SCRIPT_MAX_FUNCTIONS      50                      /**< ! Functions                                    */
#endif /* PARS_MWSL_SCRIPT_MAX_FUNCTIONS */
#ifndef PARS_MWSL_MAX_CONTEXT_FUNCTIONS
#  define PARS_MWSL_MAX_CONTEXT_FUNCTIONS     5                       /**< Context sensitive Functions                    */
#endif /* PARS_MWSL_MAX_CONTEXT_FUNCTIONS */

#ifndef PARS_MWSL_SYMBOLS_PER_BLOCK
#  define PARS_MWSL_SYMBOLS_PER_BLOCK         10
#endif /* PARS_MWSL_SYMBOLS_PER_BLOCK */
#ifndef PARS_MWSL_MAX_SYMTABS
#  define PARS_MWSL_MAX_SYMTABS               100                     /**< Max count of simult. symbol tables             */
#endif /* PARS_MWSL_MAX_SYMTABS */
#ifndef PARS_MWSL_MAX_ARGUMENTS
#  define PARS_MWSL_MAX_ARGUMENTS             10                      /**< Max count of args in a funct. call             */
#endif /* PARS_MWSL_MAX_ARGUMENTS */

#ifndef PARS_MWSL_MAX_TEMPLATE_FRAGMENTS
#  define PARS_MWSL_MAX_TEMPLATE_FRAGMENTS    100                     /**< Max Fragments in a template file               */
#endif /* PARS_MWSL_MAX_TEMPLATE_FRAGMENTS */
#ifndef PARS_MWSL_MAX_TEMPLATES
#  define PARS_MWSL_MAX_TEMPLATES             20                      /**< Max Templates in a Template file               */
#endif /* PARS_MWSL_MAX_TEMPLATES */

#ifndef PARS_MWSL_MAX_LITERAL_LEN
#  define PARS_MWSL_MAX_LITERAL_LEN           4   PARS_CF_KILO_BYTES  /**< Init. output size for MWSL Script              */
#endif /* PARS_MWSL_MAX_LITERAL_LEN */
#ifndef PARS_MWSL_INIT_OUTPUT_SIZE
#  define PARS_MWSL_INIT_OUTPUT_SIZE          32  PARS_CF_KILO_BYTES  /**< Init. output size for MWSL Script              */
#endif /* PARS_MWSL_INIT_OUTPUT_SIZE */
#ifndef PARS_MWSL_MAX_VARSOURCE_COUNT
#  define PARS_MWSL_MAX_VARSOURCE_COUNT       10                      /**< Max Var Sources (MWSL need 3!)                 */
#endif /* PARS_MWSL_MAX_VARSOURCE_COUNT */

#ifndef PARS_MWSL_NUMBER_TYPE
#  define PARS_MWSL_NUMBER_TYPE               PARS_VT_R4              /**< Type for Script Numbers                        */
#endif /* PARS_MWSL_NUMBER_TYPE */
#ifndef PARS_MWSL_NUMBER
#  define PARS_MWSL_NUMBER                    PARS_FLOAT              /**< Type for Script Numbers                        */
#endif /* PARS_MWSL_NUMBER */
#ifndef PARS_MWSL_LPNUMBER
#  define PARS_MWSL_LPNUMBER                  PARS_LPFLOAT            /**< Type for Script Numbers                        */
#endif /* PARS_MWSL_LPNUMBER */
#ifndef PARS_MWSL_SCRIPT_MAX_LOOP_COUNT
#  define PARS_MWSL_SCRIPT_MAX_LOOP_COUNT     100                     /**< Maximum Loop Depth                             */
#endif /* PARS_MWSL_SCRIPT_MAX_LOOP_COUNT */
#ifndef MWSL_MAX_STATEMENTS_PER_BLOCK
#  define MWSL_MAX_STATEMENTS_PER_BLOCK       2                       /**< Force Thread change after X States.            */
#endif /* MWSL_MAX_STATEMENTS_PER_BLOCK */

#ifndef PARS_MWSL_MAX_POST_PARAMETER
#  define PARS_MWSL_MAX_POST_PARAMETER        10                      /**< how many post parameter for POST VarSource     */
#endif /* PARS_MWSL_MAX_POST_PARAMETER */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name MWSL2
 *
 * Configuration of the successor of MWSL
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/

#ifndef PARS_MWSL2_MAX_TEMPLATES
#  define PARS_MWSL2_MAX_TEMPLATES            100                      /**< Maximum count of simultaneusly used templates */
#endif /* PARS_MWSL2_MAX_TEMPLATES */

#ifndef PARS_MWSL2_MAX_POST_PARAMETER
#  define PARS_MWSL2_MAX_POST_PARAMETER       20                          /**< Maximum handled post parameter per request */
#endif /* PARS_MWSL2_MAX_POST_PARAMETER */

#ifndef PARS_MWSL2_MAX_ARGUMENTS
#  define PARS_MWSL2_MAX_ARGUMENTS            10                      /**< Max count of args in a funct. call             */
#endif /* PARS_MWSL2_MAX_ARGUMENTS */

#ifndef PARS_MWSL2_MAX_LITERAL_LEN
#  define PARS_MWSL2_MAX_LITERAL_LEN          (4 PARS_CF_KILO_BYTES)          /**< Maximum length for strings and buffers */
#endif /* PARS_MWSL2_MAX_LITERAL_LEN */
#ifndef PARS_MWSL2_MAX_SYMBOLS_PER_TABLE
#  define PARS_MWSL2_MAX_SYMBOLS_PER_TABLE    50                                  /**< Maximum count of symbols per scope */
#endif /* PARS_MWSL2_MAX_SYMBOLS_PER_TABLE */
#ifndef PARS_MWSL2_MAX_FUNCTIONS
#  define PARS_MWSL2_MAX_FUNCTIONS            40                            /**< Maximum count of context local functions */
#endif /* PARS_MWSL2_MAX_FUNCTIONS */
#ifndef PARS_MWSL2_MAX_GLOBAL_FUNCTIONS
#  define PARS_MWSL2_MAX_GLOBAL_FUNCTIONS     50                            /**< Maximum count of context local functions */
#endif /* PARS_MWSL2_MAX_GLOBAL_FUNCTIONS */
#ifndef PARS_MWSL2_MAX_SCRIPT_FUNCTIONS
#  define PARS_MWSL2_MAX_SCRIPT_FUNCTIONS     20                        /**< Maximum allowed script function in a context */
#endif /* PARS_MWSL2_MAX_SCRIPT_FUNCTIONS */
#ifndef PARS_MWSL2_MAX_VARSOURCES
#  define PARS_MWSL2_MAX_VARSOURCES           10                        /**< Maximum count of all variable sources        */
#endif /* PARS_MWSL2_MAX_VARSOURCES */
#ifndef PARS_MWSL2_MAX_SOURCE_LINES
#  define PARS_MWSL2_MAX_SOURCE_LINES         1000          /**< Only for debug, so we get source lines at runtime errors */
#endif /* PARS_MWSL2_MAX_SOURCE_LINES */
#ifndef PARS_MWSL2_MAX_ARRAY_ELEMENTS
#  define PARS_MWSL2_MAX_ARRAY_ELEMENTS       10                                                  /**< Max size of Arrays */
#endif /* PARS_MWSL2_MAX_ARRAY_ELEMENTS */

#ifndef PARS_MWSL2_MAX_TEMPL_RT_CTX
#  define PARS_MWSL2_MAX_TEMPL_RT_CTX         10                 /**< Max count of simultaneous Template runtime contexts */
#endif /* PARS_MWSL2_MAX_TEMPL_RT_CTX */

#ifndef PARS_MWSL2_MAX_TEMPL_COMPILE_CTX
#  define PARS_MWSL2_MAX_TEMPL_COMPILE_CTX    2                  /**< Max count of simultaneous Template compile contexts */
#endif /* PARS_MWSL2_MAX_TEMPL_COMPILE_CTX */

#ifndef PARS_MWSL2_MAX_PAGE_CTX
#  define PARS_MWSL2_MAX_PAGE_CTX             10                    /**< Max count of simultaneous MWSL2 runtime contexts */
#endif /* PARS_MWSL2_MAX_PAGE_CTX */
#ifndef PARS_MWSL2_DENY_TEMPL_RECURSION
#  define PARS_MWSL2_DENY_TEMPL_RECURSION     PARS_CF_NO                          /**< Prohibit Template within Templates */
#endif /* PARS_MWSL2_DENY_TEMPL_RECURSION */
#ifndef PARS_MWSL2_SUPPORT_VARIF
#  define PARS_MWSL2_SUPPORT_VARIF            PARS_CF_YES                        /**< This enables the var source PROCESS */
#endif /* PARS_MWSL2_SUPPORT_VARIF */
#ifndef PARS_MWSL2_USE_COMPILEDFILE
#  define PARS_MWSL2_USE_COMPILEDFILE         PARS_CF_YES                     /**< write the compiled script and reuse it */
#endif /* PARS_MWSL2_USE_COMPILEDFILE */
#ifndef PARS_MWSL2_USE_COMPILATION
#  define PARS_MWSL2_USE_COMPILATION          PARS_CF_YES                               /**< only look for compiled files */
#endif /* PARS_MWSL2_USE_COMPILATION */
#ifndef PARS_MWSL2_USE_TEMPLATES
#  define PARS_MWSL2_USE_TEMPLATES            PARS_CF_YES                                 /**< include template mechanism */
#endif /* PARS_MWSL2_USE_TEMPLATES */
#ifndef PARS_MWSL2_SUPPORT_DATAIF
#  define PARS_MWSL2_SUPPORT_DATAIF           PARS_CF_YES                        /**< This enables the var source PROCESS */
#endif /* PARS_MWSL2_SUPPORT_DATAIF */
#ifndef PARS_MWSL2_USE_AS_APPLICATION
#  define PARS_MWSL2_USE_AS_APPLICATION       PARS_CF_YES                      /**< Enable automatic processing of .mwsl  *
                                                                                 *  Files via HTTP.                       */
#endif /* PARS_MWSL2_USE_AS_APPLICATION */
#ifndef PARS_MWSL2_PUSHED_OUTPUT
#  define PARS_MWSL2_PUSHED_OUTPUT           PARS_CF_YES           /**< Immediate send produced data or collect full page */
#endif /* PARS_MWSL2_PUSHED_OUTPUT */

#ifndef PARS_MWSL2_NUMBER_TYPE
#  define PARS_MWSL2_NUMBER_TYPE              PARS_VT_R4                            /**< value type of the native numbers */
#endif /* PARS_MWSL2_NUMBER_TYPE */
#ifndef PARS_MWSL2_NUMBER
#  define PARS_MWSL2_NUMBER                   PARS_FLOAT                            /**< native type of number variables */
#endif /* PARS_MWSL2_NUMBER */
#ifndef PARS_MWSL2_LPNUMBER
#  define PARS_MWSL2_LPNUMBER                 PARS_LPFLOAT                           /**< native type of number variables */
#endif /* PARS_MWSL2_LPNUMBER */

#ifndef PARS_MWSL2_VERBOSE_COMPILE
#  define PARS_MWSL2_VERBOSE_COMPILE          PARS_CF_YES                        /**< Generate and write compiler logfile */
#endif /* PARS_MWSL2_VERBOSE_COMPILE */
#ifndef PARS_MWSL2_SRC_LEVEL_DEBUG
#  define PARS_MWSL2_SRC_LEVEL_DEBUG          PARS_CF_YES                       /**< write references to source positions */
#endif /* PARS_MWSL2_SRC_LEVEL_DEBUG */

#ifndef PARS_MWSL2_MAX_LABELS
#  define PARS_MWSL2_MAX_LABELS               5000                          /**< how many labels can be used in a context */
#endif /* PARS_MWSL2_MAX_LABELS */
#ifndef PARS_MWSL2_MAX_FRAGMENTS
#  define PARS_MWSL2_MAX_FRAGMENTS            5000                                /**< How many fragments can a page have */
#endif /* PARS_MWSL2_MAX_FRAGMENTS */

#ifndef PARS_MWSL2_MAX_VALUE_STACK_SIZE
#  define PARS_MWSL2_MAX_VALUE_STACK_SIZE     10                                        /**< Maximum depth of value stack */
#endif /* PARS_MWSL2_MAX_VALUE_STACK_SIZE */

#ifndef PARS_MWSL2_AVERAGE_OUTPUT_SIZE
#  define PARS_MWSL2_AVERAGE_OUTPUT_SIZE     (4 PARS_CF_KILO_BYTES)                   /**< Average size of an output page */
#endif /* PARS_MWSL2_AVERAGE_OUTPUT_SIZE */

#ifndef PARS_MWSL2_FILVER_MAGIC
#  define PARS_MWSL2_FILVER_MAGIC             0xC3                           /**< To double check MWSL compiled file type */
#endif /* PARS_MWSL2_FILVER_MAGIC */

#ifndef PARS_MWSL2_FILVER_FIX
#  define PARS_MWSL2_FILVER_FIX               3                                                 /**< Version of compiler! */
#endif /* PARS_MWSL2_FILVER_FIX */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Authentication applications
 *
 * Settings for several authentication modules
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/

/* --- Don't change the values below ------------------------------------------------------------------------------------ */

#ifndef PARS_AUTH_ALGO_DIGEST
/** DON'T CHANGE THIS VALUE! */
#  define PARS_AUTH_ALGO_DIGEST               "DigestAuthentication"
#endif /* PARS_AUTH_ALGO_DIGEST */
#ifndef PARS_AUTH_ALGO_BASIC
/** DON'T CHANGE THIS VALUE! */
#  define PARS_AUTH_ALGO_BASIC                "BasicAuthentication"
#endif /* PARS_AUTH_ALGO_BASIC */
#ifndef PARS_AUTH_ALGO_FORMULA
/** DON'T CHANGE THIS VALUE! */
#  define PARS_AUTH_ALGO_FORMULA              "FormulaAuthentication"
#endif /* PARS_AUTH_ALGO_FORMULA */
#ifndef PARS_AUTH_ALGO_CERTIFICATE
/** DON'T CHANGE THIS VALUE! */
#  define PARS_AUTH_ALGO_CERTIFICATE          "CertificateAuthentication"
#endif /* PARS_AUTH_ALGO_CERTIFICATE */

/* --- Don't change the values above ------------------------------------------------------------------------------------ */

#ifndef PARS_AUTH_BLOCK_PASSWD_BRUTE_FORCE
#  define PARS_AUTH_BLOCK_PASSWD_BRUTE_FORCE  PARS_CF_YES           /**< Detect Password Bruteforce attacks               */
#endif /* PARS_AUTH_BLOCK_PASSWD_BRUTE_FORCE */

#ifndef PARS_AUTH_SUPPORT_SESSION
#  define PARS_AUTH_SUPPORT_SESSION           PARS_CF_NO            /**< Session ID and logouts                           */
#endif /* PARS_AUTH_SUPPORT_SESSION */
#ifndef PARS_AUTH_SEND_COOKIE_AS_HTTP
#  define PARS_AUTH_SEND_COOKIE_AS_HTTP       PARS_CF_YES           /**< set set-cookie header with cred.                 */
#endif /* PARS_AUTH_SEND_COOKIE_AS_HTTP */
#ifndef PARS_AUTH_USE_HASH
#  define PARS_AUTH_USE_HASH                  PARS_CF_YES           /**< Secure Login cookie with a hash (needs openssl)  */
#endif /* PARS_AUTH_USE_HASH */
#ifndef PARS_AUTH_STORE_PLAIN_PASSWORD
#  define PARS_AUTH_STORE_PLAIN_PASSWORD      PARS_CF_YES           /**< Hash Passwords even in Basic Auth                */
#endif /* PARS_AUTH_STORE_PLAIN_PASSWORD */
#ifndef PARS_AUTH_DIG_HANDLE_DOMAIN
#  define PARS_AUTH_DIG_HANDLE_DOMAIN         PARS_CF_YES           /**< AuthDig handles Names with domain                */
#endif /* PARS_AUTH_DIG_HANDLE_DOMAIN */

#ifndef PARS_AUTH_MAX_AUTH_METHODS
#  define PARS_AUTH_MAX_AUTH_METHODS          10                    /**< Maximum count of Auth Methods                    */
#endif /* PARS_AUTH_MAX_AUTH_METHODS */

/** Which algorithm is the default algorithm. When using Cert authentication: set fallback here     */
#ifndef PARS_AUTH_DEFAULT_AUTH_ALGO
#  define PARS_AUTH_DEFAULT_AUTH_ALGO         PARS_AUTH_ALGO_FORMULA
#endif /* PARS_AUTH_DEFAULT_AUTH_ALGO */
#ifndef PARS_DEFAP_DEFAULT_UPLOAD_AUTH_ALGO
#  define PARS_DEFAP_DEFAULT_UPLOAD_AUTH_ALGO PARS_AUTH_ALGO_DIGEST /**< Algorithm used for uploads                       */
#endif /* PARS_DEFAP_DEFAULT_UPLOAD_AUTH_ALGO */

#ifndef PARS_AUTH_LoginTimerElapse
#  define PARS_AUTH_LoginTimerElapse          1   PARS_CF_SECOND    /**< Time to simulate successful login                */
#endif /* PARS_AUTH_LoginTimerElapse */
#ifndef PARS_AUTH_MAX_FAILED_LOGINS
#  define PARS_AUTH_MAX_FAILED_LOGINS         10                    /**< Max Failed Logins to Block(max 15)               */
#endif /* PARS_AUTH_MAX_FAILED_LOGINS */
#ifndef PARS_AUTH_MAX_GROUPS
#  define PARS_AUTH_MAX_GROUPS                50                    /**< groups available in XML parser                   */
#endif /* PARS_AUTH_MAX_GROUPS */
#ifndef PARS_AUTH_MAX_SESSIONS
#  define PARS_AUTH_MAX_SESSIONS              10                    /**< Max simultaneous sessions                        */
#endif /* PARS_AUTH_MAX_SESSIONS */
#ifndef PARS_AUTH_FORM_COOKIENAME
#  define PARS_AUTH_FORM_COOKIENAME           "siemens_ad_session"  /**< Cookie name for form based login                 */
#endif /* PARS_AUTH_FORM_COOKIENAME */
#ifndef PARS_AUTH_FORM_STR_LOGOFF_LOCATION
#  define PARS_AUTH_FORM_STR_LOGOFF_LOCATION            "/Portal/FormLogout.mwsl"
#endif /* PARS_AUTH_FORM_STR_LOGOFF_LOCATION */
#ifndef PARS_AUTH_DIGEST_STR_LOGOFF_LOCATION
#  define PARS_AUTH_DIGEST_STR_LOGOFF_LOCATION            "/Portal/FormLogout.mwsl"
#endif /* PARS_AUTH_DIGEST_STR_LOGOFF_LOCATION */

#ifndef PARS_AUTH_FORM_SIM_ASYNC_AUTH
#  define PARS_AUTH_FORM_SIM_ASYNC_AUTH       PARS_CF_NO
#endif /* PARS_AUTH_FORM_SIM_ASYNC_AUTH */

#ifndef PARS_AUTH_FORM_DEFAULT_LOGIN_PATH
#  define PARS_AUTH_FORM_DEFAULT_LOGIN_PATH   "/FormLogin.mwsl"     /**< Default URL to login page                        */
#endif /* PARS_AUTH_FORM_DEFAULT_LOGIN_PATH */
#ifndef PARS_AUTH_FORM_DEFAULT_RESPONSE
#define PARS_AUTH_FORM_DEFAULT_RESPONSE     "<HTML>\r\n" \
                                            "  <head>\r\n" \
                                            "    <meta http-equiv='refresh' content='0; URL=%=Redirection%'>\r\n" \
                                            "  </head>\r\n" \
                                            "  <body>\r\n" \
                                            "    <p><a href='%=Redirection%'>Click here, when you're not automatically redirected</a></p>\r\n" \
                                            "  </body\r\n" \
                                            "</HTML>\r\n"

#endif /* PARS_AUTH_FORM_DEFAULT_RESPONSE */

#ifndef PARS_AUTH_FORM_COOKIE_USED_HASH
#  define PARS_AUTH_FORM_COOKIE_USED_HASH     "SHA1"                 /**< hash algorithm to use to secure cookie           */
#endif /* PARS_AUTH_FORM_COOKIE_USED_HASH */

#ifndef PARS_AUTH_FORM_HASH_LEN
#  define PARS_AUTH_FORM_HASH_LEN             20                    /**< corresponds to the used hash algorithm           */
#endif /* PARS_AUTH_FORM_HASH_LEN */

#ifndef PARS_AUTH_FORM_COOKIE_MAGIC
#  define PARS_AUTH_FORM_COOKIE_MAGIC         0xD37FA1C3
#endif /* PARS_AUTH_FORM_COOKIE_MAGIC */

#ifndef PARS_AUTH_FORM_NONCE_LEN
#  define PARS_AUTH_FORM_NONCE_LEN            18
#endif /* PARS_AUTH_FORM_NONCE_LEN */

#ifndef PARS_AUTH_FORM_BASEURL
#  define PARS_AUTH_FORM_BASEURL              "/FormLogin"          /**< Url to access FormAuth application               */
#endif /* PARS_AUTH_FORM_BASEURL */

#ifndef PARS_AUTH_CERT_STR_LOGOFF_LOCATION
#  define PARS_AUTH_CERT_STR_LOGOFF_LOCATION            "/Portal/FormLogout.mwsl"
#endif /* PARS_AUTH_CERT_STR_LOGOFF_LOCATION */

#ifndef PARS_AUTH_GROUP_SEPARATOR
#  define PARS_AUTH_GROUP_SEPARATOR           ','
#endif /* PARS_AUTH_GROUP_SEPARATOR */

#ifndef PARS_AUTH_DIGEST_MAX_NONCE_AGE
#  define PARS_AUTH_DIGEST_MAX_NONCE_AGE      10 PARS_CF_HOURS
#endif /* PARS_AUTH_DIGEST_MAX_NONCE_AGE */

#ifndef PARS_AUTH_DIGEST_HASH_LEN
#  define PARS_AUTH_DIGEST_HASH_LEN           16
#endif /* PARS_AUTH_DIGEST_HASH_LEN */
#ifndef PARS_AUTH_DIGEST_HEX_HASH_LEN
#  define PARS_AUTH_DIGEST_HEX_HASH_LEN       2*PARS_AUTH_DIGEST_HASH_LEN+1
#endif /* PARS_AUTH_DIGEST_HEX_HASH_LEN */
#ifndef PARS_AUTH_DIGEST_RANDOM_LEN
#  define PARS_AUTH_DIGEST_RANDOM_LEN         16
#endif /* PARS_AUTH_DIGEST_RANDOM_LEN */
/** Enable callback for login information, this is a risky feature, cause it can be used to spy for credential
 * information, so it should be set to PARS_CF_NO if it will not explicit be used */
#ifndef PARS_AUTH_ENABLE_LOGIN_CB
#  define PARS_AUTH_ENABLE_LOGIN_CB           PARS_CF_NO
#endif /* PARS_AUTH_ENABLE_LOGIN_CB */
#ifndef PARS_AUTH_S7
#  define PARS_AUTH_S7                        PARS_CF_NO        /**< enable S7 md5 password hashing                       */

#ifndef PARS_AUTH_SPNEGO_STR_LOGOFF_LOCATION
#  define PARS_AUTH_SPNEGO_STR_LOGOFF_LOCATION  "/Portal/SPNEGOLogout.mwsl"
#endif /* PARS_AUTH_SPNEGO_STR_LOGOFF_LOCATION */

#ifndef PARS_SSOC_STR_LOGOFF_LOCATION
#  define PARS_SSOC_STR_LOGOFF_LOCATION       "/Portal/SSOCLogout.mwsl"
#endif /* PARS_SSOC_STR_LOGOFF_LOCATION */

#ifndef PARS_SSOC_STR_LOGOFF_LOCATION
#  define PARS_SSOC_STR_LOGOFF_LOCATION       "/Portal/SSOCLogout.mwsl"
#endif /* PARS_SSOC_STR_LOGOFF_LOCATION */

#ifndef PARS_AUTH_SPNEGO_STR_LOGOFF_LOCATION
#  define PARS_AUTH_SPNEGO_STR_LOGOFF_LOCATION      "/Portal/SPNEGOLogout.mwsl"
#endif /* PARS_AUTH_SPNEGO_STR_LOGOFF_LOCATION */

#endif /* PARS_AUTH_S7 */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name XML User Database Service
 *
 * Configure the user database service here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_XUDB_SUPPORT_DVP
#  define PARS_XUDB_SUPPORT_DVP               PARS_CF_NO            /**< Inform DVP Service about user database changes   */
#endif /* PARS_XUDB_SUPPORT_DVP */
#ifndef PARS_XUDB_SIM_ASYNC_AUTH
#  define PARS_XUDB_SIM_ASYNC_AUTH            PARS_CF_NO            /**< Simulate Asynchronous authentication             */
#endif /* PARS_XUDB_SIM_ASYNC_AUTH */

#ifndef PARS_XUDB_USERDATABASEFILENAME
#  define PARS_XUDB_USERDATABASEFILENAME      "UserDataBase.xml"    /**< Default filename of user database                */
#endif /* PARS_XUDB_USERDATABASEFILENAME */

#ifndef PARS_XUDB_FILEBUFFERSIZE
#  define PARS_XUDB_FILEBUFFERSIZE            PARS_HTTP_TRANSFER_BUFFER
#endif /* PARS_XUDB_FILEBUFFERSIZE */
#ifndef PARS_XUDB_MAX_USER_ENTRIES
#  define PARS_XUDB_MAX_USER_ENTRIES          100
#endif /* PARS_XUDB_MAX_USER_ENTRIES */
#ifndef PARS_XUDB_MAX_GROUP_MAP_ENTRIES
#  define PARS_XUDB_MAX_GROUP_MAP_ENTRIES     10                    /**< Map entries to map external to xml parser groups */
#endif /* PARS_XUDB_MAX_GROUP_MAP_ENTRIES */
#ifndef PARS_AUTH_MAX_MAPPED_GROUPS
#  define PARS_AUTH_MAX_MAPPED_GROUPS         10                    /**< count of internal groups per external group.     */
#endif /* PARS_AUTH_MAX_MAPPED_GROUPS */
#ifndef PARS_XUDB_MAX_GROUPS
#  define PARS_XUDB_MAX_GROUPS                100
#endif /* PARS_XUDB_MAX_GROUPS */
#ifndef PARS_XUDB_MAX_USER_HANDLES
#  define PARS_XUDB_MAX_USER_HANDLES          30                    /**< Maximum simultaneous user sessions               */
#endif /* PARS_XUDB_MAX_USER_HANDLES */
#ifndef PARS_XUDB_A1_LEN
#  define PARS_XUDB_A1_LEN                    33                    /**< Size of A1 in Bytes                              */
#endif /* PARS_XUDB_A1_LEN */

#ifndef PARS_XUDB_STRING_ANONYMOUS
#  define PARS_XUDB_STRING_ANONYMOUS          "Anonymous"
#endif /* PARS_XUDB_STRING_ANONYMOUS */

#ifndef PARS_XUDB_SCANRATE
#  define PARS_XUDB_SCANRATE                  1  PARS_CF_SECOND     /**< frequency of the timer callback                  */
#endif /* PARS_XUDB_SCANRATE */
#ifndef PARS_XUDB_MAX_HANDLE_AGE
#  define PARS_XUDB_MAX_HANDLE_AGE            30 PARS_CF_MINUTES    /**< absolute max age since the last access           */
#endif /* PARS_XUDB_MAX_HANDLE_AGE */
#ifndef PARS_XUDB_MAX_CACHE_AGE
#  define PARS_XUDB_MAX_CACHE_AGE             60 PARS_CF_SECONDS    /**< max age after logout                             */
#endif /* PARS_XUDB_MAX_CACHE_AGE */

#ifndef PARS_XUDB_REALM_USERMGR
#  define PARS_XUDB_REALM_USERMGR             "UserManager"         /**< Realm a user has to belong to to change users    */
#endif /* PARS_XUDB_REALM_USERMGR */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Certificate Service
 *
 * Certificate related settings
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_CERTS_PKI_CERT
#  define PARS_CERTS_PKI_CERT                 PARS_CF_YES           /**< Enable PKI certificates.                         */
#endif /* PARS_CERTS_PKI_CERT */
#ifndef PARS_TEST_PCKS12
#  define PARS_TEST_PCKS12                    PARS_CF_NO            /**< Enable self test of pcks12 certs.                */
#endif /* PARS_TEST_PCKS12 */
#ifndef PARS_PCKS12_READ_CERTIFICATE
#  define PARS_PCKS12_READ_CERTIFICATE        PARS_CF_NO            /**< Enable red method for pcks12.                    */
#endif /* PARS_PCKS12_READ_CERTIFICATE */
#ifndef PARS_CERTIFICATE_INFO_OUT
#  define PARS_CERTIFICATE_INFO_OUT           PARS_CF_NO            /**< Enable debug output of cert info.                */
#endif /* PARS_CERTIFICATE_INFO_OUT */
#ifndef PARS_CERTS_VALIDATE_SERVER_CERT
#  define PARS_CERTS_VALIDATE_SERVER_CERT     PARS_CF_YES           /**< Enable verification of validity times            */
#endif /* PARS_CERTS_VALIDATE_SERVER_CERT */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name PKI Application
 *
 * Settings for the PKI App
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_PKI_TEST
#  define PARS_PKI_TEST                       PARS_CF_NO            /**< Enable self test of PKI module.                  */
#endif /* PARS_PKI_TEST */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Cryptographic Library
 *
 * Encryption stuff
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef CRYPTLIB_IMPLEMENT_A1
#  define CRYPTLIB_IMPLEMENT_A1               PARS_CF_YES           /**< access A1 value directly                         */
#endif /* CRYPTLIB_IMPLEMENT_A1 */
#ifndef CRYPTLIB_IMPLEMENT_MBASE64
#  define CRYPTLIB_IMPLEMENT_MBASE64          PARS_CF_YES           /**< Should MBase64 Algorithm included                */
#endif /* CRYPTLIB_IMPLEMENT_MBASE64 */
#ifndef CRYPTLIB_IMPLEMENT_CIPHER
#  define CRYPTLIB_IMPLEMENT_CIPHER           PARS_CF_YES           /**< Cipher Management included                       */
#endif /* CRYPTLIB_IMPLEMENT_CIPHER */
#ifndef CRYPTLIB_MAX_HASH_ALGOS
#  define CRYPTLIB_MAX_HASH_ALGOS             5                     /**< Maximum Hash Algorithms to load                  */
#endif /* CRYPTLIB_MAX_HASH_ALGOS */
#ifndef CRYPTLIB_MAX_CIPHER_ALGOS
#  define CRYPTLIB_MAX_CIPHER_ALGOS           5                     /**< Maximum Cypher Algorithm to load                 */
#endif /* CRYPTLIB_MAX_CIPHER_ALGOS */
#ifndef PARS_MD5_MAX_CONTEXTS
#  define PARS_MD5_MAX_CONTEXTS               21                    /**< Maximum simultaneous MD5 Contexts                */
#endif /* PARS_MD5_MAX_CONTEXTS */
#ifndef PARS_CRYPTLIB_ALGO_NAME_LEN
#  define PARS_CRYPTLIB_ALGO_NAME_LEN         10                    /**< Maximum size of an algo name                     */
#endif /* PARS_CRYPTLIB_ALGO_NAME_LEN */

/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name SMTP Client
 *
 * Configuration of our Mail-Sending-Module (Client and App)
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* ---  Send Mail Application                                                                   Send Mail Application --- */
#ifndef PARS_SMAPP_CONNECT_TIMEOUT
#  define PARS_SMAPP_CONNECT_TIMEOUT          5 PARS_CF_MINUTES                         /**< Maximum length for a session */
#endif /* PARS_SMAPP_CONNECT_TIMEOUT */
#ifndef PARS_SMAPP_MAX_SUPPORTED_SESSIONS
#  define PARS_SMAPP_MAX_SUPPORTED_SESSIONS   5                                        /**< maximum simultaneous sessions */
#endif /* PARS_SMAPP_MAX_SUPPORTED_SESSIONS */
/* ---  SMTP Client                                                                                       SMTP Client --- */
#ifndef PARS_SMTPC_LOG_RAW_TRANSFERED_DATA
#  define PARS_SMTPC_LOG_RAW_TRANSFERED_DATA  PARS_CF_NO                         /**< detailed log of all transfered data */
#endif /* PARS_SMTPC_LOG_RAW_TRANSFERED_DATA */
#ifndef PARS_SMTPC_LOG_ENABLED
#  define PARS_SMTPC_LOG_ENABLED              PARS_CF_YES                                             /**< enable logging */
#endif /* PARS_SMTPC_LOG_ENABLED */
#ifndef PARS_SMTPC_DEFAULT_LOCAL_NAME
#  define PARS_SMTPC_DEFAULT_LOCAL_NAME       "127.0.0.1"                                /**< Default name for local site */
#endif /* PARS_SMTPC_DEFAULT_LOCAL_NAME */
#ifndef PARS_SMTPC_DEFAULT_SMTP_PORT
#  define PARS_SMTPC_DEFAULT_SMTP_PORT        25                                        /**< Default port for SMTP Server */
#endif /* PARS_SMTPC_DEFAULT_SMTP_PORT */
#ifndef PARS_SMTPC_CHAR_ENCODING
#  define PARS_SMTPC_CHAR_ENCODING            "US-ASCII"                                  /**< Default Body char encoding */
#endif /* PARS_SMTPC_CHAR_ENCODING */
#ifndef PARS_SMTPC_BODY_ENCODING
#  define PARS_SMTPC_BODY_ENCODING            "Text/Plain"                                /**< Default Mime type for body */
#endif /* PARS_SMTPC_BODY_ENCODING */
#ifndef PARS_SMTPC_ATTACHMENT_TYPE
#  define PARS_SMTPC_ATTACHMENT_TYPE          "text"                                     /**< Default type for attachment */
#endif /* PARS_SMTPC_ATTACHMENT_TYPE */
#ifndef PARS_SMTPC_ATTACHMENT_ENCODING
#  define PARS_SMTPC_ATTACHMENT_ENCODING      "Application/Octet-stream"            /**< Default Mime type for attachment */
#endif /* PARS_SMTPC_ATTACHMENT_ENCODING */
#ifndef PARS_SMTPC_CRAM_HASH_ALGO
#  define PARS_SMTPC_CRAM_HASH_ALGO           "MD5"                                      /**< Hash Algo used for CRAM MD5 */
#endif /* PARS_SMTPC_CRAM_HASH_ALGO */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name HTTP Client
 *
 * Configuration of the HTTP Client
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_HTTPC_STATIC_INITIALZE
#  define PARS_HTTPC_STATIC_INITIALZE         PARS_CF_NO                                  /**< Don't use XML to configure */
#endif /* PARS_HTTPC_STATIC_INITIALZE */
#ifndef PARS_HTTPC_ProxyAdress
#  define PARS_HTTPC_ProxyAdress              0xaffeaffe                           /**< static Config: Used Proxy Address */
#endif /* PARS_HTTPC_ProxyAdress */
#ifndef PARS_HTTPC_ProxyPort
#  define PARS_HTTPC_ProxyPort                8080                                    /**< static Config: Used Proxy Port */
#endif /* PARS_HTTPC_ProxyPort */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name SOAP Application
 *
 * Configuration of the SOAP App
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_SOAP_WRITE_BOM
#  define PARS_SOAP_WRITE_BOM                 PARS_CF_YES                                /**< write a BOM in the XML file */
#endif /* PARS_SOAP_WRITE_BOM */
#ifndef PARS_SOAP_MAX_CALLS
#  define PARS_SOAP_MAX_CALLS                 10                                     /**< Maximum simultaneous SOAP calls */
#endif /* PARS_SOAP_MAX_CALLS */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Simulator Service and Var provider
 *
 * These two modules simulate a real provider for the VARIF
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_SIM_CREATE_XML_VARTABLE
#  define PARS_SIM_CREATE_XML_VARTABLE        PARS_CF_YES             /**< Create XML or HTML Table                       */
#endif /* PARS_SIM_CREATE_XML_VARTABLE */
#ifndef PARS_SIM_ASYNC_RESPONSE
#  define PARS_SIM_ASYNC_RESPONSE             PARS_CF_NO              /**< Simulator responds via timer cb                */
#endif /* PARS_SIM_ASYNC_RESPONSE */
#ifndef PARS_SIM_STARTUP_TRACE
#  define PARS_SIM_STARTUP_TRACE              PARS_CF_NO              /**< Trace output for debug purposes.               */
#endif /* PARS_SIM_STARTUP_TRACE */
#ifndef PARS_SIM_SUPPORT_RANDOM_VARS
#  define PARS_SIM_SUPPORT_RANDOM_VARS        PARS_CF_YES             /**< need background timer                          */
#endif /* PARS_SIM_SUPPORT_RANDOM_VARS */
#ifndef PARS_SIM_ASYNC_RESPONSE_DELAY
#  define PARS_SIM_ASYNC_RESPONSE_DELAY       25                      /**< Delay of milliseconds for test                 */
#endif /* PARS_SIM_ASYNC_RESPONSE_DELAY */
#ifndef PARS_SIM_MAX_VARIABLES
#  define PARS_SIM_MAX_VARIABLES              2000                    /**< max number of variables                        */
#endif /* PARS_SIM_MAX_VARIABLES */
#ifndef PARS_SIM_MAX_VAR_PROPERTIES
#  define PARS_SIM_MAX_VAR_PROPERTIES         20                      /**< additional props for a variable                */
#endif /* PARS_SIM_MAX_VAR_PROPERTIES */
#ifndef PARS_SIMULATOR_DISPLAY_NAME
#  define PARS_SIMULATOR_DISPLAY_NAME         "SimVarProvider"        /**< enable browse - display name.                  */
#endif /* PARS_SIMULATOR_DISPLAY_NAME */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name S2M (Makefile generator)
 *
 * Solution to makefile, generates makefile, *.mak and *.dep for the projects in the solution
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/** Size of the temporary line for the log file */
#define PARS_S2M_DEBUGBUFFERSIZE              2048
/** Size of the text buffer for makefiles (so no produced file can be bigger than this constant */
#define PARS_S2M_BUFFERSIZE                   65536
/** Size of a text buffer to concatenate string etc. must big enough for include strings etc. */
#define PARS_S2M_TEMP_BUFFER                  4096
/** Maximum amount of project contained by the solution */
#define PARS_S2M_MAX_PROJECTS                 200
/** Should header files added to the *.dep files ( working target ) */
#define PARS_S2M_SUPPORT_HEADER_FILES         PARS_CF_YES
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name OpcXml
 *
 * Settings for classic OPC
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_OPCXML_USE_SYSTEMROOT_AS_LANGUAGEROOT
#  define PARS_OPCXML_USE_SYSTEMROOT_AS_LANGUAGEROOT PARS_CF_YES      /**< using SystemRoot as Lang Source.               */
#endif /* PARS_OPCXML_USE_SYSTEMROOT_AS_LANGUAGEROOT */
#ifndef PARS_USE_POST_PROVIDER
#  define PARS_USE_POST_PROVIDER              PARS_CF_NO
#endif /* PARS_USE_POST_PROVIDER */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name DynamicVarProvider
 *
 * This provider handles user settings
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_DVPS_FLUSH_ON_SHUTDOWN
#  define PARS_DVPS_FLUSH_ON_SHUTDOWN         PARS_CF_YES             /**< flush user variables on shutdown               */
#endif /* PARS_DVPS_FLUSH_ON_SHUTDOWN */
#ifndef PARS_DVPS_FLUSH_IMMEDIATELY
#  define PARS_DVPS_FLUSH_IMMEDIATELY         PARS_CF_NO              /**< flush user variables on change                 */
#endif /* PARS_DVPS_FLUSH_IMMEDIATELY */
#ifndef PARS_DVPS_MAX_SESSION_VAR_COUNT
#  define PARS_DVPS_MAX_SESSION_VAR_COUNT     100                     /**< How many session variables                     */
#endif /* PARS_DVPS_MAX_SESSION_VAR_COUNT */
#ifndef PARS_DVPS_MAX_USER_VAR_COUNT
#  define PARS_DVPS_MAX_USER_VAR_COUNT        100                     /**< How many user variables                        */
#endif /* PARS_DVPS_MAX_USER_VAR_COUNT */
#ifndef PARS_DVPS_USER_VAR_FILE_NAME
#  define PARS_DVPS_USER_VAR_FILE_NAME        "/UserVariables.txt"    /**< filename of the variable storage               */
#endif /* PARS_DVPS_USER_VAR_FILE_NAME */
#ifndef PARS_DVPS_FLUSHING_INTERVAL
#  define PARS_DVPS_FLUSHING_INTERVAL         10                      /**< interval: variable storing (1/s)               */
#endif /* PARS_DVPS_FLUSHING_INTERVAL */
#ifndef PARS_DVPS_CHECKING_INTERVAL
#  define PARS_DVPS_CHECKING_INTERVAL         10                      /**< Interval: checking session validity            */
#endif /* PARS_DVPS_CHECKING_INTERVAL */
#ifndef PARS_DVPS_MAX_VAR_LENGTH
#  define PARS_DVPS_MAX_VAR_LENGTH            50                      /**< general max length of a variable               */
#endif /* PARS_DVPS_MAX_VAR_LENGTH */
#ifndef PARS_DVPS_MAX_VARNAME_LENGTH
#  define PARS_DVPS_MAX_VARNAME_LENGTH        50                      /**< general max length of a variable name          */
#endif /* PARS_DVPS_MAX_VARNAME_LENGTH */

/** general max length of a var key. Key is user name or session id.                  */
#ifndef PARS_DVPS_MAX_KEY_LENGTH
#  define PARS_DVPS_MAX_KEY_LENGTH            8
#endif /* PARS_DVPS_MAX_KEY_LENGTH */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Language Filter
 *
 * Central module deciding which language is used for the pages that are sent to the client
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_LANGFLTR_CASESENS_EXCLUDES
#  define PARS_LANGFLTR_CASESENS_EXCLUDES     PARS_CF_NO              /**< Exclude Entries are case sensitive             */
#endif /* PARS_LANGFLTR_CASESENS_EXCLUDES */
#ifndef PARS_LANGFLTR_EXCLUDE_APPS
#  define PARS_LANGFLTR_EXCLUDE_APPS          PARS_CF_YES             /**< Are Application auto excluded                  */
#endif /* PARS_LANGFLTR_EXCLUDE_APPS */
#ifndef PARS_LANGFLTR_EXCLUDE_ENTRIES
#  define PARS_LANGFLTR_EXCLUDE_ENTRIES       100                     /**< Maximum Entries in Exclude List                */
#endif /* PARS_LANGFLTR_EXCLUDE_ENTRIES */
#ifndef PARS_LANGFLTR_MAX_EXCLUDE_ENTRY_LEN
#  define PARS_LANGFLTR_MAX_EXCLUDE_ENTRY_LEN 20                      /**< Maximum Length of an exclude entry             */
#endif /* PARS_LANGFLTR_MAX_EXCLUDE_ENTRY_LEN */
#ifndef PARS_LANGFLTR_SHORTCUT_COUNT
#  define PARS_LANGFLTR_SHORTCUT_COUNT        50                      /**< Maximum count of shortcuts                     */
#endif /* PARS_LANGFLTR_SHORTCUT_COUNT */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Web DAV
 *
 * Settings for the Web DAV protocol
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_DAV_QUOTE_WORKARAOUND
#  define PARS_DAV_QUOTE_WORKARAOUND          PARS_CF_YES             /**< Workaround for hash ' bug                      */
#endif /* PARS_DAV_QUOTE_WORKARAOUND */
#ifndef PARS_DAV_MAX_LOCK_ENTRIES
#  define PARS_DAV_MAX_LOCK_ENTRIES           255                     /**< Max open locks on system                       */
#endif /* PARS_DAV_MAX_LOCK_ENTRIES */
#ifndef PARS_DAV_MAX_LOCK_TIMEOUT
#  define PARS_DAV_MAX_LOCK_TIMEOUT           3600                    /**< Timeout in Seconds                             */
#endif /* PARS_DAV_MAX_LOCK_TIMEOUT */
#ifndef PARS_DAV_MAX_PROPERTY_LEN
#  define PARS_DAV_MAX_PROPERTY_LEN           1   PARS_CF_KILO_BYTES  /**< Length of Property fields                      */
#endif /* PARS_DAV_MAX_PROPERTY_LEN */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Object Manager
 *
 * The OM is responsible for managing all the internal stuff for OPC UA and introduced the DataIF, the successor of the
 * VarIF
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_OM_PROVIDE_ATTRIBUTE_UTILS
#  define PARS_OM_PROVIDE_ATTRIBUTE_UTILS     PARS_CF_YES             /**< provide attribute methods and types            */
#endif /* PARS_OM_PROVIDE_ATTRIBUTE_UTILS */
#ifndef PARS_OM_PROVIDE_PROPERTY_UTILS
#  define PARS_OM_PROVIDE_PROPERTY_UTILS      PARS_CF_YES             /**< provide Property methods                       */
#endif /* PARS_OM_PROVIDE_PROPERTY_UTILS */
#ifndef PARS_OM_PROVIDE_VALUE_UTILS
#  define PARS_OM_PROVIDE_VALUE_UTILS         PARS_CF_YES             /**< provide Value methods                          */
#endif /* PARS_OM_PROVIDE_VALUE_UTILS */
#ifndef PARS_OM_PROVIDE_VECTOR_UTILS
#  define PARS_OM_PROVIDE_VECTOR_UTILS        PARS_CF_YES             /**< provide Vector methods                         */
#endif /* PARS_OM_PROVIDE_VECTOR_UTILS */
#ifndef PARS_OM_SUPPORT_COMPLEX_TYPES
#  define PARS_OM_SUPPORT_COMPLEX_TYPES       PARS_CF_YES             /**< support usage of complex types                 */
#endif /* PARS_OM_SUPPORT_COMPLEX_TYPES */
#ifndef PARS_OM_PROVIDE_EVENT_UTILS
#  define PARS_OM_PROVIDE_EVENT_UTILS         PARS_CF_YES             /**< provide event methods                          */
#endif /* PARS_OM_PROVIDE_EVENT_UTILS */
#ifndef PARS_OM_PROVIDE_OBJECTS
#  define PARS_OM_PROVIDE_OBJECTS             PARS_CF_YES             /**< Is the valuetype PARS_VT_OBJECT supported      */
#endif /* PARS_OM_PROVIDE_OBJECTS */
#ifndef PARS_OM_PROVIDE_ENCODER
#  define PARS_OM_PROVIDE_ENCODER             PARS_CF_YES             /**< Do we have an encoder?                         */
#endif /* PARS_OM_PROVIDE_ENCODER */
#ifndef PARS_OM_MAX_ATTRIBUTES
#  define PARS_OM_MAX_ATTRIBUTES              10000                   /**< Number of attribute pool elements              */
#endif /* PARS_OM_MAX_ATTRIBUTES */
#ifndef PARS_OM_MAX_COMPLEX_TYPES
#  define PARS_OM_MAX_COMPLEX_TYPES           10                      /**< Max Number of complex types                    */
#endif /* PARS_OM_MAX_COMPLEX_TYPES */
#ifndef PARS_OM_MAX_OBJECTTYPES
#  define PARS_OM_MAX_OBJECTTYPES             10                       /**< Max count of C-based Objecttypes              */
#endif /* PARS_OM_MAX_OBJECTTYPES */
#ifndef PARS_OM_CONVERT_FROM_XMLNODE
#  define PARS_OM_CONVERT_FROM_XMLNODE        PARS_CF_YES             /**< Enable convert xml node to string              */
#endif /* PARS_OM_CONVERT_FROM_XMLNODE */

#ifndef PARS_AM_HASH_TABLE_SIZE
#  define PARS_AM_HASH_TABLE_SIZE             10007                   /**< should be prime                                */
#endif /* PARS_AM_HASH_TABLE_SIZE */
#ifndef PARS_AM_MAX_PROVIDERS
#  define PARS_AM_MAX_PROVIDERS               16                      /**< Max count of providers the AM can manage       */
#endif /* PARS_AM_MAX_PROVIDERS */
#ifndef PARS_AM_MAX_NAMESPACES
#  define PARS_AM_MAX_NAMESPACES              16                      /**< Max count of namespaces the AM can manage      */
#endif /* PARS_AM_MAX_NAMESPACES */
#ifndef PARS_AM_MAX_MODELS
#  define PARS_AM_MAX_MODELS                  5                       /**< Max count of models the AM can manage          */
#endif /* PARS_AM_MAX_MODELS  */
#ifndef PARS_AM_MAX_GROUPS
#  define PARS_AM_MAX_GROUPS                  5                       /**< Max count of groups the AM can manage          */
#endif /* PARS_AM_MAX_GROUPS  */
/** The maximum number of import files (at least their identifiers) that can be cached  */
#ifndef PARS_AM_MAX_IMPORT_FILES
#  define PARS_AM_MAX_IMPORT_FILES           10
#endif /* PARS_AM_MAX_IMPORT_FILES */
#ifndef PARS_AM_ISSUBTYPE_BY_REF
#  define PARS_AM_ISSUBTYPE_BY_REF            PARS_CF_NO
#endif /* PARS_AM_ISSUBTYPE_BY_REF */
/** the maximum number of node IDs that we can deal with                            */
#ifndef PARS_AM_MAX_NODEID_COUNT
#  define PARS_AM_MAX_NODEID_COUNT            2000
#endif /* PARS_AM_MAX_NODEID_COUNT */
/** the maximum number of nodes that we can deal with                               */
#ifndef PARS_AM_MAX_NODE_COUNT
#  define PARS_AM_MAX_NODE_COUNT              PARS_AM_MAX_NODEID_COUNT
#endif /* PARS_AM_MAX_NODE_COUNT */
/** the maximum number of references that we can deal with. An average node has
 * six references (this is an empiric value) */
#ifndef PARS_AM_MAX_REF_COUNT
#  define PARS_AM_MAX_REF_COUNT               (6*PARS_AM_MAX_NODEID_COUNT)
#endif /* PARS_AM_MAX_REF_COUNT */
/**
 * The maximum supported languages for localized text in our product
 * */
#ifndef PARS_AM_MAX_SUPPORTED_LANGUAGES
#  define PARS_AM_MAX_SUPPORTED_LANGUAGES               1
#endif /* PARS_AM_MAX_SUPPORTED_LANGUAGES */

/**
 * Specifies if every node has its own version or the global version of the address model should be
 * used. This is interesting for BrowseNext, since the decision if the continuation point is still valid
 * is being made from the version. If you use the global version (this define is PARS_CF_NO then), it is
 * more likely that you get an MWE_INVALIDCONTINUATIONPOINT, since the global address model version
 * changes every time a node is modified. If you use the version on each variable, you need more memory,
 * since every variable node is four bytes larger then.
 * */
#ifndef PARS_AM_NODE_GRANULAR_VERSIONING
#  define PARS_AM_NODE_GRANULAR_VERSIONING              PARS_CF_YES
#endif /* PARS_AM_NODE_GRANULAR_VERSIONING */
/**
 * Indicates that each node has its own critical section to use. YES needs more memory but it is likely
 * that the code is faster than using the global critical section for all nodes
 * */
#ifndef PARS_AM_NODE_GRANULAR_THREADSAFE_ACCESS
#  define PARS_AM_NODE_GRANULAR_THREADSAFE_ACCESS       PARS_CF_YES
#endif /* PARS_AM_NODE_GRANULAR_THREADSAFE_ACCESS */
/**
 * Switches the implementation of IsSubType. If set to PARS_CF_YES, the IsSubtype method will process
 * the references that are really found on the given reference nodes. If set to PARS_CF_NO, a static
 * array containing the specified reference types is used
 * */

/** @brief This is the sampling timer interval and the minimum sampling rate. */
#ifndef PARS_SMARTIFIER_SAMPLINGINTERVAL
#  define PARS_SMARTIFIER_SAMPLINGINTERVAL    50 /* ms */
#endif /* PARS_SMARTIFIER_SAMPLINGINTERVAL */
/** @brief This is the publishing timer interval and the minimum publishing rate. */
#ifndef PARS_SUBS_PUBLISHINGINTERVAL
#  define PARS_SUBS_PUBLISHINGINTERVAL        100 /* ms */
#endif /* PARS_SUBS_PUBLISHINGINTERVAL */
#ifndef PARS_SUBS_MIN_QUEUE_SIZE
#  define PARS_SUBS_MIN_QUEUE_SIZE            1                       /**< The minimum queue size that we need            */
#endif /* PARS_SUBS_MIN_QUEUE_SIZE */
#ifndef PARS_SUBS_MAX_QUEUE_SIZE
#  define PARS_SUBS_MAX_QUEUE_SIZE            20                      /**< The maximum queue size that we support         */
#endif /* PARS_SUBS_MAX_QUEUE_SIZE */
#ifndef PARS_SUBS_DEFAULT_KEEPALIVE_COUNT
#  define PARS_SUBS_DEFAULT_KEEPALIVE_COUNT   10                      /**< Used, if no specific keepalive count is given  */
#endif /* PARS_SUBS_DEFAULT_KEEPALIVE_COUNT  */
#ifndef PARS_SUBS_MAX_KEEPALIVE_COUNT
#  define PARS_SUBS_MAX_KEEPALIVE_COUNT       (PARS_DWORD)(100000)    /**< Max keep alive count. Must not overflow when multiplied by 3  */
#endif /* PARS_SUBS_MAX_KEEPALIVE_COUNT  */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Ajax Framework
 *
 * Asynchronous-Javascript-And-XML related settings here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_AJAX_MAX_FUNCTIONS
#  define PARS_AJAX_MAX_FUNCTIONS           50                      /**< Max count of inscriptionable Ajax functions      */
#endif /* PARS_AJAX_MAX_FUNCTIONS */
#ifndef PARS_AJAX_MAX_EVENTSOURCES
#  define PARS_AJAX_MAX_EVENTSOURCES        5                       /**< Max count of Event sources                       */
#endif /* PARS_AJAX_MAX_EVENTSOURCES */
#ifndef PARS_AJAX_MAX_OBJECTS
#  define PARS_AJAX_MAX_OBJECTS             25                      /**< Max count of Ajax Objects                        */
#endif /* PARS_AJAX_MAX_OBJECTS */
#ifndef PARS_AJAX_DELIMITER
#  define PARS_AJAX_DELIMITER               7                       /**< Character, which is used as parameter delimiter  */
#endif /* PARS_AJAX_DELIMITER */

#ifndef PARS_AJAX_MAX_SIM_CALLS
#  define PARS_AJAX_MAX_SIM_CALLS           10                      /**< Max simultaneous Ajax requests                   */
#endif /* PARS_AJAX_MAX_SIM_CALLS */
#ifndef PARS_AJAX_MAX_PARAMETERS
#  define PARS_AJAX_MAX_PARAMETERS          15
#endif /* PARS_AJAX_MAX_PARAMETERS */
#ifndef PARS_AJAX_NOTFICATION_STRING_LEN
#  define PARS_AJAX_NOTFICATION_STRING_LEN  PARS_HTTP_TRANSFER_BUFFER
#endif /* PARS_AJAX_NOTFICATION_STRING_LEN */
#ifndef PARS_AJAX_MAX_DELAYS
#  define PARS_AJAX_MAX_DELAYS              10                      /**< MAX Delays stored                                */
#endif /* PARS_AJAX_MAX_DELAYS */
#ifndef PARS_AJAX_MAX_TIMEOUT
#  define PARS_AJAX_MAX_TIMEOUT             10000                   /**< MAX Delay in ms                                  */
#endif /* PARS_AJAX_MAX_TIMEOUT */
#ifndef PARS_AJAX_MAX_OPEN_EVENTS
#  define PARS_AJAX_MAX_OPEN_EVENTS         10                      /**< MAX simultaneously open events                   */
#endif /* PARS_AJAX_MAX_OPEN_EVENTS */
#ifndef PARS_AJAX_MAX_SUBSCRIPTIONS
#  define PARS_AJAX_MAX_SUBSCRIPTIONS       10                      /**< MAX simultaneously open subscriptions            */
#endif /* PARS_AJAX_MAX_SUBSCRIPTIONS */
#ifndef PARS_AJAX_POLL_MAX_MISSED_SLOTS
#  define PARS_AJAX_POLL_MAX_MISSED_SLOTS   30                      /**< Timeout for a missing poll in x*250ms            */
#endif /* PARS_AJAX_POLL_MAX_MISSED_SLOTS */
#ifndef PARS_AJAX_SLOT_TIME
#  define PARS_AJAX_SLOT_TIME               250                      /**< Timeout for a missing poll:
                                                                       *  x*PARS_AJAX_SLOT_TIME ms                        */
#endif /* PARS_AJAX_SLOT_TIME */
/**@}*/

/* --- START: Section: "NewModuleConfig" ------------------------------------------ START: Section: "NewModuleConfig" --- */
/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name PARS_SSO_Client
 *
 * Project related settings here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* Add your Compiler switches here */
/**@}*/
/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name PARS_ODBC
 *
 * Project related settings here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* Add your Compiler switches here */
/**@}*/
/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name PARS_ODBC_WEB_UI
 *
 * Project related settings here
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* Add your Compiler switches here */
/**@}*/
/* --- END: Section: "NewModuleConfig" ---------------------------------------------- END: Section: "NewModuleConfig" --- */

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name HTTP Client Application
 *
 * Yes, we are not only a Web-Server but also an HTTP-Client!
 * */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#ifndef PARS_HTTPC_PREFER_STATIC_MEMORY
#  define PARS_HTTPC_PREFER_STATIC_MEMORY     PARS_CF_YES             /**< use more static than heap memory               */
#endif /* PARS_HTTPC_PREFER_STATIC_MEMORY */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Static linkage
 *
 * In this section, you can decide which modules shall be linked static instead of using the dynamic libraries.
 * PARS_CF_STATIC indicates to use the static library, PARS_CF_DYNAMIC tells us using the dynamic one.
 * PARS_CF_NONE indicates, that this module is not used.
 */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* --- START: Section: "NewModuleLinkage" ---------------------------------------- START: Section: "NewModuleLinkage" --- */

#ifndef PARS_LINKAGE_FILESYS
#  define PARS_LINKAGE_FILESYS                 PARS_CF_STATIC
#endif /* PARS_LINKAGE_FILESYS */
#ifndef PARS_LINKAGE_OMUTILS
#  define PARS_LINKAGE_OMUTILS                 PARS_CF_STATIC
#endif /* PARS_LINKAGE_OMUTILS */
#ifndef PARS_LINKAGE_XML
#  define PARS_LINKAGE_XML                     PARS_CF_STATIC
#endif /* PARS_LINKAGE_XML */
#ifndef PARS_LINKAGE_CRYPT_AES256
#  define PARS_LINKAGE_CRYPT_AES256            PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CRYPT_AES256 */
#ifndef PARS_LINKAGE_CRYPT_SERVICE
#  define PARS_LINKAGE_CRYPT_SERVICE           PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CRYPT_SERVICE */
#ifndef PARS_LINKAGE_CRYPT_DES3
#  define PARS_LINKAGE_CRYPT_DES3              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CRYPT_DES3 */
#ifndef PARS_LINKAGE_EVENTSERVICE
#  define PARS_LINKAGE_EVENTSERVICE            PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_EVENTSERVICE */
#ifndef PARS_LINKAGE_CRYPT_HMAC
#  define PARS_LINKAGE_CRYPT_HMAC              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CRYPT_HMAC */
#ifndef PARS_LINKAGE_CRYPT_MD5
#  define PARS_LINKAGE_CRYPT_MD5               PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CRYPT_MD5 */
#ifndef PARS_LINKAGE_CRYPT_RC4
#  define PARS_LINKAGE_CRYPT_RC4               PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CRYPT_RC4 */
#ifndef PARS_LINKAGE_AUTH_BASIC
#  define PARS_LINKAGE_AUTH_BASIC              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_AUTH_BASIC */
#ifndef PARS_LINKAGE_AUTH_DIGEST
#  define PARS_LINKAGE_AUTH_DIGEST             PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_AUTH_DIGEST */
#ifndef PARS_LINKAGE_AUTH_FORMBASED
#  define PARS_LINKAGE_AUTH_FORMBASED          PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_AUTH_FORMBASED */
#ifndef PARS_LINKAGE_HTTP_SECURITY
#  define PARS_LINKAGE_HTTP_SECURITY           PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_HTTP_SECURITY */
#ifndef PARS_LINKAGE_XMLUSERDATABASE
#  define PARS_LINKAGE_XMLUSERDATABASE         PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_XMLUSERDATABASE */
#ifndef PARS_LINKAGE_CRYPT_SHA1
#  define PARS_LINKAGE_CRYPT_SHA1              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CRYPT_SHA1 */
#ifndef PARS_LINKAGE_AJAXAPPLICATION
#  define PARS_LINKAGE_AJAXAPPLICATION         PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_AJAXAPPLICATION */
#ifndef PARS_LINKAGE_CONFIGTOOL
#  define PARS_LINKAGE_CONFIGTOOL              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CONFIGTOOL */
#ifndef PARS_LINKAGE_CONVERSIONSERVICE
#  define PARS_LINKAGE_CONVERSIONSERVICE       PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CONVERSIONSERVICE */
#ifndef PARS_LINKAGE_DVPSERVICE
#  define PARS_LINKAGE_DVPSERVICE              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_DVPSERVICE */
#ifndef PARS_LINKAGE_HANDLEMGMT
#  define PARS_LINKAGE_HANDLEMGMT              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_HANDLEMGMT */
#ifndef PARS_LINKAGE_HASHTABLE
#  define PARS_LINKAGE_HASHTABLE               PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_HASHTABLE */
#ifndef PARS_LINKAGE_HTTPCLIENT
#  define PARS_LINKAGE_HTTPCLIENT              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_HTTPCLIENT */
#ifndef PARS_LINKAGE_CONFIGURATOR
#  define PARS_LINKAGE_CONFIGURATOR            PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CONFIGURATOR */
#ifndef PARS_LINKAGE_SOLUTIONTOMAKEFILE
#  define PARS_LINKAGE_SOLUTIONTOMAKEFILE      PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SOLUTIONTOMAKEFILE */
#ifndef PARS_LINKAGE_MWSLAPP
#  define PARS_LINKAGE_MWSLAPP                 PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_MWSLAPP */
#ifndef PARS_LINKAGE_MWSL
#  define PARS_LINKAGE_MWSL                    PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_MWSL */
#ifndef PARS_LINKAGE_MWSL2
#  define PARS_LINKAGE_MWSL2                   PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_MWSL2 */
#ifndef PARS_LINKAGE_ADDRESSMODELLER
#  define PARS_LINKAGE_ADDRESSMODELLER         PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_ADDRESSMODELLER */
#ifndef PARS_LINKAGE_BASENODEPROVIDER
#  define PARS_LINKAGE_BASENODEPROVIDER        PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_BASENODEPROVIDER */
#ifndef PARS_LINKAGE_DATAACCESS
#  define PARS_LINKAGE_DATAACCESS              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_DATAACCESS */
#ifndef PARS_LINKAGE_MIGRATIONPROVIDER
#  define PARS_LINKAGE_MIGRATIONPROVIDER       PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_MIGRATIONPROVIDER */
#ifndef PARS_LINKAGE_SAMPLEPROVIDER_SRVC
#  define PARS_LINKAGE_SAMPLEPROVIDER_SRVC     PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SAMPLEPROVIDER_SRVC */
#ifndef PARS_LINKAGE_SMARTIFIER_SRVC
#  define PARS_LINKAGE_SMARTIFIER_SRVC         PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SMARTIFIER_SRVC */
#ifndef PARS_LINKAGE_SUBSCRIPTION
#  define PARS_LINKAGE_SUBSCRIPTION            PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SUBSCRIPTION */
#ifndef PARS_LINKAGE_UASUPPORT
#  define PARS_LINKAGE_UASUPPORT               PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_UASUPPORT */
#ifndef PARS_LINKAGE_SIMAPPLICATION
#  define PARS_LINKAGE_SIMAPPLICATION          PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SIMAPPLICATION */
#ifndef PARS_LINKAGE_SIMSERVICE
#  define PARS_LINKAGE_SIMSERVICE              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SIMSERVICE */
#ifndef PARS_LINKAGE_SMAPP
#  define PARS_LINKAGE_SMAPP                   PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SMAPP */
#ifndef PARS_LINKAGE_SMTPC
#  define PARS_LINKAGE_SMTPC                   PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SMTPC */
#ifndef PARS_LINKAGE_SOAP_SERVICE
#  define PARS_LINKAGE_SOAP_SERVICE            PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_SOAP_SERVICE */
#ifndef PARS_LINKAGE_XMLMODIFIERAPP
#  define PARS_LINKAGE_XMLMODIFIERAPP          PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_XMLMODIFIERAPP */
#ifndef PARS_LINKAGE_CGI_APP
#  define PARS_LINKAGE_CGI_APP                 PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_CGI_APP */
#ifndef PARS_LINKAGE_PKI_APP
#  define PARS_LINKAGE_PKI_APP                 PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_PKI_APP */
#ifndef PARS_LINKAGE_MINIAPP
#  define PARS_LINKAGE_MINIAPP                 PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_MINIAPP */
#ifndef PARS_LINKAGE_FTP
#  define PARS_LINKAGE_FTP                     PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_FTP */
#ifndef PARS_LINKAGE_OPCUA_APPLICATION
#  define PARS_LINKAGE_OPCUA_APPLICATION       PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_OPCUA_APPLICATION */
#ifndef PARS_LINKAGE_OPCUA_MUX
#  define PARS_LINKAGE_OPCUA_MUX               PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_OPCUA_MUX */
#ifndef PARS_LINKAGE_OPCUA_PORTLAYER_SERVICE
#  define PARS_LINKAGE_OPCUA_PORTLAYER_SERVICE PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_OPCUA_PORTLAYER_SERVICE */
#ifndef PARS_LINKAGE_OPCXML_APPLICATION
#  define PARS_LINKAGE_OPCXML_APPLICATION      PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_OPCXML_APPLICATION */
#ifndef PARS_LINKAGE_OPC_BASESERVICE
#  define PARS_LINKAGE_OPC_BASESERVICE         PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_OPC_BASESERVICE */
#ifndef PARS_LINKAGE_USEREDITOR
#  define PARS_LINKAGE_USEREDITOR              PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_USEREDITOR */
#ifndef PARS_LINKAGE_DAV
#  define PARS_LINKAGE_DAV                     PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_DAV */
#ifndef PARS_LINKAGE_VARIF_SERVICE
#  define PARS_LINKAGE_VARIF_SERVICE          PARS_CF_STATIC
#endif /* PARS_LINKAGE_VARIF_SERCVICE */
#ifndef PARS_LINKAGE_DEFAPP
#  define PARS_LINKAGE_DEFAPP                  PARS_CF_STATIC         /**< Use Default Application                        */
#endif /* PARS_LINKAGE_DEFAPP */
#ifndef PARS_LINKAGE_SOCKIF
#  define PARS_LINKAGE_SOCKIF                  PARS_CF_STATIC         /**< Basis Socket Service                           */
#endif /* PARS_LINKAGE_SOCKIF */
#ifndef PARS_LINKAGE_HTTP_SERVICE
#  define PARS_LINKAGE_HTTP_SERVICE            PARS_CF_STATIC         /**< Initialize HTTP Service                        */
#endif /* PARS_LINKAGE_HTTP_SERVICE */
#ifndef PARS_LINKAGE_LANG_FILTER
#  define PARS_LINKAGE_LANG_FILTER             PARS_CF_STATIC         /**< Initialize Language Filter Service             */
#endif /* PARS_LINKAGE_LANG_FILTER */
#ifndef PARS_LINKAGE_TRACE_APP
#  define PARS_LINKAGE_TRACE_APP               PARS_CF_DYNAMIC        /**< Initialize Trace Application Service           */
#endif /* PARS_LINKAGE_TRACE_APP */
#ifndef PARS_LINKAGE_PARS_SSO_Client
#  define PARS_LINKAGE_PARS_SSO_Client         PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_PARS_SSO_Client */
#ifndef PARS_LINKAGE_PARS_ODBC
#  define PARS_LINKAGE_PARS_ODBC               PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_PARS_ODBC */
#ifndef PARS_LINKAGE_PARS_ODBC_WEB_UI
#  define PARS_LINKAGE_PARS_ODBC_WEB_UI        PARS_CF_DYNAMIC
#endif /* PARS_LINKAGE_PARS_ODBC_WEB_UI */
/* --- END: Section: "NewModuleLinkage" -------------------------------------------- END: Section: "NewModuleLinkage" --- */

/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Automatic settings                                                                                              */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* Enable global filter support when pre- or post- filtering is active */
#if PARS_IMPLEMENT_PREFILTER
#  ifndef PARS_USES_FILTER
#    define PARS_USES_FILTER                PARS_CF_YES             /**< Enable filter support                            */
#  endif /* PARS_USES_FILTER */
#else /* PARS_IMPLEMENT_PREFILTER */
#  if PARS_IMPLEMENT_POSTFILTER
#    ifndef PARS_USES_FILTER
#      define PARS_USES_FILTER                PARS_CF_YES             /**< Enable filter support                          */
#    endif /* PARS_USES_FILTER */
#  else /* PARS_IMPLEMENT_POSTFILTER */
#    ifndef PARS_USES_FILTER
#      define PARS_USES_FILTER                PARS_CF_NO              /**< Disable filter support                         */
#    endif /* PARS_USES_FILTER */
#  endif /* PARS_IMPLEMENT_POSTFILTER */
#endif /* PARS_IMPLEMENT_PREFILTER */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Consistency checks                                                                                              */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#if PARS_SSL_USE_GENERIC_CERTIFICATES
#  if !PARS_CERTS_PKI_CERT
#    error PARS_CERTS_PKI_CERT must be set for PARS_SSL_USE_GENERIC_CERTIFICATES.
#  endif /* PARS_CERTS_PKI_CERT */
#endif /* PARS_SSL_USE_GENERIC_CERTIFICATES */

#if !PARS_FILESYS_HAVE_XML_FILESYS
#  if PARS_FILESYS_WRITEBABLE_WEBCFG
#    error PARS_FILESYS_WRITEBABLE_WEBCFG can only be set when PARS_FILESYS_HAVE_XML_FILESYS
#  endif /* PARS_FILESYS_WRITEBABLE_WEBCFG */
#endif /* PARS_FILESYS_HAVE_XML_FILESYS */

#if PARS_PROTECT_HIDDEN_FILES
#  ifndef PARS_FILES_ALLOWED_REALM
#    error an PARS_FILES_ALLOWED_REALM must be defined when PARS_PROTECT_HIDDEN_FILES is set.
#  endif  /* PARS_FILES_ALLOWED_REALM */
#endif  /* PARS_PROTECT_HIDDEN_FILES */

#if PARS_MTHREAD
#  if PARS_MAX_SOCKET_LISTS <= 1
#    error We need at least one thread per socket list,
#  endif /* 1 < PARS_MAX_SOCKET_LISTS */
#endif /* PARS_MTHREAD */

#if !PARS_FILESYS_HAVE_XML_FILESYS
#  if !PARS_USES_EXTERNAL_SYSTEM_ROOT
#    error "XML Systemroot only works with XML file sys"
#  endif /* !PARS_USES_EXTERNAL_SYSTEM_ROOT */
#endif /* PARS_FILESYS_HAVE_XML_FILESYS */

/* --- Debug Flags                                                                                                    --- */
#if !PARS_DEBUG_OUT
#  if PARS_MEMORY_TRACE_WINDOWS
#    error "PARS_MEMORY_TRACE_WINDOWS needs PARS_DEBUG_OUT set."
#  endif /* PARS_MEMORY_TRACE_WINDOWS */
#  if PARS_MEMORY_TRACE
#    error "PARS_MEMORY_TRACE needs PARS_DEBUG_OUT set."
#  endif /* PARS_MEMORY_TRACE */
#  if PARS_DEBUG_MODE_CALL_TRACE
#    error "PARS_DEBUG_MODE_CALL_TRACE needs PARS_DEBUG_OUT set."
#  endif /* PARS_DEBUG_MODE_CALL_TRACE */
#endif /* PARS_DEBUG_OUT */

/* --- Multi threading                                                                                                --- */
#if PARS_USE_THREADPOOL
#  if !PARS_MTHREAD
#    error Threadpool can only be used with multithread
#  endif  /* PARS_MTHREAD */
#endif /* PARS_USE_THREADPOOL */

#if PARS_MTHREAD
#  if !PARS_USE_SYNCHRONISATION
#    error Multithreaded needs synchronisation
#  endif  /* !PARS_USE_SYNCHRONISATION */
#endif /* PARS_MTHREAD                                                                                               --- */

#if PARS_TRACE_WITH_TIME
#  if !PARS_DEBUG_OUT
#    error "PARS_DEBUG_WITH_TIME needs PARS_DEBUG_OUT set!"
#  endif
#endif /* PARS_TRACE_WITH_TIME */
/**@}*/
/**@}*/

#endif /* __CONFIG_DEFAULT_DEFINED__ */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

