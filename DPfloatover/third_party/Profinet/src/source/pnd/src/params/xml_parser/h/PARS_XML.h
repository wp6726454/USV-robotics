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
/*  F i l e               &F: PARS_XML.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface definition of the XML Parser.                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef __XML_DEFINED__
#define __XML_DEFINED__

PARS_BEGIN_EXTERN_C

/*----------------------------------------------------------------------------------------------------*\
|   Error Callback Function                                                  Error Callback Function   |
\*----------------------------------------------------------------------------------------------------*/

typedef PARS_RESULT (DLLCALL *PARS_XML_GET_XML_TEXT_CB)(PARS_LPVOID a_pvXMLBuffer,
                                                        PARS_DWORD  a_dwBufferLen,
                                                        PARS_BOOL   a_bLastFrame,
                                                        PARS_LPVOID a_pvCookie);

#if PARS_XML_ENABLE_XPATH
typedef PARS_RESULT ( DLLCALL *PARS_XPATH_VAR_CB)   ( /*  in */ PARS_LPVOID     a_pCookie,
                                                      /*  in */ PARS_LPSTR      a_strVarName,
                                                      /*  in */ PARS_DWORD      a_dwVarNameLen,
                                                      /* out */ PARS_PT_Value   a_pResultValue);
#endif  /* PARS_XML_ENABLE_XPATH */

/*----------------------------------------------------------------------------------------------------*\
|   Data types                                                                            Data types   |
\*----------------------------------------------------------------------------------------------------*/
#if PARS_XML_ENABLE_XMLWRITER
/** Struct that holds the current write context */
typedef struct PARS_XML_S_WriteCtxt
{
    PARS_LPVOID     hFile;          /**< File handle used for export            */
    PARS_DWORD      dwIndent;       /**< Current indent depth for the file      */
    PARS_BOOL       bMustCloseTag;  /**< Indicates that we have an open tag     */
}PARS_XML_T_WriteCtxt, *PARS_XML_PT_WriteCtxt;
#endif /* PARS_XML_ENABLE_XMLWRITER */
#define PARS_XML_WRITER_CTX     PARS_LPVOID
#define PARS_XML_SXPARSER_CTX   PARS_LPVOID
/*----------------------------------------------------------------------------------------------------*\
|   Defines                                                                                  Defines   |
\*----------------------------------------------------------------------------------------------------*/

/*  SAX Parser                                                                            SAX Parser  */
#define PARS_XML_SX_BUFFER_LEN                                  PARS_MAX_URL_LEN

#define PARS_XML_SX_ALL_FRAG_TYPES                              0xffffffff
#define PARS_XML_SX_FRAG_TYPE_NONE                              0x00000000
#define PARS_XML_SX_FRAG_TYPE_START_TAG_NAME                    0x00000001
#define PARS_XML_SX_FRAG_TYPE_END_TAG_NAME                      0x00000002
#define PARS_XML_SX_FRAG_TYPE_END_EMPTY_TAG                     0x00000004
#define PARS_XML_SX_FRAG_TYPE_ATTRIBUTE_NAME                    0x00000008
#define PARS_XML_SX_FRAG_TYPE_ATTRIBUTE_VALUE                   0x00000010
#define PARS_XML_SX_FRAG_TYPE_COMMENT                           0x00000020
#define PARS_XML_SX_FRAG_TYPE_CDATA                             0x00000040
#define PARS_XML_SX_FRAG_TYPE_VERSION                           0x00000080
#define PARS_XML_SX_FRAG_TYPE_CONTENT                           0x00000100

#define PARS_XML_SX_FRAG_INCOMPLETE                             0x00
#define PARS_XML_SX_FRAG_COMPLETE                               0x01

#define PARS_XML_SX_CONTENT_NONE                                0x00
#define PARS_XML_SX_CONTENT_TAG                                 0x01
#define PARS_XML_SX_CONTENT_CDATA                               0x02
#define PARS_XML_SX_CONTENT_COMMENT                             0x03
#define PARS_XML_SX_CONTENT_TAG_ATTRIBUTE                       0x04
#define PARS_XML_SX_CONTENT_TAG_ATTRIBUTE_VALUE                 0x05
#define PARS_XML_SX_CONTENT_VERSION                             0x06

/*----------------------------------------------------------------------------------------------------*\
|   Magic characters                                                                Magic characters   |
\*----------------------------------------------------------------------------------------------------*/

#define cSTART_TAG                                      '<'
#define cSPECIAL_TAG_MARK                               '!'
#define cEND_TAG                                        '>'
#define cEND_TAG_MARK                                   '/'
#define cQUESTION_MARK                                  '?'
#define cCOMMENT_MARK                                   '-'
#define cATTRIBUTE_MARK                                 '='
#define cQUOTATION_MARK                                 '"'
#define END_EMPTY_TAG                                   "/>"
#define CDATA_START                                     "![CDATA["
#define CDATA_END                                       "]]>"
#define COMMENT_START                                   "!--"
#define COMMENT_END                                     "-->"
#define VERSION_START                                   "?xml"

/*----------------------------------------------------------------------------------------------------*\
|   Includes                                                                                Includes   |
\*----------------------------------------------------------------------------------------------------*/
#include "PARSP_XMLTags.h"                                           /* is in the Platform definition */

/*----------------------------------------------------------------------------------------------------*\
|   Document Handling                                                              Document Handling   |
\*----------------------------------------------------------------------------------------------------*/

/* XML Node types */
#define PARS_XML_NODE_TYPE_ELEMENT                0x0001        /* element node */
#define PARS_XML_NODE_TYPE_CDATA                  0x0002        /* CDATA node */
#define PARS_XML_NODE_TYPE_COMMENT                0x0004        /* comment node */
#define PARS_XML_NODE_TYPE_TEXT                   0x0008        /* text node (no child nodes) */

#define PARS_XML_NODE_TYPE_ALLNODES               0xFFFF

/* insertion flags */
#define PARS_XML_INSERT_PARENT                    0x0100   /* insert as my parent, if i have a parent this is the parent of the block to insert */
                                                           /* if the block has children insert these infront of me and my neighbours */
#define PARS_XML_INSERT_PREV                      0x0400   /* insert as my left neighbour */
#define PARS_XML_INSERT_NEXT                      0x0800   /* insert as my right neighbour */
#define PARS_XML_INSERT_FIRST_CHILD               0x1000   /* insert as my first child */
#define PARS_XML_INSERT_LAST_CHILD                0x2000   /* insert as my last child */

/* navigation flags */
#define PARS_XML_NAVIGATE_UP                      0x0001   /* up, my parent */
#define PARS_XML_NAVIGATE_DOWN                    0x0002   /* down, my children */
#define PARS_XML_NAVIGATE_PREV                    0x0004   /* left from me */
#define PARS_XML_NAVIGATE_NEXT                    0x0008   /* right from me */

#define PARS_XML_NAVIGATE_FIRST                   0x0010   /* search from first child of my parent */
#define PARS_XML_NAVIGATE_LAST                    0x0020   /* search from last child of my parent */
#define PARS_XML_NAVIGATE_ROOT                    0x0040   /* root node */
#define PARS_XML_DIRECTIONS                       0x007F   /* Mask for all directions */

#define PARS_XML_IGNORE_CASE                      0x0080   /* should search ignore case (VF 060705) */

/* memory flags */
#define PARS_XML_MEM_FLAG_NO_FLAG                 0x0000
#define PARS_XML_MEM_FLAG_COPY_ON_DEMAND          0x0001   /* make copy if necessary and return a string */
#define PARS_XML_MEM_FLAG_FORCE_ORIGINAL          0x0002   /* Don't use copy even if it exists (Advanced Feature) */
#define PARS_XML_MEM_FLAG_GIVE_A_COPY             0x0004   /* Copy my output, free outside */
#define PARS_XML_MEM_FLAG_MAKE_COPY               0x0008   /* Copy the given Value Parameter */
#define PARS_XML_MEM_FLAG_FORCE_RENEW             0x0010   /* recalculates the requested block and make Copy to return */

#define XML_C14N_NONE                             0x000000L /* No canonical XML                     */
#define XML_C14N_COMMENTS                         0x000001L /* canonical XML with comments          */
#define XML_C14N_NO_COMMENTS                      0x000002L /* canonical XML without comments       */

#define PARS_XML_USE_ORIGINAL_INDENT              0xffff    /**< Use Indent from input document                   */

/* Behavior flags for C14N */
#define PARS_XML_BEHAVIOR_NONE                    0x0000    /**< internal use, no special behavior is set */
#define PARS_XML_INHERIT_XMLNS                    0x0001    /**< internal use, only one node requested, inherit XMLNS */

/* Flags for EscapeContent */
#define     PARS_XML_FLTR_TEXT_NODE         1
#define     PARS_XML_FLTR_ATTRIBUTE_NODE    2
#define     PARS_XML_FLTR_CDATA             4
#define     PARS_XML_FLTR_ALL               PARS_XML_FLTR_CDATA|PARS_XML_FLTR_ATTRIBUTE_NODE|PARS_XML_FLTR_TEXT_NODE

/*----------------------------------------------------------------------------------------------------*\
|   XML_ServiceInterface Definition 2.2.01                    XML_ServiceInterface Definition 2.2.01   |
\*----------------------------------------------------------------------------------------------------*/
typedef struct XMLServiceInterface_
{
    /* Following is the Service vTable Header, every Service must have
       this functions / Members */
    PARS_DWORD        dwStructSize;
    PARS_RESULT    (DLLCALL *GetMMI)                    ( /* out */ PARS_LPVOID*    a_pMMI_IF);

    /* XML Parser Interface */

    /* Part 1 Generation and Management */
    PARS_RESULT    (DLLCALL *CreateSheet)               ( /*  in */ PARS_LPSTR          a_strXMLDocument,
                                                          /*  in */ PARS_BOOL           a_bFreeDocWhenDeleteSheet,
                                                          /*  in */ PARS_DWORD          a_dwDepth,
                                                          /* out */ PARS_XML_Sheet*     a_pshtXMLSheet,
                                                          /* out */ PARS_LPDWORD        a_pdwParsedLen);

    PARS_RESULT    (DLLCALL *GetSheetDoc)               ( /*  in */ PARS_XML_Sheet      a_shtXMLDocument,
                                                          /* out */ PARS_LPSTR*         a_pstrSheetDocument);

    PARS_RESULT    (DLLCALL *DeleteSheet)               ( /*  in */ PARS_XML_Sheet      a_shtXMLDocument);

    PARS_RESULT    (DLLCALL *CreateNode)                ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwInsertionFlags,
                                                          /*  in */ PARS_DWORD          a_dwNodeType,
                                                          /* out */ PARS_XML_Node*      a_pndeValue);

    PARS_RESULT    (DLLCALL *DeleteNode)                ( /*  in */ PARS_XML_Node       a_ndeNode);

    PARS_RESULT    (DLLCALL *ExpandNode)                ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwActualDepth,
                                                          /*  in */ PARS_DWORD          a_dwExpandDepth);

    PARS_RESULT    (DLLCALL *ExpandContent)             ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwActualDepth,
                                                          /*  in */ PARS_DWORD          a_dwExpandDepth);

    PARS_RESULT    (DLLCALL *AddAttribute)              ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strName,
                                                          /*  in */ PARS_DWORD          a_dwNameLen,
                                                          /*  in */ PARS_LPSTR          a_strValue,
                                                          /*  in */ PARS_DWORD          a_dwValueLen);

    PARS_RESULT    (DLLCALL *DeleteAttribute)           ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_LPSTR          a_strAttributeName,
                                                          /*  in */ PARS_DWORD          a_dwAttributeNameLen,
                                                          /*  in */ PARS_LPSTR          a_strAttributeValue,
                                                          /*  in */ PARS_DWORD          a_dwAttributeValue);

    PARS_RESULT    (DLLCALL *AddXMLSheetPI)             ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strPI);

    PARS_RESULT    (DLLCALL *DeleteXMLSheetPI)          ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_LPSTR          a_strPITarget,
                                                          /*  in */ PARS_DWORD          a_dwPITargetLen);

    PARS_RESULT    (DLLCALL *MoveNode)                  ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwInsertionFlags,
                                                          /*  in */ PARS_XML_Node       a_ndeDestinationNode);
    /* Part 2 navigation */

    PARS_XML_Sheet (DLLCALL *GetSheet)                  ( /*  in */ PARS_XML_Node       a_ndeNode);

    PARS_XML_Node  (DLLCALL *GetRootNode)               ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet);

    PARS_RESULT    (DLLCALL *GetNode)                   ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwNavigationFlags,
                                                          /*  in */ PARS_DWORD          a_dwNodeType,
                                                          /* out */ PARS_XML_Node*      a_pndeValue);

    PARS_RESULT    (DLLCALL *GetChildCount)             ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwNodeType,
                                                          /* out */ PARS_DWORD*         a_pdwChildCount);

    PARS_BOOL      (DLLCALL *IsFirstChild)              ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwNodeType);

    /* Part 3 data access (read)*/

    PARS_RESULT    (DLLCALL *GetTagName)                ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT    (DLLCALL *GetTagnameNamespace)       ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT    (DLLCALL *GetContent)                ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT    (DLLCALL *GetNamespaceUri)           ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strNamespace,
                                                          /*  in */ PARS_DWORD          a_dwNamespaceLen,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT    (DLLCALL *GetDTD)                    ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT   (DLLCALL *GetXMLSheetBom)             ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT   (DLLCALL *GetXMLSheetAttribute)       ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strAttributeName,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT   (DLLCALL *GetXMLSheetPI)              ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strPITarget,
                                                          /*  in */ PARS_DWORD          a_dwPITargetLen,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);
	PARS_RESULT    (DLLCALL *GetXMLText)                ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
														  /*  in */ PARS_DWORD          a_dwC14NForm,
                                                          /*  in */ PARS_DWORD          a_dwIndent,
                                                          /*  bi */ PARS_LPSTR          a_strXMLText,
                                                          /*  bi */ PARS_DWORD*         a_pdwXMLTextLen );

	
    PARS_RESULT    (DLLCALL *GetXMLTextAsync)           ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  bi */ PARS_LPSTR          a_strXMLText,
                                                          /*  bi */ PARS_DWORD*         a_pdwXMLTextLen,
                                                          /*  in */ PARS_DWORD          a_dwC14NForm,
                                                          /*  in */ PARS_DWORD          a_dwIndent,
                                                          /*  in */ PARS_XML_GET_XML_TEXT_CB a_cbfGetXMLText,
                                                          /*  in */ PARS_LPVOID         a_pCookie);

    PARS_RESULT    (DLLCALL *GetNodeTextAsync)          ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  bi */ PARS_LPSTR          a_strXMLText,
                                                          /*  bi */ PARS_DWORD*         a_pdwXMLTextLen,
                                                          /*  in */ PARS_DWORD          a_dwC14NForm,
                                                          /*  in */ PARS_DWORD          a_dwIndent,
                                                          /*  in */ PARS_XML_GET_XML_TEXT_CB a_cbfGetXMLText,
                                                          /*  in */ PARS_LPVOID         a_pCookie);
    /* Part 4  data access (write) */

	PARS_RESULT    (DLLCALL *SetTagName)                ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR		a_pmstrName );

    PARS_RESULT    (DLLCALL *SetContent)                ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strValue,
                                                          /*  in */ PARS_DWORD          a_dwValueLen);

    PARS_RESULT    (DLLCALL *SetDTD)                    ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strValue);

    PARS_RESULT   (DLLCALL *SetXMLSheetBom)             ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_LPSTR          a_strValue);

    PARS_RESULT   (DLLCALL *SetXMLSheetAttribute)       ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                          /*  in */ PARS_LPSTR          a_strAttributeName,
                                                          /*  in */ PARS_LPSTR          a_strValue);

    /* Part 5 data Mining / searching */

    PARS_RESULT    (DLLCALL *QueryAttributeByName)      ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strAttributeName,
                                                          /*  in */ PARS_DWORD          a_dwAttributeNameLen,
                                                          /*  in */ PARS_BOOL           a_bIgnoreNamespace,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT   (DLLCALL *QueryAttributeByValue)      ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwMemFlags,
                                                          /*  in */ PARS_LPSTR          a_strAttributeValue,
                                                          /*  in */ PARS_DWORD          a_dwAttributeValueLen,
                                                          /* out */ PARS_LPSTR*         a_pstrValue,
                                                          /* out */ PARS_LPDWORD        a_pdwLength);

    PARS_RESULT    (DLLCALL *FindNodeByTagname)         ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwNavigationFlags,
                                                          /*  in */ PARS_LPSTR          a_strTagName,
                                                          /*  in */ PARS_DWORD          a_dwTagNameLen,
                                                          /* out */ PARS_XML_Node*      a_pndeValue);

    PARS_RESULT    (DLLCALL *FindNodeByAttribute)       ( /*  in */ PARS_XML_Node       a_ndeNode,
                                                          /*  in */ PARS_DWORD          a_dwNavigationFlags,
                                                          /*  in */ PARS_LPSTR          a_strAttributeName,
                                                          /*  in */ PARS_DWORD          a_dwAttributeName,
                                                          /*  in */ PARS_LPSTR          a_strAttributeValue,
                                                          /*  in */ PARS_DWORD          a_dwAttributeValue,
                                                          /* out */ PARS_XML_Node*      a_pndeValue);

    /* Part 6 Synchronization */

    PARS_RESULT     (DLLCALL *EnterSheet)               ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet);
    PARS_RESULT     (DLLCALL *LeaveSheet)               ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet);

    /* Part 7: conversion and helper */

    PARS_RESULT     (DLLCALL *GetRealContent)           ( /*  bi */ PARS_LPSTR          a_strLine);

    PARS_RESULT     (DLLCALL *EscapeContent)            ( /*  in */ PARS_LPSTR          a_szContent,
                                                          /*  in */ PARS_DWORD          a_dwContentLen,
                                                          /*  in */ PARS_DWORD          a_dwFlags,
                                                          /* out */ PARS_LPSTR*         a_pszOutput,
                                                          /* out */ PARS_LPDWORD        a_pdwOutputLen);

    /* Part 8: SAX extension */

#if PARS_XML_ENABLE_SXPARSER

    PARS_RESULT    (DLLCALL *SXFeedParser)              ( /*  bi */ PARS_XML_SXPARSER_CTX*  a_ppContext,
                                                          /*  in */ PARS_LPSTR              a_szXMLString);

    PARS_RESULT    (DLLCALL *SXGetNextFrag)               ( /*  bi */ PARS_XML_SXPARSER_CTX   a_pContext,
                                                            /*  bi */ PARS_LPDWORD            a_pdwFragType,
                                                            /* out */ PARS_LPVOID*            a_ppvBuffer,
                                                            /* out */ PARS_LPDWORD            a_pdwBytes,
                                                            /* out */ PARS_LPBOOL             a_pfFragComplete);

    PARS_RESULT    (DLLCALL *SXQuitParser)                ( /*  bi */ PARS_XML_SXPARSER_CTX*  a_ppContext);

#endif  /* PARS_XML_ENABLE_SXPARSER    */
#if PARS_XML_ENABLE_XMLWRITER
    PARS_RESULT (DLLCALL * InitWriteCtxt)                 ( /* in */ PARS_DWORD              a_dwNameLen,
                                                            /* out */ PARS_XML_WRITER_CTX*  a_ppCtxt    );

    PARS_RESULT (DLLCALL *DeleteWriteCtxt)                ( /*  bi */ PARS_XML_WRITER_CTX*  a_ppCtxt    );


	PARS_RESULT (DLLCALL *WriteTagStart)                  ( /*  in */ PARS_XML_WRITER_CTX  a_pCtxt,
                                                            /*  in */ PARS_BOOL			   a_bHasAttributes,
                                                            /*  in */ PARS_LPSTR		   a_strTagName);


	PARS_RESULT (DLLCALL *WriteTagEnd)                    ( /*  in */ PARS_XML_WRITER_CTX  a_pCtxt,
                                                            /*  in */ PARS_LPSTR		   a_strTagName);


    PARS_RESULT (DLLCALL *WriteTagValue)                  ( /*  in */ PARS_XML_WRITER_CTX  a_pCtxt,
                                                            /*  in */ PARS_LPSTR           a_pmstrValue);
                                                            
    PARS_RESULT (DLLCALL *WriteAttribute)                 ( /* in */ PARS_XML_WRITER_CTX   a_pCtxt,
                                                            /*  in */ PARS_LPSTR		     a_strAttName,
                                                            /*  in */ PARS_LPSTR             a_strValue);
                                                            
    PARS_RESULT (DLLCALL *WriteComment)                   (  /* in */ PARS_XML_WRITER_CTX   a_pCtxt,
															 /* in */ PARS_LPSTR        a_pmstrComment );

	PARS_RESULT (DLLCALL *Generate_Rema_XML_Buffer)       ( PARS_PND_REMA_DATA_TYPE a_rema,
															PARS_LPSTR** remaData,
															PARS_DWORD* remaDataLength);
	
#endif /* PARS_XML_ENABLE_XMLWRITER */
#if PARS_XML_ENABLE_XPATH
    PARS_RESULT    (DLLCALL *XPath)                       ( /*  in */ PARS_XML_Node           a_ndeStartNode,
                                                            /*  in */ PARS_LPSTR              a_strExpression,
                                                            /*  in */ PARS_DWORD              a_dwExpressionLen,
                                                            /*  in */ PARS_XPATH_VAR_CB       a_pVarRefCB,
                                                            /*  in */ PARS_LPVOID             a_pvCookie,
                                                            /*  in */ PARS_DWORD              a_dwExpectedNodeCount,
                                                            /* out */ PARS_PT_Value           a_valResult);

    PARS_RESULT    (DLLCALL *XPathString)           ( /*  in */ PARS_XML_Node           a_ndeStartNode,
                                                      /*  in */ PARS_LPSTR              a_strExpression,
                                                      /*  in */ PARS_DWORD              a_dwExpressionLen,
                                                      /* out */ PARS_LPSTR**            a_pastrValue);

    PARS_RESULT    (DLLCALL *TestXPathPreparser)    ( /*  in */ PARS_LPSTR              a_strExpression,
                                                      /*  in */ PARS_DWORD              a_dwExpressionLen,
                                                      /* out */ PARS_LPSTR              a_strOutbuffer,
                                                      /*  in */ PARS_DWORD              a_dwOutbufferLen);
#endif /* PARS_XML_ENABLE_XPATH */
} XMLServiceInterface;
/* END XMLServiceInterface Definition 2.2.01  */

/*----------------------------------------------------------------------------------------------------*\
|   Prototypes                                                                            Prototypes   |
\*----------------------------------------------------------------------------------------------------*/

PARS_RESULT DLLCALL PARS_XML_InitializeXMLService   ( /*  in */ PARS_LPSTR      a_strServiceName,
                                                      /*  in */ PARS_LPVOID     a_pContext,
                                                      /*  in */ PARS_HDLLMODULE a_hModuleHandle);

typedef PARS_LPVOID  (DLLCALL* XML_T_SMemMalloc)    ( /*  in */ PARS_DWORD              a_dwSize
#if PARS_MEMORY_TRACE
                                                    ,/*  in */ PARS_INT       a_nLineNumber,
                                                     /*  in */ PARS_LPSTR     a_strFileName
#endif  /* PARS_MEMORY_TRACE */
                                                     );

typedef PARS_VOID    (DLLCALL* XML_T_SMemFree)      ( /*  in */ PARS_LPVOID             a_pMemoryBlock
#if PARS_MEMORY_TRACE
                                                    ,/*  in */ PARS_INT    a_nLineNumber,
                                                     /*  in */ PARS_LPSTR  a_strFileName
#endif  /* PARS_MEMORY_TRACE */
                                                     );

typedef PARS_LPVOID  (DLLCALL* XML_T_extern_SMemMalloc)( /*  in */ PARS_DWORD              a_dwSize
#if PARS_MEMORY_TRACE
                                    ,/*  in */ PARS_INT       a_nLineNumber,
                                     /*  in */ PARS_LPSTR     a_strFileName
#endif  /* PARS_MEMORY_TRACE */
                                                        );

typedef PARS_VOID    (DLLCALL* XML_T_extern_SMemFree)( /*  in */ PARS_LPVOID               a_pMemoryBlock
#if PARS_MEMORY_TRACE
                                    ,/*  in */ PARS_INT       a_nLineNumber,
                                     /*  in */ PARS_LPSTR     a_strFileName
#endif  /* PARS_MEMORY_TRACE */
                                                      );
typedef PARS_RESULT (*PARS_XML_CB_InitializeStandaloneXMLService)( /*  in */ PARS_LPVOID            a_pMemService,
                                                                   /*  in */ PARS_LPVOID            a_pStringService,
                                                                   /* out */ XMLServiceInterface**  a_ppXMLServiceIF);

// MG - added since it was exported as DLL in the original source code.
PARS_RESULT  PARS_XML_InitializeStandaloneXMLService( /*  in */ XML_T_extern_SMemMalloc      a_fpMalloc,
                                                                             /*  in */ XML_T_extern_SMemFree        a_fpFree,
                                                                             /* out */ XMLServiceInterface**        a_ppXMLServiceIF);

PARS_END_EXTERN_C

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

