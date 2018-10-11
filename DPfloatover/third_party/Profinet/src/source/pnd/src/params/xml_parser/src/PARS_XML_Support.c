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
/*  F i l e               &F: PARS_XML_Support.c                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  This file contains helper functions for the XML parser,                  */
/*  mainly functions which are parsing text                                  */
/*                                                                           */
/*****************************************************************************/
#include "pnd_xml_cfg.h"

#define LTRC_ACT_MODUL_ID   3042
#define PND_MODULE_ID       3042

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Macros and definitions                                                                                          */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/** Specifies the number of chars per indent step */
#define PARS_XML_SHIFTWIDTH     2
/** Macro to decrease the indent */
#define PARS_XML_DECINDENT(x)   if (0 < x->dwIndent) x->dwIndent--
/** Macro to increase the indent */
#define PARS_XML_INCINDENT(x)   if (PARS_INT_MAX > x->dwIndent) x->dwIndent++
/**@}*/

/*----------------------------------------------------------------------------------------------------*\
|   Includes                                                                                Includes   |
\*----------------------------------------------------------------------------------------------------*/
#include "PARSP_Utils.h"
#include "PARS_MCB.h"                                  /* Management for Services/Applications/Filter */
#include "PARS_MemoryService.h"
#include "PARS_FileSystem.h"                                                    /* file system access */
#include "PARS_Stream.h"
#include "PARS_ConversionService.h"

#include "pnd_xml_cfg.h"
#include "pnd_trc.h"
#include "pnd_int.h"

#include "PARS_XML.h"
#include "PARS_XML_Internal.h"                                            /* Prototypes of XML Parser */
#if PARS_XML_ENABLE_XPATH
  #include "PARS_VariableService.h"
  #include "PARS_XML_XPath.h"
#endif /* PARS_XML_ENABLE_XPATH */

FileSysServiceInterface PARS_XML_g_pFile2;		//TODO
FileSysServiceInterface*        PARS_XML_g_pFile        = &PARS_XML_g_pFile2;   /* file system services */
PARS_STREAM_PT_ServiceTable     PARS_XML_g_pStream      = PARS_NULL;
PARS_CONV_PT_ServiceInterface   PARS_XML_g_pConversion  = PARS_NULL;

static PARS_RESULT DLLCALL PARS_XML_CleanUpXMLService( /*  in */ PARS_LONG         a_loRefCount,
                                                       /*  in */ PARS_BOOL         a_bForceShutdown);

typedef struct PARS_XML_S_EscapeTable
{
    PARS_CHAR   cvSource;
    PARS_LPSTR  strReplacement;
    PARS_DWORD  bfLocation; /* When this is to be replaced */
}PARS_XML_T_EscapeTable, *PARS_XML_PT_EscapeTable;

#define PARS_XML_MAX_ESCAPE_SEQUENCES   7
static PARS_XML_T_EscapeTable      PARS_XML_g_aEscapeTable[PARS_XML_MAX_ESCAPE_SEQUENCES];

/*----------------------------------------------------------------------------------------------------*\
|   Wrapper for Dll Loader                                                    Wrapper for Dll Loader   |
\*----------------------------------------------------------------------------------------------------*/

#ifndef STATIC_BINDING
PARS_RESULT DLLCALL InitializeService( /*  in */ PARS_LPSTR       a_strServiceName,
                                       /*  in */ PARS_PT_MCB      a_pContext,
                                       /*  in */ PARS_HDLLMODULE  a_hModuleHandle)
{
    return PARS_XML_InitializeXMLService ( a_strServiceName,
                                           a_pContext,
                                           a_hModuleHandle);
}

#if PARS_MEMORY_TRACE
PARS_LPVOID  DLLCALL PARS_XML_SMemMalloc_Wrapper( /*  in */ PARS_DWORD  a_dwSize,
                                                  /*  in */ PARS_INT    a_nLineNumber,
                                                  /*  in */ PARS_LPSTR  a_strFileName )
{
    PARS_VOID    *myPtr    = PARS_NULL;
    PARS_DWORD   dwTemp    = 0;
    dwTemp = a_dwSize;
    a_dwSize = PARS_MEM_ALIGN(a_dwSize, PARS_MEM_ALIGNMENT);
    myPtr=  PARS_NULL;
    if(XML_extern_SMemMalloc) myPtr=(XML_extern_SMemMalloc(a_dwSize, a_nLineNumber, a_strFileName));
    /*PARS_PORT_TRACE_5( PARS_DEBUG_LEVEL_HINT,
                       "Malloc (%6d, %6d): %p. Line:%8d, File: %s.\n",
                       a_dwSize,
                       dwTemp ,
                       myPtr,
                       a_nLineNumber,
                       a_strFileName); */
    PND_PARAMS_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT,
                       "Malloc (%6d, %6d): %p. Line:%8d, File: %d.",
                       a_dwSize,
                       dwTemp ,
                       myPtr,
                       a_nLineNumber,
                       a_strFileName);
    return myPtr;
}

PARS_VOID    DLLCALL PARS_XML_SMemFree_Wrapper( /*  in */ PARS_LPVOID   a_pMemoryBlock,
                                                /*  in */ PARS_INT      a_nLineNumber,
                                                /*  in */ PARS_LPSTR    a_strFileName )
{
    if (a_pMemoryBlock!=PARS_NULL)
    {
        // PARS_PORT_TRACE_3( PARS_DEBUG_LEVEL_HINT, "Free: %p. Line:%8d, File: %s.\n",a_pMemoryBlock, a_nLineNumber, a_strFileName);
        PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT, "Free: %p. Line:%8d, File: %d",a_pMemoryBlock, a_nLineNumber, a_strFileName);
        if(XML_extern_SMemFree) XML_extern_SMemFree(a_pMemoryBlock, a_nLineNumber, a_strFileName);
    }
    else
    {
        // PARS_PORT_TRACE_2( PARS_DEBUG_LEVEL_HINT, "PARS_SafeFree called with PARS_NULL pointer, ignored! Line:%8d, File: %s.\n", a_nLineNumber, a_strFileName);
		PND_PARAMS_TRACE_02( 0, LSA_TRACE_LEVEL_CHAT, "PARS_SafeFree called with PARS_NULL pointer, ignored! Line:%8d, File: %d", a_nLineNumber, a_strFileName);
    }
    return;
}
#endif /* PARS_MEMORY_TRACE */

/*--- PARS_XML_InitializeStandaloneXMLService() ------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        Module Management                                                                             |
|                                                                                                      |
|    Description:                                                                                      |
|        initialize xml service                                                                        |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_strServiceName       [  in ]     service name to register service                           |
|        a_pContext             [  in ]     actual context                                             |
|        a_hModuleHandle        [  in ]     module handle of dll module                                |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_RESULT                                                                                   |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT  /* DLL_EXPORT */ /* MG */  PARS_XML_InitializeStandaloneXMLService( /*  in */ XML_T_extern_SMemMalloc      a_fpMalloc,
                                                                             /*  in */ XML_T_extern_SMemFree        a_fpFree,
                                                                             /* out */ XMLServiceInterface**        a_ppXMLServiceIF)
{
    static XMLServiceInterface descrXML;
RETURN_INIT(MWE_FAC_XML_SERVICE, "PARS_XML_InitializeStandaloneXMLService");
    INITIALIZE_SHUTDOWN_MODE(XML);

#if PARS_MEMORY_TRACE
    XML_extern_SMemMalloc = a_fpMalloc;
    XML_extern_SMemFree   = a_fpFree;
    XML_intern_SMemMalloc = PARS_XML_SMemMalloc_Wrapper;
    XML_intern_SMemFree   = PARS_XML_SMemFree_Wrapper;
#else
    XML_intern_SMemMalloc = (XML_T_SMemMalloc) a_fpMalloc;
    XML_intern_SMemFree   = (XML_T_SMemFree) a_fpFree;
#endif

    PARS_XML_BuildXMLServiceDescr(&descrXML);
    *a_ppXMLServiceIF = &descrXML;
LANDING_ZONE;
    PARS_XML_CleanUpXMLService(0, PARS_TRUE);
END_LANDING_ZONE;
}
#endif

/*----------------------------------------------------------------------------------------------------*\
|   Global MCB                                                                            Global MCB   |
\*----------------------------------------------------------------------------------------------------*/
DEFINE_MODULE_MCB

/*----------------------------------------------------------------------------------------------------*/
/* --- static Globals                                                               static Globals ---*/
/*----------------------------------------------------------------------------------------------------*/
IMPLEMENT_GLOBAL_SHUTDOWN_CONTROL(XML)

XML_T_SMemMalloc        XML_intern_SMemMalloc = PARS_NULL;
XML_T_SMemFree          XML_intern_SMemFree   = PARS_NULL;
XML_T_extern_SMemMalloc XML_extern_SMemMalloc = PARS_NULL;
XML_T_extern_SMemFree   XML_extern_SMemFree   = PARS_NULL;
MemServiceInterface     PARS_XML_g_pMem2;		//TODO
MemServiceInterface*    PARS_XML_g_pMem       = &PARS_XML_g_pMem2;//PARS_NULL; /* Memory Services */

#if PARS_USE_MEM_POOL
    PARS_MEM_POOL_HANDLE    PARS_XML_g_pMemPoolAttributes       = PARS_NULL;
    PARS_MEM_POOL_HANDLE    PARS_XML_g_pMemPoolNodes            = PARS_NULL;
    PARS_MEM_POOL_HANDLE    PARS_XML_g_pMemPoolSheets           = PARS_NULL;

#endif /* PARS_USE_MEM_POOL */

/*----------------------------------------------------------------------------------------------------*\
|   Disable junk warnings                                                      Disable junk warnings   |
\*----------------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma warning (disable : 4127)    /* Conditional Expression is constant */
#endif

/*----------------------------------------------------------------------------------------------------*\
|   Implementation                                                                    Implementation   |
\*----------------------------------------------------------------------------------------------------*/

/*--- PARS_XML_CreateSheetSync() ---------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        Creates a Sync-Object to synchronize access to the given style sheet                          |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_shtXMLSheet          [  in ]     the sheet to synchronize access to                         |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_UNEXPECTED         sync access object already exists                                      |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_CreateSheetSync ( /*  in */ PARS_XML_Sheet     a_shtXMLSheet)
{
    PARS_XML_PT_internalSheet   shtWorkSheet    = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_CreateSheetSync");

    CHECK_NULL_PARAMETER(a_shtXMLSheet);

    shtWorkSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if( PARS_NULL != shtWorkSheet->pvSyncAccessSheetHandle )
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_UNEXPECTED,"SyncAccess already created");
    }
    /* create critical section for thread safe sheet access */
    PARS_CREATE_CRITICAL_SECTION(shtWorkSheet->pvSyncAccessSheetHandle);
    PARS_CHECK_CRITICAL_SECTION(shtWorkSheet->pvSyncAccessSheetHandle);
LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteSheetSync() ---------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        Creates a Sync-Object to syncronize access to the given stylesheet                            |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_shtXMLSheet          [  in ]     the sheet to syncronize access to                          |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_UNEXPECTED         sync access object already exists                                      |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_DeleteSheetSync ( /*  in */ PARS_XML_Sheet     a_shtXMLSheet)
{
    PARS_XML_PT_internalSheet    shtWorkSheet    = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteSheetSync");

    CHECK_NULL_PARAMETER(a_shtXMLSheet);

    shtWorkSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;
    PARS_DELETE_CRITICAL_SECTION(shtWorkSheet->pvSyncAccessSheetHandle);

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- StripNameSpaceCopy() ---------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        Stripes a Namespace from the given Tag Name. This Funciton will make a copy of the            |
|        String.                                                                                       |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_strName           [  in ]   The Tag from Which the Namespace should be stripped             |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_LPSTR :   TagName without Namespace                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_LPSTR PARS_XML_StripNameSpaceCopy( /*  in */ PARS_LPSTR a_strName)
{
    PARS_LPSTR   strTemp    = PARS_NULL;
    PARS_LPSTR   strReturn  = PARS_NULL;
    PARS_INT         newLen     = 0;
    if (a_strName==PARS_NULL)
    {
        return PARS_NULL;
    }
    strTemp=PARS_strchr(a_strName, ':');
    if (strTemp==PARS_NULL)
    {
        return a_strName;
    }
    newLen=PARS_strlen(strTemp);
    if (newLen<2)
    {
        XML_SMemFree(a_strName);
        return PARS_NULL;
    }
    strReturn=(PARS_LPSTR)XML_SMemMalloc(newLen);
    if (strReturn==PARS_NULL)
    {
        return PARS_NULL;
    }
    PARS_strlcpy( strReturn,
                  ++strTemp,
                  newLen,
                  newLen);
    XML_SMemFree(a_strName);
    return strReturn;
}

/*--- StripNameSpace() -------------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        Stripes a Namespace from the given Tag Name.                                                  |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_strName           [  in ]   The Tag from Which the Namespace should be stripped             |
|                                                                                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_LPSTR :   TagName without Namespace                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_StripNameSpace ( /*  in */ XML_TEXT_POS*  a_pposName,
                                      /*  in */ PARS_DWORD*    a_pdwNameLen)
{
    XML_TEXT_POS    posXMLTag   = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_StripNameSpace");

    CHECK_NULL_PARAMETER(a_pposName)

    posXMLTag=PARS_strnchr(*a_pposName, ':', *a_pdwNameLen);
    if (posXMLTag)
    {
        posXMLTag++;                               /* Overread ':' */
        *a_pdwNameLen -= posXMLTag - *a_pposName;  /* Tagnamelength = Full Length - Namespecelength (incl. :) */
        *a_pposName = posXMLTag;
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_ScanNextEncounter() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|       ScanNextEncounter() searches for the next encounter of a_strTagName beginning                  |
|       with a_posActual, returning the Text Position after the Encounter or PARS_NULL                 |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_posActual           [  in ]      start position for search                                  |
|        a_strTagName          [  in ]      tagname to search for                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        XML_TEXT_POS :   next start position of tagname                                               |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
XML_TEXT_POS PARS_XML_ScanNextEncounter( /*  in */ XML_TEXT_POS a_posActual,
                                         /*  in */ PARS_LPSTR   a_strTagName)
{
    XML_TEXT_POS    posTemp        = a_posActual;
    PARS_INT             nTagLen        = 0;

    nTagLen=PARS_strlen(a_strTagName);
    /* Slow algorithm, sorry     */
    do
    {
        if (posTemp[0]==0)
        {
            return PARS_NULL;                                    /* Parsing to EOF     */
        }
        if (posTemp[0]==a_strTagName[0])
        {
            if (PARS_strncmp(posTemp, a_strTagName, nTagLen)==0)
            {
                return (XML_TEXT_POS)(posTemp+nTagLen);
            }
        }
        posTemp++;
    } while(posTemp[0]!=0);
    return PARS_NULL;                                            /* Error     */
}

/*--- PARS_XML_SafeStrCat() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|       Attach a string ( with or without terminating '\0' ) to another string.                        |
|       Calls a specified callback if a bufferoverrun would occour,                                    |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_szDestination           [  bi ]      string to attach another string                        |
|        a_pdwDestinationLen       [  bi ]      string length                                          |
|        a_szSource                [  in ]      string to attach                                       |
|        a_dwSourceLen             [  in ]      length of the string to attach                         |
|        a_dwDestBufLen            [  in ]      length of the destination buffer                       |
|        a_pXMLAsyncStruct         [  in ]      async callback struct  ( callback and cookie )         |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_UNEXPECTED         unexpected error                                                       |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_SafeStrCat( /*  bi */ PARS_LPSTR  a_szDestination,
                                 /*  bi */ PARS_DWORD* a_pdwDestinationLen,
                                 /*  in */ PARS_LPSTR  a_szSource,
                                 /*  in */ PARS_DWORD  a_dwSourceLen,
                                 /*  in */ PARS_DWORD  a_dwDestBufLen,
                                 /*  in */ PARS_XMLAsyncStruct* a_pXMLAsyncStruct)
{
    PARS_DWORD   dwCopyLen = 0;
    PARS_DWORD   dwSourceOffset      = 0;
    PARS_BOOL    fBufferOverrun      = PARS_FALSE;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SafeStrCat");

    do
    {
        if(*a_pdwDestinationLen + a_dwSourceLen >a_dwDestBufLen-1)
        {
            dwCopyLen = a_dwDestBufLen - *a_pdwDestinationLen-1;
            fBufferOverrun = PARS_TRUE;
        }
        else
        {
            dwCopyLen = a_dwSourceLen;
            fBufferOverrun = PARS_FALSE;
        }

        PARS_memcpy((PARS_LPVOID)(a_szDestination + *a_pdwDestinationLen),(PARS_LPVOID)( a_szSource + dwSourceOffset), dwCopyLen);
        *a_pdwDestinationLen = *a_pdwDestinationLen + dwCopyLen;

        if(fBufferOverrun)
        {
            if (a_pXMLAsyncStruct && ( PARS_NULL!= (a_pXMLAsyncStruct->cbXMLAsync) ))
            {
                a_szDestination[*a_pdwDestinationLen] = '\0';

                BAIL_OUT_ON_FAIL_EX( a_pXMLAsyncStruct->cbXMLAsync( a_szDestination,
                                                                   *a_pdwDestinationLen,
                                                                    PARS_FALSE,
                                                                    a_pXMLAsyncStruct->pCookie),
                                     PARS_DEBUG_LEVEL_INFO,
                                     " Buffer Overrun not handled correctly ");

                *a_pdwDestinationLen = 0;
                dwSourceOffset = dwSourceOffset + dwCopyLen;
                a_dwSourceLen = a_dwSourceLen - dwCopyLen;
            }
            else
            {
                BAIL_OUT_0( PARS_DEBUG_LEVEL_WARNING, MWE_UNEXPECTED, "Buffer Overrun and no handler available");
            }
        }
    }while(fBufferOverrun);
    a_szDestination[*a_pdwDestinationLen] = '\0';

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_SafeFormatXMLText() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|       Insert format tags in xml file                                                                 |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_szXMLText               [  bi ]      destination string to attach format chars              |
|        a_pdwXMLTextLen           [  bi ]      destination string length                              |
|        a_dwLevel                 [  in ]      actual level of tag (used to factor for format chars)  |
|        a_dwDestBufLen            [  in ]      length of the destination buffer                       |
|        a_pXMLAsyncStruct         [  in ]      async callback struct  ( callback and cookie )         |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_UNEXPECTED         memory allocation error                                                |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_SafeFormatXMLText( /*  in */ PARS_XML_PT_internalNode  a_ndeActual,
                                        /*  bi */ PARS_LPSTR                a_szXMLText,
                                        /*  bi */ PARS_DWORD*               a_pdwXMLTextLen,
                                        /*  in */ PARS_DWORD                a_dwLevel,
                                        /*  in */ PARS_DWORD                a_dwDestBufLen,
                                        /*  in */ PARS_XMLAsyncStruct*      a_pXMLAsyncStruct,
                                        /*  in */ PARS_BOOL                 a_bClosingTag)
{
    PARS_DWORD       dwXMLTextLen    = 0;
    PARS_DWORD       dwCounter       = 0;
    PARS_DWORD       dwIndent        = 0;
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SafeFormatXMLText");

    dwXMLTextLen = *a_pdwXMLTextLen;
    if( ( dwXMLTextLen==0 ) ||
        ( a_szXMLText[dwXMLTextLen-1]=='>' ) )
    {
        if(dwXMLTextLen != 0) /* omit LF in the beginning of the file! */
        {
            if( ( (a_bClosingTag==PARS_FALSE) && (a_ndeActual->dwFlags&PARS_XML_NODE_LF) )  ||
                ( (a_bClosingTag!=PARS_FALSE) && (a_ndeActual->dwFlags&PARS_XML_NODE_LF_CLOSING_TAG) )  )
            {
                BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                          a_pdwXMLTextLen,
                                                          a_pXMLAsyncStruct->strNewLine,
                                                          PARS_strlen(a_pXMLAsyncStruct->strNewLine),
                                                          a_dwDestBufLen,
                                                          a_pXMLAsyncStruct),
                                     PARS_DEBUG_LEVEL_HINT,
                                     "PARS_XML_SafeStrCat reports an error.\n");
            }
        }
        if(a_pXMLAsyncStruct==PARS_NULL)
        {
            dwIndent = PARS_XML_USE_ORIGINAL_INDENT;
        }
        else if(a_pXMLAsyncStruct->dwIndent==PARS_XML_USE_ORIGINAL_INDENT)
        {
            if(a_bClosingTag==PARS_FALSE)
            {
                a_dwLevel = a_ndeActual->dwOriginIndent;
            }
            else
            {
                a_dwLevel = a_ndeActual->dwOriginIndentClosingTag;
            }
            /* Original Count of Spaces, like in the input file */
            dwIndent = 1;
        } else if(a_pXMLAsyncStruct->dwIndent>20)
        {
            // PARS_TRACE_0(PARS_DEBUG_LEVEL_ERROR, "Indent has to be smaller than 20, set to 20!\n");
			PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "Indent has to be smaller than 20, set to 20!");
            dwIndent = 20;
        }
        else
        {
            dwIndent = a_pXMLAsyncStruct->dwIndent;
        }

        for (dwCounter = 0; dwCounter <a_dwLevel; dwCounter++)
        {
            BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                      a_pdwXMLTextLen,
                                                      "                   ", /* Max 20 Spaces! */
                                                      dwIndent,
                                                      a_dwDestBufLen,
                                                      a_pXMLAsyncStruct),
                                 PARS_DEBUG_LEVEL_HINT,
                                 "PARS_XML_SafeStrCat reports an error.\n");
        }
    }
LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_MakeCopyString() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|       allocate memory and copy the given string                                                      |
|       ( source string could be terminated with '\0' or not )                                         |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_szSource                 [  in ]      source string                                         |
|        a_dwSourceLen              [  in ]      source string length                                  |
|        a_ppszDestination          [ out ]      new string to return                                  |
|        a_pdwDestinationLen        [ out ]      length of new string                                  |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_UNEXPECTED         memory allocation                                                      |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_MakeCopyString ( /*  in */ PARS_LPSTR  a_szSource,
                                      /*  in */ PARS_DWORD  a_dwSourceLen,
                                      /* out */ PARS_LPSTR* a_ppszDestination,
                                      /* out */ PARS_DWORD* a_pdwDestinationLen)
{
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_MakeCopy");

    /* sourceLen 0 is a valid length */

    /* CHECK_NULL_PARAMETER(a_szSource); no source string is valid -> do not bail out */
    if (PARS_NULL == a_szSource)
    {
        *a_ppszDestination = PARS_NULL;
        *a_pdwDestinationLen = 0;
    }
    else
    {
        *a_pdwDestinationLen = a_dwSourceLen;
        *a_ppszDestination = (PARS_LPSTR) XML_SMemMalloc(*a_pdwDestinationLen+1);
        CHECK_MEMORY(*a_ppszDestination);
        PARS_memcpy(*a_ppszDestination, a_szSource, *a_pdwDestinationLen);
        (*a_ppszDestination)[*a_pdwDestinationLen] = '\0';
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_SAFE_strncmp() ------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        compare two strings                                                                           |
|        ( string length could be special a define )                                                   |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_strString1            [  in ]        source string                                          |
|        a_strString2            [  in ]        source string length                                   |
|        a_dwLength1             [ out ]        length of string1                                      |
|        a_dwLength2             [ out ]        length of string2                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        see PARS_strncmp documentation                                                                |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_INT PARS_XML_SAFE_strncmp( /*  in */ PARS_LPSTR a_strString1,
                                /*  in */ PARS_LPSTR a_strString2,
                                /*  in */ PARS_DWORD a_dwLength1,
                                /*  in */ PARS_DWORD a_dwLength2)
{
    PARS_INT iReturn    = 1;

    if(PARS_ZERO_TERMINATED_STRING == a_dwLength2)
    {
        a_dwLength2 = PARS_strlen(a_strString2);
    }
    if(PARS_ZERO_TERMINATED_STRING == a_dwLength1)
    {
        a_dwLength1 = PARS_strlen(a_strString1);
    }
    if(a_dwLength1>a_dwLength2)
    {
        iReturn = PARS_strncmp(a_strString1, a_strString2, a_dwLength2);
        if(!iReturn)
        {
            iReturn = 1;
        }
    }
    else if(a_dwLength1<a_dwLength2)
    {
        iReturn = PARS_strncmp(a_strString1, a_strString2, a_dwLength1);
        if(!iReturn)
        {
            iReturn = -1;
        }
    }
    else
    {
         iReturn = PARS_strncmp(a_strString1, a_strString2, a_dwLength1);
    }
    return (iReturn);

}

/*--- PARS_XML_SAFE_strnicmp() -----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        compare two strings                                                                           |
|        ( string length could be special a define )                                                   |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_strString1            [  in ]        source string                                          |
|        a_strString2            [  in ]        source string length                                   |
|        a_dwLength1             [ out ]        length of string1                                      |
|        a_dwLength2             [ out ]        length of string2                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        see PARS_strncmp documentation                                                                |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_INT PARS_XML_SAFE_strnicmp( /*  in */ PARS_LPSTR a_strString1,
                                 /*  in */ PARS_LPSTR a_strString2,
                                 /*  in */ PARS_DWORD a_dwLength1,
                                 /*  in */ PARS_DWORD a_dwLength2)
{
    PARS_INT iReturn    = 1;

    if(PARS_ZERO_TERMINATED_STRING == a_dwLength2)
    {
        a_dwLength2 = PARS_strlen(a_strString2);
    }
    if(PARS_ZERO_TERMINATED_STRING == a_dwLength1)
    {
        a_dwLength1 = PARS_strlen(a_strString1);
    }
    if(a_dwLength1>a_dwLength2)
    {
        iReturn = PARS_strnicmp(a_strString1, a_strString2, a_dwLength2);
        if(!iReturn)
        {
            iReturn = 1;
        }
    }
    else if(a_dwLength1<a_dwLength2)
    {
        iReturn = PARS_strnicmp(a_strString1, a_strString2, a_dwLength1);
        if(!iReturn)
        {
            iReturn = -1;
        }
    }
    else
    {
         iReturn = PARS_strnicmp(a_strString1, a_strString2, a_dwLength1);
    }
    return (iReturn);

}

/*--- PARS_XML_GetNextTextTag() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        XMLGetNextTextTag() returns the Text Position of the Tag following this Tag                   |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_ndeNode              [  in ]        the node to query the start                             |
|                                                                                                      |
|    Return Value:                                                                                     |
|        XML_TEXT_POS    !PARS_NULL     the Position in the XML document after this tag                |
|                        PARS_NULL      The a_ndeNode is PARS_NULL                                     |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
XML_TEXT_POS PARS_XML_GetNextTextTag ( /*  in */ PARS_XML_Node    a_ndeNode)
{
    PARS_XML_PT_internalNode tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    if (tmpNode==PARS_NULL)
    {
        return PARS_NULL;
    }
    return tmpNode->posXMLNextTag;
}

/*--- PARS_XML_GetNextTextTag() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        XMLGetNextTextTag() returns the Text Position of the Tag following this Tag                   |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_posActual         [  in ]    Actual position in the XML-Text                                 |
|       a_cTerminator       [  in ]    Character which terminates the Value                            |
|       a_strValue          [ out ]    The returning Value                                             |
|       a_nMaxLen           [  in ]    The Maximum Size from a_strValue                                |
|                                                                                                      |
|    Return Value:                                                                                     |
|            XML_TEXT_POS                The new Position after the Value                              |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_LPSTR PARS_XML_GetNextValue ( /*  in */ XML_TEXT_POS   a_posActual,
                                   /*  in */ PARS_CHAR      a_cTerminator,
                                   /* out */ PARS_LPSTR     a_strValue,
                                   /*  in */ PARS_INT       a_nMaxLen,
                                   /*  in */ PARS_BOOL      a_bIgnoreWhiteSpace)
{
    XML_TEXT_POS    posEnd  = a_posActual;
    PARS_INT        nStrLen = 0;

    if (a_strValue==PARS_NULL)
    {
        return a_posActual;
    }
    while (posEnd[0]!=a_cTerminator)
    {
        if (posEnd[0]==0) return PARS_NULL;
        if (a_bIgnoreWhiteSpace==PARS_FALSE)
        {
            /* Looking for WhiteSpace     */
            if ( (posEnd[0]==' ') ||
                 (posEnd[0]=='\t') ||
                 (posEnd[0]=='\r') ||
                 (posEnd[0]=='\n') )
            {
                break;
            }
        }
        posEnd++;
    };
    nStrLen=posEnd-a_posActual;
    if (nStrLen>a_nMaxLen)
    {
        nStrLen=a_nMaxLen;
    }
    PARS_memcpy(a_strValue, a_posActual, nStrLen);
    a_strValue[nStrLen]=0;
    return posEnd;
}

/*--- PARS_XML_FindNextValue() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        internal help function                                                                        |
|                                                                                                      |
|    Description:                                                                                      |
|        Find next value in xml-text from actual position                                              |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_posActual         [  in ]    actual position in the xml-text                                 |
|       a_cTerminator       [  in ]    character which terminates the value                            |
|       a_pposValue         [ out ]    position of next value                                          |
|       a_pdwValueLen       [  in ]    lenght of next value                                            |
|       a_bIgnoreWhiteSpace [  in ]    ignore whitespace in serch                                      |
|       a_bStopAtTagEnd     [  in ]    stops the search at TagEnd                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|            XML_TEXT_POS                The new Position after the Value                              |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_FindNextValue ( /*  in */ XML_TEXT_POS*   a_pposActual,
                                     /*  in */ PARS_CHAR       a_cTerminator,
                                     /* out */ XML_TEXT_POS*   a_pposValue,
                                     /* out */ PARS_DWORD*     a_pdwValueLen,
                                     /*  in */ PARS_BOOL       a_bIgnoreWhiteSpace,
                                     /*  in */ PARS_BOOL       a_bStopAtTagEnd)
{
    XML_TEXT_POS    posEnd  = *a_pposActual;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_FindNextValue");

    CHECK_NULL_PARAMETER(a_pposValue);

    while( posEnd[0] != a_cTerminator )
    {
        if (posEnd[0]==0)
        {
            *a_pposActual = PARS_NULL;
            RETURN(MWE_NO_ERROR);
        }
        if ((a_bStopAtTagEnd!=PARS_FALSE)
            &&(posEnd[0] == END_OF_TAG))
        {
            /* stop searching at tagend */
            *a_pposActual = PARS_NULL;
            RETURN(MWE_NO_ERROR);
        }
        if (a_bIgnoreWhiteSpace==PARS_FALSE)
        {
            /* Looking for WhiteSpace     */
            if ( (posEnd[0]==' ') ||
                 (posEnd[0]=='\t') ||
                 (posEnd[0]=='\r') ||
                 (posEnd[0]=='\n') )
            {
                break;
            }
        }
        posEnd++;
    };
    (*a_pdwValueLen) = posEnd - (*a_pposActual);
    (*a_pposValue)   = *a_pposActual;

    (*a_pposActual)  = posEnd;

LANDING_ZONE;
    (*a_pposActual) = PARS_NULL;
    if(a_pposValue != PARS_NULL)
    {
        (*a_pposValue) = PARS_NULL;
    }
    (*a_pdwValueLen) = 0;

END_LANDING_ZONE;
}

/*--- PARS_XML_PackXMLSheetHeader() ------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Packs the XML-Sheet Header the specified buffer                                                |
|       ( header means from BOM or <? to the root node )                                               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtSheet              [  in ]         sheet where the DTD should be set                      |
|       a_szXMLText             [  bi ]         in: Empty buffer for XMLText                           |
|                                               out: XML-Sheet header as Text                          |
|       a_pdwXMLTextLen         [  bi ]         length of XMLText                                      |
|       a_dwBufSize             [  in ]         size of buffer allocated for XMLText                   |
|       a_pXMLAsyncStruct       [  in ]         pointer to Async Callback-Structure                    |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_PackXMLSheetHeader ( /*  in */ PARS_XML_Sheet          a_shtXMLSheet,
                                          /*  bi */ PARS_LPSTR              a_szXMLText,
                                          /*  bi */ PARS_DWORD*             a_pdwXMLTextLen,
                                          /*  in */ PARS_DWORD              a_dwBufSize,
                                          /*  in */ PARS_XMLAsyncStruct*    a_pXMLAsyncStruct)
{
    PARS_XML_PT_internalSheet    shtXMLSheet  = PARS_NULL;
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_PackXMLSheetHeader");

    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_szXMLText);
    CHECK_ZERO_PARAMETER(a_dwBufSize);

    shtXMLSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    /* build header */

    if(shtXMLSheet->strBom[0] != '\0')
    {
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  shtXMLSheet->strBom,
                                                  PARS_strlen(shtXMLSheet->strBom),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
    }

    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              XML_TAG,
                                              PARS_strlen(XML_TAG),
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");
    if(PARS_strlen(shtXMLSheet->strVersion))
    {
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  XML_ATTR_VERSION,
                                                  PARS_strlen(XML_ATTR_VERSION),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  "=\"",
                                                  PARS_strlen("=\""),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  shtXMLSheet->strVersion,
                                                  PARS_strlen(shtXMLSheet->strVersion),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  "\"",
                                                  PARS_strlen("\""),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
    }
    if(PARS_strlen(shtXMLSheet->strEncoding))
    {
        if( (a_pXMLAsyncStruct == PARS_NULL) ||
            (a_pXMLAsyncStruct->dwC14NForm == XML_C14N_NONE) )
        {
            BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                      a_pdwXMLTextLen,
                                                      " ",
                                                      PARS_strlen(" "),
                                                      a_dwBufSize,
                                                      a_pXMLAsyncStruct),
                                 PARS_DEBUG_LEVEL_HINT,
                                 "PARS_XML_SafeStrCat reports an error.\n");
        }
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  XML_ATTR_ENCODING,
                                                  PARS_strlen(XML_ATTR_ENCODING),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  "=\"",
                                                  PARS_strlen("=\""),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  shtXMLSheet->strEncoding,
                                                  PARS_strlen(shtXMLSheet->strEncoding),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  "\"",
                                                  PARS_strlen("\""),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
    }
    if(shtXMLSheet->XML_SheetOptions.bfStandAlone)
    {
        if( (a_pXMLAsyncStruct==PARS_NULL) ||
            (a_pXMLAsyncStruct->dwC14NForm == XML_C14N_NONE) )
        {
            BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                      a_pdwXMLTextLen,
                                                      " ",
                                                      PARS_strlen(" "),
                                                      a_dwBufSize,
                                                      a_pXMLAsyncStruct),
                                 PARS_DEBUG_LEVEL_HINT,
                                 "PARS_XML_SafeStrCat reports an error.\n");
        }
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  XML_ATTR_STANDALONE,
                                                  PARS_strlen(XML_ATTR_STANDALONE),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  "=\"yes\"",
                                                  PARS_strlen("=\"yes\""),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
    }
    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              "?",
                                              PARS_strlen("?"),
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");
    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              ">",
                                              PARS_strlen(">"),
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");
    shtXMLSheet->p_ndeRoot->dwFlags |= PARS_XML_NODE_LF;
LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_PackXMLSheetPI() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Packs the XML-Sheet Processing Instructions to the specified buffer                            |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtSheet              [  in ]         sheet where the DTD should be set                      |
|       a_szXMLText             [  bi ]         in: Empty buffer for XMLText                           |
|                                               out: XML-Sheet header as Text                          |
|       a_pdwXMLTextLen         [  bi ]         length of XMLText                                      |
|       a_dwBufSize             [  in ]         size of buffer allocated for XMLText                   |
|       a_pXMLAsyncStruct       [  in ]         pointer to Async Callback-Structure                    |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_PackXMLSheetPI ( /*  in */ PARS_XML_Sheet          a_shtXMLSheet,
                                      /*  bi */ PARS_LPSTR              a_szXMLText,
                                      /*  bi */ PARS_DWORD*             a_pdwXMLTextLen,
                                      /*  in */ PARS_DWORD              a_dwBufSize,
                                      /*  in */ PARS_XMLAsyncStruct*    a_pXMLAsyncStruct)
{
    PARS_XML_PT_internalSheet    shtXMLSheet  = PARS_NULL;
    PARS_XML_PT_internalPI       piActual     = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_PackXMLSheetPI");

    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_szXMLText);
    CHECK_ZERO_PARAMETER(a_dwBufSize);

    shtXMLSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    /* build header */

    piActual = shtXMLSheet->p_piRoot;
    while(piActual)
    {
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  "<?",
                                                  PARS_strlen("<?"),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
        if(piActual->szPrefPI)
        {
            BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                      a_pdwXMLTextLen,
                                                      piActual->szPrefPI,
                                                      piActual->dwPrefPILen,
                                                      a_dwBufSize,
                                                      a_pXMLAsyncStruct),
                                 PARS_DEBUG_LEVEL_HINT,
                                 "PARS_XML_SafeStrCat reports an error.\n");
        }
        else
        {
            /* To check: Processing instruction may have no whitespace surroundings for C14N */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                      a_pdwXMLTextLen,
                                                      piActual->posXMLPI,
                                                      piActual->dwPILen,
                                                      a_dwBufSize,
                                                      a_pXMLAsyncStruct),
                                 PARS_DEBUG_LEVEL_HINT,
                                 "PARS_XML_SafeStrCat reports an error.\n");
        }
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  "?>",
                                                  PARS_strlen("?>"),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
#if 0
        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                  a_pdwXMLTextLen,
                                                  a_pXMLAsyncStruct->strNewLine,
                                                  PARS_strlen(a_pXMLAsyncStruct->strNewLine),
                                                  a_dwBufSize,
                                                  a_pXMLAsyncStruct),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_SafeStrCat reports an error.\n");
#endif
        piActual = piActual->pNextPI;
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_PackXMLAttribute() --------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Packs the specified attribute to the given buffer                                              |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_attrActual            [  in ]         attribute to pack to buffer                            |
|       a_szXMLText             [  bi ]         in: buffer to add packed attribute                     |
|                                               out: buffer with packed attribute                      |
|       a_pdwXMLTextLen         [  bi ]         length of XMLText                                      |
|       a_dwBufSize             [  in ]         size of buffer allocated for XMLText                   |
|       a_pXMLAsyncStruct       [  in ]         pointer to Async Callback-Structure                    |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT PARS_XML_PackXMLAttribute ( /*  in */ PARS_XML_PT_internalAttributes a_attrActual,
                                               /*  bi */ PARS_LPSTR                     a_szXMLText,
                                               /*  bi */ PARS_DWORD*                    a_pdwXMLTextLen,
                                               /*  in */ PARS_DWORD                     a_dwBufSize,
                                               /*  in */ PARS_XMLAsyncStruct*           a_pXMLAsyncStruct )
{
    PARS_LPSTR       szValue     = PARS_NULL;
    PARS_DWORD       dwValueLen  = 0;
    PARS_LPSTR       szName      = PARS_NULL;
    PARS_DWORD       dwNameLen   = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_PackXMLAttribute");

    CHECK_NULL_PARAMETER(a_attrActual);
    CHECK_NULL_PARAMETER(a_szXMLText);

    if(a_attrActual->szPrefValue)
    {
        szValue = a_attrActual->szPrefValue;
        dwValueLen = a_attrActual->dwPrefValueLen;
    }
    else
    {
        szValue = a_attrActual->posXMLValue;
        dwValueLen = a_attrActual->dwValueLen;
    }
    if(a_attrActual->szPrefName)
    {
        szName = a_attrActual->szPrefName;
        dwNameLen = a_attrActual->dwPrefNameLen;
    }
    else
    {
        szName = a_attrActual->posXMLName;
        dwNameLen = a_attrActual->dwNameLen;
    }
    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              " ",
                                              PARS_strlen(" "),
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");

    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              szName,
                                              dwNameLen,
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");
    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              "=\"",
                                              PARS_strlen("=\""),
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");
    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              szValue,
                                              dwValueLen,
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");
    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                              a_pdwXMLTextLen,
                                              "\"",
                                              PARS_strlen("\""),
                                              a_dwBufSize,
                                              a_pXMLAsyncStruct),
                         PARS_DEBUG_LEVEL_HINT,
                         "PARS_XML_SafeStrCat reports an error.\n");

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_EscapeContent() -----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function (C14N Extension)                                                         |
|                                                                                                      |
|    Description:                                                                                      |
|       Filters the content of a Tag / attribute CData etc. for C14N                                   |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_szContent             [  in ]         Content string to filter                               |
|       a_dwContentLen          [  in ]         Size of the content string                             |
|       a_dwFlags               [  in ]         Kind of content.                                       |
|       a_pszOutput             [ out ]         The filtered content, must be freed!                   |
|       a_pdwOutputLen          [ out ]         Length of the filtered content.                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_EscapeContent( /*  in */ PARS_LPSTR    a_szContent,
                                            /*  in */ PARS_DWORD    a_dwContentLen,
                                            /*  in */ PARS_DWORD    a_dwFlags,
                                            /* out */ PARS_LPSTR*   a_pszOutput,
                                            /* out */ PARS_LPDWORD  a_pdwOutputLen)
{
    PARS_DWORD      dwIndex     = 0;
    PARS_DWORD      dwCharIndex = 0;
    PARS_BOOL       bFound      = PARS_FALSE;
    PARS_T_Stream   strmNew;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_EscapeContent");
    CHECK_NULL_PARAMETER(a_szContent);
    CHECK_NULL_PARAMETER(a_pszOutput);
    CHECK_NULL_PARAMETER(a_pdwOutputLen);
    if( a_dwContentLen == 0 )
    {
        (*a_pszOutput) = (PARS_LPSTR) PARS_XML_g_pMem->SMemMalloc(1);
        CHECK_MEMORY((*a_pszOutput))
        *(*a_pszOutput) = '\0';
        *(a_pdwOutputLen) = 0;
        RETURN(MWE_NO_ERROR);
    }
    PARS_XML_g_pStream->Init(&strmNew);
    PARS_XML_g_pStream->ExpandStream(&strmNew, a_dwContentLen);
    /* Filtering the content */
    for(dwIndex=0;dwIndex<a_dwContentLen;dwIndex++)
    {
        bFound = PARS_FALSE;
        for(dwCharIndex=0;dwCharIndex<PARS_XML_MAX_ESCAPE_SEQUENCES;dwCharIndex++)
        {
            if(a_dwFlags & PARS_XML_g_aEscapeTable[dwCharIndex].bfLocation)
            {
                if( (a_szContent[dwIndex] == '\r') &&
                    (a_szContent[dwIndex+1] == '\n') )
                {
                    dwIndex++;
                }
                if(a_szContent[dwIndex]==PARS_XML_g_aEscapeTable[dwCharIndex].cvSource)
                {
                    PARS_XML_g_pStream->Write(&strmNew, PARS_XML_g_aEscapeTable[dwCharIndex].strReplacement, PARS_ZERO_TERMINATED_STRING);
                    bFound = PARS_TRUE;
                    break;
                }
            }
        }
        if(bFound == PARS_FALSE )
        {
            PARS_XML_g_pStream->Write(&strmNew, a_szContent+dwIndex, 1);
        }
    }
    PARS_XML_g_pStream->Detach(&strmNew, a_pszOutput, a_pdwOutputLen, PARS_NULL);
LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_CompareAttributes() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function (C14N Extension)                                                         |
|                                                                                                      |
|    Description:                                                                                      |
|       Compares 2 attribute for C14N, namespace not yet supported.                                    |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         The node the attribute belongs to (for later use)      |
|       attrLeft                [  in ]         The left attribute                                     |
|       attrRight               [  in ]         The right attribute                                    |
|                                                                                                      |
|    Return Value:                                                                                     |
|        0                      Attributes are equal or error.                                         |
|        <0                     Left is smaller                                                        |
|        >0                     Right is smaller                                                       |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_INT PARS_XML_CompareAttributes( /*  in */ PARS_XML_PT_internalNode          a_ndeNode,
                                            /*  in */ PARS_XML_PT_internalAttributes    attrLeft,
                                            /*  in */ PARS_XML_PT_internalAttributes    attrRight )
{
    PARS_LPSTR    strLeftName       = PARS_NULL;
    PARS_DWORD    dwLeftNameLen     = 0;
    PARS_LPSTR    strRightName      = PARS_NULL;
    PARS_DWORD    dwRightNameLen    = 0;
    PARS_INT      nResult           = 0;
    PARS_DWORD    dwTestLen         = 0;
    PARS_LPSTR    strLeftUri        = PARS_NULL;
    PARS_LPSTR    strRightUri       = PARS_NULL;
    PARS_LPSTR    strTemp           = PARS_NULL;
    PARS_DWORD    dwLeftUriLen      = 0;
    PARS_DWORD    dwRightUriLen     = 0;
    PARS_BOOL     bLeftXMLNS        = PARS_FALSE;
    PARS_BOOL     bRightXMLNS       = PARS_FALSE;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_CompareAttributes");
    PARS_UNREFERENCED_PARAMETER(a_ndeNode); /* This is for supporting name spaces */

    /* Step 1: Get the Attribute strings */
    if(attrRight->bAlreadySorted != PARS_FALSE)
    {
        RETURN2(0);
    }
    if(attrLeft->szPrefName)
    {
        strLeftName = attrLeft->szPrefName;
        dwLeftNameLen = attrLeft->dwPrefNameLen;
    }
    else
    {
        strLeftName = attrLeft->posXMLName;
        dwLeftNameLen = attrLeft->dwNameLen;
    }
    if(attrRight->szPrefName)
    {
        strRightName = attrRight->szPrefName;
        dwRightNameLen = attrRight->dwPrefNameLen;
    }
    else
    {
        strRightName = attrRight->posXMLName;
        dwRightNameLen = attrRight->dwNameLen;
    }

    /* Step 3a: Check for xmlns: */
    if(PARS_strnicmp(strLeftName, "xmlns", 5)==0)
    {
        bLeftXMLNS=PARS_TRUE;
    }
    if(PARS_strnicmp(strRightName, "xmlns", 5)==0)
    {
        bRightXMLNS=PARS_TRUE;
    }
    if(bLeftXMLNS && !bRightXMLNS)
    {
        RETURN2(-1); /* Left is smaller, cause xmlns has prio */
    }
    if(!bLeftXMLNS && bRightXMLNS)
    {
        RETURN2(1); /* Left is smaller, cause xmlns has prio */
    }
    if(bLeftXMLNS && bRightXMLNS)
    {
        if(dwLeftNameLen<dwRightNameLen)
        {
            dwTestLen = dwLeftNameLen;
        }
        else
        {
            dwTestLen = dwRightNameLen;

        }

        nResult = PARS_strncmp(strLeftName, strRightName, dwTestLen);
        if(nResult == 0)
        {
            /* Part strings are equal, check is one of the strings is longer */
            if(dwLeftNameLen == dwRightNameLen)
            {
                RETURN2(0); /* Both are equal */
            }
            if(dwLeftNameLen < dwRightNameLen)
            {
                RETURN2(-1); /* Left is smaller */
            }
            RETURN2(1); /* right is smaller */
        }
        RETURN2(nResult)

    }

    /* Step 3b: Get the Namespace URIs */
    strTemp = PARS_strnchr( strLeftName, ':', dwLeftNameLen);
    if(strTemp != PARS_NULL)
    {
        PARS_XML_GetNamespaceUri( (PARS_XML_Node) a_ndeNode,
                                  PARS_XML_MEM_FLAG_NO_FLAG,
                                  strLeftName,
                                  strTemp-strLeftName,
                                 &strLeftUri,
                                 &dwLeftUriLen);

    }
    strTemp = PARS_NULL;
    strTemp = PARS_strnchr( strRightName, ':', dwRightNameLen);
    if(strTemp != PARS_NULL)
    {
        PARS_XML_GetNamespaceUri( (PARS_XML_Node) a_ndeNode,
                                  PARS_XML_MEM_FLAG_NO_FLAG,
                                  strRightName,
                                  strTemp-strRightName,
                                 &strRightUri,
                                 &dwRightUriLen);

    }
    /* Step 3: Compare the URIs */
    if( (strLeftUri != PARS_NULL) &&
        (strRightUri == PARS_NULL) )
    {
        RETURN2(-1); /* Left is smaller, cause namespace go first. */
    }
    if( (strLeftUri == PARS_NULL) &&
        (strRightUri != PARS_NULL) )
    {
        RETURN2(1); /* Right is smaller, cause namespace go first. */
    }

    if( (strLeftUri != PARS_NULL) &&
        (strRightUri != PARS_NULL) )
    {
        /* Both attributes have URIs so we have to compare the URIs */
        if(dwLeftUriLen<dwRightUriLen)
        {
            dwTestLen = dwLeftUriLen;
        }
        else
        {
            dwTestLen = dwRightUriLen;

        }
        nResult = PARS_strncmp(strLeftUri, strRightUri, dwTestLen);
        if(nResult == 0)
        {
            /* Part strings are equal, check is one of the strings is longer */
            if(dwLeftUriLen > dwRightUriLen)
            {
                RETURN2(1); /* Right is smaller */
            }
            if(dwLeftUriLen < dwRightUriLen)
            {
                RETURN2(-1); /* Left is smaller */
            }
            /* Both are equal test the names */
        }
        else
        {
            /* Both URIs are different so the URIs give the sort criteria */
            RETURN2(nResult);
        }
    }
    /* Step 3: Compare the attribute strings */
    if(dwLeftNameLen<dwRightNameLen)
    {
        dwTestLen = dwLeftNameLen;
    }
    else
    {
        dwTestLen = dwRightNameLen;

    }
    nResult = PARS_strncmp(strLeftName, strRightName, dwTestLen);
    if(nResult == 0)
    {
        /* Part strings are equal, check is one of the strings is longer */
        if(dwLeftNameLen == dwRightNameLen)
        {
            RETURN2(0); /* Both are equal */
        }
        if(dwLeftNameLen < dwRightNameLen)
        {
            RETURN2(-1); /* Left is smaller */
        }
        RETURN2(1); /* right is smaller */
    }
    RETURN2(nResult)
    LANDING_ZONE2(0);
    END_LANDING_ZONE2(0);
}

/*--- PARS_XML_SortAttributeList() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function (C14N Extension)                                                         |
|                                                                                                      |
|    Description:                                                                                      |
|       Sorts the attribute list as claimed by C14N. Name spaces are not supported yet.                |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         The node the attribute belongs to (for later use)      |
|       a_attrRoot              [  in ]         Pointer to the attribute list.                         |
|       a_paattrSorted          [ out ]         Array of attribute pointer, now sorted.                |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT PARS_XML_SortAttributeList( /*  in */ PARS_XML_PT_internalNode           a_ndeNode,
                                               /*  in */ PARS_DWORD                         a_dwBehavior,
                                               /*  in */ PARS_XML_PT_internalAttributes**   a_paattrSorted)
{
    PARS_DWORD                      dwAttributeCount        = 0;
    PARS_XML_PT_internalAttributes* aattrResult             = PARS_NULL;
    PARS_XML_PT_internalAttributes  attrCursor              = PARS_NULL;
    PARS_XML_PT_internalAttributes  attrCurrentLowest       = PARS_NULL;
    PARS_DWORD                      dwIndex                 = 0;
    PARS_LPSTR                      strXmlnsUri             = PARS_NULL;
    PARS_DWORD                      dwdXmlnsUri             = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SortAttributeList");
    /* check for inheriting xmlns */
    if(a_dwBehavior&PARS_XML_INHERIT_XMLNS)
    {
        PARS_XML_QueryAttributeByName( ((PARS_XML_Node)a_ndeNode),
                                                        PARS_XML_MEM_FLAG_NO_FLAG,
                                                        "xmlns",
                                                        PARS_ZERO_TERMINATED_STRING,
                                                        PARS_TRUE,
                                                       &strXmlnsUri,
                                                       &dwdXmlnsUri);
        if(strXmlnsUri==PARS_NULL)
        {
            /* No namespace set, inherit from parent */
            PARS_XML_GetNamespaceUri( (PARS_XML_Node)a_ndeNode->p_ndeParent,
                                      PARS_XML_MEM_FLAG_NO_FLAG,
                                      "",
                                      0,
                                      &strXmlnsUri,
                                      &dwdXmlnsUri);
            if(( strXmlnsUri!=PARS_NULL) && (dwdXmlnsUri!=0))
            {
                PARS_XML_AddAttribute ( (PARS_XML_Node)a_ndeNode,
                                        PARS_XML_MEM_FLAG_MAKE_COPY,
                                        "xmlns",
                                        PARS_ZERO_TERMINATED_STRING,
                                        strXmlnsUri,
                                        dwdXmlnsUri);
            }
        }
    }
    /* Calculate the amount of attributes */
    attrCursor = a_ndeNode->p_atrRoot;
    while(attrCursor!=PARS_NULL)
    {
        attrCursor->bAlreadySorted = PARS_FALSE;
        dwAttributeCount++;
        attrCursor = attrCursor->pNextAttribute;
    }
    aattrResult = (PARS_XML_PT_internalAttributes *) PARS_XML_g_pMem->SMemMalloc((dwAttributeCount+1)*sizeof(PARS_XML_PT_internalAttributes));
    CHECK_MEMORY(aattrResult);
    PARS_memset(aattrResult, 0, (dwAttributeCount+1)*sizeof(PARS_XML_PT_internalAttributes));
    /* iteratively look for the smallest attribute */
    for(dwIndex=0;dwIndex<dwAttributeCount;dwIndex++)
    {
        attrCursor = a_ndeNode->p_atrRoot->pNextAttribute;
        attrCurrentLowest = a_ndeNode->p_atrRoot;
        while(attrCurrentLowest!=PARS_NULL)
        {
            if(attrCurrentLowest->bAlreadySorted==PARS_FALSE)
            {
                break;
            }
            attrCurrentLowest=attrCurrentLowest->pNextAttribute;
        };
        BAIL_OUT_ON_NULL_EX(attrCurrentLowest, PARS_DEBUG_LEVEL_ERROR, MWE_SYNTAX_ERROR, "All Attributes already processed, this should never happen!\n");
        while(attrCursor!=PARS_NULL)
        {
            if( PARS_XML_CompareAttributes( a_ndeNode, attrCurrentLowest, attrCursor)>0) /* attrCurrentLowest is bigger than attrCursor */
            {
                attrCurrentLowest = attrCursor;
            }
            attrCursor = attrCursor->pNextAttribute;
        }
        attrCurrentLowest->bAlreadySorted = PARS_TRUE;
        aattrResult[dwIndex]=attrCurrentLowest;
    }
    (*a_paattrSorted) = aattrResult;
    LANDING_ZONE;
    END_LANDING_ZONE;
}

static PARS_BOOL   PARS_XML_CheckNonEmptyContent( /*  in */ PARS_LPSTR   a_strContent,
                                                  /*  in */ PARS_DWORD   a_dwContentLength)
{
    PARS_DWORD  dwIndex = 0;
    if(a_strContent==PARS_NULL)
    {
        return PARS_FALSE;
    }
    if(a_dwContentLength==0)
    {
        return PARS_FALSE;
    }
    for(dwIndex=0;dwIndex<a_dwContentLength;dwIndex++)
    {
        if( PARS_strchr(" \t\r\n", a_strContent[dwIndex])==PARS_NULL)
        {
            return PARS_TRUE;
        }
    }
    return PARS_FALSE;
}

/*--- PARS_XML_PackXMLNodes() ------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Packs the specified node and all its children and neighbors to the given buffer                |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         attribute to pack to buffer                            |
|       a_dwActualDepth         [  in ]         depth of the actual node                               |
|       a_szXMLText             [  bi ]         in: buffer to add packed attribute                     |
|                                               out: buffer with packed attribute                      |
|       a_pdwXMLTextLen         [  bi ]         length of XMLText                                      |
|       a_dwBufSize             [  in ]         size of buffer allocated for XMLText                   |
|       a_pXMLAsyncStruct       [  in ]         pointer to Async Callback-Structure                    |
|       a_bWithNeighbors        [  in ]         only the children, no neighbors                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_PackXMLNodes  ( /*  in */ PARS_XML_PT_internalNode a_ndeNode,
                                     /*  in */ PARS_DWORD               a_dwActualDepth,
                                     /*  bi */ PARS_LPSTR               a_szXMLText,
                                     /*  bi */ PARS_DWORD*              a_pdwXMLTextLen,
                                     /*  in */ PARS_DWORD               a_dwBufSize,
                                     /*  in */ PARS_XMLAsyncStruct*     a_pXMLAsyncStruct,
                                     /*  in */ PARS_INT                 a_bWithNeighbors)
{
    PARS_XML_PT_internalAttributes  attrActual      = PARS_NULL;
    PARS_XML_PT_internalNode        ndeFirstChild   = PARS_NULL;
    PARS_XML_PT_internalNode        ndeNextNode     = PARS_NULL;
    PARS_BOOL                       fEmptyTag       = PARS_TRUE;
    PARS_LPSTR                      szTagName       = PARS_NULL;
    PARS_DWORD                      dwTagNameLen    = 0;
    PARS_LPSTR                      szContent       = PARS_NULL;
    PARS_DWORD                      dwContentLen    = 0;
    PARS_DWORD                      dwXMLTextLen    = 0;
    PARS_LPSTR                      szOutput        = PARS_NULL;
    PARS_DWORD                      dwOutputLen     = 0;
    PARS_XML_PT_internalAttributes* aattrSorted     = PARS_NULL;
    PARS_DWORD                      dwIndex         = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_PackXMLNodes");
    CHECK_NULL_PARAMETER(a_ndeNode);

    do
    {
        fEmptyTag       = PARS_TRUE;
        if(a_ndeNode->szPrefTagName)
        {
            szTagName       = a_ndeNode->szPrefTagName;
            dwTagNameLen    = a_ndeNode->dwPrefTagNameLen;
        }
        else
        {
            szTagName       = a_ndeNode->posXMLTagName;
            dwTagNameLen    = a_ndeNode->dwTagNameLen;
        }

        if(a_ndeNode->dwNodeType == PARS_XML_NODE_TYPE_TEXT)
        {
            fEmptyTag       = PARS_FALSE;
            /* no attributes, no children, only content */
            PARS_XML_GetContent( (PARS_XML_Node) a_ndeNode, PARS_XML_MEM_FLAG_NO_FLAG , &szContent, &dwContentLen);
            if( PARS_XML_CheckNonEmptyContent(szContent, dwContentLen) )
            {
                dwXMLTextLen = (*a_pdwXMLTextLen);

                /* eliminate leading \r\n of already set data */
                if ((dwXMLTextLen >2) &&(PARS_strncmp((a_szXMLText+dwXMLTextLen-2), "\r\n", 2) == 0))
                {
                    (*a_pdwXMLTextLen)-= 2 ;
                }
                else if( ( ( a_pXMLAsyncStruct->dwC14NForm != XML_C14N_NONE ) ) &&
                             (dwXMLTextLen >1)&&
                             (PARS_strncmp((a_szXMLText+dwXMLTextLen-1), "\n", 2) == 0))
                {
                    (*a_pdwXMLTextLen)-- ;
                }
                if( a_pXMLAsyncStruct->dwC14NForm != XML_C14N_NONE )
                {
                    PARS_XML_EscapeContent( szContent, dwContentLen, PARS_XML_FLTR_TEXT_NODE, &szOutput, &dwOutputLen);
                }
                BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                          a_pdwXMLTextLen,
                                                          szOutput,
                                                          dwOutputLen,
                                                          a_dwBufSize,
                                                          a_pXMLAsyncStruct),
                                     PARS_DEBUG_LEVEL_HINT,
                                     "PARS_XML_SafeStrCat reports an error.\n");
                PARS_XML_g_pMem->SMemFree(szOutput);
                szOutput = PARS_NULL;
            }
        }
        else
        {
            if(a_ndeNode->dwNodeType == PARS_XML_NODE_TYPE_CDATA)
            {
               fEmptyTag       = PARS_FALSE;
               if( a_pXMLAsyncStruct->dwC14NForm != XML_C14N_NONE )
               {
                   dwXMLTextLen = (*a_pdwXMLTextLen);                   /* eliminate leading \n of already set data */
                   if( (dwXMLTextLen >1)&&
                       (PARS_strncmp((a_szXMLText+dwXMLTextLen-1), "\n", 2) == 0))
                   {
                       (*a_pdwXMLTextLen)-- ;
                   }
                   /* no attributes, no children, only content */
                   PARS_XML_GetContent( (PARS_XML_Node) a_ndeNode, PARS_XML_MEM_FLAG_NO_FLAG , &szContent, &dwContentLen);
                   if( PARS_XML_CheckNonEmptyContent(szContent, dwContentLen) )
                   {
                       PARS_XML_EscapeContent( szContent, dwContentLen, PARS_XML_FLTR_TEXT_NODE, &szOutput, &dwOutputLen);
                       BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                 a_pdwXMLTextLen,
                                                                 szOutput,
                                                                 dwOutputLen,
                                                                 a_dwBufSize,
                                                                 a_pXMLAsyncStruct),
                                                                 PARS_DEBUG_LEVEL_HINT,
                                                                 "PARS_XML_SafeStrCat reports an error.\n");
                       PARS_XML_g_pMem->SMemFree(szOutput);
                   }
               }
               else
               {
                   BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeFormatXMLText( a_ndeNode,
                                                                    a_szXMLText,
                                                                    a_pdwXMLTextLen,
                                                                    a_dwActualDepth,
                                                                    a_dwBufSize,
                                                                    a_pXMLAsyncStruct,
                                                                    PARS_FALSE),
                                                                    PARS_DEBUG_LEVEL_HINT,
                                                                    "PARS_XML_SafeFormatXMLText reports an error.\n");
                   BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                             a_pdwXMLTextLen,
                                                             "<![CDATA[",
                                                             PARS_strlen("<![CDATA["),
                                                             a_dwBufSize,
                                                             a_pXMLAsyncStruct),
                                                             PARS_DEBUG_LEVEL_HINT,
                                                             "PARS_XML_SafeStrCat reports an error.\n");

                   /* no attributes, no children, only content */
                   PARS_XML_GetContent( (PARS_XML_Node) a_ndeNode, PARS_XML_MEM_FLAG_NO_FLAG , &szContent, &dwContentLen);
                   if( PARS_XML_CheckNonEmptyContent(szContent, dwContentLen) )
                   {
                       BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                 a_pdwXMLTextLen,
                                                                 szContent,
                                                                 dwContentLen,
                                                                 a_dwBufSize,
                                                                 a_pXMLAsyncStruct),
                                                                 PARS_DEBUG_LEVEL_HINT,
                                                                 "PARS_XML_SafeStrCat reports an error.\n");
                   }
                   BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                             a_pdwXMLTextLen,
                                                             "]]>",
                                                             PARS_strlen("]]>"),
                                                             a_dwBufSize,
                                                             a_pXMLAsyncStruct),
                                                             PARS_DEBUG_LEVEL_HINT,
                                                             "PARS_XML_SafeStrCat reports an error.\n");
               }

            }
            else if(a_ndeNode->dwNodeType == PARS_XML_NODE_TYPE_COMMENT)
            {
                if( a_pXMLAsyncStruct->dwC14NForm != XML_C14N_NO_COMMENTS )
                {
                    /* Double check if format comments is good for C14N */
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeFormatXMLText( a_ndeNode,
                                                                     a_szXMLText,
                                                                     a_pdwXMLTextLen,
                                                                     a_dwActualDepth,
                                                                     a_dwBufSize,
                                                                     a_pXMLAsyncStruct,
                                                                     PARS_FALSE),
                                                                     PARS_DEBUG_LEVEL_HINT,
                                                                     "PARS_XML_SafeFormatXMLText reports an error.\n");
                    fEmptyTag       = PARS_FALSE;
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                              a_pdwXMLTextLen,
                                                              "<!--",
                                                              PARS_strlen("<!--"),
                                                              a_dwBufSize,
                                                              a_pXMLAsyncStruct),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");

                    /* no attributes, no children, only content */
                    PARS_XML_GetContent( (PARS_XML_Node) a_ndeNode, PARS_XML_MEM_FLAG_NO_FLAG , &szContent, &dwContentLen);
                    if( PARS_XML_CheckNonEmptyContent(szContent, dwContentLen) )
                    {
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                  a_pdwXMLTextLen,
                                                                  szContent,
                                                                  dwContentLen,
                                                                  a_dwBufSize,
                                                                  a_pXMLAsyncStruct),
                                             PARS_DEBUG_LEVEL_HINT,
                                             "PARS_XML_SafeStrCat reports an error.\n");
                    }
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                              a_pdwXMLTextLen,
                                                              "-->",
                                                              PARS_strlen("-->"),
                                                              a_dwBufSize,
                                                              a_pXMLAsyncStruct),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");
                }
            }
            else
            {
                /* other nodes must have a tag name */
                BAIL_OUT_ON_NULL_EX( szTagName, PARS_DEBUG_LEVEL_INFO, MWE_SYNTAX_ERROR,"Invalid MXL Node found ( no tagname specified )");

                BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeFormatXMLText( a_ndeNode,
                                                                 a_szXMLText,
                                                                 a_pdwXMLTextLen,
                                                                 a_dwActualDepth,
                                                                 a_dwBufSize,
                                                                 a_pXMLAsyncStruct,
                                                                 PARS_FALSE),
                                                                 PARS_DEBUG_LEVEL_HINT,
                                                                 "PARS_XML_SafeFormatXMLText reports an error.\n");

                BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                          a_pdwXMLTextLen,
                                                          "<",
                                                          PARS_strlen("<"),
                                                          a_dwBufSize,
                                                          a_pXMLAsyncStruct),
                                      PARS_DEBUG_LEVEL_HINT,
                                      "PARS_XML_SafeStrCat reports an error.\n");
                BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                          a_pdwXMLTextLen,
                                                          szTagName,
                                                          dwTagNameLen,
                                                          a_dwBufSize,
                                                          a_pXMLAsyncStruct),
                                     PARS_DEBUG_LEVEL_HINT,
                                     "PARS_XML_SafeStrCat reports an error.\n");
                if( a_pXMLAsyncStruct->dwC14NForm != XML_C14N_NONE)
                {
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SortAttributeList( a_ndeNode, a_pXMLAsyncStruct->dwBehavior, &aattrSorted),
                                                             PARS_DEBUG_LEVEL_WARNING,
                                                             "Sort AttributeList reports an error.\n");
                    if( aattrSorted != PARS_NULL )
                    {
                        dwIndex=0;
                        while (aattrSorted[dwIndex]!=PARS_NULL)
                        {
                            BAIL_OUT_ON_FAIL_EX( PARS_XML_PackXMLAttribute( aattrSorted[dwIndex],
                                                                            a_szXMLText,
                                                                            a_pdwXMLTextLen,
                                                                            a_dwBufSize,
                                                                            a_pXMLAsyncStruct),
                                                 PARS_DEBUG_LEVEL_HINT,
                                                 "PARS_XML_SafeStrCat reports an error.\n");
                            dwIndex++;
                        }
                        PARS_XML_g_pMem->SMemFree(aattrSorted);
                        aattrSorted  = PARS_NULL;
                    }
                }
                else
                {
                    attrActual = (PARS_XML_PT_internalAttributes) a_ndeNode->p_atrRoot;

                    while (attrActual!=PARS_NULL)
                    {
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_PackXMLAttribute( attrActual,
                                                                        a_szXMLText,
                                                                        a_pdwXMLTextLen,
                                                                        a_dwBufSize,
                                                                        a_pXMLAsyncStruct),
                                                                        PARS_DEBUG_LEVEL_HINT,
                                                                        "PARS_XML_SafeStrCat reports an error.\n");
                        attrActual = attrActual->pNextAttribute;
                    }
                }
                if(PARS_XML_NODE_EXPANDED !=(a_ndeNode->dwFlags&PARS_XML_NODE_EXPANDED))
                {
                    PARS_XML_GetContent( (PARS_XML_Node) a_ndeNode, PARS_XML_MEM_FLAG_NO_FLAG , &szContent, &dwContentLen);
                    if( PARS_XML_CheckNonEmptyContent(szContent, dwContentLen) )
                    {   /* content available */
                        fEmptyTag = PARS_FALSE;
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                  a_pdwXMLTextLen,
                                                                  ">",
                                                                  PARS_strlen(">"),
                                                                  a_dwBufSize,
                                                                  a_pXMLAsyncStruct),
                                             PARS_DEBUG_LEVEL_HINT,
                                             "PARS_XML_SafeStrCat reports an error.\n");
                        /* do not format if content is next */
                        /*
                           PARS_strcat(a_szXMLText, "\r\n");
                           PARS_XML_FormatXMLText(a_szXMLText,a_dwActualDepth+1);
                        */
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                  a_pdwXMLTextLen,
                                                                  szContent,
                                                                  dwContentLen,
                                                                  a_dwBufSize,
                                                                  a_pXMLAsyncStruct),
                                             PARS_DEBUG_LEVEL_HINT,
                                             "PARS_XML_SafeStrCat reports an error.\n");

                    }
                }
                BAIL_OUT_ON_FAIL_EX( PARS_XML_GetNode( (PARS_XML_Node) a_ndeNode,
                                                       PARS_XML_NAVIGATE_DOWN,
                                                       PARS_XML_NODE_TYPE_ALLNODES,
                                                       (PARS_XML_Node*)&ndeFirstChild),
                                     PARS_DEBUG_LEVEL_HINT,
                                     "PARS_XML_GetNode reports an error.\n");

                if(ndeFirstChild)
                {
                    fEmptyTag       = PARS_FALSE;
                    /* vf3732 12.10.09: We don't have to check for empty (text) content, cause we have children, */
                    /* always close the tag! */
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                              a_pdwXMLTextLen,
                                                              ">",
                                                              PARS_strlen(">"),
                                                              a_dwBufSize,
                                                              a_pXMLAsyncStruct),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");
                    a_pXMLAsyncStruct->dwBehavior&=~PARS_XML_INHERIT_XMLNS; /* Inherit Namespace only for the first (outermost) node */
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_PackXMLNodes( ndeFirstChild,
                                                                a_dwActualDepth +1 ,
                                                                a_szXMLText,
                                                                a_pdwXMLTextLen,
                                                                a_dwBufSize,
                                                                a_pXMLAsyncStruct,
                                                                PARS_XML_USE_NEIGHBOURS),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");
                }
                if(fEmptyTag)
                {
                    if( a_pXMLAsyncStruct->dwC14NForm == XML_C14N_NONE )
                    {
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                  a_pdwXMLTextLen,
                                                                  "/>",
                                                                  PARS_strlen("/>"),
                                                                  a_dwBufSize,
                                                                  a_pXMLAsyncStruct),
                                                                  PARS_DEBUG_LEVEL_HINT,
                                                                  "PARS_XML_SafeStrCat reports an error.\n");
                    }
                    else
                    {
                        /* no LF cause the closing tag will follow immediately */
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                  a_pdwXMLTextLen,
                                                                  "></",
                                                                  PARS_strlen("></"),
                                                                  a_dwBufSize,
                                                                  a_pXMLAsyncStruct),
                                             PARS_DEBUG_LEVEL_HINT,
                                             "PARS_XML_SafeStrCat reports an error.\n");
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                  a_pdwXMLTextLen,
                                                                  szTagName,
                                                                  dwTagNameLen,
                                                                  a_dwBufSize,
                                                                  a_pXMLAsyncStruct),
                                             PARS_DEBUG_LEVEL_HINT,
                                             "PARS_XML_SafeStrCat reports an error.\n");
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                                  a_pdwXMLTextLen,
                                                                  ">",
                                                                  PARS_strlen(">"),
                                                                  a_dwBufSize,
                                                                  a_pXMLAsyncStruct),
                                             PARS_DEBUG_LEVEL_HINT,
                                             "PARS_XML_SafeStrCat reports an error.\n");
                    }
                }
                if(!fEmptyTag)
                {
                    if( a_szXMLText )
                    {
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeFormatXMLText( a_ndeNode,
                                                                         a_szXMLText,
                                                                         a_pdwXMLTextLen,
                                                                         a_dwActualDepth,
                                                                         a_dwBufSize,
                                                                         a_pXMLAsyncStruct,
                                                                         PARS_TRUE),
                                             PARS_DEBUG_LEVEL_HINT,
                                             "PARS_XML_SafeStrCat reports an error.\n");
                    }
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                              a_pdwXMLTextLen,
                                                              "</",
                                                              PARS_strlen("</"),
                                                              a_dwBufSize,
                                                              a_pXMLAsyncStruct),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                              a_pdwXMLTextLen,
                                                              szTagName,
                                                              dwTagNameLen,
                                                              a_dwBufSize,
                                                              a_pXMLAsyncStruct),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                              a_pdwXMLTextLen,
                                                              ">",
                                                              PARS_strlen(">"),
                                                              a_dwBufSize,
                                                              a_pXMLAsyncStruct),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");
#if 0
                    BAIL_OUT_ON_FAIL_EX( PARS_XML_SafeStrCat( a_szXMLText,
                                                              a_pdwXMLTextLen,
                                                              a_pXMLAsyncStruct->strNewLine,
                                                              PARS_strlen(a_pXMLAsyncStruct->strNewLine),
                                                              a_dwBufSize,
                                                              a_pXMLAsyncStruct),
                                         PARS_DEBUG_LEVEL_HINT,
                                         "PARS_XML_SafeStrCat reports an error.\n");
#endif
                }
            }
        }
        PARS_XML_GetNode( (PARS_XML_Node) a_ndeNode, PARS_XML_NAVIGATE_NEXT, PARS_XML_NODE_TYPE_ALLNODES, (PARS_XML_Node*)&ndeNextNode);
        a_ndeNode = ndeNextNode;
        if (a_bWithNeighbors==PARS_FALSE)
        {
            break;
        }
    } while (ndeNextNode!= PARS_NULL);
LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_ParseAttribute() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Parses a node to find its attributes with the given name from the given position               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_posActual             [  in ]         position to start search                               |
|       a_strAttrName           [  in ]         attribute name to search                               |
|       a_strValue              [ out ]         value of the found attribute                           |
|       a_nMaxLen               [  in ]         max len of the found attribute value                   |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_LPSTR             position after the found attribute                                     |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_LPSTR PARS_XML_ParseAttribute ( /*  in */ XML_TEXT_POS     a_posActual,
                                     /*  in */ PARS_LPSTR       a_strAttrName,
                                     /* out */ PARS_LPSTR       a_strValue,
                                     /*  in */ PARS_INT         a_nMaxLen)
{
    XML_TEXT_POS    posEnd            = a_posActual;
    XML_TEXT_POS    posTemp           = a_posActual;

    PARS_INT                nAttrLen        = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_ParseAttribute");
    CHECK_NULL_PARAMETER(a_strValue);

    nAttrLen=PARS_strlen(a_strAttrName);
    while (posEnd[0]!='>')
    {
        if (posEnd[0]==0) BAIL_OUT_SILENT( MWE_SYNTAX_ERROR );
        if (PARS_strncmp(posEnd, a_strAttrName, nAttrLen)==0)
        {
            posEnd = PARS_XML_ScanChar(posEnd, '=', '>', PARS_FALSE);
            if( posEnd== PARS_NULL )
            {
                PARS_TRACE( PARS_DEBUG_LEVEL_ERROR, "Attribute without '='" );
            }
            posTemp = PARS_XML_ScanChar(posEnd, '"', '>', PARS_FALSE);
            if (posTemp==PARS_NULL)
            {
                posEnd = PARS_XML_ScanChar(posEnd, '\'', '>', PARS_FALSE);
            }
            else
            {
                posEnd=posTemp;
            }
            if( posEnd == PARS_NULL )
            {
                PARS_TRACE( PARS_DEBUG_LEVEL_ERROR, "Attribute without '\"'" );
            }
            if (posEnd==PARS_NULL)
            {
                BAIL_OUT_SILENT( MWE_SYNTAX_ERROR );
            }
            posEnd = PARS_XML_GetNextValue(posEnd, '"', a_strValue, a_nMaxLen, PARS_TRUE);
            break;
        }
        posEnd++;
    };
LANDING_ZONE2(posEnd);
END_LANDING_ZONE2(PARS_NULL);
}

/*--- PARS_XML_ScanChar() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|    ScanChar() searches for the next occurence of a_cToSearchFor, and returns                         |
|    the Position of the next char after a_cToSearchFor.                                               |
|    if a_cTerminator or '\0' occures first, PARS_NULL is returned to signal                           |
|    that the Char is not found                                                                        |
|                                                                                                      |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_posActual             [  in ]         position to start search                               |
|       a_cToSearchFor          [  in ]         attribute name to search                               |
|       a_cTerminator           [  in ]         The Char to terminate the search,                      |
|                                               when no a_cToSearchFor Char is found.                  |
|                                                                                                      |
|    Return Value:                                                                                     |
|        XML_TEXT_POS        !=PARS_NULL    The new Position after the a_cToSearchFor                  |
|                            PARS_NULL      no encounter                                               |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
XML_TEXT_POS PARS_XML_ScanChar ( /*  in */ XML_TEXT_POS         a_posActual,
                                 /*  in */ PARS_CHAR            a_cToSearchFor,
                                 /*  in */ PARS_CHAR            a_cTerminator,
                                 /*  in */ PARS_BOOL            a_bStrict)
{
    if (PARS_NULL == a_posActual)
    {
        return (PARS_NULL);
    }
    while(a_posActual[0]!=0)
    {
        if (a_posActual[0]==a_cToSearchFor)
        {
            return ++a_posActual;
        }
        if (a_posActual[0]==a_cTerminator)
        {
            return PARS_NULL;
        }
        if (PARS_FALSE != a_bStrict)
        {
            if ( (a_posActual[0]!=' ')  &&
                 (a_posActual[0]!='\t') &&
                 (a_posActual[0]!='\r') &&
                 (a_posActual[0]!='\n') )
            {
                return PARS_NULL;
            }
        }
        a_posActual++;
    }
    return PARS_NULL;
}

/*--- PARS_XML_ScanNonWhiteSpace() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|     ScanNonWhiteSpace() searches for the next occurrence of a non WhiteSpace, and returns            |
|     the Position of the next char after the last WhiteSpace.                                         |
|     if a_cTerminator or '\0' occurs first, PARS_NULL is returned to signal                           |
|     that the Char is not found                                                                       |
|                                                                                                      |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_posActual             [  in ]         position to start search                               |
|       a_cTerminator           [  in ]         The Char to terminate the search,                      |
|                                               when no whitespace Char is found.                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        XML_TEXT_POS        !=PARS_NULL    The new Position after the whitespace                      |
|                            PARS_NULL      no encounter                                               |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
XML_TEXT_POS PARS_XML_ScanNonWhiteSpace ( /*  in */ XML_TEXT_POS   a_posActual,
                                       /*  in */ PARS_CHAR           a_cTerminator)
{

    if ((PARS_NULL == a_posActual) || (0 ==a_posActual[0]))
    {
        return PARS_NULL;
    }

    do
    {
        if ( (a_posActual[0]!=' ')  &&
             (a_posActual[0]!='\t') &&
             (a_posActual[0]!='\r') &&
             (a_posActual[0]!='\n') )
        {
            return a_posActual;
        }
        if (a_posActual[0]==a_cTerminator)
        {
            return PARS_NULL;
        }
        a_posActual++;
    } while(a_posActual[0]!=0);
    return PARS_NULL;
}

/*--- PARS_XML_NewAttribute() ------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|     Creates a new attribute and initializes it                                                       |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_pndeValue             [ out ]         the new created attribute                              |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_NewAttribute ( /* out */ PARS_XML_PT_internalAttributes*   a_pattrValue )
{

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_NewAttribute");

    CHECK_NULL_PARAMETER(a_pattrValue);

#if PARS_USE_MEM_POOL
    if( PARS_XML_g_pMem != PARS_NULL )
    {
        *a_pattrValue = (PARS_XML_PT_internalAttributes) PARS_XML_g_pMem->MemPoolFixMalloc(PARS_XML_g_pMemPoolAttributes);
    }
    else
    {
        *a_pattrValue = (PARS_XML_PT_internalAttributes) XML_SMemMalloc(sizeof(PARS_XML_T_internalAttributes));
    }
#else /* PARS_USE_MEM_POOL */
    *a_pattrValue = (PARS_XML_PT_internalAttributes) XML_SMemMalloc(sizeof(PARS_XML_T_internalAttributes));
#endif /* PARS_USE_MEM_POOL */
    CHECK_MEMORY(*a_pattrValue);

    (*a_pattrValue)->posXMLName      = PARS_NULL;
    (*a_pattrValue)->dwNameLen       = 0;
    (*a_pattrValue)->posXMLValue     = PARS_NULL;
    (*a_pattrValue)->dwValueLen      = 0;
    (*a_pattrValue)->szPrefName      = PARS_NULL;
    (*a_pattrValue)->dwPrefNameLen   = 0;
    (*a_pattrValue)->szPrefValue     = PARS_NULL;
    (*a_pattrValue)->dwPrefValueLen  = 0;
    (*a_pattrValue)->pNextAttribute  = PARS_NULL;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_NewNode() -----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|     Creates a new node and initializes it                                                            |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_pndeValue             [ out ]         the new created node                                   |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_NewNode ( /* out */ PARS_XML_Node*        a_pndeValue )
{
    PARS_XML_PT_internalNode ndeNewNode  = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_NewNode");

#if PARS_USE_MEM_POOL
    if( PARS_XML_g_pMem != PARS_NULL)
    {
        ndeNewNode = (PARS_XML_PT_internalNode) PARS_XML_g_pMem->MemPoolFixMalloc(PARS_XML_g_pMemPoolNodes);
    }
    else
    {
        ndeNewNode = (PARS_XML_PT_internalNode) XML_SMemMalloc(sizeof(PARS_XML_T_internalNode));
    }
#else /* PARS_USE_MEM_POOL */
    ndeNewNode = (PARS_XML_PT_internalNode) XML_SMemMalloc(sizeof(PARS_XML_T_internalNode));
#endif /* PARS_USE_MEM_POOL */
    CHECK_MEMORY(ndeNewNode);

    ndeNewNode->dwNodeType              = 0;
    ndeNewNode->dwFlags                 = 0;
    ndeNewNode->posXMLTagStart          = PARS_NULL;
    ndeNewNode->dwChildCount            = 0;

    ndeNewNode->p_atrRoot               = PARS_NULL;
    ndeNewNode->p_ndeFirstChild         = PARS_NULL;
    ndeNewNode->p_ndeLastChild          = PARS_NULL;

    ndeNewNode->p_ndeNextNeighbour      = PARS_NULL;
    ndeNewNode->p_ndeParent             = PARS_NULL;
    ndeNewNode->p_ndePrevNeighbour      = PARS_NULL;
    ndeNewNode->posXMLContentStart      = PARS_NULL;
    ndeNewNode->posXMLNextTag           = PARS_NULL;

    ndeNewNode->posXMLTagName           = PARS_NULL;
    ndeNewNode->szPrefTagName           = PARS_NULL;
    ndeNewNode->dwPrefTagNameLen        = 0;
    ndeNewNode->szPrefContent           = PARS_NULL;
    ndeNewNode->dwPrefContentLen        = 0;

    ndeNewNode->dwContentLen            = 0;
    ndeNewNode->dwTagNameLen            = 0;
    ndeNewNode->p_shtXMLSheet           = 0;
    ndeNewNode->dwOriginIndent          = 0;
    ndeNewNode->dwOriginIndentClosingTag= 0;

    *a_pndeValue = (PARS_XML_Node) ndeNewNode;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_NewSheet() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|     Creates a new sheet and initializes it                                                           |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_pshtXMLSheet          [ out ]         the new created sheet                                  |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation error                                                |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_NewSheet ( /* out */ PARS_XML_Sheet*       a_pshtXMLSheet )
{
    PARS_XML_PT_internalSheet    shtWork         = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_NewSheet");

#if PARS_USE_MEM_POOL
    if(PARS_XML_g_pMem!=PARS_NULL)
    {
        shtWork = (PARS_XML_PT_internalSheet) PARS_XML_g_pMem->MemPoolFixMalloc(PARS_XML_g_pMemPoolSheets);
    }
    else
    {
        shtWork = (PARS_XML_PT_internalSheet) XML_SMemMalloc(sizeof(PARS_XML_T_internalSheet));
    }
#else /* PARS_USE_MEM_POOL */
    shtWork = (PARS_XML_PT_internalSheet) XML_SMemMalloc(sizeof(PARS_XML_T_internalSheet));
#endif /* PARS_USE_MEM_POOL */
    CHECK_MEMORY(shtWork);

    /* Fill out shtWork    */
    PARS_memset(&shtWork->XML_SheetOptions, 0, sizeof(PARS_DWORD));
    shtWork->posXMLDoc                      = PARS_NULL;
    shtWork->p_piRoot                       = PARS_NULL;
    shtWork->p_ndeRoot                      = PARS_NULL;
    shtWork->posActualCursor                = PARS_NULL;
    shtWork->posXMLDTD_Data                 = PARS_NULL;
    shtWork->dwDTD_DataLen                  = 0;
    shtWork->szPrefDTD_Data                 = PARS_NULL;
    shtWork->dwPrefDTD_DataLen              = 0;
    shtWork->posActualCursor                = PARS_NULL;
    shtWork->posXMLDocEnd                   = PARS_NULL;
    shtWork->pvSyncAccessSheetHandle        = PARS_NULL;

    PARS_strlcpy( shtWork->strVersion,
                  "1.0",
                  XML_VERSIONLEN,
                  XML_VERSIONLEN);
    /* no default value for encoding */
    /* encoding is not a must */
    /* PARS_strcpy(shtWork->strEncoding,"US-ASCII"); */
    shtWork->strEncoding[0] = '\0';
    /* no byte order mask  is set */
    shtWork->strBom[0] = '\0';
    /* set standalone to no */
    PARS_strlcpy( shtWork->strStandAlone,
                  "no",
                  XML_STANDALONELEN,
                  XML_STANDALONELEN);
    *a_pshtXMLSheet = (PARS_XML_Sheet) shtWork;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteAttr() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Deletes the specified attribute                                                                |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_attrAttributes       [  in ]       attribute to delete                                       |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation                                                      |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT  PARS_XML_DeleteAttr( /*  in */ PARS_XML_PT_internalAttributes   a_attrAttributes)
{
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteAttr");

    CHECK_NULL_PARAMETER(a_attrAttributes);

    if (a_attrAttributes->szPrefName!=PARS_NULL)
    {
        XML_SMemFree(a_attrAttributes->szPrefName);
        a_attrAttributes->szPrefName=PARS_NULL;
    }
    if (a_attrAttributes->szPrefValue!=PARS_NULL)
    {
        XML_SMemFree(a_attrAttributes->szPrefValue);
        a_attrAttributes->szPrefValue=PARS_NULL;
    }
#if PARS_USE_MEM_POOL
    if( PARS_XML_g_pMem != PARS_NULL)
    {
        PARS_XML_g_pMem->MemPoolFixFree(PARS_XML_g_pMemPoolAttributes, a_attrAttributes);
    }
    else
    {
        XML_SMemFree(a_attrAttributes);
    }
#else /* PARS_USE_MEM_POOL */
    XML_SMemFree(a_attrAttributes);
#endif /* PARS_USE_MEM_POOL */

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteAllAttr() -----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal function of XML Service                                                               |
|                                                                                                      |
|    Description:                                                                                      |
|       Deletes the specified attribute and all linked attributes (the whole list of attributes below) |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_attrAttributes        [  in ]         attribute to delete                                    |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation                                                      |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_DeleteAllAttr ( /*  in */ PARS_XML_PT_internalAttributes a_attrAttributes)
{
    PARS_XML_PT_internalAttributes   tmpAttr = (PARS_XML_PT_internalAttributes) a_attrAttributes;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteAllAttr");

    CHECK_NULL_PARAMETER(a_attrAttributes);
    if (tmpAttr->pNextAttribute!=PARS_NULL)
    {
        PARS_XML_DeleteAllAttr( tmpAttr->pNextAttribute );
        tmpAttr->pNextAttribute = PARS_NULL;
    }
    BAIL_OUT_ON_FAIL_EX( PARS_XML_DeleteAttr ( a_attrAttributes),
                         PARS_DEBUG_LEVEL_INFO,
                         "DeleteAttr failed.\n");

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteAllNodes() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Deletes the specified node, all its children and all its neighbours                            |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]       node to delete                                           |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation                                                      |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_DeleteAllNodes ( /*  in */ PARS_XML_PT_internalNode    a_ndeNode)
{
    PARS_XML_PT_internalNode    tmpNode     = PARS_NULL;
    PARS_XML_PT_internalNode    tmpNextNode = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteAllNodes");

    CHECK_NULL_PARAMETER(a_ndeNode);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    while (tmpNode)
    {
        if (tmpNode->p_ndeFirstChild!=PARS_NULL)               /* Delete All children */
        {
            PARS_XML_DeleteAllNodes(tmpNode->p_ndeFirstChild);
            tmpNode->p_ndeFirstChild=PARS_NULL;
        }
        if (tmpNode->p_atrRoot!=PARS_NULL)                 /* Delete Attributes of this node */
        {
            PARS_XML_DeleteAllAttr(tmpNode->p_atrRoot);
            tmpNode->p_atrRoot=PARS_NULL;
        }
        if (tmpNode->szPrefTagName)
        {
            XML_SMemFree(tmpNode->szPrefTagName);
            tmpNode->szPrefTagName = PARS_NULL;
        }
        if (tmpNode->szPrefContent)
        {
            XML_SMemFree(tmpNode->szPrefContent);
            tmpNode->szPrefContent = PARS_NULL;
        }
        tmpNextNode = tmpNode->p_ndeNextNeighbour;
#if PARS_USE_MEM_POOL
    if( PARS_XML_g_pMem != PARS_NULL)
    {
        PARS_XML_g_pMem->MemPoolFixFree(PARS_XML_g_pMemPoolNodes, tmpNode);
    }
    else
    {
        XML_SMemFree(tmpNode);
    }
#else /* PARS_USE_MEM_POOL */
        XML_SMemFree(tmpNode);
#endif /* PARS_USE_MEM_POOL */
        tmpNode = tmpNextNode;
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_LinkNode() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Link a node to the given other nodes and                                                       |
|       Connect the parent-, child- and neighbour-nodes.                                               |
|                                                                                                      |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  bi ]       node to link into the tree                               |
|       a_ndeParent             [  bi ]       parent node of the node to link                          |
|       a_ndePrevNeighbour      [  bi ]       previous neighbour of the node to link                   |
|       a_ndeNextNeighbour      [  bi ]       next neighbour of the node to link                       |
|       a_ndeFirstChild         [  bi ]       first child of the node to link                          |
|       a_ndeLastChild          [  bi ]       last child of the node to link                           |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_LinkNode( /* bi */    PARS_XML_PT_internalNode  a_ndeNode,
                               /* bi */    PARS_XML_PT_internalNode  a_ndeParent,
                               /* bi */    PARS_XML_PT_internalNode  a_ndePrevNeighbour,
                               /* bi */    PARS_XML_PT_internalNode  a_ndeNextNeighbour,
                               /* bi */    PARS_XML_PT_internalNode  a_ndeFirstChild,
                               /* bi */    PARS_XML_PT_internalNode  a_ndeLastChild)
{
    PARS_XML_PT_internalNode ndeChildNode    = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_LinkNode");

    CHECK_NULL_PARAMETER(a_ndeNode);

    a_ndeNode->p_ndeParent            = a_ndeParent;
    a_ndeNode->p_ndePrevNeighbour     = a_ndePrevNeighbour;
    a_ndeNode->p_ndeNextNeighbour     = a_ndeNextNeighbour;
    a_ndeNode->p_ndeFirstChild        = a_ndeFirstChild;
    a_ndeNode->p_ndeLastChild         = a_ndeLastChild;

    if(a_ndeParent)
    {
        if(!a_ndePrevNeighbour)
        {
            /* link the first child */
            a_ndeParent->p_ndeFirstChild = a_ndeNode;
        }
        if(!a_ndeNextNeighbour)
        {
            /* link the last child */
            a_ndeParent->p_ndeLastChild = a_ndeNode;
        }
        a_ndeParent->dwChildCount++;
    }

    if(a_ndePrevNeighbour)
    {
        a_ndePrevNeighbour->p_ndeNextNeighbour = a_ndeNode;
    }
    if(a_ndeNextNeighbour)
    {
        a_ndeNextNeighbour->p_ndePrevNeighbour = a_ndeNode;
    }

    if (a_ndeFirstChild)
    {
        ndeChildNode = a_ndeFirstChild;
        while(ndeChildNode)
        {
            ndeChildNode->p_ndeParent = a_ndeNode;
            ndeChildNode = ndeChildNode->p_ndeNextNeighbour;
        }
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_LinkNode() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Insert a new node into a xml Sheet at a given position                                         |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeInsertNode         [  bi ]       node to insert into the tree                             |
|       a_ndeLinkBaseNode       [  bi ]       base node to link the node to                            |
|       a_dwInsertionFlags      [  in ]       indicates if to insert as neighbor or child or ...       |
|                                             from linkbasenode                                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT         PARS_XML_InsertNode ( /*  in */ PARS_XML_Node         a_ndeInsertNode,
                                          /*  in */ PARS_XML_Node         a_ndeLinkBaseNode,
                                          /*  in */ PARS_DWORD            a_dwInsertionFlags)
{
    PARS_XML_PT_internalNode    ndeInsertNode           = PARS_NULL;
    PARS_XML_PT_internalNode    ndeLinkBaseNode         = PARS_NULL;
    PARS_XML_PT_internalNode    ndeLinkParentNode       = PARS_NULL;
    PARS_XML_PT_internalNode    ndeLinkFirstChildNode   = PARS_NULL;
    PARS_XML_PT_internalNode    ndeLinkLastChildNode    = PARS_NULL;
    PARS_XML_PT_internalNode    ndeLinkPrevNode         = PARS_NULL;
    PARS_XML_PT_internalNode    ndeLinkNextNode         = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_InsertNode");

    CHECK_NULL_PARAMETER(a_ndeInsertNode);

    ndeInsertNode   = (PARS_XML_PT_internalNode) a_ndeInsertNode;
    ndeLinkBaseNode = (PARS_XML_PT_internalNode) a_ndeLinkBaseNode;

    if(a_ndeInsertNode == a_ndeLinkBaseNode)
    {
        BAIL_OUT_0(PARS_DEBUG_LEVEL_ERROR, MWE_SYNTAX_ERROR, "Recursive linking of nodes forbidden!\n");
    }
    if(ndeLinkBaseNode)
    {
        switch (a_dwInsertionFlags)
        {
        case PARS_XML_INSERT_PARENT:
            ndeLinkParentNode   = ndeLinkBaseNode->p_ndeParent;
            if(ndeLinkParentNode)
            {
                if(ndeInsertNode->p_ndeFirstChild)
                {
                    /* insert node has children */
                    ndeInsertNode->p_ndeLastChild->p_ndeNextNeighbour = ndeLinkParentNode->p_ndeFirstChild;
                    if(ndeLinkParentNode->p_ndeFirstChild)
                    {
                        ndeLinkParentNode->p_ndeFirstChild->p_ndePrevNeighbour = ndeInsertNode->p_ndeLastChild;
                        ndeLinkLastChildNode = ndeLinkParentNode->p_ndeLastChild;
                    }
                    else
                    {
                        ndeLinkLastChildNode = ndeInsertNode->p_ndeLastChild;
                    }
                    ndeLinkFirstChildNode = ndeInsertNode->p_ndeFirstChild;
                }
                else
                {
                    ndeLinkLastChildNode = ndeLinkBaseNode->p_ndeLastChild;
                    ndeLinkFirstChildNode = ndeLinkBaseNode->p_ndeFirstChild;
                }
                /* only the insert node is now child */
                ndeLinkParentNode->dwChildCount = 0;
                ndeLinkParentNode->p_ndeFirstChild = PARS_NULL;
                ndeLinkParentNode->p_ndeLastChild = PARS_NULL;
            }
            else
            {
                /* linkbase is root node */
                /* insert new root node */
                if(ndeInsertNode->p_ndeFirstChild)
                {
                    ndeInsertNode->p_ndeLastChild->p_ndeNextNeighbour = ndeLinkBaseNode;
                    ndeLinkBaseNode->p_ndePrevNeighbour = ndeInsertNode->p_ndeLastChild;
                    ndeLinkFirstChildNode = ndeInsertNode->p_ndeFirstChild;
                }
                else
                {
                    ndeLinkFirstChildNode = ndeLinkBaseNode;
                }
                ndeLinkLastChildNode = ndeLinkBaseNode;
                ndeLinkBaseNode->p_shtXMLSheet->p_ndeRoot = ndeInsertNode;
            }
            /* no neighbors */
            break;
        case PARS_XML_INSERT_PREV:
            ndeLinkParentNode = ndeLinkBaseNode->p_ndeParent;
            ndeLinkPrevNode   = ndeLinkBaseNode->p_ndePrevNeighbour;
            ndeLinkNextNode   = ndeLinkBaseNode;
            /* no children */
            break;
        case PARS_XML_INSERT_NEXT:
            ndeLinkParentNode = ndeLinkBaseNode->p_ndeParent;
            ndeLinkPrevNode   = ndeLinkBaseNode;
            ndeLinkNextNode   = ndeLinkBaseNode->p_ndeNextNeighbour;
            ndeInsertNode->dwOriginIndent = ndeLinkBaseNode->dwOriginIndent;
            /* no children */
            break;
        case PARS_XML_INSERT_FIRST_CHILD:
            ndeLinkParentNode = ndeLinkBaseNode;
            ndeLinkNextNode   = ndeLinkBaseNode->p_ndeFirstChild;
            /* no children, no previous neighbor */
            break;
        case PARS_XML_INSERT_LAST_CHILD:
            ndeLinkParentNode = ndeLinkBaseNode;
            ndeLinkPrevNode   = ndeLinkBaseNode->p_ndeLastChild;
            /* no children, no next neighbor */
            break;
        default:
            BAIL_OUT_0( PARS_DEBUG_LEVEL_WARNING, MWE_FAIL,"Invalid insert flag.\n");
        }

        BAIL_OUT_ON_FAIL_EX( PARS_XML_LinkNode( ndeInsertNode,
                                                ndeLinkParentNode,
                                                ndeLinkPrevNode,
                                                ndeLinkNextNode,
                                                ndeLinkFirstChildNode,
                                                ndeLinkLastChildNode),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_LinkNode reports an error.\n");
        if(ndeLinkParentNode!=PARS_NULL)
        {
            ndeInsertNode->dwOriginIndent = ndeLinkParentNode->dwOriginIndent+PARS_XML_DEFAULT_INDENT;
            ndeInsertNode->dwOriginIndentClosingTag = ndeLinkBaseNode->dwOriginIndentClosingTag;
            ndeInsertNode->dwFlags |= PARS_XML_NODE_LF|PARS_XML_NODE_LF_CLOSING_TAG;
        }
        else
        {
            ndeInsertNode->dwOriginIndent = 0;
            ndeInsertNode->dwOriginIndentClosingTag = 0;
        }
        ndeInsertNode->p_shtXMLSheet = ndeLinkBaseNode->p_shtXMLSheet;
    }
    else
    {
        /* root node */
        /* no link node available -> do not link */
        BAIL_OUT_ON_FAIL_EX( PARS_XML_LinkNode( ndeInsertNode,
                                                ndeLinkParentNode,
                                                ndeLinkPrevNode,
                                                ndeLinkNextNode,
                                                ndeLinkFirstChildNode,
                                                ndeLinkLastChildNode),
                             PARS_DEBUG_LEVEL_HINT,
                             "PARS_XML_LinkNode reports an error.\n");
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_RemoveNode() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       removes a given node from its xml-tree                                                         |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  bi ]       node to remove from the xml sheet                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_RemoveNode( /* in */ PARS_XML_Node          a_ndeNode)
{
    PARS_XML_PT_internalNode    ndeTmpNode  = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_RemoveNode");

    CHECK_NULL_PARAMETER(a_ndeNode);

    ndeTmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    if(ndeTmpNode->p_ndeParent)
    {
        if(!(ndeTmpNode->p_ndePrevNeighbour))
        {
            /* first child */
            if(!(ndeTmpNode->p_ndeNextNeighbour))
            {
                /* no neighbors */
                ndeTmpNode->p_ndeParent->p_ndeFirstChild = PARS_NULL;
                ndeTmpNode->p_ndeParent->p_ndeLastChild  = PARS_NULL;
            }
            else
            {
                ndeTmpNode->p_ndeParent->p_ndeFirstChild = ndeTmpNode->p_ndeNextNeighbour;
            }
        }
        else if (!ndeTmpNode->p_ndeNextNeighbour)
        {
            ndeTmpNode->p_ndeParent->p_ndeLastChild = ndeTmpNode->p_ndePrevNeighbour;
        }
    }
    else
    {
        /* no parent -> Node is root node */
        ndeTmpNode->p_shtXMLSheet->p_ndeRoot = PARS_NULL;
    }

    if(ndeTmpNode->p_ndeNextNeighbour)
    {
        ndeTmpNode->p_ndeNextNeighbour->p_ndePrevNeighbour = ndeTmpNode->p_ndePrevNeighbour;
    }

    if(ndeTmpNode->p_ndePrevNeighbour)
    {
        ndeTmpNode->p_ndePrevNeighbour->p_ndeNextNeighbour = ndeTmpNode->p_ndeNextNeighbour;
    }

    /* reset node links */
    ndeTmpNode->p_ndeParent = PARS_NULL;
    ndeTmpNode->p_ndePrevNeighbour = PARS_NULL;
    ndeTmpNode->p_ndeNextNeighbour = PARS_NULL;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_FindNextAttribute() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       find the next attribute in the given xmltext                                                   |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_posActual             [  in ]       start position for search                                |
|       a_pposNext              [ out ]       position after the found attribute                       |
|       a_pposAttrName          [ out ]       name of the found attribute                              |
|       a_pdwAttrNameLen        [ out ]       length of attribute name                                 |
|       a_pposAttrValue         [ out ]       value of the found attribute                             |
|       a_pdwAttrValueLen       [ out ]       length of attribute value                                |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_FindNextAttribute ( /*  in */ XML_TEXT_POS    a_posActual,
                                         /* out */ XML_TEXT_POS*   a_pposNext,
                                         /* out */ XML_TEXT_POS*   a_pposAttrName,
                                         /* out */ PARS_DWORD*     a_pdwAttrNameLen,
                                         /* out */ XML_TEXT_POS*   a_pposAttrValue,
                                         /* out */ PARS_DWORD*     a_pdwAttrValueLen)
{
    XML_TEXT_POS        posActual        = PARS_NULL;
    PARS_CHAR           chSearchTag      = '\"';

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_FindNextAttribute");

    posActual        = a_posActual;

    posActual = PARS_XML_ScanNonWhiteSpace(posActual, END_OF_TAG);
    BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_NO_ERROR, "Find no next Attribute.\n");
    BAIL_OUT_ON_FAIL_EX( PARS_XML_FindNextValue(&posActual,
                                                 '=',
                                                 a_pposAttrName,
                                                 a_pdwAttrNameLen,
                                                 PARS_FALSE,
                                                 PARS_TRUE),
                         PARS_DEBUG_LEVEL_INFO,
                         "Find Next Value failed.\n");

    if (posActual == PARS_NULL)
    {
        BAIL_OUT_SILENT(MWE_NO_ERROR);
    }

    if (*(posActual-1)!='=')
    {
        posActual = PARS_XML_ScanChar(posActual, '=', END_OF_TAG, PARS_TRUE);

    }
    BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_NO_ERROR, "Find no next Attribute.\n");
    /* search for ' or " */
    posActual = PARS_XML_ScanNonWhiteSpace(posActual, END_OF_TAG);
    BAIL_OUT_ON_NULL_EX(posActual, PARS_DEBUG_LEVEL_HINT, MWE_NO_ERROR, "Find no quote.\n");
    if(posActual[0] == '\'')
    {
        chSearchTag = '\'';
        posActual++;
    }
    else if (posActual[0] == '\"')
    {
        chSearchTag = '\"';
        posActual++;
    }
    else
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"Invalid Attribute specification");
    }
    /* search and do not stop at tag end char */
    BAIL_OUT_ON_FAIL_EX( PARS_XML_FindNextValue(&posActual,
                                                 chSearchTag,
                                                 a_pposAttrValue,
                                                 a_pdwAttrValueLen,
                                                 PARS_TRUE,
                                                 PARS_FALSE),
                         PARS_DEBUG_LEVEL_INFO,
                         " Find next value failed ");
    BAIL_OUT_ON_NULL_EX(posActual, PARS_DEBUG_LEVEL_HINT, MWE_NO_ERROR, "Find no next value.\n");
    posActual = PARS_XML_ScanChar(posActual, chSearchTag, END_OF_TAG, PARS_TRUE);
    BAIL_OUT_ON_NULL_EX(posActual, PARS_DEBUG_LEVEL_HINT, MWE_NO_ERROR, "Find no end in Attribute.\n");
    *a_pposNext=posActual;

LANDING_ZONE;
    *a_pposNext         = PARS_NULL;
    *a_pposAttrName     = PARS_NULL;
    *a_pdwAttrNameLen   = 0;
    *a_pposAttrValue    = PARS_NULL;
    *a_pdwAttrValueLen  = 0;
END_LANDING_ZONE;
}

/*--- PARS_XML_StrCpyEx() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|           StrCpyEx copies a_strSrc to a_pstrDest, it allocates all necessary memory                  |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_pstrDest              [  bi ]       pointer to the string, where to copy the source          |
|       a_strSrc                [  in ]       pointer to characters to trace                           |
|                                                                                                      |
|    Return Value:                                                                                     |
|       PARS_BOOL               PARS_TRUE       successful                                             |
|                               PARS_FALSE      error occured                                          |                                                          |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_BOOL PARS_XML_StrCpyEx ( /*  bi */ PARS_LPSTR* a_pstrDest,
                              /*  in */ PARS_LPSTR  a_strSrc)
{
    PARS_LPSTR   strTemp        = PARS_NULL;
    PARS_DWORD   dwSrcLen       = 0;
    /* check parameters */
    if ((a_pstrDest == PARS_NULL) || (a_strSrc == PARS_NULL))
    {
        return PARS_FALSE;
    }

    /* new string */
    dwSrcLen = PARS_strlen(a_strSrc);
    strTemp = (PARS_LPSTR) XML_SMemMalloc( dwSrcLen+1 );
    if (strTemp == PARS_NULL)
    {
        return PARS_FALSE;
    }
    PARS_strlcpy( strTemp,
                  a_strSrc,
                  dwSrcLen+1,
                  dwSrcLen);
    if ((*a_pstrDest)!=PARS_NULL)
    {
        XML_SMemFree((*a_pstrDest));
    }
    *a_pstrDest = strTemp;

    return PARS_TRUE;
}

/*--- PARS_XML_NewPI() -------------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|     Creates a new Processing Instruction and initializes it                                          |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ppiActual             [ out ]         the new created PI                                     |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|        MWE_MEMORY             memory allocation error                                                |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_NewPI ( /* out */ PARS_XML_PI* a_ppiActual)
{
    PARS_XML_PT_internalPI  piWork  = PARS_NULL;

RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_NewPI");

    CHECK_NULL_PARAMETER(a_ppiActual);

    piWork=(PARS_XML_PT_internalPI) XML_SMemMalloc(sizeof(PARS_XML_T_PI));
    CHECK_MEMORY(piWork);

    /* Fill out piWork    */
    piWork->posXMLPI                    = PARS_NULL;
    piWork->dwPILen                     = 0;
    piWork->szPrefPI                    = PARS_NULL;
    piWork->dwPrefPILen                 = 0;

    piWork->pNextPI                     = PARS_NULL;

    *a_ppiActual = (PARS_XML_PI) piWork;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteAllPI() -------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|     Deletes all Processing Instructions of the given List                                            |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_piActual              [  in ]         the PI to delete                                       |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|        MWE_MEMORY             memory error                                                           |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_DeleteAllPI( /*  in */ PARS_XML_PI a_piActual)
{
    PARS_XML_PT_internalPI  piWork  = PARS_NULL;
    PARS_XML_PT_internalPI  piNext  = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeletePI");

    CHECK_NULL_PARAMETER(a_piActual);

    piWork = (PARS_XML_PT_internalPI) a_piActual;
    while(piWork)
    {
        piNext = piWork->pNextPI;
        PARS_XML_DeletePI((PARS_XML_PI)piWork);
        piWork = piNext;
    }
LANDING_ZONE;
END_LANDING_ZONE;
}
/*--- PARS_XML_DeletePI() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal XML function                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|                                                                                                      |
|     Deletes the given Processing Instructions                                                        |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_piActual              [  in ]         the PI to delete                                       |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|        MWE_MEMORY             memory error                                                           |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT PARS_XML_DeletePI ( /*  in */ PARS_XML_PI a_piActual)
{
    PARS_XML_PT_internalPI  piWork  = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeletePI");

    CHECK_NULL_PARAMETER(a_piActual);

    piWork=(PARS_XML_PT_internalPI) a_piActual;

    if(piWork->szPrefPI)
    {
        XML_SMemFree(piWork->szPrefPI);
    }

    XML_SMemFree(a_piActual);

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_CleanUpXMLService() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        Module Management                                                                             |
|                                                                                                      |
|    Description:                                                                                      |
|        Cleanup from service xml                                                                      |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_loRefCount         [  in ]   Number of open references                                      |
|        a_bForceShutdown     [  in ]   Flag weather this is the last chance                           |
|                                                                                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_RESULT                                                                                   |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_CleanUpXMLService( /*  in */ PARS_LONG         a_loRefCount,
                                                       /*  in */ PARS_BOOL         a_bForceShutdown)
{
RETURN_INIT(MWE_FAC_XML_SERVICE, "PARS_XML_CleanUpXMLService");
    START_SHUTDOWN_MODE(XML);

    if (PARS_NULL != PARS_XML_g_pFile)
    {
        MCB->FreeService("FileSys",        (PARS_LPVOID*) &PARS_XML_g_pFile);
        PARS_XML_g_pFile = PARS_NULL;
    }
#if PARS_XML_ENABLE_XPATH
    MCB->FreeService("VarIF",(PARS_LPVOID*)&PARS_XML_XPATH_g_pVarIF);
#endif /* PARS_XML_ENABLE_XPATH */
    MCB->FreeService("Stream",(PARS_LPVOID*)&PARS_XML_g_pStream);
    MCB->FreeService("ConvSrvc",(PARS_LPVOID*)&PARS_XML_g_pConversion);

FINALIZE_SHUTDOWN(XML, a_loRefCount, a_bForceShutdown);
#if PARS_USE_MEM_POOL
    PARS_XML_g_pMem->MemPoolFixDestruct(PARS_XML_g_pMemPoolAttributes);
    PARS_XML_g_pMemPoolAttributes = PARS_NULL;
    PARS_XML_g_pMem->MemPoolFixDestruct(PARS_XML_g_pMemPoolNodes);
    PARS_XML_g_pMemPoolNodes = PARS_NULL;
    PARS_XML_g_pMem->MemPoolFixDestruct(PARS_XML_g_pMemPoolSheets);
    PARS_XML_g_pMemPoolSheets = PARS_NULL;
#endif /* PARS_USE_MEM_POOL */

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_InitializeXMLService() ----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        Module Management                                                                             |
|                                                                                                      |
|    Description:                                                                                      |
|        initialize xml service                                                                        |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_strServiceName       [  in ]     service name to register service                           |
|        a_pContext             [  in ]     actual context                                             |
|        a_hModuleHandle        [  in ]     module handle of dll module                                |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_RESULT                                                                                   |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_InitializeXMLService ( /*  in */ PARS_LPSTR       a_strServiceName,
                                                    /*  in */ PARS_LPVOID      a_pContext,
                                                    /*  in */ PARS_HDLLMODULE  a_hModuleHandle)
{
    static XMLServiceInterface descrXML;
RETURN_INIT(MWE_FAC_XML_SERVICE, "InitializeXMLService");
    ASSIGN_MCB( (PARS_PT_MCB) a_pContext);
    INITIALIZE_SHUTDOWN_MODE(XML);

    /* Request Services we will need */
    BAIL_OUT_ON_FAIL_EX( MCB->LookUpService("Mem",(PARS_LPVOID*)&PARS_XML_g_pMem),
                         PARS_DEBUG_LEVEL_ERROR,
                         "Memory Service not available.\n");
    BAIL_OUT_ON_FAIL_EX( MCB->LookUpService("Stream",(PARS_LPVOID*)&PARS_XML_g_pStream),
        PARS_DEBUG_LEVEL_ERROR,
        "Stream Service not available.\n");

    XML_intern_SMemMalloc = PARS_XML_g_pMem->DbgSMemMalloc;
    XML_intern_SMemFree   = PARS_XML_g_pMem->DbgSMemFree;

#if PARS_USE_MEM_POOL

    if (MWE_FAILED(PARS_XML_g_pMem->MemPoolFixConstruct( "XML_Attributes",
                                                         PARS_XML_MAX_ATTRIBUTES,
                                                         sizeof(PARS_XML_T_Attributes),
                                                        &PARS_XML_g_pMemPoolAttributes)))
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_WARNING, MWE_FAIL, "Cannot create XML Attributes MemPool");
    }
    if (MWE_FAILED(PARS_XML_g_pMem->MemPoolFixConstruct( "XML_Nodes",
                                                         PARS_XML_MAX_NODES,
                                                         sizeof(PARS_XML_T_Node),
                                                        &PARS_XML_g_pMemPoolNodes)))
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_WARNING, MWE_FAIL, "Cannot create XML Nodes MemPool");
    }
    if (MWE_FAILED(PARS_XML_g_pMem->MemPoolFixConstruct( "XML_Sheets",
                                                         PARS_XML_MAX_SHEETS,
                                                         sizeof(PARS_XML_T_Sheet),
                                                        &PARS_XML_g_pMemPoolSheets)))
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_WARNING, MWE_FAIL, "Cannot create FileHandle MemPool");
    }
#endif /* PARS_USE_MEM_POOL */

    PARS_XML_BuildXMLServiceDescr(&descrXML);
    BAIL_OUT_ON_FAIL_EX( MCB->RegisterService( a_strServiceName,
                                                   &descrXML,
                                                    PARS_XML_CleanUpXMLService,
                                                    a_hModuleHandle),
                         PARS_DEBUG_LEVEL_ERROR,
                         "Registerservice Failed.");

    PARS_XML_g_aEscapeTable[0].cvSource = '"';
    PARS_XML_g_aEscapeTable[0].strReplacement = "&quot;";
    PARS_XML_g_aEscapeTable[0].bfLocation   = PARS_XML_FLTR_ATTRIBUTE_NODE;
    PARS_XML_g_aEscapeTable[1].cvSource = '\t';
    PARS_XML_g_aEscapeTable[1].strReplacement = "&#x9";
    PARS_XML_g_aEscapeTable[1].bfLocation   = PARS_XML_FLTR_ATTRIBUTE_NODE;
    PARS_XML_g_aEscapeTable[2].cvSource = '\r';
    PARS_XML_g_aEscapeTable[2].strReplacement = "&#xD";
    /* the pipe was a '>' before, but we (and the static code analysis) think that was an error */
    PARS_XML_g_aEscapeTable[2].bfLocation   = PARS_XML_FLTR_ATTRIBUTE_NODE|PARS_XML_FLTR_TEXT_NODE;
    PARS_XML_g_aEscapeTable[3].cvSource = '\n';
    PARS_XML_g_aEscapeTable[3].strReplacement = "&#xA";
    PARS_XML_g_aEscapeTable[3].bfLocation   = PARS_XML_FLTR_ATTRIBUTE_NODE;
    PARS_XML_g_aEscapeTable[4].cvSource = '&';
    PARS_XML_g_aEscapeTable[4].strReplacement = "&amp;";
    /* the pipe was a '>' before, but we (and the static code analysis) think that was an error */
    PARS_XML_g_aEscapeTable[4].bfLocation   = PARS_XML_FLTR_ATTRIBUTE_NODE|PARS_XML_FLTR_TEXT_NODE;
    PARS_XML_g_aEscapeTable[5].cvSource = '<';
    PARS_XML_g_aEscapeTable[5].strReplacement = "&lt;";
    PARS_XML_g_aEscapeTable[5].bfLocation   = PARS_XML_FLTR_ATTRIBUTE_NODE|PARS_XML_FLTR_TEXT_NODE;
    PARS_XML_g_aEscapeTable[6].cvSource = '>';
    PARS_XML_g_aEscapeTable[6].strReplacement = "&gt;";
    PARS_XML_g_aEscapeTable[6].bfLocation   = PARS_XML_FLTR_TEXT_NODE;
LANDING_ZONE;
    PARS_XML_CleanUpXMLService(0, PARS_TRUE);
END_LANDING_ZONE;
}

//XML Writer Methods
#if PARS_XML_ENABLE_XMLWRITER
/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Internal Helpers                                                                                                */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* --- PARS_XML_W_WriteIndent() ------------------------------------------------------------- PARS_XML_W_WriteIndent() ---*/
/**
 *  @brief   Writes some spaces into the file associated with a_pCtxt
 *
 *  @param   a_pCtxt   [  in ]    A context specifying the file where the indention should be written
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_WriteIndent() ------------------------------------------------------------- PARS_XML_W_WriteIndent() ---*/
static PARS_RESULT PARS_XML_W_WriteIndent (    /*  in */   PARS_XML_PT_WriteCtxt       a_pCtxt)
{
    PARS_INT    iBytesToWrite   = a_pCtxt->dwIndent*PARS_XML_SHIFTWIDTH;
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteIndent");
    while (0 < iBytesToWrite)
    {
		PARS_XML_g_pFile->xmlwrite( (PARS_LPVOID) " ");
        iBytesToWrite--;
    }
LANDING_ZONE;
END_LANDING_ZONE;
}

static PARS_VOID InsertIndent (PARS_DWORD numOfIndent)
{
    PARS_INT    iBytesToWrite   = numOfIndent*PARS_XML_SHIFTWIDTH;
    while (0 < iBytesToWrite)
    {
        PARS_XML_g_pFile->xmlwrite( (PARS_LPVOID) " ");
        iBytesToWrite--;
    }
}

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Interface Methods                                                                                               */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* --- PARS_XML_W_CtxtInit() ------------------------------------------------------------------ PARS_XML_W_CtxtInit() --- */
/**
 *  @brief   Initializes the writing context. This includes creating a file handle for the given path
 *
 *  @param   a_pvUserHandle    [  in ]    User handle to use for opening the export file
 *  @param   a_strFileName     [  in ]    Name of the file to open
 *  @param   a_dwNameLen       [  in ]    Length of the file name
 *  @param   a_ppCtxt          [ out ]    The created context. Free with PARS_XML_W_CtxtDelete().
 *
 *  @return  A PARS_RESULT
 *
 *  @note    a_ppCtxt must be freed using PARS_XML_W_CtxtDelete()!
 */
/* --- PARS_XML_W_CtxtInit() ------------------------------------------------------------------ PARS_XML_W_CtxtInit() --- */
PARS_RESULT DLLCALL PARS_XML_W_CtxtInit ( /*  in */    PARS_DWORD            a_dwNameLen,
                                          /* out */    PARS_XML_WRITER_CTX*  a_ppCtxt    )
{
    PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
    PARS_LPVOID             pvFile  = PARS_NULL;
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_CtxtInit");
    CHECK_NULL_PARAMETER(a_ppCtxt);
    *a_ppCtxt = PARS_NULL;
   
	CHECK_ZERO_PARAMETER(a_dwNameLen);

	pCtxt = (PARS_XML_PT_WriteCtxt) PARS_XML_g_pMem->SMemMalloc(sizeof(PARS_XML_T_WriteCtxt));
    CHECK_MEMORY(pCtxt);
    PARS_memset(pCtxt, 0, sizeof(PARS_XML_T_WriteCtxt));
    pCtxt->hFile = pvFile;
    *a_ppCtxt   = pCtxt;

LANDING_ZONE;
    if ((PARS_NULL != PARS_XML_g_pFile) && (PARS_NULL != pvFile))
    {
        PARS_XML_g_pFile->fclose(pvFile);
        pvFile = PARS_NULL;
    }
    if (PARS_NULL != pCtxt)
    {
        PARS_XML_g_pMem->SMemFree(pCtxt);
        pCtxt = PARS_NULL;
    }
END_LANDING_ZONE;
}

/* --- PARS_XML_W_CtxtDelete() -------------------------------------------------------------- PARS_XML_W_CtxtDelete() --- */
/**
 *  @brief   Method to cleanup the context. Calling this method results in writing the file.
 *
 *  @param   a_ppCtxt  [  bi ]    Context to delete
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_CtxtDelete() -------------------------------------------------------------- PARS_XML_W_CtxtDelete() --- */
PARS_RESULT DLLCALL PARS_XML_W_CtxtDelete ( /*  bi */  PARS_XML_WRITER_CTX*  a_ppCtxt    )
{
    PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_CtxtDelete");
    CHECK_NULL_PARAMETER(a_ppCtxt);
    pCtxt = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;
    *a_ppCtxt = PARS_NULL;
    if (PARS_NULL != pCtxt->hFile)
    {
        PARS_XML_g_pFile->fflush(pCtxt->hFile);
        PARS_XML_g_pFile->fclose(pCtxt->hFile);
    }
    PARS_XML_g_pMem->SMemFree(pCtxt);
LANDING_ZONE;
END_LANDING_ZONE;
}

/* --- PARS_XML_W_WriteDocStart() -------------------------------------------------------- PARS_XML_W_WriteDocStart() --- */
/**
 *  @brief   begins the <?xml start line.
 *
 *           if a_bHasAttributes is false needed default attributes are written.
 *           WriteDocStartEnd() must always be called (even if there are no own attributes)
 *           Own attributes can be added via WriteAttribute().
 *           the xml start line must not have a content, so WriteTagValue() must not be called for this DocStart tag.
 *
 *  @param   a_pCtxt           [  in ]    Context to use
 *  @param   a_bHasAttributes  [  in ]    Indicates if the tag has attributes or not
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_WriteDocStart() -------------------------------------------------------- PARS_XML_W_WriteDocStart() --- */
PARS_RESULT DLLCALL PARS_XML_W_WriteDocStart( PARS_XML_WRITER_CTX       a_pCtxt,
                                              PARS_BOOL                 a_bHasAttributes )
{
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteDocStart");
    if( a_bHasAttributes == PARS_FALSE )
    {
        PARS_XML_W_WriteTagStart( a_pCtxt,
                                  a_bHasAttributes,
                                  MSTR_STATIC_C("?xml"));
        PARS_XML_W_WriteAttribute( a_pCtxt,
                                   MSTR_STATIC_C("version"),
                                   MSTR_STATIC_C("1.0") );
        RETURN( MWE_NO_ERROR );
    }
    RETURN( PARS_XML_W_WriteTagStart( a_pCtxt,
                                      a_bHasAttributes,
                                      MSTR_STATIC_C("?xml")) );
LANDING_ZONE;
END_LANDING_ZONE;
}

/* --- PARS_XML_W_WriteDocStartEnd() -------------------------------------------------- PARS_XML_W_WriteDocStartEnd() --- */
/**
 *  @brief   Closes the <?xml start line with a "?>\n"
 *
 *  @param   a_pCtxt           [  in ]    Context to use
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_WriteDocStartEnd() -------------------------------------------------- PARS_XML_W_WriteDocStartEnd() --- */
PARS_RESULT DLLCALL PARS_XML_W_WriteDocStartEnd( /*  in */ PARS_XML_WRITER_CTX       a_pCtxt )
{
RETURN_INIT (MWE_FAC_XML_SERVICE, "WriteDocStartEnd");
    RETURN( PARS_XML_W_WriteTagEnd( a_pCtxt,
                                    MSTR_STATIC_C("?") ) );
LANDING_ZONE;
END_LANDING_ZONE;
}

/* --- PARS_XML_W_WriteTagStart() -------------------------------------------------------- PARS_XML_W_WriteTagStart() --- */
/**
 *  @brief   Method to write the opening of a tag. The parameter a_bHasAttributes determines if the tag
 *              will be closed using a '>' or if it will stay open to append attributes to that tag. So,
 *              if your tag name is 'foo' and you pass a_bHasAttributes as PARS_FALSE, the written value
 *              will be '\<foo\>', if you set a_bHasAttributes to PARS_TRUE, only '<foo' will be written.
 *
 *  @param   a_pCtxt           [  in ]    Context to use
 *  @param   a_bHasAttributes  [  in ]    Indicates if the tag has attributes or not
 *  @param   a_pmstrTagName    [  in ]    The name of the tag to write
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_WriteTagStart() -------------------------------------------------------- PARS_XML_W_WriteTagStart() --- */
PARS_RESULT DLLCALL PARS_XML_W_WriteTagStart(PARS_XML_WRITER_CTX a_pCtxt,
                                             PARS_BOOL       a_bHasAttributes,
                                             PARS_LPSTR		 a_pmstrTagName )
{
    PARS_XML_PT_WriteCtxt   pCtxt   = (PARS_XML_PT_WriteCtxt) a_pCtxt;
    PARS_LPSTR  strTemp     = PARS_NULL;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteTagStart");
    CHECK_NULL_PARAMETER(a_pCtxt);
    CHECK_NULL_PARAMETER(a_pmstrTagName);
    if (PARS_FALSE != pCtxt->bMustCloseTag)
    {
        strTemp = ">\n";
        PARS_XML_g_pFile->xmlwrite(strTemp);
        PARS_XML_INCINDENT(pCtxt);
        pCtxt->bMustCloseTag = PARS_FALSE;
    }
    PARS_XML_W_WriteIndent(pCtxt);
    strTemp = "<";
    PARS_XML_g_pFile->xmlwrite(strTemp);
    PARS_XML_g_pFile->xmlwrite(MSTR_Raw(a_pmstrTagName));
    if (PARS_FALSE == a_bHasAttributes)
    {
        strTemp = ">\n";
        PARS_XML_g_pFile->xmlwrite(strTemp);
        PARS_XML_INCINDENT(pCtxt);
    }
    else
    {
        pCtxt->bMustCloseTag = PARS_TRUE;
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/* --- PARS_XML_W_WriteTagEnd() ------------------------------------------------------------ PARS_XML_W_WriteTagEnd() --- */
/**
 *  @brief   Writes the end of a tag. What exactly gets written depends on a_strTagName and a_dwNameLen.
 *              If a_strTagName is not PARS_NULL and a_dwNameLen is greater than zero, these values
 *              will be written out. For example, if your tag name is 'foo' again and you pass that name
 *              to this method, '\</foo\>' will be written. If you pass no name, only '/>' gets written.
 *
 *  @param   a_pCtxt        [  in ]    Context to use
 *  @param   a_pmstrTagName [  in ]    Name of the tag
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_WriteTagEnd() ------------------------------------------------------------ PARS_XML_W_WriteTagEnd() --- */
//PARS_RESULT DLLCALL PARS_XML_W_WriteTagEnd ( /*  in */  PARS_XML_WRITER_CTX  a_pCtxt,
//                                             /*  in */  PARS_PT_String       a_pmstrTagName )
PARS_RESULT DLLCALL PARS_XML_W_WriteTagEnd ( /*  in */  PARS_XML_WRITER_CTX  a_pCtxt,
                                             /*  in */  PARS_LPSTR       a_pmstrTagName )
{
    PARS_XML_PT_WriteCtxt   pCtxt   = (PARS_XML_PT_WriteCtxt) a_pCtxt;
    PARS_LPSTR              strTemp = PARS_NULL;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteTagEnd");
    CHECK_NULL_PARAMETER(a_pCtxt);
    if ( (a_pmstrTagName == PARS_NULL) || (MSTR_Len(a_pmstrTagName) == 0) )
    {
        /* end of an empty tag */
        strTemp = END_EMPTY_TAG;
        PARS_XML_g_pFile->xmlwrite(strTemp);
    }
    else
    {
        /* we have to write the end of the tag */
        PARS_XML_DECINDENT(pCtxt);
        PARS_XML_W_WriteIndent(pCtxt);
        strTemp = "</";
        PARS_XML_g_pFile->xmlwrite(strTemp);
        PARS_XML_g_pFile->xmlwrite(MSTR_Raw(a_pmstrTagName));
        strTemp = ">";
        PARS_XML_g_pFile->xmlwrite(strTemp);
    }
    strTemp = "\n";
    PARS_XML_g_pFile->xmlwrite(strTemp);
    pCtxt->bMustCloseTag = PARS_FALSE;

LANDING_ZONE;
END_LANDING_ZONE;
}

/* --- PARS_XML_W_WriteTagValue() -------------------------------------------------------- PARS_XML_W_WriteTagValue() --- */
/**
 *  @brief   Simply writes the value of a tag. Nothing else.
 *
 *  @param   a_pCtxt       [  in ]    Context to use
 *  @param   a_pmstrValue  [  in ]    Value to write
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_WriteTagValue() -------------------------------------------------------- PARS_XML_W_WriteTagValue() --- */
PARS_RESULT DLLCALL PARS_XML_W_WriteTagValue (/*  in */  PARS_XML_WRITER_CTX  a_pCtxt,
                                              /*  in */  PARS_LPSTR       a_pmstrValue )
{
    PARS_XML_PT_WriteCtxt   pCtxt   = (PARS_XML_PT_WriteCtxt) a_pCtxt;
    PARS_LPSTR  strTemp = PARS_NULL;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteTagValue");
    CHECK_NULL_PARAMETER(a_pCtxt);
    if (PARS_FALSE != pCtxt->bMustCloseTag)
    {
        strTemp = ">\n";
        PARS_XML_g_pFile->xmlwrite(strTemp);
        PARS_XML_INCINDENT(pCtxt);
        pCtxt->bMustCloseTag = PARS_FALSE;
    }

    PARS_XML_W_WriteIndent(pCtxt);
    PARS_XML_g_pFile->xmlwrite(MSTR_Raw(a_pmstrValue));
    strTemp = "\n";
    PARS_XML_g_pFile->xmlwrite(strTemp);
LANDING_ZONE;
END_LANDING_ZONE;
}

PARS_RESULT DLLCALL PARS_XML_W_WriteAttribute( /*  in */  PARS_XML_WRITER_CTX a_pCtxt,
                                               /*  in */  PARS_LPSTR      a_pmstrAttName,
                                               /*  in */  PARS_LPSTR      a_pmstrValue )
{
    PARS_LPSTR  strTemp = PARS_NULL;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteAttribute");
    CHECK_NULL_PARAMETER(a_pCtxt);
    CHECK_NULL_PARAMETER(a_pmstrAttName);
    CHECK_NULL_PARAMETER(a_pmstrValue);

    strTemp = " ";
    PARS_XML_g_pFile->xmlwrite(strTemp);
    PARS_XML_g_pFile->xmlwrite(a_pmstrAttName);
    strTemp = "=\"";
    PARS_XML_g_pFile->xmlwrite(strTemp);
    PARS_XML_g_pFile->xmlwrite(a_pmstrValue);
    strTemp = "\"";
    PARS_XML_g_pFile->xmlwrite(strTemp);

LANDING_ZONE;
END_LANDING_ZONE;
}

PARS_RESULT DLLCALL WriteValue(/*  in */  PARS_LPSTR      a_pmstrValue )
{
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteAttribute");
	
	CHECK_NULL_PARAMETER(a_pmstrValue);
	PARS_XML_g_pFile->xmlwrite(a_pmstrValue);

LANDING_ZONE;
END_LANDING_ZONE;
}

/* --- PARS_XML_W_WriteComment() ---------------------------------------------------------- PARS_XML_W_WriteComment() --- */
/**
 *  @brief   Writes the given comment to the file associated with the given context
 *
 *  @param   a_pCtxt         [  in ]    Context to use
 *  @param   a_pmstrComment  [  in ]    Comment to write to the export file
 *
 *  @return  A PARS_RESULT
 */
/* --- PARS_XML_W_WriteComment() ---------------------------------------------------------- PARS_XML_W_WriteComment() --- */
PARS_RESULT DLLCALL PARS_XML_W_WriteComment  (  /*  in */   PARS_XML_WRITER_CTX   a_pCtxt,
                                                /*  in */   PARS_LPSTR        a_pmstrComment )
{
    PARS_XML_PT_WriteCtxt   pCtxt   = (PARS_XML_PT_WriteCtxt) a_pCtxt;
    PARS_LPSTR              strTemp     = PARS_NULL;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_W_WriteComment");
    CHECK_NULL_PARAMETER(a_pmstrComment);
    if (PARS_FALSE != pCtxt->bMustCloseTag)
    {
        strTemp = ">\n";
        PARS_XML_g_pFile->xmlwrite(strTemp);
        PARS_XML_INCINDENT(pCtxt);
        pCtxt->bMustCloseTag = PARS_FALSE;
    }
    PARS_XML_W_WriteIndent(pCtxt);
    strTemp = "<!-- ";
    PARS_XML_g_pFile->xmlwrite(strTemp);
    PARS_XML_g_pFile->xmlwrite(MSTR_Raw(a_pmstrComment));
    strTemp = " -->\n";
    PARS_XML_g_pFile->xmlwrite(strTemp);
LANDING_ZONE;
END_LANDING_ZONE;
}
#else /* PARS_XML_ENABLE_XMLWRITER */
    static PARS_INT iDummy = 0;
#endif /* PARS_XML_ENABLE_XMLWRITER */

PARS_VOID (WriteOnlyStartTag)(PARS_XML_WRITER_CTX* a_ppCtxt, PARS_LPSTR tagNameStr)
{
	PARS_XML_PT_WriteCtxt   pCtxt   = (PARS_XML_PT_WriteCtxt) a_ppCtxt;
    PARS_LPSTR  strTemp     = PARS_NULL;
    PARS_XML_W_WriteIndent(pCtxt);
    strTemp = "<";
    PARS_XML_g_pFile->xmlwrite(strTemp);
    PARS_XML_g_pFile->xmlwrite(tagNameStr);
   
	WriteValue(">");
}

PARS_VOID (WriteOnlyEndTag)(PARS_XML_WRITER_CTX* a_ppCtxt, 
	PARS_DWORD startIndent, PARS_LPSTR tagNameStr)
{
	PARS_LPSTR strTemp = PARS_NULL;

	PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
	pCtxt = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;

	InsertIndent(startIndent);

	strTemp = "</";
	PARS_XML_g_pFile->xmlwrite(strTemp);
    PARS_XML_g_pFile->xmlwrite(MSTR_Raw(tagNameStr));
    strTemp = ">\n";
	WriteValue(strTemp);
}

PARS_VOID (WriteValueTag)(XMLServiceInterface* pXMLService, PARS_XML_WRITER_CTX* a_ppCtxt, PARS_LPSTR tagNameStr, 
	PARS_LPSTR valStr)
{
	PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
	
	pCtxt   = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;

	WriteOnlyStartTag(a_ppCtxt, tagNameStr);
	WriteValue( valStr);
	pXMLService->WriteTagEnd( *a_ppCtxt, tagNameStr);
}

PARS_VOID (WriteValueTagWithAttr)(XMLServiceInterface* pXMLService, PARS_XML_WRITER_CTX* a_ppCtxt, PARS_DWORD startIndent,
PARS_LPSTR tagNameStr, PARS_LPSTR valTagStr, PARS_LPSTR attrStr, PARS_LPSTR attrValStr)
{
	PARS_LPSTR strTemp = PARS_NULL;

	PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
	pCtxt = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;

	InsertIndent(startIndent);

	pXMLService->WriteTagStart( *a_ppCtxt,
                                  1,
                                  MSTR_STATIC_C(tagNameStr) );
	pXMLService->WriteAttribute( *a_ppCtxt,
                                   attrStr,
                                   attrValStr );

	strTemp = ">";
    PARS_XML_g_pFile->xmlwrite(strTemp);
	
	WriteValue(valTagStr);
	pXMLService->WriteTagEnd( *a_ppCtxt, tagNameStr);
}

PARS_VOID (SetIndent)(PARS_XML_WRITER_CTX* a_ppCtxt)
{
	PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
    
	pCtxt   = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;
	PARS_XML_INCINDENT(pCtxt);
	PARS_XML_W_WriteIndent(pCtxt);
}

PARS_DWORD (WriteTagStartWithValueTag)(XMLServiceInterface* pXMLService, PARS_XML_WRITER_CTX* a_ppCtxt, PARS_DWORD startIndent, 
	PARS_LPSTR tagStr, PARS_LPSTR attrStr, PARS_LPSTR attrValStr, PARS_LPSTR valTagStr, PARS_LPSTR tagValStr)
{
	PARS_DWORD indentVal = 0;
	PARS_LPSTR strTemp = PARS_NULL;

	PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
	pCtxt = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;

	InsertIndent(startIndent);

	pXMLService->WriteTagStart( *a_ppCtxt,
                                  1,
                                  MSTR_STATIC_C(tagStr) );
	pXMLService->WriteAttribute( *a_ppCtxt,
                                   attrStr,
                                   attrValStr );

	strTemp = ">\n";
    PARS_XML_g_pFile->xmlwrite(strTemp);
	indentVal = startIndent + 1;
	InsertIndent(indentVal);
	WriteValueTag(pXMLService, a_ppCtxt, valTagStr, tagValStr);

	return indentVal;
}

PARS_VOID (WriteRecord)(XMLServiceInterface* pXMLService, PARS_XML_WRITER_CTX* a_ppCtxt, PARS_DWORD startIndent,
	PARS_LPSTR attrValStr1, PARS_PND_REMA_RECORD_TYPE record)
{
	PARS_LPSTR tagNameStr = "Field";
	PARS_LPSTR attrStr1 = "Key";
	PARS_LPSTR attrStr2 = "Length";

	PARS_LPSTR valTagStr = record.recordData;
	PARS_LPSTR attrValStr2 = record.recordLen;

	PARS_LPSTR strTemp = PARS_NULL;
	
	PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
	pCtxt = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;

	InsertIndent(startIndent);

	strTemp = "<";
    PARS_XML_g_pFile->xmlwrite(strTemp);
    PARS_XML_g_pFile->xmlwrite(tagNameStr);

	pXMLService->WriteAttribute( *a_ppCtxt,
                                   attrStr1,
                                   attrValStr1 );
	pXMLService->WriteAttribute( *a_ppCtxt,
                                   attrStr2,
                                   attrValStr2 );
	strTemp = ">";
    PARS_XML_g_pFile->xmlwrite(strTemp);

	WriteValue(valTagStr);
	pXMLService->WriteTagEnd( *a_ppCtxt, tagNameStr);
}

PARS_VOID (CreateRemaRecords)(XMLServiceInterface* pXMLService, PARS_XML_WRITER_CTX* a_ppCtxt, PARS_DWORD startIndent, 
	PARS_PND_REMA_DATA_TYPE rema)
{
	//PARS_PND_REMA_DATA_TYPE rema
	PARS_LPSTR tagNameStr = "Value";
    PARS_LPSTR attrStr1 = "Datatype";
    PARS_LPSTR attrValStr1 = "SparseArray";
    PARS_LPSTR attrStr2 = "Valuetype";
    PARS_LPSTR attrValStr2 = "BLOB";
	PARS_LPSTR strTemp = PARS_NULL;
	
	PARS_XML_PT_WriteCtxt   pCtxt   = PARS_NULL;
	pCtxt = (PARS_XML_PT_WriteCtxt) *a_ppCtxt;

	InsertIndent(startIndent);
   
	pXMLService->WriteTagStart( *a_ppCtxt,
                                  1,
                                  MSTR_STATIC_C(tagNameStr) );
	pXMLService->WriteAttribute( *a_ppCtxt,
                                   attrStr1,
                                   attrValStr1 );
	pXMLService->WriteAttribute( *a_ppCtxt,
                                   attrStr2,
                                   attrValStr2 );
	strTemp = ">\n";

	PARS_XML_g_pFile->xmlwrite(strTemp);
	if (rema.ipSuiteRecord.recordData != LSA_NULL)
		if ((LSA_INT16)xml_strlen((char *)rema.ipSuiteRecord.recordData) != 0)
		WriteRecord(pXMLService, a_ppCtxt, startIndent + 1, "23000", rema.ipSuiteRecord);
	if (rema.nosRecord.recordData != LSA_NULL)
		if ((LSA_INT16)xml_strlen((char *)rema.nosRecord.recordData) != 0)
		WriteRecord(pXMLService, a_ppCtxt, startIndent + 1, "23003", rema.nosRecord);
	if (rema.mib2Record.recordData != LSA_NULL)
		if ((LSA_INT16)xml_strlen((char *)rema.mib2Record.recordData) != 0)
		WriteRecord(pXMLService, a_ppCtxt, startIndent + 1, "F8000002", rema.mib2Record);
	
	WriteOnlyEndTag(a_ppCtxt, startIndent, "Value");
}

PARS_RESULT DLLCALL PARS_Generate_Rema_XML_Buffer(PARS_PND_REMA_DATA_TYPE rema, PARS_LPSTR** remaData, PARS_DWORD* remaDataLength)
{
	XMLServiceInterface*    pXMLService       = PARS_NULL;    
	PARS_DWORD indentVal = 0;
	PARS_XML_T_WriteCtxt a_ppCtxt2;
    PARS_XML_WRITER_CTX* a_ppCtxt = (PARS_XML_WRITER_CTX*)(&a_ppCtxt2);

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_Generate_Rema_XML_Buffer");

    a_ppCtxt2.dwIndent = 0;	

	PARS_AllocateXmlBuffer();
	PARS_XML_InitializeStandaloneXMLService( PARS_SafeMalloc, PARS_SafeFree, &pXMLService);
	
	pXMLService->InitWriteCtxt(8, a_ppCtxt);

	WriteTagStartWithValueTag(pXMLService, a_ppCtxt, 0, "Object", "Name", "REMA", "ClassRID", "1");
	WriteTagStartWithValueTag(pXMLService, a_ppCtxt, 2, "Object", "Name", "PN Driver_1", "ClassRID", "2");
	indentVal = WriteTagStartWithValueTag(pXMLService, a_ppCtxt, 4, "Object", "Name", "PROFINET Interface", "ClassRID", "3");
	WriteValueTagWithAttr(pXMLService, a_ppCtxt, indentVal, "Key", "32768", "AID", "2"); 
	WriteTagStartWithValueTag(pXMLService, a_ppCtxt, indentVal, "Variable", "Name", "DataRecordsRema", "AID", "11");

	CreateRemaRecords(pXMLService, a_ppCtxt, indentVal+1, rema);

	WriteOnlyEndTag(a_ppCtxt, indentVal, "Variable");
	WriteOnlyEndTag(a_ppCtxt, 4, "Object");
	WriteOnlyEndTag(a_ppCtxt, 2, "Object");
	WriteOnlyEndTag(a_ppCtxt, 0, "Object");
	
	*remaData = (PARS_LPSTR*)GetXmlBuffer();
	*remaDataLength = GetXmlBufferLength();	

	pXMLService->DeleteWriteCtxt(a_ppCtxt);

	LANDING_ZONE;
	END_LANDING_ZONE;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

