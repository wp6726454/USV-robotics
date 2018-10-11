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
/*  F i l e               &F: PARS_ConversionService.h                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface definition of the conversion service                           */
/*                                                                           */
/*****************************************************************************/

#ifndef PARS_CONVSRVC_DEFINED
#define PARS_CONVSRVC_DEFINED

PARS_BEGIN_EXTERN_C

/*----------------------------------------------------------------------------------------------------*\
|   Support for Dynamic Binding                                          Support for Dynamic Binding   |
\*----------------------------------------------------------------------------------------------------*/
#ifndef STATIC_BINDING
PARS_RESULT DLLCALL InitializeService               ( /*  in */ PARS_LPSTR       a_strServiceName,
                                                      /*  in */ PARS_PT_MCB      a_pContext,
                                                      /*  in */ PARS_HDLLMODULE  a_hModuleHandle);
#endif

/*----------------------------------------------------------------------------------------------------*\
|   XML Tag Constants                                                              XML Tag Constants   |
\*----------------------------------------------------------------------------------------------------*/
#define PARS_CONV_XML_TAG_CODEPAGE      "CODEPAGE"
#define PARS_CONV_XML_TAG_CHARACTER     "CHAR"
#define PARS_CONV_XML_ATTRIBUTE_ASCII   "ASCII"
#define PARS_CONV_XML_ATTRIBUTE_UCS     "UCS"

/*----------------------------------------------------------------------------------------------------*\
|   CodePage                                                                                CodePage   |
\*----------------------------------------------------------------------------------------------------*/

typedef struct PARS_CONV_S_CodePage
{
    PARS_CHAR       achName[PARS_CONVS_MAX_CP_NAME_LENGTH];
    PARS_DWORD      dwMapEntries;   /* how many map entry are in the code page */
    PARS_BOOL       fFreeMem;       /* Must code page be freed after use */
    PARS_LPDWORD    aUTFChar;       /* Array of UTF Characters */
    PARS_LPDWORD    aASCIIChar;     /* Array of ASCII Characters */ /* sort criteria */
    PARS_LPDWORD    aMapUTFToAscii; /* Table entries sorted by UTF */
} PARS_CONV_T_CodePage;

typedef PARS_CHAR    PARS_CONV_T_CPNameList[PARS_CONVS_MAX_CODEPAGE_COUNT][PARS_CONVS_MAX_CP_NAME_LENGTH];
typedef PARS_CONV_T_CPNameList    *PARS_CONV_P_CPNameList;

/*----------------------------------------------------------------------------------------------------*\
|   Conversion Service vTable                                              Conversion Service vTable   |
\*----------------------------------------------------------------------------------------------------*/
typedef struct PARS_CONV_S_ServiceInterface
{
    /* Interface Handling */
    PARS_DWORD       dwStructSize;
    PARS_RESULT   (DLLCALL *GetMMI)                 ( /* out */ PARS_PT_MMI*            a_pMMI_IF);

    /* Interface Methods */
#if PARS_CONV_USE_CODEPAGE
    PARS_RESULT   (DLLCALL *AddCodePage)            ( /*  in */ PARS_CONV_T_CodePage*   a_pCodePagem,
                                                      /* out */ PARS_LPVOID*            a_ppCPHandle);

    PARS_RESULT   (DLLCALL *DeleteCodePage)         ( /*  in */ PARS_LPVOID             a_pCPHandle);

    PARS_RESULT   (DLLCALL *GetCPByName)            ( /*  in */ PARS_LPSTR              a_strCPName,
                                                      /* out */ PARS_LPVOID*            a_ppCPHandle);

    PARS_RESULT   (DLLCALL *ListCodePages)          ( /* out */ PARS_CONV_P_CPNameList* a_pCPNameList);

#endif /* PARS_CONV_USE_CODEPAGE */

    PARS_RESULT   (DLLCALL *GetLanguageID)          ( /*  in */ PARS_LPSTR              a_strLanguage,
                                                      /*  in */ PARS_DWORD              a_dwLanguageLen,
                                                      /* out */ PARS_LPDWORD            a_pdwLanguageId);

    PARS_RESULT   (DLLCALL *GetLanguageString)      ( /*  in */ PARS_DWORD              a_dwLanguageId,
                                                      /* out */ PARS_LPSTR*             a_pstrLanguage);

    PARS_RESULT   (DLLCALL *GetLanguageCount)       ( /* out */ PARS_LPDWORD            a_pdwLanguageCount);

    PARS_RESULT   (DLLCALL *UTF8_To_Ascii)          ( /*  in */ PARS_LPVOID             a_pCPHandle,
                                                      /*  bi */ PARS_LPSTR              a_strUtf8Text);

    PARS_RESULT   (DLLCALL *Ascii_To_UTF8)          ( /*  in */ PARS_LPVOID             a_pCPHandle,
                                                      /*  in */ PARS_LPSTR              a_strAsciiText,
                                                      /* out */ PARS_LPSTR*             a_pstrUtf8Text,
                                                      /* out */ PARS_LPDWORD            a_pdwUtf8StringLen);

    PARS_RESULT   (DLLCALL *Unicode_To_UTF8)          (/*  in */ PARS_LPWSTR            a_wstrUnicodeText,
                                                       /* out */ PARS_LPSTR*            a_pstrUtf8Text,
                                                       /* out */ PARS_LPDWORD           a_pdwUtf8StringLen);

    PARS_RESULT   (DLLCALL *Decode_URL)             ( /*  bi */ PARS_LPSTR              a_strURL);

    PARS_RESULT   (DLLCALL *Encode_URL)             ( /*  bi */ PARS_LPSTR              a_strURL,
                                                      /*  in */ PARS_DWORD              a_strMaxLen);

    PARS_RESULT   (DLLCALL *DecodeString)           ( /*  bi */ PARS_LPSTR              a_strToDecode,
                                                      /*  in */ PARS_DWORD              a_dwStrLen,
                                                      /* out */ PARS_PT_Stream          a_strmOutput);

    PARS_RESULT   (DLLCALL *MapCharInString )       ( /*  bi */ PARS_LPSTR              a_strLine,
                                                      /*  in */ PARS_CHAR               a_chToFind,
                                                      /*  in */ PARS_CHAR               a_chReplacement);

    PARS_RESULT   (DLLCALL *GetTimeFromString)      ( /*  in  */ PARS_LPSTR             a_strTime,
                                                      /*  in  */ PARS_DWORD             a_dwstrLen,
                                                      /* out  */ PARS_TIME_T*           a_timeOutput,
                                                      /* out  */ PARS_LPDWORD           a_pdwResultingType);

    PARS_RESULT   (DLLCALL *GetStringFromUtc)       ( /* opt */ PARS_TIME_T*            a_mwTime,
                                                      /* out */ PARS_CHAR*              a_achOutBuffer,
                                                      /*  in */ PARS_DWORD              a_dwOutBufferLen,
                                                      /*  in */ PARS_DWORD              a_dwSourceType);

    PARS_RESULT   (DLLCALL *GetDiffTimeMilliSecondsFromString)( /*  in */ PARS_LPSTR    a_lpStartTime,
                                                                /*  in */ PARS_DWORD   a_dwstrStartLen,
                                                                /*  in */ PARS_LPSTR    a_lpEndTime,
                                                                /*  in */ PARS_DWORD   a_dwstrEndLen,
                                                                /* out */ PARS_DWORD   *a_dw_MilliSeconds);

    PARS_DWORD    (DLLCALL *HexToBin)               ( /*  in */ PARS_LPSTR              a_strHex,
                                                      /*  in */ PARS_DWORD              a_dwHexStrLen);

    PARS_RESULT   (DLLCALL *FindCorresponding)      ( /*  in */ PARS_LPSTR              a_strInput,
                                                      /*  in */ PARS_DWORD              a_dwInputLen,
                                                      /*  in */ PARS_LPSTR              a_strWhatToFind,
                                                      /*  in */ PARS_DWORD              a_dwFindLen,
                                                      /* out */ PARS_LPSTR*             a_pstrWhereFound);

} PARS_CONV_T_ServiceInterface, *PARS_CONV_PT_ServiceInterface;

/*----------------------------------------------------------------------------------------------------*\
|   Service Management                                                            Service Management   |
\*----------------------------------------------------------------------------------------------------*/

PARS_RESULT DLLCALL PARS_CONV_InitializeService         ( /*  in */ PARS_LPSTR       a_strServiceName,
                                                      /*  in */ PARS_PT_MCB         a_pContext,
                                                      /*  in */ PARS_HDLLMODULE  a_hModuleHandle);

PARS_END_EXTERN_C

#endif /* PARS_CONVSRVC_DEFINED defined */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

