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
/*  F i l e               &F: PARSP_Types.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  !!! File from the port layer. !!!                                        */
/*  This file contains the type definitions of xml parser.                   */
/*                                                                           */
/*****************************************************************************/

/* ---------------------------------------------------------------------------------------------------------------------- */
/* --- !!! PORTLAYER !! -- !! PORTLAYER !! -- !! PORTLAYER !! - !! PORTLAYER !! -- !! PORTLAYER !! -- !! PORTLAYER !! --- */
/* ---------------------------------------------------------------------------------------------------------------------- */

#ifndef __PARSP_OS_TYPES_DEFINED__
#define __PARSP_OS_TYPES_DEFINED__

#include "PARSP_Config.h"

/* set this define, if you wish wide char support within xml parser */
/* #define PARS_USE_WIDE_CHAR */

/*----------------------------------------------------------------------------------------------------*\
|   Common Shortcuts                                                                Common Shortcuts   |
\*----------------------------------------------------------------------------------------------------*/
#define PARS_INT_MAX    2147483647                    /* maximum (signed) PARS_INT value, used by RSA */
#define PARS_INT_LEN    11                      /* length of the textual representation of an integer */
#define PARS_FLOAT_LEN  32                         /* length of the textual representation of a float */
#define PARS_DOUBLE_LEN (2*PARS_FLOAT_LEN)        /* length of the textual representation of a double */

/* get the count of DWORDs of the struct, for a correct Spare alignment */
#define PARS_DWORD_SIZEOF(x)    ((sizeof(x)-1+sizeof(PARS_DWORD))/sizeof(PARS_DWORD))

/*----------------------------------------------------------------------------------------------------*\
|   Wide char support                                                              Wide char support   |
\*----------------------------------------------------------------------------------------------------*/
#if PARS_USE_WIDE_CHAR
  /*  is PARS_TCHAR defined                                                                 PARS_TCHAR*/
  #ifndef PARS_TCHAR
    typedef unsigned short PARS_TCHAR;
  #endif
#else /* PARS_USE_WIDE_CHAR */
  /*  is PARS_TCHAR defined                                                                 PARS_TCHAR*/
  #ifndef PARS_TCHAR
    typedef char PARS_TCHAR;
  #endif
#endif /* PARS_USE_WIDE_CHAR */

/*    is PARS_LPTSTR defined                                                              PARS_LPTSTR */
#ifndef PARS_LPTSTR
  typedef PARS_TCHAR*           PARS_LPTSTR;
#endif
/*    is PARS_LPCTSTR defined                                                            PARS_LPCTSTR */
#ifndef PARS_LPCTSTR
  typedef const PARS_TCHAR*     PARS_LPCTSTR;
#endif

/*----------------------------------------------------------------------------------------------------*\
|   PARS_UNREFERENCED_PARAMETER                                          PARS_UNREFERENCED_PARAMETER   |
\*----------------------------------------------------------------------------------------------------*/
/** Use this macro, if you have unreferenced parameters in a function */
#ifndef PARS_UNREFERENCED_PARAMETER
#  define PARS_UNREFERENCED_PARAMETER(x)      (PARS_VOID)x
#endif /* PARS_UNREFERENCED_PARAMETER */
/*----------------------------------------------------------------------------------------------------*\
|   Standard Data types                                                          Standard Data types   |
\*----------------------------------------------------------------------------------------------------*/

/*    is PARS_BOOL defined                                                                  PARS_BOOL */
#ifndef    PARS_BOOL
    typedef    int          PARS_BOOL;
#endif

/*    is PARS_FALSE defined                                                                PARS_FALSE */
#ifndef PARS_FALSE
    #define PARS_FALSE           0
#endif
/*    is PARS_TRUE defined                                                                  PARS_TRUE */
#ifndef PARS_TRUE
    #define PARS_TRUE            (!PARS_FALSE)
#endif
/*    is PARS_LPBOOL defined                                                              PARS_LPBOOL */
#ifndef PARS_LPBOOL
    typedef PARS_BOOL*           PARS_LPBOOL;
#endif
/*    is PARS_BYTE defined                                                                  PARS_BYTE */
#ifndef PARS_BYTE
    typedef unsigned char   PARS_BYTE;
#endif
/*    is PARS_CHAR defined                                                                  PARS_CHAR */
#ifndef PARS_CHAR
    typedef char             PARS_CHAR;
#endif
/*    is PARS_WCHAR defined                                                                PARS_WCHAR */
#ifndef PARS_WCHAR
    typedef unsigned short     PARS_WCHAR;  /* signed short is problematic with Windows CE */
#endif
/*    is PARS_UCHAR defined                                                                PARS_UCHAR */
#ifndef PARS_UCHAR
    typedef unsigned char   PARS_UCHAR;
#endif
/*    is PARS_WORD defined                                                                  PARS_WORD */
#ifndef PARS_WORD
    typedef unsigned short  PARS_WORD;
#endif
/*    is PARS_SHORT defined                                                                PARS_SHORT */
#ifndef PARS_SHORT
    typedef short int       PARS_SHORT;
#endif
/*    is PARS_USHORT defined                                                              PARS_USHORT */
#ifndef PARS_USHORT
typedef unsigned short int       PARS_USHORT;
#endif
/*    is PARS_DWORD defined                                                                PARS_DWORD */
#ifndef PARS_DWORD
    typedef unsigned long   PARS_DWORD;
#endif
/*    is PARS_LONG defined                                                                  PARS_LONG */
#ifndef PARS_LONG
typedef long int        PARS_LONG;
#endif
/*    is PARS_ULONG defined                                                                 PARS_LONG */
#ifndef PARS_ULONG
typedef unsigned long int        PARS_ULONG;
#endif /* PARS_ULONG */
#if PARS_DEFINES_64_BIT_DATATYPES
/*    is PARS_LONGLONG defined                                                          PARS_LONGLONG */
#ifndef PARS_LONGLONG
typedef __int64                 PARS_LONGLONG;
#endif /* PARS_LONGLONG */
/*    is PARS_ULONGLONG defined                                                        PARS_ULONGLONG */
#ifndef PARS_ULONGLONG
typedef unsigned __int64        PARS_ULONGLONG;
#endif /* PARS_ULONGLONG */
#endif /* PARS_DEFINES_64_BIT_DATATYPES */
/*    is PARS_UINT defined                                                                  PARS_UINT */
#ifndef PARS_UINT
typedef unsigned int    PARS_UINT;
#endif
/*    is PARS_INT defined                                                                    PARS_INT */
#ifndef PARS_INT
typedef int    PARS_INT;
#endif
/*    is PARS_FLOAT defined                                                                PARS_FLOAT */
#ifndef PARS_FLOAT
typedef float     PARS_FLOAT;
#endif

/*    is PARS_DOUBLE defined                                                              PARS_DOUBLE */
#ifndef PARS_DOUBLE
typedef double     PARS_DOUBLE;
#endif

/*    is PARS_DECIMAL defined                                                              PARS_DOUBLE */
#ifndef PARS_DECIMAL
typedef double     PARS_DECIMAL;
#endif
/* PARS_DECIMAL is the corresponding Variable Type for VT_DECIMAL. Due xml parser does not support    */
/* the decimal data structure decimal is mapped to double, which is the largest data type supported   */

/*----------------------------------------------------------------------------------------------------*\
|   Pointers to standard types                                            Pointers to standard types   |
\*----------------------------------------------------------------------------------------------------*/

/*    is PARS_LPBYTE defined                                                              PARS_LPBYTE */
#ifndef PARS_LPBYTE
typedef PARS_BYTE*           PARS_LPBYTE;
#endif
/*    is PARS_LPCHAR defined                                                              PARS_LPCHAR */
#ifndef PARS_LPCHAR
typedef PARS_CHAR*           PARS_LPCHAR;
#endif
/*    is PARS_LPWORD defined                                                              PARS_LPWORD */
#ifndef PARS_LPWORD
typedef PARS_WORD*           PARS_LPWORD;
#endif
/*    is PARS_LPSHORT defined                                                            PARS_LPSHORT */
#ifndef PARS_LPSHORT
typedef PARS_SHORT*          PARS_LPSHORT;
#endif
/*    is PARS_LPDWORD defined                                                            PARS_LPDWORD */
#ifndef PARS_LPDWORD
typedef PARS_DWORD*          PARS_LPDWORD;
#endif
/*    is PARS_LPLONG defined                                                              PARS_LPLONG */
#ifndef PARS_LPLONG
typedef PARS_LONG*           PARS_LPLONG;
#endif
/*    is PARS_LPINT defined                                                                PARS_LPINT */
#ifndef PARS_LPINT
typedef PARS_INT*           PARS_LPINT;
#endif
/*    is PARS_LPUINT defined                                                              PARS_LPUINT */
#ifndef PARS_LPUINT
typedef PARS_UINT*           PARS_LPUINT;
#endif
/*    is PARS_LPFLOAT defined                                                            PARS_LPFLOAT */
#ifndef PARS_LPFLOAT
typedef PARS_FLOAT*    PARS_LPFLOAT;
#endif
/*    is PARS_LPDOUBLE defined                                                          PARS_LPDOUBLE */
#ifndef PARS_LPDOUBLE
typedef PARS_DOUBLE*    PARS_LPDOUBLE;
#endif

/*----------------------------------------------------------------------------------------------------*\
|   String Data types                                                              String Data types   |
\*----------------------------------------------------------------------------------------------------*/
/*    is PARS_LPSTR defined                                                                PARS_LPSTR */
#ifndef PARS_LPSTR
typedef PARS_CHAR*           PARS_LPSTR;
#endif
/*    is PARS_LPCSTR defined                                                               PARS_LPSTR */
#ifndef PARS_LPCSTR
typedef const PARS_CHAR*     PARS_LPCSTR;
#endif

/*    is PARS_LPSTR defined                                                                PARS_LPSTR */
#ifndef PARS_LPWSTR
typedef PARS_WCHAR*           PARS_LPWSTR;
#endif
/*    is PARS_LPCSTR defined                                                               PARS_LPSTR */
#ifndef PARS_LPCWSTR
typedef const PARS_WCHAR*     PARS_LPCWSTR;
#endif

/*----------------------------------------------------------------------------------------------------*\
|   Type definitions                                                                Type definitions   |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*\
|   Void Pointers                                                                      Void Pointers   |
\*----------------------------------------------------------------------------------------------------*/
/*    is PARS_VOID defined                                                                  PARS_VOID */
#ifndef PARS_VOID
#define PARS_VOID void
#endif
/*    is PARS_LPVOID defined                                                              PARS_LPVOID */
#ifndef PARS_LPVOID
typedef PARS_VOID*           PARS_LPVOID;
#endif
/*    is PARS_FPVOID defined                                                              PARS_FPVOID */
#ifndef PARS_FPVOID
typedef PARS_VOID ((* PARS_FPVOID)(PARS_VOID));
#endif

/*----------------------------------------------------------------------------------------------------*\
|   Several types                                                                      Several types   |
\*----------------------------------------------------------------------------------------------------*/
/*    is PARS_size_t defined                                                              PARS_size_t */
#ifndef PARS_SIZE_T_DEFINED
typedef unsigned int PARS_size_t;
#define PARS_SIZE_T_DEFINED
#endif  /* PARS_SIZE_T_DEFINED */

/* Prototype for Thread Start Routine */
typedef PARS_DWORD (*PARSP_LPTHREAD_START_ROUTINE)  ( /*  in */ PARS_LPVOID       lpThreadParameter);

/*----------------------------------------------------------------------------------------------------*\
|   Access Macros                                                                      Access Macros   |
\*----------------------------------------------------------------------------------------------------*/

/*    is PARS_LOWORD defined                                                              PARS_LOWORD */
#ifndef PARS_LOWORD
#define PARS_LOWORD(l)       ((PARS_WORD)(l& 0xFFFF))
#endif
/*    is PARS_HIWORD defined                                                              PARS_HIWORD */
#ifndef PARS_HIWORD
#define PARS_HIWORD(l)       ((PARS_WORD)(((PARS_DWORD)(l) >> 16) & 0xFFFF))
#endif
/*    is PARS_NULL defined                                                                  PARS_NULL */
#ifndef PARS_NULL
#  if PARS_BUILD_CPP
#    define PARS_NULL    NULL
#  else /* PARS_BUILD_CPP */
#    define PARS_NULL    ((PARS_VOID *)0)
#  endif /* PARS_BUILD_CPP */
#endif

/*----------------------------------------------------------------------------------------------------*\
|   specific types                                                                    specific types   |
\*----------------------------------------------------------------------------------------------------*/

/*    is PARS_RESULT defined                                                              PARS_RESULT */
#ifndef PARS_RESULT
    typedef PARS_DWORD       PARS_RESULT;
#endif
/*    is PARS_HDLLMODULE defined                                                      PARS_HDLLMODULE */
#ifndef PARS_HDLLMODULE
typedef PARS_LPVOID      PARS_HDLLMODULE;
#endif

/* IPv4 Sizes and lengths   */
#define PARS_IPV4_PAD_SIZE                  8   /* Number of padding bytes in a V4 address          */
#define PARS_IPV4_ADDR_BIN_LEN              4   /* binary representation is 4 Bytes                 */
#define PARS_IPV4_ADDR_STR_LEN              16  /* string is 16 bytes (including trailing 0)        */
/* IPv6 Sizes and lengths   */
#define PARS_IPV6_ADDR_BIN_LEN              16  /* binary representation is 16 Bytes                */
#define PARS_IPV6_ADDR_STR_LEN              40  /* including trailing 0x0 in a string               */

/*----------------------------------------------------------------------------------------------------*\
|   Typedefs                                                                                Typedefs   |
\*----------------------------------------------------------------------------------------------------*/
typedef PARS_LPVOID      PARS_RAW_SOCKET;                  /* the Socket handle from PARSP_Socket.h   */
typedef PARS_DWORD       PARS_SOCK_IP_ADR_BIN;             /* IP address in binary representation     */
typedef PARS_INT         PARS_SOCK_RC;                     /* return code of socket calls             */

typedef struct PARS_rema_record_type { 
    PARS_LPSTR  recordData;          
    PARS_LPSTR  recordLen;           
    PARS_DWORD  recordType;
} PARS_PND_REMA_RECORD_TYPE, *PARS_PND_REMA_RECORD_PTR_TYPE;

typedef struct PARS_rema_data_type { 
    PARS_PND_REMA_RECORD_TYPE  ipSuiteRecord;          
    PARS_PND_REMA_RECORD_TYPE  nosRecord;           
    PARS_PND_REMA_RECORD_TYPE  mib2Record;
} PARS_PND_REMA_DATA_TYPE, *PARS_PND_REMA_DATA_PTR_TYPE;

struct PARS_in_addr {
    union {
        struct
        {
            PARS_BYTE s_b1,s_b2,s_b3,s_b4;      /* 4 octet ip address                                 */
        } S_un_b;
        struct
        {
            PARS_WORD s_w1,s_w2;                /* High word, low word ip address                     */
        } S_un_w;
        PARS_DWORD S_addr;                      /* One Doubleword ip adress                           */
    } S_un;
/* Structe access already defined for CPU 1500, so we omit it here */
#if 0
#define s_addr  S_un.S_addr                     /* can be used for most tcp & ip code                 */
#define s_host  S_un.S_un_b.s_b2                /* host on imp                                        */
#define s_net   S_un.S_un_b.s_b1                /* network                                            */
#define s_imp   S_un.S_un_w.s_w2                /* imp                                                */
#define s_impno S_un.S_un_b.s_b4                /* imp #                                              */
#define s_lh    S_un.S_un_b.s_b3                /* logical host                                       */
#endif /* 0 */
};

#if PARS_IPV6_SUPPORT
struct PARS_in_addr6
{
    union{
        PARS_BYTE   _S6_u8[PARS_IPV6_ADDR_BIN_LEN];         /* IPv6 address, byte array*/
        PARS_DWORD  _S6_u32[(PARS_IPV6_ADDR_BIN_LEN/4)];    /* IPv6 address, DWORD array */
    } _S6_un;
};
#endif /* PARS_IPV6_SUPPORT */
typedef PARS_WORD        PARS_SOCK_IP_PORT;                /* IP port (in host byte order)            */

/*----------------------------------------------------------------------------------------------------*\
|   Portable socket structure                                              Portable socket structure   |
\*----------------------------------------------------------------------------------------------------*/
struct PARS_sockaddr_storage {
    PARS_SHORT        ss_family;        /* Address family (PARS_SOCK_AF_INET or PARS_SOCK_AF_INET6) */
    PARS_SOCK_IP_PORT port;             /* Port                 */
    union{
        struct{
            struct PARS_in_addr sin_addr;
            PARS_CHAR sin_zero[PARS_IPV4_PAD_SIZE];
        }v4addr;                        /* IPv4 Content         */
#if PARS_IPV6_SUPPORT
        struct{
            PARS_ULONG sin6_flowinfo;
            struct PARS_in_addr6 sin_addr;
            PARS_ULONG sin6_scope_id;
        }v6addr;                        /* IPv6 Content         */
#endif /* PARS_IPV6_SUPPORT */
    }data;
};
typedef struct PARS_sockaddr_storage    PARS_SOCKADDR_STORAGE;
typedef struct PARS_sockaddr_storage   *PARS_PSOCKADDR_STORAGE;

#endif /* __PARSP_OS_TYPES_DEFINED__ */
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

