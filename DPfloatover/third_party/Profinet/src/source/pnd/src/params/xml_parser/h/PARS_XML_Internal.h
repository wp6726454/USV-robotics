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
/*  F i l e               &F: PARS_XML_Internal.h                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The XML Parser is written in ANSI C.                                     */
/*  This file contains all private Prototypes and Structs /Defines           */
/*  of the XML Parser, the public prototypes and structs are in PARS_XML.h   */
/*                                                                           */
/*****************************************************************************/

/*----------------------------------------------------------------------------------------------------*\
|   Defines                                                                                  Defines   |
\*----------------------------------------------------------------------------------------------------*/
#ifndef __XMLPARSER_DEFINED__
#define __XMLPARSER_DEFINED__

#define    RETURN_ON_NULL(a)    if (a==PARS_NULL)                           \
                                {                                           \
                                    return PARS_NULL;                       \
                                }

#if PARS_MEMORY_TRACE
    #define PARS_MEM_TRACE_ARGS_DEC        , PARS_INT a_nLineNumber, PARS_LPSTR   a_strFileName
    #define PARS_MEM_TRACE_ARGS_MAC        , __LINE__,__FILE__
    #define PARS_MEM_TRACE_ARGS            , a_nLineNumber, a_strFileName
    #define PARS_MEM_TRACE_ARGS_UNUSED     PARS_UNREFERENCED_PARAMETER(a_nLineNumber);PARS_UNREFERENCED_PARAMETER(a_strFileName);
#else
    #define PARS_MEM_TRACE_ARGS_DEC        
    #define PARS_MEM_TRACE_ARGS_MAC        
    #define PARS_MEM_TRACE_ARGS            
    #define PARS_MEM_TRACE_ARGS_UNUSED
#endif 

extern  XML_T_SMemMalloc         XML_intern_SMemMalloc;
extern  XML_T_SMemFree           XML_intern_SMemFree;
extern  XML_T_extern_SMemMalloc  XML_extern_SMemMalloc;
extern  XML_T_extern_SMemFree    XML_extern_SMemFree;


#define XML_SMemMalloc(x)                   XML_intern_SMemMalloc((x)  PARS_MEM_TRACE_ARGS_MAC)
#define XML_SMemFree(x)                     XML_intern_SMemFree((x)  PARS_MEM_TRACE_ARGS_MAC)

    extern MemServiceInterface*             PARS_XML_g_pMem;    /* Memory Services      */
    extern FileSysServiceInterface*         PARS_XML_g_pFile;   /* file system services */
    extern PARS_CONV_PT_ServiceInterface    PARS_XML_g_pConversion;

/*----------------------------------------------------------------------------------------------------*\
|   Internal Flags                                                                    Internal Flags   |
\*----------------------------------------------------------------------------------------------------*/

#define PARS_XML_NODE_EXPANDED          0x00000001          /* node is expanded */
#define PARS_XML_NODE_LF                0x00000002          /* Was a Linefeed in front of the node */
#define PARS_XML_NODE_LF_CLOSING_TAG    0x00000004          /* Was a Linefeed in front of the closing tag of the node */


/*----------------------------------------------------------------------------------------------------*\
|   Error Codes for Error Callback Function                  Error Codes for Error Callback Function   |
\*----------------------------------------------------------------------------------------------------*/

#define XML_ERR_NO_ERROR            0x0L               /* Call is just for info                 */

#define XML_WRNG_BASE               0x1000L            /* Following return codes are warnings   */
#define XML_WRNG_NO_VERSION         0x1001L            /* XML Tag contains no Version Info      */
#define XML_WRNG_NO_XML_TAG         0x1002L            /* Theres no XML Tag                     */

#define XML_ERR_BASE                0x10000L           /* Following return codes are errors     */

#define XML_FATAL_BASE              0x100000L          /* Following return codes are fatals     */
#define XML_FATAL_OUT_OF_MEM        0x100001L          /* Cannot consume memory                 */
#define XML_FATAL_NO_VALID_XML      0x100002L          /* No <?XML ?> TAG                       */
#define XML_FATAL_SYNTAX_ERROR      0x100003L          /* z.b. Falsche option                   */
#define XML_FATAL_NO_ROOT           0x100004L          /* No Root Tag                           */

#define  PARS_XML_USE_NEIGHBOURS    PARS_TRUE
#define  PARS_XML_IGNORE_NEIGHBORS  PARS_FALSE
/*----------------------------------------------------------------------------------------------------*\
|   Externals                                                                              Externals   |
\*----------------------------------------------------------------------------------------------------*/
GLOBAL_SHUTDOWN_CONTROL(XML)

/*----------------------------------------------------------------------------------------------------*\
|   Typedef Structures                                                            Typedef Structures   |
\*----------------------------------------------------------------------------------------------------*/

typedef PARS_RESULT (DLLCALL *PARS_XML_ASYNC_CB)(PARS_LPVOID, PARS_DWORD, PARS_BOOL, PARS_LPVOID);

typedef struct S_PARS_XMLAsyncStruct
{
    PARS_XML_ASYNC_CB           cbXMLAsync;
    PARS_LPVOID                 pCookie;
    PARS_DWORD                  dwC14NForm;
    PARS_DWORD                  dwIndent;
    PARS_LPSTR                  strNewLine;
    PARS_DWORD                  dwBehavior;
} PARS_XMLAsyncStruct;

/*----------------------------------------------------------------------------------------------------*\
|   Structures                                                                            Structures   |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*\
|   Prototypes                                                                            Prototypes   |
\*----------------------------------------------------------------------------------------------------*/

#ifndef STATIC_BINDING
PARS_RESULT DLLCALL InitializeService               ( /*  in */ PARS_LPSTR              a_strServiceName,
                                                      /*  in */ PARS_PT_MCB             a_pContext,
                                                      /*  in */ PARS_HDLLMODULE         a_hModuleHandle);
#endif
LSA_CHAR* GetXmlBuffer();	//TODO
PARS_DWORD GetXmlBufferLength(); //TODO

PARS_VOID       PARS_XML_BuildXMLServiceDescr       ( /* out */ XMLServiceInterface*    pXML);

XML_TEXT_POS    PARS_XML_ScanNextEncounter          ( /*  in */ XML_TEXT_POS            a_posActual,
                                                      /*  in */ PARS_LPSTR              a_strTagName);
                                                                                        
XML_TEXT_POS    PARS_XML_ScanNonWhiteSpace          ( /*  in */ XML_TEXT_POS            a_posActual,
                                                      /*  in */ PARS_CHAR               a_cTerminator);
                                                                                        
XML_TEXT_POS    PARS_XML_ScanChar                   ( /*  in */ XML_TEXT_POS            a_posActual,
                                                      /*  in */ PARS_CHAR               a_cToSearchFor,
                                                      /*  in */ PARS_CHAR               a_cTerminator,
                                                      /*  in */ PARS_BOOL               a_bStrict);
                                                                                        
PARS_LPSTR      PARS_XML_ParseAttribute             ( /*  in */ XML_TEXT_POS            a_posActual,
                                                      /*  in */ PARS_LPSTR              a_strAttrName,
                                                      /* out */ PARS_LPSTR              a_strValue,
                                                      /*  in */ PARS_INT                a_nMaxLen);
                                                                                        
PARS_RESULT     PARS_XML_FindNextValue              ( /*  in */ XML_TEXT_POS*           a_pposActual,
                                                      /*  in */ PARS_CHAR               a_cTerminator,
                                                      /* out */ XML_TEXT_POS*           a_pposValue,
                                                      /* out */ PARS_DWORD*             a_pdwValueLen,
                                                      /*  in */ PARS_BOOL               a_bIgnoreWhiteSpace,
                                                      /*  in */ PARS_BOOL               a_bStopAtTagEnd);
                                                                                        
                                                                                        
PARS_LPSTR      PARS_XML_GetNextValue               ( /*  in */ XML_TEXT_POS            a_posActual,
                                                      /*  in */ PARS_CHAR               a_cTerminator,
                                                      /* out */ PARS_LPSTR              a_strValue,
                                                      /*  in */ PARS_INT                a_nMaxLen,
                                                      /*  in */ PARS_BOOL               a_bIgnoreWhiteSpace);
                                                                                        
PARS_RESULT     PARS_XML_InsertNode                 ( /*  in */ PARS_XML_Node           a_ndeInsertNode,
                                                      /*  in */ PARS_XML_Node           a_ndeLinkBaseNode,
                                                      /*  in */ PARS_DWORD              a_dwInsertionFlags);
                                                                                        
PARS_RESULT     PARS_XML_RemoveNode                 ( /* in */ PARS_XML_Node            a_ndeNode);
                                                                                        
PARS_RESULT     PARS_XML_LinkNode                   ( /* in */ PARS_XML_PT_internalNode a_ndeNode,
                                                      /* bi */ PARS_XML_PT_internalNode a_ndeParent,
                                                      /* bi */ PARS_XML_PT_internalNode a_ndePrevNeighbour,
                                                      /* bi */ PARS_XML_PT_internalNode a_ndeNextNeighbour,
                                                      /* bi */ PARS_XML_PT_internalNode a_ndeFirstChild,
                                                      /* bi */ PARS_XML_PT_internalNode a_ndeLastChild);

PARS_RESULT     PARS_XML_NewNode                    ( /* out */ PARS_XML_Node*          a_pndeValue );
                                                        
PARS_RESULT     PARS_XML_NewSheet                   ( /* out */ PARS_XML_Sheet*         a_pshtXMLSheet );

PARS_RESULT     PARS_XML_NewPI                      ( /* out */ PARS_XML_PI*            a_ppiActual);

PARS_RESULT     PARS_XML_DeleteAllPI                ( /*  in */ PARS_XML_PI             a_piActual);

PARS_RESULT     PARS_XML_DeletePI                   ( /*  in */ PARS_XML_PI             a_piActual);
                                                                                      
PARS_RESULT     PARS_XML_SafeStrCat                 ( /*  bi */ PARS_LPSTR              a_szDestination,
                                                      /*  bi */ PARS_DWORD*             a_pdwDestinationLen,
                                                      /*  in */ PARS_LPSTR              a_szSource,
                                                      /*  in */ PARS_DWORD              a_dwSourceLen,
                                                      /*  in */ PARS_DWORD              a_dwDestBufLen,
                                                      /*  in */ PARS_XMLAsyncStruct*    a_pXMLAsyncStruct);

PARS_RESULT     PARS_XML_SafeFormatXMLText          ( /*  in */ PARS_XML_PT_internalNode a_ndeActual,
                                                      /*  bi */ PARS_LPSTR               a_szXMLText,
                                                      /*  in */ PARS_DWORD*              a_pdwXMLTextLen,
                                                      /*  in */ PARS_DWORD               a_dwLevel,
                                                      /*  in */ PARS_DWORD               a_dwDestBufLen,
                                                      /*  in */ PARS_XMLAsyncStruct*     a_pXMLAsyncStruct,
                                                      /*  in */ PARS_BOOL                a_bClosingTag);

PARS_RESULT     PARS_XML_MakeCopyString             ( /*  in */ PARS_LPSTR              a_szSource, 
                                                      /*  in */ PARS_DWORD              a_dwSourceLen,
                                                      /* out */ PARS_LPSTR*             a_ppszDestination,
                                                      /* out */ PARS_DWORD*             a_pdwDestinationLen);
                                                                                        
PARS_RESULT     PARS_XML_FindNextAttribute          ( /*  in */ XML_TEXT_POS            a_posActual,
                                                      /*  bi */ XML_TEXT_POS*           a_pposNext,
                                                      /* out */ XML_TEXT_POS*           a_pposAttrName,
                                                      /* out */ PARS_DWORD*             a_pdwAttrNameLen,
                                                      /* out */ XML_TEXT_POS*           a_pposAttrValue,
                                                      /* out */ PARS_DWORD*             a_pdwAttrValueLen);
                                                                                        
XML_TEXT_POS    PARS_XML_GetNextTextTag             ( /*  in */ PARS_XML_Node           a_ndeNode);

PARS_RESULT     PARS_XML_StripNameSpace             ( /*  in */ XML_TEXT_POS*           a_pposName,
                                                      /*  in */ PARS_DWORD*             a_pdwNameLen);

PARS_RESULT DLLCALL PARS_XML_GetNamespaceUri        ( /*  in */ PARS_XML_Node           a_ndeNode,
                                                      /*  in */ PARS_DWORD              a_dwMemFlags,
                                                      /*  in */ PARS_LPSTR              a_strNamespace,
                                                      /*  in */ PARS_DWORD              a_dwNamespaceLen,
                                                      /* out */ PARS_LPSTR*             a_pstrValue,
                                                      /* out */ PARS_LPDWORD            a_pdwLength);

PARS_LPSTR      PARS_XML_StripNameSpaceCopy         ( /*  in */ PARS_LPSTR              a_strName);

PARS_RESULT     PARS_XML_NewAttribute               ( /* out */ PARS_XML_PT_internalAttributes*    a_pattrValue );

PARS_RESULT     PARS_XML_DeleteAllNodes             ( /*  in */ PARS_XML_PT_internalNode    a_ndeNode);

PARS_RESULT     PARS_XML_DeleteAttr                 ( /*  in */ PARS_XML_PT_internalAttributes     a_attrAttributes);

PARS_RESULT     PARS_XML_DeleteAllAttr              ( /*  in */ PARS_XML_PT_internalAttributes     a_attrAttributes);

PARS_RESULT     PARS_XML_CreateSheetSync            ( /*  in */ PARS_XML_Sheet          a_shtXMLSheet);

PARS_RESULT     PARS_XML_DeleteSheetSync            ( /*  in */ PARS_XML_Sheet          a_shtXMLSheet);

PARS_RESULT     PARS_XML_PackXMLSheetHeader         ( /*  in */ PARS_XML_Sheet          a_shtXMLSheet,
                                                      /*  bi */ PARS_LPSTR              a_szXMLText,
                                                      /*  bi */ PARS_DWORD*             a_pdwXMLTextLen,
                                                      /*  in */ PARS_DWORD              a_dwBufSize,
                                                      /*  in */ PARS_XMLAsyncStruct*    a_pXMLAsyncStruct);

PARS_RESULT     PARS_XML_PackXMLSheetPI             ( /*  in */ PARS_XML_Sheet          a_shtXMLSheet,
                                                      /*  bi */ PARS_LPSTR              a_szXMLText,
                                                      /*  bi */ PARS_DWORD*             a_pdwXMLTextLen,
                                                      /*  in */ PARS_DWORD              a_dwBufSize,
                                                      /*  in */ PARS_XMLAsyncStruct*    a_pXMLAsyncStruct);

PARS_RESULT     PARS_XML_PackXMLNodes               ( /*  in */ PARS_XML_PT_internalNode    a_ndeNode,
                                                      /*  in */ PARS_DWORD                  a_dwActualDepth,
                                                      /*  bi */ PARS_LPSTR                  a_szXMLText,
                                                      /*  bi */ PARS_DWORD*                 a_pdwXMLTextLen,
                                                      /*  in */ PARS_DWORD                  a_dwBufSize,
                                                      /*  in */ PARS_XMLAsyncStruct*        a_pXMLAsyncStruct,
                                                      /*  in */ PARS_INT                    a_bWithNeighbors);

PARS_INT        PARS_XML_SAFE_strncmp               ( /*  in */ PARS_LPSTR              a_strString1,
                                                      /*  in */ PARS_LPSTR              a_strString2, 
                                                      /*  in */ PARS_DWORD              a_dwLength1,
                                                      /*  in */ PARS_DWORD              a_dwLength2);

PARS_INT        PARS_XML_SAFE_strnicmp              ( /*  in */ PARS_LPSTR              a_strString1,
                                                      /*  in */ PARS_LPSTR              a_strString2, 
                                                      /*  in */ PARS_DWORD              a_dwLength1,
                                                      /*  in */ PARS_DWORD              a_dwLength2);

PARS_RESULT DLLCALL PARS_XML_GetContent             ( /*  in */ PARS_XML_Node           a_ndeNode,
                                                      /*  in */ PARS_DWORD              a_dwMemFlags,
                                                      /* out */ PARS_LPSTR*             a_pstrValue,
                                                      /* out */ PARS_LPDWORD            a_pdwLength);

PARS_RESULT DLLCALL PARS_XML_GetNode                ( /*  in */ PARS_XML_Node           a_ndeNode,
                                                      /*  in */ PARS_DWORD              a_dwNavigationFlags,
                                                      /*  in */ PARS_DWORD              a_dwNodeType,
                                                      /* out */ PARS_XML_Node*          a_pndeValue);

PARS_RESULT DLLCALL PARS_XML_GetTagName             ( /*  in */ PARS_XML_Node           a_ndeNode,
                                                      /*  in */ PARS_DWORD              a_dwMemFlags,
                                                      /* out */ PARS_LPSTR*             a_pstrValue,
                                                      /* out */ PARS_LPDWORD            a_pdwLength);

PARS_RESULT DLLCALL PARS_XML_AddAttribute           ( /*  in */ PARS_XML_Node           a_ndeNode,
                                                      /*  in */ PARS_DWORD              a_dwMemFlags,
                                                      /*  in */ PARS_LPSTR              a_strName,
                                                      /*  in */ PARS_DWORD              a_dwNameLen,
                                                      /*  in */ PARS_LPSTR              a_strValue,
                                                      /*  in */ PARS_DWORD              a_dwValueLen);

PARS_RESULT DLLCALL PARS_XML_QueryAttributeByName   ( /*  in */ PARS_XML_Node           a_ndeNode, 
                                                      /*  in */ PARS_DWORD              a_dwMemFlags,
                                                      /*  in */ PARS_LPSTR              a_strAttributeName,
                                                      /*  in */ PARS_DWORD              a_dwAttributeNameLen,
                                                      /*  in */ PARS_BOOL               a_bIgnoreNamespaces,
                                                      /* out */ PARS_LPSTR*             a_pstrValue,
                                                      /* out */ PARS_LPDWORD            a_pdwLength);

PARS_RESULT DLLCALL PARS_XML_EscapeContent          ( /*  in */ PARS_LPSTR              a_szContent,
                                                      /*  in */ PARS_DWORD              a_dwContentLen,
                                                      /*  in */ PARS_DWORD              a_dwFlags,
                                                      /* out */ PARS_LPSTR*             a_pszOutput,
                                                      /* out */ PARS_LPDWORD            a_pdwOutputLen);

PARS_RESULT DLLCALL PARS_Generate_Rema_XML_Buffer (	PARS_PND_REMA_DATA_TYPE a_rema,
													PARS_LPSTR** remaData,
													PARS_DWORD* remaDataLength);
//TODO
PARS_VOID PARS_AllocateXmlBuffer();

/*----------------------------------------------------------------------------------------------------*\
|   XPath extensions                                                                XPath extensions   |
\*----------------------------------------------------------------------------------------------------*/
#if PARS_XML_ENABLE_XPATH
PARS_RESULT DLLCALL PARS_XML_XPath                  ( /*  in */ PARS_XML_Node           a_ndeStartNode,
                                                      /*  in */ PARS_LPSTR              a_strExpression,
                                                      /*  in */ PARS_DWORD              a_dwExpressionLen,
                                                      /*  in */ PARS_XPATH_VAR_CB       a_pVarRefCB,
                                                      /*  in */ PARS_LPVOID             a_pvCookie,
                                                      /*  in */ PARS_DWORD              a_dwExpectedNodeCount,
                                                      /* out */ PARS_PT_Value           a_valResult);

PARS_RESULT DLLCALL PARS_XML_XPathString            ( /*  in */ PARS_XML_Node           a_ndeStartNode,
                                                      /*  in */ PARS_LPSTR              a_strExpression,
                                                      /*  in */ PARS_DWORD              a_dwExpressionLen,
                                                      /* out */ PARS_LPSTR**            a_pastrValue);

PARS_RESULT DLLCALL PARS_XML_TestXPathPreparser     ( /*  in */ PARS_LPSTR              a_strExpression,
                                                      /*  in */ PARS_DWORD              a_dwExpressionLen,
                                                      /* out */ PARS_LPSTR              a_strOutbuffer,
                                                      /*  in */ PARS_DWORD              a_dwOutbufferLen);
#endif /* PARS_XML_ENABLE_XPATH */

/*----------------------------------------------------------------------------------------------------*\
|   SAX Extensions                                                                    SAX Extensions   |
\*----------------------------------------------------------------------------------------------------*/
#if PARS_XML_ENABLE_SXPARSER
PARS_RESULT  DLLCALL PARS_XML_SXFeedParser          ( /*  bi */ PARS_XML_SXPARSER_CTX*  a_ppContext,
                                                      /*  in */ PARS_LPSTR              a_szXMLString);
                                                                                        
                                                                                        
                                                                                        
PARS_RESULT  DLLCALL PARS_XML_SXGetNextFrag         ( /*  bi */ PARS_XML_SXPARSER_CTX   a_pContext,
                                                      /*  bi */ PARS_DWORD*             a_pdwFragType,
                                                      /* out */ PARS_LPVOID*            a_ppvBuffer,
                                                      /* out */ PARS_DWORD*             a_pdwBytes,
                                                      /* out */ PARS_BOOL*              a_pfFragComplete);
                                                                                        
PARS_RESULT  DLLCALL PARS_XML_SXQuitParser          ( /*  bi */ PARS_XML_SXPARSER_CTX*  a_ppContext);
#endif /* PARS_XML_ENABLE_SXPARSER */

/*----------------------------------------------------------------------------------------------------*\
|   XML Writer Extension                                                        XML Writer Extension   |
\*----------------------------------------------------------------------------------------------------*/
#if PARS_XML_ENABLE_XMLWRITER
PARS_RESULT DLLCALL PARS_XML_W_CtxtInit ( /*  in */    PARS_DWORD            a_dwNameLen,
										  /* out */    PARS_XML_WRITER_CTX*  a_ppCtxt    );
PARS_RESULT DLLCALL PARS_XML_W_CtxtDelete ( /*  bi */  PARS_XML_WRITER_CTX*  a_ppCtxt    );

PARS_RESULT DLLCALL PARS_XML_W_WriteTagStart(/*  in */  PARS_XML_WRITER_CTX a_pCtxt,
                                             /*  in */  PARS_BOOL           a_bHasAttributes,
                                             /*  in */  PARS_LPSTR      a_pmstrTagName );

PARS_RESULT DLLCALL PARS_XML_W_WriteTagEnd          ( /*  in */ PARS_XML_WRITER_CTX       a_pCtxt,
                                                      /*  in */ PARS_LPSTR            a_pmstrTagName);

PARS_RESULT DLLCALL PARS_XML_W_WriteTagValue (/*  in */  PARS_XML_WRITER_CTX  a_pCtxt,
											  /*  in */  PARS_LPSTR             a_strValue);

PARS_RESULT DLLCALL PARS_XML_W_WriteAttribute       ( /*  in */ PARS_XML_WRITER_CTX       a_pCtxt,
                                                      /*  in */ PARS_LPSTR            a_pmstrAttName,
                                                      /*  in */ PARS_LPSTR            a_pmstrValue);

PARS_RESULT DLLCALL PARS_XML_W_WriteComment  (  /*  in */   PARS_XML_WRITER_CTX   a_pCtxt,
                                     			/*  in */   PARS_LPSTR              a_strComment);
#endif /* PARS_XML_ENABLE_XMLWRITER */

/*----------------------------------------------------------------------------------------------------*\
|   DEBUG SUPPORT                                                                      DEBUG SUPPORT   |
\*----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------*\
|   SX Parser                                                                              SX Parser   |
\*----------------------------------------------------------------------------------------------------*/

/* SX Parser Specific Stuff */

#if PARS_XML_ENABLE_SXPARSER
 #if PARS_XML_VERBOSE_PARSING
  PARS_RESULT PARS_XML_SXReportCtx ( /*  bi */ PARS_XML_SXPARSER_CTX  a_pContext);
  #define PARS_XML_SX_TRACE(x,y)           PARS_TRACE(x,y)
  #define PARS_XML_SX_REPORT_CTX(x)        PARS_XML_SXReportCtx(x)
 #else /* PARS_XML_VERBOSE_PARSING */
  #define PARS_XML_SX_TRACE(x,y)
  #define PARS_XML_SX_REPORT_CTX(x)
 #endif /* PARS_XML_VERBOSE_PARSING */

/*----------------------------------------------------------------------------------------------------*\
|   Typedef Structs                                                                  Typedef Structs   |
\*----------------------------------------------------------------------------------------------------*/
typedef struct stSXCTX
{
    PARS_DWORD   dwFragType;
    PARS_DWORD   dwFragLength;
    PARS_DWORD   dwContentType;
    PARS_BOOL    fFragComplete;
    PARS_LPSTR   szActPos;
    PARS_LPSTR   szNextPos;
    PARS_LPSTR   szRetPos;
    PARS_CHAR    strBuffer[PARS_XML_SX_BUFFER_LEN];
} PARS_XML_SXCtx, *pSXCtx;
    
#endif /* PARS_XML_ENABLE_SXPARSER */

#endif /* __XMLPARSER_DEFINED__ */
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

