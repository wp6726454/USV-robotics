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
/*  F i l e               &F: PARSP_Utils.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  xml parser runtime library, use functionality from crt when possible     */
/*                                                                           */
/*****************************************************************************/

/* --- -------------------------------------------------------------------------------------------------------------- --- *\
|   Includes                                                                                                    Includes   |
\* --- -------------------------------------------------------------------------------------------------------------- --- */

/*----------------------------------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------------------------------*/
#ifndef _PARSP_UTILS_H_DEFINED_
#define _PARSP_UTILS_H_DEFINED_

/* --- Our own (ported) types ---------------------------------------------------------------- Our own (ported) types --- */
#include "PARS_Types.h"                                                                              /* Datatypes */
#include "psi_inc.h"
#include "pnd_sys.h"

PARS_BEGIN_EXTERN_C

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|   Which Functions must be implemented by PARSP_utils.c                                                                   |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
#define PARS_OWN_MEMSET         PARS_CF_NO                                         /**< Use our implement of memset       */
#define PARS_OWN_MEMCPY         PARS_CF_NO                                         /**< Use our implement of memcpy       */
#define PARS_OWN_MEMMOVE        PARS_CF_NO                                         /**< Use our implement of memmove      */
#define PARS_OWN_MEMCMP         PARS_CF_NO                                         /**< Use our implement of memcmp       */

#define PARS_OWN_STRNLEN        PARS_CF_YES                                        /**< Use our implement of strnlen      */
#define PARS_OWN_STRLEN         PARS_CF_YES                                        /**< Use our implement of strlen       */
#define PARS_OWN_STRNSTR        PARS_CF_NO                                         /**< Use our implement of strnstr      */
#define PARS_OWN_STRSTR         PARS_CF_YES                                        /**< Use our implement of strstr       */
#define PARS_OWN_STRINSTR       PARS_CF_NO                                         /**< Use our implement of strinstr     */
#define PARS_OWN_STRISTR        PARS_CF_NO                                         /**< Use our implement of stristr      */
#define PARS_OWN_STRCAT         PARS_CF_NO                                         /**< Use our implement of strcat       */
#define PARS_OWN_STRCPY         PARS_CF_NO                                         /**< Use our implement of strcpy       */
#define PARS_OWN_STRNCMP        PARS_CF_YES                                        /**< Use our implement of strncmp      */
#define PARS_OWN_STRCMP         PARS_CF_YES                                        /**< Use our implement of strcmp       */
#define PARS_OWN_STRNICMP       PARS_CF_YES                                        /**< Use our implement of strnicmp     */
#define PARS_OWN_STRICMP        PARS_CF_NO                                         /**< Use our implement of stricmp      */
#define PARS_OWN_STRNCHR        PARS_CF_NO                                         /**< Use our implement of strnchr      */
#define PARS_OWN_STRRCHR        PARS_CF_NO                                         /**< Use our implement of strrchr      */
#define PARS_OWN_STRCHR         PARS_CF_YES                                        /**< Use our implement of strchr       */
#define PARS_OWN_STRTOK         PARS_CF_NO                                         /**< Use our implement of strtok       */
#define PARS_OWN_STRSPN         PARS_CF_YES                                        /**< Use our implement of strspn       */
#define PARS_OWN_STRCSPN        PARS_CF_YES                                        /**< Use our implement of strcspn      */
#define PARS_OWN_STRPBRK        PARS_CF_NO                                         /**< Use our implement of strpbrk      */
#define PARS_OWN_STRREV         PARS_CF_YES                                        /**< Use our implement of strrev       */

#define PARS_OWN_ISNAN          PARS_CF_NO                                         /**< Use our implement of isNan        */
#define PARS_OWN_ISFINITE       PARS_CF_NO                                         /**< Use our implement of Finite       */

#define PARS_OWN_VSPRINTF       PARS_CF_NO                                         /**< Use our implement of vsprintf     */
#define PARS_OWN_VSNPRINTF      PARS_CF_NO                                         /**< Use our implement of vsnprintf    */
#define PARS_OWN_SPRINTF        PARS_CF_NO                                         /**< Use our implement of sprintf      */
#define PARS_OWN_SSCANF         PARS_CF_NO                                         /**< Use our implement of sscanf       */

#define PARS_OWN_STRUPR         PARS_CF_NO                                         /**< Use our implement of strupr       */
#define PARS_OWN_STRLWR         PARS_CF_NO                                         /**< Use our implement of strlwr       */
#define PARS_OWN_TOUPPER        PARS_CF_NO                                         /**< Use our implement of toupper      */
#define PARS_OWN_TOLOWER        PARS_CF_NO                                         /**< Use our implement of tolower      */

#define PARS_OWN_ATOI           PARS_CF_NO                                         /**< Use our implement of atoi         */
#define PARS_OWN_ATOL           PARS_CF_NO                                         /**< Use our implement of atol         */
#define PARS_OWN_ITOA           PARS_CF_NO                                         /**< Use our implement of itoa         */
#define PARS_OWN_LTOA           PARS_CF_NO                                         /**< Use our implement of ltoa         */
#define PARS_OWN_ULTOA          PARS_CF_NO                                         /**< Use our implement of ultoa        */

#define PARS_OWN_WRMDIR         PARS_CF_NO                                         /**< Use our implement of wrmdir       */
#define PARS_OWN_WREMOVE        PARS_CF_NO                                         /**< Use our implement of wremove      */
#define PARS_OWN_TZSET          PARS_CF_NO                                         /**< Use our implement of tzset        */
#define PARS_OWN_GET_TIMEZONE   PARS_CF_NO                                         /**< Use our implement of get_timezone */
#define PARS_OWN_GET_DSTBIAS    PARS_CF_NO                                         /**< Use our implement of get_dstbias  */

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|     VA Types                                                                                                VA Types     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
#define PARS_va_start   va_start
#define PARS_va_arg     va_arg
#define PARS_va_end     va_end
#define PARS_va_list    va_list

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|     Mathematical functions                                                                    Mathematical functions     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
/** Definition of rounding strategies. Only DEFAULT is implemented by us. Add yours if needed */
/**@{*/
#define PARS_ROUND_STRATEGY_DEFAULT  1  /**< Our default rounding strategy          */
/*
 * Example:
 * #define ROUND_STRATEGY_BANKERS  2
*/
/**@}*/
PARS_RESULT DLLCALL PARS_Round( /*  in */   PARS_DOUBLE     a_dSrc,
                                /*  in */   PARS_DWORD      a_dwDecimals,
                                /*  in */   PARS_DWORD      a_dwStrategy,
                                /* out */   PARS_LPDOUBLE   a_pdDst);

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|       Helper functions (always available)                                                           Helper functions     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
/*--- PARS_MemSearch --------------------------------------------------------------------------------- PARS_MemSearch --- */
    #define PARS_MemSearch PARSP_MemSearch
PARS_LPBYTE PARSP_MemSearch                             ( /*  in */  PARS_LPBYTE    a_strMemoryBlock,
                                                          /*  in */  PARS_LPBYTE    a_strPattern,
                                                          /*  in */  PARS_DWORD     a_dwMemoryBlockLen,
                                                          /*  in */  PARS_DWORD     a_dwPatternLen);

/*--- PARS_strlcat ------------------------------------------------------------------------------------- PARS_strlcat --- */
    #define PARS_strlcat PARSP_strlcat
    PARS_DWORD  PARSP_strlcat                           ( /*  bi */ PARS_LPSTR      a_strDestination,
                                                          /*  in */ PARS_LPCSTR     a_strSource,
                                                          /*  in */ PARS_DWORD      a_szDestinationLength,
                                                          /*  in */ PARS_DWORD      a_szCopyLength);

/*--- PARS_strlcpy ------------------------------------------------------------------------------------- PARS_strlcpy --- */
    #define PARS_strlcpy PARSP_strlcpy
PARS_DWORD  PARSP_strlcpy                               ( /*  bi */ PARS_LPSTR      a_strDestination,
                                                          /*  in */ PARS_LPCSTR     a_strSource,
                                                          /*  in */ PARS_DWORD      a_szDestinationLength,  /* Maximum Buffer size of a_strDestination */
                                                          /*  in */ PARS_DWORD      a_szCopyLength);        /* Maximum chars to copy, when no '\0' is found in a_strSource */

/*  PARSP_strnstr ------------------------------------------------------------------------------------- PARSP_strnstr --- */
    #define PARS_strnstr PARSP_strnstr
PARS_LPSTR  PARSP_strnstr                               ( /*  in */ PARS_LPCSTR     a_strToCheck,
                                                          /*  in */ PARS_LPCSTR     a_strSubString,
                                                          /*  in */ PARS_DWORD      a_dwMaxLen);

// not used by XML parser
///*  PARSP_strinstr ----------------------------------------------------------------------------------- PARSP_strinstr --- */
//    #define PARS_strinstr PARSP_strinstr
//    PARS_LPSTR  PARSP_strinstr                          ( /*  in */ PARS_LPCSTR     a_strToCheck,
//                                                          /*  in */ PARS_LPCSTR     a_strSubString,
//                                                          /*  in */ PARS_DWORD      a_dwMaxLen);
//
///*  PARSP_stristr ------------------------------------------------------------------------------------- PARSP_stristr --- */
//    #define PARS_stristr PARSP_stristr
//    PARS_LPSTR  PARSP_stristr                           ( /*  in */ PARS_LPCSTR     a_strToCheck,
//                                                          /*  in */ PARS_LPCSTR     a_strSubString);

/*  PARSP_strnchr ------------------------------------------------------------------------------------- PARSP_strnchr --- */
    #define PARS_strnchr PARSP_strnchr
    PARS_LPSTR  PARSP_strnchr                           ( /*  in */ PARS_LPCSTR     a_strLeft,
                                                          /*  in */ PARS_INT        a_nCharacter,
                                                          /*  in */ PARS_DWORD      a_nMaxLen);
// not used by XML parser
///*--- PARS_snprintf ----------------------------------------------------------------------------------- PARS_snprintf --- */
//    #define PARS_snprintf PARSP_snprintf
//PARS_RESULT PARSP_snprintf                              ( /*  in */ PARS_LPSTR      a_strBuffer,
//                                                          /*  in */ PARS_INT        a_nBufferSize,
//                                                          /*  in */ PARS_LPCSTR     a_strFormat,
//                                                          /*  in */ ...);

/*--- PARS_StrToDbl ----------------------------------------------------------------------------------- PARS_StrToDbl --- */
/* Replacement for atof                                 */
/* Returns MWS_FALSE if not all of a_strIn is processed */
/* Returns MWE_Range, *a_pdblOut!=0.0 for Overflow      */
/* Returns MWE_Range, *a_pdblOut=0.0 for Underflow      */
/* Returns MWE_NO_ERROR if succeeds                     */
    #define PARS_StrToDbl PARSP_StrToDbl
PARS_RESULT PARSP_StrToDbl                              ( /*  in */ PARS_LPSTR      a_strIn,
                                                          /*  in */ PARS_DWORD      a_dwInLen, /* can be 0 to ignore */
                                                          /* out */ PARS_LPDOUBLE   a_pdblOut,
                                                          /* out */ PARS_LPDWORD    a_pdwparsedLen);

/*--- UTF8 functions                                                                                    UTF8 functions ---*/
/*--- PARSP_MapUnicodeToUTF8 ----------------------------------------------------------------- PARSP_MapUnicodeToUTF8 --- */
    #define PARS_MapUnicodeToUTF8 PARSP_MapUnicodeToUTF8
PARS_INT    PARSP_MapUnicodeToUTF8                      ( /*  in */ PARS_LPWSTR    a_wstrUnicodeText,
                                                          /* out */ PARS_LPSTR     a_strUTF8CodeText);

/*--- PARSP_MapUTF8ToUnicode ----------------------------------------------------------------- PARSP_MapUTF8ToUnicode --- */
    #define PARS_MapUTF8ToUnicode PARSP_MapUTF8ToUnicode
PARS_INT    PARSP_MapUTF8ToUnicode                      ( /*  in */ PARS_LPSTR     a_strUtf8Text,
                                                          /* out */ PARS_LPWSTR    a_wstrUniCodeText);

/*--- PARSP_Get_HTTP_Code ----------------------------------------------------------------------- PARSP_Get_HTTP_Code --- */
    #define PARS_Get_HTTP_Code PARSP_Get_HTTP_Code
PARS_INT    PARSP_Get_HTTP_Code                         ( /*  in */ PARS_DWORD      a_dwErrorCode);

/*--- PARSP_ForceThreadRotation ----------------------------------------------------------- PARSP_ForceThreadRotation --- */
    #define PARS_ForceThreadRotation PARSP_ForceThreadRotation
PARS_VOID   PARSP_ForceThreadRotation                   ( PARS_VOID );

/*--- PARSP_DebugBreak ----------------------------------------------------------------------------- PARSP_DebugBreak --- */
    #define PARS_DebugBreak PARSP_DebugBreak
PARS_VOID   PARSP_DebugBreak                            ( /*  in */ PARS_DWORD    a_dwTraceLevel );

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|   Storage access functions                                                                  Storage access functions     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
#if PARS_MEMORY_TRACE
    typedef PARS_LPVOID  (DLLCALL *PARS_MALLOC)             ( /*  in */ PARS_DWORD        a_dwSize,
                                                              /*  in */ PARS_DWORD        a_dwLine,
                                                              /*  in */ PARS_LPSTR        a_strFile );
#else
    typedef PARS_LPVOID  (DLLCALL *PARS_MALLOC)             ( /*  in */ PARS_DWORD        a_dwSize );
#endif /* PARS_MEMORY_TRACE */

//PARS_RESULT     DLLCALL PARSP_StoreSystemData           ( /*  in */ PARS_LPSTR        a_strStorage,
//                                                          /*  in */ PARS_LPSTR        a_strFileName,
//                                                          /*  in */ PARS_LPVOID       a_pData,
//                                                          /*  in */ PARS_DWORD        a_dwDataLen);
//
//PARS_RESULT     DLLCALL PARSP_LoadSystemData            ( /*  in */ PARS_LPSTR        a_strStorage,
//                                                          /*  in */ PARS_LPSTR        a_strFileName,
//                                                          /*  in */ PARS_MALLOC       a_pMalloc,
//                                                          /* out */ PARS_LPVOID*      a_ppBuffer,
//                                                          /* out */ PARS_LPDWORD      a_pdwBuffersize);
//
//PARS_RESULT     DLLCALL PARSP_DeleteSystemData          ( /*  in */ PARS_LPSTR        a_strStorage,
//                                                          /*  in */ PARS_LPSTR        a_strFileName );

PARS_LPVOID DLLCALL PARS_SafeMalloc( /*  in */ PARS_DWORD     a_dwSize );


PARS_VOID DLLCALL PARS_SafeFree( /*  in */ PARS_LPVOID a_ptrPointer );

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|   Functions from C-runtime                                                                  Functions from C-runtime     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/

/*--- memory functions from C-runtime                                                  memory functions from C-runtime ---*/

#if PARS_OWN_MEMSET /*  PARS_memset --------------------------------------------------------------------- PARS_memset --- */
    #define PARS_memset PARSP_memset
    PARS_LPVOID  PARSP_memset                           ( /* out */ PARS_LPVOID         a_pvDst,
                                                          /*  in */ PARS_INT            a_nValue,
                                                          /*  in */ PARS_size_t         a_nCount);
#else /* PARS_OWN_MEMSET */
    #define PARS_memset pnd_memset
#endif /* PARS_OWN_MEMSET */

#if PARS_OWN_MEMCPY /*  PARS_memcpy --------------------------------------------------------------------- PARS_memcpy --- */
    #define PARS_memcpy PARSP_memcpy
    PARS_LPVOID  PARSP_memcpy                           ( /* out */ PARS_LPVOID         a_pvDst,
                                                          /*  in */ const PARS_VOID*    a_pvSrc,
                                                          /*  in */ PARS_size_t         a_nCount);
#else /* PARS_OWN_MEMCPY */
    #define PARS_memcpy pnd_memcpy
#endif /* PARS_OWN_MEMCPY */

// not used in XML Parser, therefore not defined!!
//#if PARS_OWN_MEMMOVE
//#  define PARS_memmove PARSP_memmove
//    PARS_LPVOID  PARSP_memmove                          ( /* out */ PARS_LPVOID         a_pvDst,
//                                                          /*  in */ const PARS_VOID*    a_pvSrc,
//                                                          /*  in */ PARS_size_t         a_nCount);
//#else /* PARS_OWN_MEMMOVE */
//#  define PARS_memmove pnd_memmove
//#endif /* PARS_OWN_MEMMOVE */
//
//#if PARS_OWN_MEMCMP /*  PARS_memcmp --------------------------------------------------------------------- PARS_memcmp --- */
//    #define PARS_memcmp PARSP_memcmp
//    PARS_INT     PARSP_memcmp                           ( /* out */ const PARS_VOID*    a_pvDst,
//                                                          /*  in */ const PARS_VOID*    a_pvSrc,
//                                                          /*  in */ PARS_size_t         a_nCount);
//#else /* PARS_OWN_MEMCMP */
//    #define PARS_memcmp pnd_memcmp
//#endif /* PARS_OWN_MEMCMP */

/*--- String functions from C-runtime                                                  String functions from C-runtime ---*/

#if PARS_OWN_STRNLEN /*  PARSP_strnlen ---------------------------------------------------------------- PARSP_strnlen --- */
    #define PARS_strnlen PARSP_strnlen
    PARS_DWORD  PARSP_strnlen                           ( /*  in */ PARS_LPCSTR         a_strToCheck,
                                                          /*  in */ PARS_DWORD          a_nMaxLen);
#else /* PARS_OWN_STRNLEN */
    #define PARS_strnlen pnd_strnlen
#endif /* PARS_OWN_STRNLEN */

#if PARS_OWN_STRLEN /*  PARSP_strlen ------------------------------------------------------------------- PARSP_strlen --- */
    #define PARS_strlen PARSP_strlen
    PARS_DWORD  PARSP_strlen                            ( /*  in */ PARS_LPCSTR         a_strToCheck);
#else /* PARS_OWN_STRLEN */
    #define PARS_strlen pnd_strlen
#endif /* PARS_OWN_STRLEN */

#if PARS_OWN_STRSTR /*  PARSP_strstr ------------------------------------------------------------------- PARSP_strstr --- */
    #define PARS_strstr PARSP_strstr
    PARS_LPSTR  PARSP_strstr                            ( /*  in */ PARS_LPCSTR         a_strToCheck,
                                                          /*  in */ PARS_LPCSTR         a_strSubString);
#else /* PARS_OWN_STRSTR */
    #define PARS_strstr pnd_strstr
#endif /* PARS_OWN_STRSTR */

//#if PARS_OWN_STRCAT /*  PARSP_strcat ------------------------------------------------------------------- PARSP_strcat --- */
//    #define PARS_strcat PARSP_strcat
//    PARS_LPSTR  PARSP_strcat                            ( /*  in */ PARS_LPSTR          a_strSource,
//                                                          /*  in */ PARS_LPCSTR         a_strToAdd);
//#else /* PARS_OWN_STRCAT */
//    #define PARS_strcat pnd_strcat
//#endif /* PARS_OWN_STRCAT */
//
//#if PARS_OWN_STRCPY /*  PARSP_strcpy ------------------------------------------------------------------- PARSP_strcpy --- */
//    #define PARS_strcpy PARSP_strcpy
//    PARS_LPSTR  PARSP_strcpy                            ( /*  in */ PARS_LPSTR          a_strSource,
//                                                          /*  in */ PARS_LPCSTR         a_strToAdd);
//#else /* PARS_OWN_STRCPY */
//    #define PARS_strcpy pnd_strcpy
//#endif /* PARS_OWN_STRCPY */

#if PARS_OWN_STRNCMP /*  PARSP_strncmp ---------------------------------------------------------------- PARSP_strncmp --- */
    #define PARS_strncmp PARSP_strncmp
    PARS_INT  PARSP_strncmp                             ( /*  in */ PARS_LPCSTR         a_strLeft,
                                                          /*  in */ PARS_LPCSTR         a_strRight,
                                                          /*  in */ PARS_DWORD          a_nMaxLen);
#else /* PARS_OWN_STRNCMP */
    #define PARS_strncmp pnd_strncmp
#endif /* PARS_OWN_STRNCMP */

#if PARS_OWN_STRCMP /*  PARSP_strcmp ------------------------------------------------------------------- PARSP_strcmp --- */
    #define PARS_strcmp PARSP_strcmp
    PARS_INT  PARSP_strcmp                              ( /*  in */ PARS_LPCSTR         a_strLeft,
                                                          /*  in */ PARS_LPCSTR         a_strRight);
#else /* PARS_OWN_STRCMP */
    #define PARS_strcmp pnd_strcmp
#endif /* PARS_OWN_STRCMP */

#if PARS_OWN_STRNICMP /*  PARSP_strnicmp ------------------------------------------------------------- PARSP_strnicmp --- */
    #define PARS_strnicmp PARSP_strnicmp
    PARS_INT  PARSP_strnicmp                            ( /*  in */ PARS_LPCSTR         a_strLeft,
                                                          /*  in */ PARS_LPCSTR         a_strRight,
                                                          /*  in */ PARS_LONG           a_nMaxLen);
#else /* PARS_OWN_STRNICMP */
    #define PARS_strnicmp pnd_strnicmp
#endif /* PARS_OWN_STRNICMP */

// not used in XML Parser, therefore not defined!!
//#if PARS_OWN_STRICMP /*  PARSP_stricmp ---------------------------------------------------------------- PARSP_stricmp --- */
//    #define PARS_stricmp PARSP_stricmp
//    PARS_INT  PARSP_stricmp                             ( /*  in */ PARS_LPCSTR         a_strLeft,
//                                                          /*  in */ PARS_LPCSTR         a_strRight);
//#else /* PARS_OWN_STRICMP */
//    #define PARS_stricmp pnd_stricmp
//#endif /* PARS_OWN_STRICMP */
//
//#if PARS_OWN_STRRCHR /*  PARSP_strrchr ---------------------------------------------------------------- PARSP_strrchr --- */
//    #define PARS_strrchr PARSP_strrchr
//    PARS_LPSTR  PARSP_strrchr                           ( /*  in */ PARS_LPCSTR         a_strLeft,
//                                                          /*  in */ PARS_INT            a_nCharacter);
//#else /* PARS_OWN_STRRCHR */
//    #define PARS_strrchr pnd_strrchr
//#endif /* PARS_OWN_STRRCHR */

#if PARS_OWN_STRCHR /*  PARSP_strchr ------------------------------------------------------------------- PARSP_strchr --- */
    #define PARS_strchr PARSP_strchr
    PARS_LPSTR  PARSP_strchr                            ( /*  in */ PARS_LPCSTR         a_strLeft,
                                                          /*  in */ PARS_INT            a_nCharacter);
#else /* PARS_OWN_STRCHR */
    #define PARS_strchr pnd_strchr
#endif /* PARS_OWN_STRCHR */

// not used in XML Parser, therefore not defined!!
//#if PARS_OWN_STRTOK /*  PARSP_strtok ------------------------------------------------------------------- PARSP_strtok --- */
//    #define PARS_strtok PARSP_strtok
//    PARS_LPSTR  PARSP_strtok                            ( /*  in */ PARS_LPSTR          a_strString,
//                                                          /*  in */ PARS_LPCSTR         a_strControl);
//#else /* PARS_OWN_STRTOK */
//    #define PARS_strtok pnd_strtok
//#endif /* PARS_OWN_STRTOK */
//
//#if PARS_OWN_STRPBRK /*  PARSP_strpbrk ---------------------------------------------------------------- PARSP_strpbrk --- */
//    #define PARS_strpbrk PARSP_strpbrk
//    PARS_LPSTR  PARSP_strpbrk                           ( /*  in */ PARS_LPCSTR         a_strString,
//                                                          /*  in */ PARS_LPCSTR         a_strControl);
//#else /* PARS_OWN_STRPBRK */
//    #define PARS_strpbrk pnd_strpbrk
//#endif /* PARS_OWN_STRPBRK */

#if PARS_OWN_STRCSPN /*  PARSP_strcspn ---------------------------------------------------------------- PARSP_strcspn --- */
    #define PARS_strcspn PARSP_strcspn
    PARS_INT  PARSP_strcspn                             ( /*  in */ PARS_LPCSTR         a_strString,
                                                          /*  in */ PARS_LPCSTR         a_strControl);
#else /* PARS_OWN_STRCSPN */
    #define PARS_strcspn pnd_strcspn
#endif /* PARS_OWN_STRCSPN */

#if PARS_OWN_STRSPN /*  PARSP_strspn ------------------------------------------------------------------- PARSP_strspn --- */
    #define PARS_strspn PARSP_strspn
    PARS_INT  PARSP_strspn                              ( /*  in */ PARS_LPCSTR         a_strString,
                                                          /*  in */ PARS_LPCSTR         a_strControl);
#else /* PARS_OWN_STRSPN */
    #define PARS_strspn pnd_strspn
#endif /* PARS_OWN_STRSPN */

#if PARS_OWN_STRREV /*  PARSP_strrev ------------------------------------------------------------------- PARSP_strrev --- */
    #define PARS_strrev PARSP_strrev
    PARS_LPSTR  PARSP_strrev                            ( /*  in */ PARS_LPSTR          a_strString);
#else /* PARS_OWN_STRREV */
    #define PARS_strrev pnd_strrev
#endif /* PARS_OWN_STRREV */

// not used in XML Parser, therefore not defined!!
//#if PARS_OWN_TOLOWER /*  PARSP_tolower ---------------------------------------------------------------- PARSP_tolower --- */
//    #define PARS_tolower PARSP_tolower
//    PARS_CHAR   PARSP_tolower                           ( /*  in */ PARS_CHAR           a_strString);
//#else /* PARS_OWN_TOLOWER */
//    #define PARS_tolower pnd_tolower
//#endif /* PARS_OWN_TOLOWER */
//
//#if PARS_OWN_STRLWR /*  PARSP_strlwr ------------------------------------------------------------------- PARSP_strlwr --- */
//    #define PARS_strlwr PARSP_strlwr
//    PARS_LPSTR   PARSP_strlwr                           ( /*  in */ PARS_LPSTR           a_strString);
//#else /* PARS_OWN_STRLWR */
//    #define PARS_strlwr pnd_strlwr
//#endif /* PARS_OWN_STRLWR */
//
//#if PARS_OWN_TOUPPER /*  PARSP_toupper ---------------------------------------------------------------- PARSP_toupper --- */
//    #define PARS_toupper PARSP_toupper
//    PARS_CHAR   PARSP_toupper                           ( /*  in */ PARS_CHAR           a_strString);
//#else /* PARS_OWN_TOUPPER */
//    #define PARS_toupper pnd_toupper
//#endif /* PARS_OWN_TOUPPER */
//
// not used in XML Parser, therefore not defined!!
//#if PARS_OWN_STRUPR /*  PARSP_strupr ------------------------------------------------------------------- PARSP_strupr --- */
//    #define PARS_strupr PARSP_strupr
//    PARS_LPSTR   PARSP_strupr                           ( /*  in */ PARS_LPSTR           a_strString);
//#else /* PARS_OWN_STRUPR */
//    #define PARS_strupr pnd_strupr
//#endif /* PARS_OWN_STRUPR */
//
///*--- Floating Point checks                                                                      Floating Point checks ---*/
//#if PARS_OWN_ISNAN /*  PARSP_isNan ---------------------------------------------------------------------- PARSP_isNan --- */
//    #define PARS_isNan PARSP_isNan
//    PARS_INT    PARSP_isNan                             ( /*  in */ PARS_DOUBLE         a_dblToCheck);
//#else /* PARS_OWN_STRSPN */
//    #define PARS_isNan pnd_isnan
//#endif /* PARS_OWN_STRSPN */
//
//#if PARS_OWN_ISFINITE /*  PARSP_isFinite ------------------------------------------------------------- PARSP_isFinite --- */
//    #define PARS_finite PARSP_isfinite
//    PARS_INT    PARSP_isfinite                          ( /*  in */ PARS_DOUBLE         a_dblToCheck);
//#else /* PARS_OWN_STRSPN */
//    #define PARS_finite pnd_finite
//#endif /* PARS_OWN_STRSPN */
//
///*--- printf derivate                                                                                  printf derivate ---*/
//
//#if PARS_OWN_SPRINTF /*  PARSP_sprintf ---------------------------------------------------------------- PARSP_sprintf --- */
//    #define PARS_sprintf PARSP_sprintf
//    PARS_LPSTR  PARSP_sprintf                           ( /*  in */ PARS_LPSTR          a_strBuffer,
//                                                          /*  in */ PARS_LPCSTR         a_strCommand,
//                                                          /*  in */ PARS_va_list        a_Parameter);
//#else /* PARS_OWN_SPRINTF */
//    #define PARS_sprintf pnd_sprintf
//#endif /* PARS_OWN_SPRINTF */
//
//#if PARS_OWN_VSNPRINTF /*  PARSP_vsnprintf ---------------------------------------------------------- PARSP_vsnprintf --- */
//    #define PARS_vsnprintf PARSP_vsnprintf
//    PARS_INT    PARSP_vsnprintf                         ( /*  in */ PARS_LPSTR          a_strBuffer,
//                                                          /*  in */ PARS_INT            a_nBufferSize,
//                                                          /*  in */ PARS_LPCSTR         a_strCommand,
//                                                          /*  in */ PARS_va_list        a_Parameter);
//#else /* PARS_OWN_VSNPRINTF */
//    #define PARS_vsnprintf(a,b,c,d) pnd_vsnprintf(a,b,c,d)
//#endif /* PARS_OWN_VSNPRINTF */
//
// sscanf not used in XML Parser, therefore not defined!!
//#if PARS_OWN_SSCANF /*  PARSP_sscanf ------------------------------------------------------------------- PARSP_sscanf --- */
//    #define PARS_sscanf PARSP_sscanf
//    PARS_LPSTR  PARSP_sscanf                            ( /*  in */ PARS_LPCSTR         a_strBuffer,
//                                                          /*  in */ PARS_LPCSTR         a_strFormat,
//                                                          /*  in */ ...);
//#else /* PARS_OWN_SSCANF */
//    #define PARS_sscanf pnd_sscanf
//#endif /* PARS_OWN_SSCANF */

/*--- a_strToCheck to number functions                                                            a_strToCheck to number functions ---*/

// not used in XML Parser, therefore not defined!!
//#if PARS_OWN_ATOI /*  PARSP_atoi ------------------------------------------------------------------------- PARSP_atoi --- */
//    #define PARS_atoi PARSP_atoi
//    PARS_INT    PARSP_atoi                              ( /*  in */ PARS_LPCSTR         a_strBuffer);
//#else /* PARS_OWN_ATOI */
//    #define PARS_atoi pnd_atoi
//#endif /* PARS_OWN_ATOI */
//
//#if PARS_OWN_ATOL /*  PARSP_atol ------------------------------------------------------------------------- PARSP_atol --- */
//    #define PARS_atol PARSP_atol
//    PARS_LONG   PARSP_atol                              ( /*  in */ PARS_LPCSTR         a_strBuffer);
//#else /* PARS_OWN_ATOL */
//    #define PARS_atol pnd_atol
//#endif /* PARS_OWN_ATOL */
//
//#if PARS_OWN_ITOA /*  PARSP_itoa ------------------------------------------------------------------------- PARSP_itoa --- */
//    #define PARS_itoa PARSP_itoa
//    PARS_LPSTR  PARSP_itoa                              ( /*  in */ PARS_INT            a_nValue,
//                                                          /*  in */ PARS_LPSTR          a_strBuffer,
//                                                          /*  in */ PARS_INT            a_nRadix);
//#else /* PARS_OWN_ITOA */
//    #define PARS_itoa pnd_itoa
//#endif /* PARS_OWN_ITOA */
//
//#if PARS_OWN_LTOA /*  PARSP_ltoa ------------------------------------------------------------------------- PARSP_ltoa --- */
//    #define PARS_ltoa PARSP_ltoa
//    PARS_LPSTR  PARSP_ltoa                              ( /*  in */ PARS_INT            a_nValue,
//                                                          /*  in */ PARS_LPSTR          a_strBuffer,
//                                                          /*  in */ PARS_INT            a_nRadix);
//#else /* PARS_OWN_LTOA */
//    #define PARS_ltoa pnd_ltoa
//#endif /* PARS_OWN_LTOA */
//
//#if PARS_OWN_ULTOA /*  PARSP_ultoa ---------------------------------------------------------------------- PARSP_ultoa --- */
//    #define PARS_ultoa PARSP_ultoa
//    PARS_LPSTR  PARSP_ultoa                             ( /*  in */ PARS_DWORD          a_nValue,
//                                                          /*  in */ PARS_LPSTR          a_strBuffer,
//                                                          /*  in */ PARS_INT            a_nRadix);
//#else /* PARS_OWN_ULTOA */
//    #define PARS_ultoa pnd_ultoa
//#endif /* PARS_OWN_ULTOA */
//
//#if PARS_OWN_WRMDIR /*  PARSP_wrmdir ---------------------------------------------------------------------- PARSP_wrmdir --- */
//    #define PARS_wrmdir PARSP_wrmdir
//    PARS_INT  PARSP_wrmdir                            ( /*  in */ const wchar_t *dirname);
//#else /* PARS_OWN_WRMDIR */
//    #define PARS_wrmdir pnd_wrmdir
//#endif /* PARS_OWN_WRMDIR */
//
//#if PARS_OWN_WREMOVE /*  PARSP_wremove ---------------------------------------------------------------------- PARSP_wremove --- */
//    #define PARS_wremove PARSP_wremove
//    PARS_INT  PARSP_wremove                            ( /*  in */ const wchar_t *path);
//#else /* PARS_OWN_WREMOVE */
//    #define PARS_wremove pnd_wremove
//#endif /* PARS_OWN_WREMOVE */
//
// Following functions are not used by XML Parser therefore they not defined!!
//#if PARS_OWN_TZSET /*  PARSP_tzset ---------------------------------------------------------------------- PARSP_tzset --- */
//    #define PARS_tzset PARSP_tzset
//    void  PARSP_tzset                                   (void);
//#else /* PARS_OWN_TZSET */
//    #define PARS_tzset pnd_tzset
//#endif /* PARS_OWN_TZSET */
//
//#if PARS_OWN_GET_TIMEZONE /*  PARSP_get_timezone ---------------------------------------------------------------------- PARSP_get_timezone --- */
//    #define PARS_get_timezone PARSP_get_timezone
//    PARS_INT  PARSP_get_timezone                        ( /* out */ PARS_INT* seconds);
//#else /* PARS_OWN_GET_TIMEZONE */
//    #define PARS_get_timezone pnd_get_timezone
//#endif /* PARS_OWN_GET_TIMEZONE */
//
//#if PARS_OWN_GET_DSTBIAS /*  PARSP_get_dstbias ---------------------------------------------------------------------- PARSP_get_dstbias --- */
//    #define PARS_get_dstbias PARSP_get_dstbias
//    PARS_INT  PARSP_get_dstbias                         ( /* out */ PARS_INT* seconds);
//#else /* PARS_OWN_GET_DSTBIAS */
//    #define PARS_get_dstbias pnd_get_dstbias
//#endif /* PARS_OWN_GET_DSTBIAS */

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|     String functions                                                                                String functions     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
PARS_DWORD DLLCALL MSTR_Len                        ( /*  in */ PARS_LPSTR  a_mstrSource);

PARS_DWORD DLLCALL PARS_MSTR_MaxLen                     ( /*  in */ PARS_LPSTR  a_mstrSource,
                                                          /*  in */ PARS_DWORD      a_dwMaxLen);

PARS_LPSTR DLLCALL MSTR_Raw                  ( /*  in */ PARS_LPSTR  a_mstrSource);

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|     Char conversion and checking                                                        Char conversion and checking     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
#define PARS_isspace(x)  (x==0x20)
#define PARS_isdigit(x)  ((x>=0x30)&&(x<=0x39))
#define PARS_isalpha(x)  ( ((x >= 'A') && (x <= 'Z')) || ((x >= 'a') && (x <= 'z')) )
#define PARS_isprint(x)  ((x >= 0x20) && (x <= 0x7E))
#define PARS_ishex(x)    (((x)>='0'&&(x)<='9') && ((x)>='A'&&(x)<='F') && ((x)>='a'&&(x)<= 'f'))
#define PARS_isalnum(x)  (PARS_isdigit(x) || PARS_isalpha(x))

#define PARS_MAX(a,b) (((a)>(b))?(a):(b))
#define PARS_MIN(a,b) (((a)<(b))?(a):(b))
#define PARS_ABS(a)   (((a)>=0)?(a):((a)*(-1)))

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|     Limits                                                                                                    Limits     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/
//
///*#define OPC_DECIMAL_MIN     0.0001*/
//#define OPC_DECIMAL_MIN     0.0000  /* Even the OPC Spec 1.01 says at the moment min=0.0001*/
//                                    /* see "http://www.w3.org/TR/xmlschema-2/#decimal" -> "3.2.3 decimal" */
//#define OPC_DECIMAL_MAX     922337203685477.5807 /* something like that */
//#define MAX_DECIMAL_MANTISSA    "9007199254740991"
//
//#define UCHAR_MIN   ((PARS_UCHAR)0)         /* minimum unsigned char value  */
//
//#ifndef _INC_LIMITS
//#define _INC_LIMITS
//
//#ifndef CHAR_BIT
//#define CHAR_BIT      8                     /* number of bits in a char     */
//#endif
//#ifndef SCHAR_MIN
//#define SCHAR_MIN   ((PARS_CHAR)-128)       /* minimum signed char value    */
//#endif
//#ifndef SCHAR_MAX
//#define SCHAR_MAX   ((PARS_CHAR) 127)       /* maximum signed char value    */
//#endif
//#ifndef UCHAR_MAX
//#define UCHAR_MAX   ((PARS_UCHAR)0xff)      /* maximum unsigned char value  */
//#endif
//
//#ifndef _CHAR_UNSIGNED
//#ifndef CHAR_MIN
//#define CHAR_MIN    SCHAR_MIN   /* minimum char value */
//#endif
//#ifndef CHAR_MAX
//#define CHAR_MAX    SCHAR_MAX   /* maximum char value */
//#endif
//#else
//#define CHAR_MIN      0
//#define CHAR_MAX    UCHAR_MAX
//#endif  /* _CHAR_UNSIGNED */
//
//#ifndef MB_LEN_MAX
//#define MB_LEN_MAX    2                         /* max. # bytes in multi byte char */
//#endif
//#ifndef SHRT_MIN
//#define SHRT_MIN    (-32768)                    /* minimum (signed) short value */
//#endif
//#ifndef SHRT_MAX
//#define SHRT_MAX      32767                     /* maximum (signed) short value */
//#endif
//#ifndef USHRT_MAX
//#define USHRT_MAX     0xffff                    /* maximum unsigned short value */
//#endif
//#ifndef INT_MIN
//#define INT_MIN     (-2147483647 - 1)           /* minimum (signed) int value */
//#endif
//#ifndef INT_MAX
//#define INT_MAX       2147483647                /* maximum (signed) int value */
//#endif
//#ifndef UINT_MAX
//#define UINT_MAX      ((PARS_UINT)0xffffffff)   /* maximum unsigned int value */
//#endif
//#ifndef LONG_MIN
//#define LONG_MIN    (-2147483647L - 1)          /* minimum (signed) long value */
//#endif
//#ifndef LONG_MAX
//#define LONG_MAX      2147483647L               /* maximum (signed) long value */
//#endif
//#ifndef ULONG_MAX
//#define ULONG_MAX     0xffffffffUL              /* maximum unsigned long value */
//#endif
//
//#if     _INTEGRAL_MAX_BITS >= 8
//#define _I8_MIN     (-127i8 - 1)    /* minimum signed 8 bit value */
//#define _I8_MAX       127i8         /* maximum signed 8 bit value */
//#define _UI8_MAX      0xffui8       /* maximum unsigned 8 bit value */
//#endif
//
//#if     _INTEGRAL_MAX_BITS >= 16
//#define _I16_MIN    (-32767i16 - 1) /* minimum signed 16 bit value */
//#define _I16_MAX      32767i16      /* maximum signed 16 bit value */
//#define _UI16_MAX     0xffffui16    /* maximum unsigned 16 bit value */
//#endif
//
//#if     _INTEGRAL_MAX_BITS >= 32
//#define _I32_MIN    (-2147483647i32 - 1) /* minimum signed 32 bit value */
//#define _I32_MAX      2147483647i32 /* maximum signed 32 bit value */
//#define _UI32_MAX     0xffffffffui32 /* maximum unsigned 32 bit value */
//#endif
//
//#if     _INTEGRAL_MAX_BITS >= 64
///* minimum signed 64 bit value */
//#define _I64_MIN    (-9223372036854775807i64 - 1)
///* maximum signed 64 bit value */
//#define _I64_MAX      9223372036854775807i64
///* maximum unsigned 64 bit value */
//#define _UI64_MAX     0xffffffffffffffffui64
//#endif
//
//#if     _INTEGRAL_MAX_BITS >= 128
///* minimum signed 128 bit value */
//#define _I128_MIN   (-170141183460469231731687303715884105727i128 - 1)
///* maximum signed 128 bit value */
//#define _I128_MAX     170141183460469231731687303715884105727i128
///* maximum unsigned 128 bit value */
//#define _UI128_MAX    0xffffffffffffffffffffffffffffffffui128
//#endif
//
//#ifdef  _POSIX_
//
//#define _POSIX_ARG_MAX      4096
//#define _POSIX_CHILD_MAX    6
//#define _POSIX_LINK_MAX     8
//#define _POSIX_MAX_CANON    255
//#define _POSIX_MAX_INPUT    255
//#define _POSIX_NAME_MAX     14
//#define _POSIX_NGROUPS_MAX  0
//#define _POSIX_OPEN_MAX     16
//#define _POSIX_PATH_MAX     255
//#define _POSIX_PIPE_BUF     512
//#define _POSIX_SSIZE_MAX    32767
//#define _POSIX_STREAM_MAX   8
//#define _POSIX_TZNAME_MAX   3
//
//#define ARG_MAX             14500       /* 16k heap, minus overhead */
//#define LINK_MAX            1024
//#define MAX_CANON           _POSIX_MAX_CANON
//#define MAX_INPUT           _POSIX_MAX_INPUT
//#define NAME_MAX            255
//#define NGROUPS_MAX         16
//#define OPEN_MAX            32
//#define PATH_MAX            512
//#define PIPE_BUF            _POSIX_PIPE_BUF
//#define SSIZE_MAX           _POSIX_SSIZE_MAX
//#define STREAM_MAX          20
//#define TZNAME_MAX          10
//
//#endif  /* POSIX */
//
//#endif  /* _INC_LIMITS */
//
///* Ranges for FLOAT (single) */
///* http://asphelper.de/ASPKurs/A300101.asp */
///* single precision floating point with a range of */
///* -3,402823E38 to -1,401298E-45 for negatives and */
///* 1,401298E-45 to 3,402823E38 for positive values. */
//#define PARS_FLOAT_NEGATIVE_NEAR_ZERO       -1.401298E-45
//#define PARS_FLOAT_NEGATIVE_FAR_ZERO        -3.402823E38
//#define PARS_FLOAT_POSITIVE_NEAR_ZERO       1.401298E-45
//#define PARS_FLOAT_POSITIVE_FAR_ZERO        3.402823E38

PARS_END_EXTERN_C

#endif /* _PARSP_UTILS_H_DEFINED_ */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

